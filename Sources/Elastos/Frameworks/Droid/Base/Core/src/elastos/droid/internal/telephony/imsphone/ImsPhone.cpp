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

using Elastos::Droid::App::IActivityManagerNative;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::IRegistrant;
using Elastos::Droid::Os::IRegistrantList;
using Elastos::Droid::Os::PowerManager::IWakeLock;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Telephony::IPhoneNumberUtils;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Droid::Telephony::IRlog;
using Elastos::Droid::Telephony::ISubscriptionManager;
using Elastos::Droid::Text::ITextUtils;

using Elastos::Droid::Ims::IImsCallForwardInfo;
using Elastos::Droid::Ims::IImsCallProfile;
using Elastos::Droid::Ims::IImsEcbm;
using Elastos::Droid::Ims::IImsEcbmStateListener;
using Elastos::Droid::Ims::IImsReasonInfo;
using Elastos::Droid::Ims::IImsSsInfo;
using Elastos::Droid::Ims::IImsUtInterface;

using static com::Android::Internal::Telephony::CommandsInterface::ICB_FACILITY_BAOC;
using static com::Android::Internal::Telephony::CommandsInterface::ICB_FACILITY_BAOIC;
using static com::Android::Internal::Telephony::CommandsInterface::ICB_FACILITY_BAOICxH;
using static com::Android::Internal::Telephony::CommandsInterface::ICB_FACILITY_BAIC;
using static com::Android::Internal::Telephony::CommandsInterface::ICB_FACILITY_BAICr;
using static com::Android::Internal::Telephony::CommandsInterface::ICB_FACILITY_BA_ALL;
using static com::Android::Internal::Telephony::CommandsInterface::ICB_FACILITY_BA_MO;
using static com::Android::Internal::Telephony::CommandsInterface::ICB_FACILITY_BA_MT;
using static com::Android::Internal::Telephony::CommandsInterface::ICF_ACTION_DISABLE;
using static com::Android::Internal::Telephony::CommandsInterface::ICF_ACTION_ENABLE;
using static com::Android::Internal::Telephony::CommandsInterface::ICF_ACTION_ERASURE;
using static com::Android::Internal::Telephony::CommandsInterface::ICF_ACTION_REGISTRATION;
using static com::Android::Internal::Telephony::CommandsInterface::ICF_REASON_ALL;
using static com::Android::Internal::Telephony::CommandsInterface::ICF_REASON_ALL_CONDITIONAL;
using static com::Android::Internal::Telephony::CommandsInterface::ICF_REASON_NO_REPLY;
using static com::Android::Internal::Telephony::CommandsInterface::ICF_REASON_NOT_REACHABLE;
using static com::Android::Internal::Telephony::CommandsInterface::ICF_REASON_BUSY;
using static com::Android::Internal::Telephony::CommandsInterface::ICF_REASON_UNCONDITIONAL;
using static com::Android::Internal::Telephony::CommandsInterface::ISERVICE_CLASS_VOICE;
using static com::Android::Internal::Telephony::CommandsInterface::ISERVICE_CLASS_NONE;

using Elastos::Droid::Internal::Telephony::ICall;
using Elastos::Droid::Internal::Telephony::ICallForwardInfo;
using Elastos::Droid::Internal::Telephony::ICallTracker;
using Elastos::Droid::Internal::Telephony::ICommandsInterface;
using Elastos::Droid::Internal::Telephony::IConnection;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Internal::Telephony::IPhoneBase;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Internal::Telephony::IPhoneNotifier;
using Elastos::Droid::Internal::Telephony::ISubscription;
using Elastos::Droid::Internal::Telephony::ITelephonyIntents;
using Elastos::Droid::Internal::Telephony::ITelephonyProperties;
using Elastos::Droid::Internal::Telephony::Cdma::ICDMAPhone;
using Elastos::Droid::Internal::Telephony::Gsm::IGSMPhone;
using Elastos::Droid::Internal::Telephony::Gsm::ISuppServiceNotification;
using Elastos::Droid::Internal::Telephony::Uicc::IIccRecords;

using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;

/**
 * {@hide}
 */
public class ImsPhone extends ImsPhoneBase {
    private static const String LOG_TAG = "ImsPhone";
    private static const Boolean DBG = TRUE;
    private static const Boolean VDBG = FALSE; // STOPSHIP if TRUE

    protected static const Int32 EVENT_SET_CALL_BARRING_DONE          = EVENT_LAST + 1;
    protected static const Int32 EVENT_GET_CALL_BARRING_DONE          = EVENT_LAST + 2;
    protected static const Int32 EVENT_SET_CALL_WAITING_DONE          = EVENT_LAST + 3;
    protected static const Int32 EVENT_GET_CALL_WAITING_DONE          = EVENT_LAST + 4;

    public static const String CS_FALLBACK = "cs_fallback";

    static const Int32 RESTART_ECM_TIMER = 0; // restart Ecm timer
    static const Int32 CANCEL_ECM_TIMER = 1; // cancel Ecm timer

    // Default Emergency Callback Mode exit timer
    private static const Int32 DEFAULT_ECM_EXIT_TIMER_VALUE = 300000;

    // Instance Variables
    PhoneBase mDefaultPhone;
    ImsPhoneCallTracker mCT;
    ArrayList <ImsPhoneMmiCode> mPendingMMIs = new ArrayList<ImsPhoneMmiCode>();

    Registrant mPostDialHandler;
    ServiceState mSS = new ServiceState();

    // To redial silently through GSM or CDMA when dialing through IMS fails
    private String mLastDialString;

    WakeLock mWakeLock;
    protected Boolean mIsPhoneInEcmState;

    // mEcmExitRespRegistrant is informed after the phone has been exited the emergency
    // callback mode keep track of if phone is in emergency callback mode
    private Registrant mEcmExitRespRegistrant;

    private final RegistrantList mSilentRedialRegistrants = new RegistrantList();

    // List of Registrants to send supplementary service notifications to.
    RegistrantList mSsnRegistrants = new RegistrantList();

    // Variable to cache the video capabilitity. In cases where we delete/re-create the phone
    // this information is getting lost.
    private Boolean mIsVideoCapable = FALSE;

    // A runnable which is used to automatically exit from Ecm after a period of time.
    private Runnable mExitEcmRunnable = new Runnable() {
        //@Override
        CARAPI Run() {
            ExitEmergencyCallbackMode();
        }
    };

