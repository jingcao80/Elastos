
#include "org/apache/harmony/security/x509/CReasonFlags.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_OBJECT_IMPL(CReasonFlags)

CAR_INTERFACE_IMPL(CReasonFlags, Object, IReasonFlags)

ECode CReasonFlags::DumpValue(
    /* [in] */ IStringBuilder* pSb,
    /* [in] */ const String& prefix)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CReasonFlags::constructor(
    /* [in] */ ArrayOf<Boolean>* pFlags)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org