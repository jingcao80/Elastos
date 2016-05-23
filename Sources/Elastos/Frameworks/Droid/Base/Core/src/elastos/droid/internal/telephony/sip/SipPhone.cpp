/*
 * Copyright (C) 2010 The Android Open Source Project
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

package com.android.internal.telephony.sip;

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Net::Rtp::IAudioGroup;
using Elastos::Droid::Net::Sip::ISipAudioCall;
using Elastos::Droid::Net::Sip::ISipErrorCode;
using Elastos::Droid::Net::Sip::ISipManager;
using Elastos::Droid::Net::Sip::ISipProfile;
using Elastos::Droid::Net::Sip::ISipSession;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Telephony::IDisconnectCause;
using Elastos::Droid::Telephony::IPhoneNumberUtils;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Telephony::IRlog;

using Elastos::Droid::Internal::Telephony::ICall;
using Elastos::Droid::Internal::Telephony::IConnection;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Internal::Telephony::IPhoneNotifier;

using Elastos::Utility::IList;
using Elastos::Utility::Regex::IPattern;

/**
 * {@hide}
 */
public class SipPhone extends SipPhoneBase {
    private static const String LOG_TAG = "SipPhone";
    private static const Boolean DBG = TRUE;
    private static const Boolean VDBG = FALSE; // STOPSHIP if TRUE
    private static const Int32 TIMEOUT_MAKE_CALL = 15; // in seconds
    private static const Int32 TIMEOUT_ANSWER_CALL = 8; // in seconds
    private static const Int32 TIMEOUT_HOLD_CALL = 15; // in seconds

    // A call that is ringing Or (call) waiting
    private SipCall mRingingCall = new SipCall();
    private SipCall mForegroundCall = new SipCall();
    private SipCall mBackgroundCall = new SipCall();

    private SipManager mSipManager;
    private SipProfile mProfile;

    SipPhone (Context context, PhoneNotifier notifier, SipProfile profile) {
        Super("SIP:" + profile->GetUriString(), context, notifier);

        If (DBG) Log("new SipPhone: " + profile->GetUriString());
        mRingingCall = new SipCall();
        mForegroundCall = new SipCall();
        mBackgroundCall = new SipCall();
        mProfile = profile;
        mSipManager = SipManager->NewInstance(context);
    }

    //@Override
    public Boolean Equals(Object o) {
        If (o == this) return TRUE;
        If (!(o instanceof SipPhone)) return FALSE;
        SipPhone that = (SipPhone) o;
        return mProfile->GetUriString()->Equals(that.mProfile->GetUriString());
    }

    public String GetSipUri() {
        return mProfile->GetUriString();
    }

    public Boolean Equals(SipPhone phone) {
        return GetSipUri()->Equals(phone->GetSipUri());
    }

    public Connection TakeIncomingCall(Object incomingCall) {
        // FIXME: Is synchronizing on the class necessary, should we use a mLockObj?
        // Also there are many things not synchronized, of course
        // this may be TRUE of CdmaPhone and GsmPhone too!!!
        {    AutoLock syncLock(SipPhone.class);
            If (!(incomingCall instanceof SipAudioCall)) {
                If (DBG) Log("takeIncomingCall: ret=NULL, not a SipAudioCall");
                return NULL;
            }
            If (mRingingCall->GetState()->IsAlive()) {
                If (DBG) Log("takeIncomingCall: ret=NULL, ringingCall not alive");
                return NULL;
            }

            // FIXME: is it TRUE that we cannot take any incoming call if
            // both foreground and background are active
            If (mForegroundCall->GetState()->IsAlive()
                    && mBackgroundCall->GetState()->IsAlive()) {
                If (DBG) {
                    Log("takeIncomingCall: ret=NULL," + " foreground and background both alive");
                }
                return NULL;
            }

            try {
                SipAudioCall sipAudioCall = (SipAudioCall) incomingCall;
                If (DBG) Log("takeIncomingCall: taking call from: "
                        + sipAudioCall->GetPeerProfile()->GetUriString());
                String localUri = sipAudioCall->GetLocalProfile()->GetUriString();
                If (localUri->Equals(mProfile->GetUriString())) {
                    Boolean makeCallWait = mForegroundCall->GetState()->IsAlive();
                    SipConnection connection = mRingingCall->InitIncomingCall(sipAudioCall,
                            makeCallWait);
                    If (sipAudioCall->GetState() != SipSession.State.INCOMING_CALL) {
                        // Peer cancelled the call!
                        If (DBG) Log("    takeIncomingCall: call cancelled !!");
                        mRingingCall->Reset();
                        connection = NULL;
                    }
                    return connection;
                }
            } Catch (Exception e) {
                // Peer may cancel the call at any time during the time we hook
                // up ringingCall with sipAudioCall. Clean up ringingCall when
                // that happens.
                If (DBG) Log("    takeIncomingCall: exception e=" + e);
                mRingingCall->Reset();
            }
            If (DBG) Log("takeIncomingCall: NOT taking !!");
            return NULL;
        }
    }

