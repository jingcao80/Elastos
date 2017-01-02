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

#include "AbstractList.h"
#include "CSimpleListIterator.h"
#include "CFullListIterator.h"
#include "CSubAbstractListIterator.h"
#include "CSubAbstractList.h"
#include "CRandomAccessSubList.h"

using Elastos::Utility::IList;

namespace Elastos {
namespace Utility {

//=================================================
// AbstractList::SimpleListIterator
//=================================================
CAR_INTERFACE_IMPL(AbstractList::SimpleListIterator, Object, IIterator)

AbstractList::SimpleListIterator::SimpleListIterator()
    : mPos(-1)
    , mExpectedModeCount(0)
    , mLastPosition(-1)
{}

ECode AbstractList::SimpleListIterator::constructor(
    /* [in] */ IList* owner)
{
    mOwner = (AbstractList*)owner;
    mExpectedModeCount = mOwner->mModCount;
    return NOERROR;
}

ECode AbstractList::SimpleListIterator::HasNext(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 size;
    mOwner->GetSize(&size);
    *result = mPos + 1 < size;
    return NOERROR;
}

ECode AbstractList::SimpleListIterator::GetNext(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);
    if (mExpectedModeCount== mOwner->mModCount) {
        // try {
        ECode ec = mOwner->Get(mPos + 1, object);
        if (FAILED(ec)) return E_NO_SUCH_ELEMENT_EXCEPTION;
        mLastPosition = ++mPos;
        return NOERROR;
        // } catch (IndexOutOfBoundsException e) {
            // throw new NoSuchElementException();
        // }
    }
    return E_CONCURRENT_MODIFICATION_EXCEPTION;
}

ECode AbstractList::SimpleListIterator::Remove()
{
    if (mLastPosition == -1) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    if (mExpectedModeCount != mOwner->mModCount) {
        return E_CONCURRENT_MODIFICATION_EXCEPTION;
    }

    // try {
    AutoPtr<IInterface> object;
    ECode ec = mOwner->Remove(mLastPosition, (IInterface**)&object);
    if (FAILED(ec)) return E_CONCURRENT_MODIFICATION_EXCEPTION;
    // } catch (IndexOutOfBoundsException e) {
        // throw new ConcurrentModificationException();
    // }

    mExpectedModeCount = mOwner->mModCount;
    if (mPos == mLastPosition) {
        mPos--;
    }
    mLastPosition = -1;
    return NOERROR;
}


//=================================================
// AbstractList::FullListIterator
//=================================================

CAR_INTERFACE_IMPL(AbstractList::FullListIterator, SimpleListIterator, IListIterator)

AbstractList::FullListIterator::FullListIterator()
{
}

ECode AbstractList::FullListIterator::constructor(
    /* [in] */ IList* owner,
    /* [in] */ Int32 start)
{
    SimpleListIterator::constructor(owner);
    Int32 size;
    mOwner->GetSize(&size);
    if (start >= 0 && start <= size) {
        mPos = start - 1;
    }
    return NOERROR;
}

ECode AbstractList::FullListIterator::Add(
    /* [in] */ IInterface* object)
{
    if (mExpectedModeCount == mOwner->mModCount) {
        // try {
        ECode ec = mOwner->Add(mPos + 1, object);
        if (FAILED(ec)) {
            return E_NO_SUCH_ELEMENT_EXCEPTION;
        }
        // } catch (IndexOutOfBoundsException e) {
            // throw new NoSuchElementException();
        // }
        mPos++;
        mLastPosition = -1;
        if (mOwner->mModCount != mExpectedModeCount) {
            mExpectedModeCount = mOwner->mModCount;
        }
        return NOERROR;
    }
    else {
        return E_CONCURRENT_MODIFICATION_EXCEPTION;
    }
}

ECode AbstractList::FullListIterator::HasPrevious(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mPos >= 0;
    return NOERROR;
}

ECode AbstractList::FullListIterator::GetNextIndex(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mPos + 1;
    return NOERROR;
}

ECode AbstractList::FullListIterator::GetPrevious(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);
    if (mExpectedModeCount == mOwner->mModCount) {
        // try {
        ECode ec = mOwner->Get(mPos, object);
        if (FAILED(ec)) {
            return E_NO_SUCH_ELEMENT_EXCEPTION;
        }
        mLastPosition = mPos;
        mPos--;
        return NOERROR;
        // } catch (IndexOutOfBoundsException e) {
            // throw new NoSuchElementException();
        // }
    }
    return E_CONCURRENT_MODIFICATION_EXCEPTION;
}

