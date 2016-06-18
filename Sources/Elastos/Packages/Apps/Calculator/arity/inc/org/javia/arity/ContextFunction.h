
#ifndef _ORG_JAVIA_ARITY_CONTEXTFUNCITON_H__
#define _ORG_JAVIA_ARITY_CONTEXTFUNCITON_H__

#include "org/javia/arity/Function.h"

namespace Org {
namespace Javia {
namespace Arity {

class ContextFunction
    : public Function
    , public IContextFunction
{
public:
    virtual ~ContextFunction() {}

    CAR_INTERFACE_DECL()

    //@Override
    CARAPI Eval(
        /* [out] */ Double* result);

    //@Override
    CARAPI Eval(
        /* [in] */ Double x,
        /* [out] */ Double* result);

    //@Override
    CARAPI Eval(
        /* [in] */ Double x,
        /* [in] */ Double y,
        /* [out] */ Double* result);

    //@Override
    CARAPI Eval(
        /* [in] */ ArrayOf<Double>* args,
        /* [out] */ Double* result);

    CARAPI Eval(
        /* [in] */ Double x,
        /* [in] */ IEvalContext* context,
        /* [out] */ Double* result);

    CARAPI Eval(
        /* [in] */ Double x,
        /* [in] */ Double y,
        /* [in] */ IEvalContext* context,
        /* [out] */ Double* result);

    //@Override
    CARAPI EvalComplex(
        /* [out] */ IComplex** complex);

    //@Override
    CARAPI Eval(
        /* [in] */ IComplex* x,
        /* [out] */ IComplex** complex);

    //@Override
    CARAPI Eval(
        /* [in] */ IComplex* x,
        /* [in] */ IComplex* y,
        /* [out] */ IComplex** complex);

    //@Override
    CARAPI Eval(
        /* [in] */ ArrayOf<IComplex*>* args,
        /* [out] */ IComplex** complex);

    CARAPI Eval(
        /* [in] */ IComplex* x,
        /* [in] */ IEvalContext* context,
        /* [out] */ IComplex** complex);

    CARAPI Eval(
        /* [in] */ IComplex* x,
        /* [in] */ IComplex* y,
        /* [in] */ IEvalContext* context,
        /* [out] */ IComplex** complex);

protected:
    CARAPI_(AutoPtr<ArrayOf<IComplex*> >) ToComplex(
        /* [in] */ ArrayOf<Double>* args,
        /* [in] */ IEvalContext* context);

private:
    static AutoPtr<IEvalContext> CONTEXT;
    static AutoPtr<ArrayOf<Double> > NO_ARGS;
    static AutoPtr<ArrayOf<IComplex*> > NO_ARGS_COMPLEX;
};

} // namespace Arity
} // namespace Javia
} // namespace Org

#endif // _ORG_JAVIA_ARITY_CONTEXTFUNCITON_H__