#ifndef __ELASTOS_MATH_NATIVEBN_H__
#define __ELASTOS_MATH_NATIVEBN_H__

#include <elastos.h>
#include <elquintet.h>
#include <openssl/bn.h>

// see java_math_NativeBN.cpp
//
namespace Elastos {
namespace Math {

class NativeBN
{
public:
    static Int64 BN_new();
    // BIGNUM *BN_new(void);

    static void BN_free(
        /* [in] */ Int64 a);
    // void BN_free(BIGNUM *a);

    static Int32 BN_cmp(
        /* [in] */ Int64 a,
        /* [in] */ Int64 b);
    // int BN_cmp(const BIGNUM *a, const BIGNUM *b);

    static void BN_copy(
        /* [in] */ Int64 to,
        /* [in] */ Int64 from);
    // BIGNUM *BN_copy(BIGNUM *to, const BIGNUM *from);

    static void PutLongInt(
        /* [in] */ Int64 a,
        /* [in] */ Int64 dw);

    static void PutULongInt(
        /* [in] */ Int64 a,
        /* [in] */ Int64 dw,
        /* [in] */ Boolean neg);

    static Int32 BN_dec2bn(
        /* [in] */ Int64 a,
        /* [in] */ const String& str);
    // int BN_dec2bn(BIGNUM **a, const char *str);

    static Int32 BN_hex2bn(
        /* [in] */ Int64 a,
        /* [in] */ const String& str);
    // int BN_hex2bn(BIGNUM **a, const char *str);

    static void BN_bin2bn(
        /* [in] */ const ArrayOf<Byte>& s,
        /* [in] */ Int32 len,
        /* [in] */ Boolean neg,
        /* [in] */ Int64 ret);
    // BIGNUM * BN_bin2bn(const unsigned char *s, int len, BIGNUM *ret);
    // BN-Docu: s is taken as unsigned big endian;
    // Additional parameter: neg.

    /**
     * Note:
     * This procedure directly writes the internal representation of BIGNUMs.
     * We do so as there is no direct interface based on Little Endian Integer Arrays.
     * Also note that the same representation is used in the Cordoba Java Implementation of BigIntegers,
     *        whereof certain functionality is still being used.
     */
    static void LitEndInts2bn(
        /* [in] */ const ArrayOf<Int32>& ints,
        /* [in] */ Int32 len,
        /* [in] */ Boolean neg,
        /* [in] */ Int64 ret);

    static void TwosComp2bn(
        /* [in] */ const ArrayOf<Byte>& s,
        /* [in] */ Int32 len,
        /* [in] */ Int64 ret);

    static Int64 LongInt(
        /* [in] */ Int64 a);
    // unsigned /* [in] */ Int64 BN_get_word(BIGNUM *a);

    static String BN_bn2dec(
        /* [in] */ Int64 a);
    // char * BN_bn2dec(const BIGNUM *a);

    static String BN_bn2hex(
        /* [in] */ Int64 a);
    // char * BN_bn2hex(const BIGNUM *a);

    static AutoPtr<ArrayOf<Byte> > BN_bn2bin(
        /* [in] */ Int64 a);
    // Returns result byte[] AND NOT length.
    // int BN_bn2bin(const BIGNUM *a, unsigned char *to);

    static AutoPtr<ArrayOf<Int32> > Bn2litEndInts(
        /* [in] */ Int64 a);

    static Int32 Sign(
        /* [in] */ Int64 a);
    // Returns -1, 0, 1 AND NOT Boolean.
    // #define BN_is_negative(a) ((a)->neg != 0)

    static void BN_set_negative(
        /* [in] */ Int64 b,
        /* [in] */ Int32 n);
    // void BN_set_negative(BIGNUM *b, int n);

    static Int32 BitLength(
        /* [in] */ Int64 a);

    static Boolean BN_is_bit_set(
        /* [in] */ Int64 a,
        /* [in] */ Int32 n);
    // int BN_is_bit_set(const BIGNUM *a, int n);

    static void BN_shift(
        /* [in] */ Int64 r,
        /* [in] */ Int64 a,
        /* [in] */ Int32 n);
    // int BN_shift(BIGNUM *r, const BIGNUM *a, int n);

    static void BN_add_word(
        /* [in] */ Int64 a,
        /* [in] */ Int32 w);
    // ATTENTION: w is treated as unsigned.
    // int BN_add_word(BIGNUM *a, BN_ULONG w);

    static void BN_mul_word(
        /* [in] */ Int64 a,
        /* [in] */ Int32 w);
    // ATTENTION: w is treated as unsigned.
    // int BN_mul_word(BIGNUM *a, BN_ULONG w);

