
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

