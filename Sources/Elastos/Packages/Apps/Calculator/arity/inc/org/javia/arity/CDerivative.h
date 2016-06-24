
#ifndef _ORG_JAVIA_ARITY_CDERIVATIVE_H__
#define _ORG_JAVIA_ARITY_CDERIVATIVE_H__

#include "_Org_Javia_Arity_CDerivative.h"
#include "org/javia/arity/Function.h"

namespace Org {
namespace Javia {
namespace Arity {

/**
   A Derivative presented as a function, always evaluates to the same value.
 */
CarClass(CDerivative)
    , public Function
    , public IDerivative
{
public:
    CDerivative();

    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IFunction* f);

    CARAPI Eval(
        /* [in] */ Double x,
        /* [out] */ Double* result);

    CARAPI Arity(
        /* [out] */ Int32* arity);

private:
    AutoPtr<IFunction> mF;
    AutoPtr<IComplex> mC;
    static const Double H = 1e-12;
    static const Double INVH;
};

} // namespace Arity
} // namespace Javia
} // namespace Org

#endif // _ORG_JAVIA_ARITY_CDERIVATIVE_H__