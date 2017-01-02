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

#ifndef __ELASTOS_SECURITY_KEYPAIRGENERATORSPI_H__
#define __ELASTOS_SECURITY_KEYPAIRGENERATORSPI_H__

#include "Elastos.CoreLibrary.Security.h"
#include <elastos/core/Object.h>

using Elastos::Security::Spec::IAlgorithmParameterSpec;

namespace Elastos {
namespace Security {

/**
 * {@code KeyPairGeneratorSpi} is the Service Provider Interface (SPI)
 * definition for {@link KeyPairGenerator}.
 *
 * @see KeyPairGenerator
 */
class ECO_PUBLIC KeyPairGeneratorSpi
    : public Object
    , public IKeyPairGeneratorSpi
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Constructs a new instance of {@code KeyPairGeneratorSpi}.
     */
    KeyPairGeneratorSpi()
    {}

    virtual ~KeyPairGeneratorSpi() = 0;

    /**
     * Initializes this {@code KeyPairGeneratorSpi} with the given {@code
     * AlgorithmParameterSpec} and the given {@code SecureRandom}.
     *
     * @param params
     *            the parameters to use.
     * @param random
     *            the source of randomness.
     * @throws InvalidAlgorithmParameterException
     *             if the specified parameters are not supported.
     */
    CARAPI Initialize(
        /* [in] */ IAlgorithmParameterSpec* params,
        /* [in] */ ISecureRandom* random) /*throws InvalidAlgorithmParameterException*/;

    virtual CARAPI Initialize(
        /* [in] */ Int32 keysize,
        /* [in] */ ISecureRandom* random) = 0;
};

} // namespace Security
} // namespace Elastos

#endif // __ELASTOS_SECURITY_KEYPAIRGENERATORSPI_H__
