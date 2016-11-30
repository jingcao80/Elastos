
#include "Elastos.CoreLibrary.Utility.h"
#include "CBigInteger.h"
#include "Conversion.h"
#include "BitLevel.h"
#include "Logical.h"
#include "Primality.h"
#include "Math.h"
#include "Character.h"
#include "StringUtils.h"
#include "AutoLock.h"

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Core::Math;
using Elastos::Core::Character;
using Elastos::Core::StringUtils;
using Elastos::Core::EIID_INumber;
using Elastos::Core::EIID_IComparable;

namespace Elastos {
namespace Math {

static AutoPtr<IBigInteger> CreateBigInteger(Int32 sign, Int32 value)
{
    AutoPtr<IBigInteger> obj;
    CBigInteger::New(sign, value, (IBigInteger**)&obj);
    return obj;
}

INIT_PROI_2 AutoPtr<IBigInteger> CBigInteger::ZERO = CreateBigInteger(0, 0);
INIT_PROI_2 AutoPtr<IBigInteger> CBigInteger::ONE = CreateBigInteger(1, 1);
INIT_PROI_2 AutoPtr<IBigInteger> CBigInteger::TEN = CreateBigInteger(1, 10);
INIT_PROI_2 AutoPtr<IBigInteger> CBigInteger::MINUS_ONE = CreateBigInteger(-1, 1);

CAR_OBJECT_IMPL(CBigInteger)

CAR_INTERFACE_IMPL_3(CBigInteger, Object, IBigInteger, INumber, IComparable)

CBigInteger::CBigInteger()
    : mNativeIsValid(FALSE)
    , mJavaIsValid(FALSE)
    , mNumberLength(0)
    , mSign(0)
    , mFirstNonzeroDigit(-2)
    , mSignum(0)
    , mHashCode(0)
{
}

CBigInteger::~CBigInteger()
{
}

ECode CBigInteger::constructor()
{
    return NOERROR;
}

ECode CBigInteger::constructor(
    /* [in] */ Int32 sign,
    /* [in] */ Int32 numberLength,
    /* [in] */ const ArrayOf<Int32>& digits)
{
    SetJavaRepresentation(sign, numberLength, digits);
    return NOERROR;
}

ECode CBigInteger::constructor(
    /* [in] */ Int32 numBits,
    /* [in] */ IRandom* random)
{
    if (numBits < 0) {
        //throw new IllegalArgumentException("numBits < 0: " + numBits);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (numBits == 0) {
        AutoPtr<ArrayOf<Int32> > temp = ArrayOf<Int32>::Alloc(1);
        (*temp)[0] = 0;
        SetJavaRepresentation(0, 1, *temp);
    } else {
        Int32 sign = 1;
        Int32 numberLength = (numBits + 31) >> 5;
        AutoPtr<ArrayOf<Int32> > digits = ArrayOf<Int32>::Alloc(numberLength);
        for (Int32 i = 0; i < numberLength; i++) {
            random->NextInt32(&((*digits)[i]));
        }
        // Clear any extra bits.
        (*digits)[numberLength - 1] >>= (((-numBits) & 31) & 0x1F);
        SetJavaRepresentation(sign, numberLength, *digits);
    }
    mJavaIsValid = TRUE;
    return NOERROR;
}

ECode CBigInteger::constructor(
    /* [in] */ Int32 bitLength,
    /* [in] */ Int32 certainty,
    /* [in] */ IRandom* random)
{
    if (bitLength < 2) {
        //throw new ArithmeticException("bitLength < 2: " + bitLength);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (bitLength < 16) {
        // We have to generate short primes ourselves, because OpenSSL bottoms out at 16 bits.
        Int32 candidate;
        Int32 temp;
        do {
            random->NextInt32(&temp);
            candidate = temp & ((1 << (bitLength & 0x1F)) - 1);
            candidate |= (1 << ((bitLength - 1) & 0x1F)); // Set top bit.
            if (bitLength > 2) {
                candidate |= 1; // Any prime longer than 2 bits must have the bottom bit set.
            }
        } while (!IsSmallPrime(candidate));
        AutoPtr<BigInt> prime = new BigInt();
        prime->PutULongInt(candidate, FALSE);
        SetBigInt(prime);
    } else {
        // We need a loop here to work around an OpenSSL bug; http://b/8588028.
        Int32 length;
        do {
            AutoPtr<BigInt> prime = new BigInt();
            BigInt::GeneratePrimeDefault(bitLength, *prime);
            SetBigInt(prime);
        } while ((BitLength(&length), length) != bitLength);
    }

    return NOERROR;
}

Boolean CBigInteger::IsSmallPrime(
    /* [in] */ Int32 x)
{
    if (x == 2) {
        return TRUE;
    }
    if ((x % 2) == 0) {
        return FALSE;
    }
    const Int32 max = (Int32) Elastos::Core::Math::Sqrt(x);
    for (Int32 i = 3; i <= max; i += 2) {
        if ((x % i) == 0) {
            return FALSE;
        }
    }
    return TRUE;
}

ECode CBigInteger::constructor(
    /* [in] */ Int32 sign,
    /* [in] */ Int64 value)
{
    AutoPtr<BigInt> bigInt = new BigInt();
    bigInt->PutULongInt(value, (sign < 0));
    SetBigInt(bigInt);
    return NOERROR;
}

ECode CBigInteger::constructor(
    /* [in] */ const String& value)
{
    if (value.IsNull()) {
        return E_NULL_POINTER_EXCEPTION;
    }

    AutoPtr<BigInt> bigInt = new BigInt();
    bigInt->PutDecString(value);
    SetBigInt(bigInt);
    return NOERROR;
}

ECode CBigInteger::constructor(
    /* [in] */ const String& value,
    /* [in] */ Int32 radix)
{
    if (value.IsNull()) {
        return E_NULL_POINTER_EXCEPTION;
    }

    if (radix == 10) {
        AutoPtr<BigInt> bigInt = new BigInt();
        FAIL_RETURN(bigInt->PutDecString(value));
        SetBigInt(bigInt);
    }
    else if (radix == 16) {
        AutoPtr<BigInt> bigInt = new BigInt();
        FAIL_RETURN(bigInt->PutHexString(value));
        SetBigInt(bigInt);
    }
    else {
        if (radix < Character::MIN_RADIX || radix > Character::MAX_RADIX) {
            return E_NUMBER_FORMAT_EXCEPTION;
        }
        if (value.IsEmpty()) {
            return E_NUMBER_FORMAT_EXCEPTION;
        }

        FAIL_RETURN(CBigInteger::ParseFromString(*this, value, radix));
    }

    return NOERROR;
}

ECode CBigInteger::constructor(
    /* [in] */ Int32 signum,
    /* [in] */ const ArrayOf<Byte>& magnitude)
{
    if (signum < -1 || signum > 1) {
        return E_NUMBER_FORMAT_EXCEPTION;
    }
    if (signum == 0) {
        Int32 length = magnitude.GetLength();
        for (Int32 i = 0; i < length; ++i) {
            if (magnitude[i] != 0) {
                return E_NUMBER_FORMAT_EXCEPTION;
            }
        }
    }

    AutoPtr<BigInt> bigInt = new BigInt();
    bigInt->PutBigEndian(magnitude, signum < 0);
    SetBigInt(bigInt);

    return NOERROR;
}

ECode CBigInteger::constructor(
    /* [in] */ const ArrayOf<Byte>& value)
{
    Int32 length = value.GetLength();
    if (length == 0) {
        return E_NUMBER_FORMAT_EXCEPTION;
    }
    AutoPtr<BigInt> bigInt = new BigInt();
    bigInt->PutBigEndianTwosComplement(value);
    SetBigInt(bigInt);

    return NOERROR;
}

Int32 CBigInteger::GetDigit(
    /* [in] */ Int32 index)
{
    assert(index >= 0 && NULL != mDigits && index < mDigits->GetLength());
    return (*mDigits)[index];
}

void CBigInteger::SetBigInt(
    /* [in] */ BigInt* bigInt)
{
    mBigInt = bigInt;
    mNativeIsValid = TRUE;
}

AutoPtr<BigInt> CBigInteger::GetBigInt()
{
    if (mNativeIsValid) {
        return mBigInt;
    }

    {    AutoLock syncLock(this);
        if (mNativeIsValid) {
            return mBigInt;
        }

        assert(mDigits != NULL);

        AutoPtr<BigInt> bigInt = new BigInt();
        bigInt->PutLittleEndianInts(*mDigits, (mSign < 0));
        SetBigInt(bigInt);
        return bigInt;
    }
    return NOERROR;
}

void CBigInteger::SetJavaRepresentation(
    /* [in] */ Int32 sign,
    /* [in] */ Int32 numberLength,
    /* [in] */ const ArrayOf<Int32>& digits)
{
    // decrement numberLength to drop leading zeroes...
    while (numberLength > 0 && digits[--numberLength] == 0) {
        ;
    }
    // ... and then increment it back because we always drop one too many
    if (digits[numberLength++] == 0) {
        sign = 0;
    }

    if (mDigits != NULL) {
        mDigits = NULL;
    }

    mSign = sign;
    mDigits = digits.Clone();  // TODO: use copy
    mNumberLength = numberLength;
    mJavaIsValid = TRUE;
}

void CBigInteger::PrepareJavaRepresentation()
{
    if (mJavaIsValid) {
        return;
    }

    {    AutoLock syncLock(this);
        if (mJavaIsValid) {
            return;
        }

        Int32 sign = mBigInt->Sign();
        AutoPtr< ArrayOf<Int32> > digits = NULL;
        if (sign != 0) {
            digits = mBigInt->LittleEndianIntsMagnitude();
        }
        else {
            digits = ArrayOf<Int32>::Alloc(1);
            (*digits)[0] = 0;
        }

        SetJavaRepresentation(sign, digits->GetLength(), *digits);
    }
}

ECode CBigInteger::ValueOf(
    /* [in] */ Int64 value,
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(result);

    if (value < 0) {
        if (value != -1) {
            return CBigInteger::New(-1, -value, result);
        }
        *result = MINUS_ONE;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
    else if (value == 0) {
        *result = ZERO;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
    else if (value == 1) {
        *result = ONE;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
    else if (value == 10) {
        *result = TEN;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
    else {
        return CBigInteger::New(1, value, result);
    }
}

ECode CBigInteger::ToByteArray(
    /* [out, callee] */ ArrayOf<Byte>** bytes)
{
    return TwosComplement(bytes);
}

ECode CBigInteger::NewFromBigInt(
    /* [in] */ BigInt* bigInt,
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    AutoPtr<CBigInteger> obj;
    CBigInteger::NewByFriend((CBigInteger**)&obj);
    obj->SetBigInt(bigInt);
    *result = (IBigInteger*)obj;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CBigInteger::Abs(
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<BigInt> bigInt = GetBigInt();
    if (bigInt->Sign() >= 0) {
        *result = this;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }

    AutoPtr<BigInt> a = new BigInt();
    FAIL_RETURN(bigInt->Copy(*a));
    a->SetSign(1);
    return NewFromBigInt(a, result);
}

ECode CBigInteger::Negate(
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<BigInt> bigInt = GetBigInt();
    Int32 sign = bigInt->Sign();
    if (sign == 0) {
        *result = this;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }

    AutoPtr<BigInt> a = new BigInt();
    FAIL_RETURN(bigInt->Copy(*a));
    a->SetSign(-sign);
    return NewFromBigInt(a, result);
}

ECode CBigInteger::Add(
    /* [in] */ IBigInteger* value,
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(value);
    VALIDATE_NOT_NULL(result);

    AutoPtr<BigInt> lhs = GetBigInt();
    AutoPtr<BigInt> rhs = ((CBigInteger*)value)->GetBigInt();
    if (rhs->Sign() == 0) {
        *result = this;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
    if (lhs->Sign() == 0) {
        *result = (IBigInteger*)value;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }

    AutoPtr<BigInt> r = new BigInt();
    FAIL_RETURN(BigInt::Addition(*lhs, *rhs, *r));
    return NewFromBigInt(r, result);
}

ECode CBigInteger::Subtract(
    /* [in] */ IBigInteger* value,
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(value);
    VALIDATE_NOT_NULL(result);

    AutoPtr<BigInt> lhs = GetBigInt();
    AutoPtr<BigInt> rhs = ((CBigInteger*)value)->GetBigInt();
    if (rhs->Sign() == 0) {
        *result = this;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }

    AutoPtr<BigInt> r = new BigInt();
    FAIL_RETURN(BigInt::Subtraction(*lhs, *rhs, *r));
    return NewFromBigInt(r, result);
}

ECode CBigInteger::GetSignum(
    /* [out] */ Int32* sign)
{
    VALIDATE_NOT_NULL(sign);

    if (mJavaIsValid) {
        *sign = mSign;
    }
    AutoPtr<BigInt> si = GetBigInt();
    *sign = si->Sign();

    return NOERROR;
}

ECode CBigInteger::ShiftRight(
    /* [in] */ Int32 n,
    /* [out] */ IBigInteger** result)
{
    return ShiftLeft(-n, result);
}

ECode CBigInteger::ShiftLeft(
    /* [in] */ Int32 n,
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(result);

    if (n == 0) {
        *result = this;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }

    Int32 sign;
    GetSignum(&sign);
    if (sign == 0) {
        *result = this;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }

    if ((sign > 0) || (n >= 0)) {
        AutoPtr<BigInt> r = new BigInt();
        FAIL_RETURN(BigInt::Shift(*GetBigInt(), n, *r));
        return NewFromBigInt(r, result);
    }
    else {
        // Negative numbers faking 2's complement:
        // Not worth optimizing this:
        // Sticking to Harmony Java implementation.
        return BitLevel::ShiftRight(this, -n, result);
    }

    return NOERROR;
}

ECode CBigInteger::ShiftLeftOneBit(
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(result);

    Int32 signum;
    GetSignum(&signum);
    if (signum == 0) {
        *result = this;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
    else {
        return BitLevel::ShiftLeftOneBit(this, result);
    }
}

ECode CBigInteger::BitLength(
    /* [out] */ Int32* length)
{
    VALIDATE_NOT_NULL(length);

    // Optimization to avoid unnecessary duplicate representation:
    if (!mNativeIsValid && mJavaIsValid) {
        *length = BitLevel::BitLength(*this);
    }
    else {
        *length = GetBigInt()->BitLength();
    }

    return NOERROR;
}

ECode CBigInteger::TestBit(
    /* [in] */ Int32 n,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = FALSE;

    if (n < 0) {
        return E_ARITHMETIC_EXCEPTION;
    }

    Int32 sign = 0;
    GetSignum(&sign);
    if (sign > 0 && mNativeIsValid && !mJavaIsValid) {
        AutoPtr<BigInt> bigInt = GetBigInt();
        *result = bigInt->IsBitSet(n);
        return NOERROR;
    }
    else {
        // Negative numbers faking 2's complement:
        // Not worth optimizing this:
        // Sticking to Harmony Java implementation.
        PrepareJavaRepresentation();
        if (n == 0) {
            *result = (((*mDigits)[0] & 1) != 0);
            return NOERROR;
        }
        Int32 intCount = n >> 5;
        if (intCount >= mNumberLength) {
            *result =  (sign < 0);
            return NOERROR;
        }
        Int32 digit = (*mDigits)[intCount];
        n = (1 << (n & 31)); // Int32 with 1 set to the needed position
        if (sign < 0) {
            Int32 firstNonZeroDigit = GetFirstNonzeroDigit();
            if (intCount < firstNonZeroDigit) {
                return NOERROR;
            }
            else if (firstNonZeroDigit == intCount) {
                digit = -digit;
            }
            else {
                digit = ~digit;
            }
        }

        *result = ((digit & n) != 0);
    }

    return NOERROR;
}

ECode CBigInteger::SetBit(
    /* [in] */ Int32 n,
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(result);
    PrepareJavaRepresentation();
    Boolean testResult;
    TestBit(n, &testResult);
    if (!testResult) {
        return BitLevel::FlipBit(this, n, result);
    }
    else {
        *result = this;
        REFCOUNT_ADD(*result);
    }

    return NOERROR;
}

ECode CBigInteger::ClearBit(
    /* [in] */ Int32 n,
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(result);
    PrepareJavaRepresentation();
    Boolean testResult;
    TestBit(n, &testResult);
    if (testResult) {
        BitLevel::FlipBit(this, n, result);
    }
    else {
        *result = this;
        REFCOUNT_ADD(*result);
    }

    return NOERROR;
}

ECode CBigInteger::FlipBit(
    /* [in] */ Int32 n,
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(result);

    PrepareJavaRepresentation();
    if (n < 0) {
        *result = NULL;
        return E_ARITHMETIC_EXCEPTION;
    }

    return BitLevel::FlipBit(this, n, result);
}

ECode CBigInteger::GetLowestSetBit(
    /* [out] */ Int32* position)
{
    VALIDATE_NOT_NULL(position);

    using Elastos::Core::Math;

    PrepareJavaRepresentation();
    if (mSign == 0) {
        *position = -1;
        return NOERROR;
    }
    // (sign != 0) implies that exists some non zero digit
    Int32 i = GetFirstNonzeroDigit();
    *position = ((i << 5) + Math::NumberOfTrailingZeros((*mDigits)[i]));

    return NOERROR;
}

ECode CBigInteger::BitCount(
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number);

    PrepareJavaRepresentation();
    *number = BitLevel::BitCount(*this);
    return NOERROR;
}

ECode CBigInteger::Not(
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(result);
    PrepareJavaRepresentation();

    AutoPtr<IBigInteger> tmp = Logical::Not(this);
    *result = tmp;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CBigInteger::And(
    /* [in] */ IBigInteger* value,
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(value);
    VALIDATE_NOT_NULL(result);

    CBigInteger* cValue = (CBigInteger*)value;
    PrepareJavaRepresentation();
    cValue->PrepareJavaRepresentation();

    AutoPtr<IBigInteger> tmp = Logical::And(this, cValue);
    *result = tmp;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CBigInteger::Or(
    /* [in] */ IBigInteger* value,
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(value);
    VALIDATE_NOT_NULL(result);

    CBigInteger* cValue = (CBigInteger*)value;
    PrepareJavaRepresentation();
    cValue->PrepareJavaRepresentation();

    AutoPtr<IBigInteger> tmp = Logical::Or(this, cValue);
    *result = tmp;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CBigInteger::Xor(
    /* [in] */ IBigInteger* value,
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(value);
    VALIDATE_NOT_NULL(result);

    CBigInteger* cValue = (CBigInteger*)value;
    PrepareJavaRepresentation();
    cValue->PrepareJavaRepresentation();

    AutoPtr<IBigInteger> tmp = Logical::Xor(this, cValue);
    *result = tmp;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CBigInteger::AndNot(
    /* [in] */ IBigInteger* value,
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(value);
    VALIDATE_NOT_NULL(result);

    CBigInteger* cValue = (CBigInteger*)value;
    PrepareJavaRepresentation();
    cValue->PrepareJavaRepresentation();

    AutoPtr<IBigInteger> tmp = Logical::AndNot(this, cValue);
    *result = tmp;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CBigInteger::CompareTo(
    /* [in] */ IInterface* _other,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = -1;
    VALIDATE_NOT_NULL(_other);

    IBigInteger* other = (IBigInteger*)_other->Probe(EIID_IBigInteger);
    if (other == NULL) {
        return E_CLASS_CAST_EXCEPTION;
    }

    CBigInteger* bi = (CBigInteger*)other;
    if (this == bi) {
        *result = 0;
    }
    else {
        *result = BigInt::Cmp(*GetBigInt(), *bi->GetBigInt());
    }

    return NOERROR;
}

ECode CBigInteger::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    VALIDATE_NOT_NULL(other);

    if (IBigInteger::Probe(other) == NULL) return NOERROR;

    CBigInteger* bi = (CBigInteger*)IBigInteger::Probe(other);
    if (this == bi) {
        *result = TRUE;
    }
    else {
        *result = (BigInt::Cmp(*mBigInt, *bi->GetBigInt()) == 0);
    }
    return NOERROR;
}

ECode CBigInteger::Min(
    /* [in] */ IBigInteger* value,
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    VALIDATE_NOT_NULL(value);

    Int32 cmpResult;
    CompareTo(value, &cmpResult);
    if (cmpResult == -1) {
        *result = this;
        REFCOUNT_ADD(*result);
    }
    else {
        *result = value;
        REFCOUNT_ADD(*result);
    }

    return NOERROR;
}

ECode CBigInteger::Max(
    /* [in] */ IBigInteger* value,
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    VALIDATE_NOT_NULL(value);

    Int32 cmpResult;
    CompareTo(value, &cmpResult);
    if (cmpResult == 1) {
        *result = this;
        REFCOUNT_ADD(*result);
    }
    else {
        *result = value;
        REFCOUNT_ADD(*result);
    }

    return NOERROR;
}

ECode CBigInteger::Gcd(
    /* [in] */ IBigInteger* value,
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    VALIDATE_NOT_NULL(value);

    CBigInteger* cValue = (CBigInteger*)value;
    AutoPtr<BigInt> bigInt = new BigInt();
    FAIL_RETURN(BigInt::Gcd(*GetBigInt(), *cValue->GetBigInt(), *bigInt));
    return NewFromBigInt(bigInt, result);
}

ECode CBigInteger::Multiply(
    /* [in] */ IBigInteger* value,
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    VALIDATE_NOT_NULL(value);

    CBigInteger* cValue = (CBigInteger*)value;
    AutoPtr<BigInt> bigInt = new BigInt();
    FAIL_RETURN(BigInt::Product(*GetBigInt(), *cValue->GetBigInt(), *bigInt));
    return NewFromBigInt(bigInt, result);
}

ECode CBigInteger::Pow(
    /* [in] */ Int32 exp,
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(result);

    *result = NULL;
    if (exp < 0) {
        return E_ARITHMETIC_EXCEPTION;
    }

    AutoPtr<BigInt> bigInt = new BigInt();
    FAIL_RETURN(BigInt::Exp(*GetBigInt(), exp, *bigInt));
    return NewFromBigInt(bigInt, result);
}

ECode CBigInteger::DivideAndRemainder(
   /* [in] */ IBigInteger* divisor,
   /* [out, callee] */ ArrayOf<IBigInteger*>** bigIntegerArray)
{
    VALIDATE_NOT_NULL(divisor);
    VALIDATE_NOT_NULL(bigIntegerArray);

    CBigInteger* cDivisor = (CBigInteger*)divisor;
    AutoPtr<BigInt> divisorBigInt = cDivisor->GetBigInt();
    AutoPtr<BigInt> quotient = new BigInt();
    AutoPtr<BigInt> remainder = new BigInt();
    FAIL_RETURN(BigInt::Division(*GetBigInt(), *divisorBigInt, quotient, remainder));

    IBigInteger *qbi, *ebi;
    NewFromBigInt(quotient, &qbi);
    NewFromBigInt(remainder, &ebi);
    *bigIntegerArray = ArrayOf<IBigInteger*>::Alloc(2);
    REFCOUNT_ADD(*bigIntegerArray);
    (**bigIntegerArray).Set(0, qbi);
    (**bigIntegerArray).Set(1, ebi);

    return NOERROR;
}

ECode CBigInteger::Divide(
    /* [in] */ IBigInteger* divisor,
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(divisor);
    VALIDATE_NOT_NULL(result);

    CBigInteger* cDivisor = (CBigInteger*)divisor;
    AutoPtr<BigInt> divisorBigInt = cDivisor->GetBigInt();
    AutoPtr<BigInt> quotient = new BigInt();
    FAIL_RETURN(BigInt::Division(*GetBigInt(), *divisorBigInt, quotient, NULL));
    return NewFromBigInt(quotient, result);
}

ECode CBigInteger::Remainder(
    /* [in] */ IBigInteger* divisor,
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(divisor);
    VALIDATE_NOT_NULL(result);

    CBigInteger* cDivisor = (CBigInteger*)divisor;
    AutoPtr<BigInt> divisorBigInt = cDivisor->GetBigInt();
    AutoPtr<BigInt> remainder = new BigInt();
    FAIL_RETURN(BigInt::Division(*GetBigInt(), *divisorBigInt, NULL, remainder));
    return NewFromBigInt(remainder, result);
}

ECode CBigInteger::ModInverse(
    /* [in] */ IBigInteger* divisor,
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(divisor);
    VALIDATE_NOT_NULL(result);

    *result = NULL;

    CBigInteger* cDivisor = (CBigInteger*)divisor;
    Int32 sign;
    cDivisor->GetSignum(&sign);
    if (sign <= 0) {
        return E_ARITHMETIC_EXCEPTION;
    }

    AutoPtr<BigInt> divisorBigInt = cDivisor->GetBigInt();
    AutoPtr<BigInt> bigInt = new BigInt();
    FAIL_RETURN(BigInt::ModInverse(*GetBigInt(), *divisorBigInt, *bigInt));
    return NewFromBigInt(bigInt, result);
}

ECode CBigInteger::ModPow(
    /* [in] */ IBigInteger* exponent,
    /* [in] */ IBigInteger* m,
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(exponent);
    VALIDATE_NOT_NULL(m);
    VALIDATE_NOT_NULL(result);

    *result = NULL;

    CBigInteger* cm = (CBigInteger*)m;
    Int32 cmSign;
    cm->GetSignum(&cmSign);
    if (cmSign <= 0) {
        return E_ARITHMETIC_EXCEPTION;
    }

    CBigInteger* ce = (CBigInteger*)exponent;
    Int32 exponentSignum;
    ce->GetSignum(&exponentSignum);
    if (exponentSignum == 0) { // OpenSSL gets this case wrong; http://b/8574367.
        return ONE->Mod(m, result);
    }

    AutoPtr<IBigInteger> base;
    if (exponentSignum < 0) {
        ModInverse(m, (IBigInteger**)&base);
    }
    else {
        base = this;
    }

    AutoPtr<BigInt> baseBigInt = ((CBigInteger*)(&(*base)))->GetBigInt();
    AutoPtr<BigInt> ceBigInt = ce->GetBigInt();
    AutoPtr<BigInt> cmBigInt = cm->GetBigInt();
    AutoPtr<BigInt> bigInt = new BigInt();
    FAIL_RETURN(BigInt::ModExp(*baseBigInt, *ceBigInt, *cmBigInt, *bigInt));
    return NewFromBigInt(bigInt, result);
}

ECode CBigInteger::Mod(
    /* [in] */ IBigInteger* m,
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(m);
    VALIDATE_NOT_NULL(result);

    Int32 sign;
    m->GetSignum(&sign);
    if (sign < 0) {
        return E_ARITHMETIC_EXCEPTION;
    }

    CBigInteger* cm = (CBigInteger*)m;
    AutoPtr<BigInt> cmBigInt = cm->GetBigInt();
    AutoPtr<BigInt> bigInt = new BigInt();
    FAIL_RETURN(BigInt::Modulus(*GetBigInt(), *cmBigInt, *bigInt));
    return NewFromBigInt(bigInt, result);
}

ECode CBigInteger::IsProbablePrime(
    /* [in] */ Int32 certainty,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (certainty <= 0) {
        *result = TRUE;
        return NOERROR;
    }

    *result = GetBigInt()->IsPrime(certainty);
    return NOERROR;
}

ECode CBigInteger::NextProbablePrime(
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(result);

    *result = NULL;
    if (mSign < 0) {
        return E_ARITHMETIC_EXCEPTION;
    }

    return Primality::NextProbablePrime(this, result);
}

ECode CBigInteger::ProbablePrime(
    /* [in] */ Int32 bitLength,
    /* [in] */ IRandom* random,
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(result);

    return CBigInteger::New(bitLength, 100, random, result);
}

ECode CBigInteger::TwosComplement(
    /* [out, callee] */ ArrayOf<Byte>** bytes)
{
    VALIDATE_NOT_NULL(bytes);

    PrepareJavaRepresentation();
    if (mSign == 0) {
        *bytes = ArrayOf<Byte>::Alloc(1);
        REFCOUNT_ADD(*bytes);
        (**bytes)[0] = 0;
        return NOERROR;
    }

    Int32 bitLen = 0;
    BitLength(&bitLen);
    Int32 iThis = GetFirstNonzeroDigit();
    Int32 bytesLen = (bitLen >> 3) + 1;

    /* Puts the little-endian Int32 array representing the magnitude
     * of this BigInteger into the big-endian byte array. */
    *bytes = ArrayOf<Byte>::Alloc(bytesLen);
    REFCOUNT_ADD(*bytes);

    Int32 firstByteNumber = 0;
    Int32 highBytes;
    Int32 bytesInInteger = 4;
    Int32 hB;

    if (bytesLen - (mNumberLength << 2) == 1) {
        (**bytes)[0] = (Byte) ((mSign < 0) ? -1 : 0);
        highBytes = 4;
        firstByteNumber++;
    }
    else {
        hB = bytesLen & 3;
        highBytes = (hB == 0) ? 4 : hB;
    }

    Int32 digitIndex = iThis;
    bytesLen -= iThis << 2;

    if (mSign < 0) {
        Int32 digit = -(*mDigits)[digitIndex];
        digitIndex++;
        if (digitIndex == mNumberLength) {
            bytesInInteger = highBytes;
        }
        for (Int32 i = 0; i < bytesInInteger; i++, digit >>= 8) {
            (**bytes)[--bytesLen] = (Byte) digit;
        }
        while (bytesLen > firstByteNumber) {
            digit = ~(*mDigits)[digitIndex];
            digitIndex++;
            if (digitIndex == mNumberLength) {
                bytesInInteger = highBytes;
            }
            for (Int32 i = 0; i < bytesInInteger; i++, digit >>= 8) {
                (**bytes)[--bytesLen] = (Byte) digit;
            }
        }
    }
    else {
        while (bytesLen > firstByteNumber) {
            Int32 digit = (*mDigits)[digitIndex];
            digitIndex++;
            if (digitIndex == mNumberLength) {
                bytesInInteger = highBytes;
            }
            for (Int32 i = 0; i < bytesInInteger; i++, digit >>= 8) {
                (**bytes)[--bytesLen] = (Byte) digit;
            }
        }
    }

    return NOERROR;
}

Int32 CBigInteger::GetFirstNonzeroDigit()
{
    if (mFirstNonzeroDigit == -2) {
        Int32 i;
        if (mSign == 0) {
            i = -1;
        }
        else {
            for (i = 0; (*mDigits)[i] == 0; i++) {
                ;
            }
        }
        mFirstNonzeroDigit = i;
    }
    return mFirstNonzeroDigit;
}

Int32 CBigInteger::MultiplyByInt(
    /* [in] */ ArrayOf<Int32>& res,
    /* [in] */ ArrayOf<Int32>& a,
    /* [in] */ Int32 aSize,
    /* [in] */ Int32 factor)
{
    Int64 carry = 0;
    for (Int32 i = 0; i < aSize; i++) {
        carry += (a[i] & 0xFFFFFFFFLL) * (factor & 0xFFFFFFFFLL);
        res[i] = (Int32) carry;
        carry =  (UInt64)carry >> 32;
    }
    return (Int32) carry;
}

Int32 CBigInteger::InplaceAdd(
    /* [in] */ ArrayOf<Int32>& a,
    /* [in] */ Int32 aSize,
    /* [in] */ Int32 addend)
{
    Int64 carry = addend & 0xFFFFFFFFLL;

    for (Int32 i = 0; (carry != 0) && (i < aSize); i++) {
        carry += a[i] & 0xFFFFFFFFLL;
        a[i] = (Int32) carry;
        carry >>= 32;
    }
    return (Int32) carry;
}

/** @see BigInteger#BigInteger(String, Int32) */
ECode CBigInteger::ParseFromString(
    /* [in] */ CBigInteger& bi,
    /* [in] */ const String& value,
    /* [in] */ Int32 radix)
{
    Int32 stringLength = value.GetLength();
    Int32 endChar = stringLength;

    Int32 sign;
    Int32 startChar;
    if (value.GetChar(0) == '-') {
        sign = -1;
        startChar = 1;
        stringLength--;
    }
    else {
        sign = 1;
        startChar = 0;
    }

    /*
     * We use the following algorithm: split a string into portions of n
     * characters and convert each portion to an integer according to the
     * radix. Then convert an pow(radix, n) based number to binary using the
     * multiplication method. See D. Knuth, The Art of Computer Programming,
     * vol. 2.
     */

    Int32 charsPerInt = Conversion::DigitFitInInt[radix];
    Int32 bigRadixDigitsLength = stringLength / charsPerInt;
    Int32 topChars = stringLength % charsPerInt;

    if (topChars != 0) {
        bigRadixDigitsLength++;
    }
    AutoPtr<ArrayOf<Int32> > digits = ArrayOf<Int32>::Alloc(bigRadixDigitsLength);

    // Get the maximal power of radix that fits in Int32
    Int32 bigRadix = Conversion::BigRadices[radix - 2];
    // Parse an input string and accumulate the BigInteger's magnitude
    Int32 digitIndex = 0; // index of digits array
    Int32 substrEnd = startChar + ((topChars == 0) ? charsPerInt : topChars);

    for (Int32 substrStart = startChar;
            substrStart < endChar;
            substrStart = substrEnd, substrEnd = substrStart + charsPerInt) {
        String subStr(value.Substring(substrStart, substrEnd));
        Int32 bigRadixDigit;
        FAIL_RETURN(StringUtils::Parse(subStr, radix, &bigRadixDigit));
        Int32 newDigit = MultiplyByInt(*digits, *digits, digitIndex, bigRadix);
        newDigit += InplaceAdd(*digits, digitIndex, bigRadixDigit);
        (*digits)[digitIndex++] = newDigit;
    }
    Int32 numberLength = digitIndex;
    bi.SetJavaRepresentation(sign, numberLength, *digits);

    return NOERROR;
}

ECode CBigInteger::ToString(
    /* [out] */ String* string)
{
    VALIDATE_NOT_NULL(string);

    *string = GetBigInt()->DecString();
    return NOERROR;
}

ECode CBigInteger::ToString(
    /* [in] */ Int32 radix,
    /* [out] */ String* string)
{
    VALIDATE_NOT_NULL(string);

    if (radix == 10) {
        *string = GetBigInt()->DecString();
    }
    else {
        PrepareJavaRepresentation();
        *string = Conversion::BigInteger2String(*this, radix);
    }
    return NOERROR;
}

ECode CBigInteger::ByteValue(
    /* [out] */ Byte* result)
{
    VALIDATE_NOT_NULL(result);

    if (mNativeIsValid && mBigInt->TwosCompFitsIntoBytes(4)) {
        *result = (Byte) mBigInt->LongInt();
    }
    else {
        PrepareJavaRepresentation();
        *result = (Byte)(mSign * (*mDigits)[0]);
    }

    return NOERROR;
}

ECode CBigInteger::Int16Value(
    /* [out] */ Int16* result)
{
    VALIDATE_NOT_NULL(result);

    if (mNativeIsValid && mBigInt->TwosCompFitsIntoBytes(4)) {
        *result = (Int16) mBigInt->LongInt();
    }
    else {
        PrepareJavaRepresentation();
        *result = (Int16)(mSign * (*mDigits)[0]);
    }

    return NOERROR;
}

ECode CBigInteger::Int32Value(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    if (mNativeIsValid && mBigInt->TwosCompFitsIntoBytes(4)) {
        *result = (Int32) mBigInt->LongInt();
    }
    else {
        PrepareJavaRepresentation();
        *result = (mSign * (*mDigits)[0]);
    }

    return NOERROR;
}

ECode CBigInteger::Int64Value(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    if (mNativeIsValid && mBigInt->TwosCompFitsIntoBytes(8)) {
        *result = mBigInt->LongInt();
    }
    else {
        PrepareJavaRepresentation();

        if (mNumberLength > 1) {
            Int64 value = (((Int64)(*mDigits)[1]) << 32) | ((*mDigits)[0] & 0xFFFFFFFFLL);
            *result = mSign * value;
        }
        else {
            Int64 value = (*mDigits)[0] & 0xFFFFFFFFLL;
            *result = mSign * value;
        }
    }

    return NOERROR;
}

ECode CBigInteger::FloatValue(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result);

    Double value;
    DoubleValue(&value);
    *result = (Float)value;
    return NOERROR;
}

ECode CBigInteger::DoubleValue(
    /* [out] */ Double* result)
{
    VALIDATE_NOT_NULL(result);
    *result = Conversion::BigInteger2Double(*this);
    return NOERROR;
}

ECode CBigInteger::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);
    *hashCode = mHashCode;
    if (mHashCode != 0) {
        return NOERROR;
    }

    PrepareJavaRepresentation();
    for (Int32 i = 0; i < mNumberLength; ++i) {
        mHashCode = mHashCode * 33 + (*mDigits)[i];
    }
    mHashCode = mHashCode * mSign;
    *hashCode = mHashCode;
    return NOERROR;
}

} // namespace Math
} // namespace Elastos
