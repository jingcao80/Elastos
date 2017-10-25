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

#include "Elastos.Droid.Wifi.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/server/wifi/WifiMonitor.h"
#include "elastos/droid/server/wifi/WifiStateMachine.h"
#include "elastos/droid/server/wifi/p2p/WifiP2pServiceImpl.h"
#include "elastos/droid/server/wifi/CStateChangeResult.h"
#include "elastos/droid/text/TextUtils.h"
#include <cutils/properties.h>
#include "elastos/droid/internal/utility/ArrayUtils.h"
#include "elastos/core/AutoLock.h"
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringToIntegral.h>
#include "elastos/core/CoreUtils.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Internal::Utility::ArrayUtils;
using Elastos::Droid::Net::NetworkInfoDetailedState;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Server::Wifi::P2p::WifiP2pServiceImpl;
using Elastos::Droid::Server::Wifi::WifiStateMachine;
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
using Elastos::Droid::Wifi::IWifiEnterpriseConfigEap;
using Elastos::Droid::Wifi::IWifiManager;
using Elastos::Droid::Wifi::IWifiSsidHelper;
using Elastos::Droid::Wifi::CWifiSsidHelper;
using Elastos::Core::CoreUtils;
using Elastos::Core::AutoLock;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Core::StringUtils;
using Elastos::Core::StringToIntegral;
using Elastos::Core::IInteger32;
using Elastos::Core::CInteger32;
using Elastos::Utility::CHashMap;
using Elastos::Utility::IList;
using Elastos::Utility::ISet;
using Elastos::Utility::IIterator;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::Regex::IPatternHelper;
using Elastos::Utility::Regex::CPatternHelper;
using Elastos::Utility::Regex::IMatcher;
using Elastos::Utility::Regex::IMatchResult;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wifi {

static WifiP2pServiceImpl::P2pStatus ValueOf(
    /* [in] */ Int32 error)
{
    switch(error) {
    case 0 :
        return WifiP2pServiceImpl::P2pStatus_SUCCESS;
    case 1:
        return WifiP2pServiceImpl::P2pStatus_INFORMATION_IS_CURRENTLY_UNAVAILABLE;
    case 2:
        return WifiP2pServiceImpl::P2pStatus_INCOMPATIBLE_PARAMETERS;
    case 3:
        return WifiP2pServiceImpl::P2pStatus_LIMIT_REACHED;
    case 4:
        return WifiP2pServiceImpl::P2pStatus_INVALID_PARAMETER;
    case 5:
        return WifiP2pServiceImpl::P2pStatus_UNABLE_TO_ACCOMMODATE_REQUEST;
    case 6:
        return WifiP2pServiceImpl::P2pStatus_PREVIOUS_PROTOCOL_ERROR;
    case 7:
        return WifiP2pServiceImpl::P2pStatus_NO_COMMON_CHANNEL;
    case 8:
        return WifiP2pServiceImpl::P2pStatus_UNKNOWN_P2P_GROUP;
    case 9:
        return WifiP2pServiceImpl::P2pStatus_BOTH_GO_INTENT_15;
    case 10:
        return WifiP2pServiceImpl::P2pStatus_INCOMPATIBLE_PROVISIONING_METHOD;
    case 11:
        return WifiP2pServiceImpl::P2pStatus_REJECTED_BY_USER;
    default:
        return WifiP2pServiceImpl::P2pStatus_UNKNOWN;
    }
}

static WifiP2pServiceImpl::P2pStatus P2pError(
   /* [in] */ const String& dataString)
{
    WifiP2pServiceImpl::P2pStatus err = WifiP2pServiceImpl::P2pStatus_UNKNOWN;
    AutoPtr< ArrayOf<String> > tokens;
    StringUtils::Split(dataString, String(" "), (ArrayOf<String>**)&tokens);

    if (tokens == NULL || tokens->GetLength() < 2) return err;

    AutoPtr< ArrayOf<String> > nameValue;
    StringUtils::Split((*tokens)[1], String("="), (ArrayOf<String>**)&nameValue);
    if (nameValue == NULL || nameValue->GetLength() < 2) return err;

    /* Handle the special case of reason=FREQ+CONFLICT */
    if ((*nameValue)[1].Equals("FREQ_CONFLICT")) {
        return WifiP2pServiceImpl::P2pStatus_NO_COMMON_CHANNEL;
    }
    // try {
    Int32 value;
    ECode ec = StringToIntegral::Parse((*nameValue)[1], 10, &value);
    if (SUCCEEDED(ec)) {
        err = ValueOf(value);
    }
    else {
        Logger::E("WifiMonitor", "NumberFormatException %s", (*nameValue)[1].string());
    }
   // } catch (NumberFormatException e) {
   //     e.printStackTrace();
   // }
   return err;
}

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
const Int32 WifiMonitor::RX_HS20_ANQP_ICON_STR_LEN = WifiMonitor::RX_HS20_ANQP_ICON_STR.GetLength();

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
AutoPtr<WifiMonitor::WifiMonitorSingleton> WifiMonitor::WifiMonitorSingleton::sInstance
                                        = new WifiMonitor::WifiMonitorSingleton();

WifiMonitor::WifiMonitorSingleton::WifiMonitorSingleton()
{
    mConnected = FALSE;
    CHashMap::New((IHashMap**)&mIfaceMap);
}

void WifiMonitor::WifiMonitorSingleton::StartMonitoring(
    /* [in] */ const String& iface)
{
    AutoLock lock(mLock);
    AutoPtr<IInterface> obj;
    AutoPtr<IInterface> key = TO_IINTERFACE(CoreUtils::Convert(iface));
    mIfaceMap->Get(key, (IInterface**)&obj);
    AutoPtr<WifiMonitor> m = (WifiMonitor*)(IObject::Probe(obj));
    if (m == NULL) {
        Logger::E(WifiMonitor::TAG, "startMonitor called with unknown iface=%s", iface.string());
        return;
    }

    Logger::D(WifiMonitor::TAG, "startMonitoring(%s) with mConnected = %d", iface.string(), mConnected);

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
                (new MonitorThread(m, mWifiNative, this))->Start();
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
                Logger::E(TAG, "startMonitoring(%s) failed!", iface.string());
                break;
            }
        }
    }
}

