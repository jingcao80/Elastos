
#include "CWifiService.h"
#include "am/CBatteryStatsService.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/Manifest.h"
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/etl/Algorithm.h>

using Elastos::Core::StringBuilder;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Core::ICharSequence;
using Elastos::Utility::Concurrent::Atomic::CAtomicBoolean;
using Elastos::Utility::Concurrent::Atomic::CAtomicInteger32;
using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Droid::Bluetooth::IBluetoothAdapter;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Internal::Telephony::ITelephonyIntents;
using Elastos::Droid::Net::NetworkInfoState;
using Elastos::Droid::Net::CNetworkInfo;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Net::CTrafficStats;
using Elastos::Droid::Net::ITrafficStats;
using Elastos::Droid::Wifi::IWifiManager;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::CHandlerThread;
using Elastos::Droid::Os::CMessenger;
using Elastos::Droid::Os::IMessageHelper;
using Elastos::Droid::Os::CMessageHelper;
using Elastos::Droid::Os::CWorkSource;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Utility::CParcelableObjectContainer;
using Elastos::Droid::Server::Am::CBatteryStatsService;

namespace Elastos {
namespace Droid {
namespace Server {

const String CWifiService::TAG("CWifiService");
const Boolean CWifiService::DBG = FALSE;
const Int32 CWifiService::IDLE_REQUEST;
const Int32 CWifiService::POLL_TRAFFIC_STATS_INTERVAL_MSECS;
const Int64 CWifiService::DEFAULT_IDLE_MS;
const String CWifiService::ACTION_DEVICE_IDLE("com.android.server.WifiManager.action.DEVICE_IDLE");
const Int32 CWifiService::WIFI_DISABLED;
const Int32 CWifiService::WIFI_ENABLED;
const Int32 CWifiService::WIFI_ENABLED_AIRPLANE_OVERRIDE;
const Int32 CWifiService::WIFI_DISABLED_AIRPLANE_ON;
const Int32 CWifiService::ICON_NETWORKS_AVAILABLE;
const Int32 CWifiService::NUM_SCANS_BEFORE_ACTUALLY_SCANNING;

//=========================================================
// CWifiService::WifiServiceBroadcastReceiver
//=========================================================

CWifiService::WifiServiceBroadcastReceiver::WifiServiceBroadcastReceiver(
    /* [in] */ CWifiService* owner)
    : mWifiService(owner)
{
}

ECode CWifiService::WifiServiceBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);

    AutoPtr<ISettingsGlobal> global;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
    AutoPtr<IContentResolver> resolver;
    mWifiService->mContext->GetContentResolver((IContentResolver**)&resolver);

    Int64 idleMillis;
    global->GetInt64(resolver, ISettingsGlobal::WIFI_IDLE_MS, DEFAULT_IDLE_MS, &idleMillis);
    Int32 stayAwakeConditions;
    global->GetInt32(resolver, ISettingsGlobal::STAY_ON_WHILE_PLUGGED_IN, 0, &stayAwakeConditions);

    if (action.Equals(IIntent::ACTION_SCREEN_ON)) {
        if (CWifiService::DBG) {
            Slogger::D(CWifiService::TAG, "ACTION_SCREEN_ON");
        }

        mWifiService->mAlarmManager->Cancel(mWifiService->mIdleIntent);
        mWifiService->mScreenOff = false;
        mWifiService->EvaluateTrafficStatsPolling();
        mWifiService->SetDeviceIdleAndUpdateWifi(false);
    }
    else if (action.Equals(IIntent::ACTION_SCREEN_OFF)) {
        if (CWifiService::DBG) {
            Slogger::D(CWifiService::TAG, "ACTION_SCREEN_OFF");
        }
        mWifiService->mScreenOff = TRUE;
        mWifiService->EvaluateTrafficStatsPolling();
        /*
         * Set a timer to put Wi-Fi to sleep, but only if the screen is off
         * AND the "stay on while plugged in" setting doesn't match the
         * current power conditions (i.e, not plugged in, plugged in to USB,
         * or plugged in to AC).
         */
        if (!ShouldWifiStayAwake(stayAwakeConditions, mWifiService->mPluggedType)) {
            NetworkInfoDetailedState state;
            mWifiService->mNetworkInfo->GetDetailedState(&state);
            //Delayed shutdown if wifi is connected
            if (state == Elastos::Droid::Net::NetworkInfoDetailedState_CONNECTED) {
                if (CWifiService::DBG)
                    Slogger::D(CWifiService::TAG, "setting ACTION_DEVICE_IDLE: %lld ms", idleMillis);
                AutoPtr<ISystem> system;
                Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
                Int64 millis;
                system->GetCurrentTimeMillis(&millis);
                Int64 triggerTime = millis + idleMillis;
                mWifiService->mAlarmManager->Set(IAlarmManager::RTC_WAKEUP, triggerTime, mWifiService->mIdleIntent);
            }
            else {
                mWifiService->SetDeviceIdleAndUpdateWifi(TRUE);
            }
        }
    }
    else if (action.Equals(ACTION_DEVICE_IDLE)) {
        mWifiService->SetDeviceIdleAndUpdateWifi(TRUE);
    }
    else if (action.Equals(IIntent::ACTION_BATTERY_CHANGED)) {
        /*
         * Set a timer to put Wi-Fi to sleep, but only if the screen is off
         * AND we are transitioning from a state in which the device was supposed
         * to stay awake to a state in which it is not supposed to stay awake.
         * If "stay awake" state is not changing, we do nothing, to avoid resetting
         * the already-set timer.
         */
        Int32 pluggedType;
        intent->GetInt32Extra(String("plugged"), 0, &pluggedType);
        if (CWifiService::DBG) {
            Slogger::D(CWifiService::TAG, "ACTION_BATTERY_CHANGED pluggedType: %d", pluggedType);
        }
        if (mWifiService->mScreenOff
            && ShouldWifiStayAwake(stayAwakeConditions, mWifiService->mPluggedType)
            && !ShouldWifiStayAwake(stayAwakeConditions, pluggedType)) {
            AutoPtr<ISystem> system;
            Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
            Int64 millis;
            system->GetCurrentTimeMillis(&millis);
            Int64 triggerTime = millis + idleMillis;
            if (CWifiService::DBG) {
                Slogger::D(CWifiService::TAG, "setting ACTION_DEVICE_IDLE timer for %lld ms", idleMillis);
            }
            mWifiService->mAlarmManager->Set(IAlarmManager::RTC_WAKEUP, triggerTime, mWifiService->mIdleIntent);
        }

        mWifiService->mPluggedType = pluggedType;
    }
    else if (action.Equals(IBluetoothAdapter::ACTION_CONNECTION_STATE_CHANGED)) {
        Int32 state;
        intent->GetInt32Extra(IBluetoothAdapter::EXTRA_CONNECTION_STATE,
                IBluetoothAdapter::STATE_DISCONNECTED, &state);
        mWifiService->mWifiStateMachine->SendBluetoothAdapterStateChange(state);
    }
    else if (action.Equals(ITelephonyIntents::ACTION_EMERGENCY_CALLBACK_MODE_CHANGED)) {
        Boolean ecmState;
        intent->GetBooleanExtra(String("phoneinECMState"), FALSE, &ecmState);
        mWifiService->mEmergencyCallbackMode = ecmState;
        mWifiService->UpdateWifiState();
    }
    return NOERROR;
}

Boolean CWifiService::WifiServiceBroadcastReceiver::ShouldWifiStayAwake(
    /* [in] */ Int32 stayAwakeConditions,
    /* [in] */ Int32 pluggedType)
{
    //Never sleep as long as the user has not changed the settings
    AutoPtr<ISettingsGlobal> global;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
    AutoPtr<IContentResolver> resolver;
    mWifiService->mContext->GetContentResolver((IContentResolver**)&resolver);
    Int32 wifiSleepPolicy;
    global->GetInt32(resolver, ISettingsGlobal::WIFI_SLEEP_POLICY,
        ISettingsGlobal::WIFI_SLEEP_POLICY_NEVER, &wifiSleepPolicy);

    if (wifiSleepPolicy == ISettingsGlobal::WIFI_SLEEP_POLICY_NEVER) {
        // Never sleep
        return TRUE;
    }
    else if ((wifiSleepPolicy == ISettingsGlobal::WIFI_SLEEP_POLICY_NEVER_WHILE_PLUGGED)
        && (pluggedType != 0)) {
        // Never sleep while plugged, and we're plugged
        return TRUE;
    }
    else {
        // Default
        return ShouldDeviceStayAwake(stayAwakeConditions, pluggedType);
    }
}

Boolean CWifiService::WifiServiceBroadcastReceiver::ShouldDeviceStayAwake(
    /* [in] */ Int32 stayAwakeConditions,
    /* [in] */ Int32 pluggedType)
{
    return (stayAwakeConditions & pluggedType) != 0;
}

//=========================================================
// CWifiService::AirplaneModeChangedBroadcastReceiver
//=========================================================

CWifiService::AirplaneModeChangedBroadcastReceiver::AirplaneModeChangedBroadcastReceiver(
    /* [in] */ CWifiService* owner)
    : mWifiService(owner)
{
}

ECode CWifiService::AirplaneModeChangedBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    mWifiService->mAirplaneModeOn->Set(mWifiService->IsAirplaneModeOn());
    Boolean bval;
    mWifiService->mAirplaneModeOn->Get(&bval);
    mWifiService->HandleAirplaneModeToggled(bval);
    mWifiService->UpdateWifiState();
    return NOERROR;
}

