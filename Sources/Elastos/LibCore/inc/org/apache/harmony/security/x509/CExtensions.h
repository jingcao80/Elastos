
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CEXTENSIONS_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CEXTENSIONS_H__

#include "_Org_Apache_Harmony_Security_X509_CExtensions.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CExtensions)
{
public:
    CARAPI GetSize(
        /* [out] */ Int32 * pSize);

    CARAPI GetCriticalExtensions(
        /* [out] */ Elastos::Utility::ISet ** ppExtensions);

    CARAPI GetNonCriticalExtensions(
        /* [out] */ Elastos::Utility::ISet ** ppExtensions);

    CARAPI HasUnsupportedCritical(
        /* [out] */ Boolean * pHasUnsupportedCritical);

    CARAPI GetExtensionByOID(
        /* [in] */ const String& oid,
        /* [out] */ Org::Apache::Harmony::Security::X509::IExtension ** ppExtensionByOID);

    CARAPI ValueOfKeyUsage(
        /* [out, callee] */ ArrayOf<Boolean> ** ppValueOfKeyUsage);

    CARAPI ValueOfExtendedKeyUsage(
        /* [out] */ Elastos::Utility::IList ** ppValueOfExtendedKeyUsage);

    CARAPI ValueOfBasicConstrains(
        /* [out] */ Int32 * pValueOfBasicConstrains);

    CARAPI ValueOfSubjectAlternativeName(
        /* [out] */ Elastos::Utility::ICollection ** ppValueOfSubjectAlternativeName);

    CARAPI ValueOfIssuerAlternativeName(
        /* [out] */ Elastos::Utility::ICollection ** ppValueOfIssuerAlternativeName);

    CARAPI ValueOfCertificateIssuerExtension(
        /* [out] */ Elastosx::Security::Auth::X500::IX500Principal ** ppValueOfCertificateIssuerExtension);

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte> ** ppEncoded);

    CARAPI Equals(
        /* [in] */ IInterface * pAi,
        /* [out] */ Boolean * pResult);

    CARAPI GetHashCode(
        /* [out] */ Int32 * pResult);

    CARAPI DumpValue(
        /* [in] */ Elastos::Core::IStringBuilder * pSb,
        /* [in] */ const String& prefix);

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Elastos::Utility::IList * pExtensions);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CEXTENSIONS_H__
