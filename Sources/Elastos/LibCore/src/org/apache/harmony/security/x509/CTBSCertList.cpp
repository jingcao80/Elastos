
#include "org/apache/harmony/security/x509/CTBSCertList.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_OBJECT_IMPL(CTBSCertList)

CAR_INTERFACE_IMPL(CTBSCertList, Object, ITBSCertList)

ECode CTBSCertList::GetVersion(
    /* [out] */ Int32* pVer)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTBSCertList::GetSignature(
    /* [out] */ IAlgorithmIdentifier** ppSignature)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTBSCertList::GetIssuer(
    /* [out] */ IName** ppIssuer)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTBSCertList::GetThisUpdate(
    /* [out] */ IDate** ppDate)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTBSCertList::GetNextUpdate(
    /* [out] */ IDate** ppDate)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTBSCertList::GetRevokedCertificates(
    /* [out] */ IList** ppCertificates)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTBSCertList::GetCrlExtensions(
    /* [out] */ IExtensions** ppExtensions)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTBSCertList::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** ppEncoded)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTBSCertList::Equals(
    /* [in] */ IInterface* pRc,
    /* [out] */ Boolean* pResult)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTBSCertList::GetHashCode(
    /* [out] */ Int32* pHashCode)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTBSCertList::DumpValue(
    /* [in] */ IStringBuilder* pSb)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org