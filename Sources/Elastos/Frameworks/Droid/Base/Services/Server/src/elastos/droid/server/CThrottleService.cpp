#include "CThrottleService.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/Manifest.h"
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::ICharSequence;
using Elastos::Core::ICloneable;
using Elastos::Droid::App::IPendingIntentHelper;
using Elastos::Droid::App::CPendingIntentHelper;
using Elastos::Droid::App::INotification;
using Elastos::Droid::App::CNotification;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::Res::IResourcesHelper;
using Elastos::Droid::Content::Res::CResourcesHelper;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Net::INetworkStats;
using Elastos::Droid::Net::INetworkStatsEntry;
using Elastos::Droid::Net::IThrottleManager;
using Elastos::Droid::Net::EIID_INetworkManagementEventObserver;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Os::CEnvironment;
using Elastos::Droid::Os::EIID_IHandler;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Utility::INtpTrustedTime;
using Elastos::Droid::Utility::INtpTrustedTimeHelper;
using Elastos::Droid::Utility::CNtpTrustedTimeHelper;
using Elastos::IO::CFile;
using Elastos::IO::IBufferedWriter;
using Elastos::IO::IFileWriter;
using Elastos::IO::CFileWriter;
using Elastos::IO::IBufferedWriter;
using Elastos::IO::CBufferedWriter;
using Elastos::IO::ICloseable;
using Elastos::IO::IFileInputStream;
using Elastos::IO::CFileInputStream;
using Elastos::Core::IRandom;
using Elastos::Utility::CRandom;
using Elastos::Utility::ICalendarHelper;
using Elastos::Utility::CCalendarHelper;
using Elastos::Utility::IGregorianCalendar;
using Elastos::Utility::CGregorianCalendar;
using Elastos::Utility::Logging::Slogger;
using Elastos::Utility::Concurrent::Atomic::CAtomicInteger32;
using Elastos::Utility::Concurrent::Atomic::CAtomicInteger64;

namespace Elastos {
namespace Droid {
namespace Server {

const String CThrottleService::TESTING_ENABLED_PROPERTY("persist.throttle.testing");
const String CThrottleService::TAG("ThrottleService");
const Boolean CThrottleService::DBG = TRUE;
const Boolean CThrottleService::VDBG = FALSE;
const Int32 CThrottleService::INITIAL_POLL_DELAY_SEC;
const Int32 CThrottleService::TESTING_POLLING_PERIOD_SEC;
const Int32 CThrottleService::TESTING_RESET_PERIOD_SEC;
const Int64 CThrottleService::TESTING_THRESHOLD = 1 * 1024 * 1024;
const Int64 CThrottleService::MAX_NTP_CACHE_AGE = 24 * 60 * 60 * 1000;
const String CThrottleService::ACTION_POLL("com.android.server.ThrottleManager.action.POLL");
Int32 CThrottleService::POLL_REQUEST = 0;
const String CThrottleService::ACTION_RESET("com.android.server.ThorottleManager.action.RESET");
Int32 CThrottleService::RESET_REQUEST = 1;
const Int32 CThrottleService::NOTIFICATION_WARNING;
const Int32 CThrottleService::THROTTLE_INDEX_UNINITIALIZED;
const Int32 CThrottleService::THROTTLE_INDEX_UNTHROTTLED;
const Int32 CThrottleService::EVENT_REBOOT_RECOVERY;
const Int32 CThrottleService::EVENT_POLICY_CHANGED;
const Int32 CThrottleService::EVENT_POLL_ALARM;
const Int32 CThrottleService::EVENT_RESET_ALARM;
const Int32 CThrottleService::EVENT_IFACE_UP;

const Int32 CThrottleService::DataRecorder::MAX_SIMS_SUPPORTED;
const Int32 CThrottleService::DataRecorder::DATA_FILE_VERSION;

CThrottleService::CThrottleService()
{
    mMaxNtpCacheAge = MAX_NTP_CACHE_AGE;
    mPolicyPollPeriodSec = 0;
    mPolicyResetDay = 0;
    mPolicyNotificationsAllowedMask = 0;

    mLastRead = 0L;
    mLastWrite = 0L;

    mWarningNotificationSent = FALSE;
}

AutoPtr<IINetworkManagementService> CThrottleService::GetNetworkManagementService()
{
    AutoPtr<IServiceManager> sm;
    CServiceManager::AcquireSingleton((IServiceManager**)&sm);
    AutoPtr<IBinder> b;
    sm->GetService(IContext::NETWORKMANAGEMENT_SERVICE, (IInterface**)(IBinder**)&b);
    return IINetworkManagementService::Probe(b.Get());
}

ECode CThrottleService::constructor(
        /* [in] */ IContext* context)
{
    AutoPtr<INtpTrustedTimeHelper> helper;
    CNtpTrustedTimeHelper::AcquireSingleton((INtpTrustedTimeHelper**)&helper);
    AutoPtr<INtpTrustedTime> instance;
    helper->GetInstance(context, (INtpTrustedTime**)&instance);
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    String iface;
    res->GetString(/*R.string.config_datause_iface*/0x0104001e,&iface);
    constructor(context, GetNetworkManagementService(), (ITrustedTime*)instance, iface);
    return NOERROR;
}

ECode CThrottleService::constructor(
    /* [in] */  IContext* context,
    /* [in] */  IINetworkManagementService* nmService,
    /* [in] */  ITrustedTime* time,
    /* [in] */  const String& iface)
{
    if (VDBG) {
        Slogger::V(TAG, "Starting ThrottleService");
    }
    mContext = context;

    CAtomicInteger64::New((IAtomicInteger64**)&mPolicyThreshold);
    CAtomicInteger32::New((IAtomicInteger32**)&mPolicyThrottleValue);
    CAtomicInteger32::New((IAtomicInteger32**)&mThrottleIndex);

    mIface = iface;
    mContext->GetSystemService(IContext::ALARM_SERVICE, (IInterface**)&mAlarmManager);
    AutoPtr<IIntent> pollIntent;
    CIntent::New(ACTION_POLL, NULL, (IIntent**)&pollIntent);
    AutoPtr<IPendingIntentHelper> pih;
    CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&pih);
    pih->GetBroadcast(mContext, POLL_REQUEST, pollIntent, 0, (IPendingIntent**)&mPendingPollIntent);
    AutoPtr<IIntent> resetIntent;
    CIntent::New(ACTION_RESET, NULL, (IIntent**)&resetIntent);
    pih->GetBroadcast(mContext, RESET_REQUEST, resetIntent, 0, (IPendingIntent**)&mPendingResetIntent);

    mNMService = nmService;
    mTime = time;

