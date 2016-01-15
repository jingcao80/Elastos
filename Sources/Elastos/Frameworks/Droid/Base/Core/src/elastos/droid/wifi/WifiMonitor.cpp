
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
namespace Wifi {

static AutoPtr<IPattern> InitPattern()
{
    AutoPtr<IPatternHelper> helper;
    CPatternHelper::AcquireSingleton((IPatternHelper**)&helper);
    AutoPtr<IPattern> p;
    helper->Compile(String("((?:[0-9a-f]{2}:){5}[0-9a-f]{2}) .* \\[id=([0-9]+) "), (IPattern**)&p);
    return p;
}

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
const Int32 WifiMonitor::AP_STA_DISCONNECTED_EVENT;
const Int32 WifiMonitor::AP_STA_CONNECTED_EVENT;

const Int32 WifiMonitor::CONNECTED;
const Int32 WifiMonitor::DISCONNECTED;
const Int32 WifiMonitor::STATE_CHANGE;
const Int32 WifiMonitor::SCAN_RESULTS;
const Int32 WifiMonitor::LINK_SPEED;
const Int32 WifiMonitor::TERMINATING;
const Int32 WifiMonitor::DRIVER_STATE;
const Int32 WifiMonitor::EAP_FAILURE;
const Int32 WifiMonitor::UNKNOWN;

String WifiMonitor::EVENT_PREFIX_STR("CTRL-EVENT-");
const Int32 WifiMonitor::EVENT_PREFIX_LEN_STR = EVENT_PREFIX_STR.GetLength();
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
String WifiMonitor::CONNECTED_STR("CONNECTED");
String WifiMonitor::DISCONNECTED_STR("DISCONNECTED");
String WifiMonitor::STATE_CHANGE_STR("STATE-CHANGE");
String WifiMonitor::SCAN_RESULTS_STR("SCAN-RESULTS");
String WifiMonitor::LINK_SPEED_STR("LINK-SPEED");
String WifiMonitor::TERMINATING_STR("TERMINATING");
String WifiMonitor::DRIVER_STATE_STR("DRIVER-STATE");
String WifiMonitor::EAP_FAILURE_STR("EAP-FAILURE");
String WifiMonitor::EAP_AUTH_FAILURE_STR("EAP authentication failed");

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
String WifiMonitor::HOST_AP_EVENT_PREFIX_STR("AP");
String WifiMonitor::AP_STA_CONNECTED_STR("AP-STA-CONNECTED");
String WifiMonitor::AP_STA_DISCONNECTED_STR("AP-STA-DISCONNECTED");
String WifiMonitor::MONITOR_SOCKET_CLOSED_STR("connection closed");
String WifiMonitor::WPA_RECV_ERROR_STR("recv error");

const Int32 WifiMonitor::MAX_RECV_ERRORS;

AutoPtr<IPattern> WifiMonitor::mConnectedEventPattern = InitPattern();

//================================================================================
// WifiMonitor::MonitorThread
//================================================================================
WifiMonitor::MonitorThread::MonitorThread(
    /* [in] */ WifiMonitor* owner)
    : mHost(owner)
{
    Thread::constructor(String("WifiMonitor"));
}

ECode WifiMonitor::MonitorThread::Run()
{
    if (ConnectToSupplicant()) {
        // Send a message indicating that it is now possible to send commands
        // to the supplicant
        mHost->mStateMachine->SendMessage(SUP_CONNECTION_EVENT);
    }
    else {
        mHost->mStateMachine->SendMessage(SUP_DISCONNECTION_EVENT);
        return NOERROR;
    }

    //noinspection InfiniteLoopStatement
    for (;;) {
        String eventStr = mHost->mWifiNative->WaitForEvent();

        // Skip logging the common but mostly uninteresting scan-results event
        if (FALSE && eventStr.IndexOf(SCAN_RESULTS_STR) == -1) {
            Logger::D(WifiMonitor::TAG, "Event [%s]", eventStr.string());
        }

        if (!eventStr.StartWith(EVENT_PREFIX_STR)) {
            if (eventStr.StartWith(WPA_EVENT_PREFIX_STR) &&
                    0 < eventStr.IndexOf(PASSWORD_MAY_BE_INCORRECT_STR)) {
                mHost->mStateMachine->SendMessage(AUTHENTICATION_FAILURE_EVENT);
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
            continue;
        }

        String eventName = eventStr.Substring(EVENT_PREFIX_LEN_STR);
        Int32 nameEnd = eventName.IndexOf(' ');
        if (nameEnd != -1) {
            eventName = eventName.Substring(0, nameEnd);
        }
        if (eventName.IsNullOrEmpty()) {
            if (DBG) Logger::I(TAG, "Received wpa_supplicant event with empty event name");
            continue;
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

        if (event == STATE_CHANGE) {
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
                if (++mHost->mRecvErrors > MAX_RECV_ERRORS) {
                    if (DBG) {
                        Logger::D(TAG, "too many recv errors, closing connection");
                    }
                }
                else {
                    continue;
                }
            }

            // notify and exit
            mHost->mStateMachine->SendMessage(SUP_DISCONNECTION_EVENT);
            break;
        }
        else if (event == EAP_FAILURE) {
            if (eventData.StartWith(EAP_AUTH_FAILURE_STR)) {
                mHost->mStateMachine->SendMessage(AUTHENTICATION_FAILURE_EVENT);
            }
        }
        else {
            HandleEvent(event, eventData);
        }
        mHost->mRecvErrors = 0;
    }
    return NOERROR;
}

Boolean WifiMonitor::MonitorThread::ConnectToSupplicant()
{
    Int32 connectTries = 0;

    while (TRUE) {
        if (mHost->mWifiNative->ConnectToSupplicant()) {
            return TRUE;
        }
        if (connectTries++ < 5) {
            mHost->Nap(1);
        }
        else {
            break;
        }
    }
    return FALSE;
}

void WifiMonitor::MonitorThread::HandleDriverEvent(
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
            break;
    }
}

void WifiMonitor::MonitorThread::HandleWpsFailEvent(
    /* [in] */ const String& dataString)
{
    AutoPtr<IPatternHelper> helper;
    CPatternHelper::AcquireSingleton((IPatternHelper**)&helper);
    AutoPtr<IPattern> p;
    helper->Compile(WifiMonitor::WPS_FAIL_PATTERN, (IPattern**)&p);
    AutoPtr<IMatcher> match;
    p->Matcher(dataString, (IMatcher**)&match);
    Boolean find;
    if (match->Find(&find), find) {
        String cfgErr;
        match->Group(1, &cfgErr);
        String reason;
        match->Group(2, &reason);

        if (!reason.IsNull()) {
            switch(StringUtils::ParseInt32(reason)) {
                case WifiMonitor::REASON_TKIP_ONLY_PROHIBITED:
                    mHost->mStateMachine->SendMessage(mHost->mStateMachine->ObtainMessage(WPS_FAIL_EVENT,
                            IWifiManager::WPS_TKIP_ONLY_PROHIBITED, 0));
                    return;
                case WifiMonitor::REASON_WEP_PROHIBITED:
                    mHost->mStateMachine->SendMessage(mHost->mStateMachine->ObtainMessage(WPS_FAIL_EVENT,
                            IWifiManager::WPS_WEP_PROHIBITED, 0));
                    return;
            }
        }
        if (!cfgErr.IsNull()) {
            switch(StringUtils::ParseInt32(cfgErr)) {
                case CONFIG_AUTH_FAILURE:
                    mHost->mStateMachine->SendMessage(mHost->mStateMachine->ObtainMessage(WPS_FAIL_EVENT,
                            IWifiManager::WPS_AUTH_FAILURE, 0));
                    return;
                case CONFIG_MULTIPLE_PBC_DETECTED:
                    mHost->mStateMachine->SendMessage(mHost->mStateMachine->ObtainMessage(WPS_FAIL_EVENT,
                            IWifiManager::WPS_OVERLAP_ERROR, 0));
                    return;
            }
        }
    }
    //For all other errors, return a generic internal error
    mHost->mStateMachine->SendMessage(mHost->mStateMachine->ObtainMessage(WPS_FAIL_EVENT,
            IWifiManager::ERROR, 0));
}

P2pStatus WifiMonitor::MonitorThread::ValueOf(
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

P2pStatus WifiMonitor::MonitorThread::P2pError(
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

void WifiMonitor::MonitorThread::HandleP2pEvents(
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
}

void WifiMonitor::MonitorThread::HandleHostApEvents(
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

void WifiMonitor::MonitorThread::HandleSupplicantStateChange(
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
    : mStateMachine(wifiStateMachine)
    , mWifiNative(wifiNative)
    , mRecvErrors(0)
{}

void WifiMonitor::StartMonitoring()
{
    AutoPtr<MonitorThread> t = new MonitorThread(this);
    t->Start();
}

void WifiMonitor::HandleNetworkStateChange(
    /* [in] */ NetworkInfoDetailedState newState,
    /* [in] */ const String& data)
{
    String BSSID;
    Int32 networkId = -1;
    if (newState == NetworkInfoDetailedState_CONNECTED) {
        AutoPtr<IMatcher> match;
        mConnectedEventPattern->Matcher(data, (IMatcher**)&match);
        Boolean found;
        if (match->Find(&found), !found) {
            if (DBG) Logger::D(TAG, "Could not find BSSID in CONNECTED event string");
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
    }
    NotifyNetworkStateChange(newState, BSSID, networkId);
}

void WifiMonitor::NotifyNetworkStateChange(
    /* [in] */ NetworkInfoDetailedState newState,
    /* [in] */ const String& BSSID,
    /* [in] */ Int32 netId)
{
    AutoPtr<ICharSequence> cs;
    CString::New(BSSID, (ICharSequence**)&cs);
    if (newState == NetworkInfoDetailedState_CONNECTED) {
        AutoPtr<IMessage> m = mStateMachine->ObtainMessage(NETWORK_CONNECTION_EVENT,
                netId, 0, cs);
        mStateMachine->SendMessage(m);
    }
    else {
        AutoPtr<IMessage> m = mStateMachine->ObtainMessage(NETWORK_DISCONNECTION_EVENT,
                netId, 0, cs);
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
    AutoPtr<IMessage> m = mStateMachine->ObtainMessage(SUPPLICANT_STATE_CHANGE_EVENT, temp);
    mStateMachine->SendMessage(m);
}

void WifiMonitor::Nap(
    /* [in] */ Int32 secs)
{
    // try {
    Thread::Sleep(secs * 1000);
    // } catch (InterruptedException ignore) {
    // }
}

} // namespace Wifi
} // namespace Droid
} // namespace Elastos
