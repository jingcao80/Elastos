
#include "org/apache/harmony/security/x509/tsp/CTimeStampRespHelper.h"
#include "org/apache/harmony/security/x509/tsp/TimeStampResp.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {
namespace Tsp {

CAR_SINGLETON_IMPL(CTimeStampRespHelper)

CAR_INTERFACE_IMPL(CTimeStampRespHelper, Singleton, ITimeStampRespHelper)

ECode CTimeStampRespHelper::GetASN1(
    /* [out] */ IASN1Sequence** ppAsn1)
{
    VALIDATE_NOT_NULL(ppAsn1);
    return TimeStampResp::GetASN1(ppAsn1);
}

ECode CTimeStampRespHelper::SetASN1(
    /* [in] */ IASN1Sequence* pAsn1)
{
    return TimeStampResp::SetASN1(pAsn1);
}

} // namespace Tsp
} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org