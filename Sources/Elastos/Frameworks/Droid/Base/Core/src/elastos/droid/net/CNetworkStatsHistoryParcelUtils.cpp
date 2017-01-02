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