    // Create Cf (Call forward) so that dialling number &
    // MIsCfu (TRUE if reason is call forward unconditional)
    // MOnComplete (Message object passed by client) can be packed &
    // given as a single Cf object as user data to UtInterface.
    private static class Cf {
        final String mSetCfNumber;
        final Message mOnComplete;
        final Boolean mIsCfu;

        Cf(String cfNumber, Boolean isCfu, Message onComplete) {
            mSetCfNumber = cfNumber;
            mIsCfu = isCfu;
            mOnComplete = onComplete;
        }
    }

    // Constructors

    ImsPhone(Context context, PhoneNotifier notifier, Phone defaultPhone) {
        Super("ImsPhone", context, notifier);

        mDefaultPhone = (PhoneBase) defaultPhone;
        mCT = new ImsPhoneCallTracker(this);
        mSS->SetStateOff();

        mPhoneId = mDefaultPhone->GetPhoneId();

        // This is needed to handle phone process crashes
        // Same property is used for both CDMA & IMS phone.
        mIsPhoneInEcmState = SystemProperties->GetBoolean(
                TelephonyProperties.PROPERTY_INECM_MODE, FALSE);

        PowerManager pm = (PowerManager) context->GetSystemService(Context.POWER_SERVICE);
        mWakeLock = pm->NewWakeLock(PowerManager.PARTIAL_WAKE_LOCK, LOG_TAG);
        mWakeLock->SetReferenceCounted(FALSE);
    }

    CARAPI UpdateParentPhone(PhoneBase parentPhone) {
        // synchronization is managed at the PhoneBase Scope (which calls this function)
        mDefaultPhone = parentPhone;
        mPhoneId = mDefaultPhone->GetPhoneId();

        // When the parent phone is updated, we need to notify listeners of the cached video
        // capability.
        Rlog->D(LOG_TAG, "updateParentPhone - Notify video capability changed " + mIsVideoCapable);
        NotifyForVideoCapabilityChanged(mIsVideoCapable);
    }

    //@Override
    CARAPI Dispose() {
        Rlog->D(LOG_TAG, "dispose");
        // Nothing to dispose in PhoneBase
        //super->Dispose();
        mPendingMMIs->Clear();
        mCT->Dispose();

        //Force all referenced classes to unregister their former registered events
    }

    //@Override
    CARAPI RemoveReferences() {
        Rlog->D(LOG_TAG, "removeReferences");
        super->RemoveReferences();

        mCT = NULL;
        mSS = NULL;
    }

    //@Override
    public ServiceState
    GetServiceState() {
        return mSS;
    }

    /* package */ void SetServiceState(Int32 state) {
        mSS->SetState(state);
    }

    //@Override
    public CallTracker GetCallTracker() {
        return mCT;
    }

    //@Override
    public List<? extends ImsPhoneMmiCode>
    GetPendingMmiCodes() {
        return mPendingMMIs;
    }


    //@Override
    CARAPI
    AcceptCall(Int32 videoState) throws CallStateException {
        mCT->AcceptCall(videoState);
    }

    //@Override
    CARAPI
    DeflectCall(String number) throws CallStateException {
        mCT->DeflectCall(number);
    }

    //@Override
    CARAPI
    RejectCall() throws CallStateException {
        mCT->RejectCall();
    }

    //@Override
    CARAPI
    SwitchHoldingAndActive() throws CallStateException {
        mCT->SwitchWaitingOrHoldingAndActive();
    }

    //@Override
    public Boolean CanConference() {
        return mCT->CanConference();
    }

    public Boolean CanDial() {
        return mCT->CanDial();
    }

    //@Override
    CARAPI Conference() {
        mCT->Conference();
    }

    //@Override
    CARAPI ClearDisconnected() {
        mCT->ClearDisconnected();
    }

    //@Override
    public Boolean CanTransfer() {
        return mCT->CanTransfer();
    }

    //@Override
    CARAPI ExplicitCallTransfer() {
        mCT->ExplicitCallTransfer();
    }

    //@Override
    public ImsPhoneCall
    GetForegroundCall() {
        return mCT.mForegroundCall;
    }

    //@Override
    public ImsPhoneCall
    GetBackgroundCall() {
        return mCT.mBackgroundCall;
    }

    //@Override
    public ImsPhoneCall
    GetRingingCall() {
        return mCT.mRingingCall;
    }

    private Boolean HandleCallDeflectionIncallSupplementaryService(
            String dialString) {
        If (dialString->Length() > 1) {
            return FALSE;
        }

        If (GetRingingCall()->GetState() != ImsPhoneCall.State.IDLE) {
            If (DBG) Rlog->D(LOG_TAG, "MmiCode 0: rejectCall");
            try {
                mCT->RejectCall();
            } Catch (CallStateException e) {
                If (DBG) Rlog->D(LOG_TAG, "reject failed", e);
                NotifySuppServiceFailed(Phone.SuppService.REJECT);
            }
        } else If (GetBackgroundCall()->GetState() != ImsPhoneCall.State.IDLE) {
            If (DBG) Rlog->D(LOG_TAG, "MmiCode 0: hangupWaitingOrBackground");
            try {
                mCT->Hangup(GetBackgroundCall());
            } Catch (CallStateException e) {
                If (DBG) Rlog->D(LOG_TAG, "hangup failed", e);
            }
        }

        return TRUE;
    }


    private Boolean HandleCallWaitingIncallSupplementaryService(
            String dialString) {
        Int32 len = dialString->Length();

        If (len > 2) {
            return FALSE;
        }

        ImsPhoneCall call = GetForegroundCall();

        try {
            If (len > 1) {
                If (DBG) Rlog->D(LOG_TAG, "not support 1X SEND");
                NotifySuppServiceFailed(Phone.SuppService.HANGUP);
            } else {
                If (call->GetState() != ImsPhoneCall.State.IDLE) {
                    If (DBG) Rlog->D(LOG_TAG, "MmiCode 1: hangup foreground");
                    mCT->Hangup(call);
                } else {
                    If (DBG) Rlog->D(LOG_TAG, "MmiCode 1: switchWaitingOrHoldingAndActive");
                    mCT->SwitchWaitingOrHoldingAndActive();
                }
            }
        } Catch (CallStateException e) {
            If (DBG) Rlog->D(LOG_TAG, "hangup failed", e);
            NotifySuppServiceFailed(Phone.SuppService.HANGUP);
        }

        return TRUE;
    }

