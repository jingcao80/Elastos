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

#ifndef __ELASTOS_MATH_BIGINT_H__
#define __ELASTOS_MATH_BIGINT_H__

#include <elastos/core/StringBuilder.h>
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Math {

/*
 * In contrast to BigIntegers this class doesn't fake two's complement representation.
 * Any Bit-Operations, including Shifting, solely regard the unsigned magnitude.
 * Moreover BigInt objects are mutable and offer efficient in-place-operations.
 */
class BigInt
    : public Object
{
public:
    BigInt();

    ~BigInt();

    CARAPI ToString(
        /* [out] */ String* result);

    CARAPI_(String) ToString();

    CARAPI_(Int64) GetNativeBIGNUM();

    static CARAPI_(Int32) Cmp(
        /* [in] */ const BigInt& a,
        /* [in] */ const BigInt& b);

    CARAPI PutCopy(
        /* [in] */ const BigInt& from);

    CARAPI Copy(
        /* [in] */ BigInt& bigInt);

    CARAPI PutLongInt(
        /* [in] */ Int64 val);

    CARAPI PutULongInt(
        /* [in] */ Int64 val,
        /* [in] */ Boolean neg);

    CARAPI PutDecString(
        /* [in] */ const String& original);

    CARAPI PutHexString(
        /* [in] */ const String& original);

    /**
     * Returns a string suitable for passing to OpenSSL.
     * Throws if 's' doesn't match Java's rules for valid BigInteger strings.
     * BN_dec2bn and BN_hex2bn do very little checking, so we need to manually
     * ensure we comply with Java's rules.
     * http://code.google.com/p/android/issues/detail?id=7036
     */
    CARAPI CheckString(
        /* [in] */ const String& s,
        /* [in] */ Int32 base,
        /* [out] */ String* result);

    CARAPI PutBigEndian(
        /* [in] */ const ArrayOf<Byte>& bytes,
        /* [in] */ Boolean neg);

    CARAPI PutLittleEndianInts(
        /* [in] */ const ArrayOf<Int32>& ints,
        /* [in] */ Boolean neg);

    CARAPI PutBigEndianTwosComplement(
        /* [in] */ const ArrayOf<Byte>& bytes);

    Int64 LongInt();

    String DecString();

    String HexString();

    AutoPtr<ArrayOf<Byte> > BigEndianMagnitude();

    AutoPtr<ArrayOf<Int32> > LittleEndianIntsMagnitude();

    Int32 Sign();

    void SetSign(
        /* [in] */ Int32 val);

    Boolean TwosCompFitsIntoBytes(
        /* [in] */ Int32 desiredByteCount);

    Int32 BitLength();

    Boolean IsBitSet(
        /* [in] */ Int32 n);

    // n > 0: shift left (multiply)
    static CARAPI Shift(
        /* [in] */ const BigInt& a,
        /* [in] */ Int32 n,
        /* [in] */ BigInt& r);

    CARAPI Shift(
        /* [in] */ Int32 n);

    CARAPI AddPositiveInt(
        /* [in] */ Int32 w);

    CARAPI MultiplyByPositiveInt(
        /* [in] */ Int32 w);

    static CARAPI RemainderByPositiveInt(
        /* [in] */ const BigInt& a,
        /* [in] */ Int32 w,
        /* [in] */ Int32& r);

    static CARAPI Addition(
        /* [in] */ const BigInt& a,
        /* [in] */ const BigInt& b,
        /* [in] */ BigInt& r);

    CARAPI Add(
        /* [in] */ const BigInt& a);

    static CARAPI Subtraction(
        /* [in] */ const BigInt& a,
        /* [in] */ const BigInt& b,
        /* [in] */ BigInt& r);

    static CARAPI Gcd(
        /* [in] */ const BigInt& a,
        /* [in] */ const BigInt& b,
        /* [in] */ BigInt& r);

    static CARAPI Product(
        /* [in] */ const BigInt& a,
        /* [in] */ const BigInt& b,
        /* [in] */ BigInt& r);

    static CARAPI BigExp(
        /* [in] */ const BigInt& a,
        /* [in] */ const BigInt& p,
        /* [in] */ BigInt& r);

    static CARAPI Exp(
        /* [in] */ const BigInt& a,
        /* [in] */ Int32 p,
        /* [in] */ BigInt& r);

    static CARAPI Division(
        /* [in] */ const BigInt& dividend,
        /* [in] */ const BigInt& divisor,
        /* [in] */ BigInt* quotient,
        /* [in] */ BigInt* remainder);

    static CARAPI Modulus(
        /* [in] */ const BigInt& a,
        /* [in] */ const BigInt& m,
        /* [in] */ BigInt& r);

    static CARAPI ModExp(
        /* [in] */ const BigInt& a,
        /* [in] */ const BigInt& p,
        /* [in] */ const BigInt& m,
        /* [in] */ BigInt& r);

    static CARAPI ModInverse(
        /* [in] */ const BigInt& a,
        /* [in] */ const BigInt& m,
        /* [in] */ BigInt& r);

    static CARAPI GeneratePrimeDefault(
        /* [in] */ Int32 bitLength,
        /* [in] */ BigInt& r);

    Boolean IsPrime(
        /* [in] */ Int32 certainty);

    CARAPI MakeValid();

private:
    static CARAPI NewBigInt(
        /* [in] */ BigInt& bi);

    // Java supports non-ASCII decimal digits, but OpenSSL doesn't.
    // We need to translate the decimal digits but leave any other characters alone.
    // This method assumes it's being called on a string that has already been validated.
    static String ToAscii(
        /* [in] */ const String& s,
        /* [in] */ Int32 base);

private:
    /* Fields used for the internal representation. */
    Int64 mBignum;
};

} // namespace Math
} // namespace Elastos

#endif // __ELASTOS_MATH_BIGINT_H__