//=========================================================
// CWifiService::StateChangedBroadcastReceiver
//=========================================================

CWifiService::StateChangedBroadcastReceiver::StateChangedBroadcastReceiver(
    /* [in] */ CWifiService* owner)
    : mWifiService(owner)
{
}

//Override
ECode CWifiService::StateChangedBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);

    if (action.Equals(IWifiManager::WIFI_STATE_CHANGED_ACTION)) {
        Int32 wifiState;
        intent->GetInt32Extra(IWifiManager::EXTRA_WIFI_STATE,
            IWifiManager::WIFI_STATE_DISABLED, &wifiState);

        mWifiService->mWifiEnabled = (wifiState == IWifiManager::WIFI_STATE_ENABLED);

        // reset & clear notification on any wifi state change
        mWifiService->ResetNotification();
    }
    else if (action.Equals(IWifiManager::NETWORK_STATE_CHANGED_ACTION)) {
        AutoPtr<IParcelable> parcelable;
        intent->GetParcelableExtra(IWifiManager::EXTRA_NETWORK_INFO, (IParcelable**)&parcelable);
        mWifiService->mNetworkInfo = INetworkInfo::Probe(parcelable);
        NetworkInfoDetailedState state;
        mWifiService->mNetworkInfo->GetDetailedState(&state);
        // reset & clear notification on a network connect & disconnect
        switch (state) {
            case Elastos::Droid::Net::NetworkInfoDetailedState_CONNECTED:
            case Elastos::Droid::Net::NetworkInfoDetailedState_DISCONNECTED:
            case Elastos::Droid::Net::NetworkInfoDetailedState_CAPTIVE_PORTAL_CHECK:
                mWifiService->EvaluateTrafficStatsPolling();
                mWifiService->ResetNotification();
                break;
        }
    }
    else if (action.Equals(IWifiManager::SCAN_RESULTS_AVAILABLE_ACTION)) {
        mWifiService->NoteScanEnd();
        mWifiService->CheckAndSetNotification();
    }
    return NOERROR;
}

//=========================================================
// CWifiService::AsyncServiceHandler
//=========================================================

ECode CWifiService::AsyncServiceHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AutoPtr<IMessageHelper> helper;
    CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case AsyncChannel::CMD_CHANNEL_HALF_CONNECTED: {
            if (DBG) Slogger::D(TAG, "AsyncServiceHandler HandleMessage CMD_CHANNEL_HALF_CONNECTED");
            Int32 arg1;
            msg->GetArg1(&arg1);
            if (arg1 == AsyncChannel::STATUS_SUCCESSFUL) {
                AutoPtr<IInterface> obj;
                msg->GetObj((IInterface**)&obj);
                AutoPtr<AsyncChannel> c = (AsyncChannel*)obj.Get();
                mOwner->mClients.PushBack(c);

                if (DBG) Slogger::D(TAG, "New client %p listening to asynchronous messages. total: %d",
                    c.Get(), mOwner->mClients.GetSize());
            }
            else {
                Slogger::E(TAG, "Client connection failure, error=%d", arg1);
            }
            break;
        }
        case AsyncChannel::CMD_CHANNEL_DISCONNECTED: {
            if (DBG) Slogger::D(TAG, "AsyncServiceHandler HandleMessage CMD_CHANNEL_DISCONNECTED");
            Int32 arg1;
            msg->GetArg1(&arg1);
            if (arg1 == AsyncChannel::STATUS_SEND_UNSUCCESSFUL) {
                if (DBG) Slogger::D(TAG, "Send failed, client connection lost");
            }
            else {
                if (DBG) Slogger::D(TAG, "Client connection lost with reason: %d", arg1);
            }
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            AutoPtr<AsyncChannel> c = (AsyncChannel*)obj.Get();
            c->Disconnected();
            mOwner->mClients.Remove(c);
            break;
        }
        case AsyncChannel::CMD_CHANNEL_FULL_CONNECTION: {
            if (DBG) Slogger::D(TAG, "AsyncServiceHandler HandleMessage CMD_CHANNEL_FULL_CONNECTION");
            AutoPtr<AsyncChannel> ac = new AsyncChannel();
            AutoPtr<IMessenger> replyTo;
            msg->GetReplyTo((IMessenger**)&replyTo);
            ac->Connect(mOwner->mContext, mOwner->mAsyncServiceHandler, replyTo);
            break;
        }
        case IWifiManager::ENABLE_TRAFFIC_STATS_POLL: {
            if (DBG) Slogger::D(TAG, "AsyncServiceHandler HandleMessage ENABLE_TRAFFIC_STATS_POLL");
            Int32 arg1;
            msg->GetArg1(&arg1);
            mOwner->mEnableTrafficStatsPoll = (arg1 == 1);
            mOwner->mTrafficStatsPollToken++;
            if (mOwner->mEnableTrafficStatsPoll) {
                mOwner->NotifyOnDataActivity();
                AutoPtr<IMessage> m;
                mOwner->mAsyncServiceHandler->ObtainMessage(IWifiManager::TRAFFIC_STATS_POLL,
                    mOwner->mTrafficStatsPollToken, 0, (IMessage**)&m);

                Boolean result;
                SendMessageDelayed(m, POLL_TRAFFIC_STATS_INTERVAL_MSECS, &result);
            }
            break;
        }
        case IWifiManager::TRAFFIC_STATS_POLL: {
            if (DBG) Slogger::D(TAG, "HandleMessage TRAFFIC_STATS_POLL");
            Int32 arg1;
            msg->GetArg1(&arg1);
            if (arg1 == mOwner->mTrafficStatsPollToken) {
                mOwner->NotifyOnDataActivity();
                AutoPtr<IMessage> m;
                mOwner->mAsyncServiceHandler->ObtainMessage(IWifiManager::TRAFFIC_STATS_POLL,
                    mOwner->mTrafficStatsPollToken, 0, (IMessage**)&m);

                Boolean result;
                SendMessageDelayed(m, POLL_TRAFFIC_STATS_INTERVAL_MSECS, &result);
            }
            break;
        }
        case IWifiManager::CONNECT_NETWORK: {
            if (DBG) Slogger::D(TAG, "AsyncServiceHandler HandleMessage CONNECT_NETWORK");
            AutoPtr<IMessage> m;
            helper->Obtain(msg, (IMessage**)&m);
            mOwner->mWifiStateMachine->SendMessage(m);
            break;
        }
        case IWifiManager::SAVE_NETWORK: {
            if (DBG) Slogger::D(TAG, "HandleMessage SAVE_NETWORK");
            AutoPtr<IMessage> m;
            helper->Obtain(msg, (IMessage**)&m);
            mOwner->mWifiStateMachine->SendMessage(m);
            break;
        }
        case IWifiManager::FORGET_NETWORK: {
            if (DBG) Slogger::D(TAG, "HandleMessage FORGET_NETWORK");
            AutoPtr<IMessage> m;
            helper->Obtain(msg, (IMessage**)&m);
            mOwner->mWifiStateMachine->SendMessage(m);
            break;
        }
        case IWifiManager::START_WPS: {
            if (DBG) Slogger::D(TAG, "HandleMessage START_WPS");
            AutoPtr<IMessage> m;
            helper->Obtain(msg, (IMessage**)&m);
            mOwner->mWifiStateMachine->SendMessage(m);
            break;
        }
        case IWifiManager::CANCEL_WPS: {
            if (DBG) Slogger::D(TAG, "HandleMessage CANCEL_WPS");
            AutoPtr<IMessage> m;
            helper->Obtain(msg, (IMessage**)&m);
            mOwner->mWifiStateMachine->SendMessage(m);
            break;
        }
        case IWifiManager::DISABLE_NETWORK: {
            if (DBG) Slogger::D(TAG, "HandleMessage DISABLE_NETWORK");
            AutoPtr<IMessage> m;
            helper->Obtain(msg, (IMessage**)&m);
            mOwner->mWifiStateMachine->SendMessage(m);
            break;
        }
        case IWifiManager::RSSI_PKTCNT_FETCH: {
            if (DBG) Slogger::D(TAG, "HandleMessage RSSI_PKTCNT_FETCH");
            AutoPtr<IMessage> m;
            helper->Obtain(msg, (IMessage**)&m);
            mOwner->mWifiStateMachine->SendMessage(m);
            break;
        }
        default: {
            Slogger::D(TAG, "WifiServicehandler.handleMessage ignoring msg=%p", msg);
            break;
        }
    }
    return NOERROR;
}


//=========================================================
// CWifiService::WifiStateMachineHandler
//=========================================================
ECode CWifiService::WifiStateMachineHandler::Clear()
{
    // break retain-cycle
    if (mWsmChannel) {
        mWsmChannel->Disconnected();
        mWsmChannel = NULL;
    }
    return NOERROR;
}

