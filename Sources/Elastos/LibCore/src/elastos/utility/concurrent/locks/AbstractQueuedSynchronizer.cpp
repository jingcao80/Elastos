
#include "Elastos.CoreLibrary.Utility.h"
#include "AbstractQueuedSynchronizer.h"
#include "LockSupport.h"
#include "CSystem.h"
#include <Thread.h>
#include <cutils/atomic.h>
#include <StringUtils.h>

using Elastos::Core::Thread;
using Elastos::Core::CSystem;
using Elastos::Core::StringUtils;
using Elastos::IO::EIID_ISerializable;

namespace Elastos {
namespace Utility {
namespace Concurrent {
namespace Locks {

//==========================================================
//       AbstractQueuedSynchronizer::Node
//==========================================================
const AutoPtr<AbstractQueuedSynchronizer::Node> AbstractQueuedSynchronizer::Node::SHARED =
        new AbstractQueuedSynchronizer::Node();
const AutoPtr<AbstractQueuedSynchronizer::Node> AbstractQueuedSynchronizer::Node::EXCLUSIVE;
const Int32 AbstractQueuedSynchronizer::Node::CANCELLED;
const Int32 AbstractQueuedSynchronizer::Node::SIGNAL;
const Int32 AbstractQueuedSynchronizer::Node::CONDITION;
const Int32 AbstractQueuedSynchronizer::Node::PROPAGATE;

ECode AbstractQueuedSynchronizer::Node::Predecessor(
    /* [out] */ Node** node)
{
    AutoPtr<Node> p = mPrev;
    if (p == NULL) {
        // throw new NullPointerException();
        return E_NULL_POINTER_EXCEPTION;
    }
    else {
        *node = p;
        REFCOUNT_ADD(*node);
        return NOERROR;
    }
}


//==========================================================
//       AbstractQueuedSynchronizer::ConditionObject
//==========================================================
const Int64 AbstractQueuedSynchronizer::ConditionObject::sSerialVersionUID = 1173984872572414699L;

const Int32 AbstractQueuedSynchronizer::ConditionObject::REINTERRUPT =  1;

const Int32 AbstractQueuedSynchronizer::ConditionObject::THROW_IE    = -1;

CAR_INTERFACE_IMPL_3(AbstractQueuedSynchronizer::ConditionObject, Object, IConditionObject, ICondition, ISerializable)

AbstractQueuedSynchronizer::ConditionObject::ConditionObject(
    /* [in] */ AbstractQueuedSynchronizer* host)
{
    mHost = host;
}

ECode AbstractQueuedSynchronizer::ConditionObject::Signal()
{
    if (!mHost->IsHeldExclusively()) {
        // throw new IllegalMonitorStateException();
        return E_ILLEGAL_MONITOR_STATE_EXCEPTION;
    }
    AutoPtr<Node> first = mFirstWaiter;
    if (first != NULL) {
        DoSignal(first);
    }
    return NOERROR;
}

ECode AbstractQueuedSynchronizer::ConditionObject::SignalAll()
{
    if (!mHost->IsHeldExclusively()) {
        // throw new IllegalMonitorStateException();
        return E_ILLEGAL_MONITOR_STATE_EXCEPTION;
    }
    AutoPtr<Node> first = mFirstWaiter;
    if (first != NULL) {
        DoSignalAll(first);
    }
    return NOERROR;
}

ECode AbstractQueuedSynchronizer::ConditionObject::AwaitUninterruptibly()
{
    AutoPtr<Node> node = AddConditionWaiter();
    Int32 savedState = mHost->FullyRelease(node);
    Boolean interrupted = FALSE;
    while (!mHost->IsOnSyncQueue(node)) {
        LockSupport::Park(this->Probe(EIID_IInterface));
        if (Thread::Interrupted()) {
            interrupted = TRUE;
        }
    }
    if (mHost->AcquireQueued(node, savedState) || interrupted) {
        SelfInterrupt();
    }
    return NOERROR;
}

ECode AbstractQueuedSynchronizer::ConditionObject::Await()
{
    if (Thread::Interrupted()) {
        // throw new InterruptedException();
        return E_INTERRUPTED_EXCEPTION;
    }
    AutoPtr<Node> node = AddConditionWaiter();
    Int32 savedState = mHost->FullyRelease(node);
    Int32 interruptMode = 0;
    while (!mHost->IsOnSyncQueue(node)) {
        LockSupport::Park(this->Probe(EIID_IInterface));
        if ((interruptMode = CheckInterruptWhileWaiting(node)) != 0)
            break;
    }
    if (mHost->AcquireQueued(node, savedState) && interruptMode != THROW_IE)
        interruptMode = REINTERRUPT;
    if (node->mNextWaiter != NULL) // clean up if cancelled
        UnlinkCancelledWaiters();
    if (interruptMode != 0)
        ReportInterruptAfterWait(interruptMode);
    return NOERROR;
}

ECode AbstractQueuedSynchronizer::ConditionObject::AwaitNanos(
    /* [in] */ Int64 nanosTimeout,
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value)

    if (Thread::Interrupted()) {
        // throw new InterruptedException();
        return E_INTERRUPTED_EXCEPTION;
    }

    AutoPtr<CSystem> system;
    CSystem::AcquireSingletonByFriend((CSystem**)&system);

