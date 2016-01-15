
#ifndef __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CTRUSTEDCERTIFICATESTORE_H__
#define __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CTRUSTEDCERTIFICATESTORE_H__

#include "_Org_Apache_Harmony_Xnet_Provider_Jsse_CTrustedCertificateStore.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CarClass(CTrustedCertificateStore)
{
public:
    CARAPI GetCertificate(
        /* [in] */ const String& alias,
        /* [out] */ Elastos::Security::Cert::ICertificate ** ppCertificate);

    CARAPI GetCertificateEx(
        /* [in] */ const String& alias,
        /* [in] */ Boolean includeDeletedSystem,
        /* [out] */ Elastos::Security::Cert::ICertificate ** ppCertificate);

    CARAPI GetCreationDate(
        /* [in] */ const String& alias,
        /* [out] */ Elastos::Utility::IDate ** ppDate);

    CARAPI Aliases(
        /* [out] */ Elastos::Utility::ISet ** ppAliases);

    CARAPI UserAliases(
        /* [out] */ Elastos::Utility::ISet ** ppAliases);

    CARAPI AllSystemAliases(
        /* [out] */ Elastos::Utility::ISet ** ppAliases);

    CARAPI ContainsAlias(
        /* [in] */ const String& alias,
        /* [out] */ Boolean * pIsContained);

    CARAPI GetCertificateAlias(
        /* [in] */ Elastos::Security::Cert::ICertificate * pC,
        /* [out] */ String * pAlias);

    CARAPI IsUserAddedCertificate(
        /* [in] */ Elastos::Security::Cert::IX509Certificate * pCert,
        /* [out] */ Boolean * pIsUserAddedCertificate);

    CARAPI IsTrustAnchor(
        /* [in] */ Elastos::Security::Cert::IX509Certificate * pC,
        /* [out] */ Boolean * pIsTrustAnchor);

    CARAPI FindIssuer(
        /* [in] */ Elastos::Security::Cert::IX509Certificate * pC,
        /* [out] */ Elastos::Security::Cert::IX509Certificate ** ppIssuer);

    CARAPI GetCertificateChain(
        /* [in] */ Elastos::Security::Cert::IX509Certificate * pLeaf,
        /* [out] */ Elastos::Utility::IList ** ppChain);

    CARAPI InstallCertificate(
        /* [in] */ Elastos::Security::Cert::IX509Certificate * pCert);

    CARAPI DeleteCertificateEntry(
        /* [in] */ const String& alias);

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Elastos::IO::IFile * pSystemDir,
        /* [in] */ Elastos::IO::IFile * pAddedDir,
        /* [in] */ Elastos::IO::IFile * pDeletedDir);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CTRUSTEDCERTIFICATESTORE_H__
