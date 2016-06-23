
#include "elastos/droid/internal/telephony/dataconnection/DcSwitchState.h"
#include "elastos/droid/internal/telephony/PhoneBase.h"
#include "elastos/droid/internal/telephony/dataconnection/DataConnection.h"
#include "elastos/droid/internal/telephony/dataconnection/DcSwitchAsyncChannel.h"
#include "elastos/droid/internal/utility/CAsyncChannel.h"
#include "elastos/droid/internal/utility/StateMachine.h"
#include "elastos/droid/os/SystemProperties.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/core/StringUtils.h>
#include <elastos/droid/text/TextUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Internal::Telephony::IPhoneBase;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Internal::Telephony::IPhoneProxy;
using Elastos::Droid::Internal::Telephony::ISubscriptionController;
using Elastos::Droid::Internal::Telephony::PhoneConstantsDataState_CONNECTED;
using Elastos::Droid::Internal::Utility::CAsyncChannel;
using Elastos::Droid::Internal::Utility::IProtocol;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IMessenger;
using Elastos::Droid::Os::IRegistrant;
using Elastos::Droid::Os::SystemProperties;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::StringUtils;
using Elastos::Utility::CHashSet;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

//=============================================================================
// DcSwitchState::IdleState
//=============================================================================
DcSwitchState::IdleState::IdleState(
    /* [in] */ DcSwitchState* host)
    : mHost(host)
{}

ECode DcSwitchState::IdleState::Enter()
{
    assert(0 && "IRegistrantList");
    // return mHost->mIdleRegistrants->NotifyRegistrants();
    return NOERROR;
}

ECode DcSwitchState::IdleState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 msgWhat;
    msg->GetWhat(&msgWhat);
    AutoPtr<IInterface> msgObj;
    msg->GetObj((IInterface**)&msgObj);
    Boolean retVal;
    if (msgWhat == DcSwitchAsyncChannel::REQ_CONNECT
            || msgWhat == EVENT_CONNECT) {
        String type;
        IObject::Probe(msgObj)->ToString(&type);
        if (DBG) {
            mHost->Log("IdleState: REQ_CONNECT/EVENT_CONNECT(%d) type=%s",
                msgWhat, type.string());
        }
        Boolean isPrimarySubFeatureEnable;
        SystemProperties::GetBoolean(String("persist.radio.primarycard"), FALSE, &isPrimarySubFeatureEnable);
        AutoPtr<IPhone> phone;
        IPhoneProxy::Probe(mHost->mPhone)->GetActivePhone((IPhone**)&phone);
        AutoPtr<IPhoneBase> pb = IPhoneBase::Probe(phone);
        Int64 subId;
        pb->GetSubId(&subId);
        mHost->Log("Setting default DDS on %ld primary Sub feature %d",
                subId, isPrimarySubFeatureEnable);
        // When isPrimarySubFeatureEnable is enabled apps will take care
        // of sending DDS request during device power-up.
        if (!isPrimarySubFeatureEnable) {
            assert(0 && "SubscriptionController");
            // AutoPtr<ISubscriptionController> subscriptionController
            //         = SubscriptionController::GetInstance();
            // subscriptionController->SetDefaultDataSubId(subId);
        }
        Int32 result;
        mHost->SetupConnection(type, &result);
        if (msgWhat == DcSwitchAsyncChannel::REQ_CONNECT) {
                mHost->mAc->ReplyToMessage(msg, DcSwitchAsyncChannel::RSP_CONNECT, result);
        }
        mHost->TransitionTo(mHost->mActingState);
        retVal = HANDLED;
    }
    else if (msgWhat == DcSwitchAsyncChannel::REQ_DISCONNECT) {
        String type;
        IObject::Probe(msgObj)->ToString(&type);
        if (DBG) {
            mHost->Log("IdleState: DcSwitchAsyncChannel.REQ_DISCONNECT type=%s", type.string());
        }
        mHost->mAc->ReplyToMessage(msg, DcSwitchAsyncChannel::RSP_DISCONNECT,
            IPhoneConstants::APN_ALREADY_INACTIVE);
        retVal = HANDLED;
    }
    else if (msgWhat == EVENT_CLEANUP_ALL) {
        if (DBG) {
            mHost->Log("IdleState: EVENT_CLEANUP_ALL" );
        }
        mHost->RequestDataIdle();
        retVal = HANDLED;
    }
    else if (msgWhat == EVENT_CONNECTED) {
        if (DBG) {
            mHost->Log("IdleState: Receive invalid event EVENT_CONNECTED!");
        }
        retVal = HANDLED;
    }
    else {
        if (VDBG) {
            mHost->Log("IdleState: nothandled msg.what=0x%08X",
                    StringUtils::ToHexString(msgWhat).string());
        }
        retVal = NOT_HANDLED;
    }
    *result = retVal;
    return NOERROR;
}

