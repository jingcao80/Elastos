#include "org/conscrypt/OpenSSLCipherRSA.h"
#include "org/conscrypt/NativeCrypto.h"
#include "org/conscrypt/OpenSSLRSAPrivateCrtKey.h"
#include "org/conscrypt/OpenSSLRSAPrivateKey.h"
#include "org/conscrypt/OpenSSLRSAPublicKey.h"
#include <libcore/utility/EmptyArray.h>
#include <elastos/utility/Arrays.h>

using Elastos::Security::CKeyFactoryHelper;
using Elastos::Security::IKeyFactory;
using Elastos::Security::IKeyFactoryHelper;
using Elastos::Security::Interfaces::IRSAPrivateCrtKey;
using Elastos::Security::Interfaces::IRSAPublicKey;
using Elastos::Security::IPrivateKey;
using Elastos::Security::IPublicKey;
using Elastos::Security::Spec::CX509EncodedKeySpec;
using Elastos::Security::Spec::IKeySpec;
using Elastos::Security::Spec::IX509EncodedKeySpec;
using Elastos::Utility::Arrays;
using Elastosx::Crypto::ICipher;
using Elastosx::Crypto::Spec::CSecretKeySpec;
using Elastosx::Crypto::Spec::ISecretKeySpec;
using Libcore::Utility::EmptyArray;

namespace Org {
namespace Conscrypt {

//==============================================================================
//  OpenSSLCipherRSA_PKCS1
//==============================================================================

OpenSSLCipherRSA_PKCS1::OpenSSLCipherRSA_PKCS1()
{
    OpenSSLCipherRSA::constructor(INativeCrypto::RSA_PKCS1_PADDING);
}

ECode OpenSSLCipherRSA_PKCS1::constructor()
{
    return NOERROR;
}

//==============================================================================
//  OpenSSLCipherRSA_Raw
//==============================================================================

OpenSSLCipherRSA_Raw::OpenSSLCipherRSA_Raw()
{
    OpenSSLCipherRSA::constructor(INativeCrypto::RSA_NO_PADDING);
}

ECode OpenSSLCipherRSA_Raw::constructor()
{
    return NOERROR;
}
//==============================================================================
//  OpenSSLCipherRSA
//==============================================================================

CAR_INTERFACE_IMPL(OpenSSLCipherRSA, CipherSpi, IOpenSSLCipherRSA)

OpenSSLCipherRSA::OpenSSLCipherRSA()
    : mUsingPrivateKey(FALSE)
    , mEncrypting(FALSE)
    , mBufferOffset(0)
    , mInputTooLarge(FALSE)
    , mPadding(INativeCrypto::RSA_PKCS1_PADDING)
{}

ECode OpenSSLCipherRSA::constructor(
    /* [in] */ Int32 padding)
{
    mPadding = padding;
    return NOERROR;
}

ECode OpenSSLCipherRSA::EngineSetMode(
    /* [in] */ const String& mode)
{
    String modeUpper = mode.ToUpperCase(/*Locale.ROOT*/);
    if (modeUpper.Equals("NONE") || modeUpper.Equals("ECB")) {
        return NOERROR;
    }

    // throw new NoSuchAlgorithmException("mode not supported: " + mode);
    return E_NO_SUCH_ALGORITHM_EXCEPTION;
}

ECode OpenSSLCipherRSA::EngineSetPadding(
    /* [in] */ const String& padding)
{
    String paddingUpper = padding.ToUpperCase(/*Locale.ROOT*/);
    if (paddingUpper.Equals("PKCS1PADDING")) {
        mPadding = INativeCrypto::RSA_PKCS1_PADDING;
        return NOERROR;
    }
    if (paddingUpper.Equals("NOPADDING")) {
        mPadding = INativeCrypto::RSA_NO_PADDING;
        return NOERROR;
    }

    // throw new NoSuchPaddingException("padding not supported: " + padding);
    return E_NO_SUCH_ALGORITHM_EXCEPTION;
}

ECode OpenSSLCipherRSA::EngineGetBlockSize(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    if (mEncrypting) {
        *result = PaddedBlockSizeBytes();
        return NOERROR;
    }

    KeySizeBytes(result);
    return NOERROR;
}

ECode OpenSSLCipherRSA::EngineGetOutputSize(
    /* [in] */ Int32 inputLen,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    if (mEncrypting) {
        KeySizeBytes(result);
        return NOERROR;
    }

    *result = PaddedBlockSizeBytes();
    return NOERROR;
}

ECode OpenSSLCipherRSA::EngineGetIV(
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    return NOERROR;
}

ECode OpenSSLCipherRSA::EngineGetParameters(
    /* [out] */ IAlgorithmParameters** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    return NOERROR;
}

ECode OpenSSLCipherRSA::EngineInit(
    /* [in] */ Int32 opmode,
    /* [in] */ IKey* key,
    /* [in] */ ISecureRandom* random)
{
    EngineInitInternal(opmode, key);
    return NOERROR;
}

ECode OpenSSLCipherRSA::EngineInit(
    /* [in] */ Int32 opmode,
    /* [in] */ IKey* key,
    /* [in] */ IAlgorithmParameterSpec* params,
    /* [in] */ ISecureRandom* random)
{
    if (params != NULL) {
        // throw new InvalidAlgorithmParameterException("unknown param type: "
        //         + params.getClass().getName());
        return E_INVALID_ALGORITHM_PARAMETER_EXCEPTION;
    }

    EngineInitInternal(opmode, key);
    return NOERROR;
}

ECode OpenSSLCipherRSA::EngineInit(
    /* [in] */ Int32 opmode,
    /* [in] */ IKey* key,
    /* [in] */ IAlgorithmParameters* params,
    /* [in] */ ISecureRandom* random)
{
    if (params != NULL) {
        // throw new InvalidAlgorithmParameterException("unknown param type: "
        //         + params.getClass().getName());
        return E_INVALID_ALGORITHM_PARAMETER_EXCEPTION;
    }

    EngineInitInternal(opmode, key);
    return NOERROR;
}

ECode OpenSSLCipherRSA::EngineUpdate(
    /* [in] */ ArrayOf<Byte>* input,
    /* [in] */ Int32 inputOffset,
    /* [in] */ Int32 inputLen,
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)
    if (mBufferOffset + inputLen > mBuffer->GetLength()) {
        mInputTooLarge = TRUE;
        *result = EmptyArray::BYTE;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }

