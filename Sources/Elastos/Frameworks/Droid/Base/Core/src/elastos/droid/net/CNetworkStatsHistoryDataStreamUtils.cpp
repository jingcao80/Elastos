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

