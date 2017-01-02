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

#include "ASN1Primitive.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

ASN1Primitive::ASN1Primitive()
{}

ECode ASN1Primitive::constructor(
    /* [in] */ Int32 tagNumber)
{
    return ASN1Type::constructor(tagNumber);
}

/**
 * Tests provided identifier.
 *
 * @param identifier identifier to be verified
 * @return true if identifier correspond to primitive identifier of this
 *     ASN.1 type, otherwise false
 */
ECode ASN1Primitive::CheckTag(
    /* [in] */ Int32 identifier,
    /* [ou] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mId == identifier;
    return NOERROR;
}

ECode ASN1Primitive::EncodeASN(
    /* [in] */ IBerOutputStream* out)
{
    out->EncodeTag(mId);
    return EncodeContent(out);
}

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org