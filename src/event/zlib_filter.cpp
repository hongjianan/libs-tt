/*
 * zlib_filter.cpp
 *
 *  Created on: 2018年1月4日
 *      Author: jason
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <assert.h>
#include <errno.h>

#include <event2/event-config.h>
#include <event2/util.h>
#include <event2/event.h>
#include <event2/event_compat.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>

#include "zlib_filter.h"


static int getstate(enum bufferevent_flush_mode state)
{
    switch (state) {
    case BEV_FINISHED:
        return Z_FINISH;
    case BEV_FLUSH:
        return Z_SYNC_FLUSH;
    case BEV_NORMAL:
    default:
        return Z_NO_FLUSH;
    }
}

void zlib_deflate_free(void *ctx)
{
    z_streamp p = (z_streamp)ctx;

    assert(deflateEnd(p) == Z_OK);
    free(p);
}

void zlib_inflate_free(void *ctx)
{
    z_streamp p = (z_streamp)ctx;

    assert(inflateEnd(p) == Z_OK);
    free(p);
}

/*
 * The input filter is triggered only on new input read from the network.
 * That means all input data needs to be consumed or the filter needs to
 * initiate its own triggering via a timeout.
 */

enum bufferevent_filter_result
zlib_input_filter(struct evbuffer *src, struct evbuffer *dst,
    ev_ssize_t lim, enum bufferevent_flush_mode state, void *ctx)
{
    printf("call zlib_input_filter\n");
    struct evbuffer_iovec v_in[1];
    struct evbuffer_iovec v_out[1];
    int nread, nwrite;
    int res, n;

    z_streamp p = (z_streamp)ctx;

    do {
        /* let's do some decompression */
        n = evbuffer_peek(src, -1, NULL, v_in, 1);
        if (n) {
            p->avail_in = v_in[0].iov_len;
            p->next_in = (unsigned char *)v_in[0].iov_base;
        } else {
            p->avail_in = 0;
            p->next_in = 0;
        }

        evbuffer_reserve_space(dst, 4096, v_out, 1);
        p->next_out = (unsigned char *)v_out[0].iov_base;
        p->avail_out = v_out[0].iov_len;

        /* we need to flush zlib if we got a flush */
        res = inflate(p, getstate(state));

        /* let's figure out how much was compressed */
        nread = v_in[0].iov_len - p->avail_in;
        nwrite = v_out[0].iov_len - p->avail_out;

        evbuffer_drain(src, nread);
        v_out[0].iov_len = nwrite;
        evbuffer_commit_space(dst, v_out, 1);

        if (res==Z_BUF_ERROR) {
            /* We're out of space, or out of decodeable input.
               Only if nwrite == 0 assume the latter.
             */
            if (nwrite == 0)
                return BEV_NEED_MORE;
        } else {
            assert(res == Z_OK || res == Z_STREAM_END);
        }

    } while (evbuffer_get_length(src) > 0);

    return (BEV_OK);
}

enum bufferevent_filter_result zlib_output_filter(struct evbuffer *src, struct evbuffer *dst,
        ev_ssize_t lim, enum bufferevent_flush_mode state, void *ctx)
{
    printf("call zlib_output_filter\n");

    struct evbuffer_iovec v_in[1];
    struct evbuffer_iovec v_out[1];
    int nread, nwrite;
    int res, n;

    z_streamp p = (z_streamp)ctx;

    do {
        /* let's do some compression */
        n = evbuffer_peek(src, -1, NULL, v_in, 1);
        if (n) {
            p->avail_in = v_in[0].iov_len;
            p->next_in = (unsigned char *)v_in[0].iov_base;
        } else {
            p->avail_in = 0;
            p->next_in = 0;
        }

        evbuffer_reserve_space(dst, 4096, v_out, 1);
        p->next_out = (unsigned char *)v_out[0].iov_base;
        p->avail_out = v_out[0].iov_len;

        /* we need to flush zlib if we got a flush */
        res = deflate(p, getstate(state));

        /* let's figure out how much was decompressed */
        nread = v_in[0].iov_len - p->avail_in;
        nwrite = v_out[0].iov_len - p->avail_out;

        evbuffer_drain(src, nread);
        v_out[0].iov_len = nwrite;
        evbuffer_commit_space(dst, v_out, 1);

        if (res==Z_BUF_ERROR) {
            /* We're out of space, or out of decodeable input.
               Only if nwrite == 0 assume the latter.
             */
            if (nwrite == 0)
                return BEV_NEED_MORE;
        } else {
            assert(res == Z_OK || res == Z_STREAM_END);
        }

    } while (evbuffer_get_length(src) > 0);

    printf("zlib_output_filter OK\n");

    return (BEV_OK);
}

