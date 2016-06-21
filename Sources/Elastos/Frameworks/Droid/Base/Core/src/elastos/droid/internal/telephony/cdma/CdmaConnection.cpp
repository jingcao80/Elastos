#include "Elastos.Droid.Internal.h"

#include "elastos/droid/internal/telephony/cdma/CdmaConnection.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {

//=====================================================================
//                      CdmaConnection::MyHandler
//=====================================================================
CdmaConnection::MyHandler::MyHandler(
    /* [in] */ ILooper* l)
{
    // ==================before translated======================
    // super(l);
}

ECode CdmaConnection::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    VALIDATE_NOT_NULL(msg);
    // ==================before translated======================
    //
    // switch (msg.what) {
    //     case EVENT_NEXT_POST_DIAL:
    //     case EVENT_DTMF_DONE:
    //     case EVENT_PAUSE_DONE:
    //         processNextPostDialChar();
    //         break;
    //     case EVENT_WAKE_LOCK_TIMEOUT:
    //         releaseWakeLock();
    //         break;
    // }
    assert(0);
    return NOERROR;
}

//=====================================================================
//                            CdmaConnection
//=====================================================================
CAR_INTERFACE_IMPL(CdmaConnection, Connection, ICdmaConnection);
const String CdmaConnection::LOGTAG("CdmaConnection");
const Int32 CdmaConnection::EVENT_DTMF_DONE;
const Int32 CdmaConnection::EVENT_PAUSE_DONE;
const Int32 CdmaConnection::EVENT_NEXT_POST_DIAL;
const Int32 CdmaConnection::EVENT_WAKE_LOCK_TIMEOUT;
const Int32 CdmaConnection::WAKE_LOCK_TIMEOUT_MILLIS;
const Int32 CdmaConnection::PAUSE_DELAY_MILLIS;
const Boolean CdmaConnection::VDBG = FALSE;

CdmaConnection::CdmaConnection()
{
}

ECode CdmaConnection::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IDriverCall* dc,
    /* [in] */ ICdmaCallTracker* ct,
    /* [in] */ Int32 index)
{
    // ==================before translated======================
    // createWakeLock(context);
    // acquireWakeLock();
    //
    // mOwner = ct;
    // mHandler = new MyHandler(mOwner.getLooper());
    //
    // mAddress = dc.number;
    //
    // mIsIncoming = dc.isMT;
    // mCreateTime = System.currentTimeMillis();
    // mCnapName = dc.name;
    // mCnapNamePresentation = dc.namePresentation;
    // mNumberPresentation = dc.numberPresentation;
    //
    // mIndex = index;
    //
    // mParent = parentFromDCState (dc.state);
    // mParent.attach(this, dc);
    return NOERROR;
}

ECode CdmaConnection::constructor(
    /* [in] */ IContext* context,
    /* [in] */ const String& dialString,
    /* [in] */ ICdmaCallTracker* ct,
    /* [in] */ ICdmaCall* parent)
{
    // ==================before translated======================
    // createWakeLock(context);
    // acquireWakeLock();
    //
    // mOwner = ct;
    // mHandler = new MyHandler(mOwner.getLooper());
    //
    // mDialString = dialString;
    // Rlog.d(LOGTAG, "[CDMAConn] CdmaConnection: dialString=" + dialString);
    // dialString = formatDialString(dialString);
    // Rlog.d(LOGTAG, "[CDMAConn] CdmaConnection:formated dialString=" + dialString);
    //
    // mAddress = PhoneNumberUtils.extractNetworkPortionAlt(dialString);
    // mPostDialString = PhoneNumberUtils.extractPostDialPortion(dialString);
    //
    // mIndex = -1;
    //
    // mIsIncoming = false;
    // mCnapName = null;
    // mCnapNamePresentation = PhoneConstants.PRESENTATION_ALLOWED;
    // mNumberPresentation = PhoneConstants.PRESENTATION_ALLOWED;
    // mCreateTime = System.currentTimeMillis();
    //
    // if (parent != null) {
    //     mParent = parent;
    //
    //     //for the three way call case, not change parent state
    //     if (parent.mState == CdmaCall.State.ACTIVE) {
    //         parent.attachFake(this, CdmaCall.State.ACTIVE);
    //     } else {
    //         parent.attachFake(this, CdmaCall.State.DIALING);
    //     }
    // }
    return NOERROR;
}

