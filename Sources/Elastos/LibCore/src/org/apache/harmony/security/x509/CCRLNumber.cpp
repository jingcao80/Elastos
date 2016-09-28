
#include "org/apache/harmony/security/x509/CCRLNumber.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_OBJECT_IMPL(CCRLNumber)

CAR_INTERFACE_IMPL_2(CCRLNumber, Object, ICRLNumber, IExtensionValue)

ECode CCRLNumber::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** ppEncode)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CCRLNumber::DumpValue(
    /* [in] */ IStringBuilder* pSb,
    /* [in] */ const String& prefix)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CCRLNumber::DumpValue(
    /* [in] */ IStringBuilder* pSb)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CCRLNumber::GetNumber(
    /* [out] */ IBigInteger** ppNumber)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CCRLNumber::constructor(
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