
#include "org/apache/harmony/security/x509/CDistributionPointHelper.h"
#include "org/apache/harmony/security/x509/CDistributionPoint.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_SINGLETON_IMPL(CDistributionPointHelper)

CAR_INTERFACE_IMPL(CDistributionPointHelper, Singleton, IDistributionPointHelper)

ECode CDistributionPointHelper::GetASN1(
    /* [out] */ IASN1Sequence** ppAsn1)
{
    VALIDATE_NOT_NULL(ppAsn1);

    return CDistributionPoint::GetASN1(ppAsn1);
}

ECode CDistributionPointHelper::SetASN1(
    /* [in] */ IASN1Sequence* pAsn1)
{
    return CDistributionPoint::SetASN1(pAsn1);
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org