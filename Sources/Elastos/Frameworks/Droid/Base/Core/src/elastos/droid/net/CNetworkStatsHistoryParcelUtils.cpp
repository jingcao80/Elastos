
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/net/CNetworkStatsHistoryParcelUtils.h"
#include "elastos/droid/net/NetworkStatsHistory.h"

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL(CNetworkStatsHistoryParcelUtils, Singleton, INetworkStatsHistoryParcelUtils)

CAR_SINGLETON_IMPL(CNetworkStatsHistoryParcelUtils)

ECode CNetworkStatsHistoryParcelUtils::ReadLongArray(
        /* [in] */ IParcel* in,
        /* [out, callee] */ ArrayOf<Int64>** result)
{
    return NetworkStatsHistoryParcelUtils::ReadLongArray(in, result);
}

ECode CNetworkStatsHistoryParcelUtils::WriteLongArray(
        /* [in] */ IParcel* out,
        /* [in] */ ArrayOf<Int64>* values,
        /* [in] */ Int32 size)
{
    return NetworkStatsHistoryParcelUtils::WriteLongArray(out, values, size);
}


} // namespace Net
} // namespace Droid
} // namespace Elastos