    mBuffer->Copy(mBufferOffset, input, inputOffset, inputLen);
    mBufferOffset += inputLen;
    *result = EmptyArray::BYTE;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLCipherRSA::EngineUpdate(
    /* [in] */ ArrayOf<Byte>* input,
    /* [in] */ Int32 inputOffset,
    /* [in] */ Int32 inputLen,
    /* [in] */ ArrayOf<Byte>* output,
    /* [in] */ Int32 outputOffset,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ArrayOf<Byte> > ret;
    EngineUpdate(input, inputOffset, inputLen, (ArrayOf<Byte>**)&ret);
    *result = 0;
    return NOERROR;
}

ECode OpenSSLCipherRSA::EngineDoFinal(
    /* [in] */ ArrayOf<Byte>* input,
    /* [in] */ Int32 inputOffset,
    /* [in] */ Int32 inputLen,
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)
    if (input != NULL) {
        AutoPtr<ArrayOf<Byte> > ret;
        EngineUpdate(input, inputOffset, inputLen, (ArrayOf<Byte>**)&ret);
    }

    if (mInputTooLarge) {
        // throw new IllegalBlockSizeException("input must be under " + mBuffer->GetLength() + " bytes");
        return E_ILLEGAL_BLOCK_SIZE_EXCEPTION;
    }

    AutoPtr<ArrayOf<Byte> > tmpBuf;
    if (mBufferOffset != mBuffer->GetLength()) {
        if (mPadding == INativeCrypto::RSA_NO_PADDING) {
            tmpBuf = ArrayOf<Byte>::Alloc(mBuffer->GetLength());
            tmpBuf->Copy(mBuffer->GetLength() - mBufferOffset, mBuffer, 0, mBufferOffset);
        }
        else {
            Arrays::CopyOf(mBuffer, mBufferOffset, (ArrayOf<Byte>**)&tmpBuf);
        }
    }
    else {
        tmpBuf = mBuffer;
    }

