
#include "elastos/droid/server/telecom/InCallAdapter.h"
#include "elastos/droid/server/telecom/Call.h"
#include "elastos/droid/server/telecom/CallIdMapper.h"
#include "elastos/droid/server/telecom/Log.h"
#include "elastos/droid/server/telecom/ThreadUtil.h"
#include <elastos/droid/internal/os/SomeArgs.h>
#include <elastos/core/StringUtils.h>

using Elastos::Droid::Internal::Os::SomeArgs;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Telecomm::Internal::EIID_IIInCallAdapter;
using Elastos::Core::CBoolean;
using Elastos::Core::CInteger32;
using Elastos::Core::IBoolean;
using Elastos::Core::IChar32;
using Elastos::Core::IInteger32;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

//=============================================================================
// InCallAdapter::InCallAdapterHandler
//=============================================================================
InCallAdapter::InCallAdapterHandler::InCallAdapterHandler(
    /* [in] */ InCallAdapter* host)
    : mHost(host)
{}

ECode InCallAdapter::InCallAdapterHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 msgWhat;
    msg->GetWhat(&msgWhat);
    AutoPtr<IInterface> msgObj;
    msg->GetObj((IInterface**)&msgObj);
    Int32 msgArg1;
    msg->GetArg1(&msgArg1);
    AutoPtr<ICall> call;
    if (msgWhat == MSG_ANSWER_CALL) {
        AutoPtr<SomeArgs> args = (SomeArgs*) IObject::Probe(msgObj);
        // try {
        ((CallIdMapper*) mHost->mCallIdMapper.Get())->GetCall(args->mArg1, (ICall**)&call);
        Int32 videoState;
        IInteger32::Probe(args->mArg2)->GetValue(&videoState);
        if (call != NULL) {
            mHost->mCallsManager->AnswerCall(call, videoState);
        } else {
            Log::W("InCallAdapter", "answerCall, unknown call id: %s", TO_CSTR(msgObj));
        }
        // } finally {
        args->Recycle();
        // }
    }
    else if (msgWhat == MSG_REJECT_CALL) {
        AutoPtr<SomeArgs> args = (SomeArgs*) IObject::Probe(msgObj);
        // try {
        ((CallIdMapper*) mHost->mCallIdMapper.Get())->GetCall(args->mArg1, (ICall**)&call);
        Boolean rejectWithMessage = args->mArgi1 == 1;
        String textMessage;
        IObject::Probe(args->mArg2)->ToString(&textMessage);
        if (call != NULL) {
            mHost->mCallsManager->RejectCall(call, rejectWithMessage, textMessage);
        } else {
            Log::W("InCallAdapter", "setRingback, unknown call id: %s", TO_CSTR(args->mArg1));
        }
        // } finally {
        args->Recycle();
        // }
    }
    else if (msgWhat == MSG_PLAY_DTMF_TONE) {
        ((CallIdMapper*) mHost->mCallIdMapper.Get())->GetCall(msgObj, (ICall**)&call);
        if (call != NULL) {
            Char32 c = msgArg1;
            mHost->mCallsManager->PlayDtmfTone(call, c);
        } else {
            Log::W("InCallAdapter", "playDtmfTone, unknown call id: %s", TO_CSTR(msgObj));
        }
    }
    else if (msgWhat == MSG_STOP_DTMF_TONE) {
        ((CallIdMapper*) mHost->mCallIdMapper.Get())->GetCall(msgObj, (ICall**)&call);
        if (call != NULL) {
            mHost->mCallsManager->StopDtmfTone(call);
        } else {
            Log::W("InCallAdapter", "stopDtmfTone, unknown call id: %s", TO_CSTR(msgObj));
        }
    }
    else if (msgWhat == MSG_POST_DIAL_CONTINUE) {
        ((CallIdMapper*) mHost->mCallIdMapper.Get())->GetCall(msgObj, (ICall**)&call);
        mHost->mCallsManager->PostDialContinue(call, msgArg1 == 1);
        ((CallIdMapper*) mHost->mCallIdMapper.Get())->GetCall(msgObj, (ICall**)&call);
        if (call != NULL) {
            mHost->mCallsManager->PostDialContinue(call, msgArg1 == 1);
        } else {
            Log::W("InCallAdapter", "postDialContinue, unknown call id: %s", TO_CSTR(msgObj));
        }
    }
    else if (msgWhat == MSG_DISCONNECT_CALL) {
        ((CallIdMapper*) mHost->mCallIdMapper.Get())->GetCall(msgObj, (ICall**)&call);
        if (call != NULL) {
            mHost->mCallsManager->DisconnectCall(call);
        } else {
            Log::W("InCallAdapter", "disconnectCall, unknown call id: %s", TO_CSTR(msgObj));
        }
    }
    else if (msgWhat == MSG_HOLD_CALL) {
        ((CallIdMapper*) mHost->mCallIdMapper.Get())->GetCall(msgObj, (ICall**)&call);
        if (call != NULL) {
            mHost->mCallsManager->HoldCall(call);
        } else {
            Log::W("InCallAdapter", "holdCall, unknown call id: %s", TO_CSTR(msgObj));
        }
    }
    else if (msgWhat == MSG_UNHOLD_CALL) {
        ((CallIdMapper*) mHost->mCallIdMapper.Get())->GetCall(msgObj, (ICall**)&call);
        if (call != NULL) {
            mHost->mCallsManager->UnholdCall(call);
        } else {
            Log::W("InCallAdapter", "unholdCall, unknown call id: %s", TO_CSTR(msgObj));
        }
    }
    else if (msgWhat == MSG_PHONE_ACCOUNT_SELECTED) {
        AutoPtr<SomeArgs> args = (SomeArgs*) IObject::Probe(msgObj);
        // try {
        ((CallIdMapper*) mHost->mCallIdMapper.Get())->GetCall(args->mArg1, (ICall**)&call);
        if (call != NULL) {
            mHost->mCallsManager->PhoneAccountSelected(call, IPhoneAccountHandle::Probe(args->mArg2));
        } else {
            Log::W("InCallAdapter", "phoneAccountSelected, unknown call id: %s", TO_CSTR(args->mArg1));
        }
        // } finally {
        args->Recycle();
        // }
    }
    else if (msgWhat == MSG_MUTE) {
        mHost->mCallsManager->Mute(msgArg1 == 1);
    }
    else if (msgWhat == MSG_SET_AUDIO_ROUTE) {
        mHost->mCallsManager->SetAudioRoute(msgArg1);
    }
    else if (msgWhat == MSG_CONFERENCE) {
        AutoPtr<SomeArgs> args = (SomeArgs*) IObject::Probe(msgObj);
        // try {
        ((CallIdMapper*) mHost->mCallIdMapper.Get())->GetCall(args->mArg1, (ICall**)&call);
        AutoPtr<ICall> otherCall;
        ((CallIdMapper*) mHost->mCallIdMapper.Get())->GetCall(args->mArg2, (ICall**)&otherCall);
        if (call != NULL && otherCall != NULL) {
            mHost->mCallsManager->Conference(call, otherCall);
        } else {
            Log::W("InCallAdapter", "conference, unknown call id: %s", TO_CSTR(msgObj));
        }
        // } finally {
        args->Recycle();
        // }
    }
    else if (msgWhat == MSG_SPLIT_FROM_CONFERENCE) {
        ((CallIdMapper*) mHost->mCallIdMapper.Get())->GetCall(msgObj, (ICall**)&call);
        if (call != NULL) {
            ((Call*) call.Get())->SplitFromConference();
        } else {
            Log::W("InCallAdapter", "splitFromConference, unknown call id: %s", TO_CSTR(msgObj));
        }
    }
    else if (msgWhat == MSG_TURN_ON_PROXIMITY_SENSOR) {
        mHost->mCallsManager->TurnOnProximitySensor();
    }
    else if (msgWhat == MSG_TURN_OFF_PROXIMITY_SENSOR) {
        Boolean b;
        IBoolean::Probe(msgObj)->GetValue(&b);
        mHost->mCallsManager->TurnOffProximitySensor(b);
    }
    else if (msgWhat == MSG_MERGE_CONFERENCE) {
        ((CallIdMapper*) mHost->mCallIdMapper.Get())->GetCall(msgObj, (ICall**)&call);
        if (call != NULL) {
            ((Call*) call.Get())->MergeConference();
        } else {
            Log::W("InCallAdapter", "mergeConference, unknown call id: %s", TO_CSTR(msgObj));
        }
    }
    else if (msgWhat == MSG_SWAP_CONFERENCE) {
        ((CallIdMapper*) mHost->mCallIdMapper.Get())->GetCall(msgObj, (ICall**)&call);
        if (call != NULL) {
            ((Call*) call.Get())->SwapConference();
        } else {
            Log::W("InCallAdapter", "swapConference, unknown call id: %s", TO_CSTR(msgObj));
        }
    }
    else if (msgWhat == MSG_DEFLECT_CALL) {
        AutoPtr<SomeArgs> args = (SomeArgs*) IObject::Probe(msgObj);
        // try {
        ((CallIdMapper*) mHost->mCallIdMapper.Get())->GetCall(args->mArg1, (ICall**)&call);
        String number;
        IObject::Probe(args->mArg2)->ToString(&number);
        if (call != NULL) {
            mHost->mCallsManager->DeflectCall(call, number);
        } else {
            Log::W("InCallAdapter", "deflectCall, unknown call id: %s", TO_CSTR(args->mArg1));
        }
        // } finally {
        args->Recycle();
        // }
    }
    else if (msgWhat == MSG_SET_SWITCH_SUBSCRIPTION) {
        AutoPtr<SomeArgs> args = (SomeArgs*) IObject::Probe(msgObj);
        // try {
        Log::I("InCallAdapter", "MSG_SET_SWITCH_SUBSCRIPTION sub: %s, lchState: %s",
                TO_CSTR(args->mArg1), TO_CSTR(args->mArg2));
        Boolean b;
        IBoolean::Probe(args->mArg2)->GetValue(&b);
        mHost->mCallsManager->SwitchToOtherActiveSub(TO_STR(args->mArg1), b);
        // } finally {
        args->Recycle();
        // }
    }
    return NOERROR;
}

