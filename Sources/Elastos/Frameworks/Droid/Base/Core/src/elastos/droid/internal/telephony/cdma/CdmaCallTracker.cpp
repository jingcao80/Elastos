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

using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IRegistrant;
using Elastos::Droid::Os::IRegistrantList;
using Elastos::Droid::Telephony::IDisconnectCause;
using Elastos::Droid::Telephony::IPhoneNumberUtils;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Droid::Telephony::IRlog;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Text::ITextUtils;

using Elastos::Droid::Internal::Telephony::ICallTracker;
using Elastos::Droid::Internal::Telephony::ICommandsInterface;
using Elastos::Droid::Internal::Telephony::IConnection;
using Elastos::Droid::Internal::Telephony::IDriverCall;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Internal::Telephony::IPhoneBase;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Internal::Telephony::ITelephonyProperties;

using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;


/**
 * {@hide}
 */
public class CdmaCallTracker extends CallTracker {
    static const String LOG_TAG = "CdmaCallTracker";

    private static const Boolean REPEAT_POLLING = FALSE;

    private static const Boolean DBG_POLL = FALSE;

    //***** Constants

    static const Int32 MAX_CONNECTIONS = 8;
    static const Int32 MAX_CONNECTIONS_PER_CALL = 1; // only 1 connection allowed per call

    //***** Instance Variables

    CdmaConnection mConnections[] = new CdmaConnection[MAX_CONNECTIONS];
    RegistrantList mVoiceCallEndedRegistrants = new RegistrantList();
    RegistrantList mVoiceCallStartedRegistrants = new RegistrantList();
    RegistrantList mCallWaitingRegistrants =  new RegistrantList();


    // connections dropped during last poll
    ArrayList<CdmaConnection> mDroppedDuringPoll
        = new ArrayList<CdmaConnection>(MAX_CONNECTIONS);

    CdmaCall mRingingCall = new CdmaCall(this);
    // A call that is ringing Or (call) waiting
    CdmaCall mForegroundCall = new CdmaCall(this);
    CdmaCall mBackgroundCall = new CdmaCall(this);

    CdmaConnection mPendingMO;
    Boolean mHangupPendingMO;
    Boolean mPendingCallInEcm=FALSE;
    Boolean mIsInEmergencyCall = FALSE;
    CDMAPhone mPhone;

    Boolean mDesiredMute = FALSE;    // FALSE = mute off

    Int32 mPendingCallClirMode;
    PhoneConstants.State mState = PhoneConstants.State.IDLE;

    private Boolean mIsEcmTimerCanceled = FALSE;

    private Int32 m3WayCallFlashDelay = 0;
//    Boolean needsPoll;



    //***** Events

    //***** Constructors
    CdmaCallTracker(CDMAPhone phone) {
        mPhone = phone;
        mCi = phone.mCi;
        mCi->RegisterForCallStateChanged(this, EVENT_CALL_STATE_CHANGE, NULL);
        mCi->RegisterForOn(this, EVENT_RADIO_AVAILABLE, NULL);
        mCi->RegisterForNotAvailable(this, EVENT_RADIO_NOT_AVAILABLE, NULL);
        mCi->RegisterForCallWaitingInfo(this, EVENT_CALL_WAITING_INFO_CDMA, NULL);
        mForegroundCall->SetGeneric(FALSE);
    }

    CARAPI Dispose() {
        Rlog->D(LOG_TAG, "CdmaCallTracker dispose");
        mCi->UnregisterForLineControlInfo(this);
        mCi->UnregisterForCallStateChanged(this);
        mCi->UnregisterForOn(this);
        mCi->UnregisterForNotAvailable(this);
        mCi->UnregisterForCallWaitingInfo(this);

        ClearDisconnected();

    }

    //@Override
    protected void Finalize() {
        Rlog->D(LOG_TAG, "CdmaCallTracker finalized");
    }

    //***** Instance Methods

