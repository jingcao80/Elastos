
#include "coredef.h"
#include "ContentInfo.h"
#include "SignedData.h"
#include "asn1/CASN1Any.h"
#include "asn1/CASN1Explicit.h"
#include "asn1/CASN1Oid.h"
#include "asn1/CASN1OctetString.h"
#include "elastos/core/CoreUtils.h"
#include "elastos/utility/Arrays.h"

using Org::Apache::Harmony::Security::Asn1::CASN1Any;
using Org::Apache::Harmony::Security::Asn1::CASN1Explicit;
using Org::Apache::Harmony::Security::Asn1::CASN1Oid;
using Org::Apache::Harmony::Security::Asn1::CASN1OctetString;
using Org::Apache::Harmony::Security::Asn1::IASN1OctetString;
using Org::Apache::Harmony::Security::Asn1::IASN1Any;
using Org::Apache::Harmony::Security::Asn1::IASN1Type;
using Org::Apache::Harmony::Security::Asn1::IASN1Sequence;
using Elastos::Core::CoreUtils;
using Elastos::Utility::Arrays;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Pkcs7 {

//-----------------------------------------------------------------
//  ContentInfo::ASN1SequenceDerived
//-----------------------------------------------------------------
ECode ContentInfo::ASN1SequenceDerived::constructor(
    /* [in] */ ArrayOf<IASN1Type *>* type)
{
    FAIL_RETURN(ASN1Sequence::constructor(type));
    SetOptional(1);
    return NOERROR;
}

ECode ContentInfo::ASN1SequenceDerived::GetValues(
    /* [in] */ IInterface* object,
    /* [in] */ ArrayOf<IInterface*>* values)
{
    AutoPtr<ContentInfo> ci = (ContentInfo*)IContentInfo::Probe(object);
    values->Set(0,  CoreUtils::Convert(ci->mOid));
    if (ci->mContent != NULL) {
        if (Arrays::Equals(ci->mOid, DATA)) {
            if (ci->mContent != NULL) {
                AutoPtr<IASN1OctetString> aos;
                CASN1OctetString::GetInstance((IASN1OctetString**)&aos);
                AutoPtr<ArrayOf<Byte> > ret;
                IASN1Type::Probe(aos)->Encode(ci->mContent, (ArrayOf<Byte>**)&ret);
                AutoPtr<IArrayOf> arr = CoreUtils::ConvertByteArray(ret);
                values->Set(1, arr.Get());
            }
        }
        else if (ISignedData::Probe(ci->mContent)) {
            AutoPtr<ArrayOf<Byte> > bytes;
            IASN1Type::Probe(SignedData::ASN1)->Encode(ci->mContent, (ArrayOf<Byte>**)&bytes);
            AutoPtr<IArrayOf> arr = CoreUtils::ConvertByteArray(bytes);
            values->Set(1, arr.Get());
        }
        else {
            values->Set(1, ci->mContent);
        }
    }
    return NOERROR;
}

ECode ContentInfo::ASN1SequenceDerived::GetDecodedObject(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)
    AutoPtr<IInterface> content;
    bis->GetContent((IInterface**)&content);
    AutoPtr<IArrayOf> values = IArrayOf::Probe(content);
    AutoPtr<IInterface> val0;
    values->Get(0, (IInterface**)&val0);
    AutoPtr<IArrayOf> arr = IArrayOf::Probe(val0);
    Int32 size;
    arr->GetLength(&size);
    AutoPtr<ArrayOf<Int32> > oid = ArrayOf<Int32>::Alloc(size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> elem;
        arr->Get(i, (IInterface**)&elem);
        Int32 tmp;
        IInteger32::Probe(elem)->GetValue(&tmp);
        (*oid)[i] = tmp;
    }
    if (Arrays::Equals(oid, DATA)) {
        AutoPtr<IInterface> val1;
        values->Get(1, (IInterface**)&val1);
        if (val1 != NULL) {
            AutoPtr<IASN1OctetString> aos;
            CASN1OctetString::GetInstance((IASN1OctetString**)&aos);
            arr = IArrayOf::Probe(val1);
            Int32 val1Size;
            arr->GetLength(&val1Size);
            AutoPtr<ArrayOf<Byte> > bytes = ArrayOf<Byte>::Alloc(val1Size);
            for (Int32 i = 0; i < val1Size; i++) {
                AutoPtr<IInterface> val1Elem;
                arr->Get(i, (IInterface**)&val1Elem);
                Byte bt;
                IByte::Probe(val1Elem)->GetValue(&bt);
                (*bytes)[i] = bt;
            }
            AutoPtr<IInterface> arg2;
            IASN1Type::Probe(aos)->Decode(bytes, (IInterface**)&arg2);
            AutoPtr<ArrayOf<Byte> > arg3;
            bis->GetEncoded((ArrayOf<Byte>**)&arg3);
            AutoPtr<IContentInfo> ret = new ContentInfo(oid, arg2, arg3);
            *object = ret.Get();
            REFCOUNT_ADD(*object)
            return NOERROR;
        }
        else {
            AutoPtr<ArrayOf<Byte> > arg3;
            bis->GetEncoded((ArrayOf<Byte>**)&arg3);
            AutoPtr<IContentInfo> ret = new ContentInfo(oid, NULL, arg3);
            *object = ret.Get();
            REFCOUNT_ADD(*object)
            return NOERROR;
        }
    }

    if (Arrays::Equals(oid, SIGNED_DATA)) {
        AutoPtr<IInterface> val1;
        values->Get(1, (IInterface**)&val1);
        arr = IArrayOf::Probe(val1);
        Int32 val1Size;
        arr->GetLength(&val1Size);
        AutoPtr<ArrayOf<Byte> > bytes = ArrayOf<Byte>::Alloc(val1Size);
        for (Int32 i = 0; i < val1Size; i++) {
            AutoPtr<IInterface> val1Elem;
            arr->Get(i, (IInterface**)&val1Elem);
            Byte bt;
            IByte::Probe(val1Elem)->GetValue(&bt);
            (*bytes)[i] = bt;
        }
        AutoPtr<IInterface> decodedRet;
        IASN1Type::Probe(SignedData::ASN1)->Decode(bytes, (IInterface**)&decodedRet);
        AutoPtr<ArrayOf<Byte> > arg3;
        bis->GetEncoded((ArrayOf<Byte>**)&arg3);
        AutoPtr<IContentInfo> ret = new ContentInfo(oid, decodedRet, arg3);
        *object = ret.Get();
        REFCOUNT_ADD(*object)
        return NOERROR;
    }
    AutoPtr<IInterface> val1;
    values->Get(1, (IInterface**)&val1);
    AutoPtr<ArrayOf<Byte> > arg3;
    bis->GetEncoded((ArrayOf<Byte>**)&arg3);
    AutoPtr<IContentInfo> ret = new ContentInfo(oid, val1, arg3);
    *object = ret.Get();
    REFCOUNT_ADD(*object)
    return NOERROR;
}