String DcSwitchState::IdleState::GetName()
{
    return String("DcSwitchState::IdleState");
}

//=============================================================================
// DcSwitchState::ActingState
//=============================================================================
DcSwitchState::ActingState::ActingState(
    /* [in] */ DcSwitchState* host)
    : mHost(host)
{}

ECode DcSwitchState::ActingState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 msgWhat;
    msg->GetWhat(&msgWhat);
    AutoPtr<IInterface> msgObj;
    msg->GetObj((IInterface**)&msgObj);
    Boolean retVal;
    if (msgWhat == DcSwitchAsyncChannel::REQ_CONNECT
            || msgWhat == EVENT_CONNECT) {
        String type;
        IObject::Probe(msgObj)->ToString(&type);
        if (DBG) {
            mHost->Log("ActingState: REQ_CONNECT/EVENT_CONNECT(%d) type=%s",
                    msgWhat, type.string());
        }
        Int32 result;
        mHost->SetupConnection(type, &result);
        if (msgWhat == DcSwitchAsyncChannel::REQ_CONNECT) {
            mHost->mAc->ReplyToMessage(msg, DcSwitchAsyncChannel::RSP_CONNECT, result);
        }
        retVal = HANDLED;
    }
    else if (msgWhat == DcSwitchAsyncChannel::REQ_DISCONNECT) {
        String type;
        IObject::Probe(msgObj)->ToString(&type);
        if (DBG) {
            mHost->Log("ActingState: DcSwitchAsyncChannel.REQ_DISCONNECT type=%s", type.string());
        }
        Int32 result;
        mHost->TeardownConnection(type, &result);
        mHost->mAc->ReplyToMessage(msg, DcSwitchAsyncChannel::RSP_DISCONNECT, result);
        retVal = HANDLED;
    }
    else if (msgWhat == EVENT_CONNECTED) {
        if (DBG) {
            mHost->Log("ActingState: EVENT_CONNECTED");
        }
        mHost->TransitionTo(mHost->mActedState);
        retVal = HANDLED;
    }
    else if (msgWhat == EVENT_CLEANUP_ALL) {
        if (DBG) {
            mHost->Log("ActingState: EVENT_CLEANUP_ALL" );
        }
        mHost->RequestDataIdle();
        mHost->TransitionTo(mHost->mDeactingState);
        retVal = HANDLED;
    }
    else {
        if (VDBG) {
            mHost->Log("ActingState: nothandled msg.what=0x%08X",
                    StringUtils::ToHexString(msgWhat).string());
        }
        retVal = NOT_HANDLED;
    }
    *result = retVal;
    return NOERROR;
}

String DcSwitchState::ActingState::GetName()
{
    return String("DcSwitchState::IdleState");
}

//=============================================================================
// DcSwitchState::ActedState
//=============================================================================
DcSwitchState::ActedState::ActedState(
    /* [in] */ DcSwitchState* host)
    : mHost(host)
{}

