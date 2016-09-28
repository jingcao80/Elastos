
#include "org/apache/harmony/security/x509/CCertificateList.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_OBJECT_IMPL(CCertificateList)

CAR_INTERFACE_IMPL(CCertificateList, Object, ICertificateList)

ECode CCertificateList::GetTbsCertList(
    /* [out] */ ITBSCertList** ppTbsCertList)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CCertificateList::GetSignatureValue(
    /* [out, callee] */ ArrayOf<Byte>** ppSignatureValue)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CCertificateList::ToString(
    /* [out] */ String* pStr)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CCertificateList::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** ppEncode)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CCertificateList::constructor(
    /* [in] */ ITBSCertList* pTbsCertList,
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