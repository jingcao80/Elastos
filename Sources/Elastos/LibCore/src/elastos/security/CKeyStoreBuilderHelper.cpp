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

#include "CKeyStoreBuilderHelper.h"
#include "KeyStoreBuilder.h"

namespace Elastos {
namespace Security {

CAR_SINGLETON_IMPL(CKeyStoreBuilderHelper)

CAR_INTERFACE_IMPL(CKeyStoreBuilderHelper, Singleton, IKeyStoreBuilderHelper)

ECode CKeyStoreBuilderHelper::NewInstance(
    /* [in] */ IKeyStore* keyStore,
    /* [in] */ IKeyStoreProtectionParameter* protectionParameter,
    /* [out] */ IKeyStoreBuilder** instance)
{
    return KeyStoreBuilder::NewInstance(keyStore, protectionParameter, instance);
}

ECode CKeyStoreBuilderHelper::NewInstance(
    /* [in] */ const String& type,
    /* [in] */ IProvider* provider,
    /* [in] */ IFile* file,
    /* [in] */ IKeyStoreProtectionParameter* protectionParameter,
    /* [out] */ IKeyStoreBuilder** instance)
{
    return KeyStoreBuilder::NewInstance(type, provider, file ,protectionParameter, instance);
}

ECode CKeyStoreBuilderHelper::NewInstance(
    /* [in] */ const String& type,
    /* [in] */ IProvider* provider,
    /* [in] */ IKeyStoreProtectionParameter* protectionParameter,
    /* [out] */ IKeyStoreBuilder** instance)
{
    return KeyStoreBuilder::NewInstance(type, provider, protectionParameter, instance);
}

} // namespace Security
} // namespace Elastos