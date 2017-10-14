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

#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Os.h"

#include "elastos/droid/internal/telephony/cdma/CCdmaCall.h"
#include "elastos/droid/internal/telephony/cdma/CCdmaConnection.h"
#include "elastos/droid/internal/telephony/cdma/CdmaCallTracker.h"
#include "elastos/droid/internal/telephony/cdma/CDMAPhone.h"
#include "elastos/droid/os/CRegistrant.h"
#include "elastos/droid/os/SystemProperties.h"
#include "elastos/droid/R.h"
#include "elastos/droid/telephony/PhoneNumberUtils.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::CRegistrant;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IRegistrant;
using Elastos::Droid::Os::SystemProperties;
using Elastos::Droid::R;
using Elastos::Droid::Telephony::IDisconnectCause;
using Elastos::Droid::Telephony::IPhoneNumberUtils;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Droid::Telephony::PhoneNumberUtils;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::IArrayOf;
using Elastos::Core::IInteger32;
using Elastos::Core::StringUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {

ECode CdmaCallTracker::CdmaCallTrackerRunnable::Run()
{
    if (mHost->mPendingMO != NULL) {
        String addr;
        IConnection::Probe(mHost->mPendingMO)->GetAddress(&addr);
        AutoPtr<IMessage> msg;
        mHost->ObtainMessage(EVENT_THREE_WAY_DIAL_L2_RESULT_CDMA, (IMessage**)&msg);
        mHost->mCi->SendCDMAFeatureCode(addr, msg);
    }
    return NOERROR;
}

//=====================================================================
//                           CdmaCallTracker
//=====================================================================
CAR_INTERFACE_IMPL(CdmaCallTracker, CallTracker, ICdmaCallTracker);

const String CdmaCallTracker::LOGTAG("CdmaCallTracker");
const Int32 CdmaCallTracker::MAX_CONNECTIONS;
const Int32 CdmaCallTracker::MAX_CONNECTIONS_PER_CALL;
const Boolean CdmaCallTracker::REPEAT_POLLING = FALSE;
const Boolean CdmaCallTracker::DBG_POLL = FALSE;

CdmaCallTracker::CdmaCallTracker()
    : mHangupPendingMO(FALSE)
    , mPendingCallInEcm(FALSE)
    , mIsInEmergencyCall(FALSE)
    , mDesiredMute(FALSE)
    , mPendingCallClirMode(0)
    , mState(PhoneConstantsState_IDLE)
    , mIsEcmTimerCanceled(FALSE)
    , m3WayCallFlashDelay(0)
{
    mConnections = ArrayOf<ICdmaConnection*>::Alloc(MAX_CONNECTIONS);
    mVoiceCallEndedRegistrants = new RegistrantList();
    mVoiceCallStartedRegistrants = new RegistrantList();
    mCallWaitingRegistrants =  new RegistrantList();

    CArrayList::New(MAX_CONNECTIONS, (IArrayList**)&mDroppedDuringPoll);

    CCdmaCall::New(this, (ICdmaCall**)&mRingingCall);
    CCdmaCall::New(this, (ICdmaCall**)&mForegroundCall);
    CCdmaCall::New(this, (ICdmaCall**)&mBackgroundCall);
}

CdmaCallTracker::~CdmaCallTracker()
{
    Logger::D(LOGTAG, "CdmaCallTracker finalized");
}

ECode CdmaCallTracker::constructor(
    /* [in] */ ICDMAPhone* phone)
{
    mPhone = phone;
    mCi = ((CDMAPhone*)phone)->mCi;
    mCi->RegisterForCallStateChanged(this, EVENT_CALL_STATE_CHANGE, NULL);
    mCi->RegisterForOn(this, EVENT_RADIO_AVAILABLE, NULL);
    mCi->RegisterForNotAvailable(this, EVENT_RADIO_NOT_AVAILABLE, NULL);
    mCi->RegisterForCallWaitingInfo(this, EVENT_CALL_WAITING_INFO_CDMA, NULL);
    ICall::Probe(mForegroundCall)->SetGeneric(FALSE);
    return NOERROR;
}

ECode CdmaCallTracker::Dispose()
{
    Logger::D(LOGTAG, "CdmaCallTracker dispose");
    mCi->UnregisterForLineControlInfo(this);
    mCi->UnregisterForCallStateChanged(this);
    mCi->UnregisterForOn(this);
    mCi->UnregisterForNotAvailable(this);
    mCi->UnregisterForCallWaitingInfo(this);

    ClearDisconnected();
    return NOERROR;
}

ECode CdmaCallTracker::RegisterForVoiceCallStarted(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IRegistrant> r;
    CRegistrant::New(h, what, obj, (IRegistrant**)&r);
    mVoiceCallStartedRegistrants->Add(r);
    // Notify if in call when registering
    if (mState != PhoneConstantsState_IDLE) {
        AutoPtr<AsyncResult> ar = new AsyncResult(NULL, NULL, NULL);
        r->NotifyRegistrant(ar);
    }
    return NOERROR;
}

ECode CdmaCallTracker::UnregisterForVoiceCallStarted(
    /* [in] */ IHandler* h)
{
    mVoiceCallStartedRegistrants->Remove(h);
    return NOERROR;
}

ECode CdmaCallTracker::RegisterForVoiceCallEnded(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    VALIDATE_NOT_NULL(h);
    VALIDATE_NOT_NULL(obj);
    AutoPtr<IRegistrant> r;
    CRegistrant::New(h, what, obj, (IRegistrant**)&r);
    mVoiceCallEndedRegistrants->Add(r);
    return NOERROR;
}

ECode CdmaCallTracker::UnregisterForVoiceCallEnded(
    /* [in] */ IHandler* h)
{
    VALIDATE_NOT_NULL(h);
    mVoiceCallEndedRegistrants->Remove(h);
    return NOERROR;
}

ECode CdmaCallTracker::RegisterForCallWaiting(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IRegistrant> r;
    CRegistrant::New(h, what, obj, (IRegistrant**)&r);
    mCallWaitingRegistrants->Add(r);
    return NOERROR;
}

ECode CdmaCallTracker::UnregisterForCallWaiting(
    /* [in] */ IHandler* h)
{
    VALIDATE_NOT_NULL(h);
    mCallWaitingRegistrants->Remove(h);
    return NOERROR;
}

ECode CdmaCallTracker::Dial(
    /* [in] */ const String& _dialString,
    /* [in] */ Int32 clirMode,
    /* [out] */ IConnection** result)
{
    VALIDATE_NOT_NULL(result);
    String dialString = _dialString;
    // note that this triggers call state changed notif
    ClearDisconnected();

    Boolean b;
    if (CanDial(&b), !b) {
        // throw new CallStateException("cannot dial in current state");
        return E_CALL_STATE_EXCEPTION;
    }

    String origNumber = dialString;
    String operatorIsoContry;
    IPhoneBase::Probe(mPhone)->GetSystemProperty(
            ITelephonyProperties::PROPERTY_OPERATOR_ISO_COUNTRY, String(""),
            &operatorIsoContry);
    String simIsoContry;
    IPhoneBase::Probe(mPhone)->GetSystemProperty(
            ITelephonyProperties::PROPERTY_ICC_OPERATOR_ISO_COUNTRY, String(""),
            &simIsoContry);
    Boolean internationalRoaming = !TextUtils::IsEmpty(operatorIsoContry)
            && !TextUtils::IsEmpty(simIsoContry)
            && !simIsoContry.Equals(operatorIsoContry);
    if (internationalRoaming) {
        if (simIsoContry.Equals("us")) {
            internationalRoaming = internationalRoaming && !operatorIsoContry.Equals("vi");
        }
        else if (simIsoContry.Equals("vi")) {
            internationalRoaming = internationalRoaming && !operatorIsoContry.Equals("us");
        }
    }
    if (internationalRoaming) {
        dialString = ConvertNumberIfNecessary(IPhoneBase::Probe(mPhone), dialString);
    }

    String inEcm;
    SystemProperties::Get(ITelephonyProperties::PROPERTY_INECM_MODE, String("FALSE"), &inEcm);
    Boolean isPhoneInEcmMode = inEcm.Equals("TRUE");
    AutoPtr<IContext> ctx;
    IPhone::Probe(mPhone)->GetContext((IContext**)&ctx);
    Boolean isEmergencyCall;
    PhoneNumberUtils::IsLocalEmergencyNumber(ctx, dialString, &isEmergencyCall);

    // Cancel Ecm timer if a second emergency call is originating in Ecm mode
    if (isPhoneInEcmMode && isEmergencyCall) {
        HandleEcmTimer(CDMAPhone::CANCEL_ECM_TIMER);
    }

    // We are initiating a call therefore even if we previously
    // didn't know the state (i.e. Generic was TRUE) we now know
    // and therefore can set Generic to FALSE.
    ICall::Probe(mForegroundCall)->SetGeneric(FALSE);

    // The new call must be assigned to the foreground call.
    // That call must be idle, so place anything that's
    // there on hold
    ICallState foregroundCallState;
    ICall::Probe(mForegroundCall)->GetState(&foregroundCallState);
    if (foregroundCallState == ICallState_ACTIVE) {
        AutoPtr<IConnection> conn = DialThreeWay(dialString);
        *result = conn;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }

    CCdmaConnection::New(ctx, CheckForTestEmergencyNumber(dialString),
            this, mForegroundCall, (ICdmaConnection**)&mPendingMO);
    mHangupPendingMO = FALSE;

    String addr;
    IConnection::Probe(mPendingMO)->GetAddress(&addr);
    if ( addr.IsNull() || addr.GetLength() == 0
            || addr.IndexOf(IPhoneNumberUtils::WILD) >= 0 ) {
        // Phone number is invalid
        ((CdmaConnection*)mPendingMO.Get())->mCause = IDisconnectCause::INVALID_NUMBER;

        // handlePollCalls() will notice this call not present
        // and will mark it as dropped.
        PollCallsWhenSafe();
    }
    else {
        // Always unmute when initiating a new call
        SetMute(FALSE);

        // Check data call
        DisableDataCallInEmergencyCall(dialString);

        // In Ecm mode, if another emergency call is dialed, Ecm mode will not exit.
        if(!isPhoneInEcmMode || (isPhoneInEcmMode && isEmergencyCall)) {
            mCi->Dial(addr, clirMode, ObtainCompleteMessage());
        }
        else {
            IPhone::Probe(mPhone)->ExitEmergencyCallbackMode();
            IPhone::Probe(mPhone)->SetOnEcbModeExitResponse(this, EVENT_EXIT_ECM_RESPONSE_CDMA, NULL);
            mPendingCallClirMode = clirMode;
            mPendingCallInEcm = TRUE;
        }
    }

    if (mNumberConverted) {
        IConnection::Probe(mPendingMO)->SetConverted(origNumber);
        mNumberConverted = FALSE;
    }

    UpdatePhoneState();
    ((CDMAPhone*)mPhone.Get())->NotifyPreciseCallStateChanged();

    *result = IConnection::Probe(mPendingMO);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CdmaCallTracker::Dial(
    /* [in] */ const String& dialString,
    /* [out] */ IConnection** result)
{
    VALIDATE_NOT_NULL(result);
    return Dial(dialString, ICommandsInterface::CLIR_DEFAULT, result);
}

ECode CdmaCallTracker::AcceptCall()
{
    ICallState ringingCallState;
    ICall::Probe(mRingingCall)->GetState(&ringingCallState);
    if (ringingCallState == ICallState_INCOMING) {
        Logger::I("phone", "acceptCall: incoming...");
        // Always unmute when answering a new call
        SetMute(FALSE);
        mCi->AcceptCall(ObtainCompleteMessage());
    }
    else if (ringingCallState == ICallState_WAITING) {
        AutoPtr<IConnection> conn;
        ICall::Probe(mRingingCall)->GetLatestConnection((IConnection**)&conn);
        AutoPtr<ICdmaConnection> cwConn = ICdmaConnection::Probe(conn);

        // Since there is no network response for supplimentary
        // service for CDMA, we assume call waiting is answered.
        // ringing Call state change to idle is in CdmaCall.detach
        // triggered by updateParent.
        cwConn->UpdateParent(mRingingCall, mForegroundCall);
        ((CdmaConnection*)cwConn.Get())->OnConnectedInOrOut();
        UpdatePhoneState();
        SwitchWaitingOrHoldingAndActive();
    }
    else {
        // throw new CallStateException("phone not ringing");
        return E_CALL_STATE_EXCEPTION;
    }
    return NOERROR;
}

ECode CdmaCallTracker::RejectCall()
{
    // AT+CHLD=0 means "release held or UDUB"
    // so if the phone isn't ringing, this could hang up held
    ICallState ringingCallState;
    ICall::Probe(mRingingCall)->GetState(&ringingCallState);
    Boolean b = FALSE;
// TODO: Need ICallState.IsRinging
    // ringingCallState.IsRinging(&b);
    if (b) {
        mCi->RejectCall(ObtainCompleteMessage());
    }
    else {
        // throw new CallStateException("phone not ringing");
        return E_CALL_STATE_EXCEPTION;
    }
    return NOERROR;
}

ECode CdmaCallTracker::SwitchWaitingOrHoldingAndActive()
{
    // Should we bother with this check?
    ICallState ringingCallState;
    ICall::Probe(mRingingCall)->GetState(&ringingCallState);
    AutoPtr<IList> list;
    ICall::Probe(mForegroundCall)->GetConnections((IList**)&list);
    Int32 size;
    list->GetSize(&size);
    if (ringingCallState == ICallState_INCOMING) {
        // throw new CallStateException("cannot be in the incoming state");
        return E_CALL_STATE_EXCEPTION;
    }
    else if (size > 1) {
        FlashAndSetGenericTrue();
    }
    else {
        // Send a flash command to CDMA network for putting the other party on hold.
        // For CDMA networks which do not support this the user would just hear a beep
        // from the network. For CDMA networks which do support it will put the other
        // party on hold.
        AutoPtr<IMessage> msg;
        ObtainMessage(EVENT_SWITCH_RESULT, (IMessage**)&msg);
        mCi->SendCDMAFeatureCode(String(""), msg);
    }
    return NOERROR;
}

ECode CdmaCallTracker::Conference()
{
    // Should we be checking state?
    FlashAndSetGenericTrue();
    return NOERROR;
}

ECode CdmaCallTracker::ExplicitCallTransfer()
{
    return mCi->ExplicitCallTransfer(ObtainCompleteMessage(EVENT_ECT_RESULT));
}

ECode CdmaCallTracker::ClearDisconnected()
{
    InternalClearDisconnected();

    UpdatePhoneState();
    ((CDMAPhone*)mPhone.Get())->NotifyPreciseCallStateChanged();
    return NOERROR;
}

ECode CdmaCallTracker::CanConference(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    ICallState foregroundCallState;
    ICall::Probe(mForegroundCall)->GetState(&foregroundCallState);
    ICallState backgroundCallState;
    ICall::Probe(mBackgroundCall)->GetState(&backgroundCallState);
    Boolean b1, b2;
    *result = foregroundCallState == ICallState_ACTIVE
            && backgroundCallState == ICallState_HOLDING
            && (((CdmaCall*)mBackgroundCall.Get())->IsFull(&b1), !b1)
            && (((CdmaCall*)mForegroundCall.Get())->IsFull(&b2), !b2);
    return NOERROR;
}

ECode CdmaCallTracker::CanDial(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean ret;
    AutoPtr<IServiceState> ss;
    IPhone::Probe(mPhone)->GetServiceState((IServiceState**)&ss);
    Int32 serviceState;
    ss->GetState(&serviceState);
    String disableCall;
    SystemProperties::Get(
            ITelephonyProperties::PROPERTY_DISABLE_CALL, String("FALSE"),
            &disableCall);

    ICallState foregroundCallState;
    ICall::Probe(mForegroundCall)->GetState(&foregroundCallState);

    ICallState backgroundCallState;
    ICall::Probe(mBackgroundCall)->GetState(&backgroundCallState);

    Boolean b1 = FALSE, b2 = FALSE, b3 = FALSE;
// TODO: Need ICallState::IsAlive
    // foregroundCallState->IsAlive(&b1);
    // backgroundCallState->IsAlive(&b2);
    ICall::Probe(mRingingCall)->IsRinging(&b3);

    ret = (serviceState != IServiceState::STATE_POWER_OFF)
            && mPendingMO == NULL
            && !b3
            && !disableCall.Equals("TRUE")
            && (!b1
                || (foregroundCallState == ICallState_ACTIVE)
                || !b2);

    if (!ret) {
        // Log(String.format("canDial is FALSE\n" +
        //                   "((serviceState=%d) != IServiceState::STATE_POWER_OFF)::=%s\n" +
        //                   "&& pendingMO == NULL::=%s\n" +
        //                   "&& !ringingCall.isRinging()::=%s\n" +
        //                   "&& !disableCall.equals(\"TRUE\")::=%s\n" +
        //                   "&& (!foregroundCall.getState().isAlive()::=%s\n" +
        //                   "   || foregroundCall.getState() == ICallState_ACTIVE::=%s\n" +
        //                   "   ||!backgroundCall.getState().isAlive())::=%s)",
        //         serviceState,
        //         serviceState != IServiceState::STATE_POWER_OFF,
        //         mPendingMO == NULL,
        //         !mRingingCall->IsRinging(),
        //         !disableCall.Equals("TRUE"),
        //         !foregroundCallState.isAlive(),
        //         foregroundCallState == ICallState_ACTIVE,
        //         !backgroundCallState.isAlive()));
    }
    *result = ret;
    return NOERROR;
}

ECode CdmaCallTracker::CanTransfer(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Logger::E(LOGTAG, "canTransfer: not possible in CDMA");
    *result = FALSE;
    return NOERROR;
}

ECode CdmaCallTracker::Hangup(
    /* [in] */ ICdmaConnection* conn)
{
    VALIDATE_NOT_NULL(conn);
    if (((CdmaConnection*)conn)->mOwner != this) {
        // throw new CallStateException ("CdmaConnection " + conn
        //                         + "does not belong to CdmaCallTracker " + this);
        return E_CALL_STATE_EXCEPTION;
    }

    AutoPtr<ICall> call;
    IConnection::Probe(conn)->GetCall((ICall**)&call);
    ICallState ringingCallState;
    ICall::Probe(mRingingCall)->GetState(&ringingCallState);

    Boolean b;
    if (conn == mPendingMO) {
        // We're hanging up an outgoing call that doesn't have it's
        // GSM index assigned yet

        if (IPhone::DEBUG_PHONE) Log(String("hangup: set hangupPendingMO to TRUE"));
        mHangupPendingMO = TRUE;
    }
    else if ((IObject::Probe(call)->Equals(mRingingCall, &b), b)
            && (ringingCallState == ICallState_WAITING)) {
        // Handle call waiting hang up case.
        //
        // The ringingCall state will change to IDLE in CdmaCall.detach
        // if the ringing call connection size is 0. We don't specifically
        // set the ringing call state to IDLE here to avoid a race condition
        // where a new call waiting could get a hang up from an old call
        // waiting ringingCall.
        //
        // PhoneApp does the call log itself since only PhoneApp knows
        // the hangup reason is user ignoring or timing out. So conn.onDisconnect()
        // is not called here. Instead, conn.onLocalDisconnect() is called.
        ((CdmaConnection*)conn)->OnLocalDisconnect();
        UpdatePhoneState();
        ((CDMAPhone*)mPhone.Get())->NotifyPreciseCallStateChanged();
        return NOERROR;
    }
    else {
        // try {
        Int32 index;
        ((CdmaConnection*)conn)->GetCDMAIndex(&index);
        mCi->HangupConnection (index, ObtainCompleteMessage());
        // } catch (CallStateException ex) {
        //     // Ignore "connection not found"
        //     // Call may have hung up already
        //     Logger::W(LOGTAG,"CdmaCallTracker WARN: hangup() on absent connection "
        //                     + conn);
        // }
    }

    ((CdmaConnection*)conn)->OnHangupLocal();
    return NOERROR;
}

ECode CdmaCallTracker::Separate(
    /* [in] */ ICdmaConnection* conn)
{
    VALIDATE_NOT_NULL(conn);
    if (((CdmaConnection*)conn)->mOwner != this) {
        // throw new CallStateException ("CdmaConnection " + conn
        //                         + "does not belong to CdmaCallTracker " + this);
        return E_CALL_STATE_EXCEPTION;
    }
    // try {
    Int32 index;
    ((CdmaConnection*)conn)->GetCDMAIndex(&index);
    mCi->SeparateConnection (index,
        ObtainCompleteMessage(EVENT_SEPARATE_RESULT));
    // } catch (CallStateException ex) {
    //     // Ignore "connection not found"
    //     // Call may have hung up already
    //     Logger::W(LOGTAG,"CdmaCallTracker WARN: separate() on absent connection "
    //                   + conn);
    // }
    return NOERROR;
}

ECode CdmaCallTracker::SetMute(
    /* [in] */ Boolean mute)
{
    mDesiredMute = mute;
    mCi->SetMute(mDesiredMute, NULL);
    return NOERROR;
}

ECode CdmaCallTracker::GetMute(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mDesiredMute;
    return NOERROR;
}

ECode CdmaCallTracker::Hangup(
    /* [in] */ ICdmaCall* call)
{
    AutoPtr<IList> list;
    ICall::Probe(call)->GetConnections((IList**)&list);
    Int32 size;
    list->GetSize(&size);
    if (size == 0) {
        // throw new CallStateException("no connections in call");
        return E_CALL_STATE_EXCEPTION;
    }

    if (call == mRingingCall) {
        if (IPhone::DEBUG_PHONE) Log(String("(ringing) hangup waiting or background"));
        mCi->HangupWaitingOrBackground(ObtainCompleteMessage());
    }
    else if (call == mForegroundCall) {
        Boolean b;
        if (ICall::Probe(call)->IsDialingOrAlerting(&b), b) {
            if (IPhone::DEBUG_PHONE) {
                Log(String("(foregnd) hangup dialing or alerting..."));
            }
            AutoPtr<IInterface> obj;
            list->Get(0, (IInterface**)&obj);
            Hangup(ICdmaConnection::Probe(obj));
        }
        else {
            HangupForegroundResumeBackground();
        }
    }
    else if (call == mBackgroundCall) {
        Boolean b;
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
        // throw new RuntimeException ("CdmaCall " + call +
        //         "does not belong to CdmaCallTracker " + this);
        return E_RUNTIME_EXCEPTION;
    }

    ((CdmaCall*)call)->OnHangupLocal();
    ((CDMAPhone*)mPhone.Get())->NotifyPreciseCallStateChanged();
    return NOERROR;
}

ECode CdmaCallTracker::HangupWaitingOrBackground()
{
    if (IPhone::DEBUG_PHONE) Log(String("hangupWaitingOrBackground"));
    return mCi->HangupWaitingOrBackground(ObtainCompleteMessage());
}

ECode CdmaCallTracker::HangupForegroundResumeBackground()
{
    if (IPhone::DEBUG_PHONE) Log(String("hangupForegroundResumeBackground"));
    return mCi->HangupForegroundResumeBackground(ObtainCompleteMessage());
}

ECode CdmaCallTracker::HangupConnectionByIndex(
    /* [in] */ ICdmaCall* call,
    /* [in] */ Int32 index)
{
    VALIDATE_NOT_NULL(call);
    AutoPtr<IList> list;
    ICall::Probe(call)->GetConnections((IList**)&list);
    Int32 count;
    list->GetSize(&count);

    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> obj;
        ((CdmaCall*)call)->mConnections->Get(i, (IInterface**)&obj);
        AutoPtr<ICdmaConnection> cn = ICdmaConnection::Probe(obj);
        Int32 cdmaIndex;
        ((CdmaConnection*)cn.Get())->GetCDMAIndex(&cdmaIndex);
        if (cdmaIndex == index) {
            mCi->HangupConnection(index, ObtainCompleteMessage());
            return NOERROR;
        }
    }

    // throw new CallStateException("no gsm index found");
    return E_CALL_STATE_EXCEPTION;
}

ECode CdmaCallTracker::HangupAllConnections(
    /* [in] */ ICdmaCall* call)
{
    VALIDATE_NOT_NULL(call);
    // try {
    Int32 count;
    ((CdmaCall*)call)->mConnections->GetSize(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> obj;
        ((CdmaCall*)call)->mConnections->Get(i, (IInterface**)&obj);
        AutoPtr<ICdmaConnection> cn = ICdmaConnection::Probe(obj);
        Int32 cdmaIndex;
        ((CdmaConnection*)cn.Get())->GetCDMAIndex(&cdmaIndex);
        mCi->HangupConnection(cdmaIndex, ObtainCompleteMessage());
    }
    // } catch (CallStateException ex) {
    //     Logger::E(LOGTAG, "hangupConnectionByIndex caught " + ex);
    // }
    return NOERROR;
}

ECode CdmaCallTracker::GetConnectionByIndex(
    /* [in] */ ICdmaCall* call,
    /* [in] */ Int32 index,
    /* [out] */ ICdmaConnection** result)
{
    VALIDATE_NOT_NULL(result);
    Int32 count;
    ((CdmaCall*)call)->mConnections->GetSize(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> obj;
        ((CdmaCall*)call)->mConnections->Get(i, (IInterface**)&obj);
        AutoPtr<ICdmaConnection> cn = ICdmaConnection::Probe(obj);
        Int32 cdmaIndex;
        ((CdmaConnection*)cn.Get())->GetCDMAIndex(&cdmaIndex);
        if (cdmaIndex == index) {
            *result = cn;
            REFCOUNT_ADD(*result)
            return NOERROR;
        }
    }

    *result = NULL;
    return NOERROR;
}

ECode CdmaCallTracker::HandleMessage(
    /* [in] */ IMessage* msg)
{
    VALIDATE_NOT_NULL(msg);
    AutoPtr<AsyncResult> ar;

    if (!((CDMAPhone*)mPhone.Get())->mIsTheCurrentActivePhone) {
        Logger::W(LOGTAG, "Ignoring events received on inactive CdmaPhone");
        return NOERROR;
    }

    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case EVENT_POLL_CALLS_RESULT:{
            Logger::D(LOGTAG, "Event EVENT_POLL_CALLS_RESULT Received");
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            ar = (AsyncResult*)IAsyncResult::Probe(obj);

            if(msg == mLastRelevantPoll) {
                if(DBG_POLL) Log(
                        String("handle EVENT_POLL_CALL_RESULT: set needsPoll=F"));
                mNeedsPoll = FALSE;
                mLastRelevantPoll = NULL;
                HandlePollCalls(ar);
            }
        }
        break;

        case EVENT_OPERATION_COMPLETE:
            OperationComplete();
        break;

        case EVENT_SWITCH_RESULT:
             // In GSM call operationComplete() here which gets the
             // current call list. But in CDMA there is no list so
             // there is nothing to do.
        break;

        case EVENT_GET_LAST_CALL_FAIL_CAUSE: {
            Int32 causeCode;
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            ar = (AsyncResult*)IAsyncResult::Probe(obj);

            OperationComplete();

            if (ar->mException != NULL) {
                // An exception occurred...just treat the disconnect
                // cause as "normal"
                causeCode = ICallFailCause::NORMAL_CLEARING;
                Logger::I(LOGTAG,
                        "Exception during getLastCallFailCause, assuming normal disconnect");
            }
            else {
                AutoPtr<IArrayOf> iArray = IArrayOf::Probe(ar->mResult);
                AutoPtr<IInterface> obj;
                iArray->Get(0, (IInterface**)&obj);
                IInteger32::Probe(obj)->GetValue(&causeCode);
            }

            Int32 size;
            mDroppedDuringPoll->GetSize(&size);
            for (Int32 i = 0, s = size; i < s ; i++)
            {
                AutoPtr<IInterface> obj;
                mDroppedDuringPoll->Get(i, (IInterface**)&obj);
                AutoPtr<ICdmaConnection> conn = ICdmaConnection::Probe(obj);

                ((CdmaConnection*)conn.Get())->OnRemoteDisconnect(causeCode);
            }

            UpdatePhoneState();

            ((CDMAPhone*)mPhone.Get())->NotifyPreciseCallStateChanged();
            mDroppedDuringPoll->Clear();
            break;
        }
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

        case EVENT_EXIT_ECM_RESPONSE_CDMA:
            // no matter the result, we still do the same here
            if ( mPendingCallInEcm ) {
                String addr;
                IConnection::Probe(mPendingMO)->GetAddress(&addr);
                mCi->Dial(addr, mPendingCallClirMode, ObtainCompleteMessage());
                mPendingCallInEcm = FALSE;
            }
            IPhone::Probe(mPhone)->UnsetOnEcbModeExitResponse(this);
        break;

        case EVENT_CALL_WAITING_INFO_CDMA: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            ar = (AsyncResult*)IAsyncResult::Probe(obj);
            if (ar->mException == NULL) {
                HandleCallWaitingInfo(ICdmaCallWaitingNotification::Probe(ar->mResult));
                Logger::D(LOGTAG, "Event EVENT_CALL_WAITING_INFO_CDMA Received");
            }
            break;
        }
        case EVENT_THREE_WAY_DIAL_L2_RESULT_CDMA: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            ar = (AsyncResult*)IAsyncResult::Probe(obj);
            if (ar->mException == NULL) {
                // Assume 3 way call is connected
                ((CdmaConnection*)mPendingMO.Get())->OnConnectedInOrOut();
                mPendingMO = NULL;
            }
            break;
        }
        case EVENT_THREE_WAY_DIAL_BLANK_FLASH: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            ar = (AsyncResult*)IAsyncResult::Probe(obj);
            if (ar->mException == NULL) {
                AutoPtr<CdmaCallTrackerRunnable> r = new CdmaCallTrackerRunnable(this);
                Boolean b;
                PostDelayed(r, m3WayCallFlashDelay, &b);
            }
            else {
                mPendingMO = NULL;
                Logger::W(LOGTAG, "exception happened on Blank Flash for 3-way call");
            }
            break;
        }
        default:{
            // throw new RuntimeException("unexpected event not handled");
            return E_RUNTIME_EXCEPTION;
        }
    }
    return NOERROR;
}

