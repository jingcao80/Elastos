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

#include "CKeyStoreHelper.h"
#include "KeyStore.h"

namespace Elastos {
namespace Security {

CAR_SINGLETON_IMPL(CKeyStoreHelper);
CAR_INTERFACE_IMPL(CKeyStoreHelper, Singleton, IKeyStoreHelper)
ECode CKeyStoreHelper::GetInstance(
    /* [in] */ const String& type,
    /* [out] */ IKeyStore **instance)
{
    return KeyStore::GetInstance(type, instance);
}

ECode CKeyStoreHelper::GetInstance(
    /* [in] */ const String& type,
    /* [in] */ const String& provider,
    /* [out] */ IKeyStore **instance)
{
    return KeyStore::GetInstance(type, provider, instance);
}

ECode CKeyStoreHelper::GetInstance(
    /* [in] */ const String& type,
    /* [in] */ IProvider *provider,
    /* [out] */ IKeyStore **instance)
{
    return KeyStore::GetInstance(type, provider, instance);
}

ECode CKeyStoreHelper::GetDefaultType(
    /* [out] */ String *defaultType)
{
    return KeyStore::GetDefaultType(defaultType);
}

} // namespace Security
} // namespace Elastos