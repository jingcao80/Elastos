
#include "org/apache/harmony/security/x509/CCertificatePoliciesHelper.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_SINGLETON_IMPL(CCertificatePoliciesHelper)

CAR_INTERFACE_IMPL(CCertificatePoliciesHelper, Singleton, ICertificatePoliciesHelper)

ECode CCertificatePoliciesHelper::Decode(
    /* [in] */ ArrayOf<Byte> * pEncoding,
    /* [out] */ ICertificatePolicies** ppPolicies)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CCertificatePoliciesHelper::GetASN1(
    /* [out] */ IASN1Type** ppAsn1)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CCertificatePoliciesHelper::SetASN1(
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