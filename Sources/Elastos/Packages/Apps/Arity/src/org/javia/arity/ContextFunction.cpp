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

#include "org/javia/arity/ContextFunction.h"
#include "org/javia/arity/CEvalContext.h"
#include "org/javia/arity/CComplex.h"
#include <elastos/core/AutoLock.h>

using Elastos::Core::AutoLock;
using Org::Javia::Arity::EIID_IContextFunction;

namespace Org {
namespace Javia {
namespace Arity {

static AutoPtr<IEvalContext> InitContext()
{
    AutoPtr<IEvalContext> ctx;
    CEvalContext::New((IEvalContext**)&ctx);
    return ctx;
}
AutoPtr<IEvalContext> ContextFunction::sContext = InitContext();

static AutoPtr<ArrayOf<Double> > InitNoArgs()
{
    AutoPtr<ArrayOf<Double> > args = ArrayOf<Double>::Alloc(0);
    return args;
}
const AutoPtr<ArrayOf<Double> > ContextFunction::NO_ARGS = InitNoArgs();

static AutoPtr<ArrayOf<IComplex*> > InitNoArgsComplex()
{
    AutoPtr<ArrayOf<IComplex*> > args = ArrayOf<IComplex*>::Alloc(0);
    return args;
}
const  AutoPtr<ArrayOf<IComplex*> > ContextFunction::NO_ARGS_COMPLEX = InitNoArgsComplex();

CAR_INTERFACE_IMPL(ContextFunction, Function, IContextFunction)

AutoPtr<ArrayOf<IComplex*> > ContextFunction::ToComplex(
    /* [in] */ ArrayOf<Double>* args,
    /* [in] */ IEvalContext* context)
{
    AutoPtr<ArrayOf<IComplex*> > argsC;
    AutoPtr<CEvalContext> obj = (CEvalContext*)context;
    switch (args->GetLength()) {
        case 0:
            argsC = NO_ARGS_COMPLEX;
            break;
        case 1:
            argsC = obj->mArgs1c;
            (*argsC)[0]->Set((*args)[0], 0);
            break;
        case 2:
            argsC = obj->mArgs2c;
            (*argsC)[0]->Set((*args)[0], 0);
            (*argsC)[1]->Set((*args)[1], 0);
            break;
        default:
            argsC = ArrayOf<IComplex*>::Alloc(args->GetLength());
            for (Int32 i = 0; i < args->GetLength(); ++i) {
                AutoPtr<IComplex> complex;
                CComplex::New((*args)[i], 0, (IComplex**)&complex);
                argsC->Set(i, complex);
            }
    }
    return argsC;
}

ECode ContextFunction::Eval(
    /* [out] */ Double* result)
{
    VALIDATE_NOT_NULL(result)
    return Eval(NO_ARGS, result);
}

ECode ContextFunction::Eval(
    /* [in] */ Double x,
    /* [out] */ Double* result)
{
    VALIDATE_NOT_NULL(result)
    AutoLock lock(sContext);
    return Eval(x, sContext, result);
}

ECode ContextFunction::Eval(
    /* [in] */ Double x,
    /* [in] */ Double y,
    /* [out] */ Double* result)
{
    VALIDATE_NOT_NULL(result)
    AutoLock lock(sContext);
    return Eval(x, y, sContext, result);
}

ECode ContextFunction::Eval(
    /* [in] */ ArrayOf<Double>* args,
    /* [out] */ Double* result)
{
    VALIDATE_NOT_NULL(result)
    AutoLock lock(sContext);
    return Eval(args, sContext, result);
}

ECode ContextFunction::Eval(
    /* [in] */ Double x,
    /* [in] */ IEvalContext* context,
    /* [out] */ Double* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ArrayOf<Double> > args = ((CEvalContext*)context)->mArgs1;
    (*args)[0] = x;
    return Eval(args, context, result);
}

ECode ContextFunction::Eval(
    /* [in] */ Double x,
    /* [in] */ Double y,
    /* [in] */ IEvalContext* context,
    /* [out] */ Double* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ArrayOf<Double> > args = ((CEvalContext*)context)->mArgs2;
    (*args)[0] = x;
    (*args)[1] = y;
    return Eval(args, context, result);
}

ECode ContextFunction::EvalComplex(
    /* [out] */ IComplex** complex)
{
    VALIDATE_NOT_NULL(complex)
    return Eval(NO_ARGS_COMPLEX, complex);
}

ECode ContextFunction::Eval(
    /* [in] */ IComplex* x,
    /* [out] */ IComplex** complex)
{
    VALIDATE_NOT_NULL(complex)
    AutoLock lock(sContext);
    return Eval(x, sContext, complex);
}

ECode ContextFunction::Eval(
    /* [in] */ IComplex* x,
    /* [in] */ IComplex* y,
    /* [out] */ IComplex** complex)
{
    VALIDATE_NOT_NULL(complex)
    AutoLock lock(sContext);
    return Eval(x, y, sContext, complex);
}

ECode ContextFunction::Eval(
    /* [in] */ ArrayOf<IComplex*>* args,
    /* [out] */ IComplex** complex)
{
    VALIDATE_NOT_NULL(complex)
    AutoLock lock(sContext);
    return Eval(args, sContext, complex);
}

ECode ContextFunction::Eval(
    /* [in] */ IComplex* x,
    /* [in] */ IEvalContext* context,
    /* [out] */ IComplex** complex)
{
    VALIDATE_NOT_NULL(complex)
    AutoPtr<ArrayOf<IComplex*> > args = ((CEvalContext*)context)->mArgs1c;
    args->Set(0, x);
    return Eval(args, context, complex);
}

ECode ContextFunction::Eval(
    /* [in] */ IComplex* x,
    /* [in] */ IComplex* y,
    /* [in] */ IEvalContext* context,
    /* [out] */ IComplex** complex)
{
    VALIDATE_NOT_NULL(complex)
    AutoPtr<ArrayOf<IComplex*> > args = ((CEvalContext*)context)->mArgs2c;
    args->Set(0, x);
    args->Set(1, y);
    return Eval(args, context, complex);
}

} // namespace Arity
} // namespace Javia
} // namespace Org