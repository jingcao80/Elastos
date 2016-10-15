
#include "org/apache/harmony/security/x509/CReasonCodeHelper.h"
#include "org/apache/harmony/security/x509/CReasonCode.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_SINGLETON_IMPL(CReasonCodeHelper)
CAR_INTERFACE_IMPL(CReasonCodeHelper, Singleton, IReasonCodeHelper)

ECode CReasonCodeHelper::GetASN1(
    /* [out] */ IASN1Type** ppAsn1)
{
    VALIDATE_NOT_NULL(ppAsn1);
    *ppAsn1 = CReasonCode::ASN1;
    REFCOUNT_ADD(*ppAsn1);
    return NOERROR;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org