/*
 * Copyright (C) 2006 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.internal.telephony.cdma;

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Internal::Telephony::I*;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::IRegistrant;
using Elastos::Droid::Os::ISystemClock;
using Elastos::Droid::Telephony::IDisconnectCause;
using Elastos::Droid::Telephony::IRlog;
using Elastos::Droid::Text::ITextUtils;

using Elastos::Droid::Telephony::IPhoneNumberUtils;
using Elastos::Droid::Telephony::IServiceState;

using Elastos::Droid::Internal::Telephony::Uicc::IUiccCardApplication;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccController;
using Elastos::Droid::Internal::Telephony::Uicc::IccCardApplicationStatus::IAppState;

/**
 * {@hide}
 */
public class CdmaConnection extends Connection {
    static const String LOG_TAG = "CdmaConnection";
    private static const Boolean VDBG = FALSE;

    //***** Instance Variables

    CdmaCallTracker mOwner;
    CdmaCall mParent;


    String mPostDialString;      // outgoing calls only
    Boolean mDisconnected;
    Int32 mIndex;          // index in CdmaCallTracker.connections[], -1 if unassigned

    /*
     * These time/timespan values are based on System->CurrentTimeMillis(),
     * i.e., "wall clock" time.
     */
    Int64 mDisconnectTime;

    Int32 mNextPostDialChar;       // index into postDialString

    Int32 mCause = DisconnectCause.NOT_DISCONNECTED;
    PostDialState mPostDialState = PostDialState.NOT_STARTED;
    Int32 mPreciseCause = 0;

    Handler mHandler;

    private PowerManager.WakeLock mPartialWakeLock;

    //***** Event Constants
    static const Int32 EVENT_DTMF_DONE = 1;
    static const Int32 EVENT_PAUSE_DONE = 2;
    static const Int32 EVENT_NEXT_POST_DIAL = 3;
    static const Int32 EVENT_WAKE_LOCK_TIMEOUT = 4;

    //***** Constants
    static const Int32 WAKE_LOCK_TIMEOUT_MILLIS = 60*1000;
    static const Int32 PAUSE_DELAY_MILLIS = 2 * 1000;

    //***** Inner Classes

    class MyHandler extends Handler {
        MyHandler(Looper l) {Super(l);}

        //@Override
        CARAPI
        HandleMessage(Message msg) {

            Switch (msg.what) {
                case EVENT_NEXT_POST_DIAL:
                case EVENT_DTMF_DONE:
                case EVENT_PAUSE_DONE:
                    ProcessNextPostDialChar();
                    break;
                case EVENT_WAKE_LOCK_TIMEOUT:
                    ReleaseWakeLock();
                    break;
            }
        }
    }

    //***** Constructors

    /** This is probably an MT call that we first saw in a CLCC response */
    /*package*/
    CdmaConnection (Context context, DriverCall dc, CdmaCallTracker ct, Int32 index) {
        CreateWakeLock(context);
        AcquireWakeLock();

        mOwner = ct;
        mHandler = new MyHandler(mOwner->GetLooper());

        mAddress = dc.number;

        mIsIncoming = dc.isMT;
        mCreateTime = System->CurrentTimeMillis();
        mCnapName = dc.name;
        mCnapNamePresentation = dc.namePresentation;
        mNumberPresentation = dc.numberPresentation;

        mIndex = index;

        mParent = ParentFromDCState (dc.state);
        mParent->Attach(this, dc);
    }

