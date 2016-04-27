#include "elastos/droid/server/wifi/WifiStateMachine.h"
#include "Elastos.CoreLibrary.Utility.Concurrent.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Core.Server.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wifi {

//const Boolean WifiStateMachine::DBG = FALSE;
//const String WifiStateMachine::TAG("WifiStateMachine");
//const String WifiStateMachine::NETWORKTYPE("WIFI");
//
//const Int32 WifiStateMachine::SCAN_RESULT_CACHE_SIZE;
//const Int32 WifiStateMachine::POLL_RSSI_INTERVAL_MSECS;
//const Int32 WifiStateMachine::SUPPLICANT_RESTART_INTERVAL_MSECS;
//const Int32 WifiStateMachine::SUPPLICANT_RESTART_TRIES;
//const Int32 WifiStateMachine::TETHER_NOTIFICATION_TIME_OUT_MSECS;
//const Int32 WifiStateMachine::DRIVER_START_TIME_OUT_MSECS;
//const Int32 WifiStateMachine::EVENTLOG_WIFI_STATE_CHANGED;
//const Int32 WifiStateMachine::EVENTLOG_WIFI_EVENT_HANDLED;
//const Int32 WifiStateMachine::EVENTLOG_SUPPLICANT_STATE_CHANGED;
//
//const Int32 WifiStateMachine::BASE;
//const Int32 WifiStateMachine::CMD_LOAD_DRIVER;
//const Int32 WifiStateMachine::CMD_UNLOAD_DRIVER;
//const Int32 WifiStateMachine::CMD_LOAD_DRIVER_SUCCESS;
//const Int32 WifiStateMachine::CMD_LOAD_DRIVER_FAILURE;
//const Int32 WifiStateMachine::CMD_UNLOAD_DRIVER_SUCCESS;
//const Int32 WifiStateMachine::CMD_UNLOAD_DRIVER_FAILURE;
//const Int32 WifiStateMachine::CMD_START_SUPPLICANT;
//const Int32 WifiStateMachine::CMD_STOP_SUPPLICANT;
//const Int32 WifiStateMachine::CMD_START_DRIVER;
//const Int32 WifiStateMachine::CMD_STOP_DRIVER;
//const Int32 WifiStateMachine::CMD_STATIC_IP_SUCCESS;
//const Int32 WifiStateMachine::CMD_STATIC_IP_FAILURE;
//const Int32 WifiStateMachine::CMD_STOP_SUPPLICANT_FAILED;
//const Int32 WifiStateMachine::CMD_DELAYED_STOP_DRIVER;
//const Int32 WifiStateMachine::CMD_DRIVER_START_TIMED_OUT;
//const Int32 WifiStateMachine::CMD_CAPTIVE_CHECK_COMPLETE;
//const Int32 WifiStateMachine::CMD_START_AP;
//const Int32 WifiStateMachine::CMD_START_AP_SUCCESS;
//const Int32 WifiStateMachine::CMD_START_AP_FAILURE;
//const Int32 WifiStateMachine::CMD_STOP_AP;
//const Int32 WifiStateMachine::CMD_SET_AP_CONFIG;
//const Int32 WifiStateMachine::CMD_SET_AP_CONFIG_COMPLETED;
//const Int32 WifiStateMachine::CMD_REQUEST_AP_CONFIG;
//const Int32 WifiStateMachine::CMD_RESPONSE_AP_CONFIG;
//const Int32 WifiStateMachine::CMD_TETHER_STATE_CHANGE;
//const Int32 WifiStateMachine::CMD_TETHER_NOTIFICATION_TIMED_OUT;
//const Int32 WifiStateMachine::CMD_BLUETOOTH_ADAPTER_STATE_CHANGE;
//const Int32 WifiStateMachine::CMD_PING_SUPPLICANT;
//const Int32 WifiStateMachine::CMD_ADD_OR_UPDATE_NETWORK;
//const Int32 WifiStateMachine::CMD_REMOVE_NETWORK;
//const Int32 WifiStateMachine::CMD_ENABLE_NETWORK;
//const Int32 WifiStateMachine::CMD_ENABLE_ALL_NETWORKS;
//const Int32 WifiStateMachine::CMD_BLACKLIST_NETWORK;
//const Int32 WifiStateMachine::CMD_CLEAR_BLACKLIST;
//const Int32 WifiStateMachine::CMD_SAVE_CONFIG;
//const Int32 WifiStateMachine::CMD_GET_CONFIGURED_NETWORKS;
//const Int32 WifiStateMachine::CMD_START_SCAN;
//const Int32 WifiStateMachine::CMD_SET_SCAN_MODE;
//const Int32 WifiStateMachine::CMD_SET_SCAN_TYPE;
//const Int32 WifiStateMachine::CMD_DISCONNECT;
//const Int32 WifiStateMachine::CMD_RECONNECT;
//const Int32 WifiStateMachine::CMD_REASSOCIATE;
//const Int32 WifiStateMachine::CMD_SET_HIGH_PERF_MODE;
//const Int32 WifiStateMachine::CMD_SET_COUNTRY_CODE;
//const Int32 WifiStateMachine::CMD_ENABLE_RSSI_POLL;
//const Int32 WifiStateMachine::CMD_RSSI_POLL;
//const Int32 WifiStateMachine::CMD_START_PACKET_FILTERING;
//const Int32 WifiStateMachine::CMD_STOP_PACKET_FILTERING;
//const Int32 WifiStateMachine::CMD_SET_SUSPEND_OPT_ENABLED;
//const Int32 WifiStateMachine::CMD_NO_NETWORKS_PERIODIC_SCAN;
//const Int32 WifiStateMachine::MULTICAST_V6;
//const Int32 WifiStateMachine::MULTICAST_V4;
//const Int32 WifiStateMachine::CMD_SET_FREQUENCY_BAND;
//const Int32 WifiStateMachine::CMD_ENABLE_BACKGROUND_SCAN;
//const Int32 WifiStateMachine::CMD_RESET_SUPPLICANT_STATE;
//const Int32 WifiStateMachine::CMD_ENABLE_P2P;
//const Int32 WifiStateMachine::CMD_DISABLE_P2P_REQ;
//const Int32 WifiStateMachine::CMD_DISABLE_P2P_RSP;
//const Int32 WifiStateMachine::CONNECT_MODE;
//const Int32 WifiStateMachine::SCAN_ONLY_MODE;
//const Int32 WifiStateMachine::SCAN_ONLY_WITH_WIFI_OFF_MODE;
//const Int32 WifiStateMachine::SCAN_ACTIVE;
//const Int32 WifiStateMachine::SCAN_PASSIVE;
//const Int32 WifiStateMachine::SUCCESS;
//const Int32 WifiStateMachine::FAILURE;
//const Int32 WifiStateMachine::IN_ECM_STATE;
//const Int32 WifiStateMachine::NOT_IN_ECM_STATE;
//const Int32 WifiStateMachine::DEFAULT_MAX_DHCP_RETRIES;
//const Int32 WifiStateMachine::SUSPEND_DUE_TO_DHCP;
//const Int32 WifiStateMachine::SUSPEND_DUE_TO_HIGH_PERF;
//const Int32 WifiStateMachine::SUSPEND_DUE_TO_SCREEN;
//const Int32 WifiStateMachine::MIN_INTERVAL_ENABLE_ALL_NETWORKS_MS;
//const Int32 WifiStateMachine::MIN_RSSI;
//const Int32 WifiStateMachine::MAX_RSSI;
//const Int32 WifiStateMachine::SCAN_REQUEST;
//String WifiStateMachine::ACTION_START_SCAN("com.android.server.WifiManager.action.START_SCAN");
//String WifiStateMachine::DELAYED_STOP_COUNTER("DelayedStopCounter");
//const Int32 WifiStateMachine::DRIVER_STOP_REQUEST;
//String WifiStateMachine::ACTION_DELAYED_DRIVER_STOP("com.android.server.WifiManager.action.DELAYED_DRIVER_STOP");
//const String WifiStateMachine::BSSID_STR("bssid=");
//const String WifiStateMachine::FREQ_STR("freq=");
//const String WifiStateMachine::LEVEL_STR("level=");
//const String WifiStateMachine::TSF_STR("tsf=");
//const String WifiStateMachine::FLAGS_STR("flags=");
//const String WifiStateMachine::SSID_STR("ssid=");
//const String WifiStateMachine::DELIMITER_STR("====");

