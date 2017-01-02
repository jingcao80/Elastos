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

#ifndef __ELASTOS_DROID_SERVER_CONNECTIVITY_NAT_464_XLAT_H__
#define __ELASTOS_DROID_SERVER_CONNECTIVITY_NAT_464_XLAT_H__

#include <elastos/droid/server/net/BaseNetworkObserver.h>
#include "_Elastos.Droid.Server.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Net::IIConnectivityManager;
using Elastos::Droid::Net::IInterfaceConfiguration;
using Elastos::Droid::Net::ILinkAddress;
using Elastos::Droid::Net::ILinkProperties;
using Elastos::Droid::Net::INetworkAgent;
using Elastos::Droid::Net::IRouteInfo;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IMessenger;
using Elastos::Droid::Os::IINetworkManagementService;
using Elastos::Net::IInet4Address;

using Elastos::Droid::Server::Net::BaseNetworkObserver;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Connectivity {

/**
 * @hide
 *
 * Class to manage a 464xlat CLAT daemon.
 */
class Nat464Xlat
    : public BaseNetworkObserver
{
public:
    Nat464Xlat();

    virtual ~Nat464Xlat();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IINetworkManagementService* nmService,
        /* [in] */ IIConnectivityManager* connService,
        /* [in] */ IHandler* handler);

    /**
     * Determines whether a network requires clat.
     * @param network the NetworkAgentInfo corresponding to the network.
     * @return true if the network requires clat, false otherwise.
     */
    static Boolean RequiresClat(
        /* [in ]*/ INetworkAgentInfo* nai);

    CARAPI IsRunningClat(
        /* [in] */ INetworkAgentInfo* network,
        /* [out] */ Boolean* result);

    /**
     * Starts the clat daemon.
     * @param lp The link properties of the interface to start clatd on.
     */
    CARAPI StartClat(
        /* [in] */ INetworkAgentInfo* network);

    /**
     * Stops the clat daemon.
     */
    CARAPI StopClat();

    // Copies the stacked clat link in oldLp, if any, to the LinkProperties in nai.
    CARAPI FixupLinkProperties(
        /* [in] */ INetworkAgentInfo* nai,
        /* [in] */ ILinkProperties* oldLp);

private:
    void UpdateConnectivityService();

    //@Override
    CARAPI InterfaceAdded(
        /* [in] */ const String& iface);

    //@Override
    CARAPI InterfaceRemoved(
        /* [in] */ const String& iface);

private:
    AutoPtr<IContext> mContext;
    AutoPtr<IINetworkManagementService> mNMService;
    AutoPtr<IIConnectivityManager> mConnService;
    // Whether we started clatd and expect it to be running.
    Boolean mIsStarted;
    // Whether the clatd interface exists (i.e., clatd is running).
    Boolean mIsRunning;
    // The LinkProperties of the clat interface.
    AutoPtr<ILinkProperties> mLP;
    // Current LinkProperties of the network.  Includes mLP as a stacked link when clat is active.
    AutoPtr<ILinkProperties> mBaseLP;
    // ConnectivityService Handler for LinkProperties updates.
    AutoPtr<IHandler> mHandler;
    // Marker to connote which network we're augmenting.
    AutoPtr<IMessenger> mNetworkMessenger;

    // This must match the interface name in clatd.conf.
    static const String CLAT_INTERFACE_NAME;

    static const String TAG;
};

} // Connectivity
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_CONNECTIVITY_NAT_464_XLAT_H__
