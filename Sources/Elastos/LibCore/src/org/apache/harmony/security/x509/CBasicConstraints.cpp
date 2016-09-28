
#include "org/apache/harmony/security/x509/CBasicConstraints.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_OBJECT_IMPL(CBasicConstraints)

CAR_INTERFACE_IMPL_2(CBasicConstraints, Object, IBasicConstraints, IExtensionValue)

ECode CBasicConstraints::GetCa(
    /* [out] */ Boolean* ca)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CBasicConstraints::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** ppEncode)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CBasicConstraints::DumpValue(
    /* [in] */ IStringBuilder* pSb,
    /* [in] */ const String& prefix)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CBasicConstraints::DumpValue(
    /* [in] */ IStringBuilder* pSb)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CBasicConstraints::GetPathLenConstraint(
    /* [out] */ Int32* pPathLenConstraint)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CBasicConstraints::constructor(
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