
#include "org/apache/harmony/security/x509/COtherName.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_OBJECT_IMPL(COtherName)

CAR_INTERFACE_IMPL(COtherName, Object, IOtherName)

ECode COtherName::GetValue(
    /* [out, callee] */ ArrayOf<Byte>** ppValue)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COtherName::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** ppEncoded)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COtherName::constructor(
    /* [in] */ const String& typeID,
    /* [in] */ ArrayOf<Byte>* pValue)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org