/*
 * Copyright (c) 2012-2014, The Linux Foundation. All rights reserved.
 * Not a Contribution.
 *
 * Copyright (C) 2007 The Android Open Source Project
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

using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Net::ILinkProperties;
using Elastos::Droid::Net::INetworkCapabilities;
using Elastos::Droid::Net::Wifi::IWifiManager;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IBuild;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IRegistrant;
using Elastos::Droid::Os::IRegistrantList;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Preference::IPreferenceManager;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Telecom::IVideoProfile;
using Elastos::Droid::Telephony::ICellIdentityCdma;
using Elastos::Droid::Telephony::ICellInfo;
using Elastos::Droid::Telephony::ICellInfoCdma;
using Elastos::Droid::Telephony::IDataConnectionRealTimeInfo;
using Elastos::Droid::Telephony::IVoLteServiceState;
using Elastos::Droid::Telephony::IRlog;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Droid::Telephony::ISignalStrength;
using Elastos::Droid::Telephony::ISubscriptionManager;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Text::ITextUtils;

using Elastos::Droid::Ims::IImsManager;
using Elastos::Droid::Internal::IR;
using Elastos::Droid::Internal::Telephony::Dataconnection::IDcTrackerBase;
using Elastos::Droid::Internal::Telephony::Imsphone::IImsPhone;
using Elastos::Droid::Internal::Telephony::Imsphone::IImsPhoneConnection;
using Elastos::Droid::Internal::Telephony::Test::ISimulatedRadioControl;
using Elastos::Droid::Internal::Telephony::Uicc::IccCardApplicationStatus::IAppType;
using Elastos::Droid::Internal::Telephony::Uicc::IIccFileHandler;
using Elastos::Droid::Internal::Telephony::Uicc::IIccRecords;
using Elastos::Droid::Internal::Telephony::Uicc::IIsimRecords;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccCard;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccCardApplication;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccController;
using Elastos::Droid::Internal::Telephony::Uicc::IUsimServiceTable;

using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;
using Elastos::Utility::ILocale;
using Elastos::Utility::Concurrent::Atomic::IAtomicReference;

/**
 * (<em>Not for SDK use</em>)
 * A base implementation for the com.android.internal.telephony.Phone interface.
 *
 * Note that implementations of Phone.java are expected to be used
 * from a single application thread. This should be the same thread that
 * originally called PhoneFactory to obtain the interface.
 *
 *  {@hide}
 *
 */

public abstract class PhoneBase extends Handler implements Phone {
    private static const String LOG_TAG = "PhoneBase";

    private BroadcastReceiver mImsIntentReceiver = new BroadcastReceiver() {
        //@Override
        CARAPI OnReceive(Context context, Intent intent) {
            // TODO: MSIM potentially replace SUBID with information available to external
            // service, e.g. slot ID
            If (intent->HasExtra(ImsManager.EXTRA_SUBID)) {
                If (intent->GetLongExtra(ImsManager.EXTRA_SUBID, -1) != GetSubId())
                    return;
            }

            // For MSIM, check phone ID
            If (intent->HasExtra(ImsManager.EXTRA_PHONEID)) {
                If (intent->GetIntExtra(ImsManager.EXTRA_PHONEID, -1) != GetPhoneId())
                    return;
            }

            If (intent->GetAction()->Equals(ImsManager.ACTION_IMS_SERVICE_UP)) {
                mImsServiceReady = TRUE;
                UpdateImsPhone();
            } else If (intent->GetAction()->Equals(ImsManager.ACTION_IMS_SERVICE_DOWN)) {
                mImsServiceReady = FALSE;
                UpdateImsPhone();
            }
        }
    };
    /**
     * Indicates whether Out Of Service is considered as data call disconnect.
     */
    public static const String PROPERTY_OOS_IS_DISCONNECT = "persist.telephony.oosisdc";

    // Key used to read and write the saved network selection numeric value
    public static const String NETWORK_SELECTION_KEY = "network_selection_key";
    // Key used to read and write the saved network selection operator name
    public static const String NETWORK_SELECTION_NAME_KEY = "network_selection_name_key";


    // Key used to read/write "disable data connection on boot" Pref (used for testing)
    public static const String DATA_DISABLED_ON_BOOT_KEY = "disabled_on_boot_key";

    /* Event Constants */
    protected static const Int32 EVENT_RADIO_AVAILABLE             = 1;
    /** Supplementary Service Notification received. */
    protected static const Int32 EVENT_SSN                         = 2;
    protected static const Int32 EVENT_SIM_RECORDS_LOADED          = 3;
    protected static const Int32 EVENT_MMI_DONE                    = 4;
    protected static const Int32 EVENT_RADIO_ON                    = 5;
    protected static const Int32 EVENT_GET_BASEBAND_VERSION_DONE   = 6;
    protected static const Int32 EVENT_USSD                        = 7;
    protected static const Int32 EVENT_RADIO_OFF_OR_NOT_AVAILABLE  = 8;
    protected static const Int32 EVENT_GET_IMEI_DONE               = 9;
    protected static const Int32 EVENT_GET_IMEISV_DONE             = 10;
    protected static const Int32 EVENT_GET_SIM_STATUS_DONE         = 11;
    protected static const Int32 EVENT_SET_CALL_FORWARD_DONE       = 12;
    protected static const Int32 EVENT_GET_CALL_FORWARD_DONE       = 13;
    protected static const Int32 EVENT_CALL_RING                   = 14;
    protected static const Int32 EVENT_CALL_RING_CONTINUE          = 15;

    // Used to intercept the carrier selection calls so that
    // we can save the values.
    protected static const Int32 EVENT_SET_NETWORK_MANUAL_COMPLETE    = 16;
    protected static const Int32 EVENT_SET_NETWORK_AUTOMATIC_COMPLETE = 17;
    protected static const Int32 EVENT_SET_CLIR_COMPLETE              = 18;
    protected static const Int32 EVENT_REGISTERED_TO_NETWORK          = 19;
    protected static const Int32 EVENT_SET_VM_NUMBER_DONE             = 20;
    // Events for CDMA support
    protected static const Int32 EVENT_GET_DEVICE_IDENTITY_DONE       = 21;
    protected static const Int32 EVENT_RUIM_RECORDS_LOADED            = 22;
    protected static const Int32 EVENT_NV_READY                       = 23;
    protected static const Int32 EVENT_SET_ENHANCED_VP                = 24;
    protected static const Int32 EVENT_EMERGENCY_CALLBACK_MODE_ENTER  = 25;
    protected static const Int32 EVENT_EXIT_EMERGENCY_CALLBACK_RESPONSE = 26;
    protected static const Int32 EVENT_CDMA_SUBSCRIPTION_SOURCE_CHANGED = 27;
    // other
    protected static const Int32 EVENT_SET_NETWORK_AUTOMATIC          = 28;
    protected static const Int32 EVENT_ICC_RECORD_EVENTS              = 29;
    protected static const Int32 EVENT_ICC_CHANGED                    = 30;
    // Single Radio Voice Call Continuity
    protected static const Int32 EVENT_SRVCC_STATE_CHANGED            = 31;
    protected static const Int32 EVENT_INITIATE_SILENT_REDIAL         = 32;
    protected static const Int32 EVENT_UNSOL_OEM_HOOK_RAW             = 33;
    protected static const Int32 EVENT_SS                             = 34;
    protected static const Int32 EVENT_SET_CALL_FORWARD_TIMER_DONE    = 35;
    protected static const Int32 EVENT_GET_CALL_FORWARD_TIMER_DONE    = 36;
    protected static const Int32 EVENT_LAST                   = EVENT_GET_CALL_FORWARD_TIMER_DONE;


    // Key used to read/write current CLIR setting
    public static const String CLIR_KEY = "clir_key";

    // Key used for storing voice mail count
    public static const String VM_COUNT = "vm_count_key";
    // Key used to read/write the ID for storing the voice mail
    public static const String VM_ID = "vm_id_key";

    // Key used to read/write "disable DNS server check" Pref (used for testing)
    public static const String DNS_SERVER_CHECK_DISABLED_KEY = "dns_server_check_disabled_key";

   //Telephony System Property used to indicate a multimode target
    public static const String PROPERTY_MULTIMODE_CDMA = "ro.config.multimode_cdma";

    /**
     * Small container class used to hold information relevant to
     * the carrier selection process. operatorNumeric can be ""
     * if we are looking for automatic selection. operatorAlphaLong is the
     * corresponding operator name.
     */
    protected static class NetworkSelectMessage {
        public Message message;
        public String operatorNumeric;
        public String operatorAlphaLong;
    }

    /* Instance Variables */
    public CommandsInterface mCi;
    private Int32 mVmCount = 0;
    Boolean mDnsCheckDisabled;
    public DcTrackerBase mDcTracker;
    Boolean mDoesRilSendMultipleCallRing;
    Int32 mCallRingContinueToken;
    Int32 mCallRingDelay;
    public Boolean mIsTheCurrentActivePhone = TRUE;
    Boolean mIsVoiceCapable = TRUE;

    // Variable to cache the video capabilitity. In some cases, we lose this info and are unable
    // to recover from the state. So, we cache it and notify listeners when they register.
    private Boolean mIsVideoCapable = FALSE;
    protected UiccController mUiccController = NULL;
    public AtomicReference<IccRecords> mIccRecords = new AtomicReference<IccRecords>();
    public SmsStorageMonitor mSmsStorageMonitor;
    public SmsUsageMonitor mSmsUsageMonitor;
    protected AtomicReference<UiccCardApplication> mUiccApplication =
            new AtomicReference<UiccCardApplication>();

    private TelephonyTester mTelephonyTester;
    private final String mName;
    private final String mActionDetached;
    private final String mActionAttached;

