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

#ifndef __ELASTOS_DROID_NET_CNETWORKSTATSHISTORYHELPER_H__
#define __ELASTOS_DROID_NET_CNETWORKSTATSHISTORYHELPER_H__

#include "_Elastos_Droid_Net_CNetworkStatsHistoryHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Utility::IRandom;

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * Collection of historical network statistics, recorded into equally-sized
 * "buckets" in time. Internally it stores data in {@code long} series for more
 * efficient persistence.
 * <p>
 * Each bucket is defined by a {@link #bucketStart} timestamp, and lasts for
 * {@link #bucketDuration}. Internally assumes that {@link #bucketStart} is
 * sorted at all times.
 *
 * @hide
 */
CarClass(CNetworkStatsHistoryHelper)
    , public Singleton
    , public INetworkStatsHistoryHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI RandomLong(
        /* [in] */ IRandom * pR,
        /* [in] */ Int64 start,
        /* [in] */ Int64 end,
        /* [out] */ Int64 * result);
};

} // namespace Net
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_CNETWORKSTATSHISTORYHELPER_H__