    //@Override
    CARAPI AcceptCall(Int32 videoState) throws CallStateException {
        {    AutoLock syncLock(SipPhone.class);
            If ((mRingingCall->GetState() == Call.State.INCOMING) ||
                    (mRingingCall->GetState() == Call.State.WAITING)) {
                If (DBG) Log("acceptCall: accepting");
                // Always unmute when answering a new call
                mRingingCall->SetMute(FALSE);
                mRingingCall->AcceptCall();
            } else {
                If (DBG) {
                    Log("acceptCall:" +
                        " throw CallStateException(\"phone not ringing\")");
                }
                throw new CallStateException("phone not ringing");
            }
        }
    }

    //@Override
    CARAPI RejectCall() throws CallStateException {
        {    AutoLock syncLock(SipPhone.class);
            If (mRingingCall->GetState()->IsRinging()) {
                If (DBG) Log("rejectCall: rejecting");
                mRingingCall->RejectCall();
            } else {
                If (DBG) {
                    Log("rejectCall:" +
                        " throw CallStateException(\"phone not ringing\")");
                }
                throw new CallStateException("phone not ringing");
            }
        }
    }

    //@Override
    public Connection Dial(String dialString, Int32 videoState) throws CallStateException {
        {    AutoLock syncLock(SipPhone.class);
            return DialInternal(dialString, videoState);
        }
    }

    private Connection DialInternal(String dialString, Int32 videoState)
            throws CallStateException {
        If (DBG) Log("dialInternal: dialString=" + (VDBG ? dialString : "xxxxxx"));
        ClearDisconnected();

        If (!CanDial()) {
            throw new CallStateException("dialInternal: cannot dial in current state");
        }
        If (mForegroundCall->GetState() == SipCall.State.ACTIVE) {
            SwitchHoldingAndActive();
        }
        If (mForegroundCall->GetState() != SipCall.State.IDLE) {
            //we should have failed in !CanDial() above before we get here
            throw new CallStateException("cannot dial in current state");
        }

        mForegroundCall->SetMute(FALSE);
        try {
            Connection c = mForegroundCall->Dial(dialString);
            return c;
        } Catch (SipException e) {
            Loge("dialInternal: ", e);
            throw new CallStateException("dial error: " + e);
        }
    }

    //@Override
    CARAPI SwitchHoldingAndActive() throws CallStateException {
        If (DBG) Log("dialInternal: switch fg and bg");
        {    AutoLock syncLock(SipPhone.class);
            mForegroundCall->SwitchWith(mBackgroundCall);
            If (mBackgroundCall->GetState()->IsAlive()) mBackgroundCall->Hold();
            If (mForegroundCall->GetState()->IsAlive()) mForegroundCall->Unhold();
        }
    }

    //@Override
    public Boolean CanConference() {
        If (DBG) Log("canConference: ret=TRUE");
        return TRUE;
    }

    //@Override
    CARAPI Conference() throws CallStateException {
        {    AutoLock syncLock(SipPhone.class);
            If ((mForegroundCall->GetState() != SipCall.State.ACTIVE)
                    || (mForegroundCall->GetState() != SipCall.State.ACTIVE)) {
                throw new CallStateException("wrong state to merge calls: fg="
                        + mForegroundCall->GetState() + ", bg="
                        + mBackgroundCall->GetState());
            }
            If (DBG) Log("conference: merge fg & bg");
            mForegroundCall->Merge(mBackgroundCall);
        }
    }

    CARAPI Conference(Call that) throws CallStateException {
        {    AutoLock syncLock(SipPhone.class);
            If (!(that instanceof SipCall)) {
                throw new CallStateException("expect " + SipCall.class
                        + ", cannot merge with " + that->GetClass());
            }
            mForegroundCall->Merge((SipCall) that);
        }
    }

    //@Override
    public Boolean CanTransfer() {
        return FALSE;
    }

    //@Override
    CARAPI ExplicitCallTransfer() {
        //mCT->ExplicitCallTransfer();
    }