    // Holds the subscription information
    protected Subscription mSubscriptionData = NULL;
    protected Int32 mPhoneId;

    private final Object mImsLock = new Object();
    private Boolean mImsServiceReady = FALSE;
    protected ImsPhone mImsPhone = NULL;

    //@Override
    public String GetPhoneName() {
        return mName;
    }

    /**
     * Return the ActionDetached string. When this action is received by components
     * they are to simulate detaching from the network.
     *
     * @return com.android.internal.telephony.{mName}.action_detached
     *          {mName} is GSM, CDMA ...
     */
    public String GetActionDetached() {
        return mActionDetached;
    }

    /**
     * Return the ActionAttached string. When this action is received by components
     * they are to simulate attaching to the network.
     *
     * @return com.android.internal.telephony.{mName}.action_detached
     *          {mName} is GSM, CDMA ...
     */
    public String GetActionAttached() {
        return mActionAttached;
    }

    // Flag that indicates that Out Of Service is considered as data call disconnect
    protected Boolean mOosIsDisconnect = SystemProperties->GetBoolean(
            PROPERTY_OOS_IS_DISCONNECT, TRUE);

    /**
     * Set a system property, unless we're in unit test mode
     */
    // CAF_MSIM TODO this need to be replated with TelephonyManager API ?
    CARAPI SetSystemProperty(String property, String value) {
        If(GetUnitTestMode()) {
            return;
        }
        SystemProperties->Set(property, value);
    }

    /**
     * Set a system property, unless we're in unit test mode
     */
    // CAF_MSIM TODO this need to be replated with TelephonyManager API ?
    public String GetSystemProperty(String property, String defValue) {
        If(GetUnitTestMode()) {
            return NULL;
        }
        return SystemProperties->Get(property, defValue);
    }


    protected final RegistrantList mPreciseCallStateRegistrants
            = new RegistrantList();

    protected final RegistrantList mHandoverRegistrants
             = new RegistrantList();

    protected final RegistrantList mNewRingingConnectionRegistrants
            = new RegistrantList();

    protected final RegistrantList mIncomingRingRegistrants
            = new RegistrantList();

    protected final RegistrantList mDisconnectRegistrants
            = new RegistrantList();

    protected final RegistrantList mServiceStateRegistrants
            = new RegistrantList();

    protected final RegistrantList mMmiCompleteRegistrants
            = new RegistrantList();

    protected final RegistrantList mMmiRegistrants
            = new RegistrantList();

    protected final RegistrantList mUnknownConnectionRegistrants
            = new RegistrantList();

    protected final RegistrantList mSuppServiceFailedRegistrants
            = new RegistrantList();

    protected final RegistrantList mSimRecordsLoadedRegistrants
            = new RegistrantList();

    protected final RegistrantList mVideoCapabilityChangedRegistrants
            = new RegistrantList();


    protected Looper mLooper; /* to insure registrants are in correct thread*/

    protected final Context mContext;

    /**
     * PhoneNotifier is an abstraction for all system-wide
     * state change notification. DefaultPhoneNotifier is
     * used here unless running we're inside a unit test.
     */
    protected PhoneNotifier mNotifier;

    protected SimulatedRadioControl mSimulatedRadioControl;

    Boolean mUnitTestMode;

    /**
     * Constructs a PhoneBase in Normal (non-unit test) mode.
     *
     * @param notifier An instance of DefaultPhoneNotifier,
     * @param context Context object from hosting application
     * unless unit testing.
     * @param ci the CommandsInterface
     */
    protected PhoneBase(String name, PhoneNotifier notifier, Context context, CommandsInterface ci) {
        This(name, notifier, context, ci, FALSE);
    }

    /**
     * Constructs a PhoneBase in Normal (non-unit test) mode.
     *
     * @param notifier An instance of DefaultPhoneNotifier,
     * @param context Context object from hosting application
     * unless unit testing.
     * @param ci is CommandsInterface
     * @param unitTestMode when TRUE, prevents notifications
     * of state change events
     */
    protected PhoneBase(String name, PhoneNotifier notifier, Context context, CommandsInterface ci,
            Boolean unitTestMode) {
        This(name, notifier, context, ci, unitTestMode, SubscriptionManager.DEFAULT_PHONE_ID);
    }

    /**
     * Constructs a PhoneBase in Normal (non-unit test) mode.
     *
     * @param notifier An instance of DefaultPhoneNotifier,
     * @param context Context object from hosting application
     * unless unit testing.
     * @param ci is CommandsInterface
     * @param unitTestMode when TRUE, prevents notifications
     * of state change events
     * @param subscription is current phone subscription
     */
    protected PhoneBase(String name, PhoneNotifier notifier, Context context, CommandsInterface ci,
            Boolean unitTestMode, Int32 phoneId) {
        mPhoneId = phoneId;
        mName = name;
        mNotifier = notifier;
        mContext = context;
        mLooper = Looper->MyLooper();
        mCi = ci;
        mActionDetached = this->GetClass()->GetPackage().GetName() + ".action_detached";
        mActionAttached = this->GetClass()->GetPackage().GetName() + ".action_attached";

        If (Build.IS_DEBUGGABLE) {
            mTelephonyTester = new TelephonyTester(this);
        }

        SetUnitTestMode(unitTestMode);

        SharedPreferences sp = PreferenceManager->GetDefaultSharedPreferences(context);
        mDnsCheckDisabled = sp->GetBoolean(DNS_SERVER_CHECK_DISABLED_KEY, FALSE);
        mCi->SetOnCallRing(this, EVENT_CALL_RING, NULL);

        /* "Voice capable" means that this device supports circuit-switched
        * (i.e. voice) phone calls over the telephony network, and is allowed
        * to display the in-call UI while a cellular voice call is active.
        * This will be FALSE on "data only" devices which can't make voice
        * calls and don't support any in-call UI.
        */
        mIsVoiceCapable = mContext->GetResources()->GetBoolean(
                R.bool.config_voice_capable);

        /**
         *  Some RIL's don't always send RIL_UNSOL_CALL_RING so it needs
         *  to be generated locally. Ideally all ring tones should be loops
         * and this wouldn't be necessary. But to minimize changes to upper
         * layers it is requested that it be generated by lower layers.
         *
         * By default old phones won't have the property set but do generate
         * the RIL_UNSOL_CALL_RING so the default if there is no property is
         * TRUE.
         */
        mDoesRilSendMultipleCallRing = SystemProperties->GetBoolean(
                TelephonyProperties.PROPERTY_RIL_SENDS_MULTIPLE_CALL_RING, TRUE);
        Rlog->D(LOG_TAG, "mDoesRilSendMultipleCallRing=" + mDoesRilSendMultipleCallRing);

        mCallRingDelay = SystemProperties->GetInt(
                TelephonyProperties.PROPERTY_CALL_RING_DELAY, 3000);
        Rlog->D(LOG_TAG, "mCallRingDelay=" + mCallRingDelay);

        If (GetPhoneType() == PhoneConstants.PHONE_TYPE_IMS) return;

        SetPropertiesByCarrier();

        // Initialize device storage and outgoing SMS usage monitors for SMSDispatchers.
        mSmsStorageMonitor = new SmsStorageMonitor(this);
        mSmsUsageMonitor = new SmsUsageMonitor(context);
        mUiccController = UiccController->GetInstance();
        mUiccController->RegisterForIccChanged(this, EVENT_ICC_CHANGED, NULL);

        // Monitor IMS service
        IntentFilter filter = new IntentFilter();
        filter->AddAction(ImsManager.ACTION_IMS_SERVICE_UP);
        filter->AddAction(ImsManager.ACTION_IMS_SERVICE_DOWN);
        mContext->RegisterReceiver(mImsIntentReceiver, filter);

        mCi->RegisterForSrvccStateChanged(this, EVENT_SRVCC_STATE_CHANGED, NULL);
        mCi->SetOnUnsolOemHookRaw(this, EVENT_UNSOL_OEM_HOOK_RAW, NULL);
        Rlog->D(LOG_TAG, "mOosIsDisconnect=" + mOosIsDisconnect);
    }

    //@Override
    CARAPI Dispose() {
        Synchronized(PhoneProxy.lockForRadioTechnologyChange) {
            mContext->UnregisterReceiver(mImsIntentReceiver);
            mCi->UnSetOnCallRing(this);
            // Must cleanup all connectionS and needs to use sendMessage!
            mDcTracker->CleanUpAllConnections(NULL);
            mIsTheCurrentActivePhone = FALSE;
            // Dispose the SMS usage and storage monitors
            mSmsStorageMonitor->Dispose();
            mSmsUsageMonitor->Dispose();
            mUiccController->UnregisterForIccChanged(this);
            mCi->UnregisterForSrvccStateChanged(this);
            mCi->UnSetOnUnsolOemHookRaw(this);

            If (mTelephonyTester != NULL) {
                mTelephonyTester->Dispose();
            }

            ImsPhone imsPhone = mImsPhone;
            If (imsPhone != NULL) {
                imsPhone->UnregisterForSilentRedial(this);
                imsPhone->Dispose();
            }
        }
    }

    //@Override
    CARAPI RemoveReferences() {
        mSmsStorageMonitor = NULL;
        mSmsUsageMonitor = NULL;
        mIccRecords->Set(NULL);
        mUiccApplication->Set(NULL);
        mDcTracker = NULL;
        mUiccController = NULL;

        ImsPhone imsPhone = mImsPhone;
        If (imsPhone != NULL) {
            imsPhone->RemoveReferences();
            mImsPhone = NULL;
        }
    }

