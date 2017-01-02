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

#include "Elastos.CoreLibrary.IO.h"
#include "ArrayList.h"
#include "CArrayList.h"
#include "CArrayListIterator.h"
#include "StringBuilder.h"

using Elastos::Core::EIID_ICloneable;
using Elastos::Core::StringBuilder;
using Elastos::IO::IObjectOutputStreamPutField;
using Elastos::IO::EIID_ISerializable;
using Elastos::IO::IOutputStream;
using Elastos::IO::IInputStream;

namespace Elastos {
namespace Utility {


//====================================================================
// ArrayList::ArrayListIterator
//====================================================================

CAR_INTERFACE_IMPL_4(ArrayList, AbstractList, IArrayList, ICloneable, ISerializable, IRandomAccess)

CAR_INTERFACE_IMPL(ArrayList::ArrayListIterator, Object, IIterator)

ArrayList::ArrayListIterator::ArrayListIterator()
    : mRemaining(0)
    , mRemovalIndex(-1)
    , mExpectedModCount(0)
{}

ECode ArrayList::ArrayListIterator::constructor(
    /* [in] */ IArrayList* owner)
{
    mOwner = (ArrayList*)owner;
    mRemaining = mOwner->mSize;
    mExpectedModCount = mOwner->mModCount;
    return NOERROR;
}

ECode ArrayList::ArrayListIterator::HasNext(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mRemaining != 0;
    return NOERROR;
}

ECode ArrayList::ArrayListIterator::GetNext(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)
    *object = NULL;

