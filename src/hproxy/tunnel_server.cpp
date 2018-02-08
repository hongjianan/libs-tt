
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/util.h>
#include <event2/event.h>

#include "log.h"
#include "traffic_stat.h"
#include "proto.h"
#include "header.h"
#include "proxy_cfg.h"
#include "tunnel_mgr.h"
#include "tunnel.h"
#include "tunnel_server.h"

extern TrafficStat* g_trafficStat;
extern struct event_base*  g_evbase;

void ts_conn_readcb(struct bufferevent *bev, void *arg)
{
    LOG_DBG("ts_conn_readcb bev:%p arg:%p\n", bev, arg);

    struct tunnel* tunnel = (struct tunnel*)arg;
    struct evbuffer *ebuf = bufferevent_get_input(bev);
    switch (tunnel->status)
    {
    case TUNNEL_INIT:
        break;
    case TUNNEL_CLIENT_BUILD_OK:
        break;

    default:
        LOG_ERR("conn_readcb error status:%d\n", tunnel->status);
        break;
    }
    
    // g_trafficStat->rx_bytes_period += length;
}


void ts_conn_writecb(struct bufferevent *bev, void *arg)
{
}

void ts_conn_eventcb(struct bufferevent *bev, short events, void *arg)
{
    struct tunnel* tunnel = (struct tunnel*)arg;

    if (events & BEV_EVENT_CONNECTED) {
        LOG_INF("Connection server success.\n");
        tunnel->status = TUNNEL_SERVER_BUILD_OK;

		struct setup_tunnel_rsp rsp;
        struct message_header header;
		header.length = sizeof(header) + sizeof(rsp);
		header.magic = HEADER_MAGIC;
		header.type = setup_tunnel_rsp_pid;
		rsp.ret_code = 0x11001100;

		uint8_t msg[1024];
		memcpy(msg, &header, sizeof(header));
		memcpy(msg + sizeof(header), &rsp, sizeof(rsp));

		bufferevent_write(tunnel->bev[TUNNEL_CLIENT_IDX], msg, header.length);
		tunnel->status = TUNNEL_COMPLETED_OK;
		g_trafficStat->tx_bytes_period += header.length;

        return;
    } else if (events & BEV_EVENT_EOF) {
        LOG_INF("Connection closed.\n");
    } else if (events & BEV_EVENT_ERROR) {
        LOG_WAR("Got an error on the connection: %s\n", strerror(errno));
    }

    // TODO:: don't cut client connection
    LOG_WAR("release tunnel:%p\n", tunnel);

    bufferevent_free(bev);
    bufferevent_free(tunnel->bev[0]);
    /* None of the other events can happen here, since we haven't enabled
     * timeouts */
    g_tunnel_mgr.tunnels.erase(tunnel->bev[0]);
    free(tunnel);
}

