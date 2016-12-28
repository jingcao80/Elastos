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

#ifndef __ECCORE_H__
#define __ECCORE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#ifndef _ECERR
#define _ECERR(e) ((int)e)
#endif

/* new error code */
#define ECCORE_E_NOERROR            _ECERR(0)

#define ECCORE_E_UNKNOWN_CARRIER    _ECERR(0x80001001)
#define ECCORE_E_BAD_CARRIER        _ECERR(0x80001002)
#define ECCORE_E_BAD_HTTPS_SERVER   _ECERR(0x80001003)

#define ECCORE_E_ALREADY_STARTED    _ECERR(0x80001005)
#define ECCORE_E_NO_PEER_START      _ECERR(0x80001006)

#define ECCORE_E_CLIENT_OFFLINE     _ECERR(0x80001020)
#define ECCORE_E_DEVICE_OFFLINE     _ECERR(0x80001021)
#define ECCORE_E_CLIENT_EXIST       _ECERR(0x80001022)
#define ECCORE_E_NO_CLIENT          _ECERR(0x80001023)
#define ECCORE_E_NO_DEVICE          _ECERR(0x80001024)
#define ECCORE_E_BAD_SIGNATURE      _ECERR(0x80001025)
#define ECCORE_E_BAD_CREDENTIAL     _ECERR(0x80001026)
#define ECCORE_E_BAD_CLIENT         _ECERR(0x80001027)
#define ECCORE_E_BAD_APP_ID         _ECERR(0x80001028)
#define ECCORE_E_PERM_DENIED        _ECERR(0x80001029)
#define ECCORE_E_BAD_PHONENO        _ECERR(0x80001030)
#define ECCORE_E_BAD_VERIFY_CODE    _ECERR(0x80001031)
#define ECCORE_E_NETWORK            _ECERR(0x80001032)
#define ECCORE_E_LIMIT              _ECERR(0x80001033)
#define ECCORE_E_NO_APP_ID          _ECERR(0X80001034)
#define ECCORE_E_NO_APP_KEY         _ECERR(0x80001035)
#define ECCORE_E_BAD_CLIENT_ID      _ECERR(0x80001036)
#define ECCORE_E_BAD_CLIENT_KEY     _ECERR(0x80001037)

#define ECCORE_E_BAD_PARAM          _ECERR(0x80001040)
#define ECCORE_E_OOM                _ECERR(0x80001041)
#define ECCORE_E_NOT_IMPLEMENTED    _ECERR(0x80001042)
#define ECCORE_E_TIMEOUT            _ECERR(0x80001043)
#define ECCORE_E_SOCKET             _ECERR(0x80001044)
#define ECCORE_E_HTTPS              _ECERR(0x80001045)
#define ECCORE_E_JSON               _ECERR(0x80001046)

#define ECCORE_E_UNKNOWN            _ECERR(0x80001099)

#define _STATUS(s) ((uint32_t)s)

#define MAX_HOST_NAME             255
#define MAX_NAME_LEN              32
#define PEER_ID_LEN               32
#define PEER_ID_TLEN             (PEER_ID_LEN + 4)

enum {
    DEVICE_EVENT_ONLINE          = (int)0x01, // Device becomes online.
    DEVICE_EVENT_OFFLINE         = (int)0x02, // Device becomes offline.
    DEVICE_EVENT_PAIRING_DENIED  = (int)0x03, // Pairing to device was denied.
    DEVICE_EVENT_HOST_PAIRED     = (int)0x04, // Host pairing to device is authorized.
    DEVICE_EVENT_GUEST_PAIRED    = (int)0x05, // Guest pairing to device is authorized.
    DEVICE_EVENT_UNKNOWN         = (int)0x06,
    DEVICE_STATUS_BUTT
};

enum {
    SUBSCRIBER_MODE_DENY         = (int)0x0,
    SUBSCRIBER_MODE_GUEST        = (int)0x01,
    SUBSCRIBER_MODE_HOST         = (int)0x02,
};