    private Boolean HandleCallHoldIncallSupplementaryService(String dialString) {
        Int32 len = dialString->Length();

        If (len > 2) {
            return FALSE;
        }

        ImsPhoneCall call = GetForegroundCall();

        If (len > 1) {
            If (DBG) Rlog->D(LOG_TAG, "separate not supported");
            NotifySuppServiceFailed(Phone.SuppService.SEPARATE);
        } else {
            try {
                If (GetRingingCall()->GetState() != ImsPhoneCall.State.IDLE) {
                    If (DBG) Rlog->D(LOG_TAG, "MmiCode 2: accept ringing call");
                    mCT->AcceptCall(ImsCallProfile.CALL_TYPE_VOICE);
                } else {
                    If (DBG) Rlog->D(LOG_TAG, "MmiCode 2: switchWaitingOrHoldingAndActive");
                    mCT->SwitchWaitingOrHoldingAndActive();
                }
            } Catch (CallStateException e) {
                If (DBG) Rlog->D(LOG_TAG, "switch failed", e);
                NotifySuppServiceFailed(Phone.SuppService.SWITCH);
            }
        }

        return TRUE;
    }

    private Boolean HandleMultipartyIncallSupplementaryService(
            String dialString) {
        If (dialString->Length() > 1) {
            return FALSE;
        }

        If (DBG) Rlog->D(LOG_TAG, "MmiCode 3: merge calls");
        Conference();
        return TRUE;
    }

    private Boolean HandleEctIncallSupplementaryService(String dialString) {

        Int32 len = dialString->Length();

        If (len != 1) {
            return FALSE;
        }

        If (DBG) Rlog->D(LOG_TAG, "MmiCode 4: not support explicit call transfer");
        NotifySuppServiceFailed(Phone.SuppService.TRANSFER);
        return TRUE;
    }

    private Boolean HandleCcbsIncallSupplementaryService(String dialString) {
        If (dialString->Length() > 1) {
            return FALSE;
        }

        Rlog->I(LOG_TAG, "MmiCode 5: CCBS not supported!");
        // Treat it as an "unknown" service.
        NotifySuppServiceFailed(Phone.SuppService.UNKNOWN);
        return TRUE;
    }

    CARAPI NotifySuppSvcNotification(SuppServiceNotification suppSvc) {
        Rlog->D(LOG_TAG, "notifySuppSvcNotification: suppSvc = " + suppSvc);

        AsyncResult ar = new AsyncResult(NULL, suppSvc, NULL);
        mSsnRegistrants->NotifyRegistrants(ar);
    }

    //@Override
    public Boolean HandleInCallMmiCommands(String dialString) {
        If (!IsInCall()) {
            return FALSE;
        }

        If (TextUtils->IsEmpty(dialString)) {
            return FALSE;
        }

        Boolean result = FALSE;
        Char32 ch = dialString->CharAt(0);
        Switch (ch) {
            case '0':
                result = HandleCallDeflectionIncallSupplementaryService(
                        dialString);
                break;
            case '1':
                result = HandleCallWaitingIncallSupplementaryService(
                        dialString);
                break;
            case '2':
                result = HandleCallHoldIncallSupplementaryService(dialString);
                break;
            case '3':
                result = HandleMultipartyIncallSupplementaryService(dialString);
                break;
            case '4':
                result = HandleEctIncallSupplementaryService(dialString);
                break;
            case '5':
                result = HandleCcbsIncallSupplementaryService(dialString);
                break;
            default:
                break;
        }

        return result;
    }

    Boolean IsInCall() {
        ImsPhoneCall.State foregroundCallState = GetForegroundCall()->GetState();
        ImsPhoneCall.State backgroundCallState = GetBackgroundCall()->GetState();
        ImsPhoneCall.State ringingCallState = GetRingingCall()->GetState();

       Return (foregroundCallState->IsAlive() ||
               backgroundCallState->IsAlive() ||
               ringingCallState->IsAlive());
    }

    void NotifyNewRingingConnection(Connection c) {
        mDefaultPhone->NotifyNewRingingConnectionP(c);
    }

    CARAPI NotifyForVideoCapabilityChanged(Boolean isVideoCapable) {
        mIsVideoCapable = isVideoCapable;
        mDefaultPhone->NotifyForVideoCapabilityChanged(isVideoCapable);
    }

    //@Override
    public Connection
    Dial(String dialString, Int32 videoState, Bundle extras) throws CallStateException {
        return DialInternal(dialString, videoState, extras);
    }

    //@Override
    public Connection
    Dial(String dialString, Int32 videoState) throws CallStateException {
        return DialInternal(dialString, videoState, NULL);
    }

    protected Connection DialInternal(String dialString, Int32 videoState, Bundle extras)
            throws CallStateException {
        Boolean isConferenceUri = FALSE;
        Boolean isSkipSchemaParsing = FALSE;
        If (extras != NULL) {
            isConferenceUri = extras->GetBoolean(TelephonyProperties.EXTRA_DIAL_CONFERENCE_URI,
                    FALSE);
            isSkipSchemaParsing = extras->GetBoolean(TelephonyProperties.EXTRA_SKIP_SCHEMA_PARSING,
                    FALSE);
        }
        String newDialString = dialString;
        // Need to make sure dialString gets parsed properly
        If (!isConferenceUri && !isSkipSchemaParsing) {
            newDialString = PhoneNumberUtils->StripSeparators(dialString);
        }

        // handle in-call MMI first if applicable
        If (HandleInCallMmiCommands(newDialString)) {
            return NULL;
        }

        String networkPortion = newDialString;
        If (!ImsPhoneMmiCode->IsScMatchesSuppServType(newDialString)) {
            // Only look at the Network portion for mmi
            networkPortion = PhoneNumberUtils->ExtractNetworkPortionAlt(newDialString);
        }

        ImsPhoneMmiCode mmi =
                ImsPhoneMmiCode->NewFromDialString(networkPortion, this);
        If (DBG) Rlog->D(LOG_TAG,
                "dialing w/ mmi '" + mmi + "'...");

        If (mmi == NULL) {
            return mCT->Dial(dialString, videoState, extras);
        } else If (mmi->IsTemporaryModeCLIR()) {
            return mCT->Dial(mmi->GetDialingNumber(), mmi->GetCLIRMode(), videoState, extras);
        } else If (!mmi->IsSupportedOverImsPhone()) {
            // If the mmi is not supported by IMS service,
            // try to initiate dialing with default phone
            throw new CallStateException(CS_FALLBACK);
        } else {
            mPendingMMIs->Add(mmi);
            mMmiRegistrants->NotifyRegistrants(new AsyncResult(NULL, mmi, NULL));
            mmi->ProcessCode();

            return NULL;
        }
    }

