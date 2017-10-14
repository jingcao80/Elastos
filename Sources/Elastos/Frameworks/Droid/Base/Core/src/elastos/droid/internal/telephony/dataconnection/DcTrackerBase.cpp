//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "elastos/droid/R.h"
#include "elastos/droid/app/CPendingIntent.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/content/CIntentFilter.h"
#include "elastos/droid/database/ContentObserver.h"
#include "elastos/droid/internal/telephony/PhoneBase.h"
#include "elastos/droid/internal/telephony/uicc/CUiccControllerHelper.h"
#include "elastos/droid/internal/telephony/dataconnection/ApnContext.h"
#include "elastos/droid/internal/telephony/dataconnection/ApnSetting.h"
#include "elastos/droid/internal/telephony/dataconnection/CDataProfile.h"
#include "elastos/droid/internal/telephony/dataconnection/CDcTesterFailBringUpAll.h"
#include "elastos/droid/internal/telephony/dataconnection/DataConnection.h"
#include "elastos/droid/internal/telephony/dataconnection/DcController.h"
#include "elastos/droid/internal/telephony/dataconnection/DcTrackerBase.h"
#include "elastos/droid/internal/utility/ArrayUtils.h"
#include "elastos/droid/internal/utility/CAsyncChannel.h"
#include "elastos/droid/net/CLinkProperties.h"
#include "elastos/droid/net/CNetworkCapabilities.h"
#include "elastos/droid/net/TrafficStats.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/os/CHandler.h"
#include "elastos/droid/os/CHandlerThread.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/SystemProperties.h"
#include "elastos/droid/preference/PreferenceManager.h"
#include "elastos/droid/provider/Settings.h"
#include "elastos/droid/provider/Telephony.h"
#include "elastos/droid/telephony/SubscriptionManager.h"
#include "elastos/droid/text/TextUtils.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.Droid.Net.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.Wifi.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/Thread.h>
#include <elastos/droid/R.h>
#include <elastos/droid/text/TextUtils.h>
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/etl/List.h>
#include <elastos/utility/logging/Logger.h>

// using Elastos::Droid::Internal::Telephony::IEventLogTags;
using Elastos::Droid::App::CPendingIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Internal::Telephony::IServiceStateTracker;
using Elastos::Droid::Internal::Telephony::PhoneConstantsDataState_CONNECTED;
using Elastos::Droid::Internal::Telephony::PhoneConstantsDataState_CONNECTING;
using Elastos::Droid::Internal::Telephony::PhoneConstantsDataState_DISCONNECTED;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccControllerHelper;
using Elastos::Droid::Internal::Telephony::Uicc::CUiccControllerHelper;
using Elastos::Droid::Internal::Utility::ArrayUtils;
using Elastos::Droid::Internal::Utility::CAsyncChannel;
using Elastos::Droid::Internal::Utility::IStateMachine;
using Elastos::Droid::Net::CLinkProperties;
using Elastos::Droid::Net::CNetworkCapabilities;
using Elastos::Droid::Net::TrafficStats;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::CHandlerThread;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::SystemProperties;
using Elastos::Droid::Preference::PreferenceManager;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::Telephony;
using Elastos::Droid::R;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Droid::Telephony::ISubscriptionManager;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Telephony::SubscriptionManager;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Wifi::IWifiManager;
using Elastos::Core::AutoLock;
using Elastos::Core::CInteger32;
using Elastos::Core::CObject;
using Elastos::Core::CSystem;
using Elastos::Core::EIID_IComparator;
using Elastos::Core::EIID_IRunnable;
using Elastos::Core::ICharSequence;
using Elastos::Core::IInteger32;
using Elastos::Core::ISystem;
using Elastos::Core::IThread;
using Elastos::Core::StringUtils;
using Elastos::Core::Thread;
using Elastos::IO::IFlushable;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CHashMap;
using Elastos::Utility::CPriorityQueue;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::Etl::List;
using Elastos::Utility::ICollection;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::Concurrent::Atomic::CAtomicInteger32;
using Elastos::Utility::Concurrent::Atomic::CAtomicReference;
using Elastos::Utility::Concurrent::Atomic::IAtomicInteger32;
using Elastos::Utility::Concurrent::CConcurrentHashMap;

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
    return Reset();
}

ECode DcTrackerBase::TxRxSum::constructor(
    /* [in] */ Int64 txPkts,
    /* [in] */ Int64 rxPkts)
{
    mTxPkts = txPkts;
    mRxPkts = rxPkts;
    return NOERROR;
}

ECode DcTrackerBase::TxRxSum::constructor(
    /* [in] */ IDcTrackerBaseTxRxSum* sum)
{
    sum->GetTxPkts(&mTxPkts);
    sum->GetRxPkts(&mRxPkts);
    return NOERROR;
}

ECode DcTrackerBase::TxRxSum::Reset()
{
    mTxPkts = -1;
    mRxPkts = -1;
    return NOERROR;
}

ECode DcTrackerBase::TxRxSum::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    String rev;
    rev.AppendFormat("{txSum=%lld rxSum=%lld}", mTxPkts, mRxPkts);
    *result = rev;
    return NOERROR;
}

ECode DcTrackerBase::TxRxSum::UpdateTxRxSum()
{
    TrafficStats::GetMobileTcpTxPackets(&mTxPkts);
    TrafficStats::GetMobileTcpRxPackets(&mRxPkts);
    return NOERROR;
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
const Int32 DcTrackerBase::RecoveryAction::GET_DATA_CALL_LIST      = 0;
const Int32 DcTrackerBase::RecoveryAction::CLEANUP                 = 1;
const Int32 DcTrackerBase::RecoveryAction::REREGISTER              = 2;
const Int32 DcTrackerBase::RecoveryAction::RADIO_RESTART           = 3;
const Int32 DcTrackerBase::RecoveryAction::RADIO_RESTART_WITH_PROP = 4;

ECode DcTrackerBase::RecoveryAction::IsAggressiveRecovery(
    /* [in] */ Int32 value,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = ((value == RecoveryAction::CLEANUP) ||
            (value == RecoveryAction::REREGISTER) ||
            (value == RecoveryAction::RADIO_RESTART) ||
            (value == RecoveryAction::RADIO_RESTART_WITH_PROP));
    return NOERROR;
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
    String action;
    intent->GetAction(&action);
    if (DBG) mHost->Log("onReceive: action=%s", action.string());
    if (action.Equals(IIntent::ACTION_SCREEN_ON)) {
        mHost->mIsScreenOn = TRUE;
        mHost->StopNetStatPoll();
        mHost->StartNetStatPoll();
        mHost->RestartDataStallAlarm();
    } else if (action.Equals(IIntent::ACTION_SCREEN_OFF)) {
        mHost->mIsScreenOn = FALSE;
        mHost->StopNetStatPoll();
        mHost->StartNetStatPoll();
        mHost->RestartDataStallAlarm();
    } else if (action.StartWith(INTENT_RECONNECT_ALARM)) {
        if (DBG) mHost->Log("Reconnect alarm. Previous state was %d", mHost->mState);
        mHost->OnActionIntentReconnectAlarm(intent);
    } else if (action.StartWith(INTENT_RESTART_TRYSETUP_ALARM)) {
        if (DBG) mHost->Log("Restart trySetup alarm");
        mHost->OnActionIntentRestartTrySetupAlarm(intent);
    } else if (action.Equals(INTENT_DATA_STALL_ALARM)) {
        mHost->OnActionIntentDataStallAlarm(intent);
    } else if (action.Equals(INTENT_PROVISIONING_APN_ALARM)) {
        mHost->OnActionIntentProvisioningApnAlarm(intent);
    } else if (action.Equals(IWifiManager::NETWORK_STATE_CHANGED_ACTION)) {
        AutoPtr<IParcelable> parcelable;
        intent->GetParcelableExtra(IWifiManager::EXTRA_NETWORK_INFO, (IParcelable**)&parcelable);
        AutoPtr<Elastos::Droid::Net::INetworkInfo> networkInfo = Elastos::Droid::Net::INetworkInfo::Probe(parcelable);
        Boolean isConnected;
        networkInfo->IsConnected(&isConnected);
        mHost->mIsWifiConnected = (networkInfo != NULL && isConnected);
        if (DBG) mHost->Log("NETWORK_STATE_CHANGED_ACTION: mIsWifiConnected=%d", mHost->mIsWifiConnected);
    } else if (action.Equals(IWifiManager::WIFI_STATE_CHANGED_ACTION)) {
        Int32 value;
        intent->GetInt32Extra(IWifiManager::EXTRA_WIFI_STATE,
                IWifiManager::WIFI_STATE_UNKNOWN, &value);
        Boolean enabled = value == IWifiManager::WIFI_STATE_ENABLED;
        if (!enabled) {
            // when WiFi got disabled, the NETWORK_STATE_CHANGED_ACTION
            // quit and won't report disconnected until next enabling.
            mHost->mIsWifiConnected = FALSE;
        }
        if (DBG) mHost->Log("WIFI_STATE_CHANGED_ACTION: enabled=%d mIsWifiConnected=%d",
                enabled, mHost->mIsWifiConnected);
    }
    return NOERROR;
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
    mHost->UpdateDataActivity();
    if (mHost->mIsScreenOn) {
        Settings::Global::GetInt32(mHost->mResolver,
                ISettingsGlobal::PDP_WATCHDOG_POLL_INTERVAL_MS, POLL_NETSTAT_MILLIS, &mHost->mNetStatPollPeriod);
    } else {
        Settings::Global::GetInt32(mHost->mResolver,
                ISettingsGlobal::PDP_WATCHDOG_LONG_POLL_INTERVAL_MS,
                POLL_NETSTAT_SCREEN_OFF_MILLIS, &mHost->mNetStatPollPeriod);
    }
    if (mHost->mNetStatPollEnabled) {
        Boolean b;
        mHost->mDataConnectionTracker->PostDelayed(this, mHost->mNetStatPollPeriod, &b);
    }
    return NOERROR;
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
    ContentObserver::constructor(handler);
    context->GetContentResolver((IContentResolver**)&mHost->mResolver);
    return NOERROR;
}

ECode DcTrackerBase::DataRoamingSettingObserver::Register()
{
    Int32 phoneId;
    mHost->mPhone->GetPhoneId(&phoneId);
    AutoPtr<IUri> uri;
    Settings::Global::GetUriFor(ISettingsGlobal::DATA_ROAMING + StringUtils::ToString(phoneId), (IUri**)&uri);
    mHost->mResolver->RegisterContentObserver(uri, FALSE, this);
    return NOERROR;
}

ECode DcTrackerBase::DataRoamingSettingObserver::Unregister()
{
    return mHost->mResolver->UnregisterContentObserver(this);
}

ECode DcTrackerBase::DataRoamingSettingObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    // already running on mPhone handler thread
    AutoPtr<IServiceState> serviceState;
    IPhone::Probe(mHost->mPhone)->GetServiceState((IServiceState**)&serviceState);
    Boolean roaming;
    serviceState->GetRoaming(&roaming);
    if (roaming) {
        AutoPtr<IMessage> msg;
        mHost->ObtainMessage(IDctConstants::EVENT_ROAMING_ON, (IMessage**)&msg);
        Boolean b;
        mHost->SendMessage(msg, &b);
    }
    return NOERROR;
}

//=============================================================================
// DcTrackerBase::SubComparator
//=============================================================================
CAR_INTERFACE_IMPL(DcTrackerBase::SubComparator, Object, IComparator)

ECode DcTrackerBase::SubComparator::Compare(
    /* [in] */ IInterface* c1,
    /* [in] */ IInterface* c2,
    /* [out] */ Int32* result)
{
    Int32 priority1;
    IApnContext::Probe(c1)->GetPriority(&priority1);
    Int32 priority2;
    IApnContext::Probe(c2)->GetPriority(&priority2);

    *result = priority2 - priority1;
    return NOERROR;
}

//=============================================================================
// DcTrackerBase
//=============================================================================
CAR_INTERFACE_IMPL(DcTrackerBase, Handler, IDcTrackerBase)

