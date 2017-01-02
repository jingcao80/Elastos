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

#include "CDelayQueue.h"
#include <Math.h>
#include "TimeUnit.h"
#include "Thread.h"

using Elastos::Core::Thread;
using Elastos::Core::Math;
using Elastos::Utility::Concurrent::Locks::ILock;

namespace Elastos {
namespace Utility {
namespace Concurrent {

//====================================================================
// CDelayQueue::
//====================================================================
CAR_INTERFACE_IMPL_2(CDelayQueue, AbstractQueue, IDelayQueue, IBlockingQueue)

CAR_OBJECT_IMPL(CDelayQueue)

ECode CDelayQueue::constructor()
{
    return NOERROR;
}


ECode CDelayQueue::constructor(
    /* [in] */ ICollection* c)
{
    Boolean b;
    return AbstractQueue::AddAll(c, &b);
}

ECode CDelayQueue::Add(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    VALIDATE_NOT_NULL(modified);
    return Offer(object, modified);
}

ECode CDelayQueue::Offer(
    /* [in] */ IInterface* e,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IReentrantLock> lock = mLock;
    (ILock::Probe(lock))->Lock();
    Boolean b = FALSE;
    (IQueue::Probe(mQ))->Offer(e, &b);
    AutoPtr<IInterface> p;
    (IQueue::Probe(mQ))->Peek((IInterface**)&p);
    if (Object::Equals(p.Get(), e)) {
        mLeader = NULL;
        mAvailable->Signal();
    }
    *result = TRUE;
    (ILock::Probe(lock))->UnLock();
    return NOERROR;
}

ECode CDelayQueue::Put(
    /* [in] */ IInterface* e)
{
    Boolean b = FALSE;
    return Offer(e, &b);
}

ECode CDelayQueue::Offer(
    /* [in] */ IInterface* e,
    /* [in] */ Int64 timeout,
    /* [in] */ ITimeUnit* unit,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return Offer(e, result);
}

ECode CDelayQueue::Poll(
    /* [out] */ IInterface** e)
{
    VALIDATE_NOT_NULL(e);
    *e = NULL;

    AutoPtr<IReentrantLock> lock = mLock;
    (ILock::Probe(lock))->Lock();

    ECode ec = NOERROR;
    AutoPtr<IInterface> first;
    (IQueue::Probe(mQ))->Peek((IInterface**)&first);
    if (first != NULL) {
        AutoPtr<ITimeUnit> tu = TimeUnit::GetNANOSECONDS();
        AutoPtr<IDelayed> d = IDelayed::Probe(first.Get());
        Int64 val = 0;
        d->GetDelay(tu, &val);
        if (val <= 0) {
            ec = (IQueue::Probe(mQ))->Poll(e);
        }
    }

    (ILock::Probe(lock))->UnLock();
    return ec;
}

ECode CDelayQueue::Take(
    /* [out] */ IInterface** e)
{
    VALIDATE_NOT_NULL(e);
    *e = NULL;

    AutoPtr<IReentrantLock> lock = mLock;
    (ILock::Probe(lock))->LockInterruptibly();

    for (;;) {
        AutoPtr<IInterface> first;
        (IQueue::Probe(mQ))->Peek((IInterface**)&first);
        if (first == NULL) {
            mAvailable->Await();
        }
        else {
            AutoPtr<ITimeUnit> tu = TimeUnit::GetNANOSECONDS();
            AutoPtr<IDelayed> d = IDelayed::Probe(first);
            Int64 delay = 0;
            d->GetDelay(tu, &delay);
            if (delay <= 0) {
                ECode ec = (IQueue::Probe(mQ))->Poll(e);
                (ILock::Probe(lock))->UnLock();
                return ec;
            }
            first = NULL;
            if (mLeader != NULL)
                mAvailable->Await();
            else {
                AutoPtr<IThread> thisThread = Thread::GetCurrentThread();
                mLeader = thisThread;
                Int64 val = 0;
                mAvailable->AwaitNanos(delay, &val);
                if (mLeader == thisThread ||
                    Object::Equals(mLeader->Probe(EIID_IInterface), thisThread->Probe(EIID_IInterface)))
                    mLeader = NULL;
            }
        }
    }
    AutoPtr<IInterface> p;
    if (mLeader == NULL && ((IQueue::Probe(mQ))->Peek((IInterface**)&p), p) != NULL)
        mAvailable->Signal();
    (ILock::Probe(lock))->UnLock();
    return NOERROR;
}

ECode CDelayQueue::Poll(
    /* [in] */ Int64 timeout,
    /* [in] */ ITimeUnit* unit,
    /* [out] */ IInterface** e)
{
    VALIDATE_NOT_NULL(e);
    *e = NULL;

    Int64 nanos;
    unit->ToNanos(timeout, &nanos);
    AutoPtr<IReentrantLock> lock = mLock;
    (ILock::Probe(lock))->LockInterruptibly();
    for (;;) {
        AutoPtr<IInterface> first;
        (IQueue::Probe(mQ))->Peek((IInterface**)&first);
        if (first == NULL) {
            if (nanos <= 0) {
                (ILock::Probe(lock))->UnLock();
                return NOERROR;
            }
            else {
                Int64 val;
                mAvailable->AwaitNanos(nanos, &val);
                nanos = val;
            }
        }
        else {
            AutoPtr<ITimeUnit> tu = TimeUnit::GetNANOSECONDS();
            AutoPtr<IDelayed> d = IDelayed::Probe(first);
            Int64 delay = 0;
            d->GetDelay(tu, &delay);
            if (delay <= 0) {
                (IQueue::Probe(mQ))->Poll(e);
                (ILock::Probe(lock))->UnLock();
                return NOERROR;
            }
            if (nanos <= 0) {
                (ILock::Probe(lock))->UnLock();
                return NOERROR;
            }
            first = NULL; // don't retain ref while waiting
            if (nanos < delay || mLeader != NULL) {
                Int64 val;
                mAvailable->AwaitNanos(nanos, &val);
                nanos = val;
            }
            else {
                AutoPtr<IThread> thisThread = Thread::GetCurrentThread();
                mLeader = thisThread;
                Int64 timeLeft;
                mAvailable->AwaitNanos(delay, &timeLeft);
                nanos -= delay - timeLeft;
                if (mLeader == thisThread ||
                    Object::Equals(mLeader->Probe(EIID_IInterface), thisThread->Probe(EIID_IInterface)))
                    mLeader = NULL;
            }
        }
    }

    AutoPtr<IInterface> o;
    if (mLeader == NULL && ((IQueue::Probe(mQ))->Peek((IInterface**)&o), o) != NULL)
        mAvailable->Signal();
    (ILock::Probe(lock))->UnLock();
    return NOERROR;
}

ECode CDelayQueue::Peek(
    /* [out] */ IInterface** e)
{
    VALIDATE_NOT_NULL(e);
    AutoPtr<IReentrantLock> lock = mLock;
    (ILock::Probe(lock))->Lock();
    AutoPtr<IInterface> o;
    ECode ec = (IQueue::Probe(mQ))->Peek((IInterface**)&o);
    (ILock::Probe(lock))->UnLock();
    return ec;
}

ECode CDelayQueue::GetSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size);
    AutoPtr<IReentrantLock> lock = mLock;
    (ILock::Probe(lock))->Lock();
    ECode ec = (ICollection::Probe(mQ))->GetSize(size);
    (ILock::Probe(lock))->UnLock();
    return ec;
}

AutoPtr<IInterface> CDelayQueue::PeekExpired()
{
    // assert lock.isHeldByCurrentThread();
    AutoPtr<IInterface> first;
    (IQueue::Probe(mQ))->Peek((IInterface**)&first);
    if (first == NULL) {
        return NULL;
    }
    else {
        AutoPtr<ITimeUnit> tu = TimeUnit::GetNANOSECONDS();
        AutoPtr<IDelayed> d = IDelayed::Probe(first);
        Int64 val = 0;
        d->GetDelay(tu, &val);
        return val > 0 ? NULL : first;
    }
}

ECode CDelayQueue::DrainTo(
    /* [in] */ ICollection* c,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number);
    *number = 0;

