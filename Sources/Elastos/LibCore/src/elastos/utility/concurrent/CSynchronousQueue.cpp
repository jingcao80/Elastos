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
#include "CSynchronousQueue.h"
#include "Thread.h"
#include "LockSupport.h"
#include "CReentrantLock.h"
#include "CSystem.h"
#include <cutils/atomic.h>
#include <cutils/atomic-inline.h>
#include "CLibcore.h"
#include <elastos/droid/system/OsConstants.h>

using Elastos::IO::EIID_ISerializable;
using Elastos::Core::ISystem;
using Elastos::Core::Thread;
using Elastos::Utility::Concurrent::Locks::LockSupport;
using Elastos::Utility::Concurrent::Locks::CReentrantLock;
using Elastos::Droid::System::OsConstants;
using Libcore::IO::IOs;
using Libcore::IO::ILibcore;
using Libcore::IO::CLibcore;

namespace Elastos {
namespace Utility {
namespace Concurrent {

static Boolean CompareAndSwapObject(volatile int32_t* address, Object* expect, Object* update)
{
    // Note: android_atomic_cmpxchg() returns 0 on success, not failure.
    int ret = android_atomic_release_cas((int32_t)expect,
            (int32_t)update, address);
    if (ret == 0) {
        REFCOUNT_ADD(update)
        REFCOUNT_RELEASE(expect)
    }
    return (ret == 0);
}

static Boolean CompareAndSwapObject(volatile int32_t* address, IInterface* expect, IInterface* update)
{
    // Note: android_atomic_cmpxchg() returns 0 on success, not failure.
    int ret = android_atomic_release_cas((int32_t)expect,
            (int32_t)update, address);
    if (ret == 0) {
        REFCOUNT_ADD(update)
        REFCOUNT_RELEASE(expect)
    }
    return (ret == 0);
}

//===============================================================================
// CSynchronousQueue::TransferStack::SNode::
//===============================================================================

CSynchronousQueue::TransferStack::SNode::SNode(
    /* [in] */ IInterface* item)
{
    mItem = item;
}

Boolean CSynchronousQueue::TransferStack::SNode::CasNext(
    /* [in] */ SNode* cmp,
    /* [in] */ SNode* val)
{
    return cmp == mNext &&
        CompareAndSwapObject((volatile int32_t*)&mNext, cmp, val);
}

Boolean CSynchronousQueue::TransferStack::SNode::TryMatch(
    /* [in] */ SNode* s)
{
    if (mMatch == NULL &&
        CompareAndSwapObject((volatile int32_t*)&mMatch, NULL, s)) {
        AutoPtr<IThread> w = mWaiter;
        if (w != NULL) {    // waiters need at most one unpark
            mWaiter = NULL;
            LockSupport::Unpark(w);
        }
        return TRUE;
    }
    return mMatch.Get() == s;
}

void CSynchronousQueue::TransferStack::SNode::TryCancel()
{
    CompareAndSwapObject((volatile int32_t*)&mMatch, NULL, this);
}

Boolean CSynchronousQueue::TransferStack::SNode::IsCancelled()
{
    return mMatch.Get() == this;
}

//===============================================================================
// CSynchronousQueue::TransferStack::
//===============================================================================
Int32 CSynchronousQueue::TransferStack::REQUEST    = 0;
Int32 CSynchronousQueue::TransferStack::DATA       = 1;
Int32 CSynchronousQueue::TransferStack::FULFILLING = 2;

CAR_INTERFACE_IMPL(CSynchronousQueue::TransferStack, Transferer, ISynchronousQueueTransferStack)

Boolean CSynchronousQueue::TransferStack::IsFulfilling(
    /* [in] */ Int32 m)
{
    return (m & FULFILLING) != 0;
}

Boolean CSynchronousQueue::TransferStack::CasHead(
    /* [in] */ SNode* h,
    /* [in] */ SNode* nh)
{
    return h == mHead &&
        CompareAndSwapObject((volatile int32_t*)&mHead, h, nh);
}

AutoPtr<CSynchronousQueue::TransferStack::SNode> CSynchronousQueue::TransferStack::Snode(
    /* [in] */ SNode* s,
    /* [in] */ IInterface* e,
    /* [in] */ SNode* next,
    /* [in] */ Int32 mode)
{
    if (s == NULL) s = new SNode(e);
    s->mMode = mode;
    s->mNext = next;
    return s;
}

AutoPtr<IInterface> CSynchronousQueue::TransferStack::Transfer(
    /* [in] */ IInterface* e,
    /* [in] */ Boolean timed,
    /* [in] */ Int64 nanos)
{
    /*
     * Basic algorithm is to loop trying one of three actions:
     *
     * 1. If apparently empty or already containing nodes of same
     *    mode, try to push node on stack and wait for a match,
     *    returning it, or null if cancelled.
     *
     * 2. If apparently containing node of complementary mode,
     *    try to push a fulfilling node on to stack, match
     *    with corresponding waiting node, pop both from
     *    stack, and return matched item. The matching or
     *    unlinking might not actually be necessary because of
     *    other threads performing action 3:
     *
     * 3. If top of stack already holds another fulfilling node,
     *    help it out by doing its match and/or pop
     *    operations, and then continue. The code for helping
     *    is essentially the same as for fulfilling, except
     *    that it doesn't return the item.
     */

    AutoPtr<SNode> s; // constructed/reused as needed
    Int32 mode = (e == NULL) ? REQUEST : DATA;

    for (;;) {
        AutoPtr<SNode> h = mHead;
        if (h == NULL || h->mMode == mode) {  // empty or same-mode
            if (timed && nanos <= 0) {      // can't wait
                if (h != NULL && h->IsCancelled())
                    CasHead(h, h->mNext);     // pop cancelled node
                else
                    return NULL;
            }
            else if (CasHead(h, s = Snode(s, e, h, mode))) {
                AutoPtr<SNode> m = AwaitFulfill(s, timed, nanos);
                if (m == s) {               // wait was cancelled
                    Clean(s);
                    return NULL;
                }
                if ((h = mHead) != NULL && h->mNext == s)
                    CasHead(h, s->mNext);     // help s's fulfiller
                return ((mode == REQUEST) ? m->mItem : s->mItem);
            }
        }
        else if (!IsFulfilling(h->mMode)) { // try to fulfill
            if (h->IsCancelled())            // already cancelled
                CasHead(h, h->mNext);         // pop and retry
            else if (CasHead(h, s=Snode(s, e, h, FULFILLING|mode))) {
                for (;;) { // loop until matched or waiters disappear
                    AutoPtr<SNode> m = s->mNext;       // m is s's match
                    if (m == NULL) {        // all waiters are gone
                        CasHead(s, NULL);   // pop fulfill node
                        s = NULL;           // use new node next time
                        break;              // restart main loop
                    }
                    AutoPtr<SNode> mn = m->mNext;
                    if (m->TryMatch(s)) {
                        CasHead(s, mn);     // pop both s and m
                        return ((mode == REQUEST) ? m->mItem : s->mItem);
                    }
                    else                  // lost match
                        s->CasNext(m, mn);   // help unlink
                }
            }
        }
        else {                            // help a fulfiller
            AutoPtr<SNode> m = h->mNext;               // m is h's match
            if (m == NULL)                  // waiter is gone
                CasHead(h, NULL);           // pop fulfilling node
            else {
                AutoPtr<SNode> mn = m->mNext;
                if (m->TryMatch(h))          // help match
                    CasHead(h, mn);         // pop both h and m
                else                        // lost match
                    h->CasNext(m, mn);       // help unlink
            }
        }
    }
}

AutoPtr<CSynchronousQueue::TransferStack::SNode> CSynchronousQueue::TransferStack::AwaitFulfill(
    /* [in] */ SNode* s,
    /* [in] */ Boolean timed,
    /* [in] */ Int64 nanos)
{
    /*
     * When a node/thread is about to block, it sets its waiter
     * field and then rechecks state at least one more time
     * before actually parking, thus covering race vs
     * fulfiller noticing that waiter is non-null so should be
     * woken.
     *
     * When invoked by nodes that appear at the point of call
     * to be at the head of the stack, calls to park are
     * preceded by spins to avoid blocking when producers and
     * consumers are arriving very close in time.  This can
     * happen enough to bother only on multiprocessors.
     *
     * The order of checks for returning out of main loop
     * reflects fact that interrupts have precedence over
     * normal returns, which have precedence over
     * timeouts. (So, on timeout, one last check for match is
     * done before giving up.) Except that calls from untimed
     * SynchronousQueue.{poll/offer} don't check interrupts
     * and don't wait at all, so are trapped in transfer
     * method rather than calling awaitFulfill.
     */
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    Int64 lastTime = 0;
    system->GetNanoTime(&lastTime);
    Int64 deadline = timed ? lastTime + nanos : 0L;
    AutoPtr<IThread> w = Thread::GetCurrentThread();
    Int32 spins = (ShouldSpin(s) ?
                 (timed ? mMaxTimedSpins : mMaxUntimedSpins) : 0);
    for (;;) {
        Boolean b = FALSE;
        if ((w->IsInterrupted(&b), b))
            s->TryCancel();
        AutoPtr<SNode> m = s->mMatch;
        if (m != NULL)
            return m;
        if (timed) {
            system->GetNanoTime(&lastTime);
            nanos = deadline - lastTime;
            if (nanos <= 0L) {
                s->TryCancel();
                continue;
            }
        }
        if (spins > 0)
            spins = ShouldSpin(s) ? (spins-1) : 0;
        else if (s->mWaiter == NULL)
            s->mWaiter = w; // establish waiter so can park next iter
        else if (!timed)
            LockSupport::Park(TO_IINTERFACE(this));
        else if (nanos > mSpinForTimeoutThreshold)
            LockSupport::ParkNanos(TO_IINTERFACE(this), nanos);
    }
}

Boolean CSynchronousQueue::TransferStack::ShouldSpin(
    /* [in] */ SNode* s)
{
    AutoPtr<SNode> h = mHead;
    return h.Get() == s || h.Get() == NULL || IsFulfilling(h->mMode);
}

void CSynchronousQueue::TransferStack::Clean(
    /* [in] */ SNode* s)
{
    s->mItem = NULL;   // forget item
    s->mWaiter = NULL; // forget thread

    /*
     * At worst we may need to traverse entire stack to unlink
     * s. If there are multiple concurrent calls to clean, we
     * might not see s if another thread has already removed
     * it. But we can stop when we see any node known to
     * follow s. We use s.next unless it too is cancelled, in
     * which case we try the node one past. We don't check any
     * further because we don't want to doubly traverse just to
     * find sentinel.
     */

    AutoPtr<SNode> past = s->mNext;
    if (past != NULL && past->IsCancelled())
        past = past->mNext;

    // Absorb cancelled nodes at head
    AutoPtr<SNode> p;
    while ((p = mHead) != NULL && p != past && p->IsCancelled())
        CasHead(p, p->mNext);

    // Unsplice embedded nodes
    while (p != NULL && p != past) {
        AutoPtr<SNode> n = p->mNext;
        if (n != NULL && n->IsCancelled())
            p->CasNext(n, n->mNext);
        else
            p = n;
    }
}

//===============================================================================
// CSynchronousQueue::TransferQueue::QNode::
//===============================================================================

CSynchronousQueue::TransferQueue::QNode::QNode(
    /* [in] */ IInterface* item,
    /* [in] */ Boolean isData)
{
    mItem = item;
    mIsData = isData;
}

Boolean CSynchronousQueue::TransferQueue::QNode::CasNext(
    /* [in] */ QNode* cmp,
    /* [in] */ QNode* val)
{
    return mNext.Get() == cmp &&
        CompareAndSwapObject((volatile int32_t*)&mNext, cmp, val);
}

Boolean CSynchronousQueue::TransferQueue::QNode::CasItem(
    /* [in] */ IInterface* cmp,
    /* [in] */ IInterface* val)
{
    return mItem.Get() == cmp &&
        CompareAndSwapObject((volatile int32_t*)&mItem, cmp, val);
}

void CSynchronousQueue::TransferQueue::QNode::TryCancel(
    /* [in] */ IInterface* cmp)
{
    CompareAndSwapObject((volatile int32_t*)&mItem, cmp, (IObject*)this);
}

Boolean CSynchronousQueue::TransferQueue::QNode::IsCancelled()
{
    return TO_IINTERFACE(mItem) == TO_IINTERFACE(this);
}

Boolean CSynchronousQueue::TransferQueue::QNode::IsOffList()
{
    return mNext.Get() == this;
}

//===============================================================================
// CSynchronousQueue::TransferQueue::
//===============================================================================
CAR_INTERFACE_IMPL(CSynchronousQueue::TransferQueue, Transferer, ISynchronousQueueTransferQueue)

CSynchronousQueue::TransferQueue::TransferQueue()
{
    AutoPtr<QNode> h = new QNode(NULL, FALSE); // initialize to dummy node.
    mHead = h;
    mTail = h;
}

void CSynchronousQueue::TransferQueue::AdvanceHead(
    /* [in] */ QNode* h,
    /* [in] */ QNode* nh)
{
    if (h == mHead &&
        CompareAndSwapObject((volatile int32_t*)&mHead, h, nh))
        h->mNext = h; // forget old next
}

void CSynchronousQueue::TransferQueue::AdvanceTail(
    /* [in] */ QNode* t,
    /* [in] */ QNode* nt)
{
    if (mTail.Get() == t)
        CompareAndSwapObject((volatile int32_t*)&mTail, t, nt);
}

Boolean CSynchronousQueue::TransferQueue::CasCleanMe(
    /* [in] */ QNode* cmp,
    /* [in] */ QNode* val)
{
    return mCleanMe.Get() == cmp &&
        CompareAndSwapObject((volatile int32_t*)&mCleanMe, cmp, val);
}

AutoPtr<IInterface> CSynchronousQueue::TransferQueue::Transfer(
    /* [in] */ IInterface* e,
    /* [in] */ Boolean timed,
    /* [in] */ Int64 nanos)
{
    /* Basic algorithm is to loop trying to take either of
     * two actions:
     *
     * 1. If queue apparently empty or holding same-mode nodes,
     *    try to add node to queue of waiters, wait to be
     *    fulfilled (or cancelled) and return matching item.
     *
     * 2. If queue apparently contains waiting items, and this
     *    call is of complementary mode, try to fulfill by CAS'ing
     *    item field of waiting node and dequeuing it, and then
     *    returning matching item.
     *
     * In each case, along the way, check for and try to help
     * advance head and tail on behalf of other stalled/slow
     * threads.
     *
     * The loop starts off with a null check guarding against
     * seeing uninitialized head or tail values. This never
     * happens in current SynchronousQueue, but could if
     * callers held non-volatile/final ref to the
     * transferer. The check is here anyway because it places
     * null checks at top of loop, which is usually faster
     * than having them implicitly interspersed.
     */

    AutoPtr<QNode> s; // constructed/reused as needed
    Boolean isData = (e != NULL);

    for (;;) {
        AutoPtr<QNode> t = mTail;
        AutoPtr<QNode> h = mHead;
        if (t == NULL || h == NULL)         // saw uninitialized value
            continue;                       // spin

        if (h == t ||
            t->mIsData == isData) { // empty or same-mode
            AutoPtr<QNode> tn = t->mNext;
            if (t != mTail)                  // inconsistent read
                continue;
            if (tn != NULL) {               // lagging tail
                AdvanceTail(t, tn);
                continue;
            }
            if (timed && nanos <= 0)        // can't wait
                return NULL;
            if (s == NULL)
                s = new QNode(e, isData);
            if (!t->CasNext(NULL, s))        // failed to link in
                continue;

            AdvanceTail(t, s);              // swing tail and wait
            AutoPtr<IInterface> x = AwaitFulfill(s, e, timed, nanos);
            if (TO_IINTERFACE(x) == TO_IINTERFACE(s)) {                   // wait was cancelled
                Clean(t, s);
                return NULL;
            }

            if (!s->IsOffList()) {           // not already unlinked
                AdvanceHead(t, s);          // unlink if head
                if (x != NULL)              // and forget fields
                    s->mItem = TO_IINTERFACE(s);
                s->mWaiter = NULL;
            }
            return (x != NULL) ? x.Get() : e;

        }
        else {                            // complementary-mode
            AutoPtr<QNode> m = h->mNext;               // node to fulfill
            if (t != mTail || m == NULL || h != mHead)
                continue;                   // inconsistent read

            AutoPtr<IInterface> x = m->mItem;
            if (isData == (x != NULL) ||    // m already fulfilled
                TO_IINTERFACE(x) == TO_IINTERFACE(m) ||                   // m cancelled
                !m->CasItem(x, e)) {         // lost CAS
                AdvanceHead(h, m);          // dequeue and retry
                continue;
            }

            AdvanceHead(h, m);              // successfully fulfilled
            LockSupport::Unpark(m->mWaiter);
            return (x != NULL) ? x.Get() : e;
        }
    }
}

AutoPtr<IInterface> CSynchronousQueue::TransferQueue::AwaitFulfill(
    /* [in] */ QNode* s,
    /* [in] */ IInterface* e,
    /* [in] */ Boolean timed,
    /* [in] */ Int64 nanos)
{
    /* Same idea as TransferStack.awaitFulfill */
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    Int64 lastTime = 0;
    system->GetNanoTime(&lastTime);
    Int64 deadline = timed ? lastTime + nanos : 0L;
    AutoPtr<IThread> w = Thread::GetCurrentThread();
    Int32 spins = ((mHead->mNext.Get() == s) ?
                 (timed ? mMaxTimedSpins : mMaxTimedSpins) : 0);
    for (;;) {
        Boolean b = FALSE;
        if ((w->IsInterrupted(&b), b))
            s->TryCancel(e);
        AutoPtr<IInterface> x = s->mItem;
        if (x.Get() != e)
            return x;
        if (timed) {
            system->GetNanoTime(&lastTime);
            nanos = deadline - lastTime;
            if (nanos <= 0L) {
                s->TryCancel(e);
                continue;
            }
        }
        if (spins > 0)
            --spins;
        else if (s->mWaiter == NULL)
            s->mWaiter = w;
        else if (!timed)
            LockSupport::Park(TO_IINTERFACE(this));
        else if (nanos > mSpinForTimeoutThreshold)
            LockSupport::ParkNanos(TO_IINTERFACE(this), nanos);
    }
}

void CSynchronousQueue::TransferQueue::Clean(
    /* [in] */ QNode* pred,
    /* [in] */ QNode* s)
{
    s->mWaiter = NULL; // forget thread
    /*
     * At any given time, exactly one node on list cannot be
     * deleted -- the last inserted node. To accommodate this,
     * if we cannot delete s, we save its predecessor as
     * "cleanMe", deleting the previously saved version
     * first. At least one of node s or the node previously
     * saved can always be deleted, so this always terminates.
     */
    while (pred->mNext.Get() == s) { // Return early if already unlinked
        AutoPtr<QNode> h = mHead;
        AutoPtr<QNode> hn = h->mNext;   // Absorb cancelled first node as head
        if (hn != NULL && hn->IsCancelled()) {
            AdvanceHead(h, hn);
            continue;
        }
        AutoPtr<QNode> t = mTail;      // Ensure consistent read for tail
        if (t == h)
            return;
        AutoPtr<QNode> tn = t->mNext;
        if (t != mTail)
            continue;
        if (tn != NULL) {
            AdvanceTail(t, tn);
            continue;
        }
        if (s != t) {        // If not tail, try to unsplice
            AutoPtr<QNode> sn = s->mNext;
            if (sn.Get() == s ||
                pred->CasNext(s, sn))
                return;
        }
        AutoPtr<QNode> dp = mCleanMe;
        if (dp != NULL) {    // Try unlinking previous cancelled node
            AutoPtr<QNode> d = dp->mNext;
            AutoPtr<QNode> dn;
            if (d == NULL ||               // d is gone or
                d == dp ||                 // d is off list or
                !d->IsCancelled() ||        // d not cancelled or
                (d != t &&                 // d not tail and
                 (dn = d->mNext) != NULL &&  //   has successor
                 dn != d &&                //   that is on list
                 dp->CasNext(d, dn)))       // d unspliced
                CasCleanMe(dp, NULL);
            if (dp.Get() == pred)
                return;      // s is already saved node
        }
        else if (CasCleanMe(NULL, pred))
            return;          // Postpone cleaning s
    }
}

//===============================================================================
// CSynchronousQueue::WaitQueue::
//===============================================================================
CAR_INTERFACE_IMPL_2(CSynchronousQueue::WaitQueue, Object, IWaitQueue, ISerializable)

//===============================================================================
// CSynchronousQueue::LifoWaitQueue::
//===============================================================================
Int64 CSynchronousQueue::LifoWaitQueue::mSerialVersionUID = -3633113410248163686L;

CAR_INTERFACE_IMPL(CSynchronousQueue::LifoWaitQueue, WaitQueue, ILifoWaitQueue)

//===============================================================================
// CSynchronousQueue::FifoWaitQueue::
//===============================================================================
Int64 CSynchronousQueue::FifoWaitQueue::mSerialVersionUID = -3623113410248163686L;

CAR_INTERFACE_IMPL(CSynchronousQueue::FifoWaitQueue, WaitQueue, IFifoWaitQueue)

//===============================================================================
// CSynchronousQueue::EmptyIterator::
//===============================================================================
AutoPtr<CSynchronousQueue::EmptyIterator> CSynchronousQueue::EmptyIterator::EMPTY_ITERATOR = new EmptyIterator();

CAR_INTERFACE_IMPL(CSynchronousQueue::EmptyIterator, Object, IIterator)

ECode CSynchronousQueue::EmptyIterator::HasNext(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = FALSE;
    return NOERROR;
}

ECode CSynchronousQueue::EmptyIterator::GetNext(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)