CWifiService::WifiStateMachineHandler::WifiStateMachineHandler(
    /* [in] */ ILooper* l,
    /* [in] */ CWifiService* owner)
    : HandlerBase(l)
    , mOwner(owner)
{
    mWsmChannel = new AsyncChannel();
    AutoPtr<IHandler> h = mOwner->mWifiStateMachine->GetHandler();
    mWsmChannel->Connect(mOwner->mContext, this, h);
}

ECode CWifiService::WifiStateMachineHandler::HandleMessage(
   /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case AsyncChannel::CMD_CHANNEL_HALF_CONNECTED: {
            Int32 arg1;
            msg->GetArg1(&arg1);
            if (arg1 == AsyncChannel::STATUS_SUCCESSFUL) {
                mOwner->mWifiStateMachineChannel = mWsmChannel;
            }
            else {
                Slogger::E(TAG, "WifiStateMachine connection failure, error=%d", arg1);
                mOwner->mWifiStateMachineChannel = NULL;
            }
            break;
        }
        case AsyncChannel::CMD_CHANNEL_DISCONNECTED: {
            Int32 arg1;
            msg->GetArg1(&arg1);
            Slogger::E(TAG, "WifiStateMachine channel lost, msg.arg1 =%d", arg1);
            mOwner->mWifiStateMachineChannel = NULL;
            //Re-establish connection to state machine
            AutoPtr<IHandler> h = mOwner->mWifiStateMachine->GetHandler();
            mWsmChannel->Connect(mOwner->mContext, mOwner->mWifiStateMachineHandler, h);
            break;
        }
        default: {
            Slogger::D(TAG, "WifiStateMachineHandler.handleMessage ignoring msg=%p", msg);
            break;
        }
    }
    return NOERROR;
}


//=========================================================
// CWifiService::DeathRecipient
//=========================================================
CWifiService::DeathRecipient::DeathRecipient(
   /* [in] */ Int32 mode,
   /* [in] */ const String& tag,
   /* [in] */ IBinder* binder,
   /* [in] */ IWorkSource* ws)
   : mTag(tag)
   , mMode(mode)
   , mBinder(binder)
   , mWorkSource(ws)
{
    // try {
    IProxy* proxy = (IProxy*)mBinder->Probe(EIID_IProxy);
    if (proxy != NULL) {
        proxy->LinkToDeath(this, 0);
    }
    // } catch (RemoteException e) {
        // binderDied();
    // }
}

CAR_INTERFACE_IMPL(CWifiService::DeathRecipient, IProxyDeathRecipient)

void CWifiService::DeathRecipient::UnlinkDeathRecipient()
{
    IProxy* proxy = (IProxy*)mBinder->Probe(EIID_IProxy);
    if (proxy != NULL) {
        Boolean result;
        proxy->UnlinkToDeath(this, 0, &result);
    }
}


//=========================================================
// CWifiService::WifiLock
//=========================================================
CWifiService::WifiLock::WifiLock(
    /* [in] */ Int32 lockMode,
    /* [in] */ const String& tag,
    /* [in] */ IBinder* binder,
    /* [in] */ IWorkSource* ws,
    /* [in] */ CWifiService* owner)
    : DeathRecipient(lockMode, tag, binder, ws)
    , mOwner(owner)
{}

ECode CWifiService::WifiLock::ProxyDied()
{
   {
       AutoLock lock(mOwner->mLocksLock);
       mOwner->ReleaseWifiLockLocked(mBinder);
   }
   return NOERROR;
}

String CWifiService::WifiLock::ToString()
{
    StringBuilder sb(128);
    sb.Append("WifiLock{");
    sb.Append(mTag);
    sb.Append(" type=");
    sb.Append(mMode);
    sb.Append(" binder=");
    sb.AppendObject(mBinder);
    sb.Append("}");
    return sb.ToString();
}


//=========================================================
// CWifiService::LockList
//=========================================================
Boolean CWifiService::LockList::HasLocks()
{
    AutoLock lock(mLock);
    return !mList.IsEmpty();
}

Int32 CWifiService::LockList::GetStrongestLockMode()
{
    AutoLock lock(mLock);
    if (mList.IsEmpty()) {
        return IWifiManager::WIFI_MODE_FULL;
    }

    if (mOwner->mFullHighPerfLocksAcquired > mOwner->mFullHighPerfLocksReleased) {
        return IWifiManager::WIFI_MODE_FULL_HIGH_PERF;
    }

    if (mOwner->mFullLocksAcquired > mOwner->mFullLocksReleased) {
        return IWifiManager::WIFI_MODE_FULL;
    }

    return IWifiManager::WIFI_MODE_SCAN_ONLY;
}

void CWifiService::LockList::AddLock(
   /* [in] */ WifiLock* lock)
{
    if (FindLockByBinder(lock->mBinder) == mList.End()) {
        mList.PushBack(lock);
    }
}

AutoPtr<CWifiService::WifiLock> CWifiService::LockList::RemoveLock(
    /* [in] */ IBinder* binder)
{
    List< AutoPtr<WifiLock> >::Iterator it = FindLockByBinder(binder);
    if (it != mList.End()) {
        AutoPtr<WifiLock> ret = *it;
        mList.Erase(it);
        ret->UnlinkDeathRecipient();
        return ret;
    }
    else {
        return NULL;
    }
}

List< AutoPtr<CWifiService::WifiLock> >::Iterator
CWifiService::LockList::FindLockByBinder(
    /* [in] */ IBinder* binder)
{
    List< AutoPtr<CWifiService::WifiLock> >::ReverseIterator rit;
    for (rit = mList.RBegin(); rit != mList.REnd(); ++rit) {
        if ((*rit)->mBinder.Get() == binder) {
            return (++rit).GetBase();
        }
    }
    return mList.End();
}

void CWifiService::LockList::Dump(
    /* [in] */ IPrintWriter* pw)
{
    assert(0);
    // for (WifiLock l : mList) {
    //     pw->Print("    ");
    //     pw->Println(l);
    // }
}


//=========================================================
// CWifiService::Multicaster
//=========================================================
CWifiService::Multicaster::Multicaster(
    /* [in] */ const String& tag,
    /* [in] */ IBinder* binder,
    /* [in] */ CWifiService* owner)
    : DeathRecipient(Binder::GetCallingUid(), tag, binder, NULL)
    , mOwner(owner)
{}

ECode CWifiService::Multicaster::ProxyDied()
{
    Slogger::E(TAG, "Multicaster binderDied");
    {
        AutoLock lock(mOwner->mMulticastersLock);

        List< AutoPtr<Multicaster> >::Iterator it =
                Find(mOwner->mMulticasters.Begin(), mOwner->mMulticasters.End(), AutoPtr<Multicaster>(this));
        if (it != mOwner->mMulticasters.End()) {
            mOwner->RemoveMulticasterLocked(it, mMode);
        }
    }
    return NOERROR;
}

String CWifiService::Multicaster::ToString()
{
    StringBuilder sb("Multicaster{");
    sb += mTag;
    sb += " binder=";
    sb += (Int32)mBinder.Get();
    sb += "}";
    return sb.ToString();
}


//=========================================================
// CWifiService::NotificationEnabledSettingObserver
//=========================================================
ECode CWifiService::NotificationEnabledSettingObserver::Register()
{
    AutoPtr<ISettingsGlobal> global;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
    AutoPtr<IContentResolver> cr;
    mOwner->mContext->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<IUri> uri;
    global->GetUriFor(ISettingsGlobal::WIFI_NETWORKS_AVAILABLE_NOTIFICATION_ON,
        (IUri**)&uri);

    cr->RegisterContentObserver(uri, TRUE, this);
    mOwner->mNotificationEnabled = GetValue();
    return NOERROR;
}

ECode CWifiService::NotificationEnabledSettingObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    ContentObserver::OnChange(selfChange);

    mOwner->mNotificationEnabled = GetValue();
    mOwner->ResetNotification();
    return NOERROR;
}

Boolean CWifiService::NotificationEnabledSettingObserver::GetValue()
{
    AutoPtr<IContentResolver> cr;
    mOwner->mContext->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<ISettingsGlobal> global;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
    Int32 value;
    global->GetInt32(cr, ISettingsGlobal::WIFI_NETWORKS_AVAILABLE_NOTIFICATION_ON, 1, &value);
    return value == 1;
}


//=========================================================
// CWifiService
//=========================================================
CWifiService::CWifiService()
    : mScreenOff(FALSE)
    , mDeviceIdle(FALSE)
    , mEmergencyCallbackMode(FALSE)
    , mPluggedType(FALSE)
    , mFullHighPerfLocksAcquired(0)
    , mFullHighPerfLocksReleased(0)
    , mFullLocksAcquired(0)
    , mFullLocksReleased(0)
    , mScanLocksAcquired(0)
    , mScanLocksReleased(0)
    , mMulticastEnabled(0)
    , mMulticastDisabled(0)
    , mEnableTrafficStatsPoll(FALSE)
    , mTrafficStatsPollToken(0)
    , mTxPkts(0)
    , mRxPkts(0)
    , mDataActivity(0)
    , mWifiEnabled(FALSE)
    , mIsReceiverRegistered(FALSE)
    , mNotificationEnabled(FALSE)
    , mNotificationRepeatTime(0)
    , mNotificationShown(FALSE)
    , mNumScansSinceNetworkStateChange(0)
{
    CAtomicInteger32::New(WIFI_DISABLED, (IAtomicInteger32**)&mPersistWifiState);
    CAtomicBoolean::New(FALSE, (IAtomicBoolean**)&mAirplaneModeOn);
    CNetworkInfo::New(IConnectivityManager::TYPE_WIFI, 0, String("WIFI"), String(""), (INetworkInfo**)&mNetworkInfo);
    CWorkSource::New((IWorkSource**)&mTmpWorkSource);
    mReceiver = new WifiServiceBroadcastReceiver(this);
    mLocks = new LockList(this);
}

