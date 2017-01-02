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

#include "elastos/droid/server/hdmi/UnmodifiableSparseArray.h"
#include <Elastos.Droid.Utility.h>

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

const String UnmodifiableSparseArray::TAG("ImmutableSparseArray");

ECode UnmodifiableSparseArray::constructor(
    /* [in] */ ISparseArray* array)
{
    mArray = array;
    return NOERROR;
}

ECode UnmodifiableSparseArray::GetSize(
    /* [out] */ Int32* result)
{
    return mArray->GetSize(result);
}

ECode UnmodifiableSparseArray::Get(
    /* [in] */ Int32 key,
    /* [out] */ IInterface** result)
{
    return mArray->Get(key, result);
}

ECode UnmodifiableSparseArray::Get(
    /* [in] */ Int32 key,
    /* [in] */ IInterface* valueIfKeyNotFound,
    /* [out] */ IInterface** result)
{
    return mArray->Get(key, valueIfKeyNotFound, result);
}

ECode UnmodifiableSparseArray::KeyAt(
    /* [in] */ Int32 index,
    /* [out] */ Int32* result)
{
    return mArray->KeyAt(index, result);
}

ECode UnmodifiableSparseArray::ValueAt(
    /* [in] */ Int32 index,
    /* [out] */ IInterface** result)
{
    return mArray->ValueAt(index, result);
}

ECode UnmodifiableSparseArray::IndexOfValue(
    /* [in] */ IInterface* value,
    /* [out] */ Int32* result)
{
    return mArray->IndexOfValue(value, result);
}

ECode UnmodifiableSparseArray::ToString(
    /* [out] */ String* result)
{
    return IObject::Probe(mArray)->ToString(result);
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