    mContext->GetSystemService(IContext::NOTIFICATION_SERVICE, (IInterface**)&mNotificationManager);
    return NOERROR;
}

//====================================================================
// CThrottleService::InterfaceObserver
//====================================================================
CThrottleService::InterfaceObserver::InterfaceObserver(
    /* [in] */ IHandler* handler,
    /* [in] */ Int32 msg,
    /* [in] */ const String& iface)
{
    //super();
    mHandler = handler;
    mMsg = msg;
    mIface = iface;
}

CAR_INTERFACE_IMPL_2(CThrottleService::InterfaceObserver, INetworkManagementEventObserver, IBinder);

ECode CThrottleService::InterfaceObserver::InterfaceStatusChanged(
    /* [in] */ const String& iface,
    /* [in] */ Boolean up)
{
    if (up) {
        if (TextUtils::Equals(iface, mIface)) {
            AutoPtr<IMessage> gpMsg;
            mHandler->ObtainMessage(mMsg, (IMessage**)&gpMsg);
            gpMsg->SendToTarget();
        }
    }
    return NOERROR;
}

ECode CThrottleService::InterfaceObserver::InterfaceLinkStateChanged(
    /* [in] */ const String& iface,
    /* [in] */ Boolean up)
{
    return NOERROR;
}

ECode CThrottleService::InterfaceObserver::InterfaceAdded(
    /* [in] */ const String& iface)
{
    // TODO - an interface added in the UP state should also trigger a StatusChanged
    // notification..
    if (TextUtils::Equals(iface, mIface)) {
        AutoPtr<IMessage> gpMsg;
        mHandler->ObtainMessage(mMsg, (IMessage**)&gpMsg);
        gpMsg->SendToTarget();
    }
    return NOERROR;
}

ECode CThrottleService::InterfaceObserver::InterfaceRemoved(
    /* [in] */ const String& iface)
{
    return NOERROR;
}

ECode CThrottleService::InterfaceObserver::LimitReached(
    /* [in] */ const String& limitName,
    /* [in] */ const String& iface)
{
    return NOERROR;
}

ECode CThrottleService::InterfaceObserver::InterfaceClassDataActivityChanged(
    /* [in] */ const String& label,
    /* [in] */ Boolean active)
{
    return NOERROR;
}

ECode CThrottleService::InterfaceObserver::GetHashCode(
    /* [out] */ Int32* hash)
{
    VALIDATE_NOT_NULL(hash);
    *hash = (Int32)this;
    return NOERROR;
}

ECode CThrottleService::InterfaceObserver::ToString(
    /* [out] */ String* result)
{
    return E_NOT_IMPLEMENTED;
}

//====================================================================
// CThrottleService::SettingsObserver
//====================================================================
CThrottleService::SettingsObserver::SettingsObserver(
    /* [in] */ IHandler* handler,
    /* [in] */ Int32 msg)
    : ContentObserver(handler)
{
    mHandler = handler;
    mMsg = msg;
}

void CThrottleService::SettingsObserver::Register(
    /* [in] */ IContext* context)
{
    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<IUri> uri;
    AutoPtr<ISettingsGlobal> settingsGlobal;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&settingsGlobal);
    settingsGlobal->GetUriFor(ISettingsGlobal::THROTTLE_POLLING_SEC, (IUri**)&uri);
    resolver->RegisterContentObserver(uri, FALSE, this);
    uri = NULL;
    settingsGlobal->GetUriFor(ISettingsGlobal::THROTTLE_THRESHOLD_BYTES, (IUri**)&uri);
    resolver->RegisterContentObserver(uri, FALSE, this);
    uri = NULL;
    settingsGlobal->GetUriFor(ISettingsGlobal::THROTTLE_VALUE_KBITSPS, (IUri**)&uri);
    resolver->RegisterContentObserver(uri, FALSE, this);
    uri = NULL;
    settingsGlobal->GetUriFor(ISettingsGlobal::THROTTLE_RESET_DAY, (IUri**)&uri);
    resolver->RegisterContentObserver(uri, FALSE, this);
    uri = NULL;
    settingsGlobal->GetUriFor(ISettingsGlobal::THROTTLE_NOTIFICATION_TYPE, (IUri**)&uri);
    resolver->RegisterContentObserver(uri, FALSE, this);
    uri = NULL;
    settingsGlobal->GetUriFor(ISettingsGlobal::THROTTLE_HELP_URI, (IUri**)&uri);
    resolver->RegisterContentObserver(uri, FALSE, this);
    uri = NULL;
    settingsGlobal->GetUriFor(ISettingsGlobal::THROTTLE_MAX_NTP_CACHE_AGE_SEC, (IUri**)&uri);
    resolver->RegisterContentObserver(uri, FALSE, this);
}

void CThrottleService::SettingsObserver::Unregister(
    /* [in] */ IContext* context)
{
    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    resolver->UnregisterContentObserver(this);
}

ECode CThrottleService::SettingsObserver::OnChange(
    /* [in] */  Boolean selfChange)
{
    AutoPtr<IMessage> gpMsg;
    mHandler->ObtainMessage(mMsg, (IMessage**)&gpMsg);
    gpMsg->SendToTarget();

    return NOERROR;
}

ECode CThrottleService::EnforceAccessPermission()
{
    return mContext->EnforceCallingOrSelfPermission(
        Elastos::Droid::Manifest::permission::ACCESS_NETWORK_STATE,
        String("ThrottleService"));
}

Int64 CThrottleService::NtpToWallTime(
    /* [in] */ Int64 ntpTime)
{
    // get time quickly without worrying about trusted state
    Boolean hasCache;
    mTime->HasCache(&hasCache);
    Int64 currentTimeMillis;
    mTime->GetCurrentTimeMillis(&currentTimeMillis);
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    Int64 millis;
    system->GetCurrentTimeMillis(&millis);
    Int64 bestNow = hasCache ? currentTimeMillis : millis;
    Int64 localNow = millis;
    return localNow + (ntpTime - bestNow);
}

ECode CThrottleService::GetResetTime(
    /* [in] */  const String& iface,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    FAIL_RETURN(EnforceAccessPermission());
    Int64 resetTime = 0;
    if (mRecorder != NULL) {
        mRecorder->GetPeriodEnd(&resetTime);
    }
    resetTime = NtpToWallTime(resetTime);
    *result=resetTime;
    return NOERROR;
}

ECode CThrottleService::GetPeriodStartTime(
    /* [in] */  const String& iface,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    Int64 startTime = 0;
    FAIL_RETURN(EnforceAccessPermission());
    if (mRecorder != NULL) {
        mRecorder->GetPeriodStart(&startTime);
    }
    startTime = NtpToWallTime(startTime);
    *result=startTime;
    return NOERROR;
}

ECode CThrottleService::GetCliffThreshold(
    /* [in] */ const String& iface,
    /* [in] */ Int32 cliff,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    FAIL_RETURN(EnforceAccessPermission());
    if (cliff == 1) {
       mPolicyThreshold->Get(result);
       return NOERROR;
    }
    *result = 0;
    return NOERROR;
}

ECode CThrottleService::GetCliffLevel(
    /* [in] */ const String& iface,
    /* [in] */ Int32 cliff,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    FAIL_RETURN(EnforceAccessPermission());
    if (cliff == 1) {
        mPolicyThrottleValue->Get(result);
        return NOERROR;
    }
    *result = 0;
    return NOERROR;
}

ECode CThrottleService::GetHelpUri(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    FAIL_RETURN(EnforceAccessPermission());
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<ISettingsGlobal> settingsGlobal;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&settingsGlobal);
    settingsGlobal->GetString(resolver, ISettingsGlobal::THROTTLE_HELP_URI, result);
    return NOERROR;
}

ECode CThrottleService::GetByteCount(
    /* [in] */ const String& iface,
    /* [in] */ Int32 dir,
    /* [in] */ Int32 period,
    /* [in] */ Int32 ago,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    FAIL_RETURN(EnforceAccessPermission());
    if ((period == IThrottleManager::PERIOD_CYCLE) && (mRecorder != NULL)) {
        if (dir == IThrottleManager::DIRECTION_TX)
        {
            *result = mRecorder->GetPeriodTx(ago); //check
            return NOERROR;
        }

        if (dir == IThrottleManager::DIRECTION_RX)
        {
            *result = mRecorder->GetPeriodRx(ago);
            return NOERROR;
        }
    }
    *result = 0;
    return NOERROR;
}

