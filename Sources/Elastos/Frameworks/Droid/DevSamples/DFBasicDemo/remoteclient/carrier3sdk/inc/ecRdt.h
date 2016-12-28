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

#ifndef __ECRDT_H__
#define __ECRDT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#ifndef _ECERR
#define _ECERR(e) ((uint32_t)e)
#endif

/* new error code */
#define ECRDT_OK                    _ECERR(0)
#define ECRDT_E_BAD_PARAM           _ECERR(0x80003001)
#define ECRDT_E_OOM                 _ECERR(0x80003002)
#define ECRDT_E_ALREADY_STARTED     _ECERR(0x80003003)
#define ECRDT_E_NOT_STARTED         _ECERR(0x80003004)
#define ECRDT_E_BAD_RDT_TUNNEL      _ECERR(0x80003008)
#define ECRDT_E_EXCEED_LIMIT        _ECERR(0x80003009)
#define ECRDT_E_NETWORK             _ECERR(0x8000300A)
#define ECRDT_E_UNKOWN              _ECERR(0x8000300B)
#define ECRDT_E_NOT_IMPLEMENTED     _ECERR(0x8000300C)

#define MAX_CHANNEL_NUM           5

typedef struct ecRdtInfo {
    int sessionId;
    int channelId;
    uint64_t bytesOfReceived;
    uint64_t bytesOfSent;
} ecRdtInfo;

typedef struct ecRdtHandler {
    /**
     * @brief This callback will be invoked when data finished receiving for
     *  specific rdt tunnel.
     *
     * @param
     *      rdtId            [in] The ID of rdt tunnel.
     * @param
     *      data             [in] buffer to received data.
     * @param
     *      length           [in] The length of received data.
     *
     */
    void (*onData)(int rdtId, void* data, int length);

    /**
     * @brief This callback will be invoked when specific rdt tunnel is closed.
     *
     * @param
     *      rdtId            [in] The ID of this rdt tunnel
     * @param
     *      status           [in] The reason to close this rdt tunnel.
     *
     */
    void (*onClosed)(int rdtId, int status);
} ecRdtHandler;

typedef struct ecRdtInitializer {
    /**
     * @brief This callback invoked when a rdt tunnel is newly opened.
     *
     * @param
     *      sessionId        [in] ID of session on which rdt tunnel rely.
     * @param
     *      channelId        [in] ID of channel used by this rdt tunnel.
     *
     */
    ecRdtHandler* (*onRdtOpened)(int sessionId, int channelId, int rdtId);
} ecRdtInitializer;

/**
 * @brief Initialize ECRDT module.
 *
 * @brief This function should be called before using any other RDT functions.
 *
 * @param
 *     initializer        [in] The initializer to RDT module.
 *
 * @return
 *     Error code if return value < 0.
 */
int ecRdtModuleInitialize(ecRdtInitializer* initializer);

/**
 * @brief Destroy ECRDT module.
 *
 * @return
 *     Error code.
 */
int ecRdtModuleDestroy(void);

/**
 * @brief Open a ECRDT channel on specific channel.
 *
 * @param
 *      sessionId         [in] The ID of the session for rdt tunnel to open
 * @param
 *      channelId         [in] The ID of channel used by rdt tunnel. *
 * @param
 *      handler           [in] The event handler to rdt tunnel.
 *
 * @return
 *     Rdt tunnel ID if return value >= 0.
 * @return
 *     Error code if return value < 0.
 */
int ecRdtOpen(int sessionId, int channelId, ecRdtHandler* handler);


/**
 * @brief Close a ECRDT channel.
 *
 * @param
 *     rtdId               [in] The ID of the ECRDT tunnel to close.
 *
 * @return
 *     Error code.
 */
int ecRdtClose(int rtdId);

/**
 * @brief Write data through a ECRDT channel.
 *
 *
 * @param
 *     rdtId               [in] The ID of the ECRDT tunnel to write data
 * @param
 *     data                [in] The buffer to data to write.
 * @param
 *     length              [in] The length of data to write.
 *
 * @return
 *     The actual length of buffer to be written if write successfully.
 * @return
 *     Error code if return value < 0.
 */
int ecRdtWrite(int rdtId, const void* data, int length);

/**
 * @brief Get information of a ECRDT channel.
 *
 * @param
 *     rdtId               [in] The ID of the ECRDT tunnel to get information.
 * @param
 *     info                [out] The info to this rdt tunnel.
 *
 * @return
 *     Error code.
 */
int ecRdtGetInfo(int rdtId, ecRdtInfo* info);

#ifdef __cplusplus
}
#endif

#endif ///< __ECRDT_H__
