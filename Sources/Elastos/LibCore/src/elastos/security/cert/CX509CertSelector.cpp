
#include "CX509CertSelector.h"
#include "StringUtils.h"
#include "CX500Principal.h"
#include "CHashSet.h"
#include "CBoolean.h"

using Elastos::Core::IBoolean;
using Elastos::Core::ICharSequence;
using Elastos::Core::CBoolean;
using Elastos::Core::StringUtils;
using Elastos::Core::ICloneable;

using Elastosx::Security::Auth::X500::CX500Principal;
using Elastos::Utility::IIterator;
using Elastos::Utility::CHashSet;

namespace Elastos {
namespace Security {
namespace Cert {

CAR_OBJECT_IMPL(CX509CertSelector)
CAR_INTERFACE_IMPL_3(CX509CertSelector, Object, IX509CertSelector, ICertSelector,ICloneable)
CX509CertSelector::CX509CertSelector()
    : mMatchAllNames(TRUE)
    , mPathLen(-1)
{}

ECode CX509CertSelector::SetCertificate(
    /* [in] */ IX509Certificate *certificate)
{
    mCertificateEquals = certificate;
    return NOERROR;
}

ECode CX509CertSelector::GetCertificate(
    /* [out] */ IX509Certificate **certificate)
{
    VALIDATE_NOT_NULL(certificate)
    *certificate = mCertificateEquals;
    REFCOUNT_ADD(*certificate)
    return NOERROR;
}

ECode CX509CertSelector::SetSerialNumber(
    /* [in] */ IBigInteger *serialNumber)
{
    mSerialNumber = serialNumber;
    return NOERROR;
}

ECode CX509CertSelector::GetSerialNumber(
    /* [out] */ IBigInteger **serialNumber)
{
    VALIDATE_NOT_NULL(serialNumber)
    *serialNumber = mSerialNumber;
    REFCOUNT_ADD(*serialNumber)
    return NOERROR;
}

ECode CX509CertSelector::SetIssuer(
    /* [in] */ IX500Principal *issuer)
{
    mIssuer = issuer;
    mIssuerName = String();
    mIssuerBytes = NULL;
    return NOERROR;
}

ECode CX509CertSelector::GetIssuer(
    /* [out] */ IX500Principal **issuer)
{
    VALIDATE_NOT_NULL(issuer)
    *issuer = mIssuer;
    REFCOUNT_ADD(*issuer)
    return NOERROR;
}

ECode CX509CertSelector::SetIssuerUsingString(
    /* [in] */ const String& issuerName)
{
    if (issuerName.IsNull()) {
        mIssuer = NULL;
        mIssuerName = String();
        mIssuerBytes = NULL;
        return NOERROR;
    }
    mIssuer = NULL;
    ECode ec;
    FAIL_GOTO(ec = CX500Principal::New(issuerName, (IX500Principal**)&mIssuer), ERROR_PROCESS)
    mIssuerName = issuerName;
    mIssuerBytes = NULL;
ERROR_PROCESS:
    if (ec == (ECode)E_ILLEGAL_ARGUMENT_EXCEPTION) {
        return E_IO_EXCEPTION;
    }
    return ec;
}

ECode CX509CertSelector::GetIssuerAsString(
    /* [out] */ String *issuer)
{
    VALIDATE_NOT_NULL(issuer)
    if (mIssuer == NULL) {
        *issuer = String();
    }
    if (mIssuerName.IsNull()) {
        mIssuer->GetName(&mIssuerName);
    }
    *issuer = mIssuerName;
    return NOERROR;
}

ECode CX509CertSelector::SetIssuerUsingBytes(
    /* [in] */ ArrayOf<Byte> *issuerDN)
{
    if (issuerDN == NULL) {
        mIssuer = NULL;
        return NOERROR;
    }
    ECode ec;
    mIssuer = NULL;
    FAIL_GOTO(ec = CX500Principal::New(issuerDN, (IX500Principal**)&mIssuer), ERROR_PROCESS)
    mIssuerName = String();
    mIssuerBytes = ArrayOf<Byte>::Alloc(issuerDN->GetLength());
    FAIL_GOTO(ec = mIssuerBytes->Copy(0, issuerDN, 0, issuerDN->GetLength()), ERROR_PROCESS)
ERROR_PROCESS:
    if (ec == (ECode)E_ILLEGAL_ARGUMENT_EXCEPTION) {
        return E_IO_EXCEPTION;
    }
    return ec;
}

ECode CX509CertSelector::GetIssuerAsBytes(
    /* [out, callee] */ ArrayOf<Byte> **issuerDN)
{
    VALIDATE_NOT_NULL(issuerDN)
    if (mIssuer == NULL) {
        return NOERROR;
    }
    if (mIssuerBytes == NULL) {
        mIssuer->GetEncoded((ArrayOf<Byte>**)&mIssuerBytes);
    }
    AutoPtr<ArrayOf<Byte> > result = ArrayOf<Byte>::Alloc(mIssuerBytes->GetLength());
    result->Copy(0, mIssuerBytes, 0, mIssuerBytes->GetLength());
    *issuerDN = result;
    REFCOUNT_ADD(*issuerDN)
    return NOERROR;
}

ECode CX509CertSelector::SetSubject(
    /* [in] */ IX500Principal *subject)
{
    mSubject = subject;
    return NOERROR;
}

ECode CX509CertSelector::GetSubject(
    /* [out] */ IX500Principal **subject)
{
    VALIDATE_NOT_NULL(subject)
    *subject = mSubject;
    REFCOUNT_ADD(*subject)
    return NOERROR;
}

ECode CX509CertSelector::SetSubjectUsingString(
    /* [in] */ const String& subjectDN)
{
    if (subjectDN.IsNull()) {
        mSubject = NULL;
        return NOERROR;
    }
    mSubject = NULL;
    ECode ec;
    FAIL_GOTO(ec = CX500Principal::New(subjectDN, (IX500Principal**)&mSubject), ERROR_PROCESS)
ERROR_PROCESS:
    if (ec == (ECode)E_ILLEGAL_ARGUMENT_EXCEPTION) {
        return E_IO_EXCEPTION;
    }
    return ec;
}

ECode CX509CertSelector::GetSubjectAsString(
    /* [out] */ String *subjectDN)
{
    VALIDATE_NOT_NULL(subjectDN)
    if (mSubject == NULL) {
        *subjectDN = String();
        return NOERROR;
    }
    return mSubject->GetName(subjectDN);
}

ECode CX509CertSelector::SetSubjectUsingBytes(
    /* [in] */ ArrayOf<Byte> *subjectDN)
{
    if (subjectDN == NULL) {
        mSubject = NULL;
        return NOERROR;
    }
    mSubject = NULL;
    ECode ec = CX500Principal::New(subjectDN, (IX500Principal**)&mSubject);
    if (ec == (ECode)E_ILLEGAL_ARGUMENT_EXCEPTION) {
        return E_IO_EXCEPTION;
    }
    return ec;
}

ECode CX509CertSelector::GetSubjectAsBytes(
    /* [out, callee] */ ArrayOf<Byte> **subjectDN)
{
    if (mSubject == NULL) {
        return NOERROR;
    }
    return mSubject->GetEncoded(subjectDN);
}

ECode CX509CertSelector::SetSubjectKeyIdentifier(
    /* [in] */ ArrayOf<Byte> *subjectKeyIdentifier)
{
    if (subjectKeyIdentifier == NULL) {
        mSubjectKeyIdentifier = NULL;
        return NOERROR;
    }
    mSubjectKeyIdentifier = ArrayOf<Byte>::Alloc(subjectKeyIdentifier->GetLength());
    return mSubjectKeyIdentifier->Copy(0, subjectKeyIdentifier, 0, subjectKeyIdentifier->GetLength());
}

ECode CX509CertSelector::GetSubjectKeyIdentifier(
    /* [out, callee] */ ArrayOf<Byte> **subjectKeyIdentifier)
{
    VALIDATE_NOT_NULL(subjectKeyIdentifier)
    if (mSubjectKeyIdentifier == NULL) {
        *subjectKeyIdentifier = NULL;
        return NOERROR;
    }
    AutoPtr<ArrayOf<Byte> > res = ArrayOf<Byte>::Alloc(mSubjectKeyIdentifier->GetLength());
    res->Copy(0, mSubjectKeyIdentifier, 0, res->GetLength());
    *subjectKeyIdentifier = res;
    REFCOUNT_ADD(*subjectKeyIdentifier)
    return NOERROR;
}

ECode CX509CertSelector::SetAuthorityKeyIdentifier(
    /* [in] */ ArrayOf<Byte> *authorityKeyIdentifier)
{
    if (authorityKeyIdentifier == NULL) {
        mAuthorityKeyIdentifier = NULL;
        return NOERROR;
    }
    mAuthorityKeyIdentifier = ArrayOf<Byte>::Alloc(authorityKeyIdentifier->GetLength());
    return mAuthorityKeyIdentifier->Copy(0, authorityKeyIdentifier, 0, authorityKeyIdentifier->GetLength());
}

ECode CX509CertSelector::GetAuthorityKeyIdentifier(
    /* [out, callee] */ ArrayOf<Byte> **authorityKeyIdentifier)
{
    VALIDATE_NOT_NULL(authorityKeyIdentifier)
    if (mAuthorityKeyIdentifier == NULL) {
        return NOERROR;
    }
    AutoPtr<ArrayOf<Byte> > res = ArrayOf<Byte>::Alloc(mAuthorityKeyIdentifier->GetLength());
    res->Copy(0, mAuthorityKeyIdentifier, 0, res->GetLength());
    *authorityKeyIdentifier = res;
    REFCOUNT_ADD(*authorityKeyIdentifier);
    return NOERROR;
}

ECode CX509CertSelector::SetCertificateValid(
    /* [in] */ IDate *certificateValid)
{
    if (certificateValid == NULL) {
        mCertificateValid = NULL;
    }
    else {
        AutoPtr<IInterface> clobj;
        ICloneable::Probe(certificateValid)->Clone((IInterface**)&clobj);
        mCertificateValid = IDate::Probe(clobj);
    }
    return NOERROR;
}

ECode CX509CertSelector::GetCertificateValid(
    /* [out] */ IDate **certificateValid)
{
    VALIDATE_NOT_NULL(certificateValid)
    if (mCertificateValid != NULL){
        AutoPtr<IInterface> clobj;
        ICloneable::Probe(mCertificateValid)->Clone((IInterface**)&clobj);
        *certificateValid = IDate::Probe(clobj.Get());
        REFCOUNT_ADD(*certificateValid)
    }
    return NOERROR;
}

ECode CX509CertSelector::SetPrivateKeyValid(
    /* [in] */ IDate *privateKeyValid)
{
    if (privateKeyValid == NULL) {
        mPrivateKeyValid = NULL;
        return NOERROR;
    }
    AutoPtr<IInterface> clobj;
    ICloneable::Probe(privateKeyValid)->Clone((IInterface**)&clobj);
    mPrivateKeyValid = IDate::Probe(clobj);
    return NOERROR;
}

ECode CX509CertSelector::GetPrivateKeyValid(
    /* [out] */ IDate **privateKeyValid)
{
    VALIDATE_NOT_NULL(privateKeyValid)
    if (mPrivateKeyValid != NULL) {
        AutoPtr<IInterface> clobj;
        ICloneable::Probe(mPrivateKeyValid)->Clone((IInterface**)&clobj);
        *privateKeyValid = IDate::Probe(clobj.Get());
        REFCOUNT_ADD(*privateKeyValid)
    }
    return NOERROR;
}

ECode CX509CertSelector::CheckOID(
        /* [in] */ const String& oid)
{
    Int32 beg = 0;
    Int32 end = oid.IndexOf('.', beg);
    Int32 comp;
    FAIL_RETURN(StringUtils::ParseInt32(oid.Substring(beg, end), &comp))
    beg = end + 1;
    if (comp < 0 || comp > 2) {
        return E_IO_EXCEPTION;
    }
    end = oid.IndexOf('.', beg);
    FAIL_RETURN(StringUtils::ParseInt32(oid.Substring(beg, end), &comp))
    if (comp < 0 || comp > 39) {
        return E_IO_EXCEPTION;
    }
    return NOERROR;
}

ECode CX509CertSelector::SetSubjectPublicKeyAlgID(
    /* [in] */ const String& oid)
{
    if (oid.IsNull()) {
        mSubjectPublicKeyAlgID = String();
        return NOERROR;
    }
    FAIL_RETURN(CheckOID(oid))
    mSubjectPublicKeyAlgID = oid;
    return NOERROR;
}

ECode CX509CertSelector::GetSubjectPublicKeyAlgID(
    /* [out] */ String *oid)
{
    VALIDATE_NOT_NULL(oid)
    *oid = mSubjectPublicKeyAlgID;
    return NOERROR;
}

ECode CX509CertSelector::SetSubjectPublicKey(
    /* [in] */ IPublicKey *key)
{
    if (key == NULL) {
        mSubjectPublicKey = NULL;
    }
    else {
        mSubjectPublicKey = NULL;
        key->GetEncoded((ArrayOf<Byte>**)&mSubjectPublicKey);
    }
    mSubjectPublicKeyImpl = key;
    return NOERROR;
}

ECode CX509CertSelector::SetSubjectPublicKeyEx(
    /* [in] */ ArrayOf<Byte> *key)
{
    if (key == NULL) {
        mSubjectPublicKey = NULL;
        mSubjectPublicKeyImpl = NULL;
        return NOERROR;
    }
    mSubjectPublicKey = ArrayOf<Byte>::Alloc(key->GetLength());
    mSubjectPublicKey->Copy(0, key, 0, key->GetLength());
    //Apache...Todo later
    /*
    subjectPublicKeyImpl =
        ((SubjectPublicKeyInfo) SubjectPublicKeyInfo.ASN1.decode(key))
        .getPublicKey();
    */
    return E_NOT_IMPLEMENTED;
}

ECode CX509CertSelector::GetSubjectPublicKey(
    /* [out] */ IPublicKey **key)
{
    VALIDATE_NOT_NULL(key)
    *key = mSubjectPublicKeyImpl;
    REFCOUNT_ADD(*key)
    return NOERROR;
}

ECode CX509CertSelector::SetKeyUsage(
    /* [in] */ ArrayOf<Boolean> *keyUsage)
{
    if (keyUsage == NULL) {
        mKeyUsage = NULL;
        return NOERROR;
    }
    mKeyUsage = ArrayOf<Boolean>::Alloc(keyUsage->GetLength());
    return mKeyUsage->Copy(0, keyUsage, 0, keyUsage->GetLength());
}

ECode CX509CertSelector::GetKeyUsage(
    /* [out, callee] */ ArrayOf<Boolean> **keyUsage)
{
    VALIDATE_NOT_NULL(keyUsage)
    if (mKeyUsage != NULL) {
        AutoPtr<ArrayOf<Boolean> > result = ArrayOf<Boolean>::Alloc(mKeyUsage->GetLength());
        result->Copy(0, mKeyUsage, 0, mKeyUsage->GetLength());
        *keyUsage = result;
        REFCOUNT_ADD(*keyUsage)
    }
    return NOERROR;
}

ECode CX509CertSelector::SetExtendedKeyUsage(
    /* [in] */ ISet *keyUsage)
{
    mExtendedKeyUsage = NULL;
    if (mKeyUsage == NULL || mKeyUsage->GetLength() == 0) {
        return NOERROR;
    }
    AutoPtr<ISet> key_u;
    CHashSet::New((ISet**)&key_u);
    Boolean hasNext;
    AutoPtr<IIterator> it;
    keyUsage->GetIterator((IIterator**)&it);
    while ((it->HasNext(&hasNext), hasNext)) {
        AutoPtr<IInterface> elem;
        it->GetNext((IInterface**)&elem);
        String usage;
        ICharSequence::Probe(elem)->ToString(&usage);
        FAIL_RETURN(CheckOID(usage))
        key_u->Add(elem, &hasNext);
    }
    //Todo
    //extendedKeyUsage = Collections.unmodifiableSet(key_u);
    return E_NOT_IMPLEMENTED;
}

ECode CX509CertSelector::GetExtendedKeyUsage(
    /* [out] */ ISet **keyUsage)
{
    VALIDATE_NOT_NULL(keyUsage)
    //*keyUsage = mKeyUsage;
    Boolean assist;
    for (Int32 i = 0; i < mKeyUsage->GetLength(); i++) {
        AutoPtr<IBoolean> elem;
        CBoolean::New((*mKeyUsage)[i], (IBoolean**)&elem);
        (*keyUsage)->Add(elem.Get(), &assist);
    }

    REFCOUNT_ADD(*keyUsage)
    return NOERROR;
}

ECode CX509CertSelector::SetMatchAllSubjectAltNames(
    /* [in] */ Boolean matchAllNames)
{
    mMatchAllNames = matchAllNames;
    return NOERROR;
}

ECode CX509CertSelector::GetMatchAllSubjectAltNames(
    /* [out] */ Boolean *matchAllNames)
{
    VALIDATE_NOT_NULL(matchAllNames)
    *matchAllNames = mMatchAllNames;
    return NOERROR;
}

ECode CX509CertSelector::SetSubjectAlternativeNames(
    /* [in] */ ICollection *names)
{
    //Apache...Todo later
    /*
        subjectAltNames = null;
        if ((names == null) || (names.size() == 0)) {
            return;
        }
        for (List<?> name : names) {
            int tag = (Integer) name.get(0);
            Object value = name.get(1);
            if (value instanceof String) {
                addSubjectAlternativeName(tag, (String) value);
            } else if (value instanceof byte[]) {
                addSubjectAlternativeName(tag, (byte[]) value);
            } else {
                throw new IOException("name neither a String nor a byte[]");
            }
        }
    */
    return E_NOT_IMPLEMENTED;
}

ECode CX509CertSelector::AddSubjectAlternativeName(
    /* [in] */ Int32 tag,
    /* [in] */ const String& name)
{
    //Apache...Todo later
    /*
        GeneralName alt_name = new GeneralName(tag, name);
        // create only if there was not any errors
        if (subjectAltNames == null) {
            subjectAltNames = new ArrayList[9];
        }
        if (subjectAltNames[tag] == null) {
            subjectAltNames[tag] = new ArrayList<GeneralName>();
        }
        subjectAltNames[tag].add(alt_name);
    */
    return E_NOT_IMPLEMENTED;
}

ECode CX509CertSelector::AddSubjectAlternativeNameEx(
    /* [in] */ Int32 tag,
    /* [in] */ ArrayOf<Byte> *name)
{
    //Apache...Todo later
    /*
        GeneralName alt_name = new GeneralName(tag, name);
        // create only if there was not any errors
        if (subjectAltNames == null) {
            subjectAltNames = new ArrayList[9];
        }
        if (subjectAltNames[tag] == null) {
            subjectAltNames[tag] = new ArrayList<GeneralName>();
        }
        subjectAltNames[tag].add(alt_name);
    */
    return E_NOT_IMPLEMENTED;
}

ECode CX509CertSelector::GetSubjectAlternativeNames(
    /* [out] */ ICollection **names)
{
    //Apache...Todo later
    /*
        if (subjectAltNames == null) {
            return null;
        }
        ArrayList<List<?>> result = new ArrayList<List<?>>();
        for (int tag=0; tag<9; tag++) {
            if (subjectAltNames[tag] != null) {
                for (int name=0; name<subjectAltNames[tag].size(); name++) {
                    List<Object> list = new ArrayList<Object>(2);
                    list.add(tag);
                    list.add(subjectAltNames[tag].get(name));
                    result.add(list);
                }
            }
        }
        return result;
    */
    return E_NOT_IMPLEMENTED;
}

ECode CX509CertSelector::SetNameConstraints(
    /* [in] */ ArrayOf<Byte> *bytes)
{
    //Apache...Todo later
    /*
        this.nameConstraints = (bytes == null)
        ? null
        : (NameConstraints) NameConstraints.ASN1.decode(bytes);
    */
    return E_NOT_IMPLEMENTED;
}

ECode CX509CertSelector::GetNameConstraints(
    /* [out, callee] */ ArrayOf<Byte> **bytes)
{
    //Apache...Todo later
    /*
        return (nameConstraints == null)
        ? null
        : nameConstraints.getEncoded();
    */
    return E_NOT_IMPLEMENTED;
}

ECode CX509CertSelector::SetBasicConstraints(
    /* [in] */ Int32 pathLen)
{
    if (pathLen < -2) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mPathLen = pathLen;
    return NOERROR;
}

ECode CX509CertSelector::GetBasicConstraints(
    /* [out] */ Int32 *pathLen)
{
    VALIDATE_NOT_NULL(pathLen)
    *pathLen = mPathLen;
    return NOERROR;
}

ECode CX509CertSelector::SetPolicy(
    /* [in] */ ISet *policies)
{
    if (policies == NULL) {
        mPolicies = NULL;
        return NOERROR;
    }
    Int32 size;
    policies->GetSize(&size);
    AutoPtr<ISet> pols;
    CHashSet::New(size, (ISet**)&pols);
    Boolean hasNext;
    AutoPtr<IIterator> it;
    policies->GetIterator((IIterator**)&it);
    while ((it->HasNext(&hasNext), hasNext)) {
        AutoPtr<IInterface> elem;
        it->GetNext((IInterface**)&elem);
        String certPolicyId;
        ICharSequence::Probe(elem)->ToString(&certPolicyId);
        FAIL_RETURN(CheckOID(certPolicyId))
        pols->Add(elem, &hasNext);
    }
    //this.policies = Collections.unmodifiableSet(pols);
    return E_NOT_IMPLEMENTED;
}

ECode CX509CertSelector::GetPolicy(
    /* [out] */ ISet **policies)
{
    VALIDATE_NOT_NULL(policies)
    *policies = mPolicies;
    REFCOUNT_ADD(*policies)
    return NOERROR;
}

ECode CX509CertSelector::AddPathToName(
    /* [in] */ Int32 type,
    /* [in] */ const String& name)
{
    //Apache...Todo later
    /*
        GeneralName path_name = new GeneralName(type, name);
        // create only if there was not any errors
        if (pathToNames == null) {
            pathToNames = new ArrayList<GeneralName>();
        }
        pathToNames.add(path_name);
    */
    return E_NOT_IMPLEMENTED;
}

ECode CX509CertSelector::SetPathToNames(
    /* [in] */ ICollection *names)
{
    //Apache...Todo later
    /*
        pathToNames = null;
        if ((names == null) || (names.size() == 0)) {
            return;
        }
        for (List<?> name : names) {
            int tag = (Integer) name.get(0);
            Object value = name.get(1);
            if (value instanceof String) {
                addPathToName(tag, (String) value);
            } else if (value instanceof byte[]) {
                addPathToName(tag, (byte[]) value);
            } else {
                throw new IOException("name neither a String nor a byte[]");
            }
        }
    */
    return E_NOT_IMPLEMENTED;
}

ECode CX509CertSelector::AddPathToNameEx(
    /* [in] */ Int32 type,
    /* [in] */ ArrayOf<Byte> *name)
{
    //Apache...Todo later
    /*
        GeneralName path_name= new GeneralName(type, name);
        // create only if there was not any errors
        if (pathToNames == null) {
            pathToNames = new ArrayList<GeneralName>();
        }
        pathToNames.add(path_name);
    */
    return E_NOT_IMPLEMENTED;
}

ECode CX509CertSelector::GetPathToNames(
    /* [out] */ ICollection **name)
{
    //Apache...Todo later
    /*
        if (pathToNames == null) {
            return null;
        }
        Collection<List<?>> result = new ArrayList<List<?>>();
        for (GeneralName name : pathToNames) {
            result.add(name.getAsList());
        }
        return result;
    */
    return E_NOT_IMPLEMENTED;
}

ECode CX509CertSelector::ToString(
    /* [out] */ String *str)
{
    //Apache...Todo later
    /*
        // For convenient reading of the string representation
        // all of the fields named according to the rfc 3280
        // (http://www.ietf.org/rfc/rfc3280.txt).

        StringBuilder result = new StringBuilder();
        result.append("X509CertSelector: \n[");
        if (this.certificateEquals != null) {
            result.append("\n  certificateEquals: ").append(certificateEquals);
        }
        if (this.serialNumber != null) {
            result.append("\n  serialNumber: ").append(serialNumber);
        }
        if (this.issuer != null) {
            result.append("\n  issuer: ").append(issuer);
        }
        if (this.subject != null) {
            result.append("\n  subject: ").append(subject);
        }
        if (this.subjectKeyIdentifier != null) {
            result.append("\n  subjectKeyIdentifier: ")
                    .append(Array.getBytesAsString(subjectKeyIdentifier));
        }
        if (this.authorityKeyIdentifier != null) {
            result.append("\n  authorityKeyIdentifier: ")
                    .append(Array.getBytesAsString(authorityKeyIdentifier));
        }
        if (this.certificateValid != null) {
            result.append("\n  certificateValid: ").append(certificateValid);
        }
        if (this.subjectPublicKeyAlgID != null) {
            result.append("\n  subjectPublicKeyAlgID: ").append(subjectPublicKeyAlgID);
        }
        if (this.privateKeyValid != null) {
            result.append("\n  privateKeyValid: ").append(privateKeyValid);
        }
        if (this.subjectPublicKey != null) {
            result.append("\n  subjectPublicKey: ")
                    .append(Array.getBytesAsString(subjectPublicKey));
        }
        if (this.keyUsage != null) {
            result.append("\n  keyUsage: \n  [");
            String[] kuNames = new String[] {
                "digitalSignature", "nonRepudiation", "keyEncipherment",
                "dataEncipherment", "keyAgreement", "keyCertSign", "cRLSign",
                "encipherOnly", "decipherOnly"
            };
            for (int i=0; i<9; i++) {
                if (keyUsage[i]) {
                    result.append("\n    ").append(kuNames[i]);
                }
            }
            result.append("\n  ]");
        }
        if (this.extendedKeyUsage != null) {
            result.append("\n  extendedKeyUsage: ").append(extendedKeyUsage.toString());
        }
        result.append("\n  matchAllNames: ").append(matchAllNames);
        result.append("\n  pathLen: ").append(pathLen);
        if (this.subjectAltNames != null) {
            result.append("\n  subjectAltNames:  \n  [");
            for (int i=0; i<9; i++) {
                List<GeneralName> names = subjectAltNames[i];
                if (names != null) {
                    int size = names.size();
                    for (GeneralName generalName : names) {
                        result.append("\n    ").append(generalName.toString());
                    }
                }
            }
            result.append("\n  ]");
        }
        if (this.nameConstraints != null) {
        }
        if (this.policies != null) {
            result.append("\n  policies: ").append(policies.toString());
        }
        if (this.pathToNames != null) {
            result.append("\n  pathToNames:  \n  [");
            for (GeneralName generalName : pathToNames) {
                result.append("\n    ").append(generalName.toString());
            }
        }
        result.append("\n]");
        return result.toString();
    */
    return E_NOT_IMPLEMENTED;
}

ECode CX509CertSelector::constructor()
{
    return NOERROR;
}

ECode CX509CertSelector::GetExtensionValue(
        /* [in] */ IX509Certificate *cert,
        /* [in] */ const String& oid,
        /* [out] */ ArrayOf<Byte> **extval)
{
    AutoPtr<ArrayOf<Byte> > bytes;
    IX509Extension::Probe(cert)->GetExtensionValue(oid, (ArrayOf<Byte>**)&bytes);
    if (bytes == NULL) {
        return NOERROR;
    }
    //Apache...Todo later
    //return (byte[]) ASN1OctetString.getInstance().decode(bytes);
    return E_NOT_IMPLEMENTED;
}

ECode CX509CertSelector::Match(
    /* [in] */ ICertificate *certificate,
    /* [out] */ Boolean *result)
{
    //Apache...Todo later
    /*
        if (! (certificate instanceof X509Certificate)) {
            return false;
        }

        X509Certificate cert = (X509Certificate) certificate;
        if ((certificateEquals != null) &&
            !certificateEquals.equals(cert)) {
            return false;
        }
        if ((serialNumber != null) &&
            !serialNumber.equals(cert.getSerialNumber())) {
            return false;
        }
        if ((issuer != null) &&
            !issuer.equals(cert.getIssuerX500Principal())) {
            return false;
        }
        if ((subject != null) &&
            !subject.equals(cert.getSubjectX500Principal())) {
            return false;
        }
        if ((subjectKeyIdentifier != null) &&
            !Arrays.equals(subjectKeyIdentifier,
            // Here and later all of the extension OIDs
            // are taken from rfc 3280 (http://www.ietf.org/rfc/rfc3280.txt)
                           getExtensionValue(cert, "2.5.29.14"))) {
            return false;
        }
        if ((authorityKeyIdentifier != null) &&
            !Arrays.equals(authorityKeyIdentifier,
                           getExtensionValue(cert, "2.5.29.35"))) {
            return false;
        }
        if (certificateValid != null) {
            try {
                cert.checkValidity(certificateValid);
            } catch(CertificateExpiredException e) {
                return false;
            } catch(CertificateNotYetValidException e) {
                return false;
            }
        }
        if (privateKeyValid != null) {
            try {
                byte[] bytes = getExtensionValue(cert, "2.5.29.16");
                if (bytes == null) {
                    return false;
                }
                PrivateKeyUsagePeriod pkup = (PrivateKeyUsagePeriod)
                                    PrivateKeyUsagePeriod.ASN1.decode(bytes);
                Date notBefore = pkup.getNotBefore();
                Date notAfter = pkup.getNotAfter();
                if ((notBefore == null) && (notAfter == null)) {
                    return false;
                }
                if ((notBefore != null)
                    && notBefore.compareTo(privateKeyValid) > 0) {
                    return false;
                }
                if ((notAfter != null)
                    && notAfter.compareTo(privateKeyValid) < 0) {
                    return false;
                }
            } catch (IOException e) {
                return false;
            }
        }
        if (subjectPublicKeyAlgID  != null) {
            try {
                byte[] encoding = cert.getPublicKey().getEncoded();
                AlgorithmIdentifier ai = ((SubjectPublicKeyInfo)
                        SubjectPublicKeyInfo.ASN1.decode(encoding))
                        .getAlgorithmIdentifier();
                if (!subjectPublicKeyAlgID.equals(ai.getAlgorithm())) {
                    return false;
                }
            } catch (IOException e) {
                e.printStackTrace();
                return false;
            }
        }
        if (subjectPublicKey != null) {
            if (!Arrays.equals(subjectPublicKey,
                               cert.getPublicKey().getEncoded())) {
                return false;
            }
        }
        if (keyUsage != null) {
            boolean[] ku = cert.getKeyUsage();
            if (ku != null) {
                int i = 0;
                int min_length = (ku.length < keyUsage.length) ? ku.length
                        : keyUsage.length;
                for (; i < min_length; i++) {
                    if (keyUsage[i] && !ku[i]) {
                        // the specified keyUsage allows,
                        // but certificate does not.
                        return false;
                    }
                }
                for (; i<keyUsage.length; i++) {
                    if (keyUsage[i]) {
                        return false;
                    }
                }
            }
        }
        if (extendedKeyUsage != null) {
            try {
                List keyUsage = cert.getExtendedKeyUsage();
                if (keyUsage != null) {
                    if (!keyUsage.containsAll(extendedKeyUsage)) {
                        return false;
                    }
                }
            } catch (CertificateParsingException e) {
                return false;
            }
        }
        if (pathLen != -1) {
            int p_len = cert.getBasicConstraints();
            if ((pathLen < 0) && (p_len >= 0)) {
                // need end-entity but got CA
                return false;
            }
            if ((pathLen > 0) && (pathLen > p_len)) {
                // allowed _pathLen is small
                return false;
            }
        }
        if (subjectAltNames != null) {
            PASSED:
            try {
                byte[] bytes = getExtensionValue(cert, "2.5.29.17");
                if (bytes == null) {
                    return false;
                }
                List<GeneralName> sans = ((GeneralNames) GeneralNames.ASN1.decode(bytes))
                            .getNames();
                if ((sans == null) || (sans.size() == 0)) {
                    return false;
                }
                boolean[][] map = new boolean[9][];
                // initialize the check map
                for (int i=0; i<9; i++) {
                    map[i] = (subjectAltNames[i] == null)
                            ? EmptyArray.BOOLEAN : new boolean[subjectAltNames[i].size()];
                }
                for (GeneralName name : sans) {
                    int tag = name.getTag();
                    for (int i = 0; i < map[tag].length; i++) {
                        if (subjectAltNames[tag].get(i).equals(name)) {
                            if (!matchAllNames) {
                                break PASSED;
                            }
                            map[tag][i] = true;
                        }
                    }
                }
                if (!matchAllNames) {
                    // there was not any match
                    return false;
                }
                // else check the map
                for (int tag=0; tag<9; tag++) {
                    for (int name=0; name<map[tag].length; name++) {
                        if (!map[tag][name]) {
                            return false;
                        }
                    }
                }
            } catch (IOException e) {
                e.printStackTrace();
                return false;
            }
        }
        if (nameConstraints != null) {
            if (!nameConstraints.isAcceptable(cert)) {
                return false;
            }
        }
        if (policies != null) {
            byte[] bytes = getExtensionValue(cert, "2.5.29.32");
            if (bytes == null) {
                return false;
            }
            if (policies.size() == 0) {
                // if certificate has such extension than it has at least
                // one policy in it.
                return true;
            }
            PASSED:
            try {
                List<PolicyInformation> policyInformations
                        = ((CertificatePolicies) CertificatePolicies.ASN1.decode(bytes))
                        .getPolicyInformations();
                for (PolicyInformation policyInformation : policyInformations) {
                    if (policies.contains(policyInformation.getPolicyIdentifier())) {
                        break PASSED;
                    }
                }
                return false;
            } catch (IOException e) {
                // the extension is invalid
                return false;
            }
        }
        if (pathToNames != null) {
            byte[] bytes = getExtensionValue(cert, "2.5.29.30");
            if (bytes != null) {
                NameConstraints nameConstraints;
                try {
                    nameConstraints =
                        (NameConstraints) NameConstraints.ASN1.decode(bytes);
                } catch (IOException e) {
                    // the extension is invalid;
                    return false;
                }
                if (!nameConstraints.isAcceptable(pathToNames)) {
                    return false;
                }
            }
        }
        return true;
    */
    return E_NOT_IMPLEMENTED;
}

ECode CX509CertSelector::Clone(
/* [out] */ IInterface **obj)
{
    //Apache...Todo later
    /*
        X509CertSelector result;

        try {
            result = (X509CertSelector) super.clone();
        } catch (CloneNotSupportedException e) {
            return null;
        }

        if (this.subjectKeyIdentifier != null) {
            result.subjectKeyIdentifier =
                new byte[this.subjectKeyIdentifier.length];
            System.arraycopy(this.subjectKeyIdentifier, 0,
                    result.subjectKeyIdentifier, 0,
                    this.subjectKeyIdentifier.length);
        }
        if (this.authorityKeyIdentifier != null) {
            result.authorityKeyIdentifier =
                new byte[this.authorityKeyIdentifier.length];
            System.arraycopy(this.authorityKeyIdentifier, 0,
                    result.authorityKeyIdentifier, 0,
                    this.authorityKeyIdentifier.length);
        }
        if (this.subjectPublicKey != null) {
            result.subjectPublicKey = new byte[this.subjectPublicKey.length];
            System.arraycopy(this.subjectPublicKey, 0, result.subjectPublicKey,
                    0, this.subjectPublicKey.length);
        }
        if (this.keyUsage != null) {
            result.keyUsage = new boolean[this.keyUsage.length];
            System.arraycopy(this.keyUsage, 0, result.keyUsage, 0,
                    this.keyUsage.length);
        }
        result.extendedKeyUsage = (this.extendedKeyUsage == null)
            ? null
            : new HashSet<String>(this.extendedKeyUsage);
        if (this.subjectAltNames != null) {
            result.subjectAltNames = new ArrayList[9];
            for (int i=0; i<9; i++) {
                if (this.subjectAltNames[i] != null) {
                    result.subjectAltNames[i] =
                        new ArrayList<GeneralName>(this.subjectAltNames[i]);
                }
            }
        }
        result.policies = (this.policies == null) ? null : new HashSet<String>(this.policies);
        result.pathToNames = (this.pathToNames == null)
            ? null
            : new ArrayList<GeneralName>(this.pathToNames);
        return result;
    */
    return E_NOT_IMPLEMENTED;
}

}
}
}

