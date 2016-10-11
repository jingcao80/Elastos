
#include "OpenSSLSignature.h"
#include "NativeCrypto.h"
#include "OpenSSLKey.h"

namespace Org {
namespace Conscrypt {

//=========================================
// MD5RSA::
//=========================================
static Int64 Init_MD5RSA_EVP_MD()
{
    Int64 res = 0;
    NativeCrypto::EVP_get_digestbyname(String("RSA-MD5"), &res);
    return res;
}
Int64 MD5RSA::EVP_MD = Init_MD5RSA_EVP_MD();

MD5RSA::MD5RSA()
{
    OpenSSLSignature::constructor(EVP_MD, EngineType_RSA);
}

//=========================================
// SHA1RSA::
//=========================================
static Int64 Init_SHA1RSA_EVP_MD()
{
    Int64 res = 0;
    NativeCrypto::EVP_get_digestbyname(String("RSA-SHA1"), &res);
    return res;
}
Int64 SHA1RSA::EVP_MD = Init_SHA1RSA_EVP_MD();

SHA1RSA::SHA1RSA()
{
    OpenSSLSignature::constructor(EVP_MD, EngineType_RSA);
}

//=========================================
// SHA224RSA::
//=========================================
static Int64 Init_SHA224RSA_EVP_MD()
{
    Int64 res = 0;
    NativeCrypto::EVP_get_digestbyname(String("RSA-SHA224"), &res);
    return res;
}
Int64 SHA224RSA::EVP_MD = Init_SHA224RSA_EVP_MD();

SHA224RSA::SHA224RSA()
{
    OpenSSLSignature::constructor(EVP_MD, EngineType_RSA);
}

//=========================================
// SHA256RSA::
//=========================================
static Int64 Init_SHA256RSA_EVP_MD()
{
    Int64 res = 0;
    NativeCrypto::EVP_get_digestbyname(String("RSA-SHA256"), &res);
    return res;
}
Int64 SHA256RSA::EVP_MD = Init_SHA256RSA_EVP_MD();

SHA256RSA::SHA256RSA()
{
    OpenSSLSignature::constructor(EVP_MD, EngineType_RSA);
}

//=========================================
// SHA384RSA::
//=========================================
static Int64 Init_SHA384RSA_EVP_MD()
{
    Int64 res = 0;
    NativeCrypto::EVP_get_digestbyname(String("RSA-SHA384"), &res);
    return res;
}
Int64 SHA384RSA::EVP_MD = Init_SHA384RSA_EVP_MD();

SHA384RSA::SHA384RSA()
{
    OpenSSLSignature::constructor(EVP_MD, EngineType_RSA);
}

//=========================================
// SHA512RSA::
//=========================================
static Int64 Init_SHA512RSA_EVP_MD()
{
    Int64 res = 0;
    NativeCrypto::EVP_get_digestbyname(String("RSA-SHA512"), &res);
    return res;
}
Int64 SHA512RSA::EVP_MD = Init_SHA512RSA_EVP_MD();

SHA512RSA::SHA512RSA()
{
    OpenSSLSignature::constructor(EVP_MD, EngineType_RSA);
}

//=========================================
// SHA1DSA::
//=========================================
static Int64 Init_SHA1DSA_EVP_MD()
{
    Int64 res = 0;
    NativeCrypto::EVP_get_digestbyname(String("DSA-SHA1"), &res);
    return res;
}
Int64 SHA1DSA::EVP_MD = Init_SHA1DSA_EVP_MD();

SHA1DSA::SHA1DSA()
{
    OpenSSLSignature::constructor(EVP_MD, EngineType_DSA);
}

//=========================================
// SHA1ECDSA::
//=========================================
static Int64 Init_SHA1ECDSA_EVP_MD()
{
    Int64 res = 0;
    NativeCrypto::EVP_get_digestbyname(String("SHA1"), &res);
    return res;
}
Int64 SHA1ECDSA::EVP_MD = Init_SHA1ECDSA_EVP_MD();

SHA1ECDSA::SHA1ECDSA()
{
    OpenSSLSignature::constructor(EVP_MD, EngineType_EC);
}

//=========================================
// SHA224ECDSA::
//=========================================
static Int64 Init_SHA224ECDSA_EVP_MD()
{
    Int64 res = 0;
    NativeCrypto::EVP_get_digestbyname(String("SHA224"), &res);
    return res;
}
Int64 SHA224ECDSA::EVP_MD = Init_SHA224ECDSA_EVP_MD();

SHA224ECDSA::SHA224ECDSA()
{
    OpenSSLSignature::constructor(EVP_MD, EngineType_EC);
}

//=========================================
// SHA256ECDSA::
//=========================================
static Int64 Init_SHA256ECDSA_EVP_MD()
{
    Int64 res = 0;
    NativeCrypto::EVP_get_digestbyname(String("SHA256"), &res);
    return res;
}
Int64 SHA256ECDSA::EVP_MD = Init_SHA256ECDSA_EVP_MD();

SHA256ECDSA::SHA256ECDSA()
{
    OpenSSLSignature::constructor(EVP_MD, EngineType_EC);
}

//=========================================
// SHA384ECDSA::
//=========================================
static Int64 Init_SHA384ECDSA_EVP_MD()
{
    Int64 res = 0;
    NativeCrypto::EVP_get_digestbyname(String("SHA384"), &res);
    return res;
}
Int64 SHA384ECDSA::EVP_MD = Init_SHA384ECDSA_EVP_MD();

SHA384ECDSA::SHA384ECDSA()
{
    OpenSSLSignature::constructor(EVP_MD, EngineType_EC);
}

//=========================================
// SHA512ECDSA::
//=========================================
static Int64 Init_SHA512ECDSA_EVP_MD()
{
    Int64 res = 0;
    NativeCrypto::EVP_get_digestbyname(String("SHA512"), &res);
    return res;
}
Int64 SHA512ECDSA::EVP_MD = Init_SHA512ECDSA_EVP_MD();

SHA512ECDSA::SHA512ECDSA()
{
    SHA512ECDSA::constructor(EVP_MD, EngineType_EC);
}

//=========================================
// OpenSSLSignature::
//=========================================
CAR_INTERFACE_IMPL(OpenSSLSignature, Object, IOpenSSLSignature)

ECode OpenSSLSignature::constructor(
    /* [in] */ Int64 algorithm,
    /* [in] */ EngineType engineType)
{
    mEngineType = engineType;
    mEvpAlgorithm = algorithm;
    mSingleByte = ArrayOf<Byte>::Alloc(1);
    return NOERROR;
}

void OpenSSLSignature::ResetContext()
{
    Int64 create = 0;
    NativeCrypto::EVP_MD_CTX_create(&create);
    AutoPtr<IOpenSSLDigestContext> ctxLocal;
    assert(0 && "TODO");
    // COpenSSLDigestContext::New(create, (IOpenSSLDigestContext**)&ctxLocal);
    NativeCrypto::EVP_MD_CTX_init(ctxLocal);
    if (mSigning) {
        EnableDSASignatureNonceHardeningIfApplicable();
        Int32 ret = 0;
        NativeCrypto::EVP_SignInit(ctxLocal, mEvpAlgorithm, &ret);
    }
    else {
        Int32 ret = 0;
        NativeCrypto::EVP_VerifyInit(ctxLocal, mEvpAlgorithm, &ret);
    }
    mCtx = ctxLocal;
}

ECode OpenSSLSignature::EngineUpdate(
    /* [in] */ Byte input)
{
    (*mSingleByte)[0] = input;
    EngineUpdate(mSingleByte, 0, 1);
    return NOERROR;
}

ECode OpenSSLSignature::EngineUpdate(
    /* [in] */ ArrayOf<Byte>* input,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 len)
{
    AutoPtr<IOpenSSLDigestContext> ctxLocal = mCtx;
    if (mSigning) {
        NativeCrypto::EVP_SignUpdate(ctxLocal, input, offset, len);
    }
    else {
        NativeCrypto::EVP_VerifyUpdate(ctxLocal, input, offset, len);
    }
    return NOERROR;
}

ECode OpenSSLSignature::EngineGetParameter(
    /* [in] */ String param,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    return NOERROR;
}

void OpenSSLSignature::CheckEngineType(
    /* [in] */ IOpenSSLKey* pkey)
{
    Int32 pkeyType = 0;
    Int64 context = 0;
    pkey->GetPkeyContext(&context);
    NativeCrypto::EVP_PKEY_type(context, &pkeyType);

    switch (mEngineType) {
        case EngineType_RSA:
            if (pkeyType != INativeCrypto::EVP_PKEY_RSA) {
                // throw new InvalidKeyException("Signature initialized as " + engineType
                //         + " (not RSA)");
                return;
            }
            break;
        case EngineType_DSA:
            if (pkeyType != INativeCrypto::EVP_PKEY_DSA) {
                // throw new InvalidKeyException("Signature initialized as " + engineType
                //         + " (not DSA)");
                return;
            }
            break;
        case EngineType_EC:
            if (pkeyType != INativeCrypto::EVP_PKEY_EC) {
                // throw new InvalidKeyException("Signature initialized as " + engineType
                //         + " (not EC)");
                return;
            }
            break;
        default: {
            // throw new InvalidKeyException("Key must be of type " + engineType);
            return;
        }
    }
}

void OpenSSLSignature::InitInternal(
    /* [in] */ IOpenSSLKey* newKey,
    /* [in] */ Boolean signing)
{
    CheckEngineType(newKey);
    mKey = newKey;

    mSigning = signing;
    ResetContext();
}

ECode OpenSSLSignature::EngineInitSign(
    /* [in] */ IPrivateKey* privateKey)
{
    InitInternal(OpenSSLKey::FromPrivateKey(privateKey), TRUE);
    return NOERROR;
}

void OpenSSLSignature::EnableDSASignatureNonceHardeningIfApplicable()
{
    AutoPtr<IOpenSSLKey> key = mKey;
    switch (mEngineType) {
        case EngineType_DSA: {
            Int64 context = 0;
            key->GetPkeyContext(&context);
            NativeCrypto::Set_DSA_flag_nonce_from_hash(context);
            break;
        }
        case EngineType_EC: {
            Int64 context = 0;
            key->GetPkeyContext(&context);
            NativeCrypto::EC_KEY_set_nonce_from_hash(context, TRUE);
            break;
        }
        default: {
            return;
            // Hardening not applicable
        }
    }
}

ECode OpenSSLSignature::EngineInitVerify(
    /* [in] */ IPublicKey* publicKey)
{
    InitInternal(OpenSSLKey::FromPublicKey(publicKey), FALSE);
    return NOERROR;
}

ECode OpenSSLSignature::EngineSetParameter(
    /* [in] */ String param,
    /* [in] */ IInterface* value)
{
    return NOERROR;
}

ECode OpenSSLSignature::EngineSign(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)
    if (mKey == NULL) {
        // This can't actually happen, but you never know...
        // throw new SignatureException("Need DSA or RSA or EC private key");
        return E_SIGNATURE_EXCEPTION;
    }

