#include "Conversion.h"
#include "BitLevel.h"
#include "BigInt.h"
#include "Division.h"
#include "Math.h"
#include "Character.h"
#include "IntegralToString.h"
#include "StringBuilder.h"
#include "StringUtils.h"

using Elastos::Core::Math;
using Elastos::Core::Character;
using Elastos::Core::StringUtils;
using Elastos::Core::EIID_INumber;
using Elastos::Core::StringBuilder;
using Elastos::Core::IntegralToString;

namespace Elastos {
namespace Math {

const Int32 Conversion::DigitFitInInt[] = {
    -1, -1, 31, 19, 15, 13, 11,
    11, 10, 9, 9, 8, 8, 8, 8, 7, 7, 7, 7, 7, 7, 7, 6, 6, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 5
};

const Int32 Conversion::BigRadices[] = {
    (Int32)0x80000000, 1162261467,
    1073741824, 1220703125, 362797056, 1977326743, 1073741824,
    387420489, 1000000000, 214358881, 429981696, 815730721, 1475789056,
    170859375, 268435456, 410338673, 612220032, 893871739, 1280000000,
    1801088541, 113379904, 148035889, 191102976, 244140625, 308915776,
    387420489, 481890304, 594823321, 729000000, 887503681, 1073741824,
    1291467969, 1544804416, 1838265625, 60466176
};

String Conversion::BigInteger2String(
    /* [in] */ CBigInteger& val,
    /* [in] */ Int32 radix)
{
    val.PrepareJavaRepresentation();
    Int32 sign = val.mSign;
    Int32 numberLength = val.mNumberLength;
    ArrayOf<Int32>* digits = val.mDigits;

    if (sign == 0) {
        return String("0");
    }

    if (numberLength == 1) {
        Int32 highDigit = (*digits)[numberLength - 1];
        Int64 v = highDigit & 0xFFFFFFFFL;
        if (sign < 0) {
            v = -v;
        }

        return IntegralToString::ToString(v, radix);
    }

    if ((radix == 10) || (radix < Character::MIN_RADIX)
            || (radix > Character::MAX_RADIX)) {
        String str;
        val.ToString(&str);
        return str;
    }

    using Elastos::Core::Math;

    AutoPtr<IBigInteger> absBi;
    val.Abs((IBigInteger**)&absBi);
    Int32 absBitLen = 0;
    absBi->BitLength(&absBitLen);

    Double bitsForRadixDigit;
    bitsForRadixDigit = Math::Log(radix) / Math::Log(2);
    Int32 resLengthInChars =
            (Int32) (absBitLen / bitsForRadixDigit + ((sign < 0) ? 1 : 0)) + 1;

    AutoPtr< ArrayOf<Byte> > result = ArrayOf<Byte>::Alloc(resLengthInChars);

    Int32 currentChar = resLengthInChars;
    Int32 resDigit;
    if (radix != 16) {
        AutoPtr< ArrayOf<Int32> > temp = ArrayOf<Int32>::Alloc(numberLength);
        temp->Copy(digits, 0, numberLength);
        Int32 tempLen = numberLength;
        Int32 charsPerInt = DigitFitInInt[radix];
        Int32 i;
        // get the maximal power of radix that fits in Int32
        Int32 bigRadix = BigRadices[radix - 2];
        while (true) {
            // divide the array of digits by bigRadix and convert remainders
            // to characters collecting them in the char array
            resDigit = Division::DivideArrayByInt(*temp, *temp, tempLen, bigRadix);
            Int32 previous = currentChar;
            do {
                (*result)[--currentChar] = Character::ForDigit(resDigit % radix, radix);
            } while (((resDigit /= radix) != 0) && (currentChar != 0));

            Int32 delta = charsPerInt - previous + currentChar;
            for (i = 0; i < delta && currentChar > 0; i++) {
                (*result)[--currentChar] = '0';
            }
            for (i = tempLen - 1; (i > 0) && ((*temp)[i] == 0); i--) {
                ;
            }
            tempLen = i + 1;
            if ((tempLen == 1) && ((*temp)[0] == 0)) { // the quotient is 0
                break;
            }
        }
    }
    else {
        // radix == 16
        for (Int32 i = 0; i < numberLength; i++) {
            for (Int32 j = 0; (j < 8) && (currentChar > 0); j++) {
                resDigit = (*digits)[i] >> (j << 2) & 0xf;
                (*result)[--currentChar] = Character::ForDigit(resDigit, 16);
            }
        }
    }
    while ((*result)[currentChar] == '0') {
        currentChar++;
    }
    if (sign == -1) {
        (*result)[--currentChar] = '-';
    }

    const char* resultStr = (const char*)result->GetPayload();
    return String(resultStr + currentChar, resLengthInChars - currentChar);
}

String Conversion::ToDecimalScaledString(
    /* [in] */ CBigInteger& val,
    /* [in] */ Int32 scale)
{
    val.PrepareJavaRepresentation();
    Int32 sign = val.mSign;

    if (sign == 0) {
        switch (scale) {
            case 0:
                return String("0");
            case 1:
                return String("0.0");
            case 2:
                return String("0.00");
            case 3:
                return String("0.000");
            case 4:
                return String("0.0000");
            case 5:
                return String("0.00000");
            case 6:
                return String("0.000000");
            default:
                StringBuilder result1;
                if (scale < 0) {
                    result1.Append("0E+");
                } else {
                    result1.Append("0E");
                }
                result1.Append(-scale);
                return result1.ToString();
        }
    }

    Int32 numberLength = val.mNumberLength;
    ArrayOf<Int32>* digits = val.mDigits;
    Int32 resLengthInChars;
    Int32 currentChar;

    // one 32-bit unsigned value may contains 10 decimal digits
    resLengthInChars = numberLength * 10 + 1 + 7;
    // Explanation why +1+7:
    // +1 - one char for sign if needed.
    // +7 - For "special case 2" (see below) we have 7 free chars for
    // inserting necessary scaled digits.
    AutoPtr<ArrayOf<Byte> > result = ArrayOf<Byte>::Alloc(resLengthInChars + 1);

    // allocated [resLengthInChars+1] characters.
    // a free latest character may be used for "special case 1" (see
    // below)
    currentChar = resLengthInChars;
    if (numberLength == 1) {
        Int32 highDigit = (*digits)[0];
        if (highDigit < 0) {
            Int64 v = highDigit & 0xFFFFFFFFL;
            do {
                Int64 prev = v;
                v /= 10;
                (*result)[--currentChar] = (char) (0x0030 + ((Int32) (prev - v * 10)));
            } while (v != 0);
        }
        else {
            Int32 v = highDigit;
            do {
                Int32 prev = v;
                v /= 10;
                (*result)[--currentChar] = (char) (0x0030 + (prev - v * 10));
            } while (v != 0);
        }
    }
    else {
        AutoPtr<ArrayOf<Int32> > temp = ArrayOf<Int32>::Alloc(numberLength);
        Int32 tempLen = numberLength;
        temp->Copy(digits, 0, tempLen);

        Boolean exitLoop = FALSE;
        while (true) {
            // divide the array of digits by bigRadix and convert
            // remainders
            // to characters collecting them in the char array
            Int64 result11 = 0;
            for (Int32 i1 = tempLen - 1; i1 >= 0; i1--) {
                Int64 temp1 = (result11 << 32)
                        + ((*temp)[i1] & 0xFFFFFFFFL);
                Int64 res = DivideLongByBillion(temp1);
                (*temp)[i1] = (Int32) res;
                result11 = (Int32) (res >> 32);
            }

            Int32 resDigit = (Int32) result11;
            Int32 previous = currentChar;
            do {
                (*result)[--currentChar] = (char) (0x0030 + (resDigit % 10));
            } while (((resDigit /= 10) != 0) && (currentChar != 0));

            Int32 delta = 9 - previous + currentChar;
            for (Int32 i = 0; (i < delta) && (currentChar > 0); i++) {
                (*result)[--currentChar] = '0';
            }

            Int32 j = tempLen - 1;
            for (; (*temp)[j] == 0; j--) {
                if (j == 0) { // means temp[0] == 0
                    exitLoop = TRUE;
                    break;
                }
            }

            if (exitLoop) {
                break;
            }

            tempLen = j + 1;
        }

        while ((*result)[currentChar] == '0') {
            currentChar++;
        }
    }

    Boolean negNumber = (sign < 0);
    Int32 exponent = resLengthInChars - currentChar - scale - 1;
    if (scale == 0) {
        if (negNumber) {
            (*result)[--currentChar] = '-';
        }
        return String((const char*)result->GetPayload() + currentChar, resLengthInChars
                - currentChar);
    }
    if ((scale > 0) && (exponent >= -6)) {
        if (exponent >= 0) {
            // special case 1
            Int32 insertPoint = currentChar + exponent;
            for (Int32 j = resLengthInChars - 1; j >= insertPoint; j--) {
                (*result)[j + 1] = (*result)[j];
            }
            (*result)[++insertPoint] = '.';
            if (negNumber) {
                (*result)[--currentChar] = '-';
            }
            return String((const char*)result->GetPayload() + currentChar, resLengthInChars
                    - currentChar + 1);
        }

        // special case 2
        for (Int32 j = 2; j < -exponent + 1; j++) {
            (*result)[--currentChar] = '0';
        }
        (*result)[--currentChar] = '.';
        (*result)[--currentChar] = '0';
        if (negNumber) {
            (*result)[--currentChar] = '-';
        }
        return String((const char*)result->GetPayload() + currentChar, resLengthInChars
                - currentChar);
    }

    Int32 startPoint = currentChar + 1;
    Int32 endPoint = resLengthInChars;
    StringBuilder result1(16 + endPoint - startPoint);
    if (negNumber) {
        result1.AppendChar('-');
    }
    if (endPoint - startPoint >= 1) {
        result1.AppendChar((*result)[currentChar]);
        result1.AppendChar('.');

        String str((const char*)result->GetPayload() + currentChar + 1, resLengthInChars
                - currentChar - 1);
        result1.Append(str);
    }
    else {
        String str((const char*)result->GetPayload() + currentChar, resLengthInChars
                - currentChar);
        result1.Append(str);
    }
    result1.AppendChar('E');
    if (exponent > 0) {
        result1.AppendChar('+');
    }
    result1.Append(StringUtils::ToString(exponent));
    return result1.ToString();
}

/* can process only 32-bit numbers */
String Conversion::ToDecimalScaledString(
    /* [in] */ Int64 value,
    /* [in] */ Int32 scale)
{
    using Elastos::Core::Math;

    Boolean negNumber = value < 0;
    if(negNumber) {
        value = -value;
    }
    if (value == 0) {
        switch (scale) {
            case 0: return String("0");
            case 1: return String("0.0");
            case 2: return String("0.00");
            case 3: return String("0.000");
            case 4: return String("0.0000");
            case 5: return String("0.00000");
            case 6: return String("0.000000");
            default:
                StringBuilder result1;
                if (scale  < 0) {
                    result1.Append("0E+");
                } else {
                    result1.Append("0E");
                }
                if (scale == Math::INT32_MIN_VALUE) {
                    result1.Append("2147483648");
                }
                else {
                    result1.Append(StringUtils::ToString(-scale));
                }
                return result1.ToString();
        }
    }

    // one 32-bit unsigned value may contains 10 decimal digits
    Int32 resLengthInChars = 18;
    // Explanation why +1+7:
    // +1 - one char for sign if needed.
    // +7 - For "special case 2" (see below) we have 7 free chars for
    //  inserting necessary scaled digits.
    AutoPtr<ArrayOf<Byte> > result = ArrayOf<Byte>::Alloc(resLengthInChars+1);
    //  Allocated [resLengthInChars+1] characters.
    // a free latest character may be used for "special case 1" (see below)
    Int32 currentChar = resLengthInChars;
    Int64 v = value;
    do {
        Int64 prev = v;
        v /= 10;
        (*result)[--currentChar] = (char) (0x0030 + (prev - v * 10));
    } while (v != 0);

    Int64 exponent = (Int64)resLengthInChars - (Int64)currentChar - scale - 1L;
    if (scale == 0) {
        if (negNumber) {
            (*result)[--currentChar] = '-';
        }
        return String((const char*)result->GetPayload() + currentChar, resLengthInChars - currentChar);
    }

    if (scale > 0 && exponent >= -6) {
        if (exponent >= 0) {
            // special case 1
            Int32 insertPoint = currentChar + (Int32) exponent ;
            for (Int32 j=resLengthInChars-1; j>=insertPoint; j--) {
                (*result)[j+1] = (*result)[j];
            }
            (*result)[++insertPoint]='.';
            if (negNumber) {
                (*result)[--currentChar] = '-';
            }
            return String((const char*)result->GetPayload() + currentChar, resLengthInChars - currentChar + 1);
        }

        // special case 2
        for (Int32 j = 2; j < -exponent + 1; j++) {
            (*result)[--currentChar] = '0';
        }
        (*result)[--currentChar] = '.';
        (*result)[--currentChar] = '0';
        if (negNumber) {
            (*result)[--currentChar] = '-';
        }
        return String((const char*)result->GetPayload() + currentChar, resLengthInChars - currentChar);
    }

    Int32 startPoint = currentChar + 1;
    Int32 endPoint = resLengthInChars;
    StringBuilder result1(16 + endPoint - startPoint);
    if (negNumber) {
        result1.AppendChar('-');
    }
    if (endPoint - startPoint >= 1) {
        result1.AppendChar((*result)[currentChar]);
        result1.AppendChar('.');
        String temp((const char*)result->GetPayload() + currentChar+1,resLengthInChars - currentChar-1);
        result1.Append(temp);
    }
    else {
        String temp((const char*)result->GetPayload() + currentChar,resLengthInChars - currentChar);
        result1.Append(temp);
    }

    result1.AppendChar('E');
    if (exponent > 0) {
        result1.AppendChar('+');
    }
    result1.Append(StringUtils::ToString(exponent));
    return result1.ToString();
}

Int64 Conversion::DivideLongByBillion(
    /* [in] */ Int64 a)
{
    Int64 quot;
    Int64 rem;

    if (a >= 0) {
        Int64 bLong = 1000000000L;
        quot = (a / bLong);
        rem = (a % bLong);
    } else {
        /*
         * Make the dividend positive shifting it right by 1 bit then get
         * the quotient an remainder and correct them properly
         */
        Int64 aPos = ((UInt64)a) >> 1;//a >>> 1;
        Int64 bPos = 1000000000L >> 1;;//1000000000L >>> 1;
        quot = aPos / bPos;
        rem = aPos % bPos;
        // double the remainder and add 1 if 'a' is odd
        rem = (rem << 1) + (a & 1);
    }
    return ((rem << 32) | (quot & 0xFFFFFFFFL));
}

/** @see BigInteger#doubleValue() */
Double Conversion::BigInteger2Double(
    /* [in] */ CBigInteger& val)
{
    using Elastos::Core::Math;

    val.PrepareJavaRepresentation();
    // val.bitLength() < 64
    if ((val.mNumberLength < 2)
            || ((val.mNumberLength == 2) && (val.GetDigit(1) > 0))) {
        Int64 value;
        val.Int64Value(&value);
        return value;
    }

    // val.bitLength() >= 33 * 32 > 1024
    if (val.mNumberLength > 32) {
        return ((val.mSign > 0) ? Math::DOUBLE_POSITIVE_INFINITY
                : Math::DOUBLE_NEGATIVE_INFINITY);
    }

    AutoPtr<IBigInteger> absBi;
    val.Abs((IBigInteger**)&absBi);
    Int32 bitLen = 0;
    absBi->BitLength(&bitLen);

    Int64 exponent = bitLen - 1;
    Int32 delta = bitLen - 54;
    // We need 54 top bits from this, the 53th bit is always 1 in lVal.
    AutoPtr<IBigInteger> srBi;
    absBi->ShiftRight(delta, (IBigInteger**)&srBi);
    Int64 lVal = 0;
    INumber* number = (INumber*)srBi->Probe(EIID_INumber);
    number->Int64Value(&lVal);

    /*
     * Take 53 bits from lVal to mantissa. The least significant bit is
     * needed for rounding.
     */
    Int64 mantissa = lVal & 0x1FFFFFFFFFFFFFL;
    if (exponent == 1023) {
        if (mantissa == 0X1FFFFFFFFFFFFFL) {
            return ((val.mSign > 0) ? Math::DOUBLE_POSITIVE_INFINITY
                    : Math::DOUBLE_NEGATIVE_INFINITY);
        }
        if (mantissa == 0x1FFFFFFFFFFFFEL) {
            return ((val.mSign > 0) ? Math::DOUBLE_MAX_VALUE : -Math::DOUBLE_MAX_VALUE);
        }
    }
    // Round the mantissa
    if (((mantissa & 1) == 1)
            && (((mantissa & 2) == 2) || BitLevel::NonZeroDroppedBits(delta,
                    *(val.mDigits)))) {
        mantissa += 2;
    }
    mantissa >>= 1; // drop the rounding bit
    Int64 resSign = (val.mSign < 0) ? 0x8000000000000000L : 0;
    exponent = ((1023 + exponent) << 52) & 0x7FF0000000000000L;
    Int64 result = resSign | exponent | mantissa;
    return Math::Int64BitsToDouble(result);
}

} // namespace Math
} // namespace Elastos
