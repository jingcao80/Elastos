#include "elastos/droid/internal/telephony/gsm/CGsmCall.h"
#include "elastos/droid/internal/telephony/gsm/CGsmCallTracker.h"
#include "elastos/droid/internal/telephony/gsm/CGsmConnection.h"
#include "elastos/droid/internal/telephony/gsm/CGSMPhone.h"
#include "elastos/droid/os/CRegistrant.h"
#include "elastos/droid/os/CRegistrantList.h"
#include "elastos/droid/os/RegistrantList.h"
#include "elastos/droid/os/SystemProperties.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Telephony::Gsm::IGsmCellLocation;
using Elastos::Droid::Telephony::ICellLocation;
using Elastos::Droid::Telephony::IDisconnectCause;
using Elastos::Droid::Telephony::IPhoneNumberUtils;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Droid::Os::CRegistrant;
using Elastos::Droid::Os::CRegistrantList;
using Elastos::Droid::Os::IRegistrant;
using Elastos::Droid::Os::RegistrantList;
using Elastos::Droid::Os::SystemProperties;
using Elastos::Core::ICloneable;
using Elastos::Core::IInteger32;
using Elastos::Core::StringUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Gsm {

const Int32 CGsmCallTracker::MAX_CONNECTIONS;   // only 7 connections allowed in GSM
const Int32 CGsmCallTracker::MAX_CONNECTIONS_PER_CALL; // only 5 connections allowed per call

const String CGsmCallTracker::TAG("GsmCallTracker");

const Boolean CGsmCallTracker::REPEAT_POLLING;

const Boolean CGsmCallTracker::DBG_POLL;

CAR_INTERFACE_IMPL(CGsmCallTracker, CallTracker, IGsmCallTracker)

CAR_OBJECT_IMPL(CGsmCallTracker)

CGsmCallTracker::CGsmCallTracker()
    : mHangupPendingMO(FALSE)
    , mDesiredMute(FALSE)
    , mState(PhoneConstantsState_IDLE)
    , mSrvccState(ICallSrvccState_NONE)
{
    mConnections = ArrayOf<IGsmConnection*>::Alloc(MAX_CONNECTIONS);
    CRegistrantList::New((IRegistrantList**)&mVoiceCallEndedRegistrants);
    CRegistrantList::New((IRegistrantList**)&mVoiceCallStartedRegistrants);

    // connections dropped during last poll
    CArrayList::New(MAX_CONNECTIONS, (IArrayList**)&mDroppedDuringPoll);

    CGsmCall::New(this, (IGsmCall**)&mRingingCall);
            // A call that is ringing or (call) waiting
    CGsmCall::New(this, (IGsmCall**)&mForegroundCall);
    CGsmCall::New(this, (IGsmCall**)&mBackgroundCall);
}

ECode CGsmCallTracker::constructor()
{
    return NOERROR;
}

ECode CGsmCallTracker::constructor(
    /* [in] */ IGSMPhone* phone)
{
    mPhone = phone;
    mCi = ((CGSMPhone*)phone)->mCi;

    mCi->RegisterForCallStateChanged(this, EVENT_CALL_STATE_CHANGE, NULL);

    mCi->RegisterForOn(this, EVENT_RADIO_AVAILABLE, NULL);
    mCi->RegisterForNotAvailable(this, EVENT_RADIO_NOT_AVAILABLE, NULL);
    return NOERROR;
}

ECode CGsmCallTracker::Dispose()
{
    Logger::D(TAG, "GsmCallTracker dispose");
    //Unregister for all events
    mCi->UnregisterForCallStateChanged(this);
    mCi->UnregisterForOn(this);
    mCi->UnregisterForNotAvailable(this);

    ClearDisconnected();
    return NOERROR;
}

ECode CGsmCallTracker::RegisterForVoiceCallStarted(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IRegistrant> r;
    CRegistrant::New(h, what, obj, (IRegistrant**)&r);
    ((RegistrantList*)mVoiceCallStartedRegistrants.Get())->Add(r);
    return NOERROR;
}

ECode CGsmCallTracker::UnregisterForVoiceCallStarted(
    /* [in] */ IHandler* h)
{
    ((RegistrantList*)mVoiceCallStartedRegistrants.Get())->Remove(h);
    return NOERROR;
}

ECode CGsmCallTracker::RegisterForVoiceCallEnded(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IRegistrant> r;
    CRegistrant::New(h, what, obj, (IRegistrant**)&r);
    ((RegistrantList*)mVoiceCallEndedRegistrants.Get())->Add(r);
    return NOERROR;
}

ECode CGsmCallTracker::UnregisterForVoiceCallEnded(
    /* [in] */ IHandler* h)
{
    ((RegistrantList*)mVoiceCallEndedRegistrants.Get())->Remove(h);
    return NOERROR;
}

ECode CGsmCallTracker::Dial(
    /* [in] */ const String& _dialString,
    /* [in] */ Int32 clirMode,
    /* [in] */ IUUSInfo* uusInfo,
    /* [out] */ IConnection** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    String dialString = _dialString;
    // note that this triggers call state changed notif
    ClearDisconnected();

    Boolean b;
    if (CanDial(&b), !b) {
        // throw new CallStateException("cannot dial in current state");
        return E_CALL_STATE_EXCEPTION;
    }

    String origNumber = dialString;
    dialString = ConvertNumberIfNecessary(IPhoneBase::Probe(mPhone), dialString);

    // The new call must be assigned to the foreground call.
    // That call must be idle, so place anything that's
    // there on hold
    ICallState state;
    ICall::Probe(mForegroundCall)->GetState(&state);
    if (state == ICallState_ACTIVE) {
        // this will probably be done by the radio anyway
        // but the dial might fail before this happens
        // and we need to make sure the foreground call is clear
        // for the newly dialed connection
        SwitchWaitingOrHoldingAndActive();

        // Fake local state so that
        // a) foregroundCall is empty for the newly dialed connection
        // b) hasNonHangupStateChanged remains false in the
        // next poll, so that we don't clear a failed dialing call
        FakeHoldForegroundBeforeDial();
    }

    ICall::Probe(mForegroundCall)->GetState(&state);
    if (state != ICallState_IDLE) {
        //we should have failed in !canDial() above before we get here
        // throw new CallStateException("cannot dial in current state");
        return E_CALL_STATE_EXCEPTION;
    }

    AutoPtr<IContext> context;
    ((CGSMPhone*)mPhone.Get())->GetContext((IContext**)&context);
    CGsmConnection::New(context, CheckForTestEmergencyNumber(dialString),
            this, mForegroundCall, (IGsmConnection**)&mPendingMO);
    mHangupPendingMO = FALSE;

    String addr;
    ((CGsmConnection*)mPendingMO.Get())->GetAddress(&addr);
    if ( addr.IsNull() || addr.GetLength() == 0
            || addr.IndexOf(IPhoneNumberUtils::WILD) >= 0
    ) {
        // Phone number is invalid
        ((CGsmConnection*)mPendingMO.Get())->mCause = IDisconnectCause::INVALID_NUMBER;

        // handlePollCalls() will notice this call not present
        // and will mark it as dropped.
        PollCallsWhenSafe();
    }
    else {
        // Always unmute when initiating a new call
        SetMute(FALSE);

// TODO: Need ICommandsInterface
        // mCi->Dial(addr, clirMode, uusInfo, ObtainCompleteMessage());
    }

    if (mNumberConverted) {
        ((CGsmConnection*)mPendingMO.Get())->SetConverted(origNumber);
        mNumberConverted = FALSE;
    }

    UpdatePhoneState();
    ((CGSMPhone*)mPhone.Get())->NotifyPreciseCallStateChanged();

    *result = IConnection::Probe(mPendingMO);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CGsmCallTracker::Dial(
    /* [in] */ const String& dialString,
    /* [out] */ IConnection** result)
{
    VALIDATE_NOT_NULL(result)
    return Dial(dialString, ICommandsInterface::CLIR_DEFAULT, NULL, result);
}

ECode CGsmCallTracker::Dial(
    /* [in] */ const String& dialString,
    /* [in] */ IUUSInfo* uusInfo,
    /* [out] */ IConnection** result)
{
    VALIDATE_NOT_NULL(result)
    return Dial(dialString, ICommandsInterface::CLIR_DEFAULT, uusInfo, result);
}

ECode CGsmCallTracker::Dial(
    /* [in] */ const String& dialString,
    /* [in] */ Int32 clirMode,
    /* [out] */ IConnection** result)
{
    VALIDATE_NOT_NULL(result)
    return Dial(dialString, clirMode, NULL, result);
}

ECode CGsmCallTracker::AcceptCall()
{
    // FIXME if SWITCH fails, should retry with ANSWER
    // in case the active/holding call disappeared and this
    // is no longer call waiting
    ICallState state;
    ICall::Probe(mRingingCall)->GetState(&state);
    if (state == ICallState_INCOMING) {
        Logger::I("phone", "acceptCall: incoming...");
        // Always unmute when answering a new call
        SetMute(FALSE);
        mCi->AcceptCall(ObtainCompleteMessage());
    }
    else if (state == ICallState_WAITING) {
        SetMute(FALSE);
        SwitchWaitingOrHoldingAndActive();
    }
    else {
        // throw new CallStateException("phone not ringing");
        return E_CALL_STATE_EXCEPTION;
    }
    return NOERROR;
}

ECode CGsmCallTracker::RejectCall()
{
    // AT+CHLD=0 means "release held or UDUB"
    // so if the phone isn't ringing, this could hang up held
    ICallState state;
    ICall::Probe(mRingingCall)->GetState(&state);
// TODO: Need ICallState::IsRinging
    // if (state.IsRinging()) {
    //     mCi->RejectCall(ObtainCompleteMessage());
    // }
    // else {
    //     // throw new CallStateException("phone not ringing");
    //     return E_CALL_STATE_EXCEPTION;
    // }
    return NOERROR;
}

ECode CGsmCallTracker::SwitchWaitingOrHoldingAndActive()
{
    // Should we bother with this check?
    ICallState state;
    ICall::Probe(mRingingCall)->GetState(&state);
    if (state == ICallState_INCOMING) {
        // throw new CallStateException("cannot be in the incoming state");
        return E_CALL_STATE_EXCEPTION;
    }
    else {
        mCi->SwitchWaitingOrHoldingAndActive(
                ObtainCompleteMessage(EVENT_SWITCH_RESULT));
    }
    return NOERROR;
}

ECode CGsmCallTracker::Conference()
{
    return mCi->Conference(ObtainCompleteMessage(EVENT_CONFERENCE_RESULT));
}

ECode CGsmCallTracker::ExplicitCallTransfer()
{
    return mCi->ExplicitCallTransfer(ObtainCompleteMessage(EVENT_ECT_RESULT));
}

ECode CGsmCallTracker::ClearDisconnected()
{
    InternalClearDisconnected();

    UpdatePhoneState();
    ((CGSMPhone*)mPhone.Get())->NotifyPreciseCallStateChanged();
    return NOERROR;
}

ECode CGsmCallTracker::CanConference(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    ICallState fgState;
    ICall::Probe(mForegroundCall)->GetState(&fgState);
    ICallState bgState;
    ICall::Probe(mBackgroundCall)->GetState(&bgState);
    Boolean b1, b2;
    *result = fgState == ICallState_ACTIVE
            && bgState == ICallState_HOLDING
            && (mBackgroundCall->IsFull(&b1), !b1)
            && (mForegroundCall->IsFull(&b2), !b2);
    return NOERROR;
}

ECode CGsmCallTracker::CanDial(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Boolean ret;
    AutoPtr<IServiceState> ss;
    ((CGSMPhone*)mPhone.Get())->GetServiceState((IServiceState**)&ss);
    Int32 serviceState;
    ss->GetState(&serviceState);
    String disableCall;
    SystemProperties::Get(
            ITelephonyProperties::PROPERTY_DISABLE_CALL, String("false"), &disableCall);

    ICallState fgState;
    ICall::Probe(mForegroundCall)->GetState(&fgState);
    ICallState bgState;
    ICall::Probe(mBackgroundCall)->GetState(&bgState);
    Boolean b1, b2, b3;
    ret = (serviceState != IServiceState::STATE_POWER_OFF)
            && mPendingMO == NULL
            && (ICall::Probe(mRingingCall)->IsRinging(&b1), !b1)
            && !disableCall.Equals("true");
// TODO: Need ICallState::IsAlive
            // && ((fgState->IsAlive(&b2), b2)
            //     || (bgState->IsAlive(&b3), b3));

    *result = ret;
    return NOERROR;
}

ECode CGsmCallTracker::CanTransfer(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    ICallState fgState;
    ICall::Probe(mForegroundCall)->GetState(&fgState);
    ICallState bgState;
    ICall::Probe(mBackgroundCall)->GetState(&bgState);
    *result = (fgState == ICallState_ACTIVE
            || fgState == ICallState_ALERTING
            || fgState == ICallState_DIALING)
        && bgState == ICallState_HOLDING;
    return NOERROR;
}

ECode CGsmCallTracker::Hangup(
    /* [in] */ IGsmConnection* conn)
{
    if (((CGsmConnection*)conn)->mOwner != this) {
        // throw new CallStateException ("GsmConnection " + conn
        //                         + "does not belong to GsmCallTracker " + this);
        return E_CALL_STATE_EXCEPTION;
    }

    if (conn == mPendingMO) {
        // We're hanging up an outgoing call that doesn't have it's
        // GSM index assigned yet

        if (IPhone::DEBUG_PHONE) Log(String("hangup: set hangupPendingMO to true"));
        mHangupPendingMO = TRUE;
    }
    else {
        // try {
        Int32 index;
        ((CGsmConnection*)conn)->GetGSMIndex(&index);
        mCi->HangupConnection(index, ObtainCompleteMessage());
        // } catch (CallStateException ex) {
        //     // Ignore "connection not found"
        //     // Call may have hung up already
        //     Logger::W(TAG,"GsmCallTracker WARN: hangup() on absent connection "
        //                     + conn);
        // }
    }

    ((CGsmConnection*)conn)->OnHangupLocal();
    return NOERROR;
}

ECode CGsmCallTracker::Separate(
    /* [in] */ IGsmConnection* conn)
{
    if (((CGsmConnection*)conn)->mOwner != this) {
        // throw new CallStateException ("GsmConnection " + conn
        //                         + "does not belong to GsmCallTracker " + this);
        return E_CALL_STATE_EXCEPTION;
    }
    // try {
    Int32 index;
    ((CGsmConnection*)conn)->GetGSMIndex(&index);
    mCi->SeparateConnection (index,
        ObtainCompleteMessage(EVENT_SEPARATE_RESULT));
    // } catch (CallStateException ex) {
    //     // Ignore "connection not found"
    //     // Call may have hung up already
    //     Logger::W(TAG,"GsmCallTracker WARN: separate() on absent connection "
    //                   + conn);
    // }
    return NOERROR;
}

ECode CGsmCallTracker::SetMute(
    /* [in] */ Boolean mute)
{
    mDesiredMute = mute;
    mCi->SetMute(mDesiredMute, NULL);
    return NOERROR;
}

ECode CGsmCallTracker::GetMute(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mDesiredMute;
    return NOERROR;
}

ECode CGsmCallTracker::Hangup(
    /* [in] */ IGsmCall* call)
{
    AutoPtr<IList> conns;
    ((CGsmCall*)call)->GetConnections((IList**)&conns);
    Int32 size;
    conns->GetSize(&size);
    if (size == 0) {
        // throw new CallStateException("no connections in call");
        return E_CALL_STATE_EXCEPTION;
    }

    Boolean b;
    if (call == mRingingCall) {
        if (IPhone::DEBUG_PHONE) Log(String("(ringing) hangup waiting or background"));
        mCi->HangupWaitingOrBackground(ObtainCompleteMessage());
    }
    else if (call == mForegroundCall) {
        if (ICall::Probe(call)->IsDialingOrAlerting(&b), b) {
            if (IPhone::DEBUG_PHONE) {
                Log(String("(foregnd) hangup dialing or alerting..."));
            }
            AutoPtr<IInterface> obj;
            conns->Get(0, (IInterface**)&obj);
            Hangup(IGsmConnection::Probe(obj));
        }
        else if (ICall::Probe(mRingingCall)->IsRinging(&b), b) {
            // Do not auto-answer ringing on CHUP, instead just end active calls
            Log(String("hangup all conns in active/background call, without affecting ringing call"));
            HangupAllConnections(call);
        }
        else {
            HangupForegroundResumeBackground();
        }
    }
    else if (call == mBackgroundCall) {
        if (ICall::Probe(mRingingCall)->IsRinging(&b), b) {
            if (IPhone::DEBUG_PHONE) {
                Log(String("hangup all conns in background call"));
            }
            HangupAllConnections(call);
        }
        else {
            HangupWaitingOrBackground();
        }
    }
    else {
        // throw new RuntimeException ("GsmCall " + call +
        //         "does not belong to GsmCallTracker " + this);
        return E_RUNTIME_EXCEPTION;
    }

    call->OnHangupLocal();
    ((CGSMPhone*)mPhone.Get())->NotifyPreciseCallStateChanged();
    return NOERROR;
}

ECode CGsmCallTracker::HangupWaitingOrBackground()
{
    if (IPhone::DEBUG_PHONE) Log(String("hangupWaitingOrBackground"));
    return mCi->HangupWaitingOrBackground(ObtainCompleteMessage());
}

ECode CGsmCallTracker::HangupForegroundResumeBackground()
{
    if (IPhone::DEBUG_PHONE) Log(String("hangupForegroundResumeBackground"));
    return mCi->HangupForegroundResumeBackground(ObtainCompleteMessage());
}

ECode CGsmCallTracker::HangupConnectionByIndex(
    /* [in] */ IGsmCall* call,
    /* [in] */ Int32 index)
{
    Int32 count;
    ((CGsmCall*)call)->mConnections->GetSize(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> obj;
        ((CGsmCall*)call)->mConnections->Get(i, (IInterface**)&obj);
        AutoPtr<IGsmConnection> cn = IGsmConnection::Probe(obj);
        Int32 idx;
        ((CGsmConnection*)cn.Get())->GetGSMIndex(&idx);
        if (idx == index) {
            mCi->HangupConnection(index, ObtainCompleteMessage());
            return NOERROR;
        }
    }

    // throw new CallStateException("no gsm index found");
    return E_CALL_STATE_EXCEPTION;
}

ECode CGsmCallTracker::HangupAllConnections(
    /* [in] */ IGsmCall* call)
{
    // try {
    Int32 count;
    ((CGsmCall*)call)->mConnections->GetSize(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> obj;
        ((CGsmCall*)call)->mConnections->Get(i, (IInterface**)&obj);
        AutoPtr<IGsmConnection> cn = IGsmConnection::Probe(obj);
        Int32 idx;
        ((CGsmConnection*)cn.Get())->GetGSMIndex(&idx);
        mCi->HangupConnection(idx, ObtainCompleteMessage());
    }
    // } catch (CallStateException ex) {
    //     Logger::E(TAG, "hangupConnectionByIndex caught " + ex);
    // }
    return NOERROR;
}

ECode CGsmCallTracker::GetConnectionByIndex(
    /* [in] */ IGsmCall* call,
    /* [in] */ Int32 index,
    /* [out] */ IGsmConnection** result)
{
    VALIDATE_NOT_NULL(result)
    Int32 count;
    ((CGsmCall*)call)->mConnections->GetSize(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> obj;
        ((CGsmCall*)call)->mConnections->Get(i, (IInterface**)&obj);
        AutoPtr<IGsmConnection> cn = IGsmConnection::Probe(obj);
        Int32 idx;
        ((CGsmConnection*)cn.Get())->GetGSMIndex(&idx);
        if (idx == index) {
            *result = cn;
            REFCOUNT_ADD(*result)
            return NOERROR;
        }
    }

    *result = NULL;
    return NOERROR;
}

ECode CGsmCallTracker::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AutoPtr<AsyncResult> ar;

    if (!((CGSMPhone*)mPhone.Get())->mIsTheCurrentActivePhone) {
        // Logger::E(TAG, "Received message " + msg +
        //         "[" + msg.what + "] while being destroyed. Ignoring.");
        return NOERROR;
    }
    Int32 what;
    msg->GetWhat(&what);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    switch (what) {
        case EVENT_POLL_CALLS_RESULT:
            ar = (AsyncResult*)(IObject::Probe(obj));

            if (msg == mLastRelevantPoll) {
                if (DBG_POLL) Log(String("handle EVENT_POLL_CALL_RESULT: set needsPoll=F"));
                mNeedsPoll = FALSE;
                mLastRelevantPoll = NULL;
                HandlePollCalls((AsyncResult*)(IObject::Probe(obj)));
            }
        break;

        case EVENT_OPERATION_COMPLETE:
            ar = (AsyncResult*)(IObject::Probe(obj));
            OperationComplete();
        break;

        case EVENT_SWITCH_RESULT:
        case EVENT_CONFERENCE_RESULT:
        case EVENT_SEPARATE_RESULT:
        case EVENT_ECT_RESULT:
            ar = (AsyncResult*)(IObject::Probe(obj));
            if (ar->mException != NULL) {
                ((CGSMPhone*)mPhone.Get())->NotifySuppServiceFailed(GetFailedService(what));
            }
            OperationComplete();
        break;

        case EVENT_GET_LAST_CALL_FAIL_CAUSE: {
            Int32 causeCode;
            ar = (AsyncResult*)(IObject::Probe(obj));

            OperationComplete();

            if (ar->mException != NULL) {
                // An exception occurred...just treat the disconnect
                // cause as "normal"
                causeCode = ICallFailCause::NORMAL_CLEARING;
                Logger::I(TAG,
                        "Exception during getLastCallFailCause, assuming normal disconnect");
            }
            else {
                AutoPtr<IArrayList> array = IArrayList::Probe(ar->mResult);
                AutoPtr<IInterface> obj;
                array->Get(0, (IInterface**)&obj);
                IInteger32::Probe(obj)->GetValue(&causeCode);
            }
            // Log the causeCode if its not normal
            if (causeCode == ICallFailCause::NO_CIRCUIT_AVAIL ||
                causeCode == ICallFailCause::TEMPORARY_FAILURE ||
                causeCode == ICallFailCause::SWITCHING_CONGESTION ||
                causeCode == ICallFailCause::CHANNEL_NOT_AVAIL ||
                causeCode == ICallFailCause::QOS_NOT_AVAIL ||
                causeCode == ICallFailCause::BEARER_NOT_AVAIL ||
                causeCode == ICallFailCause::ERROR_UNSPECIFIED) {
                AutoPtr<ICellLocation> cl;
                ((CGSMPhone*)mPhone.Get())->GetCellLocation((ICellLocation**)&cl);
                AutoPtr<IGsmCellLocation> loc = IGsmCellLocation::Probe(cl);
                // EventLog::WriteEvent(EventLogTags.CALL_DROP,
                //         causeCode, loc != NULL ? loc.getCid() : -1,
                //         TelephonyManager.getDefault().getNetworkType());
            }

            Int32 size;
            mDroppedDuringPoll->GetSize(&size);
            for (Int32 i = 0, s =  size; i < s ; i++) {
                AutoPtr<IInterface> obj;
                mDroppedDuringPoll->Get(i, (IInterface**)&obj);
                AutoPtr<IGsmConnection> conn = IGsmConnection::Probe(obj);

                ((CGsmConnection*)conn.Get())->OnRemoteDisconnect(causeCode);
            }

            UpdatePhoneState();

            ((CGSMPhone*)mPhone.Get())->NotifyPreciseCallStateChanged();
            mDroppedDuringPoll->Clear();
        }
        break;

        case EVENT_REPOLL_AFTER_DELAY:
        case EVENT_CALL_STATE_CHANGE:
            PollCallsWhenSafe();
        break;

        case EVENT_RADIO_AVAILABLE:
            HandleRadioAvailable();
        break;

        case EVENT_RADIO_NOT_AVAILABLE:
            HandleRadioNotAvailable();
        break;
    }
    return NOERROR;
}

ECode CGsmCallTracker::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    pw->Println(String("GsmCallTracker extends:"));
    CallTracker::Dump(fd, pw, args);
    // pw->Println("mConnections: length=" + mConnections->GetLength());
    // for(int i=0; i < mConnections->GetLength(); i++) {
    //     pw->Printf("  mConnections[%d]=%s\n", i, (*mConnections)[i]);
    // }
    // pw->Println(" mVoiceCallEndedRegistrants=" + mVoiceCallEndedRegistrants);
    // pw->Println(" mVoiceCallStartedRegistrants=" + mVoiceCallStartedRegistrants);
    // pw->Println(" mDroppedDuringPoll: size=" + mDroppedDuringPoll.size());
    // for(int i = 0; i < mDroppedDuringPoll.size(); i++) {
    //     pw->Printf( "  mDroppedDuringPoll[%d]=%s\n", i, mDroppedDuringPoll.get(i));
    // }
    // pw->Println(" mRingingCall=" + mRingingCall);
    // pw->Println(" mForegroundCall=" + mForegroundCall);
    // pw->Println(" mBackgroundCall=" + mBackgroundCall);
    // pw->Println(" mPendingMO=" + mPendingMO);
    // pw->Println(" mHangupPendingMO=" + mHangupPendingMO);
    // pw->Println(" mPhone=" + mPhone);
    // pw->Println(" mDesiredMute=" + mDesiredMute);
    // pw->Println(" mState=" + mState);
    return NOERROR;
}

