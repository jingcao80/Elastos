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
#include "elastos/droid/net/CNetworkScorerAppManager.h"
#include "elastos/droid/net/NetworkScorerAppManager.h"

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL(CNetworkScorerAppManager, Singleton, INetworkScorerAppManager)

CAR_SINGLETON_IMPL(CNetworkScorerAppManager)

ECode CNetworkScorerAppManager::GetAllValidScorers(
        /* [in] */ IContext* context,
        /* [out] */ ICollection** result)
{
    return NetworkScorerAppManager::GetAllValidScorers(context, result);
}

ECode CNetworkScorerAppManager::GetActiveScorer(
        /* [in] */ IContext* context,
        /* [out] */ INetworkScorerAppData** result)
{
    return NetworkScorerAppManager::GetActiveScorer(context, result);
}

ECode CNetworkScorerAppManager::SetActiveScorer(
        /* [in] */ IContext* context,
        /* [in] */ const String& packageName,
        /* [out] */ Boolean* result)
{
    return NetworkScorerAppManager::SetActiveScorer(context, packageName, result);
}

ECode CNetworkScorerAppManager::IsCallerActiveScorer(
        /* [in] */ IContext* context,
        /* [in] */ Int32 callingUid,
        /* [out] */ Boolean* result)
{
    return NetworkScorerAppManager::IsCallerActiveScorer(context, callingUid, result);
}

ECode CNetworkScorerAppManager::GetScorer(
        /* [in] */ IContext* context,
        /* [in] */ const String& packageName,
        /* [out] */ INetworkScorerAppData** result)
{
    return NetworkScorerAppManager::GetScorer(context, packageName, result);
}

} // namespace Net
} // namespace Droid
} // namespace Elastos