CWifiService::~CWifiService()
{
    if (mWifiStateMachineHandler) {
        WifiStateMachineHandler* wsmh = (WifiStateMachineHandler*)mWifiStateMachineHandler.Get();
        wsmh->Clear();
        mWifiStateMachineHandler = NULL;
    }

    List< AutoPtr<AsyncChannel> >::Iterator it = mClients.Begin();
    for (; it != mClients.End(); ++it) {
        (*it)->Disconnected();
    }
    mClients.Clear();
}

ECode CWifiService::constructor(
    /* [in] */ IContext* context)
{
    mContext = context;

    AutoPtr<ISystemProperties> sysProp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
    sysProp->Get(String("wifi.interface"), String("wlan0"), &mInterfaceName);

    mWifiStateMachine = new WifiStateMachine(mContext, mInterfaceName);
    mWifiStateMachine->EnableRssiPolling(TRUE);
    mBatteryStats = CBatteryStatsService::GetService();

    AutoPtr<IInterface> tempObj;
    mContext->GetSystemService(IContext::ALARM_SERVICE, (IInterface**)&tempObj);
    mAlarmManager = IAlarmManager::Probe(tempObj.Get());

    AutoPtr<IPendingIntentHelper> helper;
    CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&helper);

    AutoPtr<IIntent> idleIntent;
    CIntent::New(ACTION_DEVICE_IDLE, NULL,(IIntent**)&idleIntent);
    helper->GetBroadcast(mContext, IDLE_REQUEST, idleIntent, 0, (IPendingIntent**)&mIdleIntent);

    AutoPtr<IIntentFilter> filter;
    AutoPtr<IIntent> stickyIntent;
    CIntentFilter::New(IIntent::ACTION_AIRPLANE_MODE_CHANGED, (IIntentFilter**)&filter);
    AutoPtr<IBroadcastReceiver> receiver = new AirplaneModeChangedBroadcastReceiver(this);
    mContext->RegisterReceiver(receiver, filter, (IIntent**)&stickyIntent);

    filter = NULL;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(IWifiManager::WIFI_STATE_CHANGED_ACTION);
    filter->AddAction(IWifiManager::NETWORK_STATE_CHANGED_ACTION);
    filter->AddAction(IWifiManager::SCAN_RESULTS_AVAILABLE_ACTION);

    receiver = new StateChangedBroadcastReceiver(this);
    stickyIntent = NULL;
    mContext->RegisterReceiver(receiver, filter, (IIntent**)&stickyIntent);

    AutoPtr<IHandlerThread> wifiThread;
    CHandlerThread::New(String("WifiService"), (IHandlerThread**)&wifiThread);
    wifiThread->Start();
    AutoPtr<ILooper> l;
    wifiThread->GetLooper((ILooper**)&l);
    mAsyncServiceHandler = new AsyncServiceHandler(l, this);
    mWifiStateMachineHandler = new WifiStateMachineHandler(l, this);

    // Setting is in seconds
    AutoPtr<ISettingsGlobal> global;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);

    Int32 intVal;
    global->GetInt32(cr, ISettingsGlobal::WIFI_NETWORKS_AVAILABLE_REPEAT_DELAY,
        900, &intVal);
    NOTIFICATION_REPEAT_DELAY_MS = intVal * 1000l;
    AutoPtr<IHandler> handler;
    CHandler::New((IHandler**)&handler);
    mNotificationEnabledSettingObserver = new NotificationEnabledSettingObserver(handler, this);
    mNotificationEnabledSettingObserver->Register();
    return NOERROR;
}

void CWifiService::NoteScanStart()
{
    AutoPtr<IWorkSource> scanWorkSource;
    {
        AutoLock lock(this);

        if (mScanWorkSource != NULL) {
            // Scan already in progress, don't add this one to battery stats
            return;
        }
        CWorkSource::New(Binder::GetCallingUid(), (IWorkSource**)&scanWorkSource);
        mScanWorkSource = scanWorkSource;
    }

    Int64 id = Binder::ClearCallingIdentity();
    // try {
    mBatteryStats->NoteWifiScanStartedFromSource(scanWorkSource);
    // } catch (RemoteException e) {
        // Log.w(TAG, e);
    // } finally {
    Binder::RestoreCallingIdentity(id);
    // }
}

void CWifiService::NoteScanEnd()
{
    AutoPtr<IWorkSource> scanWorkSource;
    {
        AutoLock lock(this);

        scanWorkSource = mScanWorkSource;
        mScanWorkSource = NULL;
    }
    if (scanWorkSource != NULL) {
        // try {
        mBatteryStats->NoteWifiScanStoppedFromSource(scanWorkSource);
        // } catch (RemoteException e) {
            // Log.w(TAG, e);
        // }
    }
}

void CWifiService::CheckAndStartWifi()
{
    mAirplaneModeOn->Set(IsAirplaneModeOn());
    mPersistWifiState->Set(GetPersistedWifiState());
    /* Start if Wi-Fi should be enabled or the saved state indicates Wi-Fi was on */
    Boolean wifiEnabled = ShouldWifiBeEnabled() || TestAndClearWifiSavedState();
    Slogger::I(TAG, "WifiService starting up with Wi-Fi %s", wifiEnabled ? "enabled" : "disabled");

    // If we are already disabled (could be due to airplane mode), avoid changing persist
    // state here
    Boolean result;
    if (wifiEnabled) SetWifiEnabled(wifiEnabled, &result);

    mWifiWatchdogStateMachine = WifiWatchdogStateMachine::MakeWifiWatchdogStateMachine(mContext);
}

Boolean CWifiService::TestAndClearWifiSavedState()
{
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    Int32 wifiSavedState = 0;
    // try {
    AutoPtr<ISettingsGlobal> global;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);

    global->GetInt32(cr, ISettingsGlobal::WIFI_SAVED_STATE, &wifiSavedState);
    if(wifiSavedState == 1) {
        Boolean result;
        global->PutInt32(cr, ISettingsGlobal::WIFI_SAVED_STATE, 0, &result);
    }
    // } catch (Settings.SettingNotFoundException e) {

    // }
    return (wifiSavedState == 1);
}

Int32 CWifiService::GetPersistedWifiState()
{
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    // try {
    AutoPtr<ISettingsGlobal> global;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
    Int32 value;
    ECode ec = global->GetInt32(cr, ISettingsGlobal::WIFI_ON, &value);
    if (SUCCEEDED(ec)) {
        return value;
    }
    // } catch (Settings.SettingNotFoundException e) {
    Boolean result;
    global->PutInt32(cr, ISettingsGlobal::WIFI_ON, WIFI_DISABLED, &result);
    return WIFI_DISABLED;
    // }
}

Boolean CWifiService::ShouldWifiBeEnabled()
{
    Boolean isOn;
    if (mAirplaneModeOn->Get(&isOn), isOn) {
        Int32 wifiState;
        mPersistWifiState->Get(&wifiState);
        return wifiState == WIFI_ENABLED_AIRPLANE_OVERRIDE;
    }
    else {
        Int32 wifiState;
        mPersistWifiState->Get(&wifiState);
        return wifiState != WIFI_DISABLED;
    }
}

void CWifiService::HandleWifiToggled(
    /* [in] */ Boolean wifiEnabled)
{
    Boolean isOn;
    mAirplaneModeOn->Get(&isOn);
    Boolean airplaneEnabled = isOn && IsAirplaneToggleable();
    if (wifiEnabled) {
        if (airplaneEnabled) {
            PersistWifiState(WIFI_ENABLED_AIRPLANE_OVERRIDE);
        }
        else {
            PersistWifiState(WIFI_ENABLED);
        }
    }
    else {
        // When wifi state is disabled, we do not care
        // if airplane mode is on or not. The scenario of
        // wifi being disabled due to airplane mode being turned on
        // is handled handleAirplaneModeToggled()
        PersistWifiState(WIFI_DISABLED);
    }
}

void CWifiService::HandleAirplaneModeToggled(
    /* [in] */ Boolean airplaneEnabled)
{
    if (airplaneEnabled) {
        // Wifi disabled due to airplane on
        if (mWifiEnabled) {
            PersistWifiState(WIFI_DISABLED_AIRPLANE_ON);
        }
    }
    else {
        /* On airplane mode disable, restore wifi state if necessary */
        Int32 wifiState;
        if (TestAndClearWifiSavedState() ||
                (mPersistWifiState->Get(&wifiState), wifiState == WIFI_ENABLED_AIRPLANE_OVERRIDE)) {
            PersistWifiState(WIFI_ENABLED);
        }
    }
}

void CWifiService::PersistWifiState(
    /* [in] */ Int32 state)
{
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    mPersistWifiState->Set(state);

    AutoPtr<ISettingsGlobal> global;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
    Boolean result;
    global->PutInt32(cr, ISettingsGlobal::WIFI_ON, state, &result);
}

