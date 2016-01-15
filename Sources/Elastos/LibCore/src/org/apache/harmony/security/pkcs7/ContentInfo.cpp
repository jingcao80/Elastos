
#include "coredef.h"
#include "ContentInfo.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Pkcs7 {

//-----------------------------------------------------------------
//  ContentInfo::ASN1SequenceDerived
//-----------------------------------------------------------------
ContentInfo::ASN1SequenceDerived::ASN1SequenceDerived(
    /* [in] */ ArrayOf<IASN1Type *>* type)
{
    ASSERT_SUCCEEDED(ASN1Sequence::constructor(type));
    SetOptional(1);
}

ECode ContentInfo::ASN1SequenceDerived::GetValues(
    /* [in] */ IInterface* object,
    /* [in] */ ArrayOf<IInterface*>* values)
{
//    AutoPtr<ContentInfo> ci = (ContentInfo*)IContentInfo::Probe(object);
//    values->Set(0, ci->mOid);
//    for (ci->mContent != NULL) {
//        if (ci->mOid->Equals(DATA)) {
//            if (ci->mContent != NULL) {
//                AutoPtr<IASN1OctetStringHelper> hlp;
//                CASN1OctetStringHelper::AcquireSingleton((IASN1OctetStringHelper**)&hlp);
//                AutoPtr<IASN1OctetString> aos;
//                hlp->GetInstance((IASN1OctetString**)&aos);
//                AutoPtr<ArrayOf<Byte> > ret;
//                aos->Encode(ci->mContent, (ArrayOf<Byte>**)&ret);
//                arr = NULL;
//                CArrayOf::New(EIID_IByte, ret->GetLength(), (IArrayOf**)&arr);
//                for (Int32 i = 0; i < ret->GetLength(); i++) {
//                    AutoPtr<IByte> elem;
//                    CByte::New((*ret)[i], (IByte**)&elem);
//                    arr->Put(i, elem.Get());
//                }
//                values->Set(1, arr.Get());
//            }
//        }
//        else if (ISignedData::Probe(ci->mContent)) {
//            AutoPtr<ISignedDataHelper> sdh;
//            CSignedDataHelper::AcquireSingleton((ISignedDataHelper**)&sdh);
//            AutoPtr<IASN1Sequence> asn1Seq;
//            sdh->GetASN1((IASN1Sequence**)&asn1Seq);
//            AutoPtr<ArrayOf<Byte> > bytes;
//            asn1Seq->Encode(ci->mContent, (ArrayOf<Byte>**)&bytes);
//            arr = NULL;
//            CArrayOf::New(EIID_IByte, bytes->GetLength(), (IArrayOf**)&arr);
//            for (Int32 i = 0; i < bytes->GetLength(); i++) {
//                AutoPtr<IByte> elem;
//                CByte::New((*bytes)[i], (IByte**)&elem);
//                arr->Put(i, elem.Get());
//            }
//            values->Set(1, arr.Get());
//        }
//        else {
//            values->Set(1, ci->mContent);
//        }
//    }
    return NOERROR;
}

