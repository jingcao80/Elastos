
#include "elastos/droid/internal/telephony/dataconnection/DcAsyncChannel.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

CAR_INTERFACE_IMPL(DcAsyncChannel, AsyncChannel, IDcAsyncChannel)

const Boolean DcAsyncChannel::DBG = false;
const Int32 DcAsyncChannel::CMD_TO_STRING_COUNT = RSP_RESET - BASE + 1;
AutoPtr<ArrayOf<String> > DcAsyncChannel::sCmdToString = InitCmdToString();

ECode DcAsyncChannel::CmdToString(
    /* [in] */ Int32 cmd,
    /* [out] */ String* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        cmd -= BASE;
        if ((cmd >= 0) && (cmd < sCmdToString.length)) {
            return sCmdToString[cmd];
        } else {
            return AsyncChannel.cmdToString(cmd + BASE);
        }

#endif
}

ECode DcAsyncChannel::constructor(
    /* [in] */ IDataConnection* dc,
    /* [in] */ const String& logTag)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mDc = dc;
        mDcThreadId = mDc.getHandler().getLooper().getThread().getId();
        mLogTag = logTag;

#endif
}

ECode DcAsyncChannel::ReqIsInactive()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        sendMessage(REQ_IS_INACTIVE);
        if (DBG) log("reqIsInactive");

#endif
}

ECode DcAsyncChannel::RspIsInactive(
    /* [in] */ IMessage* response,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        boolean retVal = response.arg1 == 1;
        if (DBG) log("rspIsInactive=" + retVal);
        return retVal;

#endif
}

ECode DcAsyncChannel::IsInactiveSync(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        boolean value;
        if (isCallerOnDifferentThread()) {
            Message response = sendMessageSynchronously(REQ_IS_INACTIVE);
            if ((response != null) && (response.what == RSP_IS_INACTIVE)) {
                value = rspIsInactive(response);
            } else {
                log("rspIsInactive error response=" + response);
                value = false;
            }
        } else {
            value = mDc.getIsInactive();
        }
        return value;

#endif
}

ECode DcAsyncChannel::ReqCid()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        sendMessage(REQ_GET_CID);
        if (DBG) log("reqCid");

#endif
}

ECode DcAsyncChannel::RspCid(
    /* [in] */ IMessage* response,
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        int retVal = response.arg1;
        if (DBG) log("rspCid=" + retVal);
        return retVal;

#endif
}

ECode DcAsyncChannel::GetCidSync(
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        int value;
        if (isCallerOnDifferentThread()) {
            Message response = sendMessageSynchronously(REQ_GET_CID);
            if ((response != null) && (response.what == RSP_GET_CID)) {
                value = rspCid(response);
            } else {
                log("rspCid error response=" + response);
                value = -1;
            }
        } else {
            value = mDc.getCid();
        }
        return value;

#endif
}

ECode DcAsyncChannel::ReqApnSetting()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        sendMessage(REQ_GET_APNSETTING);
        if (DBG) log("reqApnSetting");

#endif
}

ECode DcAsyncChannel::RspApnSetting(
    /* [in] */ IMessage* response,
    /* [out] */ IApnSetting** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        ApnSetting retVal = (ApnSetting) response.obj;
        if (DBG) log("rspApnSetting=" + retVal);
        return retVal;

#endif
}

ECode DcAsyncChannel::GetApnSettingSync(
    /* [out] */ IApnSetting** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        ApnSetting value;
        if (isCallerOnDifferentThread()) {
            Message response = sendMessageSynchronously(REQ_GET_APNSETTING);
            if ((response != null) && (response.what == RSP_GET_APNSETTING)) {
                value = rspApnSetting(response);
            } else {
                log("getApnSetting error response=" + response);
                value = null;
            }
        } else {
            value = mDc.getApnSetting();
        }
        return value;

#endif
}

