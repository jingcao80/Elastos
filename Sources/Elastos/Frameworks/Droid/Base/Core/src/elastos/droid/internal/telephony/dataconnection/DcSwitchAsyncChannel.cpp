
#include "elastos/droid/internal/telephony/dataconnection/DcSwitchAsyncChannel.h"
#include "elastos/droid/internal/telephony/dataconnection/DataConnection.h"
#include "elastos/droid/internal/utility/AsyncChannel.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Internal::Utility::IProtocol;
using Elastos::Droid::Internal::Utility::IStateMachine;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

CAR_INTERFACE_IMPL(DcSwitchAsyncChannel, AsyncChannel, IDcSwitchAsyncChannel)

const Boolean DcSwitchAsyncChannel::DBG = TRUE;
const Boolean DcSwitchAsyncChannel::VDBG = FALSE;
const String DcSwitchAsyncChannel::LOG__TAG("DcSwitchAsyncChannel");
const Int32 DcSwitchAsyncChannel::BASE = IProtocol::BASE_DATA_CONNECTION_TRACKER + 0x00002000;
const Int32 DcSwitchAsyncChannel::REQ_CONNECT = BASE + 0;
const Int32 DcSwitchAsyncChannel::RSP_CONNECT = BASE + 1;
const Int32 DcSwitchAsyncChannel::REQ_DISCONNECT = BASE + 2;
const Int32 DcSwitchAsyncChannel::RSP_DISCONNECT = BASE + 3;
const Int32 DcSwitchAsyncChannel::REQ_IS_IDLE_STATE = BASE + 4;
const Int32 DcSwitchAsyncChannel::RSP_IS_IDLE_STATE = BASE + 5;
const Int32 DcSwitchAsyncChannel::REQ_IS_IDLE_OR_DEACTING_STATE = BASE + 6;
const Int32 DcSwitchAsyncChannel::RSP_IS_IDLE_OR_DEACTING_STATE = BASE + 7;
const Int32 DcSwitchAsyncChannel::CMD_TO_STRING_COUNT = RSP_IS_IDLE_OR_DEACTING_STATE - BASE + 1;
AutoPtr<ArrayOf<String> > DcSwitchAsyncChannel::sCmdToString = InitCmdToString();

DcSwitchAsyncChannel::DcSwitchAsyncChannel()
    : mTagId(0)
{}