ECode CWifiService::PingSupplicant(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    FAIL_RETURN(EnforceAccessPermission());
    if (mWifiStateMachineChannel != NULL) {
        *result = mWifiStateMachine->SyncPingSupplicant(mWifiStateMachineChannel);
        return NOERROR;
    }
    else {
        Slogger::E(TAG, "mWifiStateMachineChannel is not initialized");
        *result = FALSE;
        return NOERROR;
    }
}

ECode CWifiService::StartScan(
    /* [in] */ Boolean forceActive)
{
    FAIL_RETURN(EnforceChangePermission());
    mWifiStateMachine->StartScan(forceActive);
    NoteScanStart();
    return NOERROR;
}

ECode CWifiService::EnforceAccessPermission()
{
    return mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::ACCESS_WIFI_STATE, String("WifiService"));
}

ECode CWifiService::EnforceChangePermission()
{
    return mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::CHANGE_WIFI_STATE, String("WifiService"));
}

ECode CWifiService::EnforceMulticastChangePermission()
{
    return mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::CHANGE_WIFI_MULTICAST_STATE, String("WifiService"));
}

ECode CWifiService::EnforceConnectivityInternalPermission()
{
    return mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::CONNECTIVITY_INTERNAL, String("ConnectivityService"));
}

ECode CWifiService::SetWifiEnabled(
    /* [in] */ Boolean enable,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    AutoLock lock(this);
    FAIL_RETURN(EnforceChangePermission());
    Slogger::D(TAG, "SetWifiEnabled: %d pid=%d, uid=%d", enable, Binder::GetCallingPid(),
            Binder::GetCallingUid());
    if (DBG) {
        Slogger::D(TAG, "Invoking mWifiStateMachine.setWifiEnabled %d", enable);
    }

    if (enable) {
        ReportStartWorkSource();
    }
    mWifiStateMachine->SetWifiEnabled(enable);

    /*
     * Caller might not have WRITE_SECURE_SETTINGS,
     * only CHANGE_WIFI_STATE is enforced
     */

    Int64 ident = Binder::ClearCallingIdentity();
    // try {
    HandleWifiToggled(enable);
    // } finally {
    Binder::RestoreCallingIdentity(ident);
    // }

    if (enable) {
        if (!mIsReceiverRegistered) {
            RegisterForBroadcasts();
            mIsReceiverRegistered = TRUE;
        }
    }
    else if (mIsReceiverRegistered) {
        mContext->UnregisterReceiver(mReceiver);
        mIsReceiverRegistered = FALSE;
    }

    *result = TRUE;
    return NOERROR;
}

ECode CWifiService::GetWifiEnabledState(
    /* [our] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    FAIL_RETURN(EnforceAccessPermission());
    *result = mWifiStateMachine->SyncGetWifiState();
    return NOERROR;
}

ECode CWifiService::SetWifiApEnabled(
    /* [in] */ IWifiConfiguration* wifiConfig,
    /* [in] */ Boolean enabled)
{
    FAIL_RETURN(EnforceChangePermission());
    mWifiStateMachine->SetWifiApEnabled(wifiConfig, enabled);
    return NOERROR;
}

ECode CWifiService::GetWifiApEnabledState(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    FAIL_RETURN(EnforceAccessPermission());
    *result = mWifiStateMachine->SyncGetWifiApState();
    return NOERROR;
}

