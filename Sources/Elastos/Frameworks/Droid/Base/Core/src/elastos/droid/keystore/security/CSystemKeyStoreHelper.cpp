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

#include "elastos/droid/keystore/security/CSystemKeyStoreHelper.h"

namespace Elastos {
namespace Droid {
namespace KeyStore {
namespace Security {

CAR_INTERFACE_IMPL(CSystemKeyStoreHelper, Singleton, ISystemKeyStoreHelper)
CAR_SINGLETON_IMPL(CSystemKeyStoreHelper)

ECode CSystemKeyStoreHelper::GetInstance(
    /* [out] */ ISystemKeyStore** store)
{
    VALIDATE_NOT_NULL(store);
    AutoPtr<ISystemKeyStore> ss = SystemKeyStore::GetInstance();
    *store = ss;
    REFCOUNT_ADD(*store);
    return NOERROR;
}

ECode CSystemKeyStoreHelper::ToHexString(
    /* [in] */ ArrayOf<Byte>* keyData,
    /* [out] */ String* hex)
{
    VALIDATE_NOT_NULL(hex);
    *hex = SystemKeyStore::ToHexString(keyData);
    return NOERROR;
}

} // namespace Security
} // namespace KeyStore
} // namespace Droid
} // namespace Elastos