    Int32 rem = mRemaining;
    if (mOwner->mModCount != mExpectedModCount) {
        return E_CONCURRENT_MODIFICATION_EXCEPTION;
    }
    if (rem == 0) {
        return E_NO_SUCH_ELEMENT_EXCEPTION;
    }
    mRemaining = rem - 1;
    *object = (*mOwner->mArray)[mRemovalIndex = mOwner->mSize - rem];
    REFCOUNT_ADD(*object)
    return NOERROR;
}

ECode ArrayList::ArrayListIterator::Remove()
{
    Int32 removalIdx = mRemovalIndex;
    if (mOwner->mModCount != mExpectedModCount) {
        return E_CONCURRENT_MODIFICATION_EXCEPTION;
    }
    if (removalIdx < 0) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    mOwner->mArray->Copy(removalIdx + 1, mOwner->mArray, removalIdx, mRemaining);
    mOwner->mArray->Set(--mOwner->mSize, NULL);
    mRemovalIndex = -1;
    mExpectedModCount = ++mOwner->mModCount;
    return NOERROR;
}


//====================================================================
// ArrayList::
//====================================================================
const Int32 ArrayList::MIN_CAPACITY_INCREMENT = 12;
// const Int64 ArrayList::mSerialVersionUID = 8683452581122892189L;

ArrayList::ArrayList()
    : mSize(0)
{}

ECode ArrayList::constructor()
{
    mArray = ArrayOf<IInterface*>::Alloc(0); // EmptyArray.OBJECT;
    return NOERROR;
}

ECode ArrayList::constructor(
    /* [in] */ Int32 capacity)
{
    if (capacity < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mArray = ArrayOf<IInterface*>::Alloc(capacity); // (capacity == 0 ? EmptyArray.OBJECT : new Object[capacity]);
    return NOERROR;
}

ECode ArrayList::constructor(
    /* [in] */ ICollection* collection)
{
    AutoPtr< ArrayOf<IInterface*> > a;
    collection->ToArray((ArrayOf<IInterface*>**)&a);
    // assert(0 && "TODO");
    // if (a.getClass() != Object[].class) {
    //     Object[] newArray = new Object[a.length];
    //     System.arraycopy(a, 0, newArray, 0, a.length);
    //     a = newArray;
    // }
    mArray = a;
    mSize = a->GetLength();
    return NOERROR;
}

ECode ArrayList::Add(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    VALIDATE_NOT_NULL(modified)

    AutoPtr< ArrayOf<IInterface*> > a = mArray;
    Int32 s = mSize;
    if (s == a->GetLength()) {
        AutoPtr< ArrayOf<IInterface*> > newArray = ArrayOf<IInterface*>::Alloc(s +
                (s < (MIN_CAPACITY_INCREMENT / 2) ? MIN_CAPACITY_INCREMENT : s >> 1));
        newArray->Copy(a, s);
        mArray = a = newArray;
    }
    a->Set(s, object);
    mSize = s + 1;
    mModCount++;
    *modified = TRUE;
    return NOERROR;
}

ECode ArrayList::Add(
    /* [in] */ Int32 location,
    /* [in] */ IInterface* object)
{
    AutoPtr< ArrayOf<IInterface*> > a = mArray;
    Int32 s = mSize;
    if (location > s || location < 0) {
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    if (s < a->GetLength()) {
        a->Copy(location + 1, a, location, s - location);
    }
    else {
        // assert s == a.length;
        AutoPtr< ArrayOf<IInterface*> > newArray = ArrayOf<IInterface*>::Alloc(NewCapacity(s));
        newArray->Copy(a, location);
        newArray->Copy(location + 1, a, location, s - location);
        mArray = a = newArray;
    }
    a->Set(location, object);
    mSize = s + 1;
    mModCount++;
    return NOERROR;
}

Int32 ArrayList::NewCapacity(
    /* [in] */ Int32 currentCapacity)
{
    Int32 increment = (currentCapacity < (MIN_CAPACITY_INCREMENT / 2) ?
            MIN_CAPACITY_INCREMENT : currentCapacity >> 1);
    return currentCapacity + increment;
}

ECode ArrayList::AddAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    VALIDATE_NOT_NULL(modified)

    AutoPtr< ArrayOf<IInterface*> > newPart;
    collection->ToArray((ArrayOf<IInterface*>**)&newPart);
    Int32 newPartSize = newPart->GetLength();
    if (newPartSize == 0) {
        *modified = FALSE;
        return NOERROR;
    }
    AutoPtr< ArrayOf<IInterface*> > a = mArray;
    Int32 s = mSize;
    Int32 newSize = s + newPartSize; // If add overflows, arraycopy will fail
    if (newSize > a->GetLength()) {
        Int32 newCapacity = NewCapacity(newSize - 1);  // ~33% growth room
        AutoPtr< ArrayOf<IInterface*> > newArray = ArrayOf<IInterface*>::Alloc(newCapacity);
        newArray->Copy(a, s);
        mArray = a = newArray;
    }
    a->Copy(s, newPart, 0, newPartSize);
    mSize = newSize;
    mModCount++;
    *modified = TRUE;
    return NOERROR;
}

ECode ArrayList::AddAll(
    /* [in] */ Int32 location,
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    VALIDATE_NOT_NULL(modified)

    Int32 s = mSize;
    if (location > s || location < 0) {
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    AutoPtr< ArrayOf<IInterface*> > newPart;
    collection->ToArray((ArrayOf<IInterface*>**)&newPart);
    Int32 newPartSize = newPart->GetLength();
    if (newPartSize == 0) {
        *modified = FALSE;
        return NOERROR;
    }
    AutoPtr< ArrayOf<IInterface*> > a = mArray;
    Int32 newSize = s + newPartSize; // If add overflows, arraycopy will fail
    if (newSize <= a->GetLength()) {
        a->Copy(location + newPartSize, a, location, s - location);
    }
    else {
        Int32 newCapacity = NewCapacity(newSize - 1);  // ~33% growth room
        AutoPtr< ArrayOf<IInterface*> > newArray = ArrayOf<IInterface*>::Alloc(newCapacity);
        newArray->Copy(a, location);
        newArray->Copy(location + newPartSize, a, location, s - location);
        mArray = a = newArray;
    }
    a->Copy(location, newPart, 0, newPartSize);
    mSize = newSize;
    mModCount++;
    *modified = TRUE;
    return NOERROR;
}

ECode ArrayList::Clear()
{
    if (mSize != 0) {
        for (Int32 i = 0; i < mSize; ++i) {
            mArray->Set(i, NULL);
        }
        mSize = 0;
        mModCount++;
    }
    return NOERROR;
}

ECode ArrayList::Clone(
    /* [out] */ IInterface** outarray)
{
    VALIDATE_NOT_NULL(outarray)

    // try {
    AutoPtr<CArrayList> result;
    CArrayList::NewByFriend((CArrayList**)&result);
    result->mArray = mArray->Clone();
    result->mSize = mSize;
    *outarray = (IArrayList*)result.Get();
    REFCOUNT_ADD(*outarray)
    // } catch (CloneNotSupportedException e) {
    //    throw new AssertionError();
    // }
    return NOERROR;
}

ECode ArrayList::EnsureCapacity(
    /* [in] */ Int32 minimumCapacity)
{
    AutoPtr< ArrayOf<IInterface*> > a = mArray;
    if (a->GetLength() < minimumCapacity) {
        AutoPtr< ArrayOf<IInterface*> > newArray = ArrayOf<IInterface*>::Alloc(minimumCapacity);
        newArray->Copy(a, mSize);
        mArray = newArray;
        mModCount++;
    }
    return NOERROR;
}

ECode ArrayList::Get(
    /* [in] */ Int32 location,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)

    if (location >= mSize || location < 0) {
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    *object = (*mArray)[location];
    REFCOUNT_ADD(*object)
    return NOERROR;
}

ECode ArrayList::GetSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size)
    *size = mSize;
    return NOERROR;
}

ECode ArrayList::IsEmpty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mSize == 0;
    return NOERROR;
}

