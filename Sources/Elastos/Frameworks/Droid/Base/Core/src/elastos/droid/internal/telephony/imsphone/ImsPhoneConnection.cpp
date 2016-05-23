/*
 * Copyright (C) 2013 The Android Open Source Project
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

package com.android.internal.telephony.imsphone;

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::IRegistrant;
using Elastos::Droid::Os::ISystemClock;
using Elastos::Droid::Telecom::ILog;
using Elastos::Droid::Telephony::IDisconnectCause;
using Elastos::Droid::Telephony::IPhoneNumberUtils;
using Elastos::Droid::Telephony::IRlog;

using Elastos::Droid::Ims::IImsStreamMediaProfile;
using Elastos::Droid::Internal::Telephony::IConnection;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Internal::Telephony::ITelephonyProperties;
using Elastos::Droid::Internal::Telephony::IUUSInfo;
using Elastos::Droid::Ims::IImsCall;
using Elastos::Droid::Ims::IImsCallProfile;

/**
 * {@hide}
 */
public class ImsPhoneConnection extends Connection {
    private static const String LOG_TAG = "ImsPhoneConnection";
    private static const Boolean DBG = TRUE;

    //***** Instance Variables

    private ImsPhoneCallTracker mOwner;
    private ImsPhoneCall mParent;
    private ImsCall mImsCall;

    private String mPostDialString;      // outgoing calls only
    private Boolean mDisconnected;

    private Bundle mCallExtras = NULL;

    /*
    Int32 mIndex;          // index in ImsPhoneCallTracker.connections[], -1 if unassigned
                        // The GSM index is 1 + this
    */

    /*
     * These time/timespan values are based on System->CurrentTimeMillis(),
     * i.e., "wall clock" time.
     */
    private Int64 mDisconnectTime;

    private Int32 mNextPostDialChar;       // index into postDialString

    private Int32 mCause = DisconnectCause.NOT_DISCONNECTED;
    private PostDialState mPostDialState = PostDialState.NOT_STARTED;
    private UUSInfo mUusInfo;
    private Handler mHandler;

    private PowerManager.WakeLock mPartialWakeLock;

    //***** Event Constants
    private static const Int32 EVENT_DTMF_DONE = 1;
    private static const Int32 EVENT_PAUSE_DONE = 2;
    private static const Int32 EVENT_NEXT_POST_DIAL = 3;
    private static const Int32 EVENT_WAKE_LOCK_TIMEOUT = 4;

    //***** Constants
    private static const Int32 PAUSE_DELAY_MILLIS = 3 * 1000;
    private static const Int32 WAKE_LOCK_TIMEOUT_MILLIS = 60*1000;

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

    /** This is probably an MT call */
    /*package*/
    ImsPhoneConnection(Context context, ImsCall imsCall, ImsPhoneCallTracker ct, ImsPhoneCall parent) {
        CreateWakeLock(context);
        AcquireWakeLock();

        mOwner = ct;
        mHandler = new MyHandler(mOwner->GetLooper());
        mImsCall = imsCall;

        If ((imsCall != NULL) && (imsCall->GetCallProfile() != NULL)) {
            mAddress = imsCall->GetCallProfile()->GetCallExtra(ImsCallProfile.EXTRA_OI);
            mCnapName = imsCall->GetCallProfile()->GetCallExtra(ImsCallProfile.EXTRA_CNA);
            mNumberPresentation = ImsCallProfile->OIRToPresentation(
                    imsCall->GetCallProfile()->GetCallExtraInt(ImsCallProfile.EXTRA_OIR));
            mCnapNamePresentation = ImsCallProfile->OIRToPresentation(
                    imsCall->GetCallProfile()->GetCallExtraInt(ImsCallProfile.EXTRA_CNAP));

            ImsCallProfile imsCallProfile = imsCall->GetCallProfile();
            If (imsCallProfile != NULL) {
                SetVideoState(ImsCallProfile->GetVideoStateFromImsCallProfile(imsCallProfile));

                ImsStreamMediaProfile mediaProfile = imsCallProfile.mMediaProfile;
                If (mediaProfile != NULL) {
                    SetAudioQuality(GetAudioQualityFromMediaProfile(mediaProfile));
                }
            }

            // Determine if the current call have video capabilities.
            try {
                ImsCallProfile localCallProfile = imsCall->GetLocalCallProfile();
                If (localCallProfile != NULL) {
                    Boolean isLocalVideoCapable = localCallProfile.mCallType
                            == ImsCallProfile.CALL_TYPE_VT;

                    SetLocalVideoCapable(isLocalVideoCapable);
                }
                ImsCallProfile remoteCallProfile = imsCall->GetRemoteCallProfile();
                If (remoteCallProfile != NULL) {
                    Boolean isRemoteVideoCapable = remoteCallProfile.mCallType
                            == ImsCallProfile.CALL_TYPE_VT;

                    SetRemoteVideoCapable(isRemoteVideoCapable);
                }
            } Catch (ImsException e) {
                // No session, so cannot get local capabilities.
            }
        } else {
            mNumberPresentation = PhoneConstants.PRESENTATION_UNKNOWN;
            mCnapNamePresentation = PhoneConstants.PRESENTATION_UNKNOWN;
        }

        mIsIncoming = TRUE;
        mCreateTime = System->CurrentTimeMillis();
        mUusInfo = NULL;

        //mIndex = index;

        mParent = parent;
        mParent->Attach(this, ImsPhoneCall.State.INCOMING);
    }