void CGsmCallTracker::HandlePollCalls(
    /* [in] */ AsyncResult* ar)
{
    AutoPtr<IArrayList> polledCalls;

    if (ar->mException == NULL) {
        polledCalls = IArrayList::Probe(ar->mResult);
    }
    else if (IsCommandExceptionRadioNotAvailable(ar->mException)) {
        // just a dummy empty ArrayList to cause the loop
        // to hang up all the calls
        CArrayList::New((IArrayList**)&polledCalls);
    }
    else {
        // Radio probably wasn't ready--try again in a bit
        // But don't keep polling if the channel is closed
        PollCallsAfterDelay();
        return;
    }

    AutoPtr<IConnection> newRinging; //or waiting
    AutoPtr<IConnection> newUnknown;
    Boolean hasNonHangupStateChanged = FALSE;   // Any change besides
                                                // a dropped connection
    Boolean hasAnyCallDisconnected = FALSE;
    Boolean needsPollDelay = FALSE;
    Boolean unknownConnectionAppeared = FALSE;

    Int32 size;
    polledCalls->GetSize(&size);
    for (Int32 i = 0, curDC = 0, dcSize = size
            ; i < mConnections->GetLength(); i++) {
        AutoPtr<IGsmConnection> conn = (*mConnections)[i];
        AutoPtr<IDriverCall> dc;

        // polledCall list is sparse
        if (curDC < dcSize) {
            AutoPtr<IInterface> obj;
            polledCalls->Get(curDC, (IInterface**)&obj);
            dc = IDriverCall::Probe(obj);

            if (((DriverCall*)dc.Get())->mIndex == i + 1) {
                curDC++;
            }
            else {
                dc = NULL;
            }
        }

        // if (DBG_POLL) Log(String("poll: conn[i=") + i + "]=" +
        //         conn+", dc=" + dc);

        Boolean b;
        if (conn == NULL && dc != NULL) {
            // Connection appeared in CLCC response that we don't know about
            ((CGsmConnection*)mPendingMO.Get())->CompareTo(dc, &b);
            if (mPendingMO != NULL && b) {

                // if (DBG_POLL) Log(String("poll: pendingMO=") + mPendingMO);

                // It's our pending mobile originating call
                mConnections->Set(i, mPendingMO);
                ((CGsmConnection*)mPendingMO.Get())->mIndex = i;
                ((CGsmConnection*)mPendingMO.Get())->Update(dc, &b);
                mPendingMO = NULL;

                // Someone has already asked to hangup this call
                if (mHangupPendingMO) {
                    mHangupPendingMO = FALSE;
                    // try {
                    if (IPhone::DEBUG_PHONE) Log(String("poll: hangupPendingMO, hangup conn ") + i);
                    Hangup((*mConnections)[i]);
                    // } catch (CallStateException ex) {
                    //     Logger::E(TAG, "unexpected error on hangup");
                    // }

                    // Do not continue processing this poll
                    // Wait for hangup and repoll
                    return;
                }
            }
            else {
                AutoPtr<IContext> ctx;
                ((CGSMPhone*)mPhone.Get())->GetContext((IContext**)&ctx);
                AutoPtr<IGsmConnection> gc;
                CGsmConnection::New(ctx, dc, this, i, (IGsmConnection**)&gc);
                mConnections->Set(i, gc);

                AutoPtr<IConnection> hoConnection = GetHoConnection((DriverCall*)dc.Get());
                AutoPtr<ICall> call;
                ((CGsmConnection*)(*mConnections)[i])->GetCall((ICall**)&call);
                AutoPtr<IGsmCall> gsmCall = IGsmCall::Probe(call);
                if (hoConnection != NULL) {
                    // Single Radio Voice Call Continuity (SRVCC) completed
                    ((CGsmConnection*)(*mConnections)[i])->MigrateFrom(hoConnection);
                    Boolean b;
                    if (hoConnection->IsMultiparty(&b), b) {
                        // Remove only if it is not multiparty
                        mHandoverConnections->Remove(hoConnection);
                    }
                    ((CGSMPhone*)mPhone.Get())->NotifyHandoverStateChanged(IConnection::Probe((*mConnections)[i]));
                }
                else if ( gsmCall == mRingingCall ) { // it's a ringing call
                    newRinging = IConnection::Probe((*mConnections)[i]);
                }
                else {
                    // Something strange happened: a call appeared
                    // which is neither a ringing call or one we created.
                    // Either we've crashed and re-attached to an existing
                    // call, or something else (eg, SIM) initiated the call.

                    String str;
                    IObject::Probe(dc)->ToString(&str);
                    Logger::I(TAG,"Phantom call appeared %s", str.string());

                    // If it's a connected call, set the connect time so that
                    // it's non-zero.  It may not be accurate, but at least
                    // it won't appear as a Missed Call.
                    if (((DriverCall*)dc.Get())->mState != DriverCallState_ALERTING
                            && ((DriverCall*)dc.Get())->mState != DriverCallState_DIALING) {
                        ((CGsmConnection*)(*mConnections)[i])->OnConnectedInOrOut();
                        if (((DriverCall*)dc.Get())->mState == DriverCallState_HOLDING) {
                            // We've transitioned into HOLDING
                            ((CGsmConnection*)(*mConnections)[i])->OnStartedHolding();
                        }
                    }

                    newUnknown = IConnection::Probe((*mConnections)[i]);

                    unknownConnectionAppeared = TRUE;
                }
            }
            hasNonHangupStateChanged = TRUE;
        }
        else if (conn != NULL && dc == NULL) {
            // Connection missing in CLCC response that we were
            // tracking.
            mDroppedDuringPoll->Add(conn);
            // Dropped connections are removed from the CallTracker
            // list but kept in the GsmCall list
            mConnections->Set(i, NULL);
        }
        else if (conn != NULL && dc != NULL && (((CGsmConnection*)conn.Get())->CompareTo(dc, &b), b)) {
            // Connection in CLCC response does not match what
            // we were tracking. Assume dropped call and new call

            mDroppedDuringPoll->Add(conn);
            AutoPtr<IContext> ctx;
            ((CGSMPhone*)mPhone.Get())->GetContext((IContext**)&ctx);
            AutoPtr<IGsmConnection> gc;
            CGsmConnection::New(ctx, dc, this, i, (IGsmConnection**)&gc);
            mConnections->Set(i, gc);

            AutoPtr<ICall> call;
            ((CGsmConnection*)(*mConnections)[i])->GetCall((ICall**)&call);
            AutoPtr<IGsmCall> gcall = IGsmCall::Probe(call);
            if (gcall == mRingingCall) {
                newRinging = IConnection::Probe((*mConnections)[i]);
            } // else something strange happened
            hasNonHangupStateChanged = TRUE;
        }
        else if (conn != NULL && dc != NULL) { /* implicit conn.compareTo(dc) */
            Boolean changed;
            ((CGsmConnection*)conn.Get())->Update(dc, &changed);
            hasNonHangupStateChanged = hasNonHangupStateChanged || changed;
        }

        if (REPEAT_POLLING) {
            if (dc != NULL) {
                // FIXME with RIL, we should not need this anymore
                if ((((DriverCall*)dc.Get())->mState == DriverCallState_DIALING
                        /*&& cm.getOption(cm.OPTION_POLL_DIALING)*/)
                    || (((DriverCall*)dc.Get())->mState == DriverCallState_ALERTING
                        /*&& cm.getOption(cm.OPTION_POLL_ALERTING)*/)
                    || (((DriverCall*)dc.Get())->mState == DriverCallState_INCOMING
                        /*&& cm.getOption(cm.OPTION_POLL_INCOMING)*/)
                    || (((DriverCall*)dc.Get())->mState == DriverCallState_WAITING
                        /*&& cm.getOption(cm.OPTION_POLL_WAITING)*/)
                ) {
                    // Sometimes there's no unsolicited notification
                    // for state transitions
                    needsPollDelay = TRUE;
                }
            }
        }
    }

    // This is the first poll after an ATD.
    // We expect the pending call to appear in the list
    // If it does not, we land here
    if (mPendingMO != NULL) {
        // Logger::D(TAG,"Pending MO dropped before poll fg state:"
        //                 + mForegroundCall.getState());

        mDroppedDuringPoll->Add(mPendingMO);
        mPendingMO = NULL;
        mHangupPendingMO = FALSE;
    }

    if (newRinging != NULL) {
        mPhone->NotifyNewRingingConnection(newRinging);
    }

    // clear the "local hangup" and "missed/rejected call"
    // cases from the "dropped during poll" list
    // These cases need no "last call fail" reason
    mDroppedDuringPoll->GetSize(&size);
    for (Int32 i = size - 1; i >= 0 ; i--) {
        AutoPtr<IInterface> obj;
        mDroppedDuringPoll->Get(i, (IInterface**)&obj);
        AutoPtr<CGsmConnection> conn = (CGsmConnection*)IGsmConnection::Probe(obj);

        Boolean b;
        conn->IsIncoming(&b);
        Int64 t;
        conn->GetConnectTime(&t);
        if (b && t == 0) {
            // Missed or rejected call
            Int32 cause;
            if (conn->mCause == IDisconnectCause::LOCAL) {
                cause = IDisconnectCause::INCOMING_REJECTED;
            }
            else {
                cause = IDisconnectCause::INCOMING_MISSED;
            }

            if (IPhone::DEBUG_PHONE) {
                Log(String("missed/rejected call, conn.cause=") + conn->mCause);
                Log(String("setting cause to ") + cause);
            }
            mDroppedDuringPoll->Remove(i);
            hasAnyCallDisconnected |= (conn->OnDisconnect(cause, &b), b);
        }
        else if (conn->mCause == IDisconnectCause::LOCAL
                || conn->mCause == IDisconnectCause::INVALID_NUMBER) {
            mDroppedDuringPoll->Remove(i);
            hasAnyCallDisconnected |= (conn->OnDisconnect(conn->mCause, &b), b);
        }
    }

    // Any non-local disconnects: determine cause
    mDroppedDuringPoll->GetSize(&size);
    if (size > 0) {
        mCi->GetLastCallFailCause(
            ObtainNoPollCompleteMessage(EVENT_GET_LAST_CALL_FAIL_CAUSE));
    }

    if (needsPollDelay) {
        PollCallsAfterDelay();
    }

    // Cases when we can no longer keep disconnected Connection's
    // with their previous calls
    // 1) the phone has started to ring
    // 2) A Call/Connection object has changed state...
    //    we may have switched or held or answered (but not hung up)
    if (newRinging != NULL || hasNonHangupStateChanged || hasAnyCallDisconnected) {
        InternalClearDisconnected();
    }

    UpdatePhoneState();

    if (unknownConnectionAppeared) {
        ((CGSMPhone*)mPhone.Get())->NotifyUnknownConnection(newUnknown);
    }

    if (hasNonHangupStateChanged || newRinging != NULL || hasAnyCallDisconnected) {
        ((CGSMPhone*)mPhone.Get())->NotifyPreciseCallStateChanged();
    }

    //dumpState();
}

