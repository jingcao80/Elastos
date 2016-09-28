
#include "org/apache/harmony/security/x509/CCertificateIssuer.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_OBJECT_IMPL(CCertificateIssuer)

CAR_INTERFACE_IMPL_2(CCertificateIssuer, Object, ICertificateIssuer, IExtensionValue)

ECode CCertificateIssuer::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** ppEncode)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CCertificateIssuer::DumpValue(
    /* [in] */ IStringBuilder* pSb,
    /* [in] */ const String& prefix)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CCertificateIssuer::DumpValue(
    /* [in] */ IStringBuilder* pSb)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CCertificateIssuer::GetIssuer(
    /* [out] */ IX500Principal** ppX500Principal)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CCertificateIssuer::constructor(
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