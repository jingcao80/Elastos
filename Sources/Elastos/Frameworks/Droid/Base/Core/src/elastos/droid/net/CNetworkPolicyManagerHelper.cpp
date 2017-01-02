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

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/net/CNetworkPolicyManager.h"
#include "elastos/droid/net/CNetworkPolicyManagerHelper.h"


namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL(CNetworkPolicyManagerHelper, Singleton, INetworkPolicyManagerHelper)

CAR_SINGLETON_IMPL(CNetworkPolicyManagerHelper)

ECode CNetworkPolicyManagerHelper::From(
    /* [in] */ IContext* context,
    /* [out] */ INetworkPolicyManager** result)
{
    return CNetworkPolicyManager::From(context, result);
}

ECode CNetworkPolicyManagerHelper::ComputeLastCycleBoundary(
    /* [in] */ Int64 currentTime,
    /* [in] */ INetworkPolicy* policy,
    /* [out] */ Int64* result)
{
    return CNetworkPolicyManager::ComputeLastCycleBoundary(currentTime, policy, result);
}

ECode CNetworkPolicyManagerHelper::ComputeNextCycleBoundary(
    /* [in] */ Int64 currentTime,
    /* [in] */ INetworkPolicy* policy,
    /* [out] */ Int64* result)
{
    return CNetworkPolicyManager::ComputeNextCycleBoundary(currentTime, policy, result);
}

ECode CNetworkPolicyManagerHelper::SnapToCycleDay(
    /* [in] */ ITime* time,
    /* [in] */ Int32 cycleDay)
{
    return CNetworkPolicyManager::SnapToCycleDay(time, cycleDay);
}

ECode CNetworkPolicyManagerHelper::IsUidValidForPolicy(
    /* [in] */ IContext* context,
    /* [in] */ Int32 uid,
    /* [out] */ Boolean* result)
{
    return CNetworkPolicyManager::IsUidValidForPolicy(context, uid, result);
}

ECode CNetworkPolicyManagerHelper::DumpPolicy(
    /* [in] */ IPrintWriter* fout,
    /* [in] */ Int32 policy)
{
    return CNetworkPolicyManager::DumpPolicy(fout, policy);
}

ECode CNetworkPolicyManagerHelper::DumpRules(
    /* [in] */ IPrintWriter* fout,
    /* [in] */ Int32 rules)
{
    return CNetworkPolicyManager::DumpRules(fout, rules);
}

} // namespace Net
} // namepsace Droid
} // namespace Elastos