    //@Override
    CARAPI ClearDisconnected() {
        {    AutoLock syncLock(SipPhone.class);
            mRingingCall->ClearDisconnected();
            mForegroundCall->ClearDisconnected();
            mBackgroundCall->ClearDisconnected();

            UpdatePhoneState();
            NotifyPreciseCallStateChanged();
        }
    }

    //@Override
    CARAPI SendDtmf(Char32 c) {
        If (!PhoneNumberUtils->Is12Key(c)) {
            Loge("sendDtmf called with invalid character '" + c + "'");
        } else If (mForegroundCall->GetState()->IsAlive()) {
            {    AutoLock syncLock(SipPhone.class);
                mForegroundCall->SendDtmf(c);
            }
        }
    }

    //@Override
    CARAPI StartDtmf(Char32 c) {
        If (!PhoneNumberUtils->Is12Key(c)) {
            Loge("startDtmf called with invalid character '" + c + "'");
        } else {
            SendDtmf(c);
        }
    }

    //@Override
    CARAPI StopDtmf() {
        // no op
    }

    CARAPI SendBurstDtmf(String dtmfString) {
        Loge("SendBurstDtmf() is a CDMA method");
    }

    //@Override
    CARAPI GetOutgoingCallerIdDisplay(Message onComplete) {
        // FIXME: what to reply?
        AsyncResult->ForMessage(onComplete, NULL, NULL);
        onComplete->SendToTarget();
    }

    //@Override
    CARAPI SetOutgoingCallerIdDisplay(Int32 commandInterfaceCLIRMode,
                                           Message onComplete) {
        // FIXME: what's this for SIP?
        AsyncResult->ForMessage(onComplete, NULL, NULL);
        onComplete->SendToTarget();
    }

    //@Override
    CARAPI GetCallWaiting(Message onComplete) {
        // FIXME: what to reply?
        AsyncResult->ForMessage(onComplete, NULL, NULL);
        onComplete->SendToTarget();
    }

    //@Override
    CARAPI SetCallWaiting(Boolean enable, Message onComplete) {
        // FIXME: what to reply?
        Loge("call waiting not supported");
    }

    //@Override
    CARAPI SetEchoSuppressionEnabled() {
        // Echo suppression may not be available on every device. So, check
        // whether it is supported
        {    AutoLock syncLock(SipPhone.class);
            AudioManager audioManager = (AudioManager) mContext->GetSystemService(Context.AUDIO_SERVICE);
            String echoSuppression = audioManager->GetParameters("ec_supported");
            If (echoSuppression->Contains("off")) {
                mForegroundCall->SetAudioGroupMode();
            }
        }
    }

    //@Override
    CARAPI SetMute(Boolean muted) {
        {    AutoLock syncLock(SipPhone.class);
            mForegroundCall->SetMute(muted);
        }
    }

    //@Override
    public Boolean GetMute() {
        Return (mForegroundCall->GetState()->IsAlive()
                ? mForegroundCall->GetMute()
                : mBackgroundCall->GetMute());
    }

    //@Override
    public Call GetForegroundCall() {
        return mForegroundCall;
    }

    //@Override
    public Call GetBackgroundCall() {
        return mBackgroundCall;
    }

    //@Override
    public Call GetRingingCall() {
        return mRingingCall;
    }

    //@Override
    public ServiceState GetServiceState() {
        // FIXME: we may need to provide this when data connectivity is lost
        // or when server is down
        return super->GetServiceState();
    }

    private String GetUriString(SipProfile p) {
        // SipProfile->GetUriString() may contain "SIP:" and port
        return p->GetUserName() + "@" + GetSipDomain(p);
    }

    private String GetSipDomain(SipProfile p) {
        String domain = p->GetSipDomain();
        // TODO: move this to SipProfile
        If (domain->EndsWith(":5060")) {
            return domain->Substring(0, domain->Length() - 5);
        } else {
            return domain;
        }
    }

    private static Call.State GetCallStateFrom(SipAudioCall sipAudioCall) {
        If (sipAudioCall->IsOnHold()) return Call.State.HOLDING;
        Int32 sessionState = sipAudioCall->GetState();
        Switch (sessionState) {
            case SipSession.State.READY_TO_CALL:            return Call.State.IDLE;
            case SipSession.State.INCOMING_CALL:
            case SipSession.State.INCOMING_CALL_ANSWERING:  return Call.State.INCOMING;
            case SipSession.State.OUTGOING_CALL:            return Call.State.DIALING;
            case SipSession.State.OUTGOING_CALL_RING_BACK:  return Call.State.ALERTING;
            case SipSession.State.OUTGOING_CALL_CANCELING:  return Call.State.DISCONNECTING;
            case SipSession.State.IN_CALL:                  return Call.State.ACTIVE;
            default:
                Slog("illegal connection state: " + sessionState);
                return Call.State.DISCONNECTED;
        }
    }