    AutoPtr<Node> node = AddConditionWaiter();
    Int32 savedState = mHost->FullyRelease(node);
    Int64 lastTime;
    system->GetNanoTime(&lastTime);
    Int32 interruptMode = 0;
    while (!mHost->IsOnSyncQueue(node)) {
        if (nanosTimeout <= 0L) {
            mHost->TransferAfterCancelledWait(node);
            break;
        }
        LockSupport::ParkNanos(this->Probe(EIID_IInterface), nanosTimeout);
        if ((interruptMode = CheckInterruptWhileWaiting(node)) != 0)
            break;

        Int64 now;
        system->GetNanoTime(&now);
        nanosTimeout -= now - lastTime;
        lastTime = now;
    }
    if (mHost->AcquireQueued(node, savedState) && interruptMode != THROW_IE)
        interruptMode = REINTERRUPT;
    if (node->mNextWaiter != NULL)
        UnlinkCancelledWaiters();
    if (interruptMode != 0)
        ReportInterruptAfterWait(interruptMode);

    Int64 nanos;
    system->GetNanoTime(&nanos);
    *value = nanosTimeout - (nanos - lastTime);
    return NOERROR;
}

ECode AbstractQueuedSynchronizer::ConditionObject::AwaitUntil(
    /* [in] */ IDate* deadline,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)

    if (deadline == NULL) {
        // throw new NullPointerException();
        return E_NULL_POINTER_EXCEPTION;
    }
    Int64 abstime = 0;
    deadline->GetTime(&abstime);
    if (Thread::Interrupted()) {
        // throw new InterruptedException();
        return E_INTERRUPTED_EXCEPTION;
    }

    AutoPtr<CSystem> system;
    CSystem::AcquireSingletonByFriend((CSystem**)&system);

    AutoPtr<Node> node = AddConditionWaiter();
    Int32 savedState = mHost->FullyRelease(node);
    Boolean timedout = FALSE;
    Int32 interruptMode = 0;
    Int64 millis;
    while (!mHost->IsOnSyncQueue(node)) {
        system->GetCurrentTimeMillis(&millis);
        if (millis > abstime) {
            timedout = mHost->TransferAfterCancelledWait(node);
            break;
        }
        LockSupport::ParkUntil(this->Probe(EIID_IInterface), abstime);
        if ((interruptMode = CheckInterruptWhileWaiting(node)) != 0)
            break;
    }
    if (mHost->AcquireQueued(node, savedState) && interruptMode != THROW_IE)
        interruptMode = REINTERRUPT;
    if (node->mNextWaiter != NULL)
        UnlinkCancelledWaiters();
    if (interruptMode != 0)
        ReportInterruptAfterWait(interruptMode);
    *value = !timedout;
    return NOERROR;
}

ECode AbstractQueuedSynchronizer::ConditionObject::Await(
    /* [in] */ Int64 time,
    /* [in] */ ITimeUnit* unit,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)

    if (unit == NULL) {
        // throw new NullPointerException();
        return E_NULL_POINTER_EXCEPTION;
    }
    Int64 nanosTimeout = 0;
    unit->ToNanos(time, &nanosTimeout);
    if (Thread::Interrupted()) {
        // throw new InterruptedException();
        return E_INTERRUPTED_EXCEPTION;
    }

    AutoPtr<CSystem> system;
    CSystem::AcquireSingletonByFriend((CSystem**)&system);

