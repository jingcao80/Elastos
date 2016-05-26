
#include "CCertificationRequestHelper.h"
#include <CCertificationRequest.h>
#include <cmdef.h>

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Pkcs10 {

CAR_SINGLETON_IMPL(CCertificationRequestHelper)

CAR_INTERFACE_IMPL(CCertificationRequestHelper, Singleton, ICertificationRequestHelper)

ECode CCertificationRequestHelper::GetASN1(
    /* [out] */ IASN1Sequence** asn1)
{
    VALIDATE_NOT_NULL(asn1)
    *asn1 = CCertificationRequest::ASN1;
    REFCOUNT_ADD(*asn1)
    return NOERROR;
}

} // namespace Pkcs10
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

