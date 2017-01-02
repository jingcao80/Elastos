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

#include "IntegralToString.h"
#include "Character.h"
#include "StringBuilder.h"
#include "Math.h"
#include <stdlib.h>

namespace Elastos {
namespace Core {

const Int32 IntegralToString::SMALL_VALUES_LENGTH;
const Int32 IntegralToString::TENS_LENGTH;
const Int32 IntegralToString::ONES_LENGTH;
const Int32 IntegralToString::MOD_10_TABLE_LENGTH;
const Int32 IntegralToString::DIGITS_LENGTH;

/** Ones [i] contains the tens digit of the number i, 0 <= i <= 99. */
const char IntegralToString::TENS[TENS_LENGTH] = {
    '0', '0', '0', '0', '0', '0', '0', '0', '0', '0',
    '1', '1', '1', '1', '1', '1', '1', '1', '1', '1',
    '2', '2', '2', '2', '2', '2', '2', '2', '2', '2',
    '3', '3', '3', '3', '3', '3', '3', '3', '3', '3',
    '4', '4', '4', '4', '4', '4', '4', '4', '4', '4',
    '5', '5', '5', '5', '5', '5', '5', '5', '5', '5',
    '6', '6', '6', '6', '6', '6', '6', '6', '6', '6',
    '7', '7', '7', '7', '7', '7', '7', '7', '7', '7',
    '8', '8', '8', '8', '8', '8', '8', '8', '8', '8',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
};

/** Ones [i] contains the tens digit of the number i, 0 <= i <= 99. */
const char IntegralToString::ONES[ONES_LENGTH] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
};

const char IntegralToString::MOD_10_TABLE[MOD_10_TABLE_LENGTH] = {
    0, 1, 2, 2, 3, 3, 4, 5, 5, 6, 7, 7, 8, 8, 9, 0
};

const char IntegralToString::DIGITS[DIGITS_LENGTH] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
    'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't',
    'u', 'v', 'w', 'x', 'y', 'z'
};

const char IntegralToString::UPPER_CASE_DIGITS[DIGITS_LENGTH] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
    'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
    'U', 'V', 'W', 'X', 'Y', 'Z'
};

AutoPtr<ArrayOf<String> > IntegralToString::SMALL_NONNEGATIVE_VALUES;
AutoPtr<ArrayOf<String> > IntegralToString::SMALL_NEGATIVE_VALUES;

String IntegralToString::ToString(
    /* [in] */ Int32 i,
    /* [in] */ Int32 radix)
{
    if (radix < Character::MIN_RADIX || radix > Character::MAX_RADIX) {
        radix = 10;
    }
    if (radix == 10) {
        return ConvertInt32(i);
    }

    /*
     * If i is positive, negate it. This is the opposite of what one might
     * expect. It is necessary because the range of the negative values is
     * strictly larger than that of the positive values: there is no
     * positive value corresponding to Integer.MIN_VALUE.
     */
    Boolean negative = FALSE;
    if (i < 0) {
        negative = TRUE;
    }
    else {
        i = -i;
    }

    Int32 bufLen = radix < 8 ? 33 : 12;  // Max chars in result (conservative)
    AutoPtr< ArrayOf<Byte> > buf = ArrayOf<Byte>::Alloc(bufLen);
    Int32 cursor = bufLen;

    do {
        Int32 q = i / radix;
        (*buf)[--cursor] = DIGITS[radix * q - i];
        i = q;
    } while (i != 0);

    if (negative) {
        (*buf)[--cursor] = '-';
    }

    String str((const char*)buf->GetPayload() + cursor, bufLen - cursor);
    return str;
}

