
#ifndef __ORG_CONSCRYPT_OPENSSLSIGNATURE_H__
#define __ORG_CONSCRYPT_OPENSSLSIGNATURE_H__

#include "Elastos.CoreLibrary.Security.h"
#include "_Org.Conscrypt.h"
// #include "elastos/security/SignatureSpi.h"
#include <elastos/core/Object.h>

// using Elastos::Security::SignatureSpi;
using Elastos::Security::IPrivateKey;
using Elastos::Security::IPublicKey;

namespace Org {
namespace Conscrypt {

/**
 * Implements the subset of the JDK Signature interface needed for
 * signature verification using OpenSSL.
 */
class OpenSSLSignature
    : public Object
    , public IOpenSSLSignature
//    : public SignatureSpi
{
public:
    CAR_INTERFACE_DECL()

    CARAPI EngineUpdate(
        /* [in] */ Byte input);

    CARAPI EngineUpdate(
        /* [in] */ ArrayOf<Byte>* input,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 len);

    CARAPI EngineGetParameter(
        /* [in] */ String param,
        /* [out] */ IInterface** result);

    CARAPI EngineInitSign(
        /* [in] */ IPrivateKey* privateKey);

    CARAPI EngineInitVerify(
        /* [in] */ IPublicKey* publicKey);

    CARAPI EngineSetParameter(
        /* [in] */ String param,
        /* [in] */ IInterface* value);

    CARAPI EngineSign(
        /* [out] */ ArrayOf<Byte>** result);

    CARAPI EngineVerify(
        /* [in] */ ArrayOf<Byte>* sigBytes,
        /* [out] */ Boolean* result);

    /**
     * Creates a new OpenSSLSignature instance for the given algorithm name.
     *
     * @param algorithm OpenSSL name of the algorithm, e.g. "RSA-SHA1".
     */
    CARAPI constructor(
        /* [in] */ Int64 algorithm,
        /* [in] */ EngineType engineType);

private:
    void ResetContext();

    void CheckEngineType(
        /* [in] */ IOpenSSLKey* pkey);

    void InitInternal(
        /* [in] */ IOpenSSLKey* newKey,
        /* [in] */ Boolean signing);

    /**
     * Enables a mitigation against private key leakage through DSA and ECDSA signatures when weak
     * nonces (per-message k values) are used. To mitigate the issue, private key and message being
     * signed is mixed into the randomly generated nonce (k).
     *
     * <p>Does nothing for signatures that are neither DSA nor ECDSA.
     */
    void EnableDSASignatureNonceHardeningIfApplicable();

private:
    AutoPtr<IOpenSSLDigestContext> mCtx;

    /**
     * The current OpenSSL key we're operating on.
     */
    AutoPtr<IOpenSSLKey> mKey;

    /**
     * Holds the type of the Java algorithm.
     */
    EngineType mEngineType;

    /**
     * Holds the OpenSSL name of the algorithm (lower case, no dashes).
     */
    Int64 mEvpAlgorithm;

    /**
     * Holds a dummy buffer for writing single bytes to the digest.
     */
    AutoPtr<ArrayOf<Byte> > mSingleByte;

    /**
     * True when engine is initialized to signing.
     */
    Boolean mSigning;
};

class MD5RSA
    : public OpenSSLSignature
{
public:
    MD5RSA();

private:
    static Int64 EVP_MD;
};

class SHA1RSA
    : public OpenSSLSignature
{
public:
    SHA1RSA();

private:
    static Int64 EVP_MD;
};

class SHA224RSA
    : public OpenSSLSignature
{
public:
    SHA224RSA();

private:
    static Int64 EVP_MD;
};

class SHA256RSA
    : public OpenSSLSignature
{
public:
    SHA256RSA();

private:
    static Int64 EVP_MD;
};

class SHA384RSA
    : public OpenSSLSignature
{
public:
    SHA384RSA();

private:
    static Int64 EVP_MD;
};

class SHA512RSA
    : public OpenSSLSignature
{
public:
    SHA512RSA();

private:
    static Int64 EVP_MD;
};

class SHA1DSA
    : public OpenSSLSignature
{
public:
    SHA1DSA();

private:
    static Int64 EVP_MD;
};

class SHA1ECDSA
    : public OpenSSLSignature
{
public:
    SHA1ECDSA();

private:
    static Int64 EVP_MD;
};

class SHA224ECDSA
    : public OpenSSLSignature
{
public:
    SHA224ECDSA();

private:
    static Int64 EVP_MD;
};

class SHA256ECDSA
    : public OpenSSLSignature
{
public:
    SHA256ECDSA();

private:
    static Int64 EVP_MD;
};

class SHA384ECDSA
    : public OpenSSLSignature
{
public:
    SHA384ECDSA();

private:
    static Int64 EVP_MD;
};

class SHA512ECDSA
    : public OpenSSLSignature
{
public:
    SHA512ECDSA();

private:
    static Int64 EVP_MD;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_OPENSSLSIGNATURE_H__