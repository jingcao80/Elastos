
#include "org/apache/harmony/security/x509/CCertificateHelper.h"
#include "org/apache/harmony/security/x509/CCertificate.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_SINGLETON_IMPL(CCertificateHelper)

CAR_INTERFACE_IMPL(CCertificateHelper, Singleton, ICertificateHelper)

ECode CCertificateHelper::GetASN1(
    /* [out] */ IASN1Sequence** ppAsn1)
{
    VALIDATE_NOT_NULL(ppAsn1);

    return CCertificate::GetASN1(ppAsn1);
}

ECode CCertificateHelper::SetASN1(
    /* [in] */ IASN1Sequence* pAsn1)
{
    return CCertificate::SetASN1(pAsn1);
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org