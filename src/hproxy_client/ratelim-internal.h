
#ifndef RATELIM_INTERNAL_H_INCLUDED_
#define RATELIM_INTERNAL_H_INCLUDED_

#include <event2/util.h>
#include <assert.h>
#include "log.h"

/** Configuration info for a token bucket or set of token buckets. */
struct ev_token_bucket_cfg_ {
    /** How many bytes are we willing to read on average per tick? */
    size_t read_rate;
    /** How many bytes are we willing to read at most in any one tick? */
    size_t read_maximum;
    /** How many bytes are we willing to write on average per tick? */
    size_t write_rate;
    /** How many bytes are we willing to write at most in any one tick? */
    size_t write_maximum;

    /* How long is a tick?  Note that fractions of a millisecond are
     * ignored. */
    struct timeval tick_timeout;

    /* How long is a tick, in milliseconds?  Derived from tick_timeout. */
    unsigned msec_per_tick;
};

inline void reset_rate_cfg(struct ev_token_bucket_cfg_* cfg, size_t read_rate, size_t read_burst,
                    size_t write_rate, size_t write_burst, const struct timeval *tick_len)
{
    assert(cfg);
    struct timeval g;
    if (!tick_len) {
        g.tv_sec = 1;
        g.tv_usec = 0;
        tick_len = &g;
    }

    LOG_INF("set rate limit, read:%d write:%d tick:%d\n", read_rate, write_rate, tick_len);

    cfg->read_rate = read_rate;
    cfg->write_rate = write_rate;
    cfg->read_maximum = read_burst;
    cfg->write_maximum = write_burst;
    memcpy(&cfg->tick_timeout, tick_len, sizeof(struct timeval));
    cfg->msec_per_tick = (tick_len->tv_sec * 1000) + 
                (tick_len->tv_usec & 0x000fffff) / 1000;
}

#endif /* RATELIM_INTERNAL_H_INCLUDED_ */
