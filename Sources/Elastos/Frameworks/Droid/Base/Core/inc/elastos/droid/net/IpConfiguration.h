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

#ifndef __ELASTOS_DROID_NET_IPCONFIGURATION_H__
#define __ELASTOS_DROID_NET_IPCONFIGURATION_H__

#include "Elastos.Droid.Net.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * A class representing a configured network.
 * @hide
 */
class IpConfiguration
    : public Object
    , public IIpConfiguration
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

public:
    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IpConfigurationIpAssignment ipAssignment,
        /* [in] */ IpConfigurationProxySettings proxySettings,
        /* [in] */ IStaticIpConfiguration* staticIpConfiguration,
        /* [in] */ IProxyInfo* httpProxy);

    CARAPI constructor(
        /* [in] */ IIpConfiguration* source);

    CARAPI GetIpAssignment(
        /* [out] */ IpConfigurationIpAssignment* result);

    CARAPI SetIpAssignment(
        /* [in] */ IpConfigurationIpAssignment ipAssignment);

    CARAPI GetStaticIpConfiguration(
        /* [out] */ IStaticIpConfiguration** result);

    CARAPI SetStaticIpConfiguration(
        /* [in] */ IStaticIpConfiguration* staticIpConfiguration);

    CARAPI GetProxySettings(
        /* [out] */ IpConfigurationProxySettings* result);

    CARAPI SetProxySettings(
        /* [in] */ IpConfigurationProxySettings proxySettings);

    CARAPI GetHttpProxy(
        /* [out] */ IProxyInfo** result);

    CARAPI SetHttpProxy(
        /* [in] */ IProxyInfo* httpProxy);

    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

    // @Override
    CARAPI Equals(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI GetHashCode(
        /* [out] */ Int32* result);

    /** Implement the Parcelable interface */
    CARAPI ReadFromParcel(
        /* [in] */ IParcel* parcel);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

private:
    CARAPI Init(
        /* [in] */ IpConfigurationIpAssignment ipAssignment,
        /* [in] */ IpConfigurationProxySettings proxySettings,
        /* [in] */ IStaticIpConfiguration* staticIpConfiguration,
        /* [in] */ IProxyInfo* httpProxy);

private:
    static const String TAG;

    IpConfigurationIpAssignment mIpAssignment;

    AutoPtr<IStaticIpConfiguration> mStaticIpConfiguration;

    IpConfigurationProxySettings mProxySettings;

    AutoPtr<IProxyInfo> mHttpProxy;
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_IPCONFIGURATION_H__