    /** This is an MO call/three way call, created when dialing */
    /*package*/
    CdmaConnection(Context context, String dialString, CdmaCallTracker ct, CdmaCall parent) {
        CreateWakeLock(context);
        AcquireWakeLock();

        mOwner = ct;
        mHandler = new MyHandler(mOwner->GetLooper());

        mDialString = dialString;
        Rlog->D(LOG_TAG, "[CDMAConn] CdmaConnection: dialString=" + dialString);
        dialString = FormatDialString(dialString);
        Rlog->D(LOG_TAG, "[CDMAConn] CdmaConnection:formated dialString=" + dialString);

        mAddress = PhoneNumberUtils->ExtractNetworkPortionAlt(dialString);
        mPostDialString = PhoneNumberUtils->ExtractPostDialPortion(dialString);

        mIndex = -1;

        mIsIncoming = FALSE;
        mCnapName = NULL;
        mCnapNamePresentation = PhoneConstants.PRESENTATION_ALLOWED;
        mNumberPresentation = PhoneConstants.PRESENTATION_ALLOWED;
        mCreateTime = System->CurrentTimeMillis();

        If (parent != NULL) {
            mParent = parent;

            //for the three way call case, not change parent state
            If (parent.mState == CdmaCall.State.ACTIVE) {
                parent->AttachFake(this, CdmaCall.State.ACTIVE);
            } else {
                parent->AttachFake(this, CdmaCall.State.DIALING);
            }
        }
    }

    /** This is a Call waiting call*/
    CdmaConnection(Context context, CdmaCallWaitingNotification cw, CdmaCallTracker ct,
            CdmaCall parent) {
        CreateWakeLock(context);
        AcquireWakeLock();

        mOwner = ct;
        mHandler = new MyHandler(mOwner->GetLooper());
        mAddress = cw.number;
        mNumberPresentation = cw.numberPresentation;
        mCnapName = cw.name;
        mCnapNamePresentation = cw.namePresentation;
        mIndex = -1;
        mIsIncoming = TRUE;
        mCreateTime = System->CurrentTimeMillis();
        mConnectTime = 0;
        mParent = parent;
        parent->AttachFake(this, CdmaCall.State.WAITING);
    }

    CARAPI Dispose() {
    }

    static Boolean
    EqualsHandlesNulls (Object a, Object b) {
        Return (a == NULL) ? (b == NULL) : a.Equals (b);
    }

    /*package*/ Boolean
    CompareTo(DriverCall c) {
        // On mobile Originated (MO) calls, the phone number may have changed
        // due to a SIM Toolkit call control modification.
        //
        // We assume we know when MO calls are Created (since we created them)
        // and therefore don't need to compare the phone number anyway.
        If (! (mIsIncoming || c.isMT)) return TRUE;

        // ... but we can compare phone numbers on MT calls, and we have
        // no control over when they begin, so we might as well

        String cAddress = PhoneNumberUtils->StringFromStringAndTOA(c.number, c.TOA);
        return mIsIncoming == c.isMT && EqualsHandlesNulls(mAddress, cAddress);
    }


    //@Override
    public String GetOrigDialString(){
        return mDialString;
    }

    //@Override
    public CdmaCall GetCall() {
        return mParent;
    }

    //@Override
    public Int64 GetDisconnectTime() {
        return mDisconnectTime;
    }

    //@Override
    public Int64 GetHoldDurationMillis() {
        If (GetState() != CdmaCall.State.HOLDING) {
            // If not holding, return 0
            return 0;
        } else {
            return SystemClock->ElapsedRealtime() - mHoldingStartTime;
        }
    }

    //@Override
    public Int32 GetDisconnectCause() {
        return mCause;
    }

    //@Override
    public CdmaCall.State GetState() {
        If (mDisconnected) {
            return CdmaCall.State.DISCONNECTED;
        } else {
            return super->GetState();
        }
    }

    //@Override
    CARAPI Hangup() throws CallStateException {
        If (!mDisconnected) {
            mOwner->Hangup(this);
        } else {
            throw new CallStateException ("disconnected");
        }
    }

    //@Override
    CARAPI Separate() throws CallStateException {
        If (!mDisconnected) {
            mOwner->Separate(this);
        } else {
            throw new CallStateException ("disconnected");
        }
    }

    //@Override
    public PostDialState GetPostDialState() {
        return mPostDialState;
    }

