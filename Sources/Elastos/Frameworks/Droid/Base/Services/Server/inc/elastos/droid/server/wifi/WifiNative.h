//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_SERVER_WIFI_WIFINATIVE_H__
#define __ELASTOS_DROID_SERVER_WIFI_WIFINATIVE_H__

#include "_Elastos.Droid.Server.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Thread.h"

using Elastos::Droid::Wifi::IBatchedScanSettings;
using Elastos::Droid::Wifi::IScanResult;
using Elastos::Droid::Wifi::IRttManagerRttResult;
using Elastos::Droid::Wifi::IWifiScannerHotlistSettings;
using Elastos::Droid::Wifi::IWifiScannerWifiChangeSettings;
using Elastos::Droid::Wifi::IWifiLinkLayerStats;
using Elastos::Droid::Wifi::IRttManagerRttParams;
using Elastos::Droid::Wifi::P2p::IWifiP2pConfig;
using Elastos::Droid::Wifi::P2p::IWifiP2pGroup;
using Elastos::Droid::Wifi::P2p::Nsd::IWifiP2pServiceInfo;
using Elastos::Droid::Utility::ILocalLog;
using Elastos::Core::Thread;

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
    class ScanCapabilities : public Object {
    public:
        Int32 max_scan_cache_size;                 // in number of scan results??
        Int32 max_scan_buckets;
        Int32 max_ap_cache_per_scan;
        Int32 max_rssi_sample_size;
        Int32 max_scan_reporting_threshold;        // in number of scan results??
        Int32 max_hotlist_aps;
        Int32 max_significant_wifi_change_aps;
    };

    class ChannelSettings : public Object {
    public:
        Int32 frequency;
        Int32 dwell_time_ms;
        Boolean passive;
    };

    class BucketSettings : public Object {
    public:
        Int32 bucket;
        Int32 band;
        Int32 period_ms;
        Int32 report_events;
        Int32 num_channels;
        AutoPtr<ArrayOf<ChannelSettings*> > channels;
    };

    class ScanSettings : public Object {
    public:
        Int32 base_period_ms;
        Int32 max_ap_per_scan;
        Int32 report_threshold;
        Int32 num_buckets;
        AutoPtr<ArrayOf<BucketSettings*> > buckets;
    };

