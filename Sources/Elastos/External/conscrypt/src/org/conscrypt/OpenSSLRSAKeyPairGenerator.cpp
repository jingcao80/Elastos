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

#include "Elastos.CoreLibrary.Security.h"
#include "OpenSSLRSAKeyPairGenerator.h"
#include "org/conscrypt/COpenSSLKey.h"
#include "org/conscrypt/NativeCrypto.h"
#include "org/conscrypt/OpenSSLRSAPrivateKey.h"
#include "org/conscrypt/OpenSSLRSAPublicKey.h"

using Elastos::Math::IBigInteger;
using Elastos::Security::CKeyPair;
using Elastos::Security::EIID_IKeyPairGeneratorSpi;
using Elastos::Security::IPrivateKey;
using Elastos::Security::IPublicKey;
using Elastos::Security::Spec::IRSAKeyGenParameterSpec;

namespace Org {
namespace Conscrypt {

//=========================================
// OpenSSLRSAKeyPairGenerator::
//=========================================
CAR_INTERFACE_IMPL_2(OpenSSLRSAKeyPairGenerator, Object, IKeyPairGeneratorSpi, IOpenSSLRSAKeyPairGenerator)

OpenSSLRSAKeyPairGenerator::OpenSSLRSAKeyPairGenerator()
    : mModulusBits(2048)
{
    mPublicExponent = ArrayOf<Byte>::Alloc(3);
    (*mPublicExponent)[0] = 0x01;
    (*mPublicExponent)[1] = 0x00;
    (*mPublicExponent)[2] = 0x01;
}

ECode OpenSSLRSAKeyPairGenerator::GenerateKeyPair(
    /* [out] */ IKeyPair** result)
{
    VALIDATE_NOT_NULL(result)

    Int64 val = 0;
    NativeCrypto::RSA_generate_key_ex(mModulusBits, mPublicExponent, &val);
    AutoPtr<IOpenSSLKey> key;
    COpenSSLKey::New(val, (IOpenSSLKey**)&key);

    AutoPtr<IPrivateKey> privKey = IPrivateKey::Probe(
            OpenSSLRSAPrivateKey::GetInstance(key));
    AutoPtr<OpenSSLRSAPublicKey> opensslRsaPublicKey = new OpenSSLRSAPublicKey();
    opensslRsaPublicKey->constructor(key);
    AutoPtr<IPublicKey> pubKey = IPublicKey::Probe(opensslRsaPublicKey);

    AutoPtr<IKeyPair> p;
    CKeyPair::New(pubKey, privKey, (IKeyPair**)&p);
    *result = p;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLRSAKeyPairGenerator::Initialize(
    /* [in] */ Int32 keysize,
    /* [in] */ ISecureRandom* random)
{
    mModulusBits = keysize;
    return NOERROR;
}

ECode OpenSSLRSAKeyPairGenerator::Initialize(
    /* [in] */ IAlgorithmParameterSpec* params,
    /* [in] */ ISecureRandom* random)
{
    if (IRSAKeyGenParameterSpec::Probe(params) == NULL) {
        // throw new InvalidAlgorithmParameterException("Only RSAKeyGenParameterSpec supported");
        return E_INVALID_ALGORITHM_PARAMETER_EXCEPTION;
    }

    AutoPtr<IRSAKeyGenParameterSpec> spec = IRSAKeyGenParameterSpec::Probe(params);

    AutoPtr<IBigInteger> publicExponent;
    spec->GetPublicExponent((IBigInteger**)&publicExponent);
    if (publicExponent != NULL) {
        publicExponent->ToByteArray((ArrayOf<Byte>**)&mPublicExponent);
    }

    spec->GetKeysize(&mModulusBits);
    return NOERROR;
}

} // namespace Conscrypt
} // namespace Org
