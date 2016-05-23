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
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;

using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IRegistrant;
using Elastos::Droid::Os::IRegistrantList;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Preference::IPreferenceManager;
using Elastos::Droid::Telecom::IConferenceParticipant;
using Elastos::Droid::Telecom::IVideoProfile;
using Elastos::Droid::Telephony::IDisconnectCause;
using Elastos::Droid::Telephony::IPhoneNumberUtils;
using Elastos::Droid::Telephony::IRlog;
using Elastos::Droid::Telephony::IServiceState;

using Elastos::Droid::Ims::IImsCall;
using Elastos::Droid::Ims::IImsCallProfile;
using Elastos::Droid::Ims::IImsConfig;
using Elastos::Droid::Ims::IImsConnectionStateListener;
using Elastos::Droid::Ims::IImsEcbm;
using Elastos::Droid::Ims::IImsManager;
using Elastos::Droid::Ims::IImsReasonInfo;
using Elastos::Droid::Ims::IImsServiceClass;
using Elastos::Droid::Ims::IImsUtInterface;
using Elastos::Droid::Ims::Internal::ICallGroup;
using Elastos::Droid::Ims::Internal::IIImsVideoCallProvider;
using Elastos::Droid::Ims::Internal::IImsCallSession;
using Elastos::Droid::Ims::Internal::IImsVideoCallProviderWrapper;
using Elastos::Droid::Internal::Telephony::ICall;
using Elastos::Droid::Internal::Telephony::ICallTracker;
using Elastos::Droid::Internal::Telephony::ICommandsInterface;
using Elastos::Droid::Internal::Telephony::IConnection;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Internal::Telephony::IPhoneBase;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Internal::Telephony::ITelephonyProperties;
using Elastos::Droid::Internal::Telephony::Gsm::ISuppServiceNotification;

/**
 * {@hide}
 */
public class ImsPhoneCallTracker extends CallTracker {
    static const String LOG_TAG = "ImsPhoneCallTracker";

    private static const Boolean DBG = TRUE;

    private Boolean mIsVolteEnabled = FALSE;
    private Boolean mIsVtEnabled = FALSE;

    private BroadcastReceiver mReceiver = new BroadcastReceiver() {
        //@Override
        CARAPI OnReceive(Context context, Intent intent) {
            If (intent->GetAction()->Equals(ImsManager.ACTION_IMS_INCOMING_CALL)) {
                If (DBG) Log("onReceive : incoming call intent");

                If (mImsManager == NULL) return;

                If (mServiceId < 0) return;

                try {
                    // Network initiated USSD will be treated by mImsUssdListener
                    Boolean isUssd = intent->GetBooleanExtra(ImsManager.EXTRA_USSD, FALSE);
                    If (isUssd) {
                        If (DBG) Log("onReceive : USSD");
                        mUssdSession = mImsManager->TakeCall(mServiceId, intent, mImsUssdListener);
                        If (mUssdSession != NULL) {
                            mUssdSession->Accept(ImsCallProfile.CALL_TYPE_VOICE);
                        }
                        return;
                    }

                    // Normal MT call
                    ImsCall imsCall = mImsManager->TakeCall(mServiceId, intent, mImsCallListener);
                    ImsPhoneConnection conn = new ImsPhoneConnection(mPhone->GetContext(), imsCall,
                            ImsPhoneCallTracker.this, mRingingCall);
                    AddConnection(conn);

                    IImsVideoCallProvider imsVideoCallProvider =
                            imsCall->GetCallSession()->GetVideoCallProvider();
                    If (imsVideoCallProvider != NULL) {
                        ImsVideoCallProviderWrapper imsVideoCallProviderWrapper =
                                new ImsVideoCallProviderWrapper(imsVideoCallProvider);
                        conn->SetVideoProvider(imsVideoCallProviderWrapper);
                    }

                    If ((mForegroundCall->GetState() != ImsPhoneCall.State.IDLE) ||
                            (mBackgroundCall->GetState() != ImsPhoneCall.State.IDLE)) {
                        conn->Update(imsCall, ImsPhoneCall.State.WAITING);
                    }

                    mPhone->NotifyNewRingingConnection(conn);
                    mPhone->NotifyIncomingRing();

                    UpdatePhoneState();
                    mPhone->NotifyPreciseCallStateChanged();
                } Catch (ImsException e) {
                    Loge("onReceive : exception " + e);
                } Catch (RemoteException e) {
                }
            }
        }
    };

    //***** Constants

    static const Int32 MAX_CONNECTIONS = 7;
    static const Int32 MAX_CONNECTIONS_PER_CALL = 5;

    private static const Int32 EVENT_HANGUP_PENDINGMO = 18;
    private static const Int32 EVENT_RESUME_BACKGROUND = 19;
    private static const Int32 EVENT_DIAL_PENDINGMO = 20;

    private static const Int32 TIMEOUT_HANGUP_PENDINGMO = 500;

    //***** Instance Variables
    private ArrayList<ImsPhoneConnection> mConnections = new ArrayList<ImsPhoneConnection>();
    private RegistrantList mVoiceCallEndedRegistrants = new RegistrantList();
    private RegistrantList mVoiceCallStartedRegistrants = new RegistrantList();

    ImsPhoneCall mRingingCall = new ImsPhoneCall(this);
    ImsPhoneCall mForegroundCall = new ImsPhoneCall(this);
    ImsPhoneCall mBackgroundCall = new ImsPhoneCall(this);
    ImsPhoneCall mHandoverCall = new ImsPhoneCall(this);

    private ImsPhoneConnection mPendingMO;
    private Int32 mClirMode = CommandsInterface.CLIR_DEFAULT;
    private Object mSyncHold = new Object();

    private ImsCall mUssdSession = NULL;
    private Message mPendingUssd = NULL;

    ImsPhone mPhone;

    private Boolean mDesiredMute = FALSE;    // FALSE = mute off
    private Boolean mOnHoldToneStarted = FALSE;

    PhoneConstants.State mState = PhoneConstants.State.IDLE;

    private ImsManager mImsManager;
    private Int32 mServiceId = -1;

    private Call.SrvccState mSrvccState = Call.SrvccState.NONE;

    private Boolean mIsInEmergencyCall = FALSE;

    private Int32 pendingCallClirMode;
    private Int32 mPendingCallVideoState;
    private Boolean pendingCallInEcm = FALSE;

    //***** Events


    //***** Constructors

    ImsPhoneCallTracker(ImsPhone phone) {
        this.mPhone = phone;

        IntentFilter intentfilter = new IntentFilter();
        intentfilter->AddAction(ImsManager.ACTION_IMS_INCOMING_CALL);
        mPhone->GetContext()->RegisterReceiver(mReceiver, intentfilter);

        Thread t = new Thread() {
            CARAPI Run() {
                GetImsService();
            }
        };
        t->Start();
    }

