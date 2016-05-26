
#include "CPrivateKeyInfoHelper.h"
#include <CPrivateKeyInfo.h>

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Pkcs8 {

CAR_SINGLETON_IMPL(CPrivateKeyInfoHelper)

CAR_INTERFACE_IMPL(CPrivateKeyInfoHelper, Singleton, IPrivateKeyInfoHelper)

ECode CPrivateKeyInfoHelper::GetASN1(
    /* [out] */ IASN1Sequence** asn1)
{
    VALIDATE_NOT_NULL(asn1)
    *asn1 = CPrivateKeyInfo::ASN1.Get();
    REFCOUNT_ADD(*asn1)
    return NOERROR;
}

} // namespace Pkcs8
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

