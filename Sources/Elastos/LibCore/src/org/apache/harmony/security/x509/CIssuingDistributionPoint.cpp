
#include "org/apache/harmony/security/x509/CIssuingDistributionPoint.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_OBJECT_IMPL(CIssuingDistributionPoint)

CAR_INTERFACE_IMPL_2(CIssuingDistributionPoint, Object, IIssuingDistributionPoint, IExtensionValue)

ECode CIssuingDistributionPoint::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** ppEncode)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CIssuingDistributionPoint::DumpValue(
    /* [in] */ IStringBuilder* pSb,
    /* [in] */ const String& prefix)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CIssuingDistributionPoint::DumpValue(
    /* [in] */ IStringBuilder* pSb)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CIssuingDistributionPoint::SetOnlyContainsUserCerts(
    /* [in] */ Boolean onlyContainsUserCerts)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CIssuingDistributionPoint::SetOnlyContainsCACerts(
    /* [in] */ Boolean onlyContainsCACerts)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CIssuingDistributionPoint::SetIndirectCRL(
    /* [in] */ Boolean indirectCRL)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CIssuingDistributionPoint::SetOnlyContainsAttributeCerts(
    /* [in] */ Boolean onlyContainsAttributeCerts)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CIssuingDistributionPoint::constructor(
    /* [in] */ IDistributionPointName* pDistributionPoint,
    /* [in] */ IReasonFlags* pOnlySomeReasons)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org