
#include "CASN1Any.h"
#include <cmdef.h>

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

AutoPtr<IASN1Any> CASN1Any::sASN1 = Init();

CAR_OBJECT_IMPL(CASN1Any)

AutoPtr<IASN1Any> CASN1Any::Init()
{
    return new CASN1Any();
}

ECode CASN1Any::GetInstance(
    /* [out] */ IASN1Any** instance)
{
    VALIDATE_NOT_NULL(instance)
    *instance = sASN1;
    REFCOUNT_ADD(*instance)
    return NOERROR;
}

ECode CASN1Any::GetId(
    /* [out] */ Int32* id)
{
    return ASN1Type::GetId(id);
}

ECode CASN1Any::GetConstrId(
    /* [out] */ Int32* constrId)
{
    return ASN1Type::GetConstrId(constrId);
}

ECode CASN1Any::Decode(
    /* [in] */ ArrayOf<Byte>* encoded,
    /* [out] */ IInterface** object)
{
    return ASN1Type::Decode(encoded, object);
}

ECode CASN1Any::DecodeEx(
    /* [in] */ ArrayOf<Byte> *encoded,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 encodingLen,
    /* [out] */ IInterface **object)
{
    return ASN1Type::DecodeEx(encoded, offset, encodingLen, object);
}

ECode CASN1Any::DecodeEx2(
    /* [in] */ IInputStream* is,
    /* [out] */ IInterface** object)
{
    return ASN1Type::DecodeEx2(is, object);
}

ECode CASN1Any::Verify(
    /* [in] */ ArrayOf<Byte>* encoded)
{
    return ASN1Type::Verify(encoded);
}

ECode CASN1Any::VerifyEx(
    /* [in] */ IInputStream* is)
{
    return ASN1Type::VerifyEx(is);
}

ECode CASN1Any::Encode(
    /* [in] */ IInterface* object,
    /* [out, callee] */ ArrayOf<Byte>** encode)
{
    return ASN1Type::Encode(object, encode);
}

ECode CASN1Any::DecodeEx3(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)
    // only read content, doesn't check it
    bis->ReadContent();

    if (((CBerInputStream*)bis)->mIsVerify) {
        return NOERROR;
    }
    return GetDecodedObject(bis, object);
}

ECode CASN1Any::CheckTag(
    /* [in] */ Int32 identifier,
    /* [out] */ Boolean* checkTag)
{
    VALIDATE_NOT_NULL(checkTag)
    *checkTag = TRUE; //all tags are OK
    return NOERROR;
}

ECode CASN1Any::GetDecodedObject(
    /* [in] */ IBerInputStream* bis,
    /* [out, callee] */ ArrayOf<Byte>** object)
{
    VALIDATE_NOT_NULL(object)
    Int32 offset, tagOffset;
    bis->GetOffset(&offset);
    bis->GetTagOffset(tagOffset);
    AutoPtr<ArrayOf<Byte> > bytesEncoded = ArrayOf<Byte>::Alloc(offset- tagOffset);
    bytesEncoded->Copy(0, bis, tagOffset, bytesEncoded->GetLength());
    *object = bytesEncoded;
    REFCOUNT_ADD(*object)
    return NOERROR;
}

ECode CASN1Any::EncodeASN(
    /* [in] */ IBerOutputStream* bos)
{
    return bos->EncodeAny();
}

ECode CASN1Any::EncodeContent(
    /* [in] */ IBerOutputStream* bos)
{
    return bos->EncodeAny();
}

ECode CASN1Any::SetEncodingContent(
    /* [in] */ IBerOutputStream* bos)
{
    AutoPtr<ArrayOf<Byte> > content;
    bos->GetContent((ArrayOf<Byte>**)&content);
    return bos->SetLength(content->GetLength());
}

ECode CASN1Any::GetEncodedLength(
    /* [in] */ IBerOutputStream* bos,
    /* [out] */ Int32* length)
{
    return bos->GetLength(length);
}

ECode CASN1Any::ToString(
    /* [out] */ String* result)
{
    return ASN1Type::ToString(result);
}

ECode CASN1Any::constructor()
{
    return ASN1Type::Init(IASN1Constants::TAG_ANY);
}

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

