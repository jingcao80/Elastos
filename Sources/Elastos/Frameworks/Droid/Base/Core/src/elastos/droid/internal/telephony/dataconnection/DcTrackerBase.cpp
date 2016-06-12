
#include "elastos/droid/internal/telephony/dataconnection/DcTrackerBase.h"
#include <Elastos.CoreLibrary.Utility.Concurrent.h>

using Elastos::Core::EIID_IRunnable;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

//=============================================================================
// DcTrackerBase::TxRxSum
//=============================================================================
CAR_INTERFACE_IMPL(DcTrackerBase::TxRxSum, Object, IDcTrackerBaseTxRxSum)

ECode DcTrackerBase::TxRxSum::constructor()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
            reset();

#endif
}

ECode DcTrackerBase::TxRxSum::constructor(
    /* [in] */ Int64 txPkts,
    /* [in] */ Int64 rxPkts)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
            this.txPkts = txPkts;
            this.rxPkts = rxPkts;

#endif
}

ECode DcTrackerBase::TxRxSum::constructor(
    /* [in] */ IDcTrackerBaseTxRxSum* sum)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
            txPkts = sum.txPkts;
            rxPkts = sum.rxPkts;

#endif
}

ECode DcTrackerBase::TxRxSum::Reset()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
            txPkts = -1;
            rxPkts = -1;

#endif
}

ECode DcTrackerBase::TxRxSum::ToString(
    /* [out] */ String* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
            return "{txSum=" + txPkts + " rxSum=" + rxPkts + "}";

#endif
}

ECode DcTrackerBase::TxRxSum::UpdateTxRxSum()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
            this.txPkts = TrafficStats.getMobileTcpTxPackets();
            this.rxPkts = TrafficStats.getMobileTcpRxPackets();

#endif
}

ECode DcTrackerBase::TxRxSum::GetTxPkts(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mTxPkts;
    return NOERROR;
}

ECode DcTrackerBase::TxRxSum::SetTxPkts(
    /* [in] */ Int64 txPkts)
{
    mTxPkts = txPkts;
    return NOERROR;
}

ECode DcTrackerBase::TxRxSum::GetRxPkts(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mRxPkts;
    return NOERROR;
}

ECode DcTrackerBase::TxRxSum::SetRxPkts(
    /* [in] */ Int64 rxPkts)
{
    mRxPkts = rxPkts;
    return NOERROR;
}

//=============================================================================
// DcTrackerBase::RecoveryAction
//=============================================================================
ECode DcTrackerBase::RecoveryAction::IsAggressiveRecovery(
    /* [in] */ Int32 value,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                return ((value == RecoveryAction.CLEANUP) ||
                        (value == RecoveryAction.REREGISTER) ||
                        (value == RecoveryAction.RADIO_RESTART) ||
                        (value == RecoveryAction.RADIO_RESTART_WITH_PROP));

#endif
}

//=============================================================================
// DcTrackerBase::SubBroadcastReceiver
//=============================================================================
DcTrackerBase::SubBroadcastReceiver::SubBroadcastReceiver(
    /* [in] */ DcTrackerBase* host)
    : mHost(host)
{}

ECode DcTrackerBase::SubBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                String action = intent.getAction();
                if (DBG) log("onReceive: action=" + action);
                if (action.equals(Intent.ACTION_SCREEN_ON)) {
                    mIsScreenOn = true;
                    stopNetStatPoll();
                    startNetStatPoll();
                    restartDataStallAlarm();
                } else if (action.equals(Intent.ACTION_SCREEN_OFF)) {
                    mIsScreenOn = false;
                    stopNetStatPoll();
                    startNetStatPoll();
                    restartDataStallAlarm();
                } else if (action.startsWith(INTENT_RECONNECT_ALARM)) {
                    if (DBG) log("Reconnect alarm. Previous state was " + mState);
                    onActionIntentReconnectAlarm(intent);
                } else if (action.startsWith(INTENT_RESTART_TRYSETUP_ALARM)) {
                    if (DBG) log("Restart trySetup alarm");
                    onActionIntentRestartTrySetupAlarm(intent);
                } else if (action.equals(INTENT_DATA_STALL_ALARM)) {
                    onActionIntentDataStallAlarm(intent);
                } else if (action.equals(INTENT_PROVISIONING_APN_ALARM)) {
                    onActionIntentProvisioningApnAlarm(intent);
                } else if (action.equals(WifiManager.NETWORK_STATE_CHANGED_ACTION)) {
                    final android.net.NetworkInfo networkInfo = (NetworkInfo)
                            intent.getParcelableExtra(WifiManager.EXTRA_NETWORK_INFO);
                    mIsWifiConnected = (networkInfo != null && networkInfo.isConnected());
                    if (DBG) log("NETWORK_STATE_CHANGED_ACTION: mIsWifiConnected=" + mIsWifiConnected);
                } else if (action.equals(WifiManager.WIFI_STATE_CHANGED_ACTION)) {
                    final boolean enabled = intent.getIntExtra(WifiManager.EXTRA_WIFI_STATE,
                            WifiManager.WIFI_STATE_UNKNOWN) == WifiManager.WIFI_STATE_ENABLED;
                    if (!enabled) {
                        // when WiFi got disabled, the NETWORK_STATE_CHANGED_ACTION
                        // quit and won't report disconnected until next enabling.
                        mIsWifiConnected = false;
                    }
                    if (DBG) log("WIFI_STATE_CHANGED_ACTION: enabled=" + enabled
                            + " mIsWifiConnected=" + mIsWifiConnected);
                }

#endif
}

//=============================================================================
// DcTrackerBase::SubRunnable
//=============================================================================
CAR_INTERFACE_IMPL(DcTrackerBase::SubRunnable, Object, IRunnable)

DcTrackerBase::SubRunnable::SubRunnable(
    /* [in] */ DcTrackerBase* host)
    : mHost(host)
{}

ECode DcTrackerBase::SubRunnable::Run()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                updateDataActivity();
                if (mIsScreenOn) {
                    mNetStatPollPeriod = Settings.Global.getInt(mResolver,
                            Settings.Global.PDP_WATCHDOG_POLL_INTERVAL_MS, POLL_NETSTAT_MILLIS);
                } else {
                    mNetStatPollPeriod = Settings.Global.getInt(mResolver,
                            Settings.Global.PDP_WATCHDOG_LONG_POLL_INTERVAL_MS,
                            POLL_NETSTAT_SCREEN_OFF_MILLIS);
                }
                if (mNetStatPollEnabled) {
                    mDataConnectionTracker.postDelayed(this, mNetStatPollPeriod);
                }

#endif
}

//=============================================================================
// DcTrackerBase::DataRoamingSettingObserver
//=============================================================================
DcTrackerBase::DataRoamingSettingObserver::DataRoamingSettingObserver(
    /* [in] */ DcTrackerBase* host)
    : mHost(host)
{}

ECode DcTrackerBase::DataRoamingSettingObserver::constructor(
    /* [in] */ IHandler* handler,
    /* [in] */ IContext* context)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                super(handler);
                mResolver = context.getContentResolver();

#endif
}

ECode DcTrackerBase::DataRoamingSettingObserver::Register()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                mResolver.registerContentObserver(
                        Settings.Global.getUriFor(Settings.Global.DATA_ROAMING +
                        mPhone.getPhoneId()), false, this);

#endif
}

ECode DcTrackerBase::DataRoamingSettingObserver::Unregister()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                mResolver.unregisterContentObserver(this);

#endif
}

ECode DcTrackerBase::DataRoamingSettingObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                // already running on mPhone handler thread
                if (mPhone.getServiceState().getRoaming()) {
                    sendMessage(obtainMessage(DctConstants.EVENT_ROAMING_ON));
                }

#endif
}

//=============================================================================
// DcTrackerBase
//=============================================================================
CAR_INTERFACE_IMPL(DcTrackerBase, Handler, IDcTrackerBase)