    private void Log(String s) {
        Rlog->D(LOG_TAG, s);
    }

    private static void Slog(String s) {
        Rlog->D(LOG_TAG, s);
    }

    private void Loge(String s) {
        Rlog->E(LOG_TAG, s);
    }

    private void Loge(String s, Exception e) {
        Rlog->E(LOG_TAG, s, e);
    }

    private class SipCall extends SipCallBase {
        private static const String SC_TAG = "SipCall";
        private static const Boolean SC_DBG = TRUE;
        private static const Boolean SC_VDBG = FALSE; // STOPSHIP if TRUE

        void Reset() {
            If (SC_DBG) Log("reset");
            mConnections->Clear();
            SetState(Call.State.IDLE);
        }

        void SwitchWith(SipCall that) {
            If (SC_DBG) Log("switchWith");
            {    AutoLock syncLock(SipPhone.class);
                SipCall tmp = new SipCall();
                tmp->TakeOver(this);
                this->TakeOver(that);
                that->TakeOver(tmp);
            }
        }

        private void TakeOver(SipCall that) {
            If (SC_DBG) Log("takeOver");
            mConnections = that.mConnections;
            mState = that.mState;
            For (Connection c : mConnections) {
                ((SipConnection) c).ChangeOwner(this);
            }
        }

        //@Override
        public Phone GetPhone() {
            return SipPhone.this;
        }

        //@Override
        public List<Connection> GetConnections() {
            If (SC_VDBG) Log("getConnections");
            {    AutoLock syncLock(SipPhone.class);
                // FIXME should return Collections->UnmodifiableList();
                return mConnections;
            }
        }

        Connection Dial(String originalNumber) throws SipException {
            If (SC_DBG) Log("dial: num=" + (SC_VDBG ? originalNumber : "xxx"));
            // TODO: Should this be synchronized?
            String calleeSipUri = originalNumber;
            If (!calleeSipUri->Contains("@")) {
                String replaceStr = Pattern->Quote(mProfile->GetUserName() + "@");
                calleeSipUri = mProfile->GetUriString()->ReplaceFirst(replaceStr,
                        calleeSipUri + "@");
            }
            try {
                SipProfile callee =
                        new SipProfile->Builder(calleeSipUri).Build();
                SipConnection c = new SipConnection(this, callee,
                        originalNumber);
                c->Dial();
                mConnections->Add(c);
                SetState(Call.State.DIALING);
                return c;
            } Catch (ParseException e) {
                throw new SipException("dial", e);
            }
        }

        //@Override
        CARAPI Hangup() throws CallStateException {
            {    AutoLock syncLock(SipPhone.class);
                If (mState->IsAlive()) {
                    If (SC_DBG) Log("hangup: call " + GetState()
                            + ": " + this + " on phone " + GetPhone());
                    SetState(State.DISCONNECTING);
                    CallStateException excp = NULL;
                    For (Connection c : mConnections) {
                        try {
                            c->Hangup();
                        } Catch (CallStateException e) {
                            excp = e;
                        }
                    }
                    If (excp != NULL) throw excp;
                } else {
                    If (SC_DBG) Log("hangup: dead call " + GetState()
                            + ": " + this + " on phone " + GetPhone());
                }
            }
        }

        SipConnection InitIncomingCall(SipAudioCall sipAudioCall, Boolean makeCallWait) {
            SipProfile callee = sipAudioCall->GetPeerProfile();
            SipConnection c = new SipConnection(this, callee);
            mConnections->Add(c);

            Call.State newState = makeCallWait ? State.WAITING : State.INCOMING;
            c->InitIncomingCall(sipAudioCall, newState);

            SetState(newState);
            NotifyNewRingingConnectionP(c);
            return c;
        }

        void RejectCall() throws CallStateException {
            If (SC_DBG) Log("rejectCall:");
            Hangup();
        }

        void AcceptCall() throws CallStateException {
            If (SC_DBG) Log("acceptCall: accepting");
            If (this != mRingingCall) {
                throw new CallStateException("AcceptCall() in a non-ringing call");
            }
            If (mConnections->Size() != 1) {
                throw new CallStateException("AcceptCall() in a conf call");
            }
            ((SipConnection) mConnections->Get(0)).AcceptCall();
        }

