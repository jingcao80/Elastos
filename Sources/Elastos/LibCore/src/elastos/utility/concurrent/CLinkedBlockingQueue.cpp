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

#include "CLinkedBlockingQueue.h"
#include "CAtomicInteger32.h"
#include "CReentrantLock.h"
#include "Math.h"

using Elastos::IO::EIID_ISerializable;
using Elastos::Utility::Concurrent::Atomic::CAtomicInteger32;
using Elastos::Utility::Concurrent::Locks::CReentrantLock;
using Elastos::Utility::Concurrent::Locks::ILock;

namespace Elastos {
namespace Utility {
namespace Concurrent {

//====================================================================
// CLinkedBlockingQueue::Itr::
//====================================================================
CAR_INTERFACE_IMPL(CLinkedBlockingQueue::Itr, Object, IIterator)

CLinkedBlockingQueue::Itr::Itr(
    /* [in] */ CLinkedBlockingQueue* owner)
    : mOwner(owner)
{
    mOwner->FullyLock();
    mCurrent = mOwner->mHead->mNext;
    if (mCurrent != NULL) {
        mCurrentElement = mCurrent->mItem;
    }
    mOwner->FullyUnlock();
}

ECode CLinkedBlockingQueue::Itr::HasNext(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mCurrent != NULL;
    return NOERROR;
}

AutoPtr<CLinkedBlockingQueue::Node> CLinkedBlockingQueue::Itr::NextNode(
    /* [in] */ CLinkedBlockingQueue::Node* p)
{
    for (;;) {
        AutoPtr<Node> s = p->mNext;
        if (s.Get() == p) {
            return mOwner->mHead->mNext;
        }
        if (s == NULL || s->mItem != NULL) {
            return s;
        }
        p = s;
    }
}

ECode CLinkedBlockingQueue::Itr::GetNext(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);
    mOwner->FullyLock();
    if (mCurrent == NULL) {
        mOwner->FullyUnlock();
        return E_NO_SUCH_ELEMENT_EXCEPTION;
    }
    AutoPtr<IInterface> x = mCurrentElement;
    mLastRet = mCurrent;
    mCurrent = NextNode(mCurrent);
    mCurrentElement = (mCurrent == NULL) ? NULL : mCurrent->mItem;
    mOwner->FullyUnlock();
    *object = x;
    REFCOUNT_ADD(*object);
    return NOERROR;
}

ECode CLinkedBlockingQueue::Itr::Remove()
{
    if (mLastRet == NULL) return E_ILLEGAL_STATE_EXCEPTION;
    mOwner->FullyLock();
    AutoPtr<Node> node = mLastRet;
    mLastRet = NULL;
    for (AutoPtr<Node> trail = mOwner->mHead, p = trail->mNext;
        p != NULL;
        trail = p, p = p->mNext) {
        if (p == node) {
            mOwner->Unlink(p, trail);
            break;
        }
    }
    mOwner->FullyUnlock();
    return NOERROR;
}

//====================================================================
// CLinkedBlockingQueue::
//====================================================================
CAR_INTERFACE_IMPL_2(CLinkedBlockingQueue, AbstractQueue, IBlockingQueue, ISerializable)

CAR_OBJECT_IMPL(CLinkedBlockingQueue)

CLinkedBlockingQueue::CLinkedBlockingQueue()
    : mCapacity(0)
{
    ASSERT_SUCCEEDED(CAtomicInteger32::New((IAtomicInteger32**)&mCount));
    CReentrantLock::New((IReentrantLock**)&mTakeLock);
    ILock::Probe(mTakeLock)->NewCondition((ICondition**)&mNotEmpty);
    CReentrantLock::New((IReentrantLock**)&mPutLock);
    ILock::Probe(mPutLock)->NewCondition((ICondition**)&mNotFull);
}

ECode CLinkedBlockingQueue::constructor()
{
    return constructor(Elastos::Core::Math::INT32_MAX_VALUE);
}

ECode CLinkedBlockingQueue::constructor(
    /* [in] */ Int32 capacity)
{
    if (capacity <= 0) return E_ILLEGAL_ARGUMENT_EXCEPTION;
    mCapacity = capacity;
    mLast = mHead = new Node(NULL);
    return NOERROR;
}

ECode CLinkedBlockingQueue::constructor(
    /* [in] */ ICollection* c)
{
    FAIL_RETURN(constructor(Elastos::Core::Math::INT32_MAX_VALUE));
    AutoPtr<IReentrantLock> putLock = mPutLock;
    ILock::Probe(putLock)->Lock(); // Never contended, but necessary for visibility
    Int32 n = 0;

    AutoPtr<IIterator> it;
    c->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        if (obj == NULL) {
            ILock::Probe(putLock)->UnLock();
            return E_NULL_POINTER_EXCEPTION;
        }
        if (n == mCapacity) {
            ILock::Probe(putLock)->UnLock();
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        Enqueue(new Node(obj));
        ++n;
    }
    mCount->Set(n);
    ILock::Probe(putLock)->UnLock();
    return NOERROR;
}

void CLinkedBlockingQueue::SignalNotEmpty()
{
    AutoPtr<IReentrantLock> takeLock = mTakeLock;
    ILock::Probe(takeLock)->Lock();
    mNotEmpty->Signal();
    ILock::Probe(takeLock)->UnLock();
}

void CLinkedBlockingQueue::SignalNotFull()
{
    AutoPtr<IReentrantLock> putLock = mPutLock;
    ILock::Probe(putLock)->Lock();
    mNotFull->Signal();
    ILock::Probe(putLock)->UnLock();
}

void CLinkedBlockingQueue::Enqueue(
    /* [in] */ Node* node)
{
    // assert putLock.isHeldByCurrentThread();
    // assert last.next == null;
    mLast = mLast->mNext = node;
}

AutoPtr<IInterface> CLinkedBlockingQueue::Dequeue()
{
    // assert takeLock.isHeldByCurrentThread();
    // assert head.item == null;
    AutoPtr<Node> h = mHead;
    AutoPtr<Node> first = h->mNext;
    h->mNext = NULL; // help GC
    mHead = first;
    AutoPtr<IInterface> x = first->mItem;
    first->mItem = NULL;
    return x;
}

void CLinkedBlockingQueue::FullyLock()
{
    ILock::Probe(mPutLock)->Lock();
    ILock::Probe(mTakeLock)->Lock();
}

void CLinkedBlockingQueue::FullyUnlock()
{
    ILock::Probe(mTakeLock)->UnLock();
    ILock::Probe(mPutLock)->UnLock();
}

ECode CLinkedBlockingQueue::GetSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size);
    return mCount->Get(size);
}

