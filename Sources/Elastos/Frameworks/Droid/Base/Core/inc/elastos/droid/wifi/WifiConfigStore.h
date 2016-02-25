
#ifndef __ELASTOS_DROID_NET_WIFI_WIFICONFIGSTORE_H__
#define __ELASTOS_DROID_NET_WIFI_WIFICONFIGSTORE_H__

#include "elastos/droid/ext/frameworkext.h"
//#include "elastos/droid/net/DhcpInfoInternal.h"
#include "elastos/droid/wifi/NetworkUpdateResult.h"
#include "elastos/droid/wifi/WifiNative.h"
#include <elastos/utility/etl/HashMap.h>

using Elastos::Core::IRunnable;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::IBitSet;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Net::NetworkInfoDetailedState;
//using Elastos::Droid::Net::DhcpInfoInternal;
using Elastos::Droid::Net::ILinkProperties;
using Elastos::Droid::Net::IProxyProperties;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::IHandler;

namespace Elastos {
namespace Droid {
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
class WifiConfigStore : public Object
{
private:
    class DelayedDiskWrite
    {
    private:
        class DiskWriteRunnable : public Runnable
        {
        public:
            DiskWriteRunnable(
                /* [in] */ ArrayOf<IWifiConfiguration*>* networks)
                : mNetworks(networks)
            {}

            CARAPI Run();

        private:
            AutoPtr< ArrayOf<IWifiConfiguration*> > mNetworks;
        };

    public:
        static CARAPI_(void) Write(
            /* [in] */ ArrayOf<IWifiConfiguration*>* networks);

    private:
        static CARAPI_(void) OnWriteCalled(
            /* [in] */ ArrayOf<IWifiConfiguration*>* networks);

        //private static void loge(String s) {
        //    Log.e(TAG, s);
        //}

    private:
        static AutoPtr<IHandlerThread> sDiskWriteHandlerThread;
        static AutoPtr<IHandler> sDiskWriteHandler;
        /* Tracks multiple writes on the same thread */
        static Int32 sWriteSequence;
        static String TAG;
        static Object sLock;
        friend class DiskWriteRunnable;
    };

public:
    WifiConfigStore(
        /* [in] */ IContext* c,
        /* [in] */ WifiNative* wn);

    /**
     * Fetch the list of configured networks
     * and enable all stored networks in supplicant.
     */
    CARAPI_(void) Initialize();

    /**
     * Fetch the list of currently configured networks
     * @return List of networks
     */
    CARAPI_(AutoPtr<IObjectContainer>) GetConfiguredNetworks();

    /**
     * enable all networks and save config. This will be a no-op if the list
     * of configured networks indicates all networks as being enabled
     */
    CARAPI_(void) EnableAllNetworks();

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
     * @return FALSE if the network id is invalid
     */
    CARAPI_(Boolean) SelectNetwork(
        /* [in] */ Int32 netId);

    /**
     * Add/update the specified configuration and save config
     *
     * @param config WifiConfiguration to be saved
     * @return network update result
     */
    CARAPI_(AutoPtr<NetworkUpdateResult>) SaveNetwork(
        /* [in] */ IWifiConfiguration* config);

    CARAPI_(void) UpdateStatus(
        /* [in] */ Int32 netId,
        /* [in] */ NetworkInfoDetailedState state);

    /**
     * Forget the specified network and save config
     *
     * @param netId network to forget
     * @return {@code true} if it succeeds, {@code FALSE} otherwise
     */
    CARAPI_(Boolean) ForgetNetwork(
        /* [in] */ Int32 netId);

    /**
     * Add/update a network. Note that there is no saveConfig operation.
     * This function is retained for compatibility with the public
     * API. The more powerful saveNetwork() is used by the
     * state machine
     *
     * @param config wifi configuration to add/update
     * @return network Id
     */
    CARAPI_(Int32) AddOrUpdateNetwork(
        /* [in] */ IWifiConfiguration* config);

