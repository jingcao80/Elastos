
#include "elastos/droid/internal/telephony/dataconnection/DcSwitchAsyncChannel.h"

using Elastos::Droid::Internal::Utility::IProtocol;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

CAR_INTERFACE_IMPL(DcSwitchAsyncChannel, AsyncChannel, IDcSwitchAsyncChannel)

const Boolean DcSwitchAsyncChannel::DBG = true;
const Boolean DcSwitchAsyncChannel::VDBG = false;
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

ECode DcSwitchAsyncChannel::constructor(
    /* [in] */ IDcSwitchState* dcSwitchState,
    /* [in] */ Int32 id)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mDcSwitchState = dcSwitchState;
        tagId = id;

#endif
}

ECode DcSwitchAsyncChannel::ReqConnect(
    /* [in] */ const String& type)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        sendMessage(REQ_CONNECT, type);
        if (DBG) log("reqConnect");

#endif
}

ECode DcSwitchAsyncChannel::RspConnect(
    /* [in] */ IMessage* response,
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        int retVal = response.arg1;
        if (DBG) log("rspConnect=" + retVal);
        return retVal;

#endif
}

ECode DcSwitchAsyncChannel::ConnectSync(
    /* [in] */ const String& type,
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Message response = sendMessageSynchronously(REQ_CONNECT, type);
        if ((response != null) && (response.what == RSP_CONNECT)) {
            return rspConnect(response);
        } else {
            log("rspConnect error response=" + response);
            return PhoneConstants.APN_REQUEST_FAILED;
        }

#endif
}

ECode DcSwitchAsyncChannel::ReqDisconnect(
    /* [in] */ const String& type)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        sendMessage(REQ_DISCONNECT, type);
        if (DBG) log("reqDisconnect");

#endif
}

ECode DcSwitchAsyncChannel::RspDisconnect(
    /* [in] */ IMessage* response,
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        int retVal = response.arg1;
        if (DBG) log("rspDisconnect=" + retVal);
        return retVal;

#endif
}

ECode DcSwitchAsyncChannel::DisconnectSync(
    /* [in] */ const String& type,
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Message response = sendMessageSynchronously(REQ_DISCONNECT, type);
        if ((response != null) && (response.what == RSP_DISCONNECT)) {
            return rspDisconnect(response);
        } else {
            log("rspDisconnect error response=" + response);
            return PhoneConstants.APN_REQUEST_FAILED;
        }

#endif
}

ECode DcSwitchAsyncChannel::ReqIsIdle()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        sendMessage(REQ_IS_IDLE_STATE);
        if (DBG) log("reqIsIdle");

#endif
}

ECode DcSwitchAsyncChannel::RspIsIdle(
    /* [in] */ IMessage* response,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        boolean retVal = response.arg1 == 1;
        if (DBG) log("rspIsIdle=" + retVal);
        return retVal;

#endif
}

ECode DcSwitchAsyncChannel::IsIdleSync(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Message response = sendMessageSynchronously(REQ_IS_IDLE_STATE);
        if ((response != null) && (response.what == RSP_IS_IDLE_STATE)) {
            return rspIsIdle(response);
        } else {
            log("rspIsIndle error response=" + response);
            return false;
        }

#endif
}

ECode DcSwitchAsyncChannel::ReqIsIdleOrDeacting()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        sendMessage(REQ_IS_IDLE_OR_DEACTING_STATE);
        if (DBG) log("reqIsIdleOrDeacting");

#endif
}

ECode DcSwitchAsyncChannel::RspIsIdleOrDeacting(
    /* [in] */ IMessage* response,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        boolean retVal = response.arg1 == 1;
        if (DBG) log("rspIsIdleOrDeacting=" + retVal);
        return retVal;

#endif
}

ECode DcSwitchAsyncChannel::IsIdleOrDeactingSync(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Message response = sendMessageSynchronously(REQ_IS_IDLE_OR_DEACTING_STATE);
        if ((response != null) && (response.what == RSP_IS_IDLE_OR_DEACTING_STATE)) {
            return rspIsIdleOrDeacting(response);
        } else {
            log("rspIsIndleOrDeacting error response=" + response);
            return false;
        }

#endif
}

ECode DcSwitchAsyncChannel::ToString(
    /* [out] */ String* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return mDcSwitchState.getName();

#endif
}

ECode DcSwitchAsyncChannel::Log(
    /* [in] */ const String& s)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Log.d(LOG__TAG, "[DcSwitchAsyncChannel-" + tagId + "]: " + s);

#endif
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
