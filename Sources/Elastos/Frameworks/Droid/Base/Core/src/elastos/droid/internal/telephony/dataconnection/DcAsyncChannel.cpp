
#include "elastos/droid/internal/telephony/dataconnection/DcAsyncChannel.h"
#include "elastos/droid/internal/telephony/dataconnection/DataConnection.h"
#include "elastos/droid/internal/utility/AsyncChannel.h"
#include <elastos/core/Thread.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Internal::Utility::IStateMachine;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::ILooper;
using Elastos::Core::IThread;
using Elastos::Core::Thread;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

CAR_INTERFACE_IMPL(DcAsyncChannel, AsyncChannel, IDcAsyncChannel)

const Boolean DcAsyncChannel::DBG = FALSE;
const Int32 DcAsyncChannel::CMD_TO_STRING_COUNT = RSP_RESET - BASE + 1;
AutoPtr<ArrayOf<String> > DcAsyncChannel::sCmdToString = InitCmdToString();

ECode DcAsyncChannel::CmdToString(
    /* [in] */ Int32 _cmd,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = String(NULL);

    Int32 cmd = _cmd;
    cmd -= BASE;
    if ((cmd >= 0) && (cmd < sCmdToString->GetLength())) {
        *result = (*sCmdToString)[cmd];
        return NOERROR;
    } else {
        assert(0 && "AsyncChannel");
        // return AsyncChannel::CmdToString(cmd + BASE, result);
    }
    return NOERROR;
}

ECode DcAsyncChannel::constructor(
    /* [in] */ IDataConnection* dc,
    /* [in] */ const String& logTag)
{
    mDc = dc;
    AutoPtr<IHandler> handler;
    IStateMachine::Probe(mDc)->GetHandler((IHandler**)&handler);
    AutoPtr<ILooper> looper;
    handler->GetLooper((ILooper**)&looper);
    AutoPtr<IThread> thread;
    looper->GetThread((IThread**)&thread);
    thread->GetId(&mDcThreadId);
    mLogTag = logTag;
    return NOERROR;
}

ECode DcAsyncChannel::ReqIsInactive()
{
    SendMessage(REQ_IS_INACTIVE);
    if (DBG) Log("reqIsInactive");
    return NOERROR;
}

ECode DcAsyncChannel::RspIsInactive(
    /* [in] */ IMessage* response,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 arg1;
    response->GetArg1(&arg1);
    Boolean retVal = arg1 == 1;
    if (DBG) Log("rspIsInactive=%d", retVal);
    *result = retVal;
    return NOERROR;
}

ECode DcAsyncChannel::IsInactiveSync(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean value;
    Boolean isCallerOnDifferentThread;
    IsCallerOnDifferentThread(&isCallerOnDifferentThread);
    if (isCallerOnDifferentThread) {
        AutoPtr<IMessage> response;
        SendMessageSynchronously(REQ_IS_INACTIVE, (IMessage**)&response);
        Int32 what;
        response->GetWhat(&what);
        if ((response != NULL) && (what == RSP_IS_INACTIVE)) {
            RspIsInactive(response, &value);
        } else {
            Log("rspIsInactive error response=%s", TO_CSTR(response));
            value = FALSE;
        }
    } else {
        ((DataConnection*) mDc.Get())->GetIsInactive(&value);
    }
    *result = value;
    return NOERROR;
}

ECode DcAsyncChannel::ReqCid()
{
    SendMessage(REQ_GET_CID);
    if (DBG) Log("reqCid");
    return NOERROR;
}

ECode DcAsyncChannel::RspCid(
    /* [in] */ IMessage* response,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 retVal;
    response->GetArg1(&retVal);
    if (DBG) Log("rspCid=%d", retVal);
    *result = retVal;
    return NOERROR;
}

ECode DcAsyncChannel::GetCidSync(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 value;
    Boolean isCallerOnDifferentThread;
    IsCallerOnDifferentThread(&isCallerOnDifferentThread);
    if (isCallerOnDifferentThread) {
        AutoPtr<IMessage> response;
        SendMessageSynchronously(REQ_GET_CID, (IMessage**)&response);
        Int32 what;
        response->GetWhat(&what);
        if ((response != NULL) && (what == RSP_GET_CID)) {
            RspCid(response, &value);
        } else {
            Log("rspCid error response=%s", TO_CSTR(response));
            value = -1;
        }
    } else {
        ((DataConnection*) mDc.Get())->GetCid(&value);
    }
    *result = value;
    return NOERROR;
}

ECode DcAsyncChannel::ReqApnSetting()
{
    SendMessage(REQ_GET_APNSETTING);
    if (DBG) Log("reqApnSetting");
    return NOERROR;
}

