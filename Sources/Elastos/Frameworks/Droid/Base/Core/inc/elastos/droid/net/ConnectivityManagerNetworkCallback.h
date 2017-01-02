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

#ifndef __ELASTOS_DROID_NET_CONNECTIVITYMANAGERNETWORKCALLBACK_H__
#define __ELASTOS_DROID_NET_CONNECTIVITYMANAGERNETWORKCALLBACK_H__

#include "Elastos.Droid.Net.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * Base class for NetworkRequest callbacks.  Used for notifications about network
 * changes.  Should be extended by applications wanting notifications.
 */
class ECO_PUBLIC ConnectivityManagerNetworkCallback
    : public Object
    , public IConnectivityManagerNetworkCallback
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

    /**
     * @hide
     * Called whenever the framework connects to a network that it may use to
     * satisfy this request
     */
    CARAPI OnPreCheck(
        /* [in] */ INetwork* network);

    /**
     * Called when the framework connects and has declared new network ready for use.
     * This callback may be called more than once if the {@link Network} that is
     * satisfying the request changes.
     *
     * @param network The {@link Network} of the satisfying network.
     */
    CARAPI OnAvailable(
        /* [in] */ INetwork* network);

    /**
     * Called when the network is about to be disconnected.  Often paired with an
     * {@link NetworkCallback#onAvailable} call with the new replacement network
     * for graceful handover.  This may not be called if we have a hard loss
     * (loss without warning).  This may be followed by either a
     * {@link NetworkCallback#onLost} call or a
     * {@link NetworkCallback#onAvailable} call for this network depending
     * on whether we lose or regain it.
     *
     * @param network The {@link Network} that is about to be disconnected.
     * @param maxMsToLive The time in ms the framework will attempt to keep the
     *                     network connected.  Note that the network may suffer a
     *                     hard loss at any time.
     */
    CARAPI OnLosing(
        /* [in] */ INetwork* network,
        /* [in] */ Int32 maxMsToLive);

    /**
     * Called when the framework has a hard loss of the network or when the
     * graceful failure ends.
     *
     * @param network The {@link Network} lost.
     */
    CARAPI OnLost(
        /* [in] */ INetwork* network);

    /**
     * Called if no network is found in the given timeout time.  If no timeout is given,
     * this will not be called.
     * @hide
     */
    CARAPI OnUnavailable();

    /**
     * Called when the network the framework connected to for this request
     * changes capabilities but still satisfies the stated need.
     *
     * @param network The {@link Network} whose capabilities have changed.
     * @param networkCapabilities The new {@link NetworkCapabilities} for this network.
     */
    CARAPI OnCapabilitiesChanged(
        /* [in] */ INetwork* network,
        /* [in] */ INetworkCapabilities* networkCapabilities);

    /**
     * Called when the network the framework connected to for this request
     * changes {@link LinkProperties}.
     *
     * @param network The {@link Network} whose link properties have changed.
     * @param linkProperties The new {@link LinkProperties} for this network.
     */
    CARAPI OnLinkPropertiesChanged(
        /* [in] */ INetwork* network,
        /* [in] */ ILinkProperties* linkProperties);

private:
    AutoPtr<INetworkRequest> mNetworkRequest;

    #ifdef DROID_CORE
        friend class CConnectivityManager;
    #endif
};

} // namespace Net
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_CONNECTIVITYMANAGERNETWORKCALLBACK_H__
