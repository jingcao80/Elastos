
#include "org/apache/harmony/security/x509/CPolicyConstraints.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_OBJECT_IMPL(CPolicyConstraints)

CAR_INTERFACE_IMPL_2(CPolicyConstraints, Object, IPolicyConstraints, IExtensionValue)

ECode CPolicyConstraints::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** ppEncode)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CPolicyConstraints::DumpValue(
    /* [in] */ IStringBuilder* pSb,
    /* [in] */ const String& prefix)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CPolicyConstraints::DumpValue(
    /* [in] */ IStringBuilder* pSb)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CPolicyConstraints::constructor(
    /* [in] */ IBigInteger* pRequireExplicitPolicy,
    /* [in] */ IBigInteger* pInhibitPolicyMapping)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CPolicyConstraints::constructor(
    /* [in] */ ArrayOf<Byte>* pEncoding)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org