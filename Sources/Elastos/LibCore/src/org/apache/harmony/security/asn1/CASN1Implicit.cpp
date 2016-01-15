
#include "CASN1Implicit.h"
#include <elastos/Slogger.h>
#include <cmdef.h>

using Elastos::Utility::Logging::Slogger;

static const String TAG("CASN1Implicit");

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {


ECode CASN1Implicit::GetId(
    /* [out] */ Int32* id)
{
    return ASN1Type::GetId(id);
}

ECode CASN1Implicit::GetConstrId(
    /* [out] */ Int32* constrId)
{
    return ASN1Type::GetConstrId(constrId);
}

ECode CASN1Implicit::Decode(
    /* [in] */ ArrayOf<Byte>* encoded,
    /* [out] */ IInterface** object)
{
    return ASN1Type::Decode(encoded, object);
}

ECode CASN1Implicit::DecodeEx(
    /* [in] */ ArrayOf<Byte>* encoded,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 encodingLen,
    /* [out] */ IInterface** object)
{
    return ASN1Type::DecodeEx(encoded, offset, encodingLen, object);
}

ECode CASN1Implicit::DecodeEx2(
    /* [in] */ IInputStream* is,
    /* [out] */ IInterface** object)
{
    return ASN1Type::DecodeEx2(is, object);
}

ECode CASN1Implicit::Verify(
    /* [in] */ ArrayOf<Byte>* encoded)
{
    return ASN1Type::Verify(encoded);
}

ECode CASN1Implicit::VerifyEx(
    /* [in] */ IInputStream* is)
{
    return ASN1Type::VerifyEx(is);
}

ECode CASN1Implicit::Encode(
    /* [in] */ IInterface* object,
    /* [out, callee] */ ArrayOf<Byte>** encode)
{
    return ASN1Type::Encode(object, encode);
}

ECode CASN1Implicit::DecodeEx3(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    Int32 tag;
    bis->GetTag(&tag);
    Boolean chk;
    if (CheckTag(tag, &chk), !chk) {
        // FIXME need look for tagging type
        Int32 tagOffset;
        bis->GetTagOffset(&tagOffset);
        String msg;
        msg.AppendFormat("ASN.1 implicitly tagged type expected at "
            "[%x]. Expected tag: %x, but got %x", tagOffset, mId, tag);
        Slogger::E(TAG, msg);
        return E_ASN1_EXCEPTION;
    }

    // substitute identifier for further decoding
    Int32 id, constrId;
    mType->GetId(&id);
    mType->GetConstrId(&constrId);
    if (mId == tag) {
        bis->SetTag(id);
    } else {
        bis->SetTag(constrId);
    }
    AutoPtr<IInterface> content;
    mType->DecodeEx3(bis, (IInterface**)&content);
    bis->SetContent(content);

    if (((CBerInputStream*)bis)->mIsVerify) {
        return NOERROR;
    }
    return GetDecodedObject(bis, object);
}

ECode CASN1Implicit::CheckTag(
    /* [in] */ Int32 identifier,
    /* [out] */ Boolean* checkTag)
{
    VALIDATE_NOT_NULL(checkTag)
    switch (mTaggingType) {
        case TAGGING_PRIMITIVE:
            *checkTag = mId == identifier;
            break;
        case TAGGING_CONSTRUCTED:
            *checkTag = mConstrId == identifier;
            break;
        default: // TAGGING_STRING
            *checkTag = mId == identifier || mConstrId == identifier;
    }
    return NOERROR;
}

ECode CASN1Implicit::GetDecodedObject(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    return ASN1Type::GetDecodedObject(bis, object);
}

ECode CASN1Implicit::EncodeASN(
    /* [in] */ IBerOutputStream* bos)
{
    //FIXME need another way for specifying identifier to be encoded
    if (mTaggingType == TAGGING_CONSTRUCTED) {
        bos->EncodeTag(mConstrId);
    } else {
        bos->EncodeTag(mId);
    }
    return EncodeContent(bos);
}

ECode CASN1Implicit::EncodeContent(
    /* [in] */ IBerOutputStream* bos)
{
   return mType->EncodeContent(bos);
}

ECode CASN1Implicit::SetEncodingContent(
    /* [in] */ IBerOutputStream* bos)
{
    return mType->SetEncodingContent(bos);
}

ECode CASN1Implicit::GetEncodedLength(
    /* [in] */ IBerOutputStream* bos,
    /* [out] */ Int32* length)
{
    return ASN1Type::GetEncodedLength(bos, length);
}

ECode CASN1Implicit::ToString(
    /* [out] */ String* result)
{
    return ASN1Type::ToString(result);
}

ECode CASN1Implicit::constructor(
    /* [in] */ Int32 tagNumber,
    /* [in] */ IASN1Type* type)
{
    ASN1Type::Init(IASN1Constants::CLASS_CONTEXTSPECIFIC, tagNumber);

    if ((IASN1Choice::Probe(type)) || (IASN1Any::Probe(type))) {
        // According to X.680:
        // 'The IMPLICIT alternative shall not be used if the type
        // defined by "Type" is an untagged choice type or an
        // untagged open type'
        Slogger::E(TAG, "Implicit tagging can not be used for ASN.1 ANY or CHOICE type");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mType = type;
    Int32 id, constrId;
    type->GetId(&id);
    type->GetConstrId(&constrId);
    Boolean chkTag, chkConstrId;
    if (type->CheckTag(id, &chkTag), chkTag) {
        if (type->CheckTag(constrId, &chkConstrId), chkConstrId) {
            // the base encoding can be primitive ot constructed
            // use both encodings
            mTaggingType = TAGGING_STRING;
        } else {
            // if the base encoding is primitive use primitive encoding
            mTaggingType = TAGGING_PRIMITIVE;
        }
    } else {
        // if the base encoding is constructed use constructed encoding
        mTaggingType = TAGGING_CONSTRUCTED;
    }
    return NOERROR;
}

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

