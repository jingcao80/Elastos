//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_DCASYNCCHANNEL_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_DCASYNCCHANNEL_H__

#include "_Elastos.Droid.Internal.h"
#include <elastos/core/Object.h>
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/droid/internal/utility/AsyncChannel.h>

using Elastos::Droid::Internal::Utility::AsyncChannel;
using Elastos::Droid::Net::ILinkProperties;
using Elastos::Droid::Net::INetworkCapabilities;
using Elastos::Droid::Net::IProxyInfo;
using Elastos::Droid::Os::IMessage;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

/**
 * AsyncChannel to a DataConnection
 */
class DcAsyncChannel
    : public AsyncChannel
    , public IDcAsyncChannel
{
public:
    CAR_INTERFACE_DECL()

    // Convert cmd to string or null if unknown
    static CARAPI CmdToString(
        /* [in] */ Int32 cmd,
        /* [out] */ String* result);

    CARAPI constructor(
        /* [in] */ IDataConnection* dc,
        /* [in] */ const String& logTag);

    /**
     * Request if the state machine is in the inactive state.
     * Response {@link #rspIsInactive}
     */
    CARAPI ReqIsInactive();

    /**
     * Evaluate RSP_IS_INACTIVE.
     *
     * @return true if the state machine is in the inactive state.
     */
    CARAPI RspIsInactive(
        /* [in] */ IMessage* response,
        /* [out] */ Boolean* result);

    /**
     * @return true if the state machine is in the inactive state
     * and can be used for a new connection.
     */
    CARAPI IsInactiveSync(
        /* [out] */ Boolean* result);

    /**
     * Request the Connection ID.
     * Response {@link #rspCid}
     */
    CARAPI ReqCid();

    /**
     * Evaluate a RSP_GET_CID message and return the cid.
     *
     * @param response Message
     * @return connection id or -1 if an error
     */
    CARAPI RspCid(
        /* [in] */ IMessage* response,
        /* [out] */ Int32* result);

    /**
     * @return connection id or -1 if an error
     */
    CARAPI GetCidSync(
        /* [out] */ Int32* result);

    /**
     * Request the connections ApnSetting.
     * Response {@link #rspApnSetting}
     */
    CARAPI ReqApnSetting();

    /**
     * Evaluate a RSP_APN_SETTING message and return the ApnSetting.
     *
     * @param response Message
     * @return ApnSetting, maybe null
     */
    CARAPI RspApnSetting(
        /* [in] */ IMessage* response,
        /* [out] */ IApnSetting** result);

    /**
     * Get the connections ApnSetting.
     *
     * @return ApnSetting or null if an error
     */
    CARAPI GetApnSettingSync(
        /* [out] */ IApnSetting** result);

    /**
     * Request the connections LinkProperties.
     * Response {@link #rspLinkProperties}
     */
    CARAPI ReqLinkProperties();

    /**
     * Evaluate RSP_GET_LINK_PROPERTIES
     *
     * @param response
     * @return LinkProperties, maybe null.
     */
    CARAPI RspLinkProperties(
        /* [in] */ IMessage* response,
        /* [out] */ ILinkProperties** result);

    /**
     * Get the connections LinkProperties.
     *
     * @return LinkProperties or null if an error
     */
    CARAPI GetLinkPropertiesSync(
        /* [out] */ ILinkProperties** result);

    /**
     * Request setting the connections LinkProperties.HttpProxy.
     * Response RSP_SET_LINK_PROPERTIES when complete.
     */
    CARAPI ReqSetLinkPropertiesHttpProxy(
        /* [in] */ IProxyInfo* proxy);

    /**
     * Set the connections LinkProperties.HttpProxy
     */
    CARAPI SetLinkPropertiesHttpProxySync(
        /* [in] */ IProxyInfo* proxy);

    /**
     * Request the connections NetworkCapabilities.
     * Response {@link #rspNetworkCapabilities}
     */
    CARAPI ReqNetworkCapabilities();

    /**
     * Evaluate RSP_GET_NETWORK_CAPABILITIES
     *
     * @param response
     * @return NetworkCapabilites, maybe null.
     */
    CARAPI RspNetworkCapabilities(
        /* [in] */ IMessage* response,
        /* [out] */ INetworkCapabilities** result);

    /**
     * Get the connections NetworkCapabilities.
     *
     * @return NetworkCapabilities or null if an error
     */
    CARAPI GetNetworkCapabilitiesSync(
        /* [out] */ INetworkCapabilities** result);

    /**
     * Response RSP_RESET when complete
     */
    CARAPI ReqReset();

    /**
     * Bring up a connection to the apn and return an AsyncResult in onCompletedMsg.
     * Used for cellular networks that use Acesss Point Names (APN) such
     * as GSM networks.
     *
     * @param apnContext is the Access Point Name to bring up a connection to
     * @param initialMaxRetry the number of retires for initial bringup.
     * @param profileId for the conneciton
     * @param onCompletedMsg is sent with its msg.obj as an AsyncResult object.
     *        With AsyncResult.userObj set to the original msg.obj,
     *        AsyncResult.result = FailCause and AsyncResult.exception = Exception().
     */
    CARAPI BringUp(
        /* [in] */ IApnContext* apnContext,
        /* [in] */ Int32 initialMaxRetry,
        /* [in] */ Int32 profileId,
        /* [in] */ Int32 rilRadioTechnology,
        /* [in] */ Boolean retryWhenSSChange,
        /* [in] */ IMessage* onCompletedMsg);

    /**
     * Tear down the connection through the apn on the network.
     *
     * @param onCompletedMsg is sent with its msg.obj as an AsyncResult object.
     *        With AsyncResult.userObj set to the original msg.obj.
     */
    CARAPI TearDown(
        /* [in] */ IApnContext* apnContext,
        /* [in] */ const String& reason,
        /* [in] */ IMessage* onCompletedMsg);

    /**
     * Tear down the connection through the apn on the network.  Ignores refcount and
     * and always tears down.
     *
     * @param onCompletedMsg is sent with its msg.obj as an AsyncResult object.
     *        With AsyncResult.userObj set to the original msg.obj.
     */
    CARAPI TearDownAll(
        /* [in] */ const String& reason,
        /* [in] */ IMessage* onCompletedMsg);

    /**
     * @return connection id
     */
    CARAPI GetDataConnectionIdSync(
        /* [out] */ Int32* result);

    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

    CARAPI GetPcscfAddr(
        /* [out, callee] */ ArrayOf<String>** result);

private:
    CARAPI IsCallerOnDifferentThread(
        /* [out] */ Boolean* result);

    CARAPI Log(
        /* [in] */ const char *fmt, ...);

    static CARAPI_(AutoPtr<ArrayOf<String> >) InitCmdToString();

private:
    static const Boolean DBG;

    String mLogTag;

    AutoPtr<IDataConnection> mDc;

    Int64 mDcThreadId;

    static const Int32 CMD_TO_STRING_COUNT;

    static AutoPtr<ArrayOf<String> > sCmdToString;
};

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_DCASYNCCHANNEL_H__
