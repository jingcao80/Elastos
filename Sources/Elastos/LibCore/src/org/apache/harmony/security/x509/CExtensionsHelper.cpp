
#include "org/apache/harmony/security/x509/CExtensionsHelper.h"
#include "org/apache/harmony/security/x509/CExtensions.h"

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
    VALIDATE_NOT_NULL(ppAsn1);

    return CExtensions::GetASN1(ppAsn1);
}

ECode CExtensionsHelper::SetASN1(
    /* [in] */ IASN1Type* asn1)
{
    return CExtensions::SetASN1(asn1);
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org