enum {
    OFFLINE_REASON_NETWORK       = (int)0,
    OFFLINE_REASON_BUTT
};

enum {
    PAIR_RESULT_OK               = (int)0x0,
    PAIR_RESULT_WRONG_CREDENTIAL = (int)0x01,
    PAIR_RESULT_ALREADY_PAIR     = (int)0x02,
    PAIR_RESULT_NO_DEVICE        = (int)0x03,
    PAIR_RESULT_DEVICE_OFFLINE   = (int)0x04,
    PAIR_RESULT_DENY             = (int)0x05,
    PAIR_RESULT_UNKNOWN          = (int)0x06,
    PAIR_RESULT_BUTT
};

enum {
    CREDENTIAL_OK                = (int)0x0,
    CREDENTIAL_E_BAD             = (int)0x01,
    CREDENTIAL_E_LIMIT           = (int)0x02,
    CREDENTIAL_E_UNKNOWN         = (int)0x03,
    CREDENTIAL_E_BUTT
};

enum {
    MESSAGE_ONLINE               = (int)0x0,
    MESSAGE_ANYWAY               = (int)0x01,
    MESSAGE_BUTT
};

/**
 * Context to be necessary for device/client to run.
 */
struct ecContext {
    /**
     * @brief turn hostname or IP address
     *
     */
    char turnServer[MAX_HOST_NAME+1];

    /**
     * @brief turn server port
     *
     */
    int  turnPort;
};
typedef struct ecContext ecContext;

/**
 * Handler for device.
 */
struct ecDeviceHandler {
    /**
     * @brief Used when device is about to set preference information.
     *
     * @param
     *      key             [in] The key of preference item, only support:
     *                       "carrier.server.host",
     *
     * @param
     *      value           [in] The value of preference item.
     *
     * @return
     *      On success, value 0 is returned.
     *      On error, value -1 is returned.
     *
     */
    int (*setPreference)(const char* key, const char* value);

    /**
     * @brief Used when device is about to get preference information.
     *
     * @param
     *      key             [in] The key of preference item, should be:
     *                       "carrier.server.host",
     *
     * @param
     *      value           [out] The buffer to store the preference item value.
     * @param
     *      length          [in]  The buffer length.
     *
     * @return
     *      On success, value 0 is returned, and the specific preference item
     *  value is stored on given buffer.
     *      On error, value -1 is returned.
     *
     */
    int (*getPreference)(const char* key, char* value, int length);

    /**
     * @brief Used when device is connecting to carrier host.
     *
     * @param
     *      context:        [in] The context for device.
     *
     */
    void (*onOnline)(ecContext* context);
    /**
     * @brief Used when device disconnect with carrier host, or conduct unregister.
     *
     * @param
     *      status          [in] the reason that device become offline.
     *
     */
    void (*onOffline)(int status);

    /**
     * @brief Used when client want to pair the device by password.
     *
     * @param
     *     clientId         [in] Client ID
     *
     * @pram
     *     crendential      [in] Credential from client.
     *
     * @return
     *     return value should be one of following values:
     *                      CREDENTIAL_OK,
     *                      CREDENTIAL_E_BAD,
     *                      CREDENTIAL_E_LIMIT.
     *
     */
    int (*onCredential)(const char* clientId, const char* credential);

    /**
     * @brief Used when device received short message from client.
     *
     * @param
     *      serviceId       [in] reserved, not used.
     * @param
     *      clientId        [in] client ID where the message is from.
     * @param
     *      message         [in] buffer to message body.
     * @param
     *      msgSize         [in] message size.
     *
     */
    void (*onMessage)(const char* serviceId, const char* clientId, void* message, int msgSize);
};

/**
 * Handler for client.
 */
