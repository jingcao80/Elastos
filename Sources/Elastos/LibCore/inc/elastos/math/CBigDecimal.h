#ifndef __ELASTOS_MATH_CBIGDECIMAL_H__
#define __ELASTOS_MATH_CBIGDECIMAL_H__

#include "_Elastos_Math_CBigDecimal.h"
#include <CBigInteger.h>
#include <elastos/core/Math.h>
#include <elastos/core/Object.h>
#include <elstring.h>

using Elastos::Core::INumber;
using Elastos::Core::IComparable;

namespace Elastos {
namespace Math {

/**
 * An immutable arbitrary-precision signed decimal.
 *
 * <p>A value is represented by an arbitrary-precision "unscaled value" and a signed 32-bit "scale",
 * combined thus: {@code unscaled * 10<sup>-scale</sup>}. See {@link #unscaledValue} and {@link #scale}.
 *
 * <p>Most operations allow you to supply a {@link MathContext} to specify a desired rounding mode.
 */
CarClass(CBigDecimal)
    , public Object
    , public IBigDecimal
    , public INumber
    , public IComparable
{
private:
    class StaticInitializer
    {
    public:
        StaticInitializer();
    };

public:
    CBigDecimal();

    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    /**
     * Constructs a new {@code BigDecimal} instance from the given int
     * {@code val}. The scale of the result is 0.
     *
     * @param val
     *            int value to be converted to a {@code BigDecimal} instance.
     */
    CARAPI constructor(
        /* [in] */ Int32 val);

    /**
     * Constructs a new {@code BigDecimal} instance from the given long {@code
     * val}. The scale of the result is {@code 0}.
     *
     * @param val
     *            long value to be converted to a {@code BigDecimal} instance.
     */
    CARAPI constructor(
        /* [in] */ Int64 val);

    /**
     * Constructs a new {@code BigDecimal} instance from the 64bit double
     * {@code val}. The constructed big decimal is equivalent to the given
     * double. For example, {@code new BigDecimal(0.1)} is equal to {@code
     * 0.1000000000000000055511151231257827021181583404541015625}. This happens
     * as {@code 0.1} cannot be represented exactly in binary.
     * <p>
     * To generate a big decimal instance which is equivalent to {@code 0.1} use
     * the {@code BigDecimal(String)} constructor.
     *
     * @param val
     *            double value to be converted to a {@code BigDecimal} instance.
     * @throws NumberFormatException
     *             if {@code val} is infinity or not a number.
     */
    CARAPI constructor(
        /* [in] */ Double val);

    CARAPI constructor(
        /* [in] */ Int64 smallValue,
        /* [in] */ Int32 scale);

    CARAPI constructor(
        /* [in] */ Int32 smallValue,
        /* [in] */ Int32 scale);

    /**
     * Constructs a new {@code BigDecimal} instance from a string
     * representation.
     *
     * @throws NumberFormatException
     *             if {@code val} does not contain a valid string representation
     *             of a big decimal.
     */
    CARAPI constructor(
        /* [in] */ const String& val);

    /**
     * Constructs a new {@code BigDecimal} instance from a string representation
     * given as a character array.
     *
     * @param in
     *            array of characters containing the string representation of
     *            this {@code BigDecimal}.
     * @throws NumberFormatException
     *             if {@code in} does not contain a valid string representation
     *             of a big decimal.
     */
    CARAPI constructor(
        /* [in] */ const ArrayOf<Char32>& buf);

    /**
     * Constructs a new {@code BigDecimal} instance from a string representation
     * given as a character array.
     *
     * @param in
     *            array of characters containing the string representation of
     *            this {@code BigDecimal}.
     * @param offset
     *            first index to be copied.
     * @param len
     *            number of characters to be used.
     * @throws NumberFormatException
     *             if {@code offset < 0 || len <= 0 || offset+len-1 < 0 ||
     *             offset+len-1 >= in.length}, or if {@code in} does not
     *             contain a valid string representation of a big decimal.
     */
    CARAPI constructor(
        /* [in] */ const ArrayOf<Char32>& buf,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length);

    /**
     * Constructs a new {@code BigDecimal} instance from the given big integer
     * {@code val}. The scale of the result is {@code 0}.
     */
    CARAPI constructor(
        /* [in] */ IBigInteger* val);

    /**
     * Constructs a new {@code BigDecimal} instance from a given unscaled value
     * {@code unscaledVal} and a given scale. The value of this instance is
     * {@code unscaledVal * 10<sup>-scale</sup>}).
     *
     * @throws NullPointerException
     *             if {@code unscaledVal == null}.
     */
    CARAPI constructor(
        /* [in] */ IBigInteger* val,
        /* [in] */ Int32 scale);

    /**
     * Constructs a new {@code BigDecimal} instance from a given unscaled value
     * {@code unscaledVal} and a given scale. The value of this instance is
     * {@code unscaledVal * 10<sup>-scale</sup>). The result is rounded according
     * to the specified math context.
     *
     * @param mc
     *            rounding mode and precision for the result of this operation.
     * @throws ArithmeticException
     *             if {@code mc.precision > 0} and {@code mc.roundingMode ==
     *             UNNECESSARY} and the new big decimal cannot be represented
     *             within the given precision without rounding.
     * @throws NullPointerException
     *             if {@code unscaledVal == null}.
     */
    CARAPI constructor(
        /* [in] */ IBigInteger* val,
        /* [in] */ Int32 scale,
        /* [in] */ IMathContext* mc);

    /**
     * Constructs a new {@code BigDecimal} instance from a string representation
     * given as a character array.
     *
     * @param in
     *            array of characters containing the string representation of
     *            this {@code BigDecimal}.
     * @param offset
     *            first index to be copied.
     * @param len
     *            number of characters to be used.
     * @param mc
     *            rounding mode and precision for the result of this operation.
     * @throws NumberFormatException
     *             if {@code offset < 0 || len <= 0 || offset+len-1 < 0 ||
     *             offset+len-1 >= in.length}, or if {@code in} does not
     *             contain a valid string representation of a big decimal.
     * @throws ArithmeticException
     *             if {@code mc.precision > 0} and {@code mc.roundingMode ==
     *             UNNECESSARY} and the new big decimal cannot be represented
     *             within the given precision without rounding.
     */
    CARAPI constructor(
        /* [in] */ const ArrayOf<Char32>& buf,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length,
        /* [in] */ IMathContext* mc);

    /**
     * Constructs a new {@code BigDecimal} instance from a string representation
     * given as a character array. The result is rounded according to the
     * specified math context.
     *
     * @param in
     *            array of characters containing the string representation of
     *            this {@code BigDecimal}.
     * @param mc
     *            rounding mode and precision for the result of this operation.
     * @throws NumberFormatException
     *             if {@code in} does not contain a valid string representation
     *             of a big decimal.
     * @throws ArithmeticException
     *             if {@code mc.precision > 0} and {@code mc.roundingMode ==
     *             UNNECESSARY} and the new big decimal cannot be represented
     *             within the given precision without rounding.
     */
    CARAPI constructor(
        /* [in] */ const ArrayOf<Char32>& buf,
        /* [in] */ IMathContext* mc);

    /**
     * Constructs a new {@code BigDecimal} instance from a string
     * representation. The result is rounded according to the specified math
     * context.
     *
     * @param mc
     *            rounding mode and precision for the result of this operation.
     * @throws NumberFormatException
     *             if {@code val} does not contain a valid string representation
     *             of a big decimal.
     * @throws ArithmeticException
     *             if {@code mc.precision > 0} and {@code mc.roundingMode ==
     *             UNNECESSARY} and the new big decimal cannot be represented
     *             within the given precision without rounding.
     */
    CARAPI constructor(
        /* [in] */ const String& val,
        /* [in] */ IMathContext* mc);

    /**
     * Constructs a new {@code BigDecimal} instance from the 64bit double
     * {@code val}. The constructed big decimal is equivalent to the given
     * double. For example, {@code new BigDecimal(0.1)} is equal to {@code
     * 0.1000000000000000055511151231257827021181583404541015625}. This happens
     * as {@code 0.1} cannot be represented exactly in binary.
     * <p>
     * To generate a big decimal instance which is equivalent to {@code 0.1} use
     * the {@code BigDecimal(String)} constructor.
     *
     * @param val
     *            double value to be converted to a {@code BigDecimal} instance.
     * @param mc
     *            rounding mode and precision for the result of this operation.
     * @throws NumberFormatException
     *             if {@code val} is infinity or not a number.
     * @throws ArithmeticException
     *             if {@code mc.precision > 0} and {@code mc.roundingMode ==
     *             UNNECESSARY} and the new big decimal cannot be represented
     *             within the given precision without rounding.
     */
    CARAPI constructor(
        /* [in] */ Double val,
        /* [in] */ IMathContext* mc);

    /**
     * Constructs a new {@code BigDecimal} instance from the given big integer
     * {@code val}. The scale of the result is {@code 0}.
     *
     * @param mc
     *            rounding mode and precision for the result of this operation.
     * @throws ArithmeticException
     *             if {@code mc.precision > 0} and {@code mc.roundingMode ==
     *             UNNECESSARY} and the new big decimal cannot be represented
     *             within the given precision without rounding.
     */
    CARAPI constructor(
        /* [in] */ IBigInteger* val,
        /* [in] */ IMathContext* mc);

    /**
     * Constructs a new {@code BigDecimal} instance from the given int {@code
     * val}. The scale of the result is {@code 0}. The result is rounded
     * according to the specified math context.
     *
     * @param val
     *            int value to be converted to a {@code BigDecimal} instance.
     * @param mc
     *            rounding mode and precision for the result of this operation.
     * @throws ArithmeticException
     *             if {@code mc.precision > 0} and {@code c.roundingMode ==
     *             UNNECESSARY} and the new big decimal cannot be represented
     *             within the given precision without rounding.
     */
    CARAPI constructor(
        /* [in] */ Int32 val,
        /* [in] */ IMathContext* mc);

    /**
     * Constructs a new {@code BigDecimal} instance from the given long {@code
     * val}. The scale of the result is {@code 0}. The result is rounded
     * according to the specified math context.
     *
     * @param val
     *            long value to be converted to a {@code BigDecimal} instance.
     * @param mc
     *            rounding mode and precision for the result of this operation.
     * @throws ArithmeticException
     *             if {@code mc.precision > 0} and {@code mc.roundingMode ==
     *             UNNECESSARY} and the new big decimal cannot be represented
     *             within the given precision without rounding.
     */
    CARAPI constructor(
        /* [in] */ Int64 val,
        /* [in] */ IMathContext* mc);

    /**
     * Returns a new {@code BigDecimal} instance whose value is equal to {@code
     * unscaledVal * 10<sup>-scale</sup>}). The scale of the result is {@code
     * scale}, and its unscaled value is {@code unscaledVal}.
     */
    static CARAPI ValueOf(
        /* [in] */ Int64 unscaledVal,
        /* [in] */ Int32 scale,
        /* [out] */ IBigDecimal** result);

    /**
     * Returns a new {@code BigDecimal} instance whose value is equal to {@code
     * unscaledVal}. The scale of the result is {@code 0}, and its unscaled
     * value is {@code unscaledVal}.
     *
     * @param unscaledVal
     *            value to be converted to a {@code BigDecimal}.
     * @return {@code BigDecimal} instance with the value {@code unscaledVal}.
     */
    static CARAPI ValueOf(
        /* [in] */ Int64 unscaledVal,
        /* [out] */ IBigDecimal** result);

    /**
     * Returns a new {@code BigDecimal} instance whose value is equal to {@code
     * val}. The new decimal is constructed as if the {@code BigDecimal(String)}
     * constructor is called with an argument which is equal to {@code
     * Double.toString(val)}. For example, {@code valueOf("0.1")} is converted to
     * (unscaled=1, scale=1), although the double {@code 0.1} cannot be
     * represented exactly as a double value. In contrast to that, a new {@code
     * BigDecimal(0.1)} instance has the value {@code
     * 0.1000000000000000055511151231257827021181583404541015625} with an
     * unscaled value {@code 1000000000000000055511151231257827021181583404541015625}
     * and the scale {@code 55}.
     *
     * @param val
     *            double value to be converted to a {@code BigDecimal}.
     * @return {@code BigDecimal} instance with the value {@code val}.
     * @throws NumberFormatException
     *             if {@code val} is infinite or {@code val} is not a number
     */
    static CARAPI ValueOf(
        /* [in] */ Double val,
        /* [out] */ IBigDecimal** result);

    /**
     * Returns a new {@code BigDecimal} whose value is {@code this + augend}.
     * The scale of the result is the maximum of the scales of the two
     * arguments.
     *
     * @param augend
     *            value to be added to {@code this}.
     * @return {@code this + augend}.
     * @throws NullPointerException
     *             if {@code augend == null}.
     */
    CARAPI Add(
        /* [in] */ IBigDecimal* augend,
        /* [out] */ IBigDecimal** result);

    /**
     * Returns a new {@code BigDecimal} whose value is {@code this + augend}.
     * The result is rounded according to the passed context {@code mc}.
     *
     * @param augend
     *            value to be added to {@code this}.
     * @param mc
     *            rounding mode and precision for the result of this operation.
     * @return {@code this + augend}.
     * @throws NullPointerException
     *             if {@code augend == null} or {@code mc == null}.
     */
    CARAPI Add(
        /* [in] */ IBigDecimal* augend,
        /* [in] */ IMathContext* mc,
        /* [out] */ IBigDecimal** result);

    /**
     * Returns a new {@code BigDecimal} whose value is {@code this - subtrahend}.
     * The scale of the result is the maximum of the scales of the two arguments.
     *
     * @param subtrahend
     *            value to be subtracted from {@code this}.
     * @return {@code this - subtrahend}.
     * @throws NullPointerException
     *             if {@code subtrahend == null}.
     */
    CARAPI Subtract(
        /* [in] */ IBigDecimal* subtrahend,
        /* [out] */ IBigDecimal** result);

    /**
     * Returns a new {@code BigDecimal} whose value is {@code this - subtrahend}.
     * The result is rounded according to the passed context {@code mc}.
     *
     * @param subtrahend
     *            value to be subtracted from {@code this}.
     * @param mc
     *            rounding mode and precision for the result of this operation.
     * @return {@code this - subtrahend}.
     * @throws NullPointerException
     *             if {@code subtrahend == null} or {@code mc == null}.
     */
    CARAPI Subtract(
        /* [in] */ IBigDecimal* subtrahend,
        /* [in] */ IMathContext* mc,
        /* [out] */ IBigDecimal** result);

    /**
     * Returns a new {@code BigDecimal} whose value is {@code this *
     * multiplicand}. The scale of the result is the sum of the scales of the
     * two arguments.
     *
     * @param multiplicand
     *            value to be multiplied with {@code this}.
     * @return {@code this * multiplicand}.
     * @throws NullPointerException
     *             if {@code multiplicand == null}.
     */
    CARAPI Multiply(
        /* [in] */ IBigDecimal* multiplicand,
        /* [out] */ IBigDecimal** result);

    /**
     * Returns a new {@code BigDecimal} whose value is {@code this *
     * multiplicand}. The result is rounded according to the passed context
     * {@code mc}.
     *
     * @param multiplicand
     *            value to be multiplied with {@code this}.
     * @param mc
     *            rounding mode and precision for the result of this operation.
     * @return {@code this * multiplicand}.
     * @throws NullPointerException
     *             if {@code multiplicand == null} or {@code mc == null}.
     */
    CARAPI Multiply(
        /* [in] */ IBigDecimal* multiplicand,
        /* [in] */ IMathContext* mc,
        /* [out] */ IBigDecimal** result);

    /**
     * Returns a new {@code BigDecimal} whose value is {@code this / divisor}.
     * As scale of the result the parameter {@code scale} is used. If rounding
     * is required to meet the specified scale, then the specified rounding mode
     * {@code roundingMode} is applied.
     *
     * @param divisor
     *            value by which {@code this} is divided.
     * @param scale
     *            the scale of the result returned.
     * @param roundingMode
     *            rounding mode to be used to round the result.
     * @return {@code this / divisor} rounded according to the given rounding
     *         mode.
     * @throws NullPointerException
     *             if {@code divisor == null} or {@code roundingMode == null}.
     * @throws ArithmeticException
     *             if {@code divisor == 0}.
     * @throws ArithmeticException
     *             if {@code roundingMode == RoundingMode.UNNECESSAR}Y and
     *             rounding is necessary according to the given scale and given
     *             precision.
     */
    CARAPI Divide(
        /* [in] */ IBigDecimal* divisor,
        /* [in] */ Int32 scale,
        /* [in] */ RoundingMode roundingMode,
        /* [out] */ IBigDecimal** result);

    /**
     * Returns a new {@code BigDecimal} whose value is {@code this / divisor}.
     * The scale of the result is the scale of {@code this}. If rounding is
     * required to meet the specified scale, then the specified rounding mode
     * {@code roundingMode} is applied.
     *
     * @param divisor
     *            value by which {@code this} is divided.
     * @param roundingMode
     *            rounding mode to be used to round the result.
     * @return {@code this / divisor} rounded according to the given rounding
     *         mode.
     * @throws NullPointerException
     *             if {@code divisor == null} or {@code roundingMode == null}.
     * @throws ArithmeticException
     *             if {@code divisor == 0}.
     * @throws ArithmeticException
     *             if {@code roundingMode == RoundingMode.UNNECESSARY} and
     *             rounding is necessary according to the scale of this.
     */
    CARAPI Divide(
        /* [in] */ IBigDecimal* divisor,
        /* [in] */ RoundingMode roundingMode,
        /* [out] */ IBigDecimal** result);

    /**
     * Returns a new {@code BigDecimal} whose value is {@code this / divisor}.
     * The scale of the result is the difference of the scales of {@code this}
     * and {@code divisor}. If the exact result requires more digits, then the
     * scale is adjusted accordingly. For example, {@code 1/128 = 0.0078125}
     * which has a scale of {@code 7} and precision {@code 5}.
     *
     * @param divisor
     *            value by which {@code this} is divided.
     * @return {@code this / divisor}.
     * @throws NullPointerException
     *             if {@code divisor == null}.
     * @throws ArithmeticException
     *             if {@code divisor == 0}.
     * @throws ArithmeticException
     *             if the result cannot be represented exactly.
     */
    CARAPI Divide(
        /* [in] */ IBigDecimal* divisor,
        /* [out] */ IBigDecimal** result);

    /**
     * Returns a new {@code BigDecimal} whose value is {@code this / divisor}.
     * The result is rounded according to the passed context {@code mc}. If the
     * passed math context specifies precision {@code 0}, then this call is
     * equivalent to {@code this.divide(divisor)}.
     *
     * @param divisor
     *            value by which {@code this} is divided.
     * @param mc
     *            rounding mode and precision for the result of this operation.
     * @return {@code this / divisor}.
     * @throws NullPointerException
     *             if {@code divisor == null} or {@code mc == null}.
     * @throws ArithmeticException
     *             if {@code divisor == 0}.
     * @throws ArithmeticException
     *             if {@code mc.getRoundingMode() == UNNECESSARY} and rounding
     *             is necessary according {@code mc.getPrecision()}.
     */
    CARAPI Divide(
        /* [in] */ IBigDecimal* divisor,
        /* [in] */ IMathContext* mc,
        /* [out] */ IBigDecimal** result);

    /**
     * Returns a new {@code BigDecimal} whose value is the integral part of
     * {@code this / divisor}. The quotient is rounded down towards zero to the
     * next integer. For example, {@code 0.5/0.2 = 2}.
     *
     * @param divisor
     *            value by which {@code this} is divided.
     * @return integral part of {@code this / divisor}.
     * @throws NullPointerException
     *             if {@code divisor == null}.
     * @throws ArithmeticException
     *             if {@code divisor == 0}.
     */
    CARAPI DivideToIntegralValue(
        /* [in] */ IBigDecimal* divisor,
        /* [out] */ IBigDecimal** result);

    /**
     * Returns a new {@code BigDecimal} whose value is the integral part of
     * {@code this / divisor}. The quotient is rounded down towards zero to the
     * next integer. The rounding mode passed with the parameter {@code mc} is
     * not considered. But if the precision of {@code mc > 0} and the integral
     * part requires more digits, then an {@code ArithmeticException} is thrown.
     *
     * @param divisor
     *            value by which {@code this} is divided.
     * @param mc
     *            math context which determines the maximal precision of the
     *            result.
     * @return integral part of {@code this / divisor}.
     * @throws NullPointerException
     *             if {@code divisor == null} or {@code mc == null}.
     * @throws ArithmeticException
     *             if {@code divisor == 0}.
     * @throws ArithmeticException
     *             if {@code mc.getPrecision() > 0} and the result requires more
     *             digits to be represented.
     */
    CARAPI DivideToIntegralValue(
        /* [in] */ IBigDecimal* divisor,
        /* [in] */ IMathContext* mc,
        /* [out] */ IBigDecimal** result);

    /**
     * Returns a new {@code BigDecimal} whose value is {@code this % divisor}.
     * <p>
     * The remainder is defined as {@code this -
     * this.divideToIntegralValue(divisor) * divisor}.
     *
     * @param divisor
     *            value by which {@code this} is divided.
     * @return {@code this % divisor}.
     * @throws NullPointerException
     *             if {@code divisor == null}.
     * @throws ArithmeticException
     *             if {@code divisor == 0}.
     */
    CARAPI Remainder(
        /* [in] */ IBigDecimal* divisor,
        /* [out] */ IBigDecimal** result);

    /**
     * Returns a new {@code BigDecimal} whose value is {@code this % divisor}.
     * <p>
     * The remainder is defined as {@code this -
     * this.divideToIntegralValue(divisor) * divisor}.
     * <p>
     * The specified rounding mode {@code mc} is used for the division only.
     *
     * @param divisor
     *            value by which {@code this} is divided.
     * @param mc
     *            rounding mode and precision to be used.
     * @return {@code this % divisor}.
     * @throws NullPointerException
     *             if {@code divisor == null}.
     * @throws ArithmeticException
     *             if {@code divisor == 0}.
     * @throws ArithmeticException
     *             if {@code mc.getPrecision() > 0} and the result of {@code
     *             this.divideToIntegralValue(divisor, mc)} requires more digits
     *             to be represented.
     */
    CARAPI Remainder(
        /* [in] */ IBigDecimal* divisor,
        /* [in] */ IMathContext* mc,
        /* [out] */ IBigDecimal** result);

    /**
     * Returns a {@code BigDecimal} array which contains the integral part of
     * {@code this / divisor} at index 0 and the remainder {@code this %
     * divisor} at index 1. The quotient is rounded down towards zero to the
     * next integer.
     *
     * @param divisor
     *            value by which {@code this} is divided.
     * @return {@code [this.divideToIntegralValue(divisor),
     *         this.remainder(divisor)]}.
     * @throws NullPointerException
     *             if {@code divisor == null}.
     * @throws ArithmeticException
     *             if {@code divisor == 0}.
     * @see #divideToIntegralValue
     * @see #remainder
     */
    CARAPI DivideAndRemainder(
        /* [in] */ IBigDecimal* divisor,
        /* [out, callee] */ ArrayOf<IBigDecimal*>** result);

    /**
     * Returns a {@code BigDecimal} array which contains the integral part of
     * {@code this / divisor} at index 0 and the remainder {@code this %
     * divisor} at index 1. The quotient is rounded down towards zero to the
     * next integer. The rounding mode passed with the parameter {@code mc} is
     * not considered. But if the precision of {@code mc > 0} and the integral
     * part requires more digits, then an {@code ArithmeticException} is thrown.
     *
     * @param divisor
     *            value by which {@code this} is divided.
     * @param mc
     *            math context which determines the maximal precision of the
     *            result.
     * @return {@code [this.divideToIntegralValue(divisor),
     *         this.remainder(divisor)]}.
     * @throws NullPointerException
     *             if {@code divisor == null}.
     * @throws ArithmeticException
     *             if {@code divisor == 0}.
     * @see #divideToIntegralValue
     * @see #remainder
     */
    CARAPI DivideAndRemainder(
        /* [in] */ IBigDecimal* divisor,
        /* [in] */ IMathContext* mc,
        /* [out, callee] */ ArrayOf<IBigDecimal*>** result);

    /**
     * Returns a new {@code BigDecimal} whose value is {@code this<sup>n</sup>}. The
     * scale of the result is {@code n * this.scale()}.
     *
     * <p>{@code x.pow(0)} returns {@code 1}, even if {@code x == 0}.
     *
     * <p>Implementation Note: The implementation is based on the ANSI standard
     * X3.274-1996 algorithm.
     *
     * @throws ArithmeticException
     *             if {@code n < 0} or {@code n > 999999999}.
     */
    CARAPI Pow(
        /* [in] */ Int32 n,
        /* [out] */ IBigDecimal** result);

    /**
     * Returns a new {@code BigDecimal} whose value is {@code this<sup>n</sup>}. The
     * result is rounded according to the passed context {@code mc}.
     *
     * <p>Implementation Note: The implementation is based on the ANSI standard
     * X3.274-1996 algorithm.
     *
     * @param mc
     *            rounding mode and precision for the result of this operation.
     * @throws ArithmeticException
     *             if {@code n < 0} or {@code n > 999999999}.
     */
    CARAPI Pow(
        /* [in] */ Int32 n,
        /* [in] */ IMathContext* mc,
        /* [out] */ IBigDecimal** result);

    /**
     * Returns a {@code BigDecimal} whose value is the absolute value of
     * {@code this}. The scale of the result is the same as the scale of this.
     */
    CARAPI Abs(
        /* [out] */ IBigDecimal** result);

    /**
     * Returns a {@code BigDecimal} whose value is the absolute value of
     * {@code this}. The result is rounded according to the passed context
     * {@code mc}.
     */
    CARAPI Abs(
        /* [in] */ IMathContext* mc,
        /* [out] */ IBigDecimal** result);

    /**
     * Returns a new {@code BigDecimal} whose value is the {@code -this}. The
     * scale of the result is the same as the scale of this.
     *
     * @return {@code -this}
     */
    CARAPI Negate(
        /* [out] */ IBigDecimal** result);

    /**
     * Returns a new {@code BigDecimal} whose value is the {@code -this}. The
     * result is rounded according to the passed context {@code mc}.
     *
     * @param mc
     *            rounding mode and precision for the result of this operation.
     * @return {@code -this}
     */
    CARAPI Negate(
        /* [in] */ IMathContext* mc,
        /* [out] */ IBigDecimal** result);

    /**
     * Returns a new {@code BigDecimal} whose value is {@code +this}. The scale
     * of the result is the same as the scale of this.
     *
     * @return {@code this}
     */
    CARAPI Plus(
        /* [out] */ IBigDecimal** result);

    /**
     * Returns a new {@code BigDecimal} whose value is {@code +this}. The result
     * is rounded according to the passed context {@code mc}.
     *
     * @param mc
     *            rounding mode and precision for the result of this operation.
     * @return {@code this}, rounded
     */
    CARAPI Plus(
        /* [in] */ IMathContext* mc,
        /* [out] */ IBigDecimal** result);

    /**
     * Returns the sign of this {@code BigDecimal}.
     *
     * @return {@code -1} if {@code this < 0},
     *         {@code 0} if {@code this == 0},
     *         {@code 1} if {@code this > 0}.     */
    CARAPI GetSignum(
        /* [out] */ Int32* sign);

    /**
     * Returns the scale of this {@code BigDecimal}. The scale is the number of
     * digits behind the decimal point. The value of this {@code BigDecimal} is
     * the {@code unsignedValue * 10<sup>-scale</sup>}. If the scale is negative,
     * then this {@code BigDecimal} represents a big integer.
     *
     * @return the scale of this {@code BigDecimal}.
     */
    CARAPI GetScale(
        /* [out] */ Int32* scale);

    /**
     * Returns the precision of this {@code BigDecimal}. The precision is the
     * number of decimal digits used to represent this decimal. It is equivalent
     * to the number of digits of the unscaled value. The precision of {@code 0}
     * is {@code 1} (independent of the scale).
     *
     * @return the precision of this {@code BigDecimal}.
     */
    CARAPI GetPrecision(
        /* [out] */ Int32* precision);

    /**
     * Returns the unscaled value (mantissa) of this {@code BigDecimal} instance
     * as a {@code BigInteger}. The unscaled value can be computed as
     * {@code this * 10<sup>scale</sup>}.
     */
    CARAPI GetUnscaledValue(
        /* [out] */ IBigInteger** unscaledValue);

    /**
     * Returns a new {@code BigDecimal} whose value is {@code this}, rounded
     * according to the passed context {@code mc}.
     * <p>
     * If {@code mc.precision = 0}, then no rounding is performed.
     * <p>
     * If {@code mc.precision > 0} and {@code mc.roundingMode == UNNECESSARY},
     * then an {@code ArithmeticException} is thrown if the result cannot be
     * represented exactly within the given precision.
     *
     * @param mc
     *            rounding mode and precision for the result of this operation.
     * @return {@code this} rounded according to the passed context.
     * @throws ArithmeticException
     *             if {@code mc.precision > 0} and {@code mc.roundingMode ==
     *             UNNECESSARY} and this cannot be represented within the given
     *             precision.
     */
    CARAPI Round(
        /* [in] */ IMathContext* mc,
        /* [out] */ IBigDecimal** result);

    /**
     * Returns a new {@code BigDecimal} instance with the specified scale.
     * <p>
     * If the new scale is greater than the old scale, then additional zeros are
     * added to the unscaled value. In this case no rounding is necessary.
     * <p>
     * If the new scale is smaller than the old scale, then trailing digits are
     * removed. If these trailing digits are not zero, then the remaining
     * unscaled value has to be rounded. For this rounding operation the
     * specified rounding mode is used.
     *
     * @param newScale
     *            scale of the result returned.
     * @param roundingMode
     *            rounding mode to be used to round the result.
     * @return a new {@code BigDecimal} instance with the specified scale.
     * @throws NullPointerException
     *             if {@code roundingMode == null}.
     * @throws ArithmeticException
     *             if {@code roundingMode == ROUND_UNNECESSARY} and rounding is
     *             necessary according to the given scale.
     */
    CARAPI SetScale(
        /* [in] */ Int32 newScale,
        /* [in] */ RoundingMode roundingMode,
        /* [out] */ IBigDecimal** result);

    /**
     * Returns a new {@code BigDecimal} instance with the specified scale. If
     * the new scale is greater than the old scale, then additional zeros are
     * added to the unscaled value. If the new scale is smaller than the old
     * scale, then trailing zeros are removed. If the trailing digits are not
     * zeros then an ArithmeticException is thrown.
     * <p>
     * If no exception is thrown, then the following equation holds: {@code
     * x.setScale(s).compareTo(x) == 0}.
     *
     * @param newScale
     *            scale of the result returned.
     * @return a new {@code BigDecimal} instance with the specified scale.
     * @throws ArithmeticException
     *             if rounding would be necessary.
     */
    CARAPI SetScale(
        /* [in] */ Int32 newScale,
        /* [out] */ IBigDecimal** result);

    /**
     * Returns a new {@code BigDecimal} instance where the decimal point has
     * been moved {@code n} places to the left. If {@code n < 0} then the
     * decimal point is moved {@code -n} places to the right.
     *
     * <p>The result is obtained by changing its scale. If the scale of the result
     * becomes negative, then its precision is increased such that the scale is
     * zero.
     *
     * <p>Note, that {@code movePointLeft(0)} returns a result which is
     * mathematically equivalent, but which has {@code scale >= 0}.
     */
    CARAPI MovePointLeft(
       /* [in] */ Int32 n,
       /* [out] */ IBigDecimal** result);

    /**
     * Returns a new {@code BigDecimal} instance where the decimal point has
     * been moved {@code n} places to the right. If {@code n < 0} then the
     * decimal point is moved {@code -n} places to the left.
     *
     * <p>The result is obtained by changing its scale. If the scale of the result
     * becomes negative, then its precision is increased such that the scale is
     * zero.
     *
     * <p>Note, that {@code movePointRight(0)} returns a result which is
     * mathematically equivalent, but which has scale >= 0.
     */
    CARAPI MovePointRight(
       /* [in] */ Int32 n,
       /* [out] */ IBigDecimal** result);

    /**
     * Returns a new {@code BigDecimal} whose value is {@code this * 10<sup>n</sup>}.
     * The scale of the result is {@code this.scale()} - {@code n}.
     * The precision of the result is the precision of {@code this}.
     *
     * <p>This method has the same effect as {@link #movePointRight}, except that
     * the precision is not changed.
     */
    CARAPI ScaleByPowerOfTen(
        /* [in] */ Int32 n,
        /* [out] */ IBigDecimal** result);

    /**
     * Returns a new {@code BigDecimal} instance with the same value as {@code
     * this} but with a unscaled value where the trailing zeros have been
     * removed. If the unscaled value of {@code this} has n trailing zeros, then
     * the scale and the precision of the result has been reduced by n.
     *
     * @return a new {@code BigDecimal} instance equivalent to this where the
     *         trailing zeros of the unscaled value have been removed.
     */
    CARAPI StripTrailingZeros(
       /* [out] */ IBigDecimal** result);

    /**
     * Compares this {@code BigDecimal} with {@code val}. Returns one of the
     * three values {@code 1}, {@code 0}, or {@code -1}. The method behaves as
     * if {@code this.subtract(val)} is computed. If this difference is > 0 then
     * 1 is returned, if the difference is < 0 then -1 is returned, and if the
     * difference is 0 then 0 is returned. This means, that if two decimal
     * instances are compared which are equal in value but differ in scale, then
     * these two instances are considered as equal.
     *
     * @param other
     *            value to be compared with {@code this}.
     * @return {@code 1} if {@code this > val}, {@code -1} if {@code this < val},
     *         {@code 0} if {@code this == val}.
     * @throws NullPointerException
     *             if {@code val == null}.
     */
    CARAPI CompareTo(
        /* [in] */ IInterface* other,
        /* [out] */ Int32* result);

    /**
     * Returns {@code true} if {@code x} is a {@code BigDecimal} instance and if
     * this instance is equal to this big decimal. Two big decimals are equal if
     * their unscaled value and their scale is equal. For example, 1.0
     * (10*10<sup>-1</sup>) is not equal to 1.00 (100*10<sup>-2</sup>). Similarly, zero
     * instances are not equal if their scale differs.
     */
    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* result);