String IntegralToString::ConvertInt32(
    /* [in] */ Int32 i)
{
    // If i is Integer.MIN_VALUE, -i is still negative
    if (i == Math::INT32_MIN_VALUE) {
        return String("-2147483648");
    }

    Boolean negative = FALSE;
    String quickResult;
    if (i < 0) {
        negative = TRUE;
        i = -i;
        if (i < 100) {
            if (SMALL_NEGATIVE_VALUES == NULL) {
                SMALL_NEGATIVE_VALUES = ArrayOf<String>::Alloc(SMALL_VALUES_LENGTH);
            }

            quickResult = (*SMALL_NEGATIVE_VALUES)[i];
            if (quickResult.IsNull()) {
                StringBuilder sb(3);
                if (i < 10) {
                    sb.AppendChar('-');
                    sb.AppendChar(ONES[i]);
                }
                else {
                    sb.AppendChar('-');
                    sb.AppendChar(TENS[i]);
                    sb.AppendChar(ONES[i]);
                }
                sb.ToString(&quickResult);
                (*SMALL_NEGATIVE_VALUES)[i] = quickResult;
            }
        }
    }
    else {
        if (i < 100) {
            if (SMALL_NONNEGATIVE_VALUES == NULL) {
                SMALL_NONNEGATIVE_VALUES = ArrayOf<String>::Alloc(SMALL_VALUES_LENGTH);
            }

            quickResult = (*SMALL_NONNEGATIVE_VALUES)[i];
            if (quickResult.IsNull()) {
                StringBuilder sb(2);
                if (i < 10) {
                    sb.AppendChar(ONES[i]);
                }
                else {
                    sb.AppendChar(TENS[i]);
                    sb.AppendChar(ONES[i]);
                }
                sb.ToString(&quickResult);
                (*SMALL_NONNEGATIVE_VALUES)[i] = quickResult;
            }
        }
    }
    if (!quickResult.IsNull()) {
        return quickResult;
    }
    Int32 bufLen = 11; // Max number of chars in result
    AutoPtr<ArrayOf<char> > buf = ArrayOf<char>::Alloc(bufLen);
    //char[] buf = (sb != null) ? BUFFER.get() : new char[bufLen];
    Int32 cursor = bufLen;

    // Calculate digits two-at-a-time till remaining digits fit in 16 bits
    while (i >= (1 << 16)) {
        // Compute q = n/100 and r = n % 100 as per "Hacker's Delight" 10-8
        // Int32 q = (Int32) ((0x51EB851FL * i) >>> 37);
        UInt64 qq= (UInt64)(((UInt64)0x51EB851FL * i) >> 37);
        Int32 q = ((Int32 *)&qq)[0];
        Int32 r = i - 100*q;
        (*buf)[--cursor] = ONES[r];
        (*buf)[--cursor] = TENS[r];
        i = q;
    }

    // Calculate remaining digits one-at-a-time for performance
    while (i != 0) {
        // Compute q = n/10 and r = n % 10 as per "Hacker's Delight" 10-8
        //Int32 q = (0xCCCD * i) >>> 19;
        UInt64 qq=(UInt64)(((UInt64)0xCCCD * i) >> 19);
        Int32 q = ((Int32 *)&qq)[0];
        Int32 r = i - 10*q;
        (*buf)[--cursor] = DIGITS[r];
        i = q;
    }

    if (negative) {
        (*buf)[--cursor] = '-';
    }

    quickResult = String((const char*)buf->GetPayload() + cursor, bufLen - cursor);
    return quickResult;
}

String IntegralToString::ToString(
    /* [in] */ Int64 v,
    /* [in] */ Int32 radix)
{
    Int32 i = (Int32) v;
    if (i == v) {
        return ToString(i, radix);
    }

    if (radix < Character::MIN_RADIX || radix > Character::MAX_RADIX) {
        radix = 10;
    }
    if (radix == 10) {
        return ConvertInt64(v);
    }

    /*
     * If v is positive, negate it. This is the opposite of what one might
     * expect. It is necessary because the range of the negative values is
     * strictly larger than that of the positive values: there is no
     * positive value corresponding to Integer.MIN_VALUE.
     */
    Boolean negative = FALSE;
    if (v < 0) {
        negative = TRUE;
    }
    else {
        v = -v;
    }

    Int32 bufLen = radix < 8 ? 65 : 23;  // Max chars in result (conservative)
    AutoPtr< ArrayOf<Byte> > buf = ArrayOf<Byte>::Alloc(bufLen);
    Int32 cursor = bufLen;

    do {
        Int64 q = v / radix;
        (*buf)[--cursor] = DIGITS[(Int32) (radix * q - v)];
        v = q;
    } while (v != 0);

    if (negative) {
        (*buf)[--cursor] = '-';
    }

    return String((const char*)buf->GetPayload() + cursor, bufLen - cursor);
}

