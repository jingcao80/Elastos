#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Net.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Bluetooth.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Wifi.h"
#include "elastos/droid/R.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/provider/Settings.h"
#include "elastos/droid/Manifest.h"
#include "elastos/droid/server/wifi/WifiServiceImpl.h"
#include "elastos/droid/server/am/BatteryStatsService.h"
#include "elastos/core/StringUtils.h"
#include "elastos/core/CoreUtils.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::App::IActivityManagerHelper;
using Elastos::Droid::App::CActivityManagerHelper;
using Elastos::Droid::Bluetooth::IBluetoothAdapter;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Internal::Telephony::ITelephonyIntents;
using Elastos::Droid::Internal::Utility::CAsyncChannel;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::CWorkSource;
using Elastos::Droid::Os::IBinderHelper;
using Elastos::Droid::Os::CBinderHelper;
using Elastos::Droid::Os::IMessageHelper;
using Elastos::Droid::Os::CMessageHelper;
using Elastos::Droid::Os::CMessenger;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::CHandlerThread;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Os::IUserManagerHelper;
using Elastos::Droid::Os::CUserManagerHelper;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Provider::Settings;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Net::CDhcpInfo;
using Elastos::Droid::Net::ILinkAddress;
using Elastos::Droid::Net::INetworkUtils;
using Elastos::Droid::Net::CNetworkUtils;
using Elastos::Droid::Net::IStaticIpConfiguration;
using Elastos::Droid::Net::IConnectivityManagerHelper;
using Elastos::Droid::Net::CConnectivityManagerHelper;
using Elastos::Droid::Manifest;
using Elastos::Droid::Server::Am::BatteryStatsService;
using Elastos::Droid::R;
using Elastos::Droid::Wifi::CScanSettings;
using Elastos::Droid::Wifi::EIID_IIWifiManager;
using Elastos::Droid::Wifi::IWifiManager;
using Elastos::Droid::Wifi::CBatchedScanSettings;
using Elastos::Droid::Wifi::CWifiActivityEnergyInfo;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;
using Elastos::IO::ICloseable;
using Elastos::IO::IReader;
using Elastos::IO::IFileReader;
using Elastos::IO::CFileReader;
using Elastos::IO::IBufferedReader;
using Elastos::IO::CBufferedReader;
using Elastos::Net::IInetAddress;
using Elastos::Net::IInet4Address;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wifi {

//=====================================================================
//                      WifiServiceImpl::TdlsTask
//=====================================================================
ECode WifiServiceImpl::TdlsTask::DoInBackground(
    /* [in] */ ArrayOf<IInterface*>* params,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    // Retrieve parameters for the call
    AutoPtr<TdlsTaskParams> param = (TdlsTaskParams*)(IObject::Probe((*params)[0]));
    String remoteIpAddress = param->remoteIpAddress.Trim();
    Boolean enable = param->enable;

    // Get MAC address of Remote IP
    String macAddress;

    AutoPtr<IBufferedReader> reader;

    //try {
    AutoPtr<IFileReader> fileReader;
    ECode ec = CFileReader::New(String("/proc/net/arp"), (IFileReader**)&fileReader);
    if (FAILED(ec)) {
        Slogger::E(WifiServiceImpl::TAG, "Could not open /proc/net/arp to lookup mac address");
        AutoPtr<IInteger32> res = CoreUtils::Convert(0);
        *result = TO_IINTERFACE(res);
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
    ec = CBufferedReader::New(IReader::Probe(fileReader), (IBufferedReader**)&reader);
    if (FAILED(ec)) {
        Slogger::E(WifiServiceImpl::TAG, "Could not open /proc/net/arp to lookup mac address");
        AutoPtr<IInteger32> res = CoreUtils::Convert(0);
        *result = TO_IINTERFACE(res);
        REFCOUNT_ADD(*result);
        return NOERROR;
    }

    // Skip over the line bearing colum titles
    String line;
    reader->ReadLine(&line);

    while ((reader->ReadLine(&line), !line.IsNull())) {
        AutoPtr<ArrayOf<String> > tokens;
        StringUtils::Split(line, String("[ ]+"), (ArrayOf<String>**)&tokens);
        if (tokens->GetLength() < 6) {
            continue;
        }

        // ARP column format is
        // Address HWType HWAddress Flags Mask IFace
        String ip = (*tokens)[0];
        String mac = (*tokens)[3];

        if (remoteIpAddress.Equals(ip)) {
            macAddress = mac;
            break;
        }
    }

    if (macAddress.IsNull()) {
        Slogger::W(WifiServiceImpl::TAG, "Did not find remoteAddress {%s} in /proc/net/arp", remoteIpAddress.string());
    } else {
        mOwner->EnableTdlsWithMacAddress(macAddress, enable);
    }

    //} catch (FileNotFoundException e) {
    //    Slog.e(TAG, "Could not open /proc/net/arp to lookup mac address");
    //} catch (IOException e) {
    //    Slog.e(TAG, "Could not read /proc/net/arp to lookup mac address");
    //} finally {
    //    try {
    if (reader != NULL) {
        ICloseable::Probe(reader)->Close();
    }
    //    }
    //    catch (IOException e) {
    //        // Do nothing
    //    }
    //}

    AutoPtr<IInteger32> res = CoreUtils::Convert(0);
    *result = TO_IINTERFACE(res);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

//=====================================================================
//                      WifiServiceImpl::LockList
//=====================================================================
// synchronized
ECode WifiServiceImpl::LockList::HasLocks(
    /* [out] */ Boolean* result)
{
    AutoLock lock(this);
    VALIDATE_NOT_NULL(result);
    Boolean isEmpty;
    mList->IsEmpty(&isEmpty);
    *result = !isEmpty;
    return NOERROR;
}

// synchronized
ECode WifiServiceImpl::LockList::GetStrongestLockMode(
    /* [out] */ Int32* result)
{
    AutoLock lock(this);
    VALIDATE_NOT_NULL(result);
    Boolean isEmpty;
    if (mList->IsEmpty(&isEmpty), isEmpty) {
        *result = IWifiManager::WIFI_MODE_FULL;
        return NOERROR;
    }

    if (mOwner->mFullHighPerfLocksAcquired > mOwner->mFullHighPerfLocksReleased) {
        *result = IWifiManager::WIFI_MODE_FULL_HIGH_PERF;
        return NOERROR;
    }

    if (mOwner->mFullLocksAcquired > mOwner->mFullLocksReleased) {
        *result = IWifiManager::WIFI_MODE_FULL;
        return NOERROR;
    }

    *result = IWifiManager::WIFI_MODE_SCAN_ONLY;
    return NOERROR;
}

// synchronized
ECode WifiServiceImpl::LockList::UpdateWorkSource(
    /* [in] */ IWorkSource* ws)
{
    AutoLock lock(this);
    VALIDATE_NOT_NULL(ws);
    Int32 size;
    mOwner->mLocks->mList->GetSize(&size);
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> obj;
        mOwner->mLocks->mList->Get(i, (IInterface**)&obj);
        WifiLock* wifiLock = (WifiLock*)(IObject::Probe(obj));
        Boolean added;
        ws->Add(wifiLock->mWorkSource, &added);
    }
    return NOERROR;
}

WifiServiceImpl::LockList::LockList(
    /* [in] */ WifiServiceImpl* owner)
    : mOwner(owner)
{
    CArrayList::New((IList**)&mList);
}

void WifiServiceImpl::LockList::AddLock(
    /* [in] */ WifiLock* lock)
{
    if (FindLockByBinder(lock->mBinder) < 0) {
         mList->Add(TO_IINTERFACE(lock));
     }
}

AutoPtr<WifiServiceImpl::WifiLock> WifiServiceImpl::LockList::RemoveLock(
    /* [in] */ IBinder* binder)
{
    Int32 index = FindLockByBinder(binder);
    if (index >= 0) {
        AutoPtr<IInterface> obj;
        mList->Remove(index, (IInterface**)&obj);
        AutoPtr<WifiLock> ret = (WifiLock*)(IObject::Probe(obj));
        ret->UnlinkDeathRecipient();
        return ret;
    } else {
        return NULL;
    }
    return NULL;
}

Int32 WifiServiceImpl::LockList::FindLockByBinder(
    /* [in] */ IBinder* binder)
{
    Int32 size;
    mList->GetSize(&size);
    for (Int32 i = size - 1; i >= 0; --i) {
        AutoPtr<IInterface> obj;
        mList->Get(i, (IInterface**)&obj);
        AutoPtr<WifiLock> ret = (WifiLock*)(IObject::Probe(obj));
        if (ret->mBinder.Get() == binder)
            return i;
    }
    return -1;
}

void WifiServiceImpl::LockList::Dump(
    /* [in] */ IPrintWriter* pw)
{
    Int32 size;
    mList->GetSize(&size);
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> obj;
        mList->Get(i, (IInterface**)&obj);
        AutoPtr<WifiLock> ret = (WifiLock*)(IObject::Probe(obj));
        pw->Print(String("    "));
        String str;
        ret->ToString(&str);
        pw->Println(str);
    }
}

//=====================================================================
//                    WifiServiceImpl::ClientHandler
//=====================================================================
WifiServiceImpl::ClientHandler::ClientHandler(
    /* [in] */ WifiServiceImpl* owner,
    /* [in] */ ILooper* looper)
    : Handler(looper)
    , mOwner(owner)
{
}

ECode WifiServiceImpl::ClientHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case IAsyncChannel::CMD_CHANNEL_HALF_CONNECTED: {
            Int32 arg1;
            msg->GetArg1(&arg1);
            if (arg1 == IAsyncChannel::STATUS_SUCCESSFUL) {
                if (WifiServiceImpl::DBG) Slogger::D(WifiServiceImpl::TAG, "New client listening to asynchronous messages");
                // We track the clients by the Messenger
                // since it is expected to be always available
                AutoPtr<IMessenger> replyTo;
                msg->GetReplyTo((IMessenger**)&replyTo);
                mOwner->mTrafficPoller->AddClient(replyTo);
            } else {
                Slogger::E(TAG, "Client connection failure, error=%d", arg1);
            }
            break;
        }
        case IAsyncChannel::CMD_CHANNEL_DISCONNECTED: {
            Int32 arg1;
            msg->GetArg1(&arg1);
            if (arg1 == IAsyncChannel::STATUS_SEND_UNSUCCESSFUL) {
                if (WifiServiceImpl::DBG) Slogger::D(WifiServiceImpl::TAG, "Send failed, client connection lost");
            } else {
                if (WifiServiceImpl::DBG) Slogger::D(WifiServiceImpl::TAG, "Client connection lost with reason: %d", arg1);
            }
            AutoPtr<IMessenger> replyTo;
            msg->GetReplyTo((IMessenger**)&replyTo);
            mOwner->mTrafficPoller->RemoveClient(replyTo);
            break;
        }
        case IAsyncChannel::CMD_CHANNEL_FULL_CONNECTION: {
            AutoPtr<IAsyncChannel> ac;
            CAsyncChannel::New((IAsyncChannel**)&ac);
            AutoPtr<IMessenger> replyTo;
            msg->GetReplyTo((IMessenger**)&replyTo);
            ac->Connect(mOwner->mContext, this, replyTo);
            break;
        }
        /* Client commands are forwarded to state machine */
        case IWifiManager::CONNECT_NETWORK:
        case IWifiManager::SAVE_NETWORK: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            AutoPtr<IWifiConfiguration> config = IWifiConfiguration::Probe(obj);
            Int32 networkId;
            msg->GetArg1(&networkId);
            if (what == IWifiManager::SAVE_NETWORK) {
                if (config != NULL) {
                    Int32 confignwId;
                    config->GetNetworkId(&confignwId);
                    AutoPtr<IBinderHelper> binderHelper;
                    CBinderHelper::AcquireSingleton((IBinderHelper**)&binderHelper);
                    Int32 uid;
                    binderHelper->GetCallingUid(&uid);
                    if (confignwId == IWifiConfiguration::INVALID_NETWORK_ID) {
                        config->SetCreatorUid(uid);
                    } else {
                        config->SetLastUpdateUid(uid);
                    }
                }
                Slogger::E("WiFiServiceImpl ", "SAVE nid=%d", networkId);
                        //+ " uid=" + Integer.toString(config.creatorUid)
                        //+ "/" + Integer.toString(config.lastUpdateUid);
            }
            if (what == IWifiManager::CONNECT_NETWORK) {
                if (config != NULL) {
                    AutoPtr<IBinderHelper> binderHelper;
                    CBinderHelper::AcquireSingleton((IBinderHelper**)&binderHelper);
                    Int32 uid;
                    binderHelper->GetCallingUid(&uid);
                    Int32 confignwId;
                    config->GetNetworkId(&confignwId);
                    if (confignwId == IWifiConfiguration::INVALID_NETWORK_ID) {
                        config->SetCreatorUid(uid);
                    } else {
                        config->SetLastUpdateUid(uid);
                    }
                }
                Slogger::E("WiFiServiceImpl ", "CONNECT  nid=%d", networkId);
                        //+ " uid=" + Binder.getCallingUid();
            }
            Boolean isValid;
            if (config != NULL && (config->IsValid(&isValid), isValid)) {
                if (WifiServiceImpl::DBG) Slogger::D(WifiServiceImpl::TAG, "Connect with config");// + config);
                AutoPtr<IMessage> message;
                AutoPtr<IMessageHelper> helper;
                CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
                helper->Obtain(msg, (IMessage**)&message);
                mOwner->mWifiStateMachine->SendMessage(message);
            } else if (config == NULL
                    && networkId != IWifiConfiguration::INVALID_NETWORK_ID) {
                if (WifiServiceImpl::DBG) Slogger::D(TAG, "Connect with networkId %d", networkId);
                AutoPtr<IMessage> message;
                AutoPtr<IMessageHelper> helper;
                CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
                helper->Obtain(msg, (IMessage**)&message);
                mOwner->mWifiStateMachine->SendMessage(message);
            } else {
                Slogger::E(WifiServiceImpl::TAG, "ClientHandler.handleMessage ignoring invalid msg=");// + msg);
                if (what == IWifiManager::CONNECT_NETWORK) {
                    ReplyFailed(msg, IWifiManager::CONNECT_NETWORK_FAILED, IWifiManager::INVALID_ARGS);
                } else {
                    ReplyFailed(msg, IWifiManager::SAVE_NETWORK_FAILED, IWifiManager::INVALID_ARGS);
                }
            }
            break;
        }
        case IWifiManager::FORGET_NETWORK:
            Int32 sendingUid;
            msg->GetSendingUid(&sendingUid);
            if (mOwner->IsOwner(sendingUid)) {
                AutoPtr<IMessage> message;
                AutoPtr<IMessageHelper> helper;
                CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
                helper->Obtain(msg, (IMessage**)&message);
                mOwner->mWifiStateMachine->SendMessage(message);
            } else {
                Slogger::E(TAG, "Forget is not authorized for user");
                ReplyFailed(msg, IWifiManager::FORGET_NETWORK_FAILED, IWifiManager::NOT_AUTHORIZED);
            }
            break;
        case IWifiManager::START_WPS:
        case IWifiManager::CANCEL_WPS:
        case IWifiManager::DISABLE_NETWORK:
        case IWifiManager::RSSI_PKTCNT_FETCH: {
            AutoPtr<IMessage> message;
            AutoPtr<IMessageHelper> helper;
            CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
            helper->Obtain(msg, (IMessage**)&message);
            mOwner->mWifiStateMachine->SendMessage(message);
            break;
        }
        default: {
            Slogger::D(TAG, "ClientHandler.handleMessage ignoring msg=");// + msg);
            break;
        }
    }
    return NOERROR;
}