    AutoPtr<IOpenSSLDigestContext> ctxLocal = mCtx;
    // try {
    Int64 context = 0;
    mKey->GetPkeyContext(&context);
    Int32 size = 0;
    NativeCrypto::EVP_PKEY_size(context, &size);
    AutoPtr<ArrayOf<Byte> > buffer = ArrayOf<Byte>::Alloc(size);
    Int32 bytesWritten = 0;
    NativeCrypto::EVP_SignFinal(ctxLocal, buffer, 0,
            context, &bytesWritten);

    AutoPtr<ArrayOf<Byte> > signature = ArrayOf<Byte>::Alloc(bytesWritten);
    signature->Copy(0, buffer, 0, bytesWritten);
    /*
     * Java expects the digest context to be reset completely after sign
     * calls.
     */
    ResetContext();
    *result = signature;
    return NOERROR;
    // } catch (Exception ex) {
    //     throw new SignatureException(ex);
    // } finally {
    // }
}

ECode OpenSSLSignature::EngineVerify(
    /* [in] */ ArrayOf<Byte>* sigBytes,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (mKey == NULL) {
        // This can't actually happen, but you never know...
        // throw new SignatureException("Need DSA or RSA public key");
        return E_SIGNATURE_EXCEPTION;
    }

    // try {
    Int64 context = 0;
    mKey->GetPkeyContext(&context);
    Int32 res = 0;
    NativeCrypto::EVP_VerifyFinal(mCtx, sigBytes, 0, sigBytes->GetLength(),
            context, &res);
    /*
     * Java expects the digest context to be reset completely after
     * verify calls.
     */
    ResetContext();
    *result = res == 1;
    return NOERROR;
    // } catch (Exception ex) {
    //     return false;
    // } finally {
    // }
}

} // namespace Conscrypt
} // namespace Org