const Boolean DcTrackerBase::DBG = TRUE;
const Boolean DcTrackerBase::VDBG = FALSE;
const Boolean DcTrackerBase::VDBG_STALL = TRUE;
const Boolean DcTrackerBase::RADIO_TESTS = FALSE;
Boolean DcTrackerBase::mIsCleanupRequired = FALSE;
const Int32 DcTrackerBase::DATA_CONNECTION_ACTIVE_PH_LINK_INACTIVE = 0;
const Int32 DcTrackerBase::DATA_CONNECTION_ACTIVE_PH_LINK_DOWN = 1;
const Int32 DcTrackerBase::DATA_CONNECTION_ACTIVE_PH_LINK_UP = 2;
const Int32 DcTrackerBase::APN_DELAY_DEFAULT_MILLIS = 20000;
const Int32 DcTrackerBase::APN_FAIL_FAST_DELAY_DEFAULT_MILLIS = 3000;
Boolean DcTrackerBase::sPolicyDataEnabled = TRUE;
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
const Boolean DcTrackerBase::DATA_STALL_SUSPECTED = TRUE;
const Boolean DcTrackerBase::DATA_STALL_NOT_SUSPECTED = FALSE;
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
    : mInternalDataEnabled(TRUE)
    , mUserDataEnabled(TRUE)
    , mRequestedApnType(IPhoneConstants::APN_TYPE_DEFAULT)
    , mRADIO_RESET_PROPERTY("gsm.radioreset")
    , mActivity(DctConstantsActivity_NONE)
    , mState(DctConstantsState_IDLE)
    , mTxPkts(0)
    , mRxPkts(0)
    , mNetStatPollPeriod(0)
    , mNetStatPollEnabled(FALSE)
    , mDataStallAlarmTag(0)
    , mSentSinceLastRecv(0)
    , mNoRecvPollCount(0)
    , mDataStallDetectionEnabled(TRUE)
    , mFailFast(FALSE)
    , mInVoiceCall(FALSE)
    , mIsWifiConnected(FALSE)
    , mCidActive(0)
    , mAutoAttachOnCreationConfig(FALSE)
    , mAutoAttachOnCreation(FALSE)
    , mIsScreenOn(TRUE)
    , mIsPsRestricted(FALSE)
    , mIsDisposed(FALSE)
    , mIsProvisioning(FALSE)
    , mProvisioningApnAlarmTag(0)
    , mDataEnabled(ArrayOf<Boolean>::Alloc(IDctConstants::APN_NUM_TYPES))
    , mEnabledCount(0)
{
    mDataStallAlarmTag = (Int32) SystemClock::GetElapsedRealtime();
    mProvisioningApnAlarmTag = (Int32) SystemClock::GetElapsedRealtime();
    Elastos::Core::CObject::New((IObject**)&mDataEnabledLock);
    CAtomicReference::New((IAtomicReference**)&mIccRecords);
    mDataStallTxRxSum = new TxRxSum();
    ((TxRxSum*) mDataStallTxRxSum.Get())->constructor(0, 0);
    CAtomicInteger32::New(0, (IAtomicInteger32**)&mUniqueIdGenerator);
    CHashMap::New((IHashMap**)&mDataConnections);
    CHashMap::New((IHashMap**)&mDataConnectionAcHashMap);
    CHashMap::New((IHashMap**)&mApnToDataConnectionId);
    CConcurrentHashMap::New((IConcurrentHashMap**)&mApnContexts);
    AutoPtr<IComparator> comparator = new SubComparator();
    CPriorityQueue::New(5, comparator, (IPriorityQueue**)&mPrioritySortedApnContexts);
    CArrayList::New((IArrayList**)&mAllApnSettings);
    CAsyncChannel::New((IAsyncChannel**)&mReplyAc);
    mIntentReceiver = new SubBroadcastReceiver(this);
    mPollNetStat = new SubRunnable(this);
}

ECode DcTrackerBase::GetInitialMaxRetry(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    if (mFailFast) {
        *result = 0;
        return NOERROR;
    }
    // Get default value from system property or use DEFAULT_MDC_INITIAL_RETRY
    Int32 value;
    SystemProperties::GetInt32(
            ISettingsGlobal::MDC_INITIAL_MAX_RETRY, DEFAULT_MDC_INITIAL_RETRY, &value);
    // Check if its been overridden
    return Settings::Global::GetInt32(mResolver,
            ISettingsGlobal::MDC_INITIAL_MAX_RETRY, value, result);
}

ECode DcTrackerBase::OnActionIntentReconnectAlarm(
    /* [in] */ IIntent* intent)
{
    String reason;
    intent->GetStringExtra(INTENT_RECONNECT_ALARM_EXTRA_REASON, &reason);
    String apnType;
    intent->GetStringExtra(INTENT_RECONNECT_ALARM_EXTRA_TYPE, &apnType);
    Int64 phoneSubId;
    mPhone->GetSubId(&phoneSubId);
    Int64 currSubId;
    intent->GetInt64Extra(IPhoneConstants::SUBSCRIPTION_KEY,
            ISubscriptionManager::INVALID_SUB_ID, &currSubId);
    Log("onActionIntentReconnectAlarm: currSubId = %lld phoneSubId=%lld", currSubId, phoneSubId);
    // Stop reconnect if not current subId is not correct.
    // FIXME STOPSHIP - phoneSubId is coming up as -1 way after boot and failing this.
//        if ((currSubId == SubscriptionManager.INVALID_SUB_ID) || (currSubId != phoneSubId)) {
//            log("receive ReconnectAlarm but subId incorrect, ignore");
//            return;
//        }
    AutoPtr<IInterface> obj;
    mApnContexts->Get(StringUtils::ParseCharSequence(apnType), (IInterface**)&obj);
    AutoPtr<IApnContext> apnContext = IApnContext::Probe(obj);
    if (DBG) {
        Log("onActionIntentReconnectAlarm: mState=%d reason=%s apnType=%s apnContext=%s mDataConnectionAsyncChannels=%s",
                mState, reason.string(), apnType.string(), TO_CSTR(apnContext), TO_CSTR(mDataConnectionAcHashMap));
    }
    Boolean isEnabled;
    apnContext->IsEnabled(&isEnabled);
    if ((apnContext != NULL) && (isEnabled)) {
        apnContext->SetReason(reason);
        DctConstantsState apnContextState;
        apnContext->GetState(&apnContextState);
        if (DBG) {
            Log("onActionIntentReconnectAlarm: apnContext state=%s", apnContextState);
        }
        if ((apnContextState == DctConstantsState_FAILED)
                || (apnContextState == DctConstantsState_IDLE)) {
            if (DBG) {
                Log("onActionIntentReconnectAlarm: state is FAILED|IDLE, disassociate");
            }
            AutoPtr<IDcAsyncChannel> dcac;
            apnContext->GetDcAc((IDcAsyncChannel**)&dcac);
            if (dcac != NULL) {
                dcac->TearDown(apnContext, String(""), NULL);
            }
            apnContext->SetDataConnectionAc(NULL);
            apnContext->SetState(DctConstantsState_IDLE);
        } else {
            if (DBG) Log("onActionIntentReconnectAlarm: keep associated");
        }
        // TODO: IF already associated should we send the EVENT_TRY_SETUP_DATA???
        AutoPtr<IMessage> msg;
        ObtainMessage(IDctConstants::EVENT_TRY_SETUP_DATA, apnContext, (IMessage**)&msg);
        Boolean b;
        SendMessage(msg, &b);
        apnContext->SetReconnectIntent(NULL);
    }
    return NOERROR;
}

ECode DcTrackerBase::OnActionIntentRestartTrySetupAlarm(
    /* [in] */ IIntent* intent)
{
    String apnType;
    intent->GetStringExtra(INTENT_RESTART_TRYSETUP_ALARM_EXTRA_TYPE, &apnType);
    AutoPtr<IInterface> obj;
    mApnContexts->Get(StringUtils::ParseCharSequence(apnType), (IInterface**)&obj);
    AutoPtr<IApnContext> apnContext = IApnContext::Probe(obj);
    if (DBG) {
        Log("onActionIntentRestartTrySetupAlarm: mState=%d apnType=%s apnContext=%s mDataConnectionAsyncChannels=%s",
                mState, apnType.string(), TO_CSTR(apnContext), TO_CSTR(mDataConnectionAcHashMap));
    }
    AutoPtr<IMessage> msg;
    ObtainMessage(IDctConstants::EVENT_TRY_SETUP_DATA, apnContext, (IMessage**)&msg);
    Boolean b;
    SendMessage(msg, &b);
    return NOERROR;
}

ECode DcTrackerBase::OnActionIntentDataStallAlarm(
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (VDBG_STALL) Log("onActionIntentDataStallAlarm: action=%s", action.string());
    AutoPtr<IMessage> msg;
    ObtainMessage(IDctConstants::EVENT_DATA_STALL_ALARM, StringUtils::ParseCharSequence(action), (IMessage**)&msg);
    Int32 value;
    intent->GetInt32Extra(DATA_STALL_ALARM_TAG_EXTRA, 0, &value);
    msg->SetArg1(value);
    Boolean b;
    SendMessage(msg, &b);
    return NOERROR;
}

ECode DcTrackerBase::constructor(
    /* [in] */ IPhoneBase* phone)
{
    Handler::constructor();
    mPhone = phone;
    if (DBG) Log("DCT.constructor");
    AutoPtr<IContext> context;
    IPhone::Probe(mPhone)->GetContext((IContext**)&context);
    context->GetContentResolver((IContentResolver**)&mResolver);
    AutoPtr<IUiccControllerHelper> ucHelper;
    CUiccControllerHelper::AcquireSingleton((IUiccControllerHelper**)&ucHelper);
    ucHelper->GetInstance((IUiccController**)&mUiccController);
    mUiccController->RegisterForIccChanged(this, IDctConstants::EVENT_ICC_CHANGED, NULL);
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::ALARM_SERVICE, (IInterface**)&obj);
    mAlarmManager = IAlarmManager::Probe(obj);
    obj = NULL;
    context->GetSystemService(IContext::CONNECTIVITY_SERVICE, (IInterface**)&obj);
    mCm = IConnectivityManager::Probe(obj);
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(IIntent::ACTION_SCREEN_ON);
    filter->AddAction(IIntent::ACTION_SCREEN_OFF);
    filter->AddAction(IWifiManager::NETWORK_STATE_CHANGED_ACTION);
    filter->AddAction(IWifiManager::WIFI_STATE_CHANGED_ACTION);
    filter->AddAction(INTENT_DATA_STALL_ALARM);
    filter->AddAction(INTENT_PROVISIONING_APN_ALARM);
    AutoPtr<IContentResolver> contentResolver;
    context->GetContentResolver((IContentResolver**)&contentResolver);
    Int32 phoneId;
    mPhone->GetPhoneId(&phoneId);
    Int32 i32;
    Settings::Global::GetInt32(contentResolver, ISettingsGlobal::MOBILE_DATA + StringUtils::ToString(phoneId), 1, &i32);
    mUserDataEnabled = i32 == 1;
    AutoPtr<IIntent> intent;
    context->RegisterReceiver(mIntentReceiver, filter, String(NULL), IHandler::Probe(mPhone), (IIntent**)&intent);
    // This preference tells us 1) initial condition for "dataEnabled",
    // and 2) whether the RIL will setup the baseband to auto-PS attach.
    SystemProperties::GetBoolean(DEFALUT_DATA_ON_BOOT_PROP, TRUE, &(*mDataEnabled)[IDctConstants::APN_DEFAULT_ID]);
    if (mDataEnabled[IDctConstants::APN_DEFAULT_ID]) {
        mEnabledCount++;
    }
    AutoPtr<ISharedPreferences> sp;
    PreferenceManager::GetDefaultSharedPreferences(context, (ISharedPreferences**)&sp);
    sp->GetBoolean(IPhoneBase::DATA_DISABLED_ON_BOOT_KEY, FALSE, &mAutoAttachOnCreation);
    // Watch for changes to Settings.Global.DATA_ROAMING
    mDataRoamingSettingObserver = new DataRoamingSettingObserver(this);
    mDataRoamingSettingObserver->constructor(IHandler::Probe(mPhone), context);
    mDataRoamingSettingObserver->Register();
    AutoPtr<IHandlerThread> dcHandlerThread;
    CHandlerThread::New(String("DcHandlerThread"), (IHandlerThread**)&dcHandlerThread);
    IThread::Probe(dcHandlerThread)->Start();
    AutoPtr<IHandler> dcHandler;
    CHandler::New((IHandler**)&dcHandler);
    DcController::MakeDcc(mPhone, this, dcHandler, (IDcController**)&mDcc);
    CDcTesterFailBringUpAll::New(mPhone, dcHandler, (IDcTesterFailBringUpAll**)&mDcTesterFailBringUpAll);
    return NOERROR;
}

