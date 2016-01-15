
#include "ASN1SequenceOf.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {


ECode ASN1SequenceOf::GetId(
    /* [out] */ Int32* id)
{
    return ASN1ValueCollection::GetId(id);
}

ECode ASN1SequenceOf::GetConstrId(
    /* [out] */ Int32* constrId)
{
    return ASN1ValueCollection::GetConstrId(constrId);
}

ECode ASN1SequenceOf::Decode(
    /* [in] */ ArrayOf<Byte>* encoded,
    /* [out] */ IInterface** object)
{
    return ASN1ValueCollection::Decode(encoded, object);
}

ECode ASN1SequenceOf::DecodeEx(
    /* [in] */ ArrayOf<Byte>* encoded,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 encodingLen,
    /* [out] */ IInterface** object)
{
    return ASN1ValueCollection::DecodeEx(encoded, offset, encodingLen, object);
}

ECode ASN1SequenceOf::DecodeEx2(
    /* [in] */ IInputStream* is,
    /* [out] */ IInterface** object)
{
    return ASN1ValueCollection::DecodeEx2(is, object);
}

ECode ASN1SequenceOf::Verify(
    /* [in] */ ArrayOf<Byte>* encoded)
{
    return ASN1ValueCollection::Verify(encoded);
}

ECode ASN1SequenceOf::VerifyEx(
    /* [in] */ IInputStream* is)
{
    return ASN1ValueCollection::VerifyEx(is);
}

ECode ASN1SequenceOf::Encode(
    /* [in] */ IInterface* object,
    /* [out, callee] */ ArrayOf<Byte>** encode)
{
    return ASN1ValueCollection::Encode(object, encode);
}

ECode ASN1SequenceOf::DecodeEx3(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    bis->ReadSequenceOf(this);

    if (((CBerInputStream*)bis)->mIsVerify) {
        return NOERROR;
    }
    return GetDecodedObject(bis, object);
}

ECode ASN1SequenceOf::CheckTag(
    /* [in] */ Int32 identifier,
    /* [out] */ Boolean* checkTag)
{
    return ASN1ValueCollection::CheckTag(identifier, checkTag);
}

ECode ASN1SequenceOf::GetDecodedObject(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    return ASN1ValueCollection::GetDecodedObject(bis, object);
}

ECode ASN1SequenceOf::EncodeASN(
    /* [in] */ IBerOutputStream* bos)
{
    return ASN1ValueCollection::EncodeASN(bos);
}

ECode ASN1SequenceOf::EncodeContent(
    /* [in] */ IBerOutputStream* bos)
{
    return bos->EncodeSequenceOf(this);
}

ECode ASN1SequenceOf::SetEncodingContent(
    /* [in] */ IBerOutputStream* bos)
{
    return bos->GetSequenceOfLength(this);
}

ECode ASN1SequenceOf::GetEncodedLength(
    /* [in] */ IBerOutputStream* bos,
    /* [out] */ Int32* length)
{
    return ASN1ValueCollection::GetEncodedLength(bos, length);
}

ECode ASN1SequenceOf::ToString(
    /* [out] */ String* result)
{
    return ASN1ValueCollection::ToString(result);
}

ASN1SequenceOf::ASN1SequenceOf(
    /* [in] */ IASN1Type* type)
{
    ASN1SequenceOf::Init(type);
}

ECode ASN1SequenceOf::SetType(
    /* [in] */ IASN1Type* type)
{
    mType = type;
    return NOERROR;
}

ECode ASN1SequenceOf::GetType(
    /* [out] */ IASN1Type** type)
{
    VALIDATE_NOT_NULL(type)
    *type = mType;
    REFCOUNT_ADD(*type)
    return NOERROR;
}

ECode ASN1SequenceOf::GetValues(
    /* [in] */ IInterface* object,
    /* [out] */ ICollection** values)
{
    VALIDATE_NOT_NULL(values)
    *values = ICollection::Probe(object);
    REFCOUNT_ADD(*values)
    return NOERROR;
}

ECode ASN1SequenceOf::Init(
    /* [in] */ IASN1Type* type)
{
    return ASN1ValueCollection::Init(IASN1Constants::TAG_SEQUENCE, type);
}

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