    /**
     * Returns the minimum of this {@code BigDecimal} and {@code val}.
     *
     * @param val
     *            value to be used to compute the minimum with this.
     * @return {@code min(this, val}.
     * @throws NullPointerException
     *             if {@code val == null}.
     */
    CARAPI Min(
        /* [in] */ IBigDecimal* val,
        /* [out] */ IBigDecimal** result);

    /**
     * Returns the maximum of this {@code BigDecimal} and {@code val}.
     *
     * @param val
     *            value to be used to compute the maximum with this.
     * @return {@code max(this, val}.
     * @throws NullPointerException
     *             if {@code val == null}.
     */
    CARAPI Max(
        /* [in] */ IBigDecimal* val,
        /* [out] */ IBigDecimal** result);

    /**
     * Returns a canonical string representation of this {@code BigDecimal}. If
     * necessary, scientific notation is used. This representation always prints
     * all significant digits of this value.
     * <p>
     * If the scale is negative or if {@code scale - precision >= 6} then
     * scientific notation is used.
     *
     * @return a string representation of {@code this} in scientific notation if
     *         necessary.
     */
    CARAPI ToString(
        /* [out] */ String*  str);

    /**
     * Returns a string representation of this {@code BigDecimal}. This
     * representation always prints all significant digits of this value.
     * <p>
     * If the scale is negative or if {@code scale - precision >= 6} then
     * engineering notation is used. Engineering notation is similar to the
     * scientific notation except that the exponent is made to be a multiple of
     * 3 such that the integer part is >= 1 and < 1000.
     *
     * @return a string representation of {@code this} in engineering notation
     *         if necessary.
     */
    CARAPI ToEngineeringString(
        /* [out] */ String* representation);

