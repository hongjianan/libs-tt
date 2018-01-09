#include <event2/event.h>

void ts_conn_readcb(struct bufferevent *bev, void *arg);

void ts_conn_writecb(struct bufferevent *bev, void *arg);

void ts_conn_eventcb(struct bufferevent *bev, short events, void *arg);