WifiStateMachine::WifiStateMachine(
        /* [in] */ IContext* context,
        /* [in] */ const String& wlanInterface,
        /* [in] */ WifiTrafficPoller* trafficPoller)
    : reason3(3)
{
}

    ECode WifiStateMachine::IsRoaming(
        /* [out] */ Boolean* result){return NOERROR;}

    ECode WifiStateMachine::AutoRoamSetBSSID(
        /* [in] */ Int32 netId,
        /* [in] */ const String& bssid){return NOERROR;}

    ECode WifiStateMachine::AutoRoamSetBSSID(
        /* [in] */ IWifiConfiguration* config,
        /* [in] */ const String& bssid,
        /* [out] */ Boolean* result){return NOERROR;}

    ECode WifiStateMachine::GetVerboseLoggingLevel(
        /* [out] */ Int32* result){return NOERROR;}

    ECode WifiStateMachine::EnableVerboseLogging(
        /* [in] */ Int32 verbose){return NOERROR;}

    ECode WifiStateMachine::GetAggressiveHandover(
        /* [out] */ Int32* result){return NOERROR;}

    ECode WifiStateMachine::EnableAggressiveHandover(
        /* [in] */ Int32 enabled){return NOERROR;}

    ECode WifiStateMachine::SetAllowScansWithTraffic(
        /* [in] */ Int32 enabled){return NOERROR;}

    ECode WifiStateMachine::GetAllowScansWithTraffic(
        /* [out] */ Int32* result){return NOERROR;}

    ECode WifiStateMachine::GetMessenger(
        /* [out] */ IMessenger** result){return NOERROR;}

    ECode WifiStateMachine::GetWifiMonitor(
        /* [out] */ WifiMonitor** result){return NOERROR;}

    ECode WifiStateMachine::SyncPingSupplicant(
        /* [in] */ IAsyncChannel* channel,
        /* [out] */ Boolean* result){return NOERROR;}

    ECode WifiStateMachine::SyncGetChannelList(
        /* [in] */ IAsyncChannel* channel,
        /* [out] */ IList** result){return NOERROR;}

    /**
     * Initiate a wifi scan. If workSource is not null, blame is given to it, otherwise blame is
     * given to callingUid.
     *
     * @param callingUid The uid initiating the wifi scan. Blame will be given here unless
     *                   workSource is specified.
     * @param workSource If not null, blame is given to workSource.
     * @param settings Scan settings, see {@link ScanSettings}.
     */
    ECode WifiStateMachine::StartScan(
        /* [in] */ Int32 callingUid,
        /* [in] */ Int32 scanCounter,
        /* [in] */ IScanSettings* settings,
        /* [in] */ IWorkSource* workSource){return NOERROR;}

    /**
     * start or stop batched scanning using the given settings
     */
    ECode WifiStateMachine::SetBatchedScanSettings(
        /* [in] */ IBatchedScanSettings* settings,
        /* [in] */ Int32 callingUid,
        /* [in] */ Int32 csph,
        /* [in] */ IWorkSource* workSource){return NOERROR;}

    ECode WifiStateMachine::SyncGetBatchedScanResultsList(
        /* [out] */ IList** result){return NOERROR;}

    ECode WifiStateMachine::RequestBatchedScanPoll(){return NOERROR;}

    ECode WifiStateMachine::GetDisconnectedTimeMilli(
        /* [out] */ Int64* result){return NOERROR;}

    ECode WifiStateMachine::ReportOnTime(
        /* [out] */ String* result){return NOERROR;}

    ECode WifiStateMachine::GetWifiLinkLayerStats(
        /* [in] */ Boolean dbg,
        /* [out] */ IWifiLinkLayerStats** result){return NOERROR;}

    ECode WifiStateMachine::StartRadioScanStats(){return NOERROR;}

    ECode WifiStateMachine::CloseRadioScanStats(){return NOERROR;}

    ECode WifiStateMachine::SetSupplicantRunning(
        /* [in] */ Boolean enable){return NOERROR;}

    ECode WifiStateMachine::SetHostApRunning(
        /* [in] */ IWifiConfiguration* wifiConfig,
        /* [in] */ Boolean enable){return NOERROR;}

    ECode WifiStateMachine::SetWifiApConfiguration(
        /* [in] */ IWifiConfiguration* config){return NOERROR;}

    ECode WifiStateMachine::SyncGetWifiApConfiguration(
        /* [out] */ IWifiConfiguration** result){return NOERROR;}

    ECode WifiStateMachine::SyncGetWifiState(
        /* [out] */ Int32* result){return NOERROR;}

    ECode WifiStateMachine::SyncGetWifiStateByName(
        /* [out] */ String* result){return NOERROR;}

    ECode WifiStateMachine::SyncGetWifiApState(
        /* [out] */ Int32* result){return NOERROR;}

    ECode WifiStateMachine::SyncGetWifiApStateByName(
        /* [out] */ String* result){return NOERROR;}

    /**
     * Get status information for the current connection, if any.
     * @return a {@link WifiInfo} object containing information about the current connection
     *
     */
    ECode WifiStateMachine::SyncRequestConnectionInfo(
        /* [out] */ IWifiInfo** result){return NOERROR;}

    ECode WifiStateMachine::SyncGetDhcpResults(
        /* [out] */ IDhcpResults** result){return NOERROR;}

    ECode WifiStateMachine::SetDriverStart(
        /* [in] */ Boolean enable){return NOERROR;}

    ECode WifiStateMachine::SetOperationalMode(
        /* [in] */ Int32 mode){return NOERROR;}

    ECode WifiStateMachine::SyncGetScanResultsList(
        /* [out] */ IList** result){return NOERROR;}

    /**
     * Get unsynchronized pointer to scan result list
     * Can be called only from AutoJoinController which runs in the WifiStateMachine context
     */
    ECode WifiStateMachine::GetScanResultsListNoCopyUnsync(
        /* [out] */ IList** result){return NOERROR;}

    /**
     * Disconnect from Access Point
     */
    ECode WifiStateMachine::DisconnectCommand(){return NOERROR;}

    /**
     * Initiate a reconnection to AP
     */
    ECode WifiStateMachine::ReconnectCommand(){return NOERROR;}

    /**
     * Initiate a re-association to AP
     */
    ECode WifiStateMachine::ReassociateCommand(){return NOERROR;}

    /**
     * Reload networks and then reconnect{return NOERROR;} helps load correct data for TLS networks
     */
    ECode WifiStateMachine::ReloadTlsNetworksAndReconnect(){return NOERROR;}

    /**
     * Add a network synchronously
     *
     * @return network id of the new network
     */
    ECode WifiStateMachine::SyncAddOrUpdateNetwork(
        /* [in] */ IAsyncChannel* channel,
        /* [in] */ IWifiConfiguration* config,
        /* [out] */ Int32* result){return NOERROR;}

    /**
     * Get configured networks synchronously
     * @param channel
     * @return
     */
    ECode WifiStateMachine::SyncGetConfiguredNetworks(
        /* [in] */ Int32 uuid,
        /* [in] */ IAsyncChannel* channel,
        /* [out] */ IList** result){return NOERROR;}

    ECode WifiStateMachine::SyncGetPrivilegedConfiguredNetwork(
        /* [in] */ IAsyncChannel* channel,
        /* [out] */ IList** result){return NOERROR;}

    /**
     * Get connection statistics synchronously
     * @param channel
     * @return
     */
    ECode WifiStateMachine::SyncGetConnectionStatistics(
        /* [in] */ IAsyncChannel* channel,
        /* [out] */ IWifiConnectionStatistics** result){return NOERROR;}

    /**
     * Get adaptors synchronously
     */
    ECode WifiStateMachine::SyncGetSupportedFeatures(
        /* [in] */ IAsyncChannel* channel,
        /* [out] */ Int32* result){return NOERROR;}

    /**
     * Get link layers stats for adapter synchronously
     */
    ECode WifiStateMachine::SyncGetLinkLayerStats(
        /* [in] */ IAsyncChannel* channel,
        /* [out] */ IWifiLinkLayerStats** result){return NOERROR;}

    /**
     * Delete a network
     *
     * @param networkId id of the network to be removed
     */
    ECode WifiStateMachine::SyncRemoveNetwork(
        /* [in] */ IAsyncChannel* channel,
        /* [in] */ Int32 networkId,
        /* [out] */ Boolean* result){return NOERROR;}

    /**
     * Enable a network
     *
     * @param netId network id of the network
     * @param disableOthers true, if all other networks have to be disabled
     * @return {@code true} if the operation succeeds, {@code FALSE} otherwise
     */
    ECode WifiStateMachine::SyncEnableNetwork(
        /* [in] */ IAsyncChannel* channel,
        /* [in] */ Int32 netId,
        /* [in] */ Boolean disableOthers,
        /* [out] */ Boolean* result){return NOERROR;}

    /**
     * Disable a network
     *
     * @param netId network id of the network
     * @return {@code true} if the operation succeeds, {@code FALSE} otherwise
     */
    ECode WifiStateMachine::SyncDisableNetwork(
        /* [in] */ IAsyncChannel* channel,
        /* [in] */ Int32 netId,
        /* [out] */ Boolean* result){return NOERROR;}

    /**
     * Retrieves a WPS-NFC configuration token for the specified network
     * @return a hex string representation of the WPS-NFC configuration token
     */
    ECode WifiStateMachine::SyncGetWpsNfcConfigurationToken(
        /* [in] */ Int32 netId,
        /* [out] */ String* result){return NOERROR;}

    /**
     * Blacklist a BSSID. This will avoid the AP if there are
     * alternate APs to connect
     *
     * @param bssid BSSID of the network
     */
    ECode WifiStateMachine::AddToBlacklist(
        /* [in] */ const String& bssid){return NOERROR;}

    /**
     * Clear the blacklist list
     *
     */
    ECode WifiStateMachine::ClearBlacklist(){return NOERROR;}

    ECode WifiStateMachine::EnableRssiPolling(
        /* [in] */ Boolean enabled){return NOERROR;}

    ECode WifiStateMachine::EnableAllNetworks(){return NOERROR;}

    /**
     * Start filtering Multicast v4 packets
     */
    ECode WifiStateMachine::StartFilteringMulticastV4Packets(){return NOERROR;}

    /**
     * Stop filtering Multicast v4 packets
     */
    ECode WifiStateMachine::StopFilteringMulticastV4Packets(){return NOERROR;}

    /**
     * Start filtering Multicast v4 packets
     */
    ECode WifiStateMachine::StartFilteringMulticastV6Packets(){return NOERROR;}

    /**
     * Stop filtering Multicast v4 packets
     */
    ECode WifiStateMachine::StopFilteringMulticastV6Packets(){return NOERROR;}

    /**
     * Set high performance mode of operation.
     * Enabling would set active power mode and disable suspend optimizations{return NOERROR;}
     * disabling would set auto power mode and enable suspend optimizations
     * @param enable true if enable, FALSE otherwise
     */
    ECode WifiStateMachine::SetHighPerfModeEnabled(
        /* [in] */ Boolean enable){return NOERROR;}

    /**
     * Set the country code
     * @param countryCode following ISO 3166 format
     * @param persist {@code true} if the setting should be remembered.
     */
    ECode WifiStateMachine::SetCountryCode(
        /* [in] */ const String& countryCode,
        /* [in] */ Boolean persist){return NOERROR;}

    ECode WifiStateMachine::SyncIsIbssSupported(
        /* [in] */ IAsyncChannel* channel,
        /* [out] */ Int32* result){return NOERROR;}

    /**
     * Set the operational frequency band
     * @param band
     * @param persist {@code true} if the setting should be remembered.
     */
    ECode WifiStateMachine::SetFrequencyBand(
        /* [in] */ Int32 band,
        /* [in] */ Boolean persist){return NOERROR;}

    /**
     * Enable TDLS for a specific MAC address
     */
    ECode WifiStateMachine::EnableTdls(
        /* [in] */ const String& remoteMacAddress,
        /* [in] */ Boolean enable){return NOERROR;}

    /**
     * Returns the operational frequency band
     */
    ECode WifiStateMachine::GetFrequencyBand(
        /* [out] */ Int32* result){return NOERROR;}

    /**
     * Returns the wifi configuration file
     */
    ECode WifiStateMachine::GetConfigFile(
        /* [out] */ String* result){return NOERROR;}

    /**
     * Send a message indicating bluetooth adapter connection state changed
     */
    ECode WifiStateMachine::SendBluetoothAdapterStateChange(
        /* [in] */ Int32 state){return NOERROR;}

    /**
     * Save configuration on supplicant
     *
     * @return {@code true} if the operation succeeds, {@code FALSE} otherwise
     *
     * TODO: deprecate this
     */
    ECode WifiStateMachine::SyncSaveConfig(
        /* [in] */ IAsyncChannel* channel,
        /* [out] */ Boolean* result){return NOERROR;}

    ECode WifiStateMachine::UpdateBatteryWorkSource(
        /* [in] */ IWorkSource* newSource){return NOERROR;}

    ECode WifiStateMachine::Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args){return NOERROR;}

    /**
     *  Determine if we need to switch network:
     * - the delta determine the urgency to switch and/or or the expected evilness of the disruption
     * - match the uregncy of the switch versus the packet usage at the interface
     */
    ECode WifiStateMachine::ShouldSwitchNetwork(
        /* [in] */ Int32 networkDelta,
        /* [out] */ Boolean* result){return NOERROR;}

    ECode WifiStateMachine::GetTxPacketRate(
        /* [out] */ Double* result){return NOERROR;}

    ECode WifiStateMachine::GetRxPacketRate(
        /* [out] */ Double* result){return NOERROR;}

    ECode WifiStateMachine::HandlePreDhcpSetup(){return NOERROR;}

    ECode WifiStateMachine::StartDhcp(){return NOERROR;}

    ECode WifiStateMachine::RenewDhcp(){return NOERROR;}

    ECode WifiStateMachine::StopDhcp(){return NOERROR;}

    ECode WifiStateMachine::HandlePostDhcpSetup(){return NOERROR;}

    ECode WifiStateMachine::SmToString(
        /* [in] */ IMessage* message,
        /* [out] */ String* result){return NOERROR;}

    ECode WifiStateMachine::SmToString(
        /* [in] */ Int32 what,
        /* [out] */ String* result){return NOERROR;}

    ECode WifiStateMachine::RegisterConnected(){return NOERROR;}

    ECode WifiStateMachine::RegisterDisconnected(){return NOERROR;}

    ECode WifiStateMachine::NoteWifiDisabledWhileAssociated(){return NOERROR;}

    ECode WifiStateMachine::SetInternetAccessState(
        /* [in] */ Boolean enabled){return NOERROR;}

    ECode WifiStateMachine::GetCurrentWifiConfiguration(
        /* [out] */ IWifiConfiguration** result){return NOERROR;}

    ECode WifiStateMachine::GetCurrentScanResult(
        /* [out] */ IScanResult** result){return NOERROR;}

    ECode WifiStateMachine::GetCurrentBSSID(
        /* [out] */ String* result){return NOERROR;}

    ECode WifiStateMachine::UnwantedNetwork(
        /* [in] */ Int32 reason){return NOERROR;}

    ECode WifiStateMachine::StartScanForConfiguration(
        /* [in] */ IWifiConfiguration* config,
        /* [in] */ Boolean restrictChannelList,
        /* [out] */ Boolean* result){return NOERROR;}

    ECode WifiStateMachine::ClearCurrentConfigBSSID(
        /* [in] */ const String& dbg){return NOERROR;}

    ECode WifiStateMachine::ClearConfigBSSID(
        /* [in] */ IWifiConfiguration* config,
        /* [in] */ const String& dbg){return NOERROR;}

    ECode WifiStateMachine::HandleGsmAuthRequest(
        /* [in] */ SimAuthRequestData* requestData){return NOERROR;}

    ECode WifiStateMachine::Handle3GAuthRequest(
        /* [in] */ SimAuthRequestData* requestData){return NOERROR;}

} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namespace Elastos
