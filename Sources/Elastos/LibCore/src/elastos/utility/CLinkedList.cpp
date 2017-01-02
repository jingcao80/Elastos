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

#include "CLinkedList.h"
#include "CArrayList.h"

using Elastos::Core::EIID_ICloneable;
using Elastos::IO::IObjectInput;
using Elastos::IO::IObjectOutput;
using Elastos::IO::EIID_ISerializable;
using Elastos::IO::IDataOutput;
using Elastos::IO::IDataInput;

namespace Elastos {
namespace Utility {

//============================================================
// CLinkedList::Link
//============================================================
CLinkedList::Link::Link(
    /* [in] */ IInterface* o,
    /* [in] */ Link* p,
    /* [in] */ Link* n)
    : mData(o)
    , mPrevious(p)
    , mNext(n)
{}

//============================================================
// CLinkedList::LinkIterator
//============================================================
CLinkedList::LinkIterator::LinkIterator(
    /* [in] */ CLinkedList* object,
    /* [in] */ Int32 location)
    : mList(object)
{
    mExpectedModCount = mList->mModCount;
    if (location >= 0 && location <= mList->mSize) {
        // pos ends up as -1 if list is empty, it ranges from -1 to
        // list.size - 1
        // if link == voidLink then pos must == -1
        mLink = mList->mVoidLink;
        if (location < mList->mSize / 2) {
            for (mPos = -1; mPos + 1 < location; mPos++) {
                mLink = mLink->mNext;
            }
        }
        else {
            for (mPos = mList->mSize; mPos >= location; mPos--) {
                mLink = mLink->mPrevious;
            }
        }
    }
}

CAR_INTERFACE_IMPL_2(CLinkedList::LinkIterator, Object, IListIterator, IIterator)

ECode CLinkedList::LinkIterator::Add(
    /* [in] */ IInterface* object)
{
    if (mExpectedModCount == mList->mModCount) {
        AutoPtr<Link> next = mLink->mNext;
        AutoPtr<Link> newLink = new Link(object, mLink, next);
        mLink->mNext = newLink;
        next->mPrevious = newLink;
        mLink = newLink;
        mLastLink = NULL;
        mPos++;
        mExpectedModCount++;
        mList->mSize++;
        mList->mModCount++;
        return NOERROR;
    }
    else {
        return E_CONCURRENT_MODIFICATION_EXCEPTION;
    }
}

ECode CLinkedList::LinkIterator::HasNext(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mLink->mNext != mList->mVoidLink;
    return NOERROR;
}

ECode CLinkedList::LinkIterator::HasPrevious(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mLink != mList->mVoidLink;
    return NOERROR;
}

ECode CLinkedList::LinkIterator::GetNext(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);
    if (mExpectedModCount == mList->mModCount) {
        AutoPtr<Link> next = mLink->mNext;
        if (next != mList->mVoidLink) {
            mLastLink = mLink = next;
            mPos++;
            *object = mLink->mData;
            REFCOUNT_ADD(*object);
            return NOERROR;
        }
        *object = NULL;
        return E_NO_SUCH_ELEMENT_EXCEPTION;
    }
    *object = NULL;
    return E_CONCURRENT_MODIFICATION_EXCEPTION;
}

ECode CLinkedList::LinkIterator::NextIndex(
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index);
    *index = mPos + 1;
    return NOERROR;
}

ECode CLinkedList::LinkIterator::Previous(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);
    if (mExpectedModCount == mList->mModCount) {
        if (mLink != mList->mVoidLink) {
            mLastLink = mLink;
            mLink = mLink->mPrevious;
            mPos--;
            *object = mLastLink->mData;
            REFCOUNT_ADD(*object);
            return NOERROR;
        }
        *object = NULL;
        return E_NO_SUCH_ELEMENT_EXCEPTION;
    }
    *object = NULL;
    return E_CONCURRENT_MODIFICATION_EXCEPTION;
}

ECode CLinkedList::LinkIterator::PreviousIndex(
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index);
    *index = mPos;
    return NOERROR;
}

