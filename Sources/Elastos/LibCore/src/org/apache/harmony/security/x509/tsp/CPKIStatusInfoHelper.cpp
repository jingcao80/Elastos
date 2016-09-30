
#include "org/apache/harmony/security/x509/tsp/CPKIStatusInfoHelper.h"
#include "org/apache/harmony/security/x509/tsp/PKIStatusInfo.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {
namespace Tsp {

CAR_SINGLETON_IMPL(CPKIStatusInfoHelper)

CAR_INTERFACE_IMPL(CPKIStatusInfoHelper, Singleton, IPKIStatusInfoHelper)

ECode CPKIStatusInfoHelper::GetASN1(
    /* [out] */ IASN1Sequence** ppAsn1)
{
    VALIDATE_NOT_NULL(ppAsn1);

    return PKIStatusInfo::GetASN1(ppAsn1);
}

ECode CPKIStatusInfoHelper::SetASN1(
    /* [in] */ IASN1Sequence* pAsn1)
{
    return PKIStatusInfo::SetASN1(pAsn1);
}

} // namespace Tsp
} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org