ECode CThrottleService::GetThrottle(
    /* [in] */ const String& iface,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    FAIL_RETURN(EnforceAccessPermission());
    Int32 value;
    mThrottleIndex->Get(&value);
    if (value == 1) {
        mPolicyThrottleValue->Get(result);
        return NOERROR;
    }
    *result = 0;
    return NOERROR;

}

//====================================================================
// CThrottleService::BroadcastReceiverSelfDefine1
//====================================================================
CThrottleService::BroadcastReceiverSelfDefine1::BroadcastReceiverSelfDefine1(
    /* [in] */ CThrottleService* host)
    : BroadcastReceiver()
    , mHost(host)
{
}

ECode CThrottleService::BroadcastReceiverSelfDefine1::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    mHost->DispatchPoll();
    return NOERROR;
}

//====================================================================
// CThrottleService::BroadcastReceiverSelfDefine2
//====================================================================
CThrottleService::BroadcastReceiverSelfDefine2::BroadcastReceiverSelfDefine2(
    /* [in] */ CThrottleService* host)
    : BroadcastReceiver()
    , mHost(host)
{
}

ECode CThrottleService::BroadcastReceiverSelfDefine2::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    mHost->DispatchReset();
    return NOERROR;
}

void CThrottleService::SystemReady()
{
    if (VDBG) {
        Slogger::V(TAG, "systemReady");
    }
    AutoPtr<IIntentFilter> intentFilter;
    CIntentFilter::New(ACTION_POLL, (IIntentFilter**)&intentFilter);
    AutoPtr<BroadcastReceiverSelfDefine1> br1 = new BroadcastReceiverSelfDefine1(this);
    AutoPtr<IIntent> intent;
    mContext->RegisterReceiver(br1, intentFilter, (IIntent**)&intent);

    intentFilter= NULL;
    intent = NULL;
    CIntentFilter::New(ACTION_RESET, (IIntentFilter**)&intentFilter);
    AutoPtr<BroadcastReceiverSelfDefine2> br2 = new BroadcastReceiverSelfDefine2(this);
    mContext->RegisterReceiver(br2, intentFilter, (IIntent**)&intent);

    // use a new thread as we don't want to stall the system for file writes
    CHandlerThread::New(TAG, (IHandlerThread**)&mThread);
    mThread->Start();
    AutoPtr<ILooper> looper;
    mThread->GetLooper((ILooper**)&looper);
    mHandler = new MyHandler(looper, this);

    AutoPtr<IMessage> gpMsg;
    mHandler->ObtainMessage(EVENT_REBOOT_RECOVERY, (IMessage**)&gpMsg);
    gpMsg->SendToTarget();

    mInterfaceObserver = new InterfaceObserver(mHandler, EVENT_IFACE_UP, mIface);
    //try {
        mNMService->RegisterObserver((INetworkManagementEventObserver*)(InterfaceObserver*)mInterfaceObserver);
    //} catch (RemoteException e) {
    //    Slog.e(TAG, "Could not register InterfaceObserver " + e);
    //}

    mSettingsObserver = new SettingsObserver(mHandler, EVENT_POLICY_CHANGED);
    mSettingsObserver->Register(mContext);
}

void CThrottleService::Shutdown()
{
    // TODO: eventually connect with ShutdownThread to persist stats during
    // graceful shutdown.

    if (mThread != NULL) {
        Boolean isQuit;
        mThread->Quit(&isQuit);
    }

    if (mSettingsObserver != NULL) {
        mSettingsObserver->Unregister(mContext);
    }

    if (mPollStickyBroadcast != NULL) {
        AutoPtr<IUserHandleHelper> helper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
        AutoPtr<IUserHandle> all;
        helper->GetALL((IUserHandle**)&all);
        mContext->RemoveStickyBroadcastAsUser(mPollStickyBroadcast, all);
    }
}

void CThrottleService::DispatchPoll()
{
    AutoPtr<IMessage> gpMsg;
    mHandler->ObtainMessage(EVENT_POLL_ALARM, (IMessage**)&gpMsg);
    gpMsg->SendToTarget();
}

void CThrottleService::DispatchReset()
{
    AutoPtr<IMessage> gpMsg;
    mHandler->ObtainMessage(EVENT_RESET_ALARM, (IMessage**)&gpMsg);
    gpMsg->SendToTarget();
}

ECode CThrottleService::ToString(
    /* [in] */ String* result)
{
    return E_NOT_IMPLEMENTED;
}

//====================================================================
// CThrottleService::MyHandler
//====================================================================
CThrottleService::MyHandler::MyHandler(
    /* [in] */ ILooper* l,
    /* [in] */ CThrottleService* host)
    : HandlerBase(l)
    , mHost(host)
{
}

ECode CThrottleService::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
    case EVENT_REBOOT_RECOVERY:
        OnRebootRecovery();
        break;
    case EVENT_POLICY_CHANGED:
        OnPolicyChanged();
        break;
    case EVENT_POLL_ALARM:
        OnPollAlarm();
        break;
    case EVENT_RESET_ALARM:
        OnResetAlarm();
        break;
    case EVENT_IFACE_UP:
        OnIfaceUp();
    }
    return NOERROR;
}

ECode CThrottleService::MyHandler::OnRebootRecovery()
{
    if (VDBG) {
        Slogger::V(TAG, "onRebootRecovery");
    }
    // check for sim change TODO
    // reregister for notification of policy change

    mHost->mThrottleIndex->Set(THROTTLE_INDEX_UNINITIALIZED);

    mHost->mRecorder = new DataRecorder(mHost->mContext, mHost);

    // get policy
    AutoPtr<IMessage> gpMsg;
    mHost->mHandler->ObtainMessage(EVENT_POLICY_CHANGED, (IMessage**)&gpMsg);
    gpMsg->SendToTarget();

    // if we poll now we won't have network connectivity or even imsi access
    // queue up a poll to happen in a little while - after ntp and imsi are avail
    // TODO - make this callback based (ie, listen for notificaitons)
    AutoPtr<IMessage> pdMsg;
    mHost->mHandler->ObtainMessage(INITIAL_POLL_DELAY_SEC, (IMessage**)&pdMsg);
    Boolean result;
    return mHost->mHandler->SendMessageDelayed(pdMsg, INITIAL_POLL_DELAY_SEC * 1000, &result);
}