ECode CdmaConnection::constructor(
    /* [in] */ IContext* context,
    /* [in] */ ICdmaCallWaitingNotification* cw,
    /* [in] */ ICdmaCallTracker* ct,
    /* [in] */ ICdmaCall* parent)
{
    // ==================before translated======================
    // createWakeLock(context);
    // acquireWakeLock();
    //
    // mOwner = ct;
    // mHandler = new MyHandler(mOwner.getLooper());
    // mAddress = cw.number;
    // mNumberPresentation = cw.numberPresentation;
    // mCnapName = cw.name;
    // mCnapNamePresentation = cw.namePresentation;
    // mIndex = -1;
    // mIsIncoming = true;
    // mCreateTime = System.currentTimeMillis();
    // mConnectTime = 0;
    // mParent = parent;
    // parent.attachFake(this, CdmaCall.State.WAITING);
    return NOERROR;
}

ECode CdmaConnection::Dispose()
{
    assert(0);
    return NOERROR;
}

Boolean CdmaConnection::EqualsHandlesNulls(
    /* [in] */ IInterface* a,
    /* [in] */ IInterface* b)
{
    // ==================before translated======================
    // return (a == null) ? (b == null) : a.equals (b);
    assert(0);
    return FALSE;
}

ECode CdmaConnection::CompareTo(
    /* [in] */ IDriverCall* c,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // // On mobile originated (MO) calls, the phone number may have changed
    // // due to a SIM Toolkit call control modification.
    // //
    // // We assume we know when MO calls are created (since we created them)
    // // and therefore don't need to compare the phone number anyway.
    // if (! (mIsIncoming || c.isMT)) return true;
    //
    // // ... but we can compare phone numbers on MT calls, and we have
    // // no control over when they begin, so we might as well
    //
    // String cAddress = PhoneNumberUtils.stringFromStringAndTOA(c.number, c.TOA);
    // return mIsIncoming == c.isMT && equalsHandlesNulls(mAddress, cAddress);
    assert(0);
    return NOERROR;
}

ECode CdmaConnection::GetOrigDialString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // return mDialString;
    assert(0);
    return NOERROR;
}

ECode CdmaConnection::GetCall(
    /* [out] */ ICall** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // return mParent;
    assert(0);
    return NOERROR;
}

ECode CdmaConnection::GetDisconnectTime(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mDisconnectTime;
    assert(0);
    return NOERROR;
}