private:
    class MonitorThread : public Thread {
    public:
        MonitorThread(
            /* [in] */ WifiNative* owner);

        CARAPI Run();
    private:
        WifiNative* mOwner;
    };

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

    CARAPI_(AutoPtr<ILocalLog>) GetLocalLog();

    CARAPI_(Boolean) ConnectToSupplicant();

    CARAPI_(void) CloseSupplicantConnection();

    CARAPI_(String) WaitForEvent();

    CARAPI_(Boolean) Ping();

    CARAPI_(void) SetSupplicantLogLevel(
        /* [in] */ const String& level);

    CARAPI_(String) GetFreqCapability();

    CARAPI_(Boolean) Scan(
        /* [in] */ Int32 type,
        /* [in] */ const String& freqList);

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

    CARAPI_(String) Status(
        /* [in] */ Boolean noEvents);

    CARAPI_(String) GetMacAddress();

    /**
     * Format of results:
     * =================
     * id=1
     * bssid=68:7f:74:d7:1b:6e
     * freq=2412
     * level=-43
     * tsf=1344621975160944
     * age=2623
     * flags=[WPA2-PSK-CCMP][WPS][ESS]
     * ssid=zubyb
     * ====
     *
     * RANGE=ALL gets all scan results
     * RANGE=ID- gets results from ID
     * MASK=<N> see wpa_supplicant/src/common/wpa_ctrl.h for details
     */
    CARAPI_(String) ScanResults(
        /* [in] */ Int32 sid);

    /**
     * Format of result:
     * id=1016
     * bssid=00:03:7f:40:84:10
     * freq=2462
     * beacon_int=200
     * capabilities=0x0431
     * qual=0
     * noise=0
     * level=-46
     * tsf=0000002669008476
     * age=5
     * ie=00105143412d485332302d52322d54455354010882848b960c12182403010b0706555...
     * flags=[WPA2-EAP-CCMP][ESS][P2P][HS20]
     * ssid=QCA-HS20-R2-TEST
     * p2p_device_name=
     * p2p_config_methods=0x0SET_NE
     * anqp_venue_name=02083d656e6757692d466920416c6c69616e63650a3239383920436f...
     * anqp_network_auth_type=010000
     * anqp_roaming_consortium=03506f9a05001bc504bd
     * anqp_ip_addr_type_availability=0c
     * anqp_nai_realm=0200300000246d61696c2e6578616d706c652e636f6d3b636973636f2...
     * anqp_3gpp=000600040132f465
     * anqp_domain_name=0b65786d61706c652e636f6d
     * hs20_operator_friendly_name=11656e6757692d466920416c6c69616e63650e636869...
     * hs20_wan_metrics=01c40900008001000000000a00
     * hs20_connection_capability=0100000006140001061600000650000106bb010106bb0...
     * hs20_osu_providers_list=0b5143412d4f53552d425353010901310015656e6757692d...
     */
    CARAPI_(String) ScanResult(
        /* [in] */ const String& bssid);

    /**
     * Format of command
     * DRIVER WLS_BATCHING SET SCANFREQ=x MSCAN=r BESTN=y CHANNEL=<z, w, t> RTT=s
     * where x is an ascii representation of an integer number of seconds between scans
     *       r is an ascii representation of an integer number of scans per batch
     *       y is an ascii representation of an integer number of the max AP to remember per scan
     *       z, w, t represent a 1..n size list of channel numbers and/or 'A', 'B' values
     *           indicating entire ranges of channels
     *       s is an ascii representation of an integer number of highest-strength AP
     *           for which we'd like approximate distance reported
     *
     * The return value is an ascii integer representing a guess of the number of scans
     * the firmware can remember before it runs out of buffer space or -1 on error
     */
    CARAPI_(String) SetBatchedScanSettings(
        /* [in] */ IBatchedScanSettings* settings);

    CARAPI_(String) GetBatchedScanResults();

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

    CARAPI_(void) EnableSaveConfig();

    CARAPI_(Boolean) SaveConfig();

    CARAPI_(Boolean) AddToBlacklist(
        /* [in] */ const String& bssid);

    CARAPI_(Boolean) ClearBlacklist();

    CARAPI_(Boolean) SetSuspendOptimizations(
        /* [in] */ Boolean enabled);

    CARAPI_(Boolean) SetCountryCode(
        /* [in] */ const String& countryCode);

    CARAPI_(Boolean) EnableBackgroundScan(
        /* [in] */ Boolean enable);

    CARAPI_(void) SetScanInterval(
        /* [in] */ Int32 scanInterval);

    CARAPI_(void) EnableAutoConnect(
        /* [in] */ Boolean enable);

    CARAPI_(void) StartTdls(
        /* [in] */ const String& macAddr,
        /* [in] */ Boolean enable);

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

    CARAPI_(void) BssFlush();

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

    CARAPI_(Boolean) SetExternalSim(
        /* [in] */ Boolean external);

    CARAPI_(Boolean) SimAuthResponse(
        /* [in] */ Int32 id,
        /* [in] */ const String& response);

    /* Configures an access point connection */
    CARAPI_(Boolean) StartWpsRegistrar(
        /* [in] */ const String& bssid,
        /* [in] */ const String& pin);

    CARAPI_(Boolean) CancelWps();

    CARAPI_(Boolean) SetPersistentReconnect(
        /* [in] */ Boolean enabled);

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

    CARAPI_(Boolean) P2pExtListen(
        /* [in] */ Boolean enable,
        /* [in] */ Int32 period,
        /* [in] */ Int32 interval);

    CARAPI_(Boolean) P2pSetChannel(
        /* [in] */ Int32 lc,
        /* [in] */ Int32 oc);

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

    /* Set the current mode of miracast operation.
     *  0 = disabled
     *  1 = operating as source
     *  2 = operating as sink
     */
    CARAPI_(void) SetMiracastMode(
        /* [in] */ Int32 mode);

    CARAPI_(Boolean) GetModeCapability(
        /* [in] */ const String& mode);

    CARAPI_(Boolean) FetchAnqp(
        /* [in] */ const String& bssid,
        /* [in] */ const String& subtypes);

    static CARAPI_(Boolean) SetMode(
        /* [in] */ Int32 mode);

    static CARAPI_(Boolean) StartHal();

    static CARAPI_(void) StopHal();

    static CARAPI_(Int32) GetInterfaces();

    static CARAPI_(String) GetInterfaceName(
        /* [in] */ Int32 index);

    static CARAPI_(Boolean) GetScanCapabilities(
        /* [in] */ ScanCapabilities* capabilities);

    static CARAPI_(void) OnScanResultsAvailable(
        /* [in] */ Int32 id);

    static CARAPI_(void) OnScanStatus(
        /* [in] */ Int32 status);

    static CARAPI_(void) OnFullScanResult(
        /* [in] */ Int32 id,
        /* [in] */ IScanResult* result,
        /* [in] */ ArrayOf<Byte>* bytes);

    static CARAPI_(Boolean) StartScan(
        /* [in] */ ScanSettings* settings,
        /* [in] */ IWifiNativeScanEventHandler* eventHandler);

    static CARAPI_(void) StopScan();

    static CARAPI_(void) PauseScan();

    static CARAPI_(void) RestartScan();

    static CARAPI_(AutoPtr<ArrayOf<IScanResult*> >) GetScanResults();

    static CARAPI_(Boolean) SetHotlist(
        /* [in] */ IWifiScannerHotlistSettings* settings,
        /* [in] */ IWifiNativeHotlistEventHandler* eventHandler);

    static CARAPI_(void) ResetHotlist();

    static CARAPI_(void) OnHotlistApFound(
        /* [in] */ Int32 id,
        /* [in] */ ArrayOf<IScanResult*>* results);

    static CARAPI_(Boolean) TrackSignificantWifiChange(
        /* [in] */ IWifiScannerWifiChangeSettings* settings,
        /* [in] */ IWifiNativeSignificantWifiChangeEventHandler* handler);

    static CARAPI_(void) UntrackSignificantWifiChange();

    static CARAPI_(void) OnSignificantWifiChange(
        /* [in] */ Int32 id,
        /* [in] */ ArrayOf<IScanResult*>* results);

    static CARAPI_(AutoPtr<IWifiLinkLayerStats>) GetWifiLinkLayerStats(
        /* [in] */ const String& iface);

    /*
     * NFC-related calls
     */
    CARAPI_(String) GetNfcWpsConfigurationToken(
        /* [in] */ Int32 netId);

    CARAPI_(String) GetNfcHandoverRequest();

    CARAPI_(String) GetNfcHandoverSelect();

    CARAPI_(Boolean) InitiatorReportNfcHandover(
        /* [in] */ const String& selectMessage);

    CARAPI_(Boolean) ResponderReportNfcHandover(
        /* [in] */ const String& requestMessage);

    static CARAPI_(Int32) GetSupportedFeatureSetNative(
        /* [in] */ Int32 iface);

    static Int32 GetSupportedFeatureSet();

    static CARAPI_(Boolean) RequestRtt(
        /* [in] */ ArrayOf<IRttManagerRttParams*>* params,
        /* [in] */ IWifiNativeRttEventHandler* handler);

    static CARAPI_(Boolean) CancelRtt(
        /* [in] */ ArrayOf<IRttManagerRttParams*>* params);

    static CARAPI_(Boolean) SetScanningMacOui(
        /* [in] */ ArrayOf<Byte>* oui);

    static CARAPI_(AutoPtr<ArrayOf<Int32> >) GetChannelsForBand(
        /* [in] */ Int32 band);

    CARAPI_(Boolean) Disable5GHzFrequencies(
        /* [in] */ Boolean disable);

    static CARAPI_(void) OnRttResults(
        /* [in] */ Int32 id,
        /* [in] */ ArrayOf<IRttManagerRttResult*>* results);

    static CARAPI_(void) SetWifiHalHandle(
        /* [in] */ Int64 handle);

    CARAPI_(void) EnableVerboseLogging(
        /* [in] */ Int32 verbose);