    if (c == NULL)
        return E_NULL_POINTER_EXCEPTION;
    if (Object::Equals(c->Probe(EIID_IInterface), TO_IINTERFACE(this)))
        return E_ILLEGAL_ARGUMENT_EXCEPTION;

    AutoPtr<IReentrantLock> lock = mLock;
    (ILock::Probe(lock))->Lock();
    Int32 n = 0;
    Boolean b;
    AutoPtr<IInterface> e;
    for (; (e = PeekExpired()) != NULL;) {
        c->Add(e, &b);       // In this order, in case add() throws.
        AutoPtr<IInterface> pRes;
        (IQueue::Probe(mQ))->Poll((IInterface**)&pRes);
        ++n;
    }
    *number = n;
    (ILock::Probe(lock))->UnLock();
    return NOERROR;
}

ECode CDelayQueue::DrainTo(
    /* [in] */ ICollection* c,
    /* [in] */ Int32 maxElements,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number);
    *number = 0;

    if (c == NULL)
        return E_NULL_POINTER_EXCEPTION;
    if (Object::Equals(c->Probe(EIID_IInterface), TO_IINTERFACE(this)))
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    if (maxElements <= 0) {
        return NOERROR;
    }

    AutoPtr<IReentrantLock> lock = mLock;
    (ILock::Probe(lock))->Lock();
    Int32 n = 0;
    Boolean b;
    AutoPtr<IInterface> e;
    for (; n < maxElements && (e = PeekExpired()) != NULL;) {
        c->Add(e, &b);       // In this order, in case add() throws.
        AutoPtr<IInterface> pRes;
        (IQueue::Probe(mQ))->Poll((IInterface**)&pRes);
        ++n;
    }
    *number = n;
    (ILock::Probe(lock))->UnLock();
    return NOERROR;
}

