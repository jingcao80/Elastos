
#include "org/apache/harmony/security/x509/CDistributionPointName.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_OBJECT_IMPL(CDistributionPointName)

CAR_INTERFACE_IMPL(CDistributionPointName, Object, IDistributionPointName)

ECode CDistributionPointName::DumpValue(
    /* [in] */ IStringBuilder* pSb,
    /* [in] */ const String& prefix)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CDistributionPointName::constructor(
    /* [in] */ IGeneralNames* pFullName)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CDistributionPointName::constructor(
    /* [in] */ IName* pNameRelativeToCRLIssuer)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org