ECode DcTrackerBase::Dispose()
{
    if (DBG) Log("DCT.dispose");
    AutoPtr<ICollection> values;
    mDataConnectionAcHashMap->GetValues((ICollection**)&values);
    AutoPtr<IIterator> it;
    values->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IDcAsyncChannel> dcac = IDcAsyncChannel::Probe(obj);
        IAsyncChannel::Probe(dcac)->Disconnect();
    }
    mDataConnectionAcHashMap->Clear();
    mIsDisposed = TRUE;
    AutoPtr<IContext> context;
    IPhone::Probe(mPhone)->GetContext((IContext**)&context);
    context->UnregisterReceiver(mIntentReceiver);
    if (mUiccController)
        mUiccController->UnregisterForIccChanged(this);
    mDataRoamingSettingObserver->Unregister();
    ((DcController*) mDcc.Get())->Dispose();
    ((DcController*) mDcTesterFailBringUpAll.Get())->Dispose();
    return NOERROR;
}

ECode DcTrackerBase::GetActivity(
    /* [out] */ DctConstantsActivity* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mActivity;
    return NOERROR;
}

ECode DcTrackerBase::SetActivity(
    /* [in] */ DctConstantsActivity activity)
{
    Log("setActivity = %d", activity);
    mActivity = activity;
    IPhone::Probe(mPhone)->NotifyDataActivity();
    return NOERROR;
}

ECode DcTrackerBase::IsApnTypeActive(
    /* [in] */ const String& type,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    // TODO: support simultaneous with List instead
    if (IPhoneConstants::APN_TYPE_DUN.Equals(type)) {
        AutoPtr<IApnSetting> dunApn;
        FetchDunApn((IApnSetting**)&dunApn);
        if (dunApn != NULL) {
            *result = ((mActiveApn != NULL) && (TO_STR(dunApn).Equals(TO_STR(mActiveApn))));
            return NOERROR;
        }
    }
    Boolean canHandleType;
    mActiveApn->CanHandleType(type, &canHandleType);
    *result = mActiveApn != NULL && canHandleType;
    return NOERROR;
}

ECode DcTrackerBase::FetchDunApn(
    /* [out] */ IApnSetting** result)
{
    VALIDATE_NOT_NULL(result)

    Boolean b;
    SystemProperties::GetBoolean(String("net.tethering.noprovisioning"), FALSE, &b);
    if (b) {
        Log("fetchDunApn: net.tethering.noprovisioning=true ret: null");
        *result = NULL;
        return NOERROR;
    }
    Int32 bearer = -1;
    AutoPtr<IContext> c;
    IPhone::Probe(mPhone)->GetContext((IContext**)&c);
    AutoPtr<IContentResolver> contentResolver;
    c->GetContentResolver((IContentResolver**)&contentResolver);
    String apnData;
    Settings::Global::GetString(contentResolver,
            ISettingsGlobal::TETHER_DUN_APN, &apnData);
    AutoPtr<IList> dunSettings;
    ApnSetting::ArrayFromString(apnData, (IList**)&dunSettings);
    AutoPtr<IIterator> it;
    dunSettings->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IApnSetting> dunSetting = IApnSetting::Probe(obj);
        obj = NULL;
        mIccRecords->Get((IInterface**)&obj);
        AutoPtr<IIccRecords> r = IIccRecords::Probe(obj);
        String _operator;
        if (r != NULL) {
            r->GetOperatorNumeric(&_operator);
        }
        else {
            _operator = "";
        }
        Int32 dunSettingBearer;
        dunSetting->GetBearer(&dunSettingBearer);
        if (dunSettingBearer != 0) {
            AutoPtr<IServiceState> serviceState;
            IPhone::Probe(mPhone)->GetServiceState((IServiceState**)&serviceState);
            if (bearer == -1) serviceState->GetRilDataRadioTechnology(&bearer);
            if (dunSettingBearer != bearer) continue;
        }
        String numeric;
        dunSetting->GetNumeric(&numeric);
        if (numeric.Equals(_operator)) {
            Boolean hasMvnoParams;
            dunSetting->HasMvnoParams(&hasMvnoParams);
            if (hasMvnoParams) {
                String dunSettingMvnoMatchData;
                dunSetting->GetMvnoMatchData(&dunSettingMvnoMatchData);
                String dunSettingMvnoType;
                dunSetting->GetMvnoType(&dunSettingMvnoType);
                Boolean isMvnoMatches;
                MvnoMatches(r, dunSettingMvnoType, dunSettingMvnoMatchData, &isMvnoMatches);
                if (r != NULL && isMvnoMatches) {
                    if (VDBG) {
                        Log("fetchDunApn: global TETHER_DUN_APN dunSetting=%s", TO_CSTR(dunSetting));
                    }
                    *result = dunSetting;
                    REFCOUNT_ADD(*result)
                    return NOERROR;
                }
            } else {
                if (VDBG) Log("fetchDunApn: global TETHER_DUN_APN dunSetting=%s", TO_CSTR(dunSetting));
                *result = dunSetting;
                REFCOUNT_ADD(*result)
                return NOERROR;
            }
        }
    }
    AutoPtr<IResources> res;
    c->GetResources((IResources**)&res);
    res->GetString(R::string::config_tether_apndata, &apnData);
    AutoPtr<IApnSetting> dunSetting;
    ApnSetting::FromString(apnData, (IApnSetting**)&dunSetting);
    if (VDBG) Log("fetchDunApn: config_tether_apndata dunSetting=%s", TO_CSTR(dunSettings));
    *result = dunSetting;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode DcTrackerBase::GetActiveApnTypes(
    /* [out, callee] */ ArrayOf<String>** _result)
{
    VALIDATE_NOT_NULL(_result)

    AutoPtr<ArrayOf<String> > result;
    if (mActiveApn != NULL) {
        mActiveApn->GetTypes((ArrayOf<String>**)&result);
    } else {
        result = ArrayOf<String>::Alloc(1);
        (*result)[0] = IPhoneConstants::APN_TYPE_DEFAULT;
    }
    *_result = result;
    REFCOUNT_ADD(*_result)
    return NOERROR;
}

ECode DcTrackerBase::GetActiveApnString(
    /* [in] */ const String& apnType,
    /* [out] */ String* _result)
{
    VALIDATE_NOT_NULL(_result)

    String result(NULL);
    if (mActiveApn != NULL) {
        mActiveApn->GetApn(&result);
    }
    *_result = result;
    return NOERROR;
}

ECode DcTrackerBase::SetDataOnRoamingEnabled(
    /* [in] */ Boolean enabled)
{
    Boolean isDataOnRoamingEnabled;
    GetDataOnRoamingEnabled(&isDataOnRoamingEnabled);
    if (isDataOnRoamingEnabled != enabled) {
        AutoPtr<IContext> context;
        IPhone::Probe(mPhone)->GetContext((IContext**)&context);
        AutoPtr<IContentResolver> resolver;
        context->GetContentResolver((IContentResolver**)&resolver);
        Int32 phoneId;
        mPhone->GetPhoneId(&phoneId);
        Boolean b;
        Settings::Global::PutInt32(resolver, ISettingsGlobal::DATA_ROAMING + StringUtils::ToString(phoneId), enabled ? 1 : 0, &b);
        // will trigger HandleDataOnRoamingChange() through observer
    }
    return NOERROR;
}

ECode DcTrackerBase::GetDataOnRoamingEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    // try {
    ECode ec;
    do {
        AutoPtr<IContext> context;
        ec = IPhone::Probe(mPhone)->GetContext((IContext**)&context);
        if (FAILED(ec)) break;
        AutoPtr<IContentResolver> resolver;
        ec = context->GetContentResolver((IContentResolver**)&resolver);
        if (FAILED(ec)) break;
        Int32 phoneId;
        ec = mPhone->GetPhoneId(&phoneId);
        if (FAILED(ec)) break;
        Int32 i32;
        Settings::Global::GetInt32(resolver, ISettingsGlobal::DATA_ROAMING + StringUtils::ToString(phoneId), &i32);
        *result = i32 != 0;
        return NOERROR;
    } while(FALSE);
    // } catch (SettingNotFoundException snfe) {
    if ((ECode) E_SETTING_NOT_FOUND_EXCEPTION == ec) {
        *result = FALSE;
        return NOERROR;
    }
    // }
    return NOERROR;
}

ECode DcTrackerBase::SetDataEnabled(
    /* [in] */ Boolean enable)
{
    AutoPtr<IMessage> msg;
    ObtainMessage(IDctConstants::CMD_SET_USER_DATA_ENABLE, (IMessage**)&msg);
    msg->SetArg1(enable ? 1 : 0);
    Boolean b;
    SendMessage(msg, &b);
    return NOERROR;
}

ECode DcTrackerBase::GetDataEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    // try {
    ECode ec;
    do {
        AutoPtr<IContext> context;
        ec = IPhone::Probe(mPhone)->GetContext((IContext**)&context);
        if (FAILED(ec)) break;
        AutoPtr<IContentResolver> resolver;
        ec = context->GetContentResolver((IContentResolver**)&resolver);
        if (FAILED(ec)) break;
        Int32 phoneId;
        ec = mPhone->GetPhoneId(&phoneId);
        if (FAILED(ec)) break;
        Int32 i32;
        Settings::Global::GetInt32(resolver, ISettingsGlobal::MOBILE_DATA + StringUtils::ToString(phoneId), &i32);
        *result = i32 != 0;
        return NOERROR;
    } while(FALSE);
    // } catch (SettingNotFoundException snfe) {
    if ((ECode) E_SETTING_NOT_FOUND_EXCEPTION == ec) {
        *result = FALSE;
        return NOERROR;
    }
    // }
    return NOERROR;
}