void WifiServiceImpl::ClientHandler::ReplyFailed(
    /* [in] */ IMessage* msg,
    /* [in] */ Int32 what,
    /* [in] */ Int32 why)
{
    AutoPtr<IMessage> reply;
    AutoPtr<IMessageHelper> helper;
    CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
    helper->Obtain((IMessage**)&reply);
    reply->SetWhat(what);
    reply->SetArg1(why);
    //try {
    AutoPtr<IMessenger> messenger;
    msg->GetReplyTo((IMessenger**)&messenger);
    messenger->Send(reply);
    //} catch (RemoteException e) {
    //    // There's not much we can do if reply can't be sent!
    //}
}

//=====================================================================
//               WifiServiceImpl::WifiStateMachineHandler
//=====================================================================
WifiServiceImpl::WifiStateMachineHandler::WifiStateMachineHandler(
    /* [in] */ WifiServiceImpl* owner,
    /* [in] */ ILooper* looper)
    : Handler(looper)
    , mOwner(owner)
{
    CAsyncChannel::New((IAsyncChannel**)&mWsmChannel);
    AutoPtr<IHandler> handler;
    mOwner->mWifiStateMachine->GetHandler((IHandler**)&handler);
    mWsmChannel->Connect(mOwner->mContext, this, handler);
}

ECode WifiServiceImpl::WifiStateMachineHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case IAsyncChannel::CMD_CHANNEL_HALF_CONNECTED: {
            Int32 arg1;
            msg->GetArg1(&arg1);
            if (arg1 == IAsyncChannel::STATUS_SUCCESSFUL) {
                mOwner->mWifiStateMachineChannel = mWsmChannel;
            } else {
                Slogger::E(TAG, "WifiStateMachine connection failure, error=%d", arg1);
                mOwner->mWifiStateMachineChannel = NULL;
            }
            break;
        }
        case IAsyncChannel::CMD_CHANNEL_DISCONNECTED: {
            Int32 arg1;
            msg->GetArg1(&arg1);
            Slogger::E(TAG, "WifiStateMachine channel lost, msg.arg1 =%d", arg1);
            mOwner->mWifiStateMachineChannel = NULL;
            //Re-establish connection to state machine
            AutoPtr<IHandler> handler;
            mOwner->mWifiStateMachine->GetHandler((IHandler**)&handler);
            mWsmChannel->Connect(mOwner->mContext, this, handler);
            break;
        }
        default: {
            Slogger::D(TAG, "WifiStateMachineHandler.handleMessage ignoring msg=");// + msg);
            break;
        }
    }
    return NOERROR;
}

//=====================================================================
//               WifiServiceImpl::InnerBroadcastReceiver1
//=====================================================================
WifiServiceImpl::InnerBroadcastReceiver1::InnerBroadcastReceiver1(
    /* [in] */ WifiServiceImpl* owner)
    : mOwner(owner)
{
}

ECode WifiServiceImpl::InnerBroadcastReceiver1::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    Boolean result;
    if (mOwner->mSettingsStore->HandleAirplaneModeToggled(&result), result) {
         mOwner->mWifiController->SendMessage(WifiController::CMD_AIRPLANE_TOGGLED);
    }
    return NOERROR;
}

//=====================================================================
//                 WifiServiceImpl::BatchedScanRequest
//=====================================================================
WifiServiceImpl::BatchedScanRequest::BatchedScanRequest(
    /* [in] */ WifiServiceImpl* owner,
    /* [in] */ IBatchedScanSettings* settings,
    /* [in] */ IBinder* binder,
    /* [in] */ IWorkSource* ws)
    : DeathRecipient(owner, 0, String(NULL), binder, NULL)
{
    settings = settings;

    AutoPtr<IBinderHelper> binderHelper;
    CBinderHelper::AcquireSingleton((IBinderHelper**)&binderHelper);
    Int32 uid;
    binderHelper->GetCallingUid(&uid);
    binderHelper->GetCallingPid(&pid);
    workSource = ws;
}

ECode WifiServiceImpl::BatchedScanRequest::ProxyDied()
{
    mOwner->StopBatchedScan(settings, uid, pid);
    return NOERROR;
}

ECode WifiServiceImpl::BatchedScanRequest::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = String("BatchedScanRequest{settings=TODO") + /*settings +*/ String(", binder=TODO");// + mBinder + "}";
    return NOERROR;
}

ECode WifiServiceImpl::BatchedScanRequest::IsSameApp(
    /* [in] */ Int32 uid,
    /* [in] */ Int32 pid,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = (this->uid == uid && this->pid == pid);
    return NOERROR;
}

//=====================================================================
//               WifiServiceImpl::InnerBroadcastReceiver2
//=====================================================================
WifiServiceImpl::InnerBroadcastReceiver2::InnerBroadcastReceiver2(
    /* [in] */ WifiServiceImpl* owner)
    : mOwner(owner)
{
}

