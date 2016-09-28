
#include "org/apache/harmony/security/x509/CDistributionPointNameHelper.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_SINGLETON_IMPL(CDistributionPointNameHelper)

CAR_INTERFACE_IMPL(CDistributionPointNameHelper, Singleton, IDistributionPointNameHelper)

ECode CDistributionPointNameHelper::GetASN1(
    /* [out] */ IASN1Choice** ppAsn1)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CDistributionPointNameHelper::SetASN1(
    /* [in] */ IASN1Choice* pAsn1)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org