    //@Override
    CARAPI AddParticipant(String dialString) throws CallStateException {
        mCT->AddParticipant(dialString);
    }

    //@Override
    CARAPI
    SendDtmf(Char32 c) {
        If (!PhoneNumberUtils->Is12Key(c)) {
            Rlog->E(LOG_TAG,
                    "sendDtmf called with invalid character '" + c + "'");
        } else {
            If (mCT.mState ==  PhoneConstants.State.OFFHOOK) {
                mCT->SendDtmf(c);
            }
        }
    }

    //@Override
    CARAPI
    StartDtmf(Char32 c) {
        If (!(PhoneNumberUtils->Is12Key(c) || (c >= 'A' && c <= 'D'))) {
            Rlog->E(LOG_TAG,
                    "startDtmf called with invalid character '" + c + "'");
        } else {
            mCT->StartDtmf(c);
        }
    }

    //@Override
    CARAPI
    StopDtmf() {
        mCT->StopDtmf();
    }

    //@Override
    CARAPI SetOnPostDialCharacter(Handler h, Int32 what, Object obj) {
        mPostDialHandler = new Registrant(h, what, obj);
    }

    /*package*/ void NotifyIncomingRing() {
        If (DBG) Rlog->D(LOG_TAG, "notifyIncomingRing");
        AsyncResult ar = new AsyncResult(NULL, NULL, NULL);
        SendMessage(ObtainMessage(EVENT_CALL_RING, ar));
    }

    //@Override
    CARAPI SetMute(Boolean muted) {
        mCT->SetMute(muted);
    }

    //@Override
    CARAPI SetUiTTYMode(Int32 uiTtyMode, Message onComplete) {
        mCT->SetUiTTYMode(uiTtyMode, onComplete);
    }

    //@Override
    public Boolean GetMute() {
        return mCT->GetMute();
    }

    //@Override
    public PhoneConstants.State GetState() {
        return mCT.mState;
    }

    private Boolean IsValidCommandInterfaceCFReason (Int32 commandInterfaceCFReason) {
        Switch (commandInterfaceCFReason) {
        case CF_REASON_UNCONDITIONAL:
        case CF_REASON_BUSY:
        case CF_REASON_NO_REPLY:
        case CF_REASON_NOT_REACHABLE:
        case CF_REASON_ALL:
        case CF_REASON_ALL_CONDITIONAL:
            return TRUE;
        default:
            return FALSE;
        }
    }

    private Boolean IsValidCommandInterfaceCFAction (Int32 commandInterfaceCFAction) {
        Switch (commandInterfaceCFAction) {
        case CF_ACTION_DISABLE:
        case CF_ACTION_ENABLE:
        case CF_ACTION_REGISTRATION:
        case CF_ACTION_ERASURE:
            return TRUE;
        default:
            return FALSE;
        }
    }

    private  Boolean IsCfEnable(Int32 action) {
        Return (action == CF_ACTION_ENABLE) || (action == CF_ACTION_REGISTRATION);
    }

    private Int32 GetConditionFromCFReason(Int32 reason) {
        Switch(reason) {
            case CF_REASON_UNCONDITIONAL: return ImsUtInterface.CDIV_CF_UNCONDITIONAL;
            case CF_REASON_BUSY: return ImsUtInterface.CDIV_CF_BUSY;
            case CF_REASON_NO_REPLY: return ImsUtInterface.CDIV_CF_NO_REPLY;
            case CF_REASON_NOT_REACHABLE: return ImsUtInterface.CDIV_CF_NOT_REACHABLE;
            case CF_REASON_ALL: return ImsUtInterface.CDIV_CF_ALL;
            case CF_REASON_ALL_CONDITIONAL: return ImsUtInterface.CDIV_CF_ALL_CONDITIONAL;
            default:
                break;
        }

        return ImsUtInterface.INVALID;
    }

    private Int32 GetCFReasonFromCondition(Int32 condition) {
        Switch(condition) {
            case ImsUtInterface.CDIV_CF_UNCONDITIONAL: return CF_REASON_UNCONDITIONAL;
            case ImsUtInterface.CDIV_CF_BUSY: return CF_REASON_BUSY;
            case ImsUtInterface.CDIV_CF_NO_REPLY: return CF_REASON_NO_REPLY;
            case ImsUtInterface.CDIV_CF_NOT_REACHABLE: return CF_REASON_NOT_REACHABLE;
            case ImsUtInterface.CDIV_CF_ALL: return CF_REASON_ALL;
            case ImsUtInterface.CDIV_CF_ALL_CONDITIONAL: return CF_REASON_ALL_CONDITIONAL;
            default:
                break;
        }

        return CF_REASON_NOT_REACHABLE;
    }

    private Int32 GetActionFromCFAction(Int32 action) {
        Switch(action) {
            case CF_ACTION_DISABLE: return ImsUtInterface.ACTION_DEACTIVATION;
            case CF_ACTION_ENABLE: return ImsUtInterface.ACTION_ACTIVATION;
            case CF_ACTION_ERASURE: return ImsUtInterface.ACTION_ERASURE;
            case CF_ACTION_REGISTRATION: return ImsUtInterface.ACTION_REGISTRATION;
            default:
                break;
        }

        return ImsUtInterface.INVALID;
    }

    //@Override
    CARAPI GetCallForwardingOption(Int32 commandInterfaceCFReason,
            Message onComplete) {
        If (DBG) Rlog->D(LOG_TAG, "getCallForwardingOption reason=" + commandInterfaceCFReason);
        If (IsValidCommandInterfaceCFReason(commandInterfaceCFReason)) {
            If (DBG) Rlog->D(LOG_TAG, "requesting call forwarding query.");
            Message resp;
            resp = ObtainMessage(EVENT_GET_CALL_FORWARD_DONE, onComplete);

            try {
                ImsUtInterface ut = mCT->GetUtInterface();
                ut->QueryCallForward(GetConditionFromCFReason(commandInterfaceCFReason),NULL,resp);
            } Catch (ImsException e) {
                SendErrorResponse(onComplete, e);
            }
        } else If (onComplete != NULL) {
            SendErrorResponse(onComplete);
        }
    }