String IntegralToString::ConvertInt64(
    /* [in] */ Int64 n)
{
    Int32 i = (Int32) n;
    if (i == n) {
        return ConvertInt32(i);
    }

    Boolean negative = (n < 0);
    if (negative) {
        n = -n;
        if (n < 0) {
            // If -n is still negative, n is Long.MIN_VALUE
            String quickResult("-9223372036854775808");
            return quickResult;
        }
    }

    Int32 bufLen = 20; // Maximum number of chars in result
    //char[] buf = (sb != null) ? BUFFER.get() : new char[bufLen];
    AutoPtr< ArrayOf<Byte> > buf = ArrayOf<Byte>::Alloc(bufLen);

    Int32 low = (Int32) (n % 1000000000); // Extract low-order 9 digits
    Int32 cursor = Int32IntoCharArray(*buf, bufLen, low);

    // Zero-pad Low order part to 9 digits
    while (cursor != (bufLen - 9)) {
        (*buf)[--cursor] = '0';
    }

    /*
     * The remaining digits are (n - low) / 1,000,000,000.  This
     * "exact division" is done as per the online addendum to Hank Warren's
     * "Hacker's Delight" 10-20, http://www.hackersdelight.org/divcMore.pdf
     */
    //Int64 newN = (n - low) / 1000000000;
    n = (((UInt64)(n - low)) >> 9) * 0x8E47CE423A2E9C6DL;

    /*
     * If the remaining digits fit in an Int32, emit them using a
     * single call to Int32IntoCharArray. Otherwise, strip off the
     * low-order digit, put it in buf, and then call Int32IntoCharArray
     * on the remaining digits (which now fit in an Int32).
     */
    if ((n & ((UInt64)-1L << 32)) == 0) {
        cursor = Int32IntoCharArray(*buf, cursor, (Int32) n);
    }
    else {
        /*
         * Set midDigit to n % 10
         */
        Int32 lo32 = (Int32) n;
        Int32 hi32 = (Int32) (((UInt64)n) >> 32);

        // midDigit = ((unsigned) low32) % 10, per "Hacker's Delight" 10-21
        UInt32 ulo32 = (UInt32)lo32;
        Int32 index = ((UInt32)(0x19999999 * lo32 + (ulo32 >> 1) + (ulo32 >> 3))) >> 28;
        Int32 midDigit = MOD_10_TABLE[index];

        // Adjust midDigit for hi32. (assert hi32 == 1 || hi32 == 2)
        midDigit -= hi32 << 2;  // 1L << 32 == -4 MOD 10
        if (midDigit < 0) {
            midDigit += 10;
        }

        (*buf)[--cursor] = DIGITS[midDigit];

        // Exact division as per Warren 10-20
        UInt64 umd = (n - midDigit);
        Int32 rest = ((Int32) (umd >> 1)) * 0xCCCCCCCD;
        cursor = Int32IntoCharArray(*buf, cursor, rest);
    }

    if (negative) {
        (*buf)[--cursor] = '-';
    }

    String str = String((const char*)buf->GetPayload() + cursor, bufLen - cursor);
    return str;
}

Int32 IntegralToString::Int32IntoCharArray(
    /* [in] */ ArrayOf<Byte>& buf,
    /* [in] */ Int32 cursor,
    /* [in] */ Int32 n)
{
    // Calculate digits two-at-a-time till remaining digits fit in 16 bits
    while ((n & 0xffff0000) != 0) {
        /*
         * Compute q = n/100 and r = n % 100 as per "Hacker's Delight" 10-8.
         * This computation is slightly different from the corresponding
         * computation in intToString: the shifts before and after
         * multiply can't be combined, as that would yield the wrong result
         * if n's sign bit were set.
         */
        UInt64 un = (((UInt64)0x51EB851FL * (((UInt32)n) >> 2)) >> 35);
        Int32 q = (Int32)un;
        Int32 r = n - 100* q ;
        buf[--cursor] = ONES[r];
        buf[--cursor] = TENS[r];
        n = q;
    }

    // Calculate remaining digits one-at-a-time for performance
    while (n != 0) {
        // Compute q = n / 10 and r = n % 10 as per "Hacker's Delight" 10-8
        Int32 q = (Int32)(((UInt64)0xCCCD * ((UInt32)n)) >> 19);
        Int32 r = n - 10*q;
        buf[--cursor] = DIGITS[r];
        n = q;
    }
    return cursor;
}

String IntegralToString::ToBinaryString(
    /* [in] */ Int32 i)
{
    Int32 bufLen = 32;  // Max number of binary digits in an int
    AutoPtr< ArrayOf<Byte> > buf = ArrayOf<Byte>::Alloc(bufLen);
    Int32 cursor = bufLen;

    UInt32 idx = (UInt32)i;
    do {
        (*buf)[--cursor] = DIGITS[idx & 1];
    }  while ((idx >>= 1) != 0);

    String str = String((const char*)buf->GetPayload() + cursor, bufLen - cursor);
    return str;
}

