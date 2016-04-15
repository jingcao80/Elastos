
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Security.h"
#include "Elastos.Droid.Wifi.h"
#include "elastos/droid/server/wifi/WifiConfigStore.h"

using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Os::CEnvironment;
using Elastos::IO::IFile;
using Elastos::Utility::Regex::IPatternHelper;
using Elastos::Utility::Regex::CPatternHelper;
using Elastos::Utility::CHashMap;
using Elastos::Security::IKeyStoreHelper;
//TODO using Elastos::Security::CKeyStoreHelper;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wifi {

//=====================================================================
//                WifiConfigStore::WpaConfigFileObserver
//=====================================================================
WifiConfigStore::WpaConfigFileObserver::WpaConfigFileObserver()
{
    // ==================before translated======================
    // super(SUPPLICANT_CONFIG_FILE, CLOSE_WRITE);
}

ECode WifiConfigStore::WpaConfigFileObserver::OnEvent(
    /* [in] */ Int32 event,
    /* [in] */ const String& path)
{
    // ==================before translated======================
    // if (event == CLOSE_WRITE) {
    //     File file = new File(SUPPLICANT_CONFIG_FILE);
    //     if (VDBG) localLog("wpa_supplicant.conf changed; new size = " + file.length());
    // }
    assert(0);
    return NOERROR;
}

//=====================================================================
//                           WifiConfigStore
//=====================================================================
const String WifiConfigStore::OLD_PRIVATE_KEY_NAME("private_key");
const String WifiConfigStore::EMPTY_VALUE("NULL");
const String WifiConfigStore::TAG("WifiConfigStore");
const Boolean WifiConfigStore::DBG = true;
Boolean WifiConfigStore::VDBG = false;
Boolean WifiConfigStore::VVDBG = false;
const String WifiConfigStore::SUPPLICANT_CONFIG_FILE("/data/misc/wifi/wpa_supplicant.conf");
static String GetDataDirectory()
{
    AutoPtr<IEnvironment> env;
    CEnvironment::AcquireSingleton((IEnvironment**)&env);
    AutoPtr<IFile> dFile;
    env->GetDataDirectory((IFile**)&dFile);
    String dir;
    dFile->GetAbsolutePath(&dir);
    return dir;
}
const String WifiConfigStore::ipConfigFile(GetDataDirectory() +
             "/misc/wifi/ipconfig.txt");
const String WifiConfigStore::networkHistoryConfigFile(GetDataDirectory() +
             "/misc/wifi/networkHistory.txt");
const String WifiConfigStore::autoJoinConfigFile(GetDataDirectory() +
             "/misc/wifi/autojoinconfig.txt");
const String WifiConfigStore::SSID_KEY("SSID:  ");
const String WifiConfigStore::CONFIG_KEY("CONFIG:  ");
const String WifiConfigStore::CHOICE_KEY("CHOICE:  ");
const String WifiConfigStore::LINK_KEY("LINK:  ");
const String WifiConfigStore::BSSID_KEY("BSSID:  ");
const String WifiConfigStore::BSSID_KEY_END("/BSSID:  ");
const String WifiConfigStore::RSSI_KEY("RSSI:  ");
const String WifiConfigStore::FREQ_KEY("FREQ:  ");
const String WifiConfigStore::DATE_KEY("DATE:  ");
const String WifiConfigStore::MILLI_KEY("MILLI:  ");
const String WifiConfigStore::BLACKLIST_MILLI_KEY("BLACKLIST_MILLI:  ");
const String WifiConfigStore::NETWORK_ID_KEY("ID:  ");
const String WifiConfigStore::PRIORITY_KEY("PRIORITY:  ");
const String WifiConfigStore::DEFAULT_GW_KEY("DEFAULT_GW:  ");
const String WifiConfigStore::AUTH_KEY("AUTH:  ");
const String WifiConfigStore::SEPARATOR_KEY("\n");
const String WifiConfigStore::STATUS_KEY("AUTO_JOIN_STATUS:  ");
const String WifiConfigStore::BSSID_STATUS_KEY("BSSID_STATUS:  ");
const String WifiConfigStore::SELF_ADDED_KEY("SELF_ADDED:  ");
const String WifiConfigStore::FAILURE_KEY("FAILURE:  ");
const String WifiConfigStore::DID_SELF_ADD_KEY("DID_SELF_ADD:  ");
const String WifiConfigStore::PEER_CONFIGURATION_KEY("PEER_CONFIGURATION:  ");
const String WifiConfigStore::CREATOR_UID_KEY("CREATOR_UID_KEY:  ");
const String WifiConfigStore::CONNECT_UID_KEY("CONNECT_UID_KEY:  ");
const String WifiConfigStore::UPDATE_UID_KEY("UPDATE_UID:  ");
const String WifiConfigStore::SUPPLICANT_STATUS_KEY("SUP_STATUS:  ");
const String WifiConfigStore::SUPPLICANT_DISABLE_REASON_KEY("SUP_DIS_REASON:  ");
const String WifiConfigStore::FQDN_KEY("FQDN:  ");
const String WifiConfigStore::NUM_CONNECTION_FAILURES_KEY("CONNECT_FAILURES:  ");
const String WifiConfigStore::NUM_IP_CONFIG_FAILURES_KEY("IP_CONFIG_FAILURES:  ");
const String WifiConfigStore::NUM_AUTH_FAILURES_KEY("AUTH_FAILURES:  ");
const String WifiConfigStore::SCORER_OVERRIDE_KEY("SCORER_OVERRIDE:  ");
const String WifiConfigStore::SCORER_OVERRIDE_AND_SWITCH_KEY("SCORER_OVERRIDE_AND_SWITCH:  ");
const String WifiConfigStore::NO_INTERNET_ACCESS_KEY("NO_INTERNET_ACCESS:  ");
const String WifiConfigStore::EPHEMERAL_KEY("EPHEMERAL:   ");
const String WifiConfigStore::NUM_ASSOCIATION_KEY("NUM_ASSOCIATION:  ");
const String WifiConfigStore::JOIN_ATTEMPT_BOOST_KEY("JOIN_ATTEMPT_BOOST:  ");
const String WifiConfigStore::THRESHOLD_INITIAL_AUTO_JOIN_ATTEMPT_RSSI_MIN_5G_KEY("THRESHOLD_INITIAL_AUTO_JOIN_ATTEMPT_RSSI_MIN_5G:  ");
const String WifiConfigStore::THRESHOLD_INITIAL_AUTO_JOIN_ATTEMPT_RSSI_MIN_24G_KEY("THRESHOLD_INITIAL_AUTO_JOIN_ATTEMPT_RSSI_MIN_24G:  ");
const String WifiConfigStore::THRESHOLD_UNBLACKLIST_HARD_5G_KEY("THRESHOLD_UNBLACKLIST_HARD_5G:  ");
const String WifiConfigStore::THRESHOLD_UNBLACKLIST_SOFT_5G_KEY("THRESHOLD_UNBLACKLIST_SOFT_5G:  ");
const String WifiConfigStore::THRESHOLD_UNBLACKLIST_HARD_24G_KEY("THRESHOLD_UNBLACKLIST_HARD_24G:  ");
const String WifiConfigStore::THRESHOLD_UNBLACKLIST_SOFT_24G_KEY("THRESHOLD_UNBLACKLIST_SOFT_24G:  ");
const String WifiConfigStore::THRESHOLD_GOOD_RSSI_5_KEY("THRESHOLD_GOOD_RSSI_5:  ");
const String WifiConfigStore::THRESHOLD_LOW_RSSI_5_KEY("THRESHOLD_LOW_RSSI_5:  ");
const String WifiConfigStore::THRESHOLD_BAD_RSSI_5_KEY("THRESHOLD_BAD_RSSI_5:  ");
const String WifiConfigStore::THRESHOLD_GOOD_RSSI_24_KEY("THRESHOLD_GOOD_RSSI_24:  ");
const String WifiConfigStore::THRESHOLD_LOW_RSSI_24_KEY("THRESHOLD_LOW_RSSI_24:  ");
const String WifiConfigStore::THRESHOLD_BAD_RSSI_24_KEY("THRESHOLD_BAD_RSSI_24:  ");
const String WifiConfigStore::THRESHOLD_MAX_TX_PACKETS_FOR_NETWORK_SWITCHING_KEY("THRESHOLD_MAX_TX_PACKETS_FOR_NETWORK_SWITCHING:   ");
const String WifiConfigStore::THRESHOLD_MAX_RX_PACKETS_FOR_NETWORK_SWITCHING_KEY("THRESHOLD_MAX_RX_PACKETS_FOR_NETWORK_SWITCHING:   ");
const String WifiConfigStore::THRESHOLD_MAX_TX_PACKETS_FOR_FULL_SCANS_KEY("THRESHOLD_MAX_TX_PACKETS_FOR_FULL_SCANS:   ");
const String WifiConfigStore::THRESHOLD_MAX_RX_PACKETS_FOR_FULL_SCANS_KEY("THRESHOLD_MAX_RX_PACKETS_FOR_FULL_SCANS:   ");
const String WifiConfigStore::THRESHOLD_MAX_TX_PACKETS_FOR_PARTIAL_SCANS_KEY("THRESHOLD_MAX_TX_PACKETS_FOR_PARTIAL_SCANS:   ");
const String WifiConfigStore::THRESHOLD_MAX_RX_PACKETS_FOR_PARTIAL_SCANS_KEY("THRESHOLD_MAX_RX_PACKETS_FOR_PARTIAL_SCANS:   ");
const String WifiConfigStore::MAX_NUM_ACTIVE_CHANNELS_FOR_PARTIAL_SCANS_KEY("MAX_NUM_ACTIVE_CHANNELS_FOR_PARTIAL_SCANS:   ");
const String WifiConfigStore::MAX_NUM_PASSIVE_CHANNELS_FOR_PARTIAL_SCANS_KEY("MAX_NUM_PASSIVE_CHANNELS_FOR_PARTIAL_SCANS:   ");
const String WifiConfigStore::A_BAND_PREFERENCE_RSSI_THRESHOLD_LOW_KEY("A_BAND_PREFERENCE_RSSI_THRESHOLD_LOW:   ");
const String WifiConfigStore::A_BAND_PREFERENCE_RSSI_THRESHOLD_KEY("A_BAND_PREFERENCE_RSSI_THRESHOLD:   ");
const String WifiConfigStore::G_BAND_PREFERENCE_RSSI_THRESHOLD_KEY("G_BAND_PREFERENCE_RSSI_THRESHOLD:   ");
const String WifiConfigStore::ENABLE_AUTOJOIN_WHILE_ASSOCIATED_KEY("ENABLE_AUTOJOIN_WHILE_ASSOCIATED:   ");
const String WifiConfigStore::ASSOCIATED_PARTIAL_SCAN_PERIOD_KEY("ASSOCIATED_PARTIAL_SCAN_PERIOD:   ");
const String WifiConfigStore::ASSOCIATED_FULL_SCAN_BACKOFF_KEY("ASSOCIATED_FULL_SCAN_BACKOFF_PERIOD:   ");
const String WifiConfigStore::ALWAYS_ENABLE_SCAN_WHILE_ASSOCIATED_KEY("ALWAYS_ENABLE_SCAN_WHILE_ASSOCIATED:   ");
const String WifiConfigStore::AUTO_JOIN_SCAN_INTERVAL_WHEN_P2P_CONNECTED_KEY("AUTO_JOIN_SCAN_INTERVAL_WHEN_P2P_CONNECTED:   ");
const String WifiConfigStore::ONLY_LINK_SAME_CREDENTIAL_CONFIGURATIONS_KEY("ONLY_LINK_SAME_CREDENTIAL_CONFIGURATIONS:   ");
const String WifiConfigStore::ENABLE_FULL_BAND_SCAN_WHEN_ASSOCIATED_KEY("ENABLE_FULL_BAND_SCAN_WHEN_ASSOCIATED:   ");
const String WifiConfigStore::ENABLE_AUTO_JOIN_SCAN_WHILE_ASSOCIATED_KEY("ENABLE_AUTO_JOIN_SCAN_WHILE_ASSOCIATED:   ");
const String WifiConfigStore::ENABLE_AUTO_JOIN_WHILE_ASSOCIATED_KEY("ENABLE_AUTO_JOIN_WHILE_ASSOCIATED:   ");
const String WifiConfigStore::ENABLE_CHIP_WAKE_UP_WHILE_ASSOCIATED_KEY("ENABLE_CHIP_WAKE_UP_WHILE_ASSOCIATED:   ");
const String WifiConfigStore::ENABLE_RSSI_POLL_WHILE_ASSOCIATED_KEY("ENABLE_RSSI_POLL_WHILE_ASSOCIATED_KEY:   ");
const String WifiConfigStore::WIFI_VERBOSE_LOGS_KEY("WIFI_VERBOSE_LOGS:   ");
const String WifiConfigStore::DELETED_CONFIG_PSK("Mjkd86jEMGn79KhKll298Uu7-deleted");
static AutoPtr<IPattern> InitConnectChoice()
{
    AutoPtr<IPatternHelper> helper;
    CPatternHelper::AcquireSingleton((IPatternHelper**)&helper);
    AutoPtr<IPattern> pattern;
    helper->Compile(String("(.*)=([0-9]+)"), (IPattern**)&pattern);
    return pattern;
}
AutoPtr<IPattern> WifiConfigStore::mConnectChoice = InitConnectChoice();
AutoPtr< ArrayOf<String> > WifiConfigStore::ENTERPRISE_CONFIG_SUPPLICANT_KEYS = WifiConfigStore::MiddleInitEnterpriseConfigSupplicantKeys();
const Int32 WifiConfigStore::DEFAULT_MAX_DHCP_RETRIES;

WifiConfigStore::WifiConfigStore(
    /* [in] */ IContext* c,
    /* [in] */ WifiNative* wn)
{
    CHashMap::New((IHashMap**)&mConfiguredNetworks);
    CHashMap::New((IHashMap**)&mNetworkIds);
    AutoPtr<IKeyStoreHelper> keyStoreHelper;
    //TODO CKeyStoreHelper::AcquireSingleton((IKeyStoreHelper**)&keyStoreHelper);
    String type;
    keyStoreHelper->GetDefaultType(&type);
    keyStoreHelper->GetInstance(type, (IKeyStore**)&mKeyStore);

    // ==================before translated======================
    // mContext = c;
    // mWifiNative = wn;
    //
    // if (showNetworks) {
    //     mLocalLog = mWifiNative.getLocalLog();
    //     mFileObserver = new WpaConfigFileObserver();
    //     mFileObserver.startWatching();
    // } else {
    //     mLocalLog = null;
    //     mFileObserver = null;
    // }
    //
    // associatedPartialScanPeriodMilli = mContext.getResources().getInteger(
    //         R.integer.config_wifi_framework_associated_scan_interval);
    // loge("associatedPartialScanPeriodMilli set to " + associatedPartialScanPeriodMilli);
    //
    // onlyLinkSameCredentialConfigurations = mContext.getResources().getBoolean(
    //         R.bool.config_wifi_only_link_same_credential_configurations);
    // maxNumActiveChannelsForPartialScans = mContext.getResources().getInteger(
    //         R.integer.config_wifi_framework_associated_partial_scan_max_num_active_channels);
    // maxNumPassiveChannelsForPartialScans = mContext.getResources().getInteger(
    //         R.integer.config_wifi_framework_associated_partial_scan_max_num_passive_channels);
    // associatedFullScanMaxIntervalMilli = mContext.getResources().getInteger(
    //         R.integer.config_wifi_framework_associated_full_scan_max_interval);
    // associatedFullScanBackoff = mContext.getResources().getInteger(
    //         R.integer.config_wifi_framework_associated_full_scan_backoff);
    // enableLinkDebouncing = mContext.getResources().getBoolean(
    //         R.bool.config_wifi_enable_disconnection_debounce);
    //
    // enable5GHzPreference = mContext.getResources().getBoolean(
    //         R.bool.config_wifi_enable_5GHz_preference);
    //
    // bandPreferenceBoostFactor5 = mContext.getResources().getInteger(
    //         R.integer.config_wifi_framework_5GHz_preference_boost_factor);
    // bandPreferencePenaltyFactor5 = mContext.getResources().getInteger(
    //         R.integer.config_wifi_framework_5GHz_preference_penalty_factor);
    //
    // bandPreferencePenaltyThreshold5 = mContext.getResources().getInteger(
    //         R.integer.config_wifi_framework_5GHz_preference_penalty_threshold);
    // bandPreferenceBoostThreshold5 = mContext.getResources().getInteger(
    //         R.integer.config_wifi_framework_5GHz_preference_boost_threshold);
    //
    // associatedHysteresisHigh = mContext.getResources().getInteger(
    //         R.integer.config_wifi_framework_current_association_hysteresis_high);
    // associatedHysteresisLow = mContext.getResources().getInteger(
    //         R.integer.config_wifi_framework_current_association_hysteresis_low);
    //
    // thresholdBadRssi5 = mContext.getResources().getInteger(
    //         R.integer.config_wifi_framework_wifi_score_bad_rssi_threshold_5GHz);
    // thresholdLowRssi5 = mContext.getResources().getInteger(
    //         R.integer.config_wifi_framework_wifi_score_low_rssi_threshold_5GHz);
    // thresholdGoodRssi5 = mContext.getResources().getInteger(
    //         R.integer.config_wifi_framework_wifi_score_good_rssi_threshold_5GHz);
    // thresholdBadRssi24 = mContext.getResources().getInteger(
    //         R.integer.config_wifi_framework_wifi_score_bad_rssi_threshold_24GHz);
    // thresholdLowRssi24 = mContext.getResources().getInteger(
    //         R.integer.config_wifi_framework_wifi_score_low_rssi_threshold_24GHz);
    // thresholdGoodRssi24 = mContext.getResources().getInteger(
    //         R.integer.config_wifi_framework_wifi_score_good_rssi_threshold_24GHz);
    //
    // enableWifiCellularHandoverUserTriggeredAdjustment = mContext.getResources().getBoolean(
    //         R.bool.config_wifi_framework_cellular_handover_enable_user_triggered_adjustment);
    //
    // badLinkSpeed24 = mContext.getResources().getInteger(
    //         R.integer.config_wifi_framework_wifi_score_bad_link_speed_24);
    // badLinkSpeed5 = mContext.getResources().getInteger(
    //         R.integer.config_wifi_framework_wifi_score_bad_link_speed_5);
    // goodLinkSpeed24 = mContext.getResources().getInteger(
    //         R.integer.config_wifi_framework_wifi_score_good_link_speed_24);
    // goodLinkSpeed5 = mContext.getResources().getInteger(
    //         R.integer.config_wifi_framework_wifi_score_good_link_speed_5);
    //
    // maxAuthErrorsToBlacklist = mContext.getResources().getInteger(
    //         R.integer.config_wifi_framework_max_auth_errors_to_blacklist);
    // maxConnectionErrorsToBlacklist = mContext.getResources().getInteger(
    //         R.integer.config_wifi_framework_max_connection_errors_to_blacklist);
    // wifiConfigBlacklistMinTimeMilli = mContext.getResources().getInteger(
    //         R.integer.config_wifi_framework_network_black_list_min_time_milli);
    //
    //
    // enableAutoJoinScanWhenAssociated = mContext.getResources().getBoolean(
    //         R.bool.config_wifi_framework_enable_associated_autojoin_scan);
    //
    // enableAutoJoinWhenAssociated = mContext.getResources().getBoolean(
    //         R.bool.config_wifi_framework_enable_associated_network_selection);
}

ECode WifiConfigStore::EnableVerboseLogging(
    /* [in] */ Int32 verbose)
{
    // ==================before translated======================
    // enableVerboseLogging = verbose;
    // if (verbose > 0) {
    //     VDBG = true;
    //     showNetworks = true;
    // } else {
    //     VDBG = false;
    // }
    // if (verbose > 1) {
    //     VVDBG = true;
    // } else {
    //     VVDBG = false;
    // }
    assert(0);
    return NOERROR;
}

ECode WifiConfigStore::LoadAndEnableAllNetworks()
{
    // ==================before translated======================
    // if (DBG) log("Loading config and enabling all networks ");
    // loadConfiguredNetworks();
    // enableAllNetworks();
    assert(0);
    return NOERROR;
}

ECode WifiConfigStore::GetConfiguredNetworksSize(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mConfiguredNetworks.size();
    assert(0);
    return NOERROR;
}

ECode WifiConfigStore::GetConfiguredNetworks(
    /* [out] */ IList** result)//WifiConfiguration
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return getConfiguredNetworks(null);
    assert(0);
    return NOERROR;
}

ECode WifiConfigStore::GetPrivilegedConfiguredNetworks(
    /* [out] */ IList** result)//WifiConfiguration
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // Map<String, String> pskMap = getCredentialsBySsidMap();
    // return getConfiguredNetworks(pskMap);
    assert(0);
    return NOERROR;
}

ECode WifiConfigStore::GetconfiguredNetworkSize(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (mConfiguredNetworks == null)
    //     return 0;
    // return mConfiguredNetworks.size();
    assert(0);
    return NOERROR;
}

ECode WifiConfigStore::GetRecentConfiguredNetworks(
    /* [in] */ Int32 milli,
    /* [in] */ Boolean copy,
    /* [out] */ IList** result)//WifiConfiguration
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // List<WifiConfiguration> networks = null;
    //
    // for (WifiConfiguration config : mConfiguredNetworks.values()) {
    //     if (config.autoJoinStatus == WifiConfiguration.AUTO_JOIN_DELETED) {
    //         // Do not enumerate and return this configuration to any one,
    //         // instead treat it as unknown. the configuration can still be retrieved
    //         // directly by the key or networkId
    //         continue;
    //     }
    //
    //     // Calculate the RSSI for scan results that are more recent than milli
    //     config.setVisibility(milli);
    //     if (config.visibility == null) {
    //         continue;
    //     }
    //     if (config.visibility.rssi5 == WifiConfiguration.INVALID_RSSI &&
    //             config.visibility.rssi24 == WifiConfiguration.INVALID_RSSI) {
    //         continue;
    //     }
    //     if (networks == null)
    //         networks = new ArrayList<WifiConfiguration>();
    //     if (copy) {
    //         networks.add(new WifiConfiguration(config));
    //     } else {
    //         networks.add(config);
    //     }
    // }
    // return networks;
    assert(0);
    return NOERROR;
}

