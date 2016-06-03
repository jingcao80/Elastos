#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Os.h"

#include "elastos/droid/internal/telephony/cdma/CdmaCallTracker.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {

//=====================================================================
//                           CdmaCallTracker
//=====================================================================
CAR_INTERFACE_IMPL(CdmaCallTracker, /*TODO CallTracker*/ Object, ICdmaCallTracker);

const String CdmaCallTracker::LOGTAG("CdmaCallTracker");
const Int32 CdmaCallTracker::MAX_CONNECTIONS;
const Int32 CdmaCallTracker::MAX_CONNECTIONS_PER_CALL;
const Boolean CdmaCallTracker::REPEAT_POLLING = FALSE;
const Boolean CdmaCallTracker::DBG_POLL = FALSE;

CdmaCallTracker::CdmaCallTracker(
    /* [in] */ ICDMAPhone* phone)
{
    // ==================before translated======================
    // mPhone = phone;
    // mCi = phone.mCi;
    // mCi.registerForCallStateChanged(this, EVENT_CALL_STATE_CHANGE, null);
    // mCi.registerForOn(this, EVENT_RADIO_AVAILABLE, null);
    // mCi.registerForNotAvailable(this, EVENT_RADIO_NOT_AVAILABLE, null);
    // mCi.registerForCallWaitingInfo(this, EVENT_CALL_WAITING_INFO_CDMA, null);
    // mForegroundCall.setGeneric(false);
}

ECode CdmaCallTracker::Dispose()
{
    // ==================before translated======================
    // Rlog.d(LOGTAG, "CdmaCallTracker dispose");
    // mCi.unregisterForLineControlInfo(this);
    // mCi.unregisterForCallStateChanged(this);
    // mCi.unregisterForOn(this);
    // mCi.unregisterForNotAvailable(this);
    // mCi.unregisterForCallWaitingInfo(this);
    //
    // clearDisconnected();
    assert(0);
    return NOERROR;
}

ECode CdmaCallTracker::RegisterForVoiceCallStarted(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    // ==================before translated======================
    // Registrant r = new Registrant(h, what, obj);
    // mVoiceCallStartedRegistrants.add(r);
    // // Notify if in call when registering
    // if (mState != PhoneConstants.State.IDLE) {
    //     r.notifyRegistrant(new AsyncResult(null, null, null));
    // }
    assert(0);
    return NOERROR;
}

ECode CdmaCallTracker::UnregisterForVoiceCallStarted(
    /* [in] */ IHandler* h)
{
    // ==================before translated======================
    // mVoiceCallStartedRegistrants.remove(h);
    assert(0);
    return NOERROR;
}

ECode CdmaCallTracker::RegisterForVoiceCallEnded(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    VALIDATE_NOT_NULL(h);
    VALIDATE_NOT_NULL(obj);
    // ==================before translated======================
    // Registrant r = new Registrant(h, what, obj);
    // mVoiceCallEndedRegistrants.add(r);
    assert(0);
    return NOERROR;
}

ECode CdmaCallTracker::UnregisterForVoiceCallEnded(
    /* [in] */ IHandler* h)
{
    VALIDATE_NOT_NULL(h);
    // ==================before translated======================
    // mVoiceCallEndedRegistrants.remove(h);
    assert(0);
    return NOERROR;
}

ECode CdmaCallTracker::RegisterForCallWaiting(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    // ==================before translated======================
    // Registrant r = new Registrant (h, what, obj);
    // mCallWaitingRegistrants.add(r);
    assert(0);
    return NOERROR;
}

ECode CdmaCallTracker::UnregisterForCallWaiting(
    /* [in] */ IHandler* h)
{
    VALIDATE_NOT_NULL(h);
    // ==================before translated======================
    // mCallWaitingRegistrants.remove(h);
    assert(0);
    return NOERROR;
}

ECode CdmaCallTracker::Dial(
    /* [in] */ const String& dialString,
    /* [in] */ Int32 clirMode,
    /* [out] */ IConnection** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // // note that this triggers call state changed notif
    // clearDisconnected();
    //
    // if (!canDial()) {
    //     throw new CallStateException("cannot dial in current state");
    // }
    //
    // String origNumber = dialString;
    // String operatorIsoContry = mPhone.getSystemProperty(
    //         TelephonyProperties.PROPERTY_OPERATOR_ISO_COUNTRY, "");
    // String simIsoContry = mPhone.getSystemProperty(
    //         TelephonyProperties.PROPERTY_ICC_OPERATOR_ISO_COUNTRY, "");
    // boolean internationalRoaming = !TextUtils.isEmpty(operatorIsoContry)
    //         && !TextUtils.isEmpty(simIsoContry)
    //         && !simIsoContry.equals(operatorIsoContry);
    // if (internationalRoaming) {
    //     if ("us".equals(simIsoContry)) {
    //         internationalRoaming = internationalRoaming && !"vi".equals(operatorIsoContry);
    //     } else if ("vi".equals(simIsoContry)) {
    //         internationalRoaming = internationalRoaming && !"us".equals(operatorIsoContry);
    //     }
    // }
    // if (internationalRoaming) {
    //     dialString = convertNumberIfNecessary(mPhone, dialString);
    // }
    //
    // String inEcm=SystemProperties.get(TelephonyProperties.PROPERTY_INECM_MODE, "false");
    // boolean isPhoneInEcmMode = inEcm.equals("true");
    // boolean isEmergencyCall =
    //         PhoneNumberUtils.isLocalEmergencyNumber(mPhone.getContext(), dialString);
    //
    // // Cancel Ecm timer if a second emergency call is originating in Ecm mode
    // if (isPhoneInEcmMode && isEmergencyCall) {
    //     handleEcmTimer(CDMAPhone.CANCEL_ECM_TIMER);
    // }
    //
    // // We are initiating a call therefore even if we previously
    // // didn't know the state (i.e. Generic was true) we now know
    // // and therefore can set Generic to false.
    // mForegroundCall.setGeneric(false);
    //
    // // The new call must be assigned to the foreground call.
    // // That call must be idle, so place anything that's
    // // there on hold
    // if (mForegroundCall.getState() == CdmaCall.State.ACTIVE) {
    //     return dialThreeWay(dialString);
    // }
    //
    // mPendingMO = new CdmaConnection(mPhone.getContext(), checkForTestEmergencyNumber(dialString),
    //         this, mForegroundCall);
    // mHangupPendingMO = false;
    //
    // if ( mPendingMO.getAddress() == null || mPendingMO.getAddress().length() == 0
    //         || mPendingMO.getAddress().indexOf(PhoneNumberUtils.WILD) >= 0 ) {
    //     // Phone number is invalid
    //     mPendingMO.mCause = DisconnectCause.INVALID_NUMBER;
    //
    //     // handlePollCalls() will notice this call not present
    //     // and will mark it as dropped.
    //     pollCallsWhenSafe();
    // } else {
    //     // Always unmute when initiating a new call
    //     setMute(false);
    //
    //     // Check data call
    //     disableDataCallInEmergencyCall(dialString);
    //
    //     // In Ecm mode, if another emergency call is dialed, Ecm mode will not exit.
    //     if(!isPhoneInEcmMode || (isPhoneInEcmMode && isEmergencyCall)) {
    //         mCi.dial(mPendingMO.getAddress(), clirMode, obtainCompleteMessage());
    //     } else {
    //         mPhone.exitEmergencyCallbackMode();
    //         mPhone.setOnEcbModeExitResponse(this,EVENT_EXIT_ECM_RESPONSE_CDMA, null);
    //         mPendingCallClirMode=clirMode;
    //         mPendingCallInEcm=true;
    //     }
    // }
    //
    // if (mNumberConverted) {
    //     mPendingMO.setConverted(origNumber);
    //     mNumberConverted = false;
    // }
    //
    // updatePhoneState();
    // mPhone.notifyPreciseCallStateChanged();
    //
    // return mPendingMO;
    assert(0);
    return NOERROR;
}