    /**
     * Returns a string representation of this {@code BigDecimal}. No scientific
     * notation is used. This methods adds zeros where necessary.
     * <p>
     * If this string representation is used to create a new instance, this
     * instance is generally not identical to {@code this} as the precision
     * changes.
     * <p>
     * {@code x.equals(new BigDecimal(x.toPlainString())} usually returns
     * {@code false}.
     * <p>
     * {@code x.compareTo(new BigDecimal(x.toPlainString())} returns {@code 0}.
     *
     * @return a string representation of {@code this} without exponent part.
     */
    CARAPI ToPlainString(
        /* [out] */ String* representation);

    /**
     * Returns this {@code BigDecimal} as a big integer instance. A fractional
     * part is discarded.
     *
     * @return this {@code BigDecimal} as a big integer instance.
     */
    CARAPI ToBigInteger(
        /* [out] */ IBigInteger** result);

    /**
     * Returns this {@code BigDecimal} as a big integer instance if it has no
     * fractional part. If this {@code BigDecimal} has a fractional part, i.e.
     * if rounding would be necessary, an {@code ArithmeticException} is thrown.
     *
     * @return this {@code BigDecimal} as a big integer value.
     * @throws ArithmeticException
     *             if rounding is necessary.
     */
    CARAPI ToBigIntegerExact(
        /* [out] */ IBigInteger** result);

