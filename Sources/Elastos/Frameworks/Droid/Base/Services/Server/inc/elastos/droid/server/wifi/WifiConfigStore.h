#ifndef __ELASTOS_DROID_SERVER_WIFI_WIFICONFIGSTORE_H__
#define __ELASTOS_DROID_SERVER_WIFI_WIFICONFIGSTORE_H__

#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/FileObserver.h"
#include "elastos/droid/server/wifi/NetworkUpdateResult.h"
#include "elastos/droid/server/wifi/WifiNative.h"
#include "elastos/droid/server/net/IpConfigStore.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Net::IStaticIpConfiguration;
using Elastos::Droid::Net::NetworkInfoDetailedState;//enum
using Elastos::Droid::Net::IProxyInfo;
using Elastos::Droid::Os::FileObserver;
using Elastos::Droid::Server::Net::IpConfigStore;
using Elastos::Droid::Server::Net::IDelayedDiskWriteWriter;
using Elastos::Droid::Wifi::IWifiInfo;
using Elastos::Droid::Wifi::IWifiConfiguration;
using Elastos::Droid::Wifi::IWpsInfo;
using Elastos::Droid::Wifi::IWpsResult;
using Elastos::Droid::Wifi::IScanResult;
using Elastos::Droid::Wifi::IWifiEnterpriseConfig;
using Elastos::Droid::Utility::ILocalLog;
using Elastos::Core::IInteger32;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::IO::IDataOutputStream;
using Elastos::Security::IKeyStore;
using Elastos::Security::IPrivateKey;
using Elastos::Security::Cert::ICertificate;
using Elastos::Utility::IBitSet;
using Elastos::Utility::IList;
using Elastos::Utility::IMap;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IHashSet;
using Elastos::Utility::Regex::IPattern;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wifi {

/**
  * This class provides the API to manage configured
  * wifi networks. The API is not thread safe is being
  * used only from WifiStateMachine.
  *
  * It deals with the following
  * - Add/update/remove a WifiConfiguration
  *   The configuration contains two types of information.
  *     = IP and proxy configuration that is handled by WifiConfigStore and
  *       is saved to disk on any change.
  *
  *       The format of configuration file is as follows:
  *       <version>
  *       <netA_key1><netA_value1><netA_key2><netA_value2>...<EOS>
  *       <netB_key1><netB_value1><netB_key2><netB_value2>...<EOS>
  *       ..
  *
  *       (key, value) pairs for a given network are grouped together and can
  *       be in any order. A EOS at the end of a set of (key, value) pairs
  *       indicates that the next set of (key, value) pairs are for a new
  *       network. A network is identified by a unique ID_KEY. If there is no
  *       ID_KEY in the (key, value) pairs, the data is discarded.
  *
  *       An invalid version on read would result in discarding the contents of
  *       the file. On the next write, the latest version is written to file.
  *
  *       Any failures during read or write to the configuration file are ignored
  *       without reporting to the user since the likelihood of these errors are
  *       low and the impact on connectivity is low.
  *
  *     = SSID & security details that is pushed to the supplicant.
  *       supplicant saves these details to the disk on calling
  *       saveConfigCommand().
  *
  *       We have two kinds of APIs exposed:
  *        > public API calls that provide fine grained control
  *          - enableNetwork, disableNetwork, addOrUpdateNetwork(),
  *          removeNetwork(). For these calls, the config is not persisted
  *          to the disk. (TODO: deprecate these calls in WifiManager)
  *        > The new API calls - selectNetwork(), saveNetwork() & forgetNetwork().
  *          These calls persist the supplicant config to disk.
  *
  * - Maintain a list of configured networks for quick access
  *
  */
class WifiConfigStore
    : public IpConfigStore
{
public:
    class WpaConfigFileObserver
        : public FileObserver
    {
    public:
        WpaConfigFileObserver();

        // @Override
        CARAPI OnEvent(
            /* [in] */ Int32 event,
            /* [in] */ const String& path);
    };

    class InnerDelayedDiskWriteWriter
        : public Object
        , public IDelayedDiskWriteWriter
    {
    public:
        CAR_INTERFACE_DECL();

        InnerDelayedDiskWriteWriter(
            /* [in] */ WifiConfigStore* owner,
            /* [in] */ IList* networks);

        CARAPI OnWriteCalled(
            /* [in] */ IDataOutputStream* out);

    private:
        WifiConfigStore* mOwner;
        AutoPtr<IList> mNetworks;
    };

public:
    WifiConfigStore(
        /* [in] */ IContext* c,
        /* [in] */ WifiNative* wn);

    virtual CARAPI EnableVerboseLogging(
        /* [in] */ Int32 verbose);

    /**
      * Fetch the list of configured networks
      * and enable all stored networks in supplicant.
      */
    virtual CARAPI LoadAndEnableAllNetworks();

    virtual CARAPI GetConfiguredNetworksSize(
        /* [out] */ Int32* result);

    /**
      * Fetch the list of currently configured networks
      * @return List of networks
      */
    virtual CARAPI GetConfiguredNetworks(
        /* [out] */ IList** result);//WifiConfiguration

    /**
      * Fetch the list of currently configured networks, filled with real preSharedKeys
      * @return List of networks
      */
    virtual CARAPI GetPrivilegedConfiguredNetworks(
        /* [out] */ IList** result);//WifiConfiguration

    virtual CARAPI GetconfiguredNetworkSize(
        /* [out] */ Int32* result);

    /**
      * Fetch the list of currently configured networks that were recently seen
      *
      * @return List of networks
      */
    virtual CARAPI GetRecentConfiguredNetworks(
        /* [in] */ Int32 milli,
        /* [in] */ Boolean copy,
        /* [out] */ IList** result);//WifiConfiguration

    /**
      *  Update the configuration and BSSID with latest RSSI value.
      */
    virtual CARAPI UpdateConfiguration(
        /* [in] */ IWifiInfo* info);

    /**
      * get the Wificonfiguration for this netId
      *
      * @return Wificonfiguration
      */
    virtual CARAPI GetWifiConfiguration(
        /* [in] */ Int32 netId,
        /* [out] */ IWifiConfiguration** result);

    /**
      * Get the Wificonfiguration for this key
      * @return Wificonfiguration
      */
    virtual CARAPI GetWifiConfiguration(
        /* [in] */ const String& key,
        /* [out] */ IWifiConfiguration** result);

    /**
      * Enable all networks and save config. This will be a no-op if the list
      * of configured networks indicates all networks as being enabled
      */
    virtual CARAPI EnableAllNetworks();

    /**
      * Selects the specified network for connection. This involves
      * updating the priority of all the networks and enabling the given
      * network while disabling others.
      *
      * Selecting a network will leave the other networks disabled and
      * a call to enableAllNetworks() needs to be issued upon a connection
      * or a failure event from supplicant
      *
      * @param netId network to select for connection
      * @return false if the network id is invalid
      */
    virtual CARAPI SelectNetwork(
        /* [in] */ Int32 netId,
        /* [out] */ Boolean* result);

    /**
      * Add/update the specified configuration and save config
      *
      * @param config WifiConfiguration to be saved
      * @return network update result
      */
    virtual CARAPI SaveNetwork(
        /* [in] */ IWifiConfiguration* config,
        /* [in] */ Int32 uid,
        /* [out] */ NetworkUpdateResult** result);

    /**
      * Firmware is roaming away from this BSSID, and this BSSID was on 5GHz, and it's RSSI was good,
      * this means we have a situation where we would want to remain on this BSSID but firmware
      * is not successful at it.
      * This situation is observed on a small number of Access Points, b/17960587
      * In that situation, blacklist this BSSID really hard so as framework will not attempt to
      * roam to it for the next 8 hours. We do not to keep flipping between 2.4 and 5GHz band..
      * TODO: review the blacklisting strategy so as to make it softer and adaptive
      * @param info
      */
    virtual CARAPI DriverRoamedFrom(
        /* [in] */ IWifiInfo* info);

    virtual CARAPI SaveWifiConfigBSSID(
        /* [in] */ IWifiConfiguration* config);

    virtual CARAPI UpdateStatus(
        /* [in] */ Int32 netId,
        /* [in] */ NetworkInfoDetailedState state);

    /**
      * Forget the specified network and save config
      *
      * @param netId network to forget
      * @return {@code true} if it succeeds, {@code false} otherwise
      */
    virtual CARAPI ForgetNetwork(
        /* [in] */ Int32 netId,
        /* [out] */ Boolean* result);

    /**
      * Add/update a network. Note that there is no saveConfig operation.
      * This function is retained for compatibility with the public
      * API. The more powerful saveNetwork() is used by the
      * state machine
      *
      * @param config wifi configuration to add/update
      * @return network Id
      */
    virtual CARAPI AddOrUpdateNetwork(
        /* [in] */ IWifiConfiguration* config,
        /* [in] */ Int32 uid,
        /* [out] */ Int32* result);

    /**
      * Remove a network. Note that there is no saveConfig operation.
      * This function is retained for compatibility with the public
      * API. The more powerful forgetNetwork() is used by the
      * state machine for network removal
      *
      * @param netId network to be removed
      * @return {@code true} if it succeeds, {@code false} otherwise
      */
    virtual CARAPI RemoveNetwork(
        /* [in] */ Int32 netId,
        /* [out] */ Boolean* result);

    /**
      * Enable a network. Note that there is no saveConfig operation.
      * This function is retained for compatibility with the public
      * API. The more powerful selectNetwork()/saveNetwork() is used by the
      * state machine for connecting to a network
      *
      * @param netId network to be enabled
      * @return {@code true} if it succeeds, {@code false} otherwise
      */
    virtual CARAPI EnableNetwork(
        /* [in] */ Int32 netId,
        /* [in] */ Boolean disableOthers,
        /* [out] */ Boolean* result);

    virtual CARAPI EnableNetworkWithoutBroadcast(
        /* [in] */ Int32 netId,
        /* [in] */ Boolean disableOthers,
        /* [out] */ Boolean* result);

    virtual CARAPI DisableAllNetworks();

    /**
      * Disable a network. Note that there is no saveConfig operation.
      * @param netId network to be disabled
      * @return {@code true} if it succeeds, {@code false} otherwise
      */
    virtual CARAPI DisableNetwork(
        /* [in] */ Int32 netId,
        /* [out] */ Boolean* result);

    /**
      * Disable a network. Note that there is no saveConfig operation.
      * @param netId network to be disabled
      * @param reason reason code network was disabled
      * @return {@code true} if it succeeds, {@code false} otherwise
      */
    virtual CARAPI DisableNetwork(
        /* [in] */ Int32 netId,
        /* [in] */ Int32 reason,
        /* [out] */ Boolean* result);

    /**
      * Save the configured networks in supplicant to disk
      * @return {@code true} if it succeeds, {@code false} otherwise
      */
    virtual CARAPI SaveConfig(
        /* [out] */ Boolean* result);

    /**
      * Start WPS pin method configuration with pin obtained
      * from the access point
      * @param config WPS configuration
      * @return Wps result containing status and pin
      */
    virtual CARAPI StartWpsWithPinFromAccessPoint(
        /* [in] */ IWpsInfo* config,
        /* [out] */ IWpsResult** result);

    /**
      * Start WPS pin method configuration with pin obtained
      * from the device
      * @return WpsResult indicating status and pin
      */
    virtual CARAPI StartWpsWithPinFromDevice(
        /* [in] */ IWpsInfo* config,
        /* [out] */ IWpsResult** result);

    /**
      * Start WPS push button configuration
      * @param config WPS configuration
      * @return WpsResult indicating status and pin
      */
    virtual CARAPI StartWpsPbc(
        /* [in] */ IWpsInfo* config,
        /* [out] */ IWpsResult** result);

    /**
      * Fetch the static IP configuration for a given network id
      */
    virtual CARAPI GetStaticIpConfiguration(
        /* [in] */ Int32 netId,
        /* [out] */ IStaticIpConfiguration** result);

    /**
      * Set the static IP configuration for a given network id
      */
    virtual CARAPI SetStaticIpConfiguration(
        /* [in] */ Int32 netId,
        /* [in] */ IStaticIpConfiguration* staticIpConfiguration);

    /**
      * set default GW MAC address
      */
    virtual CARAPI SetDefaultGwMacAddress(
        /* [in] */ Int32 netId,
        /* [in] */ const String& macAddress);

    /**
      * Fetch the proxy properties for a given network id
      * @param network id
      * @return ProxyInfo for the network id
      */
    virtual CARAPI GetProxyProperties(
        /* [in] */ Int32 netId,
        /* [out] */ IProxyInfo** result);

    /**
      * Return if the specified network is using static IP
      * @param network id
      * @return {@code true} if using static ip for netId
      */
    virtual CARAPI IsUsingStaticIp(
        /* [in] */ Int32 netId,
        /* [out] */ Boolean* result);

    virtual CARAPI LoadConfiguredNetworks();

    virtual CARAPI GetNetworkIdFromSsid(
        /* [in] */ const String& ssid,
        /* [out] */ Int32* result);

    virtual CARAPI NeedsUnlockedKeyStore(
        /* [out] */ Boolean* result);

    virtual CARAPI WriteKnownNetworkHistory();

    virtual CARAPI SetLastSelectedConfiguration(
        /* [in] */ Int32 netId);

    virtual CARAPI GetLastSelectedConfiguration(
        /* [out] */ String* result);

    virtual CARAPI IsLastSelectedConfiguration(
        /* [in] */ IWifiConfiguration* config,
        /* [out] */ Boolean* result);

    /**
      * This function run thru the Saved WifiConfigurations and check if some should be linked.
      * @param config
      */
    virtual CARAPI LinkConfiguration(
        /* [in] */ IWifiConfiguration* config);

    /*
      * We try to link a scan result with a WifiConfiguration for which SSID and
      * key management dont match,
      * for instance, we try identify the 5GHz SSID of a DBDC AP,
      * even though we know only of the 2.4GHz
      *
      * Obviously, this function is not optimal since it is used to compare every scan
      * result with every Saved WifiConfiguration, with a string.equals operation.
      * As a speed up, might be better to implement the mConfiguredNetworks store as a
      * <String, WifiConfiguration> object instead of a <Integer, WifiConfiguration> object
      * so as to speed this up. Also to prevent the tiny probability of hash collision.
      *
      */
    virtual CARAPI AssociateWithConfiguration(
        /* [in] */ IScanResult* result,
        /* [out] */ IWifiConfiguration** wificonfig);

    virtual CARAPI MakeChannelList(
        /* [in] */ IWifiConfiguration* config,
        /* [in] */ Int32 age,
        /* [in] */ Boolean _restrict,
        /* [out] */ IHashSet** result);// IInteger32*

    // Update the WifiConfiguration database with the new scan result
    // A scan result can be associated to multiple WifiConfigurations
    virtual CARAPI UpdateSavedNetworkHistory(
        /* [in] */ IScanResult* scanResult,
        /* [out] */ Boolean* result);

    /* return the allowed key management based on a scan result */
    virtual CARAPI WifiConfigurationFromScanResult(
        /* [in] */ IScanResult* result,
        /* [out] */ IWifiConfiguration** wifiConfig);

    virtual CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

    virtual CARAPI GetConfigFile(
        /* [out] */ String* result);

    // Certificate and private key management for EnterpriseConfig
    static CARAPI_(Boolean) NeedsKeyStore(
        /* [in] */ IWifiEnterpriseConfig* config);

    static CARAPI_(Boolean) IsHardwareBackedKey(
        /* [in] */ IPrivateKey* key);

    static CARAPI_(Boolean) HasHardwareBackedKey(
        /* [in] */ ICertificate* certificate);

    static CARAPI_(Boolean) NeedsSoftwareBackedKeyStore(
        /* [in] */ IWifiEnterpriseConfig* config);

    /** called when CS ask WiFistateMachine to disconnect the current network
      * because the score is bad.
      */
    virtual CARAPI HandleBadNetworkDisconnectReport(
        /* [in] */ Int32 netId,
        /* [in] */ IWifiInfo* info);

    virtual CARAPI HandleBSSIDBlackList(
        /* [in] */ Int32 netId,
        /* [in] */ const String& BSSID,
        /* [in] */ Boolean enable,
        /* [out] */ Boolean* result);

    virtual CARAPI HandleDisabledAPs(
        /* [in] */ Boolean enable,
        /* [in] */ const String& BSSID,
        /* [in] */ Int32 reason);

    virtual CARAPI GetMaxDhcpRetries(
        /* [out] */ Int32* result);

    virtual CARAPI HandleSSIDStateChange(
        /* [in] */ Int32 netId,
        /* [in] */ Boolean enabled,
        /* [in] */ const String& message,
        /* [in] */ const String& BSSID);

    virtual CARAPI InstallKeys(
        /* [in] */ IWifiEnterpriseConfig* config,
        /* [in] */ const String& name,
        /* [out] */ Boolean* result);

    virtual CARAPI RemoveKeys(
        /* [in] */ IWifiEnterpriseConfig* config);

    /** Migrates the old style TLS config to the new config style. This should only be used
      * when restoring an old wpa_supplicant.conf or upgrading from a previous
      * platform version.
      * @return true if the config was updated
      * @hide
      */
    virtual CARAPI MigrateOldEapTlsNative(
        /* [in] */ IWifiEnterpriseConfig* config,
        /* [in] */ Int32 netId,
        /* [out] */ Boolean* result);

    /** Migrate certs from global pool to wifi UID if not already done */
    virtual CARAPI MigrateCerts(
        /* [in] */ IWifiEnterpriseConfig* config);

protected:
    virtual CARAPI_(void) Loge(
        /* [in] */ const String& s);

    virtual CARAPI_(void) Loge(
        /* [in] */ const String& s,
        /* [in] */ Boolean stack);

    virtual CARAPI_(void) Log(
        /* [in] */ const String& s);

private:
    static CARAPI_(AutoPtr< ArrayOf<String> >) MiddleInitEnterpriseConfigSupplicantKeys();

    //IWifiConfiguration
    CARAPI_(AutoPtr<IList>) GetConfiguredNetworks(
        /* [in] */ IMap* pskMap);//String, String

    /**
      * Fetch the preSharedKeys for all networks.
      * @return a map from Ssid to preSharedKey.
      */
    //String, String
    CARAPI_(AutoPtr<IMap>) GetCredentialsBySsidMap();

    CARAPI_(Boolean) RemoveConfigAndSendBroadcastIfNeeded(
        /* [in] */ Int32 netId);

    /**
      * Should be called when a single network configuration is made.
      * @param network The network configuration that changed.
      * @param reason The reason for the change, should be one of WifiManager.CHANGE_REASON_ADDED,
      * WifiManager.CHANGE_REASON_REMOVED, or WifiManager.CHANGE_REASON_CHANGE.
      */
    CARAPI_(void) SendConfiguredNetworksChangedBroadcast(
        /* [in] */ IWifiConfiguration* network,
        /* [in] */ Int32 reason);

    /**
      * Should be called when multiple network configuration changes are made.
      */
    CARAPI_(void) SendConfiguredNetworksChangedBroadcast();

    // return map<String, String>
    CARAPI_(AutoPtr<IMap>) ReadNetworkVariablesFromSupplicantFile(
        /* [in] */ const String& key);

    CARAPI_(String) ReadNetworkVariableFromSupplicantFile(
        /* [in] */ const String& ssid,
        /* [in] */ const String& key);

    /* Mark all networks except specified netId as disabled */
    CARAPI_(void) MarkAllNetworksDisabledExcept(
        /* [in] */ Int32 netId);

    CARAPI_(void) MarkAllNetworksDisabled();

    CARAPI_(void) ReadNetworkHistory();

    CARAPI_(void) ReadAutoJoinConfig();

    CARAPI_(void) WriteIpAndProxyConfigurations();

    CARAPI_(void) ReadIpAndProxyConfigurations();

    /*
      * Convert string to Hexadecimal before passing to wifi native layer
      * In native function "doCommand()" have trouble in converting Unicode character string to UTF8
      * conversion to hex is required because SSIDs can have space characters in them;
      * and that can confuses the supplicant because it uses space charaters as delimiters
      */
    CARAPI_(String) EncodeSSID(
        /* [in] */ const String& str);

    CARAPI_(AutoPtr<NetworkUpdateResult>) AddOrUpdateNetworkNative(
        /* [in] */ IWifiConfiguration* config,
        /* [in] */ Int32 uid);

    /* Compare current and new configuration and write to file on change */
    CARAPI_(AutoPtr<NetworkUpdateResult>) WriteIpAndProxyConfigurationsOnChange(
        /* [in] */ IWifiConfiguration* currentConfig,
        /* [in] */ IWifiConfiguration* newConfig);

    /** Returns true if a particular config key needs to be quoted when passed to the supplicant. */
    CARAPI_(Boolean) EnterpriseConfigKeyShouldBeQuoted(
        /* [in] */ const String& key);

    /**
      * Read the variables from the supplicant daemon that are needed to
      * fill in the WifiConfiguration object.
      *
      * @param config the {@link WifiConfiguration} object to be filled in.
      */
    CARAPI_(void) ReadNetworkVariables(
        /* [in] */ IWifiConfiguration* config);

    static CARAPI_(String) RemoveDoubleQuotes(
        /* [in] */ const String& string);

    static CARAPI_(String) MakeString(
        /* [in] */ IBitSet* set,
        /* [in] */ ArrayOf<String>* strings);

    CARAPI_(Int32) LookupString(
        /* [in] */ const String& string,
        /* [in] */ ArrayOf<String>* strings);

    /* Returns a unique for a given configuration */
    static CARAPI_(Int32) ConfigKey(
        /* [in] */ IWifiConfiguration* config);

    CARAPI_(void) LocalLog(
        /* [in] */ const String& s);

    CARAPI_(void) LocalLog(
        /* [in] */ const String& s,
        /* [in] */ Boolean force);

    CARAPI_(void) LocalLog(
        /* [in] */ const String& s,
        /* [in] */ Int32 netId);

    CARAPI_(Boolean) PutCertInKeyStore(
        /* [in] */ const String& name,
        /* [in] */ ICertificate* cert);

public:
    Boolean enableAutoJoinScanWhenAssociated;
    Boolean enableAutoJoinWhenAssociated;
    Boolean enableChipWakeUpWhenAssociated;
    Boolean enableRssiPollWhenAssociated;
    Int32 maxTxPacketForNetworkSwitching;
    Int32 maxRxPacketForNetworkSwitching;
    Int32 maxTxPacketForFullScans;
    Int32 maxRxPacketForFullScans;
    Int32 maxTxPacketForPartialScans;
    Int32 maxRxPacketForPartialScans;
    Boolean enableFullBandScanWhenAssociated;
    Int32 thresholdInitialAutoJoinAttemptMin5RSSI;
    Int32 thresholdInitialAutoJoinAttemptMin24RSSI;
    Int32 thresholdBadRssi5;
    Int32 thresholdLowRssi5;
    Int32 thresholdGoodRssi5;
    Int32 thresholdBadRssi24;
    Int32 thresholdLowRssi24;
    Int32 thresholdGoodRssi24;
    Int32 associatedFullScanBackoff;
    // Will be divided by 8 by WifiStateMachine
    Int32 associatedFullScanMaxIntervalMilli;
    Int32 associatedPartialScanPeriodMilli;
    Int32 bandPreferenceBoostFactor5;
    // Boost by 5 dB per dB above threshold
    Int32 bandPreferencePenaltyFactor5;
    // Penalize by 2 dB per dB below threshold
    Int32 bandPreferencePenaltyThreshold5;
    Int32 bandPreferenceBoostThreshold5;
    Int32 badLinkSpeed24;
    Int32 badLinkSpeed5;
    Int32 goodLinkSpeed24;
    Int32 goodLinkSpeed5;
    Int32 maxAuthErrorsToBlacklist;
    Int32 maxConnectionErrorsToBlacklist;
    Int32 wifiConfigBlacklistMinTimeMilli;
    // Boost RSSI values of associated networks
    Int32 associatedHysteresisHigh;
    Int32 associatedHysteresisLow;
    Int32 thresholdUnblacklistThreshold5Hard;
    Int32 thresholdUnblacklistThreshold5Soft;
    Int32 thresholdUnblacklistThreshold24Hard;
    Int32 thresholdUnblacklistThreshold24Soft;
    Int32 enableVerboseLogging;
    Boolean showNetworks;
    // TODO set this back to false, used for debugging 17516271
    Int32 alwaysEnableScansWhileAssociated;
    Int32 autoJoinScanIntervalWhenP2pConnected;
    Int32 maxNumActiveChannelsForPartialScans;
    Int32 maxNumPassiveChannelsForPartialScans;
    Boolean roamOnAny;
    Boolean onlyLinkSameCredentialConfigurations;
    Boolean enableLinkDebouncing;
    Boolean enable5GHzPreference;
    Boolean enableWifiCellularHandoverUserTriggeredAdjustment;
    /* Enterprise configuration keys */
    /**
      * In old configurations, the "private_key" field was used. However, newer
      * configurations use the key_id field with the engine_id set to "keystore".
      * If this field is found in the configuration, the migration code is
      * triggered.
      */
    static const String OLD_PRIVATE_KEY_NAME;
    /**
      * This represents an empty value of an enterprise field.
      * NULL is used at wpa_supplicant to indicate an empty value
      */
    static const String EMPTY_VALUE;
    /**
      * If Connectivity Service has triggered an unwanted network disconnect
      */
    Int64 lastUnwantedNetworkDisconnectTimestamp;

private:
    AutoPtr<IContext> mContext;
    static const String TAG;
    static const Boolean DBG;
    static Boolean VDBG;
    static Boolean VVDBG;
    static const String SUPPLICANT_CONFIG_FILE;
    /* configured networks with network id as the key */
    AutoPtr<IHashMap> mConfiguredNetworks;//Integer, IWifiConfiguration
    /* A network id is a unique identifier for a network configured in the
      * supplicant. Network ids are generated when the supplicant reads
      * the configuration file at start and can thus change for networks.
      * We store the IP configuration for networks along with a unique id
      * that is generated from SSID and security type of the network. A mapping
      * from the generated unique id to network id of the network is needed to
      * map supplicant config to IP configuration. */
    AutoPtr<IHashMap> mNetworkIds;//Integer, Integer
    /* Tracks the highest priority of configured networks */
    Int32 mLastPriority;
    static const String ipConfigFile;
    static const String networkHistoryConfigFile;
    static const String autoJoinConfigFile;
    /* Network History Keys */
    static const String SSID_KEY;
    static const String CONFIG_KEY;
    static const String CHOICE_KEY;
    static const String LINK_KEY;
    static const String BSSID_KEY;
    static const String BSSID_KEY_END;
    static const String RSSI_KEY;
    static const String FREQ_KEY;
    static const String DATE_KEY;
    static const String MILLI_KEY;
    static const String BLACKLIST_MILLI_KEY;
    static const String NETWORK_ID_KEY;
    static const String PRIORITY_KEY;
    static const String DEFAULT_GW_KEY;
    static const String AUTH_KEY;
    static const String SEPARATOR_KEY;
    static const String STATUS_KEY;
    static const String BSSID_STATUS_KEY;
    static const String SELF_ADDED_KEY;
    static const String FAILURE_KEY;
    static const String DID_SELF_ADD_KEY;
    static const String PEER_CONFIGURATION_KEY;
    static const String CREATOR_UID_KEY;
    static const String CONNECT_UID_KEY;
    static const String UPDATE_UID_KEY;
    static const String SUPPLICANT_STATUS_KEY;
    static const String SUPPLICANT_DISABLE_REASON_KEY;
    static const String FQDN_KEY;
    static const String NUM_CONNECTION_FAILURES_KEY;
    static const String NUM_IP_CONFIG_FAILURES_KEY;
    static const String NUM_AUTH_FAILURES_KEY;
    static const String SCORER_OVERRIDE_KEY;
    static const String SCORER_OVERRIDE_AND_SWITCH_KEY;
    static const String NO_INTERNET_ACCESS_KEY;
    static const String EPHEMERAL_KEY;
    static const String NUM_ASSOCIATION_KEY;
    static const String JOIN_ATTEMPT_BOOST_KEY;
    static const String THRESHOLD_INITIAL_AUTO_JOIN_ATTEMPT_RSSI_MIN_5G_KEY;
    static const String THRESHOLD_INITIAL_AUTO_JOIN_ATTEMPT_RSSI_MIN_24G_KEY;
    static const String THRESHOLD_UNBLACKLIST_HARD_5G_KEY;
    static const String THRESHOLD_UNBLACKLIST_SOFT_5G_KEY;
    static const String THRESHOLD_UNBLACKLIST_HARD_24G_KEY;
    static const String THRESHOLD_UNBLACKLIST_SOFT_24G_KEY;
    static const String THRESHOLD_GOOD_RSSI_5_KEY;
    static const String THRESHOLD_LOW_RSSI_5_KEY;
    static const String THRESHOLD_BAD_RSSI_5_KEY;
    static const String THRESHOLD_GOOD_RSSI_24_KEY;
    static const String THRESHOLD_LOW_RSSI_24_KEY;
    static const String THRESHOLD_BAD_RSSI_24_KEY;
    static const String THRESHOLD_MAX_TX_PACKETS_FOR_NETWORK_SWITCHING_KEY;
    static const String THRESHOLD_MAX_RX_PACKETS_FOR_NETWORK_SWITCHING_KEY;
    static const String THRESHOLD_MAX_TX_PACKETS_FOR_FULL_SCANS_KEY;
    static const String THRESHOLD_MAX_RX_PACKETS_FOR_FULL_SCANS_KEY;
    static const String THRESHOLD_MAX_TX_PACKETS_FOR_PARTIAL_SCANS_KEY;
    static const String THRESHOLD_MAX_RX_PACKETS_FOR_PARTIAL_SCANS_KEY;
    static const String MAX_NUM_ACTIVE_CHANNELS_FOR_PARTIAL_SCANS_KEY;
    static const String MAX_NUM_PASSIVE_CHANNELS_FOR_PARTIAL_SCANS_KEY;
    static const String A_BAND_PREFERENCE_RSSI_THRESHOLD_LOW_KEY;
    static const String A_BAND_PREFERENCE_RSSI_THRESHOLD_KEY;
    static const String G_BAND_PREFERENCE_RSSI_THRESHOLD_KEY;
    static const String ENABLE_AUTOJOIN_WHILE_ASSOCIATED_KEY;
    static const String ASSOCIATED_PARTIAL_SCAN_PERIOD_KEY;
    static const String ASSOCIATED_FULL_SCAN_BACKOFF_KEY;
    static const String ALWAYS_ENABLE_SCAN_WHILE_ASSOCIATED_KEY;
    static const String AUTO_JOIN_SCAN_INTERVAL_WHEN_P2P_CONNECTED_KEY;
    static const String ONLY_LINK_SAME_CREDENTIAL_CONFIGURATIONS_KEY;
    static const String ENABLE_FULL_BAND_SCAN_WHEN_ASSOCIATED_KEY;
    // The three below configurations are mainly for power stats and CPU usage tracking
    // allowing to incrementally disable framework features
    static const String ENABLE_AUTO_JOIN_SCAN_WHILE_ASSOCIATED_KEY;
    static const String ENABLE_AUTO_JOIN_WHILE_ASSOCIATED_KEY;
    static const String ENABLE_CHIP_WAKE_UP_WHILE_ASSOCIATED_KEY;
    static const String ENABLE_RSSI_POLL_WHILE_ASSOCIATED_KEY;
    // The Wifi verbose log is provided as a way to persist the verbose logging settings
    // for testing purpose.
    // It is not intended for normal use.
    static const String WIFI_VERBOSE_LOGS_KEY;
    // As we keep deleted PSK WifiConfiguration for a while, the PSK of
    // those deleted WifiConfiguration is set to this random unused PSK
    static const String DELETED_CONFIG_PSK;
    /**
      * Regex pattern for extracting a connect choice.
      * Matches a strings like the following:
      * <configKey>=([0:9]+)
      */
    static AutoPtr<IPattern> mConnectChoice;
    // Internal use only
    static AutoPtr<ArrayOf<String> > ENTERPRISE_CONFIG_SUPPLICANT_KEYS;
    /**
      * The maximum number of times we will retry a connection to an access point
      * for which we have failed in acquiring an IP address from DHCP. A value of
      * N means that we will make N+1 connection attempts in all.
      * <p>
      * See {@link Settings.Secure#WIFI_MAX_DHCP_RETRY_COUNT}. This is the default
      * value if a Settings value is not present.
      */
    static const Int32 DEFAULT_MAX_DHCP_RETRIES = 9;
    /*const*/ AutoPtr<ILocalLog> mLocalLog;
    /*const*/ AutoPtr<WpaConfigFileObserver> mFileObserver;
    AutoPtr<WifiNative> mWifiNative;
    /*const*/ AutoPtr<IKeyStore> mKeyStore;
    /**
      * The lastSelectedConfiguration is used to remember which network
      * was selected last by the user.
      * The connection to this network may not be successful, as well
      * the selection (i.e. network priority) might not be persisted.
      * WiFi state machine is the only object that sets this variable.
      */
    String lastSelectedConfiguration;
};

} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_WIFI_WIFICONFIGSTORE_H__

