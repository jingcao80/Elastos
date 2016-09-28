
#include "org/apache/harmony/security/x509/tsp/CTimeStampResp.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {
namespace Tsp {

CAR_OBJECT_IMPL(CTimeStampResp)

CAR_INTERFACE_IMPL(CTimeStampResp, Object, ITimeStampResp)

ECode CTimeStampResp::ToString(
    /* [out] */ String* pStr)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTimeStampResp::GetStatus(
    /* [out] */ IPKIStatusInfo** ppStatus)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTimeStampResp::GetTimeStampToken(
    /* [out] */ IContentInfo** ppTimeStampToken)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTimeStampResp::constructor(
    /* [in] */ IPKIStatusInfo* pStatus,
    /* [in] */ IContentInfo* pTimeStampToken)
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