ECode CWifiService::GetWifiApConfiguration(
    /* [out] */ IWifiConfiguration** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    FAIL_RETURN(EnforceAccessPermission());
    AutoPtr<IWifiConfiguration> config = mWifiStateMachine->SyncGetWifiApConfiguration();
    *result = config;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CWifiService::SetWifiApConfiguration(
    /* [in] */ IWifiConfiguration* wifiConfig)
{
    FAIL_RETURN(EnforceChangePermission());
    if (wifiConfig == NULL) return NOERROR;
    mWifiStateMachine->SetWifiApConfiguration(wifiConfig);
    return NOERROR;
}

ECode CWifiService::Disconnect()
{
    FAIL_RETURN(EnforceChangePermission());
    mWifiStateMachine->DisconnectCommand();
    return NOERROR;
}

ECode CWifiService::Reconnect()
{
    FAIL_RETURN(EnforceChangePermission());
    mWifiStateMachine->ReconnectCommand();
    return NOERROR;
}

ECode CWifiService::Reassociate()
{
    FAIL_RETURN(EnforceChangePermission());
    mWifiStateMachine->ReassociateCommand();
    return NOERROR;
}

ECode CWifiService::GetConfiguredNetworks(
    /* [out] */ IObjectContainer** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    FAIL_RETURN(EnforceAccessPermission());
    if (mWifiStateMachineChannel != NULL) {
        AutoPtr<IObjectContainer> networks =
                mWifiStateMachine->SyncGetConfiguredNetworks(mWifiStateMachineChannel);
        *result = networks;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
    else {
        Slogger::E(TAG, "mWifiStateMachineChannel is not initialized");
        return NOERROR;
    }
}

ECode CWifiService::AddOrUpdateNetwork(
    /* [in] */ IWifiConfiguration* config,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = -1;

    FAIL_RETURN(EnforceChangePermission());
    if (mWifiStateMachineChannel != NULL) {
        *result = mWifiStateMachine->SyncAddOrUpdateNetwork(mWifiStateMachineChannel, config);
        return NOERROR;
    }
    else {
        Slogger::E(TAG, "mWifiStateMachineChannel is not initialized");
        *result = -1;
        return NOERROR;
    }
}

ECode CWifiService::RemoveNetwork(
    /* [in] */ Int32 netId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    FAIL_RETURN(EnforceChangePermission());
    if (mWifiStateMachineChannel != NULL) {
        *result = mWifiStateMachine->SyncRemoveNetwork(mWifiStateMachineChannel, netId);
        return NOERROR;
    }
    else {
        Slogger::E(TAG, "mWifiStateMachineChannel is not initialized");
        return NOERROR;
    }
}

ECode CWifiService::EnableNetwork(
    /* [in] */ Int32 netId,
    /* [in] */ Boolean disableOthers,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    FAIL_RETURN(EnforceChangePermission());
    if (mWifiStateMachineChannel != NULL) {
        *result = mWifiStateMachine->SyncEnableNetwork(mWifiStateMachineChannel, netId, disableOthers);
        return NOERROR;
    }
    else {
        Slogger::E(TAG, "mWifiStateMachineChannel is not initialized");
        return NOERROR;
    }
}

ECode CWifiService::DisableNetwork(
    /* [in] */ Int32 netId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    FAIL_RETURN(EnforceChangePermission());
    if (mWifiStateMachineChannel != NULL) {
        *result = mWifiStateMachine->SyncDisableNetwork(mWifiStateMachineChannel, netId);
        return NOERROR;
    }
    else {
        Slogger::E(TAG, "mWifiStateMachineChannel is not initialized");
        return NOERROR;
    }
}

ECode CWifiService::GetConnectionInfo(
    /* [out] */ IWifiInfo** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    FAIL_RETURN(EnforceAccessPermission());
    /*
     * Make sure we have the latest information, by sending
     * a status request to the supplicant.
     */
    AutoPtr<IWifiInfo> winfo = mWifiStateMachine->SyncRequestConnectionInfo();
    *result = winfo;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CWifiService::GetScanResults(
    /* [out] */ IObjectContainer** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    FAIL_RETURN(EnforceAccessPermission());
    Int32 userId = UserHandle::GetCallingUserId();
    Int64 ident = Binder::ClearCallingIdentity();

    AutoPtr<IActivityManagerHelper> helper;
    CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&helper);
    Int32 currentUser;
    helper->GetCurrentUser(&currentUser);
    if (userId != currentUser) {
        Binder::RestoreCallingIdentity(ident);
        return CParcelableObjectContainer::New(result);
    }
    else {
        AutoPtr<ArrayOf<IScanResult*> > results = mWifiStateMachine->SyncGetScanResultsList();
        CParcelableObjectContainer::New(result);
        for (Int32 i = 0; i < results->GetLength(); ++i) {
            (*result)->Add((*results)[i]->Probe(EIID_IInterface));
        }
        Binder::RestoreCallingIdentity(ident);
        return NOERROR;
    }
}

ECode CWifiService::SaveConfiguration(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    FAIL_RETURN(EnforceChangePermission());
    if (mWifiStateMachineChannel != NULL) {
        *result = mWifiStateMachine->SyncSaveConfig(mWifiStateMachineChannel);
        return NOERROR;
    }
    else {
        Slogger::E(TAG, "mWifiStateMachineChannel is not initialized");
        return NOERROR;
    }
}

ECode CWifiService::SetCountryCode(
    /* [in] */ const String& countryCode,
    /* [in] */ Boolean persist)
{
    // Slogger::I(TAG, "WifiService trying to set country code to " + countryCode +
    //         " with persist set to " + persist);
    FAIL_RETURN(EnforceChangePermission());
    mWifiStateMachine->SetCountryCode(countryCode, persist);
    return NOERROR;
}

ECode CWifiService::SetFrequencyBand(
    /* [in] */ Int32 band,
    /* [in] */ Boolean persist)
{
    FAIL_RETURN(EnforceChangePermission());
    Boolean isDualBand;
    if (IsDualBandSupported(&isDualBand), !isDualBand) return NOERROR;
    // Slogger::I(TAG, "WifiService trying to set frequency band to " + band +
    //         " with persist set to " + persist);
    mWifiStateMachine->SetFrequencyBand(band, persist);
    return NOERROR;
}

ECode CWifiService::GetFrequencyBand(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    FAIL_RETURN(EnforceAccessPermission());
    *result = mWifiStateMachine->GetFrequencyBand();
    return NOERROR;
}

ECode CWifiService::IsDualBandSupported(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    //TODO: Should move towards adding a driver API that checks at runtime
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    return res->GetBoolean(R::bool_::config_wifi_dual_band_support, result);
}

ECode CWifiService::GetDhcpInfo(
    /* [out] */ IDhcpInfo** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    FAIL_RETURN(EnforceAccessPermission());
    AutoPtr<IDhcpInfo> dinfo = mWifiStateMachine->SyncGetDhcpInfo();
    *result = dinfo;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CWifiService::StartWifi()
{
    FAIL_RETURN(EnforceConnectivityInternalPermission());
    /* TODO: may be add permissions for access only to connectivity service
     * TODO: if a start issued, keep wifi alive until a stop issued irrespective
     * of WifiLock & device idle status unless wifi enabled status is toggled
     */
    mWifiStateMachine->SetDriverStart(TRUE, mEmergencyCallbackMode);
    mWifiStateMachine->ReconnectCommand();
    return NOERROR;
}

ECode CWifiService::CaptivePortalCheckComplete()
{
    FAIL_RETURN(EnforceConnectivityInternalPermission());
    mWifiStateMachine->CaptivePortalCheckComplete();
    return NOERROR;
}

ECode CWifiService::StopWifi()
{
    FAIL_RETURN(EnforceConnectivityInternalPermission());
    /*
     * TODO: if a stop is issued, wifi is brought up only by startWifi
     * unless wifi enabled status is toggled
     */
    mWifiStateMachine->SetDriverStart(FALSE, mEmergencyCallbackMode);
    return NOERROR;
}

ECode CWifiService::AddToBlacklist(
    /* [in] */ const String& bssid)
{
    FAIL_RETURN(EnforceChangePermission());
    mWifiStateMachine->AddToBlacklist(bssid);
    return NOERROR;
}

ECode CWifiService::ClearBlacklist()
{
    FAIL_RETURN(EnforceChangePermission());
    mWifiStateMachine->ClearBlacklist();
    return NOERROR;
}

ECode CWifiService::GetWifiServiceMessenger(
    /* [out] */ IMessenger** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    FAIL_RETURN(EnforceAccessPermission());
    FAIL_RETURN(EnforceChangePermission());
    return CMessenger::New(mAsyncServiceHandler, result);
}

ECode CWifiService::GetWifiStateMachineMessenger(
    /* [out] */ IMessenger** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    FAIL_RETURN(EnforceAccessPermission());
    FAIL_RETURN(EnforceChangePermission());
    AutoPtr<IMessenger> mgr = mWifiStateMachine->GetMessenger();
    *result = mgr;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CWifiService::GetConfigFile(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = String(NULL);

    FAIL_RETURN(EnforceAccessPermission());
    *result = mWifiStateMachine->GetConfigFile();
    return NOERROR;
}

void CWifiService::SetDeviceIdleAndUpdateWifi(
    /* [in] */ Boolean deviceIdle)
{
    mDeviceIdle = deviceIdle;
    ReportStartWorkSource();
    UpdateWifiState();
}

void CWifiService::ReportStartWorkSource()
{
    mTmpWorkSource->Clear();
    if (mDeviceIdle) {
        List< AutoPtr<WifiLock> >::Iterator it;
        for (it = mLocks->mList.Begin(); it != mLocks->mList.End(); ++it) {
            Boolean result;
            mTmpWorkSource->Add((*it)->mWorkSource, &result);
        }
    }
    mWifiStateMachine->UpdateBatteryWorkSource(mTmpWorkSource);
}

void CWifiService::UpdateWifiState()
{
    Boolean lockHeld = mLocks->HasLocks();
    Int32 strongestLockMode = IWifiManager::WIFI_MODE_FULL;
    Boolean wifiShouldBeStarted;

    if (mEmergencyCallbackMode) {
        wifiShouldBeStarted = FALSE;
    }
    else {
        wifiShouldBeStarted = !mDeviceIdle || lockHeld;
    }

    if (lockHeld) {
        strongestLockMode = mLocks->GetStrongestLockMode();
    }
    /* If device is not idle, lockmode cannot be scan only */
    if (!mDeviceIdle && strongestLockMode == IWifiManager::WIFI_MODE_SCAN_ONLY) {
        strongestLockMode = IWifiManager::WIFI_MODE_FULL;
    }

    /* Disable tethering when airplane mode is enabled */
    Boolean isAirplane;
    if (mAirplaneModeOn->Get(&isAirplane), isAirplane) {
        mWifiStateMachine->SetWifiApEnabled(NULL, FALSE);
    }

    if (ShouldWifiBeEnabled()) {
        if (wifiShouldBeStarted) {
            ReportStartWorkSource();
            mWifiStateMachine->SetWifiEnabled(TRUE);
            mWifiStateMachine->SetScanOnlyMode(
                    strongestLockMode == IWifiManager::WIFI_MODE_SCAN_ONLY);
            mWifiStateMachine->SetDriverStart(TRUE, mEmergencyCallbackMode);
            mWifiStateMachine->SetHighPerfModeEnabled(strongestLockMode
                    == IWifiManager::WIFI_MODE_FULL_HIGH_PERF);
        }
        else {
            mWifiStateMachine->SetDriverStart(FALSE, mEmergencyCallbackMode);
        }
    }
    else {
        mWifiStateMachine->SetWifiEnabled(FALSE);
    }
}

void CWifiService::RegisterForBroadcasts()
{
    AutoPtr<IIntentFilter> intentFilter;
    CIntentFilter::New((IIntentFilter**)&intentFilter);
    intentFilter->AddAction(IIntent::ACTION_SCREEN_ON);
    intentFilter->AddAction(IIntent::ACTION_SCREEN_OFF);
    intentFilter->AddAction(IIntent::ACTION_BATTERY_CHANGED);
    intentFilter->AddAction(ACTION_DEVICE_IDLE);
    intentFilter->AddAction(IBluetoothAdapter::ACTION_CONNECTION_STATE_CHANGED);
    intentFilter->AddAction(ITelephonyIntents::ACTION_EMERGENCY_CALLBACK_MODE_CHANGED);
    AutoPtr<IIntent> intent;
    mContext->RegisterReceiver(mReceiver, intentFilter, (IIntent**)&intent);
}

Boolean CWifiService::IsAirplaneSensitive()
{
    AutoPtr<ISettingsGlobal> global;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    String airplaneModeRadios;
    global->GetString(resolver, ISettingsGlobal::AIRPLANE_MODE_RADIOS, &airplaneModeRadios);
    return airplaneModeRadios.IsNull() || airplaneModeRadios.Contains(ISettingsGlobal::RADIO_WIFI);
}

Boolean CWifiService::IsAirplaneToggleable()
{
    AutoPtr<ISettingsGlobal> global;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    String toggleableRadios;
    global->GetString(resolver, ISettingsGlobal::AIRPLANE_MODE_TOGGLEABLE_RADIOS, &toggleableRadios);
    return !toggleableRadios.IsNull() && toggleableRadios.Contains(ISettingsGlobal::RADIO_WIFI);
}

Boolean CWifiService::IsAirplaneModeOn()
{
    AutoPtr<ISettingsGlobal> global;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    Int32 value;
    global->GetInt32(resolver, ISettingsGlobal::AIRPLANE_MODE_ON, 0, &value);
    return IsAirplaneSensitive() && value == 1;
}

void CWifiService::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    assert(0);
//    if (mContext->CheckCallingOrSelfPermission(Elastos::Droid::Manifest::permission::DUMP) != PackageManager::PERMISSION_GRANTED) {
//        pw->Println("Permission Denial: can't dump WifiService from from pid=" + Binder->GetCallingPid() + ", uid=" + Binder->GetCallingUid());
//        return;
//    }
//    pw->Println("Wi-Fi is " + mWifiStateMachine->SyncGetWifiStateByName());
//    pw->Println("Stay-awake conditions: " + Settings::Global->GetInt32(mContext->GetContentResolver(),Settings::Global::STAY_ON_WHILE_PLUGGED_IN, 0));
//    pw->Println();
//
//    pw->Println("Internal state:");
//    pw->Println(mWifiStateMachine);
//    pw->Println();
//    pw->Println("Latest scan results:");
//    List<ScanResult> scanResults = mWifiStateMachine->SyncGetScanResultsList();
//    if (scanResults != NULL && scanResults->Size() != 0) {
//        pw->Println("  BSSID              Frequency   RSSI  Flags             SSID");
//        for (ScanResult r : scanResults) {
//            pw->Printf("  %17s  %9d  %5d  %-16s  %s%n",
//                                     r.BSSID,
//                                     r.frequency,
//                                     r.level,
//                                     r.capabilities,
//                                     r.SSID == NULL ? "" : r.SSID);
//        }
//    }
//    pw->Println();
//    pw->Println("Locks acquired: " + mFullLocksAcquired + " full, " + mFullHighPerfLocksAcquired + " full high perf, " + mScanLocksAcquired + " scan");
//    pw->Println("Locks released: " + mFullLocksReleased + " full, " + mFullHighPerfLocksReleased + " full high perf, " + mScanLocksReleased + " scan");
//    pw->Println();
//    pw->Println("Locks held:");
//    mLocks->Dump(pw);
//
//    pw->Println();
//    pw->Println("WifiWatchdogStateMachine dump");
//    mWifiWatchdogStateMachine->Dump(pw);
//    pw->Println("WifiStateMachine dump");
//    mWifiStateMachine->Dump(fd, pw, args);
}

ECode CWifiService::EnforceWakeSourcePermission(
    /* [in] */ Int32 uid,
    /* [in] */ Int32 pid)
{
    if (uid == Process::MyUid()) {
        return NOERROR;
    }
    return mContext->EnforcePermission(Elastos::Droid::Manifest::permission::UPDATE_DEVICE_STATS,
            pid, uid, String(NULL));
}

ECode CWifiService::AcquireWifiLock(
    /* [in] */ IBinder* binder,
    /* [in] */ Int32 lockMode,
    /* [in] */ const String& tag,
    /* [in] */ IWorkSource* inWs,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::WAKE_LOCK, String(NULL)));
    if (lockMode != IWifiManager::WIFI_MODE_FULL &&
            lockMode != IWifiManager::WIFI_MODE_SCAN_ONLY &&
            lockMode != IWifiManager::WIFI_MODE_FULL_HIGH_PERF) {
        Slogger::E(TAG, "Illegal argument, lockMode= %d", lockMode);
        if (DBG) {
            // throw new IllegalArgumentException("lockMode=" + lockMode);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        *result = FALSE;
        return NOERROR;
    }

    AutoPtr<IWorkSource> ws = inWs;
    Int32 size;
    if (ws != NULL && (ws->GetSize(&size), size == 0)) {
        ws = NULL;
    }
    if (ws != NULL) {
        FAIL_RETURN(EnforceWakeSourcePermission(Binder::GetCallingUid(), Binder::GetCallingPid()));
    }
    if (ws == NULL) {
        CWorkSource::New(Binder::GetCallingUid(), ws);
    }
    AutoPtr<WifiLock> wifiLock = new WifiLock(lockMode, tag, binder, ws, this);
    {
        AutoLock lock(mLocksLock);
        *result = AcquireWifiLockLocked(wifiLock);
    }
    return NOERROR;
}

void CWifiService::NoteAcquireWifiLock(
    /* [in] */ WifiLock* wifiLock)
{
    switch(wifiLock->mMode) {
        case IWifiManager::WIFI_MODE_FULL:
        case IWifiManager::WIFI_MODE_FULL_HIGH_PERF:
        case IWifiManager::WIFI_MODE_SCAN_ONLY:
            mBatteryStats->NoteFullWifiLockAcquiredFromSource(wifiLock->mWorkSource);
            break;
    }
}

void CWifiService::NoteReleaseWifiLock(
    /* [in] */ WifiLock* wifiLock)
{
    switch(wifiLock->mMode) {
        case IWifiManager::WIFI_MODE_FULL:
        case IWifiManager::WIFI_MODE_FULL_HIGH_PERF:
        case IWifiManager::WIFI_MODE_SCAN_ONLY:
            mBatteryStats->NoteFullWifiLockReleasedFromSource(wifiLock->mWorkSource);
            break;
    }
}

Boolean CWifiService::AcquireWifiLockLocked(
    /* [in] */ WifiLock* wifiLock)
{
    if (DBG) Slogger::D(TAG, "acquireWifiLockLocked: %p", wifiLock);

    mLocks->AddLock(wifiLock);

    Int64 ident = Binder::ClearCallingIdentity();
    // try {
    NoteAcquireWifiLock(wifiLock);
    switch(wifiLock->mMode) {
    case IWifiManager::WIFI_MODE_FULL:
        ++mFullLocksAcquired;
        break;
    case IWifiManager::WIFI_MODE_FULL_HIGH_PERF:
        ++mFullHighPerfLocksAcquired;
        break;

    case IWifiManager::WIFI_MODE_SCAN_ONLY:
        ++mScanLocksAcquired;
        break;
    }

    // Be aggressive about adding new locks into the accounted state...
    // we want to over-report rather than under-report.
    ReportStartWorkSource();

    UpdateWifiState();
    // } catch (RemoteException e) {
        // return false;
    // } finally {
    Binder::RestoreCallingIdentity(ident);
    // }
    return TRUE;
}

ECode CWifiService::UpdateWifiLockWorkSource(
    /* [in] */ IBinder* lock,
    /* [in] */ IWorkSource* ws)
{
    Int32 uid = Binder::GetCallingUid();
    Int32 pid = Binder::GetCallingPid();
    Int32 size;
    if (ws != NULL && (ws->GetSize(&size), size == 0)) {
        ws = NULL;
    }
    if (ws != NULL) {
        FAIL_RETURN(EnforceWakeSourcePermission(uid, pid));
    }
    Int64 ident = Binder::ClearCallingIdentity();
    // try {
    {
        AutoLock l(mLocksLock);

        List< AutoPtr<WifiLock> >::Iterator it = mLocks->FindLockByBinder(lock);
        if (it == mLocks->mList.End()) {
            // throw new IllegalArgumentException("Wifi lock not active");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        AutoPtr<WifiLock> wl = *it;
        NoteReleaseWifiLock(wl);
        AutoPtr<IWorkSource> newWs;
        if (ws != NULL) {
            CWorkSource::New(ws, (IWorkSource**)&newWs);
        }
        else {
            CWorkSource::New(uid, (IWorkSource**)&newWs);
        }
        wl->mWorkSource = newWs;
        NoteAcquireWifiLock(wl);
    }
    // } catch (RemoteException e) {
    // } finally {
    Binder::RestoreCallingIdentity(ident);
    // }
    return NOERROR;
}

ECode CWifiService::ReleaseWifiLock(
    /* [in] */ IBinder* lock,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::WAKE_LOCK, String(NULL)));
    {
        AutoLock l(mLocksLock);
        *result = ReleaseWifiLockLocked(lock);
        return NOERROR;
    }
}

Boolean CWifiService::ReleaseWifiLockLocked(
    /* [in] */ IBinder* lock)
{
    Boolean hadLock;

    AutoPtr<WifiLock> wifiLock = mLocks->RemoveLock(lock);

    if (DBG) Slogger::D(TAG, "releaseWifiLockLocked: %p", wifiLock.Get());

    hadLock = (wifiLock != NULL);

    Int64 ident = Binder::ClearCallingIdentity();
    // try {
    if (hadLock) {
        NoteReleaseWifiLock(wifiLock);
        switch(wifiLock->mMode) {
            case IWifiManager::WIFI_MODE_FULL:
                ++mFullLocksReleased;
                break;
            case IWifiManager::WIFI_MODE_FULL_HIGH_PERF:
                ++mFullHighPerfLocksReleased;
                break;
            case IWifiManager::WIFI_MODE_SCAN_ONLY:
                ++mScanLocksReleased;
                break;
        }
    }

    // TODO - should this only happen if you hadLock?
    UpdateWifiState();

    // } catch (RemoteException e) {
    // } finally {
    Binder::RestoreCallingIdentity(ident);
    // }

    return hadLock;
}

ECode CWifiService::InitializeMulticastFiltering()
{
    FAIL_RETURN(EnforceMulticastChangePermission());

    {
        AutoLock lock(mMulticastersLock);
        // if anybody had requested filters be off, leave off
        if (mMulticasters.IsEmpty() == FALSE) {
            return NOERROR;
        }
        else {
            mWifiStateMachine->StartFilteringMulticastV4Packets();
            return NOERROR;
        }
    }
}

ECode CWifiService::AcquireMulticastLock(
    /* [in] */ IBinder* binder,
    /* [in] */ const String& tag)
{
    FAIL_RETURN(EnforceMulticastChangePermission());

    {
        AutoLock lock(mMulticastersLock);

        mMulticastEnabled++;
        mMulticasters.PushBack(new Multicaster(tag, binder, this));
        // Note that we could call stopFilteringMulticastV4Packets only when
        // our new size == 1 (first call), but this function won't
        // be called often and by making the stopPacket call each
        // time we're less fragile and self-healing.
        mWifiStateMachine->StopFilteringMulticastV4Packets();
    }

    Int32 uid = Binder::GetCallingUid();
    Int64 ident = Binder::ClearCallingIdentity();
    // try {
    mBatteryStats->NoteWifiMulticastEnabled(uid);
    // } catch (RemoteException e) {
    // } finally {
    Binder::RestoreCallingIdentity(ident);
    // }
    return NOERROR;
}

ECode CWifiService::ReleaseMulticastLock()
{
    FAIL_RETURN(EnforceMulticastChangePermission());

    Int32 uid = Binder::GetCallingUid();
    {
        Mutex::Autolock lock(mMulticastersLock);

        mMulticastDisabled++;
        List< AutoPtr<Multicaster> >::ReverseIterator rit;
        for (rit = mMulticasters.RBegin(); rit != mMulticasters.REnd();) {
            AutoPtr<Multicaster> m = *rit;
            if ((m != NULL) && (m->GetUid() == uid)) {
                List<AutoPtr<Multicaster> >::ReverseIterator tmpRit(rit);
                RemoveMulticasterLocked((++tmpRit).GetBase(), uid);
            }
            else
                ++rit;
        }
    }
    return NOERROR;
}

void CWifiService::RemoveMulticasterLocked(
    /* [in] */ List< AutoPtr<Multicaster> >::Iterator it,
    /* [in] */ Int32 uid)
{
    AutoPtr<Multicaster> removed = *it;
    mMulticasters.Erase(it);

    if (removed != NULL) {
        removed->UnlinkDeathRecipient();
    }
    if (mMulticasters.IsEmpty()) {
        mWifiStateMachine->StartFilteringMulticastV4Packets();
    }

    Int64 ident = Binder::ClearCallingIdentity();
    // try {
    mBatteryStats->NoteWifiMulticastDisabled(uid);
    // } catch (RemoteException e) {
    // } finally {
    Binder::RestoreCallingIdentity(ident);
    // }
}

ECode CWifiService::IsMulticastEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    FAIL_RETURN(EnforceAccessPermission());

    {
        AutoLock lock(mMulticastersLock);
        *result =  mMulticasters.IsEmpty() == FALSE;
    }
    return NOERROR;
}

