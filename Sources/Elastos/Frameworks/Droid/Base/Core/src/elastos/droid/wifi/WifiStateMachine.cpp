
#include "elastos/droid/wifi/WifiStateMachine.h"
#include "elastos/droid/wifi/SupplicantStateHelper.h"
#include "elastos/droid/wifi/WifiWatchdogStateMachine.h"
#include "elastos/droid/wifi/WifiApConfigStore.h"
#ifdef DROID_CORE
#include "elastos/droid/app/CPendingIntentHelper.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/content/CIntentFilter.h"
#include "elastos/droid/CInterfaceConfiguration.h"
#include "elastos/droid/CNetworkInfo.h"
#include "elastos/droid/CLinkProperties.h"
#include "elastos/droid/CLinkAddress.h"
#include "elastos/droid/wifi/CRssiPacketCountInfo.h"
#include "elastos/droid/wifi/CWifiInfo.h"
#include "elastos/droid/wifi/CWifiInfoHelper.h"
#include "elastos/droid/wifi/CWpsResult.h"
#include "elastos/droid/wifi/CWifiSsidHelper.h"
#include "elastos/droid/wifi/CScanResult.h"
#include "elastos/droid/wifi/CWifiManagerHelper.h"
#include "elastos/droid/os/CMessageHelper.h"
#include "elastos/droid/os/CMessenger.h"
#include "elastos/droid/os/CWorkSource.h"
#include "elastos/droid/os/CUserHandleHelper.h"
#include "elastos/droid/os/CSystemProperties.h"
#include "elastos/droid/utility/CParcelableObjectContainer.h"
#include "elastos/droid/provider/CSettingsSecure.h"
#include "elastos/droid/provider/CSettingsGlobal.h"
#endif
#include "elastos/droid/net/NetworkUtils.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/R.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/StringUtils.h>

using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Core::EIID_IRunnable;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Core::CThread;
using Elastos::Net::IInetAddress;
using Elastos::Core::StringUtils;
using Elastos::Utility::Concurrent::Atomic::CAtomicBoolean;
using Elastos::Utility::Concurrent::Atomic::CAtomicInteger32;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::R;
using Elastos::Droid::App::IPendingIntentHelper;
using Elastos::Droid::App::CPendingIntentHelper;
using Elastos::Droid::Bluetooth::IBluetoothAdapter;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Net::CInterfaceConfiguration;
using Elastos::Droid::Net::CNetworkInfo;
using Elastos::Droid::Net::NetworkUtils;
using Elastos::Droid::Net::CLinkAddress;
using Elastos::Droid::Wifi::IWifiManagerHelper;
using Elastos::Droid::Wifi::CWifiManagerHelper;
using Elastos::Droid::Wifi::P2p::IWifiP2pService;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::IMessageHelper;
using Elastos::Droid::Os::CMessageHelper;
using Elastos::Droid::Os::CMessenger;
using Elastos::Droid::Os::CWorkSource;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Utility::CParcelableObjectContainer;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::CSettingsSecure;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::CSettingsGlobal;

namespace Elastos {
namespace Droid {
namespace Wifi {

const Boolean WifiStateMachine::DBG = FALSE;
const String WifiStateMachine::TAG("WifiStateMachine");
const String WifiStateMachine::NETWORKTYPE("WIFI");

const Int32 WifiStateMachine::SCAN_RESULT_CACHE_SIZE;
const Int32 WifiStateMachine::POLL_RSSI_INTERVAL_MSECS;
const Int32 WifiStateMachine::SUPPLICANT_RESTART_INTERVAL_MSECS;
const Int32 WifiStateMachine::SUPPLICANT_RESTART_TRIES;
const Int32 WifiStateMachine::TETHER_NOTIFICATION_TIME_OUT_MSECS;
const Int32 WifiStateMachine::DRIVER_START_TIME_OUT_MSECS;
const Int32 WifiStateMachine::EVENTLOG_WIFI_STATE_CHANGED;
const Int32 WifiStateMachine::EVENTLOG_WIFI_EVENT_HANDLED;
const Int32 WifiStateMachine::EVENTLOG_SUPPLICANT_STATE_CHANGED;

const Int32 WifiStateMachine::BASE;
const Int32 WifiStateMachine::CMD_LOAD_DRIVER;
const Int32 WifiStateMachine::CMD_UNLOAD_DRIVER;
const Int32 WifiStateMachine::CMD_LOAD_DRIVER_SUCCESS;
const Int32 WifiStateMachine::CMD_LOAD_DRIVER_FAILURE;
const Int32 WifiStateMachine::CMD_UNLOAD_DRIVER_SUCCESS;
const Int32 WifiStateMachine::CMD_UNLOAD_DRIVER_FAILURE;
const Int32 WifiStateMachine::CMD_START_SUPPLICANT;
const Int32 WifiStateMachine::CMD_STOP_SUPPLICANT;
const Int32 WifiStateMachine::CMD_START_DRIVER;
const Int32 WifiStateMachine::CMD_STOP_DRIVER;
const Int32 WifiStateMachine::CMD_STATIC_IP_SUCCESS;
const Int32 WifiStateMachine::CMD_STATIC_IP_FAILURE;
const Int32 WifiStateMachine::CMD_STOP_SUPPLICANT_FAILED;
const Int32 WifiStateMachine::CMD_DELAYED_STOP_DRIVER;
const Int32 WifiStateMachine::CMD_DRIVER_START_TIMED_OUT;
const Int32 WifiStateMachine::CMD_CAPTIVE_CHECK_COMPLETE;
const Int32 WifiStateMachine::CMD_START_AP;
const Int32 WifiStateMachine::CMD_START_AP_SUCCESS;
const Int32 WifiStateMachine::CMD_START_AP_FAILURE;
const Int32 WifiStateMachine::CMD_STOP_AP;
const Int32 WifiStateMachine::CMD_SET_AP_CONFIG;
const Int32 WifiStateMachine::CMD_SET_AP_CONFIG_COMPLETED;
const Int32 WifiStateMachine::CMD_REQUEST_AP_CONFIG;
const Int32 WifiStateMachine::CMD_RESPONSE_AP_CONFIG;
const Int32 WifiStateMachine::CMD_TETHER_STATE_CHANGE;
const Int32 WifiStateMachine::CMD_TETHER_NOTIFICATION_TIMED_OUT;
const Int32 WifiStateMachine::CMD_BLUETOOTH_ADAPTER_STATE_CHANGE;
const Int32 WifiStateMachine::CMD_PING_SUPPLICANT;
const Int32 WifiStateMachine::CMD_ADD_OR_UPDATE_NETWORK;
const Int32 WifiStateMachine::CMD_REMOVE_NETWORK;
const Int32 WifiStateMachine::CMD_ENABLE_NETWORK;
const Int32 WifiStateMachine::CMD_ENABLE_ALL_NETWORKS;
const Int32 WifiStateMachine::CMD_BLACKLIST_NETWORK;
const Int32 WifiStateMachine::CMD_CLEAR_BLACKLIST;
const Int32 WifiStateMachine::CMD_SAVE_CONFIG;
const Int32 WifiStateMachine::CMD_GET_CONFIGURED_NETWORKS;
const Int32 WifiStateMachine::CMD_START_SCAN;
const Int32 WifiStateMachine::CMD_SET_SCAN_MODE;
const Int32 WifiStateMachine::CMD_SET_SCAN_TYPE;
const Int32 WifiStateMachine::CMD_DISCONNECT;
const Int32 WifiStateMachine::CMD_RECONNECT;
const Int32 WifiStateMachine::CMD_REASSOCIATE;
const Int32 WifiStateMachine::CMD_SET_HIGH_PERF_MODE;
const Int32 WifiStateMachine::CMD_SET_COUNTRY_CODE;
const Int32 WifiStateMachine::CMD_ENABLE_RSSI_POLL;
const Int32 WifiStateMachine::CMD_RSSI_POLL;
const Int32 WifiStateMachine::CMD_START_PACKET_FILTERING;
const Int32 WifiStateMachine::CMD_STOP_PACKET_FILTERING;
const Int32 WifiStateMachine::CMD_SET_SUSPEND_OPT_ENABLED;
const Int32 WifiStateMachine::CMD_NO_NETWORKS_PERIODIC_SCAN;
const Int32 WifiStateMachine::MULTICAST_V6;
const Int32 WifiStateMachine::MULTICAST_V4;
const Int32 WifiStateMachine::CMD_SET_FREQUENCY_BAND;
const Int32 WifiStateMachine::CMD_ENABLE_BACKGROUND_SCAN;
const Int32 WifiStateMachine::CMD_RESET_SUPPLICANT_STATE;
const Int32 WifiStateMachine::CMD_ENABLE_P2P;
const Int32 WifiStateMachine::CMD_DISABLE_P2P_REQ;
const Int32 WifiStateMachine::CMD_DISABLE_P2P_RSP;
const Int32 WifiStateMachine::CONNECT_MODE;
const Int32 WifiStateMachine::SCAN_ONLY_MODE;
const Int32 WifiStateMachine::SCAN_ACTIVE;
const Int32 WifiStateMachine::SCAN_PASSIVE;
const Int32 WifiStateMachine::SUCCESS;
const Int32 WifiStateMachine::FAILURE;
const Int32 WifiStateMachine::IN_ECM_STATE;
const Int32 WifiStateMachine::NOT_IN_ECM_STATE;
const Int32 WifiStateMachine::DEFAULT_MAX_DHCP_RETRIES;
const Int32 WifiStateMachine::SUSPEND_DUE_TO_DHCP;
const Int32 WifiStateMachine::SUSPEND_DUE_TO_HIGH_PERF;
const Int32 WifiStateMachine::SUSPEND_DUE_TO_SCREEN;
const Int32 WifiStateMachine::MIN_INTERVAL_ENABLE_ALL_NETWORKS_MS;
const Int32 WifiStateMachine::MIN_RSSI;
const Int32 WifiStateMachine::MAX_RSSI;
const Int32 WifiStateMachine::SCAN_REQUEST;
String WifiStateMachine::ACTION_START_SCAN("com.android.server.WifiManager.action.START_SCAN");
String WifiStateMachine::DELAYED_STOP_COUNTER("DelayedStopCounter");
const Int32 WifiStateMachine::DRIVER_STOP_REQUEST;
String WifiStateMachine::ACTION_DELAYED_DRIVER_STOP("com.android.server.WifiManager.action.DELAYED_DRIVER_STOP");
const String WifiStateMachine::BSSID_STR("bssid=");
const String WifiStateMachine::FREQ_STR("freq=");
const String WifiStateMachine::LEVEL_STR("level=");
const String WifiStateMachine::TSF_STR("tsf=");
const String WifiStateMachine::FLAGS_STR("flags=");
const String WifiStateMachine::SSID_STR("ssid=");
const String WifiStateMachine::DELIMITER_STR("====");


static void DebugProcessMessage(const char* tag, Int32 what)
{
    if (what == WifiStateMachine::CMD_RSSI_POLL) {
        return;
    }

    if (WifiStateMachine::DBG) {
        static HashMap<Int32, String> map;
        if (map.IsEmpty()) {
            map[WifiMonitor::SCAN_RESULTS_EVENT] = String("ProcessMessage SCAN_RESULTS_EVENT");

            map[DhcpStateMachine::CMD_PRE_DHCP_ACTION] = String("ProcessMessage CMD_PRE_DHCP_ACTION");
            map[DhcpStateMachine::CMD_POST_DHCP_ACTION] = String("ProcessMessage CMD_POST_DHCP_ACTION");

            map[WifiStateMachine::CMD_LOAD_DRIVER] = String("ProcessMessage CMD_LOAD_DRIVER");
            map[WifiStateMachine::CMD_STOP_DRIVER] = String("ProcessMessage CMD_STOP_DRIVER");
            map[WifiStateMachine::CMD_START_SUPPLICANT] = String("ProcessMessage CMD_START_SUPPLICANT");
            map[WifiStateMachine::CMD_RSSI_POLL] = String("ProcessMessage CMD_RSSI_POLL");
            map[WifiStateMachine::CMD_SAVE_CONFIG] = String("ProcessMessage CMD_SAVE_CONFIG");
            map[WifiStateMachine::CMD_START_SCAN] = String("ProcessMessage CMD_START_SCAN");
            map[WifiStateMachine::CMD_GET_CONFIGURED_NETWORKS] = String("ProcessMessage CMD_GET_CONFIGURED_NETWORKS");
            map[WifiStateMachine::CMD_NO_NETWORKS_PERIODIC_SCAN] = String("ProcessMessage CMD_NO_NETWORKS_PERIODIC_SCAN");
            map[WifiStateMachine::CMD_CAPTIVE_CHECK_COMPLETE] = String("ProcessMessage CMD_CAPTIVE_CHECK_COMPLETE");
            map[WifiStateMachine::CMD_SET_FREQUENCY_BAND] = String("ProcessMessage CMD_SET_FREQUENCY_BAND");
            map[WifiStateMachine::CMD_ENABLE_RSSI_POLL] = String("ProcessMessage CMD_ENABLE_RSSI_POLL");
            map[WifiStateMachine::CMD_SET_SUSPEND_OPT_ENABLED] = String("ProcessMessage CMD_SET_SUSPEND_OPT_ENABLED");
            map[WifiStateMachine::CMD_ENABLE_ALL_NETWORKS] = String("ProcessMessage CMD_ENABLE_ALL_NETWORKS");

            map[WifiMonitor::NETWORK_CONNECTION_EVENT] = String("ProcessMessage NETWORK_CONNECTION_EVENT");
            map[WifiMonitor::SUPPLICANT_STATE_CHANGE_EVENT] = String("ProcessMessage SUPPLICANT_STATE_CHANGE_EVENT");
            map[WifiMonitor::SUP_CONNECTION_EVENT] = String("ProcessMessage SUP_CONNECTION_EVENT");

            map[WifiWatchdogStateMachine::EVENT_RSSI_CHANGE] = String("ProcessMessage EVENT_RSSI_CHANGE");
            map[WifiWatchdogStateMachine::POOR_LINK_DETECTED] = String("ProcessMessage POOR_LINK_DETECTED");
            map[WifiWatchdogStateMachine::GOOD_LINK_DETECTED] = String("ProcessMessage GOOD_LINK_DETECTED");

            map[IWifiP2pService::P2P_CONNECTION_CHANGED] = String("ProcessMessage IWifiP2pService::P2P_CONNECTION_CHANGED");
            map[IWifiP2pService::DISCONNECT_WIFI_REQUEST] = String("ProcessMessage IWifiP2pService::DISCONNECT_WIFI_REQUEST");

            map[AsyncChannel::CMD_CHANNEL_HALF_CONNECTED] = String("ProcessMessage AsyncChannel::CMD_CHANNEL_HALF_CONNECTED");
            map[AsyncChannel::CMD_CHANNEL_FULL_CONNECTION] = String("ProcessMessage AsyncChannel::CMD_CHANNEL_FULL_CONNECTION");
            map[AsyncChannel::CMD_CHANNEL_FULLY_CONNECTED] = String("ProcessMessage AsyncChannel::CMD_CHANNEL_FULLY_CONNECTED");
            map[AsyncChannel::CMD_CHANNEL_DISCONNECT] = String("ProcessMessage AsyncChannel::CMD_CHANNEL_DISCONNECT");
            map[AsyncChannel::CMD_CHANNEL_DISCONNECTED] = String("ProcessMessage AsyncChannel::CMD_CHANNEL_DISCONNECTED");
        }

        HashMap<Int32, String>::Iterator it = map.Find(what);
        if (it != map.End()) {
            Logger::D(tag, it->mSecond.string());
        }
        else {
            Logger::D(tag, "ProcessMessage %08x", what);
        }
    }
}

//==============================================================
// WifiStateMachine::DefaultState
//==============================================================
ECode WifiStateMachine::DefaultState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    Int32 what, arg1;
    msg->GetWhat(&what);
    msg->GetArg1(&arg1);
    DebugProcessMessage("WifiStateMachine::DefaultState", what);
    switch (what) {
        case AsyncChannel::CMD_CHANNEL_HALF_CONNECTED: {
            //Logger::D("WifiStateMachine::DefaultState", " >> ProcessMessage CMD_CHANNEL_HALF_CONNECTED");
            if (arg1 == AsyncChannel::STATUS_SUCCESSFUL) {
                mOwner->mWifiP2pChannel->SendMessage(AsyncChannel::CMD_CHANNEL_FULL_CONNECTION);
            }
            else {
                mOwner->Loge(String("WifiP2pService connection failure, error=") + StringUtils::Int32ToString(arg1));
            }
            break;
        }
        case AsyncChannel::CMD_CHANNEL_DISCONNECTED: {
            mOwner->Loge(String("WifiP2pService channel lost, message.arg1 =") + StringUtils::Int32ToString(arg1));
            //TODO: Re-establish connection to state machine after a delay
            //mWifiP2pChannel.connect(mContext, getHandler(), mWifiP2pManager.getMessenger());
            break;
        }
        case CMD_BLUETOOTH_ADAPTER_STATE_CHANGE: {
            mOwner->mBluetoothConnectionActive = (arg1 !=
                    IBluetoothAdapter::STATE_DISCONNECTED);
            break;
            /* Synchronous call returns */
        }
        case CMD_PING_SUPPLICANT:
        case CMD_ENABLE_NETWORK:
        case CMD_ADD_OR_UPDATE_NETWORK:
        case CMD_REMOVE_NETWORK:
        case CMD_SAVE_CONFIG:
            mOwner->ReplyToMessage(msg, what, FAILURE);
            break;
        case CMD_GET_CONFIGURED_NETWORKS:
            mOwner->ReplyToMessage(msg, what, (IInterface*)NULL);
            break;
        case CMD_ENABLE_RSSI_POLL: {
            mOwner->mEnableRssiPolling = (arg1 == 1);
            break;
        }
        case CMD_ENABLE_BACKGROUND_SCAN: {
            mOwner->mEnableBackgroundScan = (arg1 == 1);
            break;
        }
        case CMD_SET_HIGH_PERF_MODE: {
            if (arg1 == 1) {
                mOwner->SetSuspendOptimizations(SUSPEND_DUE_TO_HIGH_PERF, FALSE);
            }
            else {
                mOwner->SetSuspendOptimizations(SUSPEND_DUE_TO_HIGH_PERF, TRUE);
            }
            break;
            /* Discard */
        }
        case CMD_LOAD_DRIVER:
        case CMD_UNLOAD_DRIVER:
        case CMD_START_SUPPLICANT:
        case CMD_STOP_SUPPLICANT:
        case CMD_STOP_SUPPLICANT_FAILED:
        case CMD_START_DRIVER:
        case CMD_STOP_DRIVER:
        case CMD_DELAYED_STOP_DRIVER:
        case CMD_DRIVER_START_TIMED_OUT:
        case CMD_CAPTIVE_CHECK_COMPLETE:
        case CMD_START_AP:
        case CMD_START_AP_SUCCESS:
        case CMD_START_AP_FAILURE:
        case CMD_STOP_AP:
        case CMD_TETHER_STATE_CHANGE:
        case CMD_TETHER_NOTIFICATION_TIMED_OUT:
        case CMD_START_SCAN:
        case CMD_DISCONNECT:
        case CMD_RECONNECT:
        case CMD_REASSOCIATE:
        case WifiMonitor::SUP_CONNECTION_EVENT:
        case WifiMonitor::SUP_DISCONNECTION_EVENT:
        case WifiMonitor::NETWORK_CONNECTION_EVENT:
        case WifiMonitor::NETWORK_DISCONNECTION_EVENT:
        case WifiMonitor::SCAN_RESULTS_EVENT:
        case WifiMonitor::SUPPLICANT_STATE_CHANGE_EVENT:
        case WifiMonitor::AUTHENTICATION_FAILURE_EVENT:
        case WifiMonitor::WPS_OVERLAP_EVENT:
        case CMD_BLACKLIST_NETWORK:
        case CMD_CLEAR_BLACKLIST:
        case CMD_SET_SCAN_MODE:
        case CMD_SET_SCAN_TYPE:
        case CMD_SET_COUNTRY_CODE:
        case CMD_SET_FREQUENCY_BAND:
        case CMD_RSSI_POLL:
        case CMD_ENABLE_ALL_NETWORKS:
        case DhcpStateMachine::CMD_PRE_DHCP_ACTION:
        case DhcpStateMachine::CMD_POST_DHCP_ACTION:
        /* Handled by WifiApConfigStore */
        case CMD_SET_AP_CONFIG:
        case CMD_SET_AP_CONFIG_COMPLETED:
        case CMD_REQUEST_AP_CONFIG:
        case CMD_RESPONSE_AP_CONFIG:
        case WifiWatchdogStateMachine::POOR_LINK_DETECTED:
        case WifiWatchdogStateMachine::GOOD_LINK_DETECTED:
        case CMD_NO_NETWORKS_PERIODIC_SCAN:
            break;
        case DhcpStateMachine::CMD_ON_QUIT:
            mOwner->mDhcpStateMachine = NULL;
            break;
        case CMD_SET_SUSPEND_OPT_ENABLED: {
            if (arg1 == 1) {
                mOwner->mSuspendWakeLock->ReleaseLock();
                mOwner->SetSuspendOptimizations(SUSPEND_DUE_TO_SCREEN, TRUE);
            }
            else {
                mOwner->SetSuspendOptimizations(SUSPEND_DUE_TO_SCREEN, FALSE);
            }
            break;
        }
        case WifiMonitor::DRIVER_HUNG_EVENT:
            mOwner->SetWifiEnabled(FALSE);
            mOwner->SetWifiEnabled(TRUE);
            break;
        case IWifiManager::CONNECT_NETWORK:
            mOwner->ReplyToMessage(msg, IWifiManager::CONNECT_NETWORK_FAILED,
                    IWifiManager::BUSY);
            break;
        case IWifiManager::FORGET_NETWORK:
            mOwner->ReplyToMessage(msg, IWifiManager::FORGET_NETWORK_FAILED,
                    IWifiManager::BUSY);
            break;
        case IWifiManager::SAVE_NETWORK:
            mOwner->ReplyToMessage(msg, IWifiManager::SAVE_NETWORK_FAILED,
                    IWifiManager::BUSY);
            break;
        case IWifiManager::START_WPS:
            mOwner->ReplyToMessage(msg, IWifiManager::WPS_FAILED,
                    IWifiManager::BUSY);
            break;
        case IWifiManager::CANCEL_WPS:
            mOwner->ReplyToMessage(msg, IWifiManager::CANCEL_WPS_FAILED,
                    IWifiManager::BUSY);
            break;
        case IWifiManager::DISABLE_NETWORK:
            mOwner->ReplyToMessage(msg, IWifiManager::DISABLE_NETWORK_FAILED,
                    IWifiManager::BUSY);
            break;
        case IWifiManager::RSSI_PKTCNT_FETCH:
            mOwner->ReplyToMessage(msg, IWifiManager::RSSI_PKTCNT_FETCH_FAILED,
                    IWifiManager::BUSY);
            break;
        case IWifiP2pService::P2P_CONNECTION_CHANGED: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            AutoPtr<INetworkInfo> info = INetworkInfo::Probe(obj);
            Boolean isConnected;
            info->IsConnected(&isConnected);
            mOwner->mP2pConnected->Set(isConnected);
            break;
        }
       case IWifiP2pService::DISCONNECT_WIFI_REQUEST:
           mOwner->mTemporarilyDisconnectWifi = (arg1 == 1);
           mOwner->ReplyToMessage(msg, IWifiP2pService::DISCONNECT_WIFI_RESPONSE);
           break;
        default:
            mOwner->Loge(String("Error! unhandled message")/* + msg*/);
            break;
    }
    *result = HANDLED;
    return NOERROR;
}


//==============================================================
// WifiStateMachine::InitialState
//==============================================================
ECode WifiStateMachine::InitialState::Enter()
{
    // if (DBG) mOwner->Log(GetName() + "\n");
    if (DBG) mOwner->Log(String("InitialState::Enter"));
    // [31-8] Reserved for future use
    // [7 - 0] HSM state change
    // 50021 wifi_state_changed (custom|1|5)
    // EventLog.writeEvent(EVENTLOG_WIFI_STATE_CHANGED, getName());

    if (mOwner->mWifiNative->IsDriverLoaded()) {
        mOwner->TransitionTo(mOwner->mDriverLoadedState);
    }
    else {
        mOwner->TransitionTo(mOwner->mDriverUnloadedState);
    }

    //Connect to WifiP2pService
    AutoPtr<IInterface> obj;
    mOwner->mContext->GetSystemService(IContext::WIFI_P2P_SERVICE, (IInterface**)&obj);
    mOwner->mWifiP2pManager = IWifiP2pManager::Probe(obj);
    AutoPtr<IHandler> handler = mOwner->GetHandler();
    AutoPtr<IMessenger> messenger;
    mOwner->mWifiP2pManager->GetMessenger((IMessenger**)&messenger);
    mOwner->mWifiP2pChannel->Connect(mOwner->mContext, handler, messenger);

    /* IPv6 is disabled at boot time and is controlled by framework
     * to be enabled only as long as we are connected to an access point
     *
     * This fixes issues, a few being:
     * - IPv6 addresses and routes stick around after disconnection
     * - When connected, the kernel is unaware and can fail to start IPv6 negotiation
     * - The kernel sometimes starts autoconfiguration when 802.1x is not complete
     */
    // try {
    if (mOwner->mNwService) {
        mOwner->mNwService->DisableIpv6(mOwner->mInterfaceName);
    }
    // } catch (RemoteException re) {
    //     loge("Failed to disable IPv6: " + re);
    // } catch (IllegalStateException e) {
    //     loge("Failed to disable IPv6: " + e);
    // }
    return NOERROR;
}


//==============================================================
// WifiStateMachine::DriverLoadingState::Runnable
//==============================================================
CAR_INTERFACE_IMPL(WifiStateMachine::DriverLoadingState::Runnable, IRunnable)

ECode WifiStateMachine::DriverLoadingState::Runnable::Run()
{
    mOwner->mWakeLock->AcquireLock();
    //enabling state
    switch(mArg) {
        case IWifiManager::WIFI_STATE_ENABLING:
            mOwner->SetWifiState(IWifiManager::WIFI_STATE_ENABLING);
            break;
        case IWifiManager::WIFI_AP_STATE_ENABLING:
            mOwner->SetWifiApState(IWifiManager::WIFI_AP_STATE_ENABLING);
            break;
    }

    if(mOwner->mWifiNative->LoadDriver()) {
        if (DBG) mOwner->Log(String("Driver load successful"));
        mOwner->SendMessage(CMD_LOAD_DRIVER_SUCCESS);
    }
    else {
        mOwner->Loge(String("Failed to load driver!"));
        switch(mArg) {
            case IWifiManager::WIFI_STATE_ENABLING:
                mOwner->SetWifiState(IWifiManager::WIFI_STATE_UNKNOWN);
                break;
            case IWifiManager::WIFI_AP_STATE_ENABLING:
                mOwner->SetWifiApState(IWifiManager::WIFI_AP_STATE_FAILED);
                break;
        }
        mOwner->SendMessage(CMD_LOAD_DRIVER_FAILURE);
    }
    mOwner->mWakeLock->ReleaseLock();
    return NOERROR;
}


//==============================================================
// WifiStateMachine::DriverLoadingState
//==============================================================
ECode WifiStateMachine::DriverLoadingState::Enter()
{
    // if (DBG) log(getName() + "\n");
    if (DBG) mOwner->Log(String("DriverLoadingState::Enter"));
    // EventLog.writeEvent(EVENTLOG_WIFI_STATE_CHANGED, getName());

    AutoPtr<IMessage> msg = mOwner->GetCurrentMessage();
    Int32 arg1;
    msg->GetArg1(&arg1);
    /* TODO: add a timeout to fail when driver load is hung.
     * Similarly for driver unload.
     */
    AutoPtr<IRunnable> r = new DriverLoadingState::Runnable(arg1, mOwner);
    AutoPtr<IThread> t;
    CThread::New(r, (IThread**)&t);
    t->Start();
    return NOERROR;
}

ECode WifiStateMachine::DriverLoadingState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    Int32 what;
    msg->GetWhat(&what);
    DebugProcessMessage("WifiStateMachine::DriverLoadingState", what);
    switch (what) {
        case CMD_LOAD_DRIVER_SUCCESS:
            mOwner->TransitionTo(mOwner->mDriverLoadedState);
            break;
        case CMD_LOAD_DRIVER_FAILURE:
            mOwner->TransitionTo(mOwner->mDriverFailedState);
            break;
        case CMD_LOAD_DRIVER:
        case CMD_UNLOAD_DRIVER:
        case CMD_START_SUPPLICANT:
        case CMD_STOP_SUPPLICANT:
        case CMD_START_AP:
        case CMD_STOP_AP:
        case CMD_START_DRIVER:
        case CMD_STOP_DRIVER:
        case CMD_SET_SCAN_MODE:
        case CMD_SET_SCAN_TYPE:
        case CMD_SET_COUNTRY_CODE:
        case CMD_SET_FREQUENCY_BAND:
        case CMD_START_PACKET_FILTERING:
        case CMD_STOP_PACKET_FILTERING:
            mOwner->DeferMessage(msg);
            break;
        default:
            *result = NOT_HANDLED;
            return NOERROR;
    }
    *result = HANDLED;
    return NOERROR;
}


