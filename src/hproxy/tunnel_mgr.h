/*
 * client_mgr.h
 *
 *  
 *
 *  Created on: 2018年1月8日
 *      Author: Hong
 */

#ifndef SRC_HPROXY_TUNNEL_MGR_H_
#define SRC_HPROXY_TUNNEL_MGR_H_

#include <map>
#include <event2/event.h>
#include "tunnel.h"
#include "tunnel_client.h"
#include "tunnel_server.h"

struct tunnel_mgr
{
    int32_t    tunnel_count;
    std::map<uint32_t, int32_t> host_tunnels;
    std::map<struct bufferevent*, struct tunnel*> tunnels;
};


void tm_listener_cb(struct evconnlistener *listener, evutil_socket_t fd,
                    struct sockaddr *sa, int socklen, void *user_data);


extern struct tunnel_mgr    g_tunnel_mgr;

#endif /* SRC_HPROXY_TUNNEL_MGR_H_ */
/* end of file */