ECode DcTrackerBase::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 msgWhat;
    msg->GetWhat(&msgWhat);
    AutoPtr<IInterface> msgObj;
    msg->GetObj((IInterface**)&msgObj);
    Int32 msgArg1;
    msg->GetArg1(&msgArg1);
    Int32 msgArg2;
    msg->GetArg2(&msgArg2);
    switch (msgWhat) {
        case IAsyncChannel::CMD_CHANNEL_DISCONNECTED: {
            Log("DISCONNECTED_CONNECTED: msg=%s", TO_CSTR(msg));
            AutoPtr<IDcAsyncChannel> dcac = IDcAsyncChannel::Probe(msgObj);
            Int32 dataConnectionIdSync;
            dcac->GetDataConnectionIdSync(&dataConnectionIdSync);
            AutoPtr<IInteger32> i32;
            CInteger32::New(dataConnectionIdSync, (IInteger32**)&i32);
            mDataConnectionAcHashMap->Remove(i32);
            IAsyncChannel::Probe(dcac)->Disconnected();
            break;
        }
        case IDctConstants::EVENT_ENABLE_NEW_APN:
            OnEnableApn(msgArg1, msgArg2);
            break;
        case IDctConstants::EVENT_TRY_SETUP_DATA: {
            String reason(NULL);
            if (ICharSequence::Probe(msgObj) != NULL) {
                reason = TO_STR(msgObj);
            }
            Boolean b;
            OnTrySetupData(reason, &b);
            break;
        }
        case IDctConstants::EVENT_DATA_STALL_ALARM:
            OnDataStallAlarm(msgArg1);
            break;
        case IDctConstants::EVENT_ROAMING_OFF:
            OnRoamingOff();
            break;
        case IDctConstants::EVENT_ROAMING_ON:
            OnRoamingOn();
            break;
        case IDctConstants::EVENT_RADIO_AVAILABLE:
            OnRadioAvailable();
            break;
        case IDctConstants::EVENT_RADIO_OFF_OR_NOT_AVAILABLE:
            OnRadioOffOrNotAvailable();
            break;
        case IDctConstants::EVENT_DATA_SETUP_COMPLETE:
            mCidActive = msgArg1;
            OnDataSetupComplete(IAsyncResult::Probe(msgObj));
            break;
        case IDctConstants::EVENT_DATA_SETUP_COMPLETE_ERROR:
            OnDataSetupCompleteError(IAsyncResult::Probe(msgObj));
            break;
        case IDctConstants::EVENT_DISCONNECT_DONE:
            Log("DataConnectionTracker.handleMessage: EVENT_DISCONNECT_DONE msg=%s", TO_CSTR(msg));
            OnDisconnectDone(msgArg1, IAsyncResult::Probe(msgObj));
            break;
        case IDctConstants::EVENT_DISCONNECT_DC_RETRYING:
            Log("DataConnectionTracker.handleMessage: EVENT_DISCONNECT_DC_RETRYING msg=%s", TO_CSTR(msg));
            OnDisconnectDcRetrying(msgArg1, IAsyncResult::Probe(msgObj));
            break;
        case IDctConstants::EVENT_VOICE_CALL_STARTED:
            OnVoiceCallStarted();
            break;
        case IDctConstants::EVENT_VOICE_CALL_ENDED:
            OnVoiceCallEnded();
            break;
        case IDctConstants::EVENT_CLEAN_UP_ALL_CONNECTIONS: {
            OnCleanUpAllConnections(TO_STR(msgObj));
            break;
        }
        case IDctConstants::EVENT_CLEAN_UP_CONNECTION: {
            Boolean tearDown = (msgArg1 == 0) ? FALSE : TRUE;
            OnCleanUpConnection(tearDown, msgArg2, TO_STR(msgObj));
            break;
        }
        case IDctConstants::EVENT_SET_INTERNAL_DATA_ENABLE: {
            Boolean enabled = (msgArg1 == IDctConstants::ENABLED) ? TRUE : FALSE;
            OnSetInternalDataEnabled(enabled);
            break;
        }
        case IDctConstants::EVENT_RESET_DONE: {
            if (DBG) Log("EVENT_RESET_DONE");
            OnResetDone(IAsyncResult::Probe(msgObj));
            break;
        }
        case IDctConstants::CMD_SET_USER_DATA_ENABLE: {
            Boolean enabled = (msgArg1 == IDctConstants::ENABLED) ? TRUE : FALSE;
            if (DBG) Log("CMD_SET_USER_DATA_ENABLE enabled=%d", enabled);
            OnSetUserDataEnabled(enabled);
            break;
        }
        case IDctConstants::CMD_SET_DEPENDENCY_MET: {
            Boolean met = (msgArg1 == IDctConstants::ENABLED) ? TRUE : FALSE;
            if (DBG) Log("CMD_SET_DEPENDENCY_MET met=%d", met);
            AutoPtr<IBundle> bundle;
            msg->GetData((IBundle**)&bundle);
            if (bundle != NULL) {
                AutoPtr<IInterface> obj;
                bundle->Get(IDctConstants::APN_TYPE_KEY, (IInterface**)&obj);
                String apnType;
                IObject::Probe(obj)->ToString(&apnType);
                if (apnType != NULL) {
                    OnSetDependencyMet(apnType, met);
                }
            }
            break;
        }
        case IDctConstants::CMD_SET_POLICY_DATA_ENABLE: {
            Boolean enabled = (msgArg1 == IDctConstants::ENABLED) ? TRUE : FALSE;
            OnSetPolicyDataEnabled(enabled);
            break;
        }
        case IDctConstants::CMD_SET_ENABLE_FAIL_FAST_MOBILE_DATA: {
            sEnableFailFastRefCounter += (msgArg1 == IDctConstants::ENABLED) ? 1 : -1;
            if (DBG) {
                Log("CMD_SET_ENABLE_FAIL_FAST_MOBILE_DATA: "
                        " sEnableFailFastRefCounter=%d", sEnableFailFastRefCounter);
            }
            if (sEnableFailFastRefCounter < 0) {
                String s;
                s.AppendFormat("CMD_SET_ENABLE_FAIL_FAST_MOBILE_DATA: "
                        "sEnableFailFastRefCounter:%d < 0", sEnableFailFastRefCounter);
                Loge(s);
                sEnableFailFastRefCounter = 0;
            }
            Boolean enabled = sEnableFailFastRefCounter > 0;
            if (DBG) {
                Log("CMD_SET_ENABLE_FAIL_FAST_MOBILE_DATA: enabled=%d"
                        " sEnableFailFastRefCounter=%d", enabled, sEnableFailFastRefCounter);
            }
            if (mFailFast != enabled) {
                mFailFast = enabled;
                mDataStallDetectionEnabled = !enabled;
                AutoPtr<IServiceStateTracker> serviceStateTracker;
                mPhone->GetServiceStateTracker((IServiceStateTracker**)&serviceStateTracker);
                Boolean isConcurrentVoiceAndDataAllowed;
                serviceStateTracker->IsConcurrentVoiceAndDataAllowed(&isConcurrentVoiceAndDataAllowed);
                DctConstantsState overallState;
                GetOverallState(&overallState);
                if (mDataStallDetectionEnabled&& (overallState == DctConstantsState_CONNECTED)
                        && (!mInVoiceCall || isConcurrentVoiceAndDataAllowed)) {
                    if (DBG) Log("CMD_SET_ENABLE_FAIL_FAST_MOBILE_DATA: start data stall");
                    StopDataStallAlarm();
                    StartDataStallAlarm(DATA_STALL_NOT_SUSPECTED);
                } else {
                    if (DBG) Log("CMD_SET_ENABLE_FAIL_FAST_MOBILE_DATA: stop data stall");
                    StopDataStallAlarm();
                }
            }
            break;
        }
        case IDctConstants::CMD_ENABLE_MOBILE_PROVISIONING: {
            AutoPtr<IBundle> bundle;
            msg->GetData((IBundle**)&bundle);
            if (bundle != NULL) {
                // try {
                ECode ec;
                do {
                    AutoPtr<IInterface> obj;
                    ec = bundle->Get(IDctConstants::PROVISIONING_URL_KEY, (IInterface**)&obj);
                    if (FAILED(ec)) break;
                    IObject::Probe(obj)->ToString(&mProvisioningUrl);
                } while(FALSE);
                // } catch (ClassCastException e) {
                if ((ECode) E_CLASS_CAST_EXCEPTION == ec) {
                    Loge("CMD_ENABLE_MOBILE_PROVISIONING: provisioning url not a string %d", ec);
                    mProvisioningUrl = NULL;
                }
                // }
            }
            if (TextUtils::IsEmpty(mProvisioningUrl)) {
                Loge("CMD_ENABLE_MOBILE_PROVISIONING: provisioning url is empty, ignoring");
                mIsProvisioning = FALSE;
                mProvisioningUrl = NULL;
            } else {
                Loge("CMD_ENABLE_MOBILE_PROVISIONING: provisioningUrl=%s", mProvisioningUrl.string());
                mIsProvisioning = TRUE;
                StartProvisioningApnAlarm();
            }
            break;
        }
        case IDctConstants::EVENT_PROVISIONING_APN_ALARM: {
            if (DBG) Log("EVENT_PROVISIONING_APN_ALARM");
            AutoPtr<IInterface> obj;
            mApnContexts->Get(StringUtils::ParseCharSequence(String("default")), (IInterface**)&obj);
            AutoPtr<IApnContext> apnCtx = IApnContext::Probe(obj);
            Boolean isConnectedOrConnecting;
            apnCtx->IsConnectedOrConnecting(&isConnectedOrConnecting);
            Boolean isProvisioningApn;
            apnCtx->IsProvisioningApn(&isProvisioningApn);
            if (isProvisioningApn && isConnectedOrConnecting) {
                if (mProvisioningApnAlarmTag == msgArg1) {
                    if (DBG) Log("EVENT_PROVISIONING_APN_ALARM: Disconnecting");
                    mIsProvisioning = FALSE;
                    mProvisioningUrl = NULL;
                    StopProvisioningApnAlarm();
                    SendCleanUpConnection(TRUE, apnCtx);
                } else {
                    if (DBG) {
                        Log("EVENT_PROVISIONING_APN_ALARM: ignore stale tag,"
                                " mProvisioningApnAlarmTag:%d != arg1:%d", mProvisioningApnAlarmTag, msgArg1);
                    }
                }
            } else {
                if (DBG) Log("EVENT_PROVISIONING_APN_ALARM: Not connected ignore");
            }
            break;
        }
        case IDctConstants::CMD_IS_PROVISIONING_APN: {
            if (DBG) Log("CMD_IS_PROVISIONING_APN");
            Boolean isProvApn;
            // try {
            ECode ec;
            do {
                String apnType(NULL);
                AutoPtr<IBundle> bundle;
                ec = msg->GetData((IBundle**)&bundle);
                if (FAILED(ec)) break;
                if (bundle != NULL) {
                    AutoPtr<IInterface> obj;
                    ec = bundle->Get(IDctConstants::APN_TYPE_KEY, (IInterface**)&obj);
                    if (FAILED(ec)) break;
                    IObject::Probe(obj)->ToString(&apnType);
                }
                if (TextUtils::IsEmpty(apnType)) {
                    Loge("CMD_IS_PROVISIONING_APN: apnType is empty");
                    isProvApn = FALSE;
                } else {
                    IsProvisioningApn(apnType, &isProvApn);
                }
            } while(FALSE);
            // } catch (ClassCastException e) {
                Loge("CMD_IS_PROVISIONING_APN: NO provisioning url ignoring");
                isProvApn = FALSE;
            // }
            if (DBG) Log("CMD_IS_PROVISIONING_APN: ret=%d", isProvApn);
            mReplyAc->ReplyToMessage(msg, IDctConstants::CMD_IS_PROVISIONING_APN,
                    isProvApn ? IDctConstants::ENABLED : IDctConstants::DISABLED);
            break;
        }
        case IDctConstants::EVENT_ICC_CHANGED: {
            Boolean b;
            OnUpdateIcc(&b);
            break;
        }
        case IDctConstants::EVENT_RESTART_RADIO: {
            RestartRadio();
            break;
        }
        case IDctConstants::CMD_NET_STAT_POLL: {
            if (msgArg1 == IDctConstants::ENABLED) {
                DctConstantsActivity dca;
                IInteger32::Probe(msgObj)->GetValue(&dca);
                HandleStartNetStatPoll(dca);
            } else if (msgArg1 == IDctConstants::DISABLED) {
                DctConstantsActivity dca;
                IInteger32::Probe(msgObj)->GetValue(&dca);
                HandleStartNetStatPoll(dca);
            }
            break;
        }
        default:
            Logger::E("DATA", "Unidentified event msg=%s", TO_CSTR(msg));
            break;
    }
    return NOERROR;
}

ECode DcTrackerBase::GetAnyDataEnabled(
    /* [out] */ Boolean* _result)
{
    VALIDATE_NOT_NULL(_result)

    Boolean result;
    {
        AutoLock lock(mDataEnabledLock);
        result = (mInternalDataEnabled && mUserDataEnabled && sPolicyDataEnabled
                && (mEnabledCount != 0));
    }
    if (!result && DBG) Log("getAnyDataEnabled %d", result);
    *_result = result;
    return NOERROR;
}

ECode DcTrackerBase::IsEmergency(
    /* [out] */ Boolean* _result)
{
    VALIDATE_NOT_NULL(_result)

    Boolean result;
    {
        AutoLock lock(mDataEnabledLock);
        Boolean isInEmergencyCall;
        mPhone->IsInEmergencyCall(&isInEmergencyCall);
        Boolean isInEcm;
        mPhone->IsInEcm(&isInEcm);
        result = isInEcm || isInEmergencyCall;
    }
    Log("isEmergency: result=%d", result);
    *_result = result;
    return NOERROR;
}

ECode DcTrackerBase::ApnTypeToId(
    /* [in] */ const String& type,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    if (TextUtils::Equals(type, IPhoneConstants::APN_TYPE_DEFAULT)) {
        *result = IDctConstants::APN_DEFAULT_ID;
        return NOERROR;
    } else if (TextUtils::Equals(type, IPhoneConstants::APN_TYPE_MMS)) {
        *result = IDctConstants::APN_MMS_ID;
        return NOERROR;
    } else if (TextUtils::Equals(type, IPhoneConstants::APN_TYPE_SUPL)) {
        *result = IDctConstants::APN_SUPL_ID;
        return NOERROR;
    } else if (TextUtils::Equals(type, IPhoneConstants::APN_TYPE_DUN)) {
        *result = IDctConstants::APN_DUN_ID;
        return NOERROR;
    } else if (TextUtils::Equals(type, IPhoneConstants::APN_TYPE_HIPRI)) {
        *result = IDctConstants::APN_HIPRI_ID;
        return NOERROR;
    } else if (TextUtils::Equals(type, IPhoneConstants::APN_TYPE_IMS)) {
        *result = IDctConstants::APN_IMS_ID;
        return NOERROR;
    } else if (TextUtils::Equals(type, IPhoneConstants::APN_TYPE_FOTA)) {
        *result = IDctConstants::APN_FOTA_ID;
        return NOERROR;
    } else if (TextUtils::Equals(type, IPhoneConstants::APN_TYPE_CBS)) {
        *result = IDctConstants::APN_CBS_ID;
        return NOERROR;
    } else if (TextUtils::Equals(type, IPhoneConstants::APN_TYPE_IA)) {
        *result = IDctConstants::APN_IA_ID;
        return NOERROR;
    } else if (TextUtils::Equals(type, IPhoneConstants::APN_TYPE_EMERGENCY)) {
        *result = IDctConstants::APN_EMERGENCY_ID;
        return NOERROR;
    } else {
        *result = IDctConstants::APN_INVALID_ID;
        return NOERROR;
    }
    return NOERROR;
}