ECode WifiServiceImpl::InnerBroadcastReceiver2::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (action.Equals(IIntent::ACTION_SCREEN_ON)) {
        mOwner->mWifiController->SendMessage(WifiController::CMD_SCREEN_ON);
    } else if (action.Equals(IIntent::ACTION_USER_PRESENT)) {
        mOwner->mWifiController->SendMessage(WifiController::CMD_USER_PRESENT);
    } else if (action.Equals(IIntent::ACTION_SCREEN_OFF)) {
        mOwner->mWifiController->SendMessage(WifiController::CMD_SCREEN_OFF);
    } else if (action.Equals(IIntent::ACTION_BATTERY_CHANGED)) {
        Int32 pluggedType;
        intent->GetInt32Extra(String("plugged"), 0, &pluggedType);
        mOwner->mWifiController->SendMessage(WifiController::CMD_BATTERY_CHANGED, pluggedType, 0, NULL);
    } else if (action.Equals(IBluetoothAdapter::ACTION_CONNECTION_STATE_CHANGED)) {
        Int32 state;
        intent->GetInt32Extra(IBluetoothAdapter::EXTRA_CONNECTION_STATE,
                IBluetoothAdapter::STATE_DISCONNECTED, &state);
        mOwner->mWifiStateMachine->SendBluetoothAdapterStateChange(state);
    } else if (action.Equals(ITelephonyIntents::ACTION_EMERGENCY_CALLBACK_MODE_CHANGED)) {
        Boolean emergencyMode;
        intent->GetBooleanExtra(String("phoneinECMState"), FALSE, &emergencyMode);
        mOwner->mWifiController->SendMessage(WifiController::CMD_EMERGENCY_MODE_CHANGED, emergencyMode ? 1 : 0, 0);
    } else if (action.Equals(IWifiManager::WIFI_AP_STATE_CHANGED_ACTION)) {
        Int32 wifiApState;
        intent->GetInt32Extra(IWifiManager::EXTRA_WIFI_AP_STATE,
                IWifiManager::WIFI_AP_STATE_FAILED, &wifiApState);
        Slogger::D(TAG, "wifiApState=%d", wifiApState);
        /*
         * If start SoftAp fails, WifiStateMachine would transition to InitialState,
         * but WifiController is left stuck in ApEnabledState, which in turn
         * fails to turn on WLAN again.
         *
         * Register WifiService to receive WIFI_AP_STATE_CHANGED_ACTION intent
         * from WifiStateMachine, and if wifiApState is failed, inform WifiController
         * to transtion to ApStaDisabledState.
         */
        if (wifiApState == IWifiManager::WIFI_AP_STATE_FAILED) {
            mOwner->SetWifiApEnabled(NULL, FALSE);
        }
    }
    return NOERROR;
}

//=====================================================================
//                WifiServiceImpl::InnerContentObserver1
//=====================================================================
WifiServiceImpl::InnerContentObserver1::InnerContentObserver1(
    /* [in] */ WifiServiceImpl* owner)
    : mOwner(owner)
{
}

ECode WifiServiceImpl::InnerContentObserver1::OnChange(
    /* [in] */ Boolean selfChange)
{
    mOwner->mSettingsStore->HandleWifiScanAlwaysAvailableToggled();
    mOwner->mWifiController->SendMessage(WifiController::CMD_SCAN_ALWAYS_MODE_CHANGED);
    return NOERROR;
}

//=====================================================================
//                      WifiServiceImpl::WifiLock
//=====================================================================
WifiServiceImpl::WifiLock::WifiLock(
    /* [in] */ WifiServiceImpl* owner,
    /* [in] */ Int32 lockMode,
    /* [in] */ const String& tag,
    /* [in] */ IBinder* binder,
    /* [in] */ IWorkSource* ws)
    : DeathRecipient(owner, lockMode, tag, binder, ws)
{
}

ECode WifiServiceImpl::WifiLock::ProxyDied()
{
    {
        AutoLock lock(mOwner->mLocks);
        mOwner->ReleaseWifiLockLocked(mBinder);
    }
    return NOERROR;
}

ECode WifiServiceImpl::WifiLock::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = String("WifiLock{") + mTag + String(" type=") + StringUtils::ToString(mMode) + String(" binder=TODO ");// + mBinder + "}";
    return NOERROR;
}

//=====================================================================
//                   WifiServiceImpl::DeathRecipient
//=====================================================================
CAR_INTERFACE_IMPL(WifiServiceImpl::DeathRecipient, Object, IProxyDeathRecipient)

WifiServiceImpl::DeathRecipient::DeathRecipient(
    /* [in] */ WifiServiceImpl* owner,
    /* [in] */ Int32 mode,
    /* [in] */ const String& tag,
    /* [in] */ IBinder* binder,
    /* [in] */ IWorkSource* ws)
    : mOwner(owner)
{
    mTag = tag;
    mMode = mode;
    mBinder = binder;
    mWorkSource = ws;
    //try {
    AutoPtr<IProxy> proxy = (IProxy*)mBinder->Probe(EIID_IProxy);//IProxy::Probe(mBinder);
    if (proxy != NULL) {
        proxy->LinkToDeath(this, 0);
    //    //if (FAILED(ec))
    //    //    ProxyDied();
    }
    //} catch (RemoteException e) {
        //binderDied();
    //}
}

ECode WifiServiceImpl::DeathRecipient::UnlinkDeathRecipient()
{
    AutoPtr<IProxy> proxy = (IProxy*)mBinder->Probe(EIID_IProxy);//IProxy::Probe(mBinder);
    if (proxy != NULL) {
        Boolean b;
        proxy->UnlinkToDeath(this, 0, &b);
    }
    return NOERROR;
}

//=====================================================================
//                     WifiServiceImpl::Multicaster
//=====================================================================
WifiServiceImpl::Multicaster::Multicaster(
    /* [in] */ WifiServiceImpl* owner,
    /* [in] */ const String& tag,
    /* [in] */ IBinder* binder)
    : DeathRecipient(owner, 0, tag, binder, NULL)
{
    // ==================before translated======================
    AutoPtr<IBinderHelper> binderHelper;
    CBinderHelper::AcquireSingleton((IBinderHelper**)&binderHelper);
    Int32 uid;
    binderHelper->GetCallingUid(&uid);
    mMode = uid;
}

ECode WifiServiceImpl::Multicaster::ProxyDied()
{
    Slogger::E(TAG, "Multicaster binderDied");
    {
        AutoLock lock(mOwner->mMulticasters);
        Int32 i;
        mOwner->mMulticasters->IndexOf(TO_IINTERFACE(this), &i);
        if (i != -1) {
            mOwner->RemoveMulticasterLocked(i, mMode);
        }
    }
    return NOERROR;
}

ECode WifiServiceImpl::Multicaster::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = String("Multicaster{") + mTag + String(" binder=TOOD");// + mBinder + "}";
    return NOERROR;
}

ECode WifiServiceImpl::Multicaster::GetUid(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mMode;
    return NOERROR;
}

//=====================================================================
//                           WifiServiceImpl
//=====================================================================
const String WifiServiceImpl::TAG("WifiService");
const Boolean WifiServiceImpl::DBG = true;

CAR_INTERFACE_IMPL_2(WifiServiceImpl, Object, IIWifiManager, IBinder);

WifiServiceImpl::WifiServiceImpl()
{
    CArrayList::New((IList**)&mBatchedScanners);
    mLocks = new LockList(this);
}

ECode WifiServiceImpl::constructor(
    /* [in] */ IContext* context)
{
    mContext = context;

    AutoPtr<ISystemProperties> syspro;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&syspro);
    syspro->Get(String("wifi.interface"), String("wlan0"), &mInterfaceName);

    mTrafficPoller = new WifiTrafficPoller(mContext, mInterfaceName);
    mWifiStateMachine = new WifiStateMachine(mContext, mInterfaceName, mTrafficPoller);
    mWifiStateMachine->EnableRssiPolling(TRUE);
    mBatteryStats = BatteryStatsService::GetService();

    AutoPtr<IInterface> appOpsObj;
    mContext->GetSystemService(IContext::APP_OPS_SERVICE, (IInterface**)&appOpsObj);
    mAppOps = IAppOpsManager::Probe(appOpsObj);

    mNotificationController = new WifiNotificationController(mContext, mWifiStateMachine);
    mSettingsStore = new WifiSettingsStore(mContext);

    AutoPtr<IHandlerThread> wifiThread;
    CHandlerThread::New(String("WifiService"), (IHandlerThread**)&wifiThread);
    IThread::Probe(wifiThread)->Start();
    AutoPtr<ILooper> looper;
    wifiThread->GetLooper((ILooper**)&looper);
    mClientHandler = new ClientHandler(this, looper);
    mWifiStateMachineHandler = new WifiStateMachineHandler(this, looper);
    mWifiController = new WifiController(mContext, this, looper);

    AutoPtr<IResources> resources;
    mContext->GetResources((IResources**)&resources);
    resources->GetBoolean(R::bool_::config_wifi_batched_scan_supported, &mBatchedScanSupported);
    return NOERROR;
}

ECode WifiServiceImpl::CheckAndStartWifi()
{
    /* Check if wi-fi needs to be enabled */
    Boolean wifiEnabled;
    mSettingsStore->IsWifiToggleEnabled(&wifiEnabled);
    Slogger::I(TAG, "WifiService starting up with Wi-Fi %s", (wifiEnabled ? "enabled" : "disabled"));

    RegisterForScanModeChange();
    AutoPtr<IBroadcastReceiver> receiver = new InnerBroadcastReceiver1(this);
    AutoPtr<IIntentFilter> intentFilter;
    CIntentFilter::New(IIntent::ACTION_AIRPLANE_MODE_CHANGED, (IIntentFilter**)&intentFilter);
    AutoPtr<IIntent> intent;
    mContext->RegisterReceiver(receiver, intentFilter, (IIntent**)&intent);

    // Adding optimizations of only receiving broadcasts when wifi is enabled
    // can result in race conditions when apps toggle wifi in the background
    // without active user involvement. Always receive broadcasts.
    RegisterForBroadcasts();

    mWifiController->Start();

    mIsControllerStarted = TRUE;

    // If we are already disabled (could be due to airplane mode), avoid changing persist
    // state here
    Boolean bTemp;
    if (wifiEnabled) SetWifiEnabled(wifiEnabled, &bTemp);

    AutoPtr<IMessenger> messenger;
    mWifiStateMachine->GetMessenger((IMessenger**)&messenger);
    mWifiWatchdogStateMachine = WifiWatchdogStateMachine::MakeWifiWatchdogStateMachine(mContext, messenger);
    return NOERROR;
}

ECode WifiServiceImpl::PingSupplicant(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    EnforceAccessPermission();
    if (mWifiStateMachineChannel != NULL) {
        mWifiStateMachine->SyncPingSupplicant(mWifiStateMachineChannel, result);
    } else {
        Slogger::E(TAG, "mWifiStateMachineChannel is not initialized");
        *result = FALSE;
    }
    return NOERROR;
}

ECode WifiServiceImpl::GetChannelList(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);
    EnforceAccessPermission();
    if (mWifiStateMachineChannel != NULL) {
        mWifiStateMachine->SyncGetChannelList(mWifiStateMachineChannel, result);
    } else {
        Slogger::E(TAG, "mWifiStateMachineChannel is not initialized");
        *result = NULL;
    }
    return NOERROR;
}