//-----------------------------------------------------------------
//  ContentInfo
//-----------------------------------------------------------------
INIT_PROI_4 AutoPtr<ArrayOf<Int32> > ContentInfo::DATA;
INIT_PROI_4 AutoPtr<ArrayOf<Int32> > ContentInfo::SIGNED_DATA;
INIT_PROI_4 AutoPtr<ArrayOf<Int32> > ContentInfo::ENVELOPED_DATA;
INIT_PROI_4 AutoPtr<ArrayOf<Int32> > ContentInfo::SIGNED_AND_ENVELOPED_DATA;
INIT_PROI_4 AutoPtr<ArrayOf<Int32> > ContentInfo::DIGESTED_DATA;
INIT_PROI_4 AutoPtr<ArrayOf<Int32> > ContentInfo::ENCRYPTED_DATA;
INIT_PROI_4 AutoPtr<IASN1Sequence> ContentInfo::ASN1 = InitStatic();

AutoPtr<IASN1Sequence> ContentInfo::InitStatic()
{
    Int32 data[] = {1, 2, 840, 113549, 1, 7, 1};
    Int32 size = ArraySize(data);
    DATA = ArrayOf<Int32>::Alloc(size);
    DATA->Copy(data, size);

    Int32 signedData[] = {1, 2, 840, 113549, 1, 7, 2};
    size = ArraySize(signedData);
    SIGNED_DATA = ArrayOf<Int32>::Alloc(size);
    SIGNED_DATA->Copy(signedData, size);

    Int32 envelopedData[] = {1, 2, 840, 113549, 1, 7, 3};
    size = ArraySize(envelopedData);
    ENVELOPED_DATA = ArrayOf<Int32>::Alloc(size);
    ENVELOPED_DATA->Copy(envelopedData, size);

    Int32 signedAndEnvelopedData[] = {1, 2, 840, 113549, 1, 7, 4};
    size = ArraySize(signedAndEnvelopedData);
    SIGNED_AND_ENVELOPED_DATA = ArrayOf<Int32>::Alloc(size);
    SIGNED_AND_ENVELOPED_DATA->Copy(signedAndEnvelopedData, size);

    Int32 digestedData[] = {1, 2, 840, 113549, 1, 7, 5};
    size = ArraySize(digestedData);
    DIGESTED_DATA = ArrayOf<Int32>::Alloc(size);
    DIGESTED_DATA->Copy(digestedData, size);

    Int32 encryptedData[] = {1, 2, 840, 113549, 1, 7, 6};
    size = ArraySize(encryptedData);
    ENCRYPTED_DATA = ArrayOf<Int32>::Alloc(size);
    ENCRYPTED_DATA->Copy(encryptedData, size);

    AutoPtr<ArrayOf<IASN1Type*> > arg = ArrayOf<IASN1Type*>::Alloc(2);
    AutoPtr<IASN1Type> inst;
    CASN1Oid::GetInstance((IASN1Type**)&inst);
    arg->Set(0, inst);
    AutoPtr<IASN1Any> any;
    CASN1Any::GetInstance((IASN1Any**)&any);
    AutoPtr<IASN1Type> any1Exp;
    CASN1Explicit::New(0, IASN1Type::Probe(any), (IASN1Type**)&any1Exp);
    arg->Set(1, any1Exp);
    AutoPtr<ASN1SequenceDerived> ret = new ASN1SequenceDerived();
    ret->constructor(arg);
    return ret;
}

