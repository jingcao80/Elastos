
#include "org/apache/harmony/security/x509/tsp/CTimeStampReqHelper.h"
#include "org/apache/harmony/security/x509/tsp/TimeStampReq.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {
namespace Tsp {

CAR_SINGLETON_IMPL(CTimeStampReqHelper)

CAR_INTERFACE_IMPL(CTimeStampReqHelper, Singleton, ITimeStampReqHelper)

ECode CTimeStampReqHelper::GetASN1(
    /* [out] */ IASN1Sequence** ppAsn1)
{
    return TimeStampReq::GetASN1(ppAsn1);
}

ECode CTimeStampReqHelper::SetASN1(
    /* [in] */ IASN1Sequence* pAsn1)
{
    return TimeStampReq::SetASN1(pAsn1);
}

} // namespace Tsp
} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org