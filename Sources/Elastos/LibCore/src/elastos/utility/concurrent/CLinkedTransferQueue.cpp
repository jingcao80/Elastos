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
#include "CLinkedTransferQueue.h"
#include "LockSupport.h"
#include <Math.h>
#include "CSystem.h"
#include "Thread.h"
#include "CLibcore.h"
#include <elastos/droid/system/OsConstants.h>

using Elastos::Core::Thread;
using Elastos::Core::ISystem;
using Elastos::Core::Math;
using Elastos::Utility::Concurrent::Locks::LockSupport;
using Elastos::Droid::System::OsConstants;
using Libcore::IO::IOs;
using Libcore::IO::ILibcore;
using Libcore::IO::CLibcore;

namespace Elastos {
namespace Utility {
namespace Concurrent {

//====================================================================
// CLinkedTransferQueue::Node::
//====================================================================
Boolean CLinkedTransferQueue::Node::CasNext(
    /* [in] */ Node* cmp,
    /* [in] */ Node* val)
{
    assert(0);
//    return UNSAFE.compareAndSwapObject(this, nextOffset, cmp, val);
    return FALSE;
}

Boolean CLinkedTransferQueue::Node::CasItem(
    /* [in] */ IInterface* cmp,
    /* [in] */ IInterface* val)
{
    assert(0);
    // assert cmp == null || cmp.getClass() != Node.class;
//    return UNSAFE.compareAndSwapObject(this, itemOffset, cmp, val);
    return FALSE;
}

CLinkedTransferQueue::Node::Node(
    /* [in] */ IInterface* item,
    /* [in] */ Boolean isData)
{
    assert(0);
//    UNSAFE.putObject(this, itemOffset, item); // relaxed write
    mIsData = isData;
}

void CLinkedTransferQueue::Node::ForgetNext()
{
    assert(0);
//    UNSAFE.putObject(this, nextOffset, this);
}

void CLinkedTransferQueue::Node::ForgetContents()
{
    assert(0);
    // UNSAFE.putObject(this, itemOffset, this);
    // UNSAFE.putObject(this, waiterOffset, null);
}

Boolean CLinkedTransferQueue::Node::IsMatched()
{
    AutoPtr<IInterface> x = mItem;
    return (Object::Equals(x, TO_IINTERFACE(this))) || ((x == NULL) == mIsData);
}

Boolean CLinkedTransferQueue::Node::IsUnmatchedRequest()
{
    return !mIsData && mItem == NULL;
}

Boolean CLinkedTransferQueue::Node::CannotPrecede(
    /* [in] */ Boolean haveData)
{
    Boolean d = mIsData;
    AutoPtr<IInterface> x;
    return d != haveData && ((x = mItem), !Object::Equals(x, TO_IINTERFACE(this))) && (x != NULL) == d;
}

Boolean CLinkedTransferQueue::Node::TryMatchData()
{
    // assert isData;
    AutoPtr<IInterface> x = mItem;
    if (x != NULL && (!Object::Equals(x, TO_IINTERFACE(this))) && CasItem(x, NULL)) {
        LockSupport::Unpark(mWaiter);
        return TRUE;
    }
    return FALSE;
}

Int64 CLinkedTransferQueue::Node::sSerialVersionUID = -3375979862319811754L;

//====================================================================
// CLinkedTransferQueue::
//====================================================================

static Int32 GetCpuCount()
{
    AutoPtr<IOs> os;
    AutoPtr<ILibcore> libcore;
    CLibcore::AcquireSingleton((ILibcore**)&libcore);
    libcore->GetOs((IOs**)&os);
    Int64 ival = 4;
    os->Sysconf(OsConstants::__SC_NPROCESSORS_CONF, &ival);
    return (Int32)ival;
}

Boolean CLinkedTransferQueue::MP = GetCpuCount() > 1;

Int32 CLinkedTransferQueue::FRONT_SPINS   = 1 << 7;

Int32 CLinkedTransferQueue::CHAINED_SPINS = FRONT_SPINS >> 1;

Int32 CLinkedTransferQueue::SWEEP_THRESHOLD = 32;

CAR_INTERFACE_IMPL_2(CLinkedTransferQueue, AbstractQueue, ITransferQueue, IBlockingQueue)

CAR_OBJECT_IMPL(CLinkedTransferQueue)

Boolean CLinkedTransferQueue::CasTail(
    /* [in] */ Node* cmp,
    /* [in] */ Node* val)
{
    assert(0);
//    return UNSAFE.compareAndSwapObject(this, tailOffset, cmp, val);
    return FALSE;
}

Boolean CLinkedTransferQueue::CasHead(
    /* [in] */ Node* cmp,
    /* [in] */ Node* val)
{
    assert(0);
//    return UNSAFE.compareAndSwapObject(this, headOffset, cmp, val);
    return FALSE;
}

Boolean CLinkedTransferQueue::CasSweepVotes(
    /* [in] */ Int32 cmp,
    /* [in] */ Int32 val)
{
    assert(0);
//    return UNSAFE.compareAndSwapInt(this, sweepVotesOffset, cmp, val);
    return FALSE;
}

Int32 CLinkedTransferQueue::NOW   = 0;
Int32 CLinkedTransferQueue::ASYNC = 1;
Int32 CLinkedTransferQueue::SYNC  = 2;
Int32 CLinkedTransferQueue::TIMED = 3;

// static <E> E cast(Object item)
// {
//     // assert item == NULL || item.getClass() != Node.class;
//     return (E) item;
// }

AutoPtr<IInterface> CLinkedTransferQueue::Xfer(
    /* [in] */ IInterface* e,
    /* [in] */ Boolean haveData,
    /* [in] */ Int32 how,
    /* [in] */ Int64 nanos)
{
    if (haveData && (e == NULL))
        return NULL;
//        return E_NULL_POINTER_EXCEPTION;
    AutoPtr<Node> s = NULL;                        // the node to append, if needed

    retry:
    for (;;) {                            // restart on append race

        for (AutoPtr<Node> h = mHead, p = h; p != NULL;) { // find & match first node
            Boolean isData = p->mIsData;
            AutoPtr<IInterface> item = p->mItem;
            if ((!Object::Equals(item, p->Probe(EIID_IInterface))) && (item != NULL) == isData) { // unmatched
                if (isData == haveData)   // can't match
                    break;
                if (p->CasItem(item, e)) { // match
                    for (AutoPtr<Node> q = p; !Object::Equals(q->Probe(EIID_IInterface), h->Probe(EIID_IInterface));) {
                        AutoPtr<Node> n = q->mNext;  // update by 2 unless singleton
                        if ( (Object::Equals(mHead->Probe(EIID_IInterface), h->Probe(EIID_IInterface))) && CasHead(h, n == NULL ? q : n)) {
                            h->ForgetNext();
                            break;
                        }                 // advance and retry
                        if ((h = mHead)   == NULL ||
                            (q = h->mNext) == NULL || !q->IsMatched())
                            break;        // unless slack < 2
                    }
                    LockSupport::Unpark(p->mWaiter);
                    return item;
//                    return CLinkedTransferQueue.Cast(item);
                }
            }
            AutoPtr<Node> n = p->mNext;
            p = (!Object::Equals(p->Probe(EIID_IInterface), n->Probe(EIID_IInterface))) ? n : (h = mHead); // Use head if p offlist
        }

        if (how != NOW) {                 // No matches available
            if (s == NULL)
                s = new Node(e, haveData);
            AutoPtr<Node> pred = TryAppend(s, haveData);
            if (pred == NULL)
                goto retry;           // lost race vs opposite mode
            if (how != ASYNC)
                return AwaitMatch(s, pred, e, (how == TIMED), nanos);
        }
        return e; // not waiting
    }
}

AutoPtr<CLinkedTransferQueue::Node> CLinkedTransferQueue::TryAppend(
    /* [in] */ Node* s,
    /* [in] */ Boolean haveData)
{
    for (AutoPtr<Node> t = mTail, p = t;;) {        // move p to last node and append
        AutoPtr<Node> n, u;                        // temps for reads of next & tail
        if (p == NULL && (p = mHead) == NULL) {
            if (CasHead(NULL, s))
                return s;                 // initialize
        }
        else if (p->CannotPrecede(haveData))
            return NULL;                  // lost race vs opposite mode
        else if ((n = p->mNext) != NULL)    // not last; keep traversing
            p = (!Object::Equals(p->Probe(EIID_IInterface), t->Probe(EIID_IInterface))) &&
                (u = mTail, !Object::Equals(t->Probe(EIID_IInterface), u->Probe(EIID_IInterface)))
                ? (t = u) : // stale tail
                (!Object::Equals(p->Probe(EIID_IInterface), n->Probe(EIID_IInterface))) ? n : NULL;      // restart if off list
        else if (!p->CasNext(NULL, s))
            p = p->mNext;                   // re-read on CAS failure
        else {
            if (!Object::Equals(p->Probe(EIID_IInterface), t->Probe(EIID_IInterface))) {                 // update if slack now >= 2
                while (((!Object::Equals(mTail->Probe(EIID_IInterface), t->Probe(EIID_IInterface))) || !CasTail(t, s)) &&
                       (t = mTail)   != NULL &&
                       (s = t->mNext) != NULL && // advance and retry
                       (s = s->mNext) != NULL &&
                       !Object::Equals(s->Probe(EIID_IInterface), t->Probe(EIID_IInterface)));
            }
            return p;
        }
    }
}

AutoPtr<IInterface> CLinkedTransferQueue::AwaitMatch(
    /* [in] */ Node* s,
    /* [in] */ Node* pred,
    /* [in] */ IInterface* e,
    /* [in] */ Boolean timed,
    /* [in] */ Int64 nanos)
{
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    Int64 deadline = 0ll;
    if (timed) {
        system->GetNanoTime(&deadline);
        deadline += nanos;
    }
    AutoPtr<IThread> w = Thread::GetCurrentThread();
    Int32 spins = -1; // initialized after first item and cancel checks
    AutoPtr<IThreadLocalRandom> randomYields; // bound if needed

    for (;;) {
        AutoPtr<IInterface> item = s->mItem;
        if (!Object::Equals(item, e)) {                  // matched
            // assert item != s;
            s->ForgetContents();           // avoid garbage
            return item;
//            return LinkedTransferQueue.<E>cast(item);
        }
        Boolean b = FALSE;
        if (((w->IsInterrupted(&b), b) || (timed && nanos <= 0)) &&
                s->CasItem(e, s->Probe(EIID_IInterface))) {        // cancel
            Unsplice(pred, s);
            return e;
        }

        if (spins < 0) {                  // establish spins at/near front
            assert(0 && "TODO");
            if ((spins = SpinsFor(pred, s->mIsData)) > 0)
                randomYields;// = ThreadLocalRandom.current();
        }
        else if (spins > 0) {             // spin
            --spins;
            Int32 num = 0;
            if (((IRandom::Probe(randomYields))->NextInt32(CHAINED_SPINS, &num), num == 0))
                Thread::Yield();           // occasionally yield
        }
        else if (s->mWaiter == NULL) {
            s->mWaiter = w;                 // request unpark then recheck
        }
        else if (timed) {
            Int64 now = 0;
            system->GetNanoTime(&now);
            nanos = deadline - now;
             if (nanos > 0)
                LockSupport::ParkNanos(TO_IINTERFACE(this), nanos);
        }
        else {
            LockSupport::Park(TO_IINTERFACE(this));
        }
    }
}

Int32 CLinkedTransferQueue::SpinsFor(
    /* [in] */ Node* pred,
    /* [in] */ Boolean haveData)
{
    if (MP && pred != NULL) {
        if (pred->mIsData != haveData)      // phase change
            return FRONT_SPINS + CHAINED_SPINS;
        if (pred->IsMatched())             // probably at front
            return FRONT_SPINS;
        if (pred->mWaiter == NULL)          // pred apparently spinning
            return CHAINED_SPINS;
    }
    return 0;
}

AutoPtr<CLinkedTransferQueue::Node> CLinkedTransferQueue::Succ(
    /* [in] */ Node* p)
{
    AutoPtr<Node> next = p->mNext;
    return (Object::Equals(p->Probe(EIID_IInterface), next->Probe(EIID_IInterface))) ? mHead : next;
}

AutoPtr<CLinkedTransferQueue::Node> CLinkedTransferQueue::FirstOfMode(
    /* [in] */ Boolean isData)
{
    for (AutoPtr<Node> p = mHead; p != NULL; p = Succ(p)) {
        if (!p->IsMatched())
            return (p->mIsData == isData) ? p : NULL;
    }
    return NULL;
}

AutoPtr<IInterface> CLinkedTransferQueue::FirstDataItem()
{
    for (AutoPtr<Node> p = mHead; p != NULL; p = Succ(p)) {
        AutoPtr<IInterface> item = p->mItem;
        if (p->mIsData) {
            if (item != NULL && !Object::Equals(item, p->Probe(EIID_IInterface)))
                return item;
//                return LinkedTransferQueue.<E>cast(item);
        }
        else if (item == NULL)
            return NULL;
    }
    return NULL;
}

Int32 CLinkedTransferQueue::CountOfMode(
    /* [in] */ Boolean data)
{
    Int32 count = 0;
    for (AutoPtr<Node> p = mHead; p != NULL; ) {
        if (!p->IsMatched()) {
            if (p->mIsData != data)
                return 0;
            if (++count == Elastos::Core::Math::INT32_MAX_VALUE) // saturated
                break;
        }
        AutoPtr<Node> n = p->mNext;
        if (!Object::Equals(n->Probe(EIID_IInterface), p->Probe(EIID_IInterface)))
            p = n;
        else {
            count = 0;
            p = mHead;
        }
    }
    return count;
}

//====================================================================
// CLinkedTransferQueue::Itr::
//====================================================================
CAR_INTERFACE_IMPL(CLinkedTransferQueue::Itr, Object, IIterator)

void CLinkedTransferQueue::Itr::Advance(
    /* [in] */ Node* prev)
{
    /*
     * To track and avoid buildup of deleted nodes in the face
     * of calls to both Queue.remove and Itr.remove, we must
     * include variants of unsplice and sweep upon each
     * advance: Upon Itr.remove, we may need to catch up links
     * from lastPred, and upon other removes, we might need to
     * skip ahead from stale nodes and unsplice deleted ones
     * found while advancing.
     */

    AutoPtr<Node> r, b; // reset lastPred upon possible deletion of lastRet
    if ((r = mLastRet) != NULL && !r->IsMatched())
        mLastPred = r;    // next lastPred is old lastRet
    else if ((b = mLastPred) == NULL || b->IsMatched())
        mLastPred = NULL; // at start of list
    else {
        AutoPtr<Node> s, n;       // help with removal of lastPred.next
        while ((s = b->mNext) != NULL &&
                (!Object::Equals(s->Probe(EIID_IInterface), b->Probe(EIID_IInterface))) &&
                s->IsMatched() &&
                (n = s->mNext) != NULL &&
                (!Object::Equals(n->Probe(EIID_IInterface), s->Probe(EIID_IInterface))))
            b->CasNext(s, n);
    }

    mLastRet = prev;

    for (AutoPtr<Node> p = prev, s, n;;) {
        s = (p == NULL) ? mOwner->mHead : p->mNext;
        if (s == NULL)
            break;
        else if (Object::Equals(s->Probe(EIID_IInterface), p->Probe(EIID_IInterface))) {
            p = NULL;
            continue;
        }
        AutoPtr<IInterface> item = s->mItem;
        if (s->mIsData) {
            if (item != NULL && !(Object::Equals(item, s->Probe(EIID_IInterface)))) {
//                mNextItem = LinkedTransferQueue.<E>cast(item);
                mNextItem = item;
                mNextNode = s;
                return;
            }
        }
        else if (item == NULL)
            break;
        // assert s.isMatched();
        if (p == NULL)
            p = s;
        else if ((n = s->mNext) == NULL)
            break;
        else if (Object::Equals(s->Probe(EIID_IInterface), n->Probe(EIID_IInterface)))
            p = NULL;
        else
            p->CasNext(s, n);
    }
    mNextNode = NULL;
    mNextItem = NULL;
}

CLinkedTransferQueue::Itr::Itr(
    /* [in] */ CLinkedTransferQueue* owner)
{
    mOwner = owner;
    Advance(NULL);
}

ECode CLinkedTransferQueue::Itr::HasNext(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mNextNode != NULL;
    return NOERROR;
}

ECode CLinkedTransferQueue::Itr::GetNext(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);
    AutoPtr<Node> p = mNextNode;
    if (p == NULL) {
        *object = NULL;
        return E_NO_SUCH_ELEMENT_EXCEPTION;
    }
    AutoPtr<IInterface> e = mNextItem;
    Advance(p);
    *object = e;
    REFCOUNT_ADD(*object);
    return NOERROR;
}

ECode CLinkedTransferQueue::Itr::Remove()
{
    AutoPtr<Node> lastRet = mLastRet;
    if (lastRet == NULL)
        return E_ILLEGAL_STATE_EXCEPTION;
    mLastRet = NULL;
    if (lastRet->TryMatchData())
        mOwner->Unsplice(mLastPred, lastRet);
    return NOERROR;
}

//====================================================================
// CLinkedTransferQueue::
//====================================================================

void CLinkedTransferQueue::Unsplice(
    /* [in] */ Node* pred,
    /* [in] */ Node* s)
{
    s->ForgetContents(); // forget unneeded fields
    /*
     * See above for rationale. Briefly: if pred still points to
     * s, try to unlink s.  If s cannot be unlinked, because it is
     * trailing node or pred might be unlinked, and neither pred
     * nor s are head or offlist, add to sweepVotes, and if enough
     * votes have accumulated, sweep.
     */
    if (pred != NULL &&
        !Object::Equals(pred->Probe(EIID_IInterface), s->Probe(EIID_IInterface)) &&
        Object::Equals(pred->mNext->Probe(EIID_IInterface), s->Probe(EIID_IInterface))) {
        AutoPtr<Node> n = s->mNext;
        if (n == NULL ||
            (!Object::Equals(n->Probe(EIID_IInterface), s->Probe(EIID_IInterface)) && pred->CasNext(s, n)
                && pred->IsMatched())) {
            for (;;) {               // check if at, or could be, head
                AutoPtr<Node> h = mHead;
                if (Object::Equals(h->Probe(EIID_IInterface), pred->Probe(EIID_IInterface)) ||
                    Object::Equals(h->Probe(EIID_IInterface), s->Probe(EIID_IInterface)) ||
                    h == NULL)
                    return;          // at head or list empty
                if (!h->IsMatched())
                    break;
                AutoPtr<Node> hn = h->mNext;
                if (hn == NULL)
                    return;          // now empty
                if (!Object::Equals(hn->Probe(EIID_IInterface), h->Probe(EIID_IInterface)) &&
                    CasHead(h, hn))
                    h->ForgetNext();  // advance head
            }
            if (!Object::Equals(pred->mNext->Probe(EIID_IInterface), pred->Probe(EIID_IInterface)) &&
                !Object::Equals(s->mNext->Probe(EIID_IInterface), s->Probe(EIID_IInterface))) { // recheck if offlist
                for (;;) {           // sweep now if enough votes
                    Int32 v = mSweepVotes;
                    if (v < SWEEP_THRESHOLD) {
                        if (CasSweepVotes(v, v + 1))
                            break;
                    }
                    else if (CasSweepVotes(v, 0)) {
                        Sweep();
                        break;
                    }
                }
            }
        }
    }
}

void CLinkedTransferQueue::Sweep()
{
    for (AutoPtr<Node> p = mHead, s, n; p != NULL && (s = p->mNext) != NULL; ) {
        if (!s->IsMatched())
            // Unmatched nodes are never self-linked
            p = s;
        else if ((n = s->mNext) == NULL) // trailing node is pinned
            break;
        else if (Object::Equals(s->Probe(EIID_IInterface), n->Probe(EIID_IInterface)))    // stale
            // No need to also check for p == s, since that implies s == n
            p = mHead;
        else
            p->CasNext(s, n);
    }
}

Boolean CLinkedTransferQueue::FindAndRemove(
    /* [in] */ IInterface* e)
{
    if (e != NULL) {
        for (AutoPtr<Node> pred = NULL, p = mHead; p != NULL; ) {
            AutoPtr<IInterface> item = p->mItem;
            if (p->mIsData) {
                if (item != NULL &&
                    !Object::Equals(item, p->Probe(EIID_IInterface)) &&
                    Object::Equals(e, item) &&
                    p->TryMatchData()) {
                    Unsplice(pred, p);
                    return TRUE;
                }
            }
            else if (item == NULL)
                break;
            pred = p;
            if (((p = p->mNext), Object::Equals(p->Probe(EIID_IInterface), pred->Probe(EIID_IInterface)))) { // stale
                pred = NULL;
                p = mHead;
            }
        }
    }
    return FALSE;
}

ECode CLinkedTransferQueue::constructor()
{
    return NOERROR;
}

ECode CLinkedTransferQueue::constructor(
    /* [in] */ ICollection* c)
{
    constructor();
    Boolean b = FALSE;
    AbstractQueue::AddAll(c, &b);
    return NOERROR;
}

ECode CLinkedTransferQueue::Put(
    /* [in] */ IInterface* e)
{
    Xfer(e, TRUE, ASYNC, 0);
    return NOERROR;
}

ECode CLinkedTransferQueue::Offer(
    /* [in] */ IInterface* e,
    /* [in] */ Int64 timeout,
    /* [in] */ ITimeUnit* unit,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Xfer(e, TRUE, ASYNC, 0);
    *result = TRUE;
    return NOERROR;
}

ECode CLinkedTransferQueue::Offer(
    /* [in] */ IInterface* e,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Xfer(e, TRUE, ASYNC, 0);
    *result = TRUE;
    return NOERROR;
}

ECode CLinkedTransferQueue::Add(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    VALIDATE_NOT_NULL(modified);
    Xfer(object, TRUE, ASYNC, 0);
    *modified = TRUE;
    return NOERROR;
}

ECode CLinkedTransferQueue::TryTransfer(
    /* [in] */ IInterface* e,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    *value = Xfer(e, TRUE, NOW, 0) == NULL;
    return NOERROR;
}

ECode CLinkedTransferQueue::Transfer(
    /* [in] */ IInterface* e)
{
    if (Xfer(e, TRUE, SYNC, 0) != NULL) {
        Thread::Interrupted(); // failure possible only due to interrupt
        return E_INTERRUPTED_EXCEPTION;
    }
    return NOERROR;
}

ECode CLinkedTransferQueue::TryTransfer(
    /* [in] */ IInterface* e,
    /* [in] */ Int64 timeout,
    /* [in] */ ITimeUnit* unit,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    Int64 nanos = 0ll;
    unit->ToNanos(timeout, &nanos);
    if (Xfer(e, TRUE, TIMED, nanos) == NULL) {
        *value = TRUE;
        return NOERROR;
    }
    if (!Thread::Interrupted()) {
        *value = FALSE;
        return NOERROR;
    }
    return E_INTERRUPTED_EXCEPTION;
}

ECode CLinkedTransferQueue::Take(
    /* [out] */ IInterface** res)
{
    VALIDATE_NOT_NULL(res);
    AutoPtr<IInterface> e = Xfer(NULL, FALSE, SYNC, 0);
    if (e != NULL) {
        *res = e;
        REFCOUNT_ADD(*res);
        return NOERROR;
    }
    Thread::Interrupted();
    return E_INTERRUPTED_EXCEPTION;
}

ECode CLinkedTransferQueue::Poll(
    /* [in] */ Int64 timeout,
    /* [in] */ ITimeUnit* unit,
    /* [out] */ IInterface** e)
{
    VALIDATE_NOT_NULL(e);
    Int64 nanos = 0ll;
    unit->ToNanos(timeout, &nanos);
    AutoPtr<IInterface> res = Xfer(NULL, FALSE, TIMED, nanos);
    if (res != NULL || !Thread::Interrupted()) {
        *e = res;
        REFCOUNT_ADD(*e);
        return NOERROR;
    }
    return E_INTERRUPTED_EXCEPTION;
}

ECode CLinkedTransferQueue::Poll(
    /* [out] */ IInterface** e)
{
    VALIDATE_NOT_NULL(e);
    AutoPtr<IInterface> res = Xfer(NULL, FALSE, NOW, 0);
    *e = res.Get();
    REFCOUNT_ADD(*e);
    return NOERROR;
}

ECode CLinkedTransferQueue::DrainTo(
    /* [in] */ ICollection* c,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number);
    if (c == NULL)
        return E_NULL_POINTER_EXCEPTION;
    if (Object::Equals(c, TO_IINTERFACE(this)))
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    Int32 n = 0;
    for (AutoPtr<IInterface> e; (Poll((IInterface**)&e), e != NULL);) {
        Boolean b;
        c->Add(e, &b);
        ++n;
    }
    *number = n;
    return NOERROR;
}

ECode CLinkedTransferQueue::DrainTo(
    /* [in] */ ICollection* c,
    /* [in] */ Int32 maxElements,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number);
    if (c == NULL)
        return E_NULL_POINTER_EXCEPTION;
    if (Object::Equals(c, TO_IINTERFACE(this)))
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    Int32 n = 0;
    for (AutoPtr<IInterface> e; n < maxElements && (Poll((IInterface**)&e), e != NULL);) {
        Boolean b;
        c->Add(e, &b);
        ++n;
    }
    *number = n;
    return NOERROR;
}

