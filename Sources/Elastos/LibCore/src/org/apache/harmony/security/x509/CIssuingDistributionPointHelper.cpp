
#include "org/apache/harmony/security/x509/CIssuingDistributionPointHelper.h"

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
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CIssuingDistributionPointHelper::GetASN1(
    /* [out] */ IASN1Type** ppAsn1)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org