ECode DcTrackerBase::ApnIdToType(
    /* [in] */ Int32 id,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    switch (id) {
    case IDctConstants::APN_DEFAULT_ID:
        *result = IPhoneConstants::APN_TYPE_DEFAULT;
        return NOERROR;
    case IDctConstants::APN_MMS_ID:
        *result = IPhoneConstants::APN_TYPE_MMS;
        return NOERROR;
    case IDctConstants::APN_SUPL_ID:
        *result = IPhoneConstants::APN_TYPE_SUPL;
        return NOERROR;
    case IDctConstants::APN_DUN_ID:
        *result = IPhoneConstants::APN_TYPE_DUN;
        return NOERROR;
    case IDctConstants::APN_HIPRI_ID:
        *result = IPhoneConstants::APN_TYPE_HIPRI;
        return NOERROR;
    case IDctConstants::APN_IMS_ID:
        *result = IPhoneConstants::APN_TYPE_IMS;
        return NOERROR;
    case IDctConstants::APN_FOTA_ID:
        *result = IPhoneConstants::APN_TYPE_FOTA;
        return NOERROR;
    case IDctConstants::APN_CBS_ID:
        *result = IPhoneConstants::APN_TYPE_CBS;
        return NOERROR;
    case IDctConstants::APN_IA_ID:
        *result = IPhoneConstants::APN_TYPE_IA;
        return NOERROR;
    case IDctConstants::APN_EMERGENCY_ID:
        *result = IPhoneConstants::APN_TYPE_EMERGENCY;
        return NOERROR;
    default:
        Log("Unknown id (%d) in apnIdToType", id);
        *result = IPhoneConstants::APN_TYPE_DEFAULT;
        return NOERROR;
    }
    return NOERROR;
}

ECode DcTrackerBase::GetLinkProperties(
    /* [in] */ const String& apnType,
    /* [out] */ ILinkProperties** result)
{
    VALIDATE_NOT_NULL(result)

    Int32 id;
    ApnTypeToId(apnType, &id);
    Boolean isApnIdEnabled;
    IsApnIdEnabled(id, &isApnIdEnabled);
    if (isApnIdEnabled) {
        AutoPtr<IInterface> obj;
        mDataConnectionAcHashMap->Get(0, (IInterface**)&obj);
        AutoPtr<IDcAsyncChannel> dcac = IDcAsyncChannel::Probe(obj);
        return dcac->GetLinkPropertiesSync(result);
    } else {
        return CLinkProperties::New(result);
    }
    return NOERROR;
}

ECode DcTrackerBase::GetNetworkCapabilities(
    /* [in] */ const String& apnType,
    /* [out] */ INetworkCapabilities** result)
{
    VALIDATE_NOT_NULL(result)

    Int32 id;
    ApnTypeToId(apnType, &id);
    Boolean isApnIdEnabled;
    IsApnIdEnabled(id, &isApnIdEnabled);
    if (isApnIdEnabled) {
        AutoPtr<IInterface> obj;
        mDataConnectionAcHashMap->Get(0, (IInterface**)&obj);
        AutoPtr<IDcAsyncChannel> dcac = IDcAsyncChannel::Probe(obj);
        return dcac->GetNetworkCapabilitiesSync(result);
    } else {
        return CNetworkCapabilities::New(result);
    }
    return NOERROR;
}

ECode DcTrackerBase::NotifyDataConnection(
    /* [in] */ const String& reason)
{
    for (Int32 id = 0; id < IDctConstants::APN_NUM_TYPES; id++) {
        if ((*mDataEnabled)[id]) {
            String apnType;
            ApnIdToType(id, &apnType);
            mPhone->NotifyDataConnection(reason, apnType);
        }
    }
    NotifyOffApnsOfAvailability(reason);
    return NOERROR;
}

ECode DcTrackerBase::NotifyApnIdUpToCurrent(
    /* [in] */ const String& reason,
    /* [in] */ Int32 apnId)
{
    String apnType;
    ApnIdToType(apnId, &apnType);
    switch (mState) {
        case DctConstantsState_IDLE:
            break;
        case DctConstantsState_RETRYING:
        case DctConstantsState_CONNECTING:
        case DctConstantsState_SCANNING:
            mPhone->NotifyDataConnection(reason, apnType,
                    PhoneConstantsDataState_CONNECTING);
            break;
        case DctConstantsState_CONNECTED:
        case DctConstantsState_DISCONNECTING:
            mPhone->NotifyDataConnection(reason, apnType,
                    PhoneConstantsDataState_CONNECTING);
            mPhone->NotifyDataConnection(reason, apnType,
                    PhoneConstantsDataState_CONNECTED);
            break;
        default:
            // Ignore
            break;
    }
    return NOERROR;
}

ECode DcTrackerBase::NotifyApnIdDisconnected(
    /* [in] */ const String& reason,
    /* [in] */ Int32 apnId)
{
    String apnType;
    ApnIdToType(apnId, &apnType);
    mPhone->NotifyDataConnection(reason, apnType,
            PhoneConstantsDataState_DISCONNECTED);
    return NOERROR;
}

ECode DcTrackerBase::NotifyOffApnsOfAvailability(
    /* [in] */ const String& reason)
{
    if (DBG) Log("notifyOffApnsOfAvailability - reason= %s", reason.string());
    for (Int32 id = 0; id < IDctConstants::APN_NUM_TYPES; id++) {
        Boolean isApnIdEnabled;
        IsApnIdEnabled(id, &isApnIdEnabled);
        if (!isApnIdEnabled) {
            NotifyApnIdDisconnected(reason, id);
        }
    }
    return NOERROR;
}

ECode DcTrackerBase::IsApnTypeEnabled(
    /* [in] */ const String& apnType,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (apnType == NULL) {
        *result = FALSE;
        return NOERROR;
    } else {
        Int32 apnId;
        ApnTypeToId(apnType, &apnId);
        Boolean isApnIdEnabled;
        IsApnIdEnabled(apnId, &isApnIdEnabled);
        *result = isApnIdEnabled;
        return NOERROR;
    }
    return NOERROR;
}

