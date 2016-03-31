
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/wifi/WifiMonitor.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/StringUtils.h>
#include <cutils/properties.h>
#ifdef DROID_CORE
#include "elastos/droid/wifi/CWifiSsidHelper.h"
#include "elastos/droid/wifi/CStateChangeResult.h"
#include "elastos/droid/wifi/p2p/CWifiP2pDevice.h"
#include "elastos/droid/wifi/p2p/CWifiP2pConfig.h"
#include "elastos/droid/wifi/p2p/CWifiP2pGroup.h"
#include "elastos/droid/wifi/p2p/CWifiP2pProvDiscEvent.h"
#include "elastos/droid/wifi/p2p/nsd/CWifiP2pServiceResponse.h"
#include "elastos/droid/wifi/p2p/nsd/CWifiP2pServiceResponseHelper.h"
#endif
#include "elastos/droid/utility/ArrayUtils.h"
#include "elastos/core/AutoLock.h"

using Elastos::Core::AutoLock;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Core::StringUtils;
using Elastos::Core::IInteger32;
using Elastos::Core::CInteger32;
using Elastos::Utility::Regex::IPatternHelper;
using Elastos::Utility::Regex::CPatternHelper;
using Elastos::Utility::Regex::IMatcher;
using Elastos::Utility::Logging::Logger;
using Elastos::Droid::Wifi::P2p::P2pStatus_UNKNOWN;
using Elastos::Droid::Wifi::P2p::IWifiP2pDevice;
using Elastos::Droid::Wifi::P2p::CWifiP2pDevice;
using Elastos::Droid::Wifi::P2p::IWifiP2pConfig;
using Elastos::Droid::Wifi::P2p::CWifiP2pConfig;
using Elastos::Droid::Wifi::P2p::IWifiP2pGroup;
using Elastos::Droid::Wifi::P2p::CWifiP2pGroup;
using Elastos::Droid::Wifi::P2p::IWifiP2pProvDiscEvent;
using Elastos::Droid::Wifi::P2p::CWifiP2pProvDiscEvent;
using Elastos::Droid::Wifi::P2p::Nsd::IWifiP2pServiceResponse;
using Elastos::Droid::Wifi::P2p::Nsd::CWifiP2pServiceResponse;
using Elastos::Droid::Wifi::P2p::Nsd::IWifiP2pServiceResponseHelper;
using Elastos::Droid::Wifi::P2p::Nsd::CWifiP2pServiceResponseHelper;
using Elastos::Droid::Internal::Utility::ArrayUtils;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wifi {

static AutoPtr<IPattern> InitPattern(const String& patternStr)
{
    AutoPtr<IPatternHelper> helper;
    CPatternHelper::AcquireSingleton((IPatternHelper**)&helper);
    AutoPtr<IPattern> p;
    helper->Compile(patternStr, (IPattern**)&p);
    return p;
}

Boolean WifiMonitor::DBG = FALSE;
const Boolean WifiMonitor::VDBG = FALSE;
String WifiMonitor::TAG("WifiMonitor");
static const Boolean DBG = FALSE;

const Int32 WifiMonitor::BASE;
const Int32 WifiMonitor::SUP_CONNECTION_EVENT;
const Int32 WifiMonitor::SUP_DISCONNECTION_EVENT;
const Int32 WifiMonitor::NETWORK_CONNECTION_EVENT;
const Int32 WifiMonitor::NETWORK_DISCONNECTION_EVENT;
const Int32 WifiMonitor::SCAN_RESULTS_EVENT;
const Int32 WifiMonitor::SUPPLICANT_STATE_CHANGE_EVENT;
const Int32 WifiMonitor::AUTHENTICATION_FAILURE_EVENT;
const Int32 WifiMonitor::WPS_SUCCESS_EVENT;
const Int32 WifiMonitor::WPS_FAIL_EVENT;
const Int32 WifiMonitor::WPS_OVERLAP_EVENT;
const Int32 WifiMonitor::WPS_TIMEOUT_EVENT;
const Int32 WifiMonitor::DRIVER_HUNG_EVENT;
const Int32 WifiMonitor::SSID_TEMP_DISABLED;
const Int32 WifiMonitor::SSID_REENABLED;
const Int32 WifiMonitor::SUP_REQUEST_IDENTITY;
const Int32 WifiMonitor::SUP_REQUEST_SIM_AUTH;

const Int32 WifiMonitor::P2P_DEVICE_FOUND_EVENT;
const Int32 WifiMonitor::P2P_DEVICE_LOST_EVENT;
const Int32 WifiMonitor::P2P_GO_NEGOTIATION_REQUEST_EVENT;
const Int32 WifiMonitor::P2P_GO_NEGOTIATION_SUCCESS_EVENT;
const Int32 WifiMonitor::P2P_GO_NEGOTIATION_FAILURE_EVENT;
const Int32 WifiMonitor::P2P_GROUP_FORMATION_SUCCESS_EVENT;
const Int32 WifiMonitor::P2P_GROUP_FORMATION_FAILURE_EVENT;
const Int32 WifiMonitor::P2P_GROUP_STARTED_EVENT;
const Int32 WifiMonitor::P2P_GROUP_REMOVED_EVENT;
const Int32 WifiMonitor::P2P_INVITATION_RECEIVED_EVENT;
const Int32 WifiMonitor::P2P_INVITATION_RESULT_EVENT;
const Int32 WifiMonitor::P2P_PROV_DISC_PBC_REQ_EVENT;
const Int32 WifiMonitor::P2P_PROV_DISC_PBC_RSP_EVENT;
const Int32 WifiMonitor::P2P_PROV_DISC_ENTER_PIN_EVENT;
const Int32 WifiMonitor::P2P_PROV_DISC_SHOW_PIN_EVENT;
const Int32 WifiMonitor::P2P_FIND_STOPPED_EVENT;
const Int32 WifiMonitor::P2P_SERV_DISC_RESP_EVENT;
const Int32 WifiMonitor::P2P_PROV_DISC_FAILURE_EVENT;
const Int32 WifiMonitor::P2P_REMOVE_AND_REFORM_GROUP_EVENT;
const Int32 WifiMonitor::AP_STA_DISCONNECTED_EVENT;
const Int32 WifiMonitor::AP_STA_CONNECTED_EVENT;

const Int32 WifiMonitor::ASSOCIATION_REJECTION_EVENT;

const Int32 WifiMonitor::GAS_QUERY_START_EVENT;
const Int32 WifiMonitor::GAS_QUERY_DONE_EVENT;
const Int32 WifiMonitor::RX_HS20_ANQP_ICON_EVENT;

const Int32 WifiMonitor::HS20_REMEDIATION_EVENT;
const Int32 WifiMonitor::HS20_DEAUTH_EVENT;

const Int32 WifiMonitor::CONNECTED;
const Int32 WifiMonitor::DISCONNECTED;
const Int32 WifiMonitor::STATE_CHANGE;
const Int32 WifiMonitor::SCAN_RESULTS;
const Int32 WifiMonitor::LINK_SPEED;
const Int32 WifiMonitor::TERMINATING;
const Int32 WifiMonitor::DRIVER_STATE;
const Int32 WifiMonitor::EAP_FAILURE;
const Int32 WifiMonitor::ASSOC_REJECT;
const Int32 WifiMonitor::SSID_TEMP_DISABLE;
const Int32 WifiMonitor::SSID_REENABLE;
const Int32 WifiMonitor::BSS_ADDED;
const Int32 WifiMonitor::BSS_REMOVED;
const Int32 WifiMonitor::UNKNOWN;

String WifiMonitor::EVENT_PREFIX_STR("CTRL-EVENT-");
const Int32 WifiMonitor::EVENT_PREFIX_LEN_STR = EVENT_PREFIX_STR.GetLength();
const String WifiMonitor::REQUEST_PREFIX_STR("CTRL-REQ-");
const Int32 WifiMonitor::REQUEST_PREFIX_LEN_STR = REQUEST_PREFIX_STR.GetLength();
String WifiMonitor::WPA_EVENT_PREFIX_STR("WPA:");
String WifiMonitor::PASSWORD_MAY_BE_INCORRECT_STR("pre-shared key may be incorrect");
String WifiMonitor::WPS_SUCCESS_STR("WPS-SUCCESS");
String WifiMonitor::WPS_FAIL_STR("WPS-FAIL");
String WifiMonitor::WPS_FAIL_PATTERN("WPS-FAIL msg=\\d+(?: config_error=(\\d+))?(?: reason=(\\d+))?");

const Int32 WifiMonitor::CONFIG_MULTIPLE_PBC_DETECTED;
const Int32 WifiMonitor::CONFIG_AUTH_FAILURE;
const Int32 WifiMonitor::REASON_TKIP_ONLY_PROHIBITED;
const Int32 WifiMonitor::REASON_WEP_PROHIBITED;

String WifiMonitor::WPS_OVERLAP_STR("WPS-OVERLAP-DETECTED");
String WifiMonitor::WPS_TIMEOUT_STR("WPS-TIMEOUT");
const String WifiMonitor::GAS_QUERY_PREFIX_STR("GAS-QUERY-");
const String WifiMonitor::GAS_QUERY_START_STR("GAS-QUERY-START");
const String WifiMonitor::GAS_QUERY_DONE_STR("GAS-QUERY-DONE");
const String WifiMonitor::RX_HS20_ANQP_ICON_STR("RX-HS20-ANQP-ICON");
const Int32 WifiMonitor::RX_HS20_ANQP_ICON_STR_LEN = WifiMonitor::RX_HS20_ANQP_ICON_STR->GetLength();

const String WifiMonitor::HS20_PREFIX_STR("HS20-");
const String WifiMonitor::HS20_SUB_REM_STR("HS20-SUBSCRIPTION-REMEDIATION");
const String WifiMonitor::HS20_DEAUTH_STR("HS20-DEAUTH-IMMINENT-NOTICE");

const String WifiMonitor::IDENTITY_STR("IDENTITY");

const String WifiMonitor::SIM_STR("SIM");

Int32 WifiMonitor::eventLogCounter = 0;

String WifiMonitor::CONNECTED_STR("CONNECTED");
String WifiMonitor::DISCONNECTED_STR("DISCONNECTED");
String WifiMonitor::STATE_CHANGE_STR("STATE-CHANGE");
String WifiMonitor::SCAN_RESULTS_STR("SCAN-RESULTS");
String WifiMonitor::LINK_SPEED_STR("LINK-SPEED");
String WifiMonitor::TERMINATING_STR("TERMINATING");
String WifiMonitor::DRIVER_STATE_STR("DRIVER-STATE");
String WifiMonitor::EAP_FAILURE_STR("EAP-FAILURE");
String WifiMonitor::EAP_AUTH_FAILURE_STR("EAP authentication failed");

const String WifiMonitor::ASSOC_REJECT_STR("ASSOC-REJECT");
const String WifiMonitor::TEMP_DISABLED_STR("SSID-TEMP-DISABLED");
const String WifiMonitor::REENABLED_STR("SSID-REENABLED");
const String WifiMonitor::BSS_ADDED_STR("BSS-ADDED");
const String WifiMonitor::BSS_REMOVED_STR("BSS-REMOVED");

String WifiMonitor::P2P_EVENT_PREFIX_STR("P2P");
String WifiMonitor::P2P_DEVICE_FOUND_STR("P2P-DEVICE-FOUND");
String WifiMonitor::P2P_DEVICE_LOST_STR("P2P-DEVICE-LOST");
String WifiMonitor::P2P_FIND_STOPPED_STR("P2P-FIND-STOPPED");
String WifiMonitor::P2P_GO_NEG_REQUEST_STR("P2P-GO-NEG-REQUEST");
String WifiMonitor::P2P_GO_NEG_SUCCESS_STR ("P2P-GO-NEG-SUCCESS");
String WifiMonitor::P2P_GO_NEG_FAILURE_STR("P2P-GO-NEG-FAILURE");
String WifiMonitor::P2P_GROUP_FORMATION_SUCCESS_STR("P2P-GROUP-FORMATION-SUCCESS");
String WifiMonitor::P2P_GROUP_FORMATION_FAILURE_STR("P2P-GROUP-FORMATION-FAILURE");
String WifiMonitor::P2P_GROUP_STARTED_STR("P2P-GROUP-STARTED");
String WifiMonitor::P2P_GROUP_REMOVED_STR("P2P-GROUP-REMOVED");
String WifiMonitor::P2P_INVITATION_RECEIVED_STR("P2P-INVITATION-RECEIVED");
String WifiMonitor::P2P_INVITATION_RESULT_STR("P2P-INVITATION-RESULT");
String WifiMonitor::P2P_PROV_DISC_PBC_REQ_STR ("P2P-PROV-DISC-PBC-REQ");
String WifiMonitor::P2P_PROV_DISC_PBC_RSP_STR("P2P-PROV-DISC-PBC-RESP");
String WifiMonitor::P2P_PROV_DISC_ENTER_PIN_STR("P2P-PROV-DISC-ENTER-PIN");
String WifiMonitor::P2P_PROV_DISC_SHOW_PIN_STR("P2P-PROV-DISC-SHOW-PIN");
String WifiMonitor::P2P_PROV_DISC_FAILURE_STR("P2P-PROV-DISC-FAILURE");
String WifiMonitor::P2P_SERV_DISC_RESP_STR("P2P-SERV-DISC-RESP");
const String WifiMonitor::P2P_REMOVE_AND_REFORM_GROUP_STR("P2P-REMOVE-AND-REFORM-GROUP");
String WifiMonitor::HOST_AP_EVENT_PREFIX_STR("AP");
String WifiMonitor::AP_STA_CONNECTED_STR("AP-STA-CONNECTED");
String WifiMonitor::AP_STA_DISCONNECTED_STR("AP-STA-DISCONNECTED");
//String WifiMonitor::MONITOR_SOCKET_CLOSED_STR("connection closed");
String WifiMonitor::WPA_RECV_ERROR_STR("recv error");

const Int32 WifiMonitor::MAX_RECV_ERRORS;
Int32 WifiMonitor::sRecvErrors;

AutoPtr<IPattern> WifiMonitor::mConnectedEventPattern = InitPattern(String("((?:[0-9a-f]{2}:){5}[0-9a-f]{2}) .* \\[id=([0-9]+) "));
AutoPtr<IPattern> WifiMonitor::mDisconnectedEventPattern =
                                InitPattern(String("((?:[0-9a-f]{2}:){5}[0-9a-f]{2}) +reason=([0-9]+) +locally_generated=([0-1])"));
AutoPtr<IPattern> WifiMonitor::mAssocRejectEventPattern =
                                InitPattern(String("((?:[0-9a-f]{2}:){5}[0-9a-f]{2}) +status_code=([0-9]+)"));
const String WifiMonitor::TARGET_BSSID_STR("Trying to associate with ");
AutoPtr<IPattern> WifiMonitor::mTargetBSSIDPattern =
                                InitPattern(String("Trying to associate with ((?:[0-9a-f]{2}:){5}[0-9a-f]{2}).*"));
const String WifiMonitor::TARGET_SSID_STR("Trying to associate with SSID ");
AutoPtr<IPattern> WifiMonitor::mTargetSSIDPattern =
                                InitPattern(String("Trying to associate with SSID '(.*)'"));
const String WifiMonitor::ASSOCIATED_WITH_STR("Associated with ");
AutoPtr<IPattern> WifiMonitor::mAssociatedPattern = InitPattern(String("Associated with ((?:[0-9a-f]{2}:){5}[0-9a-f]{2}).*"));
AutoPtr<IPattern> WifiMonitor::mRequestGsmAuthPattern = InitPattern(String("SIM-([0-9]*):GSM-AUTH((:[0-9a-f]+)+) needed for SSID (.+)"));
AutoPtr<IPattern> WifiMonitor::mRequestIdentityPattern = InitPattern(String("IDENTITY-([0-9]+):Identity needed for SSID (.+)"));

//================================================================================
// WifiMonitor::WifiMonitorSingleton
//================================================================================
AutoPtr<WifiMonitorSingleton sInstance = new WifiMonitorSingleton();

WifiMonitor::WifiMonitorSingleton::WifiMonitorSingleton()
{
    mConnected = FALSE;
}

void WifiMonitor::WifiMonitorSingleton::startMonitoring(
    /* [in] */ const String& iface)
{
    AutoPtr<AutoLock> lock(mLock);
    AutoPtr<WifiMonitor> m = mIfaceMap[iface];
    if (m == NULL) {
        Logger::E(WifiMonitor::TAG, "startMonitor called with unknown iface=%s", iface.string());
        return;
    }

    Logger::D(WifiMonitor::TAG, "startMonitoring(%s) with mConnected = %d", iface, mConnected);

    if (mConnected) {
        m->mMonitoring = TRUE;
        m->mStateMachine->SendMessage(WifiMonitor::SUP_CONNECTION_EVENT);
    }
    else {
        if (DBG) Logger::D(TAG, "connecting to supplicant");
        Int32 connectTries = 0;
        while (TRUE) {
            if (mWifiNative->ConnectToSupplicant()) {
                m->mMonitoring = TRUE;
                m->mStateMachine->SendMessage(WifiMonitor::SUP_CONNECTION_EVENT);
                new MonitorThread(mWifiNative, this)->Start();
                mConnected = TRUE;
                break;
            }
            if (connectTries++ < 50) {
                //try {
                    Thread::Sleep(100);
                //} catch (InterruptedException ignore) {
                //}
            } else {
                m->mStateMachine->SendMessage(SUP_DISCONNECTION_EVENT);
                Logger::E(TAG, "startMonitoring(%s) failed!", iface);
                break;
            }
        }
    }
}

void WifiMonitor::WifiMonitorSingleton::StopMonitoring(
    /* [in] */ const String& iface)
{
    AutoPtr<AutoLock> lock(mLock);
    AutoPtr<WifiMonitor> m = mIfaceMap[iface];
    if (DBG) Logger::D(TAG, "stopMonitoring(%s) = ", iface.string());//m.mStateMachine);
    m->mMonitoring = FALSE;
    m->mStateMachine->SendMessage(SUP_DISCONNECTION_EVENT);
}

void WifiMonitor::WifiMonitorSingleton::RegisterInterfaceMonitor(
    /* [in] */ const String& iface,
    /* [in] */ WifiMonitor* m)
{
    AutoPtr<AutoLock> lock(mLock);
    if (DBG) Logger::D(TAG, "registerInterface(%s)", iface.string());//+" + m.mStateMachine + ")");
    mIfaceMap[iface] = m;
    if (mWifiNative == NULL) {
        mWifiNative = m->mWifiNative;
    }
}

void WifiMonitor::WifiMonitorSingleton::UnregisterInterfaceMonitor(
    /* [in] */ const String& iface)
{
    // REVIEW: When should we call this? If this isn't called, then WifiMonitor
    // objects will remain in the mIfaceMap; and won't ever get deleted
    AutoPtr<AutoLock> lock(mLock);
    AutoPtr<WifiMonitor> m = mIfaceMap[iface];
    mIfaceMap.Erase(iface);
    if (DBG) Logger::D(TAG, "unregisterInterface(%s)", iface.string());// "+" + m.mStateMachine + ")");
}

void WifiMonitor::WifiMonitorSingleton::StopSupplicant()
{
    AutoPtr<AutoLock> lock(mLock);
    mWifiNative->StopSupplicant();
}

void WifiMonitor::WifiMonitorSingleton::KillSupplicant(
    /* [in] */ Boolean p2pSupported)
{
    AutoPtr<AutoLock> lock(mLock);
    WifiNative::KillSupplicant(p2pSupported);
    mConnected = FALSE;
    HashMap<String, AutoPtr<WifiMonitor> >::Iterator iter = mIfaceMap.Begin();
    for (; iter != mIfaceMap.End(); ++iter){
        AutoPtr<WifiMonitor> m = iter->mSecond;
        m->mMonitoring = FALSE;
    }
}

Boolean WifiMonitor::WifiMonitorSingleton::DispatchEvent(
    /* [in] */ const String& eventStr)
{
    AutoPtr<AutoLock> lock(mLock);
    String iface;
    if (eventStr.StartWith("IFNAME=")) {
        Int32 space = eventStr.IndexOf(' ');
        if (space != -1) {
            iface = eventStr.Substring(7, space);
            HashMap<String, AutoPtr<WifiMonitor> >::Iterator iter = mIfaceMap.Find(iface);
            if (iter == mIfaceMap.End() && iface.StartsWith("p2p-")) {
                // p2p interfaces are created dynamically, but we have
                // only one P2p state machine monitoring all of them; look
                // for it explicitly, and send messages there ..
                iface = "p2p0";
            }
            eventStr = eventStr.Substring(space + 1);
        } else {
            // No point dispatching this event to any interface, the dispatched
            // event string will begin with "IFNAME=" which dispatchEvent can't really
            // do anything about.
            Logger::E(TAG, "Dropping malformed event (unparsable iface): %s", eventStr.string());
            return FALSE;
        }
    } else {
        // events without prefix belong to p2p0 monitor
        iface = "p2p0";
    }

    if (VDBG) Logger::D(TAG, "Dispatching event to interface: %s", iface.string());

    AutoPtr<WifiMonitor> m = mIfaceMap[iface];
    if (m != NULL) {
        if (m->mMonitoring) {
            if (m->DispatchEvent(eventStr, iface)) {
                mConnected = FALSE;
                return TRUE;
            }

            return FALSE;
        } else {
            if (DBG) Logger::D(TAG, "Dropping event because (%s) is stopped", iface.string());
            return FALSE;
        }
    } else {
        if (DBG) Logger::D(TAG, "Sending to all monitors because there's no matching iface");
        Boolean done = FALSE;
        HashMap<String, AutoPtr<WifiMonitor> >::Iterator iter = mIfaceMap.Begin();
        for (; iter != mIfaceMap.End(); ++iter){
            AutoPtr<WifiMonitor> m = iter->mSecond;
            if (m->mMonitoring && m->DispatchEvent(eventStr, iface)) {
                done = TRUE;
            }
        }

        if (done) {
            mConnected = FALSE;
        }

        return done;
    }
}

//================================================================================
// WifiMonitor::MonitorThread
//================================================================================
WifiMonitor::MonitorThread::MonitorThread(
    /* [in] */ WifiMonitor* owner,
    /* [in] */ WifiNative* wifiNative,
    /* [in] */ WifiMonitorSingleton* wifiMonitorSingleton)
    : mHost(owner)
    , mWifiNative(wifiNative)
    , mWifiMonitorSingleton(wifiMonitorSingleton)
{
    Thread::constructor(String("WifiMonitor"));
}

ECode WifiMonitor::MonitorThread::Run()
{
    //noinspection InfiniteLoopStatement
    for (;;) {
        String eventStr = mHost->mWifiNative->WaitForEvent();

        // Skip logging the common but mostly uninteresting scan-results event
        if (DBG && eventStr.IndexOf(SCAN_RESULTS_STR) == -1) {
            Logger::D(WifiMonitor::TAG, "Event [%s]", eventStr.string());
        }

        if (mWifiMonitorSingleton->DispatchEvent(eventStr)) {
            if (DBG) Logger::D(TAG, "Disconnecting from the supplicant, no more events");
            break;
        }
    }
}

void WifiMonitor::LogDbg(
    /* [in] */ const String& debug)
{
    Logger::E(TAG, debug/*+ " stack:" + Thread.currentThread().getStackTrace()[2].getMethodName()
                       +" - "+ Thread.currentThread().getStackTrace()[3].getMethodName()
                       +" - "+ Thread.currentThread().getStackTrace()[4].getMethodName()
                       +" - "+ Thread.currentThread().getStackTrace()[5].getMethodName()*/);
}

/* @return true if the event was supplicant disconnection */
Boolean WifiMonitor::DispatchEvent(
    /* [in] */ const String& eventStr,
    /* [in] */ const String& iface);

{
    if (DBG) {
        // Dont log CTRL-EVENT-BSS-ADDED which are too verbose and not handled
        if (!eventStr.IsNull()&& !eventStr.Contains("CTRL-EVENT-BSS-ADDED")) {
            //logDbg("WifiMonitor:" + iface + " cnt=" + Integer.toString(eventLogCounter)
            //+ " dispatchEvent: " + eventStr);
            Logger::E(TAG, "WifiMonitor:%s, cnt = %d, dispatchEvent: ", iface.string(), eventLogCounter, eventStr.string());
        }
    }

    if (!eventStr.StartWith(EVENT_PREFIX_STR)) {
        if (eventStr.StartWith(WPA_EVENT_PREFIX_STR) &&
                0 < eventStr.IndexOf(PASSWORD_MAY_BE_INCORRECT_STR)) {
            mHost->mStateMachine->SendMessage(AUTHENTICATION_FAILURE_EVENT, eventLogCounter);
        }
        else if (eventStr.StartWith(WPS_SUCCESS_STR)) {
            mHost->mStateMachine->SendMessage(WPS_SUCCESS_EVENT);
        }
        else if (eventStr.StartWith(WPS_FAIL_STR)) {
            HandleWpsFailEvent(eventStr);
        }
        else if (eventStr.StartWith(WPS_OVERLAP_STR)) {
            mHost->mStateMachine->SendMessage(WPS_OVERLAP_EVENT);
        }
        else if (eventStr.StartWith(WPS_TIMEOUT_STR)) {
            mHost->mStateMachine->SendMessage(WPS_TIMEOUT_EVENT);
        }
        else if (eventStr.StartWith(P2P_EVENT_PREFIX_STR)) {
            HandleP2pEvents(eventStr);
        }
        else if (eventStr.StartWith(HOST_AP_EVENT_PREFIX_STR)) {
            HandleHostApEvents(eventStr);
        }
        else if (eventStr.StartWith(GAS_QUERY_PREFIX_STR)) {
            HandleGasQueryEvents(eventStr);
        }
        else if (eventStr.StartWith(RX_HS20_ANQP_ICON_STR)) {
            if (mStateMachine2 != NULL)
                mStateMachine2->SendMessage(RX_HS20_ANQP_ICON_EVENT,
                        eventStr.Substring(RX_HS20_ANQP_ICON_STR_LEN + 1));
        }
        else if (eventStr.StartWith(HS20_PREFIX_STR)) {
            HandleHs20Events(eventStr);
        }
        else if (eventStr.StartWith(REQUEST_PREFIX_STR)) {
            HandleRequests(eventStr);
        }
        else if (eventStr.StartWith(TARGET_SSID_STR)) {
            HandleTargetSSIDEvent(eventStr);
        }
        else if (eventStr.StartWith(TARGET_BSSID_STR)) {
            HandleTargetBSSIDEvent(eventStr);
        }
        else if (eventStr.StartWith(ASSOCIATED_WITH_STR)) {
            HandleAssociatedBSSIDEvent(eventStr);
        }
        else {
            if (DBG) Logger::W(TAG, "couldn't identify event type - %s", eventStr.string());
        }
        eventLogCounter++;
        return FALSE;
    }

    String eventName = eventStr.Substring(EVENT_PREFIX_LEN_STR);
    Int32 nameEnd = eventName.IndexOf(' ');
    if (nameEnd != -1) {
        eventName = eventName.Substring(0, nameEnd);
    }
    if (eventName.IsNullOrEmpty()) {
        if (DBG) Logger::I(TAG, "Received wpa_supplicant event with empty event name");
        eventLogCounter++;
        return FALSE;
    }
    /*
     * Map event name into event enum
     */
    Int32 event;
    if (eventName.Equals(CONNECTED_STR)) {
        event = CONNECTED;
    }
    else if (eventName.Equals(DISCONNECTED_STR)) {
        event = DISCONNECTED;
    }
    else if (eventName.Equals(STATE_CHANGE_STR)) {
        event = STATE_CHANGE;
    }
    else if (eventName.Equals(SCAN_RESULTS_STR)) {
        event = SCAN_RESULTS;
    }
    else if (eventName.Equals(LINK_SPEED_STR)) {
        event = LINK_SPEED;
    }
    else if (eventName.Equals(TERMINATING_STR)) {
        event = TERMINATING;
    }
    else if (eventName.Equals(DRIVER_STATE_STR)) {
        event = DRIVER_STATE;
    }
    else if (eventName.Equals(EAP_FAILURE_STR)) {
        event = EAP_FAILURE;
    }
    else if (eventName.Equals(ASSOC_REJECT_STR))
        event = ASSOC_REJECT;
    else if (eventName.Equals(TEMP_DISABLED_STR)) {
        event = SSID_TEMP_DISABLE;
    }
    else if (eventName.Equals(REENABLED_STR)) {
        event = SSID_REENABLE;
    }
    else if (eventName.Equals(BSS_ADDED_STR)) {
        event = BSS_ADDED;
    }
    else if (eventName.Equals(BSS_REMOVED_STR)) {
        event = BSS_REMOVED;
    }
    else {
        event = UNKNOWN;
    }

    String eventData = eventStr;
    if (event == DRIVER_STATE || event == LINK_SPEED) {
        AutoPtr< ArrayOf<String> > strArray;
        StringUtils::Split(eventData, String(" "), (ArrayOf<String>**)&strArray);
        eventData = (*strArray)[1];
    }
    else if (event == STATE_CHANGE || event == EAP_FAILURE) {
        Int32 ind = eventStr.IndexOf(" ");
        if (ind != -1) {
            eventData = eventStr.Substring(ind + 1);
        }
    }
    else {
        Int32 ind = eventStr.IndexOf(" - ");
        if (ind != -1) {
            eventData = eventStr.Substring(ind + 3);
        }
    }

    if ((event == SSID_TEMP_DISABLE)||(event == SSID_REENABLE)) {
        String substr;
        Int32 netId = -1;
        Int32 ind = eventStr.IndexOf(" ");
        if (ind != -1) {
            substr = eventStr.Substring(ind + 1);
        }
        if (!substr.IsNull()) {
            AutoPtr<ArrayOf<String> > status;
            StringUtils::Split(substr, String(" "), (ArrayOf<String>**)&status);
            for(Int32 i = 0; i < status->GetLength(); ++i) {
                String key = (*status)[i];
                if (key.RegionMatches(0, String("id="), 0, 3)) {
                    Int32 idx = 3;
                    netId = 0;
                    while (idx < key.GetLength()) {
                        char c = (char)key.GetChar(idx);
                        if ((c >= 0x30) && (c <= 0x39)) {
                            netId *= 10;
                            netId += c - 0x30;
                            idx++;
                        } else {
                            break;
                        }
                    }
                }
            }
        }
        mStateMachine->SendMessage((event == SSID_TEMP_DISABLE)? SSID_TEMP_DISABLED:SSID_REENABLED, netId, 0, substr);
    }
    else if (event == STATE_CHANGE) {
        HandleSupplicantStateChange(eventData);
    }
    else if (event == DRIVER_STATE) {
        HandleDriverEvent(eventData);
    }
    else if (event == TERMINATING) {
        /**
         * Close the supplicant connection if we see
         * too many recv errors
         */
        if (eventData.StartWith(WPA_RECV_ERROR_STR)) {
            if (++mHost->sRecvErrors > MAX_RECV_ERRORS) {
                if (DBG) {
                    Logger::D(TAG, "too many recv errors, closing connection");
                }
            }
            else {
                eventLogCounter++;
                return FALSE;
            }
        }

        // notify and exit
        mHost->mStateMachine->SendMessage(SUP_DISCONNECTION_EVENT, eventLogCounter);
        return TRUE;
    }
    else if (event == EAP_FAILURE) {
        if (eventData.StartWith(EAP_AUTH_FAILURE_STR)) {
            Logger::E(TAG, "WifiMonitor send auth failure (EAP_AUTH_FAILURE) ");
            mHost->mStateMachine->SendMessage(AUTHENTICATION_FAILURE_EVENT);
        }
    }
    else if (event == ASSOC_REJECT) {
        assert(0);
        /*TODO
        Matcher match = mAssocRejectEventPattern.matcher(eventData);
        String BSSID = "";
        Int32 status = -1;
        if (!match.find()) {
            if (DBG) Logger::D(TAG, "Assoc Reject: Could not parse assoc reject string");
        } else {
            BSSID = match.group(1);
            try {
                status = Integer.parseInt(match.group(2));
            } catch (NumberFormatException e) {
                status = -1;
            }
        }
        mStateMachine->SendMessage(ASSOCIATION_REJECTION_EVENT, eventLogCounter, status, BSSID);
        */
    } else if (event == BSS_ADDED && !VDBG) {
        // Ignore that event - it is not handled, and dont log it as it is too verbose
    } else if (event == BSS_REMOVED && !VDBG) {
        // Ignore that event - it is not handled, and dont log it as it is too verbose
    }  else {
        HandleEvent(event, eventData);
    }
    sRecvErrors = 0;
    eventLogCounter++;
    return FALSE;
}

//Boolean WifiMonitor::MonitorThread::ConnectToSupplicant()
//{
//    Int32 connectTries = 0;
//
//    while (TRUE) {
//        if (mHost->mWifiNative->ConnectToSupplicant()) {
//            return TRUE;
//        }
//        if (connectTries++ < 5) {
//            mHost->Nap(1);
//        }
//        else {
//            break;
//        }
//    }
//    return FALSE;
//}

void WifiMonitor::HandleDriverEvent(
    /* [in] */ const String& state)
{
    if (state.IsNull()) {
        return;
    }
    if (state.Equals("HANGED")) {
        mHost->mStateMachine->SendMessage(DRIVER_HUNG_EVENT);
    }
}

void WifiMonitor::MonitorThread::HandleEvent(
    /* [in] */ Int32 event,
    /* [in] */ const String& remainder)
{
    if (DBG) {
        //logDbg("handleEvent " + Integer.toString(event) + "  " + remainder);
        Logger::E(TAG, "handleEvent %d %s", event, remainder.string());
    }
    switch (event) {
        case DISCONNECTED:
            Logger::D(WifiMonitor::TAG, "TODO: should be delete");
            property_set("net.state", "0");
            mHost->HandleNetworkStateChange(NetworkInfoDetailedState_DISCONNECTED, remainder);
            break;

        case CONNECTED:
            Logger::D(WifiMonitor::TAG, "TODO: should be delete");
            property_set("net.state", "1");
            mHost->HandleNetworkStateChange(NetworkInfoDetailedState_CONNECTED, remainder);
            break;

        case SCAN_RESULTS:
            mHost->mStateMachine->SendMessage(SCAN_RESULTS_EVENT);
            break;

        case UNKNOWN:
            if (DBG) {
                //logDbg("handleEvent unknown: " + Integer.toString(event) + "  " + remainder);
                Logger::E(TAG, "handleEvent unknown: %d, %s", event, remainder.string());
            }
            break;
        default:
            break;
    }
}

void WifiMonitor::HandleTargetSSIDEvent(
    /* [in] */ const String& eventStr)
{
    //TODO
    assert(0);
    //String SSID = null;
    //Matcher match = mTargetSSIDPattern.matcher(eventStr);
    //if (match.find()) {
    //    SSID = match.group(1);
    //} else {
    //    Log.d(TAG, "didn't find SSID " + eventStr);
    //}
    //mStateMachine.sendMessage(WifiStateMachine.CMD_TARGET_SSID,
    //        eventLogCounter, 0, SSID);
}

void WifiMonitor::HandleTargetBSSIDEvent(
    /* [in] */ const String& eventStr)
{
    //TODO
    assert(0);
    //String BSSID = null;
    //Matcher match = mTargetBSSIDPattern.matcher(eventStr);
    //if (match.find()) {
    //    BSSID = match.group(1);
    //} else {
    //    Log.d(TAG, "didn't find BSSID " + eventStr);
    //}
    //mStateMachine.sendMessage(WifiStateMachine.CMD_TARGET_BSSID, eventLogCounter, 0, BSSID);
}

void WifiMonitor::HandleAssociatedBSSIDEvent(
    /* [in] */ const String& eventStr)
{
    //TODO
    assert(0);
    //String BSSID = null;
    //Matcher match = mAssociatedPattern.matcher(eventStr);
    //if (match.find()) {
    //    BSSID = match.group(1);
    //} else {
    //    Log.d(TAG, "handleAssociatedBSSIDEvent: didn't find BSSID " + eventStr);
    //}
    //mStateMachine.sendMessage(WifiStateMachine.CMD_ASSOCIATED_BSSID, eventLogCounter, 0, BSSID);
}

void WifiMonitor::HandleWpsFailEvent(
    /* [in] */ const String& dataString)
{
    AutoPtr<IPatternHelper> helper;
    CPatternHelper::AcquireSingleton((IPatternHelper**)&helper);
    AutoPtr<IPattern> p;
    helper->Compile(WifiMonitor::WPS_FAIL_PATTERN, (IPattern**)&p);
    AutoPtr<IMatcher> match;
    p->Matcher(dataString, (IMatcher**)&match);
    Int32 reason = 0;
    Boolean find;
    if (match->Find(&find), find) {
        String cfgErrStr;
        match->Group(1, &cfgErrStr);
        String reasonStr;
        match->Group(2, &reasonStr);

        if (!reasonStr.IsNull()) {
            Int32 reasonInt = StringUtils::ParseInt32(reasonStr);
            switch(reasonInt) {
                case WifiMonitor::REASON_TKIP_ONLY_PROHIBITED:
                    mHost->mStateMachine->SendMessage(mHost->mStateMachine->ObtainMessage(WPS_FAIL_EVENT,
                            IWifiManager::WPS_TKIP_ONLY_PROHIBITED, 0));
                    return;
                case WifiMonitor::REASON_WEP_PROHIBITED:
                    mHost->mStateMachine->SendMessage(mHost->mStateMachine->ObtainMessage(WPS_FAIL_EVENT,
                            IWifiManager::WPS_WEP_PROHIBITED, 0));
                    return;
                default:
                    reason = reasonInt;
                    break;
            }
        }
        if (!cfgErrStr.IsNull()) {
            Int32 cfgErrInt = StringUtils::ParseInt32(cfgErr);
            switch(cfgErrInt) {
                case CONFIG_AUTH_FAILURE:
                    mHost->mStateMachine->SendMessage(mHost->mStateMachine->ObtainMessage(WPS_FAIL_EVENT,
                            IWifiManager::WPS_AUTH_FAILURE, 0));
                    return;
                case CONFIG_MULTIPLE_PBC_DETECTED:
                    mHost->mStateMachine->SendMessage(mHost->mStateMachine->ObtainMessage(WPS_FAIL_EVENT,
                            IWifiManager::WPS_OVERLAP_ERROR, 0));
                    return;
                default:
                    if (reason == 0) reason = cfgErrInt;
                    break;
            }
        }
    }
    //For all other errors, return a generic internal error
    mHost->mStateMachine->SendMessage(mHost->mStateMachine->ObtainMessage(WPS_FAIL_EVENT,
            IWifiManager::ERROR, reason));
}

P2pStatus WifiMonitor::ValueOf(
    /* [in] */ Int32 error)
{
    using namespace Elastos::Droid::Wifi::P2p;
    switch(error) {
    case 0 :
        return P2pStatus_SUCCESS;
    case 1:
        return P2pStatus_INFORMATION_IS_CURRENTLY_UNAVAILABLE;
    case 2:
        return P2pStatus_INCOMPATIBLE_PARAMETERS;
    case 3:
        return P2pStatus_LIMIT_REACHED;
    case 4:
        return P2pStatus_INVALID_PARAMETER;
    case 5:
        return P2pStatus_UNABLE_TO_ACCOMMODATE_REQUEST;
    case 6:
        return P2pStatus_PREVIOUS_PROTOCOL_ERROR;
    case 7:
        return P2pStatus_NO_COMMON_CHANNEL;
    case 8:
        return P2pStatus_UNKNOWN_P2P_GROUP;
    case 9:
        return P2pStatus_BOTH_GO_INTENT_15;
    case 10:
        return P2pStatus_INCOMPATIBLE_PROVISIONING_METHOD;
    case 11:
        return P2pStatus_REJECTED_BY_USER;
    default:
        return P2pStatus_UNKNOWN;
    }
}

P2pStatus WifiMonitor::P2pError(
   /* [in] */ const String& dataString)
{
    P2pStatus err = P2pStatus_UNKNOWN;
    AutoPtr< ArrayOf<String> > tokens;
    StringUtils::Split(dataString, String(" "), (ArrayOf<String>**)&tokens);

    if (tokens == NULL || tokens->GetLength() < 2) return err;

    AutoPtr< ArrayOf<String> > nameValue;
    StringUtils::Split((*tokens)[1], String("="), (ArrayOf<String>**)&nameValue);
    if (nameValue == NULL || nameValue->GetLength() < 2) return err;

    /* Handle the special case of reason=FREQ+CONFLICT */
    if ((*nameValue)[1].Equals("FREQ_CONFLICT")) {
        return Elastos::Droid::Wifi::P2p::P2pStatus_NO_COMMON_CHANNEL;
    }
    // try {
    Int32 value;
    ECode ec = StringUtils::ParseInt32((*nameValue)[1], 10, &value);
    if (SUCCEEDED(ec)) {
        err = ValueOf(value);
    }
    else {
        Logger::E(WifiMonitor::TAG, "NumberFormatException %s", (*nameValue)[1].string());
    }
   // } catch (NumberFormatException e) {
   //     e.printStackTrace();
   // }
   return err;
}

void WifiMonitor::HandleP2pEvents(
    /* [in]*/ const String& dataString)
{
    if (DBG) {
        Logger::D(WifiMonitor::TAG, "HandleP2pEvents [%s]", dataString.string());
    }

    if (dataString.StartWith(P2P_DEVICE_FOUND_STR)) {
        AutoPtr<IWifiP2pDevice> device;
        ASSERT_SUCCEEDED(CWifiP2pDevice::New(dataString, (IWifiP2pDevice**)&device));
        mHost->mStateMachine->SendMessage(P2P_DEVICE_FOUND_EVENT, device);
    }
    else if (dataString.StartWith(P2P_DEVICE_LOST_STR)) {
        AutoPtr<IWifiP2pDevice> device;
        ASSERT_SUCCEEDED(CWifiP2pDevice::New(dataString, (IWifiP2pDevice**)&device));
        mHost->mStateMachine->SendMessage(P2P_DEVICE_LOST_EVENT, device);
    }
    else if (dataString.StartWith(P2P_FIND_STOPPED_STR)) {
        mHost->mStateMachine->SendMessage(P2P_FIND_STOPPED_EVENT);
    }
    else if (dataString.StartWith(P2P_GO_NEG_REQUEST_STR)) {
        AutoPtr<IWifiP2pConfig> config;
        ASSERT_SUCCEEDED(CWifiP2pConfig::New(dataString, (IWifiP2pConfig**)&config));
        mHost->mStateMachine->SendMessage(P2P_GO_NEGOTIATION_REQUEST_EVENT, config);
    }
    else if (dataString.StartWith(P2P_GO_NEG_SUCCESS_STR)) {
        mHost->mStateMachine->SendMessage(P2P_GO_NEGOTIATION_SUCCESS_EVENT);
    }
    else if (dataString.StartWith(P2P_GO_NEG_FAILURE_STR)) {
        Int32 value = P2pError(dataString);
        AutoPtr<IInteger32> iobj;
        CInteger32::New(value, (IInteger32**)&iobj);
        mHost->mStateMachine->SendMessage(P2P_GO_NEGOTIATION_FAILURE_EVENT, iobj);
    }
    else if (dataString.StartWith(P2P_GROUP_FORMATION_SUCCESS_STR)) {
        mHost->mStateMachine->SendMessage(P2P_GROUP_FORMATION_SUCCESS_EVENT);
    }
    else if (dataString.StartWith(P2P_GROUP_FORMATION_FAILURE_STR)) {
        Int32 value = P2pError(dataString);
        AutoPtr<IInteger32> iobj;
        CInteger32::New(value, (IInteger32**)&iobj);
        mHost->mStateMachine->SendMessage(P2P_GROUP_FORMATION_FAILURE_EVENT, iobj);
    }
    else if (dataString.StartWith(P2P_GROUP_STARTED_STR)) {
        AutoPtr<IWifiP2pGroup> group;
        ASSERT_SUCCEEDED(CWifiP2pGroup::New(dataString, (IWifiP2pGroup**)&group));
        mHost->mStateMachine->SendMessage(P2P_GROUP_STARTED_EVENT, group);
    }
    else if (dataString.StartWith(P2P_GROUP_REMOVED_STR)) {
        AutoPtr<IWifiP2pGroup> group;
        ASSERT_SUCCEEDED(CWifiP2pGroup::New(dataString, (IWifiP2pGroup**)&group));
        mHost->mStateMachine->SendMessage(P2P_GROUP_REMOVED_EVENT, group);
    }
    else if (dataString.StartWith(P2P_INVITATION_RECEIVED_STR)) {
        AutoPtr<IWifiP2pGroup> group;
        ASSERT_SUCCEEDED(CWifiP2pGroup::New(dataString, (IWifiP2pGroup**)&group));
        mHost->mStateMachine->SendMessage(P2P_INVITATION_RECEIVED_EVENT, group);
    }
    else if (dataString.StartWith(P2P_INVITATION_RESULT_STR)) {
        Int32 value = P2pError(dataString);
        AutoPtr<IInteger32> iobj;
        CInteger32::New(value, (IInteger32**)&iobj);
        mHost->mStateMachine->SendMessage(P2P_INVITATION_RESULT_EVENT, iobj);
    }
    else if (dataString.StartWith(P2P_PROV_DISC_PBC_REQ_STR)) {
        AutoPtr<IWifiP2pProvDiscEvent> event;
        ASSERT_SUCCEEDED(CWifiP2pProvDiscEvent::New(dataString, (IWifiP2pProvDiscEvent**)&event));
        mHost->mStateMachine->SendMessage(P2P_PROV_DISC_PBC_REQ_EVENT, event);
    }
    else if (dataString.StartWith(P2P_PROV_DISC_PBC_RSP_STR)) {
        AutoPtr<IWifiP2pProvDiscEvent> event;
        ASSERT_SUCCEEDED(CWifiP2pProvDiscEvent::New(dataString, (IWifiP2pProvDiscEvent**)&event));
        mHost->mStateMachine->SendMessage(P2P_PROV_DISC_PBC_RSP_EVENT, event);
    }
    else if (dataString.StartWith(P2P_PROV_DISC_ENTER_PIN_STR)) {
        AutoPtr<IWifiP2pProvDiscEvent> event;
        ASSERT_SUCCEEDED(CWifiP2pProvDiscEvent::New(dataString, (IWifiP2pProvDiscEvent**)&event));
        mHost->mStateMachine->SendMessage(P2P_PROV_DISC_ENTER_PIN_EVENT, event);
    }
    else if (dataString.StartWith(P2P_PROV_DISC_SHOW_PIN_STR)) {
        AutoPtr<IWifiP2pProvDiscEvent> event;
        ASSERT_SUCCEEDED(CWifiP2pProvDiscEvent::New(dataString, (IWifiP2pProvDiscEvent**)&event));
        mHost->mStateMachine->SendMessage(P2P_PROV_DISC_SHOW_PIN_EVENT, event);
    }
    else if (dataString.StartWith(P2P_PROV_DISC_FAILURE_STR)) {
        mHost->mStateMachine->SendMessage(P2P_PROV_DISC_FAILURE_EVENT);
    }
    else if (dataString.StartWith(P2P_SERV_DISC_RESP_STR)) {
        AutoPtr<IWifiP2pServiceResponseHelper> helper;
        CWifiP2pServiceResponseHelper::AcquireSingleton((IWifiP2pServiceResponseHelper**)&helper);
        AutoPtr<ArrayOf<IWifiP2pServiceResponse*> > array;
        ASSERT_SUCCEEDED(helper->NewInstance(dataString, (ArrayOf<IWifiP2pServiceResponse*>**)&array));
        if (array != NULL && array->GetLength() > 0) {
            AutoPtr<IObjectContainer> container = ArrayUtils::ToObjectContainer(array);
            mHost->mStateMachine->SendMessage(P2P_SERV_DISC_RESP_EVENT, container);
        }
        else {
            Logger::E(WifiMonitor::TAG, "Null service resp %s", dataString.string());
        }
    }
    else if (dataString.StartWith(P2P_REMOVE_AND_REFORM_GROUP_STR)) {
        Logger::D(TAG, "Received event= %s", dataString.string());
        mStateMachine->SendMessage(P2P_REMOVE_AND_REFORM_GROUP_EVENT);
    }
}

void WifiMonitor::HandleHostApEvents(
    /* [in] */ const String& dataString)
{
    AutoPtr< ArrayOf<String> > tokens;
    StringUtils::Split(dataString, String(" "), (ArrayOf<String>**)&tokens);

    if (tokens == NULL) {
        Logger::E(WifiMonitor::TAG, "HandleHostApEvents: invalid argument %s", dataString.string());
        return;
    }

    /* AP-STA-CONNECTED 42:fc:89:a8:96:09 p2p_dev_addr=02:90:4c:a0:92:54 */
    if ((*tokens)[0].Equals(AP_STA_CONNECTED_STR)) {
        AutoPtr<IWifiP2pDevice> device;
        CWifiP2pDevice::New(dataString, (IWifiP2pDevice**)&device);
        mHost->mStateMachine->SendMessage(AP_STA_CONNECTED_EVENT, device);
    /* AP-STA-DISCONNECTED 42:fc:89:a8:96:09 p2p_dev_addr=02:90:4c:a0:92:54 */
    }
    else if ((*tokens)[0].Equals(AP_STA_DISCONNECTED_STR)) {
        AutoPtr<IWifiP2pDevice> device;
        CWifiP2pDevice::New(dataString, (IWifiP2pDevice**)&device);
        mHost->mStateMachine->SendMessage(AP_STA_DISCONNECTED_EVENT, device);
    }
}

/**
  * Handle ANQP events
  */
void WifiMonitor::HandleGasQueryEvents(
    /* [in] */ const String& dataString)
{
    //TODO
    assert(0);
    // hs20
    //if (mStateMachine2 == null) return;
    //if (dataString.startsWith(GAS_QUERY_START_STR)) {
    //    mStateMachine2.sendMessage(GAS_QUERY_START_EVENT);
    //} else if (dataString.startsWith(GAS_QUERY_DONE_STR)) {
    //    String[] dataTokens = dataString.split(" ");
    //    String bssid = null;
    //    int success = 0;
    //    for (String token : dataTokens) {
    //        String[] nameValue = token.split("=");
    //        if (nameValue.length != 2) {
    //            continue;
    //        }
    //        if (nameValue[0].equals("addr")) {
    //            bssid = nameValue[1];
    //            continue;
    //        }
    //        if (nameValue[0].equals("result"))  {
    //            success = nameValue[1].equals("SUCCESS") ? 1 : 0;
    //            continue;
    //        }
    //    }
    //    mStateMachine2.sendMessage(GAS_QUERY_DONE_EVENT, success, 0, bssid);
    //} else {
    //    if (DBG) Log.d(TAG, "Unknown GAS query event: " + dataString);
    //}
}

/**
 * Handle HS20 events
 */
void WifiMonitor::HandleHs20Events(
    /* [in] */ const String& dataString)
{
    //TODO
    assert(0);
    //if (mStateMachine2 == null) return;
    //if (dataString.startsWith(HS20_SUB_REM_STR)) {
    //    // format: HS20-SUBSCRIPTION-REMEDIATION osu_method, url
    //    String[] dataTokens = dataString.split(" ");
    //    int method = -1;
    //    String url = null;
    //    if (dataTokens.length >= 3) {
    //        method = Integer.parseInt(dataTokens[1]);
    //        url = dataTokens[2];
    //    }
    //    mStateMachine2.sendMessage(HS20_REMEDIATION_EVENT, method, 0, url);
    //} else if (dataString.startsWith(HS20_DEAUTH_STR)) {
    //    // format: HS20-DEAUTH-IMMINENT-NOTICE code, delay, url
    //    int code = -1;
    //    int delay = -1;
    //    String url = null;
    //    String[] dataTokens = dataString.split(" ");
    //    if (dataTokens.length >= 4) {
    //        code = Integer.parseInt(dataTokens[1]);
    //        delay = Integer.parseInt(dataTokens[2]);
    //        url = dataTokens[3];
    //    }
    //    mStateMachine2.sendMessage(HS20_DEAUTH_EVENT, code, delay, url);
    //} else {
    //    if (DBG) Log.d(TAG, "Unknown HS20 event: " + dataString);
    //}
}

/**
 * Handle Supplicant Requests
 */
void WifiMonitor::HandleRequests(
    /* [in] */ const String& dataString)
{
    //TODO
    assert(0);
    //String SSID = null;
    //int reason = -2;
    //String requestName = dataString.substring(REQUEST_PREFIX_LEN_STR);
    //if (TextUtils.isEmpty(requestName)) {
    //    return;
    //}
    //if (requestName.startsWith(IDENTITY_STR)) {
    //    Matcher match = mRequestIdentityPattern.matcher(requestName);
    //    if (match.find()) {
    //        SSID = match.group(2);
    //        try {
    //            reason = Integer.parseInt(match.group(1));
    //        } catch (NumberFormatException e) {
    //            reason = -1;
    //        }
    //    } else {
    //        Log.e(TAG, "didn't find SSID " + requestName);
    //    }
    //    mStateMachine.sendMessage(SUP_REQUEST_IDENTITY, eventLogCounter, reason, SSID);
    //} if (requestName.startsWith(SIM_STR)) {
    //    Matcher match = mRequestGsmAuthPattern.matcher(requestName);
    //    if (match.find()) {
    //        WifiStateMachine.SimAuthRequestData data =
    //            new WifiStateMachine.SimAuthRequestData();
    //        data.networkId = Integer.parseInt(match.group(1));
    //        data.protocol = WifiEnterpriseConfig.Eap.SIM;
    //        data.ssid = match.group(4);
    //        data.challenges = match.group(2).split(":");
    //        mStateMachine.sendMessage(SUP_REQUEST_SIM_AUTH, data);
    //    } else {
    //        Log.e(TAG, "couldn't parse SIM auth request - " + requestName);
    //    }

    //} else {
    //    if (DBG) Log.w(TAG, "couldn't identify request type - " + dataString);
    //}
}

void WifiMonitor::HandleSupplicantStateChange(
    /* [in] */ const String& dataString)
{
    AutoPtr<IWifiSsid> wifiSsid;

    Int32 index = dataString.LastIndexOf("SSID=");
    if (index != -1) {
        AutoPtr<IWifiSsidHelper> helper;
        CWifiSsidHelper::AcquireSingleton((IWifiSsidHelper**)&helper);
        String ssid = dataString.Substring(index + 5);
        helper->CreateFromAsciiEncoded(ssid, (IWifiSsid**)&wifiSsid);
    }

    AutoPtr<ArrayOf<String> > dataTokens;
    StringUtils::Split(dataString, String(" "), (ArrayOf<String>**)&dataTokens);

    String BSSID;
    Int32 networkId = -1;
    Int32 newState  = -1;
    if (dataTokens && dataTokens->GetLength() > 0) {
        ECode ec;
        String token;
        for (Int32 i = 0; i < dataTokens->GetLength(); ++i) {
            token = (*dataTokens)[i];
            AutoPtr<ArrayOf<String> > nameValue;
            StringUtils::Split(token, String("="), (ArrayOf<String>**)&nameValue);

            if (nameValue == NULL || nameValue->GetLength() != 2) {
                continue;
            }

            if ((*nameValue)[0].Equals("BSSID")) {
                BSSID = (*nameValue)[1];
                continue;
            }

            Int32 value = 0;
            ec = StringUtils::ParseInt32((*nameValue)[1], &value);
            if (ec == (ECode)E_NUMBER_FORMAT_EXCEPTION) continue;

            if ((*nameValue)[0].Equals("id")) {
                networkId = value;
            }
            else if ((*nameValue)[0].Equals("state")) {
                newState = value;
            }
        }
    }

    if (newState == -1) return;

    if (newState == SupplicantState_INVALID) {
        Logger::W(TAG, "Invalid supplicant state: %d", newState);
    }

    mHost->NotifySupplicantStateChange(networkId, wifiSsid, BSSID, newState);
}

//================================================================================
// WifiMonitor::WifiMonitor
//================================================================================
WifiMonitor::WifiMonitor(
    /* [in] */ StateMachine* wifiStateMachine,
    /* [in] */ WifiNative* wifiNative)
    : mWifiNative(wifiNative)
    , mStateMachine(wifiStateMachine)
{
    if (DBG) Logger::D(TAG, "Creating WifiMonitor");
    mInterfaceName = wifiNative->GetInterfaceName();//TODO
    mMonitoring = FALSE;
    WifiMonitorSingleton::sInstance->RegisterInterfaceMonitor(mInterfaceName, this);
}

void WifiMonitor::EnableVerboseLogging(
    /* [in] */ Int32 verbose)
{
    if (verbose > 0) {
        DBG = TRUE;
    }
    else {
        DBG = FALSE;
    }
}

// TODO: temporary hack, should be handle by supplicant manager (new component in future)
void WifiMonitor::SetStateMachine2(
    /* [in] */ StateMachine* stateMachine)
{
    mStateMachine2 = stateMachine;
}

void WifiMonitor::StartMonitoring()
{
    WifiMonitorSingleton::sInstance->StartMonitoring(mInterfaceName);
}

void WifiMonitor::StopMonitoring()
{
    WifiMonitorSingleton::sInstance->StopMonitoring(mInterfaceName);
}

void WifiMonitor::StopSupplicant()
{
    WifiMonitorSingleton::sInstance->StopSupplicant();
}

void WifiMonitor::KillSupplicant(
    /* [in] */ Boolean p2pSupported)
{
    WifiMonitorSingleton::sInstance->KillSupplicant(p2pSupported);
}

void WifiMonitor::HandleNetworkStateChange(
    /* [in] */ NetworkInfoDetailedState newState,
    /* [in] */ const String& data)
{
    String BSSID;
    Int32 networkId = -1;
    Int32 reason = 0;
    Int32 ind = -1;
    Int32 local = 0;
    AutoPtr<IMatcher> match;
    if (newState == NetworkInfoDetailedState_CONNECTED) {
        mConnectedEventPattern->Matcher(data, (IMatcher**)&match);
        Boolean found;
        if (match->Find(&found), !found) {
            if (DBG) Logger::D(TAG, "handleNetworkStateChange: Could not find BSSID in CONNECTED event string");
        }
        else {
            match->Group(1, &BSSID);
            // try {
            String id;
            match->Group(2, &id);
            networkId = StringUtils::ParseInt32(id);
            // } catch (NumberFormatException e) {
            //     networkId = -1;
        // }
        }
        NotifyNetworkStateChange(newState, BSSID, networkId, reason);
    }
    else if (newState == NetworkInfoDetailedState_DISCONNECTED) {
        mDisconnectedEventPattern->Matcher(data, (IMatcher**)&match);
        Boolean found;
        if (match->Find(&found), !found) {
            if (DBG) Logger::D(TAG, "handleNetworkStateChange: Could not parse disconnect string");
        } else {
            match->Group(1, &BSSID);
            //try {
            String id;
            match->Group(2, &id);
            reason = StringUtils::ParseInt32(id);
            //} catch (NumberFormatException e) {
            //    reason = -1;
            //}
            //try {
            String lStr;
            match->Group(3, &lStr);
            local = StringUtils::ParseInt32(lStr);
            //} catch (NumberFormatException e) {
            //    local = -1;
            //}
        }
        notifyNetworkStateChange(newState, BSSID, local, reason);
    }
}

void WifiMonitor::NotifyNetworkStateChange(
    /* [in] */ NetworkInfoDetailedState newState,
    /* [in] */ const String& BSSID,
    /* [in] */ Int32 netId,
    /* [in] */ Int32 reason)
{
    AutoPtr<ICharSequence> cs;
    CString::New(BSSID, (ICharSequence**)&cs);
    if (newState == NetworkInfoDetailedState_CONNECTED) {
        AutoPtr<IMessage> m = mStateMachine->ObtainMessage(NETWORK_CONNECTION_EVENT,
                netId, reason, cs);
        mStateMachine->SendMessage(m);
    }
    else {
        AutoPtr<IMessage> m = mStateMachine->ObtainMessage(NETWORK_DISCONNECTION_EVENT,
                netId, reason, cs);
        if (DBG) Logger::E(TAG, "WifiMonitor notify network disconnect: %s, reason=%d", BSSID.string(), reason);
        mStateMachine->SendMessage(m);
    }
}

void WifiMonitor::NotifySupplicantStateChange(
    /* [in] */ Int32 networkId,
    /* [in] */ IWifiSsid* wifiSsid,
    /* [in] */ const String& BSSID,
    /* [in] */ SupplicantState newState)
{
    if (DBG) {
        Logger::W("MonitorThread", "NotifySupplicantStateChange BSSID %s, wifiSsid: %p", BSSID.string(), wifiSsid);
    }

    AutoPtr<IStateChangeResult> result;
    CStateChangeResult::New(networkId, wifiSsid, BSSID, newState, (IStateChangeResult**)&result);
    AutoPtr<IInterface> temp = result->Probe(EIID_IInterface);
    AutoPtr<IMessage> m = mStateMachine->ObtainMessage(SUPPLICANT_STATE_CHANGE_EVENT, eventLogCounter, 0, temp);
    mStateMachine->SendMessage(m);
}

//void WifiMonitor::Nap(
//    /* [in] */ Int32 secs)
//{
//    // try {
//    Thread::Sleep(secs * 1000);
//    // } catch (InterruptedException ignore) {
//    // }
//}

} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namespace Elastos
