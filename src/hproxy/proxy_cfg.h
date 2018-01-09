/*
 * proxy_cfg.h
 *
 *  
 *
 *  Created on: 2018年1月7日
 *      Author: Hong
 */

#ifndef SRC_PROXY_CFG_H_
#define SRC_PROXY_CFG_H_

#include <stdint.h>


struct proxy_cfg
{
    uint16_t    proxy_port;
    uint16_t    debug_port;
};

static inline uint16_t cfg_get_proxy_port(struct proxy_cfg* cfg)
{
    return cfg->proxy_port;
}

static inline void cfg_set_proxy_port(struct proxy_cfg* cfg, uint16_t port)
{
    cfg->proxy_port = port;
}

static inline uint16_t cfg_get_debug_port(struct proxy_cfg* cfg)
{
    return cfg->debug_port;
}

static inline void cfg_set_debug_port(struct proxy_cfg* cfg, uint16_t port)
{
    cfg->debug_port = port;
}


struct flow_speed_cfg
{
    int32_t    file_speed_min;
    int32_t    file_speed_max;

    int32_t    iolog_speed_min;
    int32_t    iolog_speed_max;
};

extern struct proxy_cfg g_cfg;

#endif /* SRC_PROXY_CFG_H_ */