const Boolean DcTrackerBase::DBG = true;
const Boolean DcTrackerBase::VDBG = false;
const Boolean DcTrackerBase::VDBG_STALL = true;
const Boolean DcTrackerBase::RADIO_TESTS = false;
Boolean DcTrackerBase::mIsCleanupRequired = false;
const Int32 DcTrackerBase::DATA_CONNECTION_ACTIVE_PH_LINK_INACTIVE = 0;
const Int32 DcTrackerBase::DATA_CONNECTION_ACTIVE_PH_LINK_DOWN = 1;
const Int32 DcTrackerBase::DATA_CONNECTION_ACTIVE_PH_LINK_UP = 2;
const Int32 DcTrackerBase::APN_DELAY_DEFAULT_MILLIS = 20000;
const Int32 DcTrackerBase::APN_FAIL_FAST_DELAY_DEFAULT_MILLIS = 3000;
Boolean DcTrackerBase::sPolicyDataEnabled = true;
const String DcTrackerBase::DEFAULT_DATA_RETRY_CONFIG("default_randomization=2000,5000,10000,20000,40000,80000:5000,160000:5000,320000:5000,640000:5000,1280000:5000,1800000:5000");
const String DcTrackerBase::SECONDARY_DATA_RETRY_CONFIG("max_retries=3, 5000, 5000, 5000");
const Int32 DcTrackerBase::POLL_NETSTAT_SLOW_MILLIS = 5000;
const Int32 DcTrackerBase::DEFAULT_MAX_PDP_RESET_FAIL = 3;
const Int32 DcTrackerBase::NO_RECV_POLL_LIMIT = 24;
const Int32 DcTrackerBase::POLL_NETSTAT_MILLIS = 1000;
const Int32 DcTrackerBase::POLL_NETSTAT_SCREEN_OFF_MILLIS = 1000*60*10;
const Int32 DcTrackerBase::POLL_LONGEST_RTT = 120 * 1000;
const Int32 DcTrackerBase::NUMBER_SENT_PACKETS_OF_HANG = 10;
const Int32 DcTrackerBase::RESTORE_DEFAULT_APN_DELAY = 1 * 60 * 1000;
const String DcTrackerBase::APN_RESTORE_DELAY_PROP_NAME("android.telephony.apn-restore");
const String DcTrackerBase::NULL_IP("0.0.0.0");
const Int32 DcTrackerBase::DATA_STALL_ALARM_NON_AGGRESSIVE_DELAY_IN_MS_DEFAULT = 1000 * 60 * 6;
const Int32 DcTrackerBase::DATA_STALL_ALARM_AGGRESSIVE_DELAY_IN_MS_DEFAULT = 1000 * 60;
const Int32 DcTrackerBase::DATA_STALL_NO_RECV_POLL_LIMIT = 1;
const String DcTrackerBase::DATA_STALL_ALARM_TAG_EXTRA("data.stall.alram.tag");
const Boolean DcTrackerBase::DATA_STALL_SUSPECTED = true;
const Boolean DcTrackerBase::DATA_STALL_NOT_SUSPECTED = false;
const String DcTrackerBase::INTENT_RECONNECT_ALARM("com.android.internal.telephony.data-reconnect");
const String DcTrackerBase::INTENT_RECONNECT_ALARM_EXTRA_TYPE("reconnect_alarm_extra_type");
const String DcTrackerBase::INTENT_RECONNECT_ALARM_EXTRA_REASON("reconnect_alarm_extra_reason");
const String DcTrackerBase::INTENT_RESTART_TRYSETUP_ALARM("com.android.internal.telephony.data-restart-trysetup");
const String DcTrackerBase::INTENT_RESTART_TRYSETUP_ALARM_EXTRA_TYPE("restart_trysetup_alarm_extra_type");
const String DcTrackerBase::INTENT_DATA_STALL_ALARM("com.android.internal.telephony.data-stall");
const String DcTrackerBase::DEFALUT_DATA_ON_BOOT_PROP("net.def_data_on_boot");
Int32 DcTrackerBase::sEnableFailFastRefCounter = 0;
const String DcTrackerBase::INTENT_PROVISIONING_APN_ALARM("com.android.internal.telephony.provisioning_apn_alarm");
const String DcTrackerBase::PROVISIONING_APN_ALARM_TAG_EXTRA("provisioning.apn.alarm.tag");
const String DcTrackerBase::DEBUG_PROV_APN_ALARM("persist.debug.prov_apn_alarm");
const Int32 DcTrackerBase::PROVISIONING_APN_ALARM_DELAY_IN_MS_DEFAULT = 1000 * 60 * 15;
const Int32 DcTrackerBase::DEFAULT_MDC_INITIAL_RETRY = 1;

DcTrackerBase::DcTrackerBase()
    : mInternalDataEnabled(true)
    , mUserDataEnabled(true)
    , mDataEnabled(ArrayOf<Boolean>::Alloc(IDctConstants::APN_NUM_TYPES))
    , mEnabledCount(0)
    , mRequestedApnType(IPhoneConstants::APN_TYPE_DEFAULT)
    , mRADIO_RESET_PROPERTY("gsm.radioreset")
    , mActivity(DctConstantsActivity_NONE)
    , mState(DctConstantsState_IDLE)
    , mDataConnectionTracker(NULL)
    , mNetStatPollEnabled(FALSE)
    , mDataStallAlarmIntent(NULL)
    , mNoRecvPollCount(0)
    , mDataStallDetectionEnabled(TRUE)
    , mFailFast(FALSE)
    , mInVoiceCall(FALSE)
    , mIsWifiConnected(FALSE)
    , mReconnectIntent(NULL)
    , mAutoAttachOnCreationConfig(false)
    , mAutoAttachOnCreation(false)
    , mIsScreenOn(true)
    , mPreferredApn(NULL)
    , mIsPsRestricted(false)
    , mEmergencyApn(NULL)
    , mIsDisposed(false)
    , mIsProvisioning(false)
    , mProvisioningUrl(NULL)
    , mProvisioningApnAlarmIntent(NULL)
{
#if 0 // TODO: Translate codes below
    mDataStallAlarmTag = (int) SystemClock.elapsedRealtime();
    mProvisioningApnAlarmTag = (int) SystemClock.elapsedRealtime();
    AutoPtr<IObject> mDataEnabledLock = new Object();
    AutoPtr<IAtomicReference> mIccRecords = new AtomicReference();
    CTxRxSum::New(0, 0, (IDcTrackerBaseTxRxSum**)&mDataStallTxRxSum);
    CAtomicInteger::New(0, (IAtomicInteger**mUniqueIdGenerator)&);
    CHashMap::New((IHashMap**)&mDataConnections);
    CHashMap::New((IHashMap**)&mDataConnectionAcHashMap);
    CHashMap::New((IHashMap**)&mApnToDataConnectionId);
    CConcurrentHashMap::New((ConcurrentHashMapIfinal**)&mApnContexts);
    AutoPtr<IPriorityQueue> mPrioritySortedApnContexts =
            new PriorityQueue<ApnContext>(5,
            new Comparator<ApnContext>() {
                public int compare(ApnContext c1, ApnContext c2) {
                    return c2.priority - c1.priority;
                }
            }
    CArrayList::New((IArrayList**)&mAllApnSettings);
    CAsyncChannel::New((IAsyncChannel**)&mReplyAc);
    mIntentReceiver = new SubBroadcastReceiver(this);
    mPollNetStat = new SubRunnable(this);
#endif
}

ECode DcTrackerBase::GetInitialMaxRetry(
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (mFailFast) {
            return 0;
        }
        // Get default value from system property or use DEFAULT_MDC_INITIAL_RETRY
        int value = SystemProperties.getInt(
                Settings.Global.MDC_INITIAL_MAX_RETRY, DEFAULT_MDC_INITIAL_RETRY);
        // Check if its been overridden
        return Settings.Global.getInt(mResolver,
                Settings.Global.MDC_INITIAL_MAX_RETRY, value);

#endif
}

ECode DcTrackerBase::OnActionIntentReconnectAlarm(
    /* [in] */ IIntent* intent)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        String reason = intent.getStringExtra(INTENT_RECONNECT_ALARM_EXTRA_REASON);
        String apnType = intent.getStringExtra(INTENT_RECONNECT_ALARM_EXTRA_TYPE);
        long phoneSubId = mPhone.getSubId();
        long currSubId = intent.getLongExtra(PhoneConstants.SUBSCRIPTION_KEY,
                SubscriptionManager.INVALID_SUB_ID);
        log("onActionIntentReconnectAlarm: currSubId = " + currSubId + " phoneSubId=" + phoneSubId);
        // Stop reconnect if not current subId is not correct.
        // FIXME STOPSHIP - phoneSubId is coming up as -1 way after boot and failing this.
//        if ((currSubId == SubscriptionManager.INVALID_SUB_ID) || (currSubId != phoneSubId)) {
//            log("receive ReconnectAlarm but subId incorrect, ignore");
//            return;
//        }
        ApnContext apnContext = mApnContexts.get(apnType);
        if (DBG) {
            log("onActionIntentReconnectAlarm: mState=" + mState + " reason=" + reason +
                    " apnType=" + apnType + " apnContext=" + apnContext +
                    " mDataConnectionAsyncChannels=" + mDataConnectionAcHashMap);
        }
        if ((apnContext != null) && (apnContext.isEnabled())) {
            apnContext.setReason(reason);
            DctConstants.State apnContextState = apnContext.getState();
            if (DBG) {
                log("onActionIntentReconnectAlarm: apnContext state=" + apnContextState);
            }
            if ((apnContextState == DctConstants.State.FAILED)
                    || (apnContextState == DctConstants.State.IDLE)) {
                if (DBG) {
                    log("onActionIntentReconnectAlarm: state is FAILED|IDLE, disassociate");
                }
                DcAsyncChannel dcac = apnContext.getDcAc();
                if (dcac != null) {
                    dcac.tearDown(apnContext, "", null);
                }
                apnContext.setDataConnectionAc(null);
                apnContext.setState(DctConstants.State.IDLE);
            } else {
                if (DBG) log("onActionIntentReconnectAlarm: keep associated");
            }
            // TODO: IF already associated should we send the EVENT_TRY_SETUP_DATA???
            sendMessage(obtainMessage(DctConstants.EVENT_TRY_SETUP_DATA, apnContext));
            apnContext.setReconnectIntent(null);
        }

#endif
}

ECode DcTrackerBase::OnActionIntentRestartTrySetupAlarm(
    /* [in] */ IIntent* intent)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        String apnType = intent.getStringExtra(INTENT_RESTART_TRYSETUP_ALARM_EXTRA_TYPE);
        ApnContext apnContext = mApnContexts.get(apnType);
        if (DBG) {
            log("onActionIntentRestartTrySetupAlarm: mState=" + mState +
                    " apnType=" + apnType + " apnContext=" + apnContext +
                    " mDataConnectionAsyncChannels=" + mDataConnectionAcHashMap);
        }
        sendMessage(obtainMessage(DctConstants.EVENT_TRY_SETUP_DATA, apnContext));

#endif
}