//==============================================================
// WifiStateMachine::DriverLoadedState
//==============================================================
ECode WifiStateMachine::DriverLoadedState::Enter()
{
    // if (DBG) log(getName() + "\n");
    if (DBG) mOwner->Log(String("DriverLoadedState::Enter"));
    // EventLog.writeEvent(EVENTLOG_WIFI_STATE_CHANGED, getName());
    return NOERROR;
}

ECode WifiStateMachine::DriverLoadedState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    Int32 what;
    msg->GetWhat(&what);
    DebugProcessMessage("WifiStateMachine::DriverLoadedState", what);
    ECode ec = NOERROR;
    switch(what) {
        case CMD_UNLOAD_DRIVER:
            mOwner->TransitionTo(mOwner->mDriverUnloadingState);
            break;
        case CMD_START_SUPPLICANT:

            ec = mOwner->mNwService->WifiFirmwareReload(mOwner->mInterfaceName, String("STA"));
            if (FAILED(ec)) {
                Logger::E("WifiStateMachine::DriverLoadedState",
                    "Failed to reload STA firmware, ec=%08x", ec);
            }

            //A runtime crash can leave the interface up and
            //this affects connectivity when supplicant starts up.
            //Ensure interface is down before a supplicant start.

            mOwner->mNwService->SetInterfaceDown(mOwner->mInterfaceName);
            //Set privacy extensions
            ec = mOwner->mNwService->SetInterfaceIpv6PrivacyExtensions(mOwner->mInterfaceName, TRUE);
            if (FAILED(ec)) {
                Logger::E("WifiStateMachine::DriverLoadedState",
                    "Unable to change interface settings, ec=%08x", ec);
            }

            /* Stop a running supplicant after a runtime restart
             * Avoids issues with drivers that do not handle interface down
             * on a running supplicant properly.
             */
            if (DBG) mOwner->Log(String("Kill any running supplicant"));
            mOwner->mWifiNative->KillSupplicant(mOwner->mP2pSupported);

            if (mOwner->mWifiNative->StartSupplicant(mOwner->mP2pSupported)) {
                if (DBG) mOwner->Log(String("Supplicant start successful"));
                mOwner->mWifiMonitor->StartMonitoring();
                mOwner->TransitionTo(mOwner->mSupplicantStartingState);
            }
            else {
                mOwner->Loge(String("Failed to start supplicant!"));
                AutoPtr<IMessage> m = mOwner->ObtainMessage(CMD_UNLOAD_DRIVER, IWifiManager::WIFI_STATE_UNKNOWN, 0);
                mOwner->SendMessage(m);
            }
            break;
        case CMD_START_AP:
            mOwner->TransitionTo(mOwner->mSoftApStartingState);
            break;
        default:
            *result = NOT_HANDLED;
            return NOERROR;
    }
    *result = HANDLED;
    return NOERROR;
}


//==============================================================
// WifiStateMachine::DriverUnloadingState::Runnable
//==============================================================
CAR_INTERFACE_IMPL(WifiStateMachine::DriverUnloadingState::Runnable, IRunnable)

ECode WifiStateMachine::DriverUnloadingState::Runnable::Run()
{
    // if (DBG) log(getName() + message.toString() + "\n");
    if (DBG) mOwner->Log(String("DriverUnloadingState::Runnable::Run"));
    mOwner->mWakeLock->AcquireLock();
    if(mOwner->mWifiNative->UnloadDriver()) {
        if (DBG) mOwner->Log(String("Driver unload successful"));
        mOwner->SendMessage(CMD_UNLOAD_DRIVER_SUCCESS);

        switch(mArg) {
            case IWifiManager::WIFI_STATE_DISABLED:
            case IWifiManager::WIFI_STATE_UNKNOWN:
                mOwner->SetWifiState(mArg);
                break;
            case IWifiManager::WIFI_AP_STATE_DISABLED:
            case IWifiManager::WIFI_AP_STATE_FAILED:
                mOwner->SetWifiApState(mArg);
                break;
        }
    }
    else {
        mOwner->Loge(String("Failed to unload driver!"));
        mOwner->SendMessage(CMD_UNLOAD_DRIVER_FAILURE);

        switch(mArg) {
            case IWifiManager::WIFI_STATE_DISABLED:
            case IWifiManager::WIFI_STATE_UNKNOWN:
                mOwner->SetWifiState(IWifiManager::WIFI_STATE_UNKNOWN);
                break;
            case IWifiManager::WIFI_AP_STATE_DISABLED:
            case IWifiManager::WIFI_AP_STATE_FAILED:
                mOwner->SetWifiApState(IWifiManager::WIFI_AP_STATE_FAILED);
                break;
        }
    }
    mOwner->mWakeLock->ReleaseLock();
    return NOERROR;
}


//==============================================================
// WifiStateMachine::DriverUnloadingState
//==============================================================
ECode WifiStateMachine::DriverUnloadingState::Enter()
{
    // if (DBG) log(getName() + "\n");
    if (DBG) mOwner->Log(String("DriverUnloadingState::Enter"));
    // EventLog.writeEvent(EVENTLOG_WIFI_STATE_CHANGED, getName());

    AutoPtr<IMessage> msg = mOwner->GetCurrentMessage();
    Int32 arg1;
    msg->GetArg1(&arg1);
    AutoPtr<IRunnable> r = new Runnable(arg1, mOwner);
    AutoPtr<IThread> t;
    CThread::New(r, (IThread**)&t);
    t->Start();
    return NOERROR;
}

ECode WifiStateMachine::DriverUnloadingState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    Int32 what;
    msg->GetWhat(&what);
    DebugProcessMessage("WifiStateMachine::DriverUnloadingState", what);
    switch (what) {
        case CMD_UNLOAD_DRIVER_SUCCESS:
            mOwner->TransitionTo(mOwner->mDriverUnloadedState);
            break;
        case CMD_UNLOAD_DRIVER_FAILURE:
            mOwner->TransitionTo(mOwner->mDriverFailedState);
            break;
        case CMD_LOAD_DRIVER:
        case CMD_UNLOAD_DRIVER:
        case CMD_START_SUPPLICANT:
        case CMD_STOP_SUPPLICANT:
        case CMD_START_AP:
        case CMD_STOP_AP:
        case CMD_START_DRIVER:
        case CMD_STOP_DRIVER:
        case CMD_SET_SCAN_MODE:
        case CMD_SET_SCAN_TYPE:
        case CMD_SET_COUNTRY_CODE:
        case CMD_SET_FREQUENCY_BAND:
        case CMD_START_PACKET_FILTERING:
        case CMD_STOP_PACKET_FILTERING:
            mOwner->DeferMessage(msg);
            break;
        default:
            *result = NOT_HANDLED;
            return NOERROR;
    }
    *result = HANDLED;
    return NOERROR;
}


//==============================================================
// WifiStateMachine::DriverUnloadedState
//==============================================================
ECode WifiStateMachine::DriverUnloadedState::Enter()
{
    // if (DBG) log(getName() + "\n");
    if (DBG) mOwner->Log(String("DriverUnloadedState::Enter"));
    // EventLog.writeEvent(EVENTLOG_WIFI_STATE_CHANGED, getName());
    return NOERROR;
}

ECode WifiStateMachine::DriverUnloadedState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    Int32 what;
    msg->GetWhat(&what);
    DebugProcessMessage("WifiStateMachine::DriverUnloadedState", what);
    switch (what) {
        case CMD_LOAD_DRIVER:
            mOwner->TransitionTo(mOwner->mDriverLoadingState);
            break;
        default:
            *result = NOT_HANDLED;
            return NOERROR;
    }
    *result = HANDLED;
    return NOERROR;
}


//==============================================================
// WifiStateMachine::DriverFailedState
//==============================================================
ECode WifiStateMachine::DriverFailedState::Enter()
{
    // loge(getName() + "\n");
    if (DBG) mOwner->Log(String("DriverFailedState::Enter"));
    // EventLog.writeEvent(EVENTLOG_WIFI_STATE_CHANGED, getName());
    return NOERROR;
}

ECode WifiStateMachine::DriverFailedState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    // if (DBG) log(getName() + message.toString() + "\n");
    if (DBG) mOwner->Log(String("DriverFailedState::ProcessMessage"));
    *result = NOT_HANDLED;
    return NOERROR;
}


//==============================================================
// WifiStateMachine::SupplicantStartingState
//==============================================================
ECode WifiStateMachine::SupplicantStartingState::Enter()
{
    // if (DBG) log(getName() + "\n");
    if (DBG) mOwner->Log(String("SupplicantStartingState::Enter"));
    // EventLog.writeEvent(EVENTLOG_WIFI_STATE_CHANGED, getName());
    return NOERROR;
}

void WifiStateMachine::SupplicantStartingState::InitializeWpsDetails()
{
    String detail;
    AutoPtr<ISystemProperties> sysProp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
    sysProp->Get(String("ro.product.name"), String(""), &detail);
    if (!mOwner->mWifiNative->SetDeviceName(detail)) {
        mOwner->Loge(String("Failed to set device name ") +  detail);
    }
    sysProp->Get(String("ro.product.manufacturer"), String(""), &detail);
    if (!mOwner->mWifiNative->SetManufacturer(detail)) {
        mOwner->Loge(String("Failed to set manufacturer ") + detail);
    }
    sysProp->Get(String("ro.product.model"), String(""), &detail);
    if (!mOwner->mWifiNative->SetModelName(detail)) {
        mOwner->Loge(String("Failed to set model name ") + detail);
    }
    sysProp->Get(String("ro.product.model"), String(""), &detail);
    if (!mOwner->mWifiNative->SetModelNumber(detail)) {
        mOwner->Loge(String("Failed to set model number ") + detail);
    }
    sysProp->Get(String("ro.serialno"), String(""), &detail);
    if (!mOwner->mWifiNative->SetSerialNumber(detail)) {
        mOwner->Loge(String("Failed to set serial number ") + detail);
    }
    if (!mOwner->mWifiNative->SetConfigMethods(String("physical_display virtual_push_button keypad"))) {
        mOwner->Loge(String("Failed to set WPS config methods"));
    }
    if (!mOwner->mWifiNative->SetDeviceType(mOwner->mPrimaryDeviceType)) {
        mOwner->Loge(String("Failed to set primary device type ") + mOwner->mPrimaryDeviceType);
    }
}

ECode WifiStateMachine::SupplicantStartingState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    Int32 what;
    msg->GetWhat(&what);
    DebugProcessMessage("WifiStateMachine::SupplicantStartingState", what);
    switch(what) {
        case WifiMonitor::SUP_CONNECTION_EVENT:
            if (DBG) mOwner->Log(String("Supplicant connection established"));
            mOwner->SetWifiState(IWifiManager::WIFI_STATE_ENABLED);
            mOwner->mSupplicantRestartCount = 0;
            /* Reset the supplicant state to indicate the supplicant
             * state is not known at this time */
            mOwner->mSupplicantStateTracker->SendMessage(CMD_RESET_SUPPLICANT_STATE);
            /* Initialize data structures */
            mOwner->mLastBssid = NULL;
            mOwner->mLastNetworkId = IWifiConfiguration::INVALID_NETWORK_ID;
            mOwner->mLastSignalLevel = -1;

            mOwner->mWifiInfo->SetMacAddress(mOwner->mWifiNative->GetMacAddress());
            mOwner->mWifiConfigStore->Initialize();
            InitializeWpsDetails();

            mOwner->SendSupplicantConnectionChangedBroadcast(TRUE);
            mOwner->TransitionTo(mOwner->mDriverStartedState);
            break;
        case WifiMonitor::SUP_DISCONNECTION_EVENT:
            if (++mOwner->mSupplicantRestartCount <= SUPPLICANT_RESTART_TRIES) {
                mOwner->Loge(String("Failed to setup control channel, restart supplicant"));
                mOwner->mWifiNative->KillSupplicant(mOwner->mP2pSupported);
                mOwner->TransitionTo(mOwner->mDriverLoadedState);
                mOwner->SendMessageDelayed(CMD_START_SUPPLICANT, SUPPLICANT_RESTART_INTERVAL_MSECS);
            }
            else {
                mOwner->Loge(String("Failed ") + StringUtils::Int32ToString(mOwner->mSupplicantRestartCount) +
                        String(" times to start supplicant, unload driver"));
                mOwner->mSupplicantRestartCount = 0;
                mOwner->TransitionTo(mOwner->mDriverLoadedState);
                AutoPtr<IMessage> m = mOwner->ObtainMessage(CMD_UNLOAD_DRIVER, IWifiManager::WIFI_STATE_UNKNOWN, 0);
                mOwner->SendMessage(m);
            }
            break;
        case CMD_LOAD_DRIVER:
        case CMD_UNLOAD_DRIVER:
        case CMD_START_SUPPLICANT:
        case CMD_STOP_SUPPLICANT:
        case CMD_START_AP:
        case CMD_STOP_AP:
        case CMD_START_DRIVER:
        case CMD_STOP_DRIVER:
        case CMD_SET_SCAN_MODE:
        case CMD_SET_SCAN_TYPE:
        case CMD_SET_COUNTRY_CODE:
        case CMD_SET_FREQUENCY_BAND:
        case CMD_START_PACKET_FILTERING:
        case CMD_STOP_PACKET_FILTERING:
            mOwner->DeferMessage(msg);
            break;
        default:
            *result = NOT_HANDLED;
            return NOERROR;
    }
    *result = HANDLED;
    return NOERROR;
}


//==============================================================
// WifiStateMachine::SupplicantStartedState
//==============================================================
ECode WifiStateMachine::SupplicantStartedState::Enter()
{
    // if (DBG) log(getName() + "\n");
    if (DBG) mOwner->Log(String("SupplicantStartedState::Enter"));
    // EventLog.writeEvent(EVENTLOG_WIFI_STATE_CHANGED, getName());
    /* Initialize for connect mode operation at start */
    mOwner->mIsScanMode = FALSE;
    /* Wifi is available as long as we have a connection to supplicant */
    mOwner->mNetworkInfo->SetIsAvailable(TRUE);

    AutoPtr<IResources> r;
    mOwner->mContext->GetResources((IResources**)&r);
    Int32 defaultInterval;
    r->GetInteger(R::integer::config_wifi_supplicant_scan_interval, &defaultInterval);

    AutoPtr<ISettingsGlobal> global;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
    AutoPtr<IContentResolver> resolver;
    mOwner->mContext->GetContentResolver((IContentResolver**)&resolver);
    global->GetInt64(resolver, ISettingsGlobal::WIFI_SUPPLICANT_SCAN_INTERVAL_MS,
        defaultInterval, &mOwner->mSupplicantScanIntervalMs);
    mOwner->mWifiNative->SetScanInterval((Int32)mOwner->mSupplicantScanIntervalMs / 1000);
    return NOERROR;
}

ECode WifiStateMachine::SupplicantStartedState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    AutoPtr<IWifiConfiguration> config;
    Int32 what;
    msg->GetWhat(&what);
    DebugProcessMessage("WifiStateMachine::SupplicantStartedState", what);
    switch(what) {
        case CMD_STOP_SUPPLICANT: {  /* Supplicant stopped by user */
            if (mOwner->mP2pSupported) {
                mOwner->TransitionTo(mOwner->mWaitForP2pDisableState);
            }
            else {
                mOwner->TransitionTo(mOwner->mSupplicantStoppingState);
            }
            break;
        }
        case WifiMonitor::SUP_DISCONNECTION_EVENT: { /* Supplicant connection lost */
            mOwner->Loge(String("Connection lost, restart supplicant"));
            mOwner->mWifiNative->KillSupplicant(mOwner->mP2pSupported);
            mOwner->mWifiNative->CloseSupplicantConnection();
            mOwner->mNetworkInfo->SetIsAvailable(FALSE);
            mOwner->HandleNetworkDisconnect();
            mOwner->SendSupplicantConnectionChangedBroadcast(FALSE);
            mOwner->mSupplicantStateTracker->SendMessage(CMD_RESET_SUPPLICANT_STATE);
            if (mOwner->mP2pSupported) {
                mOwner->TransitionTo(mOwner->mWaitForP2pDisableState);
            }
            else {
                mOwner->TransitionTo(mOwner->mDriverLoadedState);
            }
            mOwner->SendMessageDelayed(CMD_START_SUPPLICANT, SUPPLICANT_RESTART_INTERVAL_MSECS);
            break;
        }
        case WifiMonitor::SCAN_RESULTS_EVENT: {
            mOwner->SetScanResults(mOwner->mWifiNative->ScanResults());
            mOwner->SendScanResultsAvailableBroadcast();
            mOwner->mScanResultIsPending = FALSE;
            break;
        }
        case CMD_PING_SUPPLICANT: {
            Boolean ok = mOwner->mWifiNative->Ping();
            mOwner->ReplyToMessage(msg, what, ok ? SUCCESS : FAILURE);
            break;
        }
        case CMD_ADD_OR_UPDATE_NETWORK: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            config = IWifiConfiguration::Probe(obj);
            mOwner->ReplyToMessage(msg, CMD_ADD_OR_UPDATE_NETWORK,
                    mOwner->mWifiConfigStore->AddOrUpdateNetwork(config));
            break;
        }
        case CMD_REMOVE_NETWORK: {
            Int32 arg1;
            msg->GetArg1(&arg1);
            Boolean ok = mOwner->mWifiConfigStore->RemoveNetwork(arg1);
            mOwner->ReplyToMessage(msg, what, ok ? SUCCESS : FAILURE);
            break;
        }
        case CMD_ENABLE_NETWORK: {
            Int32 arg1, arg2;
            msg->GetArg1(&arg1);
            msg->GetArg2(&arg2);
            Boolean ok = mOwner->mWifiConfigStore->EnableNetwork(arg1, arg2 == 1);
            mOwner->ReplyToMessage(msg, what, ok ? SUCCESS : FAILURE);
            break;
        }
        case CMD_ENABLE_ALL_NETWORKS: {
            Int64 time =  SystemClock::GetElapsedRealtime();
            if (time - mOwner->mLastEnableAllNetworksTime > MIN_INTERVAL_ENABLE_ALL_NETWORKS_MS) {
                mOwner->mWifiConfigStore->EnableAllNetworks();
                mOwner->mLastEnableAllNetworksTime = time;
            }
            break;
        }
        case IWifiManager::DISABLE_NETWORK: {
            Int32 arg1;
            msg->GetArg1(&arg1);
            if (mOwner->mWifiConfigStore->DisableNetwork(arg1,
                    IWifiConfiguration::DISABLED_UNKNOWN_REASON) == TRUE) {
                mOwner->ReplyToMessage(msg, IWifiManager::DISABLE_NETWORK_SUCCEEDED);
            }
            else {
                mOwner->ReplyToMessage(msg, IWifiManager::DISABLE_NETWORK_FAILED,
                        IWifiManager::ERROR);
            }
            break;
        }
        case CMD_BLACKLIST_NETWORK: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            String bssid;
            ICharSequence::Probe(obj)->ToString(&bssid);
            mOwner->mWifiNative->AddToBlacklist(bssid);
            break;
        }
        case CMD_CLEAR_BLACKLIST:
            mOwner->mWifiNative->ClearBlacklist();
            break;
        case CMD_SAVE_CONFIG: {
            Boolean ok = mOwner->mWifiConfigStore->SaveConfig();
            mOwner->ReplyToMessage(msg, CMD_SAVE_CONFIG, ok ? SUCCESS : FAILURE);

//            // Inform the backup manager about a data change
//            IBackupManager ibm = IBackupManager.Stub.asInterface(
//                    ServiceManager.getService(Context.BACKUP_SERVICE));
//            if (ibm != null) {
//                try {
//                    ibm.dataChanged("com.android.providers.settings");
//                } catch (Exception e) {
//                    // Try again later
//                }
//            }
            break;
        }
        case CMD_GET_CONFIGURED_NETWORKS: {
            AutoPtr<IObjectContainer> networks = mOwner->mWifiConfigStore->GetConfiguredNetworks();
            mOwner->ReplyToMessage(msg, what, networks);
            break;
        }
            /* Cannot start soft AP while in client mode */
        case CMD_START_AP: {
            mOwner->Loge(String("Failed to start soft AP with a running supplicant"));
            mOwner->SetWifiApState(IWifiManager::WIFI_AP_STATE_FAILED);
            break;
        }
        case CMD_SET_SCAN_MODE: {
            Int32 arg1;
            msg->GetArg1(&arg1);
            mOwner->mIsScanMode = (arg1 == SCAN_ONLY_MODE);
            break;
        }
        case IWifiManager::SAVE_NETWORK: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            config = IWifiConfiguration::Probe(obj);
            AutoPtr<NetworkUpdateResult> result = mOwner->mWifiConfigStore->SaveNetwork(config);
            if (result->GetNetworkId() != IWifiConfiguration::INVALID_NETWORK_ID) {
                mOwner->ReplyToMessage(msg, IWifiManager::SAVE_NETWORK_SUCCEEDED);
            }
            else {
                mOwner->Loge(String("Failed to save network"));
                mOwner->ReplyToMessage(msg, IWifiManager::SAVE_NETWORK_FAILED,
                        IWifiManager::ERROR);
            }
            break;
        }
        case IWifiManager::FORGET_NETWORK: {
            Int32 arg1;
            msg->GetArg1(&arg1);
            if (mOwner->mWifiConfigStore->ForgetNetwork(arg1)) {
                mOwner->ReplyToMessage(msg, IWifiManager::FORGET_NETWORK_SUCCEEDED);
            }
            else {
                mOwner->Loge(String("Failed to forget network"));
                mOwner->ReplyToMessage(msg, IWifiManager::FORGET_NETWORK_FAILED,
                        IWifiManager::ERROR);
            }
            break;
        }
        default:
            *result = NOT_HANDLED;
            return NOERROR;
    }
    *result = HANDLED;
    return NOERROR;
}