ECode ArrayList::Contains(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr< ArrayOf<IInterface*> > a = mArray;
    Int32 s = mSize;
    if (object != NULL) {
        for (Int32 i = 0; i < s; i++) {
            if (Object::Equals(object, (*a)[i])) {
                *result = TRUE;
                return NOERROR;
            }
        }
    }
    else {
        for (Int32 i = 0; i < s; i++) {
            if ((*a)[i] == NULL) {
                *result = TRUE;
                return NOERROR;
            }
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode ArrayList::IndexOf(
    /* [in] */ IInterface* object,
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index)

    AutoPtr< ArrayOf<IInterface*> > a = mArray;
    Int32 s = mSize;
    if (object != NULL) {
        for (Int32 i = 0; i < s; i++) {
            if (Object::Equals(object, (*a)[i])) {
                *index = i;
                return NOERROR;
            }
        }
    }
    else {
        for (Int32 i = 0; i < s; i++) {
            if ((*a)[i] == NULL) {
                *index = i;
                return NOERROR;
            }
        }
    }
    *index = -1;
    return NOERROR;
}

ECode ArrayList::LastIndexOf(
    /* [in] */ IInterface* object,
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index)

    AutoPtr< ArrayOf<IInterface*> > a = mArray;
    if (object != NULL) {
        for (Int32 i = mSize - 1; i >= 0; i--) {
            if (Object::Equals(object, (*a)[i])) {
                *index = i;
                return NOERROR;
            }
        }
    }
    else {
        for (Int32 i = mSize - 1; i >= 0; i--) {
            if ((*a)[i] == NULL) {
                *index = i;
                return NOERROR;
            }
        }
    }
    *index = -1;
    return NOERROR;
}

ECode ArrayList::Remove(
    /* [in] */ Int32 location,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)

    AutoPtr< ArrayOf<IInterface*> > a = mArray;
    Int32 s = mSize;
    if (location >= s || location < 0) {
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    AutoPtr<IInterface> result = (*a)[location];
    a->Copy(location, a, location + 1, --s - location);
    a->Set(s, NULL);
    mSize = s;
    mModCount++;
    *object = result;
    REFCOUNT_ADD(*object)
    return NOERROR;
}

ECode ArrayList::Remove(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    VALIDATE_NOT_NULL(modified)

    AutoPtr< ArrayOf<IInterface*> > a = mArray;
    Int32 s = mSize;
    if (object != NULL) {
        for (Int32 i = 0; i < s; i++) {
            if (Object::Equals(object, (*a)[i])) {
                a->Copy(i, a, i + 1, --s - i);
                a->Set(s, NULL);
                mSize = s;
                mModCount++;
                *modified = TRUE;
                return NOERROR;
            }
        }
    }
    else {
        for (Int32 i = 0; i < s; i++) {
            if ((*a)[i] == NULL) {
                a->Copy(i, a, i + 1, --s - i);
                a->Set(s, NULL);
                mSize = s;
                mModCount++;
                *modified = TRUE;
                return NOERROR;
            }
        }
    }
    *modified = FALSE;
    return NOERROR;
}

ECode ArrayList::RemoveRange(
    /* [in] */ Int32 fromIndex,
    /* [in] */ Int32 toIndex)
{
    if (fromIndex == toIndex) {
        return NOERROR;
    }
    AutoPtr< ArrayOf<IInterface*> > a = mArray;
    Int32 s = mSize;
    if (fromIndex >= s) {
        // throw new IndexOutOfBoundsException("fromIndex " + fromIndex + " >= size " + size);
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    if (toIndex > s) {
        // throw new IndexOutOfBoundsException("toIndex " + toIndex + " > size " + size);
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    if (fromIndex > toIndex) {
        // throw new IndexOutOfBoundsException("fromIndex " + fromIndex + " > toIndex " + toIndex);
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    a->Copy(fromIndex, a, toIndex, s - toIndex);
    Int32 rangeSize = toIndex - fromIndex;
    // Arrays.fill(a, s - rangeSize, s, null);
    for (Int32 i = s - rangeSize; i < s; ++i) {
        a->Set(i, NULL);
    }
    mSize = s - rangeSize;
    mModCount++;
    return NOERROR;
}

ECode ArrayList::Set(
    /* [in] */ Int32 location,
    /* [in] */ IInterface* object,
    /* [out] */ IInterface** prevObject)
{
    AutoPtr< ArrayOf<IInterface*> > a = mArray;
    if (location >= mSize || location < 0) {
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    if (prevObject) {
        AutoPtr<IInterface> result = (*a)[location];
        *prevObject = result;
        REFCOUNT_ADD(*prevObject)
    }

    a->Set(location, object);
    return NOERROR;
}

ECode ArrayList::ToArray(
    /* [out, callee] */ ArrayOf<IInterface*>** array)
{
    VALIDATE_NOT_NULL(array)

    Int32 s = mSize;
    AutoPtr< ArrayOf<IInterface*> > result = ArrayOf<IInterface*>::Alloc(s);
    result->Copy(mArray, s);
    *array = result;
    REFCOUNT_ADD(*array)
    return NOERROR;
}

ECode ArrayList::ToArray(
    /* [in] */ ArrayOf<IInterface*>* inArray,
    /* [out, callee] */ ArrayOf<IInterface*>** outArray)
{
    VALIDATE_NOT_NULL(outArray)

    AutoPtr< ArrayOf<IInterface*> > contents = inArray;
    Int32 s = mSize;
    if (contents->GetLength() < s) {
        contents = ArrayOf<IInterface*>::Alloc(s);
    }
    contents->Copy(mArray, s);
    if (contents->GetLength() > s) {
        contents->Set(s, NULL);
    }
    *outArray = contents;
    REFCOUNT_ADD(*outArray)
    return NOERROR;
}

ECode ArrayList::TrimToSize()
{
    Int32 s = mSize;
    if (s == mArray->GetLength()) {
        return NOERROR;
    }
    if (s == 0) {
        mArray = ArrayOf<IInterface*>::Alloc(0);
    }
    else {
        AutoPtr< ArrayOf<IInterface*> > newArray = ArrayOf<IInterface*>::Alloc(s);
        newArray->Copy(mArray, s);;
        mArray = newArray;
    }
    mModCount++;
    return NOERROR;
}

ECode ArrayList::GetIterator(
    /* [out] */ IIterator** it)
{
    VALIDATE_NOT_NULL(it)
    return CArrayListIterator::New(this, it);
}

ECode ArrayList::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode)

    AutoPtr< ArrayOf<IInterface*> > a = mArray;
    Int32 outhashCode = 1;
    for (Int32 i = 0, s = mSize; i < s; i++) {
        AutoPtr<IInterface> e = (*a)[i];
        outhashCode = 31 * outhashCode + (e == NULL ? 0 : (Object::GetHashCode(e)));
    }
    *hashCode = outhashCode;
    return NOERROR;
}

ECode ArrayList::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (object == TO_IINTERFACE(this)) {
        *result = TRUE;
        return NOERROR;
    }
    if (!(IList::Probe(object))) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<IList> that = IList::Probe(object);
    Int32 s = mSize;
    Int32 length = 0;
    if (((ICollection::Probe(that))->GetSize(&length), length) != s) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr< ArrayOf<IInterface*> > a = mArray;
    if (IRandomAccess::Probe(that)) {
        for (Int32 i = 0; i < s; i++) {
            AutoPtr<IInterface> eThis = (*a)[i];
            AutoPtr<IInterface> ethat;
            that->Get(i, (IInterface**)&ethat);
            if (eThis == NULL ? ethat != NULL : !(Object::Equals(eThis, ethat))) {
                *result = FALSE;
                return NOERROR;
            }
        }
    }
    else {  // Argument list is not random access; use its iterator
        AutoPtr<IIterator> it;
        (IIterable::Probe(that))->GetIterator((IIterator**)&it);
        for (Int32 i = 0; i < s; i++) {
            AutoPtr<IInterface> eThis = (*a)[i];
            AutoPtr<IInterface> eThat;
            it->GetNext((IInterface**)&eThat);
            if (eThis == NULL ? eThat != NULL : !(Object::Equals(eThis, eThat))) {
                *result = FALSE;
                return NOERROR;
            }
        }
    }
    *result = TRUE;
    return NOERROR;
}

ECode ArrayList::WriteObject(
    /* [in] */ IObjectOutputStream* stream)
{
    stream->DefaultWriteObject();
    (IOutputStream::Probe(stream))->Write(mArray->GetLength());
    for (Int32 i = 0; i < mSize; i++) {
        assert(0 && "TODO");
        // stream->WriteObject((*mArray)[i]);
    }
    return NOERROR;
}

ECode ArrayList::ReadObject(
    /* [in] */ IObjectInputStream* stream)
{
    stream->DefaultReadObject();
    Int32 cap = 0;
    (IInputStream::Probe(stream))->Read(&cap);
    if (cap < mSize) {
        // throw new InvalidObjectException("Capacity: " + cap + " < size: " + size);
        return E_INVALID_OBJECT_EXCEPTION;
    }
    // array = (cap == 0 ? EmptyArray.OBJECT : new Object[cap]);
    mArray = NULL;
    mArray = ArrayOf<IInterface*>::Alloc(cap);
    for (Int32 i = 0; i < mSize; i++) {
        AutoPtr<IInterface> inface;
        assert(0 && "TODO");
        // stream->ReadObject((IInterface**)&inface);
        mArray->Set(i, inface);
    }
    return NOERROR;
}

ECode ArrayList::ContainsAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    return AbstractList::ContainsAll(collection, result);
}

ECode ArrayList::RemoveAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return AbstractList::RemoveAll(collection, modified);
}

