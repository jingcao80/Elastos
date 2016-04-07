
#include "CX509CRLImpl.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Provider {
namespace Cert {


ECode CX509CRLImpl::Equals(
    /* [in] */ IInterface * pOther,
    /* [out] */ Boolean * pResult)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CX509CRLImpl::GetHashCode(
    /* [out] */ Int32 * pHashCode)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CX509CRLImpl::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** encoded)
{
    if (mEncoding == NULL) {
        mCrl->GetEncoded((ArrayOf<Byte>**)&mEncoding);
    }
    AutoPtr<ArrayOf<Byte> > result = ArrayOf<Byte>::Alloc(mEncoding->GetLength());
    result->Copy(0, mEncoding, 0, mEncoding->GetLength());
    *encoded = result;
    REFCOUNT_ADD(*encoded)
    return NOERROR;
}

ECode CX509CRLImpl::Verify(
    /* [in] */ IPublicKey* key)
{
    String sigAlgName;
    GetSigAlgName(&sigAlgName);
    AutoPtr<ISignatureHelper> helper;
    CSignatureHelper::AcquireSingleton((ISignatureHelper**)&helper);
    AutoPtr<ISignature> signature;
    helper->GetInstance(sigAlgName, (ISignature**)&signature);
    signature->InitVerify(key);
    AutoPtr<ArrayOf<Byte> > tbsEncoding;
    mTbsCertList->GetEncoded((ArrayOf<Byte>**)&tbsEncoding);
    signature->Update(tbsEncoding, 0, tbsEncoding->GetLength());
    AutoPtr<ArrayOf<Byte> > tmp;
    mCrl->GetSignatureValue((ArrayOf<Byte>**)&tmp);
    Boolean veri;
    if (signature->Verify(tmp, &veri), !veri) {
        //throw new SignatureException("Signature was not verified");
        return E_SIGNATURE_EXCEPTION;
    }
    return NOERROR;
}

ECode CX509CRLImpl::VerifyEx(
    /* [in] */ IPublicKey* key,
    /* [in] */ const String& sigProvider)
{
    AutoPtr<ISignatureHelper> helper;
    CSignatureHelper::AcquireSingleton((ISignatureHelper**)&helper);
    String sigAlgName;
    GetSigAlgName(&sigAlgName);
    AutoPtr<ISignature> signature;
    helper->GetInstanceEx(sigAlgName, sigProvider, (ISignature**)&signature);
    signature->InitVerify(key);
    AutoPtr<ArrayOf<Byte> > tbsEncoding;
    mTbsCertList->GetEncoded((ArrayOf<Byte>**)&tbsEncoding);
    signature->Update(tbsEncoding, 0, tbsEncoding->GetLength());
    AutoPtr<ArrayOf<Byte> > tmp;
    mCrl->GetSignatureValue((ArrayOf<Byte>**)&tmp);
    Boolean veri;
    if (signature->Verify(tmp, &veri), !veri) {
        //throw new SignatureException("Signature was not verified");
        return E_SIGNATURE_EXCEPTION;
    }
    return NOERROR;
}

ECode CX509CRLImpl::GetVersion(
    /* [out] */ Int32* vs)
{
    return mTbsCertList->GetVersion(vs);
}

ECode CX509CRLImpl::GetIssuerDN(
    /* [out] */ IPrincipal** issuer)
{
    REFCOUNT_ADD(*issuer)
    if (mIssuer == NULL) {
        AutoPtr<IName> name;
        AutoPtr<IX500Principal> ret;
        mTbsCertList->GetIssuerEx((IName**)&name);
        mIssuer = NULL;
        name->GetX500Principal((IX500Principal**)&mIssuer);
    }
    *issuer = mIssuer.Get();
    REFCOUNT_ADD(*issuer)
    return NOERROR;
}

ECode CX509CRLImpl::GetIssuerX500Principal(
    /* [out] */ IX500Principal** issuer)
{
    REFCOUNT_ADD(*issuer)
    if (mIssuer == NULL) {
        AutoPtr<IName> name;
        AutoPtr<IX500Principal> ret;
        mTbsCertList->GetIssuerEx((IName**)&name);
        mIssuer = NULL;
        name->GetX500Principal((IX500Principal**)&mIssuer);
    }
    *issuer = mIssuer.Get();
    REFCOUNT_ADD(*issuer)
    return NOERROR;
}

