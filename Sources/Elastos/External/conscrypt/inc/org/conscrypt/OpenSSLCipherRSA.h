
#ifndef __ORG_CONSCRYPT_OPENSSLCIPHERRSA_H__
#define __ORG_CONSCRYPT_OPENSSLCIPHERRSA_H__

#include <elastos/coredef.h>
#include <Elastos.CoreLibrary.Extensions.h>
#include <Elastos.CoreLibrary.Security.h>
#include <elastosx/crypto/CipherSpi.h>
#include "_Org.Conscrypt.h"

using Elastos::Security::IKey;
using Elastos::Security::ISecureRandom;
using Elastos::Security::IAlgorithmParameters;
using Elastos::Security::Spec::IAlgorithmParameterSpec;
using Elastosx::Crypto::CipherSpi;

namespace Org {
namespace Conscrypt {

/**
 * Provides an interface to OpenSSL's BIO system directly from a Java
 * InputStream. It allows an OpenSSL API to read directly from something more
 * flexible interface than a byte array.
 */
class OpenSSLCipherRSA
    : public IOpenSSLCipherRSA
    , public CipherSpi
{
public:
    CAR_INTERFACE_DECL()

    OpenSSLCipherRSA();

    CARAPI constructor(
        /* [in] */ Int32 padding);

protected:
    // @Override
    virtual CARAPI EngineSetMode(
        /* [in] */ const String& mode);

    // @Override
    virtual CARAPI EngineSetPadding(
        /* [in] */ const String& padding);

    // @Override
    virtual CARAPI EngineGetBlockSize(
        /* [out] */ Int32* size);

    // @Override
    virtual CARAPI EngineGetOutputSize(
        /* [in] */ Int32 inputLen,
        /* [out] */ Int32* size);

    // @Override
    virtual CARAPI EngineGetIV(
        /* [out, callee] */ ArrayOf<Byte>** iv);

    // @Override
    virtual CARAPI EngineGetParameters(
        /* [out] */ IAlgorithmParameters** param);

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
        /* [out, callee] */ ArrayOf<Byte>** output);

    // @Override
    virtual CARAPI EngineUpdate(
        /* [in] */ ArrayOf<Byte>* input,
        /* [in] */ Int32 inputOffset,
        /* [in] */ Int32 inputLen,
        /* [in] */ ArrayOf<Byte>* output,
        /* [in] */ Int32 outputOffset,
        /* [out] */ Int32* number);

    // @Override
    virtual CARAPI EngineDoFinal(
        /* [in] */ ArrayOf<Byte>* input,
        /* [in] */ Int32 inputOffset,
        /* [in] */ Int32 inputLen,
        /* [out, callee] */ ArrayOf<Byte>** bytes);

    // @Override
    virtual CARAPI EngineDoFinal(
        /* [in] */ ArrayOf<Byte>* input,
        /* [in] */ Int32 inputOffset,
        /* [in] */ Int32 inputLen,
        /* [in] */ ArrayOf<Byte>* output,
        /* [in] */ Int32 outputOffset,
        /* [out] */ Int32* number);

    // @Override
    virtual CARAPI EngineWrap(
        /* [in] */ IKey* keyToWrap,
        /* [out, callee] */ ArrayOf<Byte>** wrappedKey);

    // @Override
    virtual CARAPI EngineUnwrap(
        /* [in] */ ArrayOf<Byte>* wrappedKey,
        /* [in] */ const String& wrappedKeyAlgorithm,
        /* [in] */ Int32 wrappedKeyType,
        /* [out] */ IKey** key);

private:
    CARAPI_(Int32) PaddedBlockSizeBytes();

    CARAPI KeySizeBytes(
        /* [out] */ Int32* result);

    CARAPI EngineInitInternal(
        /* [in] */ Int32 opmode,
        /* [in] */ IKey* key);

private:
    /**
     * The current OpenSSL key we're operating on.
     */
    AutoPtr<IOpenSSLKey> mKey;

    /**
     * Current key type: private or public.
     */
    Boolean mUsingPrivateKey;

    /**
     * Current cipher mode: encrypting or decrypting.
     */
    Boolean mEncrypting;

    /**
     * Buffer for operations
     */
    AutoPtr<ArrayOf<Byte> > mBuffer;

    /**
     * Current offset in the buffer.
     */
    Int32 mBufferOffset;

    /**
     * Flag that indicates an exception should be thrown when the input is too
     * large during doFinal.
     */
    Boolean mInputTooLarge;

    /**
     * Current padding mode
     */
    Int32 mPadding; // = NativeCrypto::RSA_PKCS1_PADDING;
};

class OpenSSLCipherRSA_PKCS1
    : public OpenSSLCipherRSA
{
public:
    OpenSSLCipherRSA_PKCS1();
};

class OpenSSLCipherRSA_Raw
    : public OpenSSLCipherRSA
{
public:
    OpenSSLCipherRSA_Raw();
};


} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_OPENSSLCIPHERRSA_H__
