/*
 * header.h
 *
 *  
 *
 *  Created on: 2018年1月8日
 *      Author: Hong
 */

#ifndef SRC_HPROXY_HEADER_H_
#define SRC_HPROXY_HEADER_H_

#include <stdint.h>

#define HEADER_MAGIC    (0x11223344)

struct message_header
{
    int32_t length;
    int32_t magic;
    int32_t type;
};


static inline int header_unpack(struct message_header* header)
{
    if (HEADER_MAGIC != header->magic) {
        return -1;
    }

    return header->length;
}


#endif /* SRC_HPROXY_HEADER_H_ */
/* end of file */
