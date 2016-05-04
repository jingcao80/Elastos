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

using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IRegistrant;
using Elastos::Droid::Os::IRegistrantList;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Telephony::IDisconnectCause;
using Elastos::Droid::Telephony::IPhoneNumberUtils;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Telephony::Gsm::IGsmCellLocation;
using Elastos::Droid::Utility::IEventLog;
using Elastos::Droid::Telephony::IRlog;

using Elastos::Droid::Internal::Telephony::ICall;
using Elastos::Droid::Internal::Telephony::ICallTracker;
using Elastos::Droid::Internal::Telephony::ICommandsInterface;
using Elastos::Droid::Internal::Telephony::IConnection;
using Elastos::Droid::Internal::Telephony::IDriverCall;
using Elastos::Droid::Internal::Telephony::IEventLogTags;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Internal::Telephony::IPhoneBase;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Internal::Telephony::ITelephonyProperties;
using Elastos::Droid::Internal::Telephony::IUUSInfo;
using Elastos::Droid::Internal::Telephony::Gsm::ICallFailCause;
using Elastos::Droid::Internal::Telephony::Gsm::IGSMPhone;
using Elastos::Droid::Internal::Telephony::Gsm::IGsmCall;
using Elastos::Droid::Internal::Telephony::Gsm::IGsmConnection;

using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::IList;
using Elastos::Utility::IArrayList;

/**
 * {@hide}
 */
public class GsmCallTracker extends CallTracker {
    static const String LOG_TAG = "GsmCallTracker";
    private static const Boolean REPEAT_POLLING = FALSE;

    private static const Boolean DBG_POLL = FALSE;

    //***** Constants

    static const Int32 MAX_CONNECTIONS = 7;   // only 7 connections allowed in GSM
    static const Int32 MAX_CONNECTIONS_PER_CALL = 5; // only 5 connections allowed per call

    //***** Instance Variables
    GsmConnection mConnections[] = new GsmConnection[MAX_CONNECTIONS];
    RegistrantList mVoiceCallEndedRegistrants = new RegistrantList();
    RegistrantList mVoiceCallStartedRegistrants = new RegistrantList();


    // connections dropped during last poll
    ArrayList<GsmConnection> mDroppedDuringPoll
        = new ArrayList<GsmConnection>(MAX_CONNECTIONS);

    GsmCall mRingingCall = new GsmCall(this);
            // A call that is ringing Or (call) waiting
    GsmCall mForegroundCall = new GsmCall(this);
    GsmCall mBackgroundCall = new GsmCall(this);

    GsmConnection mPendingMO;
    Boolean mHangupPendingMO;

    GSMPhone mPhone;

    Boolean mDesiredMute = FALSE;    // FALSE = mute off

    PhoneConstants.State mState = PhoneConstants.State.IDLE;

    Call.SrvccState mSrvccState = Call.SrvccState.NONE;

    //***** Events


    //***** Constructors

    GsmCallTracker (GSMPhone phone) {
        this.mPhone = phone;
        mCi = phone.mCi;

        mCi->RegisterForCallStateChanged(this, EVENT_CALL_STATE_CHANGE, NULL);

        mCi->RegisterForOn(this, EVENT_RADIO_AVAILABLE, NULL);
        mCi->RegisterForNotAvailable(this, EVENT_RADIO_NOT_AVAILABLE, NULL);
    }

    CARAPI Dispose() {
        Rlog->D(LOG_TAG, "GsmCallTracker dispose");
        //Unregister for all events
        mCi->UnregisterForCallStateChanged(this);
        mCi->UnregisterForOn(this);
        mCi->UnregisterForNotAvailable(this);


        ClearDisconnected();
    }

    //@Override
    protected void Finalize() {
        Rlog->D(LOG_TAG, "GsmCallTracker finalized");
    }

    //***** Instance Methods

    //***** Public Methods
    //@Override
    CARAPI RegisterForVoiceCallStarted(Handler h, Int32 what, Object obj) {
        Registrant r = new Registrant(h, what, obj);
        mVoiceCallStartedRegistrants->Add(r);
    }

    //@Override
    CARAPI UnregisterForVoiceCallStarted(Handler h) {
        mVoiceCallStartedRegistrants->Remove(h);
    }