ECode CThrottleService::MyHandler::OnPolicyChanged()
{
    AutoPtr<ISystemProperties> sysProp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
    String str;
    sysProp->Get(TESTING_ENABLED_PROPERTY, &str);
    Boolean testing = str == String("true");

    AutoPtr<IResources> res;
    mHost->mContext->GetResources((IResources**)&res);
    Int32 pollingPeriod;
    res->GetInteger(/*R.integer.config_datause_polling_period_sec*/0x010e002a,&pollingPeriod);

    AutoPtr<IContentResolver> resolver;
    mHost->mContext->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<ISettingsGlobal> settingsGlobal;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&settingsGlobal);
    settingsGlobal->GetInt32(resolver,
            ISettingsGlobal::THROTTLE_POLLING_SEC, pollingPeriod, &(mHost->mPolicyPollPeriodSec));

    // TODO - remove testing stuff?
    Int64 defaultThreshold;
    res->GetInteger(/*R.integer.config_datause_threshold_bytes*/0x010e002b, (Int32*)&defaultThreshold);
    Int32 defaultValue;
    res->GetInteger(/*R.integer.config_datause_throttle_kbitsps*/0x010e002c, &defaultValue);
    Int64 threshold;
    settingsGlobal->GetInt64(resolver,
            ISettingsGlobal::THROTTLE_THRESHOLD_BYTES, defaultThreshold, &threshold);
    Int32 value;
    settingsGlobal->GetInt32(resolver,
            ISettingsGlobal::THROTTLE_VALUE_KBITSPS, defaultValue, &value);

    mHost->mPolicyThreshold->Set(threshold);
    mHost->mPolicyThrottleValue->Set(value);
    if (testing) {
        mHost->mPolicyPollPeriodSec = TESTING_POLLING_PERIOD_SEC;
        mHost->mPolicyThreshold->Set(TESTING_THRESHOLD);
    }

    settingsGlobal->GetInt32(resolver,
            ISettingsGlobal::THROTTLE_RESET_DAY, -1, &(mHost->mPolicyResetDay));
    if (mHost->mPolicyResetDay == -1 ||
            ((mHost->mPolicyResetDay < 1) || (mHost->mPolicyResetDay > 28))) {
        AutoPtr<IRandom> g;
        CRandom::New((IRandom**)&g);
        Int32 i;
        g->NextInt32(28, &i);
        mHost->mPolicyResetDay = 1 + i; // 1-28
        Boolean result;
        settingsGlobal->PutInt32(resolver,
                ISettingsGlobal::THROTTLE_RESET_DAY, mHost->mPolicyResetDay, &result);
    }
    if (mHost->mIface == NULL) {
        mHost->mPolicyThreshold->Set(0);
    }

    Int32 defaultNotificationType;
    res->GetInteger(
            /*R.integer.config_datause_notification_type*/0x010e002d, &defaultNotificationType);
    settingsGlobal->GetInt32(resolver,
            ISettingsGlobal::THROTTLE_NOTIFICATION_TYPE, defaultNotificationType, &(mHost->mPolicyNotificationsAllowedMask));

    Int32 maxNtpCacheAgeSec;
    settingsGlobal->GetInt32(resolver,
            ISettingsGlobal::THROTTLE_MAX_NTP_CACHE_AGE_SEC,
            (Int32) (MAX_NTP_CACHE_AGE / 1000), &maxNtpCacheAgeSec);
    mHost->mMaxNtpCacheAge = maxNtpCacheAgeSec * 1000;

    Int64 v;
    mHost->mPolicyThreshold->Get(&v);
    if (VDBG || (v != 0)) {
        Int64 policyThreshold;
        mHost->mPolicyThreshold->Get(&policyThreshold);
        Int32 policyThrottleValue;
        mHost->mPolicyThrottleValue->Get(&policyThrottleValue);
        Slogger::D(TAG, "onPolicyChanged testing=",StringUtils::BooleanToString(testing).string(),
                ", period=", StringUtils::Int32ToString(mHost->mPolicyPollPeriodSec).string(),
                ", threshold=", StringUtils::Int64ToString(policyThreshold).string(),
                ", value=",StringUtils::Int32ToString(policyThrottleValue).string(),
                ", resetDay=",StringUtils::Int32ToString(mHost->mPolicyResetDay).string(),
                ", noteType=",StringUtils::Int32ToString(mHost->mPolicyNotificationsAllowedMask).string(),
                ", mMaxNtpCacheAge=",StringUtils::Int64ToString(mHost->mMaxNtpCacheAge).string());
    }

    // force updates
    mHost->mThrottleIndex->Set(THROTTLE_INDEX_UNINITIALIZED);

    OnResetAlarm();

    OnPollAlarm();

    AutoPtr<IIntent> broadcast;
    CIntent::New(IThrottleManager::POLICY_CHANGED_ACTION, (IIntent**)&broadcast);
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    AutoPtr<IUserHandle> all;
    helper->GetALL((IUserHandle**)&all);
    mHost->mContext->SendBroadcastAsUser(broadcast, all);
    return NOERROR;
}

ECode CThrottleService::MyHandler::OnPollAlarm()
{
    Int64 now = SystemClock::GetElapsedRealtime();
    Int64 next = now + mHost->mPolicyPollPeriodSec * 1000;

    // when trusted cache outdated, try refreshing
    Int64 cacheAge;
    mHost->mTime->GetCacheAge(&cacheAge);
    if (cacheAge > mHost->mMaxNtpCacheAge) {
        Boolean isRefreshed;
        mHost->mTime->ForceRefresh(&isRefreshed);
        if (isRefreshed) {
            if (VDBG) {
                Slogger::D(TAG, "updated trusted time, reseting alarm");
            }
            mHost->DispatchReset();
        }
    }

    Int64 incRead = 0;
    Int64 incWrite = 0;
    // try {
    const AutoPtr<INetworkStats> stats;
    mHost->mNMService->GetNetworkStatsSummaryDev((INetworkStats**)&stats);
    if (stats == NULL) {
        return NOERROR;
    }

    Int32 index;
    stats->FindIndex(mHost->mIface, INetworkStats::UID_ALL,
            INetworkStats::SET_DEFAULT, INetworkStats::TAG_NONE, &index);

    if (index != -1) {
        const AutoPtr<INetworkStatsEntry> entry;
        stats->GetValues(index, NULL, (INetworkStatsEntry**)&entry);
        Int64 rxBytes;
        entry->GetRxBytes(&rxBytes);
        Int64 txBytes;
        entry->GetTxBytes(&txBytes);
        incRead = rxBytes - mHost->mLastRead;
        incWrite = txBytes - mHost->mLastWrite;
    } else {
        // missing iface, assume stats are 0
        Slogger::W(TAG, "unable to find stats for iface ",mHost->mIface.string());
    }

    // handle iface resets - on some device the 3g iface comes and goes and gets
    // totals reset to 0.  Deal with it
    if ((incRead < 0) || (incWrite < 0)) {
        incRead += mHost->mLastRead;
        incWrite += mHost->mLastWrite;
        mHost->mLastRead = 0;
        mHost->mLastWrite = 0;
    }
    // } catch (IllegalStateException e) {
    //     Slog.e(TAG, "problem during onPollAlarm: " + e);
    // } catch (RemoteException e) {
    //     Slog.e(TAG, "problem during onPollAlarm: " + e);
    // }

    // don't count this data if we're roaming.
    AutoPtr<ISystemProperties> sysProp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
    String str;
    sysProp->Get(String("gsm.operator.isroaming")/*TelephonyProperties.PROPERTY_OPERATOR_ISROAMING*/, &str);
    Boolean roaming = String("true") == str;
    if (!roaming) {
        mHost->mRecorder->AddData(incRead, incWrite);
    }

    Int64 periodRx = mHost->mRecorder->GetPeriodRx(0);
    Int64 periodTx = mHost->mRecorder->GetPeriodTx(0);
    Int64 total = periodRx + periodTx;
    Int64 value;
    mHost->mPolicyThreshold->Get(&value);
    if (VDBG || (value != 0)) {
        Slogger::D(TAG, "onPollAlarm - roaming =",
                StringUtils::BooleanToString(roaming).string(),
                ", read =", StringUtils::Int64ToString(incRead).string(),
                ", written =", StringUtils::Int64ToString(incWrite).string(),
                ", new total =", StringUtils::Int64ToString(total).string());
    }
    mHost->mLastRead += incRead;
    mHost->mLastWrite += incWrite;

    CheckThrottleAndPostNotification(total);

    AutoPtr<IIntent> broadcast;
    CIntent::New(IThrottleManager::THROTTLE_POLL_ACTION, (IIntent**)&broadcast);
    broadcast->PutExtra(IThrottleManager::EXTRA_CYCLE_READ, periodRx);
    broadcast->PutExtra(IThrottleManager::EXTRA_CYCLE_WRITE, periodTx);
    Int64 periodStartTime;
    FAIL_RETURN(mHost->GetPeriodStartTime(mHost->mIface, &periodStartTime));
    broadcast->PutExtra(IThrottleManager::EXTRA_CYCLE_START, periodStartTime);
    Int64 resetTime;
    FAIL_RETURN(mHost->GetResetTime(mHost->mIface, &resetTime));
    broadcast->PutExtra(IThrottleManager::EXTRA_CYCLE_END, resetTime);
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    AutoPtr<IUserHandle> all;
    helper->GetALL((IUserHandle**)&all);
    mHost->mContext->SendStickyBroadcastAsUser(broadcast, all);
    mHost->mPollStickyBroadcast = broadcast;

    mHost->mAlarmManager->Cancel(mHost->mPendingPollIntent);
    mHost->mAlarmManager->Set(IAlarmManager::ELAPSED_REALTIME, next, mHost->mPendingPollIntent);
    return NOERROR;
}