    /** This is an MO call, created when dialing */
    /*package*/
    ImsPhoneConnection(Context context, String dialString, ImsPhoneCallTracker ct,
            ImsPhoneCall parent, Bundle extras) {
        CreateWakeLock(context);
        AcquireWakeLock();
        Boolean isConferenceUri = FALSE;
        Boolean isSkipSchemaParsing = FALSE;
        If (extras != NULL) {
            isConferenceUri = extras->GetBoolean(
                    TelephonyProperties.EXTRA_DIAL_CONFERENCE_URI, FALSE);
            isSkipSchemaParsing = extras->GetBoolean(
                    TelephonyProperties.EXTRA_SKIP_SCHEMA_PARSING, FALSE);
        }

        mOwner = ct;
        mHandler = new MyHandler(mOwner->GetLooper());

        mDialString = dialString;

        If (isConferenceUri || isSkipSchemaParsing) {
            mAddress = dialString;
            mPostDialString = "";
        } else {
            mAddress = PhoneNumberUtils->ExtractNetworkPortionAlt(dialString);
            mPostDialString = PhoneNumberUtils->ExtractPostDialPortion(dialString);
        }

        //mIndex = -1;

        mIsIncoming = FALSE;
        mCnapName = NULL;
        mCnapNamePresentation = PhoneConstants.PRESENTATION_ALLOWED;
        mNumberPresentation = PhoneConstants.PRESENTATION_ALLOWED;
        mCreateTime = System->CurrentTimeMillis();

        If (extras != NULL) {
            mCallExtras = extras;
        }

        mParent = parent;
        parent->AttachFake(this, ImsPhoneCall.State.DIALING);
    }

    CARAPI Dispose() {
    }

    static Boolean
    EqualsHandlesNulls (Object a, Object b) {
        Return (a == NULL) ? (b == NULL) : a.Equals (b);
    }

    /**
     * Determines the {@link ImsPhoneConnection} audio quality based on an
     * {@link ImsStreamMediaProfile}.
     *
     * @param mediaProfile The media profile.
     * @return The audio quality.
     */
    private Int32 GetAudioQualityFromMediaProfile(ImsStreamMediaProfile mediaProfile) {
        Int32 audioQuality;

        // The Adaptive Multi-Rate Wideband codec is used for high definition audio calls.
        If (mediaProfile.mAudioQuality == ImsStreamMediaProfile.AUDIO_QUALITY_AMR_WB) {
            audioQuality = AUDIO_QUALITY_HIGH_DEFINITION;
        } else {
            audioQuality = AUDIO_QUALITY_STANDARD;
        }

        return audioQuality;
    }


    //@Override
    public String GetOrigDialString(){
        return mDialString;
    }

    //@Override
    public ImsPhoneCall GetCall() {
        return mParent;
    }

    //@Override
    public Int64 GetDisconnectTime() {
        return mDisconnectTime;
    }

    //@Override
    public Int64 GetHoldingStartTime() {
        return mHoldingStartTime;
    }