    //@Override
    CARAPI RegisterForVoiceCallEnded(Handler h, Int32 what, Object obj) {
        Registrant r = new Registrant(h, what, obj);
        mVoiceCallEndedRegistrants->Add(r);
    }

    //@Override
    CARAPI UnregisterForVoiceCallEnded(Handler h) {
        mVoiceCallEndedRegistrants->Remove(h);
    }

    private void
    FakeHoldForegroundBeforeDial() {
        List<Connection> connCopy;

        // We need to make a copy here, since FakeHoldBeforeDial()
        // modifies the lists, and we don't want to reverse the order
        connCopy = (List<Connection>) mForegroundCall.mConnections->Clone();

        For (Int32 i = 0, s = connCopy->Size() ; i < s ; i++) {
            GsmConnection conn = (GsmConnection)connCopy->Get(i);

            conn->FakeHoldBeforeDial();
        }
    }

    /**
     * clirMode is one of the CLIR_ constants
     */
    synchronized Connection
    Dial (String dialString, Int32 clirMode, UUSInfo uusInfo) throws CallStateException {
        // note that this triggers call state changed notif
        ClearDisconnected();

        If (!CanDial()) {
            throw new CallStateException("cannot dial in current state");
        }

        String origNumber = dialString;
        dialString = ConvertNumberIfNecessary(mPhone, dialString);

        // The new call must be assigned to the foreground call.
        // That call must be idle, so place anything that's
        // there on hold
        If (mForegroundCall->GetState() == GsmCall.State.ACTIVE) {
            // this will probably be done by the radio anyway
            // but the dial might fail before this happens
            // and we need to make sure the foreground call is clear
            // for the newly dialed connection
            SwitchWaitingOrHoldingAndActive();

            // Fake local state so that
            // a) foregroundCall is empty for the newly dialed connection
            // b) hasNonHangupStateChanged remains FALSE in the
            // next poll, so that we don't clear a failed dialing call
            FakeHoldForegroundBeforeDial();
        }

        If (mForegroundCall->GetState() != GsmCall.State.IDLE) {
            //we should have failed in !CanDial() above before we get here
            throw new CallStateException("cannot dial in current state");
        }

        mPendingMO = new GsmConnection(mPhone->GetContext(), CheckForTestEmergencyNumber(dialString),
                this, mForegroundCall);
        mHangupPendingMO = FALSE;

        If ( mPendingMO->GetAddress() == NULL || mPendingMO->GetAddress()->Length() == 0
                || mPendingMO->GetAddress()->IndexOf(PhoneNumberUtils.WILD) >= 0
        ) {
            // Phone number is invalid
            mPendingMO.mCause = DisconnectCause.INVALID_NUMBER;

            // HandlePollCalls() will notice this call not present
            // and will mark it as dropped.
            PollCallsWhenSafe();
        } else {
            // Always unmute when initiating a new call
            SetMute(FALSE);

            mCi->Dial(mPendingMO->GetAddress(), clirMode, uusInfo, ObtainCompleteMessage());
        }

        If (mNumberConverted) {
            mPendingMO->SetConverted(origNumber);
            mNumberConverted = FALSE;
        }

        UpdatePhoneState();
        mPhone->NotifyPreciseCallStateChanged();

        return mPendingMO;
    }

    Connection
    Dial(String dialString) throws CallStateException {
        return Dial(dialString, CommandsInterface.CLIR_DEFAULT, NULL);
    }

    Connection
    Dial(String dialString, UUSInfo uusInfo) throws CallStateException {
        return Dial(dialString, CommandsInterface.CLIR_DEFAULT, uusInfo);
    }

    Connection
    Dial(String dialString, Int32 clirMode) throws CallStateException {
        return Dial(dialString, clirMode, NULL);
    }

    void
    AcceptCall () throws CallStateException {
        // FIXME if SWITCH fails, should retry with ANSWER
        // in case the active/holding call disappeared and this
        // is no longer call waiting

        If (mRingingCall->GetState() == GsmCall.State.INCOMING) {
            Rlog->I("phone", "acceptCall: incoming...");
            // Always unmute when answering a new call
            SetMute(FALSE);
            mCi->AcceptCall(ObtainCompleteMessage());
        } else If (mRingingCall->GetState() == GsmCall.State.WAITING) {
            SetMute(FALSE);
            SwitchWaitingOrHoldingAndActive();
        } else {
            throw new CallStateException("phone not ringing");
        }
    }