ECode CThrottleService::MyHandler::OnIfaceUp()
{
    // if we were throttled before, be sure and set it again - the iface went down
    // (and may have disappeared all together) and these settings were lost
    Int32 value;
    mHost->mThrottleIndex->Get(&value);
    if (value == 1) {
    // try {
        mHost->mNMService->SetInterfaceThrottle(mHost->mIface, -1, -1);
        Int32 policyThrottleValue;
        mHost->mPolicyThrottleValue->Get(&policyThrottleValue);
        mHost->mNMService->SetInterfaceThrottle(mHost->mIface, policyThrottleValue, policyThrottleValue);
    // } catch (Exception e) {
    //     Slog.e(TAG, "error setting Throttle: " + e);
    // }
    }
    return NOERROR;
}

void CThrottleService::MyHandler::CheckThrottleAndPostNotification(
    /* [in] */ Int64 currentTotal)
{
    // is throttling enabled?
    Int64 threshold;
    mHost->mPolicyThreshold->Get(&threshold);
    if (threshold == 0) {
        ClearThrottleAndNotification();
        return;
    }

    // have we spoken with an ntp server yet?
    // this is controversial, but we'd rather err towards not throttling
    Boolean hasCache;
    mHost->mTime->HasCache(&hasCache);
    if (!hasCache) {
        Slogger::W(TAG, "missing trusted time, skipping throttle check");
        return;
    }

    // check if we need to throttle
    if (currentTotal > threshold) {
        Int32 throttleIndex;
        mHost->mThrottleIndex->Get(&throttleIndex);
        if (throttleIndex != 1) {
            mHost->mThrottleIndex->Set(1);
            if (DBG) {
                Slogger::D(TAG, "Threshold ",StringUtils::Int64ToString(threshold).string(), " exceeded!");
            }
            // try {
            Int32 policyThrottleValue;
            mHost->mPolicyThrottleValue->Get(&policyThrottleValue);
            mHost->mNMService->SetInterfaceThrottle(mHost->mIface, policyThrottleValue, policyThrottleValue);
            // } catch (Exception e) {
            //     Slog.e(TAG, "error setting Throttle: " + e);
            // }
            mHost->mNotificationManager->Cancel(0x01080568/*R.drawable.stat_sys_throttled*/);
            PostNotification(0x0104049d/*R.string.throttled_notification_title*/,
                    0x0104049e/*R.string.throttled_notification_message*/,
                    0x01080568/*R.drawable.stat_sys_throttled*/,
                    INotification::FLAG_ONGOING_EVENT);

            AutoPtr<IIntent> broadcast;
            CIntent::New(IThrottleManager::THROTTLE_ACTION, (IIntent**)&broadcast);
            broadcast->PutExtra(IThrottleManager::EXTRA_THROTTLE_LEVEL, policyThrottleValue);
            AutoPtr<IUserHandleHelper> helper;
            CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
            AutoPtr<IUserHandle> all;
            helper->GetALL((IUserHandle**)&all);
            mHost->mContext->SendStickyBroadcastAsUser(broadcast, all);
        }
    }
    else {
        ClearThrottleAndNotification();
        if ((mHost->mPolicyNotificationsAllowedMask & NOTIFICATION_WARNING) != 0) {
            // check if we should warn about throttle
            // pretend we only have 1/2 the time remaining that we actually do
            // if our burn rate in the period so far would have us exceed the limit
            // in that 1/2 window, warn the user.
            // this gets more generous in the early to middle period and converges back
            // to the limit as we move toward the period end.

            // adding another factor - it must be greater than the total cap/4
            // else we may get false alarms very early in the period..  in the first
            // tenth of a percent of the period if we used more than a tenth of a percent
            // of the cap we'd get a warning and that's not desired.
            Int64 start;
            mHost->mRecorder->GetPeriodStart(&start);
            Int64 end;
            mHost->mRecorder->GetPeriodEnd(&end);
            Int64 periodLength = end - start;
            AutoPtr<ISystem> system;
            Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
            Int64 now;
            system->GetCurrentTimeMillis(&now);
            Int64 timeUsed = now - start;
            Int64 warningThreshold = 2 * threshold * timeUsed / (timeUsed + periodLength);

            if ((currentTotal > warningThreshold) && (currentTotal > threshold / 4)) {
                if (mHost->mWarningNotificationSent == FALSE) {
                    mHost->mWarningNotificationSent = TRUE;
                    mHost->mNotificationManager->Cancel(0x01080568/*R.drawable.stat_sys_throttled*/);
                    PostNotification(0x0104049b/*R.string.throttle_warning_notification_title*/,
                        0x0104049c/*R.string.throttle_warning_notification_message*/,
                        0x01080568/*R.drawable.stat_sys_throttled*/, 0);
                }
            } else {
                if (mHost->mWarningNotificationSent == TRUE) {
                    mHost->mNotificationManager->Cancel(0x01080568/*R.drawable.stat_sys_throttled*/);
                    mHost->mWarningNotificationSent =FALSE;
                }
            }
        }
    }
}