void CWifiService::EvaluateTrafficStatsPolling()
{
    AutoPtr<IMessage> message;

    NetworkInfoDetailedState state;
    mNetworkInfo->GetDetailedState(&state);
    if (state == Elastos::Droid::Net::NetworkInfoState_CONNECTED && !mScreenOff) {
        mAsyncServiceHandler->ObtainMessage(IWifiManager::ENABLE_TRAFFIC_STATS_POLL,
            1, 0, (IMessage**)&message);
    }
    else {
        mAsyncServiceHandler->ObtainMessage(IWifiManager::ENABLE_TRAFFIC_STATS_POLL,
            0, 0, (IMessage**)&message);
    }

    Boolean bval;
    mAsyncServiceHandler->SendMessage(message, &bval);
}

void CWifiService::NotifyOnDataActivity()
{
    Int64 sent, received;
    Int64 preTxPkts = mTxPkts, preRxPkts = mRxPkts;
    Int32 dataActivity = IWifiManager::DATA_ACTIVITY_NONE;

    AutoPtr<ITrafficStats> ts;
    CTrafficStats::AcquireSingleton((ITrafficStats**)&ts);
    ts->GetTxPackets(mInterfaceName, &mTxPkts);
    ts->GetRxPackets(mInterfaceName, &mRxPkts);

    if (preTxPkts > 0 || preRxPkts > 0) {
        sent = mTxPkts - preTxPkts;
        received = mRxPkts - preRxPkts;
        if (sent > 0) {
            dataActivity |= IWifiManager::DATA_ACTIVITY_OUT;
        }
        if (received > 0) {
            dataActivity |= IWifiManager::DATA_ACTIVITY_IN;
        }

        if (dataActivity != mDataActivity && !mScreenOff) {
            mDataActivity = dataActivity;
            List< AutoPtr<AsyncChannel> >::Iterator it;
            for (it = mClients.Begin(); it != mClients.End(); ++it) {
                (*it)->SendMessage(IWifiManager::DATA_ACTIVITY_NOTIFICATION, mDataActivity);
            }
        }
    }
}

