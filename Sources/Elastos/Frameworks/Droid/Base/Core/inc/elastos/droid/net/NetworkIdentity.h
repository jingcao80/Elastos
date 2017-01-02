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

#ifndef __ELASTOS_DROID_NET_NETWORKIDENTITY_H__
#define __ELASTOS_DROID_NET_NETWORKIDENTITY_H__

#include "Elastos.Droid.Net.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * Network definition that includes strong identity. Analogous to combining
 * {@link NetworkInfo} and an IMSI.
 *
 * @hide
 */
class NetworkIdentity
    : public Object
    , public INetworkIdentity
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ Int32 type,
        /* [in] */ Int32 subType,
        /* [in] */ const String& subscriberId,
        /* [in] */ const String& networkId,
        /* [in] */ Boolean roaming);

    // @Override
    CARAPI GetHashCode(
        /* [out] */ Int32* result);

    // @Override
    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

    CARAPI GetType(
        /* [out] */ Int32* result);

    CARAPI GetSubType(
        /* [out] */ Int32* result);

    CARAPI GetSubscriberId(
        /* [out] */ String* result);

    CARAPI GetNetworkId(
        /* [out] */ String* result);

    CARAPI GetRoaming(
        /* [out] */ Boolean* result);

    /**
     * Scrub given IMSI on production builds.
     */
    static CARAPI ScrubSubscriberId(
        /* [in] */ const String& subscriberId,
        /* [out] */ String* result);

    /**
     * Build a {@link NetworkIdentity} from the given {@link NetworkState},
     * assuming that any mobile networks are using the current IMSI.
     */
    static CARAPI BuildNetworkIdentity(
        /* [in] */ IContext* context,
        /* [in] */ INetworkState* state,
        /* [out] */ INetworkIdentity** result);

private:
    Int32 mType;

    Int32 mSubType;

    String mSubscriberId;

    String mNetworkId;

    Boolean mRoaming;
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_NETWORKIDENTITY_H__
