
#ifndef __ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CDELAYQUEUE_H__
#define __ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CDELAYQUEUE_H__

#include "_Elastos_Utility_Concurrent_CDelayQueue.h"
#include "AbstractQueue.h"

using Elastos::Core::IThread;
using Elastos::Utility::Concurrent::Locks::IReentrantLock;
using Elastos::Utility::Concurrent::Locks::ICondition;

namespace Elastos {
namespace Utility {
namespace Concurrent {

CarClass(CDelayQueue)
    , public AbstractQueue
    , public IDelayQueue
    , public IBlockingQueue
{
private:
    /**
     * Snapshot iterator that works off copy of underlying q array.
     */
    class Itr
        : public IIterator
        , public Object
    {
    public:
        CAR_INTERFACE_DECL()

        Itr(
            /* [in] */ IWeakReference* owner,
            /* [in] */ ArrayOf<IInterface*>* array);

        CARAPI HasNext(
            /* [out] */ Boolean* res);

        CARAPI GetNext(
            /* [out] */ IInterface** res);

        CARAPI Remove();

    public:
        AutoPtr<ArrayOf<IInterface*> > mArray;         // Array of all elements
        Int32 mCursor;           // index of next element to return
        Int32 mLastRet;          // index of last element, or -1 if no such
        AutoPtr<IWeakReference> mWeakOwner;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /**
     * Creates a new <tt>DelayQueue</tt> that is initially empty.
     */
    CARAPI constructor();

    /**
     * Creates a <tt>DelayQueue</tt> initially containing the elements of the
     * given collection of {@link Delayed} instances.
     *
     * @param c the collection of elements to initially contain
     * @throws NullPointerException if the specified collection or any
     *         of its elements are null
     */
    CARAPI constructor(
        /* [in] */ ICollection* c);

    CARAPI Add(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* modified);

    CARAPI Offer(
        /* [in] */ IInterface* e,
        /* [out] */ Boolean* result);

    CARAPI Put(
        /* [in] */ IInterface* e);

    CARAPI Offer(
        /* [in] */ IInterface* e,
        /* [in] */ Int64 timeout,
        /* [in] */ ITimeUnit* unit,
        /* [out] */ Boolean* result);

    CARAPI Poll(
        /* [out] */ IInterface** e);

    CARAPI Take(
        /* [out] */ IInterface** e);

    CARAPI Poll(
        /* [in] */ Int64 timeout,
        /* [in] */ ITimeUnit* unit,
        /* [out] */ IInterface** e);

    CARAPI Peek(
        /* [out] */ IInterface** e);

    CARAPI GetSize(
        /* [out] */ Int32* size);

    CARAPI DrainTo(
        /* [in] */ ICollection* c,
        /* [out] */ Int32* number);

    CARAPI DrainTo(
        /* [in] */ ICollection* c,
        /* [in] */ Int32 maxElements,
        /* [out] */ Int32* number);

    CARAPI Clear();

    CARAPI RemainingCapacity(
        /* [out] */ Int32* capacity);

    CARAPI ToArray(
        /* [out, callee] */ ArrayOf<IInterface*>** array);

    CARAPI ToArray(
        /* [in] */ ArrayOf<IInterface*>* inArray,
        /* [out, callee] */ ArrayOf<IInterface*>** outArray);

    CARAPI Remove(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* modified);

    CARAPI GetIterator(
        /* [out] */ IIterator** it);

    CARAPI RemoveEQ(
        /* [in] */ IInterface* o);

    CARAPI Equals(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

private:
    /**
     * Return first element only if it is expired.
     * Used only by drainTo.  Call only when holding lock.
     */
    CARAPI_(AutoPtr<IInterface>) PeekExpired();

protected:
    CARAPI_(String) GetClassName() { return String("CDelayQueue"); }

private:
    AutoPtr<IReentrantLock> mLock;
    AutoPtr<IPriorityQueue> mQ;

    /**
     * Thread designated to wait for the element at the head of
     * the queue.  This variant of the Leader-Follower pattern
     * (http://www.cs.wustl.edu/~schmidt/POSA/POSA2/) serves to
     * minimize unnecessary timed waiting.  When a thread becomes
     * the leader, it waits only for the next delay to elapse, but
     * other threads await indefinitely.  The leader thread must
     * signal some other thread before returning from take() or
     * poll(...), unless some other thread becomes leader in the
     * interim.  Whenever the head of the queue is replaced with
     * an element with an earlier expiration time, the leader
     * field is invalidated by being reset to null, and some
     * waiting thread, but not necessarily the current leader, is
     * signalled.  So waiting threads must be prepared to acquire
     * and lose leadership while waiting.
     */
    AutoPtr<IThread> mLeader;

    /**
     * Condition signalled when a newer element becomes available
     * at the head of the queue or a new thread may need to
     * become leader.
     */
    AutoPtr<ICondition> mAvailable;
};

} // namespace Concurrent
} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CDELAYQUEUE_H__
