
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

