
#include "org/apache/harmony/security/x509/CCertificate.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_OBJECT_IMPL(CCertificate)

CAR_INTERFACE_IMPL(CCertificate, Object, ICertificate)

ECode CCertificate::GetTbsCertificate(
    /* [out] */ ITBSCertificate** ppTbsCertificate)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CCertificate::GetSignatureValue(
    /* [out, callee] */ ArrayOf<Byte>** ppSignatureValue)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CCertificate::ToString(
    /* [out] */ String * pStr)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CCertificate::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** ppEncode)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CCertificate::constructor(
    /* [in] */ ITBSCertificate* pTbsCertificate,
    /* [in] */ IAlgorithmIdentifier* pSignatureAlgorithm,
    /* [in] */ ArrayOf<Byte>* pSignatureValue)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org