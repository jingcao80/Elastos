
#ifndef __ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CARRAYBLOCKINGQUEUE_H__
#define __ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CARRAYBLOCKINGQUEUE_H__

#include "_Elastos_Utility_Concurrent_CArrayBlockingQueue.h"
#include "AbstractQueue.h"

using Elastos::IO::ISerializable;
using Elastos::Utility::Concurrent::Locks::IReentrantLock;
using Elastos::Utility::Concurrent::Locks::ICondition;

namespace Elastos {
namespace Utility {
namespace Concurrent {

CarClass(CArrayBlockingQueue)
    , public AbstractQueue
    , public IArrayBlockingQueue
    , public IBlockingQueue
    , public ISerializable
{
private:
    /**
     * Iterator for ArrayBlockingQueue.
     *
     * To maintain weak consistency with respect to puts and takes, we
     * read ahead one slot, so as to not report hasNext true but then
     * not have an element to return.
     *
     * We switch into "detached" mode (allowing prompt unlinking from
     * itrs without help from the GC) when all indices are negative, or
     * when hasNext returns false for the first time.  This allows the
     * iterator to track concurrent updates completely accurately,
     * except for the corner case of the user calling Iterator.remove()
     * after hasNext() returned false.  Even in this case, we ensure
     * that we don't remove the wrong element by keeping track of the
     * expected element to remove, in lastItem.  Yes, we may fail to
     * remove lastItem from the queue if it moved due to an interleaved
     * interior remove while in detached mode.
     */
    class Itr
        : public Object
        , public IIterator
    {
    public:
        CAR_INTERFACE_DECL()

        Itr(
            /* [in] */ CArrayBlockingQueue* host);

        CARAPI_(Boolean) IsDetached();

        /**
         * For performance reasons, we would like not to acquire a lock in
         * hasNext in the common case.  To allow for this, we only access
         * fields (i.e. nextItem) that are not modified by update operations
         * triggered by queue modifications.
         */
        CARAPI HasNext(
            /* [out] */ Boolean* result);

        CARAPI GetNext(
            /* [out] */ IInterface** object);

        CARAPI Remove();

        /**
         * Called to notify the iterator that the queue is empty, or that it
         * has fallen hopelessly behind, so that it should abandon any
         * further iteration, except possibly to return one more element
         * from next(), as promised by returning true from hasNext().
         */
        CARAPI Shutdown();

        /**
         * Called whenever an interior remove (not at takeIndex) occured.
         *
         * @return true if this iterator should be unlinked from itrs
         */
        CARAPI_(Boolean) RemovedAt(
            /* [in] */ Int32 removedIndex);

        /**
         * Called whenever takeIndex wraps around to zero.
         *
         * @return true if this iterator should be unlinked from itrs
         */
        CARAPI_(Boolean) TakeIndexWrapped();

        // /** Uncomment for debugging. */
        // public String toString() {
        //     return ("cursor=" + cursor + " " +
        //             "nextIndex=" + nextIndex + " " +
        //             "lastRet=" + lastRet + " " +
        //             "nextItem=" + nextItem + " " +
        //             "lastItem=" + lastItem + " " +
        //             "prevCycles=" + prevCycles + " " +
        //             "prevTakeIndex=" + prevTakeIndex + " " +
        //             "size()=" + size() + " " +
        //             "remainingCapacity()=" + remainingCapacity());
        // }

    private:
        CARAPI_(Int32) IncCursor(
            /* [in] */ Int32 index);

        /**
         * Returns true if index is invalidated by the given number of
         * dequeues, starting from prevTakeIndex.
         */
        CARAPI_(Boolean) Invalidated(
            /* [in] */ Int32 index,
            /* [in] */ Int32 prevTakeIndex,
            /* [in] */ Int64 dequeues,
            /* [in] */ Int32 length);

        /**
         * Adjusts indices to incorporate all dequeues since the last
         * operation on this iterator.  Call only from iterating thread.
         */
        CARAPI IncorporateDequeues();

        /**
         * Called when itrs should stop tracking this iterator, either
         * because there are no more indices to update (cursor < 0 &&
         * nextIndex < 0 && lastRet < 0) or as a special exception, when
         * lastRet >= 0, because hasNext() is about to return false for the
         * first time.  Call only from iterating thread.
         */
        CARAPI Detach();

        CARAPI_(Int32) Distance(
            /* [in] */ Int32 index,
            /* [in] */ Int32 prevTakeIndex,
            /* [in] */ Int32 length);

        CARAPI NoNext();

    private:
        CArrayBlockingQueue* mHost;

        /** Index to look for new nextItem; NONE at end */
        Int32 mCursor;

        /** Element to be returned by next call to next(); null if none */
        AutoPtr<IInterface> mNextItem;

        /** Index of nextItem; NONE if none, REMOVED if removed elsewhere */
        Int32 mNextIndex;

        /** Last element returned; null if none or not detached. */
        AutoPtr<IInterface> mLastItem;

        /** Index of lastItem, NONE if none, REMOVED if removed elsewhere */
        Int32 mLastRet;

        /** Previous value of takeIndex, or DETACHED when detached */
        Int32 mPrevTakeIndex;

        /** Previous value of iters.cycles */
        Int32 mPrevCycles;

        /** Special index value indicating "not available" or "undefined" */
        static const Int32 NONE;

        /**
         * Special index value indicating "removed elsewhere", that is,
         * removed by some operation other than a call to this.remove().
         */
        static const Int32 REMOVED;

        /** Special value for prevTakeIndex indicating "detached mode" */
        static const Int32 DETACHED;
    };

public:
    /**
     * Shared data between iterators and their queue, allowing queue
     * modifications to update iterators when elements are removed.
     *
     * This adds a lot of complexity for the sake of correctly
     * handling some uncommon operations, but the combination of
     * circular-arrays and supporting interior removes (i.e., those
     * not at head) would cause iterators to sometimes lose their
     * places and/or (re)report elements they shouldn't.  To avoid
     * this, when a queue has one or more iterators, it keeps iterator
     * state consistent by:
     *
     * (1) keeping track of the number of "cycles", that is, the
     *     number of times takeIndex has wrapped around to 0.
     * (2) notifying all iterators via the callback removedAt whenever
     *     an interior element is removed (and thus other elements may
     *     be shifted).
     *
     * These suffice to eliminate iterator inconsistencies, but
     * unfortunately add the secondary responsibility of maintaining
     * the list of iterators.  We track all active iterators in a
     * simple linked list (accessed only when the queue's lock is
     * held) of weak references to Itr.  The list is cleaned up using
     * 3 different mechanisms:
     *
     * (1) Whenever a new iterator is created, do some O(1) checking for
     *     stale list elements.
     *
     * (2) Whenever takeIndex wraps around to 0, check for iterators
     *     that have been unused for more than one wrap-around cycle.
     *
     * (3) Whenever the queue becomes empty, all iterators are notified
     *     and this entire data structure is discarded.
     *
     * So in addition to the removedAt callback that is necessary for
     * correctness, iterators have the shutdown and takeIndexWrapped
     * callbacks that help remove stale iterators from the list.
     *
     * Whenever a list element is examined, it is expunged if either
     * the GC has determined that the iterator is discarded, or if the
     * iterator reports that it is "detached" (does not need any
     * further state updates).  Overhead is maximal when takeIndex
     * never advances, iterators are discarded before they are
     * exhausted, and all removals are interior removes, in which case
     * all stale iterators are discovered by the GC.  But even in this
     * case we don't increase the amortized complexity.
     *
     * Care must be taken to keep list sweeping methods from
     * reentrantly invoking another such method, causing subtle
     * corruption bugs.
     */
    class Itrs
        : public Object
    {
    private:
        /**
         * Node in a linked list of weak iterator references.
         */
        class Node
            : public Object // extends WeakReference<Itr> {
        {
        public:
            Node(
                /* [in] */ Itr* iterator,
                /* [in] */ Node* next)
            {
                // super(iterator);
                mNext = next;
            }

        public:
            AutoPtr<Node> mNext;
        };

    public:
        Itrs(
            /* [in] */ Itr* initial,
            /* [in] */ CArrayBlockingQueue* owner);

        /**
         * Sweeps itrs, looking for and expunging stale iterators.
         * If at least one was found, tries harder to find more.
         * Called only from iterating thread.
         *
         * @param tryHarder whether to start in try-harder mode, because
         * there is known to be at least one iterator to collect
         */
        CARAPI DoSomeSweeping(
            /* [in] */ Boolean tryHarder);

        /**
         * Adds a new iterator to the linked list of tracked iterators.
         */
        CARAPI Register(
            /* [in] */ Itr* itr);

        /**
         * Called whenever takeIndex wraps around to 0.
         *
         * Notifies all iterators, and expunges any that are now stale.
         */
        CARAPI TakeIndexWrapped();

        /**
         * Called whenever an interior remove (not at takeIndex) occured.
         *
         * Notifies all iterators, and expunges any that are now stale.
         */
        CARAPI RemovedAt(
            /* [in] */ Int32 removedIndex);

        /**
         * Called whenever the queue becomes empty.
         *
         * Notifies all active iterators that the queue is empty,
         * clears all weak refs, and unlinks the itrs datastructure.
         */
        CARAPI QueueIsEmpty();

        /**
         * Called whenever an element has been dequeued (at takeIndex).
         */
        CARAPI ElementDequeued();

    public:
        /** Incremented whenever takeIndex wraps around to 0 */
        Int32 mCycles;

    private:
        /** Linked list of weak iterator references */
        AutoPtr<Node> mHead;

        /** Used to expunge stale iterators */
        AutoPtr<Node> mSweeper;

        static const Int32 SHORT_SWEEP_PROBES;
        static const Int32 LONG_SWEEP_PROBES;

        CArrayBlockingQueue* mOwner;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CArrayBlockingQueue();

    /**
     * Creates an {@code ArrayBlockingQueue} with the given (fixed)
     * capacity and default access policy.
     *
     * @param capacity the capacity of this queue
     * @throws IllegalArgumentException if {@code capacity < 1}
     */
    CARAPI constructor(
        /* [in] */ Int32 capacity);

    /**
     * Creates an {@code ArrayBlockingQueue} with the given (fixed)
     * capacity and the specified access policy.
     *
     * @param capacity the capacity of this queue
     * @param fair if {@code true} then queue accesses for threads blocked
     *        on insertion or removal, are processed in FIFO order;
     *        if {@code false} the access order is unspecified.
     * @throws IllegalArgumentException if {@code capacity < 1}
     */
    CARAPI constructor(
        /* [in] */ Int32 capacity,
        /* [in] */ Boolean fair);

    /**
     * Creates an {@code ArrayBlockingQueue} with the given (fixed)
     * capacity, the specified access policy and initially containing the
     * elements of the given collection,
     * added in traversal order of the collection's iterator.
     *
     * @param capacity the capacity of this queue
     * @param fair if {@code true} then queue accesses for threads blocked
     *        on insertion or removal, are processed in FIFO order;
     *        if {@code false} the access order is unspecified.
     * @param c the collection of elements to initially contain
     * @throws IllegalArgumentException if {@code capacity} is less than
     *         {@code c.size()}, or less than 1.
     * @throws NullPointerException if the specified collection or any
     *         of its elements are null
     */
    CARAPI constructor(
        /* [in] */ Int32 capacity,
        /* [in] */ Boolean fair,
        /* [in] */ ICollection* c);

    /**
     * Inserts the specified element at the tail of this queue if it is
     * possible to do so immediately without exceeding the queue's capacity,
     * returning {@code true} upon success and throwing an
     * {@code IllegalStateException} if this queue is full.
     *
     * @param e the element to add
     * @return {@code true} (as specified by {@link Collection#add})
     * @throws IllegalStateException if this queue is full
     * @throws NullPointerException if the specified element is null
     */
    // CARAPI Add(
    //     /* [in] */ IInterface* e,
    //     /* [out] */ Boolean* modified);

    /**
     * Inserts the specified element at the tail of this queue if it is
     * possible to do so immediately without exceeding the queue's capacity,
     * returning {@code true} upon success and {@code false} if this queue
     * is full.  This method is generally preferable to method {@link #add},
     * which can fail to insert an element only by throwing an exception.
     *
     * @throws NullPointerException if the specified element is null
     */
    CARAPI Offer(
        /* [in] */ IInterface* e,
        /* [out] */ Boolean* result);

    /**
     * Inserts the specified element at the tail of this queue, waiting
     * for space to become available if the queue is full.
     *
     * @throws InterruptedException {@inheritDoc}
     * @throws NullPointerException {@inheritDoc}
     */
    CARAPI Put(
        /* [in] */ IInterface* e);

    /**
     * Inserts the specified element at the tail of this queue, waiting
     * up to the specified wait time for space to become available if
     * the queue is full.
     *
     * @throws InterruptedException {@inheritDoc}
     * @throws NullPointerException {@inheritDoc}
     */
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

    CARAPI GetElement(
        /* [out] */ IInterface** e);

    // this doc comment is overridden to remove the reference to collections
    // greater in size than Integer.MAX_VALUE
    /**
     * Returns the number of elements in this queue.
     *
     * @return the number of elements in this queue
     */
    CARAPI GetSize(
        /* [out] */ Int32* size);

    // this doc comment is a modified copy of the inherited doc comment,
    // without the reference to unlimited queues.
    /**
     * Returns the number of additional elements that this queue can ideally
     * (in the absence of memory or resource constraints) accept without
     * blocking. This is always equal to the initial capacity of this queue
     * less the current {@code size} of this queue.
     *
     * <p>Note that you <em>cannot</em> always tell if an attempt to insert
     * an element will succeed by inspecting {@code remainingCapacity}
     * because it may be the case that another thread is about to
     * insert or remove an element.
     */
    CARAPI RemainingCapacity(
        /* [out] */ Int32* capacity);

    /**
     * Tests whether this {@code Collection} contains all objects contained in the
     * specified {@code Collection}. If an element {@code elem} is contained several
     * times in the specified {@code Collection}, the method returns {@code true} even
     * if {@code elem} is contained only once in this {@code Collection}.
     *
     * @param collection
     *            the collection of objects.
     * @return {@code true} if all objects in the specified {@code Collection} are
     *         elements of this {@code Collection}, {@code false} otherwise.
     * @throws ClassCastException
     *                if one or more elements of {@code collection} isn't of the
     *                correct type.
     * @throws NullPointerException
     *                if {@code collection} contains at least one {@code null}
     *                element and this {@code Collection} doesn't support {@code null}
     *                elements.
     * @throws NullPointerException
     *                if {@code collection} is {@code null}.
     */
    // CARAPI ContainsAll(
    //     /* [in] */ ICollection* collection,
    //     /* [out] */ Boolean* result);

    /**
     * Compares the argument to the receiver, and returns true if they represent
     * the <em>same</em> object using a class specific comparison.
     *
     * @param object
     *            the object to compare with this object.
     * @return {@code true} if the object is the same as this object and
     *         {@code false} if it is different from this object.
     * @see #hashCode
     */
    CARAPI Equals(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* result);

    /**
     * Returns an integer hash code for the receiver. Objects which are equal
     * return the same value for this method.
     *
     * @return the receiver's hash.
     *
     * @see #equals
     */
    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    /**
     * Returns if this {@code Collection} contains no elements.
     *
     * @return {@code true} if this {@code Collection} has no elements, {@code false}
     *         otherwise.
     *
     * @see #size
     */
    // CARAPI IsEmpty(
    //       [out]  Boolean* result);

    /**
     * Removes a single instance of the specified element from this queue,
     * if it is present.  More formally, removes an element {@code e} such
     * that {@code o.equals(e)}, if this queue contains one or more such
     * elements.
     * Returns {@code true} if this queue contained the specified element
     * (or equivalently, if this queue changed as a result of the call).
     *
     * <p>Removal of interior elements in circular array based queues
     * is an intrinsically slow and disruptive operation, so should
     * be undertaken only in exceptional circumstances, ideally
     * only when the queue is known not to be accessible by other
     * threads.
     *
     * @param o element to be removed from this queue, if present
     * @return {@code true} if this queue changed as a result of the call
     */
    CARAPI Remove(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* modified);

    // CARAPI Remove(
    //      [out]  IInterface** e);

    /**
     * Removes all occurrences in this {@code Collection} of each object in the
     * specified {@code Collection} (optional). After this method returns none of the
     * elements in the passed {@code Collection} can be found in this {@code Collection}
     * anymore.
     *
     * @param collection
     *            the collection of objects to remove.
     * @return {@code true} if this {@code Collection} is modified, {@code false}
     *         otherwise.
     *
     * @throws UnsupportedOperationException
     *                if removing from this {@code Collection} is not supported.
     * @throws ClassCastException
     *                if one or more elements of {@code collection}
     *                isn't of the correct type.
     * @throws NullPointerException
     *                if {@code collection} contains at least one
     *                {@code null} element and this {@code Collection} doesn't support
     *                {@code null} elements.
     * @throws NullPointerException
     *                if {@code collection} is {@code null}.
     */
    // CARAPI RemoveAll(
    //     /* [in] */ ICollection* collection,
    //     /* [out] */ Boolean* modified);

    /**
     * Removes all objects from this {@code Collection} that are not also found in the
     * {@code Collection} passed (optional). After this method returns this {@code Collection}
     * will only contain elements that also can be found in the {@code Collection}
     * passed to this method.
     *
     * @param collection
     *            the collection of objects to retain.
     * @return {@code true} if this {@code Collection} is modified, {@code false}
     *         otherwise.
     * @throws UnsupportedOperationException
     *                if removing from this {@code Collection} is not supported.
     * @throws ClassCastException
     *                if one or more elements of {@code collection}
     *                isn't of the correct type.
     * @throws NullPointerException
     *                if {@code collection} contains at least one
     *                {@code null} element and this {@code Collection} doesn't support
     *                {@code null} elements.
     * @throws NullPointerException
     *                if {@code collection} is {@code null}.
     */
    // CARAPI RetainAll(
    //     /* [in] */ ICollection* collection,
    //     /* [out] */ Boolean* modified);

    /**
     * Returns {@code true} if this queue contains the specified element.
     * More formally, returns {@code true} if and only if this queue contains
     * at least one element {@code e} such that {@code o.equals(e)}.
     *
     * @param o object to be checked for containment in this queue
     * @return {@code true} if this queue contains the specified element
     */
    CARAPI Contains(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* result);

    /**
     * Returns an array containing all of the elements in this queue, in
     * proper sequence.
     *
     * <p>The returned array will be "safe" in that no references to it are
     * maintained by this queue.  (In other words, this method must allocate
     * a new array).  The caller is thus free to modify the returned array.
     *
     * <p>This method acts as bridge between array-based and collection-based
     * APIs.
     *
     * @return an array containing all of the elements in this queue
     */
    CARAPI ToArray(
        /* [out, callee] */ ArrayOf<IInterface*>** array);

    /**
     * Returns an array containing all of the elements in this queue, in
     * proper sequence; the runtime type of the returned array is that of
     * the specified array.  If the queue fits in the specified array, it
     * is returned therein.  Otherwise, a new array is allocated with the
     * runtime type of the specified array and the size of this queue.
     *
     * <p>If this queue fits in the specified array with room to spare
     * (i.e., the array has more elements than this queue), the element in
     * the array immediately following the end of the queue is set to
     * {@code null}.
     *
     * <p>Like the {@link #toArray()} method, this method acts as bridge between
     * array-based and collection-based APIs.  Further, this method allows
     * precise control over the runtime type of the output array, and may,
     * under certain circumstances, be used to save allocation costs.
     *
     * <p>Suppose {@code x} is a queue known to contain only strings.
     * The following code can be used to dump the queue into a newly
     * allocated array of {@code String}:
     *
     *  <pre> {@code String[] y = x.toArray(new String[0]);}</pre>
     *
     * Note that {@code toArray(new Object[0])} is identical in function to
     * {@code toArray()}.
     *
     * @param a the array into which the elements of the queue are to
     *          be stored, if it is big enough; otherwise, a new array of the
     *          same runtime type is allocated for this purpose
     * @return an array containing all of the elements in this queue
     * @throws ArrayStoreException if the runtime type of the specified array
     *         is not a supertype of the runtime type of every element in
     *         this queue
     * @throws NullPointerException if the specified array is null
     */
    // @SuppressWarnings("unchecked")
    CARAPI ToArray(
        /* [in] */ ArrayOf<IInterface*>* a,
        /* [out, callee] */ ArrayOf<IInterface*>** outArray);

    CARAPI ToString(
        /* [out] */ String* str);

    /**
     * Attempts to add all of the objects contained in {@code Collection}
     * to the contents of this {@code Collection} (optional). If the passed {@code Collection}
     * is changed during the process of adding elements to this {@code Collection}, the
     * behavior is not defined.
     *
     * @param collection
     *            the {@code Collection} of objects.
     * @return {@code true} if this {@code Collection} is modified, {@code false}
     *         otherwise.
     * @throws UnsupportedOperationException
     *                if adding to this {@code Collection} is not supported.
     * @throws ClassCastException
     *                if the class of an object is inappropriate for this
     *                {@code Collection}.
     * @throws IllegalArgumentException
     *                if an object cannot be added to this {@code Collection}.
     * @throws NullPointerException
     *                if {@code collection} is {@code null}, or if it
     *                contains {@code null} elements and this {@code Collection} does
     *                not support such elements.
     */
    CARAPI AddAll(
        /* [in] */ ICollection* collection,
        /* [out] */ Boolean* modified);

    /**
     * Atomically removes all of the elements from this queue.
     * The queue will be empty after this call returns.
     */
    CARAPI Clear();

    /**
     * @throws UnsupportedOperationException {@inheritDoc}
     * @throws ClassCastException            {@inheritDoc}
     * @throws NullPointerException          {@inheritDoc}
     * @throws IllegalArgumentException      {@inheritDoc}
     */
    CARAPI DrainTo(
        /* [in] */ ICollection* c,
        /* [out] */ Int32* number);

    /**
     * @throws UnsupportedOperationException {@inheritDoc}
     * @throws ClassCastException            {@inheritDoc}
     * @throws NullPointerException          {@inheritDoc}
     * @throws IllegalArgumentException      {@inheritDoc}
     */
    CARAPI DrainTo(
        /* [in] */ ICollection* c,
        /* [in] */ Int32 maxElements,
        /* [out] */ Int32* number);

    /**
     * Returns an iterator over the elements in this queue in proper sequence.
     * The elements will be returned in order from first (head) to last (tail).
     *
     * <p>The returned iterator is a "weakly consistent" iterator that
     * will never throw {@link java.util.ConcurrentModificationException
     * ConcurrentModificationException}, and guarantees to traverse
     * elements as they existed upon construction of the iterator, and
     * may (but is not guaranteed to) reflect any modifications
     * subsequent to construction.
     *
     * @return an iterator over the elements in this queue in proper sequence
     */
    CARAPI GetIterator(
        /* [out] */ IIterator** it);

protected:
    // Internal helper methods

    /**
     * Circularly increment i.
     */
    CARAPI_(Int32) Inc(
        /* [in] */ Int32 i);

    /**
     * Circularly decrement i.
     */
    CARAPI_(Int32) Dec(
        /* [in] */ Int32 i);

    /**
     * Returns item at index i.
     */
    CARAPI_(AutoPtr<IInterface>) ItemAt(
        /* [in] */ Int32 i);

    /**
     * Deletes item at array index removeIndex.
     * Utility for remove(Object) and iterator.remove.
     * Call only when holding lock.
     */
    CARAPI RemoveAt(
        /* [in] */ Int32 removeIndex);

private:
    /**
     * Throws NullPointerException if argument is null.
     *
     * @param v the element
     */
    static CARAPI CheckNotNull(
        /* [in] */ IInterface* v);

    /**
     * Inserts element at current put position, advances, and signals.
     * Call only when holding lock.
     */
    CARAPI Enqueue(
        /* [in] */ IInterface* x);

    /**
     * Extracts element at current take position, advances, and signals.
     * Call only when holding lock.
     */
    CARAPI_(AutoPtr<IInterface>) Dequeue();

public:
    /** The queued items */
    AutoPtr< ArrayOf<IInterface*> > mItems;

    /** items index for next take, poll, peek or remove */
    Int32 mTakeIndex;

    /** items index for next put, offer, or add */
    Int32 mPutIndex;

    /** Number of elements in the queue */
    Int32 mCount;

    /*
     * Concurrency control uses the classic two-condition algorithm
     * found in any textbook.
     */

    /** Main lock guarding all access */
    AutoPtr<IReentrantLock> mLock;

    /**
     * Shared state for currently active iterators, or null if there
     * are known not to be any.  Allows queue operations to update
     * iterator state.
     */
    /* transient */ AutoPtr<Itrs> mItrs;
private:
    /**
     * Serialization ID. This class relies on default serialization
     * even for the items array, which is default-serialized, even if
     * it is empty. Otherwise it could not be declared final, which is
     * necessary here.
     */
    static const Int64 sSerialVersionUID = -817911632652898426L;

    /** Condition for waiting takes */
    AutoPtr<ICondition> mNotEmpty;

    /** Condition for waiting puts */
    AutoPtr<ICondition> mNotFull;
};

} // namespace Concurrent
} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CARRAYBLOCKINGQUEUE_H__