ECode AbstractList::FullListIterator::GetPreviousIndex(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mPos;
    return NOERROR;
}

ECode AbstractList::FullListIterator::Set(
    /* [in] */ IInterface* object)
{
    if (mExpectedModeCount == mOwner->mModCount) {
        // try {
        ECode ec = mOwner->Set(mLastPosition, object);
        if (FAILED(ec)) {
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        return NOERROR;
        // } catch (IndexOutOfBoundsException e) {
            // throw new IllegalStateException();
        // }
    }
    else {
        return E_CONCURRENT_MODIFICATION_EXCEPTION;
    }
}

ECode AbstractList::FullListIterator::HasNext(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return SimpleListIterator::HasNext(result);
}

ECode AbstractList::FullListIterator::GetNext(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);
    return SimpleListIterator::GetNext(object);
}

ECode AbstractList::FullListIterator::Remove()
{
    return SimpleListIterator::Remove();
}

////////////////////////////////////////////
// SubAbstractList::SubAbstractListIterator
////////////////////////////////////////////

CAR_INTERFACE_IMPL_2(SubAbstractList::SubAbstractListIterator, Object, IListIterator, IIterator)

SubAbstractList::SubAbstractListIterator::SubAbstractListIterator()
    : mStart(0)
    , mEnd(0)
{
}

ECode SubAbstractList::SubAbstractListIterator::constructor(
    /* [in] */ IList* list,
    /* [in] */ IListIterator* it,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length)
{
    mSubList = (SubAbstractList*)list;
    mIterator = it;
    mStart = offset;
    mEnd = mStart + length;
    return NOERROR;
}

ECode SubAbstractList::SubAbstractListIterator::Add(
    /* [in] */ IInterface* object)
{
    mIterator->Add(object);
    mSubList->SizeChanged(TRUE);
    mEnd++;
    return NOERROR;
}

ECode SubAbstractList::SubAbstractListIterator::HasNext(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 index;
    mIterator->GetNextIndex(&index);
    *result = index < mEnd;
    return NOERROR;
}

ECode SubAbstractList::SubAbstractListIterator::HasPrevious(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 previousindex;
    mIterator->GetPreviousIndex(&previousindex);
    *result = previousindex >= mStart;
    return NOERROR;
}

ECode SubAbstractList::SubAbstractListIterator::GetNext(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);
    Int32 index;
    mIterator->GetNextIndex(&index);
    if (index < mEnd) {
        IIterator* iterator = IIterator::Probe(mIterator);
        iterator->GetNext(object);
    }
    return E_NO_SUCH_ELEMENT_EXCEPTION;
}

ECode SubAbstractList::SubAbstractListIterator::GetNextIndex(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 index;
    mIterator->GetNextIndex(&index);
    *result = index - mStart;
    return NOERROR;
}

ECode SubAbstractList::SubAbstractListIterator::GetPrevious(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);
    Int32 previousindex;
    mIterator->GetPreviousIndex(&previousindex);
    if (previousindex >= mStart) {
       return mIterator->GetPrevious(object);
    }
    return E_NO_SUCH_ELEMENT_EXCEPTION;
}

ECode SubAbstractList::SubAbstractListIterator::GetPreviousIndex(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 previousindex;
    mIterator->GetPreviousIndex(&previousindex);
    if (previousindex >= mStart) {
        *result = previousindex - mStart;
        return NOERROR;
    }
    *result = -1;
    return NOERROR;
}

ECode SubAbstractList::SubAbstractListIterator::Remove()
{
    IIterator* iterator = IIterator::Probe(mIterator);
    iterator->Remove();
    mSubList->SizeChanged(FALSE);
    mEnd--;
    return NOERROR;
}

ECode SubAbstractList::SubAbstractListIterator::Set(
    /* [in] */ IInterface* object)
{
    return mIterator->Set(object);
}

////////////////////////////////////////////
//              SubAbstractList
////////////////////////////////////////////

SubAbstractList::SubAbstractList()
    : mOffset(0)
    , mSize(0)
{
}

ECode SubAbstractList::constructor(
    /* [in] */ IList* list,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    mOffset = start;
    mSize = end - start;
    mFulllist = (AbstractList*)list;
    mModCount = mFulllist->mModCount;
    return NOERROR;
}

ECode SubAbstractList::Add(
    /* [in] */ Int32 location,
    /* [in] */ IInterface* object)
{
    if (mModCount == mFulllist->mModCount) {
        if (location >= 0 && location <= mSize) {
            mFulllist->Add(location + mOffset, object);
            mSize++;
            mModCount = mFulllist->mModCount;
            return NOERROR;
        }
        else {
            return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
        }
    }
    else {
        return E_CONCURRENT_MODIFICATION_EXCEPTION;
    }
}

