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

#include "CKeyStoreSecretKeyEntry.h"
#include "StringBuilder.h"

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Security {

CAR_OBJECT_IMPL(CKeyStoreSecretKeyEntry)
CAR_INTERFACE_IMPL_2(CKeyStoreSecretKeyEntry, Object, IKeyStoreSecretKeyEntry, IKeyStoreEntry)
ECode CKeyStoreSecretKeyEntry::GetSecretKey(
    /* [out] */ ISecretKey **key)
{
    VALIDATE_NOT_NULL(key)
    *key = mSecretKey;
    REFCOUNT_ADD(*key)
    return NOERROR;
}

ECode CKeyStoreSecretKeyEntry::ToString(
    /* [out] */ String *str)
{
    StringBuilder sb("SecretKeyEntry: algorithm - ");
    String algo;
    IKey::Probe(mSecretKey)->GetAlgorithm(&algo);
    sb.Append(algo);
    return sb.ToString(str);
}

ECode CKeyStoreSecretKeyEntry::constructor(
    /* [in] */ ISecretKey *secretKey)
{
    if (secretKey == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    mSecretKey = secretKey;
    return NOERROR;
}

}
}

