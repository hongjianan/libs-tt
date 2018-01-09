/*
 * zlib_proxy.cpp
 *
 *  Created on: 2018年1月4日
 *      Author: jason
 */
/*
 * Copyright (c) 2008-2012 Niels Provos and Nick Mathewson
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include "htest/htest.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <netdb.h>
#include <signal.h>
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


static int readcb_finished;
static int writecb_finished;
static int errorcb_invoked;


void dump_mem(uint8_t *buffer, int size)
{
    int i;
    for (i = 0; i < size; ++i) {
        printf("%x ", buffer[i]);
        if (i && (i % 16) == 0) {
            printf("\n");
        }
    }
    if (i % 16 != 0) {
        printf("\n");
    }
}


/*
 * simple bufferevent test (over transparent zlib treatment)
 */

static void readcb(struct bufferevent *bev, void *arg)
{
    printf("call readcb\n");

    struct evbuffer *input = bufferevent_get_input(bev);

    if (evbuffer_get_length(input) == 8333) {

        uint8_t buffer[1000];
        /* gratuitous test of bufferevent_read_buffer */
        int size = bufferevent_read(bev, buffer, sizeof(buffer));
        dump_mem(buffer, size);
        evbuffer_drain(input, evbuffer_get_length(input));

        bufferevent_disable(bev, EV_READ);

    }
}

static void writecb(struct bufferevent *bev, void *arg)
{
    printf("call writecb\n");

    if (evbuffer_get_length(bufferevent_get_output(bev)) == 0) {
        ++writecb_finished;
    }
}

static void errorcb(struct bufferevent *bev, short what, void *arg)
{
    errorcb_invoked = 1;
}

int ZlibProxyTest_zlib(int argc, const char* argv[])
{
    struct event_base* base = event_base_new();
    if (!base) {
        fprintf(stderr, "Could not initialize libevent!\n");
        return -11;
    }

    struct bufferevent *bev1=NULL, *bev2=NULL;
    char buffer[8333];

    int i, r;
    evutil_socket_t pair[2] = {-1, -1};

    readcb_finished = writecb_finished = errorcb_invoked = 0;

    if (evutil_socketpair(AF_UNIX, SOCK_STREAM, 0, pair) == -1) {
        perror("socketpair error");
        return -1;
    }

    evutil_make_socket_nonblocking(pair[0]);
    evutil_make_socket_nonblocking(pair[1]);

    bev1 = bufferevent_socket_new(base, pair[0], 0);
    bev2 = bufferevent_socket_new(base, pair[1], 0);

    z_stream* z_output = (z_stream*)calloc(sizeof(*z_output), 1);
    r = deflateInit(z_output, Z_DEFAULT_COMPRESSION);
    z_stream* z_input = (z_stream*)calloc(sizeof(*z_input), 1);
    r = inflateInit(z_input);

    /* initialize filters */
    bev1 = bufferevent_filter_new(bev1, NULL, zlib_output_filter,
        BEV_OPT_CLOSE_ON_FREE, zlib_deflate_free, z_output);
    bev2 = bufferevent_filter_new(bev2, zlib_input_filter,
        NULL, BEV_OPT_CLOSE_ON_FREE, zlib_inflate_free, z_input);
    bufferevent_setcb(bev1, readcb, writecb, errorcb, NULL);
    bufferevent_setcb(bev2, readcb, writecb, errorcb, NULL);

    bufferevent_disable(bev1, EV_READ);
    bufferevent_enable(bev1, EV_WRITE);

    bufferevent_enable(bev2, EV_READ);
    bufferevent_disable(bev2, EV_WRITE);

    for (i = 0; i < (int)sizeof(buffer); i++)
        buffer[i] = i;

    /* break it up into multiple buffer chains */
    bufferevent_write(bev1, buffer, 1800);
    bufferevent_write(bev1, buffer + 1800, sizeof(buffer) - 1800);

    /* we are done writing - we need to flush everything */
    bufferevent_flush(bev1, EV_WRITE, BEV_FINISHED);

    event_base_dispatch(base);

    if (bev1)
        bufferevent_free(bev1);
    if (bev2)
        bufferevent_free(bev2);

    if (pair[0] >= 0)
        evutil_closesocket(pair[0]);
    if (pair[1] >= 0)
        evutil_closesocket(pair[1]);
}


DEFINE_TEST(ZlibProxyTest)
{
    return ZlibProxyTest_zlib(argc, argv);
}