    /**
     * Returns this {@code BigDecimal} as a long value if it has no fractional
     * part and if its value fits to the int range ([-2<sup>63</sup>..2<sup>63</sup>-1]). If
     * these conditions are not met, an {@code ArithmeticException} is thrown.
     *
     * @throws ArithmeticException
     *             if rounding is necessary or the number doesn't fit in a long.
     */
    CARAPI Int64ValueExact(
        /* [out] */ Int64* value);

    /**
     * Returns this {@code BigDecimal} as a int value if it has no fractional
     * part and if its value fits to the int range ([-2<sup>31</sup>..2<sup>31</sup>-1]). If
     * these conditions are not met, an {@code ArithmeticException} is thrown.
     *
     * @throws ArithmeticException
     *             if rounding is necessary or the number doesn't fit in an int.
     */
    CARAPI Int32ValueExact(
        /* [out] */ Int32* value);

    /**
     * Returns this {@code BigDecimal} as a short value if it has no fractional
     * part and if its value fits to the short range ([-2<sup>15</sup>..2<sup>15</sup>-1]). If
     * these conditions are not met, an {@code ArithmeticException} is thrown.
     *
     * @throws ArithmeticException
     *             if rounding is necessary of the number doesn't fit in a short.
     */
    CARAPI Int16ValueExact(
        /* [out] */ Int16* value);