ECode WifiServiceImpl::StartScan(
    /* [in] */ IScanSettings* settings,
    /* [in] */ IWorkSource* workSource)
{
    AutoPtr<IScanSettings> scanSettings = settings;
    EnforceChangePermission();
    if (settings != NULL) {
        // TODO: should be removed once the startCustomizedScan API is opened up
        mContext->EnforceCallingOrSelfPermission(Manifest::permission::LOCATION_HARDWARE,
                String("LocationHardware"));
        scanSettings = NULL;
        CScanSettings::New(settings, (IScanSettings**)&scanSettings);
        Boolean isValid;

        if (scanSettings->IsValid(&isValid), !isValid) {
            Slogger::E(TAG, "invalid scan setting");
            return NOERROR;
        }
    }
    if (workSource != NULL) {
        EnforceWorkSourcePermission();
        // WifiManager currently doesn't use names, so need to clear names out of the
        // supplied WorkSource to allow future WorkSource combining.
        workSource->ClearNames();
    }
    AutoPtr<IBinderHelper> binderHelper;
    CBinderHelper::AcquireSingleton((IBinderHelper**)&binderHelper);
    Int32 uid;
    binderHelper->GetCallingUid(&uid);
    mWifiStateMachine->StartScan(uid, scanRequestCounter++, scanSettings, workSource);
    return NOERROR;
}

ECode WifiServiceImpl::IsBatchedScanSupported(
    /* [out] */ Boolean* result)
{
    *result = mBatchedScanSupported;
    return NOERROR;
}

ECode WifiServiceImpl::PollBatchedScan()
{
    EnforceChangePermission();
    if (mBatchedScanSupported == FALSE) return NOERROR;
    return mWifiStateMachine->RequestBatchedScanPoll();
}

ECode WifiServiceImpl::GetWpsNfcConfigurationToken(
    /* [in] */ Int32 netId,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    EnforceConnectivityInternalPermission();
    return mWifiStateMachine->SyncGetWpsNfcConfigurationToken(netId, result);
}

ECode WifiServiceImpl::RequestBatchedScan(
    /* [in] */ IBatchedScanSettings* requested,
    /* [in] */ IBinder* binder,
    /* [in] */ IWorkSource* workSource,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    EnforceChangePermission();
    if (workSource != NULL) {
        EnforceWorkSourcePermission();
        // WifiManager currently doesn't use names, so need to clear names out of the
        // supplied WorkSource to allow future WorkSource combining.
        workSource->ClearNames();
    }
    if (mBatchedScanSupported == FALSE) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<IBatchedScanSettings> batchedScanSettings;
    CBatchedScanSettings::New(requested, (IBatchedScanSettings**)&batchedScanSettings);
    Boolean isInvalid;
    if (batchedScanSettings->IsInvalid(&isInvalid), isInvalid) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<BatchedScanRequest> r = new BatchedScanRequest(this, batchedScanSettings, binder, workSource);
    synchronized(mBatchedScanners) {
        mBatchedScanners->Add(TO_IINTERFACE(r));
        ResolveBatchedScannersLocked();
    }
    *result = TRUE;
    return NOERROR;
}

ECode WifiServiceImpl::GetBatchedScanResults(
    /* [in] */ const String& callingPackage,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);
    EnforceAccessPermission();
    if (mBatchedScanSupported == FALSE) {
        CArrayList::New(result);
        return NOERROR;
    }
    AutoPtr<IBinderHelper> binderHelper;
    CBinderHelper::AcquireSingleton((IBinderHelper**)&binderHelper);
    Int32 uid;
    binderHelper->GetCallingUid(&uid);
    AutoPtr<IUserHandleHelper> uhHelper;
    Int32 userId;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&uhHelper);
    uhHelper->GetCallingUserId(&userId);
    Int64 ident;
    binderHelper->ClearCallingIdentity(&ident);
    //try {
    Int32 noteOp;
    if ((mAppOps->NoteOp(IAppOpsManager::OP_WIFI_SCAN, uid, callingPackage, &noteOp), noteOp)
                != IAppOpsManager::MODE_ALLOWED) {
        CArrayList::New(result);
        return NOERROR;
    }
    if (!IsCurrentProfile(userId)) {
        CArrayList::New(result);
        return NOERROR;
    }
    mWifiStateMachine->SyncGetBatchedScanResultsList(result);
    //} finally {
    binderHelper->RestoreCallingIdentity(ident);
    //}
    return NOERROR;
}

ECode WifiServiceImpl::StopBatchedScan(
    /* [in] */ IBatchedScanSettings* settings)
{
    EnforceChangePermission();
    if (mBatchedScanSupported == FALSE) return NOERROR;
    AutoPtr<IBinderHelper> binderHelper;
    CBinderHelper::AcquireSingleton((IBinderHelper**)&binderHelper);
    Int32 uid;
    binderHelper->GetCallingUid(&uid);
    Int32 pid;
    binderHelper->GetCallingPid(&pid);
    StopBatchedScan(settings, uid, pid);
    return NOERROR;
}

// synchronized
ECode WifiServiceImpl::SetWifiEnabled(
    /* [in] */ Boolean enable,
    /* [out] */ Boolean* result)
{
    AutoLock lock(this);
    VALIDATE_NOT_NULL(result);
    EnforceChangePermission();
    AutoPtr<IBinderHelper> binderHelper;
    CBinderHelper::AcquireSingleton((IBinderHelper**)&binderHelper);
    Int32 uid;
    binderHelper->GetCallingUid(&uid);
    Int32 pid;
    binderHelper->GetCallingPid(&pid);
    Slogger::D(TAG, "setWifiEnabled: %d, pid = %d, uid = %d", enable, pid, uid);
    if (DBG) {
        Slogger::E(TAG, "Invoking mWifiStateMachine.setWifiEnabled\n");
    }

    /*
    * Caller might not have WRITE_SECURE_SETTINGS,
    * only CHANGE_WIFI_STATE is enforced
    */

    Int64 ident;
    binderHelper->ClearCallingIdentity(&ident);
    //try {
    Boolean bTemp;
    if (!(mSettingsStore->HandleWifiToggled(enable, &bTemp), bTemp)) {
        // Nothing to do if wifi cannot be toggled
        *result = TRUE;
        return NOERROR;
    }
    //} finally {
    binderHelper->RestoreCallingIdentity(ident);
    //}

    if (!mIsControllerStarted) {
        Slogger::E(TAG,"WifiController is not yet started, abort setWifiEnabled");
        *result = FALSE;
        return NOERROR;
    }

    mWifiController->SendMessage(WifiController::CMD_WIFI_TOGGLED);
    *result = TRUE;
    return NOERROR;
}

ECode WifiServiceImpl::GetWifiEnabledState(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    EnforceAccessPermission();
    return mWifiStateMachine->SyncGetWifiState(result);
}

ECode WifiServiceImpl::SetWifiApEnabled(
    /* [in] */ IWifiConfiguration* wifiConfig,
    /* [in] */ Boolean enabled)
{
    EnforceChangePermission();
    AutoPtr<IConnectivityManagerHelper> cmHelper;
    CConnectivityManagerHelper::AcquireSingleton((IConnectivityManagerHelper**)&cmHelper);
    cmHelper->EnforceTetherChangePermission(mContext);
    AutoPtr<IUserManager> um;
    AutoPtr<IUserManagerHelper> umHelper;
    CUserManagerHelper::AcquireSingleton((IUserManagerHelper**)&umHelper);
    umHelper->Get(mContext, (IUserManager**)&um);
    Boolean restriction;
    if (um->HasUserRestriction(IUserManager::DISALLOW_CONFIG_TETHERING, &restriction), restriction) {
        //throw new SecurityException("DISALLOW_CONFIG_TETHERING is enabled for this user.");
        Slogger::E(TAG, "DISALLOW_CONFIG_TETHERING is enabled for this user.");
        return E_SECURITY_EXCEPTION;
    }
    // null wifiConfig is a meaningful input for CMD_SET_AP
    Boolean isValid;
    if (wifiConfig == NULL || (wifiConfig->IsValid(&isValid), isValid)) {
        AutoPtr<IMessage> msg;
        mWifiController->ObtainMessage(WifiController::CMD_SET_AP, enabled ? 1 : 0, 0, TO_IINTERFACE(wifiConfig), (IMessage**)&msg);
        msg->SendToTarget();
    } else {
        Slogger::E(TAG, "Invalid WifiConfiguration");
    }
    return NOERROR;
}

ECode WifiServiceImpl::GetWifiApEnabledState(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    EnforceAccessPermission();
    return mWifiStateMachine->SyncGetWifiApState(result);
}

ECode WifiServiceImpl::GetWifiApConfiguration(
    /* [out] */ IWifiConfiguration** result)
{
    VALIDATE_NOT_NULL(result);
    EnforceAccessPermission();
    return mWifiStateMachine->SyncGetWifiApConfiguration(result);
}

ECode WifiServiceImpl::SetWifiApConfiguration(
    /* [in] */ IWifiConfiguration* wifiConfig)
{
    EnforceChangePermission();
    if (wifiConfig == NULL)
        return NOERROR;
    Boolean isValid;
    if (wifiConfig->IsValid(&isValid), isValid) {
        return mWifiStateMachine->SetWifiApConfiguration(wifiConfig);
    } else {
        Slogger::E(TAG, "Invalid WifiConfiguration");
    }
    return NOERROR;
}

ECode WifiServiceImpl::IsScanAlwaysAvailable(
    /* [out] */ Boolean* result)
{
    EnforceAccessPermission();
    return mSettingsStore->IsScanAlwaysAvailable(result);
}

ECode WifiServiceImpl::Disconnect()
{
    EnforceChangePermission();
    return mWifiStateMachine->DisconnectCommand();
}

ECode WifiServiceImpl::Reconnect()
{
    EnforceChangePermission();
    return mWifiStateMachine->ReconnectCommand();
}

ECode WifiServiceImpl::Reassociate()
{
    EnforceChangePermission();
    return mWifiStateMachine->ReassociateCommand();
}

ECode WifiServiceImpl::GetSupportedFeatures(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    EnforceAccessPermission();
    if (mWifiStateMachineChannel != NULL) {
        return mWifiStateMachine->SyncGetSupportedFeatures(mWifiStateMachineChannel, result);
    } else {
        Slogger::E(TAG, "mWifiStateMachineChannel is not initialized");
        *result = 0;
    }
    return NOERROR;
}