//=============================================================================
// InCallAdapter
//=============================================================================
CAR_INTERFACE_IMPL_2(InCallAdapter, Object, IIInCallAdapter, IBinder)

const Int32 InCallAdapter::MSG_ANSWER_CALL = 0;
const Int32 InCallAdapter::MSG_REJECT_CALL = 1;
const Int32 InCallAdapter::MSG_PLAY_DTMF_TONE = 2;
const Int32 InCallAdapter::MSG_STOP_DTMF_TONE = 3;
const Int32 InCallAdapter::MSG_POST_DIAL_CONTINUE = 4;
const Int32 InCallAdapter::MSG_DISCONNECT_CALL = 5;
const Int32 InCallAdapter::MSG_HOLD_CALL = 6;
const Int32 InCallAdapter::MSG_UNHOLD_CALL = 7;
const Int32 InCallAdapter::MSG_MUTE = 8;
const Int32 InCallAdapter::MSG_SET_AUDIO_ROUTE = 9;
const Int32 InCallAdapter::MSG_CONFERENCE = 10;
const Int32 InCallAdapter::MSG_SPLIT_FROM_CONFERENCE = 11;
const Int32 InCallAdapter::MSG_SWAP_WITH_BACKGROUND_CALL = 12;
const Int32 InCallAdapter::MSG_PHONE_ACCOUNT_SELECTED = 13;
const Int32 InCallAdapter::MSG_TURN_ON_PROXIMITY_SENSOR = 14;
const Int32 InCallAdapter::MSG_TURN_OFF_PROXIMITY_SENSOR = 15;
const Int32 InCallAdapter::MSG_MERGE_CONFERENCE = 16;
const Int32 InCallAdapter::MSG_SWAP_CONFERENCE = 17;
const Int32 InCallAdapter::MSG_SET_SWITCH_SUBSCRIPTION = 18;
const Int32 InCallAdapter::MSG_DEFLECT_CALL = 19;

