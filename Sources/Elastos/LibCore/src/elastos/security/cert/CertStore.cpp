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

#include "security/cert/CertStore.h"
#include "security/CSecurity.h"

using Elastos::Security::CSecurity;
using Elastos::Security::ISecurity;
using Elastos::Core::ICloneable;

namespace Elastos {
namespace Security {
namespace Cert {

const String CertStore::SERVICE = String("CertStore");

//Apache...Todo
// Used to access common engine functionality
//static const Engine ENGINE = new Engine(SERVICE);

const String CertStore::PROPERTY_NAME = String("certstore.type");

// Default value of CertStore type. It returns if certpathbuild.type
// property is not defined in java.security file
const String CertStore::DEFAULT_PROPERTY = String("LDAP");

CAR_INTERFACE_IMPL(CertStore, Object, ICertStore)
ECode CertStore:: GetInstance(
    /* [in] */ const String& type,
    /* [in] */ ICertStoreParameters *params,
    /* [out] */ ICertStore **certStore)
{
    VALIDATE_NOT_NULL(certStore)
    if (type.IsNull()) {
        return E_NULL_POINTER_EXCEPTION;
    }
    assert(0 && "TODO");
    //Apache...Todo
    /*
    try {
        Engine.SpiAndProvider sap = ENGINE.getInstance(type, params);
        return new CertStore((CertStoreSpi) sap.spi, sap.provider, type, params);
    } catch (NoSuchAlgorithmException e) {
        Throwable th = e.getCause();
        if (th == null) {
            throw e;
        } else {
            throw new InvalidAlgorithmParameterException(e.getMessage(), th);
        }
    }
    */
    return E_NOT_IMPLEMENTED;
}

ECode CertStore:: GetInstance(
    /* [in] */ const String& type,
    /* [in] */ ICertStoreParameters *params,
    /* [in] */ const String& provider,
    /* [out] */ ICertStore **certStore)
{
    if (provider.IsNullOrEmpty()) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IProvider> impProvider;
    AutoPtr<ISecurity> security;
    CSecurity::AcquireSingleton((ISecurity**)&security);
    security->GetProvider(provider, (IProvider**)&impProvider);
    if (impProvider == NULL) {
        return E_NO_SUCH_PROVIDER_EXCEPTION;
    }
    return GetInstance(type, params, impProvider, certStore);
}

ECode CertStore:: GetInstance(
    /* [in] */ const String& type,
    /* [in] */ ICertStoreParameters *params,
    /* [in] */ IProvider *provider,
    /* [out] */ ICertStore **certStore)
{
    if (provider == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (type.IsNull()) {
        return E_NULL_POINTER_EXCEPTION;
    }
    //Apache...Todo later
    /*
    try {
        Object spi = ENGINE.getInstance(type, provider, params);
        return new CertStore((CertStoreSpi) spi, provider, type, params);
    } catch (NoSuchAlgorithmException e) {
        Throwable th = e.getCause();
        if (th == null) {
            throw e;
        } else {
            throw new InvalidAlgorithmParameterException(e.getMessage(), th);
        }
    }
    */
    return E_NOT_IMPLEMENTED;
}

ECode CertStore:: GetDefaultType(
    /* [out] */ String *type)
{
    VALIDATE_NOT_NULL(type)
    String defaultType;
    AutoPtr<ISecurity> security;
    CSecurity::AcquireSingleton((ISecurity**)&security);
    security->GetProperty(PROPERTY_NAME, &defaultType);
    *type = defaultType.IsNull() ? DEFAULT_PROPERTY : defaultType;
    return NOERROR;
}

ECode CertStore::GetType(
    /* [out] */ String *type)
{
    VALIDATE_NOT_NULL(type)
    *type = mType;
    return NOERROR;
}

ECode CertStore::GetProvider(
    /* [out] */ IProvider **provider)
{
    VALIDATE_NOT_NULL(provider)
    *provider = mProvider;
    REFCOUNT_ADD(*provider)
    return NOERROR;
}

ECode CertStore::GetCertStoreParameters(
    /* [out] */ ICertStoreParameters **parameters)
{
    if (mCertStoreParams == NULL) {
        return NOERROR;
    }
    else {
        AutoPtr<IInterface> clonedObj;
        ICloneable::Probe(mCertStoreParams)->Clone((IInterface**)&clonedObj);
        *parameters = ICertStoreParameters::Probe(clonedObj);
        REFCOUNT_ADD(*parameters)
        return NOERROR;
    }
}

ECode CertStore::GetCertificates(
    /* [in] */ ICertSelector *selector,
    /* [out] */ ICollection **certs)
{
    return mSpiImpl->EngineGetCertificates(selector, certs);
}

ECode CertStore::GetCRLs(
    /* [in] */ ICRLSelector *selector,
    /* [out] */ ICollection **crls)
{
    return mSpiImpl->EngineGetCRLs(selector, crls);
}

CertStore::CertStore(
    /* [in] */ ICertStoreSpi* storeSpi,
    /* [in] */ IProvider* provider,
    /* [in] */ const String& type,
    /* [in] */ ICertStoreParameters* params)
    : mProvider(provider)
    , mSpiImpl(storeSpi)
    , mType(type)
    , mCertStoreParams(params)
{}

}
}
}