    //@Override
    CARAPI ProceedAfterWaitChar() {
        If (mPostDialState != PostDialState.WAIT) {
            Rlog->W(LOG_TAG, "CdmaConnection->ProceedAfterWaitChar(): Expected "
                + "GetPostDialState() to be WAIT but was " + mPostDialState);
            return;
        }

        SetPostDialState(PostDialState.STARTED);

        ProcessNextPostDialChar();
    }

    //@Override
    CARAPI ProceedAfterWildChar(String str) {
        If (mPostDialState != PostDialState.WILD) {
            Rlog->W(LOG_TAG, "CdmaConnection->ProceedAfterWaitChar(): Expected "
                + "GetPostDialState() to be WILD but was " + mPostDialState);
            return;
        }

        SetPostDialState(PostDialState.STARTED);

        // make a new postDialString, with the wild Char32 replacement string
        // at the beginning, followed by the remaining postDialString.

        StringBuilder buf = new StringBuilder(str);
        buf->Append(mPostDialString->Substring(mNextPostDialChar));
        mPostDialString = buf->ToString();
        mNextPostDialChar = 0;
        If (Phone.DEBUG_PHONE) {
            Log("proceedAfterWildChar: new postDialString is " +
                    mPostDialString);
        }

        ProcessNextPostDialChar();
    }

    //@Override
    CARAPI CancelPostDial() {
        SetPostDialState(PostDialState.CANCELLED);
    }

    /**
     * Called when this Connection is being hung up Locally (eg, user pressed "end")
     * Note that at this point, the hangup request has been dispatched to the radio
     * but no response has yet been received so Update() has not yet been called
     */
    void
    OnHangupLocal() {
        mCause = DisconnectCause.LOCAL;
        mPreciseCause = 0;
    }

    /**
     * Maps RIL call disconnect code to {@link DisconnectCause}.
     * @param causeCode RIL disconnect code
     * @return the corresponding value from {@link DisconnectCause}
     */
    Int32 DisconnectCauseFromCode(Int32 causeCode) {
        /**
         * See 22.001 Annex F.4 for mapping of cause codes
         * to local tones
         */

        Switch (causeCode) {
            case CallFailCause.USER_BUSY:
                return DisconnectCause.BUSY;
            case CallFailCause.NO_CIRCUIT_AVAIL:
                return DisconnectCause.CONGESTION;
            case CallFailCause.ACM_LIMIT_EXCEEDED:
                return DisconnectCause.LIMIT_EXCEEDED;
            case CallFailCause.CALL_BARRED:
                return DisconnectCause.CALL_BARRED;
            case CallFailCause.FDN_BLOCKED:
                return DisconnectCause.FDN_BLOCKED;
            case CallFailCause.DIAL_MODIFIED_TO_USSD:
                return DisconnectCause.DIAL_MODIFIED_TO_USSD;
            case CallFailCause.DIAL_MODIFIED_TO_SS:
                return DisconnectCause.DIAL_MODIFIED_TO_SS;
            case CallFailCause.DIAL_MODIFIED_TO_DIAL:
                return DisconnectCause.DIAL_MODIFIED_TO_DIAL;
            case CallFailCause.CDMA_LOCKED_UNTIL_POWER_CYCLE:
                return DisconnectCause.CDMA_LOCKED_UNTIL_POWER_CYCLE;
            case CallFailCause.CDMA_DROP:
                return DisconnectCause.CDMA_DROP;
            case CallFailCause.CDMA_INTERCEPT:
                return DisconnectCause.CDMA_INTERCEPT;
            case CallFailCause.CDMA_REORDER:
                return DisconnectCause.CDMA_REORDER;
            case CallFailCause.CDMA_SO_REJECT:
                return DisconnectCause.CDMA_SO_REJECT;
            case CallFailCause.CDMA_RETRY_ORDER:
                return DisconnectCause.CDMA_RETRY_ORDER;
            case CallFailCause.CDMA_ACCESS_FAILURE:
                return DisconnectCause.CDMA_ACCESS_FAILURE;
            case CallFailCause.CDMA_PREEMPTED:
                return DisconnectCause.CDMA_PREEMPTED;
            case CallFailCause.CDMA_NOT_EMERGENCY:
                return DisconnectCause.CDMA_NOT_EMERGENCY;
            case CallFailCause.CDMA_ACCESS_BLOCKED:
                return DisconnectCause.CDMA_ACCESS_BLOCKED;
            case CallFailCause.ERROR_UNSPECIFIED:
            case CallFailCause.NORMAL_CLEARING:
            default:
                CDMAPhone phone = mOwner.mPhone;
                Int32 serviceState = phone->GetServiceState()->GetState();
                UiccCardApplication app = UiccController
                        .GetInstance()
                        .GetUiccCardApplication(UiccController.APP_FAM_3GPP2);
                AppState uiccAppState = (app != NULL) ? app->GetState() : AppState.APPSTATE_UNKNOWN;
                If (serviceState == ServiceState.STATE_POWER_OFF) {
                    return DisconnectCause.POWER_OFF;
                } else If (serviceState == ServiceState.STATE_OUT_OF_SERVICE
                        || serviceState == ServiceState.STATE_EMERGENCY_ONLY) {
                    return DisconnectCause.OUT_OF_SERVICE;
                } else If (phone.mCdmaSubscriptionSource ==
                        CdmaSubscriptionSourceManager.SUBSCRIPTION_FROM_RUIM
                        && uiccAppState != AppState.APPSTATE_READY) {
                    return DisconnectCause.ICC_ERROR;
                } else If (causeCode==CallFailCause.NORMAL_CLEARING) {
                    return DisconnectCause.NORMAL;
                } else {
                    return DisconnectCause.ERROR_UNSPECIFIED;
                }
        }
    }