ECode CLinkedBlockingQueue::RemainingCapacity(
    /* [out] */ Int32* capacity)
{
    VALIDATE_NOT_NULL(capacity);
    Int32 c = -1;
    mCount->Get(&c);
    *capacity = mCapacity - c;
    return NOERROR;
}

ECode CLinkedBlockingQueue::Put(
    /* [in] */ IInterface* e)
{
    if (e == NULL) return E_NULL_POINTER_EXCEPTION;
    // Note: convention in all put/take/etc is to preset local var
    // holding count negative to indicate failure unless set.
    Int32 c = -1;
    AutoPtr<Node> node = new Node(e);
    {
        AutoPtr<IReentrantLock> putLock = mPutLock;
        ILock::Probe(putLock)->LockInterruptibly();
        /*
         * Note that count is used in wait guard even though it is
         * not protected by lock. This works because count can
         * only decrease at this point (all other puts are shut
         * out by lock), and we (or some other waiting put) are
         * signalled if it ever changes from capacity. Similarly
         * for all other uses of count in other wait guards.
         */
        while (mCount->Get(&c), c == mCapacity) {
            mNotFull->Await();
        }
        Enqueue(node);
        mCount->GetAndIncrement(&c);
        if (c + 1 < mCapacity) {
            mNotFull->Signal();
        }
        ILock::Probe(putLock)->UnLock();
    }
    if (c == 0) {
        SignalNotEmpty();
    }
    return NOERROR;
}

ECode CLinkedBlockingQueue::Offer(
    /* [in] */ IInterface* e,
    /* [in] */ Int64 timeout,
    /* [in] */ ITimeUnit* unit,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    if (e == NULL) return E_NULL_POINTER_EXCEPTION;

    Int64 nanos;
    unit->ToNanos(timeout, &nanos);
    Int32 c = -1;
    {
        AutoPtr<IReentrantLock> putLock = mPutLock;
        ILock::Probe(putLock)->LockInterruptibly();
        while (mCount->Get(&c), c == mCapacity) {
            if (nanos <= 0) {
                *result = FALSE;
                return NOERROR;
            }
            mNotFull->AwaitNanos(nanos, &nanos);
        }
        Enqueue(new Node(e));
        mCount->GetAndIncrement(&c);
        if (c + 1 < mCapacity) {
            mNotFull->Signal();
        }
        ILock::Probe(putLock)->UnLock();
    }
    if (c == 0) {
        SignalNotEmpty();
    }
    *result = TRUE;
    return NOERROR;
}

