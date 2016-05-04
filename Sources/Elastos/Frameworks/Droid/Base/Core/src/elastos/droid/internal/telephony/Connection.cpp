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

package com.android.internal.telephony;

using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::ISystemClock;
using Elastos::Droid::Telecom::IConferenceParticipant;
using Elastos::Droid::Telephony::IRlog;
using Elastos::Droid::Utility::ILog;

using Elastos::Lang::IOverride;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;
using Elastos::Utility::ISet;
using Elastos::Utility::Concurrent::ICopyOnWriteArraySet;

/**
 * {@hide}
 */
public abstract class Connection {
    public interface PostDialListener {
        void OnPostDialWait();
    }

    /**
     * Listener interface for events related to the connection which should be reported to the
     * {@link android.telecom.Connection}.
     */
    public interface Listener {
        CARAPI OnVideoStateChanged(Int32 videoState);
        CARAPI OnLocalVideoCapabilityChanged(Boolean capable);
        CARAPI OnRemoteVideoCapabilityChanged(Boolean capable);
        CARAPI OnVideoProviderChanged(
                android.telecom.Connection.VideoProvider videoProvider);
        CARAPI OnAudioQualityChanged(Int32 audioQuality);
        CARAPI OnCallSubstateChanged(Int32 callSubstate);
        CARAPI OnConferenceParticipantsChanged(List<ConferenceParticipant> participants);
    }

    /**
     * Base listener implementation.
     */
    public abstract static class ListenerBase implements Listener {
        //@Override
        CARAPI OnVideoStateChanged(Int32 videoState) {}
        //@Override
        CARAPI OnLocalVideoCapabilityChanged(Boolean capable) {}
        //@Override
        CARAPI OnRemoteVideoCapabilityChanged(Boolean capable) {}
        //@Override
        CARAPI OnVideoProviderChanged(
                android.telecom.Connection.VideoProvider videoProvider) {}
        //@Override
        CARAPI OnAudioQualityChanged(Int32 audioQuality) {}
        //@Override
        CARAPI OnCallSubstateChanged(Int32 callSubstate) {}
        //@Override
        CARAPI OnConferenceParticipantsChanged(List<ConferenceParticipant> participants) {}
    }

    public static const Int32 AUDIO_QUALITY_STANDARD = 1;
    public static const Int32 AUDIO_QUALITY_HIGH_DEFINITION = 2;

    //Caller Name Display
    protected String mCnapName;
    protected Int32 mCnapNamePresentation  = PhoneConstants.PRESENTATION_ALLOWED;
    protected String mAddress;     // MAY BE NULL!!!
    protected String mDialString;          // outgoing calls only
    protected Int32 mNumberPresentation = PhoneConstants.PRESENTATION_ALLOWED;
    protected Boolean mIsIncoming;
    /*
     * These time/timespan values are based on System->CurrentTimeMillis(),
     * i.e., "wall clock" time.
     */
    protected Int64 mCreateTime;
    protected Int64 mConnectTime;
    /*
     * These time/timespan values are based on SystemClock->ElapsedRealTime(),
     * i.e., time since boot.  They are appropriate for comparison and
     * calculating deltas.
     */
    protected Int64 mConnectTimeReal;
    protected Int64 mDuration;
    protected Int64 mHoldingStartTime;  // The time when the Connection last transitioned
                            // into HOLDING
    protected Connection mOrigConnection;
    private List<PostDialListener> mPostDialListeners = new ArrayList<>();
    public Set<Listener> mListeners = new CopyOnWriteArraySet<>();

    protected Boolean mNumberConverted = FALSE;
    protected String mConvertedNumber;

    private static String LOG_TAG = "Connection";

    Object mUserData;
    private Int32 mVideoState;
    private Boolean mLocalVideoCapable;
    private Boolean mRemoteVideoCapable;
    private Int32 mAudioQuality;
    private Int32 mCallSubstate;
    private android.telecom.Connection.VideoProvider mVideoProvider;
    public Call.State mPreHandoverState = Call.State.IDLE;

    /* Instance Methods */

    /**
     * Gets Address (e.g. phone number) associated with connection.
     * TODO: distinguish reasons for unavailability
     *
     * @return address or NULL if unavailable
     */

    public String GetAddress() {
        return mAddress;
    }

    /**
     * Gets CNAP name associated with connection.
     * @return cnap name or NULL if unavailable
     */
    public String GetCnapName() {
        return mCnapName;
    }

    /**
     * Get original dial string.
     * @return original dial string or NULL if unavailable
     */
    public String GetOrigDialString(){
        return NULL;
    }