    /*package*/ void
    OnRemoteDisconnect(Int32 causeCode) {
        this.mPreciseCause = causeCode;
        OnDisconnect(DisconnectCauseFromCode(causeCode));
    }

    /**
     * Called when the radio indicates the connection has been disconnected.
     * @param cause call disconnect cause; values are defined in {@link DisconnectCause}
     */
    /*package*/ Boolean
    OnDisconnect(Int32 cause) {
        Boolean changed = FALSE;

        mCause = cause;

        If (!mDisconnected) {
            DoDisconnect();
            If (VDBG) Rlog->D(LOG_TAG, "onDisconnect: cause=" + cause);

            mOwner.mPhone->NotifyDisconnect(this);

            If (mParent != NULL) {
                changed = mParent->ConnectionDisconnected(this);
            }
        }
        ReleaseWakeLock();
        return changed;
    }

    /** Called when the call waiting connection has been hung up */
    /*package*/ void
    OnLocalDisconnect() {
        If (!mDisconnected) {
            DoDisconnect();
            If (VDBG) Rlog->D(LOG_TAG, "onLoalDisconnect" );

            If (mParent != NULL) {
                mParent->Detach(this);
            }
        }
        ReleaseWakeLock();
    }

    // Returns TRUE if state has changed, FALSE if nothing changed
    /*package*/ Boolean
    Update (DriverCall dc) {
        CdmaCall newParent;
        Boolean changed = FALSE;
        Boolean wasConnectingInOrOut = IsConnectingInOrOut();
        Boolean wasHolding = (GetState() == CdmaCall.State.HOLDING);

        newParent = ParentFromDCState(dc.state);

        If (Phone.DEBUG_PHONE) Log("parent= " +mParent +", newParent= " + newParent);

        Log(" mNumberConverted " + mNumberConverted);
        If (!EqualsHandlesNulls(mAddress, dc.number) && (!mNumberConverted
                || !EqualsHandlesNulls(mConvertedNumber, dc.number)))  {
            If (Phone.DEBUG_PHONE) Log("update: phone # changed!");
            mAddress = dc.number;
            changed = TRUE;
        }

        // A NULL cnapName should be the same as ""
        If (TextUtils->IsEmpty(dc.name)) {
            If (!TextUtils->IsEmpty(mCnapName)) {
                changed = TRUE;
                mCnapName = "";
            }
        } else If (!dc.name->Equals(mCnapName)) {
            changed = TRUE;
            mCnapName = dc.name;
        }

        If (Phone.DEBUG_PHONE) Log("--dssds----"+mCnapName);
        mCnapNamePresentation = dc.namePresentation;
        mNumberPresentation = dc.numberPresentation;

        If (newParent != mParent) {
            If (mParent != NULL) {
                mParent->Detach(this);
            }
            newParent->Attach(this, dc);
            mParent = newParent;
            changed = TRUE;
        } else {
            Boolean parentStateChange;
            parentStateChange = mParent.Update (this, dc);
            changed = changed || parentStateChange;
        }

        /** Some state-transition events */

        If (Phone.DEBUG_PHONE) Log(
                "Update, wasConnectingInOrOut=" + wasConnectingInOrOut +
                ", wasHolding=" + wasHolding +
                ", isConnectingInOrOut=" + IsConnectingInOrOut() +
                ", changed=" + changed);


        If (wasConnectingInOrOut && !IsConnectingInOrOut()) {
            OnConnectedInOrOut();
        }

        If (changed && !wasHolding && (GetState() == CdmaCall.State.HOLDING)) {
            // We've transitioned into HOLDING
            OnStartedHolding();
        }

        return changed;
    }

