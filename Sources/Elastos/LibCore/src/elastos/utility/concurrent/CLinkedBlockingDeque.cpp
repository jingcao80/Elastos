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
#include "CLinkedBlockingDeque.h"
#include <StringBuilder.h>
#include <Math.h>

using Elastos::Core::StringBuilder;
using Elastos::Core::Math;
using Elastos::IO::EIID_ISerializable;
using Elastos::Utility::Concurrent::Locks::ILock;

namespace Elastos {
namespace Utility {
namespace Concurrent {

//====================================================================
// CLinkedBlockingDeque::Node::
//====================================================================
CLinkedBlockingDeque::Node::Node(
    /* [in] */ IInterface* x)
{
    mItem = x;
}

//====================================================================
// CLinkedBlockingDeque::AbstractItr::
//====================================================================
CAR_INTERFACE_IMPL(CLinkedBlockingDeque::AbstractItr, Object, IIterator)

CLinkedBlockingDeque::AbstractItr::AbstractItr(
    /* [in] */ CLinkedBlockingDeque* owner) : mOwner(owner)
{
    // set to initial position
    AutoPtr<IReentrantLock> lock;// = LinkedBlockingDeque.this.lock;
    (ILock::Probe(lock))->Lock();
//    mNext = FirstNode();
    mNextItem = (mNext == NULL) ? NULL : mNext->mItem;
    (ILock::Probe(lock))->UnLock();
}

AutoPtr<CLinkedBlockingDeque::Node> CLinkedBlockingDeque::AbstractItr::Succ(
    /* [in] */ Node* n)
{
    // Chains of deleted nodes ending in null or self-links
    // are possible if multiple interior nodes are removed.
    for (;;) {
        AutoPtr<Node> s = NextNode(n);
        if (s == NULL)
            return NULL;
        else if (s->mItem != NULL)
            return s;
        else if (Object::Equals(s->Probe(EIID_IInterface), n->Probe(EIID_IInterface)))
            return FirstNode();
        else
            n = s;
    }
}

void CLinkedBlockingDeque::AbstractItr::Advance()
{
    AutoPtr<IReentrantLock> lock = mOwner->mLock;
    (ILock::Probe(lock))->Lock();
    // assert next != null;
    mNext = Succ(mNext);
    mNextItem = (mNext == NULL) ? NULL : mNext->mItem;
    (ILock::Probe(lock))->UnLock();
}

ECode CLinkedBlockingDeque::AbstractItr::HasNext(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mNext != NULL;
    return NOERROR;
}

ECode CLinkedBlockingDeque::AbstractItr::GetNext(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);
    if (mNext == NULL)
        return E_NO_SUCH_ELEMENT_EXCEPTION;
    mLastRet = mNext;
    AutoPtr<IInterface> x = mNextItem;
    Advance();
    *object = x;
    REFCOUNT_ADD(*object);
    return NOERROR;
}

ECode CLinkedBlockingDeque::AbstractItr::Remove()
{
    AutoPtr<Node> n = mLastRet;
    if (n == NULL)
        return E_ILLEGAL_STATE_EXCEPTION;
    mLastRet = NULL;
    AutoPtr<IReentrantLock> lock = mOwner->mLock;
    (ILock::Probe(lock))->Lock();
    if (n->mItem != NULL)
        mOwner->Unlink(n);
    (ILock::Probe(lock))->UnLock();
    return NOERROR;
}

//====================================================================
// CLinkedBlockingDeque::Itr::
//====================================================================
CLinkedBlockingDeque::Itr::Itr(
    /* [in] */ CLinkedBlockingDeque* owner) : AbstractItr(owner)
{

}

AutoPtr<CLinkedBlockingDeque::Node> CLinkedBlockingDeque::Itr::FirstNode()
{
    return mOwner->mFirst;
}

AutoPtr<CLinkedBlockingDeque::Node> CLinkedBlockingDeque::Itr::NextNode(
    /* [in] */ Node* n)
{
    return n->mNext;
}

//====================================================================
// CLinkedBlockingDeque::DescendingItr::
//====================================================================
CLinkedBlockingDeque::DescendingItr::DescendingItr(
    /* [in] */ CLinkedBlockingDeque* owner) : AbstractItr(owner)
{

}

AutoPtr<CLinkedBlockingDeque::Node> CLinkedBlockingDeque::DescendingItr::FirstNode()
{
    return mOwner->mLast;
}

AutoPtr<CLinkedBlockingDeque::Node> CLinkedBlockingDeque::DescendingItr::NextNode(
    /* [in] */ Node* n)
{
    return n->mPrev;
}

//====================================================================
// CLinkedBlockingDeque::
//====================================================================

Int64 CLinkedBlockingDeque::sSerialVersionUID = -387911632671998426L;

CAR_INTERFACE_IMPL_3(CLinkedBlockingDeque, AbstractQueue, IBlockingDeque, IBlockingQueue, ISerializable)

CAR_OBJECT_IMPL(CLinkedBlockingDeque)

ECode CLinkedBlockingDeque::constructor()
{
    return constructor(Elastos::Core::Math::INT32_MAX_VALUE);
}

ECode CLinkedBlockingDeque::constructor(
    /* [in] */ Int32 capacity)
{
    if (capacity <= 0) return E_ILLEGAL_ARGUMENT_EXCEPTION;
    mCapacity = capacity;
    return NOERROR;
}

ECode CLinkedBlockingDeque::constructor(
    /* [in] */ ICollection* c)
{
    constructor(Elastos::Core::Math::INT32_MAX_VALUE);
    AutoPtr<IReentrantLock> lock = mLock;
    (ILock::Probe(lock))->Lock(); // Never contended, but necessary for visibility

    AutoPtr<IIterator> it;
    c->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        if (obj == NULL) {
            (ILock::Probe(lock))->UnLock();
            return E_NULL_POINTER_EXCEPTION;
        }
        AutoPtr<Node> p = new Node(obj);
        if (!LinkLast(p)) {
            (ILock::Probe(lock))->UnLock();
            return E_ILLEGAL_STATE_EXCEPTION;
        }
    }
    (ILock::Probe(lock))->UnLock();
    return NOERROR;
}

Boolean CLinkedBlockingDeque::LinkFirst(
    /* [in] */ Node* node)
{
    // assert lock.isHeldByCurrentThread();
    if (mCount >= mCapacity)
        return FALSE;
    AutoPtr<Node> f = mFirst;
    node->mNext = f;
    mFirst = node;
    if (mLast == NULL)
        mLast = node;
    else
        f->mPrev = node;
    ++mCount;
    mNotEmpty->Signal();
    return TRUE;
}

Boolean CLinkedBlockingDeque::LinkLast(
    /* [in] */ Node* node)
{
    // assert lock.isHeldByCurrentThread();
    if (mCount >= mCapacity)
        return FALSE;
    AutoPtr<Node> l = mLast;
    node->mPrev = l;
    mLast = node;
    if (mFirst == NULL)
        mFirst = node;
    else
        l->mNext = node;
    ++mCount;
    mNotEmpty->Signal();
    return TRUE;
}

AutoPtr<IInterface> CLinkedBlockingDeque::UnlinkFirst()
{
    // assert lock.isHeldByCurrentThread();
    AutoPtr<Node> f = mFirst;
    if (f == NULL)
        return NULL;
    AutoPtr<Node> n = f->mNext;
    AutoPtr<IInterface> item = f->mItem;
    f->mItem = NULL;
    f->mNext = f; // help GC
    mFirst = n;
    if (n == NULL)
        mLast = NULL;
    else
        n->mPrev = NULL;
    --mCount;
    mNotFull->Signal();
    return item;
}

AutoPtr<IInterface> CLinkedBlockingDeque::UnlinkLast()
{
    // assert lock.isHeldByCurrentThread();
    AutoPtr<Node> l = mLast;
    if (l == NULL)
        return NULL;
    AutoPtr<Node> p = l->mPrev;
    AutoPtr<IInterface> item = l->mItem;
    l->mItem = NULL;
    l->mPrev = l; // help GC
    mLast = p;
    if (p == NULL)
        mFirst = NULL;
    else
        p->mNext = NULL;
    --mCount;
    mNotFull->Signal();
    return item;
}

void CLinkedBlockingDeque::Unlink(
    /* [in] */ Node* x)
{
    // assert lock.isHeldByCurrentThread();
    AutoPtr<Node> p = x->mPrev;
    AutoPtr<Node> n = x->mNext;
    if (p == NULL) {
        UnlinkFirst();
    }
    else if (n == NULL) {
        UnlinkLast();
    }
    else {
        p->mNext = n;
        n->mPrev = p;
        x->mItem = NULL;
        // Don't mess with x's links.  They may still be in use by
        // an iterator.
        --mCount;
        mNotFull->Signal();
    }
}

ECode CLinkedBlockingDeque::AddFirst(
    /* [in] */ IInterface* e)
{
    Boolean b = FALSE;
    if (!(OfferFirst(e, &b), b))
        return E_ILLEGAL_STATE_EXCEPTION;
    return NOERROR;
}

ECode CLinkedBlockingDeque::AddLast(
    /* [in] */ IInterface* e)
{
    Boolean b = FALSE;
    if (!(OfferLast(e, &b), b))
        return E_ILLEGAL_STATE_EXCEPTION;
    return NOERROR;
}

ECode CLinkedBlockingDeque::OfferFirst(
    /* [in] */ IInterface* e,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(e);
    VALIDATE_NOT_NULL(value);
    AutoPtr<Node> node = new Node(e);
    AutoPtr<IReentrantLock> lock = mLock;
    (ILock::Probe(lock))->Lock();
    *value = LinkFirst(node);
    (ILock::Probe(lock))->UnLock();
    return NOERROR;
}

ECode CLinkedBlockingDeque::OfferLast(
    /* [in] */ IInterface* e,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(e);
    VALIDATE_NOT_NULL(value);
    AutoPtr<Node> node = new Node(e);
    AutoPtr<IReentrantLock> lock = mLock;
    (ILock::Probe(lock))->Lock();
    *value = LinkLast(node);
    (ILock::Probe(lock))->UnLock();
    return NOERROR;
}

ECode CLinkedBlockingDeque::PutFirst(
    /* [in] */ IInterface* e)
{
    VALIDATE_NOT_NULL(e);
    AutoPtr<Node> node = new Node(e);
    AutoPtr<IReentrantLock> lock = mLock;
    (ILock::Probe(lock))->Lock();
    while (!LinkFirst(node))
        mNotFull->Await();
    (ILock::Probe(lock))->UnLock();
    return NOERROR;
}

ECode CLinkedBlockingDeque::PutLast(
    /* [in] */ IInterface* e)
{
    VALIDATE_NOT_NULL(e);
    AutoPtr<Node> node = new Node(e);
    AutoPtr<IReentrantLock> lock = mLock;
    (ILock::Probe(lock))->Lock();
    while (!LinkLast(node))
        mNotFull->Await();
    (ILock::Probe(lock))->UnLock();
    return NOERROR;
}

ECode CLinkedBlockingDeque::OfferFirst(
    /* [in] */ IInterface* e,
    /* [in] */ Int64 timeout,
    /* [in] */ ITimeUnit* unit,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(e);
    AutoPtr<Node> node = new Node(e);
    Int64 nanos;
    unit->ToNanos(timeout, &nanos);
    AutoPtr<IReentrantLock> lock = mLock;
    (ILock::Probe(lock))->LockInterruptibly();
    while (!LinkFirst(node)) {
        if (nanos <= 0) {
            *value = FALSE;
            (ILock::Probe(lock))->UnLock();
            return NOERROR;
        }
        mNotFull->AwaitNanos(nanos, &nanos);
    }
    *value = TRUE;
    (ILock::Probe(lock))->UnLock();
    return NOERROR;
}

ECode CLinkedBlockingDeque::OfferLast(
    /* [in] */ IInterface* e,
    /* [in] */ Int64 timeout,
    /* [in] */ ITimeUnit* unit,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(e);
    VALIDATE_NOT_NULL(value);
    AutoPtr<Node> node = new Node(e);
    Int64 nanos;
    unit->ToNanos(timeout, &nanos);
    AutoPtr<IReentrantLock> lock = mLock;
    (ILock::Probe(lock))->LockInterruptibly();
    while (!LinkLast(node)) {
        if (nanos <= 0) {
            *value = FALSE;
            (ILock::Probe(lock))->UnLock();
            return NOERROR;
        }
        mNotFull->AwaitNanos(nanos, &nanos);
    }
    *value = TRUE;
    (ILock::Probe(lock))->UnLock();
    return NOERROR;
}

ECode CLinkedBlockingDeque::RemoveFirst(
    /* [out] */ PInterface* res)
{
    VALIDATE_NOT_NULL(res);
    AutoPtr<IInterface> x;
    PollFirst((IInterface**)&x);
    if (x == NULL) return E_NO_SUCH_ELEMENT_EXCEPTION;
    *res = x;
    REFCOUNT_ADD(*res);
    return NOERROR;
}

ECode CLinkedBlockingDeque::RemoveLast(
    /* [out] */ PInterface* res)
{
    VALIDATE_NOT_NULL(res);
    AutoPtr<IInterface> x;
    PollLast((IInterface**)&x);
    if (x == NULL) return E_NO_SUCH_ELEMENT_EXCEPTION;
    *res = x;
    REFCOUNT_ADD(*res);
    return NOERROR;
}

ECode CLinkedBlockingDeque::PollFirst(
    /* [out] */ PInterface* res)
{
    VALIDATE_NOT_NULL(res);
    AutoPtr<IReentrantLock> lock = mLock;
    (ILock::Probe(lock))->Lock();
    AutoPtr<IInterface> p = UnlinkFirst();
    *res = p.Get();
    REFCOUNT_ADD(*res);
    (ILock::Probe(lock))->UnLock();
    return NOERROR;
}

ECode CLinkedBlockingDeque::PollLast(
    /* [out] */ PInterface* res)
{
    VALIDATE_NOT_NULL(res);
    AutoPtr<IReentrantLock> lock = mLock;
    (ILock::Probe(lock))->Lock();
    AutoPtr<IInterface> p = UnlinkLast();
    *res = p.Get();
    REFCOUNT_ADD(*res);
    (ILock::Probe(lock))->UnLock();
    return NOERROR;
}

ECode CLinkedBlockingDeque::TakeFirst(
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);
    AutoPtr<IReentrantLock> lock = mLock;
    (ILock::Probe(lock))->Lock();
    AutoPtr<IInterface> x;
    while ( (x = UnlinkFirst()) == NULL)
        mNotEmpty->Await();
    *outface = x;
    REFCOUNT_ADD(*outface);
    (ILock::Probe(lock))->UnLock();
    return NOERROR;
}