ECode CLinkedTransferQueue::GetIterator(
    /* [out] */ IIterator** it)
{
    VALIDATE_NOT_NULL(it);
    AutoPtr<IIterator> p = new Itr(this);
    *it = p.Get();
    REFCOUNT_ADD(*it);
    return NOERROR;
}

ECode CLinkedTransferQueue::Peek(
    /* [out] */ IInterface** e)
{
    VALIDATE_NOT_NULL(e);
    AutoPtr<IInterface> p = FirstDataItem();
    *e = p.Get();
    REFCOUNT_ADD(*e);
    return NOERROR;
}

ECode CLinkedTransferQueue::IsEmpty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    for (AutoPtr<Node> p = mHead; p != NULL; p = Succ(p)) {
        if (!p->IsMatched()) {
            *result = !p->mIsData;
            return NOERROR;
        }
    }
    *result = TRUE;
    return NOERROR;
}

ECode CLinkedTransferQueue::HasWaitingConsumer(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    *value = FirstOfMode(FALSE) != NULL;
    return NOERROR;
}

ECode CLinkedTransferQueue::GetSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size);
    *size = CountOfMode(TRUE);
    return NOERROR;
}

ECode CLinkedTransferQueue::GetWaitingConsumerCount(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = CountOfMode(FALSE);
    return NOERROR;
}

