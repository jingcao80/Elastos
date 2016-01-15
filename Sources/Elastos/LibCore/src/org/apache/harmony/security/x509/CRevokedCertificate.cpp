
#include "CRevokedCertificate.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {


ECode CRevokedCertificate::GetCrlEntryExtensions(
    /* [out] */ Org::Apache::Harmony::Security::X509::IExtensions ** ppExtensions)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CRevokedCertificate::GetUserCertificate(
    /* [out] */ Elastos::Math::IBigInteger ** ppCertificate)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CRevokedCertificate::GetRevocationDate(
    /* [out] */ Elastos::Utility::IDate ** ppDate)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CRevokedCertificate::GetIssuer(
    /* [out] */ Elastosx::Security::Auth::X500::IX500Principal ** ppIssuer)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CRevokedCertificate::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte> ** ppEncoded)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CRevokedCertificate::Equals(
    /* [in] */ IInterface * pRc,
    /* [out] */ Boolean * pResult)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CRevokedCertificate::GetHashCode(
    /* [out] */ Int32 * pHashCode)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CRevokedCertificate::DumpValue(
    /* [in] */ Elastos::Core::IStringBuilder * pSb,
    /* [in] */ const String& prefix)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CRevokedCertificate::constructor(
    /* [in] */ Elastos::Math::IBigInteger * pUserCertificate,
    /* [in] */ Elastos::Utility::IDate * pRevocationDate,
    /* [in] */ Org::Apache::Harmony::Security::X509::IExtensions * pCrlEntryExtensions)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

}
}
}
}
}

