
#include "OpenSSLMac.h"

namespace Org {
namespace Conscrypt {

//=========================================
// OpenSSLMac::HmacMD5::
//=========================================
static Int64 GetHmacMD5_EVP_MD()
{
    Int64 md = 0;
    NativeCrypto::EVP_get_digestbyname("md5", &md);
    return md;
}
Int64 OpenSSLMac::HmacMD5::EVP_MD = GetHmacMD5_EVP_MD();

static Int32 GetHmacMD5_SIZE()
{
    Int64 size = 0;
    NativeCrypto::EVP_MD_size(HmacMD5::EVP_MD, &size);
    return size;
}
Int32 OpenSSLMac::HmacMD5::SIZE = GetHmacMD5_SIZE();

OpenSSLMac::HmacMD5::HmacMD5()
    : OpenSSLMac(EVP_MD, SIZE, INativeCrypto::EVP_PKEY_HMAC)
{
}

//=========================================
// OpenSSLMac::HmacSHA1::
//=========================================
static Int64 GetHmacSHA1_EVP_MD()
{
    Int64 md = 0;
    NativeCrypto::EVP_get_digestbyname("sha1", &md);
    return md;
}
Int64 OpenSSLMac::HmacSHA1::EVP_MD = GetHmacSHA1_EVP_MD();

static Int32 GetHmacSHA1_SIZE()
{
    Int64 size = 0;
    NativeCrypto::EVP_MD_size(HmacSHA1::EVP_MD, &size);
    return size;
}
Int32 OpenSSLMac::HmacSHA1::SIZE = GetHmacSHA1_SIZE();

OpenSSLMac::HmacSHA1::HmacSHA1()
    : OpenSSLMac(EVP_MD, SIZE, INativeCrypto::EVP_PKEY_HMAC)
{
}

//=========================================
// OpenSSLMac::HmacSHA224::
//=========================================
static Int64 GetHmacSHA224_EVP_MD()
{
    Int64 md = 0;
    NativeCrypto::EVP_get_digestbyname("sha224", &md);
    return md;
}
Int64 OpenSSLMac::HmacSHA224::EVP_MD = GetHmacSHA224_EVP_MD();

static Int32 GetHmacSHA224_SIZE()
{
    Int64 size = 0;
    NativeCrypto::EVP_MD_size(HmacSHA224::EVP_MD, &size);
    return size;
}
Int32 OpenSSLMac::HmacSHA224::SIZE = GetHmacSHA224_SIZE();

OpenSSLMac::HmacSHA224::HmacSHA224()
    : OpenSSLMac(EVP_MD, SIZE, INativeCrypto::EVP_PKEY_HMAC)
{
}

//=========================================
// OpenSSLMac::HmacSHA256::
//=========================================
static Int64 GetHmacSHA256_EVP_MD()
{
    Int64 md = 0;
    NativeCrypto::EVP_get_digestbyname("sha256", &md);
    return md;
}
Int64 OpenSSLMac::HmacSHA256::EVP_MD = GetHmacSHA256_EVP_MD();

static Int32 GetHmacSHA256_SIZE()
{
    Int64 size = 0;
    NativeCrypto::EVP_MD_size(HmacSHA256::EVP_MD, &size);
    return size;
}
Int32 OpenSSLMac::HmacSHA256::SIZE = GetHmacSHA256_SIZE();

OpenSSLMac::HmacSHA256::HmacSHA256()
    : OpenSSLMac(EVP_MD, SIZE, INativeCrypto::EVP_PKEY_HMAC)
{
}

//=========================================
// OpenSSLMac::HmacSHA384::
//=========================================
static Int64 GetHmacSHA384_EVP_MD()
{
    Int64 md = 0;
    NativeCrypto::EVP_get_digestbyname("sha384", &md);
    return md;
}
Int64 OpenSSLMac::HmacSHA384::EVP_MD = GetHmacSHA384_EVP_MD();

static Int32 GetHmacSHA384_SIZE()
{
    Int64 size = 0;
    NativeCrypto::EVP_MD_size(HmacSHA384::EVP_MD, &size);
    return size;
}
Int32 OpenSSLMac::HmacSHA384::SIZE = GetHmacSHA384_SIZE();

OpenSSLMac::HmacSHA384::HmacSHA384()
    : OpenSSLMac(EVP_MD, SIZE, INativeCrypto::EVP_PKEY_HMAC)
{
}

//=========================================
// OpenSSLMac::HmacSHA512::
//=========================================
static Int64 GetHmacSHA512_EVP_MD()
{
    Int64 md = 0;
    NativeCrypto::EVP_get_digestbyname("sha512", &md);
    return md;
}
Int64 OpenSSLMac::HmacSHA512::EVP_MD = GetHmacSHA512_EVP_MD();

static Int32 GetHmacSHA512_SIZE()
{
    Int64 size = 0;
    NativeCrypto::EVP_MD_size(HmacSHA512::EVP_MD, &size);
    return size;
}
Int32 OpenSSLMac::HmacSHA512::SIZE = GetHmacSHA512_SIZE();

OpenSSLMac::HmacSHA512::HmacSHA512()
    : OpenSSLMac(EVP_MD, SIZE, INativeCrypto::EVP_PKEY_HMAC)
{
}

//=========================================
// OpenSSLMac::
//=========================================
OpenSSLMac::OpenSSLMac(
    /* [in] */ Int64 evp_md,
    /* [in] */ Int32 size,
    /* [in] */ Int32 evp_pkey_type)
{
    mEvp_md = evp_md;
    mSize = size;
    mEvp_pkey_type = evp_pkey_type;
    mSingleByte = ArrayOf<Byte>::Alloc(1);
}

Int32 OpenSSLMac::EngineGetMacLength()
{
    return mSize;
}

void OpenSSLMac::EngineInit(
    /* [in] */ IKey* key,
    /* [in] */ IAlgorithmParameterSpec* params)
{
    if (ISecretKey::Probe(key) == NULL) {
        // throw new InvalidKeyException("key must be a SecretKey");
        return;
    }

    if (params != NULL) {
        // throw new InvalidAlgorithmParameterException("unknown parameter type");
        return;
    }

    if (IOpenSSLKeyHolder::Probe(key) != NULL) {
        IOpenSSLKeyHolder::Probe(key)->GetOpenSSLKey((IOpenSSLKey**)&mMacKey);
    }
    else {
        AutoPtr<ArrayOf<Byte> > keyBytes;
        key->GetEncoded((ArrayOf<Byte>**)&keyBytes);
        if (keyBytes == NULL) {
            // throw new InvalidKeyException("key cannot be encoded");
            return;
        }

        Int64 new_mac_key = 0;
        NativeCrypto::EVP_PKEY_new_mac_key(evp_pkey_type, keyBytes, &new_mac_key);
        macKey = new OpenSSLKey(new_mac_key);
    }

    ResetContext();
}

void OpenSSLMac::ResetContext()
{
    Int64 cxt = 0;
    NativeCrypto::EVP_MD_CTX_create(&cxt);
    AutoPtr<IOpenSSLDigestContext> ctxLocal = new OpenSSLDigestContext(cxt);
    NativeCrypto::EVP_MD_CTX_init(ctxLocal);

    AutoPtr<IOpenSSLKey> macKey = mMacKey;
    if (macKey != NULL) {
        NativeCrypto::EVP_DigestSignInit(ctxLocal, evp_md, macKey->GetPkeyContext());
    }

    mCtx = ctxLocal;
}

void OpenSSLMac::EngineUpdate(
    /* [in] */ Byte input)
{
    (*mSingleByte)[0] = input;
    EngineUpdate(singleByte, 0, 1);
}

void OpenSSLMac::EngineUpdate(
    /* [in] */ ArrayOf<Byte>* input,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 len)
{
    AutoPtr<IOpenSSLDigestContext> ctxLocal = mCtx;
    NativeCrypto::EVP_DigestUpdate(ctxLocal, input, offset, len);
}

AutoPtr<ArrayOf<Byte> > OpenSSLMac::EngineDoFinal()
{
    AutoPtr<IOpenSSLDigestContext> ctxLocal = mCtx;
    AutoPtr<ArrayOf<Byte> > output;
    NativeCrypto::EVP_DigestSignFinal(ctxLocal, (ArrayOf<Byte>**)&output);
    ResetContext();
    return output;
}

void OpenSSLMac::EngineReset()
{
    ResetContext();
}

} // namespace Conscrypt
} // namespace Org