ECode InCallAdapter::constructor(
    /* [in] */ CallsManager* callsManager,
    /* [in] */ ICallIdMapper* callIdMapper)
{
    mHandler = new InCallAdapterHandler(this);
    ThreadUtil::CheckOnMainThread();
    mCallsManager = callsManager;
    mCallIdMapper = callIdMapper;
    return NOERROR;
}

ECode InCallAdapter::AnswerCall(
    /* [in] */ const String& callId,
    /* [in] */ Int32 videoState)
{
    Log::D("InCallAdapter", "answerCall(%s,%d)", callId.string(), videoState);
    Boolean isValidCallId;
    ((CallIdMapper*) mCallIdMapper.Get())->IsValidCallId(callId, &isValidCallId);
    if (isValidCallId) {
        AutoPtr<SomeArgs> args = SomeArgs::Obtain();
        args->mArg1 = StringUtils::ParseCharSequence(callId);
        AutoPtr<IInteger32> i32;
        CInteger32::New(videoState, (IInteger32**)&i32);
        args->mArg2 = i32;
        AutoPtr<IMessage> msg;
        mHandler->ObtainMessage(MSG_ANSWER_CALL, TO_IINTERFACE(args), (IMessage**)&msg);
        msg->SendToTarget();
    }
    return NOERROR;
}

