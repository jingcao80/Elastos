
#include "OpenSSLSignature.h"
#include "NativeCrypto.h"
#include "OpenSSLKey.h"

namespace Org {
namespace Conscrypt {

//=========================================
// OpenSSLSignatureMD5RSA::
//=========================================
CAR_INTERFACE_IMPL(OpenSSLSignatureMD5RSA, OpenSSLSignature,
        IOpenSSLSignatureMD5RSA)

static Int64 Init_MD5RSA_EVP_MD()
{
    Int64 res = 0;
    NativeCrypto::EVP_get_digestbyname(String("RSA-MD5"), &res);
    return res;
}
Int64 OpenSSLSignatureMD5RSA::EVP_MD = Init_MD5RSA_EVP_MD();

ECode OpenSSLSignatureMD5RSA::constructor()
{
    return OpenSSLSignature::constructor(EVP_MD, EngineType_RSA);
}

//=========================================
// OpenSSLSignatureSHA1RSA::
//=========================================
CAR_INTERFACE_IMPL(OpenSSLSignatureSHA1RSA, OpenSSLSignature,
        IOpenSSLSignatureSHA1RSA)

static Int64 Init_SHA1RSA_EVP_MD()
{
    Int64 res = 0;
    NativeCrypto::EVP_get_digestbyname(String("RSA-SHA1"), &res);
    return res;
}
Int64 OpenSSLSignatureSHA1RSA::EVP_MD = Init_SHA1RSA_EVP_MD();

ECode OpenSSLSignatureSHA1RSA::constructor()
{
    return OpenSSLSignature::constructor(EVP_MD, EngineType_RSA);
}

//=========================================
// OpenSSLSignatureSHA224RSA::
//=========================================
CAR_INTERFACE_IMPL(OpenSSLSignatureSHA224RSA, OpenSSLSignature,
        IOpenSSLSignatureSHA224RSA)

static Int64 Init_SHA224RSA_EVP_MD()
{
    Int64 res = 0;
    NativeCrypto::EVP_get_digestbyname(String("RSA-SHA224"), &res);
    return res;
}
Int64 OpenSSLSignatureSHA224RSA::EVP_MD = Init_SHA224RSA_EVP_MD();

ECode OpenSSLSignatureSHA224RSA::constructor()
{
    return OpenSSLSignature::constructor(EVP_MD, EngineType_RSA);
}

//=========================================
// OpenSSLSignatureSHA256RSA::
//=========================================
CAR_INTERFACE_IMPL(OpenSSLSignatureSHA256RSA, OpenSSLSignature,
        IOpenSSLSignatureSHA256RSA)

static Int64 Init_SHA256RSA_EVP_MD()
{
    Int64 res = 0;
    NativeCrypto::EVP_get_digestbyname(String("RSA-SHA256"), &res);
    return res;
}
Int64 OpenSSLSignatureSHA256RSA::EVP_MD = Init_SHA256RSA_EVP_MD();

ECode OpenSSLSignatureSHA256RSA::constructor()
{
    return OpenSSLSignature::constructor(EVP_MD, EngineType_RSA);
}

//=========================================
// OpenSSLSignatureSHA384RSA::
//=========================================
CAR_INTERFACE_IMPL(OpenSSLSignatureSHA384RSA, OpenSSLSignature,
        IOpenSSLSignatureSHA384RSA)

static Int64 Init_SHA384RSA_EVP_MD()
{
    Int64 res = 0;
    NativeCrypto::EVP_get_digestbyname(String("RSA-SHA384"), &res);
    return res;
}
Int64 OpenSSLSignatureSHA384RSA::EVP_MD = Init_SHA384RSA_EVP_MD();

ECode OpenSSLSignatureSHA384RSA::constructor()
{
    return OpenSSLSignature::constructor(EVP_MD, EngineType_RSA);
}

//=========================================
// OpenSSLSignatureSHA512RSA::
//=========================================
CAR_INTERFACE_IMPL(OpenSSLSignatureSHA512RSA, OpenSSLSignature,
        IOpenSSLSignatureSHA512RSA)

static Int64 Init_SHA512RSA_EVP_MD()
{
    Int64 res = 0;
    NativeCrypto::EVP_get_digestbyname(String("RSA-SHA512"), &res);
    return res;
}
Int64 OpenSSLSignatureSHA512RSA::EVP_MD = Init_SHA512RSA_EVP_MD();

ECode OpenSSLSignatureSHA512RSA::constructor()
{
    return OpenSSLSignature::constructor(EVP_MD, EngineType_RSA);
}

//=========================================
// OpenSSLSignatureSHA1DSA::
//=========================================
CAR_INTERFACE_IMPL(OpenSSLSignatureSHA1DSA, OpenSSLSignature,
        IOpenSSLSignatureSHA1DSA)

static Int64 Init_SHA1DSA_EVP_MD()
{
    Int64 res = 0;
    NativeCrypto::EVP_get_digestbyname(String("DSA-SHA1"), &res);
    return res;
}
Int64 OpenSSLSignatureSHA1DSA::EVP_MD = Init_SHA1DSA_EVP_MD();

ECode OpenSSLSignatureSHA1DSA::constructor()
{
    return OpenSSLSignature::constructor(EVP_MD, EngineType_DSA);
}

//=========================================
// OpenSSLSignatureSHA1ECDSA::
//=========================================
CAR_INTERFACE_IMPL(OpenSSLSignatureSHA1ECDSA, OpenSSLSignature,
        IOpenSSLSignatureSHA1ECDSA)

static Int64 Init_SHA1ECDSA_EVP_MD()
{
    Int64 res = 0;
    NativeCrypto::EVP_get_digestbyname(String("SHA1"), &res);
    return res;
}
Int64 OpenSSLSignatureSHA1ECDSA::EVP_MD = Init_SHA1ECDSA_EVP_MD();

ECode OpenSSLSignatureSHA1ECDSA::constructor()
{
    return OpenSSLSignature::constructor(EVP_MD, EngineType_EC);
}

//=========================================
// OpenSSLSignatureSHA224ECDSA::
//=========================================
CAR_INTERFACE_IMPL(OpenSSLSignatureSHA224ECDSA, OpenSSLSignature,
        IOpenSSLSignatureSHA224ECDSA)

static Int64 Init_SHA224ECDSA_EVP_MD()
{
    Int64 res = 0;
    NativeCrypto::EVP_get_digestbyname(String("SHA224"), &res);
    return res;
}
Int64 OpenSSLSignatureSHA224ECDSA::EVP_MD = Init_SHA224ECDSA_EVP_MD();

ECode OpenSSLSignatureSHA224ECDSA::constructor()
{
    return OpenSSLSignature::constructor(EVP_MD, EngineType_EC);
}

//=========================================
// OpenSSLSignatureSHA256ECDSA::
//=========================================
CAR_INTERFACE_IMPL(OpenSSLSignatureSHA256ECDSA, OpenSSLSignature,
        IOpenSSLSignatureSHA256ECDSA)

static Int64 Init_SHA256ECDSA_EVP_MD()
{
    Int64 res = 0;
    NativeCrypto::EVP_get_digestbyname(String("SHA256"), &res);
    return res;
}
Int64 OpenSSLSignatureSHA256ECDSA::EVP_MD = Init_SHA256ECDSA_EVP_MD();

ECode OpenSSLSignatureSHA256ECDSA::constructor()
{
    return OpenSSLSignature::constructor(EVP_MD, EngineType_EC);
}

//=========================================
// OpenSSLSignatureSHA384ECDSA::
//=========================================
CAR_INTERFACE_IMPL(OpenSSLSignatureSHA384ECDSA, OpenSSLSignature,
        IOpenSSLSignatureSHA384ECDSA)

static Int64 Init_SHA384ECDSA_EVP_MD()
{
    Int64 res = 0;
    NativeCrypto::EVP_get_digestbyname(String("SHA384"), &res);
    return res;
}
Int64 OpenSSLSignatureSHA384ECDSA::EVP_MD = Init_SHA384ECDSA_EVP_MD();

ECode OpenSSLSignatureSHA384ECDSA::constructor()
{
    return OpenSSLSignature::constructor(EVP_MD, EngineType_EC);
}

//=========================================
// OpenSSLSignatureSHA512ECDSA::
//=========================================
CAR_INTERFACE_IMPL(OpenSSLSignatureSHA512ECDSA, OpenSSLSignature,
        IOpenSSLSignatureSHA512ECDSA)

static Int64 Init_SHA512ECDSA_EVP_MD()
{
    Int64 res = 0;
    NativeCrypto::EVP_get_digestbyname(String("SHA512"), &res);
    return res;
}
Int64 OpenSSLSignatureSHA512ECDSA::EVP_MD = Init_SHA512ECDSA_EVP_MD();

ECode OpenSSLSignatureSHA512ECDSA::constructor()
{
    return OpenSSLSignature::constructor(EVP_MD, EngineType_EC);
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