ECode CLinkedBlockingQueue::Offer(
    /* [in] */ IInterface* e,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    if (e == NULL) return E_NULL_POINTER_EXCEPTION;

    Int32 c = -1;
    if (mCount->Get(&c), c == mCapacity) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<Node> node = new Node(e);
    {
        AutoPtr<IReentrantLock> putLock = mPutLock;
        ILock::Probe(putLock)->LockInterruptibly();
        if (mCount->Get(&c), c < mCapacity) {
            Enqueue(node);
            mCount->GetAndIncrement(&c);
            if (c + 1 < mCapacity) {
                mNotFull->Signal();
            }
        }
        ILock::Probe(putLock)->UnLock();
    }
    if (c == 0) {
        SignalNotEmpty();
    }
    *result = c >= 0;
    return NOERROR;
}

ECode CLinkedBlockingQueue::Take(
    /* [out] */ IInterface** e)
{
    VALIDATE_NOT_NULL(e)
    AutoPtr<IInterface> x;
    Int32 c = -1;
    {
        AutoPtr<IReentrantLock> takeLock = mTakeLock;
        ILock::Probe(takeLock)->LockInterruptibly();
        while (mCount->Get(&c), c == 0) {
            mNotEmpty->Await();
        }
        x = Dequeue();
        mCount->GetAndDecrement(&c);
        if (c > 1) {
            mNotEmpty->Signal();
        }
        ILock::Probe(takeLock)->UnLock();
    }
    if (c == mCapacity) {
        SignalNotFull();
    }
    *e = x;
    REFCOUNT_ADD(*e);
    return NOERROR;
}

ECode CLinkedBlockingQueue::Poll(
    /* [in] */ Int64 timeout,
    /* [in] */ ITimeUnit* unit,
    /* [out] */ IInterface** e)
{
    VALIDATE_NOT_NULL(e);
    AutoPtr<IInterface> x;
    Int64 nanos;
    unit->ToNanos(timeout, &nanos);
    Int32 c = -1;
    {
        AutoPtr<IReentrantLock> takeLock = mTakeLock;
        ILock::Probe(takeLock)->LockInterruptibly();
        while (mCount->Get(&c), c == 0) {
            if (nanos <= 0) {
                *e = NULL;
                return NOERROR;
            }
            mNotEmpty->AwaitNanos(nanos, &nanos);
        }
        x = Dequeue();
        mCount->GetAndDecrement(&c);
        if (c > 1) {
            mNotEmpty->Signal();
        }
        ILock::Probe(takeLock)->UnLock();
    }
    if (c == mCapacity) {
        SignalNotFull();
    }
    *e = x;
    REFCOUNT_ADD(*e);
    return NOERROR;
}

ECode CLinkedBlockingQueue::Poll(
    /* [out] */ IInterface** e)
{
    VALIDATE_NOT_NULL(e);
    Int32 c = -1;
    if (mCount->Get(&c), c == 0) {
        *e = NULL;
        return NOERROR;
    }
    AutoPtr<IInterface> x;
    {
        AutoPtr<IReentrantLock> takeLock = mTakeLock;
        ILock::Probe(takeLock)->Lock();
        if (mCount->Get(&c), c > 0) {
            x = Dequeue();
            mCount->GetAndDecrement(&c);
            if (c > 1) {
                mNotEmpty->Signal();
            }
        }
        ILock::Probe(takeLock)->UnLock();
    }
    if (c == mCapacity) {
        SignalNotFull();
    }
    *e = x;
    REFCOUNT_ADD(*e);
    return NOERROR;
}

ECode CLinkedBlockingQueue::Peek(
    /* [out] */ IInterface** e)
{
    VALIDATE_NOT_NULL(e);
    Int32 c = -1;
    if (mCount->Get(&c), c == 0) {
        *e = NULL;
        return NOERROR;
    }
    {
        AutoPtr<IReentrantLock> takeLock = mTakeLock;
        ILock::Probe(takeLock)->Lock();
        AutoPtr<Node> first = mHead->mNext;
        if (first == NULL) {
            *e = NULL;
            ILock::Probe(takeLock)->UnLock();
            return NOERROR;
        }
        else {
            *e = first->mItem;
            REFCOUNT_ADD(*e);
            ILock::Probe(takeLock)->UnLock();
            return NOERROR;
        }
    }
}

