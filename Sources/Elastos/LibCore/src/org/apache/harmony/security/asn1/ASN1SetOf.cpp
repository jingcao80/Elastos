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

#include "ASN1SetOf.h"
#include "CBerInputStream.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

CAR_INTERFACE_IMPL(ASN1SetOf, ASN1ValueCollection, IASN1SetOf)
ECode ASN1SetOf::Decode(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** ret)
{
    bis->ReadSetOf(this);

    if (((CBerInputStream*)bis)->mIsVerify) {
        return NOERROR;
    }
    return GetDecodedObject(bis, ret);
}

ECode ASN1SetOf::EncodeContent(
    /* [in] */ IBerOutputStream* bos)
{
    return bos->EncodeSetOf(this);
}

ECode ASN1SetOf::SetEncodingContent(
    /* [in] */ IBerOutputStream* bos)
{
    return bos->GetSetOfLength(this);
}

ECode ASN1SetOf::constructor(
    /* [in] */ IASN1Type* type)
{
    return ASN1ValueCollection::constructor(IASN1Constants::TAG_SETOF, type);
}

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

