
#include "BitLevel.h"
#include "Math.h"
#include <assert.h>

namespace Elastos {
namespace Math {

Int32 BitLevel::BitLength(
    /* [in] */ CBigInteger& val)
{
    val.PrepareJavaRepresentation();
    if (val.mSign == 0) {
        return 0;
    }
    Int32 bLength = (val.mNumberLength << 5);
    Int32 highDigit = val.GetDigit(val.mNumberLength - 1);

    if (val.mSign < 0) {
        Int32 i = val.GetFirstNonzeroDigit();
        // We reduce the problem to the positive case.
        if (i == val.mNumberLength - 1) {
            highDigit--;
        }
    }
    // Subtracting all sign bits
    bLength -= Elastos::Core::Math::NumberOfLeadingZeros(highDigit);
    return bLength;
}

Int32 BitLevel::BitCount(
    /* [in] */ CBigInteger& val)
{
    val.PrepareJavaRepresentation();
    Int32 bCount = 0;

    if (val.mSign == 0) {
        return 0;
    }

    Int32 i = val.GetFirstNonzeroDigit();
    if (val.mSign > 0) {
        for ( ; i < val.mNumberLength; i++) {
            bCount += Elastos::Core::Math::BitCount(val.GetDigit(i));
        }
    }
    else {// (sign < 0)
        // this digit absorbs the carry
        bCount += Elastos::Core::Math::BitCount(-val.GetDigit(i));
        for (i++; i < val.mNumberLength; i++) {
            bCount += Elastos::Core::Math::BitCount(~val.GetDigit(i));
        }
        // We take the complement sum:
        bCount = (val.mNumberLength << 5) - bCount;
    }
    return bCount;
}

Boolean BitLevel::TestBit(
    /* [in] */ CBigInteger& val,
    /* [in] */ Int32 n)
{
    val.PrepareJavaRepresentation();
    // PRE: 0 <= n < val.bitLength()
    return ((val.GetDigit(n >> 5) & (1 << (n & 31))) != 0);
}

Boolean BitLevel::NonZeroDroppedBits(
    /* [in] */ Int32 numberOfBits,
    /* [in] */ const ArrayOf<Int32>& digits)
{
    Int32 intCount = numberOfBits >> 5;
    Int32 bitCount = numberOfBits & 31;
    Int32 i;

    for (i = 0; (i < intCount) && (digits[i] == 0); i++) {
        ;
    }
    return ((i != intCount) || (digits[i] << (32 - bitCount) != 0));
}

Void BitLevel::ShiftLeftOneBit(
    /* [in,out] */ ArrayOf<Int32>& result,
    /* [in] */ const ArrayOf<Int32>& source,
    /* [in] */ Int32 srcLen)
{
    Int32 carry = 0;
    for (Int32 i = 0; i < srcLen; i++) {
        Int32 val = source[i];
        result[i] = (val << 1) | carry;
        carry = ((UInt32)val) >> 31;
    }
    if(carry != 0) {
        result[srcLen] = carry;
    }
}

ECode BitLevel::ShiftLeftOneBit(
    /* [in] */ CBigInteger* source,
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    assert(NULL != source);

    source->PrepareJavaRepresentation();
    Int32 srcLen = source->mNumberLength;
    Int32 resLen = srcLen + 1;
    AutoPtr< ArrayOf<Int32> > resDigits = ArrayOf<Int32>::Alloc(resLen);
    ShiftLeftOneBit(*resDigits, *(source->mDigits), srcLen);

    return CBigInteger::New(source->mSign, resLen, *resDigits, result);
}

ECode BitLevel::ShiftRight(
    /* [in] */ CBigInteger* source,
    /* [in] */ Int32 count,
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    assert(NULL != source);

    source->PrepareJavaRepresentation();
    Int32 intCount = count >> 5; // count of integers
    count &= 31; // count of remaining bits
    if (intCount >= source->mNumberLength) {
        if (source->mSign < 0) {
            *result = CBigInteger::MINUS_ONE;
            REFCOUNT_ADD(*result);
            return NOERROR;
        }
        else {
            *result = CBigInteger::ZERO;
            REFCOUNT_ADD(*result);
            return NOERROR;
        }
    }

    Int32 i;
    Int32 resLength = source->mNumberLength - intCount;
    AutoPtr< ArrayOf<Int32> > resDigits = ArrayOf<Int32>::Alloc(resLength);
    ArrayOf<Int32>* srcDigits = source->mDigits;

    ShiftRight(*resDigits, *srcDigits, intCount, count);
    if (source->mSign < 0) {
        // Checking if the dropped bits are zeros (the remainder equals to
        // 0)
        for (i = 0; (i < intCount) && ((*srcDigits)[i] == 0); i++) {
            ;
        }
        // If the remainder is not zero, add 1 to the result
        if ((i < intCount)
                || ((count > 0) && (((*srcDigits)[i] << (32 - count)) != 0))) {
            for (i = 0; (i < resLength) && ((*resDigits)[i] == -1); i++) {
                (*resDigits)[i] = 0;
            }
            if (i == resLength) {
                resLength++;
            }
            (*resDigits)[i]++;
        }
    }

    return CBigInteger::New(source->mSign, resLength, *resDigits, result);
}

Boolean BitLevel::ShiftRight(
    /* [in] */ ArrayOf<Int32>& result,
    /* [in] */ const ArrayOf<Int32>& source,
    /* [in] */ Int32 intCount,
    /* [in] */ Int32 count)
{
    Int32 i;
    Int32 resultLen = result.GetLength();
    Boolean allZero = TRUE;
    for (i = 0; i < intCount; i++) {
        allZero &= source[i] == 0;
    }
    if (count == 0) {
        using Elastos::Core::Math;
        Int32 minLen = Math::Min(source.GetLength() - intCount, resultLen);
        for (i = 0; i < minLen; ++i) {
            result[i] = source[intCount + i];
        }
        //i = minLen;
    }
    else {
        Int32 leftShiftCount = 32 - count;
        UInt32 temp;

        allZero &= ( source[i] << leftShiftCount ) == 0;
        for (i = 0; i < resultLen - 1; i++) {
            temp = source[i + intCount];
            result[i] = (temp >> count) | (temp << leftShiftCount);
        }

        temp = source[i + intCount];
        result[i] = (temp >> count);
        i++;
    }

    return allZero;
}

ECode BitLevel::FlipBit(
    /* [in] */ CBigInteger* val,
    /* [in] */ Int32 n,
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    assert(NULL != val);

    val->PrepareJavaRepresentation();
    Int32 resSign = (val->mSign == 0) ? 1 : val->mSign;
    Int32 intCount = n >> 5;
    Int32 bitN = n & 31;
    using Elastos::Core::Math;
    Int32 resLength = Math::Max(intCount + 1, val->mNumberLength) + 1;
    AutoPtr< ArrayOf<Int32> > resDigits = ArrayOf<Int32>::Alloc(resLength);
    ArrayOf<Int32>* srcDigits = val->mDigits;
    Int32 i;

    Int32 bitNumber = 1 << bitN;
    for (Int32 i = 0; i < srcDigits->GetLength(); ++i) {
        (*resDigits)[i] = (*srcDigits)[i];
    }

    if (val->mSign < 0) {
        if (intCount >= val->mNumberLength) {
            (*resDigits)[intCount] = bitNumber;
        }
        else {
            //val->mSign<0 y intCount < val->mNumberLength
            Int32 firstNonZeroDigit = val->GetFirstNonzeroDigit();
            if (intCount > firstNonZeroDigit) {
                (*resDigits)[intCount] ^= bitNumber;
            }
            else if (intCount < firstNonZeroDigit) {
                (*resDigits)[intCount] = -bitNumber;
                for (i = intCount + 1; i < firstNonZeroDigit; i++) {
                    (*resDigits)[i] = -1;
                }
                (*resDigits)[i] = (*resDigits)[i]--;
            }
            else {
                i = intCount;
                (*resDigits)[i] = -((-(*resDigits)[intCount]) ^ bitNumber);
                if ((*resDigits)[i] == 0) {
                    for (i++; (*resDigits)[i] == -1 ; i++) {
                        (*resDigits)[i] = 0;
                    }
                    (*resDigits)[i]++;
                }
            }
        }
    }
    else {//case where val is positive
        (*resDigits)[intCount] ^= bitNumber;
    }

    return CBigInteger::New(resSign, resLength, *resDigits, result);
}

} // namespace Math
} // namespace Elastos