ECode WifiStateMachine::SupplicantStartedState::Exit()
{
    mOwner->mNetworkInfo->SetIsAvailable(FALSE);
    return NOERROR;
}


//==============================================================
// WifiStateMachine::SupplicantStoppingState
//==============================================================
ECode WifiStateMachine::SupplicantStoppingState::Enter()
{
    // if (DBG) log(getName() + "\n");
    if (DBG) mOwner->Log(String("SupplicantStoppingState::Enter"));
    // EventLog.writeEvent(EVENTLOG_WIFI_STATE_CHANGED, getName());

    /* Send any reset commands to supplicant before shutting it down */
    mOwner->HandleNetworkDisconnect();
    if (mOwner->mDhcpStateMachine != NULL) {
        mOwner->mDhcpStateMachine->DoQuit();
    }

    if (DBG) mOwner->Log(String("stopping supplicant"));
    if (!mOwner->mWifiNative->StopSupplicant()) {
        mOwner->Loge(String("Failed to stop supplicant"));
    }

    /* Send ourselves a delayed message to indicate failure after a wait time */
    AutoPtr<IMessage> m = mOwner->ObtainMessage(CMD_STOP_SUPPLICANT_FAILED,
            ++mOwner->mSupplicantStopFailureToken, 0);
    mOwner->SendMessageDelayed(m, SUPPLICANT_RESTART_INTERVAL_MSECS);

    mOwner->mNetworkInfo->SetIsAvailable(FALSE);
    mOwner->SetWifiState(IWifiManager::WIFI_STATE_DISABLING);
    mOwner->SendSupplicantConnectionChangedBroadcast(FALSE);
    mOwner->mSupplicantStateTracker->SendMessage(CMD_RESET_SUPPLICANT_STATE);
    return NOERROR;
}

ECode WifiStateMachine::SupplicantStoppingState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    Int32 what;
    msg->GetWhat(&what);
    DebugProcessMessage("WifiStateMachine::SupplicantStoppingState", what);
    switch(what) {
        case WifiMonitor::SUP_CONNECTION_EVENT:
            mOwner->Loge(String("Supplicant connection received while stopping"));
            break;
        case WifiMonitor::SUP_DISCONNECTION_EVENT:
            if (DBG) mOwner->Log(String("Supplicant connection lost"));
            /* Socket connection can be lost when we do a graceful shutdown
             * or when the driver is hung. Ensure supplicant is stopped here.
             */
            mOwner->mWifiNative->KillSupplicant(mOwner->mP2pSupported);
            mOwner->mWifiNative->CloseSupplicantConnection();
            mOwner->TransitionTo(mOwner->mDriverLoadedState);
            break;
        case CMD_STOP_SUPPLICANT_FAILED: {
            Int32 arg1;
            msg->GetArg1(&arg1);
            if (arg1 == mOwner->mSupplicantStopFailureToken) {
                mOwner->Loge(String("Timed out on a supplicant stop, kill and proceed"));
                mOwner->mWifiNative->KillSupplicant(mOwner->mP2pSupported);
                mOwner->mWifiNative->CloseSupplicantConnection();
                mOwner->TransitionTo(mOwner->mDriverLoadedState);
            }
            break;
        }
        case CMD_LOAD_DRIVER:
        case CMD_UNLOAD_DRIVER:
        case CMD_START_SUPPLICANT:
        case CMD_STOP_SUPPLICANT:
        case CMD_START_AP:
        case CMD_STOP_AP:
        case CMD_START_DRIVER:
        case CMD_STOP_DRIVER:
        case CMD_SET_SCAN_MODE:
        case CMD_SET_SCAN_TYPE:
        case CMD_SET_COUNTRY_CODE:
        case CMD_SET_FREQUENCY_BAND:
        case CMD_START_PACKET_FILTERING:
        case CMD_STOP_PACKET_FILTERING:
            mOwner->DeferMessage(msg);
            break;
        default:
            *result = NOT_HANDLED;
            return NOERROR;
    }
    *result = HANDLED;
    return NOERROR;
}


//==============================================================
// WifiStateMachine::DriverStartingState
//==============================================================
ECode WifiStateMachine::DriverStartingState::Enter()
{
    // if (DBG) log(getName() + "\n");
    if (DBG) mOwner->Log(String("DriverStartingState::Enter"));
    // EventLog.writeEvent(EVENTLOG_WIFI_STATE_CHANGED, getName());

    mTries = 1;
    /* Send ourselves a delayed message to start driver a second time */
    AutoPtr<IMessage> m = mOwner->ObtainMessage(CMD_DRIVER_START_TIMED_OUT,
            ++mOwner->mDriverStartToken, 0);
    mOwner->SendMessageDelayed(m, DRIVER_START_TIME_OUT_MSECS);
    return NOERROR;
}

ECode WifiStateMachine::DriverStartingState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    Int32 what;
    msg->GetWhat(&what);
    DebugProcessMessage("WifiStateMachine::DriverStartingState", what);
    switch(what) {
       case WifiMonitor::SUPPLICANT_STATE_CHANGE_EVENT: {
            SupplicantState state = mOwner->HandleSupplicantStateChange(msg);
            /* If suplicant is exiting out of INTERFACE_DISABLED state into
             * a state that indicates driver has started, it is ready to
             * receive driver commands
             */
            if (SupplicantStateHelper::IsDriverActive(state)) {
                mOwner->TransitionTo(mOwner->mDriverStartedState);
            }
            break;
        }
        case CMD_DRIVER_START_TIMED_OUT: {
            Int32 arg1;
            msg->GetArg1(&arg1);
            if (arg1 == mOwner->mDriverStartToken) {
                if (mTries >= 2) {
                    // loge("Failed to start driver after " + mTries);
                    mOwner->TransitionTo(mOwner->mDriverStoppedState);
                }
                else {
                    mOwner->Loge(String("Driver start failed, retrying"));
                    mOwner->mWakeLock->AcquireLock();
                    mOwner->mWifiNative->StartDriver();
                    mOwner->mWakeLock->ReleaseLock();

                    ++mTries;
                    /* Send ourselves a delayed message to start driver again */
                    AutoPtr<IMessage> m = mOwner->ObtainMessage(CMD_DRIVER_START_TIMED_OUT,
                            ++mOwner->mDriverStartToken, 0);
                    mOwner->SendMessageDelayed(m, DRIVER_START_TIME_OUT_MSECS);
                }
            }
            break;
        }
            /* Queue driver commands & connection events */
        case CMD_START_DRIVER:
        case CMD_STOP_DRIVER:
        case WifiMonitor::NETWORK_CONNECTION_EVENT:
        case WifiMonitor::NETWORK_DISCONNECTION_EVENT:
        case WifiMonitor::AUTHENTICATION_FAILURE_EVENT:
        case WifiMonitor::WPS_OVERLAP_EVENT:
        case CMD_SET_SCAN_TYPE:
        case CMD_SET_COUNTRY_CODE:
        case CMD_SET_FREQUENCY_BAND:
        case CMD_START_PACKET_FILTERING:
        case CMD_STOP_PACKET_FILTERING:
        case CMD_START_SCAN:
        case CMD_DISCONNECT:
        case CMD_REASSOCIATE:
        case CMD_RECONNECT:
            mOwner->DeferMessage(msg);
            break;
        default:
            *result = NOT_HANDLED;
            return NOERROR;
    }
    *result = HANDLED;
    return NOERROR;
}


//==============================================================
// WifiStateMachine::DriverStartedState
//==============================================================
ECode WifiStateMachine::DriverStartedState::Enter()
{
    // if (DBG) log(getName() + "\n");
    if (DBG) mOwner->Log(String("DriverStartedState::Enter"));
    // EventLog.writeEvent(EVENTLOG_WIFI_STATE_CHANGED, getName());

    mOwner->mIsRunning = TRUE;
    mOwner->mInDelayedStop = FALSE;
    mOwner->UpdateBatteryWorkSource(NULL);

    /**
     * Enable bluetooth coexistence scan mode when bluetooth connection is active.
     * When this mode is on, some of the low-level scan parameters used by the
     * driver are changed to reduce interference with bluetooth
     */
    mOwner->mWifiNative->SetBluetoothCoexistenceScanMode(mOwner->mBluetoothConnectionActive);
    /* set country code */
    mOwner->SetCountryCode();
    /* set frequency band of operation */
    mOwner->SetFrequencyBand();
    /* initialize network state */
    mOwner->SetNetworkDetailedState(NetworkInfoDetailedState_DISCONNECTED);

    /* Remove any filtering on Multicast v6 at start */
    mOwner->mWifiNative->StopFilteringMulticastV6Packets();

    /* Reset Multicast v4 filtering state */
    Boolean filtering;
    if (mOwner->mFilteringMulticastV4Packets->Get(&filtering), filtering) {
        mOwner->mWifiNative->StartFilteringMulticastV4Packets();
    }
    else {
        mOwner->mWifiNative->StopFilteringMulticastV4Packets();
    }

    if (mOwner->mIsScanMode) {
        mOwner->mWifiNative->SetScanResultHandling(SCAN_ONLY_MODE);
        mOwner->mWifiNative->Disconnect();
        mOwner->TransitionTo(mOwner->mScanModeState);
    }
    else {
        mOwner->mWifiNative->SetScanResultHandling(CONNECT_MODE);
        mOwner->mWifiNative->Reconnect();
        // Status pulls in the current supplicant state and network connection state
        // events over the monitor connection. This helps framework sync up with
        // current supplicant state
        mOwner->mWifiNative->Status();
        mOwner->TransitionTo(mOwner->mDisconnectedState);
    }

    // We may have missed screen update at boot
    Boolean received;
    if (mOwner->mScreenBroadcastReceived->Get(&received), !received) {
        AutoPtr<IInterface> obj;
        mOwner->mContext->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&obj);
        AutoPtr<IPowerManager> powerManager = IPowerManager::Probe(obj);
        Boolean isOn;
        powerManager->IsScreenOn(&isOn);
        mOwner->HandleScreenStateChanged(isOn);
    }
    else {
        // Set the right suspend mode settings
        Boolean suspend;
        mOwner->mWifiNative->SetSuspendOptimizations(mOwner->mSuspendOptNeedsDisabled == 0
                && (mOwner->mUserWantsSuspendOpt->Get(&suspend), suspend));
    }
    mOwner->mWifiNative->SetPowerSave(TRUE);

    if (mOwner->mP2pSupported) mOwner->mWifiP2pChannel->SendMessage(WifiStateMachine::CMD_ENABLE_P2P);
    return NOERROR;
}

ECode WifiStateMachine::DriverStartedState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    Int32 what;
    msg->GetWhat(&what);
    DebugProcessMessage("WifiStateMachine::DriverStartedState", what);

    switch(what) {
       case CMD_SET_SCAN_TYPE: {
            Int32 arg1;
            msg->GetArg1(&arg1);
            mOwner->mSetScanActive = (arg1 == SCAN_ACTIVE);
            mOwner->mWifiNative->SetScanMode(mOwner->mSetScanActive);
            break;
        }
        case CMD_START_SCAN: {
            Int32 arg1;
            msg->GetArg1(&arg1);
            Boolean forceActive = (arg1 == SCAN_ACTIVE);
            if (forceActive && !mOwner->mSetScanActive) {
                mOwner->mWifiNative->SetScanMode(forceActive);
            }
            mOwner->mWifiNative->Scan();
            if (forceActive && !mOwner->mSetScanActive) {
                mOwner->mWifiNative->SetScanMode(mOwner->mSetScanActive);
            }
            mOwner->mScanResultIsPending = TRUE;
            break;
        }
        case CMD_SET_COUNTRY_CODE: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            String country;
            ICharSequence::Probe(obj)->ToString(&country);
            if (DBG) mOwner->Log(String("set country code ") + country);
            country = country.ToUpperCase();
            if (!mOwner->mWifiNative->SetCountryCode(country)) {
                mOwner->Loge(String("Failed to set country code ") + country);
            }
            break;
        }
        case CMD_SET_FREQUENCY_BAND: {
            Int32 band;
            msg->GetArg1(&band);
            if (DBG) mOwner->Log(String("set frequency band ") + StringUtils::Int32ToString(band));
            if (mOwner->mWifiNative->SetBand(band)) {
                mOwner->mFrequencyBand->Set(band);
                //Fetch the latest scan results when frequency band is set
                mOwner->StartScan(TRUE);
            }
            else {
                mOwner->Loge(String("Failed to set frequency band ") + StringUtils::Int32ToString(band));
            }
            break;
        }
        case CMD_BLUETOOTH_ADAPTER_STATE_CHANGE: {
            Int32 arg1;
            msg->GetArg1(&arg1);
            mOwner->mBluetoothConnectionActive = (arg1 !=
                    IBluetoothAdapter::STATE_DISCONNECTED);
            mOwner->mWifiNative->SetBluetoothCoexistenceScanMode(mOwner->mBluetoothConnectionActive);
            break;
        }
        case CMD_STOP_DRIVER: {
            Int32 mode;
            msg->GetArg1(&mode);

            /* Already doing a delayed stop && not in ecm state */
            if (mOwner->mInDelayedStop && mode != IN_ECM_STATE) {
                if (DBG) mOwner->Log(String("Already in delayed stop"));
                break;
            }
            mOwner->mInDelayedStop = TRUE;
            mOwner->mDelayedStopCounter++;
            if (DBG) mOwner->Log(String("Delayed stop message ") + StringUtils::Int32ToString(mOwner->mDelayedStopCounter));

            if (mode == IN_ECM_STATE) {
                /* send a shut down immediately */
                AutoPtr<IMessage> m = mOwner->ObtainMessage(CMD_DELAYED_STOP_DRIVER, mOwner->mDelayedStopCounter, 0);
                mOwner->SendMessage(m);
            }
            else {
                /* send regular delayed shut down */
                AutoPtr<IIntent> driverStopIntent;
                CIntent::New(ACTION_DELAYED_DRIVER_STOP, NULL, (IIntent**)&driverStopIntent);
                driverStopIntent->PutExtra(DELAYED_STOP_COUNTER, mOwner->mDelayedStopCounter);
                AutoPtr<IPendingIntentHelper> helper;
                CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&helper);
                mOwner->mDriverStopIntent = NULL;
                helper->GetBroadcast(mOwner->mContext,
                       DRIVER_STOP_REQUEST, driverStopIntent,
                       IPendingIntent::FLAG_UPDATE_CURRENT, (IPendingIntent**)&mOwner->mDriverStopIntent);

                AutoPtr<ISystem> system;
                Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
                Int64 millis;
                system->GetCurrentTimeMillis(&millis);
                mOwner->mAlarmManager->Set(IAlarmManager::RTC_WAKEUP,
                    millis + mOwner->mDriverStopDelayMs, mOwner->mDriverStopIntent);
            }
            break;
        }
        case CMD_START_DRIVER:
            if (mOwner->mInDelayedStop) {
                mOwner->mInDelayedStop = FALSE;
                mOwner->mDelayedStopCounter++;
                mOwner->mAlarmManager->Cancel(mOwner->mDriverStopIntent);
                if (DBG) mOwner->Log(String("Delayed stop ignored due to start"));
            }
            break;
        case CMD_DELAYED_STOP_DRIVER: {
            Int32 arg1;
            msg->GetArg1(&arg1);
            // if (DBG) log("delayed stop " + message.arg1 + " " + mDelayedStopCounter);
            if (arg1 != mOwner->mDelayedStopCounter) break;
            if (mOwner->GetCurrentState() != mOwner->mDisconnectedState) {
                mOwner->mWifiNative->Disconnect();
                mOwner->HandleNetworkDisconnect();
            }
            mOwner->mWakeLock->AcquireLock();
            mOwner->mWifiNative->StopDriver();
            mOwner->mWakeLock->ReleaseLock();
            if (mOwner->mP2pSupported) {
                mOwner->TransitionTo(mOwner->mWaitForP2pDisableState);
            }
            else {
                mOwner->TransitionTo(mOwner->mDriverStoppingState);
            }
            break;
        }
        case CMD_START_PACKET_FILTERING: {
            Int32 arg1;
            msg->GetArg1(&arg1);
            if (arg1 == MULTICAST_V6) {
                mOwner->mWifiNative->StartFilteringMulticastV6Packets();
            }
            else if (arg1 == MULTICAST_V4) {
                mOwner->mWifiNative->StartFilteringMulticastV4Packets();
            }
            else {
                mOwner->Loge(String("Illegal arugments to CMD_START_PACKET_FILTERING"));
            }
            break;
        }
        case CMD_STOP_PACKET_FILTERING: {
            Int32 arg1;
            msg->GetArg1(&arg1);
            if (arg1 == MULTICAST_V6) {
                mOwner->mWifiNative->StopFilteringMulticastV6Packets();
            }
            else if (arg1 == MULTICAST_V4) {
                mOwner->mWifiNative->StopFilteringMulticastV4Packets();
            }
            else {
                mOwner->Loge(String("Illegal arugments to CMD_STOP_PACKET_FILTERING"));
            }
            break;
        }
        case CMD_SET_SUSPEND_OPT_ENABLED: {
            Int32 arg1;
            msg->GetArg1(&arg1);
            if (arg1 == 1) {
                mOwner->SetSuspendOptimizationsNative(SUSPEND_DUE_TO_SCREEN, TRUE);
                mOwner->mSuspendWakeLock->ReleaseLock();
            }
            else {
                mOwner->SetSuspendOptimizationsNative(SUSPEND_DUE_TO_SCREEN, FALSE);
            }
            break;
        }
        case CMD_SET_HIGH_PERF_MODE: {
            Int32 arg1;
            msg->GetArg1(&arg1);
            if (arg1 == 1) {
                mOwner->SetSuspendOptimizationsNative(SUSPEND_DUE_TO_HIGH_PERF, FALSE);
            }
            else {
                mOwner->SetSuspendOptimizationsNative(SUSPEND_DUE_TO_HIGH_PERF, TRUE);
            }
            break;
        }
        default:
            *result = NOT_HANDLED;
            return NOERROR;
    }
    *result = HANDLED;
    return NOERROR;
}

ECode WifiStateMachine::DriverStartedState::Exit()
{
    // if (DBG) log(getName() + " exit\n");
    if (DBG) mOwner->Log(String("DriverStartedState::Exit"));
    mOwner->mIsRunning = FALSE;
    mOwner->UpdateBatteryWorkSource(NULL);
    AutoLock lock(mOwner->mScanResultCacheLock);
    mOwner->mScanResults.Clear();
    return NOERROR;
}


//==============================================================
// WifiStateMachine::WaitForP2pDisableState
//==============================================================
ECode WifiStateMachine::WaitForP2pDisableState::Enter()
{
    // if (DBG) log(getName() + "\n");
    if (DBG) mOwner->Log(String("WaitForP2pDisableState::Enter"));
    // EventLog.writeEvent(EVENTLOG_WIFI_STATE_CHANGED, getName());
    Int32 what;
    mOwner->GetCurrentMessage()->GetWhat(&what);
    switch (what) {
        case WifiMonitor::SUP_DISCONNECTION_EVENT:
            mTransitionToState = mOwner->mDriverLoadedState;
            break;
        case CMD_DELAYED_STOP_DRIVER:
            mTransitionToState = mOwner->mDriverStoppingState;
            break;
        case CMD_STOP_SUPPLICANT:
            mTransitionToState = mOwner->mSupplicantStoppingState;
            break;
        default:
            mTransitionToState = mOwner->mDriverStoppingState;
            break;
    }
    mOwner->mWifiP2pChannel->SendMessage(WifiStateMachine::CMD_DISABLE_P2P_REQ);
    return NOERROR;
}

ECode WifiStateMachine::WaitForP2pDisableState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    Int32 what;
    msg->GetWhat(&what);
    DebugProcessMessage("WifiStateMachine::WaitForP2pDisableState", what);

    switch(what) {
        case WifiStateMachine::CMD_DISABLE_P2P_RSP:
            mOwner->TransitionTo(mTransitionToState);
            break;
        /* Defer wifi start/shut and driver commands */
        case WifiMonitor::SUPPLICANT_STATE_CHANGE_EVENT:
        case CMD_LOAD_DRIVER:
        case CMD_UNLOAD_DRIVER:
        case CMD_START_SUPPLICANT:
        case CMD_STOP_SUPPLICANT:
        case CMD_START_AP:
        case CMD_STOP_AP:
        case CMD_START_DRIVER:
        case CMD_STOP_DRIVER:
        case CMD_SET_SCAN_MODE:
        case CMD_SET_SCAN_TYPE:
        case CMD_SET_COUNTRY_CODE:
        case CMD_SET_FREQUENCY_BAND:
        case CMD_START_PACKET_FILTERING:
        case CMD_STOP_PACKET_FILTERING:
        case CMD_START_SCAN:
        case CMD_DISCONNECT:
        case CMD_REASSOCIATE:
        case CMD_RECONNECT:
            mOwner->DeferMessage(msg);
            break;
        default:
            *result = NOT_HANDLED;
            return NOERROR;
    }
    *result = HANDLED;
    return NOERROR;
}


//==============================================================
// WifiStateMachine::DriverStoppingState
//==============================================================
ECode WifiStateMachine::DriverStoppingState::Enter()
{
    // if (DBG) log(getName() + "\n");
    if (DBG) mOwner->Log(String("DriverStoppingState::Enter"));
    // EventLog.writeEvent(EVENTLOG_WIFI_STATE_CHANGED, getName());
    return NOERROR;
}

ECode WifiStateMachine::DriverStoppingState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    Int32 what;
    msg->GetWhat(&what);
    DebugProcessMessage("WifiStateMachine::DriverStoppingState", what);

    switch(what) {
        case WifiMonitor::SUPPLICANT_STATE_CHANGE_EVENT: {
            SupplicantState state = mOwner->HandleSupplicantStateChange(msg);
            if (state == SupplicantState_INTERFACE_DISABLED) {
                mOwner->TransitionTo(mOwner->mDriverStoppedState);
            }
            break;
        }
            /* Queue driver commands */
        case CMD_START_DRIVER:
        case CMD_STOP_DRIVER:
        case CMD_SET_SCAN_TYPE:
        case CMD_SET_COUNTRY_CODE:
        case CMD_SET_FREQUENCY_BAND:
        case CMD_START_PACKET_FILTERING:
        case CMD_STOP_PACKET_FILTERING:
        case CMD_START_SCAN:
        case CMD_DISCONNECT:
        case CMD_REASSOCIATE:
        case CMD_RECONNECT:
            mOwner->DeferMessage(msg);
            break;
        default:
            *result = NOT_HANDLED;
            return NOERROR;
    }
    *result = HANDLED;
    return NOERROR;
}


//==============================================================
// WifiStateMachine::DriverStoppedState
//==============================================================
ECode WifiStateMachine::DriverStoppedState::Enter()
{
    // if (DBG) log(getName() + "\n");
    if (DBG) mOwner->Log(String("DriverStoppedState::Enter"));
    // EventLog.writeEvent(EVENTLOG_WIFI_STATE_CHANGED, getName());
    return NOERROR;
}

ECode WifiStateMachine::DriverStoppedState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    Int32 what;
    msg->GetWhat(&what);
    DebugProcessMessage("WifiStateMachine::DriverStoppedState", what);

    switch (what) {
        case WifiMonitor::SUPPLICANT_STATE_CHANGE_EVENT: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            IStateChangeResult* stateChangeResult = IStateChangeResult::Probe(obj.Get());
            SupplicantState state;
            stateChangeResult->GetSupplicantState(&state);
            // A WEXT bug means that we can be back to driver started state
            // unexpectedly
            if (SupplicantStateHelper::IsDriverActive(state)) {
                mOwner->TransitionTo(mOwner->mDriverStartedState);
            }
            break;
        }
        case CMD_START_DRIVER:
            mOwner->mWakeLock->AcquireLock();
            mOwner->mWifiNative->StartDriver();
            mOwner->mWakeLock->ReleaseLock();
            mOwner->TransitionTo(mOwner->mDriverStartingState);
            break;
        default:
            *result = NOT_HANDLED;
            return NOERROR;
    }
    *result = HANDLED;
    return NOERROR;
}


//==============================================================
// WifiStateMachine::ScanModeState
//==============================================================
ECode WifiStateMachine::ScanModeState::Enter()
{
    // if (DBG) log(getName() + "\n");
    if (DBG) mOwner->Log(String("ScanModeState::Enter"));
    // EventLog.writeEvent(EVENTLOG_WIFI_STATE_CHANGED, getName());
    return NOERROR;
}

ECode WifiStateMachine::ScanModeState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    Int32 what;
    msg->GetWhat(&what);
    DebugProcessMessage("WifiStateMachine::ScanModeState", what);
    switch(what) {
        case CMD_SET_SCAN_MODE: {
            Int32 arg1;
            msg->GetArg1(&arg1);
            if (arg1 == SCAN_ONLY_MODE) {
                /* Ignore */
                *result = HANDLED;
                return NOERROR;
            }
            else {
                mOwner->mWifiNative->SetScanResultHandling(arg1);
                mOwner->mWifiNative->Reconnect();
                mOwner->mIsScanMode = FALSE;
                mOwner->TransitionTo(mOwner->mDisconnectedState);
            }
            break;
        }
            /* Ignore */
        case CMD_DISCONNECT:
        case CMD_RECONNECT:
        case CMD_REASSOCIATE:
        case WifiMonitor::SUPPLICANT_STATE_CHANGE_EVENT:
        case WifiMonitor::NETWORK_CONNECTION_EVENT:
        case WifiMonitor::NETWORK_DISCONNECTION_EVENT:
            break;
        default:
            *result = NOT_HANDLED;
            return NOERROR;
    }
    *result = HANDLED;
    return NOERROR;
}


