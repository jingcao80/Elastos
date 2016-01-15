
#include "CASN1SequenceOf.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {


ECode CASN1SequenceOf::GetId(
    /* [out] */ Int32 *id)
{
    return ASN1ValueCollection::GetId(id);
}

ECode CASN1SequenceOf::GetConstrId(
    /* [out] */ Int32 *constrId)
{
    return ASN1ValueCollection::GetConstrId(constrId);
}

ECode CASN1SequenceOf::Decode(
    /* [in] */ ArrayOf<Byte>* encoded,
    /* [out] */ IInterface** object)
{
    return ASN1ValueCollection::Decode(encoded, object);
}

ECode CASN1SequenceOf::DecodeEx(
    /* [in] */ ArrayOf<Byte>* encoded,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 encodingLen,
    /* [out] */ IInterface** object)
{
    return ASN1ValueCollection::DecodeEx(encoded, offset, encodingLen, object);
}

ECode CASN1SequenceOf::DecodeEx2(
    /* [in] */ IInputStream* is,
    /* [out] */ IInterface** object)
{
    return ASN1ValueCollection::DecodeEx2(is, object);
}

ECode CASN1SequenceOf::Verify(
    /* [in] */ ArrayOf<Byte>* encoded)
{
    return ASN1ValueCollection::Verify(encoded);
}

ECode CASN1SequenceOf::VerifyEx(
    /* [in] */ IInputStream* is)
{
    return ASN1ValueCollection::VerifyEx(is);
}

ECode CASN1SequenceOf::Encode(
    /* [in] */ IInterface* object,
    /* [out, callee] */ ArrayOf<Byte>** encode)
{
    return ASN1ValueCollection::Encode(object, encode);
}

ECode CASN1SequenceOf::DecodeEx3(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    bis->ReadSequenceOf(this);

    if (((CBerInputStream*)bis)->mIsVerify) {
        return NOERROR;
    }
    return GetDecodedObject(bis, object);
}

ECode CASN1SequenceOf::CheckTag(
    /* [in] */ Int32 identifier,
    /* [out] */ Boolean* checkTag)
{
    return ASN1ValueCollection::CheckTag(identifier, checkTag);
}

ECode CASN1SequenceOf::GetDecodedObject(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    return ASN1ValueCollection::GetDecodedObject(bis, object);
}

ECode CASN1SequenceOf::EncodeASN(
    /* [in] */ IBerOutputStream* bos)
{
    return ASN1ValueCollection::EncodeASN(bos);
}

ECode CASN1SequenceOf::EncodeContent(
    /* [in] */ IBerOutputStream* bos)
{
    bos->EncodeSequenceOf(this);
}

ECode CASN1SequenceOf::SetEncodingContent(
    /* [in] */ IBerOutputStream* bos)
{
    bos->GetSequenceOfLength(this);
}

ECode CASN1SequenceOf::GetEncodedLength(
    /* [in] */ IBerOutputStream* bos,
    /* [out] */ Int32* length)
{
    return ASN1ValueCollection::GetEncodedLength(bos, length);
}

ECode CASN1SequenceOf::ToString(
    /* [out] */ String* result)
{
    return ASN1ValueCollection::ToString(result);
}

ECode CASN1SequenceOf::SetType(
    /* [in] */ IASN1Type* type)
{
    return ASN1ValueCollection::SetType(type);
}

ECode CASN1SequenceOf::GetType(
    /* [out] */ IASN1Type** type)
{
    return ASN1ValueCollection::GetType(type);
}

ECode CASN1SequenceOf::GetValues(
    /* [in] */ IInterface* object,
    /* [out] */ ICollection** values)
{
    return ASN1ValueCollection::GetValues(object, values);
}

ECode CASN1SequenceOf::constructor(
    /* [in] */ IASN1Type* asn1Type)
{
    return ASN1ValueCollection::Init(IASN1Constants::TAG_SEQUENCE, asn1Type);
}

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org