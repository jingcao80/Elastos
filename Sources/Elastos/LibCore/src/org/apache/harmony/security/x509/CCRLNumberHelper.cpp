
#include "org/apache/harmony/security/x509/CCRLNumberHelper.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_SINGLETON_IMPL(CCRLNumberHelper)

CAR_INTERFACE_IMPL(CCRLNumberHelper, Singleton, ICRLNumberHelper)

ECode CCRLNumberHelper::GetASN1(
    /* [out] */ IASN1Type** ppAsn1)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CCRLNumberHelper::SetASN1(
    /* [in] */ IASN1Type* pAsn1)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org