    static Int32 BN_mod_word(
        /* [in] */ Int64 a,
        /* [in] */ Int32 w);
    // ATTENTION: w is treated as unsigned.
    // BN_ULONG BN_mod_word(BIGNUM *a, BN_ULONG w);

    static void BN_add(
        /* [in] */ Int64 r,
        /* [in] */ Int64 a,
        /* [in] */ Int64 b);
    // int BN_add(BIGNUM *r, const BIGNUM *a, const BIGNUM *b);

    static void BN_sub(
        /* [in] */ Int64 r,
        /* [in] */ Int64 a,
        /* [in] */ Int64 b);
    // int BN_sub(BIGNUM *r, const BIGNUM *a, const BIGNUM *b);

    static void BN_gcd(
        /* [in] */ Int64 r,
        /* [in] */ Int64 a,
        /* [in] */ Int64 b);
    // int BN_gcd(BIGNUM *r, const BIGNUM *a, const BIGNUM *b, BN_CTX *ctx);

    static void BN_mul(
        /* [in] */ Int64 r,
        /* [in] */ Int64 a,
        /* [in] */ Int64 b);
    // int BN_mul(BIGNUM *r, const BIGNUM *a, const BIGNUM *b, BN_CTX *ctx);

    static void BN_exp(
        /* [in] */ Int64 r,
        /* [in] */ Int64 a,
        /* [in] */ Int64 p);
    // int BN_exp(BIGNUM *r, const BIGNUM *a, const BIGNUM *p, BN_CTX *ctx);

    static void BN_div(
        /* [in] */ Int64 dv,
        /* [in] */ Int64 rem,
        /* [in] */ Int64 m,
        /* [in] */ Int64 d);
    // int BN_div(BIGNUM *dv, BIGNUM *rem, const BIGNUM *m, const BIGNUM *d, BN_CTX *ctx);

    static void BN_nnmod(
        /* [in] */ Int64 r,
        /* [in] */ Int64 a,
        /* [in] */ Int64 m);
    // int BN_nnmod(BIGNUM *r, const BIGNUM *a, const BIGNUM *m, BN_CTX *ctx);

    static void BN_mod_exp(
        /* [in] */ Int64 r,
        /* [in] */ Int64 a,
        /* [in] */ Int64 p,
        /* [in] */ Int64 m);
    // int BN_mod_exp(BIGNUM *r, const BIGNUM *a, const BIGNUM *p, const BIGNUM *m, BN_CTX *ctx);

    static void BN_mod_inverse(
        /* [in] */ Int64 ret,
        /* [in] */ Int64 a,
        /* [in] */ Int64 n);
    // BIGNUM * BN_mod_inverse(BIGNUM *ret, const BIGNUM *a, const BIGNUM *n, BN_CTX *ctx);


    static void BN_generate_prime_ex(
        /* [in] */ Int64 ret,
        /* [in] */ Int32 bits,
        /* [in] */ Boolean safe,
        /* [in] */ Int64 add,
        /* [in] */ Int64 rem,
        /* [in] */ Int64 cb);
    // int BN_generate_prime_ex(BIGNUM *ret, int bits, int safe,
    //         const BIGNUM *add, const BIGNUM *rem, BN_GENCB *cb);

    static Boolean BN_is_prime_ex(
        /* [in] */ Int64 p,
        /* [in] */ Int32 nchecks,
        /* [in] */ Int64 cb);
    // int BN_is_prime_ex(const BIGNUM *p, int nchecks, BN_CTX *ctx, BN_GENCB *cb);

private:
    static BIGNUM* ToBigNum(
        /* [in] */ Int64 address);

    static Boolean IsValidHandle(
        /* [in] */ Int64 handle);

    static Boolean OneValidHandle(
        /* [in] */ Int64 a);

    static Boolean TwoValidHandles(
        /* [in] */ Int64 a,
        /* [in] */ Int64 b);

    static Boolean ThreeValidHandles(
        /* [in] */ Int64 a,
        /* [in] */ Int64 b,
        /* [in] */ Int64 c);

    static Boolean FourValidHandles(
        /* [in] */ Int64 a,
        /* [in] */ Int64 b,
        /* [in] */ Int64 c,
        /* [in] */ Int64 d);

    static void NegBigEndianBytes2bn(
        /* [in] */ const unsigned char* bytes,
        /* [in] */ Int32 bytesLen,
        /* [in] */ Int64 ret);

    static char* LeadingZerosTrimmed(
        /* [in] */ char* s);
};

} // namespace Math
} // namespace Elastos

#endif // __ELASTOS_MATH_NATIVEBN_H__
