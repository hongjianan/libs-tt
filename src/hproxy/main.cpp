/*
 * main.cpp
 *
 *  
 *
 *  Created on: 2018年1月7日
 *      Author: Hong
 */


#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/util.h>
#include <event2/event.h>

#include "log.h"
#include "traffic_stat.h"
#include "proxy_cfg.h"
#include "tunnel_mgr.h"
#include "tunnel.h"

static const int TUNNEL_PORT = 12345;
struct event_base*  g_evbase;
TrafficStat* g_trafficStat;

static struct event* create_timer(struct event_base *base, int period);
static void timer_cb(evutil_socket_t fd, short event, void *arg);
static void signal_cb(evutil_socket_t, short, void *);

int main(int argc, const char* argv[])
{
    int server_port = TUNNEL_PORT;
    if (argc == 2)
    {
        server_port = atoi(argv[1]);
    }

    cfg_set_proxy_port(&g_cfg, server_port);

    struct evconnlistener *listener;
    struct event *signal_event;

    struct sockaddr_in sin;

    /* init stat */
    struct timeval tv;
    evutil_gettimeofday(&tv, NULL);
    g_trafficStat = new TrafficStat(tv);

    g_evbase = event_base_new();
    if (!g_evbase) {
        LOG_ERR("Could not initialize libevent!\n");
        return -1;
    }

    struct event* timer = create_timer(g_evbase, 60);

    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(cfg_get_proxy_port(&g_cfg));

    listener = evconnlistener_new_bind(g_evbase, tm_listener_cb, (void *)g_evbase,
        LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE, -1,
        (struct sockaddr*)&sin, sizeof(sin));

    if (!listener) {
        LOG_ERR("Could not create a listener!\n");
        return -1;
    }

    signal_event = evsignal_new(g_evbase, SIGINT, signal_cb, (void *)g_evbase);

    if (!signal_event || event_add(signal_event, NULL) < 0) {
        LOG_ERR("Could not create/add a signal event!\n");
        return 1;
    }

    event_base_dispatch(g_evbase);

    evconnlistener_free(listener);
    event_free(signal_event);
    event_base_free(g_evbase);

    printf("proxy done\n");

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

static void timer_cb(evutil_socket_t fd, short event, void *arg)
{
    static int cnt = 0;
    g_trafficStat->output_period(KBPS);

    if (cnt++ >= 11) {
        cnt = 0;
        g_trafficStat->output_total();
    }
}


static void signal_cb(evutil_socket_t sig, short event, void *user_data)
{
    struct event_base *base = (struct event_base *)user_data;
    struct timeval delay = { 1, 0 };

    printf("event:%x Caught an interrupt signal; exiting cleanly in one seconds.\n", event);

    event_base_loopexit(base, &delay);
}


