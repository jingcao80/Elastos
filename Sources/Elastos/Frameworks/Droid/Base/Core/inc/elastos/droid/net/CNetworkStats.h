
#ifndef __ELASTOS_DROID_NET_CNETWORKSTATS_H__
#define __ELASTOS_DROID_NET_CNETWORKSTATS_H__

#include "_Elastos_Droid_Net_CNetworkStats.h"
#include "elastos/droid/net/NetworkStats.h"

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * Collection of active network statistics. Can contain summary details across
 * all interfaces, or details with per-UID granularity. Internally stores data
 * as a large table, closely matching {@code /proc/} data format. This structure
 * optimizes for rapid in-memory comparison, but consider using
 * {@link NetworkStatsHistory} when persisting.
 *
 * @hide
 */
CarClass(CNetworkStats)
    , public NetworkStats
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_CNETWORKSTATS_H__
