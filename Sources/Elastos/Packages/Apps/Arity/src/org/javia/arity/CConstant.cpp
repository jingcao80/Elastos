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

#include "org/javia/arity/CConstant.h"
#include "org/javia/arity/CComplex.h"

using Org::Javia::Arity::EIID_IConstant;

namespace Org {
namespace Javia {
namespace Arity {

CAR_OBJECT_IMPL(CConstant)

CAR_INTERFACE_IMPL(CConstant, Function, IConstant)

ECode CConstant::constructor(
    /* [in] */ IComplex* o)
{
    CComplex::New(o, (IComplex**)&mValue);
    return NOERROR;
}

ECode CConstant::EvalComplex(
    /* [out] */ IComplex** complex)
{
    VALIDATE_NOT_NULL(complex)
    *complex = mValue;
    REFCOUNT_ADD(*complex)
    return NOERROR;
}

ECode CConstant::Eval(
    /* [out] */ Double* result)
{
    VALIDATE_NOT_NULL(result)
    return mValue->AsReal(result);
}

ECode CConstant::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    return IObject::Probe(mValue)->ToString(str);
}

ECode CConstant::Arity(
    /* [out] */ Int32* arity)
{
    VALIDATE_NOT_NULL(arity)
    *arity = 0;
    return NOERROR;
}

} // namespace Arity
} // namespace Javia
} // namespace Org