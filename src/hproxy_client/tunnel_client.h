
#include <event2/event.h>
#include "proto.h"

void tc_conn_readcb(struct bufferevent *bev, void *arg);

void tc_conn_writecb(struct bufferevent *bev, void *arg);

void tc_conn_eventcb(struct bufferevent *bev, short events, void *arg);

struct setup_tunnel_req do_setup_tunnel_req(const char* ip, uint16_t port);

void handler_setup_tunnel_rsp(struct setup_tunnel_rsp* rsp);
