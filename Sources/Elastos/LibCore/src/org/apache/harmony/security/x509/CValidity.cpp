
#include "org/apache/harmony/security/x509/CValidity.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_OBJECT_IMPL(CValidity)

CAR_INTERFACE_IMPL(CValidity, Object, IValidity)

ECode CValidity::GetNotBefore(
    /* [out] */ IDate** ppDate)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CValidity::GetNotAfter(
    /* [out] */ IDate** ppDate)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CValidity::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** ppEncoded)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CValidity::constructor(
    /* [in] */ IDate* pNotBefore,
    /* [in] */ IDate* pNotAfter)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org