ECode CX509CRLImpl::GetThisUpdate(
    /* [out] */ IDate** thisUpdate)
{
    return mTbsCertList->GetThisUpdate(thisUpdate);
}

ECode CX509CRLImpl::GetNextUpdate(
    /* [out] */ IDate** nextUpdate)
{
    return mTbsCertList->GetNextUpdate(nextUpdate);
}

ECode CX509CRLImpl::RetrieveEntries()
{
    mEntriesRetrieved = TRUE;
    AutoPtr<IList> rcerts;
    mTbsCertList->GetRevokedCertificates((IList**)&rcerts);
    if (rcerts == NULL) {
        return NOERROR;
    }
    rcerts->GetSize(&mEntriesSize);
    CArrayList::New((IArrayList**)&mEntries);
    // null means that revoked certificate issuer is the same as CRL issuer
    AutoPtr<IX500Principal> rcertIssuer;
    for (Int32 i=0; i < mEntriesSize; i++) {
        AutoPtr<IInterface> tmp;
        rcerts->Get(i, (IInterface**)&tmp);
        AutoPtr<IRevokedCertificate> rcert = IRevokedCertificate::Probe(tmp);
        AutoPtr<IX500Principal> iss;
        rcert->GetIssuer((IX500Principal**)&iss);
        if (iss != NULL) {
            // certificate issuer differs from CRL issuer
            // and CRL is indirect.
            rcertIssuer = iss;
            mIsIndirectCRL = TRUE;
            // remember how many leading revoked certificates in the
            // list are issued by the same issuer as issuer of CRL
            // (these certificates are first in the list)
            mNonIndirectEntriesSize = i;
        }
        AutoPtr<IX509CRLEntry> crlEntry;
        CX509CRLEntryImpl::New(rcert.Get(), rcertIssuer.Get(), (IX509CRLEntry**)&crlEntry);
        mEntries->Add(crlEntry.Get());
    }
}