//==============================================================
// WifiStateMachine::ConnectModeState
//==============================================================
ECode WifiStateMachine::ConnectModeState::Enter()
{
    // if (DBG) log(getName() + "\n");
    if (DBG) mOwner->Log(String("ConnectModeState::Enter"));
    // EventLog.writeEvent(EVENTLOG_WIFI_STATE_CHANGED, getName());
    return NOERROR;
}

ECode WifiStateMachine::ConnectModeState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    Int32 what;
    msg->GetWhat(&what);
    DebugProcessMessage("WifiStateMachine::ConnectModeState", what);
    switch(what) {
        case WifiMonitor::AUTHENTICATION_FAILURE_EVENT:
            if (DBG) Logger::D("WifiStateMachine::ConnectModeState", "ProcessMessage AUTHENTICATION_FAILURE_EVENT");
            mOwner->mSupplicantStateTracker->SendMessage(WifiMonitor::AUTHENTICATION_FAILURE_EVENT);
            break;
        case WifiMonitor::SUPPLICANT_STATE_CHANGE_EVENT: {
            if (DBG) Logger::D("WifiStateMachine::ConnectModeState", "ProcessMessage SUPPLICANT_STATE_CHANGE_EVENT");
            SupplicantState state = mOwner->HandleSupplicantStateChange(msg);
            // A driver/firmware hang can now put the interface in a down state.
            // We detect the interface going down and recover from it
            NetworkInfoState s;
            if (!SupplicantStateHelper::IsDriverActive(state)) {
                if (mOwner->mNetworkInfo->GetState(&s), s != NetworkInfoState_DISCONNECTED) {
                    mOwner->HandleNetworkDisconnect();
                }
                mOwner->Log(String("Detected an interface down, restart driver"));
                mOwner->TransitionTo(mOwner->mDriverStoppedState);
                mOwner->SendMessage(CMD_START_DRIVER);
                break;
            }

            // Supplicant can fail to report a NETWORK_DISCONNECTION_EVENT
            // when authentication times out after a successful connection,
            // we can figure this from the supplicant state. If supplicant
            // state is DISCONNECTED, but the mNetworkInfo says we are not
            // disconnected, we need to handle a disconnection
            if (state == SupplicantState_DISCONNECTED &&
                    (mOwner->mNetworkInfo->GetState(&s), s != NetworkInfoState_DISCONNECTED)) {
                if (DBG) mOwner->Log(String("Missed CTRL-EVENT-DISCONNECTED, disconnect"));
                mOwner->HandleNetworkDisconnect();
                mOwner->TransitionTo(mOwner->mDisconnectedState);
            }
            break;
        }
        case IWifiP2pService::DISCONNECT_WIFI_REQUEST: {
            Int32 arg1;
            msg->GetArg1(&arg1);
            if (arg1 == 1) {
                mOwner->mWifiNative->Disconnect();
                mOwner->mTemporarilyDisconnectWifi = TRUE;
            }
            else {
                mOwner->mWifiNative->Reconnect();
                mOwner->mTemporarilyDisconnectWifi = FALSE;
            }
            break;
        }
            /* Do a redundant disconnect without transition */
        case CMD_DISCONNECT:
            if (DBG) Logger::D("WifiStateMachine::ConnectModeState", "ProcessMessage CMD_DISCONNECT");
            mOwner->mWifiNative->Disconnect();
            break;
        case CMD_RECONNECT:
            if (DBG) Logger::D("WifiStateMachine::ConnectModeState", "ProcessMessage CMD_RECONNECT");
            mOwner->mWifiNative->Reconnect();
            break;
        case CMD_REASSOCIATE:
            if (DBG) Logger::D("WifiStateMachine::ConnectModeState", "ProcessMessage CMD_REASSOCIATE");
            mOwner->mWifiNative->Reassociate();
            break;
        case IWifiManager::CONNECT_NETWORK: {
            if (DBG) Logger::D("WifiStateMachine::ConnectModeState", "ProcessMessage CONNECT_NETWORK");

            /* The connect message can contain a network id passed as arg1 on message or
             * or a config passed as obj on message.
             * For a new network, a config is passed to create and connect.
             * For an existing network, a network id is passed
             */
            Int32 netId;
            msg->GetArg1(&netId);
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            AutoPtr<IWifiConfiguration> config = IWifiConfiguration::Probe(obj);

            /* Save the network config */
            if (config != NULL) {
                AutoPtr<NetworkUpdateResult> result = mOwner->mWifiConfigStore->SaveNetwork(config);
                netId = result->GetNetworkId();
            }

            if (mOwner->mWifiConfigStore->SelectNetwork(netId) &&
                    mOwner->mWifiNative->Reconnect()) {
                if (DBG) Logger::D(TAG, "Succeeded to connect config: %p netId: %d", config.Get(), netId);
                /* The state tracker handles enabling networks upon completion/failure */
                mOwner->mSupplicantStateTracker->SendMessage(IWifiManager::CONNECT_NETWORK);
                mOwner->ReplyToMessage(msg, IWifiManager::CONNECT_NETWORK_SUCCEEDED);
                /* Expect a disconnection from the old connection */
                mOwner->TransitionTo(mOwner->mDisconnectingState);
            }
            else {
                if (DBG) Logger::D(TAG, "Failed to connect config: %p netId: %d", config.Get(), netId);
                mOwner->ReplyToMessage(msg, IWifiManager::CONNECT_NETWORK_FAILED,
                        IWifiManager::ERROR);
                break;
            }
            break;
        }
        case IWifiManager::START_WPS: {
            if (DBG) Logger::D("WifiStateMachine::ConnectModeState", "ProcessMessage START_WPS");

            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            IWpsInfo* wpsInfo = IWpsInfo::Probe(obj);
            AutoPtr<IWpsResult> result;
            Int32 setup;
            wpsInfo->GetSetup(&setup);
            switch (setup) {
                case IWpsInfo::PBC:
                    result = mOwner->mWifiConfigStore->StartWpsPbc(wpsInfo);
                    break;
                case IWpsInfo::KEYPAD:
                    result = mOwner->mWifiConfigStore->StartWpsWithPinFromAccessPoint(wpsInfo);
                    break;
                case IWpsInfo::DISPLAY:
                    result = mOwner->mWifiConfigStore->StartWpsWithPinFromDevice(wpsInfo);
                    break;
                default:
                    CWpsResult::New(WpsResultStatus_FAILURE, (IWpsResult**)&result);
                    Logger::E(TAG, "Invalid setup for WPS");
                    break;
            }
            WpsResultStatus status;
            result->GetStatus(&status);
            if (status == WpsResultStatus_SUCCESS) {
                mOwner->ReplyToMessage(msg, IWifiManager::START_WPS_SUCCEEDED, result);
                mOwner->TransitionTo(mOwner->mWpsRunningState);
            }
            else {
                // Logger::E(TAG, "Failed to start WPS with config " + wpsInfo.toString());
                mOwner->ReplyToMessage(msg, IWifiManager::WPS_FAILED, IWifiManager::ERROR);
            }
            break;
        }
        case WifiMonitor::SCAN_RESULTS_EVENT:
            if (DBG) Logger::D("WifiStateMachine::ConnectModeState", "ProcessMessage SCAN_RESULTS_EVENT");
            /* Set the scan setting back to "connect" mode */
            mOwner->mWifiNative->SetScanResultHandling(CONNECT_MODE);
            /* Handle scan results */
            *result = NOT_HANDLED;
            return NOERROR;
        case WifiMonitor::NETWORK_CONNECTION_EVENT: {
            if (DBG) Logger::D("WifiStateMachine::ConnectModeState", "Network connection established");
            msg->GetArg1(&mOwner->mLastNetworkId);
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            assert(ICharSequence::Probe(obj) != NULL);
            ICharSequence::Probe(obj)->ToString(&mOwner->mLastBssid);
            mOwner->mWifiInfo->SetBSSID(mOwner->mLastBssid);
            mOwner->mWifiInfo->SetNetworkId(mOwner->mLastNetworkId);
            /* send event to CM & network change broadcast */
            mOwner->SetNetworkDetailedState(NetworkInfoDetailedState_OBTAINING_IPADDR);
            mOwner->SendNetworkStateChangeBroadcast(mOwner->mLastBssid);
            mOwner->TransitionTo(mOwner->mObtainingIpState);
            break;
        }
        case WifiMonitor::NETWORK_DISCONNECTION_EVENT:
            if (DBG) Logger::D("WifiStateMachine::ConnectModeState", "Network connection lost");
            mOwner->HandleNetworkDisconnect();
            mOwner->TransitionTo(mOwner->mDisconnectedState);
            break;
        default:
            DebugProcessMessage("WifiStateMachine::ConnectModeState", what);
            *result = NOT_HANDLED;
            return NOERROR;
    }
    *result = HANDLED;
    return NOERROR;
}


//==============================================================
// WifiStateMachine::L2ConnectedState
//==============================================================
ECode WifiStateMachine::L2ConnectedState::Enter()
{
    // if (DBG) log(getName() + "\n");
    if (DBG) mOwner->Log(String("L2ConnectedState::Enter"));
    // EventLog.writeEvent(EVENTLOG_WIFI_STATE_CHANGED, getName());
    mOwner->mRssiPollToken++;
    if (mOwner->mEnableRssiPolling) {
        AutoPtr<IMessage> m = mOwner->ObtainMessage(CMD_RSSI_POLL, mOwner->mRssiPollToken, 0);
        mOwner->SendMessage(m);
    }
    return NOERROR;
}

ECode WifiStateMachine::L2ConnectedState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    Int32 what, arg1;
    msg->GetWhat(&what);
    msg->GetArg1(&arg1);
    DebugProcessMessage("WifiStateMachine::L2ConnectedState", what);

    switch (what) {
        case DhcpStateMachine::CMD_PRE_DHCP_ACTION:
            if (DBG) Logger::D("WifiStateMachine::L2ConnectedState", "ProcessMessage CMD_PRE_DHCP_ACTION");
            mOwner->HandlePreDhcpSetup();
            mOwner->mDhcpStateMachine->SendMessage(DhcpStateMachine::CMD_PRE_DHCP_ACTION_COMPLETE);
            break;
        case DhcpStateMachine::CMD_POST_DHCP_ACTION: {
            if (DBG) Logger::D("WifiStateMachine::L2ConnectedState", "ProcessMessage CMD_POST_DHCP_ACTION");
            mOwner->HandlePostDhcpSetup();
            if (arg1 == DhcpStateMachine::DHCP_SUCCESS) {
                if (DBG) mOwner->Log(String("DHCP successful"));
                AutoPtr<IInterface> obj;
                msg->GetObj((IInterface**)&obj);
                mOwner->HandleSuccessfulIpConfiguration((DhcpInfoInternal*)obj.Get());
                mOwner->TransitionTo(mOwner->mVerifyingLinkState);
            }
            else if (arg1 == DhcpStateMachine::DHCP_FAILURE) {
                if (DBG) mOwner->Log(String("DHCP failed"));
                mOwner->HandleFailedIpConfiguration();
                mOwner->TransitionTo(mOwner->mDisconnectingState);
            }
            break;
        }
        case CMD_DISCONNECT:
            mOwner->mWifiNative->Disconnect();
            mOwner->TransitionTo(mOwner->mDisconnectingState);
            break;
        case IWifiP2pService::DISCONNECT_WIFI_REQUEST:
            if (arg1 == 1) {
                mOwner->mWifiNative->Disconnect();
                mOwner->mTemporarilyDisconnectWifi = TRUE;
                mOwner->TransitionTo(mOwner->mDisconnectingState);
            }
            break;
        case CMD_SET_SCAN_MODE: {
            if (arg1 == SCAN_ONLY_MODE) {
                mOwner->SendMessage(CMD_DISCONNECT);
                mOwner->DeferMessage(msg);
            }
            break;
        }
        case CMD_START_SCAN:
            /* When the network is connected, re-scanning can trigger
             * a reconnection. Put it in scan-only mode during scan.
             * When scan results are received, the mode is switched
             * back to CONNECT_MODE.
             */
            mOwner->mWifiNative->SetScanResultHandling(SCAN_ONLY_MODE);
            /* Have the parent state handle the rest */
            *result = NOT_HANDLED;
            return NOERROR;
            /* Ignore connection to same network */
        case IWifiManager::CONNECT_NETWORK: {
            Int32 id;
            if (mOwner->mWifiInfo->GetNetworkId(&id), id == arg1) {
                break;
            }
            *result = NOT_HANDLED;
            return NOERROR;
        }
        case IWifiManager::SAVE_NETWORK: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            IWifiConfiguration* config = IWifiConfiguration::Probe(obj);
            AutoPtr<NetworkUpdateResult> result = mOwner->mWifiConfigStore->SaveNetwork(config);
            Int32 networkId1, networkId2;
            mOwner->mWifiInfo->GetNetworkId(&networkId1);
            networkId2 = result->GetNetworkId();
            if (networkId1 == networkId2) {
                if (result->HasIpChanged()) {
                    mOwner->Log(String("Reconfiguring IP on connection"));
                    mOwner->TransitionTo(mOwner->mObtainingIpState);
                }
                if (result->HasProxyChanged()) {
                    mOwner->Log(String("Reconfiguring proxy on connection"));
                    mOwner->ConfigureLinkProperties();
                    mOwner->SendLinkConfigurationChangedBroadcast();
                }
            }
            if (networkId2 != IWifiConfiguration::INVALID_NETWORK_ID) {
                mOwner->ReplyToMessage(msg, IWifiManager::SAVE_NETWORK_SUCCEEDED);
            }
            else {
                mOwner->Loge(String("Failed to save network"));
                mOwner->ReplyToMessage(msg, IWifiManager::SAVE_NETWORK_FAILED,
                        IWifiManager::ERROR);
            }
            break;
        }
            /* Ignore */
        case WifiMonitor::NETWORK_CONNECTION_EVENT:
            break;
        case CMD_RSSI_POLL: {
            if (arg1 == mOwner->mRssiPollToken) {
                // Get Info and continue polling
                mOwner->FetchRssiAndLinkSpeedNative();
                AutoPtr<IMessage> m = mOwner->ObtainMessage(CMD_RSSI_POLL,
                        mOwner->mRssiPollToken, 0);
                mOwner->SendMessageDelayed(m, POLL_RSSI_INTERVAL_MSECS);
            }
            else {
                // Polling has completed
            }
            break;
        }
        case CMD_ENABLE_RSSI_POLL: {
            mOwner->mEnableRssiPolling = (arg1 == 1);
            mOwner->mRssiPollToken++;
            if (mOwner->mEnableRssiPolling) {
                // first poll
                mOwner->FetchRssiAndLinkSpeedNative();
                AutoPtr<IMessage> m = mOwner->ObtainMessage(CMD_RSSI_POLL,
                        mOwner->mRssiPollToken, 0);
                mOwner->SendMessageDelayed(m, POLL_RSSI_INTERVAL_MSECS);
            }
            break;
        }
        case IWifiManager::RSSI_PKTCNT_FETCH: {
            AutoPtr<IRssiPacketCountInfo> info;
            CRssiPacketCountInfo::New((IRssiPacketCountInfo**)&info);
            mOwner->FetchRssiAndLinkSpeedNative();
            Int32 rssi;
            mOwner->mWifiInfo->GetRssi(&rssi);
            info->SetRssi(rssi);
            mOwner->FetchPktcntNative(info);
            mOwner->ReplyToMessage(msg, IWifiManager::RSSI_PKTCNT_FETCH_SUCCEEDED, info);
            break;
        }
        default:
            *result = NOT_HANDLED;
            return NOERROR;
    }

    *result = HANDLED;
    return NOERROR;
}

ECode WifiStateMachine::L2ConnectedState::Exit()
{
    if (DBG) mOwner->Log(String("L2ConnectedState::Exit"));
    /* If a scan result is pending in connected state, the supplicant
     * is in SCAN_ONLY_MODE. Restore CONNECT_MODE on exit
     */
    if (mOwner->mScanResultIsPending) {
        mOwner->mWifiNative->SetScanResultHandling(CONNECT_MODE);
    }
    return NOERROR;
}


//==============================================================
// WifiStateMachine::ObtainingIpState
//==============================================================
ECode WifiStateMachine::ObtainingIpState::Enter()
{
    // EventLog.writeEvent(EVENTLOG_WIFI_STATE_CHANGED, getName());

    if (!mOwner->mWifiConfigStore->IsUsingStaticIp(mOwner->mLastNetworkId)) {
        //start DHCP
        if (mOwner->mDhcpStateMachine == NULL) {
            mOwner->mDhcpStateMachine = DhcpStateMachine::MakeDhcpStateMachine(
                    mOwner->mContext, mOwner, mOwner->mInterfaceName);

        }
        mOwner->mDhcpStateMachine->RegisterForPreDhcpNotification();
        mOwner->mDhcpStateMachine->SendMessage(DhcpStateMachine::CMD_START_DHCP);
        if (DBG) Logger::D("WifiStateMachine::ObtainingIpState",
            "Enter CMD_START_DHCP");
    }
    else {
        AutoPtr<DhcpInfoInternal> dhcpInfoInternal = mOwner->mWifiConfigStore->GetIpConfiguration(
                mOwner->mLastNetworkId);
        AutoPtr<IInterfaceConfiguration> ifcg;
        CInterfaceConfiguration::New((IInterfaceConfiguration**)&ifcg);
        AutoPtr<ILinkAddress> addr = dhcpInfoInternal->MakeLinkAddress();
        ifcg->SetLinkAddress(addr);
        ifcg->SetInterfaceUp();
        // try {
        if (mOwner->mNwService) {
            mOwner->mNwService->SetInterfaceConfig(mOwner->mInterfaceName, ifcg);
        }
        if (DBG) Logger::D("WifiStateMachine::ObtainingIpState",
            "Enter CMD_STATIC_IP_SUCCESS Static IP configuration succeeded");
        mOwner->SendMessage(CMD_STATIC_IP_SUCCESS, dhcpInfoInternal);
        // } catch (RemoteException re) {
        //     loge("Static IP configuration failed: " + re);
        //     sendMessage(CMD_STATIC_IP_FAILURE);
        // } catch (IllegalStateException e) {
        //     loge("Static IP configuration failed: " + e);
        //     sendMessage(CMD_STATIC_IP_FAILURE);
        // }
    }
    return NOERROR;
}

ECode WifiStateMachine::ObtainingIpState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    Int32 what;
    msg->GetWhat(&what);
    switch(what) {
        case CMD_STATIC_IP_SUCCESS: {
            if (DBG) Logger::D("WifiStateMachine::ObtainingIpState", "ProcessMessage CMD_STATIC_IP_SUCCESS");
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            mOwner->HandleSuccessfulIpConfiguration((DhcpInfoInternal*)obj.Get());
            mOwner->TransitionTo(mOwner->mVerifyingLinkState);
            break;
        }
        case CMD_STATIC_IP_FAILURE:
            if (DBG) Logger::D("WifiStateMachine::ObtainingIpState", "ProcessMessage CMD_STATIC_IP_FAILURE");
            mOwner->HandleFailedIpConfiguration();
            mOwner->TransitionTo(mOwner->mDisconnectingState);
            break;
        case IWifiManager::SAVE_NETWORK:
            if (DBG) Logger::D("WifiStateMachine::ObtainingIpState", "ProcessMessage SAVE_NETWORK");
            mOwner->DeferMessage(msg);
            break;
            /* Defer any power mode changes since we must keep active power mode at DHCP */
        case CMD_SET_HIGH_PERF_MODE:
            if (DBG) Logger::D("WifiStateMachine::ObtainingIpState", "ProcessMessage CMD_SET_HIGH_PERF_MODE");
            mOwner->DeferMessage(msg);
            break;
        default:
            DebugProcessMessage("WifiStateMachine::ObtainingIpState", what);

            *result = NOT_HANDLED;
            return NOERROR;
    }
    *result = HANDLED;
    return NOERROR;
}


//==============================================================
// WifiStateMachine::VerifyingLinkState
//==============================================================
ECode WifiStateMachine::VerifyingLinkState::Enter()
{
    if (DBG) Logger::D("WifiStateMachine::VerifyingLinkState", "Enter");

    // EventLog.writeEvent(EVENTLOG_WIFI_STATE_CHANGED, getName());
    mOwner->SetNetworkDetailedState(NetworkInfoDetailedState_VERIFYING_POOR_LINK);
    mOwner->mWifiConfigStore->UpdateStatus(mOwner->mLastNetworkId, NetworkInfoDetailedState_VERIFYING_POOR_LINK);
    mOwner->SendNetworkStateChangeBroadcast(mOwner->mLastBssid);
    return NOERROR;
}

ECode WifiStateMachine::VerifyingLinkState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case WifiWatchdogStateMachine::POOR_LINK_DETECTED:
            if (DBG) Logger::D("WifiStateMachine::VerifyingLinkState",
                "ProcessMessage POOR_LINK_DETECTED");
            //stay here
            break;
        case WifiWatchdogStateMachine::GOOD_LINK_DETECTED:
            if (DBG) Logger::D("WifiStateMachine::VerifyingLinkState",
                "ProcessMessage GOOD_LINK_DETECTED");
            mOwner->TransitionTo(mOwner->mCaptivePortalCheckState);
            break;
        default:
            // DebugProcessMessage("WifiStateMachine::VerifyingLinkState", what);

            *result = NOT_HANDLED;
            return NOERROR;
    }
    *result = HANDLED;
    return NOERROR;
}


//==============================================================
// WifiStateMachine::CaptivePortalCheckState
//==============================================================
ECode WifiStateMachine::CaptivePortalCheckState::Enter()
{
    if (DBG) mOwner->Log(String("CaptivePortalCheckState::Enter"));
    mOwner->SetNetworkDetailedState(NetworkInfoDetailedState_CAPTIVE_PORTAL_CHECK);
    mOwner->mWifiConfigStore->UpdateStatus(mOwner->mLastNetworkId, NetworkInfoDetailedState_CAPTIVE_PORTAL_CHECK);
    mOwner->SendNetworkStateChangeBroadcast(mOwner->mLastBssid);
    return NOERROR;
}

ECode WifiStateMachine::CaptivePortalCheckState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case CMD_CAPTIVE_CHECK_COMPLETE:
            if (DBG) Logger::D("WifiStateMachine::CaptivePortalCheckState",
                "ProcessMessage CMD_CAPTIVE_CHECK_COMPLETE");

            if (mOwner->mNwService) {
                ECode ec = mOwner->mNwService->DisableIpv6(mOwner->mInterfaceName);
                if (FAILED(ec)) {
                    Logger::E("WifiStateMachine::CaptivePortalCheckState", "Failed to disable IPv6: %08x", ec);
                }
            }

            /* Report a connect */
            mOwner->SetNetworkDetailedState(NetworkInfoDetailedState_CONNECTED);
            mOwner->mWifiConfigStore->UpdateStatus(mOwner->mLastNetworkId, NetworkInfoDetailedState_CONNECTED);
            mOwner->SendNetworkStateChangeBroadcast(mOwner->mLastBssid);
            mOwner->TransitionTo(mOwner->mConnectedState);
            break;

        default:
            DebugProcessMessage("WifiStateMachine::CaptivePortalCheckState", what);

            *result = NOT_HANDLED;
            return NOERROR;
    }
    *result = HANDLED;
    return NOERROR;
}


//==============================================================
// WifiStateMachine::ConnectedState
//==============================================================
ECode WifiStateMachine::ConnectedState::Enter()
{
    // if (DBG) log(getName() + "\n");
    if (DBG) mOwner->Log(String("ConnectedState::Enter"));
    // EventLog.writeEvent(EVENTLOG_WIFI_STATE_CHANGED, getName());
    return NOERROR;
}

ECode WifiStateMachine::ConnectedState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    Int32 what;
    msg->GetWhat(&what);

    switch (what) {
        case WifiWatchdogStateMachine::POOR_LINK_DETECTED:
            if (DBG) Logger::D("WifiStateMachine::ConnectedState",
                "ProcessMessage POOR_LINK_DETECTED");

            if (mOwner->mNwService) {
                ECode ec = mOwner->mNwService->DisableIpv6(mOwner->mInterfaceName);
                if (FAILED(ec)) {
                    Logger::E("WifiStateMachine::ConnectedState", "Failed to disable IPv6: %08x", ec);
                }
            }

            /* Report a disconnect */
            mOwner->SetNetworkDetailedState(NetworkInfoDetailedState_DISCONNECTED);
            mOwner->mWifiConfigStore->UpdateStatus(mOwner->mLastNetworkId, NetworkInfoDetailedState_DISCONNECTED);
            mOwner->SendNetworkStateChangeBroadcast(mOwner->mLastBssid);
            mOwner->TransitionTo(mOwner->mVerifyingLinkState);
            break;
        default:
            // DebugProcessMessage("WifiStateMachine::ConnectedState", what);

            *result = NOT_HANDLED;
            return NOERROR;
    }
    *result = HANDLED;
    return NOERROR;
}