    //***** Public Methods
    //@Override
    CARAPI RegisterForVoiceCallStarted(Handler h, Int32 what, Object obj) {
        Registrant r = new Registrant(h, what, obj);
        mVoiceCallStartedRegistrants->Add(r);
        // Notify if in call when registering
        If (mState != PhoneConstants.State.IDLE) {
            r->NotifyRegistrant(new AsyncResult(NULL, NULL, NULL));
        }
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

    CARAPI RegisterForCallWaiting(Handler h, Int32 what, Object obj) {
        Registrant r = new Registrant (h, what, obj);
        mCallWaitingRegistrants->Add(r);
    }

    CARAPI UnregisterForCallWaiting(Handler h) {
        mCallWaitingRegistrants->Remove(h);
    }

    /**
     * clirMode is one of the CLIR_ constants
     */
    Connection
    Dial (String dialString, Int32 clirMode) throws CallStateException {
        // note that this triggers call state changed notif
        ClearDisconnected();

        If (!CanDial()) {
            throw new CallStateException("cannot dial in current state");
        }

        String origNumber = dialString;
        String operatorIsoContry = mPhone->GetSystemProperty(
                TelephonyProperties.PROPERTY_OPERATOR_ISO_COUNTRY, "");
        String simIsoContry = mPhone->GetSystemProperty(
                TelephonyProperties.PROPERTY_ICC_OPERATOR_ISO_COUNTRY, "");
        Boolean internationalRoaming = !TextUtils->IsEmpty(operatorIsoContry)
                && !TextUtils->IsEmpty(simIsoContry)
                && !simIsoContry->Equals(operatorIsoContry);
        If (internationalRoaming) {
            If ("us".Equals(simIsoContry)) {
                internationalRoaming = internationalRoaming && !"vi".Equals(operatorIsoContry);
            } else If ("vi".Equals(simIsoContry)) {
                internationalRoaming = internationalRoaming && !"us".Equals(operatorIsoContry);
            }
        }
        If (internationalRoaming) {
            dialString = ConvertNumberIfNecessary(mPhone, dialString);
        }

        String inEcm=SystemProperties->Get(TelephonyProperties.PROPERTY_INECM_MODE, "FALSE");
        Boolean isPhoneInEcmMode = inEcm->Equals("TRUE");
        Boolean isEmergencyCall =
                PhoneNumberUtils->IsLocalEmergencyNumber(mPhone->GetContext(), dialString);

        // Cancel Ecm timer if a second emergency call is originating in Ecm mode
        If (isPhoneInEcmMode && isEmergencyCall) {
            HandleEcmTimer(CDMAPhone.CANCEL_ECM_TIMER);
        }

        // We are initiating a call therefore even if we previously
        // didn't know the State (i.e. Generic was TRUE) we now know
        // and therefore can set Generic to FALSE.
        mForegroundCall->SetGeneric(FALSE);

        // The new call must be assigned to the foreground call.
        // That call must be idle, so place anything that's
        // there on hold
        If (mForegroundCall->GetState() == CdmaCall.State.ACTIVE) {
            return DialThreeWay(dialString);
        }

        mPendingMO = new CdmaConnection(mPhone->GetContext(), CheckForTestEmergencyNumber(dialString),
                this, mForegroundCall);
        mHangupPendingMO = FALSE;

        If ( mPendingMO->GetAddress() == NULL || mPendingMO->GetAddress()->Length() == 0
                || mPendingMO->GetAddress()->IndexOf(PhoneNumberUtils.WILD) >= 0 ) {
            // Phone number is invalid
            mPendingMO.mCause = DisconnectCause.INVALID_NUMBER;

            // HandlePollCalls() will notice this call not present
            // and will mark it as dropped.
            PollCallsWhenSafe();
        } else {
            // Always unmute when initiating a new call
            SetMute(FALSE);

            // Check data call
            DisableDataCallInEmergencyCall(dialString);

            // In Ecm mode, if another emergency call is dialed, Ecm mode will not exit.
            If(!isPhoneInEcmMode || (isPhoneInEcmMode && isEmergencyCall)) {
                mCi->Dial(mPendingMO->GetAddress(), clirMode, ObtainCompleteMessage());
            } else {
                mPhone->ExitEmergencyCallbackMode();
                mPhone->SetOnEcbModeExitResponse(this,EVENT_EXIT_ECM_RESPONSE_CDMA, NULL);
                mPendingCallClirMode=clirMode;
                mPendingCallInEcm=TRUE;
            }
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
    Dial (String dialString) throws CallStateException {
        return Dial(dialString, CommandsInterface.CLIR_DEFAULT);
    }

    private Connection
    DialThreeWay (String dialString) {
        If (!mForegroundCall->IsIdle()) {
            // Check data call
            DisableDataCallInEmergencyCall(dialString);

            // Attach the new connection to foregroundCall
            mPendingMO = new CdmaConnection(mPhone->GetContext(),
                                CheckForTestEmergencyNumber(dialString), this, mForegroundCall);
            // Some network need a empty flash before sending the normal one
            m3WayCallFlashDelay = mPhone->GetContext()->GetResources()
                    .GetInteger(R.integer.config_cdma_3waycall_flash_delay);
            If (m3WayCallFlashDelay > 0) {
                mCi->SendCDMAFeatureCode("", ObtainMessage(EVENT_THREE_WAY_DIAL_BLANK_FLASH));
            } else {
                mCi->SendCDMAFeatureCode(mPendingMO->GetAddress(),
                        ObtainMessage(EVENT_THREE_WAY_DIAL_L2_RESULT_CDMA));
            }
            return mPendingMO;
        }
        return NULL;
    }

    void
    AcceptCall() throws CallStateException {
        If (mRingingCall->GetState() == CdmaCall.State.INCOMING) {
            Rlog->I("phone", "acceptCall: incoming...");
            // Always unmute when answering a new call
            SetMute(FALSE);
            mCi->AcceptCall(ObtainCompleteMessage());
        } else If (mRingingCall->GetState() == CdmaCall.State.WAITING) {
            CdmaConnection cwConn = (CdmaConnection)(mRingingCall->GetLatestConnection());

            // Since there is no network response for supplimentary
            // service for CDMA, we assume call waiting is answered.
            // ringing Call state change to idle is in CdmaCall.detach
            // triggered by updateParent.
            cwConn->UpdateParent(mRingingCall, mForegroundCall);
            cwConn->OnConnectedInOrOut();
            UpdatePhoneState();
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
        If (mRingingCall->GetState() == CdmaCall.State.INCOMING) {
            throw new CallStateException("cannot be in the incoming state");
        } else If (mForegroundCall->GetConnections()->Size() > 1) {
            FlashAndSetGenericTrue();
        } else {
            // Send a flash command to CDMA network for putting the other party on hold.
            // For CDMA networks which do not support this the user would just hear a beep
            // from the network. For CDMA networks which do support it will put the other
            // party on hold.
            mCi->SendCDMAFeatureCode("", ObtainMessage(EVENT_SWITCH_RESULT));
        }
    }

    void
    Conference() {
        // Should we be checking state?
        FlashAndSetGenericTrue();
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
        return mForegroundCall->GetState() == CdmaCall.State.ACTIVE
                && mBackgroundCall->GetState() == CdmaCall.State.HOLDING
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
                    || (mForegroundCall->GetState() == CdmaCall.State.ACTIVE)
                    || !mBackgroundCall->GetState()->IsAlive());

        If (!ret) {
            Log(String->Format("canDial is FALSE\n" +
                              "((serviceState=%d) != ServiceState.STATE_POWER_OFF)::=%s\n" +
                              "&& pendingMO == NULL::=%s\n" +
                              "&& !ringingCall->IsRinging()::=%s\n" +
                              "&& !disableCall->Equals(\"TRUE\")::=%s\n" +
                              "&& (!foregroundCall->GetState()->IsAlive()::=%s\n" +
                              "   || foregroundCall->GetState() == CdmaCall.State.ACTIVE::=%s\n" +
                              "   ||!backgroundCall->GetState()->IsAlive())::=%s)",
                    serviceState,
                    serviceState != ServiceState.STATE_POWER_OFF,
                    mPendingMO == NULL,
                    !mRingingCall->IsRinging(),
                    !disableCall->Equals("TRUE"),
                    !mForegroundCall->GetState()->IsAlive(),
                    mForegroundCall->GetState() == CdmaCall.State.ACTIVE,
                    !mBackgroundCall->GetState()->IsAlive()));
        }
        return ret;
    }

    Boolean
    CanTransfer() {
        Rlog->E(LOG_TAG, "canTransfer: not possible in CDMA");
        return FALSE;
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
            Rlog->E(LOG_TAG,"CdmaCallTracker.pendingOperations < 0");
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
        If (Phone.DEBUG_PHONE) {
            Log("update phone state, old=" + oldState + " new="+ mState);
        }
        If (mState != oldState) {
            mPhone->NotifyPhoneStateChanged();
        }
    }

    // ***** Overwritten from CallTracker

    //@Override
    protected void
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
            CdmaConnection conn = mConnections[i];
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
                        // Re-start Ecm timer when an uncompleted emergency call ends
                        If (mIsEcmTimerCanceled) {
                            HandleEcmTimer(CDMAPhone.RESTART_ECM_TIMER);
                        }

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
                    If (Phone.DEBUG_PHONE) {
                        Log("pendingMo=" + mPendingMO + ", dc=" + dc);
                    }
                    mConnections[i] = new CdmaConnection(mPhone->GetContext(), dc, this, i);

                    Connection hoConnection = GetHoConnection(dc);
                    If (hoConnection != NULL) {
                        // Single Radio Voice Call Continuity (SRVCC) completed
                        mConnections[i].MigrateFrom(hoConnection);
                        mHandoverConnections->Remove(hoConnection);
                        mPhone->NotifyHandoverStateChanged(mConnections[i]);
                    } else {
                        // find if the MT call is a new ring or unknown connection
                        newRinging = CheckMtFindNewRinging(dc,i);
                        If (newRinging == NULL) {
                            unknownConnectionAppeared = TRUE;
                            newUnknown = mConnections[i];
                        }
                    }
                    CheckAndEnableDataCallAfterEmergencyCallDropped();
                }
                hasNonHangupStateChanged = TRUE;
            } else If (conn != NULL && dc == NULL) {
                // This case means the RIL has no more active call anymore and
                // we need to clean up the foregroundCall and ringingCall.
                // Loop through foreground call connections as
                // it contains the known logical connections.
                Int32 count = mForegroundCall.mConnections->Size();
                For (Int32 n = 0; n < count; n++) {
                    If (Phone.DEBUG_PHONE) Log("adding fgCall cn " + n + " to droppedDuringPoll");
                    CdmaConnection cn = (CdmaConnection)mForegroundCall.mConnections->Get(n);
                    mDroppedDuringPoll->Add(cn);
                }
                count = mRingingCall.mConnections->Size();
                // Loop through ringing call connections as
                // it may contain the known logical connections.
                For (Int32 n = 0; n < count; n++) {
                    If (Phone.DEBUG_PHONE) Log("adding rgCall cn " + n + " to droppedDuringPoll");
                    CdmaConnection cn = (CdmaConnection)mRingingCall.mConnections->Get(n);
                    mDroppedDuringPoll->Add(cn);
                }
                mForegroundCall->SetGeneric(FALSE);
                mRingingCall->SetGeneric(FALSE);

                // Re-start Ecm timer when the connected emergency call ends
                If (mIsEcmTimerCanceled) {
                    HandleEcmTimer(CDMAPhone.RESTART_ECM_TIMER);
                }
                // If emergency call is not going through while dialing
                CheckAndEnableDataCallAfterEmergencyCallDropped();

                // Dropped connections are removed from the CallTracker
                // list but kept in the Call list
                mConnections[i] = NULL;
            } else If (conn != NULL && dc != NULL) { /* implicit conn->CompareTo(dc) */
                // Call collision case
                If (conn->IsIncoming() != dc.isMT) {
                    If (dc.isMT == TRUE){
                        // Mt call takes precedence than Mo,drops Mo
                        mDroppedDuringPoll->Add(conn);
                        // find if the MT call is a new ring or unknown connection
                        newRinging = CheckMtFindNewRinging(dc,i);
                        If (newRinging == NULL) {
                            unknownConnectionAppeared = TRUE;
                            newUnknown = conn;
                        }
                        CheckAndEnableDataCallAfterEmergencyCallDropped();
                    } else {
                        // Call info stored in conn is not consistent with the call info from dc.
                        // We should follow the rule of MT calls taking precedence over MO calls
                        // when there is conflict, so here we drop the call info from dc and
                        // continue to use the call info from conn, and only take a log.
                        Rlog->E(LOG_TAG,"Error in RIL, Phantom call appeared " + dc);
                    }
                } else {
                    Boolean changed;
                    changed = conn->Update(dc);
                    hasNonHangupStateChanged = hasNonHangupStateChanged || changed;
                }
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
            If( mPendingCallInEcm) {
                mPendingCallInEcm = FALSE;
            }
            CheckAndEnableDataCallAfterEmergencyCallDropped();
        }

        If (newRinging != NULL) {
            mPhone->NotifyNewRingingConnection(newRinging);
        }

        // clear the "local hangup" and "missed/rejected call"
        // cases from the "dropped during poll" list
        // These cases need no "last call fail" reason
        For (Int32 i = mDroppedDuringPoll->Size() - 1; i >= 0 ; i--) {
            CdmaConnection conn = mDroppedDuringPoll->Get(i);

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

    //***** Called from CdmaConnection
    /*package*/ void
    Hangup (CdmaConnection conn) throws CallStateException {
        If (conn.mOwner != this) {
            throw new CallStateException ("CdmaConnection " + conn
                                    + "does not belong to CdmaCallTracker " + this);
        }

        If (conn == mPendingMO) {
            // We're hanging up an outgoing call that doesn't have it's
            // GSM index assigned yet

            If (Phone.DEBUG_PHONE) Log("hangup: set hangupPendingMO to TRUE");
            mHangupPendingMO = TRUE;
        } else If ((conn->GetCall() == mRingingCall)
                && (mRingingCall->GetState() == CdmaCall.State.WAITING)) {
            // Handle call waiting hang up case.
            //
            // The ringingCall state will change to IDLE in CdmaCall.detach
            // if the ringing call connection size is 0. We don't specifically
            // set the ringing call state to IDLE here to avoid a race condition
            // where a new call waiting could get a hang up from an old call
            // waiting ringingCall.
            //
            // PhoneApp does the call log itself since only PhoneApp knows
            // the hangup reason is user ignoring or timing out. So conn->OnDisconnect()
            // is not called here. Instead, conn->OnLocalDisconnect() is called.
            conn->OnLocalDisconnect();
            UpdatePhoneState();
            mPhone->NotifyPreciseCallStateChanged();
            return;
        } else {
            try {
                mCi.HangupConnection (conn->GetCDMAIndex(), ObtainCompleteMessage());
            } Catch (CallStateException ex) {
                // Ignore "connection not found"
                // Call may have hung up already
                Rlog->W(LOG_TAG,"CdmaCallTracker WARN: Hangup() on absent connection "
                                + conn);
            }
        }

        conn->OnHangupLocal();
    }

    /*package*/ void
    Separate (CdmaConnection conn) throws CallStateException {
        If (conn.mOwner != this) {
            throw new CallStateException ("CdmaConnection " + conn
                                    + "does not belong to CdmaCallTracker " + this);
        }
        try {
            mCi.SeparateConnection (conn->GetCDMAIndex(),
                ObtainCompleteMessage(EVENT_SEPARATE_RESULT));
        } Catch (CallStateException ex) {
            // Ignore "connection not found"
            // Call may have hung up already
            Rlog->W(LOG_TAG,"CdmaCallTracker WARN: Separate() on absent connection "
                          + conn);
        }
    }

    //***** Called from CDMAPhone

    /*package*/ void
    SetMute(Boolean mute) {
        mDesiredMute = mute;
        mCi->SetMute(mDesiredMute, NULL);
    }

    /*package*/ Boolean
    GetMute() {
        return mDesiredMute;
    }


    //***** Called from CdmaCall

    /* package */ void
    Hangup (CdmaCall call) throws CallStateException {
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
                Hangup((CdmaConnection)(call->GetConnections()->Get(0)));
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
            throw new RuntimeException ("CdmaCall " + call +
                    "does not belong to CdmaCallTracker " + this);
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

    void HangupConnectionByIndex(CdmaCall call, Int32 index)
            throws CallStateException {
        Int32 count = call.mConnections->Size();
        For (Int32 i = 0; i < count; i++) {
            CdmaConnection cn = (CdmaConnection)call.mConnections->Get(i);
            If (cn->GetCDMAIndex() == index) {
                mCi->HangupConnection(index, ObtainCompleteMessage());
                return;
            }
        }

        throw new CallStateException("no gsm index found");
    }

    void HangupAllConnections(CdmaCall call) {
        try {
            Int32 count = call.mConnections->Size();
            For (Int32 i = 0; i < count; i++) {
                CdmaConnection cn = (CdmaConnection)call.mConnections->Get(i);
                mCi->HangupConnection(cn->GetCDMAIndex(), ObtainCompleteMessage());
            }
        } Catch (CallStateException ex) {
            Rlog->E(LOG_TAG, "hangupConnectionByIndex caught " + ex);
        }
    }

    /* package */
    CdmaConnection GetConnectionByIndex(CdmaCall call, Int32 index)
            throws CallStateException {
        Int32 count = call.mConnections->Size();
        For (Int32 i = 0; i < count; i++) {
            CdmaConnection cn = (CdmaConnection)call.mConnections->Get(i);
            If (cn->GetCDMAIndex() == index) {
                return cn;
            }
        }

        return NULL;
    }

    private void FlashAndSetGenericTrue() {
        mCi->SendCDMAFeatureCode("", ObtainMessage(EVENT_SWITCH_RESULT));

        // Set generic to TRUE because in CDMA it is not known what
        // the status of the call is after a call waiting is answered,
        // 3 way call merged or a switch between calls.
        mForegroundCall->SetGeneric(TRUE);
        mPhone->NotifyPreciseCallStateChanged();
    }

    private void HandleRadioNotAvailable() {
        // handlePollCalls will clear out its
        // call list when it gets the CommandException
        // error result from this
        PollCallsWhenSafe();
    }

    private void NotifyCallWaitingInfo(CdmaCallWaitingNotification obj) {
        If (mCallWaitingRegistrants != NULL) {
            mCallWaitingRegistrants->NotifyRegistrants(new AsyncResult(NULL, obj, NULL));
        }
    }

    private void HandleCallWaitingInfo (CdmaCallWaitingNotification cw) {
        // Check how many connections in foregroundCall.
        // If the connection in foregroundCall is more
        // than one, then the connection information is
        // not reliable anymore since it means either
        // call waiting is connected or 3 way call is
        // dialed before, so set generic.
        If (mForegroundCall.mConnections->Size() > 1 ) {
            mForegroundCall->SetGeneric(TRUE);
        }

        // Create a new CdmaConnection which attaches itself to ringingCall.
        mRingingCall->SetGeneric(FALSE);
        new CdmaConnection(mPhone->GetContext(), cw, this, mRingingCall);
        UpdatePhoneState();

        // Finally notify application
        NotifyCallWaitingInfo(cw);
    }
    //****** Overridden from Handler

    //@Override
    CARAPI
    HandleMessage (Message msg) {
        AsyncResult ar;

        If (!mPhone.mIsTheCurrentActivePhone) {
            Rlog->W(LOG_TAG, "Ignoring events received on inactive CdmaPhone");
            return;
        }
        Switch (msg.what) {
            case EVENT_POLL_CALLS_RESULT:{
                Rlog->D(LOG_TAG, "Event EVENT_POLL_CALLS_RESULT Received");
                ar = (AsyncResult)msg.obj;

                If(msg == mLastRelevantPoll) {
                    If(DBG_POLL) Log(
                            "handle EVENT_POLL_CALL_RESULT: set needsPoll=F");
                    mNeedsPoll = FALSE;
                    mLastRelevantPoll = NULL;
                    HandlePollCalls((AsyncResult)msg.obj);
                }
            }
            break;

            case EVENT_OPERATION_COMPLETE:
                OperationComplete();
            break;

            case EVENT_SWITCH_RESULT:
                 // In GSM call OperationComplete() here which gets the
                 // current call list. But in CDMA there is no list so
                 // there is nothing to do.
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

                For (Int32 i = 0, s =  mDroppedDuringPoll->Size()
                        ; i < s ; i++
                ) {
                    CdmaConnection conn = mDroppedDuringPoll->Get(i);

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

            case EVENT_EXIT_ECM_RESPONSE_CDMA:
                // no matter the result, we still do the same here
                If ( mPendingCallInEcm ) {
                    mCi->Dial(mPendingMO->GetAddress(), mPendingCallClirMode, ObtainCompleteMessage());
                    mPendingCallInEcm = FALSE;
                }
                mPhone->UnsetOnEcbModeExitResponse(this);
            break;

            case EVENT_CALL_WAITING_INFO_CDMA:
               ar = (AsyncResult)msg.obj;
               If (ar.exception == NULL) {
                   HandleCallWaitingInfo((CdmaCallWaitingNotification)ar.result);
                   Rlog->D(LOG_TAG, "Event EVENT_CALL_WAITING_INFO_CDMA Received");
               }
            break;

            case EVENT_THREE_WAY_DIAL_L2_RESULT_CDMA:
                ar = (AsyncResult)msg.obj;
                If (ar.exception == NULL) {
                    // Assume 3 way call is connected
                    mPendingMO->OnConnectedInOrOut();
                    mPendingMO = NULL;
                }
            break;

            case EVENT_THREE_WAY_DIAL_BLANK_FLASH:
                ar = (AsyncResult) msg.obj;
                If (ar.exception == NULL) {
                    PostDelayed(
                            new Runnable() {
                                CARAPI Run() {
                                    If (mPendingMO != NULL) {
                                        mCi->SendCDMAFeatureCode(mPendingMO->GetAddress(),
                                                ObtainMessage(EVENT_THREE_WAY_DIAL_L2_RESULT_CDMA));
                                    }
                                }
                            }, m3WayCallFlashDelay);
                } else {
                    mPendingMO = NULL;
                    Rlog->W(LOG_TAG, "exception happened on Blank Flash for 3-way call");
                }
            break;

            default:{
               throw new RuntimeException("unexpected event not handled");
            }
        }
    }

    /**
     * Handle Ecm timer to be canceled or re-started
     */
    private void HandleEcmTimer(Int32 action) {
        mPhone->HandleTimerInEmergencyCallbackMode(action);
        Switch(action) {
        case CDMAPhone.CANCEL_ECM_TIMER: mIsEcmTimerCanceled = TRUE; break;
        case CDMAPhone.RESTART_ECM_TIMER: mIsEcmTimerCanceled = FALSE; break;
        default:
            Rlog->E(LOG_TAG, "handleEcmTimer, unsupported action " + action);
        }
    }

    /**
     * Disable data call when emergency call is connected
     */
    private void DisableDataCallInEmergencyCall(String dialString) {
        If (PhoneNumberUtils->IsLocalEmergencyNumber(mPhone->GetContext(), dialString)) {
            If (Phone.DEBUG_PHONE) Log("disableDataCallInEmergencyCall");
            mIsInEmergencyCall = TRUE;
            mPhone.mDcTracker->SetInternalDataEnabled(FALSE);
        }
    }

    /**
     * Check and enable data call after an emergency call is dropped if it's
     * not in ECM
     */
    private void CheckAndEnableDataCallAfterEmergencyCallDropped() {
        If (mIsInEmergencyCall) {
            mIsInEmergencyCall = FALSE;
            String inEcm=SystemProperties->Get(TelephonyProperties.PROPERTY_INECM_MODE, "FALSE");
            If (Phone.DEBUG_PHONE) {
                Log("checkAndEnableDataCallAfterEmergencyCallDropped,inEcm=" + inEcm);
            }
            If (inEcm->CompareTo("FALSE") == 0) {
                // Re-initiate data connection
                mPhone.mDcTracker->SetInternalDataEnabled(TRUE);
            }
        }
    }

    /**
     * Check the MT call to see if it's a new ring or
     * a unknown connection.
     */
    private Connection CheckMtFindNewRinging(DriverCall dc, Int32 i) {

        Connection newRinging = NULL;

        // it's a ringing call
        If (mConnections[i].GetCall() == mRingingCall) {
            newRinging = mConnections[i];
            If (Phone.DEBUG_PHONE) Log("Notify new ring " + dc);
        } else {
            // Something strange happened: a call which is neither
            // a ringing call nor the one we created. It could be the
            // call collision result from RIL
            Rlog->E(LOG_TAG,"Phantom call appeared " + dc);
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
        }
        return newRinging;
    }

    /**
     * Check if current call is in emergency call
     *
     * @return TRUE if it is in emergency call
     *         FALSE if it is not in emergency call
     */
    Boolean IsInEmergencyCall() {
        return mIsInEmergencyCall;
    }

    //@Override
    protected void Log(String msg) {
        Rlog->D(LOG_TAG, "[CdmaCallTracker] " + msg);
    }

    //@Override
    CARAPI Dump(FileDescriptor fd, PrintWriter pw, String[] args) {
        pw->Println("GsmCallTracker extends:");
        super->Dump(fd, pw, args);
        pw->Println("droppedDuringPoll: length=" + mConnections.length);
        For(Int32 i=0; i < mConnections.length; i++) {
            pw->Printf(" mConnections[%d]=%s\n", i, mConnections[i]);
        }
        pw->Println(" mVoiceCallEndedRegistrants=" + mVoiceCallEndedRegistrants);
        pw->Println(" mVoiceCallStartedRegistrants=" + mVoiceCallStartedRegistrants);
        pw->Println(" mCallWaitingRegistrants=" + mCallWaitingRegistrants);
        pw->Println("droppedDuringPoll: size=" + mDroppedDuringPoll->Size());
        For(Int32 i = 0; i < mDroppedDuringPoll->Size(); i++) {
            pw->Printf( " mDroppedDuringPoll[%d]=%s\n", i, mDroppedDuringPoll->Get(i));
        }
        pw->Println(" mRingingCall=" + mRingingCall);
        pw->Println(" mForegroundCall=" + mForegroundCall);
        pw->Println(" mBackgroundCall=" + mBackgroundCall);
        pw->Println(" mPendingMO=" + mPendingMO);
        pw->Println(" mHangupPendingMO=" + mHangupPendingMO);
        pw->Println(" mPendingCallInEcm=" + mPendingCallInEcm);
        pw->Println(" mIsInEmergencyCall=" + mIsInEmergencyCall);
        pw->Println(" mPhone=" + mPhone);
        pw->Println(" mDesiredMute=" + mDesiredMute);
        pw->Println(" mPendingCallClirMode=" + mPendingCallClirMode);
        pw->Println(" mState=" + mState);
        pw->Println(" mIsEcmTimerCanceled=" + mIsEcmTimerCanceled);
    }
}
