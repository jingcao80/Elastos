#ifndef __ELASTOS_MATH_CONERSION_H__
#define __ELASTOS_MATH_CONERSION_H__

#include <CBigInteger.h>
#include <elastos.h>
#include <elquintet.h>

namespace Elastos {
namespace Math {

class Conversion
{
public:

    /** @see BigInteger#toString(int) */
    static String BigInteger2String(
        /* [in] */ CBigInteger& val,
        /* [in] */ Int32 radix);

    /**
     * Builds the correspondent {@code String} representation of {@code val}
     * being scaled by {@code scale}.
     *
     * @see BigInteger#toString()
     * @see BigDecimal#toString()
     */
    static String ToDecimalScaledString(
        /* [in] */ CBigInteger& val,
        /* [in] */ Int32 scale);

    /* can process only 32-bit numbers */
    static String ToDecimalScaledString(
        /* [in] */ Int64 value,
        /* [in] */ Int32 scale);

    static Int64 DivideLongByBillion(
        /* [in] */ Int64 a);

    /** @see BigInteger#doubleValue() */
    static Double BigInteger2Double(
        /* [in] */ CBigInteger& val);

private:
    /** Just to denote that this class can't be instantiated. */
    Conversion();

    Conversion(
        /* [in] */ const Conversion&);

    Conversion& operator = (
        /* [in] */ const Conversion&);

public:
    /**
     * Holds the maximal exponent for each radix, so that radix<sup>digitFitInInt[radix]</sup>
     * fit in an {@code int} (32 bits).
     */
    static const Int32 DigitFitInIntLength = 37;
    static const Int32 DigitFitInInt[DigitFitInIntLength];

    /**
     * bigRadices values are precomputed maximal powers of radices (integer
     * numbers from 2 to 36) that fit into unsigned int (32 bits). bigRadices[0] =
     * 2 ^ 31, bigRadices[8] = 10 ^ 9, etc.
     */
    static const Int32 BigRadicesLength = 35;
    static const Int32 BigRadices[BigRadicesLength];
};

} // namespace Math
} // namespace Elastos

#endif // __ELASTOS_MATH_CONERSION_H__
