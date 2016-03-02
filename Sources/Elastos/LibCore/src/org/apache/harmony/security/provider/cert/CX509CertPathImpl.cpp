
#ifndef FOR_DERIVED
#define FOR_DERIVED
#endif

#ifndef FOR_CX509CERTPATHIMPL
#define FOR_CX509CERTPATHIMPL
#endif

#include "CX509CertPathImpl.h"
#include <cmdef.h>

using Elastos::Utility::CCollections;
using Elastos::Utility::ICollections;
using Elastos::Utility::Arrays;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Provider {
namespace Cert {

CAR_INTERFACE_IMPL(CX509CertPathImpl::ASN1SequenceDerived1, IASN1Sequence)

CAR_INTERFACE_IMPL(CX509CertPathImpl::ASN1SequenceDerived2, IASN1Sequence)

ASN1SEQUENCE_METHODS_IMPL(CX509CertPathImpl::ASN1SequenceDerived1, ASN1Sequence)

ASN1SEQUENCE_METHODS_IMPL(CX509CertPathImpl::ASN1SequenceDerived2, ASN1Sequence)

CX509CertPathImpl::ASN1SequenceDerived1::ASN1SequenceDerived1(
    /* [in] */ ArrayOf<IASN1Type *>* type)
    : ASN1Sequence(type)
{
    Byte precalculated_head[] = {0x02, 0x01,
            0x01,// version (v1)
            0x31, 0x00,// empty set of DigestAlgorithms
            0x30, 0x03, 0x06, 0x01, 0x00 // empty ContentInfo with oid=0
        };
    PRECALCULATED_HEAD = ArrayOf<Byte>::Alloc(sizeof(precalculated_head)/sizeof(precalculated_head[0]));
    for (Int32 i = 0; i < sizeof(precalculated_head)/sizeof(precalculated_head[0]); i++) {
        (*PRECALCULATED_HEAD)[i] = precalculated_head[i];
    }

    SIGNERS_INFO = ArrayOf<Byte>::Alloc(2);
    (*SIGNERS_INFO)[0] = 0x31;
    (*SIGNERS_INFO)[1] = 0x00;
}

ECode CX509CertPathImpl::ASN1SequenceDerived1::GetValues(
    /* [in] */ IInterface* object,
    /* [in] */ ArrayOf<IInterface*>* values)
{
    values[0] = PRECALCULATED_HEAD;
    values->Set(0, PRECALCULATED_HEAD.Get());
    values->Set(1, object); // pass X509CertPathImpl object
    return values->Set(2, SIGNERS_INFO.Get());
}

ECode CX509CertPathImpl::ASN1SequenceDerived1::GetDecodedObject(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    return ASN1Sequence::GetDecodedObject(bis, object);
}

ECode CX509CertPathImpl::ASN1SequenceDerived1::DecodeEx3(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    /*
    throw new RuntimeException(
                    "Invalid use of encoder for PKCS#7 SignedData object");*/
    return E_RUNTIME_EXCEPTION;
}

CX509CertPathImpl::ASN1SequenceDerived2::ASN1SequenceDerived2(
    /* [in] */ ArrayOf<IASN1Type *>* type)
    : ASN1Sequence(type)
{
    AutoPtr<IASN1Type> ty;
    ASN1Oid::GetInstance((IASN1Type**)&ty);
    ty->Encode(CContentInfo::SIGNED_DATA.Get(), (ArrayOf<Byte>**)&SIGNED_DATA_OID);
}

ECode CX509CertPathImpl::ASN1SequenceDerived2::GetValues(
    /* [in] */ IInterface* object,
    /* [in] */ ArrayOf<IInterface*>* values)
{
    //Todo, transfer ArrayOf<Byte>* to IInterface*
    AutoPtr<IArrayOf> arr;
    CArrayOf::New(EIID_IByte, SIGNED_DATA_OID->GetLength(), (IArrayOf**)&arr);
    for (Int32 i = 0; i < SIGNED_DATA_OID->GetLength(); i++) {
        AutoPtr<IByte> bt;
        CByte::New((*SIGNED_DATA_OID)[i], (IByte**)&bt);
        arr->Put(i, bt.Get());
    }
    values->Set(0, arr.Get());
    return values->Set(1, object); // pass X509CertPathImpl object
}

ECode CX509CertPathImpl::ASN1SequenceDerived2::GetDecodedObject(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    return ASN1Sequence::GetDecodedObject(bis, object);
}

ECode CX509CertPathImpl::ASN1SequenceDerived2::DecodeEx3(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    /* throw new RuntimeException(
            "Invalid use of encoder for PKCS#7 SignedData object"); */
    return E_RUNTIME_EXCEPTION;
}

CAR_INTERFACE_IMPL(CX509CertPathImpl::ASN1SequenceOfDerived, IASN1SequenceOf)

ASN1TYPE_METHODS_IMPL(CX509CertPathImpl::ASN1SequenceOfDerived, ASN1SequenceOf)

ECode CX509CertPathImpl::ASN1SequenceOfDerived::GetDecodedObject(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    // retrieve the decoded content
    AutoPtr<IInterface> content;
    bis->GetContent((IInterface**)&content);
    AutoPtr<IList> encodings = IList::Probe(content);
    Int32 size;
    encodings->GetSize(&size);
    AutoPtr<IList> certificates;
    CArrayList::New(size, (IArrayList**)&certificates);
    for (Int32 i = 0; i < size; i++) {
        // create the X.509 certificate on the base of its encoded form
        // and add it to the list.
        AutoPtr<IInterface> elem;
        encodings->Get(i, (IInterface**)&elem);
        AutoPtr<IArrayOf> arr = IArrayOf::Probe(elem);
        Int32 len;
        arr->GetLength(&len);
        AutoPtr<ArrayOf<Byte> > bytes = ArrayOf<Byte>::Alloc(len);
        for (Int32 idx = 0; idx < len; idx++) {
            AutoPtr<IInterface> elm;
            arr->Get(idx, (IInterface**)&elm);
            Byte bt;
            IByte::Probe(elm)->GetValue(&bt);
            (*bytes)[idx] = bt;
        }
        AutoPtr<IInterface> decObj;
        CCertificate::ASN1->Decode(bytes, (IInterface**)&decObj);
        certificates->Add(new CX509CertImpl(ICertificate::Probe(decObj)));
    }
    // create and return the resulting object
    AutoPtr<ArrayOf<Byte> > encoded;
    bis->GetEncoded((ArrayOf<Byte>**)&encoded);
    *object = new CX509CertPathImpl(certificates.Get(), PKI_PATH, encoded);
    REFCOUNT_ADD(*object)
    return NOERROR;
}

ECode CX509CertPathImpl::ASN1SequenceOfDerived::SetEncodingContent(
    /* [in] */ IBerOutputStream* bos)
{
    return ASN1SequenceOf::SetEncodingContent(bos);
}

ECode CX509CertPathImpl::ASN1SequenceOfDerived::SetType(
    /* [in] */ IASN1Type* type)
{
    return ASN1SequenceOf::SetType(type);
}

ECode CX509CertPathImpl::ASN1SequenceOfDerived::GetType(
    /* [out] */ IASN1Type** type)
{
    return ASN1SequenceOf::GetType(type);
}

ECode CX509CertPathImpl::ASN1SequenceOfDerived::GetValues(
    /* [in] */ IInterface* object,
    /* [out] */ ICollection** values)
{
    VALIDATE_NOT_NULL(values)
    // object to be encoded
    AutoPtr<CX509CertPathImpl> cp = (CX509CertPathImpl*) object;
    // if it has no certificates in it - create the sequence of size 0
    if (cp->mCertificates == NULL) {
        AutoPtr<IArrayList> arrList;
        CArrayList::New((IArrayList**)&arrList);
        *values = ICollection::Probe(arrList.Get());
        REFCOUNT_ADD(*values)
        return NOERROR;
    }
    Int32 size;
    cp->mCertificates->GetSize(&size);
    AutoPtr<IList> encodings;
    CArrayList::New(size, (IArrayList**)&encodings);
    ECode ec = NOERROR;
    for (Int32 i = 0; i < size; i++) {
        // get the encoded form of certificate and place it into the
        // list to be encoded in PkiPath format
        AutoPtr<IInterface> elem;
        cp->mCertificates->Get(i, (IInterface**)&elem);
        AutoPtr<ArrayOf<Byte> > bytes;
        ec = IX509Certificate::Probe(elem)->GetEncoded((ArrayOf<Byte>**)&bytes);
        FAIL_GOTO(ec, ERROR_PROCESS)
        AutoPtr<IArrayOf> arr;
        Int32 len = bytes->GetLength();
        CArrayOf::New(EIID_IByte, len, (IArrayOf**)&arr);
        for (Int32 idx = 0; idx < len; idx++) {
            AutoPtr<IByte> bt;
            CByte::New((*bytes)[idx], (IByte**)&bt);
            arr->Put(idx, bt.Get());
        }
        Boolean ret;
        encodings->Add(arr.Get(), &ret);
    }
    *values = ICollection::Probe(encodings.Get());
    REFCOUNT_ADD(*values)
ERROR_PROCESS:
    if (ec == (ECode)E_CERTIFICATE_ENCODING_EXCEPTION) {
        ec = E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return ec;
}

CX509CertPathImpl::ASN1SequenceOfDerived::ASN1SequenceOfDerived(
    /* [in] */ IASN1Type* type)
    : ASN1SequenceOf(type)
{}

AutoPtr<IList> CX509CertPathImpl::sEncodings;
AutoPtr<IASN1SequenceOf> CX509CertPathImpl::ASN1;
AutoPtr<IASN1Sequence> CX509CertPathImpl::ASN1_SIGNED_DATA;
AutoPtr<IASN1Sequence> CX509CertPathImpl::PKCS7_SIGNED_DATA_OBJECT;
AutoPtr<ArrayOf<String> > CX509CertPathImpl::sEncodingsArr = InitStatic();

AutoPtr<ArrayOf<String> > CX509CertPathImpl::InitStatic()
{
    AutoPtr<ArrayOf<IInterface*> > tmp = ArrayOf<IInterface*>::Alloc(2);
    AutoPtr<ICharSequence> cs;
    CString::New("PkiPath", (ICharSequence**)&cs);
    tmp->Set(0, cs.Get());
    cs = NULL;
    CString::New("PKCS7", (ICharSequence**)&cs);
    tmp->Set(1, cs.Get());
    AutoPtr<IArrays> arr;
    CArrays::AcquireSingleton((IArrays**)&arr);
    AutoPtr<IList> lst;
    arr->AsList(tmp, (IList**)&lst);
    AutoPtr<ICollections> col;
    CCollections::AcquireSingleton((ICollections**)&col);
    col->UnmodifiableList(lst, (IList**)&sEncodings);

    AutoPtr<ArrayOf<String> > ret = ArrayOf<String>::Alloc(2);
    (*ret)[0] = "PkiPath";
    (*ret)[1] = "PKCS7";

    AutoPtr<IASN1AnyHelper> hlp;
    CASN1AnyHelper::AcquireSingleton((IASN1AnyHelper**)&hlp);
    AutoPtr<IASN1Type> ty;
    hlp->GetInstance((IASN1Type**)&ty);
    ASN1 = new ASN1SequenceOfDerived(ty);

    AutoPtr<ArrayOf<IASN1Type*> > argForISN = ArrayOf<IASN1Type*>::Alloc(3);
    argForISN->Set(0, ty);
    argForISN->Set(1, new CASN1Implicit(0, ASN1));
    argForISN->Set(2, ty);
    ASN1_SIGNED_DATA = new ASN1SequenceDerived1(argForISN);

    argForISN = ArrayOf<IASN1Type*>::Alloc(2);
    argForISN->Set(0, ty);
    argForISN->Set(1, new CASN1Explicit(0, ASN1_SIGNED_DATA.Get()));
    PKCS7_SIGNED_DATA_OBJECT = new ASN1SequenceDerived2(argForISN);

    return ret;
}

ECode CX509CertPathImpl::GetInstance(
    /* [in] */ IInputStream* is,
    /* [out] */ IX509CertPathImpl** instance)
{
    VALIDATE_NOT_NULL(instance)
    AutoPtr<IInterface> tmp;
    ECode ec;
    FAIL_GOTO(ec = ASN1->DecodeEx2(is, (IInterface**)&tmp), ERROR_PROCESS)
    *instance = IX509CertPathImpl::Probe(tmp);
    REFCOUNT_ADD(*instance)
ERROR_PROCESS:
    if (E_IO_EXCEPTION == ec) {
        ec = E_CERTIFICATE_EXCEPTION;
    }
    return ec;
}

ECode CX509CertPathImpl::GetInstance(
    /* [in] */ IInputStream* is,
    /* [in] */ const String& encoding,
    /* [out] */ IX509CertPathImpl** instance)
{
    VALIDATE_NOT_NULL(instance)
    Boolean isContained;
    AutoPtr<ICharSequence> cs;
    CString::New(encoding, (ICharSequence**)&cs);
    if (sEncodings->Contains(cs.Get(), &isContained), !isContained) {
        //throw new CertificateException("Unsupported encoding");
        return E_CERTIFICATE_EXCEPTION;
    }
    if ((*sEncodingsArr)[0]->Equals(encoding)) {
        // generate the object from PkiPath encoded form
        AutoPtr<IInterface> tmp;
        ASN1->DecodeEx2(is.Get(), (IInterface**)&tmp);
        *instance = IX509CertPathImpl::Probe(tmp);
        REFCOUNT_ADD(*instance)
        return NOERROR;
    }
    else {
        AutoPtr<IInterface> tmp;
        CContentInfo::ASN1->DecodeEx2(is, (IInterface**)&tmp);
        AutoPtr<IContentInfo> ci = IContentInfo::Probe(tmp);
        AutoPtr<ISignedData> sd;
        ci->GetSignedData((ISignedData**)&sd);
        if (sd == NULL) {
            /* throw new CertificateException(
                            "Incorrect PKCS7 encoded form: missing signed data");*/
            return E_CERTIFICATE_EXCEPTION;
        }
        AutoPtr<IList> certs;
        sd->GetCertificates((IList**)&certs);
        if (certs == NULL) {
            // empty chain of certificates
            CArrayList::New((IArrayList**)&certs);
        }
        AutoPtr<IList> result;
        CArrayList::New((IArrayList**)&result);
        AutoPtr<IIterator> it;
        certs->GetIterator((IIterator**)&it);
        Boolean hasNext;
        it->HasNext(&hasNext);
        while(hasNext) {
            AutoPtr<IInterface> cert;
            AutoPtr<IX509CertImpl> impl;
            it->GetNext((IInterface**)&cert);
            CX509CertImpl::New(cert.Get(), (IX509CertImpl**)&impl);
            result->Add(impl.Get());
            it->HasNext(&hasNext);
        }
        AutoPtr<ArrayOf<Byte> > arg2;
        ci->GetEncoded((ArrayOf<Byte>**)&arg2);
        *instance = new CX509CertPathImpl(result, PKCS7, arg2);
        REFCOUNT_ADD(*instance)
        return NOERROR;
    }
}

ECode CX509CertPathImpl::GetInstance(
    /* [in] */ ArrayOf<Byte>* inp,
    /* [out] */ IX509CertPathImpl** instance)
{
    VALIDATE_NOT_NULL(instance)
    AutoPtr<IInterface> tmp;
    ECode ec;
    FAIL_GOTO(ec = ASN1->DecodeEx2(inp, (IInterface**)&tmp), ERROR_PROCESS)
    *instance = I509CertPathImpl::Probe(tmp);
    REFCOUNT_ADD(*instance)
    return ec;
ERROR_PROCESS:
    if (ec == (ECode)E_IO_EXCEPTION) {
        ec = E_CERTIFICATE_EXCEPTION;
    }
    return ec;
}

ECode CX509CertPathImpl::GetInstance(
    /* [in] */ ArrayOf<Byte>* inp,
    /* [in] */ const String& encoding,
    /* [out] */ IX509CertPathImpl** instance)
{
    VALIDATE_NOT_NULL(instance)
    Boolean isContained;
    AutoPtr<ICharSequence> cs;
    CString::New(encoding, (ICharSequence**)&cs);
    if (sEncodings->Contains(cs.Get(), &isContained), !isContained) {
        //throw new CertificateException("Unsupported encoding");
        return E_CERTIFICATE_EXCEPTION;
    }
    if ((*sEncodingsArr)[0]->Equals(encoding)) {
        // generate the object from PkiPath encoded form
        AutoPtr<IInterface> tmp;
        FAIL_RETURN(ASN1->DecodeEx2(inp, (IInterface**)&tmp))
        *instance = IX509CertPathImpl::Probe(tmp);
        REFCOUNT_ADD(*instance)
        return NOERROR;
    }
    else {
        // generate the object from PKCS #7 encoded form
        AutoPtr<IInterface> tmp;
        CContentInfo::ASN1->DecodeEx2(inp, (IInterface**)&tmp);
        AutoPtr<IContentInfo> ci = IContentInfo::Probe(tmp);
        AutoPtr<ISignedData> sd;
        ci->GetSignedData((ISignedData**)&sd);
        if (sd == NULL) {
            //throw new CertificateException("Incorrect PKCS7 encoded form: missing signed data");
            return E_CERTIFICATE_EXCEPTION;
        }
        AutoPtr<IList> certs;
        sd->GetCertificates((IList**)&certs);
        if (certs == NULL) {
            CArrayList::New((IArrayList**)&certs);
        }

        AutoPtr<IList> result;
        CArrayList::New((IArrayList**)&result);
        AutoPtr<IIterator> it;
        certs->GetIterator((IIterator**)&it);
        Boolean hasNext;
        it->HasNext(&hasNext)
        while (hasNext) {
            AutoPtr<IInterface> tmp;
            it->GetNext((IInterface**)&tmp);
            AutoPtr<IX509CertImpl> impl;
            CX509CertImpl::New(ICertificate::Probe(tmp), (IX509CertImpl**)&impl);
            result->Add(impl.Get());
        }
        AutoPtr<ArrayOf<Byte> > bytes;
        FAIL_RETURN(ci->GetEncoded((ArrayOf<Byte>**)&bytes))
        *instance = new CX509CertPathImpl(result, PKCS7, bytes);
        REFCOUNT_ADD(*instance)
        return NOERROR;
    }
}

ECode CX509CertPathImpl::GetType(
    /* [out] */ String * pType)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CX509CertPathImpl::Equals(
    /* [in] */ IInterface * pOther,
    /* [out] */ Boolean * pResult)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CX509CertPathImpl::GetHashCode(
    /* [out] */ Int32 * pHashCode)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CX509CertPathImpl::ToString(
    /* [out] */ String * pStr)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CX509CertPathImpl::GetCertificates(
    /* [out] */ IList** certs)
{
    AutoPtr<ICollections> col;
    CCollections::AcquireSingleton((ICollections**)&col);
    return col->UnmodifiableList(mCertificates, certs);
}

ECode CX509CertPathImpl::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** encoded)
{
    VALIDATE_NOT_NULL(encoded)
    if (mPkiPathEncoding == NULL) {
        ASN1->Encode(this, (ArrayOf<Byte>**)&mPkiPathEncoding);
    }
    AutoPtr<ArrayOf<Byte> > result = ArrayOf<Byte>::Alloc(mPkiPathEncoding->GetLength());
    result->Copy(0, mPkiPathEncoding, 0, mPkiPathEncoding->GetLength());
    *encoded = result;
    REFCOUNT_ADD(*encoded)
    return NOERROR;
}

ECode CX509CertPathImpl::GetEncodedEx(
    /* [in] */ const String& encoding,
    /* [out, callee] */ ArrayOf<Byte>** encoded)
{
    VALIDATE_NOT_NULL(encoded)
    Boolean isContained;
    AutoPtr<ICharSequence> cs;
    CString::New(encoding, (ICharSequence**)&cs);
    if (sEncodings->Contains(cs.Get(), &isContained), !isContained) {
        //throw new CertificateEncodingException("Unsupported encoding");
        return E_CERTIFICATE_ENCODING_EXCEPTION;
    }
    if ((*sEncodingsArr)[0].Equals(encoding)) {
        // PkiPath encoded form
        return GetEncoded(encoded);
    }
    else {
        // PKCS7 encoded form
        if (mPkcs7Encoding == NULL) {
            PKCS7_SIGNED_DATA_OBJECT->Encode(this, (ArrayOf<Byte>**)&mPkcs7Encoding);
        }
        AutoPtr<ArrayOf<Byte> > result = ArrayOf<Byte>::Alloc(mPkcs7Encoding->GetLength());
        result->Copy(0, mPkcs7Encoding, 0, mPkcs7Encoding->GetLength());
        *encoded = result;
        REFCOUNT_ADD(*encoded)
        return NOERROR;
    }
}

ECode CX509CertPathImpl::GetEncodings(
    /* [out] */ IIterator** encodings)
{
    return sEncodings->GetIterator(encodings);
}

ECode CX509CertPathImpl::constructor(
    /* [in] */ IList* certs)
{
    CertPath::Init("X.509");
    Int32 size;
    certs->GetSize(&size);
    CArrayList::New(size, (IArrayList**)&mCertificates);

    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> cert;
        certs->Get(i, (IInterface**)&cert);
        if (IX509Certificate::Probe(cert) == NULL) {
            /*throw new CertificateException(
                    "One of the provided certificates is not an X509 certificate");*/
            return E_CERTIFICATE_EXCEPTION;
        }
        mCertificates->Add(cert.Get());
    }
    return NOERROR;
}

} // namespace Cert
} // namespace Provider
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