    //@Override
    CARAPI SetCallForwardingOption(Int32 commandInterfaceCFAction,
            Int32 commandInterfaceCFReason,
            String dialingNumber,
            Int32 timerSeconds,
            Message onComplete) {
        If (DBG) Rlog->D(LOG_TAG, "setCallForwardingOption action=" + commandInterfaceCFAction
                + ", reason=" + commandInterfaceCFReason);
        If ((IsValidCommandInterfaceCFAction(commandInterfaceCFAction)) &&
                (IsValidCommandInterfaceCFReason(commandInterfaceCFReason))) {
            Message resp;
            Cf cf = new Cf(dialingNumber,
                    (commandInterfaceCFReason == CF_REASON_UNCONDITIONAL ? TRUE : FALSE),
                    onComplete);
            resp = ObtainMessage(EVENT_SET_CALL_FORWARD_DONE,
                    IsCfEnable(commandInterfaceCFAction) ? 1 : 0, 0, cf);

            try {
                ImsUtInterface ut = mCT->GetUtInterface();
                ut->UpdateCallForward(GetActionFromCFAction(commandInterfaceCFAction),
                        GetConditionFromCFReason(commandInterfaceCFReason),
                        dialingNumber,
                        timerSeconds,
                        onComplete);
             } Catch (ImsException e) {
                SendErrorResponse(onComplete, e);
             }
        } else If (onComplete != NULL) {
            SendErrorResponse(onComplete);
        }
    }

    //@Override
    CARAPI SetCallForwardingUncondTimerOption(Int32 startHour, Int32 startMinute,
            Int32 endHour, Int32 endMinute, Int32 commandInterfaceCFAction,
            Int32 commandInterfaceCFReason, String dialingNumber, Message onComplete) {
        If (DBG) Rlog->D(LOG_TAG, "setCallForwardingUncondTimerOption action="
                + commandInterfaceCFAction + ", reason=" + commandInterfaceCFReason
                + ", startHour=" + startHour + ", startMinute=" + startMinute
                + ", endHour=" + endHour + ", endMinute=" + endMinute);
        If ((IsValidCommandInterfaceCFAction(commandInterfaceCFAction)) &&
                (IsValidCommandInterfaceCFReason(commandInterfaceCFReason))) {
            Message resp;
            Cf cf = new Cf(dialingNumber,
                    (commandInterfaceCFReason == CF_REASON_UNCONDITIONAL ? TRUE : FALSE),
                    onComplete);
            resp = ObtainMessage(EVENT_SET_CALL_FORWARD_TIMER_DONE,
                    IsCfEnable(commandInterfaceCFAction) ? 1 : 0, 0, cf);

            try {
                ImsUtInterface ut = mCT->GetUtInterface();
                ut->UpdateCallForwardUncondTimer(startHour, startMinute, endHour,
                        endMinute, GetActionFromCFAction(commandInterfaceCFAction),
                        GetConditionFromCFReason(commandInterfaceCFReason),
                        dialingNumber, resp);
             } Catch (ImsException e) {
                SendErrorResponse(onComplete, e);
             }
        } else If (onComplete != NULL) {
            SendErrorResponse(onComplete);
        }
    }


    //@Override
    CARAPI GetCallWaiting(Message onComplete) {
        If (DBG) Rlog->D(LOG_TAG, "getCallWaiting");
        Message resp;
        resp = ObtainMessage(EVENT_GET_CALL_WAITING_DONE, onComplete);

        try {
            ImsUtInterface ut = mCT->GetUtInterface();
            ut->QueryCallWaiting(resp);
        } Catch (ImsException e) {
            SendErrorResponse(onComplete, e);
        }
    }

    //@Override
    CARAPI SetCallWaiting(Boolean enable, Message onComplete) {
        If (DBG) Rlog->D(LOG_TAG, "setCallWaiting enable=" + enable);
        Message resp;
        resp = ObtainMessage(EVENT_SET_CALL_WAITING_DONE, onComplete);

        try {
            ImsUtInterface ut = mCT->GetUtInterface();
            ut->UpdateCallWaiting(enable, resp);
        } Catch (ImsException e) {
            SendErrorResponse(onComplete, e);
        }
    }

    private Int32 GetCBTypeFromFacility(String facility) {
        If (CB_FACILITY_BAOC->Equals(facility)) {
            return ImsUtInterface.CB_BAOC;
        } else If (CB_FACILITY_BAOIC->Equals(facility)) {
            return ImsUtInterface.CB_BOIC;
        } else If (CB_FACILITY_BAOICxH->Equals(facility)) {
            return ImsUtInterface.CB_BOIC_EXHC;
        } else If (CB_FACILITY_BAIC->Equals(facility)) {
            return ImsUtInterface.CB_BAIC;
        } else If (CB_FACILITY_BAICr->Equals(facility)) {
            return ImsUtInterface.CB_BIC_WR;
        } else If (CB_FACILITY_BA_ALL->Equals(facility)) {
            return ImsUtInterface.CB_BA_ALL;
        } else If (CB_FACILITY_BA_MO->Equals(facility)) {
            return ImsUtInterface.CB_BA_MO;
        } else If (CB_FACILITY_BA_MT->Equals(facility)) {
            return ImsUtInterface.CB_BA_MT;
        }

        return 0;
    }

    /* package */
    void GetCallBarring(String facility, Message onComplete) {
        If (DBG) Rlog->D(LOG_TAG, "getCallBarring facility=" + facility);
        Message resp;
        resp = ObtainMessage(EVENT_GET_CALL_BARRING_DONE, onComplete);

        try {
            ImsUtInterface ut = mCT->GetUtInterface();
            ut->QueryCallBarring(GetCBTypeFromFacility(facility), resp);
        } Catch (ImsException e) {
            SendErrorResponse(onComplete, e);
        }
    }