ECode CLinkedBlockingDeque::TakeLast(
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);
    AutoPtr<IReentrantLock> lock = mLock;
    (ILock::Probe(lock))->Lock();
    AutoPtr<IInterface> x;
    while ( (x = UnlinkLast()) == NULL)
        mNotEmpty->Await();
    *outface = x;
    REFCOUNT_ADD(*outface);
    (ILock::Probe(lock))->UnLock();
    return NOERROR;
}

ECode CLinkedBlockingDeque::PollFirst(
    /* [in] */ Int64 timeout,
    /* [in] */ ITimeUnit* unit,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);
    Int64 nanos;
    unit->ToNanos(timeout, &nanos);
    AutoPtr<IReentrantLock> lock = mLock;
    (ILock::Probe(lock))->LockInterruptibly();
    AutoPtr<IInterface> x;
    while ( (x = UnlinkFirst()) == NULL) {
        if (nanos <= 0) {
            *outface = NULL;
            (ILock::Probe(lock))->UnLock();
            return NOERROR;
        }
        mNotEmpty->AwaitNanos(nanos, &nanos);
    }
    *outface = x;
    REFCOUNT_ADD(*outface);
    (ILock::Probe(lock))->UnLock();
    return NOERROR;
}

ECode CLinkedBlockingDeque::PollLast(
    /* [in] */ Int64 timeout,
    /* [in] */ ITimeUnit* unit,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);
    Int64 nanos;
    unit->ToNanos(timeout, &nanos);
    AutoPtr<IReentrantLock> lock = mLock;
    (ILock::Probe(lock))->LockInterruptibly();
    AutoPtr<IInterface> x;
    while ( (x = UnlinkLast()) == NULL) {
        if (nanos <= 0) {
            *outface = NULL;
            (ILock::Probe(lock))->UnLock();
            return NOERROR;
        }
        mNotEmpty->AwaitNanos(nanos, &nanos);
    }
    *outface = x;
    REFCOUNT_ADD(*outface);
    (ILock::Probe(lock))->UnLock();
    return NOERROR;
}

