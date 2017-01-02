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

#ifndef __ELASTOS_DROID_NET_DUMMYDATASTATETRACKER_H__
#define __ELASTOS_DROID_NET_DUMMYDATASTATETRACKER_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/net/NetworkStateTracker.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Net::INetworkInfo;
using Elastos::Droid::Os::IHandler;

namespace Elastos {
namespace Droid {
namespace Net {

class DummyDataStateTracker
    : public ElRefBase
    , public INetworkStateTracker
{
public:
    /**
     * Create a new DummyDataStateTracker
     * @param netType the ConnectivityManager network type
     * @param tag the name of this network
     */
    DummyDataStateTracker(
        /* [in] */ Int32 netType,
        /* [in] */ const String& tag);

    CAR_INTERFACE_DECL()

    /**
    * Begin monitoring data connectivity.
     *
     * @param context is the current Android context
     * @param target is the Handler to which to return the events.
     */
    CARAPI StartMonitoring(
        /* [in] */ IContext* context,
        /* [in] */ IHandler* target);

    CARAPI IsPrivateDnsRouteSet(
        /* [out] */ Boolean* isSet);

    CARAPI SetPrivateDnsRoute(
        /* [in] */ Boolean enabled);

    CARAPI GetNetworkInfo(
        /* [out] */ INetworkInfo** info);

    CARAPI IsDefaultRouteSet(
        /* [out] */ Boolean* isSet);

    CARAPI SetDefaultRoute(
        /* [in] */ Boolean enabled);

    /**
     * This is not implemented.
     */
    CARAPI_(void) ReleaseWakeLock();

    /**
     * Report whether data connectivity is possible.
     */
    CARAPI IsAvailable(
        /* [out] */ Boolean* isAvailable);

    /**
     * Return the system properties name associated with the tcp buffer sizes
     * for this network.
     */
    CARAPI GetTcpBufferSizesPropName(
        /* [out] */ String* propName);

    /**
     * Tear down mobile data connectivity, i.e., disable the ability to create
     * mobile data connections.
     * TODO - make async and return nothing?
     */
    CARAPI Teardown(
        /* [out] */ Boolean* result);

    CARAPI CaptivePortalCheckComplete();

    CARAPI SetTeardownRequested(
        /* [in] */ Boolean isRequested);

    CARAPI IsTeardownRequested(
        /* [out] */ Boolean* isRequested);

    /**
     * Re-enable mobile data connectivity after a {@link #teardown()}.
     * TODO - make async and always get a notification?
     */
    CARAPI Reconnect(
        /* [out] */ Boolean* result);

    /**
     * Turn on or off the mobile radio. No connectivity will be possible while the
     * radio is off. The operation is a no-op if the radio is already in the desired state.
     * @param turnOn {@code true} if the radio should be turned on, {@code false} if
     */
    CARAPI SetRadio(
        /* [in] */ Boolean turnOn,
        /* [out] */ Boolean* result);

    CARAPI SetUserDataEnable(
        /* [in] */ Boolean enabled);

    CARAPI SetPolicyDataEnable(
        /* [in] */ Boolean enabled);

    CARAPI_(String) ToString();

    /**
     * @see android.net.NetworkStateTracker#getLinkProperties()
     */
    CARAPI GetLinkProperties(
        /* [out] */ ILinkProperties** linkProperties);

    /**
     * @see android.net.NetworkStateTracker#getLinkCapabilities()
     */
    CARAPI GetLinkCapabilities(
        /* [out] */ ILinkCapabilities** linkCapabilities);

    CARAPI SetDependencyMet(
        /* [in] */ Boolean met);

private:
    /**
     * Record the detailed state of a network, and if it is a
     * change from the previous state, send a notification to
     * any listeners.
     * @param state the new {@code DetailedState}
     * @param reason a {@code String} indicating a reason for the state change,
     * if one was supplied. May be {@code null}.
     * @param extraInfo optional {@code String} providing extra information about the state change
     */
    CARAPI_(void) SetDetailedState(
        /* [in] */ NetworkInfoDetailedState state,
        /* [in] */ const String& reason,
        /* [in] */ const String& extraInfo);

private:
    static const String TAG;
    static const Boolean DBG;
    static const Boolean VDBG;

    AutoPtr<INetworkInfo> mNetworkInfo;
    Boolean mTeardownRequested;
    AutoPtr<IHandler> mTarget;
    AutoPtr<IContext> mContext;
    AutoPtr<ILinkProperties> mLinkProperties;
    AutoPtr<ILinkCapabilities> mLinkCapabilities;
    Boolean mPrivateDnsRouteSet;
    Boolean mDefaultRouteSet;

    // DEFAULT and HIPRI are the same connection.  If we're one of these we need to check if
    // the other is also disconnected before we reset sockets
    Boolean mIsDefaultOrHipri;
};

} // namespace Net
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_NET_DUMMYDATASTATETRACKER_H__
