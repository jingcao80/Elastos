
#include "org/apache/harmony/security/x509/CInvalidityDate.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_OBJECT_IMPL(CInvalidityDate)

CAR_INTERFACE_IMPL_2(CInvalidityDate, Object, IInvalidityDate, IExtensionValue)

ECode CInvalidityDate::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** ppEncode)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CInvalidityDate::DumpValue(
    /* [in] */ IStringBuilder* pSb,
    /* [in] */ const String& prefix)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CInvalidityDate::DumpValue(
    /* [in] */ IStringBuilder* pSb)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CInvalidityDate::GetDate(
    /* [out] */ IDate** ppDate)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CInvalidityDate::constructor(
    /* [in] */ ArrayOf<Byte>* pEncoding)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org