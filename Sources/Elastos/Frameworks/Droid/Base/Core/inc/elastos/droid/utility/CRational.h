
#ifndef __ELASTOS_DROID_UTILITY_CRATIONAL_H__
#define __ELASTOS_DROID_UTILITY_CRATIONAL_H__

#include "_Elastos_Droid_Utility_CRational.h"
#include <elastos/core/Object.h>

using Elastos::Core::INumber;
using Elastos::Core::IComparable;

namespace Elastos {
namespace Droid {
namespace Utility {

/**
 * <p>An immutable data type representation a rational number.</p>
 *
 * <p>Contains a pair of {@code int}s representing the numerator and denominator of a
 * Rational number. </p>
 */
CarClass(CRational)
    , public Object
    , public IRational
    , public INumber
    , public IComparable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CRational();

    /**
     * <p>Create a {@code Rational} with a given numerator and denominator.</p>
     *
     * <p>The signs of the numerator and the denominator may be flipped such that the denominator
     * is always positive. Both the numerator and denominator will be converted to their reduced
     * forms (see {@link #equals} for more details).</p>
     *
     * <p>For example,
     * <ul>
     * <li>a rational of {@code 2/4} will be reduced to {@code 1/2}.
     * <li>a rational of {@code 1/-1} will be flipped to {@code -1/1}
     * <li>a rational of {@code 5/0} will be reduced to {@code 1/0}
     * <li>a rational of {@code 0/5} will be reduced to {@code 0/1}
     * </ul>
     * </p>
     *
     * @param numerator the numerator of the rational
     * @param denominator the denominator of the rational
     *
     * @see #equals
     */
    CARAPI constructor(
        /* [in] */ Int32 numerator,
        /* [in] */ Int32 denominator);

    /**
     * Gets the numerator of the rational.
     *
     * <p>The numerator will always return {@code 1} if this rational represents
     * infinity (that is, the denominator is {@code 0}).</p>
     */
    CARAPI GetNumerator(
        /* [out] */ Int32* value);

    /**
     * Gets the denominator of the rational
     *
     * <p>The denominator may return {@code 0}, in which case the rational may represent
     * positive infinity (if the numerator was positive), negative infinity (if the numerator
     * was negative), or {@code NaN} (if the numerator was {@code 0}).</p>
     *
     * <p>The denominator will always return {@code 1} if the numerator is {@code 0}.
     */
    CARAPI GetDenominator(
        /* [out] */ Int32* value);

    /**
     * Indicates whether this rational is a <em>Not-a-Number (NaN)</em> value.
     *
     * <p>A {@code NaN} value occurs when both the numerator and the denominator are {@code 0}.</p>
     *
     * @return {@code true} if this rational is a <em>Not-a-Number (NaN)</em> value;
     *         {@code false} if this is a (potentially infinite) number value
     */
    CARAPI IsNaN(
        /* [out] */ Boolean* value);

    /**
     * Indicates whether this rational represents an infinite value.
     *
     * <p>An infinite value occurs when the denominator is {@code 0} (but the numerator is not).</p>
     *
     * @return {@code true} if this rational is a (positive or negative) infinite value;
     *         {@code false} if this is a finite number value (or {@code NaN})
     */
    CARAPI IsInfinite(
        /* [out] */ Boolean* value);

    /**
     * Indicates whether this rational represents a finite value.
     *
     * <p>A finite value occurs when the denominator is not {@code 0}; in other words
     * the rational is neither infinity or {@code NaN}.</p>
     *
     * @return {@code true} if this rational is a (positive or negative) infinite value;
     *         {@code false} if this is a finite number value (or {@code NaN})
     */
    CARAPI IsFinite(
        /* [out] */ Boolean* value);

    /**
     * Indicates whether this rational represents a zero value.
     *
     * <p>A zero value is a {@link #isFinite finite} rational with a numerator of {@code 0}.</p>
     *
     * @return {@code true} if this rational is finite zero value;
     *         {@code false} otherwise
     */
    CARAPI IsZero(
        /* [out] */ Boolean* value);

    Boolean IsPosInf();

    Boolean IsNegInf();

    /**
     * <p>Compare this Rational to another object and see if they are equal.</p>
     *
     * <p>A Rational object can only be equal to another Rational object (comparing against any
     * other type will return {@code false}).</p>
     *
     * <p>A Rational object is considered equal to another Rational object if and only if one of
     * the following holds:</p>
     * <ul><li>Both are {@code NaN}</li>
     *     <li>Both are infinities of the same sign</li>
     *     <li>Both have the same numerator and denominator in their reduced form</li>
     * </ul>
     *
     * <p>A reduced form of a Rational is calculated by dividing both the numerator and the
     * denominator by their greatest common divisor.</p>
     *
     * <pre>{@code
     * (new Rational(1, 2)).equals(new Rational(1, 2)) == true   // trivially true
     * (new Rational(2, 3)).equals(new Rational(1, 2)) == false  // trivially false
     * (new Rational(1, 2)).equals(new Rational(2, 4)) == true   // true after reduction
     * (new Rational(0, 0)).equals(new Rational(0, 0)) == true   // NaN.equals(NaN)
     * (new Rational(1, 0)).equals(new Rational(5, 0)) == true   // both are +infinity
     * (new Rational(1, 0)).equals(new Rational(-1, 0)) == false // +infinity != -infinity
     * }</pre>
     *
     * @param obj a reference to another object
     *
     * @return A boolean that determines whether or not the two Rational objects are equal.
     */
    // @Override
    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* value);