ECode SubAbstractList::AddAll(
    /* [in] */ Int32 location,
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(collection);
    VALIDATE_NOT_NULL(result);
    if (mModCount == mFulllist->mModCount) {
        if (location >= 0 && location <= mSize) {
            *result = FALSE;
            mFulllist->AddAll(location + mOffset, collection, result);
            if (*result) {
                Int32 collectionsize;
                collection->GetSize(&collectionsize);
                mSize += collectionsize;
                mModCount += mFulllist->mModCount;
            }
            return NOERROR;
        }
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    return E_CONCURRENT_MODIFICATION_EXCEPTION;
}

ECode SubAbstractList::AddAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (mModCount == mFulllist->mModCount) {
        *result = FALSE;
        mFulllist->AddAll(mOffset + mSize, collection, result);
        if (*result) {
            Int32 collectionsize;
            collection->GetSize(&collectionsize);
            mSize += collectionsize;
            mModCount = mFulllist->mModCount;
        }
        return NOERROR;
    }
    return E_CONCURRENT_MODIFICATION_EXCEPTION;
}

ECode SubAbstractList::Get(
    /* [in] */ Int32 location,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);
    if (mModCount == mFulllist->mModCount) {
        if (location >= 0 && location < mSize) {
            return mFulllist->Get(location + mOffset, object);
        }
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    return E_CONCURRENT_MODIFICATION_EXCEPTION;
}

ECode SubAbstractList::GetIterator(
    /* [out] */ IIterator** it)
{
    VALIDATE_NOT_NULL(it);
    AutoPtr<IListIterator> listit;
    GetListIterator(0, (IListIterator**)&listit);
    *it = IIterator::Probe(listit);
    REFCOUNT_ADD(*it);
    return NOERROR;
}

ECode SubAbstractList::GetListIterator(
    /* [in] */ Int32 location,
    /* [out] */ IListIterator** listiterator)
{
    VALIDATE_NOT_NULL(listiterator);
    if (mModCount == mFulllist->mModCount) {
        if (location >= 0 && location <= mSize) {
            AutoPtr<IListIterator> it;
            mFulllist->GetListIterator(location + mOffset, (IListIterator**)&it);
            return CSubAbstractListIterator::New(this, it, mOffset, mSize, listiterator);
        }
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    return E_CONCURRENT_MODIFICATION_EXCEPTION;
}

ECode SubAbstractList::Remove(
    /* [in] */ Int32 location,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);
    if (mModCount == mFulllist->mModCount) {
        if (location >= 0 && location < mSize) {
            mFulllist->Remove(location + mOffset, object);
            mSize--;
            mModCount =  mFulllist->mModCount;
            return NOERROR;
        }
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    return E_CONCURRENT_MODIFICATION_EXCEPTION;
}

ECode SubAbstractList::RemoveRange(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    if (start != end) {
        if (mModCount == mFulllist->mModCount) {
            mFulllist->RemoveRange(start + mOffset, end + mOffset);
            mSize -= (end - start);
            mModCount = mFulllist->mModCount;
            return NOERROR;
        }
        else {
            return E_CONCURRENT_MODIFICATION_EXCEPTION;
        }
    }
    return NOERROR;
}

ECode SubAbstractList::Set(
    /* [in] */ Int32 location,
    /* [in] */ IInterface* inobject,
    /* [out] */ IInterface** outobject)
{
    VALIDATE_NOT_NULL(outobject);
    *outobject = NULL;

    if (mModCount == mFulllist->mModCount) {
        if (location >= 0 && location < mSize) {
           return mFulllist->Set(location + mOffset, inobject, outobject);
        }
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    return E_CONCURRENT_MODIFICATION_EXCEPTION;
}

ECode SubAbstractList::GetSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size);
    *size = 0;

    if (mModCount == (mFulllist->mModCount)) {
        *size = mSize;
        return NOERROR;
    }
    return E_CONCURRENT_MODIFICATION_EXCEPTION;
}

ECode SubAbstractList::SizeChanged(
    /* [in] */ Boolean increment)
{
    if (increment) {
        mSize++;
    }
    else {
        mSize--;
    }
    mModCount = mFulllist->mModCount;
    return NOERROR;
}

////////////////////////////////////////////
//              AbstractList
////////////////////////////////////////////

CAR_INTERFACE_IMPL(AbstractList, AbstractCollection, IList)

ECode AbstractList::constructor()
{
    mModCount = 0;
    return NOERROR;
}

