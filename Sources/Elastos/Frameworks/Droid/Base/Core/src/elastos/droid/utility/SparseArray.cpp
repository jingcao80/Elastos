
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/utility/SparseArray.h"
#include "elastos/droid/utility/CSparseArray.h"
#include "elastos/droid/utility/ContainerHelpers.h"
#include "elastos/droid/internal/utility/ArrayUtils.h"
#include "elastos/droid/internal/utility/GrowingArrayUtils.h"
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>
#include <libcore/utility/EmptyArray.h>

using Elastos::Droid::Internal::Utility::ArrayUtils;
using Elastos::Droid::Internal::Utility::GrowingArrayUtils;
using Elastos::Core::Math;
using Elastos::Core::EIID_ICloneable;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;
using Libcore::Utility::EmptyArray;

namespace Elastos {
namespace Droid {
namespace Utility {

const AutoPtr<Object> SparseArray::DELETED = new Object();

CAR_INTERFACE_IMPL_2(SparseArray, Object, ISparseArray, ICloneable);

SparseArray::SparseArray()
    : mGarbage(FALSE)
    , mSize(0)
{}

ECode SparseArray::constructor()
{
    return constructor(10);
}

ECode SparseArray::constructor(
    /* [in] */ Int32 initialCapacity)
{
    mGarbage = FALSE;
    if (initialCapacity == 0) {
        mKeys = EmptyArray::INT32;
        mValues = EmptyArray::OBJECT;
    }
    else {
        mValues = ArrayUtils::NewUnpaddedObjectArray(initialCapacity);
        mKeys = ArrayOf<Int32>::Alloc(mValues->GetLength());
    }
    mSize = 0;
    return NOERROR;
}

ECode SparseArray::Clone(
    /* [out] */ IInterface** clone)
{
    VALIDATE_NOT_NULL(clone);
    *clone = NULL;

    AutoPtr<CSparseArray> cloneObj;
    CSparseArray::NewByFriend((CSparseArray**)&cloneObj);
    // try {
    cloneObj->mKeys = mKeys->Clone();
    cloneObj->mValues = mValues->Clone();
    cloneObj->mSize = mSize;
    // } catch (CloneNotSupportedException cnse) {
    //     /* ignore */
    // }
    *clone = (ISparseArray*)cloneObj.Get();
    REFCOUNT_ADD(*clone);
    return NOERROR;
}

ECode SparseArray::Get(
    /* [in] */ Int32 key,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);
    return Get(key, NULL, outface);
}

ECode SparseArray::Get(
    /* [in] */ Int32 key,
    /* [in] */ IInterface* valueIfKeyNotFound,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);
    Int32 i = ContainerHelpers::BinarySearch(mKeys, mSize, key);

