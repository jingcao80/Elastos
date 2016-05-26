
#include "CSignedDataHelper.h"
#include "CSignedData.h"
#include <cmdef.h>

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Pkcs7 {

CAR_SINGLETON_IMPL(CSignedDataHelper)

CAR_INTERFACE_IMPL(CSignedDataHelper, Singleton, ISignedDataHelper)

ECode CSignedDataHelper::GetASN1(
    /* [out] */ IASN1Sequence** asn1)
{
    VALIDATE_NOT_NULL(asn1)
    *asn1 = CSignedData::ASN1;
    REFCOUNT_ADD(*asn1)
    return NOERROR;
}

} // namespace Pkcs7
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

