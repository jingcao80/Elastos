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

#include "coredef.h"
#include "ASN1Constructed.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

ECode ASN1Constructed::constructor(
    /* [in] */ Int32 tagNumber)
{
    return ASN1Type::constructor(CLASS_UNIVERSAL, tagNumber);
}

ECode ASN1Constructed::constructor(
    /* [in] */ Int32 tagClass,
    /* [in] */ Int32 tagNumber)
{
    return ASN1Type::constructor(tagClass, tagNumber);
}

/**
 * Tests provided identifier.
 *
 * @param identifier - identifier to be verified
 * @return - true if identifier correspond to constructed identifier of
 *           this ASN.1 type, otherwise false
 */
ECode ASN1Constructed::CheckTag(
    /* [in] */ Int32 identifier,
    /* [ou] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mConstrId == identifier;
    return NOERROR;
}

ECode ASN1Constructed::EncodeASN(
    /* [in] */ IBerOutputStream* out)
{
    FAIL_RETURN(out->EncodeTag(mConstrId));
    return EncodeContent(out);
}

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org
