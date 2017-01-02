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

#include "CASN1Sequence.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

CAR_OBJECT_IMPL(CASN1Sequence)

ASN1TYPE_METHODS_IMPL(CASN1Sequence, ASN1Sequence)

ECode CASN1Sequence::SetType(
    /* [in] */ ArrayOf<IASN1Type *>* type)
{
    return ASN1Sequence::SetType(type);
}

ECode CASN1Sequence::GetType(
    /* [out, callee] */ ArrayOf<IASN1Type*>** type)
{
    return ASN1Sequence::GetType(type);
}

ECode CASN1Sequence::SetOPTIONAL(
    /* [in] */ ArrayOf<Boolean>* optional)
{
    return ASN1Sequence::SetOPTIONAL(optional);
}

ECode CASN1Sequence::GetOPTIONAL(
    /* [out, callee] */ ArrayOf<Boolean>** optional)
{
    return ASN1Sequence::GetOPTIONAL(optional);
}

ECode CASN1Sequence::SetDEFAULT(
    /* [in] */ ArrayOf<IInterface*>* def)
{
    return ASN1Sequence::SetDEFAULT(def);
}

ECode CASN1Sequence::GetDEFAULT(
    /* [out, callee] */ ArrayOf<IInterface*>** def)
{
    return ASN1Sequence::GetDEFAULT(def);
}

ECode CASN1Sequence::GetValues(
    /* [in] */ IInterface* object,
    /* [in] */ ArrayOf<IInterface*>* values)
{
    return ASN1Sequence::GetValues(object, values);
}

ECode CASN1Sequence::constructor(
    /* [in] */ ArrayOf<IASN1Type *>* type)
{
    return ASN1Sequence::Init(IASN1Constants::TAG_SEQUENCE, type);
}

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