ECode DcTrackerBase::IsApnIdEnabled(
    /* [in] */ Int32 id,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (id != IDctConstants::APN_INVALID_ID) {
        *result = (*mDataEnabled)[id];
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode DcTrackerBase::SetEnabled(
    /* [in] */ Int32 id,
    /* [in] */ Boolean enable)
{
    if (DBG) {
        Log("setEnabled(%d, %d) with old state = %d and enabledCount = %d",
                id, enable, (*mDataEnabled)[id], mEnabledCount);
    }
    AutoPtr<IMessage> msg;
    ObtainMessage(IDctConstants::EVENT_ENABLE_NEW_APN, (IMessage**)&msg);
    msg->SetArg1(id);
    msg->SetArg2(enable ? IDctConstants::ENABLED : IDctConstants::DISABLED);
    Boolean b;
    SendMessage(msg, &b);
    return NOERROR;
}

ECode DcTrackerBase::OnEnableApn(
    /* [in] */ Int32 apnId,
    /* [in] */ Int32 enabled)
{
    if (DBG) {
        String apnType;
        ApnIdToType(apnId, &apnType);
        Boolean isApnTypeActive;
        IsApnTypeActive(apnType, &isApnTypeActive);
        Log("EVENT_APN_ENABLE_REQUEST apnId=%d, apnType=%s, enabled=%d, dataEnabled = %d, enabledCount = %d, isApnTypeActive = %d",
                apnId, apnType.string(), enabled, (*mDataEnabled)[apnId], mEnabledCount, isApnTypeActive);
    }
    if (enabled == IDctConstants::ENABLED) {
        {
            AutoLock lock(this);
            if (!(*mDataEnabled)[apnId]) {
                (*mDataEnabled)[apnId] = TRUE;
                mEnabledCount++;
            }
        }
        String type;
        ApnIdToType(apnId, &type);
        Boolean isApnTypeActive;
        IsApnTypeActive(type, &isApnTypeActive);
        if (!isApnTypeActive) {
            mRequestedApnType = type;
            OnEnableNewApn();
        } else {
            NotifyApnIdUpToCurrent(IPhone::REASON_APN_SWITCHED, apnId);
        }
    } else {
        // disable
        Boolean didDisable = FALSE;
        {
            AutoLock lock(this);
            if ((*mDataEnabled)[apnId]) {
                (*mDataEnabled)[apnId] = FALSE;
                mEnabledCount--;
                didDisable = TRUE;
            }
        }
        if (didDisable) {
            if ((mEnabledCount == 0) || (apnId == IDctConstants::APN_DUN_ID)) {
                mRequestedApnType = IPhoneConstants::APN_TYPE_DEFAULT;
                OnCleanUpConnection(TRUE, apnId, IPhone::REASON_DATA_DISABLED);
            }
            // send the disconnect msg manually, since the normal route wont send
            // it (it's not enabled)
            NotifyApnIdDisconnected(IPhone::REASON_DATA_DISABLED, apnId);
            Boolean isApnTypeActive;
            IsApnTypeActive(IPhoneConstants::APN_TYPE_DEFAULT, &isApnTypeActive);
            if ((*mDataEnabled)[IDctConstants::APN_DEFAULT_ID] == TRUE
                    && !isApnTypeActive) {
                // TODO - this is an ugly way to restore the default conn - should be done
                // by a real contention manager and policy that disconnects the lower pri
                // stuff as enable requests come in and pops them back on as we disable back
                // down to the lower pri stuff
                mRequestedApnType = IPhoneConstants::APN_TYPE_DEFAULT;
                OnEnableNewApn();
            }
        }
    }
    return NOERROR;
}

ECode DcTrackerBase::OnEnableNewApn()
{
    return NOERROR;
}

ECode DcTrackerBase::OnResetDone(
    /* [in] */ IAsyncResult* ar)
{
    if (DBG) Log("EVENT_RESET_DONE");
    String reason(NULL);
    if (ICharSequence::Probe(((AsyncResult*) ar)->mUserObj) != NULL) {
        IObject::Probe(((AsyncResult*) ar)->mUserObj)->ToString(&reason);
    }
    GotoIdleAndNotifyDataConnection(reason);
    return NOERROR;
}

ECode DcTrackerBase::SetInternalDataEnabled(
    /* [in] */ Boolean enable,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (DBG)
        Log("setInternalDataEnabled(%d)", enable);
    AutoPtr<IMessage> msg;
    ObtainMessage(IDctConstants::EVENT_SET_INTERNAL_DATA_ENABLE, (IMessage**)&msg);
    msg->SetArg1(enable ? IDctConstants::ENABLED : IDctConstants::DISABLED);
    Boolean b;
    SendMessage(msg, &b);
    *result = TRUE;
    return NOERROR;
}

ECode DcTrackerBase::OnSetInternalDataEnabled(
    /* [in] */ Boolean enabled)
{
    AutoLock lock(mDataEnabledLock);
    mInternalDataEnabled = enabled;
    if (enabled) {
        Log("onSetInternalDataEnabled: changed to enabled, try to setup data call");
        Boolean b;
        OnTrySetupData(IPhone::REASON_DATA_ENABLED, &b);
    } else {
        Log("onSetInternalDataEnabled: changed to disabled, cleanUpAllConnections");
        CleanUpAllConnections(String(NULL));
    }
    return NOERROR;
}

ECode DcTrackerBase::CleanUpAllConnections(
    /* [in] */ const String& cause)
{
    AutoPtr<IMessage> msg;
    ObtainMessage(IDctConstants::EVENT_CLEAN_UP_ALL_CONNECTIONS, (IMessage**)&msg);
    msg->SetObj(StringUtils::ParseCharSequence(cause));
    Boolean b;
    SendMessage(msg, &b);
    return NOERROR;
}

ECode DcTrackerBase::OnSetUserDataEnabled(
    /* [in] */ Boolean enabled)
{
    AutoLock lock(mDataEnabledLock);
    if (mUserDataEnabled != enabled) {
        mUserDataEnabled = enabled;
        AutoPtr<IContext> context;
        IPhone::Probe(mPhone)->GetContext((IContext**)&context);
        AutoPtr<IContentResolver> contentResolver;
        context->GetContentResolver((IContentResolver**)&contentResolver);
        Int32 phoneId;
        mPhone->GetPhoneId(&phoneId);
        Boolean b;
        Settings::Global::PutInt32(contentResolver, ISettingsGlobal::MOBILE_DATA + StringUtils::ToString(phoneId), enabled ? 1 : 0, &b);
        AutoPtr<IServiceState> serviceState;
        IPhone::Probe(mPhone)->GetServiceState((IServiceState**)&serviceState);
        Boolean roaming;
        serviceState->GetRoaming(&roaming);
        Boolean isDataOnRoamingEnabled;
        GetDataOnRoamingEnabled(&isDataOnRoamingEnabled);
        if (isDataOnRoamingEnabled == FALSE && roaming == TRUE) {
            if (enabled) {
                NotifyOffApnsOfAvailability(IPhone::REASON_ROAMING_ON);
            } else {
                NotifyOffApnsOfAvailability(IPhone::REASON_DATA_DISABLED);
            }
        }
        if (enabled) {
            Boolean b;
            OnTrySetupData(IPhone::REASON_DATA_ENABLED, &b);
        } else {
            OnCleanUpAllConnections(IPhone::REASON_DATA_SPECIFIC_DISABLED);
        }
    }
    return NOERROR;
}

ECode DcTrackerBase::OnSetDependencyMet(
    /* [in] */ const String& apnType,
    /* [in] */ Boolean met)
{
    return NOERROR;
}

ECode DcTrackerBase::OnSetPolicyDataEnabled(
    /* [in] */ Boolean enabled)
{
    AutoLock lock(mDataEnabledLock);
    if (sPolicyDataEnabled != enabled) {
        sPolicyDataEnabled = enabled;
        if (enabled) {
            Boolean b;
            OnTrySetupData(IPhone::REASON_DATA_ENABLED, &b);
        } else {
            OnCleanUpAllConnections(IPhone::REASON_DATA_SPECIFIC_DISABLED);
        }
    }
    return NOERROR;
}

ECode DcTrackerBase::GetReryConfig(
    /* [in] */ Boolean forDefault,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IServiceState> serviceState;
    IPhone::Probe(mPhone)->GetServiceState((IServiceState**)&serviceState);
    Int32 nt;
    serviceState->GetNetworkType(&nt);
    if ((nt == ITelephonyManager::NETWORK_TYPE_CDMA) ||
        (nt == ITelephonyManager::NETWORK_TYPE_1xRTT) ||
        (nt == ITelephonyManager::NETWORK_TYPE_EVDO_0) ||
        (nt == ITelephonyManager::NETWORK_TYPE_EVDO_A) ||
        (nt == ITelephonyManager::NETWORK_TYPE_EVDO_B) ||
        (nt == ITelephonyManager::NETWORK_TYPE_EHRPD)) {
        // CDMA variant
        return SystemProperties::Get(String("ro.cdma.data_retry_config"), result);
    } else {
        // Use GSM varient for all others.
        if (forDefault) {
            return SystemProperties::Get(String("ro.gsm.data_retry_config"), result);
        } else {
            return SystemProperties::Get(String("ro.gsm.2nd_data_retry_config"), result);
        }
    }
    return NOERROR;
}

ECode DcTrackerBase::ResetPollStats()
{
    mTxPkts = -1;
    mRxPkts = -1;
    mNetStatPollPeriod = POLL_NETSTAT_MILLIS;
    return NOERROR;
}

ECode DcTrackerBase::StartNetStatPoll()
{
    DctConstantsState overallState;
    GetOverallState(&overallState);
    if (overallState == DctConstantsState_CONNECTED
            && mNetStatPollEnabled == FALSE) {
        if (DBG) {
            Log("startNetStatPoll");
        }
        ResetPollStats();
        mNetStatPollEnabled = TRUE;
        mPollNetStat->Run();
    }
    return NOERROR;
}

ECode DcTrackerBase::StopNetStatPoll()
{
    mNetStatPollEnabled = FALSE;
    RemoveCallbacks(mPollNetStat);
    if (DBG) {
        Log("stopNetStatPoll");
    }
    return NOERROR;
}

ECode DcTrackerBase::SendStartNetStatPoll(
    /* [in] */ DctConstantsActivity activity)
{
    AutoPtr<IMessage> msg;
    ObtainMessage(IDctConstants::CMD_NET_STAT_POLL, (IMessage**)&msg);
    msg->SetArg1(IDctConstants::ENABLED);
    AutoPtr<IInteger32> i32;
    CInteger32::New(activity, (IInteger32**)&i32);
    msg->SetObj(i32);
    Boolean b;
    SendMessage(msg, &b);
    return NOERROR;
}

ECode DcTrackerBase::HandleStartNetStatPoll(
    /* [in] */ DctConstantsActivity activity)
{
    StartNetStatPoll();
    StartDataStallAlarm(DATA_STALL_NOT_SUSPECTED);
    SetActivity(activity);
    return NOERROR;
}

ECode DcTrackerBase::SendStopNetStatPoll(
    /* [in] */ DctConstantsActivity activity)
{
    AutoPtr<IMessage> msg;
    ObtainMessage(IDctConstants::CMD_NET_STAT_POLL, (IMessage**)&msg);
    msg->SetArg1(IDctConstants::DISABLED);
    AutoPtr<IInteger32> i32;
    CInteger32::New(activity, (IInteger32**)&i32);
    msg->SetObj(i32);
    Boolean b;
    SendMessage(msg, &b);
    return NOERROR;
}

ECode DcTrackerBase::HandleStopNetStatPoll(
    /* [in] */ DctConstantsActivity activity)
{
    StopNetStatPoll();
    StopDataStallAlarm();
    SetActivity(activity);
    return NOERROR;
}

ECode DcTrackerBase::UpdateDataActivity()
{
    Int64 sent, received;
    DctConstantsActivity newActivity;
    AutoPtr<TxRxSum> preTxRxSum = new TxRxSum();
    AutoPtr<TxRxSum> curTxRxSum = new TxRxSum();
    curTxRxSum->UpdateTxRxSum();
    curTxRxSum->GetTxPkts(&mTxPkts);
    curTxRxSum->GetRxPkts(&mRxPkts);
    if (VDBG) {
        Log("updateDataActivity: curTxRxSum=%s preTxRxSum=%s", TO_CSTR(curTxRxSum), TO_CSTR(preTxRxSum));
    }
    Int64 txPkts;
    preTxRxSum->GetTxPkts(&txPkts);
    Int64 rxPkts;
    preTxRxSum->GetRxPkts(&rxPkts);
    if (mNetStatPollEnabled && (txPkts > 0 || rxPkts > 0)) {
        sent = mTxPkts - txPkts;
        received = mRxPkts - rxPkts;
        if (VDBG)
            Log("updateDataActivity: sent=%lld received=%lld", sent, received);
        if (sent > 0 && received > 0) {
            newActivity = DctConstantsActivity_DATAINANDOUT;
        } else if (sent > 0 && received == 0) {
            newActivity = DctConstantsActivity_DATAOUT;
        } else if (sent == 0 && received > 0) {
            newActivity = DctConstantsActivity_DATAIN;
        } else {
            newActivity = (mActivity == DctConstantsActivity_DORMANT) ?
                    mActivity : DctConstantsActivity_NONE;
        }
        if (mActivity != newActivity && mIsScreenOn) {
            if (VDBG)
                Log("updateDataActivity: newActivity=%d", newActivity);
            mActivity = newActivity;
            IPhone::Probe(mPhone)->NotifyDataActivity();
        }
    }
    return NOERROR;
}

ECode DcTrackerBase::GetRecoveryAction(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IContext> context;
    IPhone::Probe(mPhone)->GetContext((IContext**)&context);
    AutoPtr<IContentResolver> contentResolver;
    context->GetContentResolver((IContentResolver**)&contentResolver);
    Int32 action;
    Settings::System::GetInt32(contentResolver,
            String("radio.data.stall.recovery.action"), RecoveryAction::GET_DATA_CALL_LIST, &action);
    if (VDBG_STALL) Log("getRecoveryAction: %d", action);
    *result = action;
    return NOERROR;
}

ECode DcTrackerBase::PutRecoveryAction(
    /* [in] */ Int32 action)
{
    AutoPtr<IContext> context;
    IPhone::Probe(mPhone)->GetContext((IContext**)&context);
    AutoPtr<IContentResolver> contentResolver;
    context->GetContentResolver((IContentResolver**)&contentResolver);
    Boolean b;
    Settings::System::PutInt32(contentResolver,
            String("radio.data.stall.recovery.action"), action, &b);
    if (VDBG_STALL) Log("putRecoveryAction: %d", action);
    return NOERROR;
}

ECode DcTrackerBase::IsConnected(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = FALSE;
    return NOERROR;
}

ECode DcTrackerBase::DoRecovery()
{
    DctConstantsState overallState;
    GetOverallState(&overallState);
    if (overallState == DctConstantsState_CONNECTED) {
        // Go through a series of recovery steps, each action transitions to the next action
        Int32 recoveryAction;
        GetRecoveryAction(&recoveryAction);
        switch (recoveryAction) {
        case RecoveryAction::GET_DATA_CALL_LIST: {
            Logger::D("DcTrackerBase", "TODO: EventLog");
            // EventLog::WriteEvent(IEventLogTags::DATA_STALL_RECOVERY_GET_DATA_CALL_LIST,
            //         mSentSinceLastRecv);
            if (DBG) Log("doRecovery() get data call list");
            AutoPtr<IMessage> msg;
            ObtainMessage(IDctConstants::EVENT_DATA_STATE_CHANGED, (IMessage**)&msg);
            ((PhoneBase*) mPhone.Get())->mCi->GetDataCallList(msg);
            PutRecoveryAction(RecoveryAction::CLEANUP);
            break;
        }
        case RecoveryAction::CLEANUP:
            Logger::D("DcTrackerBase", "TODO: EventLog");
            // EventLog::WriteEvent(IEventLogTags::DATA_STALL_RECOVERY_CLEANUP, mSentSinceLastRecv);
            if (DBG) Log("doRecovery() cleanup all connections");
            CleanUpAllConnections(IPhone::REASON_PDP_RESET);
            PutRecoveryAction(RecoveryAction::REREGISTER);
            break;
        case RecoveryAction::REREGISTER: {
            Logger::D("DcTrackerBase", "TODO: EventLog");
            // EventLog::WriteEvent(IEventLogTags::DATA_STALL_RECOVERY_REREGISTER,
            //         mSentSinceLastRecv);
            if (DBG) Log("doRecovery() re-register");
            AutoPtr<IServiceStateTracker> serviceStateTracker;
            mPhone->GetServiceStateTracker((IServiceStateTracker**)&serviceStateTracker);
            serviceStateTracker->ReRegisterNetwork(NULL);
            PutRecoveryAction(RecoveryAction::RADIO_RESTART);
            break;
        }
        case RecoveryAction::RADIO_RESTART:
            Logger::D("DcTrackerBase", "TODO: EventLog");
            // EventLog::WriteEvent(IEventLogTags::DATA_STALL_RECOVERY_RADIO_RESTART,
            //         mSentSinceLastRecv);
            if (DBG) Log("restarting radio");
            PutRecoveryAction(RecoveryAction::RADIO_RESTART_WITH_PROP);
            RestartRadio();
            break;
        case RecoveryAction::RADIO_RESTART_WITH_PROP:
            // This is in case radio restart has not recovered the data.
            // It will set an additional "gsm.radioreset" property to tell
            // RIL or system to take further action.
            // The implementation of hard reset recovery action is up to OEM product.
            // Once RADIO_RESET property is consumed, it is expected to set back
            // to false by RIL.
            Logger::D("DcTrackerBase", "TODO: EventLog");
            // EventLog::WriteEvent(IEventLogTags::DATA_STALL_RECOVERY_RADIO_RESTART_WITH_PROP, -1);
            if (DBG) Log("restarting radio with gsm.radioreset to true");
            SystemProperties::Set(mRADIO_RESET_PROPERTY, String("true"));
            // give 1 sec so property change can be notified.
            // try {
                Thread::Sleep(1000);
            // } catch (InterruptedException e) {}
            RestartRadio();
            PutRecoveryAction(RecoveryAction::GET_DATA_CALL_LIST);
            break;
        default:
            Logger::E("DcTrackerBase", "doRecovery: Invalid recoveryAction=%d", recoveryAction);
            return E_RUNTIME_EXCEPTION;
        }
        mSentSinceLastRecv = 0;
    }
    return NOERROR;
}

ECode DcTrackerBase::UpdateDataStallInfo()
{
    Int64 sent, received;
    AutoPtr<TxRxSum> preTxRxSum = new TxRxSum();
    mDataStallTxRxSum->UpdateTxRxSum();
    if (VDBG_STALL) {
        Log("updateDataStallInfo: mDataStallTxRxSum=%s preTxRxSum=%s",
                TO_CSTR(mDataStallTxRxSum), TO_CSTR(preTxRxSum));
    }
    Int64 dataStallTxRxSumTxPkts;
    mDataStallTxRxSum->GetTxPkts(&dataStallTxRxSumTxPkts);
    Int64 dataStallTxRxSumRxPkts;
    mDataStallTxRxSum->GetRxPkts(&dataStallTxRxSumRxPkts);
    Int64 preTxRxSumTxPkts;
    preTxRxSum->GetTxPkts(&preTxRxSumTxPkts);
    Int64 preTxRxSumRxPkts;
    preTxRxSum->GetRxPkts(&preTxRxSumRxPkts);
    sent = dataStallTxRxSumTxPkts - preTxRxSumTxPkts;
    received = dataStallTxRxSumRxPkts - preTxRxSumRxPkts;
    if (RADIO_TESTS) {
        Boolean b;
        SystemProperties::GetBoolean(String("radio.test.data.stall"), FALSE, &b);
        if (b) {
            Log("updateDataStallInfo: radio.test.data.stall true received = 0;");
            received = 0;
        }
    }
    if ( sent > 0 && received > 0 ) {
        if (VDBG_STALL) Log("updateDataStallInfo: IN/OUT");
        mSentSinceLastRecv = 0;
        PutRecoveryAction(RecoveryAction::GET_DATA_CALL_LIST);
    } else if (sent > 0 && received == 0) {
        Int32 state;
        IPhone::Probe(mPhone)->GetState(&state);
        if (state == PhoneConstantsState_IDLE) {
            mSentSinceLastRecv += sent;
        } else {
            mSentSinceLastRecv = 0;
        }
        if (DBG) {
            Log("updateDataStallInfo: OUT sent=%d mSentSinceLastRecv=%lld",
                    sent, mSentSinceLastRecv);
        }
    } else if (sent == 0 && received > 0) {
        if (VDBG_STALL) Log("updateDataStallInfo: IN");
        mSentSinceLastRecv = 0;
        PutRecoveryAction(RecoveryAction::GET_DATA_CALL_LIST);
    } else {
        if (VDBG_STALL) Log("updateDataStallInfo: NONE");
    }
    return NOERROR;
}

ECode DcTrackerBase::OnDataStallAlarm(
    /* [in] */ Int32 tag)
{
    if (mDataStallAlarmTag != tag) {
        if (DBG) {
            Log("onDataStallAlarm: ignore, tag=%d expecting %d", tag, mDataStallAlarmTag);
        }
        return NOERROR;
    }
    UpdateDataStallInfo();
    Int32 hangWatchdogTrigger;
    Settings::Global::GetInt32(mResolver,
            ISettingsGlobal::PDP_WATCHDOG_TRIGGER_PACKET_COUNT,
            NUMBER_SENT_PACKETS_OF_HANG, &hangWatchdogTrigger);
    Boolean suspectedStall = DATA_STALL_NOT_SUSPECTED;
    if (mSentSinceLastRecv >= hangWatchdogTrigger) {
        if (DBG) {
            Int32 recoveryAction;
            GetRecoveryAction(&recoveryAction);
            Log("onDataStallAlarm: tag=%d do recovery action=%s", tag, recoveryAction);
        }
        suspectedStall = DATA_STALL_SUSPECTED;
        AutoPtr<IMessage> msg;
        ObtainMessage(IDctConstants::EVENT_DO_RECOVERY, (IMessage**)&msg);
        Boolean b;
        SendMessage(msg, &b);
    } else {
        if (VDBG_STALL) {
            Log("onDataStallAlarm: tag=%d Sent %lld pkts since last received, < watchdogTrigger=%d",
                    tag, mSentSinceLastRecv, hangWatchdogTrigger);
        }
    }
    StartDataStallAlarm(suspectedStall);
    return NOERROR;
}

ECode DcTrackerBase::StartDataStallAlarm(
    /* [in] */ Boolean suspectedStall)
{
    Int32 nextAction;
    GetRecoveryAction(&nextAction);
    Int32 delayInMs;
    DctConstantsState overallState;
    GetOverallState(&overallState);
    if (mDataStallDetectionEnabled && overallState == DctConstantsState_CONNECTED) {
        // If screen is on or data stall is currently suspected, set the alarm
        // with an aggresive timeout.
        Boolean isAggressiveRecovery;
        RecoveryAction::IsAggressiveRecovery(nextAction, &isAggressiveRecovery);
        if (mIsScreenOn || suspectedStall || isAggressiveRecovery) {
            Settings::Global::GetInt32(mResolver,
                    ISettingsGlobal::DATA_STALL_ALARM_AGGRESSIVE_DELAY_IN_MS,
                    DATA_STALL_ALARM_AGGRESSIVE_DELAY_IN_MS_DEFAULT, &delayInMs);
        } else {
            Settings::Global::GetInt32(mResolver,
                    ISettingsGlobal::DATA_STALL_ALARM_NON_AGGRESSIVE_DELAY_IN_MS,
                    DATA_STALL_ALARM_NON_AGGRESSIVE_DELAY_IN_MS_DEFAULT, &delayInMs);
        }
        mDataStallAlarmTag += 1;
        if (VDBG_STALL) {
            Log("startDataStallAlarm: tag=%lld delay=%ds",
                    mDataStallAlarmTag, (delayInMs / 1000));
        }
        AutoPtr<IIntent> intent;
        CIntent::New((IIntent**)&intent);
        intent->PutExtra(DATA_STALL_ALARM_TAG_EXTRA, mDataStallAlarmTag);
        AutoPtr<IContext> context;
        IPhone::Probe(mPhone)->GetContext((IContext**)&context);
        CPendingIntent::GetBroadcast(context, 0, intent,
                IPendingIntent::FLAG_UPDATE_CURRENT, (IPendingIntent**)&mDataStallAlarmIntent);
        mAlarmManager->Set(IAlarmManager::ELAPSED_REALTIME_WAKEUP,
                SystemClock::GetElapsedRealtime() + delayInMs, mDataStallAlarmIntent);
    } else {
        if (VDBG_STALL) {
            Log("startDataStallAlarm: NOT started, no connection tag=%lld", mDataStallAlarmTag);
        }
    }
    return NOERROR;
}

ECode DcTrackerBase::StopDataStallAlarm()
{
    if (VDBG_STALL) {
        Log("stopDataStallAlarm: current tag=%lld mDataStallAlarmIntent=%s",
                mDataStallAlarmTag, TO_CSTR(mDataStallAlarmIntent));
    }
    mDataStallAlarmTag += 1;
    if (mDataStallAlarmIntent != NULL) {
        mAlarmManager->Cancel(mDataStallAlarmIntent);
        mDataStallAlarmIntent = NULL;
    }
    return NOERROR;
}

ECode DcTrackerBase::RestartDataStallAlarm()
{
    Boolean isConnected;
    IsConnected(&isConnected);
    if (isConnected == FALSE) return NOERROR;
    // To be called on screen status change.
    // Do not cancel the alarm if it is set with aggressive timeout.
    Int32 nextAction;
    GetRecoveryAction(&nextAction);
    Boolean isAggressiveRecovery;
    RecoveryAction::IsAggressiveRecovery(nextAction, &isAggressiveRecovery);
    if (isAggressiveRecovery) {
        if (DBG) Log("restartDataStallAlarm: action is pending. not resetting the alarm.");
        return NOERROR;
    }
    if (VDBG_STALL) Log("restartDataStallAlarm: stop then start.");
    StopDataStallAlarm();
    StartDataStallAlarm(DATA_STALL_NOT_SUSPECTED);
    return NOERROR;
}

ECode DcTrackerBase::SetInitialAttachApn()
{
    AutoPtr<IApnSetting> iaApnSetting;
    AutoPtr<IApnSetting> defaultApnSetting;
    AutoPtr<IApnSetting> firstApnSetting;
    Log("setInitialApn: E mPreferredApn=%s", TO_CSTR(mPreferredApn));
    Boolean isEmpty;
    mAllApnSettings->IsEmpty(&isEmpty);
    if (mAllApnSettings != NULL && !isEmpty) {
        AutoPtr<IInterface> obj;
        mAllApnSettings->Get(0, (IInterface**)&obj);
        firstApnSetting = IApnSetting::Probe(obj);
        Log("setInitialApn: firstApnSetting=%s", TO_CSTR(firstApnSetting));
        // Search for Initial APN setting and the first apn that can handle default
        AutoPtr<IIterator> it;
        mAllApnSettings->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<IApnSetting> apn = IApnSetting::Probe(obj);
            // Can't use apn.CanHandleType(), as that returns true for APNs that have no type.
            AutoPtr<ArrayOf<String> > types;
            apn->GetTypes((ArrayOf<String>**)&types);
            Boolean canHandleType;
            apn->CanHandleType(IPhoneConstants::APN_TYPE_DEFAULT, &canHandleType);
            Boolean isCarrierEnabled;
            apn->GetCarrierEnabled(&isCarrierEnabled);
            if (ArrayUtils::Contains(types.Get(), IPhoneConstants::APN_TYPE_IA) && isCarrierEnabled) {
                // The Initial Attach APN is highest priority so use it if there is one
                Log("setInitialApn: iaApnSetting=%s", TO_CSTR(apn));
                iaApnSetting = apn;
                break;
            } else if ((defaultApnSetting == NULL)
                    && (canHandleType)) {
                // Use the first default apn if no better choice
                Log("setInitialApn: defaultApnSetting=%s", TO_CSTR(apn));
                defaultApnSetting = apn;
            }
        }
    }
    // The priority of apn candidates from highest to lowest is:
    //   1) APN_TYPE_IA (Inital Attach)
    //   2) mPreferredApn, i.e. the current preferred apn
    //   3) The first apn that than handle APN_TYPE_DEFAULT
    //   4) The first APN we can find.
    AutoPtr<IApnSetting> initialAttachApnSetting;
    if (iaApnSetting != NULL) {
        if (DBG) Log("setInitialAttachApn: using iaApnSetting");
        initialAttachApnSetting = iaApnSetting;
    } else if (mPreferredApn != NULL) {
        if (DBG) Log("setInitialAttachApn: using mPreferredApn");
        initialAttachApnSetting = mPreferredApn;
    } else if (defaultApnSetting != NULL) {
        if (DBG) Log("setInitialAttachApn: using defaultApnSetting");
        initialAttachApnSetting = defaultApnSetting;
    } else if (firstApnSetting != NULL) {
        if (DBG) Log("setInitialAttachApn: using firstApnSetting");
        initialAttachApnSetting = firstApnSetting;
    }
    if (initialAttachApnSetting == NULL) {
        if (DBG) Log("setInitialAttachApn: X There in no available apn");
    } else {
        if (DBG) Log("setInitialAttachApn: X selected Apn=%s", TO_CSTR(initialAttachApnSetting));
        String apn;
        initialAttachApnSetting->GetApn(&apn);
        String initialAttachApnSettingProtocol;
        initialAttachApnSetting->GetProtocol(&initialAttachApnSettingProtocol);
        Int32 initialAttachApnSettingAuthType;
        initialAttachApnSetting->GetAuthType(&initialAttachApnSettingAuthType);
        String user;
        initialAttachApnSetting->GetUser(&user);
        String password;
        initialAttachApnSetting->GetPassword(&password);
        ((PhoneBase*) mPhone.Get())->mCi->SetInitialAttachApn(apn,
                initialAttachApnSettingProtocol, initialAttachApnSettingAuthType,
                user, password, NULL);
    }
    return NOERROR;
}

ECode DcTrackerBase::SetDataProfilesAsNeeded()
{
    if (DBG) Log("setDataProfilesAsNeeded");
    Boolean isEmpty;
    mAllApnSettings->IsEmpty(&isEmpty);
    if (mAllApnSettings != NULL && !isEmpty) {
        AutoPtr<IArrayList> dps;
        CArrayList::New((IArrayList**)&dps);
        AutoPtr<IIterator> it;
        mAllApnSettings->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<IApnSetting> apn = IApnSetting::Probe(obj);
            Boolean isModemCognitive;
            apn->GetModemCognitive(&isModemCognitive);
            if (isModemCognitive) {
                AutoPtr<IServiceState> serviceState;
                IPhone::Probe(mPhone)->GetServiceState((IServiceState**)&serviceState);
                Boolean roaming;
                serviceState->GetRoaming(&roaming);
                AutoPtr<IDataProfile> dp;
                CDataProfile::New(apn, roaming, (IDataProfile**)&dp);
                Boolean isDup = FALSE;
                AutoPtr<IIterator> it;
                dps->GetIterator((IIterator**)&it);
                Boolean hasNext;
                while (it->HasNext(&hasNext), hasNext) {
                    AutoPtr<IInterface> obj;
                    it->GetNext((IInterface**)&obj);
                    AutoPtr<IDataProfile> dpIn = IDataProfile::Probe(obj);
                    Boolean isEquals;
                    IObject::Probe(dp)->Equals(dpIn, &isEquals);
                    if (isEquals) {
                        isDup = TRUE;
                        break;
                    }
                }
                if (!isDup) {
                    dps->Add(dp);
                }
            }
        }
        Int32 size;
        dps->GetSize(&size);
        if (size > 0) {
            AutoPtr<ArrayOf<IInterface*> > array;
            dps->ToArray(ArrayOf<IInterface*>::Alloc(0), (ArrayOf<IInterface*>**)&array);
            assert(0 && "TODO: ICommandsInterface");
            // ((PhoneBase*) mPhone.Get())->mCi->SetDataProfile(array, NULL);
        }
    }
    return NOERROR;
}

