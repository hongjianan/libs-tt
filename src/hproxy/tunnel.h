/*
 * tunnel.h
 *
 *  
 *
 *  Created on: 2018年1月8日
 *      Author: Hong
 */

#ifndef SRC_HPROXY_TUNNEL_H_
#define SRC_HPROXY_TUNNEL_H_

// #include <netinet/in.h>
#include <sys/socket.h>
#include <event2/event.h>

#define TUNNEL_CLIENT_IDX	(0)
#define TUNNEL_SERVER_IDX	(1)

enum tunnel_type
{
    TUNNEL_TYPE_FILE = 0,
    TUNNEL_TYPE_IOLOG,
    TUNNEL_TYPE_FCMP,
    TUNNEL_TYPE_OTHERS
};


enum tunnel_status
{
    TUNNEL_INIT = 0,
    TUNNEL_CLIENT_BUILD_OK,
    TUNNEL_CLIENT_BUILD_FAIL,
    TUNNEL_SERVER_BUILD_OK,
    TUNNEL_SERVER_BUILD_FAIL,
    TUNNEL_COMPLETED_OK,
    TUNNEL_COMPLETED_FAIL,
    TUNNEL_FREE,
};


struct tunnel
{
    uint32_t            id;
    enum tunnel_type    type;
    enum tunnel_status  status;
    struct bufferevent* bev[2];
    struct sockaddr     addr[2];
};


#endif /* SRC_HPROXY_TUNNEL_H_ */
/* end of file */