void CThrottleService::MyHandler::PostNotification(
    /* [in] */ Int32 titleInt,
    /* [in] */ Int32 messageInt,
    /* [in] */ Int32 icon,
    /* [in] */ Int32 flags)
{
    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    // TODO - fix up intent
    intent->SetClassName(String("com.android.phone"), String("com.android.phone.DataUsage"));
    intent->SetFlags(IIntent::FLAG_ACTIVITY_NO_HISTORY);

    AutoPtr<IPendingIntentHelper> pendingIntenthelper;
    CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&pendingIntenthelper);

    AutoPtr<IUserHandleHelper> userHandleHelper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&userHandleHelper);
    AutoPtr<IUserHandle> current;
    userHandleHelper->GetCURRENT((IUserHandle**)&current);

    AutoPtr<IPendingIntent> pi;
    pendingIntenthelper->GetActivityAsUser(mHost->mContext, 0, intent, 0,
            NULL, current, (IPendingIntent**)&pi);

    AutoPtr<IResourcesHelper> resourcesHelper;
    CResourcesHelper::AcquireSingleton((IResourcesHelper**)&resourcesHelper);
    AutoPtr<IResources> r;
    resourcesHelper->GetSystem((IResources**)&r);
    AutoPtr<ICharSequence> title;
    r->GetText(titleInt, (ICharSequence**)&title);
    AutoPtr<ICharSequence> message;
    r->GetText(messageInt, (ICharSequence**)&message);

    if (mHost->mThrottlingNotification == NULL) {
        CNotification::New( (INotification**)&(mHost->mThrottlingNotification));
        mHost->mThrottlingNotification->SetWhen(0);
        // TODO -  fixup icon
        mHost->mThrottlingNotification->SetIcon(icon);
        Int32 defaults;
        mHost->mThrottlingNotification->GetDefaults(&defaults);
        mHost->mThrottlingNotification->SetDefaults(defaults &= ~ INotification::DEFAULT_SOUND);
    }
    mHost->mThrottlingNotification->SetFlags(flags);
    mHost->mThrottlingNotification->SetTickerText(title);
    mHost->mThrottlingNotification->SetLatestEventInfo(mHost->mContext, title, message, pi);

    AutoPtr<IUserHandle> all;
    userHandleHelper->GetALL((IUserHandle**)&all);
    Int32 i;
    mHost->mThrottlingNotification->GetIcon(&i);
    mHost->mNotificationManager->NotifyAsUser(String(NULL), i, mHost->mThrottlingNotification, all);
}

void CThrottleService::MyHandler::ClearThrottleAndNotification()
{
    Int32 throttleIndex;
    mHost->mThrottleIndex->Get(&throttleIndex);
    if (throttleIndex != THROTTLE_INDEX_UNTHROTTLED) {
        mHost->mThrottleIndex->Set(THROTTLE_INDEX_UNTHROTTLED);
        // try {
        mHost->mNMService->SetInterfaceThrottle(mHost->mIface, -1, -1);
        // } catch (Exception e) {
        //     Slog.e(TAG, "error clearing Throttle: " + e);
        // }
        AutoPtr<IIntent> broadcast;
        CIntent::New(IThrottleManager::THROTTLE_ACTION,(IIntent**)&broadcast);
        broadcast->PutExtra(IThrottleManager::EXTRA_THROTTLE_LEVEL, -1);
        AutoPtr<IUserHandleHelper> userHandleHelper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&userHandleHelper);
        AutoPtr<IUserHandle> all;
        userHandleHelper->GetALL((IUserHandle**)&all);
        mHost->mContext->SendStickyBroadcastAsUser(broadcast, all);
        mHost->mNotificationManager->CancelAsUser(String(NULL), 0x01080568/*R::drawable::stat_sys_throttled*/, all);
        mHost->mWarningNotificationSent = FALSE;
    }
}

AutoPtr<ICalendar> CThrottleService::MyHandler::CalculatePeriodEnd(
    /* [in] */ Int64 now)
{
    AutoPtr<ICalendarHelper> calendarHelper;
    CCalendarHelper::AcquireSingleton((ICalendarHelper**)&calendarHelper);
    AutoPtr<ICalendar> end;
    calendarHelper->GetInstance((ICalendar**)&end);
    end->SetTimeInMillis(now);
    Int32 day;
    end->Get(ICalendar::DAY_OF_MONTH, &day);
    end->Set(ICalendar::DAY_OF_MONTH, mHost->mPolicyResetDay);
    end->Set(ICalendar::HOUR_OF_DAY, 0);
    end->Set(ICalendar::MINUTE, 0);
    end->Set(ICalendar::SECOND, 0);
    end->Set(ICalendar::MILLISECOND, 0);

    if (day >= mHost->mPolicyResetDay) {
        Int32 month;
        end->Get(ICalendar::MONTH, &month);
        if (month == ICalendar::DECEMBER) {
            Int32 year;
            end->Get(ICalendar::YEAR, &year);
            end->Set(ICalendar::YEAR, year + 1);
            month = ICalendar::JANUARY - 1;
        }
        end->Set(ICalendar::MONTH, month + 1);
    }

    // TODO - remove!
    AutoPtr<ISystemProperties> sysProp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
    String value;
    if (sysProp->Get(TESTING_ENABLED_PROPERTY, &value), value == String("true")) {
//        end = GregorianCalendar::GetInstance();
//        end->SetTimeInMillis(now);
//        end->Add(ICalendar::SECOND, TESTING_RESET_PERIOD_SEC);
    }
    return end;
}

AutoPtr<ICalendar> CThrottleService::MyHandler::CalculatePeriodStart(
    /* [in] */ ICalendar* end)
{
    AutoPtr<IInterface> temp;
    ICloneable::Probe(end)->Clone((IInterface**)&temp);
    AutoPtr<ICalendar> start = ICalendar::Probe(temp);
    Int32 month;
    end->Get(ICalendar::MONTH, &month);
    if (month == ICalendar::JANUARY) {
        month = ICalendar::DECEMBER + 1;
        Int32 year;
        start->Get(ICalendar::YEAR, &year);
        start->Set(ICalendar::YEAR, year - 1);
    }
    start->Set(ICalendar::MONTH, month - 1);

    // TODO - remove!!
    AutoPtr<ISystemProperties> sysProp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
    String value;
    if (sysProp->Get(TESTING_ENABLED_PROPERTY, &value), value == String("true")) {
        temp = NULL;
        ICloneable::Probe(end)->Clone((IInterface**)&temp);
        start = ICalendar::Probe(temp);
        start->Add(ICalendar::SECOND, -TESTING_RESET_PERIOD_SEC);
    }
    return start;
}

ECode CThrottleService::MyHandler::OnResetAlarm()
{
    Int64 policyThreshold;
    mHost->mPolicyThreshold->Get(&policyThreshold);
    if (VDBG || policyThreshold != 0) {
        Slogger::E(TAG, "onResetAlarm - last period had %ld bytes read and %ld written", mHost->mRecorder->GetPeriodRx(0), mHost->mRecorder->GetPeriodTx(0));
    }
    // when trusted cache outdated, try refreshing
    Int64 cacheAge;
    mHost->mTime->GetCacheAge(&cacheAge);
    if (cacheAge > mHost->mMaxNtpCacheAge) {
        Boolean isRefresh;
        mHost->mTime->ForceRefresh(&isRefresh);
    }

    // as long as we have a trusted time cache, we always reset alarms,
    // even if the refresh above failed.
    Boolean hasCache;
    mHost->mTime->HasCache(&hasCache);
    if (hasCache) {
        Int64 now;
        mHost->mTime->GetCurrentTimeMillis(&now);
        AutoPtr<ICalendar> end = CalculatePeriodEnd(now);
        AutoPtr<ICalendar> start = CalculatePeriodStart(end);

        if (mHost->mRecorder->SetNextPeriod(start, end)) {
            OnPollAlarm();
        }

        mHost->mAlarmManager->Cancel(mHost->mPendingResetIntent);
        Int64 offset;
        end->GetTimeInMillis(&offset);
        offset -= now;
        // use Elapsed realtime so clock changes don't fool us.
        mHost->mAlarmManager->Set(IAlarmManager::ELAPSED_REALTIME,
                SystemClock::GetElapsedRealtime() + offset,
                mHost->mPendingResetIntent);
    } else {
        if (VDBG) {
            Slogger::D(TAG, "no trusted time, not resetting period");
        }
    }
    return NOERROR;
}

