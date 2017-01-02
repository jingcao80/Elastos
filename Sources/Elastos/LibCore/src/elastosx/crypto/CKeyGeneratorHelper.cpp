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

#include "CKeyGeneratorHelper.h"
#include "CKeyGenerator.h"

namespace Elastosx {
namespace Crypto {

CAR_INTERFACE_IMPL(CKeyGeneratorHelper, Singleton, IKeyGeneratorHelper)

CAR_SINGLETON_IMPL(CKeyGeneratorHelper)

ECode CKeyGeneratorHelper::GetInstance(
    /* [in] */ const String& algorithm,
    /* [out] */ IKeyGenerator ** ka)
{
    VALIDATE_NOT_NULL(ka);
    return CKeyGenerator::GetInstance(algorithm, ka);
}

ECode CKeyGeneratorHelper::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ const String& provider,
    /* [out] */ IKeyGenerator ** ka)
{
    VALIDATE_NOT_NULL(ka);
    return CKeyGenerator::GetInstance(algorithm, provider, ka);
}

ECode CKeyGeneratorHelper::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ IProvider * provider,
    /* [out] */ IKeyGenerator ** ka)
{
    VALIDATE_NOT_NULL(ka);
    return CKeyGenerator::GetInstance(algorithm, provider, ka);
}

} // Crypto
} // Elastosx