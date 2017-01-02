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

#include "CDESedeKeySpec.h"

namespace Elastosx {
namespace Crypto {
namespace Spec {

CAR_OBJECT_IMPL(CDESedeKeySpec)
CAR_INTERFACE_IMPL_2(CDESedeKeySpec, Object, IDESedeKeySpec, IKeySpec)

CDESedeKeySpec::CDESedeKeySpec()
{
}

ECode CDESedeKeySpec::constructor(
    /* [in] */ ArrayOf<Byte> * key)
{
    if (key == NULL) {
        // throw new NullPointerException("key == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (key->GetLength() < IDESedeKeySpec::DES_EDE_KEY_LEN) {
        // throw new InvalidKeyException();
        return E_INVALID_KEY_EXCEPTION;
    }
    mKey = ArrayOf<Byte>::Alloc(IDESedeKeySpec::DES_EDE_KEY_LEN);
    // System.arraycopy(key, 0, this.key, 0, IDESedeKeySpec::DES_EDE_KEY_LEN);
    mKey->Copy(0, key, 0, IDESedeKeySpec::DES_EDE_KEY_LEN);
    return NOERROR;
}

ECode CDESedeKeySpec::constructor(
    /* [in] */ ArrayOf<Byte> * key,
    /* [in] */ Int32 offset)
{
    if (key == NULL) {
        // throw new NullPointerException("key == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (key->GetLength() - offset < IDESedeKeySpec::DES_EDE_KEY_LEN) {
        // throw new InvalidKeyException("key too short");
        return E_INVALID_KEY_EXCEPTION;
    }
    mKey = ArrayOf<Byte>::Alloc(IDESedeKeySpec::DES_EDE_KEY_LEN);
    // System.arraycopy(key, offset, this.key, 0, IDESedeKeySpec::DES_EDE_KEY_LEN);
    mKey->Copy(0, key, offset, IDESedeKeySpec::DES_EDE_KEY_LEN);
    return NOERROR;
}

ECode CDESedeKeySpec::GetKey(
    /* [out, callee] */ ArrayOf<Byte> ** key)
{
    VALIDATE_NOT_NULL(key)
    *key = ArrayOf<Byte>::Alloc(IDESedeKeySpec::DES_EDE_KEY_LEN);
    // System.arraycopy(this.key, 0, result, 0, IDESedeKeySpec::DES_EDE_KEY_LEN);
    (*key)->Copy(0, mKey, 0, IDESedeKeySpec::DES_EDE_KEY_LEN);
    return NOERROR;
}

ECode CDESedeKeySpec::IsParityAdjusted(
    /* [in] */ ArrayOf<Byte> * key,
    /* [in] */ Int32 offset,
    /* [out] */ Boolean * result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (key->GetLength() - offset < IDESedeKeySpec::DES_EDE_KEY_LEN) {
        // throw new InvalidKeyException("key too short");
        return E_INVALID_KEY_EXCEPTION;
    }

    for (Int32 i = offset; i < IDESedeKeySpec::DES_EDE_KEY_LEN; i++) {
        Int32 b = (*key)[i];
        if ((((b & 1) + ((b & 2) >> 1) + ((b & 4) >> 2)
            + ((b & 8) >> 3) + ((b & 16) >> 4) + ((b & 32) >> 5)
            + ((b & 64) >> 6)) & 1) == ((b & 128) >> 7)) {
            *result = FALSE;
            return NOERROR;
        }
    }
    *result = TRUE;
    return NOERROR;
}

} // Spec
} // Crypto
} // Elastosx