    if (i < 0 || (*mValues)[i] == (IObject*)DELETED) {
        *outface = valueIfKeyNotFound;
        REFCOUNT_ADD(*outface);
        return NOERROR;
    }
    else {
        *outface = (*mValues)[i];
        REFCOUNT_ADD(*outface);
        return NOERROR;
    }
}

ECode SparseArray::Delete(
    /* [in] */ Int32 key)
{
    Int32 i = ContainerHelpers::BinarySearch(mKeys, mSize, key);

    if (i >= 0) {
        if ((*mValues)[i] != (IObject*)DELETED) {
            mValues->Set(i, (IObject*)DELETED);
            mGarbage = TRUE;
        }
    }
    return NOERROR;
}

ECode SparseArray::Remove(
    /* [in] */ Int32 key)
{
    return Delete(key);
}

ECode SparseArray::RemoveAt(
    /* [in] */ Int32 index)
{
    if ((*mValues)[index] != (IObject*)DELETED) {
        mValues->Set(index, (IObject*)DELETED);
        mGarbage = TRUE;
    }
    return NOERROR;
}

ECode SparseArray::RemoveAtRange(
    /* [in] */ Int32 index,
    /* [in] */ Int32 size)
{
    for (Int32 i = 0; i < Min(mSize, index + size); i++) {
        RemoveAt(i);
    }
    return NOERROR;
}

void SparseArray::Gc()
{
    Int32 o = 0;
    AutoPtr< ArrayOf<Int32> > keys = mKeys;
    AutoPtr< ArrayOf<IInterface*> > values = mValues;

    for (Int32 i = 0; i < mSize; i++) {
        AutoPtr<IInterface> val = (*values)[i];

        if (val != (IObject*)DELETED) {
            if (i != o) {
                (*keys)[o] = (*keys)[i];
                values->Set(o, val);
                values->Set(i, NULL);
            }

            o++;
        }
    }

    mGarbage = FALSE;
    mSize = o;
}

ECode SparseArray::Put(
    /* [in] */ Int32 key,
    /* [in] */ IInterface* value)
{
    Int32 i = ContainerHelpers::BinarySearch(mKeys, mSize, key);

    if (i >= 0) {
        mValues->Set(i, value);
    }
    else {
        i = ~i;

        if (i < mSize && (*mValues)[i] == (IObject*)DELETED) {
            mKeys->Set(i, key);
            mValues->Set(i, value);
            return NOERROR;
        }

        if (mGarbage && mSize >= mKeys->GetLength()) {
            Gc();

            // Search again because indices may have changed.
            i = ~ContainerHelpers::BinarySearch(mKeys, mSize, key);
        }
        mKeys = GrowingArrayUtils::Insert(mKeys, mSize, i, key);
        mValues = GrowingArrayUtils::Insert(mValues, mSize, i, value);

        mSize++;
    }
    return NOERROR;
}

ECode SparseArray::GetSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size);
    if (mGarbage) {
        Gc();
    }

    *size = mSize;
    return NOERROR;
}

ECode SparseArray::KeyAt(
    /* [in] */ Int32 index,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    if (mGarbage) {
        Gc();
    }

    *value = (*mKeys)[index];
    return NOERROR;
}

ECode SparseArray::ValueAt(
    /* [in] */ Int32 index,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);
    if (mGarbage) {
        Gc();
    }

    *outface = (*mValues)[index];
    REFCOUNT_ADD(*outface);
    return NOERROR;
}

ECode SparseArray::SetValueAt(
    /* [in] */ Int32 index,
    /* [in] */ IInterface* value)
{
    if (mGarbage) {
        Gc();
    }

    mValues->Set(index, value);
    return NOERROR;
}

ECode SparseArray::IndexOfKey(
    /* [in] */ Int32 key,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    if (mGarbage) {
        Gc();
    }

    *value = ContainerHelpers::BinarySearch(mKeys, mSize, key);
    return NOERROR;
}

ECode SparseArray::IndexOfValue(
    /* [in] */ IInterface* value,
    /* [out] */ Int32* outval)
{
    VALIDATE_NOT_NULL(outval);
    if (mGarbage) {
        Gc();
    }

    for (Int32 i = 0; i < mSize; i++) {
        if ((*mValues)[i] == value) {
            *outval = i;
            return NOERROR;
        }
    }

    *outval = -1;
    return NOERROR;
}

ECode SparseArray::Clear()
{
    AutoPtr< ArrayOf<IInterface*> > values = mValues;

    for (Int32 i = 0; i < mSize; i++) {
        values->Set(i, NULL);
    }

    mSize = 0;
    mGarbage = FALSE;
    return NOERROR;
}

ECode SparseArray::Append(
    /* [in] */ Int32 key,
    /* [in] */ IInterface* value)
{
    if (mSize != 0 && key <= (*mKeys)[mSize - 1]) {
        Put(key, value);
        return NOERROR;
    }

    if (mGarbage && mSize >= mKeys->GetLength()) {
        Gc();
    }

    mKeys = GrowingArrayUtils::Append(mKeys, mSize, key);
    mValues = GrowingArrayUtils::Append(mValues, mSize, value);
    mSize++;

    return NOERROR;
}

ECode SparseArray::ToString(
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
        AutoPtr<IInterface> value;
        ValueAt(i, (IInterface**)&value);
        if (TO_IINTERFACE(value) != TO_IINTERFACE(this)) {
            buffer.Append(value);
        }
        else {
            buffer.Append("(this Map)");
        }
    }
    buffer.AppendChar('}');
    *str = buffer.ToString();
    return NOERROR;
}

}
}
}