    /**
     * Called when this Connection is in the foregroundCall
     * when a dial is initiated.
     * We know we're ACTIVE, and we know we're going to end up
     * HOLDING in the backgroundCall
     */
    void
    FakeHoldBeforeDial() {
        If (mParent != NULL) {
            mParent->Detach(this);
        }

        mParent = mOwner.mBackgroundCall;
        mParent->AttachFake(this, CdmaCall.State.HOLDING);

        OnStartedHolding();
    }

    /*package*/ Int32
    GetCDMAIndex() throws CallStateException {
        If (mIndex >= 0) {
            return mIndex + 1;
        } else {
            throw new CallStateException ("CDMA connection index not assigned");
        }
    }

    /**
     * An incoming or outgoing call has connected
     */
    void
    OnConnectedInOrOut() {
        mConnectTime = System->CurrentTimeMillis();
        mConnectTimeReal = SystemClock->ElapsedRealtime();
        mDuration = 0;

        // bug #678474: incoming call interpreted as missed call, even though
        // it sounds like the user has picked up the call.
        If (Phone.DEBUG_PHONE) {
            Log("onConnectedInOrOut: connectTime=" + mConnectTime);
        }

        If (!mIsIncoming) {
            // outgoing calls only
            ProcessNextPostDialChar();
        } else {
            // Only release wake lock for incoming calls, for outgoing calls the wake lock
            // will be released after any pause-dial is completed
            ReleaseWakeLock();
        }
    }

    private void
    DoDisconnect() {
        mIndex = -1;
        mDisconnectTime = System->CurrentTimeMillis();
        mDuration = SystemClock->ElapsedRealtime() - mConnectTimeReal;
        mDisconnected = TRUE;
        ClearPostDialListeners();
    }

