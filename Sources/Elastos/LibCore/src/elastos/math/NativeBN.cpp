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

#include "NativeBN.h"
#include <elastos/core/UniquePtr.h>
#include <openssl/bn.h>
#include <openssl/crypto.h>
#include <openssl/err.h>

namespace Elastos {
namespace Math {

using Elastos::Core::UniquePtr;

struct BN_CTX_Deleter
{
    void operator()(BN_CTX* p) const
    {
        BN_CTX_free(p);
    }
};

typedef UniquePtr<BN_CTX, BN_CTX_Deleter> Unique_BN_CTX;

BIGNUM* NativeBN::ToBigNum(
    /* [in] */ Int64 address)
{
    return reinterpret_cast<BIGNUM*>(static_cast<uintptr_t>(address));
}

Boolean NativeBN::IsValidHandle(
    /* [in] */ Int64 handle)
{
    if (handle == 0) {
        //jniThrowNullPointerException(env, message);
        return FALSE;
    }
    return TRUE;
}

Boolean NativeBN::OneValidHandle(
    /* [in] */ Int64 a)
{
    return IsValidHandle(a);
}

Boolean NativeBN::TwoValidHandles(
    /* [in] */ Int64 a,
    /* [in] */ Int64 b)
{
    if (!OneValidHandle(a)) return FALSE;
    return IsValidHandle(b);
}

Boolean NativeBN::ThreeValidHandles(
    /* [in] */ Int64 a,
    /* [in] */ Int64 b,
    /* [in] */ Int64 c)
{
    if (!TwoValidHandles(a, b)) return FALSE;
    return IsValidHandle(c);
}

Boolean NativeBN::FourValidHandles(
    /* [in] */ Int64 a,
    /* [in] */ Int64 b,
    /* [in] */ Int64 c,
    /* [in] */ Int64 d)
{
    if (!ThreeValidHandles(a, b, c)) return FALSE;
    return IsValidHandle(d);
}

Int64 NativeBN::BN_new()
{
    return static_cast<Int64>(reinterpret_cast<uintptr_t>(::BN_new()));
}

void NativeBN::BN_free(
    /* [in] */ Int64 a)
{
    if (!OneValidHandle(a)) return;
    ::BN_free(ToBigNum(a));
}

Int32 NativeBN::BN_cmp(
    /* [in] */ Int64 a,
    /* [in] */ Int64 b)
{
    if (!TwoValidHandles(a, b)) return 1;
    return ::BN_cmp(ToBigNum(a), ToBigNum(b));
}

void NativeBN::BN_copy(
    /* [in] */ Int64 to,
    /* [in] */ Int64 from)
{
    if (!TwoValidHandles(to, from)) return;
    ::BN_copy(ToBigNum(to), ToBigNum(from));
}

void NativeBN::PutULongInt(
    /* [in] */ Int64 a0,
    /* [in] */ Int64 java_dw,
    /* [in] */ Boolean neg)
{
    if (!OneValidHandle(a0)) return;

    uint64_t dw = java_dw;
    BIGNUM* a = ToBigNum(a0);

    if (!BN_set_u64(a, dw)) {
        // throwException(env);
        return;
    }

    ::BN_set_negative(a, neg);
}

void NativeBN::PutLongInt(
    /* [in] */ Int64 a,
    /* [in] */ Int64 dw)
{
    if (dw >= 0) {
        PutULongInt(a, dw, FALSE);
    } else {
        PutULongInt(a, -dw, TRUE);
    }
}

Int32 NativeBN::BN_dec2bn(
    /* [in] */ Int64 a0,
    /* [in] */ const String& str)
{
    if (!OneValidHandle(a0)) return -1;
    if (str.IsNull()) return -1;

    BIGNUM* a = ToBigNum(a0);
    return ::BN_dec2bn(&a, str);
    //throwExceptionIfNecessary(env);
}

Int32 NativeBN::BN_hex2bn(
    /* [in] */ Int64 a0,
    /* [in] */ const String& str)
{
    if (!OneValidHandle(a0)) return -1;
    if (str.IsNull()) return -1;

    BIGNUM* a = ToBigNum(a0);
    return ::BN_hex2bn(&a, str);
    //throwExceptionIfNecessary(env);
}

void NativeBN::BN_bin2bn(
    /* [in] */ const ArrayOf<Byte>& arr,
    /* [in] */ Int32 len,
    /* [in] */ Boolean neg,
    /* [in] */ Int64 ret)
{
    if (!OneValidHandle(ret)) return;
    if (arr.GetLength() == 0) return;

    Boolean success = (::BN_bin2bn(reinterpret_cast<const unsigned char*>(arr.GetPayload()), len, ToBigNum(ret)) != NULL);
    if (success && neg) {
        ::BN_set_negative(ToBigNum(ret), 1);
    }
}

/**
 * Note:
 * This procedure directly writes the internal representation of BIGNUMs.
 * We do so as there is no direct interface based on Little Endian Integer Arrays.
 * Also note that the same representation is used in the Cordoba Java Implementation of BigIntegers,
 *        whereof certain functionality is still being used.
 */
void NativeBN::LitEndInts2bn(
    /* [in] */ const ArrayOf<Int32>& arr,
    /* [in] */ Int32 len,
    /* [in] */ Boolean neg,
    /* [in] */ Int64 ret0)
{
    if (!OneValidHandle(ret0)) return;
    BIGNUM* ret = ToBigNum(ret0);
    if (len > 0) {
        if (arr.GetLength() == 0) return;
    #ifdef __LP64__
        const int wlen = (len + 1) / 2;
    #else
        const int wlen = len;
    #endif
        const unsigned int* tmpInts = reinterpret_cast<const unsigned int*>(arr.GetPayload());
        if ((tmpInts != NULL) && (bn_wexpand(ret, wlen) != NULL)) {
        #ifdef __LP64__
            if (len % 2) {
                ret->d[wlen - 1] = tmpInts[--len];
            }
            if (len > 0) {
                for (int i = len - 2; i >= 0; i -= 2) {
                    ret->d[i/2] = ((unsigned long long)tmpInts[i+1] << 32) | tmpInts[i];
                }
            }
        #else
            int i = len; do { i--; ret->d[i] = tmpInts[i]; } while (i > 0);
        #endif
            ret->top = wlen;
            ret->neg = neg;
            // need to call this due to clear byte at top if avoiding
            // having the top bit set (-ve number)
            // Basically get rid of top zero ints:
            bn_correct_top(ret);
        }
        else {
            //throwExceptionIfNecessary(env);
        }
    }
    else { // (len = 0) means value = 0 and sign will be 0, too.
       ret->top = 0;
    }
}

void NativeBN::TwosComp2bn(
    /* [in] */ const ArrayOf<Byte>& arr,
    /* [in] */ Int32 bytesLen,
    /* [in] */ Int64 ret0)
{
    if (!OneValidHandle(ret0)) return;
    BIGNUM* ret = ToBigNum(ret0);

    if (bytesLen == 0) return;

    const unsigned char* s = reinterpret_cast<const unsigned char*>(arr.GetPayload());

    if (!::BN_bin2bn(s, bytesLen, ret)) {
        // throwException(env);
        return;
    }

    // Use the high bit to determine the sign in twos-complement.
    ::BN_set_negative(ret, (arr[0] & 0x80) != 0);

    if (BN_is_negative(ret)) {
        // For negative values, BN_bin2bn doesn't interpret the twos-complement
        // representation, so ret is now (- value - 2^N). We can use nnmod_pow2 to set
        // ret to (-value).
        if (!BN_nnmod_pow2(ret, ret, bytesLen * 8)) {
            // throwException(env);
            return;
        }

        // And now we correct the sign.
        ::BN_set_negative(ret, 1);
    }
}

Int64 NativeBN::LongInt(
    /* [in] */ Int64 a0)
{
    if (!OneValidHandle(a0)) return -1;

    BIGNUM* a = ToBigNum(a0);
    int wLen = a->top;
    if (wLen == 0) {
        return 0;
    }

#ifdef __LP64__
    Int64 result = a->d[0];
#else
    Int64 result = static_cast<Int64>(a->d[0]) & 0xffffffff;
    if (wLen > 1) {
        result |= static_cast<Int64>(a->d[1]) << 32;
    }
#endif
    return a->neg ? -result : result;
}

char* NativeBN::LeadingZerosTrimmed(
    /* [in] */ char* s)
{
    char* p = s;
    if (*p == '-') {
        p++;
        while ((*p == '0') && (*(p + 1) != 0)) { p++; }
        p--;
        *p = '-';
    } else {
        while ((*p == '0') && (*(p + 1) != 0)) { p++; }
    }
    return p;
}

String NativeBN::BN_bn2dec(
    /* [in] */ Int64 a)
{
    if (!OneValidHandle(a)) return String();
    char* tmpStr;
    char* retStr;
    tmpStr = ::BN_bn2dec(ToBigNum(a));
    if (tmpStr != NULL) {
        retStr = LeadingZerosTrimmed(tmpStr);
        String returnJString(retStr);
        OPENSSL_free(tmpStr);
        return returnJString;
    }

    return String();
}

String NativeBN::BN_bn2hex(
    /* [in] */ Int64 a)
{
    if (!OneValidHandle(a)) return String();
    char* tmpStr;
    char* retStr;
    tmpStr = ::BN_bn2hex(ToBigNum(a));
    if (tmpStr != NULL) {
        retStr = LeadingZerosTrimmed(tmpStr);
        String returnJString(retStr);
        OPENSSL_free(tmpStr);
        return returnJString;
    }

    return String();
}

AutoPtr<ArrayOf<Byte> > NativeBN::BN_bn2bin(
    /* [in] */ Int64 a0)
{
    if (!OneValidHandle(a0)) return NULL;
    BIGNUM* a = ToBigNum(a0);

    AutoPtr<ArrayOf<Byte> > result = NULL;
    Int32 size = BN_num_bytes(a);
    if (size <= 0) {
        return result;
    }

    result = ArrayOf<Byte>::Alloc(size);
    ::BN_bn2bin(a, reinterpret_cast<unsigned char*>(result->GetPayload()));
    return result;
}

AutoPtr<ArrayOf<Int32> > NativeBN::Bn2litEndInts(
    /* [in] */ Int64 a0)
{
  if (!OneValidHandle(a0)) return NULL;
  BIGNUM* a = ToBigNum(a0);

  // The number of integers we need is BN_num_bytes(a) / sizeof(int), rounded up
  int intLen = (BN_num_bytes(a) + sizeof(int) - 1) / sizeof(int);

  AutoPtr<ArrayOf<Int32> > result = ArrayOf<Int32>::Alloc(intLen);
  if (result == NULL) {
    return NULL;
  }

  unsigned int* uints = reinterpret_cast<unsigned int*>(result->GetPayload());
  if (uints == NULL) {
    return NULL;
  }

  // We can simply interpret a little-endian byte stream as a little-endian integer stream.
  if (!BN_bn2le_padded(reinterpret_cast<uint8_t*>(uints), intLen * sizeof(int), a)) {
    // throwException(env);
    return NULL;
  }
  return result;
}

Int32 NativeBN::Sign(
    /* [in] */ Int64 a)
{
  if (!OneValidHandle(a)) return -2;
  if (BN_is_zero(ToBigNum(a))) {
      return 0;
  }
  else if (BN_is_negative(ToBigNum(a))) {
    return -1;
  }
  return 1;
}

void NativeBN::BN_set_negative(
    /* [in] */ Int64 b,
    /* [in] */ Int32 n)
{
    if (!OneValidHandle(b)) return;
    ::BN_set_negative(ToBigNum(b), n);
}

Int32 NativeBN::BitLength(
    /* [in] */ Int64 a0)
{
  if (!OneValidHandle(a0)) return 0; //FALSE;
  BIGNUM* a = ToBigNum(a0);

  // If a is not negative, we can use BN_num_bits directly.
  if (!BN_is_negative(a)) {
    return BN_num_bits(a);
  }

  // In the negative case, the number of bits in a is the same as the number of bits in |a|,
  // except one less when |a| is a power of two.
  BIGNUM positiveA;
  BN_init(&positiveA);

  if (!::BN_copy(&positiveA, a)) {
    ::BN_free(&positiveA);
    // throwException(env);
    return -1;
  }

  ::BN_set_negative(&positiveA, false);
  int numBits = BN_is_pow2(&positiveA) ? BN_num_bits(&positiveA) - 1 : BN_num_bits(&positiveA);

  ::BN_free(&positiveA);
  return numBits;
}

Boolean NativeBN::BN_is_bit_set(
    /* [in] */ Int64 a,
    /* [in] */ Int32 n)
{
    if (!OneValidHandle(a)) return FALSE;
    return ::BN_is_bit_set(ToBigNum(a), n);
}

void NativeBN::BN_shift(
    /* [in] */ Int64 r,
    /* [in] */ Int64 a,
    /* [in] */ Int32 n)
{
  if (!TwoValidHandles(r, a)) return;
  if (n >= 0) {
    ::BN_lshift(ToBigNum(r), ToBigNum(a), n);
  } else {
    ::BN_rshift(ToBigNum(r), ToBigNum(a), -n);
  }
  //throwExceptionIfNecessary(env);
}

void NativeBN::BN_add_word(
    /* [in] */ Int64 a,
    /* [in] */ Int32 w)
{
    if (!OneValidHandle(a)) return;
    ::BN_add_word(ToBigNum(a), w);
    //throwExceptionIfNecessary(env);
}

void NativeBN::BN_mul_word(
    /* [in] */ Int64 a,
    /* [in] */ Int32 w)
{
    if (!OneValidHandle(a)) return;
    ::BN_mul_word(ToBigNum(a), w);
    //throwExceptionIfNecessary(env);
}

Int32 NativeBN::BN_mod_word(
    /* [in] */ Int64 a,
    /* [in] */ Int32 w)
{
    if (!OneValidHandle(a)) return 0;
    return ::BN_mod_word(ToBigNum(a), w);
    //throwExceptionIfNecessary(env);
}

void NativeBN::BN_add(
    /* [in] */ Int64 r,
    /* [in] */ Int64 a,
    /* [in] */ Int64 b)
{
    if (!ThreeValidHandles(r, a, b)) return;
    ::BN_add(ToBigNum(r), ToBigNum(a), ToBigNum(b));
    //throwExceptionIfNecessary(env);
}

void NativeBN::BN_sub(
    /* [in] */ Int64 r,
    /* [in] */ Int64 a,
    /* [in] */ Int64 b)
{
    if (!ThreeValidHandles(r, a, b)) return;
    ::BN_sub(ToBigNum(r), ToBigNum(a), ToBigNum(b));
    //throwExceptionIfNecessary(env);
}

void NativeBN::BN_gcd(
    /* [in] */ Int64 r,
    /* [in] */ Int64 a,
    /* [in] */ Int64 b)
{
    if (!ThreeValidHandles(r, a, b)) return;
    Unique_BN_CTX ctx(BN_CTX_new());
    ::BN_gcd(ToBigNum(r), ToBigNum(a), ToBigNum(b), ctx.get());
    //throwExceptionIfNecessary(env);
}

void NativeBN::BN_mul(
    /* [in] */ Int64 r,
    /* [in] */ Int64 a,
    /* [in] */ Int64 b)
{
    if (!ThreeValidHandles(r, a, b)) return;
    Unique_BN_CTX ctx(BN_CTX_new());
    ::BN_mul(ToBigNum(r), ToBigNum(a), ToBigNum(b), ctx.get());
    //throwExceptionIfNecessary(env);
}

void NativeBN::BN_exp(
    /* [in] */ Int64 r,
    /* [in] */ Int64 a,
    /* [in] */ Int64 p)
{
    if (!ThreeValidHandles(r, a, p)) return;
    Unique_BN_CTX ctx(BN_CTX_new());
    ::BN_exp(ToBigNum(r), ToBigNum(a), ToBigNum(p), ctx.get());
    //throwExceptionIfNecessary(env);
}

void NativeBN::BN_div(
    /* [in] */ Int64 dv,
    /* [in] */ Int64 rem,
    /* [in] */ Int64 m,
    /* [in] */ Int64 d)
{
    if (!FourValidHandles((rem ? rem : dv), (dv ? dv : rem), m, d)) return;
    Unique_BN_CTX ctx(BN_CTX_new());
    ::BN_div(ToBigNum(dv), ToBigNum(rem), ToBigNum(m), ToBigNum(d), ctx.get());
    //throwExceptionIfNecessary(env);
}

void NativeBN::BN_nnmod(
    /* [in] */ Int64 r,
    /* [in] */ Int64 a,
    /* [in] */ Int64 m)
{
    if (!ThreeValidHandles(r, a, m)) return;
    Unique_BN_CTX ctx(BN_CTX_new());
    ::BN_nnmod(ToBigNum(r), ToBigNum(a), ToBigNum(m), ctx.get());
    //throwExceptionIfNecessary(env);
}

void NativeBN::BN_mod_exp(
    /* [in] */ Int64 r,
    /* [in] */ Int64 a,
    /* [in] */ Int64 p,
    /* [in] */ Int64 m)
{
    if (!FourValidHandles(r, a, p, m)) return;
    Unique_BN_CTX ctx(BN_CTX_new());
    ::BN_mod_exp(ToBigNum(r), ToBigNum(a), ToBigNum(p), ToBigNum(m), ctx.get());
    //throwExceptionIfNecessary(env);
}

void NativeBN::BN_mod_inverse(
    /* [in] */ Int64 ret,
    /* [in] */ Int64 a,
    /* [in] */ Int64 n)
{
    if (!ThreeValidHandles(ret, a, n)) return;
    Unique_BN_CTX ctx(BN_CTX_new());
    ::BN_mod_inverse(ToBigNum(ret), ToBigNum(a), ToBigNum(n), ctx.get());
    //throwExceptionIfNecessary(env);
}

void NativeBN::BN_generate_prime_ex(
    /* [in] */ Int64 ret,
    /* [in] */ Int32 bits,
    /* [in] */ Boolean safe,
    /* [in] */ Int64 add,
    /* [in] */ Int64 rem,
    /* [in] */ Int64 cb)
{
  if (!OneValidHandle(ret)) return;
  ::BN_generate_prime_ex(ToBigNum(ret), bits, safe, ToBigNum(add), ToBigNum(rem),
                       reinterpret_cast<BN_GENCB*>(cb));
  //throwExceptionIfNecessary(env);
}

Boolean NativeBN::BN_is_prime_ex(
    /* [in] */ Int64 p,
    /* [in] */ Int32 nchecks,
    /* [in] */ Int64 cb)
{
  if (!OneValidHandle(p)) return FALSE;
  Unique_BN_CTX ctx(BN_CTX_new());
  return ::BN_is_prime_ex(ToBigNum(p), nchecks, ctx.get(), reinterpret_cast<BN_GENCB*>(cb));
}

} // namespace Math
} // namespace Elastos
