
#include "org/apache/harmony/security/x509/CExtendedKeyUsageHelper.h"
#include "org/apache/harmony/security/x509/CExtendedKeyUsage.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_SINGLETON_IMPL(CExtendedKeyUsageHelper)

CAR_INTERFACE_IMPL(CExtendedKeyUsageHelper, Singleton, IExtendedKeyUsageHelper)

ECode CExtendedKeyUsageHelper::GetASN1(
    /* [out] */ IASN1Type** ppAsn1)
{
    VALIDATE_NOT_NULL(ppAsn1);

    return CExtendedKeyUsage::GetASN1(ppAsn1);
}

ECode CExtendedKeyUsageHelper::SetASN1(
    /* [in] */ IASN1Type* pAsn1)
{
    return CExtendedKeyUsage::SetASN1(pAsn1);
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org