    /*package*/ void
    OnStartedHolding() {
        mHoldingStartTime = SystemClock->ElapsedRealtime();
    }
    /**
     * Performs the appropriate action for a post-dial Char32, but does not
     * notify application. returns FALSE if the character is invalid and
     * should be ignored
     */
    private Boolean
    ProcessPostDialChar(Char32 c) {
        If (PhoneNumberUtils->Is12Key(c)) {
            mOwner.mCi->SendDtmf(c, mHandler->ObtainMessage(EVENT_DTMF_DONE));
        } else If (c == PhoneNumberUtils.PAUSE) {
            SetPostDialState(PostDialState.PAUSE);

            // Upon occurrences of the separator, the UE shall
            // pause again for 2 seconds before sending any
            // further DTMF digits.
            mHandler->SendMessageDelayed(mHandler->ObtainMessage(EVENT_PAUSE_DONE),
                                            PAUSE_DELAY_MILLIS);
        } else If (c == PhoneNumberUtils.WAIT) {
            SetPostDialState(PostDialState.WAIT);
        } else If (c == PhoneNumberUtils.WILD) {
            SetPostDialState(PostDialState.WILD);
        } else {
            return FALSE;
        }

        return TRUE;
    }

    //@Override
    public String GetRemainingPostDialString() {
        If (mPostDialState == PostDialState.CANCELLED
                || mPostDialState == PostDialState.COMPLETE
                || mPostDialString == NULL
                || mPostDialString->Length() <= mNextPostDialChar) {
            return "";
        }

        String subStr = mPostDialString->Substring(mNextPostDialChar);
        If (subStr != NULL) {
            Int32 wIndex = subStr->IndexOf(PhoneNumberUtils.WAIT);
            Int32 pIndex = subStr->IndexOf(PhoneNumberUtils.PAUSE);

            If (wIndex > 0 && (wIndex < pIndex || pIndex <= 0)) {
                subStr = subStr->Substring(0, wIndex);
            } else If (pIndex > 0) {
                subStr = subStr->Substring(0, pIndex);
            }
        }
        return subStr;
    }

    CARAPI UpdateParent(CdmaCall oldParent, CdmaCall newParent){
        If (newParent != oldParent) {
            If (oldParent != NULL) {
                oldParent->Detach(this);
            }
            newParent->AttachFake(this, CdmaCall.State.ACTIVE);
            mParent = newParent;
        }
    }

    //@Override
    protected void Finalize()
    {
        /**
         * It is understood that This finializer is not guaranteed
         * to be called and the release lock call is here just in
         * case there is some path that doesn't call onDisconnect
         * and or onConnectedInOrOut.
         */
        If (mPartialWakeLock->IsHeld()) {
            Rlog->E(LOG_TAG, "[CdmaConn] UNEXPECTED; mPartialWakeLock is held when finalizing.");
        }
        ReleaseWakeLock();
    }

    void ProcessNextPostDialChar() {
        Char32 c = 0;
        Registrant postDialHandler;

        If (mPostDialState == PostDialState.CANCELLED) {
            ReleaseWakeLock();
            //Rlog->V("CDMA", "##### processNextPostDialChar: postDialState == CANCELLED, bail");
            return;
        }

        If (mPostDialString == NULL ||
                mPostDialString->Length() <= mNextPostDialChar) {
            SetPostDialState(PostDialState.COMPLETE);

            // We were holding a wake lock until pause-dial was complete, so give it up now
            ReleaseWakeLock();

            // notifyMessage.arg1 is 0 on complete
            c = 0;
        } else {
            Boolean isValid;

            SetPostDialState(PostDialState.STARTED);

            c = mPostDialString->CharAt(mNextPostDialChar++);

            isValid = ProcessPostDialChar(c);

            If (!isValid) {
                // Will call processNextPostDialChar
                mHandler->ObtainMessage(EVENT_NEXT_POST_DIAL).SendToTarget();
                // Don't notify application
                Rlog->E("CDMA", "processNextPostDialChar: c=" + c + " isn't valid!");
                return;
            }
        }

        postDialHandler = mOwner.mPhone.mPostDialHandler;

        Message notifyMessage;

        If (postDialHandler != NULL &&
                (notifyMessage = postDialHandler->MessageForRegistrant()) != NULL) {
            // The AsyncResult.result is the Connection object
            PostDialState state = mPostDialState;
            AsyncResult ar = AsyncResult->ForMessage(notifyMessage);
            ar.result = this;
            ar.userObj = state;

            // arg1 is the character that was/is being processed
            notifyMessage.arg1 = c;

            notifyMessage->SendToTarget();
        }
    }


