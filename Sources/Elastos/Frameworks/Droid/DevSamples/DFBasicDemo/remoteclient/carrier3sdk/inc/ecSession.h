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

#ifndef __ECSESSION_H__
#define __ECSESSION_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#ifndef _ECERR
#define _ECERR(e) ((uint32_t)e)
#endif

/* new error code */
#define ECSESS_E_BAD_PARAM           _ECERR(0x80002001)
#define ECSESS_E_OOM                 _ECERR(0x80002002)
#define ECSESS_E_ALREADY_STARTED     _ECERR(0x80002003)
#define ECSESS_E_NOT_STARTED         _ECERR(0x80002004)
#define ECSESS_E_BAD_SESSION         _ECERR(0x80002008)
#define ECSESS_E_EXCEED_LIMIT        _ECERR(0x80002009)
#define ECSESS_E_NETWORK             _ECERR(0x8000200A)
#define ECSESS_E_UNKOWN              _ECERR(0x8000200B)

//pj error (70001-70024)
#define ECSESS_E_PJ_UNKNOWN          _ECERR(0x80002201)        //Unknown error has been reported.
#define ECSESS_E_PJ_PENDING          _ECERR(0x80002202)        //The operation is pending and will be completed later.
#define ECSESS_E_PJ_TOOMANYCONN      _ECERR(0x80002203)        //Too many connecting sockets.
#define ECSESS_E_PJ_INVAL            _ECERR(0x80002204)        //Invalid argument.
#define ECSESS_E_PJ_NAMETOOLONG      _ECERR(0x80002205)        //Name too long (eg. hostname too long).
#define ECSESS_E_PJ_NOTFOUND         _ECERR(0x80002206)        //Not found.
#define ECSESS_E_PJ_NOMEM            _ECERR(0x80002207)        //Not enough memory.
#define ECSESS_E_PJ_BUG              _ECERR(0x80002208)        //Bug detected!
#define ECSESS_E_PJ_TIMEDOUT         _ECERR(0x80002209)        //Operation timed out.
#define ECSESS_E_PJ_TOOMANY          _ECERR(0x8000220A)        //Too many objects.
#define ECSESS_E_PJ_BUSY             _ECERR(0x8000220B)        //Object is busy.
#define ECSESS_E_PJ_NOTSUP           _ECERR(0x8000220C)        //The specified option is not supported.
#define ECSESS_E_PJ_INVALIDOP        _ECERR(0x8000220D)        //Invalid operation.
#define ECSESS_E_PJ_CANCELLED        _ECERR(0x8000220E)        //Operation is cancelled.
#define ECSESS_E_PJ_EXISTS           _ECERR(0x8000220F)        //Object already exists.
#define ECSESS_E_PJ_EOF              _ECERR(0x80002210)        //End of file.
#define ECSESS_E_PJ_TOOBIG           _ECERR(0x80002211)        //Size is too big.
#define ECSESS_E_PJ_RESOLVE          _ECERR(0x80002212)        //Error in gethostbyname(). This is a generic error returned when gethostbyname() has returned an error.
#define ECSESS_E_PJ_TOOSMALL         _ECERR(0x80002213)        //Size is too small.
#define ECSESS_E_PJ_IGNORED          _ECERR(0x80002214)        //Ignored
#define ECSESS_E_PJ_IPV6NOTSUP       _ECERR(0x80002215)        //IPv6 is not supported
#define ECSESS_E_PJ_AFNOTSUP         _ECERR(0x80002216)        //Unsupported address family
#define ECSESS_E_PJ_GONE             _ECERR(0x80002217)        //Object no longer exists
#define ECSESS_E_PJ_SOCKETSTOP       _ECERR(0x80002218)        //Socket is stopped

