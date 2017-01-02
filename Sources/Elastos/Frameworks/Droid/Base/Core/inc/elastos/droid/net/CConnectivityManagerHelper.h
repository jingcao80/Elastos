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

#ifndef __ELASTOS_DROID_NET_CCONNECTIVITYMANAGERHELPER_H__
#define __ELASTOS_DROID_NET_CCONNECTIVITYMANAGERHELPER_H__

#include "_Elastos_Droid_Net_CConnectivityManagerHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;

using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Net {

CarClass(CConnectivityManagerHelper)
    , public Singleton
    , public IConnectivityManagerHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Tests if a given integer represents a valid network type.
     * @param networkType the type to be tested
     * @return a boolean.  {@code true} if the type is valid, else {@code false}
     */
    CARAPI IsNetworkTypeValid(
        /* [in] */ Int32 networkType,
        /* [out] */ Boolean* result);

    /**
     * Returns a non-localized string representing a given network type.
     * ONLY used for debugging output.
     * @param type the type needing naming
     * @return a String for the given type, or a string version of the type ("87")
     * if no name is known.
     * {@hide}
     */
    CARAPI GetNetworkTypeName(
        /* [in] */ Int32 networkType,
        /* [out] */ String* result);

    /**
     * Checks if a given type uses the cellular data connection.
     * This should be replaced in the future by a network property.
     * @param networkType the type to check
     * @return a boolean - {@code true} if uses cellular network, else {@code false}
     * {@hide}
     */
    CARAPI  IsNetworkTypeMobile(
        /* [in] */ Int32 networkType,
        /* [out] */ Boolean* result);

    /**
     * Checks if the given network type is backed by a Wi-Fi radio.
     *
     * @hide
     */
    CARAPI IsNetworkTypeWifi(
        /* [in] */ Int32 networkType,
        /* [out] */ Boolean* result);

    /**
     * Removes the NET_CAPABILITY_NOT_RESTRICTED capability from the given
     * NetworkCapabilities object if all the capabilities it provides are
     * typically provided by restricted networks.
     *
     * TODO: consider:
     * - Moving to NetworkCapabilities
     * - Renaming it to guessRestrictedCapability and make it set the
     *   restricted capability bit in addition to clearing it.
     * @hide
     */
    CARAPI MaybeMarkCapabilitiesRestricted(
        /* [in] */ INetworkCapabilities* nc);

    CARAPI  From(
        /* [in] */ IContext* ctx,
        /* [out] */ IConnectivityManager** result);

    /** {@hide */
    CARAPI EnforceTetherChangePermission(
        /* [in] */ IContext* context);

    /**
     * Binds the current process to {@code network}.  All Sockets created in the future
     * (and not explicitly bound via a bound SocketFactory from
     * {@link Network#getSocketFactory() Network.getSocketFactory()}) will be bound to
     * {@code network}.  All host name resolutions will be limited to {@code network} as well.
     * Note that if {@code network} ever disconnects, all Sockets created in this way will cease to
     * work and all host name resolutions will fail.  This is by design so an application doesn't
     * accidentally use Sockets it thinks are still bound to a particular {@link Network}.
     * To clear binding pass {@code null} for {@code network}.  Using individually bound
     * Sockets created by Network.getSocketFactory().createSocket() and
     * performing network-specific host name resolutions via
     * {@link Network#getAllByName Network.getAllByName} is preferred to calling
     * {@code setProcessDefaultNetwork}.
     *
     * @param network The {@link Network} to bind the current process to, or {@code null} to clear
     *                the current binding.
     * @return {@code true} on success, {@code false} if the {@link Network} is no longer valid.
     */
    CARAPI SetProcessDefaultNetwork(
        /* [in] */ INetwork* network,
        /* [out] */ Boolean* result);

    /**
     * Returns the {@link Network} currently bound to this process via
     * {@link #setProcessDefaultNetwork}, or {@code null} if no {@link Network} is explicitly bound.
     *
     * @return {@code Network} to which this process is bound, or {@code null}.
     */
    CARAPI GetProcessDefaultNetwork(
        /* [out] */ INetwork** result);

    /**
     * Binds host resolutions performed by this process to {@code network}.
     * {@link #setProcessDefaultNetwork} takes precedence over this setting.
     *
     * @param network The {@link Network} to bind host resolutions from the current process to, or
     *                {@code null} to clear the current binding.
     * @return {@code true} on success, {@code false} if the {@link Network} is no longer valid.
     * @hide
     * @deprecated This is strictly for legacy usage to support {@link #startUsingNetworkFeature}.
     */
    CARAPI SetProcessDefaultNetworkForHostResolution(
        /* [in] */ INetwork* network,
        /* [out] */ Boolean* result);
};

} // namespace Net
} // namepsace Droid
} // namespace Elastos

#endif // __CCONNECTIVITYMANAGERHELPER_H__
