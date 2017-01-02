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

#include <elastos/droid/server/connectivity/Nat464Xlat.h>
#include <elastos/droid/server/connectivity/NetworkAgentInfo.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Slogger.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Net.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.Net.h>

using Elastos::Droid::Net::CLinkProperties;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Net::INetworkUtils;
using Elastos::Droid::Net::CNetworkUtils;
using Elastos::Droid::Net::CRouteInfo;
using Elastos::Droid::Net::CLinkAddress;
using Elastos::Core::CoreUtils;
using Elastos::Utility::IList;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Slogger;
using Elastos::Net::IInetAddress;
using Elastos::Net::IInet4AddressHelper;
using Elastos::Net::CInet4AddressHelper;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Connectivity {

// This must match the interface name in clatd.conf.
const String Nat464Xlat::CLAT_INTERFACE_NAME("clat4");
const String Nat464Xlat::TAG("Nat464Xlat");

Nat464Xlat::Nat464Xlat()
    : mIsStarted(FALSE)
    , mIsRunning(FALSE)
{}

Nat464Xlat::~Nat464Xlat()
{}

ECode Nat464Xlat::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IINetworkManagementService* nmService,
    /* [in] */ IIConnectivityManager* connService,
    /* [in] */ IHandler* handler)
{
    BaseNetworkObserver::constructor();

    mContext = context;
    mNMService = nmService;
    mConnService = connService;
    mHandler = handler;

    mIsStarted = FALSE;
    mIsRunning = FALSE;
    CLinkProperties::New((ILinkProperties**)&mLP);

    // If this is a runtime restart, it's possible that clatd is already
    // running, but we don't know about it. If so, stop it.
    // try {
    Boolean bval;
    mNMService->IsClatdStarted(&bval);
    if (bval) {
        mNMService->StopClatd();
    }
    // } catch(RemoteException e) {}  // Well, we tried.
    return NOERROR;
}

Boolean Nat464Xlat::RequiresClat(
    /* [in ]*/ INetworkAgentInfo* nai)
{
    NetworkAgentInfo* info = (NetworkAgentInfo*)nai;
    Int32 netType;
    info->mNetworkInfo->GetType(&netType);
    Boolean connected;
    info->mNetworkInfo->IsConnected(&connected);
    Boolean hasIPv4Address = FALSE;
    if (info->mLinkProperties != NULL) {
        info->mLinkProperties->HasIPv4Address(&hasIPv4Address);
    }
    Slogger::D(TAG, "RequiresClat: netType=%d, connected=%d, hasIPv4Address=%d",
        netType, connected, hasIPv4Address);
    // Only support clat on mobile for now.
    return netType == IConnectivityManager::TYPE_MOBILE && connected && !hasIPv4Address;
}

ECode Nat464Xlat::IsRunningClat(
    /* [in] */ INetworkAgentInfo* network,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    NetworkAgentInfo* info = (NetworkAgentInfo*)network;
    *result =  mNetworkMessenger == info->mMessenger;
    return NOERROR;
}

ECode Nat464Xlat::StartClat(
    /* [in] */ INetworkAgentInfo* network)
{
    NetworkAgentInfo* info = (NetworkAgentInfo*)network;
    if (mNetworkMessenger != NULL && mNetworkMessenger != info->mMessenger) {
        Slogger::E(TAG, "startClat: too many networks requesting clat");
        return NOERROR;
    }
    mNetworkMessenger = info->mMessenger;
    AutoPtr<ILinkProperties> lp = info->mLinkProperties;
    CLinkProperties::New(lp, (ILinkProperties**)&mBaseLP);
    if (mIsStarted) {
        Slogger::E(TAG, "startClat: already started");
        return NOERROR;
    }
    String iface;
    lp->GetInterfaceName(&iface);
    Slogger::I(TAG, "Starting clatd on %s, lp=", iface.string(), TO_CSTR(lp));
    ECode ec = mNMService->StartClatd(iface);
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        Slogger::E(TAG, "Error starting clat daemon: ");
    }
    mIsStarted = TRUE;
    return NOERROR;
}

ECode Nat464Xlat::StopClat()
{
    if (mIsStarted) {
        Slogger::I(TAG, "Stopping clatd");
        ECode ec = mNMService->StopClatd();
        if (ec == (ECode)E_REMOTE_EXCEPTION) {
            Slogger::E(TAG, "Error stopping clat daemon: ");
        }
        mIsStarted = FALSE;
        mIsRunning = FALSE;
        mNetworkMessenger = NULL;
        mBaseLP = NULL;
        mLP->Clear();
    }
    else {
        Slogger::E(TAG, "stopClat: already stopped");
    }
    return NOERROR;
}

void Nat464Xlat::UpdateConnectivityService()
{
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(
        INetworkAgent::EVENT_NETWORK_PROPERTIES_CHANGED, mBaseLP.Get(), (IMessage**)&msg);
    msg->SetReplyTo(mNetworkMessenger);
    Slogger::I(TAG, "sending message to ConnectivityService: %s", TO_CSTR(msg));
    msg->SendToTarget();
}