    void
    RejectCall () throws CallStateException {
        // AT+CHLD=0 means "release held or UDUB"
        // so if the phone isn't ringing, this could hang up held
        If (mRingingCall->GetState()->IsRinging()) {
            mCi->RejectCall(ObtainCompleteMessage());
        } else {
            throw new CallStateException("phone not ringing");
        }
    }

    void
    SwitchWaitingOrHoldingAndActive() throws CallStateException {
        // Should we bother with this check?
        If (mRingingCall->GetState() == GsmCall.State.INCOMING) {
            throw new CallStateException("cannot be in the incoming state");
        } else {
            mCi->SwitchWaitingOrHoldingAndActive(
                    ObtainCompleteMessage(EVENT_SWITCH_RESULT));
        }
    }

    void
    Conference() {
        mCi->Conference(ObtainCompleteMessage(EVENT_CONFERENCE_RESULT));
    }

    void
    ExplicitCallTransfer() {
        mCi->ExplicitCallTransfer(ObtainCompleteMessage(EVENT_ECT_RESULT));
    }

    void
    ClearDisconnected() {
        InternalClearDisconnected();

        UpdatePhoneState();
        mPhone->NotifyPreciseCallStateChanged();
    }

    Boolean
    CanConference() {
        return mForegroundCall->GetState() == GsmCall.State.ACTIVE
                && mBackgroundCall->GetState() == GsmCall.State.HOLDING
                && !mBackgroundCall->IsFull()
                && !mForegroundCall->IsFull();
    }

    Boolean
    CanDial() {
        Boolean ret;
        Int32 serviceState = mPhone->GetServiceState()->GetState();
        String disableCall = SystemProperties->Get(
                TelephonyProperties.PROPERTY_DISABLE_CALL, "FALSE");

        ret = (serviceState != ServiceState.STATE_POWER_OFF)
                && mPendingMO == NULL
                && !mRingingCall->IsRinging()
                && !disableCall->Equals("TRUE")
                && (!mForegroundCall->GetState()->IsAlive()
                    || !mBackgroundCall->GetState()->IsAlive());

        return ret;
    }

    Boolean
    CanTransfer() {
        Return (mForegroundCall->GetState() == GsmCall.State.ACTIVE
                || mForegroundCall->GetState() == GsmCall.State.ALERTING
                || mForegroundCall->GetState() == GsmCall.State.DIALING)
            && mBackgroundCall->GetState() == GsmCall.State.HOLDING;
    }

    //***** Private Instance Methods

    private void
    InternalClearDisconnected() {
        mRingingCall->ClearDisconnected();
        mForegroundCall->ClearDisconnected();
        mBackgroundCall->ClearDisconnected();
    }

    /**
     * Obtain a message to use for signalling "invoke GetCurrentCalls() when
     * this operation and all other pending operations are complete
     */
    private Message
    ObtainCompleteMessage() {
        return ObtainCompleteMessage(EVENT_OPERATION_COMPLETE);
    }

    /**
     * Obtain a message to use for signalling "invoke GetCurrentCalls() when
     * this operation and all other pending operations are complete
     */
    private Message
    ObtainCompleteMessage(Int32 what) {
        mPendingOperations++;
        mLastRelevantPoll = NULL;
        mNeedsPoll = TRUE;

        If (DBG_POLL) Log("obtainCompleteMessage: pendingOperations=" +
                mPendingOperations + ", needsPoll=" + mNeedsPoll);

        return ObtainMessage(what);
    }

    private void
    OperationComplete() {
        mPendingOperations--;

        If (DBG_POLL) Log("operationComplete: pendingOperations=" +
                mPendingOperations + ", needsPoll=" + mNeedsPoll);

        If (mPendingOperations == 0 && mNeedsPoll) {
            mLastRelevantPoll = ObtainMessage(EVENT_POLL_CALLS_RESULT);
            mCi->GetCurrentCalls(mLastRelevantPoll);
        } else If (mPendingOperations < 0) {
            // this should never happen
            Rlog->E(LOG_TAG,"GsmCallTracker.pendingOperations < 0");
            mPendingOperations = 0;
        }
    }

