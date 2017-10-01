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

#ifndef __ELASTOSX_CRYPTO_SECRETKEYFACTORYSPI_H__
#define __ELASTOSX_CRYPTO_SECRETKEYFACTORYSPI_H__

#include "Elastos.CoreLibrary.Extensions.h"
#include <elastos/core/Object.h>

using Elastos::Security::Spec::IKeySpec;
using Elastosx::Crypto::ISecretKey;;

namespace Elastosx {
namespace Crypto {

class SecretKeyFactorySpi
    : public Object
    , public ISecretKeyFactorySpi
{
public:
    CAR_INTERFACE_DECL()

    SecretKeyFactorySpi();

    virtual ~SecretKeyFactorySpi();

    /**
     * Creates a new {@code SecretKeyFactorySpi} instance.
     */
    CARAPI constructor();
};

}
}

#endif // __ELASTOSX_CRYPTO_SECRETKEYFACTORYSPI_H__
