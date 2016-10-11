
#include "org/apache/harmony/security/x509/CDistributionPointNameHelper.h"
#include "org/apache/harmony/security/x509/CDistributionPointName.h"

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
    VALIDATE_NOT_NULL(ppAsn1);

    return CDistributionPointName::GetASN1(ppAsn1);
}

ECode CDistributionPointNameHelper::SetASN1(
    /* [in] */ IASN1Choice* pAsn1)
{
    return CDistributionPointName::SetASN1(pAsn1);
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org