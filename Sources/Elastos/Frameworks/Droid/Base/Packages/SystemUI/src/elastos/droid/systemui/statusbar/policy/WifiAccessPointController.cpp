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

#include "elastos/droid/systemui/statusbar/policy/WifiAccessPointController.h"
#include "elastos/droid/systemui/statusbar/policy/NetworkController.h"
#include "../R.h"
#include <elastos/droid/text/TextUtils.h>
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Utility::CArrayMap;
using Elastos::Droid::Utility::CArrayMapHelper;
using Elastos::Droid::Utility::CArraySet;
using Elastos::Droid::Utility::IArrayMapHelper;
using Elastos::Droid::Utility::IArraySet;
using Elastos::Droid::Wifi::CWifiManagerHelper;
using Elastos::Droid::Wifi::EIID_IWifiManagerActionListener;
using Elastos::Droid::Wifi::IScanResult;
using Elastos::Droid::Wifi::IWifiConfiguration;
using Elastos::Droid::Wifi::IWifiManagerHelper;
using Elastos::Droid::Wifi::IWifiInfo;
using Elastos::Core::CString;
using Elastos::Core::EIID_IComparator;
using Elastos::Core::ICharSequence;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CCollections;
using Elastos::Utility::ICollection;
using Elastos::Utility::ICollections;
using Elastos::Utility::IIterator;
using Elastos::Utility::IList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

const String WifiAccessPointController::TAG("WifiAccessPointController");
const Boolean WifiAccessPointController::DEBUG = FALSE;
const Int32 WifiAccessPointController::ICONS[5] = {
    R::drawable::ic_qs_wifi_0,
    R::drawable::ic_qs_wifi_full_1,
    R::drawable::ic_qs_wifi_full_2,
    R::drawable::ic_qs_wifi_full_3,
    R::drawable::ic_qs_wifi_full_4,
};

WifiAccessPointController::Receiver::Receiver(
    /* [in] */ WifiAccessPointController* host)
    : mRegistered(FALSE)
    , mHost(host)
{}

ECode WifiAccessPointController::Receiver::SetListening(
    /* [in] */ Boolean listening)
{
    if (listening && !mRegistered) {
        if (DEBUG) Logger::D(TAG, "Registering receiver");
        AutoPtr<IIntentFilter> filter;
        CIntentFilter::New((IIntentFilter**)&filter);
        filter->AddAction(IWifiManager::WIFI_STATE_CHANGED_ACTION);
        filter->AddAction(IWifiManager::SCAN_RESULTS_AVAILABLE_ACTION);
        filter->AddAction(IWifiManager::NETWORK_IDS_CHANGED_ACTION);
        filter->AddAction(IWifiManager::SUPPLICANT_STATE_CHANGED_ACTION);
        filter->AddAction(IWifiManager::CONFIGURED_NETWORKS_CHANGED_ACTION);
        filter->AddAction(IWifiManager::LINK_CONFIGURATION_CHANGED_ACTION);
        filter->AddAction(IWifiManager::NETWORK_STATE_CHANGED_ACTION);
        filter->AddAction(IWifiManager::RSSI_CHANGED_ACTION);

        AutoPtr<IIntent> i;
        mHost->mContext->RegisterReceiver(this, filter, (IIntent**)&i);
        mRegistered = TRUE;
    }
    else if (!listening && mRegistered) {
        if (DEBUG) Logger::D(TAG, "Unregistering receiver");
        mHost->mContext->UnregisterReceiver(this);
        mRegistered = FALSE;
    }
    return NOERROR;
}

