
#ifndef __ORG_APACHE_HARMONY_SECURITY_UTILS_WRAPPEDX509CERTIFICATE_H__
#define __ORG_APACHE_HARMONY_SECURITY_UTILS_WRAPPEDX509CERTIFICATE_H__

#include "Elastos.CoreLibrary.Security.h"
#include "security/cert/X509Certificate.h"

using Elastos::Security::IPublicKey;
using Elastos::Utility::IDate;
using Elastos::Utility::ISet;
using Elastos::Security::Cert::X509Certificate;
using Elastos::Security::Cert::IX509Certificate;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Utils {

class WrappedX509Certificate : public X509Certificate
{
public:
    CARAPI constructor(
        /* [in] */ IX509Certificate* wrapped);

    // @Override
    CARAPI GetCriticalExtensionOIDs(
        /* [out] */ ISet** oids);

    // @Override
    CARAPI GetExtensionValue(
        /* [in] */ const String& oid,
        /* [out, callee] */ ArrayOf<Byte>** extensionValue);

    // @Override
    CARAPI GetNonCriticalExtensionOIDs(
        /* [out] */ ISet** oids);

    // @Override
    CARAPI HasUnsupportedCriticalExtension(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI CheckValidity();

    // @Override
    CARAPI CheckValidity(
        /* [in] */ IDate* date);

    // @Override
    CARAPI GetVersion(
        /* [out] */ Int32* versionNumber);

    // @Override
    CARAPI GetSerialNumber(
        /* [out] */ IBigInteger** number);

    // @Override
    CARAPI GetIssuerDN(
        /* [out] */ IPrincipal** issuer);

    // @Override
    CARAPI GetSubjectDN(
        /* [out] */ IPrincipal** subject);

    // @Override
    CARAPI GetNotBefore(
        /* [out] */ IDate** notBefore);

    // @Override
    CARAPI GetNotAfter(
        /* [out] */ IDate** notAfter);

    // @Override
    CARAPI GetTBSCertificate(
        /* [out, callee] */ ArrayOf<Byte>** tbsCertificate);

    // @Override
    CARAPI GetSignature(
        /* [out, callee] */ ArrayOf<Byte>** signature);

    // @Override
    CARAPI GetSigAlgName(
        /* [out] */ String* sigAlgName);

    // @Override
    CARAPI GetSigAlgOID(
        /* [out] */ String* oid);

    // @Override
    CARAPI GetSigAlgParams(
        /* [out, callee] */ ArrayOf<Byte>** sigAlgParams);

    // @Override
    CARAPI GetIssuerUniqueID(
        /* [out, callee] */ ArrayOf<Boolean>** issuerUniqueID);

    // @Override
    CARAPI GetSubjectUniqueID(
        /* [out, callee] */ ArrayOf<Boolean>** subjectUniqueID);

    // @Override
    CARAPI GetKeyUsage(
        /* [out, callee] */ ArrayOf<Boolean>** keyUsage);

    // @Override
    CARAPI GetBasicConstraints(
        /* [out] */ Int32* constraints);

    // @Override
    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte>** encoded);

    // @Override
    CARAPI Verify(
        /* [in] */ IPublicKey* key);

    // @Override
    CARAPI Verify(
        /* [in] */ IPublicKey* key,
        /* [in] */ const String& sigProvider);

    // @Override
    CARAPI ToString(
        /* [out] */ String* str);

    // @Override
    CARAPI GetPublicKey(
        /* [out] */ IPublicKey** key);

private:
    AutoPtr<IX509Certificate> mWrapped;
};

} // namespace Utils
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif //__ORG_APACHE_HARMONY_SECURITY_UTILS_WRAPPEDX509CERTIFICATE_H__