private:
    static CARAPI_(Int32) GetNewCmdIdLocked();

    CARAPI_(void) LocalLog(
        /* [in] */ const String& s);

    CARAPI_(Boolean) ConnectToSupplicantNative();

    CARAPI_(void) CloseSupplicantConnectionNative();

    /**
     * Wait for the supplicant to send an event, returning the event string.
     * @return the event string sent by the supplicant.
     */
    CARAPI_(String) WaitForEventNative();

    CARAPI_(Boolean) DoBooleanCommandNative(
            /* [in] */ const String& command);

    CARAPI_(Int32) DoIntCommandNative(
        /* [in] */ const String& command);

    CARAPI_(String) DoStringCommandNative(
        /* [in] */ const String& command);

    CARAPI_(Boolean) DoBooleanCommand(
        /* [in] */ const String& command);

    CARAPI_(Int32) DoIntCommand(
        /* [in] */ const String& command);

    CARAPI_(String) DoStringCommand(
        /* [in] */ const String& command);

    CARAPI_(String) DoStringCommandWithoutLogging(
        /* [in] */ const String& command);

    CARAPI_(void) LogDbg(
        /* [in] */ const String& debug);

    CARAPI_(String) P2pGetParam(
        /* [in] */ const String& deviceAddress,
        /* [in] */ const String& key);

    static CARAPI_(Boolean) StartHalNative();

    static CARAPI_(void) StopHalNative();

    static CARAPI_(void) WaitForHalEventNative();

    static CARAPI_(Int32) GetInterfacesNative();

    static CARAPI_(String) GetInterfaceNameNative(
        /* [in] */ Int32 index);

    static CARAPI_(Boolean) GetScanCapabilitiesNative(
        /* [in] */ Int32 iface,
        /* [in] */ ScanCapabilities* capabilities);

    static CARAPI_(Boolean) StartScanNative(
        /* [in] */ Int32 iface,
        /* [in] */ Int32 id,
        /* [in] */ ScanSettings* settings);

    static CARAPI_(Boolean) StopScanNative(
        /* [in] */ Int32 iface,
        /* [in] */ Int32 id);

    static CARAPI_(AutoPtr<ArrayOf<IScanResult*> >) GetScanResultsNative(
        /* [in] */ Int32 iface,
        /* [in] */ Boolean flush);

    static CARAPI_(AutoPtr<IWifiLinkLayerStats>) GetWifiLinkLayerStatsNative(
        /* [in] */ Int32 iface);

    static CARAPI_(Boolean) SetHotlistNative(
        /* [in] */ Int32 iface,
        /* [in] */ Int32 id,
        /* [in] */ IWifiScannerHotlistSettings* settings);

    static CARAPI_(Boolean) ResetHotlistNative(
        /* [in] */ Int32 iface,
        /* [in] */ Int32 id);

    static CARAPI_(Boolean) TrackSignificantWifiChangeNative(
        /* [in] */ Int32 iface,
        /* [in] */ Int32 id,
        /* [in] */ IWifiScannerWifiChangeSettings* settings);

    static CARAPI_(Boolean) UntrackSignificantWifiChangeNative(
        /* [in] */ Int32 iface,
        /* [in] */ Int32 id);

    static CARAPI_(Boolean) RequestRangeNative(
        /* [in] */ Int32 iface,
        /* [in] */ Int32 id,
        /* [in] */ ArrayOf<IRttManagerRttParams*>* params);

    static CARAPI_(Boolean) CancelRangeRequestNative(
        /* [in] */ Int32 iface,
        /* [in] */ Int32 id,
        /* [in] */ ArrayOf<IRttManagerRttParams*>* params);

    static CARAPI_(Boolean) SetScanningMacOuiNative(
        /* [in] */ Int32 iface,
        /* [in] */ ArrayOf<Byte>* oui);

    static CARAPI_(AutoPtr<ArrayOf<Int32> >) GetChannelsForBandNative(
        /* [in] */ Int32 iface,
        /* [in] */ Int32 band);

