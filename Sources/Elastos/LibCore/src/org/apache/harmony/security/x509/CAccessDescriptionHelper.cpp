
#include "org/apache/harmony/security/x509/CAccessDescriptionHelper.h"
#include "org/apache/harmony/security/x509/CAccessDescription.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_SINGLETON_IMPL(CAccessDescriptionHelper)

CAR_INTERFACE_IMPL(CAccessDescriptionHelper, Singleton, IAccessDescriptionHelper)

ECode CAccessDescriptionHelper::GetASN1(
    /* [out] */ IASN1Sequence** ppAsn1)
{
    VALIDATE_NOT_NULL(ppAsn1);

    return CAccessDescription::GetASN1(ppAsn1);
}

ECode CAccessDescriptionHelper::SetASN1(
    /* [in] */ IASN1Sequence* pAsn1)
{
    return CAccessDescription::SetASN1(pAsn1);
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org