ECode WifiConfigStore::UpdateConfiguration(
    /* [in] */ IWifiInfo* info)
{
    VALIDATE_NOT_NULL(info);
    // ==================before translated======================
    // WifiConfiguration config = getWifiConfiguration(info.getNetworkId());
    // if (config != null && config.scanResultCache != null) {
    //     ScanResult result = config.scanResultCache.get(info.getBSSID());
    //     if (result != null) {
    //         long previousSeen = result.seen;
    //         int previousRssi = result.level;
    //
    //         // Update the scan result
    //         result.seen = System.currentTimeMillis();
    //         result.level = info.getRssi();
    //
    //         // Average the RSSI value
    //         result.averageRssi(previousRssi, previousSeen,
    //                 WifiAutoJoinController.mScanResultMaximumAge);
    //         if (VDBG) {
    //             loge("updateConfiguration freq=" + result.frequency
    //                 + " BSSID=" + result.BSSID
    //                 + " RSSI=" + result.level
    //                 + " " + config.configKey());
    //         }
    //     }
    // }
    assert(0);
    return NOERROR;
}

ECode WifiConfigStore::GetWifiConfiguration(
    /* [in] */ Int32 netId,
    /* [out] */ IWifiConfiguration** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (mConfiguredNetworks == null)
    //     return null;
    // return mConfiguredNetworks.get(netId);
    assert(0);
    return NOERROR;
}

ECode WifiConfigStore::GetWifiConfiguration(
    /* [in] */ const String& key,
    /* [out] */ IWifiConfiguration** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (key == null)
    //     return null;
    // int hash = key.hashCode();
    // if (mNetworkIds == null)
    //     return null;
    // Integer n = mNetworkIds.get(hash);
    // if (n == null)
    //     return null;
    // int netId = n.intValue();
    // return getWifiConfiguration(netId);
    assert(0);
    return NOERROR;
}

ECode WifiConfigStore::EnableAllNetworks()
{
    // ==================before translated======================
    // long now = System.currentTimeMillis();
    // boolean networkEnabledStateChanged = false;
    //
    // for(WifiConfiguration config : mConfiguredNetworks.values()) {
    //
    //     if(config != null && config.status == Status.DISABLED
    //             && (config.autoJoinStatus
    //             <= WifiConfiguration.AUTO_JOIN_DISABLED_ON_AUTH_FAILURE)) {
    //
    //         // Wait for 20 minutes before reenabling config that have known, repeated connection
    //         // or DHCP failures
    //         if (config.disableReason == WifiConfiguration.DISABLED_DHCP_FAILURE
    //                 || config.disableReason == WifiConfiguration.DISABLED_ASSOCIATION_REJECT
    //                 || config.disableReason == WifiConfiguration.DISABLED_AUTH_FAILURE) {
    //             if (config.blackListTimestamp != 0
    //                    && now > config.blackListTimestamp
    //                    && (now - config.blackListTimestamp) < wifiConfigBlacklistMinTimeMilli) {
    //                 continue;
    //             }
    //         }
    //
    //         if(mWifiNative.enableNetwork(config.networkId, false)) {
    //             networkEnabledStateChanged = true;
    //             config.status = Status.ENABLED;
    //
    //             // Reset the blacklist condition
    //             config.numConnectionFailures = 0;
    //             config.numIpConfigFailures = 0;
    //             config.numAuthFailures = 0;
    //
    //             // Reenable the wifi configuration
    //             config.setAutoJoinStatus(WifiConfiguration.AUTO_JOIN_ENABLED);
    //         } else {
    //             loge("Enable network failed on " + config.networkId);
    //         }
    //     }
    // }
    //
    // if (networkEnabledStateChanged) {
    //     mWifiNative.saveConfig();
    //     sendConfiguredNetworksChangedBroadcast();
    // }
    assert(0);
    return NOERROR;
}

ECode WifiConfigStore::SelectNetwork(
    /* [in] */ Int32 netId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    //  if (VDBG) localLog("selectNetwork", netId);
    //  if (netId == INVALID_NETWORK_ID) return false;
    //
    //  // Reset the priority of each network at start or if it goes too high.
    //  if (mLastPriority == -1 || mLastPriority > 1000000) {
    //      for(WifiConfiguration config : mConfiguredNetworks.values()) {
    //          if (config.networkId != INVALID_NETWORK_ID) {
    //              config.priority = 0;
    //              addOrUpdateNetworkNative(config, -1);
    //          }
    //      }
    //      mLastPriority = 0;
    //  }
    //
    //  // Set to the highest priority and save the configuration.
    //  WifiConfiguration config = new WifiConfiguration();
    //  config.networkId = netId;
    //  config.priority = ++mLastPriority;
    //
    //  addOrUpdateNetworkNative(config, -1);
    //  mWifiNative.saveConfig();
    //
    //  /* Enable the given network while disabling all other networks */
    //  enableNetworkWithoutBroadcast(netId, true);
    //
    // /* Avoid saving the config & sending a broadcast to prevent settings
    //  * from displaying a disabled list of networks */
    //  return true;
    assert(0);
    return NOERROR;
}

ECode WifiConfigStore::SaveNetwork(
    /* [in] */ IWifiConfiguration* config,
    /* [in] */ Int32 uid,
    /* [out] */ NetworkUpdateResult** result)
{
    VALIDATE_NOT_NULL(config);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // WifiConfiguration conf;
    //
    // // A new network cannot have null SSID
    // if (config == null || (config.networkId == INVALID_NETWORK_ID &&
    //         config.SSID == null)) {
    //     return new NetworkUpdateResult(INVALID_NETWORK_ID);
    // }
    // if (VDBG) localLog("WifiConfigStore: saveNetwork netId", config.networkId);
    // if (VDBG) {
    //     loge("WifiConfigStore saveNetwork, size=" + mConfiguredNetworks.size()
    //             + " SSID=" + config.SSID
    //             + " Uid=" + Integer.toString(config.creatorUid)
    //             + "/" + Integer.toString(config.lastUpdateUid));
    // }
    // boolean newNetwork = (config.networkId == INVALID_NETWORK_ID);
    // NetworkUpdateResult result = addOrUpdateNetworkNative(config, uid);
    // int netId = result.getNetworkId();
    //
    // if (VDBG) localLog("WifiConfigStore: saveNetwork got it back netId=", netId);
    //
    // /* enable a new network */
    // if (newNetwork && netId != INVALID_NETWORK_ID) {
    //     if (VDBG) localLog("WifiConfigStore: will enable netId=", netId);
    //
    //     mWifiNative.enableNetwork(netId, false);
    //     conf = mConfiguredNetworks.get(netId);
    //     if (conf != null)
    //         conf.status = Status.ENABLED;
    // }
    //
    // conf = mConfiguredNetworks.get(netId);
    // if (conf != null) {
    //     if (conf.autoJoinStatus != WifiConfiguration.AUTO_JOIN_ENABLED) {
    //         if (VDBG) localLog("WifiConfigStore: re-enabling: " + conf.SSID);
    //
    //         // reenable autojoin, since new information has been provided
    //         conf.setAutoJoinStatus(WifiConfiguration.AUTO_JOIN_ENABLED);
    //         enableNetworkWithoutBroadcast(conf.networkId, false);
    //     }
    //     if (VDBG) {
    //         loge("WifiConfigStore: saveNetwork got config back netId="
    //                 + Integer.toString(netId)
    //                 + " uid=" + Integer.toString(config.creatorUid));
    //     }
    // }
    //
    // mWifiNative.saveConfig();
    // sendConfiguredNetworksChangedBroadcast(conf, result.isNewNetwork() ?
    //         WifiManager.CHANGE_REASON_ADDED : WifiManager.CHANGE_REASON_CONFIG_CHANGE);
    // return result;
    assert(0);
    return NOERROR;
}

ECode WifiConfigStore::DriverRoamedFrom(
    /* [in] */ IWifiInfo* info)
{
    VALIDATE_NOT_NULL(info);
    // ==================before translated======================
    // if (info != null
    //     && info.getBSSID() != null
    //     && ScanResult.is5GHz(info.getFrequency())
    //     && info.getRssi() > (bandPreferenceBoostThreshold5 + 3)) {
    //     WifiConfiguration config = getWifiConfiguration(info.getNetworkId());
    //     if (config != null) {
    //         if (config.scanResultCache != null) {
    //             ScanResult result = config.scanResultCache.get(info.getBSSID());
    //             if (result != null) {
    //                 result.setAutoJoinStatus(ScanResult.AUTO_ROAM_DISABLED + 1);
    //             }
    //         }
    //     }
    // }
    assert(0);
    return NOERROR;
}

ECode WifiConfigStore::SaveWifiConfigBSSID(
    /* [in] */ IWifiConfiguration* config)
{
    VALIDATE_NOT_NULL(config);
    // ==================before translated======================
    // // Sanity check the config is valid
    // if (config == null || (config.networkId == INVALID_NETWORK_ID &&
    //         config.SSID == null)) {
    //     return;
    // }
    //
    // // If an app specified a BSSID then dont over-write it
    // if (config.BSSID != null && config.BSSID != "any") {
    //     return;
    // }
    //
    // // If autojoin specified a BSSID then write it in the network block
    // if (config.autoJoinBSSID != null) {
    //     loge("saveWifiConfigBSSID Setting BSSID for " + config.configKey()
    //             + " to " + config.autoJoinBSSID);
    //     if (!mWifiNative.setNetworkVariable(
    //             config.networkId,
    //             WifiConfiguration.bssidVarName,
    //             config.autoJoinBSSID)) {
    //         loge("failed to set BSSID: " + config.autoJoinBSSID);
    //     } else if (config.autoJoinBSSID.equals("any")) {
    //         // Paranoia, we just want to make sure that we restore the config to normal
    //         mWifiNative.saveConfig();
    //     }
    // }
    assert(0);
    return NOERROR;
}

ECode WifiConfigStore::UpdateStatus(
    /* [in] */ Int32 netId,
    /* [in] */ NetworkInfoDetailedState* state)
{
    VALIDATE_NOT_NULL(state);
    // ==================before translated======================
    // if (netId != INVALID_NETWORK_ID) {
    //     WifiConfiguration config = mConfiguredNetworks.get(netId);
    //     if (config == null) return;
    //     switch (state) {
    //         case CONNECTED:
    //             config.status = Status.CURRENT;
    //             //we successfully connected, hence remove the blacklist
    //             config.setAutoJoinStatus(WifiConfiguration.AUTO_JOIN_ENABLED);
    //             break;
    //         case DISCONNECTED:
    //             //If network is already disabled, keep the status
    //             if (config.status == Status.CURRENT) {
    //                 config.status = Status.ENABLED;
    //             }
    //             break;
    //         default:
    //             //do nothing, retain the existing state
    //             break;
    //     }
    // }
    assert(0);
    return NOERROR;
}

ECode WifiConfigStore::ForgetNetwork(
    /* [in] */ Int32 netId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (showNetworks) localLog("forgetNetwork", netId);
    //
    // boolean remove = removeConfigAndSendBroadcastIfNeeded(netId);
    // if (!remove) {
    //     //success but we dont want to remove the network from supplicant conf file
    //     return true;
    // }
    // if (mWifiNative.removeNetwork(netId)) {
    //     mWifiNative.saveConfig();
    //     return true;
    // } else {
    //     loge("Failed to remove network " + netId);
    //     return false;
    // }
    assert(0);
    return NOERROR;
}

ECode WifiConfigStore::AddOrUpdateNetwork(
    /* [in] */ IWifiConfiguration* config,
    /* [in] */ Int32 uid,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(config);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (showNetworks) localLog("addOrUpdateNetwork id=", config.networkId);
    // //adding unconditional message to chase b/15111865
    // Log.e(TAG, " key=" + config.configKey() + " netId=" + Integer.toString(config.networkId)
    //         + " uid=" + Integer.toString(config.creatorUid)
    //         + "/" + Integer.toString(config.lastUpdateUid));
    // NetworkUpdateResult result = addOrUpdateNetworkNative(config, uid);
    // if (result.getNetworkId() != WifiConfiguration.INVALID_NETWORK_ID) {
    //     WifiConfiguration conf = mConfiguredNetworks.get(result.getNetworkId());
    //     if (conf != null) {
    //         sendConfiguredNetworksChangedBroadcast(conf,
    //             result.isNewNetwork ? WifiManager.CHANGE_REASON_ADDED :
    //                     WifiManager.CHANGE_REASON_CONFIG_CHANGE);
    //     }
    // }
    // return result.getNetworkId();
    assert(0);
    return NOERROR;
}

ECode WifiConfigStore::RemoveNetwork(
    /* [in] */ Int32 netId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (showNetworks) localLog("removeNetwork", netId);
    // boolean ret = mWifiNative.removeNetwork(netId);
    // if (ret) {
    //     removeConfigAndSendBroadcastIfNeeded(netId);
    // }
    // return ret;
    assert(0);
    return NOERROR;
}

ECode WifiConfigStore::EnableNetwork(
    /* [in] */ Int32 netId,
    /* [in] */ Boolean disableOthers,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // boolean ret = enableNetworkWithoutBroadcast(netId, disableOthers);
    // if (disableOthers) {
    //     if (VDBG) localLog("enableNetwork(disableOthers=true) ", netId);
    //     sendConfiguredNetworksChangedBroadcast();
    // } else {
    //     if (VDBG) localLog("enableNetwork(disableOthers=false) ", netId);
    //     WifiConfiguration enabledNetwork = null;
    //     synchronized(mConfiguredNetworks) {
    //         enabledNetwork = mConfiguredNetworks.get(netId);
    //     }
    //     // check just in case the network was removed by someone else.
    //     if (enabledNetwork != null) {
    //         sendConfiguredNetworksChangedBroadcast(enabledNetwork,
    //                 WifiManager.CHANGE_REASON_CONFIG_CHANGE);
    //     }
    // }
    // return ret;
    assert(0);
    return NOERROR;
}

ECode WifiConfigStore::EnableNetworkWithoutBroadcast(
    /* [in] */ Int32 netId,
    /* [in] */ Boolean disableOthers,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // boolean ret = mWifiNative.enableNetwork(netId, disableOthers);
    //
    // WifiConfiguration config = mConfiguredNetworks.get(netId);
    // if (config != null) config.status = Status.ENABLED;
    //
    // if (disableOthers) {
    //     markAllNetworksDisabledExcept(netId);
    // }
    // return ret;
    assert(0);
    return NOERROR;
}

ECode WifiConfigStore::DisableAllNetworks()
{
    // ==================before translated======================
    // if (VDBG) localLog("disableAllNetworks");
    // boolean networkDisabled = false;
    // for(WifiConfiguration config : mConfiguredNetworks.values()) {
    //     if(config != null && config.status != Status.DISABLED) {
    //         if(mWifiNative.disableNetwork(config.networkId)) {
    //             networkDisabled = true;
    //             config.status = Status.DISABLED;
    //         } else {
    //             loge("Disable network failed on " + config.networkId);
    //         }
    //     }
    // }
    //
    // if (networkDisabled) {
    //     sendConfiguredNetworksChangedBroadcast();
    // }
    assert(0);
    return NOERROR;
}

ECode WifiConfigStore::DisableNetwork(
    /* [in] */ Int32 netId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return disableNetwork(netId, WifiConfiguration.DISABLED_UNKNOWN_REASON);
    assert(0);
    return NOERROR;
}

ECode WifiConfigStore::DisableNetwork(
    /* [in] */ Int32 netId,
    /* [in] */ Int32 reason,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (VDBG) localLog("disableNetwork", netId);
    // boolean ret = mWifiNative.disableNetwork(netId);
    // WifiConfiguration network = null;
    // WifiConfiguration config = mConfiguredNetworks.get(netId);
    //
    // if (VDBG) {
    //     if (config != null) {
    //         loge("disableNetwork netId=" + Integer.toString(netId)
    //                 + " SSID=" + config.SSID
    //                 + " disabled=" + (config.status == Status.DISABLED)
    //                 + " reason=" + Integer.toString(config.disableReason));
    //     }
    // }
    // /* Only change the reason if the network was not previously disabled
    // /* and the reason is not DISABLED_BY_WIFI_MANAGER, that is, if a 3rd party
    //  * set its configuration as disabled, then leave it disabled */
    // if (config != null && config.status != Status.DISABLED
    //         && config.disableReason != WifiConfiguration.DISABLED_BY_WIFI_MANAGER) {
    //     config.status = Status.DISABLED;
    //     config.disableReason = reason;
    //     network = config;
    // }
    // if (reason == WifiConfiguration.DISABLED_BY_WIFI_MANAGER) {
    //     // Make sure autojoin wont reenable this configuration without further user
    //     // intervention
    //     config.status = Status.DISABLED;
    //     config.autoJoinStatus = WifiConfiguration.AUTO_JOIN_DISABLED_USER_ACTION;
    // }
    // if (network != null) {
    //     sendConfiguredNetworksChangedBroadcast(network,
    //             WifiManager.CHANGE_REASON_CONFIG_CHANGE);
    // }
    // return ret;
    assert(0);
    return NOERROR;
}

ECode WifiConfigStore::SaveConfig(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mWifiNative.saveConfig();
    assert(0);
    return NOERROR;
}

ECode WifiConfigStore::StartWpsWithPinFromAccessPoint(
    /* [in] */ IWpsInfo* config,
    /* [out] */ IWpsResult** result)
{
    VALIDATE_NOT_NULL(config);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // WpsResult result = new WpsResult();
    // if (mWifiNative.startWpsRegistrar(config.BSSID, config.pin)) {
    //     /* WPS leaves all networks disabled */
    //     markAllNetworksDisabled();
    //     result.status = WpsResult.Status.SUCCESS;
    // } else {
    //     loge("Failed to start WPS pin method configuration");
    //     result.status = WpsResult.Status.FAILURE;
    // }
    // return result;
    assert(0);
    return NOERROR;
}

ECode WifiConfigStore::StartWpsWithPinFromDevice(
    /* [in] */ IWpsInfo* config,
    /* [out] */ IWpsResult** result)
{
    VALIDATE_NOT_NULL(config);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // WpsResult result = new WpsResult();
    // result.pin = mWifiNative.startWpsPinDisplay(config.BSSID);
    // /* WPS leaves all networks disabled */
    // if (!TextUtils.isEmpty(result.pin)) {
    //     markAllNetworksDisabled();
    //     result.status = WpsResult.Status.SUCCESS;
    // } else {
    //     loge("Failed to start WPS pin method configuration");
    //     result.status = WpsResult.Status.FAILURE;
    // }
    // return result;
    assert(0);
    return NOERROR;
}

ECode WifiConfigStore::StartWpsPbc(
    /* [in] */ IWpsInfo* config,
    /* [out] */ IWpsResult** result)
{
    VALIDATE_NOT_NULL(config);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // WpsResult result = new WpsResult();
    // if (mWifiNative.startWpsPbc(config.BSSID)) {
    //     /* WPS leaves all networks disabled */
    //     markAllNetworksDisabled();
    //     result.status = WpsResult.Status.SUCCESS;
    // } else {
    //     loge("Failed to start WPS push button configuration");
    //     result.status = WpsResult.Status.FAILURE;
    // }
    // return result;
    assert(0);
    return NOERROR;
}

ECode WifiConfigStore::GetStaticIpConfiguration(
    /* [in] */ Int32 netId,
    /* [out] */ IStaticIpConfiguration** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // WifiConfiguration config = mConfiguredNetworks.get(netId);
    // if (config != null) {
    //     return config.getStaticIpConfiguration();
    // }
    // return null;
    assert(0);
    return NOERROR;
}

ECode WifiConfigStore::SetStaticIpConfiguration(
    /* [in] */ Int32 netId,
    /* [in] */ IStaticIpConfiguration* staticIpConfiguration)
{
    VALIDATE_NOT_NULL(staticIpConfiguration);
    // ==================before translated======================
    // WifiConfiguration config = mConfiguredNetworks.get(netId);
    // if (config != null) {
    //     config.setStaticIpConfiguration(staticIpConfiguration);
    // }
    assert(0);
    return NOERROR;
}

ECode WifiConfigStore::SetDefaultGwMacAddress(
    /* [in] */ Int32 netId,
    /* [in] */ const String& macAddress)
{
    // ==================before translated======================
    // WifiConfiguration config = mConfiguredNetworks.get(netId);
    // if (config != null) {
    //     //update defaultGwMacAddress
    //     config.defaultGwMacAddress = macAddress;
    // }
    assert(0);
    return NOERROR;
}

ECode WifiConfigStore::GetProxyProperties(
    /* [in] */ Int32 netId,
    /* [out] */ IProxyInfo** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // WifiConfiguration config = mConfiguredNetworks.get(netId);
    // if (config != null) {
    //     return config.getHttpProxy();
    // }
    // return null;
    assert(0);
    return NOERROR;
}

ECode WifiConfigStore::IsUsingStaticIp(
    /* [in] */ Int32 netId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // WifiConfiguration config = mConfiguredNetworks.get(netId);
    // if (config != null && config.getIpAssignment() == IpAssignment.STATIC) {
    //     return true;
    // }
    // return false;
    assert(0);
    return NOERROR;
}