ECode DcTrackerBase::OnActionIntentDataStallAlarm(
    /* [in] */ IIntent* intent)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (VDBG_STALL) log("onActionIntentDataStallAlarm: action=" + intent.getAction());
        Message msg = obtainMessage(DctConstants.EVENT_DATA_STALL_ALARM,
                intent.getAction());
        msg.arg1 = intent.getIntExtra(DATA_STALL_ALARM_TAG_EXTRA, 0);
        sendMessage(msg);

#endif
}

ECode DcTrackerBase::constructor(
    /* [in] */ IPhoneBase* phone)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        super();
        mPhone = phone;
        if (DBG) log("DCT.constructor");
        mResolver = mPhone.getContext().getContentResolver();
        mUiccController = UiccController.getInstance();
        mUiccController.registerForIccChanged(this, DctConstants.EVENT_ICC_CHANGED, null);
        mAlarmManager =
                (AlarmManager) mPhone.getContext().getSystemService(Context.ALARM_SERVICE);
        mCm = (ConnectivityManager) mPhone.getContext().getSystemService(
                Context.CONNECTIVITY_SERVICE);
        IntentFilter filter = new IntentFilter();
        filter.addAction(Intent.ACTION_SCREEN_ON);
        filter.addAction(Intent.ACTION_SCREEN_OFF);
        filter.addAction(WifiManager.NETWORK_STATE_CHANGED_ACTION);
        filter.addAction(WifiManager.WIFI_STATE_CHANGED_ACTION);
        filter.addAction(INTENT_DATA_STALL_ALARM);
        filter.addAction(INTENT_PROVISIONING_APN_ALARM);
        mUserDataEnabled = Settings.Global.getInt(mPhone.getContext().getContentResolver(),
                Settings.Global.MOBILE_DATA + mPhone.getPhoneId(), 1) == 1;
        mPhone.getContext().registerReceiver(mIntentReceiver, filter, null, mPhone);
        // This preference tells us 1) initial condition for "dataEnabled",
        // and 2) whether the RIL will setup the baseband to auto-PS attach.
        mDataEnabled[DctConstants.APN_DEFAULT_ID] =
                SystemProperties.getBoolean(DEFALUT_DATA_ON_BOOT_PROP,true);
        if (mDataEnabled[DctConstants.APN_DEFAULT_ID]) {
            mEnabledCount++;
        }
        SharedPreferences sp = PreferenceManager.getDefaultSharedPreferences(mPhone.getContext());
        mAutoAttachOnCreation = sp.getBoolean(PhoneBase.DATA_DISABLED_ON_BOOT_KEY, false);
        // Watch for changes to Settings.Global.DATA_ROAMING
        mDataRoamingSettingObserver = new DataRoamingSettingObserver(mPhone, mPhone.getContext());
        mDataRoamingSettingObserver.register();
        HandlerThread dcHandlerThread = new HandlerThread("DcHandlerThread");
        dcHandlerThread.start();
        Handler dcHandler = new Handler(dcHandlerThread.getLooper());
        mDcc = DcController.makeDcc(mPhone, this, dcHandler);
        mDcTesterFailBringUpAll = new DcTesterFailBringUpAll(mPhone, dcHandler);

#endif
}

ECode DcTrackerBase::Dispose()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (DBG) log("DCT.dispose");
        for (DcAsyncChannel dcac : mDataConnectionAcHashMap.values()) {
            dcac.disconnect();
        }
        mDataConnectionAcHashMap.clear();
        mIsDisposed = true;
        mPhone.getContext().unregisterReceiver(mIntentReceiver);
        mUiccController.unregisterForIccChanged(this);
        mDataRoamingSettingObserver.unregister();
        mDcc.dispose();
        mDcTesterFailBringUpAll.dispose();

#endif
}

ECode DcTrackerBase::GetActivity(
    /* [out] */ DctConstantsActivity* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return mActivity;

#endif
}

ECode DcTrackerBase::SetActivity(
    /* [in] */ DctConstantsActivity activity)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        log("setActivity = " + activity);
        mActivity = activity;
        mPhone.notifyDataActivity();

#endif
}

ECode DcTrackerBase::IsApnTypeActive(
    /* [in] */ const String& type,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        // TODO: support simultaneous with List instead
        if (PhoneConstants.APN_TYPE_DUN.equals(type)) {
            ApnSetting dunApn = fetchDunApn();
            if (dunApn != null) {
                return ((mActiveApn != null) && (dunApn.toString().equals(mActiveApn.toString())));
            }
        }
        return mActiveApn != null && mActiveApn.canHandleType(type);

#endif
}

ECode DcTrackerBase::FetchDunApn(
    /* [out] */ IApnSetting** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (SystemProperties.getBoolean("net.tethering.noprovisioning", false)) {
            log("fetchDunApn: net.tethering.noprovisioning=true ret: null");
            return null;
        }
        int bearer = -1;
        Context c = mPhone.getContext();
        String apnData = Settings.Global.getString(c.getContentResolver(),
                Settings.Global.TETHER_DUN_APN);
        List<ApnSetting> dunSettings = ApnSetting.arrayFromString(apnData);
        for (ApnSetting dunSetting : dunSettings) {
            IccRecords r = mIccRecords.get();
            String operator = (r != null) ? r.getOperatorNumeric() : "";
            if (dunSetting.bearer != 0) {
                if (bearer == -1) bearer = mPhone.getServiceState().getRilDataRadioTechnology();
                if (dunSetting.bearer != bearer) continue;
            }
            if (dunSetting.numeric.equals(operator)) {
                if (dunSetting.hasMvnoParams()) {
                    if (r != null &&
                            mvnoMatches(r, dunSetting.mvnoType, dunSetting.mvnoMatchData)) {
                        if (VDBG) {
                            log("fetchDunApn: global TETHER_DUN_APN dunSetting=" + dunSetting);
                        }
                        return dunSetting;
                    }
                } else {
                    if (VDBG) log("fetchDunApn: global TETHER_DUN_APN dunSetting=" + dunSetting);
                    return dunSetting;
                }
            }
        }
        apnData = c.getResources().getString(R.string.config_tether_apndata);
        ApnSetting dunSetting = ApnSetting.fromString(apnData);
        if (VDBG) log("fetchDunApn: config_tether_apndata dunSetting=" + dunSettings);
        return dunSetting;

#endif
}

ECode DcTrackerBase::GetActiveApnTypes(
    /* [out, callee] */ ArrayOf<String>** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        String[] result;
        if (mActiveApn != null) {
            result = mActiveApn.types;
        } else {
            result = new String[1];
            result[0] = PhoneConstants.APN_TYPE_DEFAULT;
        }
        return result;

#endif
}

ECode DcTrackerBase::GetActiveApnString(
    /* [in] */ const String& apnType,
    /* [out] */ String* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        String result = null;
        if (mActiveApn != null) {
            result = mActiveApn.apn;
        }
        return result;

#endif
}

ECode DcTrackerBase::SetDataOnRoamingEnabled(
    /* [in] */ Boolean enabled)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (getDataOnRoamingEnabled() != enabled) {
            final ContentResolver resolver = mPhone.getContext().getContentResolver();
            Settings.Global.putInt(resolver,
                    Settings.Global.DATA_ROAMING + mPhone.getPhoneId(), enabled ? 1 : 0);
            // will trigger handleDataOnRoamingChange() through observer
        }

#endif
}

ECode DcTrackerBase::GetDataOnRoamingEnabled(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        try {
            final ContentResolver resolver = mPhone.getContext().getContentResolver();
            return Settings.Global.getInt(resolver,
                    Settings.Global.DATA_ROAMING + mPhone.getPhoneId()) != 0;
        } catch (SettingNotFoundException snfe) {
            return false;
        }

#endif
}

ECode DcTrackerBase::SetDataEnabled(
    /* [in] */ Boolean enable)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Message msg = obtainMessage(DctConstants.CMD_SET_USER_DATA_ENABLE);
        msg.arg1 = enable ? 1 : 0;
        sendMessage(msg);

#endif
}

ECode DcTrackerBase::GetDataEnabled(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        try {
            final ContentResolver resolver = mPhone.getContext().getContentResolver();
            return Settings.Global.getInt(resolver,
                    Settings.Global.MOBILE_DATA + mPhone.getPhoneId()) != 0;
        } catch (SettingNotFoundException snfe) {
            return false;
        }

#endif
}