    /**
     * When overridden the derived class needs to call
     * super->HandleMessage(msg) so this method has a
     * a chance to process the message.
     *
     * @param msg
     */
    //@Override
    CARAPI HandleMessage(Message msg) {
        AsyncResult ar;

        If (!mIsTheCurrentActivePhone) {
            Rlog->E(LOG_TAG, "Received message " + msg +
                    "[" + msg.what + "] while being destroyed. Ignoring.");
            return;
        }
        Switch(msg.what) {
            case EVENT_CALL_RING:
                Rlog->D(LOG_TAG, "Event EVENT_CALL_RING Received state=" + GetState());
                ar = (AsyncResult)msg.obj;
                If (ar.exception == NULL) {
                    PhoneConstants.State state = GetState();
                    If ((!mDoesRilSendMultipleCallRing)
                            && ((state == PhoneConstants.State.RINGING) ||
                                    (state == PhoneConstants.State.IDLE))) {
                        mCallRingContinueToken += 1;
                        SendIncomingCallRingNotification(mCallRingContinueToken);
                    } else {
                        NotifyIncomingRing();
                    }
                }
                break;

            case EVENT_CALL_RING_CONTINUE:
                Rlog->D(LOG_TAG, "Event EVENT_CALL_RING_CONTINUE Received stat=" + GetState());
                If (GetState() == PhoneConstants.State.RINGING) {
                    SendIncomingCallRingNotification(msg.arg1);
                }
                break;

            case EVENT_ICC_CHANGED:
                OnUpdateIccAvailability();
                break;

            // handle the select network completion callbacks.
            case EVENT_SET_NETWORK_MANUAL_COMPLETE:
            case EVENT_SET_NETWORK_AUTOMATIC_COMPLETE:
                HandleSetSelectNetwork((AsyncResult) msg.obj);
                break;

            case EVENT_INITIATE_SILENT_REDIAL:
                Rlog->D(LOG_TAG, "Event EVENT_INITIATE_SILENT_REDIAL Received");
                ar = (AsyncResult) msg.obj;
                If ((ar.exception == NULL) && (ar.result != NULL)) {
                    String dialString = (String) ar.result;
                    If (TextUtils->IsEmpty(dialString)) return;
                    try {
                        DialInternal(dialString, NULL, VideoProfile.VideoState.AUDIO_ONLY);
                    } Catch (CallStateException e) {
                        Rlog->E(LOG_TAG, "silent redial failed: " + e);
                    }
                }
                break;

            case EVENT_SRVCC_STATE_CHANGED:
                ar = (AsyncResult)msg.obj;
                If (ar.exception == NULL) {
                    HandleSrvccStateChanged((Int32[]) ar.result);
                } else {
                    Rlog->E(LOG_TAG, "Srvcc exception: " + ar.exception);
                }
                break;

            case EVENT_UNSOL_OEM_HOOK_RAW:
                ar = (AsyncResult)msg.obj;
                If (ar.exception == NULL) {
                    Byte[] data = (Byte[])ar.result;
                    Rlog->D(LOG_TAG, "EVENT_UNSOL_OEM_HOOK_RAW data="
                            + IccUtils->BytesToHexString(data));
                    mNotifier->NotifyOemHookRawEventForSubscriber(GetSubId(), data);
                } else {
                    Rlog->E(LOG_TAG, "OEM hook raw exception: " + ar.exception);
                }
                break;

            default:
                throw new RuntimeException("unexpected event not handled");
        }
    }

    private void HandleSrvccStateChanged(Int32[] ret) {
        Rlog->D(LOG_TAG, "handleSrvccStateChanged");

        ArrayList<Connection> conn = NULL;
        ImsPhone imsPhone = mImsPhone;
        Call.SrvccState srvccState = Call.SrvccState.NONE;
        If (ret != NULL && ret.length != 0) {
            Int32 state = ret[0];
            Switch(state) {
                case VoLteServiceState.HANDOVER_STARTED:
                    srvccState = Call.SrvccState.STARTED;
                    If (imsPhone != NULL) {
                        conn = imsPhone->GetHandoverConnection();
                        MigrateFrom(imsPhone);
                    } else {
                        Rlog->D(LOG_TAG, "HANDOVER_STARTED: mImsPhone NULL");
                    }
                    break;
                case VoLteServiceState.HANDOVER_COMPLETED:
                    srvccState = Call.SrvccState.COMPLETED;
                    If (imsPhone != NULL) {
                        imsPhone->NotifySrvccState(srvccState);
                    } else {
                        Rlog->D(LOG_TAG, "HANDOVER_COMPLETED: mImsPhone NULL");
                    }
                    break;
                case VoLteServiceState.HANDOVER_FAILED:
                case VoLteServiceState.HANDOVER_CANCELED:
                    srvccState = Call.SrvccState.FAILED;
                    break;

                default:
                    //ignore invalid state
                    return;
            }

            GetCallTracker()->NotifySrvccState(srvccState, conn);

            VoLteServiceState lteState = new VoLteServiceState(state);
            NotifyVoLteServiceStateChanged(lteState);
        }
    }

    // Inherited documentation suffices.
    //@Override
    public Context GetContext() {
        return mContext;
    }

    // Set the Card into the Phone Book.
    protected void SetCardInPhoneBook() {
    }

    // Will be called when icc changed
    protected abstract void OnUpdateIccAvailability();

    /**
     * Disables the DNS Check (i.e., allows "0.0.0.0").
     * Useful for lab testing environment.
     * @param b TRUE disables the check, FALSE enables.
     */
    //@Override
    CARAPI DisableDnsCheck(Boolean b) {
        mDnsCheckDisabled = b;
        SharedPreferences sp = PreferenceManager->GetDefaultSharedPreferences(GetContext());
        SharedPreferences.Editor editor = sp->Edit();
        editor->PutBoolean(DNS_SERVER_CHECK_DISABLED_KEY, b);
        editor->Apply();
    }

    /**
     * Returns TRUE if the DNS check is currently disabled.
     */
    //@Override
    public Boolean IsDnsCheckDisabled() {
        return mDnsCheckDisabled;
    }

    // Inherited documentation suffices.
    //@Override
    CARAPI RegisterForPreciseCallStateChanged(Handler h, Int32 what, Object obj) {
        CheckCorrectThread(h);

        mPreciseCallStateRegistrants->AddUnique(h, what, obj);
    }

    // Inherited documentation suffices.
    //@Override
    CARAPI UnregisterForPreciseCallStateChanged(Handler h) {
        mPreciseCallStateRegistrants->Remove(h);
    }

    /**
     * Subclasses of Phone probably want to replace this with a
     * version scoped to their packages
     */
    protected void NotifyPreciseCallStateChangedP() {
        AsyncResult ar = new AsyncResult(NULL, this, NULL);
        mPreciseCallStateRegistrants->NotifyRegistrants(ar);

        mNotifier->NotifyPreciseCallState(this);
    }

    //@Override
    CARAPI RegisterForHandoverStateChanged(Handler h, Int32 what, Object obj) {
        CheckCorrectThread(h);
        mHandoverRegistrants->AddUnique(h, what, obj);
    }

    //@Override
    CARAPI UnregisterForHandoverStateChanged(Handler h) {
        mHandoverRegistrants->Remove(h);
    }

    /**
     * Subclasses of Phone probably want to replace this with a
     * version scoped to their packages
     */
    CARAPI NotifyHandoverStateChanged(Connection cn) {
       AsyncResult ar = new AsyncResult(NULL, cn, NULL);
       mHandoverRegistrants->NotifyRegistrants(ar);
    }

    CARAPI MigrateFrom(PhoneBase from) {
        Migrate(mHandoverRegistrants, from.mHandoverRegistrants);
        Migrate(mPreciseCallStateRegistrants, from.mPreciseCallStateRegistrants);
        Migrate(mNewRingingConnectionRegistrants, from.mNewRingingConnectionRegistrants);
        Migrate(mIncomingRingRegistrants, from.mIncomingRingRegistrants);
        Migrate(mDisconnectRegistrants, from.mDisconnectRegistrants);
        Migrate(mServiceStateRegistrants, from.mServiceStateRegistrants);
        Migrate(mMmiCompleteRegistrants, from.mMmiCompleteRegistrants);
        Migrate(mMmiRegistrants, from.mMmiRegistrants);
        Migrate(mUnknownConnectionRegistrants, from.mUnknownConnectionRegistrants);
        Migrate(mSuppServiceFailedRegistrants, from.mSuppServiceFailedRegistrants);
    }

    CARAPI Migrate(RegistrantList to, RegistrantList from) {
        from->RemoveCleared();
        For (Int32 i = 0, n = from->Size(); i < n; i++) {
            to->Add((Registrant) from->Get(i));
        }
    }

    // Inherited documentation suffices.
    //@Override
    CARAPI RegisterForUnknownConnection(Handler h, Int32 what, Object obj) {
        CheckCorrectThread(h);

        mUnknownConnectionRegistrants->AddUnique(h, what, obj);
    }

    // Inherited documentation suffices.
    //@Override
    CARAPI UnregisterForUnknownConnection(Handler h) {
        mUnknownConnectionRegistrants->Remove(h);
    }

    // Inherited documentation suffices.
    //@Override
    CARAPI RegisterForNewRingingConnection(
            Handler h, Int32 what, Object obj) {
        CheckCorrectThread(h);

        mNewRingingConnectionRegistrants->AddUnique(h, what, obj);
    }

    // Inherited documentation suffices.
    //@Override
    CARAPI UnregisterForNewRingingConnection(Handler h) {
        mNewRingingConnectionRegistrants->Remove(h);
    }

