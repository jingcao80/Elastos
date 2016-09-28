
#include "org/apache/harmony/security/x509/CCertificateListHelper.h"

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
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CCertificateListHelper::SetASN1(
    /* [in] */ IASN1Sequence* pAsn1)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org