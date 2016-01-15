
#ifndef __ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CPRIORITYBLOCKINGQUEUE_H__
#define __ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CPRIORITYBLOCKINGQUEUE_H__

#include "_Elastos_Utility_Concurrent_CPriorityBlockingQueue.h"
#include "AbstractQueue.h"

using Elastos::IO::ISerializable;
using Elastos::IO::IObjectOutputStream;
using Elastos::IO::IObjectInputStream;
using Elastos::Core::IComparator;
using Elastos::Utility::Concurrent::Locks::IReentrantLock;
using Elastos::Utility::Concurrent::Locks::ICondition;

namespace Elastos {
namespace Utility {
namespace Concurrent {

CarClass(CPriorityBlockingQueue)
    , public AbstractQueue
    , public IBlockingQueue
    , public ISerializable
{
public:
    /**
     * Snapshot iterator that works off copy of underlying q array.
     */
    class Itr
        : public Object
        , public IIterator
    {
    public:
        CAR_INTERFACE_DECL()

        Itr(
            /* [in] */ ArrayOf<IInterface*>* array,
            /* [in] */ CPriorityBlockingQueue* owner);

        CARAPI HasNext(
            /* [out] */ Boolean* result);

        CARAPI GetNext(
            /* [out] */ IInterface** object);

        CARAPI Remove();

    public:
        AutoPtr<ArrayOf<IInterface*> > mArray; // Array of all elements
        Int32 mCursor;           // index of next element to return
        Int32 mLastRet;          // index of last element, or -1 if no such
        CPriorityBlockingQueue* mOwner;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /**
     * Creates a {@code PriorityBlockingQueue} with the default
     * initial capacity (11) that orders its elements according to
     * their {@linkplain Comparable natural ordering}.
     */
    CARAPI constructor();

    /**
     * Creates a {@code PriorityBlockingQueue} with the specified
     * initial capacity that orders its elements according to their
     * {@linkplain Comparable natural ordering}.
     *
     * @param initialCapacity the initial capacity for this priority queue
     * @throws IllegalArgumentException if {@code initialCapacity} is less
     *         than 1
     */
    CARAPI constructor(
        /* [in] */ Int32 initialCapacity);

    /**
     * Creates a {@code PriorityBlockingQueue} with the specified initial
     * capacity that orders its elements according to the specified
     * comparator.
     *
     * @param initialCapacity the initial capacity for this priority queue
     * @param  comparator the comparator that will be used to order this
     *         priority queue.  If {@code null}, the {@linkplain Comparable
     *         natural ordering} of the elements will be used.
     * @throws IllegalArgumentException if {@code initialCapacity} is less
     *         than 1
     */
    CARAPI constructor(
        /* [in] */ Int32 initialCapacity,
        /* [in] */ IComparator* comparator);

    /**
     * Creates a {@code PriorityBlockingQueue} containing the elements
     * in the specified collection.  If the specified collection is a
     * {@link SortedSet} or a {@link PriorityQueue},  this
     * priority queue will be ordered according to the same ordering.
     * Otherwise, this priority queue will be ordered according to the
     * {@linkplain Comparable natural ordering} of its elements.
     *
     * @param  c the collection whose elements are to be placed
     *         into this priority queue
     * @throws ClassCastException if elements of the specified collection
     *         cannot be compared to one another according to the priority
     *         queue's ordering
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
        /* [out] */ IInterface** res);

    CARAPI Poll(
        /* [in] */ Int64 timeout,
        /* [in] */ ITimeUnit* unit,
        /* [out] */ IInterface** e);

    CARAPI Peek(
        /* [out] */ IInterface** e);

    CARAPI GetComparator(
        /* [out] */ IComparator** outcom);

    CARAPI GetSize(
        /* [out] */ Int32* size);

    CARAPI RemainingCapacity(
        /* [out] */ Int32* capacity);

    CARAPI Remove(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* modified);

    CARAPI Contains(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* result);

    CARAPI ToArray(
        /* [out, callee] */ ArrayOf<IInterface*>** array);

    CARAPI ToString(
        /* [out] */ String* res);

    CARAPI DrainTo(
        /* [in] */ ICollection* c,
        /* [out] */ Int32* number);

    CARAPI DrainTo(
        /* [in] */ ICollection* c,
        /* [in] */ Int32 maxElements,
        /* [out] */ Int32* number);

    CARAPI Clear();

    CARAPI ToArray(
        /* [in] */ ArrayOf<IInterface*>* inArray,
        /* [out, callee] */ ArrayOf<IInterface*>** outArray);

    CARAPI GetIterator(
        /* [out] */ IIterator** it);

    /**
     * Identity-based version for use in Itr.remove
     */
    CARAPI_(void) RemoveEQ(
        /* [in] */ IInterface* o);

private:

    /**
     * Tries to grow array to accommodate at least one more element
     * (but normally expand by about 50%), giving up (allowing retry)
     * on contention (which we expect to be rare). Call only while
     * holding lock.
     *
     * @param array the heap array
     * @param oldCap the length of the array
     */
    CARAPI_(void) TryGrow(
        /* [in] */ ArrayOf<IInterface*>* array,
        /* [in] */ Int32 oldCap);

    /**
     * Mechanics for poll().  Call only while holding lock.
     */
    CARAPI_(AutoPtr<IInterface>) Dequeue();

    /**
     * Inserts item x at position k, maintaining heap invariant by
     * promoting x up the tree until it is greater than or equal to
     * its parent, or is the root.
     *
     * To simplify and speed up coercions and comparisons. the
     * Comparable and Comparator versions are separated into different
     * methods that are otherwise identical. (Similarly for siftDown.)
     * These methods are static, with heap state as arguments, to
     * simplify use in light of possible comparator exceptions.
     *
     * @param k the position to fill
     * @param x the item to insert
     * @param array the heap array
     */
    static CARAPI_(void) SiftUpComparable(
        /* [in] */ Int32 k,
        /* [in] */ IInterface* x,
        /* [in] */ ArrayOf<IInterface*>* array);

    static CARAPI_(void) SiftUpUsingComparator(
        /* [in] */ Int32 k,
        /* [in] */ IInterface* x,
        /* [in] */ ArrayOf<IInterface*>* array,
        /* [in] */ IComparator* cmp);

    /**
     * Inserts item x at position k, maintaining heap invariant by
     * demoting x down the tree repeatedly until it is less than or
     * equal to its children or is a leaf.
     *
     * @param k the position to fill
     * @param x the item to insert
     * @param array the heap array
     * @param n heap size
     */
    static CARAPI_(void) SiftDownComparable(
        /* [in] */ Int32 k,
        /* [in] */ IInterface* x,
        /* [in] */ ArrayOf<IInterface*>* array,
        /* [in] */ Int32 n);

    static CARAPI_(void) SiftDownUsingComparator(
        /* [in] */ Int32 k,
        /* [in] */ IInterface* x,
        /* [in] */ ArrayOf<IInterface*>* array,
        /* [in] */ Int32 n,
        /* [in] */ IComparator* cmp);

    /**
     * Establishes the heap invariant (described above) in the entire tree,
     * assuming nothing about the order of the elements prior to the call.
     */
    CARAPI_(void) Heapify();

    CARAPI_(Int32) IndexOf(
        /* [in] */ IInterface* o);

    /**
     * Removes the ith element from queue.
     */
    CARAPI_(void) RemoveAt(
        /* [in] */ Int32 i);

    /**
     * Saves this queue to a stream (that is, serializes it).
     *
     * For compatibility with previous version of this class, elements
     * are first copied to a java.util.PriorityQueue, which is then
     * serialized.
     */
    CARAPI_(void) WriteObject(
        /* [in] */ IObjectOutputStream* s);

    /**
     * Reconstitutes this queue from a stream (that is, deserializes it).
     */
    CARAPI_(void) ReadObject(
        /* [in] */ IObjectInputStream* s);

private:
    static Int64 mSerialVersionUID;

    /*
     * The implementation uses an array-based binary heap, with public
     * operations protected with a single lock. However, allocation
     * during resizing uses a simple spinlock (used only while not
     * holding main lock) in order to allow takes to operate
     * concurrently with allocation.  This avoids repeated
     * postponement of waiting consumers and consequent element
     * build-up. The need to back away from lock during allocation
     * makes it impossible to simply wrap delegated
     * java.util.PriorityQueue operations within a lock, as was done
     * in a previous version of this class. To maintain
     * interoperability, a plain PriorityQueue is still used during
     * serialization, which maintains compatibility at the expense of
     * transiently doubling overhead.
     */

    /**
     * Default array capacity.
     */
    static Int32 DEFAULT_INITIAL_CAPACITY;

    /**
     * The maximum size of array to allocate.
     * Some VMs reserve some header words in an array.
     * Attempts to allocate larger arrays may result in
     * OutOfMemoryError: Requested array size exceeds VM limit
     */
    static Int32 MAX_ARRAY_SIZE;

    /**
     * Priority queue represented as a balanced binary heap: the two
     * children of queue[n] are queue[2*n+1] and queue[2*(n+1)].  The
     * priority queue is ordered by comparator, or by the elements'
     * natural ordering, if comparator is null: For each node n in the
     * heap and each descendant d of n, n <= d.  The element with the
     * lowest value is in queue[0], assuming the queue is nonempty.
     */
    AutoPtr<ArrayOf<IInterface*> > mQueue;

    /**
     * The number of elements in the priority queue.
     */
    Int32 mSize;

    /**
     * The comparator, or null if priority queue uses elements'
     * natural ordering.
     */
    AutoPtr<IComparator> mComparator;

    /**
     * Lock used for all public operations
     */
    AutoPtr<IReentrantLock> mLock;

    /**
     * Condition for blocking when empty
     */
    AutoPtr<ICondition> mNotEmpty;

    /**
     * Spinlock for allocation, acquired via CAS.
     */
    volatile Int32 mAllocationSpinLock;

    /**
     * A plain PriorityQueue used only for serialization,
     * to maintain compatibility with previous versions
     * of this class. Non-null only during serialization/deserialization.
     */
    AutoPtr<IPriorityQueue> mQ;

    // // Unsafe mechanics
    // private static final sun.misc.Unsafe UNSAFE;
    // private static final long allocationSpinLockOffset;
    // static {
    //     try {
    //         UNSAFE = sun.misc.Unsafe.getUnsafe();
    //         Class<?> k = PriorityBlockingQueue.class;
    //         allocationSpinLockOffset = UNSAFE.objectFieldOffset
    //             (k.getDeclaredField("allocationSpinLock"));
    //     } catch (Exception e) {
    //         throw new Error(e);
    //     }
    // }
};

} // namespace Concurrent
} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CPRIORITYBLOCKINGQUEUE_H__
