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
#include "CPriorityBlockingQueue.h"
#include "Math.h"
#include "CReentrantLock.h"
#include "Thread.h"
#include "Arrays.h"
#include "Thread.h"
#include "CPriorityQueue.h"

using Elastos::Core::Thread;
using Elastos::IO::EIID_ISerializable;
using Elastos::Utility::CPriorityQueue;
using Elastos::Utility::Concurrent::Locks::ILock;
using Elastos::Utility::Concurrent::Locks::CReentrantLock;

namespace Elastos {
namespace Utility {
namespace Concurrent {

Int64 CPriorityBlockingQueue::mSerialVersionUID = 5595510919245408276L;

Int32 CPriorityBlockingQueue::DEFAULT_INITIAL_CAPACITY = 11;

Int32 CPriorityBlockingQueue::MAX_ARRAY_SIZE = Elastos::Core::Math::INT32_MAX_VALUE - 8;

//===============================================================================
// CPriorityBlockingQueue::
//===============================================================================
CAR_INTERFACE_IMPL_2(CPriorityBlockingQueue, AbstractQueue, IBlockingQueue, ISerializable)

CAR_OBJECT_IMPL(CPriorityBlockingQueue)

ECode CPriorityBlockingQueue::constructor()
{
    return constructor(DEFAULT_INITIAL_CAPACITY, NULL);
}

ECode CPriorityBlockingQueue::constructor(
    /* [in] */ Int32 initialCapacity)
{
    return constructor(initialCapacity, NULL);
}

ECode CPriorityBlockingQueue::constructor(
    /* [in] */ Int32 initialCapacity,
    /* [in] */ IComparator* comparator)
{
    if (initialCapacity < 1)
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    CReentrantLock::New((IReentrantLock**)&mLock);
    ILock::Probe(mLock)->NewCondition((ICondition**)&mNotEmpty);
    mComparator = comparator;
    mQueue = ArrayOf<IInterface*>::Alloc(initialCapacity);
    return NOERROR;
}

ECode CPriorityBlockingQueue::constructor(
    /* [in] */ ICollection* c)
{
    CReentrantLock::New((IReentrantLock**)&mLock);
    ILock::Probe(mLock)->NewCondition((ICondition**)&mNotEmpty);
    Boolean heapify = TRUE; // true if not known to be in heap order
    Boolean screen = TRUE;  // true if must screen for nulls
    if (c->Probe(EIID_ISortedSet) != NULL) {
        AutoPtr<ISortedSet> ss = (ISortedSet*) (c->Probe(EIID_ISortedSet));
        ss->GetComparator((IComparator**)&mComparator);
        heapify = FALSE;
    }
    else if (c->Probe(EIID_IPriorityBlockingQueue) != NULL) {
        AutoPtr<IPriorityBlockingQueue> pq =
            (IPriorityBlockingQueue*) (c->Probe(EIID_IPriorityBlockingQueue));
        pq->GetComparator((IComparator**)&mComparator);
        screen = FALSE;
        // if (pq->GetClass() == PriorityBlockingQueue.class) // exact match
        //     heapify = FALSE;
    }
    AutoPtr<ArrayOf<IInterface*> > a;
    c->ToArray((ArrayOf<IInterface*>**)&a);
    Int32 n = a->GetLength();
    // If c.toArray incorrectly doesn't return Object[], copy it.
    // if (a.getClass() != Object[].class)
    //     a = Arrays.copyOf(a, n, Object[].class);
    if (screen && (n == 1 || mComparator != NULL)) {
        for (Int32 i = 0; i < n; ++i)
            if ((*a)[i] == NULL)
                return E_NULL_POINTER_EXCEPTION;
    }
    mQueue = a;
    mSize = n;
    if (heapify)
        Heapify();
    return NOERROR;
}

void CPriorityBlockingQueue::TryGrow(
    /* [in] */ ArrayOf<IInterface*>* array,
    /* [in] */ Int32 oldCap)
{
    ILock::Probe(mLock)->UnLock(); // must release and then re-acquire main lock
    AutoPtr<ArrayOf<IInterface*> > newArray = NULL;
    // if (mAllocationSpinLock == 0 &&
    //     UNSAFE.compareAndSwapInt(this, allocationSpinLockOffset,
    //                              0, 1)) {
    //     try {
    //         int newCap = oldCap + ((oldCap < 64) ?
    //                                (oldCap + 2) : // grow faster if small
    //                                (oldCap >> 1));
    //         if (newCap - MAX_ARRAY_SIZE > 0) {    // possible overflow
    //             int minCap = oldCap + 1;
    //             if (minCap < 0 || minCap > MAX_ARRAY_SIZE)
    //                 throw new OutOfMemoryError();
    //             newCap = MAX_ARRAY_SIZE;
    //         }
    //         if (newCap > oldCap && queue == array)
    //             newArray = new Object[newCap];
    //     } finally {
    //         allocationSpinLock = 0;
    //     }
    // }
    if (newArray == NULL) // back off if another thread is allocating
        Thread::Yield();
    ILock::Probe(mLock)->Lock();
    if (newArray != NULL && mQueue.Get() == array) {
        mQueue = newArray;
        newArray->Copy(0, array, 0, oldCap);
    }
}

AutoPtr<IInterface> CPriorityBlockingQueue::Dequeue()
{
    Int32 n = mSize - 1;
    if (n < 0)
        return NULL;
    else {
        AutoPtr<ArrayOf<IInterface*> > array = mQueue;
        AutoPtr<IInterface> result = (*array)[0];
        AutoPtr<IInterface> x = (*array)[n];
        (*array)[n] = NULL;
        AutoPtr<IComparator> cmp = mComparator;
        if (cmp == NULL)
            SiftDownComparable(0, x, array, n);
        else
            SiftDownUsingComparator(0, x, array, n, cmp);
        mSize = n;
        return result;
    }
}

void CPriorityBlockingQueue::SiftUpComparable(
    /* [in] */ Int32 k,
    /* [in] */ IInterface* x,
    /* [in] */ ArrayOf<IInterface*>* array)
{
    AutoPtr<IComparable> key = (IComparable*) x->Probe(EIID_IComparable);
    while (k > 0) {
        Int32 parent = (k - 1) >> 1;
        AutoPtr<IInterface> e = (*array)[parent];
        Int32 res = 0;
        if ((key->CompareTo(e, &res), res) >= 0)
            break;
        (*array)[k] = e;
        k = parent;
    }
    (*array)[k] = key;
}

void CPriorityBlockingQueue::SiftUpUsingComparator(
    /* [in] */ Int32 k,
    /* [in] */ IInterface* x,
    /* [in] */ ArrayOf<IInterface*>* array,
    /* [in] */ IComparator* cmp)
{
    while (k > 0) {
        Int32 parent = (k - 1) >> 1;
        AutoPtr<IInterface> e = (*array)[parent];
        Int32 res = 0;
        if ((cmp->Compare(x, e, &res), res) >= 0)
            break;
        (*array)[k] = e;
        k = parent;
    }
    (*array)[k] = x;
}

void CPriorityBlockingQueue::SiftDownComparable(
    /* [in] */ Int32 k,
    /* [in] */ IInterface* x,
    /* [in] */ ArrayOf<IInterface*>* array,
    /* [in] */ Int32 n)
{
    if (n > 0) {
        AutoPtr<IComparable> key = (IComparable*) x->Probe(EIID_IComparable);
        Int32 half = n >> 1;           // loop while a non-leaf
        while (k < half) {
            Int32 child = (k << 1) + 1; // assume left child is least
            AutoPtr<IInterface> c = (*array)[child];
            Int32 right = child + 1;
            Int32 res = 0;
            if (right < n &&
                (IComparable::Probe(c)->CompareTo((*array)[right], &res), res) > 0)
                c = (*array)[child = right];
            Int32 kc = 0;
            if ((key->CompareTo(c, &kc), kc) <= 0)
                break;
            (*array)[k] = c;
            k = child;
        }
        (*array)[k] = key;
    }
}

void CPriorityBlockingQueue::SiftDownUsingComparator(
    /* [in] */ Int32 k,
    /* [in] */ IInterface* x,
    /* [in] */ ArrayOf<IInterface*>* array,
    /* [in] */ Int32 n,
    /* [in] */ IComparator* cmp)
{
    if (n > 0) {
        Int32 half = n >> 1;
        while (k < half) {
            Int32 child = (k << 1) + 1;
            AutoPtr<IInterface> c = (*array)[child];
            Int32 right = child + 1;
            Int32 res = 0;
            if (right < n && (cmp->Compare(c, (*array)[right], &res), res) > 0)
                c = (*array)[child = right];
            if ((cmp->Compare(x, c, &res), res) <= 0)
                break;
            (*array)[k] = c;
            k = child;
        }
        (*array)[k] = x;
    }
}

void CPriorityBlockingQueue::Heapify()
{
    AutoPtr<ArrayOf<IInterface*> > array = mQueue;
    Int32 n = mSize;
    Int32 half = (n >> 1) - 1;
    AutoPtr<IComparator> cmp = mComparator;
    if (cmp == NULL) {
        for (Int32 i = half; i >= 0; i--)
            SiftDownComparable(i, (*array)[i], array, n);
    }
    else {
        for (Int32 i = half; i >= 0; i--)
            SiftDownUsingComparator(i, (*array)[i], array, n, cmp);
    }
}

ECode CPriorityBlockingQueue::Add(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    VALIDATE_NOT_NULL(modified)

    return Offer(object, modified);
}

ECode CPriorityBlockingQueue::Offer(
    /* [in] */ IInterface* e,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (e == NULL)
        return E_NULL_POINTER_EXCEPTION;
    AutoPtr<IReentrantLock> lock = mLock;
    ILock::Probe(lock)->Lock();
    Int32 n, cap;
    AutoPtr<ArrayOf<IInterface*> > array;
    while ((n = mSize) >= (cap = (array = mQueue)->GetLength()))
        TryGrow(array, cap);
//    try {
        AutoPtr<IComparator> cmp = mComparator;
        if (cmp == NULL)
            SiftUpComparable(n, e, array);
        else
            SiftUpUsingComparator(n, e, array, cmp);
        mSize = n + 1;
        mNotEmpty->Signal();
//    } finally {
        ILock::Probe(lock)->UnLock();
//    }
    *result = TRUE;
    return NOERROR;
}

ECode CPriorityBlockingQueue::Put(
    /* [in] */ IInterface* e)
{
    Boolean res;
    return Offer(e, &res); // never need to block
}

ECode CPriorityBlockingQueue::Offer(
    /* [in] */ IInterface* e,
    /* [in] */ Int64 timeout,
    /* [in] */ ITimeUnit* unit,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    return Offer(e, result); // never need to block
}

ECode CPriorityBlockingQueue::Poll(
    /* [out] */ IInterface** e)
{
    VALIDATE_NOT_NULL(e)

    AutoPtr<IReentrantLock> lock = mLock;
    ILock::Probe(lock)->Lock();
    Dequeue();
    ILock::Probe(lock)->UnLock();
    return NOERROR;
}

ECode CPriorityBlockingQueue::Take(
    /* [out] */ IInterface** res)
{
    VALIDATE_NOT_NULL(res)

    AutoPtr<IReentrantLock> lock = mLock;
    ILock::Probe(lock)->LockInterruptibly();
    AutoPtr<IInterface> result;
    while ( (result = Dequeue()) == NULL)
        mNotEmpty->Await();
    ILock::Probe(lock)->UnLock();
    *res = result;
    REFCOUNT_ADD(*res)
    return NOERROR;
}

ECode CPriorityBlockingQueue::Poll(
    /* [in] */ Int64 timeout,
    /* [in] */ ITimeUnit* unit,
    /* [out] */ IInterface** e)
{
    VALIDATE_NOT_NULL(e)

    Int64 nanos;
    unit->ToNanos(timeout, &nanos);
    AutoPtr<IReentrantLock> lock = mLock;
    ILock::Probe(lock)->LockInterruptibly();
    AutoPtr<IInterface> result;
    while ( (result = Dequeue()) == NULL && nanos > 0)
        mNotEmpty->AwaitNanos(nanos, &nanos);
    ILock::Probe(lock)->UnLock();
    *e = result;
    REFCOUNT_ADD(*e)
    return NOERROR;
}

ECode CPriorityBlockingQueue::Peek(
    /* [out] */ IInterface** e)
{
    VALIDATE_NOT_NULL(e)

    AutoPtr<IReentrantLock> lock = mLock;
    ILock::Probe(lock)->Lock();
    if (mSize == 0)
        *e = NULL;
    else {
        *e = (*mQueue)[0];
        REFCOUNT_ADD(*e)
    }
    ILock::Probe(lock)->UnLock();
    return NOERROR;
}

ECode CPriorityBlockingQueue::GetComparator(
    /* [out] */ IComparator** outcom)
{
    VALIDATE_NOT_NULL(outcom)

    *outcom = mComparator;
    REFCOUNT_ADD(*outcom)
    return NOERROR;
}

ECode CPriorityBlockingQueue::GetSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size)

