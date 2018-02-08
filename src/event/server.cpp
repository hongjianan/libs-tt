/*
  This example program provides a trivial server program that listens for TCP
  connections on port 9995.  When they arrive, it writes a short message to
  each client connection, and closes each connection once it is flushed.

  Where possible, it exits cleanly in response to a SIGINT (ctrl-c).
*/


#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <assert.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/util.h>
#include <event2/event.h>

#include "traffic_stat.h"


static int g_server_port = 12345;
static TrafficStat *g_trafficStat;
static int             g_ratio;
static struct ev_token_bucket_cfg* g_rate_cfg;
static struct bufferevent_rate_limit_group *g_rate_group;

static struct event* create_timer(struct event_base *base, int period);
static int create_limit_group(struct event_base *base, int ratio, struct ev_token_bucket_cfg **cfg,
        struct bufferevent_rate_limit_group **group);
static void timer_cb(evutil_socket_t fd, short event, void *arg);
static void listener_cb(struct evconnlistener *, evutil_socket_t, struct sockaddr *, int socklen, void *);
static void conn_readcb(struct bufferevent *, void *);
static void conn_writecb(struct bufferevent *, void *);
static void conn_eventcb(struct bufferevent *, short, void *);
static void signal_cb(evutil_socket_t, short, void *);

int ServerTest_server(int argc, const char* argv[])
{
    if (3 != argc) {
        printf("usage: port ratio_KPBS\n");
        return -1;
    }

    g_server_port   = atoi(argv[1]);
    g_ratio         = atoi(argv[2]);

	struct event_base *base;
	struct evconnlistener *listener;
	struct event *signal_event;

	struct sockaddr_in sin;

	/* init stat */
	struct timeval tv;
	evutil_gettimeofday(&tv, NULL);
	g_trafficStat = new TrafficStat(tv);

	base = event_base_new();
	if (!base) {
		fprintf(stderr, "Could not initialize libevent!\n");
		return 1;
	}

	struct event* timer = create_timer(base, 5);

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(g_server_port);

	listener = evconnlistener_new_bind(base, listener_cb, (void *)base,
	    LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE, -1,
	    (struct sockaddr*)&sin, sizeof(sin));

	if (!listener) {
		fprintf(stderr, "Could not create a listener!\n");
		return 1;
	}

	signal_event = evsignal_new(base, SIGINT, signal_cb, (void *)base);

	if (!signal_event || event_add(signal_event, NULL) < 0) {
		fprintf(stderr, "Could not create/add a signal event!\n");
		return 1;
	}

	create_limit_group(base, g_ratio, &g_rate_cfg, &g_rate_group);

	event_base_dispatch(base);

	evconnlistener_free(listener);
	event_free(signal_event);
	event_base_free(base);

	printf("done\n");

	return 0;
}


static struct event* create_timer(struct event_base *base, int period)
{
	struct event* timer = event_new(base, -1, EV_PERSIST, timer_cb, NULL);

	struct timeval tv;
	evutil_timerclear(&tv);
	tv.tv_sec = period;
	event_add(timer, &tv);

	return timer;
}

static int create_limit_group(struct event_base *base, int ratio, struct ev_token_bucket_cfg **cfg,
        struct bufferevent_rate_limit_group **group)
{
    *cfg = ev_token_bucket_cfg_new(ratio, ratio * 4, ratio, ratio * 4, NULL);
    *group = bufferevent_rate_limit_group_new(base, *cfg);
    if (*cfg == NULL || *group == NULL) {
        printf("create_limit_group error\n");
        return -1;
    }
    bufferevent_rate_limit_group_set_min_share(*group, 1500);

    return 0;
}


static void timer_cb(evutil_socket_t fd, short event, void *arg)
{
	static int cnt = 0;
	g_trafficStat->output_period(KBPS);

	if (cnt++ >= 11) {
		cnt = 0;
		g_trafficStat->output_total();
	}
}

static void listener_cb(struct evconnlistener *listener, evutil_socket_t fd,
    					struct sockaddr *sa, int socklen, void *user_data)
{
	struct event_base *base = (struct event_base *)user_data;
	struct bufferevent *bev;

	struct sockaddr_in *addr = (struct sockaddr_in *)sa;
	printf("client connected: %s:%u\n", inet_ntoa(addr->sin_addr), addr->sin_port);

	bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
	if (!bev) {
		fprintf(stderr, "Error constructing bufferevent!");
		event_base_loopbreak(base);
		return;
	}
	bufferevent_add_to_rate_limit_group(bev, g_rate_group);

	bufferevent_setcb(bev, conn_readcb, conn_writecb, conn_eventcb, NULL);
	// bufferevent_enable(bev, EV_WRITE);
	bufferevent_enable(bev, EV_READ);

}

static void conn_readcb(struct bufferevent *bev, void *user_data)
{
	struct evbuffer *input = bufferevent_get_input(bev);

	int length = evbuffer_get_length(input);
	g_trafficStat->rx_bytes_period += length;
	// printf("read length:%d\n", length);

	char buffer[4096];
	while (length > 0) {
		int size = bufferevent_read(bev, buffer, 4096);
		length -= 4096;
	}
	
}


static void conn_writecb(struct bufferevent *bev, void *user_data)
{
}

static void conn_eventcb(struct bufferevent *bev, short events, void *user_data)
{
	if (events & BEV_EVENT_EOF) {
		printf("Connection closed.\n");
	} else if (events & BEV_EVENT_ERROR) {
		printf("Got an error on the connection: %s\n", strerror(errno));
	}
	/* None of the other events can happen here, since we haven't enabled
	 * timeouts */
	bufferevent_free(bev);
}

static void signal_cb(evutil_socket_t sig, short event, void *user_data)
{
	struct event_base *base = (struct event_base *)user_data;
	struct timeval delay = { 1, 0 };

	printf("event:%x Caught an interrupt signal; exiting cleanly in one seconds.\n", event);

	event_base_loopexit(base, &delay);
}


int ServerTest(int argc, const char* argv[])
{
	ServerTest_server(argc, argv);

    return 0;
}
