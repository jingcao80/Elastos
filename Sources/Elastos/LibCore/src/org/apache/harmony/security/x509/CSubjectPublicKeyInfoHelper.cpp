
#include "org/apache/harmony/security/x509/CSubjectPublicKeyInfoHelper.h"
#include "org/apache/harmony/security/x509/CSubjectPublicKeyInfo.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_SINGLETON_IMPL(CSubjectPublicKeyInfoHelper)

CAR_INTERFACE_IMPL(CSubjectPublicKeyInfoHelper, Singleton, ISubjectPublicKeyInfoHelper)

ECode CSubjectPublicKeyInfoHelper::GetASN1(
    /* [out] */ IASN1Sequence** ppAsn1)
{
    VALIDATE_NOT_NULL(ppAsn1);
    *ppAsn1 = CSubjectPublicKeyInfo::ASN1;
    REFCOUNT_ADD(*ppAsn1);
    return NOERROR;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org