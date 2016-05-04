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

using Elastos::Droid::App::IActivityManagerNative;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::PowerManager::IWakeLock;
using Elastos::Droid::Os::IRegistrant;
using Elastos::Droid::Os::IRegistrantList;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Preference::IPreferenceManager;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Provider::ITelephony;
using Elastos::Droid::Telephony::ICellLocation;
using Elastos::Droid::Telephony::IPhoneNumberUtils;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Droid::Telephony::ISubscriptionManager;
using Elastos::Droid::Telephony::Cdma::ICdmaCellLocation;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Telephony::IRlog;

using Elastos::Droid::Ims::IImsManager;
using Elastos::Droid::Internal::Telephony::ICall;
using Elastos::Droid::Internal::Telephony::ICallTracker;
using Elastos::Droid::Internal::Telephony::ICommandsInterface;
using Elastos::Droid::Internal::Telephony::IConnection;
using Elastos::Droid::Internal::Telephony::IIccPhoneBookInterfaceManager;
using Elastos::Droid::Internal::Telephony::IMccTable;
using Elastos::Droid::Internal::Telephony::IMmiCode;
using Elastos::Droid::Internal::Telephony::IOperatorInfo;
using Elastos::Droid::Internal::Telephony::IPhoneBase;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Internal::Telephony::IPhoneFactory;
using Elastos::Droid::Internal::Telephony::IPhoneNotifier;
using Elastos::Droid::Internal::Telephony::IPhoneProxy;
using Elastos::Droid::Internal::Telephony::IPhoneSubInfo;
using Elastos::Droid::Internal::Telephony::IServiceStateTracker;
using Elastos::Droid::Internal::Telephony::ISubInfoRecordUpdater;
using Elastos::Droid::Internal::Telephony::ISubscriptionController;
using Elastos::Droid::Internal::Telephony::ITelephonyIntents;
using Elastos::Droid::Internal::Telephony::ITelephonyProperties;
using Elastos::Droid::Internal::Telephony::IUUSInfo;
using Elastos::Droid::Internal::Telephony::Dataconnection::IDcTracker;
using Elastos::Droid::Internal::Telephony::Imsphone::IImsPhone;
using Elastos::Droid::Internal::Telephony::Uicc::IIccRecords;
using Elastos::Droid::Internal::Telephony::Uicc::IRuimRecords;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccCard;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccCardApplication;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccController;

using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;
using Elastos::Utility::Regex::IMatcher;
using Elastos::Utility::Regex::IPattern;

using static com::Android::Internal::Telephony::TelephonyProperties::IPROPERTY_ICC_OPERATOR_ALPHA;
using static com::Android::Internal::Telephony::TelephonyProperties::IPROPERTY_ICC_OPERATOR_ISO_COUNTRY;
using static com::Android::Internal::Telephony::TelephonyProperties::IPROPERTY_ICC_OPERATOR_NUMERIC;

/**
 * {@hide}
 */
public class CDMAPhone extends PhoneBase {
    static const String LOG_TAG = "CDMAPhone";
    private static const Boolean DBG = TRUE;
    private static const Boolean VDBG = FALSE; /* STOP SHIP if TRUE */

    // Default Emergency Callback Mode exit timer
    private static const Int32 DEFAULT_ECM_EXIT_TIMER_VALUE = 300000;

    private static const String VM_NUMBER_CDMA = "vm_number_key_cdma";
    private String mVmNumber = NULL;

    static const Int32 RESTART_ECM_TIMER = 0; // restart Ecm timer
    static const Int32 CANCEL_ECM_TIMER = 1; // cancel Ecm timer

    // Instance Variables
    CdmaCallTracker mCT;
    CdmaServiceStateTracker mSST;
    CdmaSubscriptionSourceManager mCdmaSSM;
    ArrayList <CdmaMmiCode> mPendingMmis = new ArrayList<CdmaMmiCode>();
    RuimPhoneBookInterfaceManager mRuimPhoneBookInterfaceManager;
    Int32 mCdmaSubscriptionSource =
            CdmaSubscriptionSourceManager.SUBSCRIPTION_SOURCE_UNKNOWN;
    PhoneSubInfo mSubInfo;
    EriManager mEriManager;
    WakeLock mWakeLock;

    // mEriFileLoadedRegistrants are informed after the ERI text has been loaded
    private final RegistrantList mEriFileLoadedRegistrants = new RegistrantList();

    // mEcmTimerResetRegistrants are informed after Ecm timer is canceled or re-started
    private final RegistrantList mEcmTimerResetRegistrants = new RegistrantList();

    // mEcmExitRespRegistrant is informed after the phone has been exited
    //the emergency callback mode
    //keep track of if phone is in emergency callback mode
    protected Boolean mIsPhoneInEcmState;
    private Registrant mEcmExitRespRegistrant;
    protected String mImei;
    protected String mImeiSv;
    private String mEsn;
    private String mMeid;
    // string to define how the carrier specifies its own ota sp number
    protected String mCarrierOtaSpNumSchema;

    // A runnable which is used to automatically exit from Ecm after a period of time.
    private Runnable mExitEcmRunnable = new Runnable() {
        //@Override
        CARAPI Run() {
            ExitEmergencyCallbackMode();
        }
    };

    Registrant mPostDialHandler;

    public static const String PROPERTY_CDMA_HOME_OPERATOR_NUMERIC =
            "ro.cdma.home.operator.numeric";

    // Constructors
    public CDMAPhone(Context context, CommandsInterface ci, PhoneNotifier notifier) {
        Super("CDMA", notifier, context, ci, FALSE);
        InitSstIcc();
        Init(context, notifier);
    }

    public CDMAPhone(Context context, CommandsInterface ci, PhoneNotifier notifier,
            Int32 phoneId) {
        Super("CDMA", notifier, context, ci, FALSE, phoneId);
        InitSstIcc();
        Init(context, notifier);
    }

    public CDMAPhone(Context context, CommandsInterface ci, PhoneNotifier notifier,
            Boolean unitTestMode) {
        Super("CDMA", notifier, context, ci, unitTestMode);
        InitSstIcc();
        Init(context, notifier);
    }

    protected void InitSstIcc() {
        mSST = new CdmaServiceStateTracker(this);
    }

    protected void Init(Context context, PhoneNotifier notifier) {
        mCi->SetPhoneType(PhoneConstants.PHONE_TYPE_CDMA);
        mCT = new CdmaCallTracker(this);
        mCdmaSSM = CdmaSubscriptionSourceManager->GetInstance(context, mCi, this,
                EVENT_CDMA_SUBSCRIPTION_SOURCE_CHANGED, NULL);
        mDcTracker = new DcTracker(this);
        mRuimPhoneBookInterfaceManager = new RuimPhoneBookInterfaceManager(this);
        mSubInfo = new PhoneSubInfo(this);
        mEriManager = new EriManager(this, context, EriManager.ERI_FROM_XML);

        mCi->RegisterForAvailable(this, EVENT_RADIO_AVAILABLE, NULL);
        mCi->RegisterForOffOrNotAvailable(this, EVENT_RADIO_OFF_OR_NOT_AVAILABLE, NULL);
        mCi->RegisterForOn(this, EVENT_RADIO_ON, NULL);
        mCi->SetOnSuppServiceNotification(this, EVENT_SSN, NULL);
        mSST->RegisterForNetworkAttached(this, EVENT_REGISTERED_TO_NETWORK, NULL);
        mCi->SetEmergencyCallbackMode(this, EVENT_EMERGENCY_CALLBACK_MODE_ENTER, NULL);
        mCi->RegisterForExitEmergencyCallbackMode(this, EVENT_EXIT_EMERGENCY_CALLBACK_RESPONSE,
                NULL);

        PowerManager pm
            = (PowerManager) context->GetSystemService(Context.POWER_SERVICE);
        mWakeLock = pm->NewWakeLock(PowerManager.PARTIAL_WAKE_LOCK,LOG_TAG);

        //Change the system setting
        SystemProperties->Set(TelephonyProperties.CURRENT_ACTIVE_PHONE,
                Integer->ToString(PhoneConstants.PHONE_TYPE_CDMA));

        // This is needed to handle phone process crashes
        String inEcm=SystemProperties->Get(TelephonyProperties.PROPERTY_INECM_MODE, "FALSE");
        mIsPhoneInEcmState = inEcm->Equals("TRUE");
        If (mIsPhoneInEcmState) {
            // Send a message which will invoke handleExitEmergencyCallbackMode
            mCi->ExitEmergencyCallbackMode(ObtainMessage(EVENT_EXIT_EMERGENCY_CALLBACK_RESPONSE));
        }

        // get the string that specifies the carrier OTA Sp number
        mCarrierOtaSpNumSchema = SystemProperties->Get(
                TelephonyProperties.PROPERTY_OTASP_NUM_SCHEMA,"");

        // Sets operator properties by retrieving from build-time system property
        String operatorAlpha = SystemProperties->Get("ro.cdma.home.operator.alpha");
        String operatorNumeric = SystemProperties->Get(PROPERTY_CDMA_HOME_OPERATOR_NUMERIC);
        Log("init: operatorAlpha='" + operatorAlpha
                + "' operatorNumeric='" + operatorNumeric + "'");
        If (mUiccController->GetUiccCardApplication(mPhoneId, UiccController.APP_FAM_3GPP) == NULL) {
            Log("init: APP_FAM_3GPP == NULL");
            If (!TextUtils->IsEmpty(operatorAlpha)) {
                Log("init: set 'gsm.sim.operator.alpha' to operator='" + operatorAlpha + "'");
                SetSystemProperty(PROPERTY_ICC_OPERATOR_ALPHA, operatorAlpha);
            }
            If (!TextUtils->IsEmpty(operatorNumeric)) {
                Log("init: set 'gsm.sim.operator.numeric' to operator='" + operatorNumeric + "'");
                Log("update icc_operator_numeric=" + operatorNumeric);
                SetSystemProperty(PROPERTY_ICC_OPERATOR_NUMERIC, operatorNumeric);

                SubscriptionController->GetInstance()->SetMccMnc(operatorNumeric, GetSubId());
            }
            SetIsoCountryProperty(operatorNumeric);
        }

        // Sets current entry in the telephony carrier table
        UpdateCurrentCarrierInProvider(operatorNumeric);
    }

