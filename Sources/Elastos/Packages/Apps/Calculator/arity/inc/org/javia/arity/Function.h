
#ifndef _ORG_JAVIA_ARITY_FUNCITON_H__
#define _ORG_JAVIA_ARITY_FUNCITON_H__

#include "_Org.Javia.Arity.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>

namespace Org {
namespace Javia {
namespace Arity {

class Function
    : public Object
    , public IFunction
{
public:
    virtual ~Function() {}

    CAR_INTERFACE_DECL()

    CARAPI GetDerivative(
        /* [out] */ IFunction** func);

    /**
       Evaluates an arity-0 function (a function with no arguments).
       @return the value of the function
    */
    CARAPI Eval(
        /* [out] */ Double* result);

    /**
       Evaluates a function with a single argument (arity == 1).
     */
    CARAPI Eval(
        /* [in] */ Double x,
        /* [out] */ Double* result);

    /**
       Evaluates a function with two arguments (arity == 2).
    */
    CARAPI Eval(
        /* [in] */ Double x,
        /* [in] */ Double y,
        /* [out] */ Double* result);

    /**
       Evaluates the function given the argument values.
       @param args array containing the arguments.
       @return the value of the function
       @throws ArityException if args.length != arity.
    */
    CARAPI Eval(
        /* [in] */ ArrayOf<Double>* args,
        /* [out] */ Double* result);


    /** By default complex forwards to real eval is the arguments are real,
     *  otherwise returns NaN.
     *  This allow calling any real functions as a (restricted) complex one.
     */
    CARAPI EvalComplex(
        /* [out] */ IComplex** complex);

    /**
       Complex evaluates a function with a single argument.
    */
    CARAPI Eval(
        /* [in] */ IComplex* x,
        /* [out] */ IComplex** complex);

    /**
       Complex evaluates a function with two arguments.
     */
    CARAPI Eval(
        /* [in] */ IComplex* x,
        /* [in] */ IComplex* y,
        /* [out] */ IComplex** complex);

    /**
       Complex evaluates a function with an arbitrary number of arguments.
    */
    CARAPI Eval(
        /* [in] */ ArrayOf<IComplex*>* args,
        /* [out] */ IComplex** complex);

    CARAPI CheckArity(
        /* [in] */ Int32 nArgs);

protected:
    CARAPI_(void) SetDerivative(
        /* [in] */ IFunction* deriv);

protected:
    String mComment;

private:
    AutoPtr<IFunction> mCachedDerivate;
};

} // namespace Arity
} // namespace Javia
} // namespace Org

#endif // _ORG_JAVIA_ARITY_FUNCITON_H__