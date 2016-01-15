
#include <BigInt.h>
#include <NativeBN.h>
#include "Character.h"

using Elastos::Core::Character;

namespace Elastos {
namespace Math {

BigInt::BigInt()
    : mBignum(0)
{
}

BigInt::~BigInt()
{
    if (mBignum != 0) {
        NativeBN::BN_free(mBignum);
        mBignum = 0;
    }
}

ECode BigInt::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = DecString();
    return NOERROR;
}

String BigInt::ToString()
{
    return DecString();
}

Int64 BigInt::GetNativeBIGNUM()
{
    return mBignum;
}

ECode BigInt::MakeValid()
{
    if (mBignum == 0) {
        mBignum = NativeBN::BN_new();
    }
    return NOERROR;
}

ECode BigInt::NewBigInt(
    /* [in] */ BigInt& bi)
{
    bi.mBignum = NativeBN::BN_new();
    return NOERROR;
}

Int32 BigInt::Cmp(
    /* [in] */ const BigInt& a,
    /* [in] */ const BigInt& b)
{
    return NativeBN::BN_cmp(a.mBignum, b.mBignum);
}

ECode BigInt::PutCopy(
    /* [in] */ const BigInt& from)
{
    FAIL_RETURN(MakeValid());
    NativeBN::BN_copy(mBignum, from.mBignum);
    return NOERROR;
}

ECode BigInt::Copy(
    /* [in] */ BigInt& bi)
{
    return bi.PutCopy(*this);
}

ECode BigInt::PutLongInt(
    /* [in] */ Int64 val)
{
    FAIL_RETURN(MakeValid());
    NativeBN::PutLongInt(mBignum, val);
    return NOERROR;
}

ECode BigInt::PutULongInt(
    /* [in] */ Int64 val,
    /* [in] */ Boolean neg)
{
    FAIL_RETURN(MakeValid());
    NativeBN::PutULongInt(mBignum, val, neg);
    return NOERROR;
}

//
//private NumberFormatException invalidBigInteger(String s) {
//    throw new NumberFormatException("Invalid BigInteger: " + s);
//}

ECode BigInt::PutDecString(
    /* [in] */ const String& original)
{
    String s;
    FAIL_RETURN(CheckString(original, 10, &s));
    FAIL_RETURN(MakeValid());
    Int32 usedLen = NativeBN::BN_dec2bn(mBignum, s);
    if (usedLen < s.GetByteLength()) {
        //throw invalidBigInteger(original);
        return E_NUMBER_FORMAT_EXCEPTION;
    }
    return NOERROR;
}

ECode BigInt::PutHexString(
    /* [in] */ const String& original)
{
    String s;
    FAIL_RETURN(CheckString(original, 16, &s));
    FAIL_RETURN(MakeValid());
    Int32 usedLen = NativeBN::BN_hex2bn(mBignum, s);
    if (usedLen < s.GetByteLength()) {
        //throw invalidBigInteger(original);
        return E_NUMBER_FORMAT_EXCEPTION;
    }
    return NOERROR;
}

/**
 * Returns a string suitable for passing to OpenSSL.
 * Throws if 's' doesn't match Java's rules for valid BigInteger strings.
 * BN_dec2bn and BN_hex2bn do very little checking, so we need to manually
 * ensure we comply with Java's rules.
 * http://code.google.com/p/android/issues/detail?id=7036
 */
ECode BigInt::CheckString(
    /* [in] */ const String& original,
    /* [in] */ Int32 base,
    /* [in] */ String* result)
{
    if (original.IsNull()) {
        //throw new NullPointerException("s == null");
        return E_NULL_POINTER_EXCEPTION;
    }

    String s(original);
    // A valid big integer consists of an optional '-' or '+' followed by
    // one or more digit characters appropriate to the given base,
    // and no other characters.
    AutoPtr<ArrayOf<Char32> > charArray = s.GetChars();
    Int32 charCount = charArray->GetLength();
    Int32 i = 0;
    Char32 ch;
    if (charCount > 0) {
        ch = (*charArray)[0];
        if (ch == '+') {
            // Java supports leading +, but OpenSSL doesn't, so we need to strip it.
            s = s.Substring(1);
            --charCount;
        } else if (ch == '-') {
            ++i;
        }
    }
    if (charCount - i == 0) {
        //throw invalidBigInteger(s);
        return E_NUMBER_FORMAT_EXCEPTION;
    }
    Boolean nonAscii = FALSE;
    for (; i < charCount; ++i) {
        ch = (*charArray)[i];
        if (Character::ToDigit(ch, base) == -1) {
            //throw invalidBigInteger(s);
            return E_NUMBER_FORMAT_EXCEPTION;
        }
        if (ch > 128) {
            nonAscii = TRUE;
        }
    }

    *result = nonAscii ? ToAscii(s, base) : s;
    return NOERROR;
}


// Java supports non-ASCII decimal digits, but OpenSSL doesn't.
// We need to translate the decimal digits but leave any other characters alone.
// This method assumes it's being called on a string that has already been validated.
String BigInt::ToAscii(
    /* [in] */ const String& s,
    /* [in] */ Int32 base)
{
    AutoPtr<ArrayOf<Char32> > charArray = s.GetChars();
    Int32 length = charArray->GetLength();
    Char32 ch;
    StringBuilder result(length);
    for (Int32 i = 0; i < length; ++i) {
        ch = (*charArray)[i];
        Int32 value = Character::ToDigit(ch, base);
        if (value >= 0 && value <= 9) {
            ch = (char) ('0' + value);
        }
        result.AppendChar(ch);
    }

    return result.ToString();
}

ECode BigInt::PutBigEndian(
    /* [in] */ const ArrayOf<Byte>& bytes,
    /* [in] */ Boolean neg)
{
    FAIL_RETURN(MakeValid());
    NativeBN::BN_bin2bn(bytes, bytes.GetLength(), neg, mBignum);
    return NOERROR;
}

ECode BigInt::PutLittleEndianInts(
    /* [in] */ const ArrayOf<Int32>& ints,
    /* [in] */ Boolean neg)
{
    FAIL_RETURN(MakeValid());
    NativeBN::LitEndInts2bn(ints, ints.GetLength(), neg, mBignum);
    return NOERROR;
}

ECode BigInt::PutBigEndianTwosComplement(
    /* [in] */ const ArrayOf<Byte>& bytes)
{
    FAIL_RETURN(MakeValid());
    NativeBN::TwosComp2bn(bytes, bytes.GetLength(), mBignum);
    return NOERROR;
}

Int64 BigInt::LongInt()
{
    return NativeBN::LongInt(mBignum);
}

String BigInt::DecString()
{
    return NativeBN::BN_bn2dec(mBignum);
}

String BigInt::HexString()
{
    return NativeBN::BN_bn2hex(mBignum);
}

AutoPtr<ArrayOf<Byte> > BigInt::BigEndianMagnitude()
{
    return NativeBN::BN_bn2bin(mBignum);
}

AutoPtr<ArrayOf<Int32> > BigInt::LittleEndianIntsMagnitude()
{
    return NativeBN::Bn2litEndInts(mBignum);
}

Int32 BigInt::Sign()
{
    return NativeBN::Sign(mBignum);
}

void BigInt::SetSign(
    /* [in] */ Int32 val)
{
    if (val > 0) {
        NativeBN::BN_set_negative(mBignum, 0);
    }
    else {
        if (val < 0) {
            NativeBN::BN_set_negative(mBignum, 1);
        }
    }
}

Boolean BigInt::TwosCompFitsIntoBytes(
    /* [in] */ Int32 desiredByteCount)
{
    Int32 actualByteCount = (NativeBN::BitLength(mBignum) + 7) / 8;
    return actualByteCount <= desiredByteCount;
}

Int32 BigInt::BitLength()
{
    return NativeBN::BitLength(mBignum);
}

Boolean BigInt::IsBitSet(
    /* [in] */ Int32 n)
{
    return NativeBN::BN_is_bit_set(mBignum, n);
}

// n > 0: shift left (multiply)
ECode BigInt::Shift(
    /* [in] */ const BigInt& a,
    /* [in] */ Int32 n,
    /* [in] */ BigInt& r)
{
    FAIL_RETURN(NewBigInt(r));
    NativeBN::BN_shift(r.mBignum, a.mBignum, n);
    return NOERROR;
}

ECode BigInt::Shift(
    /* [in] */ Int32 n)
{
    NativeBN::BN_shift(mBignum, mBignum, n);
    return NOERROR;
}

ECode BigInt::AddPositiveInt(
    /* [in] */ Int32 w)
{
    NativeBN::BN_add_word(mBignum, w);
    return NOERROR;
}

ECode BigInt::MultiplyByPositiveInt(
    /* [in] */ Int32 w)
{
    NativeBN::BN_mul_word(mBignum, w);
    return NOERROR;
}

ECode BigInt::RemainderByPositiveInt(
    /* [in] */ const BigInt& a,
    /* [in] */ Int32 w,
    /* [in] */ Int32& r)
{
    r = NativeBN::BN_mod_word(a.mBignum, w);
    return NOERROR;
}

ECode BigInt::Addition(
    /* [in] */ const BigInt& a,
    /* [in] */ const BigInt& b,
    /* [in] */ BigInt& r)
{
    FAIL_RETURN(NewBigInt(r));
    NativeBN::BN_add(r.mBignum, a.mBignum, b.mBignum);
    return NOERROR;
}

ECode BigInt::Add(
    /* [in] */ const BigInt& a)
{
    NativeBN::BN_add(mBignum, mBignum, a.mBignum);
    return NOERROR;
}

ECode BigInt::Subtraction(
    /* [in] */ const BigInt& a,
    /* [in] */ const BigInt& b,
    /* [in] */ BigInt& r)
{
    FAIL_RETURN(NewBigInt(r));
    NativeBN::BN_sub(r.mBignum, a.mBignum, b.mBignum);
    return NOERROR;
}

ECode BigInt::Gcd(
    /* [in] */ const BigInt& a,
    /* [in] */ const BigInt& b,
    /* [in] */ BigInt& r)
{
    FAIL_RETURN(NewBigInt(r));
    NativeBN::BN_gcd(r.mBignum, a.mBignum, b.mBignum);
    return NOERROR;
}

ECode BigInt::Product(
    /* [in] */ const BigInt& a,
    /* [in] */ const BigInt& b,
    /* [in] */ BigInt& r)
{
    FAIL_RETURN(NewBigInt(r));
    NativeBN::BN_mul(r.mBignum, a.mBignum, b.mBignum);
    return NOERROR;
}

ECode BigInt::BigExp(
    /* [in] */ const BigInt& a,
    /* [in] */ const BigInt& p,
    /* [in] */ BigInt& r)
{
    // Sign of p is ignored!
    FAIL_RETURN(NewBigInt(r));
    NativeBN::BN_exp(r.mBignum, a.mBignum, p.mBignum);
    return NOERROR;
}

ECode BigInt::Exp(
    /* [in] */ const BigInt& a,
    /* [in] */ Int32 p,
    /* [in] */ BigInt& r)
{
    // Sign of p is ignored!
    BigInt power;
    power.PutLongInt(p);
    return BigExp(a, power, r);
    // OPTIONAL:
    // Int32 BN_sqr(BigInteger r, BigInteger a, BN_CTX ctx);
    // Int32 BN_sqr(BIGNUM *r, const BIGNUM *a,BN_CTX *ctx);
}

ECode BigInt::Division(
    /* [in] */ const BigInt& dividend,
    /* [in] */ const BigInt& divisor,
    /* [in] */ BigInt* quotient,
    /* [in] */ BigInt* remainder)
{
    Int64 quot, rem;
    if (quotient != NULL) {
        FAIL_RETURN(quotient->MakeValid());
        quot = quotient->mBignum;
    }
    else {
        quot = 0;
    }
    if (remainder != NULL) {
        FAIL_RETURN(remainder->MakeValid());
        rem = remainder->mBignum;
    }
    else {
        rem = 0;
    }
    NativeBN::BN_div(quot, rem, dividend.mBignum, divisor.mBignum);
    return NOERROR;
}

ECode BigInt::Modulus(
    /* [in] */ const BigInt& a,
    /* [in] */ const BigInt& m,
    /* [in] */ BigInt& r)
{
    // Sign of p is ignored! ?
    FAIL_RETURN(NewBigInt(r));
    NativeBN::BN_nnmod(r.mBignum, a.mBignum, m.mBignum);
    return NOERROR;
}

ECode BigInt::ModExp(
    /* [in] */ const BigInt& a,
    /* [in] */ const BigInt& p,
    /* [in] */ const BigInt& m,
    /* [in] */ BigInt& r)
{
    // Sign of p is ignored!
    FAIL_RETURN(NewBigInt(r));
    NativeBN::BN_mod_exp(r.mBignum, a.mBignum, p.mBignum, m.mBignum);
    return NOERROR;
}

ECode BigInt::ModInverse(
    /* [in] */ const BigInt& a,
    /* [in] */ const BigInt& m,
    /* [in] */ BigInt& r)
{
    FAIL_RETURN(NewBigInt(r));
    NativeBN::BN_mod_inverse(r.mBignum, a.mBignum, m.mBignum);
    return NOERROR;
}

ECode BigInt::GeneratePrimeDefault(
    /* [in] */ Int32 bitLength,
    /* [in] */ BigInt& r)
{
    FAIL_RETURN(NewBigInt(r));
    NativeBN::BN_generate_prime_ex(r.mBignum, bitLength, FALSE, 0, 0, 0);
    return NOERROR;
}

Boolean BigInt::IsPrime(
    /* [in] */ Int32 certainty)
{
    return NativeBN::BN_is_prime_ex(mBignum, certainty, 0);
}

} // namespace Math
} // namespace Elastos