ECode DcAsyncChannel::RspApnSetting(
    /* [in] */ IMessage* response,
    /* [out] */ IApnSetting** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IInterface> obj;
    response->GetObj((IInterface**)&obj);
    AutoPtr<IApnSetting> retVal = IApnSetting::Probe(obj);
    if (DBG) Log("rspApnSetting=%s", TO_CSTR(retVal));
    *result = retVal;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode DcAsyncChannel::GetApnSettingSync(
    /* [out] */ IApnSetting** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IApnSetting> value;
    Boolean isCallerOnDifferentThread;
    IsCallerOnDifferentThread(&isCallerOnDifferentThread);
    if (isCallerOnDifferentThread) {
        AutoPtr<IMessage> response;
        SendMessageSynchronously(REQ_GET_APNSETTING, (IMessage**)&response);
        Int32 what;
        response->GetWhat(&what);
        if ((response != NULL) && (what == RSP_GET_APNSETTING)) {
            RspApnSetting(response, (IApnSetting**)&value);
        } else {
            Log("getApnSetting error response=%s", TO_CSTR(response));
            value = NULL;
        }
    } else {
        ((DataConnection*) mDc.Get())->GetApnSetting((IApnSetting**)&value);
    }
    *result = value;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode DcAsyncChannel::ReqLinkProperties()
{
    SendMessage(REQ_GET_LINK_PROPERTIES);
    if (DBG) Log("reqLinkProperties");
    return NOERROR;
}

ECode DcAsyncChannel::RspLinkProperties(
    /* [in] */ IMessage* response,
    /* [out] */ ILinkProperties** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IInterface> obj;
    response->GetObj((IInterface**)&obj);
    AutoPtr<ILinkProperties> retVal = ILinkProperties::Probe(obj);
    if (DBG) Log("rspLinkProperties=%s", TO_CSTR(retVal));
    *result = retVal;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode DcAsyncChannel::GetLinkPropertiesSync(
    /* [out] */ ILinkProperties** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ILinkProperties> value;
    Boolean isCallerOnDifferentThread;
    IsCallerOnDifferentThread(&isCallerOnDifferentThread);
    if (isCallerOnDifferentThread) {
        AutoPtr<IMessage> response;
        SendMessageSynchronously(REQ_GET_LINK_PROPERTIES, (IMessage**)&response);
        Int32 what;
        response->GetWhat(&what);
        if ((response != NULL) && (what == RSP_GET_LINK_PROPERTIES)) {
            RspLinkProperties(response, (ILinkProperties**)&value);
        } else {
            Log("getLinkProperties error response=%s", TO_CSTR(response));
            value = NULL;
        }
    } else {
        ((DataConnection*) mDc.Get())->GetCopyLinkProperties((ILinkProperties**)&value);
    }
    *result = value;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode DcAsyncChannel::ReqSetLinkPropertiesHttpProxy(
    /* [in] */ IProxyInfo* proxy)
{
    SendMessage(REQ_SET_LINK_PROPERTIES_HTTP_PROXY, proxy);
    if (DBG) Log("reqSetLinkPropertiesHttpProxy proxy=%s", TO_CSTR(proxy));
    return NOERROR;
}

ECode DcAsyncChannel::SetLinkPropertiesHttpProxySync(
    /* [in] */ IProxyInfo* proxy)
{
    Boolean isCallerOnDifferentThread;
    IsCallerOnDifferentThread(&isCallerOnDifferentThread);
    if (isCallerOnDifferentThread) {
        AutoPtr<IMessage> response;
        SendMessageSynchronously(REQ_SET_LINK_PROPERTIES_HTTP_PROXY, proxy,
                (IMessage**)&response);
        Int32 what;
        response->GetWhat(&what);
        if ((response != NULL) && (what == RSP_SET_LINK_PROPERTIES_HTTP_PROXY)) {
            if (DBG) Log("setLinkPropertiesHttpPoxy ok");
        } else {
            Log("setLinkPropertiesHttpPoxy error response=%s", TO_CSTR(response));
        }
    } else {
        ((DataConnection*) mDc.Get())->SetLinkPropertiesHttpProxy(proxy);
    }
    return NOERROR;
}

ECode DcAsyncChannel::ReqNetworkCapabilities()
{
    SendMessage(REQ_GET_NETWORK_CAPABILITIES);
    if (DBG) Log("reqNetworkCapabilities");
    return NOERROR;
}

ECode DcAsyncChannel::RspNetworkCapabilities(
    /* [in] */ IMessage* response,
    /* [out] */ INetworkCapabilities** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IInterface> obj;
    response->GetObj((IInterface**)&obj);
    AutoPtr<INetworkCapabilities> retVal = INetworkCapabilities::Probe(obj);
    if (DBG) Log("rspNetworkCapabilities=%s", TO_CSTR(retVal));
    *result = retVal;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode DcAsyncChannel::GetNetworkCapabilitiesSync(
    /* [out] */ INetworkCapabilities** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<INetworkCapabilities> value;
    Boolean isCallerOnDifferentThread;
    IsCallerOnDifferentThread(&isCallerOnDifferentThread);
    if (isCallerOnDifferentThread) {
        AutoPtr<IMessage> response;
        SendMessageSynchronously(REQ_GET_NETWORK_CAPABILITIES, (IMessage**)&response);
        Int32 what;
        response->GetWhat(&what);
        if ((response != NULL) && (what == RSP_GET_NETWORK_CAPABILITIES)) {
            RspNetworkCapabilities(response, (INetworkCapabilities**)&value);
        } else {
            value = NULL;
        }
    } else {
        ((DataConnection*) mDc.Get())->GetCopyNetworkCapabilities((INetworkCapabilities**)&value);
    }
    *result = value;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode DcAsyncChannel::ReqReset()
{
    SendMessage(REQ_RESET);
    if (DBG) Log("reqReset");
    return NOERROR;
}

ECode DcAsyncChannel::BringUp(
    /* [in] */ IApnContext* apnContext,
    /* [in] */ Int32 initialMaxRetry,
    /* [in] */ Int32 profileId,
    /* [in] */ Int32 rilRadioTechnology,
    /* [in] */ Boolean retryWhenSSChange,
    /* [in] */ IMessage* onCompletedMsg)
{
    if (DBG) {
        Log("bringUp: apnContext=%s initialMaxRetry=%d onCompletedMsg=%s",
                TO_CSTR(apnContext), initialMaxRetry, TO_CSTR(onCompletedMsg));
    }
    AutoPtr<DataConnection::ConnectionParams> params = new DataConnection::ConnectionParams();
    params->constructor(apnContext, initialMaxRetry, profileId,
            rilRadioTechnology, retryWhenSSChange, onCompletedMsg);
    SendMessage(DataConnection::EVENT_CONNECT, TO_IINTERFACE(params));
    return NOERROR;
}

ECode DcAsyncChannel::TearDown(
    /* [in] */ IApnContext* apnContext,
    /* [in] */ const String& reason,
    /* [in] */ IMessage* onCompletedMsg)
{
    if (DBG) {
        Log("tearDown: apnContext=%s reason=%s onCompletedMsg=%s",
                TO_CSTR(apnContext), reason.string(), TO_CSTR(onCompletedMsg));
    }
    AutoPtr<DataConnection::DisconnectParams> params = new DataConnection::DisconnectParams();
    params->constructor(apnContext, reason, onCompletedMsg);
    SendMessage(DataConnection::EVENT_DISCONNECT, TO_IINTERFACE(params));
    return NOERROR;
}

ECode DcAsyncChannel::TearDownAll(
    /* [in] */ const String& reason,
    /* [in] */ IMessage* onCompletedMsg)
{
    if (DBG) Log("tearDownAll: reason=%s onCompletedMsg=%s",
            reason.string(), TO_CSTR(onCompletedMsg));
    AutoPtr<DataConnection::DisconnectParams> params = new DataConnection::DisconnectParams();
    params->constructor(NULL, reason, onCompletedMsg);
    SendMessage(DataConnection::EVENT_DISCONNECT_ALL, TO_IINTERFACE(params));
    return NOERROR;
}

ECode DcAsyncChannel::GetDataConnectionIdSync(
    /* [out] */ Int32* result)
{
    // Safe because this is owned by the caller.
    return mDc->GetDataConnectionId(result);
}

ECode DcAsyncChannel::ToString(
    /* [out] */ String* result)
{
    return ((DataConnection*) mDc.Get())->GetName(result);
}

ECode DcAsyncChannel::IsCallerOnDifferentThread(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int64 curThreadId;
    Thread::GetCurrentThread()->GetId(&curThreadId);
    Boolean value = mDcThreadId != curThreadId;
    if (DBG) Log("isCallerOnDifferentThread: %d", value);
    *result = value;
    return NOERROR;
}

#define MSG_BUF_SIZE    1024
ECode DcAsyncChannel::Log(
    /* [in] */ const char *fmt, ...)
{
    char msgBuf[MSG_BUF_SIZE];
    va_list args;
    va_start(args, fmt);
    vsnprintf(msgBuf, MSG_BUF_SIZE, fmt, args);
    va_end(args);

    return Logger::D(mLogTag, "DataConnectionAc %s", msgBuf);
}

ECode DcAsyncChannel::GetPcscfAddr(
    /* [out, callee] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result)

    *result = ((DataConnection*) mDc.Get())->mPcscfAddr;
    REFCOUNT_ADD(*result)
    return NOERROR;
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