void CLinkedBlockingQueue::Unlink(
    /* [in] */ Node* p,
    /* [in] */ Node* trail)
{
    // assert isFullyLocked();
    // p.next is not changed, to allow iterators that are
    // traversing p to maintain their weak-consistency guarantee.
    p->mItem = NULL;
    trail->mNext = p->mNext;
    if (mLast.Get() == p) {
        mLast = trail;
    }
    Int32 c = -1;
    if (mCount->GetAndDecrement(&c), c == mCapacity) {
        mNotFull->Signal();
    }
}

ECode CLinkedBlockingQueue::Remove(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* modified)
{
    VALIDATE_NOT_NULL(modified);
    if (o == NULL) {
        *modified = FALSE;
        return NOERROR;
    }
    FullyLock();
    for (AutoPtr<Node> trail = mHead, p = trail->mNext;
        p != NULL;
        trail = p, p = p->mNext) {
        if (Object::Equals(o, p->mItem)) {
            Unlink(p, trail);
            FullyUnlock();
            *modified = TRUE;
            return NOERROR;
        }
    }
    FullyUnlock();
    *modified = FALSE;
    return NOERROR;
}

ECode CLinkedBlockingQueue::Contains(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (object == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    FullyLock();
    for (AutoPtr<Node> p = mHead->mNext; p != NULL; p = p->mNext) {
        if (Object::Equals(object, p->mItem)) {
            FullyUnlock();
            *result = TRUE;
            return NOERROR;
        }
    }
    FullyUnlock();
    *result = FALSE;
    return NOERROR;
}

ECode CLinkedBlockingQueue::ToArray(
    /* [out, callee] */ ArrayOf<IInterface*>** array)
{
    VALIDATE_NOT_NULL(array);
    FullyLock();
    Int32 size;
    mCount->Get(&size);
    AutoPtr< ArrayOf<IInterface*> > a = ArrayOf<IInterface*>::Alloc(size);
    Int32 k = 0;
    for (AutoPtr<Node> p = mHead->mNext; p != NULL; p = p->mNext) {
        a->Set(k++, p->mItem);
    }
    FullyUnlock();
    *array = a;
    REFCOUNT_ADD(*array);
    return NOERROR;
}

ECode CLinkedBlockingQueue::ToArray(
    /* [in] */ ArrayOf<IInterface*>* inArray,
    /* [out, callee] */ ArrayOf<IInterface*>** outArray)
{
    VALIDATE_NOT_NULL(outArray);
    FullyLock();
    AutoPtr< ArrayOf<IInterface*> > a = inArray;
    Int32 size;
    mCount->Get(&size);
    if (a->GetLength() < size) {
        a = ArrayOf<IInterface*>::Alloc(size);
    }

    Int32 k = 0;
    for (AutoPtr<Node> p = mHead->mNext; p != NULL; p = p->mNext) {
        a->Set(k++, p->mItem);
    }
    if (a->GetLength() > k) {
        a->Set(k, NULL);
    }
    FullyUnlock();
    *outArray = a;
    REFCOUNT_ADD(*outArray);
    return NOERROR;
}

ECode CLinkedBlockingQueue::Clear()
{
    FullyLock();
    for (AutoPtr<Node> p, h = mHead; (p = h->mNext) != NULL; h = p) {
        h->mNext = NULL;
        p->mItem = NULL;
    }
    mHead = mLast;
    // assert head.item == null && head.next == null;
    Int32 c = 0;
    if (mCount->GetAndSet(0, &c), c == mCapacity) {
        mNotFull->Signal();
    }
    FullyUnlock();
    return NOERROR;
}

ECode CLinkedBlockingQueue::DrainTo(
    /* [in] */ ICollection* c,
    /* [out] */ Int32* number)
{
    return DrainTo(c, Elastos::Core::Math::INT32_MAX_VALUE, number);
}

ECode CLinkedBlockingQueue::DrainTo(
    /* [in] */ ICollection* c,
    /* [in] */ Int32 maxElements,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number);
    if (c == NULL) return E_NULL_POINTER_EXCEPTION;
    if (c == ICollection::Probe(this)) return E_ILLEGAL_ARGUMENT_EXCEPTION;
    if (maxElements <= 0) {
        *number = 0;
        return NOERROR;
    }
    Boolean signalNotFull = FALSE;
    ILock::Probe(mTakeLock)->Lock();
    Int32 size;
    mCount->Get(&size);
    Int32 n = Elastos::Core::Math::Min(maxElements, size);
    // count.get provides visibility to first n Nodes
    AutoPtr<Node> h = mHead;
    Int32 i = 0;
    while (i < n) {
        AutoPtr<Node> p = h->mNext;
        Boolean result;
        c->Add(p->mItem, &result);
        p->mItem = NULL;
        h->mNext = NULL;
        h = p;
        ++i;
    }
    // Restore invariants even if c.add() threw
    if (i > 0) {
        // assert h.item == null;
        mHead = h;
        signalNotFull = (mCount->GetAndAdd(-i, &size), size == mCapacity);
    }
    ILock::Probe(mTakeLock)->UnLock();
    if (signalNotFull) {
        SignalNotFull();
    }
    *number = n;
    return NOERROR;
}

