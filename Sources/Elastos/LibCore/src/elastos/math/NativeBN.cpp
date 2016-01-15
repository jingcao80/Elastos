
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

    // cf. litEndInts2bn:
    BIGNUM* a = ToBigNum(a0);
    bn_check_top(a);
    if (bn_wexpand(a, 8/BN_BYTES) != NULL) {
#ifdef __LP64__
        a->d[0] = dw;
#else
        unsigned int hi = dw >> 32; // This shifts without sign extension.
        int lo = (int)dw; // This truncates implicitly.
        a->d[0] = lo;
        a->d[1] = hi;
#endif
        a->top = 8 / BN_BYTES;
        a->neg = neg;
        bn_correct_top(a);
    }
    else {
        //throwExceptionIfNecessary(env);
    }
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
    bn_check_top(ret);
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

#ifdef __LP64__
#define BYTES2ULONG(bytes, k) \
    ((bytes[k + 7] & 0xffULL)       | (bytes[k + 6] & 0xffULL) <<  8 | (bytes[k + 5] & 0xffULL) << 16 | (bytes[k + 4] & 0xffULL) << 24 | \
     (bytes[k + 3] & 0xffULL) << 32 | (bytes[k + 2] & 0xffULL) << 40 | (bytes[k + 1] & 0xffULL) << 48 | (bytes[k + 0] & 0xffULL) << 56)
#else
#define BYTES2ULONG(bytes, k) \
    ((bytes[k + 3] & 0xff) | (bytes[k + 2] & 0xff) << 8 | (bytes[k + 1] & 0xff) << 16 | (bytes[k + 0] & 0xff) << 24)
#endif

void NativeBN::NegBigEndianBytes2bn(
    /* [in] */ const unsigned char* bytes,
    /* [in] */ Int32 bytesLen,
    /* [in] */ Int64 ret0)
{
    BIGNUM* ret = ToBigNum(ret0);

    bn_check_top(ret);
    // FIXME: assert bytesLen > 0
    int wLen = (bytesLen + BN_BYTES - 1) / BN_BYTES;
    int firstNonzeroDigit = -2;
    if (bn_wexpand(ret, wLen) != NULL) {
        BN_ULONG* d = ret->d;
        BN_ULONG di;
        ret->top = wLen;
        int highBytes = bytesLen % BN_BYTES;
        int k = bytesLen;
        // Put bytes to the int array starting from the end of the byte array
        int i = 0;
        while (k > highBytes) {
            k -= BN_BYTES;
            di = BYTES2ULONG(bytes, k);
            if (di != 0) {
                d[i] = -di;
                firstNonzeroDigit = i;
                i++;
                while (k > highBytes) {
                    k -= BN_BYTES;
                    d[i] = ~BYTES2ULONG(bytes, k);
                    i++;
                }
                break;
            }
            else {
                d[i] = 0;
                i++;
            }
        }
        if (highBytes != 0) {
            di = -1;
            // Put the first bytes in the highest element of the int array
            if (firstNonzeroDigit != -2) {
                for (k = 0; k < highBytes; k++) {
                    di = (di << 8) | (bytes[k] & 0xFF);
                }
                d[i] = ~di;
            }
            else {
                for (k = 0; k < highBytes; k++) {
                    di = (di << 8) | (bytes[k] & 0xFF);
                }
                d[i] = -di;
            }
        }
        // The top may have superfluous zeros, so fix it.
        bn_correct_top(ret);
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
    if ((arr[0] & 0X80) == 0) { // Positive value!
        //
        // We can use the existing BN implementation for unsigned big endian bytes:
        //
        ::BN_bin2bn(s, bytesLen, ret);
        ::BN_set_negative(ret, FALSE);
    }
    else { // Negative value!
        //
        // We need to apply two's complement:
        //
        NegBigEndianBytes2bn(s, bytesLen, ret0);
        ::BN_set_negative(ret, TRUE);
    }
    //throwExceptionIfNecessary(env);
}

Int64 NativeBN::LongInt(
    /* [in] */ Int64 a0)
{
    if (!OneValidHandle(a0)) return -1;

    BIGNUM* a = ToBigNum(a0);
    bn_check_top(a);
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
  bn_check_top(a);
  int wLen = a->top;
  if (wLen == 0) {
    return NULL;
  }
  AutoPtr<ArrayOf<Int32> > result = ArrayOf<Int32>::Alloc(wLen * BN_BYTES/sizeof(unsigned int));
  if (result == NULL) {
    return NULL;
  }
  unsigned int* uints = reinterpret_cast<unsigned int*>(result->GetPayload());
  if (uints == NULL) {
    return NULL;
  }
#ifdef __LP64__
  int i = wLen; do { i--; uints[i*2+1] = a->d[i] >> 32; uints[i*2] = a->d[i]; } while (i > 0);
#else
  int i = wLen; do { i--; uints[i] = a->d[i]; } while (i > 0);
#endif
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
  bn_check_top(a);
  int wLen = a->top;
  if (wLen == 0) return 0;
  BN_ULONG* d = a->d;
  int i = wLen - 1;
  BN_ULONG msd = d[i]; // most significant digit
  if (a->neg) {
    // Handle negative values correctly:
    // i.e. decrement the msd if all other digits are 0:
    // while ((i > 0) && (d[i] != 0)) { i--; }
    do { i--; } while (!((i < 0) || (d[i] != 0)));
    if (i < 0) msd--; // Only if all lower significant digits are 0 we decrement the most significant one.
  }
  return (wLen - 1) * BN_BYTES * 8 + ::BN_num_bits_word(msd);
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
