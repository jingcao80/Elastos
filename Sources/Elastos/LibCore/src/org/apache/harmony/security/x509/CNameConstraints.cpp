
#include "org/apache/harmony/security/x509/CNameConstraints.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_OBJECT_IMPL(CNameConstraints)

CAR_INTERFACE_IMPL_2(CNameConstraints, Object, INameConstraints, IExtensionValue)

ECode CNameConstraints::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** ppEncode)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNameConstraints::DumpValue(
    /* [in] */ IStringBuilder* pSb,
    /* [in] */ const String& prefix)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNameConstraints::DumpValue(
    /* [in] */ IStringBuilder* pSb)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNameConstraints::IsAcceptable(
    /* [in] */ IX509Certificate* pCert,
    /* [out] */ Boolean* pIsAcceptable)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNameConstraints::IsAcceptableEx(
    /* [in] */ IList* pNames,
    /* [out] */ Boolean* pIsAcceptable)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNameConstraints::constructor(
    /* [in] */ IGeneralSubtrees* pPermittedSubtrees,
    /* [in] */ IGeneralSubtrees* pExcludedSubtrees)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org