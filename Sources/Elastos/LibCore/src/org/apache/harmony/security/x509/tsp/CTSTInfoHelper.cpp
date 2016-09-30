
#include "org/apache/harmony/security/x509/tsp/CTSTInfoHelper.h"
#include "org/apache/harmony/security/x509/tsp/TSTInfo.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {
namespace Tsp {

CAR_SINGLETON_IMPL(CTSTInfoHelper)

CAR_INTERFACE_IMPL(CTSTInfoHelper, Singleton, ITSTInfoHelper)

ECode CTSTInfoHelper::GetACCURACY(
    /* [out] */ IASN1Sequence** ppAccuracy)
{
    VALIDATE_NOT_NULL(ppAccuracy)

    return TSTInfo::GetACCURACY(ppAccuracy);
}

ECode CTSTInfoHelper::SetACCURACY(
    /* [in] */ IASN1Sequence* pAccuracy)
{
    return TSTInfo::SetACCURACY(pAccuracy);
}

ECode CTSTInfoHelper::GetASN1(
    /* [out] */ IASN1Sequence** ppAsn1)
{
    VALIDATE_NOT_NULL(ppAsn1)

    return TSTInfo::GetASN1(ppAsn1);
}

ECode CTSTInfoHelper::SetASN1(
    /* [in] */ IASN1Sequence* pAsn1)
{
    return TSTInfo::SetASN1(pAsn1);
}

} // namespace Tsp
} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org