        private Boolean IsSpeakerOn() {
            Boolean ret = ((AudioManager) mContext->GetSystemService(Context.AUDIO_SERVICE))
                    .IsSpeakerphoneOn();
            If (SC_VDBG) Log("isSpeakerOn: ret=" + ret);
            return ret;
        }

        void SetAudioGroupMode() {
            AudioGroup audioGroup = GetAudioGroup();
            If (audioGroup == NULL) {
                If (SC_DBG) Log("setAudioGroupMode: audioGroup == NULL ignore");
                return;
            }
            Int32 mode = audioGroup->GetMode();
            If (mState == State.HOLDING) {
                audioGroup->SetMode(AudioGroup.MODE_ON_HOLD);
            } else If (GetMute()) {
                audioGroup->SetMode(AudioGroup.MODE_MUTED);
            } else If (IsSpeakerOn()) {
                audioGroup->SetMode(AudioGroup.MODE_ECHO_SUPPRESSION);
            } else {
                audioGroup->SetMode(AudioGroup.MODE_NORMAL);
            }
            If (SC_DBG) Log(String->Format(
                    "setAudioGroupMode change: %d --> %d", mode,
                    audioGroup->GetMode()));
        }

        void Hold() throws CallStateException {
            If (SC_DBG) Log("hold:");
            SetState(State.HOLDING);
            For (Connection c : mConnections) ((SipConnection) c).Hold();
            SetAudioGroupMode();
        }

        void Unhold() throws CallStateException {
            If (SC_DBG) Log("unhold:");
            SetState(State.ACTIVE);
            AudioGroup audioGroup = new AudioGroup();
            For (Connection c : mConnections) {
                ((SipConnection) c).Unhold(audioGroup);
            }
            SetAudioGroupMode();
        }

        void SetMute(Boolean muted) {
            If (SC_DBG) Log("setMute: muted=" + muted);
            For (Connection c : mConnections) {
                ((SipConnection) c).SetMute(muted);
            }
        }

        Boolean GetMute() {
            Boolean ret = mConnections->IsEmpty()
                    ? FALSE
                    : ((SipConnection) mConnections->Get(0)).GetMute();
            If (SC_DBG) Log("getMute: ret=" + ret);
            return ret;
        }

        void Merge(SipCall that) throws CallStateException {
            If (SC_DBG) Log("merge:");
            AudioGroup audioGroup = GetAudioGroup();

            // copy to an array to avoid concurrent modification as connections
            // in that.connections will be removed in Add(SipConnection).
            Connection[] cc = that.mConnections->ToArray(
                    new Connection[that.mConnections->Size()]);
            For (Connection c : cc) {
                SipConnection conn = (SipConnection) c;
                Add(conn);
                If (conn->GetState() == Call.State.HOLDING) {
                    conn->Unhold(audioGroup);
                }
            }
            that->SetState(Call.State.IDLE);
        }

        private void Add(SipConnection conn) {
            If (SC_DBG) Log("add:");
            SipCall call = conn->GetCall();
            If (call == this) return;
            If (call != NULL) call.mConnections->Remove(conn);

            mConnections->Add(conn);
            conn->ChangeOwner(this);
        }

        void SendDtmf(Char32 c) {
            If (SC_DBG) Log("sendDtmf: c=" + c);
            AudioGroup audioGroup = GetAudioGroup();
            If (audioGroup == NULL) {
                If (SC_DBG) Log("sendDtmf: audioGroup == NULL, ignore c=" + c);
                return;
            }
            audioGroup->SendDtmf(ConvertDtmf(c));
        }

        private Int32 ConvertDtmf(Char32 c) {
            Int32 code = c - '0';
            If ((code < 0) || (code > 9)) {
                Switch (c) {
                    case '*': return 10;
                    case '#': return 11;
                    case 'A': return 12;
                    case 'B': return 13;
                    case 'C': return 14;
                    case 'D': return 15;
                    default:
                        throw new IllegalArgumentException(
                                "invalid DTMF Char32: " + (Int32) c);
                }
            }
            return code;
        }

        //@Override
        protected void SetState(State newState) {
            If (mState != newState) {
                If (SC_DBG) Log("setState: cur state" + mState
                        + " --> " + newState + ": " + this + ": on phone "
                        + GetPhone() + " " + mConnections->Size());

                If (newState == Call.State.ALERTING) {
                    mState = newState; // need in ALERTING to enable ringback
                    StartRingbackTone();
                } else If (mState == Call.State.ALERTING) {
                    StopRingbackTone();
                }
                mState = newState;
                UpdatePhoneState();
                NotifyPreciseCallStateChanged();
            }
        }

