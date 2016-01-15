
#include "CTrustedCertificateStore.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {


ECode CTrustedCertificateStore::GetCertificate(
    /* [in] */ const String& alias,
    /* [out] */ Elastos::Security::Cert::ICertificate ** ppCertificate)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTrustedCertificateStore::GetCertificateEx(
    /* [in] */ const String& alias,
    /* [in] */ Boolean includeDeletedSystem,
    /* [out] */ Elastos::Security::Cert::ICertificate ** ppCertificate)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTrustedCertificateStore::GetCreationDate(
    /* [in] */ const String& alias,
    /* [out] */ Elastos::Utility::IDate ** ppDate)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTrustedCertificateStore::Aliases(
    /* [out] */ Elastos::Utility::ISet ** ppAliases)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTrustedCertificateStore::UserAliases(
    /* [out] */ Elastos::Utility::ISet ** ppAliases)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTrustedCertificateStore::AllSystemAliases(
    /* [out] */ Elastos::Utility::ISet ** ppAliases)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTrustedCertificateStore::ContainsAlias(
    /* [in] */ const String& alias,
    /* [out] */ Boolean * pIsContained)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTrustedCertificateStore::GetCertificateAlias(
    /* [in] */ Elastos::Security::Cert::ICertificate * pC,
    /* [out] */ String * pAlias)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTrustedCertificateStore::IsUserAddedCertificate(
    /* [in] */ Elastos::Security::Cert::IX509Certificate * pCert,
    /* [out] */ Boolean * pIsUserAddedCertificate)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTrustedCertificateStore::IsTrustAnchor(
    /* [in] */ Elastos::Security::Cert::IX509Certificate * pC,
    /* [out] */ Boolean * pIsTrustAnchor)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTrustedCertificateStore::FindIssuer(
    /* [in] */ Elastos::Security::Cert::IX509Certificate * pC,
    /* [out] */ Elastos::Security::Cert::IX509Certificate ** ppIssuer)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTrustedCertificateStore::GetCertificateChain(
    /* [in] */ Elastos::Security::Cert::IX509Certificate * pLeaf,
    /* [out] */ Elastos::Utility::IList ** ppChain)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTrustedCertificateStore::InstallCertificate(
    /* [in] */ Elastos::Security::Cert::IX509Certificate * pCert)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTrustedCertificateStore::DeleteCertificateEntry(
    /* [in] */ const String& alias)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTrustedCertificateStore::constructor()
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTrustedCertificateStore::constructor(
    /* [in] */ Elastos::IO::IFile * pSystemDir,
    /* [in] */ Elastos::IO::IFile * pAddedDir,
    /* [in] */ Elastos::IO::IFile * pDeletedDir)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

}
}
}
}
}
}

