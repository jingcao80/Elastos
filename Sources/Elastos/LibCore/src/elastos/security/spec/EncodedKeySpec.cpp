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

#include "EncodedKeySpec.h"

namespace Elastos {
namespace Security {
namespace Spec {

CAR_INTERFACE_IMPL_2(EncodedKeySpec, Object, IEncodedKeySpec, IKeySpec)
EncodedKeySpec::EncodedKeySpec()
{}

EncodedKeySpec::EncodedKeySpec(
    /* [in] */ ArrayOf<Byte> *encodedKey)
{
    // Defensively copies parameter
    // to prevent subsequent modification
    mEncodedKey = ArrayOf<Byte>::Alloc(encodedKey->GetLength());
    mEncodedKey->Copy(0, encodedKey, 0, mEncodedKey->GetLength());
}

ECode EncodedKeySpec::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte> **encodedKey)
{
    // Defensively copies private array
    // to prevent subsequent modification
    VALIDATE_NOT_NULL(encodedKey)
    AutoPtr<ArrayOf<Byte> > ret = ArrayOf<Byte>::Alloc(mEncodedKey->GetLength());
    ret->Copy(0, mEncodedKey, 0, ret->GetLength());
    *encodedKey = ret;
    REFCOUNT_ADD(*encodedKey)
    return NOERROR;
}

ECode EncodedKeySpec::constructor(
    /* [in] */ ArrayOf<Byte> *encodedKey)
{
    // Defensively copies parameter
    // to prevent subsequent modification
    mEncodedKey = ArrayOf<Byte>::Alloc(encodedKey->GetLength());
    return mEncodedKey->Copy(0, encodedKey, 0, mEncodedKey->GetLength());
}

} // namespace Spec
} // namespace Security
} // namespace Elastos
