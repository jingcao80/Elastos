
#include "CPKIStatusInfo.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {
namespace Tsp {

CAR_OBJECT_IMPL(CPKIStatusInfo)

CAR_INTERFACE_IMPL(CPKIStatusInfo, Object, IPKIStatusInfo)

ECode CPKIStatusInfo::ToString(
    /* [out] */ String * pStr)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CPKIStatusInfo::GetFailInfo(
    /* [out] */ Int32 * pFailInfo)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CPKIStatusInfo::GetStatus(
    /* [out] */ Org::Apache::Harmony::Security::X509::Tsp::PKIStatus * pStatus)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CPKIStatusInfo::GetStatusString(
    /* [out] */ Elastos::Utility::IList ** ppList)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CPKIStatusInfo::constructor(
    /* [in] */ Int32 pkiStatus,
    /* [in] */ Elastos::Utility::IList * pStatusString,
    /* [in] */ Int32 failInfo)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

}
}
}
}
}
}