    private PendingIntent CreateIncomingCallPendingIntent() {
        Intent intent = new Intent(ImsManager.ACTION_IMS_INCOMING_CALL);
        intent->AddFlags(IIntent::FLAG_RECEIVER_FOREGROUND);
        return PendingIntent->GetBroadcast(mPhone->GetContext(), 0, intent,
                PendingIIntent::FLAG_UPDATE_CURRENT);
    }

    private void GetImsService() {
        If (DBG) Log("getImsService");
        mImsManager = ImsManager->GetInstance(mPhone->GetContext(), mPhone->GetSubId());
        try {
            mServiceId = mImsManager->Open(ImsServiceClass.MMTEL,
                    CreateIncomingCallPendingIntent(),
                    mImsConnectionStateListener);

            // Get the ECBM interface and set IMSPhone's listener object for notifications
            GetEcbmInterface()->SetEcbmStateListener(mPhone.mImsEcbmStateListener);
            If (mPhone->IsInEcm()) {
                // Call exit ECBM which will invoke onECBMExited
                mPhone->ExitEmergencyCallbackMode();
            }
            Int32 mPreferredTtyMode = Settings.Secure->GetInt(
                mPhone->GetContext()->GetContentResolver(),
                Settings.Secure.PREFERRED_TTY_MODE,
                Phone.TTY_MODE_OFF);
           mImsManager->SetUiTTYMode(mServiceId, mPreferredTtyMode, NULL);

        } Catch (ImsException e) {
            Loge("getImsService: " + e);
            //Leave mImsManager as NULL, then CallStateException will be thrown when dialing
            mImsManager = NULL;
        }
    }

    CARAPI Dispose() {
        If (DBG) Log("dispose");
        mRingingCall->Dispose();
        mBackgroundCall->Dispose();
        mForegroundCall->Dispose();
        mHandoverCall->Dispose();

        ClearDisconnected();
        mPhone->GetContext()->UnregisterReceiver(mReceiver);
    }