struct ecClientHandler {
    /**
     * @brief Used when device is about to set preference information.
     *
     * @param
     *      key             [in] The key of preference item, currently support:
     *                       "carrier.server.host",
     *                       "api.carrier.server.host",
     *                       "client.Id",
     *                       "client.key",
     *                       "app.Id",
     *                       "app.key"
     *
     * @param
     *      value           [in] The value of preference item.
     *
     * @return
     *      On success, value 0 is returned.
     *      On error, value -1 is returned.
     *
     */
    int (*setPreference)(const char* key, const char* value);

    /**
     * @brief Used when device is about to get preference information.
     *
     * @param
     *      key             [in] The key of preference item, currently support:
     *                       "carrier.server.host",
     *                       "api.carrier.server.host",
     *                       "client.Id",
     *                       "client.key",
     *                       "app.Id",
     *                       "app.key"
     *
     * @param
     *      value           [out] The buffer to store the preference item value.
     * @param
     *      length          [in]  The buffer length.
     *
     * @return
     *      On success, value 0 is returned, and the specific preference item
     *  value is stored on given buffer.
     *      On error, value -1 is returned.
     *
     */
    int (*getPreference)(const char* key, char* value, int length);

   /**
     * @brief Used when client is connecting to carrier host.
     *
     * @param
     *      context:        [in] The context for device.
     *
     */

    void (*onOnline)(ecContext* context);

    /**
     * @brief Used when client disconnect with carrier host.
     *        or conduct unregister.
     *
     * @param
     *      status            [in] the reason that device become offline.
     *
     */
    void (*onOffline)(int status);

    /**
     * @brief Get a pair request for a device from other client.
     *
     * @param
            deviceId           [in] The device id of the device want to pair.
     * @param
            deviceName         [in] The name of the device want to pair.
     * @param
            serviceId          [in] service ID. reserved, not used.
     * @param
            clientId           [in] The id of the client sent the request.
     * @param
            clientName         [in] The name of the client sent the request.
     * @param
            helloMsg           [in] The hello message of the client sent the request.
     *
     */
    void (*onPairRequest)(const char* deviceId,  const char* deviceName,
                       const char* serviceId,
                       const char* clientId, const char* clientName,
                       const char* helloMsg);

    /**
     * @brief The host client confirmed the pair request of this client.
     *
     * @param
            deviceId           [in] The device id of the device want to pair.
     * @param
            serviceId          [in] Service ID. reserved, not used.
     * @param
            mode               [in] The target mode of guest request to the device.
     *                           If status is not zero, mode is invalid value, zero.
     * @param
     *      status             [in] Error code for pair confirming.
     *
     */
    void (*onPairConfirmed)(const char* deviceId, const char* serviceId, int mode, int status);

    /**
     * @brief The device status of this client changed.
     *
     * @param
            deviceId           [in] The device id of the device of this client.
     * @param
            event              [in] The event from device.
     *
     */
    void (*onDeviceEvent)(const char* deviceId, int event);

    /**
     * @brief Used when client received short message from device.
     *
     * @param
     *      serviceId          [in] reserved, not used.
     * @param
     *      deviceId           [in] device ID where the message is from.
     * @param
     *      message            [in] buffer to message body.
     * @param
     *      msgSize            [in] message size.
     *
     */
    void (*onMessage)(const char* serviceId, const char* deviceId, void* message, int msgSize);
};

typedef struct ecDeviceHandler ecDeviceHandler;
typedef struct ecClientHandler ecClientHandler;

/**
 * @brief information for a device.
 */
struct ecDeviceInfo {
   /**
     * @brief Device ID
     *
     */
    char deviceId[PEER_ID_LEN+1];

   /**
     * @brief Device name
     *
     */
    char deviceName[PEER_ID_LEN+1];

   /**
     * @brief Device display name
     *
     */
    char displayName[PEER_ID_LEN+1];

   /**
     * @brief Device status
     *
     */
    int status;

   /**
     * @brief Device mode
     *
     */
    int mode;

   /**
     * @brief Device type.
     *
     */
    int type;
};

/**
 * @brief information for pairing subscriber.
 */
struct ecSubscriberInfo {
   /**
     * @brief Client ID
     *
     */
    char clientId[PEER_ID_LEN+1];