    /**
     * Return a string representation of this rational, e.g. {@code "1/2"}.
     *
     * <p>The following rules of conversion apply:
     * <ul>
     * <li>{@code NaN} values will return {@code "NaN"}
     * <li>Positive infinity values will return {@code "Infinity"}
     * <li>Negative infinity values will return {@code "-Infinity"}
     * <li>All other values will return {@code "numerator/denominator"} where {@code numerator}
     * and {@code denominator} are substituted with the appropriate numerator and denominator
     * values.
     * </ul></p>
     */
    // @Override
    CARAPI ToString(
        /* [out] */ String* str);

    /**
     * <p>Convert to a floating point representation.</p>
     *
     * @return The floating point representation of this rational number.
     * @hide
     */
    CARAPI ToFloat(
        /* [out] */ Float* value);

    /**
     * {@inheritDoc}
     */
    // @Override
    CARAPI GetHashCode(
        /* [out] */ Int32* value);

    /**
     * Calculates the greatest common divisor using Euclid's algorithm.
     *
     * <p><em>Visible for testing only.</em></p>
     *
     * @param numerator the numerator in a fraction
     * @param denominator the denominator in a fraction
     *
     * @return An int value representing the gcd. Always positive.
     * @hide
     */
    static Int32 Gcd(
        /* [in] */ Int32 numerator,
        /* [in] */ Int32 denominator);

    CARAPI ByteValue(
        /* [out] */ Byte* result);

    CARAPI Int16Value(
        /* [out] */ Int16* result);

    CARAPI Int32Value(
        /* [out] */ Int32* result);

    CARAPI Int64Value(
        /* [out] */ Int64* result);

    CARAPI FloatValue(
        /* [out] */ Float* result);

    CARAPI DoubleValue(
        /* [out] */ Double* result);

    /**
     * Compare this rational to the specified rational to determine their natural order.
     *
     * <p>{@link #NaN} is considered to be equal to itself and greater than all other
     * {@code Rational} values. Otherwise, if the objects are not {@link #equals equal}, then
     * the following rules apply:</p>
     *
     * <ul>
     * <li>Positive infinity is greater than any other finite number (or negative infinity)
     * <li>Negative infinity is less than any other finite number (or positive infinity)
     * <li>The finite number represented by this rational is checked numerically
     * against the other finite number by converting both rationals to a common denominator multiple
     * and comparing their numerators.
     * </ul>
     *
     * @param another the rational to be compared
     *
     * @return a negative integer, zero, or a positive integer as this object is less than,
     *         equal to, or greater than the specified rational.
     *
     * @throws NullPointerException if {@code another} was {@code null}
     */
    CARAPI CompareTo(
        /* [in] */ IInterface* other,
        /* [out] */ Int32* result);

    /**
     * Parses the specified string as a rational value.
     * <p>The ASCII characters {@code \}{@code u003a} (':') and
     * {@code \}{@code u002f} ('/') are recognized as separators between
     * the numerator and denumerator.</p>
     * <p>
     * For any {@code Rational r}: {@code Rational.parseRational(r.toString()).equals(r)}.
     * However, the method also handles rational numbers expressed in the
     * following forms:</p>
     * <p>
     * "<i>num</i>{@code /}<i>den</i>" or
     * "<i>num</i>{@code :}<i>den</i>" {@code => new Rational(num, den);},
     * where <i>num</i> and <i>den</i> are string integers potentially
     * containing a sign, such as "-10", "+7" or "5".</p>
     *
     * <pre>{@code
     * Rational.parseRational("3:+6").equals(new Rational(1, 2)) == true
     * Rational.parseRational("-3/-6").equals(new Rational(1, 2)) == true
     * Rational.parseRational("4.56") => throws NumberFormatException
     * }</pre>
     *
     * @param string the string representation of a rational value.
     * @return the rational value represented by {@code string}.
     *
     * @throws NumberFormatException if {@code string} cannot be parsed
     * as a rational value.
     * @throws NullPointerException if {@code string} was {@code null}
     */
     static ECode ParseRational(
        /* [in] */ const String& string,
        /* [out] */ IRational** result);

public:
    /**
     * Constant for the <em>Not-a-Number (NaN)</em> value of the {@code Rational} type.
     *
     * <p>A {@code NaN} value is considered to be equal to itself (that is {@code NaN.equals(NaN)}
     * will return {@code true}; it is always greater than any non-{@code NaN} value (that is
     * {@code NaN.compareTo(notNaN)} will return a number greater than {@code 0}).</p>
     *
     * <p>Equivalent to constructing a new rational with both the numerator and denominator
     * equal to {@code 0}.</p>
     */
    static const AutoPtr<IRational> NaN;// = new Rational(0, 0);

    /**
     * Constant for the positive infinity value of the {@code Rational} type.
     *
     * <p>Equivalent to constructing a new rational with a positive numerator and a denominator
     * equal to {@code 0}.</p>
     */
    static const AutoPtr<IRational> POSITIVE_INFINITY;// = new Rational(1, 0);

    /**
     * Constant for the negative infinity value of the {@code Rational} type.
     *
     * <p>Equivalent to constructing a new rational with a negative numerator and a denominator
     * equal to {@code 0}.</p>
     */
    static const AutoPtr<IRational> NEGATIVE_INFINITY;// = new Rational(-1, 0);

    /**
     * Constant for the zero value of the {@code Rational} type.
     *
     * <p>Equivalent to constructing a new rational with a numerator equal to {@code 0} and
     * any non-zero denominator.</p>
     */
    static const AutoPtr<IRational> ZERO;// = new Rational(0, 1);

private:
    /*
     * Do not change the order of these fields or add new instance fields to maintain the
     * Serializable compatibility across API revisions.
     */
    Int32 mNumerator;
    Int32 mDenominator;
};

} // namespace Utility
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_UTILITY_CRATIONAL_H__