ContentInfo::ContentInfo(
    /* [in] */ ArrayOf<Int32>* oid,
    /* [in] */ IInterface* content,
    /* [in] */ ArrayOf<Byte>* encoding)
{
    mOid = oid;
    mContent = content;
    mEncoding = encoding;
}

CAR_INTERFACE_IMPL(ContentInfo, Object, IContentInfo)

ECode ContentInfo::GetSignedData(
    /* [out] */ ISignedData** signedData)
{
    VALIDATE_NOT_NULL(signedData)
    *signedData = NULL;
    if (Arrays::Equals(mOid, SIGNED_DATA)) {
        *signedData = ISignedData::Probe(mContent);
        REFCOUNT_ADD(*signedData)
    }
    return NOERROR;
}

ECode ContentInfo::GetContent(
    /* [out] */ IInterface** content)
{
    VALIDATE_NOT_NULL(content)
    *content = mContent;
    REFCOUNT_ADD(*content)
    return NOERROR;
}

ECode ContentInfo::GetContentType(
    /* [out, callee] */ ArrayOf<Int32>** contentType)
{
    VALIDATE_NOT_NULL(contentType)
    *contentType = mOid;
    REFCOUNT_ADD(*contentType)
    return NOERROR;
}

ECode ContentInfo::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** encoded)
{
    VALIDATE_NOT_NULL(encoded)
    if (mEncoding == NULL) {
        IASN1Type::Probe(ASN1)->Encode((IObject*)this, (ArrayOf<Byte>**)&mEncoding);
    }
    *encoded = mEncoding;
    REFCOUNT_ADD(*encoded);
    return NOERROR;
}

ECode ContentInfo::ToString(
    /* [out] */ String* str)
{
    StringBuilder res;
    res.Append("==== ContentInfo:");
    res.Append("\n== ContentType (OID): ");
    for (Int32 i = 0; i < mOid->GetLength(); i++) {
        res.Append((*mOid)[i]);
        res.AppendChar(' ');
    }

    res.Append("\n== Content: ");
    if (mContent != NULL) {
        res.Append("\n");
        res.Append(mContent);
    }
    res.Append("\n== Content End");
    res.Append("\n==== ContentInfo End\n");
    return res.ToString(str);
}

} // namespace Pkcs7
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