void CGsmCallTracker::Log(
    /* [in] */ const String& msg)
{
    Logger::D(TAG, String("[GsmCallTracker] ") + msg);
}

void CGsmCallTracker::FakeHoldForegroundBeforeDial()
{

    // We need to make a copy here, since fakeHoldBeforeDial()
    // modifies the lists, and we don't want to reverse the order
    AutoPtr<IInterface> obj;
    ICloneable::Probe(((CGsmCall*)mForegroundCall.Get())->mConnections)->Clone((IInterface**)&obj);
    AutoPtr<IList> connCopy = IList::Probe(obj);

    Int32 size;
    connCopy->GetSize(&size);
    for (Int32 i = 0, s = size ; i < s ; i++) {
        AutoPtr<IInterface> obj;
        connCopy->Get(i, (IInterface**)&obj);
        AutoPtr<IGsmConnection> conn = IGsmConnection::Probe(obj);

        ((CGsmConnection*)conn.Get())->FakeHoldBeforeDial();
    }
}

void CGsmCallTracker::InternalClearDisconnected()
{
    mRingingCall->ClearDisconnected();
    mForegroundCall->ClearDisconnected();
    mBackgroundCall->ClearDisconnected();
}

AutoPtr<IMessage> CGsmCallTracker::ObtainCompleteMessage()
{
    return ObtainCompleteMessage(EVENT_OPERATION_COMPLETE);
}

