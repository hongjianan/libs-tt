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

#include "log.h"
#include "traffic_stat.h"
#include "tunnel_client.h"

static const int TUNNEL_PORT = 12345;
struct event_base*  g_evbase;
TrafficStat* g_trafficStat;
uint8_t*        g_message;
int             g_length;
int             g_packets_pers;
int             g_ratio;
char*           g_server_ip;
uint16_t        g_server_port;
char*           g_proxy_ip;
uint16_t        g_proxy_port;
static struct bufferevent* g_client;
static struct ev_token_bucket_cfg* g_rate_cfg;

static uint8_t* build_message(int length);
static struct event* create_timer(struct event_base *base, int period);
static struct bufferevent* create_client(struct event_base *base, const char* ip, uint16_t port);
static void timer_cb(evutil_socket_t fd, short event, void *arg);
static void signal_cb(evutil_socket_t, short, void *);

int main(int argc, char* argv[])
{
    if (7 != argc) {
        fprintf(stderr, "usage: ip port proxy_ip proxy_port length packets_pers\n");
        return -1;
    }
    char* server_ip = argv[1];
    int   server_port = atoi(argv[2]);
    g_proxy_ip = argv[3];
    g_proxy_port = atoi(argv[4]);
    g_length = atoi(argv[5]);
    g_packets_pers = atoi(argv[6]);
    g_ratio = g_length * g_packets_pers;

    printf("argc: %s:%d %s:%d pack_len:%d rate:%fKBS\n",
        server_ip, server_port, g_proxy_ip, g_proxy_port, g_length, g_length * g_packets_pers / 1024);

    /* init stat */
    struct timeval tv;
    evutil_gettimeofday(&tv, NULL);
    g_trafficStat = new TrafficStat(tv);

    g_evbase = event_base_new();
    if (!g_evbase) {
        fprintf(stderr, "Could not initialize libevent!\n");
        return 1;
    }

    g_message = build_message(g_length);

    struct event* timer = create_timer(g_evbase, 10);    // period 5s

    // create_client
    g_client = create_client(g_evbase, g_proxy_ip, g_proxy_port);

    // set rate
    g_rate_cfg = ev_token_bucket_cfg_new(g_ratio, g_ratio * 4, g_ratio, g_ratio * 4, NULL);
    bufferevent_set_rate_limit(g_client, g_rate_cfg);

    event_base_dispatch(g_evbase);
    event_base_free(g_evbase);

    printf("client done\n");

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

static struct bufferevent* create_client(struct event_base *base, const char* ip, uint16_t port)
{
    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    inet_aton(ip, &sin.sin_addr);
    sin.sin_port = htons(port);

    struct bufferevent *bev = bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE);
    bufferevent_setcb(bev, tc_conn_readcb, tc_conn_writecb, tc_conn_eventcb, NULL);
    
    if (0 < bufferevent_socket_connect(bev, (struct sockaddr *)&sin, sizeof(sin))) {
        bufferevent_free(bev);
        return NULL;
    }

    return bev;
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