ECode CdmaCallTracker::Dial(
    /* [in] */ const String& dialString,
    /* [out] */ IConnection** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return dial(dialString, CommandsInterface.CLIR_DEFAULT);
    assert(0);
    return NOERROR;
}

ECode CdmaCallTracker::AcceptCall()
{
    // ==================before translated======================
    // if (mRingingCall.getState() == CdmaCall.State.INCOMING) {
    //     Rlog.i("phone", "acceptCall: incoming...");
    //     // Always unmute when answering a new call
    //     setMute(false);
    //     mCi.acceptCall(obtainCompleteMessage());
    // } else if (mRingingCall.getState() == CdmaCall.State.WAITING) {
    //     CdmaConnection cwConn = (CdmaConnection)(mRingingCall.getLatestConnection());
    //
    //     // Since there is no network response for supplimentary
    //     // service for CDMA, we assume call waiting is answered.
    //     // ringing Call state change to idle is in CdmaCall.detach
    //     // triggered by updateParent.
    //     cwConn.updateParent(mRingingCall, mForegroundCall);
    //     cwConn.onConnectedInOrOut();
    //     updatePhoneState();
    //     switchWaitingOrHoldingAndActive();
    // } else {
    //     throw new CallStateException("phone not ringing");
    // }
    assert(0);
    return NOERROR;
}

ECode CdmaCallTracker::RejectCall()
{
    // ==================before translated======================
    // // AT+CHLD=0 means "release held or UDUB"
    // // so if the phone isn't ringing, this could hang up held
    // if (mRingingCall.getState().isRinging()) {
    //     mCi.rejectCall(obtainCompleteMessage());
    // } else {
    //     throw new CallStateException("phone not ringing");
    // }
    assert(0);
    return NOERROR;
}

ECode CdmaCallTracker::SwitchWaitingOrHoldingAndActive()
{
    // ==================before translated======================
    // // Should we bother with this check?
    // if (mRingingCall.getState() == CdmaCall.State.INCOMING) {
    //     throw new CallStateException("cannot be in the incoming state");
    // } else if (mForegroundCall.getConnections().size() > 1) {
    //     flashAndSetGenericTrue();
    // } else {
    //     // Send a flash command to CDMA network for putting the other party on hold.
    //     // For CDMA networks which do not support this the user would just hear a beep
    //     // from the network. For CDMA networks which do support it will put the other
    //     // party on hold.
    //     mCi.sendCDMAFeatureCode("", obtainMessage(EVENT_SWITCH_RESULT));
    // }
    assert(0);
    return NOERROR;
}

ECode CdmaCallTracker::Conference()
{
    // ==================before translated======================
    // // Should we be checking state?
    // flashAndSetGenericTrue();
    assert(0);
    return NOERROR;
}

ECode CdmaCallTracker::ExplicitCallTransfer()
{
    // ==================before translated======================
    // mCi.explicitCallTransfer(obtainCompleteMessage(EVENT_ECT_RESULT));
    assert(0);
    return NOERROR;
}

ECode CdmaCallTracker::ClearDisconnected()
{
    // ==================before translated======================
    // internalClearDisconnected();
    //
    // updatePhoneState();
    // mPhone.notifyPreciseCallStateChanged();
    assert(0);
    return NOERROR;
}

ECode CdmaCallTracker::CanConference(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mForegroundCall.getState() == CdmaCall.State.ACTIVE
    //         && mBackgroundCall.getState() == CdmaCall.State.HOLDING
    //         && !mBackgroundCall.isFull()
    //         && !mForegroundCall.isFull();
    assert(0);
    return NOERROR;
}