ECode CLinkedBlockingDeque::GetFirst(
    /* [out] */ PInterface* res)
{
    VALIDATE_NOT_NULL(res);
    AutoPtr<IInterface> x;
    PeekFirst((IInterface**)&x);
    if (x == NULL) return E_NO_SUCH_ELEMENT_EXCEPTION;
    *res = x;
    REFCOUNT_ADD(*res);
    return NOERROR;
}

ECode CLinkedBlockingDeque::GetLast(
    /* [out] */ PInterface* res)
{
    VALIDATE_NOT_NULL(res);
    AutoPtr<IInterface> x;
    PeekLast((IInterface**)&x);
    if (x == NULL) return E_NO_SUCH_ELEMENT_EXCEPTION;
    *res = x;
    REFCOUNT_ADD(*res);
    return NOERROR;
}

ECode CLinkedBlockingDeque::PeekFirst(
    /* [out] */ PInterface* res)
{
    VALIDATE_NOT_NULL(res);
    AutoPtr<IReentrantLock> lock = mLock;
    (ILock::Probe(lock))->Lock();
    *res = (mFirst == NULL) ? NULL : mFirst->mItem;
    REFCOUNT_ADD(*res);
    (ILock::Probe(lock))->UnLock();
    return NOERROR;
}

ECode CLinkedBlockingDeque::PeekLast(
    /* [out] */ PInterface* res)
{
    VALIDATE_NOT_NULL(res);
    AutoPtr<IReentrantLock> lock = mLock;
    (ILock::Probe(lock))->Lock();
    *res = (mLast == NULL) ? NULL : mLast->mItem;
    REFCOUNT_ADD(*res);
    (ILock::Probe(lock))->UnLock();
    return NOERROR;
}

