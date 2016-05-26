
#ifndef __ORG_APACHE_HARMONY_SECURITY_PROVIDER_CERT_CX509CRLIMPL_H__
#define __ORG_APACHE_HARMONY_SECURITY_PROVIDER_CERT_CX509CRLIMPL_H__

#include "_Org_Apache_Harmony_Security_Provider_Cert_CX509CRLImpl.h"

using Elastos::Security::IPublicKey;
using Elastos::Security::IPrincipal;
using Elastos::Utility::IDate;
using Elastos::Utility::ISet;
using Elastos::Math::IBigInteger;
using Elastos::Security::Cert::X509CRL;
using Elastos::Security::Cert::IX509CRLEntry;
using Elastos::Security::Cert::IX509Certificate;
using Elastosx::Security::Auth::X500::IX500Principal;
using Org::Apache::Harmony::Security::X509::ICertificateList;
using Elastos::IO::IInputStream;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Provider {
namespace Cert {

CarClass(CX509CRLImpl), public X509CRL
{
public:
    CAR_OBJECT_DECL()

    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte>** encoded);

    CARAPI Verify(
        /* [in] */ IPublicKey* key);

    CARAPI VerifyEx(
        /* [in] */ IPublicKey* key,
        /* [in] */ const String& sigProvider);

    CARAPI GetVersion(
        /* [out] */ Int32* vs);

    CARAPI GetIssuerDN(
        /* [out] */ IPrincipal** issuer);

    CARAPI GetIssuerX500Principal(
        /* [out] */ IX500Principal** issuer);

    CARAPI GetThisUpdate(
        /* [out] */ IDate** thisUpdate);

    CARAPI GetNextUpdate(
        /* [out] */ IDate** nextUpdate);

    CARAPI GetRevokedCertificate(
        /* [in] */ IBigInteger* serialNumber,
        /* [out] */ IX509CRLEntry** crlEntry);

    CARAPI GetRevokedCertificateEx(
        /* [in] */ IX509Certificate* certificate,
        /* [out] */ IX509CRLEntry** crlEntry);

    CARAPI GetRevokedCertificates(
        /* [out] */ ISet** ctrlEntrys);

    CARAPI GetTBSCertList(
        /* [out, callee] */ ArrayOf<Byte>** tbsCertList);

    CARAPI GetSignature(
        /* [out, callee] */ ArrayOf<Byte>** signature);

    CARAPI GetSigAlgName(
        /* [out] */ String* sigAlgName);

    CARAPI GetSigAlgOID(
        /* [out] */ String* sigAlgOid);

    CARAPI GetSigAlgParams(
        /* [out, callee] */ ArrayOf<Byte>** sigAlgParams);

    CARAPI IsRevoked(
        /* [in] */ ICertificate* cert,
        /* [out] */ Boolean* isRevoked);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI GetNonCriticalExtensionOIDs(
        /* [out] */ ISet** oids);

    CARAPI GetCriticalExtensionOIDs(
        /* [out] */ ISet** oids);

    CARAPI GetExtensionValue(
        /* [in] */ const String& oid,
        /* [out, callee] */ ArrayOf<Byte>** value);

    CARAPI HasUnsupportedCriticalExtension(
        /* [out] */ Boolean* hasExtensions);

    CARAPI constructor(
        /* [in] */ ICertificateList* crl);

    CARAPI constructor(
        /* [in] */ IInputStream* is);

    CARAPI constructor(
        /* [in] */ ArrayOf<Byte>* encoding);

private:
    CARAPI RetrieveEntries();

private:
        // the core object to be wrapped in X509CRL
    AutoPtr<ICertificateList> mCrl;

    // To speed up access to the info, the following fields
    // cache values retrieved from the CertificateList object
    AutoPtr<ITBSCertList> mTbsCertList;
    AutoPtr<ArrayOf<Byte> > mTbsCertListEncoding;
    AutoPtr<IExtensions> mExtensions;
    AutoPtr<IX500Principal> mIssuer;
    AutoPtr<IArrayList> mEntries;
    Int32 mEntriesSize;
    AutoPtr<ArrayOf<Byte> > mSignature;
    String mSigAlgOID;
    String mSigAlgName;
    AutoPtr<ArrayOf<Byte> > mSigAlgParams;

    // encoded form of crl
    AutoPtr<ArrayOf<Byte> > mEncoding;

    // indicates whether the signature algorithm parameters are null
    Boolean mNullSigAlgParams;
    // indicates whether the crl entries have already been retrieved
    // from CertificateList object (crl)
    Boolean mEntriesRetrieved;

    // indicates whether this X.509 CRL is direct or indirect
    // (see rfc 3280 http://www.ietf.org/rfc/rfc3280.txt, p 5.)
    Boolean mIsIndirectCRL;
    // if crl is indirect, this field holds an info about how
    // many of the leading certificates in the list are issued
    // by the same issuer as CRL.
    Int32 mNonIndirectEntriesSize;
};

} // namespace Cert
} // namespace Provider
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org


#endif // __ORG_APACHE_HARMONY_SECURITY_PROVIDER_CERT_CX509CRLIMPL_H__
