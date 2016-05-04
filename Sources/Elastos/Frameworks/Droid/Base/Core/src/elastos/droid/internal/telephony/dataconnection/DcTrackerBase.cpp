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

package com.android.internal.telephony.dataconnection;

using Elastos::Droid::App::IAlarmManager;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Database::IContentObserver;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Net::ILinkProperties;
using Elastos::Droid::Net::INetworkCapabilities;
using Elastos::Droid::Net::INetworkInfo;
using Elastos::Droid::Net::ITrafficStats;
using Elastos::Droid::Net::Wifi::IWifiManager;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IBuild;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IMessenger;
using Elastos::Droid::Os::ISystemClock;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Preference::IPreferenceManager;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Telephony::ISubscriptionManager;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Utility::IEventLog;
using Elastos::Droid::Telephony::IRlog;
using Elastos::Droid::Telephony::IServiceState;

using Elastos::Droid::Internal::IR;
using Elastos::Droid::Internal::Telephony::IDctConstants;
using Elastos::Droid::Internal::Telephony::DctConstants::IState;
using Elastos::Droid::Internal::Telephony::IEventLogTags;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Internal::Telephony::IPhoneBase;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Internal::Telephony::Uicc::IIccRecords;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccController;
using Elastos::Droid::Internal::Utility::IAsyncChannel;
using Elastos::Droid::Internal::Utility::IArrayUtils;

using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IComparator;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IList;
using Elastos::Utility::Map::IEntry;
using Elastos::Utility::ISet;
using Elastos::Utility::Concurrent::IConcurrentHashMap;
using Elastos::Utility::Concurrent::Atomic::IAtomicInteger;
using Elastos::Utility::Concurrent::Atomic::IAtomicReference;
using Elastos::Utility::IPriorityQueue;

/**
 * {@hide}
 */
public abstract class DcTrackerBase extends Handler {
    protected static const Boolean DBG = TRUE;
    protected static const Boolean VDBG = FALSE; // STOPSHIP if TRUE
    protected static const Boolean VDBG_STALL = TRUE; // STOPSHIP if TRUE
    protected static const Boolean RADIO_TESTS = FALSE;

    static Boolean mIsCleanupRequired = FALSE;
    /**
     * Constants for the data connection activity:
     * physical link down/up
     */
    protected static const Int32 DATA_CONNECTION_ACTIVE_PH_LINK_INACTIVE = 0;
    protected static const Int32 DATA_CONNECTION_ACTIVE_PH_LINK_DOWN = 1;
    protected static const Int32 DATA_CONNECTION_ACTIVE_PH_LINK_UP = 2;

    /** Delay between APN attempts.
        Note the property override mechanism is there just for testing purpose only. */
    protected static const Int32 APN_DELAY_DEFAULT_MILLIS = 20000;

    /** Delay between APN attempts when in fail fast mode */
    protected static const Int32 APN_FAIL_FAST_DELAY_DEFAULT_MILLIS = 3000;

    AlarmManager mAlarmManager;

    protected Object mDataEnabledLock = new Object();

    // responds to the setInternalDataEnabled call - used internally to turn off data
    // for example during emergency calls
    protected Boolean mInternalDataEnabled = TRUE;

    // responds to Public (user) API to enable/disable data use
    // independent of mInternalDataEnabled and requests for APN access
    // persisted
    protected Boolean mUserDataEnabled = TRUE;

    // TODO: move away from static state once 5587429 is fixed.
    protected static Boolean sPolicyDataEnabled = TRUE;

    private Boolean[] mDataEnabled = new Boolean[DctConstants.APN_NUM_TYPES];

    private Int32 mEnabledCount = 0;

    /* Currently requested APN Type (TODO: This should probably be a parameter not a member) */
    protected String mRequestedApnType = PhoneConstants.APN_TYPE_DEFAULT;

    /** Retry configuration: A doubling of retry times from 5secs to 30minutes */
    protected static const String DEFAULT_DATA_RETRY_CONFIG = "default_randomization=2000,"
        + "5000,10000,20000,40000,80000:5000,160000:5000,"
        + "320000:5000,640000:5000,1280000:5000,1800000:5000";

    /** Retry configuration for secondary networks: 4 tries in 20 sec */
    protected static const String SECONDARY_DATA_RETRY_CONFIG =
            "max_retries=3, 5000, 5000, 5000";

    /** Slow poll when attempting connection recovery. */
    protected static const Int32 POLL_NETSTAT_SLOW_MILLIS = 5000;
    /** Default max failure count before attempting to network re-registration. */
    protected static const Int32 DEFAULT_MAX_PDP_RESET_FAIL = 3;

    /**
     * After detecting a potential connection problem, this is the max number
     * of subsequent polls before attempting recovery.
     */
    protected static const Int32 NO_RECV_POLL_LIMIT = 24;
    // 1 sec. default polling interval when screen is on.
    protected static const Int32 POLL_NETSTAT_MILLIS = 1000;
    // 10 min. default polling interval when screen is off.
    protected static const Int32 POLL_NETSTAT_SCREEN_OFF_MILLIS = 1000*60*10;
    // 2 min for round trip time
    protected static const Int32 POLL_LONGEST_RTT = 120 * 1000;
    // Default sent packets without ack which triggers initial recovery steps
    protected static const Int32 NUMBER_SENT_PACKETS_OF_HANG = 10;
    // how Int64 to wait before switching back to default APN
    protected static const Int32 RESTORE_DEFAULT_APN_DELAY = 1 * 60 * 1000;
    // system property that can override the above value
    protected static const String APN_RESTORE_DELAY_PROP_NAME = "android.telephony.apn-restore";
    // represents an invalid IP address
    protected static const String NULL_IP = "0.0.0.0";

    // Default for the data stall alarm while non-aggressive stall detection
    protected static const Int32 DATA_STALL_ALARM_NON_AGGRESSIVE_DELAY_IN_MS_DEFAULT = 1000 * 60 * 6;
    // Default for the data stall alarm for aggressive stall detection
    protected static const Int32 DATA_STALL_ALARM_AGGRESSIVE_DELAY_IN_MS_DEFAULT = 1000 * 60;
    // If attempt is less than this value we're doing first level recovery
    protected static const Int32 DATA_STALL_NO_RECV_POLL_LIMIT = 1;
    // Tag for tracking stale alarms
    protected static const String DATA_STALL_ALARM_TAG_EXTRA = "data.stall.alram.tag";

    protected static const Boolean DATA_STALL_SUSPECTED = TRUE;
    protected static const Boolean DATA_STALL_NOT_SUSPECTED = FALSE;

    protected String RADIO_RESET_PROPERTY = "gsm.radioreset";

    protected static const String INTENT_RECONNECT_ALARM =
            "com.android.internal.telephony.data-reconnect";
    protected static const String INTENT_RECONNECT_ALARM_EXTRA_TYPE = "reconnect_alarm_extra_type";
    protected static const String INTENT_RECONNECT_ALARM_EXTRA_REASON =
            "reconnect_alarm_extra_reason";

    protected static const String INTENT_RESTART_TRYSETUP_ALARM =
            "com.android.internal.telephony.data-restart-trysetup";
    protected static const String INTENT_RESTART_TRYSETUP_ALARM_EXTRA_TYPE =
            "restart_trysetup_alarm_extra_type";

    protected static const String INTENT_DATA_STALL_ALARM =
            "com.android.internal.telephony.data-stall";



    protected static const String DEFALUT_DATA_ON_BOOT_PROP = "net.def_data_on_boot";

    protected DcTesterFailBringUpAll mDcTesterFailBringUpAll;
    protected DcController mDcc;

    // member variables
    protected PhoneBase mPhone;
    protected UiccController mUiccController;
    protected AtomicReference<IccRecords> mIccRecords = new AtomicReference<IccRecords>();
    protected DctConstants.Activity mActivity = DctConstants.Activity.NONE;
    protected DctConstants.State mState = DctConstants.State.IDLE;
    protected Handler mDataConnectionTracker = NULL;

    protected Int64 mTxPkts;
    protected Int64 mRxPkts;
    protected Int32 mNetStatPollPeriod;
    protected Boolean mNetStatPollEnabled = FALSE;

    protected TxRxSum mDataStallTxRxSum = new TxRxSum(0, 0);
    // Used to track stale data stall alarms.
    protected Int32 mDataStallAlarmTag = (Int32) SystemClock->ElapsedRealtime();
    // The current data stall alarm intent
    protected PendingIntent mDataStallAlarmIntent = NULL;
    // Number of packets sent since the last received packet
    protected Int64 mSentSinceLastRecv;
    // Controls when a simple recovery attempt it to be tried
    protected Int32 mNoRecvPollCount = 0;
    // Refrence counter for enabling fail fast
    protected static Int32 sEnableFailFastRefCounter = 0;
    // True if data stall detection is enabled
    protected volatile Boolean mDataStallDetectionEnabled = TRUE;

    protected volatile Boolean mFailFast = FALSE;

    // True when in voice call
    protected Boolean mInVoiceCall = FALSE;

    // wifi connection status will be updated by sticky intent
    protected Boolean mIsWifiConnected = FALSE;

    /** Intent sent when the reconnect alarm fires. */
    protected PendingIntent mReconnectIntent = NULL;

    /** CID of active data connection */
    protected Int32 mCidActive;

    // When FALSE we will not auto attach and manually attaching is required.
    protected Boolean mAutoAttachOnCreationConfig = FALSE;
    protected Boolean mAutoAttachOnCreation = FALSE;

    // State of screen
    // (TODO: Reconsider tying directly to screen, maybe this is
    //        really a lower power mode")
    protected Boolean mIsScreenOn = TRUE;

    /** Allows the generation of unique Id's for DataConnection objects */
    protected AtomicInteger mUniqueIdGenerator = new AtomicInteger(0);

    /** The data connections. */
    protected HashMap<Integer, DataConnection> mDataConnections =
        new HashMap<Integer, DataConnection>();

    /** The data connection async channels */
    protected HashMap<Integer, DcAsyncChannel> mDataConnectionAcHashMap =
        new HashMap<Integer, DcAsyncChannel>();

    /** Convert an ApnType string to Id (TODO: Use "enumeration" instead of String for ApnType) */
    protected HashMap<String, Integer> mApnToDataConnectionId =
                                    new HashMap<String, Integer>();

    /** Phone.APN_TYPE_* ===> ApnContext */
    protected final ConcurrentHashMap<String, ApnContext> mApnContexts =
                                    new ConcurrentHashMap<String, ApnContext>();