ECode CdmaCallTracker::CanDial(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // boolean ret;
    // int serviceState = mPhone.getServiceState().getState();
    // String disableCall = SystemProperties.get(
    //         TelephonyProperties.PROPERTY_DISABLE_CALL, "false");
    //
    // ret = (serviceState != ServiceState.STATE_POWER_OFF)
    //         && mPendingMO == null
    //         && !mRingingCall.isRinging()
    //         && !disableCall.equals("true")
    //         && (!mForegroundCall.getState().isAlive()
    //             || (mForegroundCall.getState() == CdmaCall.State.ACTIVE)
    //             || !mBackgroundCall.getState().isAlive());
    //
    // if (!ret) {
    //     log(String.format("canDial is false\n" +
    //                       "((serviceState=%d) != ServiceState.STATE_POWER_OFF)::=%s\n" +
    //                       "&& pendingMO == null::=%s\n" +
    //                       "&& !ringingCall.isRinging()::=%s\n" +
    //                       "&& !disableCall.equals(\"true\")::=%s\n" +
    //                       "&& (!foregroundCall.getState().isAlive()::=%s\n" +
    //                       "   || foregroundCall.getState() == CdmaCall.State.ACTIVE::=%s\n" +
    //                       "   ||!backgroundCall.getState().isAlive())::=%s)",
    //             serviceState,
    //             serviceState != ServiceState.STATE_POWER_OFF,
    //             mPendingMO == null,
    //             !mRingingCall.isRinging(),
    //             !disableCall.equals("true"),
    //             !mForegroundCall.getState().isAlive(),
    //             mForegroundCall.getState() == CdmaCall.State.ACTIVE,
    //             !mBackgroundCall.getState().isAlive()));
    // }
    // return ret;
    assert(0);
    return NOERROR;
}

ECode CdmaCallTracker::CanTransfer(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // Rlog.e(LOGTAG, "canTransfer: not possible in CDMA");
    // return false;
    assert(0);
    return NOERROR;
}

ECode CdmaCallTracker::Hangup(
    /* [in] */ ICdmaConnection* conn)
{
    VALIDATE_NOT_NULL(conn);
    // ==================before translated======================
    // if (conn.mOwner != this) {
    //     throw new CallStateException ("CdmaConnection " + conn
    //                             + "does not belong to CdmaCallTracker " + this);
    // }
    //
    // if (conn == mPendingMO) {
    //     // We're hanging up an outgoing call that doesn't have it's
    //     // GSM index assigned yet
    //
    //     if (Phone.DEBUG_PHONE) log("hangup: set hangupPendingMO to true");
    //     mHangupPendingMO = true;
    // } else if ((conn.getCall() == mRingingCall)
    //         && (mRingingCall.getState() == CdmaCall.State.WAITING)) {
    //     // Handle call waiting hang up case.
    //     //
    //     // The ringingCall state will change to IDLE in CdmaCall.detach
    //     // if the ringing call connection size is 0. We don't specifically
    //     // set the ringing call state to IDLE here to avoid a race condition
    //     // where a new call waiting could get a hang up from an old call
    //     // waiting ringingCall.
    //     //
    //     // PhoneApp does the call log itself since only PhoneApp knows
    //     // the hangup reason is user ignoring or timing out. So conn.onDisconnect()
    //     // is not called here. Instead, conn.onLocalDisconnect() is called.
    //     conn.onLocalDisconnect();
    //     updatePhoneState();
    //     mPhone.notifyPreciseCallStateChanged();
    //     return;
    // } else {
    //     try {
    //         mCi.hangupConnection (conn.getCDMAIndex(), obtainCompleteMessage());
    //     } catch (CallStateException ex) {
    //         // Ignore "connection not found"
    //         // Call may have hung up already
    //         Rlog.w(LOGTAG,"CdmaCallTracker WARN: hangup() on absent connection "
    //                         + conn);
    //     }
    // }
    //
    // conn.onHangupLocal();
    assert(0);
    return NOERROR;
}

ECode CdmaCallTracker::Separate(
    /* [in] */ ICdmaConnection* conn)
{
    VALIDATE_NOT_NULL(conn);
    // ==================before translated======================
    // if (conn.mOwner != this) {
    //     throw new CallStateException ("CdmaConnection " + conn
    //                             + "does not belong to CdmaCallTracker " + this);
    // }
    // try {
    //     mCi.separateConnection (conn.getCDMAIndex(),
    //         obtainCompleteMessage(EVENT_SEPARATE_RESULT));
    // } catch (CallStateException ex) {
    //     // Ignore "connection not found"
    //     // Call may have hung up already
    //     Rlog.w(LOGTAG,"CdmaCallTracker WARN: separate() on absent connection "
    //                   + conn);
    // }
    assert(0);
    return NOERROR;
}

ECode CdmaCallTracker::SetMute(
    /* [in] */ Boolean mute)
{
    // ==================before translated======================
    // mDesiredMute = mute;
    // mCi.setMute(mDesiredMute, null);
    assert(0);
    return NOERROR;
}

ECode CdmaCallTracker::GetMute(
    /* [out] */ Boolean* result)
{
    // ==================before translated======================
    // return mDesiredMute;
    assert(0);
    return NOERROR;
}

ECode CdmaCallTracker::Hangup(
    /* [in] */ ICdmaCall* call)
{
    // ==================before translated======================
    // if (call.getConnections().size() == 0) {
    //     throw new CallStateException("no connections in call");
    // }
    //
    // if (call == mRingingCall) {
    //     if (Phone.DEBUG_PHONE) log("(ringing) hangup waiting or background");
    //     mCi.hangupWaitingOrBackground(obtainCompleteMessage());
    // } else if (call == mForegroundCall) {
    //     if (call.isDialingOrAlerting()) {
    //         if (Phone.DEBUG_PHONE) {
    //             log("(foregnd) hangup dialing or alerting...");
    //         }
    //         hangup((CdmaConnection)(call.getConnections().get(0)));
    //     } else {
    //         hangupForegroundResumeBackground();
    //     }
    // } else if (call == mBackgroundCall) {
    //     if (mRingingCall.isRinging()) {
    //         if (Phone.DEBUG_PHONE) {
    //             log("hangup all conns in background call");
    //         }
    //         hangupAllConnections(call);
    //     } else {
    //         hangupWaitingOrBackground();
    //     }
    // } else {
    //     throw new RuntimeException ("CdmaCall " + call +
    //             "does not belong to CdmaCallTracker " + this);
    // }
    //
    // call.onHangupLocal();
    // mPhone.notifyPreciseCallStateChanged();
    assert(0);
    return NOERROR;
}

