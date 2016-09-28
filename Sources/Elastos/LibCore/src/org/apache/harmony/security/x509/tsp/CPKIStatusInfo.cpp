
#include "org/apache/harmony/security/x509/tsp/CPKIStatusInfo.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {
namespace Tsp {

CAR_OBJECT_IMPL(CPKIStatusInfo)

CAR_INTERFACE_IMPL(CPKIStatusInfo, Object, IPKIStatusInfo)

ECode CPKIStatusInfo::ToString(
    /* [out] */ String* pStr)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CPKIStatusInfo::GetFailInfo(
    /* [out] */ Int32* pFailInfo)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CPKIStatusInfo::GetStatus(
    /* [out] */ PKIStatus* pStatus)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CPKIStatusInfo::GetStatusString(
    /* [out] */ IList** ppList)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CPKIStatusInfo::constructor(
    /* [in] */ Int32 pkiStatus,
    /* [in] */ IList* pStatusString,
    /* [in] */ Int32 failInfo)
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