ECode InCallAdapter::DeflectCall(
    /* [in] */ const String& callId,
    /* [in] */ const String& deflectNumber)
{
    Log::D("InCallAdapter", "deflectCall(%s)-(%s)", callId.string(), deflectNumber.string());
    Boolean isValidCallId;
    ((CallIdMapper*) mCallIdMapper.Get())->IsValidCallId(callId, &isValidCallId);
    if (isValidCallId) {
        AutoPtr<SomeArgs> args = SomeArgs::Obtain();
        args->mArg1 = StringUtils::ParseCharSequence(callId);
        args->mArg2 = StringUtils::ParseCharSequence(deflectNumber);
        AutoPtr<IMessage> msg;
        mHandler->ObtainMessage(MSG_DEFLECT_CALL, TO_IINTERFACE(args), (IMessage**)&msg);
        msg->SendToTarget();
    }
    return NOERROR;
}

ECode InCallAdapter::RejectCall(
    /* [in] */ const String& callId,
    /* [in] */ Boolean rejectWithMessage,
    /* [in] */ const String& textMessage)
{
    Log::D("InCallAdapter", "rejectCall(%s,%d,%s)", callId.string(), rejectWithMessage, textMessage.string());
    Boolean isValidCallId;
    ((CallIdMapper*) mCallIdMapper.Get())->IsValidCallId(callId, &isValidCallId);
    if (isValidCallId) {
        AutoPtr<SomeArgs> args = SomeArgs::Obtain();
        args->mArg1 = StringUtils::ParseCharSequence(callId);
        args->mArgi1 = rejectWithMessage ? 1 : 0;
        args->mArg2 = StringUtils::ParseCharSequence(textMessage);
        AutoPtr<IMessage> msg;
        mHandler->ObtainMessage(MSG_REJECT_CALL, TO_IINTERFACE(args), (IMessage**)&msg);
        msg->SendToTarget();
    }
    return NOERROR;
}

ECode InCallAdapter::PlayDtmfTone(
    /* [in] */ const String& callId,
    /* [in] */ Char32 digit)
{
    Log::D("InCallAdapter", "playDtmfTone(%s,%c)", callId.string(), digit);
    Boolean isValidCallId;
    ((CallIdMapper*) mCallIdMapper.Get())->IsValidCallId(callId, &isValidCallId);
    if (isValidCallId) {
        AutoPtr<IMessage> msg;
        mHandler->ObtainMessage(MSG_PLAY_DTMF_TONE, (Int32) digit, 0, StringUtils::ParseCharSequence(callId), (IMessage**)&msg);
        msg->SendToTarget();
    }
    return NOERROR;
}