    /** kept in sync with mApnContexts
     * Higher numbers are higher priority and sorted so highest priority is first */
    protected final PriorityQueue<ApnContext>mPrioritySortedApnContexts =
            new PriorityQueue<ApnContext>(5,
            new Comparator<ApnContext>() {
                public Int32 Compare(ApnContext c1, ApnContext c2) {
                    return c2.priority - c1.priority;
                }
            } );

    /* Currently active APN */
    protected ApnSetting mActiveApn;

    /** allApns holds all apns */
    protected ArrayList<ApnSetting> mAllApnSettings = new ArrayList<ApnSetting>();

    /** preferred apn */
    protected ApnSetting mPreferredApn = NULL;

    /** Is packet service restricted by network */
    protected Boolean mIsPsRestricted = FALSE;

    /** emergency apn Setting*/
    protected ApnSetting mEmergencyApn = NULL;

    /* Once disposed dont handle any messages */
    protected Boolean mIsDisposed = FALSE;

    protected ContentResolver mResolver;

    /* Set to TRUE with CMD_ENABLE_MOBILE_PROVISIONING */
    protected Boolean mIsProvisioning = FALSE;

    /* The Url passed as object parameter in CMD_ENABLE_MOBILE_PROVISIONING */
    protected String mProvisioningUrl = NULL;

    /* Intent for the provisioning apn alarm */
    protected static const String INTENT_PROVISIONING_APN_ALARM =
            "com.android.internal.telephony.provisioning_apn_alarm";

    /* Tag for tracking stale alarms */
    protected static const String PROVISIONING_APN_ALARM_TAG_EXTRA = "provisioning.apn.alarm.tag";

    /* Debug property for overriding the PROVISIONING_APN_ALARM_DELAY_IN_MS */
    protected static const String DEBUG_PROV_APN_ALARM =
            "persist.debug.prov_apn_alarm";

    /* Default for the provisioning apn alarm timeout */
    protected static const Int32 PROVISIONING_APN_ALARM_DELAY_IN_MS_DEFAULT = 1000 * 60 * 15;

    /* The provision apn alarm intent used to disable the provisioning apn */
    protected PendingIntent mProvisioningApnAlarmIntent = NULL;

    /* Used to track stale provisioning apn alarms */
    protected Int32 mProvisioningApnAlarmTag = (Int32) SystemClock->ElapsedRealtime();

    protected AsyncChannel mReplyAc = new AsyncChannel();

    protected BroadcastReceiver mIntentReceiver = new BroadcastReceiver ()
    {
        //@Override
        CARAPI OnReceive(Context context, Intent intent)
        {
            String action = intent->GetAction();
            If (DBG) Log("onReceive: action=" + action);
            If (action->Equals(IIntent::ACTION_SCREEN_ON)) {
                mIsScreenOn = TRUE;
                StopNetStatPoll();
                StartNetStatPoll();
                RestartDataStallAlarm();
            } else If (action->Equals(IIntent::ACTION_SCREEN_OFF)) {
                mIsScreenOn = FALSE;
                StopNetStatPoll();
                StartNetStatPoll();
                RestartDataStallAlarm();
            } else If (action->StartsWith(INTENT_RECONNECT_ALARM)) {
                If (DBG) Log("Reconnect alarm. Previous state was " + mState);
                OnActionIntentReconnectAlarm(intent);
            } else If (action->StartsWith(INTENT_RESTART_TRYSETUP_ALARM)) {
                If (DBG) Log("Restart trySetup alarm");
                OnActionIntentRestartTrySetupAlarm(intent);
            } else If (action->Equals(INTENT_DATA_STALL_ALARM)) {
                OnActionIntentDataStallAlarm(intent);
            } else If (action->Equals(INTENT_PROVISIONING_APN_ALARM)) {
                OnActionIntentProvisioningApnAlarm(intent);
            } else If (action->Equals(WifiManager.NETWORK_STATE_CHANGED_ACTION)) {
                final android.net.NetworkInfo networkInfo = (NetworkInfo)
                        intent->GetParcelableExtra(WifiManager.EXTRA_NETWORK_INFO);
                mIsWifiConnected = (networkInfo != NULL && networkInfo->IsConnected());
                If (DBG) Log("NETWORK_STATE_CHANGED_ACTION: mIsWifiConnected=" + mIsWifiConnected);
            } else If (action->Equals(WifiManager.WIFI_STATE_CHANGED_ACTION)) {
                final Boolean enabled = intent->GetIntExtra(WifiManager.EXTRA_WIFI_STATE,
                        WifiManager.WIFI_STATE_UNKNOWN) == WifiManager.WIFI_STATE_ENABLED;

                If (!enabled) {
                    // when WiFi got disabled, the NETWORK_STATE_CHANGED_ACTION
                    // quit and won't report disconnected until next enabling.
                    mIsWifiConnected = FALSE;
                }
                If (DBG) Log("WIFI_STATE_CHANGED_ACTION: enabled=" + enabled
                        + " mIsWifiConnected=" + mIsWifiConnected);
            }
        }
    };

    private Runnable mPollNetStat = new Runnable()
    {
        //@Override
        CARAPI Run() {
            UpdateDataActivity();

            If (mIsScreenOn) {
                mNetStatPollPeriod = Settings::Global::>GetInt(mResolver,
                        Settings::Global::PDP_WATCHDOG_POLL_INTERVAL_MS, POLL_NETSTAT_MILLIS);
            } else {
                mNetStatPollPeriod = Settings::Global::>GetInt(mResolver,
                        Settings::Global::PDP_WATCHDOG_LONG_POLL_INTERVAL_MS,
                        POLL_NETSTAT_SCREEN_OFF_MILLIS);
            }

            If (mNetStatPollEnabled) {
                mDataConnectionTracker->PostDelayed(this, mNetStatPollPeriod);
            }
        }
    };

    private class DataRoamingSettingObserver extends ContentObserver {

        public DataRoamingSettingObserver(Handler handler, Context context) {
            Super(handler);
            mResolver = context->GetContentResolver();
        }

        CARAPI Register() {
            mResolver->RegisterContentObserver(
                    Settings::Global::>GetUriFor(Settings::Global::DATA_ROAMING +
                    mPhone->GetPhoneId()), FALSE, this);
        }

        CARAPI Unregister() {
            mResolver->UnregisterContentObserver(this);
        }

        //@Override
        CARAPI OnChange(Boolean selfChange) {
            // already running on mPhone handler thread
            If (mPhone->GetServiceState()->GetRoaming()) {
                SendMessage(ObtainMessage(DctConstants.EVENT_ROAMING_ON));
            }
        }
    }
    private final DataRoamingSettingObserver mDataRoamingSettingObserver;

    /**
     * The Initial MaxRetry sent to a DataConnection as a parameter
     * to DataConnectionAc.bringUp. This value can be defined at compile
     * time using the SystemProperty Settings::Global::DCT_INITIAL_MAX_RETRY
     * and at runtime using gservices to change Settings::Global::DCT_INITIAL_MAX_RETRY.
     */
    private static const Int32 DEFAULT_MDC_INITIAL_RETRY = 1;
    protected Int32 GetInitialMaxRetry() {
        If (mFailFast) {
            return 0;
        }
        // Get default value from system property or use DEFAULT_MDC_INITIAL_RETRY
        Int32 value = SystemProperties->GetInt(
                Settings::Global::MDC_INITIAL_MAX_RETRY, DEFAULT_MDC_INITIAL_RETRY);

        // Check if its been overridden
        return Settings::Global::>GetInt(mResolver,
                Settings::Global::MDC_INITIAL_MAX_RETRY, value);
    }

    /**
     * Maintain the sum of transmit and receive packets.
     *
     * The packet counts are initialized and reset to -1 and
     * remain -1 until they can be updated.
     */
    public class TxRxSum {
        public Int64 txPkts;
        public Int64 rxPkts;

        public TxRxSum() {
            Reset();
        }

        public TxRxSum(Int64 txPkts, Int64 rxPkts) {
            this.txPkts = txPkts;
            this.rxPkts = rxPkts;
        }

        public TxRxSum(TxRxSum sum) {
            txPkts = sum.txPkts;
            rxPkts = sum.rxPkts;
        }

        CARAPI Reset() {
            txPkts = -1;
            rxPkts = -1;
        }

        //@Override
        CARAPI ToString(
        /* [out] */ String* str)
    {
            return "{txSum=" + txPkts + " rxSum=" + rxPkts + "}";
        }

        CARAPI UpdateTxRxSum() {
            this.txPkts = TrafficStats->GetMobileTcpTxPackets();
            this.rxPkts = TrafficStats->GetMobileTcpRxPackets();
        }
    }

    protected void OnActionIntentReconnectAlarm(Intent intent) {
        String reason = intent->GetStringExtra(INTENT_RECONNECT_ALARM_EXTRA_REASON);
        String apnType = intent->GetStringExtra(INTENT_RECONNECT_ALARM_EXTRA_TYPE);

        Int64 phoneSubId = mPhone->GetSubId();
        Int64 currSubId = intent->GetLongExtra(PhoneConstants.SUBSCRIPTION_KEY,
                SubscriptionManager.INVALID_SUB_ID);
        Log("onActionIntentReconnectAlarm: currSubId = " + currSubId + " phoneSubId=" + phoneSubId);

        // Stop reconnect if not current subId is not correct.
        // FIXME STOPSHIP - phoneSubId is coming up as -1 way after boot and failing this.
//        If ((currSubId == SubscriptionManager.INVALID_SUB_ID) || (currSubId != phoneSubId)) {
//            Log("receive ReconnectAlarm but subId incorrect, ignore");
//            return;
//        }

        ApnContext apnContext = mApnContexts->Get(apnType);

        If (DBG) {
            Log("onActionIntentReconnectAlarm: mState=" + mState + " reason=" + reason +
                    " apnType=" + apnType + " apnContext=" + apnContext +
                    " mDataConnectionAsyncChannels=" + mDataConnectionAcHashMap);
        }

        If ((apnContext != NULL) && (apnContext->IsEnabled())) {
            apnContext->SetReason(reason);
            DctConstants.State apnContextState = apnContext->GetState();
            If (DBG) {
                Log("onActionIntentReconnectAlarm: apnContext state=" + apnContextState);
            }
            If ((apnContextState == DctConstants.State.FAILED)
                    || (apnContextState == DctConstants.State.IDLE)) {
                If (DBG) {
                    Log("onActionIntentReconnectAlarm: state is FAILED|IDLE, disassociate");
                }
                DcAsyncChannel dcac = apnContext->GetDcAc();
                If (dcac != NULL) {
                    dcac->TearDown(apnContext, "", NULL);
                }
                apnContext->SetDataConnectionAc(NULL);
                apnContext->SetState(DctConstants.State.IDLE);
            } else {
                If (DBG) Log("onActionIntentReconnectAlarm: keep associated");
            }
            // TODO: IF already associated should we send the EVENT_TRY_SETUP_DATA???
            SendMessage(ObtainMessage(DctConstants.EVENT_TRY_SETUP_DATA, apnContext));

            apnContext->SetReconnectIntent(NULL);
        }
    }

