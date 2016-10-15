
#include "org/apache/harmony/security/x509/CTBSCertificateHelper.h"
#include "org/apache/harmony/security/x509/CTBSCertificate.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_SINGLETON_IMPL(CTBSCertificateHelper)

CAR_INTERFACE_IMPL(CTBSCertificateHelper, Singleton, ITBSCertificateHelper)

ECode CTBSCertificateHelper::GetASN1(
    /* [out] */ IASN1Sequence** ppAsn1)
{
    VALIDATE_NOT_NULL(ppAsn1);
    *ppAsn1 = CTBSCertificate::ASN1;
    REFCOUNT_ADD(*ppAsn1);
    return NOERROR;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org