//====================================================================
// CThrottleService::DataRecorder
//====================================================================
CThrottleService::DataRecorder::DataRecorder(
    /* [in] */ IContext* context,
    /* [in] */ CThrottleService* parent)
    : mParent(parent)
    , mContext(context)
{
//    mContext->GetSystemService(IContext::TELEPHONY_SERVICE, (IInterface**)&mTelephonyManager);
    {
        AutoLock lock(mParentLock);

        mPeriodCount = 6;
        mPeriodRxData = ArrayOf<Int64>::Alloc(mPeriodCount);
        mPeriodTxData = ArrayOf<Int64>::Alloc(mPeriodCount);

        AutoPtr<ICalendarHelper> ch;
        CCalendarHelper::AcquireSingleton((ICalendarHelper**)&ch);
        ch->GetInstance((ICalendar**)&mPeriodStart);
        ch->GetInstance((ICalendar**)&mPeriodEnd);

        Retrieve();
    }
}

Boolean CThrottleService::DataRecorder::SetNextPeriod(
    /* [in] */ ICalendar* start,
    /* [in] */ ICalendar* end)
{
    // TODO - how would we deal with a dual-IMSI device?
    CheckForSubscriberId();
    Boolean startNewPeriod = TRUE;

    Int64 t1, t2, t3, t4;
    Boolean b1,b2;
    start->Equals(mPeriodStart, &b1);
    end->Equals(mPeriodEnd, &b2);
    if (b1 && b2) {
        // same endpoints - keep collecting
        if (VDBG) {
            start->GetTimeInMillis(&t1);
            end->GetTimeInMillis(&t2);
            Slogger::D(TAG, "same period (%ld,%ld)- ammending data", t1, t2);
        }
        startNewPeriod = FALSE;
    }
    else {
        if (VDBG) {
            start->Equals(mPeriodEnd, &b1);
            start->IsAfter(mPeriodEnd, &b2);
            if(b1 || b2) {
                start->GetTimeInMillis(&t1);
                end->GetTimeInMillis(&t2);
                mPeriodEnd->GetTimeInMillis(&t3);
                Slogger::D(TAG, "next period (%ld,%ld) - old end was %ld, following", t1,t2,t3);
            } else {
                start->GetTimeInMillis(&t1);
                end->GetTimeInMillis(&t2);
                mPeriodStart->GetTimeInMillis(&t3);
                mPeriodEnd->GetTimeInMillis(&t4);
                Slogger::D(TAG, "new period (%ld,%ld) replacing old (%ld,%ld)",t1,t2,t3,t4);
            }
        }
        {
            AutoLock lock(mParentLock);
            ++mCurrentPeriod;
            if (mCurrentPeriod >= mPeriodCount) {
                mCurrentPeriod = 0;
            }
            (*mPeriodRxData)[mCurrentPeriod] = 0;
            (*mPeriodTxData)[mCurrentPeriod] = 0;
        }
    }
    SetPeriodStart(start);
    SetPeriodEnd(end);
    Record();
    return startNewPeriod;
}

ECode CThrottleService::DataRecorder::GetPeriodEnd(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    AutoLock lock(mParentLock);
    mPeriodEnd->GetTimeInMillis(result);
    return NOERROR;
}

void CThrottleService::DataRecorder::SetPeriodEnd(
    /* [in] */ ICalendar* end)
{
    AutoLock lock(mParentLock);
    mPeriodEnd = end;
}

ECode CThrottleService::DataRecorder::GetPeriodStart(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    AutoLock lock(&mParentLock);
    mPeriodStart->GetTimeInMillis(result);
    return NOERROR;
}

void CThrottleService::DataRecorder::SetPeriodStart(
    /* [in] */ ICalendar* start)
{
    AutoLock lock(mParentLock);
    mPeriodStart = start;
}

ECode CThrottleService::DataRecorder::GetPeriodCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    AutoLock lock(mParentLock);
    *result = mPeriodCount;
    return NOERROR;
}

void CThrottleService::DataRecorder::ZeroData(
    /* [in] */ Int32 field)
{
    AutoLock lock(mParentLock);
    for(Int32 period = 0; period<mPeriodCount; period++) {
        (*mPeriodRxData)[period] = 0;
        (*mPeriodTxData)[period] = 0;
    }
    mCurrentPeriod = 0;
}

void CThrottleService::DataRecorder::AddData(
    /* [in] */ Int64 bytesRead,
    /* [in] */ Int64 bytesWritten)
{
    CheckForSubscriberId();
    {
        AutoLock lock(mParentLock);
        (*mPeriodRxData)[mCurrentPeriod] += bytesRead;
        (*mPeriodTxData)[mCurrentPeriod] += bytesWritten;
    }
    Record();
}

AutoPtr<IFile> CThrottleService::DataRecorder::GetDataFile()
{
    AutoPtr<IEnvironment> env;
    CEnvironment::AcquireSingleton((IEnvironment**)&env);
    AutoPtr<IFile> dataDir;
    env->GetDataDirectory((IFile**)&dataDir);
    AutoPtr<IFile> throttleDir;
    CFile::New(dataDir, String("system/throttle"),(IFile**)&throttleDir);
    Boolean result;
    throttleDir->Mkdirs(&result);
    String mImsi;
//    mTelephonyManager->GetSubscriberId(&mImsi);
    AutoPtr<IFile> dataFile;
    if (mImsi.IsNull()) {
        dataFile = UseMRUFile(throttleDir);
        if (VDBG) {
            String path;
            dataFile->GetPath(&path);
            Slogger::V(TAG, "imsi not available yet, using %s", path.string());
        }
    }
    else {
       String imsiHash = StringUtils::ToString(mImsi.GetHashCode());
       CFile::New(throttleDir, imsiHash, (IFile**)&dataFile);
    }
    // touch the file so it's not LRU
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    Int64 now;
    system->GetCurrentTimeMillis(&now);
    dataFile->SetLastModified(now, &result);
    CheckAndDeleteLRUDataFile(throttleDir);
    return dataFile;
}

void CThrottleService::DataRecorder::CheckForSubscriberId()
{
    if (mImsi != NULL) return;

//    mTelephonyManager->GetSubscriberId(&mImsi);
    if (mImsi == NULL) return;

    if (VDBG) {
        Slogger::D(TAG, "finally have imsi - retreiving data");
    }
    Retrieve();
}

void CThrottleService::DataRecorder::CheckAndDeleteLRUDataFile(
    /* [in] */ IFile* dir)
{
    AutoPtr<ArrayOf<IFile* > > files;
    dir->ListFiles((ArrayOf<IFile*>**)&files);

    Int32 filesize = files->GetLength();
    if (files == NULL || filesize <= MAX_SIMS_SUPPORTED) {
        return;
    }
    if (DBG) {
        Slogger::D(TAG, "Too many data files");
    }
    do {
        AutoPtr<IFile> oldest;
        for (Int32 i = 0; i < filesize; i++){
            AutoPtr<IFile> f = (*files)[i];
            if (oldest == NULL) {
             oldest = f;
            }
            Int64 t1, t2;
            oldest->LastModified(&t1);
            f->LastModified(&t2);
            if (t1 > t2) {
                oldest = f;
            }
        }
        if (oldest == NULL) return;
        if (DBG) {
            //Slogger::D(TAG, " deleting " + oldest);
        }
        Boolean isDeleted;
        oldest->Delete(&isDeleted);
        files = NULL;
        dir->ListFiles((ArrayOf<IFile*>**)&files);
        filesize = files->GetLength();
    } while (filesize > MAX_SIMS_SUPPORTED);
}

