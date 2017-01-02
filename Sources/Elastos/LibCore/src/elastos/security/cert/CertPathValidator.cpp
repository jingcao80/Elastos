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

#include "security/cert/CertPathValidator.h"
#include "security/CSecurity.h"

using Elastos::Security::ISecurity;
using Elastos::Security::CSecurity;

namespace Elastos {
namespace Security {
namespace Cert {

// Store CertPathValidator implementation service name
const String CertPathValidator::SERVICE(String("CertPathValidator"));

//Apache...Todo
// Used to access common engine functionality
//static final Engine ENGINE = new Engine(SERVICE);

// Store default property name
const String CertPathValidator::PROPERTY_NAME(String("certpathvalidator.type"));

// Default value of CertPathBuilder type. It returns if certpathbuild.type
// property is not defined in java.security file
const String CertPathValidator::DEFAULT_PROPERTY(String("PKIX"));

CAR_INTERFACE_IMPL(CertPathValidator, Object, ICertPathValidator)
ECode CertPathValidator::GetAlgorithm(
    /* [out] */ String *algorithm)
{
    VALIDATE_NOT_NULL(algorithm)
    *algorithm = mAlgorithm;
    return NOERROR;
}

ECode CertPathValidator::GetProvider(
    /* [out] */ IProvider **provider)
{
    VALIDATE_NOT_NULL(provider)
    *provider = mProvider;
    REFCOUNT_ADD(*provider)
    return NOERROR;
}

ECode CertPathValidator::Validate(
    /* [in] */ ICertPath *certPath,
    /* [in] */ ICertPathParameters *params,
    /* [out] */ ICertPathValidatorResult **result)
{
    return mSpiImpl->EngineValidate(certPath, params, result);
}

ECode CertPathValidator::GetInstance(
/* [in] */ const String& algorithm,
/* [out] */ ICertPathValidator **validator)
{
    VALIDATE_NOT_NULL(validator)
    if (algorithm.IsNull()) {
        return E_NULL_POINTER_EXCEPTION;
    }
    assert(0 && "TODO");
    //Apache...Todo
    /*
    Engine.SpiAndProvider sap = ENGINE.getInstance(algorithm, null);
    return new CertPathValidator((CertPathValidatorSpi) sap.spi, sap.provider, algorithm);
    */
    return E_NOT_IMPLEMENTED;
}

ECode CertPathValidator::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ const String& provider,
    /* [out] */ ICertPathValidator **validator)
{
    if (provider.IsNullOrEmpty()) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IProvider> impProvider;
    AutoPtr<ISecurity> security;
    CSecurity::AcquireSingleton((ISecurity**)&security);
    FAIL_RETURN(security->GetProvider(provider, (IProvider**)&impProvider))
    if (impProvider == NULL) {
        return E_NO_SUCH_PROVIDER_EXCEPTION;
    }
    return GetInstance(algorithm, impProvider, validator);
}

ECode CertPathValidator::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ IProvider *provider,
    /* [out] */ ICertPathValidator **validator)
{
    if (!provider) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (algorithm.IsNull()) {
        return E_NULL_POINTER_EXCEPTION;
    }
    //Apache...Todo
    /*
    Object spi = ENGINE.getInstance(algorithm, provider, null);
    return new CertPathValidator((CertPathValidatorSpi) spi, provider, algorithm);
    */
    return E_NOT_IMPLEMENTED;
}

ECode CertPathValidator::GetDefaultType(
    /* [out] */ String *type)
{
    VALIDATE_NOT_NULL(type)
    String defaultType;
    AutoPtr<ISecurity> security;
    CSecurity::AcquireSingleton((ISecurity**)&security);
    security->GetProperty(PROPERTY_NAME, &defaultType);
    *type = !defaultType.IsNull() ? defaultType : DEFAULT_PROPERTY;
    return NOERROR;
}

CertPathValidator::CertPathValidator(
    /* [in] */ ICertPathValidatorSpi* validatorSpi,
    /* [in] */ IProvider* provider,
    /* [in] */ const String& algorithm)
    : mProvider(provider)
    , mSpiImpl(validatorSpi)
    , mAlgorithm(algorithm)
{}

}
}
}

