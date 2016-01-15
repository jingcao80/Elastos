
#include "CCertificationRequestInfoHelper.h"
#include <cmdef.h>
#include <CCertificationRequestInfo.h>

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Pkcs10 {


ECode CCertificationRequestInfoHelper::GetASN1(
    /* [out] */ IASN1Sequence** asn1)
{
    VALIDATE_NOT_NULL(asn1)
    *asn1 = CCertificationRequestInfo::ASN1;
    REFCOUNT_ADD(*asn1);
    return NOERROR;
}

} // namespace Pkcs10
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

