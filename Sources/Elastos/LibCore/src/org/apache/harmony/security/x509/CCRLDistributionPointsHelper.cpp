
#include "org/apache/harmony/security/x509/CCRLDistributionPointsHelper.h"
#include "org/apache/harmony/security/x509/CCRLDistributionPoints.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_SINGLETON_IMPL(CCRLDistributionPointsHelper)

CAR_INTERFACE_IMPL(CCRLDistributionPointsHelper, Singleton, ICRLDistributionPointsHelper)

ECode CCRLDistributionPointsHelper::Decode(
    /* [in] */ ArrayOf<Byte>* pEncoding,
    /* [out] */ ICRLDistributionPoints** ppCrlDistributionPoints)
{
    VALIDATE_NOT_NULL(ppCrlDistributionPoints);

    return CCRLDistributionPoints::Decode(pEncoding, ppCrlDistributionPoints);
}

ECode CCRLDistributionPointsHelper::GetASN1(
    /* [out] */ IASN1Type** ppAsn1)
{
    VALIDATE_NOT_NULL(ppAsn1);

    return CCRLDistributionPoints::GetASN1(ppAsn1);
}

ECode CCRLDistributionPointsHelper::SetASN1(
    /* [in] */ IASN1Type* pAsn1)
{
    return CCRLDistributionPoints::SetASN1(pAsn1);
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org