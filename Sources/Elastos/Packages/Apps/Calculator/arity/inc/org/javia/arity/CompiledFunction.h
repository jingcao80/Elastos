
#ifndef _ORG_JAVIA_ARITY_COMPILEDFUNCTION_H__
#define _ORG_JAVIA_ARITY_COMPILEDFUNCTION_H__

#include "org/javia/arity/ContextFunction.h"
#include "Elastos.CoreLibrary.Utility.h"

using Elastos::Utility::IRandom;

namespace Org {
namespace Javia {
namespace Arity {

/**
   CompiledFunction is a function that was parsed from a string expression.
   It is represented as a sequence of bytecodes which are executed in order to
   evaluate the function.

   <h3>Thread safety</h3>
   CompiledFunction instances are thread-safe (don't require external locking),
   By default the evaluation is globally serialized
   (it doesn't take advantage of multiple threads).<p>

   You can achive parallel evaluation by creating an instance of EvalContext
   for each thread, and passing the EvalContext as the last parameter of the
   eval() methods.
 */
class CompiledFunction
    : public ContextFunction
    , public ICompiledFunction
{
private:
    class DerivativeFunction : public Function
    {
    public:
        CARAPI Arity(
            /* [out] */ Int32* arity);

        CARAPI Eval(
            /* [in] */ Double x,
            /* [out] */ Double* result);
    };

public:
    CompiledFunction()
        : mArity(0)
    {}

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ Int32 arity,
        /* [in] */ ArrayOf<Byte>* code,
        /* [in] */ ArrayOf<Double>* constsRe,
        /* [in] */ ArrayOf<Double>* constsIm,
        /* [in] */ ArrayOf<IFunction*>* funcs);

    //@Override
    CARAPI Arity(
        /* [out] */ Int32* arity);

    CARAPI ToString(
        /* [out] */ String* str);

    //@Override
    CARAPI Eval(
        /* [in] */ ArrayOf<Double>* x,
        /* [in] */ IEvalContext* context,
        /* [out] */ Double* result);

    //@Override
    CARAPI Eval(
        /* [in] */ ArrayOf<IComplex*>* args,
        /* [in] */ IEvalContext* context,
        /* [out] */ IComplex** complex);

    //@Override
    CARAPI Eval(
        /* [in] */ Double x,
        /* [in] */ Double y,
        /* [out] */ Double* result);

    //@Override
    CARAPI Eval(
        /* [in] */ ArrayOf<Double>* args,
        /* [out] */ Double* result);

protected:
    CARAPI MakeOpFunction(
        /* [in] */ Int32 op,
        /* [out] */ IFunction** func);

    CARAPI ExecWithoutCheck(
        /* [in] */ IEvalContext* context,
        /* [in] */ Int32 p,
        /* [out] */ Int32* result);

    CARAPI ExecWithoutCheckComplex(
        /* [in] */ IEvalContext* context,
        /* [in] */ Int32 p,
        /* [in] */ Int32 percentPC,
        /* [out] */ Int32* result);

private:
    CARAPI EvalComplexToReal(
        /* [in] */ ArrayOf<Double>* args,
        /* [in] */ IEvalContext* context,
        /* [out] */ Double* result);

    CARAPI ExecReal(
        /* [in] */ IEvalContext* context,
        /* [in] */ Int32 p,
        /* [out] */ Int32* result);

    CARAPI ExecComplex(
        /* [in] */ IEvalContext* context,
        /* [in] */ Int32 p,
        /* [out] */ Int32* result);

private:
    static const AutoPtr<IRandom> RANDOM;
    static const AutoPtr<ArrayOf<Double> > EMPTY_DOUBLE;
    static const AutoPtr<ArrayOf<IFunction*> > EMPTY_FUN;
    static const AutoPtr<IComplex> ONE_THIRD;

    AutoPtr<ArrayOf<Double> > mConstsRe,
    AutoPtr<ArrayOf<Double> > mConstsIm;

    AutoPtr<ArrayOf<IFunction*> > mFuncs;
    AutoPtr<ArrayOf<Byte> > mCode;
    Int32 mArity; // >= 0
};

} // namespace Arity
} // namespace Javia
} // namespace Org

#endif // _ORG_JAVIA_ARITY_COMPILEDFUNCTION_H__