#include "org/conscrypt/OpenSSLCipher.h"
#include "org/conscrypt/NativeCrypto.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/Arrays.h>
#include <libcore/utility/EmptyArray.h>

using Elastos::Core::StringUtils;
using Elastos::Security::CAlgorithmParametersHelper;
using Elastos::Security::CKeyFactoryHelper;
using Elastos::Security::CSecureRandom;
using Elastos::Security::IAlgorithmParametersHelper;
using Elastos::Security::IKeyFactory;
using Elastos::Security::IKeyFactoryHelper;
using Elastos::Security::IPrivateKey;
using Elastos::Security::IPublicKey;
using Elastos::Security::Spec::CPKCS8EncodedKeySpec;
using Elastos::Security::Spec::CX509EncodedKeySpec;
using Elastos::Security::Spec::IKeySpec;
using Elastos::Security::Spec::IPKCS8EncodedKeySpec;
using Elastos::Security::Spec::IX509EncodedKeySpec;
using Elastos::Utility::Arrays;
using Elastos::Utility::IRandom;
using Elastosx::Crypto::ICipher;
using Elastosx::Crypto::ISecretKey;
using Elastosx::Crypto::Spec::CSecretKeySpec;
using Elastosx::Crypto::Spec::IIvParameterSpec;
using Elastosx::Crypto::Spec::ISecretKeySpec;
using Libcore::Utility::EmptyArray;