ECode DcSwitchState::ActedState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 msgWhat;
    msg->GetWhat(&msgWhat);
    AutoPtr<IInterface> msgObj;
    msg->GetObj((IInterface**)&msgObj);
    Boolean retVal;
    if (msgWhat == DcSwitchAsyncChannel::REQ_CONNECT
            || msgWhat == EVENT_CONNECT) {
        String type;
        IObject::Probe(msgObj)->ToString(&type);
        if (DBG) {
            mHost->Log("ActedState: REQ_CONNECT/EVENT_CONNECT(%d) type=%s", msgWhat, type.string());
        }
        Int32 result;
        mHost->SetupConnection(type, &result);
        if (msgWhat == DcSwitchAsyncChannel::REQ_CONNECT) {
            mHost->mAc->ReplyToMessage(msg, DcSwitchAsyncChannel::RSP_CONNECT, result);
        }
        retVal = HANDLED;
    }
    else if (msgWhat == DcSwitchAsyncChannel::REQ_DISCONNECT) {
        String type;
        IObject::Probe(msgObj)->ToString(&type);
        if (DBG) {
            mHost->Log("ActedState: DcSwitchAsyncChannel.REQ_DISCONNECT type=%s", type.string());
        }
        Int32 result;
        mHost->TeardownConnection(type, &result);
        mHost->mAc->ReplyToMessage(msg, DcSwitchAsyncChannel::RSP_DISCONNECT, result);
        retVal = HANDLED;
    }
    else if (msgWhat == EVENT_CONNECTED) {
        if (DBG) {
            mHost->Log("ActedState: EVENT_CONNECTED");
        }
        retVal = HANDLED;
    }
    else if (msgWhat == EVENT_CLEANUP_ALL) {
        if (DBG) {
            mHost->Log("ActedState: EVENT_CLEANUP_ALL" );
        }
        mHost->RequestDataIdle();
        mHost->TransitionTo(mHost->mDeactingState);
        retVal = HANDLED;
    }
    else {
        if (VDBG) {
            mHost->Log("ActingState: nothandled msg.what=0x%08X",
                    StringUtils::ToHexString(msgWhat).string());
        }
        retVal = NOT_HANDLED;
    }
    *result = retVal;
    return NOERROR;
}

String DcSwitchState::ActedState::GetName()
{
    return String("DcSwitchState::IdleState");
}

//=============================================================================
// DcSwitchState::DeactingState
//=============================================================================
DcSwitchState::DeactingState::DeactingState(
    /* [in] */ DcSwitchState* host)
    : mHost(host)
{}

ECode DcSwitchState::DeactingState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 msgWhat;
    msg->GetWhat(&msgWhat);
    AutoPtr<IInterface> msgObj;
    msg->GetObj((IInterface**)&msgObj);
    Boolean retVal;
    if (msgWhat == DcSwitchAsyncChannel::REQ_CONNECT
            || msgWhat == EVENT_CONNECT) {
        String type;
        IObject::Probe(msgObj)->ToString(&type);
        if (DBG) {
            mHost->Log("DeactingState: REQ_CONNECT/EVENT_CONNECT(%d) type=%s, request is defered.",
                msgWhat, type.string());
        }
        AutoPtr<IMessage> msg;
        mHost->ObtainMessage(EVENT_CONNECT, StringUtils::ParseCharSequence(type), (IMessage**)&msg);
        mHost->DeferMessage(msg);
        if (msgWhat == DcSwitchAsyncChannel::REQ_CONNECT) {
            mHost->mAc->ReplyToMessage(msg, DcSwitchAsyncChannel::RSP_CONNECT,
                    IPhoneConstants::APN_REQUEST_STARTED);
        }
        retVal = HANDLED;
    }
    else if (msgWhat == DcSwitchAsyncChannel::REQ_DISCONNECT) {
        String type;
        IObject::Probe(msgObj)->ToString(&type);
        if (DBG) {
            mHost->Log("DeactingState: DcSwitchAsyncChannel.REQ_DISCONNECT type=%s", type.string());
        }
        mHost->mAc->ReplyToMessage(msg, DcSwitchAsyncChannel::RSP_DISCONNECT,
                IPhoneConstants::APN_ALREADY_INACTIVE);
        retVal = HANDLED;
    }
    else if (msgWhat == EVENT_DETACH_DONE) {
        if (DBG) {
            mHost->Log("DeactingState: EVENT_DETACH_DONE");
        }
        mHost->TransitionTo(mHost->mIdleState);
        retVal = HANDLED;
    }
    else if (msgWhat == EVENT_CONNECTED) {
        if (DBG) {
            mHost->Log("DeactingState: Receive invalid event EVENT_CONNECTED!");
        }
        retVal = HANDLED;
    }
    else if (msgWhat == EVENT_CLEANUP_ALL) {
        if (DBG) {
            mHost->Log("DeactingState: EVENT_CLEANUP_ALL, already deacting." );
        }
        retVal = HANDLED;
    }
    else {
        if (VDBG) {
            mHost->Log("DeactingState: nothandled msg.what=0x%s",
                    StringUtils::ToHexString(msgWhat).string());
        }
        retVal = NOT_HANDLED;
    }
    *result = retVal;
    return NOERROR;
}