    protected void OnActionIntentRestartTrySetupAlarm(Intent intent) {
        String apnType = intent->GetStringExtra(INTENT_RESTART_TRYSETUP_ALARM_EXTRA_TYPE);
        ApnContext apnContext = mApnContexts->Get(apnType);
        If (DBG) {
            Log("onActionIntentRestartTrySetupAlarm: mState=" + mState +
                    " apnType=" + apnType + " apnContext=" + apnContext +
                    " mDataConnectionAsyncChannels=" + mDataConnectionAcHashMap);
        }
        SendMessage(ObtainMessage(DctConstants.EVENT_TRY_SETUP_DATA, apnContext));
    }

    protected void OnActionIntentDataStallAlarm(Intent intent) {
        If (VDBG_STALL) Log("onActionIntentDataStallAlarm: action=" + intent->GetAction());
        Message msg = ObtainMessage(DctConstants.EVENT_DATA_STALL_ALARM,
                intent->GetAction());
        msg.arg1 = intent->GetIntExtra(DATA_STALL_ALARM_TAG_EXTRA, 0);
        SendMessage(msg);
    }

    ConnectivityManager mCm;

    /**
     * Default constructor
     */
    protected DcTrackerBase(PhoneBase phone) {
        Super();
        mPhone = phone;
        If (DBG) Log("DCT.constructor");
        mResolver = mPhone->GetContext()->GetContentResolver();
        mUiccController = UiccController->GetInstance();
        mUiccController->RegisterForIccChanged(this, DctConstants.EVENT_ICC_CHANGED, NULL);
        mAlarmManager =
                (AlarmManager) mPhone->GetContext()->GetSystemService(Context.ALARM_SERVICE);
        mCm = (ConnectivityManager) mPhone->GetContext()->GetSystemService(
                Context.CONNECTIVITY_SERVICE);


        IntentFilter filter = new IntentFilter();
        filter->AddAction(IIntent::ACTION_SCREEN_ON);
        filter->AddAction(IIntent::ACTION_SCREEN_OFF);
        filter->AddAction(WifiManager.NETWORK_STATE_CHANGED_ACTION);
        filter->AddAction(WifiManager.WIFI_STATE_CHANGED_ACTION);
        filter->AddAction(INTENT_DATA_STALL_ALARM);
        filter->AddAction(INTENT_PROVISIONING_APN_ALARM);

        mUserDataEnabled = Settings::Global::>GetInt(mPhone->GetContext()->GetContentResolver(),
                Settings::Global::MOBILE_DATA + mPhone->GetPhoneId(), 1) == 1;

        mPhone->GetContext()->RegisterReceiver(mIntentReceiver, filter, NULL, mPhone);

        // This preference tells us 1) initial condition for "dataEnabled",
        // and 2) whether the RIL will setup the baseband to auto-PS attach.

        mDataEnabled[DctConstants.APN_DEFAULT_ID] =
                SystemProperties->GetBoolean(DEFALUT_DATA_ON_BOOT_PROP,TRUE);
        If (mDataEnabled[DctConstants.APN_DEFAULT_ID]) {
            mEnabledCount++;
        }

        SharedPreferences sp = PreferenceManager->GetDefaultSharedPreferences(mPhone->GetContext());
        mAutoAttachOnCreation = sp->GetBoolean(PhoneBase.DATA_DISABLED_ON_BOOT_KEY, FALSE);

        // Watch for changes to Settings::Global::DATA_ROAMING
        mDataRoamingSettingObserver = new DataRoamingSettingObserver(mPhone, mPhone->GetContext());
        mDataRoamingSettingObserver->Register();

        HandlerThread dcHandlerThread = new HandlerThread("DcHandlerThread");
        dcHandlerThread->Start();
        Handler dcHandler = new Handler(dcHandlerThread->GetLooper());
        mDcc = DcController->MakeDcc(mPhone, this, dcHandler);
        mDcTesterFailBringUpAll = new DcTesterFailBringUpAll(mPhone, dcHandler);
    }

    CARAPI Dispose() {
        If (DBG) Log("DCT.dispose");
        For (DcAsyncChannel dcac : mDataConnectionAcHashMap->Values()) {
            dcac->Disconnect();
        }
        mDataConnectionAcHashMap->Clear();
        mIsDisposed = TRUE;
        mPhone->GetContext()->UnregisterReceiver(mIntentReceiver);
        mUiccController->UnregisterForIccChanged(this);
        mDataRoamingSettingObserver->Unregister();
        mDcc->Dispose();
        mDcTesterFailBringUpAll->Dispose();
    }

    public DctConstants.Activity GetActivity() {
        return mActivity;
    }

    void SetActivity(DctConstants.Activity activity) {
        Log("setActivity = " + activity);
        mActivity = activity;
        mPhone->NotifyDataActivity();
    }

    public Boolean IsApnTypeActive(String type) {
        // TODO: support simultaneous with List instead
        If (PhoneConstants.APN_TYPE_DUN->Equals(type)) {
            ApnSetting dunApn = FetchDunApn();
            If (dunApn != NULL) {
                Return ((mActiveApn != NULL) && (dunApn->ToString()->Equals(mActiveApn->ToString())));
            }
        }
        return mActiveApn != NULL && mActiveApn->CanHandleType(type);
    }

    protected ApnSetting FetchDunApn() {
        If (SystemProperties->GetBoolean("net.tethering.noprovisioning", FALSE)) {
            Log("fetchDunApn: net.tethering.noprovisioning=TRUE ret: NULL");
            return NULL;
        }
        Int32 bearer = -1;
        Context c = mPhone->GetContext();
        String apnData = Settings::Global::>GetString(c->GetContentResolver(),
                Settings::Global::TETHER_DUN_APN);
        List<ApnSetting> dunSettings = ApnSetting->ArrayFromString(apnData);
        For (ApnSetting dunSetting : dunSettings) {
            IccRecords r = mIccRecords->Get();
            String operator = (r != NULL) ? r->GetOperatorNumeric() : "";
            If (dunSetting.bearer != 0) {
                If (bearer == -1) bearer = mPhone->GetServiceState()->GetRilDataRadioTechnology();
                If (dunSetting.bearer != bearer) continue;
            }
            If (dunSetting.numeric->Equals(operator)) {
                If (dunSetting->HasMvnoParams()) {
                    If (r != NULL &&
                            MvnoMatches(r, dunSetting.mvnoType, dunSetting.mvnoMatchData)) {
                        If (VDBG) {
                            Log("fetchDunApn: global TETHER_DUN_APN dunSetting=" + dunSetting);
                        }
                        return dunSetting;
                    }
                } else {
                    If (VDBG) Log("fetchDunApn: global TETHER_DUN_APN dunSetting=" + dunSetting);
                    return dunSetting;
                }
            }
        }

        apnData = c->GetResources()->GetString(R::string::config_tether_apndata);
        ApnSetting dunSetting = ApnSetting->FromString(apnData);
        If (VDBG) Log("fetchDunApn: config_tether_apndata dunSetting=" + dunSettings);
        return dunSetting;
    }

    public String[] GetActiveApnTypes() {
        String[] result;
        If (mActiveApn != NULL) {
            result = mActiveApn.types;
        } else {
            result = new String[1];
            result[0] = PhoneConstants.APN_TYPE_DEFAULT;
        }
        return result;
    }

    /** TODO: See if we can remove */
    public String GetActiveApnString(String apnType) {
        String result = NULL;
        If (mActiveApn != NULL) {
            result = mActiveApn.apn;
        }
        return result;
    }

    /**
     * Modify {@link android.provider.Settings::Global::DATA_ROAMING} value.
     */
    CARAPI SetDataOnRoamingEnabled(Boolean enabled) {
        If (GetDataOnRoamingEnabled() != enabled) {
            final ContentResolver resolver = mPhone->GetContext()->GetContentResolver();
            Settings::Global::>PutInt(resolver,
                    Settings::Global::DATA_ROAMING + mPhone->GetPhoneId(), enabled ? 1 : 0);
            // will trigger HandleDataOnRoamingChange() through observer
        }
    }

    /**
     * Return current {@link android.provider.Settings::Global::DATA_ROAMING} value.
     */
    public Boolean GetDataOnRoamingEnabled() {
        try {
            final ContentResolver resolver = mPhone->GetContext()->GetContentResolver();
            return Settings::Global::>GetInt(resolver,
                    Settings::Global::DATA_ROAMING + mPhone->GetPhoneId()) != 0;
        } Catch (SettingNotFoundException snfe) {
            return FALSE;
        }
    }

    /**
     * Modify {@link android.provider.Settings::Global::MOBILE_DATA} value.
     */
    CARAPI SetDataEnabled(Boolean enable) {
        Message msg = ObtainMessage(DctConstants.CMD_SET_USER_DATA_ENABLE);
        msg.arg1 = enable ? 1 : 0;
        SendMessage(msg);
    }

    /**
     * Return current {@link android.provider.Settings::Global::MOBILE_DATA} value.
     */
    public Boolean GetDataEnabled() {
        try {
            final ContentResolver resolver = mPhone->GetContext()->GetContentResolver();
            return Settings::Global::>GetInt(resolver,
                    Settings::Global::MOBILE_DATA + mPhone->GetPhoneId()) != 0;
        } Catch (SettingNotFoundException snfe) {
            return FALSE;
        }
    }

    // abstract methods
    protected abstract void RestartRadio();
    protected abstract void Log(String s);
    protected abstract void Loge(String s);
    protected abstract Boolean IsDataAllowed();
    protected abstract Boolean IsApnTypeAvailable(String type);
    public    abstract DctConstants.State GetState(String apnType);
    protected abstract Boolean IsProvisioningApn(String apnType);
    protected abstract void SetState(DctConstants.State s);
    protected abstract void GotoIdleAndNotifyDataConnection(String reason);

