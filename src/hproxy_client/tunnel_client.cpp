
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/time.h>

#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/util.h>
#include <event2/event.h>

#include "log.h"
#include "header.h"
#include "proto.h"
#include "tunnel.h"
#include "traffic_stat.h"
#include "tunnel_client.h"

extern struct event_base*  g_evbase;
extern char*           g_server_ip;
extern uint16_t        g_server_port;
extern TrafficStat*    g_trafficStat;
extern uint8_t*        g_message;
extern int				g_length;
enum tunnel_status		g_tunnel_status;

void tc_conn_readcb(struct bufferevent *bev, void *arg)
{
    LOG_DBG("conn_readcb bev:%p arg:%p\n", bev, arg);

    struct evbuffer *ebuf = bufferevent_get_input(bev);
    int buf_len = evbuffer_get_length(ebuf);
    int pack_len = 0;

	switch (g_tunnel_status)
	{
	case TUNNEL_INIT:
        evbuffer_copyout(ebuf, &pack_len, sizeof(pack_len));
        if (pack_len > buf_len) {
            LOG_DBG("message uncomplete pack_len:%d buf_len:%d\n", pack_len, buf_len);
            return;
        }
        struct message_header header;
        evbuffer_remove(ebuf, &header, sizeof(header));

        switch (header.type)
        {
        case setup_tunnel_rsp_pid:
        	LOG_INF("read setup_tunnel_rsp_pid\n");
            struct setup_tunnel_rsp rsp;
            evbuffer_remove(ebuf, &rsp, sizeof(rsp));
            handler_setup_tunnel_rsp(&rsp);
            
            g_trafficStat->rx_bytes_period += sizeof(rsp);

            bufferevent_write(bev, g_message, g_length);
            g_trafficStat->tx_bytes_period += g_length;
			g_trafficStat->tx_packets_period++;

            break;

        default:
            LOG_ERR("tunnel_mgr::conn_readcb error message type:%d\n", header.type);
            evbuffer_drain(ebuf, header.length - sizeof(header));
            break;
        }
        
        break;

	default:
    	 LOG_ERR("conn_readcb error status:%d\n", g_tunnel_status);
         break;
	}
    
    g_trafficStat->rx_bytes_period += pack_len;
    g_trafficStat->rx_packets_period ++;
}


void tc_conn_writecb(struct bufferevent *bev, void *arg)
{
	struct evbuffer *output = bufferevent_get_output(bev);
	if (evbuffer_get_length(output) == 0) {
		bufferevent_write(bev, g_message, g_length);
		g_trafficStat->tx_bytes_period += g_length;
		g_trafficStat->tx_packets_period++;
//		LOG_DBG("write complete.\n");
	} else {
		//printf("write left:%d\n", evbuffer_get_length(output));
	}
}

void tc_conn_eventcb(struct bufferevent *bev, short events, void *arg)
{
    LOG_DBG("tc_conn_eventcb %p\n", arg);

    if (events & BEV_EVENT_CONNECTED) {
        LOG_INF("Connection server success.\n");
        struct setup_tunnel_req req;
        do_setup_tunnel_req(&req, g_server_ip, g_server_port);
        struct message_header header;
        header.length = sizeof(header) + sizeof(req);
        header.magic = HEADER_MAGIC;
        header.type = setup_tunnel_req_pid;
        
        uint8_t msg[1024];
        memcpy(msg, &header, sizeof(header));
        memcpy(msg + sizeof(header), &req, sizeof(req));

        bufferevent_write(bev, msg, header.length);

        return;
    } else if (events & BEV_EVENT_EOF) {
        LOG_INF("Connection closed.\n");
    } else if (events & BEV_EVENT_ERROR) {
        LOG_WAR("Got an error on the connection: %s\n", strerror(errno));
    }

    bufferevent_free(bev);
    struct timeval tv = {1, 0};
    event_base_loopexit(g_evbase, &tv);
}


int do_setup_tunnel_req(struct setup_tunnel_req* req, const char* ip, uint16_t port)
{
    LOG_INF("setup_tunnel_req server_addr:%s:%u\n", ip, port);

    req->tunnel_type = (uint8_t)TUNNEL_TYPE_FILE;
    req->server_ip = (uint32_t)inet_addr(ip);
    req->server_port = port;
    req->vmid      = 2222;
    req->client_id = 1111;

    g_tunnel_status = TUNNEL_INIT;

    return 0;
}


void handler_setup_tunnel_rsp(struct setup_tunnel_rsp* rsp)
{
    LOG_INF("setup_tunnel_rsp ret_code:%d\n", rsp->ret_code);
    return;
}


// handler_setup_tunnel_rsp