ECode CLinkedBlockingQueue::GetIterator(
    /* [out] */ IIterator** it)
{
    VALIDATE_NOT_NULL(it);
    *it = (IIterator*)new Itr(this);
    REFCOUNT_ADD(*it);
    return NOERROR;
}

ECode CLinkedBlockingQueue::Add(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    FAIL_RETURN(Offer(object, modified));
    if (*modified == TRUE) return NOERROR;
    else return E_ILLEGAL_STATE_EXCEPTION;
}

ECode CLinkedBlockingQueue::Remove(
    /* [out] */ IInterface** e)
{
    FAIL_RETURN(Poll(e));
    if (*e != NULL) return NOERROR;
    else return E_NO_SUCH_ELEMENT_EXCEPTION;
}

ECode CLinkedBlockingQueue::GetElement(
    /* [out] */ IInterface** e)
{
    FAIL_RETURN(Peek(e));
    if (*e != NULL) return NOERROR;
    else return E_NO_SUCH_ELEMENT_EXCEPTION;
}

ECode CLinkedBlockingQueue::AddAll(
    /* [in] */ ICollection* c,
    /* [out] */ Boolean* modified)
{
    VALIDATE_NOT_NULL(modified);
    if (c == NULL) return E_NULL_POINTER_EXCEPTION;
    if (c == ICollection::Probe(this)) return E_ILLEGAL_ARGUMENT_EXCEPTION;
    *modified = FALSE;

    AutoPtr<IIterator> it;
    c->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        Boolean result = FALSE;
        if (Add(obj, &result), result) *modified = TRUE;
    }
    return NOERROR;
}

ECode CLinkedBlockingQueue::ContainsAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IIterator> it;
    collection->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> e;
        it->GetNext((IInterface**)&e);
        Boolean contains;
        if (Contains(e, &contains), !contains) {
            *result = FALSE;
            return NOERROR;
        }
    }
    *result = TRUE;
    return NOERROR;
}

ECode CLinkedBlockingQueue::IsEmpty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 size;
    GetSize(&size);
    *result = size == 0;
    return NOERROR;
}

ECode CLinkedBlockingQueue::RemoveAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    VALIDATE_NOT_NULL(modified);
    *modified = FALSE;
    AutoPtr<IIterator> it;
    GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> e;
        it->GetNext((IInterface**)&e);
        Boolean contains;
        if (collection->Contains(e, &contains), contains) {
            it->Remove();
            *modified = TRUE;
        }
    }
    return NOERROR;
}

ECode CLinkedBlockingQueue::RetainAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    VALIDATE_NOT_NULL(modified)
    *modified = FALSE;
    AutoPtr<IIterator> it;
    GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> e;
        it->GetNext((IInterface**)&e);
        Boolean contains;
        if (collection->Contains(e, &contains), !contains) {
            it->Remove();
            *modified = TRUE;
        }
    }
    return NOERROR;
}

ECode CLinkedBlockingQueue::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    return AbstractQueue::Equals(object, result);
}

ECode CLinkedBlockingQueue::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    return AbstractQueue::GetHashCode(hashCode);
}

} // namespace Concurrent
} // namespace Utility
} // namespace Elastos
