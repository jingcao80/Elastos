
#include "CX509CertImpl.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Provider {
namespace Cert {

CAR_OBJECT_IMPL(CX509CertImpl)

ECode CX509CertImpl::GetType(
    /* [out] */ String* type)
{
    return X509Certificate::GetType(type);
}

ECode CX509CertImpl::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* result)
{
    return X509Certificate::Equals(other, result);
}

ECode CX509CertImpl::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    return X509Certificate::GetHashCode(hashCode);
}

ECode CX509CertImpl::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** encoded)
{
    VALIDATE_NOT_NULL(encoded)
    AutoPtr<ArrayOf<Byte> > bytes;
    GetEncodedInternal((ArrayOf<Byte>**)&bytes);
    encoded = bytes->Clone();
    REFCOUNT_ADD(encoded)
    return NOERROR;
}

ECode CX509CertImpl::GetEncodedInternal(
    /* [out, callee] */ ArrayOf<Byte>** ret)
{
    VALIDATE_NOT_NULL(ret)
    AutoPtr<ArrayOf<Byte> > result = mEncoding;
    if (mEncoding == NULL) {
        mCertificate->GetEncoded((ArrayOf<Byte>**)&result);
        mEncoding = result;
    }
    *ret = result;
    REFCOUNT_ADD(*ret)
    return NOERROR;
}

ECode CX509CertImpl::Verify(
    /* [in] */ IPublicKey* key)
{
    AutoPtr<ISignature> signature;
    AutoPtr<ISignatureHelper> helper;
    CSignatureHelper::AcquireSingleton((ISignatureHelper**)&helper);
    String algName;
    GetSigAlgName(&algName);
    ECode ec = helper->GetInstanceEx(algName, IOpenSSLProvider::PROVIDER_NAME, (ISignature**)&signature);
    if (E_NO_SUCH_ALGORITHM_EXCEPTION == ec) {
        helper->GetInstance(algName, (ISignature**)&signature);
        ec = NOERROR;
    }

    signature->InitVerify(key);
    // retrieve the encoding of the TBSCertificate structure
    AutoPtr<ArrayOf<Byte> > tbsCertificateLocal;
    GetTbsCertificateInternal((ArrayOf<Byte>**)&tbsCertificateLocal);
    // compute and verify the signature
    signature->UpdateEx2(tbsCertificateLocal, 0, tbsCertificateLocal->GetLength());
    AutoPtr<ArrayOf<Byte> > sv;
    mCertificate->GetSignatureValue((ArrayOf<Byte>**)&sv);
    Boolean veri;
    signature->Verify(sv, &veri);
    if (!veri) {
        //throw new SignatureException("Signature was not verified");
        return E_SIGNATURE_EXCEPTION;
    }
    return NOERROR;
}

ECode CX509CertImpl::VerifyEx(
    /* [in] */ IPublicKey* key,
    /* [in] */ const String& sigProvider)
{
    AutoPtr<ISignature> signature;
    ECode ec;
    if (sigProvider.IsNull()) {
        AutoPtr<ISignatureHelper> helper;
        CSignatureHelper::AcquireSingleton((ISignatureHelper**)&helper);
        String algName;
        GetSigAlgName(&algName);
        ec = helper->GetInstanceEx(algName, IOpenSSLProvider::PROVIDER_NAME, (ISignature**)&signature);
    }
    else {
        ec = helper->GetInstance(algName, sigProvider, (ISignature**)&signature);
    }
    if (E_NO_SUCH_ALGORITHM_EXCEPTION == ec) {
        ec = helper->GetInstance(algName, sigProvider, (ISignature**)&signature);
    }
    signature->InitVerify(key);
    AutoPtr<ArrayOf<Byte> > tbsCertificateLocal;
    GetTbsCertificateInternal((ArrayOf<Byte>**)&tbsCertificateLocal);
    signature->UpdateEx2(tbsCertificateLocal, 0, tbsCertificateLocal->GetLength());
    AutoPtr<ArrayOf<Byte> > sv;
    mCertificate->GetSignatureValue((ArrayOf<Byte>**)&sv);
    Boolean veri;
    signature->Verify(sv, &veri);
    if (!veri) {
        //throw new SignatureException("Signature was not verified");
        ec = E_SIGNATURE_EXCEPTION;
    }
    return ec;
}

