
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CREVOKEDCERTIFICATE_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CREVOKEDCERTIFICATE_H__

#include "_Org_Apache_Harmony_Security_X509_CRevokedCertificate.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CRevokedCertificate)
    , public Object
    , public IRevokedCertificate
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetCrlEntryExtensions(
        /* [out] */ Org::Apache::Harmony::Security::X509::IExtensions ** ppExtensions);

    CARAPI GetUserCertificate(
        /* [out] */ Elastos::Math::IBigInteger ** ppCertificate);

    CARAPI GetRevocationDate(
        /* [out] */ Elastos::Utility::IDate ** ppDate);

    CARAPI GetIssuer(
        /* [out] */ Elastosx::Security::Auth::X500::IX500Principal ** ppIssuer);

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte> ** ppEncoded);

    CARAPI Equals(
        /* [in] */ IInterface * pRc,
        /* [out] */ Boolean * pResult);

    CARAPI GetHashCode(
        /* [out] */ Int32 * pHashCode);

    CARAPI DumpValue(
        /* [in] */ Elastos::Core::IStringBuilder * pSb,
        /* [in] */ const String& prefix);

    CARAPI constructor(
        /* [in] */ Elastos::Math::IBigInteger * pUserCertificate,
        /* [in] */ Elastos::Utility::IDate * pRevocationDate,
        /* [in] */ Org::Apache::Harmony::Security::X509::IExtensions * pCrlEntryExtensions);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CREVOKEDCERTIFICATE_H__
