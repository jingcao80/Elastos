
#include "CASN1Boolean.h"
#include <cmdef.h>

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

static AutoPtr<IASN1Type> CASN1Boolean::sASN1 = InitStatic();

CAR_OBJECT_IMPL(CASN1Boolean)

AutoPtr<IASN1Type> CASN1Boolean::InitStatic()
{
    return new CASN1Boolean();
}

ECode CASN1Boolean::GetInstance(
    /* [out] */ IASN1Type** instance)
{
    VALIDATE_NOT_NULL(instance)
    *instance = sASN1;
    REFCOUNT_ADD(*instance)
    return NOERROR;
}

ECode CASN1Boolean::GetId(
    /* [out] */ Int32* id)
{
    return ASN1Primitive::GetId(id);
}

ECode CASN1Boolean::GetConstrId(
    /* [out] */ Int32* constrId)
{
    return ASN1Primitive::GetConstrId(constrId);
}

ECode CASN1Boolean::Decode(
    /* [in] */ ArrayOf<Byte>* encoded,
    /* [out] */ IInterface** object)
{
    return ASN1Primitive::Decode(encoded, object);
}

ECode CASN1Boolean::DecodeEx(
    /* [in] */ ArrayOf<Byte>* encoded,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 encodingLen,
    /* [out] */ IInterface** object)
{
    return ASN1Primitive::DecodeEx(encoded, offset, encodingLen, object);
}

ECode CASN1Boolean::DecodeEx2(
    /* [in] */ IInputStream* is,
    /* [out] */ IInterface** object)
{
    return ASN1Primitive::DecodeEx2(is, object);
}

ECode CASN1Boolean::Verify(
    /* [in] */ ArrayOf<Byte>* encoded)
{
    return ASN1Primitive::Verify(encoded);
}

ECode CASN1Boolean::VerifyEx(
    /* [in] */ IInputStream* is)
{
    return ASN1Primitive::VerifyEx(is);
}

ECode CASN1Boolean::Encode(
    /* [in] */ IInterface* object,
    /* [out, callee] */ ArrayOf<Byte>** encode)
{
    return ASN1Primitive::Encode(object, encode);
}

ECode CASN1Boolean::DecodeEx3(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    bis->ReadBoolean();

    if (((CBerInputStream*)bis)->mIsVerify) {
        return NOERROR;
    }
    return GetDecodedObject(bis);
}

ECode CASN1Boolean::CheckTag(
    /* [in] */ Int32 identifier,
    /* [out] */ Boolean* checkTag)
{
    return ASN1Primitive::CheckTag(identifier, checkTag);
}

ECode CASN1Boolean::GetDecodedObject(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    if (in.buffer[in.contentOffset] == 0) {
            return Boolean.FALSE;
        }
        return Boolean.TRUE;

    AutoPtr<ArrayOf<Byte> > buffer;
    bis->GetBuffer((ArrayOf<Byte>**)&buffer);
    Int32 contentOffset;
    bis->GetContentOffset(&contentOffset);
    Boolean val = TRUE;
    if ((*buffer)[contentOffset] == 0) {
        val = FALSE;
    }
    AutoPtr<IBoolean> ret;
    CBoolean::New(val, (IBoolean**)&ret);
    *object = ret.Get();
    REFCOUNT_ADD(*object)
    return NOERROR;
}

ECode CASN1Boolean::EncodeASN(
    /* [in] */ IBerOutputStream* bos)
{
    return ASN1Primitive::EncodeASN(bos);
}

ECode CASN1Boolean::EncodeContent(
    /* [in] */ IBerOutputStream* bos)
{
    return bos->EncodeBoolean();
}

ECode CASN1Boolean::SetEncodingContent(
    /* [in] */ IBerOutputStream* bos)
{
    bos->SetLength(1);
}

ECode CASN1Boolean::GetEncodedLength(
    /* [in] */ IBerOutputStream* bos,
    /* [out] */ Int32* length)
{
    return ASN1Primitive::GetEncodedLength(bos, length);
}

ECode CASN1Boolean::ToString(
    /* [out] */ String* result)
{
    return ASN1Primitive::ToString(result);
}

ECode CASN1Boolean::constructor()
{
    return ASN1Primitive::Init(IASN1Constants::TAG_BOOLEAN);
}

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