ECode ContentInfo::ASN1SequenceDerived::GetDecodedObject(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
//    VALIDATE_NOT_NULL(object)
//    AutoPtr<IInterface> content;
//    bis->GetContent((IInterface**)&content);
//    AutoPtr<IArrayOf> values = IArrayOf::Probe(content);
//    AutoPtr<IInterface> val0;
//    values->Get(0, (IInterface**)&val0);
//    Int32 size;
//    IArrayOf::Probe(val0)->GetSize(&size);
//    AutoPtr<ArrayOf<Int32> > oid = ArrayOf<Int32>::Alloc(size);
//    for (Int32 i = 0; i < size; i++) {
//        AutoPtr<IInterface> elem;
//        IArrayOf::Probe(val0)->Get(i, (IInterface**)&elem);
//        Int32 tmp;
//        IInteger32::Probe(elem)->GetValue(&tmp);
//        (*oid)[i] = tmp;
//    }
//    if (oid->Equals(DATA)) {
//        AutoPtr<IInterface> val1;
//        values->Get(1, (IInterface**)&val1);
//        if (val1 != NULL) {
//            AutoPtr<IASN1OctetStringHelper> hlp;
//            CASN1OctetStringHelper::AcquireSingleton((IASN1OctetStringHelper**)&hlp);
//            AutoPtr<IASN1OctetString> aos;
//            hlp->GetInstance((IASN1OctetString**)&aos);
//            Int32 val1Size;
//            IArrayOf::Prbe(val1)->GetSize(&val1Size);
//            AutoPtr<ArrayOf<Byte> > bytes = ArrayOf<Byte>::Alloc(val1Size);
//            for (Int32 i = 0; i < val1Size; i++) {
//                AutoPtr<IInterface> val1Elem;
//                val1->Get(i, (IInterface**)&val1Elem);
//                Byte bt;
//                IByte::Probe(val1Elem)->GetValue(&bt);
//                (*bytes)[i] = bt;
//            }
//            AutoPtr<IInterface> arg2;
//            aos->Decode(bytes, (IInterface**)&arg2);
//            AutoPtr<ArrayOf<Byte> > arg3;
//            bis->GetEncoded((ArrayOf<Byte>**)&arg3);
//            AutoPtr<IContentInfo> ret = new ContentInfo(oid, arg2, arg3);
//            *object = ret.Get();
//            REFCOUNT_ADD(*object)
//            return NOERROR;
//        }
//        else {
//            AutoPtr<ArrayOf<Byte> > arg3;
//            bis->GetEncoded((ArrayOf<Byte>**)&arg3);
//            AutoPtr<IContentInfo> ret = new ContentInfo(oid, NULL, arg3);
//            *object = ret.Get();
//            REFCOUNT_ADD(*object)
//            return NOERROR;
//        }
//    }
//
//    if (oid->Equals(SIGNED_DATA)) {
//        AutoPtr<ISignedDataHelper> sdh;
//        CSignedDataHelper::AcquireSingleton((ISignedDataHelper**)&sdh);
//        AutoPtr<IASN1Sequence> asn1Seq;
//        sdh->GetASN1((IASN1Sequence**)&asn1Seq);
//        AutoPtr<IInterface> val1;
//        values->Get(1, (IInterface**)&val1);
//        Int32 val1Size;
//        IArrayOf::Prbe(val1)->GetSize(&val1Size);
//        AutoPtr<ArrayOf<Byte> > bytes = ArrayOf<Byte>::Alloc(val1Size);
//        for (Int32 i = 0; i < val1Size; i++) {
//            AutoPtr<IInterface> val1Elem;
//            val1->Get(i, (IInterface**)&val1Elem);
//            Byte bt;
//            IByte::Probe(val1Elem)->GetValue(&bt);
//            (*bytes)[i] = bt;
//        }
//        AutoPtr<IInterface> decodedRet;
//        asn1Seq->Decode(bytes, (IInterface**)&decodedRet);
//        AutoPtr<ArrayOf<Byte> > arg3;
//        bis->GetEncoded((ArrayOf<Byte>**)&arg3);
//        AutoPtr<IContentInfo> ret = new ContentInfo(oid, decodedRet, arg3);
//        *object = ret.Get();
//        REFCOUNT_ADD(*object)
//        return NOERROR;
//    }
//    AutoPtr<IInterface> val1;
//    values->Get(1, (IInterface**)&val1);
//    AutoPtr<ArrayOf<Byte> > arg3;
//    bis->GetEncoded((ArrayOf<Byte>**)&arg3);
//    AutoPtr<IContentInfo> ret = new ContentInfo(oid, val1, arg3);
//    *object = ret.Get();
//    REFCOUNT_ADD(*object)
    return NOERROR;
}

//-----------------------------------------------------------------
//  ContentInfo
//-----------------------------------------------------------------
AutoPtr<ArrayOf<Int32> > ContentInfo::DATA;
AutoPtr<ArrayOf<Int32> > ContentInfo::SIGNED_DATA;
AutoPtr<ArrayOf<Int32> > ContentInfo::ENVELOPED_DATA;
AutoPtr<ArrayOf<Int32> > ContentInfo::SIGNED_AND_ENVELOPED_DATA;
AutoPtr<ArrayOf<Int32> > ContentInfo::DIGESTED_DATA;
AutoPtr<ArrayOf<Int32> > ContentInfo::ENCRYPTED_DATA;
AutoPtr<IASN1Sequence> ContentInfo::ASN1;