    /** "connecting" means "has never been ACTIVE" for both incoming
     *  and outgoing calls
     */
    private Boolean
    IsConnectingInOrOut() {
        return mParent == NULL || mParent == mOwner.mRingingCall
            || mParent.mState == CdmaCall.State.DIALING
            || mParent.mState == CdmaCall.State.ALERTING;
    }

    private CdmaCall
    ParentFromDCState (DriverCall.State state) {
        Switch (state) {
            case ACTIVE:
            case DIALING:
            case ALERTING:
                return mOwner.mForegroundCall;
            //break;

            case HOLDING:
                return mOwner.mBackgroundCall;
            //break;

            case INCOMING:
            case WAITING:
                return mOwner.mRingingCall;
            //break;

            default:
                throw new RuntimeException("illegal call state: " + state);
        }
    }

    /**
     * Set post dial state and acquire wake lock while switching to "started" or "wait"
     * state, the wake lock will be released if state switches out of "started" or "wait"
     * state or after WAKE_LOCK_TIMEOUT_MILLIS.
     * @param s new PostDialState
     */
    private void SetPostDialState(PostDialState s) {
        If (s == PostDialState.STARTED ||
                s == PostDialState.PAUSE) {
            {    AutoLock syncLock(mPartialWakeLock);
                If (mPartialWakeLock->IsHeld()) {
                    mHandler->RemoveMessages(EVENT_WAKE_LOCK_TIMEOUT);
                } else {
                    AcquireWakeLock();
                }
                Message msg = mHandler->ObtainMessage(EVENT_WAKE_LOCK_TIMEOUT);
                mHandler->SendMessageDelayed(msg, WAKE_LOCK_TIMEOUT_MILLIS);
            }
        } else {
            mHandler->RemoveMessages(EVENT_WAKE_LOCK_TIMEOUT);
            ReleaseWakeLock();
        }
        mPostDialState = s;
        NotifyPostDialListeners();
    }

    private void CreateWakeLock(Context context) {
        PowerManager pm = (PowerManager)context->GetSystemService(Context.POWER_SERVICE);
        mPartialWakeLock = pm->NewWakeLock(PowerManager.PARTIAL_WAKE_LOCK, LOG_TAG);
    }

    private void AcquireWakeLock() {
        Log("acquireWakeLock");
        mPartialWakeLock->Acquire();
    }

    private void ReleaseWakeLock() {
        {    AutoLock syncLock(mPartialWakeLock);
            If (mPartialWakeLock->IsHeld()) {
                Log("releaseWakeLock");
                mPartialWakeLock->Release();
            }
        }
    }

    private static Boolean IsPause(Char32 c) {
        return c == PhoneNumberUtils.PAUSE;
    }

    private static Boolean IsWait(Char32 c) {
        return c == PhoneNumberUtils.WAIT;
    }

    // This function is to find the next PAUSE character index if
    // multiple pauses in a row. Otherwise it finds the next non PAUSE or
    // non WAIT character index.
    private static Int32
    FindNextPCharOrNonPOrNonWCharIndex(String phoneNumber, Int32 currIndex) {
        Boolean wMatched = IsWait(phoneNumber->CharAt(currIndex));
        Int32 index = currIndex + 1;
        Int32 length = phoneNumber->Length();
        While (index < length) {
            Char32 cNext = phoneNumber->CharAt(index);
            // if there is any W inside P/W sequence,mark it
            If (IsWait(cNext)) {
                wMatched = TRUE;
            }
            // if any characters other than P/W chars after P/W sequence
            // we break out the loop and append the correct
            If (!IsWait(cNext) && !IsPause(cNext)) {
                break;
            }
            index++;
        }

        // It means the PAUSE Character(s) is in the middle of dial string
        // and it needs to be handled one by one.
        If ((index < length) && (index > (currIndex + 1))  &&
            ((wMatched == FALSE) && IsPause(phoneNumber->CharAt(currIndex)))) {
            Return (currIndex + 1);
        }
        return index;
    }

