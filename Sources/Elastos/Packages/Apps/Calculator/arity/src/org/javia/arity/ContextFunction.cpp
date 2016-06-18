
#include "org/javia/arity/ContextFunction.h"
#include "org/javia/arity/CEvalContext.h"
#include "org/javia/arity/CComplex.h"
#include <elastos/core/AutoLock.h>

using Elastos::Core::Autolock;
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
AutoPtr<IEvalContext> ContextFunction::context = InitContext();

static AutoPtr<ArrayOf<Double> > InitNoArgs()
{
    AutoPtr<ArrayOf<Double> > args = ArrayOf<Double>::Alloc(0);
    return args;
}
static AutoPtr<ArrayOf<Double> > ContextFunction::NO_ARGS = InitNoArgs();

static AutoPtr<ArrayOf<IComplex*> > InitNoArgsComplex()
{
    AutoPtr<ArrayOf<IComplex*> > args = ArrayOf<IComplex*>::Alloc(0);
    return args;
}
static AutoPtr<ArrayOf<IComplex*> > ContextFunction::NO_ARGS_COMPLEX = InitNoArgsComplex();

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
    Autolock Lock(CONTEXT);
    return Eval(x, CONTEXT, result);
}

ECode ContextFunction::Eval(
    /* [in] */ Double x,
    /* [in] */ Double y,
    /* [out] */ Double* result)
{
    VALIDATE_NOT_NULL(result)
    Autolock Lock(CONTEXT);
    return Eval(x, y, CONTEXT, result);
}

ECode ContextFunction::Eval(
    /* [in] */ ArrayOf<Double>* args,
    /* [out] */ Double* result)
{
    VALIDATE_NOT_NULL(result)
    Autolock Lock(CONTEXT);
    return Eval(args, CONTEXT, result);
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
    Autolock Lock(CONTEXT);
    return Eval(x, CONTEXT, complex);
}

ECode ContextFunction::Eval(
    /* [in] */ IComplex* x,
    /* [in] */ IComplex* y,
    /* [out] */ IComplex** complex)
{
    VALIDATE_NOT_NULL(complex)
    Autolock Lock(CONTEXT);
    return Eval(x, y, CONTEXT, complex);
}

ECode ContextFunction::Eval(
    /* [in] */ ArrayOf<IComplex*>* args,
    /* [out] */ IComplex** complex)
{
    VALIDATE_NOT_NULL(complex)
    Autolock Lock(CONTEXT);
    return Eval(args, CONTEXT, complex);
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

} // namespace Calculator2
} // namespace Droid
} // namespace Elastos