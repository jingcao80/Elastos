
#include "CDistributionPoint.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_OBJECT_IMPL(CDistributionPoint)

CAR_INTERFACE_IMPL(CDistributionPoint, Object, IDistributionPoint)

ECode CDistributionPoint::DumpValue(
    /* [in] */ Elastos::Core::IStringBuilder * pSb,
    /* [in] */ const String& prefix)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CDistributionPoint::constructor(
    /* [in] */ Org::Apache::Harmony::Security::X509::IDistributionPointName * pDistributionPoint,
    /* [in] */ Org::Apache::Harmony::Security::X509::IReasonFlags * pReasons,
    /* [in] */ Org::Apache::Harmony::Security::X509::IGeneralNames * pCRLIssuer)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

}
}
}
}
}