    return E_NO_SUCH_ELEMENT_EXCEPTION;
}

ECode CSynchronousQueue::EmptyIterator::Remove()
{
    return E_ILLEGAL_STATE_EXCEPTION;
}

//===============================================================================
// CSynchronousQueue::
//===============================================================================
static Int32 GetCpuCount()
{
    AutoPtr<IOs> os;
    AutoPtr<ILibcore> libcore;
    CLibcore::AcquireSingleton((ILibcore**)&libcore);
    libcore->GetOs((IOs**)&os);
    Int64 ival = 4;
    os->Sysconf(OsConstants::__SC_NPROCESSORS_CONF, &ival);
    assert(ival > 0);
    return (Int32)ival;
}

Int64 CSynchronousQueue::mSerialVersionUID = -3223113410248163686L;

Int32 CSynchronousQueue::NCPUS = GetCpuCount();

Int32 CSynchronousQueue::mMaxTimedSpins = (NCPUS < 2) ? 0 : 32;

Int32 CSynchronousQueue::mMaxUntimedSpins = mMaxTimedSpins * 16;

Int64 CSynchronousQueue::mSpinForTimeoutThreshold = 1000L;

CAR_INTERFACE_IMPL_3(CSynchronousQueue, AbstractQueue, ISynchronousQueue, IBlockingQueue, ISerializable)

CAR_OBJECT_IMPL(CSynchronousQueue)

ECode CSynchronousQueue::constructor()
{
    return constructor(FALSE);
}

ECode CSynchronousQueue::constructor(
    /* [in] */ Boolean fair)
{
    if (fair) {
        AutoPtr<TransferQueue> p = new TransferQueue();
        mTransferer = (Transferer*)p.Get();
    }
    else {
        AutoPtr<TransferStack> p = new TransferStack();
        mTransferer = (Transferer*)p.Get();
    }
    return NOERROR;
}

ECode CSynchronousQueue::Put(
    /* [in] */ IInterface* e)
{
    if (e == NULL) return E_NULL_POINTER_EXCEPTION;
    if (mTransferer->Transfer(e, FALSE, 0) == NULL) {
        Thread::Interrupted();
        return E_INTERRUPTED_EXCEPTION;
    }
    return NOERROR;
}

ECode CSynchronousQueue::Offer(
    /* [in] */ IInterface* e,
    /* [in] */ Int64 timeout,
    /* [in] */ ITimeUnit* unit,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (e == NULL) return E_NULL_POINTER_EXCEPTION;
    Int64 n;
    unit->ToNanos(timeout, &n);
    if (mTransferer->Transfer(e, TRUE, n) != NULL) {
        *result = TRUE;
        return NOERROR;
    }
    if (!Thread::Interrupted()) {
        *result = FALSE;
        return NOERROR;
    }
    return E_INTERRUPTED_EXCEPTION;
}

ECode CSynchronousQueue::Offer(
    /* [in] */ IInterface* e,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (e == NULL) return E_NULL_POINTER_EXCEPTION;
    *result = mTransferer->Transfer(e, TRUE, 0) != NULL;
    return NOERROR;
}

ECode CSynchronousQueue::Take(
    /* [out] */ IInterface** res)
{
    VALIDATE_NOT_NULL(res)

    AutoPtr<IInterface> e = mTransferer->Transfer(NULL, FALSE, 0);
    if (e != NULL) {
        *res = e;
        REFCOUNT_ADD(*res)
        return NOERROR;
    }
    Thread::Interrupted();
    return E_INTERRUPTED_EXCEPTION;
}

ECode CSynchronousQueue::Poll(
    /* [in] */ Int64 timeout,
    /* [in] */ ITimeUnit* unit,
    /* [out] */ IInterface** res)
{
    VALIDATE_NOT_NULL(res)

    Int64 n;
    unit->ToNanos(timeout, &n);
    AutoPtr<IInterface> e = mTransferer->Transfer(NULL, TRUE, n);
    if (e != NULL || !Thread::Interrupted()) {
        *res = e;
        REFCOUNT_ADD(*res)
        return NOERROR;
    }
    return E_INTERRUPTED_EXCEPTION;
}

ECode CSynchronousQueue::Poll(
    /* [out] */ IInterface** res)
{
    VALIDATE_NOT_NULL(res)
    AutoPtr<IInterface> temp = mTransferer->Transfer(NULL, TRUE, 0);
    *res = temp;
    REFCOUNT_ADD(*res)
    return NOERROR;
}

ECode CSynchronousQueue::IsEmpty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = TRUE;
    return NOERROR;
}

ECode CSynchronousQueue::GetSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size)

    *size = 0;
    return NOERROR;
}