        void OnConnectionStateChanged(SipConnection conn) {
            // this can be called back when a conf call is formed
            If (SC_DBG) Log("onConnectionStateChanged: conn=" + conn);
            If (mState != State.ACTIVE) {
                SetState(conn->GetState());
            }
        }

        void OnConnectionEnded(SipConnection conn) {
            // set state to DISCONNECTED only when all conns are disconnected
            If (SC_DBG) Log("onConnectionEnded: conn=" + conn);
            If (mState != State.DISCONNECTED) {
                Boolean allConnectionsDisconnected = TRUE;
                If (SC_DBG) Log("---check connections: "
                        + mConnections->Size());
                For (Connection c : mConnections) {
                    If (SC_DBG) Log("   state=" + c->GetState() + ": "
                            + c);
                    If (c->GetState() != State.DISCONNECTED) {
                        allConnectionsDisconnected = FALSE;
                        break;
                    }
                }
                If (allConnectionsDisconnected) SetState(State.DISCONNECTED);
            }
            NotifyDisconnectP(conn);
        }

        private AudioGroup GetAudioGroup() {
            If (mConnections->IsEmpty()) return NULL;
            Return ((SipConnection) mConnections->Get(0)).GetAudioGroup();
        }

        private void Log(String s) {
            Rlog->D(SC_TAG, s);
        }
    }

    private class SipConnection extends SipConnectionBase {
        private static const String SCN_TAG = "SipConnection";
        private static const Boolean SCN_DBG = TRUE;

        private SipCall mOwner;
        private SipAudioCall mSipAudioCall;
        private Call.State mState = Call.State.IDLE;
        private SipProfile mPeer;
        private Boolean mIncoming = FALSE;
        private String mOriginalNumber; // may be a PSTN number

        private SipAudioCallAdapter mAdapter = new SipAudioCallAdapter() {
            //@Override
            protected void OnCallEnded(Int32 cause) {
                If (GetDisconnectCause() != DisconnectCause.LOCAL) {
                    SetDisconnectCause(cause);
                }
                {    AutoLock syncLock(SipPhone.class);
                    SetState(Call.State.DISCONNECTED);
                    SipAudioCall sipAudioCall = mSipAudioCall;
                    // FIXME: This goes NULL and is synchronized, but many uses aren't sync'd
                    mSipAudioCall = NULL;
                    String sessionState = (sipAudioCall == NULL)
                            ? ""
                            : (sipAudioCall->GetState() + ", ");
                    If (SCN_DBG) Log("[SipAudioCallAdapter] onCallEnded: "
                            + mPeer->GetUriString() + ": " + sessionState
                            + "cause: " + GetDisconnectCause() + ", on phone "
                            + GetPhone());
                    If (sipAudioCall != NULL) {
                        sipAudioCall->SetListener(NULL);
                        sipAudioCall->Close();
                    }
                    mOwner->OnConnectionEnded(SipConnection.this);
                }
            }

            //@Override
            CARAPI OnCallEstablished(SipAudioCall call) {
                OnChanged(call);
                // Race onChanged synchronized this isn't
                If (mState == Call.State.ACTIVE) call->StartAudio();
            }

            //@Override
            CARAPI OnCallHeld(SipAudioCall call) {
                OnChanged(call);
                // Race onChanged synchronized this isn't
                If (mState == Call.State.HOLDING) call->StartAudio();
            }

            //@Override
            CARAPI OnChanged(SipAudioCall call) {
                {    AutoLock syncLock(SipPhone.class);
                    Call.State newState = GetCallStateFrom(call);
                    If (mState == newState) return;
                    If (newState == Call.State.INCOMING) {
                        SetState(mOwner->GetState()); // INCOMING or WAITING
                    } else {
                        If (mOwner == mRingingCall) {
                            If (mRingingCall->GetState() == Call.State.WAITING) {
                                try {
                                    SwitchHoldingAndActive();
                                } Catch (CallStateException e) {
                                    // disconnect the call.
                                    OnCallEnded(DisconnectCause.LOCAL);
                                    return;
                                }
                            }
                            mForegroundCall->SwitchWith(mRingingCall);
                        }
                        SetState(newState);
                    }
                    mOwner->OnConnectionStateChanged(SipConnection.this);
                    If (SCN_DBG) Log("onChanged: "
                            + mPeer->GetUriString() + ": " + mState
                            + " on phone " + GetPhone());
                }
            }

            //@Override
            protected void OnError(Int32 cause) {
                If (SCN_DBG) Log("onError: " + cause);
                OnCallEnded(cause);
            }
        };