ECode CdmaCallTracker::HangupWaitingOrBackground()
{
    // ==================before translated======================
    // if (Phone.DEBUG_PHONE) log("hangupWaitingOrBackground");
    // mCi.hangupWaitingOrBackground(obtainCompleteMessage());
    assert(0);
    return NOERROR;
}

ECode CdmaCallTracker::HangupForegroundResumeBackground()
{
    // ==================before translated======================
    // if (Phone.DEBUG_PHONE) log("hangupForegroundResumeBackground");
    // mCi.hangupForegroundResumeBackground(obtainCompleteMessage());
    assert(0);
    return NOERROR;
}

ECode CdmaCallTracker::HangupConnectionByIndex(
    /* [in] */ ICdmaCall* call,
    /* [in] */ Int32 index)
{
    VALIDATE_NOT_NULL(call);
    // ==================before translated======================
    // int count = call.mConnections.size();
    // for (int i = 0; i < count; i++) {
    //     CdmaConnection cn = (CdmaConnection)call.mConnections.get(i);
    //     if (cn.getCDMAIndex() == index) {
    //         mCi.hangupConnection(index, obtainCompleteMessage());
    //         return;
    //     }
    // }
    //
    // throw new CallStateException("no gsm index found");
    assert(0);
    return NOERROR;
}

ECode CdmaCallTracker::HangupAllConnections(
    /* [in] */ ICdmaCall* call)
{
    VALIDATE_NOT_NULL(call);
    // ==================before translated======================
    // try {
    //     int count = call.mConnections.size();
    //     for (int i = 0; i < count; i++) {
    //         CdmaConnection cn = (CdmaConnection)call.mConnections.get(i);
    //         mCi.hangupConnection(cn.getCDMAIndex(), obtainCompleteMessage());
    //     }
    // } catch (CallStateException ex) {
    //     Rlog.e(LOGTAG, "hangupConnectionByIndex caught " + ex);
    // }
    assert(0);
    return NOERROR;
}

ECode CdmaCallTracker::GetConnectionByIndex(
    /* [in] */ ICdmaCall* call,
    /* [in] */ Int32 index,
    /* [out] */ ICdmaConnection** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // int count = call.mConnections.size();
    // for (int i = 0; i < count; i++) {
    //     CdmaConnection cn = (CdmaConnection)call.mConnections.get(i);
    //     if (cn.getCDMAIndex() == index) {
    //         return cn;
    //     }
    // }
    //
    // return null;
    assert(0);
    return NOERROR;
}

ECode CdmaCallTracker::HandleMessage(
    /* [in] */ IMessage* msg)
{
    VALIDATE_NOT_NULL(msg);
    // ==================before translated======================
    // AsyncResult ar;
    //
    // if (!mPhone.mIsTheCurrentActivePhone) {
    //     Rlog.w(LOGTAG, "Ignoring events received on inactive CdmaPhone");
    //     return;
    // }
    // switch (msg.what) {
    //     case EVENT_POLL_CALLS_RESULT:{
    //         Rlog.d(LOGTAG, "Event EVENT_POLL_CALLS_RESULT Received");
    //         ar = (AsyncResult)msg.obj;
    //
    //         if(msg == mLastRelevantPoll) {
    //             if(DBG_POLL) log(
    //                     "handle EVENT_POLL_CALL_RESULT: set needsPoll=F");
    //             mNeedsPoll = false;
    //             mLastRelevantPoll = null;
    //             handlePollCalls((AsyncResult)msg.obj);
    //         }
    //     }
    //     break;
    //
    //     case EVENT_OPERATION_COMPLETE:
    //         operationComplete();
    //     break;
    //
    //     case EVENT_SWITCH_RESULT:
    //          // In GSM call operationComplete() here which gets the
    //          // current call list. But in CDMA there is no list so
    //          // there is nothing to do.
    //     break;
    //
    //     case EVENT_GET_LAST_CALL_FAIL_CAUSE:
    //         int causeCode;
    //         ar = (AsyncResult)msg.obj;
    //
    //         operationComplete();
    //
    //         if (ar.exception != null) {
    //             // An exception occurred...just treat the disconnect
    //             // cause as "normal"
    //             causeCode = CallFailCause.NORMAL_CLEARING;
    //             Rlog.i(LOGTAG,
    //                     "Exception during getLastCallFailCause, assuming normal disconnect");
    //         } else {
    //             causeCode = ((int[])ar.result)[0];
    //         }
    //
    //         for (int i = 0, s =  mDroppedDuringPoll.size()
    //                 ; i < s ; i++
    //         ) {
    //             CdmaConnection conn = mDroppedDuringPoll.get(i);
    //
    //             conn.onRemoteDisconnect(causeCode);
    //         }
    //
    //         updatePhoneState();
    //
    //         mPhone.notifyPreciseCallStateChanged();
    //         mDroppedDuringPoll.clear();
    //     break;
    //
    //     case EVENT_REPOLL_AFTER_DELAY:
    //     case EVENT_CALL_STATE_CHANGE:
    //         pollCallsWhenSafe();
    //     break;
    //
    //     case EVENT_RADIO_AVAILABLE:
    //         handleRadioAvailable();
    //     break;
    //
    //     case EVENT_RADIO_NOT_AVAILABLE:
    //         handleRadioNotAvailable();
    //     break;
    //
    //     case EVENT_EXIT_ECM_RESPONSE_CDMA:
    //         // no matter the result, we still do the same here
    //         if ( mPendingCallInEcm ) {
    //             mCi.dial(mPendingMO.getAddress(), mPendingCallClirMode, obtainCompleteMessage());
    //             mPendingCallInEcm = false;
    //         }
    //         mPhone.unsetOnEcbModeExitResponse(this);
    //     break;
    //
    //     case EVENT_CALL_WAITING_INFO_CDMA:
    //        ar = (AsyncResult)msg.obj;
    //        if (ar.exception == null) {
    //            handleCallWaitingInfo((CdmaCallWaitingNotification)ar.result);
    //            Rlog.d(LOGTAG, "Event EVENT_CALL_WAITING_INFO_CDMA Received");
    //        }
    //     break;
    //
    //     case EVENT_THREE_WAY_DIAL_L2_RESULT_CDMA:
    //         ar = (AsyncResult)msg.obj;
    //         if (ar.exception == null) {
    //             // Assume 3 way call is connected
    //             mPendingMO.onConnectedInOrOut();
    //             mPendingMO = null;
    //         }
    //     break;
    //
    //     case EVENT_THREE_WAY_DIAL_BLANK_FLASH:
    //         ar = (AsyncResult) msg.obj;
    //         if (ar.exception == null) {
    //             postDelayed(
    //                     new Runnable() {
    //                         public void run() {
    //                             if (mPendingMO != null) {
    //                                 mCi.sendCDMAFeatureCode(mPendingMO.getAddress(),
    //                                         obtainMessage(EVENT_THREE_WAY_DIAL_L2_RESULT_CDMA));
    //                             }
    //                         }
    //                     }, m3WayCallFlashDelay);
    //         } else {
    //             mPendingMO = null;
    //             Rlog.w(LOGTAG, "exception happened on Blank Flash for 3-way call");
    //         }
    //     break;
    //
    //     default:{
    //        throw new RuntimeException("unexpected event not handled");
    //     }
    // }
    assert(0);
    return NOERROR;
}