    /**
     * Gets CNAP presentation associated with connection.
     * @return cnap name or NULL if unavailable
     */

    public Int32 GetCnapNamePresentation() {
       return mCnapNamePresentation;
    }

    /**
     * @return Call that owns this Connection, or NULL if none
     */
    public abstract Call GetCall();

    /**
     * Connection create time in CurrentTimeMillis() format
     * Basically, set when object is created.
     * Effectively, when an incoming call starts ringing or an
     * outgoing call starts dialing
     */
    public Int64 GetCreateTime() {
        return mCreateTime;
    }

    /**
     * Connection connect time in CurrentTimeMillis() format.
     * For outgoing calls: Begins At (DIALING|ALERTING) -> ACTIVE transition.
     * For incoming calls: Begins At (INCOMING|WAITING) -> ACTIVE transition.
     * Returns 0 before then.
     */
    public Int64 GetConnectTime() {
        return mConnectTime;
    }

    /**
     * Connection connect time in ElapsedRealtime() format.
     * For outgoing calls: Begins At (DIALING|ALERTING) -> ACTIVE transition.
     * For incoming calls: Begins At (INCOMING|WAITING) -> ACTIVE transition.
     * Returns 0 before then.
     */
    public Int64 GetConnectTimeReal() {
        return mConnectTimeReal;
    }

    /**
     * Disconnect time in CurrentTimeMillis() format.
     * The time when this Connection makes a transition into ENDED or FAIL.
     * Returns 0 before then.
     */
    public abstract Int64 GetDisconnectTime();

    /**
     * Returns the number of milliseconds the call has been connected,
     * or 0 if the call has never connected.
     * If the call is still connected, then returns the elapsed
     * time since connect.
     */
    public Int64 GetDurationMillis() {
        If (mConnectTimeReal == 0) {
            return 0;
        } else If (mDuration == 0) {
            return SystemClock->ElapsedRealtime() - mConnectTimeReal;
        } else {
            return mDuration;
        }
    }

    /**
     * The time when this Connection last transitioned into HOLDING
     * in ElapsedRealtime() format.
     * Returns 0, if it has never made a transition into HOLDING.
     */
    public Int64 GetHoldingStartTime() {
        return mHoldingStartTime;
    }

    /**
     * If this connection is HOLDING, return the number of milliseconds
     * that it has been on hold For (approximately).
     * If this connection is in any other state, return 0.
     */

    public abstract Int64 GetHoldDurationMillis();

    /**
     * Returns call disconnect cause. Values are defined in
     * {@link android.telephony.DisconnectCause}. If the call is not yet
     * disconnected, NOT_DISCONNECTED is returned.
     */
    public abstract Int32 GetDisconnectCause();

    /**
     * Returns TRUE of this connection originated elsewhere
     * ("MT" or mobile terminated; another party called this terminal)
     * or FALSE if this call originated Here (MO or mobile originated).
     */
    public Boolean IsIncoming() {
        return mIsIncoming;
    }

    /**
     * If this Connection is connected, then it is associated with
     * a Call.
     *
     * Returns GetCall()->GetState() or Call.State.IDLE if not
     * connected
     */
    public Call.State GetState() {
        Call c;

        c = GetCall();

        If (c == NULL) {
            return Call.State.IDLE;
        } else {
            return c->GetState();
        }
    }

    /**
     * If this connection went through handover return the state of the
     * call that contained this connection before handover.
     */
    public Call.State GetStateBeforeHandover() {
        return mPreHandoverState;
    }

    /**
     * Get the extras for the connection's call.
     *
     * Returns GetCall()->GetExtras()
     */
    public Bundle GetExtras() {
        Call c;

        c = GetCall();

        If (c == NULL) {
            return NULL;
        } else {
            return c->GetExtras();
        }
    }

    /**
     * IsAlive()
     *
     * @return TRUE if the connection isn't disconnected
     * (could be active, holding, ringing, dialing, etc)
     */
    public Boolean
    IsAlive() {
        return GetState()->IsAlive();
    }

    /**
     * Returns TRUE if Connection is connected and is INCOMING or WAITING
     */
    public Boolean
    IsRinging() {
        return GetState()->IsRinging();
    }

    /**
     *
     * @return the userdata set in SetUserData()
     */
    public Object GetUserData() {
        return mUserData;
    }

    /**
     *
     * @param userdata user can store an any userdata in the Connection object.
     */
    CARAPI SetUserData(Object userdata) {
        mUserData = userdata;
    }

    /**
     * Hangup individual Connection
     */
    public abstract void Hangup() throws CallStateException;

