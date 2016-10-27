
#ifndef __ORG_CONSCRYPT_OPENSSLX509CERTIFICATE_H__
#define __ORG_CONSCRYPT_OPENSSLX509CERTIFICATE_H__

#include "Elastos.CoreLibrary.Extensions.h"
#include "elastos/security/cert/X509Certificate.h"
#include "_Org.Conscrypt.h"
#include <elastos/core/Object.h>

using Elastos::IO::IInputStream;
using Elastos::Math::IBigInteger;
using Elastos::Utility::ICollection;
using Elastos::Utility::ISet;
using Elastos::Utility::IDate;
using Elastos::Utility::IList;
using Elastos::Security::IPrincipal;
using Elastos::Security::IPublicKey;
using Elastos::Security::Cert::ICertificate;
using Elastos::Security::Cert::X509Certificate;
using Elastosx::Security::Auth::X500::IX500Principal;

namespace Org {
namespace Conscrypt {

class OpenSSLX509Certificate
    : public X509Certificate
    , public IOpenSSLX509Certificate
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ Int64 ctx);

    static CARAPI FromX509DerInputStream(
        /* [in] */ IInputStream* is,
        /* [out] */ IOpenSSLX509Certificate** result);

    static CARAPI FromX509Der(
        /* [in] */ ArrayOf<Byte>* encoded,
        /* [out] */ IOpenSSLX509Certificate** result);

    static CARAPI FromPkcs7DerInputStream(
        /* [in] */ IInputStream* is,
        /* [out] */ IList** result);

    static CARAPI FromX509PemInputStream(
        /* [in] */ IInputStream* is,
        /* [out] */ IOpenSSLX509Certificate** result);

    static CARAPI FromPkcs7PemInputStream(
        /* [in] */ IInputStream* is,
        /* [out] */ IList** result);

    static CARAPI FromCertificate(
        /* [in] */ ICertificate* cert,
        /* [out] */ IOpenSSLX509Certificate** result);

    CARAPI GetCriticalExtensionOIDs(
        /* [out] */ ISet** result);

    CARAPI GetExtensionValue(
        /* [in] */ const String& oid,
        /* [out] */ ArrayOf<Byte>** result);

    CARAPI GetNonCriticalExtensionOIDs(
        /* [out] */ ISet** result);

    CARAPI HasUnsupportedCriticalExtension(
        /* [out] */ Boolean* result);

    CARAPI CheckValidity();

    CARAPI CheckValidity(
        /* [in] */ IDate* date);

    CARAPI GetVersion(
        /* [out] */ Int32* result);

    CARAPI GetSerialNumber(
        /* [out] */ IBigInteger** result);

    CARAPI GetIssuerDN(
        /* [out] */ IPrincipal** result);

    CARAPI GetSubjectDN(
        /* [out] */ IPrincipal** result);

    CARAPI GetNotBefore(
        /* [out] */ IDate** result);

    CARAPI GetNotAfter(
        /* [out] */ IDate** result);

    CARAPI GetTBSCertificate(
        /* [out] */ ArrayOf<Byte>** result);

    CARAPI GetSignature(
        /* [out] */ ArrayOf<Byte>** result);

    CARAPI GetSigAlgName(
        /* [out] */ String* result);

    CARAPI GetSigAlgOID(
        /* [out] */ String* result);

    CARAPI GetSigAlgParams(
        /* [out] */ ArrayOf<Byte>** result);

    CARAPI GetIssuerUniqueID(
        /* [out] */ ArrayOf<Boolean>** result);

    CARAPI GetSubjectUniqueID(
        /* [out] */ ArrayOf<Boolean>** result);

    CARAPI GetKeyUsage(
        /* [out] */ ArrayOf<Boolean>** result);

    CARAPI GetBasicConstraints(
        /* [out] */ Int32* result);

    CARAPI GetEncoded(
        /* [out] */ ArrayOf<Byte>** result);

    CARAPI Verify(
        /* [in] */ IPublicKey* key);

    CARAPI Verify(
        /* [in] */ IPublicKey* key,
        /* [in] */ const String& sigProvider);

    CARAPI ToString(
        /* [out] */ String* result);

    CARAPI GetPublicKey(
        /* [out] */ IPublicKey** result);

    CARAPI GetIssuerX500Principal(
        /* [out] */ IX500Principal** result);

    CARAPI GetSubjectX500Principal(
        /* [out] */ IX500Principal** result);

    CARAPI GetExtendedKeyUsage(
        /* [out] */ IList** result);

    CARAPI GetSubjectAlternativeNames(
        /* [out] */ ICollection** result);

    CARAPI GetIssuerAlternativeNames(
        /* [out] */ ICollection** result);

    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* result);

    /**
     * Returns the raw pointer to the X509 context for use in JNI calls. The
     * life cycle of this native pointer is managed by the
     * {@code OpenSSLX509Certificate} instance and must not be destroyed or
     * freed by users of this API.
     */
    CARAPI GetContext(
        /* [out] */ Int64* result);

    CARAPI Finalize();

private:
    void VerifyOpenSSL(
        /* [in] */ IOpenSSLKey* pkey);

    void VerifyInternal(
        /* [in] */ IPublicKey* key,
        /* [in] */ String sigProvider);

    static AutoPtr<ICollection> AlternativeNameArrayToList(
        /* [in] */ ArrayOf<AutoPtr<ArrayOf<IInterface*> > >* altNameArray);

private:
    Int64 mContext;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_OPENSSLX509CERTIFICATE_H__