        public SipConnection(SipCall owner, SipProfile callee,
                String originalNumber) {
            Super(originalNumber);
            mOwner = owner;
            mPeer = callee;
            mOriginalNumber = originalNumber;
        }

        public SipConnection(SipCall owner, SipProfile callee) {
            This(owner, callee, GetUriString(callee));
        }

        //@Override
        public String GetCnapName() {
            String displayName = mPeer->GetDisplayName();
            return TextUtils->IsEmpty(displayName) ? NULL
                                                  : displayName;
        }

        //@Override
        public Int32 GetNumberPresentation() {
            return PhoneConstants.PRESENTATION_ALLOWED;
        }

        void InitIncomingCall(SipAudioCall sipAudioCall, Call.State newState) {
            SetState(newState);
            mSipAudioCall = sipAudioCall;
            sipAudioCall->SetListener(mAdapter); // call back to set state
            mIncoming = TRUE;
        }

        void AcceptCall() throws CallStateException {
            try {
                mSipAudioCall->AnswerCall(TIMEOUT_ANSWER_CALL);
            } Catch (SipException e) {
                throw new CallStateException("AcceptCall(): " + e);
            }
        }

        void ChangeOwner(SipCall owner) {
            mOwner = owner;
        }

        AudioGroup GetAudioGroup() {
            If (mSipAudioCall == NULL) return NULL;
            return mSipAudioCall->GetAudioGroup();
        }

        void Dial() throws SipException {
            SetState(Call.State.DIALING);
            mSipAudioCall = mSipManager->MakeAudioCall(mProfile, mPeer, NULL,
                    TIMEOUT_MAKE_CALL);
            mSipAudioCall->SetListener(mAdapter);
        }

        void Hold() throws CallStateException {
            SetState(Call.State.HOLDING);
            try {
                mSipAudioCall->HoldCall(TIMEOUT_HOLD_CALL);
            } Catch (SipException e) {
                throw new CallStateException("Hold(): " + e);
            }
        }

        void Unhold(AudioGroup audioGroup) throws CallStateException {
            mSipAudioCall->SetAudioGroup(audioGroup);
            SetState(Call.State.ACTIVE);
            try {
                mSipAudioCall->ContinueCall(TIMEOUT_HOLD_CALL);
            } Catch (SipException e) {
                throw new CallStateException("Unhold(): " + e);
            }
        }

        void SetMute(Boolean muted) {
            If ((mSipAudioCall != NULL) && (muted != mSipAudioCall->IsMuted())) {
                If (SCN_DBG) Log("setState: prev muted=" + !muted + " new muted=" + muted);
                mSipAudioCall->ToggleMute();
            }
        }

        Boolean GetMute() {
            Return (mSipAudioCall == NULL) ? FALSE
                                           : mSipAudioCall->IsMuted();
        }

        //@Override
        protected void SetState(Call.State state) {
            If (state == mState) return;
            super->SetState(state);
            mState = state;
        }

        //@Override
        public Call.State GetState() {
            return mState;
        }

        //@Override
        public Boolean IsIncoming() {
            return mIncoming;
        }

        //@Override
        public String GetAddress() {
            // Phone app uses this to query caller ID. Return the original dial
            // Number (which may be a PSTN number) instead of the peer's SIP
            // URI.
            return mOriginalNumber;
        }

        //@Override
        public SipCall GetCall() {
            return mOwner;
        }

        //@Override
        protected Phone GetPhone() {
            return mOwner->GetPhone();
        }

        //@Override
        CARAPI Hangup() throws CallStateException {
            {    AutoLock syncLock(SipPhone.class);
                If (SCN_DBG) Log("hangup: conn=" + mPeer->GetUriString()
                        + ": " + mState + ": on phone "
                        + GetPhone()->GetPhoneName());
                If (!mState->IsAlive()) return;
                try {
                    SipAudioCall sipAudioCall = mSipAudioCall;
                    If (sipAudioCall != NULL) {
                        sipAudioCall->SetListener(NULL);
                        sipAudioCall->EndCall();
                    }
                } Catch (SipException e) {
                    throw new CallStateException("Hangup(): " + e);
                } finally {
                    mAdapter->OnCallEnded(((mState == Call.State.INCOMING)
                            || (mState == Call.State.WAITING))
                            ? DisconnectCause.INCOMING_REJECTED
                            : DisconnectCause.LOCAL);
                }
            }
        }

