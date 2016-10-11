
#include "org/apache/harmony/security/x509/CCertificateIssuerHelper.h"
#include "org/apache/harmony/security/x509/CCertificateIssuer.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_SINGLETON_IMPL(CCertificateIssuerHelper)

CAR_INTERFACE_IMPL(CCertificateIssuerHelper, Singleton, ICertificateIssuerHelper)

ECode CCertificateIssuerHelper::GetASN1(
    /* [out] */ IASN1Type** ppAsn1)
{
    VALIDATE_NOT_NULL(ppAsn1);

    return CCertificateIssuer::GetASN1(ppAsn1);
}

ECode CCertificateIssuerHelper::SetASN1(
    /* [in] */ IASN1Type* pAsn1)
{
    return CCertificateIssuer::SetASN1(pAsn1);
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org