    /**
     * @brief Client name
     *
     */
    char clientName[PEER_ID_LEN+1];

   /**
     * @brief Subscribered mode
     *
     */
    int mode;

};

typedef struct ecDeviceInfo     ecDeviceInfo;
typedef struct ecSubscriberInfo ecSubscriberInfo;



/**
 * @brief Start this device.
 *
 * @brief A device may use this function to communicate with carrier server.
 *
 * @param
 *     deviceId        [in] The ID for this device.
 * @param
 *     serialNo        [in] The device serial number.
 * @param
 *     handler         [in] The callback handler to this device.
 *
 * @return
 *     On success, value 0 is returned, otherwise, following error code is returned:
 *     ECCORE_E_BAD_PARAM:       arguments is invalid;
 *     ECCORE_E_ALREADY_STARTED: device is already started;
 *     ECCORE_E_BAD_CARRIER:     bad carrier hostname or port;
 *     ECCORE_E_SOCKET:          socket-related problem;
 *
 */
int ecDeviceStart(const char *deviceId, const char *serialNo, ecDeviceHandler* handler);

/**
 * @brief Stop this device.
 *
 * @brief A device may use this function to disconnect with carrier server.
 *
 * @return
 *     always succeed and value 0 is returned;
 */
int ecDeviceStop(void);

/**
 * @brief Start this client.
 *
 * @brief A client may use this function to communicate with carrier server.
 *
 * @param
 *      handler          [in] The callback handler to this client.
 *
 * @return
 *     On success, value 0 is returned, otherwise, following error code is returned:
 *     ECCORE_E_BAD_PARAM:       arguments is invalid;
 *     ECCORE_E_ALREADY_STARTED: client is already started;
 *     ECCORE_E_BAD_CARRIER:     bad carrier hostname or port;
 *     ECCORE_E_NO_APP_ID:       invalid app id;
 *     ECCORE_E_NO_APP_KEY:      invalid app key;
 *     ECCORE_E_BAD_CLIENT_ID:   bad client id;
 *     ECCORE_E_BAD_CLIENT_KEY:  bad client key;
 *     ECCORE_E_HTTPS:           https-related problem;
 *
 */
int ecClientStart(ecClientHandler* handler);

/**
 * @brief Stop this client running.
 *
 * @brief A client may use this function to stop working.
 *
 * @return
 *     always succeed and value 0 is returned;
 */
int ecClientStop(void);

/**
 *
 * @brief Get current client Id.
 *
 * @brief A client app may use this function to get current client Id.
 *
 * @param
 *     clientId         [out] The buffer to store the value of client Id.
 *
 * @param
 *     length           [in] The buffer length;
 *
 * @return
 *     On success, value 0 is returned, otherwiase, a negative value is returned:
 *
 */
int ecGetClientId(char* clientId, int length);

/**
 * @brief Set this client name.
 *
 * @param
 *     clientName       [in] The client name.
 *
 * @return
 *     On sucess, value 0 is returned, otherwise, following error code is returned:
 *     ECCORE_E_BAD_PARAM:      argument is invalid;
 *     ECCORE_E_CLIENT_OFFLINE: client currently is offline;
 *     ECCORE_E_UNKNOWN:        unknown network problem.
 *
 */
int ecSetClientName(const char *clientName);

