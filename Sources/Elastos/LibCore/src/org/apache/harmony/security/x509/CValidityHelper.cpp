
#include "CValidityHelper.h"
#include "CValidity.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_SINGLETON_IMPL(CValidityHelper)

CAR_INTERFACE_IMPL(CValidityHelper, Singleton, IValidityHelper)

ECode CValidityHelper::GetASN1(
    /* [out] */ IASN1Sequence** result)
{
    VALIDATE_NOT_NULL(result);
    *result = CValidity::ASN1;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org