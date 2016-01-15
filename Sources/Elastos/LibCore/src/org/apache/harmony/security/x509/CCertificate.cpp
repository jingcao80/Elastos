
#include "CCertificate.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {


ECode CCertificate::GetTbsCertificate(
    /* [out] */ Org::Apache::Harmony::Security::X509::ITBSCertificate ** ppTbsCertificate)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CCertificate::GetSignatureValue(
    /* [out, callee] */ ArrayOf<Byte> ** ppSignatureValue)
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
    /* [out, callee] */ ArrayOf<Byte> ** ppEncode)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CCertificate::constructor(
    /* [in] */ Org::Apache::Harmony::Security::X509::ITBSCertificate * pTbsCertificate,
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

