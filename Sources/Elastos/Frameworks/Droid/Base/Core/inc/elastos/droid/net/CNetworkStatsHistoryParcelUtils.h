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

#ifndef __ELASTOS_DROID_NET_CNETWORKSTATSHISTORYPARCELUTILS_H__
#define __ELASTOS_DROID_NET_CNETWORKSTATSHISTORYPARCELUTILS_H__

#include "_Elastos_Droid_Net_CNetworkStatsHistoryParcelUtils.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * Utility methods for interacting with {@link Parcel} structures, mostly
 * dealing with writing partial arrays.
 */
CarClass(CNetworkStatsHistoryParcelUtils)
    , public Singleton
    , public INetworkStatsHistoryParcelUtils
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI ReadLongArray(
        /* [in] */ IParcel* in,
        /* [out, callee] */ ArrayOf<Int64>** result);

    CARAPI WriteLongArray(
        /* [in] */ IParcel* out,
        /* [in] */ ArrayOf<Int64>* values,
        /* [in] */ Int32 size);
};

} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif //  __ELASTOS_DROID_NET_CNETWORKSTATSHISTORYPARCELUTILS_H__