ECode AbstractList::Add(
    /* [in] */ Int32 location,
    /* [in] */ IInterface* object)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode AbstractList::Add(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 size;
    GetSize(&size);
    FAIL_RETURN(Add(size, object));
    *result = TRUE;
    return NOERROR;
}

ECode AbstractList::Add(
    /* [in] */ IInterface* object)
{
    Boolean result;
    return Add(object, &result);
}

ECode AbstractList::AddAll(
    /* [in] */ Int32 location,
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    VALIDATE_NOT_NULL(collection);
    AutoPtr<IIterator> it;
    (IIterable::Probe(collection))->GetIterator((IIterator**)&it);
    Boolean hasnext = FALSE;
    while ((it->HasNext(&hasnext), hasnext)) {
        AutoPtr<IInterface> nextobject;
        it->GetNext((IInterface**)&nextobject);
        FAIL_RETURN(Add(location++, nextobject));
    }
    Boolean flag = FALSE;
    collection->IsEmpty(&flag);
    *result = !flag;
    return NOERROR;
}

ECode AbstractList::AddAll(
    /* [in] */ Int32 location,
    /* [in] */ ICollection* collection)
{
    Boolean result;
    return AddAll(location, collection, &result);
}

ECode AbstractList::AddAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return AbstractCollection::AddAll(collection, modified);
}

ECode AbstractList::AddAll(
    /* [in] */ ICollection* collection)
{
    return AbstractCollection::AddAll(collection);
}

ECode AbstractList::Clear()
{
    Int32 size;
    GetSize(&size);
    return RemoveRange(0, size);
}

ECode AbstractList::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (TO_IINTERFACE(this) == object) {
        *result = TRUE;
        return NOERROR;
    }
    AutoPtr<IList> list = (IList*)(object->Probe(EIID_IList));
    if (list == NULL) {
        *result = FALSE;
        return NOERROR;
    }

    Int32 listsize;
    (ICollection::Probe(list))->GetSize(&listsize);
    Int32 size;
    GetSize(&size);
    if (listsize != size) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<IIterator> it1;
    (IIterable::Probe(this))->GetIterator((IIterator**)&it1);
    AutoPtr<IIterator> it2;
    (IIterable::Probe(list))->GetIterator((IIterator**)&it2);
    Boolean hasnext = FALSE;
    while ((it1->HasNext(&hasnext), hasnext)) {
        AutoPtr<IInterface> e1;
        it1->GetNext((IInterface**)&e1);
        AutoPtr<IInterface> e2;
        it2->GetNext((IInterface**)&e2);
        if (!Object::Equals(e1, e2)) {
            *result = FALSE;
            return NOERROR;
        }
    }
    *result = TRUE;
    return NOERROR;
}

ECode AbstractList::GetHashCode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 1;
    AutoPtr<IIterator> it;
    GetIterator((IIterator**)&it);
    Boolean hasnext = FALSE;
    while ((it->HasNext(&hasnext), hasnext)) {
        AutoPtr<IInterface> object;
        it->GetNext((IInterface**)&object);
        if (object == NULL) {
            *result = 31 * (*result);
        }
        else {
            Int32 ret = Object::GetHashCode(object);
            *result = 31 * (*result) + ret;
        }
    }
    return NOERROR;
}

ECode AbstractList::IndexOf(
    /* [in] */ IInterface* object,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IListIterator> it;
    GetListIterator((IListIterator**)&it);
    Boolean hasnext = FALSE;
    if (object != NULL) {
        while (((IIterator::Probe(it))->HasNext(&hasnext), hasnext)) {
            AutoPtr<IInterface> nextobject;
            (IIterator::Probe(it))->GetNext((IInterface**)&nextobject);
            if (Object::Equals(object, nextobject)) {
                Int32 previousindex;
                it->GetPreviousIndex(&previousindex);
                *result = previousindex;
                return NOERROR;
            }
        }
    } else {
        while (((IIterator::Probe(it))->HasNext(&hasnext), hasnext)) {
            AutoPtr<IInterface> nextobject;
            (IIterator::Probe(it))->GetNext((IInterface**)&nextobject);
            if (nextobject == NULL) {
                Int32 previousindex;
                it->GetPreviousIndex(&previousindex);
                *result = previousindex;
                return NOERROR;
            }
        }
    }
    *result = -1;
    return NOERROR;
}

ECode AbstractList::GetIterator(
    /* [out] */ IIterator** it)
{
    return CSimpleListIterator::New(this, it);
}

