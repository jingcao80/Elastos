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

#include "elastos/droid/server/hdmi/UnmodifiableSparseInt32Array.h"
#include <Elastos.Droid.Utility.h>

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

const String UnmodifiableSparseInt32Array::TAG("ImmutableSparseInt32Array");

ECode UnmodifiableSparseInt32Array::constructor(
    /* [in] */ ISparseInt32Array* array)
{
    mArray = array;
    return NOERROR;
}

ECode UnmodifiableSparseInt32Array::GetSize(
    /* [out] */ Int32* result)
{
    return mArray->GetSize(result);
}

ECode UnmodifiableSparseInt32Array::Get(
    /* [in] */ Int32 key,
    /* [out] */ Int32* result)
{
    return mArray->Get(key, result);
}

ECode UnmodifiableSparseInt32Array::Get(
    /* [in] */ Int32 key,
    /* [in] */ Int32 valueIfKeyNotFound,
    /* [out] */ Int32* result)
{
    return mArray->Get(key, valueIfKeyNotFound, result);
}

ECode UnmodifiableSparseInt32Array::KeyAt(
    /* [in] */ Int32 index,
    /* [out] */ Int32* result)
{
    return mArray->KeyAt(index, result);
}

ECode UnmodifiableSparseInt32Array::ValueAt(
    /* [in] */ Int32 index,
    /* [out] */ Int32* result)
{
    return mArray->ValueAt(index, result);
}

ECode UnmodifiableSparseInt32Array::IndexOfValue(
    /* [in] */ Int32 value,
    /* [out] */ Int32* result)
{
    return mArray->IndexOfValue(value, result);
}

ECode UnmodifiableSparseInt32Array::ToString(
    /* [out] */ String* result)
{
    return IObject::Probe(mArray)->ToString(result);
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
