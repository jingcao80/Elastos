
#ifndef __ELASTOS_DROID_NET_WIFI_WIFIMONITOR_H__
#define  __ELASTOS_DROID_NET_WIFI_WIFIMONITOR_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/net/wifi/WifiNative.h"
#include "elastos/droid/utility/StateMachine.h"

using Elastos::Droid::Wifi::P2p::P2pStatus;
using Elastos::Utility::Regex::IPattern;
using Elastos::Droid::Utility::IProtocol;
using Elastos::Droid::Internal::Utility::StateMachine;

namespace Elastos {
namespace Droid {
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
    class MonitorThread
        : public ThreadBase
    {
    public:
        MonitorThread(
            /* [in] */ WifiMonitor* owner);

        virtual CARAPI Run();

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
        CARAPI_(Boolean) ConnectToSupplicant();

        CARAPI_(void) HandleDriverEvent(
            /* [in] */ const String& state);

        CARAPI_(void) HandleWpsFailEvent(
            /* [in] */ const String& dataString);

        CARAPI_(P2pStatus) ValueOf(
            /* [in] */ Int32 error);

        /* <event> status=<err> and the special case of <event> reason=FREQ_CONFLICT */
       CARAPI_(P2pStatus) P2pError(
           /* [in] */ const String& dataString);

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

        /**
         * Handle the supplicant STATE-CHANGE event
         * @param dataString New supplicant state string in the format:
         * id=network-id state=new-state
         */
        CARAPI_(void) HandleSupplicantStateChange(
            /* [in] */ const String& dataString);

    private:
        WifiMonitor* mHost;
    };

public:
    WifiMonitor(
        /* [in] */ StateMachine* wifiStateMachine,
        /* [in] */ WifiNative* wifiNative);

    CARAPI_(void) StartMonitoring();

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
        /* [in] */ Int32 netId);

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
     * Sleep for a period of time.
     * @param secs the number of seconds to sleep
     */
    static CARAPI_(void) Nap(
        /* [in] */ Int32 secs);

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

    /* hostap events */
    static const Int32 AP_STA_DISCONNECTED_EVENT = BASE + 41;
    static const Int32 AP_STA_CONNECTED_EVENT = BASE + 42;

private:
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
    static const Int32 UNKNOWN = 9;

    /** All events coming from the supplicant start with this prefix */
    static String EVENT_PREFIX_STR;
    static const Int32 EVENT_PREFIX_LEN_STR;

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
     * Regex pattern for extracting an Ethernet-style MAC address from a string.
     * Matches a strings like the following:<pre>
     * CTRL-EVENT-CONNECTED - Connection to 00:1e:58:ec:d5:6d completed (reauth) [id=1 id_str=]</pre>
     */
    static AutoPtr<IPattern> mConnectedEventPattern;

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

    static String HOST_AP_EVENT_PREFIX_STR;
    /* AP-STA-CONNECTED 42:fc:89:a8:96:09 dev_addr=02:90:4c:a0:92:54 */
    static String AP_STA_CONNECTED_STR;
    /* AP-STA-DISCONNECTED 42:fc:89:a8:96:09 */
    static String AP_STA_DISCONNECTED_STR;

    AutoPtr<StateMachine> mStateMachine;
    AutoPtr<WifiNative> mWifiNative;

    /**
     * This indicates the supplicant connection for the monitor is closed
     */
    static String MONITOR_SOCKET_CLOSED_STR;

    /**
     * This indicates a read error on the monitor socket conenction
     */
    static String WPA_RECV_ERROR_STR;

    /**
     * Tracks consecutive receive errors
     */
    Int32 mRecvErrors;

    /**
     * Max errors before we close supplicant connection
     */
    static const Int32 MAX_RECV_ERRORS = 10;
};

} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_NET_WIFI_WIFIMONITOR_H__
