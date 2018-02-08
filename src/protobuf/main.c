#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "message.pb-c.h"


int main(int argc, char *argv[])
{
	//GOOGLE_PROTOBUF_VERIFY_VERSION;
    // pack
    int size = 3;
    Message__MultiHeader out = MESSAGE__MULTI_HEADER__INIT;
    out.n_headers = size;
    out.headers = (Message__Header**)malloc(sizeof(Message__Header*) * size);
    Message__Header* headers = (Message__Header*)malloc(sizeof(*headers) * size);
    for (int idx = 0; idx < size; ++idx) {
        Message__Header* header = &headers[idx];
        message__header__init(header);
        header->length = 10 + idx;
        header->ret_code = 0 + idx;
        header->uri_type = 0 + idx;

        out.headers[idx] = header;
    }

    size_t pack_size = message__multi_header__get_packed_size(&out);
    uint8_t* outbuf = (uint8_t*)malloc(pack_size);
    message__multi_header__pack(&out, outbuf);
    free(out.headers);


    // unpack
    Message__MultiHeader* in = message__multi_header__unpack(NULL, pack_size, outbuf);
    printf("header size:%d\n", in->n_headers);
    for (int idx = 0; idx < in->n_headers; ++idx) {
        Message__Header* header = in->headers[idx];
        printf("header:%p length:%d ret_code:%d uri_type:%d\n",
                header, header->length, header->ret_code, header->uri_type);
    }
    message__multi_header__free_unpacked(in, NULL);


    // release
	free(outbuf);
	
	return 0;
}
