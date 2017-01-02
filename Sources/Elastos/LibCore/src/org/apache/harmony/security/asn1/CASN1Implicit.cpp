//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "CASN1Implicit.h"
#include "CBerInputStream.h"
#include "elastos/utility/logging/Slogger.h"

using Elastos::Utility::Logging::Slogger;

static const String TAG("CASN1Implicit");

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

const Int32 CASN1Implicit::TAGGING_PRIMITIVE = 0;
const Int32 CASN1Implicit::TAGGING_CONSTRUCTED = 1;
const Int32 CASN1Implicit::TAGGING_STRING = 2;

CAR_OBJECT_IMPL(CASN1Implicit)
CAR_INTERFACE_IMPL(CASN1Implicit, ASN1Type, IASN1Implicit)
ECode CASN1Implicit::Decode(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    Int32 tag = ((CBerInputStream*)bis)->mTag;
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
        ((CBerInputStream*)bis)->mTag = id;
    }
    else {
        ((CBerInputStream*)bis)->mTag = constrId;
    }
    AutoPtr<IInterface> content;
    mType->Decode(bis, (IInterface**)&content);
    ((CBerInputStream*)bis)->mContent = content;

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

ECode CASN1Implicit::constructor(
    /* [in] */ Int32 tagNumber,
    /* [in] */ IASN1Type* type)
{
    ASN1Type::constructor(IASN1Constants::CLASS_CONTEXTSPECIFIC, tagNumber);

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

ECode CASN1Implicit::EncodeContent(
    /* [in] */ IBerOutputStream* out)
{
    return mType->EncodeContent(out);
}

ECode CASN1Implicit::SetEncodingContent(
    /* [in] */ IBerOutputStream* out)
{
    return mType->SetEncodingContent(out);
}

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

