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

#ifndef __ELASTOS_DROID_NET_NETWORKSTATE_H__
#define __ELASTOS_DROID_NET_NETWORKSTATE_H__

#include "Elastos.Droid.Net.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * Snapshot of network state.
 *
 * @hide
 */
class NetworkState
    : public Object
    , public IParcelable
    , public INetworkState
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ INetworkInfo* networkInfo,
        /* [in] */ ILinkProperties* linkProperties,
        /* [in] */ INetworkCapabilities* networkCapabilities);

    CARAPI constructor(
        /* [in] */ INetworkInfo* networkInfo,
        /* [in] */ ILinkProperties* linkProperties,
        /* [in] */ INetworkCapabilities* networkCapabilities,
        /* [in] */ const String& subscriberId,
        /* [in] */ const String& networkId);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* parcel);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI GetNetworkInfo(
        /* [out] */ INetworkInfo** result);

    CARAPI GetLinkProperties(
        /* [out] */ ILinkProperties** result);

    CARAPI GetNetworkCapabilities(
        /* [out] */ INetworkCapabilities** result);

    /** Currently only used by testing. */
    CARAPI GetSubscriberId(
        /* [out] */ String* result);

    CARAPI GetNetworkId(
        /* [out] */ String* result);

private:
    AutoPtr<INetworkInfo> mNetworkInfo;

    AutoPtr<ILinkProperties> mLinkProperties;

    AutoPtr<INetworkCapabilities> mNetworkCapabilities;

    /** Currently only used by testing. */
    String mSubscriberId;

    String mNetworkId;
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_NETWORKSTATE_H__
