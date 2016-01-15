
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
