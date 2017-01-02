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
#include "CKeyGenerator.h"
#include "AutoLock.h"
#include "CSecurity.h"
#include "CSecureRandom.h"
#include "org/apache/harmony/security/fortress/CEngine.h"

using Elastos::Security::CSecureRandom;
using Elastos::Security::CSecurity;
using Elastos::Security::ISecurity;
using Org::Apache::Harmony::Security::Fortress::CEngine;
using Org::Apache::Harmony::Security::Fortress::ISpiAndProvider;

namespace Elastosx {
namespace Crypto {

static AutoPtr<IEngine> InitEngine()
{
    AutoPtr<CEngine> e;
    CEngine::NewByFriend(String("KeyGenerator"), (CEngine**)&e);
    return e;
}

static AutoPtr<ISecureRandom> InitSecureRandom()
{
    AutoPtr<CSecureRandom> r;
    CSecureRandom::NewByFriend((CSecureRandom**)&r);
    return r;
}

CAR_OBJECT_IMPL(CKeyGenerator)
CAR_INTERFACE_IMPL(CKeyGenerator, Object, IKeyGenerator)

AutoPtr<ISecureRandom> CKeyGenerator::RANDOM = InitSecureRandom();
AutoPtr<IEngine> CKeyGenerator::ENGINE = InitEngine();

CKeyGenerator::CKeyGenerator()
    : mAlgorithm(String(NULL))
{
}

ECode CKeyGenerator::constructor(
    /* [in] */ IKeyGeneratorSpi* keyGenSpi,
    /* [in] */ IProvider* provider,
    /* [in] */ const String& algorithm)
{
    mProvider = provider;
    mAlgorithm = algorithm;
    mSpiImpl = keyGenSpi;
    return NOERROR;
}

ECode CKeyGenerator::GetAlgorithm(
    /* [out] */ String * name)
{
    VALIDATE_NOT_NULL(name)
    *name = mAlgorithm;
    return NOERROR;
}

ECode CKeyGenerator::GetProvider(
    /* [out] */ IProvider ** provider)
{
    VALIDATE_NOT_NULL(provider)
    *provider = mProvider;
    REFCOUNT_ADD(*provider)
    return NOERROR;
}

ECode CKeyGenerator::GenerateKey(
    /* [out] */ ISecretKey** skey)
{
    VALIDATE_NOT_NULL(skey)
    return mSpiImpl->EngineGenerateKey(skey);
}

ECode CKeyGenerator::Init(
    /* [in] */ IAlgorithmParameterSpec * params)
{
    return mSpiImpl->EngineInit(params, RANDOM);//new SecureRandom());
}

ECode CKeyGenerator::Init(
    /* [in] */ IAlgorithmParameterSpec * params,
    /* [in] */ ISecureRandom * random)
{
    return mSpiImpl->EngineInit(params, random);
}

ECode CKeyGenerator::Init(
    /* [in] */ Int32 keysize)
{
    return mSpiImpl->EngineInit(keysize, RANDOM);//new SecureRandom());
}

ECode CKeyGenerator::Init(
    /* [in] */ Int32 keysize,
    /* [in] */ ISecureRandom * random)
{
    return mSpiImpl->EngineInit(keysize, random);
}

ECode CKeyGenerator::Init(
    /* [in] */ ISecureRandom * random)
{
    return mSpiImpl->EngineInit(random);
}

ECode CKeyGenerator::GetInstance(
    /* [in] */ const String& algorithm,
    /* [out] */ IKeyGenerator ** ka)
{
    VALIDATE_NOT_NULL(ka)
    *ka = NULL;
    if (algorithm == NULL) {
        // throw new NULLPointerException("algorithm == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    AutoPtr<ISpiAndProvider> sap;
    ENGINE->GetInstance(algorithm, NULL, (ISpiAndProvider**)&sap);
    AutoPtr<IInterface> spi;
    sap->GetSpi((IInterface**)&spi);
    AutoPtr<IProvider> provider;
    sap->GetProvider((IProvider**)&provider);
    return CKeyGenerator::New(IKeyGeneratorSpi::Probe(spi), provider, algorithm, ka);
}

ECode CKeyGenerator::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ const String& provider,
    /* [out] */ IKeyGenerator ** ka)
{
    VALIDATE_NOT_NULL(ka)
    *ka = NULL;
    if (provider == NULL || provider.IsEmpty()) {
        // throw new IllegalArgumentException("Provider is NULL or empty");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IProvider> impProvider;
    AutoPtr<ISecurity> security;
    CSecurity::AcquireSingleton((ISecurity**)&security);
    security->GetProvider(provider, (IProvider**)&impProvider);

    if (impProvider == NULL) {
        // throw new NoSuchProviderException(provider);
        return E_NO_SUCH_PROVIDER_EXCEPTION;
    }
    return GetInstance(algorithm, impProvider, ka);
}

ECode CKeyGenerator::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ IProvider * provider,
    /* [out] */ IKeyGenerator** ka)
{
    VALIDATE_NOT_NULL(ka)
    *ka = NULL;
    if (provider == NULL) {
        // throw new IllegalArgumentException("provider == NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (algorithm == NULL) {
        // throw new NULLPointerException("algorithm == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    AutoPtr<IInterface> spi;
    ENGINE->GetInstance(algorithm, provider, NULL, (IInterface**)&spi);
    return CKeyGenerator::New(IKeyGeneratorSpi::Probe(spi), provider, algorithm, ka);
}

} // Crypto
} // Elastosx