    // Inherited documentation suffices.
    //@Override
    CARAPI RegisterForVideoCapabilityChanged(
            Handler h, Int32 what, Object obj) {
        CheckCorrectThread(h);

        mVideoCapabilityChangedRegistrants->AddUnique(h, what, obj);

        // Notify any registrants of the cached video capability as soon as they register.
        NotifyForVideoCapabilityChanged(mIsVideoCapable);
    }

    // Inherited documentation suffices.
    //@Override
    CARAPI UnregisterForVideoCapabilityChanged(Handler h) {
        mVideoCapabilityChangedRegistrants->Remove(h);
    }

    // Inherited documentation suffices.
    //@Override
    CARAPI RegisterForInCallVoicePrivacyOn(Handler h, Int32 what, Object obj){
        mCi->RegisterForInCallVoicePrivacyOn(h, what, obj);
    }

    // Inherited documentation suffices.
    //@Override
    CARAPI UnregisterForInCallVoicePrivacyOn(Handler h){
        mCi->UnregisterForInCallVoicePrivacyOn(h);
    }

    // Inherited documentation suffices.
    //@Override
    CARAPI RegisterForInCallVoicePrivacyOff(Handler h, Int32 what, Object obj){
        mCi->RegisterForInCallVoicePrivacyOff(h, what, obj);
    }

    // Inherited documentation suffices.
    //@Override
    CARAPI UnregisterForInCallVoicePrivacyOff(Handler h){
        mCi->UnregisterForInCallVoicePrivacyOff(h);
    }

    // Inherited documentation suffices.
    //@Override
    CARAPI RegisterForIncomingRing(
            Handler h, Int32 what, Object obj) {
        CheckCorrectThread(h);

        mIncomingRingRegistrants->AddUnique(h, what, obj);
    }

    // Inherited documentation suffices.
    //@Override
    CARAPI UnregisterForIncomingRing(Handler h) {
        mIncomingRingRegistrants->Remove(h);
    }

    // Inherited documentation suffices.
    //@Override
    CARAPI RegisterForDisconnect(Handler h, Int32 what, Object obj) {
        CheckCorrectThread(h);

        mDisconnectRegistrants->AddUnique(h, what, obj);
    }

    // Inherited documentation suffices.
    //@Override
    CARAPI UnregisterForDisconnect(Handler h) {
        mDisconnectRegistrants->Remove(h);
    }

    // Inherited documentation suffices.
    //@Override
    CARAPI RegisterForSuppServiceFailed(Handler h, Int32 what, Object obj) {
        CheckCorrectThread(h);

        mSuppServiceFailedRegistrants->AddUnique(h, what, obj);
    }

    // Inherited documentation suffices.
    //@Override
    CARAPI UnregisterForSuppServiceFailed(Handler h) {
        mSuppServiceFailedRegistrants->Remove(h);
    }

    // Inherited documentation suffices.
    //@Override
    CARAPI RegisterForMmiInitiate(Handler h, Int32 what, Object obj) {
        CheckCorrectThread(h);

        mMmiRegistrants->AddUnique(h, what, obj);
    }

    // Inherited documentation suffices.
    //@Override
    CARAPI UnregisterForMmiInitiate(Handler h) {
        mMmiRegistrants->Remove(h);
    }

    // Inherited documentation suffices.
    //@Override
    CARAPI RegisterForMmiComplete(Handler h, Int32 what, Object obj) {
        CheckCorrectThread(h);

        mMmiCompleteRegistrants->AddUnique(h, what, obj);
    }

    // Inherited documentation suffices.
    //@Override
    CARAPI UnregisterForMmiComplete(Handler h) {
        CheckCorrectThread(h);

        mMmiCompleteRegistrants->Remove(h);
    }

    CARAPI RegisterForSimRecordsLoaded(Handler h, Int32 what, Object obj) {
        LogUnexpectedCdmaMethodCall("registerForSimRecordsLoaded");
    }

    CARAPI UnregisterForSimRecordsLoaded(Handler h) {
        LogUnexpectedCdmaMethodCall("unregisterForSimRecordsLoaded");
    }

    //@Override
    CARAPI SetNetworkSelectionModeAutomatic(Message response) {
        // wrap the response message in our own message along with
        // an empty String (to indicate automatic selection) for the
        // operator's id.
        NetworkSelectMessage nsm = new NetworkSelectMessage();
        nsm.message = response;
        nsm.operatorNumeric = "";
        nsm.operatorAlphaLong = "";

        Message msg = ObtainMessage(EVENT_SET_NETWORK_AUTOMATIC_COMPLETE, nsm);
        mCi->SetNetworkSelectionModeAutomatic(msg);
    }

    //@Override
    CARAPI SelectNetworkManually(OperatorInfo network, Message response) {
        // wrap the response message in our own message along with
        // the operator's id.
        NetworkSelectMessage nsm = new NetworkSelectMessage();
        nsm.message = response;
        nsm.operatorNumeric = network->GetOperatorNumeric();
        nsm.operatorAlphaLong = network->GetOperatorAlphaLong();

        Message msg = ObtainMessage(EVENT_SET_NETWORK_MANUAL_COMPLETE, nsm);
        If (network->GetRadioTech()->Equals("")) {
            mCi->SetNetworkSelectionModeManual(network->GetOperatorNumeric(), msg);
        } else {
            mCi->SetNetworkSelectionModeManual(network->GetOperatorNumeric()
                    + "+" + network->GetRadioTech(), msg);
        }
    }

    /**
     * Used to track the settings upon completion of the network change.
     */
    private void HandleSetSelectNetwork(AsyncResult ar) {
        // look for our wrapper within the asyncresult, skip the rest if it
        // is NULL.
        If (!(ar.userObj instanceof NetworkSelectMessage)) {
            Rlog->E(LOG_TAG, "unexpected result from user object.");
            return;
        }

        NetworkSelectMessage nsm = (NetworkSelectMessage) ar.userObj;

        // found the object, now we send off the message we had originally
        // attached to the request.
        If (nsm.message != NULL) {
            AsyncResult->ForMessage(nsm.message, ar.result, ar.exception);
            nsm.message->SendToTarget();
        }

        // open the shared preferences editor, and write the value.
        // nsm.operatorNumeric is "" if we're in automatic.selection.
        SharedPreferences sp = PreferenceManager->GetDefaultSharedPreferences(GetContext());
        SharedPreferences.Editor editor = sp->Edit();
        editor->PutString(NETWORK_SELECTION_KEY, nsm.operatorNumeric);
        editor->PutString(NETWORK_SELECTION_NAME_KEY, nsm.operatorAlphaLong);

        // commit and log the result.
        If (!editor->Commit()) {
            Rlog->E(LOG_TAG, "failed to commit network selection preference");
        }
    }

    /**
     * Method to retrieve the saved operator id from the Shared Preferences
     */
    private String GetSavedNetworkSelection() {
        // open the shared preferences and search with our key.
        SharedPreferences sp = PreferenceManager->GetDefaultSharedPreferences(GetContext());
        return sp->GetString(NETWORK_SELECTION_KEY, "");
    }

    /**
     * Method to restore the previously saved operator id, or reset to
     * automatic selection, all depending upon the value in the shared
     * preferences.
     */
    CARAPI RestoreSavedNetworkSelection(Message response) {
        // retrieve the operator id
        String networkSelection = GetSavedNetworkSelection();

        // set to auto if the id is empty, otherwise select the network.
        If (TextUtils->IsEmpty(networkSelection)) {
            mCi->SetNetworkSelectionModeAutomatic(response);
        } else {
            mCi->SetNetworkSelectionModeManual(networkSelection, response);
        }
    }

    // Inherited documentation suffices.
    //@Override
    CARAPI SetUnitTestMode(Boolean f) {
        mUnitTestMode = f;
    }

    // Inherited documentation suffices.
    //@Override
    public Boolean GetUnitTestMode() {
        return mUnitTestMode;
    }

    /**
     * To be invoked when a voice call Connection disconnects.
     *
     * Subclasses of Phone probably want to replace this with a
     * version scoped to their packages
     */
    protected void NotifyDisconnectP(Connection cn) {
        AsyncResult ar = new AsyncResult(NULL, cn, NULL);
        mDisconnectRegistrants->NotifyRegistrants(ar);
    }

    // Inherited documentation suffices.
    //@Override
    CARAPI RegisterForServiceStateChanged(
            Handler h, Int32 what, Object obj) {
        CheckCorrectThread(h);

        mServiceStateRegistrants->Add(h, what, obj);
    }

    // Inherited documentation suffices.
    //@Override
    CARAPI UnregisterForServiceStateChanged(Handler h) {
        mServiceStateRegistrants->Remove(h);
    }

    // Inherited documentation suffices.
    //@Override
    CARAPI RegisterForRingbackTone(Handler h, Int32 what, Object obj) {
        mCi->RegisterForRingbackTone(h, what, obj);
    }

    // Inherited documentation suffices.
    //@Override
    CARAPI UnregisterForRingbackTone(Handler h) {
        mCi->UnregisterForRingbackTone(h);
    }

    // Inherited documentation suffices.
    //@Override
    CARAPI RegisterForOnHoldTone(Handler h, Int32 what, Object obj) {
    }

    // Inherited documentation suffices.
    //@Override
    CARAPI UnregisterForOnHoldTone(Handler h) {
    }

    // Inherited documentation suffices.
    //@Override
    CARAPI RegisterForResendIncallMute(Handler h, Int32 what, Object obj) {
        mCi->RegisterForResendIncallMute(h, what, obj);
    }

    // Inherited documentation suffices.
    //@Override
    CARAPI UnregisterForResendIncallMute(Handler h) {
        mCi->UnregisterForResendIncallMute(h);
    }

    //@Override
    CARAPI SetEchoSuppressionEnabled() {
        // no need for regular phone
    }