/**
 * @brief Client pair with the device with specific pair mode.
 *
 * @brief A client may use this function to pair with a specific device.
 * @brief After pair is conducted, the client can access that device.
 *
 * @param
 *      deviceId         [in] The device ID of the device that client want to pair with.
 * @param
 *      serviceId        [in] Reserved, not used.
 * @param
 *      mode             [in] Pairing mode, should be:
 *                             SUBSCRIBER_MODE_GUEST,
 *                             SUBSCRIBER_MODE_HOST,
 * @param
 *      options          [in] Pasword if pairing with host mode, otherwise just hello message.
 *
 * @param
 *      alreadyPaired    [out] value 1 if pairing is already done, otherwise will be value 0.
 *
 * @return
 *     On success, value 0 is returned, otherwise, following error code is returned:
 *     ECCORE_E_BAD_PARAM:      argument is invalid;
 *     ECCORE_E_CLIENT_OFFLINE: client currently is offline;
 *     ECCORE_E_UNKNOWN:        unknown network problem.
 *     ECCORE_E_LIMIT:          exceed the maximum try times;
 *     ECCORE_E_PERM_DENIED:    pairing is denied by host or device;
 *     ECCORE_E_NO_DEVICE:      No such device;
 *     ECCORE_E_BAD_CREDENTIAL: Bad credential;
 *     ECCORE_E_DEVICE_OFFLINE: Remote device is offline;
 *
 */
int ecPair(const char *deviceId, const char *serviceId, int mode, const char *options, int* alreadyPaired);

/**
 * @brief Client unpair the device.
 *
 * @brief A client may use this function to unpair a specific device.
 * @brief After this call, the client can not access that device.
 *
 * @param
 *     deviceId          [in] The device ID of the device that client want to unpair.
 * @param
 *     serviceId         [in] Reserved, not used.
 *
 * @return
 *     On success, value 0 is returned, otherwise, following error code is returned:
 *     ECCORE_E_BAD_PARAM:      argument is invalid;
 *     ECCORE_E_CLIENT_OFFLINE: client currently is offline;
 *     ECCORE_E_UNKNOWN:        unknown network problem.
 *     ECCORE_E_NO_DEVICE:      No such device.
 *
 */
int ecUnpair(const char *deviceId, const char *serviceId);


/**
 * @brief Make a confirm for pairing request from client, which can accept or reject
 *        the request according to 'mode' parameter.
 *
 *
 * @param
 *      deviceId        [in] The id of the device wanted to pair.
 * @param
 *      clientId        [in] The id of the client sent the request.
 * @param
 *      serviceId       [in] Reserved, not used.
 * @param
 *      mode            [in] The pairing mode to confirm, should be one of them:
 *                                  SUBSCRIBER_MODE_DENY,
 *                                  SUBSCRIBER_MODE_GUEST,
 *                                  SUBSCRIBER_MODE_HOST
 *
 * @return
 *     On success, value 0 is returned, otherwise, following error code is returned:
 *     ECCORE_E_BAD_PARAM:      argument is invalid;
 *     ECCORE_E_CLIENT_OFFLINE: client currently is offline;
 *     ECCORE_E_UNKNOWN:        unknown network problem.
 *     ECCORE_E_OOM:            Out of memory;
 *
 */
int ecPairConfirm(const char *deviceId, const char* serviceId, const char *clientId, int mode);

/**
 * @brief to get all devices' info paired by this client.
 *
 * @brief A client may use this function to query the devices paired by this client.
 *
 * @param
 *     devices           [out] The devices array queried by this client,
 *                             after using this, caller must use 'free' API to
 *                             release memory of devices' info.
 * @param
 *     nDevices          [out] The number of devices' info.
 *
 * @return
 *     On success, value 0 is returned, otherwise, following error code is returned:
 *     ECCORE_E_BAD_PARAM:      argument is invalid;
 *     ECCORE_E_CLIENT_OFFLINE: client currently is offline;
 *     ECCORE_E_UNKNOWN:        unknown network problem.
 *     ECCORE_E_OOM:            Out of memory;
 *
 *     Note: after calling this API, memory pointed by devices must be freed.
 */
int ecGetDevices(ecDeviceInfo **devices, int *nDevices);

/**
 * @brief Query all clients paired with the specific device.
 *
 * @brief A client may use this function to query all clients paired with specific device.
 *
 * @param
 *     deviceId         [in] The ID of a device to be queried.
 * @param
 *     subscribers         [out] The client array.
 * @param
 *     nSubscribers     [out] The count of the clients.
 *
 * @return
 *     On success, value 0 is returned, otherwise, following error code is returned:
 *     ECCORE_E_BAD_PARAM:      argument is invalid;
 *     ECCORE_E_CLIENT_OFFLINE: client currently is offline;
 *     ECCORE_E_UNKNOWN:        unknown network problem.
 *     ECCORE_E_OOM:            Out of memory;
 *
 *     Note: after calling this API, memory pointed by devices must be freed.
 */