ECode CdmaConnection::GetHoldDurationMillis(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (getState() != CdmaCall.State.HOLDING) {
    //     // If not holding, return 0
    //     return 0;
    // } else {
    //     return SystemClock.elapsedRealtime() - mHoldingStartTime;
    // }
    assert(0);
    return NOERROR;
}

ECode CdmaConnection::GetDisconnectCause(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mCause;
    assert(0);
    return NOERROR;
}

ECode CdmaConnection::GetState(
    /* [out] */ ICallState* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (mDisconnected) {
    //     return CdmaCall.State.DISCONNECTED;
    // } else {
    //     return super.getState();
    // }
    assert(0);
    return NOERROR;
}

ECode CdmaConnection::Hangup()
{
    // ==================before translated======================
    // if (!mDisconnected) {
    //     mOwner.hangup(this);
    // } else {
    //     throw new CallStateException ("disconnected");
    // }
    assert(0);
    return NOERROR;
}

ECode CdmaConnection::Separate()
{
    // ==================before translated======================
    // if (!mDisconnected) {
    //     mOwner.separate(this);
    // } else {
    //     throw new CallStateException ("disconnected");
    // }
    assert(0);
    return NOERROR;
}

ECode CdmaConnection::GetPostDialState(
    /* [out] */ IConnectionPostDialState* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPostDialState;
    assert(0);
    return NOERROR;
}

ECode CdmaConnection::ProceedAfterWaitChar()
{
    // ==================before translated======================
    // if (mPostDialState != PostDialState.WAIT) {
    //     Rlog.w(LOGTAG, "CdmaConnection.proceedAfterWaitChar(): Expected "
    //         + "getPostDialState() to be WAIT but was " + mPostDialState);
    //     return;
    // }
    //
    // setPostDialState(PostDialState.STARTED);
    //
    // processNextPostDialChar();
    assert(0);
    return NOERROR;
}

ECode CdmaConnection::ProceedAfterWildChar(
    /* [in] */ const String& str)
{
    // ==================before translated======================
    // if (mPostDialState != PostDialState.WILD) {
    //     Rlog.w(LOGTAG, "CdmaConnection.proceedAfterWaitChar(): Expected "
    //         + "getPostDialState() to be WILD but was " + mPostDialState);
    //     return;
    // }
    //
    // setPostDialState(PostDialState.STARTED);
    //
    // // make a new postDialString, with the wild char replacement string
    // // at the beginning, followed by the remaining postDialString.
    //
    // StringBuilder buf = new StringBuilder(str);
    // buf.append(mPostDialString.substring(mNextPostDialChar));
    // mPostDialString = buf.toString();
    // mNextPostDialChar = 0;
    // if (Phone.DEBUG_PHONE) {
    //     log("proceedAfterWildChar: new postDialString is " +
    //             mPostDialString);
    // }
    //
    // processNextPostDialChar();
    assert(0);
    return NOERROR;
}

ECode CdmaConnection::CancelPostDial()
{
    // ==================before translated======================
    // setPostDialState(PostDialState.CANCELLED);
    assert(0);
    return NOERROR;
}

ECode CdmaConnection::OnHangupLocal()
{
    // ==================before translated======================
    // mCause = DisconnectCause.LOCAL;
    // mPreciseCause = 0;
    assert(0);
    return NOERROR;
}

ECode CdmaConnection::DisconnectCauseFromCode(
    /* [in] */ Int32 causeCode,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // /**
    //  * See 22.001 Annex F.4 for mapping of cause codes
    //  * to local tones
    //  */
    //
    // switch (causeCode) {
    //     case CallFailCause.USER_BUSY:
    //         return DisconnectCause.BUSY;
    //     case CallFailCause.NO_CIRCUIT_AVAIL:
    //         return DisconnectCause.CONGESTION;
    //     case CallFailCause.ACM_LIMIT_EXCEEDED:
    //         return DisconnectCause.LIMIT_EXCEEDED;
    //     case CallFailCause.CALL_BARRED:
    //         return DisconnectCause.CALL_BARRED;
    //     case CallFailCause.FDN_BLOCKED:
    //         return DisconnectCause.FDN_BLOCKED;
    //     case CallFailCause.DIAL_MODIFIED_TO_USSD:
    //         return DisconnectCause.DIAL_MODIFIED_TO_USSD;
    //     case CallFailCause.DIAL_MODIFIED_TO_SS:
    //         return DisconnectCause.DIAL_MODIFIED_TO_SS;
    //     case CallFailCause.DIAL_MODIFIED_TO_DIAL:
    //         return DisconnectCause.DIAL_MODIFIED_TO_DIAL;
    //     case CallFailCause.CDMA_LOCKED_UNTIL_POWER_CYCLE:
    //         return DisconnectCause.CDMA_LOCKED_UNTIL_POWER_CYCLE;
    //     case CallFailCause.CDMA_DROP:
    //         return DisconnectCause.CDMA_DROP;
    //     case CallFailCause.CDMA_INTERCEPT:
    //         return DisconnectCause.CDMA_INTERCEPT;
    //     case CallFailCause.CDMA_REORDER:
    //         return DisconnectCause.CDMA_REORDER;
    //     case CallFailCause.CDMA_SO_REJECT:
    //         return DisconnectCause.CDMA_SO_REJECT;
    //     case CallFailCause.CDMA_RETRY_ORDER:
    //         return DisconnectCause.CDMA_RETRY_ORDER;
    //     case CallFailCause.CDMA_ACCESS_FAILURE:
    //         return DisconnectCause.CDMA_ACCESS_FAILURE;
    //     case CallFailCause.CDMA_PREEMPTED:
    //         return DisconnectCause.CDMA_PREEMPTED;
    //     case CallFailCause.CDMA_NOT_EMERGENCY:
    //         return DisconnectCause.CDMA_NOT_EMERGENCY;
    //     case CallFailCause.CDMA_ACCESS_BLOCKED:
    //         return DisconnectCause.CDMA_ACCESS_BLOCKED;
    //     case CallFailCause.ERROR_UNSPECIFIED:
    //     case CallFailCause.NORMAL_CLEARING:
    //     default:
    //         CDMAPhone phone = mOwner.mPhone;
    //         int serviceState = phone.getServiceState().getState();
    //         UiccCardApplication app = UiccController
    //                 .getInstance()
    //                 .getUiccCardApplication(UiccController.APP_FAM_3GPP2);
    //         AppState uiccAppState = (app != null) ? app.getState() : AppState.APPSTATE_UNKNOWN;
    //         if (serviceState == ServiceState.STATE_POWER_OFF) {
    //             return DisconnectCause.POWER_OFF;
    //         } else if (serviceState == ServiceState.STATE_OUT_OF_SERVICE
    //                 || serviceState == ServiceState.STATE_EMERGENCY_ONLY) {
    //             return DisconnectCause.OUT_OF_SERVICE;
    //         } else if (phone.mCdmaSubscriptionSource ==
    //                 CdmaSubscriptionSourceManager.SUBSCRIPTION_FROM_RUIM
    //                 && uiccAppState != AppState.APPSTATE_READY) {
    //             return DisconnectCause.ICC_ERROR;
    //         } else if (causeCode==CallFailCause.NORMAL_CLEARING) {
    //             return DisconnectCause.NORMAL;
    //         } else {
    //             return DisconnectCause.ERROR_UNSPECIFIED;
    //         }
    // }
    assert(0);
    return NOERROR;
}

ECode CdmaConnection::OnRemoteDisconnect(
    /* [in] */ Int32 causeCode)
{
    // ==================before translated======================
    // this.mPreciseCause = causeCode;
    // onDisconnect(disconnectCauseFromCode(causeCode));
    assert(0);
    return NOERROR;
}

ECode CdmaConnection::OnDisconnect(
    /* [in] */ Int32 cause,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // boolean changed = false;
    //
    // mCause = cause;
    //
    // if (!mDisconnected) {
    //     doDisconnect();
    //     if (VDBG) Rlog.d(LOGTAG, "onDisconnect: cause=" + cause);
    //
    //     mOwner.mPhone.notifyDisconnect(this);
    //
    //     if (mParent != null) {
    //         changed = mParent.connectionDisconnected(this);
    //     }
    // }
    // releaseWakeLock();
    // return changed;
    assert(0);
    return NOERROR;
}

ECode CdmaConnection::OnLocalDisconnect()
{
    // ==================before translated======================
    // if (!mDisconnected) {
    //     doDisconnect();
    //     if (VDBG) Rlog.d(LOGTAG, "onLoalDisconnect" );
    //
    //     if (mParent != null) {
    //         mParent.detach(this);
    //     }
    // }
    // releaseWakeLock();
    assert(0);
    return NOERROR;
}

ECode CdmaConnection::Update(
    /* [in] */ IDriverCall* dc,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // CdmaCall newParent;
    // boolean changed = false;
    // boolean wasConnectingInOrOut = isConnectingInOrOut();
    // boolean wasHolding = (getState() == CdmaCall.State.HOLDING);
    //
    // newParent = parentFromDCState(dc.state);
    //
    // if (Phone.DEBUG_PHONE) log("parent= " +mParent +", newParent= " + newParent);
    //
    // log(" mNumberConverted " + mNumberConverted);
    // if (!equalsHandlesNulls(mAddress, dc.number) && (!mNumberConverted
    //         || !equalsHandlesNulls(mConvertedNumber, dc.number)))  {
    //     if (Phone.DEBUG_PHONE) log("update: phone # changed!");
    //     mAddress = dc.number;
    //     changed = true;
    // }
    //
    // // A null cnapName should be the same as ""
    // if (TextUtils.isEmpty(dc.name)) {
    //     if (!TextUtils.isEmpty(mCnapName)) {
    //         changed = true;
    //         mCnapName = "";
    //     }
    // } else if (!dc.name.equals(mCnapName)) {
    //     changed = true;
    //     mCnapName = dc.name;
    // }
    //
    // if (Phone.DEBUG_PHONE) log("--dssds----"+mCnapName);
    // mCnapNamePresentation = dc.namePresentation;
    // mNumberPresentation = dc.numberPresentation;
    //
    // if (newParent != mParent) {
    //     if (mParent != null) {
    //         mParent.detach(this);
    //     }
    //     newParent.attach(this, dc);
    //     mParent = newParent;
    //     changed = true;
    // } else {
    //     boolean parentStateChange;
    //     parentStateChange = mParent.update (this, dc);
    //     changed = changed || parentStateChange;
    // }
    //
    // /** Some state-transition events */
    //
    // if (Phone.DEBUG_PHONE) log(
    //         "Update, wasConnectingInOrOut=" + wasConnectingInOrOut +
    //         ", wasHolding=" + wasHolding +
    //         ", isConnectingInOrOut=" + isConnectingInOrOut() +
    //         ", changed=" + changed);
    //
    //
    // if (wasConnectingInOrOut && !isConnectingInOrOut()) {
    //     onConnectedInOrOut();
    // }
    //
    // if (changed && !wasHolding && (getState() == CdmaCall.State.HOLDING)) {
    //     // We've transitioned into HOLDING
    //     onStartedHolding();
    // }
    //
    // return changed;
    assert(0);
    return NOERROR;
}

ECode CdmaConnection::FakeHoldBeforeDial()
{
    // ==================before translated======================
    // if (mParent != null) {
    //     mParent.detach(this);
    // }
    //
    // mParent = mOwner.mBackgroundCall;
    // mParent.attachFake(this, CdmaCall.State.HOLDING);
    //
    // onStartedHolding();
    assert(0);
    return NOERROR;
}

ECode CdmaConnection::GetCDMAIndex(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (mIndex >= 0) {
    //     return mIndex + 1;
    // } else {
    //     throw new CallStateException ("CDMA connection index not assigned");
    // }
    assert(0);
    return NOERROR;
}

ECode CdmaConnection::OnConnectedInOrOut()
{
    // ==================before translated======================
    // mConnectTime = System.currentTimeMillis();
    // mConnectTimeReal = SystemClock.elapsedRealtime();
    // mDuration = 0;
    //
    // // bug #678474: incoming call interpreted as missed call, even though
    // // it sounds like the user has picked up the call.
    // if (Phone.DEBUG_PHONE) {
    //     log("onConnectedInOrOut: connectTime=" + mConnectTime);
    // }
    //
    // if (!mIsIncoming) {
    //     // outgoing calls only
    //     processNextPostDialChar();
    // } else {
    //     // Only release wake lock for incoming calls, for outgoing calls the wake lock
    //     // will be released after any pause-dial is completed
    //     releaseWakeLock();
    // }
    assert(0);
    return NOERROR;
}

ECode CdmaConnection::OnStartedHolding()
{
    // ==================before translated======================
    // mHoldingStartTime = SystemClock.elapsedRealtime();
    assert(0);
    return NOERROR;
}

ECode CdmaConnection::GetRemainingPostDialString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // if (mPostDialState == PostDialState.CANCELLED
    //         || mPostDialState == PostDialState.COMPLETE
    //         || mPostDialString == null
    //         || mPostDialString.length() <= mNextPostDialChar) {
    //     return "";
    // }
    //
    // String subStr = mPostDialString.substring(mNextPostDialChar);
    // if (subStr != null) {
    //     int wIndex = subStr.indexOf(PhoneNumberUtils.WAIT);
    //     int pIndex = subStr.indexOf(PhoneNumberUtils.PAUSE);
    //
    //     if (wIndex > 0 && (wIndex < pIndex || pIndex <= 0)) {
    //         subStr = subStr.substring(0, wIndex);
    //     } else if (pIndex > 0) {
    //         subStr = subStr.substring(0, pIndex);
    //     }
    // }
    // return subStr;
    assert(0);
    return NOERROR;
}

ECode CdmaConnection::UpdateParent(
    /* [in] */ ICdmaCall* oldParent,
    /* [in] */ ICdmaCall* newParent)
{
    // ==================before translated======================
    // if (newParent != oldParent) {
    //     if (oldParent != null) {
    //         oldParent.detach(this);
    //     }
    //     newParent.attachFake(this, CdmaCall.State.ACTIVE);
    //     mParent = newParent;
    // }
    assert(0);
    return NOERROR;
}

ECode CdmaConnection::ProcessNextPostDialChar()
{
    // ==================before translated======================
    // char c = 0;
    // Registrant postDialHandler;
    //
    // if (mPostDialState == PostDialState.CANCELLED) {
    //     releaseWakeLock();
    //     //Rlog.v("CDMA", "##### processNextPostDialChar: postDialState == CANCELLED, bail");
    //     return;
    // }
    //
    // if (mPostDialString == null ||
    //         mPostDialString.length() <= mNextPostDialChar) {
    //     setPostDialState(PostDialState.COMPLETE);
    //
    //     // We were holding a wake lock until pause-dial was complete, so give it up now
    //     releaseWakeLock();
    //
    //     // notifyMessage.arg1 is 0 on complete
    //     c = 0;
    // } else {
    //     boolean isValid;
    //
    //     setPostDialState(PostDialState.STARTED);
    //
    //     c = mPostDialString.charAt(mNextPostDialChar++);
    //
    //     isValid = processPostDialChar(c);
    //
    //     if (!isValid) {
    //         // Will call processNextPostDialChar
    //         mHandler.obtainMessage(EVENT_NEXT_POST_DIAL).sendToTarget();
    //         // Don't notify application
    //         Rlog.e("CDMA", "processNextPostDialChar: c=" + c + " isn't valid!");
    //         return;
    //     }
    // }
    //
    // postDialHandler = mOwner.mPhone.mPostDialHandler;
    //
    // Message notifyMessage;
    //
    // if (postDialHandler != null &&
    //         (notifyMessage = postDialHandler.messageForRegistrant()) != null) {
    //     // The AsyncResult.result is the Connection object
    //     PostDialState state = mPostDialState;
    //     AsyncResult ar = AsyncResult.forMessage(notifyMessage);
    //     ar.result = this;
    //     ar.userObj = state;
    //
    //     // arg1 is the character that was/is being processed
    //     notifyMessage.arg1 = c;
    //
    //     notifyMessage.sendToTarget();
    // }
    assert(0);
    return NOERROR;
}

String CdmaConnection::FormatDialString(
    /* [in] */ const String& phoneNumber)
{
    // ==================before translated======================
    // /**
    //  * TODO(cleanup): This function should move to PhoneNumberUtils, and
    //  * tests should be added.
    //  */
    //
    // if (phoneNumber == null) {
    //     return null;
    // }
    // int length = phoneNumber.length();
    // StringBuilder ret = new StringBuilder();
    // char c;
    // int currIndex = 0;
    //
    // while (currIndex < length) {
    //     c = phoneNumber.charAt(currIndex);
    //     if (isPause(c) || isWait(c)) {
    //         if (currIndex < length - 1) {
    //             // if PW not at the end
    //             int nextIndex = findNextPCharOrNonPOrNonWCharIndex(phoneNumber, currIndex);
    //             // If there is non PW char following PW sequence
    //             if (nextIndex < length) {
    //                 char pC = findPOrWCharToAppend(phoneNumber, currIndex, nextIndex);
    //                 ret.append(pC);
    //                 // If PW char sequence has more than 2 PW characters,
    //                 // skip to the last PW character since the sequence already be
    //                 // converted to WAIT character
    //                 if (nextIndex > (currIndex + 1)) {
    //                     currIndex = nextIndex - 1;
    //                 }
    //             } else if (nextIndex == length) {
    //                 // It means PW characters at the end, ignore
    //                 currIndex = length - 1;
    //             }
    //         }
    //     } else {
    //         ret.append(c);
    //     }
    //     currIndex++;
    // }
    // return PhoneNumberUtils.cdmaCheckAndProcessPlusCode(ret.toString());
    assert(0);
    return String("");
}

ECode CdmaConnection::GetNumberPresentation(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mNumberPresentation;
    assert(0);
    return NOERROR;
}

ECode CdmaConnection::GetUUSInfo(
    /* [out] */ IUUSInfo** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // // UUS information not supported in CDMA
    // return null;
    assert(0);
    return NOERROR;
}

ECode CdmaConnection::GetPreciseDisconnectCause(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPreciseCause;
    assert(0);
    return NOERROR;
}

ECode CdmaConnection::GetOrigConnection(
    /* [out] */ IConnection** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // return null;
    assert(0);
    return NOERROR;
}

ECode CdmaConnection::IsMultiparty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    // ==================before translated======================
    // return false;
    assert(0);
    return NOERROR;
}

void CdmaConnection::Finalize()
{
    // ==================before translated======================
    // /**
    //  * It is understood that This finializer is not guaranteed
    //  * to be called and the release lock call is here just in
    //  * case there is some path that doesn't call onDisconnect
    //  * and or onConnectedInOrOut.
    //  */
    // if (mPartialWakeLock.isHeld()) {
    //     Rlog.e(LOGTAG, "[CdmaConn] UNEXPECTED; mPartialWakeLock is held when finalizing.");
    // }
    // releaseWakeLock();
    assert(0);
}

void CdmaConnection::DoDisconnect()
{
    // ==================before translated======================
    // mIndex = -1;
    // mDisconnectTime = System.currentTimeMillis();
    // mDuration = SystemClock.elapsedRealtime() - mConnectTimeReal;
    // mDisconnected = true;
    // clearPostDialListeners();
    assert(0);
}

Boolean CdmaConnection::ProcessPostDialChar(
    /* [in] */ Byte c)
{
    // ==================before translated======================
    // if (PhoneNumberUtils.is12Key(c)) {
    //     mOwner.mCi.sendDtmf(c, mHandler.obtainMessage(EVENT_DTMF_DONE));
    // } else if (c == PhoneNumberUtils.PAUSE) {
    //     setPostDialState(PostDialState.PAUSE);
    //
    //     // Upon occurrences of the separator, the UE shall
    //     // pause again for 2 seconds before sending any
    //     // further DTMF digits.
    //     mHandler.sendMessageDelayed(mHandler.obtainMessage(EVENT_PAUSE_DONE),
    //                                     PAUSE_DELAY_MILLIS);
    // } else if (c == PhoneNumberUtils.WAIT) {
    //     setPostDialState(PostDialState.WAIT);
    // } else if (c == PhoneNumberUtils.WILD) {
    //     setPostDialState(PostDialState.WILD);
    // } else {
    //     return false;
    // }
    //
    // return true;
    assert(0);
    return FALSE;
}

Boolean CdmaConnection::IsConnectingInOrOut()
{
    // ==================before translated======================
    // return mParent == null || mParent == mOwner.mRingingCall
    //     || mParent.mState == CdmaCall.State.DIALING
    //     || mParent.mState == CdmaCall.State.ALERTING;
    assert(0);
    return FALSE;
}

AutoPtr<ICdmaCall> CdmaConnection::ParentFromDCState(
    /* [in] */  IDriverCallState state)
{
    // ==================before translated======================
    // switch (state) {
    //     case ACTIVE:
    //     case DIALING:
    //     case ALERTING:
    //         return mOwner.mForegroundCall;
    //     //break;
    //
    //     case HOLDING:
    //         return mOwner.mBackgroundCall;
    //     //break;
    //
    //     case INCOMING:
    //     case WAITING:
    //         return mOwner.mRingingCall;
    //     //break;
    //
    //     default:
    //         throw new RuntimeException("illegal call state: " + state);
    // }
    assert(0);
    AutoPtr<ICdmaCall> empty;
    return empty;
}

void CdmaConnection::SetPostDialState(
    /* [in] */ IConnectionPostDialState s)
{
    // ==================before translated======================
    // if (s == PostDialState.STARTED ||
    //         s == PostDialState.PAUSE) {
    //     synchronized (mPartialWakeLock) {
    //         if (mPartialWakeLock.isHeld()) {
    //             mHandler.removeMessages(EVENT_WAKE_LOCK_TIMEOUT);
    //         } else {
    //             acquireWakeLock();
    //         }
    //         Message msg = mHandler.obtainMessage(EVENT_WAKE_LOCK_TIMEOUT);
    //         mHandler.sendMessageDelayed(msg, WAKE_LOCK_TIMEOUT_MILLIS);
    //     }
    // } else {
    //     mHandler.removeMessages(EVENT_WAKE_LOCK_TIMEOUT);
    //     releaseWakeLock();
    // }
    // mPostDialState = s;
    // notifyPostDialListeners();
    assert(0);
}

void CdmaConnection::CreateWakeLock(
    /* [in] */ IContext* context)
{
    // ==================before translated======================
    // PowerManager pm = (PowerManager)context.getSystemService(Context.POWER_SERVICE);
    // mPartialWakeLock = pm.newWakeLock(PowerManager.PARTIAL_WAKE_LOCK, LOGTAG);
    assert(0);
}

void CdmaConnection::AcquireWakeLock()
{
    // ==================before translated======================
    // log("acquireWakeLock");
    // mPartialWakeLock.acquire();
    assert(0);
}

void CdmaConnection::ReleaseWakeLock()
{
    // ==================before translated======================
    // synchronized (mPartialWakeLock) {
    //     if (mPartialWakeLock.isHeld()) {
    //         log("releaseWakeLock");
    //         mPartialWakeLock.release();
    //     }
    // }
    assert(0);
}

Boolean CdmaConnection::IsPause(
    /* [in] */ Byte c)
{
    // ==================before translated======================
    // return c == PhoneNumberUtils.PAUSE;
    assert(0);
    return FALSE;
}

Boolean CdmaConnection::IsWait(
    /* [in] */ Byte c)
{
    // ==================before translated======================
    // return c == PhoneNumberUtils.WAIT;
    assert(0);
    return FALSE;
}

Int32 CdmaConnection::FindNextPCharOrNonPOrNonWCharIndex(
    /* [in] */ const String& phoneNumber,
    /* [in] */ Int32 currIndex)
{
    // ==================before translated======================
    // boolean wMatched = isWait(phoneNumber.charAt(currIndex));
    // int index = currIndex + 1;
    // int length = phoneNumber.length();
    // while (index < length) {
    //     char cNext = phoneNumber.charAt(index);
    //     // if there is any W inside P/W sequence,mark it
    //     if (isWait(cNext)) {
    //         wMatched = true;
    //     }
    //     // if any characters other than P/W chars after P/W sequence
    //     // we break out the loop and append the correct
    //     if (!isWait(cNext) && !isPause(cNext)) {
    //         break;
    //     }
    //     index++;
    // }
    //
    // // It means the PAUSE character(s) is in the middle of dial string
    // // and it needs to be handled one by one.
    // if ((index < length) && (index > (currIndex + 1))  &&
    //     ((wMatched == false) && isPause(phoneNumber.charAt(currIndex)))) {
    //     return (currIndex + 1);
    // }
    // return index;
    assert(0);
    return 0;
}

Byte CdmaConnection::FindPOrWCharToAppend(
    /* [in] */ const String& phoneNumber,
    /* [in] */ Int32 currPwIndex,
    /* [in] */ Int32 nextNonPwCharIndex)
{
    // ==================before translated======================
    // char c = phoneNumber.charAt(currPwIndex);
    // char ret;
    //
    // // Append the PW char
    // ret = (isPause(c)) ? PhoneNumberUtils.PAUSE : PhoneNumberUtils.WAIT;
    //
    // // If the nextNonPwCharIndex is greater than currPwIndex + 1,
    // // it means the PW sequence contains not only P characters.
    // // Since for the sequence that only contains P character,
    // // the P character is handled one by one, the nextNonPwCharIndex
    // // equals to currPwIndex + 1.
    // // In this case, skip P, append W.
    // if (nextNonPwCharIndex > (currPwIndex + 1)) {
    //     ret = PhoneNumberUtils.WAIT;
    // }
    // return ret;
    assert(0);
    return 0;
}

void CdmaConnection::Log(
    /* [in] */ const String& msg)
{
    // ==================before translated======================
    // Rlog.d(LOGTAG, "[CDMAConn] " + msg);
    assert(0);
}

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
