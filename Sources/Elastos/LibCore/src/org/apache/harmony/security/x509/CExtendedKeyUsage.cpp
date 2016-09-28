
#include "org/apache/harmony/security/x509/CExtendedKeyUsage.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_OBJECT_IMPL(CExtendedKeyUsage)

CAR_INTERFACE_IMPL_2(CExtendedKeyUsage, Object, IExtendedKeyUsage, IExtensionValue)

ECode CExtendedKeyUsage::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** ppEncode)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExtendedKeyUsage::DumpValue(
    /* [in] */ IStringBuilder* pSb,
    /* [in] */ const String& prefix)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExtendedKeyUsage::DumpValue(
    /* [in] */ IStringBuilder* pSb)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExtendedKeyUsage::GetExtendedKeyUsage(
    /* [out] */ IList** ppExtendedKeyUsage)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExtendedKeyUsage::constructor(
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