ECode WifiServiceImpl::ReportActivityInfo(
    /* [out] */ IWifiActivityEnergyInfo** result)
{
    EnforceAccessPermission();
    AutoPtr<IWifiLinkLayerStats> stats;
    AutoPtr<IWifiActivityEnergyInfo> energyInfo;
    if (mWifiStateMachineChannel != NULL) {
        mWifiStateMachine->SyncGetLinkLayerStats(mWifiStateMachineChannel, (IWifiLinkLayerStats**)&stats);
        if (stats != NULL) {
            // Convert the LinkLayerStats into EnergyActivity
            Int32 on_time, tx_time, rx_time;
            stats->GetOn_time(&on_time);
            stats->GetTx_time(&tx_time);
            stats->GetRx_time(&rx_time);
            CWifiActivityEnergyInfo::New(
                    IWifiActivityEnergyInfo::STACK_STATE_STATE_IDLE, tx_time,
                    rx_time, on_time - tx_time - rx_time,
                    0 /* TBD */, (IWifiActivityEnergyInfo**)&energyInfo);
        }
        *result = energyInfo;
        REFCOUNT_ADD(*result);
    } else {
        Slogger::E(TAG, "mWifiStateMachineChannel is not initialized");
        *result = NULL;
    }
    return NOERROR;
}

ECode WifiServiceImpl::GetConfiguredNetworks(
    /* [out] */ IList** result)
{
    EnforceAccessPermission();
    if (mWifiStateMachineChannel != NULL) {
        AutoPtr<IBinderHelper> binderHelper;
        CBinderHelper::AcquireSingleton((IBinderHelper**)&binderHelper);
        Int32 uid;
        binderHelper->GetCallingUid(&uid);
        return mWifiStateMachine->SyncGetConfiguredNetworks(uid,
                mWifiStateMachineChannel, result);
    } else {
        Slogger::E(TAG, "mWifiStateMachineChannel is not initialized");
        *result = NULL;
    }
    return NOERROR;
}

ECode WifiServiceImpl::GetPrivilegedConfiguredNetworks(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);
    EnforceReadCredentialPermission();
    EnforceAccessPermission();
    if (mWifiStateMachineChannel != NULL) {
        return mWifiStateMachine->SyncGetPrivilegedConfiguredNetwork(mWifiStateMachineChannel, result);
    } else {
        Slogger::E(TAG, "mWifiStateMachineChannel is not initialized");
        *result = NULL;
        return NOERROR;
    }
    return NOERROR;
}

ECode WifiServiceImpl::AddOrUpdateNetwork(
    /* [in] */ IWifiConfiguration* config,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    EnforceChangePermission();
    Boolean isValid;
    if (config->IsValid(&isValid), isValid) {
        //TODO: pass the Uid the WifiStateMachine as a message parameter
        AutoPtr<IBinderHelper> binderHelper;
        CBinderHelper::AcquireSingleton((IBinderHelper**)&binderHelper);
        Int32 uid;
        binderHelper->GetCallingUid(&uid);
        String SSID;
        config->GetSSID(&SSID);
        Int32 networkId;
        config->GetNetworkId(&networkId);
        Slogger::E("addOrUpdateNetwork", " uid = %d, SSID=%s, nid=%d", uid,SSID.string(), networkId);
        if (networkId == IWifiConfiguration::INVALID_NETWORK_ID) {
            config->SetCreatorUid(uid);
        } else {
            config->SetLastUpdateUid(uid);
        }
        if (mWifiStateMachineChannel != NULL) {
            return mWifiStateMachine->SyncAddOrUpdateNetwork(mWifiStateMachineChannel, config, result);
        } else {
            Slogger::E(TAG, "mWifiStateMachineChannel is not initialized");
            *result = -1;
        }
    } else {
        Slogger::E(TAG, "bad network configuration");
        *result = -1;
    }
    return NOERROR;
}

ECode WifiServiceImpl::RemoveNetwork(
    /* [in] */ Int32 netId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    EnforceChangePermission();

    AutoPtr<IBinderHelper> binderHelper;
    CBinderHelper::AcquireSingleton((IBinderHelper**)&binderHelper);
    Int32 uid;
    binderHelper->GetCallingUid(&uid);
    if (!IsOwner(uid)) {
        Slogger::E(TAG, "Remove is not authorized for user");
        *result = FALSE;
        return NOERROR;
    }

    if (mWifiStateMachineChannel != NULL) {
        return mWifiStateMachine->SyncRemoveNetwork(mWifiStateMachineChannel, netId, result);
    } else {
        Slogger::E(TAG, "mWifiStateMachineChannel is not initialized");
        *result = FALSE;
    }
    return NOERROR;
}

ECode WifiServiceImpl::EnableNetwork(
    /* [in] */ Int32 netId,
    /* [in] */ Boolean disableOthers,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    EnforceChangePermission();
    if (mWifiStateMachineChannel != NULL) {
        return mWifiStateMachine->SyncEnableNetwork(mWifiStateMachineChannel, netId,
                disableOthers, result);
    } else {
        Slogger::E(TAG, "mWifiStateMachineChannel is not initialized");
        return false;
    }
    return NOERROR;
}

ECode WifiServiceImpl::DisableNetwork(
    /* [in] */ Int32 netId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    EnforceChangePermission();
    if (mWifiStateMachineChannel != NULL) {
        return mWifiStateMachine->SyncDisableNetwork(mWifiStateMachineChannel, netId, result);
    } else {
        Slogger::E(TAG, "mWifiStateMachineChannel is not initialized");
        *result = FALSE;
    }
    return NOERROR;
}

ECode WifiServiceImpl::GetConnectionInfo(
    /* [out] */ IWifiInfo** result)
{
    VALIDATE_NOT_NULL(result);
    EnforceAccessPermission();
    /*
     * Make sure we have the latest information, by sending
     * a status request to the supplicant.
     */
    return mWifiStateMachine->SyncRequestConnectionInfo(result);
}

ECode WifiServiceImpl::GetScanResults(
    /* [in] */ const String& callingPackage,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);
    EnforceAccessPermission();
    AutoPtr<IUserHandleHelper> uhHelper;
    Int32 userId;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&uhHelper);
    uhHelper->GetCallingUserId(&userId);
    AutoPtr<IBinderHelper> binderHelper;
    CBinderHelper::AcquireSingleton((IBinderHelper**)&binderHelper);
    Int32 uid;
    binderHelper->GetCallingUid(&uid);
    Int64 ident;
    binderHelper->ClearCallingIdentity(&ident);
    //try {
    Int32 noteOp;
    if ((mAppOps->NoteOp(IAppOpsManager::OP_WIFI_SCAN, uid, callingPackage, &noteOp), noteOp)
            != IAppOpsManager::MODE_ALLOWED) {
        CArrayList::New(result);
        return NOERROR;
    }
    if (!IsCurrentProfile(userId)) {
        CArrayList::New(result);
        return NOERROR;
    }
    mWifiStateMachine->SyncGetScanResultsList(result);
    //} finally {
    binderHelper->RestoreCallingIdentity(ident);
    //}
    return NOERROR;
}

ECode WifiServiceImpl::SaveConfiguration(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    EnforceChangePermission();
    if (mWifiStateMachineChannel != NULL) {
        return mWifiStateMachine->SyncSaveConfig(mWifiStateMachineChannel, result);
    } else {
        Slogger::E(TAG, "mWifiStateMachineChannel is not initialized");
        *result = FALSE;
        return NOERROR;
    }
    return NOERROR;
}

ECode WifiServiceImpl::SetCountryCode(
    /* [in] */ const String& countryCode,
    /* [in] */ Boolean persist)
{
    Slogger::I(TAG, "WifiService trying to set country code to %s with persist set to %d", countryCode.string(), persist);
    EnforceConnectivityInternalPermission();
    Int64 token;
    AutoPtr<IBinderHelper> binderHelper;
    CBinderHelper::AcquireSingleton((IBinderHelper**)&binderHelper);
    binderHelper->ClearCallingIdentity(&token);
    //try {
    mWifiStateMachine->SetCountryCode(countryCode, persist);
    //} finally {
    binderHelper->RestoreCallingIdentity(token);
    //}
    return NOERROR;
}

ECode WifiServiceImpl::SetFrequencyBand(
    /* [in] */ Int32 band,
    /* [in] */ Boolean persist)
{
    EnforceChangePermission();
    Boolean bTemp;
    if (!(IsDualBandSupported(&bTemp), bTemp)) return NOERROR;
    Slogger::I(TAG, "WifiService trying to set frequency band to %d  with persist set to %d", band, persist);
    Int64 token;
    AutoPtr<IBinderHelper> binderHelper;
    CBinderHelper::AcquireSingleton((IBinderHelper**)&binderHelper);
    binderHelper->ClearCallingIdentity(&token);
    //try {
    mWifiStateMachine->SetFrequencyBand(band, persist);
    //} finally {
    binderHelper->RestoreCallingIdentity(token);
    //}
    return NOERROR;
}

ECode WifiServiceImpl::GetFrequencyBand(
    /* [out] */ Int32* result)
{
    EnforceAccessPermission();
    return mWifiStateMachine->GetFrequencyBand(result);
}

ECode WifiServiceImpl::IsDualBandSupported(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    //TODO: Should move towards adding a driver API that checks at runtime
    AutoPtr<IResources> resources;
    mContext->GetResources((IResources**)&resources);
    resources->GetBoolean(
            R::bool_::config_wifi_dual_band_support, result);
    return NOERROR;
}

ECode WifiServiceImpl::IsIbssSupported(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    EnforceAccessPermission();
    if (mWifiStateMachineChannel != NULL) {
        Int32 supported;
        *result = ((mWifiStateMachine->SyncIsIbssSupported(mWifiStateMachineChannel, &supported), supported) == 1);
    } else {
        Slogger::E(TAG, "mWifiStateMachineChannel is not initialized");
        *result = FALSE;
    }
    return NOERROR;
}