    AutoPtr<IReentrantLock> lock = mLock;
    ILock::Probe(lock)->Lock();
    *size = mSize;
    ILock::Probe(lock)->UnLock();
    return NOERROR;
}

ECode CPriorityBlockingQueue::RemainingCapacity(
    /* [out] */ Int32* capacity)
{
    VALIDATE_NOT_NULL(capacity)

    *capacity = Elastos::Core::Math::INT32_MAX_VALUE;
    return NOERROR;
}

Int32 CPriorityBlockingQueue::IndexOf(
    /* [in] */ IInterface* o)
{
    if (o != NULL) {
        AutoPtr<ArrayOf<IInterface*> > array = mQueue;
        Int32 n = mSize;
        for (Int32 i = 0; i < n; i++)
            if (Object::Equals(o, (*array)[i]))
                return i;
    }
    return -1;
}

void CPriorityBlockingQueue::RemoveAt(
    /* [in] */ Int32 i)
{
    AutoPtr<ArrayOf<IInterface*> > array = mQueue;
    Int32 n = mSize - 1;
    if (n == i) // removed last element
        (*array)[i] = NULL;
    else {
        AutoPtr<IInterface> moved = (*array)[n];
        (*array)[n] = NULL;
        AutoPtr<IComparator> cmp = mComparator;
        if (cmp == NULL)
            SiftDownComparable(i, moved, array, n);
        else
            SiftDownUsingComparator(i, moved, array, n, cmp);
        if (Object::Equals((*array)[i], moved)) {
            if (cmp == NULL)
                SiftUpComparable(i, moved, array);
            else
                SiftUpUsingComparator(i, moved, array, cmp);
        }
    }
    mSize = n;
}

ECode CPriorityBlockingQueue::Remove(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    VALIDATE_NOT_NULL(modified)

    AutoPtr<IReentrantLock> lock = mLock;
    ILock::Probe(lock)->Lock();
    Int32 i = IndexOf(object);
    if (i == -1) {
        *modified = FALSE;
        ILock::Probe(lock)->UnLock();
        return NOERROR;
    }
    RemoveAt(i);
    *modified = TRUE;
    ILock::Probe(lock)->UnLock();
    return NOERROR;

}

void CPriorityBlockingQueue::RemoveEQ(
    /* [in] */ IInterface* o)
{
    AutoPtr<IReentrantLock> lock = mLock;
    ILock::Probe(lock)->Lock();
    AutoPtr<ArrayOf<IInterface*> > array = mQueue;
    for (Int32 i = 0, n = mSize; i < n; i++) {
        if (Object::Equals(o, (*array)[i])) {
            RemoveAt(i);
            break;
        }
    }
    ILock::Probe(lock)->UnLock();
}

ECode CPriorityBlockingQueue::Contains(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IReentrantLock> lock = mLock;
    ILock::Probe(lock)->Lock();
    *result = IndexOf(object) != -1;
    ILock::Probe(lock)->UnLock();
    return NOERROR;
}

ECode CPriorityBlockingQueue::ToArray(
    /* [out, callee] */ ArrayOf<IInterface*>** array)
{
    VALIDATE_NOT_NULL(array)

    AutoPtr<IReentrantLock> lock = mLock;
    ILock::Probe(lock)->Lock();
    Arrays::CopyOf(mQueue, mSize, array);
    ILock::Probe(lock)->UnLock();
    return NOERROR;
}

ECode CPriorityBlockingQueue::ToString(
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res)

