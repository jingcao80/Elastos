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

#include "CASN1Set.h"
#include "CBerInputStream.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

CAR_OBJECT_IMPL(CASN1Set)
CAR_INTERFACE_IMPL(CASN1Set, ASN1TypeCollection, IASN1Set)
ECode CASN1Set::Decode(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    bis->ReadSet(this);

    if(((CBerInputStream*)bis)->mIsVerify){
        return NOERROR;
    }
    return GetDecodedObject(bis, object);
}

ECode CASN1Set::EncodeContent(
    /* [in] */ IBerOutputStream* bos)
{
    return bos->EncodeSet(this);
}

ECode CASN1Set::SetEncodingContent(
    /* [in] */ IBerOutputStream* bos)
{
    return bos->GetSetLength(this);
}

ECode CASN1Set::constructor(
    /* [in] */ ArrayOf<IASN1Type *>* type)
{
    return ASN1TypeCollection::constructor(IASN1Constants::TAG_SET, type);
}

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

