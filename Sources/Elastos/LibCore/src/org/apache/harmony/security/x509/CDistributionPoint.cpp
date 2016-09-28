
#include "org/apache/harmony/security/x509/CDistributionPoint.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_OBJECT_IMPL(CDistributionPoint)

CAR_INTERFACE_IMPL(CDistributionPoint, Object, IDistributionPoint)

ECode CDistributionPoint::DumpValue(
    /* [in] */ IStringBuilder* pSb,
    /* [in] */ const String& prefix)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CDistributionPoint::constructor(
    /* [in] */ IDistributionPointName* pDistributionPoint,
    /* [in] */ IReasonFlags* pReasons,
    /* [in] */ IGeneralNames* pCRLIssuer)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org