
#ifndef __ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CLINKEDBLOCKINGQUEUE_H__
#define __ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CLINKEDBLOCKINGQUEUE_H__

#include "_Elastos_Utility_Concurrent_CLinkedBlockingQueue.h"
//#include "Condition.h"
#include "AbstractQueue.h"

using Elastos::IO::ISerializable;
//using Elastos::Core::Condition;
using Elastos::Utility::IIterator;
using Elastos::Utility::Concurrent::Atomic::IAtomicInteger32;
using Elastos::Utility::Concurrent::Locks::IReentrantLock;

namespace Elastos {
namespace Utility {
namespace Concurrent {

CarClass(CLinkedBlockingQueue)
    , public AbstractQueue
    , public IBlockingQueue
    , public ISerializable
{
public:
    /**
     * Linked list node class
     */
    class Node
        : public Object
    {
    public:
        Node(
            /* [in] */ IInterface* x)
            : mItem(x)
        {}

    public:
        AutoPtr<IInterface> mItem;

        /**
         * One of:
         * - the real successor Node
         * - this Node, meaning the successor is head.next
         * - null, meaning there is no successor (this is the last node)
         */
        AutoPtr<Node> mNext;
    };

private:
    class Itr
        : public Object
        , public IIterator
    {
    public:
        CAR_INTERFACE_DECL();

        Itr(
            /* [in] */ CLinkedBlockingQueue* owner);

        CARAPI HasNext(
            /* [out] */ Boolean* result);

        CARAPI GetNext(
            /* [out] */ IInterface** object);

        CARAPI Remove();

    private:
        /**
         * Returns the next live successor of p, or null if no such.
         *
         * Unlike other traversal methods, iterators need to handle both:
         * - dequeued nodes (p.next == p)
         * - (possibly multiple) interior removed nodes (p.item == null)
         */
        CARAPI_(AutoPtr<Node>) NextNode(
            /* [in] */ Node* p);

    private:
        /*
         * Basic weakly-consistent iterator.  At all times hold the next
         * item to hand out so that if hasNext() reports true, we will
         * still have it to return even if lost race with a take etc.
         */
        AutoPtr<Node> mCurrent;
        AutoPtr<Node> mLastRet;
        AutoPtr<IInterface> mCurrentElement;
        AutoPtr<CLinkedBlockingQueue> mOwner;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CLinkedBlockingQueue();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 capacity);

    CARAPI constructor(
        /* [in] */ ICollection* c);

    /**
     * Lock to prevent both puts and takes.
     */
    CARAPI_(void) FullyLock();

    /**
     * Unlock to allow both puts and takes.
     */
    CARAPI_(void) FullyUnlock();

    CARAPI GetSize(
        /* [out] */ Int32* size);

    CARAPI RemainingCapacity(
        /* [out] */ Int32* capacity);

    CARAPI Put(
        /* [in] */ IInterface* e);

    CARAPI Offer(
        /* [in] */ IInterface* e,
        /* [in] */ Int64 timeout,
        /* [in] */ ITimeUnit* unit,
        /* [out] */ Boolean* result);

    CARAPI Offer(
        /* [in] */ IInterface* e,
        /* [out] */ Boolean* result);

    CARAPI Take(
        /* [out] */ IInterface** e);

    CARAPI Poll(
        /* [in] */ Int64 timeout,
        /* [in] */ ITimeUnit* unit,
        /* [out] */ IInterface** e);

    CARAPI Poll(
        /* [out] */ IInterface** e);

    CARAPI Peek(
        /* [out] */ IInterface** e);

    /**
     * Unlinks interior Node p with predecessor trail.
     */
    CARAPI_(void) Unlink(
        /* [in] */ Node* p,
        /* [in] */ Node* trail);

    CARAPI Remove(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* modified);

    CARAPI Contains(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* result);

    CARAPI ToArray(
        /* [out, callee] */ ArrayOf<IInterface*>** array);

    CARAPI ToArray(
        /* [in] */ ArrayOf<IInterface*>* inArray,
        /* [out, callee] */ ArrayOf<IInterface*>** outArray);

    CARAPI Clear();

    CARAPI DrainTo(
        /* [in] */ ICollection* c,
        /* [out] */ Int32* number);

    CARAPI DrainTo(
        /* [in] */ ICollection* c,
        /* [in] */ Int32 maxElements,
        /* [out] */ Int32* number);

    CARAPI GetIterator(
        /* [out] */ IIterator** it);

    CARAPI Add(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* modified);

    CARAPI Remove(
        /* [out] */ IInterface** e);

    CARAPI GetElement(
        /* [out] */ IInterface** e);

    CARAPI AddAll(
        /* [in] */ ICollection* collection,
        /* [out] */ Boolean* modified);

    CARAPI ContainsAll(
        /* [in] */ ICollection* collection,
        /* [out] */ Boolean* result);

    CARAPI IsEmpty(
        /* [out] */ Boolean* result);

    CARAPI RemoveAll(
        /* [in] */ ICollection* collection,
        /* [out] */ Boolean* modified);

    CARAPI RetainAll(
        /* [in] */ ICollection* collection,
        /* [out] */ Boolean* modified);

    CARAPI Equals(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

private:
    /**
     * Signals a waiting take. Called only from put/offer (which do not
     * otherwise ordinarily lock takeLock.)
     */
    CARAPI_(void) SignalNotEmpty();

    /**
     * Signals a waiting put. Called only from take/poll.
     */
    CARAPI_(void) SignalNotFull();

    /**
     * Links node at end of queue.
     *
     * @param node the node
     */
    CARAPI_(void) Enqueue(
        /* [in] */ Node* node);

    /**
     * Removes a node from head of queue.
     *
     * @return the node
     */
    CARAPI_(AutoPtr<IInterface>) Dequeue();

private:
    /** The capacity bound, or Integer.MAX_VALUE if none */
    Int32 mCapacity;

    /** Current number of elements */
    AutoPtr<IAtomicInteger32> mCount;

    /**
     * Head of linked list.
     * Invariant: head.item == null
     */
    AutoPtr<Node> mHead;

    /**
     * Tail of linked list.
     * Invariant: last.next == null
     */
    AutoPtr<Node> mLast;

    /** Lock held by take, poll, etc */
    AutoPtr<IReentrantLock> mTakeLock;
    /** Wait queue for waiting takes */
//    Condition mNotEmpty;

    /** Lock held by put, offer, etc */
    AutoPtr<IReentrantLock> mPutLock;

    /** Wait queue for waiting puts */
//    AutoPtr<Condition> mNotFull;
};

} // namespace Concurrent
} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CLINKEDBLOCKINGQUEUE_H__