    //@Override
    CARAPI Dispose() {
        Synchronized(PhoneProxy.lockForRadioTechnologyChange) {
            super->Dispose();
            Log("dispose");

            //Unregister from all former registered events
            UnregisterForRuimRecordEvents();
            mCi->UnregisterForAvailable(this); //EVENT_RADIO_AVAILABLE
            mCi->UnregisterForOffOrNotAvailable(this); //EVENT_RADIO_OFF_OR_NOT_AVAILABLE
            mCi->UnregisterForOn(this); //EVENT_RADIO_ON
            mSST->UnregisterForNetworkAttached(this); //EVENT_REGISTERED_TO_NETWORK
            mCi->UnSetOnSuppServiceNotification(this);
            mCi->UnregisterForExitEmergencyCallbackMode(this);
            RemoveCallbacks(mExitEcmRunnable);

            mPendingMmis->Clear();

            //Force all referenced classes to unregister their former registered events
            mCT->Dispose();
            mDcTracker->Dispose();
            mSST->Dispose();
            mCdmaSSM->Dispose(this);
            mRuimPhoneBookInterfaceManager->Dispose();
            mSubInfo->Dispose();
            mEriManager->Dispose();
        }
    }

    //@Override
    CARAPI RemoveReferences() {
        Log("removeReferences");
        mRuimPhoneBookInterfaceManager = NULL;
        mSubInfo = NULL;
        mCT = NULL;
        mSST = NULL;
        mEriManager = NULL;
        mExitEcmRunnable = NULL;

        super->RemoveReferences();
    }

    //@Override
    protected void Finalize() {
        If(DBG) Rlog->D(LOG_TAG, "CDMAPhone finalized");
        If (mWakeLock->IsHeld()) {
            Rlog->E(LOG_TAG, "UNEXPECTED; mWakeLock is held when finalizing.");
            mWakeLock->Release();
        }
    }

    //@Override
    public ServiceState GetServiceState() {
        If (mSST == NULL || mSST.mSS->GetState() != ServiceState.STATE_IN_SERVICE) {
            If (mImsPhone != NULL &&
                    mImsPhone->GetServiceState()->GetState() == ServiceState.STATE_IN_SERVICE) {
                return mImsPhone->GetServiceState();
            }
        }

        If (mSST != NULL) {
            return mSST.mSS;
        } else {
            // avoid potential NPE in EmergencyCallHelper during Phone switch
            return new ServiceState();
        }
    }


    //@Override
    public CallTracker GetCallTracker() {
        return mCT;
    }

    //@Override
    public PhoneConstants.State GetState() {
        return mCT.mState;
    }

    //@Override
    public ServiceStateTracker GetServiceStateTracker() {
        return mSST;
    }

    //@Override
    public Int32 GetPhoneType() {
        return PhoneConstants.PHONE_TYPE_CDMA;
    }

    //@Override
    public Boolean CanTransfer() {
        Rlog->E(LOG_TAG, "canTransfer: not possible in CDMA");
        return FALSE;
    }

    //@Override
    public Call GetRingingCall() {
        ImsPhone imPhone = mImsPhone;
        If ( mCT.mRingingCall != NULL && mCT.mRingingCall->IsRinging() ) {
            return mCT.mRingingCall;
        } else If ( imPhone != NULL ) {
            return imPhone->GetRingingCall();
        }
        return mCT.mRingingCall;
    }

    //@Override
    CARAPI SetUiTTYMode(Int32 uiTtyMode, Message onComplete) {
       If (mImsPhone != NULL) {
           mImsPhone->SetUiTTYMode(uiTtyMode, onComplete);
       }
    }

    //@Override
    CARAPI SetMute(Boolean muted) {
        mCT->SetMute(muted);
    }

    //@Override
    public Boolean GetMute() {
        return mCT->GetMute();
    }

    //@Override
    CARAPI Conference() {
        If (mImsPhone != NULL && mImsPhone->CanConference()) {
            Log("Conference() - delegated to IMS phone");
            mImsPhone->Conference();
            return;
        }
        // three way calls in CDMA will be handled by feature codes
        Rlog->E(LOG_TAG, "conference: not possible in CDMA");
    }

    //@Override
    CARAPI EnableEnhancedVoicePrivacy(Boolean enable, Message onComplete) {
        mCi->SetPreferredVoicePrivacy(enable, onComplete);
    }

    //@Override
    CARAPI GetEnhancedVoicePrivacy(Message onComplete) {
        mCi->GetPreferredVoicePrivacy(onComplete);
    }

    //@Override
    CARAPI ClearDisconnected() {
        mCT->ClearDisconnected();
    }

    //@Override
    public DataActivityState GetDataActivityState() {
        DataActivityState ret = DataActivityState.NONE;

        If (mSST->GetCurrentDataConnectionState() == ServiceState.STATE_IN_SERVICE) {

            Switch (mDcTracker->GetActivity()) {
                case DATAIN:
                    ret = DataActivityState.DATAIN;
                break;

                case DATAOUT:
                    ret = DataActivityState.DATAOUT;
                break;

                case DATAINANDOUT:
                    ret = DataActivityState.DATAINANDOUT;
                break;

                case DORMANT:
                    ret = DataActivityState.DORMANT;
                break;

                default:
                    ret = DataActivityState.NONE;
                break;
            }
        }
        return ret;
    }

    //@Override
    public Connection
    Dial (String dialString, Int32 videoState) throws CallStateException {
        Return (Dial(dialString, videoState, NULL));
    }

    //@Override
    public Connection
    Dial (String dialString, Int32 videoState, Bundle extras) throws CallStateException {
        ImsPhone imsPhone = mImsPhone;

        Boolean imsUseEnabled =
                ImsManager->IsEnhanced4gLteModeSettingEnabledByPlatform(mContext) &&
                ImsManager->IsEnhanced4gLteModeSettingEnabledByUser(mContext);
        If (!imsUseEnabled) {
            Rlog->W(LOG_TAG, "IMS is disabled: forced to CS");
        }

        If (imsUseEnabled && imsPhone != NULL
                && ((imsPhone->GetServiceState()->GetState() == ServiceState.STATE_IN_SERVICE
                && !PhoneNumberUtils->IsEmergencyNumber(dialString))
                || (PhoneNumberUtils->IsEmergencyNumber(dialString)
                && mContext->GetResources()->GetBoolean(
                        R.bool.useImsAlwaysForEmergencyCall))) ) {
            try {
                If (DBG) Rlog->D(LOG_TAG, "Trying IMS PS call");
                return imsPhone->Dial(dialString, videoState, extras);
            } Catch (CallStateException e) {
                If (DBG) Rlog->D(LOG_TAG, "IMS PS call exception " + e);
                If (!ImsPhone.CS_FALLBACK->Equals(e->GetMessage())) {
                    CallStateException ce = new CallStateException(e->GetMessage());
                    ce->SetStackTrace(e->GetStackTrace());
                    throw ce;
                }
            }
        }

        If (DBG) Rlog->D(LOG_TAG, "Trying (non-IMS) CS call");
        return DialInternal(dialString, NULL, videoState);
    }


    //@Override
    protected Connection
    DialInternal (String dialString, UUSInfo uusInfo,
            Int32 videoState) throws CallStateException {
        // Need to make sure dialString gets parsed properly
        String newDialString = PhoneNumberUtils->StripSeparators(dialString);
        return mCT->Dial(newDialString);
    }

    Connection
    Dial(String dialString, UUSInfo uusInfo, Int32 videoState, Bundle extras)
            throws CallStateException {
        return Dial(dialString, uusInfo, videoState, NULL);
    }