ECode CLinkedBlockingDeque::RemoveFirstOccurrence(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    if (o == NULL) {
        *value = FALSE;
        return NOERROR;
    }
    AutoPtr<IReentrantLock> lock = mLock;
    (ILock::Probe(lock))->Lock();
    for (AutoPtr<Node> p = mFirst; p != NULL; p = p->mNext) {
        if (Object::Equals(o, p->mItem)) {
            Unlink(p);
            *value = TRUE;
            (ILock::Probe(lock))->UnLock();
            return NOERROR;
        }
    }
    *value = FALSE;
    (ILock::Probe(lock))->UnLock();
    return NOERROR;
}

ECode CLinkedBlockingDeque::RemoveLastOccurrence(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    if (o == NULL) {
        *value = FALSE;
        return NOERROR;
    }
    AutoPtr<IReentrantLock> lock = mLock;
    (ILock::Probe(lock))->Lock();
    for (AutoPtr<Node> p = mLast; p != NULL; p = p->mPrev) {
        if (Object::Equals(o, p->mItem)) {
            Unlink(p);
            *value = TRUE;
            (ILock::Probe(lock))->UnLock();
            return NOERROR;
        }
    }
    *value = FALSE;
    (ILock::Probe(lock))->UnLock();
    return NOERROR;
}

