
#include "org/apache/harmony/security/x509/CPolicyInformation.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_OBJECT_IMPL(CPolicyInformation)

CAR_INTERFACE_IMPL(CPolicyInformation, Object, IPolicyInformation)

ECode CPolicyInformation::GetPolicyIdentifier(
    /* [out] */ String* pPolicyIdentifier)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CPolicyInformation::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** ppEncoded)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CPolicyInformation::DumpValue(
    /* [in] */ IStringBuilder* pSb)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CPolicyInformation::constructor(
    /* [in] */ const String& policyIdentifier)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org