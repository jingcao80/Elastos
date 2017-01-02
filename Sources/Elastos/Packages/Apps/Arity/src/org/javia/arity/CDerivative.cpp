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

#include "org/javia/arity/CDerivative.h"
#include "org/javia/arity/CComplex.h"

using Org::Javia::Arity::EIID_IDerivative;

namespace Org {
namespace Javia {
namespace Arity {

const Double CDerivative::H;
const Double CDerivative::INVH = 1 / H;

CDerivative::CDerivative()
{
    CComplex::New((IComplex**)&mC);
}

CAR_OBJECT_IMPL(CDerivative)

CAR_INTERFACE_IMPL(CDerivative, Function, IDerivative)

ECode CDerivative::constructor(
    /* [in] */ IFunction* f)
{
    mF = f;
    return f->CheckArity(1);
}

ECode CDerivative::Eval(
    /* [in] */ Double x,
    /* [out] */ Double* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;
    mC->Set(x, H);
    AutoPtr<IComplex> c;
    FAIL_RETURN(mF->Eval(mC, (IComplex**)&c))
    *result = ((CComplex*)c.Get())->mIm * INVH;
    return NOERROR;
}

ECode CDerivative::Arity(
    /* [out] */ Int32* arity)
{
    VALIDATE_NOT_NULL(arity)
    *arity = 1;
    return NOERROR;
}

} // namespace Arity
} // namespace Javia
} // namespace Org