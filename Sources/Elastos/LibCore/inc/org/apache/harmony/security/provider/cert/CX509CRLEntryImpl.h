
#ifndef __ORG_APACHE_HARMONY_SECURITY_PROVIDER_CERT_CX509CRLENTRYIMPL_H__
#define __ORG_APACHE_HARMONY_SECURITY_PROVIDER_CERT_CX509CRLENTRYIMPL_H__

#include "_Org_Apache_Harmony_Security_Provider_Cert_CX509CRLEntryImpl.h"

using Elastos::Utility::ISet;
using Elastos::Math::IBigInteger;
using Elastosx::Security::Auth::X500::IX500Principal;
using Elastos::Utility::IDate;
using Org::Apache::Harmony::Security::X509::IRevokedCertificate;
using Elastosx::Security::Auth::X500::IX500Principal;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Provider {
namespace Cert {

CarClass(CX509CRLEntryImpl), public X509CRLEntry
{
public:
    CAR_OBJECT_DECL()

    CARAPI GetCriticalExtensionOIDs(
        /* [out] */ ISet** oids);

    CARAPI GetExtensionValue(
        /* [in] */ const String& oid,
        /* [out, callee] */ ArrayOf<Byte>** extensionValue);

    CARAPI GetNonCriticalExtensionOIDs(
        /* [out] */ ISet** oids);

    CARAPI HasUnsupportedCriticalExtension(
        /* [out] */ Boolean* result);

    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte>** encoded);

    CARAPI GetSerialNumber(
        /* [out] */ IBigInteger** number);

    CARAPI GetCertificateIssuer(
        /* [out] */ IX500Principal** issuer);

    CARAPI GetRevocationDate(
        /* [out] */ IDate** revocationDate);

    CARAPI HasExtensions(
        /* [out] */ Boolean* result);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI constructor(
        /* [in] */ IRevokedCertificate* rcert,
        /* [in] */ IX500Principal* issuer);

private:
    // the crl entry object to be wrapped in X509CRLEntry
    AutoPtr<IRevokedCertificate> mRcert;
    // the extensions of the entry
    AutoPtr<IExtensions> mExtensions;
    // issuer of the revoked certificate described by this crl entry
    AutoPtr<IX500Principal> mIssuer;

    // encoded form of this revoked certificate entry
    AutoPtr<ArrayOf<Byte> > mEncoding;

};

} // namespace Cert
} // namespace Provider
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_PROVIDER_CERT_CX509CRLENTRYIMPL_H__