ECode CSynchronousQueue::RemainingCapacity(
    /* [out] */ Int32* capacity)
{
    VALIDATE_NOT_NULL(capacity)

    *capacity = 0;
    return NOERROR;
}

ECode CSynchronousQueue::Clear()
{
    return NOERROR;
}

ECode CSynchronousQueue::Contains(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = FALSE;
    return NOERROR;
}

ECode CSynchronousQueue::Remove(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    VALIDATE_NOT_NULL(modified)

    *modified = FALSE;
    return NOERROR;
}

ECode CSynchronousQueue::ContainsAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    return collection->IsEmpty(result);
}

ECode CSynchronousQueue::RemoveAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    VALIDATE_NOT_NULL(modified)

    *modified = FALSE;
    return NOERROR;
}

ECode CSynchronousQueue::RetainAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    VALIDATE_NOT_NULL(modified)

    *modified = FALSE;
    return NOERROR;
}

ECode CSynchronousQueue::Peek(
    /* [out] */ IInterface** e)
{
    VALIDATE_NOT_NULL(e)

    *e = NULL;
    return NOERROR;
}

ECode CSynchronousQueue::GetIterator(
    /* [out] */ IIterator** it)
{
    VALIDATE_NOT_NULL(it)

    *it = EmptyIterator::EMPTY_ITERATOR;
    REFCOUNT_ADD(*it)
    return NOERROR;
}

