
#include "elastos/droid/utility/FloatMath.h"
#include <math.h>
#include <float.h>

namespace Elastos {
namespace Droid {
namespace Utility {

/**
 * Returns the float conversion of the most positive (i.e. closest to
 * positive infinity) integer value which is less than the argument.
 *
 * @param value to be converted
 * @return the floor of value
 */
Float FloatMath::Floor(
    /* [in] */ Float value)
{
    return floorf(value);
}

/**
 * Returns the float conversion of the most negative (i.e. closest to
 * negative infinity) integer value which is greater than the argument.
 *
 * @param value to be converted
 * @return the ceiling of value
 */
Float FloatMath::Ceil(
    /* [in] */ Float value)
{
    return ceilf(value);
}

/**
 * Returns the closest float approximation of the sine of the argument.
 *
 * @param angle to compute the cosine of, in radians
 * @return the sine of angle
 */
Float FloatMath::Sin(
    /* [in] */ Float angle)
{
    return sinf(angle);
}

/**
 * Returns the closest float approximation of the cosine of the argument.
 *
 * @param angle to compute the cosine of, in radians
 * @return the cosine of angle
 */
Float FloatMath::Cos(
    /* [in] */ Float angle)
{
    return cosf(angle);
}

/**
 * Returns the closest float approximation of the square root of the
 * argument.
 *
 * @param value to compute sqrt of
 * @return the square root of value
 */
Float FloatMath::Sqrt(
    /* [in] */ Float value)
{
    return sqrtf(value);
}

/**
 * Returns the closest float approximation of the raising "e" to the power
 * of the argument.
 *
 * @param value to compute the exponential of
 * @return the exponential of value
 */
Float FloatMath::Exp(
    /* [in] */ Float value)
{
    return expf(value);
}

/**
 * Returns the closest float approximation of the result of raising {@code
 * x} to the power of {@code y}.
 *
 * @param x the base of the operation.
 * @param y the exponent of the operation.
 * @return {@code x} to the power of {@code y}.
 */
Float FloatMath::Pow(
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    return powf(x, y);
}

/**
 * Returns {@code sqrt(}<i>{@code x}</i><sup>{@code 2}</sup>{@code +} <i>
 * {@code y}</i><sup>{@code 2}</sup>{@code )}.
 *
 * @param x a float number
 * @param y a float number
 * @return the hypotenuse
 */
Float FloatMath::Hypot(
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    return hypotf(x, y);
}

} // namespace Utility
} // namepsace Droid
} // namespace Elastos
