/*
 * evbuffer_test.cpp
 *
 *  Created on: 2018年1月5日
 *      Author: jason
 */

#include "htest/htest.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <unistd.h>
#include <event2/event.h>
#include <event2/buffer.h>

using namespace std;

int EvbufferTest_evbuffer(int argc, const char* argv[])
{
    int length = 0;
    string input = "123456789\n";
    struct evbuffer* buffer = evbuffer_new();

//  evbuffer_expand();

    evbuffer_add(buffer, input.c_str(), input.length());
    cout << evbuffer_get_length(buffer) << endl;

    evbuffer_add_printf(buffer, "%s", input.c_str());
    cout << evbuffer_get_length(buffer) << endl;

    cout << "contiguous_space: " << evbuffer_get_contiguous_space(buffer) << endl;

    struct evbuffer* tmp = evbuffer_new();
    evbuffer_add_buffer(tmp, buffer);
    cout << "tmp:" << evbuffer_get_length(tmp) << " buffer:" << evbuffer_get_length(buffer) << endl;

    evbuffer_remove_buffer(tmp, buffer, 10);
    cout << "tmp:" << evbuffer_get_length(tmp) << " buffer:" << evbuffer_get_length(buffer) << endl;
    evbuffer_remove_buffer(tmp, buffer, 10);

    evbuffer_prepend(buffer, "abc", 3);

    char str[100];
    evbuffer_remove(buffer, str, 10);
    str[10] = 0;
    cout << "evbuffer_remove:" << str << endl;

    length = evbuffer_get_length(buffer);
    evbuffer_copyout(buffer, str, evbuffer_get_length(buffer));
//    evbuffer_peek();
    cout << "evbuffer_copyout:" << str << endl;
    str[length] = 0;
    evbuffer_drain(buffer, evbuffer_get_length(buffer));

    evbuffer_free(tmp);
    evbuffer_free(buffer);

    return 0;
}

int EvbufferTest_line(int argc, const char* argv[])
{
    int length = 0;
    string input = "123456789\nabcd";
    struct evbuffer* buffer = evbuffer_new();

    evbuffer_add(buffer, input.c_str(), input.length());
    cout << evbuffer_get_length(buffer) << endl;

    size_t len;
    char* line = evbuffer_readln(buffer, &len, EVBUFFER_EOL_LF);
    cout << strlen(line) << ": " << line << endl;
//    free(line);

    cout << evbuffer_get_length(buffer) << endl;

    evbuffer_free(buffer);

    return 0;
}

DEFINE_TEST(EvbufferTest)
{
//    return EvbufferTest_evbuffer(argc, argv);
//    return EvbufferTest_line(argc, argv);
    while (1) {
    for (int i = 0; i < 10000; i++)
        EvbufferTest_line(argc, argv);
    sleep(5);
    }
    return 0;
}
