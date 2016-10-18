
#include "org/apache/harmony/security/x509/CIssuingDistributionPointHelper.h"
#include "org/apache/harmony/security/x509/CIssuingDistributionPoint.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_SINGLETON_IMPL(CIssuingDistributionPointHelper)

CAR_INTERFACE_IMPL(CIssuingDistributionPointHelper, Singleton, IIssuingDistributionPointHelper)

ECode CIssuingDistributionPointHelper::Decode(
    /* [in] */ ArrayOf<Byte>* pEncoding,
    /* [out] */ IIssuingDistributionPoint** ppObject)
{
    VALIDATE_NOT_NULL(ppObject);

    return CIssuingDistributionPoint::Decode(pEncoding, ppObject);
}

ECode CIssuingDistributionPointHelper::GetASN1(
    /* [out] */ IASN1Type** ppAsn1)
{
    VALIDATE_NOT_NULL(ppAsn1);

    return CIssuingDistributionPoint::GetASN1(ppAsn1);
}

ECode CIssuingDistributionPointHelper::SetASN1(
    /* [in] */ IASN1Type* pAsn1)
{
    return CIssuingDistributionPoint::SetASN1(pAsn1);
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org