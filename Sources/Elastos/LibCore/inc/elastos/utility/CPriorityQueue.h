#ifndef __ELASTOS_UTILITY_CSTACK_H__
#define __ELASTOS_UTILITY_CSTACK_H__

#include "_Elastos_Utility_CPriorityQueue.h"
#include "AbstractQueue.h"

using Elastos::Core::IComparator;
using Elastos::IO::IObjectInputStream;
using Elastos::IO::IObjectOutputStream;
using Elastos::IO::ISerializable;

namespace Elastos {
namespace Utility {

CarClass(CPriorityQueue)
    , public AbstractQueue
    , public IPriorityQueue
    , public ISerializable
{
private:
    class PriorityIterator
        : public Object
        , public IIterator
    {
    public:
        PriorityIterator(
            /* [in] */ CPriorityQueue* host);

        CAR_INTERFACE_DECL()

        CARAPI HasNext(
            /* [out] */ Boolean* value);

        CARAPI GetNext(
            /* [out] */ IInterface** outinter);

        CARAPI Remove();

    private:
        Int32 mCurrentIndex;

        Boolean mAllowRemove;

        AutoPtr<CPriorityQueue> mHost;
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    /**
     * Constructs a priority queue with an initial capacity of 11 and natural
     * ordering.
     */
    CARAPI constructor();

    /**
     * Constructs a priority queue with the specified capacity and natural
     * ordering.
     *
     * @param initialCapacity
     *            the specified capacity.
     * @throws IllegalArgumentException
     *             if the initialCapacity is less than 1.
     */
    CARAPI constructor(
        /* [in] */ Int32 initialCapacity);

    /**
     * Constructs a priority queue with the specified capacity and comparator.
     *
     * @param initialCapacity
     *            the specified capacity.
     * @param comparator
     *            the specified comparator. If it is null, the natural ordering
     *            will be used.
     * @throws IllegalArgumentException
     *             if the initialCapacity is less than 1.
     */
    CARAPI constructor(
        /* [in] */ Int32 initialCapacity,
        /* [in] */ IComparator* comparator);

    /**
     * Constructs a priority queue that contains the elements of a collection.
     * The constructed priority queue has the initial capacity of 110% of the
     * size of the collection. The queue uses natural ordering to order its
     * elements.
     *
     * @param c
     *            the collection whose elements will be added to the priority
     *            queue to be constructed.
     * @throws ClassCastException
     *             if any of the elements in the collection are not comparable.
     * @throws NullPointerException
     *             if any of the elements in the collection are null.
     */
    CARAPI constructor(
        /* [in] */ ICollection* c);

    /**
     * Constructs a priority queue that contains the elements of another
     * priority queue. The constructed priority queue has the initial capacity
     * of 110% of the specified one. Both priority queues have the same
     * comparator.
     *
     * @param c
     *            the priority queue whose elements will be added to the
     *            priority queue to be constructed.
     */
    CARAPI constructor(
        /* [in] */ IPriorityQueue* c);

    /**
     * Constructs a priority queue that contains the elements of a sorted set.
     * The constructed priority queue has the initial capacity of 110% of the
     * size of the sorted set. The priority queue will have the same comparator
     * as the sorted set.
     *
     * @param c
     *            the sorted set whose elements will be added to the priority
     *            queue to be constructed.
     */
    CARAPI constructor(
        /* [in] */ ISortedSet* c);

    /**
     * Gets the comparator of the priority queue.
     *
     * @return the comparator of the priority queue or null if the natural
     *         ordering is used.
     */
    CARAPI GetComparator(
        /* [out] */ IComparator** outcom);

    /**
     * Inserts the specified element into this queue if it is possible to do
     * so immediately without violating capacity restrictions.
     * When using a capacity-restricted queue, this method is generally
     * preferable to {@link #add}, which can fail to insert an element only
     * by throwing an exception.
     *
     * @param e the element to add
     * @return <tt>true</tt> if the element was added to this queue, else
     *         <tt>false</tt>
     * @throws ClassCastException if the class of the specified element
     *         prevents it from being added to this queue
     * @throws NullPointerException if the specified element is null and
     *         this queue does not permit null elements
     * @throws IllegalArgumentException if some property of this element
     *         prevents it from being added to this queue
     */
    CARAPI Offer(
        /* [in] */ IInterface* e,
        /* [out] */ Boolean* result);

    /**
     * Retrieves and removes the head of this queue.  This method differs
     * from {@link #poll poll} only in that it throws an exception if this
     * queue is empty.
     *
     * @return the head of this queue
     * @throws NoSuchElementException if this queue is empty
     */
    CARAPI Remove(
        /* [out] */ IInterface** e);

    /**
     * Retrieves and removes the head of this queue,
     * or returns <tt>null</tt> if this queue is empty.
     *
     * @return the head of this queue, or <tt>null</tt> if this queue is empty
     */
    CARAPI Poll(
        /* [out] */ IInterface** e);

    /**
     * Retrieves, but does not remove, the head of this queue.  This method
     * differs from {@link #peek peek} only in that it throws an exception
     * if this queue is empty.
     *
     * @return the head of this queue
     * @throws NoSuchElementException if this queue is empty
     */
    CARAPI GetElement(
        /* [out] */ IInterface** e);

    /**
     * Retrieves, but does not remove, the head of this queue,
     * or returns <tt>null</tt> if this queue is empty.
     *
     * @return the head of this queue, or <tt>null</tt> if this queue is empty
     */
    CARAPI Peek(
        /* [out] */ IInterface** e);

    /**
     * Attempts to add {@code object} to the contents of this
     * {@code Collection} (optional).
     *
     * After this method finishes successfully it is guaranteed that the object
     * is contained in the collection.
     *
     * If the collection was modified it returns {@code true}, {@code false} if
     * no changes were made.
     *
     * An implementation of {@code Collection} may narrow the set of accepted
     * objects, but it has to specify this in the documentation. If the object
     * to be added does not meet this restriction, then an
     * {@code IllegalArgumentException} is thrown.
     *
     * If a collection does not yet contain an object that is to be added and
     * adding the object fails, this method <i>must</i> throw an appropriate
     * unchecked Exception. Returning false is not permitted in this case
     * because it would violate the postcondition that the element will be part
     * of the collection after this method finishes.
     *
     * @param object
     *            the object to add.
     * @return {@code true} if this {@code Collection} is
     *         modified, {@code false} otherwise.
     *
     * @throws UnsupportedOperationException
     *                if adding to this {@code Collection} is not supported.
     * @throws ClassCastException
     *                if the class of the object is inappropriate for this
     *                collection.
     * @throws IllegalArgumentException
     *                if the object cannot be added to this {@code Collection}.
     * @throws NullPointerException
     *                if null elements cannot be added to the {@code Collection}.
     */
    CARAPI Add(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* modified);

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
     * Removes all elements from this {@code Collection}, leaving it empty (optional).
     *
     * @throws UnsupportedOperationException
     *                if removing from this {@code Collection} is not supported.
     *
     * @see #isEmpty
     * @see #size
     */
    CARAPI Clear();

    /**
     * Tests whether this {@code Collection} contains the specified object. Returns
     * {@code true} if and only if at least one element {@code elem} in this
     * {@code Collection} meets following requirement:
     * {@code (object==null ? elem==null : object.equals(elem))}.
     *
     * @param object
     *            the object to search for.
     * @return {@code true} if object is an element of this {@code Collection},
     *         {@code false} otherwise.
     * @throws ClassCastException
     *                if the object to look for isn't of the correct
     *                type.
     * @throws NullPointerException
     *                if the object to look for is {@code null} and this
     *                {@code Collection} doesn't support {@code null} elements.
     */
    CARAPI Contains(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* result);

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
    CARAPI ContainsAll(
        /* [in] */ ICollection* collection,
        /* [out] */ Boolean* result);

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
    CARAPI IsEmpty(
        /* [out] */ Boolean* result);

    /**
     * Removes one instance of the specified object from this {@code Collection} if one
     * is contained (optional). The element {@code elem} that is removed
     * complies with {@code (object==null ? elem==null : object.equals(elem)}.
     *
     * @param object
     *            the object to remove.
     * @return {@code true} if this {@code Collection} is modified, {@code false}
     *         otherwise.
     * @throws UnsupportedOperationException
     *                if removing from this {@code Collection} is not supported.
     * @throws ClassCastException
     *                if the object passed is not of the correct type.
     * @throws NullPointerException
     *                if {@code object} is {@code null} and this {@code Collection}
     *                doesn't support {@code null} elements.
     */
    CARAPI Remove(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* modified);

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
    CARAPI RemoveAll(
        /* [in] */ ICollection* collection,
        /* [out] */ Boolean* modified);

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
    CARAPI RetainAll(
        /* [in] */ ICollection* collection,
        /* [out] */ Boolean* modified);

    /**
     * Returns a count of how many objects this {@code Collection} contains.
     *
     * @return how many objects this {@code Collection} contains, or Integer.MAX_VALUE
     *         if there are more than Integer.MAX_VALUE elements in this
     *         {@code Collection}.
     */
    CARAPI GetSize(
        /* [out] */ Int32* size);

    /**
     * Returns a new array containing all elements contained in this {@code Collection}.
     *
     * If the implementation has ordered elements it will return the element
     * array in the same order as an iterator would return them.
     *
     * The array returned does not reflect any changes of the {@code Collection}. A new
     * array is created even if the underlying data structure is already an
     * array.
     *
     * @return an array of the elements from this {@code Collection}.
     */
    CARAPI ToArray(
        /* [out, callee] */ ArrayOf<IInterface*>** array);

    /**
     * Returns an array containing all elements contained in this {@code Collection}. If
     * the specified array is large enough to hold the elements, the specified
     * array is used, otherwise an array of the same type is created. If the
     * specified array is used and is larger than this {@code Collection}, the array
     * element following the {@code Collection} elements is set to null.
     *
     * If the implementation has ordered elements it will return the element
     * array in the same order as an iterator would return them.
     *
     * {@code toArray(new Object[0])} behaves exactly the same way as
     * {@code toArray()} does.
     *
     * @param array
     *            the array.
     * @return an array of the elements from this {@code Collection}.
     *
     * @throws ArrayStoreException
     *                if the type of an element in this {@code Collection} cannot be
     *                stored in the type of the specified array.
     */
    CARAPI ToArray(
        /* [in] */ ArrayOf<IInterface*>* inArray,
        /* [out, callee] */ ArrayOf<IInterface*>** outArray);

    /**
     * Returns an {@link Iterator} for the elements in this object.
     *
     * @return An {@code Iterator} instance.
     */
    CARAPI GetIterator(
        /* [out] */ IIterator** it);

    CARAPI Add(
        /* [in] */ IInterface* object);

    CARAPI AddAll(
        /* [in] */ ICollection* collection);

    CARAPI RemoveAll(
        /* [in] */ ICollection* collection);

    CARAPI RetainAll(
        /* [in] */ ICollection* collection);

private:
    // @SuppressWarnings("unchecked")
    CARAPI ReadObject(
        /* [in] */ IObjectInputStream* in);

    // @SuppressWarnings("unchecked")
    CARAPI_(AutoPtr< ArrayOf<IInterface*> >) NewElementArray(
        /* [in] */ Int32 capacity);

    CARAPI WriteObject(
        /* [in] */ IObjectOutputStream* out);

    // @SuppressWarnings("unchecked")
    CARAPI GetFromPriorityQueue(
        /* [in] */ IPriorityQueue* c);

    // @SuppressWarnings("unchecked")
    CARAPI GetFromSortedSet(
        /* [in] */ ISortedSet* c);

    CARAPI RemoveAt(
        /* [in] */ Int32 index);

    CARAPI_(Int32) Compare(
        /* [in] */ IInterface* o1,
        /* [in] */ IInterface* o2);

    CARAPI SiftUp(
        /* [in] */ Int32 childIndex);

    CARAPI SiftDown(
        /* [in] */ Int32 rootIndex);

    CARAPI InitSize(
        /* [in] */ ICollection* c);

    CARAPI GrowToSize(
        /* [in] */ Int32 size);

private:
    static const Int64 sSerialVersionUID = -7720805057305804111L;

    static const Int32 DEFAULT_CAPACITY = 11;

    static const Double DEFAULT_INIT_CAPACITY_RATIO = 1.1;

    static const Int32 DEFAULT_CAPACITY_RATIO = 2;

    Int32 mSize;

    AutoPtr<IComparator> mComparator;

    AutoPtr< ArrayOf<IInterface*> > mElements;
};

} // namespace Utility
} // namespace Elastos

#endif // __ELASTOS_UTILITY__CSTACK_H__