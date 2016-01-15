
#include "CASN1Integer.h"
#include <cmdef.h>

using Elastos::Core::IArrayOf;
using Elastos::Core::CArrayOf;
using Elastos::Core::IByte;
using Elastos::Core::CByte;
using Elastos::Core::EIID_IByte;
using Elastos::Math::IBigInteger;
using Elastos::Math::CBigInteger;
using Elastos::Math::IBigIntegerHelper;
using Elastos::Math::CBigIntegerHelper;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

AutoPtr<IASN1Integer> CASN1Integer::ASN1 = InitStatic();

AutoPtr<IASN1Integer> CASN1Integer::InitStatic()
{
    return new CASN1Integer();
}

ECode CASN1Integer::GetInstance(
    /* [out] */ IASN1Integer** instance)
{
    VALIDATE_NOT_NULL(instance)
    *instance = ASN1;
    REFCOUNT_ADD(*instance)
    return NOERROR;
}

ECode CASN1Integer::ToIntValue(
    /* [in] */ IInterface* decoded,
    /* [out] */ Int32* intValue)
{
    Int32 size;
    IArrayOf::Probe(decoded)->GetLength(&size);
    AutoPtr<ArrayOf<Byte> > dec = ArrayOf<Byte>::Alloc(size);
    for (Int32 i = 0 ; i < size; i++) {
        AutoPtr<IInterface> bt;
        Byte elem;
        IArrayOf::Probe(decoded)->Get(i, (IInterface**)&bt);
        IByte::Probe(bt)->GetValue(&elem);
        (*dec)[i] = elem;
    }
    AutoPtr<IBigInteger> bi;
    CBigInteger::New(dec, (IBigInteger**)&bi);
    return bi->Int32Value(intValue);
}

ECode CASN1Integer::ToBigIntegerValue(
    /* [in] */ IInterface* decoded,
    /* [out] */ IBigInteger** bigInteger)
{
    Int32 size;
    IArrayOf::Probe(decoded)->GetLength(&size);
    AutoPtr<ArrayOf<Byte> > dec = ArrayOf<Byte>::Alloc(size);
    for (Int32 i = 0 ; i < size; i++) {
        AutoPtr<IInterface> bt;
        Byte elem;
        IArrayOf::Probe(decoded)->Get(i, (IInterface**)&bt);
        IByte::Probe(bt)->GetValue(&elem);
        (*dec)[i] = elem;
    }
    return CBigInteger::New(dec, bigInteger);
}

ECode CASN1Integer::FromIntValue(
    /* [in] */ Int32 value,
    /* [out] */ IInterface** fromIntValue)
{
    VALIDATE_NOT_NULL(fromIntValue)
    AutoPtr<IBigIntegerHelper> helper;
    AutoPtr<IBigInteger> bi;
    CBigIntegerHelper::AcquireSingleton((IBigIntegerHelper**)&helper);
    helper->ValueOf(value, (IBigInteger**)&bi);
    AutoPtr<ArrayOf<Byte> > bytesArray;
    bi->ToByteArray((ArrayOf<Byte>**)&bytesArray);
    AutoPtr<IArrayOf> arr;
    CArrayOf::New(EIID_IByte, bytesArray->GetLength(), (IArrayOf**)&arr);
    for (Int32 i = 0; i < bytesArray->GetLength(); i++) {
        AutoPtr<IByte> bt;
        CByte::New((*bytesArray)[i], (IByte**)&bt);
        arr->Put(i, bt.Get());
    }
    *fromIntValue = arr.Get();
    REFCOUNT_ADD(*fromIntValue)
    return NOERROR;
}


ECode CASN1Integer::GetId(
    /* [out] */ Int32* id)
{
    return ASN1Primitive::GetId(id);
}

ECode CASN1Integer::GetConstrId(
    /* [out] */ Int32* constrId)
{
    return ASN1Primitive::GetConstrId(constrId);
}

ECode CASN1Integer::Decode(
    /* [in] */ ArrayOf<Byte>* encoded,
    /* [out] */ IInterface** object)
{
    return ASN1Primitive::Decode(encoded, object);
}

ECode CASN1Integer::DecodeEx(
    /* [in] */ ArrayOf<Byte>* encoded,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 encodingLen,
    /* [out] */ IInterface** object)
{
    return ASN1Primitive::DecodeEx(encoded, offset, encodingLen, object);
}

ECode CASN1Integer::DecodeEx2(
    /* [in] */ IInputStream* is,
    /* [out] */ IInterface** object)
{
    return ASN1Primitive::DecodeEx2(is, object);
}

ECode CASN1Integer::Verify(
    /* [in] */ ArrayOf<Byte>* encoded)
{
    return ASN1Primitive::Verify(encoded);
}

ECode CASN1Integer::VerifyEx(
    /* [in] */ IInputStream* is)
{
    return ASN1Primitive::VerifyEx(is);
}

ECode CASN1Integer::Encode(
    /* [in] */ IInterface* object,
    /* [out, callee] */ ArrayOf<Byte>** encode)
{
    return ASN1Primitive::Encode(object, encode);
}

ECode CASN1Integer::DecodeEx3(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    bis->ReadInteger();

    if (((CBerInputStream*)bis)->mIsVerify) {
        return NOERROR;
    }
    return GetDecodedObject(bis, object);
}

ECode CASN1Integer::CheckTag(
    /* [in] */ Int32 identifier,
    /* [out] */ Boolean* checkTag)
{
    return ASN1Primitive::CheckTag(identifier, checkTag);
}

ECode CASN1Integer::GetDecodedObject(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)
    Int32 length, contentOffset;
    bis->GetLength(&length);
    bis->GetContentOffset(&contentOffset);
    AutoPtr<ArrayOf<Byte> > buffer, bytesEncoded = ArrayOf<Byte>::Alloc(length);
    bytesEncoded->Copy(0, buffer, contentOffset, length);
    AutoPtr<IArrayOf> arr;
    CArrayOf::New(EIID_IByte, length, (IArrayOf**)&arr);
    for (Int32 i = 0; i < length; i++) {
        AutoPtr<IByte> bt;
        CByte::New((*bytesEncoded)[i], (IByte**)&bt);
        arr->Put(i, bt.Get());
    }
    *object = arr.Get();
    REFCOUNT_ADD(*object);
    return NOERROR;
}

ECode CASN1Integer::EncodeASN(
    /* [in] */ IBerOutputStream* bos)
{
    return ASN1Primitive::EncodeASN(bos);
}

ECode CASN1Integer::EncodeContent(
    /* [in] */ IBerOutputStream* bos)
{
    return bos->EncodeInteger();
}

ECode CASN1Integer::SetEncodingContent(
    /* [in] */ IBerOutputStream* bos)
{
    AutoPtr<IInterface> content;
    bos->GetContent((IInterface**)&content);
    Int32 size;
    IArrayOf::Probe(content)->GetLength(&size);
    return bos->SetLength(size);
}

ECode CASN1Integer::GetEncodedLength(
    /* [in] */ IBerOutputStream* bos,
    /* [out] */ Int32* length)
{
    return ASN1Primitive::GetEncodedLength(bos, length);
}

ECode CASN1Integer::ToString(
    /* [out] */ String* result)
{
    return ASN1Primitive::ToString(result);
}

ECode CASN1Integer::constructor()
{
    return ASN1Primitive::Init(IASN1Constants::TAG_INTEGER);
}

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