ECode Nat464Xlat::FixupLinkProperties(
    /* [in] */ INetworkAgentInfo* nai,
    /* [in] */ ILinkProperties* oldLp)
{
    NetworkAgentInfo* info = (NetworkAgentInfo*)nai;
    Boolean bval;
    IsRunningClat(nai, &bval);
    if (bval && info->mLinkProperties != NULL) {
        AutoPtr<IList> names;
        info->mLinkProperties->GetAllInterfaceNames((IList**)&names);
        AutoPtr<ICharSequence> csq = CoreUtils::Convert(CLAT_INTERFACE_NAME);
        names->Contains((IInterface*)csq.Get(), &bval);
        if (!bval) {
            String name;
            info->mLinkProperties->GetInterfaceName(&name);
            Slogger::D(TAG, "clatd running, updating NAI for %s", name.string());
            AutoPtr<IList> links;
            oldLp->GetStackedLinks((IList**)&links);
            AutoPtr<IIterator> it;
            links->GetIterator((IIterator**)&it);
            while (it->HasNext(&bval), bval) {
                AutoPtr<IInterface> obj;
                it->GetNext((IInterface**)&obj);
                ILinkProperties* stacked = ILinkProperties::Probe(obj);
                stacked->GetInterfaceName(&name);
                if (CLAT_INTERFACE_NAME.Equals(name)) {
                    Boolean bval;
                    info->mLinkProperties->AddStackedLink(stacked, &bval);
                    break;
                }
            }
        }
    }
    return NOERROR;
}

ECode Nat464Xlat::InterfaceAdded(
    /* [in] */ const String& iface)
{
    if (iface.Equals(CLAT_INTERFACE_NAME)) {
        Slogger::I(TAG, "interface %s added, mIsRunning = %d -> true",
            CLAT_INTERFACE_NAME.string(), mIsRunning);

        mIsRunning = TRUE;

        // Create the LinkProperties for the clat interface by fetching the
        // IPv4 address for the interface and adding an IPv4 default route,
        // then stack the LinkProperties on top of the link it's running on.
        // try {
        AutoPtr<IInet4AddressHelper> helper;
        CInet4AddressHelper::AcquireSingleton((IInet4AddressHelper**)&helper);
        AutoPtr<IInterfaceConfiguration> config;
        AutoPtr<IInetAddress> any, clatAddr;
        AutoPtr<ILinkAddress> clatAddress, addr;
        AutoPtr<IRouteInfo> ipv4Default;
        Boolean bval;

        ECode ec = NOERROR;
        ec = mNMService->GetInterfaceConfig(iface, (IInterfaceConfiguration**)&config);
        FAIL_GOTO(ec, _EXIT_)

        ec = config->GetLinkAddress((ILinkAddress**)&clatAddress);
        FAIL_GOTO(ec, _EXIT_)

        mLP->Clear();
        mLP->SetInterfaceName(iface);

        // Although the clat interface is a point-to-point tunnel, we don't
        // point the route directly at the interface because some apps don't
        // understand routes without gateways (see, e.g., http://b/9597256
        // http://b/9597516). Instead, set the next hop of the route to the
        // clat IPv4 address itself (for those apps, it doesn't matter what
        // the IP of the gateway is, only that there is one).
        helper->GetANY((IInetAddress**)&any);
        CLinkAddress::New(any, 0, (ILinkAddress**)&addr);
        clatAddress->GetAddress((IInetAddress**)&clatAddr);

        CRouteInfo::New(addr, clatAddr, iface, (IRouteInfo**)&ipv4Default);
        mLP->AddRoute(ipv4Default, &bval);
        mLP->AddLinkAddress(clatAddress, &bval);
        mBaseLP->AddStackedLink(mLP, &bval);
        Slogger::I(TAG, "Adding stacked link. tracker LP: %s", TO_CSTR(mBaseLP));
        UpdateConnectivityService();
        // } catch(RemoteException e) {

_EXIT_:
        if (ec == (ECode)E_REMOTE_EXCEPTION) {
            Slogger::E(TAG, "Error getting link properties: ");
        }

        // }
    }
    return NOERROR;
}

ECode Nat464Xlat::InterfaceRemoved(
    /* [in] */ const String& iface)
{
    if (iface == CLAT_INTERFACE_NAME) {
        if (mIsRunning) {
            AutoPtr<INetworkUtils> utils;
            CNetworkUtils::AcquireSingleton((INetworkUtils**)&utils);
            Int32 result;
            utils->ResetConnections(
                CLAT_INTERFACE_NAME,
                INetworkUtils::RESET_IPV4_ADDRESSES, &result);
            Boolean bval;
            mBaseLP->RemoveStackedLink(mLP, &bval);
            UpdateConnectivityService();
        }
        Slogger::I(TAG, "interface %s removed, mIsRunning = %d -> FALSE", CLAT_INTERFACE_NAME.string(), mIsRunning);
        mIsRunning = FALSE;
        mLP->Clear();
        Slogger::I(TAG, "mLP = %s",TO_CSTR(mLP));
    }
    return NOERROR;
}


} // Connectivity
} // Server
} // Droid
} // Elastos
