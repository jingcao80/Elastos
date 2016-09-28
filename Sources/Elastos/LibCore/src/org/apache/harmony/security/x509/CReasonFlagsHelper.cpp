
#include "org/apache/harmony/security/x509/CReasonFlagsHelper.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_SINGLETON_IMPL(CReasonFlagsHelper)

CAR_INTERFACE_IMPL(CReasonFlagsHelper, Singleton, IReasonFlagsHelper)

ECode CReasonFlagsHelper::GetASN1(
    /* [out] */ IASN1BitString** ppAsn1)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org