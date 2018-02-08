
#include "htest/htest.h"

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

#include "zlib_filter.h"
#include "traffic_stat.h"
#include "ratelim-internal.h"


static TrafficStat* g_trafficStat;
static uint8_t*     g_message;
static int          g_length;
static int          g_packets_pers;
static int          g_ratio;
static int          g_zlib_flag = 1;
static struct bufferevent* g_client;
static struct ev_token_bucket_cfg* g_rate_cfg;


static uint8_t* build_message(int length);
static void send_message(struct bufferevent *bev);
static struct event* create_timer(struct event_base *base, int period);
static struct bufferevent* create_client(struct event_base *base, const char* ip, uint16_t port);
static void timer_cb(evutil_socket_t fd, short event, void *arg);
static void conn_readcb(struct bufferevent *, void *);
static void conn_writecb(struct bufferevent *, void *);
static void conn_eventcb(struct bufferevent *, short, void *);

int ZlibClientTest_client(int argc, const char* argv[])
{
    if (6 != argc) {
        fprintf(stderr, "usage: ip port length packets_pers zlib(0/1)\n");
        return -1;
    }
    const char* server_ip = argv[1];
    int   server_port = atoi(argv[2]);
    g_length = atoi(argv[3]);
    g_packets_pers = atoi(argv[4]);
    g_zlib_flag = atoi(argv[5]);
    g_ratio = g_length * g_packets_pers;

    printf("argc: %s %d %d %fKBS\n", server_ip, server_port, g_length, 1.0 * g_ratio / KBPS);

    /* init stat */
    struct timeval tv;
    evutil_gettimeofday(&tv, NULL);
    g_trafficStat = new TrafficStat(tv);

    struct event_base *base = event_base_new();
    if (!base) {
        fprintf(stderr, "Could not initialize libevent!\n");
        return 1;
    }

    g_message = build_message(g_length);

    struct event* timer = create_timer(base, 5);    // period 5s

    // create_client
    g_client = create_client(base, server_ip, server_port);

    // set rate
    g_rate_cfg = ev_token_bucket_cfg_new(g_ratio, g_ratio * 4, g_ratio, g_ratio * 4, NULL);
    bufferevent_set_rate_limit(g_client, g_rate_cfg);

    event_base_dispatch(base);
    event_base_free(base);

    printf("client done\n");

    return 0;
}


static uint8_t* build_message(int length)
{
    uint8_t* buffer = (uint8_t*)malloc(length * sizeof(uint8_t));

    uint8_t num = '0';
    for (int i = 0; i < length; ++i) {
        buffer[i] = num++;
        if ('9' == num) {
            num = '0';
        }
    }

    return buffer;
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

static struct bufferevent* create_client(struct event_base *base, const char* ip, uint16_t port)
{
    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    inet_aton(ip, &sin.sin_addr);
    sin.sin_port = htons(port);

    struct bufferevent *bev = bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE);

    bufferevent_setcb(bev, conn_readcb, conn_writecb, conn_eventcb, NULL);
    
    if (0 < bufferevent_socket_connect(bev, (struct sockaddr *)&sin, sizeof(sin))) {
        bufferevent_free(bev);
        return NULL;
    }

    return bev;
}

static void conn_readcb(struct bufferevent *bev, void *user_data)
{
}

static void send_message(struct bufferevent *bev)
{
    bufferevent_write(bev, g_message, g_length);
    g_trafficStat->tx_bytes_period += g_length;
    g_trafficStat->tx_packets_period += 1;
}

static void conn_writecb(struct bufferevent *bev, void *user_data)
{
    struct evbuffer *output = bufferevent_get_output(bev);
    if (evbuffer_get_length(output) == 0) {
        printf("write complete.\n");
        send_message(bev);
    } else {
        //printf("write left:%d\n", evbuffer_get_length(output));
    }
}

static void conn_eventcb(struct bufferevent *bev, short events, void *user_data)
{
    if (events & BEV_EVENT_CONNECTED) {
        printf("Connection success. Start to send message\n");

        if (g_zlib_flag) {
            printf("zlib open\n");
            z_stream* z_output = (z_stream*)calloc(sizeof(*z_output), 1);
            if (deflateInit(z_output, Z_DEFAULT_COMPRESSION)) {
                printf("deflateInit error\n");
                return;
            }

            bev = bufferevent_filter_new(bev, NULL, zlib_output_filter,
                    BEV_OPT_CLOSE_ON_FREE, zlib_deflate_free, z_output);
        }

        send_message(bev);
        return;
    } else if (events & BEV_EVENT_EOF) {
        printf("Connection closed.\n");
    } else if (events & BEV_EVENT_ERROR) {
        printf("Got an error on the connection: %s\n", strerror(errno));
    }

    bufferevent_free(bev);

    exit(-1);
}

static void timer_cb(evutil_socket_t fd, short event, void *arg)
{
    static int cnt = 0;
    g_trafficStat->output_period(KBPS);

    if (cnt++ >= 11) {
        cnt = 0;
        g_trafficStat->output_total();
    }

    // change rate
    // reset_rate_cfg((struct ev_token_bucket_cfg_*)g_rate_cfg, g_ratio * (cnt + 1), g_ratio * (cnt + 1) * 4,
    //              g_ratio * (cnt + 1), g_ratio * (cnt + 1) * 4, NULL);

    // bufferevent_set_rate_limit(g_client, g_rate_cfg);
}

DEFINE_TEST(ZlibClientTest)
{
    return ZlibClientTest_client(argc, argv);
}