ECode CLinkedTransferQueue::Remove(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    VALIDATE_NOT_NULL(modified);
    *modified = FindAndRemove(object);
    return NOERROR;
}

ECode CLinkedTransferQueue::Contains(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (object == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    for (AutoPtr<Node> p = mHead; p != NULL; p = Succ(p)) {
        AutoPtr<IInterface> item = p->mItem;
        if (p->mIsData) {
            if (item != NULL &&
                !Object::Equals(item, p->Probe(EIID_IInterface)) &&
                Object::Equals(object, item)) {
                *result = TRUE;
                return NOERROR;
            }
        }
        else if (item == NULL)
            break;
    }
    *result = FALSE;
    return NOERROR;
}

ECode CLinkedTransferQueue::RemainingCapacity(
    /* [out] */ Int32* capacity)
{
    VALIDATE_NOT_NULL(capacity);
    *capacity = Elastos::Core::Math::INT32_MAX_VALUE;
    return NOERROR;
}

ECode CLinkedTransferQueue::AddAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    VALIDATE_NOT_NULL(modified);
    return AbstractQueue::AddAll(collection, modified);
}

ECode CLinkedTransferQueue::Clear()
{
    return AbstractQueue::Clear();
}

ECode CLinkedTransferQueue::ContainsAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    return AbstractQueue::ContainsAll(collection, result);
}

