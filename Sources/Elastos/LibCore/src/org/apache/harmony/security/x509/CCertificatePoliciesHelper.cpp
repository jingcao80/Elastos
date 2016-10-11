
#include "org/apache/harmony/security/x509/CCertificatePoliciesHelper.h"
#include "org/apache/harmony/security/x509/CCertificatePolicies.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_SINGLETON_IMPL(CCertificatePoliciesHelper)

CAR_INTERFACE_IMPL(CCertificatePoliciesHelper, Singleton, ICertificatePoliciesHelper)

ECode CCertificatePoliciesHelper::Decode(
    /* [in] */ ArrayOf<Byte>* pEncoding,
    /* [out] */ ICertificatePolicies** ppPolicies)
{
    VALIDATE_NOT_NULL(ppPolicies);

    return CCertificatePolicies::Decode(pEncoding, ppPolicies);
}

ECode CCertificatePoliciesHelper::GetASN1(
    /* [out] */ IASN1Type** ppAsn1)
{
    VALIDATE_NOT_NULL(ppAsn1);

    return CCertificatePolicies::GetASN1(ppAsn1);
}

ECode CCertificatePoliciesHelper::SetASN1(
    /* [in] */ IASN1Type* pAsn1)
{
    return CCertificatePolicies::SetASN1(pAsn1);
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org