String DcSwitchState::DeactingState::GetName()
{
    return String("DcSwitchState::IdleState");
}

//=============================================================================
// DcSwitchState::DefaultState
//=============================================================================
DcSwitchState::DefaultState::DefaultState(
    /* [in] */ DcSwitchState* host)
    : mHost(host)
{}

ECode DcSwitchState::DefaultState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 msgWhat;
    msg->GetWhat(&msgWhat);
    AutoPtr<IInterface> msgObj;
    msg->GetObj((IInterface**)&msgObj);
    AutoPtr<IMessenger> msgReplyTo;
    msg->GetReplyTo((IMessenger**)&msgReplyTo);

    AutoPtr<IAsyncResult> ar;
    if (msgWhat == IAsyncChannel::CMD_CHANNEL_FULL_CONNECTION) {
        if (mHost->mAc != NULL) {
            if (VDBG) mHost->Log("Disconnecting to previous connection mAc=%s", TO_CSTR(mHost->mAc));
            mHost->mAc->ReplyToMessage(msg, IAsyncChannel::CMD_CHANNEL_FULLY_CONNECTED,
                    IAsyncChannel::STATUS_FULL_CONNECTION_REFUSED_ALREADY_CONNECTED);
        } else {
            CAsyncChannel::New((IAsyncChannel**)&mHost->mAc);
            AutoPtr<IHandler> handler;
            mHost->GetHandler((IHandler**)&handler);
            mHost->mAc->Connected(NULL, handler, msgReplyTo);
            if (VDBG) mHost->Log("DcDefaultState: FULL_CONNECTION reply connected");
            mHost->mAc->ReplyToMessage(msg, IAsyncChannel::CMD_CHANNEL_FULLY_CONNECTED,
                    IAsyncChannel::STATUS_SUCCESSFUL, mHost->mId, StringUtils::ParseCharSequence(String("hi")));
        }
    }
    else if (msgWhat == IAsyncChannel::CMD_CHANNEL_DISCONNECT) {
        if (VDBG) mHost->Log("CMD_CHANNEL_DISCONNECT");
        mHost->mAc->Disconnect();
    }
    else if (msgWhat == IAsyncChannel::CMD_CHANNEL_DISCONNECTED) {
        if (VDBG) mHost->Log("CMD_CHANNEL_DISCONNECTED");
        mHost->mAc = NULL;
    }
    else if (msgWhat == DcSwitchAsyncChannel::REQ_IS_IDLE_STATE) {
        Boolean val = mHost->GetCurrentState() == mHost->mIdleState;
        if (VDBG) mHost->Log("REQ_IS_IDLE_STATE  isIdle=%d", val);
        mHost->mAc->ReplyToMessage(msg, DcSwitchAsyncChannel::RSP_IS_IDLE_STATE, val ? 1 : 0);
    }
    else if (msgWhat == DcSwitchAsyncChannel::REQ_IS_IDLE_OR_DEACTING_STATE) {
        Boolean val = (mHost->GetCurrentState() == mHost->mIdleState || mHost->GetCurrentState() == mHost->mDeactingState);
        if (VDBG) mHost->Log("REQ_IS_IDLE_OR_DEACTING_STATE  isIdleDeacting=%d", val);
        mHost->mAc->ReplyToMessage(msg, DcSwitchAsyncChannel::RSP_IS_IDLE_OR_DEACTING_STATE, val ? 1 : 0);
    }
    else if (msgWhat == EVENT_TO_ACTING_DIRECTLY) {
        mHost->Log("Just transit to Acting state");
        mHost->TransitionTo(mHost->mActingState);
    }
    else if (msgWhat == EVENT_TO_IDLE_DIRECTLY) {
        mHost->Log("Just transit to Idle state");
        AutoPtr<IIterator> itrType;
        mHost->mApnTypes->GetIterator((IIterator**)&itrType);
        Boolean hasNext;
        while (itrType->HasNext(&hasNext), hasNext) {
//                        mPhone.disableApnType(itrType.next()); TODO
        }
        mHost->mApnTypes->Clear();
        mHost->TransitionTo(mHost->mIdleState);
    }
    else {
        if (DBG) {
            mHost->Log("DefaultState: shouldn't happen but ignore msg.what=0x%08X",
                    StringUtils::ToHexString(msgWhat).string());
        }
    }
    *result = HANDLED;
    return NOERROR;
}

