
#include "org/apache/harmony/security/x509/CAccessDescription.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_OBJECT_IMPL(CAccessDescription)

CAR_INTERFACE_IMPL(CAccessDescription, Object, IAccessDescription)

ECode CAccessDescription::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** ppEncoded)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CAccessDescription::ToString(
    /* [out] */ String* pStr)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org