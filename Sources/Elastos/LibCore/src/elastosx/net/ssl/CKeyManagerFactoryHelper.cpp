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

#include "CKeyManagerFactoryHelper.h"
#include "KeyManagerFactory.h"

namespace Elastosx {
namespace Net {
namespace Ssl {

CAR_INTERFACE_IMPL(CKeyManagerFactoryHelper, Singleton, IKeyManagerFactoryHelper)

CAR_SINGLETON_IMPL(CKeyManagerFactoryHelper)

ECode CKeyManagerFactoryHelper::GetDefaultAlgorithm(
    /* [out] */ String* algorithm)
{
    VALIDATE_NOT_NULL(algorithm)

    return KeyManagerFactory::GetDefaultAlgorithm(algorithm);
}

ECode CKeyManagerFactoryHelper::GetInstance(
    /* [in] */ const String& algorithm,
    /* [out] */ IKeyManagerFactory** factory)
{
    VALIDATE_NOT_NULL(factory)

    return KeyManagerFactory::GetInstance(algorithm, factory);
}

ECode CKeyManagerFactoryHelper::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ const String& provider,
    /* [out] */ IKeyManagerFactory** factory)
{
    VALIDATE_NOT_NULL(factory)

    return KeyManagerFactory::GetInstance(algorithm, provider, factory);
}

ECode CKeyManagerFactoryHelper::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ IProvider* provider,
    /* [out] */ IKeyManagerFactory** factory)
{
    VALIDATE_NOT_NULL(factory)

    return KeyManagerFactory::GetInstance(algorithm, provider, factory);
}

} // namespace Ssl
} // namespace Net
} // namespace Elastosx
