
#include "CPhaser.h"
#include "LockSupport.h"
#include "StringBuilder.h"
#include "Math.h"
#include "CSystem.h"
#include "Thread.h"
#include "CAtomicReference.h"
#include "CForkJoinPool.h"

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Core::Thread;
using Elastos::Core::ISystem;
using Elastos::Core::Math;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Concurrent::CForkJoinPool;
using Elastos::Utility::Concurrent::Locks::LockSupport;
using Elastos::Utility::Concurrent::Atomic::CAtomicReference;

namespace Elastos {
namespace Utility {
namespace Concurrent {

//====================================================================
// CPhaser::
//====================================================================

Int32  CPhaser::MAX_PARTIES     = 0xffff;
Int32  CPhaser::MAX_PHASE       = Elastos::Core::Math::INT32_MAX_VALUE;
Int32  CPhaser::PARTIES_SHIFT   = 16;
Int32  CPhaser::PHASE_SHIFT     = 32;
Int32  CPhaser::UNARRIVED_MASK  = 0xffff;      // to mask ints
Int64 CPhaser::PARTIES_MASK    = 0xffff0000LL; // to mask longs
Int64 CPhaser::COUNTS_MASK     = 0xffffffffLL;
Int64 CPhaser::TERMINATION_BIT = 1LL << 63;

Int32  CPhaser::ONE_ARRIVAL     = 1;
Int32  CPhaser::ONE_PARTY       = 1 << PARTIES_SHIFT;
Int32  CPhaser::ONE_DEREGISTER  = ONE_ARRIVAL|ONE_PARTY;
Int32  CPhaser::EMPTY           = 1;

CAR_INTERFACE_IMPL(CPhaser, Object, IPhaser)

CAR_OBJECT_IMPL(CPhaser)

Int32 CPhaser::UnarrivedOf(
    /* [in] */ Int64 s)
{
    Int32 counts = (Int32)s;
    return (counts == EMPTY) ? 0 : (counts & UNARRIVED_MASK);
}

Int32 CPhaser::PartiesOf(
    /* [in] */ Int64 s)
{
    return (Int32)s >> PARTIES_SHIFT;
}

Int32 CPhaser::PhaseOf(
    /* [in] */ Int64 s)
{
    return (Int32)(s >> PHASE_SHIFT);
}

Int32 CPhaser::ArrivedOf(
    /* [in] */ Int64 s)
{
    Int32 counts = (Int32)s;
    return (counts == EMPTY) ? 0 :
        (counts >> PARTIES_SHIFT) - (counts & UNARRIVED_MASK);
}

AutoPtr<IAtomicReference> CPhaser::QueueFor(
    /* [in] */ Int32 phase)
{
    return ((phase & 1) == 0) ? mEvenQ : mOddQ;
}

String CPhaser::BadArrive(
    /* [in] */ Int64 s)
{
    StringBuilder sb("Attempted arrival of unregistered party for ");
    sb += StateToString(s);
    return sb.ToString();
}

String CPhaser::BadRegister(
    /* [in] */ Int64 s)
{
    StringBuilder sb("Attempt to register more than ");
    sb += MAX_PARTIES;
    sb += " parties for ";
    sb += StateToString(s);
    return sb.ToString();
}

Int32 CPhaser::DoArrive(
    /* [in] */ Int32 adjust)
{
    AutoPtr<IPhaser> root = mRoot;
    for (;;) {
        Int64 s = Object::Equals(root, TO_IINTERFACE(this)) ? mState : ReconcileState();
        Int32 phase = (Int32)(s >> PHASE_SHIFT);
        if (phase < 0)
            return phase;
        Int32 counts = (Int32)s;
        Int32 unarrived = (counts == EMPTY) ? 0 : (counts & UNARRIVED_MASK);
        if (unarrived <= 0)
            assert(0);
        //    throw new IllegalStateException(badArrive(s));
        // if (UNSAFE.compareAndSwapLong(this, stateOffset, s, s-=adjust)) {
        //     if (unarrived == 1) {
        //         Int64 n = s & PARTIES_MASK;  // base of next state
        //         Int32 nextUnarrived = (Int32)n >>> PARTIES_SHIFT;
        //         if (root == this) {
        //             if (onAdvance(phase, nextUnarrived))
        //                 n |= TERMINATION_BIT;
        //             else if (nextUnarrived == 0)
        //                 n |= EMPTY;
        //             else
        //                 n |= nextUnarrived;
        //             Int32 nextPhase = (phase + 1) & MAX_PHASE;
        //             n |= (Int64)nextPhase << PHASE_SHIFT;
        //             UNSAFE.compareAndSwapLong(this, stateOffset, s, n);
        //             releaseWaiters(phase);
        //         }
        //         else if (nextUnarrived == 0) { // propagate deregistration
        //             phase = parent.doArrive(ONE_DEREGISTER);
        //             UNSAFE.compareAndSwapLong(this, stateOffset,
        //                                       s, s | EMPTY);
        //         }
        //         else
        //             phase = parent.doArrive(ONE_ARRIVAL);
        //     }
        //     return phase;
        // }
    }
}

Int32 CPhaser::DoRegister(
    /* [in] */ Int32 registrations)
{
    // adjustment to state
    Int64 adjust = ((Int64)registrations << PARTIES_SHIFT) | registrations;
    AutoPtr<IPhaser> parent = mParent;
    Int32 phase;
    for (;;) {
        Int64 s = (parent == NULL) ? mState : ReconcileState();
        Int32 counts = (Int32)s;
        Int32 parties = counts >> PARTIES_SHIFT;
        Int32 unarrived = counts & UNARRIVED_MASK;
        // TODO:
        if (registrations > MAX_PARTIES - parties)
            assert(0);
        //     throw new IllegalStateException(badRegister(s));
        // phase = (Int32)(s >> PHASE_SHIFT);
        // if (phase < 0)
        //     break;
        // if (counts != EMPTY) {                  // not 1st registration
        //     if (parent == NULL || ReconcileState() == s) {
        //         if (unarrived == 0)             // wait out advance
        //             root.internalAwaitAdvance(phase, NULL);
        //         else if (UNSAFE.compareAndSwapLong(this, stateOffset,
        //                                            s, s + adjust))
        //             break;
        //     }
        // }
        // else if (parent == NULL) {              // 1st root registration
        //     Int64 next = ((Int64)phase << PHASE_SHIFT) | adjust;
        //     if (UNSAFE.compareAndSwapLong(this, stateOffset, s, next))
        //         break;
        // }
        // else {
        //     {    AutoLock syncLock(this);               // 1st sub registration
        //         if (state == s) {               // recheck under lock
        //             phase = parent.doRegister(1);
        //             if (phase < 0)
        //                 break;
        //             // finish registration whenever parent registration
        //             // succeeded, even when racing with termination,
        //             // since these are part of the same "transaction".
        //             while (!UNSAFE.compareAndSwapLong
        //                    (this, stateOffset, s,
        //                     ((Int64)phase << PHASE_SHIFT) | adjust)) {
        //                 s = state;
        //                 phase = (Int32)(root.state >>> PHASE_SHIFT);
        //                 // assert (int)s == EMPTY;
        //             }
        //             break;
        //         }
        //     }
        // }
    }
    return phase;
}

Int64 CPhaser::ReconcileState()
{
    AutoPtr<IPhaser> root = mRoot;
    Int64 s = mState;
    if (!Object::Equals(root, TO_IINTERFACE(this))) {
        Int32 phase, p;
        // CAS to root phase with current parties, tripping unarrived
        assert(0 && "TODO");
        // while ((phase = (Int32)(root->mState >> PHASE_SHIFT)) !=
        //        (Int32)(s >>> PHASE_SHIFT) &&
        //        !UNSAFE.compareAndSwapLong
        //        (this, stateOffset, s,
        //         s = (((Int64)phase << PHASE_SHIFT) |
        //              ((phase < 0) ? (s & COUNTS_MASK) :
        //               (((p = (Int32)s >>> PARTIES_SHIFT) == 0) ? EMPTY :
        //                ((s & PARTIES_MASK) | p))))))
        //     s = state;
    }
    return s;
}

ECode CPhaser::constructor()
{
    return constructor(NULL, 0);
}

ECode CPhaser::constructor(
    /* [in] */ Int32 parties)
{
    return constructor(NULL, parties);
}

ECode CPhaser::constructor(
    /* [in] */ IPhaser* parent)
{
    return constructor(parent, 0);
}

ECode CPhaser::constructor(
    /* [in] */ IPhaser* parent,
    /* [in] */ Int32 parties)
{
    if (parties >> PARTIES_SHIFT != 0)
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    Int32 phase = 0;
    mParent = parent;
    if (parent != NULL) {
        AutoPtr<CPhaser> cp = (CPhaser*)parent;
        AutoPtr<IPhaser> root = cp->mRoot;
        mRoot = root;
        AutoPtr<CPhaser> cr = (CPhaser*)root.Get();
        mEvenQ = cr->mEvenQ;
        mOddQ = cr->mOddQ;
        if (parties != 0)
            phase = cp->DoRegister(1);
    }
    else {
        mRoot = this;
        CAtomicReference::New((IAtomicReference**)&mEvenQ);
        CAtomicReference::New((IAtomicReference**)&mOddQ);
    }
    mState = (parties == 0) ? (Int64)EMPTY :
        ((Int64)phase << PHASE_SHIFT) |
        ((Int64)parties << PARTIES_SHIFT) |
        ((Int64)parties);
    return NOERROR;
}

ECode CPhaser::Register(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = DoRegister(1);
    return NOERROR;
}

ECode CPhaser::BulkRegister(
    /* [in] */ Int32 parties,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    if (parties < 0)
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    if (parties == 0)
        return GetPhase(value);
    *value = DoRegister(parties);
    return NOERROR;
}

ECode CPhaser::Arrive(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = DoArrive(ONE_ARRIVAL);
    return NOERROR;
}

ECode CPhaser::ArriveAndDeregister(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = DoArrive(ONE_DEREGISTER);
    return NOERROR;
}

ECode CPhaser::ArriveAndAwaitAdvance(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    // Specialization of doArrive+awaitAdvance eliminating some reads/paths
    AutoPtr<IPhaser> root = mRoot;
    for (;;) {
        Int64 s = Object::Equals(root, TO_IINTERFACE(this)) ? mState : ReconcileState();
        Int32 phase = (Int32)(s >> PHASE_SHIFT);
        if (phase < 0) {
            *value = phase;
            return NOERROR;
        }
        Int32 counts = (Int32)s;
        Int32 unarrived = (counts == EMPTY) ? 0 : (counts & UNARRIVED_MASK);
        if (unarrived <= 0)
            return E_ILLEGAL_STATE_EXCEPTION;
//            throw new IllegalStateException(badArrive(s));
        assert(0 && "TODO");
        // if (UNSAFE.compareAndSwapLong(this, stateOffset, s,
        //                               s -= ONE_ARRIVAL)) {
        //     if (unarrived > 1)
        //         return root.internalAwaitAdvance(phase, NULL);
        //     if (root != this)
        //         return parent.arriveAndAwaitAdvance();
        //     Int64 n = s & PARTIES_MASK;  // base of next state
        //     Int32 nextUnarrived = (Int32)n >>> PARTIES_SHIFT;
        //     if (onAdvance(phase, nextUnarrived))
        //         n |= TERMINATION_BIT;
        //     else if (nextUnarrived == 0)
        //         n |= EMPTY;
        //     else
        //         n |= nextUnarrived;
        //     Int32 nextPhase = (phase + 1) & MAX_PHASE;
        //     n |= (Int64)nextPhase << PHASE_SHIFT;
        //     if (!UNSAFE.compareAndSwapLong(this, stateOffset, s, n))
        //         return (Int32)(state >>> PHASE_SHIFT); // terminated
        //     releaseWaiters(phase);
        //     return nextPhase;
        // }
    }
}

ECode CPhaser::AwaitAdvance(
    /* [in] */ Int32 phase,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    AutoPtr<IPhaser> root = mRoot;
    Int64 s = Object::Equals(root, TO_IINTERFACE(this)) ? mState : ReconcileState();
    Int32 p = (Int32)(s >> PHASE_SHIFT);
    if (phase < 0) {
        *value = phase;
        return NOERROR;
    }
    if (p == phase) {
        AutoPtr<CPhaser> cr = (CPhaser*)root.Get();
        *value = cr->InternalAwaitAdvance(phase, NULL);
        return NOERROR;
    }
    *value = p;
    return NOERROR;
}

ECode CPhaser::AwaitAdvanceInterruptibly(
    /* [in] */ Int32 phase,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    AutoPtr<IPhaser> root = mRoot;
    Int64 s = Object::Equals(root, TO_IINTERFACE(this)) ? mState : ReconcileState();
    Int32 p = (Int32)(s >> PHASE_SHIFT);
    if (phase < 0) {
        *value = phase;
        return NOERROR;
    }
    if (p == phase) {
        AutoPtr<QNode> node = new QNode(this, phase, TRUE, FALSE, 0L);
        AutoPtr<CPhaser> cr = (CPhaser*)root.Get();
        p = cr->InternalAwaitAdvance(phase, node);
        if (node->mWasInterrupted)
            return E_INTERRUPTED_EXCEPTION;
    }
    *value = p;
    return NOERROR;
}

ECode CPhaser::AwaitAdvanceInterruptibly(
    /* [in] */ Int32 phase,
    /* [in] */ Int64 timeout,
    /* [in] */ ITimeUnit* unit,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    Int64 nanos;
    unit->ToNanos(timeout, &nanos);
    AutoPtr<IPhaser> root = mRoot;
    Int64 s = Object::Equals(root, TO_IINTERFACE(this)) ? mState : ReconcileState();
    Int32 p = (Int32)(s >> PHASE_SHIFT);
    if (phase < 0) {
        *value = phase;
        return NOERROR;
    }
    if (p == phase) {
        AutoPtr<QNode> node = new QNode(this, phase, TRUE, TRUE, nanos);
        AutoPtr<CPhaser> cr = (CPhaser*)root.Get();
        p = cr->InternalAwaitAdvance(phase, node);
        if (node->mWasInterrupted)
            return E_INTERRUPTED_EXCEPTION;
        else if (p == phase)
            return E_TIMEOUT_EXCEPTION;
    }
    *value = p;
    return NOERROR;
}

ECode CPhaser::ForceTermination()
{
    // Only need to change root state
    AutoPtr<IPhaser> root = mRoot;
    Int64 s;
    assert(0 && "TODO");
    // while ((s = root->mState) >= 0) {
    //     if (UNSAFE.compareAndSwapLong(root, stateOffset,
    //                                   s, s | TERMINATION_BIT)) {
    //         // signal all threads
    //         releaseWaiters(0); // Waiters on evenQ
    //         releaseWaiters(1); // Waiters on oddQ
    //         return;
    //     }
    // }
    return NOERROR;
}

ECode CPhaser::GetPhase(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    AutoPtr<CPhaser> cr = (CPhaser*)mRoot.Get();
    *value = (Int32)(cr->mState >> PHASE_SHIFT);
    return NOERROR;
}

ECode CPhaser::GetRegisteredParties(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = PartiesOf(mState);
    return NOERROR;
}

ECode CPhaser::GetArrivedParties(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = ArrivedOf(ReconcileState());
    return NOERROR;
}

ECode CPhaser::GetUnarrivedParties(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = UnarrivedOf(ReconcileState());
    return NOERROR;
}

ECode CPhaser::GetParent(
    /* [out] */ IPhaser** outphase)
{
    VALIDATE_NOT_NULL(outphase);
    *outphase = mParent;
    REFCOUNT_ADD(*outphase);
    return NOERROR;
}

ECode CPhaser::GetRoot(
    /* [out] */ IPhaser** outphase)
{
    VALIDATE_NOT_NULL(outphase);
    *outphase = mRoot;
    REFCOUNT_ADD(*outphase);
    return NOERROR;
}

ECode CPhaser::IsTerminated(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    AutoPtr<CPhaser> cr = (CPhaser*)mRoot.Get();
    *value = cr->mState < 0L;
    return NOERROR;
}

Boolean CPhaser::OnAdvance(
    /* [in] */ Int32 phase,
    /* [in] */ Int32 registeredParties)
{
    return registeredParties == 0;
}

ECode CPhaser::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = StateToString(ReconcileState());
    return NOERROR;
}

String CPhaser::StateToString(
    /* [in] */ Int64 s)
{
    StringBuilder sb("[phase = ");
    sb += PhaseOf(s);
    sb += " parties = "; sb += PartiesOf(s);
    sb += " arrived = "; sb += ArrivedOf(s); sb += "]";
    return sb.ToString();
}

void CPhaser::ReleaseWaiters(
    /* [in] */ Int32 phase)
{
    AutoPtr<QNode> q;   // first element of queue
    AutoPtr<IThread> t;  // its thread
    AutoPtr<IAtomicReference> head = (phase & 1) == 0 ? mEvenQ : mOddQ;
    AutoPtr<CPhaser> cr = (CPhaser*)mRoot.Get();
    AutoPtr<IInterface> obj;
    while ((head->Get((IInterface**)&obj), q = (QNode*)IObject::Probe(obj)) != NULL &&
           q->mPhase != (Int32)(cr->mState >> PHASE_SHIFT)) {
        Boolean b = FALSE;
        if ((head->CompareAndSet(q->Probe(EIID_IInterface), q->mNext->Probe(EIID_IInterface), &b), b) &&
            (t = q->mThread) != NULL) {
            q->mThread = NULL;
            LockSupport::Unpark(t);
        }
    }
}

Int32 CPhaser::AbortWait(
    /* [in] */ Int32 phase)
{
    AutoPtr<IAtomicReference> head = (phase & 1) == 0 ? mEvenQ : mOddQ;
    for (;;) {
        AutoPtr<IThread> t;
        AutoPtr<IInterface> obj;
        head->Get((IInterface**)&obj);
        AutoPtr<QNode> q = (QNode*)IObject::Probe(obj);
        AutoPtr<CPhaser> cr = (CPhaser*)mRoot.Get();
        Int32 p = (Int32)(cr->mState >> PHASE_SHIFT);
        if (q == NULL || ((t = q->mThread) != NULL && q->mPhase == p))
            return p;
        Boolean b = FALSE;
        head->CompareAndSet(q->Probe(EIID_IInterface), q->mNext->Probe(EIID_IInterface), &b);
        if (b && t != NULL) {
            q->mThread = NULL;
            LockSupport::Unpark(t);
        }
    }
}

Int32 CPhaser::NCPU = 2; //Runtime.getRuntime().availableProcessors();

Int32 CPhaser::SPINS_PER_ARRIVAL = (NCPU < 2) ? 1 : 1 << 8;

Int32 CPhaser::InternalAwaitAdvance(
    /* [in] */ Int32 phase,
    /* [in] */ QNode* node)
{
    // assert root == this;
    ReleaseWaiters(phase-1);          // ensure old queue clean
    Boolean queued = FALSE;           // true when node is enqueued
    Int32 lastUnarrived = 0;            // to increase spins upon change
    Int32 spins = SPINS_PER_ARRIVAL;
    Int64 s;
    Int32 p;
    Boolean b = FALSE;
    while ((p = (Int32)((s = mState) >> PHASE_SHIFT)) == phase) {
        if (node == NULL) {           // spinning in noninterruptible mode
            Int32 unarrived = (Int32)s & UNARRIVED_MASK;
            if (unarrived != lastUnarrived &&
                (lastUnarrived = unarrived) < NCPU)
                spins += SPINS_PER_ARRIVAL;
            Boolean interrupted = Thread::Interrupted();
            if (interrupted || --spins < 0) { // need node to record intr
                node = new QNode(this, phase, FALSE, FALSE, 0L);
                node->mWasInterrupted = interrupted;
            }
        }
        else if ((node->IsReleasable(&b), b)) // done or aborted
            break;
        else if (!queued) {           // push onto queue
            AutoPtr<IAtomicReference> head = (phase & 1) == 0 ? mEvenQ : mOddQ;
            AutoPtr<IInterface> obj;
            head->Get((IInterface**)&obj);
            AutoPtr<QNode> q = (QNode*)IObject::Probe(obj);
            node->mNext = q;
            if ((q == NULL || q->mPhase == phase) &&
                (Int32)(mState >> PHASE_SHIFT) == phase) // avoid stale enq
                head->CompareAndSet(q->Probe(EIID_IInterface), node->Probe(EIID_IInterface), &queued);
        }
        else {
            // try {
            CForkJoinPool::ManagedBlock(node);
            // } catch (InterruptedException ie) {
            //     node.wasInterrupted = true;
            // }
        }
    }

    if (node != NULL) {
        if (node->mThread != NULL)
            node->mThread = NULL;       // avoid need for unpark()
        if (node->mWasInterrupted && !node->mInterruptible)
            Thread::GetCurrentThread()->Interrupt();
        if (p == phase && (p = (Int32)(mState >> PHASE_SHIFT)) == phase)
            return AbortWait(phase); // possibly clean up on abort
    }
    ReleaseWaiters(phase);
    return p;
}

//====================================================================
// CPhaser::QNode::
//====================================================================
CAR_INTERFACE_IMPL(CPhaser::QNode, Object, IForkJoinPoolManagedBlocker)

CPhaser::QNode::QNode(
    /* [in] */ IPhaser* phaser,
    /* [in] */ Int32 phase,
    /* [in] */ Boolean interruptible,
    /* [in] */ Boolean timed,
    /* [in] */ Int64 nanos)
{
    mPhaser = phaser;
    mPhase = phase;
    mInterruptible = interruptible;
    mNanos = nanos;
    mTimed = timed;

    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    mDeadLine = 0ll;
    if (timed) {
        system->GetNanoTime(&mDeadLine);
        mDeadLine += nanos;
    }

    mThread = Thread::GetCurrentThread();
}

ECode CPhaser::QNode::IsReleasable(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    if (mThread == NULL) {
        *res = TRUE;
        return NOERROR;
    }
    Int32 num;
    if ((mPhaser->GetPhase(&num), num) != mPhase) {
        mThread = NULL;
        *res = TRUE;
        return NOERROR;
    }
    if (Thread::Interrupted())
        mWasInterrupted = TRUE;
    if (mWasInterrupted && mInterruptible) {
        mThread = NULL;
        *res = TRUE;
        return NOERROR;
    }
    if (mTimed) {
        if (mNanos > 0L) {
            AutoPtr<ISystem> system;
            Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
            Int64 now = 0ll;
            system->GetNanoTime(&now);
            mNanos = mDeadLine - now;
        }
        if (mNanos <= 0L) {
            mThread = NULL;
            *res = TRUE;
            return NOERROR;
        }
    }
    *res = FALSE;
    return NOERROR;
}

ECode CPhaser::QNode::Block(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    Boolean b = FALSE;
    if ((IsReleasable(&b), b)) {
        *res = TRUE;
        return NOERROR;
    }
    else if (!mTimed) {
        LockSupport::Park(TO_IINTERFACE(this));
    }
    else if (mNanos > 0) {
        LockSupport::ParkNanos(TO_IINTERFACE(this), mNanos);
    }
    return IsReleasable(res);
}

} // namespace Concurrent
} // namespace Utility
} // namespace Elastos
