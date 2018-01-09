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
#include "tunnel_mgr.h"
#include "tunnel.h"


void fc_listener_cb(struct evconnlistener *listener, evutil_socket_t fd,
                        struct sockaddr *sa, int socklen, void *user_data)
{
//    struct event_base *base = (struct event_base *)user_data;
//    struct bufferevent *bev;
//
//    struct sockaddr_in *addr = (struct sockaddr_in *)sa;
//    LOG_DBG("client connected: %s:%u\n", inet_ntoa(addr->sin_addr), addr->sin_port);
//
//    bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
//    if (!bev) {
//        LOG_ERR("Error constructing bufferevent!");
//        event_base_loopbreak(base);
//        return;
//    }
//
//    g_tunnel_mgr.tunnel_count++;
//    g_tunnel_mgr.host_tunnels[addr->sin_addr.s_addr]++;
//
//    struct tunnel* tunnel = (struct tunnel*)malloc(sizeof(*tunnel));
//    tunnel->bev[0] = bev;
//    tunnel->bev[1] = NULL;
//
//    memcpy(&tunnel->client_addr, sa, sizeof(*sa));
//    memset(&tunnel->server_addr, 0, sizeof(tunnel->server_addr));
//    tunnel->status = TUNNEL_INIT;
//
//    g_tunnel_mgr.tunnels[bev] = tunnel;
//
//    bufferevent_setcb(bev, conn_readcb, conn_writecb, conn_eventcb, tunnel);
//    bufferevent_enable(bev, EV_READ | EV_WRITE);
}

