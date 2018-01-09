/*
 * proto.h
 *
 *  Created on: 2018年1月5日
 *      Author: jason
 */

#ifndef SRC_EVENT_PROTO_H_
#define SRC_EVENT_PROTO_H_

#include <sys/socket.h>
//#include <>

struct message_header {
    int32_t length;
    int32_t magic;
    int32_t type;
};

struct setup_req {
    uint8_t     tunnel_type;
    uint32_t    client_id;

};

struct setup_rsp {

};



#endif /* SRC_EVENT_PROTO_H_ */