    /**
     * Returns this {@code BigDecimal} as a byte value if it has no fractional
     * part and if its value fits to the byte range ([-128..127]). If these
     * conditions are not met, an {@code ArithmeticException} is thrown.
     *
     * @throws ArithmeticException
     *             if rounding is necessary or the number doesn't fit in a byte.
     */
    CARAPI ByteValueExact(
        /* [out] */ Byte* value);

    /**
     * Returns the unit in the last place (ULP) of this {@code BigDecimal}
     * instance. An ULP is the distance to the nearest big decimal with the same
     * precision.
     *
     * <p>The amount of a rounding error in the evaluation of a floating-point
     * operation is often expressed in ULPs. An error of 1 ULP is often seen as
     * a tolerable error.
     *
     * <p>For class {@code BigDecimal}, the ULP of a number is simply 10<sup>-scale</sup>.
     * For example, {@code new BigDecimal(0.1).ulp()} returns {@code 1E-55}.
     *
     * @return unit in the last place (ULP) of this {@code BigDecimal} instance.
     */
    CARAPI Ulp(
        /* [out] */ IBigDecimal** newObj);

    /*=================================INumber================================*/

    /**
     * Returns this object's value as a byte. Might involve rounding and/or
     * truncating the value, so it fits into a byte.
     *
     * @return the primitive byte value of this object.
     */
    CARAPI ByteValue(
        /* [out] */  Byte* result);