ECode WifiStateMachine::ConnectedState::Exit()
{
    if (DBG) mOwner->Log(String("ConnectedState::Exit"));
    /* Request a CS wakelock during transition to mobile */
    mOwner->CheckAndSetConnectivityInstance();
    Boolean result;
    mOwner->mConnectivityManager->RequestNetworkTransitionWakelock(TAG, &result);
    return NOERROR;
}


//==============================================================
// WifiStateMachine::DisconnectingState
//==============================================================
ECode WifiStateMachine::DisconnectingState::Enter()
{
    // if (DBG) log(getName() + "\n");
    if (DBG) mOwner->Log(String("DisconnectingState::Enter"));
    // EventLog.writeEvent(EVENTLOG_WIFI_STATE_CHANGED, getName());
    return NOERROR;
}

ECode WifiStateMachine::DisconnectingState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case CMD_SET_SCAN_MODE: {
            if (DBG) Logger::D("WifiStateMachine::DisconnectingState",
                "ProcessMessage CMD_SET_SCAN_MODE");

            Int32 arg1;
            msg->GetArg1(&arg1);
            if (arg1 == SCAN_ONLY_MODE) {
                mOwner->DeferMessage(msg);
            }
            break;
        }
        case WifiMonitor::SUPPLICANT_STATE_CHANGE_EVENT:
            if (DBG) Logger::D("WifiStateMachine::DisconnectingState",
                "ProcessMessage SUPPLICANT_STATE_CHANGE_EVENT");

            /* If we get a SUPPLICANT_STATE_CHANGE_EVENT before NETWORK_DISCONNECTION_EVENT
             * we have missed the network disconnection, transition to mDisconnectedState
             * and handle the rest of the events there
             */
            mOwner->DeferMessage(msg);
            mOwner->HandleNetworkDisconnect();
            mOwner->TransitionTo(mOwner->mDisconnectedState);
            break;
        default:
            DebugProcessMessage("WifiStateMachine::DisconnectingState", what);
            *result = NOT_HANDLED;
            return NOERROR;
    }
    *result = HANDLED;
    return NOERROR;
}


//==============================================================
// WifiStateMachine::DisconnectedState
//==============================================================
void WifiStateMachine::DisconnectedState::SetScanAlarm(
    /* [in] */ Boolean enabled)
{
    if (enabled == mAlarmEnabled) {
        return;
    }
    if (enabled) {
        AutoPtr<ISystem> system;
        Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
        Int64 millis;
        system->GetCurrentTimeMillis(&millis);
        if (mFrameworkScanIntervalMs > 0) {
            mOwner->mAlarmManager->SetRepeating(IAlarmManager::RTC_WAKEUP,
                millis + mFrameworkScanIntervalMs,
                mFrameworkScanIntervalMs,
                mOwner->mScanIntent);
            mAlarmEnabled = TRUE;
        }
    }
    else {
        mOwner->mAlarmManager->Cancel(mOwner->mScanIntent);
        mAlarmEnabled = FALSE;
    }
}

ECode WifiStateMachine::DisconnectedState::Enter()
{
    // if (DBG) log(getName() + "\n");
    if (DBG) mOwner->Log(String("DisconnectedState::Enter"));
    // EventLog.writeEvent(EVENTLOG_WIFI_STATE_CHANGED, getName());

    // We dont scan frequently if this is a temporary disconnect
    // due to p2p
    if (mOwner->mTemporarilyDisconnectWifi) {
        mOwner->mWifiP2pChannel->SendMessage(IWifiP2pService::DISCONNECT_WIFI_RESPONSE);
        return NOERROR;
    }

    // TODO
    // mFrameworkScanIntervalMs = Settings.Global.getLong(mContext.getContentResolver(),
    //         Settings.Global.WIFI_FRAMEWORK_SCAN_INTERVAL_MS,
    //         mDefaultFrameworkScanIntervalMs);
    mFrameworkScanIntervalMs = mOwner->mDefaultFrameworkScanIntervalMs;
    /*
     * We initiate background scanning if it is enabled, otherwise we
     * initiate an infrequent scan that wakes up the device to ensure
     * a user connects to an access point on the move
     */
    if (mOwner->mEnableBackgroundScan) {
        /* If a regular scan result is pending, do not initiate background
         * scan until the scan results are returned. This is needed because
         * initiating a background scan will cancel the regular scan and
         * scan results will not be returned until background scanning is
         * cleared
         */
        if (!mOwner->mScanResultIsPending) {
            mOwner->mWifiNative->EnableBackgroundScan(TRUE);
        }
    }
    else {
        SetScanAlarm(TRUE);
    }

    /**
     * If we have no networks saved, the supplicant stops doing the periodic scan.
     * The scans are useful to notify the user of the presence of an open network.
     * Note that these are not wake up scans.
     */
    Boolean connected;
    Int32 size;
    if ((mOwner->mP2pConnected->Get(&connected), !connected) &&
            (mOwner->mWifiConfigStore->GetConfiguredNetworks()->GetObjectCount(&size), size == 0)) {
        AutoPtr<IMessage> m = mOwner->ObtainMessage(CMD_NO_NETWORKS_PERIODIC_SCAN,
                ++mOwner->mPeriodicScanToken, 0);
        mOwner->SendMessageDelayed(m, mOwner->mSupplicantScanIntervalMs);
    }
    return NOERROR;
}

ECode WifiStateMachine::DisconnectedState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    Boolean ret = HANDLED;
    Int32 what;
    msg->GetWhat(&what);
    DebugProcessMessage("WifiStateMachine::DisconnectedState", what);

    switch (what) {
        case CMD_NO_NETWORKS_PERIODIC_SCAN: {
            Boolean connected;
            if (mOwner->mP2pConnected->Get(&connected), connected) break;
            Int32 arg1, size;
            if ((msg->GetArg1(&arg1), arg1 == mOwner->mPeriodicScanToken) &&
                    (mOwner->mWifiConfigStore->GetConfiguredNetworks()->GetObjectCount(&size), size == 0)) {
                mOwner->SendMessage(CMD_START_SCAN);
                AutoPtr<IMessage> m = mOwner->ObtainMessage(CMD_NO_NETWORKS_PERIODIC_SCAN,
                    ++mOwner->mPeriodicScanToken, 0);
                mOwner->SendMessageDelayed(m, mOwner->mSupplicantScanIntervalMs);
            }
            break;
        }
        case IWifiManager::FORGET_NETWORK:
        case CMD_REMOVE_NETWORK: {
            // Set up a delayed message here. After the forget/remove is handled
            // the handled delayed message will determine if there is a need to
            // scan and continue
            AutoPtr<IMessage> m = mOwner->ObtainMessage(CMD_NO_NETWORKS_PERIODIC_SCAN,
                ++mOwner->mPeriodicScanToken, 0);
            mOwner->SendMessageDelayed(m, mOwner->mSupplicantScanIntervalMs);
            ret = NOT_HANDLED;
            break;
        }
        case CMD_SET_SCAN_MODE: {
            Int32 arg1;
            msg->GetArg1(&arg1);
            if (arg1 == SCAN_ONLY_MODE) {
                mOwner->mWifiNative->SetScanResultHandling(arg1);
                //Supplicant disconnect to prevent further connects
                mOwner->mWifiNative->Disconnect();
                mOwner->mIsScanMode = TRUE;
                mOwner->TransitionTo(mOwner->mScanModeState);
            }
            break;
        }
        case CMD_ENABLE_BACKGROUND_SCAN: {
            Int32 arg1;
            msg->GetArg1(&arg1);
            mOwner->mEnableBackgroundScan = (arg1 == 1);
            if (mOwner->mEnableBackgroundScan) {
                mOwner->mWifiNative->EnableBackgroundScan(TRUE);
                SetScanAlarm(FALSE);
            }
            else {
                mOwner->mWifiNative->EnableBackgroundScan(FALSE);
                SetScanAlarm(TRUE);
            }
            break;
        }
            /* Ignore network disconnect */
        case WifiMonitor::NETWORK_DISCONNECTION_EVENT:
            break;
        case WifiMonitor::SUPPLICANT_STATE_CHANGE_EVENT: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            IStateChangeResult* stateChangeResult = IStateChangeResult::Probe(obj.Get());
            AutoPtr<IWifiInfoHelper> helper;
            CWifiInfoHelper::AcquireSingleton((IWifiInfoHelper**)&helper);
            NetworkInfoDetailedState detailedState;
            SupplicantState state;
            stateChangeResult->GetSupplicantState(&state);
            helper->GetDetailedStateOf(state, &detailedState);
            mOwner->SetNetworkDetailedState(detailedState);
            /* ConnectModeState does the rest of the handling */
            ret = NOT_HANDLED;
            break;
        }
        case CMD_START_SCAN:
            /* Disable background scan temporarily during a regular scan */
            if (mOwner->mEnableBackgroundScan) {
                mOwner->mWifiNative->EnableBackgroundScan(FALSE);
            }
            /* Handled in parent state */
            ret = NOT_HANDLED;
            break;
        case WifiMonitor::SCAN_RESULTS_EVENT:
            /* Re-enable background scan when a pending scan result is received */
            if (mOwner->mEnableBackgroundScan && mOwner->mScanResultIsPending) {
                mOwner->mWifiNative->EnableBackgroundScan(TRUE);
            }
            /* Handled in parent state */
            ret = NOT_HANDLED;
            break;
        case IWifiP2pService::P2P_CONNECTION_CHANGED: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            AutoPtr<INetworkInfo> info = INetworkInfo::Probe(obj);
            Boolean isConnected;
            info->IsConnected(&isConnected);
            mOwner->mP2pConnected->Set(isConnected);
            mOwner->mP2pConnected->Get(&isConnected);
            if (isConnected) {
                Int32 defaultInterval;
                AutoPtr<IContentResolver> cr;
                mOwner->mContext->GetContentResolver((IContentResolver**)&cr);
                AutoPtr<IResources> res;
                mOwner->mContext->GetResources((IResources**)&res);
                res->GetInteger(R::integer::config_wifi_scan_interval_p2p_connected, &defaultInterval);
                AutoPtr<ISettingsGlobal> global;
                CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
                Int64 scanIntervalMs;
                global->GetInt64(cr, ISettingsGlobal::WIFI_SCAN_INTERVAL_WHEN_P2P_CONNECTED_MS,
                   (Int64)defaultInterval, &scanIntervalMs);

                mOwner->mWifiNative->SetScanInterval((Int32) scanIntervalMs/1000);
            }
            else {
                AutoPtr<IObjectContainer> c = mOwner->mWifiConfigStore->GetConfiguredNetworks();
                Int32 size = 0;
                if (c) c->GetObjectCount(&size);
                if (size == 0) {
                    if (DBG) mOwner->Log(String("Turn on scanning after p2p disconnected"));
                    AutoPtr<IMessage> m = mOwner->ObtainMessage(CMD_NO_NETWORKS_PERIODIC_SCAN,
                        ++mOwner->mPeriodicScanToken, 0);
                    mOwner->SendMessageDelayed(m, mOwner->mSupplicantScanIntervalMs);
                }
            }
        }
        case CMD_RECONNECT:
        case CMD_REASSOCIATE:
            // Drop a third party reconnect/reassociate if we are
            // tempoarily disconnected for p2p
            if (mOwner->mTemporarilyDisconnectWifi) ret = NOT_HANDLED;
            break;
        default:
            ret = NOT_HANDLED;
    }
    *result = ret;
    return NOERROR;
}

ECode WifiStateMachine::DisconnectedState::Exit()
{
    if (DBG) mOwner->Log(String("DisconnectedState::ProcessMessage"));
    /* No need for a background scan upon exit from a disconnected state */
    if (mOwner->mEnableBackgroundScan) {
        mOwner->mWifiNative->EnableBackgroundScan(FALSE);
    }
    SetScanAlarm(FALSE);
    return NOERROR;
}


//==============================================================
// WifiStateMachine::WpsRunningState
//==============================================================
ECode WifiStateMachine::WpsRunningState::Enter()
{
    // if (DBG) log(getName() + "\n");
    if (DBG) mOwner->Log(String("WpsRunningState::Enter"));
    // EventLog.writeEvent(EVENTLOG_WIFI_STATE_CHANGED, getName());
    AutoPtr<IMessageHelper> helper;
    CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
    helper->Obtain(mOwner->GetCurrentMessage(), (IMessage**)&mSourceMessage);
    return NOERROR;
}

ECode WifiStateMachine::WpsRunningState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    Int32 what;
    msg->GetWhat(&what);
    DebugProcessMessage("WifiStateMachine::WpsRunningState", what);

    switch (what) {
        case WifiMonitor::WPS_SUCCESS_EVENT:
            mOwner->ReplyToMessage(mSourceMessage, IWifiManager::WPS_COMPLETED);
            mSourceMessage->Recycle();
            mSourceMessage = NULL;
            mOwner->TransitionTo(mOwner->mDisconnectedState);
            break;
        case WifiMonitor::WPS_OVERLAP_EVENT:
            mOwner->ReplyToMessage(mSourceMessage, IWifiManager::WPS_FAILED,
                    IWifiManager::WPS_OVERLAP_ERROR);
            mSourceMessage->Recycle();
            mSourceMessage = NULL;
            mOwner->TransitionTo(mOwner->mDisconnectedState);
            break;
        case WifiMonitor::WPS_FAIL_EVENT: {
            //arg1 has the reason for the failure
            Int32 arg1;
            msg->GetArg1(&arg1);
            mOwner->ReplyToMessage(mSourceMessage, IWifiManager::WPS_FAILED, arg1);
            mSourceMessage->Recycle();
            mSourceMessage = NULL;
            mOwner->TransitionTo(mOwner->mDisconnectedState);
            break;
        }
        case WifiMonitor::WPS_TIMEOUT_EVENT:
            mOwner->ReplyToMessage(mSourceMessage, IWifiManager::WPS_FAILED,
                    IWifiManager::WPS_TIMED_OUT);
            mSourceMessage->Recycle();
            mSourceMessage = NULL;
            mOwner->TransitionTo(mOwner->mDisconnectedState);
            break;
        case IWifiManager::START_WPS:
            mOwner->ReplyToMessage(msg, IWifiManager::WPS_FAILED, IWifiManager::IN_PROGRESS);
            break;
        case IWifiManager::CANCEL_WPS:
            if (mOwner->mWifiNative->CancelWps()) {
                mOwner->ReplyToMessage(msg, IWifiManager::CANCEL_WPS_SUCCEDED);
            }
            else {
                mOwner->ReplyToMessage(msg, IWifiManager::CANCEL_WPS_FAILED, IWifiManager::ERROR);
            }
            mOwner->TransitionTo(mOwner->mDisconnectedState);
            break;
        /* Defer all commands that can cause connections to a different network
         * or put the state machine out of connect mode
         */
        case CMD_STOP_DRIVER:
        case CMD_SET_SCAN_MODE:
        case IWifiManager::CONNECT_NETWORK:
        case CMD_ENABLE_NETWORK:
        case CMD_RECONNECT:
        case CMD_REASSOCIATE:
        case WifiMonitor::NETWORK_CONNECTION_EVENT: /* Handled after exiting WPS state */
            mOwner->DeferMessage(msg);
            break;
        case WifiMonitor::NETWORK_DISCONNECTION_EVENT:
            if (DBG) mOwner->Log(String("Network connection lost"));
            mOwner->HandleNetworkDisconnect();
            break;
        case WifiMonitor::AUTHENTICATION_FAILURE_EVENT:
            // Disregard auth failure events during WPS connection. The
            // EAP sequence is retried several times, and there might be
            // failures (especially for wps pin). We will get a WPS_XXX
            // event at the end of the sequence anyway.
            if (DBG) mOwner->Log(String("Ignore auth failure during WPS connection"));
            break;
        case WifiMonitor::SUPPLICANT_STATE_CHANGE_EVENT:
            //Throw away supplicant state changes when WPS is running.
            //We will start getting supplicant state changes once we get
            //a WPS success or failure
            break;
        default:
            *result = NOT_HANDLED;
            return NOERROR;
    }
    *result = HANDLED;
    return NOERROR;
}

ECode WifiStateMachine::WpsRunningState::Exit()
{
    if (DBG) mOwner->Log(String("WpsRunningState::Exit"));
    mOwner->mWifiConfigStore->EnableAllNetworks();
    mOwner->mWifiConfigStore->LoadConfiguredNetworks();
    return NOERROR;
}


//==============================================================
// WifiStateMachine::SoftApStartingState
//==============================================================
ECode WifiStateMachine::SoftApStartingState::Enter()
{
    // if (DBG) log(getName() + "\n");
    if (DBG) mOwner->Log(String("SoftApStartingState::Enter"));
    // EventLog.writeEvent(EVENTLOG_WIFI_STATE_CHANGED, getName());

    AutoPtr<IMessage> message = mOwner->GetCurrentMessage();
    Int32 what;
    message->GetWhat(&what);
    if (what == CMD_START_AP) {
        AutoPtr<IInterface> obj;
        message->GetObj((IInterface**)&obj);
        IWifiConfiguration* config = IWifiConfiguration::Probe(obj);

        if (config == NULL) {
            mOwner->mWifiApConfigChannel->SendMessage(CMD_REQUEST_AP_CONFIG);
        }
        else {
            mOwner->mWifiApConfigChannel->SendMessage(CMD_SET_AP_CONFIG, config);
            mOwner->StartSoftApWithConfig(config);
        }
        return NOERROR;
    }
    else {
        // throw new RuntimeException("Illegal transition to SoftApStartingState: " + message);
        return E_RUNTIME_EXCEPTION;
    }
}

ECode WifiStateMachine::SoftApStartingState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    Int32 what;
    msg->GetWhat(&what);
    DebugProcessMessage("WifiStateMachine::SoftApStartingState", what);

    switch(what) {
        case CMD_LOAD_DRIVER:
        case CMD_UNLOAD_DRIVER:
        case CMD_START_SUPPLICANT:
        case CMD_STOP_SUPPLICANT:
        case CMD_START_AP:
        case CMD_STOP_AP:
        case CMD_START_DRIVER:
        case CMD_STOP_DRIVER:
        case CMD_SET_SCAN_MODE:
        case CMD_SET_SCAN_TYPE:
        case CMD_SET_COUNTRY_CODE:
        case CMD_SET_FREQUENCY_BAND:
        case CMD_START_PACKET_FILTERING:
        case CMD_STOP_PACKET_FILTERING:
        case CMD_TETHER_STATE_CHANGE:
            mOwner->DeferMessage(msg);
            break;
        case WifiStateMachine::CMD_RESPONSE_AP_CONFIG: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            IWifiConfiguration* config = IWifiConfiguration::Probe(obj);
            if (config != NULL) {
                mOwner->StartSoftApWithConfig(config);
            }
            else {
                mOwner->Loge(String("Softap config is null!"));
                mOwner->SendMessage(CMD_START_AP_FAILURE);
            }
            break;
        }
        case CMD_START_AP_SUCCESS:
            mOwner->SetWifiApState(IWifiManager::WIFI_AP_STATE_ENABLED);
            mOwner->TransitionTo(mOwner->mSoftApStartedState);
            break;
        case CMD_START_AP_FAILURE: {
            // initiate driver unload
            AutoPtr<IMessage> m = mOwner->ObtainMessage(CMD_UNLOAD_DRIVER,
                    IWifiManager::WIFI_AP_STATE_FAILED, 0);
            mOwner->SendMessage(m);
            break;
        }
        default:
            *result = NOT_HANDLED;
            return NOERROR;
    }
    *result = HANDLED;
    return NOERROR;
}


//==============================================================
// WifiStateMachine::SoftApStartedState
//==============================================================
ECode WifiStateMachine::SoftApStartedState::Enter()
{
    // if (DBG) log(getName() + "\n");
    if (DBG) mOwner->Log(String("SoftApStartedState::Enter"));
    // EventLog.writeEvent(EVENTLOG_WIFI_STATE_CHANGED, getName());
    return NOERROR;
}

ECode WifiStateMachine::SoftApStartedState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    Int32 what;
    msg->GetWhat(&what);
    DebugProcessMessage("WifiStateMachine::SoftApStartedState", what);

    switch(what) {
        case CMD_STOP_AP:
            if (DBG) mOwner->Log(String("Stopping Soft AP"));
            mOwner->SetWifiApState(IWifiManager::WIFI_AP_STATE_DISABLING);

            /* We have not tethered at this point, so we just shutdown soft Ap */
            // try {
            if (mOwner->mNwService) {
                mOwner->mNwService->StopAccessPoint(mOwner->mInterfaceName);
            }
            // } catch(Exception e) {
            //     loge("Exception in stopAccessPoint()");
            // }
            mOwner->TransitionTo(mOwner->mDriverLoadedState);
            break;
        case CMD_START_AP:
            // Ignore a start on a running access point
            break;
            /* Fail client mode operation when soft AP is enabled */
        case CMD_START_SUPPLICANT:
            mOwner->Loge(String("Cannot start supplicant with a running soft AP"));
            mOwner->SetWifiState(IWifiManager::WIFI_STATE_UNKNOWN);
            break;
        case CMD_TETHER_STATE_CHANGE: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            AutoPtr<TetherStateChange> stateChange = (TetherStateChange*)obj.Get();
            if (mOwner->StartTethering(stateChange->mAvailable)) {
                mOwner->TransitionTo(mOwner->mTetheringState);
            }
            break;
        }
        default:
            *result = NOT_HANDLED;
            return NOERROR;
    }
    *result = HANDLED;
    return NOERROR;
}


//==============================================================
// WifiStateMachine::TetheringState
//==============================================================
ECode WifiStateMachine::TetheringState::Enter()
{
    // if (DBG) log(getName() + "\n");
    if (DBG) mOwner->Log(String("TetheringState::Enter"));
    // EventLog.writeEvent(EVENTLOG_WIFI_STATE_CHANGED, getName());

    /* Send ourselves a delayed message to shut down if tethering fails to notify */
    AutoPtr<IMessage> m = mOwner->ObtainMessage(CMD_TETHER_NOTIFICATION_TIMED_OUT,
            ++mOwner->mTetherToken, 0);
    mOwner->SendMessageDelayed(m, TETHER_NOTIFICATION_TIME_OUT_MSECS);
    return NOERROR;
}

ECode WifiStateMachine::TetheringState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    Int32 what;
    msg->GetWhat(&what);
    DebugProcessMessage("WifiStateMachine::TetheringState", what);

    switch(what) {
        case CMD_TETHER_STATE_CHANGE: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            AutoPtr<TetherStateChange> stateChange = (TetherStateChange*)obj.Get();
            if (mOwner->IsWifiTethered(stateChange->mActive)) {
                mOwner->TransitionTo(mOwner->mTetheredState);
            }
            *result = HANDLED;
            return NOERROR;
        }
        case CMD_TETHER_NOTIFICATION_TIMED_OUT: {
            Int32 arg1;
            msg->GetArg1(&arg1);
            if (arg1 == mOwner->mTetherToken) {
                mOwner->Loge(String("Failed to get tether update, shutdown soft access point"));
                mOwner->SetWifiApEnabled(NULL, FALSE);
            }
            break;
        }
        case CMD_LOAD_DRIVER:
        case CMD_UNLOAD_DRIVER:
        case CMD_START_SUPPLICANT:
        case CMD_STOP_SUPPLICANT:
        case CMD_START_AP:
        case CMD_STOP_AP:
        case CMD_START_DRIVER:
        case CMD_STOP_DRIVER:
        case CMD_SET_SCAN_MODE:
        case CMD_SET_SCAN_TYPE:
        case CMD_SET_COUNTRY_CODE:
        case CMD_SET_FREQUENCY_BAND:
        case CMD_START_PACKET_FILTERING:
        case CMD_STOP_PACKET_FILTERING:
            mOwner->DeferMessage(msg);
            break;
        default:
            *result = NOT_HANDLED;
            return NOERROR;
    }
    *result = HANDLED;
    return NOERROR;
}


//==============================================================
// WifiStateMachine::TetheredState
//==============================================================
ECode WifiStateMachine::TetheredState::Enter()
{
    // if (DBG) log(getName() + "\n");
    if (DBG) mOwner->Log(String("TetheredState::Enter"));
    // EventLog.writeEvent(EVENTLOG_WIFI_STATE_CHANGED, getName());
    return NOERROR;
}

ECode WifiStateMachine::TetheredState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    Int32 what;
    msg->GetWhat(&what);
    DebugProcessMessage("WifiStateMachine::TetheredState", what);

    switch(what) {
        case CMD_TETHER_STATE_CHANGE: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            AutoPtr<TetherStateChange> stateChange = (TetherStateChange*)obj.Get();
            if (!mOwner->IsWifiTethered(stateChange->mActive)) {
                mOwner->Loge(String("Tethering reports wifi as untethered!, shut down soft Ap"));
                mOwner->SetWifiApEnabled(NULL, FALSE);
            }
            *result = HANDLED;
            return NOERROR;
        }
        case CMD_STOP_AP:
            if (DBG) mOwner->Log(String("Untethering before stopping AP"));
            mOwner->SetWifiApState(IWifiManager::WIFI_AP_STATE_DISABLING);
            mOwner->StopTethering();
            mOwner->TransitionTo(mOwner->mSoftApStoppingState);
            break;
        default:
            *result = NOT_HANDLED;
            return NOERROR;
    }
    *result = HANDLED;
    return NOERROR;
}