ECode DcSwitchAsyncChannel::CmdToString(
    /* [in] */ Int32 _cmd,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

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

ECode DcSwitchAsyncChannel::constructor(
    /* [in] */ IDcSwitchState* dcSwitchState,
    /* [in] */ Int32 id)
{
    mDcSwitchState = dcSwitchState;
    mTagId = id;
    return NOERROR;
}

ECode DcSwitchAsyncChannel::ReqConnect(
    /* [in] */ const String& type)
{
    SendMessage(REQ_CONNECT, StringUtils::ParseCharSequence(type));
    if (DBG) Log("reqConnect");
    return NOERROR;
}

ECode DcSwitchAsyncChannel::RspConnect(
    /* [in] */ IMessage* response,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 retVal;
    response->GetArg1(&retVal);
    if (DBG) Log("rspConnect=%d", retVal);
    *result = retVal;
    return NOERROR;
}

ECode DcSwitchAsyncChannel::ConnectSync(
    /* [in] */ const String& type,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    AutoPtr<IMessage> response;
    SendMessageSynchronously(REQ_CONNECT, StringUtils::ParseCharSequence(type), (IMessage**)&response);
    Int32 what;
    response->GetWhat(&what);
    if ((response != NULL) && (what == RSP_CONNECT)) {
        return RspConnect(response, result);
    } else {
        Log("rspConnect error response=%s", TO_CSTR(response));
        return IPhoneConstants::APN_REQUEST_FAILED;
    }
    return NOERROR;
}

ECode DcSwitchAsyncChannel::ReqDisconnect(
    /* [in] */ const String& type)
{
    SendMessage(REQ_DISCONNECT, StringUtils::ParseCharSequence(type));
    if (DBG) Log("reqDisconnect");
    return NOERROR;
}

ECode DcSwitchAsyncChannel::RspDisconnect(
    /* [in] */ IMessage* response,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 retVal;
    response->GetArg1(&retVal);
    if (DBG) Log("rspDisconnect=%d", retVal);
    *result = retVal;
    return NOERROR;
}

ECode DcSwitchAsyncChannel::DisconnectSync(
    /* [in] */ const String& type,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IMessage> response;
    SendMessageSynchronously(REQ_DISCONNECT, StringUtils::ParseCharSequence(type), (IMessage**)&response);
    Int32 what;
    response->GetWhat(&what);
    if ((response != NULL) && (what == RSP_DISCONNECT)) {
        return RspDisconnect(response, result);
    } else {
        Log("rspDisconnect error response=%s", TO_CSTR(response));
        *result = IPhoneConstants::APN_REQUEST_FAILED;
        return NOERROR;
    }
    return NOERROR;
}

ECode DcSwitchAsyncChannel::ReqIsIdle()
{
    SendMessage(REQ_IS_IDLE_STATE);
    if (DBG) Log("reqIsIdle");
    return NOERROR;
}

ECode DcSwitchAsyncChannel::RspIsIdle(
    /* [in] */ IMessage* response,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 arg1;
    response->GetArg1(&arg1);
    Boolean retVal = arg1 == 1;
    if (DBG) Log("rspIsIdle=%d", retVal);
    *result = retVal;
    return NOERROR;
}

ECode DcSwitchAsyncChannel::IsIdleSync(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IMessage> response;
    SendMessageSynchronously(REQ_IS_IDLE_STATE, (IMessage**)&response);
    Int32 what;
    response->GetWhat(&what);
    if ((response != NULL) && (what == RSP_IS_IDLE_STATE)) {
        return RspIsIdle(response, result);
    } else {
        Log("rspIsIndle error response=%s", TO_CSTR(response));
        *result = FALSE;
        return NOERROR;
    }
    return NOERROR;
}

ECode DcSwitchAsyncChannel::ReqIsIdleOrDeacting()
{
    SendMessage(REQ_IS_IDLE_OR_DEACTING_STATE);
    if (DBG) Log("reqIsIdleOrDeacting");
    return NOERROR;
}

ECode DcSwitchAsyncChannel::RspIsIdleOrDeacting(
    /* [in] */ IMessage* response,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 arg1;
    response->GetArg1(&arg1);
    Boolean retVal = arg1 == 1;
    if (DBG) Log("rspIsIdleOrDeacting=%d", retVal);
    *result = retVal;
    return NOERROR;
}

ECode DcSwitchAsyncChannel::IsIdleOrDeactingSync(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IMessage> response;
    SendMessageSynchronously(REQ_IS_IDLE_OR_DEACTING_STATE, (IMessage**)&response);
    Int32 what;
    response->GetWhat(&what);
    if ((response != NULL) && (what == RSP_IS_IDLE_OR_DEACTING_STATE)) {
        return RspIsIdleOrDeacting(response, result);
    } else {
        Log("rspIsIndleOrDeacting error response=%s", TO_CSTR(response));
        *result = FALSE;
        return NOERROR;
    }
    return NOERROR;
}

ECode DcSwitchAsyncChannel::ToString(
    /* [out] */ String* result)
{
    return IStateMachine::Probe(mDcSwitchState)->GetName(result);
}

#define MSG_BUF_SIZE    1024
ECode DcSwitchAsyncChannel::Log(
    /* [in] */ const char *fmt, ...)
{
    char msgBuf[MSG_BUF_SIZE];
    va_list args;
    va_start(args, fmt);
    vsnprintf(msgBuf, MSG_BUF_SIZE, fmt, args);
    va_end(args);

    return Logger::D(LOG__TAG, "[DcSwitchAsyncChannel-%d]: %s", mTagId, msgBuf);
}

AutoPtr<ArrayOf<String> > DcSwitchAsyncChannel::InitCmdToString()
{
    AutoPtr<ArrayOf<String> > rev = ArrayOf<String>::Alloc(CMD_TO_STRING_COUNT);
    (*rev)[REQ_CONNECT - BASE] = "REQ_CONNECT";
    (*rev)[RSP_CONNECT - BASE] = "RSP_CONNECT";
    (*rev)[REQ_DISCONNECT - BASE] = "REQ_DISCONNECT";
    (*rev)[RSP_DISCONNECT - BASE] = "RSP_DISCONNECT";
    (*rev)[REQ_IS_IDLE_STATE - BASE] = "REQ_IS_IDLE_STATE";
    (*rev)[RSP_IS_IDLE_STATE - BASE] = "RSP_IS_IDLE_STATE";
    (*rev)[REQ_IS_IDLE_OR_DEACTING_STATE - BASE] = "REQ_IS_IDLE_OR_DEACTING_STATE";
    (*rev)[RSP_IS_IDLE_OR_DEACTING_STATE - BASE] = "RSP_IS_IDLE_OR_DEACTING_STATE";
    return rev;
}

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
