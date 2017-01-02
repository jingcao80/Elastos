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
#include "TrustManagerFactory.h"
#include "CTrustManagerFactory.h"
#include "CSecurity.h"
#include "org/apache/harmony/security/fortress/CEngine.h"

using Elastos::Security::CSecurity;
using Elastos::Security::ISecurity;
using Org::Apache::Harmony::Security::Fortress::CEngine;
using Org::Apache::Harmony::Security::Fortress::ISpiAndProvider;

namespace Elastosx {
namespace Net {
namespace Ssl {

static AutoPtr<IEngine> InitEngine()
{
    AutoPtr<CEngine> e;
    CEngine::NewByFriend(String("TrustManagerFactory")/*SERVICE*/, (CEngine**)&e);
    return e;
}

const String TrustManagerFactory::SERVICE = String("TrustManagerFactory");
AutoPtr<IEngine> TrustManagerFactory::ENGINE = InitEngine();
const String TrustManagerFactory::PROPERTY_NAME = String("ssl.TrustManagerFactory.algorithm");
const String TrustManagerFactory::DEFAULT_PROPERTY = String("PKIX");

CAR_INTERFACE_IMPL(TrustManagerFactory, Object, ITrustManagerFactory)

ECode TrustManagerFactory::GetDefaultAlgorithm(
    /* [out] */ String* algorithm)
{
    VALIDATE_NOT_NULL(algorithm)

    String _algorithm;
    AutoPtr<ISecurity> security;
    CSecurity::AcquireSingleton((ISecurity**)&security);
    security->GetProperty(PROPERTY_NAME, &_algorithm);
    *algorithm = (!_algorithm.IsNull() ? _algorithm : DEFAULT_PROPERTY);
    return NOERROR;
}

ECode TrustManagerFactory::GetInstance(
    /* [in] */ const String& algorithm,
    /* [out] */ ITrustManagerFactory** factory)
{
    VALIDATE_NOT_NULL(factory)

    if (algorithm.IsNull()) {
        //throw new NullPointerException("algorithm == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    AutoPtr<ISpiAndProvider> sap;
    ENGINE->GetInstance(algorithm, NULL, (ISpiAndProvider**)&sap);
    AutoPtr<IInterface> spi;
    sap->GetSpi((IInterface**)&spi);
    AutoPtr<IProvider> provider;
    sap->GetProvider((IProvider**)&provider);
    return CTrustManagerFactory::New(ITrustManagerFactorySpi::Probe(spi), provider, algorithm, factory);
}

ECode TrustManagerFactory::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ const String& provider,
    /* [out] */ ITrustManagerFactory** factory)
{
    VALIDATE_NOT_NULL(factory)

    if ((provider.IsNull()) || (provider.GetLength() == 0)) {
        //throw new IllegalArgumentException("Provider is null or empty");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IProvider> impProvider;
    AutoPtr<ISecurity> security;
    CSecurity::AcquireSingleton((ISecurity**)&security);
    security->GetProvider(provider, (IProvider**)&impProvider);
    if (impProvider == NULL) {
        //throw new NoSuchProviderException(provider);
        return E_NO_SUCH_PROVIDER_EXCEPTION;
    }
    return GetInstance(algorithm, impProvider, factory);
}

ECode TrustManagerFactory::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ IProvider* provider,
    /* [out] */ ITrustManagerFactory** factory)
{
    VALIDATE_NOT_NULL(factory)

    if (provider == NULL) {
        //throw new IllegalArgumentException("Provider is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (algorithm.IsNull()) {
        //throw new NullPointerException("algorithm == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    AutoPtr<IInterface> spi;
    ENGINE->GetInstance(algorithm, provider, NULL, (IInterface**)&spi);
    return CTrustManagerFactory::New(ITrustManagerFactorySpi::Probe(spi), provider, algorithm, factory);
}

ECode TrustManagerFactory::constructor(
    /* [in] */ ITrustManagerFactorySpi* factorySpi,
    /* [in] */ IProvider* provider,
    /* [in] */ const String& algorithm)
{
    mProvider = provider;
    mAlgorithm = algorithm;
    mSpiImpl = factorySpi;
    return NOERROR;
}

ECode TrustManagerFactory::GetAlgorithm(
    /* [out] */ String* algorithm)
{
    VALIDATE_NOT_NULL(algorithm)

    *algorithm = mAlgorithm;
    return NOERROR;
}

ECode TrustManagerFactory::GetProvider(
    /* [out] */ IProvider** provider)
{
    VALIDATE_NOT_NULL(provider)

    *provider = mProvider;
    REFCOUNT_ADD(*provider)
    return NOERROR;
}

ECode TrustManagerFactory::Init(
    /* [in] */ IKeyStore* ks)
{
   return mSpiImpl->EngineInit(ks);
}

ECode TrustManagerFactory::Init(
    /* [in] */ IManagerFactoryParameters* spec)
{
    return mSpiImpl->EngineInit(spec);
}

ECode TrustManagerFactory::GetTrustManagers(
    /* [out, callee] */ ArrayOf<ITrustManager*>** managers)
{
    VALIDATE_NOT_NULL(managers)

    return mSpiImpl->EngineGetTrustManagers(managers);
}

} // namespace Ssl
} // namespace Net
} // namespace Elastosx