void WifiMonitor::WifiMonitorSingleton::StopMonitoring(
    /* [in] */ const String& iface)
{
    AutoLock lock(mLock);
    AutoPtr<IInterface> obj;
    AutoPtr<IInterface> key = TO_IINTERFACE(CoreUtils::Convert(iface));
    mIfaceMap->Get(key, (IInterface**)&obj);
    AutoPtr<WifiMonitor> m = (WifiMonitor*)(IObject::Probe(obj));
    if (DBG) Logger::D(TAG, "stopMonitoring(%s) = ", iface.string());//m.mStateMachine);
    m->mMonitoring = FALSE;
    m->mStateMachine->SendMessage(SUP_DISCONNECTION_EVENT);
}

void WifiMonitor::WifiMonitorSingleton::RegisterInterfaceMonitor(
    /* [in] */ const String& iface,
    /* [in] */ WifiMonitor* m)
{
    AutoLock lock(mLock);
    if (DBG) Logger::D(TAG, "registerInterface(%s)", iface.string());//+" + m.mStateMachine + ")");
    AutoPtr<IInterface> key = TO_IINTERFACE(CoreUtils::Convert(iface));
    AutoPtr<IInterface> value = TO_IINTERFACE(m);
    mIfaceMap->Put(key, value);
    if (mWifiNative == NULL) {
        mWifiNative = m->mWifiNative;
    }
}

void WifiMonitor::WifiMonitorSingleton::UnregisterInterfaceMonitor(
    /* [in] */ const String& iface)
{
    // REVIEW: When should we call this? If this isn't called, then WifiMonitor
    // objects will remain in the mIfaceMap; and won't ever get deleted
    AutoLock lock(mLock);
    AutoPtr<IInterface> obj;
    AutoPtr<IInterface> key = TO_IINTERFACE(CoreUtils::Convert(iface));
    mIfaceMap->Get(key, (IInterface**)&obj);
    AutoPtr<WifiMonitor> m = (WifiMonitor*)(IObject::Probe(obj));
    mIfaceMap->Remove(key);
    if (DBG) Logger::D(TAG, "unregisterInterface(%s)", iface.string());// "+" + m.mStateMachine + ")");
}

void WifiMonitor::WifiMonitorSingleton::StopSupplicant()
{
    AutoLock lock(mLock);
    mWifiNative->StopSupplicant();
}

void WifiMonitor::WifiMonitorSingleton::KillSupplicant(
    /* [in] */ Boolean p2pSupported)
{
    AutoLock lock(mLock);
    WifiNative::KillSupplicant(p2pSupported);
    mConnected = FALSE;
    AutoPtr<ISet> entrySet;
    mIfaceMap->GetEntrySet((ISet**)&entrySet);
    AutoPtr<IIterator> it;
    entrySet->GetIterator((IIterator**)&it);
    Boolean hasNext;
    AutoPtr<IMapEntry> entry;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> mapEntry;
        it->GetNext((IInterface**)&mapEntry);
        entry = IMapEntry::Probe(mapEntry);
        AutoPtr<IInterface> vo;
        entry->GetValue((IInterface**)&vo);
        AutoPtr<WifiMonitor> m = (WifiMonitor*)(IObject::Probe(vo));
        m->mMonitoring = FALSE;
    }
}

