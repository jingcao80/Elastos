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

#ifndef __ELASTOS_SECURITY_CERT_CERTPATHVALIDATOR_H__
#define __ELASTOS_SECURITY_CERT_CERTPATHVALIDATOR_H__

#include "Elastos.CoreLibrary.Security.h"
#include "core/Object.h"

using Elastos::Core::Object;
using Elastos::Security::IProvider;

namespace Elastos {
namespace Security {
namespace Cert {

class CertPathValidator
    : public Object
    , public ICertPathValidator
{
public:
    CAR_INTERFACE_DECL()

    static CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [out] */ ICertPathValidator **validator);

    static CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [in] */ const String& provider,
        /* [out] */ ICertPathValidator **validator);

    static CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [in] */ IProvider *provider,
        /* [out] */ ICertPathValidator **validator);

    static CARAPI GetDefaultType(
        /* [out] */ String *type);
    CARAPI GetAlgorithm(
        /* [out] */ String *algorithm);

    CARAPI GetProvider(
        /* [out] */ IProvider **provider);

    CARAPI Validate(
        /* [in] */ ICertPath *certPath,
        /* [in] */ ICertPathParameters *params,
        /* [out] */ ICertPathValidatorResult **result);
protected:
    /**
     * Creates a new {@code CertPathValidator} instance.
     *
     * @param validatorSpi
     *            the implementation delegate.
     * @param provider
     *            the security provider.
     * @param algorithm
     *            the name of the algorithm.
     */
    CertPathValidator(
        /* [in] */ ICertPathValidatorSpi* validatorSpi,
        /* [in] */ IProvider* provider,
        /* [in] */ const String& algorithm);

private:
    // Store CertPathValidator implementation service name
    static const String SERVICE;

    //Apache...Todo
    // Used to access common engine functionality
    //static final Engine ENGINE = new Engine(SERVICE);

    // Store default property name
    static const String PROPERTY_NAME;

    // Default value of CertPathBuilder type. It returns if certpathbuild.type
    // property is not defined in java.security file
    static const String DEFAULT_PROPERTY;

    // Store used provider
    const AutoPtr<IProvider> mProvider;

    // Store used spi implementation
    const AutoPtr<ICertPathValidatorSpi> mSpiImpl;

    // Store used algorithm value
    const String mAlgorithm;
};

}
}
}

#endif // __ELASTOS_SECURITY_CERT_CERTPATHVALIDATOR_H__