    /**
     * Subclasses of Phone probably want to replace this with a
     * version scoped to their packages
     */
    protected void NotifyServiceStateChangedP(ServiceState ss) {
        AsyncResult ar = new AsyncResult(NULL, ss, NULL);
        mServiceStateRegistrants->NotifyRegistrants(ar);

        mNotifier->NotifyServiceState(this);
    }

    // Inherited documentation suffices.
    //@Override
    public SimulatedRadioControl GetSimulatedRadioControl() {
        return mSimulatedRadioControl;
    }

    /**
     * Verifies the current thread is the same as the thread originally
     * used in the initialization of this instance. Throws RuntimeException
     * if not.
     *
     * @exception RuntimeException if the current thread is not
     * the thread that originally obtained this PhoneBase instance.
     */
    private void CheckCorrectThread(Handler h) {
        If (h->GetLooper() != mLooper) {
            throw new RuntimeException(
                    "com.android.internal.telephony.Phone must be used from within one thread");
        }
    }

    /**
     * Set the properties by matching the carrier string in
     * a string-array resource
     */
    private void SetPropertiesByCarrier() {
        String carrier = SystemProperties->Get("ro.carrier");

        If (NULL == carrier || 0 == carrier->Length() || "unknown".Equals(carrier)) {
            return;
        }

        CharSequence[] carrierLocales = mContext.
                GetResources()->GetTextArray(R.array.carrier_properties);

        For (Int32 i = 0; i < carrierLocales.length; i+=3) {
            String c = carrierLocales[i].ToString();
            If (carrier->Equals(c)) {
                final Locale l = Locale->ForLanguageTag(carrierLocales[i + 1].ToString()->Replace('_', '-'));
                final String country = l->GetCountry();
                MccTable->SetSystemLocale(mContext, l->GetLanguage(), country);

                If (!country->IsEmpty()) {
                    try {
                        Settings::Global::>GetInt(mContext->GetContentResolver(),
                                Settings::Global::WIFI_COUNTRY_CODE);
                    } Catch (Settings.SettingNotFoundException e) {
                        // note this is not persisting
                        WifiManager wM = (WifiManager)
                                mContext->GetSystemService(Context.WIFI_SERVICE);
                        wM->SetCountryCode(country, FALSE);
                    }
                }
                return;
            }
        }
    }

    /**
     * Get state
     */
    //@Override
    public abstract PhoneConstants.State GetState();

    /**
     * Retrieves the IccFileHandler of the Phone instance
     */
    public IccFileHandler GetIccFileHandler(){
        UiccCardApplication uiccApplication = mUiccApplication->Get();
        IccFileHandler fh;

        If (uiccApplication == NULL) {
            Rlog->D(LOG_TAG, "getIccFileHandler: uiccApplication == NULL, return NULL");
            fh = NULL;
        } else {
            fh = uiccApplication->GetIccFileHandler();
        }

        Rlog->D(LOG_TAG, "getIccFileHandler: fh=" + fh);
        return fh;
    }

    /**
     * Retrieves the IccRecords of the Phone instance
     */
    public IccRecords GetIccRecords(){
        UiccCardApplication uiccApplication = mUiccApplication->Get();
        If (uiccApplication == NULL) return NULL;
        return uiccApplication->GetIccRecords();
    }

    /*
     * Retrieves the Handler of the Phone instance
     */
    public Handler GetHandler() {
        return this;
    }

    //@Override
    CARAPI UpdatePhoneObject(Int32 voiceRadioTech) {
        Rlog->D(LOG_TAG, "updatePhoneObject: phoneid = " + mPhoneId + " rat = " + voiceRadioTech);
        // Only the PhoneProxy can update the phone object.
        Phone phoneProxy = PhoneFactory->GetPhone(mPhoneId);
        If (phoneProxy != NULL) {
            phoneProxy->UpdatePhoneObject(voiceRadioTech);
        }
    }

    /**
    * Retrieves the ServiceStateTracker of the phone instance.
    */
    public ServiceStateTracker GetServiceStateTracker() {
        return NULL;
    }

    /**
    * Get call tracker
    */
    public CallTracker GetCallTracker() {
        return NULL;
    }

    public AppType GetCurrentUiccAppType() {
        UiccCardApplication currentApp = mUiccApplication->Get();
        If (currentApp != NULL) {
            return currentApp->GetType();
        }
        return AppType.APPTYPE_UNKNOWN;
    }

    //@Override
    public IccCard GetIccCard() {
        return NULL;
        //throw new Exception("getIccCard Shouldn't be called from PhoneBase");
    }

    //@Override
    public String GetIccSerialNumber() {
        IccRecords r = mIccRecords->Get();
        Return (r != NULL) ? r->GetIccId() : NULL;
    }

    //@Override
    public Boolean GetIccRecordsLoaded() {
        IccRecords r = mIccRecords->Get();
        Return (r != NULL) ? r->GetRecordsLoaded() : FALSE;
    }

    /**
     * @return all available cell information or NULL if none.
     */
    //@Override
    public List<CellInfo> GetAllCellInfo() {
        List<CellInfo> cellInfoList = GetServiceStateTracker()->GetAllCellInfo();
        return PrivatizeCellInfoList(cellInfoList);
    }

    /**
     * Clear CDMA base station lat/Int64 values if location setting is disabled.
     * @param cellInfoList the original cell info list from the RIL
     * @return the original list with CDMA lat/Int64 cleared if necessary
     */
    private List<CellInfo> PrivatizeCellInfoList(List<CellInfo> cellInfoList) {
        Int32 mode = Settings.Secure->GetInt(GetContext()->GetContentResolver(),
                Settings.Secure.LOCATION_MODE, Settings.Secure.LOCATION_MODE_OFF);
        If (mode == Settings.Secure.LOCATION_MODE_OFF) {
            ArrayList<CellInfo> privateCellInfoList = new ArrayList<CellInfo>(cellInfoList->Size());
            // clear lat/lon values for location privacy
            For (CellInfo c : cellInfoList) {
                If (c instanceof CellInfoCdma) {
                    CellInfoCdma cellInfoCdma = (CellInfoCdma) c;
                    CellIdentityCdma cellIdentity = cellInfoCdma->GetCellIdentity();
                    CellIdentityCdma maskedCellIdentity = new CellIdentityCdma(
                            cellIdentity->GetNetworkId(),
                            cellIdentity->GetSystemId(),
                            cellIdentity->GetBasestationId(),
                            Integer.MAX_VALUE, Integer.MAX_VALUE);
                    CellInfoCdma privateCellInfoCdma = new CellInfoCdma(cellInfoCdma);
                    privateCellInfoCdma->SetCellIdentity(maskedCellIdentity);
                    privateCellInfoList->Add(privateCellInfoCdma);
                } else {
                    privateCellInfoList->Add(c);
                }
            }
            cellInfoList = privateCellInfoList;
        }
        return cellInfoList;
    }

    /**
     * {@inheritDoc}
     */
    //@Override
    CARAPI SetCellInfoListRate(Int32 rateInMillis) {
        mCi->SetCellInfoListRate(rateInMillis, NULL);
    }

    //@Override
    /** @return TRUE if there are messages waiting, FALSE otherwise. */
    public Boolean GetMessageWaitingIndicator() {
        return mVmCount != 0;
    }

    //@Override
    public Boolean GetCallForwardingIndicator() {
        IccRecords r = mIccRecords->Get();
        Return (r != NULL) ? r->GetVoiceCallForwardingFlag() : FALSE;
    }

    //@Override
    CARAPI SetCallForwardingUncondTimerOption(Int32 startHour, Int32 startMinute,
            Int32 endHour, Int32 endMinute, Int32 commandInterfaceCFReason,
            Int32 commandInterfaceCFAction, String dialingNumber,
            Message onComplete) {
        Rlog->E(LOG_TAG, "setCallForwardingUncondTimerOption error ");
    }

    //@Override
    CARAPI GetCallForwardingUncondTimerOption(Int32 commandInterfaceCFReason,
            Message onComplete) {
        Rlog->E(LOG_TAG, "getCallForwardingUncondTimerOption error ");
    }

    /**
     *  Query the status of the CDMA roaming preference
     */
    //@Override
    CARAPI QueryCdmaRoamingPreference(Message response) {
        mCi->QueryCdmaRoamingPreference(response);
    }

    /**
     * Get the signal strength
     */
    //@Override
    public SignalStrength GetSignalStrength() {
        ServiceStateTracker sst = GetServiceStateTracker();
        If (sst == NULL) {
            return new SignalStrength();
        } else {
            return sst->GetSignalStrength();
        }
    }

    /**
     *  Set the status of the CDMA roaming preference
     */
    //@Override
    CARAPI SetCdmaRoamingPreference(Int32 cdmaRoamingType, Message response) {
        mCi->SetCdmaRoamingPreference(cdmaRoamingType, response);
    }

    /**
     *  Set the status of the CDMA subscription mode
     */
    //@Override
    CARAPI SetCdmaSubscription(Int32 cdmaSubscriptionType, Message response) {
        mCi->SetCdmaSubscriptionSource(cdmaSubscriptionType, response);
    }

    /**
     *  Set the preferred Network Type: Global, CDMA only or GSM/UMTS only
     */
    //@Override
    CARAPI SetPreferredNetworkType(Int32 networkType, Message response) {
        If (TelephonyManager->GetDefault()->GetPhoneCount() > 1) {
            ModemBindingPolicyHandler->GetInstance()->SetPreferredNetworkType(networkType,
                    GetPhoneId(),response);
        } else {
            mCi->SetPreferredNetworkType(networkType, response);
        }
    }

    //@Override
    CARAPI GetPreferredNetworkType(Message response) {
        mCi->GetPreferredNetworkType(response);
    }

