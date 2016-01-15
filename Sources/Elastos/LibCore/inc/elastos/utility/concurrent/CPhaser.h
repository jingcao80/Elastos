
#ifndef __ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CPHASER_H__
#define __ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CPHASER_H__

#include "_Elastos_Utility_Concurrent_CPhaser.h"
#include "Object.h"

using Elastos::Core::IThread;
using Elastos::Utility::Concurrent::Atomic::IAtomicReference;

namespace Elastos {
namespace Utility {
namespace Concurrent {

CarClass(CPhaser)
    , public Object
    , public IPhaser
{
public:
    /**
     * Wait nodes for Treiber stack representing wait queue
     */
    class QNode
        : public IForkJoinPoolManagedBlocker
        , public Object
    {
    public:
        CAR_INTERFACE_DECL();

        QNode(
            /* [in] */ IPhaser* phaser,
            /* [in] */ Int32 phase,
            /* [in] */ Boolean interruptible,
            /* [in] */ Boolean timed,
            /* [in] */ Int64 nanos);

        CARAPI IsReleasable(
            /* [out] */ Boolean* res);

        CARAPI Block(
            /* [out] */ Boolean* res);

    public:
        AutoPtr<IPhaser> mPhaser;
        Int32 mPhase;
        Boolean mInterruptible;
        Boolean mTimed;
        Boolean mWasInterrupted;
        Int64 mNanos;
        Int64 mDeadLine;
        AutoPtr<IThread> mThread; // nulled to cancel wait
        AutoPtr<QNode> mNext;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /**
     * Creates a new phaser with no initially registered parties, no
     * parent, and initial phase number 0. Any thread using this
     * phaser will need to first register for it.
     */
    CARAPI constructor();

    /**
     * Creates a new phaser with the given number of registered
     * unarrived parties, no parent, and initial phase number 0.
     *
     * @param parties the number of parties required to advance to the
     * next phase
     * @throws IllegalArgumentException if parties less than zero
     * or greater than the maximum number of parties supported
     */
    CARAPI constructor(
        /* [in] */ Int32 parties);

    /**
     * Equivalent to {@link #Phaser(Phaser, int) Phaser(parent, 0)}.
     *
     * @param parent the parent phaser
     */
    CARAPI constructor(
        /* [in] */ IPhaser* parent);

    /**
     * Creates a new phaser with the given parent and number of
     * registered unarrived parties.  When the given parent is non-null
     * and the given number of parties is greater than zero, this
     * child phaser is registered with its parent.
     *
     * @param parent the parent phaser
     * @param parties the number of parties required to advance to the
     * next phase
     * @throws IllegalArgumentException if parties less than zero
     * or greater than the maximum number of parties supported
     */
    CARAPI constructor(
        /* [in] */ IPhaser* parent,
        /* [in] */ Int32 parties);

    CARAPI Register(
        /* [out] */ Int32* value);

    CARAPI BulkRegister(
        /* [in] */ Int32 parties,
        /* [out] */ Int32* value);

    CARAPI Arrive(
        /* [out] */ Int32* value);

    CARAPI ArriveAndDeregister(
        /* [out] */ Int32* value);

    CARAPI ArriveAndAwaitAdvance(
        /* [out] */ Int32* value);

    CARAPI AwaitAdvance(
        /* [in] */ Int32 phase,
        /* [out] */ Int32* value);

    CARAPI AwaitAdvanceInterruptibly(
        /* [in] */ Int32 phase,
        /* [out] */ Int32* value);

    CARAPI AwaitAdvanceInterruptibly(
        /* [in] */ Int32 phase,
        /* [in] */ Int64 timeout,
        /* [in] */ ITimeUnit* unit,
        /* [out] */ Int32* value);

    CARAPI ForceTermination();

    CARAPI GetPhase(
        /* [out] */ Int32* value);

    CARAPI GetRegisteredParties(
        /* [out] */ Int32* value);

    CARAPI GetArrivedParties(
        /* [out] */ Int32* value);

    CARAPI GetUnarrivedParties(
        /* [out] */ Int32* value);

    CARAPI GetParent(
        /* [out] */ IPhaser** outphase);

    CARAPI GetRoot(
        /* [out] */ IPhaser** outphase);

    CARAPI IsTerminated(
        /* [out] */ Boolean* value);

    CARAPI ToString(
        /* [out] */ String* str);

protected:
    /**
     * Overridable method to perform an action upon impending phase
     * advance, and to control termination. This method is invoked
     * upon arrival of the party advancing this phaser (when all other
     * waiting parties are dormant).  If this method returns {@code
     * true}, this phaser will be set to a final termination state
     * upon advance, and subsequent calls to {@link #isTerminated}
     * will return true. Any (unchecked) Exception or Error thrown by
     * an invocation of this method is propagated to the party
     * attempting to advance this phaser, in which case no advance
     * occurs.
     *
     * <p>The arguments to this method provide the state of the phaser
     * prevailing for the current transition.  The effects of invoking
     * arrival, registration, and waiting methods on this phaser from
     * within {@code onAdvance} are unspecified and should not be
     * relied on.
     *
     * <p>If this phaser is a member of a tiered set of phasers, then
     * {@code onAdvance} is invoked only for its root phaser on each
     * advance.
     *
     * <p>To support the most common use cases, the default
     * implementation of this method returns {@code true} when the
     * number of registered parties has become zero as the result of a
     * party invoking {@code arriveAndDeregister}.  You can disable
     * this behavior, thus enabling continuation upon future
     * registrations, by overriding this method to always return
     * {@code false}:
     *
     * <pre> {@code
     * Phaser phaser = new Phaser() {
     *   protected boolean onAdvance(int phase, int parties) { return false; }
     * }}</pre>
     *
     * @param phase the current phase number on entry to this method,
     * before this phaser is advanced
     * @param registeredParties the current number of registered parties
     * @return {@code true} if this phaser should terminate
     */
    CARAPI_(Boolean) OnAdvance(
        /* [in] */ Int32 phase,
        /* [in] */ Int32 registeredParties);

private:
    // The following unpacking methods are usually manually inlined

    static CARAPI_(Int32) UnarrivedOf(
        /* [in] */ Int64 s);

    static CARAPI_(Int32) PartiesOf(
        /* [in] */ Int64 s);

    static CARAPI_(Int32) PhaseOf(
        /* [in] */ Int64 s);

    static CARAPI_(Int32) ArrivedOf(
        /* [in] */ Int64 s);

    CARAPI_(AutoPtr<IAtomicReference>) QueueFor(
        /* [in] */ Int32 phase);

    /**
     * Returns message string for bounds exceptions on arrival.
     */
    CARAPI_(String) BadArrive(
        /* [in] */ Int64 s);

    /**
     * Returns message string for bounds exceptions on registration.
     */
    CARAPI_(String) BadRegister(
        /* [in] */ Int64 s);

    /**
     * Main implementation for methods arrive and arriveAndDeregister.
     * Manually tuned to speed up and minimize race windows for the
     * common case of just decrementing unarrived field.
     *
     * @param adjust value to subtract from state;
     *               ONE_ARRIVAL for arrive,
     *               ONE_DEREGISTER for arriveAndDeregister
     */
    CARAPI_(Int32) DoArrive(
        /* [in] */ Int32 adjust);

    /**
     * Implementation of register, bulkRegister
     *
     * @param registrations number to add to both parties and
     * unarrived fields. Must be greater than zero.
     */
    CARAPI_(Int32) DoRegister(
        /* [in] */ Int32 registrations);

    /**
     * Resolves lagged phase propagation from root if necessary.
     * Reconciliation normally occurs when root has advanced but
     * subphasers have not yet done so, in which case they must finish
     * their own advance by setting unarrived to parties (or if
     * parties is zero, resetting to unregistered EMPTY state).
     *
     * @return reconciled state
     */
    CARAPI_(Int64) ReconcileState();

    /**
     * Implementation of toString and string-based error messages
     */
    CARAPI_(String) StateToString(
        /* [in] */ Int64 s);

    // Waiting mechanics

    /**
     * Removes and signals threads from queue for phase.
     */
    CARAPI_(void) ReleaseWaiters(
        /* [in] */ Int32 phase);

    /**
     * Variant of releaseWaiters that additionally tries to remove any
     * nodes no longer waiting for advance due to timeout or
     * interrupt. Currently, nodes are removed only if they are at
     * head of queue, which suffices to reduce memory footprint in
     * most usages.
     *
     * @return current phase on exit
     */
    CARAPI_(Int32) AbortWait(
        /* [in] */ Int32 phase);

    /**
     * Possibly blocks and waits for phase to advance unless aborted.
     * Call only on root phaser.
     *
     * @param phase current phase
     * @param node if non-null, the wait node to track interrupt and timeout;
     * if null, denotes noninterruptible wait
     * @return current phase
     */
    CARAPI_(Int32) InternalAwaitAdvance(
        /* [in] */ Int32 phase,
        /* [in] */ QNode* node);

public:
    /**
     * The number of times to spin before blocking while waiting for
     * advance, per arrival while waiting. On multiprocessors, fully
     * blocking and waking up a large number of threads all at once is
     * usually a very slow process, so we use rechargeable spins to
     * avoid it when threads regularly arrive: When a thread in
     * internalAwaitAdvance notices another arrival before blocking,
     * and there appear to be enough CPUs available, it spins
     * SPINS_PER_ARRIVAL more times before blocking. The value trades
     * off good-citizenship vs big unnecessary slowdowns.
     */
    static Int32 SPINS_PER_ARRIVAL;

private:
    Int64 mState;

    static Int32  MAX_PARTIES;
    static Int32  MAX_PHASE;
    static Int32  PARTIES_SHIFT;
    static Int32  PHASE_SHIFT;
    static Int32  UNARRIVED_MASK;      // to mask ints
    static Int64 PARTIES_MASK; // to mask longs
    static Int64 COUNTS_MASK;
    static Int64 TERMINATION_BIT;

    // some special values
    static Int32  ONE_ARRIVAL;
    static Int32  ONE_PARTY;
    static Int32  ONE_DEREGISTER;
    static Int32  EMPTY;

    /**
     * The parent of this phaser, or null if none
     */
    AutoPtr<IPhaser> mParent;

    /**
     * The root of phaser tree. Equals this if not in a tree.
     */
    AutoPtr<IPhaser> mRoot;

    /**
     * Heads of Treiber stacks for waiting threads. To eliminate
     * contention when releasing some threads while adding others, we
     * use two of them, alternating across even and odd phases.
     * Subphasers share queues with root to speed up releases.
     */
    AutoPtr<IAtomicReference> mEvenQ;
    AutoPtr<IAtomicReference> mOddQ;

    /** The number of CPUs, for spin control */
    static Int32 NCPU;

    // // Unsafe mechanics

    // private static final sun.misc.Unsafe UNSAFE;
    // private static final long stateOffset;
    // static {
    //     try {
    //         UNSAFE = sun.misc.Unsafe.getUnsafe();
    //         Class<?> k = Phaser.class;
    //         stateOffset = UNSAFE.objectFieldOffset
    //             (k.getDeclaredField("state"));
    //     } catch (Exception e) {
    //         throw new Error(e);
    //     }
    // }
};

} // namespace Concurrent
} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CPHASER_H__