int ecGetSubscribers(const char *deviceId, ecSubscriberInfo **subscribers, int *nSubscribers);

/**
 * @brief Set pairing mode of the specific client and device.
 *
 * @brief After calling this function, the pairing mode between client and device
 *        will change to be given mode.
 *
 * @param
 *     deviceId         [in] The ID of a device to be operated.
 * @param
 *     clientId         [in] The client ID.
 * @param
 *     mode             [in] The pairing mode to be set, can be:
 *                           SUBSCRIBER_MODE_GUEST,
 *                           SUBSCRIBER_MODE_HOST
 *
 * @return
 *     On success, value 0 is returned, otherwise, following error code is returned:
 *     ECCORE_E_BAD_PARAM:      argument is invalid;
 *     ECCORE_E_CLIENT_OFFLINE: client currently is offline;
 *     ECCORE_E_UNKNOWN:        unknown network problem.
 *     ECCORE_E_PERM_DENIED:    update is denied by host client;
 *     ECCORE_E_NO_DEVICE:      No such device.
 *
 */
int ecSetSubscriberMode(const char *deviceId, const char *clientId, int mode);

/**
 *
 * @brief Remove the pairing of the speicific client and device,
 *
 * @brief After calling this function, that client can no longer access that device.
 *
 * @param
 *     deviceId         [in] The ID of a device to be operated.
 * @param
 *     clientId         [in] The client to be removed from pairing.
 *
 * @return
 *     On success, value 0 is returned, otherwise, following error code is returned:
 *     ECCORE_E_BAD_PARAM:      argument is invalid;
 *     ECCORE_E_CLIENT_OFFLINE: client currently is offline;
 *     ECCORE_E_UNKNOWN:        unknown network problem.
 *     ECCORE_E_PERM_DENIED:    update is denied by host client;
 *     ECCORE_E_NO_DEVICE:      No such device.
 *     ECCORE_E_NO_CLIENT:      No schu client.
 *
 */
int ecRemoveSubscriber(const char* deviceId, const char* clientId);

/**
 * @brief To set the name of specific device.
 *
 * @param
 *     deviceId         [in] The device ID of a device to be set.
 * @param
 *     deviceName       [in] The name to be set, the MAX length is PEER_ID_LEN.
 *
 * @return
 *     On success, value 0 is returned, otherwise, following error code is returned:
 *     ECCORE_E_BAD_PARAM:      argument is invalid;
 *     ECCORE_E_CLIENT_OFFLINE: client currently is offline;
 *     ECCORE_E_UNKNOWN:        unknown network problem.
 *     ECCORE_E_PERM_DENIED:    update is denied by host client;
 *     ECCORE_E_NO_DEVICE:      No such device.
 *
 */
int ecSetDeviceName(const char *deviceId, const char *deviceName);

/**
 * @brief To get the name of speicific device.
 *
 * @brief This name is binding with device id, so it is unique.
 *
 * @param
 *     deviceId         [in] The device ID of a device to be set.
 * @param
 *     deviceName       [in] The buffer to store the device name.
 * @param
 *     nameLen          [in] the length of buffer
 *
 * @return
 *     On success, value 0 is returned, otherwise, following error code is returned:
 *     ECCORE_E_BAD_PARAM:      argument is invalid;
 *     ECCORE_E_CLIENT_OFFLINE: client currently is offline;
 *     ECCORE_E_UNKNOWN:        unknown network problem.
 *     ECCORE_E_PERM_DENIED:    update is denied by host client;
 *     ECCORE_E_NO_DEVICE:      No such device.
 *
 */
int ecGetDeviceName(const char *deviceId, char *deviceName, int nameLen);

