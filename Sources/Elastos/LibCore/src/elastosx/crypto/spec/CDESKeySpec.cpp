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

#include "CDESKeySpec.h"

namespace Elastosx {
namespace Crypto {
namespace Spec {

CAR_OBJECT_IMPL(CDESKeySpec)
CAR_INTERFACE_IMPL_2(CDESKeySpec, Object, IDESKeySpec, IKeySpec)

const Byte CDESKeySpec::sSEMIWEAKS[16][8] = {
    {(Byte) 0xE0, (Byte) 0x01, (Byte) 0xE0, (Byte) 0x01,
     (Byte) 0xF1, (Byte) 0x01, (Byte) 0xF1, (Byte) 0x01},

    {(Byte) 0x01, (Byte) 0xE0, (Byte) 0x01, (Byte) 0xE0,
     (Byte) 0x01, (Byte) 0xF1, (Byte) 0x01, (Byte) 0xF1},

    {(Byte) 0xFE, (Byte) 0x1F, (Byte) 0xFE, (Byte) 0x1F,
     (Byte) 0xFE, (Byte) 0x0E, (Byte) 0xFE, (Byte) 0x0E},

    {(Byte) 0x1F, (Byte) 0xFE, (Byte) 0x1F, (Byte) 0xFE,
     (Byte) 0x0E, (Byte) 0xFE, (Byte) 0x0E, (Byte) 0xFE},

    {(Byte) 0xE0, (Byte) 0x1F, (Byte) 0xE0, (Byte) 0x1F,
     (Byte) 0xF1, (Byte) 0x0E, (Byte) 0xF1, (Byte) 0x0E},

    {(Byte) 0x1F, (Byte) 0xE0, (Byte) 0x1F, (Byte) 0xE0,
     (Byte) 0x0E, (Byte) 0xF1, (Byte) 0x0E, (Byte) 0xF1},

    {(Byte) 0x01, (Byte) 0xFE, (Byte) 0x01, (Byte) 0xFE,
     (Byte) 0x01, (Byte) 0xFE, (Byte) 0x01, (Byte) 0xFE},

    {(Byte) 0xFE, (Byte) 0x01, (Byte) 0xFE, (Byte) 0x01,
     (Byte) 0xFE, (Byte) 0x01, (Byte) 0xFE, (Byte) 0x01},

    {(Byte) 0x01, (Byte) 0x1F, (Byte) 0x01, (Byte) 0x1F,
     (Byte) 0x01, (Byte) 0x0E, (Byte) 0x01, (Byte) 0x0E},

    {(Byte) 0x1F, (Byte) 0x01, (Byte) 0x1F, (Byte) 0x01,
     (Byte) 0x0E, (Byte) 0x01, (Byte) 0x0E, (Byte) 0x01},

    {(Byte) 0xE0, (Byte) 0xFE, (Byte) 0xE0, (Byte) 0xFE,
     (Byte) 0xF1, (Byte) 0xFE, (Byte) 0xF1, (Byte) 0xFE},

    {(Byte) 0xFE, (Byte) 0xE0, (Byte) 0xFE, (Byte) 0xE0,
     (Byte) 0xFE, (Byte) 0xF1, (Byte) 0xFE, (Byte) 0xF1},

    {(Byte) 0x01, (Byte) 0x01, (Byte) 0x01, (Byte) 0x01,
     (Byte) 0x01, (Byte) 0x01, (Byte) 0x01, (Byte) 0x01},

    {(Byte) 0xFE, (Byte) 0xFE, (Byte) 0xFE, (Byte) 0xFE,
     (Byte) 0xFE, (Byte) 0xFE, (Byte) 0xFE, (Byte) 0xFE},

    {(Byte) 0xE0, (Byte) 0xE0, (Byte) 0xE0, (Byte) 0xE0,
     (Byte) 0xF1, (Byte) 0xF1, (Byte) 0xF1, (Byte) 0xF1},

    {(Byte) 0x1F, (Byte) 0x1F, (Byte) 0x1F, (Byte) 0x1F,
     (Byte) 0x0E, (Byte) 0x0E, (Byte) 0x0E, (Byte) 0x0E},
};

CDESKeySpec::CDESKeySpec()
{
}

ECode CDESKeySpec::constructor(
    /* [in] */ ArrayOf<Byte> * key)
{
    return constructor(key, 0);
}

ECode CDESKeySpec::constructor(
    /* [in] */ ArrayOf<Byte> * key,
    /* [in] */ Int32 offset)
{
    if (key == NULL) {
        // throw new NullPointerException("key == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (key->GetLength() - offset < IDESKeySpec::DES_KEY_LEN) {
        // throw new InvalidKeyException("key too short");
        return E_INVALID_KEY_EXCEPTION;
    }
    mKey = ArrayOf<Byte>::Alloc(IDESKeySpec::DES_KEY_LEN);
    // System.arraycopy(key, offset, this.key, 0, IDESKeySpec::DES_KEY_LEN);
    mKey->Copy(0, key, offset, IDESKeySpec::DES_KEY_LEN);
    return NOERROR;
}

ECode CDESKeySpec::GetKey(
    /* [out, callee] */ ArrayOf<Byte> ** key)
{
    VALIDATE_NOT_NULL(key)
    *key = ArrayOf<Byte>::Alloc(IDESKeySpec::DES_KEY_LEN);
    // System.arraycopy(this.key, 0, result, 0, IDESKeySpec::DES_KEY_LEN);
    (*key)->Copy(0, mKey, 0, IDESKeySpec::DES_KEY_LEN);
    return NOERROR;
}

ECode CDESKeySpec::IsParityAdjusted(
    /* [in] */ ArrayOf<Byte> * key,
    /* [in] */ Int32 offset,
    /* [out] */ Boolean * result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    if (key == NULL) {
        // throw new InvalidKeyException("key == NULL");
        return E_INVALID_KEY_EXCEPTION;
    }
    if (key->GetLength() - offset < IDESKeySpec::DES_KEY_LEN) {
        // throw new InvalidKeyException("key too short");
        return E_INVALID_KEY_EXCEPTION;
    }

    Int32 byteKey = 0;

    for (Int32 i = offset; i < IDESKeySpec::DES_KEY_LEN; i++) {
        byteKey = (*key)[i];

        byteKey ^= byteKey >> 1;
        byteKey ^= byteKey >> 2;
        byteKey ^= byteKey >> 4;

        if ((byteKey & 1) == 0) {
            *result = FALSE;
            return NOERROR;
        }
    }
    *result = TRUE;
    return NOERROR;
}

ECode CDESKeySpec::IsWeak(
    /* [in] */ ArrayOf<Byte> * key,
    /* [in] */ Int32 offset,
    /* [out] */ Boolean * result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    if (key == NULL) {
        // throw new InvalidKeyException("key == NULL");
        return E_INVALID_KEY_EXCEPTION;
    }
    if (key->GetLength() - offset < IDESKeySpec::DES_KEY_LEN) {
        // throw new InvalidKeyException("key too short");
        return E_INVALID_KEY_EXCEPTION;
    }

    Boolean b = FALSE;
    for (Int32 i = 0; i < 16 /*SEMIWEAKS.length*/; i++) {
        for (Int32 j = 0; j < IDESKeySpec::DES_KEY_LEN; j++) {
            if (sSEMIWEAKS[i][j] != (*key)[offset+j]) {
                b = TRUE;
                break;
            }
        }
        if(b) {
            b = FALSE;
            continue;
        }
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

} // Spec
} // Crypto
} // Elastosx