//==============================================================
// WifiStateMachine::SoftApStoppingState
//==============================================================
ECode WifiStateMachine::SoftApStoppingState::Enter()
{
    // if (DBG) log(getName() + "\n");
    if (DBG) mOwner->Log(String("SoftApStoppingState::Enter"));
    // EventLog.writeEvent(EVENTLOG_WIFI_STATE_CHANGED, getName());

    /* Send ourselves a delayed message to shut down if tethering fails to notify */
    AutoPtr<IMessage> m = mOwner->ObtainMessage(CMD_TETHER_NOTIFICATION_TIMED_OUT,
            ++mOwner->mTetherToken, 0);
    mOwner->SendMessageDelayed(m, TETHER_NOTIFICATION_TIME_OUT_MSECS);
    return NOERROR;
}

ECode WifiStateMachine::SoftApStoppingState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    Int32 what;
    msg->GetWhat(&what);
    DebugProcessMessage("WifiStateMachine::SoftApStoppingState", what);

    switch(what) {
        case CMD_TETHER_STATE_CHANGE: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            AutoPtr<TetherStateChange> stateChange = (TetherStateChange*)obj.Get();

            /* Wait till wifi is untethered */
            if (mOwner->IsWifiTethered(stateChange->mActive)) break;

            // try {
            if (mOwner->mNwService) {
                mOwner->mNwService->StopAccessPoint(mOwner->mInterfaceName);
            }
            // } catch(Exception e) {
            //     loge("Exception in stopAccessPoint()");
            // }
            mOwner->TransitionTo(mOwner->mDriverLoadedState);
            break;
        }
        case CMD_TETHER_NOTIFICATION_TIMED_OUT: {
            Int32 arg1;
            msg->GetArg1(&arg1);
            if (arg1 == mOwner->mTetherToken) {
                mOwner->Loge(String("Failed to get tether update, force stop access point"));
                // try {
                if (mOwner->mNwService) {
                    mOwner->mNwService->StopAccessPoint(mOwner->mInterfaceName);
                }
                // } catch(Exception e) {
                //     loge("Exception in stopAccessPoint()");
                // }
                mOwner->TransitionTo(mOwner->mDriverLoadedState);
            }
            break;
        }
        case CMD_LOAD_DRIVER:
        case CMD_UNLOAD_DRIVER:
        case CMD_START_SUPPLICANT:
        case CMD_STOP_SUPPLICANT:
        case CMD_START_AP:
        case CMD_STOP_AP:
        case CMD_START_DRIVER:
        case CMD_STOP_DRIVER:
        case CMD_SET_SCAN_MODE:
        case CMD_SET_SCAN_TYPE:
        case CMD_SET_COUNTRY_CODE:
        case CMD_SET_FREQUENCY_BAND:
        case CMD_START_PACKET_FILTERING:
        case CMD_STOP_PACKET_FILTERING:
            mOwner->DeferMessage(msg);
            break;
        default:
            *result = NOT_HANDLED;
            return NOERROR;
    }
    *result = HANDLED;
    return NOERROR;
}


//==============================================================
// WifiStateMachine::Runnable
//==============================================================
CAR_INTERFACE_IMPL(WifiStateMachine::Runnable, IRunnable)

ECode WifiStateMachine::Runnable::Run()
{
    // try {
    if (mOwner->mNwService) {
        ECode ec = mOwner->mNwService->StartAccessPoint(mConfig, mOwner->mInterfaceName);
        if (FAILED(ec)) {
            mOwner->mNwService->StopAccessPoint(mOwner->mInterfaceName);
            mOwner->mNwService->StartAccessPoint(mConfig, mOwner->mInterfaceName);
        }
    }
    // } catch (Exception e) {
    //     loge("Exception in softap start " + e);
    //     try {
    //         mNwService.stopAccessPoint(mInterfaceName);
    //         mNwService.startAccessPoint(config, mInterfaceName);
    //     } catch (Exception e1) {
    //         loge("Exception in softap re-start " + e1);
    //         sendMessage(CMD_START_AP_FAILURE);
    //         return;
    //     }
    // }
    if (DBG) mOwner->Log(String("Soft AP start successful"));
    mOwner->SendMessage(CMD_START_AP_SUCCESS);
    return NOERROR;
}

//==============================================================
// WifiStateMachine
//==============================================================
WifiStateMachine::WifiContentObserver::WifiContentObserver(
    /* [in] */ IHandler* handler,
    /* [in] */ WifiStateMachine* owner)
    : ContentObserver(handler)
    , mOwner(owner)
{}

ECode WifiStateMachine::WifiContentObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    AutoPtr<IContentResolver> cr;
    mOwner->mContext->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<ISettingsGlobal> global;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
    Int32 ival;
    global->GetInt32(cr, ISettingsGlobal::WIFI_SUSPEND_OPTIMIZATIONS_ENABLED, 1, &ival);
    mOwner->mUserWantsSuspendOpt->Set(ival == 1);
    return NOERROR;
}

CAR_INTERFACE_IMPL(WifiStateMachine::TetherStateChange, IInterface)

//==============================================================
// WifiStateMachine::TetherReceiver
//==============================================================
WifiStateMachine::TetherReceiver::TetherReceiver(
    /* [in] */ WifiStateMachine* owner)
    : mOwner(owner)
{}

ECode WifiStateMachine::TetherReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    VALIDATE_NOT_NULL(intent);
    AutoPtr<IObjectContainer> available, active;
    intent->GetStringArrayListExtra(
        IConnectivityManager::EXTRA_AVAILABLE_TETHER,
        (IObjectContainer**)&available);
    intent->GetStringArrayListExtra(
        IConnectivityManager::EXTRA_ACTIVE_TETHER,
        (IObjectContainer**)&active);

    AutoPtr<ArrayOf<String> > avs, acs;
    Int32 count = 0;

    if (available) {
        available->GetObjectCount(&count);
        avs = ArrayOf<String>::Alloc(count);
        if (count > 0) {
            String str;
            ICharSequence* seq;
            AutoPtr<IObjectEnumerator> emu;
            available->GetObjectEnumerator((IObjectEnumerator**)&emu);
            Int32 i = 0;
            Boolean hasNext;
            while (emu->MoveNext(&hasNext), hasNext) {
                AutoPtr<IInterface> current;
                emu->Current((IInterface**)&current);
                seq = ICharSequence::Probe(current);
                seq->ToString(&str);
                avs->Set(i++, str);
            }
        }
    }
    else {
        avs = ArrayOf<String>::Alloc(0);
    }

    if (active) {
        active->GetObjectCount(&count);
        acs = ArrayOf<String>::Alloc(count);
        if (count > 0) {
            String str;
            ICharSequence* seq;
            AutoPtr<IObjectEnumerator> emu;
            active->GetObjectEnumerator((IObjectEnumerator**)&emu);
            Int32 i = 0;
            Boolean hasNext;
            while (emu->MoveNext(&hasNext), hasNext) {
                AutoPtr<IInterface> current;
                emu->Current((IInterface**)&current);
                seq = ICharSequence::Probe(current);
                seq->ToString(&str);
                acs->Set(i++, str);
            }
        }
    }
    else {
        acs = ArrayOf<String>::Alloc(0);
    }

    AutoPtr<TetherStateChange> state = new TetherStateChange(avs, acs);
    mOwner->SendMessage(CMD_TETHER_STATE_CHANGE, state);
    return NOERROR;
}

//==============================================================
// WifiStateMachine::ScanReceiver
//==============================================================
WifiStateMachine::ScanReceiver::ScanReceiver(
    /* [in] */ WifiStateMachine* owner)
    : mOwner(owner)
{}

ECode WifiStateMachine::ScanReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    VALIDATE_NOT_NULL(intent);

    mOwner->StartScan(FALSE);
    return NOERROR;
}

//==============================================================
// WifiStateMachine::ScreenReceiver
//==============================================================
WifiStateMachine::ScreenReceiver::ScreenReceiver(
    /* [in] */ WifiStateMachine* owner)
    : mOwner(owner)
{}

ECode WifiStateMachine::ScreenReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    VALIDATE_NOT_NULL(intent);

    String action;
    intent->GetAction(&action);

    if (action.Equals(IIntent::ACTION_SCREEN_ON)) {
        mOwner->HandleScreenStateChanged(TRUE);
    }
    else if (action.Equals(IIntent::ACTION_SCREEN_OFF)) {
        mOwner->HandleScreenStateChanged(FALSE);
    }
    return NOERROR;
}

//==============================================================
// WifiStateMachine::StopCounterReceiver
//==============================================================
WifiStateMachine::StopCounterReceiver::StopCounterReceiver(
    /* [in] */ WifiStateMachine* owner)
    : mOwner(owner)
{}

ECode WifiStateMachine::StopCounterReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    VALIDATE_NOT_NULL(intent);

    Int32 counter;
    intent->GetInt32Extra(DELAYED_STOP_COUNTER, 0, &counter);
    AutoPtr<IMessage> m = mOwner->ObtainMessage(CMD_DELAYED_STOP_DRIVER, counter, 0);
    mOwner->SendMessage(m);
    return NOERROR;
}

//==============================================================
// WifiStateMachine
//==============================================================
WifiStateMachine::WifiStateMachine(
    /* [in] */ IContext* context,
    /* [in] */ const String& wlanInterface)
    : StateMachine(TAG)
    , mP2pSupported(FALSE)
    , mTemporarilyDisconnectWifi(FALSE)
    , mScanResultCache(SCAN_RESULT_CACHE_SIZE)
    , mBackgroundScanSupported(FALSE)
    , mLastSignalLevel(-1)
    , mLastNetworkId(0)
    , mEnableRssiPolling(FALSE)
    , mEnableBackgroundScan(FALSE)
    , mRssiPollToken(0)
    , mReconnectCount(0)
    , mIsScanMode(FALSE)
    , mScanResultIsPending(FALSE)
    , mSetScanActive(FALSE)
    , mBluetoothConnectionActive(FALSE)
    , mSupplicantRestartCount(0)
    , mSupplicantStopFailureToken(0)
    , mTetherToken(0)
    , mDriverStartToken(0)
    , mPeriodicScanToken(0)
    , mSuspendOptNeedsDisabled(0)
    , mDefaultFrameworkScanIntervalMs(0)
    , mSupplicantScanIntervalMs(0)
    , mLastEnableAllNetworksTime(0)
    , mDriverStopDelayMs(0)
    , mDelayedStopCounter(0)
    , mInDelayedStop(FALSE)
    , mIsRunning(FALSE)
    , mReportedRunning(FALSE)
{
    AutoPtr<IUserHandleHelper> userHandleHelper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&userHandleHelper);
    userHandleHelper->GetALL((IUserHandle**)&USERHANDLE_ALL);

    CAtomicBoolean::New(FALSE, (IAtomicBoolean**)&mP2pConnected);
    CAtomicBoolean::New(FALSE, (IAtomicBoolean**)&mScreenBroadcastReceived);
    CAtomicInteger32::New(IWifiManager::WIFI_FREQUENCY_BAND_AUTO, (IAtomicInteger32**)&mFrequencyBand);
    CAtomicBoolean::New(TRUE, (IAtomicBoolean**)&mFilteringMulticastV4Packets);
    mReplyChannel = new AsyncChannel();
    mWifiP2pChannel = new AsyncChannel();
    mWifiApConfigChannel = new AsyncChannel();
    CAtomicBoolean::New(TRUE, (IAtomicBoolean**)&mUserWantsSuspendOpt);
    mDefaultState = new DefaultState(this);
    mInitialState = new InitialState(this);
    mDriverUnloadingState = new DriverUnloadingState(this);
    mDriverUnloadedState = new DriverUnloadedState(this);
    mDriverFailedState = new DriverFailedState(this);
    mDriverLoadingState = new DriverLoadingState(this);
    mDriverLoadedState = new DriverLoadedState(this);
    mSupplicantStartingState = new SupplicantStartingState(this);
    mSupplicantStartedState = new SupplicantStartedState(this);
    mSupplicantStoppingState = new SupplicantStoppingState(this);
    mDriverStartingState = new DriverStartingState(this);
    mDriverStartedState = new DriverStartedState(this);
    mWaitForP2pDisableState = new WaitForP2pDisableState(this);
    mDriverStoppingState = new DriverStoppingState(this);
    mDriverStoppedState = new DriverStoppedState(this);
    mScanModeState = new ScanModeState(this);
    mConnectModeState = new ConnectModeState(this);
    mL2ConnectedState = new L2ConnectedState(this);
    mObtainingIpState = new ObtainingIpState(this);
    mVerifyingLinkState = new VerifyingLinkState(this);
    mCaptivePortalCheckState = new CaptivePortalCheckState(this);
    mConnectedState = new ConnectedState(this);
    mDisconnectingState = new DisconnectingState(this);
    mDisconnectedState = new DisconnectedState(this);
    mWpsRunningState = new WpsRunningState(this);
    mSoftApStartingState = new SoftApStartingState(this);
    mSoftApStartedState = new SoftApStartedState(this);
    mTetheringState = new TetheringState(this);
    mTetheredState = new TetheredState(this);
    mSoftApStoppingState = new SoftApStoppingState(this);
    CAtomicInteger32::New(IWifiManager::WIFI_STATE_DISABLED, (IAtomicInteger32**)&mWifiState);
    CAtomicInteger32::New(IWifiManager::WIFI_AP_STATE_DISABLED, (IAtomicInteger32**)&mWifiApState);
    CAtomicInteger32::New(Process::MyUid(), (IAtomicInteger32**)&mLastEnableUid);
    CAtomicInteger32::New(Process::MyUid(), (IAtomicInteger32**)&mLastApEnableUid);
    CWorkSource::New((IWorkSource**)&mRunningWifiUids);
    CWorkSource::New((IWorkSource**)&mLastRunningWifiUids);

    mContext = context;
    mInterfaceName = wlanInterface;

    CNetworkInfo::New(IConnectivityManager::TYPE_WIFI, 0, NETWORKTYPE, String(""), (INetworkInfo**)&mNetworkInfo);
    AutoPtr<IInterface> obj = ServiceManager::GetService(String("batteryinfo"));
    mBatteryStats = IIBatteryStats::Probe(obj);

    obj = ServiceManager::GetService(IContext::NETWORKMANAGEMENT_SERVICE);
    mNwService = IINetworkManagementService::Probe(obj);

    AutoPtr<IPackageManager> pm;
    mContext->GetPackageManager((IPackageManager**)&pm);
    pm->HasSystemFeature(IPackageManager::FEATURE_WIFI_DIRECT, &mP2pSupported);

    Slogger::D("WifiStateMachine", " >>> P2pSupported %d", mP2pSupported);

    mWifiNative = new WifiNative(mInterfaceName);
    mWifiConfigStore = new WifiConfigStore(context, mWifiNative);
    mWifiMonitor = new WifiMonitor(this, mWifiNative);
    mDhcpInfoInternal = new DhcpInfoInternal();
    CWifiInfo::New((IWifiInfo**)&mWifiInfo);

    AutoPtr<IHandler> handler = GetHandler();
    AutoPtr<ILooper> l;
    handler->GetLooper((ILooper**)&l);
    mSupplicantStateTracker = new SupplicantStateTracker(context, this, mWifiConfigStore, l);
    CLinkProperties::New((ILinkProperties**)&mLinkProperties);

    AutoPtr<WifiApConfigStore> wifiApConfigStore =
        WifiApConfigStore::MakeWifiApConfigStore(mContext, handler);
    wifiApConfigStore->LoadApConfiguration();
    AutoPtr<IMessenger> msger;
    wifiApConfigStore->GetMessenger((IMessenger**)&msger);
    mWifiApConfigChannel->ConnectSync(mContext, handler, msger);

    mNetworkInfo->SetIsAvailable(FALSE);
    mLinkProperties->Clear();
    mLastBssid = NULL;
    mLastNetworkId = IWifiConfiguration::INVALID_NETWORK_ID;
    mLastSignalLevel = -1;

    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::ALARM_SERVICE, (IInterface**)&obj);
    mAlarmManager = IAlarmManager::Probe(obj);
    AutoPtr<IIntent> scanIntent;
    CIntent::New(ACTION_START_SCAN, NULL, (IIntent**)&scanIntent);
    AutoPtr<IPendingIntentHelper> helper;
    CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&helper);
    helper->GetBroadcast(mContext, SCAN_REQUEST, scanIntent, 0, (IPendingIntent**)&mScanIntent);

    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    res->GetInteger(R::integer::config_wifi_framework_scan_interval, &mDefaultFrameworkScanIntervalMs);
    res->GetInteger(R::integer::config_wifi_driver_stop_delay, &mDriverStopDelayMs);
    res->GetBoolean(R::bool_::config_wifi_background_scan_support, &mBackgroundScanSupported);
    res->GetString(R::string::config_wifi_p2p_device_type, &mPrimaryDeviceType);

    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<ISettingsGlobal> global;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);

    Int32 ivalue;
    global->GetInt32(cr, ISettingsGlobal::WIFI_SUSPEND_OPTIMIZATIONS_ENABLED, 1, &ivalue);
    mUserWantsSuspendOpt->Set(ivalue == 1);

    AutoPtr<IIntent> stickyIntent;
    AutoPtr<IBroadcastReceiver> receive = new TetherReceiver(this);
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New(IConnectivityManager::ACTION_TETHER_STATE_CHANGED, (IIntentFilter**)&filter);
    mContext->RegisterReceiver(receive, filter, (IIntent**)&stickyIntent);

    stickyIntent = NULL;
    receive = new TetherReceiver(this);
    AutoPtr<IIntentFilter> scanFilter;
    CIntentFilter::New(ACTION_START_SCAN, (IIntentFilter**)&scanFilter);
    mContext->RegisterReceiver(receive, scanFilter, (IIntent**)&stickyIntent);

    stickyIntent = NULL;
    AutoPtr<IIntentFilter> screenFilter;
    CIntentFilter::New((IIntentFilter**)&screenFilter);
    screenFilter->AddAction(IIntent::ACTION_SCREEN_ON);
    screenFilter->AddAction(IIntent::ACTION_SCREEN_OFF);
    receive = new ScreenReceiver(this);
    mContext->RegisterReceiver(receive, screenFilter, (IIntent**)&stickyIntent);

    stickyIntent = NULL;
    AutoPtr<IIntentFilter> counterFilter;
    CIntentFilter::New(ACTION_DELAYED_DRIVER_STOP, (IIntentFilter**)&counterFilter);
    receive = new StopCounterReceiver(this);
    mContext->RegisterReceiver(receive, counterFilter, (IIntent**)&stickyIntent);

    AutoPtr<IUri> uri;
    global->GetUriFor(ISettingsGlobal::WIFI_SUSPEND_OPTIMIZATIONS_ENABLED, (IUri**)&uri);
    AutoPtr<IContentObserver> observer = new WifiContentObserver(handler, this);
    cr->RegisterContentObserver(uri, FALSE, observer);

    obj = NULL;
    mContext->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&obj);
    AutoPtr<IPowerManager> powerManager = IPowerManager::Probe(obj);
    powerManager->NewWakeLock(IPowerManager::PARTIAL_WAKE_LOCK, TAG, (IPowerManagerWakeLock**)&mWakeLock);
    powerManager->NewWakeLock(IPowerManager::PARTIAL_WAKE_LOCK, String("WifiSuspend"), (IPowerManagerWakeLock**)&mSuspendWakeLock);
    mSuspendWakeLock->SetReferenceCounted(FALSE);

    AddState(mDefaultState);
    AddState(mInitialState, mDefaultState);
    AddState(mDriverUnloadingState, mDefaultState);
    AddState(mDriverUnloadedState, mDefaultState);
    AddState(mDriverFailedState, mDriverUnloadedState);
    AddState(mDriverLoadingState, mDefaultState);
    AddState(mDriverLoadedState, mDefaultState);
    AddState(mSupplicantStartingState, mDefaultState);
    AddState(mSupplicantStartedState, mDefaultState);
    AddState(mDriverStartingState, mSupplicantStartedState);
    AddState(mDriverStartedState, mSupplicantStartedState);
    AddState(mScanModeState, mDriverStartedState);
    AddState(mConnectModeState, mDriverStartedState);
    AddState(mL2ConnectedState, mConnectModeState);
    AddState(mObtainingIpState, mL2ConnectedState);
    AddState(mVerifyingLinkState, mL2ConnectedState);
    AddState(mCaptivePortalCheckState, mL2ConnectedState);
    AddState(mConnectedState, mL2ConnectedState);
    AddState(mDisconnectingState, mConnectModeState);
    AddState(mDisconnectedState, mConnectModeState);
    AddState(mWpsRunningState, mConnectModeState);
    AddState(mWaitForP2pDisableState, mSupplicantStartedState);
    AddState(mDriverStoppingState, mSupplicantStartedState);
    AddState(mDriverStoppedState, mSupplicantStartedState);
    AddState(mSupplicantStoppingState, mDefaultState);
    AddState(mSoftApStartingState, mDefaultState);
    AddState(mSoftApStartedState, mDefaultState);
    AddState(mTetheringState, mSoftApStartedState);
    AddState(mTetheredState, mSoftApStartedState);
    AddState(mSoftApStoppingState, mDefaultState);

    SetInitialState(mInitialState);

    SetLogRecSize(100);
    if (DBG) SetDbg(TRUE);

    //start the state machine
    Start();
}

WifiStateMachine::~WifiStateMachine()
{
    if (mReplyChannel) {
        mReplyChannel->Disconnected();
        mReplyChannel = NULL;
    }

    if (mWifiP2pChannel) {
        mWifiP2pChannel->Disconnected();
        mWifiP2pChannel = NULL;
    }

    if (mWifiApConfigChannel) {
        mWifiApConfigChannel->Disconnected();
        mWifiApConfigChannel = NULL;
    }
}

AutoPtr<IMessenger> WifiStateMachine::GetMessenger()
{
    AutoPtr<IMessenger> messenger;
    CMessenger::New(GetHandler(), (IMessenger**)&messenger);
    return messenger;
}

Boolean WifiStateMachine::SyncPingSupplicant(
    /* [in] */ AsyncChannel* channel)
{
    AutoPtr<IMessage> resultMsg = channel->SendMessageSynchronously(CMD_PING_SUPPLICANT);
    Int32 arg1;
    resultMsg->GetArg1(&arg1);
    Boolean result = (arg1 != FAILURE);
    resultMsg->Recycle();
    return result;
}

void WifiStateMachine::StartScan(
    /* [in] */ Boolean forceActive)
{
    AutoPtr<IMessage> m = ObtainMessage(CMD_START_SCAN, forceActive ?
            SCAN_ACTIVE : SCAN_PASSIVE, 0);
    SendMessage(m);
}

void WifiStateMachine::SetWifiEnabled(
    /* [in] */ Boolean enable)
{
    mLastEnableUid->Set(Binder::GetCallingUid());
    if (enable) {
        /* Argument is the state that is entered prior to load */
        AutoPtr<IMessage> m = ObtainMessage(CMD_LOAD_DRIVER, IWifiManager::WIFI_STATE_ENABLING, 0);
        SendMessage(m);
        SendMessage(CMD_START_SUPPLICANT);
    }
    else {
        SendMessage(CMD_STOP_SUPPLICANT);
        /* Argument is the state that is entered upon success */
        AutoPtr<IMessage> m = ObtainMessage(CMD_UNLOAD_DRIVER, IWifiManager::WIFI_STATE_DISABLED, 0);
        SendMessage(m);
    }
}

void WifiStateMachine::SetWifiApEnabled(
    /* [in] */ IWifiConfiguration* wifiConfig,
    /* [in] */ Boolean enable)
{
    mLastApEnableUid->Set(Binder::GetCallingUid());
    if (enable) {
        /* Argument is the state that is entered prior to load */
        AutoPtr<IMessage> m = ObtainMessage(CMD_LOAD_DRIVER, IWifiManager::WIFI_AP_STATE_ENABLING, 0);
        SendMessage(m);
        m = ObtainMessage(CMD_START_AP, wifiConfig);
        SendMessage(m);
    }
    else {
        SendMessage(CMD_STOP_AP);
        /* Argument is the state that is entered upon success */
        AutoPtr<IMessage> m = ObtainMessage(CMD_UNLOAD_DRIVER, IWifiManager::WIFI_AP_STATE_DISABLED, 0);
        SendMessage(m);
    }
}

void WifiStateMachine::SetWifiApConfiguration(
    /* [in] */ IWifiConfiguration* config)
{
    mWifiApConfigChannel->SendMessage(CMD_SET_AP_CONFIG, config);
}

AutoPtr<IWifiConfiguration> WifiStateMachine::SyncGetWifiApConfiguration()
{
    AutoPtr<IMessage> resultMsg = mWifiApConfigChannel->SendMessageSynchronously(CMD_REQUEST_AP_CONFIG);
    AutoPtr<IInterface> obj;
    resultMsg->GetObj((IInterface**)&obj);
    AutoPtr<IWifiConfiguration> ret = IWifiConfiguration::Probe(obj);
    resultMsg->Recycle();
    return ret;
}

Int32 WifiStateMachine::SyncGetWifiState()
{
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<ISettingsSecure> ss;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&ss);
    Int32 value;
    ss->GetInt32(cr, ISettingsSecure::WIFI_CHEAT_ON, 0, &value);
    if (value != 0) {
        Int32 currentWifiState;
        mWifiState->Get(&currentWifiState);
        if(currentWifiState != IWifiManager::WIFI_STATE_ENABLED ) {
            CheckAndSetConnectivityInstance();
            AutoPtr<INetworkInfo> networkinfo;
            mConnectivityManager->GetNetworkInfo(
                IConnectivityManager::TYPE_ETHERNET,
                (INetworkInfo**)&networkinfo);
            Boolean bval;
            if (networkinfo != NULL && (networkinfo->IsConnected(&bval), bval)) {
                Slogger::D("WifiStateMachine", "SyncGetWifiState: ethernet connected, but wifi disabled.");
                return IWifiManager::WIFI_STATE_ENABLED;
            }
        }
    }

    Int32 state;
    mWifiState->Get(&state);
    return state;
}