AutoPtr<ArrayOf<Int32> > ContentInfo::InitStatic()
{
//    Int32[] data = {1, 2, 840, 113549, 1, 7, 1};
//    DATA = ArrayOf<Int32>::Alloc(sizeof(data)/sizeof(data[0]));
//    Int32 i = 0;
//    for (i = 0; i < sizeof(data)/sizeof(data[0]); i++) {
//        (*DATA)[i] = data[i];
//    }
//    Int32 signedData = {1, 2, 840, 113549, 1, 7, 2};
//    SIGNED_DATA = ArrayOf<Int32>::Alloc(sizeof(signedData)/sizeof(signedData[0]));
//    for (i = 0; i < sizeof(signedData)/sizeof(signedData[0]); i++) {
//        (*SIGNED_DATA)[i] = signedData[i];
//    }
//    Int32 envelopedData = {1, 2, 840, 113549, 1, 7, 3};
//    ENVELOPED_DATA = ArrayOf<Int32>::Alloc(sizeof(envelopedData)/sizeof(envelopedData[0]));
//    for (i = 0; i < sizeof(envelopedData)/sizeof(envelopedData[0]); i++) {
//        (*ENVELOPED_DATA)[i] = envelopedData[i];
//    }
//    Int32 signedAndEnvelopedData = {1, 2, 840, 113549, 1, 7, 4};
//    SIGNED_AND_ENVELOPED_DATA = ArrayOf<Int32>::Alloc(sizeof(signedAndEnvelopedData)/sizeof(signedAndEnvelopedData[0]));
//    for (i = 0; i < sizeof(signedAndEnvelopedData)/sizeof(signedAndEnvelopedData[0]); i++) {
//        (*SIGNED_AND_ENVELOPED_DATA)[i] = signedAndEnvelopedData[i];
//    }
//    Int32 digestedData = {1, 2, 840, 113549, 1, 7, 5};
//    DIGESTED_DATA = ArrayOf<Int32>::Alloc(sizeof(digestedData)/sizeof(digestedData[0]));
//    for (i = 0; i < sizeof(digestedData)/sizeof(digestedData[0]); i++) {
//        (*DIGESTED_DATA)[i] = digestedData[i];
//    }
//    Int32 encryptedData = {1, 2, 840, 113549, 1, 7, 6};
//    AutoPtr<ArrayOf<Int32> > ret = ArrayOf<Int32>::Alloc(sizeof(encryptedData)/sizeof(encryptedData[0]));
//    for (i = 0; i < sizeof(encryptedData)/sizeof(encryptedData[0]); i++) {
//        (*ret)[i] = encryptedData[i];
//    }
//
//    AutoPtr<ArrayOf<IASN1Type*> > arg = ArrayOf<IASN1Type*>::Alloc(2);
//    AutoPtr<IASN1OidHelper> oidHelper;
//    CASN1OidHelper::AcquireSingleton((IASN1OidHelper**)&oidHelper);
//    AutoPtr<IASN1Type> inst;
//    oidHelper->GetInstance((IASN1Type**)&inst);
//    arg->Set(0, inst);
//    AutoPtr<IASN1Any> any;
//    AutoPtr<IASN1AnyHelper> anyHelper;
//    CASN1AnyHelper::AcquireSingleton((IASN1AnyHelper**)&anyHelper);
//    anyHelper->GetInstance((IASN1Any**)&any);
//    AutoPtr<IASN1Explicit> any1Exp;
//    CASN1Explicit::New(0, any.Get(), (IASN1Explicit**)&any1Exp);
//    arg->Set(1, any1Exp.Get());
//    ASN1 = new ASN1SequenceDerived(arg);
//    return ret;
    return NULL;
}

ContentInfo::ContentInfo(
    /* [in] */ ArrayOf<Int32>* oid,
    /* [in] */ IInterface* content,
    /* [in] */ ArrayOf<Byte>* encoding)
{
//    mOid = oid;
//    mContent = content;
//    mEncoding = encoding;
}

CAR_INTERFACE_IMPL(ContentInfo, Object, IContentInfo)

ECode ContentInfo::GetSignedData(
    /* [out] */ ISignedData** signedData)
{
//    VALIDATE_NOT_NULL(signedData)
//    if (mOid->Equals(SIGNED_DATA)) {
//        *signedData = ISignedData::Probe(mContent);
//        REFCOUNT_ADD(*signedData)
//    }
    return NOERROR;
}

ECode ContentInfo::GetContent(
    /* [out] */ IInterface** content)
{
//    VALIDATE_NOT_NULL(content)
//    *content = mContent;
//    REFCOUNT_ADD(*content)
    return NOERROR;
}

ECode ContentInfo::GetContentType(
    /* [out, callee] */ ArrayOf<Int32>** contentType)
{
//    VALIDATE_NOT_NULL(contentType)
//    *contentType = mOid;
//    REFCOUNT_ADD(*contentType)
    return NOERROR;
}

ECode ContentInfo::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** encoded)
{
//    VALIDATE_NOT_NULL(encoded)
//    if (mEncoding == NULL) {
//        ASN1->Encode(this, (ArrayOf<Byte>**)&mEncoding);
//    }
//    *encoded = mEncoding;
//    REFCOUNT_ADD(*encoded);
    return NOERROR;
}

ECode ContentInfo::ToString(
    /* [out] */ String* str)
{
//    StringBuilder res;
//    res.AppendCStr("==== ContentInfo:");
//    res.AppendCStr("\n== ContentType (OID): ");
//    for (Int32 i = 0; i < mOid->GetLength(); i++) {
//        res.AppendInt32((*mOid)[i]);
//        res.AppendChar(' ');
//    }
//
//    res.AppendCStr("\n== Content: ");
//    if (mContent != NULL) {
//        res.AppendCStr("\n");
//        String str;
//        ICharSequence::Probe(mContent)->ToString(&str);
//        res.AppendString(str);
//    }
//    res.AppendCStr("\n== Content End");
//    res.AppendCStr("\n==== ContentInfo End\n");
//    return res.ToString(str);
    return NOERROR;
}

} // namespace Pkcs7
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