    protected abstract Boolean OnTrySetupData(String reason);
    protected abstract void OnRoamingOff();
    protected abstract void OnRoamingOn();
    protected abstract void OnRadioAvailable();
    protected abstract void OnRadioOffOrNotAvailable();
    protected abstract void OnDataSetupComplete(AsyncResult ar);
    protected abstract void OnDataSetupCompleteError(AsyncResult ar);
    protected abstract void OnDisconnectDone(Int32 connId, AsyncResult ar);
    protected abstract void OnDisconnectDcRetrying(Int32 connId, AsyncResult ar);
    protected abstract void OnVoiceCallStarted();
    protected abstract void OnVoiceCallEnded();
    protected abstract void OnCleanUpConnection(Boolean tearDown, Int32 apnId, String reason);
    protected abstract void OnCleanUpAllConnections(String cause);
    public abstract Boolean IsDataPossible(String apnType);
    public abstract Boolean IsOnDemandDataPossible(String apnType);
    protected abstract Boolean OnUpdateIcc();
    protected abstract void CompleteConnection(ApnContext apnContext);
    public abstract void SetDataAllowed(Boolean enable, Message response);
    public abstract String[] GetPcscfAddress(String apnType);
    public abstract void SetImsRegistrationState(Boolean registered);
    protected abstract Boolean MvnoMatches(IccRecords r, String mvno_type, String mvno_match_data);
    protected abstract Boolean IsPermanentFail(DcFailCause dcFailCause);

    //@Override
    CARAPI HandleMessage(Message msg) {
        Switch (msg.what) {
            case AsyncChannel.CMD_CHANNEL_DISCONNECTED: {
                Log("DISCONNECTED_CONNECTED: msg=" + msg);
                DcAsyncChannel dcac = (DcAsyncChannel) msg.obj;
                mDataConnectionAcHashMap->Remove(dcac->GetDataConnectionIdSync());
                dcac->Disconnected();
                break;
            }
            case DctConstants.EVENT_ENABLE_NEW_APN:
                OnEnableApn(msg.arg1, msg.arg2);
                break;

            case DctConstants.EVENT_TRY_SETUP_DATA:
                String reason = NULL;
                If (msg.obj instanceof String) {
                    reason = (String) msg.obj;
                }
                OnTrySetupData(reason);
                break;

            case DctConstants.EVENT_DATA_STALL_ALARM:
                OnDataStallAlarm(msg.arg1);
                break;

            case DctConstants.EVENT_ROAMING_OFF:
                OnRoamingOff();
                break;

            case DctConstants.EVENT_ROAMING_ON:
                OnRoamingOn();
                break;

            case DctConstants.EVENT_RADIO_AVAILABLE:
                OnRadioAvailable();
                break;

            case DctConstants.EVENT_RADIO_OFF_OR_NOT_AVAILABLE:
                OnRadioOffOrNotAvailable();
                break;

            case DctConstants.EVENT_DATA_SETUP_COMPLETE:
                mCidActive = msg.arg1;
                OnDataSetupComplete((AsyncResult) msg.obj);
                break;

            case DctConstants.EVENT_DATA_SETUP_COMPLETE_ERROR:
                OnDataSetupCompleteError((AsyncResult) msg.obj);
                break;

            case DctConstants.EVENT_DISCONNECT_DONE:
                Log("DataConnectionTracker.handleMessage: EVENT_DISCONNECT_DONE msg=" + msg);
                OnDisconnectDone(msg.arg1, (AsyncResult) msg.obj);
                break;

            case DctConstants.EVENT_DISCONNECT_DC_RETRYING:
                Log("DataConnectionTracker.handleMessage: EVENT_DISCONNECT_DC_RETRYING msg=" + msg);
                OnDisconnectDcRetrying(msg.arg1, (AsyncResult) msg.obj);
                break;

            case DctConstants.EVENT_VOICE_CALL_STARTED:
                OnVoiceCallStarted();
                break;

            case DctConstants.EVENT_VOICE_CALL_ENDED:
                OnVoiceCallEnded();
                break;

            case DctConstants.EVENT_CLEAN_UP_ALL_CONNECTIONS: {
                OnCleanUpAllConnections((String) msg.obj);
                break;
            }
            case DctConstants.EVENT_CLEAN_UP_CONNECTION: {
                Boolean tearDown = (msg.arg1 == 0) ? FALSE : TRUE;
                OnCleanUpConnection(tearDown, msg.arg2, (String) msg.obj);
                break;
            }
            case DctConstants.EVENT_SET_INTERNAL_DATA_ENABLE: {
                Boolean enabled = (msg.arg1 == DctConstants.ENABLED) ? TRUE : FALSE;
                OnSetInternalDataEnabled(enabled);
                break;
            }
            case DctConstants.EVENT_RESET_DONE: {
                If (DBG) Log("EVENT_RESET_DONE");
                OnResetDone((AsyncResult) msg.obj);
                break;
            }
            case DctConstants.CMD_SET_USER_DATA_ENABLE: {
                final Boolean enabled = (msg.arg1 == DctConstants.ENABLED) ? TRUE : FALSE;
                If (DBG) Log("CMD_SET_USER_DATA_ENABLE enabled=" + enabled);
                OnSetUserDataEnabled(enabled);
                break;
            }
            case DctConstants.CMD_SET_DEPENDENCY_MET: {
                Boolean met = (msg.arg1 == DctConstants.ENABLED) ? TRUE : FALSE;
                If (DBG) Log("CMD_SET_DEPENDENCY_MET met=" + met);
                Bundle bundle = msg->GetData();
                If (bundle != NULL) {
                    String apnType = (String)bundle->Get(DctConstants.APN_TYPE_KEY);
                    If (apnType != NULL) {
                        OnSetDependencyMet(apnType, met);
                    }
                }
                break;
            }
            case DctConstants.CMD_SET_POLICY_DATA_ENABLE: {
                final Boolean enabled = (msg.arg1 == DctConstants.ENABLED) ? TRUE : FALSE;
                OnSetPolicyDataEnabled(enabled);
                break;
            }
            case DctConstants.CMD_SET_ENABLE_FAIL_FAST_MOBILE_DATA: {
                sEnableFailFastRefCounter += (msg.arg1 == DctConstants.ENABLED) ? 1 : -1;
                If (DBG) {
                    Log("CMD_SET_ENABLE_FAIL_FAST_MOBILE_DATA: "
                            + " sEnableFailFastRefCounter=" + sEnableFailFastRefCounter);
                }
                If (sEnableFailFastRefCounter < 0) {
                    final String s = "CMD_SET_ENABLE_FAIL_FAST_MOBILE_DATA: "
                            + "sEnableFailFastRefCounter:" + sEnableFailFastRefCounter + " < 0";
                    Loge(s);
                    sEnableFailFastRefCounter = 0;
                }
                final Boolean enabled = sEnableFailFastRefCounter > 0;
                If (DBG) {
                    Log("CMD_SET_ENABLE_FAIL_FAST_MOBILE_DATA: enabled=" + enabled
                            + " sEnableFailFastRefCounter=" + sEnableFailFastRefCounter);
                }
                If (mFailFast != enabled) {
                    mFailFast = enabled;
                    mDataStallDetectionEnabled = !enabled;
                    If (mDataStallDetectionEnabled
                            && (GetOverallState() == DctConstants.State.CONNECTED)
                            && (!mInVoiceCall ||
                                    mPhone->GetServiceStateTracker()
                                        .IsConcurrentVoiceAndDataAllowed())) {
                        If (DBG) Log("CMD_SET_ENABLE_FAIL_FAST_MOBILE_DATA: start data stall");
                        StopDataStallAlarm();
                        StartDataStallAlarm(DATA_STALL_NOT_SUSPECTED);
                    } else {
                        If (DBG) Log("CMD_SET_ENABLE_FAIL_FAST_MOBILE_DATA: stop data stall");
                        StopDataStallAlarm();
                    }
                }

                break;
            }
            case DctConstants.CMD_ENABLE_MOBILE_PROVISIONING: {
                Bundle bundle = msg->GetData();
                If (bundle != NULL) {
                    try {
                        mProvisioningUrl = (String)bundle->Get(DctConstants.PROVISIONING_URL_KEY);
                    } Catch(ClassCastException e) {
                        Loge("CMD_ENABLE_MOBILE_PROVISIONING: provisioning url not a string" + e);
                        mProvisioningUrl = NULL;
                    }
                }
                If (TextUtils->IsEmpty(mProvisioningUrl)) {
                    Loge("CMD_ENABLE_MOBILE_PROVISIONING: provisioning url is empty, ignoring");
                    mIsProvisioning = FALSE;
                    mProvisioningUrl = NULL;
                } else {
                    Loge("CMD_ENABLE_MOBILE_PROVISIONING: provisioningUrl=" + mProvisioningUrl);
                    mIsProvisioning = TRUE;
                    StartProvisioningApnAlarm();
                }
                break;
            }
            case DctConstants.EVENT_PROVISIONING_APN_ALARM: {
                If (DBG) Log("EVENT_PROVISIONING_APN_ALARM");
                ApnContext apnCtx = mApnContexts->Get("default");
                If (apnCtx->IsProvisioningApn() && apnCtx->IsConnectedOrConnecting()) {
                    If (mProvisioningApnAlarmTag == msg.arg1) {
                        If (DBG) Log("EVENT_PROVISIONING_APN_ALARM: Disconnecting");
                        mIsProvisioning = FALSE;
                        mProvisioningUrl = NULL;
                        StopProvisioningApnAlarm();
                        SendCleanUpConnection(TRUE, apnCtx);
                    } else {
                        If (DBG) {
                            Log("EVENT_PROVISIONING_APN_ALARM: ignore stale tag,"
                                    + " mProvisioningApnAlarmTag:" + mProvisioningApnAlarmTag
                                    + " != arg1:" + msg.arg1);
                        }
                    }
                } else {
                    If (DBG) Log("EVENT_PROVISIONING_APN_ALARM: Not connected ignore");
                }
                break;
            }
            case DctConstants.CMD_IS_PROVISIONING_APN: {
                If (DBG) Log("CMD_IS_PROVISIONING_APN");
                Boolean isProvApn;
                try {
                    String apnType = NULL;
                    Bundle bundle = msg->GetData();
                    If (bundle != NULL) {
                        apnType = (String)bundle->Get(DctConstants.APN_TYPE_KEY);
                    }
                    If (TextUtils->IsEmpty(apnType)) {
                        Loge("CMD_IS_PROVISIONING_APN: apnType is empty");
                        isProvApn = FALSE;
                    } else {
                        isProvApn = IsProvisioningApn(apnType);
                    }
                } Catch (ClassCastException e) {
                    Loge("CMD_IS_PROVISIONING_APN: NO provisioning url ignoring");
                    isProvApn = FALSE;
                }
                If (DBG) Log("CMD_IS_PROVISIONING_APN: ret=" + isProvApn);
                mReplyAc->ReplyToMessage(msg, DctConstants.CMD_IS_PROVISIONING_APN,
                        isProvApn ? DctConstants.ENABLED : DctConstants.DISABLED);
                break;
            }
            case DctConstants.EVENT_ICC_CHANGED: {
                OnUpdateIcc();
                break;
            }
            case DctConstants.EVENT_RESTART_RADIO: {
                RestartRadio();
                break;
            }
            case DctConstants.CMD_NET_STAT_POLL: {
                If (msg.arg1 == DctConstants.ENABLED) {
                    HandleStartNetStatPoll((DctConstants.Activity)msg.obj);
                } else If (msg.arg1 == DctConstants.DISABLED) {
                    HandleStopNetStatPoll((DctConstants.Activity)msg.obj);
                }
                break;
            }
            default:
                Rlog->E("DATA", "Unidentified event msg=" + msg);
                break;
        }
    }

