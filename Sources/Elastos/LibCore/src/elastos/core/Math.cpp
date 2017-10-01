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

#include <elastos/core/Math.h>
#include <math.h>
#include <stdlib.h>
#include "CRandom.h"

using Elastos::Utility::CRandom;

namespace Elastos {
namespace Core {

#ifdef _win32
#define isnanf __isnanf
#endif

const Int32 Math::BYTE_SIZE                 = 8;
const Int32 Math::INT16_SIZE                = 16;
const Int32 Math::INT32_SIZE                = 32;
const Int32 Math::INT64_SIZE                = 64;
const Int32 Math::FLOAT_SIZE                = 32;
const Int32 Math::DOUBLE_SIZE               = 64;

const Byte Math::BYTE_MAX_VALUE             = (Byte)0x7F;
const Byte Math::BYTE_MIN_VALUE             = (Byte)0x80;
const Int16 Math::INT16_MAX_VALUE           = (Int16)0x7FFF;
const Int16 Math::INT16_MIN_VALUE           = (Int16)0x8000;
const Int32 Math::INT32_MAX_VALUE           = 0x7FFFFFFF;
const Int32 Math::INT32_MIN_VALUE           = 0x80000000;
const Int64 Math::INT64_MAX_VALUE           = 0x7FFFFFFFFFFFFFFFLL;
const Int64 Math::INT64_MIN_VALUE           = 0x8000000000000000LL;

const Int32 Math::FLOAT_EXPONENT_BIAS       = 127;
const Int32 Math::FLOAT_EXPONENT_BITS       = 9;
const Int32 Math::FLOAT_MANTISSA_BITS       = 23;
const Int32 Math::FLOAT_NON_MANTISSA_BITS   = 9;
const Int32 Math::FLOAT_SIGN_MASK           = 0X80000000;
const Int32 Math::FLOAT_EXPONENT_MASK       = 0X7F800000;
const Int32 Math::FLOAT_MANTISSA_MASK       = 0X007FFFFF;

const Float Math::FLOAT_MAX_VALUE           = 3.40282346638528860e+38F;
const Float Math::FLOAT_MIN_VALUE           = 1.40129846432481707e-45F;
const Float Math::FLOAT_ZERO                = 0.0F;
const Float Math::FLOAT_NAN                 = 0.0F/0.0F;
const Float Math::FLOAT_POSITIVE_INFINITY   = 1.0F/0.0F;
const Float Math::FLOAT_NEGATIVE_INFINITY   = -1.0F/0.0F;
const Float Math::FLOAT_MIN_NORMAL          = 1.1754943508222875E-38F;
const Int32 Math::FLOAT_MAX_EXPONENT        = 127;
const Int32 Math::FLOAT_MIN_EXPONENT        = -126;

const Int32 Math::DOUBLE_EXPONENT_BIAS      = 1023;
const Int32 Math::DOUBLE_EXPONENT_BITS      = 12;
const Int32 Math::DOUBLE_MANTISSA_BITS      = 52;
const Int32 Math::DOUBLE_NON_MANTISSA_BITS  = 12;
const Int64 Math::DOUBLE_SIGN_MASK          = 0x8000000000000000LL;
const Int64 Math::DOUBLE_EXPONENT_MASK      = 0x7ff0000000000000LL;
const Int64 Math::DOUBLE_MANTISSA_MASK      = 0x000fffffffffffffLL;

const Double Math::DOUBLE_MAX_VALUE         = 1.79769313486231570E+308;
const Double Math::DOUBLE_MIN_VALUE         = 5E-324;
const Double Math::DOUBLE_ZERO              = 0.0;
const Double Math::DOUBLE_NAN               = 0.0/0.0;
const Double Math::DOUBLE_POSITIVE_INFINITY = 1.0/0.0;
const Double Math::DOUBLE_NEGATIVE_INFINITY = -1.0/0.0;
const Double Math::DOUBLE_MIN_NORMAL        = 2.2250738585072014E-308;
const Int32 Math::DOUBLE_MAX_EXPONENT       = 1023;
const Int32 Math::DOUBLE_MIN_EXPONENT       = -1022;

const Double Math::E                        = 2.718281828459045;
const Double Math::PI                       = 3.141592653589793;

const Int32 Math::IntegerNtzTable[] = {
    32,  0,  1, 12,  2,  6, -1, 13,   3, -1,  7, -1, -1, -1, -1, 14,
    10,  4, -1, -1,  8, -1, -1, 25,  -1, -1, -1, -1, -1, 21, 27, 15,
    31, 11,  5, -1, -1, -1, -1, -1,   9, -1, -1, 24, -1, -1, 20, 26,
    30, -1, -1, -1, -1, 23, -1, 19,  29, -1, 22, 18, 28, 17, 16, -1
};

INIT_PROI_2 const AutoPtr< ArrayOf<Int64> > Math::LONG_POWERS_OF_TEN = INIT_LONG_POWERS_OF_TEN();

AutoPtr< ArrayOf<Int64> > Math::INIT_LONG_POWERS_OF_TEN()
{
    AutoPtr< ArrayOf<Int64> > array = ArrayOf<Int64>::Alloc(19);
    (*array)[0] = 1LL;
    (*array)[1] = 10LL;
    (*array)[2] = 100LL;
    (*array)[3] = 1000LL;
    (*array)[4] = 10000LL;
    (*array)[5] = 100000LL;
    (*array)[6] = 1000000LL;
    (*array)[7] = 10000000LL;
    (*array)[8] = 100000000LL;
    (*array)[9] = 1000000000LL;
    (*array)[10] = 10000000000LL;
    (*array)[11] = 100000000000LL;
    (*array)[12] = 1000000000000LL;
    (*array)[13] = 10000000000000LL;
    (*array)[14] = 100000000000000LL;
    (*array)[15] = 1000000000000000LL;
    (*array)[16] = 10000000000000000LL;
    (*array)[17] = 100000000000000000LL;
    (*array)[18] = 1000000000000000000LL;
    return array;
}

AutoPtr<IRandom> Math::sRandom;

/*
 * These match the definitions in the VM specification.
 */
typedef uint8_t             u1;
typedef uint16_t            u2;
typedef uint32_t            u4;
typedef uint64_t            u8;
typedef int8_t              s1;
typedef int16_t             s2;
typedef int32_t             s4;
typedef int64_t             s8;
union Convert32 {
    u4 arg;
    float ff;
};

union Convert64 {
    u4 arg[2];
    s8 ll;
    double dd;
};


#define CAST_PTR(type, val) ((type *)(&(val)))
#define CAST(type, val) (*CAST_PTR(type, val))

Boolean Math::Constrain(Int32 amount, Int32 low, Int32 high)
{
    return amount < low ? low : (amount > high ? high : amount);
}

Boolean Math::Constrain(Int64 amount, Int64 low, Int64 high)
{
    return amount < low ? low : (amount > high ? high : amount);
}

Boolean Math::Constrain(Float amount, Float low, Float high)
{
    return amount < low ? low : (amount > high ? high : amount);
}

Int32 Math::Shr(
    /* [in] */ Int32 value,
    /* [in] */ Int32 bitNum)
{
    return ((UInt32)value) >> bitNum;
}

Double Math::Abs(
    /* [in] */ Double d)
{
    Int64 i = DoubleToRawInt64Bits(d);
    i &= 0X7FFFFFFFFFFFFFFFLL;
    return Int64BitsToDouble(i);
}

Float Math::Abs(
    /* [in] */ Float f)
{
    Int32 bits = FloatToRawInt32Bits(f);
    bits &= 0X7FFFFFFF;
    return Int32BitsToFloat(bits);
}

Int32 Math::Abs(
    /* [in] */ Int32 i)
{
    return (i >= 0) ? i : -i;
}

Int64 Math::Abs(
    /* [in] */Int64 i)
{
    return (i >= 0) ? i : -i;
}

Double Math::Acos(
    /* [in] */ Double d)
{
    return acos(d);
}

Double Math::Asin(
    /* [in] */ Double d)
{
    return asin(d);
}

Double Math::Atan(
    /* [in] */ Double d)
{
    return atan(d);
}

Double Math::Atan2(
    /* [in] */ Double y,
    /* [in] */ Double x)
{
    return atan2(y, x);
}

Double Math::Cbrt(
    /* [in] */ Double d)
{
    return cbrt(d);
}

Double Math::Ceil(
    /* [in] */ Double d)
{
    return ceil(d);
}

Double Math::Cos(
    /* [in] */ Double d)
{
    return cos(d);
}

Double Math::Cosh(
    /* [in] */ Double d)
{
    return cosh(d);
}

Double Math::Exp(
    /* [in] */ Double d)
{
    return exp(d);
}

Double Math::Expm1(
    /* [in] */ Double d)
{
    return expm1(d);
}

Double Math::Floor(
    /* [in] */ Double d)
{
    return floor(d);
}

Double Math::Hypot(
    /* [in] */ Double x,
    /* [in] */ Double y)
{
    return hypot(x, y);
}

Double Math::IEEEremainder(
    /* [in] */ Double x,
    /* [in] */ Double y)
{
    return remainder(x, y);
}

Double Math::Log(
    /* [in] */ Double d)
{
    return log(d);
}

Double Math::Log10(
    /* [in] */ Double d)
{
    return log10(d);
}

Double Math::Log1p(
    /* [in] */ Double d)
{
    return log1p(d);
}

Double Math::Max(
    /* [in] */ Double d1,
    /* [in] */ Double d2)
{
    if (d1 > d2) {
        return d1;
    }
    if (d1 < d2) {
        return d2;
    }
    /* if either arg is DOUBLE_NAN, return DOUBLE_NAN */
    if (d1 != d2) {
        return DOUBLE_NAN;
    }
    /* max(+0.0,-0.0) == +0.0 */
    if(DoubleToRawInt64Bits(d1) != 0) {
        return d2;
    }
    return 0.0;
}

Float Math::Max(
    /* [in] */ Float f1,
    /* [in] */ Float f2)
{
    if (f1 > f2) {
        return f1;
    }
    if (f1 < f2) {
        return f2;
    }
    /* if either arg is NAN, return NAN */
    if (f1 != f2) {
        return FLOAT_NAN;
    }
    /* Max(+0.0,-0.0) == +0.0 */
    /* FloatToRawInt32Bits(0.0F) == 0*/
    if (FloatToRawInt32Bits(f1) != 0) {
        return f2;
    }
    return 0.0F;
}

Int32 Math::Max(
    /* [in] */ Int32 i1,
    /* [in] */ Int32 i2)
{
    return (i1>i2)?i1:i2;
}

Int64 Math::Max(
    /* [in] */ Int64 l1,
    /* [in] */ Int64 l2)
{
    return l1 > l2 ? l1 : l2;
}

Double Math::Min(
    /* [in] */ Double d1,
    /* [in] */ Double d2)
{
    if (d1 > d2) {
        return d2;
    }
    if (d1 < d2) {
        return d1;
    }
    /* if either arg is DOUBLE_NAN, return DOUBLE_NAN */
    if (d1 != d2) {
        return DOUBLE_NAN;
    }
    /* min(+0.0,-0.0) == -0.0 */
    /* 0X8000000000000000L == Double.doubleToRawLongBits(-0.0d) */
    if (DoubleToRawInt64Bits(d1) == (Int64)0X8000000000000000LL) {
        return -0.0;
    }
    return d2;
}

Float Math::Min(
    /* [in] */ Float f1,
    /* [in] */ Float f2)
{
    if (f1 > f2) {
        return f2;
    }
    if (f1 < f2) {
        return f1;
    }
    /* if either arg is NAN, return NAN */
    if (f1 != f2) {
        return FLOAT_NAN;
    }
    /* Min(+0.0,-0.0) == -0.0 */
    /* 0X80000000 == FloatToRawInt32Bits(-0.0F) */
    if (FloatToRawInt32Bits(f1) == (Int32)0X80000000) {
        return -0.0F;
    }
    return f2;
}

Int32 Math::Min(
    /* [in] */ Int32 i1,
    /* [in] */ Int32 i2)
{
    return (i1<i2)?i1:i2;
}

Int64 Math::Min(
    /* [in] */ Int64 l1,
    /* [in] */ Int64 l2)
{
    return l1 < l2 ? l1 : l2;
}

Double Math::Pow(
    /* [in] */ Double x,
    /* [in] */ Double y)
{
    return pow(x, y);
}

Double Math::Rint(
    /* [in] */ Double d)
{
    return rint(d);
}

Int64 Math::Round(
    /* [in] */ Double d)
{
    // check for NAN
    if (d != d) {
        return 0;
    }
    return (Int64)Floor(d + 0.5);
}

Int32 Math::Round(
    /* [in] */ Float f)
{
    // check for NAN
    if (f != f) {
        return 0;
    }
    return (Int32)Floor(f + 0.5F);
}

Double Math::Signum(
    /* [in] */ Double d)
{
    if (IsNaN(d)) {
        return DOUBLE_NAN;
    }
    Double sig = d;
    if (d > 0) {
        sig = 1.0;
    } else if (d < 0) {
        sig = -1.0;
    }
    return sig;
}

Float Math::Signum(
    /* [in] */ Float f)
{
    //check for NAN
    if(IsNaN(f)){
        return FLOAT_NAN;
    }
    Float sig = f;
    if(f > 0){
        sig = 1.0F;
    }else if(f < 0){
        sig = -1.0F;
    }
    return sig;
}

Int32 Math::Signum(
    /* [in] */ Int64 v)
{
    return v < 0 ? -1 : (v == 0 ? 0 : 1);
}

Int32 Math::Signum(
    /* [in] */ Int32 v)
{
    return v < 0 ? -1 : (v == 0 ? 0 : 1);
}

Double Math::Sin(
    /* [in] */ Double d)
{
    return sin(d);
}

Double Math::Sinh(
    /* [in] */ Double d)
{
    return sinh(d);
}

Double Math::Sqrt(
    /* [in] */ Double d)
{
    return sqrt(d);
}

Double Math::Tan(
    /* [in] */ Double d)
{
    return tan(d);
}

Double Math::Tanh(
    /* [in] */ Double d)
{
    return tanh(d);
}

Double Math::Random()
{
    if (NULL == sRandom) {
        CRandom::New((IRandom**)&sRandom);
    }

    Double d;
    sRandom->NextDouble(&d);
    return d;
}

Double Math::ToRadians(
    /* [in] */ Double angdeg)
{
    return angdeg / 180 * Math::PI;
}

Double Math::ToDegrees(
    /* [in] */ Double angrad)
{
    return angrad * 180 / Math::PI;
}

Double Math::Ulp(
    /* [in] */ Double d)
{
    if (IsInfinite(d)) {
        return DOUBLE_POSITIVE_INFINITY;
    }
    else if (d == DOUBLE_MAX_VALUE || d == -DOUBLE_MAX_VALUE) {
        return pow(2.0, 971);
    }
    d = Abs(d);
    return NativeNextAfter(d, DOUBLE_MAX_VALUE) - d;
}

Double Math::NativeNextAfter(
    /* [in] */ Double x,
    /* [in] */ Double y)
{
    return nextafter(x, y);
}

Float Math::Ulp(
   /* [in] */ Float f)
{
    //special cases
    if(IsNaN(f)){
        return FLOAT_NAN;
    }
    else if(IsInfinite(f)){
        return FLOAT_POSITIVE_INFINITY;
    }
    else if (f == FLOAT_MAX_VALUE || f == -FLOAT_MAX_VALUE) {
        return (Float) pow(2, 104);
    }

    f = Abs(f);
    Int32 hx = FloatToRawInt32Bits(f);
    Int32 hy = FloatToRawInt32Bits(FLOAT_MAX_VALUE);
    if ((hx & 0x7fffffff) == 0) { /* f == 0 */
        return Int32BitsToFloat((hy & 0x80000000) | 0x1);
    }
    if ((hx > 0) ^ (hx > hy)) { /* |f| < |FLOAT_MAX_VALUE| */
        hx += 1;
    } else {
        hx -= 1;
    }
    return Int32BitsToFloat(hx) - f;
}

Float Math::CopySign(
    /* [in] */ Float magnitude,
    /* [in] */ Float sign)
{
    Int32 magnitudeBits = FloatToRawInt32Bits(magnitude);
    Int32 signBits = FloatToRawInt32Bits(sign);
    magnitudeBits = (magnitudeBits & ~FLOAT_SIGN_MASK) | (signBits & FLOAT_SIGN_MASK);
    return Int32BitsToFloat(magnitudeBits);
}

Double Math::CopySign(
    /* [in] */ Double magnitude,
    /* [in] */ Double sign)
{
    Int64 magnitudeBits = DoubleToRawInt64Bits(magnitude);
    Int64 signBits = DoubleToRawInt64Bits(sign);
    magnitudeBits = (magnitudeBits & ~DOUBLE_SIGN_MASK) | (signBits & DOUBLE_SIGN_MASK);
    return Int64BitsToDouble(magnitudeBits);
}

Int32 Math::GetExponent(
    /* [in] */ Double d)
{
    Int64 bits = DoubleToRawInt64Bits(d);
    bits = (bits & DOUBLE_EXPONENT_MASK) >> DOUBLE_MANTISSA_BITS;
    return (Int32) bits - DOUBLE_EXPONENT_BIAS;
}

Int32 Math::GetExponent(
    /* [in] */ Float d)
{
    Int32 bits = FloatToRawInt32Bits(d);
    bits = (bits & FLOAT_EXPONENT_MASK) >> FLOAT_MANTISSA_BITS;
    return (Int32) bits - FLOAT_EXPONENT_BIAS;
}

Double Math::NextAfter(
    /* [in] */ Double start,
    /* [in] */ Double direction)
{
    if (start == 0 && direction == 0) {
        return direction;
    }
    return NativeNextAfter(start, direction);
}

Float Math::NextAfter(
    /* [in] */ Float start,
    /* [in] */ Double direction)
{
    if(IsNaN(start) || IsNaN(direction)){
        return FLOAT_NAN;
    }
    if(start == 0 && direction == 0){
        return (Float)direction;
    }
    if ((start == FLOAT_MIN_VALUE && direction < start)
             || (start == -FLOAT_MIN_VALUE && direction > start)) {
        return ((start > 0) ? 0.0F : -0.0F);
    }
    if (IsInfinite(start) && (direction != start)) {
          return (start > 0 ? FLOAT_MAX_VALUE : -FLOAT_MAX_VALUE);
    }
    if ((start == FLOAT_MAX_VALUE && direction > start)
             || (start == -FLOAT_MAX_VALUE && direction < start)) {
        return (start > 0 ? FLOAT_POSITIVE_INFINITY : FLOAT_NEGATIVE_INFINITY);
    }
    if (direction > start) {
        if (start > 0) {
            return Int32BitsToFloat(FloatToInt32Bits(start) + 1);
        }
        if (start < 0) {
            return Int32BitsToFloat(FloatToInt32Bits(start) - 1);
        }
        return +FLOAT_MIN_VALUE;
    }
    if (direction < start) {
        if (start > 0) {
            return Int32BitsToFloat(FloatToInt32Bits(start) - 1);
        }
        if (start < 0) {
            return Int32BitsToFloat(FloatToInt32Bits(start) + 1);
        }
        return -FLOAT_MIN_VALUE;
    }
    return (Float)direction;
}

Float Math::NextUp(
    /* [in] */ Float f)
{
    if(IsNaN(f)){
        return FLOAT_NAN;
    }
    if(f == FLOAT_POSITIVE_INFINITY){
        return FLOAT_POSITIVE_INFINITY;
    }
    if(f == 0){
        return FLOAT_MIN_VALUE;
    }else if(f > 0){
        return Int32BitsToFloat(FloatToInt32Bits(f) + 1);
    }else{
        return Int32BitsToFloat(FloatToInt32Bits(f) - 1);
    }
}

Double Math::NextUp(
    /* [in] */ Double d)
{
    if (IsNaN(d)) {
        return DOUBLE_NAN;
    }
    if (d == DOUBLE_POSITIVE_INFINITY) {
        return d;
    }
    if (d == 0) {
        return DOUBLE_MIN_VALUE;
    } else if (d > 0) {
        return Int64BitsToDouble(DoubleToInt64Bits(d) + 1);
    } else {
        return Int64BitsToDouble(DoubleToInt64Bits(d) - 1);
    }
}

Double Math::Scalb(
    /* [in] */ Double d,
    /* [in] */ Int32 scaleFactor)
{
    if (IsNaN(d) || IsInfinite(d) || d == 0) {
        return d;
    }
    // change double to long for calculation
    Int64 bits = DoubleToInt64Bits(d);
    // the sign of the results must be the same of given d
    Int64 sign = bits & DOUBLE_SIGN_MASK;
    // calculates the factor of the result
    Int64 factor = ((bits & DOUBLE_EXPONENT_MASK) >> DOUBLE_MANTISSA_BITS)
        - DOUBLE_EXPONENT_BIAS + scaleFactor;

    // calculates the factor of sub-normal values
    Int64 subNormalFactor = NumberOfLeadingZeros(bits & ~DOUBLE_SIGN_MASK)
        - DOUBLE_NON_MANTISSA_BITS;
    if (subNormalFactor < 0) {
        // not sub-normal values
        subNormalFactor = 0;
    } else {
        factor = factor - subNormalFactor;
    }
    if (factor > DOUBLE_MAX_EXPONENT) {
        return (d > 0 ? DOUBLE_POSITIVE_INFINITY : DOUBLE_NEGATIVE_INFINITY);
    }

    Int64 result;
    // if result is a sub-normal
    if (factor <= -DOUBLE_EXPONENT_BIAS) {
        // the number of digits that shifts
        Int64 digits = factor + DOUBLE_EXPONENT_BIAS + subNormalFactor;
        if (Abs(d) < DOUBLE_MIN_NORMAL) {
            // origin d is already sub-normal
            result = ShiftInt64Bits(bits & DOUBLE_MANTISSA_MASK, digits);
        } else {
            // origin d is not sub-normal, change mantissa to sub-normal
            result = ShiftInt64Bits((bits & DOUBLE_MANTISSA_MASK) | 0X0010000000000000LL, digits - 1);
        }
    } else {
        if (Abs(d) >= DOUBLE_MIN_NORMAL) {
            // common situation
            result = ((factor + DOUBLE_EXPONENT_BIAS) << DOUBLE_MANTISSA_BITS)
                | (bits & DOUBLE_MANTISSA_MASK);
        } else {
            // origin d is sub-normal, change mantissa to normal style
            result = ((factor + DOUBLE_EXPONENT_BIAS) << DOUBLE_MANTISSA_BITS)
                | ((bits << (subNormalFactor + 1)) & DOUBLE_MANTISSA_MASK);
        }
    }
    return Int64BitsToDouble(result | sign);
}

Float Math::Scalb(
    /* [in] */ Float d,
    /* [in] */ Int32 scaleFactor)
{
    if ((IsNaN(d)) || IsInfinite(d) || d == 0) {
        return d;
    }
    Int32 bits = FloatToInt32Bits(d);
    Int32 sign = bits & FLOAT_SIGN_MASK;
    Int32 factor = ((bits & FLOAT_EXPONENT_MASK) >> FLOAT_MANTISSA_BITS)
                - FLOAT_EXPONENT_BIAS + scaleFactor;
    // calcutes the factor of sub-normal values
    Int32 subNormalFactor = NumberOfLeadingZeros((Int32)(bits & ~FLOAT_SIGN_MASK))
                - FLOAT_NON_MANTISSA_BITS;
    if (subNormalFactor < 0) {
    // not sub-normal values
        subNormalFactor = 0;
    } else {
           factor = factor - subNormalFactor;
    }
    if (factor > FLOAT_MAX_EXPONENT) {
           return (d > 0 ? FLOAT_POSITIVE_INFINITY : FLOAT_NEGATIVE_INFINITY);
    }

    Int32 result;
    // if result is a sub-normal
    if (factor <= -FLOAT_EXPONENT_BIAS) {
        // the number of digits that shifts
        Int32 digits = factor + FLOAT_EXPONENT_BIAS + subNormalFactor;
        if (Abs(d) < FLOAT_MIN_NORMAL) {
            // origin d is already sub-normal
            result = ShiftInt32Bits(bits & FLOAT_MANTISSA_MASK, digits);
        } else {
            // origin d is not sub-normal, change mantissa to sub-normal
            result = ShiftInt32Bits((bits & FLOAT_MANTISSA_MASK) | 0X00800000, digits - 1);
        }
    } else {
        if (Abs(d) >= FLOAT_MIN_NORMAL) {
            // common situation
            result = ((factor + FLOAT_EXPONENT_BIAS) << FLOAT_MANTISSA_BITS)
                        | (bits & FLOAT_MANTISSA_MASK);
        } else {
            // origin d is sub-normal, change mantissa to normal style
            result = ((factor + FLOAT_EXPONENT_BIAS) << FLOAT_MANTISSA_BITS)
                        | ((bits << (subNormalFactor + 1)) & FLOAT_MANTISSA_MASK);
        }
    }
    return Int32BitsToFloat(result | sign);
}

Int32 Math::ShiftInt32Bits(
    /* [in] */ Int32 bits,
    /* [in] */ Int32 digits)
{
    if (digits > 0) {
        return bits << digits;
    }
    // change it to positive
    Int32 absdigits = -digits;
    if (!(NumberOfLeadingZeros(bits & ~DOUBLE_SIGN_MASK) <= (32 - absdigits))) {
        return 0;
    }
    Int32 ret = bits >> absdigits;
    Boolean halfbit = ((bits >> (absdigits - 1)) & 0X1) == 1;
    if (halfbit) {
        if (NumberOfTrailingZeros(bits) < (absdigits - 1)) {
            ret = ret + 1;
        }
        if (NumberOfTrailingZeros(bits) == (absdigits - 1)) {
            if ((ret & 0X1) == 1) {
                ret = ret + 1;
            }
        }
    }
    return ret;
}

Int64 Math::ShiftInt64Bits(
    /* [in] */ Int64 bits,
    /* [in] */ Int64 digits)
{
    if (digits > 0) {
        return bits << digits;
    }
    // change it to positive
    Int64 absdigits = -digits;
    if (!(NumberOfLeadingZeros((Int64)(bits & ~DOUBLE_SIGN_MASK)) <= (64 - absdigits))) {
        return 0;
    }
    Int64 ret = bits >> absdigits;
    Boolean halfbit = ((bits >> (absdigits - 1)) & 0X1) == 1;
    if (halfbit) {
        // some bits will remain after shifting, calculates its carry
        // subnormal
        if (NumberOfTrailingZeros((Int64)bits) < (absdigits - 1)) {
             ret = ret + 1;
        }
        if (NumberOfTrailingZeros((Int64)bits) == (absdigits - 1)) {
             if ((ret & 0X1) == 1) {
                 ret = ret + 1;
             }
        }
    }
    return ret;
}

Int64 Math::DoubleToRawInt64Bits(
    /* [in] */ Double d)
{
    Convert64 convert;
    convert.arg[0] = ((UInt32*)&d)[0];
    convert.arg[1] = ((UInt32*)&d)[1];
    return convert.ll;
}

Int64 Math::DoubleToInt64Bits(
    /* [in] */ Double d)
{
    Convert64 convert;
    convert.arg[0] = ((UInt32*)&d)[0];
    convert.arg[1] = ((UInt32*)&d)[1];
    return IsNaN(convert.dd) ? 0x7ff8000000000000LL : convert.ll;
}

Double Math::Int64BitsToDouble(
    /* [in] */ Int64 i)
{
    Convert64 convert;
    convert.arg[0] = ((UInt32*)&i)[0];
    convert.arg[1] = ((UInt32*)&i)[1];
    return convert.dd;
}

Int32 Math::HighestOneBit(
    /* [in] */ Int32 i)
{
    // Hacker's Delight, Figure 3-1
    i |= (i >> 1);
    i |= (i >> 2);
    i |= (i >> 4);
    i |= (i >> 8);
    i |= (i >> 16);
    return i - (((UInt32)i) >> 1);
}

Int32 Math::NumberOfTrailingZeros(
    /* [in] */ Int32 i)
{
    i = ((i & -i) * 0x0450FBAF);
    return IntegerNtzTable[(UInt32)i >> 26];
}

Int32 Math::NumberOfTrailingZeros(
    /* [in] */ Int64 v)
{
    Int32 low = (Int32)v;
    return low !=0 ? NumberOfTrailingZeros(low)
        : 32 + NumberOfTrailingZeros((Int32)((UInt64)v >> 32));
}

Int32 Math::NumberOfLeadingZeros(
    /* [in] */ Int64 v)
{
    // After Hacker's Delight, Figure 5-6
    if (v < 0) {
        return 0;
    }
    if (v == 0) {
        return 64;
    }
    // On a 64-bit VM, the two previous tests should probably be replaced by
    // if (v <= 0) return ((Int32) (~v >> 57)) & 64;

    Int32 n = 1;
    Int32 i = (Int32)((UInt64)v >> 32);
    if (i == 0) {
        n +=  32;
        i = (Int32) v;
    }
    if ((UInt32)i >> 16 == 0) {
        n +=  16;
        i <<= 16;
    }
    if ((UInt32)i >> 24 == 0) {
        n +=  8;
        i <<= 8;
    }
    if ((UInt32)i >> 28 == 0) {
        n +=  4;
        i <<= 4;
    }
    if ((UInt32)i >> 30 == 0) {
        n +=  2;
        i <<= 2;
    }
    return n - ((UInt32)i >> 31);
}

Int32 Math::NumberOfLeadingZeros(
    /* [in] */ Int32 i)
{
    // Hacker's Delight, Figure 5-6
    if (i <= 0) {
        return (~i >> 26) & 32;
    }
    Int32 n = 1;
    if (i >> 16 == 0) {
        n +=  16;
        i <<= 16;
    }
    if (i >> 24 == 0) {
        n +=  8;
        i <<= 8;
    }
    if (i >> 28 == 0) {
        n +=  4;
        i <<= 4;
    }
    if (i >> 30 == 0) {
        n +=  2;
        i <<= 2;
    }
    //return n - (i >>> 31);
    return n - ((UInt32)i >> 31);
}

Boolean Math::IsNaN(
    /* [in] */ Float f)
{
    return f != f;
}

Boolean Math::IsNaN(
    /* [in] */ Double d)
{
    return d != d;
}

Boolean Math::IsInfinite(
    /* [in] */ Float f)
{
    return (f == FLOAT_POSITIVE_INFINITY) || (f == FLOAT_NEGATIVE_INFINITY);
}

Boolean Math::IsInfinite(
    /* [in] */ Double d)
{
    return (d == DOUBLE_POSITIVE_INFINITY) || (d == DOUBLE_NEGATIVE_INFINITY);
}

Int32 Math::FloatToRawInt32Bits(
    /* [in] */ Float value)
{
    return (Int32)((UInt32*)&value)[0];
}

Int32 Math::FloatToInt32Bits(
    /* [in] */ Float value)
{
    Convert32 convert;
    convert.arg = ((UInt32*)&value)[0];
    return IsNaN(convert.ff) ? 0x7fc00000 : (Int32)((UInt32*)&value)[0];
}

Float Math::Int32BitsToFloat(
    /* [in] */ Int32 value)
{
    Convert32 convert;
    convert.arg = (UInt32)value;
    return convert.ff;
}

Float Math::NativeNextAfter(
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    return nextafterf(x,y);
}

Int32 Math::BitCount(
    /* [in] */ Int32 i)
{
    // Hacker's Delight, Figure 5-2
    i -=  (i >> 1) & 0x55555555;
    i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
    i = (((i >> 4) + i) & 0x0F0F0F0F);
    i += i >> 8;
    i += i >> 16;
    return i  & 0x0000003F;
}

Int32 Math::BitCount(
    /* [in] */ Int64 v)
{
    // Combines techniques from several sources
    v -=  ((UInt64)v >> 1) & 0x5555555555555555ll;
    v = (v & 0x3333333333333333ll) + (((UInt64)v >> 2) & 0x3333333333333333ll);
    Int32 i =  ((Int32)((UInt64)v >> 32)) + (Int32) v;
    i = (i & 0x0F0F0F0F) + (((UInt32)i >> 4) & 0x0F0F0F0F);
    i += (UInt32)i >> 8;
    i += (UInt32)i >> 16;
    return i  & 0x0000007F;
}

Int32 Math::ReverseBytes(
    /* [in] */ Int32 i)
{
    // Hacker's Delight 7-1, with minor tweak from Veldmeijer
    // http://graphics.stanford.edu/~seander/bithacks.html
    i =    ((((UInt32)i) >>  8) & 0x00FF00FF) | ((i & 0x00FF00FF) <<  8);
    return ( ((UInt32)i) >> 16             ) | ( i               << 16);
}

Int32 Math::Compare(
    /* [in] */ Int32 lhs,
    /* [in] */ Int32 rhs)
{
    return lhs < rhs ? -1 : (lhs == rhs ? 0 : 1);
}

Int32 Math::Compare(
    /* [in] */ Int64 lhs,
    /* [in] */ Int64 rhs)
{
    return lhs < rhs ? -1 : (lhs == rhs ? 0 : 1);
}

Int32 Math::Compare(
    /* [in] */ Float float1,
    /* [in] */ Float float2)
{
    // Non-zero, non-NaN checking.
    if (float1 > float2) {
        return 1;
    }
    if (float2 > float1) {
        return -1;
    }
    if (float1 == float2 && 0.0f != float1) {
        return 0;
    }

    // NaNs are equal to other NaNs and larger than any other float
    if (IsNaN(float1)) {
        if (IsNaN(float2)) {
            return 0;
        }
        return 1;
    } else if (IsNaN(float2)) {
        return -1;
    }

    // Deal with +0.0 and -0.0
    Int32 f1 = FloatToRawInt32Bits(float1);
    Int32 f2 = FloatToRawInt32Bits(float2);
    // The below expression is equivalent to:
    // (f1 == f2) ? 0 : (f1 < f2) ? -1 : 1
    // because f1 and f2 are either 0 or Integer.MIN_VALUE
    return (f1 >> 31) - (f2 >> 31);
}

Int32 Math::Compare(
    /* [in] */ Double double1,
    /* [in] */ Double double2)
{
    // Non-zero, non-NaN checking.
    if (double1 > double2) {
        return 1;
    }
    if (double2 > double1) {
        return -1;
    }
    if (double1 == double2 && 0.0l != double1) {
        return 0;
    }

    // NaNs are equal to other NaNs and larger than any other double
    if (IsNaN(double1)) {
        if (IsNaN(double2)) {
            return 0;
        }
        return 1;
    } else if (IsNaN(double2)) {
        return -1;
    }

    // Deal with +0.0 and -0.0
    Int64 d1 = DoubleToRawInt64Bits(double1);
    Int64 d2 = DoubleToRawInt64Bits(double2);
    // The below expression is equivalent to:
    // (d1 == d2) ? 0 : (d1 < d2) ? -1 : 1
    return (Int32) ((d1 >> 63) - (d2 >> 63));
}

Boolean Math::Equals(
    /* [in] */ Float float1,
    /* [in] */ Float float2)
{
    return (FloatToRawInt32Bits(float1) == FloatToRawInt32Bits(float2));
}

Boolean Math::Equals(
    /* [in] */ Double double1,
    /* [in] */ Double double2)
{
    return (DoubleToRawInt64Bits(double1) == DoubleToRawInt64Bits(double2));
}

} // namespace Core
} // namespace Elastos
