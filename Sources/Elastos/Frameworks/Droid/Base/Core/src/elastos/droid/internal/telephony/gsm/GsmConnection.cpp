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

package com.android.internal.telephony.gsm;
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
using Elastos::Droid::Telephony::IPhoneNumberUtils;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Droid::Text::ITextUtils;

using Elastos::Droid::Internal::Telephony::I*;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccCardApplication;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccController;
using Elastos::Droid::Internal::Telephony::Uicc::IccCardApplicationStatus::IAppState;

/**
 * {@hide}
 */
public class GsmConnection extends Connection {
    private static const String LOG_TAG = "GsmConnection";
    private static const Boolean DBG = TRUE;

    //***** Instance Variables

    GsmCallTracker mOwner;
    GsmCall mParent;

    String mPostDialString;      // outgoing calls only
    Boolean mDisconnected;

    Int32 mIndex;          // index in GsmCallTracker.connections[], -1 if unassigned
                        // The GSM index is 1 + this

    /*
     * These time/timespan values are based on System->CurrentTimeMillis(),
     * i.e., "wall clock" time.
     */
    Int64 mDisconnectTime;

    Int32 mNextPostDialChar;       // index into postDialString

    Int32 mCause = DisconnectCause.NOT_DISCONNECTED;
    PostDialState mPostDialState = PostDialState.NOT_STARTED;
    UUSInfo mUusInfo;
    Int32 mPreciseCause = 0;

    Connection mOrigConnection;

    Handler mHandler;

    private PowerManager.WakeLock mPartialWakeLock;

    //***** Event Constants
    static const Int32 EVENT_DTMF_DONE = 1;
    static const Int32 EVENT_PAUSE_DONE = 2;
    static const Int32 EVENT_NEXT_POST_DIAL = 3;
    static const Int32 EVENT_WAKE_LOCK_TIMEOUT = 4;

    //***** Constants
    static const Int32 PAUSE_DELAY_MILLIS = 3 * 1000;
    static const Int32 WAKE_LOCK_TIMEOUT_MILLIS = 60*1000;

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
    GsmConnection (Context context, DriverCall dc, GsmCallTracker ct, Int32 index) {
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
        mUusInfo = dc.uusInfo;

        mIndex = index;

        mParent = ParentFromDCState (dc.state);
        mParent->Attach(this, dc);
    }

    /** This is an MO call, created when dialing */
    /*package*/
    GsmConnection (Context context, String dialString, GsmCallTracker ct, GsmCall parent) {
        CreateWakeLock(context);
        AcquireWakeLock();

        mOwner = ct;
        mHandler = new MyHandler(mOwner->GetLooper());

        mDialString = dialString;

        mAddress = PhoneNumberUtils->ExtractNetworkPortionAlt(dialString);
        mPostDialString = PhoneNumberUtils->ExtractPostDialPortion(dialString);

        mIndex = -1;

        mIsIncoming = FALSE;
        mCnapName = NULL;
        mCnapNamePresentation = PhoneConstants.PRESENTATION_ALLOWED;
        mNumberPresentation = PhoneConstants.PRESENTATION_ALLOWED;
        mCreateTime = System->CurrentTimeMillis();

        mParent = parent;
        parent->AttachFake(this, GsmCall.State.DIALING);
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

        // A new call appearing by SRVCC may have invalid number
        //  if IMS service is not tightly coupled with cellular modem stack.
        // Thus we prefer the preexisting handover connection instance.
        If (mOrigConnection != NULL) return TRUE;

        // ... but we can compare phone numbers on MT calls, and we have
        // no control over when they begin, so we might as well

        String cAddress = PhoneNumberUtils->StringFromStringAndTOA(c.number, c.TOA);
        return mIsIncoming == c.isMT && EqualsHandlesNulls(mAddress, cAddress);
    }

    //@Override
    public GsmCall GetCall() {
        return mParent;
    }

    //@Override
    public Int64 GetDisconnectTime() {
        return mDisconnectTime;
    }