        //@Override
        CARAPI Separate() throws CallStateException {
            {    AutoLock syncLock(SipPhone.class);
                SipCall call = (GetPhone() == SipPhone.this)
                        ? (SipCall) GetBackgroundCall()
                        : (SipCall) GetForegroundCall();
                If (call->GetState() != Call.State.IDLE) {
                    throw new CallStateException(
                            "cannot put conn back to a call in non-idle state: "
                            + call->GetState());
                }
                If (SCN_DBG) Log("separate: conn="
                        + mPeer->GetUriString() + " from " + mOwner + " back to "
                        + call);

                // separate the AudioGroup and connection from the original call
                Phone originalPhone = GetPhone();
                AudioGroup audioGroup = call->GetAudioGroup(); // may be NULL
                call->Add(this);
                mSipAudioCall->SetAudioGroup(audioGroup);

                // put the original call to bg; and the separated call becomes
                // fg if it was in bg
                originalPhone->SwitchHoldingAndActive();

                // start audio and notify the phone app of the state change
                call = (SipCall) GetForegroundCall();
                mSipAudioCall->StartAudio();
                call->OnConnectionStateChanged(this);
            }
        }

        private void Log(String s) {
            Rlog->D(SCN_TAG, s);
        }
    }

    private abstract class SipAudioCallAdapter extends SipAudioCall.Listener {
        private static const String SACA_TAG = "SipAudioCallAdapter";
        private static const Boolean SACA_DBG = TRUE;
        /** Call ended with cause defined in {@link DisconnectCause}. */
        protected abstract void OnCallEnded(Int32 cause);
        /** Call failed with cause defined in {@link DisconnectCause}. */
        protected abstract void OnError(Int32 cause);

        //@Override
        CARAPI OnCallEnded(SipAudioCall call) {
            If (SACA_DBG) Log("onCallEnded: call=" + call);
            OnCallEnded(call->IsInCall()
                    ? DisconnectCause.NORMAL
                    : DisconnectCause.INCOMING_MISSED);
        }

        //@Override
        CARAPI OnCallBusy(SipAudioCall call) {
            If (SACA_DBG) Log("onCallBusy: call=" + call);
            OnCallEnded(DisconnectCause.BUSY);
        }

        //@Override
        CARAPI OnError(SipAudioCall call, Int32 errorCode,
                String errorMessage) {
            If (SACA_DBG) {
                Log("onError: call=" + call + " code="+ SipErrorCode->ToString(errorCode)
                    + ": " + errorMessage);
            }
            Switch (errorCode) {
                case SipErrorCode.SERVER_UNREACHABLE:
                    OnError(DisconnectCause.SERVER_UNREACHABLE);
                    break;
                case SipErrorCode.PEER_NOT_REACHABLE:
                    OnError(DisconnectCause.NUMBER_UNREACHABLE);
                    break;
                case SipErrorCode.INVALID_REMOTE_URI:
                    OnError(DisconnectCause.INVALID_NUMBER);
                    break;
                case SipErrorCode.TIME_OUT:
                case SipErrorCode.TRANSACTION_TERMINTED:
                    OnError(DisconnectCause.TIMED_OUT);
                    break;
                case SipErrorCode.DATA_CONNECTION_LOST:
                    OnError(DisconnectCause.LOST_SIGNAL);
                    break;
                case SipErrorCode.INVALID_CREDENTIALS:
                    OnError(DisconnectCause.INVALID_CREDENTIALS);
                    break;
                case SipErrorCode.CROSS_DOMAIN_AUTHENTICATION:
                    OnError(DisconnectCause.OUT_OF_NETWORK);
                    break;
                case SipErrorCode.SERVER_ERROR:
                    OnError(DisconnectCause.SERVER_ERROR);
                    break;
                case SipErrorCode.SOCKET_ERROR:
                case SipErrorCode.CLIENT_ERROR:
                default:
                    OnError(DisconnectCause.ERROR_UNSPECIFIED);
            }
        }

        private void Log(String s) {
            Rlog->D(SACA_TAG, s);
        }
    }

    //@Override
    public Connection Dial(String dialString, Int32 videoState, Bundle extras)
            throws CallStateException {
        // NOTE: Add implementation if call extras are needed to be
        //       passed through SIP phone.
        return NULL;
    }
}
