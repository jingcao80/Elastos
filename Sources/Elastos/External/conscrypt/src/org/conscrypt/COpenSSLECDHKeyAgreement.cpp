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

#include "org/conscrypt/COpenSSLECDHKeyAgreement.h"
#include "org/conscrypt/NativeCrypto.h"
#include "org/conscrypt/OpenSSLKey.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Security::IPrivateKey;
using Elastos::Security::IPublicKey;
using Elastosx::Crypto::Spec::CSecretKeySpec;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Conscrypt {

CAR_OBJECT_IMPL(COpenSSLECDHKeyAgreement)

CAR_INTERFACE_IMPL(COpenSSLECDHKeyAgreement, KeyAgreementSpi,
        IOpenSSLECDHKeyAgreement)

ECode COpenSSLECDHKeyAgreement::constructor()
{
    return KeyAgreementSpi::constructor();
}

ECode COpenSSLECDHKeyAgreement::EngineDoPhase(
    /* [in] */ IKey* key,
    /* [in] */ Boolean lastPhase,
    /* [out] */ IKey** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    if (mOpenSslPrivateKey == NULL) {
        // throw new IllegalStateException("Not initialized");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (!lastPhase) {
        // throw new IllegalStateException("ECDH only has one phase");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    if (key == NULL) {
        // throw new InvalidKeyException("key == NULL");
        return E_INVALID_KEY_EXCEPTION;
    }
    if (IPublicKey::Probe(key) == NULL) {
        // throw new InvalidKeyException("Not a public key: " + key.getClass());
        return E_INVALID_KEY_EXCEPTION;
    }
    AutoPtr<IOpenSSLKey> openSslPublicKey = OpenSSLKey::FromPublicKey(IPublicKey::Probe(key));

    AutoPtr<ArrayOf<Byte> > buffer = ArrayOf<Byte>::Alloc(mExpectedResultLength);
    Int64 publicKeyCtx;
    openSslPublicKey->GetPkeyContext(&publicKeyCtx);
    Int64 privateKeyCtx;
    mOpenSslPrivateKey->GetPkeyContext(&privateKeyCtx);
    Int32 actualResultLength;
    NativeCrypto::ECDH_compute_key(
            buffer,
            0,
            publicKeyCtx,
            privateKeyCtx, &actualResultLength);
    AutoPtr<ArrayOf<Byte> > array;
    if (actualResultLength == -1) {
        // throw new RuntimeException("Engine returned " + actualResultLength);
        return E_RUNTIME_EXCEPTION;
    }
    else if (actualResultLength == mExpectedResultLength) {
        // The output is as long as expected -- use the whole buffer
        array = buffer;
    }
    else if (actualResultLength < mExpectedResultLength) {
        // The output is shorter than expected -- use only what's produced by the engine
        array = ArrayOf<Byte>::Alloc(actualResultLength);
        array->Copy(0, buffer, 0, array->GetLength());
    }
    else {
        // The output is longer than expected
        // throw new RuntimeException("Engine produced a longer than expected result. Expected: "
        //     + mExpectedResultLength + ", actual: " + actualResultLength);
        return E_RUNTIME_EXCEPTION;
    }
    mResult = array;

    *result = NULL; // No intermediate key
    return NOERROR;
}

ECode COpenSSLECDHKeyAgreement::EngineGenerateSecret(
    /* [in] */ ArrayOf<Byte>* sharedSecret,
    /* [in] */ Int32 offset,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;
    CheckCompleted();
    Int32 available = sharedSecret->GetLength() - offset;
    if (mResult->GetLength() > available) {
        // throw new ShortBufferException(
        //         "Needed: " + mResult.length + ", available: " + available);
        return E_SHORT_BUFFER_EXCEPTION;
    }

    sharedSecret->Copy(offset, mResult, 0, mResult->GetLength());
    *result = mResult->GetLength();
    return NOERROR;
}

ECode COpenSSLECDHKeyAgreement::EngineGenerateSecret(
    /* [out] */ ArrayOf<Byte> ** result)
{
    VALIDATE_NOT_NULL(result)
    CheckCompleted();
    *result = mResult;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode COpenSSLECDHKeyAgreement::EngineGenerateSecret(
    /* [in] */ const String& algorithm,
    /* [out] */ ISecretKey** result)
{
    VALIDATE_NOT_NULL(result)
    CheckCompleted();
    AutoPtr<ArrayOf<Byte> > array;
    EngineGenerateSecret((ArrayOf<Byte>**)&array);
    return CSecretKeySpec::New(array, algorithm, result);
}

ECode COpenSSLECDHKeyAgreement::EngineInit(
    /* [in] */ IKey* key,
    /* [in] */ ISecureRandom* random)
{
    if (key == NULL) {
        // throw new InvalidKeyException("key == NULL");
        Logger::E("COpenSSLECDHKeyAgreement", "EngineInit key == NULL");
        return E_INVALID_KEY_EXCEPTION;
    }
    if (IPrivateKey::Probe(key) == NULL) {
        // throw new InvalidKeyException("Not a private key: " + key.getClass());
        Logger::E("COpenSSLECDHKeyAgreement", "EngineInit Not a private key");
        return E_INVALID_KEY_EXCEPTION;
    }

    AutoPtr<IOpenSSLKey> openSslKey = OpenSSLKey::FromPrivateKey(IPrivateKey::Probe(key));
    Int64 ctx;
    openSslKey->GetPkeyContext(&ctx);
    Int64 val;
    NativeCrypto::EC_KEY_get0_group(ctx, &val);
    Int32 fieldSizeBits;
    NativeCrypto::EC_GROUP_get_degree(val, &fieldSizeBits);
    mExpectedResultLength = (fieldSizeBits + 7) / 8;
    mOpenSslPrivateKey = openSslKey;
    return NOERROR;
}

ECode COpenSSLECDHKeyAgreement::EngineInit(
    /* [in] */ IKey* key,
    /* [in] */ IAlgorithmParameterSpec* params,
    /* [in] */ ISecureRandom* random)
{
    // ECDH doesn't need an AlgorithmParameterSpec
    if (params != NULL) {
        // throw new InvalidAlgorithmParameterException("No algorithm parameters supported");
        return E_INVALID_ALGORITHM_PARAMETER_EXCEPTION;
    }
    EngineInit(key, random);
    return NOERROR;
}

ECode COpenSSLECDHKeyAgreement::CheckCompleted()
{
    if (mResult == NULL) {
        // throw new IllegalStateException("Key agreement not completed");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

} // namespace Conscrypt
} // namespace Org