String IntegralToString::ToBinaryString(
    /* [in] */ Int64 v)
{
    Int32 i = (Int32) v;
    if (v >= 0 && i == v) {
        return ToBinaryString(i);
    }

    Int32 bufLen = 64;  // Max number of binary digits in a long
    AutoPtr< ArrayOf<Byte> > buf = ArrayOf<Byte>::Alloc(bufLen);
    Int32 cursor = bufLen;

    UInt64 uv = (UInt64)v;
    do {
        (*buf)[--cursor] = DIGITS[((Int32) uv) & 1];
    } while ((uv >>= 1) != 0);

    String str = String((const char*)buf->GetPayload() + cursor, bufLen - cursor);
    return str;
}

StringBuilder& IntegralToString::AppendByteAsHex(
    /* [in] */ StringBuilder& sb,
    /* [in] */ Byte b,
    /* [in] */ Boolean upperCase)
{
    const char* digits = upperCase ? UPPER_CASE_DIGITS : DIGITS;
    sb.Append(digits[(b >> 4) & 0xf]);
    sb.Append(digits[b & 0xf]);
    return sb;
}

String IntegralToString::ToHexString(
    /* [in] */ Byte b,
    /* [in] */ Boolean upperCase)
{
    const char* digits = upperCase ? UPPER_CASE_DIGITS : DIGITS;
    StringBuilder sb(2);
    sb.AppendChar(digits[(b >> 4) & 0xf]);
    sb.AppendChar(digits[b & 0xf]);

    return sb.ToString();
}

String IntegralToString::ToHexString(
    /* [in] */ ArrayOf<Byte>& bytes,
    /* [in] */ Boolean upperCase)
{
    Int32 len = bytes.GetLength();
    if (len == 0) {
        return String();
    }

    const char* digits = upperCase ? UPPER_CASE_DIGITS : DIGITS;
    char* buf = new char[len * 2 + 1];
    int c = 0;
    Byte b;
    for (Int32 i = 0; i < len; ++i) {
        b = bytes[i];
        buf[c++] = digits[(b >> 4) & 0xf];
        buf[c++] = digits[b & 0xf];
    }
    buf[c] = '\0';
    return String(buf);
}

String IntegralToString::ToHexString(
    /* [in] */ Int32 i,
    /* [in] */ Boolean upperCase,
    /* [in] */ Int32 minWidth)
{
    Int32 bufLen = 8;  // Max number of hex digits in an int
    char buf[8];
    Int32 cursor = bufLen;

    const char* digits = upperCase ? UPPER_CASE_DIGITS : DIGITS;
    do {
        buf[--cursor] = digits[i & 0xf];
    } while ((i = (((UInt32)i) >> 4)) != 0 || (bufLen - cursor < minWidth));

    return String(buf + cursor, bufLen - cursor);
}

String IntegralToString::ToHexString(
    /* [in] */ Int64 v,
    /* [in] */ Boolean upperCase)
{
    Int32 i = (Int32) v;
    if (v >= 0 && i == v) {
        return ToHexString(i, upperCase, 0);
    }

    int bufLen = 16;  // Max number of hex digits in a long
    char buf[16];
    int cursor = bufLen;

    const char* digits = upperCase ? UPPER_CASE_DIGITS : DIGITS;
    do {
        buf[--cursor] = digits[((Int32) v) & 0xF];
    } while ((v = (((UInt64)v) >> 4)) != 0);

    return String(buf + cursor, bufLen - cursor);
}

String IntegralToString::ToOctalString(
    /* [in] */ Int32 i)
{
    Int32 bufLen = 11;  // Max number of octal digits in an Int32
    char buf[11];
    Int32 cursor = bufLen;

    do {
        buf[--cursor] = DIGITS[i & 7];
    } while ((i = (((UInt32)i) >> 3) ) != 0);

    return String(buf + cursor, bufLen - cursor);
}

String IntegralToString::ToOctalString(
    /* [in] */ Int64 v)
{
    Int32 i = (Int32) v;
    if (v >= 0 && i == v) {
        return ToOctalString(i);
    }

    Int32 bufLen = 22;  // Max number of octal digits in a long
    char buf[22];
    Int32 cursor = bufLen;

    do {
        buf[--cursor] = DIGITS[((Int32) v) & 7];
    } while ((v = (((UInt64)v) >> 3)) != 0);

    return String(buf + cursor, bufLen - cursor);
}

} // namespace Core
} // namespace Elastos
