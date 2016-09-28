
#include "org/apache/harmony/security/x509/CPrivateKeyUsagePeriod.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_OBJECT_IMPL(CPrivateKeyUsagePeriod)

CAR_INTERFACE_IMPL(CPrivateKeyUsagePeriod, Object, IPrivateKeyUsagePeriod)

ECode CPrivateKeyUsagePeriod::GetNotBefore(
    /* [out] */ IDate** ppNotBefore)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CPrivateKeyUsagePeriod::GetNotAfter(
    /* [out] */ IDate** ppNotAfter)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CPrivateKeyUsagePeriod::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** ppEncoded)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CPrivateKeyUsagePeriod::constructor(
    /* [in] */ IDate* pNotBeforeDate,
    /* [in] */ IDate* pNotAfterDate)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org