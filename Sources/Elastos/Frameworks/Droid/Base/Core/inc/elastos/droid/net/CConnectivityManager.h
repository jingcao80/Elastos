
#ifndef __ELASTOS_DROID_NET_CCONNECTIVITYMANAGER_H__
#define __ELASTOS_DROID_NET_CCONNECTIVITYMANAGER_H__

#include "Elastos.Droid.Utility.h"
#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_Net_CConnectivityManager.h"
#include "elastos/droid/net/ConnectivityManagerNetworkCallback.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/core/Object.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessenger;
using Elastos::Droid::Os::IINetworkManagementService;
using Elastos::Droid::Utility::IArrayMap;

using Elastos::Net::IInetAddress;
using Elastos::Utility::Concurrent::Atomic::IAtomicInteger32;
using Elastos::Utility::IHashMap;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * Class that answers queries about the state of network connectivity. It also
 * notifies applications when network connectivity changes. Get an instance
 * of this class by calling
 * {@link android.content.Context#getSystemService(String) Context.getSystemService(Context.CONNECTIVITY_SERVICE)}.
 * <p>
 * The primary responsibilities of this class are to:
 * <ol>
 * <li>Monitor network connections (Wi-Fi, GPRS, UMTS, etc.)</li>
 * <li>Send broadcast intents when network connectivity changes</li>
 * <li>Attempt to "fail over" to another network when connectivity to a network
 * is lost</li>
 * <li>Provide an API that allows applications to query the coarse-grained or fine-grained
 * state of the available networks</li>
 * <li>Provide an API that allows applications to request and select networks for their data
 * traffic</li>
 * </ol>
 */
CarClass(CConnectivityManager)
    , public Object
    , public IConnectivityManager
{
private:
    class LegacyRequest
        : public Object
    {
    public:
        class InnerSub_ConnectivityManagerNetworkCallback
            : public ConnectivityManagerNetworkCallback
        {
        public:
            InnerSub_ConnectivityManagerNetworkCallback(LegacyRequest* const host);

            // @Override
            CARAPI OnAvailable(
                /* [in] */ INetwork* network);

            // @Override
            CARAPI OnLost(
                /* [in] */ INetwork* network);

        private:
            LegacyRequest* const mHost;
        };

        LegacyRequest();

        AutoPtr<INetworkCapabilities> mNetworkCapabilities;
        AutoPtr<INetworkRequest> mNetworkRequest;
        Int32 mExpireSequenceNumber;
        AutoPtr<INetwork> mCurrentNetwork;
        Int32 mDelay;
        AutoPtr<InnerSub_ConnectivityManagerNetworkCallback> mNetworkCallback;
    };

    class CallbackHandler
        : public Handler
    {
    public:
        CallbackHandler(
            /* [in] */ ILooper* looper,
            /* [in] */ IHashMap* callbackMap,
            /* [in] */ IAtomicInteger32* refCount,
            /* [in] */ CConnectivityManager* cm);

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* message);

    private:
        /* const */ AutoPtr<IHashMap> mCallbackMap;
        /* const */ AutoPtr<IAtomicInteger32> mRefCount;
        static const String TAG;

        CConnectivityManager* const mCm;


        CARAPI GetObject(
            /* [in] */ IMessage* msg,
            /* [in] */ const String& clsName,
            /* [out] */ IInterface** result);

        CARAPI GetCallbacks(
            /* [in] */ INetworkRequest* req,
            /* [out] */ IConnectivityManagerNetworkCallback** result);
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CConnectivityManager();

    /**
     * Tests if a given integer represents a valid network type.
     * @param networkType the type to be tested
     * @return a boolean.  {@code true} if the type is valid, else {@code false}
     */
    static CARAPI IsNetworkTypeValid(
        /* [in] */ Int32 networkType,
        /* [out] */ Boolean* result);

    /**
     * Returns a non-localized string representing a given network type.
     * ONLY used for debugging output.
     * @param type the type needing naming
     * @return a String for the given type, or a string version of the type ("87")
     * if no name is known.
     * {@hide}
     */
    static CARAPI GetNetworkTypeName(
        /* [in] */ Int32 networkType,
        /* [out] */ String* result);

    /**
     * Checks if a given type uses the cellular data connection.
     * This should be replaced in the future by a network property.
     * @param networkType the type to check
     * @return a boolean - {@code true} if uses cellular network, else {@code false}
     * {@hide}
     */
    static CARAPI IsNetworkTypeMobile(
        /* [in] */ Int32 networkType,
        /* [out] */ Boolean* result);

    /**
     * Checks if the given network type is backed by a Wi-Fi radio.
     *
     * @hide
     */
    static CARAPI IsNetworkTypeWifi(
        /* [in] */ Int32 networkType,
        /* [out] */ Boolean* result);

    CARAPI constructor(
        /* [in] */ IIConnectivityManager* service);

    /**
     * Specifies the preferred network type.  When the device has more
     * than one type available the preferred network type will be used.
     *
     * @param preference the network type to prefer over all others.  It is
     *         unspecified what happens to the old preferred network in the
     *         overall ordering.
     * @deprecated Functionality has been removed as it no longer makes sense,
     *             with many more than two networks - we'd need an array to express
     *             preference.  Instead we use dynamic network properties of
     *             the networks to describe their precedence.
     */
    CARAPI SetNetworkPreference(
        /* [in] */ Int32 preference);

    /**
     * Retrieves the current preferred network type.
     *
     * @return an integer representing the preferred network type
     *
     * <p>This method requires the caller to hold the permission
     * {@link android.Manifest.permission#ACCESS_NETWORK_STATE}.
     * @deprecated Functionality has been removed as it no longer makes sense,
     *             with many more than two networks - we'd need an array to express
     *             preference.  Instead we use dynamic network properties of
     *             the networks to describe their precedence.
     */
    CARAPI GetNetworkPreference(
        /* [out] */ Int32* result);

    /**
     * Returns details about the currently active default data network. When
     * connected, this network is the default route for outgoing connections.
     * You should always check {@link NetworkInfo#isConnected()} before initiating
     * network traffic. This may return {@code null} when there is no default
     * network.
     *
     * @return a {@link NetworkInfo} object for the current default network
     *        or {@code null} if no network default network is currently active
     *
     * <p>This method requires the call to hold the permission
     * {@link android.Manifest.permission#ACCESS_NETWORK_STATE}.
     */
    CARAPI GetActiveNetworkInfo(
        /* [out] */ INetworkInfo** result);

    /**
     * Returns details about the currently active default data network
     * for a given uid.  This is for internal use only to avoid spying
     * other apps.
     *
     * @return a {@link NetworkInfo} object for the current default network
     *        for the given uid or {@code null} if no default network is
     *        available for the specified uid.
     *
     * <p>This method requires the caller to hold the permission
     * {@link android.Manifest.permission#CONNECTIVITY_INTERNAL}
     * {@hide}
     */
    CARAPI GetActiveNetworkInfoForUid(
        /* [in] */ Int32 uid,
        /* [out] */ INetworkInfo** result);

    /**
     * Returns connection status information about a particular
     * network type.
     *
     * @param networkType integer specifying which networkType in
     *        which you're interested.
     * @return a {@link NetworkInfo} object for the requested
     *        network type or {@code null} if the type is not
     *        supported by the device.
     *
     * <p>This method requires the caller to hold the permission
     * {@link android.Manifest.permission#ACCESS_NETWORK_STATE}.
     */
    CARAPI GetNetworkInfo(
        /* [in] */ Int32 networkType,
        /* [out] */ INetworkInfo** result);

    /**
     * Returns connection status information about a particular
     * Network.
     *
     * @param network {@link Network} specifying which network
     *        in which you're interested.
     * @return a {@link NetworkInfo} object for the requested
     *        network or {@code null} if the {@code Network}
     *        is not valid.
     *
     * <p>This method requires the caller to hold the permission
     * {@link android.Manifest.permission#ACCESS_NETWORK_STATE}.
     */
    CARAPI GetNetworkInfo(
        /* [in] */ INetwork* network,
        /* [out] */ INetworkInfo** result);

    /**
     * Returns connection status information about all network
     * types supported by the device.
     *
     * @return an array of {@link NetworkInfo} objects.  Check each
     * {@link NetworkInfo#getType} for which type each applies.
     *
     * <p>This method requires the caller to hold the permission
     * {@link android.Manifest.permission#ACCESS_NETWORK_STATE}.
     */
    CARAPI GetAllNetworkInfo(
        /* [out, callee] */ ArrayOf<INetworkInfo*>** result);

    /**
     * Returns the {@link Network} object currently serving a given type, or
     * null if the given type is not connected.
     *
     * <p>This method requires the caller to hold the permission
     * {@link android.Manifest.permission#ACCESS_NETWORK_STATE}.
     *
     * @hide
     */
    CARAPI GetNetworkForType(
        /* [in] */ Int32 networkType,
        /* [out] */ INetwork** result);

    /**
     * Returns an array of all {@link Network} currently tracked by the
     * framework.
     *
     * @return an array of {@link Network} objects.
     *
     * <p>This method requires the caller to hold the permission
     * {@link android.Manifest.permission#ACCESS_NETWORK_STATE}.
     */
    CARAPI GetAllNetworks(
        /* [out] */ ArrayOf<INetwork*>** result);

    /**
     * Returns details about the Provisioning or currently active default data network. When
     * connected, this network is the default route for outgoing connections.
     * You should always check {@link NetworkInfo#isConnected()} before initiating
     * network traffic. This may return {@code null} when there is no default
     * network.
     *
     * @return a {@link NetworkInfo} object for the current default network
     *        or {@code null} if no network default network is currently active
     *
     * <p>This method requires the call to hold the permission
     * {@link android.Manifest.permission#ACCESS_NETWORK_STATE}.
     *
     * {@hide}
     */
    CARAPI GetProvisioningOrActiveNetworkInfo(
        /* [out] */ INetworkInfo** result);

    /**
     * Returns the IP information for the current default network.
     *
     * @return a {@link LinkProperties} object describing the IP info
     *        for the current default network, or {@code null} if there
     *        is no current default network.
     *
     * <p>This method requires the call to hold the permission
     * {@link android.Manifest.permission#ACCESS_NETWORK_STATE}.
     * {@hide}
     */
    CARAPI GetActiveLinkProperties(
        /* [out] */ ILinkProperties** result);

    /**
     * Returns the IP information for a given network type.
     *
     * @param networkType the network type of interest.
     * @return a {@link LinkProperties} object describing the IP info
     *        for the given networkType, or {@code null} if there is
     *        no current default network.
     *
     * <p>This method requires the call to hold the permission
     * {@link android.Manifest.permission#ACCESS_NETWORK_STATE}.
     * {@hide}
     */
    CARAPI GetLinkProperties(
        /* [in] */ Int32 networkType,
        /* [out] */ ILinkProperties** result);

    /**
     * Get the {@link LinkProperties} for the given {@link Network}.  This
     * will return {@code null} if the network is unknown.
     *
     * @param network The {@link Network} object identifying the network in question.
     * @return The {@link LinkProperties} for the network, or {@code null}.
     **/
    CARAPI GetLinkProperties(
        /* [in] */ INetwork* network,
        /* [out] */ ILinkProperties** result);

    /**
     * Get the {@link NetworkCapabilities} for the given {@link Network}.  This
     * will return {@code null} if the network is unknown.
     *
     * @param network The {@link Network} object identifying the network in question.
     * @return The {@link NetworkCapabilities} for the network, or {@code null}.
     */
    CARAPI GetNetworkCapabilities(
        /* [in] */ INetwork* network,
        /* [out] */ INetworkCapabilities** result);

    /**
     * Tells the underlying networking system that the caller wants to
     * begin using the named feature. The interpretation of {@code feature}
     * is completely up to each networking implementation.
     * <p>This method requires the caller to hold the permission
     * {@link android.Manifest.permission#CHANGE_NETWORK_STATE}.
     * @param networkType specifies which network the request pertains to
     * @param feature the name of the feature to be used
     * @return an integer value representing the outcome of the request.
     * The interpretation of this value is specific to each networking
     * implementation+feature combination, except that the value {@code -1}
     * always indicates failure.
     *
     * @deprecated Deprecated in favor of the cleaner {@link #requestNetwork} api.
     */
    CARAPI StartUsingNetworkFeature(
        /* [in] */ Int32 networkType,
        /* [in] */ const String& feature,
        /* [out] */ Int32* result);

    /**
     * Tells the underlying networking system that the caller is finished
     * using the named feature. The interpretation of {@code feature}
     * is completely up to each networking implementation.
     * <p>This method requires the caller to hold the permission
     * {@link android.Manifest.permission#CHANGE_NETWORK_STATE}.
     * @param networkType specifies which network the request pertains to
     * @param feature the name of the feature that is no longer needed
     * @return an integer value representing the outcome of the request.
     * The interpretation of this value is specific to each networking
     * implementation+feature combination, except that the value {@code -1}
     * always indicates failure.
     *
     * @deprecated Deprecated in favor of the cleaner {@link #requestNetwork} api.
     */
    CARAPI StopUsingNetworkFeature(
        /* [in] */ Int32 networkType,
        /* [in] */ const String& feature,
        /* [out] */ Int32* result);

    /**
     * Removes the NET_CAPABILITY_NOT_RESTRICTED capability from the given
     * NetworkCapabilities object if all the capabilities it provides are
     * typically provided by restricted networks.
     *
     * TODO: consider:
     * - Moving to NetworkCapabilities
     * - Renaming it to guessRestrictedCapability and make it set the
     *   restricted capability bit in addition to clearing it.
     * @hide
     */
    static CARAPI MaybeMarkCapabilitiesRestricted(
        /* [in] */ INetworkCapabilities* nc);

    /**
     * Ensure that a network route exists to deliver traffic to the specified
     * host via the specified network interface. An attempt to add a route that
     * already exists is ignored, but treated as successful.
     * <p>This method requires the caller to hold the permission
     * {@link android.Manifest.permission#CHANGE_NETWORK_STATE}.
     * @param networkType the type of the network over which traffic to the specified
     * host is to be routed
     * @param hostAddress the IP address of the host to which the route is desired
     * @return {@code true} on success, {@code false} on failure
     *
     * @deprecated Deprecated in favor of the {@link #requestNetwork},
     *             {@link #setProcessDefaultNetwork} and {@link Network#getSocketFactory} api.
     */
    CARAPI RequestRouteToHost(
        /* [in] */ Int32 networkType,
        /* [in] */ Int32 hostAddress,
        /* [out] */ Boolean* result);

    /**
     * Ensure that a network route exists to deliver traffic to the specified
     * host via the specified network interface. An attempt to add a route that
     * already exists is ignored, but treated as successful.
     * <p>This method requires the caller to hold the permission
     * {@link android.Manifest.permission#CHANGE_NETWORK_STATE}.
     * @param networkType the type of the network over which traffic to the specified
     * host is to be routed
     * @param hostAddress the IP address of the host to which the route is desired
     * @return {@code true} on success, {@code false} on failure
     * @hide
     * @deprecated Deprecated in favor of the {@link #requestNetwork} and
     *             {@link #setProcessDefaultNetwork} api.
     */
    CARAPI RequestRouteToHostAddress(
        /* [in] */ Int32 networkType,
        /* [in] */ IInetAddress* hostAddress,
        /* [out] */ Boolean* result);

    /**
     * Returns the value of the setting for background data usage. If false,
     * applications should not use the network if the application is not in the
     * foreground. Developers should respect this setting, and check the value
     * of this before performing any background data operations.
     * <p>
     * All applications that have background services that use the network
     * should listen to {@link #ACTION_BACKGROUND_DATA_SETTING_CHANGED}.
     * <p>
     * @deprecated As of {@link VERSION_CODES#ICE_CREAM_SANDWICH}, availability of
     * background data depends on several combined factors, and this method will
     * always return {@code true}. Instead, when background data is unavailable,
     * {@link #getActiveNetworkInfo()} will now appear disconnected.
     *
     * @return Whether background data usage is allowed.
     */
    // @Deprecated
    CARAPI GetBackgroundDataSetting(
        /* [out] */ Boolean* result);

    /**
     * Sets the value of the setting for background data usage.
     *
     * @param allowBackgroundData Whether an application should use data while
     *            it is in the background.
     *
     * @attr ref android.Manifest.permission#CHANGE_BACKGROUND_DATA_SETTING
     * @see #getBackgroundDataSetting()
     * @hide
     */
    // @Deprecated
    CARAPI SetBackgroundDataSetting(
        /* [in] */ Boolean allowBackgroundData);

    /**
     * Return quota status for the current active network, or {@code null} if no
     * network is active. Quota status can change rapidly, so these values
     * shouldn't be cached.
     *
     * <p>This method requires the call to hold the permission
     * {@link android.Manifest.permission#ACCESS_NETWORK_STATE}.
     *
     * @hide
     */
    CARAPI GetActiveNetworkQuotaInfo(
        /* [out] */ INetworkQuotaInfo** result);

    /**
     * @hide
     * @deprecated Talk to TelephonyManager directly
     */
     CARAPI GetMobileDataEnabled(
        /* [out] */ Boolean* result);

    static CARAPI From(
        /* [in] */ IContext* ctx,
        /* [out] */ IConnectivityManager** result);

    /** {@hide */
    static CARAPI EnforceTetherChangePermission(
        /* [in] */ IContext* context);


    /**
     * Get the set of tetherable, available interfaces.  This list is limited by
     * device configuration and current interface existence.
     *
     * @return an array of 0 or more Strings of tetherable interface names.
     *
     * <p>This method requires the call to hold the permission
     * {@link android.Manifest.permission#ACCESS_NETWORK_STATE}.
     * {@hide}
     */
    CARAPI GetTetherableIfaces(
        /* [out, callee] */ ArrayOf<String>** result);

    /**
     * Get the set of tethered interfaces.
     *
     * @return an array of 0 or more String of currently tethered interface names.
     *
     * <p>This method requires the call to hold the permission
     * {@link android.Manifest.permission#ACCESS_NETWORK_STATE}.
     * {@hide}
     */
    CARAPI GetTetheredIfaces(
        /* [out, callee] */ ArrayOf<String>** result);

    /**
     * Get the set of interface names which attempted to tether but
     * failed.  Re-attempting to tether may cause them to reset to the Tethered
     * state.  Alternatively, causing the interface to be destroyed and recreated
     * may cause them to reset to the available state.
     * {@link ConnectivityManager#getLastTetherError} can be used to get more
     * information on the cause of the errors.
     *
     * @return an array of 0 or more String indicating the interface names
     *        which failed to tether.
     *
     * <p>This method requires the call to hold the permission
     * {@link android.Manifest.permission#ACCESS_NETWORK_STATE}.
     * {@hide}
     */
    CARAPI GetTetheringErroredIfaces(
        /* [out, callee] */ ArrayOf<String>** result);

    /**
     * Get the set of tethered dhcp ranges.
     *
     * @return an array of 0 or more {@code String} of tethered dhcp ranges.
     * {@hide}
     */
    CARAPI GetTetheredDhcpRanges(
        /* [out] */ ArrayOf<String>** result);

    /**
     * Attempt to tether the named interface.  This will setup a dhcp server
     * on the interface, forward and NAT IP packets and forward DNS requests
     * to the best active upstream network interface.  Note that if no upstream
     * IP network interface is available, dhcp will still run and traffic will be
     * allowed between the tethered devices and this device, though upstream net
     * access will of course fail until an upstream network interface becomes
     * active.
     *
     * @param iface the interface name to tether.
     * @return error a {@code TETHER_ERROR} value indicating success or failure type
     *
     * <p>This method requires the call to hold the permission
     * {@link android.Manifest.permission#CHANGE_NETWORK_STATE}.
     * {@hide}
     */
    CARAPI  Tether(
        /* [in] */ const String& iface,
        /* [out] */ Int32* result);

    /**
     * Stop tethering the named interface.
     *
     * @param iface the interface name to untether.
     * @return error a {@code TETHER_ERROR} value indicating success or failure type
     *
     * <p>This method requires the call to hold the permission
     * {@link android.Manifest.permission#CHANGE_NETWORK_STATE}.
     * {@hide}
     */
    CARAPI Untether(
        /* [in] */ const String& iface,
        /* [out] */ Int32* result);

    /**
     * Check if the device allows for tethering.  It may be disabled via
     * {@code ro.tether.denied} system property, Settings.TETHER_SUPPORTED or
     * due to device configuration.
     *
     * @return a boolean - {@code true} indicating Tethering is supported.
     *
     * <p>This method requires the call to hold the permission
     * {@link android.Manifest.permission#ACCESS_NETWORK_STATE}.
     * {@hide}
     */
    CARAPI IsTetheringSupported(
        /* [out] */ Boolean* result);

    /**
     * Get the list of regular expressions that define any tetherable
     * USB network interfaces.  If USB tethering is not supported by the
     * device, this list should be empty.
     *
     * @return an array of 0 or more regular expression Strings defining
     *        what interfaces are considered tetherable usb interfaces.
     *
     * <p>This method requires the call to hold the permission
     * {@link android.Manifest.permission#ACCESS_NETWORK_STATE}.
     * {@hide}
     */
    CARAPI GetTetherableUsbRegexs(
        /* [out, callee] */ ArrayOf<String>** result);

    /**
     * Get the list of regular expressions that define any tetherable
     * Wifi network interfaces.  If Wifi tethering is not supported by the
     * device, this list should be empty.
     *
     * @return an array of 0 or more regular expression Strings defining
     *        what interfaces are considered tetherable wifi interfaces.
     *
     * <p>This method requires the call to hold the permission
     * {@link android.Manifest.permission#ACCESS_NETWORK_STATE}.
     * {@hide}
     */
    CARAPI GetTetherableWifiRegexs(
        /* [out, callee] */ ArrayOf<String>** result);

    /**
     * Get the list of regular expressions that define any tetherable
     * Bluetooth network interfaces.  If Bluetooth tethering is not supported by the
     * device, this list should be empty.
     *
     * @return an array of 0 or more regular expression Strings defining
     *        what interfaces are considered tetherable bluetooth interfaces.
     *
     * <p>This method requires the call to hold the permission
     * {@link android.Manifest.permission#ACCESS_NETWORK_STATE}.
     * {@hide}
     */
    CARAPI GetTetherableBluetoothRegexs(
        /* [out, callee] */ ArrayOf<String>** result);

    /**
     * Attempt to both alter the mode of USB and Tethering of USB.  A
     * utility method to deal with some of the complexity of USB - will
     * attempt to switch to Rndis and subsequently tether the resulting
     * interface on {@code true} or turn off tethering and switch off
     * Rndis on {@code false}.
     *
     * @param enable a boolean - {@code true} to enable tethering
     * @return error a {@code TETHER_ERROR} value indicating success or failure type
     *
     * <p>This method requires the call to hold the permission
     * {@link android.Manifest.permission#CHANGE_NETWORK_STATE}.
     * {@hide}
     */
    CARAPI SetUsbTethering(
        /* [in] */ Boolean enabled,
        /* [out] */ Int32* result);

    /**
     * Get a more detailed error code after a Tethering or Untethering
     * request asynchronously failed.
     *
     * @param iface The name of the interface of interest
     * @return error The error code of the last error tethering or untethering the named
     *               interface
     *
     * <p>This method requires the call to hold the permission
     * {@link android.Manifest.permission#ACCESS_NETWORK_STATE}.
     * {@hide}
     */
    CARAPI GetLastTetherError(
        /* [in] */ const String& iface,
        /* [out] */ Int32* result);

    /**
     * Report network connectivity status.  This is currently used only
     * to alter status bar UI.
     *
     * @param networkType The type of network you want to report on
     * @param percentage The quality of the connection 0 is bad, 100 is good
     *
     * <p>This method requires the call to hold the permission
     * {@link android.Manifest.permission#STATUS_BAR}.
     * {@hide}
     */
    CARAPI ReportInetCondition(
        /* [in] */ Int32 networkType,
        /* [in] */ Int32 percentage);

    /**
     * Report a problem network to the framework.  This provides a hint to the system
     * that there might be connectivity problems on this network and may cause
     * the framework to re-evaluate network connectivity and/or switch to another
     * network.
     *
     * @param network The {@link Network} the application was attempting to use
     *                or {@code null} to indicate the current default network.
     */
    CARAPI ReportBadNetwork(
        /* [in] */ INetwork* network);

    /**
     * Set a network-independent global http proxy.  This is not normally what you want
     * for typical HTTP proxies - they are general network dependent.  However if you're
     * doing something unusual like general internal filtering this may be useful.  On
     * a private network where the proxy is not accessible, you may break HTTP using this.
     *
     * @param p The a {@link ProxyInfo} object defining the new global
     *        HTTP proxy.  A {@code null} value will clear the global HTTP proxy.
     *
     * <p>This method requires the call to hold the permission
     * android.Manifest.permission#CONNECTIVITY_INTERNAL.
     * @hide
     */
    CARAPI SetGlobalProxy(
        /* [in] */ IProxyInfo* proxyp);

    /**
     * Retrieve any network-independent global HTTP proxy.
     *
     * @return {@link ProxyInfo} for the current global HTTP proxy or {@code null}
     *        if no global HTTP proxy is set.
     *
     * <p>This method requires the call to hold the permission
     * {@link android.Manifest.permission#ACCESS_NETWORK_STATE}.
     * @hide
     */
    CARAPI GetGlobalProxy(
        /* [out] */ IProxyInfo** p);

    /**
     * Get the HTTP proxy settings for the current default network.  Note that
     * if a global proxy is set, it will override any per-network setting.
     *
     * @return the {@link ProxyInfo} for the current HTTP proxy, or {@code null} if no
     *        HTTP proxy is active.
     *
     * <p>This method requires the call to hold the permission
     * {@link android.Manifest.permission#ACCESS_NETWORK_STATE}.
     * {@hide}
     * @deprecated Deprecated in favor of {@link #getLinkProperties}
     */
    CARAPI GetProxy(
        /* [out] */ IProxyInfo** p);

    /**
     * Sets a secondary requirement bit for the given networkType.
     * This requirement bit is generally under the control of the carrier
     * or its agents and is not directly controlled by the user.
     *
     * @param networkType The network who's dependence has changed
     * @param met Boolean - true if network use is OK, false if not
     *
     * <p>This method requires the call to hold the permission
     * {@link android.Manifest.permission#CONNECTIVITY_INTERNAL}.
     * {@hide}
     */
    CARAPI SetDataDependency(
        /* [in] */ Int32 networkType,
        /* [in] */ Boolean met);

    /**
     * Returns true if the hardware supports the given network type
     * else it returns false.  This doesn't indicate we have coverage
     * or are authorized onto a network, just whether or not the
     * hardware supports it.  For example a GSM phone without a SIM
     * should still return {@code true} for mobile data, but a wifi only
     * tablet would return {@code false}.
     *
     * @param networkType The network type we'd like to check
     * @return {@code true} if supported, else {@code false}
     *
     * <p>This method requires the call to hold the permission
     * {@link android.Manifest.permission#ACCESS_NETWORK_STATE}.
     * @hide
     */
    CARAPI IsNetworkSupported(
        /* [in] */ Int32 networkType,
        /* [out] */ Boolean* result);

    /**
     * Returns if the currently active data network is metered. A network is
     * classified as metered when the user is sensitive to heavy data usage on
     * that connection due to monetary costs, data limitations or
     * battery/performance issues. You should check this before doing large
     * data transfers, and warn the user or delay the operation until another
     * network is available.
     *
     * @return {@code true} if large transfers should be avoided, otherwise
     *        {@code false}.
     *
     * <p>This method requires the call to hold the permission
     * {@link android.Manifest.permission#ACCESS_NETWORK_STATE}.
     */
    CARAPI IsActiveNetworkMetered(
        /* [out] */ Boolean* result);

    /**
     * If the LockdownVpn mechanism is enabled, updates the vpn
     * with a reload of its profile.
     *
     * @return a boolean with {@code} indicating success
     *
     * <p>This method can only be called by the system UID
     * {@hide}
     */
    CARAPI UpdateLockdownVpn(
        /* [out] */ Boolean* result);

    /**
     * Signal that the captive portal check on the indicated network
     * is complete and whether its a captive portal or not.
     *
     * @param info the {@link NetworkInfo} object for the networkType
     *        in question.
     * @param isCaptivePortal true/false.
     *
     * <p>This method requires the call to hold the permission
     * {@link android.Manifest.permission#CONNECTIVITY_INTERNAL}.
     * {@hide}
     */
    CARAPI CaptivePortalCheckCompleted(
        /* [in] */ INetworkInfo* info,
        /* [in] */ Boolean isCaptivePortal);

    /**
     * Supply the backend messenger for a network tracker
     *
     * @param networkType NetworkType to set
     * @param messenger {@link Messenger}
     * {@hide}
     */
    CARAPI SupplyMessenger(
        /* [in] */ Int32 networkType,
        /* [in] */ IMessenger* messenger);

    /**
     * Check mobile provisioning.
     *
     * @param suggestedTimeOutMs, timeout in milliseconds
     *
     * @return time out that will be used, maybe less that suggestedTimeOutMs
     * -1 if an error.
     *
     * {@hide}
     */
    CARAPI CheckMobileProvisioning(
        /* [in] */ Int32 suggestedTimeOutMs,
        /* [out] */ Int32* result);

    /**
     * Get the mobile provisioning url.
     * {@hide}
     */
    CARAPI GetMobileProvisioningUrl(
        /* [out] */ String* result);

    /**
     * Get the mobile redirected provisioning url.
     * {@hide}
     */
    CARAPI GetMobileRedirectedProvisioningUrl(
        /* [out] */ String* result);

    /**
     * get the information about a specific network link
     * @hide
     */
    CARAPI GetLinkQualityInfo(
        /* [in] */ Int32 networkType,
        /* [out] */ ILinkQualityInfo** result);

    /**
     * get the information of currently active network link
     * @hide
     */
    CARAPI GetActiveLinkQualityInfo(
        /* [out] */ ILinkQualityInfo** result);

    /**
     * get the information of all network links
     * @hide
     */
    CARAPI GetAllLinkQualityInfo(
        /* [out, callee] */ ArrayOf<ILinkQualityInfo*>** result);

    /**
     * Set sign in error notification to visible or in visible
     *
     * @param visible
     * @param networkType
     *
     * {@hide}
     */
    CARAPI SetProvisioningNotificationVisible(
        /* [in] */ Boolean visible,
        /* [in] */ Int32 networkType,
        /* [in] */ const String& action);

    /**
     * Set the value for enabling/disabling airplane mode
     *
     * @param enable whether to enable airplane mode or not
     *
     * <p>This method requires the call to hold the permission
     * {@link android.Manifest.permission#CONNECTIVITY_INTERNAL}.
     * @hide
     */
    CARAPI SetAirplaneMode(
        /* [in] */ Boolean enable);

    /** {@hide} */
    CARAPI RegisterNetworkFactory(
        /* [in] */ IMessenger* messenger,
        /* [in] */ const String& name);

    /** {@hide} */
    CARAPI UnregisterNetworkFactory(
        /* [in] */ IMessenger* messenger);

    /** {@hide} */
    CARAPI RegisterNetworkAgent(
        /* [in] */ IMessenger* messenger,
        /* [in] */ INetworkInfo* ni,
        /* [in] */ ILinkProperties* lp,
        /* [in] */ INetworkCapabilities* nc,
        /* [in] */ Int32 score,
        /* [in] */ INetworkMisc* misc);

    /**
     * Start listening to reports when the system's default data network is active, meaning it is
     * a good time to perform network traffic.  Use {@link #isDefaultNetworkActive()}
     * to determine the current state of the system's default network after registering the
     * listener.
     * <p>
     * If the process default network has been set with
     * {@link ConnectivityManager#setProcessDefaultNetwork} this function will not
     * reflect the process's default, but the system default.
     *
     * @param l The listener to be told when the network is active.
     */
    CARAPI AddDefaultNetworkActiveListener(
        /* [in] */ IConnectivityManagerOnNetworkActiveListener* l);

    /**
     * Remove network active listener previously registered with
     * {@link #addDefaultNetworkActiveListener}.
     *
     * @param l Previously registered listener.
     */
    CARAPI RemoveDefaultNetworkActiveListener(
        /* [in] */ IConnectivityManagerOnNetworkActiveListener* l);

    /**
     * Return whether the data network is currently active.  An active network means that
     * it is currently in a high power state for performing data transmission.  On some
     * types of networks, it may be expensive to move and stay in such a state, so it is
     * more power efficient to batch network traffic together when the radio is already in
     * this state.  This method tells you whether right now is currently a good time to
     * initiate network traffic, as the network is already active.
     */
    CARAPI IsDefaultNetworkActive(
        /* [out] */ Boolean* result);

    /**
     * Request a network to satisfy a set of {@link NetworkCapabilities}.
     *
     * This {@link NetworkRequest} will live until released via
     * {@link #unregisterNetworkCallback} or the calling application exits.
     * Status of the request can be followed by listening to the various
     * callbacks described in {@link NetworkCallback}.  The {@link Network}
     * can be used to direct traffic to the network.
     *
     * @param request {@link NetworkRequest} describing this request.
     * @param networkCallback The {@link NetworkCallback} to be utilized for this
     *                        request.  Note the callback must not be shared - they
     *                        uniquely specify this request.
     */
    CARAPI RequestNetwork(
        /* [in] */ INetworkRequest* request,
        /* [in] */ IConnectivityManagerNetworkCallback* networkCallback);

    /**
     * Request a network to satisfy a set of {@link NetworkCapabilities}, limited
     * by a timeout.
     *
     * This function behaves identically to the non-timedout version, but if a suitable
     * network is not found within the given time (in milliseconds) the
     * {@link NetworkCallback#unavailable} callback is called.  The request must
     * still be released normally by calling {@link releaseNetworkRequest}.
     * @param request {@link NetworkRequest} describing this request.
     * @param networkCallback The callbacks to be utilized for this request.  Note
     *                        the callbacks must not be shared - they uniquely specify
     *                        this request.
     * @param timeoutMs The time in milliseconds to attempt looking for a suitable network
     *                  before {@link NetworkCallback#unavailable} is called.
     * @hide
     */
    CARAPI RequestNetwork(
        /* [in] */ INetworkRequest* request,
        /* [in] */ IConnectivityManagerNetworkCallback* networkCallback,
        /* [in] */ Int32 timeoutMs);

    /**
     * Request a network to satisfy a set of {@link NetworkCapabilities}.
     *
     * This function behavies identically to the version that takes a NetworkCallback, but instead
     * of {@link NetworkCallback} a {@link PendingIntent} is used.  This means
     * the request may outlive the calling application and get called back when a suitable
     * network is found.
     * <p>
     * The operation is an Intent broadcast that goes to a broadcast receiver that
     * you registered with {@link Context#registerReceiver} or through the
     * &lt;receiver&gt; tag in an AndroidManifest.xml file
     * <p>
     * The operation Intent is delivered with two extras, a {@link Network} typed
     * extra called {@link #EXTRA_NETWORK_REQUEST_NETWORK} and a {@link NetworkRequest}
     * typed extra called {@link #EXTRA_NETWORK_REQUEST_NETWORK_REQUEST} containing
     * the original requests parameters.  It is important to create a new,
     * {@link NetworkCallback} based request before completing the processing of the
     * Intent to reserve the network or it will be released shortly after the Intent
     * is processed.
     * <p>
     * If there is already an request for this Intent registered (with the equality of
     * two Intents defined by {@link Intent#filterEquals}), then it will be removed and
     * replaced by this one, effectively releasing the previous {@link NetworkRequest}.
     * <p>
     * The request may be released normally by calling {@link #unregisterNetworkCallback}.
     *
     * @param request {@link NetworkRequest} describing this request.
     * @param operation Action to perform when the network is available (corresponds
     *                  to the {@link NetworkCallback#onAvailable} call.  Typically
     *                  comes from {@link PendingIntent#getBroadcast}.
     * @hide
     */
    CARAPI RequestNetwork(
        /* [in] */ INetworkRequest* request,
        /* [in] */ IPendingIntent* operation);

    /**
     * Registers to receive notifications about all networks which satisfy the given
     * {@link NetworkRequest}.  The callbacks will continue to be called until
     * either the application exits or {@link #unregisterNetworkCallback} is called
     *
     * @param request {@link NetworkRequest} describing this request.
     * @param networkCallback The {@link NetworkCallback} that the system will call as suitable
     *                        networks change state.
     */
    CARAPI RegisterNetworkCallback(
        /* [in] */ INetworkRequest* request,
        /* [in] */ IConnectivityManagerNetworkCallback* networkCallback);

    /**
     * Unregisters callbacks about and possibly releases networks originating from
     * {@link #requestNetwork} and {@link #registerNetworkCallback} calls.  If the
     * given {@code NetworkCallback} had previosuly been used with {@code #requestNetwork},
     * any networks that had been connected to only to satisfy that request will be
     * disconnected.
     *
     * @param networkCallback The {@link NetworkCallback} used when making the request.
     */
    CARAPI UnregisterNetworkCallback(
        /* [in] */ IConnectivityManagerNetworkCallback* networkCallback);

    /**
     * Binds the current process to {@code network}.  All Sockets created in the future
     * (and not explicitly bound via a bound SocketFactory from
     * {@link Network#getSocketFactory() Network.getSocketFactory()}) will be bound to
     * {@code network}.  All host name resolutions will be limited to {@code network} as well.
     * Note that if {@code network} ever disconnects, all Sockets created in this way will cease to
     * work and all host name resolutions will fail.  This is by design so an application doesn't
     * accidentally use Sockets it thinks are still bound to a particular {@link Network}.
     * To clear binding pass {@code null} for {@code network}.  Using individually bound
     * Sockets created by Network.getSocketFactory().createSocket() and
     * performing network-specific host name resolutions via
     * {@link Network#getAllByName Network.getAllByName} is preferred to calling
     * {@code setProcessDefaultNetwork}.
     *
     * @param network The {@link Network} to bind the current process to, or {@code null} to clear
     *                the current binding.
     * @return {@code true} on success, {@code false} if the {@link Network} is no longer valid.
     */
    static CARAPI SetProcessDefaultNetwork(
        /* [in] */ INetwork* network,
        /* [out] */ Boolean* result);

    /**
     * Returns the {@link Network} currently bound to this process via
     * {@link #setProcessDefaultNetwork}, or {@code null} if no {@link Network} is explicitly bound.
     *
     * @return {@code Network} to which this process is bound, or {@code null}.
     */
    static CARAPI GetProcessDefaultNetwork(
        /* [out] */ INetwork** result);

    /**
     * Binds host resolutions performed by this process to {@code network}.
     * {@link #setProcessDefaultNetwork} takes precedence over this setting.
     *
     * @param network The {@link Network} to bind host resolutions from the current process to, or
     *                {@code null} to clear the current binding.
     * @return {@code true} on success, {@code false} if the {@link Network} is no longer valid.
     * @hide
     * @deprecated This is strictly for legacy usage to support {@link #startUsingNetworkFeature}.
     */
    static CARAPI SetProcessDefaultNetworkForHostResolution(
        /* [in] */ INetwork* network,
        /* [out] */ Boolean* result);

private:
    CARAPI NetworkCapabilitiesForFeature(
        /* [in] */ Int32 networkType,
        /* [in] */ const String& feature,
        /* [out] */ INetworkCapabilities** result);

    CARAPI InferLegacyTypeForNetworkCapabilities(
        /* [in] */ INetworkCapabilities* netCap,
        /* [out] */ Int32* result);

    CARAPI LegacyTypeForNetworkCapabilities(
        /* [in] */ INetworkCapabilities* netCap,
        /* [out] */ Int32* result);

    CARAPI FindRequestForFeature(
        /* [in] */ INetworkCapabilities* netCap,
        /* [out] */ INetworkRequest** result);


    CARAPI RenewRequestLocked(
        /* [in] */ LegacyRequest* l);

    CARAPI ExpireRequest(
        /* [in] */ INetworkCapabilities* netCap,
        /* [in] */ Int32 sequenceNum);

    CARAPI RequestNetworkForFeatureLocked(
        /* [in] */ INetworkCapabilities* netCap,
        /* [out] */ INetworkRequest** result);

    CARAPI SendExpireMsgForFeature(
        /* [in] */ INetworkCapabilities* netCap,
        /* [in] */ Int32 seqNum,
        /* [in] */ Int32 delay);

    CARAPI RemoveRequestForFeature(
        /* [in] */ INetworkCapabilities* netCap,
        /* [out] */ IConnectivityManagerNetworkCallback** result);

    CARAPI GetNetworkManagementService(
        /* [out] */ IINetworkManagementService** result);

    CARAPI IncCallbackHandlerRefCount();

    CARAPI DecCallbackHandlerRefCount();

    CARAPI SendRequestForNetwork(
        /* [in] */ INetworkCapabilities* need,
        /* [in] */ IConnectivityManagerNetworkCallback* networkCallback,
        /* [in] */ Int32 timeoutSec,
        /* [in] */ Int32 action,
        /* [in] */ Int32 legacyType,
        /* [out] */ INetworkRequest** result);

public:
    static const AutoPtr<IHashMap> sNetworkCallback;

    static const AutoPtr<IAtomicInteger32> sCallbackRefCount;

    static AutoPtr<IHandler> sCallbackHandler;

private:
    static const String TAG;
    static const Boolean LEGACY_DBG; // STOPSHIP
    AutoPtr<IIConnectivityManager> mService;
    AutoPtr<IINetworkManagementService> mNMService;
    static const AutoPtr<IHashMap> sLegacyRequests;
    AutoPtr<IArrayMap> mNetworkActivityListeners;
    static const Int32 BASE;
    static const Int32 EXPIRE_LEGACY_REQUEST;
    const static Int32 LISTEN;
    const static Int32 REQUEST;

    friend class CConnectivityManager::CallbackHandler;
};

} // namespace Net
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_CCONNECTIVITYMANAGER_H__
