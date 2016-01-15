
#include "ASN1BitString.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

AutoPtr<IASN1Type> ASN1BitString::sASN1 = Init();

AutoPtr<IASN1Type> ASN1BitString::Init()
{
    AutoPtr<CASN1BitString> bs;
    CASN1BitString::NewByFriend((CASN1BitString**)&bs);
    return bs.Get();
}

ECode ASN1BitString::GetInstance(
    /* [out] */ IASN1Type** instance)
{
    VALIDATE_NOT_NULL(instance)
    *instance = sASN1;
    REFCOUNT_ADD(*instance)
    return NOERROR;
}

ECode ASN1BitString::GetId(
    /* [out] */ Int32* id)
{
    return ASN1StringType::GetId(id);
}

ECode ASN1BitString::GetConstrId(
    /* [out] */ Int32* constrId)
{
    return ASN1StringType::GetConstrId(constrId);
}

ECode ASN1BitString::Decode(
    /* [in] */ ArrayOf<Byte>* encoded,
    /* [out] */ IInterface** object)
{
    return ASN1StringType::Decode(encoded, object);
}

ECode ASN1BitString::DecodeEx(
    /* [in] */ ArrayOf<Byte>* encoded,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 encodingLen,
    /* [out] */ IInterface** object)
{
    return ASN1StringType::DecodeEx(encoded, offset, encodingLen, object);
}

ECode ASN1BitString::DecodeEx2(
    /* [in] */ IInputStream* is,
    /* [out] */ IInterface** object)
{
    return ASN1StringType::DecodeEx2(is, object);
}

ECode ASN1BitString::Verify(
    /* [in] */ ArrayOf<Byte>* encoded)
{
    return ASN1StringType::Verify(encoded);
}

ECode ASN1BitString::VerifyEx(
    /* [in] */ IInputStream* is)
{
    return ASN1StringType::VerifyEx(is);
}

ECode ASN1BitString::Encode(
    /* [in] */ IInterface* object,
    /* [out, callee] */ ArrayOf<Byte>** encode)
{
    return ASN1StringType::Encode(object, encode);
}

ECode ASN1BitString::DecodeEx3(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    bis->ReadBitString();

    if (((CBerInputStream*)bis)->mIsVerify) {
        return NOERROR;
    }
    return GetDecodedObject(bis, object);
}

ECode ASN1BitString::CheckTag(
    /* [in] */ Int32 identifier,
    /* [out] */ Boolean* checkTag)
{
    return ASN1StringType::CheckTag(identifier, checkTag);
}

ECode ASN1BitString::GetDecodedObject(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)
    Int32 length, contentOffset;
    bis->GetLength(&length);
    AutoPtr<ArrayOf<Byte> > buffer, bytes = ArrayOf<Byte>::Alloc(length - 1);
    bis->GetBuffer((ArrayOf<Byte>**)&buffer);
    bis->GetContentOffset(&contentOffset);
    bytes->Copy(0, buffer, contentOffset + 1, length - 1);
    AutoPtr<IBitString> obj;
    CBitString::New(bytes, (*buffer)[contentOffset], (IBitString**)&obj);
    *object = obj.Get();
    REFCOUNT_ADD(*object)
    return NOERROR;
}

ECode ASN1BitString::EncodeASN(
    /* [in] */ IBerOutputStream* bos)
{
    return ASN1StringType::EncodeASN(bos);
}

ECode ASN1BitString::EncodeContent(
    /* [in] */ IBerOutputStream *bos)
{
    return bos->EncodeBitString();
}

ECode ASN1BitString::SetEncodingContent(
    /* [in] */ IBerOutputStream* bos)
{
    AutoPtr<IInterface> tmp;
    bos->GetContent((IInterface**)&tmp);
    AutoPtr<IBitString> bs = IBitString::Probe(tmp);
    AutoPtr<ArrayOf<Byte> > bytes;
    bs->GetBytes((ArrayOf<Byte>**)&bytes);
    return bos->SetLength(bytes->GetLength() + 1);
}

ECode ASN1BitString::GetEncodedLength(
    /* [in] */ IBerOutputStream* bos,
    /* [out] */ Int32* length)
{
    return ASN1StringType::GetEncodedLength(bos, length);
}

ECode ASN1BitString::ToString(
    /* [out] */ String* result)
{
    return ASN1StringType::ToString(result);
}

ECode ASN1BitString::constructor()
{
    return ASN1StringType::Init(IASN1Constants::TAG_BITSTRING);
}

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

