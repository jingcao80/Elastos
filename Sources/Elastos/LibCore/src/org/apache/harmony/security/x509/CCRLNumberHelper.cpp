
#include "org/apache/harmony/security/x509/CCRLNumberHelper.h"
#include "org/apache/harmony/security/x509/CCRLNumber.h"

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
    VALIDATE_NOT_NULL(ppAsn1);

    return CCRLNumber::GetASN1(ppAsn1);
}

ECode CCRLNumberHelper::SetASN1(
    /* [in] */ IASN1Type* pAsn1)
{
    return CCRLNumber::SetASN1(pAsn1);
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org