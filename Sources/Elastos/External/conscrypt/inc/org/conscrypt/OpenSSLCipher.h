
#ifndef __ORG_CONSCRYPT_OPENSSLCIPHER_H__
#define __ORG_CONSCRYPT_OPENSSLCIPHER_H__

#include <elastos/coredef.h>
#include <Elastos.CoreLibrary.Extensions.h>
#include <Elastos.CoreLibrary.Security.h>
#include <elastosx/crypto/CipherSpi.h>
#include "_Org.Conscrypt.h"
#include "org/conscrypt/OpenSSLCipherContext.h"

using Elastos::Security::IAlgorithmParameters;
using Elastos::Security::IKey;
using Elastos::Security::ISecureRandom;
using Elastos::Security::Spec::IAlgorithmParameterSpec;
using Elastosx::Crypto::CipherSpi;

namespace Org {
namespace Conscrypt {

class OpenSSLCipher
    : public IOpenSSLCipher
    , public CipherSpi
{
public:
    /**
     * Modes that a block cipher may support.
     */
    enum Mode
    {
        MODE_CBC,
        MODE_CFB, MODE_CFB1, MODE_CFB8, MODE_CFB128,
        MODE_CTR,
        MODE_CTS,
        MODE_ECB,
        MODE_OFB, MODE_OFB64, MODE_OFB128,
        MODE_PCBC,
    };

    static Mode ModeValueOf(
        /* [in] */ const String& str);

    static String ModeToString(
        /* [in] */ Mode mode);

    /**
     * Paddings that a block cipher may support.
     */
    enum Padding
    {
        PADDING_NOPADDING,
        PADDING_PKCS5PADDING,
        PADDING_ISO10126PADDING,
    };

    static Padding PaddingValueOf(
        /* [in] */ const String& str);

    static String PaddingToString(
        /* [in] */ Padding padding);

public:
    CAR_INTERFACE_DECL()

    OpenSSLCipher();

    CARAPI constructor(
        /* [in] */ Mode mode,
        /* [in] */ Padding padding);

    /**
     * Returns the standard name for the particular algorithm.
     */
    virtual CARAPI GetBaseCipherName(
        /* [out] */ String* result) = 0;

    /**
     * Returns the OpenSSL cipher name for the particular {@code keySize} and
     * cipher {@code mode}.
     */
    virtual CARAPI GetCipherName(
        /* [in] */ Int32 keySize,
        /* [in] */ Mode mode,
        /* [out] */ String* result) = 0;

    /**
     * Checks whether the cipher supports this particular {@code keySize} (in
     * bytes) and throws {@code InvalidKeyException} if it doesn't.
     */
    virtual CARAPI CheckSupportedKeySize(
        /* [in] */ Int32 keySize) = 0;

    /**
     * Checks whether the cipher supports this particular cipher {@code mode}
     * and throws {@code NoSuchAlgorithmException} if it doesn't.
     */
    virtual CARAPI CheckSupportedMode(
        /* [in] */ Mode mode) = 0;

    /**
     * Checks whether the cipher supports this particular cipher {@code padding}
     * and throws {@code NoSuchPaddingException} if it doesn't.
     */
    virtual CARAPI CheckSupportedPadding(
        /* [in] */ Padding padding) = 0;

    virtual CARAPI GetCipherBlockSize(
        /* [out] */ Int32* result) = 0;

    virtual CARAPI SupportsVariableSizeKey(
        /* [out] */ Boolean* result);

    // @Override
    virtual CARAPI EngineSetMode(
        /* [in] */ const String& mode);

    // @Override
    virtual CARAPI EngineSetPadding(
        /* [in] */ const String& padding);

    // @Override
    virtual CARAPI EngineGetBlockSize(
        /* [out] */ Int32* result);

    // @Override
    virtual CARAPI EngineGetOutputSize(
        /* [in] */ Int32 inputLen,
        /* [out] */ Int32* result);

    // @Override
    virtual CARAPI EngineGetIV(
        /* [out, callee] */ ArrayOf<Byte>** result);

    // @Override
    virtual CARAPI EngineGetParameters(
        /* [out] */ IAlgorithmParameters** result);

    // @Override
    virtual CARAPI EngineInit(
        /* [in] */ Int32 opmode,
        /* [in] */ IKey* key,
        /* [in] */ ISecureRandom* random);

    // @Override
    virtual CARAPI EngineInit(
        /* [in] */ Int32 opmode,
        /* [in] */ IKey* key,
        /* [in] */ IAlgorithmParameterSpec* params,
        /* [in] */ ISecureRandom* random);

    // @Override
    virtual CARAPI EngineInit(
        /* [in] */ Int32 opmode,
        /* [in] */ IKey* key,
        /* [in] */ IAlgorithmParameters* params,
        /* [in] */ ISecureRandom* random);

    // @Override
    virtual CARAPI EngineUpdate(
        /* [in] */ ArrayOf<Byte>* input,
        /* [in] */ Int32 inputOffset,
        /* [in] */ Int32 inputLen,
        /* [out, callee] */ ArrayOf<Byte>** result);

    // @Override
    virtual CARAPI EngineUpdate(
        /* [in] */ ArrayOf<Byte>* input,
        /* [in] */ Int32 inputOffset,
        /* [in] */ Int32 inputLen,
        /* [in] */ ArrayOf<Byte>* output,
        /* [in] */ Int32 outputOffset,
        /* [out] */ Int32* result);

    // @Override
    virtual CARAPI EngineDoFinal(
        /* [in] */ ArrayOf<Byte>* input,
        /* [in] */ Int32 inputOffset,
        /* [in] */ Int32 inputLen,
        /* [out, callee] */ ArrayOf<Byte>** result);

    // @Override
    virtual CARAPI EngineDoFinal(
        /* [in] */ ArrayOf<Byte>* input,
        /* [in] */ Int32 inputOffset,
        /* [in] */ Int32 inputLen,
        /* [in] */ ArrayOf<Byte>* output,
        /* [in] */ Int32 outputOffset,
        /* [out] */ Int32* result);

    // @Override
    virtual CARAPI EngineWrap(
        /* [in] */ IKey* keyToWrap,
        /* [out, callee] */ ArrayOf<Byte>** result);

    // @Override
    virtual CARAPI EngineUnwrap(
        /* [in] */ ArrayOf<Byte>* wrappedKey,
        /* [in] */ const String& wrappedKeyAlgorithm,
        /* [in] */ Int32 wrappedKeyType,
        /* [out] */ IKey** result);

private:
    /**
     * The size of output if {@code doFinal()} is called with this
     * {@code inputLen}. If padding is enabled and the size of the input puts it
     * right at the block size, it will add another block for the padding.
     */
    CARAPI_(Int32) GetOutputSize(
        /* [in] */ Int32 inputLen);

    CARAPI EngineInitInternal(
        /* [in] */ Int32 opmode,
        /* [in] */ IKey* key,
        /* [in] */ ArrayOf<Byte>* iv,
        /* [in] */ ISecureRandom* random);

    CARAPI UpdateInternal(
        /* [in] */ ArrayOf<Byte>* input,
        /* [in] */ Int32 inputOffset,
        /* [in] */ Int32 inputLen,
        /* [in] */ ArrayOf<Byte>* output,
        /* [in] */ Int32 outputOffset,
        /* [in] */ Int32 maximumLen,
        /* [out] */ Int32* result);

    /**
     * Reset this Cipher instance state to process a new chunk of data.
     */
    CARAPI_(void) Reset();

    CARAPI DoFinalInternal(
        /* [in] */ ArrayOf<Byte>* input,
        /* [in] */ Int32 inputOffset,
        /* [in] */ Int32 inputLen,
        /* [in] */ ArrayOf<Byte>* output,
        /* [in] */ Int32 outputOffset,
        /* [in] */ Int32 maximumLen,
        /* [out] */ Int32* result);

private:
    /**
     * Native pointer for the OpenSSL EVP_CIPHER context.
     */
    AutoPtr<OpenSSLCipherContext> mCipherCtx;

    /**
     * The current cipher mode.
     */
    Mode mMode; // = Mode.ECB;

    /**
     * The current cipher padding.
     */
    Padding mPadding; // = Padding.PKCS5PADDING;

    /**
     * May be used when reseting the cipher instance after calling
     * {@code doFinal}.
     */
    AutoPtr<ArrayOf<Byte> > mEncodedKey;

    /**
     * The Initial Vector (IV) used for the current cipher.
     */
    AutoPtr<ArrayOf<Byte> > mIv;

    /**
     * Current cipher mode: encrypting or decrypting.
     */
    Boolean mEncrypting;

    /**
     * The block size of the current cipher.
     */
    Int32 mBlockSize;

    /**
     * The block size of the current mode.
     */
    Int32 mModeBlockSize;

    /**
     * Whether the cipher has processed any data yet. OpenSSL doesn't like
     * calling "doFinal()" in decryption mode without processing any updates.
     */
    Boolean mCalledUpdate;
};

class OpenSSLCipher_AES
    : public OpenSSLCipher
    , public IOpenSSLCipherAES
{
public:
    CAR_INTERFACE_DECL();

    CARAPI constructor(
        /* [in] */ Mode mode,
        /* [in] */ Padding padding);

    OpenSSLCipher_AES();

    // @Override
    virtual CARAPI CheckSupportedKeySize(
        /* [in] */ Int32 keySize);

    // @Override
    virtual CARAPI CheckSupportedMode(
        /* [in] */ OpenSSLCipher::Mode mode);

    // @Override
    virtual CARAPI CheckSupportedPadding(
        /* [in] */ OpenSSLCipher::Padding padding);

    // @Override
    virtual CARAPI GetBaseCipherName(
        /* [out] */ String* result);

    // @Override
    virtual CARAPI GetCipherName(
        /* [in] */ Int32 keySize,
        /* [in] */ OpenSSLCipher::Mode mode,
        /* [out] */ String* result);

    // @Override
    virtual CARAPI GetCipherBlockSize(
        /* [out] */ Int32* result);

private:
    static const Int32 AES_BLOCK_SIZE = 16;
};

class OpenSSLCipher_AES_CBC
    : public OpenSSLCipher_AES
{
public:
    OpenSSLCipher_AES_CBC();

    CARAPI constructor(
        /* [in] */ Padding padding);
};

class OpenSSLCipher_AES_CBC_NoPadding
    : public OpenSSLCipher_AES_CBC
{
public:
    OpenSSLCipher_AES_CBC_NoPadding();

    CARAPI constructor();
};

class OpenSSLCipher_AES_CBC_PKCS5Padding
    : public OpenSSLCipher_AES_CBC
{
public:
    OpenSSLCipher_AES_CBC_PKCS5Padding();

    CARAPI constructor();
};

class OpenSSLCipher_AES_CFB
    : public OpenSSLCipher_AES
{
public:
    OpenSSLCipher_AES_CFB();

    CARAPI constructor();
};

class OpenSSLCipher_AES_CTR
    : public OpenSSLCipher_AES
{
public:
    OpenSSLCipher_AES_CTR();

    CARAPI constructor();
};

class OpenSSLCipher_AES_ECB
    : public OpenSSLCipher_AES
{
public:
    OpenSSLCipher_AES_ECB();

    CARAPI constructor(
        /* [in] */ Padding padding);
};

class OpenSSLCipher_AES_ECB_NoPadding
    : public OpenSSLCipher_AES_ECB
    , public IOpenSSLCipherAESECBNoPadding
{
public:
    CAR_INTERFACE_DECL();

    OpenSSLCipher_AES_ECB_NoPadding();

    CARAPI constructor();
};

class OpenSSLCipher_AES_ECB_PKCS5Padding
    : public OpenSSLCipher_AES_ECB
{
public:
    OpenSSLCipher_AES_ECB_PKCS5Padding();

    CARAPI constructor();
};

class OpenSSLCipher_AES_OFB
    : public OpenSSLCipher_AES
{
public:
    OpenSSLCipher_AES_OFB();

    CARAPI constructor();
};

class OpenSSLCipher_DESEDE
    : public OpenSSLCipher
{
public:
    OpenSSLCipher_DESEDE(
        /* [in] */ OpenSSLCipher::Mode mode,
        /* [in] */ OpenSSLCipher::Padding padding);

protected:
    // @Override
    virtual CARAPI GetBaseCipherName(
        /* [out] */ String* result);

    // @Override
    virtual CARAPI GetCipherName(
        /* [in] */ Int32 keySize,
        /* [in] */ OpenSSLCipher::Mode mode,
        /* [out] */ String* result);

    // @Override
    virtual CARAPI CheckSupportedKeySize(
        /* [in] */ Int32 keySize);

    // @Override
    virtual CARAPI CheckSupportedMode(
        /* [in] */ OpenSSLCipher::Mode mode);

    // @Override
    virtual CARAPI CheckSupportedPadding(
        /* [in] */ OpenSSLCipher::Padding padding);

    // @Override
    virtual CARAPI GetCipherBlockSize(
        /* [out] */ Int32* result);

private:
    static const Int32 DES_BLOCK_SIZE = 8;
};

class OpenSSLCipher_DESEDE_CBC
    : public OpenSSLCipher_DESEDE
{
public:
    OpenSSLCipher_DESEDE_CBC(
        /* [in] */ OpenSSLCipher::Padding padding);

    CARAPI constructor();
};

class OpenSSLCipher_DESEDE_CBC_NoPadding
    : public OpenSSLCipher_DESEDE_CBC
{
public:
    OpenSSLCipher_DESEDE_CBC_NoPadding();
};

class OpenSSLCipher_DESEDE_CBC_PKCS5Padding
    : public OpenSSLCipher_DESEDE_CBC
{
public:
    OpenSSLCipher_DESEDE_CBC_PKCS5Padding();
};

class OpenSSLCipher_DESEDE_CFB
    : public OpenSSLCipher_DESEDE
{
public:
    OpenSSLCipher_DESEDE_CFB();

    CARAPI constructor();
};

class OpenSSLCipher_DESEDE_ECB
    : public OpenSSLCipher_DESEDE
{
public:
    OpenSSLCipher_DESEDE_ECB(
        /* [in] */ OpenSSLCipher::Padding padding);

    CARAPI constructor();
};

class OpenSSLCipher_DESEDE_ECB_NoPadding
    : public OpenSSLCipher_DESEDE_ECB
{
public:
    OpenSSLCipher_DESEDE_ECB_NoPadding();
};

class OpenSSLCipher_DESEDE_ECB_PKCS5Padding
    : public OpenSSLCipher_DESEDE_ECB
{
public:
    OpenSSLCipher_DESEDE_ECB_PKCS5Padding();
};

class OpenSSLCipher_DESEDE_OFB
    : public OpenSSLCipher_DESEDE
{
public:
    OpenSSLCipher_DESEDE_OFB();

    CARAPI constructor();
};

class OpenSSLCipher_ARC4
    : public OpenSSLCipher
{
public:
    OpenSSLCipher_ARC4();

    CARAPI constructor();

protected:
    // @Override
    virtual CARAPI GetBaseCipherName(
        /* [out] */ String* result);

    // @Override
    virtual CARAPI GetCipherName(
        /* [in] */ Int32 keySize,
        /* [in] */ OpenSSLCipher::Mode mode,
        /* [out] */ String* result);

    // @Override
    virtual CARAPI CheckSupportedKeySize(
        /* [in] */ Int32 keySize);

    // @Override
    virtual CARAPI CheckSupportedMode(
        /* [in] */ OpenSSLCipher::Mode mode);

    // @Override
    virtual CARAPI CheckSupportedPadding(
        /* [in] */ OpenSSLCipher::Padding padding);

    // @Override
    virtual CARAPI GetCipherBlockSize(
        /* [out] */ Int32* result);

    // @Override
    virtual CARAPI SupportsVariableSizeKey(
        /* [out] */ Boolean* result);
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_OPENSSLCIPHER_H__