ECode CX509CertImpl::ToString(
    /* [out] */ String* str)
{
    return mCertificate->ToString(str);
}

ECode CX509CertImpl::GetPublicKey(
    /* [out] */ IPublicKey** key)
{
    VALIDATE_NOT_NULL(key)
    AutoPtr<IPublicKey> result = mPublicKey;
    if (result == NULL) {
        AutoPtr<ISubjectPublicKeyInfo> info;
        mTbsCert->GetSubjectPublicKeyInfo((ISubjectPublicKeyInfo**)&info);
        info->GetPublicKey((IPublicKey**)&result);
        mPublicKey = result;
    }
    *key = result;
    REFCOUNT_ADD(*key)
    return NOERROR;
}

ECode CX509CertImpl::CheckValidity()
{
    Int64 tm;
    System::GetCurrentTimeMillis(&tm);
    AutoPtr<IDate> dt;
    CDate::New(tm, (IDate**)&dt);
    return CheckValidity(dt);
}

ECode CX509CertImpl::CheckValidityEx(
    /* [in] */ IDate* date)
{
    Int64 tm;
    GetTime(&tm);
    return CheckValidity(tm);
}

ECode CX509CertImpl::CheckValidity(
    /* [in] */ Int64 tm)
{
    Int64 notBefore, notAfter;
    GetNotBeforeInternal(&notBefore);
    GetNotAfterInternal(&notAfter);

    if (tm < notBefore) {
        /*throw new CertificateNotYetValidException("current time: " + new Date(time)
            + ", validation time: " + new Date(getNotBeforeInternal()));*/
        return E_CERTIFICATE_NOT_YET_VALID_EXCEPTION;
    }
    if (tm > notAfter) {
        /*throw new CertificateExpiredException("current time: " + new Date(time)
            + ", expiration time: " + new Date(getNotAfterInternal()));*/
        return E_CERTIFICATE_EXPIRED_EXCEPTION;
    }
    return NOERROR;
}

ECode CX509CertImpl::GetVersion(
    /* [out] */ Int32* versionNumber)
{
    VALIDATE_NOT_NULL(versionNumber)
    mTbsCert->GetVersion(versionNumber);
    *versionNumber += 1;
    return NOERROR;
}

ECode CX509CertImpl::GetSerialNumber(
    /* [out] */ IBigInteger** number)
{
    VALIDATE_NOT_NULL(number)
    AutoPtr<IBigInteger> result = mSerialNumber;
    if (result == NULL) {
        mTbsCert->GetSerialNumber(&result);
        mSerialNumber = result;
    }
    *number = result;
    REFCOUNT_ADD(*number)
    return NOERROR;
}

ECode CX509CertImpl::GetIssuerDN(
    /* [out] */ IPrincipal** issuer)
{
    return GetIssuerX500Principal(issuer);
}

ECode CX509CertImpl::GetIssuerX500Principal(
    /* [out] */ IX500Principal** issuerName)
{
    VALIDATE_NOT_NULL(issuerName)
    AutoPtr<IX500Principal> result = mIssuer;
    if (result == NULL) {
        // retrieve the issuer's principal
        AutoPtr<IName> name;
        mTbsCert->GetIssuer((IName**)&name);
        name->GetX500Principal((IX500Principal**)&result);
        mIssuer = result;
    }
    *issuerName = result;
    REFCOUNT_ADD(*issuerName)
    return NOERROR;
}

ECode CX509CertImpl::GetSubjectDN(
    /* [out] */ IPrincipal** subject)
{
    return GetSubjectX500Principal(subject);
}