ECode WifiConfigStore::LoadConfiguredNetworks()
{
    // ==================before translated======================
    // String listStr = mWifiNative.listNetworks();
    // mLastPriority = 0;
    //
    // mConfiguredNetworks.clear();
    // mNetworkIds.clear();
    //
    // if (listStr == null)
    //     return;
    //
    // String[] lines = listStr.split("\n");
    //
    // if (showNetworks) {
    //     localLog("WifiConfigStore: loadConfiguredNetworks:  ");
    //     for (String net : lines) {
    //         localLog(net);
    //     }
    // }
    //
    // // Skip the first line, which is a header
    // for (int i = 1; i < lines.length; i++) {
    //     String[] result = lines[i].split("\t");
    //     // network-id | ssid | bssid | flags
    //     WifiConfiguration config = new WifiConfiguration();
    //     try {
    //         config.networkId = Integer.parseInt(result[0]);
    //     } catch(NumberFormatException e) {
    //         loge("Failed to read network-id '" + result[0] + "'");
    //         continue;
    //     }
    //     if (result.length > 3) {
    //         if (result[3].indexOf("[CURRENT]") != -1)
    //             config.status = WifiConfiguration.Status.CURRENT;
    //         else if (result[3].indexOf("[DISABLED]") != -1)
    //             config.status = WifiConfiguration.Status.DISABLED;
    //         else
    //             config.status = WifiConfiguration.Status.ENABLED;
    //     } else {
    //         config.status = WifiConfiguration.Status.ENABLED;
    //     }
    //
    //     readNetworkVariables(config);
    //
    //     String psk = readNetworkVariableFromSupplicantFile(config.SSID, "psk");
    //     if (psk!= null && psk.equals(DELETED_CONFIG_PSK)) {
    //         // This is a config we previously deleted, ignore it
    //         if (showNetworks) {
    //             localLog("found deleted network " + config.SSID + " ", config.networkId);
    //         }
    //         config.setAutoJoinStatus(WifiConfiguration.AUTO_JOIN_DELETED);
    //         config.priority = 0;
    //     }
    //
    //     if (config.priority > mLastPriority) {
    //         mLastPriority = config.priority;
    //     }
    //
    //     config.setIpAssignment(IpAssignment.DHCP);
    //     config.setProxySettings(ProxySettings.NONE);
    //
    //     if (mNetworkIds.containsKey(configKey(config))) {
    //         // That SSID is already known, just ignore this duplicate entry
    //         if (showNetworks)
    //             localLog("Duplicate network found ", config.networkId);
    //
    //         Integer n = mNetworkIds.get(configKey(config));
    //         WifiConfiguration tempCfg = mConfiguredNetworks.get(n);
    //
    //         if ( (tempCfg != null &&
    //               tempCfg.status != WifiConfiguration.Status.CURRENT) &&
    //               config.status == WifiConfiguration.Status.CURRENT) {
    //
    //             // Clear the existing entry, we don't need it
    //             mConfiguredNetworks.remove(tempCfg.networkId);
    //             mNetworkIds.remove(configKey(tempCfg));
    //
    //             // Add current entry to the list
    //             mConfiguredNetworks.put(config.networkId, config);
    //             mNetworkIds.put(configKey(config), config.networkId);
    //
    //             // Enable AutoJoin status and indicate the network as
    //             // duplicate The duplicateNetwork flag will be used
    //             // to decide whether to restore network configurations
    //             // in readNetworkHistory() along with IP and proxy settings
    //             config.setAutoJoinStatus(WifiConfiguration.AUTO_JOIN_ENABLED);
    //             config.duplicateNetwork = true;
    //         }
    //     } else if (config.isValid()) {
    //         mConfiguredNetworks.put(config.networkId, config);
    //         mNetworkIds.put(configKey(config), config.networkId);
    //         if (showNetworks) localLog("loaded configured network", config.networkId);
    //     } else {
    //         if (showNetworks) log("Ignoring loaded configured for network " + config.networkId
    //             + " because config are not valid");
    //     }
    // }
    //
    // readIpAndProxyConfigurations();
    // readNetworkHistory();
    // readAutoJoinConfig();
    //
    // sendConfiguredNetworksChangedBroadcast();
    //
    // if (showNetworks) localLog("loadConfiguredNetworks loaded " + mNetworkIds.size() + " networks");
    //
    // if (mNetworkIds.size() == 0) {
    //     // no networks? Lets log if the wpa_supplicant.conf file contents
    //     BufferedReader reader = null;
    //     try {
    //         reader = new BufferedReader(new FileReader(SUPPLICANT_CONFIG_FILE));
    //         if (DBG) {
    //             localLog("--- Begin wpa_supplicant.conf Contents ---", true);
    //             for (String line = reader.readLine(); line != null; line = reader.readLine()) {
    //                 localLog(line, true);
    //             }
    //             localLog("--- End wpa_supplicant.conf Contents ---", true);
    //         }
    //     } catch (FileNotFoundException e) {
    //         localLog("Could not open " + SUPPLICANT_CONFIG_FILE + ", " + e, true);
    //     } catch (IOException e) {
    //         localLog("Could not read " + SUPPLICANT_CONFIG_FILE + ", " + e, true);
    //     } finally {
    //         try {
    //             if (reader != null) {
    //                 reader.close();
    //             }
    //         } catch (IOException e) {
    //             // Just ignore the fact that we couldn't close
    //         }
    //     }
    // }
    assert(0);
    return NOERROR;
}

ECode WifiConfigStore::GetNetworkIdFromSsid(
    /* [in] */ const String& ssid,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // int networkId = 0;
    // int ret = -1;
    // String listStr = mWifiNative.listNetworks();
    // if (VDBG) loge("getNetworkIdFromSsid " + ssid);
    // if (listStr == null)
    //     return -1;
    // String[] lines = listStr.split("\n");
    //
    // /* Skip the first line, which is a header */
    // for (int i = 1; i < lines.length; i++) {
    //     String[] result = lines[i].split("\t");
    //     if (VDBG) loge("getNetworkIdFromSsid " + result[1]);
    //     /* network-id | ssid | bssid | flags */
    //
    //     if (result[1].equals(ssid)) {
    //         try {
    //             networkId = Integer.parseInt(result[0]);
    //             if (VDBG) loge("getNetworkIdFromSsid " + networkId);
    //             return networkId;
    //         } catch(NumberFormatException e) {
    //             loge("Failed to read network-id '" + result[0] + "'");
    //             continue;
    //         }
    //     }
    // }
    // return -1;
    assert(0);
    return NOERROR;
}

ECode WifiConfigStore::NeedsUnlockedKeyStore(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    //
    // // Any network using certificates to authenticate access requires
    // // unlocked key store; unless the certificates can be stored with
    // // hardware encryption
    //
    // for(WifiConfiguration config : mConfiguredNetworks.values()) {
    //
    //     if (config.allowedKeyManagement.get(KeyMgmt.WPA_EAP)
    //             && config.allowedKeyManagement.get(KeyMgmt.IEEE8021X)) {
    //
    //         if (needsSoftwareBackedKeyStore(config.enterpriseConfig)) {
    //             return true;
    //         }
    //     }
    // }
    //
    // return false;
    assert(0);
    return NOERROR;
}

ECode WifiConfigStore::WriteKnownNetworkHistory()
{
    // ==================before translated======================
    // boolean needUpdate = false;
    //
    // /* Make a copy */
    // final List<WifiConfiguration> networks = new ArrayList<WifiConfiguration>();
    // for (WifiConfiguration config : mConfiguredNetworks.values()) {
    //     networks.add(new WifiConfiguration(config));
    //     if (config.dirty == true) {
    //         config.dirty = false;
    //         needUpdate = true;
    //     }
    // }
    // if (VDBG) {
    //     loge(" writeKnownNetworkHistory() num networks:" +
    //             mConfiguredNetworks.size() + " needWrite=" + needUpdate);
    // }
    // if (needUpdate == false) {
    //     return;
    // }
    // mWriter.write(networkHistoryConfigFile, new DelayedDiskWrite.Writer() {
    //     public void onWriteCalled(DataOutputStream out) throws IOException {
    //         for (WifiConfiguration config : networks) {
    //             //loge("onWriteCalled write SSID: " + config.SSID);
    //            /* if (config.getLinkProperties() != null)
    //                 loge(" lp " + config.getLinkProperties().toString());
    //             else
    //                 loge("attempt config w/o lp");
    //             */
    //
    //             if (VDBG) {
    //                 int num = 0;
    //                 int numlink = 0;
    //                 if (config.connectChoices != null) {
    //                     num = config.connectChoices.size();
    //                 }
    //                 if (config.linkedConfigurations != null) {
    //                     numlink = config.linkedConfigurations.size();
    //                 }
    //                 loge("saving network history: " + config.configKey()  + " gw: " +
    //                         config.defaultGwMacAddress + " autojoin-status: " +
    //                         config.autoJoinStatus + " ephemeral=" + config.ephemeral
    //                         + " choices:" + Integer.toString(num)
    //                         + " link:" + Integer.toString(numlink)
    //                         + " status:" + Integer.toString(config.status)
    //                         + " nid:" + Integer.toString(config.networkId));
    //             }
    //
    //             if (config.isValid() == false)
    //                 continue;
    //
    //             if (config.SSID == null) {
    //                 if (VDBG) {
    //                     loge("writeKnownNetworkHistory trying to write config with null SSID");
    //                 }
    //                 continue;
    //             }
    //             if (VDBG) {
    //                 loge("writeKnownNetworkHistory write config " + config.configKey());
    //             }
    //             out.writeUTF(CONFIG_KEY + config.configKey() + SEPARATOR_KEY);
    //
    //             out.writeUTF(SSID_KEY + config.SSID + SEPARATOR_KEY);
    //             out.writeUTF(FQDN_KEY + config.FQDN + SEPARATOR_KEY);
    //
    //             out.writeUTF(PRIORITY_KEY + Integer.toString(config.priority) + SEPARATOR_KEY);
    //             out.writeUTF(STATUS_KEY + Integer.toString(config.autoJoinStatus)
    //                     + SEPARATOR_KEY);
    //             out.writeUTF(SUPPLICANT_STATUS_KEY + Integer.toString(config.status)
    //                     + SEPARATOR_KEY);
    //             out.writeUTF(SUPPLICANT_DISABLE_REASON_KEY
    //                     + Integer.toString(config.disableReason)
    //                     + SEPARATOR_KEY);
    //             out.writeUTF(NETWORK_ID_KEY + Integer.toString(config.networkId)
    //                     + SEPARATOR_KEY);
    //             out.writeUTF(SELF_ADDED_KEY + Boolean.toString(config.selfAdded)
    //                     + SEPARATOR_KEY);
    //             out.writeUTF(DID_SELF_ADD_KEY + Boolean.toString(config.didSelfAdd)
    //                     + SEPARATOR_KEY);
    //             out.writeUTF(NO_INTERNET_ACCESS_KEY
    //                     + Boolean.toString(config.noInternetAccess)
    //                     + SEPARATOR_KEY);
    //             out.writeUTF(EPHEMERAL_KEY
    //                     + Boolean.toString(config.ephemeral)
    //                     + SEPARATOR_KEY);
    //             if (config.peerWifiConfiguration != null) {
    //                 out.writeUTF(PEER_CONFIGURATION_KEY + config.peerWifiConfiguration
    //                         + SEPARATOR_KEY);
    //             }
    //             out.writeUTF(NUM_CONNECTION_FAILURES_KEY
    //                     + Integer.toString(config.numConnectionFailures)
    //                     + SEPARATOR_KEY);
    //             out.writeUTF(NUM_AUTH_FAILURES_KEY
    //                     + Integer.toString(config.numAuthFailures)
    //                     + SEPARATOR_KEY);
    //             out.writeUTF(NUM_IP_CONFIG_FAILURES_KEY
    //                     + Integer.toString(config.numIpConfigFailures)
    //                     + SEPARATOR_KEY);
    //             out.writeUTF(SCORER_OVERRIDE_KEY + Integer.toString(config.numScorerOverride)
    //                     + SEPARATOR_KEY);
    //             out.writeUTF(SCORER_OVERRIDE_AND_SWITCH_KEY
    //                     + Integer.toString(config.numScorerOverrideAndSwitchedNetwork)
    //                     + SEPARATOR_KEY);
    //             out.writeUTF(NUM_ASSOCIATION_KEY
    //                     + Integer.toString(config.numAssociation)
    //                     + SEPARATOR_KEY);
    //             out.writeUTF(JOIN_ATTEMPT_BOOST_KEY
    //                     + Integer.toString(config.autoJoinUseAggressiveJoinAttemptThreshold)
    //                     + SEPARATOR_KEY);
    //             //out.writeUTF(BLACKLIST_MILLI_KEY + Long.toString(config.blackListTimestamp)
    //             //        + SEPARATOR_KEY);
    //             out.writeUTF(CREATOR_UID_KEY + Integer.toString(config.creatorUid)
    //                     + SEPARATOR_KEY);
    //             out.writeUTF(CONNECT_UID_KEY + Integer.toString(config.lastConnectUid)
    //                     + SEPARATOR_KEY);
    //             out.writeUTF(UPDATE_UID_KEY + Integer.toString(config.lastUpdateUid)
    //                     + SEPARATOR_KEY);
    //             String allowedKeyManagementString =
    //                     makeString(config.allowedKeyManagement,
    //                             WifiConfiguration.KeyMgmt.strings);
    //             out.writeUTF(AUTH_KEY + allowedKeyManagementString + SEPARATOR_KEY);
    //
    //             if (config.connectChoices != null) {
    //                 for (String key : config.connectChoices.keySet()) {
    //                     Integer choice = config.connectChoices.get(key);
    //                     out.writeUTF(CHOICE_KEY + key + "="
    //                             + choice.toString() + SEPARATOR_KEY);
    //                 }
    //             }
    //             if (config.linkedConfigurations != null) {
    //                 loge("writeKnownNetworkHistory write linked "
    //                         + config.linkedConfigurations.size());
    //
    //                 for (String key : config.linkedConfigurations.keySet()) {
    //                     out.writeUTF(LINK_KEY + key + SEPARATOR_KEY);
    //                 }
    //             }
    //
    //             String macAddress = config.defaultGwMacAddress;
    //             if (macAddress != null) {
    //                 out.writeUTF(DEFAULT_GW_KEY + macAddress + SEPARATOR_KEY);
    //             }
    //
    //             if (config.scanResultCache != null) {
    //                 for (ScanResult result : config.scanResultCache.values()) {
    //                     out.writeUTF(BSSID_KEY + result.BSSID + SEPARATOR_KEY);
    //
    //                     out.writeUTF(FREQ_KEY + Integer.toString(result.frequency)
    //                             + SEPARATOR_KEY);
    //
    //                     out.writeUTF(RSSI_KEY + Integer.toString(result.level)
    //                             + SEPARATOR_KEY);
    //
    //                     out.writeUTF(BSSID_STATUS_KEY
    //                             + Integer.toString(result.autoJoinStatus)
    //                             + SEPARATOR_KEY);
    //
    //                     //if (result.seen != 0) {
    //                     //    out.writeUTF(MILLI_KEY + Long.toString(result.seen)
    //                     //            + SEPARATOR_KEY);
    //                     //}
    //                     out.writeUTF(BSSID_KEY_END + SEPARATOR_KEY);
    //                 }
    //             }
    //             if (config.lastFailure != null) {
    //                 out.writeUTF(FAILURE_KEY + config.lastFailure + SEPARATOR_KEY);
    //             }
    //             out.writeUTF(SEPARATOR_KEY);
    //             // Add extra blank lines for clarity
    //             out.writeUTF(SEPARATOR_KEY);
    //             out.writeUTF(SEPARATOR_KEY);
    //         }
    //     }
    //
    // });
    assert(0);
    return NOERROR;
}

ECode WifiConfigStore::SetLastSelectedConfiguration(
    /* [in] */ Int32 netId)
{
    // ==================before translated======================
    // if (DBG) {
    //     loge("setLastSelectedConfiguration " + Integer.toString(netId));
    // }
    // if (netId == WifiConfiguration.INVALID_NETWORK_ID) {
    //     lastSelectedConfiguration = null;
    // } else {
    //     WifiConfiguration selected = getWifiConfiguration(netId);
    //     if (selected == null) {
    //         lastSelectedConfiguration = null;
    //     } else {
    //         lastSelectedConfiguration = selected.configKey();
    //         selected.numConnectionFailures = 0;
    //         selected.numIpConfigFailures = 0;
    //         selected.numAuthFailures = 0;
    //         if (VDBG) {
    //             loge("setLastSelectedConfiguration now: " + lastSelectedConfiguration);
    //         }
    //     }
    // }
    assert(0);
    return NOERROR;
}

ECode WifiConfigStore::GetLastSelectedConfiguration(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return lastSelectedConfiguration;
    assert(0);
    return NOERROR;
}

