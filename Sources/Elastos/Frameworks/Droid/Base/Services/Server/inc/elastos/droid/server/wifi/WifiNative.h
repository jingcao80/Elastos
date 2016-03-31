
#ifndef __ELASTOS_DROID_SERVER_WIFI_WIFINATIVE_H__
#define __ELASTOS_DROID_SERVER_WIFI_WIFINATIVE_H__

#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::Wifi::P2p::IWifiP2pConfig;
using Elastos::Droid::Wifi::P2p::IWifiP2pGroup;
using Elastos::Droid::Wifi::P2p::Nsd::IWifiP2pServiceInfo;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wifi {

/**
 * Native calls for bring up/shut down of the supplicant daemon and for
 * sending requests to the supplicant daemon
 *
 * waitForEvent() is called on the monitor thread for events. All other methods
 * must be serialized from the framework.
 *
 * {@hide}
 */
class WifiNative : public Object
{
public:
    WifiNative(
        /* [in] */ const String& iface);

    static CARAPI_(Boolean) LoadDriver();

    static CARAPI_(Boolean) IsDriverLoaded();

    static CARAPI_(Boolean) UnloadDriver();

    static CARAPI_(Boolean) StartSupplicant(
        /* [in] */ Boolean p2pSupported);

    /* Sends a kill signal to supplicant. To be used when we have lost connection
       or when the supplicant is hung */
    static CARAPI_(Boolean) KillSupplicant(
        /* [in] */ Boolean p2pSupported);

    CARAPI_(Boolean) ConnectToSupplicant();

    CARAPI_(void) CloseSupplicantConnection();

    CARAPI_(String) WaitForEvent();

    CARAPI_(Boolean) Ping();

    CARAPI_(Boolean) Scan();

    CARAPI_(Boolean) SetScanMode(
        /* [in] */ Boolean setActive);

    /* Does a graceful shutdown of supplicant. Is a common stop function for both p2p and sta.
     *
     * Note that underneath we use a harsh-sounding "terminate" supplicant command
     * for a graceful stop and a mild-sounding "stop" interface
     * to kill the process
     */
    CARAPI_(Boolean) StopSupplicant();

    CARAPI_(String) ListNetworks();

    CARAPI_(Int32) AddNetwork();

    CARAPI_(Boolean) SetNetworkVariable(
        /* [in] */ Int32 netId,
        /* [in] */ const String& name,
        /* [in] */ const String& value);

    CARAPI_(String) GetNetworkVariable(
        /* [in] */ Int32 netId,
        /* [in] */ const String& name);

    CARAPI_(Boolean) RemoveNetwork(
        /* [in] */ Int32 netId);

    CARAPI_(Boolean) EnableNetwork(
        /* [in] */ Int32 netId,
        /* [in] */ Boolean disableOthers);

    CARAPI_(Boolean) DisableNetwork(
        /* [in] */ Int32 netId);

    CARAPI_(Boolean) Reconnect();

    CARAPI_(Boolean) Reassociate();

    CARAPI_(Boolean) Disconnect();

    CARAPI_(String) Status();

    CARAPI_(String) GetMacAddress();

    /**
     * Format of results:
     * =================
     * bssid=68:7f:74:d7:1b:6e
     * freq=2412
     * level=-43
     * tsf=1344621975160944
     * age=2623
     * flags=[WPA2-PSK-CCMP][WPS][ESS]
     * ssid=zubyb
     *
     * RANGE=ALL gets all scan results
     * MASK=<N> see wpa_supplicant/src/common/wpa_ctrl.h for details
     */
    CARAPI_(String) ScanResults();

    CARAPI_(Boolean) StartDriver();

    CARAPI_(Boolean) StopDriver();


    /**
     * Start filtering out Multicast V4 packets
     * @return {@code TRUE} if the operation succeeded, {@code FALSE} otherwise
     *
     * Multicast filtering rules work as follows:
     *
     * The driver can filter multicast (v4 and/or v6) and broadcast packets when in
     * a power optimized mode (typically when screen goes off).
     *
     * In order to prevent the driver from filtering the multicast/broadcast packets, we have to
     * add a DRIVER RXFILTER-ADD rule followed by DRIVER RXFILTER-START to make the rule effective
     *
     * DRIVER RXFILTER-ADD Num
     *   where Num = 0 - Unicast, 1 - Broadcast, 2 - Mutil4 or 3 - Multi6
     *
     * and DRIVER RXFILTER-START
     * In order to stop the usage of these rules, we do
     *
     * DRIVER RXFILTER-STOP
     * DRIVER RXFILTER-REMOVE Num
     *   where Num is as described for RXFILTER-ADD
     *
     * The  SETSUSPENDOPT driver command overrides the filtering rules
     */
    CARAPI_(Boolean) StartFilteringMulticastV4Packets();

    /**
     * Stop filtering out Multicast V4 packets.
     * @return {@code TRUE} if the operation succeeded, {@code FALSE} otherwise
     */
    CARAPI_(Boolean) StopFilteringMulticastV4Packets();

    /**
     * Start filtering out Multicast V6 packets
     * @return {@code TRUE} if the operation succeeded, {@code FALSE} otherwise
     */
    CARAPI_(Boolean) StartFilteringMulticastV6Packets();

    /**
     * Stop filtering out Multicast V6 packets.
     * @return {@code TRUE} if the operation succeeded, {@code FALSE} otherwise
     */
    CARAPI_(Boolean) StopFilteringMulticastV6Packets();

    CARAPI_(Int32) GetBand();

    CARAPI_(Boolean) SetBand(
        /* [in] */ Int32 band);

   /**
     * Sets the bluetooth coexistence mode.
     *
     * @param mode One of {@link #BLUETOOTH_COEXISTENCE_MODE_DISABLED},
     *            {@link #BLUETOOTH_COEXISTENCE_MODE_ENABLED}, or
     *            {@link #BLUETOOTH_COEXISTENCE_MODE_SENSE}.
     * @return Whether the mode was successfully set.
     */
    CARAPI_(Boolean) SetBluetoothCoexistenceMode(
        /* [in] */ Int32 mode);

    /**
     * Enable or disable Bluetooth coexistence scan mode. When this mode is on,
     * some of the low-level scan parameters used by the driver are changed to
     * reduce interference with A2DP streaming.
     *
     * @param isSet whether to enable or disable this mode
     * @return {@code TRUE} if the command succeeded, {@code FALSE} otherwise.
     */
    CARAPI_(Boolean) SetBluetoothCoexistenceScanMode(
        /* [in] */ Boolean setCoexScanMode);

    CARAPI_(Boolean) SaveConfig();

    CARAPI_(Boolean) SetScanResultHandling(
        /* [in] */ Int32 mode);

    CARAPI_(Boolean) AddToBlacklist(
        /* [in] */ const String& bssid);

    CARAPI_(Boolean) ClearBlacklist();

    CARAPI_(Boolean) SetSuspendOptimizations(
        /* [in] */ Boolean enabled);

    CARAPI_(Boolean) SetCountryCode(
        /* [in] */ const String& countryCode);

    CARAPI_(void) EnableBackgroundScan(
        /* [in] */ Boolean enable);

    CARAPI_(void) SetScanInterval(
        /* [in] */ Int32 scanInterval);

    /** Example output:
     * RSSI=-65
     * LINKSPEED=48
     * NOISE=9999
     * FREQUENCY=0
     */
    CARAPI_(String) SignalPoll();

    /** Example outout:
     * TXGOOD=396
     * TXBAD=1
     */
    CARAPI_(String) PktcntPoll();

    CARAPI_(Boolean) StartWpsPbc(
        /* [in] */ const String& bssid);

    CARAPI_(Boolean) StartWpsPbc(
        /* [in] */ const String& iface,
        /* [in] */ const String& bssid);

    CARAPI_(Boolean) StartWpsPinKeypad(
        /* [in] */ const String& pin);

    CARAPI_(Boolean) StartWpsPinKeypad(
        /* [in] */ const String& iface,
        /* [in] */ const String& pin);

    CARAPI_(String) StartWpsPinDisplay(
        /* [in] */ const String& bssid);

    CARAPI_(String) StartWpsPinDisplay(
        /* [in] */ const String& iface,
        /* [in] */ const String& bssid);

    /* Configures an access point connection */
    CARAPI_(Boolean) StartWpsRegistrar(
        /* [in] */ const String& bssid,
        /* [in] */ const String& pin);

    CARAPI_(Boolean) CancelWps();

    CARAPI_(Boolean) SetPersistentReconnect(
        /* [in] */ Boolean enabled);

    CARAPI_(Boolean) SetGroupOwnerPsk(
        /* [in] */ const String& name);

    CARAPI_(Boolean) SetDeviceName(
        /* [in] */ const String& name);

    CARAPI_(Boolean) SetDeviceType(
        /* [in] */ const String& type);

    CARAPI_(Boolean) SetConfigMethods(
        /* [in] */ const String& cfg);

    CARAPI_(Boolean) SetManufacturer(
        /* [in] */ const String& value);

    CARAPI_(Boolean) SetModelName(
        /* [in] */ const String& value);

    CARAPI_(Boolean) SetModelNumber(
        /* [in] */ const String& value);

    CARAPI_(Boolean) SetSerialNumber(
        /* [in] */ const String& value);

    CARAPI_(Boolean) SetP2pSsidPostfix(
        /* [in] */ const String& postfix);

    CARAPI_(Boolean) SetP2pGroupIdle(
        /* [in] */ const String& iface,
        /* [in] */ Int32 time);

    CARAPI_(void) SetPowerSave(
        /* [in] */ Boolean enabled);

    CARAPI_(Boolean) SetP2pPowerSave(
        /* [in] */ const String& iface,
        /* [in] */ Boolean enabled);

    CARAPI_(Boolean) SetWfdEnable(
        /* [in] */ Boolean enable);

    CARAPI_(Boolean) SetWfdDeviceInfo(
        /* [in] */ const String& hex);

    /**
     * "sta" prioritizes STA connection over P2P and "p2p" prioritizes
     * P2P connection over STA
     */
    CARAPI_(Boolean) SetConcurrencyPriority(
        /* [in] */ const String& s);

    CARAPI_(Boolean) P2pFind();

    CARAPI_(Boolean) P2pFind(
        /* [in] */ Int32 timeout);

    CARAPI_(Boolean) P2pStopFind();

    CARAPI_(Boolean) P2pListen();

    CARAPI_(Boolean) P2pListen(
        /* [in] */ Int32 timeout);

    CARAPI_(Boolean) P2pFlush();

    /* p2p_connect <peer device address> <pbc|pin|PIN#> [label|display|keypad]
        [persistent] [join|auth] [go_intent=<0..15>] [freq=<in MHz>] */
    CARAPI_(String) P2pConnect(
        /* [in] */ IWifiP2pConfig* config,
        /* [in] */ Boolean joinExistingGroup);

    CARAPI_(Boolean) P2pCancelConnect();

    CARAPI_(Boolean) P2pProvisionDiscovery(
        /* [in] */ IWifiP2pConfig* config);

    CARAPI_(Boolean) P2pGroupAdd(
        /* [in] */ Boolean persistent);

    CARAPI_(Boolean) P2pGroupAdd(
        /* [in] */ Int32 netId);

    CARAPI_(Boolean) P2pGroupRemove(
        /* [in] */ const String& iface);

    CARAPI_(Boolean) P2pReject(
        /* [in] */ const String& deviceAddress);

    /* Invite a peer to a group */
    CARAPI_(Boolean) P2pInvite(
        /* [in] */ IWifiP2pGroup* group,
        /* [in] */ const String& deviceAddress);

    /* Reinvoke a persistent connection */
    CARAPI_(Boolean) P2pReinvoke(
        /* [in] */ Int32 netId,
        /* [in] */ const String& deviceAddress);

    CARAPI_(String) P2pGetSsid(
        /* [in] */ const String& deviceAddress);

    CARAPI_(String) P2pGetDeviceAddress();

    CARAPI_(Int32) GetGroupCapability(
        /* [in] */ const String& deviceAddress);

    CARAPI_(String) P2pPeer(
        /* [in] */ const String& deviceAddress);

    CARAPI_(Boolean) P2pServiceAdd(
        /* [in] */ IWifiP2pServiceInfo* servInfo);

    CARAPI_(Boolean) P2pServiceDel(
        /* [in] */ IWifiP2pServiceInfo* servInfo);

    CARAPI_(Boolean) P2pServiceFlush();

    CARAPI_(String) P2pServDiscReq(
        /* [in] */ const String& addr,
        /* [in] */ const String& query);

    CARAPI_(Boolean) P2pServDiscCancelReq(
        /* [in] */ const String& id);

private:
    CARAPI_(Boolean) ConnectToSupplicant(
        /* [in] */ const String& iface);

    CARAPI_(void) CloseSupplicantConnection(
        /* [in] */ const String& iface);

    /**
     * Wait for the supplicant to send an event, returning the event string.
     * @return the event string sent by the supplicant.
     */
    CARAPI_(String) WaitForEvent(
        /* [in] */ const String& iface);

    CARAPI_(Boolean) DoBooleanCommand(
        /* [in] */ const String& iface,
        /* [in] */ const String& command);

    CARAPI_(Int32) DoIntCommand(
        /* [in] */ const String& iface,
        /* [in] */ const String& command);

    CARAPI_(String) DoStringCommand(
        /* [in] */ const String& iface,
        /* [in] */ const String& command);

    CARAPI_(Boolean) DoBooleanCommand(
        /* [in] */ const String& command);

    CARAPI_(Int32) DoIntCommand(
        /* [in] */ const String& command);

    CARAPI_(String) DoStringCommand(
        /* [in] */ const String& command);

    CARAPI_(String) P2pGetParam(
        /* [in] */ const String& deviceAddress,
        /* [in] */ const String& key);

public:
    static const Int32 BLUETOOTH_COEXISTENCE_MODE_ENABLED = 0;
    static const Int32 BLUETOOTH_COEXISTENCE_MODE_DISABLED = 1;
    static const Int32 BLUETOOTH_COEXISTENCE_MODE_SENSE = 2;

private:
    String mTAG;
    static const Int32 DEFAULT_GROUP_OWNER_INTENT = 7;

    String mInterface;
    Boolean mSuspendOptEnabled;
};

} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_WIFI_WIFINATIVE_H__