    AutoPtr<ArrayOf<Byte> > output = ArrayOf<Byte>::Alloc(mBuffer->GetLength());
    Int64 keyContext;
    mKey->GetPkeyContext(&keyContext);
    Int32 resultSize;
    if (mEncrypting) {
        if (mUsingPrivateKey) {
            NativeCrypto::RSA_private_encrypt(
                    tmpBuf->GetLength(), tmpBuf, output, keyContext, mPadding, &resultSize);
        }
        else {
            NativeCrypto::RSA_public_encrypt(
                    tmpBuf->GetLength(), tmpBuf, output, keyContext, mPadding, &resultSize);
        }
    }
    else {
        // try {
        if (mUsingPrivateKey) {
            NativeCrypto::RSA_private_decrypt(
                    tmpBuf->GetLength(), tmpBuf, output, keyContext, mPadding, &resultSize);
        }
        else {
            NativeCrypto::RSA_public_decrypt(
                    tmpBuf->GetLength(), tmpBuf, output, keyContext, mPadding, &resultSize);
        }
        // } catch (SignatureException e) {
        //     IllegalBlockSizeException newE = new IllegalBlockSizeException();
        //     newE.initCause(e);
        //     throw newE;
        // }
    }
    if (!mEncrypting && resultSize != output->GetLength()) {
        Arrays::CopyOf(output, resultSize, (ArrayOf<Byte>**)&output);
    }

