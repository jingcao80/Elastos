//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ORG_CONSCRYPT_OPENSSLSIGNATURE_H__
#define __ORG_CONSCRYPT_OPENSSLSIGNATURE_H__

#include "Elastos.CoreLibrary.Security.h"
#include "_Org.Conscrypt.h"
#include "elastos/security/SignatureSpi.h"
#include <elastos/core/Object.h>

using Elastos::Security::SignatureSpi;
using Elastos::Security::IPrivateKey;
using Elastos::Security::IPublicKey;

namespace Org {
namespace Conscrypt {

/**
 * Implements the subset of the JDK Signature interface needed for
 * signature verification using OpenSSL.
 */
class OpenSSLSignature
    : public SignatureSpi
    , public IOpenSSLSignature
{
public:
    CAR_INTERFACE_DECL()

    OpenSSLSignature();

    CARAPI EngineUpdate(
        /* [in] */ Byte input);

    CARAPI EngineUpdate(
        /* [in] */ ArrayOf<Byte>* input,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 len);

    CARAPI EngineGetParameter(
        /* [in] */ const String& param,
        /* [out] */ IInterface** result);

    CARAPI EngineInitSign(
        /* [in] */ IPrivateKey* privateKey);

    CARAPI EngineInitVerify(
        /* [in] */ IPublicKey* publicKey);

    CARAPI EngineSetParameter(
        /* [in] */ const String& param,
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

class OpenSSLSignatureMD5RSA
    : public OpenSSLSignature
    , public IOpenSSLSignatureMD5RSA
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

private:
    static Int64 EVP_MD;
};

class OpenSSLSignatureSHA1RSA
    : public OpenSSLSignature
    , public IOpenSSLSignatureSHA1RSA
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

private:
    static Int64 EVP_MD;
};

class OpenSSLSignatureSHA224RSA
    : public OpenSSLSignature
    , public IOpenSSLSignatureSHA224RSA
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

private:
    static Int64 EVP_MD;
};

class OpenSSLSignatureSHA256RSA
    : public OpenSSLSignature
    , public IOpenSSLSignatureSHA256RSA
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

private:
    static Int64 EVP_MD;
};

class OpenSSLSignatureSHA384RSA
    : public OpenSSLSignature
    , public IOpenSSLSignatureSHA384RSA
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

private:
    static Int64 EVP_MD;
};

class OpenSSLSignatureSHA512RSA
    : public OpenSSLSignature
    , public IOpenSSLSignatureSHA512RSA
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

private:
    static Int64 EVP_MD;
};

class OpenSSLSignatureSHA1DSA
    : public OpenSSLSignature
    , public IOpenSSLSignatureSHA1DSA
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

private:
    static Int64 EVP_MD;
};

class OpenSSLSignatureSHA1ECDSA
    : public OpenSSLSignature
    , public IOpenSSLSignatureSHA1ECDSA
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

private:
    static Int64 EVP_MD;
};

class OpenSSLSignatureSHA224ECDSA
    : public OpenSSLSignature
    , public IOpenSSLSignatureSHA224ECDSA
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

private:
    static Int64 EVP_MD;
};

class OpenSSLSignatureSHA256ECDSA
    : public OpenSSLSignature
    , public IOpenSSLSignatureSHA256ECDSA
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

private:
    static Int64 EVP_MD;
};

class OpenSSLSignatureSHA384ECDSA
    : public OpenSSLSignature
    , public IOpenSSLSignatureSHA384ECDSA
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

private:
    static Int64 EVP_MD;
};

class OpenSSLSignatureSHA512ECDSA
    : public OpenSSLSignature
    , public IOpenSSLSignatureSHA512ECDSA
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

private:
    static Int64 EVP_MD;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_OPENSSLSIGNATURE_H__