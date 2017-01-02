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

#include "org/conscrypt/OpenSSLDSAKeyPairGenerator.h"
#include "org/conscrypt/NativeCrypto.h"
#include "org/conscrypt/OpenSSLDSAPrivateKey.h"
#include "org/conscrypt/COpenSSLDSAPublicKey.h"
#include "org/conscrypt/OpenSSLKey.h"

using Elastos::Security::CKeyPair;
using Elastos::Security::Interfaces::IDSAParams;
using Elastos::Security::Spec::IDSAParameterSpec;
using Elastos::Utility::IRandom;

namespace Org {
namespace Conscrypt {

CAR_INTERFACE_IMPL(OpenSSLDSAKeyPairGenerator, KeyPairGeneratorSpi,
        IOpenSSLDSAKeyPairGenerator)

OpenSSLDSAKeyPairGenerator::OpenSSLDSAKeyPairGenerator()
    : mPrimeBits(1024)
{
}

ECode OpenSSLDSAKeyPairGenerator::constructor()
{
    return NOERROR;
}

ECode OpenSSLDSAKeyPairGenerator::GenerateKeyPair(
    /* [out] */  IKeyPair** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ArrayOf<Byte> > seed;
    if (mRandom == NULL) {
        seed = NULL;
    }
    else {
        seed = ArrayOf<Byte>::Alloc(20);
        IRandom::Probe(mRandom)->NextBytes(seed);
    }

    Int64 val;
    NativeCrypto::DSA_generate_key(mPrimeBits, seed, mG, mP, mQ, &val);
    AutoPtr<OpenSSLKey> key = new OpenSSLKey();
    key->constructor(val);

    AutoPtr<OpenSSLDSAPrivateKey> privKey = new OpenSSLDSAPrivateKey();
    privKey->constructor(IOpenSSLKey::Probe(key));
    AutoPtr<IOpenSSLDSAPublicKey> pubKey;
    COpenSSLDSAPublicKey::New(IOpenSSLKey::Probe(key), (IOpenSSLDSAPublicKey**)&pubKey);

    return CKeyPair::New(IPublicKey::Probe(pubKey), IPrivateKey::Probe(privKey), result);
}

ECode OpenSSLDSAKeyPairGenerator::Initialize(
    /* [in] */ Int32 keysize,
    /* [in] */ ISecureRandom* random)
{
    mPrimeBits = keysize;
    mRandom = random;
    return NOERROR;
}

ECode OpenSSLDSAKeyPairGenerator::Initialize(
    /* [in] */ IAlgorithmParameterSpec* params,
    /* [in] */ ISecureRandom* random)
{
    mRandom = random;

    if (IDSAParameterSpec::Probe(params) != NULL) {
        AutoPtr<IDSAParameterSpec> dsaParams = IDSAParameterSpec::Probe(params);

        AutoPtr<IBigInteger> gInt;
        IDSAParams::Probe(dsaParams)->GetG((IBigInteger**)&gInt);
        if (gInt != NULL) {
            gInt->ToByteArray((ArrayOf<Byte>**)&mG);
        }

        AutoPtr<IBigInteger> pInt;
        IDSAParams::Probe(dsaParams)->GetP((IBigInteger**)&pInt);
        if (pInt != NULL) {
            pInt->ToByteArray((ArrayOf<Byte>**)&mP);
        }

        AutoPtr<IBigInteger> qInt;
        IDSAParams::Probe(dsaParams)->GetQ((IBigInteger**)&qInt);
        if (qInt != NULL) {
            qInt->ToByteArray((ArrayOf<Byte>**)&mQ);
        }
    }
    else if (params != NULL) {
        // throw new InvalidAlgorithmParameterException("Params must be DSAParameterSpec");
        return E_INVALID_ALGORITHM_PARAMETER_EXCEPTION;
    }
    return NOERROR;
}

} // namespace Conscrypt
} // namespace Org