ECode CLinkedBlockingDeque::Add(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    VALIDATE_NOT_NULL(modified);
    AddLast(object);
    *modified = TRUE;
    return NOERROR;
}

ECode CLinkedBlockingDeque::Offer(
    /* [in] */ IInterface* e,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return OfferLast(e, result);
}

ECode CLinkedBlockingDeque::Put(
    /* [in] */ IInterface* e)
{
    return PutLast(e);
}

ECode CLinkedBlockingDeque::Offer(
    /* [in] */ IInterface* e,
    /* [in] */ Int64 timeout,
    /* [in] */ ITimeUnit* unit,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return OfferLast(e, timeout, unit, result);
}

ECode CLinkedBlockingDeque::Remove(
        /* [out] */ PInterface* object)
{
    VALIDATE_NOT_NULL(object);
    return RemoveFirst(object);
}

ECode CLinkedBlockingDeque::Poll(
    /* [out] */ IInterface** e)
{
    VALIDATE_NOT_NULL(e);
    return PollFirst(e);
}

ECode CLinkedBlockingDeque::Take(
    /* [out] */ IInterface** e)
{
    VALIDATE_NOT_NULL(e);
    return TakeFirst(e);
}

ECode CLinkedBlockingDeque::Poll(
    /* [in] */ Int64 timeout,
    /* [in] */ ITimeUnit* unit,
    /* [out] */ IInterface** e)
{
    VALIDATE_NOT_NULL(e);
    return PollFirst(timeout, unit, e);
}

