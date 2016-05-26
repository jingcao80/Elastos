
#include "CDistributionPointName.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_OBJECT_IMPL(CDistributionPointName)

CAR_INTERFACE_IMPL(CDistributionPointName, Object, IDistributionPointName)

ECode CDistributionPointName::DumpValue(
    /* [in] */ Elastos::Core::IStringBuilder * pSb,
    /* [in] */ const String& prefix)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CDistributionPointName::constructor(
    /* [in] */ Org::Apache::Harmony::Security::X509::IGeneralNames * pFullName)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CDistributionPointName::constructor(
    /* [in] */ Org::Apache::Harmony::Security::X501::IName * pNameRelativeToCRLIssuer)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

}
}
}
}
}