    /**
     * Remove a network. Note that there is no saveConfig operation.
     * This function is retained for compatibility with the public
     * API. The more powerful forgetNetwork() is used by the
     * state machine for network removal
     *
     * @param netId network to be removed
     * @return {@code true} if it succeeds, {@code FALSE} otherwise
     */
    CARAPI_(Boolean) RemoveNetwork(
        /* [in] */ Int32 netId);

    /**
     * Enable a network. Note that there is no saveConfig operation.
     * This function is retained for compatibility with the public
     * API. The more powerful selectNetwork()/saveNetwork() is used by the
     * state machine for connecting to a network
     *
     * @param netId network to be enabled
     * @return {@code true} if it succeeds, {@code FALSE} otherwise
     */
    CARAPI_(Boolean) EnableNetwork(
        /* [in] */ Int32 netId,
        /* [in] */ Boolean disableOthers);

    CARAPI_(Boolean) EnableNetworkWithoutBroadcast(
        /* [in] */ Int32 netId,
        /* [in] */ Boolean disableOthers);

    /**
     * Disable a network. Note that there is no saveConfig operation.
     * @param netId network to be disabled
     * @return {@code true} if it succeeds, {@code FALSE} otherwise
     */
    CARAPI_(Boolean) DisableNetwork(
        /* [in] */ Int32 netId);

    /**
     * Disable a network. Note that there is no saveConfig operation.
     * @param netId network to be disabled
     * @param reason reason code network was disabled
     * @return {@code true} if it succeeds, {@code FALSE} otherwise
     */
    CARAPI_(Boolean) DisableNetwork(
        /* [in] */ Int32 netId,
        /* [in] */ Int32 reason);

    /**
     * Save the configured networks in supplicant to disk
     * @return {@code true} if it succeeds, {@code FALSE} otherwise
     */
    CARAPI_(Boolean) SaveConfig();

    /**
     * Start WPS pin method configuration with pin obtained
     * from the access point
     * @param config WPS configuration
     * @return Wps result containing status and pin
     */
    CARAPI_(AutoPtr<IWpsResult>) StartWpsWithPinFromAccessPoint(
        /* [in] */ IWpsInfo* config);

    /**
     * Start WPS pin method configuration with pin obtained
     * from the device
     * @return WpsResult indicating status and pin
     */
    CARAPI_(AutoPtr<IWpsResult>) StartWpsWithPinFromDevice(
        /* [in] */ IWpsInfo* config);

    /**
     * Start WPS push button configuration
     * @param config WPS configuration
     * @return WpsResult indicating status and pin
     */
    CARAPI_(AutoPtr<IWpsResult>) StartWpsPbc(
        /* [in] */ IWpsInfo* config);

    /**
     * Fetch the link properties for a given network id
     * @return LinkProperties for the given network id
     */
    CARAPI_(AutoPtr<ILinkProperties>) GetLinkProperties(
        /* [in] */ Int32 netId);

    /**
     * get IP configuration for a given network id
     * TODO: We cannot handle IPv6 addresses for configuration
     *       right now until NetworkUtils is fixed. When we do
     *       that, we should remove handling DhcpInfo and move
     *       to using LinkProperties
     * @return DhcpInfoInternal for the given network id
     */
//    CARAPI_(AutoPtr<DhcpInfoInternal>) GetIpConfiguration(
//        /* [in] */ Int32 netId);

    /**
     * set IP configuration for a given network id
     */
//    CARAPI_(void) SetIpConfiguration(
//        /* [in] */ Int32 netId,
//        /* [in] */ DhcpInfoInternal* dhcpInfo);

    /**
     * clear IP configuration for a given network id
     * @param network id
     */
    CARAPI_(void) ClearIpConfiguration(
        /* [in] */ Int32 netId);


    /**
     * Fetch the proxy properties for a given network id
     * @param network id
     * @return ProxyProperties for the network id
     */
    CARAPI_(AutoPtr<IProxyProperties>) GetProxyProperties(
        /* [in] */ Int32 netId);

    /**
     * Return if the specified network is using static IP
     * @param network id
     * @return {@code true} if using static ip for netId
     */
    CARAPI_(Boolean) IsUsingStaticIp(
        /* [in] */ Int32 netId);

