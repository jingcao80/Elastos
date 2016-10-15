
#include "org/apache/harmony/security/x509/CPrivateKeyUsagePeriodHelper.h"
#include "org/apache/harmony/security/x509/CPrivateKeyUsagePeriod.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_SINGLETON_IMPL(CPrivateKeyUsagePeriodHelper)
CAR_INTERFACE_IMPL(CPrivateKeyUsagePeriodHelper, Singleton, IPrivateKeyUsagePeriodHelper)

ECode CPrivateKeyUsagePeriodHelper::GetASN1(
    /* [out] */ IASN1Sequence** ppAsn1)
{
    VALIDATE_NOT_NULL(ppAsn1);
    *ppAsn1 = CPrivateKeyUsagePeriod::ASN1;
    REFCOUNT_ADD(*ppAsn1);
    return NOERROR;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org