//pjnath error (370001-370120)
#define ECSESS_E_PJNATH_EINSTUNMSG          _ECERR(0x80002230)     //Invalid STUN message
#define ECSESS_E_PJNATH_EINSTUNMSGLEN       _ECERR(0x80002231)     //Invalid STUN message length
#define ECSESS_E_PJNATH_EINSTUNMSGTYPE      _ECERR(0x80002232)     //Invalid or unexpected STUN message type
#define ECSESS_E_PJNATH_STUNTIMEDOUT        _ECERR(0x80002233)     //STUN transaction has timed out
#define ECSESS_E_PJNATH_ESTUNTOOMANYATTR    _ECERR(0x80002234)     //Too many STUN attributes
#define ECSESS_E_PJNATH_ESTUNINATTRLEN      _ECERR(0x80002235)     //Invalid STUN attribute length
#define ECSESS_E_PJNATH_ESTUNDUPATTR        _ECERR(0x80002236)     //Found duplicate STUN attribute
#define ECSESS_E_PJNATH_ESTUNFINGERPRINT    _ECERR(0x80002237)     //STUN FINGERPRINT verification failed
#define ECSESS_E_PJNATH_ESTUNMSGINTPOS      _ECERR(0x80002238)     //Invalid STUN attribute after MESSAGE-INTEGRITY
#define ECSESS_E_PJNATH_ESTUNFINGERPOS      _ECERR(0x80002239)     //Invalid STUN attribute after FINGERPRINT
#define ECSESS_E_PJNATH_ESTUNNOMAPPEDADDR   _ECERR(0x8000223A)     //STUN (XOR-)MAPPED-ADDRESS attribute not found
#define ECSESS_E_PJNATH_ESTUNIPV6NOTSUPP    _ECERR(0x8000223B)     //STUN IPv6 attribute not supported
#define ECSESS_E_PJNATH_EINVAF              _ECERR(0x8000223C)     //Invalid address family value in STUN message
#define ECSESS_E_PJNATH_ESTUNINSERVER       _ECERR(0x8000223D)     //Invalid STUN server or server not configured
#define ECSESS_E_PJNATH_ESTUNDESTROYED      _ECERR(0x8000223E)     //STUN object has been destoyed
#define ECSESS_E_PJNATH_ENOICE              _ECERR(0x8000223F)     //ICE session not available
#define ECSESS_E_PJNATH_EICEINPROGRESS      _ECERR(0x80002240)     //ICE check is in progress
#define ECSESS_E_PJNATH_EICEFAILED          _ECERR(0x80002241)     //ICE connectivity check has failed
#define ECSESS_E_PJNATH_EICEMISMATCH        _ECERR(0x80002242)     //Default destination does not match any ICE candidates
#define ECSESS_E_PJNATH_EICEINCOMPID        _ECERR(0x80002243)     //Invalid ICE component ID
#define ECSESS_E_PJNATH_EICEINCANDID        _ECERR(0x80002244)     //Invalid ICE candidate ID
#define ECSESS_E_PJNATH_EICEINSRCADDR       _ECERR(0x80002245)     //Source address mismatch
#define ECSESS_E_PJNATH_EICEMISSINGSDP      _ECERR(0x80002246)     //Missing ICE SDP attribute
#define ECSESS_E_PJNATH_EICEINCANDSDP       _ECERR(0x80002247)     //Invalid SDP 'candidate' attribute
#define ECSESS_E_PJNATH_EICENOHOSTCAND      _ECERR(0x80002248)     //No host candidate associated with srflx
#define ECSESS_E_PJNATH_EICENOMTIMEOUT      _ECERR(0x80002249)     //Controlled agent timed-out
#define ECSESS_E_PJNATH_ETURNINTP           _ECERR(0x8000224A)     //Invalid or unsupported TURN transport

#define MAX_HOST_NAME             255
#define MAX_SESSION_NUM           5
#define MAX_CHANNEL_NUM           5

#define MAX_NAME_LEN              32
#define PEER_ID_LEN               32
#define PEER_ID_TLEN             (PEER_ID_LEN + 4)

enum {
     SESSION_CLOSED_REASON_OK,
     SESSION_CLOSED_REASON_NETWORK
};

enum {
     SESSION_MODE_LAN,
     SESSION_MODE_PUNCH,
     SESSION_MODE_RELAY,
     SESSION_MODE_DISCONNECTED
};


/*for session peer*/
struct ecPeerSessionInfo {
   /**
     * @brief Session state, can be 0: Ready, 1: Error
     *
     */

    int  state;
   /**
     * @brief Channel number
     *
     */
   int  nChannels;

   /**
     * @brief Session mode, can be
     *            SESSION_MODE_LAN,
     *            SESSION_MODE_PUNCH,
     *            SESSION_MODE_RELAY,
     *            SESSION_MODE_DISCONNECTED,
     */
    int  mode;

   /**
     * @brief Session role
     *
     */
    int  role;                      ///< 0: As a Client, 1: As a Device

   /**
     * @brief Remote peer ID of this session
     *
     */
    char remotePeerId[PEER_ID_LEN + 1];  ///< The ID of the other peer side.

   /**
     * @brief Remote peer IP address of this session.
     *
     */
    char remoteIp[PEER_ID_LEN + 1];      ///< The IP of remote peer

   /**
     * @brief total bytes of packets sent.
     *
     */
    uint64_t  bytesOfSent;

   /**
     * @brief total bytes of packets received.
     *
     */
    uint64_t  bytesofReceived;
};
typedef struct ecPeerSessionInfo ecPeerSessionInfo;