ECode ArrayList::RetainAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return AbstractList::RetainAll(collection, modified);
}

ECode ArrayList::GetListIterator(
    /* [out] */ IListIterator** it)
{
    return AbstractList::GetListIterator(it);
}

ECode ArrayList::GetListIterator(
    /* [in] */ Int32 location,
    /* [out] */ IListIterator** it)
{
    return AbstractList::GetListIterator(location, it);
}

ECode ArrayList::GetSubList(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [out] */ IList** subList)
{
    return AbstractList::GetSubList(start, end, subList);
}

ECode ArrayList::Add(
    /* [in] */ IInterface* obj)
{
    return AbstractList::Add(obj);
}

ECode ArrayList::AddAll(
    /* [in] */ Int32 value,
    /* [in] */ ICollection* collection)
{
    return AbstractList::AddAll(value, collection);
}

ECode ArrayList::Remove(
    /* [in] */ Int32 value)
{
    return AbstractList::Remove(value);
}

ECode ArrayList::Set(
    /* [in] */ Int32 value,
    /* [in] */ IInterface* obj)
{
    return Set(value, obj, NULL);
}

ECode ArrayList::AddAll(
    /* [in] */ /* [in] */ ICollection* collection)
{
    return AbstractList::AddAll(collection);
}

ECode ArrayList::Remove(
    /* [in] */ IInterface* obj)
{
    return AbstractList::Remove(obj);
}

ECode ArrayList::RemoveAll(
    /* [in] */ /* [in] */ ICollection* collection)
{
    return AbstractList::RemoveAll(collection);
}

ECode ArrayList::RetainAll(
    /* [in] */ /* [in] */ ICollection* collection)
{
    return AbstractList::RetainAll(collection);
}


} // namespace Utility
} // namespace Elastos