    /**
     * Report on whether data connectivity is enabled
     *
     * @return {@code FALSE} if data connectivity has been explicitly disabled,
     *         {@code TRUE} otherwise.
     */
    public Boolean GetAnyDataEnabled() {
        final Boolean result;
        Synchronized (mDataEnabledLock) {
            result = (mInternalDataEnabled && mUserDataEnabled && sPolicyDataEnabled
                    && (mEnabledCount != 0));
        }
        If (!result && DBG) Log("getAnyDataEnabled " + result);
        return result;
    }

    protected Boolean IsEmergency() {
        final Boolean result;
        Synchronized (mDataEnabledLock) {
            result = mPhone->IsInEcm() || mPhone->IsInEmergencyCall();
        }
        Log("isEmergency: result=" + result);
        return result;
    }

    protected Int32 ApnTypeToId(String type) {
        If (TextUtils->Equals(type, PhoneConstants.APN_TYPE_DEFAULT)) {
            return DctConstants.APN_DEFAULT_ID;
        } else If (TextUtils->Equals(type, PhoneConstants.APN_TYPE_MMS)) {
            return DctConstants.APN_MMS_ID;
        } else If (TextUtils->Equals(type, PhoneConstants.APN_TYPE_SUPL)) {
            return DctConstants.APN_SUPL_ID;
        } else If (TextUtils->Equals(type, PhoneConstants.APN_TYPE_DUN)) {
            return DctConstants.APN_DUN_ID;
        } else If (TextUtils->Equals(type, PhoneConstants.APN_TYPE_HIPRI)) {
            return DctConstants.APN_HIPRI_ID;
        } else If (TextUtils->Equals(type, PhoneConstants.APN_TYPE_IMS)) {
            return DctConstants.APN_IMS_ID;
        } else If (TextUtils->Equals(type, PhoneConstants.APN_TYPE_FOTA)) {
            return DctConstants.APN_FOTA_ID;
        } else If (TextUtils->Equals(type, PhoneConstants.APN_TYPE_CBS)) {
            return DctConstants.APN_CBS_ID;
        } else If (TextUtils->Equals(type, PhoneConstants.APN_TYPE_IA)) {
            return DctConstants.APN_IA_ID;
        } else If (TextUtils->Equals(type, PhoneConstants.APN_TYPE_EMERGENCY)) {
            return DctConstants.APN_EMERGENCY_ID;
        } else {
            return DctConstants.APN_INVALID_ID;
        }
    }

    protected String ApnIdToType(Int32 id) {
        Switch (id) {
        case DctConstants.APN_DEFAULT_ID:
            return PhoneConstants.APN_TYPE_DEFAULT;
        case DctConstants.APN_MMS_ID:
            return PhoneConstants.APN_TYPE_MMS;
        case DctConstants.APN_SUPL_ID:
            return PhoneConstants.APN_TYPE_SUPL;
        case DctConstants.APN_DUN_ID:
            return PhoneConstants.APN_TYPE_DUN;
        case DctConstants.APN_HIPRI_ID:
            return PhoneConstants.APN_TYPE_HIPRI;
        case DctConstants.APN_IMS_ID:
            return PhoneConstants.APN_TYPE_IMS;
        case DctConstants.APN_FOTA_ID:
            return PhoneConstants.APN_TYPE_FOTA;
        case DctConstants.APN_CBS_ID:
            return PhoneConstants.APN_TYPE_CBS;
        case DctConstants.APN_IA_ID:
            return PhoneConstants.APN_TYPE_IA;
        case DctConstants.APN_EMERGENCY_ID:
            return PhoneConstants.APN_TYPE_EMERGENCY;
        default:
            Log("Unknown Id (" + id + ") in apnIdToType");
            return PhoneConstants.APN_TYPE_DEFAULT;
        }
    }

    public LinkProperties GetLinkProperties(String apnType) {
        Int32 id = ApnTypeToId(apnType);

        If (IsApnIdEnabled(id)) {
            DcAsyncChannel dcac = mDataConnectionAcHashMap->Get(0);
            return dcac->GetLinkPropertiesSync();
        } else {
            return new LinkProperties();
        }
    }

    public NetworkCapabilities GetNetworkCapabilities(String apnType) {
        Int32 id = ApnTypeToId(apnType);
        If (IsApnIdEnabled(id)) {
            DcAsyncChannel dcac = mDataConnectionAcHashMap->Get(0);
            return dcac->GetNetworkCapabilitiesSync();
        } else {
            return new NetworkCapabilities();
        }
    }

    // tell all active apns of the current condition
    protected void NotifyDataConnection(String reason) {
        For (Int32 id = 0; id < DctConstants.APN_NUM_TYPES; id++) {
            If (mDataEnabled[id]) {
                mPhone->NotifyDataConnection(reason, ApnIdToType(id));
            }
        }
        NotifyOffApnsOfAvailability(reason);
    }

    // a new APN has gone active and needs to send events to catch up with the
    // current condition
    private void NotifyApnIdUpToCurrent(String reason, Int32 apnId) {
        Switch (mState) {
            case IDLE:
                break;
            case RETRYING:
            case CONNECTING:
            case SCANNING:
                mPhone->NotifyDataConnection(reason, ApnIdToType(apnId),
                        PhoneConstants.DataState.CONNECTING);
                break;
            case CONNECTED:
            case DISCONNECTING:
                mPhone->NotifyDataConnection(reason, ApnIdToType(apnId),
                        PhoneConstants.DataState.CONNECTING);
                mPhone->NotifyDataConnection(reason, ApnIdToType(apnId),
                        PhoneConstants.DataState.CONNECTED);
                break;
            default:
                // Ignore
                break;
        }
    }

    // since we normally don't send info to a disconnected APN, we need to do this specially
    private void NotifyApnIdDisconnected(String reason, Int32 apnId) {
        mPhone->NotifyDataConnection(reason, ApnIdToType(apnId),
                PhoneConstants.DataState.DISCONNECTED);
    }

    // disabled apn's still need avail/unavail notificiations - send them out
    protected void NotifyOffApnsOfAvailability(String reason) {
        If (DBG) Log("notifyOffApnsOfAvailability - reason= " + reason);
        For (Int32 id = 0; id < DctConstants.APN_NUM_TYPES; id++) {
            If (!IsApnIdEnabled(id)) {
                NotifyApnIdDisconnected(reason, id);
            }
        }
    }

    public Boolean IsApnTypeEnabled(String apnType) {
        If (apnType == NULL) {
            return FALSE;
        } else {
            return IsApnIdEnabled(ApnTypeToId(apnType));
        }
    }

    protected synchronized Boolean IsApnIdEnabled(Int32 id) {
        If (id != DctConstants.APN_INVALID_ID) {
            return mDataEnabled[id];
        }
        return FALSE;
    }

    protected void SetEnabled(Int32 id, Boolean enable) {
        If (DBG) {
            Log("SetEnabled(" + id + ", " + enable + ") with old state = " + mDataEnabled[id]
                    + " and enabledCount = " + mEnabledCount);
        }
        Message msg = ObtainMessage(DctConstants.EVENT_ENABLE_NEW_APN);
        msg.arg1 = id;
        msg.arg2 = (enable ? DctConstants.ENABLED : DctConstants.DISABLED);
        SendMessage(msg);
    }

    protected void OnEnableApn(Int32 apnId, Int32 enabled) {
        If (DBG) {
            Log("EVENT_APN_ENABLE_REQUEST apnId=" + apnId + ", apnType=" + ApnIdToType(apnId) +
                    ", enabled=" + enabled + ", dataEnabled = " + mDataEnabled[apnId] +
                    ", enabledCount = " + mEnabledCount + ", isApnTypeActive = " +
                    IsApnTypeActive(ApnIdToType(apnId)));
        }
        If (enabled == DctConstants.ENABLED) {
            Synchronized (this) {
                If (!mDataEnabled[apnId]) {
                    mDataEnabled[apnId] = TRUE;
                    mEnabledCount++;
                }
            }
            String type = ApnIdToType(apnId);
            If (!IsApnTypeActive(type)) {
                mRequestedApnType = type;
                OnEnableNewApn();
            } else {
                NotifyApnIdUpToCurrent(Phone.REASON_APN_SWITCHED, apnId);
            }
        } else {
            // disable
            Boolean didDisable = FALSE;
            Synchronized (this) {
                If (mDataEnabled[apnId]) {
                    mDataEnabled[apnId] = FALSE;
                    mEnabledCount--;
                    didDisable = TRUE;
                }
            }
            If (didDisable) {
                If ((mEnabledCount == 0) || (apnId == DctConstants.APN_DUN_ID)) {
                    mRequestedApnType = PhoneConstants.APN_TYPE_DEFAULT;
                    OnCleanUpConnection(TRUE, apnId, Phone.REASON_DATA_DISABLED);
                }

                // send the disconnect msg manually, since the normal route wont send
                // It (it's not enabled)
                NotifyApnIdDisconnected(Phone.REASON_DATA_DISABLED, apnId);
                If (mDataEnabled[DctConstants.APN_DEFAULT_ID] == TRUE
                        && !IsApnTypeActive(PhoneConstants.APN_TYPE_DEFAULT)) {
                    // TODO - this is an ugly way to restore the default conn - should be done
                    // by a real contention manager and policy that disconnects the lower pri
                    // stuff as enable requests come in and pops them back on as we disable back
                    // down to the lower pri stuff
                    mRequestedApnType = PhoneConstants.APN_TYPE_DEFAULT;
                    OnEnableNewApn();
                }
            }
        }
    }

