
#include "Logical.h"
#include "Math.h"
#include <assert.h>

namespace Elastos {
namespace Math {

/** @see BigInteger#not() */
AutoPtr<IBigInteger> Logical::Not(
    /* [in] */ CBigInteger* val)
{
    assert(val != NULL);

    if (val->mSign == 0) {
        return CBigInteger::MINUS_ONE;
    }

    Boolean isEqual = FALSE;
    val->Equals(&(*CBigInteger::MINUS_ONE), &isEqual);
    if (isEqual) {
        return CBigInteger::ZERO;
    }

    AutoPtr<IBigInteger> result;
    AutoPtr< ArrayOf<Int32> > resDigits = val->mDigits->Clone();

    Int32 i;
    if (val->mSign > 0) {
        // ~val = -val + 1
        if (val->GetDigit(val->mNumberLength - 1) != -1) {
            for (i = 0; val->GetDigit(i) == -1; i++) {
                ;
            }
        }
        else {
            for (i = 0; (i < val->mNumberLength) && (val->GetDigit(i) == -1); i++) {
                ;
            }
            if (i == val->mNumberLength) {
                (*resDigits)[i] = 1;
                CBigInteger::New(-val->mSign, i + 1, *resDigits, (IBigInteger**)&result);
                return result;
            }
        }
        // Here a carry 1 was generated
    } else {// (val->mSign < 0)
        // ~val = -val - 1
        for (i = 0; val->GetDigit(i) == 0; i++) {
            (*resDigits)[i] = -1;
        }
        // Here a borrow -1 was generated
    }
    // Now, the carry/borrow can be absorbed
    (*resDigits)[i] = val->GetDigit(i) + val->mSign;
    // Copying the remaining unchanged digit
    for (i++; i < val->mNumberLength; i++) {
        (*resDigits)[i] = val->GetDigit(i);
    }

    CBigInteger::New(-val->mSign, i, *resDigits, (IBigInteger**)&result);
    return result;
}

/** @see BigInteger#and(BigInteger) */
AutoPtr<IBigInteger> Logical::And(
    /* [in] */ CBigInteger* val,
    /* [in] */ CBigInteger* that)
{
    assert(NULL != val && NULL != that);

    if (that->mSign == 0 || val->mSign == 0) {
        return CBigInteger::ZERO;
    }

    AutoPtr<IBigInteger> result;
    Boolean isEqual = FALSE;
    that->Equals(&(*CBigInteger::MINUS_ONE), &isEqual);
    if (isEqual) {
        result = val;
        return result;
    }
    val->Equals(&(*CBigInteger::MINUS_ONE), &isEqual);
    if (isEqual) {
        result = that;
        return result;
    }

    if (val->mSign > 0) {
        if (that->mSign > 0) {
            return AndPositive(val, that);
        }
        else {
            return AndDiffSigns(val, that);
        }
    }
    else {
        if (that->mSign > 0) {
            return AndDiffSigns(that, val);
        }
        else if (val->mNumberLength > that->mNumberLength) {
            return AndNegative(val, that);
        }
        else {
            return AndNegative(that, val);
        }
    }
}

/** @return sign = 1, magnitude = val.magnitude & that.magnitude*/
AutoPtr<IBigInteger> Logical::AndPositive(
    /* [in] */ CBigInteger* val,
    /* [in] */ CBigInteger* that)
{
    assert(NULL != val && NULL != that);

    using Elastos::Core::Math;

    // PRE: both arguments are positive
    Int32 resLength = Math::Min(val->mNumberLength, that->mNumberLength);
    Int32 i = Math::Max(val->GetFirstNonzeroDigit(), that->GetFirstNonzeroDigit());

    if (i >= resLength) {
        return CBigInteger::ZERO;
    }

    AutoPtr< ArrayOf<Int32> > resDigits = ArrayOf<Int32>::Alloc(resLength);
    for ( ; i < resLength; i++) {
        (*resDigits)[i] = val->GetDigit(i) & that->GetDigit(i);
    }

    AutoPtr<IBigInteger> result;
    CBigInteger::New(1, resLength, *resDigits, (IBigInteger**)&result);
    return result;
}

/** @return sign = positive.magnitude & magnitude = -negative.magnitude */
AutoPtr<IBigInteger> Logical::AndDiffSigns(
    /* [in] */ CBigInteger* positive,
    /* [in] */ CBigInteger* negative)
{
    assert(NULL != positive && NULL != negative);

    // PRE: positive is positive and negative is negative
    Int32 iPos = positive->GetFirstNonzeroDigit();
    Int32 iNeg = negative->GetFirstNonzeroDigit();

    // Look if the trailing zeros of the negative will "blank" all
    // the positive digits
    if (iNeg >= positive->mNumberLength) {
        return CBigInteger::ZERO;
    }

    Int32 resLength = positive->mNumberLength;
    AutoPtr< ArrayOf<Int32> > resDigits = ArrayOf<Int32>::Alloc(resLength);

    using Elastos::Core::Math;
    // Must start from max(iPos, iNeg)
    Int32 i = Math::Max(iPos, iNeg);
    if (i == iNeg) {
        (*resDigits)[i] = -negative->GetDigit(i) & positive->GetDigit(i);
        i++;
    }

    Int32 limit = Math::Min(negative->mNumberLength, positive->mNumberLength);
    for ( ; i < limit; i++) {
        (*resDigits)[i] = ~negative->GetDigit(i) & positive->GetDigit(i);
    }
    // if the negative was shorter must copy the remaining digits
    // from positive
    if (i >= negative->mNumberLength) {
        for ( ; i < positive->mNumberLength; i++) {
            (*resDigits)[i] = (*(positive->mDigits))[i];
        }
    } // else positive ended and must "copy" virtual 0's, do nothing then

    AutoPtr<IBigInteger> result;
    CBigInteger::New(1, resLength, *resDigits, (IBigInteger**)&result);
    return result;
}

/** @return sign = -1, magnitude = -(-longer.magnitude & -shorter.magnitude)*/
AutoPtr<IBigInteger> Logical::AndNegative(
    /* [in] */ CBigInteger* longer,
    /* [in] */ CBigInteger* shorter)
{
    assert(NULL != longer && NULL != shorter);

    // PRE: longer and shorter are negative
    // PRE: longer has at least as many digits as shorter
    Int32 iLonger = longer->GetFirstNonzeroDigit();
    Int32 iShorter = shorter->GetFirstNonzeroDigit();

    // Does shorter matter?
    if (iLonger >= shorter->mNumberLength) {
        return (IBigInteger *)longer->Probe(EIID_IBigInteger);
    }

    AutoPtr<IBigInteger> result;
    Int32 resLength;
    AutoPtr< ArrayOf<Int32> > resDigits = NULL;
    Int32 i = Elastos::Core::Math::Max(iShorter, iLonger);
    Int32 digit;
    Int32 m = shorter->GetDigit(i);
    Int32 n = longer->GetDigit(i);
    if (iShorter > iLonger) {
        digit = -m & ~n;
    }
    else if (iShorter < iLonger) {
        digit = ~m & -n;
    }
    else {
        digit = -m & -n;
    }
    if (digit == 0) {
        for (i++;
                i < shorter->mNumberLength
                && (digit = ~(longer->GetDigit(i) | shorter->GetDigit(i))) == 0;
                i++)
            ;  // digit = ~longer->digits[i] & ~shorter->digits[i]

        if (digit == 0) {
            // shorter has only the remaining virtual sign bits
            for ( ;
                    i < longer->mNumberLength
                    && (digit = ~longer->GetDigit(i)) == 0;
                    i++)
                ;
            if (digit == 0) {
                resLength = longer->mNumberLength + 1;
                resDigits = ArrayOf<Int32>::Alloc(resLength);
                (*resDigits)[resLength - 1] = 1;

                CBigInteger::New(-1, resLength, *resDigits, (IBigInteger**)&result);
                return result;
            }
        }
    }

    resLength = longer->mNumberLength;
    resDigits = ArrayOf<Int32>::Alloc(resLength);
    (*resDigits)[i] = -digit;
    for (i++; i < shorter->mNumberLength; i++){
        // (*resDigits)[i] = ~(~longer->digits[i] & ~shorter->digits[i];)
        (*resDigits)[i] = longer->GetDigit(i) | shorter->GetDigit(i);
    }
    // shorter has only the remaining virtual sign bits
    for ( ; i < longer->mNumberLength; i++){
        (*resDigits)[i] = longer->GetDigit(i);
    }

    CBigInteger::New(1, resLength, *resDigits, (IBigInteger**)&result);
    return result;
}

/** @see BigInteger#andNot(BigInteger) */
AutoPtr<IBigInteger> Logical::AndNot(
    /* [in] */ CBigInteger* val,
    /* [in] */ CBigInteger* that)
{
    assert(NULL != val && NULL != that);

    AutoPtr<IBigInteger> result;
    if (that->mSign == 0 ) {
        result = (IBigInteger*)val;
        return result;
    }
    if (val->mSign == 0) {
        return CBigInteger::ZERO;
    }

    Boolean isEqual = FALSE;
    val->Equals(&(*CBigInteger::MINUS_ONE), &isEqual);
    if (isEqual) {
        that->Not((IBigInteger**)&result);
        return result;
    }

    that->Equals(&(*CBigInteger::MINUS_ONE), &isEqual);
    if (isEqual){
        return CBigInteger::ZERO;
    }

    //if val == that, return 0

   if (val->mSign > 0) {
        if (that->mSign > 0) {
            result = AndNotPositive(val, that);
        }
        else {
            result = AndNotPositiveNegative(val, that);
        }
   }
   else {
        if (that->mSign > 0) {
            result = AndNotNegativePositive(val, that);
        }
        else  {
            result = AndNotNegative(val, that);
        }
    }

    return result;
}

/** @return sign = 1, magnitude = val.magnitude & ~that.magnitude*/
AutoPtr<IBigInteger> Logical::AndNotPositive(
    /* [in] */ CBigInteger* val,
    /* [in] */ CBigInteger* that)
{
    assert(NULL != val && NULL != that);

    // PRE: both arguments are positive
    AutoPtr< ArrayOf<Int32> > resDigits = ArrayOf<Int32>::Alloc(val->mNumberLength);

    using Elastos::Core::Math;
    Int32 limit = Math::Min(val->mNumberLength, that->mNumberLength);
    Int32 i;
    for (i = val->GetFirstNonzeroDigit(); i < limit; i++) {
        (*resDigits)[i] = val->GetDigit(i) & ~that->GetDigit(i);
    }
    for ( ; i < val->mNumberLength; i++) {
        (*resDigits)[i] = val->GetDigit(i);
    }

    AutoPtr<IBigInteger> result;
    CBigInteger::New(1, val->mNumberLength, *resDigits, (IBigInteger**)&result);
    return result;
}

/** @return sign = 1, magnitude = positive.magnitude & ~(-negative.magnitude)*/
AutoPtr<IBigInteger> Logical::AndNotPositiveNegative(
    /* [in] */ CBigInteger* positive,
    /* [in] */ CBigInteger* negative)
{
    assert(NULL != positive && NULL != negative);

    AutoPtr<IBigInteger> result;

    // PRE: positive > 0 && negative < 0
    Int32 iNeg = negative->GetFirstNonzeroDigit();
    Int32 iPos = positive->GetFirstNonzeroDigit();

    if (iNeg >= positive->mNumberLength) {
        result = (IBigInteger*)positive;
        return result;
    }

    using Elastos::Core::Math;
    Int32 resLength = Math::Min(positive->mNumberLength, negative->mNumberLength);
    AutoPtr< ArrayOf<Int32> > resDigits = ArrayOf<Int32>::Alloc(resLength);

    // Always start from first non zero of positive
    Int32 i = iPos;
    for ( ; i < iNeg; i++) {
        // (*resDigits)[i] = positive->GetDigit(i) & -1 (~0)
        (*resDigits)[i] = positive->GetDigit(i);
    }
    if (i == iNeg) {
        (*resDigits)[i] = positive->GetDigit(i) & (negative->GetDigit(i) - 1);
        i++;
    }
    for ( ; i < resLength; i++) {
        // (*resDigits)[i] = positive->GetDigit(i) & ~(~negative->GetDigit(i));
        (*resDigits)[i] = positive->GetDigit(i) & negative->GetDigit(i);
    }

    CBigInteger::New(1, resLength, *resDigits, (IBigInteger**)&result);
    return result;
}

/** @return sign = -1, magnitude = -(-negative.magnitude & ~positive.magnitude)*/
AutoPtr<IBigInteger> Logical::AndNotNegativePositive(
    /* [in] */ CBigInteger* negative,
    /* [in] */ CBigInteger* positive)
{
    assert(NULL != positive && NULL != negative);

    AutoPtr<IBigInteger> result;

    // PRE: negative < 0 && positive > 0
    Int32 resLength;
    AutoPtr< ArrayOf<Int32> > resDigits;
    Int32 limit;
    Int32 digit;

    Int32 iNeg = negative->GetFirstNonzeroDigit();
    Int32 iPos = positive->GetFirstNonzeroDigit();

    if (iNeg >= positive->mNumberLength) {
        result = (IBigInteger*)negative;
        return result;
    }

    using Elastos::Core::Math;
    resLength = Math::Max(negative->mNumberLength, positive->mNumberLength);
    Int32 i = iNeg;
    if (iPos > iNeg) {
        resDigits = ArrayOf<Int32>::Alloc(resLength);
        limit = Math::Min(negative->mNumberLength, iPos);
        for ( ; i < limit; i++) {
            // 1st case:  resDigits [i] = -(-negative->GetDigit(i) & (~0))
            // otherwise: (*resDigits)[i] = ~(~negative->GetDigit(i) & ~0)  ;
            (*resDigits)[i] = negative->GetDigit(i);
        }
        if (i == negative->mNumberLength) {
            for (i = iPos; i < positive->mNumberLength; i++) {
                // (*resDigits)[i] = ~(~positive->GetDigit(i) & -1);
                (*resDigits)[i] = positive->GetDigit(i);
            }
        }
    } else {
        digit = -negative->GetDigit(i) & ~positive->GetDigit(i);
        if (digit == 0) {
            limit = Math::Min(positive->mNumberLength, negative->mNumberLength);
            for (i++; i < limit && (digit = ~(negative->GetDigit(i) | positive->GetDigit(i))) == 0; i++)
                ; // digit = ~negative->GetDigit(i) & ~positive->GetDigit(i)
            if (digit == 0) {
                // the shorter has only the remaining virtual sign bits
                for ( ; i < positive->mNumberLength && (digit = ~positive->GetDigit(i)) == 0; i++)
                    ; // digit = -1 & ~positive->GetDigit(i)
                for ( ; i < negative->mNumberLength && (digit = ~negative->GetDigit(i)) == 0; i++)
                    ; // digit = ~negative->GetDigit(i) & ~0
                if (digit == 0) {
                    resLength++;
                    resDigits = ArrayOf<Int32>::Alloc(resLength);
                    (*resDigits)[resLength - 1] = 1;

                    CBigInteger::New(-1, resLength, *resDigits, (IBigInteger**)&result);
                    return result;
                }
            }
        }
        resDigits = ArrayOf<Int32>::Alloc(resLength);
        (*resDigits)[i] = -digit;
        i++;
    }

    limit = Math::Min(positive->mNumberLength, negative->mNumberLength);
    for ( ; i < limit; i++) {
        //(*resDigits)[i] = ~(~negative->GetDigit(i) & ~positive->GetDigit(i));
        (*resDigits)[i] = negative->GetDigit(i) | positive->GetDigit(i);
    }
    // Actually one of the next two cycles will be executed
    for ( ; i < negative->mNumberLength; i++) {
        (*resDigits)[i] = negative->GetDigit(i);
            }
    for ( ; i < positive->mNumberLength; i++) {
        (*resDigits)[i] = positive->GetDigit(i);
    }

    CBigInteger::New(-1, resLength, *resDigits, (IBigInteger**)&result);
    return result;
}

/** @return sign = 1, magnitude = -val.magnitude & ~(-that.magnitude)*/
AutoPtr<IBigInteger> Logical::AndNotNegative(
    /* [in] */ CBigInteger* val,
    /* [in] */ CBigInteger* that)
{
    assert(NULL != val && NULL != that);

    using Elastos::Core::Math;

    // PRE: val < 0 && that < 0
    Int32 iVal = val->GetFirstNonzeroDigit();
    Int32 iThat = that->GetFirstNonzeroDigit();

    if (iVal >= that->mNumberLength) {
        return CBigInteger::ZERO;
    }

    Int32 resLength = that->mNumberLength;
    AutoPtr< ArrayOf<Int32> > resDigits = ArrayOf<Int32>::Alloc(resLength);
    Int32 limit;
    Int32 i = iVal;
    if (iVal < iThat) {
        // (*resDigits)[i] = -val->GetDigit(i) & -1;
        (*resDigits)[i] = -val->GetDigit(i);
        limit = Math::Min(val->mNumberLength, iThat);
        for (i++; i < limit; i++) {
            // (*resDigits)[i] = ~val->GetDigit(i) & -1;
            (*resDigits)[i] = ~val->GetDigit(i);
        }
        if (i == val->mNumberLength) {
            for ( ; i < iThat; i++) {
                // (*resDigits)[i] = -1 & -1;
                (*resDigits)[i] = -1;
            }
            // (*resDigits)[i] = -1 & ~-that->GetDigit(i);
            (*resDigits)[i] = that->GetDigit(i) - 1;
        } else {
            // (*resDigits)[i] = ~val->GetDigit(i) & ~-that->GetDigit(i);
            (*resDigits)[i] = ~val->GetDigit(i) & (that->GetDigit(i) - 1);
        }
    } else if (iThat < iVal ) {
        // (*resDigits)[i] = -val->GetDigit(i) & ~~that->GetDigit(i);
        (*resDigits)[i] = -val->GetDigit(i) & that->GetDigit(i);
    } else {
        // (*resDigits)[i] = -val->GetDigit(i) & ~-that->GetDigit(i);
        (*resDigits)[i] = -val->GetDigit(i) & (that->GetDigit(i) - 1);
    }

    limit = Math::Min(val->mNumberLength, that->mNumberLength);
    for (i++; i < limit; i++) {
        // (*resDigits)[i] = ~val->GetDigit(i) & ~~that->GetDigit(i);
        (*resDigits)[i] = ~val->GetDigit(i) & that->GetDigit(i);
    }
    for ( ; i < that->mNumberLength; i++) {
        // (*resDigits)[i] = -1 & ~~that->GetDigit(i);
        (*resDigits)[i] = that->GetDigit(i);
    }

    AutoPtr<IBigInteger> result;
    CBigInteger::New(1, resLength, *resDigits, (IBigInteger**)&result);
    return result;
}

/** @see BigInteger#or(BigInteger) */
AutoPtr<IBigInteger> Logical::Or(
    /* [in] */ CBigInteger* val,
    /* [in] */ CBigInteger* that)
{
    assert(NULL != val && NULL != that);

    Boolean isThatEqual = FALSE, isValEqual = FALSE;
    that->Equals(&(*CBigInteger::MINUS_ONE), &isThatEqual);
    val->Equals(&(*CBigInteger::MINUS_ONE), &isValEqual);
    if (isThatEqual || isValEqual) {
        return CBigInteger::MINUS_ONE;
    }

    AutoPtr<IBigInteger> result;
    if (that->mSign == 0) {
        result = (IBigInteger*)val;
        return result;
    }
    if (val->mSign == 0) {
        result = (IBigInteger*)that;
        return result;
    }

    if (val->mSign > 0) {
        if (that->mSign > 0) {
            if (val->mNumberLength > that->mNumberLength) {
                return OrPositive(val, that);
            }
            else {
                return OrPositive(that, val);
            }
        }
        else {
            return OrDiffSigns(val, that);
        }
    }
    else {
        if (that->mSign > 0) {
            return OrDiffSigns(that, val);
        }
        else if (that->GetFirstNonzeroDigit() > val->GetFirstNonzeroDigit()) {
            return OrNegative(that, val);
        }
        else {
            return OrNegative(val, that);
        }
    }

    return result;
}

/** @return sign = 1, magnitude = longer.magnitude | shorter.magnitude*/
AutoPtr<IBigInteger> Logical::OrPositive(
    /* [in] */ CBigInteger* longer,
    /* [in] */ CBigInteger* shorter)
{
    assert(NULL != longer && NULL != shorter);

    // PRE: longer and shorter are positive;
    // PRE: longer has at least as many digits as shorter
    Int32 resLength = longer->mNumberLength;
    AutoPtr< ArrayOf<Int32> > resDigits = ArrayOf<Int32>::Alloc(resLength);

    Int32 i;
    for (i = 0; i < shorter->mNumberLength; i++) {
        (*resDigits)[i] = longer->GetDigit(i) | shorter->GetDigit(i);
    }
    for ( ; i < resLength; i++) {
        (*resDigits)[i] = longer->GetDigit(i);
    }

    AutoPtr<IBigInteger> result;
    CBigInteger::New(1, resLength, *resDigits, (IBigInteger**)&result);
    return result;
}

/** @return sign = -1, magnitude = -(-val.magnitude | -that.magnitude) */
AutoPtr<IBigInteger> Logical::OrNegative(
    /* [in] */ CBigInteger* val,
    /* [in] */ CBigInteger* that)
{
    assert(NULL != val && NULL != that);

    using Elastos::Core::Math;

    AutoPtr<IBigInteger> result;

    // PRE: val and that are negative;
    // PRE: val has at least as many trailing zeros digits as that
    Int32 iThat = that->GetFirstNonzeroDigit();
    Int32 iVal = val->GetFirstNonzeroDigit();
    Int32 i;

    if (iVal >= that->mNumberLength) {
        result = (IBigInteger*)that;
        return result;
    }
    else if (iThat >= val->mNumberLength) {
        result = (IBigInteger*)val;
        return result;
    }

    Int32 resLength = Math::Min(val->mNumberLength, that->mNumberLength);
    AutoPtr< ArrayOf<Int32> > resDigits = ArrayOf<Int32>::Alloc(resLength);

    //Looking for the first non-zero digit of the result
    if (iThat == iVal) {
        (*resDigits)[iVal] = -(-val->GetDigit(iVal) | -that->GetDigit(iVal));
        i = iVal;
    }
    else {
        for (i = iThat; i < iVal; i++) {
            (*resDigits)[i] = that->GetDigit(i);
        }
        (*resDigits)[i] = that->GetDigit(i) & (val->GetDigit(i) - 1);
    }

    for (i++; i < resLength; i++) {
        (*resDigits)[i] = val->GetDigit(i) & that->GetDigit(i);
    }

    CBigInteger::New(-1, resLength, *resDigits, (IBigInteger**)&result);
    return result;
}

/** @return sign = -1, magnitude = -(positive.magnitude | -negative.magnitude) */
AutoPtr<IBigInteger> Logical::OrDiffSigns(
    /* [in] */ CBigInteger* positive,
    /* [in] */ CBigInteger* negative)
{
    assert(NULL != positive && NULL != negative);

    AutoPtr<IBigInteger> result;
    using Elastos::Core::Math;

    // Jumping over the least significant zero bits
    Int32 iNeg = negative->GetFirstNonzeroDigit();
    Int32 iPos = positive->GetFirstNonzeroDigit();
    Int32 i;
    Int32 limit;

    // Look if the trailing zeros of the positive will "copy" all
    // the negative digits
    if (iPos >= negative->mNumberLength) {
        result = (IBigInteger*)negative;
        return result;
    }
    Int32 resLength = negative->mNumberLength;
    AutoPtr< ArrayOf<Int32> > resDigits = ArrayOf<Int32>::Alloc(resLength);

    if (iNeg < iPos ) {
        // We know for sure that this will
        // be the first non zero digit in the result
        for (i = iNeg; i < iPos; i++) {
            (*resDigits)[i] = negative->GetDigit(i);
        }
    }
    else if (iPos < iNeg) {
        i = iPos;
        (*resDigits)[i] = -positive->GetDigit(i);
        limit = Math::Min(positive->mNumberLength, iNeg);
        for (i++; i < limit; i++ ) {
            (*resDigits)[i] = ~positive->GetDigit(i);
        }
        if (i != positive->mNumberLength) {
            (*resDigits)[i] = ~(-negative->GetDigit(i) | positive->GetDigit(i));
        }
        else {
            for (; i<iNeg; i++) {
                (*resDigits)[i] = -1;
            }
            // (*resDigits)[i] = ~(-negative->GetDigit(i) | 0);
            (*resDigits)[i] = negative->GetDigit(i) - 1;
        }
        i++;
    }
    else {// iNeg == iPos
        // Applying two complement to negative and to result
        i = iPos;
        (*resDigits)[i] = -(-negative->GetDigit(i) | positive->GetDigit(i));
        i++;
    }
    limit = Math::Min(negative->mNumberLength, positive->mNumberLength);
    for (; i < limit; i++) {
        // Applying two complement to negative and to result
        // (*resDigits)[i] = ~(~negative->GetDigit(i) | positive->GetDigit(i) );
        (*resDigits)[i] = negative->GetDigit(i) & ~positive->GetDigit(i);
    }
    for ( ; i < negative->mNumberLength; i++) {
        (*resDigits)[i] = negative->GetDigit(i);
    }

    CBigInteger::New(-1, resLength, *resDigits, (IBigInteger**)&result);
    return result;
}

/** @see BigInteger#xor(BigInteger) */
AutoPtr<IBigInteger> Logical::Xor(
    /* [in] */ CBigInteger* val,
    /* [in] */ CBigInteger* that)
{
    assert(NULL != val && NULL != that);

    AutoPtr<IBigInteger> result;

    if (that->mSign == 0) {
        result = (IBigInteger*)val;
        return result;
    }
    if (val->mSign == 0) {
        result = (IBigInteger*)that;
        return result;
    }

    Boolean isEqual = FALSE;
    that->Equals(&(*CBigInteger::MINUS_ONE), &isEqual);
    if (isEqual) {
        val->Not((IBigInteger**)&result);
        return result;
    }

    val->Equals(&(*CBigInteger::MINUS_ONE), &isEqual);
    if (isEqual) {
        that->Not((IBigInteger**)&result);
        return result;
    }

    if (val->mSign > 0) {
        if (that->mSign > 0) {
            if (val->mNumberLength > that->mNumberLength) {
                return XorPositive(val, that);
            }
            else {
                return XorPositive(that, val);
            }
        }
        else {
            return XorDiffSigns(val, that);
        }
    }
    else {
        if (that->mSign > 0) {
            return XorDiffSigns(that, val);
        }
        else if (that->GetFirstNonzeroDigit() > val->GetFirstNonzeroDigit()) {
            return XorNegative(that, val);
        }
        else {
            return XorNegative(val, that);
        }
    }

    return result;
}

/** @return sign = 0, magnitude = longer.magnitude | shorter.magnitude */
AutoPtr<IBigInteger> Logical::XorPositive(
    /* [in] */ CBigInteger* longer,
    /* [in] */ CBigInteger* shorter)
{
    assert(NULL != longer && NULL != shorter);

    using Elastos::Core::Math;

    // PRE: longer and shorter are positive;
    // PRE: longer has at least as many digits as shorter
    Int32 resLength = longer->mNumberLength;
    AutoPtr< ArrayOf<Int32> > resDigits = ArrayOf<Int32>::Alloc(resLength);
    Int32 i = Math::Min(longer->GetFirstNonzeroDigit(), shorter->GetFirstNonzeroDigit());
    for ( ; i < shorter->mNumberLength; i++) {
        (*resDigits)[i] = longer->GetDigit(i) ^ shorter->GetDigit(i);
    }
    for ( ; i < longer->mNumberLength; i++ ){
        (*resDigits)[i] = longer->GetDigit(i);
    }

    AutoPtr<IBigInteger> result;
    CBigInteger::New(1, resLength, *resDigits, (IBigInteger**)&result);
    return result;
}

/** @return sign = 0, magnitude = -val.magnitude ^ -that.magnitude */
AutoPtr<IBigInteger> Logical::XorNegative(
    /* [in] */ CBigInteger* val,
    /* [in] */ CBigInteger* that)
{
    assert(NULL != val && NULL != that);

    using Elastos::Core::Math;

    // PRE: val and that are negative
    // PRE: val has at least as many trailing zero digits as that
    Int32 resLength = Math::Max(val->mNumberLength, that->mNumberLength);
    AutoPtr< ArrayOf<Int32> > resDigits = ArrayOf<Int32>::Alloc(resLength);
    Int32 iVal = val->GetFirstNonzeroDigit();
    Int32 iThat = that->GetFirstNonzeroDigit();
    Int32 i = iThat;
    Int32 limit;

    if (iVal == iThat) {
        (*resDigits)[i] = -val->GetDigit(i) ^ -that->GetDigit(i);
    }
    else {
        (*resDigits)[i] = -that->GetDigit(i);
        limit = Math::Min(that->mNumberLength, iVal);
        for (i++; i < limit; i++) {
            (*resDigits)[i] = ~that->GetDigit(i);
        }
        // Remains digits in that?
        if (i == that->mNumberLength) {
            //Jumping over the remaining zero to the first non one
            for ( ;i < iVal; i++) {
                //(*resDigits)[i] = 0 ^ -1;
                (*resDigits)[i] = -1;
            }
            //(*resDigits)[i] = -val->GetDigit(i) ^ -1;
            (*resDigits)[i] = val->GetDigit(i) - 1;
        }
        else {
            (*resDigits)[i] = -val->GetDigit(i) ^ ~that->GetDigit(i);
        }
    }

    limit = Math::Min(val->mNumberLength, that->mNumberLength);
    //Perform ^ between that al val until that ends
    for (i++; i < limit; i++) {
        //(*resDigits)[i] = ~val->GetDigit(i) ^ ~that->GetDigit(i);
        (*resDigits)[i] = val->GetDigit(i) ^ that->GetDigit(i);
    }
    //Perform ^ between val digits and -1 until val ends
    for ( ; i < val->mNumberLength; i++) {
        //(*resDigits)[i] = ~val->GetDigit(i) ^ -1  ;
        (*resDigits)[i] = val->GetDigit(i) ;
    }
    for ( ; i < that->mNumberLength; i++) {
        //(*resDigits)[i] = -1 ^ ~that->GetDigit(i) ;
        (*resDigits)[i] = that->GetDigit(i);
    }

    AutoPtr<IBigInteger> result;
    CBigInteger::New(1, resLength, *resDigits, (IBigInteger**)&result);
    return result;
}

/** @return sign = 1, magnitude = -(positive.magnitude ^ -negative.magnitude)*/
AutoPtr<IBigInteger> Logical::XorDiffSigns(
    /* [in] */ CBigInteger* positive,
    /* [in] */ CBigInteger* negative)
{
    assert(NULL != positive && NULL != negative);

    AutoPtr<IBigInteger> result;
    using Elastos::Core::Math;

    Int32 resLength = Math::Max(negative->mNumberLength, positive->mNumberLength);
    AutoPtr< ArrayOf<Int32> > resDigits = NULL;
    Int32 iNeg = negative->GetFirstNonzeroDigit();
    Int32 iPos = positive->GetFirstNonzeroDigit();
    Int32 i;
    Int32 limit;

    //The first
    if (iNeg < iPos) {
        resDigits = ArrayOf<Int32>::Alloc(resLength);
        i = iNeg;
        //(*resDigits)[i] = -(-negative->GetDigit(i));
        (*resDigits)[i] = negative->GetDigit(i);
        limit = Math::Min(negative->mNumberLength, iPos);
        //Skip the positive digits while they are zeros
        for (i++; i < limit; i++) {
            //(*resDigits)[i] = ~(~negative->GetDigit(i));
            (*resDigits)[i] = negative->GetDigit(i);
        }
        //if the negative has no more elements, must fill the
        //result with the remaining digits of the positive
        if (i == negative->mNumberLength) {
            for ( ; i < positive->mNumberLength; i++) {
                //(*resDigits)[i] = ~(positive->GetDigit(i) ^ -1) -> ~(~positive->GetDigit(i))
                (*resDigits)[i] = positive->GetDigit(i);
            }
        }
    }
    else if (iPos < iNeg) {
        resDigits = ArrayOf<Int32>::Alloc(resLength);
        i = iPos;
        //Applying two complement to the first non-zero digit of the result
        (*resDigits)[i] = -positive->GetDigit(i);
        limit = Math::Min(positive->mNumberLength, iNeg);
        for (i++; i < limit; i++) {
            //Continue applying two complement the result
            (*resDigits)[i] = ~positive->GetDigit(i);
        }
        //When the first non-zero digit of the negative is reached, must apply
        //two complement (arithmetic negation) to it, and then operate
        if (i == iNeg) {
            (*resDigits)[i] = ~(positive->GetDigit(i) ^ -negative->GetDigit(i));
            i++;
        }
        else {
            //if the positive has no more elements must fill the remaining digits with
            //the negative ones
            for ( ; i < iNeg; i++) {
                // (*resDigits)[i] = ~(0 ^ 0)
                (*resDigits)[i] = -1;
            }
            for ( ; i < negative->mNumberLength; i++) {
                //(*resDigits)[i] = ~(~negative->GetDigit(i) ^ 0)
                (*resDigits)[i] = negative->GetDigit(i);
            }
        }
    }
    else {
        //The first non-zero digit of the positive and negative are the same
        i = iNeg;
        Int32 digit = positive->GetDigit(i) ^ -negative->GetDigit(i);
        if (digit == 0) {
            limit = Math::Min(positive->mNumberLength, negative->mNumberLength);
            for (i++; i < limit && (digit = positive->GetDigit(i) ^ ~negative->GetDigit(i)) == 0; i++)
                ;
            if (digit == 0) {
                // shorter has only the remaining virtual sign bits
                for ( ; i < positive->mNumberLength && (digit = ~positive->GetDigit(i)) == 0; i++)
                    ;
                for ( ; i < negative->mNumberLength && (digit = ~negative->GetDigit(i)) == 0; i++)
                    ;
                if (digit == 0) {
                    resLength = resLength + 1;
                    resDigits = ArrayOf<Int32>::Alloc(resLength);
                    (*resDigits)[resLength - 1] = 1;

                    CBigInteger::New(-1, resLength, *resDigits, (IBigInteger**)&result);
                    return result;
                }
            }
        }
        resDigits = ArrayOf<Int32>::Alloc(resLength);
        (*resDigits)[i] = -digit;
        i++;
    }

    limit = Math::Min(negative->mNumberLength, positive->mNumberLength);
    for ( ; i < limit; i++) {
        (*resDigits)[i] = ~(~negative->GetDigit(i) ^ positive->GetDigit(i));
    }
    for ( ; i < positive->mNumberLength; i++) {
        // (*resDigits)[i] = ~(positive->GetDigit(i) ^ -1)
        (*resDigits)[i] = positive->GetDigit(i);
    }
    for ( ; i < negative->mNumberLength; i++) {
        // (*resDigits)[i] = ~(0 ^ ~negative->GetDigit(i))
        (*resDigits)[i] = negative->GetDigit(i);
    }

    CBigInteger::New(-1, resLength, *resDigits, (IBigInteger**)&result);
    return result;
}

} // namespace Math
} // namespace Elastos