public:
    static const Int32 BLUETOOTH_COEXISTENCE_MODE_ENABLED = 0;
    static const Int32 BLUETOOTH_COEXISTENCE_MODE_DISABLED = 1;
    static const Int32 BLUETOOTH_COEXISTENCE_MODE_SENSE = 2;

    static const Int32 SCAN_WITHOUT_CONNECTION_SETUP = 1;
    static const Int32 SCAN_WITH_CONNECTION_SETUP = 2;

    String mInterfaceName;
    String mInterfacePrefix;

private:

    static Boolean DBG;
    // Hold this lock before calling supplicant - it is required to
    // mutually exclude access from Wifi and P2p state machines
    static Object mLock;// = new Object();
    String mTAG;
    static const Int32 DEFAULT_GROUP_OWNER_INTENT = 6;

    Boolean mSuspendOptEnabled;

    static AutoPtr<ILocalLog> mLocalLog;

    // hold mLock before accessing mCmdIdLock
    static Int32 sCmdId;

    /* WIFI HAL support */

    static String TAG;// = "WifiNative-HAL";
    static Int64 sWifiHalHandle;// = 0;  /* used by JNI to save wifi_handle */
    static AutoPtr<ArrayOf<Int64> > sWifiIfaceHandles;  /* used by JNI to save interface handles */
    static Int32 sWlan0Index;// = -1;
    static Int32 sP2p0Index;// = -1;

    static Boolean sHalIsStarted;// = false;
    static Object mClassLock;

    /* scan status, keep these values in sync with gscan.h */
    static Int32 WIFI_SCAN_BUFFER_FULL;// = 0;
    static Int32 WIFI_SCAN_COMPLETE;// = 1;

    static Int32 sScanCmdId;// = 0;
    static AutoPtr<IWifiNativeScanEventHandler> sScanEventHandler;
    static AutoPtr<ScanSettings> sScanSettings;

    static Int32 sHotlistCmdId;// = 0;
    static AutoPtr<IWifiNativeHotlistEventHandler> sHotlistEventHandler;

    static AutoPtr<IWifiNativeSignificantWifiChangeEventHandler> sSignificantWifiChangeHandler;
    static Int32 sSignificantWifiChangeCmdId;

    static AutoPtr<IWifiNativeRttEventHandler> sRttEventHandler;
    static Int32 sRttCmdId;
};

} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namespace Elastos

DEFINE_CONVERSION_FOR(Elastos::Droid::Server::Wifi::WifiNative::BucketSettings, IInterface);
DEFINE_CONVERSION_FOR(Elastos::Droid::Server::Wifi::WifiNative::ChannelSettings, IInterface);

#endif // __ELASTOS_DROID_SERVER_WIFI_WIFINATIVE_H__