    CARAPI_(void) LoadConfiguredNetworks();

    CARAPI_(String) GetConfigFile();

private:
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

    /* Mark all networks except specified netId as disabled */
    CARAPI_(void) MarkAllNetworksDisabledExcept(
        /* [in] */ Int32 netId);

    CARAPI_(void) MarkAllNetworksDisabled();

    CARAPI_(void) WriteIpAndProxyConfigurations();

    CARAPI_(void) ReadIpAndProxyConfigurations();

    CARAPI_(AutoPtr<NetworkUpdateResult>) AddOrUpdateNetworkNative(
        /* [in] */ IWifiConfiguration* config);

    /* Compare current and new configuration and write to file on change */
    CARAPI_(AutoPtr<NetworkUpdateResult>) WriteIpAndProxyConfigurationsOnChange(
        /* [in] */ IWifiConfiguration* currentConfig,
        /* [in] */ IWifiConfiguration* newConfig);

    CARAPI_(void) AddIpSettingsFromConfig(
        /* [in] */ ILinkProperties* linkProperties,
        /* [in] */ IWifiConfiguration* config);

    /**
     * Read the variables from the supplicant daemon that are needed to
     * fill in the WifiConfiguration object.
     *
     * @param config the {@link WifiConfiguration} object to be filled in.
     */
    CARAPI_(void) ReadNetworkVariables(
        /* [in] */ IWifiConfiguration* config);

    /**
     * Migration code for old EAP-TLS configurations. This should only be used
     * when restoring an old wpa_supplicant.conf or upgrading from a previous
     * platform version.
     *
     * @param config the configuration to be migrated
     * @param netId the wpa_supplicant's net ID
     * @param value the old private_key value
     */
    CARAPI_(void) MigrateOldEapTlsIfNecessary(
        /* [in] */ IWifiConfiguration* config,
        /* [in] */ Int32 netId);

    CARAPI_(String) RemoveDoubleQuotes(
        /* [in] */ const String& string);

    CARAPI_(String) ConvertToQuotedString(
        /* [in] */ const String& string);

    CARAPI_(String) MakeString(
        /* [in] */ IBitSet* set,
        /* [in] */ ArrayOf<String>* strings);

    CARAPI_(Int32) LookupString(
        /* [in] */ const String& string,
        /* [in] */ ArrayOf<String>* strings);

    /* Returns a unique for a given configuration */
    static CARAPI_(Int32) ConfigKey(
        /* [in] */ IWifiConfiguration* config);

    CARAPI_(String) Dump();

    CARAPI_(void) Loge(
        /* [in] */ const String& s);

    CARAPI_(void) Log(
        /* [in] */ const String& s);

private:
    AutoPtr<IContext> mContext;
    static String TAG;
    static const Boolean DBG;

    /* configured networks with network id as the key */
    HashMap<Int32, AutoPtr<IWifiConfiguration> > mConfiguredNetworks;
    Object mConfiguredNetworksLock;

    /* A network id is a unique identifier for a network configured in the
     * supplicant. Network ids are generated when the supplicant reads
     * the configuration file at start and can thus change for networks.
     * We store the IP configuration for networks along with a unique id
     * that is generated from SSID and security type of the network. A mapping
     * from the generated unique id to network id of the network is needed to
     * map supplicant config to IP configuration. */
    HashMap<Int32, Int32> mNetworkIds;

    /* Tracks the highest priority of configured networks */
    Int32 mLastPriority;

    static String mIpConfigFile;

    static const Int32 IPCONFIG_FILE_VERSION;

    /* IP and proxy configuration keys */
    static String ID_KEY;
    static String IP_ASSIGNMENT_KEY;
    static String LINK_ADDRESS_KEY;
    static String GATEWAY_KEY;
    static String DNS_KEY;
    static String PROXY_SETTINGS_KEY;
    static String PROXY_HOST_KEY;
    static String PROXY_PORT_KEY;
    static String EXCLUSION_LIST_KEY;
    static String EOS;

    AutoPtr<WifiNative> mWifiNative;
};

} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_NET_WIFI_WIFICONFIGSTORE_H__
