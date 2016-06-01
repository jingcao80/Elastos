
#ifndef __ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CLINKEDBLOCKINGDEQUE_H__
#define __ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CLINKEDBLOCKINGDEQUE_H__

#include "_Elastos_Utility_Concurrent_CLinkedBlockingDeque.h"
#include "AbstractQueue.h"

using Elastos::IO::ISerializable;
using Elastos::IO::IObjectOutputStream;
using Elastos::IO::IObjectInputStream;
using Elastos::Utility::Concurrent::Locks::IReentrantLock;
using Elastos::Utility::Concurrent::Locks::ICondition;

namespace Elastos {
namespace Utility {
namespace Concurrent {

CarClass(CLinkedBlockingDeque)
    , public AbstractQueue
    , public IBlockingDeque
    , public IBlockingQueue
    , public ISerializable
{
private:
    /** Doubly-linked list node class */
    class Node
        : public Object
    {
    public:
        Node(
            /* [in] */ IInterface* x);

    public:
        /**
         * The item, or null if this node has been removed.
         */
        AutoPtr<IInterface> mItem;

        /**
         * One of:
         * - the real predecessor Node
         * - this Node, meaning the predecessor is tail
         * - null, meaning there is no predecessor
         */
        AutoPtr<Node> mPrev;

        /**
         * One of:
         * - the real successor Node
         * - this Node, meaning the successor is head
         * - null, meaning there is no successor
         */
        AutoPtr<Node> mNext;
    };

    /**
     * Base class for Iterators for LinkedBlockingDeque
     */
    class AbstractItr
        : public IIterator
        , public Object
    {
    public:
        CAR_INTERFACE_DECL()

        AbstractItr(
            /* [in] */ CLinkedBlockingDeque* owner);

        virtual CARAPI_(AutoPtr<Node>) FirstNode() = 0;

        virtual CARAPI_(AutoPtr<Node>) NextNode(
            /* [in] */ Node* n) = 0;

        /**
         * Advances next.
         */
        CARAPI_(void) Advance();

        CARAPI HasNext(
            /* [out] */ Boolean* result);

        CARAPI GetNext(
            /* [out] */ IInterface** object);

        CARAPI Remove();

    private:
        /**
         * Returns the successor node of the given non-null, but
         * possibly previously deleted, node.
         */
        AutoPtr<Node> Succ(
            /* [in] */ Node* n);

    public:
        /**
         * The next node to return in next()
         */
        AutoPtr<Node> mNext;

        /**
         * nextItem holds on to item fields because once we claim that
         * an element exists in hasNext(), we must return item read
         * under lock (in advance()) even if it was in the process of
         * being removed when hasNext() was called.
         */
        AutoPtr<IInterface> mNextItem;

        CLinkedBlockingDeque* mOwner;

    private:
        /**
         * Node returned by most recent call to next. Needed by remove.
         * Reset to null if this element is deleted by a call to remove.
         */
        AutoPtr<Node> mLastRet;
    };

    /** Forward iterator */
    class Itr : public AbstractItr
    {
    public:
        Itr(
            /* [in] */ CLinkedBlockingDeque* owner);

        virtual CARAPI_(AutoPtr<Node>) FirstNode();

        virtual CARAPI_(AutoPtr<Node>) NextNode(
            /* [in] */ Node* n);
    };

    /** Descending iterator */
    class DescendingItr : public AbstractItr
    {
    public:
        DescendingItr(
            /* [in] */ CLinkedBlockingDeque* owner);

        virtual CARAPI_(AutoPtr<Node>) FirstNode();

        virtual CARAPI_(AutoPtr<Node>) NextNode(
            /* [in] */ Node* n);
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /**
     * Creates a {@code LinkedBlockingDeque} with a capacity of
     * {@link Integer#MAX_VALUE}.
     */
    CARAPI constructor();

    /**
     * Creates a {@code LinkedBlockingDeque} with the given (fixed) capacity.
     *
     * @param capacity the capacity of this deque
     * @throws IllegalArgumentException if {@code capacity} is less than 1
     */
    CARAPI constructor(
        /* [in] */ Int32 capacity);
    /**
     * Creates a {@code LinkedBlockingDeque} with a capacity of
     * {@link Integer#MAX_VALUE}, initially containing the elements of
     * the given collection, added in traversal order of the
     * collection's iterator.
     *
     * @param c the collection of elements to initially contain
     * @throws NullPointerException if the specified collection or any
     *         of its elements are null
     */
    CARAPI constructor(
        /* [in] */ ICollection* c);

    /**
     * Unlinks x.
     */
    CARAPI_(void) Unlink(
        /* [in] */ Node* x);

    // BlockingDeque methods

    CARAPI AddFirst(
        /* [in] */ IInterface* e);

    CARAPI AddLast(
        /* [in] */ IInterface* e);

    CARAPI OfferFirst(
        /* [in] */ IInterface* e,
        /* [out] */ Boolean* value);

    CARAPI OfferLast(
        /* [in] */ IInterface* e,
        /* [out] */ Boolean* value);

    CARAPI PutFirst(
        /* [in] */ IInterface* e);

    CARAPI PutLast(
        /* [in] */ IInterface* e);

    CARAPI OfferFirst(
        /* [in] */ IInterface* e,
        /* [in] */ Int64 timeout,
        /* [in] */ ITimeUnit* unit,
        /* [out] */ Boolean* value);

    CARAPI OfferLast(
        /* [in] */ IInterface* e,
        /* [in] */ Int64 timeout,
        /* [in] */ ITimeUnit* unit,
        /* [out] */ Boolean* value);

    CARAPI RemoveFirst(
        /* [out] */ PInterface* res);

    CARAPI RemoveLast(
        /* [out] */ PInterface* res);

    CARAPI PollFirst(
        /* [out] */ PInterface* res);

    CARAPI PollLast(
        /* [out] */ PInterface* res);

    CARAPI TakeFirst(
        /* [out] */ IInterface** outface);

    CARAPI TakeLast(
        /* [out] */ IInterface** outface);

    CARAPI PollFirst(
        /* [in] */ Int64 timeout,
        /* [in] */ ITimeUnit* unit,
        /* [out] */ IInterface** outface);

    CARAPI PollLast(
        /* [in] */ Int64 timeout,
        /* [in] */ ITimeUnit* unit,
        /* [out] */ IInterface** outface);

    CARAPI GetFirst(
        /* [out] */ PInterface* res);

    CARAPI GetLast(
        /* [out] */ PInterface* res);

    CARAPI PeekFirst(
        /* [out] */ PInterface* res);

    CARAPI PeekLast(
        /* [out] */ PInterface* res);

    CARAPI RemoveFirstOccurrence(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* value);

    CARAPI RemoveLastOccurrence(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* value);

    // BlockingQueue methods

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

    CARAPI Remove(
        /* [out] */ PInterface* object);

    CARAPI Poll(
        /* [out] */ IInterface** e);

    CARAPI Take(
        /* [out] */ IInterface** e);

    CARAPI Poll(
        /* [in] */ Int64 timeout,
        /* [in] */ ITimeUnit* unit,
        /* [out] */ IInterface** e);

    CARAPI GetElement(
        /* [out] */ IInterface** e);

    CARAPI Peek(
        /* [out] */ IInterface** e);

    CARAPI RemainingCapacity(
        /* [out] */ Int32* capacity);

    CARAPI DrainTo(
        /* [in] */ ICollection* c,
        /* [out] */ Int32* number);

    CARAPI DrainTo(
        /* [in] */ ICollection* c,
        /* [in] */ Int32 maxElements,
        /* [out] */ Int32* number);

    // Stack methods

    CARAPI Push(
        /* [in] */ IInterface* e);

    CARAPI Pop(
        /* [out] */ IInterface** outface);

    // Collection methods

    CARAPI Remove(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* modified);

    CARAPI GetSize(
        /* [out] */ Int32* size);

    CARAPI Contains(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* result);

    /*
     * TODO: Add support for more efficient bulk operations.
     *
     * We don't want to acquire the lock for every iteration, but we
     * also want other threads a chance to interact with the
     * collection, especially when count is close to capacity.
     */

//     /**
//      * Adds all of the elements in the specified collection to this
//      * queue.  Attempts to addAll of a queue to itself result in
//      * {@code IllegalArgumentException}. Further, the behavior of
//      * this operation is undefined if the specified collection is
//      * modified while the operation is in progress.
//      *
//      * @param c collection containing elements to be added to this queue
//      * @return {@code true} if this queue changed as a result of the call
//      * @throws ClassCastException            {@inheritDoc}
//      * @throws NullPointerException          {@inheritDoc}
//      * @throws IllegalArgumentException      {@inheritDoc}
//      * @throws IllegalStateException         {@inheritDoc}
//      * @see #add(Object)
//      */
//     public boolean addAll(Collection<? extends E> c) {
//         if (c == null)
//             throw new NullPointerException();
//         if (c == this)
//             throw new IllegalArgumentException();
//         final ReentrantLock lock = this.lock;
//         lock.lock();
//         try {
//             boolean modified = false;
//             for (E e : c)
//                 if (linkLast(e))
//                     modified = true;
//             return modified;
//         } finally {
//             lock.unlock();
//         }
//     }


    CARAPI ToArray(
        /* [out, callee] */ ArrayOf<IInterface*>** array);

    CARAPI ToArray(
        /* [in] */ ArrayOf<IInterface*>* inArray,
        /* [out, callee] */ ArrayOf<IInterface*>** outArray);

    String ToString();

    CARAPI Clear();

    CARAPI GetIterator(
        /* [out] */ IIterator** it);

    CARAPI GetDescendingIterator(
        /* [out] */ IIterator** iterator);

    CARAPI Equals(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

protected:
    CARAPI_(String) GetClassName() { return String("CLinkedBlockingDeque"); }

private:
    // Basic linking and unlinking operations, called only while holding lock

    /**
     * Links node as first element, or returns false if full.
     */
    CARAPI_(Boolean) LinkFirst(
        /* [in] */ Node* node);

    /**
     * Links node as last element, or returns false if full.
     */
    CARAPI_(Boolean) LinkLast(
        /* [in] */ Node* node);

    /**
     * Removes and returns first element, or null if empty.
     */
    CARAPI_(AutoPtr<IInterface>) UnlinkFirst();

    /**
     * Removes and returns last element, or null if empty.
     */
    CARAPI_(AutoPtr<IInterface>) UnlinkLast();

    /**
     * Saves the state of this deque to a stream (that is, serializes it).
     *
     * @serialData The capacity (int), followed by elements (each an
     * {@code Object}) in the proper order, followed by a null
     * @param s the stream
     */
    CARAPI_(void) WriteObject(
        /* [in] */ IObjectOutputStream* s);

    /**
     * Reconstitutes this deque from a stream (that is, deserializes it).
     *
     * @param s the stream
     */
    CARAPI_(void) ReadObject(
        /* [in] */ IObjectInputStream* s);

private:
    static Int64 sSerialVersionUID;// = -387911632671998426L;

    /**
     * Pointer to first node.
     * Invariant: (first == null && last == null) ||
     *            (first.prev == null && first.item != null)
     */
    AutoPtr<Node> mFirst;

    /**
     * Pointer to last node.
     * Invariant: (first == null && last == null) ||
     *            (last.next == null && last.item != null)
     */
    AutoPtr<Node> mLast;

    /** Number of items in the deque */
    Int32 mCount;

    /** Maximum number of items in the deque */
    Int32 mCapacity;

    /** Main lock guarding all access */
    AutoPtr<IReentrantLock> mLock;

    /** Condition for waiting takes */
    AutoPtr<ICondition> mNotEmpty;

    /** Condition for waiting puts */
    AutoPtr<ICondition> mNotFull;
};

} // namespace Concurrent
} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CLINKEDBLOCKINGDEQUE_H__
