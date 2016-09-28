
#include "org/apache/harmony/security/x509/CSubjectKeyIdentifier.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_OBJECT_IMPL(CSubjectKeyIdentifier)

CAR_INTERFACE_IMPL_2(CSubjectKeyIdentifier, Object, ISubjectKeyIdentifier, IExtensionValue)

ECode CSubjectKeyIdentifier::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** ppEncode)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSubjectKeyIdentifier::DumpValue(
    /* [in] */ IStringBuilder* pSb,
    /* [in] */ const String& prefix)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSubjectKeyIdentifier::DumpValue(
    /* [in] */ IStringBuilder* pSb)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSubjectKeyIdentifier::GetKeyIdentifier(
    /* [out, callee] */ ArrayOf<Byte>** ppKeyIdentifier)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSubjectKeyIdentifier::constructor(
    /* [in] */ ArrayOf<Byte>* pKeyIdentifier)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org