ECode WifiServiceImpl::GetDhcpInfo(
    /* [out] */ IDhcpInfo** result)
{
    VALIDATE_NOT_NULL(result);
    EnforceAccessPermission();
    AutoPtr<IDhcpResults> dhcpResults;
    mWifiStateMachine->SyncGetDhcpResults((IDhcpResults**)&dhcpResults);

    AutoPtr<IDhcpInfo> info;
    CDhcpInfo::New((IDhcpInfo**)&info);

    AutoPtr<ILinkAddress> ipAddress;
    IStaticIpConfiguration::Probe(dhcpResults)->GetIpAddress((ILinkAddress**)&ipAddress);
    AutoPtr<INetworkUtils> networkUtils;
    CNetworkUtils::AcquireSingleton((INetworkUtils**)&networkUtils);
    if (ipAddress != NULL) {
        AutoPtr<IInetAddress> address;
        ipAddress->GetAddress((IInetAddress**)&address);
        IInet4Address* ip4 = IInet4Address::Probe(address);
        if(ip4 != NULL) {
            Int32 intInetAddress;
            networkUtils->InetAddressToInt(ip4, &intInetAddress);
            info->SetIpAddress(intInetAddress);
            Int32 preFixLength;
            ipAddress->GetNetworkPrefixLength(&preFixLength);
            Int32 netmaskInt;
            networkUtils->PrefixLengthToNetmaskInt(preFixLength, &netmaskInt);
            info->SetNetmask(netmaskInt);
        }
    }

    AutoPtr<IInetAddress> gatewayAddress;
    IStaticIpConfiguration::Probe(dhcpResults)->GetGateway((IInetAddress**)&gatewayAddress);
    if (gatewayAddress != NULL) {
        Int32 gateway;
        networkUtils->InetAddressToInt(IInet4Address::Probe(gatewayAddress), &gateway);
        info->SetGateway(gateway);
    }

    Int32 dnsFound = 0;
    AutoPtr<IArrayList> servers;
    IStaticIpConfiguration::Probe(dhcpResults)->GetDnsServers((IArrayList**)&servers);
    Int32 size;
    servers->GetSize(&size);
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> obj;
        servers->Get(i, (IInterface**)&obj);
        IInetAddress* dns = IInetAddress::Probe(obj);
        IInet4Address* ip4 = IInet4Address::Probe(dns);

        if (ip4 != NULL) {
            if (dnsFound == 0) {
                Int32 dns1;
                networkUtils->InetAddressToInt(ip4, &dns1);
                info->SetDns1(dns1);
            } else {
                Int32 dns2;
                networkUtils->InetAddressToInt(ip4, &dns2);
                info->SetDns2(dns2);
            }
            if (++dnsFound > 1) break;
        }
    }
    AutoPtr<IInetAddress> serverAddress;
    dhcpResults->GetServerAddress((IInetAddress**)&serverAddress);
    IInet4Address* ip4SA = IInet4Address::Probe(serverAddress);
    if (ip4SA != NULL) {
        Int32 intSA;
        networkUtils->InetAddressToInt(ip4SA, &intSA);
        info->SetServerAddress(intSA);
    }
    Int32 duration;
    dhcpResults->GetLeaseDuration(&duration);
    info->SetLeaseDuration(duration);;

    *result = info;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode WifiServiceImpl::StartWifi()
{
    EnforceConnectivityInternalPermission();
    /* TODO: may be add permissions for access only to connectivity service
     * TODO: if a start issued, keep wifi alive until a stop issued irrespective
     * of WifiLock & device idle status unless wifi enabled status is toggled
     */

    mWifiStateMachine->SetDriverStart(TRUE);
    mWifiStateMachine->ReconnectCommand();
    return NOERROR;
}

ECode WifiServiceImpl::StopWifi()
{
    EnforceConnectivityInternalPermission();
    /*
     * TODO: if a stop is issued, wifi is brought up only by startWifi
     * unless wifi enabled status is toggled
     */
    mWifiStateMachine->SetDriverStart(FALSE);
    return NOERROR;
}

ECode WifiServiceImpl::AddToBlacklist(
    /* [in] */ const String& bssid)
{
    EnforceChangePermission();

    mWifiStateMachine->AddToBlacklist(bssid);
    return NOERROR;
}

ECode WifiServiceImpl::ClearBlacklist()
{
    EnforceChangePermission();

    mWifiStateMachine->ClearBlacklist();
    return NOERROR;
}