ECode CLinkedBlockingDeque::GetElement(
    /* [out] */ IInterface** e)
{
    VALIDATE_NOT_NULL(e);
    return GetFirst(e);
}

ECode CLinkedBlockingDeque::Peek(
    /* [out] */ IInterface** e)
{
    VALIDATE_NOT_NULL(e);
    return PeekFirst(e);
}

ECode CLinkedBlockingDeque::RemainingCapacity(
    /* [out] */ Int32* capacity)
{
    VALIDATE_NOT_NULL(capacity);
    AutoPtr<IReentrantLock> lock = mLock;
    (ILock::Probe(lock))->Lock();
    *capacity = mCapacity - mCount;
    (ILock::Probe(lock))->UnLock();
    return NOERROR;
}

ECode CLinkedBlockingDeque::DrainTo(
    /* [in] */ ICollection* c,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number);
    return DrainTo(c, Elastos::Core::Math::INT32_MAX_VALUE, number);
}

ECode CLinkedBlockingDeque::DrainTo(
    /* [in] */ ICollection* c,
    /* [in] */ Int32 maxElements,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number);
    VALIDATE_NOT_NULL(c);
    if (Object::Equals(c->Probe(EIID_IInterface), TO_IINTERFACE(this)))
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    if (maxElements <= 0) {
        *number = 0;
        return NOERROR;
    }
    AutoPtr<IReentrantLock> lock = mLock;
    (ILock::Probe(lock))->Lock();
    Int32 n = Elastos::Core::Math::Min(maxElements, mCount);
    for (Int32 i = 0; i < n; i++) {
        Boolean b;
        c->Add(mFirst->mItem, &b);   // In this order, in case add() throws.
        UnlinkFirst();
    }
    *number = n;
    (ILock::Probe(lock))->UnLock();
    return NOERROR;
}

ECode CLinkedBlockingDeque::Push(
    /* [in] */ IInterface* e)
{
    return AddFirst(e);
}

ECode CLinkedBlockingDeque::Pop(
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);
    return RemoveFirst(outface);
}

ECode CLinkedBlockingDeque::Remove(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    VALIDATE_NOT_NULL(modified);
    return RemoveFirstOccurrence(object, modified);
}

ECode CLinkedBlockingDeque::GetSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size);
    AutoPtr<IReentrantLock> lock = mLock;
    (ILock::Probe(lock))->Lock();
    *size = mCount;
    (ILock::Probe(lock))->UnLock();
    return NOERROR;
}