    //@Override
    CARAPI GetSmscAddress(Message result) {
        mCi->GetSmscAddress(result);
    }

    //@Override
    CARAPI SetSmscAddress(String address, Message result) {
        mCi->SetSmscAddress(address, result);
    }

    //@Override
    CARAPI SetTTYMode(Int32 ttyMode, Message onComplete) {
        mCi->SetTTYMode(ttyMode, onComplete);
    }

    //@Override
    CARAPI SetUiTTYMode(Int32 uiTtyMode, Message onComplete) {
        Rlog->D(LOG_TAG, "unexpected setUiTTYMode method call");
    }

    //@Override
    CARAPI QueryTTYMode(Message onComplete) {
        mCi->QueryTTYMode(onComplete);
    }

    //@Override
    CARAPI EnableEnhancedVoicePrivacy(Boolean enable, Message onComplete) {
        // This function should be overridden by the class CDMAPhone. Not implemented in GSMPhone.
        LogUnexpectedCdmaMethodCall("enableEnhancedVoicePrivacy");
    }

    //@Override
    CARAPI GetEnhancedVoicePrivacy(Message onComplete) {
        // This function should be overridden by the class CDMAPhone. Not implemented in GSMPhone.
        LogUnexpectedCdmaMethodCall("getEnhancedVoicePrivacy");
    }

    //@Override
    CARAPI SetBandMode(Int32 bandMode, Message response) {
        mCi->SetBandMode(bandMode, response);
    }

    //@Override
    CARAPI QueryAvailableBandMode(Message response) {
        mCi->QueryAvailableBandMode(response);
    }

    //@Override
    CARAPI InvokeOemRilRequestRaw(Byte[] data, Message response) {
        mCi->InvokeOemRilRequestRaw(data, response);
    }

    //@Override
    CARAPI InvokeOemRilRequestStrings(String[] strings, Message response) {
        mCi->InvokeOemRilRequestStrings(strings, response);
    }

    //@Override
    CARAPI NvReadItem(Int32 itemID, Message response) {
        mCi->NvReadItem(itemID, response);
    }

    //@Override
    CARAPI NvWriteItem(Int32 itemID, String itemValue, Message response) {
        mCi->NvWriteItem(itemID, itemValue, response);
    }

    //@Override
    CARAPI NvWriteCdmaPrl(Byte[] preferredRoamingList, Message response) {
        mCi->NvWriteCdmaPrl(preferredRoamingList, response);
    }

    //@Override
    CARAPI NvResetConfig(Int32 resetType, Message response) {
        mCi->NvResetConfig(resetType, response);
    }

    //@Override
    CARAPI NotifyDataActivity() {
        mNotifier->NotifyDataActivity(this);
    }

    CARAPI NotifyMessageWaitingIndicator() {
        // Do not notify voice mail waiting if device doesn't support voice
        If (!mIsVoiceCapable)
            return;

        // This function is added to send the notification to DefaultPhoneNotifier.
        mNotifier->NotifyMessageWaitingChanged(this);
    }

    CARAPI NotifyDataConnection(String reason, String apnType,
            PhoneConstants.DataState state) {
        mNotifier->NotifyDataConnection(this, reason, apnType, state);
    }

    CARAPI NotifyDataConnection(String reason, String apnType) {
        mNotifier->NotifyDataConnection(this, reason, apnType, GetDataConnectionState(apnType));
    }

    CARAPI NotifyDataConnection(String reason) {
        String types[] = GetActiveApnTypes();
        For (String apnType : types) {
            mNotifier->NotifyDataConnection(this, reason, apnType, GetDataConnectionState(apnType));
        }
    }

    CARAPI NotifyOtaspChanged(Int32 otaspMode) {
        mNotifier->NotifyOtaspChanged(this, otaspMode);
    }

    CARAPI NotifySignalStrength() {
        mNotifier->NotifySignalStrength(this);
    }

    CARAPI NotifyCellInfo(List<CellInfo> cellInfo) {
        mNotifier->NotifyCellInfo(this, PrivatizeCellInfoList(cellInfo));
    }

    CARAPI NotifyDataConnectionRealTimeInfo(DataConnectionRealTimeInfo dcRtInfo) {
        mNotifier->NotifyDataConnectionRealTimeInfo(this, dcRtInfo);
    }

    CARAPI NotifyVoLteServiceStateChanged(VoLteServiceState lteState) {
        mNotifier->NotifyVoLteServiceStateChanged(this, lteState);
    }

    /**
     * @return TRUE if a mobile originating emergency call is active
     */
    public Boolean IsInEmergencyCall() {
        return FALSE;
    }

    /**
     * @return TRUE if we are in the emergency call back mode. This is a period where
     * the phone should be using as little power as possible and be ready to receive an
     * incoming call from the emergency operator.
     */
    public Boolean IsInEcm() {
        return FALSE;
    }

    public static Int32 GetVideoState(Call call) {
        Int32 videoState = VideoProfile.VideoState.AUDIO_ONLY;
        ImsPhoneConnection conn = (ImsPhoneConnection) call->GetEarliestConnection();
        If (conn != NULL) {
            videoState = conn->GetVideoState();
        }
        return videoState;
    }

    private Boolean IsImsVideoCall(Call call) {
        Int32 videoState = GetVideoState(call);
        Return (VideoProfile.VideoState->IsVideo(videoState));
    }

    public Boolean IsImsVtCallPresent() {
        Boolean isVideoCallActive = FALSE;
        If (mImsPhone != NULL) {
            isVideoCallActive = IsImsVideoCall(mImsPhone->GetForegroundCall()) ||
                    IsImsVideoCall(mImsPhone->GetBackgroundCall()) ||
                    IsImsVideoCall(mImsPhone->GetRingingCall());
        }
        Rlog->D(LOG_TAG, "isVideoCallActive: " + isVideoCallActive);
        return isVideoCallActive;
    }

    //@Override
    public abstract Int32 GetPhoneType();

    /** @hide */
    /** @return number of voicemails */
    //@Override
    public Int32 GetVoiceMessageCount(){
        return mVmCount;
    }

    /** sets the voice mail count of the phone and notifies listeners. */
    CARAPI SetVoiceMessageCount(Int32 countWaiting) {
        mVmCount = countWaiting;
        // notify listeners of voice mail
        NotifyMessageWaitingIndicator();
    }

    /**
     * Returns the CDMA ERI icon index to display
     */
    //@Override
    public Int32 GetCdmaEriIconIndex() {
        LogUnexpectedCdmaMethodCall("getCdmaEriIconIndex");
        return -1;
    }

    /**
     * Returns the CDMA ERI icon mode,
     * 0 - ON
     * 1 - FLASHING
     */
    //@Override
    public Int32 GetCdmaEriIconMode() {
        LogUnexpectedCdmaMethodCall("getCdmaEriIconMode");
        return -1;
    }

    /**
     * Returns the CDMA ERI text,
     */
    //@Override
    public String GetCdmaEriText() {
        LogUnexpectedCdmaMethodCall("getCdmaEriText");
        return "GSM nw, no ERI";
    }

    //@Override
    public String GetCdmaMin() {
        // This function should be overridden by the class CDMAPhone. Not implemented in GSMPhone.
        LogUnexpectedCdmaMethodCall("getCdmaMin");
        return NULL;
    }

    //@Override
    public Boolean IsMinInfoReady() {
        // This function should be overridden by the class CDMAPhone. Not implemented in GSMPhone.
        LogUnexpectedCdmaMethodCall("isMinInfoReady");
        return FALSE;
    }

    //@Override
    public String GetCdmaPrlVersion(){
        //  This function should be overridden by the class CDMAPhone. Not implemented in GSMPhone.
        LogUnexpectedCdmaMethodCall("getCdmaPrlVersion");
        return NULL;
    }

    //@Override
    CARAPI SendBurstDtmf(String dtmfString, Int32 on, Int32 off, Message onComplete) {
        // This function should be overridden by the class CDMAPhone. Not implemented in GSMPhone.
        LogUnexpectedCdmaMethodCall("sendBurstDtmf");
    }

    //@Override
    CARAPI ExitEmergencyCallbackMode() {
        // This function should be overridden by the class CDMAPhone. Not implemented in GSMPhone.
        LogUnexpectedCdmaMethodCall("exitEmergencyCallbackMode");
    }

    //@Override
    CARAPI RegisterForCdmaOtaStatusChange(Handler h, Int32 what, Object obj) {
        // This function should be overridden by the class CDMAPhone. Not implemented in GSMPhone.
        LogUnexpectedCdmaMethodCall("registerForCdmaOtaStatusChange");
    }

    //@Override
    CARAPI UnregisterForCdmaOtaStatusChange(Handler h) {
        // This function should be overridden by the class CDMAPhone. Not implemented in GSMPhone.
        LogUnexpectedCdmaMethodCall("unregisterForCdmaOtaStatusChange");
    }

    //@Override
    CARAPI RegisterForSubscriptionInfoReady(Handler h, Int32 what, Object obj) {
        // This function should be overridden by the class CDMAPhone. Not implemented in GSMPhone.
        LogUnexpectedCdmaMethodCall("registerForSubscriptionInfoReady");
    }

    //@Override
    CARAPI UnregisterForSubscriptionInfoReady(Handler h) {
        // This function should be overridden by the class CDMAPhone. Not implemented in GSMPhone.
        LogUnexpectedCdmaMethodCall("unregisterForSubscriptionInfoReady");
    }

    /**
     * Returns TRUE if OTA Service Provisioning needs to be performed.
     * If not overridden return FALSE.
     */
    //@Override
    public Boolean NeedsOtaServiceProvisioning() {
        return FALSE;
    }