    /**
     * Called when we switch APNs.
     *
     * mRequestedApnType is set prior to call
     * To be overridden.
     */
    protected void OnEnableNewApn() {
    }

    /**
     * Called when EVENT_RESET_DONE is received so goto
     * IDLE state and send notifications to those interested.
     *
     * TODO - currently unused.  Needs to be hooked into DataConnection cleanup
     * TODO - needs to pass some notion of which connection is reset..
     */
    protected void OnResetDone(AsyncResult ar) {
        If (DBG) Log("EVENT_RESET_DONE");
        String reason = NULL;
        If (ar.userObj instanceof String) {
            reason = (String) ar.userObj;
        }
        GotoIdleAndNotifyDataConnection(reason);
    }

    /**
     * Prevent mobile data connections from being established, or once again
     * allow mobile data connections. If the state toggles, then either tear
     * down or set up data, as appropriate to match the new state.
     *
     * @param enable indicates whether to Enable ({@code TRUE}) or Disable (
     *            {@code FALSE}) data
     * @return {@code TRUE} if the operation succeeded
     */
    public Boolean SetInternalDataEnabled(Boolean enable) {
        If (DBG)
            Log("SetInternalDataEnabled(" + enable + ")");

        Message msg = ObtainMessage(DctConstants.EVENT_SET_INTERNAL_DATA_ENABLE);
        msg.arg1 = (enable ? DctConstants.ENABLED : DctConstants.DISABLED);
        SendMessage(msg);
        return TRUE;
    }

    protected void OnSetInternalDataEnabled(Boolean enabled) {
        Synchronized (mDataEnabledLock) {
            mInternalDataEnabled = enabled;
            If (enabled) {
                Log("onSetInternalDataEnabled: changed to enabled, try to setup data call");
                OnTrySetupData(Phone.REASON_DATA_ENABLED);
            } else {
                Log("onSetInternalDataEnabled: changed to disabled, cleanUpAllConnections");
                CleanUpAllConnections(NULL);
            }
        }
    }

    CARAPI CleanUpAllConnections(String cause) {
        Message msg = ObtainMessage(DctConstants.EVENT_CLEAN_UP_ALL_CONNECTIONS);
        msg.obj = cause;
        SendMessage(msg);
    }

    public abstract Boolean IsDisconnected();

    protected void OnSetUserDataEnabled(Boolean enabled) {
        Synchronized (mDataEnabledLock) {
            If (mUserDataEnabled != enabled) {
                mUserDataEnabled = enabled;
                Settings::Global::>PutInt(mPhone->GetContext()->GetContentResolver(),
                        Settings::Global::MOBILE_DATA + mPhone->GetPhoneId(), enabled ? 1 : 0);
                If (GetDataOnRoamingEnabled() == FALSE &&
                        mPhone->GetServiceState()->GetRoaming() == TRUE) {
                    If (enabled) {
                        NotifyOffApnsOfAvailability(Phone.REASON_ROAMING_ON);
                    } else {
                        NotifyOffApnsOfAvailability(Phone.REASON_DATA_DISABLED);
                    }
                }

                If (enabled) {
                    OnTrySetupData(Phone.REASON_DATA_ENABLED);
                } else {
                    OnCleanUpAllConnections(Phone.REASON_DATA_SPECIFIC_DISABLED);
                }
            }
        }
    }

    protected void OnSetDependencyMet(String apnType, Boolean met) {
    }

    protected void OnSetPolicyDataEnabled(Boolean enabled) {
        Synchronized (mDataEnabledLock) {
            If (sPolicyDataEnabled != enabled) {
                sPolicyDataEnabled = enabled;
                If (enabled) {
                    OnTrySetupData(Phone.REASON_DATA_ENABLED);
                } else {
                    OnCleanUpAllConnections(Phone.REASON_DATA_SPECIFIC_DISABLED);
                }
            }
        }
    }

    protected String GetReryConfig(Boolean forDefault) {
        Int32 nt = mPhone->GetServiceState()->GetNetworkType();

        If ((nt == TelephonyManager.NETWORK_TYPE_CDMA) ||
            (nt == TelephonyManager.NETWORK_TYPE_1xRTT) ||
            (nt == TelephonyManager.NETWORK_TYPE_EVDO_0) ||
            (nt == TelephonyManager.NETWORK_TYPE_EVDO_A) ||
            (nt == TelephonyManager.NETWORK_TYPE_EVDO_B) ||
            (nt == TelephonyManager.NETWORK_TYPE_EHRPD)) {
            // CDMA variant
            return SystemProperties->Get("ro.cdma.data_retry_config");
        } else {
            // Use GSM varient for all others.
            If (forDefault) {
                return SystemProperties->Get("ro.gsm.data_retry_config");
            } else {
                return SystemProperties->Get("ro.gsm.2nd_data_retry_config");
            }
        }
    }

    protected void ResetPollStats() {
        mTxPkts = -1;
        mRxPkts = -1;
        mNetStatPollPeriod = POLL_NETSTAT_MILLIS;
    }

    protected abstract DctConstants.State GetOverallState();

    void StartNetStatPoll() {
        If (GetOverallState() == DctConstants.State.CONNECTED
                && mNetStatPollEnabled == FALSE) {
            If (DBG) {
                Log("startNetStatPoll");
            }
            ResetPollStats();
            mNetStatPollEnabled = TRUE;
            mPollNetStat->Run();
        }
    }

    void StopNetStatPoll() {
        mNetStatPollEnabled = FALSE;
        RemoveCallbacks(mPollNetStat);
        If (DBG) {
            Log("stopNetStatPoll");
        }
    }

    CARAPI SendStartNetStatPoll(DctConstants.Activity activity) {
        Message msg = ObtainMessage(DctConstants.CMD_NET_STAT_POLL);
        msg.arg1 = DctConstants.ENABLED;
        msg.obj = activity;
        SendMessage(msg);
    }

    protected void HandleStartNetStatPoll(DctConstants.Activity activity) {
        StartNetStatPoll();
        StartDataStallAlarm(DATA_STALL_NOT_SUSPECTED);
        SetActivity(activity);
    }

    CARAPI SendStopNetStatPoll(DctConstants.Activity activity) {
        Message msg = ObtainMessage(DctConstants.CMD_NET_STAT_POLL);
        msg.arg1 = DctConstants.DISABLED;
        msg.obj = activity;
        SendMessage(msg);
    }

    protected void HandleStopNetStatPoll(DctConstants.Activity activity) {
        StopNetStatPoll();
        StopDataStallAlarm();
        SetActivity(activity);
    }

    CARAPI UpdateDataActivity() {
        Int64 sent, received;

        DctConstants.Activity newActivity;

        TxRxSum preTxRxSum = new TxRxSum(mTxPkts, mRxPkts);
        TxRxSum curTxRxSum = new TxRxSum();
        curTxRxSum->UpdateTxRxSum();
        mTxPkts = curTxRxSum.txPkts;
        mRxPkts = curTxRxSum.rxPkts;

        If (VDBG) {
            Log("updateDataActivity: curTxRxSum=" + curTxRxSum + " preTxRxSum=" + preTxRxSum);
        }

        If (mNetStatPollEnabled && (preTxRxSum.txPkts > 0 || preTxRxSum.rxPkts > 0)) {
            sent = mTxPkts - preTxRxSum.txPkts;
            received = mRxPkts - preTxRxSum.rxPkts;

            If (VDBG)
                Log("updateDataActivity: sent=" + sent + " received=" + received);
            If (sent > 0 && received > 0) {
                newActivity = DctConstants.Activity.DATAINANDOUT;
            } else If (sent > 0 && received == 0) {
                newActivity = DctConstants.Activity.DATAOUT;
            } else If (sent == 0 && received > 0) {
                newActivity = DctConstants.Activity.DATAIN;
            } else {
                newActivity = (mActivity == DctConstants.Activity.DORMANT) ?
                        mActivity : DctConstants.Activity.NONE;
            }

            If (mActivity != newActivity && mIsScreenOn) {
                If (VDBG)
                    Log("updateDataActivity: newActivity=" + newActivity);
                mActivity = newActivity;
                mPhone->NotifyDataActivity();
            }
        }
    }

    // Recovery action taken in case of data stall
    protected static class RecoveryAction {
        public static const Int32 GET_DATA_CALL_LIST      = 0;
        public static const Int32 CLEANUP                 = 1;
        public static const Int32 REREGISTER              = 2;
        public static const Int32 RADIO_RESTART           = 3;
        public static const Int32 RADIO_RESTART_WITH_PROP = 4;

        private static Boolean IsAggressiveRecovery(Int32 value) {
            Return ((value == RecoveryAction.CLEANUP) ||
                    (value == RecoveryAction.REREGISTER) ||
                    (value == RecoveryAction.RADIO_RESTART) ||
                    (value == RecoveryAction.RADIO_RESTART_WITH_PROP));
        }
    }

    public Int32 GetRecoveryAction() {
        Int32 action = Settings.System->GetInt(mPhone->GetContext()->GetContentResolver(),
                "radio.data.stall.recovery.action", RecoveryAction.GET_DATA_CALL_LIST);
        If (VDBG_STALL) Log("getRecoveryAction: " + action);
        return action;
    }
    CARAPI PutRecoveryAction(Int32 action) {
        Settings.System->PutInt(mPhone->GetContext()->GetContentResolver(),
                "radio.data.stall.recovery.action", action);
        If (VDBG_STALL) Log("putRecoveryAction: " + action);
    }

    protected Boolean IsConnected() {
        return FALSE;
    }