AutoPtr<IFile> CThrottleService::DataRecorder::UseMRUFile(
    /* [in] */ IFile* dir)
{
    AutoPtr<IFile> newest = NULL;
    AutoPtr<ArrayOf<IFile*> > files;
    dir->ListFiles((ArrayOf<IFile*>**)&files);
    if (files != NULL) {
        Int32 filesize = files->GetLength();
        Int32 i;
        for (i = 0; i < filesize; i++){
            AutoPtr<IFile> f = (*files)[i];
            if (newest == NULL) {
                newest = f;
            }
            Int64 t1, t2;
            newest->LastModified(&t1);
            f->LastModified(&t2);
            if (t1 > t2) newest = f;
        }
    }

    if (newest == NULL) {
        CFile::New(dir, String("temp"), (IFile**)&newest);
    }
    return newest;

}

void CThrottleService::DataRecorder::Record()
{
    // 1 int version
    // 1 int mPeriodCount
    // 13*6 long[PERIOD_COUNT] mPeriodRxData
    // 13*6 long[PERIOD_COUNT] mPeriodTxData
    // 1  int mCurrentPeriod
    // 13 long periodStartMS
    // 13 long periodEndMS
    // 200 chars max
    StringBuilder builder;
    builder += DATA_FILE_VERSION;
    builder += ":";
    builder += mPeriodCount;
    builder += ":";
    for(Int32 i = 0; i < mPeriodCount; i++) {
        builder += (*mPeriodRxData)[i];
        builder += ":";
    }
    for(Int32 i = 0; i < mPeriodCount; i++) {
        builder += (*mPeriodTxData)[i];
        builder += ":";
    }
    builder += mCurrentPeriod;
    builder += ":";

    Int64 t1, t2;
    mPeriodStart->GetTimeInMillis(&t1);
    builder += t1;
    builder += ":";
    mPeriodEnd->GetTimeInMillis(&t2);
    builder += t2;

    AutoPtr<IBufferedWriter> out;
    // try {
        AutoPtr<IFile> file = GetDataFile();
        AutoPtr<IFileWriter> fileWriter;
        CFileWriter::New(file, (IFileWriter**)&fileWriter);
        CBufferedWriter::New(fileWriter, 256, (IBufferedWriter**)&out);
        String result;
        builder.ToString(&result);
        out->WriteString(result);
    // } catch (IOException e) {
    //     Slog.e(TAG, "Error writing data file");
    //     return;
    // } finally {
    if (out != NULL) {
        ICloseable::Probe(out)->Close();
    }
// }
}

void CThrottleService::DataRecorder::Retrieve()
{
    // clean out any old data first.  If we fail to read we don't want old stuff
    ZeroData(0);
    AutoPtr<IFile> f = GetDataFile();
    AutoPtr<ArrayOf<byte> > buffer;
    AutoPtr<IFileInputStream> s;
    // try {
    Int64 len;
    f->GetLength(&len);
    buffer = ArrayOf<byte>::Alloc((Int32)len);
    ECode ec = CFileInputStream::New(f, (IFileInputStream**)&s);
    if (FAILED(ec)) {
        if (s != NULL) {
            ICloseable::Probe(s)->Close();
        }

        Slogger::E(TAG, "Error reading data file");
        return;
    }
    // } catch (IOException e) {
    //
    //     return;
    // } finally {
    Int32 number = 0;
    if (s != NULL) {
        s->ReadBytes(buffer, &number);
        ICloseable::Probe(s)->Close();
    }
//   }
    String data = StringUtils::BytesToHexString(*buffer);
    if (data.IsNullOrEmpty()) {
        if (DBG) Slogger::D(TAG, "data file empty");
        return;
    }

    AutoPtr<ArrayOf<String> > parsed;
    StringUtils::Split(data,String(":"), (ArrayOf<String>**)&parsed);
    Int32 parsedUsed = 0;

    if (parsed->GetLength() < 6) {
        Slogger::E(TAG, "reading data file with insufficient length - ignoring");
        return;
    }

    Int32 periodCount;
    AutoPtr<ArrayOf<Int64> > periodRxData;
    AutoPtr<ArrayOf<Int64> > periodTxData;
    Int32 currentPeriod;
    AutoPtr<ICalendar> periodStart;
    AutoPtr<ICalendar> periodEnd;
    // try {
    if (StringUtils::ParseInt32((*parsed)[parsedUsed++])!= DATA_FILE_VERSION) {
        Slogger::D(TAG, "reading data file with bad version - ignoring");
        return;
    }
    periodCount = StringUtils::ParseInt32((*parsed)[parsedUsed++]);
    if (parsed->GetLength() != 5 + (2 * periodCount)) {
        Slogger::E(TAG, "reading data file with bad length (%d != (5 + (2 * %d)) ) - ignoring",parsed->GetLength(),periodCount);
        return;
    }

    periodRxData = ArrayOf<Int64>::Alloc(periodCount);
    for (Int32 i = 0; i < periodCount; i++) {
        (*periodRxData)[i] = StringUtils::ParseInt64((*parsed)[parsedUsed++]);
    }
    periodTxData = ArrayOf<Int64>::Alloc(periodCount);
    for (Int32 i = 0; i < periodCount; i++) {
        (*periodTxData)[i] = StringUtils::ParseInt64((*parsed)[parsedUsed++]);
    }

    currentPeriod = StringUtils::ParseInt32((*parsed)[parsedUsed++]);

    CGregorianCalendar::New((IGregorianCalendar**)&periodStart);
    periodStart->SetTimeInMillis(StringUtils::ParseInt64((*parsed)[parsedUsed++]));
    CGregorianCalendar::New((IGregorianCalendar**)&periodEnd);
    periodEnd->SetTimeInMillis(StringUtils::ParseInt64((*parsed)[parsedUsed++]));
    // } catch (Exception e) {
    //     Slog.e(TAG, "Error parsing data file - ignoring");
    //     return;
    // }
    {
        AutoLock lock(mParentLock);
        mPeriodCount = periodCount;
        mPeriodRxData = periodRxData;
        mPeriodTxData = periodTxData;
        mCurrentPeriod = currentPeriod;
        mPeriodStart = periodStart;
        mPeriodEnd = periodEnd;
    }
}

Int64 CThrottleService::DataRecorder::GetPeriodRx(
    /* [in] */ Int32 which)
{
    AutoLock lock(mParentLock);
    if (which > mPeriodCount) return 0L;
    which = mCurrentPeriod - which;
    if (which < 0) which += mPeriodCount;
    return (*mPeriodRxData)[which];
}

Int64 CThrottleService::DataRecorder::GetPeriodTx(
    /* [in] */ Int32 which)
{
     AutoLock lock(mParentLock);
     if (which > mPeriodCount) return 0;
     which = mCurrentPeriod - which;
     if (which < 0) which += mPeriodCount;
     return (*mPeriodTxData)[which];
}

} // namespace Server
} // namepsace Droid
} // namespace Elastos
