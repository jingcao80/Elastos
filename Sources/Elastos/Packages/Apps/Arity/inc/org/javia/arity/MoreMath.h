
#ifndef _ORG_JAVIA_ARITY_MOREMATH_H__
#define _ORG_JAVIA_ARITY_MOREMATH_H__

#include "_Org.Javia.Arity.h"
#include <elastos/droid/ext/frameworkext.h>

namespace Org {
namespace Javia {
namespace Arity {

class MoreMath
{
public:
    static CARAPI_(Double) Asinh(
        /* [in] */ Double x);

    static CARAPI_(Double) Acosh(
        /* [in] */ Double x);

    static CARAPI_(Double) Atanh(
        /* [in] */ Double x);

    static CARAPI_(Double) Trunc(
        /* [in] */ Double x);

    static CARAPI_(Double) Gcd(
        /* [in] */ Double x,
        /* [in] */ Double y);

    static CARAPI_(Double) Lgamma(
        /* [in] */ Double x);

    static CARAPI_(Double) Factorial(
        /* [in] */ Double x);

    static CARAPI_(Double) Combinations(
        /* [in] */ Double n,
        /* [in] */ Double k);

    static CARAPI_(Double) Permutations(
        /* [in] */ Double n,
        /* [in] */ Double k);

    static CARAPI_(Double) Log2(
        /* [in] */ Double x);

    static CARAPI_(Double) Sin(
        /* [in] */ Double x);

    static CARAPI_(Double) Cos(
        /* [in] */ Double x);

    static CARAPI_(Double) Tan(
        /* [in] */ Double x);

    static CARAPI_(Int32) IntLog10(
        /* [in] */ Double x);

    static CARAPI_(Double) IntExp10(
        /* [in] */ Int32 exp);

private:
    static CARAPI_(Boolean) IsPiMultiple(
        /* [in] */ Double x);

public:
    static const AutoPtr<ArrayOf<Double> > GAMMA;

    static const AutoPtr<ArrayOf<Double> > FACT;

private:
    static const Double LOG2E  = 1.4426950408889634074;
};

} // namespace Arity
} // namespace Javia
} // namespace Org

#endif // _ORG_JAVIA_ARITY_MoreMath_H__