ECode CX509CRLImpl::GetRevokedCertificate(
    /* [in] */ IBigInteger* serialNumber,
    /* [out] */ IX509CRLEntry** crlEntry)
{
    VALIDATE_NOT_NULL(crlEntry)
    if (!mEntriesRetrieved) {
        RetrieveEntries();
    }
    if (mEntries == NULL) {
        return NOERROR;
    }
    Boolean isEqual;
    for (Int32 i = 0; i < mNonIndirectEntriesSize; i++) {
        AutoPtr<IInterface> tmp;
        mEntries->Get(i, (IInterface**)&tmp);
        AutoPtr<IX509CRLEntry> entry = IX509CRLEntry::Probe(tmp);
        AutoPtr<IBigInteger> sn;
        entry->GetSerialNumber((IBigInteger**)&sn);
        if (serialNumber->Equals(sn.Get(), &isEqual), isEqual) {
            *crlEntry = entry;
            REFCOUNT_ADD(*crlEntry)
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode CX509CRLImpl::GetRevokedCertificateEx(
    /* [in] */ IX509Certificate* certificate,
    /* [out] */ IX509CRLEntry** crlEntry)
{
    VALIDATE_NOT_NULL(crlEntry)
    if (certificate == NULL) {
        //throw new NullPointerException("certificate == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (!mEntriesRetrieved) {
        RetrieveEntries();
    }
    if (mEntries == NULL) {
        return NOERROR;
    }
    Boolean isEqual;
    AutoPtr<IBigInteger> serialN;
    certificate->GetSerialNumber((IBigInteger**)&serialN);
    if (mIsIndirectCRL) {
        // search in indirect crl
        AutoPtr<IX500Principal> certIssuer;
        certificate->GetIssuerX500Principal((IX500Principal**)&certIssuer);
        AutoPtr<IX500Principal> tmp;
        GetIssuerX500Principal((IX500Principal**)&tmp);
        if (certIssuer->Equals(tmp.Get(), &isEqual), isEqual) {
            // certificate issuer is CRL issuer
            certIssuer = NULL;
        }
        for (Int32 i = 0; i < mEntriesSize; i++) {
            AutoPtr<IInterface> tmp;
            mEntries->Get(i, (IInterface**)&tmp);
            AutoPtr<IX509CRLEntry> entry = IX509CRLEntry::Probe(tmp);
            // check the serial number of revoked certificate
            AutoPtr<IBigInteger> sn;
            entry->GetSerialNumber((IBigInteger**)&sn);
            if (serialN->Equals(sn.Get(), &isEqual), isEqual) {
                // revoked certificate issuer
                AutoPtr<IX500Principal> iss;
                entry->GetCertificateIssuer((IX500Principal**)&iss);
                // check the issuer of revoked certificate
                if (certIssuer != NULL) {
                    // certificate issuer is not a CRL issuer, so
                    // check issuers for equality
                    if (certIssuer->Equals(iss.Get(), &isEqual), isEqual) {
                        *crlEntry = entry;
                        REFCOUNT_ADD(*crlEntry)
                        return NOERROR;
                    }
                } else if (iss == NULL) {
                    // both certificates was issued by CRL issuer
                    *crlEntry = entry;
                    REFCOUNT_ADD(*crlEntry)
                    return NOERROR;
                }
            }
        }
    } else {
        // search in CA's (non indirect) crl: just look up the serial number
        for (Int32 i=0; i< mEntriesSize; i++) {
            AutoPtr<IInterface> tmp;
            mEntries->Get(i, (IInterface**)&tmp);
            AutoPtr<IX509CRLEntry> entry = IX509CRLEntry::Probe(tmp);
            AutoPtr<IBigInteger> sn;
            entry->GetSerialNumber((IBigInteger**)&sn);
            if (serialN->Equals(sn.Get(), &isEqual), isEqual) {
                *crlEntry = entry;
                REFCOUNT_ADD(*crlEntry)
                return NOERROR;
            }
        }
    }
    return NOERROR;
}

ECode CX509CRLImpl::GetRevokedCertificates(
    /* [out] */ ISet** crlEntrys)
{
    if (!mEntriesRetrieved) {
        RetrieveEntries();
    }
    if (mEntries == NULL) {
        return NOERROR;
    }
    return CHashSet::New(mEntries.Get(), crlEntrys);
}

ECode CX509CRLImpl::GetTBSCertList(
    /* [out, callee] */ ArrayOf<Byte>** tbsCertList)
{
    VALIDATE_NOT_NULL(tbsCertList)
    if (mTbsCertListEncoding == NULL) {
        mTbsCertList->GetEncoded((ArrayOf<Byte>**)&mTbsCertListEncoding);
    }
    AutoPtr<ArrayOf<Byte> > result = ArrayOf<Byte>::Alloc(mTbsCertListEncoding->GetLength());
    result->Copy(0, mTbsCertListEncoding, 0, mTbsCertListEncoding->GetLength());
    *tbsCertList = result;
    REFCOUNT_ADD(*tbsCertList)
    return NOERROR;
}

ECode CX509CRLImpl::GetSignature(
    /* [out, callee] */ ArrayOf<Byte>** signature)
{
    VALIDATE_NOT_NULL(signature)
    if (mSignature == NULL) {
        mCrl->GetSignatureValue((ArrayOf<Byte>**)&mSignature);
    }
    AutoPtr<ArrayOf<Byte> > result = ArrayOf<Byte>::Alloc(mSignature->GetLength());
    result->Copy(0, mSignature, 0, mSignature->GetLength());
    *signature = result;
    REFCOUNT_ADD(*signature)
    return NOERROR;
}

ECode CX509CRLImpl::GetSigAlgName(
    /* [out] */ String* sigAlgName)
{
    VALIDATE_NOT_NULL(sigAlgName)
    if (mSigAlgOID.IsNull()) {
        AutoPtr<IAlgorithmIdentifier> sig;
        mTbsCertList->GetSignature((IAlgorithmIdentifier**)&sig);
        sig->GetAlgorithm(&mSigAlgOID);
        AutoPtr<IAlgNameMapperHelper> helper;
        CAlgNameMapperHelper::AcquireSingleton((IAlgNameMapperHelper**)&helper);
        helper->Map2AlgName(mSigAlgOID, &mSigAlgName);
        if (mSigAlgName.IsNull()) {
            mSigAlgName = mSigAlgOID;
        }
    }
    *sigAlgName = mSigAlgName;
    return NOERROR;
}

ECode CX509CRLImpl::GetSigAlgOID(
    /* [out] */ String* sigAlgOid)
{
    VALIDATE_NOT_NULL(sigAlgOid)
    if (mSigAlgOID.IsNull()) {
        AutoPtr<IAlgorithmIdentifier> sig;
        mTbsCertList->GetSignature((IAlgorithmIdentifier**)&sig);
        sig->GetAlgorithm(&mSigAlgOID);
        AutoPtr<IAlgNameMapperHelper> helper;
        CAlgNameMapperHelper::AcquireSingleton((IAlgNameMapperHelper**)&helper);
        helper->Map2AlgName(mSigAlgOID, &mSigAlgName);
        if (mSigAlgName.IsNull()) {
            mSigAlgName = mSigAlgOID;
        }
    }
    *sigAlgOid = mSigAlgOID;
    return NOERROR;
}

ECode CX509CRLImpl::GetSigAlgParams(
    /* [out, callee] */ ArrayOf<Byte>** sigAlgParams)
{
    VALIDATE_NOT_NULL(sigAlgParams)
    if (mNullSigAlgParams) {
        return NOERROR;
    }
    if (mSigAlgParams == NULL) {
        AutoPtr<IAlgorithmIdentifier> sig;
        mTbsCertList->GetSignature((IAlgorithmIdentifier**)&sig);
        sig->GetParameters((ArrayOf<Byte>**)&mSigAlgParams);
        if (mSigAlgParams == NULL) {
            mNullSigAlgParams = TRUE;
            return NOERROR;
        }
    }
    *sigAlgParams = mSigAlgParams;
    REFCOUNT_ADD(*sigAlgParams)
    return NOERROR;
}

ECode CX509CRLImpl::IsRevoked(
    /* [in] */ ICertificate* cert,
    /* [out] */ Boolean* isRevoked)
{
    VALIDATE_NOT_NULL(isRevoked)
    if (NULL == cert->Probe(EIID_X509Certificate)) {
        *isRevoked = FALSE;
        return NOERROR;
    }
    AutoPtr<IX509CRLEntry> crlEntry;
    GetRevokedCertificateEx(cert, (IX509CRLEntry**)&crlEntry);
    *isRevoked = crlEntry != NULL;
    return NOERROR;
}

ECode CX509CRLImpl::ToString(
    /* [out] */ String* str)
{
    return mCrl->ToString(str);
}

ECode CX509CRLImpl::GetNonCriticalExtensionOIDs(
    /* [out] */ ISet** oids)
{
    if (mExtensions == NULL) {
        return NOERROR;
    }
    return mExtensions->GetNonCriticalExtensions(oids);
}

ECode CX509CRLImpl::GetCriticalExtensionOIDs(
    /* [out] */ ISet** oids)
{
    if (mExtensions == NULL) {
        return NOERROR;
    }
    return mExtensions->GetCriticalExtensions(oids);
}

ECode CX509CRLImpl::GetExtensionValue(
    /* [in] */ const String& oid,
    /* [out, callee] */ ArrayOf<Byte>** value)
{
    if (mExtensions == NULL) {
        return NOERROR;
    }
    AutoPtr<IExtension> ext;
    mExtensions->GetExtensionByOID(oid, (IExtension**)&ext);
    if (ext == NULL) {
        return NOERROR;
    }
    return ext->GetRawExtnValue(value);
}

ECode CX509CRLImpl::HasUnsupportedCriticalExtension(
    /* [out] */ Boolean* hasExtensions)
{
    VALIDATE_NOT_NULL(hasExtensions)
    if (mExtensions == NULL) {
        *hasExtensions = FALSE;
        return NOERROR;
    }
    return mExtensions->HasUnsupportedCritical(hasExtensions);
}

ECode CX509CRLImpl::constructor(
    /* [in] */ ICertificateList* crl)
{
    mCrl = crl;
    crl->GetTbsCertList((ITBSCertList**)&mTbsCertList);
    mTbsCertList.GetCrlExtensions((IExtensions**)&mExtensions);
}

ECode CX509CRLImpl::constructor(
    /* [in] */ IInputStream* is)
{
    // decode CertificateList structure
    AutoPtr<IInterface> tmp;
    CCertificateList::ASN1->DecodeEx2(is, (IInterface**)&tmp);
    mCrl = ICertificateList::Probe(tmp);
    mCrl->GetTbsCertList((ITBSCertList**)&mTbsCertList);
    return mTbsCertList->GetCrlExtensions((IExtensions**)&mExtensions);
}

ECode CX509CRLImpl::constructor(
    /* [in] */ ArrayOf<Byte>* encoding)
{
    this((CertificateList) CertificateList.ASN1.decode(encoding));
    AutoPtr<IInterface> tmp;
    CCertificateList::ASN1->Decode(encoding, (IInterface**)&tmp);
    return constructor(ICertificateList::Probe(tmp));
}

} // namespace Cert
} // namespace Provider
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