AutoPtr<IMessage> CGsmCallTracker::ObtainCompleteMessage(
    /* [in] */ Int32 what)
{
    mPendingOperations++;
    mLastRelevantPoll = NULL;
    mNeedsPoll = TRUE;

    if (DBG_POLL) Log(String("ObtainCompleteMessage: pendingOperations=") +
            mPendingOperations + ", needsPoll=" + StringUtils::BooleanToString(mNeedsPoll));

    AutoPtr<IMessage> msg;
    ObtainMessage(what, (IMessage**)&msg);
    return msg;
}

void CGsmCallTracker::OperationComplete()
{
    mPendingOperations--;

    if (DBG_POLL) Log(String("operationComplete: pendingOperations=") +
            mPendingOperations + ", needsPoll=" + StringUtils::BooleanToString(mNeedsPoll));

    if (mPendingOperations == 0 && mNeedsPoll) {
        ObtainMessage(EVENT_POLL_CALLS_RESULT, (IMessage**)&mLastRelevantPoll);
        mCi->GetCurrentCalls(mLastRelevantPoll);
    }
    else if (mPendingOperations < 0) {
        // this should never happen
        Logger::E(TAG,"GsmCallTracker.pendingOperations < 0");
        mPendingOperations = 0;
    }
}

void CGsmCallTracker::UpdatePhoneState()
{
    PhoneConstantsState oldState = mState;

    Boolean b, b1, b2;
    if (ICall::Probe(mRingingCall)->IsRinging(&b), b) {
        mState = PhoneConstantsState_RINGING;
    }
    else if (mPendingMO != NULL ||
            !((ICall::Probe(mForegroundCall)->IsIdle(&b1), b1)
                && (ICall::Probe(mBackgroundCall)->IsIdle(&b2), b2))) {
        mState = PhoneConstantsState_OFFHOOK;
    }
    else {
        mState = PhoneConstantsState_IDLE;
    }

    if (mState == PhoneConstantsState_IDLE && oldState != mState) {
        ((RegistrantList*)mVoiceCallEndedRegistrants.Get())->NotifyRegistrants(
            new AsyncResult(NULL, NULL, NULL));
    }
    else if (oldState == PhoneConstantsState_IDLE && oldState != mState) {
        ((RegistrantList*)mVoiceCallStartedRegistrants.Get())->NotifyRegistrants(
                new AsyncResult(NULL, NULL, NULL));
    }

    if (mState != oldState) {
        ((CGSMPhone*)mPhone.Get())->NotifyPhoneStateChanged();
    }
}

