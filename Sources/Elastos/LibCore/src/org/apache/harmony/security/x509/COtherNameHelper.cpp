
#include "org/apache/harmony/security/x509/COtherNameHelper.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_SINGLETON_IMPL(COtherNameHelper)

CAR_INTERFACE_IMPL(COtherNameHelper, Singleton, IOtherNameHelper)

ECode COtherNameHelper::GetASN1(
    /* [out] */ IASN1Sequence** ppAsn1)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org