
#ifndef __ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CCYCLICBARRIER_H__
#define __ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CCYCLICBARRIER_H__

#include "_Elastos_Utility_Concurrent_CCyclicBarrier.h"
#include "Object.h"

using Elastos::Core::IRunnable;
using Elastos::Utility::Concurrent::Locks::IReentrantLock;
using Elastos::Utility::Concurrent::Locks::ICondition;

namespace Elastos {
namespace Utility {
namespace Concurrent {

CarClass(CCyclicBarrier)
    , public Object
    , public ICyclicBarrier
{
private:
    /**
     * Each use of the barrier is represented as a generation instance.
     * The generation changes whenever the barrier is tripped, or
     * is reset. There can be many generations associated with threads
     * using the barrier - due to the non-deterministic way the lock
     * may be allocated to waiting threads - but only one of these
     * can be active at a time (the one to which {@code count} applies)
     * and all the rest are either broken or tripped.
     * There need not be an active generation if there has been a break
     * but no subsequent reset.
     */
    class Generation
        : public Object
    {
    public:
        Generation() {
            mBroken = FALSE;
        }

    public:
        Boolean mBroken;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()
    /**
     * Creates a new {@code CyclicBarrier} that will trip when the
     * given number of parties (threads) are waiting upon it, and which
     * will execute the given barrier action when the barrier is tripped,
     * performed by the last thread entering the barrier.
     *
     * @param parties the number of threads that must invoke {@link #await}
     *        before the barrier is tripped
     * @param barrierAction the command to execute when the barrier is
     *        tripped, or {@code null} if there is no action
     * @throws IllegalArgumentException if {@code parties} is less than 1
     */
    CARAPI constructor(
        /* [in] */ Int32 parties,
        /* [in] */ IRunnable* barrierAction);

    /**
     * Creates a new {@code CyclicBarrier} that will trip when the
     * given number of parties (threads) are waiting upon it, and
     * does not perform a predefined action when the barrier is tripped.
     *
     * @param parties the number of threads that must invoke {@link #await}
     *        before the barrier is tripped
     * @throws IllegalArgumentException if {@code parties} is less than 1
     */
    CARAPI constructor(
        /* [in] */ Int32 parties);

    CARAPI GetParties(
        /* [out] */ Int32* value);

    CARAPI Await(
        /* [out] */ Int32* value);

    CARAPI Await(
        /* [in] */ Int64 timeout,
        /* [in] */ ITimeUnit* unit,
        /* [out] */ Int32* value);

    CARAPI IsBroken(
        /* [out] */ Boolean* value);

    CARAPI Reset();

    CARAPI GetNumberWaiting(
        /* [out] */ Int32* value);

private:
    /**
     * Updates state on barrier trip and wakes up everyone.
     * Called only while holding lock.
     */
    CARAPI_(void) NextGeneration();

    /**
     * Sets current barrier generation as broken and wakes up everyone.
     * Called only while holding lock.
     */
    CARAPI_(void) BreakBarrier();

    /**
     * Main barrier code, covering the various policies.
     */
    CARAPI_(Int32) Dowait(
        /* [in] */ Boolean timed,
        /* [in] */ Int64 nanos);

private:
    /** The lock for guarding barrier entry */
    AutoPtr<IReentrantLock> mLock;
    /** Condition to wait on until tripped */
    AutoPtr<ICondition> mTrip;
    /** The number of parties */
    Int32 mParties;
    /* The command to run when tripped */
    AutoPtr<IRunnable> mBarrierCommand;
    /** The current generation */
    AutoPtr<Generation> mGeneration;

    /**
     * Number of parties still waiting. Counts down from parties to 0
     * on each generation.  It is reset to parties on each new
     * generation or when broken.
     */
    Int32 mCount;
};

} // namespace Concurrent
} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_CCONCURRENTHASHMAP_H__