void CGsmCallTracker::HandleRadioNotAvailable()
{
    // handlePollCalls will clear out its
    // call list when it gets the CommandException
    // error result from this
    PollCallsWhenSafe();
}

void CGsmCallTracker::DumpState()
{
    AutoPtr<IList> l;

    Logger::I(TAG,"Phone State:%d", mState);

    String str;
    IObject::Probe(mRingingCall)->ToString(&str);
    Logger::I(TAG,"Ringing call: %s", str.string());

    ICall::Probe(mRingingCall)->GetConnections((IList**)&l);
    Int32 size;
    l->GetSize(&size);
    for (Int32 i = 0, s = size; i < s; i++) {
        AutoPtr<IInterface> obj;
        l->Get(i, (IInterface**)&obj);
        IObject::Probe(obj)->ToString(&str);
        Logger::I(TAG, str);
    }

    IObject::Probe(mForegroundCall)->ToString(&str);
    Logger::I(TAG,"Foreground call: %s", str.string());

    ICall::Probe(mForegroundCall)->GetConnections((IList**)&l);
    l->GetSize(&size);
    for (Int32 i = 0, s = size; i < s; i++) {
        AutoPtr<IInterface> obj;
        l->Get(i, (IInterface**)&obj);
        IObject::Probe(obj)->ToString(&str);
        Logger::I(TAG, str);
    }

    IObject::Probe(mBackgroundCall)->ToString(&str);
    Logger::I(TAG,"Background call: %s", str.string());

    ICall::Probe(mBackgroundCall)->GetConnections((IList**)&l);
    l->GetSize(&size);
    for (Int32 i = 0, s = size; i < s; i++) {
        AutoPtr<IInterface> obj;
        l->Get(i, (IInterface**)&obj);
        IObject::Probe(obj)->ToString(&str);
        Logger::I(TAG, str);
    }
}

IPhoneSuppService CGsmCallTracker::GetFailedService(
    /* [in] */ Int32 what)
{
    switch (what) {
        case EVENT_SWITCH_RESULT:
            return IPhoneSuppService_SWITCH;
        case EVENT_CONFERENCE_RESULT:
            return IPhoneSuppService_CONFERENCE;
        case EVENT_SEPARATE_RESULT:
            return IPhoneSuppService_SEPARATE;
        case EVENT_ECT_RESULT:
            return IPhoneSuppService_TRANSFER;
    }
    return IPhoneSuppService_UNKNOWN;
}

} // namespace Gsm
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
