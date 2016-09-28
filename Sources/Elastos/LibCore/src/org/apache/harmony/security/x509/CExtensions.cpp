
#include "org/apache/harmony/security/x509/CExtensions.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_OBJECT_IMPL(CExtensions)

CAR_INTERFACE_IMPL(CExtensions, Object, IExtensions)

ECode CExtensions::GetSize(
    /* [out] */ Int32* pSize)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExtensions::GetCriticalExtensions(
    /* [out] */ ISet** ppExtensions)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExtensions::GetNonCriticalExtensions(
    /* [out] */ ISet** ppExtensions)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExtensions::HasUnsupportedCritical(
    /* [out] */ Boolean* pHasUnsupportedCritical)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExtensions::GetExtensionByOID(
    /* [in] */ const String& oid,
    /* [out] */ IX509Extension** ppExtensionByOID)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExtensions::ValueOfKeyUsage(
    /* [out, callee] */ ArrayOf<Boolean>** ppValueOfKeyUsage)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExtensions::ValueOfExtendedKeyUsage(
    /* [out] */ IList** ppValueOfExtendedKeyUsage)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExtensions::ValueOfBasicConstrains(
    /* [out] */ Int32* pValueOfBasicConstrains)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExtensions::ValueOfSubjectAlternativeName(
    /* [out] */ ICollection** ppValueOfSubjectAlternativeName)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExtensions::ValueOfIssuerAlternativeName(
    /* [out] */ ICollection** ppValueOfIssuerAlternativeName)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExtensions::ValueOfCertificateIssuerExtension(
    /* [out] */ IX500Principal** ppValueOfCertificateIssuerExtension)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExtensions::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** ppEncoded)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExtensions::Equals(
    /* [in] */ IInterface* pAi,
    /* [out] */ Boolean* pResult)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExtensions::GetHashCode(
    /* [out] */ Int32* pResult)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExtensions::DumpValue(
    /* [in] */ IStringBuilder* pSb,
    /* [in] */ const String& prefix)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExtensions::constructor()
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExtensions::constructor(
    /* [in] */ IList* pExtensions)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org