    /* package */
    void SetCallBarring(String facility, Boolean lockState, String password, Message onComplete) {
        If (DBG) Rlog->D(LOG_TAG, "setCallBarring facility=" + facility
                + ", lockState=" + lockState);
        Message resp;
        resp = ObtainMessage(EVENT_SET_CALL_BARRING_DONE, onComplete);

        Int32 action;
        If (lockState) {
            action = CommandsInterface.CF_ACTION_ENABLE;
        }
        else {
            action = CommandsInterface.CF_ACTION_DISABLE;
        }

        try {
            ImsUtInterface ut = mCT->GetUtInterface();
            // password is not required with Ut interface
            ut->UpdateCallBarring(GetCBTypeFromFacility(facility), action, resp, NULL);
        } Catch (ImsException e) {
            SendErrorResponse(onComplete, e);
        }
    }

    //@Override
    CARAPI SendUssdResponse(String ussdMessge) {
        Rlog->D(LOG_TAG, "sendUssdResponse");
        ImsPhoneMmiCode mmi = ImsPhoneMmiCode->NewFromUssdUserInput(ussdMessge, this);
        mPendingMMIs->Add(mmi);
        mMmiRegistrants->NotifyRegistrants(new AsyncResult(NULL, mmi, NULL));
        mmi->SendUssd(ussdMessge);
    }

    /* package */
    void SendUSSD (String ussdString, Message response) {
        mCT->SendUSSD(ussdString, response);
    }

    /* package */
    void CancelUSSD() {
        mCT->CancelUSSD();
    }

    /* package */
    void SendErrorResponse(Message onComplete) {
        Rlog->D(LOG_TAG, "sendErrorResponse");
        If (onComplete != NULL) {
            AsyncResult->ForMessage(onComplete, NULL,
                    new CommandException(CommandException.Error.GENERIC_FAILURE));
            onComplete->SendToTarget();
        }
    }

    /* package */
    void SendErrorResponse(Message onComplete, Throwable e) {
        Rlog->D(LOG_TAG, "sendErrorResponse");
        If (onComplete != NULL) {
            AsyncResult->ForMessage(onComplete, NULL, GetCommandException(e));
            onComplete->SendToTarget();
        }
    }

    /* package */
    void SendErrorResponse(Message onComplete, ImsReasonInfo reasonInfo) {
        Rlog->D(LOG_TAG, "sendErrorResponse reasonCode=" + reasonInfo->GetCode());
        If (onComplete != NULL) {
            AsyncResult->ForMessage(onComplete, NULL, GetCommandException(reasonInfo->GetCode()));
            onComplete->SendToTarget();
        }
    }

    /* package */
    CommandException GetCommandException(Int32 code) {
        Rlog->D(LOG_TAG, "getCommandException code=" + code);
        CommandException.Error error = CommandException.Error.GENERIC_FAILURE;

        Switch(code) {
            case ImsReasonInfo.CODE_UT_NOT_SUPPORTED:
                error = CommandException.Error.REQUEST_NOT_SUPPORTED;
                break;
            case ImsReasonInfo.CODE_UT_CB_PASSWORD_MISMATCH:
                error = CommandException.Error.PASSWORD_INCORRECT;
                break;
            default:
                break;
        }

        return new CommandException(error);
    }

    /* package */
    CommandException GetCommandException(Throwable e) {
        CommandException ex = NULL;

        If (e instanceof ImsException) {
            ex = GetCommandException(((ImsException)e).GetCode());
        } else {
            Rlog->D(LOG_TAG, "getCommandException generic failure");
            ex = new CommandException(CommandException.Error.GENERIC_FAILURE);
        }
        return ex;
    }

    private void
    OnNetworkInitiatedUssd(ImsPhoneMmiCode mmi) {
        Rlog->D(LOG_TAG, "onNetworkInitiatedUssd");
        mMmiCompleteRegistrants->NotifyRegistrants(
            new AsyncResult(NULL, mmi, NULL));
    }

    /* package */
    void OnIncomingUSSD (Int32 ussdMode, String ussdMessage) {
        If (DBG) Rlog->D(LOG_TAG, "onIncomingUSSD ussdMode=" + ussdMode);

        Boolean isUssdError;
        Boolean isUssdRequest;

        isUssdRequest
            = (ussdMode == CommandsInterface.USSD_MODE_REQUEST);

        isUssdError
            = (ussdMode != CommandsInterface.USSD_MODE_NOTIFY
                && ussdMode != CommandsInterface.USSD_MODE_REQUEST);

        ImsPhoneMmiCode found = NULL;
        For (Int32 i = 0, s = mPendingMMIs->Size() ; i < s; i++) {
            If(mPendingMMIs->Get(i).IsPendingUSSD()) {
                found = mPendingMMIs->Get(i);
                break;
            }
        }

        If (found != NULL) {
            // Complete pending USSD
            If (isUssdError) {
                found->OnUssdFinishedError();
            } else {
                found->OnUssdFinished(ussdMessage, isUssdRequest);
            }
        } else { // pending USSD not found
            // The network may initiate its own USSD request

            // ignore everything that isnt a Notify or a Request
            // also, discard if there is no message to present
            If (!isUssdError && ussdMessage != NULL) {
                ImsPhoneMmiCode mmi;
                mmi = ImsPhoneMmiCode->NewNetworkInitiatedUssd(ussdMessage,
                        isUssdRequest,
                        ImsPhone.this);
                OnNetworkInitiatedUssd(mmi);
            }
        }
    }

    /**
     * Removes the given MMI from the pending list and notifies
     * registrants that it is complete.
     * @param mmi MMI that is done
     */
    /*package*/ void
    OnMMIDone(ImsPhoneMmiCode mmi) {
        /* Only notify complete if it's on the pending list.
         * Otherwise, it's already been Handled (eg, previously canceled).
         * The exception is cancellation of an incoming USSD-REQUEST, which is
         * not on the list.
         */
        If (mPendingMMIs->Remove(mmi) || mmi->IsUssdRequest()) {
            mMmiCompleteRegistrants->NotifyRegistrants(
                    new AsyncResult(NULL, mmi, NULL));
        }
    }

    public ArrayList<Connection> GetHandoverConnection() {
        ArrayList<Connection> connList = new ArrayList<Connection>();
        // Add all foreground call connections
        connList->AddAll(GetForegroundCall().mConnections);
        // Add all background call connections
        connList->AddAll(GetBackgroundCall().mConnections);
        // Add all background call connections
        connList->AddAll(GetRingingCall().mConnections);
        If (connList->Size() > 0) {
            return connList;
        } else {
            return NULL;
        }
    }