ECode CLinkedList::LinkIterator::Remove()
{
    if (mExpectedModCount == mList->mModCount) {
        if (mLastLink != NULL) {
            AutoPtr<Link> next = mLastLink->mNext;
            AutoPtr<Link> previous = mLastLink->mPrevious;
            next->mPrevious = previous;
            previous->mNext = next;
            if (mLastLink == mLink) {
                mPos--;
            }
            mLink = previous;
            mLastLink = NULL;
            mExpectedModCount++;
            mList->mSize--;
            mList->mModCount++;
            return NOERROR;
        }
        else {
            return E_ILLEGAL_STATE_EXCEPTION;
        }
    }
    else {
        return E_CONCURRENT_MODIFICATION_EXCEPTION;
    }
}

ECode CLinkedList::LinkIterator::Set(
    /* [in] */ IInterface* object)
{
    if (mExpectedModCount == mList->mModCount) {
        if (mLastLink != NULL) {
            mLastLink->mData = object;
            return NOERROR;
        }
        else {
            return E_ILLEGAL_STATE_EXCEPTION;
        }
    }
    else {
        return E_CONCURRENT_MODIFICATION_EXCEPTION;
    }
}

ECode CLinkedList::LinkIterator::GetNextIndex(
    /* [out] */ Int32* index)
{
    return NextIndex(index);
}

ECode CLinkedList::LinkIterator::GetPrevious(
    /* [out] */ IInterface** object)
{
    return Previous(object);
}

ECode CLinkedList::LinkIterator::GetPreviousIndex(
    /* [out] */ Int32* index)
{
    return PreviousIndex(index);
}


//============================================================
// CLinkedList::ReverseLinkIterator
//============================================================
CLinkedList::ReverseLinkIterator::ReverseLinkIterator(
    /* [in] */ CLinkedList* linkedList)
    : mList(linkedList)
    , mCanRemove(FALSE)
{
    mExpectedModCount = mList->mModCount;
    mLink = mList->mVoidLink;
}

CAR_INTERFACE_IMPL(CLinkedList::ReverseLinkIterator, Object, IIterator)

ECode CLinkedList::ReverseLinkIterator::HasNext(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mLink->mPrevious != mList->mVoidLink;
    return NOERROR;
}

ECode CLinkedList::ReverseLinkIterator::GetNext(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);
    if (mExpectedModCount == mList->mModCount) {
        Boolean flag = FALSE;
        if ((HasNext(&flag), flag)) {
            mLink = mLink->mPrevious;
            mCanRemove = TRUE;
            *object = mLink->mData;
            REFCOUNT_ADD(*object);
            return NOERROR;
        }
        return E_NO_SUCH_ELEMENT_EXCEPTION;
    }
    return E_CONCURRENT_MODIFICATION_EXCEPTION;
}

