
#include "org/apache/harmony/security/x509/CInvalidityDateHelper.h"
#include "org/apache/harmony/security/x509/CInvalidityDate.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_SINGLETON_IMPL(CInvalidityDateHelper)

CAR_INTERFACE_IMPL(CInvalidityDateHelper, Singleton, IInvalidityDateHelper)

ECode CInvalidityDateHelper::GetASN1(
    /* [out] */ IASN1Type** ppAsn1)
{
    VALIDATE_NOT_NULL(ppAsn1);

    return CInvalidityDate::GetASN1(ppAsn1);
}

ECode CInvalidityDateHelper::SetASN1(
    /* [in] */ IASN1Type* pAsn1)
{
    return CInvalidityDate::SetASN1(pAsn1);
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org