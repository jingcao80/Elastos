
#include "org/apache/harmony/security/x509/CRevokedCertificate.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_OBJECT_IMPL(CRevokedCertificate)

CAR_INTERFACE_IMPL(CRevokedCertificate, Object, IRevokedCertificate)

ECode CRevokedCertificate::GetCrlEntryExtensions(
    /* [out] */ IExtensions** ppExtensions)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CRevokedCertificate::GetUserCertificate(
    /* [out] */ IBigInteger** ppCertificate)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CRevokedCertificate::GetRevocationDate(
    /* [out] */ IDate** ppDate)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CRevokedCertificate::GetIssuer(
    /* [out] */ IX500Principal** ppIssuer)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CRevokedCertificate::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** ppEncoded)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CRevokedCertificate::Equals(
    /* [in] */ IInterface* pRc,
    /* [out] */ Boolean* pResult)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CRevokedCertificate::GetHashCode(
    /* [out] */ Int32* pHashCode)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CRevokedCertificate::DumpValue(
    /* [in] */ IStringBuilder* pSb,
    /* [in] */ const String& prefix)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CRevokedCertificate::constructor(
    /* [in] */ EIBigInteger* pUserCertificate,
    /* [in] */ IDate* pRevocationDate,
    /* [in] */ IExtensions* pCrlEntryExtensions)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org