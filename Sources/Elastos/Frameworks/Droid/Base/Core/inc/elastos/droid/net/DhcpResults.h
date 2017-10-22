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

#ifndef __ELASTOS_DROID_NET_DHCPRESULTS_H__
#define __ELASTOS_DROID_NET_DHCPRESULTS_H__

#include <Elastos.CoreLibrary.Net.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/net/StaticIpConfiguration.h"
#include <elastos/core/Object.h>

using Elastos::Net::IInetAddress;

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * A simple object for retrieving the results of a DHCP request.
 * Optimized (attempted) for that jni interface
 * TODO - remove when DhcpInfo is deprecated.  Move the remaining api to LinkProperties.
 * @hide
 */
class DhcpResults
    : public StaticIpConfiguration
    , public IDhcpResults
{
public:
    CAR_INTERFACE_DECL()

    CARAPI GetServerAddress(
        /* [out] */ IInetAddress** result);

    CARAPI SetServerAddress(
        /* [in] */ IInetAddress* serverAddress);

    /** Vendor specific information (from RFC 2132). */
    CARAPI GetVendorInfo(
        /* [out] */ String* result);

    CARAPI GetLeaseDuration(
        /* [out] */ Int32* result);

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IStaticIpConfiguration* source);

    /** copy constructor */
    CARAPI constructor(
        /* [in] */ IDhcpResults* source);

    /**
     * Updates the DHCP fields that need to be retained from
     * original DHCP request if the current renewal shows them
     * being empty.
     */
    CARAPI UpdateFromDhcpRequest(
        /* [in] */ IDhcpResults* orig);

    /**
     * Test if this DHCP lease includes vendor hint that network link is
     * metered, and sensitive to heavy data transfers.
     */
    CARAPI HasMeteredHint(
        /* [out] */ Boolean* result);

    CARAPI Clear();

    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

    // @Override
    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result);

    /** Implement the Parcelable interface */
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* in);

    using StaticIpConfiguration::SetIpAddress;

    // Utils for jni population - false on success
    // Not part of the superclass because they're only used by the JNI iterface to the DHCP daemon.
    CARAPI SetIpAddress(
        /* [in] */ const String& addrString,
        /* [in] */ Int32 prefixLength,
        /* [out] */ Boolean* result);

    using StaticIpConfiguration::SetGateway;

    CARAPI SetGateway(
        /* [in] */ const String& addrString,
        /* [out] */ Boolean* result);

    CARAPI AddDns(
        /* [in] */ const String& addrString,
        /* [out] */ Boolean* result);

    CARAPI SetServerAddress(
        /* [in] */ const String& addrString,
        /* [out] */ Boolean* result);

    CARAPI SetLeaseDuration(
        /* [in] */ Int32 duration);

    CARAPI SetVendorInfo(
        /* [in] */ const String& info);

    CARAPI SetDomains(
        /* [in] */ const String& domains);

private:
    static const String TAG;

    AutoPtr<IInetAddress> mServerAddress;

    /** Vendor specific information (from RFC 2132). */
    String mVendorInfo;

    Int32 mLeaseDuration;
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_DHCPRESULTS_H__
