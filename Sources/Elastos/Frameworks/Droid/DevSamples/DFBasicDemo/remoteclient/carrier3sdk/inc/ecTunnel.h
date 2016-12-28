/*
 * Copyright (C) 2015 Elastos.org
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __ECTUNNEL_H__
#define __ECTUNNEL_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Max tunnel message payload length */
#define SESSTUNNEL_MAX_DATA_LEN             1400

/* Not include null terminal */
#define SESSTUNNEL_MAX_HOST_LEN             127
#define SESSTUNNEL_MAX_PORT_LEN             63

typedef struct sesstunnel SessTunnel;

/*******************************************************************************
 *
 * :: Creation and destruction
 *
 ******************************************************************************/
typedef enum SESSTUNNEL_MODE {
    /*
     * Client mode only.
     */
    SESSTUNNEL_MODE_CLIENT = 0x01,
    /*
     * Server mode only.
     */
    SESSTUNNEL_MODE_SERVER = 0x02,
    /*
     * Full duplex mode, both client and server.
     */
    SESSTUNNEL_MODE_DUPLEX = 0x03
} SESSTUNNEL_MODE;

typedef struct sesstunnel_options {
    int ipv6_enabled;
    SESSTUNNEL_MODE mode;
    uint8_t max_port_forwardings_per_tunnel;
    uint16_t max_channels_per_tunnel;
} SessTunnelOptions;

typedef enum SESSTUNNEL_ERR_NEW {
    /**
     * The function returned successfully.
     */
    SESSTUNNEL_ERR_NEW_OK,
    /**
     * One of the arguments to the function was NULL when it was not expected.
     */
    SESSTUNNEL_ERR_NEW_NULL,
    /**
     * Memory allocation failure while trying to allocate structures required
     * for the tunnel.
     */
    SESSTUNNEL_ERR_NEW_MALLOC,
    /**
     * Attempted to create a second session for the same Tox instance.
     */
    SESSTUNNEL_ERR_NEW_MULTIPLE,
} SESSTUNNEL_ERR_NEW;

/**
 * Create new SessTunnel. There can only be only one SessTunnel per Tox instance.
 */
SessTunnel *sesstunnel_new(SessTunnelOptions *options,
                         SESSTUNNEL_ERR_NEW *error);

/**
 * Releases all resources associated with the tunnel.
 *
 * If any calls were ongoing, these will be forcibly terminated without
 * notifying peers. After calling this function, no other functions may be
 * called and the SessTunnel pointer becomes invalid.
 */
void sesstunnel_kill(SessTunnel *tt);

/*******************************************************************************
 *
 * :: Tunnel event loop
 *
 ******************************************************************************/

/**
 * Start SessTunnel, begin process data packets.
 */
void sesstunnel_start(SessTunnel *tt);

/*******************************************************************************
 *
 * :: Port forwarding
 *
 ******************************************************************************/
typedef enum SESSTUNNEL_ERR_PORT_FORWARDING {
    /**
     * The function returned successfully.
     */
    SESSTUNNEL_ERR_PORT_FORWARDING_OK,
    /**
     * One of the arguments to the function was invalid.
     */
    SESSTUNNEL_ERR_PORT_FORWARDING_ARGS,
    /**
     * Memory allocation failure while trying to allocate structures required
     * for the port forwarding.
     */
    SESSTUNNEL_ERR_PORT_FORWARDING_MALLOC,
    /**
     * Network error.
     */
    SESSTUNNEL_ERR_PORT_FORWARDING_NETWORK,
} SESSTUNNEL_ERR_PORT_FORWARDING;

typedef enum PORT_FORWARDING_MODE {
    PORT_FORWARDING_MODE_TCP = 0x00,
    PORT_FORWARDING_MODE_UDP = 0x01
} PORT_FORWARDING_MODE;

/* Return port forwarding id. return value >= 0 on success, < 0 on error */
int sesstunnel_add_port_forwarding(SessTunnel *tt,
                                  const char *host, const char *port,
                                  uint32_t tunnel_id,
                                  const char *remote_host,
                                  const char *remote_port,
                                  PORT_FORWARDING_MODE mode,
                                  SESSTUNNEL_ERR_PORT_FORWARDING *error);

int sesstunnel_delete_port_forwarding(SessTunnel *tt,
                                     uint32_t tunnel_id, int pf);

#ifdef __cplusplus
}
#endif

#endif /* __ECTUNNEL_H__ */
