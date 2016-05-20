
#ifndef __ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CLINKEDTRANSFERQUEUE_H__
#define __ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CLINKEDTRANSFERQUEUE_H__

#include "_Elastos_Utility_Concurrent_CLinkedTransferQueue.h"
#include "AbstractQueue.h"

using Elastos::IO::IObjectOutputStream;
using Elastos::IO::IObjectInputStream;
using Elastos::Core::IThread;

namespace Elastos {
namespace Utility {
namespace Concurrent {

CarClass(CLinkedTransferQueue)
    , public AbstractQueue
    , public ITransferQueue
    , public IBlockingQueue
{
public:
    /**
     * Queue nodes. Uses Object, not E, for items to allow forgetting
     * them after use.  Relies heavily on Unsafe mechanics to minimize
     * unnecessary ordering constraints: Writes that are intrinsically
     * ordered wrt other accesses or CASes use simple relaxed forms.
     */
    class Node
        : public Object
    {
    public:
        // CAS methods for fields
        CARAPI_(Boolean) CasNext(
            /* [in] */ Node* cmp,
            /* [in] */ Node* val);

        CARAPI_(Boolean) CasItem(
            /* [in] */ IInterface* cmp,
            /* [in] */ IInterface* val);

        /**
         * Constructs a new node.  Uses relaxed write because item can
         * only be seen after publication via casNext.
         */
        Node(
            /* [in] */ IInterface* item,
            /* [in] */ Boolean isData);

        /**
         * Links node to itself to avoid garbage retention.  Called
         * only after CASing head field, so uses relaxed write.
         */
        CARAPI_(void) ForgetNext();

        /**
         * Sets item to self and waiter to null, to avoid garbage
         * retention after matching or cancelling. Uses relaxed writes
         * because order is already constrained in the only calling
         * contexts: item is forgotten only after volatile/atomic
         * mechanics that extract items.  Similarly, clearing waiter
         * follows either CAS or return from park (if ever parked;
         * else we don't care).
         */
        CARAPI_(void) ForgetContents();

        /**
         * Returns true if this node has been matched, including the
         * case of artificial matches due to cancellation.
         */
        CARAPI_(Boolean) IsMatched();

        /**
         * Returns true if this is an unmatched request node.
         */
        CARAPI_(Boolean) IsUnmatchedRequest();

        /**
         * Returns true if a node with the given mode cannot be
         * appended to this node because this node is unmatched and
         * has opposite data mode.
         */
        CARAPI_(Boolean) CannotPrecede(
            /* [in] */ Boolean haveData);

        /**
         * Tries to artificially match a data node -- used by remove.
         */
        CARAPI_(Boolean) TryMatchData();

    public:
        Boolean mIsData;   // false if this is a request node
        AutoPtr<IInterface> mItem;   // initially non-null if isData; CASed to match
        AutoPtr<Node> mNext;
        AutoPtr<IThread> mWaiter; // null until waiting

    private:
        static Int64 sSerialVersionUID;

        // // Unsafe mechanics
        // private static final sun.misc.Unsafe UNSAFE;
        // private static final long itemOffset;
        // private static final long nextOffset;
        // private static final long waiterOffset;
        // static {
        //     try {
        //         UNSAFE = sun.misc.Unsafe.getUnsafe();
        //         Class<?> k = Node.class;
        //         itemOffset = UNSAFE.objectFieldOffset
        //             (k.getDeclaredField("item"));
        //         nextOffset = UNSAFE.objectFieldOffset
        //             (k.getDeclaredField("next"));
        //         waiterOffset = UNSAFE.objectFieldOffset
        //             (k.getDeclaredField("waiter"));
        //     } catch (Exception e) {
        //         throw new Error(e);
        //     }
        // }
    };

    class Itr
        : public IIterator
        , public Object
    {
    public:
        CAR_INTERFACE_DECL()

        Itr(
            /* [in] */ CLinkedTransferQueue* owner);

        CARAPI HasNext(
            /* [out] */ Boolean* result);

        CARAPI GetNext(
            /* [out] */ IInterface** object);

        CARAPI Remove();

    private:
        /**
         * Moves to next node after prev, or first node if prev null.
         */
        CARAPI_(void) Advance(
            /* [in] */ Node* prev);

    private:
        AutoPtr<Node> mNextNode;   // next node to return item for
        AutoPtr<IInterface> mNextItem;      // the corresponding item
        AutoPtr<Node> mLastRet;    // last returned node, to support remove
        AutoPtr<Node> mLastPred;   // predecessor to unlink lastRet
        CLinkedTransferQueue* mOwner;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

//    static <E> E cast(Object item);

    /* -------------- Traversal methods -------------- */

    /**
     * Returns the successor of p, or the head node if p.next has been
     * linked to self, which will only be true if traversing with a
     * stale pointer that is now off the list.
     */
    CARAPI_(AutoPtr<Node>) Succ(
        /* [in] */ Node* p);

    /* -------------- Removal methods -------------- */

    /**
     * Unsplices (now or later) the given deleted/cancelled node with
     * the given predecessor.
     *
     * @param pred a node that was at one time known to be the
     * predecessor of s, or null or s itself if s is/was at head
     * @param s the node to be unspliced
     */
    CARAPI_(void) Unsplice(
        /* [in] */ Node* pred,
        /* [in] */ Node* s);

    /**
     * Creates an initially empty {@code LinkedTransferQueue}.
     */
    CARAPI constructor();

    /**
     * Creates a {@code LinkedTransferQueue}
     * initially containing the elements of the given collection,
     * added in traversal order of the collection's iterator.
     *
     * @param c the collection of elements to initially contain
     * @throws NullPointerException if the specified collection or any
     *         of its elements are null
     */
    CARAPI constructor(
        /* [in] */ ICollection* c);

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

    CARAPI Add(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* modified);

    CARAPI TryTransfer(
        /* [in] */ IInterface* e,
        /* [out] */ Boolean* value);

    CARAPI Transfer(
        /* [in] */ IInterface* e);

    CARAPI TryTransfer(
        /* [in] */ IInterface* e,
        /* [in] */ Int64 timeout,
        /* [in] */ ITimeUnit* unit,
        /* [out] */ Boolean* value);

    CARAPI Take(
        /* [out] */ IInterface** res);

    CARAPI Poll(
        /* [in] */ Int64 timeout,
        /* [in] */ ITimeUnit* unit,
        /* [out] */ IInterface** e);

    CARAPI Poll(
        /* [out] */ IInterface** e);

    CARAPI DrainTo(
        /* [in] */ ICollection* c,
        /* [out] */ Int32* number);

    CARAPI DrainTo(
        /* [in] */ ICollection* c,
        /* [in] */ Int32 maxElements,
        /* [out] */ Int32* number);

    CARAPI GetIterator(
        /* [out] */ IIterator** it);

    CARAPI Peek(
        /* [out] */ IInterface** e);

    CARAPI IsEmpty(
        /* [out] */ Boolean* result);

    CARAPI HasWaitingConsumer(
        /* [out] */ Boolean* value);

    CARAPI GetSize(
        /* [out] */ Int32* size);

    CARAPI GetWaitingConsumerCount(
        /* [out] */ Int32* value);

    CARAPI Remove(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* modified);

    CARAPI Contains(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* result);

    CARAPI RemainingCapacity(
        /* [out] */ Int32* capacity);

    CARAPI AddAll(
        /* [in] */ ICollection* collection,
        /* [out] */ Boolean* modified);

    CARAPI Clear();

    CARAPI ContainsAll(
        /* [in] */ ICollection* collection,
        /* [out] */ Boolean* result);

    CARAPI RemoveAll(
        /* [in] */ ICollection* collection,
        /* [out] */ Boolean* modified);

    CARAPI RetainAll(
        /* [in] */ ICollection* collection,
        /* [out] */ Boolean* modified);

    CARAPI ToArray(
        /* [out, callee] */ ArrayOf<IInterface*>** array);

    CARAPI ToArray(
        /* [in] */ ArrayOf<IInterface*>* inArray,
        /* [out, callee] */ ArrayOf<IInterface*>** outArray);

    CARAPI Remove(
        /* [out] */ IInterface** e);

    CARAPI GetElement(
        /* [out] */ IInterface** e);

private:
    // CAS methods for fields
    CARAPI_(Boolean) CasTail(
        /* [in] */ Node* cmp,
        /* [in] */ Node* val);

    CARAPI_(Boolean) CasHead(
        /* [in] */ Node* cmp,
        /* [in] */ Node* val);

    CARAPI_(Boolean) CasSweepVotes(
        /* [in] */ Int32 cmp,
        /* [in] */ Int32 val);

        /**
     * Implements all queuing methods. See above for explanation.
     *
     * @param e the item or null for take
     * @param haveData true if this is a put, else a take
     * @param how NOW, ASYNC, SYNC, or TIMED
     * @param nanos timeout in nanosecs, used only if mode is TIMED
     * @return an item if matched, else e
     * @throws NullPointerException if haveData mode but e is null
     */
    CARAPI_(AutoPtr<IInterface>) Xfer(
        /* [in] */ IInterface* e,
        /* [in] */ Boolean haveData,
        /* [in] */ Int32 how,
        /* [in] */ Int64 nanos);

    /**
     * Tries to append node s as tail.
     *
     * @param s the node to append
     * @param haveData true if appending in data mode
     * @return null on failure due to losing race with append in
     * different mode, else s's predecessor, or s itself if no
     * predecessor
     */
    CARAPI_(AutoPtr<Node>) TryAppend(
        /* [in] */ Node* s,
        /* [in] */ Boolean haveData);

    /**
     * Spins/yields/blocks until node s is matched or caller gives up.
     *
     * @param s the waiting node
     * @param pred the predecessor of s, or s itself if it has no
     * predecessor, or null if unknown (the null case does not occur
     * in any current calls but may in possible future extensions)
     * @param e the comparison value for checking match
     * @param timed if true, wait only until timeout elapses
     * @param nanos timeout in nanosecs, used only if timed is true
     * @return matched item, or e if unmatched on interrupt or timeout
     */
    CARAPI_(AutoPtr<IInterface>) AwaitMatch(
        /* [in] */ Node* s,
        /* [in] */ Node* pred,
        /* [in] */ IInterface* e,
        /* [in] */ Boolean timed,
        /* [in] */ Int64 nanos);

    /**
     * Returns spin/yield value for a node with given predecessor and
     * data mode. See above for explanation.
     */
    static CARAPI_(Int32) SpinsFor(
        /* [in] */ Node* pred,
        /* [in] */ Boolean haveData);

    /**
     * Returns the first unmatched node of the given mode, or null if
     * none.  Used by methods isEmpty, hasWaitingConsumer.
     */
    CARAPI_(AutoPtr<Node>) FirstOfMode(
        /* [in] */ Boolean isData);

    /**
     * Returns the item in the first unmatched node with isData; or
     * null if none.  Used by peek.
     */
    CARAPI_(AutoPtr<IInterface>) FirstDataItem();

    /**
     * Traverses and counts unmatched nodes of the given mode.
     * Used by methods size and getWaitingConsumerCount.
     */
    CARAPI_(Int32) CountOfMode(
        /* [in] */ Boolean data);

    /**
     * Unlinks matched (typically cancelled) nodes encountered in a
     * traversal from head.
     */
    CARAPI_(void) Sweep();

    /**
     * Main implementation of remove(Object)
     */
    CARAPI_(Boolean) FindAndRemove(
        /* [in] */ IInterface* e);

    /**
     * Saves the state to a stream (that is, serializes it).
     *
     * @serialData All of the elements (each an {@code E}) in
     * the proper order, followed by a null
     * @param s the stream
     */
    CARAPI_(void) WriteObject(
        /* [in] */ IObjectOutputStream* s);

    /**
     * Reconstitutes the Queue instance from a stream (that is,
     * deserializes it).
     *
     * @param s the stream
     */
    CARAPI_(void) ReadObject(
        /* [in] */ IObjectInputStream* s);

public:
     /**
     * The maximum number of estimated removal failures (sweepVotes)
     * to tolerate before sweeping through the queue unlinking
     * cancelled nodes that were not unlinked upon initial
     * removal. See above for explanation. The value must be at least
     * two to avoid useless sweeps when removing trailing nodes.
     */
    static Int32 SWEEP_THRESHOLD;

    /** head of the queue; null until first enqueue */
    AutoPtr<Node> mHead;

private:
    static Int64 sSerialVersionUID;

    /** True if on multiprocessor */
    static Boolean MP;

    /**
     * The number of times to spin (with randomly interspersed calls
     * to Thread.yield) on multiprocessor before blocking when a node
     * is apparently the first waiter in the queue.  See above for
     * explanation. Must be a power of two. The value is empirically
     * derived -- it works pretty well across a variety of processors,
     * numbers of CPUs, and OSes.
     */
    static Int32 FRONT_SPINS;

    /**
     * The number of times to spin before blocking when a node is
     * preceded by another node that is apparently spinning.  Also
     * serves as an increment to FRONT_SPINS on phase changes, and as
     * base average frequency for yielding during spins. Must be a
     * power of two.
     */
    static Int32 CHAINED_SPINS;

    /** tail of the queue; null until first append */
    AutoPtr<Node> mTail;

    /** The number of apparent failures to unsplice removed nodes */
    Int32 mSweepVotes;

    /*
     * Possible values for "how" argument in xfer method.
     */
    static Int32 NOW; // for untimed poll, tryTransfer
    static Int32 ASYNC; // for offer, put, add
    static Int32 SYNC; // for transfer, take
    static Int32 TIMED; // for timed poll, tryTransfer

    // // Unsafe mechanics

    // private static final sun.misc.Unsafe UNSAFE;
    // private static final long headOffset;
    // private static final long tailOffset;
    // private static final long sweepVotesOffset;
    // static {
    //     try {
    //         UNSAFE = sun.misc.Unsafe.getUnsafe();
    //         Class<?> k = LinkedTransferQueue.class;
    //         headOffset = UNSAFE.objectFieldOffset
    //             (k.getDeclaredField("head"));
    //         tailOffset = UNSAFE.objectFieldOffset
    //             (k.getDeclaredField("tail"));
    //         sweepVotesOffset = UNSAFE.objectFieldOffset
    //             (k.getDeclaredField("sweepVotes"));
    //     } catch (Exception e) {
    //         throw new Error(e);
    //     }
    // }
};

} // namespace Concurrent
} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CLINKEDTRANSFERQUEUE_H__