ECode CLinkedList::ReverseLinkIterator::Remove()
{
    if (mExpectedModCount == mList->mModCount) {
        if (mCanRemove) {
            AutoPtr<Link> next = mLink->mPrevious;
            AutoPtr<Link> previous = mLink->mNext;
            next->mNext = previous;
            previous->mPrevious = next;
            mLink = previous;
            mList->mSize--;
            mList->mModCount++;
            mExpectedModCount++;
            mCanRemove = FALSE;
            return NOERROR;
        }
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return E_CONCURRENT_MODIFICATION_EXCEPTION;
}


//============================================================
// CLinkedList
//============================================================
CAR_OBJECT_IMPL(CLinkedList)

CAR_INTERFACE_IMPL_5(CLinkedList, AbstractSequentialList, ILinkedList, IDeque, IQueue, ICloneable, ISerializable)

CLinkedList::CLinkedList()
    : mSize(0)
{}

CLinkedList::~CLinkedList()
{
    Clear();
    mVoidLink->mNext = NULL;
    mVoidLink->mPrevious = NULL;
}

ECode CLinkedList::constructor()
{
    mVoidLink = new Link(NULL, NULL, NULL);
    mVoidLink->mPrevious = mVoidLink;
    mVoidLink->mNext = mVoidLink;
    return NOERROR;
}

ECode CLinkedList::constructor(
    /* [in] */ ICollection* collection)
{
    constructor();
    Boolean result;
    return AddAll(collection, &result);
}

ECode CLinkedList::Add(
    /* [in] */ Int32 location,
    /* [in] */ IInterface* object)
{
    if (location >= 0 && location <= mSize) {
        AutoPtr<Link> link = mVoidLink;
        if (location < (mSize / 2)) {
            for (Int32 i = 0; i <= location; ++i) {
                link = link->mNext;
            }
        }
        else {
            for (Int32 i = mSize; i > location; i--) {
                link = link->mPrevious;
            }
        }
        AutoPtr<Link> previous = link->mPrevious;
        AutoPtr<Link> newLink = new Link(object, previous, link);
        previous->mNext = newLink;
        link->mPrevious = newLink;
        mSize++;
        mModCount++;
        return NOERROR;
    }
    else {
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
}

ECode CLinkedList::Add(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    return AddLastImpl(object, result);
}

ECode CLinkedList::AddLastImpl(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<Link> oldLast = mVoidLink->mPrevious;
    AutoPtr<Link> newLink = new Link(object, oldLast, mVoidLink);
    mVoidLink->mPrevious = newLink;
    oldLast->mNext = newLink;
    mSize++;
    mModCount++;
    *result = TRUE;
    return NOERROR;
}

ECode CLinkedList::AddAll(
    /* [in] */ Int32 location,
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    if (location < 0 || location > mSize) {
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    if (NULL == collection) {
        return E_NULL_POINTER_EXCEPTION;
    }

    Int32 adding;
    collection->GetSize(&adding);
    if (adding == 0) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<ICollection> elements;
    if (collection == this) {
        AutoPtr<IArrayList> arrayList;
        CArrayList::New(collection, (IArrayList**)&arrayList);
        elements = ICollection::Probe(arrayList);
    }
    else {
        elements = collection;
    }

    AutoPtr<Link> previous = mVoidLink;
    if (location < (mSize / 2)) {
        for (Int32 i = 0; i < location; ++i) {
            previous = previous->mNext;
        }
    }
    else {
        for (Int32 i = mSize; i >= location; i--) {
            previous = previous->mPrevious;
        }
    }

    AutoPtr<Link> next = previous->mNext;
    AutoPtr<IIterator> it;
    elements->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<Link> newLink = new Link(obj, previous, NULL);
        previous->mNext = newLink;
        previous = newLink;
    }

    previous->mNext = next;
    next->mPrevious = previous;
    mSize += adding;
    mModCount++;
    *result = TRUE;
    return NOERROR;
}

ECode CLinkedList::AddAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    if (collection == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }

    Int32 adding;
    collection->GetSize(&adding);
    if (adding == 0) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<ICollection> elements;
    if (collection == this) {
        AutoPtr<IArrayList> arrayList;
        CArrayList::New(collection, (IArrayList**)&arrayList);
        elements = ICollection::Probe(arrayList);
    }
    else {
        elements = collection;
    }

    AutoPtr<Link> previous = mVoidLink->mPrevious;
    AutoPtr<IIterator> it;
    elements->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<Link> newLink = new Link(obj, previous, NULL);
        previous->mNext = newLink;
        previous = newLink;
    }
    previous->mNext = mVoidLink;
    mVoidLink->mPrevious = previous;
    mSize += adding;
    mModCount++;
    *result = TRUE;
    return NOERROR;
}

ECode CLinkedList::AddFirst(
    /* [in] */ IInterface* object)
{
    Boolean ret = FALSE;
    return AddFirstImpl(object, &ret);
}

ECode CLinkedList::AddFirstImpl(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<Link> oldFirst = mVoidLink->mNext;
    AutoPtr<Link> newLink = new Link(object, mVoidLink, oldFirst);
    mVoidLink->mNext = newLink;
    oldFirst->mPrevious = newLink;
    mSize++;
    mModCount++;
    *result = TRUE;
    return NOERROR;
}

ECode CLinkedList::AddLast(
    /* [in] */ IInterface* object)
{
    Boolean ret = FALSE;
    return AddLastImpl(object, &ret);
}

ECode CLinkedList::Clear()
{
    while (mVoidLink->mNext != mVoidLink) {
        AutoPtr<Link> node = mVoidLink->mNext;
        mVoidLink->mNext = node->mNext;
        node->mNext->mPrevious = mVoidLink;
        node->mNext = NULL;
        node->mPrevious = NULL;
        node = NULL;
    }
    if (mSize > 0) {
        mSize = 0;
        mModCount++;
    }
    return NOERROR;
}

