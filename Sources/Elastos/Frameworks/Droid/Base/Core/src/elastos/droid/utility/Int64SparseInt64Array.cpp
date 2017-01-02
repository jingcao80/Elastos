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

#include "elastos/droid/utility/Int64SparseInt64Array.h"
#include "elastos/droid/utility/ContainerHelpers.h"
#include "elastos/droid/internal/utility/ArrayUtils.h"
#include "elastos/droid/internal/utility/GrowingArrayUtils.h"
#include "libcore/utility/EmptyArray.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::Internal::Utility::ArrayUtils;
using Elastos::Droid::Internal::Utility::GrowingArrayUtils;
using Elastos::Core::EIID_ICloneable;
using Elastos::Core::StringBuilder;
using Libcore::Utility::EmptyArray;

namespace Elastos {
namespace Droid {
namespace Utility {

CAR_INTERFACE_IMPL(Int64SparseInt64Array, Object, ICloneable)

Int64SparseInt64Array::Int64SparseInt64Array()
    : mSize(0)
{
    Init(10);
}

Int64SparseInt64Array::Int64SparseInt64Array(
    /* [in] */ Int32 initialCapacity)
    : mSize(0)
{
    Init(initialCapacity);
}

void Int64SparseInt64Array::Init(
    /* [in] */ Int32 initialCapacity)
{
    if (initialCapacity == 0) {
        mKeys = EmptyArray::INT64;
        mValues = EmptyArray::INT64;
    }
    else {
        mKeys = ArrayUtils::NewUnpaddedInt64Array(initialCapacity);
        mValues = ArrayOf<Int64>::Alloc(mKeys->GetLength());
    }
    mSize = 0;
}

ECode Int64SparseInt64Array::Clone(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<Int64SparseInt64Array> clone = new Int64SparseInt64Array(0);
    clone->mSize = mSize;
    clone->mKeys = mKeys->Clone();
    clone->mValues = mValues->Clone();

    *result = (IObject*)clone.Get();
    REFCOUNT_ADD(*result)
    return NOERROR;
}

Int64 Int64SparseInt64Array::Get(
    /* [in] */ Int64 key)
{
    return Get(key, 0);
}

Int64 Int64SparseInt64Array::Get(
    /* [in] */ Int64 key,
    /* [in] */ Int64 valueIfKeyNotFound)
{
    Int32 i = ContainerHelpers::BinarySearch(mKeys, mSize, key);

    if (i < 0) {
        return valueIfKeyNotFound;
    }
    else {
        return (*mValues)[i];
    }
}

ECode Int64SparseInt64Array::Delete(
    /* [in] */ Int64 key)
{
    Int32 i = ContainerHelpers::BinarySearch(mKeys, mSize, key);
    if (i >= 0) {
        RemoveAt(i);
    }
    return NOERROR;
}

ECode Int64SparseInt64Array::RemoveAt(
    /* [in] */ Int32 index)
{
    mKeys->Copy(index, mKeys, index + 1, mSize - (index + 1));
    mValues->Copy(index, mValues, index + 1, mSize - (index + 1));
    mSize--;
    return NOERROR;
}

ECode Int64SparseInt64Array::Put(
    /* [in] */ Int64 key,
    /* [in] */ Int64 value)
{
    Int32 i = ContainerHelpers::BinarySearch(mKeys, mSize, key);

    if (i >= 0) {
        (*mValues)[i] = value;
    }
    else {
        i = ~i;
        mKeys = GrowingArrayUtils::Insert(mKeys, mSize, i, key);
        mValues = GrowingArrayUtils::Insert(mValues, mSize, i, value);
        mSize++;
    }
    return NOERROR;
}

Int32 Int64SparseInt64Array::Size()
{
    return mSize;
}

Int64 Int64SparseInt64Array::KeyAt(
    /* [in] */ Int32 index)
{
    return (*mKeys)[index];
}

Int64 Int64SparseInt64Array::ValueAt(
    /* [in] */ Int32 index)
{
    return (*mValues)[index];
}

Int32 Int64SparseInt64Array::IndexOfKey(
    /* [in] */ Int64 key)
{
    return ContainerHelpers::BinarySearch(mKeys, mSize, key);
}

Int32 Int64SparseInt64Array::IndexOfValue(
    /* [in] */ Int64 value)
{
    for (int i = 0; i < mSize; i++)
        if ((*mValues)[i] == value)
            return i;

    return -1;
}

ECode Int64SparseInt64Array::Clear()
{
    mSize = 0;
    return NOERROR;
}

ECode Int64SparseInt64Array::Append(
    /* [in] */ Int64 key,
    /* [in] */ Int64 value)
{
    if (mSize != 0 && key <= (*mKeys)[mSize - 1]) {
        return Put(key, value);
    }

    mKeys = GrowingArrayUtils::Append(mKeys, mSize, key);
    mValues = GrowingArrayUtils::Append(mValues, mSize, value);
    mSize++;
    return NOERROR;
}

ECode Int64SparseInt64Array::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    if (Size() <= 0) {
        *str = String("{}");
        return NOERROR;
    }

    StringBuilder buffer(mSize * 28);
    buffer.Append('{');
    for (Int32 i=0; i<mSize; i++) {
        if (i > 0) {
            buffer.Append(", ");
        }
        Int64 key = KeyAt(i);
        buffer.Append(key);
        buffer.Append('=');
        Int64 value = ValueAt(i);
        buffer.Append(value);
    }
    buffer.Append('}');
    *str = buffer.ToString();
    return NOERROR;
}


}
}
}