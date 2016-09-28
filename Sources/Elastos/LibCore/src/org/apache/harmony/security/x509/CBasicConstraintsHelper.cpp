
#include "org/apache/harmony/security/x509/CBasicConstraintsHelper.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_SINGLETON_IMPL(CBasicConstraintsHelper)

CAR_INTERFACE_IMPL(CBasicConstraintsHelper, Singleton, IBasicConstraintsHelper)

ECode CBasicConstraintsHelper::GetASN1(
    /* [out] */ IASN1Type** ppAsn1)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CBasicConstraintsHelper::SetASN1(
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