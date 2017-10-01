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

/**
 * reference program:
 * ~/libcore/luni/src/main/native/java_lang_RealToString.cpp
 * ~/libcore/luni/src/main/java/java/lang/RealToString.java
 */

#include <RealToString.h>
#include <IntegralToString.h>
#include <StringBuilder.h>
#include <Math.h>
#include <cbigint.h>
#include <stdio.h>

#define INV_LOG_OF_TEN_BASE_2 (0.30102999566398114) /* Local */
//#define INV_LOG_OF_TEN_BASE_2 (Math::Log(2.0) / Math::Log(10.0))

extern pthread_once_t once;
extern pthread_key_t sKeyInstance;

namespace Elastos {
namespace Core {

static void RealToStringDestructor(void* st)
{
    RealToString* obj = static_cast<RealToString*>(st);
    if (obj) {
        delete obj;
    }
}

static void InitsKeyInstance()
{
    ASSERT_TRUE(pthread_key_create(&sKeyInstance, RealToStringDestructor) == 0);
}

RealToString* RealToString::GetInstance()
{
    pthread_once(&once, InitsKeyInstance);
    RealToString* rts = (RealToString*)pthread_getspecific(sKeyInstance);
    if (rts == NULL) {
        rts = new RealToString();
        ASSERT_TRUE(pthread_setspecific(sKeyInstance, rts) == 0);
    }
    assert(rts != NULL);
    return rts;
}

RealToString::RealToString()
    : mFirstK(0)
    , mDigitCount(0)
{
    memset(mDigits, 0, sizeof(Int32) * DIGITS_LENGTH);
}

String RealToString::ToString(Double d)
{
    return Convert(d);
}

String RealToString::Convert(Double inputNumber)
{
    Int64 inputNumberBits = Math::DoubleToRawInt64Bits(inputNumber);
    Boolean positive = (inputNumberBits & Math::DOUBLE_SIGN_MASK) == 0;
    Int64 ee = ((inputNumberBits & Math::DOUBLE_EXPONENT_MASK) >> Math::DOUBLE_MANTISSA_BITS);
    Int32 e=((Int32*)&ee)[0];

    Int64 f = inputNumberBits & Math::DOUBLE_MANTISSA_MASK;
    Boolean mantissaIsZero = f == 0;

    String quickResult;
    if (e == 2047) {
        if (mantissaIsZero) {
            quickResult = String(positive ? "Infinity" : "-Infinity");
        } else {
            quickResult = String("NaN");
        }
    } else if (e == 0) {
        if (mantissaIsZero) {
            quickResult = String(positive ? "0.0" : "-0.0");
        } else if (f == 1) {
            // special case to increase precision even though 2 * Math::DOUBLE_MIN_VALUE is 1.0e-323
            quickResult = String(positive ? "4.9E-324" : "-4.9E-324");
        }
    }

    if (!quickResult.IsNull()) {
        return quickResult;
    }

    Int32 p = Math::DOUBLE_EXPONENT_BIAS + Math::DOUBLE_MANTISSA_BITS; // the power offset (precision)
    Int32 pow;
    Int32 numBits = Math::DOUBLE_MANTISSA_BITS;
    if (e == 0) {
        pow = 1 - p; // a denormalized number
        Int64 ff = f;
        while ((ff & 0x0010000000000000L) == 0) {
            ff = ff << 1;
            numBits--;
        }
    } else {
        // 0 < e < 2047
        // a "normalized" number
        f = f | 0x0010000000000000L;
        pow = e - p;
    }

   // printf(" > pow %d\n", pow);

    mFirstK = mDigitCount = 0;
    if (((-59 < pow) && (pow < 6)) || ((pow == -59) && (!mantissaIsZero))) {
        LongDigitGenerator(f, pow, e == 0, mantissaIsZero, numBits);
    } else {
        BigIntDigitGenerator(f, pow, e == 0, numBits);
    }

    StringBuilder dst(26);
    if ((inputNumber >= 1e7l) || (inputNumber <= -1e7l)
            || ((inputNumber > -1e-3l) && (inputNumber < 1e-3l))) {
        FreeFormatExponential(dst, positive);
    } else {
        FreeFormat(dst, positive);
    }

    return dst.ToString();
}

String RealToString::ToString(Float f)
{
    return Convert(f);
}

String RealToString::Convert(Float inputNumber)
{
    Int32 inputNumberBits = Math::FloatToRawInt32Bits(inputNumber);
    Boolean positive = (inputNumberBits & Math::FLOAT_SIGN_MASK) == 0;
    Int32 e = (inputNumberBits & Math::FLOAT_EXPONENT_MASK) >> Math::FLOAT_MANTISSA_BITS;
    Int32 f = inputNumberBits & Math::FLOAT_MANTISSA_MASK;
    Boolean mantissaIsZero = f == 0;

    String quickResult;
    if (e == 255) {
        if (mantissaIsZero) {
            quickResult = String(positive ? "Infinity" : "-Infinity");
        } else {
            quickResult = String("NaN");
        }
    } else if (e == 0 && mantissaIsZero) {
        quickResult = String(positive ? "0.0" : "-0.0");
    }

    if (!quickResult.IsNull()) {
        return quickResult;
    }

    Int32 p = Math::FLOAT_EXPONENT_BIAS + Math::FLOAT_MANTISSA_BITS; // the power offset (precision)
    Int32 pow;
    Int32 numBits = Math::FLOAT_MANTISSA_BITS;
    if (e == 0) {
        pow = 1 - p; // a denormalized number
        if (f < 8) { // want more precision with smallest values
            f = f << 2;
            pow -= 2;
        }
        Int32 ff = f;
        while ((ff & 0x00800000) == 0) {
            ff = ff << 1;
            numBits--;
        }
    } else {
        // 0 < e < 255
        // a "normalized" number
        f = f | 0x00800000;
        pow = e - p;
    }

    mFirstK = mDigitCount = 0;
    if ((-59 < pow && pow < 35) || ((pow == -59) && (!mantissaIsZero))) {
        LongDigitGenerator(f, pow, e == 0, mantissaIsZero, numBits);
    } else {
        BigIntDigitGenerator(f, pow, e == 0, numBits);
    }
    StringBuilder dst(26);
    if ((inputNumber >= 1e7f) || (inputNumber <= -1e7f)
            || ((inputNumber > -1e-3f) && (inputNumber < 1e-3f))) {
        FreeFormatExponential(dst, positive);
    } else {
        FreeFormat(dst, positive);
    }
    return dst.ToString();
}

void RealToString::FreeFormatExponential(StringBuilder& sb, Boolean positive)
{
    Int32 digitIndex = 0;
    if (!positive) {
        sb.AppendChar('-');
    }
    sb.AppendChar((Char32) ('0' + mDigits[digitIndex++]));
    sb.AppendChar('.');

    Int32 k = mFirstK;
    Int32 exponent = k;
    while (true) {
        k--;
        if (digitIndex >= mDigitCount) {
            break;
        }
        sb.AppendChar('0' + mDigits[digitIndex++]);
    }
    if (k == exponent - 1) {
        sb.AppendChar('0');
    }
    sb.AppendChar('E');
    String expStr = IntegralToString::ToString(exponent,10);
    sb.Append(expStr);
}

void RealToString::FreeFormat(StringBuilder& sb, Boolean positive)
{
    Int32 digitIndex = 0;
    if (!positive) {
        sb.AppendChar('-');
    }
    Int32 k = mFirstK;
    if (k < 0) {
        sb.AppendChar('0');
        sb.AppendChar('.');
        for (Int32 i = k + 1; i < 0; ++i) {
            sb.AppendChar('0');
        }
    }
    Int32 U = mDigits[digitIndex++];
    do {
        if (U != -1) {
            sb.AppendChar('0' + U);
        } else if (k >= -1) {
            sb.AppendChar('0');
        }
        if (k == 0) {
            sb.AppendChar('.');
        }
        k--;
        U = digitIndex < mDigitCount ? mDigits[digitIndex++] : -1;
    } while (U != -1 || k >= -1);
}

void RealToString::LongDigitGenerator(Int64 f, Int32 e,
        Boolean isDenormalized, Boolean mantissaIsZero, Int32 p)
{
    Int64 R, S, M;
    if (e >= 0) {
        M = ((Int64)1l) << e;
        if (!mantissaIsZero) {
            R = f << (e + 1);
            S = 2;
        } else {
            R = f << (e + 2);
            S = 4;
        }
    } else {
        M = 1;
        if (isDenormalized || !mantissaIsZero) {
            R = f << 1;
            S = ((Int64)1l) << (1 - e);
        } else {
            R = f << 2;
            S = ((Int64)1l) << (2 - e);
        }
    }
    Int32 k = (Int32) Math::Ceil((e + p - 1) * INV_LOG_OF_TEN_BASE_2 - 1e-10);
    if (k > 0) {
        S = S * (*Math::LONG_POWERS_OF_TEN)[k];
    } else if (k < 0) {
        Int64 scale = (*Math::LONG_POWERS_OF_TEN)[-k];
        R = R * scale;
        M = M == 1 ? scale : M * scale;
    }
    if (R + M > S) { // was M_plus
        mFirstK = k;
    } else {
        mFirstK = k - 1;
        R = R * 10;
        M = M * 10;
    }
    Boolean low, high;
    Int32 U;
    while (true) {
        // Set U to floor(R/S) and R to the remainder, using *unsigned* 64-bit division
        U = 0;
        for (Int32 i = 3; i >= 0; i--) {
            Int64 remainder = R - (S << i);
            if (remainder >= 0) {
                R = remainder;
                U += 1 << i;
            }
        }
        low = R < M; // was M_minus
        high = R + M > S; // was M_plus
        if (low || high) {
            break;
        }
        R = R * 10;
        M = M * 10;
        mDigits[mDigitCount++] = U;
    }
    if (low && !high) {
        mDigits[mDigitCount++] = U;
    } else if (high && !low) {
        mDigits[mDigitCount++] = U + 1;
    } else if ((R << 1) < S) {
        mDigits[mDigitCount++] = U;
    } else {
        mDigits[mDigitCount++] = U + 1;
    }
}


/*NB the Number converter methods are synchronized so it is possible to
 *have global data for use by bigIntDigitGenerator */
#define RM_SIZE 21     /* Local. */
#define STemp_SIZE 22  /* Local. */

/* The algorithm for this particular function can be found in:
 *
 *      Printing Floating-Point Numbers Quickly and Accurately, Robert
 *      G. Burger, and R. Kent Dybvig, Programming Language Design and
 *      Implementation (PLDI) 1996, pp.108-116.
 *
 * The previous implementation of this function combined m+ and m- into
 * one single M which caused some inaccuracy of the last digit. The
 * particular case below shows this inaccuracy:
 *
 *       System.out.println(new Double((1.234123412431233E107)).toString());
 *       System.out.println(new Double((1.2341234124312331E107)).toString());
 *       System.out.println(new Double((1.2341234124312332E107)).toString());
 *
 *       outputs the following:
 *
 *           1.234123412431233E107
 *           1.234123412431233E107
 *           1.234123412431233E107
 *
 *       instead of:
 *
 *           1.234123412431233E107
 *           1.2341234124312331E107
 *           1.2341234124312331E107
 *
 */
void RealToString::BigIntDigitGenerator(Int64 f, Int32 e,
        Boolean isDenormalized, Int32 p)
{
    int RLength, SLength, TempLength, mplus_Length, mminus_Length;
    int high, low, i;
    Int32 k, firstK, U;

    UInt64 R[RM_SIZE], S[STemp_SIZE], mplus[RM_SIZE], mminus[RM_SIZE], Temp[STemp_SIZE];
    memset(R     , 0, RM_SIZE    * sizeof(UInt64));
    memset(S     , 0, STemp_SIZE * sizeof(UInt64));
    memset(mplus , 0, RM_SIZE    * sizeof(UInt64));
    memset(mminus, 0, RM_SIZE    * sizeof(UInt64));
    memset(Temp  , 0, STemp_SIZE * sizeof(UInt64));

    if (e >= 0) {
        *R = f;
        *mplus = *mminus = 1;
        simpleShiftLeftHighPrecision(mminus, RM_SIZE, e);
        if (f != (2 << (p - 1))) {
            simpleShiftLeftHighPrecision(R, RM_SIZE, e + 1);
            *S = 2;
            /*
             * m+ = m+ << e results in 1.0e23 to be printed as
             * 0.9999999999999999E23
             * m+ = m+ << e+1 results in 1.0e23 to be printed as
             * 1.0e23 (caused too much rounding)
             *      470fffffffffffff = 2.0769187434139308E34
             *      4710000000000000 = 2.076918743413931E34
             */
            simpleShiftLeftHighPrecision(mplus, RM_SIZE, e);
        } else {
            simpleShiftLeftHighPrecision(R, RM_SIZE, e + 2);
            *S = 4;
            simpleShiftLeftHighPrecision(mplus, RM_SIZE, e + 1);
        }
    } else {
        if (isDenormalized || (f != (2 << (p - 1)))) {
            *R = f << 1;
            *S = 1;
            simpleShiftLeftHighPrecision(S, STemp_SIZE, 1 - e);
            *mplus = *mminus = 1;
        } else {
            *R = f << 2;
            *S = 1;
            simpleShiftLeftHighPrecision(S, STemp_SIZE, 2 - e);
            *mplus = 2;
            *mminus = 1;
        }
    }

    k = static_cast<int>(Math::Ceil((e + p - 1) * INV_LOG_OF_TEN_BASE_2 - 1e-10));

    if (k > 0) {
        timesTenToTheEHighPrecision(S, STemp_SIZE, k);
    } else {
        timesTenToTheEHighPrecision(R     , RM_SIZE, -k);
        timesTenToTheEHighPrecision(mplus , RM_SIZE, -k);
        timesTenToTheEHighPrecision(mminus, RM_SIZE, -k);
    }

    RLength = mplus_Length = mminus_Length = RM_SIZE;
    SLength = TempLength = STemp_SIZE;

    memset(Temp + RM_SIZE, 0, (STemp_SIZE - RM_SIZE) * sizeof(UInt64));
    memcpy(Temp, R, RM_SIZE * sizeof (UInt64));

    while (RLength > 1 && R[RLength - 1] == 0)
        --RLength;
    while (mplus_Length > 1 && mplus[mplus_Length - 1] == 0)
        --mplus_Length;
    while (mminus_Length > 1 && mminus[mminus_Length - 1] == 0)
        --mminus_Length;
    while (SLength > 1 && S[SLength - 1] == 0)
      --SLength;
    TempLength = (RLength > mplus_Length ? RLength : mplus_Length) + 1;
    addHighPrecision(Temp, TempLength, mplus, mplus_Length);

    if (compareHighPrecision(Temp, TempLength, S, SLength) >= 0) {
        firstK = k;
    } else {
        firstK = k - 1;
        simpleAppendDecimalDigitHighPrecision(R     , ++RLength      , 0);
        simpleAppendDecimalDigitHighPrecision(mplus , ++mplus_Length , 0);
        simpleAppendDecimalDigitHighPrecision(mminus, ++mminus_Length, 0);
        while (RLength > 1 && R[RLength - 1] == 0)
            --RLength;
        while (mplus_Length > 1 && mplus[mplus_Length - 1] == 0)
            --mplus_Length;
        while (mminus_Length > 1 && mminus[mminus_Length - 1] == 0)
            --mminus_Length;
    }

    Int32 digitCount = 0;
    do {
        U = 0;
        for (i = 3; i >= 0; --i) {
            TempLength = SLength + 1;
            Temp[SLength] = 0;
            memcpy (Temp, S, SLength * sizeof(UInt64));
            simpleShiftLeftHighPrecision(Temp, TempLength, i);
            if (compareHighPrecision(R, RLength, Temp, TempLength) >= 0) {
                subtractHighPrecision (R, RLength, Temp, TempLength);
                U += 1 << i;
            }
        }

        low = compareHighPrecision(R, RLength, mminus, mminus_Length) <= 0;

        memset(Temp + RLength, 0, (STemp_SIZE - RLength) * sizeof (UInt64));
        memcpy(Temp, R, RLength * sizeof (UInt64));
        TempLength = (RLength > mplus_Length ? RLength : mplus_Length) + 1;
        addHighPrecision (Temp, TempLength, mplus, mplus_Length);

        high = compareHighPrecision(Temp, TempLength, S, SLength) >= 0;

        if (low || high)
            break;

        simpleAppendDecimalDigitHighPrecision(R     , ++RLength      , 0);
        simpleAppendDecimalDigitHighPrecision(mplus , ++mplus_Length , 0);
        simpleAppendDecimalDigitHighPrecision(mminus, ++mminus_Length, 0);
        while (RLength > 1 && R[RLength - 1] == 0)
            --RLength;
        while (mplus_Length > 1 && mplus[mplus_Length - 1] == 0)
            --mplus_Length;
        while (mminus_Length > 1 && mminus[mminus_Length - 1] == 0)
            --mminus_Length;
        mDigits[digitCount++] = U;
    } while (1);

    simpleShiftLeftHighPrecision(R, ++RLength, 1);
    if (low && !high)
        mDigits[digitCount++] = U;
    else if (high && !low)
        mDigits[digitCount++] = U + 1;
    else if (compareHighPrecision(R, RLength, S, SLength) < 0)
        mDigits[digitCount++] = U;
    else
        mDigits[digitCount++] = U + 1;

    mDigitCount = digitCount;
    mFirstK = firstK;

}

} // namespace Core
} // namespace Elastos
