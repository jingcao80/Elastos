
#include "org/apache/harmony/security/x509/CBasicConstraintsHelper.h"
#include "org/apache/harmony/security/x509/CBasicConstraints.h"

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
    VALIDATE_NOT_NULL(ppAsn1);

    return CBasicConstraints::GetASN1(ppAsn1);
}

ECode CBasicConstraintsHelper::SetASN1(
    /* [in] */ IASN1Type* pAsn1)
{
    return CBasicConstraints::SetASN1(pAsn1);
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org