    /**
     * Returns this object's value as a short. Might involve rounding and/or
     * truncating the value, so it fits into a short.
     *
     * @return the primitive short value of this object.
     */
    CARAPI Int16Value(
        /* [out] */  Int16* result);

    /**
     * Returns this {@code BigDecimal} as an int value. Any fractional part is
     * discarded. If the integral part of {@code this} is too big to be
     * represented as an int, then {@code this % 2<sup>32</sup>} is returned.
     */
    CARAPI Int32Value(
        /* [out] */  Int32* result);

    /**
     * Returns this {@code BigDecimal} as an long value. Any fractional part is
     * discarded. If the integral part of {@code this} is too big to be
     * represented as an long, then {@code this % 2<sup>64</sup>} is returned.
     */
    CARAPI Int64Value(
        /* [out] */  Int64* result);

    /**
     * Returns this {@code BigDecimal} as a float value. If {@code this} is too
     * big to be represented as an float, then {@code Float.POSITIVE_INFINITY}
     * or {@code Float.NEGATIVE_INFINITY} is returned.
     * <p>
     * Note, that if the unscaled value has more than 24 significant digits,
     * then this decimal cannot be represented exactly in a float variable. In
     * this case the result is rounded.
     * <p>
     * For example, if the instance {@code x1 = new BigDecimal("0.1")} cannot be
     * represented exactly as a float, and thus {@code x1.equals(new
     * BigDecimal(x1.floatValue())} returns {@code false} for this case.
     * <p>
     * Similarly, if the instance {@code new BigDecimal(16777217)} is converted
     * to a float, the result is {@code 1.6777216E}7.
     *
     * @return this {@code BigDecimal} as a float value.
     */
    CARAPI FloatValue(
        /* [out] */  Float* result);