/**
 * @brief To set display name of specific device.
 *
 * @brief The display name is just used for this client.
 *
 * @param
 *     deviceId         [in] The device ID of a device to be set.
 * @param
 *     displayName      [in] The name to be set, the maximum length is PEER_ID_LEN.
 *
 * @return
 *     On success, value 0 is returned, otherwise, following error code is returned:
 *     ECCORE_E_BAD_PARAM:      argument is invalid;
 *     ECCORE_E_CLIENT_OFFLINE: client currently is offline;
 *     ECCORE_E_UNKNOWN:        unknown network problem.
 *     ECCORE_E_PERM_DENIED:    update is denied by host client;
 *     ECCORE_E_NO_DEVICE:      No such device.
 *
 */
int ecSetDeviceDisplayName(const char *deviceId, const char *displayName);

/**
 * @brief To send a short message to other site with specific ID.
 *
 * @param
 *     peerId           [in] The device ID  or client ID of message destination.
 * @param
 *     serviceId        [in] reserved, not used.
 * @param
 *     message          [in] message body.
 * @param
 *     msgSize          [in] the length of message body.
 * @param
 *     deliverFlag      [in] the flag indicating how to deliver the message:
 *                      MESSAGE_ONLINE: deliver the message only when the recipient is online;
 *                      MESSAGE_ANYWAY: deliver the message whether the recipient is online or not.
 * @param
 *     lifetime         [in] the lifetime of the message in seconds.
 *
 * @return
 *     On success, value 0 is returned, otherwise, following error code is returned:
 *     ECCORE_E_BAD_PARAM:      argument is invalid;
 *     ECCORE_E_CLIENT_OFFLINE:  client currently is offline.
 *     ECCORE_E_DEVICE_OFFLINE:  device currently is offline.
 *
 */
int ecMessage(const char* peerId, const char* serviceId, void* message, int msgSize, int deliverFlag, int lifetime);

/**
 * @brief Request to get verify code by SMS messae or email.
 * @brief A few seconds after calling this function, user will receive a SMS
 *        message or email containing verify code.
 *
 * @param
 *     userId           [in] The user Id, can be phone number or email address,
 *                       currently only support for phone number.
 *
 * @return
 *     On success, value 0 is returned, otherwise, following error code is returned:
 *     ECCORE_E_BAD_PARAM:      argument is invalid;
 *     ECCORE_E_CLIENT_OFFLINE: client currently is offline;
 *     ECCORE_E_UNKNOWN:        unkown network problem;
 *     ECCORE_E_PERM_DENIED     denied by carrier
 *     ECCORE_E_BAD_PHONENO     invalid user Id.
 *
 */
int ecGetVerifyCode(const char *userId);

/**
 * @brief To sign in with specific verify code for given user.
 *
 * @brief After calling this function, the user will be bound with this client.
 *
 * @param
 *     userId           [in] The user Id, can be phone number or email address.
 *                      currently only support for phone number.
 * @param
 *     verifyCode       [in] The verify code
 *
 * @return
 *     On success, value 0 is returned, otherwise, following error code is returned:
 *     ECCORE_E_BAD_PARAM:      argument is invalid;
 *     ECCORE_E_CLIENT_OFFLINE: client currently is offline;
 *     ECCORE_E_UNKNOWN:        unkown network problem;
 *
 */
int ecSignin(const char *userId, const char *verifyCode);

/**
 * @brief To sign out for current user.
 * @brief After calling this function, the user is no more bound with this client.
 *
 * @return
 *     On success, value 0 is returned, otherwise, following error code is returned:
 *     ECCORE_E_CLIENT_OFFLINE: client currently is offline;
 *
 */
int ecSignout(void);

/**
 *@brief Used to get detailed description of error by error code.
 *
 *@param
 *    ecCode            [in] error code
 *
 *@return
 *    String description of this error.
 */
const char* ecGetErrString(int ecCode);

#ifdef __cplusplus
}
#endif

#endif ///< __ECCORE_H__>