    // This function returns either PAUSE or WAIT character to append.
    // It is based on the next non PAUSE/WAIT character in the phoneNumber and the
    // index for the current PAUSE/WAIT character
    private static Char32
    FindPOrWCharToAppend(String phoneNumber, Int32 currPwIndex, Int32 nextNonPwCharIndex) {
        Char32 c = phoneNumber->CharAt(currPwIndex);
        Char32 ret;

        // Append the PW Char32
        ret = (IsPause(c)) ? PhoneNumberUtils.PAUSE : PhoneNumberUtils.WAIT;

        // If the nextNonPwCharIndex is greater than currPwIndex + 1,
        // it means the PW sequence contains not only P characters.
        // Since for the sequence that only contains P character,
        // the P character is handled one by one, the nextNonPwCharIndex
        // equals to currPwIndex + 1.
        // In this case, skip P, append W.
        If (nextNonPwCharIndex > (currPwIndex + 1)) {
            ret = PhoneNumberUtils.WAIT;
        }
        return ret;
    }

    /**
     * format original dial string
     * 1) convert international dialing prefix "+" to
     *    string specified per region
     *
     * 2) handle corner cases for PAUSE/WAIT dialing:
     *
     *    If PAUSE/WAIT sequence at the end, ignore them.
     *
     *    If consecutive PAUSE/WAIT sequence in the middle of the string,
     *    and if there is any WAIT in PAUSE/WAIT sequence, treat them like WAIT.
     */
    public static String FormatDialString(String phoneNumber) {
        /**
         * TODO(cleanup): This function should move to PhoneNumberUtils, and
         * tests should be added.
         */

        If (phoneNumber == NULL) {
            return NULL;
        }
        Int32 length = phoneNumber->Length();
        StringBuilder ret = new StringBuilder();
        Char32 c;
        Int32 currIndex = 0;

        While (currIndex < length) {
            c = phoneNumber->CharAt(currIndex);
            If (IsPause(c) || IsWait(c)) {
                If (currIndex < length - 1) {
                    // if PW not at the end
                    Int32 nextIndex = FindNextPCharOrNonPOrNonWCharIndex(phoneNumber, currIndex);
                    // If there is non PW Char32 following PW sequence
                    If (nextIndex < length) {
                        Char32 pC = FindPOrWCharToAppend(phoneNumber, currIndex, nextIndex);
                        ret->Append(pC);
                        // If PW Char32 sequence has more than 2 PW characters,
                        // skip to the last PW character since the sequence already be
                        // converted to WAIT character
                        If (nextIndex > (currIndex + 1)) {
                            currIndex = nextIndex - 1;
                        }
                    } else If (nextIndex == length) {
                        // It means PW characters at the end, ignore
                        currIndex = length - 1;
                    }
                }
            } else {
                ret->Append(c);
            }
            currIndex++;
        }
        return PhoneNumberUtils->CdmaCheckAndProcessPlusCode(ret->ToString());
    }

    private void Log(String msg) {
        Rlog->D(LOG_TAG, "[CDMAConn] " + msg);
    }

    //@Override
    public Int32 GetNumberPresentation() {
        return mNumberPresentation;
    }

    //@Override
    public UUSInfo GetUUSInfo() {
        // UUS information not supported in CDMA
        return NULL;
    }

    public Int32 GetPreciseDisconnectCause() {
        return mPreciseCause;
    }

    //@Override
    public Connection GetOrigConnection() {
        return NULL;
    }

    //@Override
    public Boolean IsMultiparty() {
        return FALSE;
    }
}