ECode CdmaCallTracker::IsInEmergencyCall(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mIsInEmergencyCall;
    return NOERROR;
}

ECode CdmaCallTracker::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    pw->Println(String("GsmCallTracker extends:"));
    CallTracker::Dump(fd, pw, args);
    // pw->Println("droppedDuringPoll: length=" + mConnections.length);
    // for(int i=0; i < mConnections.length; i++) {
    //     pw->Printf(" mConnections[%d]=%s\n", i, mConnections[i]);
    // }
    // pw->Println(" mVoiceCallEndedRegistrants=" + mVoiceCallEndedRegistrants);
    // pw->Println(" mVoiceCallStartedRegistrants=" + mVoiceCallStartedRegistrants);
    // pw->Println(" mCallWaitingRegistrants=" + mCallWaitingRegistrants);
    // pw->Println("droppedDuringPoll: size=" + mDroppedDuringPoll.size());
    // for(int i = 0; i < mDroppedDuringPoll.size(); i++) {
    //     pw->Printf( " mDroppedDuringPoll[%d]=%s\n", i, mDroppedDuringPoll.get(i));
    // }
    // pw->Println(" mRingingCall=" + mRingingCall);
    // pw->Println(" mForegroundCall=" + mForegroundCall);
    // pw->Println(" mBackgroundCall=" + mBackgroundCall);
    // pw->Println(" mPendingMO=" + mPendingMO);
    // pw->Println(" mHangupPendingMO=" + mHangupPendingMO);
    // pw->Println(" mPendingCallInEcm=" + mPendingCallInEcm);
    // pw->Println(" mIsInEmergencyCall=" + mIsInEmergencyCall);
    // pw->Println(" mPhone=" + mPhone);
    // pw->Println(" mDesiredMute=" + mDesiredMute);
    // pw->Println(" mPendingCallClirMode=" + mPendingCallClirMode);
    // pw->Println(" mState=" + mState);
    // pw->Println(" mIsEcmTimerCanceled=" + mIsEcmTimerCanceled);
    return NOERROR;
}