String WifiStateMachine::SyncGetWifiStateByName()
{
    Int32 state;
    mWifiState->Get(&state);
    switch (state) {
        case IWifiManager::WIFI_STATE_DISABLING:
            return String("disabling");
        case IWifiManager::WIFI_STATE_DISABLED:
            return String("disabled");
        case IWifiManager::WIFI_STATE_ENABLING:
            return String("enabling");
        case IWifiManager::WIFI_STATE_ENABLED:
            return String("enabled");
        case IWifiManager::WIFI_STATE_UNKNOWN:
            return String("unknown state");
        default:
            return String("[invalid state]");
    }
}

Int32 WifiStateMachine::SyncGetWifiApState()
{
    Int32 state;
    mWifiApState->Get(&state);
    return state;
}

String WifiStateMachine::SyncGetWifiApStateByName()
{
    Int32 state;
    mWifiApState->Get(&state);
    switch (state) {
        case IWifiManager::WIFI_AP_STATE_DISABLING:
            return String("disabling");
        case IWifiManager::WIFI_AP_STATE_DISABLED:
            return String("disabled");
        case IWifiManager::WIFI_AP_STATE_ENABLING:
            return String("enabling");
        case IWifiManager::WIFI_AP_STATE_ENABLED:
            return String("enabled");
        case IWifiManager::WIFI_AP_STATE_FAILED:
            return String("failed");
        default:
            return String("[invalid state]");
    }
}

AutoPtr<IWifiInfo> WifiStateMachine::SyncRequestConnectionInfo()
{
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<ISettingsSecure> ss;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&ss);
    Int32 value;
    ss->GetInt32(cr, ISettingsSecure::WIFI_CHEAT_ON, 0, &value);

    if (value != 0) {
        Boolean bval;
        //Cheat ethernet to wifi --Begin--
        if (mNetworkInfo != NULL && (mNetworkInfo->IsConnected(&bval), !bval)) {
            CheckAndSetConnectivityInstance();
            AutoPtr<INetworkInfo> networkinfo;
            mConnectivityManager->GetNetworkInfo(
                IConnectivityManager::TYPE_ETHERNET,
                (INetworkInfo**)&networkinfo);

            if (networkinfo != NULL && (networkinfo->IsConnected(&bval), bval)) {
                AutoPtr<IWifiInfo> notwifiInfo;
                CWifiInfo::New((IWifiInfo**)&notwifiInfo);
                AutoPtr<ILinkProperties> ethLinkpro;
                mConnectivityManager->GetLinkProperties(
                    IConnectivityManager::TYPE_ETHERNET,
                    (ILinkProperties**)&ethLinkpro);

                AutoPtr<IWifiSsidHelper> helper;
                CWifiSsidHelper::AcquireSingleton((IWifiSsidHelper**)&helper);
                AutoPtr<IWifiSsid> wifiSsid;
                helper->CreateFromAsciiEncoded(String("android_wifi"), (IWifiSsid**)&wifiSsid);

                notwifiInfo->SetSSID(wifiSsid);
                notwifiInfo->SetBSSID(String("00:11:22:33:44:55"));
                notwifiInfo->SetNetworkId(1);
                notwifiInfo->SetRssi(-45);
                notwifiInfo->SetSupplicantState(SupplicantState_COMPLETED);

                AutoPtr<IObjectContainer> bc;
                ethLinkpro->GetAddresses((IObjectContainer**)&bc);

                AutoPtr<IObjectEnumerator> enumerator;
                bc->GetObjectEnumerator((IObjectEnumerator**)&enumerator);
                Boolean hasNext = FALSE;
                while (enumerator->MoveNext(&hasNext), hasNext) {
                    AutoPtr<IInetAddress> ethaddr;
                    enumerator->Current((IInterface**)&ethaddr);
                    notwifiInfo->SetInetAddress(ethaddr);
                    break;
                }

                notwifiInfo->SetLinkSpeed(54);
                return notwifiInfo;
            }
        }
        // log("syncRequestConnectionInfo: WifiInfo = " + mWifiInfo);
        //Cheat ethernet to wifi --End--
        return mWifiInfo;
    }

    return mWifiInfo;
}

AutoPtr<IDhcpInfo> WifiStateMachine::SyncGetDhcpInfo()
{
    {
        AutoLock lock(mDhcpInfoInternalLock);
        return mDhcpInfoInternal->MakeDhcpInfo();
    }
}

void WifiStateMachine::SetDriverStart(
    /* [in] */ Boolean enable,
    /* [in] */ Boolean ecm)
{
    if (enable) {
        SendMessage(CMD_START_DRIVER);
    }
    else {
        AutoPtr<IMessage> m = ObtainMessage(CMD_STOP_DRIVER, ecm ? IN_ECM_STATE : NOT_IN_ECM_STATE, 0);
        SendMessage(m);
    }
}

void WifiStateMachine::CaptivePortalCheckComplete()
{
    if (DBG) Logger::D(TAG, "CaptivePortalCheckComplete() CMD_CAPTIVE_CHECK_COMPLETE");
    AutoPtr<IMessage> m = ObtainMessage(CMD_CAPTIVE_CHECK_COMPLETE);
    SendMessage(m);
}

void WifiStateMachine::SetScanOnlyMode(
    /* [in] */ Boolean enable)
{
    if (enable) {
        AutoPtr<IMessage> m = ObtainMessage(CMD_SET_SCAN_MODE, SCAN_ONLY_MODE, 0);
        SendMessage(m);
    }
    else {
        AutoPtr<IMessage> m = ObtainMessage(CMD_SET_SCAN_MODE, CONNECT_MODE, 0);
        SendMessage(m);
    }
}

void WifiStateMachine::SetScanType(
    /* [in] */ Boolean active)
{
    if (active) {
        AutoPtr<IMessage> m = ObtainMessage(CMD_SET_SCAN_TYPE, SCAN_ACTIVE, 0);
        SendMessage(m);
    }
    else {
        AutoPtr<IMessage> m = ObtainMessage(CMD_SET_SCAN_TYPE, SCAN_PASSIVE, 0);
        SendMessage(m);
    }
}

AutoPtr<ArrayOf<IScanResult*> > WifiStateMachine::SyncGetScanResultsList()
{
    AutoLock lock(mScanResultCacheLock);

    AutoPtr<ArrayOf<IScanResult*> > scanList = ArrayOf<IScanResult*>::Alloc(mScanResults.GetSize());
    Int32 i = 0;
    List< AutoPtr<IScanResult> >::Iterator it;
    for(it = mScanResults.Begin(); it != mScanResults.End(); ++it) {
        AutoPtr<IScanResult> result;
        CScanResult::New(*it, (IScanResult**)&result);
        scanList->Set(i++, result);
    }
    return scanList;
}

void WifiStateMachine::DisconnectCommand()
{
    SendMessage(CMD_DISCONNECT);
}

void WifiStateMachine::ReconnectCommand()
{
    SendMessage(CMD_RECONNECT);
}

void WifiStateMachine::ReassociateCommand()
{
    SendMessage(CMD_REASSOCIATE);
}

Int32 WifiStateMachine::SyncAddOrUpdateNetwork(
    /* [in] */ AsyncChannel* channel,
    /* [in] */ IWifiConfiguration* config)
{
    AutoPtr<IMessage> resultMsg = channel->SendMessageSynchronously(CMD_ADD_OR_UPDATE_NETWORK, config);
    Int32 result;
    resultMsg->GetArg1(&result);
    resultMsg->Recycle();
    return result;
}

AutoPtr<IObjectContainer> WifiStateMachine::SyncGetConfiguredNetworks(
    /* [in] */ AsyncChannel* channel)
{
    AutoPtr<IMessage> resultMsg = channel->SendMessageSynchronously(CMD_GET_CONFIGURED_NETWORKS);
    AutoPtr<IInterface> obj;
    resultMsg->GetObj((IInterface**)&obj);
    AutoPtr<IObjectContainer> result = IObjectContainer::Probe(obj);
    resultMsg->Recycle();
    return result;
}

Boolean WifiStateMachine::SyncRemoveNetwork(
    /* [in] */ AsyncChannel* channel,
    /* [in] */ Int32 networkId)
{
    AutoPtr<IMessage> resultMsg = channel->SendMessageSynchronously(CMD_REMOVE_NETWORK, networkId);
    Int32 arg1;
    resultMsg->GetArg1(&arg1);
    Boolean result = (arg1 != FAILURE);
    resultMsg->Recycle();
    return result;
}

Boolean WifiStateMachine::SyncEnableNetwork(
    /* [in] */ AsyncChannel* channel,
    /* [in] */ Int32 netId,
    /* [in] */ Boolean disableOthers)
{
    AutoPtr<IMessage> resultMsg = channel->SendMessageSynchronously(CMD_ENABLE_NETWORK, netId,
            disableOthers ? 1 : 0);
    Int32 arg1;
    resultMsg->GetArg1(&arg1);
    Boolean result = (arg1 != FAILURE);
    resultMsg->Recycle();
    return result;
}

Boolean WifiStateMachine::SyncDisableNetwork(
    /* [in] */ AsyncChannel* channel,
    /* [in] */ Int32 netId)
{
    AutoPtr<IMessage> resultMsg = channel->SendMessageSynchronously(IWifiManager::DISABLE_NETWORK, netId);
    Int32 arg1;
    resultMsg->GetArg1(&arg1);
    Boolean result = (arg1 != IWifiManager::DISABLE_NETWORK_FAILED);
    resultMsg->Recycle();
    return result;
}

void WifiStateMachine::AddToBlacklist(
    /* [in] */ const String& bssid)
{
    AutoPtr<ICharSequence> cs;
    CString::New(bssid, (ICharSequence**)&cs);
    AutoPtr<IMessage> m = ObtainMessage(CMD_BLACKLIST_NETWORK, cs.Get());
    SendMessage(m);
}

void WifiStateMachine::ClearBlacklist()
{
    AutoPtr<IMessage> m = ObtainMessage(CMD_CLEAR_BLACKLIST);
    SendMessage(m);
}

void WifiStateMachine::EnableRssiPolling(
    /* [in] */ Boolean enabled)
{
    AutoPtr<IMessage> m = ObtainMessage(CMD_ENABLE_RSSI_POLL, enabled ? 1 : 0, 0);
    SendMessage(m);
}

void WifiStateMachine::EnableBackgroundScanCommand(
    /* [in] */ Boolean enabled)
{
    AutoPtr<IMessage> m = ObtainMessage(CMD_ENABLE_BACKGROUND_SCAN, enabled ? 1 : 0, 0);
    SendMessage(m);
}

void WifiStateMachine::EnableAllNetworks()
{
    SendMessage(CMD_ENABLE_ALL_NETWORKS);
}

void WifiStateMachine::StartFilteringMulticastV4Packets()
{
    mFilteringMulticastV4Packets->Set(TRUE);
    AutoPtr<IMessage> m = ObtainMessage(CMD_START_PACKET_FILTERING, MULTICAST_V4, 0);
    SendMessage(m);
}

void WifiStateMachine::StopFilteringMulticastV4Packets()
{
    mFilteringMulticastV4Packets->Set(FALSE);
    AutoPtr<IMessage> m = ObtainMessage(CMD_STOP_PACKET_FILTERING, MULTICAST_V4, 0);
    SendMessage(m);
}

void WifiStateMachine::StartFilteringMulticastV6Packets()
{
    AutoPtr<IMessage> m = ObtainMessage(CMD_START_PACKET_FILTERING, MULTICAST_V6, 0);
    SendMessage(m);
}

void WifiStateMachine::StopFilteringMulticastV6Packets()
{
    AutoPtr<IMessage> m = ObtainMessage(CMD_STOP_PACKET_FILTERING, MULTICAST_V6, 0);
    SendMessage(m);
}

void WifiStateMachine::SetHighPerfModeEnabled(
    /* [in] */ Boolean enable)
{
    AutoPtr<IMessage> m = ObtainMessage(CMD_SET_HIGH_PERF_MODE, enable ? 1 : 0, 0);
    SendMessage(m);
}

void WifiStateMachine::SetCountryCode(
    /* [in] */ const String& countryCode,
    /* [in] */ Boolean persist)
{
    if (persist) {
        // TODO
    //     Settings.Global.putString(mContext.getContentResolver(),
    //             Settings.Global.WIFI_COUNTRY_CODE,
    //             countryCode);
    }
    AutoPtr<ICharSequence> cs;
    CString::New(countryCode, (ICharSequence**)&cs);
    AutoPtr<IMessage> m = ObtainMessage(CMD_SET_COUNTRY_CODE, cs);
    SendMessage(m);
}

void WifiStateMachine::SetFrequencyBand(
    /* [in] */ Int32 band,
    /* [in] */ Boolean persist)
{
    if (persist) {
        assert(0);
    //     Settings.Global.putInt(mContext.getContentResolver(),
    //             Settings.Global.WIFI_FREQUENCY_BAND,
    //             band);
    }
    AutoPtr<IMessage> m = ObtainMessage(CMD_SET_FREQUENCY_BAND, band, 0);
    SendMessage(m);
}

Int32 WifiStateMachine::GetFrequencyBand()
{
    Int32 band;
    mFrequencyBand->Get(&band);
    return band;
}

String WifiStateMachine::GetConfigFile()
{
    return mWifiConfigStore->GetConfigFile();
}

void WifiStateMachine::SendBluetoothAdapterStateChange(
    /* [in] */ Int32 state)
{
    AutoPtr<IMessage> m = ObtainMessage(CMD_BLUETOOTH_ADAPTER_STATE_CHANGE, state, 0);
    SendMessage(m);
}

Boolean WifiStateMachine::SyncSaveConfig(
    /* [in] */ AsyncChannel* channel)
{
    AutoPtr<IMessage> resultMsg = channel->SendMessageSynchronously(CMD_SAVE_CONFIG);
    Int32 arg1;
    resultMsg->GetArg1(&arg1);
    Boolean result = (arg1 != FAILURE);
    resultMsg->Recycle();
    return result;
}

void WifiStateMachine::UpdateBatteryWorkSource(
    /* [in] */ IWorkSource* newSource)
{
    {
        AutoLock lock(mRunningWifiUidsLock);
        // try {
        if (newSource != NULL) {
            mRunningWifiUids->Set(newSource);
        }
        if (mIsRunning) {
            if (mReportedRunning) {
                // If the work source has changed since last time, need
                // to remove old work from battery stats.
                Boolean diff;
                if (mLastRunningWifiUids->Diff(mRunningWifiUids, &diff), diff) {
                    mBatteryStats->NoteWifiRunningChanged(mLastRunningWifiUids, mRunningWifiUids);
                    mLastRunningWifiUids->Set(mRunningWifiUids);
                }
            }
            else {
                // Now being started, report it.
                mBatteryStats->NoteWifiRunning(mRunningWifiUids);
                mLastRunningWifiUids->Set(mRunningWifiUids);
                mReportedRunning = TRUE;
            }
        }
        else {
            if (mReportedRunning) {
                // Last reported we were running, time to stop.
                mBatteryStats->NoteWifiStopped(mLastRunningWifiUids);
                mLastRunningWifiUids->Clear();
                mReportedRunning = FALSE;
            }
        }
        mWakeLock->SetWorkSource(newSource);
        // } catch (RemoteException ignore) {
        // }
    }
}

String WifiStateMachine::ToString()
{
//    StringBuffer sb = new StringBuffer();
//    String LS = System.getProperty("line.separator");
//    sb.append("current HSM state: ").append(getCurrentState().getName()).append(LS);
//    sb.append("mLinkProperties ").append(mLinkProperties).append(LS);
//    sb.append("mWifiInfo ").append(mWifiInfo).append(LS);
//    sb.append("mDhcpInfoInternal ").append(mDhcpInfoInternal).append(LS);
//    sb.append("mNetworkInfo ").append(mNetworkInfo).append(LS);
//    sb.append("mLastSignalLevel ").append(mLastSignalLevel).append(LS);
//    sb.append("mLastBssid ").append(mLastBssid).append(LS);
//    sb.append("mLastNetworkId ").append(mLastNetworkId).append(LS);
//    sb.append("mReconnectCount ").append(mReconnectCount).append(LS);
//    sb.append("mIsScanMode ").append(mIsScanMode).append(LS);
//    sb.append("mUserWantsSuspendOpt ").append(mUserWantsSuspendOpt).append(LS);
//    sb.append("mSuspendOptNeedsDisabled ").append(mSuspendOptNeedsDisabled).append(LS);
//    sb.append("Supplicant status").append(LS)
//            .append(mWifiNative.status()).append(LS).append(LS);
//
//    sb.append(mWifiConfigStore.dump());
//    return sb.toString();
    assert(0);
    return String(NULL);
}

Boolean WifiStateMachine::RecordLogRec(
    /* [in] */ IMessage* msg)
{
    //Ignore screen on/off & common messages when driver has started
    if (GetCurrentState().Get() == mConnectedState.Get() || GetCurrentState().Get() == mDisconnectedState.Get()) {
        Int32 what;
        msg->GetWhat(&what);
        switch (what) {
            case CMD_LOAD_DRIVER:
            case CMD_START_SUPPLICANT:
            case CMD_START_DRIVER:
            case CMD_SET_SCAN_MODE:
            case CMD_SET_HIGH_PERF_MODE:
            case CMD_SET_SUSPEND_OPT_ENABLED:
            case CMD_ENABLE_BACKGROUND_SCAN:
            case CMD_ENABLE_ALL_NETWORKS:
            return FALSE;
        }
    }

    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case CMD_START_SCAN:
        case CMD_ENABLE_RSSI_POLL:
        case CMD_RSSI_POLL:
        case CMD_DELAYED_STOP_DRIVER:
        case WifiMonitor::SCAN_RESULTS_EVENT:
        case IWifiManager::RSSI_PKTCNT_FETCH:
            return FALSE;
        default:
            return TRUE;
    }
}

void WifiStateMachine::HandleScreenStateChanged(
    /* [in] */ Boolean screenOn)
{
    // if (DBG) Log("handleScreenStateChanged: " + screenOn);
    EnableRssiPolling(screenOn);
    if (mBackgroundScanSupported) {
        EnableBackgroundScanCommand(screenOn == FALSE);
    }

    if (screenOn) EnableAllNetworks();
    Boolean opt;
    if (mUserWantsSuspendOpt->Get(&opt), opt) {
        if (screenOn) {
            AutoPtr<IMessage> m = ObtainMessage(CMD_SET_SUSPEND_OPT_ENABLED, 0, 0);
            SendMessage(m);
        }
        else {
            //Allow 2s for suspend optimizations to be set
            mSuspendWakeLock->AcquireLock(2000);
            AutoPtr<IMessage> m = ObtainMessage(CMD_SET_SUSPEND_OPT_ENABLED, 1, 0);
            SendMessage(m);
        }
    }
    mScreenBroadcastReceived->Set(TRUE);
}

void WifiStateMachine::CheckAndSetConnectivityInstance()
{
    if (mConnectivityManager == NULL) {
        AutoPtr<IInterface> obj;
        mContext->GetSystemService(IContext::CONNECTIVITY_SERVICE, (IInterface**)&obj);
        mConnectivityManager = IConnectivityManager::Probe(obj);
    }
}

Boolean WifiStateMachine::StartTethering(
    /* [in] */ ArrayOf<String>* available)
{
    Boolean wifiAvailable = FALSE;

    CheckAndSetConnectivityInstance();

    AutoPtr< ArrayOf<String> > wifiRegexs;
    mConnectivityManager->GetTetherableWifiRegexs((ArrayOf<String>**)&wifiRegexs);

    for (Int32 i = 0; i < available->GetLength(); ++i) {
        String intf = (*available)[i];
        for (Int32 j = 0; j < wifiRegexs->GetLength(); ++j) {
            String regex = (*wifiRegexs)[j];
            Boolean matched;
            if (StringUtils::Matches(intf, String("wlan0"), &matched), matched) {
                AutoPtr<IInterfaceConfiguration> ifcg;
                // try {
                if (mNwService) {
                    mNwService->GetInterfaceConfig(intf, (IInterfaceConfiguration**)&ifcg);

                    if (ifcg != NULL) {
                        /* IP/netmask: 192.168.43.1/255.255.255.0 */
                        AutoPtr<IInetAddress> ia;
                        NetworkUtils::NumericToInetAddress(String("192.168.43.1"), (IInetAddress**)&ia);
                        AutoPtr<ILinkAddress> la;
                        CLinkAddress::New(ia, 24, (ILinkAddress**)&la);
                        ifcg->SetLinkAddress(la);
                        ifcg->SetInterfaceUp();
                        mNwService->SetInterfaceConfig(intf, ifcg);
                    }
                }
                // } catch (Exception e) {
                //     loge("Error configuring interface " + intf + ", :" + e);
                //     return FALSE;
                // }

                Int32 result;
                if(mConnectivityManager->Tether(intf, &result), result != IConnectivityManager::TETHER_ERROR_NO_ERROR) {
                    // loge("Error tethering on " + intf);
                    return FALSE;
                }
                mTetherInterfaceName = intf;
                return TRUE;
            }
        }
    }
    // We found no interfaces to tether
    return FALSE;
}

void WifiStateMachine::StopTethering()
{
    CheckAndSetConnectivityInstance();

    /* Clear the interface config to allow dhcp correctly configure new
       ip settings */
    AutoPtr<IInterfaceConfiguration> ifcg;
    // try {
    if (mNwService) {
        mNwService->GetInterfaceConfig(mTetherInterfaceName, (IInterfaceConfiguration**)&ifcg);
        if (ifcg != NULL) {
            assert(0);
            // ifcg->SetLinkAddress(
            //         new LinkAddress(NetworkUtils.numericToInetAddress("0.0.0.0"), 0));
            mNwService->SetInterfaceConfig(mTetherInterfaceName, ifcg);
        }
    }
    // } catch (Exception e) {
    //     loge("Error resetting interface " + mTetherInterfaceName + ", :" + e);
    // }

    Int32 result;
    if (mConnectivityManager->Untether(mTetherInterfaceName, &result), result != IConnectivityManager::TETHER_ERROR_NO_ERROR) {
        Loge(String("Untether initiate failed!"));
    }
}

Boolean WifiStateMachine::IsWifiTethered(
    /* [in] */ ArrayOf<String>* active)
{
    CheckAndSetConnectivityInstance();

    AutoPtr< ArrayOf<String> > wifiRegexs;
    mConnectivityManager->GetTetherableWifiRegexs((ArrayOf<String>**)&wifiRegexs);

    for (Int32 i = 0; i < active->GetLength(); ++i) {
        String intf = (*active)[i];
        for (Int32 j = 0; j < wifiRegexs->GetLength(); ++j) {
            String regex = (*wifiRegexs)[j];
            Boolean matched;
            if (StringUtils::Matches(intf, regex, &matched), matched) {
                return TRUE;
            }
        }
    }
    // We found no interfaces that are tethered
    return FALSE;
}

/**
 * Set the country code from the system setting value, if any.
 */
void WifiStateMachine::SetCountryCode()
{
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);

    String countryCode;
    // TODO
    // Settings::Global::GetString(resolver,
    //         String("wifi_country_code") /*Settings.Global.WIFI_COUNTRY_CODE)*/, &countryCode);
    if (!countryCode.IsNullOrEmpty()) {
        SetCountryCode(countryCode, FALSE);
    }
    else {
        //use driver default
    }
}

/**
 * Set the frequency band from the system setting value, if any.
 */
void WifiStateMachine::SetFrequencyBand()
{
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);

    Int32 band = 0;
    // TODO
    // Settings::Global::GetInt32(resolver,
    //         String("wifi_frequency_band") /*Settings::Global::WIFI_FREQUENCY_BAND*/,
    //         IWifiManager::WIFI_FREQUENCY_BAND_AUTO, &band);
    SetFrequencyBand(band, FALSE);
}

void WifiStateMachine::SetSuspendOptimizationsNative(
    /* [in] */ Int32 reason,
    /* [in] */ Boolean enabled)
{
    // if (DBG) log("setSuspendOptimizationsNative: " + reason + " " + enabled);
    if (enabled) {
        mSuspendOptNeedsDisabled &= ~reason;
        /* None of dhcp, screen or highperf need it disabled and user wants it enabled */
        Boolean opt;
        if (mSuspendOptNeedsDisabled == 0 && (mUserWantsSuspendOpt->Get(&opt), opt)) {
            mWifiNative->SetSuspendOptimizations(TRUE);
        }
    }
    else {
        mSuspendOptNeedsDisabled |= reason;
        mWifiNative->SetSuspendOptimizations(FALSE);
    }
}

void WifiStateMachine::SetSuspendOptimizations(
    /* [in] */ Int32 reason,
    /* [in] */ Boolean enabled)
{
    // if (DBG) log("setSuspendOptimizations: " + reason + " " + enabled);
    if (enabled) {
        mSuspendOptNeedsDisabled &= ~reason;
    }
    else {
        mSuspendOptNeedsDisabled |= reason;
    }
    // if (DBG) log("mSuspendOptNeedsDisabled " + mSuspendOptNeedsDisabled);
}

