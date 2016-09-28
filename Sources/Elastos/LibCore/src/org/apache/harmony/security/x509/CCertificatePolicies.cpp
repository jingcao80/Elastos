
#include "org/apache/harmony/security/x509/CCertificatePolicies.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_OBJECT_IMPL(CCertificatePolicies)

CAR_INTERFACE_IMPL_2(CCertificatePolicies, Object, ICertificatePolicies, IExtensionValue)

ECode CCertificatePolicies::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** ppEncode)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CCertificatePolicies::DumpValue(
    /* [in] */ IStringBuilder* pSb,
    /* [in] */ const String& prefix)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CCertificatePolicies::DumpValue(
    /* [in] */ IStringBuilder* pSb)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CCertificatePolicies::GetPolicyInformations(
    /* [out] */ IList** ppInfomations)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CCertificatePolicies::AddPolicyInformation(
    /* [in] */ IPolicyInformation* pPolicyInformation,
    /* [out] */ ICertificatePolicies** ppPolicies)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CCertificatePolicies::constructor()
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org