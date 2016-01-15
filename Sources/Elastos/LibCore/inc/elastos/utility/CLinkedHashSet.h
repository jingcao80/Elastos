#ifndef __ELASTOS_UTILITY_CLINKEDHASHSET_H__
#define __ELASTOS_UTILITY_CLINKEDHASHSET_H__

#include "_Elastos_Utility_CLinkedHashSet.h"
#include "HashSet.h"

namespace Elastos {
namespace Utility {

CarClass(CLinkedHashSet)
    , public HashSet
    , public ILinkedHashSet
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Constructs a new empty instance of {@code LinkedHashSet}.
     */
    CARAPI constructor();

    /**
     * Constructs a new instance of {@code LinkedHashSet} with the specified
     * capacity.
     *
     * @param capacity
     *            the initial capacity of this {@code LinkedHashSet}.
     */
    CARAPI constructor(
        /* [in] */ Int32 capacity);

    /**
     * Constructs a new instance of {@code LinkedHashSet} with the specified
     * capacity and load factor.
     *
     * @param capacity
     *            the initial capacity.
     * @param loadFactor
     *            the initial load factor.
     */
    CARAPI constructor(
        /* [in] */ Int32 capacity,
        /* [in] */ Float loadFactor);

    /**
     * Constructs a new instance of {@code LinkedHashSet} containing the unique
     * elements in the specified collection.
     *
     * @param collection
     *            the collection of elements to add.
     */
    CARAPI constructor(
        /* [in] */ ICollection* collection);

    /**
     * Adds the specified object to this {@code HashSet} if not already present.
     *
     * @param object
     *            the object to add.
     * @return {@code true} when this {@code HashSet} did not already contain
     *         the object, {@code false} otherwise
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
     * Returns a new {@code HashSet} with the same elements and size as this
     * {@code HashSet}.
     *
     * @return a shallow copy of this {@code HashSet}.
     * @see java.lang.Cloneable
     */
    // @Override
    // @SuppressWarnings("unchecked")
    CARAPI Clone(
        /* [out] */ IInterface** outface);

    /**
     * Searches this {@code HashSet} for the specified object.
     *
     * @param object
     *            the object to search for.
     * @return {@code true} if {@code object} is an element of this
     *         {@code HashSet}, {@code false} otherwise.
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
     * Returns true if this {@code HashSet} has no elements, false otherwise.
     *
     * @return {@code true} if this {@code HashSet} has no elements,
     *         {@code false} otherwise.
     * @see #size
     */
    CARAPI IsEmpty(
        /* [out] */ Boolean* result);

    /**
     * Removes the specified object from this {@code HashSet}.
     *
     * @param object
     *            the object to remove.
     * @return {@code true} if the object was removed, {@code false} otherwise.
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
     * Returns the number of elements in this {@code HashSet}.
     *
     * @return the number of elements in this {@code HashSet}.
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
     * Returns an Iterator on the elements of this {@code HashSet}.
     *
     * @return an Iterator on the elements of this {@code HashSet}.
     * @see Iterator
     */
    CARAPI GetIterator(
        /* [out] */ IIterator** it);

    CARAPI Add(
        /* [in] */ IInterface* object);

    CARAPI AddAll(
        /* [in] */ ICollection* collection);

    CARAPI Remove(
        /* [in] */ IInterface* object);

    CARAPI RemoveAll(
        /* [in] */ ICollection* collection);

    CARAPI RetainAll(
        /* [in] */ ICollection* collection);

protected:
    CARAPI_(AutoPtr<IMap>) CreateBackingMap(
        /* [in] */ Int32 capacity,
        /* [in] */ Float loadFactor);
};

} // namespace Utility
} // namespace Elastos

#endif // __ELASTOS_UTILITY__CLINKEDHASHSET_H__