    protected void DoRecovery() {
        If (GetOverallState() == DctConstants.State.CONNECTED) {
            // Go through a series of recovery steps, each action transitions to the next action
            Int32 recoveryAction = GetRecoveryAction();
            Switch (recoveryAction) {
            case RecoveryAction.GET_DATA_CALL_LIST:
                EventLog->WriteEvent(EventLogTags.DATA_STALL_RECOVERY_GET_DATA_CALL_LIST,
                        mSentSinceLastRecv);
                If (DBG) Log("DoRecovery() get data call list");
                mPhone.mCi->GetDataCallList(ObtainMessage(DctConstants.EVENT_DATA_STATE_CHANGED));
                PutRecoveryAction(RecoveryAction.CLEANUP);
                break;
            case RecoveryAction.CLEANUP:
                EventLog->WriteEvent(EventLogTags.DATA_STALL_RECOVERY_CLEANUP, mSentSinceLastRecv);
                If (DBG) Log("DoRecovery() cleanup all connections");
                CleanUpAllConnections(Phone.REASON_PDP_RESET);
                PutRecoveryAction(RecoveryAction.REREGISTER);
                break;
            case RecoveryAction.REREGISTER:
                EventLog->WriteEvent(EventLogTags.DATA_STALL_RECOVERY_REREGISTER,
                        mSentSinceLastRecv);
                If (DBG) Log("DoRecovery() re-register");
                mPhone->GetServiceStateTracker()->ReRegisterNetwork(NULL);
                PutRecoveryAction(RecoveryAction.RADIO_RESTART);
                break;
            case RecoveryAction.RADIO_RESTART:
                EventLog->WriteEvent(EventLogTags.DATA_STALL_RECOVERY_RADIO_RESTART,
                        mSentSinceLastRecv);
                If (DBG) Log("restarting radio");
                PutRecoveryAction(RecoveryAction.RADIO_RESTART_WITH_PROP);
                RestartRadio();
                break;
            case RecoveryAction.RADIO_RESTART_WITH_PROP:
                // This is in case radio restart has not recovered the data.
                // It will set an additional "gsm.radioreset" property to tell
                // RIL or system to take further action.
                // The implementation of hard reset recovery action is up to OEM product.
                // Once RADIO_RESET property is consumed, it is expected to set back
                // to FALSE by RIL.
                EventLog->WriteEvent(EventLogTags.DATA_STALL_RECOVERY_RADIO_RESTART_WITH_PROP, -1);
                If (DBG) Log("restarting radio with gsm.radioreset to TRUE");
                SystemProperties->Set(RADIO_RESET_PROPERTY, "TRUE");
                // give 1 sec so property change can be notified.
                try {
                    Thread->Sleep(1000);
                } Catch (InterruptedException e) {}
                RestartRadio();
                PutRecoveryAction(RecoveryAction.GET_DATA_CALL_LIST);
                break;
            default:
                throw new RuntimeException("doRecovery: Invalid recoveryAction=" +
                    recoveryAction);
            }
            mSentSinceLastRecv = 0;
        }
    }

    private void UpdateDataStallInfo() {
        Int64 sent, received;

        TxRxSum preTxRxSum = new TxRxSum(mDataStallTxRxSum);
        mDataStallTxRxSum->UpdateTxRxSum();

        If (VDBG_STALL) {
            Log("updateDataStallInfo: mDataStallTxRxSum=" + mDataStallTxRxSum +
                    " preTxRxSum=" + preTxRxSum);
        }

        sent = mDataStallTxRxSum.txPkts - preTxRxSum.txPkts;
        received = mDataStallTxRxSum.rxPkts - preTxRxSum.rxPkts;

        If (RADIO_TESTS) {
            If (SystemProperties->GetBoolean("radio.test.data.stall", FALSE)) {
                Log("updateDataStallInfo: radio.test.data.stall TRUE received = 0;");
                received = 0;
            }
        }
        If ( sent > 0 && received > 0 ) {
            If (VDBG_STALL) Log("updateDataStallInfo: IN/OUT");
            mSentSinceLastRecv = 0;
            PutRecoveryAction(RecoveryAction.GET_DATA_CALL_LIST);
        } else If (sent > 0 && received == 0) {
            If (mPhone->GetState() == PhoneConstants.State.IDLE) {
                mSentSinceLastRecv += sent;
            } else {
                mSentSinceLastRecv = 0;
            }
            If (DBG) {
                Log("updateDataStallInfo: OUT sent=" + sent +
                        " mSentSinceLastRecv=" + mSentSinceLastRecv);
            }
        } else If (sent == 0 && received > 0) {
            If (VDBG_STALL) Log("updateDataStallInfo: IN");
            mSentSinceLastRecv = 0;
            PutRecoveryAction(RecoveryAction.GET_DATA_CALL_LIST);
        } else {
            If (VDBG_STALL) Log("updateDataStallInfo: NONE");
        }
    }

    protected void OnDataStallAlarm(Int32 tag) {
        If (mDataStallAlarmTag != tag) {
            If (DBG) {
                Log("onDataStallAlarm: ignore, tag=" + tag + " expecting " + mDataStallAlarmTag);
            }
            return;
        }
        UpdateDataStallInfo();

        Int32 hangWatchdogTrigger = Settings::Global::>GetInt(mResolver,
                Settings::Global::PDP_WATCHDOG_TRIGGER_PACKET_COUNT,
                NUMBER_SENT_PACKETS_OF_HANG);

        Boolean suspectedStall = DATA_STALL_NOT_SUSPECTED;
        If (mSentSinceLastRecv >= hangWatchdogTrigger) {
            If (DBG) {
                Log("onDataStallAlarm: tag=" + tag + " do recovery action=" + GetRecoveryAction());
            }
            suspectedStall = DATA_STALL_SUSPECTED;
            SendMessage(ObtainMessage(DctConstants.EVENT_DO_RECOVERY));
        } else {
            If (VDBG_STALL) {
                Log("onDataStallAlarm: tag=" + tag + " Sent " + String->ValueOf(mSentSinceLastRecv) +
                    " pkts since last received, < watchdogTrigger=" + hangWatchdogTrigger);
            }
        }
        StartDataStallAlarm(suspectedStall);
    }

    protected void StartDataStallAlarm(Boolean suspectedStall) {
        Int32 nextAction = GetRecoveryAction();
        Int32 delayInMs;

        If (mDataStallDetectionEnabled && GetOverallState() == DctConstants.State.CONNECTED) {
            // If screen is on or data stall is currently suspected, set the alarm
            // with an aggresive timeout.
            If (mIsScreenOn || suspectedStall || RecoveryAction->IsAggressiveRecovery(nextAction)) {
                delayInMs = Settings::Global::>GetInt(mResolver,
                        Settings::Global::DATA_STALL_ALARM_AGGRESSIVE_DELAY_IN_MS,
                        DATA_STALL_ALARM_AGGRESSIVE_DELAY_IN_MS_DEFAULT);
            } else {
                delayInMs = Settings::Global::>GetInt(mResolver,
                        Settings::Global::DATA_STALL_ALARM_NON_AGGRESSIVE_DELAY_IN_MS,
                        DATA_STALL_ALARM_NON_AGGRESSIVE_DELAY_IN_MS_DEFAULT);
            }

            mDataStallAlarmTag += 1;
            If (VDBG_STALL) {
                Log("startDataStallAlarm: tag=" + mDataStallAlarmTag +
                        " delay=" + (delayInMs / 1000) + "s");
            }
            Intent intent = new Intent(INTENT_DATA_STALL_ALARM);
            intent->PutExtra(DATA_STALL_ALARM_TAG_EXTRA, mDataStallAlarmTag);
            mDataStallAlarmIntent = PendingIntent->GetBroadcast(mPhone->GetContext(), 0, intent,
                    PendingIIntent::FLAG_UPDATE_CURRENT);
            mAlarmManager->Set(AlarmManager.ELAPSED_REALTIME_WAKEUP,
                    SystemClock->ElapsedRealtime() + delayInMs, mDataStallAlarmIntent);
        } else {
            If (VDBG_STALL) {
                Log("startDataStallAlarm: NOT started, no connection tag=" + mDataStallAlarmTag);
            }
        }
    }

    protected void StopDataStallAlarm() {
        If (VDBG_STALL) {
            Log("stopDataStallAlarm: current tag=" + mDataStallAlarmTag +
                    " mDataStallAlarmIntent=" + mDataStallAlarmIntent);
        }
        mDataStallAlarmTag += 1;
        If (mDataStallAlarmIntent != NULL) {
            mAlarmManager->Cancel(mDataStallAlarmIntent);
            mDataStallAlarmIntent = NULL;
        }
    }

    protected void RestartDataStallAlarm() {
        If (IsConnected() == FALSE) return;
        // To be called on screen status change.
        // Do not cancel the alarm if it is set with aggressive timeout.
        Int32 nextAction = GetRecoveryAction();

        If (RecoveryAction->IsAggressiveRecovery(nextAction)) {
            If (DBG) Log("restartDataStallAlarm: action is pending. not resetting the alarm.");
            return;
        }
        If (VDBG_STALL) Log("restartDataStallAlarm: stop then start.");
        StopDataStallAlarm();
        StartDataStallAlarm(DATA_STALL_NOT_SUSPECTED);
    }

    protected void SetInitialAttachApn() {
        ApnSetting iaApnSetting = NULL;
        ApnSetting defaultApnSetting = NULL;
        ApnSetting firstApnSetting = NULL;

        Log("setInitialApn: E mPreferredApn=" + mPreferredApn);

        If (mAllApnSettings != NULL && !mAllApnSettings->IsEmpty()) {
            firstApnSetting = mAllApnSettings->Get(0);
            Log("setInitialApn: firstApnSetting=" + firstApnSetting);

            // Search for Initial APN setting and the first apn that can handle default
            For (ApnSetting apn : mAllApnSettings) {
                // Can't use apn->CanHandleType(), as that returns TRUE for APNs that have no type.
                If (ArrayUtils->Contains(apn.types, PhoneConstants.APN_TYPE_IA) &&
                        apn.carrierEnabled) {
                    // The Initial Attach APN is highest priority so use it if there is one
                    Log("setInitialApn: iaApnSetting=" + apn);
                    iaApnSetting = apn;
                    break;
                } else If ((defaultApnSetting == NULL)
                        && (apn->CanHandleType(PhoneConstants.APN_TYPE_DEFAULT))) {
                    // Use the first default apn if no better choice
                    Log("setInitialApn: defaultApnSetting=" + apn);
                    defaultApnSetting = apn;
                }
            }
        }

        // The priority of apn candidates from highest to lowest is:
        //   1) APN_TYPE_IA (Inital Attach)
        //   2) mPreferredApn, i.e. the current preferred apn
        //   3) The first apn that than handle APN_TYPE_DEFAULT
        //   4) The first APN we can find.

        ApnSetting initialAttachApnSetting = NULL;
        If (iaApnSetting != NULL) {
            If (DBG) Log("setInitialAttachApn: using iaApnSetting");
            initialAttachApnSetting = iaApnSetting;
        } else If (mPreferredApn != NULL) {
            If (DBG) Log("setInitialAttachApn: using mPreferredApn");
            initialAttachApnSetting = mPreferredApn;
        } else If (defaultApnSetting != NULL) {
            If (DBG) Log("setInitialAttachApn: using defaultApnSetting");
            initialAttachApnSetting = defaultApnSetting;
        } else If (firstApnSetting != NULL) {
            If (DBG) Log("setInitialAttachApn: using firstApnSetting");
            initialAttachApnSetting = firstApnSetting;
        }

        If (initialAttachApnSetting == NULL) {
            If (DBG) Log("setInitialAttachApn: X There in no available apn");
        } else {
            If (DBG) Log("setInitialAttachApn: X selected Apn=" + initialAttachApnSetting);

            mPhone.mCi->SetInitialAttachApn(initialAttachApnSetting.apn,
                    initialAttachApnSetting.protocol, initialAttachApnSetting.authType,
                    initialAttachApnSetting.user, initialAttachApnSetting.password, NULL);
        }
    }