ECode DcTrackerBase::HandleMessage(
    /* [in] */ IMessage* msg)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        switch (msg.what) {
            case AsyncChannel.CMD_CHANNEL_DISCONNECTED: {
                log("DISCONNECTED_CONNECTED: msg=" + msg);
                DcAsyncChannel dcac = (DcAsyncChannel) msg.obj;
                mDataConnectionAcHashMap.remove(dcac.getDataConnectionIdSync());
                dcac.disconnected();
                break;
            }
            case DctConstants.EVENT_ENABLE_NEW_APN:
                onEnableApn(msg.arg1, msg.arg2);
                break;
            case DctConstants.EVENT_TRY_SETUP_DATA:
                String reason = null;
                if (msg.obj instanceof String) {
                    reason = (String) msg.obj;
                }
                onTrySetupData(reason);
                break;
            case DctConstants.EVENT_DATA_STALL_ALARM:
                onDataStallAlarm(msg.arg1);
                break;
            case DctConstants.EVENT_ROAMING_OFF:
                onRoamingOff();
                break;
            case DctConstants.EVENT_ROAMING_ON:
                onRoamingOn();
                break;
            case DctConstants.EVENT_RADIO_AVAILABLE:
                onRadioAvailable();
                break;
            case DctConstants.EVENT_RADIO_OFF_OR_NOT_AVAILABLE:
                onRadioOffOrNotAvailable();
                break;
            case DctConstants.EVENT_DATA_SETUP_COMPLETE:
                mCidActive = msg.arg1;
                onDataSetupComplete((AsyncResult) msg.obj);
                break;
            case DctConstants.EVENT_DATA_SETUP_COMPLETE_ERROR:
                onDataSetupCompleteError((AsyncResult) msg.obj);
                break;
            case DctConstants.EVENT_DISCONNECT_DONE:
                log("DataConnectionTracker.handleMessage: EVENT_DISCONNECT_DONE msg=" + msg);
                onDisconnectDone(msg.arg1, (AsyncResult) msg.obj);
                break;
            case DctConstants.EVENT_DISCONNECT_DC_RETRYING:
                log("DataConnectionTracker.handleMessage: EVENT_DISCONNECT_DC_RETRYING msg=" + msg);
                onDisconnectDcRetrying(msg.arg1, (AsyncResult) msg.obj);
                break;
            case DctConstants.EVENT_VOICE_CALL_STARTED:
                onVoiceCallStarted();
                break;
            case DctConstants.EVENT_VOICE_CALL_ENDED:
                onVoiceCallEnded();
                break;
            case DctConstants.EVENT_CLEAN_UP_ALL_CONNECTIONS: {
                onCleanUpAllConnections((String) msg.obj);
                break;
            }
            case DctConstants.EVENT_CLEAN_UP_CONNECTION: {
                boolean tearDown = (msg.arg1 == 0) ? false : true;
                onCleanUpConnection(tearDown, msg.arg2, (String) msg.obj);
                break;
            }
            case DctConstants.EVENT_SET_INTERNAL_DATA_ENABLE: {
                boolean enabled = (msg.arg1 == DctConstants.ENABLED) ? true : false;
                onSetInternalDataEnabled(enabled);
                break;
            }
            case DctConstants.EVENT_RESET_DONE: {
                if (DBG) log("EVENT_RESET_DONE");
                onResetDone((AsyncResult) msg.obj);
                break;
            }
            case DctConstants.CMD_SET_USER_DATA_ENABLE: {
                final boolean enabled = (msg.arg1 == DctConstants.ENABLED) ? true : false;
                if (DBG) log("CMD_SET_USER_DATA_ENABLE enabled=" + enabled);
                onSetUserDataEnabled(enabled);
                break;
            }
            case DctConstants.CMD_SET_DEPENDENCY_MET: {
                boolean met = (msg.arg1 == DctConstants.ENABLED) ? true : false;
                if (DBG) log("CMD_SET_DEPENDENCY_MET met=" + met);
                Bundle bundle = msg.getData();
                if (bundle != null) {
                    String apnType = (String)bundle.get(DctConstants.APN_TYPE_KEY);
                    if (apnType != null) {
                        onSetDependencyMet(apnType, met);
                    }
                }
                break;
            }
            case DctConstants.CMD_SET_POLICY_DATA_ENABLE: {
                final boolean enabled = (msg.arg1 == DctConstants.ENABLED) ? true : false;
                onSetPolicyDataEnabled(enabled);
                break;
            }
            case DctConstants.CMD_SET_ENABLE_FAIL_FAST_MOBILE_DATA: {
                sEnableFailFastRefCounter += (msg.arg1 == DctConstants.ENABLED) ? 1 : -1;
                if (DBG) {
                    log("CMD_SET_ENABLE_FAIL_FAST_MOBILE_DATA: "
                            + " sEnableFailFastRefCounter=" + sEnableFailFastRefCounter);
                }
                if (sEnableFailFastRefCounter < 0) {
                    final String s = "CMD_SET_ENABLE_FAIL_FAST_MOBILE_DATA: "
                            + "sEnableFailFastRefCounter:" + sEnableFailFastRefCounter + " < 0";
                    loge(s);
                    sEnableFailFastRefCounter = 0;
                }
                final boolean enabled = sEnableFailFastRefCounter > 0;
                if (DBG) {
                    log("CMD_SET_ENABLE_FAIL_FAST_MOBILE_DATA: enabled=" + enabled
                            + " sEnableFailFastRefCounter=" + sEnableFailFastRefCounter);
                }
                if (mFailFast != enabled) {
                    mFailFast = enabled;
                    mDataStallDetectionEnabled = !enabled;
                    if (mDataStallDetectionEnabled
                            && (getOverallState() == DctConstants.State.CONNECTED)
                            && (!mInVoiceCall ||
                                    mPhone.getServiceStateTracker()
                                        .isConcurrentVoiceAndDataAllowed())) {
                        if (DBG) log("CMD_SET_ENABLE_FAIL_FAST_MOBILE_DATA: start data stall");
                        stopDataStallAlarm();
                        startDataStallAlarm(DATA_STALL_NOT_SUSPECTED);
                    } else {
                        if (DBG) log("CMD_SET_ENABLE_FAIL_FAST_MOBILE_DATA: stop data stall");
                        stopDataStallAlarm();
                    }
                }
                break;
            }
            case DctConstants.CMD_ENABLE_MOBILE_PROVISIONING: {
                Bundle bundle = msg.getData();
                if (bundle != null) {
                    try {
                        mProvisioningUrl = (String)bundle.get(DctConstants.PROVISIONING_URL_KEY);
                    } catch(ClassCastException e) {
                        loge("CMD_ENABLE_MOBILE_PROVISIONING: provisioning url not a string" + e);
                        mProvisioningUrl = null;
                    }
                }
                if (TextUtils.isEmpty(mProvisioningUrl)) {
                    loge("CMD_ENABLE_MOBILE_PROVISIONING: provisioning url is empty, ignoring");
                    mIsProvisioning = false;
                    mProvisioningUrl = null;
                } else {
                    loge("CMD_ENABLE_MOBILE_PROVISIONING: provisioningUrl=" + mProvisioningUrl);
                    mIsProvisioning = true;
                    startProvisioningApnAlarm();
                }
                break;
            }
            case DctConstants.EVENT_PROVISIONING_APN_ALARM: {
                if (DBG) log("EVENT_PROVISIONING_APN_ALARM");
                ApnContext apnCtx = mApnContexts.get("default");
                if (apnCtx.isProvisioningApn() && apnCtx.isConnectedOrConnecting()) {
                    if (mProvisioningApnAlarmTag == msg.arg1) {
                        if (DBG) log("EVENT_PROVISIONING_APN_ALARM: Disconnecting");
                        mIsProvisioning = false;
                        mProvisioningUrl = null;
                        stopProvisioningApnAlarm();
                        sendCleanUpConnection(true, apnCtx);
                    } else {
                        if (DBG) {
                            log("EVENT_PROVISIONING_APN_ALARM: ignore stale tag,"
                                    + " mProvisioningApnAlarmTag:" + mProvisioningApnAlarmTag
                                    + " != arg1:" + msg.arg1);
                        }
                    }
                } else {
                    if (DBG) log("EVENT_PROVISIONING_APN_ALARM: Not connected ignore");
                }
                break;
            }
            case DctConstants.CMD_IS_PROVISIONING_APN: {
                if (DBG) log("CMD_IS_PROVISIONING_APN");
                boolean isProvApn;
                try {
                    String apnType = null;
                    Bundle bundle = msg.getData();
                    if (bundle != null) {
                        apnType = (String)bundle.get(DctConstants.APN_TYPE_KEY);
                    }
                    if (TextUtils.isEmpty(apnType)) {
                        loge("CMD_IS_PROVISIONING_APN: apnType is empty");
                        isProvApn = false;
                    } else {
                        isProvApn = isProvisioningApn(apnType);
                    }
                } catch (ClassCastException e) {
                    loge("CMD_IS_PROVISIONING_APN: NO provisioning url ignoring");
                    isProvApn = false;
                }
                if (DBG) log("CMD_IS_PROVISIONING_APN: ret=" + isProvApn);
                mReplyAc.replyToMessage(msg, DctConstants.CMD_IS_PROVISIONING_APN,
                        isProvApn ? DctConstants.ENABLED : DctConstants.DISABLED);
                break;
            }
            case DctConstants.EVENT_ICC_CHANGED: {
                onUpdateIcc();
                break;
            }
            case DctConstants.EVENT_RESTART_RADIO: {
                restartRadio();
                break;
            }
            case DctConstants.CMD_NET_STAT_POLL: {
                if (msg.arg1 == DctConstants.ENABLED) {
                    handleStartNetStatPoll((DctConstants.Activity)msg.obj);
                } else if (msg.arg1 == DctConstants.DISABLED) {
                    handleStopNetStatPoll((DctConstants.Activity)msg.obj);
                }
                break;
            }
            default:
                Rlog.e("DATA", "Unidentified event msg=" + msg);
                break;
        }

#endif
}

ECode DcTrackerBase::GetAnyDataEnabled(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        final boolean result;
        synchronized (mDataEnabledLock) {
            result = (mInternalDataEnabled && mUserDataEnabled && sPolicyDataEnabled
                    && (mEnabledCount != 0));
        }
        if (!result && DBG) log("getAnyDataEnabled " + result);
        return result;

#endif
}

ECode DcTrackerBase::IsEmergency(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        final boolean result;
        synchronized (mDataEnabledLock) {
            result = mPhone.isInEcm() || mPhone.isInEmergencyCall();
        }
        log("isEmergency: result=" + result);
        return result;

#endif
}

