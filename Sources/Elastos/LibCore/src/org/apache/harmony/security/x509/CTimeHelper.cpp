
#include "org/apache/harmony/security/x509/CTimeHelper.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_SINGLETON_IMPL(CTimeHelper)

CAR_INTERFACE_IMPL(CTimeHelper, Singleton, ITimeHelper)

ECode CTimeHelper::GetASN1(
    /* [out] */ IASN1Choice** ppAsn1)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org