/**
 * Event handler for session.
 */
struct ecPeerSessionHandler {
    /**
     * @brief callback invoked right after package was received from session peer.
     *
     * @param
     *      sessionId        [in] The session ID.
     * @param
     *      channelId        [in] Channel Id of this session.
     * @param
     *      buf              [in] Buffer that stores received package data.
     * @param
     *      length           [in] The length of package data.
     *
     */
    void (*onData)(int sessionId, int channelId, void *buf, int length);

    /**
     * @brief callback invoked when specific session was closed.
     *
     * @param
     *      sessionId        [in] ID of session that was closed.
     * @param
     *      status           [in] The reason to close session.
     *
     */
    void (*onClosed)(int sessionId, int status);

};
typedef struct ecPeerSessionHandler ecPeerSessionHandler;


/**
 * Initializer for session peer
 */
struct ecPeerSessionInitializer {
     /**
     * @brief Used when to reject or accept the invite request from client.
     *
     * @param
     *      serviceId       [in] reserved, not used.
     * @param
     *      clientId        [in] client ID where the request is from.
     *
     * @return
     *      value NULL if the invite request is accept by device, else return session
     *      handler.
     */
    int (*onInvite)(const char* serviceId, const char* peerId);


    /**
     * @brief Used when to reject or accept the invite request from client.
     *
     * @param
     *      serviceId       [in] reserved, not used.
     * @param
     *      clientId        [in] client ID where the request is from.
     *
     * @return
     *      value NULL if the invite request is accept by device, else return session
     *      handler.
     */
    ecPeerSessionHandler* (*onSessionOpened)(int sessionId, int nChannels);
};
typedef struct ecPeerSessionInitializer ecPeerSessionInitializer;


/**
 * @brief Initialize peer session module.
 * @brief This function should be called before any of other session APIs.
 *        The total number of sessions allowed in device or client can not
 *        exceed 5.

 * @param
 *     nMaxSessions     [in] the max number of sessions allowed to created.
 *
 * @param
 *     initializer      [in] initializer handler to this peer.
 *
 * @return
 *     Error code.
 */
int ecPeerInitialize(int nMaxSessions, ecPeerSessionInitializer* initializer);

/**
 * @brief Destroy peer session module
 * @brief
 *
 */
void ecPeerDestroy(void);


/**
 * @brief Used by a client to actively launch a session with specific device.
 *
 * @brief With the help of carrier server, the session ID will be returned
 *        if session is established, then client and device can communite
 *        with each other for data deliver.
 * @brief The number of channels allowed in a session should not exceed 5.
 *
 * @param
 *      deviceId        [in] The ID of device that client request to connect.
 * @param
 *      serviceId       [in] reserved.
 * @param
 *      nChannels       [in] the number of channels for this session
 * @param
 *      handler         [in] handler to session to open.
 *
 * @return
 *     Session ID if return value >= 0.
 * @return
 *     Error code.
 */
int ecSessionOpen(const char *deviceId, const char *serviceId, int nChannels, ecPeerSessionHandler* handler);


/**
 * @brief Used by a device or client to write data to the other peer.
 *
 * @brief A device or a client uses this function to write data through a specific
 *        channel of the session.
 *
 * @param
 *      sessionId       [in] The ID of session to use for data writing.
 * @param
 *      channelId       [in] The channel ID in this session to write data.
 * @param
 *      buf             [in] The buffer to write.
 * @param
 *      length          [in] The length of the byte buffer, must be less than
 *                           ECSESS_MAX_PACKET_SIZE.
 *
 * @return
 *     The actual length of buffer to be written if write successfully.
 * @return
 *     Error code if return value < 0.
 */
int ecSessionWrite(int sessionId, int channelId, const void* buf, int length);

/**
 * @brief Used by a device or client to close a session.
 *
 * @brief A device or a client uses this function to close a specific session.
 *        if this session is no longer required.
 *
 * @param
 *     sessionId        [in] The ID of session to write data.
 *
 * @return
 *     Error code.
 */
int ecSessionClose(int sessionId);

/**
 * @brief Used by a device or client to acquire session information.
 *
 * @param
 *     sessionId        [in] The ID of session.
 * @param
 *     sessionInfo      [in] the buffer to store session information.
 *
 * @return
 *     Error code
 */
int ecSessionGetInfo(int sessionId, ecPeerSessionInfo* sessionInfo);

/**
 *@brief Used to get detailed description of error by error code.
 *
 *@pram
 *    ecCode            [in] error code
 *
 *@return
 *    String description of this error.
 */
const char* ecSessionGetErrString(int ecCode);

#ifdef __cplusplus
}
#endif

#endif ///< __ECSESSION_H__>


