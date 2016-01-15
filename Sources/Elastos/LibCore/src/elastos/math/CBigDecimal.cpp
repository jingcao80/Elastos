#include "CBigDecimal.h"
#include "CMathContext.h"
#include "Conversion.h"
#include "Multiplication.h"
#include "StringUtils.h"

using Elastos::Core::StringUtils;
using Elastos::Core::EIID_INumber;
using Elastos::Core::EIID_IComparable;

namespace Elastos {
namespace Math {

const Double CBigDecimal::Log10_2 = 0.3010299956639812;

const Int64 CBigDecimal::LongFivePow[] = {
    1LL,
    5LL,
    25LL,
    125LL,
    625LL,
    3125LL,
    15625LL,
    78125LL,
    390625LL,
    1953125LL,
    9765625LL,
    48828125LL,
    244140625LL,
    1220703125LL,
    6103515625LL,
    30517578125LL,
    152587890625LL,
    762939453125LL,
    3814697265625LL,
    19073486328125LL,
    95367431640625LL,
    476837158203125LL,
    2384185791015625LL,
    11920928955078125LL,
    59604644775390625LL,
    298023223876953125LL,
    1490116119384765625LL,
    7450580596923828125LL,
};

AutoPtr<ArrayOf<Char32> > CBigDecimal::CharZeros = NULL;

AutoPtr<IBigDecimal> CBigDecimal::ZERO = NULL;
AutoPtr<IBigDecimal> CBigDecimal::ONE = NULL;
AutoPtr<IBigDecimal> CBigDecimal::TEN = NULL;

AutoPtr<ArrayOf<IBigInteger*> > CBigDecimal::FivePow = NULL;
AutoPtr<ArrayOf<IBigInteger*> > CBigDecimal::TenPow = NULL;
AutoPtr<ArrayOf<IBigDecimal*> > CBigDecimal::BiScaledByZero = NULL;
AutoPtr<ArrayOf<IBigDecimal*> > CBigDecimal::ZeroScaledBy = NULL;

Int32 CBigDecimal::LongFivePowBitLength[] = {0};
Int32 CBigDecimal::LongPowsersOfTenBitLength[] = {0};

Boolean CBigDecimal::mIsStaticInited = InitStatic();

static AutoPtr<IBigDecimal> CreateBigDecimal(Int32 smallValue, Int32 scale)
{
    AutoPtr<CBigDecimal> obj;
    CBigDecimal::NewByFriend(smallValue, scale, (CBigDecimal**)&obj);
    AutoPtr<IBigDecimal> result = (IBigDecimal*)(obj.Get());
    return result;
}

Boolean CBigDecimal::InitStatic()
{
    if (!mIsStaticInited) {
        mIsStaticInited = TRUE;

        CharZeros = ArrayOf<Char32>::Alloc(CharZerosLength);
        for (Int32 i = 0; i < CharZerosLength; ++i) {
            (*CharZeros)[i] = '0';
        }

        ZERO = CreateBigDecimal(0, 0);
        ONE = CreateBigDecimal(1, 0);
        TEN = CreateBigDecimal(10, 0);

        BiScaledByZero = ArrayOf<IBigDecimal*>::Alloc(BiScaledByZeroLength);
        ZeroScaledBy = ArrayOf<IBigDecimal*>::Alloc(BiScaledByZeroLength);
        for (Int32 i = 0; i < BiScaledByZeroLength; ++i) {
            AutoPtr<IBigDecimal> value1 = CreateBigDecimal(i, 0);
            AutoPtr<IBigDecimal> value2 = CreateBigDecimal(0, i);
            BiScaledByZero->Set(i, value1);
            ZeroScaledBy->Set(i, value2);
        }

        for (Int32 i = 0; i < LongFivePowLength; ++i) {
            LongFivePowBitLength[i] = BitLength(LongFivePow[i]);
        }

        using Elastos::Core::Math;
        for (Int32 i = 0; i < Math::LongPowersOfTenLength; ++i) {
            LongPowsersOfTenBitLength[i] = BitLength(Math::LongPowersOfTen[i]);
        }

        // Taking the references of useful powers.
        TenPow = Multiplication::BigTenPows;
        FivePow = Multiplication::BigFivePows;
    }

    return mIsStaticInited;
}

CAR_OBJECT_IMPL(CBigDecimal)

CAR_INTERFACE_IMPL_3(CBigDecimal, Object, IBigDecimal, INumber, IComparable)

CBigDecimal::CBigDecimal()
    : mBitLength(0)
    , mSmallValue(0)
    , mScale(0)
    , mPrecision(0)
    , mHashCode(0)
{
}

ECode CBigDecimal::constructor(
    /* [in] */ Int64 smallValue,
    /* [in] */ Int32 scale)
{
    mSmallValue = smallValue;
    mScale = scale;
    mBitLength = BitLength(smallValue);
    return NOERROR;
}

ECode CBigDecimal::constructor(
    /* [in] */ Int32 smallValue,
    /* [in] */ Int32 scale)
{
    mSmallValue = smallValue;
    mScale = scale;
    mBitLength = BitLength(smallValue);
    return NOERROR;
}

ECode CBigDecimal::constructor(
    /* [in] */ Int32 val)
{
    return constructor(val, 0);
}

ECode CBigDecimal::constructor(
    /* [in] */ Int64 val)
{
    return constructor(val, 0);
}

ECode CBigDecimal::constructor(
    /* [in] */ Double val)
{
    using Elastos::Core::Math;
    if (Math::IsInfinite(val) || Math::IsNaN(val)) {
        return E_NUMBER_FORMAT_EXCEPTION;
    }

    Int64 bits = Math::DoubleToInt64Bits(val); // IEEE-754
    Int64 mantissa;
    Int32 trailingZeros;
    // Extracting the exponent, note that the bias is 1023
    mScale = 1075 - (Int32)((bits >> 52) & 0x7FFLL);
    // Extracting the 52 bits of the mantissa.
    mantissa = (mScale == 1075) ? (bits & 0xFFFFFFFFFFFFFLL) << 1
            : (bits & 0xFFFFFFFFFFFFFLL) | 0x10000000000000LL;
    if (mantissa == 0) {
        mScale = 0;
        mPrecision = 1;
    }
    // To simplify all factors '2' in the mantissa
    if (mScale > 0) {
        trailingZeros = Math::Min(mScale, Math::NumberOfTrailingZeros(mantissa));
        mantissa = ((UInt64)mantissa) >> (trailingZeros & 0x3F);
        mScale -= trailingZeros;
    }
    // Calculating the new unscaled value and the new mScale
    if ((bits >> 63) != 0) {
        mantissa = -mantissa;
    }
    Int32 mantissaBits = BitLength(mantissa);
    if (mScale < 0) {
        mBitLength = mantissaBits == 0 ? 0 : mantissaBits - mScale;
        if(mBitLength < 64) {
            mSmallValue = mantissa << (-mScale & 0x3F);
        }
        else {
            AutoPtr<BigInt> bi = new BigInt();
            bi->PutLongInt(mantissa);
            bi->Shift(-mScale);
            mIntVal = NULL;
            CBigInteger::NewFromBigInt(bi, (IBigInteger**)&mIntVal);
        }
        mScale = 0;
    }
    else if (mScale > 0) {
        // m * 2^e =  (m * 5^(-e)) * 10^e
        if(mScale < LongFivePowLength
                && mantissaBits + LongFivePowBitLength[mScale] < 64) {
            mSmallValue = mantissa * LongFivePow[mScale];
            mBitLength = BitLength(mSmallValue);
        }
        else {
            AutoPtr<IBigInteger> mbi, rbi;
            CBigInteger::ValueOf(mantissa, (IBigInteger**)&mbi);
            FAIL_RETURN(Multiplication::MultiplyByFivePow(mbi, mScale, (IBigInteger**)&rbi));
            SetUnscaledValue(rbi);
        }
    }
    else { // scale == 0
        mSmallValue = mantissa;
        mBitLength = mantissaBits;
    }

    return NOERROR;
}

ECode CBigDecimal::constructor(
    /* [in] */ const String& val)
{
    AutoPtr< ArrayOf<Char32> > buf = val.GetChars();
    return constructor(*buf, 0, buf->GetLength());
}

ECode CBigDecimal::constructor(
    /* [in] */ const ArrayOf<Char32>& buf)
{
    return constructor(buf, 0, buf.GetLength());
}

ECode CBigDecimal::constructor(
    /* [in] */ const ArrayOf<Char32>& in,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 len)
{
    Int32 begin = offset; // first index to be copied
    Int32 last = offset + (len - 1); // last index to be copied

    if (in.GetLength() == 0) {
        return E_NULL_POINTER_EXCEPTION;
    }
    if ((last >= in.GetLength()) || (offset < 0) || (len <= 0) || (last < 0)) {
        return E_NUMBER_FORMAT_EXCEPTION;
    }

    Int64 newScale; // the new scale
    String scaleString; // buffer for scale
    StringBuilder unscaledBuffer(len);// buffer for unscaled value
    String temp;
    Int32 bufLength = 0;
    // To skip a possible '+' symbol
    if ((offset <= last) && (in[offset] == '+')) {
        offset++;
        begin++;
    }
    Int32 counter = 0;
    Boolean wasNonZero = FALSE;
    // Accumulating all digits until a possible decimal point
    for (; (offset <= last) && (in[offset] != '.')
            && (in[offset] != 'e') && (in[offset] != 'E'); offset++) {
        if (!wasNonZero) {
            if (in[offset] == '0') {
                counter++;
            } else {
                wasNonZero = TRUE;
            }
        }
    }

    unscaledBuffer.Append(in, begin, offset - begin);
    temp = unscaledBuffer.ToString();

    bufLength += offset - begin;
    // A decimal point was found
    if ((offset <= last) && (in[offset] == '.')) {
        offset++;
        // Accumulating all digits until a possible exponent
        begin = offset;
        for (; (offset <= last) && (in[offset] != 'e')
            && (in[offset] != 'E'); offset++) {
            if (!wasNonZero) {
                if (in[offset] == '0') {
                    counter++;
                }
                else {
                    wasNonZero = TRUE;
                }
            }
        }
        mScale = offset - begin;
        bufLength += mScale;
        unscaledBuffer.Append(in, begin, mScale);
        temp = unscaledBuffer.ToString();
    } else {
        mScale = 0;
    }
    // An exponent was found
    if ((offset <= last) && ((in[offset] == 'e') || (in[offset] == 'E'))) {
        offset++;
        // Checking for a possible sign of scale
        begin = offset;
        if ((offset <= last) && (in[offset] == '+')) {
            offset++;
            if ((offset <= last) && (in[offset] != '-')) {
                begin++;
            }
        }
        // Accumulating all remaining digits
        StringBuilder inSB(in.GetLength());
        inSB.Append(in);
        inSB.Substring(begin, last + 1 - begin, &scaleString);

        // Checking if the scale is defined
        newScale = (Int64)mScale - StringUtils::ParseInt32(scaleString);
        mScale = (Int32)newScale;
        if (newScale != mScale) {
            return E_NUMBER_FORMAT_EXCEPTION;
        }
    }
    // Parsing the unscaled value
    temp = unscaledBuffer.ToString();

    if (bufLength < 19) {
        mSmallValue = StringUtils::ParseInt64(temp);
        mBitLength = BitLength(mSmallValue);
    } else {
        AutoPtr<IBigInteger> bi;
        CBigInteger::New(temp, (IBigInteger**)&bi);
        SetUnscaledValue(bi);
    }

    return NOERROR;
}

ECode CBigDecimal::constructor(
    /* [in] */ IBigInteger* val)
{
    return constructor(val, 0);
}

ECode CBigDecimal::constructor(
    /* [in] */ IBigInteger* unscaledVal,
    /* [in] */ Int32 scale)
{
    VALIDATE_NOT_NULL(unscaledVal);

    mScale = scale;
    SetUnscaledValue(unscaledVal);
    return NOERROR;
}

ECode CBigDecimal::constructor(
    /* [in] */ IBigInteger* val,
    /* [in] */ Int32 scale,
    /* [in] */ IMathContext* mc)
{
    VALIDATE_NOT_NULL(mc);
    VALIDATE_NOT_NULL(val);

    ECode ec = constructor(val, scale);
    if (FAILED(ec)) return ec;

    return InplaceRound(mc);
}

ECode CBigDecimal::constructor(
    /* [in] */ const ArrayOf<Char32>& buf,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [in] */ IMathContext* mc)
{
    this->constructor(buf, offset, length);
    return InplaceRound(mc);
}

ECode CBigDecimal::constructor(
    /* [in] */ const ArrayOf<Char32>& buf,
    /* [in] */ IMathContext* mc)
{
    this->constructor(buf, 0, buf.GetLength());
    return InplaceRound(mc);
}

ECode CBigDecimal::constructor(
    /* [in] */ const String& val,
    /* [in] */ IMathContext* mc)
{
    AutoPtr<ArrayOf<Char32> > chars = val.GetChars();
    this->constructor(*chars, 0, val.GetLength());
    return InplaceRound(mc);
}

ECode CBigDecimal::constructor(
    /* [in] */ Double val,
    /* [in] */ IMathContext* mc)
{
    this->constructor(val);
    return InplaceRound(mc);
}

ECode CBigDecimal::constructor(
    /* [in] */ IBigInteger* val,
    /* [in] */ IMathContext* mc)
{
    this->constructor(val);
    return InplaceRound(mc);
}

ECode CBigDecimal::constructor(
    /* [in] */ Int32 val,
    /* [in] */ IMathContext* mc)
{
    this->constructor(val,0);
    return InplaceRound(mc);
}

ECode CBigDecimal::constructor(
    /* [in] */ Int64 val,
    /* [in] */ IMathContext* mc)
{
    this->constructor(val);
    return InplaceRound(mc);
}

ECode CBigDecimal::ValueOf(
    /* [in] */ Int64 unscaledVal,
    /* [in] */ Int32 scale,
    /* [out] */ IBigDecimal** result)
{
    VALIDATE_NOT_NULL(result);

    if (scale == 0) {
        return ValueOf(unscaledVal, result);
    }

    if ((unscaledVal == 0) && (scale >= 0)
            && (scale < BiScaledByZeroLength)) {
        *result = (*ZeroScaledBy)[scale];
        REFCOUNT_ADD(*result);
        return NOERROR;
    }

    return CBigDecimal::New(unscaledVal, scale, result);
}

ECode CBigDecimal::ValueOf(
    /* [in] */ Int64 unscaledVal,
    /* [out] */ IBigDecimal** result)
{
    VALIDATE_NOT_NULL(result);

    if ((unscaledVal >= 0) && (unscaledVal < BiScaledByZeroLength)) {
        *result = (*BiScaledByZero)[unscaledVal];
        REFCOUNT_ADD(*result);
        return NOERROR;
    }

    return CBigDecimal::New(unscaledVal, 0, result);
}

ECode CBigDecimal::ValueOf(
    /* [in] */ Double val,
    /* [out] */ IBigDecimal** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    using Elastos::Core::Math;
    if (Math::IsInfinite(val) || Math::IsNaN(val)) {
        return E_NUMBER_FORMAT_EXCEPTION;
    }

    StringBuilder sb;
    sb.Append(val);
    String str = sb.ToString();
    return CBigDecimal::New(str, result);
}

ECode CBigDecimal::Add(
    /* [in] */ IBigDecimal* iaugend,
    /* [out] */ IBigDecimal** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    VALIDATE_NOT_NULL(iaugend);

    CBigDecimal* augend = (CBigDecimal*)iaugend;

    Int32 diffScale = this->mScale - augend->mScale;
    // Fast return when some operand is zero
    if (this->IsZero()) {
        if (diffScale <= 0) {
            *result = iaugend;
            REFCOUNT_ADD(*result);
            return NOERROR;
        }
        if (augend->IsZero()) {
            *result = (IBigDecimal*)this;
            REFCOUNT_ADD(*result);
            return NOERROR;
        }
    } else if (augend->IsZero()) {
        if (diffScale >= 0) {
            *result = (IBigDecimal*)this;
             REFCOUNT_ADD(*result);
             return NOERROR;
        }
    }
    // Let be:  this = [u1,s1]  and  augend = [u2,s2]
    if (diffScale == 0) {
        using Elastos::Core::Math;
        // case s1 == s2: [u1 + u2 , s1]
        if (Math::Max(mBitLength, augend->mBitLength) + 1 < 64) {
            return ValueOf(mSmallValue + augend->mSmallValue, mScale, result);
        }

        AutoPtr<IBigInteger> tbi = GetUnscaledValue();
        AutoPtr<IBigInteger> abi = augend->GetUnscaledValue();
        AutoPtr<IBigInteger> rbi;
        tbi->Add(abi, (IBigInteger**)&rbi);
        return CBigDecimal::New(rbi, mScale, result);
    }
    else if (diffScale > 0) {
        // case s1 > s2 : [(u1 + u2) * 10 ^ (s1 - s2) , s1]
        return AddAndMult10(this, augend, diffScale, result);
    }
    else {// case s2 > s1 : [(u2 + u1) * 10 ^ (s2 - s1) , s2]
        return AddAndMult10(augend, this, -diffScale, result);
    }
}

ECode CBigDecimal::AddAndMult10(
    /* [in] */ CBigDecimal* thisValue,
    /* [in] */ CBigDecimal* augend,
    /* [in] */ Int32 diffScale,
    /* [out] */ IBigDecimal** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    assert(thisValue && augend);

    using Elastos::Core::Math;
    if(diffScale < Math::LongPowersOfTenLength &&
            Math::Max(thisValue->mBitLength,
                    augend->mBitLength + LongPowsersOfTenBitLength[diffScale]) + 1 < 64) {
        return ValueOf(
            thisValue->mSmallValue + augend->mSmallValue * Math::LongPowersOfTen[diffScale],
            thisValue->mScale, result);
    }
    else {
        AutoPtr<IBigInteger> iabi = augend->GetUnscaledValue();
        AutoPtr<IBigInteger> itbi = thisValue->GetUnscaledValue();
        CBigInteger* tbi = (CBigInteger*)(IBigInteger*)itbi;
        AutoPtr<IBigInteger> mbi;
        FAIL_RETURN(Multiplication::MultiplyByTenPow(iabi, diffScale, (IBigInteger**)&mbi));
        AutoPtr<BigInt> bi = ((CBigInteger*)(IBigInteger*)mbi)->GetBigInt();
        bi->Add(*tbi->GetBigInt());

        AutoPtr<IBigInteger> rbi;
        CBigInteger::NewFromBigInt(bi, (IBigInteger**)&rbi);
        return CBigDecimal::New(rbi, thisValue->mScale, result);
    }
}

ECode CBigDecimal::Add(
    /* [in] */ IBigDecimal* iaugend,
    /* [in] */ IMathContext* mc,
    /* [out] */ IBigDecimal** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    VALIDATE_NOT_NULL(iaugend);
    VALIDATE_NOT_NULL(mc);

    CBigDecimal* augend = (CBigDecimal*)iaugend;

    Int64 diffScale = (Int64)mScale - augend->mScale;

    Int32 mcp;
    mc->GetPrecision(&mcp);
    // Some operand is zero or the precision is infinity
    if ((augend->IsZero()) || (IsZero()) || (mcp == 0)) {
        AutoPtr<IBigDecimal> abd;
        Add(iaugend, (IBigDecimal**)&abd);
        return abd->Round(mc, result);
    }

    CBigDecimal* larger; // operand with the largest unscaled value
    CBigDecimal* smaller; // operand with the smallest unscaled value
    IBigInteger* tempBI;

    // Cases where there is room for optimizations
    if (ApproxPrecision() < diffScale - 1) {
        larger = augend;
        smaller = this;
    }
    else if (augend->ApproxPrecision() < -diffScale - 1) {
        larger = this;
        smaller = augend;
    }
    else {// No optimization is done
        AutoPtr<IBigDecimal> abd;
        Add(iaugend, (IBigDecimal**)&abd);
        return abd->Round(mc, result);
    }

    if (mcp >= larger->ApproxPrecision()) {
        // No optimization is done
        AutoPtr<IBigDecimal> abd;
        Add(iaugend, (IBigDecimal**)&abd);
        return abd->Round(mc, result);
    }

    // Cases where it's unnecessary to add two numbers with very different scales
    Int32 largerSignum, smallerSignum;
    larger->GetSignum(&largerSignum);
    smaller->GetSignum(&smallerSignum);
    AutoPtr<IBigInteger> lubi = larger->GetUnscaledValue();
    if (largerSignum == smallerSignum) {
        AutoPtr<IBigInteger> mbi;
        FAIL_RETURN(Multiplication::MultiplyByPositiveInt(lubi,10, (IBigInteger**)&mbi));
        AutoPtr<IBigInteger> lbi;
        CBigInteger::ValueOf(largerSignum, (IBigInteger**)&lbi);
        mbi->Add(lbi, &tempBI);
    }
    else {
        AutoPtr<IBigInteger> lbi, nlbi;
        CBigInteger::ValueOf(largerSignum, (IBigInteger**)&lbi);
        CBigInteger::ValueOf(largerSignum * 9, (IBigInteger**)&nlbi);

        AutoPtr<IBigInteger> sbi;
        lubi->Subtract(lbi, (IBigInteger**)&sbi);
        AutoPtr<IBigInteger> mbi;
        FAIL_RETURN(Multiplication::MultiplyByPositiveInt(sbi,10, (IBigInteger**)&mbi));
        mbi->Add(nlbi, &tempBI);
    }

    // Rounding the improved adding
    AutoPtr<IBigDecimal> lbd;
    CBigDecimal::New(tempBI, larger->mScale + 1, (IBigDecimal**)&lbd);
    return lbd->Round(mc, result);
}

ECode CBigDecimal::Subtract(
    /* [in] */ IBigDecimal* isubtrahend,
    /* [out] */ IBigDecimal** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    VALIDATE_NOT_NULL(isubtrahend);

    CBigDecimal* subtrahend = (CBigDecimal*)isubtrahend;

    Int32 diffScale = mScale - subtrahend->mScale;
    // Fast return when some operand is zero
    if (IsZero()) {
        if (diffScale <= 0) {
            return subtrahend->Negate(result);
        }
        if (subtrahend->IsZero()) {
            *result = (IBigDecimal*)this;
            REFCOUNT_ADD(*result);
            return NOERROR;
        }
    }
    else if (subtrahend->IsZero()) {
        if (diffScale >= 0) {
            *result = (IBigDecimal*)this;
            REFCOUNT_ADD(*result);
            return NOERROR;
        }
    }

    using Elastos::Core::Math;

    AutoPtr<IBigInteger> ubi = GetUnscaledValue();
    AutoPtr<IBigInteger> sbi = subtrahend->GetUnscaledValue();

    // Let be: this = [u1,s1] and subtrahend = [u2,s2] so:
    if (diffScale == 0) {
        // case s1 = s2 : [u1 - u2 , s1]
        if (Math::Max(mBitLength, subtrahend->mBitLength) + 1 < 64) {
            return ValueOf(mSmallValue - subtrahend->mSmallValue, mScale, result);
        }

        AutoPtr<IBigInteger> subBI;
        ubi->Subtract(sbi, (IBigInteger**)&subBI);
        return CBigDecimal::New(subBI, mScale, result);
    }
    else if (diffScale > 0) {
        // case s1 > s2 : [ u1 - u2 * 10 ^ (s1 - s2) , s1 ]
        if(diffScale < Math::LongPowersOfTenLength &&
                Math::Max(mBitLength, subtrahend->mBitLength+LongPowsersOfTenBitLength[diffScale]) + 1 < 64) {
            Int32 val = mSmallValue-subtrahend->mSmallValue * Math::LongPowersOfTen[diffScale];
            return ValueOf(val, mScale, result);
        }

        AutoPtr<IBigInteger> mbi;
        FAIL_RETURN(Multiplication::MultiplyByTenPow(sbi, diffScale, (IBigInteger**)&mbi));
        AutoPtr<IBigInteger> subBI;
        ubi->Subtract(mbi, (IBigInteger**)&subBI);
        return CBigDecimal::New(subBI, mScale, result);
    }
    else {// case s2 > s1 : [ u1 * 10 ^ (s2 - s1) - u2 , s2 ]
        diffScale = -diffScale;
        if(diffScale < Math::LongPowersOfTenLength &&
                Math::Max(mBitLength + LongPowsersOfTenBitLength[diffScale],subtrahend->mBitLength)+1<64) {
            Int32 val = mSmallValue*Math::LongPowersOfTen[diffScale]-subtrahend->mSmallValue;
            return ValueOf(val, subtrahend->mScale, result);
        }

        AutoPtr<IBigInteger> mbi;
        FAIL_RETURN(Multiplication::MultiplyByTenPow(ubi,diffScale, (IBigInteger**)&mbi));
        AutoPtr<IBigInteger> subBI;
        mbi->Subtract(sbi, (IBigInteger**)&subBI);
        return CBigDecimal::New(subBI, subtrahend->mScale, result);
    }
}

ECode CBigDecimal::Subtract(
    /* [in] */ IBigDecimal* isubtrahend,
    /* [in] */ IMathContext* mc,
    /* [out] */ IBigDecimal** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    VALIDATE_NOT_NULL(isubtrahend);
    VALIDATE_NOT_NULL(mc);

    CBigDecimal* subtrahend = (CBigDecimal*)isubtrahend;

    Int64 diffScale = subtrahend->mScale - (Int64)mScale;
    Int32 thisSignum, subSignum;


    Int32 mcp;
    mc->GetPrecision(&mcp);
    // Some operand is zero or the precision is infinity
    if ((subtrahend->IsZero()) || IsZero() || (mcp == 0)) {
        AutoPtr<IBigDecimal> sbd;
        Subtract(isubtrahend, (IBigDecimal**)&sbd);
        return sbd->Round(mc, result);
    }

    // Now:   this != 0   and   subtrahend != 0
    if (subtrahend->ApproxPrecision() < diffScale - 1) {
        // Cases where it is unnecessary to subtract two numbers with very different scales
        if (mcp < ApproxPrecision()) {
            GetSignum(&thisSignum);
            subtrahend->GetSignum(&subSignum);

            AutoPtr<IBigInteger> tubi = GetUnscaledValue();
            AutoPtr<IBigInteger> sigBI;
            CBigInteger::ValueOf(thisSignum, (IBigInteger**)&sigBI);
            AutoPtr<IBigInteger> tempBI;
            if (thisSignum != subSignum) {
                AutoPtr<IBigInteger> mbi;
                FAIL_RETURN(Multiplication::MultiplyByPositiveInt(tubi, 10, (IBigInteger**)&mbi));
                mbi->Add(sigBI, (IBigInteger**)&tempBI);
            }
            else {
                AutoPtr<IBigInteger> nsbi;
                CBigInteger::ValueOf(thisSignum * 9, (IBigInteger**)&nsbi);
                AutoPtr<IBigInteger> subBI;
                tubi->Subtract(sigBI, (IBigInteger**)&subBI);

                AutoPtr<IBigInteger> mbi;
                FAIL_RETURN(Multiplication::MultiplyByPositiveInt(subBI, 10, (IBigInteger**)&mbi));
                mbi->Add(nsbi, (IBigInteger**)&tempBI);
            }

            // Rounding the improved subtracting
            AutoPtr<IBigDecimal> leftOperand; // it will be only the left operand (this)
            CBigDecimal::New(tempBI, mScale + 1, (IBigDecimal**)&leftOperand);
            return leftOperand->Round(mc, result);
        }
    }
    // No optimization is done
    AutoPtr<IBigDecimal> subBI;
    Subtract(subtrahend, (IBigDecimal**)&subBI);
    return subBI->Round(mc, result);
}

ECode CBigDecimal::Multiply(
    /* [in] */ IBigDecimal* imultiplicand,
    /* [out] */ IBigDecimal** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    VALIDATE_NOT_NULL(imultiplicand);

    CBigDecimal* multiplicand = (CBigDecimal*)imultiplicand;
    Int64 newScale = (Int64)mScale + multiplicand->mScale;

    if (IsZero() || (multiplicand->IsZero())) {
        return ZeroScaledByScale(newScale, result);
    }

    Int32 slti = 0;
    ECode ec = SafeLongToInt(newScale, &slti);
    if (FAILED(ec)) return ec;

    /* Let be: this = [u1,s1] and multiplicand = [u2,s2] so:
     * this x multiplicand = [ s1 * s2 , s1 + s2 ] */
    if(mBitLength + multiplicand->mBitLength < 64) {
        Int64 val = mSmallValue * multiplicand->mSmallValue;
        return ValueOf(val, slti, result);
    }

    AutoPtr<IBigInteger> tubi = GetUnscaledValue();
    AutoPtr<IBigInteger> mubi = multiplicand->GetUnscaledValue();
    AutoPtr<IBigInteger> rbi;
    tubi->Multiply(mubi, (IBigInteger**)&rbi);
    return CBigDecimal::New(rbi, slti, result);
    return NOERROR;
}

ECode CBigDecimal::Multiply(
    /* [in] */ IBigDecimal* multiplicand,
    /* [in] */ IMathContext* mc,
    /* [out] */ IBigDecimal** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    VALIDATE_NOT_NULL(multiplicand);
    VALIDATE_NOT_NULL(mc);

    Multiply(multiplicand, result);

    ((CBigDecimal*)(*result))->InplaceRound(mc);
    return NOERROR;
}

ECode CBigDecimal::Divide(
    /* [in] */ IBigDecimal* idivisor,
    /* [in] */ Int32 scale,
    /* [in] */ RoundingMode roundingMode,
    /* [out] */ IBigDecimal** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    VALIDATE_NOT_NULL(idivisor);

    CBigDecimal* divisor = (CBigDecimal*)idivisor;

    // Let be: this = [u1,s1]  and  divisor = [u2,s2]
    if (divisor->IsZero()) {
        return E_ARITHMETIC_EXCEPTION;
    }

    using Elastos::Core::Math;

    Int64 diffScale = ((Int64)mScale - divisor->mScale) - scale;

    // Check whether the diffScale will fit into an int. See http://b/17393664.
    if (BitLength(diffScale) > 32) {
        //throw new ArithmeticException(
        //        "Unable to perform divisor / dividend scaling: the difference in scale is too" +
        //                " big (" + diffScale + ")");
        return E_ARITHMETIC_EXCEPTION;
    }

    if(mBitLength < 64 && divisor->mBitLength < 64 ) {
        if(diffScale == 0) {
            return DividePrimitiveLongs(mSmallValue,
                    divisor->mSmallValue, scale, roundingMode, result);
        }
        else if(diffScale > 0) {
            if(diffScale < Math::LongPowersOfTenLength &&
                    divisor->mBitLength + LongPowsersOfTenBitLength[(Int32)diffScale] < 64) {
                return DividePrimitiveLongs(mSmallValue,
                        divisor->mSmallValue * Math::LongPowersOfTen[(Int32)diffScale],
                        scale, roundingMode, result);
            }
        }
        else { // diffScale < 0
            if(-diffScale < Math::LongPowersOfTenLength &&
                    mBitLength + LongPowsersOfTenBitLength[(Int32)-diffScale] < 64) {
                return DividePrimitiveLongs(
                        mSmallValue * Math::LongPowersOfTen[(Int32)-diffScale],
                        divisor->mSmallValue, scale, roundingMode, result);
            }
        }
    }

    AutoPtr<IBigInteger> scaledDividend = GetUnscaledValue();
    AutoPtr<IBigInteger> scaledDivisor = divisor->GetUnscaledValue(); // for scaling of 'u2'

    if (diffScale > 0) {
        // Multiply 'u2'  by:  10^((s1 - s2) - scale)
        AutoPtr<IBigInteger> temp;
        FAIL_RETURN(Multiplication::MultiplyByTenPow(
                scaledDivisor, (Int32)diffScale, (IBigInteger**)&temp));
        scaledDivisor = temp;
    }
    else if (diffScale < 0) {
        // Multiply 'u1'  by:  10^(scale - (s1 - s2))
        AutoPtr<IBigInteger> temp;
        FAIL_RETURN(Multiplication::MultiplyByTenPow(
                scaledDividend, (Int32)-diffScale, (IBigInteger**)&temp));
        scaledDividend = temp;
    }

    return DivideBigIntegers(
            (CBigInteger*)(IBigInteger*)scaledDividend,
            (CBigInteger*)(IBigInteger*)scaledDivisor,
            scale, roundingMode, result);
}

ECode CBigDecimal::DivideBigIntegers(
    /* [in] */ CBigInteger* scaledDividend,
    /* [in] */ CBigInteger* scaledDivisor,
    /* [in] */ Int32 scale,
    /* [in] */ RoundingMode roundingMode,
    /* [out] */ IBigDecimal** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    AutoPtr<ArrayOf<IBigInteger*> > quotAndRem;
    scaledDividend->DivideAndRemainder(scaledDivisor, (ArrayOf<IBigInteger*>**)&quotAndRem);  // quotient and remainder

    // If after division there is a remainder...
    IBigInteger* quotient = (*quotAndRem)[0];
    IBigInteger* remainder = (*quotAndRem)[1];

    Int32 remSig = 0;
    remainder->GetSignum(&remSig);
    if (remSig == 0) {
        return CBigDecimal::New(quotient, scale, result);
    }

    using Elastos::Core::Math;

    Int32 dividendSig = 0, divisorSig = 0;
    scaledDividend->GetSignum(&dividendSig);
    scaledDivisor->GetSignum(&divisorSig);
    Int32 divisorBitLen = 0;
    scaledDivisor->BitLength(&divisorBitLen);

    Int32 sign = dividendSig * divisorSig;
    Int32 compRem;                                      // 'compare to remainder'
    if(divisorBitLen < 63) { // 63 in order to avoid out of Int64 after *2
        Int64 rem = 0, divisor = 0;
        INumber* number;
        number = (INumber*)remainder->Probe(EIID_INumber);
        number->Int64Value(&rem);
        number = (INumber*)scaledDivisor->Probe(EIID_INumber);
        number->Int64Value(&divisor);
        compRem = LongCompareTo(Math::Abs(rem) * 2, Math::Abs(divisor));

        Boolean testBit;
        quotient->TestBit(0, &testBit);
        Int32 parityBit = testBit ? 1 : 0;

        // To look if there is a carry
        ECode ec = RoundingBehavior(parityBit, sign * (5 + compRem), roundingMode, &compRem);
        if (FAILED(ec)) return ec;
    }
    else {
        AutoPtr<IBigInteger> absBI, divisorAbsBI, shiftBI;
        remainder->Abs((IBigInteger**)&absBI);
        scaledDivisor->Abs((IBigInteger**)&divisorAbsBI);
        absBI->ShiftLeftOneBit((IBigInteger**)&shiftBI);

        // To check if the discarded fraction >= 0.5
        IComparable* comp = (IComparable*)shiftBI->Probe(EIID_IComparable);
        comp->CompareTo((IBigInteger*)divisorAbsBI, &compRem);

        Boolean testBit;
        quotient->TestBit(0, &testBit);
        Int32 parityBit = testBit ? 1 : 0;

        // Checking if:  remainder * 2 >= scaledDivisor
        ECode ec = RoundingBehavior(parityBit, sign * (5 + compRem), roundingMode, &compRem);
        if (FAILED(ec)) return ec;
    }

    if (compRem != 0) {
        Int32 quotBitLen = 0;
        Int64 longVal = 0;
        quotient->BitLength(&quotBitLen);
        if(quotBitLen < 63) {
            INumber* number = (INumber*)quotient->Probe(EIID_INumber);
            number->Int64Value(&longVal);
            return ValueOf(longVal + compRem,scale, result);
        }

        AutoPtr<IBigInteger> compBI, temp;
        CBigInteger::ValueOf(compRem, (IBigInteger**)&compBI);
        quotient->Add(compBI, (IBigInteger**)&temp);

        return CBigDecimal::New(temp, scale, result);
    }

    // Constructing the result with the appropriate unscaled value
    return CBigDecimal::New(quotient, scale, result);
}

ECode CBigDecimal::DividePrimitiveLongs(
    /* [in] */ Int64 scaledDividend,
    /* [in] */ Int64 scaledDivisor,
    /* [in] */ Int32 scale,
    /* [in] */ RoundingMode roundingMode,
    /* [out] */ IBigDecimal** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    using Elastos::Core::Math;

    Int64 quotient = scaledDividend / scaledDivisor;
    Int64 remainder = scaledDividend % scaledDivisor;
    Int32 sign = Math::Signum(scaledDividend) * Math::Signum(scaledDivisor);
    if (remainder != 0) {
        // Checking if:  remainder * 2 >= scaledDivisor
        Int32 compRem = LongCompareTo(Math::Abs(remainder) * 2,Math::Abs(scaledDivisor));

        Int32 temp = 0;
        ECode ec = RoundingBehavior(((Int32)quotient) & 1, sign * (5 + compRem), roundingMode, &temp);
        if (FAILED(ec)) return ec;

        // To look if there is a carry
        quotient += temp;;
    }

    // Constructing the result with the appropriate unscaled value
    return ValueOf(quotient, scale, result);
}

ECode CBigDecimal::Divide(
    /* [in] */ IBigDecimal* divisor,
    /* [in] */ RoundingMode roundingMode,
    /* [out] */ IBigDecimal** result)
{
    VALIDATE_NOT_NULL(divisor);
    VALIDATE_NOT_NULL(result);

    return Divide(divisor, mScale, roundingMode, result);
}

ECode CBigDecimal::Divide(
    /* [in] */ IBigDecimal* idivisor,
    /* [out] */ IBigDecimal** result)
{
    VALIDATE_NOT_NULL(idivisor);
    VALIDATE_NOT_NULL(result);

    *result = NULL;
    using Elastos::Core::Math;

    CBigDecimal* divisor = (CBigDecimal*)idivisor;
    if (divisor->IsZero()) {
        return E_ARITHMETIC_EXCEPTION;
    }

    AutoPtr<IBigInteger> p = GetUnscaledValue();
    Int64 diffScale = (Int64)mScale - divisor->mScale;
    Int32 psignum = 0;
    p->GetSignum(&psignum);
    if (psignum == 0) {
        return ZeroScaledByScale(diffScale, result);
    }

    Int32 newScale; // the new scale for final quotient
    Int32 k; // number of factors "2" in 'q'
    Int32 l = 0; // number of factors "5" in 'q'
    Int32 i = 1;
    Int32 lastPow = FivePow->GetLength() - 1;
    AutoPtr<IBigInteger> q = divisor->GetUnscaledValue();

    // To divide both by the GCD
    AutoPtr<IBigInteger> gcd; // greatest common divisor between 'p' and 'q'
    p->Gcd(q, (IBigInteger**)&gcd);

    AutoPtr<IBigInteger> temp;
    p->Divide(gcd, (IBigInteger**)&temp);
    p = temp;
    temp = NULL;

    q->Divide(gcd, (IBigInteger**)&temp);
    q = temp;
    temp = NULL;

    // To simplify all "2" factors of q, dividing by 2^k
    q->GetLowestSetBit(&k);
    q->ShiftRight(k, (IBigInteger**)&temp);
    q = temp;
    temp = NULL;

    // To simplify all "5" factors of q, dividing by 5^l

    do {
        AutoPtr<ArrayOf<IBigInteger*> > quotAndRem;
        q->DivideAndRemainder((*FivePow)[i], (ArrayOf<IBigInteger*>**)&quotAndRem);

        Int32 remSignum = 0;
        (*quotAndRem)[1]->GetSignum(&remSignum);
        if (remSignum == 0) {
            l += i;
            if (i < lastPow) {
                i++;
            }

            q = (*quotAndRem)[0];
        }
        else {
            if (i == 1) {
                break;
            }
            i = 1;
        }
    } while (TRUE);

    // If  abs(q) != 1  then the quotient is periodic
    q->Abs((IBigInteger**)&temp);
    Boolean isEqual = FALSE;
    temp->Equals(CBigInteger::ONE, &isEqual);
    temp = NULL;
    if (!isEqual) {
        return E_ARITHMETIC_EXCEPTION;
    }

    // The sign of the is fixed and the quotient will be saved in 'p'
    Int32 qSignum = 0;
    q->GetSignum(&qSignum);
    if (qSignum < 0) {
        p->Negate((IBigInteger**)&temp);
        p = temp;
        temp = NULL;
    }
    // Checking if the new scale is out of range
    ECode ec = SafeLongToInt(diffScale + Math::Max(k, l), &newScale);
    if (FAILED(ec)) return ec;

    // k >= 0  and  l >= 0  implies that  k - l  is in the 32-bit range
    i = k - l;

    if (i > 0) {
        FAIL_RETURN(Multiplication::MultiplyByFivePow(p, i, (IBigInteger**)&temp));
    }
    else {
        p->ShiftLeft(-i, (IBigInteger**)&temp);
    }
    p = temp;
    temp = NULL;

    return CBigDecimal::New(p, newScale, result);
}

ECode CBigDecimal::Divide(
    /* [in] */ IBigDecimal* idivisor,
    /* [in] */ IMathContext* mc,
    /* [out] */ IBigDecimal** result)
{
    VALIDATE_NOT_NULL(idivisor);
    VALIDATE_NOT_NULL(mc);
    VALIDATE_NOT_NULL(result);

    *result = NULL;
    using Elastos::Core::Math;

    Int32 mcPrecision = 0;
    mc->GetPrecision(&mcPrecision);

    CBigDecimal* divisor = (CBigDecimal*)idivisor;
    if ((mcPrecision == 0) || IsZero() || (divisor->IsZero())) {
        return Divide(idivisor, result);
    }

    /* Calculating how many zeros must be append to 'dividend'
     * to obtain a  quotient with at least 'mc->GetPrecision()' digits */
    Int64 trailingZeros = mcPrecision + 2LL
            + divisor->ApproxPrecision() - ApproxPrecision();
    Int64 diffScale = (Int64)mScale - divisor->mScale;
    Int64 newScale = diffScale; // scale of the final quotient
    Int32 compRem; // to compare the remainder
    Int32 i = 1; // index
    Int32 lastPow = TenPow->GetLength() - 1; // last power of ten
    AutoPtr<IBigInteger> integerQuot; // for temporal results

    AutoPtr<IBigInteger> tuBI = GetUnscaledValue();
    AutoPtr<IBigInteger> duBI = divisor->GetUnscaledValue();
    AutoPtr<ArrayOf<IBigInteger*> > quotAndRem;
    // In special cases it reduces the problem to call the dual method

    AutoPtr<IBigInteger> temp;
    if (trailingZeros > 0) {
        AutoPtr<IBigInteger> ptenBI;
        FAIL_RETURN(Multiplication::PowerOf10(trailingZeros, (IBigInteger**)&ptenBI));
        // To append trailing zeros at end of dividend
        tuBI->Multiply(ptenBI, (IBigInteger**)&temp);
        newScale += trailingZeros;
    }
    else {
        temp = tuBI;
    }

    temp->DivideAndRemainder(duBI, (ArrayOf<IBigInteger*>**)&quotAndRem);
    integerQuot = (*quotAndRem)[0];

    Int32 remSignum = 0;
    (*quotAndRem)[1]->GetSignum(&remSignum);
    // Calculating the exact quotient with at least 'mc->GetPrecision()' digits
    if (remSignum != 0) {
        temp = NULL;
        (*quotAndRem)[1]->ShiftLeftOneBit((IBigInteger**)&temp);

        // Checking if:   2 * remainder >= divisor ?
        IComparable* comp = (IComparable*)temp->Probe(EIID_IComparable);
        comp->CompareTo(duBI, &compRem);
        temp = NULL;

        Int32 quotSignum = 0;
        (*quotAndRem)[0]->GetSignum(&quotSignum);
        CBigInteger::ValueOf(quotSignum * (5 + compRem), (IBigInteger**)&temp);

        // quot := quot * 10 + r;     with 'r' in {-6,-5,-4, 0,+4,+5,+6}
        AutoPtr<IBigInteger> mBI, aBI;
        integerQuot->Multiply(CBigInteger::TEN, (IBigInteger**)&mBI);
        mBI->Add(temp, (IBigInteger**)&aBI);
        integerQuot = aBI;
        newScale++;
    }
    else {
        Boolean testBit = FALSE;
        // To strip trailing zeros until the preferred scale is reached
        while (NOERROR == integerQuot->TestBit(0, &testBit) && !testBit) {
            AutoPtr<ArrayOf<IBigInteger*> > tempQuotAndRem;
            integerQuot->DivideAndRemainder((*TenPow)[i], (ArrayOf<IBigInteger*>**)&tempQuotAndRem);
            Int32 tempRemSignum = 0;
            (*tempQuotAndRem)[1]->GetSignum(&tempRemSignum);
            if ((tempRemSignum == 0) && (newScale - i >= diffScale)) {
                newScale -= i;
                if (i < lastPow) {
                    i++;
                }
                integerQuot = (*tempQuotAndRem)[0];
            }
            else {
                if (i == 1) {
                    break;
                }
                i = 1;
            }
        }
    }

    Int32 intScale = 0;
    ECode ec = SafeLongToInt(newScale, &intScale);
    if (FAILED(ec)) return ec;

    // To perform rounding
    return CBigDecimal::New(integerQuot, intScale, mc, result);
}

ECode CBigDecimal::DivideToIntegralValue(
    /* [in] */ IBigDecimal* idivisor,
    /* [out] */ IBigDecimal** result)
{
    VALIDATE_NOT_NULL(idivisor);
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    CBigDecimal* divisor = (CBigDecimal*)idivisor;
    if (divisor->IsZero()) {
        return E_ARITHMETIC_EXCEPTION;
    }

    AutoPtr<IBigInteger> integralValue; // the integer of result
    AutoPtr<IBigInteger> tuBI = GetUnscaledValue();
    AutoPtr<IBigInteger> duBI = divisor->GetUnscaledValue();

    Int64 newScale = (Int64)mScale - divisor->mScale;
    Int64 tempScale = 0;
    Int32 i = 1;
    Int32 lastPow = TenPow->GetLength() - 1;

    if ((divisor->ApproxPrecision() + newScale > ApproxPrecision() + 1LL)
    || (IsZero())) {
        /* If the divisor's integer part is greater than this's integer part,
         * the result must be zero with the appropriate scale */
        integralValue = CBigInteger::ZERO;
    }
    else if (newScale == 0) {
        FAIL_RETURN(tuBI->Divide(duBI, (IBigInteger**)&integralValue));
    }
    else if (newScale > 0) {
        AutoPtr<IBigInteger> powerOfTen;
        FAIL_RETURN(Multiplication::PowerOf10(newScale, (IBigInteger**)&powerOfTen));
        AutoPtr<IBigInteger> dBI, mBI;
        FAIL_RETURN(duBI->Multiply(powerOfTen, (IBigInteger**)&mBI));
        FAIL_RETURN(tuBI->Divide(mBI, (IBigInteger**)&dBI));
        FAIL_RETURN(dBI->Multiply(powerOfTen, (IBigInteger**)&integralValue));
    }
    else {// (newScale < 0)
        AutoPtr<IBigInteger> powerOfTen;
        FAIL_RETURN(Multiplication::PowerOf10(-newScale, (IBigInteger**)&powerOfTen));
        AutoPtr<IBigInteger> tempBI;
        FAIL_RETURN(tuBI->Multiply(powerOfTen, (IBigInteger**)&tempBI));
        FAIL_RETURN(tempBI->Divide(duBI, (IBigInteger**)&integralValue));

        // To strip trailing zeros approximating to the preferred scale
        Boolean testBit = FALSE;
        while (NOERROR == integralValue->TestBit(0, &testBit) && !testBit) {
            AutoPtr<ArrayOf<IBigInteger*> > quotAndRem;
            FAIL_RETURN(integralValue->DivideAndRemainder((*TenPow)[i], (ArrayOf<IBigInteger*>**)&quotAndRem));
            Int32 remSignum = 0;
            (*quotAndRem)[1]->GetSignum(&remSignum);
            if (remSignum == 0 && (tempScale - i >= newScale)) {
                tempScale -= i;
                if (i < lastPow) {
                    i++;
                }
                integralValue = (*quotAndRem)[0];
            }
            else {
                if (i == 1) {
                    break;
                }
                i = 1;
            }
        }

        newScale = tempScale;
    }

    Int32 iSignum = 0;
    integralValue->GetSignum(&iSignum);
    if (iSignum == 0) {
        return ZeroScaledByScale(newScale, result);
    }
    else {
        Int32 iScale = 0;
        FAIL_RETURN(SafeLongToInt(newScale, &iScale));
        return CBigDecimal::New(integralValue, iScale, result);
    }
}

ECode CBigDecimal::DivideToIntegralValue(
    /* [in] */ IBigDecimal* idivisor,
    /* [in] */ IMathContext* mc,
    /* [out] */ IBigDecimal** result)
{
    VALIDATE_NOT_NULL(idivisor);
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    using Elastos::Core::Math;
    CBigDecimal* divisor = (CBigDecimal*)idivisor;

    Int32 mcPrecision = 0;
    mc->GetPrecision(&mcPrecision);

    // In special cases it call the dual method
    if ((mcPrecision == 0) || (IsZero()) || (divisor->IsZero())) {
        return DivideToIntegralValue(idivisor, result);
    }

    Int32 thisPre, divPre;
    GetPrecision(&thisPre);
    divisor->GetPrecision(&divPre);
    Int32 diffPrecision = thisPre - divPre;

    Int32 lastPow = TenPow->GetLength() - 1;
    Int64 diffScale = (Int64)mScale - divisor->mScale;
    Int64 newScale = diffScale;
    Int64 quotPrecision = diffPrecision - diffScale + 1;

    AutoPtr<IBigInteger> tempBI;
    AutoPtr<IBigInteger> tuBI = GetUnscaledValue();
    AutoPtr<IBigInteger> duBI = divisor->GetUnscaledValue();

     // Let be:   this = [u1,s1]   and   divisor = [u2,s2]
    if (quotPrecision <= 0) {
        tempBI = CBigInteger::ZERO;
    }
    else if (diffScale == 0) {
        // CASE s1 == s2:  to calculate   u1 / u2
        FAIL_RETURN(tuBI->Divide(duBI, (IBigInteger**)&tempBI));
    }
    else if (diffScale > 0) {
        // CASE s1 >= s2:  to calculate   u1 / (u2 * 10^(s1-s2)
        AutoPtr<IBigInteger> dBI, pBI, mBI;
        FAIL_RETURN(Multiplication::PowerOf10(diffScale, (IBigInteger**)&pBI));
        FAIL_RETURN(duBI->Multiply(pBI, (IBigInteger**)&mBI));
        FAIL_RETURN(tuBI->Divide(mBI, (IBigInteger**)&dBI));

        // To chose  10^newScale  to get a quotient with at least 'mc->GetPrecision()' digits
        newScale = Math::Min(diffScale,
                Math::Max(mcPrecision - quotPrecision + 1, 0LL));
        // To calculate: (u1 / (u2 * 10^(s1-s2)) * 10^newScale
        pBI = NULL;
        FAIL_RETURN(Multiplication::PowerOf10(newScale, (IBigInteger**)&pBI));
        FAIL_RETURN(dBI->Multiply(pBI, (IBigInteger**)&tempBI));
    }
    else { // CASE s2 > s1:
        /* To calculate the minimum power of ten, such that the quotient
         *   (u1 * 10^exp) / u2   has at least 'mc->GetPrecision()' digits. */
        Int64 exp = Math::Min(-diffScale,
                Math::Max((Int64) mcPrecision - diffPrecision, 0LL));
        Int64 compRemDiv;
        // Let be:   (u1 * 10^exp) / u2 = [q,r]
        AutoPtr<IBigInteger> pBI, mBI;
        FAIL_RETURN(Multiplication::PowerOf10(exp, (IBigInteger**)&pBI));
        FAIL_RETURN(tuBI->Multiply(pBI, (IBigInteger**)&mBI));

        AutoPtr<ArrayOf<IBigInteger*> > quotAndRem;
        FAIL_RETURN(mBI->DivideAndRemainder(duBI, (ArrayOf<IBigInteger*>**)&quotAndRem));
        newScale += exp; // To fix the scale
        exp = -newScale; // The remaining power of ten
        // If after division there is a remainder...
        Int32 remSignum = 0;
        (*quotAndRem)[1]->GetSignum(&remSignum);
        if ((remSignum != 0) && (exp > 0)) {
            AutoPtr<IBigDecimal> tempBD;
            FAIL_RETURN(CBigDecimal::New((*quotAndRem)[1], (IBigDecimal**)&tempBD));

            // Log10(r) + ((s2 - s1) - exp) > mc->GetPrecision ?
            Int32 tempPre = 0;
            ((CBigDecimal*)(IBigDecimal*)tempBD)->GetPrecision(&tempPre);
            compRemDiv = tempPre + exp - divPre;
            if (compRemDiv == 0) {
                pBI = mBI = NULL;
                // To calculate:  (r * 10^exp2) / u2
                FAIL_RETURN(Multiplication::PowerOf10(exp, (IBigInteger**)&pBI));
                FAIL_RETURN((*quotAndRem)[1]->Multiply(pBI, (IBigInteger**)&mBI));
                pBI = NULL;
                FAIL_RETURN(mBI->Divide(duBI, (IBigInteger**)&pBI));
                Int32 dSignum = 0;
                pBI->GetSignum(&dSignum);
                compRemDiv = Math::Abs(dSignum);
            }

            if (compRemDiv > 0) {
                return E_ARITHMETIC_EXCEPTION;
            }
        }

        tempBI = (*quotAndRem)[0];
    }

     // Fast return if the quotient is zero
    Int32 qSignum = 0;
    tempBI->GetSignum(&qSignum);
    if (qSignum == 0) {
        return ZeroScaledByScale(diffScale, result);
    }

    AutoPtr<IBigInteger> strippedBI = tempBI;
    AutoPtr<IBigDecimal> integralValue;
    FAIL_RETURN(CBigDecimal::New(tempBI, (IBigDecimal**)&integralValue));
    Int32 tempPre = 0;
    integralValue->GetPrecision(&tempPre);
    Int64 resultPrecision = tempPre;
    Int32 i = 1;
    // To strip trailing zeros until the specified precision is reached
    Boolean testBit = FALSE;
    while (NOERROR == strippedBI->TestBit(0, &testBit) && !testBit) {
        AutoPtr<ArrayOf<IBigInteger*> > quotAndRem;
        FAIL_RETURN(strippedBI->DivideAndRemainder((*TenPow)[i],
                (ArrayOf<IBigInteger*>**)&quotAndRem));
        Int32 remSignum = 0;
        (*quotAndRem)[1]->GetSignum(&remSignum);
        if (remSignum == 0
                && ((resultPrecision - i >= mcPrecision)
                        || (newScale - i >= diffScale))) {
            resultPrecision -= i;
            newScale -= i;
            if (i < lastPow) {
                i++;
            }
            strippedBI = (*quotAndRem)[0];
        }
        else {
            if (i == 1) {
                break;
            }
            i = 1;
        }
    }
    // To check if the result fit in 'mc->GetPrecision()' digits
    if (resultPrecision > mcPrecision) {
        return E_ARITHMETIC_EXCEPTION;
    }

    Int32 iScale = 0;
    FAIL_RETURN(SafeLongToInt(newScale, &iScale));

    CBigDecimal* bd = (CBigDecimal*)(IBigDecimal*)integralValue;
    bd->mScale = iScale;
    bd->SetUnscaledValue(strippedBI);
    *result = integralValue;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CBigDecimal::Remainder(
    /* [in] */ IBigDecimal* divisor,
    /* [out] */ IBigDecimal** result)
{
    VALIDATE_NOT_NULL(divisor);
    VALIDATE_NOT_NULL(result);

    *result = NULL;

    AutoPtr<ArrayOf<IBigDecimal*> > quotAndRem;
    FAIL_RETURN(DivideAndRemainder(divisor, (ArrayOf<IBigDecimal*>**)&quotAndRem));

    *result = (*quotAndRem)[1];
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CBigDecimal::Remainder(
    /* [in] */ IBigDecimal* divisor,
    /* [in] */ IMathContext* mc,
    /* [out] */ IBigDecimal** result)
{
    VALIDATE_NOT_NULL(divisor);
    VALIDATE_NOT_NULL(mc);
    VALIDATE_NOT_NULL(result);

    *result = NULL;

    AutoPtr<ArrayOf<IBigDecimal*> > quotAndRem;
    FAIL_RETURN(DivideAndRemainder(divisor, mc, (ArrayOf<IBigDecimal*>**)&quotAndRem));

    *result = (*quotAndRem)[1];
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CBigDecimal::DivideAndRemainder(
    /* [in] */ IBigDecimal* divisor,
    /* [out, callee] */ ArrayOf<IBigDecimal*>** result)
{
    VALIDATE_NOT_NULL(divisor);
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    AutoPtr<ArrayOf<IBigDecimal*> > array = ArrayOf<IBigDecimal*>::Alloc(2);
    AutoPtr<IBigDecimal> tmp;
    FAIL_RETURN(DivideToIntegralValue(divisor, (IBigDecimal**)&tmp));
    array->Set(0, tmp);


    AutoPtr<IBigDecimal> mBD;
    FAIL_RETURN(tmp->Multiply(divisor, (IBigDecimal**)&mBD));

    tmp = NULL;
    FAIL_RETURN(Subtract(mBD, (IBigDecimal**)&tmp));
    array->Set(1, tmp);
    *result = array;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CBigDecimal::DivideAndRemainder(
    /* [in] */ IBigDecimal* divisor,
    /* [in] */ IMathContext* mc,
    /* [out, callee] */ ArrayOf<IBigDecimal*>** result)
{
    VALIDATE_NOT_NULL(divisor);
    VALIDATE_NOT_NULL(mc);
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    AutoPtr<ArrayOf<IBigDecimal*> > array = ArrayOf<IBigDecimal*>::Alloc(2);
    AutoPtr<IBigDecimal> tmp;
    FAIL_RETURN(DivideToIntegralValue(divisor, mc, (IBigDecimal**)&tmp));
    array->Set(0, tmp);

    AutoPtr<IBigDecimal> mBD;
    FAIL_RETURN(tmp->Multiply(divisor, (IBigDecimal**)&mBD));

    tmp = NULL;
    FAIL_RETURN(Subtract(mBD, (IBigDecimal**)&tmp));
    array->Set(1, tmp);
    *result = array;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CBigDecimal::Pow(
    /* [in] */ Int32 n,
    /* [out] */ IBigDecimal** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    if ((n < 0) || (n > 999999999)) {
        return E_ARITHMETIC_EXCEPTION;
    }

    if (n == 0) {
        *result = CBigDecimal::ONE;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }

    Int64 newScale = mScale * (Int64)n;
    // Let be: this = [u,s]   so:  this^n = [u^n, s*n]
    if (IsZero()) {
        return ZeroScaledByScale(newScale, result);
    }

    Int32 iScale = 0;
    FAIL_RETURN(SafeLongToInt(newScale, &iScale));

    AutoPtr<IBigInteger> tubi = GetUnscaledValue();
    AutoPtr<IBigInteger> tempBI;
    FAIL_RETURN(tubi->Pow(n, (IBigInteger**)&tempBI));
    return CBigDecimal::New(tempBI, iScale, result);
}

ECode CBigDecimal::Pow(
    /* [in] */ Int32 n,
    /* [in] */ IMathContext* mc,
    /* [out] */ IBigDecimal** result)
{
    VALIDATE_NOT_NULL(mc);
    VALIDATE_NOT_NULL(result);

    *result = NULL;
    using Elastos::Core::Math;

    // In particular cases, it reduces the problem to call the other 'pow()'
    if ((n == 0) || ((IsZero()) && (n > 0))) {
        return Pow(n, result);
    }

    // The ANSI standard X3.274-1996 algorithm
    Int32 m = Math::Abs(n);
    Int32 mcPrecision;
    mc->GetPrecision(&mcPrecision);
    Int32 elength = (Int32)Math::Log10(m) + 1;   // decimal digits in 'n'

    if ((m > 999999999) || ((mcPrecision == 0) && (n < 0))
            || ((mcPrecision > 0) && (elength > mcPrecision))) {
        return E_ARITHMETIC_EXCEPTION;
    }

    AutoPtr<IMathContext> newPrecision = mc; // MathContext by default
    if (mcPrecision > 0) {
        RoundingMode mode;
        mc->GetRoundingMode(&mode);
        newPrecision = NULL;
        CMathContext::New(mcPrecision + elength + 1, mode, (IMathContext**)&newPrecision);
    }

    // The result is calculated as if 'n' were positive
    AutoPtr<IBigDecimal> accum; // the single accumulator
    Round(newPrecision, (IBigDecimal**)&accum);
    Int32 oneBitMask = Math::HighestOneBit(m) >> 1; // mask of bits

    AutoPtr<IBigDecimal> temp;
    while (oneBitMask > 0) {
        temp = NULL;
        FAIL_RETURN(accum->Multiply(accum, newPrecision, (IBigDecimal**)&temp));
        accum = temp;

        if ((m & oneBitMask) == oneBitMask) {
            temp = NULL;
            FAIL_RETURN(accum->Multiply((IBigDecimal*)this, newPrecision, (IBigDecimal**)&temp));
            accum = temp;
        }
        oneBitMask >>= 1;
    }

    // If 'n' is negative, the value is divided into 'ONE'
    if (n < 0) {
        temp = NULL;
        FAIL_RETURN(ONE->Divide(accum, newPrecision, (IBigDecimal**)&temp));
        accum = temp;
    }

    // The final value is rounded to the destination precision
    FAIL_RETURN(((CBigDecimal*)(IBigDecimal*)accum)->InplaceRound(mc));
    *result = (IBigDecimal*)accum;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CBigDecimal::Abs(
    /* [out] */ IBigDecimal** result)
{
    VALIDATE_NOT_NULL(result);
    Int32 signum = 0;
    GetSignum(&signum);
    if (signum < 0) {
        return Negate(result);
    }

    *result = (IBigDecimal*)this;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CBigDecimal::Abs(
    /* [in] */ IMathContext* mc,
    /* [out] */ IBigDecimal** result)
{
    VALIDATE_NOT_NULL(mc);
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    AutoPtr<IBigDecimal> _result = NULL;
    Int32 signum = 0, scale = 0;
    GetSignum(&signum);
    GetScale(&scale);
    if (signum < 0) {
        Negate((IBigDecimal**)&_result);
    }
    else {
        CBigDecimal::New(GetUnscaledValue(), scale, (IBigDecimal**)&_result);
    }

    ((CBigDecimal*)_result.Get())->InplaceRound(mc);
    *result = _result;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CBigDecimal::Negate(
    /* [out] */ IBigDecimal** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    using Elastos::Core::Math;
    if(mBitLength < 63 || (mBitLength == 63
            && mSmallValue != Math::INT64_MIN_VALUE)) {
        return ValueOf(-mSmallValue, mScale, result);
    }

    AutoPtr<IBigInteger> tubi = GetUnscaledValue();
    AutoPtr<IBigInteger> tempBI;
    FAIL_RETURN(tubi->Negate((IBigInteger**)&tempBI));
    return CBigDecimal::New(tempBI, mScale, result);
}

ECode CBigDecimal::Negate(
    /* [in] */ IMathContext* mc,
    /* [out] */ IBigDecimal** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    FAIL_RETURN(Negate(result));
    return ((CBigDecimal*)(*result))->InplaceRound(mc);
}

ECode CBigDecimal::Plus(
    /* [out] */ IBigDecimal** result)
{
    VALIDATE_NOT_NULL(result);
    *result = this;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CBigDecimal::Plus(
    /* [in] */ IMathContext* mc,
    /* [out] */ IBigDecimal** result)
{
    VALIDATE_NOT_NULL(result);
    return Round(mc, result);
}

ECode CBigDecimal::GetSignum(
    /* [out] */ Int32* sign)
{
    VALIDATE_NOT_NULL(sign);

    using Elastos::Core::Math;
    if(mBitLength < 64) {
        *sign = Math::Signum(mSmallValue);
        return NOERROR;
    }

    AutoPtr<IBigInteger> tubi = GetUnscaledValue();
    return tubi->GetSignum(sign);
}

ECode CBigDecimal::GetScale(
    /* [out] */ Int32* scale)
{
    VALIDATE_NOT_NULL(scale);
    *scale = mScale;
    return NOERROR;
}

ECode CBigDecimal::GetPrecision(
    /* [out] */ Int32* precision)
{
    VALIDATE_NOT_NULL(precision);
    *precision = 0;

    // Return the cached value if we have one.
    if (mPrecision != 0) {
        *precision = mPrecision;
        return NOERROR;
    }

    if (mBitLength == 0) {
        mPrecision = 1;
    }
    else if (mBitLength < 64) {
        mPrecision = DecimalDigitsInLong(mSmallValue);
    }
    else {
        Int32 decimalDigits = 1 + (Int32) ((mBitLength - 1) * Log10_2);
        AutoPtr<IBigInteger> tubi = GetUnscaledValue();
        AutoPtr<IBigInteger> pbi, dbi;
        FAIL_RETURN(Multiplication::PowerOf10(decimalDigits, (IBigInteger**)&pbi));
        FAIL_RETURN(tubi->Divide(pbi, (IBigInteger**)&dbi));

        // If after division the number isn't zero, there exists an additional digit
        Int32 dSignum = 0;
        dbi->GetSignum(&dSignum);
        if (dSignum != 0) {
            decimalDigits++;
        }
        mPrecision = decimalDigits;
    }
    *precision = mPrecision;
    return NOERROR;
}

ECode CBigDecimal::Round(
    /* [in] */ IMathContext* mc,
    /* [out] */ IBigDecimal** result)
{
    VALIDATE_NOT_NULL(mc);
    VALIDATE_NOT_NULL(result);

    AutoPtr<IBigInteger> tubi = GetUnscaledValue();
    CBigDecimal::New(tubi, mScale, result);

    return ((CBigDecimal*)(*result))->InplaceRound(mc);
}

ECode CBigDecimal::SetScale(
    /* [in] */ Int32 newScale,
    /* [in] */ RoundingMode roundingMode,
    /* [out] */ IBigDecimal** result)
{
    VALIDATE_NOT_NULL(result);

    Int64 diffScale = newScale - (Int64)mScale;
    // Let be:  'this' = [u,s]
    if(diffScale == 0) {
        *result = (IBigDecimal*)this;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }

    using Elastos::Core::Math;
    AutoPtr<IBigInteger> tubi = GetUnscaledValue();
    if(diffScale > 0) {
    // return  [u * 10^(s2 - s), newScale]
        if(diffScale < Math::LongPowersOfTenLength &&
                (mBitLength + LongPowsersOfTenBitLength[(Int32)diffScale]) < 64 ) {
            Int32 val = mSmallValue * Math::LongPowersOfTen[(Int32)diffScale];
            return ValueOf(val, newScale, result);
        }

        AutoPtr<IBigInteger> mbi;
        FAIL_RETURN(Multiplication::MultiplyByTenPow(tubi,(Int32)diffScale, (IBigInteger**)&mbi));
        return CBigDecimal::New(mbi, newScale, result);
    }

    // diffScale < 0
    // return  [u,s] / [1,newScale]  with the appropriate scale and rounding
    if(mBitLength < 64 && -diffScale < Math::LongPowersOfTenLength) {
        Int32 val = Math::LongPowersOfTen[(Int32)-diffScale];
        return DividePrimitiveLongs(mSmallValue, val, newScale,roundingMode, result);
    }

    AutoPtr<IBigInteger> mbi;
    FAIL_RETURN(Multiplication::PowerOf10(-diffScale, (IBigInteger**)&mbi));
    return DivideBigIntegers(
            (CBigInteger*)(IBigInteger*)tubi,
            (CBigInteger*)(IBigInteger*)mbi,
            newScale, roundingMode, result);
}

ECode CBigDecimal::SetScale(
    /* [in] */ Int32 newScale,
    /* [out] */ IBigDecimal** result)
{
    VALIDATE_NOT_NULL(result);
    return SetScale(newScale, RoundingMode_UNNECESSARY, result);
}

ECode CBigDecimal::MovePointLeft(
   /* [in] */ Int32 n,
   /* [out] */ IBigDecimal** result)
{
    VALIDATE_NOT_NULL(result);
    return MovePoint(mScale + (Int64)n, result);
}

ECode CBigDecimal::MovePoint(
    /* [in] */ Int64 newScale,
    /* [in] */ IBigDecimal** result)
{
    VALIDATE_NOT_NULL(result);

    *result = NULL;
    using Elastos::Core::Math;

    if (IsZero()) {
        return ZeroScaledByScale(Math::Max(newScale, 0LL), result);
    }

    Int32 iNewScale = 0;
    ECode ec = SafeLongToInt(newScale, &iNewScale);
    if (FAILED(ec)) return ec;

    AutoPtr<IBigInteger> tubi = GetUnscaledValue();
    /*
     * When: 'n'== Integer.MIN_VALUE isn't possible to call to
     * movePointRight(-n) sIInterfaceince -Integer.MIN_VALUE == Integer.MIN_VALUE
     */
    if (newScale >= 0) {
        if(mBitLength < 64) {
            return ValueOf(mSmallValue, iNewScale, result);
        }
        return CBigDecimal::New(tubi, iNewScale, result);
    }

    if(-newScale < Math::LongPowersOfTenLength &&
            mBitLength + LongPowsersOfTenBitLength[(Int32)-newScale] < 64 ) {
        Int64 val = mSmallValue * Math::LongPowersOfTen[(Int32)-newScale];
        return ValueOf(val, 0, result);
    }

    ec = SafeLongToInt(-newScale, &iNewScale);
    if (FAILED(ec)) return ec;
    AutoPtr<IBigInteger> mbi;
    FAIL_RETURN(Multiplication::MultiplyByTenPow(tubi, iNewScale, (IBigInteger**)&mbi));
    return CBigDecimal::New(mbi, 0, result);
}

ECode CBigDecimal::MovePointRight(
   /* [in] */ Int32 n,
   /* [out] */ IBigDecimal** result)
{
    VALIDATE_NOT_NULL(result);
    return MovePoint(mScale - (Int64)n, result);
}

ECode CBigDecimal::ScaleByPowerOfTen(
    /* [in] */ Int32 n,
    /* [out] */ IBigDecimal** result)
{
    VALIDATE_NOT_NULL(result);

    Int64 newScale = mScale - (Int64)n;
    Int32 iNewScale = 0;
    ECode ec = SafeLongToInt(newScale, &iNewScale);
    if (FAILED(ec)) return ec;

    if (mBitLength < 64) {
        //Taking care when a 0 is to be scaled
        if(mSmallValue == 0){
            return ZeroScaledByScale(newScale, result);
        }
        return ValueOf(mSmallValue, iNewScale, result);
    }

    AutoPtr<IBigInteger> tubi = GetUnscaledValue();
    return CBigDecimal::New(tubi, iNewScale, result);
}

ECode CBigDecimal::StripTrailingZeros(
   /* [out] */ IBigDecimal** result)
{
    VALIDATE_NOT_NULL(result);

    Int32 i = 1; // 1 <= i <= 18
    Int32 lastPow = TenPow->GetLength() - 1;
    Int64 newScale = mScale;

    if (IsZero()) {
        // Preserve RI compatibility, so BigDecimal.equals (which checks
        // value *and* scale) continues to work.
        *result = (IBigDecimal*)this;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }

    AutoPtr<IBigInteger> strippedBI = GetUnscaledValue();

    // while the number is even...
    Boolean testBit = FALSE;
    while (NOERROR == strippedBI->TestBit(0, &testBit) && !testBit) {
        AutoPtr<ArrayOf<IBigInteger*> > quotAndRem;
        // To divide by 10^i
        strippedBI->DivideAndRemainder((*TenPow)[i], (ArrayOf<IBigInteger*>**)&quotAndRem);
        // To look the remainder
        Int32 remSignum = 0;
        (*quotAndRem)[1]->GetSignum(&remSignum);
        if (remSignum == 0) {
            // To adjust the scale
            newScale -= i;
            if (i < lastPow) {
                // To set to the next power
                i++;
            }
            strippedBI = (*quotAndRem)[0];
        }
        else {
            if (i == 1) {
                // 'this' has no more trailing zeros
                break;
            }
            // To set to the smallest power of ten
            i = 1;
        }
    }

    Int32 iNewScale = 0;
    ECode ec = SafeLongToInt(newScale, &iNewScale);
    if (FAILED(ec)) return ec;

    return CBigDecimal::New(strippedBI, iNewScale, result);
}

ECode CBigDecimal::CompareTo(
    /* [in] */ IInterface* other,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(other);
    VALIDATE_NOT_NULL(result);
    *result = -1;
    IBigDecimal* ival = (IBigDecimal*)other->Probe(EIID_IBigDecimal);
    if (ival == NULL) {
        return NOERROR;
    }

    CBigDecimal* val = (CBigDecimal*)ival;

    Int32 thisSign, valueSign;
    GetSignum(&thisSign);
    val->GetSignum(&valueSign);

    if( thisSign == valueSign) {
        if(mScale == val->mScale && mBitLength<64 && val->mBitLength<64 ) {
            *result = (mSmallValue < val->mSmallValue) ? -1 : (mSmallValue > val->mSmallValue) ? 1 : 0;
            return NOERROR;
        }
        Int64 diffScale = (Int64)mScale - val->mScale;
        Int32 diffPrecision = ApproxPrecision() - val->ApproxPrecision();
        if (diffPrecision > diffScale + 1) {
            *result = thisSign;
            return NOERROR;
        }
        else if (diffPrecision < diffScale - 1) {
            *result = -thisSign;
            return NOERROR;
        }
        else {// thisSign == val.signum()  and  diffPrecision is aprox. diffScale
            AutoPtr<IBigInteger> thisUnscaled = GetUnscaledValue();
            AutoPtr<IBigInteger> valUnscaled = val->GetUnscaledValue();
            // If any of both precision is bigger, append zeros to the shorter one
            if (diffScale < 0) {
                AutoPtr<IBigInteger> pbi, temp;
                FAIL_RETURN(Multiplication::PowerOf10(-diffScale, (IBigInteger**)&pbi));
                thisUnscaled->Multiply(pbi, (IBigInteger**)&temp);
                thisUnscaled = temp;
            }
            else if (diffScale > 0) {
                AutoPtr<IBigInteger> pbi, temp;
                FAIL_RETURN(Multiplication::PowerOf10(diffScale, (IBigInteger**)&pbi));
                valUnscaled->Multiply(pbi, (IBigInteger**)&temp);
                valUnscaled = temp;
            }

            IComparable* comp = (IComparable*)thisUnscaled->Probe(EIID_IComparable);
            return comp->CompareTo(valUnscaled, result);
        }
    }
    else if (thisSign < valueSign) {
        *result = -1;
        return NOERROR;
    }
    else  {
        *result = 1;
        return NOERROR;
    }
}

ECode CBigDecimal::Equals(
    /* [in] */ IInterface* x,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    VALIDATE_NOT_NULL(x);

    if (IBigDecimal::Probe(x) == NULL) return NOERROR;

    if ((IBigDecimal*)this == IBigDecimal::Probe(x)) {
        *result = TRUE;
        return NOERROR;
    }

    CBigDecimal* x1 = (CBigDecimal*)IBigDecimal::Probe(x);
    if (x1->mScale == mScale) {
        if (mBitLength < 64) {
            *result = (x1->mSmallValue == mSmallValue);
        }
        else {
            mIntVal->Equals(x1->mIntVal, result);
        }
        return NOERROR;
    }
    return NOERROR;
}

ECode CBigDecimal::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);
    *hashCode = mHashCode;
    if (mHashCode != 0) {
        return NOERROR;
    }
    if (mBitLength < 64) {
        mHashCode = (Int32)(mSmallValue & 0xffffffff);
        mHashCode = 33 * mHashCode +  (Int32)((mSmallValue >> 32) & 0xffffffff);
        mHashCode = 17 * mHashCode + mScale;
    }
    else {
        Int32 intValHashCode;
        IObject* o = (IObject*)mIntVal->Probe(EIID_IObject);
        o->GetHashCode(&intValHashCode);
        mHashCode = 17 * intValHashCode + mScale;
    }

    *hashCode = mHashCode;
    return NOERROR;
}

ECode CBigDecimal::Min(
    /* [in] */ IBigDecimal* val,
    /* [out] */ IBigDecimal** result)
{
    VALIDATE_NOT_NULL(val);
    VALIDATE_NOT_NULL(result);

    Int32 cmp = 0;
    CompareTo(val, &cmp);
    if (cmp <= 0) {
        *result = (IBigDecimal*)this;
    }
    else {
        *result = val;
    }

    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CBigDecimal::Max(
    /* [in] */ IBigDecimal* val,
    /* [out] */ IBigDecimal** result)
{
    VALIDATE_NOT_NULL(val);
    VALIDATE_NOT_NULL(result);

    Int32 cmp = 0;
    CompareTo(val, &cmp);
    if (cmp >= 0) {
        *result = (IBigDecimal*)this;
    }
    else {
        *result = val;
    }

    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CBigDecimal::ToString(
    /* [out] */ String*  str)
{
    VALIDATE_NOT_NULL(str);

    if (!mToStringImage.IsNull()) {
        *str = String(mToStringImage);
        return NOERROR;
    }

    if (mBitLength < 32) {
        mToStringImage = Conversion::ToDecimalScaledString(mSmallValue, mScale);
        *str = String(mToStringImage);
        return NOERROR;
    }

    AutoPtr<IBigInteger> thisUnscaled = GetUnscaledValue();
    String intString;
    IObject* o = (IObject*)thisUnscaled->Probe(EIID_IObject);
    o->ToString(&intString);
    if (mScale == 0) {
        *str = String(intString);
       return NOERROR;
    }

    Int32 thisSign = 0;
    thisUnscaled->GetSignum(&thisSign);
    Int32 begin = (thisSign < 0) ? 2 : 1;
    Int32 end = intString.GetLength();
    Int64 exponent = -(Int64)mScale + end - begin;
    StringBuilder result;

    result.Append(intString);
    if ((mScale > 0) && (exponent >= -6)) {
        if (exponent >= 0) {
            result.InsertChar(end - mScale, '.');
        } else {
            AutoPtr<ArrayOf<Char32> > array = ArrayOf<Char32>::Alloc(2);
            (*array)[0] = '0';
            (*array)[1] = '.';
            result.Insert(begin - 1, *(ArrayOf<Char32>*)array);
            result.Insert(begin + 1, *(ArrayOf<Char32>*)CharZeros, 0, -(Int32)exponent - 1);
        }
    } else {
        if (end - begin >= 1) {
            result.InsertChar(begin, '.');
            end++;
        }
        result.InsertChar(end, 'E');
        if (exponent > 0) {
            result.InsertChar(++end, '+');
        }
        result.Insert(++end, StringUtils::ToString(exponent));
    }

    mToStringImage = result.ToString();
    *str = mToStringImage;
   return NOERROR;
}

ECode CBigDecimal::ToEngineeringString(
    /* [out] */ String * representation)
{
    VALIDATE_NOT_NULL(representation);

    AutoPtr<IBigInteger> thisUnscaled = GetUnscaledValue();
    String intString;
    IObject* o = (IObject*)thisUnscaled->Probe(EIID_IObject);
    o->ToString(&intString);

    if (mScale == 0) {
        *representation = intString;
       return NOERROR;
    }

    Int32 thisSign = 0;
    thisUnscaled->GetSignum(&thisSign);
    Int32 begin = (thisSign < 0) ? 2 : 1;
    Int32 end = intString.GetLength();
    Int64 exponent = -(Int64)mScale + end - begin;
    StringBuilder result(intString);

    if ((mScale > 0) && (exponent >= -6)) {
        if (exponent >= 0) {
            result.InsertChar(end - mScale, '.');
        }
        else {
            AutoPtr<ArrayOf<Char32> > array = ArrayOf<Char32>::Alloc(2);
            (*array)[0] = '0';
            (*array)[1] = '.';
            result.Insert(begin - 1, *(ArrayOf<Char32>*)array);
            result.Insert(begin + 1, *(ArrayOf<Char32>*)CharZeros, 0, -(Int32)exponent - 1);
        }
    }
    else {
        Int32 delta = end - begin;
        Int32 rem = (Int32)(exponent % 3);

        if (rem != 0) {
            // adjust exponent so it is a multiple of three
            if (thisSign == 0) {
                // zero value
                rem = (rem < 0) ? -rem : 3 - rem;
                exponent += rem;
            }
            else {
                // nonzero value
                rem = (rem < 0) ? rem + 3 : rem;
                exponent -= rem;
                begin += rem;
            }

            if (delta < 3) {
                for (Int32 i = rem - delta; i > 0; i--) {
                    result.InsertChar(end++, '0');
                }
            }
        }
        if (end - begin >= 1) {
            result.InsertChar(begin, '.');
            end++;
        }
        if (exponent != 0) {
            result.InsertChar(end, 'E');
            if (exponent > 0) {
                result.InsertChar(++end, '+');
            }
            result.Insert(++end, StringUtils::ToString(exponent));
        }
    }

    result.ToString(representation);
    return NOERROR;
}

ECode CBigDecimal::ToPlainString(
    /* [out] */ String* representation)
{
    VALIDATE_NOT_NULL(representation);

    AutoPtr<IBigInteger> thisUnscaled = GetUnscaledValue();
    String intStr;
    IObject* o = (IObject*)thisUnscaled->Probe(EIID_IObject);
    o->ToString(&intStr);

    if ((mScale == 0) || ((IsZero()) && (mScale < 0))) {
        *representation = intStr;
        return NOERROR;
    }

    using Elastos::Core::Math;

    Int32 thisSign = 0;
    GetSignum(&thisSign);

    Int32 begin = (thisSign < 0) ? 1 : 0;
    Int32 delta = mScale;
    // We take space for all digits, plus a possible decimal point, plus 'scale'
    StringBuilder result(intStr.GetByteLength() + 1 + Math::Abs(mScale));

    if (begin == 1) {
        // If the number is negative, we insert a '-' character at front
        result.AppendChar('-');
    }
    if (mScale > 0) {
        delta -= (intStr.GetLength() - begin);
        if (delta >= 0) {
            result.Append("0.");
            // To append zeros after the decimal point
            for (; delta > CharZerosLength; delta -= CharZerosLength) {
                result.Append(*(ArrayOf<Char32>*)CharZeros);
            }
            result.Append(*(ArrayOf<Char32>*)CharZeros, 0, delta);

            String subStr = intStr.Substring(begin);
            result.Append(subStr);
        } else {

            delta = begin - delta;
            String subStr = intStr.Substring(begin, delta);
            result.Append(subStr);
            result.AppendChar('.');
            subStr = intStr.Substring(delta);
            result.Append(subStr);
        }
    } else {// (mScale <= 0)
        String subStr = intStr.Substring(begin);
        result.Append(subStr);
        // To append trailing zeros
        for (; delta < -CharZerosLength; delta += CharZerosLength) {
            result.Append(*(ArrayOf<Char32>*)CharZeros);
        }
        result.Append(*(ArrayOf<Char32>*)CharZeros, 0, -delta);
    }

    result.ToString(representation);
    return NOERROR;
}

ECode CBigDecimal::ToBigInteger(
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IBigInteger> thisUnscaled = GetUnscaledValue();
    if ((mScale == 0) || (IsZero())) {
        *result = thisUnscaled;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
    else if (mScale < 0) {
        AutoPtr<IBigInteger> pbi;
        FAIL_RETURN(Multiplication::PowerOf10(-(Int64)mScale, (IBigInteger**)&pbi));
        return thisUnscaled->Multiply(pbi, result);
    }
    else {// (mScale > 0)
        AutoPtr<IBigInteger> pbi;
        FAIL_RETURN(Multiplication::PowerOf10(mScale, (IBigInteger**)&pbi));
        return thisUnscaled->Divide(pbi, result);
    }
}

ECode CBigDecimal::ToBigIntegerExact(
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IBigInteger> thisUnscaled = GetUnscaledValue();
    if ((mScale == 0) || (IsZero())) {
        *result = thisUnscaled;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
    else if (mScale < 0) {
        AutoPtr<IBigInteger> pbi;
        FAIL_RETURN(Multiplication::PowerOf10(-(Int64)mScale, (IBigInteger**)&pbi));
        return thisUnscaled->Multiply(pbi, result);
    }
    else {// (mScale > 0)
        Int32 lowestBit = 0;
        thisUnscaled->GetLowestSetBit(&lowestBit);

        // An optimization before do a heavy division
        if ((mScale > ApproxPrecision()) || (mScale > lowestBit)) {
            return E_ARITHMETIC_EXCEPTION;
        }

        AutoPtr<IBigInteger> pbi;
        FAIL_RETURN(Multiplication::PowerOf10(mScale, (IBigInteger**)&pbi));

        AutoPtr<ArrayOf<IBigInteger*> > integerAndFraction;
        thisUnscaled->DivideAndRemainder(pbi, (ArrayOf<IBigInteger*>**)&integerAndFraction);
        Int32 remSign = 0;
        (*integerAndFraction)[1]->GetSignum(&remSign);
        if (remSign != 0) {
            return E_ARITHMETIC_EXCEPTION;
        }

        *result = (*integerAndFraction)[0];
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
}

ECode CBigDecimal::Int64Value(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    /*
     * If mScale <= -64 there are at least 64 trailing bits zero in
     * 10^(-scale). If the scale is positive and very large the Int64 value
     * could be zero.
     */
    if ((mScale <= -64) || (mScale > ApproxPrecision())) {
        *result = 0LL;
        return NOERROR;
    }
    else {
        AutoPtr<IBigInteger> bi;
        ToBigInteger((IBigInteger**)&bi);
        INumber* number = (INumber*)bi->Probe(EIID_INumber);
        return number->Int64Value(result);
    }
}

ECode CBigDecimal::ValueExact(
    /* [in] */ Int32 bitLengthOfType,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IBigInteger> bigInteger;
    FAIL_RETURN(ToBigIntegerExact((IBigInteger**)&bigInteger));

    Int32 bitLen = 0;
    bigInteger->BitLength(&bitLen);
    if (bitLen < bitLengthOfType) {
        // It fits in the primitive type
        INumber* number = (INumber*)bigInteger->Probe(EIID_INumber);
        return number->Int64Value(result);
    }

    return E_ARITHMETIC_EXCEPTION;
}

ECode CBigDecimal::Int64ValueExact(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    return ValueExact(64, result);
}

ECode CBigDecimal::Int32Value(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    /*
     * If scale <= -32 there are at least 32 trailing bits zero in
     * 10^(-scale). If the scale is positive and very large the Int64 value
     * could be zero.
     */
    if ((mScale <= -32) || (mScale > ApproxPrecision())) {
        *result = 0;
        return NOERROR;
    }
    else {
        AutoPtr<IBigInteger> bi;
        ToBigInteger((IBigInteger**)&bi);
        INumber* number = (INumber*)bi->Probe(EIID_INumber);
        return number->Int32Value(result);
    }
}

ECode CBigDecimal::Int32ValueExact(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    *result = 0;
    Int64 val;
    ECode ec = ValueExact(32, &val);
    if (FAILED(ec)) return ec;
    *result = (Int32)val;
    return NOERROR;
}

ECode CBigDecimal::Int16Value(
    /* [out] */ Int16* result)
{
    VALIDATE_NOT_NULL(result);

    if ((mScale <= -16) || (mScale > ApproxPrecision())) {
        *result = 0;
        return NOERROR;
    }
    else {
        AutoPtr<IBigInteger> bi;
        ToBigInteger((IBigInteger**)&bi);
        INumber* number = (INumber*)bi->Probe(EIID_INumber);
        return number->Int16Value(result);
    }
}

ECode CBigDecimal::Int16ValueExact(
    /* [out] */ Int16* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;
    Int64 val;
    ECode ec = ValueExact(16, &val);
    if (FAILED(ec)) return ec;
    *result = (Int16)val;
    return NOERROR;
}

ECode CBigDecimal::ByteValue(
    /* [out] */ Byte* result)
{
    VALIDATE_NOT_NULL(result);

    if ((mScale <= -8) || (mScale > ApproxPrecision())) {
        *result = 0;
        return NOERROR;
    }
    else {
        AutoPtr<IBigInteger> bi;
        ToBigInteger((IBigInteger**)&bi);
        INumber* number = (INumber*)bi->Probe(EIID_INumber);
        return number->ByteValue(result);
    }

    return NOERROR;
}

ECode CBigDecimal::ByteValueExact(
    /* [out] */ Byte* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;
    Int64 val;
    ECode ec = ValueExact(8, &val);
    if (FAILED(ec)) return ec;
    *result = (Byte)val;
    return NOERROR;
    return E_NOT_IMPLEMENTED;
}

ECode CBigDecimal::FloatValue(
    /* [out] */  Float* result)
{
    VALIDATE_NOT_NULL(result);

    /* A similar code like in doubleValue() could be repeated here,
     * but this simple implementation is quite efficient. */
    Int32 sign;
    GetSignum(&sign);
    Float floatResult = sign;

    using Elastos::Core::Math;
    Int64 powerOfTwo = mBitLength - (Int64)(mScale / Log10_2);
    if ((powerOfTwo < -149) || (floatResult == 0.0f)) {
        // Cases which 'this' is very small
        floatResult *= 0.0f;
    }
    else if (powerOfTwo > 129) {
        // Cases which 'this' is very large
        floatResult *= Math::FLOAT_POSITIVE_INFINITY;
    }
    else {
        Double val;
        DoubleValue(&val);
        floatResult = (Float)val;
    }

    *result = floatResult;
    return NOERROR;
}

ECode CBigDecimal::DoubleValue(
    /* [out] */  Double* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 sign;
    GetSignum(&sign);
    Int32 exponent = 1076; // bias + 53
    Int32 lowestSetBit;
    Int32 discardedSize;
    Int64 powerOfTwo = mBitLength - (Int64)(mScale / Log10_2);
    Int64 bits; // IEEE-754 Standard
    Int64 tempBits; // for temporal calculations

    using Elastos::Core::Math;

    if ((powerOfTwo < -1074) || (sign == 0)) {
        // Cases which 'this' is very small
        *result = (sign * 0.0d);
        return NOERROR;
    }
    else if (powerOfTwo > 1025) {
        // Cases which 'this' is very large
        *result = (sign * Math::DOUBLE_POSITIVE_INFINITY);
        return NOERROR;
    }

    AutoPtr<IBigInteger> mantissa;
    AutoPtr<IBigInteger> ubi = GetUnscaledValue();
    ubi->Abs((IBigInteger**)&mantissa);

    // Let be:  this = [u,s], with s > 0
    if (mScale <= 0) {
        AutoPtr<IBigInteger> pbi, temp;
        FAIL_RETURN(Multiplication::PowerOf10(-mScale, (IBigInteger**)&pbi));
        // mantissa = abs(u) * 10^s
        mantissa->Multiply(pbi, (IBigInteger**)&temp);
        mantissa = temp;
    }
    else {// (mScale > 0)
        AutoPtr<IBigInteger> powerOfTen, temp;
        FAIL_RETURN(Multiplication::PowerOf10(mScale, (IBigInteger**)&powerOfTen));
        Int32 k = 100 - (Int32)powerOfTwo;
        Int32 compRem;

        if (k > 0) {
            /* Computing (mantissa * 2^k) , where 'k' is a enough big
             * power of '2' to can divide by 10^s */
             mantissa->ShiftLeft(k, (IBigInteger**)&temp);
             mantissa = temp;
             temp = NULL;
             exponent -= k;
        }

        AutoPtr<ArrayOf<IBigInteger*> > quotAndRem;
        // Computing (mantissa * 2^k) / 10^s
        mantissa->DivideAndRemainder(powerOfTen, (ArrayOf<IBigInteger*>**)&quotAndRem);
        (*quotAndRem)[1]->ShiftLeftOneBit((IBigInteger**)&temp);

        // To check if the fractional part >= 0.5
        IComparable* comp = (IComparable*)temp->Probe(EIID_IComparable);
        comp->CompareTo(powerOfTen, &compRem);
        temp = NULL;

        AutoPtr<IBigInteger> abi, cbi;
        CBigInteger::ValueOf((compRem * (compRem + 3)) / 2 + 1, (IBigInteger**)&cbi);

        // To add two rounded bits at end of mantissa
        ((*quotAndRem)[0])->ShiftLeft(2, (IBigInteger**)&temp);
        temp->Add(cbi, (IBigInteger**)&abi);
        mantissa = abi;
        exponent -= 2;
    }

    Int32 bitLen = 0;
    mantissa->BitLength(&bitLen);
    mantissa->GetLowestSetBit(&lowestSetBit);
    discardedSize = bitLen - 54;

    if (discardedSize > 0) {// (n > 54)
        AutoPtr<IBigInteger> srbi;
        mantissa->ShiftRight(discardedSize, (IBigInteger**)&srbi);
        // mantissa = (abs(u) * 10^s) >> (n - 54)
        INumber* number = (INumber*)srbi->Probe(EIID_INumber);
        number->Int64Value(&bits);
        tempBits = bits;
        // #bits = 54, to check if the discarded fraction produces a carry
        if ((((bits & 1) == 1) && (lowestSetBit < discardedSize))
                || ((bits & 3) == 3)) {
            bits += 2;
        }
    }
    else {// (n <= 54)
        // mantissa = (abs(u) * 10^s) << (54 - n)
        Int64 lval;
        INumber* number = (INumber*)mantissa->Probe(EIID_INumber);
        number->Int64Value(&lval);
        bits = lval << (-discardedSize & 0x3F);
        tempBits = bits;
        // #bits = 54, to check if the discarded fraction produces a carry:
        if ((bits & 3) == 3) {
            bits += 2;
        }
    }
    // Testing bit 54 to check if the carry creates a new binary digit
    if ((bits & 0x40000000000000LL) == 0) {
        // To drop the last bit of mantissa (first discarded)
        bits >>= 1;
        // exponent = 2^(s-n+53+bias)
        exponent += discardedSize;
    }
    else {// #bits = 54
        bits >>= 2;
        exponent += discardedSize + 1;
    }
    // To test if the 53-bits number fits in 'double'
    if (exponent > 2046) {// (exponent - bias > 1023)
        *result = (sign * Math::DOUBLE_POSITIVE_INFINITY);
        return NOERROR;
    }
    else if (exponent <= 0) {// (exponent - bias <= -1023)
        // Denormalized numbers (having exponent == 0)
        if (exponent < -53) {// exponent - bias < -1076
            *result = (sign * 0.0d);
            return NOERROR;
        }

        // -1076 <= exponent - bias <= -1023
        // To discard '- exponent + 1' bits
        bits = tempBits >> 1;
        tempBits = bits & (((UInt64)-1LL) >> ((63 + exponent) & 0x3F));
        bits >>= (-exponent & 0x3F) ;
        // To test if after discard bits, a new carry is generated
        if (((bits & 3) == 3) || (((bits & 1) == 1) && (tempBits != 0)
        && (lowestSetBit < discardedSize))) {
            bits += 1;
        }
        exponent = 0;
        bits >>= 1;
    }
    // Construct the 64 double bits: [sign(1), exponent(11), mantissa(52)]
    bits = (sign & 0x8000000000000000LL) | ((Int64)exponent << 52)
            | (bits & 0xFFFFFFFFFFFFFLL);

    *result = Math::Int64BitsToDouble(bits);
    return NOERROR;
}

ECode CBigDecimal::Ulp(
    /* [out] */ IBigDecimal** newObj)
{
    VALIDATE_NOT_NULL(newObj);
    return ValueOf(1, mScale, newObj);
}

Int32 CBigDecimal::BitLength(
    /* [in] */ Int32 smallValue)
{
    if (smallValue < 0) {
        smallValue = ~smallValue;
    }
    using Elastos::Core::Math;
    return 32 - Math::NumberOfLeadingZeros(smallValue);
}

Int32 CBigDecimal::BitLength(
    /* [in] */ Int64 smallValue)
{
    if(smallValue < 0) {
        smallValue = ~smallValue;
    }
    using Elastos::Core::Math;
    return 64 - Math::NumberOfLeadingZeros(smallValue);
}

template <typename T>
static Int32 BinarySearch(T const * array, Int32 length, T const& key)
{
    assert(array && length >= 0);

    Int32 low = 0;
    Int32 high = length - 1;

    while (low <= high) {
        Int32 mid = ((UInt32)(low + high)) >> 1;

        if (array[mid] < key) {
            low = mid + 1;
        }
        else if (array[mid] > key) {
            high = mid - 1;
        }
        else {
            return mid;
        }
    }

    return ~low;
}

Int32 CBigDecimal::DecimalDigitsInLong(
    /* [in] */ Int64 value)
{
    using Elastos::Core::Math;
    if (value == Math::INT64_MIN_VALUE) {
        return 19; // special case required because abs(MIN_VALUE) == MIN_VALUE
    }
    else {
        Int64 absVal = Math::Abs(value);
        Int32 index = BinarySearch<Int64>(Math::LongPowersOfTen, Math::LongPowersOfTenLength, absVal);
        return (index < 0) ? (-index - 1) : (index + 1);
    }
}

Boolean CBigDecimal::IsZero()
{
    //Watch out: -1 has a bitLength=0
    return mBitLength == 0 && mSmallValue != -1;
}

void CBigDecimal::SetUnscaledValue(
    /* [in] */ IBigInteger* unscalevalue)
{
    mIntVal = unscalevalue;
    unscalevalue->BitLength(&mBitLength);
    if (mBitLength < 64) {
        INumber* number = (INumber*)unscalevalue->Probe(EIID_INumber);
        number->Int64Value(&mSmallValue);
    }
}

ECode CBigDecimal::GetUnscaledValue(
    /**/ IBigInteger** value)
{
    VALIDATE_NOT_NULL(value)
    AutoPtr<IBigInteger> bi = GetUnscaledValue();
    *value = bi;
    REFCOUNT_ADD(*value)
    return NOERROR;
}

AutoPtr<IBigInteger> CBigDecimal::GetUnscaledValue()
{
    if (mIntVal.Get() == NULL) {
        CBigInteger::ValueOf(mSmallValue, (IBigInteger**)&mIntVal);
    }

    return mIntVal;
}

Int32 CBigDecimal::ApproxPrecision()
{
    return mPrecision > 0
            ? mPrecision
            : (Int32) ((mBitLength - 1) * Log10_2) + 1;
}

ECode CBigDecimal::SafeLongToInt(
    /* [in] */ Int64 longValue,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    using Elastos::Core::Math;
    if (longValue < Math::INT32_MIN_VALUE || longValue > Math::INT32_MAX_VALUE) {
        return E_ARITHMETIC_EXCEPTION;
    }

    *result = (Int32)longValue;
    return NOERROR;
}

ECode CBigDecimal::ZeroScaledByScale(
    /* [in] */ Int64 longScale,
    /* [out] */ IBigDecimal** result)
{
    VALIDATE_NOT_NULL(result)
    if (longScale == (Int32)longScale) {
        return ValueOf(0, (Int32)longScale, result);
    }

    using Elastos::Core::Math;
    if (longScale >= 0) {
        return CBigDecimal::New( 0, Math::INT32_MAX_VALUE, result);
    }

    return CBigDecimal::New(0, Math::INT32_MAX_VALUE, result);
}

ECode CBigDecimal::InplaceRound(
    /* [in] */ IMathContext* mc)
{
    Int32 mcPrecision;
    mc->GetPrecision(&mcPrecision);
    if (ApproxPrecision() < mcPrecision || mcPrecision == 0) {
        return NOERROR;
    }

    Int32 thisPrecision;
    GetPrecision(&thisPrecision);
    Int32 discardedPrecision = thisPrecision - mcPrecision;
    // If no rounding is necessary it returns immediately
    if ((discardedPrecision <= 0)) {
        return NOERROR;
    }
    // When the number is small perform an efficient rounding
    if (mBitLength < 64) {
        SmallRound(mc, discardedPrecision);
        return NOERROR;
    }

    AutoPtr<ArrayOf<IBigInteger*> > integerAndFraction;
    AutoPtr<IBigInteger> ubi = GetUnscaledValue();
    // Getting the integer part and the discarded fraction
    AutoPtr<IBigInteger> sizeOfFraction;
    FAIL_RETURN(Multiplication::PowerOf10(discardedPrecision, (IBigInteger**)&sizeOfFraction));
    ubi->DivideAndRemainder(sizeOfFraction, (ArrayOf<IBigInteger*>**)&integerAndFraction);
    Int64 newScale = (Int64)mScale - discardedPrecision;

    AutoPtr<IBigInteger> dbi = (*integerAndFraction)[0];
    AutoPtr<IBigInteger> rbi = (*integerAndFraction)[1];
    Int32 rSignum;
    rbi->GetSignum(&rSignum);

    // If the discarded fraction is non-zero, perform rounding
    if (rSignum != 0) {
        AutoPtr<IBigInteger> absBI, shiftBI;
        rbi->Abs((IBigInteger**)&absBI);
        absBI->ShiftLeftOneBit((IBigInteger**)&shiftBI);

        // To check if the discarded fraction >= 0.5
        Int32 compRem;
        IComparable* comp = (IComparable*)shiftBI->Probe(EIID_IComparable);
        comp->CompareTo((IBigInteger*)sizeOfFraction, &compRem);
        // To look if there is a carry
        Boolean testBit;
        dbi->TestBit(0, &testBit);
        RoundingMode mode;
        mc->GetRoundingMode(&mode);

        Int32 parityBit = testBit ? 1 : 0;
        ECode ec = RoundingBehavior(parityBit, rSignum * (5 + compRem), mode, &compRem);
        if (FAILED(ec)) return ec;

        if (compRem != 0) {
            AutoPtr<IBigInteger> crBI, aBI;
            CBigInteger::ValueOf(compRem, (IBigInteger**)&crBI);
            dbi->Add(crBI, (IBigInteger**)&aBI);
            integerAndFraction->Set(0, aBI);
            dbi = (*integerAndFraction)[0];
        }

        AutoPtr<IBigDecimal> tempBD;
        CBigDecimal::New(dbi, (IBigDecimal**)&tempBD);

        Int32 tp;
        tempBD->GetPrecision(&tp);
        // If after to add the increment the precision changed, we normalize the size
        if (tp > mcPrecision) {
            AutoPtr<IBigInteger> resultBI;
            dbi->Divide(CBigInteger::TEN, (IBigInteger**)&resultBI);
            integerAndFraction->Set(0, resultBI);
            dbi = (*integerAndFraction)[0];
            newScale--;
        }
    }

    // To update all internal fields
    FAIL_RETURN(SafeLongToInt(newScale, &mScale));

    mPrecision = mcPrecision;
    SetUnscaledValue(dbi);
    return NOERROR;
}

ECode CBigDecimal::SmallRound(
    /* [in] */ IMathContext* mc,
    /* [in] */ Int32 discardedPrecision)
{
    using Elastos::Core::Math;

    Int64 sizeOfFraction = Math::LongPowersOfTen[discardedPrecision];
    Int64 newScale = (Int64)mScale - discardedPrecision;
    Int64 unscaledVal = mSmallValue;
    // Getting the integer part and the discarded fraction
    Int64 integer = unscaledVal / sizeOfFraction;
    Int64 fraction = unscaledVal % sizeOfFraction;
    Int32 compRem;

    RoundingMode mcMode;
    mc->GetRoundingMode(&mcMode);
    Int32 mcPrecision;
    mc->GetPrecision(&mcPrecision);

    // If the discarded fraction is non-zero perform rounding
    if (fraction != 0) {
        // To check if the discarded fraction >= 0.5
        compRem = LongCompareTo(Math::Abs(fraction) * 2, sizeOfFraction);
        Int32 rbResult = 0;
        ECode ec = RoundingBehavior(
                ((Int32)integer) & 1,
                Math::Signum(fraction) * (5 + compRem),
                mcMode, &rbResult);
        if (FAILED(ec)) return ec;

        // To look if there is a carry
        integer += rbResult;
        // If after to add the increment the precision changed, we normalize the size
        if (Math::Log10(Math::Abs(integer)) >= mcPrecision) {
            integer /= 10;
            newScale--;
        }
    }
    // To update all internal fields
    ECode ec = SafeLongToInt(newScale, &mScale);
    if (FAILED(ec)) return ec;

    mPrecision = mcPrecision;
    mSmallValue = integer;
    mBitLength = BitLength(integer);
    mIntVal = NULL;

    return NOERROR;
}

ECode CBigDecimal::RoundingBehavior(
    /* [in] */ Int32 parityBit,
    /* [in] */ Int32 fraction,
    /* [in] */ RoundingMode roundingMode,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0; // the carry after rounding

    using Elastos::Core::Math;
    Int32 signum = Math::Signum(fraction);

    switch (roundingMode) {
        case RoundingMode_UNNECESSARY:
            if (fraction != 0) {
                return E_ARITHMETIC_EXCEPTION;
            }
            break;
        case RoundingMode_UP:
            *result = signum;
            break;
        case RoundingMode_DOWN:
            break;
        case RoundingMode_CEILING:
            *result = Math::Max(signum, 0);
            break;
        case RoundingMode_FLOOR:
            *result = Math::Min(signum, 0);
            break;
        case RoundingMode_HALF_UP:
            if (Math::Abs(fraction) >= 5) {
                *result = signum;
            }
            break;
        case RoundingMode_HALF_DOWN:
            if (Math::Abs(fraction) > 5) {
                *result = signum;
            }
            break;
        case RoundingMode_HALF_EVEN:
            if (Math::Abs(fraction) + parityBit > 5) {
                *result = signum;
            }
            break;
    }

    return NOERROR;
}

} // namespace Math
} // namespace Elastos
