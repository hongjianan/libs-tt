/*
 * zlib_filter.h
 *
 *  Created on: 2018年1月4日
 *      Author: jason
 */

#ifndef SRC_EVENT_ZLIB_FILTER_H_
#define SRC_EVENT_ZLIB_FILTER_H_

#include <event2/util.h>

/* zlib 1.2.4 and 1.2.5 do some "clever" things with macros.  Instead of
   saying "(defined(FOO) ? FOO : 0)" they like to say "FOO-0", on the theory
   that nobody will care if the compile outputs a no-such-identifier warning.

   Sorry, but we like -Werror over here, so I guess we need to define these.
   I hope that zlib 1.2.6 doesn't break these too.
*/
#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE 0
#endif
#ifndef _LFS64_LARGEFILE
#define _LFS64_LARGEFILE 0
#endif
#ifndef _FILE_OFFSET_BITS
#define _FILE_OFFSET_BITS 0
#endif
#ifndef off64_t
#define off64_t ev_int64_t
#endif

#include <zlib.h>


void zlib_deflate_free(void *ctx);

void zlib_inflate_free(void *ctx);

enum bufferevent_filter_result
zlib_input_filter(struct evbuffer *src, struct evbuffer *dst,
        ev_ssize_t lim, enum bufferevent_flush_mode state, void *ctx);

enum bufferevent_filter_result
zlib_output_filter(struct evbuffer *src, struct evbuffer *dst,
        ev_ssize_t lim, enum bufferevent_flush_mode state, void *ctx);


#endif /* SRC_EVENT_ZLIB_FILTER_H_ */