ECode CdmaCallTracker::IsInEmergencyCall(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mIsInEmergencyCall;
    assert(0);
    return NOERROR;
}

ECode CdmaCallTracker::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    // ==================before translated======================
    // pw.println("GsmCallTracker extends:");
    // super.dump(fd, pw, args);
    // pw.println("droppedDuringPoll: length=" + mConnections.length);
    // for(int i=0; i < mConnections.length; i++) {
    //     pw.printf(" mConnections[%d]=%s\n", i, mConnections[i]);
    // }
    // pw.println(" mVoiceCallEndedRegistrants=" + mVoiceCallEndedRegistrants);
    // pw.println(" mVoiceCallStartedRegistrants=" + mVoiceCallStartedRegistrants);
    // pw.println(" mCallWaitingRegistrants=" + mCallWaitingRegistrants);
    // pw.println("droppedDuringPoll: size=" + mDroppedDuringPoll.size());
    // for(int i = 0; i < mDroppedDuringPoll.size(); i++) {
    //     pw.printf( " mDroppedDuringPoll[%d]=%s\n", i, mDroppedDuringPoll.get(i));
    // }
    // pw.println(" mRingingCall=" + mRingingCall);
    // pw.println(" mForegroundCall=" + mForegroundCall);
    // pw.println(" mBackgroundCall=" + mBackgroundCall);
    // pw.println(" mPendingMO=" + mPendingMO);
    // pw.println(" mHangupPendingMO=" + mHangupPendingMO);
    // pw.println(" mPendingCallInEcm=" + mPendingCallInEcm);
    // pw.println(" mIsInEmergencyCall=" + mIsInEmergencyCall);
    // pw.println(" mPhone=" + mPhone);
    // pw.println(" mDesiredMute=" + mDesiredMute);
    // pw.println(" mPendingCallClirMode=" + mPendingCallClirMode);
    // pw.println(" mState=" + mState);
    // pw.println(" mIsEcmTimerCanceled=" + mIsEcmTimerCanceled);
    assert(0);
    return NOERROR;
}

void CdmaCallTracker::Finalize()
{
    // ==================before translated======================
    // Rlog.d(LOGTAG, "CdmaCallTracker finalized");
    assert(0);
}

