
#include "CASN1Enumerated.h"

using Elastos::Utility::Arrays;
using Elastos::Core::IArrayOf;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

AutoPtr<IASN1Enumerated> CASN1Enumerated::sASN1 = Init();

CAR_OBJECT_IMPL(CASN1Enumerated)

CAR_INTERFACE_IMPL(CASN1Enumerated, ASN1Primitive, IASN1Enumerated)

AutoPtr<IASN1Enumerated> CASN1Enumerated::Init()
{
    return new CASN1Enumerated();
}

ECode CASN1Enumerated::GetInstance(
    /* [out] */ IASN1Enumerated** instance)
{
    VALIDATE_NOT_NULL(instance)
    *instance = sASN1;
    REFCOUNT_ADD(*instance)
    return NOERROR;
}

ECode CASN1Enumerated::DecodeEx3(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    bis->ReadEnumerated();

    if (((CBerInputStream*)bis)->mIsVerify) {
        return NOERROR;
    }
    return GetDecodedObject(bis, object);
}

ECode CASN1Enumerated::GetDecodedObject(
    /* [in] */ IBerInputStream* bis,
    /* [out, callee] */ ArrayOf<Byte>** object)
{
    AutoPtr<IArrays> arr;
    CArrays::Acquiresingleton((IArrays**)&arr);
    AutoPtr<ArrayOf<Byte> > buffer;
    bis->GetBuffer((ArrayOf<Byte>**)&buffer);
    Int32 contentOffset, length;
    bis->GetContentOffset(&contentOffset);
    bis->GetLength(&length);
    return arr->CopyOfRangeByte(buffer, contentOffset, contentOffset + length, object);
}

ECode CASN1Enumerated::EncodeContent(
    /* [in] */ IBerOutputStream* bos)
{
    return bos->EncodeInteger();
}

ECode CASN1Enumerated::SetEncodingContent(
    /* [in] */ IBerOutputStream* bos)
{
    AutoPtr<IInterface> content;
    bos->GetContent((IInterface**)&content);
    Int32 length;
    IArrayOf::Probe(content)->GetLength(&length);
    return bos->SetLength(length);
}

ECode CASN1Enumerated::GetId(
    /* [out] */ Int32* id)
{
    return ASN1Primitive::GetId(id);
}

ECode CASN1Enumerated::GetConstrId(
    /* [out] */ Int32* constrId)
{
    return ASN1Primitive::GetConstrId(constrId);
}

ECode CASN1Enumerated::Decode(
    /* [in] */ ArrayOf<Byte>* encoded,
    /* [out] */ IInterface** object)
{
    return ASN1Primitive::Decode(encoded, object);
}

ECode CASN1Enumerated::DecodeEx(
    /* [in] */ ArrayOf<Byte>* encoded,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 encodingLen,
    /* [out] */ IInterface** object)
{
    return ASN1Primitive::DecodeEx(encoded, offset, encodingLen, object);
}

ECode CASN1Enumerated::DecodeEx2(
    /* [in] */ IInputStream* is,
    /* [out] */ IInterface** object)
{
    return ASN1Primitive::DecodeEx2(is, object);
}

ECode CASN1Enumerated::Verify(
    /* [in] */ ArrayOf<Byte>* encoded)
{
    return ASN1Primitive::Verify(encoded);
}

ECode CASN1Enumerated::VerifyEx(
    /* [in] */ IInputStream* is)
{
    return ASN1Primitive::VerifyEx(is);
}

ECode CASN1Enumerated::Encode(
    /* [in] */ IInterface* object,
    /* [out, callee] */ ArrayOf<Byte>** encode)
{
    return ASN1Primitive::Encode(object, encode);
}

ECode CASN1Enumerated::CheckTag(
    /* [in] */ Int32 identifier,
    /* [out] */ Boolean* checkTag)
{
    return ASN1Primitive::CheckTag(identifier, checkTag);
}

ECode CASN1Enumerated::EncodeASN(
    /* [in] */ IBerOutputStream* bos)
{
    return ASN1Primitive::EncodeASN(bos);
}

ECode CASN1Enumerated::GetEncodedLength(
    /* [in] */ IBerOutputStream* bos,
    /* [out] */ Int32* length)
{
    return ASN1Primitive::GetEncodedLength(bos, length);
}

ECode CASN1Enumerated::ToString(
    /* [out] */ String* result)
{
    return ASN1Primitive::ToString(result);
}

ECode CASN1Enumerated::constructor()
{
    return ASN1Primitive::Init(IASN1Constants::TAG_ENUM);
}

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

