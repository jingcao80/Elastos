
#include "CASN1BitString.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

ECode CASN1BitString::GetInstance(
    /* [out] */ IASN1Type** instance)
{
    return ASN1BitString::GetInstance(instance);
}

ECode CASN1BitString::GetId(
    /* [out] */ Int32* id)
{
    return ASN1BitString::GetId(id);
}

ECode CASN1BitString::GetConstrId(
    /* [out] */ Int32* constrId)
{
    return ASN1BitString::GetConstrId(constrId);
}

ECode CASN1BitString::Decode(
    /* [in] */ ArrayOf<Byte>* encoded,
    /* [out] */ IInterface** object)
{
    return ASN1BitString::Decode(encoded, object);
}

ECode CASN1BitString::DecodeEx(
    /* [in] */ ArrayOf<Byte>* encoded,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 encodingLen,
    /* [out] */ IInterface** object)
{
    return ASN1BitString::DecodeEx(encoded, offset, encodingLen, object);
}

ECode CASN1BitString::DecodeEx2(
    /* [in] */ IInputStream* is,
    /* [out] */ IInterface** object)
{
    return ASN1BitString::DecodeEx2(is, object);
}

ECode CASN1BitString::Verify(
    /* [in] */ ArrayOf<Byte>* encoded)
{
    return ASN1BitString::Verify(encoded);
}

ECode CASN1BitString::VerifyEx(
    /* [in] */ IInputStream* is)
{
    return ASN1BitString::VerifyEx(is);
}

ECode CASN1BitString::Encode(
    /* [in] */ IInterface* object,
    /* [out, callee] */ ArrayOf<Byte>** encode)
{
    return ASN1BitString::Encode(object, encode);
}

ECode CASN1BitString::DecodeEx3(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    return ASN1BitString::DecodeEx3(bis, object);
}

ECode CASN1BitString::CheckTag(
    /* [in] */ Int32 identifier,
    /* [out] */ Boolean* checkTag)
{
    return ASN1BitString::CheckTag(identifier, checkTag);
}

ECode CASN1BitString::GetDecodedObject(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    return ASN1BitString::GetDecodedObject(bis, object);
}

ECode CASN1BitString::EncodeASN(
    /* [in] */ IBerOutputStream* bos)
{
    return ASN1BitString::EncodeASN(bos);
}

ECode CASN1BitString::EncodeContent(
    /* [in] */ IBerOutputStream *bos)
{
    return ASN1BitString::EncodeContent(bos);
}

ECode CASN1BitString::SetEncodingContent(
    /* [in] */ IBerOutputStream* bos)
{
    return ASN1BitString::SetEncodingContent(bos);
}

ECode CASN1BitString::GetEncodedLength(
    /* [in] */ IBerOutputStream* bos,
    /* [out] */ Int32* length)
{
    return ASN1BitString::GetEncodedLength(bos, length);
}

ECode CASN1BitString::ToString(
    /* [out] */ String* result)
{
    return ASN1BitString::ToString(result);
}

ECode CASN1BitString::constructor()
{
    return ASN1StringType::Init(IASN1Constants::TAG_BITSTRING);
}

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

