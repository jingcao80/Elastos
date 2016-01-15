
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/net/CNetworkStatsHistory.h"
#include "elastos/droid/net/CNetworkStatsHistoryHelper.h"

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL(CNetworkStatsHistoryHelper, Singleton, INetworkStatsHistoryHelper)

CAR_SINGLETON_IMPL(CNetworkStatsHistoryHelper)

ECode CNetworkStatsHistoryHelper::RandomLong(
    /* [in] */ IRandom * random,
    /* [in] */ Int64 start,
    /* [in] */ Int64 end,
    /* [out] */ Int64 * result)
{
    return CNetworkStatsHistory::RandomLong(random, start, end, result);
}

} // namespace Net
} // namepsace Droid
} // namespace Elastos