    CARAPI NotifySrvccState(Call.SrvccState state) {
        mCT->NotifySrvccState(state);
    }

    /* package */ void
    InitiateSilentRedial() {
        String result = mLastDialString;
        AsyncResult ar = new AsyncResult(NULL, result, NULL);
        If (ar != NULL) {
            mSilentRedialRegistrants->NotifyRegistrants(ar);
        }
    }

    CARAPI RegisterForSilentRedial(Handler h, Int32 what, Object obj) {
        mSilentRedialRegistrants->AddUnique(h, what, obj);
    }

    CARAPI UnregisterForSilentRedial(Handler h) {
        mSilentRedialRegistrants->Remove(h);
    }

    //@Override
    CARAPI RegisterForSuppServiceNotification(
            Handler h, Int32 what, Object obj) {
        mSsnRegistrants->AddUnique(h, what, obj);
    }

    //@Override
    CARAPI UnregisterForSuppServiceNotification(Handler h) {
        mSsnRegistrants->Remove(h);
    }

    //@Override
    public Int64 GetSubId() {
        return mDefaultPhone->GetSubId();
    }

    //@Override
    public Int32 GetPhoneId() {
        return mDefaultPhone->GetPhoneId();
    }

    //@Override
    public Subscription GetSubscriptionInfo() {
        return mDefaultPhone->GetSubscriptionInfo();
    }

    public IccRecords GetIccRecords() {
        return mDefaultPhone.mIccRecords->Get();
    }

    private CallForwardInfo GetCallForwardInfo(ImsCallForwardInfo info) {
        CallForwardInfo cfInfo = new CallForwardInfo();
        cfInfo.status = info.mStatus;
        cfInfo.reason = GetCFReasonFromCondition(info.mCondition);
        cfInfo.serviceClass = SERVICE_CLASS_VOICE;
        cfInfo.toa = info.mToA;
        cfInfo.number = info.mNumber;
        cfInfo.timeSeconds = info.mTimeSeconds;
        cfInfo.startHour = info.mStartHour;
        cfInfo.startMinute = info.mStartMinute;
        cfInfo.endHour = info.mEndHour;
        cfInfo.endMinute = info.mEndMinute;
        return cfInfo;
    }

    private CallForwardInfo[] HandleCfQueryResult(ImsCallForwardInfo[] infos) {
        CallForwardInfo[] cfInfos = NULL;

        If (infos != NULL && infos.length != 0) {
            cfInfos = new CallForwardInfo[infos.length];
        }

        IccRecords r = GetIccRecords();
        If (infos == NULL || infos.length == 0) {
            If (r != NULL) {
                // Assume the default is not active
                // Set unconditional CFF in SIM to FALSE
                r->SetVoiceCallForwardingFlag(1, FALSE, NULL);
            }
        } else {
            For (Int32 i = 0, s = infos.length; i < s; i++) {
                If (infos[i].mCondition == ImsUtInterface.CDIV_CF_UNCONDITIONAL) {
                    If (r != NULL) {
                        r->SetVoiceCallForwardingFlag(1, (infos[i].mStatus == 1),
                            infos[i].mNumber);
                    }
                }
                cfInfos[i] = GetCallForwardInfo(infos[i]);
            }
        }

        return cfInfos;
    }

    private Int32[] HandleCbQueryResult(ImsSsInfo[] infos) {
        Int32[] cbInfos = new Int32[1];
        cbInfos[0] = SERVICE_CLASS_NONE;

        If (infos[0].mStatus == 1) {
            cbInfos[0] = SERVICE_CLASS_VOICE;
        }

        return cbInfos;
    }

    private Int32[] HandleCwQueryResult(ImsSsInfo[] infos) {
        Int32[] cwInfos = new Int32[2];
        cwInfos[0] = 0;

        If (infos[0].mStatus == 1) {
            cwInfos[0] = 1;
            cwInfos[1] = SERVICE_CLASS_VOICE;
        }

        return cwInfos;
    }

    private void
    SendResponse(Message onComplete, Object result, Throwable e) {
        If (onComplete != NULL) {
            CommandException ex = NULL;
            ImsException imsEx = NULL;
            If (e != NULL) {
                If (e instanceof ImsException) {
                    imsEx = (ImsException) e;
                    AsyncResult->ForMessage(onComplete, result, imsEx);
                } else {
                    ex = GetCommandException(e);
                    AsyncResult->ForMessage(onComplete, result, ex);
                }
            } else {
                AsyncResult->ForMessage(onComplete, result, NULL);
            }
            onComplete->SendToTarget();
        }
    }

    //@Override
    CARAPI HandleMessage (Message msg) {
        AsyncResult ar = (AsyncResult) msg.obj;
        Message onComplete;

        If (DBG) Rlog->D(LOG_TAG, "handleMessage what=" + msg.what);
        Switch (msg.what) {
            case EVENT_SET_CALL_FORWARD_DONE:
                IccRecords r = GetIccRecords();
                Cf cf = (Cf) ar.userObj;
                If (cf.mIsCfu && ar.exception == NULL && r != NULL) {
                    r->SetVoiceCallForwardingFlag(1, msg.arg1 == 1, cf.mSetCfNumber);
                }
                SendResponse(cf.mOnComplete, NULL, ar.exception);
                break;

            case EVENT_GET_CALL_FORWARD_DONE:
                CallForwardInfo[] cfInfos = NULL;
                If (ar.exception == NULL) {
                    cfInfos = HandleCfQueryResult((ImsCallForwardInfo[])ar.result);
                }
                SendResponse((Message) ar.userObj, cfInfos, ar.exception);
                break;

             case EVENT_SET_CALL_FORWARD_TIMER_DONE:
                Cf cft = (Cf) ar.userObj;
                SendResponse(cft.mOnComplete, NULL, ar.exception);
                break;

             case EVENT_GET_CALL_BARRING_DONE:
             case EVENT_GET_CALL_WAITING_DONE:
                Int32[] ssInfos = NULL;
                If (ar.exception == NULL) {
                    If (msg.what == EVENT_GET_CALL_BARRING_DONE) {
                        ssInfos = HandleCbQueryResult((ImsSsInfo[])ar.result);
                    } else If (msg.what == EVENT_GET_CALL_WAITING_DONE) {
                        ssInfos = HandleCwQueryResult((ImsSsInfo[])ar.result);
                    }
                }
                SendResponse((Message) ar.userObj, ssInfos, ar.exception);
                break;

             case EVENT_SET_CALL_BARRING_DONE:
             case EVENT_SET_CALL_WAITING_DONE:
                SendResponse((Message) ar.userObj, NULL, ar.exception);
                break;

             default:
                 super->HandleMessage(msg);
                 break;
        }
    }