ECode CLinkedList::Clone(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);
    *object = NULL;

    // try {
    AutoPtr<ILinkedList> l;
    CLinkedList::New((ILinkedList**)&l);
    CLinkedList* cl = (CLinkedList*)l.Get();
    cl->mSize = 0;
    cl->mVoidLink = new Link(NULL, NULL, NULL);
    cl->mVoidLink->mPrevious = cl->mVoidLink;
    cl->mVoidLink->mNext = cl->mVoidLink;

    FAIL_RETURN((ICollection::Probe(l))->AddAll(this));
    *object = l.Get();
    REFCOUNT_ADD(*object);
    return NOERROR;
    // } catch (CloneNotSupportedException e) {
        // throw new AssertionError(e);
    // }
}

ECode CLinkedList::Contains(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<Link> link = mVoidLink->mNext;
    if (object != NULL) {
        while (link != mVoidLink) {
            if (Object::Equals(object,link->mData)) {
                *result = TRUE;
                return NOERROR;
            }
            link = link->mNext;
        }
    }
    else {
        while (link != mVoidLink) {
            if (link->mData == NULL) {
                *result = TRUE;
                return NOERROR;
            }
            link = link->mNext;
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode CLinkedList::Get(
    /* [in] */ Int32 location,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);
    if (location >= 0 && location < mSize) {
        AutoPtr<Link> link = mVoidLink;
        if (location < (mSize / 2)) {
            for (Int32 i = 0; i <= location; ++i) {
                link = link->mNext;
            }
        }
        else {
            for (Int32 i = mSize; i > location; i--) {
                link = link->mPrevious;
            }
        }
        *object = link->mData;
        REFCOUNT_ADD(*object);
        return NOERROR;
    }
    *object = NULL;
    return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
}

ECode CLinkedList::GetFirst(
    /* [out] */ IInterface** object)
{
    return GetFirstImpl(object);
}

ECode CLinkedList::GetFirstImpl(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)
    AutoPtr<Link> first = mVoidLink->mNext;
    if (first != mVoidLink) {
        *object = first->mData;
        REFCOUNT_ADD(*object);
        return NOERROR;
    }
    *object = NULL;
    return E_NO_SUCH_ELEMENT_EXCEPTION;
}

ECode CLinkedList::GetLast(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);
    AutoPtr<Link> last = mVoidLink->mPrevious;
    if (last != mVoidLink) {
        *object = last->mData;
        REFCOUNT_ADD(*object);
        return NOERROR;
    }
    *object = NULL;
    return E_NO_SUCH_ELEMENT_EXCEPTION;
}

ECode CLinkedList::IndexOf(
    /* [in] */ IInterface* object,
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index);
    Int32 pos = 0;
    AutoPtr<Link> link = mVoidLink->mNext;
    if (object != NULL) {
        while (link != mVoidLink) {
            if (Object::Equals(object,link->mData)) {
                *index = pos;
                return NOERROR;
            }
            link = link->mNext;
            pos++;
        }
    }
    else {
        while (link != mVoidLink) {
            if (link->mData == NULL) {
                *index = pos;
                return NOERROR;
            }
            link = link->mNext;
            pos++;
        }
    }
    *index = -1;
    return NOERROR;
}

ECode CLinkedList::LastIndexOf(
    /* [in] */ IInterface* object,
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index);
    Int32 pos = mSize;
    AutoPtr<Link> link = mVoidLink->mPrevious;
    if (object != NULL) {
        while (link != mVoidLink) {
            pos--;
            if (Object::Equals(object,link->mData)) {
                *index = pos;
                return NOERROR;
            }
            link = link->mPrevious;
        }
    }
    else {
        while (link != mVoidLink) {
            pos--;
            if (link->mData == NULL) {
                *index = pos;
                return NOERROR;
            }
            link = link->mPrevious;
        }
    }
    *index = -1;
    return NOERROR;
}

