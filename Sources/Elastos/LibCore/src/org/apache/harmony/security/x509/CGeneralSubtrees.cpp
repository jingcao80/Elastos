
#include "org/apache/harmony/security/x509/CGeneralSubtrees.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_OBJECT_IMPL(CGeneralSubtrees)

CAR_INTERFACE_IMPL(CGeneralSubtrees, Object, IGeneralSubtrees)

ECode CGeneralSubtrees::GetSubtrees(
    /* [out] */ IList** ppSubtrees)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CGeneralSubtrees::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** ppEncoded)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CGeneralSubtrees::constructor(
    /* [in] */ IList* pGeneralSubtrees)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org