    private void
    UpdatePhoneState() {
        PhoneConstants.State oldState = mState;

        If (mRingingCall->IsRinging()) {
            mState = PhoneConstants.State.RINGING;
        } else If (mPendingMO != NULL ||
                !(mForegroundCall->IsIdle() && mBackgroundCall->IsIdle())) {
            mState = PhoneConstants.State.OFFHOOK;
        } else {
            mState = PhoneConstants.State.IDLE;
        }

        If (mState == PhoneConstants.State.IDLE && oldState != mState) {
            mVoiceCallEndedRegistrants->NotifyRegistrants(
                new AsyncResult(NULL, NULL, NULL));
        } else If (oldState == PhoneConstants.State.IDLE && oldState != mState) {
            mVoiceCallStartedRegistrants.NotifyRegistrants (
                    new AsyncResult(NULL, NULL, NULL));
        }

        If (mState != oldState) {
            mPhone->NotifyPhoneStateChanged();
        }
    }

    //@Override
    protected synchronized void
    HandlePollCalls(AsyncResult ar) {
        List polledCalls;

        If (ar.exception == NULL) {
            polledCalls = (List)ar.result;
        } else If (IsCommandExceptionRadioNotAvailable(ar.exception)) {
            // just a dummy empty ArrayList to cause the loop
            // to hang up all the calls
            polledCalls = new ArrayList();
        } else {
            // Radio probably wasn't ready--try again in a bit
            // But don't keep polling if the channel is closed
            PollCallsAfterDelay();
            return;
        }

        Connection newRinging = NULL; //or waiting
        Connection newUnknown = NULL;
        Boolean hasNonHangupStateChanged = FALSE;   // Any change besides
                                                    // a dropped connection
        Boolean hasAnyCallDisconnected = FALSE;
        Boolean needsPollDelay = FALSE;
        Boolean unknownConnectionAppeared = FALSE;

        For (Int32 i = 0, curDC = 0, dcSize = polledCalls->Size()
                ; i < mConnections.length; i++) {
            GsmConnection conn = mConnections[i];
            DriverCall dc = NULL;

            // polledCall list is sparse
            If (curDC < dcSize) {
                dc = (DriverCall) polledCalls->Get(curDC);

                If (dc.index == i+1) {
                    curDC++;
                } else {
                    dc = NULL;
                }
            }

            If (DBG_POLL) Log("poll: conn[i=" + i + "]=" +
                    conn+", dc=" + dc);

            If (conn == NULL && dc != NULL) {
                // Connection appeared in CLCC response that we don't know about
                If (mPendingMO != NULL && mPendingMO->CompareTo(dc)) {

                    If (DBG_POLL) Log("poll: pendingMO=" + mPendingMO);

                    // It's our pending mobile originating call
                    mConnections[i] = mPendingMO;
                    mPendingMO.mIndex = i;
                    mPendingMO->Update(dc);
                    mPendingMO = NULL;

                    // Someone has already asked to hangup this call
                    If (mHangupPendingMO) {
                        mHangupPendingMO = FALSE;
                        try {
                            If (Phone.DEBUG_PHONE) Log(
                                    "poll: hangupPendingMO, hangup conn " + i);
                            Hangup(mConnections[i]);
                        } Catch (CallStateException ex) {
                            Rlog->E(LOG_TAG, "unexpected error on hangup");
                        }

                        // Do not continue processing this poll
                        // Wait for hangup and repoll
                        return;
                    }
                } else {
                    mConnections[i] = new GsmConnection(mPhone->GetContext(), dc, this, i);

                    Connection hoConnection = GetHoConnection(dc);
                    If (hoConnection != NULL) {
                        // Single Radio Voice Call Continuity (SRVCC) completed
                        mConnections[i].MigrateFrom(hoConnection);
                        If (!hoConnection->IsMultiparty()) {
                            // Remove only if it is not multiparty
                            mHandoverConnections->Remove(hoConnection);
                        }
                        mPhone->NotifyHandoverStateChanged(mConnections[i]);
                    } else If ( mConnections[i].GetCall() == mRingingCall ) { // it's a ringing call
                        newRinging = mConnections[i];
                    } else {
                        // Something strange happened: a call appeared
                        // which is neither a ringing call or one we created.
                        // Either we've crashed and re-attached to an existing
                        // call, or something Else (eg, SIM) initiated the call.

                        Rlog->I(LOG_TAG,"Phantom call appeared " + dc);

                        // If it's a connected call, set the connect time so that
                        // it's non-zero.  It may not be accurate, but at least
                        // it won't appear as a Missed Call.
                        If (dc.state != DriverCall.State.ALERTING
                                && dc.state != DriverCall.State.DIALING) {
                            mConnections[i].OnConnectedInOrOut();
                            If (dc.state == DriverCall.State.HOLDING) {
                                // We've transitioned into HOLDING
                                mConnections[i].OnStartedHolding();
                            }
                        }

                        newUnknown = mConnections[i];

                        unknownConnectionAppeared = TRUE;
                    }
                }
                hasNonHangupStateChanged = TRUE;
            } else If (conn != NULL && dc == NULL) {
                // Connection missing in CLCC response that we were
                // tracking.
                mDroppedDuringPoll->Add(conn);
                // Dropped connections are removed from the CallTracker
                // list but kept in the GsmCall list
                mConnections[i] = NULL;
            } else If (conn != NULL && dc != NULL && !conn->CompareTo(dc)) {
                // Connection in CLCC response does not match what
                // we were tracking. Assume dropped call and new call

                mDroppedDuringPoll->Add(conn);
                mConnections[i] = new GsmConnection (mPhone->GetContext(), dc, this, i);

                If (mConnections[i].GetCall() == mRingingCall) {
                    newRinging = mConnections[i];
                } // else something strange happened
                hasNonHangupStateChanged = TRUE;
            } else If (conn != NULL && dc != NULL) { /* implicit conn->CompareTo(dc) */
                Boolean changed;
                changed = conn->Update(dc);
                hasNonHangupStateChanged = hasNonHangupStateChanged || changed;
            }

            If (REPEAT_POLLING) {
                If (dc != NULL) {
                    // FIXME with RIL, we should not need this anymore
                    If ((dc.state == DriverCall.State.DIALING
                            /*&& cm->GetOption(cm.OPTION_POLL_DIALING)*/)
                        || (dc.state == DriverCall.State.ALERTING
                            /*&& cm->GetOption(cm.OPTION_POLL_ALERTING)*/)
                        || (dc.state == DriverCall.State.INCOMING
                            /*&& cm->GetOption(cm.OPTION_POLL_INCOMING)*/)
                        || (dc.state == DriverCall.State.WAITING
                            /*&& cm->GetOption(cm.OPTION_POLL_WAITING)*/)
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
        If (mPendingMO != NULL) {
            Rlog->D(LOG_TAG,"Pending MO dropped before poll fg state:"
                            + mForegroundCall->GetState());

            mDroppedDuringPoll->Add(mPendingMO);
            mPendingMO = NULL;
            mHangupPendingMO = FALSE;
        }

        If (newRinging != NULL) {
            mPhone->NotifyNewRingingConnection(newRinging);
        }

        // clear the "local hangup" and "missed/rejected call"
        // cases from the "dropped during poll" list
        // These cases need no "last call fail" reason
        For (Int32 i = mDroppedDuringPoll->Size() - 1; i >= 0 ; i--) {
            GsmConnection conn = mDroppedDuringPoll->Get(i);

            If (conn->IsIncoming() && conn->GetConnectTime() == 0) {
                // Missed or rejected call
                Int32 cause;
                If (conn.mCause == DisconnectCause.LOCAL) {
                    cause = DisconnectCause.INCOMING_REJECTED;
                } else {
                    cause = DisconnectCause.INCOMING_MISSED;
                }

                If (Phone.DEBUG_PHONE) {
                    Log("missed/rejected call, conn.cause=" + conn.mCause);
                    Log("setting cause to " + cause);
                }
                mDroppedDuringPoll->Remove(i);
                hasAnyCallDisconnected |= conn->OnDisconnect(cause);
            } else If (conn.mCause == DisconnectCause.LOCAL
                    || conn.mCause == DisconnectCause.INVALID_NUMBER) {
                mDroppedDuringPoll->Remove(i);
                hasAnyCallDisconnected |= conn->OnDisconnect(conn.mCause);
            }
        }

        // Any non-local disconnects: determine cause
        If (mDroppedDuringPoll->Size() > 0) {
            mCi->GetLastCallFailCause(
                ObtainNoPollCompleteMessage(EVENT_GET_LAST_CALL_FAIL_CAUSE));
        }

        If (needsPollDelay) {
            PollCallsAfterDelay();
        }

        // Cases when we can no longer keep disconnected Connection's
        // with their previous calls
        // 1) the phone has started to ring
        // 2) A Call/Connection object has changed state...
        //    we may have switched or held or Answered (but not hung up)
        If (newRinging != NULL || hasNonHangupStateChanged || hasAnyCallDisconnected) {
            InternalClearDisconnected();
        }

        UpdatePhoneState();

        If (unknownConnectionAppeared) {
            mPhone->NotifyUnknownConnection(newUnknown);
        }

        If (hasNonHangupStateChanged || newRinging != NULL || hasAnyCallDisconnected) {
            mPhone->NotifyPreciseCallStateChanged();
        }

        //DumpState();
    }

    private void
    HandleRadioNotAvailable() {
        // handlePollCalls will clear out its
        // call list when it gets the CommandException
        // error result from this
        PollCallsWhenSafe();
    }

    private void
    DumpState() {
        List l;

        Rlog->I(LOG_TAG,"Phone State:" + mState);

        Rlog->I(LOG_TAG,"Ringing call: " + mRingingCall->ToString());

        l = mRingingCall->GetConnections();
        For (Int32 i = 0, s = l->Size(); i < s; i++) {
            Rlog->I(LOG_TAG,l->Get(i).ToString());
        }

        Rlog->I(LOG_TAG,"Foreground call: " + mForegroundCall->ToString());

        l = mForegroundCall->GetConnections();
        For (Int32 i = 0, s = l->Size(); i < s; i++) {
            Rlog->I(LOG_TAG,l->Get(i).ToString());
        }

        Rlog->I(LOG_TAG,"Background call: " + mBackgroundCall->ToString());

        l = mBackgroundCall->GetConnections();
        For (Int32 i = 0, s = l->Size(); i < s; i++) {
            Rlog->I(LOG_TAG,l->Get(i).ToString());
        }

    }

    //***** Called from GsmConnection

    /*package*/ void
    Hangup (GsmConnection conn) throws CallStateException {
        If (conn.mOwner != this) {
            throw new CallStateException ("GsmConnection " + conn
                                    + "does not belong to GsmCallTracker " + this);
        }

        If (conn == mPendingMO) {
            // We're hanging up an outgoing call that doesn't have it's
            // GSM index assigned yet

            If (Phone.DEBUG_PHONE) Log("hangup: set hangupPendingMO to TRUE");
            mHangupPendingMO = TRUE;
        } else {
            try {
                mCi.HangupConnection (conn->GetGSMIndex(), ObtainCompleteMessage());
            } Catch (CallStateException ex) {
                // Ignore "connection not found"
                // Call may have hung up already
                Rlog->W(LOG_TAG,"GsmCallTracker WARN: Hangup() on absent connection "
                                + conn);
            }
        }

        conn->OnHangupLocal();
    }

    /*package*/ void
    Separate (GsmConnection conn) throws CallStateException {
        If (conn.mOwner != this) {
            throw new CallStateException ("GsmConnection " + conn
                                    + "does not belong to GsmCallTracker " + this);
        }
        try {
            mCi.SeparateConnection (conn->GetGSMIndex(),
                ObtainCompleteMessage(EVENT_SEPARATE_RESULT));
        } Catch (CallStateException ex) {
            // Ignore "connection not found"
            // Call may have hung up already
            Rlog->W(LOG_TAG,"GsmCallTracker WARN: Separate() on absent connection "
                          + conn);
        }
    }

    //***** Called from GSMPhone

    /*package*/ void
    SetMute(Boolean mute) {
        mDesiredMute = mute;
        mCi->SetMute(mDesiredMute, NULL);
    }

    /*package*/ Boolean
    GetMute() {
        return mDesiredMute;
    }


    //***** Called from GsmCall

    /* package */ void
    Hangup (GsmCall call) throws CallStateException {
        If (call->GetConnections()->Size() == 0) {
            throw new CallStateException("no connections in call");
        }

        If (call == mRingingCall) {
            If (Phone.DEBUG_PHONE) Log("(ringing) hangup waiting or background");
            mCi->HangupWaitingOrBackground(ObtainCompleteMessage());
        } else If (call == mForegroundCall) {
            If (call->IsDialingOrAlerting()) {
                If (Phone.DEBUG_PHONE) {
                    Log("(foregnd) hangup dialing or alerting...");
                }
                Hangup((GsmConnection)(call->GetConnections()->Get(0)));
            } else If (mRingingCall->IsRinging()) {
                // Do not auto-answer ringing on CHUP, instead just end active calls
                Log("hangup all conns in active/background call, without affecting ringing call");
                HangupAllConnections(call);
            } else {
                HangupForegroundResumeBackground();
            }
        } else If (call == mBackgroundCall) {
            If (mRingingCall->IsRinging()) {
                If (Phone.DEBUG_PHONE) {
                    Log("hangup all conns in background call");
                }
                HangupAllConnections(call);
            } else {
                HangupWaitingOrBackground();
            }
        } else {
            throw new RuntimeException ("GsmCall " + call +
                    "does not belong to GsmCallTracker " + this);
        }

        call->OnHangupLocal();
        mPhone->NotifyPreciseCallStateChanged();
    }

    /* package */
    void HangupWaitingOrBackground() {
        If (Phone.DEBUG_PHONE) Log("hangupWaitingOrBackground");
        mCi->HangupWaitingOrBackground(ObtainCompleteMessage());
    }

    /* package */
    void HangupForegroundResumeBackground() {
        If (Phone.DEBUG_PHONE) Log("hangupForegroundResumeBackground");
        mCi->HangupForegroundResumeBackground(ObtainCompleteMessage());
    }

    void HangupConnectionByIndex(GsmCall call, Int32 index)
            throws CallStateException {
        Int32 count = call.mConnections->Size();
        For (Int32 i = 0; i < count; i++) {
            GsmConnection cn = (GsmConnection)call.mConnections->Get(i);
            If (cn->GetGSMIndex() == index) {
                mCi->HangupConnection(index, ObtainCompleteMessage());
                return;
            }
        }

        throw new CallStateException("no gsm index found");
    }

    void HangupAllConnections(GsmCall call) {
        try {
            Int32 count = call.mConnections->Size();
            For (Int32 i = 0; i < count; i++) {
                GsmConnection cn = (GsmConnection)call.mConnections->Get(i);
                mCi->HangupConnection(cn->GetGSMIndex(), ObtainCompleteMessage());
            }
        } Catch (CallStateException ex) {
            Rlog->E(LOG_TAG, "hangupConnectionByIndex caught " + ex);
        }
    }

    /* package */
    GsmConnection GetConnectionByIndex(GsmCall call, Int32 index)
            throws CallStateException {
        Int32 count = call.mConnections->Size();
        For (Int32 i = 0; i < count; i++) {
            GsmConnection cn = (GsmConnection)call.mConnections->Get(i);
            If (cn->GetGSMIndex() == index) {
                return cn;
            }
        }

        return NULL;
    }

    private Phone.SuppService GetFailedService(Int32 what) {
        Switch (what) {
            case EVENT_SWITCH_RESULT:
                return Phone.SuppService.SWITCH;
            case EVENT_CONFERENCE_RESULT:
                return Phone.SuppService.CONFERENCE;
            case EVENT_SEPARATE_RESULT:
                return Phone.SuppService.SEPARATE;
            case EVENT_ECT_RESULT:
                return Phone.SuppService.TRANSFER;
        }
        return Phone.SuppService.UNKNOWN;
    }

    //****** Overridden from Handler

    //@Override
    CARAPI
    HandleMessage (Message msg) {
        AsyncResult ar;

        If (!mPhone.mIsTheCurrentActivePhone) {
            Rlog->E(LOG_TAG, "Received message " + msg +
                    "[" + msg.what + "] while being destroyed. Ignoring.");
            return;
        }
        Switch (msg.what) {
            case EVENT_POLL_CALLS_RESULT:
                ar = (AsyncResult)msg.obj;

                If (msg == mLastRelevantPoll) {
                    If (DBG_POLL) Log(
                            "handle EVENT_POLL_CALL_RESULT: set needsPoll=F");
                    mNeedsPoll = FALSE;
                    mLastRelevantPoll = NULL;
                    HandlePollCalls((AsyncResult)msg.obj);
                }
            break;

            case EVENT_OPERATION_COMPLETE:
                ar = (AsyncResult)msg.obj;
                OperationComplete();
            break;

            case EVENT_SWITCH_RESULT:
            case EVENT_CONFERENCE_RESULT:
            case EVENT_SEPARATE_RESULT:
            case EVENT_ECT_RESULT:
                ar = (AsyncResult)msg.obj;
                If (ar.exception != NULL) {
                    mPhone->NotifySuppServiceFailed(GetFailedService(msg.what));
                }
                OperationComplete();
            break;

            case EVENT_GET_LAST_CALL_FAIL_CAUSE:
                Int32 causeCode;
                ar = (AsyncResult)msg.obj;

                OperationComplete();

                If (ar.exception != NULL) {
                    // An exception occurred...just treat the disconnect
                    // cause as "normal"
                    causeCode = CallFailCause.NORMAL_CLEARING;
                    Rlog->I(LOG_TAG,
                            "Exception during getLastCallFailCause, assuming normal disconnect");
                } else {
                    causeCode = ((Int32[])ar.result)[0];
                }
                // Log the causeCode if its not normal
                If (causeCode == CallFailCause.NO_CIRCUIT_AVAIL ||
                    causeCode == CallFailCause.TEMPORARY_FAILURE ||
                    causeCode == CallFailCause.SWITCHING_CONGESTION ||
                    causeCode == CallFailCause.CHANNEL_NOT_AVAIL ||
                    causeCode == CallFailCause.QOS_NOT_AVAIL ||
                    causeCode == CallFailCause.BEARER_NOT_AVAIL ||
                    causeCode == CallFailCause.ERROR_UNSPECIFIED) {
                    GsmCellLocation loc = ((GsmCellLocation)mPhone->GetCellLocation());
                    EventLog->WriteEvent(EventLogTags.CALL_DROP,
                            causeCode, loc != NULL ? loc->GetCid() : -1,
                            TelephonyManager->GetDefault()->GetNetworkType());
                }

                For (Int32 i = 0, s =  mDroppedDuringPoll->Size()
                        ; i < s ; i++
                ) {
                    GsmConnection conn = mDroppedDuringPoll->Get(i);

                    conn->OnRemoteDisconnect(causeCode);
                }

                UpdatePhoneState();

                mPhone->NotifyPreciseCallStateChanged();
                mDroppedDuringPoll->Clear();
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
    }

    //@Override
    protected void Log(String msg) {
        Rlog->D(LOG_TAG, "[GsmCallTracker] " + msg);
    }

    //@Override
    CARAPI Dump(FileDescriptor fd, PrintWriter pw, String[] args) {
        pw->Println("GsmCallTracker extends:");
        super->Dump(fd, pw, args);
        pw->Println("mConnections: length=" + mConnections.length);
        For(Int32 i=0; i < mConnections.length; i++) {
            pw->Printf("  mConnections[%d]=%s\n", i, mConnections[i]);
        }
        pw->Println(" mVoiceCallEndedRegistrants=" + mVoiceCallEndedRegistrants);
        pw->Println(" mVoiceCallStartedRegistrants=" + mVoiceCallStartedRegistrants);
        pw->Println(" mDroppedDuringPoll: size=" + mDroppedDuringPoll->Size());
        For(Int32 i = 0; i < mDroppedDuringPoll->Size(); i++) {
            pw->Printf( "  mDroppedDuringPoll[%d]=%s\n", i, mDroppedDuringPoll->Get(i));
        }
        pw->Println(" mRingingCall=" + mRingingCall);
        pw->Println(" mForegroundCall=" + mForegroundCall);
        pw->Println(" mBackgroundCall=" + mBackgroundCall);
        pw->Println(" mPendingMO=" + mPendingMO);
        pw->Println(" mHangupPendingMO=" + mHangupPendingMO);
        pw->Println(" mPhone=" + mPhone);
        pw->Println(" mDesiredMute=" + mDesiredMute);
        pw->Println(" mState=" + mState);
    }
}