ECode CLinkedList::GetListIterator(
    /* [in] */ Int32 location,
    /* [out] */ IListIterator** it)
{
    VALIDATE_NOT_NULL(it);
    AutoPtr<IListIterator> obj = new LinkIterator(this , location);
    *it = obj;
    REFCOUNT_ADD(*it);
    return NOERROR;
}

ECode CLinkedList::Remove(
    /* [in] */ Int32 location,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);
    if (location >= 0 && location < mSize) {
        AutoPtr<Link> link = mVoidLink;
        if (location < (mSize / 2)) {
            for (Int32 i = 0; i <= location; ++i) {
                link = link->mNext;
            }
        }
        else {
            for (Int32 i = mSize; i > location; i--) {
                link = link->mPrevious;
            }
        }
        AutoPtr<Link> previous = link->mPrevious;
        AutoPtr<Link> next = link->mNext;
        previous->mNext = next;
        next->mPrevious = previous;
        mSize--;
        mModCount++;
        *object = link->mData;
        REFCOUNT_ADD(*object);
        return NOERROR;
    }
    *object = NULL;
    return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
}

ECode CLinkedList::Remove(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    return RemoveFirstOccurrenceImpl(object, result);
}

ECode CLinkedList::RemoveFirst(
    /* [out] */ IInterface** object)
{
    return RemoveFirstImpl(object);
}

ECode CLinkedList::RemoveFirstImpl(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)
    AutoPtr<Link> first = mVoidLink->mNext;
    if (first != mVoidLink) {
        AutoPtr<Link> next = first->mNext;
        mVoidLink->mNext = next;
        next->mPrevious = mVoidLink;
        mSize--;
        *object = first->mData;
        REFCOUNT_ADD(*object);
        return NOERROR;
    }
    *object = NULL;
    return E_NO_SUCH_ELEMENT_EXCEPTION;
}

ECode CLinkedList::RemoveLast(
    /* [out] */ IInterface** object)
{
    return RemoveLastImpl(object);
}

ECode CLinkedList::RemoveLastImpl(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)
    AutoPtr<Link> last = mVoidLink->mPrevious;
    if(last != mVoidLink) {
        AutoPtr<Link> previous = last->mPrevious;
        mVoidLink->mPrevious = previous;
        previous->mNext = mVoidLink;
        mSize--;
        mModCount++;
        *object = last->mData;
        REFCOUNT_ADD(*object);
        return NOERROR;
    }
    *object = NULL;
    return E_NO_SUCH_ELEMENT_EXCEPTION;
}

ECode CLinkedList::GetDescendingIterator(
    /* [out] */ IIterator** iterator)
{
    VALIDATE_NOT_NULL(iterator);
    *iterator = new ReverseLinkIterator(this);
    REFCOUNT_ADD(*iterator);
    return NOERROR;
}

ECode CLinkedList::OfferFirst(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    return AddFirstImpl(object, result);
}

ECode CLinkedList::OfferLast(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    return AddLastImpl(object, result);
}

ECode CLinkedList::PeekFirst(
    /* [out] */ IInterface** object)
{
    return PeekFirstImpl(object);
}

ECode CLinkedList::PeekLast(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);
    AutoPtr<Link> last = mVoidLink->mPrevious;
    *object = (last == mVoidLink) ? NULL : last->mData;
    REFCOUNT_ADD(*object);
    return NOERROR;
}

ECode CLinkedList::PollFirst(
    /* [out] */ IInterface** object)
{
    if (mSize != 0) {
        return RemoveFirstImpl(object);
    }
    *object = NULL;
    return NOERROR;
}

ECode CLinkedList::PollLast(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);
    if (mSize != 0) {
        return RemoveLastImpl(object);
    }
    *object = NULL;
    return NOERROR;
}

ECode CLinkedList::Pop(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);
    return RemoveFirstImpl(object);
}

ECode CLinkedList::Push(
    /* [in] */ IInterface* object)
{
    Boolean flag = FALSE;
    return AddFirstImpl(object, &flag);
}

ECode CLinkedList::RemoveFirstOccurrence(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    return RemoveFirstOccurrenceImpl(object, result);
}

ECode CLinkedList::RemoveLastOccurrence(
   /* [in] */ IInterface* object,
   /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IIterator> iter = new ReverseLinkIterator(this);
    return RemoveOneOccurrence(object, iter, result);
}

