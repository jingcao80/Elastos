
#include "WrappedX509Certificate.h"

using Elastos::Security::Cert::ICertificate;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Utils {

ECode WrappedX509Certificate::constructor(
    /* [in] */ IX509Certificate* wrapped)
{
    mWrapped = wrapped;
    return NOERROR;
}

ECode WrappedX509Certificate::GetCriticalExtensionOIDs(
    /* [out] */ ISet** oids)
{
    return IX509Extension::Probe(mWrapped)->GetCriticalExtensionOIDs(oids);
}

ECode WrappedX509Certificate::GetExtensionValue(
    /* [in] */ const String& oid,
    /* [out, callee] */ ArrayOf<Byte>** extensionValue)
{
    return IX509Extension::Probe(mWrapped)->GetExtensionValue(oid, extensionValue);
}

ECode WrappedX509Certificate::GetNonCriticalExtensionOIDs(
    /* [out] */ ISet** oids)
{
    return IX509Extension::Probe(mWrapped)->GetNonCriticalExtensionOIDs(oids);
}

ECode WrappedX509Certificate::HasUnsupportedCriticalExtension(
    /* [out] */ Boolean* result)
{
    return IX509Extension::Probe(mWrapped)->HasUnsupportedCriticalExtension(result);
}

ECode WrappedX509Certificate::CheckValidity()
{
    return mWrapped->CheckValidity();
}

ECode WrappedX509Certificate::CheckValidity(
    /* [in] */ IDate* date)
{
    return mWrapped->CheckValidity(date);
}

ECode WrappedX509Certificate::GetVersion(
    /* [out] */ Int32* versionNumber)
{
    return mWrapped->GetVersion(versionNumber);
}

ECode WrappedX509Certificate::GetSerialNumber(
    /* [out] */ IBigInteger** number)
{
    return mWrapped->GetSerialNumber(number);
}

ECode WrappedX509Certificate::GetIssuerDN(
    /* [out] */ IPrincipal** issuer)
{
    return mWrapped->GetIssuerDN(issuer);
}

ECode WrappedX509Certificate::GetSubjectDN(
    /* [out] */ IPrincipal** subject)
{
    return mWrapped->GetSubjectDN(subject);
}

ECode WrappedX509Certificate::GetNotBefore(
    /* [out] */ IDate** notBefore)
{
    return mWrapped->GetNotBefore(notBefore);
}

ECode WrappedX509Certificate::GetNotAfter(
    /* [out] */ IDate** notAfter)
{
    return mWrapped->GetNotAfter(notAfter);
}

ECode WrappedX509Certificate::GetTBSCertificate(
    /* [out, callee] */ ArrayOf<Byte>** tbsCertificate)
{
    return mWrapped->GetTBSCertificate(tbsCertificate);
}

ECode WrappedX509Certificate::GetSignature(
    /* [out, callee] */ ArrayOf<Byte>** signature)
{
    return mWrapped->GetSignature(signature);
}

ECode WrappedX509Certificate::GetSigAlgName(
    /* [out] */ String* sigAlgName)
{
    return mWrapped->GetSigAlgName(sigAlgName);
}

ECode WrappedX509Certificate::GetSigAlgOID(
    /* [out] */ String* oid)
{
    return mWrapped->GetSigAlgOID(oid);
}

ECode WrappedX509Certificate::GetSigAlgParams(
    /* [out, callee] */ ArrayOf<Byte>** sigAlgParams)
{
    return mWrapped->GetSigAlgParams(sigAlgParams);
}

ECode WrappedX509Certificate::GetIssuerUniqueID(
    /* [out, callee] */ ArrayOf<Boolean>** issuerUniqueID)
{
    return mWrapped->GetIssuerUniqueID(issuerUniqueID);
}

ECode WrappedX509Certificate::GetSubjectUniqueID(
    /* [out, callee] */ ArrayOf<Boolean>** subjectUniqueID)
{
    return mWrapped->GetSubjectUniqueID(subjectUniqueID);
}

ECode WrappedX509Certificate::GetKeyUsage(
    /* [out, callee] */ ArrayOf<Boolean>** keyUsage)
{
    return mWrapped->GetKeyUsage(keyUsage);
}

ECode WrappedX509Certificate::GetBasicConstraints(
    /* [out] */ Int32* constraints)
{
    return mWrapped->GetBasicConstraints(constraints);
}

ECode WrappedX509Certificate::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** encoded)
{
    return ICertificate::Probe(mWrapped)->GetEncoded(encoded);
}

ECode WrappedX509Certificate::Verify(
    /* [in] */ IPublicKey* key)
{
    return ICertificate::Probe(mWrapped)->Verify(key);
}

ECode WrappedX509Certificate::Verify(
    /* [in] */ IPublicKey* key,
    /* [in] */ const String& sigProvider)
{
    return ICertificate::Probe(mWrapped)->Verify(key, sigProvider);
}

ECode WrappedX509Certificate::ToString(
    /* [out] */ String* str)
{
    return IObject::Probe(mWrapped)->ToString(str);
}

ECode WrappedX509Certificate::GetPublicKey(
    /* [out] */ IPublicKey** key)
{
    return ICertificate::Probe(mWrapped)->GetPublicKey(key);
}

} // namespace Utils
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org
