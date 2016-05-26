
#include "CCertificateList.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_OBJECT_IMPL(CCertificateList)

CAR_INTERFACE_IMPL(CCertificateList, Object, ICertificateList)

ECode CCertificateList::GetTbsCertList(
    /* [out] */ Org::Apache::Harmony::Security::X509::ITBSCertList ** ppTbsCertList)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CCertificateList::GetSignatureValue(
    /* [out, callee] */ ArrayOf<Byte> ** ppSignatureValue)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CCertificateList::ToString(
    /* [out] */ String * pStr)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CCertificateList::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte> ** ppEncode)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CCertificateList::constructor(
    /* [in] */ Org::Apache::Harmony::Security::X509::ITBSCertList * pTbsCertList,
    /* [in] */ Org::Apache::Harmony::Security::X509::IAlgorithmIdentifier * pSignatureAlgorithm,
    /* [in] */ ArrayOf<Byte> * pSignatureValue)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

}
}
}
}
}