String DcSwitchState::DefaultState::GetName()
{
    return String("DcSwitchState::IdleState");
}

//=============================================================================
// DcSwitchState
//=============================================================================
CAR_INTERFACE_IMPL(DcSwitchState, StateMachine, IDcSwitchState)

const Boolean DcSwitchState::DBG = TRUE;
const Boolean DcSwitchState::VDBG = FALSE;
const String DcSwitchState::LOG__TAG("DcSwitchState");
const Int32 DcSwitchState::BASE = IProtocol::BASE_DATA_CONNECTION_TRACKER + 0x00001000;
const Int32 DcSwitchState::EVENT_CONNECT = BASE + 0;
const Int32 DcSwitchState::EVENT_DISCONNECT = BASE + 1;
const Int32 DcSwitchState::EVENT_CLEANUP_ALL = BASE + 2;
const Int32 DcSwitchState::EVENT_CONNECTED = BASE + 3;
const Int32 DcSwitchState::EVENT_DETACH_DONE = BASE + 4;
const Int32 DcSwitchState::EVENT_TO_IDLE_DIRECTLY = BASE + 5;
const Int32 DcSwitchState::EVENT_TO_ACTING_DIRECTLY = BASE + 6;

DcSwitchState::DcSwitchState()
{
    assert(0 && "TODO CRegistrantList");
    // CRegistrantList::New((IRegistrantList**)&mIdleRegistrants);
    CHashSet::New((IHashSet**)&mApnTypes);
    mIdleState = new IdleState(this);
    mActingState = new ActingState(this);
    mActedState = new ActedState(this);
    mDeactingState = new DeactingState(this);
    mDefaultState = new DefaultState(this);
}

ECode DcSwitchState::constructor(
    /* [in] */ IPhone* phone,
    /* [in] */ const String& name,
    /* [in] */ Int32 id)
{
    StateMachine::constructor(name);
    if (DBG) Log("DcSwitchState constructor E");
    mPhone = phone;
    mId = id;
    AddState(mDefaultState);
    AddState(mIdleState, mDefaultState);
    AddState(mActingState, mDefaultState);
    AddState(mActedState, mDefaultState);
    AddState(mDeactingState, mDefaultState);
    SetInitialState(mIdleState);
    if (DBG) Log("DcSwitchState constructor X");
    return NOERROR;
}

ECode DcSwitchState::SetupConnection(
    /* [in] */ const String& type,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    mApnTypes->Add(StringUtils::ParseCharSequence(type));
    Log("DcSwitchState:setupConnection type = %s", type.string());
//        return mPhone.enableApnType(type); TODO
    return IPhoneConstants::APN_REQUEST_STARTED;
}

