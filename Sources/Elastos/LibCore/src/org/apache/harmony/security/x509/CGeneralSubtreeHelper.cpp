
#include "org/apache/harmony/security/x509/CGeneralSubtreeHelper.h"
#include "org/apache/harmony/security/x509/CGeneralSubtree.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_SINGLETON_IMPL(CGeneralSubtreeHelper)

CAR_INTERFACE_IMPL(CGeneralSubtreeHelper, Singleton, IGeneralSubtreeHelper)

ECode CGeneralSubtreeHelper::GetASN1(
    /* [out] */ IASN1Sequence** ppAsn1)
{
    VALIDATE_NOT_NULL(ppAsn1);

    return CGeneralSubtree::GetASN1(ppAsn1);
}

ECode CGeneralSubtreeHelper::SetASN1(
    /* [in] */ IASN1Sequence* pAsn1)
{
    return CGeneralSubtree::SetASN1(pAsn1);
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org