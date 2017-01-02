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
#include "elastos/droid/net/CNetworkStats.h"
#include "elastos/droid/net/CNetworkStatsHelper.h"

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL(CNetworkStatsHelper, Singleton, INetworkStatsHelper)

CAR_SINGLETON_IMPL(CNetworkStatsHelper)

ECode CNetworkStatsHelper::GetIFACE_ALL(
        /* [out] */ String* result)
{
    return CNetworkStats::GetIFACE_ALL(result);
}

ECode CNetworkStatsHelper::SetToString(
    /* [in] */ Int32 set,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    return CNetworkStats::SetToString(set, result);
}

ECode CNetworkStatsHelper::TagToString(
    /* [in] */ Int32 tag,
    /* [out] */ String* result)
{
    return CNetworkStats::TagToString(tag, result);
}

ECode CNetworkStatsHelper::Subtract(
    /* [in] */ INetworkStats* left,
    /* [in] */ INetworkStats* right,
    /* [in] */ INetworkStatsNonMonotonicObserver* observer,
    /* [in] */ IInterface* cookie,
    /* [out] */ INetworkStats** result)
{
    return CNetworkStats::Subtract(left, right, observer, cookie, result);
}

ECode CNetworkStatsHelper::Subtract(
    /* [in] */ INetworkStats* left,
    /* [in] */ INetworkStats* right,
    /* [in] */ INetworkStatsNonMonotonicObserver* observer,
    /* [in] */ IInterface* cookie,
    /* [in] */ INetworkStats* recycle,
    /* [out] */ INetworkStats** result)
{
    return CNetworkStats::Subtract(left, right, observer, cookie, recycle, result);
}

} // namespace Net
} // namespace Droid
} // namespace Elastos