    //@Override
    public Int64 GetHoldDurationMillis() {
        If (GetState() != ImsPhoneCall.State.HOLDING) {
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

    CARAPI SetDisconnectCause(Int32 cause) {
        mCause = cause;
    }

    public ImsPhoneCallTracker GetOwner () {
        return mOwner;
    }

    //@Override
    public ImsPhoneCall.State GetState() {
        If (mDisconnected) {
            return ImsPhoneCall.State.DISCONNECTED;
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
        throw new CallStateException ("not supported");
    }

    //@Override
    public PostDialState GetPostDialState() {
        return mPostDialState;
    }

    //@Override
    CARAPI ProceedAfterWaitChar() {
        If (mPostDialState != PostDialState.WAIT) {
            Rlog->W(LOG_TAG, "ImsPhoneConnection->ProceedAfterWaitChar(): Expected "
                    + "GetPostDialState() to be WAIT but was " + mPostDialState);
            return;
        }

        SetPostDialState(PostDialState.STARTED);

        ProcessNextPostDialChar();
    }

    //@Override
    CARAPI ProceedAfterWildChar(String str) {
        If (mPostDialState != PostDialState.WILD) {
            Rlog->W(LOG_TAG, "ImsPhoneConnection->ProceedAfterWaitChar(): Expected "
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
            Rlog->D(LOG_TAG, "proceedAfterWildChar: new postDialString is " +
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
     */
    void
    OnHangupLocal() {
        mCause = DisconnectCause.LOCAL;
    }

    /** Called when the connection has been disconnected */
    /*package*/ Boolean
    OnDisconnect(Int32 cause) {
        Rlog->D(LOG_TAG, "onDisconnect: cause=" + cause);
        If (mCause != DisconnectCause.LOCAL) mCause = cause;
        return OnDisconnect();
    }

    /*package*/ Boolean
    OnDisconnect() {
        Boolean changed = FALSE;

        If (!mDisconnected) {
            //mIndex = -1;

            mDisconnectTime = System->CurrentTimeMillis();
            mDuration = SystemClock->ElapsedRealtime() - mConnectTimeReal;
            mDisconnected = TRUE;

            mOwner.mPhone->NotifyDisconnect(this);

            If (mParent != NULL) {
                changed = mParent->ConnectionDisconnected(this);
            } else {
                Rlog->D(LOG_TAG, "onDisconnect: no parent");
            }
            If (mImsCall != NULL) mImsCall->Close();
            mImsCall = NULL;
        }
        ReleaseWakeLock();
        return changed;
    }

    /**
     * An incoming or outgoing call has connected
     */
    void
    OnConnectedInOrOut() {
        mConnectTime = System->CurrentTimeMillis();
        mConnectTimeReal = SystemClock->ElapsedRealtime();
        mDuration = 0;

        If (Phone.DEBUG_PHONE) {
            Rlog->D(LOG_TAG, "onConnectedInOrOut: connectTime=" + mConnectTime);
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
        ReleaseWakeLock();
    }

    private void
    ProcessNextPostDialChar() {
        Char32 c = 0;
        Registrant postDialHandler;

        If (mPostDialState == PostDialState.CANCELLED) {
            //Rlog->D(LOG_TAG, "##### processNextPostDialChar: postDialState == CANCELLED, bail");
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
                Rlog->E(LOG_TAG, "processNextPostDialChar: c=" + c + " isn't valid!");
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

            //Rlog->V(LOG_TAG, "##### processNextPostDialChar: send msg to postDialHandler, arg1=" + c);
            notifyMessage->SendToTarget();
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
    }

    private void
    CreateWakeLock(Context context) {
        PowerManager pm = (PowerManager) context->GetSystemService(Context.POWER_SERVICE);
        mPartialWakeLock = pm->NewWakeLock(PowerManager.PARTIAL_WAKE_LOCK, LOG_TAG);
    }

    private void
    AcquireWakeLock() {
        Rlog->D(LOG_TAG, "acquireWakeLock");
        mPartialWakeLock->Acquire();
    }

    void
    ReleaseWakeLock() {
        {    AutoLock syncLock(mPartialWakeLock);
            If (mPartialWakeLock->IsHeld()) {
                Rlog->D(LOG_TAG, "releaseWakeLock");
                mPartialWakeLock->Release();
            }
        }
    }

    //@Override
    public Int32 GetNumberPresentation() {
        return mNumberPresentation;
    }

    //@Override
    public UUSInfo GetUUSInfo() {
        return mUusInfo;
    }

    //@Override
    public Connection GetOrigConnection() {
        return NULL;
    }

    //@Override
    public Boolean IsMultiparty() {
        return mImsCall != NULL && mImsCall->IsMultiparty();
    }

    /*package*/ ImsCall GetImsCall() {
        return mImsCall;
    }

    /*package*/ void SetImsCall(ImsCall imsCall) {
        mImsCall = imsCall;
    }

    /*package*/ void ChangeParent(ImsPhoneCall parent) {
        mParent = parent;
    }

    /*package*/ Boolean
    Update(ImsCall imsCall, ImsPhoneCall.State state) {
        Boolean changed = FALSE;

        If (state == ImsPhoneCall.State.ACTIVE) {
            If (mParent->GetState()->IsRinging()
                    || mParent->GetState()->IsDialing()) {
                OnConnectedInOrOut();
            }

            If (mParent->GetState()->IsRinging()
                    || mParent == mOwner.mBackgroundCall) {
                //mForegroundCall should be IDLE
                //when accepting WAITING call
                //before accept WAITING call,
                //the ACTIVE call should be held ahead
                mParent->Detach(this);
                mParent = mOwner.mForegroundCall;
                mParent->Attach(this);
            }
        } else If (state == ImsPhoneCall.State.HOLDING) {
            OnStartedHolding();
        }

        changed = mParent->Update(this, imsCall, state);

        If (imsCall != NULL) {
            // Check for a change in the video capabilities for the call and update the
            // {@link ImsPhoneConnection} with this information.
            try {
                // Get the current local VT Capabilities (i.e. even if currentCallType above is
                // audio-only, the local capability could support bi-directional video).
                ImsCallProfile localCallProfile = imsCall->GetLocalCallProfile();
                Rlog->D(LOG_TAG, " update localCallProfile=" + localCallProfile
                        + "IsLocalVideoCapable()= " + IsLocalVideoCapable());
                If (localCallProfile != NULL) {
                    Boolean newLocalVideoCapable = localCallProfile.mCallType
                            == ImsCallProfile.CALL_TYPE_VT;

                    If (IsLocalVideoCapable() != newLocalVideoCapable) {
                        SetLocalVideoCapable(newLocalVideoCapable);
                        changed = TRUE;
                    }
                }

                ImsCallProfile remoteCallProfile = imsCall->GetRemoteCallProfile();
                Rlog->D(LOG_TAG, " update remoteCallProfile=" + remoteCallProfile
                        + "IsRemoteVideoCapable()= " + IsRemoteVideoCapable());
                If (remoteCallProfile != NULL) {
                    Boolean newRemoteVideoCapable = remoteCallProfile.mCallType
                            == ImsCallProfile.CALL_TYPE_VT;

                    If (IsRemoteVideoCapable() != newRemoteVideoCapable) {
                        SetRemoteVideoCapable(newRemoteVideoCapable);
                        changed = TRUE;
                    }
                }

                // Check if call substate has changed. If so notify listeners of call state changed.
                Int32 callSubstate = GetCallSubstate();
                Int32 newCallSubstate = imsCall->GetCallSubstate();

                If (callSubstate != newCallSubstate) {
                    SetCallSubstate(newCallSubstate);
                    changed = TRUE;
                }

            } Catch (ImsException e) {
                // No session in place -- no change
            }

            // Check for a change in the call type / video state, or audio quality of the
            // {@link ImsCall} and update the {@link ImsPhoneConnection} with this information.
            ImsCallProfile callProfile = imsCall->GetCallProfile();
            If (callProfile != NULL) {
                Int32 oldVideoState = GetVideoState();
                Int32 newVideoState = ImsCallProfile->GetVideoStateFromImsCallProfile(callProfile);

                If (oldVideoState != newVideoState) {
                    SetVideoState(newVideoState);
                    changed = TRUE;
                }

                ImsStreamMediaProfile mediaProfile = callProfile.mMediaProfile;
                If (mediaProfile != NULL) {
                    Int32 oldAudioQuality = GetAudioQuality();
                    Int32 newAudioQuality = GetAudioQualityFromMediaProfile(mediaProfile);

                    If (oldAudioQuality != newAudioQuality) {
                        SetAudioQuality(newAudioQuality);
                        changed = TRUE;
                    }
                }
            }
        }
        return changed;
    }

    //@Override
    public Int32 GetPreciseDisconnectCause() {
        return 0;
    }

    public Bundle GetCallExtras() {
        return mCallExtras;
    }

    /**
     * Notifies this Connection of a request to disconnect a participant of the conference managed
     * by the connection.
     *
     * @param endpoint the {@link android.net.Uri} of the participant to disconnect.
     */
    //@Override
    CARAPI OnDisconnectConferenceParticipant(Uri endpoint) {
        ImsCall imsCall = GetImsCall();
        If (imsCall == NULL) {
            return;
        }
        try {
            imsCall->RemoveParticipants(new String[]{endpoint->ToString()});
        } Catch (ImsException e) {
            // No session in place -- no change
            Rlog->E(LOG_TAG, "onDisconnectConferenceParticipant: no session in place. "+
                    "Failed to disconnect endpoint = " + endpoint);
        }
    }

    /**
     * Provides a string representation of the {@link ImsPhoneConnection}.  Primarily intended for
     * use in log statements.
     *
     * @return String representation of call.
     */
    //@Override
    CARAPI ToString(
        /* [out] */ String* str)
    {
        StringBuilder sb = new StringBuilder();
        sb->Append("[ImsPhoneConnection objId: ");
        sb->Append(System->IdentityHashCode(this));
        sb->Append(" address:");
        sb->Append(Log->Pii(GetAddress()));
        sb->Append(" ImsCall:");
        If (mImsCall == NULL) {
            sb->Append("NULL");
        } else {
            sb->Append(mImsCall);
        }
        sb->Append("]");
        return sb->ToString();
    }
}

