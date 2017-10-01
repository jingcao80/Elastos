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

#include "StrictMath.h"
#include "Math.h"
#include <fdlibm.h>

namespace Elastos {
namespace Core {

const Double StrictMath::E                        = 2.718281828459045;
const Double StrictMath::PI                       = 3.141592653589793;

Double StrictMath::Abs(
    /* [in] */ Double d)
{
    return Math::Abs(d);
}

Float StrictMath::Abs(
    /* [in] */ Float f)
{
    return Math::Abs(f);
}

Int32 StrictMath::Abs(
    /* [in] */ Int32 i)
{
    return i >= 0 ? i : -i;
}

Int64 StrictMath::Abs(
    /* [in] */Int64 i)
{
    return (i >= 0) ? i : -i;
}

Double StrictMath::Acos(
    /* [in] */ Double d)
{
    return ieee_acos(d);
}

Double StrictMath::Asin(
    /* [in] */ Double d)
{
    return ieee_asin(d);
}

Double StrictMath::Atan(
    /* [in] */ Double d)
{
    return ieee_atan(d);
}

Double StrictMath::Atan2(
    /* [in] */ Double y,
    /* [in] */ Double x)
{
    return ieee_atan2(y, x);
}

Double StrictMath::Cbrt(
    /* [in] */ Double d)
{
    return ieee_cbrt(d);
}

Double StrictMath::Ceil(
    /* [in] */ Double d)
{
    return ieee_ceil(d);
}

Double StrictMath::Cos(
    /* [in] */ Double d)
{
    return ieee_cos(d);
}

Double StrictMath::Cosh(
    /* [in] */ Double d)
{
    return ieee_cosh(d);
}

Double StrictMath::Exp(
    /* [in] */ Double d)
{
    return ieee_exp(d);
}

Double StrictMath::Expm1(
    /* [in] */ Double d)
{
    return ieee_expm1(d);
}

Double StrictMath::Floor(
    /* [in] */ Double d)
{
    return ieee_floor(d);
}

Double StrictMath::Hypot(
    /* [in] */ Double a,
     /* [in] */ Double b)
{
    return ieee_hypot(a, b);
}

Double StrictMath::IEEEremainder(
    /* [in] */ Double x,
     /* [in] */ Double y)
{
    return ieee_remainder(x, y);
}

Double StrictMath::Log(
    /* [in] */ Double d)
{
    return ieee_log(d);
}

Double StrictMath::Log10(
    /* [in] */ Double d)
{
    return ieee_log10(d);
}

Double StrictMath::Log1p(
    /* [in] */ Double d)
{
    return ieee_log1p(d);
}

Double StrictMath::Max(
    /* [in] */ Double d1,
    /* [in] */ Double d2)
{
    if (d1 > d2)
        return d1;
    if (d1 < d2)
        return d2;
    /* if either arg is NaN, return NaN */
    if (d1 != d2)
        return Math::DOUBLE_NAN;
    /* max( +0.0,-0.0) == +0.0 */
    if (d1 == 0.0
            && ((Math::DoubleToInt64Bits(d1) & Math::DoubleToInt64Bits(d2)) & 0x8000000000000000L) == 0)
        return 0.0;
    return d1;
}

Float StrictMath::Max(
    /* [in] */ Float f1,
    /* [in] */ Float f2)
{
    if (f1 > f2) {
        return f1;
    }
    if (f1 < f2) {
        return f2;
    }
    /* if either arg is NaN, return NaN */
    if (f1 != f2) {
        return Math::FLOAT_NAN;
    }
    /* max( +0.0,-0.0) == +0.0 */
    if (f1 == 0.0f
            && ((Math::FloatToInt32Bits(f1) & Math::FloatToInt32Bits(f2)) & 0x80000000) == 0)
        return 0.0f;
    return f1;
}

Int32 StrictMath::Max(
    /* [in] */ Int32 i1,
    /* [in] */ Int32 i2)
{
    return i1 > i2 ? i1 : i2;
}

Int64 StrictMath::Max(
    /* [in] */ Int64 l1,
    /* [in] */ Int64 l2)
{
    return l1 > l2 ? l1 : l2;
}

Double StrictMath::Min(
    /* [in] */ Double d1,
    /* [in] */ Double d2)
{
    if (d1 > d2) {
        return d2;
    }
    if (d1 < d2) {
        return d1;
    }
    /* if either arg is NaN, return NaN */
    if (d1 != d2) {
        return Math::DOUBLE_NAN;
    }
    /* min( +0.0,-0.0) == -0.0 */
    if (d1 == 0.0
            && ((Math::DoubleToInt64Bits(d1) | Math::DoubleToInt64Bits(d2)) & 0x8000000000000000l) != 0)
        return 0.0 * (-1.0);
    return d1;
}

Float StrictMath::Min(
    /* [in] */ Float f1,
    /* [in] */ Float f2)
{
    if (f1 > f2) {
        return f2;
    }
    if (f1 < f2) {
        return f1;
    }
    /* if either arg is NaN, return NaN */
    if (f1 != f2) {
        return Math::FLOAT_NAN;
    }
    /* min( +0.0,-0.0) == -0.0 */
    if (f1 == 0.0f
            && ((Math::FloatToInt32Bits(f1) | Math::FloatToInt32Bits(f2)) & 0x80000000) != 0)
        return 0.0f * (-1.0f);
    return f1;
}

Int32 StrictMath::Min(
    /* [in] */ Int32 i1,
    /* [in] */ Int32 i2)
{
    return i1 < i2 ? i1 : i2;
}

Int64 StrictMath::Min(
    /* [in] */ Int64 l1,
    /* [in] */ Int64 l2)
{
    return l1 < l2 ? l1 : l2;
}

Double StrictMath::Pow(
    /* [in] */ Double x,
    /* [in] */ Double y)
{
    return ieee_pow(x, y);
}

Double StrictMath::Random()
{
    return Math::Random();
}

Double StrictMath::Rint(
    /* [in] */ Double d)
{
    return ieee_rint(d);
}

Int64 StrictMath::Round(
    /* [in] */ Double d)
{
    return Math::Round(d);
}

Int32 StrictMath::Round(
    /* [in] */ Float f)
{
    return Math::Round(f);
}

Double StrictMath::Signum(
    /* [in] */ Double d)
{
    return Math::Signum(d);
}

Float StrictMath::Signum(
    /* [in] */ Float f)
{
    return Math::Signum(f);
}

Double StrictMath::Sin(
    /* [in] */ Double d)
{
    return ieee_sin(d);
}

Double StrictMath::Sinh(
    /* [in] */ Double d)
{
    return ieee_sinh(d);
}

Double StrictMath::Sqrt(
    /* [in] */ Double d)
{
    return ieee_sqrt(d);
}

Double StrictMath::Tan(
    /* [in] */ Double d)
{
    return ieee_tan(d);
}

Double StrictMath::Tanh(
    /* [in] */ Double d)
{
    return ieee_tanh(d);
}

Double StrictMath::ToRadians(
    /* [in] */ Double angdeg)
{
    return angdeg / 180.0l * PI;
}

Double StrictMath::ToDegrees(
    /* [in] */ Double angrad)
{
    return angrad * 180.0l / PI;
}

Double StrictMath::Ulp(
    /* [in] */ Double d)
{
    // special cases
    if (Math::IsInfinite(d)) {
        return Math::DOUBLE_POSITIVE_INFINITY;
    } else if (d == Math::DOUBLE_MAX_VALUE || d == -Math::DOUBLE_MAX_VALUE) {
        return Pow(2, 971);
    }
    d = Abs(d);
    return NativeNextAfter(d, Math::DOUBLE_MAX_VALUE) - d;
}

Double StrictMath::NativeNextAfter(
    /* [in] */ Double x,
    /* [in] */ Double y)
{
    return ieee_nextafter(x, y);
}

Float StrictMath::Ulp(
    /* [in] */ Float f)
{
    return Math::Ulp(f);
}

Double StrictMath::CopySign(
    /* [in] */ Double magnitude,
    /* [in] */ Double sign)
{
    // We manually inline Math::IsNaN here because the JIT can't do it yet.
    // With Math::IsNaN: 236.3ns
    // With manual inline: 141.2ns
    // With no check (i.e. Math's behavior): 110.0ns
    // (Tested on a Nexus One.)
    Int64 magnitudeBits = Math::DoubleToRawInt64Bits(magnitude);
    Int64 signBits = Math::DoubleToRawInt64Bits((sign != sign) ? 1.0 : sign);
    magnitudeBits = (magnitudeBits & ~Math::DOUBLE_SIGN_MASK) | (signBits & Math::DOUBLE_SIGN_MASK);
    return Math::Int64BitsToDouble(magnitudeBits);
}

Float StrictMath::CopySign(
    /* [in] */ Float magnitude,
    /* [in] */ Float sign)
{
    // We manually inline Math::IsNaN here because the JIT can't do it yet.
    // With Math::IsNaN: 214.7ns
    // With manual inline: 112.3ns
    // With no check (i.e. Math's behavior): 93.1ns
    // (Tested on a Nexus One.)
    Int32 magnitudeBits = Math::FloatToRawInt32Bits(magnitude);
    Int32 signBits = Math::FloatToRawInt32Bits((sign != sign) ? 1.0f : sign);
    magnitudeBits = (magnitudeBits & ~Math::FLOAT_SIGN_MASK) | (signBits & Math::FLOAT_SIGN_MASK);
    return Math::Int32BitsToFloat(magnitudeBits);
}

Int32 StrictMath::GetExponent(
    /* [in] */ Float f)
{
    return Math::GetExponent(f);
}

Int32 StrictMath::GetExponent(
    /* [in] */ Double d)
{
    return Math::GetExponent(d);
}

Double StrictMath::NextAfter(
    /* [in] */ Double start,
    /* [in] */ Double direction)
{
    if (start == 0 && direction == 0) {
        return direction;
    }
    return NativeNextAfter(start, direction);
}

Float StrictMath::NextAfter(
    /* [in] */ Float start,
    /* [in] */ Double direction)
{
    return Math::NextAfter(start, direction);
}

Double StrictMath::NextUp(
    /* [in] */ Double d)
{
    return Math::NextUp(d);
}

Float StrictMath::NextUp(
    /* [in] */ Float f)
{
    return Math::NextUp(f);
}

Double StrictMath::Scalb(
    /* [in] */ Double d,
    /* [in] */ Int32 scaleFactor)
{
    if (Math::IsNaN(d) || Math::IsInfinite(d) || d == 0) {
        return d;
    }
    // change double to Int64 for calculation
    Int64 bits = Math::DoubleToInt64Bits(d);
    // the sign of the results must be the same of given d
    Int64 sign = bits & Math::DOUBLE_SIGN_MASK;
    // calculates the factor of the result
    Int64 factor = (Int32) ((bits & Math::DOUBLE_EXPONENT_MASK) >> Math::DOUBLE_MANTISSA_BITS)
            - Math::DOUBLE_EXPONENT_BIAS + scaleFactor;

    // calculates the factor of sub-normal values
    Int32 subNormalFactor = Math::NumberOfLeadingZeros(bits & ~Math::DOUBLE_SIGN_MASK)
            - Math::DOUBLE_EXPONENT_BITS;
    if (subNormalFactor < 0) {
        // not sub-normal values
        subNormalFactor = 0;
    }
    if (Math::Abs(d) < Math::DOUBLE_MIN_NORMAL) {
        factor = factor - subNormalFactor;
    }
    if (factor > Math::DOUBLE_MAX_EXPONENT) {
        return (d > 0 ? Math::DOUBLE_POSITIVE_INFINITY : Math::DOUBLE_NEGATIVE_INFINITY);
    }

    Int64 result;
    // if result is a sub-normal
    if (factor < -Math::DOUBLE_EXPONENT_BIAS) {
        // the number of digits that shifts
        Int64 digits = factor + Math::DOUBLE_EXPONENT_BIAS + subNormalFactor;
        if (Math::Abs(d) < Math::DOUBLE_MIN_NORMAL) {
            // origin d is already sub-normal
            result = ShiftInt64Bits(bits & Math::DOUBLE_MANTISSA_MASK, digits);
        } else {
            // origin d is not sub-normal, change mantissa to sub-normal
            result = ShiftInt64Bits((bits & Math::DOUBLE_MANTISSA_MASK) | 0x0010000000000000LL, digits - 1);
        }
    } else {
        if (Math::Abs(d) >= Math::DOUBLE_MIN_NORMAL) {
            // common situation
            result = ((factor + Math::DOUBLE_EXPONENT_BIAS) << Math::DOUBLE_MANTISSA_BITS)
                    | (bits & Math::DOUBLE_MANTISSA_MASK);
        } else {
            // origin d is sub-normal, change mantissa to normal style
            result = ((factor + Math::DOUBLE_EXPONENT_BIAS) << Math::DOUBLE_MANTISSA_BITS)
                    | ((bits << (subNormalFactor + 1)) & Math::DOUBLE_MANTISSA_MASK);
        }
    }
    return Math::Int64BitsToDouble(result | sign);
}

Float StrictMath::Scalb(
    /* [in] */ Float d,
    /* [in] */ Int32 scaleFactor)
{
    if (Math::IsNaN(d) || Math::IsInfinite(d) || d == 0) {
        return d;
    }
    Int32 bits = Math::FloatToInt32Bits(d);
    Int32 sign = bits & Math::FLOAT_SIGN_MASK;
    Int32 factor = ((bits & Math::FLOAT_EXPONENT_MASK) >> Math::FLOAT_MANTISSA_BITS)
            - Math::FLOAT_EXPONENT_BIAS + scaleFactor;
    // calculates the factor of sub-normal values
    Int32 subNormalFactor = Math::NumberOfLeadingZeros(bits & ~Math::FLOAT_SIGN_MASK)
            - Math::FLOAT_EXPONENT_BITS;
    if (subNormalFactor < 0) {
        // not sub-normal values
        subNormalFactor = 0;
    }
    if (Math::Abs(d) < Math::FLOAT_MIN_NORMAL) {
        factor = factor - subNormalFactor;
    }
    if (factor > Math::FLOAT_MAX_EXPONENT) {
        return (d > 0 ? Math::FLOAT_POSITIVE_INFINITY : Math::FLOAT_NEGATIVE_INFINITY);
    }

    Int32 result;
    // if result is a sub-normal
    if (factor < -Math::FLOAT_EXPONENT_BIAS) {
        // the number of digits that shifts
        Int32 digits = factor + Math::FLOAT_EXPONENT_BIAS + subNormalFactor;
        if (Math::Abs(d) < Math::FLOAT_MIN_NORMAL) {
            // origin d is already sub-normal
            result = ShiftInt32Bits(bits & Math::FLOAT_MANTISSA_MASK, digits);
        } else {
            // origin d is not sub-normal, change mantissa to sub-normal
            result = ShiftInt32Bits((bits & Math::FLOAT_MANTISSA_MASK) | 0x00800000, digits - 1);
        }
    } else {
        if (Math::Abs(d) >= Math::FLOAT_MIN_NORMAL) {
            // common situation
            result = ((factor + Math::FLOAT_EXPONENT_BIAS) << Math::FLOAT_MANTISSA_BITS)
                    | (bits & Math::FLOAT_MANTISSA_MASK);
        } else {
            // origin d is sub-normal, change mantissa to normal style
            result = ((factor + Math::FLOAT_EXPONENT_BIAS) << Math::FLOAT_MANTISSA_BITS)
                    | ((bits << (subNormalFactor + 1)) & Math::FLOAT_MANTISSA_MASK);
        }
    }
    return Math::Int32BitsToFloat(result | sign);
}

Int32 StrictMath::ShiftInt32Bits(
    /* [in] */ Int32 bits,
    /* [in] */ Int32 digits)
{
    if (digits > 0) {
        return bits << digits;
    }
    // change it to positive
    Int32 absDigits = -digits;
    if (Math::NumberOfLeadingZeros(bits & ~Math::FLOAT_SIGN_MASK) <= (32 - absDigits)) {
        // some bits will remain after shifting, calculates its carry
        if ((((bits >> (absDigits - 1)) & 0x1) == 0)
                || Math::NumberOfTrailingZeros(bits) == (absDigits - 1)) {
            return bits >> absDigits;
        }
        return ((bits >> absDigits) + 1);
    }
    return 0;
}

Int64 StrictMath::ShiftInt64Bits(
    /* [in] */ Int64 bits,
    /* [in] */ Int64 digits)
{
    if (digits > 0) {
        return bits << digits;
    }
    // change it to positive
    Int64 absDigits = -digits;
    if (Math::NumberOfLeadingZeros(bits & ~Math::DOUBLE_SIGN_MASK) <= (64 - absDigits)) {
        // some bits will remain after shifting, calculates its carry
        if ((((bits >> (absDigits - 1)) & 0x1) == 0)
                || Math::NumberOfTrailingZeros(bits) == (absDigits - 1)) {
            return bits >> absDigits;
        }
        return ((bits >> absDigits) + 1);
    }
    return 0;
}

} // namespace Core
} // namespace Elastos