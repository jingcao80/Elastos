
#include "org/apache/harmony/security/x509/CTBSCertListHelper.h"
#include "org/apache/harmony/security/x509/CTBSCertList.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_SINGLETON_IMPL(CTBSCertListHelper)

CAR_INTERFACE_IMPL(CTBSCertListHelper, Singleton, ITBSCertListHelper)

ECode CTBSCertListHelper::GetASN1(
    /* [out] */ IASN1Sequence** ppAsn1)
{
    VALIDATE_NOT_NULL(ppAsn1);
    *ppAsn1 = CTBSCertList::ASN1;
    REFCOUNT_ADD(*ppAsn1);
    return NOERROR;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org