ECode CLinkedList::RemoveFirstOccurrenceImpl(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IIterator> iter = new LinkIterator(this, 0);
    return RemoveOneOccurrence(object, iter, result);
}

ECode CLinkedList::RemoveOneOccurrence(
    /* [in] */ IInterface* object,
    /* [in] */ IIterator* iter,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean hasnext = FALSE;
    while ((iter->HasNext(&hasnext), hasnext)) {
        AutoPtr<IInterface> element;
        iter->GetNext((IInterface**)&element);
        if (object == NULL ? element == NULL : Object::Equals(object, element)) {
            iter->Remove();
            *result = TRUE;
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode CLinkedList::Set(
    /* [in] */ Int32 location,
    /* [in] */ IInterface* object,
    /* [out] */ IInterface** prevObject)
{
    VALIDATE_NOT_NULL(prevObject);
    if (location >= 0 && location < mSize) {
        AutoPtr<Link> link = mVoidLink;
        if (location < (mSize / 2)) {
            for (Int32 i = 0; i <= location; ++i) {
                link = link->mNext;
            }
        }
        else {
            for (Int32 i = mSize; i > location; i--) {
                link = link->mPrevious;
            }
        }
        *prevObject = link->mData;
        REFCOUNT_ADD(*prevObject);
        link->mData = object;
        return NOERROR;
    }
    *prevObject = NULL;
    return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
}

ECode CLinkedList::GetSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size);
    *size = mSize;
    return NOERROR;
}

ECode CLinkedList::Offer(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    return AddLastImpl(object, result);
}

ECode CLinkedList::Poll(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);
    if (mSize != 0) {
        return RemoveFirst(object);
    }
    *object = NULL;
    return NOERROR;
}

ECode CLinkedList::Remove(
    /* [out] */ IInterface** object)
{
    return RemoveFirstImpl(object);
}

ECode CLinkedList::Peek(
    /* [out] */ IInterface** object)
{
    return PeekFirstImpl(object);
}

ECode CLinkedList::PeekFirstImpl(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)
    AutoPtr<Link> first = mVoidLink->mNext;
    if (first != mVoidLink) {
        *object = first->mData;
        REFCOUNT_ADD(*object);
        return NOERROR;
    }
    *object = NULL;
    return NOERROR;
}

ECode CLinkedList::GetElement(
    /* [out] */ IInterface** object)
{
    return GetFirstImpl(object);
}

ECode CLinkedList::ToArray(
    /* [out, callee] */ ArrayOf<IInterface*>** array)
{
    VALIDATE_NOT_NULL(array);
    Int32 index = 0;
    AutoPtr<ArrayOf<IInterface*> > contents = ArrayOf<IInterface*>::Alloc(mSize);
    AutoPtr<Link> link = mVoidLink->mNext;
    while (link != mVoidLink) {
        contents->Set(index++, link->mData);
        link = link->mNext;
    }
    *array = contents;
    REFCOUNT_ADD(*array)
    return NOERROR;
}

ECode CLinkedList::ToArray(
    /* [in] */ ArrayOf<IInterface*>* contents,
    /* [out, callee] */ ArrayOf<IInterface*>** outArray)
{
    VALIDATE_NOT_NULL(outArray);
    *outArray = NULL;
    VALIDATE_NOT_NULL(contents);

    Int32 index = 0;
    Int32 size = contents->GetLength();
    if (mSize > size) {
        contents = ArrayOf<IInterface*>::Alloc(mSize);
    }
    AutoPtr<Link> link = mVoidLink->mNext;
    while (link != mVoidLink) {
        contents->Set(index++, link->mData);
        link = link->mNext;
    }
    Int32 sizeContent = contents->GetLength();
    if (index < sizeContent) {
        contents->Set(index, NULL);
    }
    *outArray = contents;
    REFCOUNT_ADD(*outArray)
    return NOERROR;
}

