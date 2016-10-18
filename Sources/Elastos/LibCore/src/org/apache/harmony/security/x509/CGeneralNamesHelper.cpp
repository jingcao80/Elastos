
#include "org/apache/harmony/security/x509/CGeneralNamesHelper.h"
#include "org/apache/harmony/security/x509/CGeneralNames.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_SINGLETON_IMPL(CGeneralNamesHelper)

CAR_INTERFACE_IMPL(CGeneralNamesHelper, Singleton, IGeneralNamesHelper)

ECode CGeneralNamesHelper::GetASN1(
    /* [out] */ IASN1Type** ppAsn1)
{
    VALIDATE_NOT_NULL(ppAsn1);

    return CGeneralNames::GetASN1(ppAsn1);
}

ECode CGeneralNamesHelper::SetASN1(
    /* [in] */ IASN1Type* pAsn1)
{
    return CGeneralNames::SetASN1(pAsn1);
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org