ECode WifiConfigStore::IsLastSelectedConfiguration(
    /* [in] */ IWifiConfiguration* config,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(config);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return (lastSelectedConfiguration != null
    //         && config != null
    //         && lastSelectedConfiguration.equals(config.configKey()));
    assert(0);
    return NOERROR;
}

ECode WifiConfigStore::LinkConfiguration(
    /* [in] */ IWifiConfiguration* config)
{
    VALIDATE_NOT_NULL(config);
    // ==================before translated======================
    //
    // if (config.scanResultCache != null && config.scanResultCache.size() > 6) {
    //     // Ignore configurations with large number of BSSIDs
    //     return;
    // }
    // if (!config.allowedKeyManagement.get(KeyMgmt.WPA_PSK)) {
    //     // Only link WPA_PSK config
    //     return;
    // }
    // for (WifiConfiguration link : mConfiguredNetworks.values()) {
    //     boolean doLink = false;
    //
    //     if (link.configKey().equals(config.configKey())) {
    //         continue;
    //     }
    //
    //     if (link.autoJoinStatus == WifiConfiguration.AUTO_JOIN_DELETED) {
    //         continue;
    //     }
    //
    //     // Autojoin will be allowed to dynamically jump from a linked configuration
    //     // to another, hence only link configurations that have equivalent level of security
    //     if (!link.allowedKeyManagement.equals(config.allowedKeyManagement)) {
    //         continue;
    //     }
    //
    //     if (link.scanResultCache != null && link.scanResultCache.size() > 6) {
    //         // Ignore configurations with large number of BSSIDs
    //         continue;
    //     }
    //
    //     if (config.defaultGwMacAddress != null && link.defaultGwMacAddress != null) {
    //         // If both default GW are known, link only if they are equal
    //         if (config.defaultGwMacAddress.equals(link.defaultGwMacAddress)) {
    //             if (VDBG) {
    //                 loge("linkConfiguration link due to same gw " + link.SSID +
    //                         " and " + config.SSID + " GW " + config.defaultGwMacAddress);
    //             }
    //             doLink = true;
    //         }
    //     } else {
    //         // We do not know BOTH default gateways hence we will try to link
    //         // hoping that WifiConfigurations are indeed behind the same gateway.
    //         // once both WifiConfiguration have been tried and thus once both efault gateways
    //         // are known we will revisit the choice of linking them
    //         if ((config.scanResultCache != null) && (config.scanResultCache.size() <= 6)
    //                 && (link.scanResultCache != null) && (link.scanResultCache.size() <= 6)) {
    //             for (String abssid : config.scanResultCache.keySet()) {
    //                 for (String bbssid : link.scanResultCache.keySet()) {
    //                     if (VVDBG) {
    //                         loge("linkConfiguration try to link due to DBDC BSSID match "
    //                                 + link.SSID +
    //                                 " and " + config.SSID + " bssida " + abssid
    //                                 + " bssidb " + bbssid);
    //                     }
    //                     if (abssid.regionMatches(true, 0, bbssid, 0, 16)) {
    //                         // If first 16 ascii characters of BSSID matches,
    //                         // we assume this is a DBDC
    //                         doLink = true;
    //                     }
    //                 }
    //             }
    //         }
    //     }
    //
    //     if (doLink == true && onlyLinkSameCredentialConfigurations) {
    //         String apsk = readNetworkVariableFromSupplicantFile(link.SSID, "psk");
    //         String bpsk = readNetworkVariableFromSupplicantFile(config.SSID, "psk");
    //         if (apsk == null || bpsk == null
    //                 || TextUtils.isEmpty(apsk) || TextUtils.isEmpty(apsk)
    //                 || apsk.equals("*") || apsk.equals(DELETED_CONFIG_PSK)
    //                 || !apsk.equals(bpsk)) {
    //             doLink = false;
    //         }
    //     }
    //
    //     if (doLink) {
    //         if (VDBG) {
    //            loge("linkConfiguration: will link " + link.configKey()
    //                    + " and " + config.configKey());
    //         }
    //         if (link.linkedConfigurations == null) {
    //             link.linkedConfigurations = new HashMap<String, Integer>();
    //         }
    //         if (config.linkedConfigurations == null) {
    //             config.linkedConfigurations = new HashMap<String, Integer>();
    //         }
    //         if (link.linkedConfigurations.get(config.configKey()) == null) {
    //             link.linkedConfigurations.put(config.configKey(), Integer.valueOf(1));
    //             link.dirty = true;
    //         }
    //         if (config.linkedConfigurations.get(link.configKey()) == null) {
    //             config.linkedConfigurations.put(link.configKey(), Integer.valueOf(1));
    //             config.dirty = true;
    //         }
    //     } else {
    //         if (link.linkedConfigurations != null
    //                 && (link.linkedConfigurations.get(config.configKey()) != null)) {
    //             if (VDBG) {
    //                 loge("linkConfiguration: un-link " + config.configKey()
    //                         + " from " + link.configKey());
    //             }
    //             link.dirty = true;
    //             link.linkedConfigurations.remove(config.configKey());
    //         }
    //         if (config.linkedConfigurations != null
    //                 && (config.linkedConfigurations.get(link.configKey()) != null)) {
    //             if (VDBG) {
    //                 loge("linkConfiguration: un-link " + link.configKey()
    //                         + " from " + config.configKey());
    //             }
    //             config.dirty = true;
    //             config.linkedConfigurations.remove(link.configKey());
    //         }
    //     }
    // }
    assert(0);
    return NOERROR;
}

ECode WifiConfigStore::AssociateWithConfiguration(
    /* [in] */ IScanResult* result,
    /* [out] */ IWifiConfiguration** wifiConfig)
{
    VALIDATE_NOT_NULL(result);
    VALIDATE_NOT_NULL(wifiConfig);
    // ==================before translated======================
    // String configKey = WifiConfiguration.configKey(result);
    // if (configKey == null) {
    //     if (DBG) loge("associateWithConfiguration(): no config key " );
    //     return null;
    // }
    //
    // // Need to compare with quoted string
    // String SSID = "\"" + result.SSID + "\"";
    //
    // if (VVDBG) {
    //     loge("associateWithConfiguration(): try " + configKey);
    // }
    //
    // WifiConfiguration config = null;
    // for (WifiConfiguration link : mConfiguredNetworks.values()) {
    //     boolean doLink = false;
    //
    //     if (link.autoJoinStatus == WifiConfiguration.AUTO_JOIN_DELETED || link.selfAdded) {
    //         if (VVDBG) loge("associateWithConfiguration(): skip selfadd " + link.configKey() );
    //         // Make sure we dont associate the scan result to a deleted config
    //         continue;
    //     }
    //
    //     if (!link.allowedKeyManagement.get(KeyMgmt.WPA_PSK)) {
    //         if (VVDBG) loge("associateWithConfiguration(): skip non-PSK " + link.configKey() );
    //         // Make sure we dont associate the scan result to a non-PSK config
    //         continue;
    //     }
    //
    //     if (configKey.equals(link.configKey())) {
    //         if (VVDBG) loge("associateWithConfiguration(): found it!!! " + configKey );
    //         return link; // Found it exactly
    //     }
    //
    //     if ((link.scanResultCache != null) && (link.scanResultCache.size() <= 6)) {
    //         for (String bssid : link.scanResultCache.keySet()) {
    //             if (result.BSSID.regionMatches(true, 0, bssid, 0, 16)
    //                     && SSID.regionMatches(false, 0, link.SSID, 0, 4)) {
    //                 // If first 16 ascii characters of BSSID matches, and first 3
    //                 // characters of SSID match, we assume this is a home setup
    //                 // and thus we will try to transfer the password from the known
    //                 // BSSID/SSID to the recently found BSSID/SSID
    //
    //                 // If (VDBG)
    //                 //    loge("associateWithConfiguration OK " );
    //                 doLink = true;
    //                 break;
    //             }
    //         }
    //     }
    //
    //     if (doLink) {
    //         // Try to make a non verified WifiConfiguration, but only if the original
    //         // configuration was not self already added
    //         if (VDBG) {
    //             loge("associateWithConfiguration: will create " +
    //                     result.SSID + " and associate it with: " + link.SSID);
    //         }
    //         config = wifiConfigurationFromScanResult(result);
    //         if (config != null) {
    //             config.selfAdded = true;
    //             config.didSelfAdd = true;
    //             config.dirty = true;
    //             config.peerWifiConfiguration = link.configKey();
    //             if (config.allowedKeyManagement.equals(link.allowedKeyManagement) &&
    //                     config.allowedKeyManagement.get(KeyMgmt.WPA_PSK)) {
    //                 // Transfer the credentials from the configuration we are linking from
    //                 String psk = readNetworkVariableFromSupplicantFile(link.SSID, "psk");
    //                 if (psk != null) {
    //                     config.preSharedKey = psk;
    //                     if (VDBG) {
    //                         if (config.preSharedKey != null)
    //                             loge(" transfer PSK : " + config.preSharedKey);
    //                     }
    //
    //                     // Link configurations
    //                     if (link.linkedConfigurations == null) {
    //                         link.linkedConfigurations = new HashMap<String, Integer>();
    //                     }
    //                     if (config.linkedConfigurations == null) {
    //                         config.linkedConfigurations = new HashMap<String, Integer>();
    //                     }
    //                     link.linkedConfigurations.put(config.configKey(), Integer.valueOf(1));
    //                     config.linkedConfigurations.put(link.configKey(), Integer.valueOf(1));
    //                 } else {
    //                     config = null;
    //                 }
    //             } else {
    //                 config = null;
    //             }
    //             if (config != null) break;
    //         }
    //     }
    // }
    // return config;
    assert(0);
    return NOERROR;
}

ECode WifiConfigStore::MakeChannelList(
    /* [in] */ IWifiConfiguration* config,
    /* [in] */ Int32 age,
    /* [in] */ Boolean _restrict,
    /* [out] */ IHashSet** result)
{
    VALIDATE_NOT_NULL(config);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (config == null)
    //     return null;
    // long now_ms = System.currentTimeMillis();
    //
    // HashSet<Integer> channels = new HashSet<Integer>();
    //
    // //get channels for this configuration, if there are at least 2 BSSIDs
    // if (config.scanResultCache == null && config.linkedConfigurations == null) {
    //     return null;
    // }
    //
    // if (VDBG) {
    //     StringBuilder dbg = new StringBuilder();
    //     dbg.append("makeChannelList age=" + Integer.toString(age)
    //             + " for " + config.configKey()
    //             + " max=" + maxNumActiveChannelsForPartialScans);
    //     if (config.scanResultCache != null) {
    //         dbg.append(" bssids=" + config.scanResultCache.size());
    //     }
    //     if (config.linkedConfigurations != null) {
    //         dbg.append(" linked=" + config.linkedConfigurations.size());
    //     }
    //     loge(dbg.toString());
    // }
    //
    // int numChannels = 0;
    // if (config.scanResultCache != null && config.scanResultCache.size() > 0) {
    //     for (ScanResult result : config.scanResultCache.values()) {
    //         //TODO : cout active and passive channels separately
    //         if (numChannels > maxNumActiveChannelsForPartialScans) {
    //             break;
    //         }
    //         if (VDBG) {
    //             boolean test = (now_ms - result.seen) < age;
    //             loge("has " + result.BSSID + " freq=" + Integer.toString(result.frequency)
    //                     + " age=" + Long.toString(now_ms - result.seen) + " ?=" + test);
    //         }
    //         if (((now_ms - result.seen) < age)/*||(!_restrict || result.is24GHz())*/) {
    //             channels.add(result.frequency);
    //             numChannels++;
    //         }
    //     }
    // }
    //
    // //get channels for linked configurations
    // if (config.linkedConfigurations != null) {
    //     for (String key : config.linkedConfigurations.keySet()) {
    //         WifiConfiguration linked = getWifiConfiguration(key);
    //         if (linked == null)
    //             continue;
    //         if (linked.scanResultCache == null) {
    //             continue;
    //         }
    //         for (ScanResult result : linked.scanResultCache.values()) {
    //             if (VDBG) {
    //                 loge("has link: " + result.BSSID
    //                         + " freq=" + Integer.toString(result.frequency)
    //                         + " age=" + Long.toString(now_ms - result.seen));
    //             }
    //             if (numChannels > maxNumActiveChannelsForPartialScans) {
    //                 break;
    //             }
    //             if (((now_ms - result.seen) < age)/*||(!_restrict || result.is24GHz())*/) {
    //                 channels.add(result.frequency);
    //                 numChannels++;
    //             }
    //         }
    //     }
    // }
    // return channels;
    assert(0);
    return NOERROR;
}

ECode WifiConfigStore::UpdateSavedNetworkHistory(
    /* [in] */ IScanResult* scanResult,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(scanResult);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // int numConfigFound = 0;
    // if (scanResult == null)
    //     return false;
    //
    // String SSID = "\"" + scanResult.SSID + "\"";
    //
    // for (WifiConfiguration config : mConfiguredNetworks.values()) {
    //     boolean found = false;
    //
    //     if (config.SSID == null || !config.SSID.equals(SSID)) {
    //         // SSID mismatch
    //         if (VVDBG) {
    //             loge("updateSavedNetworkHistory(): SSID mismatch " + config.configKey()
    //                     + " SSID=" + config.SSID + " " + SSID);
    //         }
    //         continue;
    //     }
    //     if (VDBG) {
    //         loge("updateSavedNetworkHistory(): try " + config.configKey()
    //                 + " SSID=" + config.SSID + " " + scanResult.SSID
    //                 + " " + scanResult.capabilities
    //                 + " ajst=" + config.autoJoinStatus);
    //     }
    //     if (scanResult.capabilities.contains("WEP")
    //             && config.configKey().contains("WEP")) {
    //         found = true;
    //     } else if (scanResult.capabilities.contains("PSK")
    //             && config.configKey().contains("PSK")) {
    //         found = true;
    //     } else if (scanResult.capabilities.contains("EAP")
    //             && config.configKey().contains("EAP")) {
    //         found = true;
    //     } else if (!scanResult.capabilities.contains("WEP")
    //         && !scanResult.capabilities.contains("PSK")
    //         && !scanResult.capabilities.contains("EAP")
    //         && !config.configKey().contains("WEP")
    //             && !config.configKey().contains("PSK")
    //             && !config.configKey().contains("EAP")) {
    //         found = true;
    //     }
    //
    //     if (found) {
    //         numConfigFound ++;
    //
    //         if (config.autoJoinStatus >= WifiConfiguration.AUTO_JOIN_DELETED) {
    //             if (VVDBG) {
    //                 loge("updateSavedNetworkHistory(): found a deleted, skip it...  "
    //                         + config.configKey());
    //             }
    //             // The scan result belongs to a deleted config:
    //             //   - increment numConfigFound to remember that we found a config
    //             //            matching for this scan result
    //             //   - dont do anything since the config was deleted, just skip...
    //             continue;
    //         }
    //
    //         if (config.scanResultCache == null) {
    //             config.scanResultCache = new HashMap<String, ScanResult>();
    //         }
    //
    //         // Adding a new BSSID
    //         ScanResult result = config.scanResultCache.get(scanResult.BSSID);
    //         if (result == null) {
    //             config.dirty = true;
    //         } else {
    //             // transfer the black list status
    //             scanResult.autoJoinStatus = result.autoJoinStatus;
    //             scanResult.blackListTimestamp = result.blackListTimestamp;
    //             scanResult.numIpConfigFailures = result.numIpConfigFailures;
    //             scanResult.numConnection = result.numConnection;
    //             scanResult.isAutoJoinCandidate = result.isAutoJoinCandidate;
    //         }
    //
    //         // Add the scan result to this WifiConfiguration
    //         config.scanResultCache.put(scanResult.BSSID, scanResult);
    //         // Since we added a scan result to this configuration, re-attempt linking
    //         linkConfiguration(config);
    //     }
    //
    //     if (VDBG && found) {
    //         String status = "";
    //         if (scanResult.autoJoinStatus > 0) {
    //             status = " status=" + Integer.toString(scanResult.autoJoinStatus);
    //         }
    //         loge("        got known scan result " +
    //                 scanResult.BSSID + " key : "
    //                 + config.configKey() + " num: " +
    //                 Integer.toString(config.scanResultCache.size())
    //                 + " rssi=" + Integer.toString(scanResult.level)
    //                 + " freq=" + Integer.toString(scanResult.frequency)
    //                 + status);
    //     }
    // }
    // return numConfigFound != 0;
    assert(0);
    return NOERROR;
}

ECode WifiConfigStore::WifiConfigurationFromScanResult(
    /* [in] */ IScanResult* result,
    /* [out] */ IWifiConfiguration** wifiConfig)
{
    VALIDATE_NOT_NULL(result);
    VALIDATE_NOT_NULL(wifiConfig);
    // ==================before translated======================
    // WifiConfiguration config = new WifiConfiguration();
    //
    // config.SSID = "\"" + result.SSID + "\"";
    //
    // if (VDBG) {
    //     loge("WifiConfiguration from scan results " +
    //             config.SSID + " cap " + result.capabilities);
    // }
    // if (result.capabilities.contains("WEP")) {
    //     config.allowedKeyManagement.set(KeyMgmt.NONE);
    //     config.allowedAuthAlgorithms.set(WifiConfiguration.AuthAlgorithm.OPEN); //?
    //     config.allowedAuthAlgorithms.set(WifiConfiguration.AuthAlgorithm.SHARED);
    // }
    //
    // if (result.capabilities.contains("PSK")) {
    //     config.allowedKeyManagement.set(KeyMgmt.WPA_PSK);
    // }
    //
    // if (result.capabilities.contains("EAP")) {
    //     //this is probably wrong, as we don't have a way to enter the enterprise config
    //     config.allowedKeyManagement.set(KeyMgmt.WPA_EAP);
    //     config.allowedKeyManagement.set(KeyMgmt.IEEE8021X);
    // }
    //
    // config.scanResultCache = new HashMap<String, ScanResult>();
    // if (config.scanResultCache == null)
    //     return null;
    // config.scanResultCache.put(result.BSSID, result);
    //
    // return config;
    assert(0);
    return NOERROR;
}

ECode WifiConfigStore::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    VALIDATE_NOT_NULL(fd);
    VALIDATE_NOT_NULL(pw);
    VALIDATE_NOT_NULL(args);
    // ==================before translated======================
    // pw.println("Dump of WifiConfigStore");
    // pw.println("mLastPriority " + mLastPriority);
    // pw.println("Configured networks");
    // for (WifiConfiguration conf : getConfiguredNetworks()) {
    //     pw.println(conf);
    // }
    // pw.println();
    //
    // if (mLocalLog != null) {
    //     pw.println("WifiConfigStore - Log Begin ----");
    //     mLocalLog.dump(fd, pw, args);
    //     pw.println("WifiConfigStore - Log End ----");
    // }
    assert(0);
    return NOERROR;
}

ECode WifiConfigStore::GetConfigFile(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return ipConfigFile;
    assert(0);
    return NOERROR;
}

Boolean WifiConfigStore::NeedsKeyStore(
    /* [in] */ IWifiEnterpriseConfig* config)
{
    // ==================before translated======================
    // // Has no keys to be installed
    // if (config.getClientCertificate() == null && config.getCaCertificate() == null)
    //     return false;
    // return true;
    assert(0);
    return FALSE;
}

Boolean WifiConfigStore::IsHardwareBackedKey(
    /* [in] */ IPrivateKey* key)
{
    // ==================before translated======================
    // return KeyChain.isBoundKeyAlgorithm(key.getAlgorithm());
    assert(0);
    return FALSE;
}

Boolean WifiConfigStore::HasHardwareBackedKey(
    /* [in] */ ICertificate* certificate)
{
    // ==================before translated======================
    // return KeyChain.isBoundKeyAlgorithm(certificate.getPublicKey().getAlgorithm());
    assert(0);
    return FALSE;
}

Boolean WifiConfigStore::NeedsSoftwareBackedKeyStore(
    /* [in] */ IWifiEnterpriseConfig* config)
{
    // ==================before translated======================
    // String client = config.getClientCertificateAlias();
    // if (!TextUtils.isEmpty(client)) {
    //     // a valid client certificate is configured
    //
    //     // BUGBUG: keyStore.get() never returns certBytes; because it is not
    //     // taking WIFI_UID as a parameter. It always looks for certificate
    //     // with SYSTEM_UID, and never finds any Wifi certificates. Assuming that
    //     // all certificates need software keystore until we get the get() API
    //     // fixed.
    //
    //     return true;
    // }
    //
    // /*
    // try {
    //
    //     if (DBG) Slog.d(TAG, "Loading client certificate " + Credentials
    //             .USER_CERTIFICATE + client);
    //
    //     CertificateFactory factory = CertificateFactory.getInstance("X.509");
    //     if (factory == null) {
    //         Slog.e(TAG, "Error getting certificate factory");
    //         return;
    //     }
    //
    //     byte[] certBytes = keyStore.get(Credentials.USER_CERTIFICATE + client);
    //     if (certBytes != null) {
    //         Certificate cert = (X509Certificate) factory.generateCertificate(
    //                 new ByteArrayInputStream(certBytes));
    //
    //         if (cert != null) {
    //             mNeedsSoftwareKeystore = hasHardwareBackedKey(cert);
    //
    //             if (DBG) Slog.d(TAG, "Loaded client certificate " + Credentials
    //                     .USER_CERTIFICATE + client);
    //             if (DBG) Slog.d(TAG, "It " + (mNeedsSoftwareKeystore ? "needs" :
    //                     "does not need" ) + " software key store");
    //         } else {
    //             Slog.d(TAG, "could not generate certificate");
    //         }
    //     } else {
    //         Slog.e(TAG, "Could not load client certificate " + Credentials
    //                 .USER_CERTIFICATE + client);
    //         mNeedsSoftwareKeystore = true;
    //     }
    //
    // } catch(CertificateException e) {
    //     Slog.e(TAG, "Could not read certificates");
    //     mCaCert = null;
    //     mClientCertificate = null;
    // }
    // */
    //
    // return false;
    assert(0);
    return FALSE;
}

ECode WifiConfigStore::HandleBadNetworkDisconnectReport(
    /* [in] */ Int32 netId,
    /* [in] */ IWifiInfo* info)
{
    VALIDATE_NOT_NULL(info);
    // ==================before translated======================
    // /* TODO verify the bad network is current */
    // WifiConfiguration config = mConfiguredNetworks.get(netId);
    // if (config != null) {
    //     if ((info.getRssi() < WifiConfiguration.UNWANTED_BLACKLIST_SOFT_RSSI_24
    //             && info.is24GHz()) || (info.getRssi() <
    //                     WifiConfiguration.UNWANTED_BLACKLIST_SOFT_RSSI_5 && info.is5GHz())) {
    //         // We got disconnected and RSSI was bad, so disable light
    //         config.setAutoJoinStatus(WifiConfiguration.AUTO_JOIN_TEMPORARY_DISABLED
    //                 + WifiConfiguration.UNWANTED_BLACKLIST_SOFT_BUMP);
    //         loge("handleBadNetworkDisconnectReport (+4) "
    //                 + Integer.toString(netId) + " " + info);
    //     } else {
    //         // We got disabled but RSSI is good, so disable hard
    //         config.setAutoJoinStatus(WifiConfiguration.AUTO_JOIN_TEMPORARY_DISABLED
    //                 + WifiConfiguration.UNWANTED_BLACKLIST_HARD_BUMP);
    //         loge("handleBadNetworkDisconnectReport (+8) "
    //                 + Integer.toString(netId) + " " + info);
    //     }
    // }
    // // Record last time Connectivity Service switched us away from WiFi and onto Cell
    // lastUnwantedNetworkDisconnectTimestamp = System.currentTimeMillis();
    assert(0);
    return NOERROR;
}

ECode WifiConfigStore::HandleBSSIDBlackList(
    /* [in] */ Int32 netId,
    /* [in] */ const String& BSSID,
    /* [in] */ Boolean enable,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // boolean found = false;
    // if (BSSID == null)
    //     return found;
    //
    // // Look for the BSSID in our config store
    // for (WifiConfiguration config : mConfiguredNetworks.values()) {
    //     if (config.scanResultCache != null) {
    //         for (ScanResult result: config.scanResultCache.values()) {
    //             if (result.BSSID.equals(BSSID)) {
    //                 if (enable) {
    //                     result.setAutoJoinStatus(ScanResult.ENABLED);
    //                 } else {
    //                     // Black list the BSSID we were trying to join
    //                     // so as the Roam state machine
    //                     // doesn't pick it up over and over
    //                     result.setAutoJoinStatus(ScanResult.AUTO_ROAM_DISABLED);
    //                     found = true;
    //                 }
    //             }
    //         }
    //     }
    // }
    // return found;
    assert(0);
    return NOERROR;
}

ECode WifiConfigStore::HandleDisabledAPs(
    /* [in] */ Boolean enable,
    /* [in] */ const String& BSSID,
    /* [in] */ Int32 reason)
{
    // ==================before translated======================
    // if (BSSID == null)
    //     return;
    // for (WifiConfiguration config : mConfiguredNetworks.values()) {
    //     if (config.scanResultCache != null) {
    //         for (ScanResult result: config.scanResultCache.values()) {
    //             if (result.BSSID.equals(BSSID)) {
    //                 if (enable) {
    //                     config.BSSID = "any";
    //                     result.setAutoJoinStatus(ScanResult.ENABLED);
    //                     // enable auto join for the blacklisted BSSID
    //                     config.setAutoJoinStatus(WifiConfiguration.AUTO_JOIN_ENABLED);
    //                 } else {
    //                     result.setAutoJoinStatus(ScanResult.AUTO_ROAM_DISABLED);
    //                     config.BSSID = BSSID;
    //                     config.setAutoJoinStatus(WifiConfiguration.AUTO_JOIN_TEMPORARY_DISABLED);
    //                 }
    //             }
    //         }
    //     }
    // }
    assert(0);
    return NOERROR;
}