ECode AbstractList::LastIndexOf(
    /* [in] */ IInterface* object,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 size;
    GetSize(&size);
    AutoPtr<IListIterator> it;
    GetListIterator(size, (IListIterator**)&it);
    Boolean hasprevious = FALSE;
    if (object != NULL) {
        while((it->HasPrevious(&hasprevious), hasprevious)) {
            AutoPtr<IInterface> previousobject;
            it->GetPrevious((IInterface**)&previousobject);
            if (Object::Equals(object, previousobject)) {
                Int32 nextindex;
                it->GetNextIndex(&nextindex);
                *result = nextindex;
                return NOERROR;
            }
        }
    }
    else {
        while ((it->HasPrevious(&hasprevious), hasprevious)) {
            AutoPtr<IInterface> previousobject;
            it->GetPrevious((IInterface**)&previousobject);
            if (previousobject == NULL) {
                Int32 nextindex;
                it->GetNextIndex(&nextindex);
                *result = nextindex;
                return NOERROR;
            }
        }
    }
    *result = -1;
    return NOERROR;
}

ECode AbstractList::GetListIterator(
    /* [out] */ IListIterator** listiterator)
{
    VALIDATE_NOT_NULL(listiterator);
    return GetListIterator(0, listiterator);
}

ECode AbstractList::GetListIterator(
    /* [in] */ Int32 location,
    /* [out] */ IListIterator** listiterator)
{
    return CFullListIterator::New(this, location, listiterator);
}

ECode AbstractList::Remove(
    /* [in] */ Int32 location,
    /* [out] */ IInterface** object)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode AbstractList::Remove(
    /* [in] */ Int32 location)
{
    AutoPtr<IInterface> obj;
    return Remove(location, (IInterface**)&obj);
}

ECode AbstractList::RemoveRange(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    AutoPtr<IIterator> it;
    AutoPtr<IListIterator> listit;
    GetListIterator(start, (IListIterator**)&listit);
    it = IIterator::Probe(listit);
    for (Int32 i = start; i < end; i++) {
        AutoPtr<IInterface> nextobject;
        it->GetNext((IInterface**)&nextobject);
        it->Remove();
    }
    return NOERROR;
}

ECode AbstractList::Remove(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    return AbstractCollection::Remove(object, result);
}

ECode AbstractList::Remove(
    /* [in] */ IInterface* object)
{
    return AbstractCollection::Remove(object);
}

ECode AbstractList::RemoveAll(
    /* [in] */ ICollection* collection)
{
    return AbstractCollection::RemoveAll(collection);
}

ECode AbstractList::RemoveAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    return AbstractCollection::RemoveAll(collection, result);
}

ECode AbstractList::RetainAll(
    /* [in] */ ICollection* collection)
{
    return AbstractCollection::RetainAll(collection);
}

ECode AbstractList::RetainAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    return AbstractCollection::RetainAll(collection, result);
}

ECode AbstractList::Set(
    /* [in] */ Int32 location,
    /* [in] */ IInterface* object,
    /* [out] */ IInterface** prevObject)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode AbstractList::Set(
    /* [in] */ Int32 location,
    /* [in] */ IInterface* object)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode AbstractList::GetSubList(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [out] */ IList** list)
{
    VALIDATE_NOT_NULL(list);
    Int32 size;
    GetSize(&size);
    if (start >= 0 && end <= size) {
        if (start <= end) {
            if (IRandomAccess::Probe(this)) {
                return CRandomAccessSubList::New(this, start, end, list);
            }

            return CSubAbstractList::New(this, start, end, list);
        }
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
}

ECode AbstractList::ToArray(
    /* [out, callee] */ ArrayOf<IInterface*>** array)
{
    return AbstractCollection::ToArray(array);
}

ECode AbstractList::ToArray(
    /* [in] */ ArrayOf<IInterface*>* contents,
    /* [out, callee] */ ArrayOf<IInterface*>** outArray)
{
    return AbstractCollection::ToArray(contents, outArray);
}

ECode AbstractList::Contains(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    return AbstractCollection::Contains(object, result);
}

ECode AbstractList::ContainsAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    return AbstractCollection::ContainsAll(collection, result);
}

ECode AbstractList::IsEmpty(
    /* [out] */ Boolean* result)
{
    return AbstractCollection::IsEmpty(result);
}

////////////////////////////////////////////
//      RandomAccessSubList
////////////////////////////////////////////

CAR_INTERFACE_IMPL(RandomAccessSubList, SubAbstractList, IRandomAccess)

ECode RandomAccessSubList::constructor(
    /* [in] */ IList* list,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    return SubAbstractList::constructor(list, start, end);
}

} // Utility
} // Elastos