ECode CSynchronousQueue::ToArray(
    /* [out, callee] */ ArrayOf<IInterface*>** array)
{
    VALIDATE_NOT_NULL(array)

    *array = ArrayOf<IInterface*>::Alloc(1);
    REFCOUNT_ADD(*array)
    return NOERROR;
}

ECode CSynchronousQueue::ToArray(
    /* [in] */ ArrayOf<IInterface*>* inArray,
    /* [out, callee] */ ArrayOf<IInterface*>** outArray)
{
    VALIDATE_NOT_NULL(outArray)

    if (inArray->GetLength() > 0)
        (*inArray)[0] = NULL;
    *outArray = inArray;
    REFCOUNT_ADD(*outArray)
    return NOERROR;
}

ECode CSynchronousQueue::DrainTo(
    /* [in] */ ICollection* c,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number)

    if (c == NULL)
        return E_NULL_POINTER_EXCEPTION;
    if (c == (ICollection*)this)
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    Int32 n = 0;
    for (AutoPtr<IInterface> e; (Poll((IInterface**)&e), e) != NULL;) {
        c->Add(e);
        ++n;
    }
    *number = n;
    return NOERROR;
}

ECode CSynchronousQueue::DrainTo(
    /* [in] */ ICollection* c,
    /* [in] */ Int32 maxElements,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number)

    if (c == NULL)
        return E_NULL_POINTER_EXCEPTION;
    if (c == (ICollection*)this)
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    Int32 n = 0;
    for (AutoPtr<IInterface> e; n < maxElements && (Poll((IInterface**)&e), e) != NULL;) {
        c->Add(e);
        ++n;
    }
    *number = n;
    return NOERROR;
}

void CSynchronousQueue::WriteObject(
    /* [in] */ IObjectOutputStream* s)
{
    Boolean fair = mTransferer->Probe(EIID_ISynchronousQueueTransferQueue) != NULL;
    if (fair) {
        CReentrantLock::New(TRUE, (IReentrantLock**)&mQlock);
        mWaitingProducers = new FifoWaitQueue();
        mWaitingConsumers = new FifoWaitQueue();
    }
    else {
        CReentrantLock::New((IReentrantLock**)&mQlock);
        mWaitingProducers = new LifoWaitQueue();
        mWaitingConsumers = new LifoWaitQueue();
    }
    s->DefaultWriteObject();
}

void CSynchronousQueue::ReadObject(
    /* [in] */ IObjectInputStream* s)
{
    s->DefaultReadObject();
    if (IFifoWaitQueue::Probe(mWaitingProducers) != NULL)
        mTransferer = new TransferQueue();
    else
        mTransferer = new TransferStack();
}

} // namespace Concurrent
} // namespace Utility
} // namespace Elastos