    AutoPtr<IReentrantLock> lock = mLock;
    ILock::Probe(lock)->Lock();
    Int32 n = mSize;
    if (n == 0) {
        *res = "[]";
        ILock::Probe(lock)->UnLock();
        return NOERROR;
    }
    StringBuilder sb;
    sb.AppendChar('[');
    for (Int32 i = 0; i < n; ++i) {
        AutoPtr<IInterface> e = (*mQueue)[i];
        if (Object::Equals(e, TO_IINTERFACE(this))) {
            sb.Append("(this Collection)");
        }
        else {
            sb.Append(e);
        }
        if (i != n - 1) {
            sb.AppendChar(',');
            sb.AppendChar(' ');
        }
    }
    sb.AppendChar(']');
    sb.ToString(res);
    ILock::Probe(lock)->UnLock();
    return NOERROR;
}

ECode CPriorityBlockingQueue::DrainTo(
    /* [in] */ ICollection* c,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number)

    return DrainTo(c, Elastos::Core::Math::INT32_MAX_VALUE, number);
}

ECode CPriorityBlockingQueue::DrainTo(
    /* [in] */ ICollection* c,
    /* [in] */ Int32 maxElements,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number)

    if (c == NULL)
        return E_NULL_POINTER_EXCEPTION;
    if (Object::Equals(c->Probe(EIID_IInterface), TO_IINTERFACE(this)))
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    if (maxElements <= 0) {
        *number = 0;
        return NOERROR;
    }
    AutoPtr<IReentrantLock> lock = mLock;
    ILock::Probe(lock)->Lock();
    Int32 n = Elastos::Core::Math::Min(mSize, maxElements);
    for (Int32 i = 0; i < n; i++) {
        c->Add((*mQueue)[0]); // In this order, in case add() throws.
        Dequeue();
    }
    *number = n;
    ILock::Probe(lock)->UnLock();
    return NOERROR;
}

