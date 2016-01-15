
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
