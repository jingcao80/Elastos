
#include "org/apache/harmony/security/x509/CAuthorityKeyIdentifier.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_OBJECT_IMPL(CAuthorityKeyIdentifier)

CAR_INTERFACE_IMPL_2(CAuthorityKeyIdentifier, Object, IAuthorityKeyIdentifier, IExtensionValue)

ECode CAuthorityKeyIdentifier::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** ppEncode)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CAuthorityKeyIdentifier::DumpValue(
    /* [in] */ IStringBuilder* pSb,
    /* [in] */ const String& prefix)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CAuthorityKeyIdentifier::DumpValue(
    /* [in] */ IStringBuilder* pSb)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CAuthorityKeyIdentifier::GetKeyIdentifier(
    /* [out, callee] */ ArrayOf<Byte>** ppKeyIdentifier)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CAuthorityKeyIdentifier::GetAuthorityCertIssuer(
    /* [out] */ IGeneralNames** ppIssuer)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CAuthorityKeyIdentifier::GetAuthorityCertSerialNumber(
    /* [out] */ IBigInteger** ppNumber)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CAuthorityKeyIdentifier::constructor(
    /* [in] */ ArrayOf<Byte>* pKeyIdentifier,
    /* [in] */ IGeneralNames* pAuthorityCertIssuer,
    /* [in] */ IBigInteger* pAuthorityCertSerialNumber)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org