    mBufferOffset = 0;
    *result = output;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLCipherRSA::EngineDoFinal(
    /* [in] */ ArrayOf<Byte>* input,
    /* [in] */ Int32 inputOffset,
    /* [in] */ Int32 inputLen,
    /* [in] */ ArrayOf<Byte>* output,
    /* [in] */ Int32 outputOffset,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ArrayOf<Byte> > b;
    EngineDoFinal(input, inputOffset, inputLen, (ArrayOf<Byte>**)&b);

    Int32 lastOffset = outputOffset + b->GetLength();
    if (lastOffset > output->GetLength()) {
        // throw new ShortBufferException(
        //     "output buffer is too small " + output->GetLength() + " < " + lastOffset);
        return E_SHORT_BUFFER_EXCEPTION;
    }

    output->Copy(outputOffset, b, 0, b->GetLength());
    *result = b->GetLength();
    return NOERROR;
}

ECode OpenSSLCipherRSA::EngineWrap(
    /* [in] */ IKey* key,
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)
    // try {
    AutoPtr<ArrayOf<Byte> > encoded;
    key->GetEncoded((ArrayOf<Byte>**)&encoded);
    return EngineDoFinal(encoded, 0, encoded->GetLength(), result);
    // } catch (BadPaddingException e) {
    //     IllegalBlockSizeException newE = new IllegalBlockSizeException();
    //     newE.initCause(e);
    //     throw newE;
    // }
}

ECode OpenSSLCipherRSA::EngineUnwrap(
    /* [in] */ ArrayOf<Byte>* wrappedKey,
    /* [in] */ const String& wrappedKeyAlgorithm,
    /* [in] */ Int32 wrappedKeyType,
    /* [out] */ IKey** result)
{
    VALIDATE_NOT_NULL(result)
    // try {
    AutoPtr<ArrayOf<Byte> > encoded;
    EngineDoFinal(wrappedKey, 0, wrappedKey->GetLength(), (ArrayOf<Byte>**)&encoded);

    AutoPtr<IKeyFactoryHelper> kfHelper;
    CKeyFactoryHelper::AcquireSingleton((IKeyFactoryHelper**)&kfHelper);
    AutoPtr<IKeyFactory> keyFactory;
    kfHelper->GetInstance(wrappedKeyAlgorithm, (IKeyFactory**)&keyFactory);
    AutoPtr<IX509EncodedKeySpec> spec;
    CX509EncodedKeySpec::New(encoded, (IX509EncodedKeySpec**)&spec);

    if (wrappedKeyType == ICipher::PUBLIC_KEY) {
        AutoPtr<IPublicKey> pk;
        keyFactory->GeneratePublic(IKeySpec::Probe(spec), (IPublicKey**)&pk);
        *result = IKey::Probe(pk);
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    else if (wrappedKeyType == ICipher::PRIVATE_KEY) {
        AutoPtr<IPrivateKey> pk;
        keyFactory->GeneratePrivate(IKeySpec::Probe(spec), (IPrivateKey**)&pk);
        *result = IKey::Probe(pk);
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    else if (wrappedKeyType == ICipher::SECRET_KEY) {
        AutoPtr<ISecretKeySpec> sks;
        CSecretKeySpec::New(encoded, wrappedKeyAlgorithm, (ISecretKeySpec**)&sks);
        *result = IKey::Probe(sks);
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    else {
        // throw new UnsupportedOperationException("wrappedKeyType == " + wrappedKeyType);
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    // } catch (IllegalBlockSizeException e) {
    //     throw new InvalidKeyException(e);
    // } catch (BadPaddingException e) {
    //     throw new InvalidKeyException(e);
    // } catch (InvalidKeySpecException e) {
    //     throw new InvalidKeyException(e);
    // }
    return NOERROR;
}

Int32 OpenSSLCipherRSA::PaddedBlockSizeBytes()
{
    Int32 paddedBlockSizeBytes;
    KeySizeBytes(&paddedBlockSizeBytes);
    if (mPadding == INativeCrypto::RSA_PKCS1_PADDING) {
        paddedBlockSizeBytes--;  // for 0 prefix
        paddedBlockSizeBytes -= 10;  // PKCS1 padding header length
    }
    return paddedBlockSizeBytes;
}

ECode OpenSSLCipherRSA::KeySizeBytes(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    if (mKey == NULL) {
        // throw new IllegalStateException("cipher is not initialized");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    Int64 keyContext;
    mKey->GetPkeyContext(&keyContext);
    return NativeCrypto::RSA_size(keyContext, result);
}

ECode OpenSSLCipherRSA::EngineInitInternal(
    /* [in] */ Int32 opmode,
    /* [in] */ IKey* key)
{
    if (opmode == ICipher::ENCRYPT_MODE || opmode == ICipher::WRAP_MODE) {
        mEncrypting = TRUE;
    }
    else if (opmode == ICipher::DECRYPT_MODE || opmode == ICipher::UNWRAP_MODE) {
        mEncrypting = FALSE;
    }
    else {
        // throw new InvalidParameterException("Unsupported opmode " + opmode);
        return E_INVALID_PARAMETER_EXCEPTION;
    }

    if (IOpenSSLRSAPrivateKey::Probe(key) != NULL) {
        AutoPtr<IOpenSSLRSAPrivateKey> rsaPrivateKey = IOpenSSLRSAPrivateKey::Probe(key);
        mUsingPrivateKey = TRUE;
        IOpenSSLKeyHolder::Probe(rsaPrivateKey)->GetOpenSSLKey((IOpenSSLKey**)&mKey);
    }
    else if (IRSAPrivateCrtKey::Probe(key) != NULL) {
        AutoPtr<IRSAPrivateCrtKey> rsaPrivateKey = IRSAPrivateCrtKey::Probe(key);
        mUsingPrivateKey = TRUE;
        OpenSSLRSAPrivateCrtKey::GetInstance(rsaPrivateKey, (IOpenSSLKey**)&mKey);
    }
    else if (IRSAPrivateKey::Probe(key) != NULL) {
        AutoPtr<IRSAPrivateKey> rsaPrivateKey = IRSAPrivateKey::Probe(key);
        mUsingPrivateKey = TRUE;
        mKey = OpenSSLRSAPrivateKey::GetInstance(rsaPrivateKey);
    }
    else if (IOpenSSLRSAPublicKey::Probe(key) != NULL) {
        AutoPtr<IOpenSSLRSAPublicKey> rsaPublicKey = IOpenSSLRSAPublicKey::Probe(key);
        mUsingPrivateKey = FALSE;
        IOpenSSLKeyHolder::Probe(rsaPublicKey)->GetOpenSSLKey((IOpenSSLKey**)&mKey);
    }
    else if (IRSAPublicKey::Probe(key) != NULL) {
        AutoPtr<IRSAPublicKey> rsaPublicKey = IRSAPublicKey::Probe(key);
        mUsingPrivateKey = FALSE;
        OpenSSLRSAPublicKey::GetInstance(rsaPublicKey, (IOpenSSLKey**)&mKey);
    }
    else {
        // throw new InvalidKeyException("Need RSA private or public key");
        return E_INVALID_KEY_EXCEPTION;
    }

    Int64 keyContext;
    mKey->GetPkeyContext(&keyContext);
    Int32 size;
    NativeCrypto::RSA_size(keyContext, &size);
    mBuffer = ArrayOf<Byte>::Alloc(size);
    mInputTooLarge = FALSE;
    return NOERROR;
}

} // namespace Conscrypt
} // namespace Org
