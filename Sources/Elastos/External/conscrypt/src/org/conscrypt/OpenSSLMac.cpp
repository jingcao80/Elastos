
#include "_Org.Conscrypt.h"
#include "Elastos.CoreLibrary.Security.h"
#include "OpenSSLMac.h"
#include "NativeCrypto.h"
#include "COpenSSLKey.h"
#include "Elastos.CoreLibrary.Extensions.h"
#include <elastos/utility/logging/Logger.h>

using Elastosx::Crypto::ISecretKey;
using Org::Conscrypt::NativeCrypto;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Conscrypt {

//=========================================
// HmacMD5::
//=========================================
static Int64 GetHmacMD5_EVP_MD()
{
    Int64 md = 0;
    NativeCrypto::EVP_get_digestbyname(String("md5"), &md);
    return md;
}
Int64 HmacMD5::EVP_MD = GetHmacMD5_EVP_MD();

static Int32 GetHmacMD5_SIZE()
{
    Int32 size = 0;
    NativeCrypto::EVP_MD_size(HmacMD5::EVP_MD, &size);
    return size;
}
Int32 HmacMD5::SIZE = GetHmacMD5_SIZE();

HmacMD5::HmacMD5()
{
    OpenSSLMac::constructor(EVP_MD, SIZE, INativeCrypto::EVP_PKEY_HMAC);
}

//=========================================
// HmacSHA1::
//=========================================
static Int64 GetHmacSHA1_EVP_MD()
{
    Int64 md = 0;
    NativeCrypto::EVP_get_digestbyname(String("sha1"), &md);
    return md;
}
Int64 HmacSHA1::EVP_MD = GetHmacSHA1_EVP_MD();

static Int32 GetHmacSHA1_SIZE()
{
    Int32 size = 0;
    NativeCrypto::EVP_MD_size(HmacSHA1::EVP_MD, &size);
    return size;
}
Int32 HmacSHA1::SIZE = GetHmacSHA1_SIZE();

HmacSHA1::HmacSHA1()
{
    OpenSSLMac::constructor(EVP_MD, SIZE, INativeCrypto::EVP_PKEY_HMAC);
}

//=========================================
// HmacSHA224::
//=========================================
static Int64 GetHmacSHA224_EVP_MD()
{
    Int64 md = 0;
    NativeCrypto::EVP_get_digestbyname(String("sha224"), &md);
    return md;
}
Int64 HmacSHA224::EVP_MD = GetHmacSHA224_EVP_MD();

static Int32 GetHmacSHA224_SIZE()
{
    Int32 size = 0;
    NativeCrypto::EVP_MD_size(HmacSHA224::EVP_MD, &size);
    return size;
}
Int32 HmacSHA224::SIZE = GetHmacSHA224_SIZE();

HmacSHA224::HmacSHA224()
{
    OpenSSLMac::constructor(EVP_MD, SIZE, INativeCrypto::EVP_PKEY_HMAC);
}

//=========================================
// HmacSHA256::
//=========================================
static Int64 GetHmacSHA256_EVP_MD()
{
    Int64 md = 0;
    NativeCrypto::EVP_get_digestbyname(String("sha256"), &md);
    return md;
}
Int64 HmacSHA256::EVP_MD = GetHmacSHA256_EVP_MD();

static Int32 GetHmacSHA256_SIZE()
{
    Int32 size = 0;
    NativeCrypto::EVP_MD_size(HmacSHA256::EVP_MD, &size);
    return size;
}
Int32 HmacSHA256::SIZE = GetHmacSHA256_SIZE();

HmacSHA256::HmacSHA256()
{
    OpenSSLMac::constructor(EVP_MD, SIZE, INativeCrypto::EVP_PKEY_HMAC);
}

//=========================================
// HmacSHA384::
//=========================================
static Int64 GetHmacSHA384_EVP_MD()
{
    Int64 md = 0;
    NativeCrypto::EVP_get_digestbyname(String("sha384"), &md);
    return md;
}
Int64 HmacSHA384::EVP_MD = GetHmacSHA384_EVP_MD();

static Int32 GetHmacSHA384_SIZE()
{
    Int32 size = 0;
    NativeCrypto::EVP_MD_size(HmacSHA384::EVP_MD, &size);
    return size;
}
Int32 HmacSHA384::SIZE = GetHmacSHA384_SIZE();

HmacSHA384::HmacSHA384()
{
    OpenSSLMac::constructor(EVP_MD, SIZE, INativeCrypto::EVP_PKEY_HMAC);
}

//=========================================
// HmacSHA512::
//=========================================
static Int64 GetHmacSHA512_EVP_MD()
{
    Int64 md = 0;
    NativeCrypto::EVP_get_digestbyname(String("sha512"), &md);
    return md;
}
Int64 HmacSHA512::EVP_MD = GetHmacSHA512_EVP_MD();

static Int32 GetHmacSHA512_SIZE()
{
    Int32 size = 0;
    NativeCrypto::EVP_MD_size(HmacSHA512::EVP_MD, &size);
    return size;
}
Int32 HmacSHA512::SIZE = GetHmacSHA512_SIZE();

HmacSHA512::HmacSHA512()
{
    OpenSSLMac::constructor(EVP_MD, SIZE, INativeCrypto::EVP_PKEY_HMAC);
}

//=========================================
// OpenSSLMac::
//=========================================
CAR_INTERFACE_IMPL(OpenSSLMac, MacSpi, IOpenSSLMac)

ECode OpenSSLMac::constructor(
    /* [in] */ Int64 evp_md,
    /* [in] */ Int32 size,
    /* [in] */ Int32 evp_pkey_type)
{
    mEvp_md = evp_md;
    mSize = size;
    mEvp_pkey_type = evp_pkey_type;
    mSingleByte = ArrayOf<Byte>::Alloc(1);
    return NOERROR;
}

ECode OpenSSLMac::EngineGetMacLength(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mSize;
    return NOERROR;
}

ECode OpenSSLMac::EngineInit(
    /* [in] */ IKey* key,
    /* [in] */ IAlgorithmParameterSpec* params)
{
    if (ISecretKey::Probe(key) == NULL) {
        // throw new InvalidKeyException("key must be a SecretKey");
        Logger::E("OpenSSLMac", "EngineInit key must be a SecretKey");
        return E_INVALID_KEY_EXCEPTION;
    }

    if (params != NULL) {
        // throw new InvalidAlgorithmParameterException("unknown parameter type");
        Logger::E("OpenSSLMac", "EngineInit unknown parameter type");
        return E_INVALID_KEY_EXCEPTION;
    }

    if (IOpenSSLKeyHolder::Probe(key) != NULL) {
        IOpenSSLKeyHolder::Probe(key)->GetOpenSSLKey((IOpenSSLKey**)&mMacKey);
    }
    else {
        AutoPtr<ArrayOf<Byte> > keyBytes;
        key->GetEncoded((ArrayOf<Byte>**)&keyBytes);
        if (keyBytes == NULL) {
            // throw new InvalidKeyException("key cannot be encoded");
            Logger::E("OpenSSLMac", "EngineInit key cannot be encoded");
            return E_INVALID_KEY_EXCEPTION;
        }

        Int64 new_mac_key = 0;
        NativeCrypto::EVP_PKEY_new_mac_key(mEvp_pkey_type, keyBytes, &new_mac_key);
        COpenSSLKey::New(new_mac_key, (IOpenSSLKey**)&mMacKey);
    }

    ResetContext();
    return NOERROR;
}

void OpenSSLMac::ResetContext()
{
    Int64 cxt = 0;
    NativeCrypto::EVP_MD_CTX_create(&cxt);
    assert(0 && "TODO");
    AutoPtr<IOpenSSLDigestContext> ctxLocal;// = new OpenSSLDigestContext(cxt);
    NativeCrypto::EVP_MD_CTX_init(ctxLocal);

    AutoPtr<IOpenSSLKey> macKey = mMacKey;
    if (macKey != NULL) {
        Int64 context = 0;
        macKey->GetPkeyContext(&context);
        NativeCrypto::EVP_DigestSignInit(ctxLocal, mEvp_md, context);
    }

    mCtx = ctxLocal;
}

ECode OpenSSLMac::EngineUpdate(
    /* [in] */ Byte input)
{
    (*mSingleByte)[0] = input;
    return EngineUpdate(mSingleByte, 0, 1);
}

ECode OpenSSLMac::EngineUpdate(
    /* [in] */ ArrayOf<Byte>* input,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 len)
{
    AutoPtr<IOpenSSLDigestContext> ctxLocal = mCtx;
    NativeCrypto::EVP_DigestUpdate(ctxLocal, input, offset, len);
    return NOERROR;
}

ECode OpenSSLMac::EngineDoFinal(
    /* [out, callee] */ ArrayOf<Byte>** input)
{
    VALIDATE_NOT_NULL(input)
    AutoPtr<IOpenSSLDigestContext> ctxLocal = mCtx;
    AutoPtr<ArrayOf<Byte> > output;
    NativeCrypto::EVP_DigestSignFinal(ctxLocal, (ArrayOf<Byte>**)&output);
    Logger::E("leliang", "OpenSSLMac::EngineDoFinal, output:%p", output.Get());
    ResetContext();
    *input = output;
    return NOERROR;
}

ECode OpenSSLMac::EngineReset()
{
    ResetContext();
    return NOERROR;
}

} // namespace Conscrypt
} // namespace Org
