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

#ifndef __ELASTOS_DROID_NET_NETWORKTEMPLATE_H__
#define __ELASTOS_DROID_NET_NETWORKTEMPLATE_H__

#include "Elastos.Droid.Net.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * Template definition used to generically match {@link NetworkIdentity},
 * usually when collecting statistics.
 *
 * @hide
 */
class NetworkTemplate
    : public Object
    , public IParcelable
    , public INetworkTemplate
{
public:
    CAR_INTERFACE_DECL()

    // @VisibleForTesting
    static CARAPI ForceAllNetworkTypes();

    /**
     * Template to match {@link ConnectivityManager#TYPE_MOBILE} networks with
     * the given IMSI.
     */
    static CARAPI BuildTemplateMobileAll(
        /* [in] */ const String& subscriberId,
        /* [out] */ INetworkTemplate** result);

    /**
     * Template to match {@link ConnectivityManager#TYPE_MOBILE} networks with
     * the given IMSI that roughly meet a "3G" definition, or lower.
     */
    // @Deprecated
    static CARAPI BuildTemplateMobile3gLower(
        /* [in] */ const String& subscriberId,
        /* [out] */ INetworkTemplate** result);

    /**
     * Template to match {@link ConnectivityManager#TYPE_MOBILE} networks with
     * the given IMSI that roughly meet a "4G" definition.
     */
    // @Deprecated
    static CARAPI BuildTemplateMobile4g(
        /* [in] */ const String& subscriberId,
        /* [out] */ INetworkTemplate** result);

    /**
     * Template to match {@link ConnectivityManager#TYPE_MOBILE} networks,
     * regardless of IMSI.
     */
    static CARAPI BuildTemplateMobileWildcard(
        /* [out] */ INetworkTemplate** result);

    /**
     * Template to match all {@link ConnectivityManager#TYPE_WIFI} networks,
     * regardless of SSID.
     */
    static CARAPI BuildTemplateWifiWildcard(
        /* [out] */ INetworkTemplate** result);

    // @Deprecated
    static CARAPI BuildTemplateWifi(
        /* [out] */ INetworkTemplate** result);

    /**
     * Template to match {@link ConnectivityManager#TYPE_WIFI} networks with the
     * given SSID.
     */
    static CARAPI BuildTemplateWifi(
        /* [in] */ const String& networkId,
        /* [out] */ INetworkTemplate** result);

    /**
     * Template to combine all {@link ConnectivityManager#TYPE_ETHERNET} style
     * networks together.
     */
    static CARAPI BuildTemplateEthernet(
        /* [out] */ INetworkTemplate** result);

    NetworkTemplate();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 matchRule,
        /* [in] */ const String& subscriberId,
        /* [in] */ const String& networkId);

    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

    // @Override
    CARAPI GetHashCode(
        /* [out] */ Int32* result);

    // @Override
    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result);

    CARAPI GetMatchRule(
        /* [out] */ Int32* result);

    CARAPI GetSubscriberId(
        /* [out] */ String* result);

    CARAPI GetNetworkId(
        /* [out] */ String* result);

    /**
     * Test if given {@link NetworkIdentity} matches this template.
     */
    CARAPI Matches(
        /* [in] */ INetworkIdentity* ident,
        /* [out] */ Boolean* result);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

private:
    NetworkTemplate(
        /* [in] */ IParcel* in);

    /**
     * Check if mobile network with matching IMSI.
     */
    CARAPI MatchesMobile(
        /* [in] */ INetworkIdentity* ident,
        /* [out] */ Boolean* result);

    /**
     * Check if mobile network classified 3G or lower with matching IMSI.
     */
    CARAPI MatchesMobile3gLower(
        /* [in] */ INetworkIdentity* ident,
        /* [out] */ Boolean* result);

    /**
     * Check if mobile network classified 4G with matching IMSI.
     */
    CARAPI MatchesMobile4g(
        /* [in] */ INetworkIdentity* ident,
        /* [out] */ Boolean* result);

    /**
     * Check if matches Wi-Fi network template.
     */
    CARAPI MatchesWifi(
        /* [in] */ INetworkIdentity* ident,
        /* [out] */ Boolean* result);

    /**
     * Check if matches Ethernet network template.
     */
    CARAPI MatchesEthernet(
        /* [in] */ INetworkIdentity* ident,
        /* [out] */ Boolean* result);

    CARAPI MatchesMobileWildcard(
        /* [in] */ INetworkIdentity* ident,
        /* [out] */ Boolean* result);

    CARAPI MatchesWifiWildcard(
        /* [in] */ INetworkIdentity* ident,
        /* [out] */ Boolean* result);

    static CARAPI GetMatchRuleName(
        /* [in] */ Int32 matchRule,
        /* [out] */ String* result);

    static CARAPI EnsureSubtypeAvailable();

    static CARAPI_(AutoPtr<ArrayOf<Int32> >) GetDataUsageNetworkTypes();

private:
    /**
     * Set of {@link NetworkInfo#getType()} that reflect data usage.
     */
    static AutoPtr<ArrayOf<Int32> > DATA_USAGE_NETWORK_TYPES;

    static Boolean sForceAllNetworkTypes;

    Int32 mMatchRule;

    String mSubscriberId;

    String mNetworkId;
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_NETWORKTEMPLATE_H__
