
#include <event2/event.h>
#include "proto.h"

void tc_conn_readcb(struct bufferevent *bev, void *arg);

void tc_conn_writecb(struct bufferevent *bev, void *arg);

void tc_conn_eventcb(struct bufferevent *bev, short events, void *arg);

int handler_setup_tunnel_req(struct tunnel* tunnel, struct setup_tunnel_req* req, struct setup_tunnel_rsp* rsp);