ECode DcTrackerBase::ApnTypeToId(
    /* [in] */ const String& type,
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (TextUtils.equals(type, PhoneConstants.APN_TYPE_DEFAULT)) {
            return DctConstants.APN_DEFAULT_ID;
        } else if (TextUtils.equals(type, PhoneConstants.APN_TYPE_MMS)) {
            return DctConstants.APN_MMS_ID;
        } else if (TextUtils.equals(type, PhoneConstants.APN_TYPE_SUPL)) {
            return DctConstants.APN_SUPL_ID;
        } else if (TextUtils.equals(type, PhoneConstants.APN_TYPE_DUN)) {
            return DctConstants.APN_DUN_ID;
        } else if (TextUtils.equals(type, PhoneConstants.APN_TYPE_HIPRI)) {
            return DctConstants.APN_HIPRI_ID;
        } else if (TextUtils.equals(type, PhoneConstants.APN_TYPE_IMS)) {
            return DctConstants.APN_IMS_ID;
        } else if (TextUtils.equals(type, PhoneConstants.APN_TYPE_FOTA)) {
            return DctConstants.APN_FOTA_ID;
        } else if (TextUtils.equals(type, PhoneConstants.APN_TYPE_CBS)) {
            return DctConstants.APN_CBS_ID;
        } else if (TextUtils.equals(type, PhoneConstants.APN_TYPE_IA)) {
            return DctConstants.APN_IA_ID;
        } else if (TextUtils.equals(type, PhoneConstants.APN_TYPE_EMERGENCY)) {
            return DctConstants.APN_EMERGENCY_ID;
        } else {
            return DctConstants.APN_INVALID_ID;
        }

#endif
}

ECode DcTrackerBase::ApnIdToType(
    /* [in] */ Int32 id,
    /* [out] */ String* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        switch (id) {
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
            log("Unknown id (" + id + ") in apnIdToType");
            return PhoneConstants.APN_TYPE_DEFAULT;
        }

#endif
}

ECode DcTrackerBase::GetLinkProperties(
    /* [in] */ const String& apnType,
    /* [out] */ ILinkProperties** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        int id = apnTypeToId(apnType);
        if (isApnIdEnabled(id)) {
            DcAsyncChannel dcac = mDataConnectionAcHashMap.get(0);
            return dcac.getLinkPropertiesSync();
        } else {
            return new LinkProperties();
        }

#endif
}

ECode DcTrackerBase::GetNetworkCapabilities(
    /* [in] */ const String& apnType,
    /* [out] */ INetworkCapabilities** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        int id = apnTypeToId(apnType);
        if (isApnIdEnabled(id)) {
            DcAsyncChannel dcac = mDataConnectionAcHashMap.get(0);
            return dcac.getNetworkCapabilitiesSync();
        } else {
            return new NetworkCapabilities();
        }

#endif
}

ECode DcTrackerBase::NotifyDataConnection(
    /* [in] */ const String& reason)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        for (int id = 0; id < DctConstants.APN_NUM_TYPES; id++) {
            if (mDataEnabled[id]) {
                mPhone.notifyDataConnection(reason, apnIdToType(id));
            }
        }
        notifyOffApnsOfAvailability(reason);

#endif
}

ECode DcTrackerBase::NotifyApnIdUpToCurrent(
    /* [in] */ const String& reason,
    /* [in] */ Int32 apnId)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        switch (mState) {
            case IDLE:
                break;
            case RETRYING:
            case CONNECTING:
            case SCANNING:
                mPhone.notifyDataConnection(reason, apnIdToType(apnId),
                        PhoneConstants.DataState.CONNECTING);
                break;
            case CONNECTED:
            case DISCONNECTING:
                mPhone.notifyDataConnection(reason, apnIdToType(apnId),
                        PhoneConstants.DataState.CONNECTING);
                mPhone.notifyDataConnection(reason, apnIdToType(apnId),
                        PhoneConstants.DataState.CONNECTED);
                break;
            default:
                // Ignore
                break;
        }

#endif
}

ECode DcTrackerBase::NotifyApnIdDisconnected(
    /* [in] */ const String& reason,
    /* [in] */ Int32 apnId)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mPhone.notifyDataConnection(reason, apnIdToType(apnId),
                PhoneConstants.DataState.DISCONNECTED);

#endif
}

ECode DcTrackerBase::NotifyOffApnsOfAvailability(
    /* [in] */ const String& reason)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (DBG) log("notifyOffApnsOfAvailability - reason= " + reason);
        for (int id = 0; id < DctConstants.APN_NUM_TYPES; id++) {
            if (!isApnIdEnabled(id)) {
                notifyApnIdDisconnected(reason, id);
            }
        }

#endif
}

ECode DcTrackerBase::IsApnTypeEnabled(
    /* [in] */ const String& apnType,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (apnType == null) {
            return false;
        } else {
            return isApnIdEnabled(apnTypeToId(apnType));
        }

#endif
}

ECode DcTrackerBase::IsApnIdEnabled(
    /* [in] */ Int32 id,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (id != DctConstants.APN_INVALID_ID) {
            return mDataEnabled[id];
        }
        return false;

#endif
}

ECode DcTrackerBase::SetEnabled(
    /* [in] */ Int32 id,
    /* [in] */ Boolean enable)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (DBG) {
            log("setEnabled(" + id + ", " + enable + ") with old state = " + mDataEnabled[id]
                    + " and enabledCount = " + mEnabledCount);
        }
        Message msg = obtainMessage(DctConstants.EVENT_ENABLE_NEW_APN);
        msg.arg1 = id;
        msg.arg2 = (enable ? DctConstants.ENABLED : DctConstants.DISABLED);
        sendMessage(msg);

#endif
}

ECode DcTrackerBase::OnEnableApn(
    /* [in] */ Int32 apnId,
    /* [in] */ Int32 enabled)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (DBG) {
            log("EVENT_APN_ENABLE_REQUEST apnId=" + apnId + ", apnType=" + apnIdToType(apnId) +
                    ", enabled=" + enabled + ", dataEnabled = " + mDataEnabled[apnId] +
                    ", enabledCount = " + mEnabledCount + ", isApnTypeActive = " +
                    isApnTypeActive(apnIdToType(apnId)));
        }
        if (enabled == DctConstants.ENABLED) {
            synchronized (this) {
                if (!mDataEnabled[apnId]) {
                    mDataEnabled[apnId] = true;
                    mEnabledCount++;
                }
            }
            String type = apnIdToType(apnId);
            if (!isApnTypeActive(type)) {
                mRequestedApnType = type;
                onEnableNewApn();
            } else {
                notifyApnIdUpToCurrent(Phone.REASON_APN_SWITCHED, apnId);
            }
        } else {
            // disable
            boolean didDisable = false;
            synchronized (this) {
                if (mDataEnabled[apnId]) {
                    mDataEnabled[apnId] = false;
                    mEnabledCount--;
                    didDisable = true;
                }
            }
            if (didDisable) {
                if ((mEnabledCount == 0) || (apnId == DctConstants.APN_DUN_ID)) {
                    mRequestedApnType = PhoneConstants.APN_TYPE_DEFAULT;
                    onCleanUpConnection(true, apnId, Phone.REASON_DATA_DISABLED);
                }
                // send the disconnect msg manually, since the normal route wont send
                // it (it's not enabled)
                notifyApnIdDisconnected(Phone.REASON_DATA_DISABLED, apnId);
                if (mDataEnabled[DctConstants.APN_DEFAULT_ID] == true
                        && !isApnTypeActive(PhoneConstants.APN_TYPE_DEFAULT)) {
                    // TODO - this is an ugly way to restore the default conn - should be done
                    // by a real contention manager and policy that disconnects the lower pri
                    // stuff as enable requests come in and pops them back on as we disable back
                    // down to the lower pri stuff
                    mRequestedApnType = PhoneConstants.APN_TYPE_DEFAULT;
                    onEnableNewApn();
                }
            }
        }

#endif
}

ECode DcTrackerBase::OnEnableNewApn()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below

#endif
}

ECode DcTrackerBase::OnResetDone(
    /* [in] */ IAsyncResult* ar)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (DBG) log("EVENT_RESET_DONE");
        String reason = null;
        if (ar.userObj instanceof String) {
            reason = (String) ar.userObj;
        }
        gotoIdleAndNotifyDataConnection(reason);

#endif
}

ECode DcTrackerBase::SetInternalDataEnabled(
    /* [in] */ Boolean enable,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (DBG)
            log("setInternalDataEnabled(" + enable + ")");
        Message msg = obtainMessage(DctConstants.EVENT_SET_INTERNAL_DATA_ENABLE);
        msg.arg1 = (enable ? DctConstants.ENABLED : DctConstants.DISABLED);
        sendMessage(msg);
        return true;

#endif
}

ECode DcTrackerBase::OnSetInternalDataEnabled(
    /* [in] */ Boolean enabled)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        synchronized (mDataEnabledLock) {
            mInternalDataEnabled = enabled;
            if (enabled) {
                log("onSetInternalDataEnabled: changed to enabled, try to setup data call");
                onTrySetupData(Phone.REASON_DATA_ENABLED);
            } else {
                log("onSetInternalDataEnabled: changed to disabled, cleanUpAllConnections");
                cleanUpAllConnections(null);
            }
        }

#endif
}

ECode DcTrackerBase::CleanUpAllConnections(
    /* [in] */ const String& cause)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Message msg = obtainMessage(DctConstants.EVENT_CLEAN_UP_ALL_CONNECTIONS);
        msg.obj = cause;
        sendMessage(msg);