ECode CLinkedTransferQueue::RemoveAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return AbstractQueue::RemoveAll(collection, modified);
}

ECode CLinkedTransferQueue::RetainAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return AbstractQueue::RetainAll(collection, modified);
}

ECode CLinkedTransferQueue::ToArray(
    /* [out, callee] */ ArrayOf<IInterface*>** array)
{
    return AbstractQueue::ToArray(array);
}

ECode CLinkedTransferQueue::ToArray(
    /* [in] */ ArrayOf<IInterface*>* inArray,
    /* [out, callee] */ ArrayOf<IInterface*>** outArray)
{
    return AbstractQueue::ToArray(inArray, outArray);
}

ECode CLinkedTransferQueue::Remove(
    /* [out] */ IInterface** e)
{
    VALIDATE_NOT_NULL(e);
    return AbstractQueue::Remove(e);
}

ECode CLinkedTransferQueue::GetElement(
    /* [out] */ IInterface** e)
{
    VALIDATE_NOT_NULL(e);
    return AbstractQueue::GetElement(e);
}

void CLinkedTransferQueue::WriteObject(
    /* [in] */ IObjectOutputStream* s)
{
    assert(0 && "TODO");
    s->DefaultWriteObject();
    // for (E e : this)
    //     s->WriteObject(e);
    // // Use trailing NULL as sentinel
    // s->WriteObject(NULL);
}

void CLinkedTransferQueue::ReadObject(
    /* [in] */ IObjectInputStream* s)
{
    assert(0 && "TODO");
    s->DefaultReadObject();
    // for (;;) {
    //     @SuppressWarnings("unchecked") E item = (E) s.readObject();
    //     if (item == NULL)
    //         break;
    //     else
    //         Offer(item);
    // }
}

} // namespace Concurrent
} // namespace Utility
} // namespace Elastos
