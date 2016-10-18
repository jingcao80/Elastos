
#include "org/apache/harmony/security/x509/COtherNameHelper.h"
#include "org/apache/harmony/security/x509/COtherName.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_SINGLETON_IMPL(COtherNameHelper)

CAR_INTERFACE_IMPL(COtherNameHelper, Singleton, IOtherNameHelper)

ECode COtherNameHelper::GetASN1(
    /* [out] */ IASN1Sequence** ppAsn1)
{
    VALIDATE_NOT_NULL(ppAsn1);
    return COtherName::GetASN1(ppAsn1);
}

ECode COtherNameHelper::SetASN1(
    /* [in] */ IASN1Sequence* ppAsn1)
{
    return COtherName::SetASN1(ppAsn1);
}


} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org