
#include "org/apache/harmony/security/x509/tsp/CTSTInfoHelper.h"

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
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTSTInfoHelper::SetACCURACY(
    /* [in] */ IASN1Sequence* pAccuracy)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTSTInfoHelper::GetASN1(
    /* [out] */ IASN1Sequence** ppAsn1)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTSTInfoHelper::SetASN1(
    /* [in] */ IASN1Sequence* pAsn1)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

} // namespace Tsp
} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org