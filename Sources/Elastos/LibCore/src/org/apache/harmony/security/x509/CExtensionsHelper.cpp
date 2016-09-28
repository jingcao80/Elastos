
#include "org/apache/harmony/security/x509/CExtensionsHelper.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_SINGLETON_IMPL(CExtensionsHelper)

CAR_INTERFACE_IMPL(CExtensionsHelper, Singleton, IExtensionsHelper)

ECode CExtensionsHelper::GetASN1(
    /* [out] */ IASN1Type** ppAsn1)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org