ECode WifiConfigStore::GetMaxDhcpRetries(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return Settings.Global.getInt(mContext.getContentResolver(),
    //         Settings.Global.WIFI_MAX_DHCP_RETRY_COUNT,
    //         DEFAULT_MAX_DHCP_RETRIES);
    assert(0);
    return NOERROR;
}

ECode WifiConfigStore::HandleSSIDStateChange(
    /* [in] */ Int32 netId,
    /* [in] */ Boolean enabled,
    /* [in] */ const String& message,
    /* [in] */ const String& BSSID)
{
    // ==================before translated======================
    // WifiConfiguration config = mConfiguredNetworks.get(netId);
    // if (config != null) {
    //     if (enabled) {
    //         loge("SSID re-enabled for  " + config.configKey() +
    //                 " had autoJoinStatus=" + Integer.toString(config.autoJoinStatus)
    //                 + " self added " + config.selfAdded + " ephemeral " + config.ephemeral);
    //         //TODO: http://b/16381983 Fix Wifi Network Blacklisting
    //         //TODO: really I don't know if re-enabling is right but we
    //         //TODO: should err on the side of trying to connect
    //         //TODO: even if the attempt will fail
    //         if (config.autoJoinStatus == WifiConfiguration.AUTO_JOIN_DISABLED_ON_AUTH_FAILURE) {
    //             config.setAutoJoinStatus(WifiConfiguration.AUTO_JOIN_ENABLED);
    //         }
    //     } else {
    //         loge("SSID temp disabled for  " + config.configKey() +
    //                 " had autoJoinStatus=" + Integer.toString(config.autoJoinStatus)
    //                 + " self added " + config.selfAdded + " ephemeral " + config.ephemeral);
    //         if (message != null) {
    //             loge(" message=" + message);
    //         }
    //         if (config.selfAdded && config.lastConnected == 0) {
    //             // This is a network we self added, and we never succeeded,
    //             // the user did not create this network and never entered its credentials,
    //             // so we want to be very aggressive in disabling it completely.
    //             removeConfigAndSendBroadcastIfNeeded(config.networkId);
    //         } else {
    //             if (message != null) {
    //                 if (message.contains("no identity")) {
    //                     config.setAutoJoinStatus(
    //                             WifiConfiguration.AUTO_JOIN_DISABLED_NO_CREDENTIALS);
    //                     if (DBG) {
    //                         loge("no identity blacklisted " + config.configKey() + " to "
    //                                 + Integer.toString(config.autoJoinStatus));
    //                     }
    //                 } else if (message.contains("WRONG_KEY")
    //                         || message.contains("AUTH_FAILED")) {
    //                     // This configuration has received an auth failure, so disable it
    //                     // temporarily because we don't want auto-join to try it out.
    //                     // this network may be re-enabled by the "usual"
    //                     // enableAllNetwork function
    //                     config.numAuthFailures++;
    //                     if (config.numAuthFailures > maxAuthErrorsToBlacklist) {
    //                         config.setAutoJoinStatus
    //                                 (WifiConfiguration.AUTO_JOIN_DISABLED_ON_AUTH_FAILURE);
    //                         disableNetwork(netId,
    //                                 WifiConfiguration.DISABLED_AUTH_FAILURE);
    //                         loge("Authentication failure, blacklist " + config.configKey() + " "
    //                                     + Integer.toString(config.networkId)
    //                                     + " num failures " + config.numAuthFailures);
    //                     }
    //                 } else if (message.contains("DHCP FAILURE")) {
    //                     config.numIpConfigFailures++;
    //                     config.lastConnectionFailure = System.currentTimeMillis();
    //                     int maxRetries = getMaxDhcpRetries();
    //                     // maxRetries == 0 means keep trying forever
    //                     if (maxRetries > 0 && config.numIpConfigFailures > maxRetries) {
    //                         /**
    //                          * If we've exceeded the maximum number of retries for DHCP
    //                          * to a given network, disable the network
    //                          */
    //                         config.setAutoJoinStatus
    //                                 (WifiConfiguration.AUTO_JOIN_DISABLED_ON_AUTH_FAILURE);
    //                         disableNetwork(netId, WifiConfiguration.DISABLED_DHCP_FAILURE);
    //                         loge("DHCP failure, blacklist " + config.configKey() + " "
    //                                 + Integer.toString(config.networkId)
    //                                 + " num failures " + config.numIpConfigFailures);
    //                     }
    //
    //                     // Also blacklist the BSSId if we find it
    //                     ScanResult result = null;
    //                     String bssidDbg = "";
    //                     if (config.scanResultCache != null && BSSID != null) {
    //                         result = config.scanResultCache.get(BSSID);
    //                     }
    //                     if (result != null) {
    //                         result.numIpConfigFailures ++;
    //                         bssidDbg = BSSID + " ipfail=" + result.numIpConfigFailures;
    //                         if (result.numIpConfigFailures > 3) {
    //                             // Tell supplicant to stop trying this BSSID
    //                             mWifiNative.addToBlacklist(BSSID);
    //                             result.setAutoJoinStatus(ScanResult.AUTO_JOIN_DISABLED);
    //                         }
    //                     }
    //
    //                     if (DBG) {
    //                         loge("blacklisted " + config.configKey() + " to "
    //                                 + config.autoJoinStatus
    //                                 + " due to IP config failures, count="
    //                                 + config.numIpConfigFailures
    //                                 + " disableReason=" + config.disableReason
    //                                 + " " + bssidDbg);
    //                     }
    //                 } else if (message.contains("CONN_FAILED")) {
    //                     config.numConnectionFailures++;
    //                     if (config.numConnectionFailures > maxConnectionErrorsToBlacklist) {
    //                         config.setAutoJoinStatus
    //                                 (WifiConfiguration.AUTO_JOIN_DISABLED_ON_AUTH_FAILURE);
    //                         disableNetwork(netId,
    //                                 WifiConfiguration.DISABLED_ASSOCIATION_REJECT);
    //                         loge("Connection failure, blacklist " + config.configKey() + " "
    //                                 + config.networkId
    //                                 + " num failures " + config.numConnectionFailures);
    //                     }
    //                 }
    //                 message.replace("\n", "");
    //                 message.replace("\r", "");
    //                 config.lastFailure = message;
    //             }
    //         }
    //     }
    // }
    assert(0);
    return NOERROR;
}

ECode WifiConfigStore::InstallKeys(
    /* [in] */ IWifiEnterpriseConfig* config,
    /* [in] */ const String& name,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(config);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // boolean ret = true;
    // String privKeyName = Credentials.USER_PRIVATE_KEY + name;
    // String userCertName = Credentials.USER_CERTIFICATE + name;
    // String caCertName = Credentials.CA_CERTIFICATE + name;
    // if (config.getClientCertificate() != null) {
    //     byte[] privKeyData = config.getClientPrivateKey().getEncoded();
    //     if (isHardwareBackedKey(config.getClientPrivateKey())) {
    //         // Hardware backed key store is secure enough to store keys un-encrypted, this
    //         // removes the need for user to punch a PIN to get access to these keys
    //         if (DBG) Log.d(TAG, "importing keys " + name + " in hardware backed store");
    //         ret = mKeyStore.importKey(privKeyName, privKeyData, android.os.Process.WIFI_UID,
    //                 KeyStore.FLAG_NONE);
    //     } else {
    //         // Software backed key store is NOT secure enough to store keys un-encrypted.
    //         // Save keys encrypted so they are protected with user's PIN. User will
    //         // have to unlock phone before being able to use these keys and connect to
    //         // networks.
    //         if (DBG) Log.d(TAG, "importing keys " + name + " in software backed store");
    //         ret = mKeyStore.importKey(privKeyName, privKeyData, Process.WIFI_UID,
    //                 KeyStore.FLAG_ENCRYPTED);
    //     }
    //     if (ret == false) {
    //         return ret;
    //     }
    //
    //     ret = putCertInKeyStore(userCertName, config.getClientCertificate());
    //     if (ret == false) {
    //         // Remove private key installed
    //         mKeyStore.delKey(privKeyName, Process.WIFI_UID);
    //         return ret;
    //     }
    // }
    //
    // if (config.getCaCertificate() != null) {
    //     ret = putCertInKeyStore(caCertName, config.getCaCertificate());
    //     if (ret == false) {
    //         if (config.getClientCertificate() != null) {
    //             // Remove client key+cert
    //             mKeyStore.delKey(privKeyName, Process.WIFI_UID);
    //             mKeyStore.delete(userCertName, Process.WIFI_UID);
    //         }
    //         return ret;
    //     }
    // }
    //
    // // Set alias names
    // if (config.getClientCertificate() != null) {
    //     config.setClientCertificateAlias(name);
    //     config.resetClientKeyEntry();
    // }
    //
    // if (config.getCaCertificate() != null) {
    //     config.setCaCertificateAlias(name);
    //     config.resetCaCertificate();
    // }
    //
    // return ret;
    assert(0);
    return NOERROR;
}

ECode WifiConfigStore::RemoveKeys(
    /* [in] */ IWifiEnterpriseConfig* config)
{
    VALIDATE_NOT_NULL(config);
    // ==================before translated======================
    // String client = config.getClientCertificateAlias();
    // // a valid client certificate is configured
    // if (!TextUtils.isEmpty(client)) {
    //     if (DBG) Log.d(TAG, "removing client private key and user cert");
    //     mKeyStore.delKey(Credentials.USER_PRIVATE_KEY + client, Process.WIFI_UID);
    //     mKeyStore.delete(Credentials.USER_CERTIFICATE + client, Process.WIFI_UID);
    // }
    //
    // String ca = config.getCaCertificateAlias();
    // // a valid ca certificate is configured
    // if (!TextUtils.isEmpty(ca)) {
    //     if (DBG) Log.d(TAG, "removing CA cert");
    //     mKeyStore.delete(Credentials.CA_CERTIFICATE + ca, Process.WIFI_UID);
    // }
    assert(0);
    return NOERROR;
}

ECode WifiConfigStore::MigrateOldEapTlsNative(
    /* [in] */ IWifiEnterpriseConfig* config,
    /* [in] */ Int32 netId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(config);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // String oldPrivateKey = mWifiNative.getNetworkVariable(netId, OLD_PRIVATE_KEY_NAME);
    // /*
    //  * If the old configuration value is not present, then there is nothing
    //  * to do.
    //  */
    // if (TextUtils.isEmpty(oldPrivateKey)) {
    //     return false;
    // } else {
    //     // Also ignore it if it's empty quotes.
    //     oldPrivateKey = removeDoubleQuotes(oldPrivateKey);
    //     if (TextUtils.isEmpty(oldPrivateKey)) {
    //         return false;
    //     }
    // }
    //
    // config.setFieldValue(WifiEnterpriseConfig.ENGINE_KEY, WifiEnterpriseConfig.ENGINE_ENABLE);
    // config.setFieldValue(WifiEnterpriseConfig.ENGINE_ID_KEY,
    //         WifiEnterpriseConfig.ENGINE_ID_KEYSTORE);
    //
    // /*
    // * The old key started with the keystore:// URI prefix, but we don't
    // * need that anymore. Trim it off if it exists.
    // */
    // final String keyName;
    // if (oldPrivateKey.startsWith(WifiEnterpriseConfig.KEYSTORE_URI)) {
    //     keyName = new String(
    //             oldPrivateKey.substring(WifiEnterpriseConfig.KEYSTORE_URI.length()));
    // } else {
    //     keyName = oldPrivateKey;
    // }
    // config.setFieldValue(WifiEnterpriseConfig.PRIVATE_KEY_ID_KEY, keyName);
    //
    // mWifiNative.setNetworkVariable(netId, WifiEnterpriseConfig.ENGINE_KEY,
    //         config.getFieldValue(WifiEnterpriseConfig.ENGINE_KEY, ""));
    //
    // mWifiNative.setNetworkVariable(netId, WifiEnterpriseConfig.ENGINE_ID_KEY,
    //         config.getFieldValue(WifiEnterpriseConfig.ENGINE_ID_KEY, ""));
    //
    // mWifiNative.setNetworkVariable(netId, WifiEnterpriseConfig.PRIVATE_KEY_ID_KEY,
    //         config.getFieldValue(WifiEnterpriseConfig.PRIVATE_KEY_ID_KEY, ""));
    //
    // // Remove old private_key string so we don't run this again.
    // mWifiNative.setNetworkVariable(netId, OLD_PRIVATE_KEY_NAME, EMPTY_VALUE);
    //
    // return true;
    assert(0);
    return NOERROR;
}

ECode WifiConfigStore::MigrateCerts(
    /* [in] */ IWifiEnterpriseConfig* config)
{
    VALIDATE_NOT_NULL(config);
    // ==================before translated======================
    // String client = config.getClientCertificateAlias();
    // // a valid client certificate is configured
    // if (!TextUtils.isEmpty(client)) {
    //     if (!mKeyStore.contains(Credentials.USER_PRIVATE_KEY + client, Process.WIFI_UID)) {
    //         mKeyStore.duplicate(Credentials.USER_PRIVATE_KEY + client, -1,
    //                 Credentials.USER_PRIVATE_KEY + client, Process.WIFI_UID);
    //         mKeyStore.duplicate(Credentials.USER_CERTIFICATE + client, -1,
    //                 Credentials.USER_CERTIFICATE + client, Process.WIFI_UID);
    //     }
    // }
    //
    // String ca = config.getCaCertificateAlias();
    // // a valid ca certificate is configured
    // if (!TextUtils.isEmpty(ca)) {
    //     if (!mKeyStore.contains(Credentials.CA_CERTIFICATE + ca, Process.WIFI_UID)) {
    //         mKeyStore.duplicate(Credentials.CA_CERTIFICATE + ca, -1,
    //                 Credentials.CA_CERTIFICATE + ca, Process.WIFI_UID);
    //     }
    // }
    assert(0);
    return NOERROR;
}

void WifiConfigStore::Loge(
    /* [in] */ const String& s)
{
    // ==================before translated======================
    // loge(s, false);
    assert(0);
}

void WifiConfigStore::Loge(
    /* [in] */ const String& s,
    /* [in] */ Boolean stack)
{
    // ==================before translated======================
    // if (stack) {
    //     Log.e(TAG, s + " stack:" + Thread.currentThread().getStackTrace()[2].getMethodName()
    //             + " - " + Thread.currentThread().getStackTrace()[3].getMethodName()
    //             + " - " + Thread.currentThread().getStackTrace()[4].getMethodName()
    //             + " - " + Thread.currentThread().getStackTrace()[5].getMethodName());
    // } else {
    //     Log.e(TAG, s);
    // }
    assert(0);
}

void WifiConfigStore::Log(
    /* [in] */ const String& s)
{
    // ==================before translated======================
    // Log.d(TAG, s);
    assert(0);
}

AutoPtr<ArrayOf<String> > WifiConfigStore::MiddleInitEnterpriseConfigSupplicantKeys()
{
    AutoPtr<ArrayOf<String> > result = ArrayOf<String>::Alloc(11);
    result->Set(0, IWifiEnterpriseConfig::EAP_KEY);
    result->Set(1, IWifiEnterpriseConfig::PHASE2_KEY);
    result->Set(2, IWifiEnterpriseConfig::IDENTITY_KEY);
    result->Set(3, IWifiEnterpriseConfig::ANON_IDENTITY_KEY);
    result->Set(4, IWifiEnterpriseConfig::PASSWORD_KEY);
    result->Set(5, IWifiEnterpriseConfig::CLIENT_CERT_KEY);
    result->Set(6, IWifiEnterpriseConfig::CA_CERT_KEY);
    result->Set(7, IWifiEnterpriseConfig::SUBJECT_MATCH_KEY);
    result->Set(8, IWifiEnterpriseConfig::ENGINE_KEY);
    result->Set(9, IWifiEnterpriseConfig::ENGINE_ID_KEY);
    result->Set(10, IWifiEnterpriseConfig::PRIVATE_KEY_ID_KEY);

    return result;
}

//IWifiConfiguration
AutoPtr<IList> WifiConfigStore::GetConfiguredNetworks(
    /* [in] */ IMap* pskMap)//String, String
{
    // ==================before translated======================
    // List<WifiConfiguration> networks = new ArrayList<>();
    // for(WifiConfiguration config : mConfiguredNetworks.values()) {
    //     WifiConfiguration newConfig = new WifiConfiguration(config);
    //     if (config.autoJoinStatus == WifiConfiguration.AUTO_JOIN_DELETED) {
    //         // Do not enumerate and return this configuration to any one,
    //         // for instance WiFi Picker.
    //         // instead treat it as unknown. the configuration can still be retrieved
    //         // directly by the key or networkId
    //         continue;
    //     }
    //
    //     if (pskMap != null && config.allowedKeyManagement != null
    //             && config.allowedKeyManagement.get(WifiConfiguration.KeyMgmt.WPA_PSK)
    //             && pskMap.containsKey(config.SSID)) {
    //         newConfig.preSharedKey = pskMap.get(config.SSID);
    //     }
    //     networks.add(newConfig);
    // }
    // return networks;
    assert(0);
    AutoPtr<IList> empty;//IWifiConfiguration
    return empty;
}

//String, String
AutoPtr<IMap> WifiConfigStore::GetCredentialsBySsidMap()
{
    // ==================before translated======================
    // return readNetworkVariablesFromSupplicantFile("psk");
    assert(0);
    AutoPtr<IMap> empty;//String,String
    return empty;
}

Boolean WifiConfigStore::RemoveConfigAndSendBroadcastIfNeeded(
    /* [in] */ Int32 netId)
{
    // ==================before translated======================
    // boolean remove = true;
    // WifiConfiguration config = mConfiguredNetworks.get(netId);
    // if (config != null) {
    //     if (VDBG) {
    //         loge("removeNetwork " + Integer.toString(netId) + " key=" +
    //                 config.configKey() + " config.id=" + Integer.toString(config.networkId));
    //     }
    //
    //     // cancel the last user choice
    //     if (config.configKey().equals(lastSelectedConfiguration)) {
    //         lastSelectedConfiguration = null;
    //     }
    //
    //     // Remove any associated keys
    //     if (config.enterpriseConfig != null) {
    //         removeKeys(config.enterpriseConfig);
    //     }
    //
    //     if (config.selfAdded || config.linkedConfigurations != null
    //             || config.allowedKeyManagement.get(KeyMgmt.WPA_PSK)) {
    //         remove = false;
    //         loge("removeNetwork " + Integer.toString(netId)
    //                 + " key=" + config.configKey()
    //                 + " config.id=" + Integer.toString(config.networkId)
    //                 + " -> mark as deleted");
    //     }
    //
    //     if (remove) {
    //         mConfiguredNetworks.remove(netId);
    //         mNetworkIds.remove(configKey(config));
    //     } else {
    //         /**
    //          * We can't directly remove the configuration since we could re-add it ourselves,
    //          * and that would look weird to the user.
    //          * Instead mark it as deleted and completely hide it from the rest of the system.
    //          */
    //         config.setAutoJoinStatus(WifiConfiguration.AUTO_JOIN_DELETED);
    //         // Disable
    //         mWifiNative.disableNetwork(config.networkId);
    //         config.status = WifiConfiguration.Status.DISABLED;
    //         // Since we don't delete the configuration, clean it up and loose the history
    //         config.linkedConfigurations = null;
    //         config.scanResultCache = null;
    //         config.connectChoices = null;
    //         config.defaultGwMacAddress = null;
    //         config.setIpConfiguration(new IpConfiguration());
    //         // Loose the PSK
    //         if (!mWifiNative.setNetworkVariable(
    //                 config.networkId,
    //                 WifiConfiguration.pskVarName,
    //                 "\"" + DELETED_CONFIG_PSK + "\"")) {
    //             loge("removeNetwork, failed to clear PSK, nid=" + config.networkId);
    //         }
    //         // Loose the BSSID
    //         config.BSSID = null;
    //         config.autoJoinBSSID = null;
    //         if (!mWifiNative.setNetworkVariable(
    //                 config.networkId,
    //                 WifiConfiguration.bssidVarName,
    //                 "any")) {
    //             loge("removeNetwork, failed to remove BSSID");
    //         }
    //         // Loose the hiddenSSID flag
    //         config.hiddenSSID = false;
    //         if (!mWifiNative.setNetworkVariable(
    //                 config.networkId,
    //                 WifiConfiguration.hiddenSSIDVarName,
    //                 Integer.toString(0))) {
    //             loge("removeNetwork, failed to remove hiddenSSID");
    //         }
    //
    //         mWifiNative.saveConfig();
    //     }
    //
    //     writeIpAndProxyConfigurations();
    //     sendConfiguredNetworksChangedBroadcast(config, WifiManager.CHANGE_REASON_REMOVED);
    //     writeKnownNetworkHistory();
    // }
    // return remove;
    assert(0);
    return FALSE;
}

void WifiConfigStore::SendConfiguredNetworksChangedBroadcast(
    /* [in] */ IWifiConfiguration* network,
    /* [in] */ Int32 reason)
{
    // ==================before translated======================
    // Intent intent = new Intent(WifiManager.CONFIGURED_NETWORKS_CHANGED_ACTION);
    // intent.addFlags(Intent.FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
    // intent.putExtra(WifiManager.EXTRA_MULTIPLE_NETWORKS_CHANGED, false);
    // intent.putExtra(WifiManager.EXTRA_WIFI_CONFIGURATION, network);
    // intent.putExtra(WifiManager.EXTRA_CHANGE_REASON, reason);
    // mContext.sendBroadcastAsUser(intent, UserHandle.ALL);
    assert(0);
}

void WifiConfigStore::SendConfiguredNetworksChangedBroadcast()
{
    // ==================before translated======================
    // Intent intent = new Intent(WifiManager.CONFIGURED_NETWORKS_CHANGED_ACTION);
    // intent.addFlags(Intent.FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
    // intent.putExtra(WifiManager.EXTRA_MULTIPLE_NETWORKS_CHANGED, true);
    // mContext.sendBroadcastAsUser(intent, UserHandle.ALL);
    assert(0);
}

AutoPtr<IMap> WifiConfigStore::ReadNetworkVariablesFromSupplicantFile(
    /* [in] */ const String& key)
{
    // ==================before translated======================
    // Map<String, String> result = new HashMap<>();
    // BufferedReader reader = null;
    // if (VDBG) loge("readNetworkVariablesFromSupplicantFile key=" + key);
    //
    // try {
    //     reader = new BufferedReader(new FileReader(SUPPLICANT_CONFIG_FILE));
    //     boolean found = false;
    //     String networkSsid = null;
    //     String value = null;
    //
    //     for (String line = reader.readLine(); line != null; line = reader.readLine()) {
    //
    //         if (line.matches("[ \\t]*network=\\{")) {
    //             found = true;
    //             networkSsid = null;
    //             value = null;
    //         } else if (line.matches("[ \\t]*\\}")) {
    //             found = false;
    //             networkSsid = null;
    //             value = null;
    //         }
    //
    //         if (found) {
    //             String trimmedLine = line.trim();
    //             if (trimmedLine.startsWith("ssid=")) {
    //                 networkSsid = trimmedLine.substring(5);
    //             } else if (trimmedLine.startsWith(key + "=")) {
    //                 value = trimmedLine.substring(key.length() + 1);
    //             }
    //
    //             if (networkSsid != null && value != null) {
    //                 result.put(networkSsid, value);
    //             }
    //         }
    //     }
    // } catch (FileNotFoundException e) {
    //     if (VDBG) loge("Could not open " + SUPPLICANT_CONFIG_FILE + ", " + e);
    // } catch (IOException e) {
    //     if (VDBG) loge("Could not read " + SUPPLICANT_CONFIG_FILE + ", " + e);
    // } finally {
    //     try {
    //         if (reader != null) {
    //             reader.close();
    //         }
    //     } catch (IOException e) {
    //         // Just ignore the fact that we couldn't close
    //     }
    // }
    //
    // return result;
    assert(0);
    AutoPtr<IMap> empty;
    return empty;
}

String WifiConfigStore::ReadNetworkVariableFromSupplicantFile(
    /* [in] */ const String& ssid,
    /* [in] */ const String& key)
{
    // ==================before translated======================
    // Map<String, String> data = readNetworkVariablesFromSupplicantFile(key);
    // if (VDBG) loge("readNetworkVariableFromSupplicantFile ssid=[" + ssid + "] key=" + key);
    // return data.get(ssid);
    assert(0);
    return String("");
}

void WifiConfigStore::MarkAllNetworksDisabledExcept(
    /* [in] */ Int32 netId)
{
    // ==================before translated======================
    // for(WifiConfiguration config : mConfiguredNetworks.values()) {
    //     if(config != null && config.networkId != netId) {
    //         if (config.status != Status.DISABLED) {
    //             config.status = Status.DISABLED;
    //             config.disableReason = WifiConfiguration.DISABLED_UNKNOWN_REASON;
    //         }
    //     }
    // }
    assert(0);
}

void WifiConfigStore::MarkAllNetworksDisabled()
{
    // ==================before translated======================
    // markAllNetworksDisabledExcept(INVALID_NETWORK_ID);
    assert(0);
}

void WifiConfigStore::ReadNetworkHistory()
{
    // ==================before translated======================
    // if (showNetworks) {
    //     localLog("readNetworkHistory() path:" + networkHistoryConfigFile);
    // }
    // DataInputStream in = null;
    // try {
    //     in = new DataInputStream(new BufferedInputStream(new FileInputStream(
    //             networkHistoryConfigFile)));
    //     WifiConfiguration config = null;
    //     while (true) {
    //         int id = -1;
    //         String key = in.readUTF();
    //         String bssid = null;
    //         String ssid = null;
    //
    //         int freq = 0;
    //         int status = 0;
    //         long seen = 0;
    //         int rssi = WifiConfiguration.INVALID_RSSI;
    //         String caps = null;
    //         if (key.startsWith(CONFIG_KEY)) {
    //
    //             if (config != null) {
    //                 config = null;
    //             }
    //             String configKey = key.replace(CONFIG_KEY, "");
    //             configKey = configKey.replace(SEPARATOR_KEY, "");
    //             // get the networkId for that config Key
    //             Integer n = mNetworkIds.get(configKey.hashCode());
    //             // skip reading that configuration data
    //             // since we don't have a corresponding network ID
    //             if (n == null) {
    //                 localLog("readNetworkHistory didnt find netid for hash="
    //                         + Integer.toString(configKey.hashCode())
    //                         + " key: " + configKey);
    //                 continue;
    //             }
    //             config = mConfiguredNetworks.get(n);
    //             if (config == null) {
    //                 localLog("readNetworkHistory didnt find config for netid="
    //                         + n.toString()
    //                         + " key: " + configKey);
    //             }
    //             status = 0;
    //             ssid = null;
    //             bssid = null;
    //             freq = 0;
    //             seen = 0;
    //             rssi = WifiConfiguration.INVALID_RSSI;
    //             caps = null;
    //
    //         } else if (config != null && config.duplicateNetwork == false) {
    //             if (key.startsWith(SSID_KEY)) {
    //                 ssid = key.replace(SSID_KEY, "");
    //                 ssid = ssid.replace(SEPARATOR_KEY, "");
    //                 if (config.SSID != null && !config.SSID.equals(ssid)) {
    //                     loge("Error parsing network history file, mismatched SSIDs");
    //                     config = null; //error
    //                     ssid = null;
    //                 } else {
    //                     config.SSID = ssid;
    //                 }
    //             }
    //
    //             if (key.startsWith(FQDN_KEY)) {
    //                 String fqdn = key.replace(FQDN_KEY, "");
    //                 fqdn = fqdn.replace(SEPARATOR_KEY, "");
    //                 config.FQDN = fqdn;
    //             }
    //
    //             if (key.startsWith(DEFAULT_GW_KEY)) {
    //                 String gateway = key.replace(DEFAULT_GW_KEY, "");
    //                 gateway = gateway.replace(SEPARATOR_KEY, "");
    //                 config.defaultGwMacAddress = gateway;
    //             }
    //
    //             if (key.startsWith(STATUS_KEY)) {
    //                 String st = key.replace(STATUS_KEY, "");
    //                 st = st.replace(SEPARATOR_KEY, "");
    //                 config.autoJoinStatus = Integer.parseInt(st);
    //             }
    //
    //             if (key.startsWith(SUPPLICANT_DISABLE_REASON_KEY)) {
    //                 String reason = key.replace(SUPPLICANT_DISABLE_REASON_KEY, "");
    //                 reason = reason.replace(SEPARATOR_KEY, "");
    //                 config.disableReason = Integer.parseInt(reason);
    //             }
    //
    //             if (key.startsWith(SELF_ADDED_KEY)) {
    //                 String selfAdded = key.replace(SELF_ADDED_KEY, "");
    //                 selfAdded = selfAdded.replace(SEPARATOR_KEY, "");
    //                 config.selfAdded = Boolean.parseBoolean(selfAdded);
    //             }
    //
    //             if (key.startsWith(DID_SELF_ADD_KEY)) {
    //                 String didSelfAdd = key.replace(DID_SELF_ADD_KEY, "");
    //                 didSelfAdd = didSelfAdd.replace(SEPARATOR_KEY, "");
    //                 config.didSelfAdd = Boolean.parseBoolean(didSelfAdd);
    //             }
    //
    //             if (key.startsWith(NO_INTERNET_ACCESS_KEY)) {
    //                 String access = key.replace(NO_INTERNET_ACCESS_KEY, "");
    //                 access = access.replace(SEPARATOR_KEY, "");
    //                 config.noInternetAccess = Boolean.parseBoolean(access);
    //             }
    //
    //             if (key.startsWith(EPHEMERAL_KEY)) {
    //                 String access = key.replace(EPHEMERAL_KEY, "");
    //                 access = access.replace(SEPARATOR_KEY, "");
    //                 config.ephemeral = Boolean.parseBoolean(access);
    //             }
    //
    //             if (key.startsWith(CREATOR_UID_KEY)) {
    //                 String uid = key.replace(CREATOR_UID_KEY, "");
    //                 uid = uid.replace(SEPARATOR_KEY, "");
    //                 config.creatorUid = Integer.parseInt(uid);
    //             }
    //
    //             if (key.startsWith(BLACKLIST_MILLI_KEY)) {
    //                 String milli = key.replace(BLACKLIST_MILLI_KEY, "");
    //                 milli = milli.replace(SEPARATOR_KEY, "");
    //                 config.blackListTimestamp = Long.parseLong(milli);
    //             }
    //
    //             if (key.startsWith(NUM_CONNECTION_FAILURES_KEY)) {
    //                 String num = key.replace(NUM_CONNECTION_FAILURES_KEY, "");
    //                 num = num.replace(SEPARATOR_KEY, "");
    //                 config.numConnectionFailures = Integer.parseInt(num);
    //             }
    //
    //             if (key.startsWith(NUM_IP_CONFIG_FAILURES_KEY)) {
    //                 String num = key.replace(NUM_IP_CONFIG_FAILURES_KEY, "");
    //                 num = num.replace(SEPARATOR_KEY, "");
    //                 config.numIpConfigFailures = Integer.parseInt(num);
    //             }
    //
    //             if (key.startsWith(NUM_AUTH_FAILURES_KEY)) {
    //                 String num = key.replace(NUM_AUTH_FAILURES_KEY, "");
    //                 num = num.replace(SEPARATOR_KEY, "");
    //                 config.numIpConfigFailures = Integer.parseInt(num);
    //             }
    //
    //             if (key.startsWith(SCORER_OVERRIDE_KEY)) {
    //                 String num = key.replace(SCORER_OVERRIDE_KEY, "");
    //                 num = num.replace(SEPARATOR_KEY, "");
    //                 config.numScorerOverride = Integer.parseInt(num);
    //             }
    //
    //             if (key.startsWith(SCORER_OVERRIDE_AND_SWITCH_KEY)) {
    //                 String num = key.replace(SCORER_OVERRIDE_AND_SWITCH_KEY, "");
    //                 num = num.replace(SEPARATOR_KEY, "");
    //                 config.numScorerOverrideAndSwitchedNetwork = Integer.parseInt(num);
    //             }
    //
    //             if (key.startsWith(NUM_ASSOCIATION_KEY)) {
    //                 String num = key.replace(NUM_ASSOCIATION_KEY, "");
    //                 num = num.replace(SEPARATOR_KEY, "");
    //                 config.numAssociation = Integer.parseInt(num);
    //             }
    //
    //             if (key.startsWith(JOIN_ATTEMPT_BOOST_KEY)) {
    //                 String num = key.replace(JOIN_ATTEMPT_BOOST_KEY, "");
    //                 num = num.replace(SEPARATOR_KEY, "");
    //                 config.autoJoinUseAggressiveJoinAttemptThreshold = Integer.parseInt(num);
    //             }
    //
    //             if (key.startsWith(CONNECT_UID_KEY)) {
    //                 String uid = key.replace(CONNECT_UID_KEY, "");
    //                 uid = uid.replace(SEPARATOR_KEY, "");
    //                 config.lastConnectUid = Integer.parseInt(uid);
    //             }
    //
    //             if (key.startsWith(UPDATE_UID_KEY)) {
    //                 String uid = key.replace(UPDATE_UID_KEY, "");
    //                 uid = uid.replace(SEPARATOR_KEY, "");
    //                 config.lastUpdateUid = Integer.parseInt(uid);
    //             }
    //
    //             if (key.startsWith(FAILURE_KEY)) {
    //                 config.lastFailure = key.replace(FAILURE_KEY, "");
    //                 config.lastFailure = config.lastFailure.replace(SEPARATOR_KEY, "");
    //             }
    //
    //             if (key.startsWith(PEER_CONFIGURATION_KEY)) {
    //                 config.peerWifiConfiguration = key.replace(PEER_CONFIGURATION_KEY, "");
    //                 config.peerWifiConfiguration =
    //                         config.peerWifiConfiguration.replace(SEPARATOR_KEY, "");
    //             }
    //
    //             if (key.startsWith(CHOICE_KEY)) {
    //                 String choiceStr = key.replace(CHOICE_KEY, "");
    //                 choiceStr = choiceStr.replace(SEPARATOR_KEY, "");
    //                 String configKey = "";
    //                 int choice = 0;
    //                 Matcher match = mConnectChoice.matcher(choiceStr);
    //                 if (!match.find()) {
    //                     if (DBG) Log.d(TAG, "WifiConfigStore: connectChoice: " +
    //                             " Couldnt match pattern : " + choiceStr);
    //                 } else {
    //                     configKey = match.group(1);
    //                     try {
    //                         choice = Integer.parseInt(match.group(2));
    //                     } catch (NumberFormatException e) {
    //                         choice = 0;
    //                     }
    //                     if (choice > 0) {
    //                         if (config.connectChoices == null) {
    //                             config.connectChoices = new HashMap<String, Integer>();
    //                         }
    //                         config.connectChoices.put(configKey, choice);
    //                     }
    //                 }
    //             }
    //
    //             if (key.startsWith(LINK_KEY)) {
    //                 String configKey = key.replace(LINK_KEY, "");
    //                 configKey = configKey.replace(SEPARATOR_KEY, "");
    //                 if (config.linkedConfigurations == null) {
    //                     config.linkedConfigurations = new HashMap<String, Integer>();
    //                 }
    //                 if (config.linkedConfigurations != null) {
    //                     config.linkedConfigurations.put(configKey, -1);
    //                 }
    //             }
    //
    //             if (key.startsWith(BSSID_KEY)) {
    //                 if (key.startsWith(BSSID_KEY)) {
    //                     bssid = key.replace(BSSID_KEY, "");
    //                     bssid = bssid.replace(SEPARATOR_KEY, "");
    //                     freq = 0;
    //                     seen = 0;
    //                     rssi = WifiConfiguration.INVALID_RSSI;
    //                     caps = "";
    //                     status = 0;
    //                 }
    //
    //                 if (key.startsWith(RSSI_KEY)) {
    //                     String lvl = key.replace(RSSI_KEY, "");
    //                     lvl = lvl.replace(SEPARATOR_KEY, "");
    //                     rssi = Integer.parseInt(lvl);
    //                 }
    //
    //                 if (key.startsWith(BSSID_STATUS_KEY)) {
    //                     String st = key.replace(BSSID_STATUS_KEY, "");
    //                     st = st.replace(SEPARATOR_KEY, "");
    //                     status = Integer.parseInt(st);
    //                 }
    //
    //                 if (key.startsWith(FREQ_KEY)) {
    //                     String channel = key.replace(FREQ_KEY, "");
    //                     channel = channel.replace(SEPARATOR_KEY, "");
    //                     freq = Integer.parseInt(channel);
    //                 }
    //
    //                 if (key.startsWith(DATE_KEY)) {
    //                 /*
    //                  * when reading the configuration from file we don't update the date
    //                  * so as to avoid reading back stale or non-sensical data that would
    //                  * depend on network time.
    //                  * The date of a WifiConfiguration should only come from actual scan result.
    //                  *
    //                 String s = key.replace(FREQ_KEY, "");
    //                 seen = Integer.getInteger(s);
    //                 */
    //                 }
    //
    //                 if (key.startsWith(BSSID_KEY_END)) {
    //
    //                     if ((bssid != null) && (ssid != null)) {
    //
    //                         if (config.scanResultCache == null) {
    //                             config.scanResultCache = new HashMap<String, ScanResult>();
    //                         }
    //                         WifiSsid wssid = WifiSsid.createFromAsciiEncoded(ssid);
    //                         ScanResult result = new ScanResult(wssid, bssid,
    //                                 caps, rssi, freq, (long) 0);
    //                         result.seen = seen;
    //                         config.scanResultCache.put(bssid, result);
    //                         result.autoJoinStatus = status;
    //                     }
    //                 }
    //             }
    //         }
    //     }
    // } catch (EOFException ignore) {
    //     if (in != null) {
    //         try {
    //             in.close();
    //         } catch (Exception e) {
    //             loge("readNetworkHistory: Error reading file" + e);
    //         }
    //     }
    // } catch (IOException e) {
    //     loge("readNetworkHistory: No config file, revert to default" + e);
    // }
    //
    // if(in!=null) {
    //     try {
    //         in.close();
    //     } catch (Exception e) {
    //         loge("readNetworkHistory: Error closing file" + e);
    //     }
    // }
    assert(0);
}

void WifiConfigStore::ReadAutoJoinConfig()
{
    // ==================before translated======================
    // BufferedReader reader = null;
    // try {
    //
    //     reader = new BufferedReader(new FileReader(autoJoinConfigFile));
    //
    //     for (String key = reader.readLine(); key != null; key = reader.readLine()) {
    //         if (key != null) {
    //             Log.d(TAG, "readAutoJoinConfig line: " + key);
    //         }
    //         if (key.startsWith(ENABLE_AUTO_JOIN_WHILE_ASSOCIATED_KEY)) {
    //             String st = key.replace(ENABLE_AUTO_JOIN_WHILE_ASSOCIATED_KEY, "");
    //             st = st.replace(SEPARATOR_KEY, "");
    //             try {
    //                 enableAutoJoinWhenAssociated = Integer.parseInt(st) != 0;
    //                 Log.d(TAG,"readAutoJoinConfig: enabled = " + enableAutoJoinWhenAssociated);
    //             } catch (NumberFormatException e) {
    //                 Log.d(TAG,"readAutoJoinConfig: incorrect format :" + key);
    //             }
    //         }
    //
    //         if (key.startsWith(ENABLE_FULL_BAND_SCAN_WHEN_ASSOCIATED_KEY)) {
    //             String st = key.replace(ENABLE_FULL_BAND_SCAN_WHEN_ASSOCIATED_KEY, "");
    //             st = st.replace(SEPARATOR_KEY, "");
    //             try {
    //                 enableFullBandScanWhenAssociated = Integer.parseInt(st) != 0;
    //                 Log.d(TAG,"readAutoJoinConfig: enableFullBandScanWhenAssociated = "
    //                         + enableFullBandScanWhenAssociated);
    //             } catch (NumberFormatException e) {
    //                 Log.d(TAG,"readAutoJoinConfig: incorrect format :" + key);
    //             }
    //         }
    //
    //         if (key.startsWith(ENABLE_AUTO_JOIN_SCAN_WHILE_ASSOCIATED_KEY)) {
    //             String st = key.replace(ENABLE_AUTO_JOIN_SCAN_WHILE_ASSOCIATED_KEY, "");
    //             st = st.replace(SEPARATOR_KEY, "");
    //             try {
    //                 enableAutoJoinScanWhenAssociated = Integer.parseInt(st) != 0;
    //                 Log.d(TAG,"readAutoJoinConfig: enabled = "
    //                         + enableAutoJoinScanWhenAssociated);
    //             } catch (NumberFormatException e) {
    //                 Log.d(TAG,"readAutoJoinConfig: incorrect format :" + key);
    //             }
    //         }
    //
    //         if (key.startsWith(ENABLE_CHIP_WAKE_UP_WHILE_ASSOCIATED_KEY)) {
    //             String st = key.replace(ENABLE_CHIP_WAKE_UP_WHILE_ASSOCIATED_KEY, "");
    //             st = st.replace(SEPARATOR_KEY, "");
    //             try {
    //                 enableChipWakeUpWhenAssociated = Integer.parseInt(st) != 0;
    //                 Log.d(TAG,"readAutoJoinConfig: enabled = "
    //                         + enableChipWakeUpWhenAssociated);
    //             } catch (NumberFormatException e) {
    //                 Log.d(TAG,"readAutoJoinConfig: incorrect format :" + key);
    //             }
    //         }
    //
    //         if (key.startsWith(ENABLE_RSSI_POLL_WHILE_ASSOCIATED_KEY)) {
    //             String st = key.replace(ENABLE_RSSI_POLL_WHILE_ASSOCIATED_KEY, "");
    //             st = st.replace(SEPARATOR_KEY, "");
    //             try {
    //                 enableRssiPollWhenAssociated = Integer.parseInt(st) != 0;
    //                 Log.d(TAG,"readAutoJoinConfig: enabled = "
    //                         + enableRssiPollWhenAssociated);
    //             } catch (NumberFormatException e) {
    //                 Log.d(TAG,"readAutoJoinConfig: incorrect format :" + key);
    //             }
    //         }
    //
    //         if (key.startsWith(THRESHOLD_INITIAL_AUTO_JOIN_ATTEMPT_RSSI_MIN_5G_KEY)) {
    //             String st =
    //                     key.replace(THRESHOLD_INITIAL_AUTO_JOIN_ATTEMPT_RSSI_MIN_5G_KEY, "");
    //             st = st.replace(SEPARATOR_KEY, "");
    //             try {
    //                 thresholdInitialAutoJoinAttemptMin5RSSI = Integer.parseInt(st);
    //                 Log.d(TAG,"readAutoJoinConfig: thresholdInitialAutoJoinAttemptMin5RSSI = "
    //                         + Integer.toString(thresholdInitialAutoJoinAttemptMin5RSSI));
    //             } catch (NumberFormatException e) {
    //                 Log.d(TAG,"readAutoJoinConfig: incorrect format :" + key);
    //             }
    //         }
    //
    //         if (key.startsWith(THRESHOLD_INITIAL_AUTO_JOIN_ATTEMPT_RSSI_MIN_24G_KEY)) {
    //             String st =
    //                     key.replace(THRESHOLD_INITIAL_AUTO_JOIN_ATTEMPT_RSSI_MIN_24G_KEY, "");
    //             st = st.replace(SEPARATOR_KEY, "");
    //             try {
    //                 thresholdInitialAutoJoinAttemptMin24RSSI = Integer.parseInt(st);
    //                 Log.d(TAG,"readAutoJoinConfig: thresholdInitialAutoJoinAttemptMin24RSSI = "
    //                         + Integer.toString(thresholdInitialAutoJoinAttemptMin24RSSI));
    //             } catch (NumberFormatException e) {
    //                 Log.d(TAG,"readAutoJoinConfig: incorrect format :" + key);
    //             }
    //         }
    //
    //         if (key.startsWith(THRESHOLD_UNBLACKLIST_HARD_5G_KEY)) {
    //             String st = key.replace(THRESHOLD_UNBLACKLIST_HARD_5G_KEY, "");
    //             st = st.replace(SEPARATOR_KEY, "");
    //             try {
    //                 thresholdUnblacklistThreshold5Hard = Integer.parseInt(st);
    //                 Log.d(TAG,"readAutoJoinConfig: thresholdUnblacklistThreshold5Hard = "
    //                     + Integer.toString(thresholdUnblacklistThreshold5Hard));
    //             } catch (NumberFormatException e) {
    //                 Log.d(TAG,"readAutoJoinConfig: incorrect format :" + key);
    //             }
    //         }
    //         if (key.startsWith(THRESHOLD_UNBLACKLIST_SOFT_5G_KEY)) {
    //             String st = key.replace(THRESHOLD_UNBLACKLIST_SOFT_5G_KEY, "");
    //             st = st.replace(SEPARATOR_KEY, "");
    //             try {
    //                 thresholdUnblacklistThreshold5Soft = Integer.parseInt(st);
    //                 Log.d(TAG,"readAutoJoinConfig: thresholdUnblacklistThreshold5Soft = "
    //                     + Integer.toString(thresholdUnblacklistThreshold5Soft));
    //             } catch (NumberFormatException e) {
    //                 Log.d(TAG,"readAutoJoinConfig: incorrect format :" + key);
    //             }
    //         }
    //         if (key.startsWith(THRESHOLD_UNBLACKLIST_HARD_24G_KEY)) {
    //             String st = key.replace(THRESHOLD_UNBLACKLIST_HARD_24G_KEY, "");
    //             st = st.replace(SEPARATOR_KEY, "");
    //             try {
    //                 thresholdUnblacklistThreshold24Hard = Integer.parseInt(st);
    //                 Log.d(TAG,"readAutoJoinConfig: thresholdUnblacklistThreshold24Hard = "
    //                     + Integer.toString(thresholdUnblacklistThreshold24Hard));
    //             } catch (NumberFormatException e) {
    //                 Log.d(TAG,"readAutoJoinConfig: incorrect format :" + key);
    //             }
    //         }
    //         if (key.startsWith(THRESHOLD_UNBLACKLIST_SOFT_24G_KEY)) {
    //             String st = key.replace(THRESHOLD_UNBLACKLIST_SOFT_24G_KEY, "");
    //             st = st.replace(SEPARATOR_KEY, "");
    //             try {
    //                 thresholdUnblacklistThreshold24Soft = Integer.parseInt(st);
    //                 Log.d(TAG,"readAutoJoinConfig: thresholdUnblacklistThreshold24Soft = "
    //                     + Integer.toString(thresholdUnblacklistThreshold24Soft));
    //             } catch (NumberFormatException e) {
    //                 Log.d(TAG,"readAutoJoinConfig: incorrect format :" + key);
    //             }
    //         }
    //
    //         if (key.startsWith(THRESHOLD_GOOD_RSSI_5_KEY)) {
    //             String st = key.replace(THRESHOLD_GOOD_RSSI_5_KEY, "");
    //             st = st.replace(SEPARATOR_KEY, "");
    //             try {
    //                 thresholdGoodRssi5 = Integer.parseInt(st);
    //                 Log.d(TAG,"readAutoJoinConfig: thresholdGoodRssi5 = "
    //                     + Integer.toString(thresholdGoodRssi5));
    //             } catch (NumberFormatException e) {
    //                 Log.d(TAG,"readAutoJoinConfig: incorrect format :" + key);
    //             }
    //         }
    //         if (key.startsWith(THRESHOLD_LOW_RSSI_5_KEY)) {
    //             String st = key.replace(THRESHOLD_LOW_RSSI_5_KEY, "");
    //             st = st.replace(SEPARATOR_KEY, "");
    //             try {
    //                 thresholdLowRssi5 = Integer.parseInt(st);
    //                 Log.d(TAG,"readAutoJoinConfig: thresholdLowRssi5 = "
    //                     + Integer.toString(thresholdLowRssi5));
    //             } catch (NumberFormatException e) {
    //                 Log.d(TAG,"readAutoJoinConfig: incorrect format :" + key);
    //             }
    //         }
    //         if (key.startsWith(THRESHOLD_BAD_RSSI_5_KEY)) {
    //             String st = key.replace(THRESHOLD_BAD_RSSI_5_KEY, "");
    //             st = st.replace(SEPARATOR_KEY, "");
    //             try {
    //                 thresholdBadRssi5 = Integer.parseInt(st);
    //                 Log.d(TAG,"readAutoJoinConfig: thresholdBadRssi5 = "
    //                     + Integer.toString(thresholdBadRssi5));
    //             } catch (NumberFormatException e) {
    //                 Log.d(TAG,"readAutoJoinConfig: incorrect format :" + key);
    //             }
    //         }
    //
    //         if (key.startsWith(THRESHOLD_GOOD_RSSI_24_KEY)) {
    //             String st = key.replace(THRESHOLD_GOOD_RSSI_24_KEY, "");
    //             st = st.replace(SEPARATOR_KEY, "");
    //             try {
    //                 thresholdGoodRssi24 = Integer.parseInt(st);
    //                 Log.d(TAG,"readAutoJoinConfig: thresholdGoodRssi24 = "
    //                     + Integer.toString(thresholdGoodRssi24));
    //             } catch (NumberFormatException e) {
    //                 Log.d(TAG,"readAutoJoinConfig: incorrect format :" + key);
    //             }
    //         }
    //         if (key.startsWith(THRESHOLD_LOW_RSSI_24_KEY)) {
    //             String st = key.replace(THRESHOLD_LOW_RSSI_24_KEY, "");
    //             st = st.replace(SEPARATOR_KEY, "");
    //             try {
    //                 thresholdLowRssi24 = Integer.parseInt(st);
    //                 Log.d(TAG,"readAutoJoinConfig: thresholdLowRssi24 = "
    //                     + Integer.toString(thresholdLowRssi24));
    //             } catch (NumberFormatException e) {
    //                 Log.d(TAG,"readAutoJoinConfig: incorrect format :" + key);
    //             }
    //         }
    //         if (key.startsWith(THRESHOLD_BAD_RSSI_24_KEY)) {
    //             String st = key.replace(THRESHOLD_BAD_RSSI_24_KEY, "");
    //             st = st.replace(SEPARATOR_KEY, "");
    //             try {
    //                 thresholdBadRssi24 = Integer.parseInt(st);
    //                 Log.d(TAG,"readAutoJoinConfig: thresholdBadRssi24 = "
    //                     + Integer.toString(thresholdBadRssi24));
    //             } catch (NumberFormatException e) {
    //                 Log.d(TAG,"readAutoJoinConfig: incorrect format :" + key);
    //             }
    //         }
    //
    //         if (key.startsWith(THRESHOLD_MAX_TX_PACKETS_FOR_NETWORK_SWITCHING_KEY)) {
    //             String st = key.replace(THRESHOLD_MAX_TX_PACKETS_FOR_NETWORK_SWITCHING_KEY, "");
    //             st = st.replace(SEPARATOR_KEY, "");
    //             try {
    //                 maxTxPacketForNetworkSwitching = Integer.parseInt(st);
    //                 Log.d(TAG,"readAutoJoinConfig: maxTxPacketForNetworkSwitching = "
    //                     + Integer.toString(maxTxPacketForNetworkSwitching));
    //             } catch (NumberFormatException e) {
    //                 Log.d(TAG,"readAutoJoinConfig: incorrect format :" + key);
    //             }
    //         }
    //         if (key.startsWith(THRESHOLD_MAX_RX_PACKETS_FOR_NETWORK_SWITCHING_KEY)) {
    //             String st = key.replace(THRESHOLD_MAX_RX_PACKETS_FOR_NETWORK_SWITCHING_KEY, "");
    //             st = st.replace(SEPARATOR_KEY, "");
    //             try {
    //                 maxRxPacketForNetworkSwitching = Integer.parseInt(st);
    //                 Log.d(TAG,"readAutoJoinConfig: maxRxPacketForNetworkSwitching = "
    //                     + Integer.toString(maxRxPacketForNetworkSwitching));
    //             } catch (NumberFormatException e) {
    //                 Log.d(TAG,"readAutoJoinConfig: incorrect format :" + key);
    //             }
    //         }
    //
    //         if (key.startsWith(THRESHOLD_MAX_TX_PACKETS_FOR_FULL_SCANS_KEY)) {
    //             String st = key.replace(THRESHOLD_MAX_TX_PACKETS_FOR_FULL_SCANS_KEY, "");
    //             st = st.replace(SEPARATOR_KEY, "");
    //             try {
    //                 maxTxPacketForNetworkSwitching = Integer.parseInt(st);
    //                 Log.d(TAG,"readAutoJoinConfig: maxTxPacketForFullScans = "
    //                         + Integer.toString(maxTxPacketForFullScans));
    //             } catch (NumberFormatException e) {
    //                 Log.d(TAG,"readAutoJoinConfig: incorrect format :" + key);
    //             }
    //         }
    //         if (key.startsWith(THRESHOLD_MAX_RX_PACKETS_FOR_FULL_SCANS_KEY)) {
    //             String st = key.replace(THRESHOLD_MAX_RX_PACKETS_FOR_FULL_SCANS_KEY, "");
    //             st = st.replace(SEPARATOR_KEY, "");
    //             try {
    //                 maxRxPacketForNetworkSwitching = Integer.parseInt(st);
    //                 Log.d(TAG,"readAutoJoinConfig: maxRxPacketForFullScans = "
    //                         + Integer.toString(maxRxPacketForFullScans));
    //             } catch (NumberFormatException e) {
    //                 Log.d(TAG,"readAutoJoinConfig: incorrect format :" + key);
    //             }
    //         }
    //
    //         if (key.startsWith(THRESHOLD_MAX_TX_PACKETS_FOR_PARTIAL_SCANS_KEY)) {
    //             String st = key.replace(THRESHOLD_MAX_TX_PACKETS_FOR_PARTIAL_SCANS_KEY, "");
    //             st = st.replace(SEPARATOR_KEY, "");
    //             try {
    //                 maxTxPacketForNetworkSwitching = Integer.parseInt(st);
    //                 Log.d(TAG,"readAutoJoinConfig: maxTxPacketForPartialScans = "
    //                         + Integer.toString(maxTxPacketForPartialScans));
    //             } catch (NumberFormatException e) {
    //                 Log.d(TAG,"readAutoJoinConfig: incorrect format :" + key);
    //             }
    //         }
    //         if (key.startsWith(THRESHOLD_MAX_RX_PACKETS_FOR_PARTIAL_SCANS_KEY)) {
    //             String st = key.replace(THRESHOLD_MAX_RX_PACKETS_FOR_PARTIAL_SCANS_KEY, "");
    //             st = st.replace(SEPARATOR_KEY, "");
    //             try {
    //                 maxRxPacketForNetworkSwitching = Integer.parseInt(st);
    //                 Log.d(TAG,"readAutoJoinConfig: maxRxPacketForPartialScans = "
    //                         + Integer.toString(maxRxPacketForPartialScans));
    //             } catch (NumberFormatException e) {
    //                 Log.d(TAG,"readAutoJoinConfig: incorrect format :" + key);
    //             }
    //         }
    //
    //         if (key.startsWith(WIFI_VERBOSE_LOGS_KEY)) {
    //             String st = key.replace(WIFI_VERBOSE_LOGS_KEY, "");
    //             st = st.replace(SEPARATOR_KEY, "");
    //             try {
    //                 enableVerboseLogging = Integer.parseInt(st);
    //                 Log.d(TAG,"readAutoJoinConfig: enable verbose logs = "
    //                         + Integer.toString(enableVerboseLogging));
    //             } catch (NumberFormatException e) {
    //                 Log.d(TAG,"readAutoJoinConfig: incorrect format :" + key);
    //             }
    //         }
    //         if (key.startsWith(A_BAND_PREFERENCE_RSSI_THRESHOLD_KEY)) {
    //             String st = key.replace(A_BAND_PREFERENCE_RSSI_THRESHOLD_KEY, "");
    //             st = st.replace(SEPARATOR_KEY, "");
    //             try {
    //                 bandPreferenceBoostThreshold5 = Integer.parseInt(st);
    //                 Log.d(TAG,"readAutoJoinConfig: bandPreferenceBoostThreshold5 = "
    //                     + Integer.toString(bandPreferenceBoostThreshold5));
    //             } catch (NumberFormatException e) {
    //                 Log.d(TAG,"readAutoJoinConfig: incorrect format :" + key);
    //             }
    //         }
    //         if (key.startsWith(ASSOCIATED_PARTIAL_SCAN_PERIOD_KEY)) {
    //             String st = key.replace(ASSOCIATED_PARTIAL_SCAN_PERIOD_KEY, "");
    //             st = st.replace(SEPARATOR_KEY, "");
    //             try {
    //                 associatedPartialScanPeriodMilli = Integer.parseInt(st);
    //                 Log.d(TAG,"readAutoJoinConfig: associatedScanPeriod = "
    //                         + Integer.toString(associatedPartialScanPeriodMilli));
    //             } catch (NumberFormatException e) {
    //                 Log.d(TAG,"readAutoJoinConfig: incorrect format :" + key);
    //             }
    //         }
    //         if (key.startsWith(ASSOCIATED_FULL_SCAN_BACKOFF_KEY)) {
    //             String st = key.replace(ASSOCIATED_FULL_SCAN_BACKOFF_KEY, "");
    //             st = st.replace(SEPARATOR_KEY, "");
    //             try {
    //                 associatedFullScanBackoff = Integer.parseInt(st);
    //                 Log.d(TAG,"readAutoJoinConfig: associatedFullScanBackoff = "
    //                         + Integer.toString(associatedFullScanBackoff));
    //             } catch (NumberFormatException e) {
    //                 Log.d(TAG,"readAutoJoinConfig: incorrect format :" + key);
    //             }
    //         }
    //         if (key.startsWith(G_BAND_PREFERENCE_RSSI_THRESHOLD_KEY)) {
    //             String st = key.replace(G_BAND_PREFERENCE_RSSI_THRESHOLD_KEY, "");
    //             st = st.replace(SEPARATOR_KEY, "");
    //             try {
    //                 bandPreferencePenaltyThreshold5 = Integer.parseInt(st);
    //                 Log.d(TAG,"readAutoJoinConfig: bandPreferencePenaltyThreshold5 = "
    //                     + Integer.toString(bandPreferencePenaltyThreshold5));
    //             } catch (NumberFormatException e) {
    //                 Log.d(TAG,"readAutoJoinConfig: incorrect format :" + key);
    //             }
    //         }
    //         if (key.startsWith(ALWAYS_ENABLE_SCAN_WHILE_ASSOCIATED_KEY)) {
    //             String st = key.replace(ALWAYS_ENABLE_SCAN_WHILE_ASSOCIATED_KEY, "");
    //             st = st.replace(SEPARATOR_KEY, "");
    //             try {
    //                 alwaysEnableScansWhileAssociated = Integer.parseInt(st);
    //                 Log.d(TAG,"readAutoJoinConfig: alwaysEnableScansWhileAssociated = "
    //                         + Integer.toString(alwaysEnableScansWhileAssociated));
    //             } catch (NumberFormatException e) {
    //                 Log.d(TAG,"readAutoJoinConfig: incorrect format :" + key);
    //             }
    //         }
    //         if (key.startsWith(MAX_NUM_PASSIVE_CHANNELS_FOR_PARTIAL_SCANS_KEY)) {
    //             String st = key.replace(MAX_NUM_PASSIVE_CHANNELS_FOR_PARTIAL_SCANS_KEY, "");
    //             st = st.replace(SEPARATOR_KEY, "");
    //             try {
    //                 maxNumPassiveChannelsForPartialScans = Integer.parseInt(st);
    //                 Log.d(TAG,"readAutoJoinConfig: maxNumPassiveChannelsForPartialScans = "
    //                         + Integer.toString(maxNumPassiveChannelsForPartialScans));
    //             } catch (NumberFormatException e) {
    //                 Log.d(TAG,"readAutoJoinConfig: incorrect format :" + key);
    //             }
    //         }
    //         if (key.startsWith(MAX_NUM_ACTIVE_CHANNELS_FOR_PARTIAL_SCANS_KEY)) {
    //             String st = key.replace(MAX_NUM_ACTIVE_CHANNELS_FOR_PARTIAL_SCANS_KEY, "");
    //             st = st.replace(SEPARATOR_KEY, "");
    //             try {
    //                 maxNumActiveChannelsForPartialScans = Integer.parseInt(st);
    //                 Log.d(TAG,"readAutoJoinConfig: maxNumActiveChannelsForPartialScans = "
    //                         + Integer.toString(maxNumActiveChannelsForPartialScans));
    //             } catch (NumberFormatException e) {
    //                 Log.d(TAG,"readAutoJoinConfig: incorrect format :" + key);
    //             }
    //         }
    //         if (key.startsWith(
    //             AUTO_JOIN_SCAN_INTERVAL_WHEN_P2P_CONNECTED_KEY)) {
    //             int scanInterval;
    //             String st = key.replace(
    //                         AUTO_JOIN_SCAN_INTERVAL_WHEN_P2P_CONNECTED_KEY,
    //                         "");
    //             st = st.replace(SEPARATOR_KEY, "");
    //             try {
    //                 scanInterval = Integer.parseInt(st);
    //                 if (scanInterval >= 10000) {
    //                     autoJoinScanIntervalWhenP2pConnected = scanInterval;
    //                 } else {
    //                     Log.d(TAG,
    //                           "Cfg value is less then 10sec, Using default="
    //                           + autoJoinScanIntervalWhenP2pConnected);
    //                 }
    //                 Log.d(TAG, "readAutoJoinConfig: " +
    //                       "autoJoinScanIntervalWhenP2pConnected = "
    //                       + Integer.toString(
    //                       autoJoinScanIntervalWhenP2pConnected));
    //             } catch (NumberFormatException e) {
    //                 Log.d(TAG, "readAutoJoinConfig: incorrect format :" +
    //                       key);
    //             }
    //         }
    //     }
    // } catch (EOFException ignore) {
    //     if (reader != null) {
    //         try {
    //             reader.close();
    //             reader = null;
    //         } catch (Exception e) {
    //             loge("readAutoJoinStatus: Error closing file" + e);
    //         }
    //     }
    // } catch (FileNotFoundException ignore) {
    //     if (reader != null) {
    //         try {
    //             reader.close();
    //             reader = null;
    //         } catch (Exception e) {
    //             loge("readAutoJoinStatus: Error closing file" + e);
    //         }
    //     }
    // } catch (IOException e) {
    //     loge("readAutoJoinStatus: Error parsing configuration" + e);
    // }
    //
    // if (reader!=null) {
    //    try {
    //        reader.close();
    //    } catch (Exception e) {
    //        loge("readAutoJoinStatus: Error closing file" + e);
    //    }
    // }
    assert(0);
}

void WifiConfigStore::WriteIpAndProxyConfigurations()
{
    // ==================before translated======================
    // final SparseArray<IpConfiguration> networks = new SparseArray<IpConfiguration>();
    // for(WifiConfiguration config : mConfiguredNetworks.values()) {
    //     if (!config.ephemeral && config.autoJoinStatus != WifiConfiguration.AUTO_JOIN_DELETED) {
    //         networks.put(configKey(config), config.getIpConfiguration());
    //     }
    // }
    //
    // super.writeIpAndProxyConfigurations(ipConfigFile, networks);
    assert(0);
}

void WifiConfigStore::ReadIpAndProxyConfigurations()
{
    // ==================before translated======================
    // SparseArray<IpConfiguration> networks = super.readIpAndProxyConfigurations(ipConfigFile);
    //
    // if (networks.size() == 0) {
    //     // IpConfigStore.readIpAndProxyConfigurations has already logged an error.
    //     return;
    // }
    //
    // for (int i = 0; i < networks.size(); i++) {
    //     int id = networks.keyAt(i);
    //     WifiConfiguration config = mConfiguredNetworks.get(mNetworkIds.get(id));
    //
    //     if (config == null || config.autoJoinStatus == WifiConfiguration.AUTO_JOIN_DELETED) {
    //         loge("configuration found for missing network, nid=" + id
    //                 +", ignored, networks.size=" + Integer.toString(networks.size()));
    //     } else if (config != null && config.duplicateNetwork == true) {
    //         if (VDBG)
    //             loge("Network configuration is not updated for duplicate network id="
    //                   + config.networkId + " SSID=" + config.SSID);
    //     } else {
    //         config.setIpConfiguration(networks.valueAt(i));
    //     }
    // }
    assert(0);
}

String WifiConfigStore::EncodeSSID(
    /* [in] */ const String& str)
{
    // ==================before translated======================
    // String tmp = removeDoubleQuotes(str);
    // return String.format("%x", new BigInteger(1, tmp.getBytes(Charset.forName("UTF-8"))));
    assert(0);
    return String("");
}

AutoPtr<NetworkUpdateResult> WifiConfigStore::AddOrUpdateNetworkNative(
    /* [in] */ IWifiConfiguration* config,
    /* [in] */ Int32 uid)
{
    // ==================before translated======================
    // /*
    //  * If the supplied networkId is INVALID_NETWORK_ID, we create a new empty
    //  * network configuration. Otherwise, the networkId should
    //  * refer to an existing configuration.
    //  */
    //
    // if (VDBG) localLog("addOrUpdateNetworkNative " + config.getPrintableSsid());
    //
    // int netId = config.networkId;
    // boolean newNetwork = false;
    // // networkId of INVALID_NETWORK_ID means we want to create a new network
    // if (netId == INVALID_NETWORK_ID) {
    //     Integer savedNetId = mNetworkIds.get(configKey(config));
    //     // Check if either we have a network Id or a WifiConfiguration
    //     // matching the one we are trying to add.
    //     if (savedNetId == null) {
    //         for (WifiConfiguration test : mConfiguredNetworks.values()) {
    //             if (test.configKey().equals(config.configKey())) {
    //                 savedNetId = test.networkId;
    //                 loge("addOrUpdateNetworkNative " + config.configKey()
    //                         + " was found, but no network Id");
    //                 break;
    //             }
    //         }
    //     }
    //     if (savedNetId != null) {
    //         netId = savedNetId;
    //     } else {
    //         newNetwork = true;
    //         netId = mWifiNative.addNetwork();
    //         if (netId < 0) {
    //             loge("Failed to add a network!");
    //             return new NetworkUpdateResult(INVALID_NETWORK_ID);
    //         } else {
    //             loge("addOrUpdateNetworkNative created netId=" + netId);
    //         }
    //     }
    // }
    //
    // boolean updateFailed = true;
    //
    // setVariables: {
    //
    //     if (config.SSID != null &&
    //             !mWifiNative.setNetworkVariable(
    //                 netId,
    //                 WifiConfiguration.ssidVarName,
    //                 encodeSSID(config.SSID))) {
    //         loge("failed to set SSID: "+config.SSID);
    //         break setVariables;
    //     }
    //
    //     if (config.BSSID != null) {
    //         loge("Setting BSSID for " + config.configKey() + " to " + config.BSSID);
    //         if (!mWifiNative.setNetworkVariable(
    //                 netId,
    //                 WifiConfiguration.bssidVarName,
    //                 config.BSSID)) {
    //             loge("failed to set BSSID: " + config.BSSID);
    //             break setVariables;
    //         }
    //     }
    //
    //     if (config.isIBSS) {
    //         if(!mWifiNative.setNetworkVariable(
    //                 netId,
    //                 WifiConfiguration.modeVarName,
    //                 "1")) {
    //             loge("failed to set adhoc mode");
    //             break setVariables;
    //         }
    //         if(!mWifiNative.setNetworkVariable(
    //                 netId,
    //                 WifiConfiguration.frequencyVarName,
    //                 Integer.toString(config.frequency))) {
    //             loge("failed to set frequency");
    //             break setVariables;
    //         }
    //     }
    //
    //     String allowedKeyManagementString =
    //         makeString(config.allowedKeyManagement, WifiConfiguration.KeyMgmt.strings);
    //     if (config.allowedKeyManagement.cardinality() != 0 &&
    //             !mWifiNative.setNetworkVariable(
    //                 netId,
    //                 WifiConfiguration.KeyMgmt.varName,
    //                 allowedKeyManagementString)) {
    //         loge("failed to set key_mgmt: "+
    //                 allowedKeyManagementString);
    //         break setVariables;
    //     }
    //
    //     String allowedProtocolsString =
    //         makeString(config.allowedProtocols, WifiConfiguration.Protocol.strings);
    //     if (config.allowedProtocols.cardinality() != 0 &&
    //             !mWifiNative.setNetworkVariable(
    //                 netId,
    //                 WifiConfiguration.Protocol.varName,
    //                 allowedProtocolsString)) {
    //         loge("failed to set proto: "+
    //                 allowedProtocolsString);
    //         break setVariables;
    //     }
    //
    //     String allowedAuthAlgorithmsString =
    //         makeString(config.allowedAuthAlgorithms, WifiConfiguration.AuthAlgorithm.strings);
    //     if (config.allowedAuthAlgorithms.cardinality() != 0 &&
    //             !mWifiNative.setNetworkVariable(
    //                 netId,
    //                 WifiConfiguration.AuthAlgorithm.varName,
    //                 allowedAuthAlgorithmsString)) {
    //         loge("failed to set auth_alg: "+
    //                 allowedAuthAlgorithmsString);
    //         break setVariables;
    //     }
    //
    //     String allowedPairwiseCiphersString =
    //             makeString(config.allowedPairwiseCiphers,
    //             WifiConfiguration.PairwiseCipher.strings);
    //     if (config.allowedPairwiseCiphers.cardinality() != 0 &&
    //             !mWifiNative.setNetworkVariable(
    //                 netId,
    //                 WifiConfiguration.PairwiseCipher.varName,
    //                 allowedPairwiseCiphersString)) {
    //         loge("failed to set pairwise: "+
    //                 allowedPairwiseCiphersString);
    //         break setVariables;
    //     }
    //
    //     String allowedGroupCiphersString =
    //         makeString(config.allowedGroupCiphers, WifiConfiguration.GroupCipher.strings);
    //     if (config.allowedGroupCiphers.cardinality() != 0 &&
    //             !mWifiNative.setNetworkVariable(
    //                 netId,
    //                 WifiConfiguration.GroupCipher.varName,
    //                 allowedGroupCiphersString)) {
    //         loge("failed to set group: "+
    //                 allowedGroupCiphersString);
    //         break setVariables;
    //     }
    //
    //     // Prevent client screw-up by passing in a WifiConfiguration we gave it
    //     // by preventing "*" as a key.
    //     if (config.preSharedKey != null && !config.preSharedKey.equals("*") &&
    //             !mWifiNative.setNetworkVariable(
    //                 netId,
    //                 WifiConfiguration.pskVarName,
    //                 config.preSharedKey)) {
    //         loge("failed to set psk");
    //         break setVariables;
    //     }
    //
    //     boolean hasSetKey = false;
    //     if (config.wepKeys != null) {
    //         for (int i = 0; i < config.wepKeys.length; i++) {
    //             // Prevent client screw-up by passing in a WifiConfiguration we gave it
    //             // by preventing "*" as a key.
    //             if (config.wepKeys[i] != null && !config.wepKeys[i].equals("*")) {
    //                 if (!mWifiNative.setNetworkVariable(
    //                             netId,
    //                             WifiConfiguration.wepKeyVarNames[i],
    //                             config.wepKeys[i])) {
    //                     loge("failed to set wep_key" + i + ": " + config.wepKeys[i]);
    //                     break setVariables;
    //                 }
    //                 hasSetKey = true;
    //             }
    //         }
    //     }
    //
    //     if (hasSetKey) {
    //         if (!mWifiNative.setNetworkVariable(
    //                     netId,
    //                     WifiConfiguration.wepTxKeyIdxVarName,
    //                     Integer.toString(config.wepTxKeyIndex))) {
    //             loge("failed to set wep_tx_keyidx: " + config.wepTxKeyIndex);
    //             break setVariables;
    //         }
    //     }
    //
    //     if (!mWifiNative.setNetworkVariable(
    //                 netId,
    //                 WifiConfiguration.priorityVarName,
    //                 Integer.toString(config.priority))) {
    //         loge(config.SSID + ": failed to set priority: "
    //                 +config.priority);
    //         break setVariables;
    //     }
    //
    //     if (config.hiddenSSID && !mWifiNative.setNetworkVariable(
    //                 netId,
    //                 WifiConfiguration.hiddenSSIDVarName,
    //                 Integer.toString(config.hiddenSSID ? 1 : 0))) {
    //         loge(config.SSID + ": failed to set hiddenSSID: "+
    //                 config.hiddenSSID);
    //         break setVariables;
    //     }
    //
    //     if (config.requirePMF && !mWifiNative.setNetworkVariable(
    //                 netId,
    //                 WifiConfiguration.pmfVarName,
    //                 "2")) {
    //         loge(config.SSID + ": failed to set requirePMF: "+
    //                 config.requirePMF);
    //         break setVariables;
    //     }
    //
    //     if (config.updateIdentifier != null && !mWifiNative.setNetworkVariable(
    //             netId,
    //             WifiConfiguration.updateIdentiferVarName,
    //             config.updateIdentifier)) {
    //         loge(config.SSID + ": failed to set updateIdentifier: "+
    //                 config.updateIdentifier);
    //         break setVariables;
    //     }
    //
    //     if (config.enterpriseConfig != null &&
    //             config.enterpriseConfig.getEapMethod() != WifiEnterpriseConfig.Eap.NONE) {
    //
    //         WifiEnterpriseConfig enterpriseConfig = config.enterpriseConfig;
    //
    //         if (needsKeyStore(enterpriseConfig)) {
    //             /**
    //              * Keyguard settings may eventually be controlled by device policy.
    //              * We check here if keystore is unlocked before installing
    //              * credentials.
    //              * TODO: Do we need a dialog here ?
    //              */
    //             if (mKeyStore.state() != KeyStore.State.UNLOCKED) {
    //                 loge(config.SSID + ": key store is locked");
    //                 break setVariables;
    //             }
    //
    //             try {
    //                 /* config passed may include only fields being updated.
    //                  * In order to generate the key id, fetch uninitialized
    //                  * fields from the currently tracked configuration
    //                  */
    //                 WifiConfiguration currentConfig = mConfiguredNetworks.get(netId);
    //                 String keyId = config.getKeyIdForCredentials(currentConfig);
    //
    //                 if (!installKeys(enterpriseConfig, keyId)) {
    //                     loge(config.SSID + ": failed to install keys");
    //                     break setVariables;
    //                 }
    //             } catch (IllegalStateException e) {
    //                 loge(config.SSID + " invalid config for key installation");
    //                 break setVariables;
    //             }
    //         }
    //
    //         HashMap<String, String> enterpriseFields = enterpriseConfig.getFields();
    //         for (String key : enterpriseFields.keySet()) {
    //                 String value = enterpriseFields.get(key);
    //                 if (key.equals("password") && value != null && value.equals("*")) {
    //                     // No need to try to set an obfuscated password, which will fail
    //                     continue;
    //                 }
    //                 if (!mWifiNative.setNetworkVariable(
    //                             netId,
    //                             key,
    //                             value)) {
    //                     removeKeys(enterpriseConfig);
    //                     loge(config.SSID + ": failed to set " + key +
    //                             ": " + value);
    //                     break setVariables;
    //                 }
    //         }
    //     }
    //     updateFailed = false;
    // } // End of setVariables
    //
    // if (updateFailed) {
    //     if (newNetwork) {
    //         mWifiNative.removeNetwork(netId);
    //         loge("Failed to set a network variable, removed network: " + netId);
    //     }
    //     return new NetworkUpdateResult(INVALID_NETWORK_ID);
    // }
    //
    // /* An update of the network variables requires reading them
    //  * back from the supplicant to update mConfiguredNetworks.
    //  * This is because some of the variables (SSID, wep keys &
    //  * passphrases) reflect different values when read back than
    //  * when written. For example, wep key is stored as * irrespective
    //  * of the value sent to the supplicant
    //  */
    // WifiConfiguration currentConfig = mConfiguredNetworks.get(netId);
    // if (currentConfig == null) {
    //     currentConfig = new WifiConfiguration();
    //     currentConfig.setIpAssignment(IpAssignment.DHCP);
    //     currentConfig.setProxySettings(ProxySettings.NONE);
    //     currentConfig.networkId = netId;
    //     if (config != null) {
    //         // Carry over the creation parameters
    //         currentConfig.selfAdded = config.selfAdded;
    //         currentConfig.didSelfAdd = config.didSelfAdd;
    //         currentConfig.ephemeral = config.ephemeral;
    //         currentConfig.autoJoinUseAggressiveJoinAttemptThreshold
    //                 = config.autoJoinUseAggressiveJoinAttemptThreshold;
    //         currentConfig.lastConnectUid = config.lastConnectUid;
    //         currentConfig.lastUpdateUid = config.lastUpdateUid;
    //         currentConfig.creatorUid = config.creatorUid;
    //         currentConfig.peerWifiConfiguration = config.peerWifiConfiguration;
    //     }
    //     if (DBG) {
    //         loge("created new config netId=" + Integer.toString(netId)
    //                 + " uid=" + Integer.toString(currentConfig.creatorUid));
    //     }
    // }
    //
    // if (uid >= 0) {
    //     if (newNetwork) {
    //         currentConfig.creatorUid = uid;
    //     } else {
    //         currentConfig.lastUpdateUid = uid;
    //     }
    // }
    //
    // if (newNetwork) {
    //     currentConfig.dirty = true;
    // }
    //
    // if (currentConfig.autoJoinStatus == WifiConfiguration.AUTO_JOIN_DELETED) {
    //     // Make sure the configuration is not deleted anymore since we just
    //     // added or modified it.
    //     currentConfig.setAutoJoinStatus(WifiConfiguration.AUTO_JOIN_ENABLED);
    //     currentConfig.selfAdded = false;
    //     currentConfig.didSelfAdd = false;
    //     if (DBG) {
    //         loge("remove deleted status netId=" + Integer.toString(netId)
    //                 + " " + currentConfig.configKey());
    //     }
    // }
    //
    // if (currentConfig.status == WifiConfiguration.Status.ENABLED) {
    //     // Make sure autojoin remain in sync with user modifying the configuration
    //     currentConfig.setAutoJoinStatus(WifiConfiguration.AUTO_JOIN_ENABLED);
    // }
    //
    // if (DBG) loge("will read network variables netId=" + Integer.toString(netId));
    //
    // readNetworkVariables(currentConfig);
    //
    // mConfiguredNetworks.put(netId, currentConfig);
    // mNetworkIds.put(configKey(currentConfig), netId);
    //
    // NetworkUpdateResult result = writeIpAndProxyConfigurationsOnChange(currentConfig, config);
    // result.setIsNewNetwork(newNetwork);
    // result.setNetworkId(netId);
    //
    // writeKnownNetworkHistory();
    //
    // return result;
    assert(0);
    AutoPtr<NetworkUpdateResult> empty;
    return empty;
}

AutoPtr<NetworkUpdateResult> WifiConfigStore::WriteIpAndProxyConfigurationsOnChange(
    /* [in] */ IWifiConfiguration* currentConfig,
    /* [in] */ IWifiConfiguration* newConfig)
{
    // ==================before translated======================
    // boolean ipChanged = false;
    // boolean proxyChanged = false;
    //
    // if (VDBG) {
    //     loge("writeIpAndProxyConfigurationsOnChange: " + currentConfig.SSID + " -> " +
    //             newConfig.SSID + " path: " + ipConfigFile);
    // }
    //
    //
    // switch (newConfig.getIpAssignment()) {
    //     case STATIC:
    //         if (currentConfig.getIpAssignment() != newConfig.getIpAssignment()) {
    //             ipChanged = true;
    //         } else {
    //             ipChanged = !Objects.equals(
    //                     currentConfig.getStaticIpConfiguration(),
    //                     newConfig.getStaticIpConfiguration());
    //         }
    //         break;
    //     case DHCP:
    //         if (currentConfig.getIpAssignment() != newConfig.getIpAssignment()) {
    //             ipChanged = true;
    //         }
    //         break;
    //     case UNASSIGNED:
    //         /* Ignore */
    //         break;
    //     default:
    //         loge("Ignore invalid ip assignment during write");
    //         break;
    // }
    //
    // switch (newConfig.getProxySettings()) {
    //     case STATIC:
    //     case PAC:
    //         ProxyInfo newHttpProxy = newConfig.getHttpProxy();
    //         ProxyInfo currentHttpProxy = currentConfig.getHttpProxy();
    //
    //         if (newHttpProxy != null) {
    //             proxyChanged = !newHttpProxy.equals(currentHttpProxy);
    //         } else {
    //             proxyChanged = (currentHttpProxy != null);
    //         }
    //         break;
    //     case NONE:
    //         if (currentConfig.getProxySettings() != newConfig.getProxySettings()) {
    //             proxyChanged = true;
    //         }
    //         break;
    //     case UNASSIGNED:
    //         /* Ignore */
    //         break;
    //     default:
    //         loge("Ignore invalid proxy configuration during write");
    //         break;
    // }
    //
    // if (ipChanged) {
    //     currentConfig.setIpAssignment(newConfig.getIpAssignment());
    //     currentConfig.setStaticIpConfiguration(newConfig.getStaticIpConfiguration());
    //     log("IP config changed SSID = " + currentConfig.SSID);
    //     if (currentConfig.getStaticIpConfiguration() != null) {
    //         log(" static configuration: " +
    //             currentConfig.getStaticIpConfiguration().toString());
    //     }
    // }
    //
    // if (proxyChanged) {
    //     currentConfig.setProxySettings(newConfig.getProxySettings());
    //     currentConfig.setHttpProxy(newConfig.getHttpProxy());
    //     log("proxy changed SSID = " + currentConfig.SSID);
    //     if (currentConfig.getHttpProxy() != null) {
    //         log(" proxyProperties: " + currentConfig.getHttpProxy().toString());
    //     }
    // }
    //
    // if (ipChanged || proxyChanged) {
    //     writeIpAndProxyConfigurations();
    //     sendConfiguredNetworksChangedBroadcast(currentConfig,
    //             WifiManager.CHANGE_REASON_CONFIG_CHANGE);
    // }
    // return new NetworkUpdateResult(ipChanged, proxyChanged);
    assert(0);
    AutoPtr<NetworkUpdateResult> empty;
    return empty;
}

Boolean WifiConfigStore::EnterpriseConfigKeyShouldBeQuoted(
    /* [in] */ const String& key)
{
    // ==================before translated======================
    // switch (key) {
    //     case WifiEnterpriseConfig.EAP_KEY:
    //     case WifiEnterpriseConfig.ENGINE_KEY:
    //         return false;
    //     default:
    //         return true;
    // }
    assert(0);
    return FALSE;
}

void WifiConfigStore::ReadNetworkVariables(
    /* [in] */ IWifiConfiguration* config)
{
    // ==================before translated======================
    //
    // int netId = config.networkId;
    // if (netId < 0)
    //     return;
    //
    // /*
    //  * TODO: maybe should have a native method that takes an array of
    //  * variable names and returns an array of values. But we'd still
    //  * be doing a round trip to the supplicant daemon for each variable.
    //  */
    // String value;
    //
    // value = mWifiNative.getNetworkVariable(netId, WifiConfiguration.ssidVarName);
    // if (!TextUtils.isEmpty(value)) {
    //     if (value.charAt(0) != '"') {
    //         config.SSID = "\"" + WifiSsid.createFromHex(value).toString() + "\"";
    //         //TODO: convert a hex string that is not UTF-8 decodable to a P-formatted
    //         //supplicant string
    //     } else {
    //         config.SSID = value;
    //     }
    // } else {
    //     config.SSID = null;
    // }
    //
    // value = mWifiNative.getNetworkVariable(netId, WifiConfiguration.bssidVarName);
    // if (!TextUtils.isEmpty(value)) {
    //     config.BSSID = value;
    // } else {
    //     config.BSSID = null;
    // }
    //
    // value = mWifiNative.getNetworkVariable(netId, WifiConfiguration.priorityVarName);
    // config.priority = -1;
    // if (!TextUtils.isEmpty(value)) {
    //     try {
    //         config.priority = Integer.parseInt(value);
    //     } catch (NumberFormatException ignore) {
    //     }
    // }
    //
    // value = mWifiNative.getNetworkVariable(netId, WifiConfiguration.hiddenSSIDVarName);
    // config.hiddenSSID = false;
    // if (!TextUtils.isEmpty(value)) {
    //     try {
    //         config.hiddenSSID = Integer.parseInt(value) != 0;
    //     } catch (NumberFormatException ignore) {
    //     }
    // }
    //
    // value = mWifiNative.getNetworkVariable(netId, WifiConfiguration.modeVarName);
    // config.isIBSS = false;
    // if (!TextUtils.isEmpty(value)) {
    //     try {
    //         config.isIBSS = Integer.parseInt(value) != 0;
    //     } catch (NumberFormatException ignore) {
    //     }
    // }
    //
    // value = mWifiNative.getNetworkVariable(netId, WifiConfiguration.frequencyVarName);
    // config.frequency = 0;
    // if (!TextUtils.isEmpty(value)) {
    //     try {
    //         config.frequency = Integer.parseInt(value);
    //     } catch (NumberFormatException ignore) {
    //     }
    // }
    //
    // value = mWifiNative.getNetworkVariable(netId, WifiConfiguration.wepTxKeyIdxVarName);
    // config.wepTxKeyIndex = -1;
    // if (!TextUtils.isEmpty(value)) {
    //     try {
    //         config.wepTxKeyIndex = Integer.parseInt(value);
    //     } catch (NumberFormatException ignore) {
    //     }
    // }
    //
    // for (int i = 0; i < 4; i++) {
    //     value = mWifiNative.getNetworkVariable(netId,
    //             WifiConfiguration.wepKeyVarNames[i]);
    //     if (!TextUtils.isEmpty(value)) {
    //         config.wepKeys[i] = value;
    //     } else {
    //         config.wepKeys[i] = null;
    //     }
    // }
    //
    // value = mWifiNative.getNetworkVariable(netId, WifiConfiguration.pskVarName);
    // if (!TextUtils.isEmpty(value)) {
    //     config.preSharedKey = value;
    // } else {
    //     config.preSharedKey = null;
    // }
    //
    // value = mWifiNative.getNetworkVariable(config.networkId,
    //         WifiConfiguration.Protocol.varName);
    // if (!TextUtils.isEmpty(value)) {
    //     String vals[] = value.split(" ");
    //     for (String val : vals) {
    //         int index =
    //             lookupString(val, WifiConfiguration.Protocol.strings);
    //         if (0 <= index) {
    //             config.allowedProtocols.set(index);
    //         }
    //     }
    // }
    //
    // value = mWifiNative.getNetworkVariable(config.networkId,
    //         WifiConfiguration.KeyMgmt.varName);
    // if (!TextUtils.isEmpty(value)) {
    //     String vals[] = value.split(" ");
    //     for (String val : vals) {
    //         int index =
    //             lookupString(val, WifiConfiguration.KeyMgmt.strings);
    //         if (0 <= index) {
    //             config.allowedKeyManagement.set(index);
    //         }
    //     }
    // }
    //
    // value = mWifiNative.getNetworkVariable(config.networkId,
    //         WifiConfiguration.AuthAlgorithm.varName);
    // if (!TextUtils.isEmpty(value)) {
    //     String vals[] = value.split(" ");
    //     for (String val : vals) {
    //         int index =
    //             lookupString(val, WifiConfiguration.AuthAlgorithm.strings);
    //         if (0 <= index) {
    //             config.allowedAuthAlgorithms.set(index);
    //         }
    //     }
    // }
    //
    // value = mWifiNative.getNetworkVariable(config.networkId,
    //         WifiConfiguration.PairwiseCipher.varName);
    // if (!TextUtils.isEmpty(value)) {
    //     String vals[] = value.split(" ");
    //     for (String val : vals) {
    //         int index =
    //             lookupString(val, WifiConfiguration.PairwiseCipher.strings);
    //         if (0 <= index) {
    //             config.allowedPairwiseCiphers.set(index);
    //         }
    //     }
    // }
    //
    // value = mWifiNative.getNetworkVariable(config.networkId,
    //         WifiConfiguration.GroupCipher.varName);
    // if (!TextUtils.isEmpty(value)) {
    //     String vals[] = value.split(" ");
    //     for (String val : vals) {
    //         int index =
    //             lookupString(val, WifiConfiguration.GroupCipher.strings);
    //         if (0 <= index) {
    //             config.allowedGroupCiphers.set(index);
    //         }
    //     }
    // }
    //
    // if (config.enterpriseConfig == null) {
    //     config.enterpriseConfig = new WifiEnterpriseConfig();
    // }
    // HashMap<String, String> enterpriseFields = config.enterpriseConfig.getFields();
    // for (String key : ENTERPRISE_CONFIG_SUPPLICANT_KEYS) {
    //     value = mWifiNative.getNetworkVariable(netId, key);
    //     if (!TextUtils.isEmpty(value)) {
    //         if (!enterpriseConfigKeyShouldBeQuoted(key)) {
    //             value = removeDoubleQuotes(value);
    //         }
    //         enterpriseFields.put(key, value);
    //     } else {
    //         enterpriseFields.put(key, EMPTY_VALUE);
    //     }
    // }
    //
    // if (migrateOldEapTlsNative(config.enterpriseConfig, netId)) {
    //     saveConfig();
    // }
    //
    // migrateCerts(config.enterpriseConfig);
    // // initializeSoftwareKeystoreFlag(config.enterpriseConfig, mKeyStore);
    assert(0);
}

String WifiConfigStore::RemoveDoubleQuotes(
    /* [in] */ const String& string)
{
    // ==================before translated======================
    // int length = string.length();
    // if ((length > 1) && (string.charAt(0) == '"')
    //         && (string.charAt(length - 1) == '"')) {
    //     return string.substring(1, length - 1);
    // }
    // return string;
    assert(0);
    return String("");
}

String WifiConfigStore::MakeString(
    /* [in] */ IBitSet* set,
    /* [in] */ ArrayOf<String>* strings)
{
    // ==================before translated======================
    // StringBuffer buf = new StringBuffer();
    // int nextSetBit = -1;
    //
    // /* Make sure all set bits are in [0, strings.length) to avoid
    //  * going out of bounds on strings.  (Shouldn't happen, but...) */
    // set = set.get(0, strings.length);
    //
    // while ((nextSetBit = set.nextSetBit(nextSetBit + 1)) != -1) {
    //     buf.append(strings[nextSetBit].replace('_', '-')).append(' ');
    // }
    //
    // // remove trailing space
    // if (set.cardinality() > 0) {
    //     buf.setLength(buf.length() - 1);
    // }
    //
    // return buf.toString();
    assert(0);
    return String("");
}

Int32 WifiConfigStore::LookupString(
    /* [in] */ const String& string,
    /* [in] */ ArrayOf<String>* strings)
{
    // ==================before translated======================
    // int size = strings.length;
    //
    // string = string.replace('-', '_');
    //
    // for (int i = 0; i < size; i++)
    //     if (string.equals(strings[i]))
    //         return i;
    //
    // // if we ever get here, we should probably add the
    // // value to WifiConfiguration to reflect that it's
    // // supported by the WPA supplicant
    // loge("Failed to look-up a string: " + string);
    //
    // return -1;
    assert(0);
    return 0;
}

Int32 WifiConfigStore::ConfigKey(
    /* [in] */ IWifiConfiguration* config)
{
    // ==================before translated======================
    // String key = config.configKey();
    // return key.hashCode();
    assert(0);
    return 0;
}

void WifiConfigStore::LocalLog(
    /* [in] */ const String& s)
{
    // ==================before translated======================
    // if (mLocalLog != null) {
    //     mLocalLog.log(s);
    // }
    assert(0);
}

void WifiConfigStore::LocalLog(
    /* [in] */ const String& s,
    /* [in] */ Boolean force)
{
    // ==================before translated======================
    // localLog(s);
    // if (force) loge(s);
    assert(0);
}

void WifiConfigStore::LocalLog(
    /* [in] */ const String& s,
    /* [in] */ Int32 netId)
{
    // ==================before translated======================
    // if (mLocalLog == null) {
    //     return;
    // }
    //
    // WifiConfiguration config;
    // synchronized(mConfiguredNetworks) {
    //     config = mConfiguredNetworks.get(netId);
    // }
    //
    // if (config != null) {
    //     mLocalLog.log(s + " " + config.getPrintableSsid() + " " + netId
    //             + " status=" + config.status
    //             + " key=" + config.configKey());
    // } else {
    //     mLocalLog.log(s + " " + netId);
    // }
    assert(0);
}

Boolean WifiConfigStore::PutCertInKeyStore(
    /* [in] */ const String& name,
    /* [in] */ ICertificate* cert)
{
    // ==================before translated======================
    // try {
    //     byte[] certData = Credentials.convertToPem(cert);
    //     if (DBG) Log.d(TAG, "putting certificate " + name + " in keystore");
    //     return mKeyStore.put(name, certData, Process.WIFI_UID, KeyStore.FLAG_NONE);
    //
    // } catch (IOException e1) {
    //     return false;
    // } catch (CertificateException e2) {
    //     return false;
    // }
    assert(0);
    return FALSE;
}

} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namespace Elastos