    //@Override
    public Connection Dial(String dialString, UUSInfo uusInfo, Int32 videoState)
            throws CallStateException {
        throw new CallStateException("Sending UUS information NOT supported in CDMA!");
    }

    //@Override
    public List<? extends MmiCode>
    GetPendingMmiCodes() {
        return mPendingMmis;
    }

    //@Override
    CARAPI RegisterForSuppServiceNotification(
            Handler h, Int32 what, Object obj) {
        Rlog->E(LOG_TAG, "method registerForSuppServiceNotification is NOT supported in CDMA!");
    }

    //@Override
    public CdmaCall GetBackgroundCall() {
        return mCT.mBackgroundCall;
    }

    //@Override
    public Boolean HandleInCallMmiCommands(String dialString) {
        Rlog->E(LOG_TAG, "method handleInCallMmiCommands is NOT supported in CDMA!");
        return FALSE;
    }

    Boolean IsInCall() {
        CdmaCall.State foregroundCallState = GetForegroundCall()->GetState();
        CdmaCall.State backgroundCallState = GetBackgroundCall()->GetState();
        CdmaCall.State ringingCallState = GetRingingCall()->GetState();

        Return (foregroundCallState->IsAlive() || backgroundCallState->IsAlive() || ringingCallState
                .IsAlive());
    }

    //@Override
    CARAPI
    SetNetworkSelectionModeAutomatic(Message response) {
        Rlog->E(LOG_TAG, "method setNetworkSelectionModeAutomatic is NOT supported in CDMA!");
        If (response != NULL) {
            Rlog->E(LOG_TAG,
                    "setNetworkSelectionModeAutomatic: not possible in CDMA- Posting exception");
            CommandException ce = new CommandException(
                    CommandException.Error.REQUEST_NOT_SUPPORTED);
            AsyncResult->ForMessage(response).exception = ce;
            response->SendToTarget();
        }
    }

    //@Override
    CARAPI UnregisterForSuppServiceNotification(Handler h) {
        Rlog->E(LOG_TAG, "method unregisterForSuppServiceNotification is NOT supported in CDMA!");
    }

    //@Override
    CARAPI
    AcceptCall(Int32 videoState) throws CallStateException {
        ImsPhone imsPhone = mImsPhone;
        If ( imsPhone != NULL && imsPhone->GetRingingCall()->IsRinging() ) {
            imsPhone->AcceptCall(videoState);
        } else {
            mCT->AcceptCall();
        }
    }

