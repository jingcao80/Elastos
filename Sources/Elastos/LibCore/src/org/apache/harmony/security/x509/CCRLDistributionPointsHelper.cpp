
#include "org/apache/harmony/security/x509/CCRLDistributionPointsHelper.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_SINGLETON_IMPL(CCRLDistributionPointsHelper)

CAR_INTERFACE_IMPL(CCRLDistributionPointsHelper, Singleton, ICRLDistributionPointsHelper)

ECode CCRLDistributionPointsHelper::Decode(
    /* [in] */ ArrayOf<Byte> * pEncoding,
    /* [out] */ ICRLDistributionPoints** ppCrlDistributionPoints)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CCRLDistributionPointsHelper::GetASN1(
    /* [out] */ IASN1Type** ppAsn1)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CCRLDistributionPointsHelper::SetASN1(
    /* [in] */ IASN1Type* pAsn1)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org