ECode CLinkedBlockingDeque::Contains(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (object == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<IReentrantLock> lock = mLock;
    (ILock::Probe(lock))->Lock();
    for (AutoPtr<Node> p = mFirst; p != NULL; p = p->mNext) {
        if (Object::Equals(object, p->mItem)) {
            *result = TRUE;
            (ILock::Probe(lock))->UnLock();
            return NOERROR;
        }
    }
    *result = FALSE;
    (ILock::Probe(lock))->UnLock();
    return NOERROR;
}

ECode CLinkedBlockingDeque::ToArray(
    /* [out, callee] */ ArrayOf<IInterface*>** array)
{
    VALIDATE_NOT_NULL(array);
    AutoPtr<IReentrantLock> lock = mLock;
    (ILock::Probe(lock))->Lock();
    AutoPtr<ArrayOf<IInterface*> > a = ArrayOf<IInterface*>::Alloc(mCount);
    Int32 k = 0;
    for (AutoPtr<Node> p = mFirst; p != NULL; p = p->mNext)
        (*a)[k++] = p->mItem;
    *array = a;
    REFCOUNT_ADD(*array);
    (ILock::Probe(lock))->UnLock();
    return NOERROR;
}

ECode CLinkedBlockingDeque::ToArray(
    /* [in] */ ArrayOf<IInterface*>* inArray,
    /* [out, callee] */ ArrayOf<IInterface*>** outArray)
{
    VALIDATE_NOT_NULL(outArray);
    AutoPtr<IReentrantLock> lock = mLock;
    (ILock::Probe(lock))->Lock();
    // if (inArray->GetLength() < mCount)
    //     a = (T[])java.lang.reflect.Array.newInstance
    //         (a.getClass().getComponentType(), mCount);

    Int32 k = 0;
    for (AutoPtr<Node> p = mFirst; p != NULL; p = p->mNext)
        (*inArray)[k++] = p->mItem;
    if (inArray->GetLength() > k)
        (*inArray)[k] = NULL;
    *outArray = inArray;
    REFCOUNT_ADD(*outArray);
    (ILock::Probe(lock))->UnLock();
    return NOERROR;
}

String CLinkedBlockingDeque::ToString()
{
    AutoPtr<IReentrantLock> lock = mLock;
    (ILock::Probe(lock))->Lock();
    AutoPtr<Node> p = mFirst;
    if (p == NULL) {
        (ILock::Probe(lock))->UnLock();
        return String("[]");
    }

    StringBuilder sb;
    sb.AppendChar('[');
    for (;;) {
        AutoPtr<IInterface> e = p->mItem;
        if (Object::Equals(e.Get(), TO_IINTERFACE(this)))
            sb += String("(this Collection)");
        else
            sb += e;
        p = p->mNext;
        if (p == NULL) {
            (ILock::Probe(lock))->UnLock();
            sb.AppendChar(']');
            return sb.ToString();
        }
        sb.AppendChar(',');
        sb.AppendChar(' ');
    }
}

ECode CLinkedBlockingDeque::Clear()
{
    AutoPtr<IReentrantLock> lock = mLock;
    (ILock::Probe(lock))->Lock();
    for (AutoPtr<Node> f = mFirst; f != NULL; ) {
        f->mItem = NULL;
        AutoPtr<Node> n = f->mNext;
        f->mPrev = NULL;
        f->mNext = NULL;
        f = n;
    }
    mFirst = mLast = NULL;
    mCount = 0;
    mNotFull->SignalAll();
    (ILock::Probe(lock))->UnLock();
    return NOERROR;
}

ECode CLinkedBlockingDeque::GetIterator(
    /* [out] */ IIterator** it)
{
    VALIDATE_NOT_NULL(it);
    *it = new Itr(this);
    REFCOUNT_ADD(*it);
    return NOERROR;
}

ECode CLinkedBlockingDeque::GetDescendingIterator(
    /* [out] */ IIterator** iterator)
{
    VALIDATE_NOT_NULL(iterator);
    *iterator = new DescendingItr(this);
    REFCOUNT_ADD(*iterator);
    return NOERROR;
}

void CLinkedBlockingDeque::WriteObject(
    /* [in] */ IObjectOutputStream* s)
{
    assert(0);
//    AutoPtr<IReentrantLock> lock = mLock;
//    lock->Lock();
    // Write out capacity and any hidden stuff
//    s->DefaultWriteObject();
    // Write out all elements in the proper order.
    // for (AutoPtr<Node> p = mFirst; p != NULL; p = p->mNext)
    //     s->WriteObject(p->mItem);
    // Use trailing NULL as sentinel
//    s->WriteObject(NULL);
//    lock->UnLock();
}

void CLinkedBlockingDeque::ReadObject(
    /* [in] */ IObjectInputStream* s)
{
    assert(0);
    s->DefaultReadObject();
    // mCount = 0;
    // mFirst = NULL;
    // mLast = NULL;
    // Read in all elements and place in queue
    // for (;;) {
    //     AutoPtr<IInterface> item = s->ReadObject();
    //     if (item == NULL)
    //         break;
    //     Add(item);
    // }
}

ECode CLinkedBlockingDeque::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    return AbstractQueue::Equals(object, result);
}

ECode CLinkedBlockingDeque::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    return AbstractQueue::GetHashCode(hashCode);
}

} // namespace Concurrent
} // namespace Utility
} // namespace Elastos