ECode CX509CertImpl::GetSubjectX500Principal(
    /* [out] */ IX500Principal** subject)
{
    VALIDATE_NOT_NULL(subject)
    AutoPtr<IX500Principal> result = mSubject;
    if (result == NULL) {
        // retrieve the subject's principal
        AutoPtr<IName> name;
        mTbsCert->GetSubject((IName**)&name);
        name->GetX500Principal((IX500Principal**)&result);
        mSubject = result;
    }
    *subject = result;
    REFCOUNT_ADD(*subject)
    return NOERROR;
}

ECode CX509CertImpl::GetNotBeforeInternal(
    /* [out] */ Int64* nbi)
{
    VALIDATE_NOT_NULL(nbi)
    Int64 result = mNotBefore;
    if (result == -1) {
        AutoPtr<IValidity> valid;
        mTbsCert->GetValidity((IValidity**)&valid);
        AutoPtr<IDate> dt;
        valid->GetNotBefore((IDate**)&dt);
        dt->GetTime(&result);
        mNotBefore = result;
    }
    *nbi = result;
    return NOERROR;
}

ECode CX509CertImpl::GetNotBefore(
    /* [out] */ IDate** notBefore)
{
    Int64 notBefore;
    GetNotBeforeInternal(&notBefore);
    return CDate::New(notBefore, notBefore);
}

ECode CX509CertImpl::GetNotAfter(
    /* [out] */ IDate** notAfter)
{
    Int64 tm;
    GetNotAfterInternal(&tm);
    return CDate::New(tm, notAfter);
}

ECode CX509CertImpl::GetNotAfterInternal(
    /* [out] */ Int64* nbi)
{
    VALIDATE_NOT_NULL(nbi)
    Int64 result = mNotAfter;
    if (result == -1) {
        AutoPtr<IValidity> valid;
        mTbsCert->GetValidity((IValidity**)&valid);
        AutoPtr<IDate> dt;
        valid->GetNotAfter((IDate**)&dt);
        dt->GetTime(&result);
        mNotAfter = result;
    }
    *nbi = result;
    return NOERROR;
}

ECode CX509CertImpl::GetTBSCertificate(
    /* [out, callee] */ ArrayOf<Byte>** tbsCertificate)
{
    VALIDATE_NOT_NULL(tbsCertificate)
    AutoPtr<ArrayOf<Byte> > bytes;
    GetTbsCertificateInternal((ArrayOf<Byte>**)&bytes);
    *tbsCertificate = bytes->Clone();
    REFCOUNT_ADD(*tbsCertificate)
    return NOERROR;
}

ECode CX509CertImpl::GetTbsCertificateInternal(
    /* [out, callee] */ ArrayOf<Byte>** ret)
{
    AutoPtr<ArrayOf<Byte> > result = mTbsCertificate;
    if (result == NULL) {
        mTbsCert->GetEncoded((ArrayOf<Byte>**)&result);
        mTbsCertificate = result;
    }
    *ret = result;
    REFCOUNT_ADD(*ret)
    return NOERROR;
}

ECode CX509CertImpl::GetSignature(
    /* [out, callee] */ ArrayOf<Byte>** signature)
{
    VALIDATE_NOT_NULL(signature)
    AutoPtr<ArrayOf<Byte> > bytes;
    GetSignatureInternal((ArrayOf<Byte>**)&bytes);
    *signature = bytes->Clone();
    REFCOUNT_ADD(*signature)
    return NOERROR;
}

ECode CX509CertImpl::GetSignatureInternal(
    /* [out, callee] */ ArrayOf<Byte>** ret)
{
    VALIDATE_NOT_NULL(ret)
    AutoPtr<ArrayOf<Byte> > result = mSignature;
    if (result == NULL) {
        mCertificate->GetSignatureValue((ArrayOf<Byte>**)&result);
        mSignature = result;
    }
    *ret = result;
    REFCOUNT_ADD(*ret)
    return NOERROR;
}