    /**
     * Listen to the IMS ECBM state change
     */
    ImsEcbmStateListener mImsEcbmStateListener =
            new ImsEcbmStateListener() {
                //@Override
                CARAPI OnECBMEntered() {
                    If (DBG) Rlog->D(LOG_TAG, "onECBMEntered");
                    HandleEnterEmergencyCallbackMode();
                }

                //@Override
                CARAPI OnECBMExited() {
                    If (DBG) Rlog->D(LOG_TAG, "onECBMExited");
                    HandleExitEmergencyCallbackMode();
                }
            };

    public Boolean IsInEmergencyCall() {
        return mCT->IsInEmergencyCall();
    }

    public Boolean IsInEcm() {
        return mIsPhoneInEcmState;
    }

    void SendEmergencyCallbackModeChange() {
        // Send an Intent
        Intent intent = new Intent(TelephonyIntents.ACTION_EMERGENCY_CALLBACK_MODE_CHANGED);
        intent->PutExtra(PhoneConstants.PHONE_IN_ECM_STATE, mIsPhoneInEcmState);
        SubscriptionManager->PutPhoneIdAndSubIdExtra(intent, GetPhoneId());
        ActivityManagerNative->BroadcastStickyIntent(intent, NULL, UserHandle.USER_ALL);
        If (DBG) Rlog->D(LOG_TAG, "sendEmergencyCallbackModeChange");
    }

    //@Override
    CARAPI ExitEmergencyCallbackMode() {
        If (mWakeLock->IsHeld()) {
            mWakeLock->Release();
        }
        If (DBG) Rlog->D(LOG_TAG, "ExitEmergencyCallbackMode()");

        // Send a message which will invoke handleExitEmergencyCallbackMode
        ImsEcbm ecbm;
        try {
            ecbm = mCT->GetEcbmInterface();
            ecbm->ExitEmergencyCallbackMode();
        } Catch (ImsException e) {
            e->PrintStackTrace();
        }
    }

    private void HandleEnterEmergencyCallbackMode() {
        If (DBG) {
            Rlog->D(LOG_TAG, "handleEnterEmergencyCallbackMode,mIsPhoneInEcmState= "
                    + mIsPhoneInEcmState);
        }
        // if phone is not in Ecm mode, and it's changed to Ecm mode
        If (mIsPhoneInEcmState == FALSE) {
            mIsPhoneInEcmState = TRUE;
            // notify change
            SendEmergencyCallbackModeChange();
            SetSystemProperty(TelephonyProperties.PROPERTY_INECM_MODE, "TRUE");

            // Post this runnable so we will automatically exit
            // if no one invokes ExitEmergencyCallbackMode() directly.
            Int64 delayInMillis = SystemProperties->GetLong(
                    TelephonyProperties.PROPERTY_ECM_EXIT_TIMER, DEFAULT_ECM_EXIT_TIMER_VALUE);
            PostDelayed(mExitEcmRunnable, delayInMillis);
            // We don't want to go to sleep while in Ecm
            mWakeLock->Acquire();
        }
    }

    private void HandleExitEmergencyCallbackMode() {
        If (DBG) {
            Rlog->D(LOG_TAG, "handleExitEmergencyCallbackMode: mIsPhoneInEcmState = "
                    + mIsPhoneInEcmState);
        }
        // Remove pending exit Ecm runnable, if any
        RemoveCallbacks(mExitEcmRunnable);

        If (mEcmExitRespRegistrant != NULL) {
            mEcmExitRespRegistrant->NotifyResult(Boolean.TRUE);
        }
            If (mIsPhoneInEcmState) {
                mIsPhoneInEcmState = FALSE;
                SetSystemProperty(TelephonyProperties.PROPERTY_INECM_MODE, "FALSE");
            }
            // send an Intent
            SendEmergencyCallbackModeChange();
    }

    /**
     * Handle to cancel or restart Ecm timer in emergency call back mode if action is
     * CANCEL_ECM_TIMER, cancel Ecm timer and notify apps the timer is canceled; otherwise, restart
     * Ecm timer and notify apps the timer is restarted.
     */
    void HandleTimerInEmergencyCallbackMode(Int32 action) {
        Switch (action) {
            case CANCEL_ECM_TIMER:
                RemoveCallbacks(mExitEcmRunnable);
                If (mDefaultPhone->GetPhoneType() == PhoneConstants.PHONE_TYPE_GSM) {
                    ((GSMPhone) mDefaultPhone).NotifyEcbmTimerReset(Boolean.TRUE);
                } else { // Should be CDMA - also go here by default
                    ((CDMAPhone) mDefaultPhone).NotifyEcbmTimerReset(Boolean.TRUE);
                }
                break;
            case RESTART_ECM_TIMER:
                Int64 delayInMillis = SystemProperties->GetLong(
                        TelephonyProperties.PROPERTY_ECM_EXIT_TIMER, DEFAULT_ECM_EXIT_TIMER_VALUE);
                PostDelayed(mExitEcmRunnable, delayInMillis);
                If (mDefaultPhone->GetPhoneType() == PhoneConstants.PHONE_TYPE_GSM) {
                    ((GSMPhone) mDefaultPhone).NotifyEcbmTimerReset(Boolean.FALSE);
                } else { // Should be CDMA - also go here by default
                    ((CDMAPhone) mDefaultPhone).NotifyEcbmTimerReset(Boolean.FALSE);
                }
                break;
            default:
                Rlog->E(LOG_TAG, "handleTimerInEmergencyCallbackMode, unsupported action " + action);
        }
    }

    CARAPI SetOnEcbModeExitResponse(Handler h, Int32 what, Object obj) {
        mEcmExitRespRegistrant = new Registrant(h, what, obj);
    }

    CARAPI UnsetOnEcbModeExitResponse(Handler h) {
        mEcmExitRespRegistrant->Clear();
    }

    public Boolean IsVolteEnabled() {
        return mCT->IsVolteEnabled();
    }

    public Boolean IsVtEnabled() {
        return mCT->IsVtEnabled();
    }
}