ECode InCallAdapter::StopDtmfTone(
    /* [in] */ const String& callId)
{
    Log::D("InCallAdapter", "stopDtmfTone(%s)", callId.string());
    Boolean isValidCallId;
    ((CallIdMapper*) mCallIdMapper.Get())->IsValidCallId(callId, &isValidCallId);
    if (isValidCallId) {
        AutoPtr<IMessage> msg;
        mHandler->ObtainMessage(MSG_STOP_DTMF_TONE, StringUtils::ParseCharSequence(callId), (IMessage**)&msg);
        msg->SendToTarget();
    }
    return NOERROR;
}

ECode InCallAdapter::PostDialContinue(
    /* [in] */ const String& callId,
    /* [in] */ Boolean proceed)
{
    Log::D("InCallAdapter", "postDialContinue(%s)", callId.string());
    Boolean isValidCallId;
    ((CallIdMapper*) mCallIdMapper.Get())->IsValidCallId(callId, &isValidCallId);
    if (isValidCallId) {
        AutoPtr<IMessage> msg;
        mHandler->ObtainMessage(MSG_POST_DIAL_CONTINUE, proceed ? 1 : 0, 0, StringUtils::ParseCharSequence(callId), (IMessage**)&msg);
        msg->SendToTarget();
    }
    return NOERROR;
}

ECode InCallAdapter::DisconnectCall(
    /* [in] */ const String& callId)
{
    Log::V("InCallAdapter", "disconnectCall: %s", callId.string());
    Boolean isValidCallId;
    ((CallIdMapper*) mCallIdMapper.Get())->IsValidCallId(callId, &isValidCallId);
    if (isValidCallId) {
        AutoPtr<IMessage> msg;
        mHandler->ObtainMessage(MSG_DISCONNECT_CALL, StringUtils::ParseCharSequence(callId), (IMessage**)&msg);
        msg->SendToTarget();
    }
    return NOERROR;
}

ECode InCallAdapter::HoldCall(
    /* [in] */ const String& callId)
{
    Boolean isValidCallId;
    ((CallIdMapper*) mCallIdMapper.Get())->IsValidCallId(callId, &isValidCallId);
    if (isValidCallId) {
        AutoPtr<IMessage> msg;
        mHandler->ObtainMessage(MSG_HOLD_CALL, StringUtils::ParseCharSequence(callId), (IMessage**)&msg);
        msg->SendToTarget();
    }
    return NOERROR;
}

ECode InCallAdapter::UnholdCall(
    /* [in] */ const String& callId)
{
    Boolean isValidCallId;
    ((CallIdMapper*) mCallIdMapper.Get())->IsValidCallId(callId, &isValidCallId);
    if (isValidCallId) {
        AutoPtr<IMessage> msg;
        mHandler->ObtainMessage(MSG_UNHOLD_CALL, StringUtils::ParseCharSequence(callId), (IMessage**)&msg);
        msg->SendToTarget();
    }
    return NOERROR;
}

ECode InCallAdapter::PhoneAccountSelected(
    /* [in] */ const String& callId,
    /* [in] */ IPhoneAccountHandle* accountHandle)
{
    Boolean isValidCallId;
    ((CallIdMapper*) mCallIdMapper.Get())->IsValidCallId(callId, &isValidCallId);
    if (isValidCallId) {
        AutoPtr<SomeArgs> args = SomeArgs::Obtain();
        args->mArg1 = StringUtils::ParseCharSequence(callId);
        args->mArg2 = accountHandle;
        AutoPtr<IMessage> msg;
        mHandler->ObtainMessage(MSG_PHONE_ACCOUNT_SELECTED, TO_IINTERFACE(args), (IMessage**)&msg);
        msg->SendToTarget();
    }
    return NOERROR;
}