ECode DcTrackerBase::OnActionIntentProvisioningApnAlarm(
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (DBG) Log("onActionIntentProvisioningApnAlarm: action=%s", action.string());
    AutoPtr<IMessage> msg;
    ObtainMessage(IDctConstants::EVENT_PROVISIONING_APN_ALARM,
            StringUtils::ParseCharSequence(action), (IMessage**)&msg);
    Int32 value;
    intent->GetInt32Extra(PROVISIONING_APN_ALARM_TAG_EXTRA, 0, &value);
    msg->SetArg1(value);
    Boolean b;
    SendMessage(msg, &b);
    return NOERROR;
}

ECode DcTrackerBase::StartProvisioningApnAlarm()
{
    Int32 delayInMs;
    Settings::Global::GetInt32(mResolver,
            ISettingsGlobal::PROVISIONING_APN_ALARM_DELAY_IN_MS,
            PROVISIONING_APN_ALARM_DELAY_IN_MS_DEFAULT, &delayInMs);
    if (Build::IS_DEBUGGABLE) {
        // Allow debug code to use a system property to provide another value
        String delayInMsStrg = StringUtils::ToString(delayInMs);
        AutoPtr<ISystem> systemHelper;
        CSystem::AcquireSingleton((ISystem**)&systemHelper);
        systemHelper->GetProperty(DEBUG_PROV_APN_ALARM, delayInMsStrg, &delayInMsStrg);
        // try {
        ECode ec = StringUtils::Parse(delayInMsStrg, &delayInMs);
        // } catch (NumberFormatException e) {
        if ((ECode) E_NUMBER_FORMAT_EXCEPTION == ec) {
            Loge("startProvisioningApnAlarm: e=%d", ec);
        }
        // }
    }
    mProvisioningApnAlarmTag += 1;
    if (DBG) {
        Log("startProvisioningApnAlarm: tag=%d delay=%d",
                mProvisioningApnAlarmTag, (delayInMs / 1000));
    }
    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    intent->PutExtra(PROVISIONING_APN_ALARM_TAG_EXTRA, mProvisioningApnAlarmTag);
    AutoPtr<IContext> context;
    IPhone::Probe(mPhone)->GetContext((IContext**)&context);
    CPendingIntent::GetBroadcast(context, 0, intent,
            IPendingIntent::FLAG_UPDATE_CURRENT, (IPendingIntent**)&mProvisioningApnAlarmIntent);
    mAlarmManager->Set(IAlarmManager::ELAPSED_REALTIME_WAKEUP,
            SystemClock::GetElapsedRealtime() + delayInMs, mProvisioningApnAlarmIntent);
    return NOERROR;
}