#endif
}

ECode DcTrackerBase::IsDisconnected(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
#endif
}

ECode DcTrackerBase::OnSetUserDataEnabled(
    /* [in] */ Boolean enabled)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        synchronized (mDataEnabledLock) {
            if (mUserDataEnabled != enabled) {
                mUserDataEnabled = enabled;
                Settings.Global.putInt(mPhone.getContext().getContentResolver(),
                        Settings.Global.MOBILE_DATA + mPhone.getPhoneId(), enabled ? 1 : 0);
                if (getDataOnRoamingEnabled() == false &&
                        mPhone.getServiceState().getRoaming() == true) {
                    if (enabled) {
                        notifyOffApnsOfAvailability(Phone.REASON_ROAMING_ON);
                    } else {
                        notifyOffApnsOfAvailability(Phone.REASON_DATA_DISABLED);
                    }
                }
                if (enabled) {
                    onTrySetupData(Phone.REASON_DATA_ENABLED);
                } else {
                    onCleanUpAllConnections(Phone.REASON_DATA_SPECIFIC_DISABLED);
                }
            }
        }

#endif
}

ECode DcTrackerBase::OnSetDependencyMet(
    /* [in] */ const String& apnType,
    /* [in] */ Boolean met)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below

#endif
}

ECode DcTrackerBase::OnSetPolicyDataEnabled(
    /* [in] */ Boolean enabled)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        synchronized (mDataEnabledLock) {
            if (sPolicyDataEnabled != enabled) {
                sPolicyDataEnabled = enabled;
                if (enabled) {
                    onTrySetupData(Phone.REASON_DATA_ENABLED);
                } else {
                    onCleanUpAllConnections(Phone.REASON_DATA_SPECIFIC_DISABLED);
                }
            }
        }

#endif
}

ECode DcTrackerBase::GetReryConfig(
    /* [in] */ Boolean forDefault,
    /* [out] */ String* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        int nt = mPhone.getServiceState().getNetworkType();
        if ((nt == TelephonyManager.NETWORK_TYPE_CDMA) ||
            (nt == TelephonyManager.NETWORK_TYPE_1xRTT) ||
            (nt == TelephonyManager.NETWORK_TYPE_EVDO_0) ||
            (nt == TelephonyManager.NETWORK_TYPE_EVDO_A) ||
            (nt == TelephonyManager.NETWORK_TYPE_EVDO_B) ||
            (nt == TelephonyManager.NETWORK_TYPE_EHRPD)) {
            // CDMA variant
            return SystemProperties.get("ro.cdma.data_retry_config");
        } else {
            // Use GSM varient for all others.
            if (forDefault) {
                return SystemProperties.get("ro.gsm.data_retry_config");
            } else {
                return SystemProperties.get("ro.gsm.2nd_data_retry_config");
            }
        }

#endif
}

ECode DcTrackerBase::ResetPollStats()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mTxPkts = -1;
        mRxPkts = -1;
        mNetStatPollPeriod = POLL_NETSTAT_MILLIS;

#endif
}

ECode DcTrackerBase::GetOverallState(
    /* [out] */ DctConstantsState* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
#endif
}

ECode DcTrackerBase::StartNetStatPoll()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (getOverallState() == DctConstants.State.CONNECTED
                && mNetStatPollEnabled == false) {
            if (DBG) {
                log("startNetStatPoll");
            }
            resetPollStats();
            mNetStatPollEnabled = true;
            mPollNetStat.run();
        }

#endif
}

ECode DcTrackerBase::StopNetStatPoll()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mNetStatPollEnabled = false;
        removeCallbacks(mPollNetStat);
        if (DBG) {
            log("stopNetStatPoll");
        }

#endif
}

ECode DcTrackerBase::SendStartNetStatPoll(
    /* [in] */ DctConstantsActivity activity)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Message msg = obtainMessage(DctConstants.CMD_NET_STAT_POLL);
        msg.arg1 = DctConstants.ENABLED;
        msg.obj = activity;
        sendMessage(msg);

#endif
}

ECode DcTrackerBase::HandleStartNetStatPoll(
    /* [in] */ DctConstantsActivity activity)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        startNetStatPoll();
        startDataStallAlarm(DATA_STALL_NOT_SUSPECTED);
        setActivity(activity);

#endif
}

ECode DcTrackerBase::SendStopNetStatPoll(
    /* [in] */ DctConstantsActivity activity)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Message msg = obtainMessage(DctConstants.CMD_NET_STAT_POLL);
        msg.arg1 = DctConstants.DISABLED;
        msg.obj = activity;
        sendMessage(msg);

#endif
}

ECode DcTrackerBase::HandleStopNetStatPoll(
    /* [in] */ DctConstantsActivity activity)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        stopNetStatPoll();
        stopDataStallAlarm();
        setActivity(activity);

#endif
}

ECode DcTrackerBase::UpdateDataActivity()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        long sent, received;
        DctConstants.Activity newActivity;
        TxRxSum preTxRxSum = new TxRxSum(mTxPkts, mRxPkts);
        TxRxSum curTxRxSum = new TxRxSum();
        curTxRxSum.updateTxRxSum();
        mTxPkts = curTxRxSum.txPkts;
        mRxPkts = curTxRxSum.rxPkts;
        if (VDBG) {
            log("updateDataActivity: curTxRxSum=" + curTxRxSum + " preTxRxSum=" + preTxRxSum);
        }
        if (mNetStatPollEnabled && (preTxRxSum.txPkts > 0 || preTxRxSum.rxPkts > 0)) {
            sent = mTxPkts - preTxRxSum.txPkts;
            received = mRxPkts - preTxRxSum.rxPkts;
            if (VDBG)
                log("updateDataActivity: sent=" + sent + " received=" + received);
            if (sent > 0 && received > 0) {
                newActivity = DctConstants.Activity.DATAINANDOUT;
            } else if (sent > 0 && received == 0) {
                newActivity = DctConstants.Activity.DATAOUT;
            } else if (sent == 0 && received > 0) {
                newActivity = DctConstants.Activity.DATAIN;
            } else {
                newActivity = (mActivity == DctConstants.Activity.DORMANT) ?
                        mActivity : DctConstants.Activity.NONE;
            }
            if (mActivity != newActivity && mIsScreenOn) {
                if (VDBG)
                    log("updateDataActivity: newActivity=" + newActivity);
                mActivity = newActivity;
                mPhone.notifyDataActivity();
            }
        }

#endif
}

ECode DcTrackerBase::GetRecoveryAction(
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        int action = Settings.System.getInt(mPhone.getContext().getContentResolver(),
                "radio.data.stall.recovery.action", RecoveryAction.GET_DATA_CALL_LIST);
        if (VDBG_STALL) log("getRecoveryAction: " + action);
        return action;

#endif
}

ECode DcTrackerBase::PutRecoveryAction(
    /* [in] */ Int32 action)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Settings.System.putInt(mPhone.getContext().getContentResolver(),
                "radio.data.stall.recovery.action", action);
        if (VDBG_STALL) log("putRecoveryAction: " + action);

#endif
}

ECode DcTrackerBase::IsConnected(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return false;

#endif
}

ECode DcTrackerBase::DoRecovery()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (getOverallState() == DctConstants.State.CONNECTED) {
            // Go through a series of recovery steps, each action transitions to the next action
            int recoveryAction = getRecoveryAction();
            switch (recoveryAction) {
            case RecoveryAction.GET_DATA_CALL_LIST:
                EventLog.writeEvent(EventLogTags.DATA_STALL_RECOVERY_GET_DATA_CALL_LIST,
                        mSentSinceLastRecv);
                if (DBG) log("doRecovery() get data call list");
                mPhone.mCi.getDataCallList(obtainMessage(DctConstants.EVENT_DATA_STATE_CHANGED));
                putRecoveryAction(RecoveryAction.CLEANUP);
                break;
            case RecoveryAction.CLEANUP:
                EventLog.writeEvent(EventLogTags.DATA_STALL_RECOVERY_CLEANUP, mSentSinceLastRecv);
                if (DBG) log("doRecovery() cleanup all connections");
                cleanUpAllConnections(Phone.REASON_PDP_RESET);
                putRecoveryAction(RecoveryAction.REREGISTER);
                break;
            case RecoveryAction.REREGISTER:
                EventLog.writeEvent(EventLogTags.DATA_STALL_RECOVERY_REREGISTER,
                        mSentSinceLastRecv);
                if (DBG) log("doRecovery() re-register");
                mPhone.getServiceStateTracker().reRegisterNetwork(null);
                putRecoveryAction(RecoveryAction.RADIO_RESTART);
                break;
            case RecoveryAction.RADIO_RESTART:
                EventLog.writeEvent(EventLogTags.DATA_STALL_RECOVERY_RADIO_RESTART,
                        mSentSinceLastRecv);
                if (DBG) log("restarting radio");
                putRecoveryAction(RecoveryAction.RADIO_RESTART_WITH_PROP);
                restartRadio();
                break;
            case RecoveryAction.RADIO_RESTART_WITH_PROP:
                // This is in case radio restart has not recovered the data.
                // It will set an additional "gsm.radioreset" property to tell
                // RIL or system to take further action.
                // The implementation of hard reset recovery action is up to OEM product.
                // Once RADIO_RESET property is consumed, it is expected to set back
                // to false by RIL.
                EventLog.writeEvent(EventLogTags.DATA_STALL_RECOVERY_RADIO_RESTART_WITH_PROP, -1);
                if (DBG) log("restarting radio with gsm.radioreset to true");
                SystemProperties.set(RADIO_RESET_PROPERTY, "true");
                // give 1 sec so property change can be notified.
                try {
                    Thread.sleep(1000);
                } catch (InterruptedException e) {}
                restartRadio();
                putRecoveryAction(RecoveryAction.GET_DATA_CALL_LIST);
                break;
            default:
                throw new RuntimeException("doRecovery: Invalid recoveryAction=" +
                    recoveryAction);
            }
            mSentSinceLastRecv = 0;
        }