    //@Override
    CARAPI
    DeflectCall(String number) throws CallStateException {
        ImsPhone imsPhone = mImsPhone;
        If ( imsPhone != NULL && imsPhone->GetRingingCall()->IsRinging() ) {
            imsPhone->DeflectCall(number);
        } else {
            throw new CallStateException("Deflect call NOT supported in CDMA!");
        }
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
    public String GetIccSerialNumber() {
        IccRecords r = mIccRecords->Get();
        If (r == NULL) {
            // to get ICCID form SIMRecords because it is on MF.
            r = mUiccController->GetIccRecords(mPhoneId, UiccController.APP_FAM_3GPP);
        }
        Return (r != NULL) ? r->GetIccId() : NULL;
    }

    //@Override
    public String GetLine1Number() {
        return mSST->GetMdnNumber();
    }

    //@Override
    public String GetCdmaPrlVersion(){
        return mSST->GetPrlVersion();
    }

    //@Override
    public String GetCdmaMin() {
        return mSST->GetCdmaMin();
    }

    //@Override
    public Boolean IsMinInfoReady() {
        return mSST->IsMinInfoReady();
    }

    //@Override
    CARAPI GetCallWaiting(Message onComplete) {
        mCi->QueryCallWaiting(CommandsInterface.SERVICE_CLASS_VOICE, onComplete);
    }

    //@Override
    CARAPI
    SetRadioPower(Boolean power) {
        mSST->SetRadioPower(power);
    }

    //@Override
    public String GetEsn() {
        return mEsn;
    }

    //@Override
    public String GetMeid() {
        return mMeid;
    }

    //returns MEID or ESN in CDMA
    //@Override
    public String GetDeviceId() {
        String id = GetMeid();
        If ((id == NULL) || id->Matches("^0*$")) {
            Rlog->D(LOG_TAG, "GetDeviceId(): MEID is not initialized use ESN");
            id = GetEsn();
        }
        return id;
    }

    //@Override
    public String GetDeviceSvn() {
        Rlog->D(LOG_TAG, "GetDeviceSvn(): return 0");
        return "0";
    }

    //@Override
    public String GetSubscriberId() {
        return mSST->GetImsi();
    }

    //@Override
    public String GetGroupIdLevel1() {
        Rlog->E(LOG_TAG, "GID1 is not available in CDMA");
        return NULL;
    }

    //@Override
    public String GetImei() {
        Rlog->E(LOG_TAG, "GetImei() called for CDMAPhone");
        return mImei;
    }

    //@Override
    public Boolean CanConference() {
        If (mImsPhone != NULL && mImsPhone->CanConference()) {
            return TRUE;
        }
        Rlog->E(LOG_TAG, "canConference: not possible in CDMA");
        return FALSE;
    }

    //@Override
    public CellLocation GetCellLocation() {
        CdmaCellLocation loc = mSST.mCellLoc;

        Int32 mode = Settings.Secure->GetInt(GetContext()->GetContentResolver(),
                Settings.Secure.LOCATION_MODE, Settings.Secure.LOCATION_MODE_OFF);
        If (mode == Settings.Secure.LOCATION_MODE_OFF) {
            // clear lat/Int64 values for location privacy
            CdmaCellLocation privateLoc = new CdmaCellLocation();
            privateLoc->SetCellLocationData(loc->GetBaseStationId(),
                    CdmaCellLocation.INVALID_LAT_LONG,
                    CdmaCellLocation.INVALID_LAT_LONG,
                    loc->GetSystemId(), loc->GetNetworkId());
            loc = privateLoc;
        }
        return loc;
    }

    //@Override
    public CdmaCall GetForegroundCall() {
        return mCT.mForegroundCall;
    }

    //@Override
    CARAPI
    SelectNetworkManually(OperatorInfo network,
            Message response) {
        Rlog->E(LOG_TAG, "selectNetworkManually: not possible in CDMA");
        If (response != NULL) {
            CommandException ce = new CommandException(
                    CommandException.Error.REQUEST_NOT_SUPPORTED);
            AsyncResult->ForMessage(response).exception = ce;
            response->SendToTarget();
        }
    }

    //@Override
    CARAPI SetOnPostDialCharacter(Handler h, Int32 what, Object obj) {
        mPostDialHandler = new Registrant(h, what, obj);
    }

    //@Override
    public Boolean HandlePinMmi(String dialString) {
        CdmaMmiCode mmi = CdmaMmiCode->NewFromDialString(dialString, this, mUiccApplication->Get());

        If (mmi == NULL) {
            Rlog->E(LOG_TAG, "Mmi is NULL!");
            return FALSE;
        } else If (mmi->IsPinPukCommand()) {
            mPendingMmis->Add(mmi);
            mMmiRegistrants->NotifyRegistrants(new AsyncResult(NULL, mmi, NULL));
            mmi->ProcessCode();
            return TRUE;
        }
        Rlog->E(LOG_TAG, "Unrecognized mmi!");
        return FALSE;
    }

    /**
     * Removes the given MMI from the pending list and notifies registrants that
     * it is complete.
     *
     * @param mmi MMI that is done
     */
    void OnMMIDone(CdmaMmiCode mmi) {
        /*
         * Only notify complete if it's on the pending list. Otherwise, it's
         * already been Handled (eg, previously canceled).
         */
        If (mPendingMmis->Remove(mmi)) {
            mMmiCompleteRegistrants->NotifyRegistrants(new AsyncResult(NULL, mmi, NULL));
        }
    }

    //@Override
    CARAPI SetLine1Number(String alphaTag, String number, Message onComplete) {
        Rlog->E(LOG_TAG, "setLine1Number: not possible in CDMA");
    }

    //@Override
    CARAPI SetCallWaiting(Boolean enable, Message onComplete) {
        Rlog->E(LOG_TAG, "method setCallWaiting is NOT supported in CDMA!");
    }

    //@Override
    CARAPI UpdateServiceLocation() {
        mSST->EnableSingleLocationUpdate();
    }

    //@Override
    CARAPI SetDataRoamingEnabled(Boolean enable) {
        mDcTracker->SetDataOnRoamingEnabled(enable);
    }

    //@Override
    CARAPI RegisterForCdmaOtaStatusChange(Handler h, Int32 what, Object obj) {
        mCi->RegisterForCdmaOtaProvision(h, what, obj);
    }

    //@Override
    CARAPI UnregisterForCdmaOtaStatusChange(Handler h) {
        mCi->UnregisterForCdmaOtaProvision(h);
    }

    //@Override
    CARAPI RegisterForSubscriptionInfoReady(Handler h, Int32 what, Object obj) {
        mSST->RegisterForSubscriptionInfoReady(h, what, obj);
    }

    //@Override
    CARAPI UnregisterForSubscriptionInfoReady(Handler h) {
        mSST->UnregisterForSubscriptionInfoReady(h);
    }

    //@Override
    CARAPI SetOnEcbModeExitResponse(Handler h, Int32 what, Object obj) {
        mEcmExitRespRegistrant = new Registrant (h, what, obj);
    }

    //@Override
    CARAPI UnsetOnEcbModeExitResponse(Handler h) {
        mEcmExitRespRegistrant->Clear();
    }

    //@Override
    CARAPI RegisterForCallWaiting(Handler h, Int32 what, Object obj) {
        mCT->RegisterForCallWaiting(h, what, obj);
    }

    //@Override
    CARAPI UnregisterForCallWaiting(Handler h) {
        mCT->UnregisterForCallWaiting(h);
    }

    //@Override
    CARAPI
    GetNeighboringCids(Message response) {
        /*
         * This is currently not implemented.  At least as of June
         * 2009, there is no neighbor cell information available for
         * CDMA because some party is resisting making this
         * information readily available.  Consequently, calling this
         * function can have no useful effect.  This situation may
         * (and hopefully will) change in the future.
         */
        If (response != NULL) {
            CommandException ce = new CommandException(
                    CommandException.Error.REQUEST_NOT_SUPPORTED);
            AsyncResult->ForMessage(response).exception = ce;
            response->SendToTarget();
        }
    }

    //@Override
    public PhoneConstants.DataState GetDataConnectionState(String apnType) {
        PhoneConstants.DataState ret = PhoneConstants.DataState.DISCONNECTED;

        If (mSST == NULL) {
             // Radio Technology Change is ongoning, Dispose() and RemoveReferences() have
             // already been called

             ret = PhoneConstants.DataState.DISCONNECTED;
        } else If (mSST->GetCurrentDataConnectionState() != ServiceState.STATE_IN_SERVICE
                && mOosIsDisconnect) {
            // If we're out of service, open TCP sockets may still work
            // but no data will flow
            ret = PhoneConstants.DataState.DISCONNECTED;
            Log("getDataConnectionState: Data is Out of Service. ret = " + ret);
        } else If (mDcTracker->IsApnTypeEnabled(apnType) == FALSE ||
                mDcTracker->IsApnTypeActive(apnType) == FALSE) {
            ret = PhoneConstants.DataState.DISCONNECTED;
        } else {
            Switch (mDcTracker->GetState(apnType)) {
                case RETRYING:
                case FAILED:
                case IDLE:
                    ret = PhoneConstants.DataState.DISCONNECTED;
                break;

                case CONNECTED:
                case DISCONNECTING:
                    If ( mCT.mState != PhoneConstants.State.IDLE
                            && !mSST->IsConcurrentVoiceAndDataAllowed()) {
                        ret = PhoneConstants.DataState.SUSPENDED;
                    } else {
                        ret = PhoneConstants.DataState.CONNECTED;
                    }
                break;

                case CONNECTING:
                case SCANNING:
                    ret = PhoneConstants.DataState.CONNECTING;
                break;
            }
        }

        Log("getDataConnectionState apnType=" + apnType + " ret=" + ret);
        return ret;
    }

    //@Override
    CARAPI SendUssdResponse(String ussdMessge) {
        Rlog->E(LOG_TAG, "sendUssdResponse: not possible in CDMA");
    }

    //@Override
    CARAPI SendDtmf(Char32 c) {
        If (!PhoneNumberUtils->Is12Key(c)) {
            Rlog->E(LOG_TAG,
                    "sendDtmf called with invalid character '" + c + "'");
        } else {
            If (mCT.mState ==  PhoneConstants.State.OFFHOOK) {
                mCi->SendDtmf(c, NULL);
            }
        }
    }

    //@Override
    CARAPI StartDtmf(Char32 c) {
        If (!PhoneNumberUtils->Is12Key(c)) {
            Rlog->E(LOG_TAG,
                    "startDtmf called with invalid character '" + c + "'");
        } else {
            mCi->StartDtmf(c, NULL);
        }
    }

    //@Override
    CARAPI StopDtmf() {
        mCi->StopDtmf(NULL);
    }

    //@Override
    CARAPI SendBurstDtmf(String dtmfString, Int32 on, Int32 off, Message onComplete) {
        Boolean check = TRUE;
        For (Int32 itr = 0;itr < dtmfString->Length(); itr++) {
            If (!PhoneNumberUtils->Is12Key(dtmfString->CharAt(itr))) {
                Rlog->E(LOG_TAG,
                        "sendDtmf called with invalid character '" + dtmfString->CharAt(itr)+ "'");
                check = FALSE;
                break;
            }
        }
        If ((mCT.mState ==  PhoneConstants.State.OFFHOOK)&&(check)) {
            mCi->SendBurstDtmf(dtmfString, on, off, onComplete);
        }
     }

    //@Override
    CARAPI GetAvailableNetworks(Message response) {
        Rlog->E(LOG_TAG, "getAvailableNetworks: not possible in CDMA");
        If (response != NULL) {
            CommandException ce = new CommandException(
                    CommandException.Error.REQUEST_NOT_SUPPORTED);
            AsyncResult->ForMessage(response).exception = ce;
            response->SendToTarget();
        }
    }

    //@Override
    CARAPI SetOutgoingCallerIdDisplay(Int32 commandInterfaceCLIRMode, Message onComplete) {
        Rlog->E(LOG_TAG, "setOutgoingCallerIdDisplay: not possible in CDMA");
    }

    //@Override
    CARAPI EnableLocationUpdates() {
        mSST->EnableLocationUpdates();
    }

    //@Override
    CARAPI DisableLocationUpdates() {
        mSST->DisableLocationUpdates();
    }

    //@Override
    CARAPI GetDataCallList(Message response) {
        mCi->GetDataCallList(response);
    }

    //@Override
    public Boolean GetDataRoamingEnabled() {
        return mDcTracker->GetDataOnRoamingEnabled();
    }

    //@Override
    CARAPI SetDataEnabled(Boolean enable) {
        mDcTracker->SetDataEnabled(enable);
    }

    //@Override
    public Boolean GetDataEnabled() {
        return mDcTracker->GetDataEnabled();
    }

    //@Override
    CARAPI SetVoiceMailNumber(String alphaTag,
                                   String voiceMailNumber,
                                   Message onComplete) {
        Message resp;
        mVmNumber = voiceMailNumber;
        resp = ObtainMessage(EVENT_SET_VM_NUMBER_DONE, 0, 0, onComplete);
        IccRecords r = mIccRecords->Get();
        If (r != NULL) {
            r->SetVoiceMailNumber(alphaTag, mVmNumber, resp);
        }
    }

    //@Override
    public String GetVoiceMailNumber() {
        String number = NULL;
        SharedPreferences sp = PreferenceManager->GetDefaultSharedPreferences(GetContext());
        number = sp->GetString(VM_NUMBER_CDMA + GetSubId(), NULL);
        If (TextUtils->IsEmpty(number)) {
            String[] listArray = GetContext()->GetResources()
                .GetStringArray(R.array.config_default_vm_number);
            If (listArray != NULL && listArray.length > 0) {
                For (Int32 i=0; i<listArray.length; i++) {
                    If (!TextUtils->IsEmpty(listArray[i])) {
                        String[] defaultVMNumberArray = listArray[i].Split(";");
                        If (defaultVMNumberArray != NULL && defaultVMNumberArray.length > 0) {
                            If (defaultVMNumberArray.length == 1) {
                                number = defaultVMNumberArray[0];
                            } else If (defaultVMNumberArray.length == 2 &&
                                    !TextUtils->IsEmpty(defaultVMNumberArray[1]) &&
                                    defaultVMNumberArray[1].EqualsIgnoreCase(GetGroupIdLevel1())) {
                                number = defaultVMNumberArray[0];
                                break;
                            }
                        }
                    }
                }
            }
        }
        If (TextUtils->IsEmpty(number)) {
            // Read platform settings for dynamic voicemail number
            If (GetContext()->GetResources().GetBoolean(com.android.internal
                    .R.bool.config_telephony_use_own_number_for_voicemail)) {
                number = GetLine1Number();
            } else {
                number = "*86";
            }
        }
        return number;
    }

    // pending voice mail count updated after phone creation
    private void UpdateVoiceMail() {
        SetVoiceMessageCount(GetStoredVoiceMessageCount());
    }

    /** gets the voice mail count from preferences */
    private Int32 GetStoredVoiceMessageCount() {
        SharedPreferences sp = PreferenceManager->GetDefaultSharedPreferences(mContext);
        Return (sp->GetInt(VM_COUNT + GetSubId(), 0));
    }

    //@Override
    public String GetVoiceMailAlphaTag() {
        // TODO: Where can we get this value has to be clarified with QC.
        String ret = "";//TODO: Remove = "", if we know where to get this value.

        //ret = mSIMRecords->GetVoiceMailAlphaTag();

        If (ret == NULL || ret->Length() == 0) {
            return mContext->GetText(
                R::string::defaultVoiceMailAlphaTag).ToString();
        }

        return ret;
    }

    //@Override
    CARAPI GetCallForwardingOption(Int32 commandInterfaceCFReason, Message onComplete) {
        Rlog->E(LOG_TAG, "getCallForwardingOption: not possible in CDMA");
    }

    //@Override
    CARAPI SetCallForwardingOption(Int32 commandInterfaceCFAction,
            Int32 commandInterfaceCFReason,
            String dialingNumber,
            Int32 timerSeconds,
            Message onComplete) {
        Rlog->E(LOG_TAG, "setCallForwardingOption: not possible in CDMA");
    }

    //@Override
    CARAPI GetCallForwardingUncondTimerOption(Int32 commandInterfaceCFReason,
            Message onComplete) {
        ImsPhone imsPhone = mImsPhone;
        If (imsPhone != NULL) {
            imsPhone->GetCallForwardingOption(commandInterfaceCFReason, onComplete);
        } else {
            If (onComplete != NULL) {
                AsyncResult->ForMessage(onComplete, NULL,
                        new CommandException(CommandException.Error.GENERIC_FAILURE));
                onComplete->SendToTarget();
            }
        }
    }

    //@Override
    CARAPI SetCallForwardingUncondTimerOption(Int32 startHour, Int32 startMinute,
            Int32 endHour, Int32 endMinute, Int32 commandInterfaceCFAction,
            Int32 commandInterfaceCFReason, String dialingNumber, Message onComplete) {
        ImsPhone imsPhone = mImsPhone;
        If (imsPhone != NULL) {
            imsPhone->SetCallForwardingUncondTimerOption(startHour, startMinute, endHour,
                    endMinute, commandInterfaceCFAction, commandInterfaceCFReason,
                    dialingNumber, onComplete);
        } else {
            If (onComplete != NULL) {
                AsyncResult->ForMessage(onComplete, NULL,
                        new CommandException(CommandException.Error.GENERIC_FAILURE));
                onComplete->SendToTarget();
           }
        }
    }

    //@Override
    CARAPI
    GetOutgoingCallerIdDisplay(Message onComplete) {
        Rlog->E(LOG_TAG, "getOutgoingCallerIdDisplay: not possible in CDMA");
    }

    //@Override
    public Boolean
    GetCallForwardingIndicator() {
        Rlog->E(LOG_TAG, "getCallForwardingIndicator: not possible in CDMA");
        return FALSE;
    }

    //@Override
    CARAPI ExplicitCallTransfer() {
        Rlog->E(LOG_TAG, "explicitCallTransfer: not possible in CDMA");
    }

    //@Override
    public String GetLine1AlphaTag() {
        Rlog->E(LOG_TAG, "getLine1AlphaTag: not possible in CDMA");
        return NULL;
    }

    /**
     * Notify any interested party of a Phone state change
     * {@link com.android.internal.telephony.PhoneConstants.State}
     */
    /*package*/ void NotifyPhoneStateChanged() {
        mNotifier->NotifyPhoneState(this);
    }

    /**
     * Notify registrants of a change in the call state. This notifies changes in
     * {@link com.android.internal.telephony.Call.State}. Use this when changes
     * in the precise call state are needed, else use notifyPhoneStateChanged.
     */
    /*package*/ void NotifyPreciseCallStateChanged() {
        /* we'd love it if this was package-scoped*/
        super->NotifyPreciseCallStateChangedP();
    }

     void NotifyServiceStateChanged(ServiceState ss) {
         super->NotifyServiceStateChangedP(ss);
     }

     void NotifyLocationChanged() {
         mNotifier->NotifyCellLocation(this);
     }

    CARAPI NotifyNewRingingConnection(Connection c) {
        super->NotifyNewRingingConnectionP(c);
    }

    /*package*/ void NotifyDisconnect(Connection cn) {
        mDisconnectRegistrants->NotifyResult(cn);

        mNotifier->NotifyDisconnectCause(cn->GetDisconnectCause(), cn->GetPreciseDisconnectCause());
    }

    void NotifyUnknownConnection(Connection connection) {
        mUnknownConnectionRegistrants->NotifyResult(connection);
    }

    //@Override
    public Boolean IsInEmergencyCall() {
        return mCT->IsInEmergencyCall();
    }

    //@Override
    public Boolean IsInEcm() {
        return mIsPhoneInEcmState;
    }

    void SendEmergencyCallbackModeChange(){
        //Send an Intent
        Intent intent = new Intent(TelephonyIntents.ACTION_EMERGENCY_CALLBACK_MODE_CHANGED);
        intent->PutExtra(PhoneConstants.PHONE_IN_ECM_STATE, mIsPhoneInEcmState);
        SubscriptionManager->PutPhoneIdAndSubIdExtra(intent, GetPhoneId());
        ActivityManagerNative->BroadcastStickyIntent(intent,NULL,UserHandle.USER_ALL);
        If (DBG) Rlog->D(LOG_TAG, "sendEmergencyCallbackModeChange");
    }

    //@Override
    CARAPI ExitEmergencyCallbackMode() {
        If (mWakeLock->IsHeld()) {
            mWakeLock->Release();
        }
        // Send a message which will invoke handleExitEmergencyCallbackMode
        mCi->ExitEmergencyCallbackMode(ObtainMessage(EVENT_EXIT_EMERGENCY_CALLBACK_RESPONSE));
    }

    private void HandleEnterEmergencyCallbackMode(Message msg) {
        If (DBG) {
            Rlog->D(LOG_TAG, "handleEnterEmergencyCallbackMode,mIsPhoneInEcmState= "
                    + mIsPhoneInEcmState);
        }
        // if phone is not in Ecm mode, and it's changed to Ecm mode
        If (mIsPhoneInEcmState == FALSE) {
            mIsPhoneInEcmState = TRUE;
            // notify change
            SendEmergencyCallbackModeChange();
            super->SetSystemProperty(TelephonyProperties.PROPERTY_INECM_MODE, "TRUE");

            // Post this runnable so we will automatically exit
            // if no one invokes ExitEmergencyCallbackMode() directly.
            Int64 delayInMillis = SystemProperties->GetLong(
                    TelephonyProperties.PROPERTY_ECM_EXIT_TIMER, DEFAULT_ECM_EXIT_TIMER_VALUE);
            PostDelayed(mExitEcmRunnable, delayInMillis);
            // We don't want to go to sleep while in Ecm
            mWakeLock->Acquire();
        }
    }

    private void HandleExitEmergencyCallbackMode(Message msg) {
        AsyncResult ar = (AsyncResult)msg.obj;
        If (DBG) {
            Rlog->D(LOG_TAG, "handleExitEmergencyCallbackMode,ar.exception , mIsPhoneInEcmState "
                    + ar.exception + mIsPhoneInEcmState);
        }
        // Remove pending exit Ecm runnable, if any
        RemoveCallbacks(mExitEcmRunnable);

        If (mEcmExitRespRegistrant != NULL) {
            mEcmExitRespRegistrant->NotifyRegistrant(ar);
        }
        // if exiting ecm success
        If (ar.exception == NULL) {
            If (mIsPhoneInEcmState) {
                mIsPhoneInEcmState = FALSE;
                super->SetSystemProperty(TelephonyProperties.PROPERTY_INECM_MODE, "FALSE");
            }
            // send an Intent
            SendEmergencyCallbackModeChange();
            // Re-initiate data connection
            mDcTracker->SetInternalDataEnabled(TRUE);
        }
    }

    /**
     * Handle to cancel or restart Ecm timer in emergency call back mode
     * if action is CANCEL_ECM_TIMER, cancel Ecm timer and notify apps the timer is canceled;
     * otherwise, restart Ecm timer and notify apps the timer is restarted.
     */
    void HandleTimerInEmergencyCallbackMode(Int32 action) {
        Switch(action) {
        case CANCEL_ECM_TIMER:
            RemoveCallbacks(mExitEcmRunnable);
            mEcmTimerResetRegistrants->NotifyResult(Boolean.TRUE);
            break;
        case RESTART_ECM_TIMER:
            Int64 delayInMillis = SystemProperties->GetLong(
                    TelephonyProperties.PROPERTY_ECM_EXIT_TIMER, DEFAULT_ECM_EXIT_TIMER_VALUE);
            PostDelayed(mExitEcmRunnable, delayInMillis);
            mEcmTimerResetRegistrants->NotifyResult(Boolean.FALSE);
            break;
        default:
            Rlog->E(LOG_TAG, "handleTimerInEmergencyCallbackMode, unsupported action " + action);
        }
    }

    CARAPI NotifyEcbmTimerReset(Boolean flag) {
        mEcmTimerResetRegistrants->NotifyResult(flag);
    }

    /**
     * Registration point for Ecm timer reset
     * @param h handler to notify
     * @param what User-defined message code
     * @param obj placed in Message.obj
     */
    //@Override
    CARAPI RegisterForEcmTimerReset(Handler h, Int32 what, Object obj) {
        mEcmTimerResetRegistrants->AddUnique(h, what, obj);
    }

    //@Override
    CARAPI UnregisterForEcmTimerReset(Handler h) {
        mEcmTimerResetRegistrants->Remove(h);
    }

    //@Override
    CARAPI HandleMessage(Message msg) {
        AsyncResult ar;
        Message     onComplete;

        If (!mIsTheCurrentActivePhone) {
            Rlog->E(LOG_TAG, "Received message " + msg +
                    "[" + msg.what + "] while being destroyed. Ignoring.");
            return;
        }
        Switch(msg.what) {
            case EVENT_RADIO_AVAILABLE: {
                mCi->GetBasebandVersion(ObtainMessage(EVENT_GET_BASEBAND_VERSION_DONE));

                mCi->GetDeviceIdentity(ObtainMessage(EVENT_GET_DEVICE_IDENTITY_DONE));
            }
            break;

            case EVENT_GET_BASEBAND_VERSION_DONE:{
                ar = (AsyncResult)msg.obj;

                If (ar.exception != NULL) {
                    break;
                }

                If (DBG) Rlog->D(LOG_TAG, "Baseband version: " + ar.result);
                If (!"".Equals((String)ar.result)) {
                    SetSystemProperty(TelephonyProperties.PROPERTY_BASEBAND_VERSION,
                                      (String)ar.result);
                }
            }
            break;

            case EVENT_GET_DEVICE_IDENTITY_DONE:{
                ar = (AsyncResult)msg.obj;

                If (ar.exception != NULL) {
                    break;
                }
                String[] respId = (String[])ar.result;
                mImei = respId[0];
                mImeiSv = respId[1];
                mEsn  =  respId[2];
                mMeid =  respId[3];
            }
            break;

            case EVENT_EMERGENCY_CALLBACK_MODE_ENTER:{
                HandleEnterEmergencyCallbackMode(msg);
            }
            break;

            case  EVENT_EXIT_EMERGENCY_CALLBACK_RESPONSE:{
                HandleExitEmergencyCallbackMode(msg);
            }
            break;

            case EVENT_RUIM_RECORDS_LOADED:{
                Rlog->D(LOG_TAG, "Event EVENT_RUIM_RECORDS_LOADED Received");
                UpdateCurrentCarrierInProvider();
                // Notify voicemails.
                Log("notifyMessageWaitingChanged");
                mNotifier->NotifyMessageWaitingChanged(this);
                UpdateVoiceMail();
            }
            break;

            case EVENT_RADIO_OFF_OR_NOT_AVAILABLE:{
                Rlog->D(LOG_TAG, "Event EVENT_RADIO_OFF_OR_NOT_AVAILABLE Received");
            }
            break;

            case EVENT_RADIO_ON:{
                Rlog->D(LOG_TAG, "Event EVENT_RADIO_ON Received");
                HandleCdmaSubscriptionSource(mCdmaSSM->GetCdmaSubscriptionSource());
            }
            break;

            case EVENT_CDMA_SUBSCRIPTION_SOURCE_CHANGED:{
                Rlog->D(LOG_TAG, "EVENT_CDMA_SUBSCRIPTION_SOURCE_CHANGED");
                HandleCdmaSubscriptionSource(mCdmaSSM->GetCdmaSubscriptionSource());
            }
            break;

            case EVENT_SSN:{
                Rlog->D(LOG_TAG, "Event EVENT_SSN Received");
            }
            break;

            case EVENT_REGISTERED_TO_NETWORK:{
                Rlog->D(LOG_TAG, "Event EVENT_REGISTERED_TO_NETWORK Received");
            }
            break;

            case EVENT_NV_READY:{
                Rlog->D(LOG_TAG, "Event EVENT_NV_READY Received");
                PrepareEri();
                // Notify voicemails.
                Log("notifyMessageWaitingChanged");
                mNotifier->NotifyMessageWaitingChanged(this);
                UpdateVoiceMail();
                SubInfoRecordUpdater subInfoRecordUpdater = PhoneFactory->GetSubInfoRecordUpdater();
                If (subInfoRecordUpdater != NULL) {
                    subInfoRecordUpdater.UpdateSubIdForNV (mPhoneId);
                }
            }
            break;

            case EVENT_SET_VM_NUMBER_DONE:{
                ar = (AsyncResult)msg.obj;
                If (IccException.class->IsInstance(ar.exception)) {
                    StoreVoiceMailNumber(mVmNumber);
                    ar.exception = NULL;
                }
                onComplete = (Message) ar.userObj;
                If (onComplete != NULL) {
                    AsyncResult->ForMessage(onComplete, ar.result, ar.exception);
                    onComplete->SendToTarget();
                }
            }
            break;

            default:{
                super->HandleMessage(msg);
            }
        }
    }

    protected UiccCardApplication GetUiccCardApplication() {
        return  mUiccController->GetUiccCardApplication(mPhoneId, UiccController.APP_FAM_3GPP2);
    }

    // Set the Card into the Phone Book.
    //@Override
    protected void SetCardInPhoneBook() {
        If (mUiccController == NULL ) {
            return;
        }

        mRuimPhoneBookInterfaceManager->SetIccCard(mUiccController->GetUiccCard(mPhoneId));
    }

    //@Override
    protected void OnUpdateIccAvailability() {
        If (mUiccController == NULL ) {
            return;
        }

        // Get the latest info on the card and
        // send this to Phone Book
        SetCardInPhoneBook();

        UiccCardApplication newUiccApplication = GetUiccCardApplication();

        If (newUiccApplication == NULL) {
            Log("can't find 3GPP2 application; trying APP_FAM_3GPP");
            newUiccApplication =
                    mUiccController->GetUiccCardApplication(mPhoneId, UiccController.APP_FAM_3GPP);
        }

        UiccCardApplication app = mUiccApplication->Get();
        If (app != newUiccApplication) {
            If (app != NULL) {
                Log("Removing stale icc objects.");
                If (mIccRecords->Get() != NULL) {
                    UnregisterForRuimRecordEvents();
                }
                mIccRecords->Set(NULL);
                mUiccApplication->Set(NULL);
            }
            If (newUiccApplication != NULL) {
                Log("New Uicc application found");
                mUiccApplication->Set(newUiccApplication);
                mIccRecords->Set(newUiccApplication->GetIccRecords());
                RegisterForRuimRecordEvents();
            }
        }
    }

    /**
     * Handles the call to get the subscription source
     *
     * @param newSubscriptionSource holds the new CDMA subscription source value
     */
    private void HandleCdmaSubscriptionSource(Int32 newSubscriptionSource) {
        If (newSubscriptionSource != mCdmaSubscriptionSource) {
             mCdmaSubscriptionSource = newSubscriptionSource;
             If (newSubscriptionSource == CDMA_SUBSCRIPTION_NV) {
                 // NV is ready when subscription source is NV
                 SendMessage(ObtainMessage(EVENT_NV_READY));
             }
        }
    }

    /**
     * Retrieves the PhoneSubInfo of the CDMAPhone
     */
    //@Override
    public PhoneSubInfo GetPhoneSubInfo() {
        return mSubInfo;
    }

    /**
     * Retrieves the IccPhoneBookInterfaceManager of the CDMAPhone
     */
    //@Override
    public IccPhoneBookInterfaceManager GetIccPhoneBookInterfaceManager() {
        return mRuimPhoneBookInterfaceManager;
    }

    CARAPI RegisterForEriFileLoaded(Handler h, Int32 what, Object obj) {
        Registrant r = new Registrant (h, what, obj);
        mEriFileLoadedRegistrants->Add(r);
    }

    CARAPI UnregisterForEriFileLoaded(Handler h) {
        mEriFileLoadedRegistrants->Remove(h);
    }

    // override for allowing access from other classes of this package
    /**
     * {@inheritDoc}
     */
    //@Override
    CARAPI SetSystemProperty(String property, String value) {
        super->SetSystemProperty(property, value);
    }

    // override for allowing access from other classes of this package
    /**
     * {@inheritDoc}
     */
    //@Override
    public String GetSystemProperty(String property, String defValue) {
        return super->GetSystemProperty(property, defValue);
    }

    /**
     * Activate or deactivate cell broadcast SMS.
     *
     * @param activate 0 = activate, 1 = deactivate
     * @param response Callback message is empty on completion
     */
    //@Override
    CARAPI ActivateCellBroadcastSms(Int32 activate, Message response) {
        Rlog->E(LOG_TAG, "[CDMAPhone] ActivateCellBroadcastSms() is obsolete; use SmsManager");
        response->SendToTarget();
    }

    /**
     * Query the current configuration of cdma cell broadcast SMS.
     *
     * @param response Callback message is empty on completion
     */
    //@Override
    CARAPI GetCellBroadcastSmsConfig(Message response) {
        Rlog->E(LOG_TAG, "[CDMAPhone] GetCellBroadcastSmsConfig() is obsolete; use SmsManager");
        response->SendToTarget();
    }

    /**
     * Configure cdma cell broadcast SMS.
     *
     * @param response Callback message is empty on completion
     */
    //@Override
    CARAPI SetCellBroadcastSmsConfig(Int32[] configValuesArray, Message response) {
        Rlog->E(LOG_TAG, "[CDMAPhone] SetCellBroadcastSmsConfig() is obsolete; use SmsManager");
        response->SendToTarget();
    }

    /**
     * Returns TRUE if OTA Service Provisioning needs to be performed.
     */
    //@Override
    public Boolean NeedsOtaServiceProvisioning() {
        return mSST->GetOtasp() != ServiceStateTracker.OTASP_NOT_NEEDED;
    }

    private static const String IS683A_FEATURE_CODE = "*228";
    private static const Int32 IS683A_FEATURE_CODE_NUM_DIGITS = 4;
    private static const Int32 IS683A_SYS_SEL_CODE_NUM_DIGITS = 2;
    private static const Int32 IS683A_SYS_SEL_CODE_OFFSET = 4;

    private static const Int32 IS683_CONST_800MHZ_A_BAND = 0;
    private static const Int32 IS683_CONST_800MHZ_B_BAND = 1;
    private static const Int32 IS683_CONST_1900MHZ_A_BLOCK = 2;
    private static const Int32 IS683_CONST_1900MHZ_B_BLOCK = 3;
    private static const Int32 IS683_CONST_1900MHZ_C_BLOCK = 4;
    private static const Int32 IS683_CONST_1900MHZ_D_BLOCK = 5;
    private static const Int32 IS683_CONST_1900MHZ_E_BLOCK = 6;
    private static const Int32 IS683_CONST_1900MHZ_F_BLOCK = 7;
    private static const Int32 INVALID_SYSTEM_SELECTION_CODE = -1;

    private static Boolean IsIs683OtaSpDialStr(String dialStr) {
        Int32 sysSelCodeInt;
        Boolean isOtaspDialString = FALSE;
        Int32 dialStrLen = dialStr->Length();

        If (dialStrLen == IS683A_FEATURE_CODE_NUM_DIGITS) {
            If (dialStr->Equals(IS683A_FEATURE_CODE)) {
                isOtaspDialString = TRUE;
            }
        } else {
            sysSelCodeInt = ExtractSelCodeFromOtaSpNum(dialStr);
            Switch (sysSelCodeInt) {
                case IS683_CONST_800MHZ_A_BAND:
                case IS683_CONST_800MHZ_B_BAND:
                case IS683_CONST_1900MHZ_A_BLOCK:
                case IS683_CONST_1900MHZ_B_BLOCK:
                case IS683_CONST_1900MHZ_C_BLOCK:
                case IS683_CONST_1900MHZ_D_BLOCK:
                case IS683_CONST_1900MHZ_E_BLOCK:
                case IS683_CONST_1900MHZ_F_BLOCK:
                    isOtaspDialString = TRUE;
                    break;
                default:
                    break;
            }
        }
        return isOtaspDialString;
    }
    /**
     * This function extracts the system selection code from the dial string.
     */
    private static Int32 ExtractSelCodeFromOtaSpNum(String dialStr) {
        Int32 dialStrLen = dialStr->Length();
        Int32 sysSelCodeInt = INVALID_SYSTEM_SELECTION_CODE;

        If ((dialStr->RegionMatches(0, IS683A_FEATURE_CODE,
                                   0, IS683A_FEATURE_CODE_NUM_DIGITS)) &&
            (dialStrLen >= (IS683A_FEATURE_CODE_NUM_DIGITS +
                            IS683A_SYS_SEL_CODE_NUM_DIGITS))) {
                // Since we checked the condition above, the system selection code
                // extracted from dialStr will not cause any exception
                sysSelCodeInt = Integer.ParseInt (
                                dialStr.Substring (IS683A_FEATURE_CODE_NUM_DIGITS,
                                IS683A_FEATURE_CODE_NUM_DIGITS + IS683A_SYS_SEL_CODE_NUM_DIGITS));
        }
        If (DBG) Rlog->D(LOG_TAG, "extractSelCodeFromOtaSpNum " + sysSelCodeInt);
        return sysSelCodeInt;
    }

    /**
     * This function checks if the system selection code extracted from
     * the dial string "sysSelCodeInt' is the system selection code specified
     * in the carrier ota sp number schema "sch".
     */
    private static Boolean
    CheckOtaSpNumBasedOnSysSelCode (Int32 sysSelCodeInt, String sch[]) {
        Boolean isOtaSpNum = FALSE;
        try {
            // Get how many number of system selection code ranges
            Int32 selRc = Integer->ParseInt(sch[1]);
            For (Int32 i = 0; i < selRc; i++) {
                If (!TextUtils->IsEmpty(sch[i+2]) && !TextUtils->IsEmpty(sch[i+3])) {
                    Int32 selMin = Integer->ParseInt(sch[i+2]);
                    Int32 selMax = Integer->ParseInt(sch[i+3]);
                    // Check if the selection code extracted from the dial string falls
                    // within any of the range pairs specified in the schema.
                    If ((sysSelCodeInt >= selMin) && (sysSelCodeInt <= selMax)) {
                        isOtaSpNum = TRUE;
                        break;
                    }
                }
            }
        } Catch (NumberFormatException ex) {
            // If the carrier ota sp number schema is not correct, we still allow dial
            // and only log the error:
            Rlog->E(LOG_TAG, "checkOtaSpNumBasedOnSysSelCode, error", ex);
        }
        return isOtaSpNum;
    }

    // Define the pattern/format for carrier specified OTASP number schema.
    // It separates by comma and/or whitespace.
    private static Pattern pOtaSpNumSchema = Pattern->Compile("[,\\s]+");

    /**
     * The following function checks if a dial string is a carrier specified
     * OTASP number or not by checking against the OTASP number schema stored
     * in PROPERTY_OTASP_NUM_SCHEMA.
     *
     * Currently, there are 2 schemas for carriers to specify the OTASP number:
     * 1) Use system selection code:
     *    The schema is:
     *    SELC,the # of code pairs,min1,max1,min2,max2,...
     *    e.g "SELC,3,10,20,30,40,60,70" indicates that there are 3 pairs of
     *    selection codes, and they are {10,20}, {30,40} and {60,70} respectively.
     *
     * 2) Use feature code:
     *    The schema is:
     *    "FC,length of feature code,feature code".
     *     e.g "FC,2,*2" indicates that the length of the feature code is 2,
     *     and the code itself is "*2".
     */
    private Boolean IsCarrierOtaSpNum(String dialStr) {
        Boolean isOtaSpNum = FALSE;
        Int32 sysSelCodeInt = ExtractSelCodeFromOtaSpNum(dialStr);
        If (sysSelCodeInt == INVALID_SYSTEM_SELECTION_CODE) {
            return isOtaSpNum;
        }
        // mCarrierOtaSpNumSchema is retrieved from PROPERTY_OTASP_NUM_SCHEMA:
        If (!TextUtils->IsEmpty(mCarrierOtaSpNumSchema)) {
            Matcher m = pOtaSpNumSchema->Matcher(mCarrierOtaSpNumSchema);
            If (DBG) {
                Rlog->D(LOG_TAG, "isCarrierOtaSpNum,schema" + mCarrierOtaSpNumSchema);
            }

            If (m->Find()) {
                String sch[] = pOtaSpNumSchema->Split(mCarrierOtaSpNumSchema);
                // If carrier uses system selection code mechanism
                If (!TextUtils->IsEmpty(sch[0]) && sch[0].Equals("SELC")) {
                    If (sysSelCodeInt!=INVALID_SYSTEM_SELECTION_CODE) {
                        isOtaSpNum=CheckOtaSpNumBasedOnSysSelCode(sysSelCodeInt,sch);
                    } else {
                        If (DBG) {
                            Rlog->D(LOG_TAG, "isCarrierOtaSpNum,sysSelCodeInt is invalid");
                        }
                    }
                } else If (!TextUtils->IsEmpty(sch[0]) && sch[0].Equals("FC")) {
                    Int32 fcLen =  Integer->ParseInt(sch[1]);
                    String fc = sch[2];
                    If (dialStr->RegionMatches(0,fc,0,fcLen)) {
                        isOtaSpNum = TRUE;
                    } else {
                        If (DBG) Rlog->D(LOG_TAG, "isCarrierOtaSpNum,not otasp number");
                    }
                } else {
                    If (DBG) {
                        Rlog->D(LOG_TAG, "isCarrierOtaSpNum,ota schema not supported" + sch[0]);
                    }
                }
            } else {
                If (DBG) {
                    Rlog->D(LOG_TAG, "isCarrierOtaSpNum,ota schema pattern not right" +
                          mCarrierOtaSpNumSchema);
                }
            }
        } else {
            If (DBG) Rlog->D(LOG_TAG, "isCarrierOtaSpNum,ota schema pattern empty");
        }
        return isOtaSpNum;
    }

    /**
     * isOTASPNumber: checks a given number against the IS-683A OTASP dial string and carrier
     * OTASP dial string.
     *
     * @param dialStr the number to look up.
     * @return TRUE if the number is in IS-683A OTASP dial string or carrier OTASP dial string
     */
    //@Override
    public  Boolean IsOtaSpNumber(String dialStr){
        Boolean isOtaSpNum = FALSE;
        String dialableStr = PhoneNumberUtils->ExtractNetworkPortionAlt(dialStr);
        If (dialableStr != NULL) {
            isOtaSpNum = IsIs683OtaSpDialStr(dialableStr);
            If (isOtaSpNum == FALSE) {
                isOtaSpNum = IsCarrierOtaSpNum(dialableStr);
            }
        }
        If (DBG) Rlog->D(LOG_TAG, "isOtaSpNumber " + isOtaSpNum);
        return isOtaSpNum;
    }

    //@Override
    public Int32 GetCdmaEriIconIndex() {
        return GetServiceState()->GetCdmaEriIconIndex();
    }

    /**
     * Returns the CDMA ERI icon mode,
     * 0 - ON
     * 1 - FLASHING
     */
    //@Override
    public Int32 GetCdmaEriIconMode() {
        return GetServiceState()->GetCdmaEriIconMode();
    }

    /**
     * Returns the CDMA ERI text,
     */
    //@Override
    public String GetCdmaEriText() {
        Int32 roamInd = GetServiceState()->GetCdmaRoamingIndicator();
        Int32 defRoamInd = GetServiceState()->GetCdmaDefaultRoamingIndicator();
        return mEriManager->GetCdmaEriText(roamInd, defRoamInd);
    }

    /**
     * Store the voicemail number in preferences
     */
    private void StoreVoiceMailNumber(String number) {
        // Update the preference value of voicemail number
        SharedPreferences sp = PreferenceManager->GetDefaultSharedPreferences(GetContext());
        SharedPreferences.Editor editor = sp->Edit();
        editor->PutString(VM_NUMBER_CDMA + GetSubId(), number);
        editor->Apply();
    }

    /**
     * Sets PROPERTY_ICC_OPERATOR_ISO_COUNTRY property
     *
     */
    protected void SetIsoCountryProperty(String operatorNumeric) {
        If (TextUtils->IsEmpty(operatorNumeric)) {
            Log("setIsoCountryProperty: clear 'gsm.sim.operator.iso-country'");
            SetSystemProperty(PROPERTY_ICC_OPERATOR_ISO_COUNTRY, "");
        } else {
            String iso = "";
            try {
                iso = MccTable->CountryCodeForMcc(Integer->ParseInt(
                        operatorNumeric->Substring(0,3)));
            } Catch (NumberFormatException ex) {
                Loge("setIsoCountryProperty: countryCodeForMcc error", ex);
            } Catch (StringIndexOutOfBoundsException ex) {
                Loge("setIsoCountryProperty: countryCodeForMcc error", ex);
            }

            Log("setIsoCountryProperty: set 'gsm.sim.operator.iso-country' to iso=" + iso);
            SetSystemProperty(PROPERTY_ICC_OPERATOR_ISO_COUNTRY, iso);
        }
    }

    /**
     * Sets the "current" field in the telephony provider according to the
     * build-time operator numeric property
     *
     * @return TRUE for success; FALSE otherwise.
     */
    Boolean UpdateCurrentCarrierInProvider(String operatorNumeric) {
        Log("CDMAPhone: updateCurrentCarrierInProvider called");
        If (!TextUtils->IsEmpty(operatorNumeric)) {
            try {
                Uri uri = Uri->WithAppendedPath(Telephony.Carriers.CONTENT_URI, "current");
                ContentValues map = new ContentValues();
                map->Put(Telephony.Carriers.NUMERIC, operatorNumeric);
                Log("updateCurrentCarrierInProvider from system: numeric=" + operatorNumeric);
                GetContext()->GetContentResolver().Insert(uri, map);

                // Updates MCC MNC device configuration information
                Log("update mccmnc=" + operatorNumeric);
                MccTable->UpdateMccMncConfiguration(mContext, operatorNumeric, FALSE);

                return TRUE;
            } Catch (SQLException e) {
                Rlog->E(LOG_TAG, "Can't store current operator", e);
            }
        }
        return FALSE;
    }

    /**
     * Sets the "current" field in the telephony provider according to the SIM's operator.
     * Implemented in {@link CDMALTEPhone} for CDMA/LTE devices.
     *
     * @return TRUE for success; FALSE otherwise.
     */
    Boolean UpdateCurrentCarrierInProvider() {
        return TRUE;
    }

    CARAPI PrepareEri() {
        If (mEriManager == NULL) {
            Rlog->E(LOG_TAG, "PrepareEri: Trying to access stale objects");
            return;
        }
        mEriManager->LoadEriFile();
        If(mEriManager->IsEriFileLoaded()) {
            // when the ERI file is loaded
            Log("ERI read, notify registrants");
            mEriFileLoadedRegistrants->NotifyRegistrants();
        }
    }

    public Boolean IsEriFileLoaded() {
        return mEriManager->IsEriFileLoaded();
    }

    protected void RegisterForRuimRecordEvents() {
        IccRecords r = mIccRecords->Get();
        If (r == NULL) {
            return;
        }
        r->RegisterForRecordsLoaded(this, EVENT_RUIM_RECORDS_LOADED, NULL);
    }

    protected void UnregisterForRuimRecordEvents() {
        IccRecords r = mIccRecords->Get();
        If (r == NULL) {
            return;
        }
        r->UnregisterForRecordsLoaded(this);
    }

     /**
     * Sets the SIM voice message count
     * @param line Subscriber Profile Number, one-based. Only '1' is supported
     * @param countWaiting The number of messages waiting, if known. Use
     *                     -1 to indicate that an unknown number of
     *                      messages are waiting
     * This is a wrapper function for setVoiceMessageCount
     */
    //@Override
    CARAPI SetVoiceMessageWaiting(Int32 line, Int32 countWaiting) {
        SetVoiceMessageCount(countWaiting);
    }

    protected void Log(String s) {
        If (DBG)
            Rlog->D(LOG_TAG, s);
    }

    protected void Loge(String s, Exception e) {
        If (DBG)
            Rlog->E(LOG_TAG, s, e);
    }

    //@Override
    CARAPI Dump(FileDescriptor fd, PrintWriter pw, String[] args) {
        pw->Println("CDMAPhone extends:");
        super->Dump(fd, pw, args);
        pw->Println(" mVmNumber=" + mVmNumber);
        pw->Println(" mCT=" + mCT);
        pw->Println(" mSST=" + mSST);
        pw->Println(" mCdmaSSM=" + mCdmaSSM);
        pw->Println(" mPendingMmis=" + mPendingMmis);
        pw->Println(" mRuimPhoneBookInterfaceManager=" + mRuimPhoneBookInterfaceManager);
        pw->Println(" mCdmaSubscriptionSource=" + mCdmaSubscriptionSource);
        pw->Println(" mSubInfo=" + mSubInfo);
        pw->Println(" mEriManager=" + mEriManager);
        pw->Println(" mWakeLock=" + mWakeLock);
        pw->Println(" mIsPhoneInEcmState=" + mIsPhoneInEcmState);
        If (VDBG) pw->Println(" mImei=" + mImei);
        If (VDBG) pw->Println(" mImeiSv=" + mImeiSv);
        If (VDBG) pw->Println(" mEsn=" + mEsn);
        If (VDBG) pw->Println(" mMeid=" + mMeid);
        pw->Println(" mCarrierOtaSpNumSchema=" + mCarrierOtaSpNumSchema);
        pw->Println(" GetCdmaEriIconIndex()=" + GetCdmaEriIconIndex());
        pw->Println(" GetCdmaEriIconMode()=" + GetCdmaEriIconMode());
        pw->Println(" GetCdmaEriText()=" + GetCdmaEriText());
        pw->Println(" IsMinInfoReady()=" + IsMinInfoReady());
        pw->Println(" IsCspPlmnEnabled()=" + IsCspPlmnEnabled());
    }

    //@Override
    public Boolean SetOperatorBrandOverride(String brand) {
        If (mUiccController == NULL) {
            return FALSE;
        }

        UiccCard card = mUiccController->GetUiccCard();
        If (card == NULL) {
            return FALSE;
        }

        Boolean status = card->SetOperatorBrandOverride(brand);

        // Refresh.
        If (status) {
            IccRecords iccRecords = mIccRecords->Get();
            If (iccRecords != NULL) {
                SystemProperties->Set(TelephonyProperties.PROPERTY_ICC_OPERATOR_ALPHA,
                        iccRecords->GetServiceProviderName());
            }
            If (mSST != NULL) {
                mSST->PollState();
            }
        }
        return status;
    }

}