void CdmaCallTracker::HandlePollCalls(
    /* [in] */ AsyncResult* ar)
{
    // ==================before translated======================
    // List polledCalls;
    //
    // if (ar.exception == null) {
    //     polledCalls = (List)ar.result;
    // } else if (isCommandExceptionRadioNotAvailable(ar.exception)) {
    //     // just a dummy empty ArrayList to cause the loop
    //     // to hang up all the calls
    //     polledCalls = new ArrayList();
    // } else {
    //     // Radio probably wasn't ready--try again in a bit
    //     // But don't keep polling if the channel is closed
    //     pollCallsAfterDelay();
    //     return;
    // }
    //
    // Connection newRinging = null; //or waiting
    // Connection newUnknown = null;
    // boolean hasNonHangupStateChanged = false;   // Any change besides
    //                                             // a dropped connection
    // boolean hasAnyCallDisconnected = false;
    // boolean needsPollDelay = false;
    // boolean unknownConnectionAppeared = false;
    //
    // for (int i = 0, curDC = 0, dcSize = polledCalls.size()
    //         ; i < mConnections.length; i++) {
    //     CdmaConnection conn = mConnections[i];
    //     DriverCall dc = null;
    //
    //     // polledCall list is sparse
    //     if (curDC < dcSize) {
    //         dc = (DriverCall) polledCalls.get(curDC);
    //
    //         if (dc.index == i+1) {
    //             curDC++;
    //         } else {
    //             dc = null;
    //         }
    //     }
    //
    //     if (DBG_POLL) log("poll: conn[i=" + i + "]=" +
    //             conn+", dc=" + dc);
    //
    //     if (conn == null && dc != null) {
    //         // Connection appeared in CLCC response that we don't know about
    //         if (mPendingMO != null && mPendingMO.compareTo(dc)) {
    //
    //             if (DBG_POLL) log("poll: pendingMO=" + mPendingMO);
    //
    //             // It's our pending mobile originating call
    //             mConnections[i] = mPendingMO;
    //             mPendingMO.mIndex = i;
    //             mPendingMO.update(dc);
    //             mPendingMO = null;
    //
    //             // Someone has already asked to hangup this call
    //             if (mHangupPendingMO) {
    //                 mHangupPendingMO = false;
    //                 // Re-start Ecm timer when an uncompleted emergency call ends
    //                 if (mIsEcmTimerCanceled) {
    //                     handleEcmTimer(CDMAPhone.RESTART_ECM_TIMER);
    //                 }
    //
    //                 try {
    //                     if (Phone.DEBUG_PHONE) log(
    //                             "poll: hangupPendingMO, hangup conn " + i);
    //                     hangup(mConnections[i]);
    //                 } catch (CallStateException ex) {
    //                     Rlog.e(LOGTAG, "unexpected error on hangup");
    //                 }
    //
    //                 // Do not continue processing this poll
    //                 // Wait for hangup and repoll
    //                 return;
    //             }
    //         } else {
    //             if (Phone.DEBUG_PHONE) {
    //                 log("pendingMo=" + mPendingMO + ", dc=" + dc);
    //             }
    //             mConnections[i] = new CdmaConnection(mPhone.getContext(), dc, this, i);
    //
    //             Connection hoConnection = getHoConnection(dc);
    //             if (hoConnection != null) {
    //                 // Single Radio Voice Call Continuity (SRVCC) completed
    //                 mConnections[i].migrateFrom(hoConnection);
    //                 mHandoverConnections.remove(hoConnection);
    //                 mPhone.notifyHandoverStateChanged(mConnections[i]);
    //             } else {
    //                 // find if the MT call is a new ring or unknown connection
    //                 newRinging = checkMtFindNewRinging(dc,i);
    //                 if (newRinging == null) {
    //                     unknownConnectionAppeared = true;
    //                     newUnknown = mConnections[i];
    //                 }
    //             }
    //             checkAndEnableDataCallAfterEmergencyCallDropped();
    //         }
    //         hasNonHangupStateChanged = true;
    //     } else if (conn != null && dc == null) {
    //         // This case means the RIL has no more active call anymore and
    //         // we need to clean up the foregroundCall and ringingCall.
    //         // Loop through foreground call connections as
    //         // it contains the known logical connections.
    //         int count = mForegroundCall.mConnections.size();
    //         for (int n = 0; n < count; n++) {
    //             if (Phone.DEBUG_PHONE) log("adding fgCall cn " + n + " to droppedDuringPoll");
    //             CdmaConnection cn = (CdmaConnection)mForegroundCall.mConnections.get(n);
    //             mDroppedDuringPoll.add(cn);
    //         }
    //         count = mRingingCall.mConnections.size();
    //         // Loop through ringing call connections as
    //         // it may contain the known logical connections.
    //         for (int n = 0; n < count; n++) {
    //             if (Phone.DEBUG_PHONE) log("adding rgCall cn " + n + " to droppedDuringPoll");
    //             CdmaConnection cn = (CdmaConnection)mRingingCall.mConnections.get(n);
    //             mDroppedDuringPoll.add(cn);
    //         }
    //         mForegroundCall.setGeneric(false);
    //         mRingingCall.setGeneric(false);
    //
    //         // Re-start Ecm timer when the connected emergency call ends
    //         if (mIsEcmTimerCanceled) {
    //             handleEcmTimer(CDMAPhone.RESTART_ECM_TIMER);
    //         }
    //         // If emergency call is not going through while dialing
    //         checkAndEnableDataCallAfterEmergencyCallDropped();
    //
    //         // Dropped connections are removed from the CallTracker
    //         // list but kept in the Call list
    //         mConnections[i] = null;
    //     } else if (conn != null && dc != null) { /* implicit conn.compareTo(dc) */
    //         // Call collision case
    //         if (conn.isIncoming() != dc.isMT) {
    //             if (dc.isMT == true){
    //                 // Mt call takes precedence than Mo,drops Mo
    //                 mDroppedDuringPoll.add(conn);
    //                 // find if the MT call is a new ring or unknown connection
    //                 newRinging = checkMtFindNewRinging(dc,i);
    //                 if (newRinging == null) {
    //                     unknownConnectionAppeared = true;
    //                     newUnknown = conn;
    //                 }
    //                 checkAndEnableDataCallAfterEmergencyCallDropped();
    //             } else {
    //                 // Call info stored in conn is not consistent with the call info from dc.
    //                 // We should follow the rule of MT calls taking precedence over MO calls
    //                 // when there is conflict, so here we drop the call info from dc and
    //                 // continue to use the call info from conn, and only take a log.
    //                 Rlog.e(LOGTAG,"Error in RIL, Phantom call appeared " + dc);
    //             }
    //         } else {
    //             boolean changed;
    //             changed = conn.update(dc);
    //             hasNonHangupStateChanged = hasNonHangupStateChanged || changed;
    //         }
    //     }
    //
    //     if (REPEAT_POLLING) {
    //         if (dc != null) {
    //             // FIXME with RIL, we should not need this anymore
    //             if ((dc.state == DriverCall.State.DIALING
    //                     /*&& cm.getOption(cm.OPTION_POLL_DIALING)*/)
    //                 || (dc.state == DriverCall.State.ALERTING
    //                     /*&& cm.getOption(cm.OPTION_POLL_ALERTING)*/)
    //                 || (dc.state == DriverCall.State.INCOMING
    //                     /*&& cm.getOption(cm.OPTION_POLL_INCOMING)*/)
    //                 || (dc.state == DriverCall.State.WAITING
    //                     /*&& cm.getOption(cm.OPTION_POLL_WAITING)*/)
    //             ) {
    //                 // Sometimes there's no unsolicited notification
    //                 // for state transitions
    //                 needsPollDelay = true;
    //             }
    //         }
    //     }
    // }
    //
    // // This is the first poll after an ATD.
    // // We expect the pending call to appear in the list
    // // If it does not, we land here
    // if (mPendingMO != null) {
    //     Rlog.d(LOGTAG,"Pending MO dropped before poll fg state:"
    //                     + mForegroundCall.getState());
    //
    //     mDroppedDuringPoll.add(mPendingMO);
    //     mPendingMO = null;
    //     mHangupPendingMO = false;
    //     if( mPendingCallInEcm) {
    //         mPendingCallInEcm = false;
    //     }
    //     checkAndEnableDataCallAfterEmergencyCallDropped();
    // }
    //
    // if (newRinging != null) {
    //     mPhone.notifyNewRingingConnection(newRinging);
    // }
    //
    // // clear the "local hangup" and "missed/rejected call"
    // // cases from the "dropped during poll" list
    // // These cases need no "last call fail" reason
    // for (int i = mDroppedDuringPoll.size() - 1; i >= 0 ; i--) {
    //     CdmaConnection conn = mDroppedDuringPoll.get(i);
    //
    //     if (conn.isIncoming() && conn.getConnectTime() == 0) {
    //         // Missed or rejected call
    //         int cause;
    //         if (conn.mCause == DisconnectCause.LOCAL) {
    //             cause = DisconnectCause.INCOMING_REJECTED;
    //         } else {
    //             cause = DisconnectCause.INCOMING_MISSED;
    //         }
    //
    //         if (Phone.DEBUG_PHONE) {
    //             log("missed/rejected call, conn.cause=" + conn.mCause);
    //             log("setting cause to " + cause);
    //         }
    //         mDroppedDuringPoll.remove(i);
    //         hasAnyCallDisconnected |= conn.onDisconnect(cause);
    //     } else if (conn.mCause == DisconnectCause.LOCAL
    //             || conn.mCause == DisconnectCause.INVALID_NUMBER) {
    //         mDroppedDuringPoll.remove(i);
    //         hasAnyCallDisconnected |= conn.onDisconnect(conn.mCause);
    //     }
    // }
    //
    // // Any non-local disconnects: determine cause
    // if (mDroppedDuringPoll.size() > 0) {
    //     mCi.getLastCallFailCause(
    //         obtainNoPollCompleteMessage(EVENT_GET_LAST_CALL_FAIL_CAUSE));
    // }
    //
    // if (needsPollDelay) {
    //     pollCallsAfterDelay();
    // }
    //
    // // Cases when we can no longer keep disconnected Connection's
    // // with their previous calls
    // // 1) the phone has started to ring
    // // 2) A Call/Connection object has changed state...
    // //    we may have switched or held or answered (but not hung up)
    // if (newRinging != null || hasNonHangupStateChanged || hasAnyCallDisconnected) {
    //     internalClearDisconnected();
    // }
    //
    // updatePhoneState();
    //
    // if (unknownConnectionAppeared) {
    //     mPhone.notifyUnknownConnection(newUnknown);
    // }
    //
    // if (hasNonHangupStateChanged || newRinging != null || hasAnyCallDisconnected) {
    //     mPhone.notifyPreciseCallStateChanged();
    // }
    //
    // //dumpState();
    assert(0);
}