ECode WifiAccessPointController::Receiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (DEBUG) Logger::D(TAG, "onReceive %s", action.string());
    if (IWifiManager::SCAN_RESULTS_AVAILABLE_ACTION.Equals(action)) {
        mHost->UpdateAccessPoints();
        mHost->mScanning = FALSE;
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(WifiAccessPointController::WifiComparator, Object, IComparator)
ECode WifiAccessPointController::WifiComparator::Compare(
    /* [in] */ IInterface* lhs,
    /* [in] */ IInterface* rhs,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = -Elastos::Core::Math::Compare(Score(lhs), Score(rhs));
    return NOERROR;
}

Int32 WifiAccessPointController::WifiComparator::Score(
    /* [in] */ IInterface* ap)
{
    Int32 level = 0;
    INetworkControllerAccessPoint::Probe(ap)->GetLevel(&level);
    Boolean isConnected = FALSE;
    INetworkControllerAccessPoint::Probe(ap)->IsConnected(&isConnected);
    return level + (isConnected ? 10 : 0);
}

CAR_INTERFACE_IMPL(WifiAccessPointController::Listener, Object, IWifiManagerActionListener)
ECode WifiAccessPointController::Listener::OnSuccess()
{
    if (DEBUG) Logger::D(TAG, "connect success");
    return NOERROR;
}

ECode WifiAccessPointController::Listener::OnFailure(
    /* [in] */ Int32 reason)
{
    if (DEBUG) Logger::D(TAG, "connect failure reason=%d", reason);
    return NOERROR;
}


CAR_INTERFACE_IMPL(WifiAccessPointController, Object, IWifiAccessPointController)
WifiAccessPointController::WifiAccessPointController(
    /* [in] */ IContext* context)
    : mScanning(FALSE)
{
    CArrayList::New((IArrayList**)&mCallbacks);
    mReceiver = new Receiver(this);
    mByStrength = new WifiComparator();
    mContext = context;
    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::WIFI_SERVICE, (IInterface**)&obj);
    mWifiManager = IWifiManager::Probe(obj);
}

ECode WifiAccessPointController::AddCallback(
    /* [in] */ INetworkControllerAccessPointCallback* callback)
{
    Boolean tmp = FALSE;
    if (callback == NULL || (mCallbacks->Contains(callback, &tmp), tmp)) return NOERROR;
    if (DEBUG) Logger::D(TAG, "addCallback %p", callback);
    mCallbacks->Add(callback);
    mReceiver->SetListening((mCallbacks->IsEmpty(&tmp), !tmp));
    return NOERROR;
}

ECode WifiAccessPointController::RemoveCallback(
    /* [in] */ INetworkControllerAccessPointCallback* callback)
{
    if (callback == NULL) return NOERROR;
    if (DEBUG) Logger::D(TAG, "removeCallback %p", callback);
    mCallbacks->Remove(callback);
    Boolean tmp = FALSE;
    mReceiver->SetListening((mCallbacks->IsEmpty(&tmp), !tmp));
    return NOERROR;
}

ECode WifiAccessPointController::Scan()
{
    if (mScanning) return NOERROR;
    if (DEBUG) Logger::D(TAG, "scan!");
    mWifiManager->StartScan(&mScanning);
    return NOERROR;
}

ECode WifiAccessPointController::Connect(
    /* [in] */ INetworkControllerAccessPoint* ap)
{
    Int32 networkId = 0;
    if (ap == NULL || (ap->GetNetworkId(&networkId), networkId) < 0) return NOERROR;
    ap->GetNetworkId(&networkId);
    if (DEBUG) Logger::D(TAG, "connect networkId=%d", networkId);
    AutoPtr<Listener> l = new Listener();
    mWifiManager->Connect(networkId, l);
    return NOERROR;
}

ECode WifiAccessPointController::FireCallback(
    /* [in] */ ArrayOf<IInterface*>* aps)
{
    AutoPtr<ArrayOf<INetworkControllerAccessPoint*> > array =
            ArrayOf<INetworkControllerAccessPoint*>::Alloc(aps->GetLength());

    for (Int32 i = 0; i < aps->GetLength(); i++) {
        array->Set(i, INetworkControllerAccessPoint::Probe((*aps)[i]));
    }
    AutoPtr<IIterator> ator;
    mCallbacks->GetIterator((IIterator**)&ator);
    Boolean has = FALSE;
    while (ator->HasNext(&has), has) {
        AutoPtr<IInterface> callback;
        ator->GetNext((IInterface**)&callback);
        INetworkControllerAccessPointCallback::Probe(callback)->OnAccessPointsChanged(array);
    }
    return NOERROR;
}

String WifiAccessPointController::TrimDoubleQuotes(
    /* [in] */ const String& v)
{
    return v != NULL && v.GetLength() >= 2 && v.GetChar(0) == '\"'
            && v.GetChar(v.GetLength() - 1) == '\"' ? v.Substring(1, v.GetLength() - 1) : v;
}

Int32 WifiAccessPointController::GetConnectedNetworkId()
{
    AutoPtr<IWifiInfo> wifiInfo;
    mWifiManager->GetConnectionInfo((IWifiInfo**)&wifiInfo);
    Int32 id = 0;
    return wifiInfo != NULL ? (wifiInfo->GetNetworkId(&id), id) : INetworkControllerAccessPoint::NO_NETWORK;
}

