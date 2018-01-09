/*
 * proto.h
 *
 *  Created on: 2018年1月5日
 *      Author: jason
 */

#ifndef SRC_PROTO_H_
#define SRC_PROTO_H_


#include "proxy_cfg.h"
#include "tunnel.h"

//
struct setup_tunnel_req
{
#define setup_tunnel_req_pid   (0x0001)
    uint8_t     tunnel_type;
    uint16_t    server_port;
    uint32_t    server_ip;
    uint32_t    client_id;
    uint64_t    vmid;
};

//
struct setup_tunnel_rsp
{
#define setup_tunnel_rsp_pid   (0x0002)
    uint32_t    ret_code;
};


//
struct set_flow_speed_req
{
#define set_flow_speed_req_pid   (0x0011)
    struct flow_speed_cfg   cfg;
};

//
struct set_flow_speed_rsp
{
#define set_flow_speed_rsp_pid   (0x0012)
    uint32_t    ret_code;
};


struct get_flow_speed_req
{
#define get_flow_speed_req_pid   (0x0015)
};

//
struct get_flow_speed_rsp
{
#define get_flow_speed_rsp_pid   (0x0016)
    struct flow_speed_cfg   cfg;
};


#endif /* SRC_PROTO_H_ */