void CdmaCallTracker::Log(
    /* [in] */ const String& msg)
{
    // ==================before translated======================
    // Rlog.d(LOGTAG, "[CdmaCallTracker] " + msg);
    assert(0);
}

//AutoPtr<CdmaConnection> CdmaCallTracker::MiddleInitMconnections[]()
//{
//    // ==================before translated======================
//    // CdmaConnection result = new CdmaConnection[MAX_CONNECTIONS];
//    assert(0);
//    AutoPtr<CdmaConnection> empty;
//    return empty;
//}

AutoPtr<IConnection> CdmaCallTracker::DialThreeWay(
    /* [in] */ const String& dialString)
{
    // ==================before translated======================
    // if (!mForegroundCall.isIdle()) {
    //     // Check data call
    //     disableDataCallInEmergencyCall(dialString);
    //
    //     // Attach the new connection to foregroundCall
    //     mPendingMO = new CdmaConnection(mPhone.getContext(),
    //                         checkForTestEmergencyNumber(dialString), this, mForegroundCall);
    //     // Some network need a empty flash before sending the normal one
    //     m3WayCallFlashDelay = mPhone.getContext().getResources()
    //             .getInteger(com.android.internal.R.integer.config_cdma_3waycall_flash_delay);
    //     if (m3WayCallFlashDelay > 0) {
    //         mCi.sendCDMAFeatureCode("", obtainMessage(EVENT_THREE_WAY_DIAL_BLANK_FLASH));
    //     } else {
    //         mCi.sendCDMAFeatureCode(mPendingMO.getAddress(),
    //                 obtainMessage(EVENT_THREE_WAY_DIAL_L2_RESULT_CDMA));
    //     }
    //     return mPendingMO;
    // }
    // return null;
    assert(0);
    AutoPtr<IConnection> empty;
    return empty;
}

void CdmaCallTracker::InternalClearDisconnected()
{
    // ==================before translated======================
    // mRingingCall.clearDisconnected();
    // mForegroundCall.clearDisconnected();
    // mBackgroundCall.clearDisconnected();
    assert(0);
}

AutoPtr<IMessage> CdmaCallTracker::ObtainCompleteMessage()
{
    // ==================before translated======================
    // return obtainCompleteMessage(EVENT_OPERATION_COMPLETE);
    assert(0);
    AutoPtr<IMessage> empty;
    return empty;
}

AutoPtr<IMessage> CdmaCallTracker::ObtainCompleteMessage(
    /* [in] */ Int32 what)
{
    // ==================before translated======================
    // mPendingOperations++;
    // mLastRelevantPoll = null;
    // mNeedsPoll = true;
    //
    // if (DBG_POLL) log("obtainCompleteMessage: pendingOperations=" +
    //         mPendingOperations + ", needsPoll=" + mNeedsPoll);
    //
    // return obtainMessage(what);
    assert(0);
    AutoPtr<IMessage> empty;
    return empty;
}

void CdmaCallTracker::OperationComplete()
{
    // ==================before translated======================
    // mPendingOperations--;
    //
    // if (DBG_POLL) log("operationComplete: pendingOperations=" +
    //         mPendingOperations + ", needsPoll=" + mNeedsPoll);
    //
    // if (mPendingOperations == 0 && mNeedsPoll) {
    //     mLastRelevantPoll = obtainMessage(EVENT_POLL_CALLS_RESULT);
    //     mCi.getCurrentCalls(mLastRelevantPoll);
    // } else if (mPendingOperations < 0) {
    //     // this should never happen
    //     Rlog.e(LOGTAG,"CdmaCallTracker.pendingOperations < 0");
    //     mPendingOperations = 0;
    // }
    assert(0);
}

