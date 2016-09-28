
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CEXTENSIONS_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CEXTENSIONS_H__

#include "_Org_Apache_Harmony_Security_X509_CExtensions.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Core::IStringBuilder;
using Elastos::Utility::ISet;
using Elastos::Utility::IList;
using Elastos::Utility::ICollection;
using Elastosx::Security::Auth::X500::IX500Principal;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CExtensions)
    , public Object
    , public IExtensions
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetSize(
        /* [out] */ Int32* pSize);

    CARAPI GetCriticalExtensions(
        /* [out] */ ISet** ppExtensions);

    CARAPI GetNonCriticalExtensions(
        /* [out] */ ISet** ppExtensions);

    CARAPI HasUnsupportedCritical(
        /* [out] */ Boolean* pHasUnsupportedCritical);

    CARAPI GetExtensionByOID(
        /* [in] */ const String& oid,
        /* [out] */ IX509Extension** ppExtensionByOID);

    CARAPI ValueOfKeyUsage(
        /* [out, callee] */ ArrayOf<Boolean>** ppValueOfKeyUsage);

    CARAPI ValueOfExtendedKeyUsage(
        /* [out] */ IList** ppValueOfExtendedKeyUsage);

    CARAPI ValueOfBasicConstrains(
        /* [out] */ Int32* pValueOfBasicConstrains);

    CARAPI ValueOfSubjectAlternativeName(
        /* [out] */ ICollection** ppValueOfSubjectAlternativeName);

    CARAPI ValueOfIssuerAlternativeName(
        /* [out] */ ICollection** ppValueOfIssuerAlternativeName);

    CARAPI ValueOfCertificateIssuerExtension(
        /* [out] */ IX500Principal** ppValueOfCertificateIssuerExtension);

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte>** ppEncoded);

    CARAPI Equals(
        /* [in] */ IInterface* pAi,
        /* [out] */ Boolean* pResult);

    CARAPI GetHashCode(
        /* [out] */ Int32* pResult);

    CARAPI DumpValue(
        /* [in] */ IStringBuilder* pSb,
        /* [in] */ const String& prefix);

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IList* pExtensions);

private:
    // TODO: Add your private member variables here.
};

} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CEXTENSIONS_H__
