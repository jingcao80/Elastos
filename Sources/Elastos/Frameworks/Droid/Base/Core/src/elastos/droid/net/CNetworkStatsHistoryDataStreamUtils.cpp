
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/net/CNetworkStatsHistoryDataStreamUtils.h"
#include "elastos/droid/net/NetworkStatsHistory.h"

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL(CNetworkStatsHistoryDataStreamUtils, Singleton, INetworkStatsHistoryDataStreamUtils)

CAR_SINGLETON_IMPL(CNetworkStatsHistoryDataStreamUtils)

ECode CNetworkStatsHistoryDataStreamUtils::ReadFullLongArray(
    /* [in] */ IDataInputStream* in,
    /* [out, callee] */ ArrayOf<Int64>** result)
{
    return NetworkStatsHistoryDataStreamUtils::ReadFullLongArray(in, result);
}

/**
 * Read variable-length {@link Long} using protobuf-style approach.
 */
ECode CNetworkStatsHistoryDataStreamUtils::ReadVarLong(
    /* [in] */ IDataInputStream* in,
    /* [out] */ Int64* result)
{
    return NetworkStatsHistoryDataStreamUtils::ReadVarLong(in, result);
}

/**
 * Write variable-length {@link Long} using protobuf-style approach.
 */
ECode CNetworkStatsHistoryDataStreamUtils::WriteVarLong(
    /* [in] */ IDataOutputStream* out,
    /* [in] */ Int64 value)
{
    return NetworkStatsHistoryDataStreamUtils::WriteVarLong(out, value);
}

ECode CNetworkStatsHistoryDataStreamUtils::ReadVarLongArray(
    /* [in] */ IDataInputStream* in,
    /* [out, callee] */ ArrayOf<Int64>** result)
{
    return NetworkStatsHistoryDataStreamUtils::ReadVarLongArray(in, result);
}

ECode CNetworkStatsHistoryDataStreamUtils::WriteVarLongArray(
    /* [in] */ IDataOutputStream* out,
    /* [in] */ ArrayOf<Int64>* values,
    /* [in] */ Int32 size)
{
    return NetworkStatsHistoryDataStreamUtils::WriteVarLongArray(out, values, size);
}

} // namespace Net
} // namespace Droid
} // namespace Elastos

