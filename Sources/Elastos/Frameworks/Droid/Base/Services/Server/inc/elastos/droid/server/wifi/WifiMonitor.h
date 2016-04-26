
#ifndef __ELASTOS_DROID_SERVER_WIFI_WIFIMONITOR_H__
#define __ELASTOS_DROID_SERVER_WIFI_WIFIMONITOR_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/server/wifi/WifiNative.h"
#include "elastos/droid/internal/utility/StateMachine.h"
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::Internal::Utility::StateMachine;
using Elastos::Droid::Internal::Utility::IProtocol;
using Elastos::Droid::Net::NetworkInfoDetailedState;
using Elastos::Droid::Wifi::SupplicantState;
using Elastos::Droid::Wifi::IWifiSsid;
using Elastos::Utility::Regex::IPattern;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::IHashMap;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wifi {

/**
 * Listens for events from the wpa_supplicant server, and passes them on
 * to the {@link StateMachine} for handling. Runs in its own thread.
 *
 * @hide
 */
class WifiMonitor : public Object
{
public:
    class WifiMonitorSingleton : public Object
    {
    public:
        CARAPI_(void) StartMonitoring(
            /* [in] */ const String& iface);

        CARAPI_(void) StopMonitoring(
            /* [in] */ const String& iface);

        CARAPI_(void) RegisterInterfaceMonitor(
            /* [in] */ const String& iface,
            /* [in] */ WifiMonitor* m);

        CARAPI_(void) UnregisterInterfaceMonitor(
            /* [in] */ const String& iface);

        CARAPI_(void) StopSupplicant();

        CARAPI_(void) KillSupplicant(
             /* [in] */ Boolean p2pSupported);

        CARAPI_(Boolean) DispatchEvent(
            /* [in] */ const String& eventStr);

    private:
        WifiMonitorSingleton();

    public:
        static AutoPtr<WifiMonitorSingleton> sInstance;

    private:
        AutoPtr<IHashMap> mIfaceMap;// <String, AutoPtr<WifiMonitor> >
        Boolean mConnected;
        AutoPtr<WifiNative> mWifiNative;
        Object mLock;
    };

    class MonitorThread
        : public Thread
    {
    public:
        MonitorThread(
            /* [in] */ WifiMonitor* owner,
            /* [in] */ WifiNative* wifiNative,
            /* [in] */ WifiMonitorSingleton* wifiMonitorSingleton);

        virtual CARAPI Run();

    private:
        WifiMonitor* mHost;
        AutoPtr<WifiNative> mWifiNative;
        AutoPtr<WifiMonitorSingleton> mWifiMonitorSingleton;
    };

public:
    WifiMonitor(
        /* [in] */ StateMachine* wifiStateMachine,
        /* [in] */ WifiNative* wifiNative);

    CARAPI_(void) EnableVerboseLogging(
        /* [in] */ Int32 verbose);

    CARAPI_(void) SetStateMachine2(
        /* [in] */ StateMachine* stateMachine);

    CARAPI_(void) StartMonitoring();

    CARAPI_(void) StopMonitoring();

    CARAPI_(void) StopSupplicant();

    CARAPI_(void) KillSupplicant(
        /* [in] */ Boolean p2pSupported);

    CARAPI_(void) HandleNetworkStateChange(
        /* [in] */ NetworkInfoDetailedState newState,
        /* [in] */ const String& data);

    /**
     * Send the state machine a notification that the state of Wifi connectivity
     * has changed.
     * @param networkId the configured network on which the state change occurred
     * @param newState the new network state
     * @param BSSID when the new state is {@link DetailedState#CONNECTED
     * NetworkInfo.DetailedState.CONNECTED},
     * this is the MAC address of the access point. Otherwise, it
     * is {@code null}.
     */
    CARAPI_(void) NotifyNetworkStateChange(
        /* [in] */ NetworkInfoDetailedState newState,
        /* [in] */ const String& BSSID,
        /* [in] */ Int32 netId,
        /* [in] */ Int32 reason);

    /**
     * Send the state machine a notification that the state of the supplicant
     * has changed.
     * @param networkId the configured network on which the state change occurred
     * @param wifiSsid network name
     * @param BSSID network address
     * @param newState the new {@code SupplicantState}
     */
    CARAPI_(void) NotifySupplicantStateChange(
        /* [in] */ Int32 networkId,
        /* [in] */ IWifiSsid* wifiSsid,
        /* [in] */ const String& BSSID,
        /* [in] */ SupplicantState newState);

    /**
    * Handle all supplicant events except STATE-CHANGE
    * @param event the event type
    * @param remainder the rest of the string following the
    * event name and &quot;&#8195;&#8212;&#8195;&quot;
    */
    CARAPI_(void) HandleEvent(
        /* [in] */ Int32 event,
        /* [in] */ const String& remainder);

private:

    CARAPI_(void) HandleDriverEvent(
        /* [in] */ const String& state);

    CARAPI_(void) HandleTargetSSIDEvent(
        /* [in] */ const String& eventStr);

    CARAPI_(void) HandleTargetBSSIDEvent(
        /* [in] */ const String& eventStr);

    CARAPI_(void) HandleAssociatedBSSIDEvent(
        /* [in] */ const String& eventStr);

    CARAPI_(void) HandleWpsFailEvent(
        /* [in] */ const String& dataString);

    //CARAPI_(P2pStatus) ValueOf(
    //    /* [in] */ Int32 error);

    ///* <event> status=<err> and the special case of <event> reason=FREQ_CONFLICT */
    //CARAPI_(P2pStatus) P2pError(
    //   /* [in] */ const String& dataString);

    /**
     * Handle p2p events
     */
    CARAPI_(void) HandleP2pEvents(
        /* [in] */ const String& dataString);

    /**
     * Handle hostap events
     */
    CARAPI_(void) HandleHostApEvents(
        /* [in] */ const String& dataString);

    CARAPI_(void) HandleGasQueryEvents(
        /* [in] */ const String& dataString);

    CARAPI_(void) HandleHs20Events(
        /* [in] */ const String& dataString);

    CARAPI_(void) HandleRequests(
        /* [in] */ const String& dataString);

    /**
     * Handle the supplicant STATE-CHANGE event
     * @param dataString New supplicant state string in the format:
     * id=network-id state=new-state
     */
    CARAPI_(void) HandleSupplicantStateChange(
        /* [in] */ const String& dataString);

    CARAPI_(void) LogDbg(
        /* [in] */ const String& debug);

    CARAPI_(Boolean) DispatchEvent(
        /* [in] */ const String& eventStr,
        /* [in] */ const String& iface);

private:
    /* Supplicant events reported to a state machine */
    static const Int32 BASE = IProtocol::BASE_WIFI_MONITOR;

public:
    /* Connection to supplicant established */
    static const Int32 SUP_CONNECTION_EVENT = BASE + 1;
    /* Connection to supplicant lost */
    static const Int32 SUP_DISCONNECTION_EVENT = BASE + 2;
   /* Network connection completed */
    static const Int32 NETWORK_CONNECTION_EVENT = BASE + 3;
    /* Network disconnection completed */
    static const Int32 NETWORK_DISCONNECTION_EVENT = BASE + 4;
    /* Scan results are available */
    static const Int32 SCAN_RESULTS_EVENT = BASE + 5;
    /* Supplicate state changed */
    static const Int32 SUPPLICANT_STATE_CHANGE_EVENT = BASE + 6;
    /* Password failure and EAP authentication failure */
    static const Int32 AUTHENTICATION_FAILURE_EVENT = BASE + 7;
    /* WPS success detected */
    static const Int32 WPS_SUCCESS_EVENT = BASE + 8;
    /* WPS failure detected */
    static const Int32 WPS_FAIL_EVENT = BASE + 9;
     /* WPS overlap detected */
    static const Int32 WPS_OVERLAP_EVENT = BASE + 10;
     /* WPS timeout detected */
    static const Int32 WPS_TIMEOUT_EVENT = BASE + 11;
    /* Driver was hung */
    static const Int32 DRIVER_HUNG_EVENT = BASE + 12;
    /* SSID was disabled due to auth failure or excessive
     * connection failures */
    static const Int32 SSID_TEMP_DISABLED = BASE + 13;
    /* SSID reenabled by supplicant */
    static const Int32 SSID_REENABLED = BASE + 14;

    /* Request Identity */
    static const Int32 SUP_REQUEST_IDENTITY = BASE + 15;

    /* Request SIM Auth */
    static const Int32 SUP_REQUEST_SIM_AUTH = BASE + 16;

    /* P2P events */
    static const Int32 P2P_DEVICE_FOUND_EVENT = BASE + 21;
    static const Int32 P2P_DEVICE_LOST_EVENT = BASE + 22;
    static const Int32 P2P_GO_NEGOTIATION_REQUEST_EVENT = BASE + 23;
    static const Int32 P2P_GO_NEGOTIATION_SUCCESS_EVENT = BASE + 25;
    static const Int32 P2P_GO_NEGOTIATION_FAILURE_EVENT = BASE + 26;
    static const Int32 P2P_GROUP_FORMATION_SUCCESS_EVENT = BASE + 27;
    static const Int32 P2P_GROUP_FORMATION_FAILURE_EVENT = BASE + 28;
    static const Int32 P2P_GROUP_STARTED_EVENT = BASE + 29;
    static const Int32 P2P_GROUP_REMOVED_EVENT = BASE + 30;
    static const Int32 P2P_INVITATION_RECEIVED_EVENT = BASE + 31;
    static const Int32 P2P_INVITATION_RESULT_EVENT = BASE + 32;
    static const Int32 P2P_PROV_DISC_PBC_REQ_EVENT = BASE + 33;
    static const Int32 P2P_PROV_DISC_PBC_RSP_EVENT = BASE + 34;
    static const Int32 P2P_PROV_DISC_ENTER_PIN_EVENT = BASE + 35;
    static const Int32 P2P_PROV_DISC_SHOW_PIN_EVENT = BASE + 36;
    static const Int32 P2P_FIND_STOPPED_EVENT = BASE + 37;
    static const Int32 P2P_SERV_DISC_RESP_EVENT = BASE + 38;
    static const Int32 P2P_PROV_DISC_FAILURE_EVENT = BASE + 39;
    static const Int32 P2P_REMOVE_AND_REFORM_GROUP_EVENT = BASE + 40;

    /* hostap events */
    static const Int32 AP_STA_DISCONNECTED_EVENT = BASE + 41;
    static const Int32 AP_STA_CONNECTED_EVENT = BASE + 42;

    /* Indicates assoc reject event */
    static const Int32 ASSOCIATION_REJECTION_EVENT = BASE + 43;

    /* hotspot 2.0 ANQP events */
    static const Int32 GAS_QUERY_START_EVENT = BASE + 51;
    static const Int32 GAS_QUERY_DONE_EVENT = BASE + 52;
    static const Int32 RX_HS20_ANQP_ICON_EVENT = BASE + 53;

    /* hotspot 2.0 events */
    static const Int32 HS20_REMEDIATION_EVENT = BASE + 61;
    static const Int32 HS20_DEAUTH_EVENT = BASE + 62;

private:
    static Boolean DBG;
    static const Boolean VDBG;
    static String TAG;

    /** Events we receive from the supplicant daemon */

    static const Int32 CONNECTED = 1;
    static const Int32 DISCONNECTED = 2;
    static const Int32 STATE_CHANGE = 3;
    static const Int32 SCAN_RESULTS = 4;
    static const Int32 LINK_SPEED = 5;
    static const Int32 TERMINATING = 6;
    static const Int32 DRIVER_STATE = 7;
    static const Int32 EAP_FAILURE = 8;
    static const Int32 ASSOC_REJECT = 9;
    static const Int32 SSID_TEMP_DISABLE = 10;
    static const Int32 SSID_REENABLE = 11;
    static const Int32 BSS_ADDED = 12;
    static const Int32 BSS_REMOVED = 13;
    static const Int32 UNKNOWN      = 14;

    /** All events coming from the supplicant start with this prefix */
    static String EVENT_PREFIX_STR;
    static const Int32 EVENT_PREFIX_LEN_STR;

    /** All events coming from the supplicant start with this prefix */
    static const String REQUEST_PREFIX_STR;// = "CTRL-REQ-";
    static const Int32 REQUEST_PREFIX_LEN_STR;// = REQUEST_PREFIX_STR.length();

    /** All WPA events coming from the supplicant start with this prefix */
    static String WPA_EVENT_PREFIX_STR;
    static String PASSWORD_MAY_BE_INCORRECT_STR;

    /* WPS events */
    static String WPS_SUCCESS_STR;

    /* Format: WPS-FAIL msg=%d [config_error=%d] [reason=%d (%s)] */
    static String WPS_FAIL_STR;
    static String WPS_FAIL_PATTERN;;

    /* config error code values for config_error=%d */
    static const Int32 CONFIG_MULTIPLE_PBC_DETECTED = 12;
    static const Int32 CONFIG_AUTH_FAILURE = 18;

    /* reason code values for reason=%d */
    static const Int32 REASON_TKIP_ONLY_PROHIBITED = 1;
    static const Int32 REASON_WEP_PROHIBITED = 2;

    static String WPS_OVERLAP_STR;
    static String WPS_TIMEOUT_STR;

    /* Hotspot 2.0 ANQP query events */
    static const String GAS_QUERY_PREFIX_STR;// = "GAS-QUERY-";
    static const String GAS_QUERY_START_STR;// = "GAS-QUERY-START";
    static const String GAS_QUERY_DONE_STR;// = "GAS-QUERY-DONE";
    static const String RX_HS20_ANQP_ICON_STR;// = "RX-HS20-ANQP-ICON";
    static const Int32 RX_HS20_ANQP_ICON_STR_LEN;// = RX_HS20_ANQP_ICON_STR.length();

    /* Hotspot 2.0 events */
    static const String HS20_PREFIX_STR;// = "HS20-";
    static const String HS20_SUB_REM_STR;// = "HS20-SUBSCRIPTION-REMEDIATION";
    static const String HS20_DEAUTH_STR;// = "HS20-DEAUTH-IMMINENT-NOTICE";

    static const String IDENTITY_STR;// = "IDENTITY";

    static const String SIM_STR;// = "SIM";


    //used to debug and detect if we miss an event
    static Int32 eventLogCounter;// = 0;

    /**
     * Names of events from wpa_supplicant (minus the prefix). In the
     * format descriptions, * &quot;<code>x</code>&quot;
     * designates a dynamic value that needs to be parsed out from the event
     * string
     */
    /**
     * <pre>
     * CTRL-EVENT-CONNECTED - Connection to xx:xx:xx:xx:xx:xx completed
     * </pre>
     * <code>xx:xx:xx:xx:xx:xx</code> is the BSSID of the associated access point
     */
    static String CONNECTED_STR;
    /**
     * <pre>
     * CTRL-EVENT-DISCONNECTED - Disconnect event - remove keys
     * </pre>
     */
    static String DISCONNECTED_STR;
    /**
     * <pre>
     * CTRL-EVENT-STATE-CHANGE x
     * </pre>
     * <code>x</code> is the numerical value of the new state.
     */
    static String STATE_CHANGE_STR;
    /**
     * <pre>
     * CTRL-EVENT-SCAN-RESULTS ready
     * </pre>
     */
    static String SCAN_RESULTS_STR;

    /**
     * <pre>
     * CTRL-EVENT-LINK-SPEED x Mb/s
     * </pre>
     * {@code x} is the link speed in Mb/sec.
     */
    static String LINK_SPEED_STR;
    /**
     * <pre>
     * CTRL-EVENT-TERMINATING - signal x
     * </pre>
     * <code>x</code> is the signal that caused termination.
     */
    static String TERMINATING_STR;
    /**
     * <pre>
     * CTRL-EVENT-DRIVER-STATE state
     * </pre>
     * <code>state</code> can be HANGED
     */
    static String DRIVER_STATE_STR;
    /**
     * <pre>
     * CTRL-EVENT-EAP-FAILURE EAP authentication failed
     * </pre>
     */
    static String EAP_FAILURE_STR;

    /**
     * This indicates an authentication failure on EAP FAILURE event
     */
    static String EAP_AUTH_FAILURE_STR;

    /**
      * This indicates an assoc reject event
      */
    static const String ASSOC_REJECT_STR;// = "ASSOC-REJECT";

    /**
      * This indicates auth or association failure bad enough so as network got disabled
      * - WPA_PSK auth failure suspecting shared key mismatch
      * - failed multiple Associations
      */
    static const String TEMP_DISABLED_STR;// = "SSID-TEMP-DISABLED";

    /**
      * This indicates a previously disabled SSID was reenabled by supplicant
      */
    static const String REENABLED_STR;// = "SSID-REENABLED";

    /**
     * This indicates supplicant found a given BSS
     */
    static const String BSS_ADDED_STR;// = "BSS-ADDED";

    /**
      * This indicates supplicant removed a given BSS
      */
    static const String BSS_REMOVED_STR;// = "BSS-REMOVED";

    /**
     * Regex pattern for extracting an Ethernet-style MAC address from a string.
     * Matches a strings like the following:<pre>
     * CTRL-EVENT-CONNECTED - Connection to 00:1e:58:ec:d5:6d completed (reauth) [id=1 id_str=]</pre>
     */
    static AutoPtr<IPattern> mConnectedEventPattern;

    /**
      * Regex pattern for extracting an Ethernet-style MAC address from a string.
      * Matches a strings like the following:<pre>
      * CTRL-EVENT-DISCONNECTED - bssid=ac:22:0b:24:70:74 reason=3 locally_generated=1
      */
    static AutoPtr<IPattern> mDisconnectedEventPattern;

    /**
      * Regex pattern for extracting an Ethernet-style MAC address from a string.
      * Matches a strings like the following:<pre>
      * CTRL-EVENT-ASSOC-REJECT - bssid=ac:22:0b:24:70:74 status_code=1
      */
    static AutoPtr<IPattern> mAssocRejectEventPattern;

    /**
      * Regex pattern for extracting an Ethernet-style MAC address from a string.
      * Matches a strings like the following:<pre>
      * IFNAME=wlan0 Trying to associate with 6c:f3:7f:ae:87:71
      */
    static const String TARGET_BSSID_STR;// =  "Trying to associate with ";

    static AutoPtr<IPattern> mTargetBSSIDPattern;

    /**
     * Regex pattern for extracting an SSID from a string.
     * Matches a strings like the following:<pre>
     * IFNAME=wlan0 Trying to associate with SSID 'ssidname'
     */
    static const String TARGET_SSID_STR;// = "Trying to associate with SSID ";

    static AutoPtr<IPattern> mTargetSSIDPattern;

    /**
      * Regex pattern for extracting an Ethernet-style MAC address from a string.
      * Matches a strings like the following:<pre>
      * IFNAME=wlan0 Associated with 6c:f3:7f:ae:87:71
      */
    static const String ASSOCIATED_WITH_STR;// =  "Associated with ";

    static AutoPtr<IPattern> mAssociatedPattern;

    /**
      * Regex pattern for extracting SSIDs from request identity string.
      * Matches a strings like the following:<pre>
      * CTRL-REQ-SIM-<network id>:GSM-AUTH:<RAND1>:<RAND2>[:<RAND3>] needed for SSID <SSID>
      * This pattern should find
      *    0 - id
      *    1 - Rand1
      *    2 - Rand2
      *    3 - Rand3
      *    4 - SSID
      */
    static AutoPtr<IPattern> mRequestGsmAuthPattern;

    /**
      * Regex pattern for extracting SSIDs from request identity string.
      * Matches a strings like the following:<pre>
      * CTRL-REQ-IDENTITY-xx:Identity needed for SSID XXXX</pre>
      */
    static AutoPtr<IPattern> mRequestIdentityPattern;

    /** P2P events */
    static String P2P_EVENT_PREFIX_STR;

    /* P2P-DEVICE-FOUND fa:7b:7a:42:02:13 p2p_dev_addr=fa:7b:7a:42:02:13 pri_dev_type=1-0050F204-1
       name='p2p-TEST1' config_methods=0x188 dev_capab=0x27 group_capab=0x0 */
    static String P2P_DEVICE_FOUND_STR;

    /* P2P-DEVICE-LOST p2p_dev_addr=42:fc:89:e1:e2:27 */
    static String P2P_DEVICE_LOST_STR;

    /* P2P-FIND-STOPPED */
    static String P2P_FIND_STOPPED_STR;

    /* P2P-GO-NEG-REQUEST 42:fc:89:a8:96:09 dev_passwd_id=4 */
    static String P2P_GO_NEG_REQUEST_STR;

    static String P2P_GO_NEG_SUCCESS_STR;

    /* P2P-GO-NEG-FAILURE status=x */
    static String P2P_GO_NEG_FAILURE_STR;

    static String P2P_GROUP_FORMATION_SUCCESS_STR;;

    static String P2P_GROUP_FORMATION_FAILURE_STR;;

    /* P2P-GROUP-STARTED p2p-wlan0-0 [client|GO] ssid="DIRECT-W8" freq=2437
       [psk=2182b2e50e53f260d04f3c7b25ef33c965a3291b9b36b455a82d77fd82ca15bc|passphrase="fKG4jMe3"]
       go_dev_addr=fa:7b:7a:42:02:13 [PERSISTENT] */
    static String P2P_GROUP_STARTED_STR;

    /* P2P-GROUP-REMOVED p2p-wlan0-0 [client|GO] reason=REQUESTED */
    static String P2P_GROUP_REMOVED_STR;

    /* P2P-INVITATION-RECEIVED sa=fa:7b:7a:42:02:13 go_dev_addr=f8:7b:7a:42:02:13
        bssid=fa:7b:7a:42:82:13 unknown-network */
    static String P2P_INVITATION_RECEIVED_STR;

    /* P2P-INVITATION-RESULT status=1 */
    static String P2P_INVITATION_RESULT_STR;

    /* P2P-PROV-DISC-PBC-REQ 42:fc:89:e1:e2:27 p2p_dev_addr=42:fc:89:e1:e2:27
       pri_dev_type=1-0050F204-1 name='p2p-TEST2' config_methods=0x188 dev_capab=0x27
       group_capab=0x0 */
    static String P2P_PROV_DISC_PBC_REQ_STR;

    /* P2P-PROV-DISC-PBC-RESP 02:12:47:f2:5a:36 */
    static String P2P_PROV_DISC_PBC_RSP_STR;

    /* P2P-PROV-DISC-ENTER-PIN 42:fc:89:e1:e2:27 p2p_dev_addr=42:fc:89:e1:e2:27
       pri_dev_type=1-0050F204-1 name='p2p-TEST2' config_methods=0x188 dev_capab=0x27
       group_capab=0x0 */
    static String P2P_PROV_DISC_ENTER_PIN_STR;
    /* P2P-PROV-DISC-SHOW-PIN 42:fc:89:e1:e2:27 44490607 p2p_dev_addr=42:fc:89:e1:e2:27
       pri_dev_type=1-0050F204-1 name='p2p-TEST2' config_methods=0x188 dev_capab=0x27
       group_capab=0x0 */
    static String P2P_PROV_DISC_SHOW_PIN_STR;
    /* P2P-PROV-DISC-FAILURE p2p_dev_addr=42:fc:89:e1:e2:27 */
    static String P2P_PROV_DISC_FAILURE_STR;

    /*
     * Protocol format is as follows.<br>
     * See the Table.62 in the WiFi Direct specification for the detail.
     * ______________________________________________________________
     * |           Length(2byte)     | Type(1byte) | TransId(1byte)}|
     * ______________________________________________________________
     * | status(1byte)  |            vendor specific(variable)      |
     *
     * P2P-SERV-DISC-RESP 42:fc:89:e1:e2:27 1 0300000101
     * length=3, service type=0(ALL Service), transaction id=1,
     * status=1(service protocol type not available)<br>
     *
     * P2P-SERV-DISC-RESP 42:fc:89:e1:e2:27 1 0300020201
     * length=3, service type=2(UPnP), transaction id=2,
     * status=1(service protocol type not available)
     *
     * P2P-SERV-DISC-RESP 42:fc:89:e1:e2:27 1 990002030010757569643a3131323
     * 2646534652d383537342d353961622d393332322d3333333435363738393034343a3
     * a75726e3a736368656d61732d75706e702d6f72673a736572766963653a436f6e746
     * 56e744469726563746f72793a322c757569643a36383539646564652d383537342d3
     * 53961622d393333322d3132333435363738393031323a3a75706e703a726f6f74646
     * 576696365
     * length=153,type=2(UPnP),transaction id=3,status=0
     *
     * UPnP Protocol format is as follows.
     * ______________________________________________________
     * |  Version (1)  |          USN (Variable)            |
     *
     * version=0x10(UPnP1.0) data=usn:uuid:1122de4e-8574-59ab-9322-33345678
     * 9044::urn:schemas-upnp-org:service:ContentDirectory:2,usn:uuid:6859d
     * ede-8574-59ab-9332-123456789012::upnp:rootdevice
     *
     * P2P-SERV-DISC-RESP 58:17:0c:bc:dd:ca 21 1900010200045f6970
     * 70c00c000c01094d795072696e746572c027
     * length=25, type=1(Bonjour),transaction id=2,status=0
     *
     * Bonjour Protocol format is as follows.
     * __________________________________________________________
     * |DNS Name(Variable)|DNS Type(1)|Version(1)|RDATA(Variable)|
     *
     * DNS Name=_ipp._tcp.local.,DNS type=12(PTR), Version=1,
     * RDATA=MyPrinter._ipp._tcp.local.
     *
     */
    static String P2P_SERV_DISC_RESP_STR;

    /* P2P-REMOVE-AND-REFORM-GROUP */
    /* Supplicant is supposed to generate this event only when p2p
     * is connected
     */
    static const String P2P_REMOVE_AND_REFORM_GROUP_STR;// = "P2P-REMOVE-AND-REFORM-GROUP";

    static String HOST_AP_EVENT_PREFIX_STR;
    /* AP-STA-CONNECTED 42:fc:89:a8:96:09 dev_addr=02:90:4c:a0:92:54 */
    static String AP_STA_CONNECTED_STR;
    /* AP-STA-DISCONNECTED 42:fc:89:a8:96:09 */
    static String AP_STA_DISCONNECTED_STR;

    /**
     * This indicates a read error on the monitor socket conenction
     */
    static String WPA_RECV_ERROR_STR;

    /**
     * Max errors before we close supplicant connection
     */
    static const Int32 MAX_RECV_ERRORS = 10;

    String mInterfaceName;
    AutoPtr<WifiNative> mWifiNative;
    AutoPtr<StateMachine> mStateMachine;
    AutoPtr<StateMachine> mStateMachine2;
    Boolean mMonitoring;

    // This is a global counter, since it's not monitor specific. However, the existing
    // implementation forwards all "global" control events like CTRL-EVENT-TERMINATING
    // to the p2p0 monitor. Is that expected ? It seems a bit surprising.
    //
    // TODO: If the p2p0 monitor isn't registered, the behaviour is even more surprising.
    // The event will be dispatched to all monitors, and each of them will end up incrementing
    // it in their dispatchXXX method. If we have 5 registered monitors (say), 2 consecutive
    // recv errors will cause us to disconnect from the supplicant (instead of the intended 10).
    //
    // This variable is always accessed and modified under a WifiMonitorSingleton lock.
    static Int32 sRecvErrors;
};

} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_WIFI_WIFIMONITOR_H__