ECode WifiServiceImpl::EnableTdls(
    /* [in] */ const String& remoteAddress,
    /* [in] */ Boolean enable)
{
    if (remoteAddress.IsNull()) {
        //throw new IllegalArgumentException("remoteAddress cannot be null");
        Slogger::E(TAG, "remoteAddress cannot be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<TdlsTaskParams> params = new TdlsTaskParams();
    params->remoteIpAddress = remoteAddress;
    params->enable = enable;
    AutoPtr<TdlsTask> tdlsTask = new TdlsTask(this);
    AutoPtr<ArrayOf<IInterface*> > arrayParams = ArrayOf<IInterface*>::Alloc(1);
    arrayParams->Set(0, TO_IINTERFACE(params));
    tdlsTask->Execute(arrayParams);
    return NOERROR;
}

ECode WifiServiceImpl::EnableTdlsWithMacAddress(
    /* [in] */ const String& remoteMacAddress,
    /* [in] */ Boolean enable)
{
    if (remoteMacAddress.IsNull()) {
        //throw new IllegalArgumentException("remoteMacAddress cannot be null");
        Slogger::E(TAG, "remoteMacAddress cannot be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    return mWifiStateMachine->EnableTdls(remoteMacAddress, enable);
}

ECode WifiServiceImpl::GetWifiServiceMessenger(
    /* [out] */ IMessenger** result)
{
    VALIDATE_NOT_NULL(result);
    EnforceAccessPermission();
    EnforceChangePermission();
    AutoPtr<IMessenger> messenger;
    CMessenger::New(mClientHandler, (IMessenger**)&messenger);
    *result = messenger;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode WifiServiceImpl::GetConfigFile(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    EnforceAccessPermission();
    return mWifiStateMachine->GetConfigFile(result);
}

ECode WifiServiceImpl::EnforceWakeSourcePermission(
    /* [in] */ Int32 uid,
    /* [in] */ Int32 pid)
{
    if (uid == Process::MyUid()) {
        return NOERROR;
    }
    return mContext->EnforcePermission(Manifest::permission::UPDATE_DEVICE_STATS, pid, uid, String(NULL));
}

ECode WifiServiceImpl::AcquireWifiLock(
    /* [in] */ IBinder* binder,
    /* [in] */ Int32 lockMode,
    /* [in] */ const String& tag,
    /* [in] */ IWorkSource* ws,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    mContext->EnforceCallingOrSelfPermission(Manifest::permission::WAKE_LOCK, String(NULL));
    if (lockMode != IWifiManager::WIFI_MODE_FULL &&
            lockMode != IWifiManager::WIFI_MODE_SCAN_ONLY &&
            lockMode != IWifiManager::WIFI_MODE_FULL_HIGH_PERF) {
        Slogger::E(TAG, "Illegal argument, lockMode= %d", lockMode);
        *result = FALSE;
        if (DBG) {
            //throw new IllegalArgumentException("lockMode=" + lockMode);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        return NOERROR;
    }
    if (ws != NULL) {
        Int32 size;
        ws->GetSize(&size);
        if(size == 0) {
            ws = NULL;
        }
    }

    AutoPtr<IBinderHelper> binderHelper;
    CBinderHelper::AcquireSingleton((IBinderHelper**)&binderHelper);
    Int32 uid;
    binderHelper->GetCallingUid(&uid);
    Int32 pid;
    binderHelper->GetCallingPid(&pid);
    if (ws != NULL) {
        EnforceWakeSourcePermission(uid, pid);
    }
    if (ws == NULL) {
        AutoPtr<IWorkSource> nws;
        CWorkSource::New(uid, (IWorkSource**)&nws);
        ws = nws;
    }
    AutoPtr<WifiLock> wifiLock = new WifiLock(this, lockMode, tag, binder, ws);
    synchronized (mLocks) {
        return AcquireWifiLockLocked(wifiLock);
    }
    return NOERROR;
}

ECode WifiServiceImpl::UpdateWifiLockWorkSource(
    /* [in] */ IBinder* lock,
    /* [in] */ IWorkSource* ws)
{
    AutoPtr<IBinderHelper> binderHelper;
    CBinderHelper::AcquireSingleton((IBinderHelper**)&binderHelper);
    Int32 uid;
    binderHelper->GetCallingUid(&uid);
    Int32 pid;
    binderHelper->GetCallingPid(&pid);

    if (ws != NULL) {
        Int32 size;
        ws->GetSize(&size);
        if(size == 0) {
            ws = NULL;
        }
    }
    if (ws != NULL) {
        EnforceWakeSourcePermission(uid, pid);
    }

    Int64 ident;
    binderHelper->ClearCallingIdentity(&ident);
    //try {
    synchronized (mLocks) {
        Int32 index = mLocks->FindLockByBinder(lock);
        if (index < 0) {
            //throw new IllegalArgumentException("Wifi lock not active");
            Slogger::E(TAG, "Wifi lock not active");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        AutoPtr<IInterface> obj;
        mLocks->mList->Get(index, (IInterface**)&obj);
        AutoPtr<WifiLock> wl = (WifiLock*)(IObject::Probe(obj));
        NoteReleaseWifiLock(wl);
        AutoPtr<IWorkSource> nws;
        (ws != NULL) ? CWorkSource::New(ws, (IWorkSource**)&nws): CWorkSource::New(uid, (IWorkSource**)&nws);
        wl->mWorkSource = nws;
        NoteAcquireWifiLock(wl);
    }
    //} catch (RemoteException e) {
    //} finally {
    binderHelper->RestoreCallingIdentity(ident);
    //}
    return NOERROR;
}

ECode WifiServiceImpl::ReleaseWifiLock(
    /* [in] */ IBinder* lock,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    mContext->EnforceCallingOrSelfPermission(Manifest::permission::WAKE_LOCK, String(NULL));
    synchronized (mLocks) {
         *result = ReleaseWifiLockLocked(lock);
    }
    return NOERROR;
}

ECode WifiServiceImpl::InitializeMulticastFiltering()
{
    EnforceMulticastChangePermission();

    synchronized (mMulticasters) {
        // if anybody had requested filters be off, leave off
        Int32 size;
        mMulticasters->GetSize(&size);
        if (size != 0) {
            return NOERROR;
        } else {
            mWifiStateMachine->StartFilteringMulticastV4Packets();
        }
    }
    return NOERROR;
}

ECode WifiServiceImpl::AcquireMulticastLock(
    /* [in] */ IBinder* binder,
    /* [in] */ const String& tag)
{
    VALIDATE_NOT_NULL(binder);
    EnforceMulticastChangePermission();

    synchronized (mMulticasters) {
        mMulticastEnabled++;
        AutoPtr<Multicaster> multicaster = new Multicaster(this, tag, binder);
        mMulticasters->Add(TO_IINTERFACE(multicaster));
        // Note that we could call stopFilteringMulticastV4Packets only when
        // our new size == 1 (first call), but this function won't
        // be called often and by making the stopPacket call each
        // time we're less fragile and self-healing.
        mWifiStateMachine->StopFilteringMulticastV4Packets();
    }

    AutoPtr<IBinderHelper> binderHelper;
    CBinderHelper::AcquireSingleton((IBinderHelper**)&binderHelper);
    Int32 uid;
    binderHelper->GetCallingUid(&uid);
    Int64 ident;
    binderHelper->ClearCallingIdentity(&ident);
    //try {
    mBatteryStats->NoteWifiMulticastEnabled(uid);
    //} catch (RemoteException e) {
    //} finally {
    binderHelper->RestoreCallingIdentity(ident);
    //}
    return NOERROR;
}

ECode WifiServiceImpl::ReleaseMulticastLock()
{
    EnforceMulticastChangePermission();

    AutoPtr<IBinderHelper> binderHelper;
    CBinderHelper::AcquireSingleton((IBinderHelper**)&binderHelper);
    Int32 uid;
    binderHelper->GetCallingUid(&uid);
    synchronized (mMulticasters) {
        mMulticastDisabled++;
        Int32 size;
        mMulticasters->GetSize(&size);
        for (Int32 i = size - 1; i >= 0; i--) {
            AutoPtr<IInterface> obj;
            mMulticasters->Get(i, (IInterface**)&obj);
            Multicaster* m = (Multicaster*)(IObject::Probe(obj));
            if (m != NULL) {
                Int32 mcUid;
                if((m->GetUid(&mcUid), mcUid) == uid) {
                    RemoveMulticasterLocked(i, uid);
                }
            }
        }
    }
    return NOERROR;
}

ECode WifiServiceImpl::IsMulticastEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    EnforceAccessPermission();

    synchronized (mMulticasters) {
        Int32 size;
        mMulticasters->GetSize(&size);
        *result = (size > 0);
    }
    return NOERROR;
}

ECode WifiServiceImpl::GetWifiMonitor(
    /* [out] */ WifiMonitor** result)
{
    return mWifiStateMachine->GetWifiMonitor(result);
}

ECode WifiServiceImpl::EnableVerboseLogging(
    /* [in] */ Int32 verbose)
{
    EnforceAccessPermission();
    return mWifiStateMachine->EnableVerboseLogging(verbose);
}

ECode WifiServiceImpl::GetVerboseLoggingLevel(
    /* [out] */ Int32* result)
{
    EnforceAccessPermission();
    return mWifiStateMachine->GetVerboseLoggingLevel(result);
}

ECode WifiServiceImpl::EnableAggressiveHandover(
    /* [in] */ Int32 enabled)
{
    EnforceAccessPermission();
    return mWifiStateMachine->EnableAggressiveHandover(enabled);
}

ECode WifiServiceImpl::GetAggressiveHandover(
    /* [out] */ Int32* result)
{
    EnforceAccessPermission();
    return mWifiStateMachine->GetAggressiveHandover(result);
}

ECode WifiServiceImpl::SetAllowScansWithTraffic(
    /* [in] */ Int32 enabled)
{
    EnforceAccessPermission();
    return mWifiStateMachine->SetAllowScansWithTraffic(enabled);
}

ECode WifiServiceImpl::GetAllowScansWithTraffic(
    /* [out] */ Int32* result)
{
    EnforceAccessPermission();
    return mWifiStateMachine->GetAllowScansWithTraffic(result);
}

ECode WifiServiceImpl::GetConnectionStatistics(
    /* [out] */ IWifiConnectionStatistics** result)
{
    VALIDATE_NOT_NULL(result);
    EnforceAccessPermission();
    EnforceReadCredentialPermission();
    if (mWifiStateMachineChannel != NULL) {
        return mWifiStateMachine->SyncGetConnectionStatistics(mWifiStateMachineChannel, result);
    } else {
        Slogger::E(TAG, "mWifiStateMachineChannel is not initialized");
        *result = NULL;
        return NOERROR;
    }
    return NOERROR;
}

void WifiServiceImpl::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    Slogger::E(TAG, "WifiServiceImpl::Dump TODO");
    // ==================before translated======================
    // if (mContext.checkCallingOrSelfPermission(android.Manifest.permission.DUMP)
    //         != PackageManager.PERMISSION_GRANTED) {
    //     pw.println("Permission Denial: can't dump WifiService from from pid="
    //             + Binder.getCallingPid()
    //             + ", uid=" + Binder.getCallingUid());
    //     return;
    // }
    // pw.println("Wi-Fi is " + mWifiStateMachine.syncGetWifiStateByName());
    // pw.println("Stay-awake conditions: " +
    //         Settings.Global.getInt(mContext.getContentResolver(),
    //                                Settings.Global.STAY_ON_WHILE_PLUGGED_IN, 0));
    // pw.println("mMulticastEnabled " + mMulticastEnabled);
    // pw.println("mMulticastDisabled " + mMulticastDisabled);
    // mWifiController.dump(fd, pw, args);
    // mSettingsStore.dump(fd, pw, args);
    // mNotificationController.dump(fd, pw, args);
    // mTrafficPoller.dump(fd, pw, args);
    //
    // pw.println("Latest scan results:");
    // List<ScanResult> scanResults = mWifiStateMachine.syncGetScanResultsList();
    // long nowMs = System.currentTimeMillis();
    // if (scanResults != null && scanResults.size() != 0) {
    //     pw.println("    BSSID              Frequency  RSSI    Age      SSID " +
    //             "                                Flags");
    //     for (ScanResult r : scanResults) {
    //         long ageSec = 0;
    //         long ageMilli = 0;
    //         if (nowMs > r.seen && r.seen > 0) {
    //             ageSec = (nowMs - r.seen) / 1000;
    //             ageMilli = (nowMs - r.seen) % 1000;
    //         }
    //         String candidate = " ";
    //         if (r.isAutoJoinCandidate > 0) candidate = "+";
    //         pw.printf("  %17s  %9d  %5d  %3d.%03d%s   %-32s  %s\n",
    //                                  r.BSSID,
    //                                  r.frequency,
    //                                  r.level,
    //                                  ageSec, ageMilli,
    //                                  candidate,
    //                                  r.SSID == null ? "" : r.SSID,
    //                                  r.capabilities);
    //     }
    // }
    // pw.println();
    // pw.println("Locks acquired: " + mFullLocksAcquired + " full, " +
    //         mFullHighPerfLocksAcquired + " full high perf, " +
    //         mScanLocksAcquired + " scan");
    // pw.println("Locks released: " + mFullLocksReleased + " full, " +
    //         mFullHighPerfLocksReleased + " full high perf, " +
    //         mScanLocksReleased + " scan");
    // pw.println();
    // pw.println("Locks held:");
    // mLocks.dump(pw);
    //
    // mWifiWatchdogStateMachine.dump(fd, pw, args);
    // pw.println();
    // mWifiStateMachine.dump(fd, pw, args);
    // pw.println();
    assert(0);
}

void WifiServiceImpl::StopBatchedScan(
    /* [in] */ IBatchedScanSettings* settings,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 pid)
{
    AutoPtr<IArrayList> found;
    CArrayList::New((IArrayList**)&found);
    synchronized(mBatchedScanners) {
        Int32 size;
        mBatchedScanners->GetSize(&size);
        for (Int32 i = 0; i < size; ++i) {
            AutoPtr<IInterface> obj;
            mBatchedScanners->Get(i, (IInterface**)&obj);
            BatchedScanRequest* r = (BatchedScanRequest*)(IObject::Probe(obj));
            Boolean sameApp = FALSE;
            if (r->IsSameApp(uid, pid, &sameApp), sameApp) {
                Boolean equals;
                if (settings == NULL || (IObject::Probe(settings)->Equals(r->settings.Get(), &equals), equals)) {
                    found->Add(TO_IINTERFACE(r));
                    if (settings != NULL) break;
                }
            }
        }
        Int32 foundSize;
        found->GetSize(&foundSize);
        for (Int32 i = 0; i < foundSize; ++i) {
            AutoPtr<IInterface> obj;
            found->Get(i, (IInterface**)&obj);
            mBatchedScanners->Remove(obj);
        }
        if (foundSize != 0) {
            ResolveBatchedScannersLocked();
        }
    }
}

void WifiServiceImpl::ResolveBatchedScannersLocked()
{
    AutoPtr<IBatchedScanSettings> setting;
    CBatchedScanSettings::New((IBatchedScanSettings**)&setting);
    AutoPtr<IWorkSource> responsibleWorkSource;
    Int32 responsibleUid = 0;
    Double responsibleCsph = 0; // Channel Scans Per Hour

    Int32 size;
    if ((mBatchedScanners->GetSize(&size), size) == 0) {
        mWifiStateMachine->SetBatchedScanSettings(NULL, 0, 0, NULL);
        return;
    }
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> obj;
        mBatchedScanners->Get(i, (IInterface**)&obj);
        BatchedScanRequest* r = (BatchedScanRequest*)(IObject::Probe(obj));

        AutoPtr<IBatchedScanSettings> s = r->settings;

        // evaluate responsibility
        Int32 currentChannelCount;
        Int32 currentScanInterval;
        Double currentCsph;

        AutoPtr<ICollection> channelSet;
        s->GetChannelSet((ICollection**)&channelSet);
        Boolean isEmpty;
        if (channelSet == NULL || (channelSet->IsEmpty(&isEmpty), isEmpty)) {
            // all channels - 11 B and 9 A channels roughly.
            currentChannelCount = 9 + 11;
        } else {
            channelSet->GetSize(&currentChannelCount);
            // these are rough est - no real need to correct for reg-domain;
            Boolean contains;
            if (channelSet->Contains(CoreUtils::Convert("A"), &contains), contains) currentChannelCount += (9 - 1);
            if (channelSet->Contains(CoreUtils::Convert("B"), &contains), contains) currentChannelCount += (11 - 1);

        }
        Int32 scanIntervalSec;
        s->GetScanIntervalSec(&scanIntervalSec);
        if (scanIntervalSec == IBatchedScanSettings::UNSPECIFIED) {
            currentScanInterval = IBatchedScanSettings::DEFAULT_INTERVAL_SEC;
        } else {
            currentScanInterval = scanIntervalSec;
        }
        currentCsph = 60 * 60 * currentChannelCount / currentScanInterval;

        if (currentCsph > responsibleCsph) {
            responsibleUid = r->uid;
            responsibleWorkSource = r->workSource;
            responsibleCsph = currentCsph;
        }

        Int32 maxScansPerBatch;
        s->GetMaxScansPerBatch(&maxScansPerBatch);
        Int32 settingsMaxScansPerBatch;
        setting->GetMaxScansPerBatch(&settingsMaxScansPerBatch);
        if (maxScansPerBatch != IBatchedScanSettings::UNSPECIFIED &&
                maxScansPerBatch < settingsMaxScansPerBatch) {
            setting->SetMaxScansPerBatch(maxScansPerBatch);
        }

        Int32 maxApPerScan;
        s->GetMaxApPerScan(&maxApPerScan);
        Int32 settingMaxApPerScan;
        setting->GetMaxApPerScan(&settingMaxApPerScan);
        if (maxApPerScan != IBatchedScanSettings::UNSPECIFIED &&
                (settingMaxApPerScan == IBatchedScanSettings::UNSPECIFIED ||
                maxApPerScan > settingMaxApPerScan)) {
            setting->SetMaxApPerScan(maxApPerScan);
        }
        Int32 settingScanIntervalSec;
        setting->GetScanIntervalSec(&settingScanIntervalSec);
        if (scanIntervalSec != IBatchedScanSettings::UNSPECIFIED &&
                scanIntervalSec < settingScanIntervalSec) {
            setting->SetScanIntervalSec(scanIntervalSec);
        }

        Int32 maxApForDistance;
        s->GetMaxApForDistance(&maxApForDistance);
        Int32 settingMaxApForDistance;
        setting->GetMaxApForDistance(&settingMaxApForDistance);
        if (maxApForDistance != IBatchedScanSettings::UNSPECIFIED &&
                (settingMaxApForDistance == IBatchedScanSettings::UNSPECIFIED ||
                maxApForDistance > settingMaxApForDistance)) {
            setting->SetMaxApForDistance(maxApForDistance);
        }

        Int32 channelSetSize = 0;
        AutoPtr<ICollection> settingChannelSet;
        setting->GetChannelSet((ICollection**)&settingChannelSet);
        Int32 settingChannelSetSize = 0;
        if (channelSet != NULL && (channelSet->GetSize(&channelSetSize), channelSetSize) != 0) {
            if (settingChannelSet == NULL ||
                    (settingChannelSet->GetSize(&settingChannelSetSize), settingChannelSetSize) != 0) {
                if (settingChannelSet == NULL) {
                    CArrayList::New((ICollection**)&settingChannelSet);
                    setting->SetChannelSet(settingChannelSet);
                }
                AutoPtr<ArrayOf<IInterface*> > array;
                channelSet->ToArray((ArrayOf<IInterface*>**)&array);
                for (Int32 i = 0; i < array->GetLength(); ++i) {
                    AutoPtr<IInterface> obj = (*array)[i];
                    //ICharSequence* cs = ICharSequence::Probe(obj);
                    //String str;
                    //cs->ToString(&str);
                    Boolean contains;
                    if ((settingChannelSet->Contains(obj, &contains), contains) == FALSE)
                        settingChannelSet->Add(obj);
                }
            } // else, ignore the constraint - we already use all channels
        } else {
            if (settingChannelSet == NULL || (settingChannelSet->GetSize(&channelSetSize), channelSetSize) != 0) {
                settingChannelSet = NULL;
                CArrayList::New((ICollection**)&settingChannelSet);
                setting->SetChannelSet(settingChannelSet);
            }
        }
    }

    setting->Constrain();
    mWifiStateMachine->SetBatchedScanSettings(setting, responsibleUid, (Int32)responsibleCsph,
            responsibleWorkSource);
}

void WifiServiceImpl::EnforceAccessPermission()
{
     mContext->EnforceCallingOrSelfPermission(Manifest::permission::ACCESS_WIFI_STATE, String("WifiService"));
}

void WifiServiceImpl::EnforceChangePermission()
{
    mContext->EnforceCallingOrSelfPermission(Manifest::permission::CHANGE_WIFI_STATE, String("WifiService"));
}

void WifiServiceImpl::EnforceReadCredentialPermission()
{
    mContext->EnforceCallingOrSelfPermission(Manifest::permission::READ_WIFI_CREDENTIAL, String("WifiService"));
}

void WifiServiceImpl::EnforceWorkSourcePermission()
{
    mContext->EnforceCallingPermission(Manifest::permission::UPDATE_DEVICE_STATS, String("WifiService"));
}

void WifiServiceImpl::EnforceMulticastChangePermission()
{
    mContext->EnforceCallingOrSelfPermission(
             Manifest::permission::CHANGE_WIFI_MULTICAST_STATE,
             String("WifiService"));
}

void WifiServiceImpl::EnforceConnectivityInternalPermission()
{
    mContext->EnforceCallingOrSelfPermission(
             Manifest::permission::CONNECTIVITY_INTERNAL,
             String("ConnectivityService"));
}

Boolean WifiServiceImpl::IsCurrentProfile(
    /* [in] */ Int32 userId)
{
    AutoPtr<IActivityManagerHelper> cmHelper;
    CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&cmHelper);
    Int32 currentUser;
    cmHelper->GetCurrentUser(&currentUser);
    if (userId == currentUser) {
        return TRUE;
    }
    AutoPtr<IUserManager> um;
    AutoPtr<IUserManagerHelper> umHelper;
    CUserManagerHelper::AcquireSingleton((IUserManagerHelper**)&umHelper);
    umHelper->Get(mContext, (IUserManager**)&um);
    AutoPtr<IList> profiles;//UserInfo
    um->GetProfiles(currentUser, (IList**)&profiles);
    Int32 size;
    profiles->GetSize(&size);
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> obj;
        profiles->Get(i, (IInterface**)&obj);
        IUserInfo* profile = IUserInfo::Probe(obj);
        Int32 id;
        profile->GetId(&id);
        if (userId == id) {
            return TRUE;
        }
    }
    return FALSE;
}

Boolean WifiServiceImpl::IsOwner(
    /* [in] */ Int32 uid)
{
    AutoPtr<IBinderHelper> binderHelper;
    CBinderHelper::AcquireSingleton((IBinderHelper**)&binderHelper);
    Int64 ident;
    binderHelper->ClearCallingIdentity(&ident);
    AutoPtr<IUserHandleHelper> uhHelper;
    Int32 userId;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&uhHelper);
    uhHelper->GetUserId(uid, &userId);
    //try {
    Int32 ownerUser = IUserHandle::USER_OWNER;
    if (userId == ownerUser) {
        return TRUE;
    }
    AutoPtr<IUserManager> um;
    AutoPtr<IUserManagerHelper> umHelper;
    CUserManagerHelper::AcquireSingleton((IUserManagerHelper**)&umHelper);
    umHelper->Get(mContext, (IUserManager**)&um);
    AutoPtr<IList> profiles;//UserInfo
    um->GetProfiles(ownerUser, (IList**)&profiles);
    Int32 size;
    profiles->GetSize(&size);
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> obj;
        profiles->Get(i, (IInterface**)&obj);
        IUserInfo* profile = IUserInfo::Probe(obj);
        Int32 id;
        profile->GetId(&id);
        if (userId == id) {
            return TRUE;
        }
    }
    //}
    //finally {
    binderHelper->RestoreCallingIdentity(ident);
    //}
    return FALSE;
}