ECode CX509CertImpl::GetSigAlgName(
    /* [out] */ String* sigAlgName)
{
    VALIDATE_NOT_NULL(sigAlgName)
    String result = mSigAlgName;
    if (result.IsNull()) {
        String sigAlgOIDLocal;
        GetSigAlgOID(&sigAlgOIDLocal);
        // retrieve the name of the signing algorithm
        AlgNameMapper::Map2AlgName(sigAlgOIDLocal, &result);
        if (result.IsNull()) {
            // if could not be found, use OID as a name
            result = sigAlgOIDLocal;
        }
        mSigAlgName = result;
    }
    *sigAlgName = result;
    return NOERROR;
}

ECode CX509CertImpl::GetSigAlgOID(
    /* [out] */ String* oid)
{
    VALIDATE_NOT_NULL(oid)
    String result = mSigAlgOID;
    if (result.IsNull()) {
        // if info was not retrieved (and cached), do it:
        AutoPtr<IAlgorithmIdentifier> ai;
        mTbsCert->GetSignature((IAlgorithmIdentifier**)&ai);
        ai->GetAlgorithm(&result);
        mSigAlgOID = result;
    }
    *oid = result;
    return NOERROR;
}

ECode CX509CertImpl::GetSigAlgParams(
    /* [out, callee] */ ArrayOf<Byte>** sigAlgParams)
{
    VALIDATE_NOT_NULL(sigAlgParams)
    if (mNullSigAlgParams) {
        return NOERROR;
    }
    AutoPtr<ArrayOf<Byte> > result = mSigAlgParams;
    if (result.IsNull()) {
        AutoPtr<IAlgorithmIdentifier> ai;
        mTbsCert->GetSignature((IAlgorithmIdentifier**)&ai);
        ai->GetParameters((ArrayOf<Byte>**)&result);
        if (result.IsNull()) {
            mNullSigAlgParams = TRUE;
            return NOERROR;
        }
        mSigAlgParams = result;
    }
    *sigAlgParams = result;
    REFCOUNT_ADD(*sigAlgParams)
    return NOERROR;
}

ECode CX509CertImpl::GetIssuerUniqueID(
    /* [out, callee] */ ArrayOf<Boolean>** issuerUniqueID)
{
    return mTbsCert->GetIssuerUniqueID(issuerUniqueID);
}

ECode CX509CertImpl::GetSubjectUniqueID(
    /* [out, callee] */ ArrayOf<Boolean>** subjectUniqueID)
{
    return mTbsCert->GetSubjectUniqueID(subjectUniqueID);
}

ECode CX509CertImpl::GetKeyUsage(
    /* [out, callee] */ ArrayOf<Boolean>** keyUsage)
{
    VALIDATE_NOT_NULL(keyUsage)
    *keyUsage = NULL;

    if (mExtensions == NULL) {
        return NOERROR;
    }
    return mExtensions->ValueOfKeyUsage(keyUsage);
}

ECode CX509CertImpl::GetExtendedKeyUsage(
    /* [out] */ IList** keyUsage)
{
    VALIDATE_NOT_NULL(keyUsage)
    *keyUsage = NULL;

    if (mExtensions == NULL) {
        return NOERROR;
    }
    ECode ec = mExtensions->ValueOfExtendedKeyUsage(keyUsage);
    if (E_IO_EXCEPTION == ec) {
        return E_CERTIFICATE_PARSING_EXCEPTION;
    }
    return ec;
}

ECode CX509CertImpl::GetBasicConstraints(
    /* [out] */ Int32* constraints)
{
    VALIDATE_NOT_NULL(constraints)
    if (mExtensions == NULL) {
        *constraints = Elastos::Core::Math::INT32_MAX_VALUE;
        return NOERROR;
    }
    return mExtensions->ValueOfBasicConstrains(constraints);
}