ECode CLinkedList::WriteObject(
    /* [in] */ IObjectOutputStream* stream)
{
    assert(0);
    // VALIDATE_NOT_NULL(stream);
    // stream->DefaultWriteObject();
    // AutoPtr<IObjectOutput> outputstream;
    // //need CObjectOutputStream implement
    // assert(0 && "TODO");
    // // outputstream = (CObjectOutputStream*)stream->Probe(EIID_IObjectOutput);
    // (IDataOutput::Probe(outputstream))->WriteInt32(mSize);
    // AutoPtr<IIterator> it;
    // GetIterator((IIterator**)&it);
    // Boolean flag = FALSE;
    // while ((it->HasNext(&flag), flag)) {
    //     AutoPtr<IInterface> nextobject;
    //     it->GetNext((IInterface**)&nextobject);
    //     outputstream->WriteObject(nextobject);
    // }
    return NOERROR;
}

ECode CLinkedList::ReadObject(
    /* [in] */ IObjectInputStream* stream)
{
    assert(0);
    // VALIDATE_NOT_NULL(stream);
    // stream->DefaultReadObject();
    // AutoPtr<IObjectInput> inputstream;
    // // need CObjectInputStream implement
    // assert(0 && "TODO");
    // // inputstream = (CObjectInputStream*)stream->Probe(EIID_IObjectInput);
    // Int32 result;
    // (IDataInput::Probe(inputstream))->ReadInt32(&result);
    // mVoidLink = new Link(NULL, NULL, NULL);
    // AutoPtr<Link> link = mVoidLink;
    // for (Int32 i = mSize; --i >= 0;) {
    //     AutoPtr<IInterface> object;
    //     inputstream->ReadObject((IInterface**)&object);
    //     AutoPtr<Link> nextLink = new Link(object, link, NULL);
    //     link->mNext = nextLink;
    //     link = nextLink;
    // }
    // link->mNext = mVoidLink;
    // mVoidLink->mPrevious = link;
    return NOERROR;
}

ECode CLinkedList::Element(
        /* [out] */ IInterface** e)
{
    return GetFirstImpl(e);
}


ECode CLinkedList::GetIterator(
    /* [in] */ IIterator** it)
{
    return AbstractSequentialList::GetIterator(it);
}

ECode CLinkedList::Add(
    /* [in] */ IInterface* object)
{
    return AbstractSequentialList::Add(object);
}

ECode CLinkedList::AddAll(
    /* [in] */ ICollection* collection)
{
    return AbstractSequentialList::AddAll(collection);
}

ECode CLinkedList::ContainsAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    return AbstractSequentialList::ContainsAll(collection, result);
}

ECode CLinkedList::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    return AbstractSequentialList::Equals(object, result);
}

ECode CLinkedList::GetHashCode(
    /* [out] */ Int32* result)
{
    return AbstractSequentialList::GetHashCode(result);
}

ECode CLinkedList::IsEmpty(
    /* [out] */ Boolean* result)
{
    return AbstractSequentialList::IsEmpty(result);
}

ECode CLinkedList::Remove(
    /* [in] */ IInterface* object)
{
    return AbstractSequentialList::Remove(object);
}

ECode CLinkedList::RemoveAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    return AbstractSequentialList::RemoveAll(collection, result);
}

ECode CLinkedList::RemoveAll(
    /* [in] */ ICollection* collection)
{
    return AbstractSequentialList::RemoveAll(collection);
}

ECode CLinkedList::RetainAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    return AbstractSequentialList::RetainAll(collection, result);
}

ECode CLinkedList::RetainAll(
    /* [in] */ ICollection* collection)
{
    return AbstractSequentialList::RetainAll(collection);
}

ECode CLinkedList::AddAll(
    /* [in] */ Int32 index,
    /* [in] */ ICollection* collection)
{
    return AbstractSequentialList::AddAll(index, collection);
}

ECode CLinkedList::GetListIterator(
    /* [in] */ IListIterator** it)
{
    return AbstractSequentialList::GetListIterator(it);
}

ECode CLinkedList::Remove(
    /* [in] */ Int32 index)
{
    return AbstractSequentialList::Remove(index);
}

ECode CLinkedList::Set(
    /* [in] */ Int32 index,
    /* [in] */ IInterface* object)
{
    return AbstractSequentialList::Set(index, object);
}

ECode CLinkedList::GetSubList(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ IList** list)
{
    return AbstractSequentialList::GetSubList(start, end, list);
}



} // Utility
} // Elastos
