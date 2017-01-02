//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