void CdmaCallTracker::UpdatePhoneState()
{
    // ==================before translated======================
    // PhoneConstants.State oldState = mState;
    //
    // if (mRingingCall.isRinging()) {
    //     mState = PhoneConstants.State.RINGING;
    // } else if (mPendingMO != null ||
    //         !(mForegroundCall.isIdle() && mBackgroundCall.isIdle())) {
    //     mState = PhoneConstants.State.OFFHOOK;
    // } else {
    //     mState = PhoneConstants.State.IDLE;
    // }
    //
    // if (mState == PhoneConstants.State.IDLE && oldState != mState) {
    //     mVoiceCallEndedRegistrants.notifyRegistrants(
    //         new AsyncResult(null, null, null));
    // } else if (oldState == PhoneConstants.State.IDLE && oldState != mState) {
    //     mVoiceCallStartedRegistrants.notifyRegistrants (
    //             new AsyncResult(null, null, null));
    // }
    // if (Phone.DEBUG_PHONE) {
    //     log("update phone state, old=" + oldState + " new="+ mState);
    // }
    // if (mState != oldState) {
    //     mPhone.notifyPhoneStateChanged();
    // }
    assert(0);
}

void CdmaCallTracker::FlashAndSetGenericTrue()
{
    // ==================before translated======================
    // mCi.sendCDMAFeatureCode("", obtainMessage(EVENT_SWITCH_RESULT));
    //
    // // Set generic to true because in CDMA it is not known what
    // // the status of the call is after a call waiting is answered,
    // // 3 way call merged or a switch between calls.
    // mForegroundCall.setGeneric(true);
    // mPhone.notifyPreciseCallStateChanged();
    assert(0);
}

void CdmaCallTracker::HandleRadioNotAvailable()
{
    // ==================before translated======================
    // // handlePollCalls will clear out its
    // // call list when it gets the CommandException
    // // error result from this
    // pollCallsWhenSafe();
    assert(0);
}

void CdmaCallTracker::NotifyCallWaitingInfo(
    /* [in] */ ICdmaCallWaitingNotification* obj)
{
    // ==================before translated======================
    // if (mCallWaitingRegistrants != null) {
    //     mCallWaitingRegistrants.notifyRegistrants(new AsyncResult(null, obj, null));
    // }
    assert(0);
}

void CdmaCallTracker::HandleCallWaitingInfo(
    /* [in] */ ICdmaCallWaitingNotification* cw)
{
    // ==================before translated======================
    // // Check how many connections in foregroundCall.
    // // If the connection in foregroundCall is more
    // // than one, then the connection information is
    // // not reliable anymore since it means either
    // // call waiting is connected or 3 way call is
    // // dialed before, so set generic.
    // if (mForegroundCall.mConnections.size() > 1 ) {
    //     mForegroundCall.setGeneric(true);
    // }
    //
    // // Create a new CdmaConnection which attaches itself to ringingCall.
    // mRingingCall.setGeneric(false);
    // new CdmaConnection(mPhone.getContext(), cw, this, mRingingCall);
    // updatePhoneState();
    //
    // // Finally notify application
    // notifyCallWaitingInfo(cw);
    assert(0);
}

void CdmaCallTracker::HandleEcmTimer(
    /* [in] */ Int32 action)
{
    // ==================before translated======================
    // mPhone.handleTimerInEmergencyCallbackMode(action);
    // switch(action) {
    // case CDMAPhone.CANCEL_ECM_TIMER: mIsEcmTimerCanceled = true; break;
    // case CDMAPhone.RESTART_ECM_TIMER: mIsEcmTimerCanceled = false; break;
    // default:
    //     Rlog.e(LOGTAG, "handleEcmTimer, unsupported action " + action);
    // }
    assert(0);
}

void CdmaCallTracker::DisableDataCallInEmergencyCall(
    /* [in] */ const String& dialString)
{
    // ==================before translated======================
    // if (PhoneNumberUtils.isLocalEmergencyNumber(mPhone.getContext(), dialString)) {
    //     if (Phone.DEBUG_PHONE) log("disableDataCallInEmergencyCall");
    //     mIsInEmergencyCall = true;
    //     mPhone.mDcTracker.setInternalDataEnabled(false);
    // }
    assert(0);
}

void CdmaCallTracker::CheckAndEnableDataCallAfterEmergencyCallDropped()
{
    // ==================before translated======================
    // if (mIsInEmergencyCall) {
    //     mIsInEmergencyCall = false;
    //     String inEcm=SystemProperties.get(TelephonyProperties.PROPERTY_INECM_MODE, "false");
    //     if (Phone.DEBUG_PHONE) {
    //         log("checkAndEnableDataCallAfterEmergencyCallDropped,inEcm=" + inEcm);
    //     }
    //     if (inEcm.compareTo("false") == 0) {
    //         // Re-initiate data connection
    //         mPhone.mDcTracker.setInternalDataEnabled(true);
    //     }
    // }
    assert(0);
}

AutoPtr<IConnection> CdmaCallTracker::CheckMtFindNewRinging(
    /* [in] */ IDriverCall* dc,
    /* [in] */ Int32 i)
{
    // ==================before translated======================
    //
    // Connection newRinging = null;
    //
    // // it's a ringing call
    // if (mConnections[i].getCall() == mRingingCall) {
    //     newRinging = mConnections[i];
    //     if (Phone.DEBUG_PHONE) log("Notify new ring " + dc);
    // } else {
    //     // Something strange happened: a call which is neither
    //     // a ringing call nor the one we created. It could be the
    //     // call collision result from RIL
    //     Rlog.e(LOGTAG,"Phantom call appeared " + dc);
    //     // If it's a connected call, set the connect time so that
    //     // it's non-zero.  It may not be accurate, but at least
    //     // it won't appear as a Missed Call.
    //     if (dc.state != DriverCall.State.ALERTING
    //         && dc.state != DriverCall.State.DIALING) {
    //         mConnections[i].onConnectedInOrOut();
    //         if (dc.state == DriverCall.State.HOLDING) {
    //             // We've transitioned into HOLDING
    //             mConnections[i].onStartedHolding();
    //         }
    //     }
    // }
    // return newRinging;
    assert(0);
    AutoPtr<IConnection> empty;
    return empty;
}

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
