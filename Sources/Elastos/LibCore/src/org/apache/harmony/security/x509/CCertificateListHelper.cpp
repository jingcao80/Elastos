
#include "org/apache/harmony/security/x509/CCertificateListHelper.h"
#include "org/apache/harmony/security/x509/CCertificateList.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_SINGLETON_IMPL(CCertificateListHelper)

CAR_INTERFACE_IMPL(CCertificateListHelper, Singleton, ICertificateListHelper)

ECode CCertificateListHelper::GetASN1(
    /* [out] */ IASN1Sequence** ppAsn1)
{
    VALIDATE_NOT_NULL(ppAsn1);

    return CCertificateList::GetASN1(ppAsn1);
}

ECode CCertificateListHelper::SetASN1(
    /* [in] */ IASN1Sequence* pAsn1)
{
    return CCertificateList::SetASN1(pAsn1);
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org