ECode DcAsyncChannel::ReqLinkProperties()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        sendMessage(REQ_GET_LINK_PROPERTIES);
        if (DBG) log("reqLinkProperties");

#endif
}

ECode DcAsyncChannel::RspLinkProperties(
    /* [in] */ IMessage* response,
    /* [out] */ ILinkProperties** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        LinkProperties retVal = (LinkProperties) response.obj;
        if (DBG) log("rspLinkProperties=" + retVal);
        return retVal;

#endif
}

ECode DcAsyncChannel::GetLinkPropertiesSync(
    /* [out] */ ILinkProperties** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        LinkProperties value;
        if (isCallerOnDifferentThread()) {
            Message response = sendMessageSynchronously(REQ_GET_LINK_PROPERTIES);
            if ((response != null) && (response.what == RSP_GET_LINK_PROPERTIES)) {
                value = rspLinkProperties(response);
            } else {
                log("getLinkProperties error response=" + response);
                value = null;
            }
        } else {
            value = mDc.getCopyLinkProperties();
        }
        return value;

#endif
}

ECode DcAsyncChannel::ReqSetLinkPropertiesHttpProxy(
    /* [in] */ IProxyInfo* proxy)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        sendMessage(REQ_SET_LINK_PROPERTIES_HTTP_PROXY, proxy);
        if (DBG) log("reqSetLinkPropertiesHttpProxy proxy=" + proxy);

#endif
}

ECode DcAsyncChannel::SetLinkPropertiesHttpProxySync(
    /* [in] */ IProxyInfo* proxy)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (isCallerOnDifferentThread()) {
            Message response =
                sendMessageSynchronously(REQ_SET_LINK_PROPERTIES_HTTP_PROXY, proxy);
            if ((response != null) && (response.what == RSP_SET_LINK_PROPERTIES_HTTP_PROXY)) {
                if (DBG) log("setLinkPropertiesHttpPoxy ok");
            } else {
                log("setLinkPropertiesHttpPoxy error response=" + response);
            }
        } else {
            mDc.setLinkPropertiesHttpProxy(proxy);
        }

#endif
}

ECode DcAsyncChannel::ReqNetworkCapabilities()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        sendMessage(REQ_GET_NETWORK_CAPABILITIES);
        if (DBG) log("reqNetworkCapabilities");

#endif
}

ECode DcAsyncChannel::RspNetworkCapabilities(
    /* [in] */ IMessage* response,
    /* [out] */ INetworkCapabilities** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        NetworkCapabilities retVal = (NetworkCapabilities) response.obj;
        if (DBG) log("rspNetworkCapabilities=" + retVal);
        return retVal;

#endif
}

ECode DcAsyncChannel::GetNetworkCapabilitiesSync(
    /* [out] */ INetworkCapabilities** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        NetworkCapabilities value;
        if (isCallerOnDifferentThread()) {
            Message response = sendMessageSynchronously(REQ_GET_NETWORK_CAPABILITIES);
            if ((response != null) && (response.what == RSP_GET_NETWORK_CAPABILITIES)) {
                value = rspNetworkCapabilities(response);
            } else {
                value = null;
            }
        } else {
            value = mDc.getCopyNetworkCapabilities();
        }
        return value;

#endif
}

ECode DcAsyncChannel::ReqReset()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        sendMessage(REQ_RESET);
        if (DBG) log("reqReset");

#endif
}

ECode DcAsyncChannel::BringUp(
    /* [in] */ IApnContext* apnContext,
    /* [in] */ Int32 initialMaxRetry,
    /* [in] */ Int32 profileId,
    /* [in] */ Int32 rilRadioTechnology,
    /* [in] */ Boolean retryWhenSSChange,
    /* [in] */ IMessage* onCompletedMsg)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (DBG) {
            log("bringUp: apnContext=" + apnContext + " initialMaxRetry=" + initialMaxRetry
                + " onCompletedMsg=" + onCompletedMsg);
        }
        sendMessage(DataConnection.EVENT_CONNECT,
                    new ConnectionParams(apnContext, initialMaxRetry, profileId,
                            rilRadioTechnology, retryWhenSSChange, onCompletedMsg));