ECode DcSwitchState::TeardownConnection(
    /* [in] */ const String& type,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    mApnTypes->Remove(StringUtils::ParseCharSequence(type));
    Boolean isEmpty;
    mApnTypes->IsEmpty(&isEmpty);
    if (isEmpty) {
        Log("No APN is using, then clean up all");
        // Since last type is removed from mApnTypes and will not be disabled in requestDataIdle()
//            mPhone.disableApnType(type); TODO
        RequestDataIdle();
        TransitionTo(mDeactingState);
        *result = IPhoneConstants::APN_REQUEST_STARTED;
    } else {
//            return mPhone.disableApnType(type); TODO
        *result = IPhoneConstants::APN_REQUEST_STARTED;
    }
    return NOERROR;
}

ECode DcSwitchState::RequestDataIdle()
{
    if (DBG) Log("requestDataIdle is triggered");
    AutoPtr<IIterator> itrType;
    mApnTypes->GetIterator((IIterator**)&itrType);
    Boolean hasNext;
    while (itrType->HasNext(&hasNext), hasNext) {
//            mPhone.disableApnType(itrType.next()); TODO
    }
    mApnTypes->Clear();
    AutoPtr<IPhone> phone;
    IPhoneProxy::Probe(mPhone)->GetActivePhone((IPhone**)&phone);
    AutoPtr<IPhoneBase> pb = IPhoneBase::Probe(phone);
    AutoPtr<IMessage> msg;
    ObtainMessage(EVENT_DETACH_DONE, (IMessage**)&msg);
    ((PhoneBase*) pb.Get())->mCi->SetDataAllowed(FALSE, msg);
    return NOERROR;
}

ECode DcSwitchState::NotifyDataConnection(
    /* [in] */ Int32 phoneId,
    /* [in] */ const String& state,
    /* [in] */ const String& reason,
    /* [in] */ const String& apnName,
    /* [in] */ const String& apnType,
    /* [in] */ Boolean unavailable)
{
    if (phoneId == mId &&
            TextUtils::Equals(state, StringUtils::ToString(PhoneConstantsDataState_CONNECTED))) {
        AutoPtr<IMessage> msg;
        ObtainMessage(EVENT_CONNECTED, (IMessage**)&msg);
        SendMessage(msg);
    }
    return NOERROR;
}

ECode DcSwitchState::CleanupAllConnection()
{
    AutoPtr<IMessage> msg;
    ObtainMessage(EVENT_CLEANUP_ALL, (IMessage**)&msg);
    SendMessage(msg);
    return NOERROR;
}

ECode DcSwitchState::RegisterForIdle(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IRegistrant> r;
    assert(0 && "TODO CRegistrant IRegistrant");
    // CRegistrant::New((IRegistrant**)&r);
    // mIdleRegistrants->Add(r);
    return NOERROR;
}

ECode DcSwitchState::UnregisterForIdle(
    /* [in] */ IHandler* h)
{
    assert(0 && "TODO CRegistrant IRegistrant");
    // return mIdleRegistrants->Remove(h);
    return NOERROR;
}

ECode DcSwitchState::TransitToIdleState()
{
    AutoPtr<IMessage> msg;
    ObtainMessage(EVENT_TO_IDLE_DIRECTLY, (IMessage**)&msg);
    SendMessage(msg);
    return NOERROR;
}

ECode DcSwitchState::TransitToActingState()
{
    AutoPtr<IMessage> msg;
    ObtainMessage(EVENT_TO_ACTING_DIRECTLY, (IMessage**)&msg);
    SendMessage(msg);
    return NOERROR;
}

#define MSG_BUF_SIZE    1024
ECode DcSwitchState::Log(
    /* [in] */ const char *fmt, ...)
{
    char msgBuf[MSG_BUF_SIZE];
    va_list args;
    va_start(args, fmt);
    vsnprintf(msgBuf, MSG_BUF_SIZE, fmt, args);
    va_end(args);

    String name;
    GetName(&name);
    return Logger::D(LOG__TAG, "[%s] %s", name.string(), msgBuf);
}

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
