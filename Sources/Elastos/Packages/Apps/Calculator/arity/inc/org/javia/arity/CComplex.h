
#ifndef _ORG_JAVIA_ARITY_CCOMPLEX_H__
#define _ORG_JAVIA_ARITY_CCOMPLEX_H__

#include "_Org_Javia_Arity_CComplex.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>

namespace Org {
namespace Javia {
namespace Arity {

CarClass(CComplex)
    , public Object
    , public IComplex
{
public:
    CComplex()
        : mRe(0.0)
        , mIm(0.0)
    {}

    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    /** Empty constructor, complex value 0. */
    CARAPI constructor();

    /** Constructor taking the real and imaginary components. */
    CARAPI constructor(
        /* [in] */ Double re,
        /* [in] */ Double im);

    /** Copy constructor. */
    CARAPI constructor(
        /* [in] */ IComplex* o);

    /** Sets the real and imaginary components. */
    CARAPI Set(
        /* [in] */ Double re,
        /* [in] */ Double im);

    /** Sets from other object (copy). */
    CARAPI Set(
        /* [in] */ IComplex* o);

    /** Formats the real and imaginary part into a string. */
    CARAPI ToString(
        /* [out] */ String* str);

    /** Returns the real part if the imaginary part is zero, otherwise returns NaN. */
    CARAPI AsReal(
        /* [out] */ Double* result);

    /** Complex conjugate (negates imaginary). */
    CARAPI Conjugate();

    /** Negate, i.e. multiply with -1.
     */
    CARAPI Negate();

    /** True if this is an infinite (and not a NaN).
     */
    CARAPI IsInfinite(
        /* [out] */ Boolean* isInfinite);

    /** True if both the real and the imaginary parts
        are finite (not infinite and not NaN).
     */
    CARAPI IsFinite(
        /* [out] */ Boolean* isFinite);

    /** True if either real or imaginary is NaN. */
    CARAPI IsNaN(
        /* [out] */ Boolean* isNan);

    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* result);

    /** The argument (angle) in polar coordinates.
     */
    CARAPI Arg(
        /* [out] */ Double* result);

    /** The absolute value (length in polar coordinates).
     */
    CARAPI Abs(
        /* [out] */ Double* result);

    /** The absolute value squared.
        re^2 + im^2
     */
    CARAPI Abs2(
        /* [out] */ Double* result);

    /** Addition.
        Modifies and returns this.
    */
    CARAPI Add(
        /* [in] */ IComplex* o);

    /** Substraction.
     */
    CARAPI Sub(
        /* [in] */ IComplex* o);

    CARAPI Mul(
        /* [in] */ Double o);

    /** Multiplication.
     */
    CARAPI Mul(
        /* [in] */ IComplex* o);

    /** Division.
     */
    CARAPI Div(
        /* [in] */ IComplex* o);

    /** Complex square root.
     */
    CARAPI Sqrt();

    /** Complex modulo (integer division remainder).
     */
    CARAPI Mod(
        /* [in] */ IComplex* o);

    /** Complex GCD, Greatest Common Denominator.
     */
    CARAPI Gcd(
        /* [in] */ IComplex* o);

    /** Complex natural logarithm.
     */
    CARAPI Log();

    /** Complex exponential.
     */
    CARAPI Exp();

    /** Complex square (x^2).
     */
    CARAPI Square();

    /** Complex power (x^y == exp(y*log(x))).
    */
    CARAPI Pow(
        /* [in] */ IComplex* y);

    /** Complex lgamma (log Gamma).
     */
    CARAPI Lgamma();

    /** Complex factorial, based on lgamma().
     */
    CARAPI Factorial();

    /** sin(a+ib) = sin(a)*cosh(b) + i*cos(a)*sinh(b). */
    CARAPI Sin();

    /** sinh(a+ib) = sinh(a)*cos(b) + i*cosh(a)*sin(b). */
    CARAPI Sinh();

    /** cos(a+ib) = cos(a)cosh(b) - i*sin(a)sinh(b). */
    CARAPI Cos();

    /** cosh(a+ib) = cosh(a)cos(b) + i*sinh(a)sin(b). */
    CARAPI Cosh();

    /** tan(a+ib) = sin(2a)/(cos(2a)+cosh(2b) + i*sinh(2b)/(cos(2a)+cosh(2b)). */
    CARAPI Tan();

    /** tanh(a+ib) = sinh(2a)/(cosh(2a) + cos(2b)) + i*sin(2b)/(cosh(2a)+cos(2b)). */
    CARAPI Tanh();

    /** asin(x) = -i*log(sqrt(1-x^2)+ix). */
    CARAPI Asin();

    /** acos(x) = -i*log(x + i*sqrt(1-x^2)). */
    CARAPI Acos();

    /** atan(x) = i/2 * log((i+x)/(i-x)). */
    CARAPI Atan();

    /** asinh(x) = log(x+sqrt(x^2+1)). */
    CARAPI Asinh();

    /** acosh(x) = log(x+sqrt(x^2-1)). */
    CARAPI Acosh();

    /** atanh(x) = log((1+x)/(1-x))/2. */
    CARAPI Atanh();

    /** Combinations: C(n, k) == exp(lgamma(n) - lgamma(k) - lgamma(n-k)). */
    CARAPI Combinations(
        /* [in] */ IComplex* o);

    /** Permutations: P(n, k) == exp(lgamma(n) - lgamma(n-k)). */
    CARAPI Permutations(
        /* [in] */ IComplex* o);

private:
    /** Swaps real and imaginary.
     */
    CARAPI_(void) Swap();

    /** Normalizes the finite components of an infinity to zero.
        Used internally by mul().
        Assumes this is infinite.
     */
    CARAPI_(void) NormalizeInfinity();

    /** sqrt(1-x^2) */
    CARAPI_(void) Sqrt1z();

public:
    /** The real component. */
    Double mRe;

    /** The imaginary component. */
    Double mIm;
};

} // namespace Arity
} // namespace Javia
} // namespace Org

#endif // _ORG_JAVIA_ARITY_CCOMPILEDFUNCTION_H__