    //@Override
    public Int64 GetHoldDurationMillis() {
        If (GetState() != GsmCall.State.HOLDING) {
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
    public GsmCall.State GetState() {
        If (mDisconnected) {
            return GsmCall.State.DISCONNECTED;
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
            Rlog->W(LOG_TAG, "GsmConnection->ProceedAfterWaitChar(): Expected "
                + "GetPostDialState() to be WAIT but was " + mPostDialState);
            return;
        }

        SetPostDialState(PostDialState.STARTED);

        ProcessNextPostDialChar();
    }

    //@Override
    CARAPI ProceedAfterWildChar(String str) {
        If (mPostDialState != PostDialState.WILD) {
            Rlog->W(LOG_TAG, "GsmConnection->ProceedAfterWaitChar(): Expected "
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
            case CallFailCause.TEMPORARY_FAILURE:
            case CallFailCause.SWITCHING_CONGESTION:
            case CallFailCause.CHANNEL_NOT_AVAIL:
            case CallFailCause.QOS_NOT_AVAIL:
            case CallFailCause.BEARER_NOT_AVAIL:
                return DisconnectCause.CONGESTION;

            case CallFailCause.ACM_LIMIT_EXCEEDED:
                return DisconnectCause.LIMIT_EXCEEDED;

            case CallFailCause.CALL_BARRED:
                return DisconnectCause.CALL_BARRED;

            case CallFailCause.FDN_BLOCKED:
                return DisconnectCause.FDN_BLOCKED;

            case CallFailCause.UNOBTAINABLE_NUMBER:
                return DisconnectCause.UNOBTAINABLE_NUMBER;

            case CallFailCause.DIAL_MODIFIED_TO_USSD:
                return DisconnectCause.DIAL_MODIFIED_TO_USSD;

            case CallFailCause.DIAL_MODIFIED_TO_SS:
                return DisconnectCause.DIAL_MODIFIED_TO_SS;

            case CallFailCause.DIAL_MODIFIED_TO_DIAL:
                return DisconnectCause.DIAL_MODIFIED_TO_DIAL;

            case CallFailCause.ERROR_UNSPECIFIED:
            case CallFailCause.NORMAL_CLEARING:
            default:
                GSMPhone phone = mOwner.mPhone;
                Int32 serviceState = phone->GetServiceState()->GetState();
                UiccCardApplication cardApp = phone->GetUiccCardApplication();
                AppState uiccAppState = (cardApp != NULL) ? cardApp->GetState() :
                                                            AppState.APPSTATE_UNKNOWN;
                If (serviceState == ServiceState.STATE_POWER_OFF) {
                    return DisconnectCause.POWER_OFF;
                } else If (serviceState == ServiceState.STATE_OUT_OF_SERVICE
                        || serviceState == ServiceState.STATE_EMERGENCY_ONLY ) {
                    return DisconnectCause.OUT_OF_SERVICE;
                } else If (uiccAppState != AppState.APPSTATE_READY) {
                    return DisconnectCause.ICC_ERROR;
                } else If (causeCode == CallFailCause.ERROR_UNSPECIFIED) {
                    If (phone.mSST.mRestrictedState->IsCsRestricted()) {
                        return DisconnectCause.CS_RESTRICTED;
                    } else If (phone.mSST.mRestrictedState->IsCsEmergencyRestricted()) {
                        return DisconnectCause.CS_RESTRICTED_EMERGENCY;
                    } else If (phone.mSST.mRestrictedState->IsCsNormalRestricted()) {
                        return DisconnectCause.CS_RESTRICTED_NORMAL;
                    } else {
                        return DisconnectCause.ERROR_UNSPECIFIED;
                    }
                } else If (causeCode == CallFailCause.NORMAL_CLEARING) {
                    return DisconnectCause.NORMAL;
                } else {
                    // If nothing else matches, report unknown call drop reason
                    // to app, not NORMAL call end.
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
    /*package*/ Boolean OnDisconnect(Int32 cause) {
        Boolean changed = FALSE;

        mCause = cause;

        If (!mDisconnected) {
            mIndex = -1;

            mDisconnectTime = System->CurrentTimeMillis();
            mDuration = SystemClock->ElapsedRealtime() - mConnectTimeReal;
            mDisconnected = TRUE;

            If (DBG) Rlog->D(LOG_TAG, "onDisconnect: cause=" + cause);

            mOwner.mPhone->NotifyDisconnect(this);

            If (mParent != NULL) {
                changed = mParent->ConnectionDisconnected(this);
            }

            mOrigConnection = NULL;
        }
        ClearPostDialListeners();
        ReleaseWakeLock();
        return changed;
    }

    // Returns TRUE if state has changed, FALSE if nothing changed
    /*package*/ Boolean
    Update (DriverCall dc) {
        GsmCall newParent;
        Boolean changed = FALSE;
        Boolean wasConnectingInOrOut = IsConnectingInOrOut();
        Boolean wasHolding = (GetState() == GsmCall.State.HOLDING);

        newParent = ParentFromDCState(dc.state);

        //Ignore dc.number and dc.name in case of a handover connection
        If (mOrigConnection != NULL) {
            If (Phone.DEBUG_PHONE) Log("update: mOrigConnection is not NULL");
        } else {
            Log(" mNumberConverted " + mNumberConverted);
            If (!EqualsHandlesNulls(mAddress, dc.number) && (!mNumberConverted
                    || !EqualsHandlesNulls(mConvertedNumber, dc.number))) {
                If (Phone.DEBUG_PHONE) Log("update: phone # changed!");
                mAddress = dc.number;
                changed = TRUE;
            }
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
                "update: parent=" + mParent +
                ", hasNewParent=" + (newParent != mParent) +
                ", wasConnectingInOrOut=" + wasConnectingInOrOut +
                ", wasHolding=" + wasHolding +
                ", isConnectingInOrOut=" + IsConnectingInOrOut() +
                ", changed=" + changed);


        If (wasConnectingInOrOut && !IsConnectingInOrOut()) {
            OnConnectedInOrOut();
        }

        If (changed && !wasHolding && (GetState() == GsmCall.State.HOLDING)) {
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
        mParent->AttachFake(this, GsmCall.State.HOLDING);

        OnStartedHolding();
    }

    /*package*/ Int32
    GetGSMIndex() throws CallStateException {
        If (mIndex >= 0) {
            return mIndex + 1;
        } else {
            throw new CallStateException ("GSM index not yet assigned");
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
        }
        ReleaseWakeLock();
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
            // From TS 22.101:
            // It continues...
            // Upon the called party answering the UE shall send the DTMF digits
            // automatically to the network after a delay of 3 Seconds( 20 ).
            // The digits shall be sent according to the procedures and timing
            // specified in 3GPP TS 24.008 [13]. The first occurrence of the
            // "DTMF Control Digits Separator" shall be used by the ME to
            // distinguish between the addressing Digits (i.e. the phone number)
            // and the DTMF digits. Upon subsequent occurrences of the
            // separator,
            // the UE shall pause again for 3 Seconds ( 20 ) before sending
            // any further DTMF digits.
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
    public String
    GetRemainingPostDialString() {
        If (mPostDialState == PostDialState.CANCELLED
            || mPostDialState == PostDialState.COMPLETE
            || mPostDialString == NULL
            || mPostDialString->Length() <= mNextPostDialChar
        ) {
            return "";
        }

        return mPostDialString->Substring(mNextPostDialChar);
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
            Rlog->E(LOG_TAG, "[GSMConn] UNEXPECTED; mPartialWakeLock is held when finalizing.");
        }
        ClearPostDialListeners();
        ReleaseWakeLock();
    }

    private void
    ProcessNextPostDialChar() {
        Char32 c = 0;
        Registrant postDialHandler;

        If (mPostDialState == PostDialState.CANCELLED) {
            //Rlog->V("GSM", "##### processNextPostDialChar: postDialState == CANCELLED, bail");
            return;
        }

        If (mPostDialString == NULL ||
                mPostDialString->Length() <= mNextPostDialChar) {
            SetPostDialState(PostDialState.COMPLETE);

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
                Rlog->E("GSM", "processNextPostDialChar: c=" + c + " isn't valid!");
                return;
            }
        }

        postDialHandler = mOwner.mPhone.mPostDialHandler;

        Message notifyMessage;

        If (postDialHandler != NULL
                && (notifyMessage = postDialHandler->MessageForRegistrant()) != NULL) {
            // The AsyncResult.result is the Connection object
            PostDialState state = mPostDialState;
            AsyncResult ar = AsyncResult->ForMessage(notifyMessage);
            ar.result = this;
            ar.userObj = state;

            // arg1 is the character that was/is being processed
            notifyMessage.arg1 = c;

            //Rlog->V("GSM", "##### processNextPostDialChar: send msg to postDialHandler, arg1=" + c);
            notifyMessage->SendToTarget();
        }
    }


    /** "connecting" means "has never been ACTIVE" for both incoming
     *  and outgoing calls
     */
    private Boolean
    IsConnectingInOrOut() {
        return mParent == NULL || mParent == mOwner.mRingingCall
            || mParent.mState == GsmCall.State.DIALING
            || mParent.mState == GsmCall.State.ALERTING;
    }

    private GsmCall
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
     * Set post dial state and acquire wake lock while switching to "started"
     * state, the wake lock will be released if state switches out of "started"
     * state or after WAKE_LOCK_TIMEOUT_MILLIS.
     * @param s new PostDialState
     */
    private void SetPostDialState(PostDialState s) {
        If (mPostDialState != PostDialState.STARTED
                && s == PostDialState.STARTED) {
            AcquireWakeLock();
            Message msg = mHandler->ObtainMessage(EVENT_WAKE_LOCK_TIMEOUT);
            mHandler->SendMessageDelayed(msg, WAKE_LOCK_TIMEOUT_MILLIS);
        } else If (mPostDialState == PostDialState.STARTED
                && s != PostDialState.STARTED) {
            mHandler->RemoveMessages(EVENT_WAKE_LOCK_TIMEOUT);
            ReleaseWakeLock();
        }
        mPostDialState = s;
        NotifyPostDialListeners();
    }

    private void
    CreateWakeLock(Context context) {
        PowerManager pm = (PowerManager) context->GetSystemService(Context.POWER_SERVICE);
        mPartialWakeLock = pm->NewWakeLock(PowerManager.PARTIAL_WAKE_LOCK, LOG_TAG);
    }

    private void
    AcquireWakeLock() {
        Log("acquireWakeLock");
        mPartialWakeLock->Acquire();
    }

    private void
    ReleaseWakeLock() {
        Synchronized(mPartialWakeLock) {
            If (mPartialWakeLock->IsHeld()) {
                Log("releaseWakeLock");
                mPartialWakeLock->Release();
            }
        }
    }

    private void Log(String msg) {
        Rlog->D(LOG_TAG, "[GSMConn] " + msg);
    }

    //@Override
    public Int32 GetNumberPresentation() {
        return mNumberPresentation;
    }

    //@Override
    public UUSInfo GetUUSInfo() {
        return mUusInfo;
    }

    public Int32 GetPreciseDisconnectCause() {
        return mPreciseCause;
    }

    //@Override
    CARAPI MigrateFrom(Connection c) {
        If (c == NULL) return;

        super->MigrateFrom(c);

        this.mUusInfo = c->GetUUSInfo();

        this->SetUserData(c->GetUserData());
    }

    //@Override
    public Connection GetOrigConnection() {
        return mOrigConnection;
    }

    //@Override
    public Boolean IsMultiparty() {
        If (mOrigConnection != NULL) {
            return mOrigConnection->IsMultiparty();
        }

        return FALSE;
    }
}