AutoPtr<IArrayMap/*<String, WifiConfiguration*/> WifiAccessPointController::GetConfiguredNetworksBySsid()
{
    AutoPtr<IList> configs;  /*<WifiConfiguration*/
    mWifiManager->GetConfiguredNetworks((IList**)&configs);

    Int32 size = 0;
    if (configs == NULL || (configs->GetSize(&size), size) == 0) {
        AutoPtr<IArrayMapHelper> helper;
        CArrayMapHelper::AcquireSingleton((IArrayMapHelper**)&helper);
        AutoPtr<IArrayMap> am;
        helper->GetEMPTY((IArrayMap**)&am);
        return am;
    }
    AutoPtr<IArrayMap> rt;  /*<String, WifiConfiguration*/
    CArrayMap::New((IArrayMap**)&rt);

    AutoPtr<IIterator> ator;
    configs->GetIterator((IIterator**)&ator);
    Boolean has = FALSE;
    while (ator->HasNext(&has), has) {
        AutoPtr<IInterface> config;
        ator->GetNext((IInterface**)&config);
        String ssid;
        IWifiConfiguration::Probe(config)->GetSSID(&ssid);
        AutoPtr<ICharSequence> key;
        CString::New(TrimDoubleQuotes(ssid), (ICharSequence**)&key);
        rt->Put(key, config);
    }
    return rt;
}

void WifiAccessPointController::UpdateAccessPoints()
{
    const Int32 connectedNetworkId = GetConnectedNetworkId();
    if (DEBUG) Logger::D(TAG, "connectedNetworkId: %d", connectedNetworkId);
    AutoPtr<IList> scanResults;  /*<ScanResult*/
    mWifiManager->GetScanResults((IList**)&scanResults);
    AutoPtr<IArrayMap> configured = GetConfiguredNetworksBySsid();  /*<String, WifiConfiguration*/
    if (DEBUG) Logger::D(TAG, "scanResults: %p", scanResults.Get());
    AutoPtr<IList> aps;  /*<AccessPoint*/
    Int32 size = 0;
    scanResults->GetSize(&size);
    CArrayList::New(size, (IList**)&aps);
    AutoPtr<IArraySet> ssids;  /*<String*/
    CArraySet::New((IArraySet**)&ssids);

    AutoPtr<IWifiManagerHelper> helper;
    CWifiManagerHelper::AcquireSingleton((IWifiManagerHelper**)&helper);
    AutoPtr<IIterator> ator;
    scanResults->GetIterator((IIterator**)&ator);
    Boolean has = FALSE;
    while (ator->HasNext(&has), has) {
        AutoPtr<IInterface> obj;
        ator->GetNext((IInterface**)&obj);
        IScanResult* scanResult = IScanResult::Probe(obj);
        if (scanResult == NULL) {
            continue;
        }
        String ssid;
        scanResult->GetSSID(&ssid);
        AutoPtr<ICharSequence> key;
        CString::New(ssid, (ICharSequence**)&key);

        Boolean tmp = FALSE;
        if (TextUtils::IsEmpty(ssid) || (ICollection::Probe(ssids)->Contains(key, &tmp), tmp)) continue;

        if (configured->ContainsKey(key, &tmp), !tmp) continue;
        ICollection::Probe(ssids)->Add(key);

        obj = NULL;
        configured->Get(key, (IInterface**)&obj);
        IWifiConfiguration* config = IWifiConfiguration::Probe(obj);
        Int32 level = 0, sl = 0;
        scanResult->GetLevel(&sl);
        helper->CalculateSignalLevel(sl, /*ICONS.length*/5, &level);
        AutoPtr<INetworkControllerAccessPoint> ap = new AccessPoint();

        Int32 networkId = 0;
        config->GetNetworkId(&networkId);
        ap->SetNetworkId(config != NULL ? networkId : INetworkControllerAccessPoint::NO_NETWORK);
        ap->SetSsid(ssid);
        ap->SetIconId(ICONS[level]);

        ap->GetNetworkId(&networkId);
        ap->SetIsConnected(networkId != INetworkControllerAccessPoint::NO_NETWORK
                        && networkId == connectedNetworkId);
        ap->SetLevel(level);
        aps->Add(ap);
    }

    AutoPtr<ICollections> c;
    CCollections::AcquireSingleton((ICollections**)&c);
    c->Sort(aps, mByStrength);

    aps->GetSize(&size);
    AutoPtr<ArrayOf<IInterface* > > aparray = ArrayOf<IInterface* >::Alloc(size);
    AutoPtr<ArrayOf<IInterface *> > result;
    aps->ToArray(aparray, (ArrayOf<IInterface *>**)&result);
    FireCallback(result);
}

} // namespace Policy
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
