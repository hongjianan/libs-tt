
#include <event2/event.h>
#include "proto.h"
#include "proxy_cfg.h"


void fc_conn_readcb(struct bufferevent *bev, void *arg);

void fc_conn_writecb(struct bufferevent *bev, void *arg);

void fc_conn_eventcb(struct bufferevent *bev, short events, void *arg);

//int handler_setup_tunnel_req(struct setup_tunnel_req_pid* req, struct setup_tunnel_rsp* rsp);
