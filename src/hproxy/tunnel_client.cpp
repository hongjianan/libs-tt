
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
#include "header.h"
#include "traffic_stat.h"
#include "proxy_cfg.h"
#include "tunnel.h"
#include "tunnel_mgr.h"
#include "tunnel_client.h"

extern TrafficStat* g_trafficStat;
extern struct event_base*  g_evbase;

void tc_conn_readcb(struct bufferevent *bev, void *arg)
{
    LOG_DBG("conn_readcb bev:%p arg:%p\n", bev, arg);

    struct tunnel* tunnel = (struct tunnel*)arg;
    struct evbuffer *ebuf = bufferevent_get_input(bev);
    int buf_len, pack_len;
    switch (tunnel->status)
    {
    case TUNNEL_INIT:
        buf_len = evbuffer_get_length(ebuf);
        evbuffer_copyout(ebuf, &pack_len, sizeof(pack_len));
        if (pack_len > buf_len) {
            LOG_DBG("message uncomplete pack_len:%d buf_len:%d\n", pack_len, buf_len);
            return;
        }
        struct message_header header;
        evbuffer_remove(ebuf, &header, sizeof(header));

        switch (header.type)
        {
        case setup_tunnel_req_pid:
            struct setup_tunnel_req req;
            struct setup_tunnel_rsp rsp;
            evbuffer_remove(ebuf, &req, sizeof(req));
            handler_setup_tunnel_req(tunnel, &req, &rsp);
            bufferevent_write(bev, &rsp, sizeof(rsp));
            g_trafficStat->rx_bytes_period += sizeof(rsp);
            break;

        default:
            LOG_ERR("tunnel_mgr::conn_readcb error message type:%d\n", header.type);
            evbuffer_drain(ebuf, header.length - sizeof(header));
            break;
        }
        
        break;

    case TUNNEL_CLIENT_BUILD_OK:
        break;

    default:
        LOG_ERR("conn_readcb error status:%d\n", tunnel->status);
        break;
    }
    

    // g_trafficStat->rx_bytes_period += length;
}


void tc_conn_writecb(struct bufferevent *bev, void *arg)
{

}

void tc_conn_eventcb(struct bufferevent *bev, short events, void *arg)
{
    LOG_DBG("tc_conn_eventcb %p\n", arg);

    struct tunnel* tunnel = (struct tunnel*)arg;

    if (events & BEV_EVENT_EOF) {
        LOG_INF("Connection closed.\n");
    } else if (events & BEV_EVENT_ERROR) {
        LOG_WAR("Got an error on the connection: %s\n", strerror(errno));
    }
    /* None of the other events can happen here, since we haven't enabled
     * timeouts */
    bufferevent_free(bev);
    bufferevent_free(tunnel->bev[1]);

    g_tunnel_mgr.tunnels.erase(tunnel->bev[0]);
    free(tunnel);

    LOG_INF("client connection stop, server connection stop.\n");
}


int handler_setup_tunnel_req(struct tunnel* tunnel, struct setup_tunnel_req* req, struct setup_tunnel_rsp* rsp)
{
    struct sockaddr_in sin;
    sin.sin_addr.s_addr = req->server_ip;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(req->server_port);

    LOG_INF("handler_setup_tunnel_req server_addr:%s:%u type:%d\n",
            inet_ntoa(sin.sin_addr), req->server_port, req->tunnel_type);

    tunnel->status = TUNNEL_CLIENT_BUILD_OK;
    tunnel->type = (enum tunnel_type)req->tunnel_type;
    memcpy(&tunnel->addr[1], &sin, sizeof(sin));

    struct bufferevent *bev = bufferevent_socket_new(g_evbase, -1, BEV_OPT_CLOSE_ON_FREE);
    bufferevent_setcb(bev, ts_conn_readcb, ts_conn_writecb, ts_conn_eventcb, tunnel);
    
    if (0 < bufferevent_socket_connect(bev, (struct sockaddr*)&sin, sizeof(sin))) {
        bufferevent_free(bev);
        return -1;
    }

    tunnel->bev[1] = bev;

    return 0;
}