    /**
     * Returns this object's value as a double. Might involve rounding.
     *
     * @return the primitive double value of this object.
     */
    CARAPI DoubleValue(
        /* [out] */  Double* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

private:
    static Int32 BitLength(
        /* [in] */ Int32 smallValue);

    static Int32 BitLength(
        /* [in] */ Int64 smallValue);

    void SetUnscaledValue(
        /* [in] */ IBigInteger* unscalevalue);
    AutoPtr<IBigInteger> GetUnscaledValue();

    Boolean IsZero();

    static CARAPI AddAndMult10(
        /* [in] */ CBigDecimal* thisValue,
        /* [in] */ CBigDecimal* augend,
        /* [in] */ Int32 diffScale,
        /* [out] */ IBigDecimal** result);

    static CARAPI DivideBigIntegers(
        /* [in] */ CBigInteger* scaledDividend,
        /* [in] */ CBigInteger* scaledDivisor,
        /* [in] */ Int32 scale,
        /* [in] */ RoundingMode roundingMode,
        /* [out] */ IBigDecimal** result);

    static CARAPI DividePrimitiveLongs(
        /* [in] */ Int64 scaledDividend,
        /* [in] */ Int64 scaledDivisor,
        /* [in] */ Int32 scale,
        /* [in] */ RoundingMode roundingMode,
        /* [out] */ IBigDecimal** result);

    /**
     * It returns the value 0 with the most approximated scale of type
     * {@code int}. if {@code longScale > Integer.MAX_VALUE} the
     * scale will be {@code Integer.MAX_VALUE}; if
     * {@code longScale < Integer.MIN_VALUE} the scale will be
     * {@code Integer.MIN_VALUE}; otherwise {@code longScale} is
     * casted to the type {@code int}.
     *
     * @param longScale
     *            the scale to which the value 0 will be scaled.
     * @return the value 0 scaled by the closer scale of type {@code int}.
     * @see #scale
     */
    static CARAPI ZeroScaledByScale(
        /* [in] */ Int64 longScale,
        /* [out] */ IBigDecimal** result);

    /**
     * If the precision already was calculated it returns that value, otherwise
     * it calculates a very good approximation efficiently . Note that this
     * value will be {@code precision()} or {@code precision()-1}
     * in the worst case.
     *
     * @return an approximation of {@code precision()} value
     */
    Int32 ApproxPrecision();

    static CARAPI SafeLongToInt(
        /* [in] */ Int64 longValue,
        /* [out] */ Int32* result);

    /**
     * It does all rounding work of the public method
     * {@code round(MathContext)}, performing an inplace rounding
     * without creating a new object.
     *
     * @param mc
     *            the {@code MathContext} for perform the rounding.
     * @see #round(MathContext)
     */
    CARAPI InplaceRound(
        /* [in] */ IMathContext* mc);

    /**
     * This method implements an efficient rounding for numbers which unscaled
     * value fits in the type {@code long}.
     *
     * @param mc
     *            the context to use
     * @param discardedPrecision
     *            the number of decimal digits that are discarded
     * @see #round(MathContext)
     */
    CARAPI SmallRound(
        /* [in] */ IMathContext* mc,
        /* [in] */ Int32 discardedPrecision);

    Int32 DecimalDigitsInLong(
        /* [in] */ Int64 value);

    /**
     * Return an increment that can be -1,0 or 1, depending of
     * {@code roundingMode}.
     *
     * @param parityBit
     *            can be 0 or 1, it's only used in the case
     *            {@code HALF_EVEN}
     * @param fraction
     *            the mantissa to be analyzed
     * @param roundingMode
     *            the type of rounding
     * @return the carry propagated after rounding
     */
    static CARAPI RoundingBehavior(
        /* [in] */ Int32 parityBit,
        /* [in] */ Int32 fraction,
        /* [in] */ RoundingMode roundingMode,
        /* [out] */ Int32* result);


    static Int32 LongCompareTo(
        /* [in] */ Int64 value1,
        /* [in] */ Int64 value2)
    {
        return value1 > value2 ? 1 : (value1 < value2 ? -1 : 0);
    }

    CARAPI MovePoint(
        /* [in] */ Int64 newScale,
        /* [out] */ IBigDecimal** result);

    CARAPI ValueExact(
        /* [in] */ Int32 bitLengthOfType,
        /* [out] */ Int64* result);

    static CARAPI_(AutoPtr< ArrayOf<Int64> >) INIT_LONG_FIVE_POW();

public:
    /**
     * The constant zero as a {@code BigDecimal}.
     */
    static AutoPtr<IBigDecimal> ZERO;

    /**
     * The constant one as a {@code BigDecimal}.
     */
    static AutoPtr<IBigDecimal> ONE;

    /**
     * The constant ten as a {@code BigDecimal}.
     */
    static AutoPtr<IBigDecimal> TEN;

private:
    /** The double closest to {@code Log10(2)}. */
    static const Double Log10_2;

    /** The <code>String</code> representation is cached. */
    String mToStringImage;

    /**
     * An array with powers of five that fit in the type <code>long</code>
     * (<code>5^0,5^1,...,5^27</code>).
     */
    static AutoPtr< ArrayOf<IBigInteger*> > FIVE_POW;

    /**
     * An array with powers of ten that fit in the type <code>long</code>
     * (<code>10^0,10^1,...,10^18</code>).
     */
    static AutoPtr< ArrayOf<IBigInteger*> > TEN_POW;

    static const AutoPtr< ArrayOf<Int64> > LONG_FIVE_POW;
    static const AutoPtr< ArrayOf<Int32> > LONG_FIVE_POW_BIT_LENGTH;

    static const AutoPtr< ArrayOf<Int32> > LONG_POWERS_OF_TEN_BIT_LENGTH;

    static const Int32 BI_SCALED_BY_ZERO_LENGTH = 11;

    /**
     * An array with the first <code>BigInteger</code> scaled by zero.
     * (<code>[0,0],[1,0],...,[10,0]</code>).
     */
    static AutoPtr< ArrayOf<IBigDecimal*> > BI_SCALED_BY_ZERO;

    /**
     * An array with the zero number scaled by the first positive scales.
     * (<code>0*10^0, 0*10^1, ..., 0*10^10</code>).
     */
    static AutoPtr< ArrayOf<IBigDecimal*> > ZERO_SCALED_BY;

    /** An array filled with characters <code>'0'</code>. */
    static AutoPtr< ArrayOf<Char32> > CH_ZEROS;

    /**
     * The arbitrary precision integer (unscaled value) in the internal
     * representation of {@code BigDecimal}.
     */
    AutoPtr<IBigInteger> mIntVal;

    Int32 mBitLength;
    Int64 mSmallValue;

    /**
     * The 32-bit integer scale in the internal representation of {@code BigDecimal}.
     */
    Int32 mScale;

    /**
     * Represent the number of decimal digits in the unscaled value. This
     * precision is calculated the first time, and used in the following calls
     * of method <code>precision()</code>. Note that some call to the private
     * method <code>inplaceRound()</code> could update this field.
     *
     * @see #precision()
     * @see #inplaceRound(MathContext)
     */
    Int32 mPrecision;

    /** Cache for the hash code. */
    Int32 mHashCode;
    static StaticInitializer sInitializer;
};

} // namespace Math
} // namespace Elastos

#endif // __ELASTOS_MATH_CBIGDECIMAL_H__