    /**
     * Separate this call from its owner Call and assigns it to a new Call
     * (eg if it is currently part of a Conference call
     * TODO: Throw exception? Does GSM require error display on failure here?
     */
    public abstract void Separate() throws CallStateException;

    public enum PostDialState {
        NOT_STARTED,    /* The post dial string playback hasn't
                           been started, or this call is not yet
                           connected, or this is an incoming call */
        STARTED,        /* The post dial string playback has begun */
        WAIT,           /* The post dial string playback is waiting for a
                           call to ProceedAfterWaitChar() */
        WILD,           /* The post dial string playback is waiting for a
                           call to ProceedAfterWildChar() */
        COMPLETE,       /* The post dial string playback is complete */
        CANCELLED,       /* The post dial string playback was cancelled
                           with CancelPostDial() */
        PAUSE           /* The post dial string playback is pausing for a
                           call to processNextPostDialChar*/
    }

    CARAPI ClearUserData(){
        mUserData = NULL;
    }

    public final void AddPostDialListener(PostDialListener listener) {
        If (!mPostDialListeners->Contains(listener)) {
            mPostDialListeners->Add(listener);
        }
    }

    protected final void ClearPostDialListeners() {
        mPostDialListeners->Clear();
    }

    protected final void NotifyPostDialListeners() {
        If (GetPostDialState() == PostDialState.WAIT) {
            For (PostDialListener listener : new ArrayList<>(mPostDialListeners)) {
                listener->OnPostDialWait();
            }
        }
    }

    public abstract PostDialState GetPostDialState();

    /**
     * Returns the portion of the post dial string that has not
     * yet been dialed, or "" if none
     */
    public abstract String GetRemainingPostDialString();

    /**
     * See Phone->SetOnPostDialWaitCharacter()
     */

    public abstract void ProceedAfterWaitChar();

    /**
     * See Phone->SetOnPostDialWildCharacter()
     */
    public abstract void ProceedAfterWildChar(String str);
    /**
     * Cancel any post
     */
    public abstract void CancelPostDial();

    /**
     * Returns the caller id presentation type for incoming and waiting calls
     * @return one of PRESENTATION_*
     */
    public abstract Int32 GetNumberPresentation();

    /**
     * Returns the User to User Signaling (UUS) information associated with
     * incoming and waiting calls
     * @return UUSInfo containing the UUS userdata.
     */
    public abstract UUSInfo GetUUSInfo();

    /**
     * Returns the CallFail reason provided by the RIL with the result of
     * RIL_REQUEST_LAST_CALL_FAIL_CAUSE
     */
    public abstract Int32 GetPreciseDisconnectCause();

    /**
     * Returns the original Connection instance associated with
     * this Connection
     */
    public Connection GetOrigConnection() {
        return mOrigConnection;
    }

    /**
     * Returns whether the original ImsPhoneConnection was a member
     * of a conference call
     * @return valid only when GetOrigConnection() is not NULL
     */
    public abstract Boolean IsMultiparty();

    CARAPI MigrateFrom(Connection c) {
        If (c == NULL) return;
        mListeners = c.mListeners;
        mAddress = c->GetAddress();
        mNumberPresentation = c->GetNumberPresentation();
        mDialString = c->GetOrigDialString();
        mCnapName = c->GetCnapName();
        mCnapNamePresentation = c->GetCnapNamePresentation();
        mIsIncoming = c->IsIncoming();
        mCreateTime = c->GetCreateTime();
        mConnectTime = c->GetConnectTime();
        mConnectTimeReal = c->GetConnectTimeReal();
        mHoldingStartTime = c->GetHoldingStartTime();
        mOrigConnection = c->GetOrigConnection();
    }

    /**
     * Assign a listener to be notified of state changes.
     *
     * @param listener A listener.
     */
    public final void AddListener(Listener listener) {
        mListeners->Add(listener);
    }

    /**
     * Removes a listener.
     *
     * @param listener A listener.
     */
    public final void RemoveListener(Listener listener) {
        mListeners->Remove(listener);
    }

    /**
     * Returns the current video state of the connection.
     *
     * @return The video state of the connection.
     */
    public Int32 GetVideoState() {
        return mVideoState;
    }

    /**
     * Returns the local video capability state for the connection.
     *
     * @return {@code True} if the connection has local video capabilities.
     */
    public Boolean IsLocalVideoCapable() {
        return mLocalVideoCapable;
    }

    /**
     * Returns the remote video capability state for the connection.
     *
     * @return {@code True} if the connection has remote video capabilities.
     */
    public Boolean IsRemoteVideoCapable() {
        return mRemoteVideoCapable;
    }