namespace Org {
namespace Conscrypt {

//==============================================================================
//  OpenSSLCipher
//==============================================================================

CAR_INTERFACE_IMPL(OpenSSLCipher, CipherSpi, IOpenSSLCipher)

OpenSSLCipher::Mode OpenSSLCipher::ModeValueOf(
    /* [in] */ const String& str)
{
    if (str.Equals(String("MODE_CBC"))) {
        return MODE_CBC;
    }
    else if (str.Equals(String("MODE_CFB"))) {
        return MODE_CFB;
    }
    else if (str.Equals(String("MODE_CFB1"))) {
        return MODE_CFB1;
    }
    else if (str.Equals(String("MODE_CFB8"))) {
        return MODE_CFB8;
    }
    else if (str.Equals(String("MODE_CFB128"))) {
        return MODE_CFB128;
    }
    else if (str.Equals(String("MODE_CTR"))) {
        return MODE_CTR;
    }
    else if (str.Equals(String("MODE_CTS"))) {
        return MODE_CTS;
    }
    else if (str.Equals(String("MODE_ECB"))) {
        return MODE_ECB;
    }
    else if (str.Equals(String("MODE_OFB"))) {
        return MODE_OFB;
    }
    else if (str.Equals(String("MODE_OFB64"))) {
        return MODE_OFB64;
    }
    else if (str.Equals(String("MODE_OFB128"))) {
        return MODE_OFB128;
    }
    else if (str.Equals(String("MODE_PCBC"))) {
        return MODE_PCBC;
    }
    return MODE_CBC;
}

String OpenSSLCipher::ModeToString(
    /* [in] */ OpenSSLCipher::Mode mode)
{
    switch (mode) {
        case MODE_CBC: return String("CBC");
        case MODE_CFB: return String("CFB");
        case MODE_CFB1: return String("CFB1");
        case MODE_CFB8: return String("CFB8");
        case MODE_CFB128: return String("CFB128");
        case MODE_CTR: return String("CTR");
        case MODE_CTS: return String("CTS");
        case MODE_ECB: return String("ECB");
        case MODE_OFB: return String("OFB");
        case MODE_OFB64: return String("OFB64");
        case MODE_OFB128: return String("OFB128");
        case MODE_PCBC: return String("PCBC");
    }
    return String(NULL);
}

OpenSSLCipher::Padding OpenSSLCipher::PaddingValueOf(
    /* [in] */ const String& str)
{
    if (str.Equals(String("PADDING_NOPADDING"))) {
        return PADDING_NOPADDING;
    }
    else if (str.Equals(String("PADDING_PKCS5PADDING"))) {
        return PADDING_PKCS5PADDING;
    }
    else if (str.Equals(String("PADDING_ISO10126PADDING"))) {
        return PADDING_ISO10126PADDING;
    }
    return PADDING_NOPADDING;
}

String OpenSSLCipher::PaddingToString(
    /* [in] */ OpenSSLCipher::Padding padding)
{
    switch (padding) {
        case PADDING_NOPADDING: return String("PADDING_NOPADDING");
        case PADDING_PKCS5PADDING: return String("PADDING_PKCS5PADDING");
        case PADDING_ISO10126PADDING: return String("PADDING_ISO10126PADDING");
    }
    return String(NULL);
}

OpenSSLCipher::OpenSSLCipher()
    : mMode(MODE_ECB)
    , mPadding(PADDING_PKCS5PADDING)
    , mEncrypting(FALSE)
    , mBlockSize(0)
    , mModeBlockSize(0)
    , mCalledUpdate(FALSE)
{
    Int64 val;
    NativeCrypto::EVP_CIPHER_CTX_new(&val);
    mCipherCtx = new OpenSSLCipherContext();
    mCipherCtx->constructor(val);
}

ECode OpenSSLCipher::constructor(
    /* [in] */ Mode mode,
    /* [in] */ Padding padding)
{
    mMode = mode;
    mPadding = padding;
    GetCipherBlockSize(&mBlockSize);
    return NOERROR;
}

ECode OpenSSLCipher::SupportsVariableSizeKey(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

ECode OpenSSLCipher::EngineSetMode(
    /* [in] */ const String& modeStr)
{
    Mode mode;
    // try {
    mode = ModeValueOf(modeStr.ToUpperCase(/*Locale.US*/));
    // } catch (IllegalArgumentException e) {
    //     NoSuchAlgorithmException newE = new NoSuchAlgorithmException("No such mode: "
    //             + modeStr);
    //     newE.initCause(e);
    //     throw newE;
    // }
    CheckSupportedMode(mode);
    mMode = mode;
    return NOERROR;
}

ECode OpenSSLCipher::EngineSetPadding(
    /* [in] */ const String& paddingStr)
{
    String paddingStrUpper = paddingStr.ToUpperCase(/*Locale.US*/);
    Padding padding;
    // try {
    padding = PaddingValueOf(paddingStrUpper);
    // } catch (IllegalArgumentException e) {
    //     NoSuchPaddingException newE = new NoSuchPaddingException("No such padding: "
    //             + paddingStr);
    //     newE.initCause(e);
    //     throw newE;
    // }
    CheckSupportedPadding(padding);
    mPadding = padding;
    return NOERROR;
}

ECode OpenSSLCipher::EngineGetBlockSize(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mBlockSize;
    return NOERROR;
}

ECode OpenSSLCipher::EngineGetOutputSize(
    /* [in] */ Int32 inputLen,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = GetOutputSize(inputLen);
    return NOERROR;
}

ECode OpenSSLCipher::EngineGetIV(
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mIv;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLCipher::EngineGetParameters(
    /* [out] */ IAlgorithmParameters** result)
{
    VALIDATE_NOT_NULL(result)
    if (mIv != NULL && mIv->GetLength() > 0) {
        // try {
        String name;
        GetBaseCipherName(&name);
        AutoPtr<IAlgorithmParametersHelper> helper;
        CAlgorithmParametersHelper::AcquireSingleton(
                (IAlgorithmParametersHelper**)&helper);
        AutoPtr<IAlgorithmParameters> params;
        helper->GetInstance(name, (IAlgorithmParameters**)&params);
        params->Init(mIv);
        *result = params;
        REFCOUNT_ADD(*result)
        return NOERROR;
        // } catch (NoSuchAlgorithmException e) {
        //     return NULL;
        // } catch (IOException e) {
        //     return NULL;
        // }
    }
    *result = NULL;
    return NOERROR;
}

ECode OpenSSLCipher::EngineInit(
    /* [in] */ Int32 opmode,
    /* [in] */ IKey* key,
    /* [in] */ ISecureRandom* random)
{
    // try {
    return EngineInitInternal(opmode, key, NULL, random);
    // } catch (InvalidAlgorithmParameterException e) {
    //     throw new RuntimeException(e);
    // }
}

ECode OpenSSLCipher::EngineInit(
    /* [in] */ Int32 opmode,
    /* [in] */ IKey* key,
    /* [in] */ IAlgorithmParameterSpec* params,
    /* [in] */ ISecureRandom* random)
{
    AutoPtr<ArrayOf<Byte> > iv;
    if (IIvParameterSpec::Probe(params) != NULL) {
        AutoPtr<IIvParameterSpec> ivParams = IIvParameterSpec::Probe(params);
        ivParams->GetIV((ArrayOf<Byte>**)&iv);
    }
    else {
        iv = NULL;
    }

    return EngineInitInternal(opmode, key, iv, random);
}

ECode OpenSSLCipher::EngineInit(
    /* [in] */ Int32 opmode,
    /* [in] */ IKey* key,
    /* [in] */ IAlgorithmParameters* params,
    /* [in] */ ISecureRandom* random)
{
    AutoPtr<IAlgorithmParameterSpec> spec;
    if (params != NULL) {
        // try {
// TODO: Need IvParameterSpec.class
        // spec = params->GetParameterSpec(IvParameterSpec.class);
        // } catch (InvalidParameterSpecException e) {
        //     throw new InvalidAlgorithmParameterException(e);
        // }
    } else {
        spec = NULL;
    }

    EngineInit(opmode, key, spec, random);
    return NOERROR;
}

ECode OpenSSLCipher::EngineUpdate(
    /* [in] */ ArrayOf<Byte>* input,
    /* [in] */ Int32 inputOffset,
    /* [in] */ Int32 inputLen,
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    Int32 maximumLen = GetOutputSize(inputLen);

    /* See how large our output buffer would need to be. */
    AutoPtr<ArrayOf<Byte> > output;
    if (maximumLen > 0) {
        output = ArrayOf<Byte>::Alloc(maximumLen);
    }
    else {
        output = EmptyArray::BYTE;
    }

    Int32 bytesWritten;
    // try {
    ECode ec = UpdateInternal(input, inputOffset,
            inputLen, output, 0, maximumLen, &bytesWritten);
    if (FAILED(ec)) {
        return E_RUNTIME_EXCEPTION;
    }
    // } catch (ShortBufferException e) {
    //     /* This shouldn't happen. */
    //     throw new RuntimeException("calculated buffer size was wrong: " + maximumLen);
    // }

    if (output->GetLength() == bytesWritten) {
        *result = output;
        REFCOUNT_ADD(*result)
    }
    else if (bytesWritten == 0) {
        *result = EmptyArray::BYTE;
        REFCOUNT_ADD(*result)
    }
    else {
        Arrays::CopyOfRange(output, 0, bytesWritten, result);
    }
    return NOERROR;
}

ECode OpenSSLCipher::EngineUpdate(
    /* [in] */ ArrayOf<Byte>* input,
    /* [in] */ Int32 inputOffset,
    /* [in] */ Int32 inputLen,
    /* [in] */ ArrayOf<Byte>* output,
    /* [in] */ Int32 outputOffset,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 maximumLen = GetOutputSize(inputLen);
    return UpdateInternal(input, inputOffset,
            inputLen, output, outputOffset, maximumLen, result);
}

ECode OpenSSLCipher::EngineDoFinal(
    /* [in] */ ArrayOf<Byte>* input,
    /* [in] */ Int32 inputOffset,
    /* [in] */ Int32 inputLen,
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    /*
     * Other implementations return NULL if we've never called update()
     * while decrypting.
     */
    if (!mEncrypting && !mCalledUpdate && inputLen == 0) {
        Reset();
        return NOERROR;
    }

    Int32 maximumLen = GetOutputSize(inputLen);
    /* Assume that we'll output exactly on a byte boundary. */
    AutoPtr<ArrayOf<Byte> > output = ArrayOf<Byte>::Alloc(maximumLen);
    Int32 bytesWritten;
    // try {
    ECode ec = DoFinalInternal(input,
            inputOffset, inputLen, output, 0, maximumLen, &bytesWritten);
    if (FAILED(ec)) {
        return E_RUNTIME_EXCEPTION;
    }
    // } catch (ShortBufferException e) {
    //     /* This should not happen since we sized our own buffer. */
    //     throw new RuntimeException("our calculated buffer was too small", e);
    // }

    if (bytesWritten == output->GetLength()) {
        *result = output;
        REFCOUNT_ADD(*result)
    }
    else if (bytesWritten == 0) {
        *result = EmptyArray::BYTE;
        REFCOUNT_ADD(*result)
    }
    else {
        return Arrays::CopyOfRange(output, 0, bytesWritten, result);
    }
    return NOERROR;
}

ECode OpenSSLCipher::EngineDoFinal(
    /* [in] */ ArrayOf<Byte>* input,
    /* [in] */ Int32 inputOffset,
    /* [in] */ Int32 inputLen,
    /* [in] */ ArrayOf<Byte>* output,
    /* [in] */ Int32 outputOffset,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    if (output == NULL) {
        // throw new NULLPointerException("output == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }

    Int32 maximumLen = GetOutputSize(inputLen);
    return DoFinalInternal(input, inputOffset,
            inputLen, output, outputOffset, maximumLen, result);
}

ECode OpenSSLCipher::EngineWrap(
    /* [in] */ IKey* key,
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)
    // try {
    AutoPtr<ArrayOf<Byte> > encoded;
    key->GetEncoded((ArrayOf<Byte>**)&encoded);
    EngineDoFinal(encoded, 0, encoded->GetLength(), result);
    // } catch (BadPaddingException e) {
    //     IllegalBlockSizeException newE = new IllegalBlockSizeException();
    //     newE.initCause(e);
    //     throw newE;
    // }
    return NOERROR;
}

ECode OpenSSLCipher::EngineUnwrap(
    /* [in] */ ArrayOf<Byte>* wrappedKey,
    /* [in] */ const String& wrappedKeyAlgorithm,
    /* [in] */ Int32 wrappedKeyType,
    /* [out] */ IKey** result)
{
    VALIDATE_NOT_NULL(result)
    // try {
    AutoPtr<ArrayOf<Byte> > encoded;
    EngineDoFinal(wrappedKey, 0, wrappedKey->GetLength(), (ArrayOf<Byte>**)&encoded);

    AutoPtr<IKeyFactoryHelper> helper;
    CKeyFactoryHelper::AcquireSingleton((IKeyFactoryHelper**)&helper);
    AutoPtr<IKeyFactory> keyFactory;
    helper->GetInstance(wrappedKeyAlgorithm, (IKeyFactory**)&keyFactory);

    if (wrappedKeyType == ICipher::PUBLIC_KEY) {
        AutoPtr<IX509EncodedKeySpec> spec;
        CX509EncodedKeySpec::New(encoded, (IX509EncodedKeySpec**)&spec);
        AutoPtr<IPublicKey> pk;
        keyFactory->GeneratePublic(IKeySpec::Probe(spec), (IPublicKey**)&pk);
        *result = IKey::Probe(pk);
        REFCOUNT_ADD(*result)
    }
    else if (wrappedKeyType == ICipher::PRIVATE_KEY) {
        AutoPtr<IPKCS8EncodedKeySpec> spec;
        CPKCS8EncodedKeySpec::New(encoded, (IPKCS8EncodedKeySpec**)&spec);
        AutoPtr<IPrivateKey> pk;
        keyFactory->GeneratePrivate(IKeySpec::Probe(spec), (IPrivateKey**)&pk);
        *result = IKey::Probe(pk);
        REFCOUNT_ADD(*result)
    }
    else if (wrappedKeyType == ICipher::SECRET_KEY) {
        AutoPtr<ISecretKeySpec> spec;
        CSecretKeySpec::New(encoded, wrappedKeyAlgorithm, (ISecretKeySpec**)&spec);
        *result = IKey::Probe(spec);
        REFCOUNT_ADD(*result)
    }
    else {
        // throw new UnsupportedOperationException(
        //         "wrappedKeyType == " + wrappedKeyType);
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

Int32 OpenSSLCipher::GetOutputSize(
    /* [in] */ Int32 inputLen)
{
    if (mModeBlockSize == 1) {
        return inputLen;
    }
    else {
        Int64 val;
        mCipherCtx->GetContext(&val);
        Int32 buffered;
        NativeCrypto::Get_EVP_CIPHER_CTX_buf_len(val, &buffered);
        if (mPadding == PADDING_NOPADDING) {
            return buffered + inputLen;
        }
        else {
            Int32 totalLen = inputLen + buffered + mModeBlockSize;
            return totalLen - (totalLen % mModeBlockSize);
        }
    }
}

ECode OpenSSLCipher::EngineInitInternal(
    /* [in] */ Int32 opmode,
    /* [in] */ IKey* key,
    /* [in] */ ArrayOf<Byte>* iv,
    /* [in] */ ISecureRandom* random)
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

    if (ISecretKey::Probe(key) == NULL) {
        // throw new InvalidKeyException("Only SecretKey is supported");
        return E_INVALID_KEY_EXCEPTION;
    }

    AutoPtr<ArrayOf<Byte> > encodedKey;
    key->GetEncoded((ArrayOf<Byte>**)&encodedKey);
    if (encodedKey == NULL) {
        // throw new InvalidKeyException("key.getEncoded() == NULL");
        return E_INVALID_KEY_EXCEPTION;
    }
    CheckSupportedKeySize(encodedKey->GetLength());
    mEncodedKey = encodedKey;

    String name;
    GetCipherName(encodedKey->GetLength(), mMode, &name);
    Int64 cipherType;
    NativeCrypto::EVP_get_cipherbyname(name, &cipherType);
    if (cipherType == 0) {
        // throw new InvalidAlgorithmParameterException(
        //         "Cannot find name for key length = "+ (encodedKey.length * 8)
        //         + " and mode = " + mode);
        return E_INVALID_PARAMETER_EXCEPTION;
    }

    Int32 ivLength;
    NativeCrypto::EVP_CIPHER_iv_length(cipherType, &ivLength);
    if (iv == NULL && ivLength != 0) {
        iv = ArrayOf<Byte>::Alloc(ivLength);
        if (mEncrypting) {
            if (random == NULL) {
                CSecureRandom::New((ISecureRandom**)&random);
            }
            IRandom::Probe(random)->NextBytes(iv);
        }
    }
    else if (iv != NULL && iv->GetLength() != ivLength) {
        // throw new InvalidAlgorithmParameterException(
        //         "expected IV length of " + ivLength);.
        return E_INVALID_ALGORITHM_PARAMETER_EXCEPTION;
    }

    mIv = iv;

    Int64 ctx;
    mCipherCtx->GetContext(&ctx);
    Boolean b;
    if (SupportsVariableSizeKey(&b), b) {
        NativeCrypto::EVP_CipherInit_ex(ctx, cipherType, NULL, NULL,
                mEncrypting);
        NativeCrypto::EVP_CIPHER_CTX_set_key_length(ctx, encodedKey->GetLength());
        NativeCrypto::EVP_CipherInit_ex(ctx, 0, encodedKey, iv, mEncrypting);
    }
    else {
        NativeCrypto::EVP_CipherInit_ex(ctx, cipherType, encodedKey, iv, mEncrypting);
    }

    // OpenSSL only supports PKCS5 Padding.
    NativeCrypto::EVP_CIPHER_CTX_set_padding(ctx, mPadding == PADDING_PKCS5PADDING);
    NativeCrypto::EVP_CIPHER_CTX_block_size(ctx, &mModeBlockSize);
    mCalledUpdate = FALSE;
    return NOERROR;
}

ECode OpenSSLCipher::UpdateInternal(
    /* [in] */ ArrayOf<Byte>* input,
    /* [in] */ Int32 inputOffset,
    /* [in] */ Int32 inputLen,
    /* [in] */ ArrayOf<Byte>* output,
    /* [in] */ Int32 outputOffset,
    /* [in] */ Int32 maximumLen,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    Int32 intialOutputOffset = outputOffset;

    Int32 bytesLeft = output->GetLength() - outputOffset;
    if (bytesLeft < maximumLen) {
        // throw new ShortBufferException("output buffer too small during update: "
        //          + bytesLeft + " < " + maximumLen);
        return E_SHORT_BUFFER_EXCEPTION;
    }

    Int64 ctx;
    mCipherCtx->GetContext(&ctx);
    Int32 val;
    NativeCrypto::EVP_CipherUpdate(ctx, output, outputOffset,
                input, inputOffset, inputLen, &val);
    outputOffset += val;

    mCalledUpdate = TRUE;

    *result = outputOffset - intialOutputOffset;
    return NOERROR;
}

void OpenSSLCipher::Reset()
{
    Int64 ctx;
    mCipherCtx->GetContext(&ctx);
    NativeCrypto::EVP_CipherInit_ex(ctx, 0, mEncodedKey, mIv, mEncrypting);
    mCalledUpdate = FALSE;
}

ECode OpenSSLCipher::DoFinalInternal(
    /* [in] */ ArrayOf<Byte>* input,
    /* [in] */ Int32 inputOffset,
    /* [in] */ Int32 inputLen,
    /* [in] */ ArrayOf<Byte>* output,
    /* [in] */ Int32 outputOffset,
    /* [in] */ Int32 maximumLen,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    /* Remember this so we can tell how many characters were written. */
    Int32 initialOutputOffset = outputOffset;

    if (inputLen > 0) {
        Int32 updateBytesWritten;
        FAIL_RETURN(UpdateInternal(input, inputOffset, inputLen, output,
                outputOffset, maximumLen, &updateBytesWritten));
        outputOffset += updateBytesWritten;
        maximumLen -= updateBytesWritten;
    }

    /*
     * If we're decrypting and haven't had any input, we should return NULL.
     * Otherwise OpenSSL will complain if we call final.
     */
    if (!mEncrypting && !mCalledUpdate) {
        *result = 0;
        return NOERROR;
    }

    /* Allow OpenSSL to pad if necessary and clean up state. */
    Int32 bytesLeft = output->GetLength() - outputOffset;
    Int32 writtenBytes;
    Int64 ctx;
    mCipherCtx->GetContext(&ctx);

    if (bytesLeft >= maximumLen) {
        NativeCrypto::EVP_CipherFinal_ex(ctx, output,
                outputOffset, &writtenBytes);
    }
    else {
        AutoPtr<ArrayOf<Byte> > lastBlock = ArrayOf<Byte>::Alloc(maximumLen);
        NativeCrypto::EVP_CipherFinal_ex(ctx, lastBlock, 0, &writtenBytes);
        if (writtenBytes > bytesLeft) {
            // throw new ShortBufferException("buffer is too short: "
            //          + writtenBytes + " > " + bytesLeft);
            return E_SHORT_BUFFER_EXCEPTION;
        }
        else if (writtenBytes > 0) {
            output->Copy(outputOffset, lastBlock, 0, writtenBytes);
        }
    }
    outputOffset += writtenBytes;

    Reset();

    *result = outputOffset - initialOutputOffset;
    return NOERROR;
}

//==============================================================================
//  OpenSSLCipher_AES
//==============================================================================

const Int32 OpenSSLCipher_AES::AES_BLOCK_SIZE;
CAR_INTERFACE_IMPL(OpenSSLCipher_AES, OpenSSLCipher, IOpenSSLCipherAES)
OpenSSLCipher_AES::OpenSSLCipher_AES()
{}

ECode OpenSSLCipher_AES::constructor(
    /* [in] */ OpenSSLCipher::Mode mode,
    /* [in] */ OpenSSLCipher::Padding padding)
{
    return OpenSSLCipher::constructor(mode, padding);
}

ECode OpenSSLCipher_AES::CheckSupportedKeySize(
    /* [in] */ Int32 keyLength)
{
    switch (keyLength) {
        case 16: // AES 128
        case 24: // AES 192
        case 32: // AES 256
            return NOERROR;
        default:
            // throw new InvalidKeyException("Unsupported key size: " + keyLength + " bytes");
            return E_INVALID_KEY_EXCEPTION;
    }
    return NOERROR;
}

ECode OpenSSLCipher_AES::CheckSupportedMode(
    /* [in] */ OpenSSLCipher::Mode mode)
{
    switch (mode) {
        case MODE_CBC:
        case MODE_CFB:
        case MODE_CFB1:
        case MODE_CFB8:
        case MODE_CFB128:
        case MODE_CTR:
        case MODE_ECB:
        case MODE_OFB:
            return NOERROR;
        default:
            // throw new NoSuchAlgorithmException("Unsupported mode " + mode.toString());
            return E_NO_SUCH_ALGORITHM_EXCEPTION;
    }
    return NOERROR;
}

ECode OpenSSLCipher_AES::CheckSupportedPadding(
    /* [in] */ OpenSSLCipher::Padding padding)
{
    switch (padding) {
        case PADDING_NOPADDING:
        case PADDING_PKCS5PADDING:
            return NOERROR;
        default:
            // throw new NoSuchPaddingException("Unsupported padding " + padding.toString());
            return E_NO_SUCH_PADDING_EXCEPTION;
    }
    return NOERROR;
}

ECode OpenSSLCipher_AES::GetBaseCipherName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = String("AES");
    return NOERROR;
}

ECode OpenSSLCipher_AES::GetCipherName(
    /* [in] */ Int32 keyLength,
    /* [in] */ OpenSSLCipher::Mode mode,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = String("aes-") + StringUtils::ToString(keyLength * 8)
            + String("-") + ModeToString(mode).ToLowerCase(/*Locale.US*/);
    return NOERROR;
}

ECode OpenSSLCipher_AES::GetCipherBlockSize(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = AES_BLOCK_SIZE;
    return NOERROR;
}

//==============================================================================
//  OpenSSLCipher_AES_CBC
//==============================================================================

OpenSSLCipher_AES_CBC::OpenSSLCipher_AES_CBC()
{
}

ECode OpenSSLCipher_AES_CBC::constructor(
    /* [in] */ Padding padding)
{
    return OpenSSLCipher_AES::constructor(MODE_CBC, padding);
}

//==============================================================================
//  OpenSSLCipher_AES_CBC_NoPadding
//==============================================================================

OpenSSLCipher_AES_CBC_NoPadding::OpenSSLCipher_AES_CBC_NoPadding()
{
}

ECode OpenSSLCipher_AES_CBC_NoPadding::constructor()
{
    return OpenSSLCipher_AES_CBC::constructor(PADDING_NOPADDING);
}

//==============================================================================
//  OpenSSLCipher_AES_CBC_PKCS5Padding
//==============================================================================

OpenSSLCipher_AES_CBC_PKCS5Padding::OpenSSLCipher_AES_CBC_PKCS5Padding()
{
}

ECode OpenSSLCipher_AES_CBC_PKCS5Padding::constructor()
{
    return OpenSSLCipher_AES_CBC::constructor(PADDING_PKCS5PADDING);
}

//==============================================================================
//  OpenSSLCipher_AES_CFB
//==============================================================================

OpenSSLCipher_AES_CFB::OpenSSLCipher_AES_CFB()
{
}

ECode OpenSSLCipher_AES_CFB::constructor()
{
    return OpenSSLCipher_AES::constructor(MODE_CFB, PADDING_NOPADDING);
}

//==============================================================================
//  OpenSSLCipher_AES_CTR
//==============================================================================

OpenSSLCipher_AES_CTR::OpenSSLCipher_AES_CTR()
{
}

ECode OpenSSLCipher_AES_CTR::constructor()
{
    return OpenSSLCipher_AES::constructor(MODE_CTR, PADDING_NOPADDING);
}

//==============================================================================
//  OpenSSLCipher_AES_ECB
//==============================================================================

OpenSSLCipher_AES_ECB::OpenSSLCipher_AES_ECB()
{
}

ECode OpenSSLCipher_AES_ECB::constructor(
    /* [in] */ Padding padding)
{
    return OpenSSLCipher_AES::constructor(MODE_ECB, padding);
}

//==============================================================================
//  OpenSSLCipher_AES_ECB_NoPadding
//==============================================================================
CAR_INTERFACE_IMPL(OpenSSLCipher_AES_ECB_NoPadding, OpenSSLCipher_AES_ECB, IOpenSSLCipherAESECBNoPadding);
OpenSSLCipher_AES_ECB_NoPadding::OpenSSLCipher_AES_ECB_NoPadding()
{
}

ECode OpenSSLCipher_AES_ECB_NoPadding::constructor()
{
    return OpenSSLCipher_AES_ECB::constructor(PADDING_NOPADDING);
}

//==============================================================================
//  OpenSSLCipher_AES_ECB_PKCS5Padding
//==============================================================================

OpenSSLCipher_AES_ECB_PKCS5Padding::OpenSSLCipher_AES_ECB_PKCS5Padding()
{
}

ECode OpenSSLCipher_AES_ECB_PKCS5Padding::constructor()
{
    return OpenSSLCipher_AES_ECB::constructor(PADDING_PKCS5PADDING);
}

//==============================================================================
//  OpenSSLCipher_AES_OFB
//==============================================================================

OpenSSLCipher_AES_OFB::OpenSSLCipher_AES_OFB()
{
}

ECode OpenSSLCipher_AES_OFB::constructor()
{
    return OpenSSLCipher_AES::constructor(MODE_OFB, PADDING_NOPADDING);
}

//==============================================================================
//  OpenSSLCipher_DESEDE
//==============================================================================

const Int32 OpenSSLCipher_DESEDE::DES_BLOCK_SIZE;

OpenSSLCipher_DESEDE::OpenSSLCipher_DESEDE(
    /* [in] */ OpenSSLCipher::Mode mode,
    /* [in] */ OpenSSLCipher::Padding padding)
{
    OpenSSLCipher::constructor(mode, padding);
}

ECode OpenSSLCipher_DESEDE::GetBaseCipherName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = String("DESede");
    return NOERROR;
}

ECode OpenSSLCipher_DESEDE::GetCipherName(
    /* [in] */ Int32 keySize,
    /* [in] */ OpenSSLCipher::Mode mode,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    String baseCipherName;
    if (keySize == 16) {
        baseCipherName = String("des-ede");
    }
    else {
        baseCipherName = String("des-ede3");
    }

    if (mode == MODE_ECB) {
        *result = baseCipherName;
        return NOERROR;
    }
    else {
        *result = baseCipherName + String("-") + ModeToString(mode).ToLowerCase(/*Locale.US*/);
        return NOERROR;
    }
    return NOERROR;
}

ECode OpenSSLCipher_DESEDE::CheckSupportedKeySize(
    /* [in] */ Int32 keySize)
{
    if (keySize != 16 && keySize != 24) {
        // throw new InvalidKeyException("key size must be 128 or 192 bits");
        return E_INVALID_KEY_EXCEPTION;
    }
    return NOERROR;
}

ECode OpenSSLCipher_DESEDE::CheckSupportedMode(
    /* [in] */ OpenSSLCipher::Mode mode)
{
    switch (mode) {
        case MODE_CBC:
        case MODE_CFB:
        case MODE_CFB1:
        case MODE_CFB8:
        case MODE_ECB:
        case MODE_OFB:
            return NOERROR;
        default:
            // throw new NoSuchAlgorithmException("Unsupported mode " + mode.toString());
            return E_NO_SUCH_ALGORITHM_EXCEPTION;
    }
    return NOERROR;
}

ECode OpenSSLCipher_DESEDE::CheckSupportedPadding(
    /* [in] */ OpenSSLCipher::Padding padding)
{
    switch (padding) {
        case PADDING_NOPADDING:
        case PADDING_PKCS5PADDING:
            return NOERROR;
        default:
            // throw new NoSuchPaddingException("Unsupported padding " + padding.toString());
            return E_NO_SUCH_PADDING_EXCEPTION;
    }
    return NOERROR;
}

ECode OpenSSLCipher_DESEDE::GetCipherBlockSize(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = DES_BLOCK_SIZE;
    return NOERROR;
}

//==============================================================================
//  OpenSSLCipher_DESEDE_CBC
//==============================================================================

OpenSSLCipher_DESEDE_CBC::OpenSSLCipher_DESEDE_CBC(
    /* [in] */ OpenSSLCipher::Padding padding)
    : OpenSSLCipher_DESEDE(MODE_CBC, padding)
{
}

ECode OpenSSLCipher_DESEDE_CBC::constructor()
{
    return NOERROR;
}
//==============================================================================
//  OpenSSLCipher_DESEDE_CBC_NoPadding
//==============================================================================

OpenSSLCipher_DESEDE_CBC_NoPadding::OpenSSLCipher_DESEDE_CBC_NoPadding()
    : OpenSSLCipher_DESEDE_CBC(PADDING_NOPADDING)
{
}

//==============================================================================
//  OpenSSLCipher_DESEDE_CBC_PKCS5Padding
//==============================================================================

OpenSSLCipher_DESEDE_CBC_PKCS5Padding::OpenSSLCipher_DESEDE_CBC_PKCS5Padding()
    : OpenSSLCipher_DESEDE_CBC(PADDING_PKCS5PADDING)
{
}

//==============================================================================
//  OpenSSLCipher_DESEDE_CFB
//==============================================================================

OpenSSLCipher_DESEDE_CFB::OpenSSLCipher_DESEDE_CFB()
    : OpenSSLCipher_DESEDE(MODE_CFB, PADDING_NOPADDING)
{
}

ECode OpenSSLCipher_DESEDE_CFB::constructor()
{
    return NOERROR;
}

//==============================================================================
//  OpenSSLCipher_DESEDE_ECB
//==============================================================================

OpenSSLCipher_DESEDE_ECB::OpenSSLCipher_DESEDE_ECB(
    /* [in] */ OpenSSLCipher::Padding padding)
    : OpenSSLCipher_DESEDE(MODE_ECB, padding)
{
}

ECode OpenSSLCipher_DESEDE_ECB::constructor()
{
    return NOERROR;
}

//==============================================================================
//  OpenSSLCipher_DESEDE_ECB_NoPadding
//==============================================================================

OpenSSLCipher_DESEDE_ECB_NoPadding::OpenSSLCipher_DESEDE_ECB_NoPadding()
    : OpenSSLCipher_DESEDE_ECB(PADDING_NOPADDING)
{
}

//==============================================================================
//  OpenSSLCipher_DESEDE_ECB_PKCS5Padding
//==============================================================================

OpenSSLCipher_DESEDE_ECB_PKCS5Padding::OpenSSLCipher_DESEDE_ECB_PKCS5Padding()
    : OpenSSLCipher_DESEDE_ECB(PADDING_PKCS5PADDING)
{
}

//==============================================================================
//  OpenSSLCipher_DESEDE_OFB
//==============================================================================

OpenSSLCipher_DESEDE_OFB::OpenSSLCipher_DESEDE_OFB()
    : OpenSSLCipher_DESEDE(MODE_OFB, PADDING_NOPADDING)
{
}

ECode OpenSSLCipher_DESEDE_OFB::constructor()
{
    return NOERROR;
}

//==============================================================================
//  OpenSSLCipher_ARC4
//==============================================================================

OpenSSLCipher_ARC4::OpenSSLCipher_ARC4()
{
}

ECode OpenSSLCipher_ARC4::constructor()
{
    return NOERROR;
}

ECode OpenSSLCipher_ARC4::GetBaseCipherName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = String("ARCFOUR");
    return NOERROR;
}

ECode OpenSSLCipher_ARC4::GetCipherName(
    /* [in] */ Int32 keySize,
    /* [in] */ OpenSSLCipher::Mode mode,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = String("rc4");
    return NOERROR;
}

ECode OpenSSLCipher_ARC4::CheckSupportedKeySize(
    /* [in] */ Int32 keySize)
{
    return NOERROR;
}

ECode OpenSSLCipher_ARC4::CheckSupportedMode(
    /* [in] */ OpenSSLCipher::Mode mode)
{
    // throw new NoSuchAlgorithmException("ARC4 does not support modes");
    return E_NO_SUCH_ALGORITHM_EXCEPTION;
}

ECode OpenSSLCipher_ARC4::CheckSupportedPadding(
    /* [in] */ OpenSSLCipher::Padding padding)
{
    // throw new NoSuchPaddingException("ARC4 does not support padding");
    return E_NO_SUCH_PADDING_EXCEPTION;
}

ECode OpenSSLCipher_ARC4::GetCipherBlockSize(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;
    return NOERROR;
}

ECode OpenSSLCipher_ARC4::SupportsVariableSizeKey(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TRUE;
    return NOERROR;
}


} // namespace Conscrypt
} // namespace Org
