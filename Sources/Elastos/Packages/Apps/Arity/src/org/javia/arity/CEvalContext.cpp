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

#include "org/javia/arity/CEvalContext.h"
#include "org/javia/arity/CComplex.h"

using Org::Javia::Arity::EIID_IEvalContext;

namespace Org {
namespace Javia {
namespace Arity {

const Int32 CEvalContext::MAX_STACK_SIZE;

CEvalContext::CEvalContext()
    : mStackBase(0)
{
    mStackRe = ArrayOf<Double>::Alloc(MAX_STACK_SIZE);
    mStackComplex = ArrayOf<IComplex*>::Alloc(MAX_STACK_SIZE);

    mArgs1 = ArrayOf<Double>::Alloc(1);
    mArgs2 = ArrayOf<Double>::Alloc(2);
}

CAR_OBJECT_IMPL(CEvalContext)

CAR_INTERFACE_IMPL(CEvalContext, Object, IEvalContext)

ECode CEvalContext::constructor()
{
    for (Int32 i = 0; i < MAX_STACK_SIZE; ++i) {
        AutoPtr<IComplex> c;
        CComplex::New((IComplex**)&c);
        mStackComplex->Set(i, c);
    }
    mArgs1c = ArrayOf<IComplex*>::Alloc(1);
    AutoPtr<IComplex> c1, c2, c3;
    CComplex::New((IComplex**)&c1);
    mArgs1c->Set(0, c1);
    mArgs2c = ArrayOf<IComplex*>::Alloc(2);
    CComplex::New((IComplex**)&c2);
    CComplex::New((IComplex**)&c3);
    mArgs2c->Set(0, c2);
    mArgs2c->Set(1, c3);
    return NOERROR;
}

} // namespace Arity
} // namespace Javia
} // namespace Org