    AutoPtr<Node> node = AddConditionWaiter();
    Int32 savedState = mHost->FullyRelease(node);
    Int64 lastTime;
    system->GetNanoTime(&lastTime);
    Boolean timedout = FALSE;
    Int32 interruptMode = 0;
    while (!mHost->IsOnSyncQueue(node)) {
        if (nanosTimeout <= 0L) {
            timedout = mHost->TransferAfterCancelledWait(node);
            break;
        }
        if (nanosTimeout >= mHost->sSpinForTimeoutThreshold)
            LockSupport::ParkNanos(this->Probe(EIID_IInterface), nanosTimeout);
        if ((interruptMode = CheckInterruptWhileWaiting(node)) != 0)
            break;
        Int64 now;
        system->GetNanoTime(&now);
        nanosTimeout -= now - lastTime;
        lastTime = now;
    }
    if (mHost->AcquireQueued(node, savedState) && interruptMode != THROW_IE)
        interruptMode = REINTERRUPT;
    if (node->mNextWaiter != NULL)
        UnlinkCancelledWaiters();
    if (interruptMode != 0)
        ReportInterruptAfterWait(interruptMode);
    *value = !timedout;
    return NOERROR;
}

Boolean AbstractQueuedSynchronizer::ConditionObject::IsOwnedBy(
    /* [in] */ AbstractQueuedSynchronizer* sync)
{
    return sync == mHost;
}

AutoPtr<AbstractQueuedSynchronizer::Node> AbstractQueuedSynchronizer::ConditionObject::AddConditionWaiter()
{
    AutoPtr<Node> t = mLastWaiter;
    // If mLastWaiter is cancelled, clean out.
    if (t != NULL && t->mWaitStatus != Node::CONDITION) {
        UnlinkCancelledWaiters();
        t = mLastWaiter;
    }
    AutoPtr<Node> node = new Node(Thread::GetCurrentThread(), Node::CONDITION);
    if (t == NULL)
        mFirstWaiter = node;
    else
        t->mNextWaiter = node;
    mLastWaiter = node;
    return node;
}

ECode AbstractQueuedSynchronizer::ConditionObject::DoSignal(
    /* [in] */ Node* first)
{
    do {
        if ( (mFirstWaiter = first->mNextWaiter) == NULL)
            mLastWaiter = NULL;
            first->mNextWaiter = NULL;
    } while (!mHost->TransferForSignal(first) &&
             (first = mFirstWaiter) != NULL);
    return NOERROR;
}

ECode AbstractQueuedSynchronizer::ConditionObject::DoSignalAll(
    /* [in] */ Node* first)
{
    mLastWaiter = mFirstWaiter = NULL;
    do {
        AutoPtr<Node> next = first->mNextWaiter;
        first->mNextWaiter = NULL;
        mHost->TransferForSignal(first);
        first = next;
    } while (first != NULL);
    return NOERROR;
}

ECode AbstractQueuedSynchronizer::ConditionObject::UnlinkCancelledWaiters()
{
    AutoPtr<Node> t = mFirstWaiter;
    AutoPtr<Node> trail;
    while (t != NULL) {
        AutoPtr<Node> next = t->mNextWaiter;
        if (t->mWaitStatus != Node::CONDITION) {
            t->mNextWaiter = NULL;
            if (trail == NULL)
                mFirstWaiter = next;
            else
                trail->mNextWaiter = next;
            if (next == NULL)
                mLastWaiter = trail;
        }
        else
            trail = t;
        t = next;
    }
    return NOERROR;
}

Int32 AbstractQueuedSynchronizer::ConditionObject::CheckInterruptWhileWaiting(
    /* [in] */ Node* node)
{
    return Thread::Interrupted() ? (mHost->TransferAfterCancelledWait(node) ? THROW_IE : REINTERRUPT) : 0;
}

ECode AbstractQueuedSynchronizer::ConditionObject::ReportInterruptAfterWait(
    /* [in] */ Int32 interruptMode)
{
    if (interruptMode == THROW_IE) {
        // throw new InterruptedException();
        return E_INTERRUPTED_EXCEPTION;
    }
    else if (interruptMode == REINTERRUPT) {
        mHost->SelfInterrupt();
    }
    return NOERROR;
}

ECode AbstractQueuedSynchronizer::ConditionObject::HasWaiters(
    /* [out] */ Boolean* result)
{
    if (!mHost->IsHeldExclusively()) {
        // throw new IllegalMonitorStateException();
        return E_ILLEGAL_MONITOR_STATE_EXCEPTION;
    }
    for (AutoPtr<Node> w = mFirstWaiter; w != NULL; w = w->mNextWaiter) {
        if (w->mWaitStatus == Node::CONDITION) {
            *result = TRUE;
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode AbstractQueuedSynchronizer::ConditionObject::GetWaitQueueLength(
    /* [out] */ Int32* number)
{
    if (!mHost->IsHeldExclusively()) {
        // throw new IllegalMonitorStateException();
        return E_ILLEGAL_MONITOR_STATE_EXCEPTION;
    }
    Int32 n = 0;
    for (AutoPtr<Node> w = mFirstWaiter; w != NULL; w = w->mNextWaiter) {
        if (w->mWaitStatus == Node::CONDITION) {
            ++n;
        }
    }
    *number = n;
    return NOERROR;
}

ECode AbstractQueuedSynchronizer::ConditionObject::GetWaitingThreads(
    /* [out] */ ICollection** threads)
{
    if (!mHost->IsHeldExclusively()) {
        // throw new IllegalMonitorStateException();
        return E_ILLEGAL_MONITOR_STATE_EXCEPTION;
    }
    assert(0 && "TODO");
    // ArrayList<Thread> list = new ArrayList<Thread>();
    // AutoPtr<ICollection> list;
    // for (Node w = mFirstWaiter; w != null; w = w.nextWaiter) {
    //     if (w.waitStatus == Node::CONDITION) {
    //         Thread t = w.thread;
    //         if (t != null)
    //             list.add(t);
    //     }
    // }
    // return list;
    return NOERROR;
}


//==========================================================
//       AbstractQueuedSynchronizer
//==========================================================
const Int64 AbstractQueuedSynchronizer::sSpinForTimeoutThreshold;

CAR_INTERFACE_IMPL(AbstractQueuedSynchronizer, AbstractOwnableSynchronizer, IAbstractQueuedSynchronizer)

Boolean AbstractQueuedSynchronizer::CompareAndSetState(
    /* [in] */ Int32 expect,
    /* [in] */ Int32 update)
{
    volatile int32_t* address = (volatile int32_t*)&mState;

    // Note: android_atomic_release_cas() returns 0 on success, not failure.
    int ret = android_atomic_release_cas(expect, update, address);

    return (ret == 0);
}

AutoPtr<AbstractQueuedSynchronizer::Node> AbstractQueuedSynchronizer::Enq(
    /* [in] */ Node* node)
{
    for (;;) {
        AutoPtr<Node> t = mTail;
        if (t == NULL) { // Must initialize
            AutoPtr<Node> n = new Node();
            if (CompareAndSetHead(n)) {
                mTail = mHead;
            }
        }
        else {
            node->mPrev = t;
            if (CompareAndSetTail(t, node)) {
                t->mNext = node;
                return t;
            }
        }
    }
}

AutoPtr<AbstractQueuedSynchronizer::Node> AbstractQueuedSynchronizer::AddWaiter(
    /* [in] */ Node* mode)
{
    AutoPtr<Node> node = new Node(Thread::GetCurrentThread(), mode);
    // Try the fast path of enq; backup to full enq on failure
    AutoPtr<Node> pred = mTail;
    if (pred != NULL) {
        node->mPrev = pred;
        if (CompareAndSetTail(pred, node)) {
            pred->mNext = node;
            return node;
        }
    }
    Enq(node);
    return node;
}

void AbstractQueuedSynchronizer::SetHead(
    /* [in] */ Node* node)
{
    mHead = node;
    node->mThread = NULL;
    node->mPrev = NULL;
}

void AbstractQueuedSynchronizer::UnparkSuccessor(
    /* [in] */ Node* node)
{
    /*
     * If status is negative (i.e., possibly needing signal) try
     * to clear in anticipation of signalling.  It is OK if this
     * fails or if status is changed by waiting thread.
     */
    Int32 ws = node->mWaitStatus;
    if (ws < 0) {
        CompareAndSetWaitStatus(node, ws, 0);
    }

    /*
     * Thread to unpark is held in successor, which is normally
     * just the next node.  But if cancelled or apparently null,
     * traverse backwards from mTail to find the actual
     * non-cancelled successor.
     */
    AutoPtr<Node> s = node->mNext;
    if (s == NULL || s->mWaitStatus > 0) {
        s = NULL;
        for (AutoPtr<Node> t = mTail; t != NULL && t.Get() != node; t = t->mPrev) {
            if (t->mWaitStatus <= 0) {
                s = t;
            }
        }
    }
    if (s != NULL) {
        LockSupport::Unpark(s->mThread);
    }
}

void AbstractQueuedSynchronizer::DoReleaseShared()
{
    /*
     * Ensure that a release propagates, even if there are other
     * in-progress acquires/releases.  This proceeds in the usual
     * way of trying to unparkSuccessor of mHead if it needs
     * signal. But if it does not, status is set to PROPAGATE to
     * ensure that upon release, propagation continues.
     * Additionally, we must loop in case a new node is added
     * while we are doing this. Also, unlike other uses of
     * unparkSuccessor, we need to know if CAS to reset status
     * fails, if so rechecking.
     */
    for (;;) {
        AutoPtr<Node> h = mHead;
        if (h != NULL && !Object::Equals(h->Probe(EIID_IInterface), mTail->Probe(EIID_IInterface))) {
            Int32 ws = h->mWaitStatus;
            if (ws == Node::SIGNAL) {
                if (!CompareAndSetWaitStatus(h, Node::SIGNAL, 0)) {
                    continue;            // loop to recheck cases
                }
                UnparkSuccessor(h);
            }
            else if (ws == 0 &&
                     !CompareAndSetWaitStatus(h, 0, Node::PROPAGATE)) {
                continue;                // loop on failed CAS
            }
        }
        if (Object::Equals(h->Probe(EIID_IInterface), mHead->Probe(EIID_IInterface))) {                   // loop if mHead changed
            break;
        }
    }
}

void AbstractQueuedSynchronizer::SetHeadAndPropagate(
    /* [in] */ Node* node,
    /* [in] */ Int32 propagate)
{
    AutoPtr<Node> h = mHead; // Record old mHead for check below
    SetHead(node);
    /*
     * Try to signal next queued node if:
     *   Propagation was indicated by caller,
     *     or was recorded (as h.waitStatus) by a previous operation
     *     (note: this uses sign-check of waitStatus because
     *      PROPAGATE status may transition to SIGNAL.)
     * and
     *   The next node is waiting in shared mode,
     *     or we don't know, because it appears null
     *
     * The conservatism in both of these checks may cause
     * unnecessary wake-ups, but only when there are multiple
     * racing acquires/releases, so most need signals now or soon
     * anyway.
     */
    if (propagate > 0 || h == NULL || h->mWaitStatus < 0) {
        AutoPtr<Node> s = node->mNext;
        if (s == NULL || s->IsShared()) {
            DoReleaseShared();
        }
    }
}

void AbstractQueuedSynchronizer::CancelAcquire(
    /* [in] */ Node* node)
{
    // Ignore if node doesn't exist
    if (node == NULL) return;

    node->mThread = NULL;

    // Skip cancelled predecessors
    AutoPtr<Node> pred = node->mPrev;
    while (pred->mWaitStatus > 0) {
        node->mPrev = pred = pred->mPrev;
    }

    // predNext is the apparent node to unsplice. CASes below will
    // fail if not, in which case, we lost race vs another cancel
    // or signal, so no further action is necessary.
    AutoPtr<Node> predNext = pred->mNext;

    // Can use unconditional write instead of CAS here.
    // After this atomic step, other Nodes can skip past us.
    // Before, we are free of interference from other threads.
    node->mWaitStatus = Node::CANCELLED;

    // If we are the mTail, remove ourselves.
    if (node == mTail.Get() && CompareAndSetTail(node, pred)) {
        CompareAndSetNext(pred, predNext, NULL);
    }
    else {
        // If successor needs signal, try to set pred's next-link
        // so it will get one. Otherwise wake it up to propagate.
        Int32 ws;
        if (pred != mHead &&
            ((ws = pred->mWaitStatus) == Node::SIGNAL ||
             (ws <= 0 && CompareAndSetWaitStatus(pred, ws, Node::SIGNAL))) &&
            pred->mThread != NULL) {
            AutoPtr<Node> next = node->mNext;
            if (next != NULL && next->mWaitStatus <= 0)
                CompareAndSetNext(pred, predNext, next);
        }
        else {
            UnparkSuccessor(node);
        }

        node->mNext = node; // help GC
    }
}

Boolean AbstractQueuedSynchronizer::ShouldParkAfterFailedAcquire(
    /* [in] */ Node* pred,
    /* [in] */ Node* node)
{
    Int32 ws = pred->mWaitStatus;
    if (ws == Node::SIGNAL) {
        /*
         * This node has already set status asking a release
         * to signal it, so it can safely park.
         */
        return TRUE;
    }
    if (ws > 0) {
        /*
         * Predecessor was cancelled. Skip over predecessors and
         * indicate retry.
         */
        do {
            node->mPrev = pred = pred->mPrev;
        } while (pred->mWaitStatus > 0);
        pred->mNext = node;
    }
    else {
        /*
         * waitStatus must be 0 or PROPAGATE.  Indicate that we
         * need a signal, but don't park yet.  Caller will need to
         * retry to make sure it cannot acquire before parking.
         */
        CompareAndSetWaitStatus(pred, ws, Node::SIGNAL);
    }
    return FALSE;
}

void AbstractQueuedSynchronizer::SelfInterrupt()
{
    Thread::GetCurrentThread()->Interrupt();
}

Boolean AbstractQueuedSynchronizer::ParkAndCheckInterrupt()
{
    // LockSupport.park(this);
    return Thread::Interrupted();
}

Boolean AbstractQueuedSynchronizer::AcquireQueued(
    /* [in] */ Node* node,
    /* [in] */ Int32 arg)
{
    Boolean failed = TRUE;
    Boolean interrupted = FALSE;
    for (;;) {
        AutoPtr<Node> p;
        ECode ec = node->Predecessor((Node**)&p);
        if (FAILED(ec)) {
            if (failed) {
                CancelAcquire(node);
            }
            return interrupted;
        }
        if (p == mHead && TryAcquire(arg)) {
            SetHead(node);
            p->mNext = NULL;
            failed = FALSE;
            return interrupted;
        }
        if (ShouldParkAfterFailedAcquire(p, node) &&
            ParkAndCheckInterrupt()) {
            interrupted = TRUE;
        }
    }
}

ECode AbstractQueuedSynchronizer::DoAcquireSharedInterruptibly(
    /* [in] */ Int32 arg)
{
    AutoPtr<Node> node = AddWaiter(Node::SHARED);
    Boolean failed = TRUE;
    // try {
    for (;;) {
        AutoPtr<Node> p;
        ECode ec = node->Predecessor((Node**)&p);
        if (FAILED(ec)) {
            if (failed) {
                CancelAcquire(node);
            }
            return ec;
        }
        if (p == mHead) {
            Int32 r = TryAcquireShared(arg);
            if (r >= 0) {
                SetHeadAndPropagate(node, r);
                p->mNext = NULL; // help GC
                failed = FALSE;
                return NOERROR;
            }
        }
        if (ShouldParkAfterFailedAcquire(p, node) &&
            ParkAndCheckInterrupt()) {
            if (failed) {
                CancelAcquire(node);
            }
            // throw new InterruptedException();
            return E_INTERRUPTED_EXCEPTION;
        }
    }
    // } finally {
    //     if (failed)
    //         cancelAcquire(node);
    // }
}

Boolean AbstractQueuedSynchronizer::TryAcquire(
    /* [in] */ Int32 arg)
{
    // throw new UnsupportedOperationException();
    assert(0);
    return FALSE;
}

Boolean AbstractQueuedSynchronizer::TryRelease(
    /* [in] */ Int32 arg)
{
    // throw new UnsupportedOperationException();
    assert(0);
    return FALSE;
}

Int32 AbstractQueuedSynchronizer::TryAcquireShared(
    /* [in] */ Int32 arg)
{
    // throw new UnsupportedOperationException();
    assert(0);
    return -1;
}

Boolean AbstractQueuedSynchronizer::TryReleaseShared(
    /* [in] */ Int32 arg)
{
    // throw new UnsupportedOperationException();
    assert(0);
    return FALSE;
}

Boolean AbstractQueuedSynchronizer::IsHeldExclusively()
{
    // throw new UnsupportedOperationException();
    assert(0);
    return FALSE;
}

ECode AbstractQueuedSynchronizer::AcquireIt(
    /* [in] */ Int32 arg)
{
    if (!TryAcquire(arg)) {
        AutoPtr<Node> node = AddWaiter(Node::EXCLUSIVE);
        if (AcquireQueued(node, arg)) {
            SelfInterrupt();
        }
    }
    return NOERROR;
}

ECode AbstractQueuedSynchronizer::ReleaseIt(
    /* [in] */ Int32 arg,
    /* [out] */ Boolean* result)
{
    assert(result != NULL);
    if (TryRelease(arg)) {
        AutoPtr<Node> h = mHead;
        if (h != NULL && h->mWaitStatus != 0) {
            UnparkSuccessor(h);
        }
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode AbstractQueuedSynchronizer::AcquireSharedInterruptibly(
    /* [in] */ Int32 arg)
{
    if (Thread::Interrupted()) {
        // throw new InterruptedException();
        return E_INTERRUPTED_EXCEPTION;
    }
    if (TryAcquireShared(arg) < 0) {
        return DoAcquireSharedInterruptibly(arg);
    }
    return NOERROR;
}

ECode AbstractQueuedSynchronizer::ReleaseShared(
    /* [in] */ Int32 arg,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    if (TryReleaseShared(arg)) {
        DoReleaseShared();
        *value = TRUE;
        return NOERROR;
    }
    *value = FALSE;
    return NOERROR;
}

Boolean AbstractQueuedSynchronizer::CompareAndSetHead(
    /* [in] */ Node* update)
{
    int32_t* address = (int32_t*)&mHead;

    // Note: android_atomic_cmpxchg() returns 0 on success, not failure.
    int result = android_atomic_release_cas((int32_t)NULL,
            (int32_t)update, address);
    // dvmWriteBarrierField(obj, address);
    if (result == 0) REFCOUNT_ADD(mHead);
    return result == 0;
}

Boolean AbstractQueuedSynchronizer::CompareAndSetTail(
    /* [in] */ Node* expect,
    /* [in] */ Node* update)
{
    int32_t* address = (int32_t*)&mTail;

    // Note: android_atomic_cmpxchg() returns 0 on success, not failure.
    int result = android_atomic_release_cas((int32_t)expect,
            (int32_t)update, address);
    // dvmWriteBarrierField(obj, address);
    if (result == 0) {
        if (expect != NULL) expect->Release();
        REFCOUNT_ADD(mTail);
    }
    return result == 0;
}

Boolean AbstractQueuedSynchronizer::CompareAndSetWaitStatus(
    /* [in] */ Node* node,
    /* [in] */ Int32 expect,
    /* [in] */ Int32 update)
{
    int32_t* address = (int32_t*)&node->mWaitStatus;

    // Note: android_atomic_cmpxchg() returns 0 on success, not failure.
    int result = android_atomic_release_cas(expect, update, address);

    return result == 0;
}

Boolean AbstractQueuedSynchronizer::CompareAndSetNext(
    /* [in] */ Node* node,
    /* [in] */ Node* expect,
    /* [in] */ Node* update)
{
    int32_t* address = (int32_t*)&node->mNext;

    // Note: android_atomic_cmpxchg() returns 0 on success, not failure.
    int result = android_atomic_release_cas((int32_t)expect,
            (int32_t)update, address);
    // dvmWriteBarrierField(obj, address);
    if (result == 0) {
        if (expect != NULL) expect->Release();
        REFCOUNT_ADD(node->mNext);
    }
    return result == 0;
}

ECode AbstractQueuedSynchronizer::AcquireShared(
    /* [in] */ Int32 arg)
{
    if (TryAcquireShared(arg) < 0)
        return DoAcquireShared(arg);

    return NOERROR;
}

ECode AbstractQueuedSynchronizer::DoAcquireInterruptibly(
    /* [in] */ Int64 arg)
{
    AutoPtr<Node> node = AddWaiter(Node::EXCLUSIVE);
    Boolean failed = TRUE;
    // try {
    for (;;) {
        AutoPtr<Node> p;
        node->Predecessor((Node**)&p);
        if (p == mHead && TryAcquire(arg)) {
            SetHead(node);
            p->mNext = NULL; // help GC
            failed = FALSE;
            return NOERROR;
        }
        if (ShouldParkAfterFailedAcquire(p, node) &&
            ParkAndCheckInterrupt()) {
            // throw new InterruptedException();
            return NOERROR;
        }
    }
    // } finally {
    if (failed)
        CancelAcquire(node);
    // }
    return NOERROR;
}

Boolean AbstractQueuedSynchronizer::DoAcquireNanos(
    /* [in] */ Int64 arg,
    /* [in] */ Int64 nanosTimeout)
{
    AutoPtr<CSystem> system;
    CSystem::AcquireSingletonByFriend((CSystem**)&system);

    Int64 lastTime;
    system->GetNanoTime(&lastTime);
    AutoPtr<Node> node = AddWaiter(Node::EXCLUSIVE);
    Boolean failed = TRUE;
    // try {
    for (;;) {
        AutoPtr<Node> p;
        node->Predecessor((Node**)&p);
        if (p == mHead && TryAcquire(arg)) {
            SetHead(node);
            p->mNext = NULL; // help GC
            failed = FALSE;
            return TRUE;
        }
        if (nanosTimeout <= 0) {
            return FALSE;
        }
        if (ShouldParkAfterFailedAcquire(p, node) &&
            nanosTimeout > sSpinForTimeoutThreshold) {
            assert(0 && "TODO");
            // LockSupport::ParkNanos(this->Probe(EIID_IInterface), nanosTimeout);
        }
        Int64 now;
        system->GetNanoTime(&now);
        nanosTimeout -= now - lastTime;
        lastTime = now;
        if (Thread::Interrupted()) {
            // throw new InterruptedException();
        }
    }
    // } finally {
    if (failed) {
        CancelAcquire(node);
    }
    // }
    return FALSE;
}

ECode AbstractQueuedSynchronizer::DoAcquireShared(
    /* [in] */ Int64 arg)
{
    AutoPtr<Node> node = AddWaiter(Node::EXCLUSIVE);
    Boolean failed = TRUE;
    // try {
    Boolean interrupted = FALSE;
    for (;;) {
        AutoPtr<Node> p;
        node->Predecessor((Node**)&p);
        if (p == mHead) {
            Int64 r = TryAcquireShared(arg);
            if (r >= 0) {
                SetHeadAndPropagate(node, r);
                p->mNext = NULL; // help GC
                if (interrupted)
                    SelfInterrupt();
                failed = FALSE;
                return NOERROR;
            }
        }
        if (ShouldParkAfterFailedAcquire(p, node) &&
            ParkAndCheckInterrupt())
            interrupted = TRUE;
    }
    // } finally {
    if (failed)
        CancelAcquire(node);
    // }
    return NOERROR;
}

ECode AbstractQueuedSynchronizer::TryAcquireSharedNanos(
    /* [in] */ Int32 arg,
    /* [in] */ Int64 nanosTimeout,
    /* [out] */ Boolean* result)
{
    if (Thread::Interrupted()) {
        // throw new InterruptedException();
        return E_INTERRUPTED_EXCEPTION;
    }
    *result = TryAcquireShared(arg) >= 0 || DoAcquireSharedNanos(arg, nanosTimeout);
    return NOERROR;
}

ECode AbstractQueuedSynchronizer::Acquire(
    /* [in] */ Int32 arg)
{
    if (!TryAcquire(arg) &&
        AcquireQueued(AddWaiter(Node::EXCLUSIVE), arg))
        SelfInterrupt();
    return NOERROR;
}

ECode AbstractQueuedSynchronizer::AcquireInterruptibly(
    /* [in] */ Int32 arg)
{
    if (Thread::Interrupted()) {
        // throw new InterruptedException();
        return E_INTERRUPTED_EXCEPTION;
    }
    if (!TryAcquire(arg)) {
        DoAcquireInterruptibly(arg);
    }
    return NOERROR;
}

ECode AbstractQueuedSynchronizer::TryAcquireNanos(
    /* [in] */ Int32 arg,
    /* [in] */ Int64 nanosTimeout,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    if (Thread::Interrupted()) {
        *value = FALSE;
        return E_INTERRUPTED_EXCEPTION;
    }
    *value = TryAcquire(arg) || DoAcquireNanos(arg, nanosTimeout);
    return NOERROR;
}

ECode AbstractQueuedSynchronizer::HasQueuedThreads(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    *value = mHead != mTail;
    return NOERROR;
}

ECode AbstractQueuedSynchronizer::HasContended(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    *value = mHead != NULL;
    return NOERROR;
}

ECode AbstractQueuedSynchronizer::GetFirstQueuedThread(
    /* [out] */ IThread** outthread)
{
    VALIDATE_NOT_NULL(outthread);
    // handle only fast path, else relay
    *outthread = (mHead == mTail) ? NULL : FullGetFirstQueuedThread();
    REFCOUNT_ADD(*outthread);
    return NOERROR;
}

ECode AbstractQueuedSynchronizer::IsQueued(
    /* [in] */ IThread* thread,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)
    if (thread == NULL) {
        *value = FALSE;
        return E_NULL_POINTER_EXCEPTION;
    }
    for (AutoPtr<Node> p = mTail; p != NULL; p = p->mPrev) {
        if (p->mThread.Get() == thread) {
            *value = TRUE;
            return NOERROR;
        }
    }
    *value = FALSE;
    return NOERROR;
}

ECode AbstractQueuedSynchronizer::ApparentlyFirstQueuedIsExclusive(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    AutoPtr<Node> h;
    AutoPtr<Node> s;
    *value = (h = mHead) != NULL &&
           (s = h->mNext) != NULL &&
           !s->IsShared()         &&
           s->mThread != NULL;
    return NOERROR;
}

ECode AbstractQueuedSynchronizer::HasQueuedPredecessors(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    // The correctness of this depends on mHead being initialized
    // before mTail and on mHead.next being accurate if the current
    // thread is first in queue.
    AutoPtr<Node> t = mTail; // Read fields in reverse initialization order
    AutoPtr<Node> h = mHead;
    AutoPtr<Node> s;
    *value = h != t && ((s = h->mNext) == NULL || s->mThread != Thread::GetCurrentThread());
    return NOERROR;
}

ECode AbstractQueuedSynchronizer::GetQueueLength(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    Int32 n = 0;
    for (AutoPtr<Node> p = mTail; p != NULL; p = p->mPrev) {
        if (p->mThread != NULL)
            ++n;
    }
    *value = n;
    return NOERROR;
}

ECode AbstractQueuedSynchronizer::GetQueuedThreads(
    /* [out] */ ICollection** outlect)
{
    assert(0 && "TODO");
    VALIDATE_NOT_NULL(outlect);
    // ArrayList<Thread> list = new ArrayList<Thread>();
    // for (Node p = mTail; p != null; p = p.prev) {
    //     Thread t = p.thread;
    //     if (t != null)
    //         list.add(t);
    // }
    // return list;
    return NOERROR;
}

ECode AbstractQueuedSynchronizer::GetExclusiveQueuedThreads(
    /* [out] */ ICollection** outlect)
{
    assert(0 && "TODO");
    VALIDATE_NOT_NULL(outlect);
    // ArrayList<Thread> list = new ArrayList<Thread>();
    // for (Node p = mTail; p != null; p = p.prev) {
    //     if (!p.isShared()) {
    //         Thread t = p.thread;
    //         if (t != null)
    //             list.add(t);
    //     }
    // }
    // return list;
    return NOERROR;
}

ECode AbstractQueuedSynchronizer::GetSharedQueuedThreads(
    /* [out] */ ICollection** outlect)
{
    assert(0 && "TODO");
    VALIDATE_NOT_NULL(outlect);
    // ArrayList<Thread> list = new ArrayList<Thread>();
    // for (Node p = mTail; p != null; p = p.prev) {
    //     if (p.isShared()) {
    //         Thread t = p.thread;
    //         if (t != null)
    //             list.add(t);
    //     }
    // }
    // return list;
    return NOERROR;
}

ECode AbstractQueuedSynchronizer::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    Int32 s = GetState();
    Boolean b = FALSE;
    String q  = (HasQueuedThreads(&b), b) ? String("non") : String("");
    // return super.toString() + "[State = " + s + ", " + q + "empty queue]";
    *str = String("[State = ") +
        StringUtils::ToString(s) +
        String(", ") +
        q +
        String("empty queue]");

    return NOERROR;
}

Boolean AbstractQueuedSynchronizer::IsOnSyncQueue(
    /* [in] */ Node* node)
{
    if (node->mWaitStatus == Node::CONDITION || node->mPrev == NULL)
        return FALSE;
    if (node->mNext != NULL) // If has successor, it must be on queue
        return TRUE;
    /*
     * node.prev can be non-null, but not yet on queue because
     * the CAS to place it on queue can fail. So we have to
     * traverse from mTail to make sure it actually made it.  It
     * will always be near the mTail in calls to this method, and
     * unless the CAS failed (which is unlikely), it will be
     * there, so we hardly ever traverse much.
     */
    return FindNodeFromTail(node);
}

Boolean AbstractQueuedSynchronizer::TransferForSignal(
    /* [in] */ Node* node)
{
    /*
     * If cannot change waitStatus, the node has been cancelled.
     */
    if (!CompareAndSetWaitStatus(node, Node::CONDITION, 0))
        return FALSE;

    /*
     * Splice onto queue and try to set waitStatus of predecessor to
     * indicate that thread is (probably) waiting. If cancelled or
     * attempt to set waitStatus fails, wake up to resync (in which
     * case the waitStatus can be transiently and harmlessly wrong).
     */
    AutoPtr<Node> p = Enq(node);
    Int32 ws = p->mWaitStatus;
    if (ws > 0 || !CompareAndSetWaitStatus(p, ws, Node::SIGNAL))
        LockSupport::Unpark(node->mThread);
    return TRUE;
}

Boolean AbstractQueuedSynchronizer::TransferAfterCancelledWait(
    /* [in] */ Node* node)
{
    if (CompareAndSetWaitStatus(node, Node::CONDITION, 0)) {
        Enq(node);
        return TRUE;
    }
    /*
     * If we lost out to a signal(), then we can't proceed
     * until it finishes its enq().  Cancelling during an
     * incomplete transfer is both rare and transient, so just
     * spin.
     */
    while (!IsOnSyncQueue(node))
        Thread::Yield();
    return FALSE;
}

Int32 AbstractQueuedSynchronizer::FullyRelease(
    /* [in] */ Node* node)
{
    Boolean failed = TRUE;
    // try {
    Int32 savedState = GetState();
    Boolean isflag = FALSE;
    if (ReleaseIt(savedState, &isflag), isflag) {
        failed = FALSE;
        return savedState;
    }
    else {
        // throw new IllegalMonitorStateException();
        return -1;
    }
    // } finally {
    if (failed)
        node->mWaitStatus = Node::CANCELLED;
    // }
    return -1;
}

ECode AbstractQueuedSynchronizer::Owns(
    /* [in] */ ICondition* condition,
    /* [out] */ Boolean* result)
{
    if (condition == NULL) {
        // throw new NullPointerException();
        return E_NULL_POINTER_EXCEPTION;
    }
    *result = ((ConditionObject*)condition)->IsOwnedBy(this);
    return NOERROR;
}

ECode AbstractQueuedSynchronizer::HasWaiters(
    /* [in] */ ICondition* condition,
    /* [out] */ Boolean* result)
{
    Boolean isOwned;
    FAIL_RETURN(Owns(condition, &isOwned));
    if (!isOwned) {
        // throw new IllegalArgumentException("Not owner");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return ((ConditionObject*)condition)->HasWaiters(result);
}

ECode AbstractQueuedSynchronizer::GetWaitQueueLength(
    /* [in] */ ICondition* condition,
    /* [out] */ Int32* number)
{
    Boolean isOwned;
    FAIL_RETURN(Owns(condition, &isOwned));
    if (!isOwned) {
        // throw new IllegalArgumentException("Not owner");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return ((ConditionObject*)condition)->GetWaitQueueLength(number);
}

ECode AbstractQueuedSynchronizer::GetWaitingThreads(
    /* [in] */ ICondition* condition,
    /* [out] */ ICollection** threads)
{
    Boolean isOwned;
    FAIL_RETURN(Owns(condition, &isOwned));
    if (!isOwned) {
        // throw new IllegalArgumentException("Not owner");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return ((ConditionObject*)condition)->GetWaitingThreads(threads);
}

AutoPtr<IThread> AbstractQueuedSynchronizer::FullGetFirstQueuedThread()
{
    /*
     * The first node is normally mHead.next. Try to get its
     * thread field, ensuring consistent reads: If thread
     * field is nulled out or s.prev is no longer mHead, then
     * some other thread(s) concurrently performed setHead in
     * between some of our reads. We try this twice before
     * resorting to traversal.
     */
    AutoPtr<Node> h;
    AutoPtr<Node> s;
    AutoPtr<IThread> st;
    if (((h = mHead) != NULL && (s = h->mNext) != NULL &&
         s->mPrev == mHead && (st = s->mThread) != NULL) ||
        ((h = mHead) != NULL && (s = h->mNext) != NULL &&
         s->mPrev == mHead && (st = s->mThread) != NULL))
        return st;

    /*
     * Head's next field might not have been set yet, or may have
     * been unset after setHead. So we must check to see if mTail
     * is actually first node. If not, we continue on, safely
     * traversing from mTail back to mHead to find first,
     * guaranteeing termination.
     */

    AutoPtr<Node> t = mTail;
    AutoPtr<IThread> firstThread = NULL;
    while (t != NULL && !Object::Equals(t->Probe(EIID_IInterface), mHead->Probe(EIID_IInterface))) {
        AutoPtr<IThread> tt = t->mThread;
        if (tt != NULL)
            firstThread = tt;
        t = t->mPrev;
    }
    return firstThread;
}

Boolean AbstractQueuedSynchronizer::FindNodeFromTail(
    /* [in] */ Node* node)
{
    AutoPtr<Node> t = mTail;
    for (;;) {
        if (Object::Equals(t->Probe(EIID_IInterface), node->Probe(EIID_IInterface)))
            return TRUE;
        if (t == NULL)
            return FALSE;
        t = t->mPrev;
    }
    return FALSE;
}

Boolean AbstractQueuedSynchronizer::DoAcquireSharedNanos(
    /* [in] */ Int32 arg,
    /* [in] */ Int64 nanosTimeout)
{
    AutoPtr<CSystem> system;
    CSystem::AcquireSingletonByFriend((CSystem**)&system);

    Int64 lastTime;
    system->GetNanoTime(&lastTime);
    AutoPtr<Node> node = AddWaiter(Node::SHARED);
    Boolean failed = TRUE;
    // try {
    for (;;) {
        AutoPtr<Node> p;
        node->Predecessor((Node**)&p);
        if (p == mHead) {
            Int32 r = TryAcquireShared(arg);
            if (r >= 0) {
                SetHeadAndPropagate(node, r);
                p->mNext = NULL; // help GC
                failed = FALSE;
                return TRUE;
            }
        }
        if (nanosTimeout <= 0)
            return FALSE;
        if (ShouldParkAfterFailedAcquire(p, node) &&
            nanosTimeout > sSpinForTimeoutThreshold) {
            // LockSupport.parkNanos(this, nanosTimeout);
            assert(0 && "TODO");
        }
        Int64 now;
        system->GetNanoTime(&now);
        nanosTimeout -= now - lastTime;
        lastTime = now;
        if (Thread::Interrupted()) {
            // throw new InterruptedException();
            return FALSE;
        }
    }
    // } finally {
    if (failed)
        CancelAcquire(node);
    // }
    return FALSE;
}

} // namespace Locks
} // namespace Concurrent
} // namespace Utility
} // namespace Elastos
