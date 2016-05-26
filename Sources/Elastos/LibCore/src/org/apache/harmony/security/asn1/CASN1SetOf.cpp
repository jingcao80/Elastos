
#include "CASN1SetOf.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

CAR_OBJECT_IMPL(CASN1SetOf)

ECode CASN1SetOf::GetId(
    /* [out] */ Int32* id)
{
    return ASN1ValueCollection::GetId(id);
}

ECode CASN1SetOf::GetConstrId(
    /* [out] */ Int32* constrId)
{
    return ASN1ValueCollection::GetConstrId(constrId);
}

ECode CASN1SetOf::Decode(
    /* [in] */ ArrayOf<Byte>* encoded,
    /* [out] */ IInterface** object)
{
    return ASN1ValueCollection::Decode(encoded, object);
}

ECode CASN1SetOf::DecodeEx(
    /* [in] */ ArrayOf<Byte>* encoded,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 encodingLen,
    /* [out] */ IInterface** object)
{
    return ASN1ValueCollection::DecodeEx(encoded, offset, encodingLen, object);
}

ECode CASN1SetOf::DecodeEx2(
    /* [in] */ IInputStream* is,
    /* [out] */ IInterface** object)
{
    return ASN1ValueCollection::DecodeEx2(is, object);
}

ECode CASN1SetOf::Verify(
    /* [in] */ ArrayOf<Byte>* encoded)
{
    return ASN1ValueCollection::Verify(encoded);
}

ECode CASN1SetOf::VerifyEx(
    /* [in] */ IInputStream* is)
{
    return ASN1ValueCollection::VerifyEx(is);
}

ECode CASN1SetOf::Encode(
    /* [in] */ IInterface* object,
    /* [out, callee] */ ArrayOf<Byte>** encode)
{
    return ASN1ValueCollection::Encode;
}

ECode CASN1SetOf::DecodeEx3(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    return ASN1SetOf::Decode(bis, object);
}

ECode CASN1SetOf::CheckTag(
    /* [in] */ Int32 identifier,
    /* [out] */ Boolean* checkTag)
{
    return ASN1ValueCollection::CheckTag(identifier, checkTag);
}

ECode CASN1SetOf::GetDecodedObject(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    return ASN1ValueCollection::GetDecodedObject(bis, object);
}

ECode CASN1SetOf::EncodeASN(
    /* [in] */ IBerOutputStream* bos)
{
    return ASN1ValueCollection::EncodeASN(bos);
}

ECode CASN1SetOf::EncodeContent(
    /* [in] */ IBerOutputStream* bos)
{
    return ASN1SetOf::EncodeContent(bos);
}

ECode CASN1SetOf::SetEncodingContent(
    /* [in] */ IBerOutputStream* bos)
{
    return ASN1SetOf::SetEncodingContent(bos);
}

ECode CASN1SetOf::GetEncodedLength(
    /* [in] */ IBerOutputStream* bos,
    /* [out] */ Int32* length)
{
    return ASN1ValueCollection::GetEncodedLength(bos, length);
}

ECode CASN1SetOf::ToString(
    /* [out] */ String* result)
{
    return ASN1ValueCollection::ToString(result);
}

ECode CASN1SetOf::SetType(
    /* [in] */ IASN1Type* type)
{
    return ASN1ValueCollection::SetType(type);
}

ECode CASN1SetOf::GetType(
    /* [out] */ IASN1Type** type)
{
    return ASN1ValueCollection::GetType(type);
}

ECode CASN1SetOf::GetValues(
    /* [in] */ IInterface* object,
    /* [out] */ ICollection** values)
{
    return ASN1ValueCollection::GetValues(object, values);
}

ECode CASN1SetOf::constructor(
    /* [in] */ IASN1Type* type)
{
    return ASN1SetOf::Init(IASN1Constants::TAG_SETOF, type);
}

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