#endif
}

ECode DcAsyncChannel::TearDown(
    /* [in] */ IApnContext* apnContext,
    /* [in] */ const String& reason,
    /* [in] */ IMessage* onCompletedMsg)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (DBG) {
            log("tearDown: apnContext=" + apnContext
                    + " reason=" + reason + " onCompletedMsg=" + onCompletedMsg);
        }
        sendMessage(DataConnection.EVENT_DISCONNECT,
                        new DisconnectParams(apnContext, reason, onCompletedMsg));

#endif
}

ECode DcAsyncChannel::TearDownAll(
    /* [in] */ const String& reason,
    /* [in] */ IMessage* onCompletedMsg)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (DBG) log("tearDownAll: reason=" + reason + " onCompletedMsg=" + onCompletedMsg);
        sendMessage(DataConnection.EVENT_DISCONNECT_ALL,
                new DisconnectParams(null, reason, onCompletedMsg));

#endif
}

ECode DcAsyncChannel::GetDataConnectionIdSync(
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        // Safe because this is owned by the caller.
        return mDc.getDataConnectionId();

#endif
}

ECode DcAsyncChannel::ToString(
    /* [out] */ String* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return mDc.getName();

#endif
}

ECode DcAsyncChannel::IsCallerOnDifferentThread(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        long curThreadId = Thread.currentThread().getId();
        boolean value = mDcThreadId != curThreadId;
        if (DBG) log("isCallerOnDifferentThread: " + value);
        return value;

#endif
}

ECode DcAsyncChannel::Log(
    /* [in] */ const String& s)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        android.telephony.Rlog.d(mLogTag, "DataConnectionAc " + s);

#endif
}

ECode DcAsyncChannel::GetPcscfAddr(
    /* [out, callee] */ ArrayOf<String>** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return mDc.mPcscfAddr;

#endif
}

AutoPtr<ArrayOf<String> > DcAsyncChannel::InitCmdToString()
{
    AutoPtr<ArrayOf<String> > rev = ArrayOf<String>::Alloc(CMD_TO_STRING_COUNT);
    (*rev)[REQ_IS_INACTIVE - BASE] = "REQ_IS_INACTIVE";
    (*rev)[RSP_IS_INACTIVE - BASE] = "RSP_IS_INACTIVE";
    (*rev)[REQ_GET_CID - BASE] = "REQ_GET_CID";
    (*rev)[RSP_GET_CID - BASE] = "RSP_GET_CID";
    (*rev)[REQ_GET_APNSETTING - BASE] = "REQ_GET_APNSETTING";
    (*rev)[RSP_GET_APNSETTING - BASE] = "RSP_GET_APNSETTING";
    (*rev)[REQ_GET_LINK_PROPERTIES - BASE] = "REQ_GET_LINK_PROPERTIES";
    (*rev)[RSP_GET_LINK_PROPERTIES - BASE] = "RSP_GET_LINK_PROPERTIES";
    (*rev)[REQ_SET_LINK_PROPERTIES_HTTP_PROXY - BASE] = "REQ_SET_LINK_PROPERTIES_HTTP_PROXY";
    (*rev)[RSP_SET_LINK_PROPERTIES_HTTP_PROXY - BASE] = "RSP_SET_LINK_PROPERTIES_HTTP_PROXY";
    (*rev)[REQ_GET_NETWORK_CAPABILITIES - BASE] = "REQ_GET_NETWORK_CAPABILITIES";
    (*rev)[RSP_GET_NETWORK_CAPABILITIES - BASE] = "RSP_GET_NETWORK_CAPABILITIES";
    (*rev)[REQ_RESET - BASE] = "REQ_RESET";
    (*rev)[RSP_RESET - BASE] = "RSP_RESET";
    return rev;
}

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
