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

#ifndef __ELASTOS_DROID_NET_WIFIKEY_H__
#define __ELASTOS_DROID_NET_WIFIKEY_H__

#include "Elastos.Droid.Net.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Utility::Regex::IPattern;

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * Information identifying a Wi-Fi network.
 * @see NetworkKey
 *
 * @hide
 */
// @SystemApi
class WifiKey
    : public Object
    , public IParcelable
    , public IWifiKey
{
public:
    CAR_INTERFACE_DECL()

    WifiKey();

    CARAPI constructor();
    /**
     * Construct a new {@link WifiKey} for the given Wi-Fi SSID/BSSID pair.
     *
     * @param ssid the service set identifier (SSID) of an 802.11 network. If the SSID can be
     *         decoded as UTF-8, it should be surrounded by double quotation marks. Otherwise,
     *         it should be a string of hex digits starting with 0x.
     * @param bssid the basic service set identifier (BSSID) of this network's access point.
     *         This should be in the form of a six-byte MAC address: {@code XX:XX:XX:XX:XX:XX},
     *         where each X is a hexadecimal digit.
     * @throws IllegalArgumentException if either the SSID or BSSID is invalid.
     */
    CARAPI constructor(
        /* [in] */ const String& ssid,
        /* [in] */ const String& bssid);

    // @Override
    CARAPI Equals(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI GetHashCode(
        /* [out] */ Int32* result);

    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* parcel);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    /**
     * The service set identifier (SSID) of an 802.11 network. If the SSID can be decoded as
     * UTF-8, it will be surrounded by double quotation marks. Otherwise, it will be a string of
     * hex digits starting with 0x.
     */
    CARAPI GetSsid(
        /* [out] */ String* result);

    /**
     * The basic service set identifier (BSSID) of an access point for this network. This will
     * be in the form of a six-byte MAC address: {@code XX:XX:XX:XX:XX:XX}, where each X is a
     * hexadecimal digit.
     */
    CARAPI GetBssid(
        /* [out] */ String* result);

private:
    static CARAPI_(AutoPtr<IPattern>) InitPattern(
        /* [in] */ const String& pattern);

private:
    // Patterns used for validation.
    static const AutoPtr<IPattern> SSID_PATTERN;
    static const AutoPtr<IPattern> BSSID_PATTERN;

    /**
     * The service set identifier (SSID) of an 802.11 network. If the SSID can be decoded as
     * UTF-8, it will be surrounded by double quotation marks. Otherwise, it will be a string of
     * hex digits starting with 0x.
     */
    /* const */ String mSsid;

    /**
     * The basic service set identifier (BSSID) of an access point for this network. This will
     * be in the form of a six-byte MAC address: {@code XX:XX:XX:XX:XX:XX}, where each X is a
     * hexadecimal digit.
     */
    /* const */ String mBssid;
};

} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_WIFIKEY_H__