    //@Override
    protected void Finalize() {
        Log("ImsPhoneCallTracker finalized");
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

    Connection
    Dial(String dialString, Int32 videoState) throws CallStateException {
        return Dial(dialString, videoState, NULL);
    }

    Connection
    Dial(String dialString, Int32 videoState, Bundle extras) throws CallStateException {
        SharedPreferences sp = PreferenceManager->GetDefaultSharedPreferences(mPhone->GetContext());
        Int32 oirMode = sp->GetInt(PhoneBase.CLIR_KEY, CommandsInterface.CLIR_DEFAULT);
        return Dial(dialString, oirMode, videoState, extras);
    }

    /**
     * oirMode is one of the CLIR_ constants
     */
    synchronized Connection
    Dial(String dialString, Int32 clirMode, Int32 videoState, Bundle extras) throws CallStateException {
        Boolean isPhoneInEcmMode = SystemProperties->GetBoolean(
                TelephonyProperties.PROPERTY_INECM_MODE, FALSE);
        Boolean isEmergencyNumber = PhoneNumberUtils->IsEmergencyNumber(dialString);

        If (DBG) Log("dial clirMode=" + clirMode);

        // note that this triggers call state changed notif
        ClearDisconnected();

        If (mImsManager == NULL) {
            throw new CallStateException("service not available");
        }

        If (!CanDial()) {
            throw new CallStateException("cannot dial in current state");
        }

        If (isPhoneInEcmMode && isEmergencyNumber) {
            HandleEcmTimer(ImsPhone.CANCEL_ECM_TIMER);
        }

        Boolean holdBeforeDial = FALSE;

        // The new call must be assigned to the foreground call.
        // That call must be idle, so place anything that's
        // there on hold
        If (mForegroundCall->GetState() == ImsPhoneCall.State.ACTIVE) {
            If (mBackgroundCall->GetState() != ImsPhoneCall.State.IDLE) {
                //we should have failed in !CanDial() above before we get here
                throw new CallStateException("cannot dial in current state");
            }
            // foreground call is empty for the newly dialed connection
            holdBeforeDial = TRUE;
            // Cache the video state for pending MO call.
            mPendingCallVideoState = videoState;
            SwitchWaitingOrHoldingAndActive();
        }

        ImsPhoneCall.State fgState = ImsPhoneCall.State.IDLE;
        ImsPhoneCall.State bgState = ImsPhoneCall.State.IDLE;

        mClirMode = clirMode;

        {    AutoLock syncLock(mSyncHold);
            If (holdBeforeDial) {
                fgState = mForegroundCall->GetState();
                bgState = mBackgroundCall->GetState();

                //holding foreground call failed
                If (fgState == ImsPhoneCall.State.ACTIVE) {
                    throw new CallStateException("cannot dial in current state");
                }

                //holding foreground call succeeded
                If (bgState == ImsPhoneCall.State.HOLDING) {
                    holdBeforeDial = FALSE;
                }
            }

            mPendingMO = new ImsPhoneConnection(mPhone->GetContext(),
                    CheckForTestEmergencyNumber(dialString), this,
                    mForegroundCall, extras);
        }
        AddConnection(mPendingMO);

        If (!holdBeforeDial) {
            If ((!isPhoneInEcmMode) || (isPhoneInEcmMode && isEmergencyNumber)) {
                DialInternal(mPendingMO, clirMode, videoState, extras);
            } else {
                try {
                    GetEcbmInterface()->ExitEmergencyCallbackMode();
                } Catch (ImsException e) {
                    e->PrintStackTrace();
                    throw new CallStateException("service not available");
                }
                mPhone->SetOnEcbModeExitResponse(this, EVENT_EXIT_ECM_RESPONSE_CDMA, NULL);
                pendingCallClirMode = clirMode;
                mPendingCallVideoState = videoState;
                pendingCallInEcm = TRUE;
            }
        }

        UpdatePhoneState();
        mPhone->NotifyPreciseCallStateChanged();

        return mPendingMO;
    }

    CARAPI
    AddParticipant(String dialString) throws CallStateException {
        If (mForegroundCall != NULL) {
            ImsCall imsCall = mForegroundCall->GetImsCall();
            If (imsCall == NULL) {
                Loge("addParticipant : No foreground ims call");
            } else {
                ImsCallSession imsCallSession = imsCall->GetCallSession();
                If (imsCallSession != NULL) {
                    String[] callees = new String[] { dialString };
                    imsCallSession->InviteParticipants(callees);
                } else {
                    Loge("addParticipant : ImsCallSession does not exist");
                }
            }
        } else {
            Loge("addParticipant : Foreground call does not exist");
        }
    }

    private void HandleEcmTimer(Int32 action) {
        mPhone->HandleTimerInEmergencyCallbackMode(action);
        Switch (action) {
            case ImsPhone.CANCEL_ECM_TIMER:
                break;
            case ImsPhone.RESTART_ECM_TIMER:
                break;
            default:
                Log("handleEcmTimer, unsupported action " + action);
        }
    }

    private void
    DialInternal(ImsPhoneConnection conn, Int32 clirMode, Int32 videoState, Bundle extras) {
        If (conn == NULL) {
            return;
        }

        Boolean isConferenceUri = FALSE;
        Boolean isSkipSchemaParsing = FALSE;
        If (extras != NULL) {
            isConferenceUri = extras->GetBoolean(TelephonyProperties.EXTRA_DIAL_CONFERENCE_URI,
                    FALSE);
            isSkipSchemaParsing = extras->GetBoolean(TelephonyProperties.EXTRA_SKIP_SCHEMA_PARSING,
                    FALSE);
        }
        If (!isConferenceUri && !isSkipSchemaParsing && (conn->GetAddress()== NULL
                || conn->GetAddress()->Length() == 0
                || conn->GetAddress()->IndexOf(PhoneNumberUtils.WILD) >= 0)) {
            // Phone number is invalid
            conn->SetDisconnectCause(DisconnectCause.INVALID_NUMBER);
            SendEmptyMessageDelayed(EVENT_HANGUP_PENDINGMO, TIMEOUT_HANGUP_PENDINGMO);
            return;
        }

        // Always unmute when initiating a new call
        SetMute(FALSE);
        Int32 serviceType = PhoneNumberUtils->IsEmergencyNumber(conn->GetAddress()) ?
                ImsCallProfile.SERVICE_TYPE_EMERGENCY : ImsCallProfile.SERVICE_TYPE_NORMAL;
        Int32 callType = ImsCallProfile->GetCallTypeFromVideoState(videoState);
        //TODO(vt): Is this sufficient?  At what point do we know the video state of the call?
        conn->SetVideoState(videoState);

        try {
            String[] callees = new String[] { conn->GetAddress() };
            ImsCallProfile profile = mImsManager->CreateCallProfile(mServiceId,
                    serviceType, callType);
            profile->SetCallExtraInt(ImsCallProfile.EXTRA_OIR, clirMode);
            profile->SetCallExtraBoolean(TelephonyProperties.EXTRAS_IS_CONFERENCE_URI,
                    isConferenceUri);

            If (extras != NULL) {
                // Pack the OEM-specific call extras.
                profile.mCallExtras->PutBundle(ImsCallProfile.EXTRA_OEM_EXTRAS, extras);
                Log("Packing OEM extras bundle in call profile.");
            } else {
                Log("No dial extras packed in call profile.");
            }

            ImsCall imsCall = mImsManager->MakeCall(mServiceId, profile,
                    callees, mImsCallListener);
            conn->SetImsCall(imsCall);

            IImsVideoCallProvider imsVideoCallProvider =
                    imsCall->GetCallSession()->GetVideoCallProvider();
            If (imsVideoCallProvider != NULL) {
                ImsVideoCallProviderWrapper imsVideoCallProviderWrapper =
                        new ImsVideoCallProviderWrapper(imsVideoCallProvider);
                conn->SetVideoProvider(imsVideoCallProviderWrapper);
            }
        } Catch (ImsException e) {
            Loge("dialInternal : " + e);
            conn->SetDisconnectCause(DisconnectCause.ERROR_UNSPECIFIED);
            SendEmptyMessageDelayed(EVENT_HANGUP_PENDINGMO, TIMEOUT_HANGUP_PENDINGMO);
        } Catch (RemoteException e) {
        }
    }

    /**
     * Accepts a call with the specified video state.  The video state is the video state that the
     * user has agreed upon in the InCall UI.
     *
     * @param videoState The video State
     * @throws CallStateException
     */
    void AcceptCall (Int32 videoState) throws CallStateException {
        If (DBG) Log("acceptCall");

        If (mForegroundCall->GetState()->IsAlive()
                && mBackgroundCall->GetState()->IsAlive()) {
            throw new CallStateException("cannot accept call");
        }

        If ((mRingingCall->GetState() == ImsPhoneCall.State.WAITING)
                && mForegroundCall->GetState()->IsAlive()) {
            SetMute(FALSE);
            // Cache video state for pending MT call.
            mPendingCallVideoState = videoState;
            SwitchWaitingOrHoldingAndActive();
        } else If (mRingingCall->GetState()->IsRinging()) {
            If (DBG) Log("acceptCall: incoming...");
            // Always unmute when answering a new call
            SetMute(FALSE);
            try {
                ImsCall imsCall = mRingingCall->GetImsCall();
                If (imsCall != NULL) {
                    imsCall->Accept(ImsCallProfile->GetCallTypeFromVideoState(videoState));
                } else {
                    throw new CallStateException("no valid ims call");
                }
            } Catch (ImsException e) {
                throw new CallStateException("cannot accept call");
            }
        } else {
            throw new CallStateException("phone not ringing");
        }
    }

    void
    DeflectCall (String number) throws CallStateException {
        If (DBG) Log("deflectCall");

        If (mRingingCall->GetState()->IsRinging()) {
            try {
                ImsCall imsCall = mRingingCall->GetImsCall();
                If (imsCall != NULL) {
                    imsCall->Deflect(number);
                } else {
                    throw new CallStateException("no valid ims call to deflect");
                }
            } Catch (ImsException e) {
                throw new CallStateException("cannot deflect call");
            }
        } else {
            throw new CallStateException("phone not ringing");
        }
    }

    void
    RejectCall () throws CallStateException {
        If (DBG) Log("rejectCall");

        If (mRingingCall->GetState()->IsRinging()) {
            Hangup(mRingingCall);
        } else {
            throw new CallStateException("phone not ringing");
        }
    }

    void
    SwitchWaitingOrHoldingAndActive() throws CallStateException {
        If (DBG) Log("switchWaitingOrHoldingAndActive");

        If (mRingingCall->GetState() == ImsPhoneCall.State.INCOMING) {
            throw new CallStateException("cannot be in the incoming state");
        }

        If (mForegroundCall->GetState() == ImsPhoneCall.State.ACTIVE) {
            ImsCall imsCall = mForegroundCall->GetImsCall();
            If (imsCall == NULL) {
                throw new CallStateException("no ims call");
            }

            mForegroundCall->SwitchWith(mBackgroundCall);

            try {
                imsCall->Hold();
            } Catch (ImsException e) {
                mForegroundCall->SwitchWith(mBackgroundCall);
                throw new CallStateException(e->GetMessage());
            }
        } else If (mBackgroundCall->GetState() == ImsPhoneCall.State.HOLDING) {
            ResumeWaitingOrHolding();
        }
    }

    void
    Conference() {
        If (DBG) Log("conference");

        ImsCall fgImsCall = mForegroundCall->GetImsCall();
        If (fgImsCall == NULL) {
            Log("conference no foreground ims call");
            return;
        }

        ImsCall bgImsCall = mBackgroundCall->GetImsCall();
        If (bgImsCall == NULL) {
            Log("conference no background ims call");
            return;
        }

        try {
            fgImsCall->Merge(bgImsCall);
        } Catch (ImsException e) {
            Log("conference " + e->GetMessage());
        }
    }

    void
    ExplicitCallTransfer() {
        //TODO : implement
    }

    void
    ClearDisconnected() {
        If (DBG) Log("clearDisconnected");

        InternalClearDisconnected();

        UpdatePhoneState();
        mPhone->NotifyPreciseCallStateChanged();
    }

    Boolean
    CanConference() {
        return mForegroundCall->GetState() == ImsPhoneCall.State.ACTIVE
            && mBackgroundCall->GetState() == ImsPhoneCall.State.HOLDING
            && !mBackgroundCall->IsFull()
            && !mForegroundCall->IsFull();
    }

    Boolean
    CanDial() {
        Boolean ret;
        Int32 serviceState = mPhone->GetServiceState()->GetState();
        String disableCall = SystemProperties->Get(
                TelephonyProperties.PROPERTY_DISABLE_CALL, "FALSE");

        Rlog->D(LOG_TAG, "CanDial(): "
                + "\nserviceState = " + serviceState
                + "\npendingMO == NULL::=" + String->ValueOf(mPendingMO == NULL)
                + "\nringingCall: " + mRingingCall->GetState()
                + "\ndisableCall = " + disableCall
                + "\nforegndCall: " + mForegroundCall->GetState()
                + "\nbackgndCall: " + mBackgroundCall->GetState());

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
        return mForegroundCall->GetState() == ImsPhoneCall.State.ACTIVE
            && mBackgroundCall->GetState() == ImsPhoneCall.State.HOLDING;
    }

    //***** Private Instance Methods

    private void
    InternalClearDisconnected() {
        mRingingCall->ClearDisconnected();
        mForegroundCall->ClearDisconnected();
        mBackgroundCall->ClearDisconnected();
        mHandoverCall->ClearDisconnected();
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

        If (DBG) Log("updatePhoneState oldState=" + oldState + ", newState=" + mState);

        If (mState != oldState) {
            mPhone->NotifyPhoneStateChanged();
        }
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

        Log("Phone State:" + mState);

        Log("Ringing call: " + mRingingCall->ToString());

        l = mRingingCall->GetConnections();
        For (Int32 i = 0, s = l->Size(); i < s; i++) {
            Log(l->Get(i).ToString());
        }

        Log("Foreground call: " + mForegroundCall->ToString());

        l = mForegroundCall->GetConnections();
        For (Int32 i = 0, s = l->Size(); i < s; i++) {
            Log(l->Get(i).ToString());
        }

        Log("Background call: " + mBackgroundCall->ToString());

        l = mBackgroundCall->GetConnections();
        For (Int32 i = 0, s = l->Size(); i < s; i++) {
            Log(l->Get(i).ToString());
        }

    }

    //***** Called from ImsPhone

    void SetUiTTYMode(Int32 uiTtyMode, Message onComplete) {
        try {
            mImsManager->SetUiTTYMode(mServiceId, uiTtyMode, onComplete);
        } Catch (ImsException e) {
            Loge("setTTYMode : " + e);
            mPhone->SendErrorResponse(onComplete, e);
        }
    }

    /*package*/ void
    SetMute(Boolean mute) {
        mDesiredMute = mute;
        mForegroundCall->SetMute(mute);
    }

    /*package*/ Boolean
    GetMute() {
        return mDesiredMute;
    }

    /*package*/ void
    SendDtmf(Char32 c) {
        If (DBG) Log("sendDtmf");

        ImsCall imscall = mForegroundCall->GetImsCall();
        If (imscall != NULL) {
            imscall->SendDtmf(c);
        }
    }

    /*package*/ void
    StartDtmf(Char32 c) {
        If (DBG) Log("startDtmf");

        ImsCall imscall = mForegroundCall->GetImsCall();
        If (imscall != NULL) {
            imscall->StartDtmf(c);
        } else {
            Loge("startDtmf : no foreground call");
        }
    }

    /*package*/ void
    StopDtmf() {
        If (DBG) Log("stopDtmf");

        ImsCall imscall = mForegroundCall->GetImsCall();
        If (imscall != NULL) {
            imscall->StopDtmf();
        } else {
            Loge("stopDtmf : no foreground call");
        }
    }

    //***** Called from ImsPhoneConnection

    /*package*/ void
    Hangup (ImsPhoneConnection conn) throws CallStateException {
        If (DBG) Log("hangup connection");

        If (conn->GetOwner() != this) {
            throw new CallStateException ("ImsPhoneConnection " + conn
                    + "does not belong to ImsPhoneCallTracker " + this);
        }

        Hangup(conn->GetCall());
    }

    //***** Called from ImsPhoneCall

    /* package */ void
    Hangup (ImsPhoneCall call) throws CallStateException {
        If (DBG) Log("hangup call");

        If (call->GetConnections()->Size() == 0) {
            throw new CallStateException("no connections");
        }

        ImsCall imsCall = call->GetImsCall();
        Boolean rejectCall = FALSE;

        If (call == mRingingCall) {
            If (Phone.DEBUG_PHONE) Log("(ringing) hangup incoming");
            rejectCall = TRUE;
        } else If (call == mForegroundCall) {
            If (call->IsDialingOrAlerting()) {
                If (Phone.DEBUG_PHONE) {
                    Log("(foregnd) hangup dialing or alerting...");
                }
            } else {
                If (Phone.DEBUG_PHONE) {
                    Log("(foregnd) hangup foreground");
                }
                //held call will be resumed by onCallTerminated
            }
        } else If (call == mBackgroundCall) {
            If (Phone.DEBUG_PHONE) {
                Log("(backgnd) hangup waiting or background");
            }
        } else {
            throw new CallStateException ("ImsPhoneCall " + call +
                    "does not belong to ImsPhoneCallTracker " + this);
        }

        call->OnHangupLocal();

        try {
            If (imsCall != NULL) {
                If (rejectCall) imsCall->Reject(ImsReasonInfo.CODE_USER_DECLINE);
                else imsCall->Terminate(ImsReasonInfo.CODE_USER_TERMINATED);
            } else If (mPendingMO != NULL && call == mForegroundCall) {
                // is holding a foreground call
                mPendingMO->Update(NULL, ImsPhoneCall.State.DISCONNECTED);
                mPendingMO->OnDisconnect();
                RemoveConnection(mPendingMO);
                mPendingMO = NULL;
                UpdatePhoneState();
                RemoveMessages(EVENT_DIAL_PENDINGMO);
            }
        } Catch (ImsException e) {
            throw new CallStateException(e->GetMessage());
        }

        mPhone->NotifyPreciseCallStateChanged();
    }

    /* package */
    void ResumeWaitingOrHolding() throws CallStateException {
        If (DBG) Log("resumeWaitingOrHolding");

        try {
            If (mForegroundCall->GetState()->IsAlive()) {
                //resume foreground call after holding background call
                //they were switched before holding
                ImsCall imsCall = mForegroundCall->GetImsCall();
                If (imsCall != NULL) imsCall->Resume();
            } else If (mRingingCall->GetState() == ImsPhoneCall.State.WAITING) {
                //accept waiting call after holding background call
                ImsCall imsCall = mRingingCall->GetImsCall();
                If (imsCall != NULL) {
                    imsCall->Accept(
                        ImsCallProfile->GetCallTypeFromVideoState(mPendingCallVideoState));
                }
            } else {
                //Just resume background call.
                //To distinguish resuming call with swapping calls
                //we do not switch calls.here
                //ImsPhoneConnection.update will chnage the parent when completed
                ImsCall imsCall = mBackgroundCall->GetImsCall();
                If (imsCall != NULL) imsCall->Resume();
            }
        } Catch (ImsException e) {
            throw new CallStateException(e->GetMessage());
        }
    }

    /* package */
    void SendUSSD (String ussdString, Message response) {
        If (DBG) Log("sendUSSD");

        try {
            If (mUssdSession != NULL) {
                mUssdSession->SendUssd(ussdString);
                AsyncResult->ForMessage(response, NULL, NULL);
                response->SendToTarget();
                return;
            }

            String[] callees = new String[] { ussdString };
            ImsCallProfile profile = mImsManager->CreateCallProfile(mServiceId,
                    ImsCallProfile.SERVICE_TYPE_NORMAL, ImsCallProfile.CALL_TYPE_VOICE);
            profile->SetCallExtraInt(ImsCallProfile.EXTRA_DIALSTRING,
                    ImsCallProfile.DIALSTRING_USSD);

            mUssdSession = mImsManager->MakeCall(mServiceId, profile,
                    callees, mImsUssdListener);
        } Catch (ImsException e) {
            Loge("sendUSSD : " + e);
            mPhone->SendErrorResponse(response, e);
        }
    }

    /* package */
    void CancelUSSD() {
        If (mUssdSession == NULL) return;

        try {
            mUssdSession->Terminate(ImsReasonInfo.CODE_USER_TERMINATED);
        } Catch (ImsException e) {
        }

    }

    private synchronized ImsPhoneConnection FindConnection(ImsCall imsCall) {
        For (ImsPhoneConnection conn : mConnections) {
            If (conn->GetImsCall() == imsCall) {
                return conn;
            }
        }
        return NULL;
    }

    private synchronized void RemoveConnection(ImsPhoneConnection conn) {
        mConnections->Remove(conn);
    }

    private synchronized void AddConnection(ImsPhoneConnection conn) {
        mConnections->Add(conn);
    }

    private void ProcessCallStateChange(ImsCall imsCall, ImsPhoneCall.State state, Int32 cause) {
        If (DBG) Log("processCallStateChange state=" + state + " cause=" + cause);

        If (imsCall == NULL) return;

        Boolean changed = FALSE;
        ImsPhoneConnection conn = FindConnection(imsCall);

        If (conn == NULL) {
            // TODO : what should be done?
            return;
        }

        changed = conn->Update(imsCall, state);

        If (state == ImsPhoneCall.State.DISCONNECTED) {
            changed = conn->OnDisconnect(cause) || changed;
            RemoveConnection(conn);
        }

        If (changed) {
            If (conn->GetCall() == mHandoverCall) return;
            UpdatePhoneState();
            mPhone->NotifyPreciseCallStateChanged();
        }
    }

    private Int32 GetDisconnectCauseFromReasonInfo(ImsReasonInfo reasonInfo) {
        Int32 cause = DisconnectCause.ERROR_UNSPECIFIED;

        //Int32 type = reasonInfo->GetReasonType();
        Int32 code = reasonInfo->GetCode();
        Switch (code) {
            case ImsReasonInfo.CODE_SIP_BAD_ADDRESS:
            case ImsReasonInfo.CODE_SIP_NOT_REACHABLE:
                return DisconnectCause.NUMBER_UNREACHABLE;

            case ImsReasonInfo.CODE_SIP_BUSY:
                return DisconnectCause.BUSY;

            case ImsReasonInfo.CODE_USER_TERMINATED:
                return DisconnectCause.LOCAL;

            case ImsReasonInfo.CODE_USER_TERMINATED_BY_REMOTE:
                return DisconnectCause.NORMAL;

            case ImsReasonInfo.CODE_SIP_REDIRECTED:
            case ImsReasonInfo.CODE_SIP_BAD_REQUEST:
            case ImsReasonInfo.CODE_SIP_FORBIDDEN:
            case ImsReasonInfo.CODE_SIP_NOT_ACCEPTABLE:
            case ImsReasonInfo.CODE_SIP_USER_REJECTED:
            case ImsReasonInfo.CODE_SIP_GLOBAL_ERROR:
                return DisconnectCause.SERVER_ERROR;

            case ImsReasonInfo.CODE_SIP_SERVICE_UNAVAILABLE:
            case ImsReasonInfo.CODE_SIP_NOT_FOUND:
            case ImsReasonInfo.CODE_SIP_SERVER_ERROR:
                return DisconnectCause.SERVER_UNREACHABLE;

            case ImsReasonInfo.CODE_LOCAL_NETWORK_ROAMING:
            case ImsReasonInfo.CODE_LOCAL_NETWORK_IP_CHANGED:
            case ImsReasonInfo.CODE_LOCAL_IMS_SERVICE_DOWN:
            case ImsReasonInfo.CODE_LOCAL_SERVICE_UNAVAILABLE:
            case ImsReasonInfo.CODE_LOCAL_NOT_REGISTERED:
            case ImsReasonInfo.CODE_LOCAL_NETWORK_NO_LTE_COVERAGE:
            case ImsReasonInfo.CODE_LOCAL_NETWORK_NO_SERVICE:
            case ImsReasonInfo.CODE_LOCAL_CALL_VCC_ON_PROGRESSING:
                return DisconnectCause.OUT_OF_SERVICE;

            case ImsReasonInfo.CODE_SIP_REQUEST_TIMEOUT:
            case ImsReasonInfo.CODE_TIMEOUT_1XX_WAITING:
            case ImsReasonInfo.CODE_TIMEOUT_NO_ANSWER:
            case ImsReasonInfo.CODE_TIMEOUT_NO_ANSWER_CALL_UPDATE:
                return DisconnectCause.TIMED_OUT;

            case ImsReasonInfo.CODE_LOCAL_LOW_BATTERY:
            case ImsReasonInfo.CODE_LOCAL_POWER_OFF:
                return DisconnectCause.POWER_OFF;

            default:
        }

        return cause;
    }

    /**
     * Listen to the IMS call state change
     */
    private ImsCall.Listener mImsCallListener = new ImsCall->Listener() {
        //@Override
        CARAPI OnCallProgressing(ImsCall imsCall) {
            If (DBG) Log("onCallProgressing");

            mPendingMO = NULL;
            ProcessCallStateChange(imsCall, ImsPhoneCall.State.ALERTING,
                    DisconnectCause.NOT_DISCONNECTED);
        }

        //@Override
        CARAPI OnCallStarted(ImsCall imsCall) {
            If (DBG) Log("onCallStarted");

            mPendingMO = NULL;
            ProcessCallStateChange(imsCall, ImsPhoneCall.State.ACTIVE,
                    DisconnectCause.NOT_DISCONNECTED);
        }

        //@Override
        CARAPI OnCallUpdated(ImsCall imsCall) {
            If (DBG) Log("onCallUpdated");
            If (imsCall == NULL) {
                return;
            }
            ImsPhoneConnection conn = FindConnection(imsCall);
            If (conn != NULL) {
                ProcessCallStateChange(imsCall, conn->GetCall().mState,
                        DisconnectCause.NOT_DISCONNECTED);
            }
        }

        /**
         * onCallStartFailed will be invoked when:
         * case 1) Dialing fails
         * case 2) Ringing call is disconnected by local or remote user
         */
        //@Override
        CARAPI OnCallStartFailed(ImsCall imsCall, ImsReasonInfo reasonInfo) {
            If (DBG) Log("onCallStartFailed reasonCode=" + reasonInfo->GetCode());

            If (mPendingMO != NULL) {
                // To initiate dialing circuit-switched call
                If (reasonInfo->GetCode() == ImsReasonInfo.CODE_LOCAL_CALL_CS_RETRY_REQUIRED
                        && mBackgroundCall->GetState() == ImsPhoneCall.State.IDLE
                        && mRingingCall->GetState() == ImsPhoneCall.State.IDLE) {
                    mForegroundCall->Detach(mPendingMO);
                    RemoveConnection(mPendingMO);
                    mPendingMO->Finalize();
                    mPendingMO = NULL;
                    mPhone->InitiateSilentRedial();
                    return;
                }
                mPendingMO->SetDisconnectCause(DisconnectCause.ERROR_UNSPECIFIED);
                SendEmptyMessageDelayed(EVENT_HANGUP_PENDINGMO, TIMEOUT_HANGUP_PENDINGMO);
            }
        }

        //@Override
        CARAPI OnCallTerminated(ImsCall imsCall, ImsReasonInfo reasonInfo) {
            If (DBG) Log("onCallTerminated reasonCode=" + reasonInfo->GetCode());

            ImsPhoneCall.State oldState = mForegroundCall->GetState();
            Int32 cause = GetDisconnectCauseFromReasonInfo(reasonInfo);
            ImsPhoneConnection conn = FindConnection(imsCall);
            If (DBG) Log("cause = " + cause + " conn = " + conn);

            If (conn != NULL && conn->IsIncoming() && conn->GetConnectTime() == 0) {
                // Missed or rejected call
                If (cause == DisconnectCause.LOCAL) {
                    cause = DisconnectCause.INCOMING_REJECTED;
                } else {
                    cause = DisconnectCause.INCOMING_MISSED;
                }
            }

            If (cause == DisconnectCause.NORMAL && conn != NULL && conn->GetImsCall()->IsMerged()) {
                // Call was terminated while it is merged instead of a remote disconnect.
                cause = DisconnectCause.IMS_MERGED_SUCCESSFULLY;
            }

            ProcessCallStateChange(imsCall, ImsPhoneCall.State.DISCONNECTED, cause);

            If (reasonInfo->GetCode() == ImsReasonInfo.CODE_USER_TERMINATED) {
                If ((oldState == ImsPhoneCall.State.DISCONNECTING)
                        && (mForegroundCall->GetState() == ImsPhoneCall.State.DISCONNECTED)
                        && (mBackgroundCall->GetState() == ImsPhoneCall.State.HOLDING)) {
                    SendEmptyMessage(EVENT_RESUME_BACKGROUND);
                }
            }
        }

        //@Override
        CARAPI OnCallHeld(ImsCall imsCall) {
            If (DBG) Log("onCallHeld");

            {    AutoLock syncLock(mSyncHold);
                ImsPhoneCall.State oldState = mBackgroundCall->GetState();
                ProcessCallStateChange(imsCall, ImsPhoneCall.State.HOLDING,
                        DisconnectCause.NOT_DISCONNECTED);
                If (oldState == ImsPhoneCall.State.ACTIVE) {
                    If ((mForegroundCall->GetState() == ImsPhoneCall.State.HOLDING)
                            || (mRingingCall->GetState() == ImsPhoneCall.State.WAITING)) {
                        Boolean isOwner = TRUE;
                        CallGroup callGroup =  imsCall->GetCallGroup();
                        If (callGroup != NULL) {
                            isOwner = callGroup->IsOwner(imsCall);
                        }
                        If (isOwner) {
                            SendEmptyMessage(EVENT_RESUME_BACKGROUND);
                        }
                    } else {
                        //when multiple connections belong to background call,
                        //only the first callback reaches here
                        //otherwise the oldState is already HOLDING
                        If (mPendingMO != NULL) {
                            SendEmptyMessage(EVENT_DIAL_PENDINGMO);
                        }
                    }
                }
            }
        }

        //@Override
        CARAPI OnCallHoldFailed(ImsCall imsCall, ImsReasonInfo reasonInfo) {
            If (DBG) Log("onCallHoldFailed reasonCode=" + reasonInfo->GetCode());

            {    AutoLock syncLock(mSyncHold);
                ImsPhoneCall.State bgState = mBackgroundCall->GetState();
                If (reasonInfo->GetCode() == ImsReasonInfo.CODE_LOCAL_CALL_TERMINATED) {
                    // disconnected while processing hold
                    If (mPendingMO != NULL) {
                        SendEmptyMessage(EVENT_DIAL_PENDINGMO);
                    }
                } else If (bgState == ImsPhoneCall.State.ACTIVE) {
                    mForegroundCall->SwitchWith(mBackgroundCall);

                    If (mPendingMO != NULL) {
                        mPendingMO->SetDisconnectCause(DisconnectCause.ERROR_UNSPECIFIED);
                        SendEmptyMessageDelayed(EVENT_HANGUP_PENDINGMO, TIMEOUT_HANGUP_PENDINGMO);
                    }
                }
            }
        }

        //@Override
        CARAPI OnCallResumed(ImsCall imsCall) {
            If (DBG) Log("onCallResumed");

            ProcessCallStateChange(imsCall, ImsPhoneCall.State.ACTIVE,
                    DisconnectCause.NOT_DISCONNECTED);
        }

        //@Override
        CARAPI OnCallResumeFailed(ImsCall imsCall, ImsReasonInfo reasonInfo) {
            // TODO : What should be done?
        }

        //@Override
        CARAPI OnCallResumeReceived(ImsCall imsCall) {
            If (DBG) Log("onCallResumeReceived");

            If (mOnHoldToneStarted) {
                mPhone->StopOnHoldTone();
                mOnHoldToneStarted = FALSE;
            }

            SuppServiceNotification supp = new SuppServiceNotification();
            // Type of notification: 0 = MO; 1 = MT
            // Refer SuppServiceNotification class documentation.
            supp.notificationType = 1;
            supp.code = SuppServiceNotification.MT_CODE_CALL_RETRIEVED;
            mPhone->NotifySuppSvcNotification(supp);
        }

        //@Override
        CARAPI OnCallHoldReceived(ImsCall imsCall) {
            If (DBG) Log("onCallHoldReceived");

            ImsPhoneConnection conn = FindConnection(imsCall);
            If (conn != NULL && conn->GetState() == ImsPhoneCall.State.ACTIVE) {
                If (!mOnHoldToneStarted && ImsPhoneCall->IsLocalTone(imsCall)) {
                    mPhone->StartOnHoldTone();
                    mOnHoldToneStarted = TRUE;
                }
            }

            SuppServiceNotification supp = new SuppServiceNotification();
            // Type of notification: 0 = MO; 1 = MT
            // Refer SuppServiceNotification class documentation.
            supp.notificationType = 1;
            supp.code = SuppServiceNotification.MT_CODE_CALL_ON_HOLD;
            mPhone->NotifySuppSvcNotification(supp);
        }

        //@Override
        CARAPI OnCallMerged(ImsCall call) {
            If (DBG) Log("onCallMerged");

            mForegroundCall->Merge(mBackgroundCall, mForegroundCall->GetState());
            UpdatePhoneState();
            mPhone->NotifyPreciseCallStateChanged();
        }

        //@Override
        CARAPI OnCallMergeFailed(ImsCall call, ImsReasonInfo reasonInfo) {
            If (DBG) Log("onCallMergeFailed reasonCode=" + reasonInfo->GetCode());
            mPhone->NotifySuppServiceFailed(Phone.SuppService.CONFERENCE);
        }

        /**
         * Called when the state of IMS conference Participant(s) has changed.
         *
         * @param call the call object that carries out the IMS call.
         * @param participants the Participant(s) and their new state information.
         */
        //@Override
        CARAPI OnConferenceParticipantsStateChanged(ImsCall call,
                List<ConferenceParticipant> participants) {
            If (DBG) Log("onConferenceParticipantsStateChanged");

            ImsPhoneConnection conn = FindConnection(call);
            If (conn != NULL) {
                conn->UpdateConferenceParticipants(participants);
            }
        }
    };

    /**
     * Listen to the IMS call state change
     */
    private ImsCall.Listener mImsUssdListener = new ImsCall->Listener() {
        //@Override
        CARAPI OnCallStarted(ImsCall imsCall) {
            If (DBG) Log("mImsUssdListener onCallStarted");

            If (imsCall == mUssdSession) {
                If (mPendingUssd != NULL) {
                    AsyncResult->ForMessage(mPendingUssd);
                    mPendingUssd->SendToTarget();
                    mPendingUssd = NULL;
                }
            }
        }

        //@Override
        CARAPI OnCallStartFailed(ImsCall imsCall, ImsReasonInfo reasonInfo) {
            If (DBG) Log("mImsUssdListener onCallStartFailed reasonCode=" + reasonInfo->GetCode());

            OnCallTerminated(imsCall, reasonInfo);
        }

        //@Override
        CARAPI OnCallTerminated(ImsCall imsCall, ImsReasonInfo reasonInfo) {
            If (DBG) Log("mImsUssdListener onCallTerminated reasonCode=" + reasonInfo->GetCode());

            If (imsCall == mUssdSession) {
                mUssdSession = NULL;
                If (mPendingUssd != NULL) {
                    CommandException ex =
                            new CommandException(CommandException.Error.GENERIC_FAILURE);
                    AsyncResult->ForMessage(mPendingUssd, NULL, ex);
                    mPendingUssd->SendToTarget();
                    mPendingUssd = NULL;
                }
            }
            imsCall->Close();
        }

        //@Override
        CARAPI OnCallUssdMessageReceived(ImsCall call,
                Int32 mode, String ussdMessage) {
            If (DBG) Log("mImsUssdListener onCallUssdMessageReceived mode=" + mode);

            Int32 ussdMode = -1;

            Switch(mode) {
                case ImsCall.USSD_MODE_REQUEST:
                    ussdMode = CommandsInterface.USSD_MODE_REQUEST;
                    break;

                case ImsCall.USSD_MODE_NOTIFY:
                    ussdMode = CommandsInterface.USSD_MODE_NOTIFY;
                    break;
            }

            mPhone->OnIncomingUSSD(ussdMode, ussdMessage);
        }
    };

    /**
     * Listen to the IMS service state change
     *
     */
    private ImsConnectionStateListener mImsConnectionStateListener =
        new ImsConnectionStateListener() {
        //@Override
        CARAPI OnImsConnected() {
            If (DBG) Log("onImsConnected");
            mPhone->SetServiceState(ServiceState.STATE_IN_SERVICE);
        }

        //@Override
        CARAPI OnImsDisconnected() {
            If (DBG) Log("onImsDisconnected");
            mPhone->SetServiceState(ServiceState.STATE_OUT_OF_SERVICE);
        }

        //@Override
        CARAPI OnImsResumed() {
            If (DBG) Log("onImsResumed");
            mPhone->SetServiceState(ServiceState.STATE_IN_SERVICE);
        }

        //@Override
        CARAPI OnImsSuspended() {
            If (DBG) Log("onImsSuspended");
            mPhone->SetServiceState(ServiceState.STATE_OUT_OF_SERVICE);
        }

        //@Override
        CARAPI OnFeatureCapabilityChanged(Int32 serviceClass,
                Int32[] enabledFeatures, Int32[] disabledFeatures) {
            If (serviceClass == ImsServiceClass.MMTEL) {
                Boolean tmpIsVtEnabled = mIsVtEnabled;

                If (disabledFeatures[ImsConfig.FeatureConstants.FEATURE_TYPE_VOICE_OVER_LTE] ==
                        ImsConfig.FeatureConstants.FEATURE_TYPE_VOICE_OVER_LTE ||
                        disabledFeatures[ImsConfig.FeatureConstants.FEATURE_TYPE_VOICE_OVER_WIFI] ==
                        ImsConfig.FeatureConstants.FEATURE_TYPE_VOICE_OVER_WIFI) {
                    mIsVolteEnabled = FALSE;
                }
                If (disabledFeatures[ImsConfig.FeatureConstants.FEATURE_TYPE_VIDEO_OVER_LTE] ==
                        ImsConfig.FeatureConstants.FEATURE_TYPE_VIDEO_OVER_LTE ||
                        disabledFeatures[ImsConfig.FeatureConstants.FEATURE_TYPE_VIDEO_OVER_WIFI] ==
                        ImsConfig.FeatureConstants.FEATURE_TYPE_VIDEO_OVER_WIFI) {
                    mIsVtEnabled = FALSE;
                }
                If (enabledFeatures[ImsConfig.FeatureConstants.FEATURE_TYPE_VOICE_OVER_LTE] ==
                        ImsConfig.FeatureConstants.FEATURE_TYPE_VOICE_OVER_LTE ||
                        enabledFeatures[ImsConfig.FeatureConstants.FEATURE_TYPE_VOICE_OVER_WIFI] ==
                        ImsConfig.FeatureConstants.FEATURE_TYPE_VOICE_OVER_WIFI) {
                    mIsVolteEnabled = TRUE;
                }
                If (enabledFeatures[ImsConfig.FeatureConstants.FEATURE_TYPE_VIDEO_OVER_LTE] ==
                        ImsConfig.FeatureConstants.FEATURE_TYPE_VIDEO_OVER_LTE ||
                        enabledFeatures[ImsConfig.FeatureConstants.FEATURE_TYPE_VIDEO_OVER_WIFI] ==
                        ImsConfig.FeatureConstants.FEATURE_TYPE_VIDEO_OVER_WIFI) {
                    mIsVtEnabled = TRUE;
                }

                If (tmpIsVtEnabled != mIsVtEnabled) {
                    mPhone->NotifyForVideoCapabilityChanged(mIsVtEnabled);
                }
            }

            If (DBG) Log("onFeatureCapabilityChanged, mIsVolteEnabled = " +  mIsVolteEnabled
                    + " mIsVtEnabled = " + mIsVtEnabled);
        }

        //@Override
        CARAPI OnVoiceMessageCountChanged(Int32 count) {
            If (DBG) Log("onVoiceMessageCountChanged :: count=" + count);
            mPhone.mDefaultPhone->SetVoiceMessageCount(count);
        }
    };

    /* package */
    ImsUtInterface GetUtInterface() throws ImsException {
        If (mImsManager == NULL) {
            throw new ImsException("no ims manager", ImsReasonInfo.CODE_UNSPECIFIED);
        }

        ImsUtInterface ut = mImsManager->GetSupplementaryServiceConfiguration(mServiceId);
        return ut;
    }

    private void TransferHandoverConnections(ImsPhoneCall call) {
        If (call.mConnections != NULL) {
            For (Connection c : call.mConnections) {
                c.mPreHandoverState = call.mState;
                Log ("Connection state before handover is " + c->GetStateBeforeHandover());
            }
        }
        If (mHandoverCall.mConnections == NULL ) {
            mHandoverCall.mConnections = call.mConnections;
        } else { // Multi-call SRVCC
            mHandoverCall.mConnections->AddAll(call.mConnections);
        }
        If (mHandoverCall.mConnections != NULL) {
            If (call->GetImsCall() != NULL) {
                call->GetImsCall()->Close();
            }
            For (Connection c : mHandoverCall.mConnections) {
                ((ImsPhoneConnection)c).ChangeParent(mHandoverCall);
            }
        }
        If (call->GetState()->IsAlive()) {
            Log ("Call is alive and state is " + call.mState);
            mHandoverCall.mState = call.mState;
        }
        call.mConnections->Clear();
        call.mState = ImsPhoneCall.State.IDLE;
    }

    /* package */
    void NotifySrvccState(Call.SrvccState state) {
        If (DBG) Log("notifySrvccState state=" + state);

        mSrvccState = state;

        If (mSrvccState == Call.SrvccState.COMPLETED) {
            TransferHandoverConnections(mForegroundCall);
            TransferHandoverConnections(mBackgroundCall);
            TransferHandoverConnections(mRingingCall);
            // release wake lock hold
            ImsPhoneConnection con = mHandoverCall->GetHandoverConnection();
            If (con != NULL) {
                con->ReleaseWakeLock();
            }
        }
    }

    //****** Overridden from Handler

    //@Override
    CARAPI
    HandleMessage (Message msg) {
        AsyncResult ar;
        If (DBG) Log("handleMessage what=" + msg.what);

        Switch (msg.what) {
            case EVENT_HANGUP_PENDINGMO:
                If (mPendingMO != NULL) {
                    mPendingMO->OnDisconnect();
                    RemoveConnection(mPendingMO);
                    mPendingMO = NULL;
                }

                UpdatePhoneState();
                mPhone->NotifyPreciseCallStateChanged();
                break;
            case EVENT_RESUME_BACKGROUND:
                try {
                    ResumeWaitingOrHolding();
                } Catch (CallStateException e) {
                    If (Phone.DEBUG_PHONE) {
                        Loge("handleMessage EVENT_RESUME_BACKGROUND exception=" + e);
                    }
                }
                break;
            case EVENT_DIAL_PENDINGMO:
                DialInternal(mPendingMO, mClirMode, mPendingCallVideoState,
                    mPendingMO->GetCallExtras());
                break;

            case EVENT_EXIT_ECM_RESPONSE_CDMA:
                // no matter the result, we still do the same here
                If (pendingCallInEcm) {
                    DialInternal(mPendingMO, pendingCallClirMode,
                            mPendingCallVideoState, mPendingMO->GetCallExtras());
                    pendingCallInEcm = FALSE;
                }
                mPhone->UnsetOnEcbModeExitResponse(this);
                break;
        }
    }

    //@Override
    protected void Log(String msg) {
        Rlog->D(LOG_TAG, "[ImsPhoneCallTracker] " + msg);
    }

    protected void Loge(String msg) {
        Rlog->E(LOG_TAG, "[ImsPhoneCallTracker] " + msg);
    }

    //@Override
    CARAPI Dump(FileDescriptor fd, PrintWriter pw, String[] args) {
        pw->Println("ImsPhoneCallTracker extends:");
        super->Dump(fd, pw, args);
        pw->Println(" mVoiceCallEndedRegistrants=" + mVoiceCallEndedRegistrants);
        pw->Println(" mVoiceCallStartedRegistrants=" + mVoiceCallStartedRegistrants);
        pw->Println(" mRingingCall=" + mRingingCall);
        pw->Println(" mForegroundCall=" + mForegroundCall);
        pw->Println(" mBackgroundCall=" + mBackgroundCall);
        pw->Println(" mHandoverCall=" + mHandoverCall);
        pw->Println(" mPendingMO=" + mPendingMO);
        //pw->Println(" mHangupPendingMO=" + mHangupPendingMO);
        pw->Println(" mPhone=" + mPhone);
        pw->Println(" mDesiredMute=" + mDesiredMute);
        pw->Println(" mState=" + mState);
    }

    //@Override
    protected void HandlePollCalls(AsyncResult ar) {
    }

    /* package */
    ImsEcbm GetEcbmInterface() throws ImsException {
        If (mImsManager == NULL) {
            throw new ImsException("no ims manager", ImsReasonInfo.CODE_UNSPECIFIED);
        }

        ImsEcbm ecbm = mImsManager->GetEcbmInterface(mServiceId);
        return ecbm;
    }

    public Boolean IsInEmergencyCall() {
        return mIsInEmergencyCall;
    }

    public Boolean IsVolteEnabled() {
        return mIsVolteEnabled;
    }

    public Boolean IsVtEnabled() {
        return mIsVtEnabled;
    }
}