    protected void SetDataProfilesAsNeeded() {
        If (DBG) Log("setDataProfilesAsNeeded");
        If (mAllApnSettings != NULL && !mAllApnSettings->IsEmpty()) {
            ArrayList<DataProfile> dps = new ArrayList<DataProfile>();
            For (ApnSetting apn : mAllApnSettings) {
                If (apn.modemCognitive) {
                    DataProfile dp = new DataProfile(apn,
                            mPhone->GetServiceState()->GetRoaming());
                    Boolean isDup = FALSE;
                    For(DataProfile dpIn : dps) {
                        If (dp->Equals(dpIn)) {
                            isDup = TRUE;
                            break;
                        }
                    }
                    If (!isDup) {
                        dps->Add(dp);
                    }
                }
            }
            If(dps->Size() > 0) {
                mPhone.mCi->SetDataProfile(dps->ToArray(new DataProfile[0]), NULL);
            }
        }
    }

    protected void OnActionIntentProvisioningApnAlarm(Intent intent) {
        If (DBG) Log("onActionIntentProvisioningApnAlarm: action=" + intent->GetAction());
        Message msg = ObtainMessage(DctConstants.EVENT_PROVISIONING_APN_ALARM,
                intent->GetAction());
        msg.arg1 = intent->GetIntExtra(PROVISIONING_APN_ALARM_TAG_EXTRA, 0);
        SendMessage(msg);
    }

    protected void StartProvisioningApnAlarm() {
        Int32 delayInMs = Settings::Global::>GetInt(mResolver,
                                Settings::Global::PROVISIONING_APN_ALARM_DELAY_IN_MS,
                                PROVISIONING_APN_ALARM_DELAY_IN_MS_DEFAULT);
        If (Build.IS_DEBUGGABLE) {
            // Allow debug code to use a system property to provide another value
            String delayInMsStrg = Integer->ToString(delayInMs);
            delayInMsStrg = System->GetProperty(DEBUG_PROV_APN_ALARM, delayInMsStrg);
            try {
                delayInMs = Integer->ParseInt(delayInMsStrg);
            } Catch (NumberFormatException e) {
                Loge("startProvisioningApnAlarm: e=" + e);
            }
        }
        mProvisioningApnAlarmTag += 1;
        If (DBG) {
            Log("startProvisioningApnAlarm: tag=" + mProvisioningApnAlarmTag +
                    " delay=" + (delayInMs / 1000) + "s");
        }
        Intent intent = new Intent(INTENT_PROVISIONING_APN_ALARM);
        intent->PutExtra(PROVISIONING_APN_ALARM_TAG_EXTRA, mProvisioningApnAlarmTag);
        mProvisioningApnAlarmIntent = PendingIntent->GetBroadcast(mPhone->GetContext(), 0, intent,
                PendingIIntent::FLAG_UPDATE_CURRENT);
        mAlarmManager->Set(AlarmManager.ELAPSED_REALTIME_WAKEUP,
                SystemClock->ElapsedRealtime() + delayInMs, mProvisioningApnAlarmIntent);
    }

    protected void StopProvisioningApnAlarm() {
        If (DBG) {
            Log("stopProvisioningApnAlarm: current tag=" + mProvisioningApnAlarmTag +
                    " mProvsioningApnAlarmIntent=" + mProvisioningApnAlarmIntent);
        }
        mProvisioningApnAlarmTag += 1;
        If (mProvisioningApnAlarmIntent != NULL) {
            mAlarmManager->Cancel(mProvisioningApnAlarmIntent);
            mProvisioningApnAlarmIntent = NULL;
        }
    }

    void SendCleanUpConnection(Boolean tearDown, ApnContext apnContext) {
        If (DBG)Log("sendCleanUpConnection: tearDown=" + tearDown + " apnContext=" + apnContext);
        Message msg = ObtainMessage(DctConstants.EVENT_CLEAN_UP_CONNECTION);
        msg.arg1 = tearDown ? 1 : 0;
        msg.arg2 = 0;
        msg.obj = apnContext;
        SendMessage(msg);
    }

    void SendRestartRadio() {
        If (DBG)Log("sendRestartRadio:");
        Message msg = ObtainMessage(DctConstants.EVENT_RESTART_RADIO);
        SendMessage(msg);
    }

    CARAPI Dump(FileDescriptor fd, PrintWriter pw, String[] args) {
        pw->Println("DataConnectionTrackerBase:");
        pw->Println(" RADIO_TESTS=" + RADIO_TESTS);
        pw->Println(" mInternalDataEnabled=" + mInternalDataEnabled);
        pw->Println(" mUserDataEnabled=" + mUserDataEnabled);
        pw->Println(" sPolicyDataEnabed=" + sPolicyDataEnabled);
        pw->Println(" mDataEnabled:");
        For(Int32 i=0; i < mDataEnabled.length; i++) {
            pw->Printf("  mDataEnabled[%d]=%b\n", i, mDataEnabled[i]);
        }
        pw->Flush();
        pw->Println(" mEnabledCount=" + mEnabledCount);
        pw->Println(" mRequestedApnType=" + mRequestedApnType);
        pw->Println(" mPhone=" + mPhone->GetPhoneName());
        pw->Println(" mPhoneId=" + mPhone->GetPhoneId());
        pw->Println(" mActivity=" + mActivity);
        pw->Println(" mState=" + mState);
        pw->Println(" mTxPkts=" + mTxPkts);
        pw->Println(" mRxPkts=" + mRxPkts);
        pw->Println(" mNetStatPollPeriod=" + mNetStatPollPeriod);
        pw->Println(" mNetStatPollEnabled=" + mNetStatPollEnabled);
        pw->Println(" mDataStallTxRxSum=" + mDataStallTxRxSum);
        pw->Println(" mDataStallAlarmTag=" + mDataStallAlarmTag);
        pw->Println(" mDataStallDetectionEanbled=" + mDataStallDetectionEnabled);
        pw->Println(" mSentSinceLastRecv=" + mSentSinceLastRecv);
        pw->Println(" mNoRecvPollCount=" + mNoRecvPollCount);
        pw->Println(" mResolver=" + mResolver);
        pw->Println(" mIsWifiConnected=" + mIsWifiConnected);
        pw->Println(" mReconnectIntent=" + mReconnectIntent);
        pw->Println(" mCidActive=" + mCidActive);
        pw->Println(" mAutoAttachOnCreation=" + mAutoAttachOnCreation);
        pw->Println(" mIsScreenOn=" + mIsScreenOn);
        pw->Println(" mUniqueIdGenerator=" + mUniqueIdGenerator);
        pw->Flush();
        pw->Println(" ***************************************");
        DcController dcc = mDcc;
        If (dcc != NULL) {
            dcc->Dump(fd, pw, args);
        } else {
            pw->Println(" mDcc=NULL");
        }
        pw->Println(" ***************************************");
        HashMap<Integer, DataConnection> dcs = mDataConnections;
        If (dcs != NULL) {
            Set<Entry<Integer, DataConnection> > mDcSet = mDataConnections->EntrySet();
            pw->Println(" mDataConnections: count=" + mDcSet->Size());
            For (Entry<Integer, DataConnection> entry : mDcSet) {
                pw->Printf(" *** mDataConnection[%d] \n", entry->GetKey());
                entry->GetValue()->Dump(fd, pw, args);
            }
        } else {
            pw->Println("mDataConnections=NULL");
        }
        pw->Println(" ***************************************");
        pw->Flush();
        HashMap<String, Integer> apnToDcId = mApnToDataConnectionId;
        If (apnToDcId != NULL) {
            Set<Entry<String, Integer>> apnToDcIdSet = apnToDcId->EntrySet();
            pw->Println(" mApnToDataConnectonId size=" + apnToDcIdSet->Size());
            For (Entry<String, Integer> entry : apnToDcIdSet) {
                pw->Printf(" mApnToDataConnectonId[%s]=%d\n", entry->GetKey(), entry->GetValue());
            }
        } else {
            pw->Println("mApnToDataConnectionId=NULL");
        }
        pw->Println(" ***************************************");
        pw->Flush();
        ConcurrentHashMap<String, ApnContext> apnCtxs = mApnContexts;
        If (apnCtxs != NULL) {
            Set<Entry<String, ApnContext>> apnCtxsSet = apnCtxs->EntrySet();
            pw->Println(" mApnContexts size=" + apnCtxsSet->Size());
            For (Entry<String, ApnContext> entry : apnCtxsSet) {
                entry->GetValue()->Dump(fd, pw, args);
            }
            pw->Println(" ***************************************");
        } else {
            pw->Println(" mApnContexts=NULL");
        }
        pw->Flush();
        pw->Println(" mActiveApn=" + mActiveApn);
        ArrayList<ApnSetting> apnSettings = mAllApnSettings;
        If (apnSettings != NULL) {
            pw->Println(" mAllApnSettings size=" + apnSettings->Size());
            For (Int32 i=0; i < apnSettings->Size(); i++) {
                pw->Printf(" mAllApnSettings[%d]: %s\n", i, apnSettings->Get(i));
            }
            pw->Flush();
        } else {
            pw->Println(" mAllApnSettings=NULL");
        }
        pw->Println(" mPreferredApn=" + mPreferredApn);
        pw->Println(" mIsPsRestricted=" + mIsPsRestricted);
        pw->Println(" mIsDisposed=" + mIsDisposed);
        pw->Println(" mIntentReceiver=" + mIntentReceiver);
        pw->Println(" mDataRoamingSettingObserver=" + mDataRoamingSettingObserver);
        pw->Flush();
    }
}