void CdmaCallTracker::HandlePollCalls(
    /* [in] */ AsyncResult* ar)
{
    AutoPtr<IList> polledCalls;

    if (ar->mException == NULL) {
        polledCalls = IList::Probe(ar->mResult);
    }
    else if (IsCommandExceptionRadioNotAvailable(ar->mException)) {
        // just a dummy empty ArrayList to cause the loop
        // to hang up all the calls
        CArrayList::New((IList**)&polledCalls);
    }
    else {
        // Radio probably wasn't ready--try again in a bit
        // But don't keep polling if the channel is closed
        PollCallsAfterDelay();
        return;
    }

    AutoPtr<IConnection> newRinging = NULL; //or waiting
    AutoPtr<IConnection> newUnknown = NULL;
    Boolean hasNonHangupStateChanged = FALSE;   // Any change besides
                                                // a dropped connection
    Boolean hasAnyCallDisconnected = FALSE;
    Boolean needsPollDelay = FALSE;
    Boolean unknownConnectionAppeared = FALSE;

    Int32 size;
    polledCalls->GetSize(&size);
    for (Int32 i = 0, curDC = 0, dcSize = size
            ; i < mConnections->GetLength(); i++) {
        AutoPtr<ICdmaConnection> conn = (*mConnections)[i];
        AutoPtr<IDriverCall> _dc;
        AutoPtr<DriverCall> dc;

        // polledCall list is sparse
        if (curDC < dcSize) {
            AutoPtr<IInterface> obj;
            polledCalls->Get(curDC, (IInterface**)&obj);
            _dc = IDriverCall::Probe(obj) ;
            dc = (DriverCall*)_dc.Get();

            if (dc->mIndex == i+1) {
                curDC++;
            }
            else {
                _dc = NULL;
            }
        }

        // if (DBG_POLL) Log(String("poll: conn[i=") + i + "]=" +
        //         conn+", dc=" + dc);

        if (conn == NULL && _dc != NULL) {
            // Connection appeared in CLCC response that we don't know about
            Boolean b;
            if (mPendingMO != NULL &&
                    (((CdmaConnection*)mPendingMO.Get())->CompareTo(_dc, &b), b)) {

                // if (DBG_POLL) Log("poll: pendingMO=" + mPendingMO);

                // It's our pending mobile originating call
                mConnections->Set(i, mPendingMO);
                ((CdmaConnection*)mPendingMO.Get())->mIndex = i;
                ((CdmaConnection*)mPendingMO.Get())->Update(_dc, &b);
                mPendingMO = NULL;

                // Someone has already asked to hangup this call
                if (mHangupPendingMO) {
                    mHangupPendingMO = FALSE;
                    // Re-start Ecm timer when an uncompleted emergency call ends
                    if (mIsEcmTimerCanceled) {
                        HandleEcmTimer(CDMAPhone::RESTART_ECM_TIMER);
                    }

                    // try {
                    if (IPhone::DEBUG_PHONE) Log(
                            String("poll: hangupPendingMO, hangup conn ")
                            + StringUtils::ToString(i));
                    Hangup((*mConnections)[i]);
                    // } catch (CallStateException ex) {
                    //     Logger::E(LOGTAG, "unexpected error on hangup");
                    // }

                    // Do not continue processing this poll
                    // Wait for hangup and repoll
                    return;
                }
            }
            else {
                if (IPhone::DEBUG_PHONE) {
                    // Log(String("pendingMo=") + mPendingMO + ", _dc=" + _dc);
                }

                AutoPtr<IContext> ctx;
                IPhone::Probe(mPhone)->GetContext((IContext**)&ctx);
                AutoPtr<ICdmaConnection> cdmaConn;
                CCdmaConnection::New(ctx, _dc, this, i, (ICdmaConnection**)&cdmaConn);
                mConnections->Set(i, cdmaConn);

                AutoPtr<IConnection> hoConnection = GetHoConnection(dc);
                if (hoConnection != NULL) {
                    // Single Radio Voice Call Continuity (SRVCC) completed
                    IConnection::Probe((*mConnections)[i])->MigrateFrom(hoConnection);
                    mHandoverConnections->Remove(hoConnection);
                    IPhoneBase::Probe(mPhone)->NotifyHandoverStateChanged(
                            IConnection::Probe((*mConnections)[i]));
                }
                else {
                    // find if the MT call is a new ring or unknown connection
                    newRinging = CheckMtFindNewRinging(dc,i);
                    if (newRinging == NULL) {
                        unknownConnectionAppeared = TRUE;
                        newUnknown = IConnection::Probe((*mConnections)[i]);
                    }
                }
                CheckAndEnableDataCallAfterEmergencyCallDropped();
            }
            hasNonHangupStateChanged = TRUE;
        }
        else if (conn != NULL && _dc == NULL) {
            // This case means the RIL has no more active call anymore and
            // we need to clean up the foregroundCall and ringingCall.
            // Loop through foreground call connections as
            // it contains the known logical connections.
            Int32 count;
            ((CdmaCall*)mForegroundCall.Get())->mConnections->GetSize(&count);
            for (Int32 n = 0; n < count; n++) {
                if (IPhone::DEBUG_PHONE) Log(String("adding fgCall cn ")
                    + StringUtils::ToString(n) + " to droppedDuringPoll");
                AutoPtr<IInterface> obj;
                ((CdmaCall*)mForegroundCall.Get())->mConnections->Get(n, (IInterface**)&obj);
                AutoPtr<ICdmaConnection> cn = ICdmaConnection::Probe(obj);
                mDroppedDuringPoll->Add(cn);
            }
            ((CdmaCall*)mRingingCall.Get())->mConnections->GetSize(&count);
            // Loop through ringing call connections as
            // it may contain the known logical connections.
            for (Int32 n = 0; n < count; n++) {
                if (IPhone::DEBUG_PHONE) Log(String("adding rgCall cn ")
                        + StringUtils::ToString(n) + " to droppedDuringPoll");
                AutoPtr<IInterface> obj;
                ((CdmaCall*)mRingingCall.Get())->mConnections->Get(n, (IInterface**)&obj);
                AutoPtr<ICdmaConnection> cn = ICdmaConnection::Probe(obj);
                mDroppedDuringPoll->Add(cn);
            }
            ICall::Probe(mForegroundCall)->SetGeneric(FALSE);
            ICall::Probe(mRingingCall)->SetGeneric(FALSE);

            // Re-start Ecm timer when the connected emergency call ends
            if (mIsEcmTimerCanceled) {
                HandleEcmTimer(CDMAPhone::RESTART_ECM_TIMER);
            }
            // If emergency call is not going through while dialing
            CheckAndEnableDataCallAfterEmergencyCallDropped();

            // Dropped connections are removed from the CallTracker
            // list but kept in the Call list
            mConnections->Set(i, NULL);
        }
        else if (conn != NULL && _dc != NULL) { /* implicit conn.compareTo(dc) */
            // Call collision case
            Boolean b;
            if ((IConnection::Probe(conn)->IsIncoming(&b), b) != dc->mIsMT) {
                if (dc->mIsMT == TRUE){
                    // Mt call takes precedence than Mo,drops Mo
                    mDroppedDuringPoll->Add(conn);
                    // find if the MT call is a new ring or unknown connection
                    newRinging = CheckMtFindNewRinging(dc,i);
                    if (newRinging == NULL) {
                        unknownConnectionAppeared = TRUE;
                        newUnknown = IConnection::Probe(conn);
                    }
                    CheckAndEnableDataCallAfterEmergencyCallDropped();
                }
                else {
                    // Call info stored in conn is not consistent with the call info from dc.
                    // We should follow the rule of MT calls taking precedence over MO calls
                    // when there is conflict, so here we drop the call info from dc and
                    // continue to use the call info from conn, and only take a log.
                    // Logger::E(LOGTAG,"Error in RIL, Phantom call appeared " + dc);
                }
            }
            else {
                Boolean changed;
                ((CdmaConnection*)conn.Get())->Update(dc, &changed);
                hasNonHangupStateChanged = hasNonHangupStateChanged || changed;
            }
        }

        if (REPEAT_POLLING) {
            if (dc != NULL) {
                // FIXME with RIL, we should not need this anymore
                if ((dc->mState == DriverCallState_DIALING
                        /*&& cm.getOption(cm.OPTION_POLL_DIALING)*/)
                    || (dc->mState == DriverCallState_ALERTING
                        /*&& cm.getOption(cm.OPTION_POLL_ALERTING)*/)
                    || (dc->mState == DriverCallState_INCOMING
                        /*&& cm.getOption(cm.OPTION_POLL_INCOMING)*/)
                    || (dc->mState == DriverCallState_WAITING
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
        ICallState foregroundCallState;
        ICall::Probe(mForegroundCall)->GetState(&foregroundCallState);
        Logger::D(LOGTAG,"Pending MO dropped before poll fg state:%d"
                        , foregroundCallState);

        mDroppedDuringPoll->Add(mPendingMO);
        mPendingMO = NULL;
        mHangupPendingMO = FALSE;
        if( mPendingCallInEcm) {
            mPendingCallInEcm = FALSE;
        }
        CheckAndEnableDataCallAfterEmergencyCallDropped();
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
        AutoPtr<ICdmaConnection> conn = ICdmaConnection::Probe(obj);

        Int64 connTime;
        IConnection::Probe(conn)->GetConnectTime(&connTime);
        Boolean b;
        Int32 connCause = ((CdmaConnection*)conn.Get())->mCause;
        if ((IConnection::Probe(conn)->IsIncoming(&b), b) && connTime == 0) {
            // Missed or rejected call
            Int32 cause;
            if (connCause == IDisconnectCause::LOCAL) {
                cause = IDisconnectCause::INCOMING_REJECTED;
            }
            else {
                cause = IDisconnectCause::INCOMING_MISSED;
            }

            if (IPhone::DEBUG_PHONE) {
                Log(String("missed/rejected call, conn.cause=")
                         + StringUtils::ToString(connCause));
                Log(String("setting cause to ") + StringUtils::ToString(cause));
            }
            mDroppedDuringPoll->Remove(i);
            hasAnyCallDisconnected |= ((CdmaConnection*)conn.Get())->OnDisconnect(cause, &b);
        }
        else if (connCause == IDisconnectCause::LOCAL
                || connCause == IDisconnectCause::INVALID_NUMBER) {
            mDroppedDuringPoll->Remove(i);
            hasAnyCallDisconnected |= ((CdmaConnection*)conn.Get())->OnDisconnect(connCause, &b);
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
        ((CDMAPhone*)mPhone.Get())->NotifyUnknownConnection(newUnknown);
    }

    if (hasNonHangupStateChanged || newRinging != NULL || hasAnyCallDisconnected) {
        ((CDMAPhone*)mPhone.Get())->NotifyPreciseCallStateChanged();
    }

    // //dumpState();
}

void CdmaCallTracker::Log(
    /* [in] */ const String& msg)
{
    Logger::D(LOGTAG, "[CdmaCallTracker] %s", msg.string());
}

//AutoPtr<CdmaConnection> CdmaCallTracker::MiddleInitMconnections[]()
//{
////    // CdmaConnection result = new CdmaConnection[MAX_CONNECTIONS];
////    AutoPtr<CdmaConnection> empty;
//    return empty;
//}

AutoPtr<IConnection> CdmaCallTracker::DialThreeWay(
    /* [in] */ const String& dialString)
{
    Boolean b;
    if (ICall::Probe(mForegroundCall)->IsIdle(&b), !b) {
        // Check data call
        DisableDataCallInEmergencyCall(dialString);

        // Attach the new connection to foregroundCall
        AutoPtr<IContext> ctx;
        IPhone::Probe(mPhone)->GetContext((IContext**)&ctx);
        CCdmaConnection::New(ctx,
                CheckForTestEmergencyNumber(dialString), this, mForegroundCall,
                (ICdmaConnection**)&mPendingMO);
        // Some network need a empty flash before sending the normal one
        AutoPtr<IResources> res;
        ctx->GetResources((IResources**)&res);
        res->GetInteger(R::integer::config_cdma_3waycall_flash_delay, &m3WayCallFlashDelay);

        if (m3WayCallFlashDelay > 0) {
            AutoPtr<IMessage> msg;
            ObtainMessage(EVENT_THREE_WAY_DIAL_BLANK_FLASH, (IMessage**)&msg);
            mCi->SendCDMAFeatureCode(String(""), msg);
        }
        else {
            String addr;
            IConnection::Probe(mPendingMO)->GetAddress(&addr);
            AutoPtr<IMessage> msg;
            ObtainMessage(EVENT_THREE_WAY_DIAL_L2_RESULT_CDMA, (IMessage**)&msg);
            mCi->SendCDMAFeatureCode(addr, msg);
        }
        return IConnection::Probe(mPendingMO);
    }
    return NULL;
}

void CdmaCallTracker::InternalClearDisconnected()
{
    ((CdmaCall*)mRingingCall.Get())->ClearDisconnected();
    ((CdmaCall*)mForegroundCall.Get())->ClearDisconnected();
    ((CdmaCall*)mBackgroundCall.Get())->ClearDisconnected();
}

AutoPtr<IMessage> CdmaCallTracker::ObtainCompleteMessage()
{
    return ObtainCompleteMessage(EVENT_OPERATION_COMPLETE);
}

AutoPtr<IMessage> CdmaCallTracker::ObtainCompleteMessage(
    /* [in] */ Int32 what)
{
    mPendingOperations++;
    mLastRelevantPoll = NULL;
    mNeedsPoll = TRUE;

    if (DBG_POLL) Log(String("ObtainCompleteMessage: pendingOperations=") +
            StringUtils::ToString(mPendingOperations)
            + ", needsPoll=" + StringUtils::BooleanToString(mNeedsPoll));

    AutoPtr<IMessage> msg;
    ObtainMessage(what, (IMessage**)&msg);
    return msg;
}

void CdmaCallTracker::OperationComplete()
{
    mPendingOperations--;

    if (DBG_POLL) Log(String("operationComplete: pendingOperations=") +
            StringUtils::ToString(mPendingOperations)
            + ", needsPoll=" + StringUtils::BooleanToString(mNeedsPoll));

    if (mPendingOperations == 0 && mNeedsPoll) {
        ObtainMessage(EVENT_POLL_CALLS_RESULT, (IMessage**)&mLastRelevantPoll);
        mCi->GetCurrentCalls(mLastRelevantPoll);
    }
    else if (mPendingOperations < 0) {
        // this should never happen
        Logger::E(LOGTAG,"CdmaCallTracker.pendingOperations < 0");
        mPendingOperations = 0;
    }
}

void CdmaCallTracker::UpdatePhoneState()
{
    PhoneConstantsState oldState = mState;

    Boolean b1, b2;
    if (ICall::Probe(mRingingCall)->IsRinging(&b1), b1) {
        mState = PhoneConstantsState_RINGING;
    }
    else if (mPendingMO != NULL ||
            ((ICall::Probe(mForegroundCall)->IsIdle(&b1), !b1)
            && (ICall::Probe(mBackgroundCall)->IsIdle(&b2), b2))) {
        mState = PhoneConstantsState_OFFHOOK;
    }
    else {
        mState = PhoneConstantsState_IDLE;
    }

    if (mState == PhoneConstantsState_IDLE && oldState != mState) {
        AutoPtr<AsyncResult> ar = new AsyncResult(NULL, NULL, NULL);
        mVoiceCallEndedRegistrants->NotifyRegistrants(ar);
    }
    else if (oldState == PhoneConstantsState_IDLE && oldState != mState) {
        AutoPtr<AsyncResult> ar = new AsyncResult(NULL, NULL, NULL);
        mVoiceCallStartedRegistrants->NotifyRegistrants (ar);
    }
    if (IPhone::DEBUG_PHONE) {
        Log(String("update phone state, old=") + StringUtils::ToString(oldState)
                + " new="+ StringUtils::ToString(mState));
    }
    if (mState != oldState) {
        ((CDMAPhone*)mPhone.Get())->NotifyPhoneStateChanged();
    }
}

void CdmaCallTracker::FlashAndSetGenericTrue()
{
    AutoPtr<IMessage> msg;
    ObtainMessage(EVENT_SWITCH_RESULT, (IMessage**)&msg);
    mCi->SendCDMAFeatureCode(String(""), msg);

    // Set generic to TRUE because in CDMA it is not known what
    // the status of the call is after a call waiting is answered,
    // 3 way call merged or a switch between calls.
    ICall::Probe(mForegroundCall)->SetGeneric(TRUE);
    ((CDMAPhone*)mPhone.Get())->NotifyPreciseCallStateChanged();
}

void CdmaCallTracker::HandleRadioNotAvailable()
{
    // handlePollCalls will clear out its
    // call list when it gets the CommandException
    // error result from this
    PollCallsWhenSafe();
}

void CdmaCallTracker::NotifyCallWaitingInfo(
    /* [in] */ ICdmaCallWaitingNotification* obj)
{
    if (mCallWaitingRegistrants != NULL) {
        AutoPtr<AsyncResult> ar = new AsyncResult(NULL, obj, NULL);
        mCallWaitingRegistrants->NotifyRegistrants(ar);
    }
}

void CdmaCallTracker::HandleCallWaitingInfo(
    /* [in] */ ICdmaCallWaitingNotification* cw)
{
    // Check how many connections in foregroundCall.
    // If the connection in foregroundCall is more
    // than one, then the connection information is
    // not reliable anymore since it means either
    // call waiting is connected or 3 way call is
    // dialed before, so set generic.
    Int32 size;
    ((CdmaCall*)mForegroundCall.Get())->mConnections->GetSize(&size);
    if (size > 1 ) {
        ICall::Probe(mForegroundCall)->SetGeneric(TRUE);
    }

    // Create a new CdmaConnection which attaches itself to ringingCall.
    ICall::Probe(mRingingCall)->SetGeneric(FALSE);
    AutoPtr<IContext> ctx;
    IPhone::Probe(mPhone)->GetContext((IContext**)&ctx);
    // new CdmaConnection(ctx, cw, this, mRingingCall);
    UpdatePhoneState();

    // Finally notify application
    NotifyCallWaitingInfo(cw);
}

void CdmaCallTracker::HandleEcmTimer(
    /* [in] */ Int32 action)
{
    ((CDMAPhone*)mPhone.Get())->HandleTimerInEmergencyCallbackMode(action);
    switch(action) {
        case CDMAPhone::CANCEL_ECM_TIMER: mIsEcmTimerCanceled = TRUE; break;
        case CDMAPhone::RESTART_ECM_TIMER: mIsEcmTimerCanceled = FALSE; break;
        default:
            Logger::E(LOGTAG, "handleEcmTimer, unsupported action %d", action);
    }
}

void CdmaCallTracker::DisableDataCallInEmergencyCall(
    /* [in] */ const String& dialString)
{
    Boolean b;
    AutoPtr<IContext> ctx;
    IPhone::Probe(mPhone)->GetContext((IContext**)&ctx);
    if (PhoneNumberUtils::IsLocalEmergencyNumber(ctx, dialString, &b),  b) {
        if (IPhone::DEBUG_PHONE) Log(String("disableDataCallInEmergencyCall"));
        mIsInEmergencyCall = TRUE;
        ((CDMAPhone*)mPhone.Get())->mDcTracker->SetInternalDataEnabled(FALSE, &b);
    }
}

void CdmaCallTracker::CheckAndEnableDataCallAfterEmergencyCallDropped()
{
    if (mIsInEmergencyCall) {
        mIsInEmergencyCall = FALSE;
        String inEcm;
        SystemProperties::Get(ITelephonyProperties::PROPERTY_INECM_MODE, String("FALSE"), &inEcm);
        if (IPhone::DEBUG_PHONE) {
            Log(String("checkAndEnableDataCallAfterEmergencyCallDropped,inEcm=") + inEcm);
        }
        if (inEcm.Compare("FALSE") == 0) {
            // Re-initiate data connection
            Boolean b;
            ((CDMAPhone*)mPhone.Get())->mDcTracker->SetInternalDataEnabled(TRUE, &b);
        }
    }
}

AutoPtr<IConnection> CdmaCallTracker::CheckMtFindNewRinging(
    /* [in] */ IDriverCall* _dc,
    /* [in] */ Int32 i)
{

    AutoPtr<IConnection> newRinging;

    // it's a ringing call
    AutoPtr<ICall> call;
    IConnection::Probe((*mConnections)[i])->GetCall((ICall**)&call);
    if (call.Get() == ICall::Probe(mRingingCall)) {
        newRinging = IConnection::Probe((*mConnections)[i]);
        // if (IPhone::DEBUG_PHONE) Log("Notify new ring " + dc);
    }
    else {
        // Something strange happened: a call which is neither
        // a ringing call nor the one we created. It could be the
        // call collision result from RIL
        // Logger::E(LOGTAG,"Phantom call appeared " + dc);
        // If it's a connected call, set the connect time so that
        // it's non-zero.  It may not be accurate, but at least
        // it won't appear as a Missed Call.
        AutoPtr<DriverCall> dc = (DriverCall*)_dc;
        if (dc->mState != DriverCallState_ALERTING
            && dc->mState != DriverCallState_DIALING) {
            ((CdmaConnection*)(*mConnections)[i])->OnConnectedInOrOut();
            if (dc->mState == DriverCallState_HOLDING) {
                // We've transitioned into HOLDING
                ((CdmaConnection*)(*mConnections)[i])->OnStartedHolding();
            }
        }
    }
    return newRinging;
}

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