ECode CDelayQueue::Clear()
{
    AutoPtr<IReentrantLock> lock = mLock;
    (ILock::Probe(lock))->Lock();
    ECode ec = (ICollection::Probe(mQ))->Clear();
    (ILock::Probe(lock))->UnLock();
    return ec;
}

ECode CDelayQueue::RemainingCapacity(
    /* [out] */ Int32* capacity)
{
    VALIDATE_NOT_NULL(capacity);
    *capacity = Elastos::Core::Math::INT32_MAX_VALUE;
    return NOERROR;
}

ECode CDelayQueue::ToArray(
    /* [out, callee] */ ArrayOf<IInterface*>** array)
{
    VALIDATE_NOT_NULL(array);
    AutoPtr<IReentrantLock> lock = mLock;
    (ILock::Probe(lock))->Lock();
    ECode ec = (ICollection::Probe(mQ))->ToArray(array);
    (ILock::Probe(lock))->UnLock();
    return ec;
}

ECode CDelayQueue::ToArray(
    /* [in] */ ArrayOf<IInterface*>* inArray,
    /* [out, callee] */ ArrayOf<IInterface*>** outArray)
{
    VALIDATE_NOT_NULL(outArray);
    AutoPtr<IReentrantLock> lock = mLock;
    (ILock::Probe(lock))->Lock();
    ECode ec = (ICollection::Probe(mQ))->ToArray(inArray, outArray);
    (ILock::Probe(lock))->UnLock();
    return ec;
}

ECode CDelayQueue::Remove(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    VALIDATE_NOT_NULL(modified);
    AutoPtr<IReentrantLock> lock = mLock;
    (ILock::Probe(lock))->Lock();
    ECode ec = (ICollection::Probe(mQ))->Remove(object, modified);
    (ILock::Probe(lock))->UnLock();
    return ec;
}

ECode CDelayQueue::RemoveEQ(
    /* [in] */ IInterface* o)
{
    AutoPtr<IReentrantLock> lock = mLock;
    (ILock::Probe(lock))->Lock();
    AutoPtr<IIterator> it;
    (IIterable::Probe(mQ))->GetIterator((IIterator**)&it);
    Boolean b = FALSE;
    for (; (it->HasNext(&b), b); ) {
        AutoPtr<IInterface> nt;
        it->GetNext((IInterface**)&nt);
        if (Object::Equals(o, nt)) {
            it->Remove();
            break;
        }
    }
    (ILock::Probe(lock))->UnLock();
    return NOERROR;
}

ECode CDelayQueue::GetIterator(
    /* [out] */ IIterator** it)
{
    VALIDATE_NOT_NULL(it);
    AutoPtr<ArrayOf<IInterface*> > arr;
    ToArray((ArrayOf<IInterface*>**)&arr);
    AutoPtr<IWeakReference> wr;
    GetWeakReference((IWeakReference**)&wr);
    AutoPtr<Itr> p = new Itr(wr.Get(), arr);
    *it = (IIterator*)p.Get();
    REFCOUNT_ADD(*it);
    return NOERROR;
}

ECode CDelayQueue::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    return E_NO_SUCH_METHOD_EXCEPTION;
}

ECode CDelayQueue::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    return E_NO_SUCH_METHOD_EXCEPTION;
}

//====================================================================
// CDelayQueue::Itr::
//====================================================================
CAR_INTERFACE_IMPL(CDelayQueue::Itr, Object, IIterator)

CDelayQueue::Itr::Itr(
    /* [in] */ IWeakReference* owner,
    /* [in] */ ArrayOf<IInterface*>* array)
{
    mLastRet = -1;
    mArray = array;
    mWeakOwner = owner;
}

ECode CDelayQueue::Itr::HasNext(
    /* [out] */ Boolean* res)
{
    *res = mCursor < mArray->GetLength();
    return NOERROR;
}

ECode CDelayQueue::Itr::GetNext(
    /* [out] */ IInterface** res)
{
    if (mCursor >= mArray->GetLength())
        return E_NO_SUCH_METHOD_EXCEPTION;
    mLastRet = mCursor;
    *res = (*mArray)[mCursor++];
    REFCOUNT_ADD(*res);
    return NOERROR;
}

ECode CDelayQueue::Itr::Remove()
{
    if (mLastRet < 0)
        return E_ILLEGAL_STATE_EXCEPTION;

    AutoPtr<IBlockingQueue> queue;
    mWeakOwner->Resolve(EIID_IBlockingQueue, (IInterface**)&queue);
    if (queue) {
        CDelayQueue* dq = (CDelayQueue*)queue.Get();
        dq->RemoveEQ((*mArray)[mLastRet]);
    }

    mLastRet = -1;
    return NOERROR;
}

} // namespace Concurrent
} // namespace Utility
} // namespace Elastos