ECode CX509CertImpl::GetSubjectAlternativeNames(
    /* [out] */ ICollection** subjectAlternativeNames)
{
    VALIDATE_NOT_NULL(subjectAlternativeNames)
    *subjectAlternativeNames = NULL;

    if (mExtensions == NULL) {
        return NOERROR
    }
    ECode ec = mExtensions->ValueOfSubjectAlternativeName(subjectAlternativeNames);
    if (E_IO_EXCEPTION == ec) {
        return E_CERTIFICATE_PARSING_EXCEPTION;
    }
    return ec;
}

ECode CX509CertImpl::GetIssuerAlternativeNames(
    /* [out] */ ICollection** issuerAlternativeNames)
{
    VALIDATE_NOT_NULL(issuerAlternativeNames)
    *issuerAlternativeNames = NULL;

    if (mExtensions == NULL) {
        return NOERROR
    }
    ECode ec = mExtensions->valueOfIssuerAlternativeName(issuerAlternativeNames);
    if (E_IO_EXCEPTION == ec) {
        return E_CERTIFICATE_PARSING_EXCEPTION;
    }
    return ec;
}

ECode CX509CertImpl::GetCriticalExtensionOIDs(
    /* [out] */ ISet** oids)
{
    VALIDATE_NOT_NULL(oids)
    *oids = NULL;

    if (mExtensions == NULL) {
            return NOERROR;
    }
        // retrieve the info from the cached extensions object
    return mExtensions->GetCriticalExtensions(oids);
}

ECode CX509CertImpl::GetExtensionValue(
    /* [in] */ const String& oid,
    /* [out, callee] */ ArrayOf<Byte>** extensionValue)
{
    VALIDATE_NOT_NULL(extensionValue)
    *extensionValue = NULL;

    if (mExtensions == NULL) {
        return NOERROR;
    }
    // retrieve the info from the cached extensions object
    AutoPtr<IExtension> ext;
    mExtensions->GetExtensionByOID(oid, (IExtension**)&ext);
    if (ext == NULL) {
        return NOERROR;
    }
    return ext->GetRawExtnValue(extensionValue);
}

ECode CX509CertImpl::GetNonCriticalExtensionOIDs(
    /* [out] */ ISet** oids)
{
    VALIDATE_NOT_NULL(oids)
    *oids = NULL;

    if (mExtensions == NULL) {
        return NOERROR;
    }
    // retrieve the info from the cached extensions object
    return mExtensions->GetNonCriticalExtensions(oids);
}

ECode CX509CertImpl::HasUnsupportedCriticalExtension(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (mExtensions == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    // retrieve the info from the cached extensions object
    return mExtensions->HasUnsupportedCritical(result);
}

ECode CX509CertImpl::constructor(
    /* [in] */ IInputStream* is)
{
    // decode the Certificate object
    ECode ec;
    FAIL_GOTO(ec = CCertificate::ASN1->Decode(is, (ICertificate**)&mCertificate), ERROR_PROCESS)
    // cache the values of TBSCertificate and Extensions
    mCertificate->GetTbsCertificate((ITBSCertificate**)&mTbsCert);
    mTbsCert->GetExtensions((IExtensions**)&mExtensions);
ERROR_PROCESS:
    if (ec == (ECode)E_IO_EXCEPTION)
        return E_CERTIFICATE_EXCEPTION;
    return NOERROR;
}

ECode CX509CertImpl::constructor(
    /* [in] */ ICertificate* certificate)
{
    mCertificate = certificate;
    // cache the values of TBSCertificate and Extensions
    mCertificate->GetTbsCertificate((ITBSCertificate**)&mTbsCert);
    return mTbsCert->GetExtensions((IExtensions**)&mExtensions);
}

ECode CX509CertImpl::constructor(
    /* [in] */ ArrayOf<Byte>* encoding)
{
    //this((Certificate) Certificate.ASN1.decode(encoding));
    AutoPtr<IInterface> tmp;
    CCertificate::ASN1->Decode(encoding, (IInterface**)&tmp);
    return constructor(ICertificate::Probe(tmp));
}

} // namespace Cert
} // namespace Provider
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org
