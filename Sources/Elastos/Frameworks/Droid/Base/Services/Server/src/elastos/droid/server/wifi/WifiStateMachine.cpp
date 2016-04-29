#include "elastos/droid/server/wifi/WifiStateMachine.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/provider/Settings.h"
#include "elastos/droid/R.h"
#include "elastos/droid/server/wifi/CStateChangeResult.h"
#include "elastos/droid/server/wifi/NetworkUpdateResult.h"
#include "elastos/droid/server/wifi/WifiApConfigStore.h"
#include "elastos/droid/server/wifi/WifiWatchdogStateMachine.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuffer.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::Backup::IIBackupManager;
using Elastos::Droid::App::CPendingIntentHelper;
using Elastos::Droid::App::IPendingIntentHelper;
using Elastos::Droid::Bluetooth::IBluetoothAdapter;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Internal::Utility::CAsyncChannel;
using Elastos::Droid::Net::CDhcpResults;
using Elastos::Droid::Net::CDhcpStateMachineHelper;
using Elastos::Droid::Net::CInterfaceConfiguration;
using Elastos::Droid::Net::CLinkAddress;
using Elastos::Droid::Net::CLinkProperties;
using Elastos::Droid::Net::CNetworkCapabilities;
using Elastos::Droid::Net::CNetworkInfo;
using Elastos::Droid::Net::CNetworkUtils;
using Elastos::Droid::Net::CTrafficStats;
using Elastos::Droid::Net::IDhcpStateMachineHelper;
using Elastos::Droid::Net::IINetworkManagementEventObserver;
using Elastos::Droid::Net::IInterfaceConfiguration;
using Elastos::Droid::Net::ILinkAddress;
using Elastos::Droid::Net::INetworkAgent;
using Elastos::Droid::Net::INetworkUtils;
using Elastos::Droid::Net::IRouteInfo;
using Elastos::Droid::Net::IStaticIpConfiguration;
using Elastos::Droid::Net::ITrafficStats;
using Elastos::Droid::Net::NetworkInfoState;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::CMessage;
using Elastos::Droid::Os::CMessageHelper;
using Elastos::Droid::Os::CMessenger;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::CWorkSource;
using Elastos::Droid::Os::IBatteryStats;
using Elastos::Droid::Os::IMessageHelper;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::Settings;
using Elastos::Droid::R;
using Elastos::Droid::Server::Net::EIID_INetlinkTrackerCallback;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Wifi::CBatchedScanResult;
using Elastos::Droid::Wifi::CScanResult;
using Elastos::Droid::Wifi::CScanResultHelper;
using Elastos::Droid::Wifi::CSupplicantState;
using Elastos::Droid::Wifi::CSupplicantStateHelper;
using Elastos::Droid::Wifi::CWifiChannel;
using Elastos::Droid::Wifi::CWifiConfiguration;
using Elastos::Droid::Wifi::CWifiConnectionStatistics;
using Elastos::Droid::Wifi::CWifiInfo;
using Elastos::Droid::Wifi::CWifiInfoHelper;
using Elastos::Droid::Wifi::CWifiLinkLayerStats;
using Elastos::Droid::Wifi::CWifiManagerHelper;
using Elastos::Droid::Wifi::CWifiSsid;
using Elastos::Droid::Wifi::CWifiSsidHelper;
using Elastos::Droid::Wifi::CWpsResult;
using Elastos::Droid::Wifi::CRssiPacketCountInfo;
using Elastos::Droid::Wifi::IBatchedScanResult;
using Elastos::Droid::Wifi::IScanResultHelper;
using Elastos::Droid::Wifi::ISupplicantState;
using Elastos::Droid::Wifi::ISupplicantStateHelper;
using Elastos::Droid::Wifi::IWifiConfigurationKeyMgmt;
using Elastos::Droid::Wifi::IWifiConfigurationStatus;
using Elastos::Droid::Wifi::IWifiChannel;
using Elastos::Droid::Wifi::IWifiEnterpriseConfigEap;
using Elastos::Droid::Wifi::IWifiInfoHelper;
using Elastos::Droid::Wifi::IWifiManager;
using Elastos::Droid::Wifi::IWifiManagerHelper;
using Elastos::Droid::Wifi::IWifiSsidHelper;
using Elastos::Droid::Wifi::IWpsInfo;
using Elastos::Droid::Wifi::IWpsResult;
using Elastos::Droid::Wifi::P2p::IWifiP2pManager;
using Elastos::Droid::Wifi::WpsResultStatus;
using Elastos::Droid::Server::Net::CNetlinkTracker;
using Elastos::Core::CoreUtils;
using Elastos::Core::CSystem;
using Elastos::Core::CThread;
using Elastos::Core::ICharSequence;
using Elastos::Core::ISystem;
using Elastos::Core::IThread;
using Elastos::Core::StringBuffer;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::IO::CBufferedReader;
using Elastos::IO::CFileReader;
using Elastos::IO::IBufferedReader;
using Elastos::IO::ICloseable;
using Elastos::IO::IFileReader;
using Elastos::IO::IReader;
using Elastos::Net::IInet4Address;
using Elastos::Net::IInetAddress;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CLinkedList;
using Elastos::Utility::Concurrent::Atomic::CAtomicBoolean;
using Elastos::Utility::Concurrent::Atomic::CAtomicInteger32;
using Elastos::Utility::ILinkedList;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Regex::IPatternHelper;
using Elastos::Utility::Regex::CPatternHelper;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wifi {

const Int32 WifiStateMachine::BASE; //= IProtocol::BASE_WIFI;
const Int32 WifiStateMachine::CMD_START_SUPPLICANT                ; //= BASE + 11;
const Int32 WifiStateMachine::CMD_STOP_SUPPLICANT                 ; //= BASE + 12;
const Int32 WifiStateMachine::CMD_START_DRIVER                    ; //= BASE + 13;
const Int32 WifiStateMachine::CMD_STOP_DRIVER                     ; //= BASE + 14;
const Int32 WifiStateMachine::CMD_STATIC_IP_SUCCESS               ; //= BASE + 15;
const Int32 WifiStateMachine::CMD_STATIC_IP_FAILURE               ; //= BASE + 16;
const Int32 WifiStateMachine::CMD_STOP_SUPPLICANT_FAILED          ; //= BASE + 17;
const Int32 WifiStateMachine::CMD_DELAYED_STOP_DRIVER             ; //= BASE + 18;
const Int32 WifiStateMachine::CMD_DRIVER_START_TIMED_OUT          ; //= BASE + 19;

const Int32 WifiStateMachine::CMD_START_AP                        ; //= BASE + 21;
const Int32 WifiStateMachine::CMD_START_AP_SUCCESS                ; //= BASE + 22;
const Int32 WifiStateMachine::CMD_START_AP_FAILURE                ; //= BASE + 23;
const Int32 WifiStateMachine::CMD_STOP_AP                         ; //= BASE + 24;
const Int32 WifiStateMachine::CMD_SET_AP_CONFIG                   ; //= BASE + 25;
const Int32 WifiStateMachine::CMD_SET_AP_CONFIG_COMPLETED         ; //= BASE + 26;
const Int32 WifiStateMachine::CMD_REQUEST_AP_CONFIG               ; //= BASE + 27;
const Int32 WifiStateMachine::CMD_RESPONSE_AP_CONFIG              ; //= BASE + 28;
const Int32 WifiStateMachine::CMD_TETHER_STATE_CHANGE             ; //= BASE + 29;
const Int32 WifiStateMachine::CMD_TETHER_NOTIFICATION_TIMED_OUT   ; //= BASE + 30;

const Int32 WifiStateMachine::CMD_BLUETOOTH_ADAPTER_STATE_CHANGE  ; //= BASE + 31;

const Int32 WifiStateMachine::CMD_PING_SUPPLICANT                 ; //= BASE + 51;
const Int32 WifiStateMachine::CMD_ADD_OR_UPDATE_NETWORK           ; //= BASE + 52;
const Int32 WifiStateMachine::CMD_REMOVE_NETWORK                  ; //= BASE + 53;
const Int32 WifiStateMachine::CMD_ENABLE_NETWORK                  ; //= BASE + 54;
const Int32 WifiStateMachine::CMD_ENABLE_ALL_NETWORKS             ; //= BASE + 55;
const Int32 WifiStateMachine::CMD_BLACKLIST_NETWORK               ; //= BASE + 56;
const Int32 WifiStateMachine::CMD_CLEAR_BLACKLIST                 ; //= BASE + 57;
const Int32 WifiStateMachine::CMD_SAVE_CONFIG                     ; //= BASE + 58;
const Int32 WifiStateMachine::CMD_GET_CONFIGURED_NETWORKS         ; //= BASE + 59;
const Int32 WifiStateMachine::CMD_GET_CAPABILITY_FREQ             ; //= BASE + 60;
const Int32 WifiStateMachine::CMD_GET_SUPPORTED_FEATURES          ; //= BASE + 61;
const Int32 WifiStateMachine::CMD_GET_PRIVILEGED_CONFIGURED_NETWORKS; //= BASE + 62;
const Int32 WifiStateMachine::CMD_GET_LINK_LAYER_STATS            ; //= BASE + 63;
const Int32 WifiStateMachine::CMD_START_SCAN                      ; //= BASE + 71;
const Int32 WifiStateMachine::CMD_SET_OPERATIONAL_MODE            ; //= BASE + 72;
const Int32 WifiStateMachine::CMD_DISCONNECT                      ; //= BASE + 73;
const Int32 WifiStateMachine::CMD_RECONNECT                       ; //= BASE + 74;
const Int32 WifiStateMachine::CMD_REASSOCIATE                     ; //= BASE + 75;
const Int32 WifiStateMachine::CMD_GET_CONNECTION_STATISTICS       ; //= BASE + 76;

const Int32 WifiStateMachine::CMD_SET_HIGH_PERF_MODE              ; //= BASE + 77;
const Int32 WifiStateMachine::CMD_SET_COUNTRY_CODE                ; //= BASE + 80;
const Int32 WifiStateMachine::CMD_ENABLE_RSSI_POLL                ; //= BASE + 82;
const Int32 WifiStateMachine::CMD_RSSI_POLL                       ; //= BASE + 83;
const Int32 WifiStateMachine::CMD_START_PACKET_FILTERING          ; //= BASE + 84;
const Int32 WifiStateMachine::CMD_STOP_PACKET_FILTERING           ; //= BASE + 85;
const Int32 WifiStateMachine::CMD_SET_SUSPEND_OPT_ENABLED         ; //= BASE + 86;
const Int32 WifiStateMachine::CMD_DELAYED_NETWORK_DISCONNECT      ; //= BASE + 87;
const Int32 WifiStateMachine::CMD_NO_NETWORKS_PERIODIC_SCAN       ; //= BASE + 88;
const Int32 WifiStateMachine::CMD_TEST_NETWORK_DISCONNECT         ; //= BASE + 89;

const Int32 WifiStateMachine::MULTICAST_V6; //= 1;
const Int32 WifiStateMachine::MULTICAST_V4; //= 0;

const Int32 WifiStateMachine::CMD_SET_FREQUENCY_BAND              ; //= BASE + 90;
const Int32 WifiStateMachine::CMD_PNO_PERIODIC_SCAN               ; //= BASE + 91;
const Int32 WifiStateMachine::CMD_ENABLE_TDLS                     ; //= BASE + 92;
const Int32 WifiStateMachine::CMD_OBTAINING_IP_ADDRESS_WATCHDOG_TIMER   ; //= BASE + 93;

const Int32 WifiStateMachine::OBTAINING_IP_ADDRESS_GUARD_TIMER_MSEC; //= 40000;

const Int32 WifiStateMachine::CMD_RESET_SUPPLICANT_STATE          ; //= BASE + 111;

const Int32 WifiStateMachine::ROAM_GUARD_TIMER_MSEC; //= 15000;

const Int32 WifiStateMachine::CMD_ROAM_WATCHDOG_TIMER   ; //= BASE + 94;
const Int32 WifiStateMachine::CMD_SCREEN_STATE_CHANGED             ; //= BASE + 95;

const Int32 WifiStateMachine::DISCONNECTING_GUARD_TIMER_MSEC; //= 5000;

const Int32 WifiStateMachine::CMD_DISCONNECTING_WATCHDOG_TIMER    ; //= BASE + 96;

const Int32 WifiStateMachine::CMD_ENABLE_P2P               ; //= BASE + 131;
const Int32 WifiStateMachine::CMD_DISABLE_P2P_REQ          ; //= BASE + 132;
const Int32 WifiStateMachine::CMD_DISABLE_P2P_RSP          ; //= BASE + 133;

const Int32 WifiStateMachine::CMD_BOOT_COMPLETED           ; //= BASE + 134;

const Int32 WifiStateMachine::CMD_SET_BATCHED_SCAN         ; //= BASE + 135;
const Int32 WifiStateMachine::CMD_START_NEXT_BATCHED_SCAN  ; //= BASE + 136;
const Int32 WifiStateMachine::CMD_POLL_BATCHED_SCAN        ; //= BASE + 137;

const Int32 WifiStateMachine::CMD_IP_CONFIGURATION_SUCCESSFUL     ; //= BASE + 138;
const Int32 WifiStateMachine::CMD_IP_CONFIGURATION_LOST           ; //= BASE + 139;
const Int32 WifiStateMachine::CMD_UPDATE_LINKPROPERTIES           ; //= BASE + 140;

const Int32 WifiStateMachine::CMD_TARGET_BSSID                    ; //= BASE + 141;

const Int32 WifiStateMachine::CMD_RELOAD_TLS_AND_RECONNECT        ; //= BASE + 142;

const Int32 WifiStateMachine::CMD_AUTO_CONNECT                    ; //= BASE + 143;

const Int32 WifiStateMachine::network_status_unwanted_disconnect; //= 0;
const Int32 WifiStateMachine::network_status_unwanted_disable_autojoin; //= 1;

const Int32 WifiStateMachine::CMD_UNWANTED_NETWORK                ; //= BASE + 144;

const Int32 WifiStateMachine::CMD_AUTO_ROAM                       ; //= BASE + 145;

const Int32 WifiStateMachine::CMD_AUTO_SAVE_NETWORK               ; //= BASE + 146;

const Int32 WifiStateMachine::CMD_ASSOCIATED_BSSID               ; //= BASE + 147;

const Int32 WifiStateMachine::CMD_TARGET_SSID                    ; //= BASE + 148;

const Int32 WifiStateMachine::CMD_GET_IBSS_SUPPORTED       ; //= BASE + 149;

const Int32 WifiStateMachine::CONNECT_MODE                  ; //= 1;
const Int32 WifiStateMachine::SCAN_ONLY_MODE                ; //= 2;
const Int32 WifiStateMachine::SCAN_ONLY_WITH_WIFI_OFF_MODE  ; //= 3;

const String WifiStateMachine::NETWORKTYPE("WIFI");
Boolean WifiStateMachine::DBG = FALSE;
Boolean WifiStateMachine::VDBG = FALSE;
Boolean WifiStateMachine::VVDBG = FALSE;
Boolean WifiStateMachine::mLogMessages = FALSE;

const Int32 WifiStateMachine::ONE_HOUR_MILLI = 1000 * 60 * 60;

const String WifiStateMachine::GOOGLE_OUI("DA-A1-19");

const String WifiStateMachine::EXTRA_FEATURE_ID("cneFeatureId");

const String WifiStateMachine::EXTRA_FEATURE_PARAMETER("cneFeatureParameter");

const String WifiStateMachine::EXTRA_PARAMETER_VALUE("cneParameterValue");

const Int32 WifiStateMachine::FEATURE_ID = 1;

const Int32 WifiStateMachine::FEATURE_PARAM = 1;

const Int32 WifiStateMachine::FEATURE_OFF = 1;

const Int32 WifiStateMachine::FEATURE_ON = FEATURE_OFF + 1;

/* temporary debug flag - best network selection development */
Boolean WifiStateMachine::PDBG = FALSE;

AutoPtr<IPattern> InitPattern()
{
    AutoPtr<IPatternHelper> ph;
    CPatternHelper::AcquireSingleton((IPatternHelper**)&ph);
    AutoPtr<IPattern> pattern;
    ph->Compile(String("\t+"), (IPattern**)&pattern);
    return pattern;
}
AutoPtr<IPattern> WifiStateMachine::scanResultPattern = InitPattern();

const Int32 WifiStateMachine::SCAN_RESULT_CACHE_SIZE = 160;

const Int32 WifiStateMachine::UNKNOWN_SCAN_SOURCE = -1;
const Int32 WifiStateMachine::SCAN_ALARM_SOURCE = -2;
const Int32 WifiStateMachine::ADD_OR_UPDATE_SOURCE = -3;

const Int32 WifiStateMachine::SCAN_REQUEST_BUFFER_MAX_SIZE = 10;
const String WifiStateMachine::CUSTOMIZED_SCAN_SETTING("customized_scan_settings");
const String WifiStateMachine::CUSTOMIZED_SCAN_WORKSOURCE("customized_scan_worksource");
const String WifiStateMachine::SCAN_REQUEST_TIME("scan_request_time");

const String WifiStateMachine::BATCHED_SETTING("batched_settings");
const String WifiStateMachine::BATCHED_WORKSOURCE("batched_worksource");

const Int32 WifiStateMachine::POLL_RSSI_INTERVAL_MSECS = 3000;

const Int32 WifiStateMachine::LINK_FLAPPING_DEBOUNCE_MSEC = 7000;

const Int32 WifiStateMachine::SUPPLICANT_RESTART_INTERVAL_MSECS = 5000;

const Int32 WifiStateMachine::SUPPLICANT_RESTART_TRIES = 5;

const Int32 WifiStateMachine::TETHER_NOTIFICATION_TIME_OUT_MSECS = 5000;

const Int32 WifiStateMachine::DRIVER_START_TIME_OUT_MSECS = 10000;

const Int32 WifiStateMachine::SUCCESS = 1;
const Int32 WifiStateMachine::FAILURE = -1;

const Int32 WifiStateMachine::SUSPEND_DUE_TO_DHCP       = 1;
const Int32 WifiStateMachine::SUSPEND_DUE_TO_HIGH_PERF  = 1<<1;
const Int32 WifiStateMachine::SUSPEND_DUE_TO_SCREEN     = 1<<2;

const Int64 WifiStateMachine::maxFullBandConnectedTimeIntervalMilli = 1000 * 60 * 5;

const Int32 WifiStateMachine::MIN_INTERVAL_ENABLE_ALL_NETWORKS_MS = 10 * 60 * 1000; /* 10 minutes */

const Int32 WifiStateMachine::SCAN_REQUEST = 0;
const String WifiStateMachine::ACTION_START_SCAN("com.android.server.WifiManager.action.START_SCAN");

const String WifiStateMachine::DELAYED_STOP_COUNTER("DelayedStopCounter");
const Int32 WifiStateMachine::DRIVER_STOP_REQUEST = 0;
const String WifiStateMachine::ACTION_DELAYED_DRIVER_STOP("com.android.server.WifiManager.action.DELAYED_DRIVER_STOP");

const String WifiStateMachine::ACTION_REFRESH_BATCHED_SCAN("com.android.server.WifiManager.action.REFRESH_BATCHED_SCAN");

Int32 WifiStateMachine::sScanAlarmIntentCount = 0;

Int32 WifiStateMachine::DEFAULT_SCORE = INetworkAgent::WIFI_BASE_SCORE;

const Boolean WifiStateMachine::DEBUG_PARSE = FALSE;

Int32 WifiStateMachine::MESSAGE_HANDLING_STATUS_PROCESSED = 2;
Int32 WifiStateMachine::MESSAGE_HANDLING_STATUS_OK = 1;
Int32 WifiStateMachine::MESSAGE_HANDLING_STATUS_UNKNOWN = 0;
Int32 WifiStateMachine::MESSAGE_HANDLING_STATUS_REFUSED = -1;
Int32 WifiStateMachine::MESSAGE_HANDLING_STATUS_FAIL = -2;
Int32 WifiStateMachine::MESSAGE_HANDLING_STATUS_BUFFERED = -3;
Int32 WifiStateMachine::MESSAGE_HANDLING_STATUS_DEFERRED = -4;
Int32 WifiStateMachine::MESSAGE_HANDLING_STATUS_DISCARD = -5;
Int32 WifiStateMachine::MESSAGE_HANDLING_STATUS_LOOPED = -6;
Int32 WifiStateMachine::MESSAGE_HANDLING_STATUS_HANDLING_ERROR = -7;

Int64 WifiStateMachine::lastScanDuringP2p = 0;

const String WifiStateMachine::ID_STR("id=");
const String WifiStateMachine::BSSID_STR("bssid=");
const String WifiStateMachine::FREQ_STR("freq=");
const String WifiStateMachine::LEVEL_STR("level=");
const String WifiStateMachine::TSF_STR("tsf=");
const String WifiStateMachine::FLAGS_STR("flags=");
const String WifiStateMachine::SSID_STR("ssid=");
const String WifiStateMachine::DELIMITER_STR("====");
const String WifiStateMachine::END_STR("####");

//==============================================================================
//  WifiStateMachine::DefaultState
//==============================================================================

ECode WifiStateMachine::DefaultState::ProcessMessage(
    /* [in] */ IMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    mHost->LogStateAndMessage(message, String("WifiStateMachine::DefaultState"));

    Int32 what;
    message->GetWhat(&what);
    switch (what) {
        case IAsyncChannel::CMD_CHANNEL_HALF_CONNECTED: {
            AutoPtr<IInterface> obj;
            message->GetObj((IInterface**)&obj);
            AutoPtr<IAsyncChannel> ac = IAsyncChannel::Probe(obj);
            if (ac == mHost->mWifiP2pChannel) {
                Int32 arg1;
                message->GetArg1(&arg1);
                if (arg1 == IAsyncChannel::STATUS_SUCCESSFUL) {
                    mHost->mWifiP2pChannel->SendMessage(IAsyncChannel::CMD_CHANNEL_FULL_CONNECTION);
                }
                else {
                    mHost->Loge(String("WifiP2pService connection failure, error=") + StringUtils::ToString(arg1));
                }
            }
            else {
                mHost->Loge(String("got HALF_CONNECTED for unknown channel"));
            }
            break;
        }
        case IAsyncChannel::CMD_CHANNEL_DISCONNECTED: {
            AutoPtr<IInterface> obj;
            message->GetObj((IInterface**)&obj);
            AutoPtr<IAsyncChannel> ac = IAsyncChannel::Probe(obj);
            if (ac == mHost->mWifiP2pChannel) {
                Int32 arg1;
                message->GetArg1(&arg1);
                mHost->Loge(String("WifiP2pService channel lost, message.arg1 =") + StringUtils::ToString(arg1));
                //TODO: Re-establish connection to state machine after a delay
                // mWifiP2pChannel.connect(mContext, getHandler(),
                // mWifiP2pManager.getMessenger());
            }
            break;
        }
        case CMD_BLUETOOTH_ADAPTER_STATE_CHANGE: {
            Int32 arg1;
            message->GetArg1(&arg1);
            mHost->mBluetoothConnectionActive = (arg1 != IBluetoothAdapter::STATE_DISCONNECTED);
            break;
        }
            /* Synchronous call returns */
        case CMD_PING_SUPPLICANT:
        case CMD_ENABLE_NETWORK:
        case CMD_ADD_OR_UPDATE_NETWORK:
        case CMD_REMOVE_NETWORK:
        case CMD_SAVE_CONFIG:
        case CMD_GET_IBSS_SUPPORTED:
            mHost->ReplyToMessage(message, what, FAILURE);
            break;
        case CMD_GET_CAPABILITY_FREQ:
            mHost->ReplyToMessage(message, what, (IInterface*)NULL);
            break;
        case CMD_GET_CONFIGURED_NETWORKS:
            mHost->ReplyToMessage(message, what, (IList*)NULL);
            break;
        case CMD_GET_PRIVILEGED_CONFIGURED_NETWORKS:
            mHost->ReplyToMessage(message, what, (IList*)NULL);
            break;
        case CMD_ENABLE_RSSI_POLL: {
            Int32 arg1;
            message->GetArg1(&arg1);
            mHost->mEnableRssiPolling = (arg1 == 1);
            break;
        }
        case CMD_SET_HIGH_PERF_MODE: {
            Int32 arg1;
            message->GetArg1(&arg1);
            if (arg1 == 1) {
                mHost->SetSuspendOptimizations(SUSPEND_DUE_TO_HIGH_PERF, FALSE);
            }
            else {
                mHost->SetSuspendOptimizations(SUSPEND_DUE_TO_HIGH_PERF, TRUE);
            }
            break;
        }
        case CMD_BOOT_COMPLETED: {
            String countryCode = mHost->mPersistedCountryCode;
            if (countryCode.IsEmpty() == FALSE) {
                AutoPtr<IContentResolver> cr;
                mHost->mContext->GetContentResolver((IContentResolver**)&cr);
                Boolean b;
                Settings::Global::PutString(cr,
                        ISettingsGlobal::WIFI_COUNTRY_CODE,
                        countryCode, &b);
                // It may be that the state transition that should send this info
                // to the driver happened between mPersistedCountryCode getting set
                // and now, so simply persisting it here would mean we have sent
                // nothing to the driver.  Send the cmd so it might be set now.
                Int32 sequenceNum;
                mHost->mCountryCodeSequence->IncrementAndGet(&sequenceNum);
                mHost->SendMessageAtFrontOfQueue(CMD_SET_COUNTRY_CODE,
                        sequenceNum, 0, CoreUtils::Convert(countryCode));
            }

            mHost->CheckAndSetConnectivityInstance();
            AutoPtr<IHandler> handler;
            mHost->GetHandler((IHandler**)&handler);
            AutoPtr<ILooper> looper;
            handler->GetLooper((ILooper**)&looper);
            mHost->mNetworkFactory = new WifiNetworkFactory(looper, mHost->mContext,
                    NETWORKTYPE, mHost->mNetworkCapabilitiesFilter);
            mHost->mNetworkFactory->SetScoreFilter(60);
            AutoPtr<IMessenger> messenger;
            CMessenger::New(IHandler::Probe(mHost->mNetworkFactory), (IMessenger**)&messenger);
            mHost->mCm->RegisterNetworkFactory(messenger, NETWORKTYPE);
            break;
        }
        case CMD_SET_BATCHED_SCAN: {
            Int32 arg1;
            message->GetArg1(&arg1);
            Int32 arg2;
            message->GetArg2(&arg2);
            AutoPtr<IInterface> obj;
            message->GetObj((IInterface**)&obj);
            mHost->RecordBatchedScanSettings(arg1, arg2, IBundle::Probe(obj));
            break;
        }
        case CMD_POLL_BATCHED_SCAN:
            mHost->HandleBatchedScanPollRequest();
            break;
        case CMD_START_NEXT_BATCHED_SCAN:
            mHost->StartNextBatchedScan();
            break;
        case CMD_SCREEN_STATE_CHANGED: {
            Int32 arg1;
            message->GetArg1(&arg1);
            mHost->HandleScreenStateChanged(arg1 != 0,
                    /* startBackgroundScanIfNeeded = */ FALSE);
            break;
        }
            /* Discard */
        case CMD_START_SCAN:
            mHost->messageHandlingStatus = MESSAGE_HANDLING_STATUS_DISCARD;
            break;
        case CMD_START_SUPPLICANT:
        case CMD_STOP_SUPPLICANT:
        case CMD_STOP_SUPPLICANT_FAILED:
        case CMD_START_DRIVER:
        case CMD_STOP_DRIVER:
        case CMD_DELAYED_STOP_DRIVER:
        case CMD_DRIVER_START_TIMED_OUT:
        case CMD_START_AP:
        case CMD_START_AP_SUCCESS:
        case CMD_START_AP_FAILURE:
        case CMD_STOP_AP:
        case CMD_TETHER_STATE_CHANGE:
        case CMD_TETHER_NOTIFICATION_TIMED_OUT:
        case CMD_DISCONNECT:
        case CMD_RECONNECT:
        case CMD_REASSOCIATE:
        case CMD_RELOAD_TLS_AND_RECONNECT:
        case WifiMonitor::SUP_CONNECTION_EVENT:
        case WifiMonitor::SUP_DISCONNECTION_EVENT:
        case WifiMonitor::NETWORK_CONNECTION_EVENT:
        case WifiMonitor::NETWORK_DISCONNECTION_EVENT:
        case WifiMonitor::SCAN_RESULTS_EVENT:
        case WifiMonitor::SUPPLICANT_STATE_CHANGE_EVENT:
        case WifiMonitor::AUTHENTICATION_FAILURE_EVENT:
        case WifiMonitor::ASSOCIATION_REJECTION_EVENT:
        case WifiMonitor::WPS_OVERLAP_EVENT:
        case CMD_BLACKLIST_NETWORK:
        case CMD_CLEAR_BLACKLIST:
        case CMD_SET_OPERATIONAL_MODE:
        case CMD_SET_COUNTRY_CODE:
        case CMD_SET_FREQUENCY_BAND:
        case CMD_RSSI_POLL:
        case CMD_ENABLE_ALL_NETWORKS:
        case IDhcpStateMachine::CMD_PRE_DHCP_ACTION:
        case IDhcpStateMachine::CMD_POST_DHCP_ACTION:
        /* Handled by WifiApConfigStore */
        case CMD_SET_AP_CONFIG:
        case CMD_SET_AP_CONFIG_COMPLETED:
        case CMD_REQUEST_AP_CONFIG:
        case CMD_RESPONSE_AP_CONFIG:
        case WifiWatchdogStateMachine::POOR_LINK_DETECTED:
        case WifiWatchdogStateMachine::GOOD_LINK_DETECTED:
        case CMD_NO_NETWORKS_PERIODIC_SCAN:
        case CMD_DISABLE_P2P_RSP:
        case WifiMonitor::SUP_REQUEST_IDENTITY:
        case CMD_TEST_NETWORK_DISCONNECT:
        case CMD_OBTAINING_IP_ADDRESS_WATCHDOG_TIMER:
        case WifiMonitor::SUP_REQUEST_SIM_AUTH:
        case CMD_TARGET_BSSID:
        case CMD_AUTO_CONNECT:
        case CMD_AUTO_ROAM:
        case CMD_AUTO_SAVE_NETWORK:
        case CMD_ASSOCIATED_BSSID:
        case CMD_UNWANTED_NETWORK:
        case CMD_DISCONNECTING_WATCHDOG_TIMER:
        case CMD_ROAM_WATCHDOG_TIMER:
            mHost->messageHandlingStatus = MESSAGE_HANDLING_STATUS_DISCARD;
            break;
        case IDhcpStateMachine::CMD_ON_QUIT:
            mHost->mDhcpStateMachine = NULL;
            break;
        case CMD_SET_SUSPEND_OPT_ENABLED: {
            Int32 arg1;
            message->GetArg1(&arg1);
            if (arg1 == 1) {
                mHost->mSuspendWakeLock->ReleaseLock();
                mHost->SetSuspendOptimizations(SUSPEND_DUE_TO_SCREEN, TRUE);
            }
            else {
                mHost->SetSuspendOptimizations(SUSPEND_DUE_TO_SCREEN, FALSE);
            }
            break;
        }
        case WifiMonitor::DRIVER_HUNG_EVENT:
            mHost->SetSupplicantRunning(FALSE);
            mHost->SetSupplicantRunning(TRUE);
            break;
        case IWifiManager::CONNECT_NETWORK:
            mHost->ReplyToMessage(message, IWifiManager::CONNECT_NETWORK_FAILED,
                    IWifiManager::BUSY);
            break;
        case IWifiManager::FORGET_NETWORK:
            mHost->ReplyToMessage(message, IWifiManager::FORGET_NETWORK_FAILED,
                    IWifiManager::BUSY);
            break;
        case IWifiManager::SAVE_NETWORK:
            mHost->messageHandlingStatus = MESSAGE_HANDLING_STATUS_FAIL;
            mHost->ReplyToMessage(message, IWifiManager::SAVE_NETWORK_FAILED,
                    IWifiManager::BUSY);
            break;
        case IWifiManager::START_WPS:
            mHost->ReplyToMessage(message, IWifiManager::WPS_FAILED,
                    IWifiManager::BUSY);
            break;
        case IWifiManager::CANCEL_WPS:
            mHost->ReplyToMessage(message, IWifiManager::CANCEL_WPS_FAILED,
                    IWifiManager::BUSY);
            break;
        case IWifiManager::DISABLE_NETWORK:
            mHost->ReplyToMessage(message, IWifiManager::DISABLE_NETWORK_FAILED,
                    IWifiManager::BUSY);
            break;
        case IWifiManager::RSSI_PKTCNT_FETCH:
            mHost->ReplyToMessage(message, IWifiManager::RSSI_PKTCNT_FETCH_FAILED,
                    IWifiManager::BUSY);
            break;
        case CMD_GET_SUPPORTED_FEATURES:
            if (WifiNative::StartHal()) {
                Int32 featureSet = WifiNative::GetSupportedFeatureSet();
                mHost->ReplyToMessage(message, what, featureSet);
            }
            else {
                mHost->ReplyToMessage(message, what, 0);
            }
            break;
        case CMD_GET_LINK_LAYER_STATS:
            // Not supported hence reply with error message
            mHost->ReplyToMessage(message, what, (IInterface*)NULL);
            break;
        case WifiP2pServiceImpl::P2P_CONNECTION_CHANGED: {
            AutoPtr<IInterface> obj;
            message->GetObj((IInterface**)&obj);
            Boolean b;
            INetworkInfo::Probe(obj)->IsConnected(&b);
            mHost->mP2pConnected->Set(b);
            break;
        }
        case WifiP2pServiceImpl::DISCONNECT_WIFI_REQUEST: {
            Int32 arg1;
            message->GetArg1(&arg1);
            mHost->mTemporarilyDisconnectWifi = (arg1 == 1);
            mHost->ReplyToMessage(message, WifiP2pServiceImpl::DISCONNECT_WIFI_RESPONSE);
            break;
        }
        case WifiP2pServiceImpl::P2P_MIRACAST_MODE_CHANGED:
            break;
        /* Link configuration (IP address, DNS, ...) changes notified via netlink */
        case CMD_UPDATE_LINKPROPERTIES: {
            AutoPtr<IInterface> obj;
            message->GetObj((IInterface**)&obj);
            mHost->UpdateLinkProperties(CMD_UPDATE_LINKPROPERTIES, ILinkProperties::Probe(obj));
            break;
        }
        case CMD_IP_CONFIGURATION_SUCCESSFUL:
        case CMD_IP_CONFIGURATION_LOST:
            mHost->messageHandlingStatus = MESSAGE_HANDLING_STATUS_DISCARD;
            break;
        case CMD_GET_CONNECTION_STATISTICS:
            mHost->ReplyToMessage(message, what, mHost->mWifiConnectionStatistics);
            break;
        default:
            mHost->Loge(String("Error! unhandled message"));
            break;
    }
    *result = HANDLED;
    return NOERROR;
}

String WifiStateMachine::DefaultState::GetName()
{
    return String("DefaultState");
}

//==============================================================================
//  WifiStateMachine::InitialState
//==============================================================================

ECode WifiStateMachine::InitialState::Enter()
{
    mHost->mWifiNative->UnloadDriver();

    AutoPtr<IHandler> handler;
    mHost->GetHandler((IHandler**)&handler);
    if (mHost->mWifiP2pChannel == NULL) {
        CAsyncChannel::New((IAsyncChannel**)&mHost->mWifiP2pChannel);
        AutoPtr<IMessenger> messenger;
        mHost->mWifiP2pServiceImpl->GetP2pStateMachineMessenger((IMessenger**)&messenger);
        mHost->mWifiP2pChannel->Connect(mHost->mContext, handler, messenger);
    }

    if (mHost->mWifiApConfigChannel == NULL) {
        CAsyncChannel::New((IAsyncChannel**)&mHost->mWifiApConfigChannel);
        AutoPtr<WifiApConfigStore> wifiApConfigStore =
                WifiApConfigStore::MakeWifiApConfigStore(mHost->mContext, handler);
        wifiApConfigStore->LoadApConfiguration();
        AutoPtr<IMessenger> messenger;
        wifiApConfigStore->GetMessenger((IMessenger**)&messenger);
        Int32 val;
        mHost->mWifiApConfigChannel->ConnectSync(mHost->mContext, handler, messenger, &val);
    }

    return NOERROR;
}

ECode WifiStateMachine::InitialState::ProcessMessage(
    /* [in] */ IMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    mHost->LogStateAndMessage(message, String("WifiStateMachine::InitialState"));

    Int32 what;
    message->GetWhat(&what);
    switch (what) {
        case CMD_START_SUPPLICANT:
            if (mHost->mWifiNative->LoadDriver()) {
                // try {
                mHost->mNwService->WifiFirmwareReload(mHost->mInterfaceName, String("STA"));
                // } catch (Exception e) {
                //     Loge("Failed to reload STA firmware " + e);
                //     // Continue
                // }

                // try {
                // A runtime crash can leave the interface up and
                // IP addresses configured, and this affects
                // connectivity when supplicant starts up.
                // Ensure interface is down and we have no IP
                // addresses before a supplicant start.
                mHost->mNwService->SetInterfaceDown(mHost->mInterfaceName);
                mHost->mNwService->ClearInterfaceAddresses(mHost->mInterfaceName);

                // Set privacy extensions
                mHost->mNwService->SetInterfaceIpv6PrivacyExtensions(mHost->mInterfaceName, TRUE);

               // IPv6 is enabled only as long as access point is connected since:
               // - IPv6 addresses and routes stick around after disconnection
               // - kernel is unaware when connected and fails to start IPv6 negotiation
               // - kernel can start autoconfiguration when 802.1x is not complete
                mHost->mNwService->DisableIpv6(mHost->mInterfaceName);
                // } catch (RemoteException re) {
                //     Loge("Unable to change interface settings: " + re);
                // } catch (IllegalStateException ie) {
                //     Loge("Unable to change interface settings: " + ie);
                // }

               /* Stop a running supplicant after a runtime restart
                * Avoids issues with drivers that do not handle interface down
                * on a running supplicant properly.
                */
                mHost->mWifiMonitor->KillSupplicant(mHost->mP2pSupported);
                if(mHost->mWifiNative->StartSupplicant(mHost->mP2pSupported)) {
                    mHost->SetWifiState(IWifiManager::WIFI_STATE_ENABLING);
                    if (DBG) {
                        mHost->Log(String("Supplicant start successful"));
                    }
                    mHost->mWifiMonitor->StartMonitoring();
                    mHost->TransitionTo(mHost->mSupplicantStartingState);
                }
                else {
                    mHost->Loge(String("Failed to start supplicant!"));
                }
            }
            else {
                mHost->Loge(String("Failed to load driver"));
            }
            break;
        case CMD_START_AP:
            if (mHost->mWifiNative->LoadDriver()) {
                mHost->SetWifiApState(IWifiManager::WIFI_AP_STATE_ENABLING);
                mHost->TransitionTo(mHost->mSoftApStartingState);
            }
            else {
                mHost->Loge(String("Failed to load driver for softap"));
            }
        default:
            *result = NOT_HANDLED;
            return NOERROR;
    }
    *result = HANDLED;
    return NOERROR;
}

String WifiStateMachine::InitialState::GetName()
{
    return String("InitialState");
}

//==============================================================================
//  WifiStateMachine::SupplicantStartingState
//==============================================================================

ECode WifiStateMachine::SupplicantStartingState::ProcessMessage(
    /* [in] */ IMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    mHost->LogStateAndMessage(message, String("WifiStateMachine::SupplicantStartingState"));

    Int32 what;
    message->GetWhat(&what);
    switch (what) {
        case WifiMonitor::SUP_CONNECTION_EVENT:
            if (DBG) mHost->Log(String("Supplicant connection established"));
            mHost->SetWifiState(IWifiManager::WIFI_STATE_ENABLED);
            mHost->mSupplicantRestartCount = 0;
            /* Reset the supplicant state to indicate the supplicant
             * state is not known at this time */
            mHost->mSupplicantStateTracker->SendMessage(CMD_RESET_SUPPLICANT_STATE);
            /* Initialize data structures */
            mHost->mLastBssid = NULL;
            mHost->mLastNetworkId = IWifiConfiguration::INVALID_NETWORK_ID;
            mHost->mLastSignalLevel = -1;

            mHost->mWifiInfo->SetMacAddress(mHost->mWifiNative->GetMacAddress());
            mHost->mWifiNative->EnableSaveConfig();
            mHost->mWifiConfigStore->LoadAndEnableAllNetworks();
            if (mHost->mWifiConfigStore->enableVerboseLogging > 0) {
                mHost->EnableVerboseLogging(mHost->mWifiConfigStore->enableVerboseLogging);
            }
            if (mHost->mWifiConfigStore->associatedPartialScanPeriodMilli < 0) {
                mHost->mWifiConfigStore->associatedPartialScanPeriodMilli = 0;
            }
            InitializeWpsDetails();

            mHost->mIbssSupported = mHost->mWifiNative->GetModeCapability(String("IBSS"));

            mHost->SendSupplicantConnectionChangedBroadcast(TRUE);
            mHost->TransitionTo(mHost->mDriverStartedState);
            break;
        case WifiMonitor::SUP_DISCONNECTION_EVENT:
            if (++mHost->mSupplicantRestartCount <= SUPPLICANT_RESTART_TRIES) {
                mHost->Loge(String("Failed to setup control channel, restart supplicant"));
                mHost->mWifiMonitor->KillSupplicant(mHost->mP2pSupported);
                mHost->TransitionTo(mHost->mInitialState);
                mHost->SendMessageDelayed(CMD_START_SUPPLICANT, SUPPLICANT_RESTART_INTERVAL_MSECS);
            }
            else {
                mHost->Loge(String("Failed ") + mHost->mSupplicantRestartCount +
                        " times to start supplicant, unload driver");
                mHost->mSupplicantRestartCount = 0;
                mHost->SetWifiState(IWifiManager::WIFI_STATE_UNKNOWN);
                mHost->TransitionTo(mHost->mInitialState);
            }
            break;
        case CMD_START_SUPPLICANT:
        case CMD_STOP_SUPPLICANT:
        case CMD_START_AP:
        case CMD_STOP_AP:
        case CMD_START_DRIVER:
        case CMD_STOP_DRIVER:
        case CMD_SET_OPERATIONAL_MODE:
        case CMD_SET_COUNTRY_CODE:
        case CMD_SET_FREQUENCY_BAND:
        case CMD_START_PACKET_FILTERING:
        case CMD_STOP_PACKET_FILTERING:
        case CMD_GET_IBSS_SUPPORTED:
            mHost->messageHandlingStatus = MESSAGE_HANDLING_STATUS_DEFERRED;
            mHost->DeferMessage(message);
            break;
        default:
            *result = NOT_HANDLED;
            return NOERROR;
    }
    *result = HANDLED;
    return NOERROR;
}

void WifiStateMachine::SupplicantStartingState::InitializeWpsDetails()
{
    AutoPtr<ISystemProperties> prop;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&prop);
    String detail;
    prop->Get(String("ro.product.name"), String(""), &detail);
    if (!mHost->mWifiNative->SetDeviceName(detail)) {
        mHost->Loge(String("Failed to set device name ") + detail);
    }
    prop->Get(String("ro.product.manufacturer"), String(""), &detail);
    if (!mHost->mWifiNative->SetManufacturer(detail)) {
        mHost->Loge(String("Failed to set manufacturer ") + detail);
    }
    prop->Get(String("ro.product.model"), String(""), &detail);
    if (!mHost->mWifiNative->SetModelName(detail)) {
        mHost->Loge(String("Failed to set model name ") + detail);
    }
    prop->Get(String("ro.product.model"), String(""), &detail);
    if (!mHost->mWifiNative->SetModelNumber(detail)) {
        mHost->Loge(String("Failed to set model number ") + detail);
    }
    prop->Get(String("ro.serialno"), String(""), &detail);
    if (!mHost->mWifiNative->SetSerialNumber(detail)) {
        mHost->Loge(String("Failed to set serial number ") + detail);
    }
    if (!mHost->mWifiNative->SetConfigMethods(String("physical_display virtual_push_button"))) {
        mHost->Loge(String("Failed to set WPS config methods"));
    }
    if (!mHost->mWifiNative->SetDeviceType(mHost->mPrimaryDeviceType)) {
        mHost->Loge(String("Failed to set primary device type ") + mHost->mPrimaryDeviceType);
    }
}

String WifiStateMachine::SupplicantStartingState::GetName()
{
    return String("SupplicantStartingState");
}

//==============================================================================
//  WifiStateMachine::SupplicantStartedState
//==============================================================================

ECode WifiStateMachine::SupplicantStartedState::Enter()
{
    /* Wifi is available as long as we have a connection to supplicant */
    mHost->mNetworkInfo->SetIsAvailable(TRUE);
    if (mHost->mNetworkAgent != NULL) mHost->mNetworkAgent->SendNetworkInfo(mHost->mNetworkInfo);

    AutoPtr<IResources> res;
    mHost->mContext->GetResources((IResources**)&res);
    Int32 defaultInterval;
    res->GetInteger(R::integer::config_wifi_supplicant_scan_interval, &defaultInterval);

    AutoPtr<IContentResolver> cr;
    mHost->mContext->GetContentResolver((IContentResolver**)&cr);
    Settings::Global::GetInt64(cr,
            ISettingsGlobal::WIFI_SUPPLICANT_SCAN_INTERVAL_MS,
            defaultInterval, &mHost->mSupplicantScanIntervalMs);

    mHost->mWifiNative->SetScanInterval((Int32)mHost->mSupplicantScanIntervalMs / 1000);
    mHost->mWifiNative->SetExternalSim(TRUE);

    mHost->SetRandomMacOui();
    if (mHost->mWifiConfigStore->enableAutoJoinWhenAssociated) {
        mHost->mWifiNative->EnableAutoConnect(FALSE);
    }
    else {
        if (DBG) {
            mHost->Log(String("Autojoin is disabled, keep autoconnect enabled in supplicant"));
        }
    }
    return NOERROR;
}

ECode WifiStateMachine::SupplicantStartedState::ProcessMessage(
    /* [in] */ IMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    mHost->LogStateAndMessage(message, String("WifiStateMachine::SupplicantStartedState"));

    Int32 what;
    message->GetWhat(&what);
    switch (what) {
        case CMD_STOP_SUPPLICANT:   /* Supplicant stopped by user */
            if (mHost->mP2pSupported) {
                mHost->TransitionTo(mHost->mWaitForP2pDisableState);
            }
            else {
                mHost->TransitionTo(mHost->mSupplicantStoppingState);
            }
            break;
        case WifiMonitor::SUP_DISCONNECTION_EVENT:  /* Supplicant connection lost */
            mHost->Loge(String("Connection lost, restart supplicant"));
            mHost->HandleSupplicantConnectionLoss(TRUE);
            mHost->HandleNetworkDisconnect();
            mHost->mSupplicantStateTracker->SendMessage(CMD_RESET_SUPPLICANT_STATE);
            if (mHost->mP2pSupported) {
                mHost->TransitionTo(mHost->mWaitForP2pDisableState);
            }
            else {
                mHost->TransitionTo(mHost->mInitialState);
            }
            mHost->SendMessageDelayed(CMD_START_SUPPLICANT, SUPPLICANT_RESTART_INTERVAL_MSECS);
            break;
        case WifiMonitor::SCAN_RESULTS_EVENT: {
            mHost->CloseRadioScanStats();
            mHost->NoteScanEnd();
            mHost->SetScanResults();
            if (mHost->mIsFullScanOngoing) {
                /* Just updated results from full scan, let apps know about this */
                mHost->SendScanResultsAvailableBroadcast();
            }
            mHost->mIsScanOngoing = FALSE;
            mHost->mIsFullScanOngoing = FALSE;
            Int32 size;
            mHost->mBufferedScanMsg->GetSize(&size);
            if (size > 0) {
                AutoPtr<IInterface> obj;
                mHost->mBufferedScanMsg->Remove((IInterface**)&obj);
                mHost->SendMessage(IMessage::Probe(obj));
            }
            break;
        }
        case CMD_PING_SUPPLICANT: {
            Boolean ok = mHost->mWifiNative->Ping();
            mHost->ReplyToMessage(message, what, ok ? SUCCESS : FAILURE);
            break;
        }
        case CMD_GET_CAPABILITY_FREQ: {
            String freqs = mHost->mWifiNative->GetFreqCapability();
            mHost->ReplyToMessage(message, what, CoreUtils::Convert(freqs));
            break;
        }
        case CMD_START_AP:
            /* Cannot start soft AP while in client mode */
            mHost->Loge(String("Failed to start soft AP with a running supplicant"));
            mHost->SetWifiApState(IWifiManager::WIFI_AP_STATE_FAILED);
            break;
        case CMD_SET_OPERATIONAL_MODE: {
            Int32 arg1;
            message->GetArg1(&arg1);
            mHost->mOperationalMode = arg1;
            break;
        }
        case CMD_TARGET_BSSID: {
            // Trying to associate to this BSSID
            AutoPtr<IInterface> obj;
            message->GetObj((IInterface**)&obj);
            if (obj != NULL) {
                ICharSequence::Probe(obj)->ToString(&mHost->mTargetRoamBSSID);
            }
            break;
        }
        case CMD_GET_LINK_LAYER_STATS: {
            AutoPtr<IWifiLinkLayerStats> stats;
            mHost->GetWifiLinkLayerStats(DBG, (IWifiLinkLayerStats**)&stats);
            if (stats == NULL) {
                // When firmware doesnt support link layer stats, return an empty object
                CWifiLinkLayerStats::New((IWifiLinkLayerStats**)&stats);
            }
            mHost->ReplyToMessage(message, what, stats);
            break;
        }
        case CMD_GET_IBSS_SUPPORTED:
            mHost->DeferMessage(message);
            break;
        default:
            *result = NOT_HANDLED;
            return NOERROR;
    }
    *result = HANDLED;
    return NOERROR;
}

ECode WifiStateMachine::SupplicantStartedState::Exit()
{
    mHost->mNetworkInfo->SetIsAvailable(FALSE);
    if (mHost->mNetworkAgent != NULL) mHost->mNetworkAgent->SendNetworkInfo(mHost->mNetworkInfo);
    return NOERROR;
}

String WifiStateMachine::SupplicantStartedState::GetName()
{
    return String("SupplicantStartedState");
}

//==============================================================================
//  WifiStateMachine::SupplicantStoppingState
//==============================================================================

ECode WifiStateMachine::SupplicantStoppingState::Enter()
{
    /* Send any reset commands to supplicant before shutting it down */
    mHost->HandleNetworkDisconnect();
    if (mHost->mDhcpStateMachine != NULL) {
        mHost->mDhcpStateMachine->DoQuit();
    }

    if (DBG) mHost->Log(String("stopping supplicant"));
    mHost->mWifiMonitor->StopSupplicant();

    /* Send ourselves a delayed message to indicate failure after a wait time */
    AutoPtr<IMessage> msg;
    mHost->ObtainMessage(CMD_STOP_SUPPLICANT_FAILED, ++mHost->mSupplicantStopFailureToken, 0, (IMessage**)&msg);
    mHost->SendMessageDelayed(msg, SUPPLICANT_RESTART_INTERVAL_MSECS);
    mHost->SetWifiState(IWifiManager::WIFI_STATE_DISABLING);
    mHost->mSupplicantStateTracker->SendMessage(CMD_RESET_SUPPLICANT_STATE);
    return NOERROR;
}

ECode WifiStateMachine::SupplicantStoppingState::ProcessMessage(
    /* [in] */ IMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    mHost->LogStateAndMessage(message, String("WifiStateMachine::SupplicantStoppingState"));

    Int32 what;
    message->GetWhat(&what);
    switch (what) {
        case WifiMonitor::SUP_CONNECTION_EVENT:
            mHost->Loge(String("Supplicant connection received while stopping"));
            break;
        case WifiMonitor::SUP_DISCONNECTION_EVENT:
            if (DBG) mHost->Log(String("Supplicant connection lost"));
            mHost->HandleSupplicantConnectionLoss(FALSE);
            mHost->TransitionTo(mHost->mInitialState);
            break;
        case CMD_STOP_SUPPLICANT_FAILED:
            Int32 arg1;
            message->GetArg1(&arg1);
            if (arg1 == mHost->mSupplicantStopFailureToken) {
                mHost->Loge(String("Timed out on a supplicant stop, kill and proceed"));
                mHost->HandleSupplicantConnectionLoss(TRUE);
                mHost->TransitionTo(mHost->mInitialState);
            }
            break;
        case CMD_START_SUPPLICANT:
        case CMD_STOP_SUPPLICANT:
        case CMD_START_AP:
        case CMD_STOP_AP:
        case CMD_START_DRIVER:
        case CMD_STOP_DRIVER:
        case CMD_SET_OPERATIONAL_MODE:
        case CMD_SET_COUNTRY_CODE:
        case CMD_SET_FREQUENCY_BAND:
        case CMD_START_PACKET_FILTERING:
        case CMD_STOP_PACKET_FILTERING:
            mHost->DeferMessage(message);
            break;
        default:
            *result = NOT_HANDLED;
            return NOERROR;
    }
    *result = HANDLED;
    return NOERROR;
}

String WifiStateMachine::SupplicantStoppingState::GetName()
{
    return String("SupplicantStoppingState");
}

//==============================================================================
//  WifiStateMachine::DriverStartingState
//==============================================================================

ECode WifiStateMachine::DriverStartingState::Enter()
{
    mTries = 1;
    /* Send ourselves a delayed message to start driver a second time */
    AutoPtr<IMessage> msg;
    mHost->ObtainMessage(CMD_DRIVER_START_TIMED_OUT, ++mHost->mDriverStartToken, 0, (IMessage**)&msg);
    mHost->SendMessageDelayed(msg, DRIVER_START_TIME_OUT_MSECS);
    return NOERROR;
}

ECode WifiStateMachine::DriverStartingState::ProcessMessage(
    /* [in] */ IMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    mHost->LogStateAndMessage(message, String("WifiStateMachine::DriverStartingState"));

    Int32 what;
    message->GetWhat(&what);
    switch (what) {
       case WifiMonitor::SUPPLICANT_STATE_CHANGE_EVENT: {
            SupplicantState state = mHost->HandleSupplicantStateChange(message);
            /* If suplicant is exiting out of INTERFACE_DISABLED state into
             * a state that indicates driver has started, it is ready to
             * receive driver commands
             */
            AutoPtr<ISupplicantStateHelper> helper;
            CSupplicantStateHelper::AcquireSingleton((ISupplicantStateHelper**)&helper);
            Boolean b;
            helper->IsDriverActive(state, &b);
            if (b) {
                mHost->TransitionTo(mHost->mDriverStartedState);
            }
            break;
        }
        case CMD_DRIVER_START_TIMED_OUT: {
            Int32 arg1;
            message->GetArg1(&arg1);
            if (arg1 == mHost->mDriverStartToken) {
                if (mTries >= 2) {
                    mHost->Loge(String("Failed to start driver after ") + mTries);
                    mHost->TransitionTo(mHost->mDriverStoppedState);
                }
                else {
                    mHost->Loge(String("Driver start failed, retrying"));
                    mHost->mWakeLock->AcquireLock();
                    mHost->mWifiNative->StartDriver();
                    mHost->mWakeLock->ReleaseLock();

                    ++mTries;
                    /* Send ourselves a delayed message to start driver again */
                    AutoPtr<IMessage> msg;
                    mHost->ObtainMessage(CMD_DRIVER_START_TIMED_OUT, ++mHost->mDriverStartToken, 0, (IMessage**)&msg);
                    mHost->SendMessageDelayed(msg, DRIVER_START_TIME_OUT_MSECS);
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
        case WifiMonitor::ASSOCIATION_REJECTION_EVENT:
        case WifiMonitor::WPS_OVERLAP_EVENT:
        case CMD_SET_COUNTRY_CODE:
        case CMD_SET_FREQUENCY_BAND:
        case CMD_START_PACKET_FILTERING:
        case CMD_STOP_PACKET_FILTERING:
        case CMD_START_SCAN:
        case CMD_DISCONNECT:
        case CMD_REASSOCIATE:
        case CMD_RECONNECT:
            mHost->messageHandlingStatus = MESSAGE_HANDLING_STATUS_DEFERRED;
            mHost->DeferMessage(message);
            break;
        case WifiMonitor::SCAN_RESULTS_EVENT:
            // Loose scan results obtained in Driver Starting state, they can only confuse
            // the state machine
            break;
        default:
            *result = NOT_HANDLED;
            return NOERROR;
    }
    *result = HANDLED;
    return NOERROR;
}

String WifiStateMachine::DriverStartingState::GetName()
{
    return String("DriverStartingState");
}

//==============================================================================
//  WifiStateMachine::DriverStartedState
//==============================================================================

ECode WifiStateMachine::DriverStartedState::Enter()
{
    if (PDBG) {
        mHost->Loge(String("Driverstarted State enter"));
    }
    mHost->mIsRunning = TRUE;
    mHost->mInDelayedStop = FALSE;
    mHost->mDelayedStopCounter++;
    mHost->UpdateBatteryWorkSource(NULL);
    /**
     * Enable bluetooth coexistence scan mode when bluetooth connection is active.
     * When this mode is on, some of the low-level scan parameters used by the
     * driver are changed to reduce interference with bluetooth
     */
    mHost->mWifiNative->SetBluetoothCoexistenceScanMode(mHost->mBluetoothConnectionActive);
    /* set country code */
    mHost->SetCountryCode();
    /* set frequency band of operation */
    mHost->SetFrequencyBand();
    /* initialize network state */
    mHost->SetNetworkDetailedState(Elastos::Droid::Net::NetworkInfoDetailedState_DISCONNECTED);

    /* Remove any filtering on Multicast v6 at start */
    mHost->mWifiNative->StopFilteringMulticastV6Packets();

    /* Reset Multicast v4 filtering state */
    Boolean b;
    mHost->mFilteringMulticastV4Packets->Get(&b);
    if (b) {
        mHost->mWifiNative->StartFilteringMulticastV4Packets();
    }
    else {
        mHost->mWifiNative->StopFilteringMulticastV4Packets();
    }

    mHost->mDhcpActive = FALSE;

    mHost->StartBatchedScan();

    if (mHost->mOperationalMode != CONNECT_MODE) {
        mHost->mWifiNative->Disconnect();
        mHost->mWifiConfigStore->DisableAllNetworks();
        if (mHost->mOperationalMode == SCAN_ONLY_WITH_WIFI_OFF_MODE) {
            mHost->SetWifiState(IWifiManager::WIFI_STATE_DISABLED);
        }
        mHost->TransitionTo(mHost->mScanModeState);
    }
    else {

        // Status pulls in the current supplicant state and network connection state
        // events over the monitor connection. This helps framework sync up with
        // current supplicant state
        // TODO: actually check th supplicant status string and make sure the supplicant
        // is in disconnecte4d state.
        mHost->mWifiNative->Status();
        // Transitioning to Disconnected state will trigger a scan and subsequently AutoJoin
        mHost->TransitionTo(mHost->mDisconnectedState);
    }

    // We may have missed screen update at boot
    mHost->mScreenBroadcastReceived->Get(&b);
    if (b == FALSE) {
        AutoPtr<IInterface> service;
        mHost->mContext->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&service);
        AutoPtr<IPowerManager> powerManager = IPowerManager::Probe(service);
        powerManager->IsScreenOn(&b);
        mHost->HandleScreenStateChanged(b,
                /* startBackgroundScanIfNeeded = */ FALSE);
    }
    else {
        // Set the right suspend mode settings
        mHost->mUserWantsSuspendOpt->Get(&b);
        mHost->mWifiNative->SetSuspendOptimizations(mHost->mSuspendOptNeedsDisabled == 0 && b);
    }
    mHost->mWifiNative->SetPowerSave(TRUE);

    if (mHost->mP2pSupported) {
        if (mHost->mOperationalMode == CONNECT_MODE) {
            mHost->mWifiP2pChannel->SendMessage(WifiStateMachine::CMD_ENABLE_P2P);
        }
        else {
            // P2P statemachine starts in disabled state, and is not enabled until
            // CMD_ENABLE_P2P is sent from here; so, nothing needs to be done to
            // keep it disabled.
        }
    }

    AutoPtr<IIntent> intent;
    CIntent::New(IWifiManager::WIFI_SCAN_AVAILABLE, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
    intent->PutExtra(IWifiManager::EXTRA_SCAN_AVAILABLE, IWifiManager::WIFI_STATE_ENABLED);
    mHost->mContext->SendStickyBroadcastAsUser(intent, UserHandle::ALL);

    if (PDBG) {
        mHost->Loge(String("Driverstarted State enter done"));
    }

    return NOERROR;
}

ECode WifiStateMachine::DriverStartedState::ProcessMessage(
    /* [in] */ IMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    mHost->LogStateAndMessage(message, String("WifiStateMachine::DriverStartedState"));

    Int32 what;
    message->GetWhat(&what);
    switch (what) {
        case CMD_START_SCAN:
            mHost->HandleScanRequest(WifiNative::SCAN_WITHOUT_CONNECTION_SETUP, message);
            break;
        case CMD_SET_BATCHED_SCAN: {
            Int32 arg1;
            message->GetArg1(&arg1);
            Int32 arg2;
            message->GetArg2(&arg2);
            AutoPtr<IInterface> obj;
            message->GetObj((IInterface**)&obj);
            if (mHost->RecordBatchedScanSettings(arg1, arg2,
                    IBundle::Probe(obj))) {
                if (mHost->mBatchedScanSettings != NULL) {
                    mHost->StartBatchedScan();
                }
                else {
                    mHost->StopBatchedScan();
                }
            }
            break;
        }
        case CMD_SET_COUNTRY_CODE: {
            AutoPtr<IInterface> obj;
            message->GetObj((IInterface**)&obj);
            String country;
            ICharSequence::Probe(obj)->ToString(&country);
            Int32 arg1;
            message->GetArg1(&arg1);
            Int32 sequence = arg1;
            Int32 arg2;
            message->GetArg2(&arg2);
            Boolean persist = (arg2 == 1);
            Int32 val;
            mHost->mCountryCodeSequence->Get(&val);
            if (sequence != val) {
                if (DBG) mHost->Log(String("set country code ignored due to sequnce num"));
                break;
            }
            if (DBG) mHost->Log(String("set country code ") + country);
            if (persist) {
                mHost->mPersistedCountryCode = country;
                AutoPtr<IContentResolver> cr;
                mHost->mContext->GetContentResolver((IContentResolver**)&cr);
                Boolean b;
                Settings::Global::PutString(cr,
                        ISettingsGlobal::WIFI_COUNTRY_CODE,
                        country, &b);
            }
            country = country.ToUpperCase();
            if (mHost->mLastSetCountryCode == NULL
                    || country.Equals(mHost->mLastSetCountryCode) == FALSE) {
                if (mHost->mWifiNative->SetCountryCode(country)) {
                    mHost->mLastSetCountryCode = country;
                }
                else {
                    mHost->Loge(String("Failed to set country code ") + country);
                }
            }
            mHost->mWifiP2pChannel->SendMessage(WifiP2pServiceImpl::SET_COUNTRY_CODE,
                    CoreUtils::Convert(country));
            break;
        }
        case CMD_SET_FREQUENCY_BAND: {
            Int32 arg1;
            message->GetArg1(&arg1);
            Int32 band =  arg1;
            if (DBG) mHost->Log(String("set frequency band ") + band);
            if (mHost->mWifiNative->SetBand(band)) {

                if (PDBG)  mHost->Loge(String("did set frequency band ") + band);

                mHost->mFrequencyBand->Set(band);
                // Flush old data - like scan results
                mHost->mWifiNative->BssFlush();
                Int32 val;
                mHost->mFrequencyBand->Get(&val);
                if (val == IWifiManager::WIFI_FREQUENCY_BAND_2GHZ) {
                    mHost->mWifiNative->Disable5GHzFrequencies(TRUE);
                    mHost->mDisabled5GhzFrequencies = TRUE;
                }
                else if ((val != IWifiManager::WIFI_FREQUENCY_BAND_2GHZ)
                   && (mHost->mDisabled5GhzFrequencies)) {
                    mHost->mWifiNative->Disable5GHzFrequencies(FALSE);
                    mHost->mDisabled5GhzFrequencies = FALSE;
                }
                // Fetch the latest scan results when frequency band is set
                mHost->StartScanNative(WifiNative::SCAN_WITHOUT_CONNECTION_SETUP, String(NULL));

                if (PDBG) mHost->Loge(String("done set frequency band ") + band);

            }
            else {
                mHost->Loge(String("Failed to set frequency band ") + band);
            }
            break;
        }
        case CMD_BLUETOOTH_ADAPTER_STATE_CHANGE: {
            Int32 arg1;
            message->GetArg1(&arg1);
            mHost->mBluetoothConnectionActive = (arg1 !=
                    IBluetoothAdapter::STATE_DISCONNECTED);
            mHost->mWifiNative->SetBluetoothCoexistenceScanMode(mHost->mBluetoothConnectionActive);
            break;
        }
        case CMD_STOP_DRIVER: {
            Int32 arg1;
            message->GetArg1(&arg1);
            Int32 mode = arg1;

            /* Already doing a delayed stop */
            if (mHost->mInDelayedStop) {
                if (DBG) mHost->Log(String("Already in delayed stop"));
                break;
            }
            /* disconnect right now, but leave the driver running for a bit */
            mHost->mWifiConfigStore->DisableAllNetworks();

            mHost->mInDelayedStop = TRUE;
            mHost->mDelayedStopCounter++;
            if (DBG) mHost->Log(String("Delayed stop message ") + mHost->mDelayedStopCounter);

            /* send regular delayed shut down */
            AutoPtr<IIntent> driverStopIntent;
            CIntent::New(ACTION_DELAYED_DRIVER_STOP, NULL, (IIntent**)&driverStopIntent);
            driverStopIntent->PutExtra(DELAYED_STOP_COUNTER, mHost->mDelayedStopCounter);

            AutoPtr<IPendingIntentHelper> helper;
            CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&helper);
            helper->GetBroadcast(mHost->mContext,
                    DRIVER_STOP_REQUEST, driverStopIntent,
                    IPendingIntent::FLAG_UPDATE_CURRENT,
                    (IPendingIntent**)&mHost->mDriverStopIntent);

            Int64 currentTimeMillis;
            AutoPtr<ISystem> system;
            CSystem::AcquireSingleton((ISystem**)&system);
            system->GetCurrentTimeMillis(&currentTimeMillis);
            mHost->mAlarmManager->Set(IAlarmManager::RTC_WAKEUP, currentTimeMillis
                    + mHost->mDriverStopDelayMs, mHost->mDriverStopIntent);
            break;
        }
        case CMD_START_DRIVER:
            if (mHost->mInDelayedStop) {
                mHost->mInDelayedStop = FALSE;
                mHost->mDelayedStopCounter++;
                mHost->mAlarmManager->Cancel(mHost->mDriverStopIntent);
                if (DBG) mHost->Log(String("Delayed stop ignored due to start"));
                if (mHost->mOperationalMode == CONNECT_MODE) {
                    mHost->mWifiConfigStore->EnableAllNetworks();
                }
            }
            break;
        case CMD_DELAYED_STOP_DRIVER: {
            Int32 arg1;
            message->GetArg1(&arg1);
            if (DBG) mHost->Log(String("delayed stop ") + arg1 + " " + mHost->mDelayedStopCounter);
            if (arg1 != mHost->mDelayedStopCounter) break;
            AutoPtr<IState> state = mHost->GetCurrentState();
            if (state != mHost->mDisconnectedState) {
                mHost->mWifiNative->Disconnect();
                mHost->HandleNetworkDisconnect();
            }
            mHost->mWakeLock->AcquireLock();
            mHost->mWifiNative->StopDriver();
            mHost->mWakeLock->ReleaseLock();
            if (mHost->mP2pSupported) {
                mHost->TransitionTo(mHost->mWaitForP2pDisableState);
            }
            else {
                mHost->TransitionTo(mHost->mDriverStoppingState);
            }
            break;
        }
        case CMD_START_PACKET_FILTERING: {
            Int32 arg1;
            message->GetArg1(&arg1);
            if (arg1 == MULTICAST_V6) {
                mHost->mWifiNative->StartFilteringMulticastV6Packets();
            }
            else if (arg1 == MULTICAST_V4) {
                mHost->mWifiNative->StartFilteringMulticastV4Packets();
            }
            else {
                mHost->Loge(String("Illegal arugments to CMD_START_PACKET_FILTERING"));
            }
            break;
        }
        case CMD_STOP_PACKET_FILTERING: {
            Int32 arg1;
            message->GetArg1(&arg1);
            if (arg1 == MULTICAST_V6) {
                mHost->mWifiNative->StopFilteringMulticastV6Packets();
            }
            else if (arg1 == MULTICAST_V4) {
                mHost->mWifiNative->StopFilteringMulticastV4Packets();
            }
            else {
                mHost->Loge(String("Illegal arugments to CMD_STOP_PACKET_FILTERING"));
            }
            break;
        }
        case CMD_SET_SUSPEND_OPT_ENABLED: {
            Int32 arg1;
            message->GetArg1(&arg1);
            if (arg1 == 1) {
                mHost->SetSuspendOptimizationsNative(SUSPEND_DUE_TO_SCREEN, TRUE);
                mHost->mSuspendWakeLock->ReleaseLock();
            }
            else {
                mHost->SetSuspendOptimizationsNative(SUSPEND_DUE_TO_SCREEN, FALSE);
            }
            break;
        }
        case CMD_SET_HIGH_PERF_MODE: {
            Int32 arg1;
            message->GetArg1(&arg1);
            if (arg1 == 1) {
                mHost->SetSuspendOptimizationsNative(SUSPEND_DUE_TO_HIGH_PERF, FALSE);
            }
            else {
                mHost->SetSuspendOptimizationsNative(SUSPEND_DUE_TO_HIGH_PERF, TRUE);
            }
            break;
        }
        case CMD_ENABLE_TDLS: {
            Int32 arg1;
            message->GetArg1(&arg1);
            AutoPtr<IInterface> obj;
            message->GetObj((IInterface**)&obj);
            if (obj != NULL) {
                String remoteAddress;
                ICharSequence::Probe(obj)->ToString(&remoteAddress);
                Boolean enable = (arg1 == 1);
                mHost->mWifiNative->StartTdls(remoteAddress, enable);
            }
            break;
        }
        case CMD_GET_IBSS_SUPPORTED:
            mHost->ReplyToMessage(message, what, mHost->mIbssSupported ? 1 : 0);
            break;
        default:
            *result = NOT_HANDLED;
            return NOERROR;
    }
    *result = HANDLED;
    return NOERROR;
}

ECode WifiStateMachine::DriverStartedState::Exit()
{
    mHost->mIsRunning = FALSE;
    mHost->UpdateBatteryWorkSource(NULL);
    CArrayList::New((IArrayList**)&mHost->mScanResults);

    mHost->StopBatchedScan();

    AutoPtr<IIntent> intent;
    CIntent::New(IWifiManager::WIFI_SCAN_AVAILABLE, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
    intent->PutExtra(IWifiManager::EXTRA_SCAN_AVAILABLE, IWifiManager::WIFI_STATE_DISABLED);
    mHost->mContext->SendStickyBroadcastAsUser(intent, UserHandle::ALL);
    mHost->NoteScanEnd(); // wrap up any pending request.
    mHost->mBufferedScanMsg->Clear();

    mHost->mLastSetCountryCode = NULL;
    return NOERROR;
}

String WifiStateMachine::DriverStartedState::GetName()
{
    return String("DriverStartedState");
}

//==============================================================================
//  WifiStateMachine::WaitForP2pDisableState
//==============================================================================

ECode WifiStateMachine::WaitForP2pDisableState::Enter()
{
    Int32 what;
    mHost->GetCurrentMessage()->GetWhat(&what);
    switch (what) {
        case WifiMonitor::SUP_DISCONNECTION_EVENT:
            mTransitionToState = mHost->mInitialState;
            break;
        case CMD_DELAYED_STOP_DRIVER:
            mTransitionToState = mHost->mDriverStoppingState;
            break;
        case CMD_STOP_SUPPLICANT:
            mTransitionToState = mHost->mSupplicantStoppingState;
            break;
        default:
            mTransitionToState = mHost->mDriverStoppingState;
            break;
    }
    mHost->mWifiP2pChannel->SendMessage(WifiStateMachine::CMD_DISABLE_P2P_REQ);
    return NOERROR;
}

ECode WifiStateMachine::WaitForP2pDisableState::ProcessMessage(
    /* [in] */ IMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    mHost->LogStateAndMessage(message, String("WifiStateMachine::WaitForP2pDisableState"));

    Int32 what;
    message->GetWhat(&what);
    switch (what) {
        case WifiStateMachine::CMD_DISABLE_P2P_RSP:
            mHost->TransitionTo(mTransitionToState);
            break;
        /* Defer wifi start/shut and driver commands */
        case WifiMonitor::SUPPLICANT_STATE_CHANGE_EVENT:
        case CMD_START_SUPPLICANT:
        case CMD_STOP_SUPPLICANT:
        case CMD_START_AP:
        case CMD_STOP_AP:
        case CMD_START_DRIVER:
        case CMD_STOP_DRIVER:
        case CMD_SET_OPERATIONAL_MODE:
        case CMD_SET_COUNTRY_CODE:
        case CMD_SET_FREQUENCY_BAND:
        case CMD_START_PACKET_FILTERING:
        case CMD_STOP_PACKET_FILTERING:
        case CMD_START_SCAN:
        case CMD_DISCONNECT:
        case CMD_REASSOCIATE:
        case CMD_RECONNECT:
            mHost->messageHandlingStatus = MESSAGE_HANDLING_STATUS_DEFERRED;
            mHost->DeferMessage(message);
            break;
        default:
            *result = NOT_HANDLED;
            return NOERROR;
    }
    *result = HANDLED;
    return NOERROR;
}

String WifiStateMachine::WaitForP2pDisableState::GetName()
{
    return String("WaitForP2pDisableState");
}

//==============================================================================
//  WifiStateMachine::DriverStoppingState
//==============================================================================

ECode WifiStateMachine::DriverStoppingState::ProcessMessage(
    /* [in] */ IMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    mHost->LogStateAndMessage(message, String("WifiStateMachine::DriverStoppingState"));

    Int32 what;
    message->GetWhat(&what);
    switch (what) {
        case WifiMonitor::SUPPLICANT_STATE_CHANGE_EVENT: {
            SupplicantState state = mHost->HandleSupplicantStateChange(message);
            if (state == Elastos::Droid::Wifi::SupplicantState_INTERFACE_DISABLED) {
                mHost->TransitionTo(mHost->mDriverStoppedState);
            }
            break;
        }
            /* Queue driver commands */
        case CMD_START_DRIVER:
        case CMD_STOP_DRIVER:
        case CMD_SET_COUNTRY_CODE:
        case CMD_SET_FREQUENCY_BAND:
        case CMD_START_PACKET_FILTERING:
        case CMD_STOP_PACKET_FILTERING:
        case CMD_START_SCAN:
        case CMD_DISCONNECT:
        case CMD_REASSOCIATE:
        case CMD_RECONNECT:
            mHost->messageHandlingStatus = MESSAGE_HANDLING_STATUS_DEFERRED;
            mHost->DeferMessage(message);
            break;
        default:
            *result = NOT_HANDLED;
            return NOERROR;
    }
    *result = HANDLED;
    return NOERROR;
}

String WifiStateMachine::DriverStoppingState::GetName()
{
    return String("DriverStoppingState");
}

//==============================================================================
//  WifiStateMachine::DriverStoppedState
//==============================================================================

ECode WifiStateMachine::DriverStoppedState::ProcessMessage(
    /* [in] */ IMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    mHost->LogStateAndMessage(message, String("WifiStateMachine::DriverStoppedState"));
    Int32 what;
    message->GetWhat(&what);
    switch (what) {
        case WifiMonitor::SUPPLICANT_STATE_CHANGE_EVENT: {
            AutoPtr<IInterface> obj;
            message->GetObj((IInterface**)&obj);
            AutoPtr<CStateChangeResult> stateChangeResult = (CStateChangeResult*)(IObject*)obj.Get();
            SupplicantState state;
            stateChangeResult->GetSupplicantState(&state);
            // A WEXT bug means that we can be back to driver started state
            // unexpectedly
            AutoPtr<ISupplicantStateHelper> helper;
            CSupplicantStateHelper::AcquireSingleton((ISupplicantStateHelper**)&helper);
            Boolean b;
            helper->IsDriverActive(state, &b);
            if (b) {
                mHost->TransitionTo(mHost->mDriverStartedState);
            }
            break;
        }
        case CMD_START_DRIVER:
            mHost->mWakeLock->AcquireLock();
            mHost->mWifiNative->StartDriver();
            mHost->mWakeLock->ReleaseLock();
            mHost->TransitionTo(mHost->mDriverStartingState);
            break;
        default:
            *result = NOT_HANDLED;
            return NOERROR;
    }
    *result = HANDLED;
    return NOERROR;
}

String WifiStateMachine::DriverStoppedState::GetName()
{
    return String("DriverStoppedState");
}

//==============================================================================
//  WifiStateMachine::ScanModeState
//==============================================================================

ECode WifiStateMachine::ScanModeState::Enter()
{
    mLastOperationMode = mHost->mOperationalMode;
    return NOERROR;
}

ECode WifiStateMachine::ScanModeState::ProcessMessage(
    /* [in] */ IMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    mHost->LogStateAndMessage(message, String("WifiStateMachine::ScanModeState"));

    Int32 what;
    message->GetWhat(&what);
    switch (what) {
        case CMD_SET_OPERATIONAL_MODE:
            Int32 arg1;
            message->GetArg1(&arg1);
            if (arg1 == CONNECT_MODE) {

                if (mLastOperationMode == SCAN_ONLY_WITH_WIFI_OFF_MODE) {
                    mHost->SetWifiState(IWifiManager::WIFI_STATE_ENABLED);
                    // Load and re-enable networks when going back to enabled state
                    // This is essential for networks to show up after restore
                    mHost->mWifiConfigStore->LoadAndEnableAllNetworks();
                    mHost->mWifiP2pChannel->SendMessage(CMD_ENABLE_P2P);
                }
                else {
                    mHost->mWifiConfigStore->EnableAllNetworks();
                }

                mHost->mWifiNative->Reconnect();

                mHost->mOperationalMode = CONNECT_MODE;
                mHost->TransitionTo(mHost->mDisconnectedState);
            }
            else {
                // Nothing to do
                *result = HANDLED;
                return NOERROR;
            }
            break;
        // Handle scan. All the connection related commands are
        // handled only in ConnectModeState
        case CMD_START_SCAN:
            mHost->HandleScanRequest(WifiNative::SCAN_WITHOUT_CONNECTION_SETUP, message);
            break;
        default:
            *result = NOT_HANDLED;
            return NOERROR;
    }
    *result = HANDLED;
    return NOERROR;
}

String WifiStateMachine::ScanModeState::GetName()
{
    return String("ScanModeState");
}

//==============================================================================
//  WifiStateMachine::ConnectModeState
//==============================================================================

ECode WifiStateMachine::ConnectModeState::ProcessMessage(
    /* [in] */ IMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IWifiConfiguration> config;
    Int32 netId;
    Boolean ok;
    Boolean didDisconnect;
    String bssid;
    String ssid;
    AutoPtr<NetworkUpdateResult> networkUpdateResult;
    mHost->LogStateAndMessage(message, String("WifiStateMachine::ConnectModeState"));

    Int32 what;
    message->GetWhat(&what);
    switch (what) {
        case WifiMonitor::ASSOCIATION_REJECTION_EVENT: {
            mHost->didBlackListBSSID = FALSE;
            AutoPtr<IInterface> obj;
            message->GetObj((IInterface**)&obj);
            ICharSequence::Probe(obj)->ToString(&bssid);

            if (bssid.IsNull() || bssid.IsEmpty()) {
                // If BSSID is NULL, use the target roam BSSID
                bssid = mHost->mTargetRoamBSSID;
            }
            if (!bssid.IsNull()) {
                // If we have a BSSID, tell configStore to black list it
                synchronized(mHost->mScanResultCache) {
                    mHost->mWifiConfigStore->HandleBSSIDBlackList
                            (mHost->mLastNetworkId, bssid, FALSE, &mHost->didBlackListBSSID);
                }
            }
            mHost->mSupplicantStateTracker->SendMessage(WifiMonitor::ASSOCIATION_REJECTION_EVENT);
            break;
        }
        case WifiMonitor::AUTHENTICATION_FAILURE_EVENT:
            mHost->mSupplicantStateTracker->SendMessage(WifiMonitor::AUTHENTICATION_FAILURE_EVENT);
            break;
        case WifiMonitor::SSID_TEMP_DISABLED:
        case WifiMonitor::SSID_REENABLED: {
            AutoPtr<IInterface> obj;
            message->GetObj((IInterface**)&obj);
            String substr;
            ICharSequence::Probe(obj)->ToString(&substr);
            String en = what == WifiMonitor::SSID_TEMP_DISABLED ?
                    String("temp-disabled") : String("re-enabled");
            Int32 arg1;
            message->GetArg1(&arg1);
            mHost->Loge(String("ConnectModeState SSID state=") + en + " nid="
                    + StringUtils::ToString(arg1) + " [" + substr + "]");
            synchronized(mHost->mScanResultCache) {
                String str;
                mHost->mWifiInfo->GetBSSID(&str);
                mHost->mWifiConfigStore->HandleSSIDStateChange(arg1, what ==
                        WifiMonitor::SSID_REENABLED, substr, str);
            }
            break;
        }
        case WifiMonitor::SUPPLICANT_STATE_CHANGE_EVENT: {
            SupplicantState state = mHost->HandleSupplicantStateChange(message);
            // A driver/firmware hang can now put the interface in a down state.
            // We detect the interface going down and recover from it
            AutoPtr<ISupplicantStateHelper> helper;
            CSupplicantStateHelper::AcquireSingleton((ISupplicantStateHelper**)&helper);
            Boolean b;
            helper->IsDriverActive(state, &b);
            NetworkInfoState s;
            mHost->mNetworkInfo->GetState(&s);
            if (!b) {
                if (s != Elastos::Droid::Net::NetworkInfoState_DISCONNECTED) {
                    mHost->HandleNetworkDisconnect();
                }
                mHost->Log(String("Detected an interface down, restart driver"));
                mHost->TransitionTo(mHost->mDriverStoppedState);
                mHost->SendMessage(CMD_START_DRIVER);
                break;
            }

            // Supplicant can fail to report a NETWORK_DISCONNECTION_EVENT
            // when authentication times out after a successful connection,
            // we can figure this from the supplicant state. If supplicant
            // state is DISCONNECTED, but the mNetworkInfo says we are not
            // disconnected, we need to handle a disconnection
            if (!mHost->linkDebouncing && state == Elastos::Droid::Wifi::SupplicantState_DISCONNECTED &&
                    s != Elastos::Droid::Net::NetworkInfoState_DISCONNECTED) {
                if (DBG) mHost->Log(String("Missed CTRL-EVENT-DISCONNECTED, disconnect"));
                mHost->HandleNetworkDisconnect();
                mHost->TransitionTo(mHost->mDisconnectedState);
            }
            break;
        }
        case WifiP2pServiceImpl::DISCONNECT_WIFI_REQUEST: {
            Int32 arg1;
            message->GetArg1(&arg1);
            if (arg1 == 1) {
                mHost->mWifiNative->Disconnect();
                mHost->mTemporarilyDisconnectWifi = TRUE;
            }
            else {
                mHost->mWifiNative->Reconnect();
                mHost->mTemporarilyDisconnectWifi = FALSE;
            }
            break;
        }
        case CMD_ADD_OR_UPDATE_NETWORK: {
            AutoPtr<IInterface> obj;
            message->GetObj((IInterface**)&obj);
            config = IWifiConfiguration::Probe(obj);
            Int32 sendingUid;
            message->GetSendingUid(&sendingUid);
            Int32 res;
            mHost->mWifiConfigStore->AddOrUpdateNetwork(config, sendingUid, &res);
            if (res < 0) {
                mHost->messageHandlingStatus = MESSAGE_HANDLING_STATUS_FAIL;
            }
            else {
                AutoPtr<IWifiConfiguration> curConfig;
                mHost->GetCurrentWifiConfiguration((IWifiConfiguration**)&curConfig);
                if (curConfig != NULL && config != NULL) {
                    Int32 curPri, pri, sta;
                    curConfig->GetPriority(&curPri);
                    config->GetPriority(&pri);
                    config->GetStatus(&sta);
                    if (curPri < pri && sta == IWifiConfigurationStatus::ENABLED) {
                        // Interpret this as a connect attempt
                        // Set the last selected configuration so as to allow the system to
                        // stick the last user choice without persisting the choice
                        mHost->mWifiConfigStore->SetLastSelectedConfiguration(res);

                        // Remember time of last connection attempt
                        AutoPtr<ISystem> system;
                        CSystem::AcquireSingleton((ISystem**)&system);
                        system->GetCurrentTimeMillis(&mHost->lastConnectAttempt);

                        Int32 val;
                        mHost->mWifiConnectionStatistics->GetNumWifiManagerJoinAttempt(&val);
                        mHost->mWifiConnectionStatistics->SetNumWifiManagerJoinAttempt(++val);

                        // As a courtesy to the caller, trigger a scan now
                        mHost->StartScan(ADD_OR_UPDATE_SOURCE, 0, NULL, NULL);
                    }
                }
            }
            mHost->ReplyToMessage(message, CMD_ADD_OR_UPDATE_NETWORK, res);
            break;
        }
        case CMD_REMOVE_NETWORK: {
            Int32 arg1;
            message->GetArg1(&arg1);
            mHost->mWifiConfigStore->RemoveNetwork(arg1, &ok);
            if (!ok) {
                mHost->messageHandlingStatus = MESSAGE_HANDLING_STATUS_FAIL;
            }
            mHost->ReplyToMessage(message, what, ok ? SUCCESS : FAILURE);
            break;
        }
        case CMD_ENABLE_NETWORK: {
            Int32 arg1;
            message->GetArg1(&arg1);
            Int32 arg2;
            message->GetArg2(&arg2);
            Boolean others = arg2 == 1;
            // Tell autojoin the user did try to select to that network
            // However, do NOT persist the choice by bumping the priority of the network
            if (others) {
                mHost->mWifiAutoJoinController->UpdateConfigurationHistory(arg1, TRUE, FALSE);
                // Set the last selected configuration so as to allow the system to
                // stick the last user choice without persisting the choice
                mHost->mWifiConfigStore->SetLastSelectedConfiguration(arg1);

                // Remember time of last connection attempt
                AutoPtr<ISystem> system;
                CSystem::AcquireSingleton((ISystem**)&system);
                system->GetCurrentTimeMillis(&mHost->lastConnectAttempt);

                Int32 val;
                mHost->mWifiConnectionStatistics->GetNumWifiManagerJoinAttempt(&val);
                mHost->mWifiConnectionStatistics->SetNumWifiManagerJoinAttempt(++val);
            }
            // Cancel auto roam requests
            mHost->AutoRoamSetBSSID(arg1, String("any"));

            mHost->mWifiConfigStore->EnableNetwork(arg1, arg2 == 1, &ok);
            if (!ok) {
                mHost->messageHandlingStatus = MESSAGE_HANDLING_STATUS_FAIL;
            }
            mHost->ReplyToMessage(message, what, ok ? SUCCESS : FAILURE);
            break;
        }
        case CMD_ENABLE_ALL_NETWORKS: {
            Int64 t = SystemClock::GetElapsedRealtime();
            if (t - mHost->mLastEnableAllNetworksTime > MIN_INTERVAL_ENABLE_ALL_NETWORKS_MS) {
                mHost->mWifiConfigStore->EnableAllNetworks();
                mHost->mLastEnableAllNetworksTime = t;
            }
            break;
        }
        case IWifiManager::DISABLE_NETWORK: {
            Int32 arg1;
            message->GetArg1(&arg1);
            Boolean b;
            mHost->mWifiConfigStore->DisableNetwork(arg1,
                    IWifiConfiguration::DISABLED_BY_WIFI_MANAGER, &b);
            if (b == TRUE) {
                mHost->ReplyToMessage(message, IWifiManager::DISABLE_NETWORK_SUCCEEDED);
            }
            else {
                mHost->messageHandlingStatus = MESSAGE_HANDLING_STATUS_FAIL;
                mHost->ReplyToMessage(message, IWifiManager::DISABLE_NETWORK_FAILED,
                        IWifiManager::ERROR);
            }
            break;
        }
        case CMD_BLACKLIST_NETWORK: {
            AutoPtr<IInterface> obj;
            message->GetObj((IInterface**)&obj);
            String str;
            ICharSequence::Probe(obj)->ToString(&str);
            mHost->mWifiNative->AddToBlacklist(str);
            break;
        }
        case CMD_CLEAR_BLACKLIST:
            mHost->mWifiNative->ClearBlacklist();
            break;
        case CMD_SAVE_CONFIG: {
            mHost->mWifiConfigStore->SaveConfig(&ok);

            if (DBG) mHost->Loge(String("wifistatemachine did save config ") + ok);
            mHost->ReplyToMessage(message, CMD_SAVE_CONFIG, ok ? SUCCESS : FAILURE);

            // Inform the backup manager about a data change
            AutoPtr<IInterface> s = ServiceManager::GetService(IContext::BACKUP_SERVICE);
            AutoPtr<IIBackupManager> ibm = IIBackupManager::Probe(s);
            if (ibm != NULL) {
                // try {
                ibm->DataChanged(String("com.android.providers.settings"));
                // } catch (Exception e) {
                //     // Try again later
                // }
            }
            break;
        }
        case CMD_GET_CONFIGURED_NETWORKS: {
            AutoPtr<IList> list;
            mHost->mWifiConfigStore->GetConfiguredNetworks((IList**)&list);
            mHost->ReplyToMessage(message, what, list);
            break;
        }
        case WifiMonitor::SUP_REQUEST_IDENTITY: {
            // Supplicant lacks credentials to connect to that network, hence black list
            AutoPtr<IInterface> obj;
            message->GetObj((IInterface**)&obj);
            ICharSequence::Probe(obj)->ToString(&ssid);

            if (mHost->targetWificonfiguration != NULL && ssid != NULL) {
                String s;
                mHost->targetWificonfiguration->GetSSID(&s);
                if (!s.IsNull() && s.Equals(String("\"") + ssid + "\"")) {
                    Int32 networkId;
                    mHost->targetWificonfiguration->GetNetworkId(&networkId);
                    mHost->mWifiConfigStore->HandleSSIDStateChange(networkId,
                            FALSE, String("AUTH_FAILED no identity"), String(NULL));
                }
            }
            // Disconnect now, as we don't have any way to fullfill the  supplicant request.
            mHost->mWifiConfigStore->SetLastSelectedConfiguration
                    (IWifiConfiguration::INVALID_NETWORK_ID);
            mHost->mWifiNative->Disconnect();
            break;
        }
        case WifiMonitor::SUP_REQUEST_SIM_AUTH: {
            mHost->Logd(String("Received SUP_REQUEST_SIM_AUTH"));
            AutoPtr<IInterface> obj;
            message->GetObj((IInterface**)&obj);
            AutoPtr<SimAuthRequestData> requestData = (SimAuthRequestData*)(IObject*)obj.Get();
            if (requestData != NULL) {
                if (requestData->protocol == IWifiEnterpriseConfigEap::SIM) {
                    mHost->HandleGsmAuthRequest(requestData);
                }
                else if (requestData->protocol == IWifiEnterpriseConfigEap::AKA) {
                    mHost->Handle3GAuthRequest(requestData);
                }
            }
            else {
                mHost->Loge(String("Invalid sim auth request"));
            }
            break;
        }
        case CMD_GET_PRIVILEGED_CONFIGURED_NETWORKS: {
            AutoPtr<IList> list;
            mHost->mWifiConfigStore->GetPrivilegedConfiguredNetworks((IList**)&list);
            mHost->ReplyToMessage(message, what, list);
            break;
        }
            /* Do a redundant disconnect without transition */
        case CMD_DISCONNECT:
            mHost->mWifiConfigStore->SetLastSelectedConfiguration
                    (IWifiConfiguration::INVALID_NETWORK_ID);
            mHost->mWifiNative->Disconnect();
            break;
        case CMD_RECONNECT: {
            AutoPtr<ISystem> system;
            CSystem::AcquireSingleton((ISystem**)&system);
            system->GetCurrentTimeMillis(&mHost->lastConnectAttempt);
            mHost->mWifiNative->Reconnect();
            break;
        }
        case CMD_REASSOCIATE: {
            AutoPtr<ISystem> system;
            CSystem::AcquireSingleton((ISystem**)&system);
            system->GetCurrentTimeMillis(&mHost->lastConnectAttempt);
            mHost->mWifiNative->Reassociate();
            break;
        }
        case CMD_RELOAD_TLS_AND_RECONNECT: {
            Boolean b;
            if (mHost->mWifiConfigStore->NeedsUnlockedKeyStore(&b), b) {
                mHost->Logd(String("Reconnecting to give a chance to un-connected TLS networks"));
                mHost->mWifiNative->Disconnect();
                AutoPtr<ISystem> system;
                CSystem::AcquireSingleton((ISystem**)&system);
                system->GetCurrentTimeMillis(&mHost->lastConnectAttempt);
                mHost->mWifiNative->Reconnect();
            }
            break;
        }
        case CMD_AUTO_ROAM:
            mHost->messageHandlingStatus = MESSAGE_HANDLING_STATUS_DISCARD;
            *result = HANDLED;
            return NOERROR;
        case CMD_AUTO_CONNECT: {
            /* Work Around: wpa_supplicant can get in a bad state where it returns a non
             * associated status to the STATUS command but somehow-someplace still thinks
             * it is associated and thus will ignore select/reconnect command with
             * following message:
             * "Already associated with the selected network - do nothing"
             *
             * Hence, sends a disconnect to supplicant first.
             */
            didDisconnect = FALSE;
            if (mHost->GetCurrentState() != mHost->mDisconnectedState) {
                /** Supplicant will ignore the reconnect if we are currently associated,
                 * hence trigger a disconnect
                 */
                didDisconnect = TRUE;
                mHost->mWifiNative->Disconnect();
            }

            /* connect command coming from auto-join */
            AutoPtr<IInterface> obj;
            message->GetObj((IInterface**)&obj);
            config = IWifiConfiguration::Probe(obj);
            Int32 arg1;
            message->GetArg1(&arg1);
            netId = arg1;
            Int32 arg2;
            message->GetArg2(&arg2);
            Int32 roam = arg2;
            String name;
            mHost->GetCurrentState()->GetName(&name);
            mHost->Loge(String("CMD_AUTO_CONNECT sup state ")
                    + mHost->mSupplicantStateTracker->GetSupplicantStateName()
                    + " my state " + name
                    + " nid=" + StringUtils::ToString(netId)
                    + " roam=" + StringUtils::ToString(roam));
            if (config == NULL) {
                mHost->Loge(String("AUTO_CONNECT and no config, bail out..."));
                break;
            }

            /* Make sure we cancel any previous roam request */
            String bssid;
            config->GetBSSID(&bssid);
            mHost->AutoRoamSetBSSID(netId, bssid);

            /* Save the network config */
            String ss;
            config->GetSSID(&ss);
            mHost->Loge(String("CMD_AUTO_CONNECT will save config -> ") + ss
                    + " nid=" + StringUtils::ToString(netId));
            mHost->mWifiConfigStore->SaveNetwork(config, -1, (NetworkUpdateResult**)&networkUpdateResult);
            netId = networkUpdateResult->GetNetworkId();
            mHost->Loge(String("CMD_AUTO_CONNECT did save config -> ")
                    + " nid=" + StringUtils::ToString(netId));

            // Make sure the network is enabled, since supplicant will not reenable it
            Boolean b;
            mHost->mWifiConfigStore->EnableNetworkWithoutBroadcast(netId, FALSE, &b);

            mHost->mWifiConfigStore->SelectNetwork(netId, &b);
            if (b && mHost->mWifiNative->Reconnect()) {
                AutoPtr<ISystem> system;
                CSystem::AcquireSingleton((ISystem**)&system);
                system->GetCurrentTimeMillis(&mHost->lastConnectAttempt);
                mHost->mWifiConfigStore->GetWifiConfiguration(netId,
                        (IWifiConfiguration**)&mHost->targetWificonfiguration);
                // We selected a better config,
                // maybe because we could not see the last user
                // selection, then forget it. We will remember the selection
                // only if it was persisted.
                mHost->mWifiConfigStore->SetLastSelectedConfiguration(IWifiConfiguration::INVALID_NETWORK_ID);

                mHost->mAutoRoaming = roam;
                Boolean b;
                mHost->IsRoaming(&b);
                if (b || mHost->linkDebouncing) {
                    mHost->TransitionTo(mHost->mRoamingState);
                }
                else if (didDisconnect) {
                    mHost->TransitionTo(mHost->mDisconnectingState);
                }
                else {
                    mHost->TransitionTo(mHost->mDisconnectedState);
                }
            }
            else {
                String str;
                IObject::Probe(config)->ToString(&str);
                mHost->Loge(String("Failed to connect config: ") + str + " netId: " + netId);
                mHost->ReplyToMessage(message, IWifiManager::CONNECT_NETWORK_FAILED,
                        IWifiManager::ERROR);
                break;
            }
            break;
        }
        case IWifiManager::CONNECT_NETWORK: {
            /**
             *  The connect message can contain a network id passed as arg1 on message or
             * or a config passed as obj on message.
             * For a new network, a config is passed to create and connect.
             * For an existing network, a network id is passed
             */
            Int32 arg1;
            message->GetArg1(&arg1);
            netId = arg1;
            AutoPtr<IInterface> obj;
            message->GetObj((IInterface**)&obj);
            config = IWifiConfiguration::Probe(obj);
            Int32 val;
            mHost->mWifiConnectionStatistics->GetNumWifiManagerJoinAttempt(&val);
            mHost->mWifiConnectionStatistics->SetNumWifiManagerJoinAttempt(++val);

            /* Save the network config */
            Int32 sendingUid;
            message->GetSendingUid(&sendingUid);
            if (config != NULL) {
                mHost->mWifiConfigStore->SaveNetwork(config, sendingUid,
                        (NetworkUpdateResult**)&networkUpdateResult);
                netId = networkUpdateResult->GetNetworkId();
            }
            mHost->mWifiConfigStore->GetWifiConfiguration(netId, (IWifiConfiguration**)&config);

            String name;
            mHost->GetCurrentState()->GetName(&name);
            if (config == NULL) {
                mHost->Loge(String("CONNECT_NETWORK id=") + StringUtils::ToString(netId) + " "
                        + mHost->mSupplicantStateTracker->GetSupplicantStateName() + " my state "
                        + name);
            }
            else {
                Boolean autoJoinBailedDueToLowRssi;
                config->GetAutoJoinBailedDueToLowRssi(&autoJoinBailedDueToLowRssi);
                String wasSkipped = autoJoinBailedDueToLowRssi ? String(" skipped") : String("");
                String SSID;
                config->GetSSID(&SSID);
                Int32 networkId;
                config->GetNetworkId(&networkId);
                mHost->Loge(String("CONNECT_NETWORK id=") + StringUtils::ToString(netId)
                        + " config=" + SSID
                        + " cnid=" + networkId
                        + " supstate=" + mHost->mSupplicantStateTracker->GetSupplicantStateName()
                        + " my state " + name
                        + " uid = " + sendingUid
                        + wasSkipped);
            }

            mHost->AutoRoamSetBSSID(netId, String("any"));

            if (sendingUid == IProcess::WIFI_UID
                || sendingUid == IProcess::SYSTEM_UID) {
                // As a sanity measure, clear the BSSID in the supplicant network block.
                // If system or Wifi Settings want to connect, they will not
                // specify the BSSID.
                // If an app however had added a BSSID to this configuration, and the BSSID
                // was wrong, Then we would forever fail to connect until that BSSID
                // is cleaned up.
                mHost->ClearConfigBSSID(config, String("CONNECT_NETWORK"));
            }

            mHost->mAutoRoaming = WifiAutoJoinController::AUTO_JOIN_IDLE;

            /* Tell autojoin the user did try to connect to that network */
            mHost->mWifiAutoJoinController->UpdateConfigurationHistory(netId, TRUE, TRUE);

            mHost->mWifiConfigStore->SetLastSelectedConfiguration(netId);

            didDisconnect = FALSE;
            if (mHost->mLastNetworkId != IWifiConfiguration::INVALID_NETWORK_ID
                    && mHost->mLastNetworkId != netId) {
                /** Supplicant will ignore the reconnect if we are currently associated,
                 * hence trigger a disconnect
                 */
                didDisconnect = TRUE;
                mHost->mWifiNative->Disconnect();
            }

            // Make sure the network is enabled, since supplicant will not reenable it
            Boolean b;
            mHost->mWifiConfigStore->EnableNetworkWithoutBroadcast(netId, FALSE, &b);

            mHost->mWifiConfigStore->SelectNetwork(netId, &b);
            if (b && mHost->mWifiNative->Reconnect()) {
                AutoPtr<ISystem> system;
                CSystem::AcquireSingleton((ISystem**)&system);
                system->GetCurrentTimeMillis(&mHost->lastConnectAttempt);
                mHost->mWifiConfigStore->GetWifiConfiguration(netId, (IWifiConfiguration**)&mHost->targetWificonfiguration);

                /* The state tracker handles enabling networks upon completion/failure */
                mHost->mSupplicantStateTracker->SendMessage(IWifiManager::CONNECT_NETWORK);
                mHost->ReplyToMessage(message, IWifiManager::CONNECT_NETWORK_SUCCEEDED);
                if (didDisconnect) {
                    /* Expect a disconnection from the old connection */
                    mHost->TransitionTo(mHost->mDisconnectingState);
                }
                else {
                    /**
                     *  Directly go to disconnected state where we
                     * process the connection events from supplicant
                     **/
                    mHost->TransitionTo(mHost->mDisconnectedState);
                }
            }
            else {
                String str;
                IObject::Probe(config)->ToString(&str);
                mHost->Loge(String("Failed to connect config: ") + str + " netId: " + netId);
                mHost->ReplyToMessage(message, IWifiManager::CONNECT_NETWORK_FAILED,
                        IWifiManager::ERROR);
                break;
            }
            break;
        }
        case IWifiManager::SAVE_NETWORK: {
            Int32 val;
            mHost->mWifiConnectionStatistics->GetNumWifiManagerJoinAttempt(&val);
            mHost->mWifiConnectionStatistics->SetNumWifiManagerJoinAttempt(++val);
            // Fall thru
        }
        case WifiStateMachine::CMD_AUTO_SAVE_NETWORK: {
            mHost->lastSavedConfigurationAttempt = NULL; // Used for debug
            AutoPtr<IInterface> obj;
            message->GetObj((IInterface**)&obj);
            config = IWifiConfiguration::Probe(obj);
            String name;
            mHost->GetCurrentState()->GetName(&name);
            if (config == NULL) {
                mHost->Loge(String("ERROR: SAVE_NETWORK with NULL configuration")
                        + mHost->mSupplicantStateTracker->GetSupplicantStateName()
                        + " my state " + name);
                mHost->messageHandlingStatus = MESSAGE_HANDLING_STATUS_FAIL;
                mHost->ReplyToMessage(message, IWifiManager::SAVE_NETWORK_FAILED,
                        IWifiManager::ERROR);
                break;
            }
            CWifiConfiguration::New(config, (IWifiConfiguration**)&mHost->lastSavedConfigurationAttempt);
            Int32 nid;
            config->GetNetworkId(&nid);
            String ssid;
            config->GetSSID(&ssid);
            mHost->Loge(String("SAVE_NETWORK id=") + StringUtils::ToString(nid)
                        + " config=" + ssid
                        + " nid=" + nid
                        + " supstate=" + mHost->mSupplicantStateTracker->GetSupplicantStateName()
                        + " my state " + name);

            mHost->mWifiConfigStore->SaveNetwork(config, -1, (NetworkUpdateResult**)&networkUpdateResult);
            if (networkUpdateResult->GetNetworkId() != IWifiConfiguration::INVALID_NETWORK_ID) {
                Int32 networkid;
                mHost->mWifiInfo->GetNetworkId(&networkid);
                if (networkid == networkUpdateResult->GetNetworkId()) {
                    if (networkUpdateResult->HasIpChanged()) {
                        // The currently connection configuration was changed
                        // We switched from DHCP to static or from static to DHCP, or the
                        // static IP address has changed.
                        mHost->Log(String("Reconfiguring IP on connection"));
                        // TODO: clear addresses and disable IPv6
                        // to simplify obtainingIpState.
                        mHost->TransitionTo(mHost->mObtainingIpState);
                    }
                    if (networkUpdateResult->HasProxyChanged()) {
                        mHost->Log(String("Reconfiguring proxy on connection"));
                        AutoPtr<ILinkProperties> lp;
                        mHost->mNetlinkTracker->GetLinkProperties((ILinkProperties**)&lp);
                        mHost->UpdateLinkProperties(CMD_UPDATE_LINKPROPERTIES, lp);
                    }
                }
                mHost->ReplyToMessage(message, IWifiManager::SAVE_NETWORK_SUCCEEDED);
                if (VDBG) {
                   mHost->Loge(String("Success save network nid=")
                                + StringUtils::ToString(networkUpdateResult->GetNetworkId()));
                }

                synchronized(mHost->mScanResultCache) {
                    /**
                     * If the command comes from WifiManager, then
                     * tell autojoin the user did try to modify and save that network,
                     * and interpret the SAVE_NETWORK as a request to connect
                     */
                    Boolean user = what == IWifiManager::SAVE_NETWORK;
                    mHost->mWifiAutoJoinController->UpdateConfigurationHistory(networkUpdateResult->GetNetworkId()
                            , user, TRUE);
                    mHost->mWifiAutoJoinController->AttemptAutoJoin();
                }
            }
            else {
                mHost->Loge(String("Failed to save network"));
                mHost->messageHandlingStatus = MESSAGE_HANDLING_STATUS_FAIL;
                mHost->ReplyToMessage(message, IWifiManager::SAVE_NETWORK_FAILED,
                        IWifiManager::ERROR);
            }
            break;
        }
        case IWifiManager::FORGET_NETWORK: {
            // Debug only, remember last configuration that was forgotten
            AutoPtr<IWifiConfiguration> toRemove;
            Int32 arg1;
            message->GetArg1(&arg1);
            mHost->mWifiConfigStore->GetWifiConfiguration(arg1, (IWifiConfiguration**)&toRemove);
            if (toRemove == NULL) {
                mHost->lastForgetConfigurationAttempt = NULL;
            }
            else {
                CWifiConfiguration::New(toRemove, (IWifiConfiguration**)&mHost->lastForgetConfigurationAttempt);
            }
            Boolean b;
            if (mHost->mWifiConfigStore->ForgetNetwork(arg1, &b), b) {
                mHost->ReplyToMessage(message, IWifiManager::FORGET_NETWORK_SUCCEEDED);
            }
            else {
                mHost->Loge(String("Failed to forget network"));
                mHost->ReplyToMessage(message, IWifiManager::FORGET_NETWORK_FAILED,
                        IWifiManager::ERROR);
            }
            break;
        }
        case IWifiManager::START_WPS: {
            AutoPtr<IInterface> obj;
            message->GetObj((IInterface**)&obj);
            AutoPtr<IWpsInfo> wpsInfo = IWpsInfo::Probe(obj);
            AutoPtr<IWpsResult> wpsResult;
            Int32 setup;
            wpsInfo->GetSetup(&setup);
            switch (setup) {
                case IWpsInfo::PBC:
                    mHost->mWifiConfigStore->StartWpsPbc(wpsInfo, (IWpsResult**)&wpsResult);
                    break;
                case IWpsInfo::KEYPAD:
                    mHost->mWifiConfigStore->StartWpsWithPinFromAccessPoint(wpsInfo, (IWpsResult**)&wpsResult);
                    break;
                case IWpsInfo::DISPLAY:
                    mHost->mWifiConfigStore->StartWpsWithPinFromDevice(wpsInfo, (IWpsResult**)&wpsResult);
                    break;
                default:
                    CWpsResult::New(Elastos::Droid::Wifi::WpsResultStatus_FAILURE, (IWpsResult**)&wpsResult);
                    mHost->Loge(String("Invalid setup for WPS"));
                    break;
            }
            mHost->mWifiConfigStore->SetLastSelectedConfiguration
                    (IWifiConfiguration::INVALID_NETWORK_ID);
            WpsResultStatus status;
            wpsResult->GetStatus(&status);
            if (status == Elastos::Droid::Wifi::WpsResultStatus_SUCCESS) {
                mHost->ReplyToMessage(message, IWifiManager::START_WPS_SUCCEEDED, wpsResult);
                mHost->TransitionTo(mHost->mWpsRunningState);
            }
            else {
                mHost->Loge(String("Failed to start WPS with config "));
                mHost->ReplyToMessage(message, IWifiManager::WPS_FAILED, IWifiManager::ERROR);
            }
            break;
        }
        case WifiMonitor::NETWORK_CONNECTION_EVENT: {
            if (DBG) mHost->Log(String("Network connection established"));
            Int32 arg1;
            message->GetArg1(&arg1);
            mHost->mLastNetworkId = arg1;
            AutoPtr<IInterface> obj;
            message->GetObj((IInterface**)&obj);
            ICharSequence::Probe(obj)->ToString(&mHost->mLastBssid);

            mHost->mWifiInfo->SetBSSID(mHost->mLastBssid);
            mHost->mWifiInfo->SetNetworkId(mHost->mLastNetworkId);
            // Send event to CM & network change broadcast
            mHost->SetNetworkDetailedState(
                    Elastos::Droid::Net::NetworkInfoDetailedState_OBTAINING_IPADDR);
            mHost->SendNetworkStateChangeBroadcast(mHost->mLastBssid);
            mHost->TransitionTo(mHost->mObtainingIpState);
            break;
        }
        case WifiMonitor::NETWORK_DISCONNECTION_EVENT:
            // Calling mHost->HandleNetworkDisconnect here is redundant because we might already
            // have called it when leaving L2ConnectedState to go to disconnecting state
            // or thru other path
            // We should normally check the mWifiInfo or mHost->mLastNetworkId so as to check
            // if they are valid, and only in this case call mHost->HandleNEtworkDisconnect,
            // TODO: this should be fixed for a L MR release
            // The side effect of calling mHost->HandleNetworkDisconnect twice is that a bunch of
            // idempotent commands are executed twice (stopping Dhcp, enabling the SPS mode
            // at the chip etc...
            if (DBG) mHost->Log(String("ConnectModeState: Network connection lost "));
            mHost->HandleNetworkDisconnect();
            mHost->TransitionTo(mHost->mDisconnectedState);
            break;
        default:
            *result = NOT_HANDLED;
            return NOERROR;
    }
    *result = HANDLED;
    return NOERROR;
}

String WifiStateMachine::ConnectModeState::GetName()
{
    return String("ConnectModeState");
}

//==============================================================================
//  WifiStateMachine::L2ConnectedState
//==============================================================================

ECode WifiStateMachine::L2ConnectedState::Enter()
{
    mHost->mRssiPollToken++;
    if (mHost->mEnableRssiPolling) {
        mHost->SendMessage(CMD_RSSI_POLL, mHost->mRssiPollToken, 0);
    }
    if (mHost->mNetworkAgent != NULL) {
        mHost->Loge(String("Have NetworkAgent when entering L2Connected"));
        mHost->SetNetworkDetailedState(Elastos::Droid::Net::NetworkInfoDetailedState_DISCONNECTED);
    }
    mHost->SetNetworkDetailedState(Elastos::Droid::Net::NetworkInfoDetailedState_CONNECTING);

    if (mHost->mTcpBufferSizes.IsEmpty() == FALSE) {
        mHost->mLinkProperties->SetTcpBufferSizes(mHost->mTcpBufferSizes);
    }
    AutoPtr<IHandler> handler;
    mHost->GetHandler((IHandler**)&handler);
    AutoPtr<ILooper> looper;
    handler->GetLooper((ILooper**)&looper);
    mHost->mNetworkAgent = new WifiNetworkAgent(looper, mHost->mContext,
            String("WifiNetworkAgent"), mHost->mNetworkInfo, mHost->mNetworkCapabilitiesFilter,
            mHost->mLinkProperties, DEFAULT_SCORE, mHost);

    // We must clear the config BSSID, as the wifi chipset may decide to roam
    // from this point on and having the BSSID specified in the network block would
    // cause the roam to faile and the device to disconnect
    mHost->ClearCurrentConfigBSSID(String("L2ConnectedState"));
    return NOERROR;
}

ECode WifiStateMachine::L2ConnectedState::ProcessMessage(
    /* [in] */ IMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    mHost->LogStateAndMessage(message, String("WifiStateMachine::L2ConnectedState"));

    Int32 what;
    message->GetWhat(&what);
    switch (what) {
        case IDhcpStateMachine::CMD_PRE_DHCP_ACTION:
            mHost->HandlePreDhcpSetup();
            break;
        case IDhcpStateMachine::CMD_POST_DHCP_ACTION: {
            mHost->HandlePostDhcpSetup();
            Int32 arg1;
            message->GetArg1(&arg1);
            if (arg1 == IDhcpStateMachine::DHCP_SUCCESS) {
                if (DBG) mHost->Log(String("WifiStateMachine DHCP successful"));
                AutoPtr<IInterface> obj;
                message->GetObj((IInterface**)&obj);
                mHost->HandleIPv4Success(IDhcpResults::Probe(obj), IDhcpStateMachine::DHCP_SUCCESS);
                // We advance to mVerifyingLinkState because handleIPv4Success will call
                // updateLinkProperties, which then sends CMD_IP_CONFIGURATION_SUCCESSFUL.
            }
            else if (arg1 == IDhcpStateMachine::DHCP_FAILURE) {
                if (DBG) {
                    Int32 count = -1;
                    AutoPtr<IWifiConfiguration> config;
                    mHost->GetCurrentWifiConfiguration((IWifiConfiguration**)&config);
                    if (config != NULL) {
                        config->GetNumConnectionFailures(&count);
                    }
                    mHost->Log(String("WifiStateMachine DHCP failure count=") + count);
                }
                mHost->HandleIPv4Failure(IDhcpStateMachine::DHCP_FAILURE);
                // As above, we transition to mHost->mDisconnectingState via updateLinkProperties.
            }
            break;
        }
        case CMD_IP_CONFIGURATION_SUCCESSFUL:
            mHost->HandleSuccessfulIpConfiguration();
            mHost->SendConnectedState();
            mHost->TransitionTo(mHost->mConnectedState);
            break;
        case CMD_IP_CONFIGURATION_LOST:
            mHost->HandleIpConfigurationLost();
            mHost->TransitionTo(mHost->mDisconnectingState);
            break;
        case CMD_DISCONNECT:
            mHost->mWifiNative->Disconnect();
            mHost->TransitionTo(mHost->mDisconnectingState);
            break;
        case WifiP2pServiceImpl::DISCONNECT_WIFI_REQUEST: {
            Int32 arg1;
            message->GetArg1(&arg1);
            if (arg1 == 1) {
                mHost->mWifiNative->Disconnect();
                mHost->mTemporarilyDisconnectWifi = TRUE;
                mHost->TransitionTo(mHost->mDisconnectingState);
            }
            break;
        }
        case CMD_SET_OPERATIONAL_MODE: {
            Int32 arg1;
            message->GetArg1(&arg1);
            if (arg1 != CONNECT_MODE) {
                mHost->SendMessage(CMD_DISCONNECT);
                mHost->DeferMessage(message);
                if (arg1 == SCAN_ONLY_WITH_WIFI_OFF_MODE) {
                    mHost->NoteWifiDisabledWhileAssociated();
                }
            }
            break;
        }
        case CMD_SET_COUNTRY_CODE:
            mHost->messageHandlingStatus = MESSAGE_HANDLING_STATUS_DEFERRED;
            mHost->DeferMessage(message);
            break;
        case CMD_START_SCAN: {
            Int32 arg1;
            message->GetArg1(&arg1);
            if (!mHost->IsScanAllowed(arg1)) {
                // Ignore the scan request
                if (VDBG) mHost->Logd(String("L2ConnectedState: ignore scan"));
                *result = HANDLED;
                return NOERROR;
            }
            if (DBG) {
                // mHost->Loge(String("WifiStateMachine CMD_START_SCAN source ") + message.arg1
                //       + " txSuccessRate="+String.format( "%.2f", mWifiInfo.txSuccessRate)
                //       + " rxSuccessRate="+String.format( "%.2f", mWifiInfo.rxSuccessRate)
                //       + " targetRoamBSSID=" + mTargetRoamBSSID
                //       + " RSSI=" + mWifiInfo.getRssi());
            }
            if (arg1 == SCAN_ALARM_SOURCE) {
                Boolean tryFullBandScan = FALSE;
                Boolean restrictChannelList = FALSE;
                AutoPtr<ISystem> system;
                CSystem::AcquireSingleton((ISystem**)&system);
                Int64 now_ms;
                system->GetCurrentTimeMillis(&now_ms);
                if (DBG) {
                    mHost->Loge(String("WifiStateMachine CMD_START_SCAN with age=")
                            + StringUtils::ToString(now_ms - mHost->lastFullBandConnectedTimeMilli)
                            + " interval=" + mHost->fullBandConnectedTimeIntervalMilli
                            + " maxinterval=" + mHost->maxFullBandConnectedTimeIntervalMilli);
                }
                if (mHost->mWifiInfo != NULL) {
                    if (mHost->mWifiConfigStore->enableFullBandScanWhenAssociated &&
                            (now_ms - mHost->lastFullBandConnectedTimeMilli)
                            > mHost->fullBandConnectedTimeIntervalMilli) {
                        if (DBG) {
                            mHost->Loge(String("WifiStateMachine CMD_START_SCAN try full band scan age=")
                                 + StringUtils::ToString(now_ms - mHost->lastFullBandConnectedTimeMilli)
                                 + " interval=" + mHost->fullBandConnectedTimeIntervalMilli
                                 + " maxinterval=" + mHost->maxFullBandConnectedTimeIntervalMilli);
                        }
                        tryFullBandScan = TRUE;
                    }

                    Double txSuccessRate;
                    mHost->mWifiInfo->GetTxSuccessRate(&txSuccessRate);
                    Double rxSuccessRate;
                    mHost->mWifiInfo->GetRxSuccessRate(&rxSuccessRate);
                    if (txSuccessRate >
                            mHost->mWifiConfigStore->maxTxPacketForFullScans
                            || rxSuccessRate >
                            mHost->mWifiConfigStore->maxRxPacketForFullScans) {
                        // Too much traffic at the interface, hence no full band scan
                        if (DBG) {
                            mHost->Loge(String("WifiStateMachine CMD_START_SCAN ") +
                                    "prevent full band scan due to pkt rate");
                        }
                        tryFullBandScan = FALSE;
                    }

                    if (txSuccessRate >
                            mHost->mWifiConfigStore->maxTxPacketForPartialScans
                            || rxSuccessRate >
                            mHost->mWifiConfigStore->maxRxPacketForPartialScans) {
                        // Don't scan if lots of packets are being sent
                        restrictChannelList = TRUE;
                        if (mHost->mWifiConfigStore->alwaysEnableScansWhileAssociated == 0) {
                            if (DBG) {
                             // mHost->Loge(String("WifiStateMachine CMD_START_SCAN source ") + message.arg1
                             //    + " ...and ignore scans"
                             //    + " tx=" + String.format("%.2f", mHost->mWifiInfo.txSuccessRate)
                             //    + " rx=" + String.format("%.2f", mHost->mWifiInfo.rxSuccessRate));
                            }
                            mHost->messageHandlingStatus = MESSAGE_HANDLING_STATUS_REFUSED;
                            *result = HANDLED;
                            return NOERROR;
                        }
                    }
                }

                AutoPtr<IWifiConfiguration> currentConfiguration;
                mHost->GetCurrentWifiConfiguration((IWifiConfiguration**)&currentConfiguration);
                if (DBG) {
                    mHost->Loge(String("WifiStateMachine CMD_START_SCAN full=") +
                            tryFullBandScan);
                }
                if (currentConfiguration != NULL) {
                    if (mHost->fullBandConnectedTimeIntervalMilli
                            < mHost->mWifiConfigStore->associatedPartialScanPeriodMilli) {
                        // Sanity
                        mHost->fullBandConnectedTimeIntervalMilli
                                = mHost->mWifiConfigStore->associatedPartialScanPeriodMilli;
                    }
                    if (tryFullBandScan) {
                        mHost->lastFullBandConnectedTimeMilli = now_ms;
                        if (mHost->fullBandConnectedTimeIntervalMilli
                                < mHost->mWifiConfigStore->associatedFullScanMaxIntervalMilli) {
                            // Increase the interval
                            mHost->fullBandConnectedTimeIntervalMilli
                                    = mHost->fullBandConnectedTimeIntervalMilli
                                    * mHost->mWifiConfigStore->associatedFullScanBackoff / 8;

                            if (DBG) {
                                mHost->Loge(String("WifiStateMachine CMD_START_SCAN bump interval =")
                                + mHost->fullBandConnectedTimeIntervalMilli);
                            }
                        }
                        mHost->HandleScanRequest(
                                WifiNative::SCAN_WITHOUT_CONNECTION_SETUP, message);
                    }
                    else {
                        Boolean b;
                        mHost->StartScanForConfiguration(
                                currentConfiguration, restrictChannelList, &b);
                        if (!b) {
                            if (DBG) {
                                mHost->Loge(String("WifiStateMachine starting scan, ") +
                                        " did not find channels -> full");
                            }
                            mHost->lastFullBandConnectedTimeMilli = now_ms;
                            if (mHost->fullBandConnectedTimeIntervalMilli
                                    < mHost->mWifiConfigStore->associatedFullScanMaxIntervalMilli) {
                                // Increase the interval
                                mHost->fullBandConnectedTimeIntervalMilli
                                        = mHost->fullBandConnectedTimeIntervalMilli
                                        * mHost->mWifiConfigStore->associatedFullScanBackoff / 8;

                                if (DBG) {
                                    mHost->Loge(String("WifiStateMachine CMD_START_SCAN bump interval =")
                                            + mHost->fullBandConnectedTimeIntervalMilli);
                                }
                            }
                            mHost->HandleScanRequest(
                                        WifiNative::SCAN_WITHOUT_CONNECTION_SETUP, message);
                        }
                    }
                }
                else {
                    mHost->Loge(String("CMD_START_SCAN : connected mode and no configuration"));
                    mHost->messageHandlingStatus = MESSAGE_HANDLING_STATUS_HANDLING_ERROR;
                }
            }
            else {
                // Not scan alarm source
                mHost->HandleScanRequest(WifiNative::SCAN_WITHOUT_CONNECTION_SETUP, message);
            }
            break;
        }
            /* Ignore connection to same network */
        case IWifiManager::CONNECT_NETWORK: {
            Int32 arg1;
            message->GetArg1(&arg1);
            Int32 netId = arg1;
            Int32 nid;
            mHost->mWifiInfo->GetNetworkId(&nid);
            if (nid == netId) {
                break;
            }
            *result = NOT_HANDLED;
            return NOERROR;
        }
            /* Ignore */
        case WifiMonitor::NETWORK_CONNECTION_EVENT:
            break;
        case CMD_RSSI_POLL: {
            Int32 arg1;
            message->GetArg1(&arg1);
            if (arg1 == mHost->mRssiPollToken) {
                if (mHost->mWifiConfigStore->enableChipWakeUpWhenAssociated) {
                    if (VVDBG) mHost->Log(String(" get link layer stats ") + mHost->mWifiLinkLayerStatsSupported);
                    AutoPtr<IWifiLinkLayerStats> stats;
                    mHost->GetWifiLinkLayerStats(VDBG, (IWifiLinkLayerStats**)&stats);
                    if (stats != NULL) {
                        // Sanity check the results provided by driver
                        Int32 rssi;
                        mHost->mWifiInfo->GetRssi(&rssi);
                        Int32 beacon_rx, rssi_mgmt;
                        stats->GetRssi_mgmt(&rssi_mgmt);
                        stats->GetBeacon_rx(&beacon_rx);
                        if (rssi != IWifiInfo::INVALID_RSSI
                                && (rssi_mgmt == 0
                                || beacon_rx == 0)) {
                            stats = NULL;
                        }
                    }
                    // Get Info and continue polling
                    mHost->FetchRssiLinkSpeedAndFrequencyNative();
                    mHost->CalculateWifiScore(stats);
                }
                AutoPtr<IMessage> msg;
                mHost->ObtainMessage(CMD_RSSI_POLL,
                        mHost->mRssiPollToken, 0, (IMessage**)&msg);
                mHost->SendMessageDelayed(msg, POLL_RSSI_INTERVAL_MSECS);

                Int32 rssi;
                mHost->mWifiInfo->GetRssi(&rssi);
                if (DBG) mHost->SendRssiChangeBroadcast(rssi);
            }
            else {
                // Polling has completed
            }
            break;
        }
        case CMD_ENABLE_RSSI_POLL:
            if (mHost->mWifiConfigStore->enableRssiPollWhenAssociated) {
                Int32 arg1;
                message->GetArg1(&arg1);
                mHost->mEnableRssiPolling = (arg1 == 1);
            }
            else {
                mHost->mEnableRssiPolling = FALSE;
            }
            mHost->mRssiPollToken++;
            if (mHost->mEnableRssiPolling) {
                // First poll
                mHost->FetchRssiLinkSpeedAndFrequencyNative();
                AutoPtr<IMessage> msg;
                mHost->ObtainMessage(CMD_RSSI_POLL,
                        mHost->mRssiPollToken, 0, (IMessage**)&msg);
                mHost->SendMessageDelayed(msg, POLL_RSSI_INTERVAL_MSECS);
            }
            else {
                mHost->CleanWifiScore();
            }
            break;
        case IWifiManager::RSSI_PKTCNT_FETCH: {
            AutoPtr<IRssiPacketCountInfo> info;
            CRssiPacketCountInfo::New((IRssiPacketCountInfo**)&info);
            mHost->FetchRssiLinkSpeedAndFrequencyNative();
            Int32 rssi;
            mHost->mWifiInfo->GetRssi(&rssi);
            info->SetRssi(rssi);
            mHost->FetchPktcntNative(info);
            mHost->ReplyToMessage(message, IWifiManager::RSSI_PKTCNT_FETCH_SUCCEEDED, info);
            break;
        }
        case CMD_DELAYED_NETWORK_DISCONNECT: {
            Int32 arg1;
            message->GetArg1(&arg1);
            if (!mHost->linkDebouncing && mHost->mWifiConfigStore->enableLinkDebouncing) {

                // Ignore if we are not debouncing
                mHost->Loge(String("CMD_DELAYED_NETWORK_DISCONNECT and not debouncing - ignore ")
                        + arg1);
                return HANDLED;
            }
            else {
                mHost->Loge(String("CMD_DELAYED_NETWORK_DISCONNECT and debouncing - disconnect ")
                        + arg1);

                mHost->linkDebouncing = FALSE;
                // If we are still debouncing while this message comes,
                // it means we were not able to reconnect within the alloted time
                // = LINK_FLAPPING_DEBOUNCE_MSEC
                // and thus, trigger a real disconnect
                mHost->HandleNetworkDisconnect();
                mHost->TransitionTo(mHost->mDisconnectedState);
            }
            break;
        }
        case CMD_ASSOCIATED_BSSID: {
            AutoPtr<IInterface> obj;
            message->GetObj((IInterface**)&obj);
            String str;
            ICharSequence::Probe(obj)->ToString(&str);
            if (str == NULL) {
                mHost->Loge(String("Associated command w/o BSSID"));
                break;
            }
            mHost->mLastBssid = str;
            mHost->mWifiInfo->SetBSSID(str);
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
    // This is handled by receiving a NETWORK_DISCONNECTION_EVENT in ConnectModeState
    // Bug: 15347363
    // For paranoia's sake, call mHost->HandleNetworkDisconnect
    // only if BSSID is NULL or last networkId
    // is not invalid.
    if (DBG) {
        StringBuilder sb;
        sb.Append(String("leaving L2ConnectedState state nid=") + StringUtils::ToString(mHost->mLastNetworkId));
        if (mHost->mLastBssid != NULL) {
            sb.Append(" ");
            sb.Append(mHost->mLastBssid);
        }
    }
    if (mHost->mLastBssid != NULL || mHost->mLastNetworkId != IWifiConfiguration::INVALID_NETWORK_ID) {
        mHost->HandleNetworkDisconnect();
    }
    return NOERROR;
}

String WifiStateMachine::L2ConnectedState::GetName()
{
    return String("L2ConnectedState");
}

//==============================================================================
//  WifiStateMachine::ObtainingIpState
//==============================================================================

ECode WifiStateMachine::ObtainingIpState::Enter()
{
    Boolean b;
    mHost->mWifiConfigStore->IsUsingStaticIp(mHost->mLastNetworkId, &b);
    if (DBG) {
        String key = String("");
        AutoPtr<IWifiConfiguration> wc;
        mHost->GetCurrentWifiConfiguration((IWifiConfiguration**)&wc);
        if (wc != NULL) {
            wc->ConfigKey(&key);
        }
        mHost->Log(String("enter ObtainingIpState netId=") + StringUtils::ToString(mHost->mLastNetworkId)
                + " " + key + " "
                + " roam=" + mHost->mAutoRoaming
                + " static=" + b
                + " watchdog= " + mHost->obtainingIpWatchdogCount);
    }

    // Reset link Debouncing, indicating we have successfully re-connected to the AP
    // We might still be roaming
    mHost->linkDebouncing = FALSE;

    // We must clear the config BSSID, as the wifi chipset may decide to roam
    // from this point on and having the BSSID specified in the network block would
    // cause the roam to faile and the device to disconnect
    mHost->ClearCurrentConfigBSSID(String("ObtainingIpAddress"));

    // try {
    mHost->mNwService->EnableIpv6(mHost->mInterfaceName);
    // } catch (RemoteException re) {
    //     mHost->Loge("Failed to enable IPv6: " + re);
    // } catch (IllegalStateException e) {
    //     mHost->Loge("Failed to enable IPv6: " + e);
    // }
    if (!b) {
        if (mHost->IsRoaming(&b), b) {
            mHost->RenewDhcp();
        }
        else {
            // Remove any IP address on the interface in case we're switching from static
            // IP configuration to DHCP. This is safe because if we get here when not
            // roaming, we don't have a usable address.
            mHost->ClearIPv4Address(mHost->mInterfaceName);
            mHost->StartDhcp();
        }
        mHost->obtainingIpWatchdogCount++;
        mHost->Loge(String("Start Dhcp Watchdog ") + mHost->obtainingIpWatchdogCount);
        AutoPtr<IMessage> msg;
        mHost->ObtainMessage(CMD_OBTAINING_IP_ADDRESS_WATCHDOG_TIMER,
                mHost->obtainingIpWatchdogCount, 0, (IMessage**)&msg);
        mHost->SendMessageDelayed(msg, OBTAINING_IP_ADDRESS_GUARD_TIMER_MSEC);
    }
    else {
        // stop any running dhcp before assigning static IP
        mHost->StopDhcp();
        AutoPtr<IStaticIpConfiguration> config;
        mHost->mWifiConfigStore->GetStaticIpConfiguration(
                mHost->mLastNetworkId, (IStaticIpConfiguration**)&config);
        AutoPtr<ILinkAddress> ipAddress;
        config->GetIpAddress((ILinkAddress**)&ipAddress);
        if (ipAddress == NULL) {
            mHost->Loge(String("Static IP lacks address"));
            mHost->SendMessage(CMD_STATIC_IP_FAILURE);
        }
        else {
            AutoPtr<IInterfaceConfiguration> ifcg;
            CInterfaceConfiguration::New((IInterfaceConfiguration**)&ifcg);
            ifcg->SetLinkAddress(ipAddress);
            ifcg->SetInterfaceUp();
            // try {
            mHost->mNwService->SetInterfaceConfig(mHost->mInterfaceName, ifcg);
            if (DBG) mHost->Log(String("Static IP configuration succeeded"));
            AutoPtr<IDhcpResults> dhcpResults;
            CDhcpResults::New(config, (IDhcpResults**)&dhcpResults);
            mHost->SendMessage(CMD_STATIC_IP_SUCCESS, dhcpResults);
            // } catch (RemoteException re) {
            //     mHost->Loge("Static IP configuration failed: " + re);
            //     sendMessage(CMD_STATIC_IP_FAILURE);
            // } catch (IllegalStateException e) {
            //     mHost->Loge("Static IP configuration failed: " + e);
            //     sendMessage(CMD_STATIC_IP_FAILURE);
            // }
        }
    }
    return NOERROR;
}

ECode WifiStateMachine::ObtainingIpState::ProcessMessage(
    /* [in] */ IMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    mHost->LogStateAndMessage(message, String("WifiStateMachine::ObtainingIpState"));

    Int32 what;
    message->GetWhat(&what);
    switch (what) {
        case CMD_STATIC_IP_SUCCESS: {
            AutoPtr<IInterface> obj;
            message->GetObj((IInterface**)&obj);
            mHost->HandleIPv4Success(IDhcpResults::Probe(obj), CMD_STATIC_IP_SUCCESS);
            break;
        }
        case CMD_STATIC_IP_FAILURE:
            mHost->HandleIPv4Failure(CMD_STATIC_IP_FAILURE);
            break;
        case CMD_AUTO_CONNECT:
        case CMD_AUTO_ROAM:
            mHost->messageHandlingStatus = MESSAGE_HANDLING_STATUS_DISCARD;
            break;
        case IWifiManager::SAVE_NETWORK:
        case WifiStateMachine::CMD_AUTO_SAVE_NETWORK:
            mHost->messageHandlingStatus = MESSAGE_HANDLING_STATUS_DEFERRED;
            mHost->DeferMessage(message);
            break;
            /* Defer any power mode changes since we must keep active power mode at DHCP */
        case CMD_SET_HIGH_PERF_MODE:
            mHost->messageHandlingStatus = MESSAGE_HANDLING_STATUS_DEFERRED;
            mHost->DeferMessage(message);
            break;
            /* Defer scan request since we should not switch to other channels at DHCP */
        case CMD_START_SCAN:
            mHost->messageHandlingStatus = MESSAGE_HANDLING_STATUS_DEFERRED;
            mHost->DeferMessage(message);
            break;
        case CMD_OBTAINING_IP_ADDRESS_WATCHDOG_TIMER: {
            Int32 arg1;
            message->GetArg1(&arg1);
            if (arg1 == mHost->obtainingIpWatchdogCount) {
                mHost->Loge(String("ObtainingIpAddress: Watchdog Triggered, count=")
                        + mHost->obtainingIpWatchdogCount);
                mHost->HandleIpConfigurationLost();
                mHost->TransitionTo(mHost->mDisconnectingState);
                break;
            }
            mHost->messageHandlingStatus = MESSAGE_HANDLING_STATUS_DISCARD;
            break;
        }
        default:
            *result = NOT_HANDLED;
            return NOERROR;
    }
    *result = HANDLED;
    return NOERROR;
}

String WifiStateMachine::ObtainingIpState::GetName()
{
    return String("ObtainingIpState");
}

//==============================================================================
//  WifiStateMachine::VerifyingLinkState
//==============================================================================

ECode WifiStateMachine::VerifyingLinkState::Enter()
{
    mHost->Log(String("WifiStateMachine::VerifyingLinkState") + " enter");
    mHost->SetNetworkDetailedState(
            Elastos::Droid::Net::NetworkInfoDetailedState_VERIFYING_POOR_LINK);
    mHost->mWifiConfigStore->UpdateStatus(mHost->mLastNetworkId,
            Elastos::Droid::Net::NetworkInfoDetailedState_VERIFYING_POOR_LINK);
    mHost->SendNetworkStateChangeBroadcast(mHost->mLastBssid);
    // End roaming
    mHost->mAutoRoaming = WifiAutoJoinController::AUTO_JOIN_IDLE;
    return NOERROR;
}

ECode WifiStateMachine::VerifyingLinkState::ProcessMessage(
    /* [in] */ IMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    mHost->LogStateAndMessage(message, String("WifiStateMachine::VerifyingLinkState"));

    String name;
    mHost->GetName(&name);
    Int32 what;
    message->GetWhat(&what);
    switch (what) {
        case WifiWatchdogStateMachine::POOR_LINK_DETECTED:
            // Stay here
            mHost->Log(name + " POOR_LINK_DETECTED: no transition");
            break;
        case WifiWatchdogStateMachine::GOOD_LINK_DETECTED:
            mHost->Log(name + " GOOD_LINK_DETECTED: transition to captive portal check");

            mHost->Log(name + " GOOD_LINK_DETECTED: transition to CONNECTED");
            mHost->SendConnectedState();
            mHost->TransitionTo(mHost->mConnectedState);
            break;
        default:
            if (DBG) mHost->Log(name + " what=" + what + " NOT_HANDLED");
            *result = NOT_HANDLED;
            return NOERROR;
    }
    *result = HANDLED;
    return NOERROR;
}

String WifiStateMachine::VerifyingLinkState::GetName()
{
    return String("VerifyingLinkState");
}

//==============================================================================
//  WifiStateMachine::RoamingState
//==============================================================================

ECode WifiStateMachine::RoamingState::Enter()
{
    if (DBG) {
        mHost->Log(String("RoamingState Enter")
                + " mScreenOn=" + mHost->mScreenOn );
    }
    mHost->SetScanAlarm(FALSE, 0);

    // Make sure we disconnect if roaming fails
    mHost->roamWatchdogCount++;
    mHost->Loge(String("Start Roam Watchdog ") + mHost->roamWatchdogCount);
    AutoPtr<IMessage> msg;
    mHost->ObtainMessage(CMD_ROAM_WATCHDOG_TIMER,
            mHost->roamWatchdogCount, 0, (IMessage**)&msg);
    mHost->SendMessageDelayed(msg, ROAM_GUARD_TIMER_MSEC);
    mAssociated = FALSE;
    return NOERROR;
}

ECode WifiStateMachine::RoamingState::ProcessMessage(
    /* [in] */ IMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    mHost->LogStateAndMessage(message, String("WifiStateMachine::RoamingState"));

    Int32 what;
    message->GetWhat(&what);
    switch (what) {
       case WifiWatchdogStateMachine::POOR_LINK_DETECTED:
            if (DBG) mHost->Log(String("Roaming and Watchdog reports poor link -> ignore"));
            *result = HANDLED;
            return NOERROR;
       case CMD_UNWANTED_NETWORK:
            if (DBG) mHost->Log(String("Roaming and CS doesnt want the network -> ignore"));
            *result = HANDLED;
            return NOERROR;
       case CMD_SET_OPERATIONAL_MODE: {
            Int32 arg1;
            message->GetArg1(&arg1);
            if (arg1 != CONNECT_MODE) {
                mHost->DeferMessage(message);
            }
            break;
        }
       case WifiMonitor::SUPPLICANT_STATE_CHANGE_EVENT: {
            /**
             * If we get a SUPPLICANT_STATE_CHANGE_EVENT indicating a DISCONNECT
             * before NETWORK_DISCONNECTION_EVENT
             * And there is an associated BSSID corresponding to our target BSSID, then
             * we have missed the network disconnection, transition to mHost->mDisconnectedState
             * and handle the rest of the events there.
             */
            AutoPtr<IInterface> obj;
            message->GetObj((IInterface**)&obj);
            AutoPtr<CStateChangeResult> stateChangeResult = (CStateChangeResult*)(IObject*)obj.Get();
            String bssid;
            stateChangeResult->GetBSSID(&bssid);
            SupplicantState state;
            stateChangeResult->GetSupplicantState(&state);

            if (state == Elastos::Droid::Wifi::SupplicantState_DISCONNECTED
                    || state == Elastos::Droid::Wifi::SupplicantState_INACTIVE
                    || state == Elastos::Droid::Wifi::SupplicantState_INTERFACE_DISABLED) {
                if (DBG) {
                    String str;
                    stateChangeResult->ToString(&str);
                    mHost->Log(String("STATE_CHANGE_EVENT in roaming state ")
                            + str);
                }
                if (bssid != NULL
                        && bssid.Equals(mHost->mTargetRoamBSSID)) {
                    mHost->HandleNetworkDisconnect();
                    mHost->TransitionTo(mHost->mDisconnectedState);
                }
            }
            if (state == Elastos::Droid::Wifi::SupplicantState_ASSOCIATED) {
                // We completed the layer2 roaming part
                mAssociated = TRUE;
                if (bssid != NULL) {
                    mHost->mTargetRoamBSSID = bssid;
                }
            }
            break;
        }
        case CMD_ROAM_WATCHDOG_TIMER: {
            Int32 arg1;
            message->GetArg1(&arg1);
            if (mHost->roamWatchdogCount == arg1) {
                if (DBG) mHost->Log(String("roaming watchdog! -> disconnect"));
                mHost->mRoamFailCount++;
                mHost->HandleNetworkDisconnect();
                mHost->mWifiNative->Disconnect();
                mHost->TransitionTo(mHost->mDisconnectedState);
            }
            break;
        }
        case WifiMonitor::NETWORK_CONNECTION_EVENT: {
            Int32 arg1;
            message->GetArg1(&arg1);
            if (mAssociated) {
                if (DBG) mHost->Log(String("roaming and Network connection established"));
                mHost->mLastNetworkId = arg1;
                AutoPtr<IInterface> obj;
                message->GetObj((IInterface**)&obj);
                ICharSequence::Probe(obj)->ToString(&mHost->mLastBssid);
                mHost->mWifiInfo->SetBSSID(mHost->mLastBssid);
                mHost->mWifiInfo->SetNetworkId(mHost->mLastNetworkId);
                Boolean b;
                mHost->mWifiConfigStore->HandleBSSIDBlackList(
                        mHost->mLastNetworkId, mHost->mLastBssid, TRUE, &b);
                mHost->TransitionTo(mHost->mObtainingIpState);
            }
            else {
                mHost->messageHandlingStatus = MESSAGE_HANDLING_STATUS_DISCARD;
            }
            break;
        }
        case WifiMonitor::NETWORK_DISCONNECTION_EVENT: {
            // Throw away but only if it corresponds to the network we're roaming to
            AutoPtr<IInterface> obj;
            message->GetObj((IInterface**)&obj);
            String bssid;
            ICharSequence::Probe(obj)->ToString(&bssid);
            if (TRUE) {
                String target = String("");
                if (mHost->mTargetRoamBSSID != NULL) target = mHost->mTargetRoamBSSID;
                mHost->Log(String("NETWORK_DISCONNECTION_EVENT in roaming state")
                        + " BSSID=" + bssid
                        + " target=" + target);
            }
            if (bssid != NULL && bssid.Equals(mHost->mTargetRoamBSSID)) {
                mHost->HandleNetworkDisconnect();
                mHost->TransitionTo(mHost->mDisconnectedState);
            }
            break;
        }
        case WifiMonitor::SSID_TEMP_DISABLED: {
            // Auth error while roaming
            Int32 arg1;
            message->GetArg1(&arg1);
            Boolean b;
            mHost->Loge(String("SSID_TEMP_DISABLED nid=") + StringUtils::ToString(mHost->mLastNetworkId)
                    + " id=" + StringUtils::ToString(arg1)
                    + " isRoaming=" + (mHost->IsRoaming(&b), b)
                    + " roam=" + StringUtils::ToString(mHost->mAutoRoaming));
            if (arg1 == mHost->mLastNetworkId) {
                mHost->HandleNetworkDisconnect();
                mHost->TransitionTo(mHost->mDisconnectingState);
            }
            *result = NOT_HANDLED;
            return NOERROR;
        }
        case CMD_START_SCAN:
            mHost->DeferMessage(message);
            break;
        default:
            *result = NOT_HANDLED;
            return NOERROR;
    }
    *result = HANDLED;
    return NOERROR;
}

ECode WifiStateMachine::RoamingState::Exit()
{
    mHost->Loge(String("WifiStateMachine: Leaving Roaming state"));
    return NOERROR;
}

String WifiStateMachine::RoamingState::GetName()
{
    return String("RoamingState");
}

//==============================================================================
//  WifiStateMachine::ConnectedState
//==============================================================================

ECode WifiStateMachine::ConnectedState::Enter()
{
    String address;
    mHost->UpdateDefaultRouteMacAddress(1000);
    if (DBG) {
        mHost->Log(String("ConnectedState Enter ")
                + " mScreenOn=" + mHost->mScreenOn
                + " scanperiod="
                + StringUtils::ToString(mHost->mWifiConfigStore->associatedPartialScanPeriodMilli) );
    }
    if (mHost->mScreenOn
            && mHost->mWifiConfigStore->enableAutoJoinScanWhenAssociated) {
        mHost->mCurrentScanAlarmMs = mHost->mWifiConfigStore->associatedPartialScanPeriodMilli;
        // Scan after 200ms
        mHost->SetScanAlarm(TRUE, 200);
    }
    else {
        mHost->mCurrentScanAlarmMs = 0;
    }
    mHost->RegisterConnected();
    mHost->lastConnectAttempt = 0;
    mHost->targetWificonfiguration = NULL;
    // Paranoia
    mHost->linkDebouncing = FALSE;

    // Not roaming anymore
    mHost->mAutoRoaming = WifiAutoJoinController::AUTO_JOIN_IDLE;

    if (mHost->testNetworkDisconnect) {
        mHost->testNetworkDisconnectCounter++;
        mHost->Loge(String("ConnectedState Enter start disconnect test ") +
                mHost->testNetworkDisconnectCounter);
        AutoPtr<IMessage> msg;
        mHost->ObtainMessage(CMD_TEST_NETWORK_DISCONNECT,
                mHost->testNetworkDisconnectCounter, 0, (IMessage**)&msg);
        mHost->SendMessageDelayed(msg, 15000);
    }

    // Reenable all networks, allow for hidden networks to be scanned
    mHost->mWifiConfigStore->EnableAllNetworks();

    mHost->mLastDriverRoamAttempt = 0;

    return NOERROR;
}

ECode WifiStateMachine::ConnectedState::ProcessMessage(
    /* [in] */ IMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IWifiConfiguration> config;
    mHost->LogStateAndMessage(message, String("WifiStateMachine::ConnectedState"));

    Int32 what;
    message->GetWhat(&what);
    switch (what) {
        case WifiWatchdogStateMachine::POOR_LINK_DETECTED:
            if (DBG) mHost->Log(String("Watchdog reports poor link"));
            mHost->TransitionTo(mHost->mVerifyingLinkState);
            break;
        case CMD_UNWANTED_NETWORK: {
            Int32 arg1;
            message->GetArg1(&arg1);
            if (arg1 == WifiStateMachine::network_status_unwanted_disconnect) {
                mHost->mWifiConfigStore->HandleBadNetworkDisconnectReport(mHost->mLastNetworkId, mHost->mWifiInfo);
                mHost->mWifiNative->Disconnect();
                mHost->TransitionTo(mHost->mDisconnectingState);
            }
            else if (arg1 == WifiStateMachine::network_status_unwanted_disconnect) {
                mHost->GetCurrentWifiConfiguration((IWifiConfiguration**)&config);
                if (config != NULL) {
                    // Disable autojoin
                    config->SetNoInternetAccess(TRUE);
                }
            }
            *result = HANDLED;
            return NOERROR;
        }
        case CMD_TEST_NETWORK_DISCONNECT: {
            // Force a disconnect
            Int32 arg1;
            message->GetArg1(&arg1);
            if (arg1 == mHost->testNetworkDisconnectCounter) {
                mHost->mWifiNative->Disconnect();
            }
            break;
        }
        case CMD_ASSOCIATED_BSSID: {
            // ASSOCIATING to a new BSSID while already connected, indicates
            // that driver is roaming
            AutoPtr<ISystem> system;
            CSystem::AcquireSingleton((ISystem**)&system);
            system->GetCurrentTimeMillis(&mHost->mLastDriverRoamAttempt);
            AutoPtr<IInterface> obj;
            message->GetObj((IInterface**)&obj);
            String toBSSID;
            ICharSequence::Probe(obj)->ToString(&toBSSID);
            String bssid;
            mHost->mWifiInfo->GetBSSID(&bssid);
            if (!toBSSID.IsNull() && !toBSSID.Equals(bssid)) {
                mHost->mWifiConfigStore->DriverRoamedFrom(mHost->mWifiInfo);
            }
            *result = NOT_HANDLED;
            return NOERROR;
        }
        case WifiMonitor::NETWORK_DISCONNECTION_EVENT: {
            Int64 lastRoam = 0;
            if (mHost->mLastDriverRoamAttempt != 0) {
                // Calculate time since last driver roam attempt
                AutoPtr<ISystem> system;
                CSystem::AcquireSingleton((ISystem**)&system);
                Int64 currentTimeMillis;
                system->GetCurrentTimeMillis(&currentTimeMillis);
                lastRoam = currentTimeMillis - mHost->mLastDriverRoamAttempt;
                mHost->mLastDriverRoamAttempt = 0;
            }
            mHost->GetCurrentWifiConfiguration((IWifiConfiguration**)&config);
            Int32 arg2;
            message->GetArg2(&arg2);
            Int32 autoJoinStatus;
            config->GetAutoJoinStatus(&autoJoinStatus);
            Int32 freq;
            mHost->mWifiInfo->GetFrequency(&freq);
            Int32 rssi;
            mHost->mWifiInfo->GetRssi(&rssi);
            String bssid;
            AutoPtr<IScanResultHelper> helper;
            CScanResultHelper::AcquireSingleton((IScanResultHelper**)&helper);
            Boolean b1, b2, b3;
            mHost->mWifiConfigStore->IsLastSelectedConfiguration(config, &b1);
            helper->Is24GHz(freq, &b2);
            helper->Is5GHz(freq, &b3);
            if (mHost->mScreenOn
                    && !mHost->linkDebouncing
                    && config != NULL
                    && autoJoinStatus == IWifiConfiguration::AUTO_JOIN_ENABLED
                    && !b1
                    && (arg2 != 3 /* reason cannot be 3, i.e. locally generated */
                        || (lastRoam > 0 && lastRoam < 2000) /* unless driver is roaming */)
                    && ((b2 && rssi > IWifiConfiguration::BAD_RSSI_24)
                        || (b3 && rssi > IWifiConfiguration::BAD_RSSI_5))) {
                // Start de-bouncing the L2 disconnection:
                // this L2 disconnection might be spurious.
                // Hence we allow 7 seconds for the state machine to try
                // to reconnect, go thru the
                // roaming cycle and enter Obtaining IP address
                // before signalling the disconnect to ConnectivityService and L3
                AutoPtr<IWifiConfiguration> wc;
                mHost->GetCurrentWifiConfiguration((IWifiConfiguration**)&wc);
                Boolean b;
                mHost->StartScanForConfiguration(wc, FALSE, &b);
                mHost->linkDebouncing = TRUE;

                AutoPtr<IMessage> msg;
                mHost->ObtainMessage(CMD_DELAYED_NETWORK_DISCONNECT,
                        0, mHost->mLastNetworkId, (IMessage**)&msg);
                mHost->SendMessageDelayed(msg, LINK_FLAPPING_DEBOUNCE_MSEC);
                if (DBG) {
                    mHost->Log(String("NETWORK_DISCONNECTION_EVENT in connected state")
                            + " BSSID=" + (mHost->mWifiInfo->GetBSSID(&bssid), bssid)
                            + " RSSI=" + (mHost->mWifiInfo->GetRssi(&rssi), rssi)
                            + " freq=" + (mHost->mWifiInfo->GetFrequency(&freq), freq)
                            + " reason=" + arg2
                            + " -> debounce");
                }
                *result = HANDLED;
                return NOERROR;
            }
            else {
                if (DBG) {
                    Int32 ajst = -1;
                    if (config != NULL) config->GetAutoJoinStatus(&ajst);
                    mHost->Log(String("NETWORK_DISCONNECTION_EVENT in connected state")
                            + " BSSID=" + (mHost->mWifiInfo->GetBSSID(&bssid), bssid)
                            + " RSSI=" + (mHost->mWifiInfo->GetRssi(&rssi), rssi)
                            + " freq=" + (mHost->mWifiInfo->GetFrequency(&freq), freq)
                            + " was debouncing=" + mHost->linkDebouncing
                            + " reason=" + arg2
                            + " ajst=" + ajst);
                }
            }
            break;
        }
        case CMD_AUTO_ROAM: {
            Int32 arg2;
            message->GetArg2(&arg2);
            // Clear the driver roam indication since we are attempting a framerwork roam
            mHost->mLastDriverRoamAttempt = 0;

            /* Connect command coming from auto-join */
            AutoPtr<IInterface> obj;
            message->GetObj((IInterface**)&obj);
            AutoPtr<IScanResult> candidate = IScanResult::Probe(obj);
            String bssid = String("any");
            Boolean b;
            if (candidate != NULL && (candidate->Is5GHz(&b), b)) {
                // Only lock BSSID for 5GHz networks
                candidate->GetBSSID(&bssid);
            }
            Int32 netId = mHost->mLastNetworkId;
            mHost->GetCurrentWifiConfiguration((IWifiConfiguration**)&config);


            if (config == NULL) {
                mHost->Loge(String("AUTO_ROAM and no config, bail out..."));
                break;
            }

            String name , key;
            mHost->Loge(String("CMD_AUTO_ROAM sup state ")
                    + mHost->mSupplicantStateTracker->GetSupplicantStateName()
                    + " my state " + (mHost->GetCurrentState()->GetName(&name), name)
                    + " nid=" + StringUtils::ToString(netId)
                    + " config " + (config->ConfigKey(&key), key)
                    + " roam=" + StringUtils::ToString(arg2)
                    + " to " + bssid
                    + " targetRoamBSSID " + mHost->mTargetRoamBSSID);

            /* Save the BSSID so as to lock it @ firmware */
            if ((mHost->AutoRoamSetBSSID(config, bssid, &b), !b) && !mHost->linkDebouncing) {
                mHost->Loge(String("AUTO_ROAM nothing to do"));
                // Same BSSID, nothing to do
                mHost->messageHandlingStatus = MESSAGE_HANDLING_STATUS_DISCARD;
                break;
            };

            // Make sure the network is enabled, since supplicant will not reenable it
            mHost->mWifiConfigStore->EnableNetworkWithoutBroadcast(netId, FALSE, &b);

            Boolean ret = FALSE;
            if (mHost->mLastNetworkId != netId) {
                mHost->mWifiConfigStore->SelectNetwork(netId, &b);
               if (b && mHost->mWifiNative->Reconnect()) {
                   ret = TRUE;
               }
            }
            else {
                 ret = mHost->mWifiNative->Reassociate();
            }
            if (ret) {
                AutoPtr<ISystem> system;
                CSystem::AcquireSingleton((ISystem**)&system);
                system->GetCurrentTimeMillis(&mHost->lastConnectAttempt);
                mHost->mWifiConfigStore->GetWifiConfiguration(netId,
                        (IWifiConfiguration**)&mHost->targetWificonfiguration);

                // mHost->ReplyToMessage(message, IWifiManager::CONNECT_NETWORK_SUCCEEDED);
                mHost->mAutoRoaming = arg2;
                mHost->TransitionTo(mHost->mRoamingState);

            }
            else {
                String str;
                IObject::Probe(config)->ToString(&str);
                mHost->Loge(String("Failed to connect config: ") + str + " netId: " + netId);
                mHost->ReplyToMessage(message, IWifiManager::CONNECT_NETWORK_FAILED,
                        IWifiManager::ERROR);
                mHost->messageHandlingStatus = MESSAGE_HANDLING_STATUS_FAIL;
                break;
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

ECode WifiStateMachine::ConnectedState::Exit()
{
    mHost->Loge(String("WifiStateMachine: Leaving Connected state"));
    mHost->SetScanAlarm(FALSE, 0);
    mHost->mLastDriverRoamAttempt = 0;
    return NOERROR;
}

String WifiStateMachine::ConnectedState::GetName()
{
    return String("ConnectedState");
}

//==============================================================================
//  WifiStateMachine::DisconnectingState
//==============================================================================

ECode WifiStateMachine::DisconnectingState::Enter()
{
    mHost->mCurrentScanAlarmMs = mHost->mDisconnectedScanPeriodMs;

    if (PDBG) {
        mHost->Loge(String(" Enter DisconnectingState State scan interval ") + mHost->mFrameworkScanIntervalMs
                + " mEnableBackgroundScan= " + mHost->mEnableBackgroundScan
                + " screenOn=" + mHost->mScreenOn);
    }

    // Make sure we disconnect: we enter this state prior connecting to a new
    // network, waiting for either a DISCONECT event or a SUPPLICANT_STATE_CHANGE
    // event which in this case will be indicating that supplicant started to associate.
    // In some cases supplicant doesn't ignore the connect requests (it might not
    // find the target SSID in its cache),
    // Therefore we end up stuck that state, hence the need for the watchdog.
    mHost->disconnectingWatchdogCount++;
    mHost->Loge(String("Start Disconnecting Watchdog ") + mHost->disconnectingWatchdogCount);
    AutoPtr<IMessage> msg;
    mHost->ObtainMessage(CMD_DISCONNECTING_WATCHDOG_TIMER,
            mHost->disconnectingWatchdogCount, 0, (IMessage**)&msg);
    mHost->SendMessageDelayed(msg, DISCONNECTING_GUARD_TIMER_MSEC);
    return NOERROR;
}

ECode WifiStateMachine::DisconnectingState::ProcessMessage(
    /* [in] */ IMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    mHost->LogStateAndMessage(message, String("WifiStateMachine::DisconnectingState"));

    Int32 arg1;
    message->GetArg1(&arg1);
    Int32 what;
    message->GetWhat(&what);
    switch (what) {
        case CMD_SET_OPERATIONAL_MODE:
            if (arg1 != CONNECT_MODE) {
                mHost->DeferMessage(message);
            }
            break;
        case CMD_START_SCAN:
            // Ignore scans while disconnecting
            *result = HANDLED;
            return NOERROR;
        case CMD_DISCONNECTING_WATCHDOG_TIMER:
            if (mHost->disconnectingWatchdogCount == arg1) {
                if (DBG) mHost->Log(String("disconnecting watchdog! -> disconnect"));
                mHost->HandleNetworkDisconnect();
                mHost->TransitionTo(mHost->mDisconnectedState);
            }
            break;
        case WifiMonitor::SUPPLICANT_STATE_CHANGE_EVENT:
            /**
             * If we get a SUPPLICANT_STATE_CHANGE_EVENT before NETWORK_DISCONNECTION_EVENT
             * we have missed the network disconnection, transition to mHost->mDisconnectedState
             * and handle the rest of the events there
             */
            mHost->DeferMessage(message);
            mHost->HandleNetworkDisconnect();
            mHost->TransitionTo(mHost->mDisconnectedState);
            break;
        default:
            *result = NOT_HANDLED;
            return NOERROR;
    }
    *result = HANDLED;
    return NOERROR;
}

ECode WifiStateMachine::DisconnectingState::Exit()
{
    mHost->mCurrentScanAlarmMs = 0;
    return NOERROR;
}

String WifiStateMachine::DisconnectingState::GetName()
{
    return String("DisconnectingState");
}

//==============================================================================
//  WifiStateMachine::DisconnectedState
//==============================================================================

ECode WifiStateMachine::DisconnectedState::Enter()
{
    // We dont scan frequently if this is a temporary disconnect
    // due to p2p
    if (mHost->mTemporarilyDisconnectWifi) {
        mHost->mWifiP2pChannel->SendMessage(WifiP2pServiceImpl::DISCONNECT_WIFI_RESPONSE);
        return NOERROR;
    }

    // Loose the last selection choice
    // mWifiAutoJoinController.setLastSelectedConfiguration
    // (WifiConfiguration.INVALID_NETWORK_ID);

    AutoPtr<IContentResolver> cr;
    mHost->mContext->GetContentResolver((IContentResolver**)&cr);
    Settings::Global::GetInt64(cr,
            ISettingsGlobal::WIFI_FRAMEWORK_SCAN_INTERVAL_MS,
            mHost->mDefaultFrameworkScanIntervalMs,
            &mHost->mFrameworkScanIntervalMs);

    // Configure the scan alarm time to mHost->mFrameworkScanIntervalMs
    // (5 minutes) if there are no saved profiles as there is
    // already a periodic scan getting issued for every
    // mSupplicantScanIntervalMs seconds. However keep the
    // scan frequency by setting it to mDisconnectedScanPeriodMs
    // (10 seconds) when there are configured profiles.
    if (mHost->mScreenOn) {
        AutoPtr<IList> list;
        mHost->mWifiConfigStore->GetConfiguredNetworks((IList**)&list);
        Int32 size;
        list->GetSize(&size);
        if (size != 0) {
            mHost->mCurrentScanAlarmMs = mHost->mDisconnectedScanPeriodMs;
        }
        else {
            mHost->mCurrentScanAlarmMs = mHost->mFrameworkScanIntervalMs;
        }
    }
    if (PDBG) {
        mHost->Loge(String(" Enter disconnected State scan interval ") + mHost->mFrameworkScanIntervalMs
                + " mEnableBackgroundScan= " + mHost->mEnableBackgroundScan
                + " screenOn=" + mHost->mScreenOn);
    }

    /** clear the roaming state, if we were roaming, we failed */
    mHost->mAutoRoaming = WifiAutoJoinController::AUTO_JOIN_IDLE;

    // Reenable all networks, allow for hidden networks to be scanned
    mHost->mWifiConfigStore->EnableAllNetworks();

    /**
     * - screen dark and PNO supported => scan alarm disabled
     * - everything else => scan alarm enabled with mDefaultFrameworkScanIntervalMs period
     */
    if ((mHost->mScreenOn == FALSE) && mHost->mEnableBackgroundScan) { //mEnableBackgroundScan)
        /* If a regular scan result is pending, do not initiate background
         * scan until the scan results are returned. This is needed because
         * initiating a background scan will cancel the regular scan and
         * scan results will not be returned until background scanning is
         * cleared
         */
        if (!mHost->mIsScanOngoing) {
            if (!mHost->mWifiNative->EnableBackgroundScan(TRUE)) {
                mHost->HandlePnoFailError();
            }
        }
    }
    else {
        mHost->SetScanAlarm(TRUE, 200);
    }

    /**
     * If we have no networks saved, the supplicant stops doing the periodic scan.
     * The scans are useful to notify the user of the presence of an open network.
     * Note that these are not wake up scans.
     */
    Boolean b;
    mHost->mP2pConnected->Get(&b);
    AutoPtr<IList> list;
    mHost->mWifiConfigStore->GetConfiguredNetworks((IList**)&list);
    Int32 size;
    list->GetSize(&size);
    if (!b && size == 0) {
        AutoPtr<IMessage> msg;
        mHost->ObtainMessage(CMD_NO_NETWORKS_PERIODIC_SCAN, ++mHost->mPeriodicScanToken, 0, (IMessage**)&msg);
        mHost->SendMessageDelayed(msg, mHost->mSupplicantScanIntervalMs);
    }

    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    system->GetCurrentTimeMillis(&mHost->mDisconnectedTimeStamp);
    return NOERROR;
}

ECode WifiStateMachine::DisconnectedState::ProcessMessage(
    /* [in] */ IMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Boolean ret = HANDLED;

    mHost->LogStateAndMessage(message, String("WifiStateMachine::DisconnectedState"));

    Int32 arg1;
    message->GetArg1(&arg1);
    Int32 what;
    message->GetWhat(&what);
    switch (what) {
        case CMD_NO_NETWORKS_PERIODIC_SCAN: {
            Boolean b;
            mHost->mP2pConnected->Get(&b);
            if (b) break;
            AutoPtr<IList> list;
            mHost->mWifiConfigStore->GetConfiguredNetworks((IList**)&list);
            Int32 size;
            list->GetSize(&size);
            if (arg1 == mHost->mPeriodicScanToken && size == 0) {
                mHost->StartScan(UNKNOWN_SCAN_SOURCE, -1, NULL, NULL);
                AutoPtr<IMessage> msg;
                mHost->ObtainMessage(CMD_NO_NETWORKS_PERIODIC_SCAN, ++mHost->mPeriodicScanToken, 0, (IMessage**)&msg);
                mHost->SendMessageDelayed(msg, mHost->mSupplicantScanIntervalMs);
            }
            break;
        }
        case CMD_PNO_PERIODIC_SCAN:
            if ((mHost->mBackgroundScanConfigured == FALSE) &&
                (arg1 == mHost->mPnoPeriodicScanToken) &&
                (mHost->mEnableBackgroundScan)) {
                mHost->StartScan(UNKNOWN_SCAN_SOURCE, -1, NULL, NULL);
                AutoPtr<IMessage> msg;
                mHost->ObtainMessage(CMD_PNO_PERIODIC_SCAN, ++mHost->mPnoPeriodicScanToken, 0, (IMessage**)&msg);
                mHost->SendMessageDelayed(msg, mHost->mDefaultFrameworkScanIntervalMs);
            }
            break;
        case IWifiManager::FORGET_NETWORK:
        case CMD_REMOVE_NETWORK: {
            // Set up a delayed message here. After the forget/remove is handled
            // the handled delayed message will determine if there is a need to
            // scan and continue
            AutoPtr<IMessage> msg;
            mHost->ObtainMessage(CMD_NO_NETWORKS_PERIODIC_SCAN, ++mHost->mPeriodicScanToken, 0, (IMessage**)&msg);
            mHost->SendMessageDelayed(msg, mHost->mSupplicantScanIntervalMs);
            ret = NOT_HANDLED;
            break;
        }
        case CMD_SET_OPERATIONAL_MODE:
            if (arg1 != CONNECT_MODE) {
                mHost->mOperationalMode = arg1;

                mHost->mWifiConfigStore->DisableAllNetworks();
                if (mHost->mOperationalMode == SCAN_ONLY_WITH_WIFI_OFF_MODE) {
                    mHost->mWifiP2pChannel->SendMessage(CMD_DISABLE_P2P_REQ);
                    mHost->SetWifiState(IWifiManager::WIFI_STATE_DISABLED);
                }

                mHost->TransitionTo(mHost->mScanModeState);
            }
            break;
            /* Ignore network disconnect */
        case WifiMonitor::NETWORK_DISCONNECTION_EVENT:
            break;
        case WifiMonitor::SUPPLICANT_STATE_CHANGE_EVENT: {
            AutoPtr<IInterface> obj;
            message->GetObj((IInterface**)&obj);
            AutoPtr<CStateChangeResult> stateChangeResult = (CStateChangeResult*)(IObject*)obj.Get();
            SupplicantState state;
            stateChangeResult->GetSupplicantState(&state);
            AutoPtr<ISupplicantState> s;
            CSupplicantState::New(state, (ISupplicantState**)&s);
            AutoPtr<IWifiInfoHelper> helper;
            CWifiInfoHelper::AcquireSingleton((IWifiInfoHelper**)&helper);
            NetworkInfoDetailedState nidState;
            helper->GetDetailedStateOf(s, &nidState);
            if (DBG) {
                mHost->Loge(String("SUPPLICANT_STATE_CHANGE_EVENT state=") + state +
                        " -> state= " + nidState
                        + " debouncing=" + mHost->linkDebouncing);
            }
            mHost->SetNetworkDetailedState(nidState);
            /* ConnectModeState does the rest of the handling */
            ret = NOT_HANDLED;
            break;
        }
        case CMD_START_SCAN:
            if (!mHost->IsScanAllowed(arg1)) {
                // Ignore the scan request
                if (VDBG) mHost->Logd(String("DisconnectedState: ignore scan"));
                *result = HANDLED;
                return NOERROR;
            }
            /* Disable background scan temporarily during a regular scan */
            if (mHost->mEnableBackgroundScan) {
                mHost->mWifiNative->EnableBackgroundScan(FALSE);
            }
            /* Handled in parent state */
            ret = NOT_HANDLED;
            break;
        case WifiMonitor::SCAN_RESULTS_EVENT:
            /* Re-enable background scan when a pending scan result is received */
            if (mHost->mEnableBackgroundScan && mHost->mIsScanOngoing) {
                if (!mHost->mWifiNative->EnableBackgroundScan(TRUE)) {
                    mHost->HandlePnoFailError();
                }
            }
            /* Handled in parent state */
            ret = NOT_HANDLED;
            break;
        case WifiP2pServiceImpl::P2P_CONNECTION_CHANGED: {
            AutoPtr<IInterface> obj;
            message->GetObj((IInterface**)&obj);
            AutoPtr<INetworkInfo> info = INetworkInfo::Probe(obj);
            Boolean b;
            info->IsConnected(&b);
            mHost->mP2pConnected->Set(b);
            AutoPtr<IList> list;
            mHost->mWifiConfigStore->GetConfiguredNetworks((IList**)&list);
            Int32 size;
            list->GetSize(&size);
            if (mHost->mP2pConnected->Get(&b), b) {
                AutoPtr<IResources> res;
                mHost->mContext->GetResources((IResources**)&res);
                Int32 defaultInterval;
                res->GetInteger(R::integer::config_wifi_scan_interval_p2p_connected, &defaultInterval);
                AutoPtr<IContentResolver> cr;
                mHost->mContext->GetContentResolver((IContentResolver**)&cr);
                Int64 scanIntervalMs;
                Settings::Global::GetInt64(cr,
                        ISettingsGlobal::WIFI_SCAN_INTERVAL_WHEN_P2P_CONNECTED_MS,
                        defaultInterval, &scanIntervalMs);
                mHost->mWifiNative->SetScanInterval((Int32)scanIntervalMs/1000);
            }
            else if (size == 0) {
                if (DBG) mHost->Log(String("Turn on scanning after p2p disconnected"));
                AutoPtr<IMessage> msg;
                mHost->ObtainMessage(CMD_NO_NETWORKS_PERIODIC_SCAN, ++mHost->mPeriodicScanToken, 0, (IMessage**)&msg);
                mHost->SendMessageDelayed(msg, mHost->mSupplicantScanIntervalMs);
            }
            else if (mHost->mEnableBackgroundScan &&
                    (mHost->mP2pConnected->Get(&b), !b) && (size != 0)) {
                if (!mHost->mWifiNative->EnableBackgroundScan(TRUE)) {
                    mHost->HandlePnoFailError();
                }
                else {
                    if (DBG) mHost->Log(String("Stop periodic scan on PNO success"));
                    mHost->mBackgroundScanConfigured = TRUE;
                }
            }
        }
        case CMD_RECONNECT:
        case CMD_REASSOCIATE:
            if (mHost->mTemporarilyDisconnectWifi) {
                // Drop a third party reconnect/reassociate if STA is
                // temporarily disconnected for p2p
                break;
            } else {
                // ConnectModeState handles it
                ret = NOT_HANDLED;
            }
            break;
        case WifiP2pServiceImpl::P2P_MIRACAST_MODE_CHANGED:
            mHost->SetScanIntevelOnMiracastModeChange(arg1);
            break;
        case CMD_SCREEN_STATE_CHANGED:
            mHost->HandleScreenStateChanged(arg1 != 0,
                    /* startBackgroundScanIfNeeded = */ TRUE);
            break;
        default:
            ret = NOT_HANDLED;
    }
    *result = ret;
    return NOERROR;
}

ECode WifiStateMachine::DisconnectedState::Exit()
{
    /* No need for a background scan upon exit from a disconnected state */
    if (mHost->mEnableBackgroundScan) {
        mHost->mWifiNative->EnableBackgroundScan(FALSE);
    }
    mHost->mCurrentScanAlarmMs = 0;
    mHost->SetScanAlarm(FALSE, 0);
    return NOERROR;
}

String WifiStateMachine::DisconnectedState::GetName()
{
    return String("DisconnectedState");
}

//==============================================================================
//  WifiStateMachine::WpsRunningState
//==============================================================================

ECode WifiStateMachine::WpsRunningState::Enter()
{
    AutoPtr<IMessageHelper> helper;
    CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
    helper->Obtain(mHost->GetCurrentMessage(), (IMessage**)&mSourceMessage);
    mHost->mWpsNetworkId = -1;
    mHost->mWpsSuccess = FALSE;
    return NOERROR;
}

ECode WifiStateMachine::WpsRunningState::ProcessMessage(
    /* [in] */ IMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    mHost->LogStateAndMessage(message, String("WifiStateMachine::WpsRunningState"));

    Int32 what;
    message->GetWhat(&what);
    switch (what) {
        case WifiMonitor::WPS_SUCCESS_EVENT:
            // Ignore intermediate success, wait for full connection
            if (mHost->mWifiConfigStore->enableAutoJoinWhenAssociated) {
                mHost->mWpsSuccess = TRUE;
            }
            break;
        case CMD_TARGET_SSID: {
            /* Trying to associate to this SSID */
            AutoPtr<IInterface> obj;
            message->GetObj((IInterface**)&obj);
            if (mHost->mWpsSuccess && obj != NULL) {
                String SSID;
                ICharSequence::Probe(obj)->ToString(&SSID);
                mHost->mWifiConfigStore->GetNetworkIdFromSsid(SSID, &mHost->mWpsNetworkId);
                /* get only once network id , ignore next connect */
                mHost->mWpsSuccess = FALSE;
            }
            break;
        }
        case WifiMonitor::NETWORK_CONNECTION_EVENT:
            mHost->ReplyToMessage(mSourceMessage, IWifiManager::WPS_COMPLETED);
            mSourceMessage->Recycle();
            mSourceMessage = NULL;
            mHost->DeferMessage(message);
            mHost->TransitionTo(mHost->mDisconnectedState);
            break;
        case WifiMonitor::WPS_OVERLAP_EVENT:
            mHost->ReplyToMessage(mSourceMessage, IWifiManager::WPS_FAILED,
                    IWifiManager::WPS_OVERLAP_ERROR);
            mSourceMessage->Recycle();
            mSourceMessage = NULL;
            mHost->TransitionTo(mHost->mDisconnectedState);
            break;
        case WifiMonitor::WPS_FAIL_EVENT: {
            Int32 arg1;
            message->GetArg1(&arg1);
            Int32 arg2;
            message->GetArg2(&arg2);
            // Arg1 has the reason for the failure
            if ((arg1 != IWifiManager::ERROR) || (arg2 != 0)) {
                mHost->ReplyToMessage(mSourceMessage, IWifiManager::WPS_FAILED, arg1);
                mSourceMessage->Recycle();
                mSourceMessage = NULL;
                mHost->TransitionTo(mHost->mDisconnectedState);
            }
            else {
                if (DBG) mHost->Log(String("Ignore unspecified fail event during WPS connection"));
            }
            break;
        }
        case WifiMonitor::WPS_TIMEOUT_EVENT:
            mHost->ReplyToMessage(mSourceMessage, IWifiManager::WPS_FAILED,
                    IWifiManager::WPS_TIMED_OUT);
            mSourceMessage->Recycle();
            mSourceMessage = NULL;
            mHost->TransitionTo(mHost->mDisconnectedState);
            break;
        case IWifiManager::START_WPS:
            mHost->ReplyToMessage(message, IWifiManager::WPS_FAILED, IWifiManager::IN_PROGRESS);
            break;
        case IWifiManager::CANCEL_WPS:
            if (mHost->mWifiNative->CancelWps()) {
                mHost->ReplyToMessage(message, IWifiManager::CANCEL_WPS_SUCCEDED);
            }
            else {
                mHost->ReplyToMessage(message, IWifiManager::CANCEL_WPS_FAILED, IWifiManager::ERROR);
            }
            mHost->TransitionTo(mHost->mDisconnectedState);
            break;
        /**
         * Defer all commands that can cause connections to a different network
         * or put the state machine out of connect mode
         */
        case CMD_STOP_DRIVER:
        case CMD_SET_OPERATIONAL_MODE:
        case IWifiManager::CONNECT_NETWORK:
        case CMD_ENABLE_NETWORK:
        case CMD_RECONNECT:
        case CMD_REASSOCIATE:
            mHost->DeferMessage(message);
            break;
        case CMD_AUTO_CONNECT:
        case CMD_AUTO_ROAM:
            mHost->messageHandlingStatus = MESSAGE_HANDLING_STATUS_DISCARD;
            *result = HANDLED;
            return NOERROR;
        case CMD_START_SCAN:
            mHost->messageHandlingStatus = MESSAGE_HANDLING_STATUS_DISCARD;
            *result = HANDLED;
            return NOERROR;
        case WifiMonitor::NETWORK_DISCONNECTION_EVENT: {
            Int32 arg2;
            message->GetArg2(&arg2);
            if (DBG) {
                mHost->Loge(String("Network connection lost reason = ")
                      + arg2);
            }
            mHost->HandleNetworkDisconnect();
            if (mHost->mWpsNetworkId >= 0 && arg2 != mHost->reason3) {
                mHost->mWifiNative->EnableNetwork(mHost->mWpsNetworkId, TRUE);
            }
            break;
        }
        case WifiMonitor::ASSOCIATION_REJECTION_EVENT:
            if (DBG) mHost->Log(String("Ignore Assoc reject event during WPS Connection"));
            break;
        case WifiMonitor::AUTHENTICATION_FAILURE_EVENT:
            // Disregard auth failure events during WPS connection. The
            // EAP sequence is retried several times, and there might be
            // failures (especially for wps pin). We will get a WPS_XXX
            // event at the end of the sequence anyway.
            if (DBG) mHost->Log(String("Ignore auth failure during WPS connection"));
            break;
        case WifiMonitor::SUPPLICANT_STATE_CHANGE_EVENT:
            // Throw away supplicant state changes when WPS is running.
            // We will start getting supplicant state changes once we get
            // a WPS success or failure
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
    mHost->mWifiConfigStore->EnableAllNetworks();
    mHost->mWifiConfigStore->LoadConfiguredNetworks();
    return NOERROR;
}

String WifiStateMachine::WpsRunningState::GetName()
{
    return String("WpsRunningState");
}

//==============================================================================
//  WifiStateMachine::SoftApStartingState
//==============================================================================

ECode WifiStateMachine::SoftApStartingState::Enter()
{
    AutoPtr<IMessage> message = mHost->GetCurrentMessage();
    Int32 what;
    message->GetWhat(&what);
    if (what == CMD_START_AP) {
        AutoPtr<IInterface> obj;
        message->GetObj((IInterface**)&obj);
        AutoPtr<IWifiConfiguration> config = IWifiConfiguration::Probe(obj);

        if (config == NULL) {
            mHost->mWifiApConfigChannel->SendMessage(CMD_REQUEST_AP_CONFIG);
        }
        else {
            mHost->mWifiApConfigChannel->SendMessage(CMD_SET_AP_CONFIG, config);
            mHost->StartSoftApWithConfig(config);
        }
    }
    else {
        // throw new RuntimeException("Illegal transition to SoftApStartingState: " + message);
        return E_RUNTIME_EXCEPTION;
    }
    return NOERROR;
}

ECode WifiStateMachine::SoftApStartingState::ProcessMessage(
    /* [in] */ IMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    mHost->LogStateAndMessage(message, String("WifiStateMachine::SoftApStartingState"));

    Int32 what;
    message->GetWhat(&what);
    switch (what) {
        case CMD_START_SUPPLICANT:
        case CMD_STOP_SUPPLICANT:
        case CMD_START_AP:
        case CMD_STOP_AP:
        case CMD_START_DRIVER:
        case CMD_STOP_DRIVER:
        case CMD_SET_OPERATIONAL_MODE:
        case CMD_SET_COUNTRY_CODE:
        case CMD_SET_FREQUENCY_BAND:
        case CMD_START_PACKET_FILTERING:
        case CMD_STOP_PACKET_FILTERING:
        case CMD_TETHER_STATE_CHANGE:
            mHost->DeferMessage(message);
            break;
        case WifiStateMachine::CMD_RESPONSE_AP_CONFIG: {
            AutoPtr<IInterface> obj;
            message->GetObj((IInterface**)&obj);
            AutoPtr<IWifiConfiguration> config = IWifiConfiguration::Probe(obj);
            if (config != NULL) {
                mHost->StartSoftApWithConfig(config);
            }
            else {
                mHost->Loge(String("Softap config is NULL!"));
                mHost->SendMessage(CMD_START_AP_FAILURE);
            }
            break;
        }
        case CMD_START_AP_SUCCESS:
            mHost->SetWifiApState(IWifiManager::WIFI_AP_STATE_ENABLED);
            mHost->TransitionTo(mHost->mSoftApStartedState);
            break;
        case CMD_START_AP_FAILURE:
            mHost->SetWifiApState(IWifiManager::WIFI_AP_STATE_FAILED);
            mHost->TransitionTo(mHost->mInitialState);
            break;
        default:
            *result = NOT_HANDLED;
            return NOERROR;
    }
    *result = HANDLED;
    return NOERROR;
}

String WifiStateMachine::SoftApStartingState::GetName()
{
    return String("SoftApStartingState");
}

//==============================================================================
//  WifiStateMachine::SoftApStartedState
//==============================================================================

ECode WifiStateMachine::SoftApStartedState::ProcessMessage(
    /* [in] */ IMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    mHost->LogStateAndMessage(message, String("WifiStateMachine::SoftApStartedState"));

    Int32 what;
    message->GetWhat(&what);
    switch (what) {
        case CMD_STOP_AP:
            if (DBG) mHost->Log(String("Stopping Soft AP"));
            /* We have not tethered at this point, so we just shutdown soft Ap */
            // try {
            mHost->mNwService->StopAccessPoint(mHost->mInterfaceName);
            // } catch(Exception e) {
            //     mHost->Loge("Exception in stopAccessPoint()");
            // }
            mHost->SetWifiApState(IWifiManager::WIFI_AP_STATE_DISABLED);
            mHost->TransitionTo(mHost->mInitialState);
            break;
        case CMD_START_AP:
            // Ignore a start on a running access point
            break;
            // Fail client mode operation when soft AP is enabled
        case CMD_START_SUPPLICANT:
            mHost->Loge(String("Cannot start supplicant with a running soft AP"));
            mHost->SetWifiState(IWifiManager::WIFI_STATE_UNKNOWN);
            break;
        case CMD_TETHER_STATE_CHANGE: {
            AutoPtr<IInterface> obj;
            message->GetObj((IInterface**)&obj);
            AutoPtr<TetherStateChange> stateChange = (TetherStateChange*)(IObject*)obj.Get();
            if (mHost->StartTethering(stateChange->available)) {
                mHost->TransitionTo(mHost->mTetheringState);
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

String WifiStateMachine::SoftApStartedState::GetName()
{
    return String("SoftApStartedState");
}

//==============================================================================
//  WifiStateMachine::TetheringState
//==============================================================================

ECode WifiStateMachine::TetheringState::Enter()
{
    /* Send ourselves a delayed message to shut down if tethering fails to notify */
    AutoPtr<IMessage> msg;
    mHost->ObtainMessage(CMD_TETHER_NOTIFICATION_TIMED_OUT, ++mHost->mTetherToken, 0, (IMessage**)&msg);
    mHost->SendMessageDelayed(msg, TETHER_NOTIFICATION_TIME_OUT_MSECS);
    return NOERROR;
}

ECode WifiStateMachine::TetheringState::ProcessMessage(
    /* [in] */ IMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    mHost->LogStateAndMessage(message, String("WifiStateMachine::TetheringState"));

    Int32 what;
    message->GetWhat(&what);
    switch (what) {
        case CMD_TETHER_STATE_CHANGE: {
            AutoPtr<IInterface> obj;
            message->GetObj((IInterface**)&obj);
            AutoPtr<TetherStateChange> stateChange = (TetherStateChange*)(IObject*)obj.Get();
            if (mHost->IsWifiTethered(stateChange->active)) {
                mHost->TransitionTo(mHost->mTetheredState);
            }
            *result = HANDLED;
            return NOERROR;
        }
        case CMD_TETHER_NOTIFICATION_TIMED_OUT: {
            Int32 arg1;
            message->GetArg1(&arg1);
            if (arg1 == mHost->mTetherToken) {
                mHost->Loge(String("Failed to get tether update, shutdown soft access point"));
                mHost->TransitionTo(mHost->mSoftApStartedState);
                mHost->SetWifiApState(IWifiManager::WIFI_AP_STATE_FAILED);
                // Needs to be first thing handled
                mHost->SendMessageAtFrontOfQueue(CMD_STOP_AP);
            }
            break;
        }
        case CMD_START_SUPPLICANT:
        case CMD_STOP_SUPPLICANT:
        case CMD_START_AP:
        case CMD_STOP_AP:
        case CMD_START_DRIVER:
        case CMD_STOP_DRIVER:
        case CMD_SET_OPERATIONAL_MODE:
        case CMD_SET_COUNTRY_CODE:
        case CMD_SET_FREQUENCY_BAND:
        case CMD_START_PACKET_FILTERING:
        case CMD_STOP_PACKET_FILTERING:
            mHost->DeferMessage(message);
            break;
        default:
            *result = NOT_HANDLED;
            return NOERROR;
    }
    *result = HANDLED;
    return NOERROR;
}

String WifiStateMachine::TetheringState::GetName()
{
    return String("TetheringState");
}

//==============================================================================
//  WifiStateMachine::TetheredState
//==============================================================================

ECode WifiStateMachine::TetheredState::ProcessMessage(
    /* [in] */ IMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    mHost->LogStateAndMessage(message, String("WifiStateMachine::TetheredState"));

    Int32 what;
    message->GetWhat(&what);
    switch (what) {
        case CMD_TETHER_STATE_CHANGE: {
            AutoPtr<IInterface> obj;
            message->GetObj((IInterface**)&obj);
            AutoPtr<TetherStateChange> stateChange = (TetherStateChange*)(IObject*)obj.Get();
            if (!mHost->IsWifiTethered(stateChange->active)) {
                mHost->Loge(String("Tethering reports wifi as untethered!, shut down soft Ap"));
                mHost->SetHostApRunning(NULL, FALSE);
                mHost->SetHostApRunning(NULL, TRUE);
            }
            *result = HANDLED;
            return NOERROR;
        }
        case CMD_STOP_AP:
            if (DBG) mHost->Log(String("Untethering before stopping AP"));
            mHost->SetWifiApState(IWifiManager::WIFI_AP_STATE_DISABLING);
            mHost->StopTethering();
            mHost->TransitionTo(mHost->mUntetheringState);
            // More work to do after untethering
            mHost->DeferMessage(message);
            break;
        default:
            *result = NOT_HANDLED;
            return NOERROR;
    }
    *result = HANDLED;
    return NOERROR;
}

String WifiStateMachine::TetheredState::GetName()
{
    return String("TetheredState");
}

//==============================================================================
//  WifiStateMachine::UntetheringState
//==============================================================================

ECode WifiStateMachine::UntetheringState::Enter()
{
    /* Send ourselves a delayed message to shut down if tethering fails to notify */
    AutoPtr<IMessage> msg;
    mHost->ObtainMessage(CMD_TETHER_NOTIFICATION_TIMED_OUT, ++mHost->mTetherToken, 0, (IMessage**)&msg);
    mHost->SendMessageDelayed(msg, TETHER_NOTIFICATION_TIME_OUT_MSECS);
    return NOERROR;
}

ECode WifiStateMachine::UntetheringState::ProcessMessage(
    /* [in] */ IMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    mHost->LogStateAndMessage(message, String("WifiStateMachine::UntetheringState"));

    Int32 what;
    message->GetWhat(&what);
    switch (what) {
        case CMD_TETHER_STATE_CHANGE: {
            AutoPtr<IInterface> obj;
            message->GetObj((IInterface**)&obj);
            AutoPtr<TetherStateChange> stateChange = (TetherStateChange*)(IObject*)obj.Get();

            /* Wait till wifi is untethered */
            if (mHost->IsWifiTethered(stateChange->active)) break;

            mHost->TransitionTo(mHost->mSoftApStartedState);
            break;
        }
        case CMD_TETHER_NOTIFICATION_TIMED_OUT: {
            Int32 arg1;
            message->GetArg1(&arg1);
            if (arg1 == mHost->mTetherToken) {
                mHost->Loge(String("Failed to get tether update, force stop access point"));
                mHost->TransitionTo(mHost->mSoftApStartedState);
            }
            break;
        }
        case CMD_START_SUPPLICANT:
        case CMD_STOP_SUPPLICANT:
        case CMD_START_AP:
        case CMD_STOP_AP:
        case CMD_START_DRIVER:
        case CMD_STOP_DRIVER:
        case CMD_SET_OPERATIONAL_MODE:
        case CMD_SET_COUNTRY_CODE:
        case CMD_SET_FREQUENCY_BAND:
        case CMD_START_PACKET_FILTERING:
        case CMD_STOP_PACKET_FILTERING:
            mHost->DeferMessage(message);
            break;
        default:
            *result = NOT_HANDLED;
            return NOERROR;
    }
    *result = HANDLED;
    return NOERROR;
 }

String WifiStateMachine::UntetheringState::GetName()
{
    return String("UntetheringState");
}

//==============================================================================
//  WifiStateMachine::TetherStateChange
//==============================================================================

WifiStateMachine::TetherStateChange::TetherStateChange(
    /* [in] */ IArrayList* av,
    /* [in] */ IArrayList* ac)
    : available(av)
    , active(ac)
{
}

//==============================================================================
//  WifiStateMachine::WifiNetworkFactory
//==============================================================================

WifiStateMachine::WifiNetworkFactory::WifiNetworkFactory(
    /* [in] */ ILooper* l,
    /* [in] */ IContext* c,
    /* [in] */ const String& TAG,
    /* [in] */ INetworkCapabilities* f)
    : NetworkFactory()
{
    constructor(l, c, TAG, f);
}

void WifiStateMachine::WifiNetworkFactory::StartNetwork()
{
    // TODO
    // Enter association mode.
}

void WifiStateMachine::WifiNetworkFactory::StopNetwork()
{
    // TODO
    // Stop associating.
}

//==============================================================================
//  WifiStateMachine::WifiNetworkAgent
//==============================================================================

WifiStateMachine::WifiNetworkAgent::WifiNetworkAgent(
    /* [in] */ ILooper* l,
    /* [in] */ IContext* c,
    /* [in] */ const String& TAG,
    /* [in] */ INetworkInfo* ni,
    /* [in] */ INetworkCapabilities* nc,
    /* [in] */ ILinkProperties* lp,
    /* [in] */ Int32 score,
    /* [in] */ WifiStateMachine* host)
    : NetworkAgent()
    , mHost(host)
{
    constructor(l, c, TAG, ni, nc, lp, score);
}

ECode WifiStateMachine::WifiNetworkAgent::Unwanted()
{
    // Ignore if we're not the current networkAgent.
    if (this != mHost->mNetworkAgent) return NOERROR;
    Int32 score;
    mHost->mWifiInfo->GetScore(&score);
    if (mHost->DBG) mHost->Log(String("WifiNetworkAgent -> Wifi unwanted score ")
            + StringUtils::ToString(score));
    mHost->UnwantedNetwork(network_status_unwanted_disconnect);
    return NOERROR;
}

void WifiStateMachine::WifiNetworkAgent::NetworkStatus(
    /* [in] */ Int32 status)
{
    if (status == INetworkAgent::INVALID_NETWORK) {
        Int32 score;
        mHost->mWifiInfo->GetScore(&score);
        if (mHost->DBG) mHost->Log(String("WifiNetworkAgent -> Wifi networkStatus invalid score ")
                + StringUtils::ToString(score));
        mHost->UnwantedNetwork(network_status_unwanted_disable_autojoin);
    }
}

//==============================================================================
//  WifiStateMachine::StartSoftApWithConfigRunnable
//==============================================================================

ECode WifiStateMachine::StartSoftApWithConfigRunnable::Run()
{
    // try {
    mHost->mNwService->StartAccessPoint(mConfig, mHost->mInterfaceName);
    // } catch (Exception e) {
    //     Loge("Exception in softap start " + e);
    //     try {
    //         mNwService.stopAccessPoint(mInterfaceName);
    //         mNwService.startAccessPoint(config, mInterfaceName);
    //     } catch (Exception e1) {
    //         Loge("Exception in softap re-start " + e1);
    //         sendMessage(CMD_START_AP_FAILURE);
    //         return;
    //     }
    // }
    if (DBG) mHost->Log(String("Soft AP start successful"));
    mHost->SendMessage(CMD_START_AP_SUCCESS);
    return NOERROR;
}

//==============================================================================
//  WifiStateMachine::NetlinkTrackerCallback
//==============================================================================

CAR_INTERFACE_IMPL(WifiStateMachine::NetlinkTrackerCallback, Object, INetlinkTrackerCallback)

ECode WifiStateMachine::NetlinkTrackerCallback::Update(
    /* [in] */ ILinkProperties* lp)
{
    return mHost->SendMessage(CMD_UPDATE_LINKPROPERTIES, lp);
}

//==============================================================================
//  WifiStateMachine::TetherStateChangedBroadcastReceiver
//==============================================================================

ECode WifiStateMachine::TetherStateChangedBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    AutoPtr<IArrayList> available;
    intent->GetStringArrayListExtra(
            IConnectivityManager::EXTRA_AVAILABLE_TETHER, (IArrayList**)&available);
    AutoPtr<IArrayList> active;
    intent->GetStringArrayListExtra(
            IConnectivityManager::EXTRA_ACTIVE_TETHER, (IArrayList**)&active);
    AutoPtr<TetherStateChange> tsc = new TetherStateChange(available, active);
    mHost->SendMessage(CMD_TETHER_STATE_CHANGE, (IInterface*)(IObject*)tsc);
    return NOERROR;
}

//==============================================================================
//  WifiStateMachine::StartScanBroadcastReceiver
//==============================================================================

ECode WifiStateMachine::StartScanBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    mHost->sScanAlarmIntentCount++;
    mHost->StartScan(SCAN_ALARM_SOURCE, -2, NULL, NULL);
    if (VDBG) {
        mHost->Loge(String("WiFiStateMachine SCAN ALARM"));
    }
    return NOERROR;
}

//==============================================================================
//  WifiStateMachine::MyBroadcastReceiver
//==============================================================================

ECode WifiStateMachine::MyBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);

    if (action.Equals(IIntent::ACTION_SCREEN_ON)) {
        mHost->SendMessage(CMD_SCREEN_STATE_CHANGED, 1);
    }
    else if (action.Equals(IIntent::ACTION_SCREEN_OFF)) {
        mHost->SendMessage(CMD_SCREEN_STATE_CHANGED, 0);
    }
    else if (action.Equals(ACTION_REFRESH_BATCHED_SCAN)) {
        mHost->StartNextBatchedScanAsync();
    }
    else if (action.Equals("com.quicinc.cne.CNE_PREFERENCE_CHANGED")) {
        Int32 featureId;
        intent->GetInt32Extra(EXTRA_FEATURE_ID, -1, &featureId);
        Int32 featureParam;
        intent->GetInt32Extra(EXTRA_FEATURE_PARAMETER, -1, &featureParam);
        Int32 featureVal;
        intent->GetInt32Extra(EXTRA_PARAMETER_VALUE, -1, &featureVal);
        mHost->HandlePrefChange(featureId, featureParam, featureVal);
    }
    else if (action.Equals("prop_state_change")) {
        Int32 state;
        intent->GetInt32Extra(String("state"), 0, &state);
        mHost->HandleStateChange(state);
    }
    else if (action.Equals("blacklist_bad_bssid") ) {
        // 1 = blacklist, 0 = unblacklist
        Int32 blacklist;
        intent->GetInt32Extra(String("blacklistBSSID"), -1, &blacklist);
        String bssid;
        intent->GetStringExtra(String("BSSIDToBlacklist"), &bssid);
        Int32 reason;
        intent->GetInt32Extra(String("blacklistReason"), -1 , &reason);
        mHost->HandleBSSIDBlacklist( ( blacklist == 0) ? TRUE : FALSE, bssid, reason );
    }
    return NOERROR;
}

//==============================================================================
//  WifiStateMachine::DelayedDriverStopBroadcastReceiver
//==============================================================================

ECode WifiStateMachine::DelayedDriverStopBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    Int32 counter;
    intent->GetInt32Extra(DELAYED_STOP_COUNTER, 0, &counter);
    mHost->SendMessage(CMD_DELAYED_STOP_DRIVER, counter, 0);
    return NOERROR;
}

//==============================================================================
//  WifiStateMachine::BootCompletedBroadcastReceiver
//==============================================================================

ECode WifiStateMachine::BootCompletedBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    return mHost->SendMessage(CMD_BOOT_COMPLETED);
}

//==============================================================================
//  WifiStateMachine::RegisterContentObserver
//==============================================================================

ECode WifiStateMachine::RegisterContentObserver::constructor(
    /* [in] */ IHandler* handler,
    /* [in] */ WifiStateMachine* host)
{
    mHost = host;
    return ContentObserver::constructor(handler);
}

ECode WifiStateMachine::RegisterContentObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    AutoPtr<IContentResolver> cr;
    mHost->mContext->GetContentResolver((IContentResolver**)&cr);
    Int32 val;
    Settings::Global::GetInt32(
            cr, ISettingsGlobal::WIFI_SUSPEND_OPTIMIZATIONS_ENABLED, 1, &val);
    mHost->mUserWantsSuspendOpt->Set(val == 1);
    return NOERROR;
}

//==============================================================================
//  WifiStateMachine
//==============================================================================

WifiStateMachine::WifiStateMachine(
    /* [in] */ IContext* context,
    /* [in] */ const String& wlanInterface,
    /* [in] */ WifiTrafficPoller* trafficPoller)
    : StateMachine(String("WifiStateMachine"))
    , obtainingIpWatchdogCount(0)
    , roamWatchdogCount(0)
    , disconnectingWatchdogCount(0)
    , mRunningBeaconCount(0)
    , emptyScanResultCount(0)
    , mBadLinkspeedcount(0)
    , didBlackListBSSID(FALSE)
    , mP2pSupported(FALSE)
    , mIbssSupported(FALSE)
    , mTemporarilyDisconnectWifi(FALSE)
    , mNumScanResultsKnown(0)
    , mNumScanResultsReturned(0)
    , mWpsNetworkId(0)
    , mWpsSuccess(FALSE)
    , reason3(3)
    , mBatchedScanOwnerUid(UNKNOWN_SCAN_SOURCE)
    , mExpectedBatchedScans(0)
    , mBatchedScanMinPollTime(0)
    , mScreenOn(FALSE)
    , mBackgroundScanSupported(FALSE)
    , mBackgroundScanConfigured(FALSE)
    , mLastSignalLevel(-1)
    , mLastNetworkId(0)
    , linkDebouncing(FALSE)
    , testNetworkDisconnect(FALSE)
    , mEnableRssiPolling(FALSE)
    , mEnableBackgroundScan(FALSE)
    , mDisabled5GhzFrequencies(FALSE)
    , mRssiPollToken(0)
    , mOperationalMode(CONNECT_MODE)
    , mIsScanOngoing(FALSE)
    , mIsFullScanOngoing(FALSE)
    , mBluetoothConnectionActive(FALSE)
    , mSupplicantRestartCount(0)
    , mSupplicantStopFailureToken(0)
    , mTetherToken(0)
    , mDriverStartToken(0)
    , mPeriodicScanToken(0)
    , mPnoPeriodicScanToken(0)
    , mDhcpActive(FALSE)
    , mWifiLinkLayerStatsSupported(4)
    , mAutoRoaming(WifiAutoJoinController::AUTO_JOIN_IDLE)
    , mRoamFailCount(0)
    , testNetworkDisconnectCounter(0)
    , mSuspendOptNeedsDisabled(0)
    , mDefaultFrameworkScanIntervalMs(0)
    , mDisconnectedScanPeriodMs(10000)
    , mSupplicantScanIntervalMs(0)
    , lastFullBandConnectedTimeMilli(0)
    , fullBandConnectedTimeIntervalMilli(0)
    , mLastEnableAllNetworksTime(0)
    , mDriverStopDelayMs(0)
    , mDelayedStopCounter(0)
    , mInDelayedStop(FALSE)
    , mIsRunning(FALSE)
    , mReportedRunning(FALSE)
    , mBatchedScanCsph(0)
    , mNotedBatchedScanCsph(0)
    , isPropFeatureEnabled(FALSE)
    , mVerboseLoggingLevel(0)
    , mAggressiveHandover(0)
    , mAlarmEnabled(FALSE)
    , mFrameworkScanIntervalMs(10000)
    , mCurrentScanAlarmMs(10000)
    , mDisconnectedTimeStamp(0)
    , lastStartScanTimeStamp(0)
    , lastScanDuration(0)
    , lastConnectAttempt(0)
    , messageHandlingStatus(0)
    , mOnTime(0)
    , mTxTime(0)
    , mRxTime(0)
    , mOnTimeStartScan(0)
    , mTxTimeStartScan(0)
    , mRxTimeStartScan(0)
    , mOnTimeScan(0)
    , mTxTimeScan(0)
    , mRxTimeScan(0)
    , mOnTimeThisScan(0)
    , mTxTimeThisScan(0)
    , mRxTimeThisScan(0)
    , mOnTimeScreenStateChange(0)
    , mOnTimeAtLastReport(0)
    , lastOntimeReportTimeStamp(0)
    , lastScreenStateChangeTimeStamp(0)
    , mOnTimeLastReport(0)
    , mTxTimeLastReport(0)
    , mRxTimeLastReport(0)
    , lastLinkLayerStatsUpdate(0)
{
    CAtomicBoolean::New(FALSE, (IAtomicBoolean**)&mP2pConnected);
    CArrayList::New((IArrayList**)&mScanResults);
    CArrayList::New((IArrayList**)&mBatchedScanResults);
    CLinkedList::New((ILinkedList**)&mBufferedScanMsg);
    CAtomicBoolean::New(FALSE, (IAtomicBoolean**)&mScreenBroadcastReceived);
    CAtomicInteger32::New((IAtomicInteger32**)&mCountryCodeSequence);
    mTargetRoamBSSID = String("any");
    CAtomicInteger32::New(IWifiManager::WIFI_FREQUENCY_BAND_AUTO, (IAtomicInteger32**)&mFrequencyBand);
    CAtomicBoolean::New(FALSE, (IAtomicBoolean**)&mFilteringMulticastV4Packets);
    CAsyncChannel::New((IAsyncChannel**)&mReplyChannel);
    CWifiConnectionStatistics::New((IWifiConnectionStatistics**)&mWifiConnectionStatistics);
    CNetworkCapabilities::New((INetworkCapabilities**)&mNetworkCapabilitiesFilter);
    CAtomicBoolean::New(FALSE, (IAtomicBoolean**)&mUserWantsSuspendOpt);
    mDefaultState = new DefaultState(this);
    mInitialState = new InitialState(this);
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
    mConnectedState = new ConnectedState(this);
    mRoamingState = new RoamingState(this);
    mDisconnectingState = new DisconnectingState(this);
    mDisconnectedState = new DisconnectedState(this);
    mWpsRunningState = new WpsRunningState(this);
    mSoftApStartingState = new SoftApStartingState(this);
    mSoftApStartedState = new SoftApStartedState(this);
    mTetheringState = new TetheringState(this);
    mTetheredState = new TetheredState(this);
    mUntetheringState = new UntetheringState(this);
    CAtomicInteger32::New(IWifiManager::WIFI_STATE_DISABLED, (IAtomicInteger32**)&mWifiState);
    CAtomicInteger32::New(IWifiManager::WIFI_AP_STATE_DISABLED, (IAtomicInteger32**)&mWifiApState);
    CWorkSource::New((IWorkSource**)&mRunningWifiUids);
    CWorkSource::New((IWorkSource**)&mLastRunningWifiUids);

    mContext = context;
    mInterfaceName = wlanInterface;
    CNetworkInfo::New(IConnectivityManager::TYPE_WIFI, 0, NETWORKTYPE, String(""), (INetworkInfo**)&mNetworkInfo);

    AutoPtr<IInterface> temp = ServiceManager::GetService(IBatteryStats::SERVICE_NAME);
    mBatteryStats = IIBatteryStats::Probe(temp);

    temp = NULL;
    temp = ServiceManager::GetService(IContext::NETWORKMANAGEMENT_SERVICE);
    mNwService = IINetworkManagementService::Probe(temp);

    AutoPtr<IPackageManager> mgr;
    mContext->GetPackageManager((IPackageManager**)&mgr);
    mgr->HasSystemFeature(IPackageManager::FEATURE_WIFI_DIRECT, &mP2pSupported);

    mWifiNative = new WifiNative(mInterfaceName);
    mWifiConfigStore = new WifiConfigStore(context, mWifiNative);
    mWifiAutoJoinController = new WifiAutoJoinController(context, this,
            mWifiConfigStore, mWifiConnectionStatistics, mWifiNative);
    mWifiMonitor = new WifiMonitor(this, mWifiNative);
    CWifiInfo::New((IWifiInfo**)&mWifiInfo);
    AutoPtr<IHandler> handler;
    GetHandler((IHandler**)&handler);
    mSupplicantStateTracker = new SupplicantStateTracker(context, this, mWifiConfigStore, handler);
    CLinkProperties::New((ILinkProperties**)&mLinkProperties);

    temp = NULL;
    temp = ServiceManager::GetService(IContext::WIFI_P2P_SERVICE);
    mWifiP2pServiceImpl = (WifiP2pServiceImpl*)(IObject*)temp.Get();

    mNetworkInfo->SetIsAvailable(FALSE);
    mLastBssid = NULL;
    mLastNetworkId = IWifiConfiguration::INVALID_NETWORK_ID;
    mLastSignalLevel = -1;

    AutoPtr<NetlinkTrackerCallback> cb = new NetlinkTrackerCallback(this);
    CNetlinkTracker::New(mInterfaceName, cb, (INetlinkTracker**)&mNetlinkTracker);

    // try {
    mNwService->RegisterObserver(IINetworkManagementEventObserver::Probe(mNetlinkTracker));
    // } catch (RemoteException e) {
    //     Loge("Couldn't register netlink tracker: " + e.toString());
    // }

    AutoPtr<IInterface> service;
    context->GetSystemService(IContext::ALARM_SERVICE, (IInterface**)&service);
    mAlarmManager = IAlarmManager::Probe(service);

    AutoPtr<IPendingIntentHelper> helper;
    CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&helper);

    AutoPtr<IIntent> scanIntent;
    CIntent::New(ACTION_START_SCAN, NULL, (IIntent**)&scanIntent);
    helper->GetBroadcast(mContext, SCAN_REQUEST, scanIntent, 0, (IPendingIntent**)&mScanIntent);

    AutoPtr<IIntent> batchedIntent;
    CIntent::New(ACTION_REFRESH_BATCHED_SCAN, NULL, (IIntent**)&batchedIntent);
    helper->GetBroadcast(mContext, 0, batchedIntent, 0, (IPendingIntent**)&mBatchedScanIntervalIntent);

    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    res->GetInteger(R::integer::config_wifi_framework_scan_interval, &mDefaultFrameworkScanIntervalMs);

    res->GetInteger(R::integer::config_wifi_driver_stop_delay, &mDriverStopDelayMs);

    res->GetBoolean(R::bool_::config_wifi_background_scan_support, &mBackgroundScanSupported);

    res->GetString(R::string::config_wifi_p2p_device_type, &mPrimaryDeviceType);

    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    Int32 val;
    Settings::Global::GetInt32(cr,
            ISettingsGlobal::WIFI_SUSPEND_OPTIMIZATIONS_ENABLED, 1, &val);
    mUserWantsSuspendOpt->Set(val == 1);

    mNetworkCapabilitiesFilter->AddTransportType(INetworkCapabilities::TRANSPORT_WIFI);
    mNetworkCapabilitiesFilter->AddCapability(INetworkCapabilities::NET_CAPABILITY_INTERNET);
    mNetworkCapabilitiesFilter->AddCapability(INetworkCapabilities::NET_CAPABILITY_NOT_RESTRICTED);
    mNetworkCapabilitiesFilter->SetLinkUpstreamBandwidthKbps(1024 * 1024);
    mNetworkCapabilitiesFilter->SetLinkDownstreamBandwidthKbps(1024 * 1024);
    // TODO - needs to be a bit more dynamic
    CNetworkCapabilities::New(mNetworkCapabilitiesFilter, (INetworkCapabilities**)&mNetworkCapabilities);

    AutoPtr<TetherStateChangedBroadcastReceiver> tscbr = new TetherStateChangedBroadcastReceiver(this);
    AutoPtr<IIntentFilter> intentFilter;
    CIntentFilter::New(IConnectivityManager::ACTION_TETHER_STATE_CHANGED, (IIntentFilter**)&intentFilter);
    AutoPtr<IIntent> tmpIntent;
    mContext->RegisterReceiver(IBroadcastReceiver::Probe(tscbr), intentFilter, (IIntent**)&tmpIntent);

    AutoPtr<StartScanBroadcastReceiver> ssbr = new StartScanBroadcastReceiver(this);
    intentFilter = NULL;
    CIntentFilter::New(ACTION_START_SCAN, (IIntentFilter**)&intentFilter);
    mContext->RegisterReceiver(IBroadcastReceiver::Probe(ssbr), intentFilter, (IIntent**)&tmpIntent);

    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(IIntent::ACTION_SCREEN_ON);
    filter->AddAction(IIntent::ACTION_SCREEN_OFF);
    filter->AddAction(ACTION_REFRESH_BATCHED_SCAN);

    AutoPtr<ISystemProperties> prop;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&prop);
    prop->GetInt32(String("persist.cne.feature"), 0, &val);

    Boolean isPropFeatureAvail = (val == 3) ? TRUE : FALSE;
    if (isPropFeatureAvail) {
        DEFAULT_SCORE = 1;
        filter->AddAction(String("com.quicinc.cne.CNE_PREFERENCE_CHANGED"));
        filter->AddAction(String("prop_state_change"));
        filter->AddAction(String("blacklist_bad_bssid"));
    }

    AutoPtr<MyBroadcastReceiver> br = new MyBroadcastReceiver(this);
    mContext->RegisterReceiver(IBroadcastReceiver::Probe(br), filter, (IIntent**)&tmpIntent);

    AutoPtr<DelayedDriverStopBroadcastReceiver> ddsbr = new DelayedDriverStopBroadcastReceiver(this);
    filter = NULL;
    CIntentFilter::New(ACTION_DELAYED_DRIVER_STOP, (IIntentFilter**)&filter);
    mContext->RegisterReceiver(ddsbr, filter, (IIntent**)&tmpIntent);

    AutoPtr<IUri> uri;
    Settings::Global::GetUriFor(ISettingsGlobal::WIFI_SUSPEND_OPTIMIZATIONS_ENABLED, (IUri**)&uri);
    AutoPtr<RegisterContentObserver> observer = new RegisterContentObserver();
    observer->constructor(handler, this);
    cr->RegisterContentObserver(uri, FALSE, observer);

    AutoPtr<BootCompletedBroadcastReceiver> bcbr = new BootCompletedBroadcastReceiver(this);
    filter = NULL;
    CIntentFilter::New(IIntent::ACTION_BOOT_COMPLETED, (IIntentFilter**)&filter);
    mContext->RegisterReceiver(bcbr, filter, (IIntent**)&tmpIntent);

    mScanResultCache = new LruCache<String, IScanResult*>(SCAN_RESULT_CACHE_SIZE);

    service = NULL;
    context->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&service);
    AutoPtr<IPowerManager> powerManager = IPowerManager::Probe(service);
    String name;
    GetName(&name);
    powerManager->NewWakeLock(IPowerManager::PARTIAL_WAKE_LOCK, name,
            (IPowerManagerWakeLock**)&mWakeLock);

    powerManager->NewWakeLock(IPowerManager::PARTIAL_WAKE_LOCK,
            String("WifiSuspend"), (IPowerManagerWakeLock**)&mSuspendWakeLock);
    mSuspendWakeLock->SetReferenceCounted(FALSE);

    res->GetString(R::string::config_wifi_tcp_buffers, &mTcpBufferSizes);

    AddState(mDefaultState);
    AddState(mInitialState, mDefaultState);
    AddState(mSupplicantStartingState, mDefaultState);
    AddState(mSupplicantStartedState, mDefaultState);
    AddState(mDriverStartingState, mSupplicantStartedState);
    AddState(mDriverStartedState, mSupplicantStartedState);
    AddState(mScanModeState, mDriverStartedState);
    AddState(mConnectModeState, mDriverStartedState);
    AddState(mL2ConnectedState, mConnectModeState);
    AddState(mObtainingIpState, mL2ConnectedState);
    AddState(mVerifyingLinkState, mL2ConnectedState);
    AddState(mConnectedState, mL2ConnectedState);
    AddState(mRoamingState, mL2ConnectedState);
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
    AddState(mUntetheringState, mSoftApStartedState);

    SetInitialState(mInitialState);

    SetLogRecSize(3000);
    SetLogOnlyTransitions(FALSE);
    if (VDBG) SetDbg(TRUE);

    //start the state machine
    Start();

    AutoPtr<IIntent> intent;
    CIntent::New(IWifiManager::WIFI_SCAN_AVAILABLE, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
    intent->PutExtra(IWifiManager::EXTRA_SCAN_AVAILABLE, IWifiManager::WIFI_STATE_DISABLED);
    mContext->SendStickyBroadcastAsUser(intent, UserHandle::ALL);
}

ECode WifiStateMachine::IsRoaming(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mAutoRoaming == WifiAutoJoinController::AUTO_JOIN_ROAMING
            || mAutoRoaming == WifiAutoJoinController::AUTO_JOIN_EXTENDED_ROAMING;
    return NOERROR;
}

ECode WifiStateMachine::AutoRoamSetBSSID(
    /* [in] */ Int32 netId,
    /* [in] */ const String& bssid)
{
    AutoPtr<IWifiConfiguration> wc;
    mWifiConfigStore->GetWifiConfiguration(netId, (IWifiConfiguration**)&wc);
    Boolean b;
    return AutoRoamSetBSSID(wc, bssid, &b);
}

ECode WifiStateMachine::AutoRoamSetBSSID(
    /* [in] */ IWifiConfiguration* config,
    /* [in] */ const String& _bssid,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    String bssid = _bssid;
    Boolean ret = TRUE;
    if (mTargetRoamBSSID.IsNull()) mTargetRoamBSSID = String("any");
    if (bssid.IsNull()) bssid = String("any");
    if (config == NULL) {
        *result = FALSE; // Nothing to do
        return NOERROR;
    }

    String str;
    config->GetBSSID(&str);
    if (!mTargetRoamBSSID.IsNull() && bssid == mTargetRoamBSSID && bssid.Equals(str)) {
        *result = FALSE; // We didnt change anything
        return NOERROR;
    }
    if (!mTargetRoamBSSID.Equals("any") && bssid.Equals("any")) {
        // Changing to ANY
        if (!mWifiConfigStore->roamOnAny) {
            ret =  FALSE; // Nothing to do
        }
    }
    if (VDBG) {
        config->ConfigKey(&str);
        Loge(String("autoRoamSetBSSID ") + bssid
                + " key=" + str);
    }

    config->SetAutoJoinBSSID(bssid);
    mTargetRoamBSSID = bssid;
    mWifiConfigStore->SaveWifiConfigBSSID(config);
    *result = ret;
    return NOERROR;
}

ECode WifiStateMachine::GetVerboseLoggingLevel(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mVerboseLoggingLevel;
    return NOERROR;
}

ECode WifiStateMachine::EnableVerboseLogging(
    /* [in] */ Int32 verbose)
{
    mVerboseLoggingLevel = verbose;
    if (verbose > 0) {
        DBG = TRUE;
        VDBG = TRUE;
        PDBG = TRUE;
        mLogMessages = TRUE;
        mWifiNative->SetSupplicantLogLevel(String("DEBUG"));
    }
    else {
        DBG = FALSE;
        VDBG = FALSE;
        PDBG = FALSE;
        mLogMessages = FALSE;
        mWifiNative->SetSupplicantLogLevel(String("INFO"));
    }
    mWifiP2pServiceImpl->EnableVerboseLogging(verbose);
    mWifiAutoJoinController->EnableVerboseLogging(verbose);
    mWifiMonitor->EnableVerboseLogging(verbose);
    mWifiNative->EnableVerboseLogging(verbose);
    mWifiConfigStore->EnableVerboseLogging(verbose);
    mSupplicantStateTracker->EnableVerboseLogging(verbose);
    return NOERROR;
}

ECode WifiStateMachine::GetAggressiveHandover(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mAggressiveHandover;
    return NOERROR;
}

ECode WifiStateMachine::EnableAggressiveHandover(
    /* [in] */ Int32 enabled)
{
    mAggressiveHandover = enabled;
    return NOERROR;
}

ECode WifiStateMachine::SetAllowScansWithTraffic(
    /* [in] */ Int32 enabled)
{
    mWifiConfigStore->alwaysEnableScansWhileAssociated = enabled;
    return NOERROR;
}

ECode WifiStateMachine::GetAllowScansWithTraffic(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mWifiConfigStore->alwaysEnableScansWhileAssociated;
    return NOERROR;
}

ECode WifiStateMachine::GetMessenger(
    /* [out] */ IMessenger** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IHandler> handler;
    GetHandler((IHandler**)&handler);
    CMessenger::New(handler, result);
    return NOERROR;
}

ECode WifiStateMachine::GetWifiMonitor(
    /* [out] */ WifiMonitor** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mWifiMonitor;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode WifiStateMachine::SyncPingSupplicant(
    /* [in] */ IAsyncChannel* channel,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IMessage> resultMsg;
    channel->SendMessageSynchronously(CMD_PING_SUPPLICANT, (IMessage**)&resultMsg);
    Int32 arg1;
    resultMsg->GetArg1(&arg1);
    *result = (arg1 != FAILURE);
    resultMsg->Recycle();
    return NOERROR;
}

ECode WifiStateMachine::SyncGetChannelList(
    /* [in] */ IAsyncChannel* channel,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IMessage> resultMsg;
    channel->SendMessageSynchronously(CMD_GET_CAPABILITY_FREQ, (IMessage**)&resultMsg);
    AutoPtr<IArrayList> list;
    AutoPtr<IInterface> obj;
    resultMsg->GetObj((IInterface**)&obj);
    if (obj != NULL) {
        CArrayList::New((IArrayList**)&list);
        String freqs;
        ICharSequence::Probe(obj)->ToString(&freqs);
        AutoPtr<ArrayOf<String> > lines;
        StringUtils::Split(freqs, String("\n"), (ArrayOf<String>**)&lines);

        for (Int32 i = 0; i < lines->GetLength(); i++) {
            String line = (*lines)[i];
            if (line.Contains("MHz")) {
                // line format: " 52 = 5260 MHz (NO_IBSS) (DFS)"
                AutoPtr<IWifiChannel> c;
                CWifiChannel::New((IWifiChannel**)&c);
                AutoPtr<ArrayOf<String> > prop;
                StringUtils::Split(line, String(" "), (ArrayOf<String>**)&prop);
                if (prop->GetLength() < 5) continue;
                // try {
                c->SetChannelNum(StringUtils::ParseInt32((*prop)[1]));
                c->SetFreqMHz(StringUtils::ParseInt32((*prop)[3]));
                // } catch (NumberFormatException e) { }
                c->SetIsDFS(line.Contains("(DFS)"));
                c->SetIbssAllowed(!line.Contains("(NO_IBSS)"));
                list->Add(c);
            }
            else if (line.Contains("Mode[B] Channels:")) {
                // B channels are the same as G channels, skipped
                break;
            }
        }
    }
    resultMsg->Recycle();
    Int32 size;
    *result = (list != NULL && list->GetSize(&size) > 0) ? IList::Probe(list) : NULL;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode WifiStateMachine::StartScan(
    /* [in] */ Int32 callingUid,
    /* [in] */ Int32 scanCounter,
    /* [in] */ IScanSettings* settings,
    /* [in] */ IWorkSource* workSource)
{
    AutoPtr<IBundle> bundle;
    CBundle::New((IBundle**)&bundle);
    bundle->PutParcelable(CUSTOMIZED_SCAN_SETTING, IParcelable::Probe(settings));
    bundle->PutParcelable(CUSTOMIZED_SCAN_WORKSOURCE, IParcelable::Probe(workSource));
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    Int64 currentTimeMillis;
    system->GetCurrentTimeMillis(&currentTimeMillis);
    bundle->PutInt64(SCAN_REQUEST_TIME, currentTimeMillis);
    SendMessage(CMD_START_SCAN, callingUid, scanCounter, bundle);
    return NOERROR;
}

ECode WifiStateMachine::SetBatchedScanSettings(
    /* [in] */ IBatchedScanSettings* settings,
    /* [in] */ Int32 callingUid,
    /* [in] */ Int32 csph,
    /* [in] */ IWorkSource* workSource)
{
    AutoPtr<IBundle> bundle;
    CBundle::New((IBundle**)&bundle);
    bundle->PutParcelable(BATCHED_SETTING, IParcelable::Probe(settings));
    bundle->PutParcelable(BATCHED_WORKSOURCE, IParcelable::Probe(workSource));
    SendMessage(CMD_SET_BATCHED_SCAN, callingUid, csph, bundle);
    return NOERROR;
}

ECode WifiStateMachine::SyncGetBatchedScanResultsList(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    synchronized (mBatchedScanResults) {
        Int32 size;
        mBatchedScanResults->GetSize(&size);
        AutoPtr<IArrayList> batchedScanList;
        CArrayList::New(size, (IArrayList**)&batchedScanList);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            mBatchedScanResults->Get(i, (IInterface**)&obj);
            AutoPtr<IBatchedScanResult> bsr = IBatchedScanResult::Probe(obj);
            AutoPtr<IBatchedScanResult> b;
            CBatchedScanResult::New(bsr, (IBatchedScanResult**)&b);
            batchedScanList->Add(b);
        }
        *result = IList::Probe(batchedScanList);
        REFCOUNT_ADD(*result)
    }
    return NOERROR;
}

ECode WifiStateMachine::RequestBatchedScanPoll()
{
    return SendMessage(CMD_POLL_BATCHED_SCAN);
}

ECode WifiStateMachine::GetDisconnectedTimeMilli(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    if (GetCurrentState() == mDisconnectedState
            && mDisconnectedTimeStamp != 0) {
        AutoPtr<ISystem> system;
        CSystem::AcquireSingleton((ISystem**)&system);
        Int64 currentTimeMillis;
        system->GetCurrentTimeMillis(&currentTimeMillis);
        *result = currentTimeMillis - mDisconnectedTimeStamp;
        return NOERROR;
    }
    *result = 0;
    return NOERROR;
}

ECode WifiStateMachine::ReportOnTime(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    Int64 now;
    system->GetCurrentTimeMillis(&now);
    StringBuilder sb;
    // Report stats since last report
    Int32 on = mOnTime - mOnTimeLastReport;
    mOnTimeLastReport = mOnTime;
    Int32 tx = mTxTime - mTxTimeLastReport;
    mTxTimeLastReport = mTxTime;
    Int32 rx = mRxTime - mRxTimeLastReport;
    mRxTimeLastReport = mRxTime;
    Int32 period = (Int32)(now - lastOntimeReportTimeStamp);
    lastOntimeReportTimeStamp = now;
    String str;
    sb.Append(str.AppendFormat("[on:%d tx:%d rx:%d period:%d]", on, tx, rx, period));
    // Report stats since Screen State Changed
    on = mOnTime - mOnTimeScreenStateChange;
    period = (Int32)(now - lastScreenStateChangeTimeStamp);
    str = String("");
    sb.Append(str.AppendFormat(" from screen [on:%d period:%d]", on, period));
    return sb.ToString(result);
}

ECode WifiStateMachine::GetWifiLinkLayerStats(
    /* [in] */ Boolean dbg,
    /* [out] */ IWifiLinkLayerStats** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IWifiLinkLayerStats> stats;
    if (mWifiLinkLayerStatsSupported > 0) {
        String name = String("wlan0");
        stats = mWifiNative->GetWifiLinkLayerStats(name);
        if (name != NULL && stats == NULL && mWifiLinkLayerStatsSupported > 0) {
            mWifiLinkLayerStatsSupported -= 1;
        }
        else if (stats != NULL) {
            AutoPtr<ISystem> system;
            CSystem::AcquireSingleton((ISystem**)&system);
            Int64 lastLinkLayerStatsUpdate;
            system->GetCurrentTimeMillis(&lastLinkLayerStatsUpdate);
            stats->GetOn_time(&mOnTime);
            stats->GetTx_time(&mTxTime);
            stats->GetRx_time(&mRxTime);
            stats->GetBeacon_rx(&mRunningBeaconCount);
            if (dbg) {
                Loge(String("WifiLinkLayerStats:"));
                String str;
                IObject::Probe(stats)->ToString(&str);
                Loge(str);
            }
        }
    }
    *result = stats;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode WifiStateMachine::StartRadioScanStats()
{
    AutoPtr<IWifiLinkLayerStats> stats;
    GetWifiLinkLayerStats(FALSE, (IWifiLinkLayerStats**)&stats);
    if (stats != NULL) {
        stats->GetOn_time(&mOnTimeStartScan);
        stats->GetTx_time(&mTxTimeStartScan);
        stats->GetRx_time(&mRxTimeStartScan);
        stats->GetOn_time(&mOnTime);
        stats->GetTx_time(&mTxTime);
        stats->GetRx_time(&mRxTime);
    }
    return NOERROR;
}

ECode WifiStateMachine::CloseRadioScanStats()
{
    AutoPtr<IWifiLinkLayerStats> stats;
    GetWifiLinkLayerStats(FALSE, (IWifiLinkLayerStats**)&stats);
    if (stats != NULL) {
        Int32 ot, tt, rt;
        stats->GetOn_time(&ot);
        stats->GetTx_time(&tt);
        stats->GetRx_time(&rt);
        mOnTimeThisScan = ot - mOnTimeStartScan;
        mTxTimeThisScan = tt - mTxTimeStartScan;
        mRxTimeThisScan = rt - mRxTimeStartScan;
        mOnTimeScan += mOnTimeThisScan;
        mTxTimeScan += mTxTimeThisScan;
        mRxTimeScan += mRxTimeThisScan;
    }
    return NOERROR;
}

ECode WifiStateMachine::SetSupplicantRunning(
    /* [in] */ Boolean enable)
{
    if (enable) {
        WifiNative::SetMode(0);
        SendMessage(CMD_START_SUPPLICANT);
    }
    else {
        SendMessage(CMD_STOP_SUPPLICANT);
    }
    return NOERROR;
}

ECode WifiStateMachine::SetHostApRunning(
    /* [in] */ IWifiConfiguration* wifiConfig,
    /* [in] */ Boolean enable)
{
    if (enable) {
        WifiNative::SetMode(1);
        SendMessage(CMD_START_AP, wifiConfig);
    }
    else {
        SendMessage(CMD_STOP_AP);
    }
    return NOERROR;
}

ECode WifiStateMachine::SetWifiApConfiguration(
    /* [in] */ IWifiConfiguration* config)
{
    return mWifiApConfigChannel->SendMessage(CMD_SET_AP_CONFIG, config);
}

ECode WifiStateMachine::SyncGetWifiApConfiguration(
    /* [out] */ IWifiConfiguration** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IMessage> resultMsg;
    mWifiApConfigChannel->SendMessageSynchronously(CMD_REQUEST_AP_CONFIG, (IMessage**)&resultMsg);
    AutoPtr<IInterface> obj;
    resultMsg->GetObj((IInterface**)&obj);
    AutoPtr<IWifiConfiguration> ret = IWifiConfiguration::Probe(obj);
    resultMsg->Recycle();
    *result = ret;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode WifiStateMachine::SyncGetWifiState(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    return mWifiState->Get(result);
}

ECode WifiStateMachine::SyncGetWifiStateByName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 state;
    mWifiState->Get(&state);
    switch (state) {
        case IWifiManager::WIFI_STATE_DISABLING:
            *result = String("disabling");
        case IWifiManager::WIFI_STATE_DISABLED:
            *result = String("disabled");
        case IWifiManager::WIFI_STATE_ENABLING:
            *result = String("enabling");
        case IWifiManager::WIFI_STATE_ENABLED:
            *result = String("enabled");
        case IWifiManager::WIFI_STATE_UNKNOWN:
            *result = String("unknown state");
        default:
            *result = String("[invalid state]");
    }
    return NOERROR;
}

ECode WifiStateMachine::SyncGetWifiApState(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    return mWifiApState->Get(result);
}

ECode WifiStateMachine::SyncGetWifiApStateByName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 state;
    mWifiApState->Get(&state);
    switch (state) {
        case IWifiManager::WIFI_AP_STATE_DISABLING:
            *result = String("disabling");
        case IWifiManager::WIFI_AP_STATE_DISABLED:
            *result = String("disabled");
        case IWifiManager::WIFI_AP_STATE_ENABLING:
            *result = String("enabling");
        case IWifiManager::WIFI_AP_STATE_ENABLED:
            *result = String("enabled");
        case IWifiManager::WIFI_AP_STATE_FAILED:
            *result = String("failed");
        default:
            *result = String("[invalid state]");
    }
    return NOERROR;
}

ECode WifiStateMachine::SyncRequestConnectionInfo(
    /* [out] */ IWifiInfo** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mWifiInfo;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode WifiStateMachine::SyncGetDhcpResults(
    /* [out] */ IDhcpResults** result)
{
    VALIDATE_NOT_NULL(result)
    synchronized (mDhcpResultsLock) {
        CDhcpResults::New(mDhcpResults, (IDhcpResults**)&result);
    }
    return NOERROR;
}

ECode WifiStateMachine::SetDriverStart(
    /* [in] */ Boolean enable)
{
    if (enable) {
        SendMessage(CMD_START_DRIVER);
    }
    else {
        SendMessage(CMD_STOP_DRIVER);
    }
    return NOERROR;
}

ECode WifiStateMachine::SetOperationalMode(
    /* [in] */ Int32 mode)
{
    if (DBG) Log(String("setting operational mode to ") + StringUtils::ToString(mode));
    SendMessage(CMD_SET_OPERATIONAL_MODE, mode, 0);
    return NOERROR;
}

ECode WifiStateMachine::SyncGetScanResultsList(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    synchronized (mScanResultCache) {
        AutoPtr<IArrayList> scanList;
        CArrayList::New((IArrayList**)&scanList);
        Int32 size;
        mScanResults->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            mScanResults->Get(i, (IInterface**)&obj);
            AutoPtr<IScanResult> sr = IScanResult::Probe(obj);
            AutoPtr<IScanResult> s;
            CScanResult::New(sr, (IScanResult**)&s);
            scanList->Add(s);
        }
        *result = IList::Probe(scanList);
        REFCOUNT_ADD(*result)
    }
    return NOERROR;
}

ECode WifiStateMachine::GetScanResultsListNoCopyUnsync(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    *result = IList::Probe(mScanResults);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode WifiStateMachine::DisconnectCommand()
{
    return SendMessage(CMD_DISCONNECT);
}

ECode WifiStateMachine::ReconnectCommand()
{
    return SendMessage(CMD_RECONNECT);
}

ECode WifiStateMachine::ReassociateCommand()
{
    return SendMessage(CMD_REASSOCIATE);
}

ECode WifiStateMachine::ReloadTlsNetworksAndReconnect()
{
    return SendMessage(CMD_RELOAD_TLS_AND_RECONNECT);
}

ECode WifiStateMachine::SyncAddOrUpdateNetwork(
    /* [in] */ IAsyncChannel* channel,
    /* [in] */ IWifiConfiguration* config,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IMessage> resultMsg;
    channel->SendMessageSynchronously(CMD_ADD_OR_UPDATE_NETWORK, config, (IMessage**)&resultMsg);
    resultMsg->GetArg1(result);
    resultMsg->Recycle();
    return NOERROR;
}

ECode WifiStateMachine::SyncGetConfiguredNetworks(
    /* [in] */ Int32 uuid,
    /* [in] */ IAsyncChannel* channel,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IMessage> resultMsg;
    channel->SendMessageSynchronously(CMD_GET_CONFIGURED_NETWORKS, uuid, (IMessage**)&resultMsg);
    AutoPtr<IInterface> obj;
    resultMsg->GetObj((IInterface**)&obj);
    AutoPtr<IList> list = IList::Probe(obj);
    resultMsg->Recycle();
    *result = list;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode WifiStateMachine::SyncGetPrivilegedConfiguredNetwork(
    /* [in] */ IAsyncChannel* channel,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IMessage> resultMsg;
    channel->SendMessageSynchronously(CMD_GET_PRIVILEGED_CONFIGURED_NETWORKS, (IMessage**)&resultMsg);
    AutoPtr<IInterface> obj;
    resultMsg->GetObj((IInterface**)&obj);
    AutoPtr<IList> list = IList::Probe(obj);
    resultMsg->Recycle();
    *result = list;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode WifiStateMachine::SyncGetConnectionStatistics(
    /* [in] */ IAsyncChannel* channel,
    /* [out] */ IWifiConnectionStatistics** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IMessage> resultMsg;
    channel->SendMessageSynchronously(CMD_GET_CONNECTION_STATISTICS, (IMessage**)&resultMsg);
    AutoPtr<IInterface> obj;
    resultMsg->GetObj((IInterface**)&obj);
    AutoPtr<IWifiConnectionStatistics> temp = IWifiConnectionStatistics::Probe(obj);
    resultMsg->Recycle();
    *result = temp;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode WifiStateMachine::SyncGetSupportedFeatures(
    /* [in] */ IAsyncChannel* channel,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IMessage> resultMsg;
    channel->SendMessageSynchronously(CMD_GET_SUPPORTED_FEATURES, (IMessage**)&resultMsg);
    resultMsg->GetArg1(result);
    resultMsg->Recycle();
    return NOERROR;
}

ECode WifiStateMachine::SyncGetLinkLayerStats(
    /* [in] */ IAsyncChannel* channel,
    /* [out] */ IWifiLinkLayerStats** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IMessage> resultMsg;
    channel->SendMessageSynchronously(CMD_GET_LINK_LAYER_STATS, (IMessage**)&resultMsg);
    AutoPtr<IInterface> obj;
    resultMsg->GetObj((IInterface**)&obj);
    AutoPtr<IWifiLinkLayerStats> temp = IWifiLinkLayerStats::Probe(obj);
    resultMsg->Recycle();
    *result = temp;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode WifiStateMachine::SyncRemoveNetwork(
    /* [in] */ IAsyncChannel* channel,
    /* [in] */ Int32 networkId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IMessage> resultMsg;
    channel->SendMessageSynchronously(CMD_REMOVE_NETWORK, networkId, (IMessage**)&resultMsg);
    Int32 arg1;
    resultMsg->GetArg1(&arg1);
    Boolean b = (arg1 != FAILURE);
    resultMsg->Recycle();
    *result = b;
    return NOERROR;
}

ECode WifiStateMachine::SyncEnableNetwork(
    /* [in] */ IAsyncChannel* channel,
    /* [in] */ Int32 netId,
    /* [in] */ Boolean disableOthers,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IMessage> resultMsg;
    channel->SendMessageSynchronously(CMD_ENABLE_NETWORK, netId, disableOthers ? 1 : 0, (IMessage**)&resultMsg);
    Int32 arg1;
    resultMsg->GetArg1(&arg1);
    Boolean b = (arg1 != FAILURE);
    resultMsg->Recycle();
    *result = b;
    return NOERROR;
}

ECode WifiStateMachine::SyncDisableNetwork(
    /* [in] */ IAsyncChannel* channel,
    /* [in] */ Int32 netId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IMessage> resultMsg;
    channel->SendMessageSynchronously(IWifiManager::DISABLE_NETWORK, netId, (IMessage**)&resultMsg);
    Int32 arg1;
    resultMsg->GetArg1(&arg1);
    Boolean b = (arg1 != IWifiManager::DISABLE_NETWORK_FAILED);
    resultMsg->Recycle();
    *result = b;
    return NOERROR;
}

ECode WifiStateMachine::SyncGetWpsNfcConfigurationToken(
    /* [in] */ Int32 netId,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mWifiNative->GetNfcWpsConfigurationToken(netId);
    return NOERROR;
}

ECode WifiStateMachine::AddToBlacklist(
    /* [in] */ const String& bssid)
{
    return SendMessage(CMD_BLACKLIST_NETWORK, CoreUtils::Convert(bssid));
}

ECode WifiStateMachine::ClearBlacklist()
{
    return SendMessage(CMD_CLEAR_BLACKLIST);
}

ECode WifiStateMachine::EnableRssiPolling(
    /* [in] */ Boolean enabled)
{
   return SendMessage(CMD_ENABLE_RSSI_POLL, enabled ? 1 : 0, 0);
}

ECode WifiStateMachine::EnableAllNetworks()
{
    return SendMessage(CMD_ENABLE_ALL_NETWORKS);
}

ECode WifiStateMachine::StartFilteringMulticastV4Packets()
{
    mFilteringMulticastV4Packets->Set(TRUE);
    return SendMessage(CMD_START_PACKET_FILTERING, MULTICAST_V4, 0);
}

ECode WifiStateMachine::StopFilteringMulticastV4Packets()
{
    mFilteringMulticastV4Packets->Set(FALSE);
    return SendMessage(CMD_STOP_PACKET_FILTERING, MULTICAST_V4, 0);
}

ECode WifiStateMachine::StartFilteringMulticastV6Packets()
{
    return SendMessage(CMD_START_PACKET_FILTERING, MULTICAST_V6, 0);
}

ECode WifiStateMachine::StopFilteringMulticastV6Packets()
{
    return SendMessage(CMD_STOP_PACKET_FILTERING, MULTICAST_V6, 0);
}

ECode WifiStateMachine::SetHighPerfModeEnabled(
    /* [in] */ Boolean enable)
{
    return SendMessage(CMD_SET_HIGH_PERF_MODE, enable ? 1 : 0, 0);
}

ECode WifiStateMachine::SetCountryCode(
    /* [in] */ const String& countryCode,
    /* [in] */ Boolean persist)
{
    // If it's a good country code, apply after the current
    // wifi connection is terminated; ignore resetting of code
    // for now (it is unclear what the chipset should do when
    // country code is reset)
    Int32 countryCodeSequence;
    mCountryCodeSequence->IncrementAndGet(&countryCodeSequence);
    if (countryCode.IsEmpty()) {
        Log(String("Ignoring resetting of country code"));
    }
    else {
        SendMessage(CMD_SET_COUNTRY_CODE, countryCodeSequence, persist ? 1 : 0, CoreUtils::Convert(countryCode));
    }
    return NOERROR;
}

ECode WifiStateMachine::SyncIsIbssSupported(
    /* [in] */ IAsyncChannel* channel,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IMessage> resultMsg;
    channel->SendMessageSynchronously(CMD_GET_IBSS_SUPPORTED, (IMessage**)&resultMsg);
    resultMsg->GetArg1(result);
    resultMsg->Recycle();
    return NOERROR;
}

ECode WifiStateMachine::SetFrequencyBand(
    /* [in] */ Int32 band,
    /* [in] */ Boolean persist)
{
    if (persist) {
        AutoPtr<IContentResolver> cr;
        mContext->GetContentResolver((IContentResolver**)&cr);
        Boolean b;
        Settings::Global::PutInt32(cr,
                ISettingsGlobal::WIFI_FREQUENCY_BAND,
                band, &b);
    }
    SendMessage(CMD_SET_FREQUENCY_BAND, band, 0);
    return NOERROR;
}

ECode WifiStateMachine::EnableTdls(
    /* [in] */ const String& remoteMacAddress,
    /* [in] */ Boolean enable)
{
    Int32 enabler = enable ? 1 : 0;
    return SendMessage(CMD_ENABLE_TDLS, enabler, 0, CoreUtils::Convert(remoteMacAddress));
}

ECode WifiStateMachine::GetFrequencyBand(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    return mFrequencyBand->Get(result);
}

ECode WifiStateMachine::GetConfigFile(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    return mWifiConfigStore->GetConfigFile(result);
}

ECode WifiStateMachine::SendBluetoothAdapterStateChange(
    /* [in] */ Int32 state)
{
    return SendMessage(CMD_BLUETOOTH_ADAPTER_STATE_CHANGE, state, 0);
}

ECode WifiStateMachine::SyncSaveConfig(
    /* [in] */ IAsyncChannel* channel,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IMessage> resultMsg;
    channel->SendMessageSynchronously(CMD_SAVE_CONFIG, (IMessage**)&resultMsg);
    Int32 arg1;
    resultMsg->GetArg1(&arg1);
    Boolean b = (arg1 != FAILURE);
    resultMsg->Recycle();
    *result = b;
    return NOERROR;
}

ECode WifiStateMachine::UpdateBatteryWorkSource(
    /* [in] */ IWorkSource* newSource)
{
    synchronized (mRunningWifiUids) {
        // try {
        if (newSource != NULL) {
            mRunningWifiUids->Set(newSource);
        }
        if (mIsRunning) {
            if (mReportedRunning) {
                // If the work source has changed since last time, need
                // to remove old work from battery stats.
                Boolean b;
                if (mLastRunningWifiUids->Diff(mRunningWifiUids, &b), b) {
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
    return NOERROR;
}

ECode WifiStateMachine::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    StateMachine::Dump(fd, pw, args);
    mSupplicantStateTracker->Dump(fd, pw, args);
    String str;
    pw->Println(String("mLinkProperties ") + (IObject::Probe(mLinkProperties)->ToString(&str), str));
    pw->Println(String("mWifiInfo ") + (IObject::Probe(mWifiInfo)->ToString(&str), str));
    pw->Println(String("mDhcpResults ") + (IObject::Probe(mDhcpResults)->ToString(&str), str));
    pw->Println(String("mNetworkInfo ") + (IObject::Probe(mNetworkInfo)->ToString(&str), str));
    pw->Println(String("mLastSignalLevel ") + mLastSignalLevel);
    pw->Println(String("mLastBssid ") + mLastBssid);
    pw->Println(String("mLastNetworkId ") + mLastNetworkId);
    pw->Println(String("mOperationalMode ") + mOperationalMode);
    Boolean b;
    mUserWantsSuspendOpt->Get(&b);
    pw->Println(String("mUserWantsSuspendOpt ") + b);
    pw->Println(String("mSuspendOptNeedsDisabled ") + mSuspendOptNeedsDisabled);
    pw->Println(String("Supplicant status ") + mWifiNative->Status(TRUE));
    pw->Println(String("mEnableBackgroundScan ") + mEnableBackgroundScan);
    pw->Println(String("mLastSetCountryCode ") + mLastSetCountryCode);
    pw->Println(String("mPersistedCountryCode ") + mPersistedCountryCode);
    pw->Println();
    mWifiConfigStore->Dump(fd, pw, args);
    return NOERROR;
}

ECode WifiStateMachine::ShouldSwitchNetwork(
    /* [in] */ Int32 networkDelta,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 delta;
    if (networkDelta <= 0) {
        *result = FALSE;
        return NOERROR;
    }
    delta = networkDelta;
    if (mWifiInfo != NULL) {
        Int32 id;
        mWifiInfo->GetNetworkId(&id);
        if (!mWifiConfigStore->enableAutoJoinWhenAssociated
                && id != IWifiConfiguration::INVALID_NETWORK_ID) {
            // If AutoJoin while associated is not enabled,
            // we should never switch network when already associated
            delta = -1000;
        }
        else {
            // TODO: Look at per AC packet count, do not switch if VO/VI traffic is present
            // TODO: at the interface. We should also discriminate between ucast and mcast,
            // TODO: since the rxSuccessRate include all the bonjour and Ipv6
            // TODO: broadcasts
            Double txSuccessRate, rxSuccessRate;
            mWifiInfo->GetTxSuccessRate(&txSuccessRate);
            mWifiInfo->GetRxSuccessRate(&rxSuccessRate);
            if ((txSuccessRate > 20) || (rxSuccessRate > 80)) {
                delta -= 999;
            }
            else if ((txSuccessRate > 5) || (rxSuccessRate > 30)) {
                delta -= 6;
            }
            Loge(String("WifiStateMachine shouldSwitchNetwork ")
                    + " txSuccessRate=" + StringUtils::ToString(txSuccessRate)
                    + " rxSuccessRate=" + StringUtils::ToString(rxSuccessRate)
                    + " delta " + networkDelta + " -> " + delta);
        }
    }
    else {
        Loge(String("WifiStateMachine shouldSwitchNetwork ")
                + " delta " + networkDelta + " -> " + delta);
    }
    if (delta > 0) {
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode WifiStateMachine::GetTxPacketRate(
    /* [out] */ Double* result)
{
    VALIDATE_NOT_NULL(result)
    if (mWifiInfo != NULL) {
        return mWifiInfo->GetTxSuccessRate(result);
    }
    *result = -1;
    return NOERROR;
}

ECode WifiStateMachine::GetRxPacketRate(
    /* [out] */ Double* result)
{
    VALIDATE_NOT_NULL(result)
    if (mWifiInfo != NULL) {
        return mWifiInfo->GetRxSuccessRate(result);
    }
    *result = -1;
    return NOERROR;
}

ECode WifiStateMachine::HandlePreDhcpSetup()
{
    mDhcpActive = TRUE;

    // Disable power save and suspend optimizations during DHCP
    // Note: The order here is important for now. Brcm driver changes
    // power settings when we control suspend mode optimizations.
    // TODO: Remove this comment when the driver is fixed.
    SetSuspendOptimizationsNative(SUSPEND_DUE_TO_DHCP, FALSE);
    mWifiNative->SetPowerSave(FALSE);

    mWifiNative->SetBluetoothCoexistenceMode(
             WifiNative::BLUETOOTH_COEXISTENCE_MODE_DISABLED);

    StopBatchedScan();
    WifiNative::PauseScan();

    /* P2p discovery breaks dhcp, shut it down in order to get through this */
    AutoPtr<IMessage> msg;
    CMessage::New((IMessage**)&msg);
    msg->SetWhat(WifiP2pServiceImpl::BLOCK_DISCOVERY);
    msg->SetArg1(WifiP2pServiceImpl::ENABLED);
    msg->SetArg2(IDhcpStateMachine::CMD_PRE_DHCP_ACTION_COMPLETE);
    msg->SetObj(mDhcpStateMachine);
    mWifiP2pChannel->SendMessage(msg);
    return NOERROR;
}

ECode WifiStateMachine::StartDhcp()
{
    if (mDhcpStateMachine == NULL) {
        AutoPtr<IDhcpStateMachineHelper> dhcpsmHelper;
        CDhcpStateMachineHelper::AcquireSingleton((IDhcpStateMachineHelper**)&dhcpsmHelper);
        dhcpsmHelper->MakeDhcpStateMachine(mContext,
                IStateMachine::Probe(this), mInterfaceName, (IDhcpStateMachine**)&mDhcpStateMachine);
    }

    mDhcpStateMachine->RegisterForPreDhcpNotification();
    IStateMachine::Probe(mDhcpStateMachine)->SendMessage(IDhcpStateMachine::CMD_START_DHCP);
    return NOERROR;
}

ECode WifiStateMachine::RenewDhcp()
{
    if (mDhcpStateMachine == NULL) {
        AutoPtr<IDhcpStateMachineHelper> dhcpsmHelper;
        CDhcpStateMachineHelper::AcquireSingleton((IDhcpStateMachineHelper**)&dhcpsmHelper);
        dhcpsmHelper->MakeDhcpStateMachine(mContext,
                IStateMachine::Probe(this), mInterfaceName, (IDhcpStateMachine**)&mDhcpStateMachine);
    }

    mDhcpStateMachine->RegisterForPreDhcpNotification();
    IStateMachine::Probe(mDhcpStateMachine)->SendMessage(IDhcpStateMachine::CMD_RENEW_DHCP);
    return NOERROR;
}

ECode WifiStateMachine::StopDhcp()
{
    if (mDhcpStateMachine != NULL) {
        /* In case we were in middle of DHCP operation restore back powermode */
        HandlePostDhcpSetup();
        IStateMachine::Probe(mDhcpStateMachine)->SendMessage(IDhcpStateMachine::CMD_STOP_DHCP);
    }
    return NOERROR;
}

ECode WifiStateMachine::HandlePostDhcpSetup()
{
    mWifiP2pChannel->SendMessage(WifiP2pServiceImpl::BLOCK_DISCOVERY, WifiP2pServiceImpl::DISABLED);

    // Set the coexistence mode back to its default value
    mWifiNative->SetBluetoothCoexistenceMode(
            WifiNative::BLUETOOTH_COEXISTENCE_MODE_SENSE);

    /* Restore power save and suspend optimizations */
    SetSuspendOptimizationsNative(SUSPEND_DUE_TO_DHCP, TRUE);
    mWifiNative->SetPowerSave(TRUE);

    mDhcpActive = FALSE;

    StartBatchedScan();
    WifiNative::RestartScan();
    return NOERROR;
}

ECode WifiStateMachine::SmToString(
    /* [in] */ IMessage* message,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 what;
    message->GetWhat(&what);
    return SmToString(what, result);
}

ECode WifiStateMachine::SmToString(
    /* [in] */ Int32 what,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    String s = String("unknown");
    switch (what) {
        case WifiMonitor::DRIVER_HUNG_EVENT:
            s = "DRIVER_HUNG_EVENT";
            break;
        case IAsyncChannel::CMD_CHANNEL_HALF_CONNECTED:
            s = "AsyncChannel.CMD_CHANNEL_HALF_CONNECTED";
            break;
        case IAsyncChannel::CMD_CHANNEL_DISCONNECTED:
            s = "AsyncChannel.CMD_CHANNEL_DISCONNECTED";
            break;
        case CMD_SET_FREQUENCY_BAND:
            s = "CMD_SET_FREQUENCY_BAND";
            break;
        case CMD_DELAYED_NETWORK_DISCONNECT:
            s = "CMD_DELAYED_NETWORK_DISCONNECT";
            break;
        case CMD_TEST_NETWORK_DISCONNECT:
            s = "CMD_TEST_NETWORK_DISCONNECT";
            break;
        case CMD_OBTAINING_IP_ADDRESS_WATCHDOG_TIMER:
            s = "CMD_OBTAINING_IP_ADDRESS_WATCHDOG_TIMER";
            break;
        case CMD_START_DRIVER:
            s = "CMD_START_DRIVER";
            break;
        case CMD_STOP_DRIVER:
            s = "CMD_STOP_DRIVER";
            break;
        case CMD_STOP_SUPPLICANT:
            s = "CMD_STOP_SUPPLICANT";
            break;
        case CMD_STOP_SUPPLICANT_FAILED:
            s = "CMD_STOP_SUPPLICANT_FAILED";
            break;
        case CMD_START_SUPPLICANT:
            s = "CMD_START_SUPPLICANT";
            break;
        case CMD_REQUEST_AP_CONFIG:
            s = "CMD_REQUEST_AP_CONFIG";
            break;
        case CMD_RESPONSE_AP_CONFIG:
            s = "CMD_RESPONSE_AP_CONFIG";
            break;
        case CMD_TETHER_STATE_CHANGE:
            s = "CMD_TETHER_STATE_CHANGE";
            break;
        case CMD_TETHER_NOTIFICATION_TIMED_OUT:
            s = "CMD_TETHER_NOTIFICATION_TIMED_OUT";
            break;
        case CMD_BLUETOOTH_ADAPTER_STATE_CHANGE:
            s = "CMD_BLUETOOTH_ADAPTER_STATE_CHANGE";
            break;
        case CMD_ADD_OR_UPDATE_NETWORK:
            s = "CMD_ADD_OR_UPDATE_NETWORK";
            break;
        case CMD_REMOVE_NETWORK:
            s = "CMD_REMOVE_NETWORK";
            break;
        case CMD_ENABLE_NETWORK:
            s = "CMD_ENABLE_NETWORK";
            break;
        case CMD_ENABLE_ALL_NETWORKS:
            s = "CMD_ENABLE_ALL_NETWORKS";
            break;
        case CMD_AUTO_CONNECT:
            s = "CMD_AUTO_CONNECT";
            break;
        case CMD_AUTO_ROAM:
            s = "CMD_AUTO_ROAM";
            break;
        case CMD_AUTO_SAVE_NETWORK:
            s = "CMD_AUTO_SAVE_NETWORK";
            break;
        case CMD_BOOT_COMPLETED:
            s = "CMD_BOOT_COMPLETED";
            break;
        case IDhcpStateMachine::CMD_START_DHCP:
            s = "CMD_START_DHCP";
            break;
        case IDhcpStateMachine::CMD_STOP_DHCP:
            s = "CMD_STOP_DHCP";
            break;
        case IDhcpStateMachine::CMD_RENEW_DHCP:
            s = "CMD_RENEW_DHCP";
            break;
        case IDhcpStateMachine::CMD_PRE_DHCP_ACTION:
            s = "CMD_PRE_DHCP_ACTION";
            break;
        case IDhcpStateMachine::CMD_POST_DHCP_ACTION:
            s = "CMD_POST_DHCP_ACTION";
            break;
        case IDhcpStateMachine::CMD_PRE_DHCP_ACTION_COMPLETE:
            s = "CMD_PRE_DHCP_ACTION_COMPLETE";
            break;
        case IDhcpStateMachine::CMD_ON_QUIT:
            s = "CMD_ON_QUIT";
            break;
        case WifiP2pServiceImpl::DISCONNECT_WIFI_REQUEST:
            s = "WifiP2pServiceImpl.DISCONNECT_WIFI_REQUEST";
            break;
        case IWifiManager::DISABLE_NETWORK:
            s = "WifiManager.DISABLE_NETWORK";
            break;
        case CMD_BLACKLIST_NETWORK:
            s = "CMD_BLACKLIST_NETWORK";
            break;
        case CMD_CLEAR_BLACKLIST:
            s = "CMD_CLEAR_BLACKLIST";
            break;
        case CMD_SAVE_CONFIG:
            s = "CMD_SAVE_CONFIG";
            break;
        case CMD_GET_CONFIGURED_NETWORKS:
            s = "CMD_GET_CONFIGURED_NETWORKS";
            break;
        case CMD_GET_SUPPORTED_FEATURES:
            s = "CMD_GET_ADAPTORS";
            break;
        case CMD_UNWANTED_NETWORK:
            s = "CMD_UNWANTED_NETWORK";
            break;
        case CMD_GET_LINK_LAYER_STATS:
            s = "CMD_GET_LINK_LAYER_STATS";
            break;
        case CMD_GET_PRIVILEGED_CONFIGURED_NETWORKS:
            s = "CMD_GET_PRIVILEGED_CONFIGURED_NETWORKS";
            break;
        case CMD_DISCONNECT:
            s = "CMD_DISCONNECT";
            break;
        case CMD_RECONNECT:
            s = "CMD_RECONNECT";
            break;
        case CMD_REASSOCIATE:
            s = "CMD_REASSOCIATE";
            break;
        case CMD_GET_CONNECTION_STATISTICS:
            s = "CMD_GET_CONNECTION_STATISTICS";
            break;
        case CMD_SET_HIGH_PERF_MODE:
            s = "CMD_SET_HIGH_PERF_MODE";
            break;
        case CMD_SET_COUNTRY_CODE:
            s = "CMD_SET_COUNTRY_CODE";
            break;
        case CMD_ENABLE_RSSI_POLL:
            s = "CMD_ENABLE_RSSI_POLL";
            break;
        case CMD_RSSI_POLL:
            s = "CMD_RSSI_POLL";
            break;
        case CMD_START_PACKET_FILTERING:
            s = "CMD_START_PACKET_FILTERING";
            break;
        case CMD_STOP_PACKET_FILTERING:
            s = "CMD_STOP_PACKET_FILTERING";
            break;
        case CMD_SET_SUSPEND_OPT_ENABLED:
            s = "CMD_SET_SUSPEND_OPT_ENABLED";
            break;
        case CMD_NO_NETWORKS_PERIODIC_SCAN:
            s = "CMD_NO_NETWORKS_PERIODIC_SCAN";
            break;
        case CMD_SET_BATCHED_SCAN:
            s = "CMD_SET_BATCHED_SCAN";
            break;
        case CMD_START_NEXT_BATCHED_SCAN:
            s = "CMD_START_NEXT_BATCHED_SCAN";
            break;
        case CMD_POLL_BATCHED_SCAN:
            s = "CMD_POLL_BATCHED_SCAN";
            break;
        case CMD_UPDATE_LINKPROPERTIES:
            s = "CMD_UPDATE_LINKPROPERTIES";
            break;
        case CMD_RELOAD_TLS_AND_RECONNECT:
            s = "CMD_RELOAD_TLS_AND_RECONNECT";
            break;
        case IWifiManager::CONNECT_NETWORK:
            s = "CONNECT_NETWORK";
            break;
        case IWifiManager::SAVE_NETWORK:
            s = "SAVE_NETWORK";
            break;
        case IWifiManager::FORGET_NETWORK:
            s = "FORGET_NETWORK";
            break;
        case WifiMonitor::SUP_CONNECTION_EVENT:
            s = "SUP_CONNECTION_EVENT";
            break;
        case WifiMonitor::SUP_DISCONNECTION_EVENT:
            s = "SUP_DISCONNECTION_EVENT";
            break;
        case WifiMonitor::SCAN_RESULTS_EVENT:
            s = "SCAN_RESULTS_EVENT";
            break;
        case WifiMonitor::SUPPLICANT_STATE_CHANGE_EVENT:
            s = "SUPPLICANT_STATE_CHANGE_EVENT";
            break;
        case WifiMonitor::AUTHENTICATION_FAILURE_EVENT:
            s = "AUTHENTICATION_FAILURE_EVENT";
            break;
        case WifiMonitor::SSID_TEMP_DISABLED:
            s = "SSID_TEMP_DISABLED";
            break;
        case WifiMonitor::SSID_REENABLED:
            s = "SSID_REENABLED";
            break;
        case WifiMonitor::WPS_SUCCESS_EVENT:
            s = "WPS_SUCCESS_EVENT";
            break;
        case WifiMonitor::WPS_FAIL_EVENT:
            s = "WPS_FAIL_EVENT";
            break;
        case WifiMonitor::SUP_REQUEST_IDENTITY:
            s = "SUP_REQUEST_IDENTITY";
            break;
        case WifiMonitor::NETWORK_CONNECTION_EVENT:
            s = "NETWORK_CONNECTION_EVENT";
            break;
        case WifiMonitor::NETWORK_DISCONNECTION_EVENT:
            s = "NETWORK_DISCONNECTION_EVENT";
            break;
        case WifiMonitor::ASSOCIATION_REJECTION_EVENT:
            s = "ASSOCIATION_REJECTION_EVENT";
            break;
        case CMD_SET_OPERATIONAL_MODE:
            s = "CMD_SET_OPERATIONAL_MODE";
            break;
        case CMD_START_SCAN:
            s = "CMD_START_SCAN";
            break;
        case CMD_DISABLE_P2P_RSP:
            s = "CMD_DISABLE_P2P_RSP";
            break;
        case CMD_DISABLE_P2P_REQ:
            s = "CMD_DISABLE_P2P_REQ";
            break;
        case WifiWatchdogStateMachine::GOOD_LINK_DETECTED:
            s = "GOOD_LINK_DETECTED";
            break;
        case WifiWatchdogStateMachine::POOR_LINK_DETECTED:
            s = "POOR_LINK_DETECTED";
            break;
        case WifiP2pServiceImpl::GROUP_CREATING_TIMED_OUT:
            s = "GROUP_CREATING_TIMED_OUT";
            break;
        case WifiP2pServiceImpl::P2P_CONNECTION_CHANGED:
            s = "P2P_CONNECTION_CHANGED";
            break;
        case WifiP2pServiceImpl::DISCONNECT_WIFI_RESPONSE:
            s = "P2P.DISCONNECT_WIFI_RESPONSE";
            break;
        case WifiP2pServiceImpl::SET_MIRACAST_MODE:
            s = "P2P.SET_MIRACAST_MODE";
            break;
        case WifiP2pServiceImpl::BLOCK_DISCOVERY:
            s = "P2P.BLOCK_DISCOVERY";
            break;
        case WifiP2pServiceImpl::SET_COUNTRY_CODE:
            s = "P2P.SET_COUNTRY_CODE";
            break;
        case IWifiManager::CANCEL_WPS:
            s = "CANCEL_WPS";
            break;
        case IWifiManager::CANCEL_WPS_FAILED:
            s = "CANCEL_WPS_FAILED";
            break;
        case IWifiManager::CANCEL_WPS_SUCCEDED:
            s = "CANCEL_WPS_SUCCEDED";
            break;
        case IWifiManager::START_WPS:
            s = "START_WPS";
            break;
        case IWifiManager::START_WPS_SUCCEEDED:
            s = "START_WPS_SUCCEEDED";
            break;
        case IWifiManager::WPS_FAILED:
            s = "WPS_FAILED";
            break;
        case IWifiManager::WPS_COMPLETED:
            s = "WPS_COMPLETED";
            break;
        case IWifiManager::RSSI_PKTCNT_FETCH:
            s = "RSSI_PKTCNT_FETCH";
            break;
        case CMD_IP_CONFIGURATION_LOST:
            s = "CMD_IP_CONFIGURATION_LOST";
            break;
        case CMD_IP_CONFIGURATION_SUCCESSFUL:
            s = "CMD_IP_CONFIGURATION_SUCCESSFUL";
            break;
        case CMD_STATIC_IP_SUCCESS:
            s = "CMD_STATIC_IP_SUCCESSFUL";
            break;
        case CMD_STATIC_IP_FAILURE:
            s = "CMD_STATIC_IP_FAILURE";
            break;
        case IDhcpStateMachine::DHCP_SUCCESS:
            s = "DHCP_SUCCESS";
            break;
        case IDhcpStateMachine::DHCP_FAILURE:
            s = "DHCP_FAILURE";
            break;
        case CMD_TARGET_BSSID:
            s = "CMD_TARGET_BSSID";
            break;
        case CMD_ASSOCIATED_BSSID:
            s = "CMD_ASSOCIATED_BSSID";
            break;
        case CMD_ROAM_WATCHDOG_TIMER:
            s = "CMD_ROAM_WATCHDOG_TIMER";
            break;
        case CMD_SCREEN_STATE_CHANGED:
            s = "CMD_SCREEN_STATE_CHANGED";
            break;
        case CMD_DISCONNECTING_WATCHDOG_TIMER:
            s = "CMD_DISCONNECTING_WATCHDOG_TIMER";
            break;
        case CMD_TARGET_SSID:
            s = "CMD_TARGET_SSID";
            break;
        default:
            s = String("what:") + StringUtils::ToString(what);
            break;
    }
    *result = s;
    return NOERROR;
}

ECode WifiStateMachine::RegisterConnected()
{
    if (mLastNetworkId != IWifiConfiguration::INVALID_NETWORK_ID) {
        AutoPtr<ISystem> system;
        CSystem::AcquireSingleton((ISystem**)&system);
        Int64 now_ms;
        system->GetCurrentTimeMillis(&now_ms);
        // We are switching away from this configuration,
        // hence record the time we were connected last
        AutoPtr<IWifiConfiguration> config;
        mWifiConfigStore->GetWifiConfiguration(mLastNetworkId, (IWifiConfiguration**)&config);
        if (config != NULL) {
            system->GetCurrentTimeMillis(&now_ms);
            config->SetLastConnected(now_ms);
            config->SetAutoJoinBailedDueToLowRssi(FALSE);
            config->SetAutoJoinStatus(IWifiConfiguration::AUTO_JOIN_ENABLED);
            config->SetNumConnectionFailures(0);
            config->SetNumIpConfigFailures(0);
            config->SetNumAuthFailures(0);
            Int32 val;
            config->GetNumAssociation(&val);
            config->SetNumAssociation(++val);
        }
        mBadLinkspeedcount = 0;
    }
    return NOERROR;
}

ECode WifiStateMachine::RegisterDisconnected()
{
    if (mLastNetworkId != IWifiConfiguration::INVALID_NETWORK_ID) {
        AutoPtr<ISystem> system;
        CSystem::AcquireSingleton((ISystem**)&system);
        Int64 now_ms;
        system->GetCurrentTimeMillis(&now_ms);
        // We are switching away from this configuration,
        // hence record the time we were connected last
        AutoPtr<IWifiConfiguration> config;
        mWifiConfigStore->GetWifiConfiguration(mLastNetworkId, (IWifiConfiguration**)&config);
        if (config != NULL) {
            config->SetLastDisconnected(now_ms);
        }
    }
    return NOERROR;
}

ECode WifiStateMachine::NoteWifiDisabledWhileAssociated()
{
    // We got disabled by user while we were associated, make note of it
    Int32 rssi;
    mWifiInfo->GetRssi(&rssi);
    AutoPtr<IWifiConfiguration> config;
    mWifiConfigStore->GetWifiConfiguration(mLastNetworkId, (IWifiConfiguration**)&config);
    if (GetCurrentState() == mConnectedState
            && rssi != IWifiInfo::INVALID_RSSI
            && config != NULL) {
        Boolean is24GHz;
        mWifiInfo->Is24GHz(&is24GHz);
        Boolean isBadRSSI = (is24GHz && rssi < mWifiConfigStore->thresholdBadRssi24)
                || (!is24GHz && rssi < mWifiConfigStore->thresholdBadRssi5);
        Boolean isLowRSSI = (is24GHz && rssi < mWifiConfigStore->thresholdLowRssi24)
                || (!is24GHz && rssi < mWifiConfigStore->thresholdLowRssi5);
        Boolean isHighRSSI = (is24GHz && rssi >= mWifiConfigStore->thresholdGoodRssi24)
                || (!is24GHz && rssi >= mWifiConfigStore->thresholdGoodRssi5);

        Int32 val;
        if (isBadRSSI) {
            // Take note that we got disabled while RSSI was Bad
            config->SetNumUserTriggeredWifiDisableLowRSSI(
                    (config->GetNumUserTriggeredWifiDisableLowRSSI(&val), ++val));
        }
        else if (isLowRSSI) {
            // Take note that we got disabled while RSSI was Low
            config->SetNumUserTriggeredWifiDisableBadRSSI(
                    (config->GetNumUserTriggeredWifiDisableLowRSSI(&val), ++val));
        }
        else if (!isHighRSSI) {
            // Take note that we got disabled while RSSI was Not high
            config->SetNumUserTriggeredWifiDisableNotHighRSSI(
                    (config->GetNumUserTriggeredWifiDisableLowRSSI(&val), ++val));
        }
    }
    return NOERROR;
}

ECode WifiStateMachine::SetInternetAccessState(
    /* [in] */ Boolean enabled)
{
    AutoPtr<IWifiConfiguration> config;
    GetCurrentWifiConfiguration((IWifiConfiguration**)&config);

    if (config != NULL) {
        config->SetNoInternetAccess(enabled);
    }
    return NOERROR;
}

ECode WifiStateMachine::GetCurrentWifiConfiguration(
    /* [out] */ IWifiConfiguration** result)
{
    VALIDATE_NOT_NULL(result)
    if (mLastNetworkId == IWifiConfiguration::INVALID_NETWORK_ID) {
        *result = NULL;
        return NOERROR;
    }
    return mWifiConfigStore->GetWifiConfiguration(mLastNetworkId, result);
}

ECode WifiStateMachine::GetCurrentScanResult(
    /* [out] */ IScanResult** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IWifiConfiguration> config;
    GetCurrentWifiConfiguration((IWifiConfiguration**)&config);

    if (config == NULL) {
        *result = NULL;
        return NOERROR;
    }

    String BSSID;
    mWifiInfo->GetBSSID(&BSSID);
    if (BSSID == NULL) {
        BSSID = mTargetRoamBSSID;
    }
    AutoPtr<IHashMap> map;
    config->GetScanResultCache((IHashMap**)&map);
    if (map == NULL) {
        *result = NULL;
        return NOERROR;
    }
    AutoPtr<IInterface> obj;
    map->Get(CoreUtils::Convert(BSSID), (IInterface**)&obj);
    *result = IScanResult::Probe(obj);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode WifiStateMachine::GetCurrentBSSID(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    if (linkDebouncing) {
        *result = String(NULL);
        return NOERROR;
    }
    *result = mLastBssid;
    return NOERROR;
}

ECode WifiStateMachine::UnwantedNetwork(
    /* [in] */ Int32 reason)
{
    return SendMessage(CMD_UNWANTED_NETWORK, reason);
}

ECode WifiStateMachine::StartScanForConfiguration(
    /* [in] */ IWifiConfiguration* config,
    /* [in] */ Boolean restrictChannelList,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (config == NULL)
        *result = FALSE;
        return NOERROR;

    // We are still seeing a fairly high power consumption triggered by autojoin scans
    // Hence do partial scans only for PSK configuration that are roamable since the
    // primary purpose of the partial scans is roaming.
    // Full badn scans with exponential backoff for the purpose or extended roaming and
    // network switching are performed unconditionally.
    AutoPtr<IHashMap> scanResultCache;
    config->GetScanResultCache((IHashMap**)&scanResultCache);
    AutoPtr<IBitSet> allowedKeyManagement;
    config->GetAllowedKeyManagement((IBitSet**)&allowedKeyManagement);
    Boolean b;
    Int32 size;
    if (scanResultCache == NULL
            || (allowedKeyManagement->Get(IWifiConfigurationKeyMgmt::WPA_PSK, &b), !b)
            || (scanResultCache->GetSize(&size), size) > 6) {
        //return TRUE but to not trigger the scan
        *result = TRUE;
        return NOERROR;
    }
    AutoPtr<IHashSet> channels;
    mWifiConfigStore->MakeChannelList(config, ONE_HOUR_MILLI, restrictChannelList, (IHashSet**)&channels);
    String key;
    config->ConfigKey(&key);
    if (channels != NULL && (channels->GetSize(&size), size) != 0) {
        StringBuilder freqs;
        Boolean first = TRUE;
        AutoPtr<ArrayOf<IInterface*> > array;
        channels->ToArray((ArrayOf<IInterface*>**)&array);
        for (Int32 i = 0; i < array->GetLength(); i++) {
            AutoPtr<IInteger32> channel = IInteger32::Probe((*array)[i]);
            if (!first)
                freqs.Append(",");
            Int32 value;
            channel->GetValue(&value);
            freqs.Append(value);
            first = FALSE;
        }
        String str;
        freqs.ToString(&str);
        if (DBG) {
            Loge(String("WifiStateMachine starting scan for ") + key + " with " + str);
        }
        // Call wifi native to start the scan
        if (StartScanNative(
                WifiNative::SCAN_WITHOUT_CONNECTION_SETUP, str)) {
            // Only count battery consumption if scan request is accepted
            NoteScanStart(SCAN_ALARM_SOURCE, NULL);
            messageHandlingStatus = MESSAGE_HANDLING_STATUS_OK;
        }
        else {
            // used for debug only, mark scan as failed
            messageHandlingStatus = MESSAGE_HANDLING_STATUS_HANDLING_ERROR;
        }
        *result = TRUE;
        return NOERROR;
    }
    else {
        if (DBG) Loge(String("WifiStateMachine no channels for ") + key);
        *result = FALSE;
        return NOERROR;
    }
}

ECode WifiStateMachine::ClearCurrentConfigBSSID(
    /* [in] */ const String& dbg)
{
    // Clear the bssid in the current config's network block
    AutoPtr<IWifiConfiguration> config;
    GetCurrentWifiConfiguration((IWifiConfiguration**)&config);
    if (config == NULL)
        return NOERROR;
    return ClearConfigBSSID(config, dbg);
}

ECode WifiStateMachine::ClearConfigBSSID(
    /* [in] */ IWifiConfiguration* config,
    /* [in] */ const String& dbg)
{
    if (config == NULL)
        return NOERROR;
    String key;
    config->ConfigKey(&key);
    String BSSID;
    config->GetBSSID(&BSSID);
    if (DBG) {
        Loge(dbg + " " + mTargetRoamBSSID + " config " + key
                + " config.bssid " + BSSID);
    }
    config->SetAutoJoinBSSID(String("any"));

    // If an app specified a BSSID then dont over-write it
    Boolean b;
    if (mWifiAutoJoinController->IsBlacklistedBSSID(BSSID, &b), !b) {
        config->SetBSSID(String("any"));
    }

    String ssid;
    config->GetSSID(&ssid);
    Int32 networkId;
    config->GetNetworkId(&networkId);
    if (DBG) {
       Loge(dbg + " " + ssid
                + " nid=" + StringUtils::ToString(networkId));
    }
    mWifiConfigStore->SaveWifiConfigBSSID(config);
    return NOERROR;
}

ECode WifiStateMachine::HandleGsmAuthRequest(
    /* [in] */ SimAuthRequestData* requestData)
{
    Int32 networkId;
    targetWificonfiguration->GetNetworkId(&networkId);
    if (targetWificonfiguration == NULL
            || networkId == requestData->networkId) {
        Logd(String("id matches targetWifiConfiguration"));
    }
    else {
        Logd(String("id does not match targetWifiConfiguration"));
        return NOERROR;
    }

    AutoPtr<IInterface> service;
    mContext->GetSystemService(IContext::TELEPHONY_SERVICE, (IInterface**)&service);
    AutoPtr<ITelephonyManager> tm = ITelephonyManager::Probe(service);

    if (tm != NULL) {
        StringBuilder sb;
        for (Int32 i = 0; i < requestData->challenges->GetLength(); i++) {
            String challenge = (*requestData->challenges)[i];

            Logd(String("RAND = ") + challenge);

            AutoPtr<ArrayOf<Byte> > rand;
            // try {
            ParseHex(challenge, (ArrayOf<Byte>**)&rand);
            // } catch (NumberFormatException e) {
            //     Loge("malformed challenge");
            //     continue;
            // }

            // base64Challenge = android.util.Base64.encodeToString(
            //         rand, android.util.Base64.NO_WRAP);
            String base64Challenge = String(*rand);

            /*
             * appType = 1 => SIM, 2 => USIM according to
             * com.android.internal.telephony.PhoneConstants#APPTYPE_xxx
             */
            Int32 appType = 2;
            String tmResponse;
            tm->GetIccSimChallengeResponse(appType, base64Challenge, &tmResponse);
            Logv(String("Raw Response - ") + tmResponse);

            if (tmResponse != NULL && tmResponse.GetLength() > 4) {
                AutoPtr<ArrayOf<Byte> > result = tmResponse.GetBytes();
                // result = android.util.Base64.decode(tmResponse,
                //         android.util.Base64.DEFAULT);

                Logv(String("Hex Response -") + MakeHex(result));
                Int32 sres_len = (*result)[0];
                String sres = MakeHex(result, 1, sres_len);
                Int32 kc_offset = 1 + sres_len;
                Int32 kc_len = (*result)[kc_offset];
                String kc = MakeHex(result, 1 + kc_offset, kc_len);
                sb.Append(String(":") + kc + ":" + sres);
                Logv(String("kc:") + kc + " sres:" + sres);
            }
            else {
                Loge(String("bad response - ") + tmResponse);
            }
        }

        String response;
        sb.ToString(&response);
        Logv(String("Supplicant Response -") + response);
        mWifiNative->SimAuthResponse(requestData->networkId, response);
    }
    else {
        Loge(String("could not get telephony manager"));
    }
    return NOERROR;
}

ECode WifiStateMachine::Handle3GAuthRequest(
    /* [in] */ SimAuthRequestData* requestData)
{
    return NOERROR;
}

void WifiStateMachine::Loge(
    /* [in] */ const String& s)
{
    String name;
    GetName(&name);
    Logger::E(name, s);
}

void WifiStateMachine::Logd(
    /* [in] */ const String& s)
{
    String name;
    GetName(&name);
    Logger::D(name, s);
}

void WifiStateMachine::Log(
    /* [in] */ const String& s)
{
    String name;
    GetName(&name);
    Logger::D(name, s);
}

String WifiStateMachine::GetLogRecString(
    /* [in] */ IMessage* msg)
{
    AutoPtr<IWifiConfiguration> config;
    Int64 now;
    Int64 milli;
    String report;
    StringBuilder sb;
    if (mScreenOn) {
        sb.Append("!");
    }
    if (messageHandlingStatus != MESSAGE_HANDLING_STATUS_UNKNOWN) {
        sb.Append("(");
        sb.Append(messageHandlingStatus);
        sb.Append(")");
    }
    String s;
    SmToString(msg, &s);
    sb.Append(s);
    Int32 sendingUid;
    msg->GetSendingUid(&sendingUid);
    if (sendingUid > 0 && sendingUid != IProcess::WIFI_UID) {
        sb.Append(" uid=" + sendingUid);
    }

    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    Int32 arg1;
    msg->GetArg1(&arg1);
    Int32 arg2;
    msg->GetArg2(&arg2);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case CMD_START_SCAN: {
            system->GetCurrentTimeMillis(&now);
            sb.Append(" ");
            sb.Append(StringUtils::ToString(arg1));
            sb.Append(" ");
            sb.Append(StringUtils::ToString(arg2));
            sb.Append(" ic=");
            sb.Append(StringUtils::ToString(sScanAlarmIntentCount));
            if (obj != NULL) {
                AutoPtr<IBundle> bundle = IBundle::Probe(obj);
                Int64 request;
                bundle->GetInt64(SCAN_REQUEST_TIME, 0, &request);
                if (request != 0) {
                    sb.Append(" proc(ms):");
                    sb.Append(now - request);
                }
            }
            if (mIsScanOngoing) sb.Append(" onGoing");
            if (mIsFullScanOngoing) sb.Append(" full");
            if (lastStartScanTimeStamp != 0) {
                sb.Append(" started:");
                sb.Append(lastStartScanTimeStamp);
                sb.Append(",");
                sb.Append(now - lastStartScanTimeStamp);
            }
            if (lastScanDuration != 0) {
                sb.Append(" dur:");
                sb.Append(lastScanDuration);
            }
            Int32 iValue;
            sb.Append(" rssi=");
            mWifiInfo->GetRssi(&iValue);
            sb.Append(iValue);
            sb.Append(" f=");
            mWifiInfo->GetFrequency(&iValue);
            sb.Append(iValue);
            sb.Append(" sc=");
            mWifiInfo->GetScore(&iValue);
            sb.Append(iValue);
            sb.Append(" link=");
            mWifiInfo->GetLinkSpeed(&iValue);
            sb.Append(iValue);
            Double dValue;
            String str = String("");
            str.AppendFormat(" tx=%.1f,", mWifiInfo->GetTxSuccessRate(&dValue), dValue);
            str.AppendFormat(" %.1f,", mWifiInfo->GetTxRetriesRate(&dValue), dValue);
            str.AppendFormat(" %.1f ", mWifiInfo->GetTxBadRate(&dValue), dValue);
            str.AppendFormat(" rx=%.1f", mWifiInfo->GetRxSuccessRate(&dValue), dValue);
            sb.Append(str);
            if (lastScanFreqs != NULL) {
                sb.Append(" list=");
                sb.Append(lastScanFreqs);
            }
            else {
                sb.Append(" fiv=");
                sb.Append(fullBandConnectedTimeIntervalMilli);
            }
            ReportOnTime(&report);
            if (report != NULL) {
                sb.Append(" ");
                sb.Append(report);
            }
            break;
        }
        case WifiMonitor::SUPPLICANT_STATE_CHANGE_EVENT: {
            milli = SystemClock::GetElapsedRealtime();
            sb.Append(" ");
            sb.Append(StringUtils::ToString(arg1));
            sb.Append(" ");
            sb.Append(StringUtils::ToString(arg2));
            sb.Append(" rt=");
            sb.Append(milli);
            sb.Append(" ");
            AutoPtr<CStateChangeResult> stateChangeResult = (CStateChangeResult*)(IObject*)obj.Get();
            if (stateChangeResult != NULL) {
                String str;
                stateChangeResult->ToString(&str);
                sb.Append(str);
            }
            break;
        }
        case IWifiManager::SAVE_NETWORK:
        case WifiStateMachine::CMD_AUTO_SAVE_NETWORK:
            sb.Append(" ");
            sb.Append(StringUtils::ToString(arg1));
            sb.Append(" ");
            sb.Append(StringUtils::ToString(arg2));
            if (lastSavedConfigurationAttempt != NULL) {
                sb.Append(" ");
                String str;
                lastSavedConfigurationAttempt->ConfigKey(&str);
                sb.Append(str);
                sb.Append(" nid=");
                Int32 val;
                lastSavedConfigurationAttempt->GetNetworkId(&val);
                sb.Append(val);
                Boolean b;
                if (lastSavedConfigurationAttempt->GetHiddenSSID(&b), b) {
                    sb.Append(" hidden");
                }
                lastSavedConfigurationAttempt->GetPreSharedKey(&str);
                if (!str.IsNull() && !str.Equals("*")) {
                    sb.Append(" hasPSK");
                }
                if (lastSavedConfigurationAttempt->GetEphemeral(&b), b) {
                    sb.Append(" ephemeral");
                }
                if (lastSavedConfigurationAttempt->GetSelfAdded(&b), b) {
                    sb.Append(" selfAdded");
                }
                sb.Append(" cuid=");
                sb.Append((lastSavedConfigurationAttempt->GetCreatorUid(&val), val));
                sb.Append(" suid=");
                sb.Append((lastSavedConfigurationAttempt->GetLastUpdateUid(&val), val));
            }
            break;
        case IWifiManager::FORGET_NETWORK:
            sb.Append(" ");
            sb.Append(StringUtils::ToString(arg1));
            sb.Append(" ");
            sb.Append(StringUtils::ToString(arg2));
            if (lastForgetConfigurationAttempt != NULL) {
                sb.Append(" ");
                String str;
                sb.Append((lastForgetConfigurationAttempt->ConfigKey(&str), str));
                sb.Append(" nid=");
                Int32 val;
                sb.Append((lastForgetConfigurationAttempt->GetNetworkId(&val), val));
                Boolean b;
                if (lastForgetConfigurationAttempt->GetHiddenSSID(&b), b) {
                    sb.Append(" hidden");
                }
                lastForgetConfigurationAttempt->GetPreSharedKey(&str);
                if (!str.IsNull()) {
                    sb.Append(" hasPSK");
                }
                if (lastForgetConfigurationAttempt->GetEphemeral(&b), b) {
                    sb.Append(" ephemeral");
                }
                if (lastForgetConfigurationAttempt->GetSelfAdded(&b), b) {
                    sb.Append(" selfAdded");
                }
                sb.Append(" cuid=");
                sb.Append((lastForgetConfigurationAttempt->GetCreatorUid(&val), val));
                sb.Append(" suid=");
                sb.Append((lastForgetConfigurationAttempt->GetLastUpdateUid(&val), val));
                sb.Append(" ajst=");
                sb.Append((lastForgetConfigurationAttempt->GetAutoJoinStatus(&val), val));
            }
            break;
        case WifiMonitor::ASSOCIATION_REJECTION_EVENT: {
            sb.Append(" ");
            sb.Append(StringUtils::ToString(arg1));
            sb.Append(" ");
            sb.Append(StringUtils::ToString(arg2));
            String bssid;
            ICharSequence::Probe(obj)->ToString(&bssid);
            if (!bssid.IsNull() && bssid.GetLength() > 0) {
                sb.Append(" ");
                sb.Append(bssid);
            }
            sb.Append(String(" blacklist=") + StringUtils::BooleanToString(didBlackListBSSID));
            milli = SystemClock::GetElapsedRealtime();
            sb.Append(" rt=");
            sb.Append(milli);
            break;
        }
        case WifiMonitor::SCAN_RESULTS_EVENT: {
            sb.Append(" ");
            sb.Append(StringUtils::ToString(arg1));
            sb.Append(" ");
            sb.Append(StringUtils::ToString(arg2));
            if (mScanResults != NULL) {
                sb.Append(" found=");
                Int32 size;
                mScanResults->GetSize(&size);
                sb.Append(size);
            }
            sb.Append(" known=");
            sb.Append(mNumScanResultsKnown);
            sb.Append(" got=");
            sb.Append(mNumScanResultsReturned);
            if (lastScanDuration != 0) {
                sb.Append(" dur:");
                sb.Append(lastScanDuration);
            }
            if (mOnTime != 0) {
                sb.Append(" on:");
                sb.Append(mOnTimeThisScan);
                sb.Append(",");
                sb.Append(mOnTimeScan);
                sb.Append(",");
                sb.Append(mOnTime);
            }
            if (mTxTime != 0) {
                sb.Append(" tx:");
                sb.Append(mTxTimeThisScan);
                sb.Append(",");
                sb.Append(mTxTimeScan);
                sb.Append(",");
                sb.Append(mTxTime);
            }
            if (mRxTime != 0) {
                sb.Append(" rx:");
                sb.Append(mRxTimeThisScan);
                sb.Append(",");
                sb.Append(mRxTimeScan);
                sb.Append(",");
                sb.Append(mRxTime);
            }
            String str;
            sb.Append(str.AppendFormat(" bcn=%d", mRunningBeaconCount));
            break;
        }
        case WifiMonitor::NETWORK_CONNECTION_EVENT:
            sb.Append(" ");
            sb.Append(StringUtils::ToString(arg1));
            sb.Append(" ");
            sb.Append(StringUtils::ToString(arg2));
            sb.Append(" ");
            sb.Append(mLastBssid);
            sb.Append(" nid=");
            sb.Append(mLastNetworkId);
            GetCurrentWifiConfiguration((IWifiConfiguration**)&config);
            if (config != NULL) {
                sb.Append(" ");
                String str;
                config->ConfigKey(&str);
                sb.Append(str);
            }
            milli = SystemClock::GetElapsedRealtime();
            sb.Append(" rt=");
            sb.Append(milli);
            break;
        case CMD_TARGET_BSSID:
        case CMD_ASSOCIATED_BSSID:
            sb.Append(" ");
            sb.Append(StringUtils::ToString(arg1));
            sb.Append(" ");
            sb.Append(StringUtils::ToString(arg2));
            if (obj != NULL) {
                sb.Append(" BSSID=");
                String str;
                ICharSequence::Probe(obj)->ToString(&str);
                sb.Append(str);
            }
            if (mTargetRoamBSSID != NULL) {
                sb.Append(" Target=");
                sb.Append(mTargetRoamBSSID);
            }
            sb.Append(" roam=");
            sb.Append(StringUtils::ToString(mAutoRoaming));
            milli = SystemClock::GetElapsedRealtime();
            sb.Append(" rt=");
            sb.Append(milli);
            break;
        case WifiMonitor::NETWORK_DISCONNECTION_EVENT: {
            if (obj != NULL) {
                sb.Append(" ");
                String str;
                ICharSequence::Probe(obj)->ToString(&str);
                sb.Append(str);
            }
            sb.Append(" nid=");
            sb.Append(arg1);
            sb.Append(" reason=");
            sb.Append(arg2);
            if (mLastBssid != NULL) {
                sb.Append(" lastbssid=");
                sb.Append(mLastBssid);
            }
            Int32 val;
            mWifiInfo->GetFrequency(&val);
            if (val != -1) {
                sb.Append(" freq=");
                sb.Append(val);
                sb.Append(" rssi=");
                sb.Append((mWifiInfo->GetRssi(&val), val));
            }
            if (linkDebouncing) {
                sb.Append(" debounce");
            }
            milli = SystemClock::GetElapsedRealtime();
            sb.Append(" rt=");
            sb.Append(milli);
            break;
        }
        case WifiMonitor::SSID_TEMP_DISABLED:
        case WifiMonitor::SSID_REENABLED:
            sb.Append(" nid=");
            sb.Append(arg1);
            if (obj != NULL) {
                sb.Append(" ");
                String str;
                ICharSequence::Probe(obj)->ToString(&str);
                sb.Append(str);
            }
            GetCurrentWifiConfiguration((IWifiConfiguration**)&config);
            if (config != NULL) {
                sb.Append(" cur=");
                String str;
                config->ConfigKey(&str);
                sb.Append(str);
                sb.Append(" ajst=");
                Int32 val;
                config->GetAutoJoinStatus(&val);
                sb.Append(val);
                Boolean b;
                if (config->GetSelfAdded(&b), b) {
                    sb.Append(" selfAdded");
                }
                config->GetStatus(&val);
                if (val != 0) {
                    sb.Append(" st=");
                    sb.Append(val);
                    sb.Append(" rs=");
                    config->GetDisableReason(&val);
                    sb.Append(val);
                }
                Int64 val64;
                config->GetLastConnected(&val64);
                if (val64 != 0) {
                    system->GetCurrentTimeMillis(&now);
                    sb.Append(" lastconn=");
                    sb.Append(now - val64);
                    sb.Append("(ms)");
                }
                if (mLastBssid != NULL) {
                    sb.Append(" lastbssid=");
                    sb.Append(mLastBssid);
                }
                mWifiInfo->GetFrequency(&val);
                if (val != -1) {
                    sb.Append(" freq=");
                    sb.Append(val);
                    sb.Append(" rssi=");
                    sb.Append((mWifiInfo->GetRssi(&val), val));
                    sb.Append(" bssid=");
                    String tmp;
                    sb.Append((mWifiInfo->GetBSSID(&tmp), tmp));
                }
            }
            milli = SystemClock::GetElapsedRealtime();
            sb.Append(" rt=");
            sb.Append(milli);
            break;
        case CMD_RSSI_POLL:
        case CMD_UNWANTED_NETWORK:
        case IWifiManager::RSSI_PKTCNT_FETCH: {
            sb.Append(" ");
            sb.Append(StringUtils::ToString(arg1));
            sb.Append(" ");
            sb.Append(StringUtils::ToString(arg2));
            String str;
            mWifiInfo->GetSSID(&str);
            if (!str.IsNull()) {
                sb.Append(" ");
                sb.Append(str);
            }
            mWifiInfo->GetBSSID(&str);
            if (!str.IsNull()) {
                sb.Append(" ");
                sb.Append(str);
            }
            sb.Append(" rssi=");
            Int32 val;
            sb.Append((mWifiInfo->GetRssi(&val), val));
            sb.Append(" f=");
            sb.Append((mWifiInfo->GetFrequency(&val), val));
            sb.Append(" sc=");
            sb.Append((mWifiInfo->GetScore(&val), val));
            sb.Append(" link=");
            sb.Append((mWifiInfo->GetLinkSpeed(&val), val));
            Double dValue;
            String tmp;
            tmp.AppendFormat(" tx=%.1f,", (mWifiInfo->GetTxSuccessRate(&dValue), dValue));
            tmp.AppendFormat(" %.1f,", (mWifiInfo->GetTxRetriesRate(&dValue), dValue));
            tmp.AppendFormat(" %.1f ", (mWifiInfo->GetTxBadRate(&dValue), dValue));
            tmp.AppendFormat(" rx=%.1f", (mWifiInfo->GetRxSuccessRate(&dValue), dValue));
            tmp.AppendFormat(" bcn=%d", mRunningBeaconCount);
            sb.Append(tmp);
            ReportOnTime(&report);
            if (report != NULL) {
                sb.Append(" ");
                sb.Append(report);
            }
            if (wifiScoringReport != NULL) {
                sb.Append(wifiScoringReport);
            }
            break;
        }
        case CMD_AUTO_CONNECT:
        case IWifiManager::CONNECT_NETWORK:
            sb.Append(" ");
            sb.Append(StringUtils::ToString(arg1));
            sb.Append(" ");
            sb.Append(StringUtils::ToString(arg2));
            config = IWifiConfiguration::Probe(obj);
            if (config != NULL) {
                sb.Append(" ");
                String str;
                config->ConfigKey(&str);
                sb.Append(str);
                AutoPtr<IWifiConfigurationVisibility> visibility;
                config->GetVisibility((IWifiConfigurationVisibility**)&visibility);
                if (visibility != NULL) {
                    Int32 val;
                    sb.Append(" [");
                    sb.Append((visibility->GetNum24(&val), val));
                    sb.Append(" ,");
                    sb.Append((visibility->GetRssi24(&val), val));
                    sb.Append(" ;");
                    sb.Append((visibility->GetNum5(&val), val));
                    sb.Append(" ,");
                    sb.Append((visibility->GetRssi5(&val), val));
                    sb.Append("]");
                }
            }
            if (mTargetRoamBSSID != NULL) {
                sb.Append(" ");
                sb.Append(mTargetRoamBSSID);
            }
            sb.Append(" roam=");
            sb.Append(StringUtils::ToString(mAutoRoaming));
            milli = SystemClock::GetElapsedRealtime();
            sb.Append(" rt=");
            sb.Append(milli);
            GetCurrentWifiConfiguration((IWifiConfiguration**)&config);
            if (config != NULL) {
                sb.Append(" ");
                String str;
                config->ConfigKey(&str);
                sb.Append(str);
                AutoPtr<IWifiConfigurationVisibility> visibility;
                config->GetVisibility((IWifiConfigurationVisibility**)&visibility);
                if (visibility != NULL) {
                    sb.Append(" [");
                    Int32 val;
                    sb.Append((visibility->GetNum24(&val), val));
                    sb.Append(" ,");
                    sb.Append((visibility->GetRssi24(&val), val));
                    sb.Append(" ;");
                    sb.Append((visibility->GetNum5(&val), val));
                    sb.Append(" ,");
                    sb.Append((visibility->GetRssi5(&val), val));
                    sb.Append("]");
                }
            }
            break;
        case CMD_AUTO_ROAM: {
            sb.Append(" ");
            sb.Append(StringUtils::ToString(arg1));
            sb.Append(" ");
            sb.Append(StringUtils::ToString(arg2));
            AutoPtr<IScanResult> result = IScanResult::Probe(obj);
            if (result != NULL) {
                system->GetCurrentTimeMillis(&now);
                sb.Append(" bssid=");
                String str;
                sb.Append((result->GetBSSID(&str), str));
                sb.Append(" rssi=");
                Int32 val;
                sb.Append((result->GetLevel(&val), val));
                sb.Append(" freq=");
                sb.Append((result->GetFrequency(&val), val));
                Int64 val64;
                result->GetSeen(&val64);
                if (val64 > 0 && val64 < now) {
                    sb.Append(" seen=");
                    sb.Append(now - val64);
                }
                else {
                    // Somehow the timestamp for this scan result is inconsistent
                    sb.Append(" !seen=");
                    sb.Append(val64);
                }
            }
            if (mTargetRoamBSSID != NULL) {
                sb.Append(" ");
                sb.Append(mTargetRoamBSSID);
            }
            sb.Append(" roam=");
            sb.Append(StringUtils::ToString(mAutoRoaming));
            sb.Append(" fail count=");
            sb.Append(StringUtils::ToString(mRoamFailCount));
            milli = SystemClock::GetElapsedRealtime();
            sb.Append(" rt=");
            sb.Append(milli);
            break;
        }
        case CMD_ADD_OR_UPDATE_NETWORK:
            sb.Append(" ");
            sb.Append(StringUtils::ToString(arg1));
            sb.Append(" ");
            sb.Append(StringUtils::ToString(arg2));
            if (obj != NULL) {
                config = IWifiConfiguration::Probe(obj);
                sb.Append(" ");
                String str;
                config->ConfigKey(&str);
                sb.Append(str);
                sb.Append(" prio=");
                Int32 val;
                sb.Append((config->GetPriority(&val), val));
                sb.Append(" status=");
                sb.Append((config->GetStatus(&val), val));
                config->GetBSSID(&str);
                if (!str.IsNull()) {
                    sb.Append(" ");
                    sb.Append(str);
                }
                AutoPtr<IWifiConfiguration> curConfig;
                GetCurrentWifiConfiguration((IWifiConfiguration**)&curConfig);
                if (curConfig != NULL) {
                    String ck;
                    config->ConfigKey(&ck);
                    String curCk;
                    curConfig->ConfigKey(&curCk);
                    if (curCk.Equals(ck)) {
                        sb.Append(" is current");
                    }
                    else {
                        sb.Append(" current=");
                        sb.Append(curCk);
                        sb.Append(" prio=");
                        Int32 value;
                        sb.Append((curConfig->GetPriority(&val), val));
                        sb.Append(" status=");
                        sb.Append((curConfig->GetStatus(&val), val));
                    }
                }
            }
            break;
        case IWifiManager::DISABLE_NETWORK:
        case CMD_ENABLE_NETWORK: {
            sb.Append(" ");
            sb.Append(StringUtils::ToString(arg1));
            sb.Append(" ");
            sb.Append(StringUtils::ToString(arg2));
            String key;
            mWifiConfigStore->GetLastSelectedConfiguration(&key);
            if (!key.IsNull()) {
                sb.Append(" last=");
                sb.Append(key);
            }
            mWifiConfigStore->GetWifiConfiguration(arg1, (IWifiConfiguration**)&config);
            String str;
            if (config != NULL && (key == NULL || !(config->ConfigKey(&str), str).Equals(key))) {
                sb.Append(" target=");
                sb.Append(key);
            }
            break;
        }
        case CMD_GET_CONFIGURED_NETWORKS: {
            sb.Append(" ");
            sb.Append(StringUtils::ToString(arg1));
            sb.Append(" ");
            sb.Append(StringUtils::ToString(arg2));
            sb.Append(" num=");
            Int32 val;
            sb.Append((mWifiConfigStore->GetConfiguredNetworksSize(&val), val));
            break;
        }
        case IDhcpStateMachine::CMD_PRE_DHCP_ACTION: {
            sb.Append(" ");
            sb.Append(StringUtils::ToString(arg1));
            sb.Append(" ");
            sb.Append(StringUtils::ToString(arg2));
            sb.Append(" txpkts=");
            Int64 val64;
            sb.Append((mWifiInfo->GetTxSuccess(&val64), val64));
            sb.Append(",");
            sb.Append((mWifiInfo->GetTxBad(&val64), val64));
            sb.Append(",");
            sb.Append((mWifiInfo->GetTxRetries(&val64), val64));
            break;
        }
        case IDhcpStateMachine::CMD_POST_DHCP_ACTION:
            sb.Append(" ");
            sb.Append(StringUtils::ToString(arg1));
            sb.Append(" ");
            sb.Append(StringUtils::ToString(arg2));
            if (arg1 == IDhcpStateMachine::DHCP_SUCCESS) {
                sb.Append(" OK ");
            }
            else if (arg1 == IDhcpStateMachine::DHCP_FAILURE) {
                sb.Append(" FAIL ");
            }
            if (mLinkProperties != NULL) {
                Boolean b;
                if (mLinkProperties->HasIPv4Address(&b), b) {
                    sb.Append(" v4");
                }
                if (mLinkProperties->HasGlobalIPv6Address(&b), b) {
                    sb.Append(" v6");
                }
                if (mLinkProperties->HasIPv4DefaultRoute(&b), b) {
                    sb.Append(" v4r");
                }
                if (mLinkProperties->HasIPv6DefaultRoute(&b), b) {
                    sb.Append(" v6r");
                }
                if (mLinkProperties->HasIPv4DnsServer(&b), b) {
                    sb.Append(" v4dns");
                }
                if (mLinkProperties->HasIPv6DnsServer(&b), b) {
                    sb.Append(" v6dns");
                }
            }
            break;
        case WifiP2pServiceImpl::P2P_CONNECTION_CHANGED:
            sb.Append(" ");
            sb.Append(StringUtils::ToString(arg1));
            sb.Append(" ");
            sb.Append(StringUtils::ToString(arg2));
            if (obj != NULL) {
                AutoPtr<INetworkInfo> info = INetworkInfo::Probe(obj);
                NetworkInfoState state;
                info->GetState(&state);
                NetworkInfoDetailedState detailedState;
                info->GetDetailedState(&detailedState);
                if (state != NULL) {
                    sb.Append(" st=");
                    sb.Append(state);
                }
                if (detailedState != NULL) {
                    sb.Append("/");
                    sb.Append(detailedState);
                }
            }
            break;
        case CMD_IP_CONFIGURATION_LOST: {
            Int32 count = -1;
            AutoPtr<IWifiConfiguration> c;
            GetCurrentWifiConfiguration((IWifiConfiguration**)&c);
            if (c != NULL) c->GetNumIpConfigFailures(&count);
            sb.Append(" ");
            sb.Append(StringUtils::ToString(arg1));
            sb.Append(" ");
            sb.Append(StringUtils::ToString(arg2));
            sb.Append(" failures: ");
            sb.Append(StringUtils::ToString(count));
            sb.Append("/");
            Int32 val;
            mWifiConfigStore->GetMaxDhcpRetries(&val);
            sb.Append(StringUtils::ToString(val));
            String str;
            mWifiInfo->GetBSSID(&str);
            if (!str.IsNull()) {
                sb.Append(" ");
                sb.Append(str);
            }
            if (c != NULL) {
                AutoPtr<IHashMap> map;
                c->GetScanResultCache((IHashMap**)&map);
                if (map != NULL) {
                    AutoPtr<ICollection> values;
                    map->GetValues((ICollection**)&values);
                    AutoPtr<ArrayOf<IInterface*> > array;
                    values->ToArray((ArrayOf<IInterface*>**)&array);
                    for (Int32 i = 0; i < array->GetLength(); i++) {
                        AutoPtr<IScanResult> r = IScanResult::Probe((*array)[i]);

                        String rBssid, wiBssid;
                        r->GetBSSID(&rBssid);
                        mWifiInfo->GetBSSID(&wiBssid);
                        if (rBssid.Equals(wiBssid)) {
                            sb.Append(" ipfail=");
                            sb.Append((r->GetNumIpConfigFailures(&val), val));
                            sb.Append(",st=");
                            sb.Append((r->GetAutoJoinStatus(&val), val));
                        }
                    }
                }
                sb.Append(" -> ajst=");
                sb.Append((c->GetAutoJoinStatus(&val), val));
                sb.Append(" ");
                sb.Append((c->GetDisableReason(&val), val));
                sb.Append(" txpkts=");
                Int64 val64;
                sb.Append((mWifiInfo->GetTxSuccess(&val64), val64));
                sb.Append(",");
                sb.Append((mWifiInfo->GetTxBad(&val64), val64));
                sb.Append(",");
                sb.Append((mWifiInfo->GetTxRetries(&val64), val64));
            }
            milli = SystemClock::GetElapsedRealtime();
            sb.Append(" rt=");
            sb.Append(milli);
            String tmp;
            sb.Append(tmp.AppendFormat(" bcn=%d", mRunningBeaconCount));
            break;
        }
        case CMD_UPDATE_LINKPROPERTIES:
            sb.Append(" ");
            sb.Append(StringUtils::ToString(arg1));
            sb.Append(" ");
            sb.Append(StringUtils::ToString(arg2));
            if (mLinkProperties != NULL) {
                Boolean b;
                if (mLinkProperties->HasIPv4Address(&b), b) {
                    sb.Append(" v4");
                }
                if (mLinkProperties->HasGlobalIPv6Address(&b), b) {
                    sb.Append(" v6");
                }
                if (mLinkProperties->HasIPv4DefaultRoute(&b), b) {
                    sb.Append(" v4r");
                }
                if (mLinkProperties->HasIPv6DefaultRoute(&b), b) {
                    sb.Append(" v6r");
                }
                if (mLinkProperties->HasIPv4DnsServer(&b), b) {
                    sb.Append(" v4dns");
                }
                if (mLinkProperties->HasIPv6DnsServer(&b), b) {
                    sb.Append(" v6dns");
                }
            }
            break;
        case CMD_SET_COUNTRY_CODE:
            sb.Append(" ");
            sb.Append(StringUtils::ToString(arg1));
            sb.Append(" ");
            sb.Append(StringUtils::ToString(arg2));
            if (obj != NULL) {
                sb.Append(" ");
                String str;
                ICharSequence::Probe(obj)->ToString(&str);
                sb.Append(str);
            }
            break;
        case CMD_ROAM_WATCHDOG_TIMER:
            sb.Append(" ");
            sb.Append(StringUtils::ToString(arg1));
            sb.Append(" ");
            sb.Append(StringUtils::ToString(arg2));
            sb.Append(" cur=");
            sb.Append(roamWatchdogCount);
            break;
        case CMD_DISCONNECTING_WATCHDOG_TIMER:
            sb.Append(" ");
            sb.Append(StringUtils::ToString(arg1));
            sb.Append(" ");
            sb.Append(StringUtils::ToString(arg2));
            sb.Append(" cur=");
            sb.Append(disconnectingWatchdogCount);
            break;
        default:
            sb.Append(" ");
            sb.Append(StringUtils::ToString(arg1));
            sb.Append(" ");
            sb.Append(StringUtils::ToString(arg2));
            break;
    }

    String str;
    sb.ToString(&str);
    return str;
}

void WifiStateMachine::SetScanAlarm(
    /* [in] */ Boolean enabled,
    /* [in] */ Int32 delayMilli)
{
    if (PDBG) {
        Loge(String("setScanAlarm ") + StringUtils::BooleanToString(enabled)
                + " period " + StringUtils::ToString(mCurrentScanAlarmMs)
                + " initial delay " + StringUtils::ToString(delayMilli));
    }
    if (mCurrentScanAlarmMs <= 0) enabled = FALSE;
    if (enabled == mAlarmEnabled) return;
    if (enabled) {
        Int64 initialDelayMilli;
        if (delayMilli <= 0) {
            // scan now
            StartScan(SCAN_ALARM_SOURCE, 0, NULL, NULL);
            initialDelayMilli = mCurrentScanAlarmMs;
        }
        else {
            initialDelayMilli = delayMilli;
        }

        Int32 type = IAlarmManager::RTC;

        /* Set RTC_WAKEUP alarms if PNO is not supported - because no one is */
        /* going to wake up the host processor to look for access points */
        if (mBackgroundScanSupported == FALSE)
            type = IAlarmManager::RTC_WAKEUP;

        AutoPtr<ISystem> system;
        CSystem::AcquireSingleton((ISystem**)&system);
        Int64 currentTimeMillis;
        system->GetCurrentTimeMillis(&currentTimeMillis);
        mAlarmManager->SetRepeating(type,
                currentTimeMillis + initialDelayMilli,
                mCurrentScanAlarmMs,
                mScanIntent);
        mAlarmEnabled = TRUE;
    }
    else {
        mAlarmManager->Cancel(mScanIntent);
        mAlarmEnabled = FALSE;
    }
}

Boolean WifiStateMachine::SetRandomMacOui()
{
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
    args->Set(0, CoreUtils::Convert(GOOGLE_OUI));
    String oui;
    res->GetString(R::string::config_wifi_random_mac_oui, args, &oui);
    AutoPtr<ArrayOf<String> > ouiParts;
    StringUtils::Split(oui, String("-"), (ArrayOf<String>**)&ouiParts);
    AutoPtr<ArrayOf<Byte> > ouiBytes = ArrayOf<Byte>::Alloc(3);
    (*ouiBytes)[0] = (Byte) (StringUtils::ParseByte((*ouiParts)[0], 16) & 0xFF);
    (*ouiBytes)[1] = (Byte) (StringUtils::ParseByte((*ouiParts)[1], 16) & 0xFF);
    (*ouiBytes)[2] = (Byte) (StringUtils::ParseByte((*ouiParts)[2], 16) & 0xFF);

    Logd(String("Setting OUI to ") + oui);
    return mWifiNative->SetScanningMacOui(ouiBytes);
}

void WifiStateMachine::StartBatchedScan()
{
    if (mBatchedScanSettings == NULL) return;

    if (mDhcpActive) {
        if (DBG) Log(String("not starting Batched Scans due to DHCP"));
        return;
    }

    // first grab any existing data
    RetrieveBatchedScanData();

    if (PDBG) Loge(String("try  starting Batched Scans due to DHCP"));


    mAlarmManager->Cancel(mBatchedScanIntervalIntent);

    String scansExpected = mWifiNative->SetBatchedScanSettings(mBatchedScanSettings);
    // try {
    mExpectedBatchedScans = StringUtils::ParseInt32(scansExpected);
    SetNextBatchedAlarm(mExpectedBatchedScans);
    if (mExpectedBatchedScans > 0) NoteBatchedScanStart();
    // } catch (NumberFormatException e) {
    //     stopBatchedScan();
    //     Loge("Exception parsing WifiNative.setBatchedScanSettings response " + e);
    // }
}

void WifiStateMachine::StartNextBatchedScanAsync()
{
    SendMessage(CMD_START_NEXT_BATCHED_SCAN);
}

void WifiStateMachine::StartNextBatchedScan()
{
    // first grab any existing data
    RetrieveBatchedScanData();

    SetNextBatchedAlarm(mExpectedBatchedScans);
}

void WifiStateMachine::HandleBatchedScanPollRequest()
{
    if (DBG) {
        // Log(String("handleBatchedScanPoll Request - mBatchedScanMinPollTime=") +
        //         mBatchedScanMinPollTime + " , mBatchedScanSettings=" +
        //         mBatchedScanSettings);
    }
    // if there is no appropriate PollTime that's because we either aren't
    // batching or we've already set a time for a poll request
    if (mBatchedScanMinPollTime == 0) return;
    if (mBatchedScanSettings == NULL) return;

    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    Int64 now;
    system->GetCurrentTimeMillis(&now);

    if (now > mBatchedScanMinPollTime) {
        // do the poll and reset our timers
        StartNextBatchedScan();
    }
    else {
        mAlarmManager->SetExact(IAlarmManager::RTC_WAKEUP, mBatchedScanMinPollTime,
                mBatchedScanIntervalIntent);
        mBatchedScanMinPollTime = 0;
    }
}

Boolean WifiStateMachine::RecordBatchedScanSettings(
    /* [in] */ Int32 responsibleUid,
    /* [in] */ Int32 csph,
    /* [in] */ IBundle* bundle)
{
    AutoPtr<IParcelable> p;
    bundle->GetParcelable(BATCHED_SETTING, (IParcelable**)&p);
    AutoPtr<IBatchedScanSettings> settings = IBatchedScanSettings::Probe(p);

    p = NULL;
    bundle->GetParcelable(BATCHED_WORKSOURCE, (IParcelable**)&p);
    AutoPtr<IWorkSource> responsibleWorkSource = IWorkSource::Probe(p);

    if (DBG) {
        // Log(String("set batched scan to ") + settings + " for uid=" + responsibleUid +
        //         ", worksource=" + responsibleWorkSource);
    }
    if (settings != NULL) {
        Boolean b;
        if (IObject::Probe(settings)->Equals(mBatchedScanSettings, &b), b) return FALSE;
    }
    else {
        if (mBatchedScanSettings == NULL) return FALSE;
    }
    mBatchedScanSettings = settings;
    if (responsibleWorkSource == NULL)
            CWorkSource::New(responsibleUid, (IWorkSource**)&responsibleWorkSource);
    mBatchedScanWorkSource = responsibleWorkSource;
    mBatchedScanCsph = csph;
    return TRUE;
}

void WifiStateMachine::StopBatchedScan()
{
    mAlarmManager->Cancel(mBatchedScanIntervalIntent);
    RetrieveBatchedScanData();
    mWifiNative->SetBatchedScanSettings(NULL);
    NoteBatchedScanStop();
}

void WifiStateMachine::SetNextBatchedAlarm(
    /* [in] */ Int32 scansExpected)
{
    if (mBatchedScanSettings == NULL || scansExpected < 1) return;

    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    Int64 currentTimeMillis;
    system->GetCurrentTimeMillis(&currentTimeMillis);
    Int32 scanIntervalSec;
    mBatchedScanSettings->GetScanIntervalSec(&scanIntervalSec);
    mBatchedScanMinPollTime = currentTimeMillis +
            scanIntervalSec * 1000;

    Int32 maxScansPerBatch;
    mBatchedScanSettings->GetMaxScansPerBatch(&maxScansPerBatch);
    if (maxScansPerBatch < scansExpected) {
        scansExpected = maxScansPerBatch;
    }

    Int32 secToFull = scanIntervalSec;
    secToFull *= scansExpected;

    AutoPtr<ISystemProperties> prop;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&prop);
    Int32 debugPeriod;
    prop->GetInt32(String("wifi.batchedScan.pollPeriod"), 0, &debugPeriod);
    if (debugPeriod > 0) secToFull = debugPeriod;

    // set the alarm to do the next poll.  We set it a little short as we'd rather
    // wake up wearly than miss a scan due to buffer overflow
    system->GetCurrentTimeMillis(&currentTimeMillis);
    mAlarmManager->SetExact(IAlarmManager::RTC_WAKEUP, currentTimeMillis
            + ((secToFull - (scanIntervalSec / 2)) * 1000),
            mBatchedScanIntervalIntent);
}

void WifiStateMachine::RetrieveBatchedScanData()
{
    String rawData = mWifiNative->GetBatchedScanResults();
    if (DEBUG_PARSE) Log(String("rawData = ") + rawData);
    mBatchedScanMinPollTime = 0;
    if (rawData == NULL || rawData.EqualsIgnoreCase("OK")) {
        Loge(String("Unexpected BatchedScanResults :") + rawData);
        return;
    }

    Int32 scanCount = 0;
    String END_OF_BATCHES = String("----");
    String SCANCOUNT = String("scancount=");
    String TRUNCATED = String("trunc");
    String AGE = String("age=");
    String DIST = String("dist=");
    String DISTSD = String("distSd=");

    AutoPtr<ArrayOf<String> > splitData;
    StringUtils::Split(rawData, String("\n"), (ArrayOf<String>**)&splitData);
    Int32 n = 0;
    if ((*splitData)[n].StartWith(SCANCOUNT)) {
        // try {
        scanCount = StringUtils::ParseInt32((*splitData)[n++].Substring(SCANCOUNT.GetLength()));
        // } catch (NumberFormatException e) {
        //     Loge("scancount parseInt Exception from " + splitData[n]);
        // }
    }
    else {
        Log(String("scancount not found"));
    }
    if (scanCount == 0) {
        Loge(String("scanCount==0 - aborting"));
        return;
    }

    AutoPtr<IIntent> intent;
    CIntent::New(IWifiManager::BATCHED_SCAN_RESULTS_AVAILABLE_ACTION, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);

    synchronized (mBatchedScanResults) {
        mBatchedScanResults->Clear();
        AutoPtr<IBatchedScanResult> batchedScanResult;
        CBatchedScanResult::New((IBatchedScanResult**)&batchedScanResult);

        String bssid;
        AutoPtr<IWifiSsid> wifiSsid;
        Int32 level = 0;
        Int32 freq = 0;
        Int32 dist, distSd;
        Int64 tsf = 0;
        dist = distSd = IScanResult::UNSPECIFIED;
        Int64 now = SystemClock::GetElapsedRealtime();
        Int32 bssidStrLen = BSSID_STR.GetLength();

        while (TRUE) {
            while (n < splitData->GetLength()) {
                if (DEBUG_PARSE) Logd(String("parsing ") + (*splitData)[n]);
                if ((*splitData)[n].Equals(END_OF_BATCHES)) {
                    if (n + 1 != splitData->GetLength()) {
                        Loge(String("didn't consume ") + (splitData->GetLength() - n));
                    }
                    Int32 size;
                    if ((mBatchedScanResults->GetSize(&size), size) > 0) {
                        mContext->SendBroadcastAsUser(intent, UserHandle::ALL);
                    }
                    Logd(String("retrieveBatchedScanResults X"));
                    return;
                }
                AutoPtr<IList> list;
                batchedScanResult->GetScanResults((IList**)&list);
                if (((*splitData)[n].Equals(END_STR)) || (*splitData)[n].Equals(DELIMITER_STR)) {
                    if (bssid != NULL) {
                        AutoPtr<IScanResult> sr;
                        CScanResult::New(wifiSsid, bssid, String(""), level, freq, tsf, dist, distSd, (IScanResult**)&sr);
                        list->Add(sr);
                        wifiSsid = NULL;
                        bssid = NULL;
                        level = 0;
                        freq = 0;
                        tsf = 0;
                        dist = distSd = IScanResult::UNSPECIFIED;
                    }
                    if ((*splitData)[n].Equals(END_STR)) {
                        Int32 size;
                        list->GetSize(&size);
                        if (size != 0) {
                            mBatchedScanResults->Add(batchedScanResult);
                            CBatchedScanResult::New((IBatchedScanResult**)&batchedScanResult);
                        }
                        else {
                            Logd(String("Found empty batch"));
                        }
                    }
                }
                else if ((*splitData)[n].Equals(TRUNCATED)) {
                    batchedScanResult->SetTruncated(TRUE);
                }
                else if ((*splitData)[n].StartWith(BSSID_STR)) {
                    bssid = String(*(*splitData)[n].GetBytes(), bssidStrLen,
                            (*splitData)[n].GetLength() - bssidStrLen);
                }
                else if ((*splitData)[n].StartWith(FREQ_STR)) {
                    // try {
                    freq = StringUtils::ParseInt32((*splitData)[n].Substring(FREQ_STR.GetLength()));
                    // } catch (NumberFormatException e) {
                    //     Loge("Invalid freqency: " + (*splitData)[n]);
                    //     freq = 0;
                    // }
                }
                else if ((*splitData)[n].StartWith(AGE)) {
                    // try {
                    tsf = now - StringUtils::ParseInt64((*splitData)[n].Substring(AGE.GetLength()));
                    tsf *= 1000; // convert mS -> uS
                    // } catch (NumberFormatException e) {
                    //     Loge("Invalid timestamp: " + (*splitData)[n]);
                    //     tsf = 0;
                    // }
                }
                else if ((*splitData)[n].StartWith(SSID_STR)) {
                    AutoPtr<IWifiSsidHelper> helper;
                    CWifiSsidHelper::AcquireSingleton((IWifiSsidHelper**)&helper);
                    helper->CreateFromAsciiEncoded((*splitData)[n].Substring(SSID_STR.GetLength()), (IWifiSsid**)&wifiSsid);
                }
                else if ((*splitData)[n].StartWith(LEVEL_STR)) {
                    // try {
                    level = StringUtils::ParseInt32((*splitData)[n].Substring(LEVEL_STR.GetLength()));
                    if (level > 0) level -= 256;
                    // } catch (NumberFormatException e) {
                    //     Loge("Invalid level: " + (*splitData)[n]);
                    //     level = 0;
                    // }
                }
                else if ((*splitData)[n].StartWith(DIST)) {
                    // try {
                    dist = StringUtils::ParseInt32((*splitData)[n].Substring(DIST.GetLength()));
                    // } catch (NumberFormatException e) {
                    //     Loge("Invalid distance: " + (*splitData)[n]);
                    //     dist = ScanResult.UNSPECIFIED;
                    // }
                }
                else if ((*splitData)[n].StartWith(DISTSD)) {
                    // try {
                    distSd = StringUtils::ParseInt32((*splitData)[n].Substring(DISTSD.GetLength()));
                    // } catch (NumberFormatException e) {
                    //     Loge("Invalid distanceSd: " + (*splitData)[n]);
                    //     distSd = ScanResult.UNSPECIFIED;
                    // }
                }
                else {
                    Loge(String("Unable to parse batched scan result line: ") + (*splitData)[n]);
                }
                n++;
            }
            rawData = mWifiNative->GetBatchedScanResults();
            if (DEBUG_PARSE) Log(String("reading more data:\n") + rawData);
            if (rawData == NULL) {
                Loge(String("Unexpected NULL BatchedScanResults"));
                return;
            }
            StringUtils::Split(rawData, String("\n"), (ArrayOf<String>**)&splitData);
            if (splitData->GetLength() == 0 || (*splitData)[0].Equals("ok")) {
                Loge(String("batch scan results just ended!"));
                Int32 size;
                if ((mBatchedScanResults->GetSize(&size), size) > 0) {
                    mContext->SendStickyBroadcastAsUser(intent, UserHandle::ALL);
                }
                return;
            }
            n = 0;
        }
    }
}

Boolean WifiStateMachine::IsScanAllowed(
    /* [in] */ Int32 scanSource)
{
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    Int64 now;
    system->GetCurrentTimeMillis(&now);
    if (lastConnectAttempt != 0 && (now - lastConnectAttempt) < 10000) {
        return FALSE;
    }
    Boolean b;
    if (mP2pConnected->Get(&b), b) {
        if (scanSource == SCAN_ALARM_SOURCE) {
            if (VDBG) {
                Logd(String("P2P connected: lastScanDuringP2p=") +
                     lastScanDuringP2p +
                     " CurrentTime=" + now +
                     " autoJoinScanIntervalWhenP2pConnected=" +
                     StringUtils::ToString(mWifiConfigStore->autoJoinScanIntervalWhenP2pConnected));
            }

            if (lastScanDuringP2p == 0 || (now - lastScanDuringP2p)
                < mWifiConfigStore->autoJoinScanIntervalWhenP2pConnected) {
                if (lastScanDuringP2p == 0) lastScanDuringP2p = now;
                if (VDBG) {
                    Logd(String("P2P connected, discard scan within ") +
                        StringUtils::ToString(mWifiConfigStore->autoJoinScanIntervalWhenP2pConnected)
                         + " milliseconds");
                }
                return FALSE;
            }
            lastScanDuringP2p = now;
        }
    }
    else {
        lastScanDuringP2p = 0;
    }
    return TRUE;
}

void WifiStateMachine::NoteScanStart(
    /* [in] */ Int32 callingUid,
    /* [in] */ IWorkSource* workSource)
{
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    Int64 now;
    system->GetCurrentTimeMillis(&now);
    lastStartScanTimeStamp = now;
    lastScanDuration = 0;
    if (DBG) {
        String ts;
        ts.AppendFormat("[%,d ms]", now);
        if (workSource != NULL) {
            String str;
            IObject::Probe(workSource)->ToString(&str);
            Loge(ts + " noteScanStart" + str
                    + " uid " + StringUtils::ToString(callingUid));
        }
        else {
            Loge(ts + " noteScanstart no scan source");
        }
    }
    StartRadioScanStats();
    if (mScanWorkSource == NULL && ((callingUid != UNKNOWN_SCAN_SOURCE
            && callingUid != SCAN_ALARM_SOURCE)
            || workSource != NULL)) {
        if (workSource != NULL) {
            mScanWorkSource = workSource;
        }
        else {
            CWorkSource::New(callingUid, (IWorkSource**)&mScanWorkSource);
        }
        // try {
        mBatteryStats->NoteWifiScanStartedFromSource(mScanWorkSource);
        // } catch (RemoteException e) {
        //     log(e.toString());
        // }
    }
}

void WifiStateMachine::NoteScanEnd()
{
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    Int64 now;
    system->GetCurrentTimeMillis(&now);
    if (lastStartScanTimeStamp != 0) {
        lastScanDuration = now - lastStartScanTimeStamp;
    }
    lastStartScanTimeStamp = 0;
    if (DBG) {
        String ts;
        ts.AppendFormat("[%,d ms]", now);
        if (mScanWorkSource != NULL) {
            String str;
            IObject::Probe(mScanWorkSource)->ToString(&str);
            Loge(ts + " noteScanEnd " + str);
        }
        else
            Loge(ts + " noteScanEnd no scan source");
    }
    if (mScanWorkSource != NULL) {
        // try {
        mBatteryStats->NoteWifiScanStoppedFromSource(mScanWorkSource);
        // } catch (RemoteException e) {
        //     log(e.toString());
        // } finally {
        mScanWorkSource = NULL;
        // }
    }
}

void WifiStateMachine::NoteBatchedScanStart()
{
    if (PDBG) Loge(String("noteBatchedScanstart()"));
    // note the end of a previous scan set
    Boolean b;
    if (mNotedBatchedScanWorkSource != NULL &&
            ((IObject::Probe(mNotedBatchedScanWorkSource)->Equals(mBatchedScanWorkSource, &b), b) == FALSE ||
             mNotedBatchedScanCsph != mBatchedScanCsph)) {
        // try {
        mBatteryStats->NoteWifiBatchedScanStoppedFromSource(mNotedBatchedScanWorkSource);
        // } catch (RemoteException e) {
        //     log(e.toString());
        // } finally {
        mNotedBatchedScanWorkSource = NULL;
        mNotedBatchedScanCsph = 0;
        // }
    }
    // note the start of the new
    // try {
    mBatteryStats->NoteWifiBatchedScanStartedFromSource(mBatchedScanWorkSource,
            mBatchedScanCsph);
    mNotedBatchedScanWorkSource = mBatchedScanWorkSource;
    mNotedBatchedScanCsph = mBatchedScanCsph;
    // } catch (RemoteException e) {
    //     log(e.toString());
    // }
}

void WifiStateMachine::NoteBatchedScanStop()
{
    if (PDBG) Loge(String("noteBatchedScanstop()"));

    if (mNotedBatchedScanWorkSource != NULL) {
        // try {
        mBatteryStats->NoteWifiBatchedScanStoppedFromSource(mNotedBatchedScanWorkSource);
        // } catch (RemoteException e) {
        //     log(e.toString());
        // } finally {
        mNotedBatchedScanWorkSource = NULL;
        mNotedBatchedScanCsph = 0;
        // }
    }
}

void WifiStateMachine::HandleScanRequest(
    /* [in] */ Int32 type,
    /* [in] */ IMessage* message)
{
    // unbundle parameters
    AutoPtr<IInterface> obj;
    message->GetObj((IInterface**)&obj);
    AutoPtr<IBundle> bundle = IBundle::Probe(obj);
    AutoPtr<IParcelable> p;
    bundle->GetParcelable(CUSTOMIZED_SCAN_SETTING, (IParcelable**)&p);
    AutoPtr<IScanSettings> settings = IScanSettings::Probe(p);
    bundle->GetParcelable(CUSTOMIZED_SCAN_WORKSOURCE, (IParcelable**)&p);
    AutoPtr<IWorkSource> workSource = IWorkSource::Probe(p);

    // parse scan settings
    String freqs;
    if (settings != NULL) {
        AutoPtr<ICollection> channelSet;
        settings->GetChannelSet((ICollection**)&channelSet);
        if (channelSet != NULL) {
            StringBuilder sb;
            Boolean first = TRUE;
            AutoPtr<ArrayOf<IInterface*> > array;
            channelSet->ToArray((ArrayOf<IInterface*>**)&array);
            for (Int32 i = 0; i < array->GetLength(); i++) {
                AutoPtr<IWifiChannel> channel = IWifiChannel::Probe((*array)[i]);
                if (!first) sb.Append(',');
                else first = FALSE;
                Int32 freqMHz;
                channel->GetFreqMHz(&freqMHz);
                sb.Append(freqMHz);
            }
            sb.ToString(&freqs);
        }
    }

    // call wifi native to start the scan
    Int32 arg1;
    message->GetArg1(&arg1);
    Int32 arg2;
    message->GetArg2(&arg2);
    if (StartScanNative(type, freqs)) {
        // only count battery consumption if scan request is accepted
        NoteScanStart(arg1, workSource);
        // a full scan covers everything, clearing scan request buffer
        if (freqs == NULL)
            mBufferedScanMsg->Clear();
        messageHandlingStatus = MESSAGE_HANDLING_STATUS_OK;
        return;
    }

    // if reach here, scan request is rejected

    if (!mIsScanOngoing) {
        // if rejection is NOT due to ongoing scan (e.g. bad scan parameters),

        // discard this request and pop up the next one
        Int32 size;
        mBufferedScanMsg->GetSize(&size);
        if (size > 0) {
            AutoPtr<IInterface> obj;
            mBufferedScanMsg->Remove((IInterface**)&obj);
            SendMessage(IMessage::Probe(obj));
        }
        messageHandlingStatus = MESSAGE_HANDLING_STATUS_DISCARD;
    }
    else if (!mIsFullScanOngoing) {
        // if rejection is due to an ongoing scan, and the ongoing one is NOT a full scan,
        // buffer the scan request to make sure specified channels will be scanned eventually
        if (freqs == NULL) {
            mBufferedScanMsg->Clear();
        }
        Int32 size;
        mBufferedScanMsg->GetSize(&size);
        if (size < SCAN_REQUEST_BUFFER_MAX_SIZE) {
            AutoPtr<IMessage> msg;
            ObtainMessage(CMD_START_SCAN, arg1, arg2, bundle, (IMessage**)&msg);
            mBufferedScanMsg->Add(msg);
        }
        else {
            // if too many requests in buffer, combine them into a single full scan
            bundle = NULL;
            CBundle::New((IBundle**)&bundle);
            bundle->PutParcelable(CUSTOMIZED_SCAN_SETTING, NULL);
            bundle->PutParcelable(CUSTOMIZED_SCAN_WORKSOURCE, IParcelable::Probe(workSource));
            AutoPtr<IMessage> msg;
            ObtainMessage(CMD_START_SCAN, arg1, arg2, bundle, (IMessage**)&msg);
            mBufferedScanMsg->Clear();
            mBufferedScanMsg->Add(msg);
        }
        messageHandlingStatus = MESSAGE_HANDLING_STATUS_LOOPED;
    }
    else {
        // mIsScanOngoing and mIsFullScanOngoing
        messageHandlingStatus = MESSAGE_HANDLING_STATUS_FAIL;
    }
}

Boolean WifiStateMachine::StartScanNative(
    /* [in] */ Int32 type,
    /* [in] */ const String& freqs)
{
    if (mWifiNative->Scan(type, freqs)) {
        mIsScanOngoing = TRUE;
        mIsFullScanOngoing = (freqs == NULL);
        lastScanFreqs = freqs;
        return TRUE;
    }
    return FALSE;
}

void WifiStateMachine::LogStateAndMessage(
    /* [in] */ IMessage* message,
    /* [in] */ const String& state)
{
    messageHandlingStatus = 0;
    if (mLogMessages) {
        //long now = SystemClock.elapsedRealtimeNanos();
        //String ts = String.format("[%,d us]", now/1000);

        Loge(String(" ") + state + " " + GetLogRecString(message));
    }
}

void WifiStateMachine::HandleBSSIDBlacklist(
    /* [in] */ Boolean enable,
    /* [in] */ const String& bssid,
    /* [in] */ Int32 reason)
{
    Log(String("Blacklisting BSSID: ") + bssid + ",reason:"
            + StringUtils::ToString(reason) + ",enable:"
            + StringUtils::BooleanToString(enable));
    if (bssid != NULL) {
        // Tell configStore to black list it
        synchronized(mScanResultCache) {
            mWifiAutoJoinController->HandleBSSIDBlackList( enable, bssid, reason );
            mWifiConfigStore->HandleDisabledAPs( enable, bssid, reason );
        }
    }
}

void WifiStateMachine::HandleStateChange(
    /* [in] */ Int32 state)
{
    Int32 offset;
    Log(String("handle state change: ") + StringUtils::ToString(state));
    if(state == 0) {
        // wifi is not good, reduce the score
        mWifiInfo->SetScore(1);
    }
    else {
        // wifi is good, increase the score
        mWifiInfo->SetScore(INetworkAgent::WIFI_BASE_SCORE);
    }
    if(mNetworkAgent != NULL) {
        Int32 score;
        mWifiInfo->GetScore(&score);
        mNetworkAgent->SendNetworkScore(score);
    }
}

void WifiStateMachine::HandlePrefChange(
    /* [in] */ Int32 featureId,
    /* [in] */ Int32 featureParam,
    /* [in] */ Int32 value)
{
    Log(String("handle pref change : featurevalue: ") + StringUtils::ToString(value));
    if(featureId == FEATURE_ID && featureParam == FEATURE_PARAM) {
        if(value == FEATURE_ON) {
            DEFAULT_SCORE = 1;
            isPropFeatureEnabled = TRUE;
        }
        else if(value == FEATURE_OFF) {
            DEFAULT_SCORE = INetworkAgent::WIFI_BASE_SCORE;
            isPropFeatureEnabled = FALSE;
        }
    }
}

void WifiStateMachine::HandleScreenStateChanged(
    /* [in] */ Boolean screenOn,
    /* [in] */ Boolean startBackgroundScanIfNeeded)
{
    mScreenOn = screenOn;
    Boolean b;
    mUserWantsSuspendOpt->Get(&b);
    if (PDBG) {
        String name;
        Loge(String(" handleScreenStateChanged Enter: screenOn=")
                + StringUtils::BooleanToString(screenOn)
                + " mCurrentScanAlarmMs = " + StringUtils::ToString(mCurrentScanAlarmMs)
                + " mUserWantsSuspendOpt=" + StringUtils::BooleanToString(b)
                + " state " + (GetCurrentState()->GetName(&name), name)
                + " suppState:" + mSupplicantStateTracker->GetSupplicantStateName());
    }
    EnableRssiPolling(screenOn);
    if (screenOn) EnableAllNetworks();
    if (b) {
        if (screenOn) {
            SendMessage(CMD_SET_SUSPEND_OPT_ENABLED, 0, 0);
        }
        else {
            // Allow 2s for suspend optimizations to be set
            mSuspendWakeLock->AcquireLock(2000);
            SendMessage(CMD_SET_SUSPEND_OPT_ENABLED, 1, 0);
        }
    }
    mScreenBroadcastReceived->Set(TRUE);

    AutoPtr<IWifiLinkLayerStats> stats;
    GetWifiLinkLayerStats(FALSE, (IWifiLinkLayerStats**)&stats);
    mOnTimeScreenStateChange = mOnTime;
    lastScreenStateChangeTimeStamp = lastLinkLayerStatsUpdate;

    if (screenOn) {
        ClearBlacklist();

        fullBandConnectedTimeIntervalMilli = mWifiConfigStore->associatedPartialScanPeriodMilli;
        // Start the scan alarm so as to enable autojoin
        if (GetCurrentState() == mConnectedState
                && mWifiConfigStore->enableAutoJoinScanWhenAssociated) {
            mCurrentScanAlarmMs = mWifiConfigStore->associatedPartialScanPeriodMilli;
            // Scan after 200ms
            SetScanAlarm(TRUE, 200);
        }
        else if (GetCurrentState() == mDisconnectedState) {

            // Configure the scan alarm time to mFrameworkScanIntervalMs
            // (5 minutes) if there are no saved profiles as there is
            // already a periodic scan getting issued for every
            // mSupplicantScanIntervalMs seconds. However keep the
            // scan frequency by setting it to mDisconnectedScanPeriodMs
            // (10 seconds) when there are configured profiles.
            AutoPtr<IList> list;
            mWifiConfigStore->GetConfiguredNetworks((IList**)&list);
            Int32 size;
            list->GetSize(&size);
            if (size != 0) {
                mCurrentScanAlarmMs = mDisconnectedScanPeriodMs;
            }
            else {
                mCurrentScanAlarmMs = mFrameworkScanIntervalMs;
            }

            // Scan after 200ms
            SetScanAlarm(TRUE, 200);
        }
    }
    else {
        SetScanAlarm(FALSE, 0);
    }

    if (mBackgroundScanSupported) {
        mEnableBackgroundScan = (screenOn == FALSE);
    }

    if (DBG) Logd(String("backgroundScan enabled=")
            + StringUtils::BooleanToString(mEnableBackgroundScan)
            + " startBackgroundScanIfNeeded:"
            + StringUtils::BooleanToString(startBackgroundScanIfNeeded));

    if (startBackgroundScanIfNeeded) {
        if (mEnableBackgroundScan) {
            if (!mWifiNative->EnableBackgroundScan(TRUE)) {
                HandlePnoFailError();
            }
        } else {
           mWifiNative->EnableBackgroundScan(FALSE);
        }
    }

    if (DBG) Log(String("handleScreenStateChanged Exit: ")
            + StringUtils::BooleanToString(screenOn));
}

void WifiStateMachine::CheckAndSetConnectivityInstance()
{
    if (mCm == NULL) {
        AutoPtr<IInterface> service;
        mContext->GetSystemService(IContext::CONNECTIVITY_SERVICE, (IInterface**)&service);
        mCm = IConnectivityManager::Probe(service);
    }
}

Boolean WifiStateMachine::StartTethering(
    /* [in] */ IArrayList* available)
{
    Boolean wifiAvailable = FALSE;

    CheckAndSetConnectivityInstance();

    AutoPtr<ArrayOf<String> > wifiRegexs;
    mCm->GetTetherableWifiRegexs((ArrayOf<String>**)&wifiRegexs);

    AutoPtr<ArrayOf<IInterface *> > array;
    available->ToArray((ArrayOf<IInterface *>**)&array);
    for (Int32 i = 0; i < array->GetLength(); i++) {
        String intf;
        ICharSequence::Probe((*array)[i])->ToString(&intf);

        for (Int32 j = 0; j < wifiRegexs->GetLength(); j++) {
            String regex = (*wifiRegexs)[j];
            Boolean b;
            StringUtils::Matches(intf, regex, &b);
            if (b) {
                AutoPtr<IInterfaceConfiguration> ifcg;

                // try {
                mNwService->GetInterfaceConfig(intf, (IInterfaceConfiguration**)&ifcg);
                if (ifcg != NULL) {
                    /* IP/netmask: 192.168.43.1/255.255.255.0 */
                    AutoPtr<IInetAddress> inetAddr;
                    AutoPtr<INetworkUtils> networkUtils;
                    CNetworkUtils::AcquireSingleton((INetworkUtils**)&networkUtils);
                    networkUtils->NumericToInetAddress(String("192.168.43.1"), (IInetAddress**)&inetAddr);
                    AutoPtr<ILinkAddress> addr;
                    CLinkAddress::New(inetAddr, 24, (ILinkAddress**)&addr);
                    ifcg->SetLinkAddress(addr);
                    ifcg->SetInterfaceUp();

                    mNwService->SetInterfaceConfig(intf, ifcg);
                }
                // } catch (Exception e) {
                //     Loge("Error configuring interface " + intf + ", :" + e);
                //     return FALSE;
                // }

                Int32 val;
                mCm->Tether(intf, &val);
                if(val != IConnectivityManager::TETHER_ERROR_NO_ERROR) {
                    Loge(String("Error tethering on ") + intf);
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
    mNwService->GetInterfaceConfig(mTetherInterfaceName, (IInterfaceConfiguration**)&ifcg);
    if (ifcg != NULL) {
        AutoPtr<IInetAddress> inetAddr;
        AutoPtr<INetworkUtils> networkUtils;
        CNetworkUtils::AcquireSingleton((INetworkUtils**)&networkUtils);
        networkUtils->NumericToInetAddress(String("0.0.0.0"), (IInetAddress**)&inetAddr);
        AutoPtr<ILinkAddress> addr;
        CLinkAddress::New(inetAddr, 0, (ILinkAddress**)&addr);
        ifcg->SetLinkAddress(addr);
        mNwService->SetInterfaceConfig(mTetherInterfaceName, ifcg);
    }
    // } catch (Exception e) {
    //     Loge("Error resetting interface " + mTetherInterfaceName + ", :" + e);
    // }

    Int32 val;
    mCm->Untether(mTetherInterfaceName, &val);
    if (val != IConnectivityManager::TETHER_ERROR_NO_ERROR) {
        Loge(String("Untether initiate failed!"));
    }
}

Boolean WifiStateMachine::IsWifiTethered(
    /* [in] */ IArrayList* active)
{
    CheckAndSetConnectivityInstance();

    AutoPtr<ArrayOf<String> > wifiRegexs;
    mCm->GetTetherableWifiRegexs((ArrayOf<String>**)&wifiRegexs);

    AutoPtr<ArrayOf<IInterface *> > array;
    active->ToArray((ArrayOf<IInterface *>**)&array);
    for (Int32 i = 0; i < array->GetLength(); i++) {
        String intf;
        ICharSequence::Probe((*array)[i])->ToString(&intf);

        for (Int32 j = 0; j < wifiRegexs->GetLength(); j++) {
            String regex = (*wifiRegexs)[j];
            Boolean b;
            StringUtils::Matches(intf, regex, &b);
            if (b) {
                return TRUE;
            }
        }
    }
    // We found no interfaces that are tethered
    return FALSE;
}

void WifiStateMachine::SetCountryCode()
{
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    String countryCode;
    Settings::Global::GetString(cr, ISettingsGlobal::WIFI_COUNTRY_CODE, &countryCode);
    if (countryCode != NULL && !countryCode.IsEmpty()) {
        SetCountryCode(countryCode, FALSE);
    }
    else {
        //use driver default
    }
}

void WifiStateMachine::SetFrequencyBand()
{
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    Int32 band;
    Settings::Global::GetInt32(cr, ISettingsGlobal::WIFI_FREQUENCY_BAND,
            IWifiManager::WIFI_FREQUENCY_BAND_AUTO, &band);
    SetFrequencyBand(band, FALSE);
}

void WifiStateMachine::SetSuspendOptimizationsNative(
    /* [in] */ Int32 reason,
    /* [in] */ Boolean enabled)
{
    if (DBG) {
        // Log(String("setSuspendOptimizationsNative: ") + reason + " " + enabled
        //         + " -want " + mUserWantsSuspendOpt.get()
        //         + " stack:" + Thread.currentThread().getStackTrace()[2].getMethodName()
        //         +" - "+ Thread.currentThread().getStackTrace()[3].getMethodName()
        //         +" - "+ Thread.currentThread().getStackTrace()[4].getMethodName()
        //         +" - "+ Thread.currentThread().getStackTrace()[5].getMethodName());
    }
    //mWifiNative.setSuspendOptimizations(enabled);

    if (enabled) {
        mSuspendOptNeedsDisabled &= ~reason;
        /* None of dhcp, screen or highperf need it disabled and user wants it enabled */
        Boolean b;
        if (mSuspendOptNeedsDisabled == 0 && (mUserWantsSuspendOpt->Get(&b), b)) {
            if (DBG) {
                // Log("setSuspendOptimizationsNative do it " + reason + " " + enabled
                //         + " stack:" + Thread.currentThread().getStackTrace()[2].getMethodName()
                //         +" - "+ Thread.currentThread().getStackTrace()[3].getMethodName()
                //         +" - "+ Thread.currentThread().getStackTrace()[4].getMethodName()
                //         +" - "+ Thread.currentThread().getStackTrace()[5].getMethodName());
            }
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
    if (DBG) Log(String("setSuspendOptimizations: ") +
            StringUtils::ToString(reason) + " " +
            StringUtils::BooleanToString(enabled));
    if (enabled) {
        mSuspendOptNeedsDisabled &= ~reason;
    }
    else {
        mSuspendOptNeedsDisabled |= reason;
    }
    if (DBG) Log(String("mSuspendOptNeedsDisabled ")
            + StringUtils::ToString(mSuspendOptNeedsDisabled));
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
    //     Loge("Failed to note battery stats in wifi");
    // }

    mWifiState->Set(wifiState);

    String str;
    SyncGetWifiStateByName(&str);
    if (DBG) Log(String("setWifiState: ") + str);

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
    //     Loge("Failed to note battery stats in wifi");
    // }

    // Update state
    mWifiApState->Set(wifiApState);

    String str;
    SyncGetWifiApStateByName(&str);
    if (DBG) Log(String("setWifiApState: ") + str);

    AutoPtr<IIntent> intent;
    CIntent::New(IWifiManager::WIFI_AP_STATE_CHANGED_ACTION, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
    intent->PutExtra(IWifiManager::EXTRA_WIFI_AP_STATE, wifiApState);
    intent->PutExtra(IWifiManager::EXTRA_PREVIOUS_WIFI_AP_STATE, previousWifiApState);
    mContext->SendStickyBroadcastAsUser(intent, UserHandle::ALL);
}

void WifiStateMachine::SetScanResults()
{
    mNumScanResultsKnown = 0;
    mNumScanResultsReturned = 0;
    String bssid = String("");
    Int32 level = 0;
    Int32 freq = 0;
    Int64 tsf = 0;
    String flags = String("");
    AutoPtr<IWifiSsid> wifiSsid;
    String scanResults;
    String tmpResults;
    StringBuffer scanResultsBuf;
    Int32 sid = 0;

    while (TRUE) {
        tmpResults = mWifiNative->ScanResults(sid);
        if (tmpResults.IsEmpty()) break;
        scanResultsBuf.Append(tmpResults);
        scanResultsBuf.Append("\n");
        AutoPtr<ArrayOf<String> > lines;
        StringUtils::Split(tmpResults, String("\n"), (ArrayOf<String>**)&lines);
        sid = -1;
        for (Int32 i = lines->GetLength() - 1; i >= 0; i--) {
            if ((*lines)[i].StartWith(END_STR)) {
                break;
            }
            else if ((*lines)[i].StartWith(ID_STR)) {
                // try {
                sid = StringUtils::ParseInt32((*lines)[i].Substring(ID_STR.GetLength())) + 1;
                // } catch (NumberFormatException e) {
                //     // Nothing to do
                // }
                break;
            }
        }
        if (sid == -1) break;
    }

    // Age out scan results, we return all scan results found in the last 12 seconds,
    // and NOT all scan results since last scan.
    // ageOutScanResults(12000);

    scanResultsBuf.ToString(&scanResults);
    if (scanResults.IsEmpty()) {
        emptyScanResultCount++;
        if (emptyScanResultCount > 10) {
            // If we got too many empty scan results, the current scan cache is stale,
            // hence clear it.
            CArrayList::New((IArrayList**)&mScanResults);
        }
       return;
    }

    emptyScanResultCount = 0;

    // note that all these splits and substrings keep references to the original
    // huge string buffer while the amount we really want is generally pretty small
    // so make copies instead (one example b/11087956 wasted 400k of heap here).
    synchronized(mScanResultCache) {
        CArrayList::New((IArrayList**)&mScanResults);
        AutoPtr<ArrayOf<String> > lines;
        StringUtils::Split(scanResults, String("\n"), (ArrayOf<String>**)&lines);
        Int32 bssidStrLen = BSSID_STR.GetLength();
        Int32 flagLen = FLAGS_STR.GetLength();

        for (Int32 i = 0; i < lines->GetLength(); i++) {
            String line = (*lines)[i];
            if (line.StartWith(BSSID_STR)) {
                bssid = String(*line.GetBytes(), bssidStrLen, line.GetLength() - bssidStrLen);
            }
            else if (line.StartWith(FREQ_STR)) {
                // try {
                freq = StringUtils::ParseInt32(line.Substring(FREQ_STR.GetLength()));
                // } catch (NumberFormatException e) {
                //     freq = 0;
                // }
            }
            else if (line.StartWith(LEVEL_STR)) {
                // try {
                level = StringUtils::ParseInt32(line.Substring(LEVEL_STR.GetLength()));
                /* some implementations avoid negative values by adding 256
                 * so we need to adjust for that here.
                 */
                if (level > 0) level -= 256;
                // } catch(NumberFormatException e) {
                //     level = 0;
                // }
            }
            else if (line.StartWith(TSF_STR)) {
                // try {
                tsf = StringUtils::ParseInt64(line.Substring(TSF_STR.GetLength()));
                // } catch (NumberFormatException e) {
                //     tsf = 0;
                // }
            }
            else if (line.StartWith(FLAGS_STR)) {
                flags = String(*line.GetBytes(), flagLen, line.GetLength() - flagLen);
            }
            else if (line.StartWith(SSID_STR)) {
                AutoPtr<IWifiSsidHelper> helper;
                CWifiSsidHelper::AcquireSingleton((IWifiSsidHelper**)&helper);
                helper->CreateFromAsciiEncoded(line.Substring(SSID_STR.GetLength()), (IWifiSsid**)&wifiSsid);
            }
            else if (line.StartWith(DELIMITER_STR) || line.StartWith(END_STR)) {
                if (bssid != NULL) {
                    String str;
                    String ssid = (wifiSsid != NULL) ? (IObject::Probe(wifiSsid)->ToString(&str), str) : IWifiSsid::NONE;
                    String key = bssid + ssid;

                    AutoPtr<ISystem> system;
                    CSystem::AcquireSingleton((ISystem**)&system);
                    AutoPtr<IScanResult> scanResult = mScanResultCache->Get(key);
                    if (scanResult != NULL) {
                        // TODO: average the RSSI, instead of overwriting it
                        scanResult->SetLevel(level);
                        scanResult->SetWifiSsid(wifiSsid);
                        // Keep existing API
                        scanResult->SetSSID((wifiSsid != NULL) ?
                                (IObject::Probe(wifiSsid)->ToString(&str), str) : IWifiSsid::NONE);
                        scanResult->SetCapabilities(flags);
                        scanResult->SetFrequency(freq);
                        scanResult->SetTimestamp(tsf);

                        Int64 currentTimeMillis;
                        system->GetCurrentTimeMillis(&currentTimeMillis);
                        scanResult->SetSeen(currentTimeMillis);
                    }
                    else {
                        CScanResult::New(wifiSsid, bssid, flags, level, freq, tsf, (IScanResult**)&scanResult);
                        Int64 currentTimeMillis;
                        system->GetCurrentTimeMillis(&currentTimeMillis);
                        scanResult->SetSeen(currentTimeMillis);
                        mScanResultCache->Put(key, scanResult);
                    }
                    mNumScanResultsReturned ++; // Keep track of how many scan results we got
                                                // as part of this scan's processing
                    mScanResults->Add(scanResult);
                }
                bssid = NULL;
                level = 0;
                freq = 0;
                tsf = 0;
                flags = String("");
                wifiSsid = NULL;
            }
        }
    }
    Boolean attemptAutoJoin = TRUE;
    AutoPtr<ISupplicantState> state;
    mWifiInfo->GetSupplicantState((ISupplicantState**)&state);
    SupplicantState ss;
    state->Get(&ss);
    AutoPtr<IState> s = GetCurrentState();
    if (s == mRoamingState
            || s == mObtainingIpState
            || s == mScanModeState
            || s == mDisconnectingState
            || (s == mConnectedState
            && !mWifiConfigStore->enableAutoJoinWhenAssociated)
            || linkDebouncing
            || ss == Elastos::Droid::Wifi::SupplicantState_ASSOCIATING
            || ss == Elastos::Droid::Wifi::SupplicantState_AUTHENTICATING
            || ss == Elastos::Droid::Wifi::SupplicantState_FOUR_WAY_HANDSHAKE
            || ss == Elastos::Droid::Wifi::SupplicantState_GROUP_HANDSHAKE) {
        // Dont attempt auto-joining again while we are already attempting to join
        // and/or obtaining Ip address
        attemptAutoJoin = FALSE;
    }
    if (DBG) {
        // Loge(String("wifi setScanResults state") + s
        //         + " sup_state=" + state
        //         + " debouncing=" + linkDebouncing);
    }
    if (attemptAutoJoin) {
        messageHandlingStatus = MESSAGE_HANDLING_STATUS_PROCESSED;
    }
    // Loose last selected configuration if we have been disconnected for 30 minutes
    Int64 milli;
    if ((GetDisconnectedTimeMilli(&milli), milli) > 1000 * 60 * 30) {
        mWifiConfigStore->SetLastSelectedConfiguration(IWifiConfiguration::INVALID_NETWORK_ID);
    }

    if (mWifiConfigStore->enableAutoJoinWhenAssociated) {
        synchronized(mScanResultCache) {
            // AutoJoincontroller will directly acces the scan result list and update it with
            // ScanResult status
            mWifiAutoJoinController->NewSupplicantResults(attemptAutoJoin, &mNumScanResultsKnown);
        }
    }
    if (linkDebouncing) {
        // If debouncing, we dont re-select a SSID or BSSID hence
        // there is no need to call the network selection code
        // in WifiAutoJoinController, instead,
        // just try to reconnect to the same SSID by triggering a roam
        SendMessage(CMD_AUTO_ROAM, mLastNetworkId, 1, NULL);
    }
}

void WifiStateMachine::FetchRssiLinkSpeedAndFrequencyNative()
{
    Int32 newRssi = -1;
    Int32 newLinkSpeed = -1;
    Int32 newFrequency = -1;

    String signalPoll = mWifiNative->SignalPoll();

    if (signalPoll != NULL) {
        AutoPtr<ArrayOf<String> > lines;
        StringUtils::Split(signalPoll, String("\n"), (ArrayOf<String>**)&lines);
        for (Int32 i = 0; i < lines->GetLength(); i++) {
            String line = (*lines)[i];

            AutoPtr<ArrayOf<String> > prop;
            StringUtils::Split(line, String("="), (ArrayOf<String>**)&prop);
            if (prop->GetLength() < 2) continue;
            // try {
            if ((*prop)[0].Equals("RSSI")) {
                newRssi = StringUtils::ParseInt32((*prop)[1]);
            }
            else if ((*prop)[0].Equals("LINKSPEED")) {
                newLinkSpeed = StringUtils::ParseInt32((*prop)[1]);
            }
            else if ((*prop)[0].Equals("FREQUENCY")) {
                newFrequency = StringUtils::ParseInt32((*prop)[1]);
            }
            // } catch (NumberFormatException e) {
            //     //Ignore, defaults on rssi and linkspeed are assigned
            // }
        }
    }

    if (PDBG) {
        Loge(String("fetchRssiLinkSpeedAndFrequencyNative rssi=")
                + StringUtils::ToString(newRssi) + " linkspeed="
                + StringUtils::ToString(newLinkSpeed));
    }

    if (newRssi > IWifiInfo::INVALID_RSSI && newRssi < IWifiInfo::MAX_RSSI) {
    // screen out invalid values
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
        Int32 newSignalLevel;
        AutoPtr<IWifiManagerHelper> helper;
        CWifiManagerHelper::AcquireSingleton((IWifiManagerHelper**)&helper);
        helper->CalculateSignalLevel(newRssi, IWifiManager::RSSI_LEVELS, &newSignalLevel);
        if (newSignalLevel != mLastSignalLevel) {
            SendRssiChangeBroadcast(newRssi);
        }
        mLastSignalLevel = newSignalLevel;
    }
    else {
        mWifiInfo->SetRssi(IWifiInfo::INVALID_RSSI);
    }

    if (newLinkSpeed != -1) {
        mWifiInfo->SetLinkSpeed(newLinkSpeed);
    }
    if (newFrequency > 0) {
        AutoPtr<IScanResultHelper> helper;
        CScanResultHelper::AcquireSingleton((IScanResultHelper**)&helper);
        Boolean b;
        if (helper->Is5GHz(newFrequency, &b), b) {
            Int32 val;
            mWifiConnectionStatistics->GetNum24GhzConnected(&val);
            mWifiConnectionStatistics->SetNum5GhzConnected(++val);
        }
        if (helper->Is24GHz(newFrequency, &b), b) {
            Int32 val;
            mWifiConnectionStatistics->GetNum24GhzConnected(&val);
            mWifiConnectionStatistics->SetNum24GhzConnected(++val);
        }
        mWifiInfo->SetFrequency(newFrequency);
    }
    mWifiConfigStore->UpdateConfiguration(mWifiInfo);
}

void WifiStateMachine::CleanWifiScore()
{
    mWifiInfo->SetTxBadRate(0);
    mWifiInfo->SetTxSuccessRate(0);
    mWifiInfo->SetTxRetriesRate(0);
    mWifiInfo->SetRxSuccessRate(0);
}

void WifiStateMachine::CalculateWifiScore(
    /* [in] */ IWifiLinkLayerStats* stats)
{
    StringBuilder sb;
    if (stats == NULL || mWifiLinkLayerStatsSupported <= 0) {
        AutoPtr<ITrafficStats> helper;
        CTrafficStats::AcquireSingleton((ITrafficStats**)&helper);
        Int64 mTxPkts;
        helper->GetTxPackets(mInterfaceName, &mTxPkts);
        Int64 mRxPkts;
        helper->GetRxPackets(mInterfaceName, &mRxPkts);
        mWifiInfo->UpdatePacketRates(mTxPkts, mRxPkts);
    }
    else {
        sb.Append(" stats");
        mWifiInfo->UpdatePacketRates(stats);
    }
    Int32 score = 56; // Starting score, temporarily hardcoded in between 50 and 60
    Boolean b;
    Int32 speed;
    mWifiInfo->GetLinkSpeed(&speed);
    Boolean isBadLinkspeed = ((mWifiInfo->Is24GHz(&b), b)
            && speed < mWifiConfigStore->badLinkSpeed24)
            || ((mWifiInfo->Is5GHz(&b), b) && speed
            < mWifiConfigStore->badLinkSpeed5);
    Boolean isGoodLinkspeed = ((mWifiInfo->Is24GHz(&b), b)
            && speed >= mWifiConfigStore->goodLinkSpeed24)
            || ((mWifiInfo->Is5GHz(&b), b) && speed
            >= mWifiConfigStore->goodLinkSpeed5);

    if (isBadLinkspeed) {
        if (mBadLinkspeedcount < 6)
            mBadLinkspeedcount++;
    }
    else {
        if (mBadLinkspeedcount > 0)
            mBadLinkspeedcount--;
    }

    if (isBadLinkspeed) {
        sb.Append(" bl(");
        sb.Append(mBadLinkspeedcount);
        sb.Append(")");
    }
    if (isGoodLinkspeed) sb.Append(" gl");

    /**
     * We want to make sure that we use the 24GHz RSSI thresholds if
     * there are 2.4GHz scan results
     * otherwise we end up lowering the score based on 5GHz values
     * which may cause a switch to LTE before roaming has a chance to try 2.4GHz
     * We also might unblacklist the configuation based on 2.4GHz
     * thresholds but joining 5GHz anyhow, and failing over to 2.4GHz because 5GHz is not good
     */
    Boolean use24Thresholds = FALSE;
    Boolean homeNetworkBoost = FALSE;
    AutoPtr<IWifiConfiguration> currentConfiguration;
    GetCurrentWifiConfiguration((IWifiConfiguration**)&currentConfiguration);
    if (currentConfiguration != NULL) {
        AutoPtr<IHashMap> scanResultCache;
        currentConfiguration->GetScanResultCache((IHashMap**)&scanResultCache);
        if (scanResultCache != NULL) {
            AutoPtr<IWifiConfigurationVisibility> visibility;
            currentConfiguration->SetVisibility(12000, (IWifiConfigurationVisibility**)&visibility);
            if (visibility != NULL) {
                Int32 rssi24, rssi5;
                visibility->GetRssi24(&rssi24);
                visibility->GetRssi5(&rssi5);
                if (rssi24 != IWifiConfiguration::INVALID_RSSI
                        && rssi24 >= (rssi5-2)) {
                    use24Thresholds = TRUE;
                }
            }
            Int32 size;
            scanResultCache->GetSize(&size);
            AutoPtr<IBitSet> allowedKeyManagement;
            currentConfiguration->GetAllowedKeyManagement((IBitSet**)&allowedKeyManagement);
            Int32 val;
            allowedKeyManagement->Cardinality(&val);
            if (size <= 6
                && val == 1
                && (allowedKeyManagement->Get(IWifiConfigurationKeyMgmt::WPA_PSK, &b), b) == TRUE) {
                // A PSK network with less than 6 known BSSIDs
                // This is most likely a home network and thus we want to stick to wifi more
                homeNetworkBoost = TRUE;
            }
        }
    }
    if (homeNetworkBoost) sb.Append(" hn");
    if (use24Thresholds) sb.Append(" u24");

    Int32 r;
    mWifiInfo->GetRssi(&r);
    Int32 rssi = r - 6 * mAggressiveHandover
            + (homeNetworkBoost ? IWifiConfiguration::HOME_NETWORK_RSSI_BOOST : 0);
    String tmp;
    sb.Append(tmp.AppendFormat(" rssi=%d ag=%d", rssi, mAggressiveHandover));

    Boolean is24GHz = use24Thresholds || (mWifiInfo->Is24GHz(&b), b);

    Boolean isBadRSSI = (is24GHz && rssi < mWifiConfigStore->thresholdBadRssi24)
            || (!is24GHz && rssi < mWifiConfigStore->thresholdBadRssi5);
    Boolean isLowRSSI = (is24GHz && rssi < mWifiConfigStore->thresholdLowRssi24)
            || (!is24GHz && r < mWifiConfigStore->thresholdLowRssi5);
    Boolean isHighRSSI = (is24GHz && rssi >= mWifiConfigStore->thresholdGoodRssi24)
            || (!is24GHz && r >= mWifiConfigStore->thresholdGoodRssi5);

    if (isBadRSSI) sb.Append(" br");
    if (isLowRSSI) sb.Append(" lr");
    if (isHighRSSI) sb.Append(" hr");

    Int32 penalizedDueToUserTriggeredDisconnect = 0;        // For debug information
    Double d1, d2;
    if (currentConfiguration!= NULL &&
            ((mWifiInfo->GetTxSuccessRate(&d1), d1) > 5 || (mWifiInfo->GetRxSuccessRate(&d2), d2) > 5)) {
        if (isBadRSSI) {
            Int32 val;
            currentConfiguration->GetNumTicksAtBadRSSI(&val);
            currentConfiguration->SetNumTicksAtBadRSSI(++val);
            currentConfiguration->GetNumTicksAtBadRSSI(&val);
            Int32 badRSSI, lowRSSI, highRSSI;
            if (val > 1000) {
                // We remained associated for a compound amount of time while passing
                // traffic, hence loose the corresponding user triggered disabled stats
                currentConfiguration->GetNumUserTriggeredWifiDisableBadRSSI(&badRSSI);
                if (badRSSI > 0) {
                    currentConfiguration->SetNumUserTriggeredWifiDisableBadRSSI(--badRSSI);
                }
                currentConfiguration->GetNumUserTriggeredWifiDisableLowRSSI(&lowRSSI);
                if (lowRSSI > 0) {
                    currentConfiguration->SetNumUserTriggeredWifiDisableLowRSSI(--lowRSSI);
                }
                currentConfiguration->GetNumUserTriggeredWifiDisableNotHighRSSI(&highRSSI);
                if (highRSSI > 0) {
                    currentConfiguration->SetNumUserTriggeredWifiDisableNotHighRSSI(--highRSSI);
                }
                currentConfiguration->SetNumTicksAtBadRSSI(0);
            }
            if (mWifiConfigStore->enableWifiCellularHandoverUserTriggeredAdjustment &&
                    (badRSSI > 0
                    || lowRSSI > 0
                    || highRSSI > 0)) {
                score = score -5;
                penalizedDueToUserTriggeredDisconnect = 1;
                sb.Append(" p1");
            }
        }
        else if (isLowRSSI) {
            Int32 val;
            currentConfiguration->GetNumTicksAtLowRSSI(&val);
            currentConfiguration->SetNumTicksAtLowRSSI(++val);
            currentConfiguration->GetNumTicksAtLowRSSI(&val);
            Int32 lowRSSI, highRSSI;
            currentConfiguration->GetNumUserTriggeredWifiDisableLowRSSI(&lowRSSI);
            currentConfiguration->GetNumUserTriggeredWifiDisableNotHighRSSI(&highRSSI);
            if (val > 1000) {
                // We remained associated for a compound amount of time while passing
                // traffic, hence loose the corresponding user triggered disabled stats
                if (lowRSSI > 0) {
                    currentConfiguration->SetNumUserTriggeredWifiDisableLowRSSI(--lowRSSI);
                }
                if (highRSSI > 0) {
                    currentConfiguration->SetNumUserTriggeredWifiDisableNotHighRSSI(--highRSSI);
                }
                currentConfiguration->SetNumTicksAtLowRSSI(0);
            }
            if (mWifiConfigStore->enableWifiCellularHandoverUserTriggeredAdjustment &&
                    (lowRSSI > 0
                    || highRSSI > 0)) {
                score = score -5;
                penalizedDueToUserTriggeredDisconnect = 2;
                sb.Append(" p2");
            }
        }
        else if (!isHighRSSI) {
            Int32 val;
            currentConfiguration->GetNumTicksAtNotHighRSSI(&val);
            currentConfiguration->SetNumTicksAtNotHighRSSI(++val);
            currentConfiguration->GetNumTicksAtNotHighRSSI(&val);
            if (val > 1000) {
                // We remained associated for a compound amount of time while passing
                // traffic, hence loose the corresponding user triggered disabled stats
                currentConfiguration->GetNumUserTriggeredWifiDisableNotHighRSSI(&val);
                if (val > 0) {
                    currentConfiguration->SetNumUserTriggeredWifiDisableNotHighRSSI(--val);
                }
                currentConfiguration->SetNumTicksAtNotHighRSSI(0);
            }
            if (mWifiConfigStore->enableWifiCellularHandoverUserTriggeredAdjustment && val > 0) {
                score = score -5;
                penalizedDueToUserTriggeredDisconnect = 3;
                sb.Append(" p3");
            }
        }
        tmp = String("");
        Int32 v1, v2, v3;
        currentConfiguration->GetNumTicksAtBadRSSI(&v1);
        currentConfiguration->GetNumTicksAtLowRSSI(&v2);
        currentConfiguration->GetNumTicksAtNotHighRSSI(&v3);
        sb.Append(tmp.AppendFormat(" ticks %d,%d,%d", v1, v2, v3));
    }

    if (PDBG) {
        String rssiStatus = String("");
        if (isBadRSSI) rssiStatus += " badRSSI ";
        else if (isHighRSSI) rssiStatus += " highRSSI ";
        else if (isLowRSSI) rssiStatus += " lowRSSI ";
        if (isBadLinkspeed) rssiStatus += " lowSpeed ";
        // Loge(String("calculateWifiScore freq=") + StringUtils::ToString(mWifiInfo.getFrequency())
        //                 + " speed=" + StringUtils::ToString(mWifiInfo.getLinkSpeed())
        //                 + " score=" + StringUtils::ToString(mWifiInfo.score)
        //                 + rssiStatus
        //                 + " -> txbadrate=" + String.format( "%.2f", mWifiInfo.txBadRate )
        //                 + " txgoodrate=" + String.format("%.2f", mWifiInfo.txSuccessRate)
        //                 + " txretriesrate=" + String.format("%.2f", mWifiInfo.txRetriesRate)
        //                 + " rxrate=" + String.format("%.2f", mWifiInfo.rxSuccessRate)
        //                 + " userTriggerdPenalty" + penalizedDueToUserTriggeredDisconnect);
    }

    Double txBadRate, txSuccessRate;
    Int32 linkStuckCount, badRssiCount, lowRssiCount, wifiScore;
    mWifiInfo->GetTxBadRate(&txBadRate);
    mWifiInfo->GetTxSuccessRate(&txSuccessRate);
    mWifiInfo->GetLinkStuckCount(&linkStuckCount);
    mWifiInfo->GetBadRssiCount(&badRssiCount);
    mWifiInfo->GetLowRssiCount(&lowRssiCount);
    mWifiInfo->GetScore(&wifiScore);
    if ((txBadRate >= 1) && (txSuccessRate < 3)
            && (isBadRSSI || isLowRSSI)) {
        // Link is stuck
        if (linkStuckCount < 5) {
            linkStuckCount = linkStuckCount + 1;
            mWifiInfo->SetLinkStuckCount(linkStuckCount);
        }
        String tmp;
        sb.Append(tmp.AppendFormat(" ls+=%d", linkStuckCount));
        if (PDBG) Loge(String(" bad link -> stuck count =")
                + StringUtils::ToString(linkStuckCount));
    }
    else if (txSuccessRate > 2 || txBadRate < 0.1) {
        if (linkStuckCount > 0) {
            linkStuckCount = linkStuckCount - 1;
            mWifiInfo->SetLinkStuckCount(linkStuckCount);
        }
        String tmp;
        sb.Append(tmp.AppendFormat(" ls-=%d", linkStuckCount));
        if (PDBG) Loge(String(" good link -> stuck count =")
                + StringUtils::ToString(linkStuckCount));
    }

    tmp = String("");
    sb.Append(tmp.AppendFormat(" [%d", score));

    if (linkStuckCount > 1) {
        // Once link gets stuck for more than 3 seconds, start reducing the score
        score = score - 2 * (linkStuckCount - 1);
    }
    tmp = String("");
    sb.Append(tmp.AppendFormat(",%d", score));

    if (isBadLinkspeed) {
        score -= 4 ;
        if (PDBG) {
            Loge(String(" isBadLinkspeed   ---> count=")
                    + StringUtils::ToString(mBadLinkspeedcount)
                    + " score=" + StringUtils::ToString(score));
        }
    }
    else if ((isGoodLinkspeed) && (txSuccessRate > 5)) {
        score += 4; // So as bad rssi alone dont kill us
    }
    tmp = "";
    sb.Append(tmp.AppendFormat(",%d", score));

    if (isBadRSSI) {
        if (badRssiCount < 7)
            badRssiCount += 1;
            mWifiInfo->SetBadRssiCount(badRssiCount);
    }
    else if (isLowRSSI) {
        mWifiInfo->SetLowRssiCount(1); // Dont increment the lowRssi count above 1
        if (badRssiCount > 0) {
            // Decrement bad Rssi count
            badRssiCount -= 1;
            mWifiInfo->SetBadRssiCount(badRssiCount);
        }
    }
    else {
        mWifiInfo->SetBadRssiCount(0);
        mWifiInfo->SetLowRssiCount(0);
    }

    score -= badRssiCount * 2 +  lowRssiCount ;
    tmp = "";
    sb.Append(tmp.AppendFormat(",%d", score));

    if (PDBG) Loge(String(" badRSSI count") + StringUtils::ToString(badRssiCount)
                 + " lowRSSI count" + StringUtils::ToString(lowRssiCount)
                    + " --> score " + StringUtils::ToString(score));


    if (isHighRSSI) {
        score += 5;
        if (PDBG) Loge(String(" isHighRSSI       ---> score=") + StringUtils::ToString(score));
    }
    tmp = "";
    tmp.AppendFormat(",%d]", score);
    tmp.AppendFormat(" brc=%d lrc=%d", badRssiCount, lowRssiCount);
    sb.Append(tmp);

    //sanitize boundaries
    if (score > INetworkAgent::WIFI_BASE_SCORE)
        score = INetworkAgent::WIFI_BASE_SCORE;
    if (score < 0)
        score = 0;

    //report score
    if (score != wifiScore) {
        if (DBG) {
            Loge(String("calculateWifiScore() report new score ") + StringUtils::ToString(score));
        }
        if (!isPropFeatureEnabled) {
            mWifiInfo->SetScore(score);
            if(mNetworkAgent != NULL) {
                mNetworkAgent->SendNetworkScore(score);
            }
        }
    }
    sb.ToString(&wifiScoringReport);
}

void WifiStateMachine::FetchPktcntNative(
    /* [in] */ IRssiPacketCountInfo* info)
{
    String pktcntPoll = mWifiNative->PktcntPoll();

    if (pktcntPoll != NULL) {
        AutoPtr<ArrayOf<String> > lines;
        StringUtils::Split(pktcntPoll, String("\n"), (ArrayOf<String>**)&lines);
        for (Int32 i = 0; i < lines->GetLength(); i++) {
            String line = (*lines)[i];

            AutoPtr<ArrayOf<String> > prop;
            StringUtils::Split(line, String("="), (ArrayOf<String>**)&prop);
            if (prop->GetLength() < 2) continue;
            // try {
            if ((*prop)[0].Equals("TXGOOD")) {
                info->SetTxgood(StringUtils::ParseInt32((*prop)[1]));
            }
            else if ((*prop)[0].Equals("TXBAD")) {
                info->SetTxbad(StringUtils::ParseInt32((*prop)[1]));
            }
            // } catch (NumberFormatException e) {
            //     // Ignore
            // }
        }
    }
}

Boolean WifiStateMachine::ClearIPv4Address(
    /* [in] */ const String& iface)
{
    // try {
    AutoPtr<IInterfaceConfiguration> ifcg;
    CInterfaceConfiguration::New((IInterfaceConfiguration**)&ifcg);
    AutoPtr<ILinkAddress> addr;
    CLinkAddress::New(String("0.0.0.0/0"), (ILinkAddress**)&addr);
    ifcg->SetLinkAddress(addr);
    mNwService->SetInterfaceConfig(iface, ifcg);
    return TRUE;
    // } catch (RemoteException e) {
    //     return FALSE;
    // }
}

Boolean WifiStateMachine::IsProvisioned(
    /* [in] */ ILinkProperties* lp)
{
    // LinkProperties#isProvisioned returns TRUE even if all we have is an IPv4 address and no
    // connectivity. This turns out not to be very useful, because we can't distinguish it from
    // a state where we have an IPv4 address assigned to the interface but are still running
    // DHCP.
    // TODO: Fix LinkProperties and remove this function.
    Boolean b;
    if (mWifiConfigStore->IsUsingStaticIp(mLastNetworkId, &b), b) {
        lp->HasIPv4Address(&b);
        return b;
    }
    else {
        Boolean b1, b2, b3, b4, b5, b6;
        lp->HasIPv4Address(&b1);
        lp->HasIPv4DefaultRoute(&b2);
        lp->HasIPv4DnsServer(&b3);
        lp->HasGlobalIPv6Address(&b4);
        lp->HasIPv6DefaultRoute(&b5);
        lp->HasIPv6DnsServer(&b6);
        return (b1 && b2 && b3) ||
               (b4 && b5 && b6);
    }
}

void WifiStateMachine::UpdateLinkProperties(
    /* [in] */ Int32 reason,
    /* [in] */ ILinkProperties* lp)
{
    AutoPtr<ILinkProperties> newLp;
    CLinkProperties::New((ILinkProperties**)&newLp);

    // Interface name and proxy are locally configured.
    newLp->SetInterfaceName(mInterfaceName);
    AutoPtr<IProxyInfo> pi;
    mWifiConfigStore->GetProxyProperties(mLastNetworkId, (IProxyInfo**)&pi);
    newLp->SetHttpProxy(pi);

    // IPv4/v6 addresses, IPv6 routes and IPv6 DNS servers come from netlink.
    AutoPtr<IList> addrs;
    lp->GetLinkAddresses((IList**)&addrs);
    newLp->SetLinkAddresses(ICollection::Probe(addrs));

    Boolean b;
    AutoPtr<IList> routes;
    lp->GetRoutes((IList**)&routes);
    Int32 size;
    routes->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        routes->Get(i, (IInterface**)&obj);
        AutoPtr<IRouteInfo> route = IRouteInfo::Probe(obj);
        newLp->AddRoute(route, &b);
    }

    AutoPtr<IList> servers;
    lp->GetDnsServers((IList**)&servers);
    servers->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        servers->Get(i, (IInterface**)&obj);
        AutoPtr<IInetAddress> dns = IInetAddress::Probe(obj);
        newLp->AddDnsServer(dns, &b);
    }

    // IPv4 routes, DNS servers and domains come from mDhcpResults.
    synchronized (mDhcpResultsLock) {
        // Even when we're using static configuration, we don't need to look at the config
        // store, because static IP configuration also populates mDhcpResults.
        if ((mDhcpResults != NULL) && (lp->HasIPv4Address(&b), b)) {
            AutoPtr<IList> list;
            IStaticIpConfiguration::Probe(mDhcpResults)->GetRoutes(mInterfaceName, (IList**)&list);
            list->GetSize(&size);
            for (Int32 i = 0; i < size; i++) {
                AutoPtr<IInterface> obj;
                list->Get(i, (IInterface**)&obj);
                AutoPtr<IRouteInfo> route = IRouteInfo::Probe(obj);
                newLp->AddRoute(route, &b);
            }

            AutoPtr<IArrayList> arrayList;
            IStaticIpConfiguration::Probe(mDhcpResults)->GetDnsServers((IArrayList**)&arrayList);
            arrayList->GetSize(&size);
            for (Int32 i = 0; i < size; i++) {
                AutoPtr<IInterface> obj;
                arrayList->Get(i, (IInterface**)&obj);
                AutoPtr<IInetAddress> dns = IInetAddress::Probe(obj);
                newLp->AddDnsServer(dns, &b);
            }

            String str;
            IStaticIpConfiguration::Probe(mDhcpResults)->GetDomains(&str);
            newLp->SetDomains(str);
        }
    }

    IObject::Probe(newLp)->Equals(mLinkProperties, &b);
    Boolean linkChanged = !b;
    Boolean wasProvisioned = IsProvisioned(mLinkProperties);
    Boolean isProvisioned = IsProvisioned(newLp);
    NetworkInfoDetailedState detailedState = GetNetworkDetailedState();

    if (linkChanged) {
        if (DBG) {
            // Log(String("Link configuration changed for netId: ") + mLastNetworkId
            //         + " old: " + mLinkProperties + " new: " + newLp);
        }
        mLinkProperties = newLp;
        if (mTcpBufferSizes.IsEmpty() == FALSE) {
            mLinkProperties->SetTcpBufferSizes(mTcpBufferSizes);
        }
        if (mNetworkAgent != NULL) {
            mNetworkAgent->SendLinkProperties(mLinkProperties);
        }
    }

    if (DBG) {
        StringBuilder sb;
        sb.Append(String("updateLinkProperties nid: ")
                + StringUtils::ToString(mLastNetworkId));
        sb.Append(String(" state: ") + StringUtils::ToString(detailedState));
        String str;
        SmToString(reason, &str);
        sb.Append(String(" reason: ") + str);

        if (mLinkProperties != NULL) {
            Boolean b;
            if (mLinkProperties->HasIPv4Address(&b), b) {
                sb.Append(" v4");
            }
            if (mLinkProperties->HasGlobalIPv6Address(&b), b) {
                sb.Append(" v6");
            }
            if (mLinkProperties->HasIPv4DefaultRoute(&b), b) {
                sb.Append(" v4r");
            }
            if (mLinkProperties->HasIPv6DefaultRoute(&b), b) {
                sb.Append(" v6r");
            }
            if (mLinkProperties->HasIPv4DnsServer(&b), b) {
                sb.Append(" v4dns");
            }
            if (mLinkProperties->HasIPv6DnsServer(&b), b) {
                sb.Append(" v6dns");
            }
            if (isProvisioned) {
                sb.Append(" isprov");
            }
        }
        sb.ToString(&str);
        Loge(str);
    }

    // If we just configured or lost IP configuration, do the needful.
    // We don't just call handleSuccessfulIpConfiguration() or handleIpConfigurationLost()
    // here because those should only be called if we're attempting to connect or already
    // connected, whereas updateLinkProperties can be called at any time.
    switch (reason) {
        case IDhcpStateMachine::DHCP_SUCCESS:
        case CMD_STATIC_IP_SUCCESS:
            // IPv4 provisioning succeded. Advance to connected state.
            SendMessage(CMD_IP_CONFIGURATION_SUCCESSFUL);
            if (!isProvisioned) {
                // Can never happen unless DHCP reports success but isProvisioned thinks the
                // resulting configuration is invalid (e.g., no IPv4 address, or the state in
                // mLinkProperties is out of sync with reality, or there's a bug in this code).
                // TODO: disconnect here instead. If our configuration is not usable, there's no
                // point in staying connected, and if mLinkProperties is out of sync with
                // reality, that will cause problems in the future.
                Loge(String("IPv4 config succeeded, but not provisioned"));
            }
            break;

        case IDhcpStateMachine::DHCP_FAILURE:
            // DHCP failed. If we're not already provisioned, give up and disconnect.
            // If we're already provisioned (e.g., IPv6-only network), stay connected.
            if (!isProvisioned) {
                SendMessage(CMD_IP_CONFIGURATION_LOST);
            }
            else {
                // DHCP failed, but we're provisioned (e.g., if we're on an IPv6-only network).
                SendMessage(CMD_IP_CONFIGURATION_SUCCESSFUL);

                // To be sure we don't get stuck with a non-working network if all we had is
                // IPv4, remove the IPv4 address from the interface (since we're using DHCP,
                // and DHCP failed). If we had an IPv4 address before, the deletion of the
                // address  will cause a CMD_UPDATE_LINKPROPERTIES. If the IPv4 address was
                // necessary for provisioning, its deletion will cause us to disconnect.
                //
                // This shouldn't be needed, because on an IPv4-only network a DHCP failure will
                // have empty DhcpResults and thus empty LinkProperties, and isProvisioned will
                // not return TRUE if we're using DHCP and don't have an IPv4 default route. So
                // for now it's only here for extra redundancy. However, it will increase
                // robustness if we move to getting IPv4 routes from netlink as well.
                Loge(String("DHCP failure: provisioned, clearing IPv4 address."));
                if (!ClearIPv4Address(mInterfaceName)) {
                    SendMessage(CMD_IP_CONFIGURATION_LOST);
                }
            }
            break;

        case CMD_STATIC_IP_FAILURE:
            // Static configuration was invalid, or an error occurred in applying it. Give up.
            SendMessage(CMD_IP_CONFIGURATION_LOST);
            break;

        case CMD_UPDATE_LINKPROPERTIES:
            // IP addresses, DNS servers, etc. changed. Act accordingly.
            Boolean isStatic;
            mWifiConfigStore->IsUsingStaticIp(mLastNetworkId, &isStatic);
            if (wasProvisioned && !isProvisioned && !isStatic) {
                // We no longer have a usable network configuration. Disconnect.
                SendMessage(CMD_IP_CONFIGURATION_LOST);
            }
            else if (!wasProvisioned && isProvisioned) {
                // We have a usable IPv6-only config. Advance to connected state.
                SendMessage(CMD_IP_CONFIGURATION_SUCCESSFUL);
            }
            if (linkChanged && GetNetworkDetailedState() ==
                    Elastos::Droid::Net::NetworkInfoDetailedState_CONNECTED) {
                // If anything has changed and we're already connected, send out a notification.
                SendLinkConfigurationChangedBroadcast();
            }
            break;
    }
}

void WifiStateMachine::ClearLinkProperties()
{
     // Clear the link properties obtained from DHCP and netlink.
     synchronized (mDhcpResultsLock) {
         if (mDhcpResults != NULL) {
             mDhcpResults->Clear();
         }
     }
     mNetlinkTracker->ClearLinkProperties();

     // Now clear the merged link properties.
     mLinkProperties->Clear();
     if (mNetworkAgent != NULL) mNetworkAgent->SendLinkProperties(mLinkProperties);
}

String WifiStateMachine::UpdateDefaultRouteMacAddress(
    /* [in] */ Int32 timeout)
{
    String address;
    AutoPtr<IList> list;
    mLinkProperties->GetRoutes((IList**)&list);
    Int32 size;
    list->GetSize(&size);

    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        list->Get(i, (IInterface**)&obj);
        AutoPtr<IRouteInfo> route = IRouteInfo::Probe(obj);

        Boolean b1, b2;
        route->IsDefaultRoute(&b1);
        route->HasGateway(&b2);
        if (b1 && b2) {
            AutoPtr<IInetAddress> gateway;
            route->GetGateway((IInetAddress**)&gateway);
            String addr;
            gateway->GetHostAddress(&addr);
            if (IInet4Address::Probe(gateway) != NULL) {
                if (PDBG) {
                    Loge(String("updateDefaultRouteMacAddress found Ipv4 default :")
                            + addr);
                }
                address = MacAddressFromRoute(addr);
                /* The gateway's MAC address is known */
                if ((address == NULL) && (timeout > 0)) {
                    Boolean reachable = FALSE;
                    // try {
                    gateway->IsReachable(timeout, &reachable);
                    // } catch (Exception e) {
                    //     Loge("updateDefaultRouteMacAddress exception reaching :"
                    //             + addr);
                    // } finally {
                    if (reachable == TRUE) {
                        address = MacAddressFromRoute(addr);
                        if (PDBG) {
                            Loge(String("updateDefaultRouteMacAddress reachable (tried again) :")
                                    + addr + " found " + address);
                        }
                    }
                    // }
                }
                if (address != NULL) {
                    mWifiConfigStore->SetDefaultGwMacAddress(mLastNetworkId, address);
                }
            }
        }
    }
    return address;
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
    // try {
    mBatteryStats->NoteWifiRssiChanged(newRssi);
    // } catch (RemoteException e) {
    //     // Won't happen.
    // }
    AutoPtr<IIntent> intent;
    CIntent::New(IWifiManager::RSSI_CHANGED_ACTION, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
    intent->PutExtra(IWifiManager::EXTRA_NEW_RSSI, newRssi);
    mContext->SendStickyBroadcastAsUser(intent, UserHandle::ALL);
}

void WifiStateMachine::SendNetworkStateChangeBroadcast(
    /* [in] */ const String& bssid)
{
    AutoPtr<IIntent> intent;
    CIntent::New(IWifiManager::NETWORK_STATE_CHANGED_ACTION, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
    AutoPtr<INetworkInfo> ni;
    CNetworkInfo::New(mNetworkInfo, (INetworkInfo**)&ni);
    intent->PutExtra(IWifiManager::EXTRA_NETWORK_INFO, IParcelable::Probe(ni));
    AutoPtr<ILinkProperties> lp;
    CLinkProperties::New(mLinkProperties, (ILinkProperties**)&lp);
    intent->PutExtra(IWifiManager::EXTRA_LINK_PROPERTIES, IParcelable::Probe(lp));
    if (bssid != NULL)
        intent->PutExtra(IWifiManager::EXTRA_BSSID, bssid);
    NetworkInfoDetailedState state;
    mNetworkInfo->GetDetailedState(&state);
    if (state == Elastos::Droid::Net::NetworkInfoDetailedState_VERIFYING_POOR_LINK ||
            state == Elastos::Droid::Net::NetworkInfoDetailedState_CONNECTED) {
        AutoPtr<IWifiInfo> wi;
        CWifiInfo::New(mWifiInfo, (IWifiInfo**)&wi);
        intent->PutExtra(IWifiManager::EXTRA_WIFI_INFO, IParcelable::Probe(wi));
    }
    mContext->SendStickyBroadcastAsUser(intent, UserHandle::ALL);
}

void WifiStateMachine::SendLinkConfigurationChangedBroadcast()
{
    AutoPtr<IIntent> intent;
    CIntent::New(IWifiManager::LINK_CONFIGURATION_CHANGED_ACTION, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
    AutoPtr<ILinkProperties> lp;
    CLinkProperties::New(mLinkProperties, (ILinkProperties**)&lp);
    intent->PutExtra(IWifiManager::EXTRA_LINK_PROPERTIES, IParcelable::Probe(lp));
    mContext->SendBroadcastAsUser(intent, UserHandle::ALL);
}

void WifiStateMachine::SendSupplicantConnectionChangedBroadcast(
    /* [in] */ Boolean connected)
{
    AutoPtr<IIntent> intent;
    CIntent::New(IWifiManager::SUPPLICANT_CONNECTION_CHANGE_ACTION, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
    intent->PutExtra(IWifiManager::EXTRA_SUPPLICANT_CONNECTED, connected);
    mContext->SendBroadcastAsUser(intent, UserHandle::ALL);
}

Boolean WifiStateMachine::SetNetworkDetailedState(
    /* [in] */ NetworkInfoDetailedState state)
{
    Boolean hidden = FALSE;

    Boolean b;
    if (linkDebouncing || (IsRoaming(&b), b)) {
        // There is generally a confusion in the system about colluding
        // WiFi Layer 2 state (as reported by supplicant) and the Network state
        // which leads to multiple confusion.
        //
        // If link is de-bouncing or roaming, we already have an IP address
        // as well we were connected and are doing L2 cycles of
        // reconnecting or renewing IP address to check that we still have it
        // This L2 link flapping should ne be reflected into the Network state
        // which is the state of the WiFi Network visible to Layer 3 and applications
        // Note that once debouncing and roaming are completed, we will
        // set the Network state to where it should be, or leave it as unchanged
        //
        hidden = TRUE;
    }
    NetworkInfoDetailedState nidState;
    mNetworkInfo->GetDetailedState(&nidState);
    if (DBG) {
        Log(String("setDetailed state, old =")
                + StringUtils::ToString(nidState) +
                " and new state=" + StringUtils::ToString(state) +
                " hidden=" + StringUtils::BooleanToString(hidden));
    }
    String eInfo;
    mNetworkInfo->GetExtraInfo(&eInfo);
    String ssid;
    mWifiInfo->GetSSID(&ssid);
    if (!eInfo.IsNull() && ssid != NULL) {
        // Always indicate that SSID has changed
        if (!eInfo.Equals(ssid)) {
            if (DBG) {
                Log(String("setDetailed state send new extra info")  + ssid);
            }
            mNetworkInfo->SetExtraInfo(ssid);
            SendNetworkStateChangeBroadcast(String(NULL));
        }
    }
    if (hidden == TRUE) {
        return FALSE;
    }

    if (state != nidState) {
        mNetworkInfo->SetDetailedState(state, String(NULL), ssid);
        if (mNetworkAgent != NULL) {
            mNetworkAgent->SendNetworkInfo(mNetworkInfo);
        }
        SendNetworkStateChangeBroadcast(String(NULL));
        return TRUE;
    }
    return FALSE;
}

NetworkInfoDetailedState WifiStateMachine::GetNetworkDetailedState()
{
    NetworkInfoDetailedState nidState;
    mNetworkInfo->GetDetailedState(&nidState);
    return nidState;
}

SupplicantState WifiStateMachine::HandleSupplicantStateChange(
    /* [in] */ IMessage* message)
{
    AutoPtr<IInterface> obj;
    message->GetObj((IInterface**)&obj);
    AutoPtr<CStateChangeResult> stateChangeResult = (CStateChangeResult*)(IObject*)obj.Get();
    SupplicantState state;
    stateChangeResult->GetSupplicantState(&state);
    // Supplicant state change
    // [31-13] Reserved for future use
    // [8 - 0] Supplicant state (as defined in SupplicantState.java)
    // 50023 supplicant_state_changed (custom|1|5)
    AutoPtr<ISupplicantState> s;
    CSupplicantState::New(state, (ISupplicantState**)&s);
    mWifiInfo->SetSupplicantState(s);
    // Network id is only valid when we start connecting
    AutoPtr<ISupplicantStateHelper> helper;
    CSupplicantStateHelper::AcquireSingleton((ISupplicantStateHelper**)&helper);
    Boolean b;
    if (helper->IsConnecting(state, &b), b) {
        Int32 id;
        stateChangeResult->GetNetworkId(&id);
        mWifiInfo->SetNetworkId(id);
    }
    else {
        mWifiInfo->SetNetworkId(IWifiConfiguration::INVALID_NETWORK_ID);
    }

    String bssid;
    stateChangeResult->GetBSSID(&bssid);
    mWifiInfo->SetBSSID(bssid);
    AutoPtr<IWifiSsid> wifissid;
    stateChangeResult->GetWifiSsid((IWifiSsid**)&wifissid);
    mWifiInfo->SetSSID(wifissid);

    AutoPtr<IMessageHelper> mHelper;
    CMessageHelper::AcquireSingleton((IMessageHelper**)&mHelper);
    AutoPtr<IMessage> msg;
    mHelper->Obtain(message, (IMessage**)&msg);
    mSupplicantStateTracker->SendMessage(msg);

    return state;
}

void WifiStateMachine::HandleNetworkDisconnect()
{
    // if (DBG) Log("handleNetworkDisconnect: Stopping DHCP and clearing IP"
    //         + " stack:" + Thread.currentThread().getStackTrace()[2].getMethodName()
    //         +" - "+ Thread.currentThread().getStackTrace()[3].getMethodName()
    //         +" - "+ Thread.currentThread().getStackTrace()[4].getMethodName()
    //         +" - "+ Thread.currentThread().getStackTrace()[5].getMethodName());

    ClearCurrentConfigBSSID(String("handleNetworkDisconnect"));

    StopDhcp();

    // try {
    mNwService->ClearInterfaceAddresses(mInterfaceName);
    mNwService->DisableIpv6(mInterfaceName);
    // } catch (Exception e) {
    //     Loge("Failed to clear addresses or disable ipv6" + e);
    // }

    /* Reset data structures */
    mBadLinkspeedcount = 0;
    mWifiInfo->Reset();
    linkDebouncing = FALSE;
    /* Reset roaming parameters */
    mAutoRoaming = WifiAutoJoinController::AUTO_JOIN_IDLE;
    fullBandConnectedTimeIntervalMilli = 20 * 1000; // Start scans at 20 seconds interval

    SetNetworkDetailedState(Elastos::Droid::Net::NetworkInfoDetailedState_DISCONNECTED);
    if (mNetworkAgent != NULL) {
        mNetworkAgent->SendNetworkInfo(mNetworkInfo);
        mNetworkAgent = NULL;
    }
    mWifiConfigStore->UpdateStatus(mLastNetworkId,
            Elastos::Droid::Net::NetworkInfoDetailedState_DISCONNECTED);

    /* Clear network properties */
    ClearLinkProperties();

    /* Cend event to CM & network change broadcast */
    SendNetworkStateChangeBroadcast(mLastBssid);

    /* Cancel auto roam requests */
    AutoRoamSetBSSID(mLastNetworkId, String("any"));

    mLastBssid= NULL;
    RegisterDisconnected();
    mLastNetworkId = IWifiConfiguration::INVALID_NETWORK_ID;
}

void WifiStateMachine::HandleSupplicantConnectionLoss(
    /* [in] */ Boolean killSupplicant)
{
    /* Socket connection can be lost when we do a graceful shutdown
    * or when the driver is hung. Ensure supplicant is stopped here.
    */
    if (killSupplicant) {
        mWifiMonitor->KillSupplicant(mP2pSupported);
    }
    mWifiNative->CloseSupplicantConnection();
    SendSupplicantConnectionChangedBroadcast(FALSE);
    SetWifiState(IWifiManager::WIFI_STATE_DISABLED);
}

void WifiStateMachine::HandleIPv4Success(
    /* [in] */ IDhcpResults* dhcpResults,
    /* [in] */ Int32 reason)
{
    if (PDBG) {
        // Loge(("wifistatemachine handleIPv4Success <") + dhcpResults.toString() + ">");
        // Loge("link address " + dhcpResults.ipAddress);
    }

    synchronized (mDhcpResultsLock) {
        mDhcpResults = dhcpResults;
    }

    AutoPtr<ILinkAddress> la;
    IStaticIpConfiguration::Probe(dhcpResults)->GetIpAddress((ILinkAddress**)&la);
    AutoPtr<IInetAddress> ia;
    la->GetAddress((IInetAddress**)&ia);
    AutoPtr<IInet4Address> addr = IInet4Address::Probe(ia);
    Boolean b;
    if (IsRoaming(&b), b) {
        if (IInet4Address::Probe(addr) != NULL) {
            Int32 previousAddress;
            mWifiInfo->GetIpAddress(&previousAddress);
            AutoPtr<INetworkUtils> networkUtils;
            CNetworkUtils::AcquireSingleton((INetworkUtils**)&networkUtils);
            Int32 newAddress;
            networkUtils->InetAddressToInt(addr, &newAddress);
            if (previousAddress != newAddress) {
                Loge(String("handleIPv4Success, roaming and address changed"));
                     // + mWifiInfo + " got: " + str);
            }
            else {

            }
        }
        else {
            String str;
            IObject::Probe(addr)->ToString(&str);
            Loge(String("handleIPv4Success, roaming and didnt get an IPv4 address") + str);
        }
    }
    mWifiInfo->SetInetAddress(IInetAddress::Probe(addr));
    dhcpResults->HasMeteredHint(&b);
    mWifiInfo->SetMeteredHint(b);
    AutoPtr<ILinkProperties> lp;
    mNetlinkTracker->GetLinkProperties((ILinkProperties**)&lp);
    UpdateLinkProperties(reason, lp);
}

void WifiStateMachine::HandleSuccessfulIpConfiguration()
{
    mLastSignalLevel = -1; // Force update of signal strength
    AutoPtr<IWifiConfiguration> c;
    GetCurrentWifiConfiguration((IWifiConfiguration**)&c);

    // Reset IP failure tracking
    if (c != NULL) {
        c->SetNumConnectionFailures(0);
    }
    if (c != NULL) {
        AutoPtr<IScanResult> result;
        GetCurrentScanResult((IScanResult**)&result);
        if (result == NULL) {
            String str;
            c->ConfigKey(&str);
            Loge(String("WifiStateMachine: handleSuccessfulIpConfiguration and no scan results") + str);
        }
        else {
            // Clear the per BSSID failure count
            result->SetNumIpConfigFailures(0);
            // Clear the WHOLE BSSID blacklist, which means supplicant is free to retry
            // any BSSID, even though it may already have a non zero ip failure count,
            // this will typically happen if the user walks away and come back to his arrea
            // TODO: implement blacklisting based on a timer, i.e. keep BSSID blacklisted
            // in supplicant for a couple of hours or a day
            mWifiNative->ClearBlacklist();
        }
    }
}

void WifiStateMachine::HandleIPv4Failure(
    /* [in] */ Int32 reason)
{
    synchronized(mDhcpResultsLock) {
         if (mDhcpResults != NULL) {
             mDhcpResults->Clear();
         }
    }
    if (PDBG) {
        Loge(String("wifistatemachine handleIPv4Failure"));
    }
    AutoPtr<ILinkProperties> lp;
    mNetlinkTracker->GetLinkProperties((ILinkProperties**)&lp);
    UpdateLinkProperties(reason, lp);
}

void WifiStateMachine::HandleIpConfigurationLost()
{
    mWifiInfo->SetInetAddress(NULL);
    mWifiInfo->SetMeteredHint(FALSE);

    String bssid;
    mWifiInfo->GetBSSID(&bssid);
    mWifiConfigStore->HandleSSIDStateChange(mLastNetworkId, FALSE,
            String("DHCP FAILURE"), bssid);

    /* DHCP times out after about 30 seconds, we do a
     * disconnect thru supplicant, we will let autojoin retry connecting to the network
     */
    mWifiNative->Disconnect();
}

void WifiStateMachine::StartSoftApWithConfig(
    /* [in] */ IWifiConfiguration* config)
{
    // Start hostapd on a separate thread
    AutoPtr<StartSoftApWithConfigRunnable> r = new StartSoftApWithConfigRunnable(this, config);

    AutoPtr<IThread> thread;
    CThread::New(r, (IThread**)&thread);
    thread->Start();
}

String WifiStateMachine::MacAddressFromRoute(
    /* [in] */ const String& ipAddress)
{
    String macAddress;
    AutoPtr<IBufferedReader> reader;
    // try {
    AutoPtr<IFileReader> fr;
    CFileReader::New(String("/proc/net/arp"), (IFileReader**)&fr);
    CBufferedReader::New(IReader::Probe(fr), (IBufferedReader**)&reader);

    // Skip over the line bearing colum titles
    String line;
    reader->ReadLine(&line);

    while (!(reader->ReadLine(&line), line).IsNull()) {
        AutoPtr<ArrayOf<String> > tokens;
        StringUtils::Split(line, String("[ ]+"), (ArrayOf<String>**)&tokens);

        if (tokens->GetLength() < 6) {
            continue;
        }

        // ARP column format is
        // Address HWType HWAddress Flags Mask IFace
        String ip = (*tokens)[0];
        String mac = (*tokens)[3];

        if (ipAddress.Equals(ip)) {
            macAddress = mac;
            break;
        }
    }

    if (macAddress == NULL) {
        Loge(String("Did not find remoteAddress {") + ipAddress + "} in " +
                "/proc/net/arp");
    }

    // } catch (FileNotFoundException e) {
    //     Loge("Could not open /proc/net/arp to lookup mac address");
    // } catch (IOException e) {
    //     Loge("Could not read /proc/net/arp to lookup mac address");
    // } finally {
    //     try {
    if (reader != NULL) {
        ICloseable::Probe(reader)->Close();
    }
    //     } catch (IOException e) {
    //         // Do nothing
    //     }
    // }
    return macAddress;
}

void WifiStateMachine::SendConnectedState()
{
    // Send out a broadcast with the CAPTIVE_PORTAL_CHECK to preserve
    // existing behaviour. The captive portal check really happens after we
    // transition into Elastos::Droid::Net::NetworkInfoDetailedState_CONNECTED.
    SetNetworkDetailedState(Elastos::Droid::Net::NetworkInfoDetailedState_CAPTIVE_PORTAL_CHECK);
    mWifiConfigStore->UpdateStatus(mLastNetworkId,
    Elastos::Droid::Net::NetworkInfoDetailedState_CAPTIVE_PORTAL_CHECK);
    SendNetworkStateChangeBroadcast(mLastBssid);

    String str;
    mWifiConfigStore->GetLastSelectedConfiguration(&str);
    if (!str.IsNull()) {
        if (mNetworkAgent != NULL) {
            mNetworkAgent->ExplicitlySelected();
        }
    }

    SetNetworkDetailedState(Elastos::Droid::Net::NetworkInfoDetailedState_CONNECTED);
    mWifiConfigStore->UpdateStatus(mLastNetworkId,
            Elastos::Droid::Net::NetworkInfoDetailedState_CONNECTED);
    SendNetworkStateChangeBroadcast(mLastBssid);
}

void WifiStateMachine::ReplyToMessage(
    /* [in] */ IMessage* msg,
    /* [in] */ Int32 what)
{
    AutoPtr<IMessenger> replyTo;
    msg->GetReplyTo((IMessenger**)&replyTo);
    if (replyTo == NULL) return;
    AutoPtr<IMessage> dstMsg = ObtainMessageWithArg2(msg);
    Int32 dstWhat;
    dstMsg->GetWhat(&dstWhat);
    dstWhat = what;
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
    Int32 dstWhat;
    dstMsg->GetWhat(&dstWhat);
    dstWhat = what;
    Int32 dstarg1;
    dstMsg->GetArg1(&dstarg1);
    dstarg1 = arg1;
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
    Int32 dstWhat;
    dstMsg->GetWhat(&dstWhat);
    dstWhat = what;
    AutoPtr<IInterface> dstobj;
    dstMsg->GetObj((IInterface**)&dstobj);
    dstobj = obj;
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

ECode WifiStateMachine::ParseHex(
    /* [in] */ Byte ch,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    if ('0' <= ch && ch <= '9') {
        *result = ch - '0';
        return NOERROR;
    }
    else if ('a' <= ch && ch <= 'f') {
        *result = ch - 'a' + 10;
        return NOERROR;
    }
    else if ('A' <= ch && ch <= 'F') {
        *result = ch - 'A' + 10;
        return NOERROR;
    }
    else {
        // throw new NumberFormatException("" + ch + " is not a valid hex digit");
        return E_NUMBER_FORMAT_EXCEPTION;
    }
}

ECode WifiStateMachine::ParseHex(
    /* [in] */ const String& hex,
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    /* This only works for good input; don't throw bad data at it */
    if (hex == NULL) {
        *result = ArrayOf<Byte>::Alloc(0);
        return NOERROR;
    }

    if (hex.GetLength() % 2 != 0) {
        // throw new NumberFormatException(hex + " is not a valid hex string");
        return E_NUMBER_FORMAT_EXCEPTION;
    }

    AutoPtr<ArrayOf<Byte> > array = ArrayOf<Byte>::Alloc((hex.GetLength())/2 + 1);
    (*array)[0] = (byte) ((hex.GetLength())/2);
    for (Int32 i = 0, j = 1; i < hex.GetLength(); i += 2, j++) {
        Int32 val1, val2;
        ParseHex(hex.GetChar(i), &val1);
        ParseHex(hex.GetChar(i + 1), &val2);
        Int32 val = val1 * 16 + val2;
        Byte b = (Byte) (val & 0xFF);
        (*array)[j] = b;
    }

    *result = array;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

String WifiStateMachine::MakeHex(
    /* [in] */ ArrayOf<Byte>* bytes)
{
    StringBuilder sb;
    for (Int32 i = 0; i < bytes->GetLength(); i++) {
        Byte b = (*bytes)[i];
        String tmp;
        sb.Append(tmp.AppendFormat("%02x", b));
    }

    String str;
    sb.ToString(&str);
    return str;
}

String WifiStateMachine::MakeHex(
    /* [in] */ ArrayOf<Byte>* bytes,
    /* [in] */ Int32 from,
    /* [in] */ Int32 len)
{
    StringBuilder sb;
    for (Int32 i = 0; i < len; i++) {
        String tmp;
        sb.Append(tmp.AppendFormat("%02x", (*bytes)[from + i]));
    }

    String str;
    sb.ToString(&str);
    return str;
}

AutoPtr<ArrayOf<Byte> > WifiStateMachine::Concat(
    /* [in] */ ArrayOf<Byte>* array1,
    /* [in] */ ArrayOf<Byte>* array2,
    /* [in] */ ArrayOf<Byte>* array3)
{
    Int32 len = array1->GetLength() + array2->GetLength() + array3->GetLength();

    if (array1->GetLength() != 0) {
        len++;                      /* add another byte for size */
    }

    if (array2->GetLength() != 0) {
        len++;                      /* add another byte for size */
    }

    if (array3->GetLength() != 0) {
        len++;                      /* add another byte for size */
    }

    AutoPtr<ArrayOf<Byte> > result = ArrayOf<Byte>::Alloc(len);

    Int32 index = 0;
    if (array1->GetLength() != 0) {
        (*result)[index] = (Byte) (array1->GetLength() & 0xFF);
        index++;
        for (Int32 i = 0; i < array1->GetLength(); i++) {
            Byte b = (*array1)[i];
            (*result)[index] = b;
            index++;
        }
    }

    if (array2->GetLength() != 0) {
        (*result)[index] = (Byte) (array2->GetLength() & 0xFF);
        index++;
        for (Int32 i = 0; i < array2->GetLength(); i++) {
            Byte b = (*array2)[i];
            (*result)[index] = b;
            index++;
        }
    }

    if (array3->GetLength() != 0) {
        (*result)[index] = (Byte) (array3->GetLength() & 0xFF);
        index++;
        for (Int32 i = 0; i < array3->GetLength(); i++) {
            Byte b = (*array3)[i];
            (*result)[index] = b;
            index++;
        }
    }
    return result;
}

void WifiStateMachine::SetScanIntevelOnMiracastModeChange(
    /* [in] */ Int32 mode)
{
    if ((mode == IWifiP2pManager::MIRACAST_SOURCE)
            || (mode == IWifiP2pManager::MIRACAST_SINK)) {
        AutoPtr<IResources> res;
        mContext->GetResources((IResources**)&res);
        Int32 defaultWfdIntervel;
        res->GetInteger(
                R::integer::config_wifi_scan_interval_wfd_connected, &defaultWfdIntervel);
        AutoPtr<IContentResolver> cr;
        mContext->GetContentResolver((IContentResolver**)&cr);
        Int64 wfdScanIntervalMs;
        Settings::Global::GetInt64(cr, ISettingsGlobal::WIFI_SUPPLICANT_SCAN_INTERVAL_WFD_CONNECTED_MS,
                defaultWfdIntervel, &wfdScanIntervalMs);
        mWifiNative->SetScanInterval((Int32) wfdScanIntervalMs / 1000);
    }
}

void WifiStateMachine::HandlePnoFailError()
{
    /* PNO should not fail when P2P is not connected and there are
       saved profiles */
    Boolean b;
    mP2pConnected->Get(&b);
    AutoPtr<IList> list;
    mWifiConfigStore->GetConfiguredNetworks((IList**)&list);
    Int32 size;
    list->GetSize(&size);
    if (!b && (size == 0)) {
        return;
    }
    /* Trigger a periodic scan for every 300Sec if PNO fails */
    if (mEnableBackgroundScan) {
        mBackgroundScanConfigured = FALSE;
        AutoPtr<IMessage> msg;
        ObtainMessage(CMD_PNO_PERIODIC_SCAN,
                           ++mPnoPeriodicScanToken, 0, (IMessage**)&msg);
        SendMessageDelayed(msg, mDefaultFrameworkScanIntervalMs);
    }
}

} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namespace Elastos