void CWifiService::CheckAndSetNotification()
{
    // If we shouldn't place a notification on available networks, then
    // don't bother doing any of the following
    if (!mNotificationEnabled) return;

    NetworkInfoState state;
    mNetworkInfo->GetState(&state);
    if ((state == Elastos::Droid::Net::NetworkInfoState_DISCONNECTED)
       || (state == Elastos::Droid::Net::NetworkInfoState_UNKNOWN)) {
        // Look for an open network
        AutoPtr<ArrayOf<IScanResult*> > scanResults = mWifiStateMachine->SyncGetScanResultsList();
        if (scanResults != NULL) {
            Int32 numOpenNetworks = 0;
            String capabilities;
            for (Int32 i = scanResults->GetLength() - 1; i >= 0; i--) {
                AutoPtr<IScanResult> scanResult = (*scanResults)[i];
                scanResult->GetCapabilities(&capabilities);
                //A capability of [ESS] represents an open access point
                //that is available for an STA to connect
                if (!capabilities.IsNull() && capabilities.Equals("[ESS]")) {
                    numOpenNetworks++;
                }
            }

            if (numOpenNetworks > 0) {
                if (++mNumScansSinceNetworkStateChange >= NUM_SCANS_BEFORE_ACTUALLY_SCANNING) {
                    /*
                     * We've scanned continuously at least
                     * NUM_SCANS_BEFORE_NOTIFICATION times. The user
                     * probably does not have a remembered network in range,
                     * since otherwise supplicant would have tried to
                     * associate and thus resetting this counter.
                     */
                    SetNotificationVisible(TRUE, numOpenNetworks, FALSE, 0);
                }
                return;
            }
        }
    }

    // No open networks in range, remove the notification
    SetNotificationVisible(FALSE, 0, FALSE, 0);
}

void CWifiService::ResetNotification()
{
    mNotificationRepeatTime = 0;
    mNumScansSinceNetworkStateChange = 0;
    SetNotificationVisible(FALSE, 0, FALSE, 0);
}

void CWifiService::SetNotificationVisible(
    /* [in] */ Boolean visible,
    /* [in] */ Int32 numNetworks,
    /* [in] */ Boolean force,
    /* [in] */ Int32 delay)
{
    // Since we use auto cancel on the notification, when the
    // mNetworksAvailableNotificationShown is true, the notification may
    // have actually been canceled.  However, when it is false we know
    // for sure that it is not being shown (it will not be shown any other
    // place than here)

    // If it should be hidden and it is already hidden, then noop
    if (!visible && !mNotificationShown && !force) {
        return;
    }

    AutoPtr<IInterface> tempObj;
    mContext->GetSystemService(IContext::NOTIFICATION_SERVICE, (IInterface**)&tempObj);
    AutoPtr<INotificationManager> notificationManager = INotificationManager::Probe(tempObj.Get());

    AutoPtr<IUserHandleHelper> handleHelper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&handleHelper);

    AutoPtr<IMessage> message;
    if (visible) {
        // Not enough time has passed to show the notification again
        AutoPtr<ISystem> system;
        Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
        Int64 now;
        system->GetCurrentTimeMillis(&now);
        if (now < mNotificationRepeatTime) {
            return;
        }

        if (mNotification == NULL) {
            // Cache the Notification object.
            CNotification::New((INotification**)&mNotification);

            mNotification->SetWhen(0);
            mNotification->SetIcon(ICON_NETWORKS_AVAILABLE);
            mNotification->SetFlags(INotification::FLAG_AUTO_CANCEL);

            AutoPtr<ITaskStackBuilderHelper> helper;
            CTaskStackBuilderHelper::AcquireSingleton((ITaskStackBuilderHelper**)&helper);
            AutoPtr<ITaskStackBuilder> builder;
            helper->Create(mContext, (ITaskStackBuilder**)&builder);
            AutoPtr<IIntent> intent;
            CIntent::New(IWifiManager::ACTION_PICK_WIFI_NETWORK, (IIntent**)&intent);
            builder->AddNextIntentWithParentStack(intent);
            AutoPtr<IUserHandle> current;
            handleHelper->GetCURRENT((IUserHandle**)&current);
            AutoPtr<IPendingIntent> pendingIntent;
            builder->GetPendingIntent(0, 0, NULL, current, (IPendingIntent**)&pendingIntent);
        }

        AutoPtr<IResources> res;
        mContext->GetResources((IResources**)&res);
        AutoPtr<ICharSequence> title, details;
        res->GetQuantityText(R::plurals::wifi_available, numNetworks, (ICharSequence**)&title);
        res->GetQuantityText(R::plurals::wifi_available_detailed, numNetworks, (ICharSequence**)&details);
        AutoPtr<IPendingIntent> contentIntent;
        mNotification->GetContentIntent((IPendingIntent**)&contentIntent);
        mNotification->SetTickerText(title);
        mNotification->SetLatestEventInfo(mContext, title, details, contentIntent);

        Int64 millis;
        system->GetCurrentTimeMillis(&millis);
        mNotificationRepeatTime = millis + NOTIFICATION_REPEAT_DELAY_MS;

        AutoPtr<IUserHandle> all;
        handleHelper->GetALL((IUserHandle**)&all);
        notificationManager->NotifyAsUser(String(NULL), ICON_NETWORKS_AVAILABLE, mNotification, all);
    }
    else {
        AutoPtr<IUserHandle> all;
        handleHelper->GetALL((IUserHandle**)&all);
        notificationManager->CancelAsUser(String(NULL), ICON_NETWORKS_AVAILABLE, all);
    }

    mNotificationShown = visible;
}

} // namespace Server
} // namespace Droid
} // namespace Elastos
