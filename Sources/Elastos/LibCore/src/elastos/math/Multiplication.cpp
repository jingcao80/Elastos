
#include "Multiplication.h"
#include "Math.h"

namespace Elastos {
namespace Math {

Multiplication::StaticInitializer::StaticInitializer()
{
    Int32 i;
    Int64 fivePow = 1LL;
    for (i = 0; i <= 18; i++) {
        AutoPtr<IBigInteger> bi;
        CBigInteger::ValueOf(fivePow, (IBigInteger**)&bi);
        sBigFivePows->Set(i, bi);
        CBigInteger::ValueOf(fivePow << i, (IBigInteger**)&bi);
        sBigTenPows->Set(i, bi);
        fivePow *= 5;
    }
    for (; i < sBigTenPows->GetLength(); i++) {
        AutoPtr<IBigInteger> fivePowTemp, tenPowTemp;
        (*sBigFivePows)[i - 1]->Multiply((*sBigFivePows)[1], (IBigInteger**)&fivePowTemp);
        sBigFivePows->Set(i, fivePowTemp);
        (*sBigTenPows)[i - 1]->Multiply(CBigInteger::TEN, (IBigInteger**)&tenPowTemp);
        sBigTenPows->Set(i, tenPowTemp);
    }
}

const Int32 Multiplication::sTenPows[] = {
    1, 10, 100, 1000, 10000, 100000, 1000000,
    10000000, 100000000, 1000000000
};

const Int32 Multiplication::sFivePows[] = {
    1, 5, 25, 125, 625, 3125, 15625, 78125, 390625,
    1953125, 9765625, 48828125, 244140625, 1220703125
};

INIT_PROI_3 AutoPtr<ArrayOf<IBigInteger*> > Multiplication::sBigTenPows = ArrayOf<IBigInteger*>::Alloc(32);
INIT_PROI_3 AutoPtr<ArrayOf<IBigInteger*> > Multiplication::sBigFivePows = ArrayOf<IBigInteger*>::Alloc(32);
INIT_PROI_3 Multiplication::StaticInitializer Multiplication::sInitializer;

ECode Multiplication::MultiplyByPositiveInt(
    /* [in] */ IBigInteger* val,
    /* [in] */ Int32 factor,
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(val);
    VALIDATE_NOT_NULL(result);

    AutoPtr<BigInt> bi = new BigInt();
    ((CBigInteger*)val)->GetBigInt()->Copy(*bi);
    FAIL_RETURN(bi->MultiplyByPositiveInt(factor));
    return CBigInteger::NewFromBigInt(bi, result);
}

ECode Multiplication::MultiplyByTenPow(
    /* [in] */ IBigInteger* val,
    /* [in] */ Int64 exp,
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(val);
    VALIDATE_NOT_NULL(result);

    if (exp < sTenPowsLength) {
        return MultiplyByPositiveInt(val, sTenPows[(Int32)exp], result);
    }
    else {
        AutoPtr<IBigInteger> powerBi;
        FAIL_RETURN(PowerOf10(exp, (IBigInteger**)&powerBi));
        return val->Multiply(powerBi, result);
    }
}

ECode Multiplication::PowerOf10(
    /* [in] */ Int64 exp,
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(result);

    // PRE: exp >= 0
    Int32 intExp = (Int32)exp;
    // "SMALL POWERS"
    if (exp < sBigTenPows->GetLength()) {
        // The largest power that fit in 'Int64' type
        *result =  (*sBigTenPows)[intExp];
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
    else if (exp <= 50) {
        // To calculate:    10^exp
        return CBigInteger::TEN->Pow(intExp, result);
    }

    using Elastos::Core::Math;

    AutoPtr<IBigInteger> res = NULL;
    //try {
    // "LARGE POWERS"
    if (exp <= Math::INT32_MAX_VALUE) {
        // To calculate:    5^exp * 2^exp
        AutoPtr<IBigInteger> temp;
        FAIL_RETURN((*sBigFivePows)[1]->Pow(intExp, (IBigInteger**)&temp));
        FAIL_RETURN(temp->ShiftLeft(intExp, (IBigInteger**)&res));
    } else {
        /*
         * "HUGE POWERS"
         *
         * This branch probably won't be executed since the power of ten is too
         * big.
         */
        // To calculate:    5^exp
        AutoPtr<IBigInteger> powerOfFive = NULL;
        FAIL_RETURN((*sBigFivePows)[1]->Pow(Math::INT32_MAX_VALUE, (IBigInteger**)&powerOfFive));
        res = powerOfFive;
        Int64 longExp = exp - Math::INT32_MAX_VALUE;

        intExp = (Int32) (exp % Math::INT32_MAX_VALUE);
        while (longExp > Math::INT32_MAX_VALUE) {
            AutoPtr<IBigInteger> temp;
            FAIL_RETURN(res->Multiply(powerOfFive, (IBigInteger**)&temp));
            res = temp;
            longExp -= Math::INT32_MAX_VALUE;
        }

        AutoPtr<IBigInteger> temp1, temp2;
        FAIL_RETURN((*sBigFivePows)[1]->Pow(intExp, (IBigInteger**)&temp1));
        FAIL_RETURN(res->Multiply(temp1, (IBigInteger**)&temp2));
        res = temp2;
        temp1 = NULL, temp2 = NULL;

        // To calculate:    5^exp << exp
        FAIL_RETURN(res->ShiftLeft(Math::INT32_MAX_VALUE, (IBigInteger**)&temp1));
        res = temp1;
        temp1 = NULL;

        longExp = exp - Math::INT32_MAX_VALUE;
        while (longExp > Math::INT32_MAX_VALUE) {
            AutoPtr<IBigInteger> temp;
            FAIL_RETURN(res->ShiftLeft(Math::INT32_MAX_VALUE, (IBigInteger**)&temp));
            res = temp;
            longExp -= Math::INT32_MAX_VALUE;
        }

        FAIL_RETURN(res->ShiftLeft(intExp, (IBigInteger**)&temp1));
        res = temp1;
        temp1 = NULL;
    }
    //} catch (OutOfMemoryError error) {
    //    throw new ArithmeticException(error.getMessage());
    //}

    *result = res;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode Multiplication::MultiplyByFivePow(
    /* [in] */ IBigInteger* val,
    /* [in] */ Int32 exp,
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(val);
    VALIDATE_NOT_NULL(result);

    // PRE: exp >= 0
    if (exp < sFivePowsLength) {
        return MultiplyByPositiveInt(val, sFivePows[exp], result);
    }
    else if (exp < sBigFivePows->GetLength()) {
        return val->Multiply((*sBigFivePows)[exp], result);
    }
    else {// Large powers of five
        AutoPtr<IBigInteger> powerBi = NULL;
        FAIL_RETURN((*sBigFivePows)[1]->Pow(exp, (IBigInteger**)&powerBi));
        return val->Multiply(powerBi, result);
    }
}

} // namespace Math
} // namespace Elastos