ECode DcTrackerBase::StopProvisioningApnAlarm()
{
    if (DBG) {
        Log("stopProvisioningApnAlarm: current tag=%d mProvsioningApnAlarmIntent=%s",
                mProvisioningApnAlarmTag, TO_CSTR(mProvisioningApnAlarmIntent));
    }
    mProvisioningApnAlarmTag += 1;
    if (mProvisioningApnAlarmIntent != NULL) {
        mAlarmManager->Cancel(mProvisioningApnAlarmIntent);
        mProvisioningApnAlarmIntent = NULL;
    }
    return NOERROR;
}

ECode DcTrackerBase::SendCleanUpConnection(
    /* [in] */ Boolean tearDown,
    /* [in] */ IApnContext* apnContext)
{
    if (DBG) Log("sendCleanUpConnection: tearDown=%d apnContext=%s", tearDown, TO_CSTR(apnContext));
    AutoPtr<IMessage> msg;
    ObtainMessage(IDctConstants::EVENT_CLEAN_UP_CONNECTION, (IMessage**)&msg);
    msg->SetArg1(tearDown ? 1 : 0);
    msg->SetArg2(0);
    msg->SetObj(apnContext);
    Boolean b;
    SendMessage(msg, &b);
    return NOERROR;
}

ECode DcTrackerBase::SendRestartRadio()
{
    if (DBG) Log("sendRestartRadio:");
    AutoPtr<IMessage> msg;
    ObtainMessage(IDctConstants::EVENT_RESTART_RADIO, (IMessage**)&msg);
    Boolean b;
    SendMessage(msg, &b);
    return NOERROR;
}

ECode DcTrackerBase::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    pw->Println(String("DataConnectionTrackerBase:"));
    pw->Println(String(" RADIO_TESTS=") + StringUtils::ToString(RADIO_TESTS));
    pw->Println(String(" mInternalDataEnabled=") + StringUtils::ToString(mInternalDataEnabled));
    pw->Println(String(" mUserDataEnabled=") + StringUtils::ToString(mUserDataEnabled));
    pw->Println(String(" sPolicyDataEnabed=") + StringUtils::ToString(sPolicyDataEnabled));
    pw->Println(String(" mDataEnabled:"));
    for (Int32 i=0; i < mDataEnabled->GetLength(); i++) {
        String s;
        s.AppendFormat("  mDataEnabled[%d]=%d\n", i, (*mDataEnabled)[i]);
        pw->Print(s);
    }
    IFlushable::Probe(pw)->Flush();
    pw->Println(String(" mEnabledCount=") + StringUtils::ToString(mEnabledCount));
    pw->Println(String(" mRequestedApnType=") + mRequestedApnType);
    String phoneName;
    IPhone::Probe(mPhone)->GetPhoneName(&phoneName);
    pw->Println(String(" mPhone=") + phoneName);
    Int32 phoneId;
    mPhone->GetPhoneId(&phoneId);
    pw->Println(String(" mPhoneId=") + StringUtils::ToString(phoneId));
    pw->Println(String(" mActivity=") + StringUtils::ToString(mActivity));
    pw->Println(String(" mState=") + StringUtils::ToString(mState));
    pw->Println(String(" mTxPkts=") + StringUtils::ToString(mTxPkts));
    pw->Println(String(" mRxPkts=") + StringUtils::ToString(mRxPkts));
    pw->Println(String(" mNetStatPollPeriod=") + StringUtils::ToString(mNetStatPollPeriod));
    pw->Println(String(" mNetStatPollEnabled=") + StringUtils::ToString(mNetStatPollEnabled));
    pw->Println(String(" mDataStallTxRxSum=") + TO_STR(mDataStallTxRxSum));
    pw->Println(String(" mDataStallAlarmTag=") + StringUtils::ToString(mDataStallAlarmTag));
    pw->Println(String(" mDataStallDetectionEanbled=") + StringUtils::ToString(mDataStallDetectionEnabled));
    pw->Println(String(" mSentSinceLastRecv=") + StringUtils::ToString(mSentSinceLastRecv));
    pw->Println(String(" mNoRecvPollCount=") + StringUtils::ToString(mNoRecvPollCount));
    pw->Println(String(" mResolver=") + TO_STR(mResolver));
    pw->Println(String(" mIsWifiConnected=") + StringUtils::ToString(mIsWifiConnected));
    pw->Println(String(" mReconnectIntent=") + TO_STR(mReconnectIntent));
    pw->Println(String(" mCidActive=") + StringUtils::ToString(mCidActive));
    pw->Println(String(" mAutoAttachOnCreation=") + StringUtils::ToString(mAutoAttachOnCreation));
    pw->Println(String(" mIsScreenOn=") + StringUtils::ToString(mIsScreenOn));
    pw->Println(String(" mUniqueIdGenerator=") + TO_STR(mUniqueIdGenerator));
    IFlushable::Probe(pw)->Flush();
    pw->Println(String(" ***************************************"));
    AutoPtr<IDcController> dcc = mDcc;
    if (dcc != NULL) {
        ((DcController*) dcc.Get())->Dump(fd, pw, args);
    } else {
        pw->Println(String(" mDcc=null"));
    }
    pw->Println(String(" ***************************************"));
    AutoPtr<IHashMap> dcs = mDataConnections;
    if (dcs != NULL) {
        AutoPtr<ISet> mDcSet;
        mDataConnections->GetEntrySet((ISet**)&mDcSet);
        Int32 size;
        mDcSet->GetSize(&size);
        pw->Println(String(" mDataConnections: count=") + StringUtils::ToString(size));
        AutoPtr<IIterator> it;
        mDcSet->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<IMapEntry> entry = IMapEntry::Probe(obj);
            obj = NULL;
            entry->GetKey((IInterface**)&obj);
            String s;
            s.AppendFormat(" *** mDataConnection[%s] \n", TO_CSTR(obj));
            pw->Print(s);
            obj = NULL;
            entry->GetValue((IInterface**)&obj);
            IStateMachine::Probe(obj)->Dump(fd, pw, args);
        }
    } else {
        pw->Println(String("mDataConnections=null"));
    }
    pw->Println(String(" ***************************************"));
    IFlushable::Probe(pw)->Flush();
    AutoPtr<IHashMap> apnToDcId = mApnToDataConnectionId;
    if (apnToDcId != NULL) {
        AutoPtr<ISet> apnToDcIdSet;
        apnToDcId->GetEntrySet((ISet**)&apnToDcIdSet);
        Int32 size;
        apnToDcIdSet->GetSize(&size);
        pw->Println(String(" mApnToDataConnectonId size=") + StringUtils::ToString(size));
        AutoPtr<IIterator> it;
        apnToDcIdSet->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<IMapEntry> entry = IMapEntry::Probe(obj);
            obj = NULL;
            entry->GetKey((IInterface**)&obj);
            String s;
            s.AppendFormat(" mApnToDataConnectonId[%s]=", TO_CSTR(obj));
            obj = NULL;
            entry->GetValue((IInterface**)&obj);
            s.AppendFormat("%s\n", TO_CSTR(obj));
            pw->Printf(s, NULL);
        }
    } else {
        pw->Println(String("mApnToDataConnectionId=null"));
    }
    pw->Println(String(" ***************************************"));
    IFlushable::Probe(pw)->Flush();
    AutoPtr<IConcurrentHashMap> apnCtxs = mApnContexts;
    if (apnCtxs != NULL) {
        AutoPtr<ISet> apnCtxsSet;
        apnCtxs->GetEntrySet((ISet**)&apnCtxsSet);
        Int32 size;
        apnCtxsSet->GetSize(&size);
        pw->Println(String(" mApnContexts size=") + StringUtils::ToString(size));
        AutoPtr<IIterator> it;
        apnCtxsSet->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<IMapEntry> entry = IMapEntry::Probe(obj);
            obj = NULL;
            entry->GetValue((IInterface**)&obj);
            ((ApnContext*) IApnContext::Probe(obj))->Dump(fd, pw, args);
        }
        pw->Println(String(" ***************************************"));
    } else {
        pw->Println(String(" mApnContexts=null"));
    }
    IFlushable::Probe(pw)->Flush();
    pw->Println(String(" mActiveApn=") + TO_STR(mActiveApn));
    AutoPtr<IArrayList> apnSettings = mAllApnSettings;
    if (apnSettings != NULL) {
        Int32 size;
        apnSettings->GetSize(&size);
        pw->Println(String(" mAllApnSettings size=") + StringUtils::ToString(size));
        for (Int32 i=0; i < size; i++) {
            AutoPtr<IInterface> obj;
            apnSettings->Get(i, (IInterface**)&obj);
            String s;
            s.AppendFormat(" mAllApnSettings[%d]: %s\n", i, TO_CSTR(obj));
            pw->Printf(s, NULL);
        }
        IFlushable::Probe(pw)->Flush();
    } else {
        pw->Println(String(" mAllApnSettings=null"));
    }
    pw->Println(String(" mPreferredApn=") + TO_STR(mPreferredApn));
    pw->Println(String(" mIsPsRestricted=") + StringUtils::ToString(mIsPsRestricted));
    pw->Println(String(" mIsDisposed=") + StringUtils::ToString(mIsDisposed));
    pw->Println(String(" mIntentReceiver=") + TO_STR(mIntentReceiver));
    pw->Println(String(" mDataRoamingSettingObserver=") + TO_STR(mDataRoamingSettingObserver));
    IFlushable::Probe(pw)->Flush();
    return NOERROR;
}

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