void WifiStateMachine::SetWifiState(
    /* [in] */ Int32 wifiState)
{
    Int32 previousWifiState;
    mWifiState->Get(&previousWifiState);

    // try {
    if (wifiState == IWifiManager::WIFI_STATE_ENABLED) {
        mBatteryStats->NoteWifiOn();
    }
    else if (wifiState == IWifiManager::WIFI_STATE_DISABLED) {
        mBatteryStats->NoteWifiOff();
    }
    // } catch (RemoteException e) {
    //     loge("Failed to note battery stats in wifi");
    // }

    mWifiState->Set(wifiState);

    if (DBG) {
        Slogger::D(TAG, "setWifiState: %s", SyncGetWifiStateByName().string());
    }

    AutoPtr<IIntent> intent;
    CIntent::New(IWifiManager::WIFI_STATE_CHANGED_ACTION, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
    intent->PutExtra(IWifiManager::EXTRA_WIFI_STATE, wifiState);
    intent->PutExtra(IWifiManager::EXTRA_PREVIOUS_WIFI_STATE, previousWifiState);
    mContext->SendStickyBroadcastAsUser(intent, UserHandle::ALL);
}

void WifiStateMachine::SetWifiApState(
    /* [in] */ Int32 wifiApState)
{
    Int32 previousWifiApState;
    mWifiApState->Get(&previousWifiApState);

    // try {
    if (wifiApState == IWifiManager::WIFI_AP_STATE_ENABLED) {
        mBatteryStats->NoteWifiOn();
    }
    else if (wifiApState == IWifiManager::WIFI_AP_STATE_DISABLED) {
        mBatteryStats->NoteWifiOff();
    }
    // } catch (RemoteException e) {
    //     loge("Failed to note battery stats in wifi");
    // }

    // Update state
    mWifiApState->Set(wifiApState);

    if (DBG) {
        Slogger::D(TAG, "setWifiApState: %s", SyncGetWifiStateByName().string());
    }

    AutoPtr<IIntent> intent;
    CIntent::New(IWifiManager::WIFI_AP_STATE_CHANGED_ACTION, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
    intent->PutExtra(IWifiManager::EXTRA_WIFI_AP_STATE, wifiApState);
    intent->PutExtra(IWifiManager::EXTRA_PREVIOUS_WIFI_AP_STATE, previousWifiApState);
    mContext->SendStickyBroadcastAsUser(intent, UserHandle::ALL);
}

void WifiStateMachine::SetScanResults(
    /* [in] */ const String& scanResults)
{
    String bssid("");
    Int32 level = 0;
    Int32 freq = 0;
    Int64 tsf = 0;
    String flags("");
    AutoPtr<IWifiSsid> wifiSsid;

    if (scanResults.IsNull()) {
        return;
    }

    {
        AutoLock lock(mScanResultCacheLock);

        mScanResults.Clear();
        AutoPtr< ArrayOf<String> > lines;
        StringUtils::Split(scanResults, String("\n"), (ArrayOf<String>**)&lines);
        assert(lines != NULL);
        AutoPtr<IWifiSsidHelper> helper;
        CWifiSsidHelper::AcquireSingleton((IWifiSsidHelper**)&helper);
        for (Int32 i = 0; i < lines->GetLength(); ++i) {
            String line = (*lines)[i];
            if (line.StartWith(BSSID_STR)) {
                bssid = line.Substring(BSSID_STR.GetLength());
            }
            else if (line.StartWith(FREQ_STR)) {
    //             try {
                freq = StringUtils::ParseInt32(line.Substring(FREQ_STR.GetLength()));
    //             } catch (NumberFormatException e) {
    //                 freq = 0;
    //             }
            }
            else if (line.StartWith(LEVEL_STR)) {
    //             try {
                level = StringUtils::ParseInt32(line.Substring(LEVEL_STR.GetLength()));
                /* some implementations avoid negative values by adding 256
                 * so we need to adjust for that here.
                 */
                if (level > 0) level -= 256;
    //             } catch(NumberFormatException e) {
    //                 level = 0;
    //             }
            }
            else if (line.StartWith(TSF_STR)) {
    //             try {
                tsf = StringUtils::ParseInt64(line.Substring(TSF_STR.GetLength()));
    //             } catch (NumberFormatException e) {
    //                 tsf = 0;
    //             }
            }
            else if (line.StartWith(FLAGS_STR)) {
                flags = line.Substring(FLAGS_STR.GetLength());
            }
            else if (line.StartWith(SSID_STR)) {
                wifiSsid = NULL;
                helper->CreateFromAsciiEncoded(
                        line.Substring(SSID_STR.GetLength()), (IWifiSsid**)&wifiSsid);
            }
            else if (line.StartWith(DELIMITER_STR)) {
                if (!bssid.IsNull()) {
                    String ssid = IWifiSsid::NONE;
                    if (wifiSsid != NULL) {
                        wifiSsid->ToString(&ssid);
                    }
                    String key = bssid + ssid;

                    AutoPtr<IScanResult> scanResult;
                    HashMap<String, AutoPtr<IScanResult> >::Iterator it = mScanResultCache.Find(key);
                    if (it != mScanResultCache.End()) {
                        scanResult = it->mSecond;
                        scanResult->SetLevel(level);
                        scanResult->SetWifiSsid(wifiSsid);
                        // Keep existing API
                        scanResult->SetSSID(ssid);
                        scanResult->SetCapabilities(flags);
                        scanResult->SetFrequency(freq);
                        scanResult->SetTimestamp(tsf);
                    }
                    else {
                        CScanResult::New(wifiSsid, bssid, flags, level,
                            freq, tsf, (IScanResult**)&scanResult);
                        mScanResultCache[key] = scanResult;
                    }
                    mScanResults.PushBack(scanResult);
                }
                bssid = NULL;
                level = 0;
                freq = 0;
                tsf = 0;
                flags = "";
                wifiSsid = NULL;
            }
        }
    }
}

/*
 * Fetch RSSI and linkspeed on current connection
 */
void WifiStateMachine::FetchRssiAndLinkSpeedNative()
{
    Int32 newRssi = -1;
    Int32 newLinkSpeed = -1;

    String signalPoll = mWifiNative->SignalPoll();

    if (!signalPoll.IsNull()) {
        AutoPtr< ArrayOf<String> > lines;
        StringUtils::Split(signalPoll, String("\n"), (ArrayOf<String>**)&lines);
        assert(lines != NULL);
        for (Int32 i = 0; i < lines->GetLength(); ++i) {
            String line = (*lines)[i];
            AutoPtr< ArrayOf<String> > prop;
            StringUtils::Split(line, String("="), (ArrayOf<String>**)&prop);
            if (prop->GetLength() < 2) continue;
        //     try {
            if ((*prop)[0].Equals("RSSI")) {
                newRssi = StringUtils::ParseInt32((*prop)[1]);
            }
            else if ((*prop)[0].Equals("LINKSPEED")) {
                newLinkSpeed = StringUtils::ParseInt32((*prop)[1]);
            }
        //     } catch (NumberFormatException e) {
        //         //Ignore, defaults on rssi and linkspeed are assigned
        //     }
        }
    }

    if (newRssi != -1 && MIN_RSSI < newRssi && newRssi < MAX_RSSI) { // screen out invalid values
        /* some implementations avoid negative values by adding 256
         * so we need to adjust for that here.
         */
        if (newRssi > 0) newRssi -= 256;
        mWifiInfo->SetRssi(newRssi);
        /*
         * Rather then sending the raw RSSI out every time it
         * changes, we precalculate the signal level that would
         * be displayed in the status bar, and only send the
         * broadcast if that much more coarse-grained number
         * changes. This cuts down greatly on the number of
         * broadcasts, at the cost of not informing others
         * interested in RSSI of all the changes in signal
         * level.
         */
        AutoPtr<IWifiManagerHelper> helper;
        CWifiManagerHelper::AcquireSingleton((IWifiManagerHelper**)&helper);
        Int32 newSignalLevel;
        helper->CalculateSignalLevel(newRssi, IWifiManager::RSSI_LEVELS, &newSignalLevel);
        if (newSignalLevel != mLastSignalLevel) {
            SendRssiChangeBroadcast(newRssi);
        }
        mLastSignalLevel = newSignalLevel;
    }
    else {
        mWifiInfo->SetRssi(MIN_RSSI);
    }

    if (newLinkSpeed != -1) {
        mWifiInfo->SetLinkSpeed(newLinkSpeed);
    }
}

void WifiStateMachine::FetchPktcntNative(
    /* [in] */ IRssiPacketCountInfo* info)
{
    String pktcntPoll = mWifiNative->PktcntPoll();

    if (!pktcntPoll.IsNull()) {
        AutoPtr< ArrayOf<String> > lines;
        StringUtils::Split(pktcntPoll, String("\n"), (ArrayOf<String>**)&lines);
        assert(lines != NULL);
        for (Int32 i = 0; i < lines->GetLength(); ++i) {
            String line = (*lines)[i];
            AutoPtr< ArrayOf<String> > prop;
            StringUtils::Split(line, String("="), (ArrayOf<String>**)&prop);
            if (prop->GetLength() < 2) continue;
        //     try {
            if ((*prop)[0].Equals("TXGOOD")) {
                info->SetTxgood(StringUtils::ParseInt32((*prop)[1]));
            }
            else if ((*prop)[0].Equals("TXBAD")) {
                info->SetTxbad(StringUtils::ParseInt32((*prop)[1]));
            }
        //     } catch (NumberFormatException e) {
        //         //Ignore
        //     }
        }
    }
}

void WifiStateMachine::ConfigureLinkProperties()
{
    if (mWifiConfigStore->IsUsingStaticIp(mLastNetworkId)) {
        mLinkProperties = mWifiConfigStore->GetLinkProperties(mLastNetworkId);
    }
    else {
        {
            AutoLock lock(mDhcpInfoInternalLock);

            mLinkProperties = mDhcpInfoInternal->MakeLinkProperties();
        }
        AutoPtr<IProxyProperties> prop = mWifiConfigStore->GetProxyProperties(mLastNetworkId);
        mLinkProperties->SetHttpProxy(prop);
    }
    mLinkProperties->SetInterfaceName(mInterfaceName);
    // if (DBG) {
    //     log("netId=" + mLastNetworkId  + " Link configured: " +
    //             mLinkProperties.toString());
    // }
}

Int32 WifiStateMachine::GetMaxDhcpRetries()
{
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);

    PFL_EX("TODO")
    Int32 value = DEFAULT_MAX_DHCP_RETRIES;
    // Settings::Global::GetInt32(resolver,
    //           String("wifi_max_dhcp_retry_count") /*Settings::Global::WIFI_MAX_DHCP_RETRY_COUNT*/,
    //           DEFAULT_MAX_DHCP_RETRIES, &value);
    return value;
}

void WifiStateMachine::SendScanResultsAvailableBroadcast()
{
    AutoPtr<IIntent> intent;
    CIntent::New(IWifiManager::SCAN_RESULTS_AVAILABLE_ACTION, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
    mContext->SendBroadcastAsUser(intent, UserHandle::ALL);
}

void WifiStateMachine::SendRssiChangeBroadcast(
    /* [in] */ Int32 newRssi)
{
    AutoPtr<IIntent> intent;
    CIntent::New(IWifiManager::RSSI_CHANGED_ACTION, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
    intent->PutExtra(IWifiManager::EXTRA_NEW_RSSI, newRssi);
    mContext->SendStickyBroadcastAsUser(intent, UserHandle::ALL);
}

void WifiStateMachine::SendNetworkStateChangeBroadcast(
    /* [in] */ const String& bssid)
{
    AutoPtr<INetworkInfo> networkInfo;
    CNetworkInfo::New(mNetworkInfo, (INetworkInfo**)&networkInfo);
    AutoPtr<ILinkProperties> linkProps;
    CLinkProperties::New(mLinkProperties, (ILinkProperties**)&linkProps);
    AutoPtr<IWifiInfo> wifiInfo;
    CWifiInfo::New(mWifiInfo, (IWifiInfo**)&wifiInfo);

    AutoPtr<IIntent> intent;
    CIntent::New(IWifiManager::NETWORK_STATE_CHANGED_ACTION, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
    intent->PutExtra(IWifiManager::EXTRA_NETWORK_INFO, IParcelable::Probe(networkInfo.Get()));
    intent->PutExtra(IWifiManager::EXTRA_LINK_PROPERTIES, IParcelable::Probe(linkProps.Get()));
    if (!bssid.IsNull())
        intent->PutExtra(IWifiManager::EXTRA_BSSID, bssid);

    NetworkInfoDetailedState state;
    mNetworkInfo->GetDetailedState(&state);
    if (state == NetworkInfoDetailedState_VERIFYING_POOR_LINK
        || state == NetworkInfoDetailedState_CONNECTED) {
        intent->PutExtra(IWifiManager::EXTRA_WIFI_INFO, IParcelable::Probe(wifiInfo.Get()));
    }

    Boolean isWifiConnected;
    networkInfo->IsConnected(&isWifiConnected);
    mContext->SendStickyBroadcastAsUser(intent, UserHandle::ALL);
}

void WifiStateMachine::SendLinkConfigurationChangedBroadcast()
{
    AutoPtr<ILinkProperties> linkProps;
    CLinkProperties::New(mLinkProperties, (ILinkProperties**)&linkProps);

    AutoPtr<IIntent> intent;
    CIntent::New(IWifiManager::LINK_CONFIGURATION_CHANGED_ACTION, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
    intent->PutExtra(IWifiManager::EXTRA_LINK_PROPERTIES, IParcelable::Probe(linkProps.Get()));
    mContext->SendStickyBroadcastAsUser(intent, UserHandle::ALL);
}

void WifiStateMachine::SendSupplicantConnectionChangedBroadcast(
    /* [in] */ Boolean connected)
{
    AutoPtr<IIntent> intent;
    CIntent::New(IWifiManager::SUPPLICANT_CONNECTION_CHANGE_ACTION, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
    intent->PutBooleanExtra(IWifiManager::EXTRA_SUPPLICANT_CONNECTED, connected);
    mContext->SendBroadcastAsUser(intent, UserHandle::ALL);
}

void WifiStateMachine::SetNetworkDetailedState(
    /* [in] */ NetworkInfoDetailedState state)
{
    if (DBG) {
        NetworkInfoDetailedState oldState;
        mNetworkInfo->GetDetailedState(&oldState);
        Slogger::D(TAG, " setDetailed state, old = %d and new state= %d", (Int32)oldState, (Int32)state);
    }
    NetworkInfoDetailedState origState;
    mNetworkInfo->GetDetailedState(&origState);
    if (state != origState) {
        String ssid;
        mWifiInfo->GetSSID(&ssid);
        mNetworkInfo->SetDetailedState(state, String(NULL), ssid);
    }
}

NetworkInfoDetailedState WifiStateMachine::GetNetworkDetailedState()
{
    NetworkInfoDetailedState state;
    mNetworkInfo->GetDetailedState(&state);
    return state;
}

SupplicantState WifiStateMachine::HandleSupplicantStateChange(
    /* [in] */ IMessage* message)
{
    AutoPtr<IInterface> obj;
    message->GetObj((IInterface**)&obj);
    IStateChangeResult* stateChangeResult = IStateChangeResult::Probe(obj.Get());
    SupplicantState state;
    stateChangeResult->GetSupplicantState(&state);
    // Supplicant state change
    // [31-13] Reserved for future use
    // [8 - 0] Supplicant state (as defined in SupplicantState.java)
    // 50023 supplicant_state_changed (custom|1|5)
    // EventLog.writeEvent(EVENTLOG_SUPPLICANT_STATE_CHANGED, state.ordinal());
    mWifiInfo->SetSupplicantState(state);
    // Network id is only valid when we start connecting
    if (SupplicantStateHelper::IsConnecting(state)) {
        Int32 networkId;
        stateChangeResult->GetNetworkId(&networkId);
        mWifiInfo->SetNetworkId(networkId);
    }
    else {
        mWifiInfo->SetNetworkId(IWifiConfiguration::INVALID_NETWORK_ID);
    }

    String bssid;
    stateChangeResult->GetBSSID(&bssid);
    mWifiInfo->SetBSSID(bssid);
    AutoPtr<IWifiSsid> wifiSsid;
    stateChangeResult->GetWifiSsid((IWifiSsid**)&wifiSsid);
    mWifiInfo->SetSSID(wifiSsid);

    AutoPtr<IMessageHelper> helper;
    CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
    AutoPtr<IMessage> m;
    helper->Obtain(message, (IMessage**)&m);
    mSupplicantStateTracker->SendMessage(m);

    return state;
}

/**
 * Resets the Wi-Fi Connections by clearing any state, resetting any sockets
 * using the interface, stopping DHCP & disabling interface
 */
void WifiStateMachine::HandleNetworkDisconnect()
{
    if (DBG) Log(String("Stopping DHCP and clearing IP"));

    /*
     * stop DHCP
     */
    if (mDhcpStateMachine != NULL) {
        /* In case we were in middle of DHCP operation
           restore back powermode */
        HandlePostDhcpSetup();
        mDhcpStateMachine->SendMessage(DhcpStateMachine::CMD_STOP_DHCP);
    }

    // try {
    if (mNwService) {
        mNwService->ClearInterfaceAddresses(mInterfaceName);
        mNwService->DisableIpv6(mInterfaceName);
    }
    // } catch (Exception e) {
    //     loge("Failed to clear addresses or disable ipv6" + e);
    // }

    /* Reset data structures */
    mWifiInfo->SetInetAddress(NULL);
    mWifiInfo->SetBSSID(String(""));
    mWifiInfo->SetSSID(NULL);
    mWifiInfo->SetNetworkId(IWifiConfiguration::INVALID_NETWORK_ID);
    mWifiInfo->SetRssi(MIN_RSSI);
    mWifiInfo->SetLinkSpeed(-1);
    mWifiInfo->SetMeteredHint(FALSE);

    SetNetworkDetailedState(NetworkInfoDetailedState_DISCONNECTED);
    mWifiConfigStore->UpdateStatus(mLastNetworkId, NetworkInfoDetailedState_DISCONNECTED);

    /* send event to CM & network change broadcast */
    SendNetworkStateChangeBroadcast(mLastBssid);

    /* Clear network properties */
    mLinkProperties->Clear();
    /* Clear IP settings if the network used DHCP */
    if (!mWifiConfigStore->IsUsingStaticIp(mLastNetworkId)) {
        mWifiConfigStore->ClearIpConfiguration(mLastNetworkId);
    }

    mLastBssid= NULL;
    mLastNetworkId = IWifiConfiguration::INVALID_NETWORK_ID;
}

void WifiStateMachine::HandlePreDhcpSetup()
{
    if (!mBluetoothConnectionActive) {
        /*
         * There are problems setting the Wi-Fi driver's power
         * mode to active when bluetooth coexistence mode is
         * enabled or sense.
         * <p>
         * We set Wi-Fi to active mode when
         * obtaining an IP address because we've found
         * compatibility issues with some routers with low power
         * mode.
         * <p>
         * In order for this active power mode to properly be set,
         * we disable coexistence mode until we're done with
         * obtaining an IP address.  One exception is if we
         * are currently connected to a headset, since disabling
         * coexistence would interrupt that connection.
         */
        // Disable the coexistence mode
        mWifiNative->SetBluetoothCoexistenceMode(
                WifiNative::BLUETOOTH_COEXISTENCE_MODE_DISABLED);
    }

    /* Disable power save and suspend optimizations during DHCP */
    // Note: The order here is important for now. Brcm driver changes
    // power settings when we control suspend mode optimizations.
    // TODO: Remove this comment when the driver is fixed.
    SetSuspendOptimizationsNative(SUSPEND_DUE_TO_DHCP, FALSE);
    mWifiNative->SetPowerSave(FALSE);
}


void WifiStateMachine::HandlePostDhcpSetup()
{
    /* Restore power save and suspend optimizations */
    SetSuspendOptimizationsNative(SUSPEND_DUE_TO_DHCP, TRUE);
    mWifiNative->SetPowerSave(TRUE);

    // Set the coexistence mode back to its default value
    mWifiNative->SetBluetoothCoexistenceMode(
            WifiNative::BLUETOOTH_COEXISTENCE_MODE_SENSE);
}

void WifiStateMachine::HandleSuccessfulIpConfiguration(
    /* [in] */ DhcpInfoInternal* dhcpInfoInternal)
{
    {
        AutoLock lock(mDhcpInfoInternalLock);
        mDhcpInfoInternal = dhcpInfoInternal;
    }
    mLastSignalLevel = -1; // force update of signal strength
    mReconnectCount = 0; //Reset IP failure tracking
    mWifiConfigStore->SetIpConfiguration(mLastNetworkId, dhcpInfoInternal);
    AutoPtr<IInetAddress> addr;
    NetworkUtils::NumericToInetAddress(dhcpInfoInternal->mIpAddress, (IInetAddress**)&addr);
    mWifiInfo->SetInetAddress(addr);
    mWifiInfo->SetMeteredHint(dhcpInfoInternal->HasMeteredHint());
    if (GetNetworkDetailedState() == NetworkInfoDetailedState_CONNECTED) {
        if (DBG) Logger::D("WifiStateMachine::HandleSuccessfulIpConfiguration",
            "===========CONNECTED===========");
        //DHCP renewal in connected state
        AutoPtr<ILinkProperties> linkProperties = dhcpInfoInternal->MakeLinkProperties();
        linkProperties->SetHttpProxy(mWifiConfigStore->GetProxyProperties(mLastNetworkId));
        linkProperties->SetInterfaceName(mInterfaceName);
        Boolean isEqual;
        if (linkProperties->Equals(mLinkProperties, &isEqual), !isEqual) {
            if (DBG) {
                // log("Link configuration changed for netId: " + mLastNetworkId
                //         + " old: " + mLinkProperties + "new: " + linkProperties);
            }
            mLinkProperties = linkProperties;
            SendLinkConfigurationChangedBroadcast();
        }
    }
    else {
        ConfigureLinkProperties();
    }
}

void WifiStateMachine::HandleFailedIpConfiguration()
{
    Loge(String("IP configuration failed"));

    mWifiInfo->SetInetAddress(NULL);
    mWifiInfo->SetMeteredHint(FALSE);
    /**
     * If we've exceeded the maximum number of retries for DHCP
     * to a given network, disable the network
     */
    Int32 maxRetries = GetMaxDhcpRetries();
    // maxRetries == 0 means keep trying forever
    if (maxRetries > 0 && ++mReconnectCount > maxRetries) {
        // loge("Failed " +
        //         mReconnectCount + " times, Disabling " + mLastNetworkId);
        mWifiConfigStore->DisableNetwork(mLastNetworkId,
                IWifiConfiguration::DISABLED_DHCP_FAILURE);
        mReconnectCount = 0;
    }

    /* DHCP times out after about 30 seconds, we do a
     * disconnect and an immediate reconnect to try again
     */
    mWifiNative->Disconnect();
    mWifiNative->Reconnect();
}

void WifiStateMachine::StartSoftApWithConfig(
    /* [in] */ IWifiConfiguration* config)
{
    // start hostapd on a seperate thread
    AutoPtr<IRunnable> r = new Runnable(config, this);
    AutoPtr<IThread> t;
    CThread::New(r, (IThread**)&t);
    t->Start();
}

void WifiStateMachine::ReplyToMessage(
    /* [in] */ IMessage* msg,
    /* [in] */ Int32 what)
{
    AutoPtr<IMessenger> replyTo;
    msg->GetReplyTo((IMessenger**)&replyTo);
    if (replyTo == NULL) return;
    AutoPtr<IMessage> dstMsg = ObtainMessageWithArg2(msg);
    dstMsg->SetWhat(what);
    mReplyChannel->ReplyToMessage(msg, dstMsg);
}

void WifiStateMachine::ReplyToMessage(
    /* [in] */ IMessage* msg,
    /* [in] */ Int32 what,
    /* [in] */ Int32 arg1)
{
    AutoPtr<IMessenger> replyTo;
    msg->GetReplyTo((IMessenger**)&replyTo);
    if (replyTo == NULL) return;
    AutoPtr<IMessage> dstMsg = ObtainMessageWithArg2(msg);
    dstMsg->SetWhat(what);
    dstMsg->SetArg1(arg1);
    mReplyChannel->ReplyToMessage(msg, dstMsg);
}

void WifiStateMachine::ReplyToMessage(
    /* [in] */ IMessage* msg,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IMessenger> replyTo;
    msg->GetReplyTo((IMessenger**)&replyTo);
    if (replyTo == NULL) return;
    AutoPtr<IMessage> dstMsg = ObtainMessageWithArg2(msg);
    dstMsg->SetWhat(what);
    dstMsg->SetObj(obj);
    mReplyChannel->ReplyToMessage(msg, dstMsg);
}

AutoPtr<IMessage> WifiStateMachine::ObtainMessageWithArg2(
    /* [in] */ IMessage* srcMsg)
{
    AutoPtr<IMessageHelper> helper;
    CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
    AutoPtr<IMessage> msg;
    helper->Obtain((IMessage**)&msg);
    Int32 arg2;
    srcMsg->GetArg2(&arg2);
    msg->SetArg2(arg2);
    return msg;
}

void WifiStateMachine::Log(
/* [in] */ const String& s)
{
    Logger::D(TAG, s.string());
}

void WifiStateMachine::Loge(
/* [in] */ const String& s)
{
    Logger::E(TAG, s.string());
}

} // namespace Wifi
} // namespace Droid
} // namespace Elastos
