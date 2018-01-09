// #include <htest/test.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include <event2/event.h>
#include <event2/event_struct.h>
#include <event2/util.h>

using namespace std;

struct timeval lasttime;

int event_is_persistent;

static void timer_cb(evutil_socket_t fd, short event, void *arg)
{
	struct timeval newtime, difference;
	struct event* timeout = (struct event*)arg;
	double elapsed;

	evutil_gettimeofday(&newtime, NULL);
	evutil_timersub(&newtime, &lasttime, &difference);
	elapsed = difference.tv_sec + (difference.tv_usec / 1.0e6);

	printf("event:%x timeout_cb called at %d: %.3f seconds elapsed.\n", event, newtime.tv_sec, elapsed);
	lasttime = newtime;

	if (!event_is_persistent) {
		struct timeval tv;
		evutil_timerclear(&tv);
		tv.tv_sec = 2;
		event_add(timeout, &tv);
	}
}


int called = 0;

static void signal_cb(evutil_socket_t fd, short event, void *arg)
{
	struct event *signal = (struct event *)arg;

	printf("event:%x signal_cb: got signal %d\n", event, event_get_signal(signal));

	if (called >= 2)
		event_del(signal);

	called++;
}


int TimerTest_1(int argc, const char* argv[])
{
	struct timeval tv;
	struct event_base *base;
	int flags;


	if (argc == 2 && !strcmp(argv[1], "-p")) {
		event_is_persistent = 1;
		flags = EV_PERSIST;
	} else {
		event_is_persistent = 0;
		flags = 0;
	}

	/* Initalize the event library */
	base = event_base_new();

	/* Initalize one event */
	struct event timer;
	event_assign(&timer, base, -1, flags, timer_cb, (void*) &timer);

	evutil_timerclear(&tv);
	tv.tv_sec = 2;
	event_add(&timer, &tv);

	/* Initalize one event */
	struct event* signal_int = evsignal_new(base, SIGINT, signal_cb, event_self_cbarg());
	event_add(signal_int, NULL);

	evutil_gettimeofday(&lasttime, NULL);
	event_base_dispatch(base);

    return 0;
}

int TimerTest(int argc, const char* argv[])
{
	TimerTest_1(argc, argv);

    return 0;
}

/* end of file */