    /**
     * Returns the {@link android.telecom.Connection.VideoProvider} for the connection.
     *
     * @return The {@link android.telecom.Connection.VideoProvider}.
     */
    public android.telecom.Connection.VideoProvider GetVideoProvider() {
        return mVideoProvider;
    }

    /**
     * Returns the audio-quality for the connection.
     *
     * @return The audio quality for the connection.
     */
    public Int32 GetAudioQuality() {
        return mAudioQuality;
    }


    /**
     * Returns the current call substate of the connection.
     *
     * @return The call substate of the connection.
     */
    public Int32 GetCallSubstate() {
        return mCallSubstate;
    }


    /**
     * Sets the videoState for the current connection and reports the changes to all listeners.
     * Valid video states are defined in {@link android.telecom.VideoProfile}.
     *
     * @return The video state.
     */
    CARAPI SetVideoState(Int32 videoState) {
        mVideoState = videoState;
        For (Listener l : mListeners) {
            l->OnVideoStateChanged(mVideoState);
        }
    }

    /**
     * Sets whether video capability is present locally.
     *
     * @param capable {@code True} if video capable.
     */
    CARAPI SetLocalVideoCapable(Boolean capable) {
        mLocalVideoCapable = capable;
        For (Listener l : mListeners) {
            l->OnLocalVideoCapabilityChanged(mLocalVideoCapable);
        }
    }

    /**
     * Sets whether video capability is present remotely.
     *
     * @param capable {@code True} if video capable.
     */
    CARAPI SetRemoteVideoCapable(Boolean capable) {
        mRemoteVideoCapable = capable;
        For (Listener l : mListeners) {
            l->OnRemoteVideoCapabilityChanged(mRemoteVideoCapable);
        }
    }

    /**
     * Set the audio quality for the connection.
     *
     * @param audioQuality The audio quality.
     */
    CARAPI SetAudioQuality(Int32 audioQuality) {
        mAudioQuality = audioQuality;
        For (Listener l : mListeners) {
            l->OnAudioQualityChanged(mAudioQuality);
        }
    }

    /**
     * Sets the call substate for the current connection and reports the changes to all listeners.
     * Valid call substates are defined in {@link android.telecom.Connection}.
     *
     * @return The call substate.
     */
    CARAPI SetCallSubstate(Int32 callSubstate) {
        mCallSubstate = callSubstate;
        For (Listener l : mListeners) {
            l->OnCallSubstateChanged(mCallSubstate);
        }
    }

    /**
     * Sets the {@link android.telecom.Connection.VideoProvider} for the connection.
     *
     * @param videoProvider The video call provider.
     */
    CARAPI SetVideoProvider(android.telecom.Connection.VideoProvider videoProvider) {
        mVideoProvider = videoProvider;
        For (Listener l : mListeners) {
            l->OnVideoProviderChanged(mVideoProvider);
        }
    }

    CARAPI SetConverted(String oriNumber) {
        mNumberConverted = TRUE;
        mConvertedNumber = mAddress;
        mAddress = oriNumber;
        mDialString = oriNumber;
    }

    /**
     * Notifies listeners of a change to conference Participant(s).
     *
     * @param conferenceParticipants The Participant(s).
     */
    CARAPI UpdateConferenceParticipants(List<ConferenceParticipant> conferenceParticipants) {
        For (Listener l : mListeners) {
            l->OnConferenceParticipantsChanged(conferenceParticipants);
        }
    }

    /**
     * Notifies this Connection of a request to disconnect a participant of the conference managed
     * by the connection.
     *
     * @param endpoint the {@link Uri} of the participant to disconnect.
     */
    CARAPI OnDisconnectConferenceParticipant(Uri endpoint) {
    }

    /**
     * Build a human representation of a connection instance, suitable for debugging.
     * Don't log personal stuff unless in debug mode.
     * @return a string representing the internal state of this connection.
     */
    CARAPI ToString(
        /* [out] */ String* str)
    {
        StringBuilder str = new StringBuilder(128);

        If (Rlog->IsLoggable(LOG_TAG, Log.DEBUG)) {
            str->Append("addr: " + GetAddress())
                    .Append(" pres.: " + GetNumberPresentation())
                    .Append(" dial: " + GetOrigDialString())
                    .Append(" postdial: " + GetRemainingPostDialString())
                    .Append(" cnap name: " + GetCnapName())
                    .Append("(" + GetCnapNamePresentation() + ")");
        }
        str->Append(" incoming: " + IsIncoming())
                .Append(" state: " + GetState())
                .Append(" post dial state: " + GetPostDialState());
        return str->ToString();
    }
}