ECode InCallAdapter::Mute(
    /* [in] */ Boolean shouldMute)
{
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(MSG_MUTE, shouldMute ? 1 : 0, 0, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode InCallAdapter::SetAudioRoute(
    /* [in] */ Int32 route)
{
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(MSG_SET_AUDIO_ROUTE, route, 0, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode InCallAdapter::Conference(
    /* [in] */ const String& callId,
    /* [in] */ const String& otherCallId)
{
    Boolean isValidCallId;
    ((CallIdMapper*) mCallIdMapper.Get())->IsValidCallId(callId, &isValidCallId);
    Boolean isOtherValidCallId;
    ((CallIdMapper*) mCallIdMapper.Get())->IsValidCallId(otherCallId, &isOtherValidCallId);
    if (isValidCallId &&
            isOtherValidCallId) {
        AutoPtr<SomeArgs> args = SomeArgs::Obtain();
        args->mArg1 = StringUtils::ParseCharSequence(callId);
        args->mArg2 = StringUtils::ParseCharSequence(otherCallId);
        AutoPtr<IMessage> msg;
        mHandler->ObtainMessage(MSG_CONFERENCE, TO_IINTERFACE(args), (IMessage**)&msg);
        msg->SendToTarget();
    }
    return NOERROR;
}

ECode InCallAdapter::SplitFromConference(
    /* [in] */ const String& callId)
{
    Boolean isValidCallId;
    ((CallIdMapper*) mCallIdMapper.Get())->IsValidCallId(callId, &isValidCallId);
    if (isValidCallId) {
        AutoPtr<IMessage> msg;
        mHandler->ObtainMessage(MSG_SPLIT_FROM_CONFERENCE, StringUtils::ParseCharSequence(callId), (IMessage**)&msg);
        msg->SendToTarget();
    }
    return NOERROR;
}

ECode InCallAdapter::MergeConference(
    /* [in] */ const String& callId)
{
    Boolean isValidCallId;
    ((CallIdMapper*) mCallIdMapper.Get())->IsValidCallId(callId, &isValidCallId);
    if (isValidCallId) {
        AutoPtr<IMessage> msg;
        mHandler->ObtainMessage(MSG_MERGE_CONFERENCE, StringUtils::ParseCharSequence(callId), (IMessage**)&msg);
        msg->SendToTarget();
    }
    return NOERROR;
}

ECode InCallAdapter::SwapConference(
    /* [in] */ const String& callId)
{
    Boolean isValidCallId;
    ((CallIdMapper*) mCallIdMapper.Get())->IsValidCallId(callId, &isValidCallId);
    if (isValidCallId) {
        AutoPtr<IMessage> msg;
        mHandler->ObtainMessage(MSG_SWAP_CONFERENCE, StringUtils::ParseCharSequence(callId), (IMessage**)&msg);
        msg->SendToTarget();
    }
    return NOERROR;
}

ECode InCallAdapter::TurnOnProximitySensor()
{
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(MSG_TURN_ON_PROXIMITY_SENSOR, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode InCallAdapter::TurnOffProximitySensor(
    /* [in] */ Boolean screenOnImmediately)
{
    AutoPtr<IBoolean> ibool;
    CBoolean::New(screenOnImmediately, (IBoolean**)&ibool);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(MSG_TURN_OFF_PROXIMITY_SENSOR, ibool, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode InCallAdapter::SwitchToOtherActiveSub(
    /* [in] */ const String& sub,
    /* [in] */ Boolean retainLch)
{
    AutoPtr<SomeArgs> args = SomeArgs::Obtain();
    args->mArg1 = StringUtils::ParseCharSequence(sub);
    AutoPtr<IBoolean> ibool;
    CBoolean::New(retainLch, (IBoolean**)&ibool);
    args->mArg2 = ibool;
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(MSG_SET_SWITCH_SUBSCRIPTION, TO_IINTERFACE(args), (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode InCallAdapter::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    String rev("InCallAdapter");
    rev.AppendFormat(":%s;%s;%s", TO_CSTR(mCallsManager), TO_CSTR(mHandler), TO_CSTR(mCallIdMapper));
    *result = rev;
    return NOERROR;
}

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos
