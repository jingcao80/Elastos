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

#include "elastos/droid/keystore/security/CKeyStoreHelper.h"

namespace Elastos {
namespace Droid {
namespace KeyStore {
namespace Security {

CAR_INTERFACE_IMPL(CKeyStoreHelper, Singleton, IKeyStoreHelper)
CAR_SINGLETON_IMPL(CKeyStoreHelper)

ECode CKeyStoreHelper::GetInstance(
    /* [out] */ IKeyStore** store)
{
    VALIDATE_NOT_NULL(store);
    AutoPtr<KeyStore> ks = KeyStore::GetInstance();
    *store = ks;
    REFCOUNT_ADD(*store);
    return NOERROR;
}

ECode CKeyStoreHelper::GetKeyTypeForAlgorithm(
    /* [in] */ const String& keyType,
    /* [out] */ Int32* algorithm)
{
    VALIDATE_NOT_NULL(algorithm);
    *algorithm = KeyStore::GetKeyTypeForAlgorithm(keyType);
    return NOERROR;
}

} // namespace Security
} // namespace KeyStore
} // namespace Droid
} // namespace Elastos
