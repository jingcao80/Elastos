
#include "org/apache/harmony/security/x509/CExtensionHelper.h"
#include "org/apache/harmony/security/x509/CExtension.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_SINGLETON_IMPL(CExtensionHelper)

CAR_INTERFACE_IMPL(CExtensionHelper, Singleton, IExtensionHelper)

ECode CExtensionHelper::GetASN1(
    /* [out] */ IASN1Sequence** ppAsn1)
{
    VALIDATE_NOT_NULL(ppAsn1);

    return CExtension::GetASN1(ppAsn1);
}

ECode CExtensionHelper::SetASN1(
    /* [in] */ IASN1Sequence* pAsn1)
{
    return CExtension::SetASN1(pAsn1);
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org