ECode CPriorityBlockingQueue::Clear()
{
    AutoPtr<IReentrantLock> lock = mLock;
    ILock::Probe(lock)->Lock();
    AutoPtr<ArrayOf<IInterface*> > array = mQueue;
    Int32 n = mSize;
    mSize = 0;
    for (Int32 i = 0; i < n; i++)
        (*array)[i] = NULL;
    ILock::Probe(lock)->UnLock();
    return NOERROR;
}

ECode CPriorityBlockingQueue::ToArray(
    /* [in] */ ArrayOf<IInterface*>* inArray,
    /* [out, callee] */ ArrayOf<IInterface*>** outArray)
{
    VALIDATE_NOT_NULL(outArray)

    AutoPtr<IReentrantLock> lock = mLock;
    ILock::Probe(lock)->Lock();
    Int32 n = mSize;
    // if (inArray->GetLength() < n)
    //     // Make a new array of a's runtime type, but my contents:
    //     return (T[]) Arrays::CopyOf(mQueue, mSize, inArray->GetClass());
    inArray->Copy(0, mQueue, 0, n);
    if (inArray->GetLength() > n)
        (*inArray)[n] = NULL;
    *outArray = inArray;
    REFCOUNT_ADD(*outArray)
    ILock::Probe(lock)->UnLock();
    return NOERROR;
}