#endif
}

ECode DcTrackerBase::UpdateDataStallInfo()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        long sent, received;
        TxRxSum preTxRxSum = new TxRxSum(mDataStallTxRxSum);
        mDataStallTxRxSum.updateTxRxSum();
        if (VDBG_STALL) {
            log("updateDataStallInfo: mDataStallTxRxSum=" + mDataStallTxRxSum +
                    " preTxRxSum=" + preTxRxSum);
        }
        sent = mDataStallTxRxSum.txPkts - preTxRxSum.txPkts;
        received = mDataStallTxRxSum.rxPkts - preTxRxSum.rxPkts;
        if (RADIO_TESTS) {
            if (SystemProperties.getBoolean("radio.test.data.stall", false)) {
                log("updateDataStallInfo: radio.test.data.stall true received = 0;");
                received = 0;
            }
        }
        if ( sent > 0 && received > 0 ) {
            if (VDBG_STALL) log("updateDataStallInfo: IN/OUT");
            mSentSinceLastRecv = 0;
            putRecoveryAction(RecoveryAction.GET_DATA_CALL_LIST);
        } else if (sent > 0 && received == 0) {
            if (mPhone.getState() == PhoneConstants.State.IDLE) {
                mSentSinceLastRecv += sent;
            } else {
                mSentSinceLastRecv = 0;
            }
            if (DBG) {
                log("updateDataStallInfo: OUT sent=" + sent +
                        " mSentSinceLastRecv=" + mSentSinceLastRecv);
            }
        } else if (sent == 0 && received > 0) {
            if (VDBG_STALL) log("updateDataStallInfo: IN");
            mSentSinceLastRecv = 0;
            putRecoveryAction(RecoveryAction.GET_DATA_CALL_LIST);
        } else {
            if (VDBG_STALL) log("updateDataStallInfo: NONE");
        }

#endif
}

ECode DcTrackerBase::OnDataStallAlarm(
    /* [in] */ Int32 tag)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (mDataStallAlarmTag != tag) {
            if (DBG) {
                log("onDataStallAlarm: ignore, tag=" + tag + " expecting " + mDataStallAlarmTag);
            }
            return;
        }
        updateDataStallInfo();
        int hangWatchdogTrigger = Settings.Global.getInt(mResolver,
                Settings.Global.PDP_WATCHDOG_TRIGGER_PACKET_COUNT,
                NUMBER_SENT_PACKETS_OF_HANG);
        boolean suspectedStall = DATA_STALL_NOT_SUSPECTED;
        if (mSentSinceLastRecv >= hangWatchdogTrigger) {
            if (DBG) {
                log("onDataStallAlarm: tag=" + tag + " do recovery action=" + getRecoveryAction());
            }
            suspectedStall = DATA_STALL_SUSPECTED;
            sendMessage(obtainMessage(DctConstants.EVENT_DO_RECOVERY));
        } else {
            if (VDBG_STALL) {
                log("onDataStallAlarm: tag=" + tag + " Sent " + String.valueOf(mSentSinceLastRecv) +
                    " pkts since last received, < watchdogTrigger=" + hangWatchdogTrigger);
            }
        }
        startDataStallAlarm(suspectedStall);

#endif
}

ECode DcTrackerBase::StartDataStallAlarm(
    /* [in] */ Boolean suspectedStall)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        int nextAction = getRecoveryAction();
        int delayInMs;
        if (mDataStallDetectionEnabled && getOverallState() == DctConstants.State.CONNECTED) {
            // If screen is on or data stall is currently suspected, set the alarm
            // with an aggresive timeout.
            if (mIsScreenOn || suspectedStall || RecoveryAction.isAggressiveRecovery(nextAction)) {
                delayInMs = Settings.Global.getInt(mResolver,
                        Settings.Global.DATA_STALL_ALARM_AGGRESSIVE_DELAY_IN_MS,
                        DATA_STALL_ALARM_AGGRESSIVE_DELAY_IN_MS_DEFAULT);
            } else {
                delayInMs = Settings.Global.getInt(mResolver,
                        Settings.Global.DATA_STALL_ALARM_NON_AGGRESSIVE_DELAY_IN_MS,
                        DATA_STALL_ALARM_NON_AGGRESSIVE_DELAY_IN_MS_DEFAULT);
            }
            mDataStallAlarmTag += 1;
            if (VDBG_STALL) {
                log("startDataStallAlarm: tag=" + mDataStallAlarmTag +
                        " delay=" + (delayInMs / 1000) + "s");
            }
            Intent intent = new Intent(INTENT_DATA_STALL_ALARM);
            intent.putExtra(DATA_STALL_ALARM_TAG_EXTRA, mDataStallAlarmTag);
            mDataStallAlarmIntent = PendingIntent.getBroadcast(mPhone.getContext(), 0, intent,
                    PendingIntent.FLAG_UPDATE_CURRENT);
            mAlarmManager.set(AlarmManager.ELAPSED_REALTIME_WAKEUP,
                    SystemClock.elapsedRealtime() + delayInMs, mDataStallAlarmIntent);
        } else {
            if (VDBG_STALL) {
                log("startDataStallAlarm: NOT started, no connection tag=" + mDataStallAlarmTag);
            }
        }

#endif
}

ECode DcTrackerBase::StopDataStallAlarm()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (VDBG_STALL) {
            log("stopDataStallAlarm: current tag=" + mDataStallAlarmTag +
                    " mDataStallAlarmIntent=" + mDataStallAlarmIntent);
        }
        mDataStallAlarmTag += 1;
        if (mDataStallAlarmIntent != null) {
            mAlarmManager.cancel(mDataStallAlarmIntent);
            mDataStallAlarmIntent = null;
        }

#endif
}

ECode DcTrackerBase::RestartDataStallAlarm()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (isConnected() == false) return;
        // To be called on screen status change.
        // Do not cancel the alarm if it is set with aggressive timeout.
        int nextAction = getRecoveryAction();
        if (RecoveryAction.isAggressiveRecovery(nextAction)) {
            if (DBG) log("restartDataStallAlarm: action is pending. not resetting the alarm.");
            return;
        }
        if (VDBG_STALL) log("restartDataStallAlarm: stop then start.");
        stopDataStallAlarm();
        startDataStallAlarm(DATA_STALL_NOT_SUSPECTED);

#endif
}

ECode DcTrackerBase::SetInitialAttachApn()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        ApnSetting iaApnSetting = null;
        ApnSetting defaultApnSetting = null;
        ApnSetting firstApnSetting = null;
        log("setInitialApn: E mPreferredApn=" + mPreferredApn);
        if (mAllApnSettings != null && !mAllApnSettings.isEmpty()) {
            firstApnSetting = mAllApnSettings.get(0);
            log("setInitialApn: firstApnSetting=" + firstApnSetting);
            // Search for Initial APN setting and the first apn that can handle default
            for (ApnSetting apn : mAllApnSettings) {
                // Can't use apn.canHandleType(), as that returns true for APNs that have no type.
                if (ArrayUtils.contains(apn.types, PhoneConstants.APN_TYPE_IA) &&
                        apn.carrierEnabled) {
                    // The Initial Attach APN is highest priority so use it if there is one
                    log("setInitialApn: iaApnSetting=" + apn);
                    iaApnSetting = apn;
                    break;
                } else if ((defaultApnSetting == null)
                        && (apn.canHandleType(PhoneConstants.APN_TYPE_DEFAULT))) {
                    // Use the first default apn if no better choice
                    log("setInitialApn: defaultApnSetting=" + apn);
                    defaultApnSetting = apn;
                }
            }
        }
        // The priority of apn candidates from highest to lowest is:
        //   1) APN_TYPE_IA (Inital Attach)
        //   2) mPreferredApn, i.e. the current preferred apn
        //   3) The first apn that than handle APN_TYPE_DEFAULT
        //   4) The first APN we can find.
        ApnSetting initialAttachApnSetting = null;
        if (iaApnSetting != null) {
            if (DBG) log("setInitialAttachApn: using iaApnSetting");
            initialAttachApnSetting = iaApnSetting;
        } else if (mPreferredApn != null) {
            if (DBG) log("setInitialAttachApn: using mPreferredApn");
            initialAttachApnSetting = mPreferredApn;
        } else if (defaultApnSetting != null) {
            if (DBG) log("setInitialAttachApn: using defaultApnSetting");
            initialAttachApnSetting = defaultApnSetting;
        } else if (firstApnSetting != null) {
            if (DBG) log("setInitialAttachApn: using firstApnSetting");
            initialAttachApnSetting = firstApnSetting;
        }
        if (initialAttachApnSetting == null) {
            if (DBG) log("setInitialAttachApn: X There in no available apn");
        } else {
            if (DBG) log("setInitialAttachApn: X selected Apn=" + initialAttachApnSetting);
            mPhone.mCi.setInitialAttachApn(initialAttachApnSetting.apn,
                    initialAttachApnSetting.protocol, initialAttachApnSetting.authType,
                    initialAttachApnSetting.user, initialAttachApnSetting.password, null);
        }

