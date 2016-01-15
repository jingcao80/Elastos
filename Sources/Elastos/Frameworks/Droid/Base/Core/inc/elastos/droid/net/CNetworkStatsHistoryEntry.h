
#ifndef __ELASTOS_DROID_NET_CNETWORKSTATSHISTORYENTRY_H__
#define __ELASTOS_DROID_NET_CNETWORKSTATSHISTORYENTRY_H__

#include "_Elastos_Droid_Net_CNetworkStatsHistoryEntry.h"
#include "elastos/droid/net/NetworkStatsHistory.h"

namespace Elastos {
namespace Droid {
namespace Net {

CarClass(CNetworkStatsHistoryEntry)
    , public NetworkStatsHistoryEntry
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_CNETWORKSTATSHISTORYENTRY_H__
