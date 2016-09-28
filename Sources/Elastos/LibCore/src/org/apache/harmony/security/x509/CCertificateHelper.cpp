
#include "org/apache/harmony/security/x509/CCertificateHelper.h"

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
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CCertificateHelper::SetASN1(
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