Boolean WifiMonitor::WifiMonitorSingleton::DispatchEvent(
    /* [in] */ const String& _eventStr)
{
    AutoLock lock(mLock);
    String iface;
    String eventStr = _eventStr;
    if (eventStr.StartWith("IFNAME=")) {
        Int32 space = eventStr.IndexOf(' ');
        if (space != -1) {
            iface = eventStr.Substring(7, space);
            AutoPtr<IInterface> obj;
            AutoPtr<IInterface> key = TO_IINTERFACE(CoreUtils::Convert(iface));
            mIfaceMap->Get(key, (IInterface**)&obj);
            AutoPtr<WifiMonitor> m = (WifiMonitor*)(IObject::Probe(obj));
            if (m == NULL && iface.StartWith("p2p-")) {
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

    AutoPtr<IInterface> obj;
    AutoPtr<IInterface> key = TO_IINTERFACE(CoreUtils::Convert(iface));
    mIfaceMap->Get(key, (IInterface**)&obj);
    AutoPtr<WifiMonitor> m = (WifiMonitor*)(IObject::Probe(obj));
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

        AutoPtr<ISet> entrySet;
        mIfaceMap->GetEntrySet((ISet**)&entrySet);
        AutoPtr<IIterator> it;
        entrySet->GetIterator((IIterator**)&it);
        Boolean hasNext;
        AutoPtr<IMapEntry> entry;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> mapEntry;
            it->GetNext((IInterface**)&mapEntry);
            entry = IMapEntry::Probe(mapEntry);
            AutoPtr<IInterface> vo;
            entry->GetValue((IInterface**)&vo);
            AutoPtr<WifiMonitor> m = (WifiMonitor*)(IObject::Probe(vo));
            m->mMonitoring = FALSE;
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
    return NOERROR;
}

void WifiMonitor::LogDbg(
    /* [in] */ const String& debug)
{
    Logger::D(TAG, debug/*+ " stack:" + Thread.currentThread().getStackTrace()[2].getMethodName()
                       +" - "+ Thread.currentThread().getStackTrace()[3].getMethodName()
                       +" - "+ Thread.currentThread().getStackTrace()[4].getMethodName()
                       +" - "+ Thread.currentThread().getStackTrace()[5].getMethodName()*/);
}

/* @return true if the event was supplicant disconnection */
Boolean WifiMonitor::DispatchEvent(
    /* [in] */ const String& eventStr,
    /* [in] */ const String& iface)
{
    if (DBG) {
        // Dont log CTRL-EVENT-BSS-ADDED which are too verbose and not handled
        if (!eventStr.IsNull()&& !eventStr.Contains("CTRL-EVENT-BSS-ADDED")) {
            //logDbg("WifiMonitor:" + iface + " cnt=" + Integer.toString(eventLogCounter)
            //+ " dispatchEvent: " + eventStr);
            Logger::D(TAG, "WifiMonitor:%s, cnt = %d, dispatchEvent: %s", iface.string(), eventLogCounter, eventStr.string());
        }
    }

    if (!eventStr.StartWith(EVENT_PREFIX_STR)) {
        if (eventStr.StartWith(WPA_EVENT_PREFIX_STR) &&
                0 < eventStr.IndexOf(PASSWORD_MAY_BE_INCORRECT_STR)) {
            mStateMachine->SendMessage(AUTHENTICATION_FAILURE_EVENT, eventLogCounter);
        }
        else if (eventStr.StartWith(WPS_SUCCESS_STR)) {
            mStateMachine->SendMessage(WPS_SUCCESS_EVENT);
        }
        else if (eventStr.StartWith(WPS_FAIL_STR)) {
            HandleWpsFailEvent(eventStr);
        }
        else if (eventStr.StartWith(WPS_OVERLAP_STR)) {
            mStateMachine->SendMessage(WPS_OVERLAP_EVENT);
        }
        else if (eventStr.StartWith(WPS_TIMEOUT_STR)) {
            mStateMachine->SendMessage(WPS_TIMEOUT_EVENT);
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
            if (mStateMachine2 != NULL) {
                AutoPtr<IInterface> arg =
                    TO_IINTERFACE(CoreUtils::Convert(eventStr.Substring(RX_HS20_ANQP_ICON_STR_LEN + 1)));
                mStateMachine2->SendMessage(RX_HS20_ANQP_ICON_EVENT,arg);
            }
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
        AutoPtr<IInterface> arg = TO_IINTERFACE(CoreUtils::Convert(substr));
        mStateMachine->SendMessage((event == SSID_TEMP_DISABLE)? SSID_TEMP_DISABLED:SSID_REENABLED, netId, 0, arg);
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
            if (++sRecvErrors > MAX_RECV_ERRORS) {
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
        mStateMachine->SendMessage(SUP_DISCONNECTION_EVENT, eventLogCounter);
        return TRUE;
    }
    else if (event == EAP_FAILURE) {
        if (eventData.StartWith(EAP_AUTH_FAILURE_STR)) {
            Logger::E(TAG, "WifiMonitor send auth failure (EAP_AUTH_FAILURE) ");
            mStateMachine->SendMessage(AUTHENTICATION_FAILURE_EVENT);
        }
    }
    else if (event == ASSOC_REJECT) {
        AutoPtr<IMatcher> match;
        mAssocRejectEventPattern->Matcher(eventData, (IMatcher**)&match);
        String BSSID("");
        Int32 status = -1;
        Boolean find;
        if (!(match->Find(&find), find)) {
            if (DBG) Logger::D(TAG, "Assoc Reject: Could not parse assoc reject string");
        } else {
            IMatchResult* matchResult = IMatchResult::Probe(match);
            matchResult->Group(1, &BSSID);
            String g2;
            matchResult->Group(2, &g2);
            //try {
            status = StringUtils::ParseInt32(g2);
            //} catch (NumberFormatException e) {
            //    status = -1;
            //}
        }
        mStateMachine->SendMessage(ASSOCIATION_REJECTION_EVENT, eventLogCounter, status, CoreUtils::Convert(BSSID));
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

void WifiMonitor::HandleDriverEvent(
    /* [in] */ const String& state)
{
    if (state.IsNull()) {
        return;
    }
    if (state.Equals("HANGED")) {
        mStateMachine->SendMessage(DRIVER_HUNG_EVENT);
    }
}

void WifiMonitor::HandleEvent(
    /* [in] */ Int32 event,
    /* [in] */ const String& remainder)
{
    if (DBG) {
        Logger::D(TAG, "handleEvent %d %s", event, remainder.string());
    }
    switch (event) {
        case DISCONNECTED:
            //Logger::D(WifiMonitor::TAG, "TODO: should be delete");
            //property_set("net.state", "0");
            HandleNetworkStateChange(Elastos::Droid::Net::NetworkInfoDetailedState_DISCONNECTED, remainder);
            break;

        case CONNECTED:
            //Logger::D(WifiMonitor::TAG, "TODO: should be delete");
            //property_set("net.state", "1");
            HandleNetworkStateChange(Elastos::Droid::Net::NetworkInfoDetailedState_CONNECTED, remainder);
            break;

        case SCAN_RESULTS:
            mStateMachine->SendMessage(SCAN_RESULTS_EVENT);
            break;

        case UNKNOWN:
            if (DBG) {
                Logger::D(TAG, "handleEvent unknown: %d, %s", event, remainder.string());
            }
            break;
        default:
            break;
    }
}

void WifiMonitor::HandleTargetSSIDEvent(
    /* [in] */ const String& eventStr)
{
    String SSID(NULL);
    AutoPtr<IMatcher> match;
    mTargetSSIDPattern->Matcher(eventStr, (IMatcher**)&match);
    Boolean find;
    if (match->Find(&find), find) {
        IMatchResult::Probe(match)->Group(1, &SSID);
    } else {
        Logger::D(TAG, "didn't find SSID %s", eventStr.string());
    }
    mStateMachine->SendMessage(WifiStateMachine::CMD_TARGET_SSID,
            eventLogCounter, 0, CoreUtils::Convert(SSID));
}

void WifiMonitor::HandleTargetBSSIDEvent(
    /* [in] */ const String& eventStr)
{
    String BSSID(NULL);
    AutoPtr<IMatcher> match;
    mTargetBSSIDPattern->Matcher(eventStr, (IMatcher**)&match);
    Boolean find;
    if (match->Find(&find), find) {
        IMatchResult::Probe(match)->Group(1, &BSSID);
    } else {
        Logger::D(TAG, "didn't find BSSID %s", eventStr.string());
    }
    mStateMachine->SendMessage(WifiStateMachine::CMD_TARGET_BSSID, eventLogCounter, 0, CoreUtils::Convert(BSSID));
}

void WifiMonitor::HandleAssociatedBSSIDEvent(
    /* [in] */ const String& eventStr)
{
    String BSSID(NULL);
    AutoPtr<IMatcher> match;
    mAssociatedPattern->Matcher(eventStr, (IMatcher**)&match);
    Boolean find;
    if (match->Find(&find), find) {
        IMatchResult* matchResult = IMatchResult::Probe(match);
        matchResult->Group(1, &BSSID);
    } else {
        Logger::D(TAG, "handleAssociatedBSSIDEvent: didn't find BSSID %s", eventStr.string());
    }
    mStateMachine->SendMessage(WifiStateMachine::CMD_ASSOCIATED_BSSID, eventLogCounter, 0, CoreUtils::Convert(BSSID));
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
        IMatchResult* matchResult = IMatchResult::Probe(match);
        matchResult->Group(1, &cfgErrStr);
        String reasonStr;
        matchResult->Group(2, &reasonStr);

        if (!reasonStr.IsNull()) {
            Int32 reasonInt = StringUtils::ParseInt32(reasonStr);
            switch(reasonInt) {
                case WifiMonitor::REASON_TKIP_ONLY_PROHIBITED:
                    mStateMachine->SendMessage(mStateMachine->ObtainMessage(WPS_FAIL_EVENT,
                            IWifiManager::WPS_TKIP_ONLY_PROHIBITED, 0));
                    return;
                case WifiMonitor::REASON_WEP_PROHIBITED:
                    mStateMachine->SendMessage(mStateMachine->ObtainMessage(WPS_FAIL_EVENT,
                            IWifiManager::WPS_WEP_PROHIBITED, 0));
                    return;
                default:
                    reason = reasonInt;
                    break;
            }
        }
        if (!cfgErrStr.IsNull()) {
            Int32 cfgErrInt = StringUtils::ParseInt32(cfgErrStr);
            switch(cfgErrInt) {
                case CONFIG_AUTH_FAILURE:
                    mStateMachine->SendMessage(mStateMachine->ObtainMessage(WPS_FAIL_EVENT,
                            IWifiManager::WPS_AUTH_FAILURE, 0));
                    return;
                case CONFIG_MULTIPLE_PBC_DETECTED:
                    mStateMachine->SendMessage(mStateMachine->ObtainMessage(WPS_FAIL_EVENT,
                            IWifiManager::WPS_OVERLAP_ERROR, 0));
                    return;
                default:
                    if (reason == 0) reason = cfgErrInt;
                    break;
            }
        }
    }
    //For all other errors, return a generic internal error
    AutoPtr<IMessage> msg;
    mStateMachine->ObtainMessage(WPS_FAIL_EVENT,IWifiManager::ERROR, reason, (IMessage**)&msg);
    mStateMachine->SendMessage(msg);
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
        mStateMachine->SendMessage(P2P_DEVICE_FOUND_EVENT, device);
    }
    else if (dataString.StartWith(P2P_DEVICE_LOST_STR)) {
        AutoPtr<IWifiP2pDevice> device;
        ASSERT_SUCCEEDED(CWifiP2pDevice::New(dataString, (IWifiP2pDevice**)&device));
        mStateMachine->SendMessage(P2P_DEVICE_LOST_EVENT, device);
    }
    else if (dataString.StartWith(P2P_FIND_STOPPED_STR)) {
        mStateMachine->SendMessage(P2P_FIND_STOPPED_EVENT);
    }
    else if (dataString.StartWith(P2P_GO_NEG_REQUEST_STR)) {
        AutoPtr<IWifiP2pConfig> config;
        ASSERT_SUCCEEDED(CWifiP2pConfig::New(dataString, (IWifiP2pConfig**)&config));
        mStateMachine->SendMessage(P2P_GO_NEGOTIATION_REQUEST_EVENT, config);
    }
    else if (dataString.StartWith(P2P_GO_NEG_SUCCESS_STR)) {
        mStateMachine->SendMessage(P2P_GO_NEGOTIATION_SUCCESS_EVENT);
    }
    else if (dataString.StartWith(P2P_GO_NEG_FAILURE_STR)) {
        Int32 value = P2pError(dataString);
        AutoPtr<IInteger32> iobj;
        CInteger32::New(value, (IInteger32**)&iobj);
        mStateMachine->SendMessage(P2P_GO_NEGOTIATION_FAILURE_EVENT, iobj);
    }
    else if (dataString.StartWith(P2P_GROUP_FORMATION_SUCCESS_STR)) {
        mStateMachine->SendMessage(P2P_GROUP_FORMATION_SUCCESS_EVENT);
    }
    else if (dataString.StartWith(P2P_GROUP_FORMATION_FAILURE_STR)) {
        Int32 value = P2pError(dataString);
        AutoPtr<IInteger32> iobj;
        CInteger32::New(value, (IInteger32**)&iobj);
        mStateMachine->SendMessage(P2P_GROUP_FORMATION_FAILURE_EVENT, iobj);
    }
    else if (dataString.StartWith(P2P_GROUP_STARTED_STR)) {
        AutoPtr<IWifiP2pGroup> group;
        ASSERT_SUCCEEDED(CWifiP2pGroup::New(dataString, (IWifiP2pGroup**)&group));
        mStateMachine->SendMessage(P2P_GROUP_STARTED_EVENT, group);
    }
    else if (dataString.StartWith(P2P_GROUP_REMOVED_STR)) {
        AutoPtr<IWifiP2pGroup> group;
        ASSERT_SUCCEEDED(CWifiP2pGroup::New(dataString, (IWifiP2pGroup**)&group));
        mStateMachine->SendMessage(P2P_GROUP_REMOVED_EVENT, group);
    }
    else if (dataString.StartWith(P2P_INVITATION_RECEIVED_STR)) {
        AutoPtr<IWifiP2pGroup> group;
        ASSERT_SUCCEEDED(CWifiP2pGroup::New(dataString, (IWifiP2pGroup**)&group));
        mStateMachine->SendMessage(P2P_INVITATION_RECEIVED_EVENT, group);
    }
    else if (dataString.StartWith(P2P_INVITATION_RESULT_STR)) {
        Int32 value = P2pError(dataString);
        AutoPtr<IInteger32> iobj;
        CInteger32::New(value, (IInteger32**)&iobj);
        mStateMachine->SendMessage(P2P_INVITATION_RESULT_EVENT, iobj);
    }
    else if (dataString.StartWith(P2P_PROV_DISC_PBC_REQ_STR)) {
        AutoPtr<IWifiP2pProvDiscEvent> event;
        ASSERT_SUCCEEDED(CWifiP2pProvDiscEvent::New(dataString, (IWifiP2pProvDiscEvent**)&event));
        mStateMachine->SendMessage(P2P_PROV_DISC_PBC_REQ_EVENT, event);
    }
    else if (dataString.StartWith(P2P_PROV_DISC_PBC_RSP_STR)) {
        AutoPtr<IWifiP2pProvDiscEvent> event;
        ASSERT_SUCCEEDED(CWifiP2pProvDiscEvent::New(dataString, (IWifiP2pProvDiscEvent**)&event));
        mStateMachine->SendMessage(P2P_PROV_DISC_PBC_RSP_EVENT, event);
    }
    else if (dataString.StartWith(P2P_PROV_DISC_ENTER_PIN_STR)) {
        AutoPtr<IWifiP2pProvDiscEvent> event;
        ASSERT_SUCCEEDED(CWifiP2pProvDiscEvent::New(dataString, (IWifiP2pProvDiscEvent**)&event));
        mStateMachine->SendMessage(P2P_PROV_DISC_ENTER_PIN_EVENT, event);
    }
    else if (dataString.StartWith(P2P_PROV_DISC_SHOW_PIN_STR)) {
        AutoPtr<IWifiP2pProvDiscEvent> event;
        ASSERT_SUCCEEDED(CWifiP2pProvDiscEvent::New(dataString, (IWifiP2pProvDiscEvent**)&event));
        mStateMachine->SendMessage(P2P_PROV_DISC_SHOW_PIN_EVENT, event);
    }
    else if (dataString.StartWith(P2P_PROV_DISC_FAILURE_STR)) {
        mStateMachine->SendMessage(P2P_PROV_DISC_FAILURE_EVENT);
    }
    else if (dataString.StartWith(P2P_SERV_DISC_RESP_STR)) {
        AutoPtr<IWifiP2pServiceResponseHelper> helper;
        CWifiP2pServiceResponseHelper::AcquireSingleton((IWifiP2pServiceResponseHelper**)&helper);
        AutoPtr<IList> list;
        ASSERT_SUCCEEDED(helper->NewInstance(dataString, (IList**)&list));
        Int32 size;
        if (list != NULL && (list->GetSize(&size), size) > 0) {
            mStateMachine->SendMessage(P2P_SERV_DISC_RESP_EVENT, TO_IINTERFACE(list));
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
        mStateMachine->SendMessage(AP_STA_CONNECTED_EVENT, device);
    /* AP-STA-DISCONNECTED 42:fc:89:a8:96:09 p2p_dev_addr=02:90:4c:a0:92:54 */
    }
    else if ((*tokens)[0].Equals(AP_STA_DISCONNECTED_STR)) {
        AutoPtr<IWifiP2pDevice> device;
        CWifiP2pDevice::New(dataString, (IWifiP2pDevice**)&device);
        mStateMachine->SendMessage(AP_STA_DISCONNECTED_EVENT, device);
    }
}

/**
  * Handle ANQP events
  */
void WifiMonitor::HandleGasQueryEvents(
    /* [in] */ const String& dataString)
{
    // hs20
    if (mStateMachine2 == NULL) return;
    if (dataString.StartWith(GAS_QUERY_START_STR)) {
        mStateMachine2->SendMessage(GAS_QUERY_START_EVENT);
    } else if (dataString.StartWith(GAS_QUERY_DONE_STR)) {
        AutoPtr< ArrayOf<String> > dataTokens;
        StringUtils::Split(dataString, String(" "), (ArrayOf<String>**)&dataTokens);
        String bssid(NULL);
        Int32 success = 0;
        for (Int32 i = 0; i < dataTokens->GetLength(); ++i) {
            String token = (*dataTokens)[i];
            AutoPtr< ArrayOf<String> > nameValue;
            StringUtils::Split(token, String("="), (ArrayOf<String>**)&nameValue);
            if (nameValue->GetLength() != 2) {
                continue;
            }
            if ((*nameValue)[0].Equals(String("addr"))) {
                bssid = (*nameValue)[1];
                continue;
            }
            if ((*nameValue)[0].Equals(String("result")))  {
                success = (*nameValue)[1].Equals(String("SUCCESS")) ? 1 : 0;
                continue;
            }
        }
        mStateMachine2->SendMessage(GAS_QUERY_DONE_EVENT, success, 0, CoreUtils::Convert(bssid));
    } else {
        if (DBG) Logger::D(TAG, "Unknown GAS query event: %s", dataString.string());
    }
}

/**
 * Handle HS20 events
 */
void WifiMonitor::HandleHs20Events(
    /* [in] */ const String& dataString)
{
    if (mStateMachine2 == NULL) return;
    if (dataString.StartWith(HS20_SUB_REM_STR)) {
        // format: HS20-SUBSCRIPTION-REMEDIATION osu_method, url
        AutoPtr< ArrayOf<String> > dataTokens;
        StringUtils::Split(dataString, String(" "), (ArrayOf<String>**)&dataTokens);
        Int32 method = -1;
        String url(NULL);
        if (dataTokens->GetLength() >= 3) {
            method = StringUtils::ParseInt32((*dataTokens)[1]);
            url = (*dataTokens)[2];
        }
        mStateMachine2->SendMessage(HS20_REMEDIATION_EVENT, method, 0, CoreUtils::Convert(url));
    } else if (dataString.StartWith(HS20_DEAUTH_STR)) {
        // format: HS20-DEAUTH-IMMINENT-NOTICE code, delay, url
        Int32 code = -1;
        Int32 delay = -1;
        String url(NULL);
        AutoPtr< ArrayOf<String> > dataTokens;
        StringUtils::Split(dataString, String(" "), (ArrayOf<String>**)&dataTokens);
        if (dataTokens->GetLength() >= 4) {
            code = StringUtils::ParseInt32((*dataTokens)[1]);
            delay = StringUtils::ParseInt32((*dataTokens)[2]);
            url = (*dataTokens)[3];
        }
        mStateMachine2->SendMessage(HS20_DEAUTH_EVENT, code, delay, CoreUtils::Convert(url));
    } else {
        if (DBG) Logger::D(TAG, "Unknown HS20 event: %s", dataString.string());
    }
}

/**
 * Handle Supplicant Requests
 */
void WifiMonitor::HandleRequests(
    /* [in] */ const String& dataString)
{
    String SSID(NULL);
    Int32 reason = -2;
    String requestName = dataString.Substring(REQUEST_PREFIX_LEN_STR);
    if (TextUtils::IsEmpty(requestName)) {
        return;
    }
    if (requestName.StartWith(IDENTITY_STR)) {
        AutoPtr<IMatcher> match;
        mRequestIdentityPattern->Matcher(requestName, (IMatcher**)&match);
        Boolean find;
        if (match->Find(&find), find) {
            IMatchResult* matchResult = IMatchResult::Probe(match);
            matchResult->Group(2, &SSID);
            //try {
            String g1;
            matchResult->Group(1, &g1);
            reason = StringUtils::ParseInt32(g1);
            //} catch (NumberFormatException e) {
            //    reason = -1;
            //}
        } else {
            Logger::E(TAG, "didn't find SSID %s", requestName.string());
        }
        mStateMachine->SendMessage(SUP_REQUEST_IDENTITY, eventLogCounter, reason, CoreUtils::Convert(SSID));
    } if (requestName.StartWith(SIM_STR)) {
        AutoPtr<IMatcher> match;
        mRequestGsmAuthPattern->Matcher(requestName, (IMatcher**)&match);
        Boolean find;
        if (match->Find(&find), find) {
            AutoPtr<WifiStateMachine::SimAuthRequestData> data =
                new WifiStateMachine::SimAuthRequestData();
            IMatchResult* matchResult = IMatchResult::Probe(match);
            String g1;
            matchResult->Group(1, &g1);
            data->networkId = StringUtils::ParseInt32(g1);
            data->protocol = IWifiEnterpriseConfigEap::SIM;
            String ssid;
            matchResult->Group(4, &ssid);
            data->ssid = ssid;
            AutoPtr<ArrayOf<String> > challenges;
            String g2;
            matchResult->Group(2, &g2);
            StringUtils::Split(g2, String("::"), (ArrayOf<String>**)&challenges);
            data->challenges = challenges;
            mStateMachine->SendMessage(SUP_REQUEST_SIM_AUTH, TO_IINTERFACE(data));
        } else {
            Logger::E(TAG, "couldn't parse SIM auth request - %s", requestName.string());
        }

    } else {
        if (DBG) Logger::W(TAG, "couldn't identify request type - %s", dataString.string());
    }
}

void WifiMonitor::HandleSupplicantStateChange(
    /* [in] */ const String& dataString)
{
    if (DBG) Logger::D("WifiMonitor", "HandleSupplicantStateChange String:%s\n", dataString.string());
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
        //ECode ec;
        String token;
        for (Int32 i = 0; i < dataTokens->GetLength(); ++i) {
            token = (*dataTokens)[i];
            AutoPtr<ArrayOf<String> > nameValue;
            StringUtils::Split(token, String("="), (ArrayOf<String>**)&nameValue);

            if (nameValue == NULL || nameValue->GetLength() != 2) {
                continue;
            }
            if (DBG)
                Logger::D("WifiMonitor", "line:%d, name:%s, value:%s\n",
                    __LINE__, (*nameValue)[0].string(), (*nameValue)[1].string());

            if ((*nameValue)[0].Equals("BSSID")) {
                BSSID = (*nameValue)[1];
                continue;
            }

            Int32 value = StringUtils::ParseInt32((*nameValue)[1]);
            //if (ec == (ECode)E_NUMBER_FORMAT_EXCEPTION) continue;

            if ((*nameValue)[0].Equals("id")) {
                networkId = value;
            }
            else if ((*nameValue)[0].Equals("state")) {
                newState = value;
            }
        }
    }

    if (newState == -1) return;

    if (newState == Elastos::Droid::Wifi::SupplicantState_INVALID) {
        Logger::W(TAG, "Invalid supplicant state: %d", newState);
    }

    NotifySupplicantStateChange(networkId, wifiSsid, BSSID, newState);
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
    mInterfaceName = wifiNative->mInterfaceName;
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
    Int32 local = 0;
    AutoPtr<IMatcher> match;
    if (newState == Elastos::Droid::Net::NetworkInfoDetailedState_CONNECTED) {
        mConnectedEventPattern->Matcher(data, (IMatcher**)&match);
        Boolean found;
        if (match->Find(&found), !found) {
            if (DBG) Logger::D(TAG, "handleNetworkStateChange: Could not find BSSID in CONNECTED event string");
        }
        else {
            IMatchResult* matchResult = IMatchResult::Probe(match);
            matchResult->Group(1, &BSSID);
            // try {
            String id;
            matchResult->Group(2, &id);
            networkId = StringUtils::ParseInt32(id);
            // } catch (NumberFormatException e) {
            //     networkId = -1;
        // }
        }
        NotifyNetworkStateChange(newState, BSSID, networkId, reason);
    }
    else if (newState == Elastos::Droid::Net::NetworkInfoDetailedState_DISCONNECTED) {
        mDisconnectedEventPattern->Matcher(data, (IMatcher**)&match);
        Boolean found;
        if (match->Find(&found), !found) {
            if (DBG) Logger::D(TAG, "handleNetworkStateChange: Could not parse disconnect string");
        } else {
            IMatchResult* matchResult = IMatchResult::Probe(match);
            matchResult->Group(1, &BSSID);
            //try {
            String id;
            matchResult->Group(2, &id);
            reason = StringUtils::ParseInt32(id);
            //} catch (NumberFormatException e) {
            //    reason = -1;
            //}
            //try {
            String lStr;
            matchResult->Group(3, &lStr);
            local = StringUtils::ParseInt32(lStr);
            //} catch (NumberFormatException e) {
            //    local = -1;
            //}
        }
        NotifyNetworkStateChange(newState, BSSID, local, reason);
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
    if (newState == Elastos::Droid::Net::NetworkInfoDetailedState_CONNECTED) {
        AutoPtr<IMessage> m;
        mStateMachine->ObtainMessage(NETWORK_CONNECTION_EVENT,
                netId, reason, TO_IINTERFACE(cs), (IMessage**)&m);
        mStateMachine->SendMessage(m);
    }
    else {
        AutoPtr<IMessage> m;
        mStateMachine->ObtainMessage(NETWORK_DISCONNECTION_EVENT,
                netId, reason, TO_IINTERFACE(cs), (IMessage**)&m);
        if (DBG) Logger::D(TAG, "WifiMonitor notify network disconnect: %s, reason=%d", BSSID.string(), reason);
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
        Logger::W("MonitorThread", "NotifySupplicantStateChange BSSID %s, wifiSsid: %p, state:%d",
                BSSID.string(), wifiSsid, newState);
    }

    AutoPtr<CStateChangeResult> result = new CStateChangeResult(networkId, wifiSsid, BSSID, newState);
    AutoPtr<IMessage> m;
    mStateMachine->ObtainMessage(SUPPLICANT_STATE_CHANGE_EVENT, eventLogCounter, 0, TO_IINTERFACE(result), (IMessage**)&m);
    mStateMachine->SendMessage(m);
}

} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namespace Elastos
