
#ifndef __ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CCONCURRENTLINKEDDEQUE_H__
#define __ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CCONCURRENTLINKEDDEQUE_H__

#include "_Elastos_Utility_Concurrent_CConcurrentLinkedDeque.h"
#include "AbstractCollection.h"
#include "Object.h"

using Elastos::IO::IObjectOutputStream;
using Elastos::IO::IObjectInputStream;
using Elastos::IO::ISerializable;

namespace Elastos {
namespace Utility {
namespace Concurrent {

CarClass(CConcurrentLinkedDeque)
    , public AbstractCollection
    , public IDeque
    , public IQueue
    , public ISerializable
{
protected:
    class Node
        : public Object
    {
    public:
        Node();

        Node(
            /* [in] */ IInterface* item);

        CARAPI_(Boolean) CasItem(
            /* [in] */ IInterface* cmp,
            /* [in] */ IInterface* val);

        CARAPI_(void) LazySetNext(
            /* [in] */ Node* val);

        CARAPI_(Boolean) CasNext(
            /* [in] */ Node* cmp,
            /* [in] */ Node* val);

        CARAPI_(void) LazySetPrev(
            /* [in] */ Node* val);

        CARAPI_(Boolean) CasPrev(
            /* [in] */ Node* cmp,
            /* [in] */ Node* val);

    public:
        AutoPtr<Node> mPrev;
        AutoPtr<IInterface> mItem;
        AutoPtr<Node> mNext;

    private:
        // Unsafe mechanics
//        static sun.misc.Unsafe UNSAFE;
        static Int64 sPrevOffset;
        static Int64 sItemOffset;
        static Int64 sNextOffset;
    };

private:
    class AbstractItr
        : public Object
        , public IIterator
    {
     public:
        CAR_INTERFACE_DECL();

        AbstractItr(
            /* [in] */ CConcurrentLinkedDeque* owner);

        virtual CARAPI_(AutoPtr<Node>) StartNode() = 0;

        virtual CARAPI_(AutoPtr<Node>) NextNode(
            /* [in] */ Node* p) = 0;

        CARAPI HasNext(
            /* [out] */ Boolean* result);

        CARAPI GetNext(
            /* [out] */ IInterface** object);

        CARAPI Remove();

    private:
        /**
         * Sets nextNode and nextItem to next valid node, or to null
         * if no such.
         */
        CARAPI_(void) Advance();

    public:
        CConcurrentLinkedDeque* mOwner;

    private:
        /**
         * Next node to return item for.
         */
        AutoPtr<Node> mNextNode;

        /**
         * nextItem holds on to item fields because once we claim
         * that an element exists in hasNext(), we must return it in
         * the following next() call even if it was in the process of
         * being removed when hasNext() was called.
         */
        AutoPtr<IInterface> mNextItem;

        /**
         * Node returned by most recent call to next. Needed by remove.
         * Reset to null if this element is deleted by a call to remove.
         */
        AutoPtr<Node> mLastRet;
    };

    /** Forward iterator */
    class Itr
        : public AbstractItr
    {
    public:
        Itr(
            /* [in] */ CConcurrentLinkedDeque* owner)
             : AbstractItr(owner)
        {}

        CARAPI_(AutoPtr<Node>) StartNode()
        {
            return mOwner->First();
        }

        CARAPI_(AutoPtr<Node>) NextNode(
            /* [in] */ Node* p)
        {
            return mOwner->Succ(p);
        }

        CARAPI GetNext(
            /* [out] */ IInterface** object)
        {
            return AbstractItr::GetNext(object);
        }
    };

    /** Descending iterator */
    class DescendingItr
        : public AbstractItr
    {
    public:
        DescendingItr(
            /* [in] */ CConcurrentLinkedDeque* owner)
             : AbstractItr(owner)
        {}

        CARAPI_(AutoPtr<Node>) StartNode()
        {
            return mOwner->Last();
        }

        CARAPI_(AutoPtr<Node>) NextNode(
            /* [in] */ Node* p)
        {
            return mOwner->Pred(p);
        }
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ ICollection* c);

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

    CARAPI PeekFirst(
        /* [out] */ IInterface** e);

    CARAPI PeekLast(
        /* [out] */ IInterface** e);

    CARAPI GetFirst(
        /* [out] */ IInterface** e);

    CARAPI GetLast(
        /* [out] */ IInterface** e);

    CARAPI PollFirst(
        /* [out] */ IInterface** e);

    CARAPI PollLast(
        /* [out] */ IInterface** e);

    CARAPI RemoveFirst(
        /* [out] */ IInterface** e);

    CARAPI RemoveLast(
        /* [out] */ IInterface** e);

    CARAPI Offer(
        /* [in] */ IInterface* e,
        /* [out] */ Boolean* result);

    CARAPI Add(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* modified);

    CARAPI Poll(
        /* [out] */ IInterface** e);

    CARAPI Remove(
        /* [out] */ IInterface** e);

    CARAPI Peek(
        /* [out] */ IInterface** e);

    CARAPI GetElement(
        /* [out] */ IInterface** e);

    CARAPI Push(
        /* [in] */ IInterface* e);

    CARAPI Pop(
        /* [out] */ IInterface** e);

    CARAPI RemoveFirstOccurrence(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* value);

    CARAPI RemoveLastOccurrence(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* value);

    CARAPI Contains(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* result);

    CARAPI IsEmpty(
        /* [out] */ Boolean* result);

    CARAPI GetSize(
        /* [out] */ Int32* size);

    CARAPI Remove(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* modified);

    CARAPI AddAll(
        /* [in] */ ICollection* collection,
        /* [out] */ Boolean* modified);

    CARAPI Clear();

    CARAPI ToArray(
        /* [out, callee] */ ArrayOf<IInterface*>** array);

    CARAPI ToArray(
        /* [in] */ ArrayOf<IInterface*>* inArray,
        /* [out, callee] */ ArrayOf<IInterface*>** outArray);

    CARAPI GetIterator(
        /* [out] */ IIterator** iterator);

    CARAPI GetDescendingIterator(
        /* [out] */ IIterator** outiter);

    CARAPI Equals(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    CARAPI Add(
        /* [in] */ IInterface* object);

    CARAPI AddAll(
        /* [in] */ ICollection* collection);

    CARAPI ContainsAll(
        /* [in] */ ICollection* collection,
        /* [out] */ Boolean* result);

    CARAPI RemoveAll(
        /* [in] */ ICollection* collection,
        /* [out] */ Boolean* result);

    CARAPI RemoveAll(
        /* [in] */ ICollection* collection);

    CARAPI RetainAll(
        /* [in] */ ICollection* collection,
        /* [out] */ Boolean* result);

    CARAPI RetainAll(
        /* [in] */ ICollection* collection);

    CARAPI Remove(
        /* [in] */ IInterface* object);

protected:
    CARAPI_(AutoPtr<Node>) PrevTerminator();

    CARAPI_(AutoPtr<Node>) NextTerminator();

    CARAPI_(void) Unlink(
        /* [in] */ Node* x);

    CARAPI_(AutoPtr<Node>) Succ(
        /* [in] */ Node* p);

    CARAPI_(AutoPtr<Node>) Pred(
        /* [in] */ Node* p);

    CARAPI_(AutoPtr<Node>) First();

    CARAPI_(AutoPtr<Node>) Last();

private:
    /**
     * Links e as first element.
     */
    CARAPI_(void) LinkFirst(
        /* [in] */ IInterface* e);

    /**
     * Links e as last element.
     */
    CARAPI_(void) LinkLast(
        /* [in] */ IInterface* e);

    /**
     * Unlinks non-null first node.
     */
    CARAPI_(void) UnlinkFirst(
        /* [in] */ Node* first,
        /* [in] */ Node* next);

    /**
     * Unlinks non-null last node.
     */
    CARAPI_(void) UnlinkLast(
        /* [in] */ Node* last,
        /* [in] */ Node* prev);

    /**
     * Guarantees that any node which was unlinked before a call to
     * this method will be unreachable from head after it returns.
     * Does not guarantee to eliminate slack, only that head will
     * point to a node that was active while this method was running.
     */
    CARAPI_(void) UpdateHead();

    /**
     * Guarantees that any node which was unlinked before a call to
     * this method will be unreachable from tail after it returns.
     * Does not guarantee to eliminate slack, only that tail will
     * point to a node that was active while this method was running.
     */
    CARAPI_(void) UpdateTail();

    CARAPI_(void) SkipDeletedPredecessors(
        /* [in] */ Node* x);

    CARAPI_(void) SkipDeletedSuccessors(
        /* [in] */ Node* x);

    // Minor convenience utilities

    /**
     * Throws NullPointerException if argument is null.
     *
     * @param v the element
     */
    static CARAPI CheckNotNull(
        /* [in] */ IInterface* v);

    /**
     * Returns element unless it is null, in which case throws
     * NoSuchElementException.
     *
     * @param v the element
     * @return the element
     */
    CARAPI_(AutoPtr<IInterface>) ScreenNullResult(
        /* [in] */ IInterface* v);

    /**
     * Creates an array list and fills it with elements of this list.
     * Used by toArray.
     *
     * @return the arrayList
     */
    CARAPI_(AutoPtr<IArrayList>) ToArrayList();

    /**
     * Initializes head and tail, ensuring invariants hold.
     */
    CARAPI_(void) InitHeadTail(
        /* [in] */ Node* h,
        /* [in] */ Node* t);

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
     * Reconstitutes the instance from a stream (that is, deserializes it).
     * @param s the stream
     */
    CARAPI_(void) ReadObject(
        /* [in] */ IObjectInputStream* s);


    CARAPI_(Boolean) CasHead(
        /* [in] */ Node* cmp,
        /* [in] */ Node* val);

    CARAPI_(Boolean) CasTail(
        /* [in] */ Node* cmp,
        /* [in] */ Node* val);

private:
//    static Int64 serialVersionUID = 876323262645176354L;

    AutoPtr<Node> mHead;

    AutoPtr<Node> mTail;

    static AutoPtr<Node> sPREV_TERMINATOR;
    static AutoPtr<Node> sNEXT_TERMINATOR;

    static Int32 sHOPS;

    // Unsafe mechanics
//    static sun.misc.Unsafe UNSAFE;
    static Int64 sHeadOffset;
    static Int64 sTailOffset;
};

} // namespace Concurrent
} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CCONCURRENTLINKEDDEQUE_H__