    /**
     * Return TRUE if number is an OTASP number.
     * If not overridden return FALSE.
     */
    //@Override
    public  Boolean IsOtaSpNumber(String dialStr) {
        return FALSE;
    }

    //@Override
    CARAPI RegisterForCallWaiting(Handler h, Int32 what, Object obj){
        // This function should be overridden by the class CDMAPhone. Not implemented in GSMPhone.
        LogUnexpectedCdmaMethodCall("registerForCallWaiting");
    }

    //@Override
    CARAPI UnregisterForCallWaiting(Handler h){
        // This function should be overridden by the class CDMAPhone. Not implemented in GSMPhone.
        LogUnexpectedCdmaMethodCall("unregisterForCallWaiting");
    }

    //@Override
    CARAPI RegisterForEcmTimerReset(Handler h, Int32 what, Object obj) {
        // This function should be overridden by the class CDMAPhone. Not implemented in GSMPhone.
        LogUnexpectedCdmaMethodCall("registerForEcmTimerReset");
    }

    //@Override
    CARAPI UnregisterForEcmTimerReset(Handler h) {
        // This function should be overridden by the class CDMAPhone. Not implemented in GSMPhone.
        LogUnexpectedCdmaMethodCall("unregisterForEcmTimerReset");
    }

    //@Override
    CARAPI RegisterForSignalInfo(Handler h, Int32 what, Object obj) {
        mCi->RegisterForSignalInfo(h, what, obj);
    }

    //@Override
    CARAPI UnregisterForSignalInfo(Handler h) {
        mCi->UnregisterForSignalInfo(h);
    }

    //@Override
    CARAPI RegisterForDisplayInfo(Handler h, Int32 what, Object obj) {
        mCi->RegisterForDisplayInfo(h, what, obj);
    }

     //@Override
    CARAPI UnregisterForDisplayInfo(Handler h) {
         mCi->UnregisterForDisplayInfo(h);
     }

    //@Override
    CARAPI RegisterForNumberInfo(Handler h, Int32 what, Object obj) {
        mCi->RegisterForNumberInfo(h, what, obj);
    }

    //@Override
    CARAPI UnregisterForNumberInfo(Handler h) {
        mCi->UnregisterForNumberInfo(h);
    }

    //@Override
    CARAPI RegisterForRedirectedNumberInfo(Handler h, Int32 what, Object obj) {
        mCi->RegisterForRedirectedNumberInfo(h, what, obj);
    }

    //@Override
    CARAPI UnregisterForRedirectedNumberInfo(Handler h) {
        mCi->UnregisterForRedirectedNumberInfo(h);
    }

    //@Override
    CARAPI RegisterForLineControlInfo(Handler h, Int32 what, Object obj) {
        mCi->RegisterForLineControlInfo( h, what, obj);
    }

    //@Override
    CARAPI UnregisterForLineControlInfo(Handler h) {
        mCi->UnregisterForLineControlInfo(h);
    }

    //@Override
    CARAPI RegisterFoT53ClirlInfo(Handler h, Int32 what, Object obj) {
        mCi->RegisterFoT53ClirlInfo(h, what, obj);
    }

    //@Override
    CARAPI UnregisterForT53ClirInfo(Handler h) {
        mCi->UnregisterForT53ClirInfo(h);
    }

    //@Override
    CARAPI RegisterForT53AudioControlInfo(Handler h, Int32 what, Object obj) {
        mCi->RegisterForT53AudioControlInfo( h, what, obj);
    }

    //@Override
    CARAPI UnregisterForT53AudioControlInfo(Handler h) {
        mCi->UnregisterForT53AudioControlInfo(h);
    }

     //@Override
    CARAPI SetOnEcbModeExitResponse(Handler h, Int32 what, Object obj){
         // This function should be overridden by the class CDMAPhone. Not implemented in GSMPhone.
         LogUnexpectedCdmaMethodCall("setOnEcbModeExitResponse");
     }

     //@Override
    CARAPI UnsetOnEcbModeExitResponse(Handler h){
        // This function should be overridden by the class CDMAPhone. Not implemented in GSMPhone.
         LogUnexpectedCdmaMethodCall("unsetOnEcbModeExitResponse");
     }

    //@Override
    public String[] GetActiveApnTypes() {
        return mDcTracker->GetActiveApnTypes();
    }

    //@Override
    public String GetActiveApnHost(String apnType) {
        return mDcTracker->GetActiveApnString(apnType);
    }

    //@Override
    public LinkProperties GetLinkProperties(String apnType) {
        return mDcTracker->GetLinkProperties(apnType);
    }

    //@Override
    public NetworkCapabilities GetNetworkCapabilities(String apnType) {
        return mDcTracker->GetNetworkCapabilities(apnType);
    }

    //@Override
    public Boolean IsDataConnectivityPossible() {
        return IsDataConnectivityPossible(PhoneConstants.APN_TYPE_DEFAULT);
    }

    //@Override
    public Boolean IsOnDemandDataPossible(String apnType) {
        Return ((mDcTracker != NULL) &&
                (mDcTracker->IsOnDemandDataPossible(apnType)));
    }

    //@Override
    public Boolean IsDataConnectivityPossible(String apnType) {
        Return ((mDcTracker != NULL) &&
                (mDcTracker->IsDataPossible(apnType)));
    }

    /**
     * Notify registrants of a new ringing Connection.
     * Subclasses of Phone probably want to replace this with a
     * version scoped to their packages
     */
    CARAPI NotifyNewRingingConnectionP(Connection cn) {
        If (!mIsVoiceCapable)
            return;
        AsyncResult ar = new AsyncResult(NULL, cn, NULL);
        mNewRingingConnectionRegistrants->NotifyRegistrants(ar);
    }


    /**
     * Notify registrants if phone is video capable.
     */
    CARAPI NotifyForVideoCapabilityChanged(Boolean isVideoCallCapable) {
        // Cache the current video capability so that we don't lose the information.
        mIsVideoCapable = isVideoCallCapable;

        AsyncResult ar = new AsyncResult(NULL, isVideoCallCapable, NULL);
        mVideoCapabilityChangedRegistrants->NotifyRegistrants(ar);
    }

    /**
     * Notify registrants of a RING event.
     */
    private void NotifyIncomingRing() {
        If (!mIsVoiceCapable)
            return;
        AsyncResult ar = new AsyncResult(NULL, this, NULL);
        mIncomingRingRegistrants->NotifyRegistrants(ar);
    }

    /**
     * Send the incoming call Ring notification if conditions are right.
     */
    private void SendIncomingCallRingNotification(Int32 token) {
        If (mIsVoiceCapable && !mDoesRilSendMultipleCallRing &&
                (token == mCallRingContinueToken)) {
            Rlog->D(LOG_TAG, "Sending notifyIncomingRing");
            NotifyIncomingRing();
            SendMessageDelayed(
                    ObtainMessage(EVENT_CALL_RING_CONTINUE, token, 0), mCallRingDelay);
        } else {
            Rlog->D(LOG_TAG, "Ignoring ring notification request,"
                    + " mDoesRilSendMultipleCallRing=" + mDoesRilSendMultipleCallRing
                    + " token=" + token
                    + " mCallRingContinueToken=" + mCallRingContinueToken
                    + " mIsVoiceCapable=" + mIsVoiceCapable);
        }
    }

    public Boolean IsManualNetSelAllowed() {
        // This function should be overridden in GsmPhone.
        // Not implemented by default.
        return FALSE;
    }

    //@Override
    public Boolean IsCspPlmnEnabled() {
        // This function should be overridden by the class GSMPhone.
        // Not implemented in CDMAPhone.
        LogUnexpectedGsmMethodCall("isCspPlmnEnabled");
        return FALSE;
    }

    //@Override
    public IsimRecords GetIsimRecords() {
        Rlog->E(LOG_TAG, "GetIsimRecords() is only supported on LTE devices");
        return NULL;
    }

    //@Override
    public String GetMsisdn() {
        LogUnexpectedGsmMethodCall("getMsisdn");
        return NULL;
    }

    /**
     * Common error logger method for unexpected calls to CDMA-only methods.
     */
    private static void LogUnexpectedCdmaMethodCall(String name)
    {
        Rlog->E(LOG_TAG, "Error! " + name + "() in PhoneBase should not be " +
                "called, CDMAPhone inactive.");
    }

    //@Override
    public PhoneConstants.DataState GetDataConnectionState() {
        return GetDataConnectionState(PhoneConstants.APN_TYPE_DEFAULT);
    }

    /**
     * Common error logger method for unexpected calls to GSM/WCDMA-only methods.
     */
    private static void LogUnexpectedGsmMethodCall(String name) {
        Rlog->E(LOG_TAG, "Error! " + name + "() in PhoneBase should not be " +
                "called, GSMPhone inactive.");
    }

    // Called by SimRecords which is constructed with a PhoneBase instead of a GSMPhone.
    CARAPI NotifyCallForwardingIndicator() {
        // This function should be overridden by the class GSMPhone. Not implemented in CDMAPhone.
        Rlog->E(LOG_TAG, "Error! This function should never be executed, inactive CDMAPhone.");
    }

    CARAPI NotifyDataConnectionFailed(String reason, String apnType) {
        mNotifier->NotifyDataConnectionFailed(this, reason, apnType);
    }

    CARAPI NotifyPreciseDataConnectionFailed(String reason, String apnType, String apn,
            String failCause) {
        mNotifier->NotifyPreciseDataConnectionFailed(this, reason, apnType, apn, failCause);
    }

    /**
     * {@inheritDoc}
     */
    //@Override
    public Int32 GetLteOnCdmaMode() {
        return mCi->GetLteOnCdmaMode();
    }