ECode CPriorityBlockingQueue::GetIterator(
    /* [out] */ IIterator** it)
{
    VALIDATE_NOT_NULL(it)

    AutoPtr<ArrayOf<IInterface*> > arr;
    ToArray((ArrayOf<IInterface*>**)&arr);
    AutoPtr<Itr> p = new Itr(arr, this);
    *it = IIterator::Probe(p);
    REFCOUNT_ADD(*it)
    return NOERROR;
}

void CPriorityBlockingQueue::WriteObject(
    /* [in] */ IObjectOutputStream* s)
{
    ILock::Probe(mLock)->Lock();
    // avoid zero capacity argument
    CPriorityQueue::New(Elastos::Core::Math::Max(mSize, 1), mComparator, (IPriorityQueue**)&mQ);
    mQ->AddAll(this);
    s->DefaultWriteObject();
    mQ = NULL;
    ILock::Probe(mLock)->UnLock();
}

void CPriorityBlockingQueue::ReadObject(
    /* [in] */ IObjectInputStream* s)
{
    s->DefaultReadObject();
    Int32 size;
    mQ->GetSize(&size);
    mQueue = ArrayOf<IInterface*>::Alloc(size);
    mQ->GetComparator((IComparator**)&mComparator);
    Boolean b = FALSE;
    AddAll(ICollection::Probe(mQ), &b);
    mQ = NULL;
}

//===============================================================================
// CPriorityBlockingQueue::Itr::
//===============================================================================
CAR_INTERFACE_IMPL(CPriorityBlockingQueue::Itr, Object, IIterator)

CPriorityBlockingQueue::Itr::Itr(
    /* [in] */ ArrayOf<IInterface*>* array,
    /* [in] */ CPriorityBlockingQueue* owner)
{
    mLastRet = -1;
    mArray = array;
    mOwner = owner;
}

ECode CPriorityBlockingQueue::Itr::HasNext(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mCursor < mArray->GetLength();
    return NOERROR;
}

ECode CPriorityBlockingQueue::Itr::GetNext(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)

    if (mCursor >= mArray->GetLength())
        return E_NO_SUCH_ELEMENT_EXCEPTION;
    mLastRet = mCursor;
    *object = (*mArray)[mCursor++];
    REFCOUNT_ADD(*object)
    return NOERROR;
}

ECode CPriorityBlockingQueue::Itr::Remove()
{
    if (mLastRet < 0)
        return E_ILLEGAL_STATE_EXCEPTION;
    mOwner->RemoveEQ((*mArray)[mLastRet]);
    mLastRet = -1;
    return NOERROR;
}

} // namespace Concurrent
} // namespace Utility
} // namespace Elastos
