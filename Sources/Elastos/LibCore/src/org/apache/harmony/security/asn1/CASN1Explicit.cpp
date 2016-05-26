
#include "CASN1Explicit.h"
#include <elastos/Slogger.h>
#include <cmdef.h>

using Elastos::Utility::Logging::Slogger;

static const String TAG("CASN1Explicit");

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

CAR_OBJECT_IMPL(CASN1Explicit)

CAR_INTERFACE_IMPL(CASN1Explicit, ASN1Constructed, IASN1Explicit)

ECode CASN1Explicit::GetType(
    /* [out] */ IASN1Type** type)
{
    VALIDATE_NOT_NULL(type)
    *type = mType;
    REFCOUNT_ADD(*type)
    return NOERROR;
}

ECode CASN1Explicit::GetId(
    /* [out] */ Int32* id)
{
    return ASN1Constructed::GetId(id);
}

ECode CASN1Explicit::GetConstrId(
    /* [out] */ Int32* constrId)
{
    return ASN1Constructed::GetConstrId(constrId);
}

ECode CASN1Explicit::Decode(
    /* [in] */ ArrayOf<Byte>* encoded,
    /* [out] */ IInterface** object)
{
    return ASN1Constructed::Decode(encoded, object);
}

ECode CASN1Explicit::DecodeEx(
    /* [in] */ ArrayOf<Byte>* encoded,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 encodingLen,
    /* [out] */ IInterface** object)
{
    return ASN1Constructed::DecodeEx(encoded, offset, encodingLen, object);
}

ECode CASN1Explicit::DecodeEx2(
    /* [in] */ IInputStream* is,
    /* [out] */ IInterface** object)
{
    return ASN1Constructed::DecodeEx2(is, object);
}

ECode CASN1Explicit::Verify(
    /* [in] */ ArrayOf<Byte>* encoded)
{
    return ASN1Constructed::Verify(encoded);
}

ECode CASN1Explicit::VerifyEx(
    /* [in] */ IInputStream* is)
{
    return ASN1Constructed::VerifyEx(is);
}

ECode CASN1Explicit::Encode(
    /* [in] */ IInterface* object,
    /* [out, callee] */ ArrayOf<Byte>** encode)
{
    return ASN1Constructed::Encode(object, encode);
}

ECode CASN1Explicit::DecodeEx3(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    Int32 tag;
    bis->GetTag(&tag);
    if (mConstrId != tag) {
        String msg;
        Int32 tagOffset;
        bis->GetTagOffset(&tagOffset);
        msg.AppendFormat("ASN.1 explicitly tagged type is expected at [%d]"
            ". Expected tag: 0x%x, but encountered tag %x", tagOffset, mConstrId, tag);
        Slogger::E(TAG, msg);
        return E_ASN1_EXCEPTION;
    }

    bis->Next();
    AutoPtr<IInterface> content;

    mType->DecodeEx3(bis, (IInterface**)&content);
    bis->SetContent(content);

    if (((CBerInputStream*)bis)->mIsVerify) {
        return NOERROR;
    }
    return GetDecodedObject(bis, object);
}

ECode CASN1Explicit::CheckTag(
    /* [in] */ Int32 identifier,
    /* [out] */ Boolean* checkTag)
{
    return ASN1Constructed::CheckTag(identifier, checkTag);
}

ECode CASN1Explicit::GetDecodedObject(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    return ASN1Constructed::GetDecodedObject(bis, object);
}

ECode CASN1Explicit::EncodeASN(
    /* [in] */ IBerOutputStream* bos)
{
    return ASN1Constructed::EncodeASN(bos);
}

ECode CASN1Explicit::EncodeContent(
    /* [in] */ OIBerOutputStream* bos)
{
    return bos->EncodeExplicit(this);
}

ECode CASN1Explicit::SetEncodingContent(
    /* [in] */ IBerOutputStream* bos)
{
    return bos->GetExplicitLength(this);
}

ECode CASN1Explicit::GetEncodedLength(
    /* [in] */ IBerOutputStream* bos,
    /* [out] */ Int32* length)
{
    return ASN1Constructed::GetEncodedLength(bos, length);
}

ECode CASN1Explicit::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    ASN1Constructed::ToString(result);
    String tmp;
    mType->ToString(&tmp);
    return result->AppendFormat(" for type %s", tmp.string());
}

ECode CASN1Explicit::constructor(
    /* [in] */ Int32 tagNumber,
    /* [in] */ IASN1Type* type)
{
    return constructor(IASN1Constants::CLASS_CONTEXTSPECIFIC,
        tagNumber, type);
}

ECode CASN1Explicit::constructor(
    /* [in] */ Int32 tagClass,
    /* [in] */ Int32 tagNumber,
    /* [in] */ IASN1Type* type)
{
    ASN1Constructed::Init(tagClass, tagNumber);
    mType = type;
}

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