    /**
     * {@hide}
     */
    //@Override
    public Int32 GetLteOnGsmMode() {
        return mCi->GetLteOnGsmMode();
    }

    /**
     * Sets the SIM voice message waiting indicator records.
     * @param line GSM Subscriber Profile Number, one-based. Only '1' is supported
     * @param countWaiting The number of messages waiting, if known. Use
     *                     -1 to indicate that an unknown number of
     *                      messages are waiting
     */
    //@Override
    CARAPI SetVoiceMessageWaiting(Int32 line, Int32 countWaiting) {
        // This function should be overridden by class GSMPhone and CDMAPhone.
        Rlog->E(LOG_TAG, "Error! This function should never be executed, inactive Phone.");
    }

    /**
     * Gets the USIM service table from the UICC, if present and available.
     * @return an interface to the UsimServiceTable record, or NULL if not available
     */
    //@Override
    public UsimServiceTable GetUsimServiceTable() {
        IccRecords r = mIccRecords->Get();
        Return (r != NULL) ? r->GetUsimServiceTable() : NULL;
    }

    /**
     * Gets the Uicc card corresponding to this phone.
     * @return the UiccCard object corresponding to the phone ID.
     */
    //@Override
    public UiccCard GetUiccCard() {
        return mUiccController->GetUiccCard(mPhoneId);
    }

    /**
     * Get P-CSCF address from PCO after data connection is established or modified.
     * @param apnType the apnType, "ims" for IMS APN, "emergency" for EMERGENCY APN
     */
    //@Override
    public String[] GetPcscfAddress(String apnType) {
        return mDcTracker->GetPcscfAddress(apnType);
    }

    /**
     * Set IMS registration state
     */
    //@Override
    CARAPI SetImsRegistrationState(Boolean registered) {
        mDcTracker->SetImsRegistrationState(registered);
    }

    /**
     * Return an instance of a IMS phone
     */
    //@Override
    public Phone GetImsPhone() {
        return mImsPhone;
    }

    //@Override
    public ImsPhone RelinquishOwnershipOfImsPhone() {
        Synchronized (mImsLock) {
            If (mImsPhone == NULL)
                return NULL;

            ImsPhone imsPhone = mImsPhone;
            mImsPhone = NULL;

            CallManager->GetInstance()->UnregisterPhone(imsPhone);
            imsPhone->UnregisterForSilentRedial(this);

            return imsPhone;
        }
    }

    //@Override
    CARAPI AcquireOwnershipOfImsPhone(ImsPhone imsPhone) {
        Synchronized (mImsLock) {
            If (imsPhone == NULL)
                return;

            If (mImsPhone != NULL) {
                Rlog->E(LOG_TAG, "acquireOwnershipOfImsPhone: non-NULL mImsPhone." +
                        " Shouldn't happen - but disposing");
                mImsPhone->Dispose();
                // Potential GC issue if someone keeps a reference to ImsPhone.
                // However: this change will make sure that such a reference does
                // not access functions through NULL pointer.
                //mImsPhone->RemoveReferences();
            }

            mImsPhone = imsPhone;

            mImsServiceReady = TRUE;
            mImsPhone->UpdateParentPhone(this);
            CallManager->GetInstance()->RegisterPhone(mImsPhone);
            mImsPhone->RegisterForSilentRedial(
                    this, EVENT_INITIATE_SILENT_REDIAL, NULL);
        }
    }

    protected void UpdateImsPhone() {
        Synchronized (mImsLock) {
            Rlog->D(LOG_TAG, "updateImsPhone"
                    + " mImsServiceReady=" + mImsServiceReady);

            If (mImsServiceReady && (mImsPhone == NULL)) {
                mImsPhone = PhoneFactory->MakeImsPhone(mNotifier, this);
                CallManager->GetInstance()->RegisterPhone(mImsPhone);
                mImsPhone->RegisterForSilentRedial(
                        this, EVENT_INITIATE_SILENT_REDIAL, NULL);
            } else If (!mImsServiceReady && (mImsPhone != NULL)) {
                CallManager->GetInstance()->UnregisterPhone(mImsPhone);
                mImsPhone->UnregisterForSilentRedial(this);

                mImsPhone->Dispose();
                // Potential GC issue if someone keeps a reference to ImsPhone.
                // However: this change will make sure that such a reference does
                // not access functions through NULL pointer.
                //mImsPhone->RemoveReferences();
                mImsPhone = NULL;
            }
        }
    }

    /**
     * Dials a number.
     *
     * @param dialString The number to dial.
     * @param uusInfo The UUSInfo.
     * @param videoState The video state for the call.
     * @return The Connection.
     * @throws CallStateException
     */
    protected Connection DialInternal(String dialString, UUSInfo uusInfo, Int32 videoState)
            throws CallStateException {
        // dialInternal shall be overriden by GSMPhone and CDMAPhone
        return NULL;
    }

    CARAPI Dump(FileDescriptor fd, PrintWriter pw, String[] args) {
        pw->Println("PhoneBase:");
        pw->Println(" mCi=" + mCi);
        pw->Println(" mDnsCheckDisabled=" + mDnsCheckDisabled);
        pw->Println(" mDcTracker=" + mDcTracker);
        pw->Println(" mDoesRilSendMultipleCallRing=" + mDoesRilSendMultipleCallRing);
        pw->Println(" mCallRingContinueToken=" + mCallRingContinueToken);
        pw->Println(" mCallRingDelay=" + mCallRingDelay);
        pw->Println(" mIsTheCurrentActivePhone=" + mIsTheCurrentActivePhone);
        pw->Println(" mIsVoiceCapable=" + mIsVoiceCapable);
        pw->Println(" mIccRecords=" + mIccRecords->Get());
        pw->Println(" mUiccApplication=" + mUiccApplication->Get());
        pw->Println(" mSmsStorageMonitor=" + mSmsStorageMonitor);
        pw->Println(" mSmsUsageMonitor=" + mSmsUsageMonitor);
        pw->Flush();
        pw->Println(" mLooper=" + mLooper);
        pw->Println(" mContext=" + mContext);
        pw->Println(" mNotifier=" + mNotifier);
        pw->Println(" mSimulatedRadioControl=" + mSimulatedRadioControl);
        pw->Println(" mUnitTestMode=" + mUnitTestMode);
        pw->Println(" IsDnsCheckDisabled()=" + IsDnsCheckDisabled());
        pw->Println(" GetUnitTestMode()=" + GetUnitTestMode());
        pw->Println(" GetState()=" + GetState());
        pw->Println(" GetIccSerialNumber()=" + GetIccSerialNumber());
        pw->Println(" GetIccRecordsLoaded()=" + GetIccRecordsLoaded());
        pw->Println(" GetMessageWaitingIndicator()=" + GetMessageWaitingIndicator());
        pw->Println(" GetCallForwardingIndicator()=" + GetCallForwardingIndicator());
        pw->Println(" IsInEmergencyCall()=" + IsInEmergencyCall());
        pw->Flush();
        pw->Println(" IsInEcm()=" + IsInEcm());
        pw->Println(" GetPhoneName()=" + GetPhoneName());
        pw->Println(" GetPhoneType()=" + GetPhoneType());
        pw->Println(" GetVoiceMessageCount()=" + GetVoiceMessageCount());
        pw->Println(" GetActiveApnTypes()=" + GetActiveApnTypes());
        pw->Println(" IsDataConnectivityPossible()=" + IsDataConnectivityPossible());
        pw->Println(" needsOtaServiceProvisioning=" + NeedsOtaServiceProvisioning());
    }

    /**
     * Returns the subscription id.
     */
    public Int64 GetSubId() {
        return SubscriptionController->GetInstance()->GetSubIdUsingPhoneId(mPhoneId);
    }

    /**
     * Returns the phone id.
     */
    public Int32 GetPhoneId() {
        return mPhoneId;
    }

    //Gets Subscription information in the Phone Object
    public Subscription GetSubscriptionInfo() {
        return mSubscriptionData;
    }

    /**
     * Return the service state of mImsPhone if it is STATE_IN_SERVICE
     * otherwise return the current voice service state
     */
    //@Override
    public Int32 GetVoicePhoneServiceState() {
        ImsPhone imsPhone = mImsPhone;
        If (imsPhone != NULL
                && imsPhone->GetServiceState()->GetState() == ServiceState.STATE_IN_SERVICE) {
            return ServiceState.STATE_IN_SERVICE;
        }
        return GetServiceState()->GetState();
    }

    //@Override
    public Boolean SetOperatorBrandOverride(String brand) {
        return FALSE;
    }

    //@Override
    public Boolean IsRadioAvailable() {
        return mCi->GetRadioState()->IsAvailable();
    }

    //@Override
    CARAPI ShutdownRadio() {
        GetServiceStateTracker()->RequestShutdown();
    }

    CARAPI GetCallBarringOption(String facility, String password, Message onComplete) {
        LogUnexpectedCdmaMethodCall("getCallBarringOption");
    }

    CARAPI SetCallBarringOption(String facility, Boolean lockState, String password,
            Message onComplete) {
        LogUnexpectedCdmaMethodCall("setCallBarringOption");
    }

    CARAPI RequestChangeCbPsw(String facility, String oldPwd, String newPwd, Message result) {
        LogUnexpectedCdmaMethodCall("requestChangeCbPsw");
    }

    //@Override
    CARAPI SetLocalCallHold(Int32 lchStatus) {
        mCi->SetLocalCallHold(lchStatus);
    }

    //@Override
    CARAPI
    DeflectCall(String number) throws CallStateException {
        throw new CallStateException("Unexpected deflectCall method call");
    }

    //@Override
    CARAPI AddParticipant(String dialString) throws CallStateException {
        throw new CallStateException("addParticipant is not supported in this phone "
                + this);
    }
}
