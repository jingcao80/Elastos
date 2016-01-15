#ifndef __ELASTOS_MATH_CMATHCONTEXT_H__
#define __ELASTOS_MATH_CMATHCONTEXT_H__

#include "_Elastos_Math_CMathContext.h"
#include <elastos/core/Object.h>


namespace Elastos {
namespace Math {

CarClass(CMathContext)
    , public Object
    , public IMathContext
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    /**
     * Constructs a new {@code MathContext} with the specified precision and
     * with the rounding mode {@link RoundingMode#HALF_UP HALF_UP}. If the
     * precision passed is zero, then this implies that the computations have to
     * be performed exact, the rounding mode in this case is irrelevant.
     *
     * @param precision
     *            the precision for the new {@code MathContext}.
     * @throws IllegalArgumentException
     *             if {@code precision < 0}.
     */
    CARAPI constructor(
        /* [in] */ Int32 precision);

    /**
     * Constructs a new {@code MathContext} with the specified precision and
     * with the specified rounding mode. If the precision passed is zero, then
     * this implies that the computations have to be performed exact, the
     * rounding mode in this case is irrelevant.
     *
     * @param precision
     *            the precision for the new {@code MathContext}.
     * @param roundingMode
     *            the rounding mode for the new {@code MathContext}.
     * @throws IllegalArgumentException
     *             if {@code precision < 0}.
     * @throws NullPointerException
     *             if {@code roundingMode} is {@code null}.
     */
    CARAPI constructor(
        /* [in] */ Int32 precision,
        /* [in] */ RoundingMode roundingMode);

    /**
     * Returns the precision. The precision is the number of digits used for an
     * operation. Results are rounded to this precision. The precision is
     * guaranteed to be non negative. If the precision is zero, then the
     * computations have to be performed exact, results are not rounded in this
     * case.
     *
     * @return the precision.
     */
    CARAPI GetPrecision(
        /* [out] */ Int32* precision);

    /**
     * Returns the rounding mode. The rounding mode is the strategy to be used
     * to round results.
     * <p>
     * The rounding mode is one of
     * {@link RoundingMode#UP},
     * {@link RoundingMode#DOWN},
     * {@link RoundingMode#CEILING},
     * {@link RoundingMode#FLOOR},
     * {@link RoundingMode#HALF_UP},
     * {@link RoundingMode#HALF_DOWN},
     * {@link RoundingMode#HALF_EVEN}, or
     * {@link RoundingMode#UNNECESSARY}.
     *
     * @return the rounding mode.
     */
    CARAPI GetRoundingMode(
        /* [out] */ RoundingMode* roundingMode);

    /**
     * Returns true if x is a {@code MathContext} with the same precision
     * setting and the same rounding mode as this {@code MathContext} instance.
     *
     * @param x
     *            object to be compared.
     * @return {@code true} if this {@code MathContext} instance is equal to the
     *         {@code x} argument; {@code false} otherwise.
     */
    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result);

    /**
     * Returns the hash code for this {@code MathContext} instance.
     *
     * @return the hash code for this {@code MathContext}.
     */
    CARAPI GetHashCode(
        /* [out] */ Int32* pCode);

    /**
     * Returns the string representation for this {@code MathContext} instance.
     * The string has the form
     * {@code
     * "precision=<precision> roundingMode=<roundingMode>"
     * } where {@code <precision>} is an integer describing the number
     * of digits used for operations and {@code <roundingMode>} is the
     * string representation of the rounding mode.
     *
     * @return a string representation for this {@code MathContext} instance
     */
    CARAPI ToString(
        /* [out] */ String* string);

public:
    /**
     * A {@code MathContext} which corresponds to the <a href="http://en.wikipedia.org/wiki/IEEE_754-1985">IEEE 754</a> quadruple
     * decimal precision format: 34 digit precision and
     * {@link RoundingMode#HALF_EVEN} rounding.
     */
    static const AutoPtr<IMathContext> DECIMAL128;

    /**
     * A {@code MathContext} which corresponds to the <a href="http://en.wikipedia.org/wiki/IEEE_754-1985">IEEE 754</a> single decimal
     * precision format: 7 digit precision and {@link RoundingMode#HALF_EVEN}
     * rounding.
     */
    static const AutoPtr<IMathContext> DECIMAL32;

    /**
     * A {@code MathContext} which corresponds to the <a href="http://en.wikipedia.org/wiki/IEEE_754-1985">IEEE 754</a> double decimal
     * precision format: 16 digit precision and {@link RoundingMode#HALF_EVEN}
     * rounding.
     */
    static const AutoPtr<IMathContext> DECIMAL64;

    /**
     * A {@code MathContext} for unlimited precision with
     * {@link RoundingMode#HALF_UP} rounding.
     */
    static const AutoPtr<IMathContext> UNLIMITED;

private:
    /**
     * The number of digits to be used for an operation; results are rounded to
     * this precision.
     */
    Int32 mPrecision;

    /**
     * A {@code RoundingMode} object which specifies the algorithm to be used
     * for rounding.
     */
    RoundingMode mRoundingMode;
};

} // namespace Math
} // namespace Elastos

#endif // __ELASTOS_MATH_CMATHCONTEXT_H__
