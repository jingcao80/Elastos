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
#include "elastos/droid/utility/CInt64Array.h"
#include "elastos/droid/internal/utility/ArrayUtils.h"
#include <libcore/utility/EmptyArray.h>

using Elastos::Droid::Internal::Utility::ArrayUtils;
using Libcore::Utility::EmptyArray;

namespace Elastos {
namespace Droid {
namespace Utility {

const Int32 CInt64Array::MIN_CAPACITY_INCREMENT = 12;

CAR_INTERFACE_IMPL_2(CInt64Array, Object, IInt64Array, ICloneable)

CAR_OBJECT_IMPL(CInt64Array)

CInt64Array::CInt64Array()
    : mSize(0)
{
}

ECode CInt64Array::constructor()
{
    return constructor(10);
}

ECode CInt64Array::constructor(
    /* [in] */ Int32 initialCapacity)
{
    if (initialCapacity == 0) {
        mValues = EmptyArray::INT64;
    }
    else {
        mValues = ArrayUtils::NewUnpaddedInt64Array(initialCapacity);
    }
    mSize = 0;
    return NOERROR;
}

ECode CInt64Array::Add(
    /* [in] */ Int64 value)
{
    return Add(mSize, value);
}

ECode CInt64Array::Add(
    /* [in] */ Int32 index,
    /* [in] */ Int64 value)
{
    if (index < 0 || index > mSize) {
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    EnsureCapacity(1);

    if (mSize - index != 0) {
        mValues->Copy(index + 1, mValues, index, mSize - index);
    }

    (*mValues)[index] = value;
    mSize++;
    return NOERROR;
}

ECode CInt64Array::AddAll(
    /* [in] */ IInt64Array* values)
{
    AutoPtr<CInt64Array> _values = (CInt64Array*)values;
    EnsureCapacity(_values->mSize);
    mValues->Copy(mSize, _values->mValues, 0, _values->mSize);
    mSize += _values->mSize;
    return NOERROR;
}

void CInt64Array::EnsureCapacity(
    /* [in] */ Int32 count)
{
    Int32 currentSize = mSize;
    Int32 minCapacity = currentSize + count;
    if (minCapacity >= mValues->GetLength()) {
        Int32 targetCap = currentSize + (currentSize < (MIN_CAPACITY_INCREMENT / 2) ?
                MIN_CAPACITY_INCREMENT : currentSize >> 1);
        Int32 newCapacity = targetCap > minCapacity ? targetCap : minCapacity;
        AutoPtr< ArrayOf<Int64> > newValues = ArrayUtils::NewUnpaddedInt64Array(newCapacity);
        newValues->Copy(0, mValues, 0, currentSize);
        mValues = newValues;
    }
}

ECode CInt64Array::Clear()
{
    mSize = 0;
    return NOERROR;
}

ECode CInt64Array::Clone(
    /* [out] */ IInterface** clone)
{
    VALIDATE_NOT_NULL(clone);
    *clone = NULL;

    AutoPtr<CInt64Array> cloneObj;
    CInt64Array::NewByFriend((CInt64Array**)&cloneObj);
    // try {
    cloneObj->mValues = mValues->Clone();
    cloneObj->mSize = mSize;
    // } catch (CloneNotSupportedException cnse) {
    //     /* ignore */
    // }
    *clone = (IInt64Array*)cloneObj.Get();
    REFCOUNT_ADD(*clone);
    return NOERROR;
}

ECode CInt64Array::Get(
    /* [in] */ Int32 index,
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value)
    *value = 0;

    if (index >= mSize) {
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    *value = (*mValues)[index];
    return NOERROR;
}

ECode CInt64Array::IndexOf(
    /* [in] */ Int64 value,
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index);

    for (Int32 i = 0; i < mSize; ++i) {
        if ((*mValues)[i] == value) {
            *index = i;
            return NOERROR;
        }
    }
    *index = -1;
    return NOERROR;
}

ECode CInt64Array::Remove(
    /* [in] */ Int32 index)
{
    if (index >= mSize) {
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    mValues->Copy(index, mValues, index + 1, mSize - index - 1);
    mSize--;
    return NOERROR;
}

ECode CInt64Array::GetSize(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = mSize;
    return NOERROR;
}

} // Utility
} // Droid
} // Elastos
