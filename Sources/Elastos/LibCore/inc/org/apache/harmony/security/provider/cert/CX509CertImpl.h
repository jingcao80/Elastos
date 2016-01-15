
#ifndef __ORG_APACHE_HARMONY_SECURITY_PROVIDER_CERT_CX509CERTIMPL_H__
#define __ORG_APACHE_HARMONY_SECURITY_PROVIDER_CERT_CX509CERTIMPL_H__

#include "_Org_Apache_Harmony_Security_Provider_Cert_CX509CertImpl.h"

using Elastos::Security::Cert::X509Certificate;
using Elastos::Security::IPublicKey;
using Elastos::Utility::IDate;
using Elastos::Math::IBigInteger;
using Elastos::Security::IPrincipal;
using Elastosx::Security::Auth::X500::IX500Principal;
using Elastos::Utility::ICollection;
using Elastos::IO::IInputStream;
using Org::Apache::Harmony::Security::X509::ICertificate;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Provider {
namespace Cert {

CarClass(CX509CertImpl), public X509Certificate
{
public:
    CARAPI GetType(
        /* [out] */ String* type);

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

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI GetPublicKey(
        /* [out] */ IPublicKey** key);

    CARAPI CheckValidity();

    CARAPI CheckValidityEx(
        /* [in] */ IDate* date);

    CARAPI GetVersion(
        /* [out] */ Int32* versionNumber);

    CARAPI GetSerialNumber(
        /* [out] */ IBigInteger** number);

    CARAPI GetIssuerDN(
        /* [out] */ IPrincipal** issuer);

    CARAPI GetIssuerX500Principal(
        /* [out] */ IX500Principal** issuerName);

    CARAPI GetSubjectDN(
        /* [out] */ IPrincipal** subject);

    CARAPI GetSubjectX500Principal(
        /* [out] */ IX500Principal** subject);

    CARAPI GetNotBefore(
        /* [out] */ IDate** notBefore);

    CARAPI GetNotAfter(
        /* [out] */ IDate** notAfter);

    CARAPI GetTBSCertificate(
        /* [out, callee] */ ArrayOf<Byte>** tbsCertificate);

    CARAPI GetSignature(
        /* [out, callee] */ ArrayOf<Byte>** signature);

    CARAPI GetSigAlgName(
        /* [out] */ String* sigAlgName);

    CARAPI GetSigAlgOID(
        /* [out] */ String* oid);

    CARAPI GetSigAlgParams(
        /* [out, callee] */ ArrayOf<Byte>** sigAlgParams);

    CARAPI GetIssuerUniqueID(
        /* [out, callee] */ ArrayOf<Boolean>** issuerUniqueID);

    CARAPI GetSubjectUniqueID(
        /* [out, callee] */ ArrayOf<Boolean>** subjectUniqueID);

    CARAPI GetKeyUsage(
        /* [out, callee] */ ArrayOf<Boolean>** keyUsage);

    CARAPI GetExtendedKeyUsage(
        /* [out] */ IList** keyUsage);

    CARAPI GetBasicConstraints(
        /* [out] */ Int32* constraints);

    CARAPI GetSubjectAlternativeNames(
        /* [out] */ ICollection** subjectAlternativeNames);

    CARAPI GetIssuerAlternativeNames(
        /* [out] */ ICollection** ppIssuerAlternativeNames);

    CARAPI GetCriticalExtensionOIDs(
        /* [out] */ ISet** oids);

    CARAPI GetExtensionValue(
        /* [in] */ const String& oid,
        /* [out, callee] */ ArrayOf<Byte>** extensionValue);

    CARAPI GetNonCriticalExtensionOIDs(
        /* [out] */ ISet** oids);

    CARAPI HasUnsupportedCriticalExtension(
        /* [out] */ Boolean* result);

    CARAPI constructor(
        /* [in] */ IInputStream* is);

    CARAPI constructor(
        /* [in] */ ICertificate* certificate);

    CARAPI constructor(
        /* [in] */ ArrayOf<Byte>* encoding);

private:
    CARAPI CheckValidity(
        /* [in] */ Int64 tm);

    CARAPI GetNotBeforeInternal(
        /* [out] */ Int64* nbi);

    CARAPI GetNotAfterInternal(
        /* [out] */ Int64* nbi);

    CARAPI GetTbsCertificateInternal(
        /* [out, callee] */ ArrayOf<Byte>** ret);

    CARAPI GetSignatureInternal(
        /* [out, callee] */ ArrayOf<Byte>** ret);

    CARAPI GetEncodedInternal(
        /* [out, callee] */ ArrayOf<Byte>** ret);

private:
        /** @serial */
    static Int64 sSerialVersionUID = 2972248729446736154LL;

    /** the core object to be wrapped in X509Certificate */
     AutoPtr<ICertificate> mCertificate;

    // to speed up access to the info, the following fields
    // cache values retrieved from the certificate object,
    // initialized using the "single-check idiom".
    AutoPtr<ITBSCertificate> mTbsCert;
    AutoPtr<IExtensions> mExtensions;
    volatile Int64 mNotBefore = -1;
    volatile Int64 mNotAfter = -1;
    volatile AutoPtr<IBigInteger> mSerialNumber;
    volatile AutoPtr<IX500Principal> mIssuer;
    volatile AUtoPtr<IX500Principal> mSubject;
    volatile AutoPtr<ArrayOf<Byte> > mTbsCertificate;
    volatile AutoPtr<ArrayOf<Byte> > mSignature;
    volatile String mSigAlgName;
    volatile String mSigAlgOID;
    volatile AutoPtr<ArrayOf<Byte> > mSigAlgParams;
    // indicates whether the signature algorithm parameters are null
    volatile Boolean mNullSigAlgParams;
    volatile AutoPtr<IPublicKey> mPublicKey;

    // encoding of the certificate
    volatile AutoPtr<ArrayOf<Byte> > mEncoding;
};

} // namespace Cert
} // namespace Provider
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_PROVIDER_CERT_CX509CERTIMPL_H__