void WifiServiceImpl::RegisterForScanModeChange()
{
    AutoPtr<IContentObserver> contentObserver = new InnerContentObserver1(this);

    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);

    AutoPtr<IUri> uri;
    Settings::Global::GetUriFor(ISettingsGlobal::WIFI_SCAN_ALWAYS_AVAILABLE, (IUri**)&uri);
    cr->RegisterContentObserver( uri, FALSE, contentObserver);
}

void WifiServiceImpl::RegisterForBroadcasts()
{
    AutoPtr<IIntentFilter> intentFilter;
    CIntentFilter::New((IIntentFilter**)&intentFilter);
    intentFilter->AddAction(IIntent::ACTION_SCREEN_ON);
    intentFilter->AddAction(IIntent::ACTION_USER_PRESENT);
    intentFilter->AddAction(IIntent::ACTION_SCREEN_OFF);
    intentFilter->AddAction(IIntent::ACTION_BATTERY_CHANGED);
    intentFilter->AddAction(IWifiManager::NETWORK_STATE_CHANGED_ACTION);
    intentFilter->AddAction(IWifiManager::WIFI_AP_STATE_CHANGED_ACTION);
    intentFilter->AddAction(IBluetoothAdapter::ACTION_CONNECTION_STATE_CHANGED);
    intentFilter->AddAction(ITelephonyIntents::ACTION_EMERGENCY_CALLBACK_MODE_CHANGED);
    AutoPtr<IIntent> tmp;
    mContext->RegisterReceiver(mReceiver, intentFilter, (IIntent**)&tmp);
}

void WifiServiceImpl::NoteAcquireWifiLock(
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

void WifiServiceImpl::NoteReleaseWifiLock(
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

Boolean WifiServiceImpl::AcquireWifiLockLocked(
    /* [in] */ WifiLock* wifiLock)
{
    if (DBG) Slogger::D(TAG, "acquireWifiLockLocked: ");// + wifiLock);

    mLocks->AddLock(wifiLock);

    AutoPtr<IBinderHelper> binderHelper;
    CBinderHelper::AcquireSingleton((IBinderHelper**)&binderHelper);
    Int64 ident;
    binderHelper->ClearCallingIdentity(&ident);
    //try {
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
    mWifiController->SendMessage(WifiController::CMD_LOCKS_CHANGED);
    //} catch (RemoteException e) {
    //    return false;
    //} finally {
    binderHelper->RestoreCallingIdentity(ident);
    //}
    return TRUE;
}

Boolean WifiServiceImpl::ReleaseWifiLockLocked(
    /* [in] */ IBinder* lock)
{
    Boolean hadLock;

    AutoPtr<WifiLock> wifiLock = mLocks->RemoveLock(lock);

    if (DBG) Slogger::D(TAG, "releaseWifiLockLocked: ");// + wifiLock);

    hadLock = (wifiLock != NULL);

    AutoPtr<IBinderHelper> binderHelper;
    CBinderHelper::AcquireSingleton((IBinderHelper**)&binderHelper);
    Int64 ident;
    binderHelper->ClearCallingIdentity(&ident);
    //try {
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
        mWifiController->SendMessage(WifiController::CMD_LOCKS_CHANGED);
    }
    //} catch (RemoteException e) {
    //} finally {
    binderHelper->RestoreCallingIdentity(ident);
    //}

    return hadLock;
}

void WifiServiceImpl::RemoveMulticasterLocked(
    /* [in] */ Int32 i,
    /* [in] */ Int32 uid)
{
    AutoPtr<IInterface> obj;
    mMulticasters->Remove(i, (IInterface**)&obj);
    AutoPtr<Multicaster> removed = (Multicaster*)(IObject::Probe(obj));

    if (removed != NULL) {
        removed->UnlinkDeathRecipient();
    }
    Int32 size;
    if ((mMulticasters->GetSize(&size), size) == 0) {
        mWifiStateMachine->StartFilteringMulticastV4Packets();
    }

    AutoPtr<IBinderHelper> binderHelper;
    CBinderHelper::AcquireSingleton((IBinderHelper**)&binderHelper);
    Int64 ident;
    binderHelper->ClearCallingIdentity(&ident);
    //try {
    mBatteryStats->NoteWifiMulticastDisabled(uid);
    //} catch (RemoteException e) {
    //} finally {
    binderHelper->RestoreCallingIdentity(ident);
    //}
}

} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namespace Elastos