#endif
}

ECode DcTrackerBase::SetDataProfilesAsNeeded()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (DBG) log("setDataProfilesAsNeeded");
        if (mAllApnSettings != null && !mAllApnSettings.isEmpty()) {
            ArrayList<DataProfile> dps = new ArrayList<DataProfile>();
            for (ApnSetting apn : mAllApnSettings) {
                if (apn.modemCognitive) {
                    DataProfile dp = new DataProfile(apn,
                            mPhone.getServiceState().getRoaming());
                    boolean isDup = false;
                    for(DataProfile dpIn : dps) {
                        if (dp.equals(dpIn)) {
                            isDup = true;
                            break;
                        }
                    }
                    if (!isDup) {
                        dps.add(dp);
                    }
                }
            }
            if(dps.size() > 0) {
                mPhone.mCi.setDataProfile(dps.toArray(new DataProfile[0]), null);
            }
        }

#endif
}

ECode DcTrackerBase::OnActionIntentProvisioningApnAlarm(
    /* [in] */ IIntent* intent)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (DBG) log("onActionIntentProvisioningApnAlarm: action=" + intent.getAction());
        Message msg = obtainMessage(DctConstants.EVENT_PROVISIONING_APN_ALARM,
                intent.getAction());
        msg.arg1 = intent.getIntExtra(PROVISIONING_APN_ALARM_TAG_EXTRA, 0);
        sendMessage(msg);

#endif
}

ECode DcTrackerBase::StartProvisioningApnAlarm()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        int delayInMs = Settings.Global.getInt(mResolver,
                                Settings.Global.PROVISIONING_APN_ALARM_DELAY_IN_MS,
                                PROVISIONING_APN_ALARM_DELAY_IN_MS_DEFAULT);
        if (Build.IS_DEBUGGABLE) {
            // Allow debug code to use a system property to provide another value
            String delayInMsStrg = Integer.toString(delayInMs);
            delayInMsStrg = System.getProperty(DEBUG_PROV_APN_ALARM, delayInMsStrg);
            try {
                delayInMs = Integer.parseInt(delayInMsStrg);
            } catch (NumberFormatException e) {
                loge("startProvisioningApnAlarm: e=" + e);
            }
        }
        mProvisioningApnAlarmTag += 1;
        if (DBG) {
            log("startProvisioningApnAlarm: tag=" + mProvisioningApnAlarmTag +
                    " delay=" + (delayInMs / 1000) + "s");
        }
        Intent intent = new Intent(INTENT_PROVISIONING_APN_ALARM);
        intent.putExtra(PROVISIONING_APN_ALARM_TAG_EXTRA, mProvisioningApnAlarmTag);
        mProvisioningApnAlarmIntent = PendingIntent.getBroadcast(mPhone.getContext(), 0, intent,
                PendingIntent.FLAG_UPDATE_CURRENT);
        mAlarmManager.set(AlarmManager.ELAPSED_REALTIME_WAKEUP,
                SystemClock.elapsedRealtime() + delayInMs, mProvisioningApnAlarmIntent);

#endif
}

ECode DcTrackerBase::StopProvisioningApnAlarm()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (DBG) {
            log("stopProvisioningApnAlarm: current tag=" + mProvisioningApnAlarmTag +
                    " mProvsioningApnAlarmIntent=" + mProvisioningApnAlarmIntent);
        }
        mProvisioningApnAlarmTag += 1;
        if (mProvisioningApnAlarmIntent != null) {
            mAlarmManager.cancel(mProvisioningApnAlarmIntent);
            mProvisioningApnAlarmIntent = null;
        }

#endif
}

ECode DcTrackerBase::SendCleanUpConnection(
    /* [in] */ Boolean tearDown,
    /* [in] */ IApnContext* apnContext)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (DBG)log("sendCleanUpConnection: tearDown=" + tearDown + " apnContext=" + apnContext);
        Message msg = obtainMessage(DctConstants.EVENT_CLEAN_UP_CONNECTION);
        msg.arg1 = tearDown ? 1 : 0;
        msg.arg2 = 0;
        msg.obj = apnContext;
        sendMessage(msg);

#endif
}

ECode DcTrackerBase::SendRestartRadio()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (DBG)log("sendRestartRadio:");
        Message msg = obtainMessage(DctConstants.EVENT_RESTART_RADIO);
        sendMessage(msg);

#endif
}

ECode DcTrackerBase::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        pw.println("DataConnectionTrackerBase:");
        pw.println(" RADIO_TESTS=" + RADIO_TESTS);
        pw.println(" mInternalDataEnabled=" + mInternalDataEnabled);
        pw.println(" mUserDataEnabled=" + mUserDataEnabled);
        pw.println(" sPolicyDataEnabed=" + sPolicyDataEnabled);
        pw.println(" mDataEnabled:");
        for(int i=0; i < mDataEnabled.length; i++) {
            pw.printf("  mDataEnabled[%d]=%b\n", i, mDataEnabled[i]);
        }
        pw.flush();
        pw.println(" mEnabledCount=" + mEnabledCount);
        pw.println(" mRequestedApnType=" + mRequestedApnType);
        pw.println(" mPhone=" + mPhone.getPhoneName());
        pw.println(" mPhoneId=" + mPhone.getPhoneId());
        pw.println(" mActivity=" + mActivity);
        pw.println(" mState=" + mState);
        pw.println(" mTxPkts=" + mTxPkts);
        pw.println(" mRxPkts=" + mRxPkts);
        pw.println(" mNetStatPollPeriod=" + mNetStatPollPeriod);
        pw.println(" mNetStatPollEnabled=" + mNetStatPollEnabled);
        pw.println(" mDataStallTxRxSum=" + mDataStallTxRxSum);
        pw.println(" mDataStallAlarmTag=" + mDataStallAlarmTag);
        pw.println(" mDataStallDetectionEanbled=" + mDataStallDetectionEnabled);
        pw.println(" mSentSinceLastRecv=" + mSentSinceLastRecv);
        pw.println(" mNoRecvPollCount=" + mNoRecvPollCount);
        pw.println(" mResolver=" + mResolver);
        pw.println(" mIsWifiConnected=" + mIsWifiConnected);
        pw.println(" mReconnectIntent=" + mReconnectIntent);
        pw.println(" mCidActive=" + mCidActive);
        pw.println(" mAutoAttachOnCreation=" + mAutoAttachOnCreation);
        pw.println(" mIsScreenOn=" + mIsScreenOn);
        pw.println(" mUniqueIdGenerator=" + mUniqueIdGenerator);
        pw.flush();
        pw.println(" ***************************************");
        DcController dcc = mDcc;
        if (dcc != null) {
            dcc.dump(fd, pw, args);
        } else {
            pw.println(" mDcc=null");
        }
        pw.println(" ***************************************");
        HashMap<Integer, DataConnection> dcs = mDataConnections;
        if (dcs != null) {
            Set<Entry<Integer, DataConnection> > mDcSet = mDataConnections.entrySet();
            pw.println(" mDataConnections: count=" + mDcSet.size());
            for (Entry<Integer, DataConnection> entry : mDcSet) {
                pw.printf(" *** mDataConnection[%d] \n", entry.getKey());
                entry.getValue().dump(fd, pw, args);
            }
        } else {
            pw.println("mDataConnections=null");
        }
        pw.println(" ***************************************");
        pw.flush();
        HashMap<String, Integer> apnToDcId = mApnToDataConnectionId;
        if (apnToDcId != null) {
            Set<Entry<String, Integer>> apnToDcIdSet = apnToDcId.entrySet();
            pw.println(" mApnToDataConnectonId size=" + apnToDcIdSet.size());
            for (Entry<String, Integer> entry : apnToDcIdSet) {
                pw.printf(" mApnToDataConnectonId[%s]=%d\n", entry.getKey(), entry.getValue());
            }
        } else {
            pw.println("mApnToDataConnectionId=null");
        }
        pw.println(" ***************************************");
        pw.flush();
        ConcurrentHashMap<String, ApnContext> apnCtxs = mApnContexts;
        if (apnCtxs != null) {
            Set<Entry<String, ApnContext>> apnCtxsSet = apnCtxs.entrySet();
            pw.println(" mApnContexts size=" + apnCtxsSet.size());
            for (Entry<String, ApnContext> entry : apnCtxsSet) {
                entry.getValue().dump(fd, pw, args);
            }
            pw.println(" ***************************************");
        } else {
            pw.println(" mApnContexts=null");
        }
        pw.flush();
        pw.println(" mActiveApn=" + mActiveApn);
        ArrayList<ApnSetting> apnSettings = mAllApnSettings;
        if (apnSettings != null) {
            pw.println(" mAllApnSettings size=" + apnSettings.size());
            for (int i=0; i < apnSettings.size(); i++) {
                pw.printf(" mAllApnSettings[%d]: %s\n", i, apnSettings.get(i));
            }
            pw.flush();
        } else {
            pw.println(" mAllApnSettings=null");
        }
        pw.println(" mPreferredApn=" + mPreferredApn);
        pw.println(" mIsPsRestricted=" + mIsPsRestricted);
        pw.println(" mIsDisposed=" + mIsDisposed);
        pw.println(" mIntentReceiver=" + mIntentReceiver);
        pw.println(" mDataRoamingSettingObserver=" + mDataRoamingSettingObserver);
        pw.flush();

#endif
}

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
