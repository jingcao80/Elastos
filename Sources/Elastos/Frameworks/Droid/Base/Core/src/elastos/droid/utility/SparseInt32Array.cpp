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

#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/utility/SparseInt32Array.h"
#include "elastos/droid/utility/CSparseInt32Array.h"
#include "elastos/droid/utility/ContainerHelpers.h"
#include "elastos/droid/internal/utility/ArrayUtils.h"
#include "elastos/droid/internal/utility/GrowingArrayUtils.h"
#include <elastos/core/StringBuilder.h>
#include <libcore/utility/EmptyArray.h>

using Elastos::Droid::Internal::Utility::ArrayUtils;
using Elastos::Droid::Internal::Utility::GrowingArrayUtils;
using Elastos::Core::EIID_ICloneable;
using Elastos::Core::StringBuilder;
using Libcore::Utility::EmptyArray;

namespace Elastos {
namespace Droid {
namespace Utility {

CAR_INTERFACE_IMPL_2(SparseInt32Array, Object, ISparseInt32Array, ICloneable);

SparseInt32Array::SparseInt32Array()
    : mSize(0)
{}

ECode SparseInt32Array::constructor()
{
    return constructor(10);
}

ECode SparseInt32Array::constructor(
    /* [in] */ Int32 initialCapacity)
{
    if (initialCapacity == 0) {
        mKeys = EmptyArray::INT32;
        mValues = EmptyArray::INT32;
    }
    else {
        mKeys = ArrayUtils::NewUnpaddedInt32Array(initialCapacity);
        mValues = ArrayOf<Int32>::Alloc(mKeys->GetLength());
    }
    mSize = 0;
    return NOERROR;
}

ECode SparseInt32Array::Clone(
    /* [out] */ IInterface** clone)
{
    VALIDATE_NOT_NULL(clone);
    *clone = NULL;

    AutoPtr<CSparseInt32Array> cloneObj;
    CSparseInt32Array::NewByFriend((CSparseInt32Array**)&cloneObj);
    // try {
    cloneObj->mKeys = mKeys->Clone();
    cloneObj->mValues = mValues->Clone();
    cloneObj->mSize = mSize;
    // } catch (CloneNotSupportedException cnse) {
    //     /* ignore */
    // }
    *clone = (ISparseInt32Array*)cloneObj.Get();
    REFCOUNT_ADD(*clone);
    return NOERROR;
}

ECode SparseInt32Array::Get(
    /* [in] */ Int32 key,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    return Get(key, 0, value);
}

ECode SparseInt32Array::Get(
    /* [in] */ Int32 key,
    /* [in] */ Int32 valueIfKeyNotFound,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    Int32 i = ContainerHelpers::BinarySearch(mKeys, mSize, key);

    if (i < 0 ) {
        *value = valueIfKeyNotFound;
        return NOERROR;
    }
    else {
        *value = (*mValues)[i];
        return NOERROR;
    }
}

ECode SparseInt32Array::Delete(
    /* [in] */ Int32 key)
{
    Int32 i = ContainerHelpers::BinarySearch(mKeys, mSize, key);

    if (i >= 0) {
        RemoveAt(i);
    }
    return NOERROR;
}

ECode SparseInt32Array::RemoveAt(
    /* [in] */ Int32 index)
{
    mKeys->Copy(index, mKeys, index + 1, mSize - (index + 1));
    mValues->Copy(index, mValues, index + 1, mSize - (index + 1));
    mSize--;
    return NOERROR;
}

ECode SparseInt32Array::Put(
    /* [in] */ Int32 key,
    /* [in] */ Int32 value)
{
    Int32 i = ContainerHelpers::BinarySearch(mKeys, mSize, key);

    if (i >= 0) {
        mValues->Set(i, value);
    }
    else {
        i = ~i;

        mKeys = GrowingArrayUtils::Insert(mKeys, mSize, i, key);
        mValues = GrowingArrayUtils::Insert(mValues, mSize, i, value);
        mSize++;
    }
    return NOERROR;
}

ECode SparseInt32Array::GetSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size);
    *size = mSize;
    return NOERROR;
}

ECode SparseInt32Array::KeyAt(
    /* [in] */ Int32 index,
    /* [out] */ Int32* key)
{
    VALIDATE_NOT_NULL(key);
    *key = (*mKeys)[index];
    return NOERROR;
}

ECode SparseInt32Array::ValueAt(
    /* [in] */ Int32 index,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = (*mValues)[index];
    return NOERROR;
}

ECode SparseInt32Array::IndexOfKey(
    /* [in] */ Int32 key,
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index);

    *index = ContainerHelpers::BinarySearch(mKeys, mSize, key);
    return NOERROR;
}

ECode SparseInt32Array::IndexOfValue(
    /* [in] */ Int32 value,
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index);

    for (Int32 i = 0; i < mSize; i++) {
        if ((*mValues)[i] == value) {
            *index = i;
            return NOERROR;
        }
    }

    *index = -1;
    return NOERROR;
}

ECode SparseInt32Array::Clear()
{
    mSize = 0;
    return NOERROR;
}

ECode SparseInt32Array::Append(
    /* [in] */ Int32 key,
    /* [in] */ Int32 value)
{
    if (mSize != 0 && key <= (*mKeys)[mSize - 1]) {
        Put(key, value);
        return NOERROR;
    }

    mKeys = GrowingArrayUtils::Append(mKeys, mSize, key);
    mValues = GrowingArrayUtils::Append(mValues, mSize, value);
    mSize++;

    return NOERROR;
}

ECode SparseInt32Array::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    if (mSize <= 0) {
        *str = "{}";
        return NOERROR;
    }

    StringBuilder buffer(mSize * 28);
    buffer.AppendChar('{');
    for (Int32 i = 0; i < mSize; i++) {
        if (i > 0) {
            buffer.Append(", ");
        }
        Int32 key;
        KeyAt(i, &key);
        buffer.Append(key);
        buffer.AppendChar('=');
        Int32 value;
        ValueAt(i, &value);
        buffer.Append(value);
    }
    buffer.AppendChar('}');
    *str = buffer.ToString();
    return NOERROR;
}

} // namespace Utility
} // namespace Droid
} // namespace Elastos
