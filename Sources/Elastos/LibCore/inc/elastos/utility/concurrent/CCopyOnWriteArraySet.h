
#ifndef __ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CCOPYONWRITEARRAYSET_H__
#define __ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CCOPYONWRITEARRAYSET_H__

#include "_Elastos_Utility_Concurrent_CCopyOnWriteArraySet.h"
#include "AbstractSet.h"

namespace Elastos {
namespace Utility {
namespace Concurrent {

CarClass(CCopyOnWriteArraySet), public AbstractSet
{
public:
    CAR_OBJECT_DECL()

    /**
     * Creates an empty set.
     */
    CARAPI constructor();

    /**
     * Creates a set containing all of the elements of the specified
     * collection.
     *
     * @param c the collection of elements to initially contain
     * @throws NullPointerException if the specified collection is null
     */
    CARAPI constructor(
        /* [in] */ ICollection* c);

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

private:
    /**
     * Test for equality, coping with nulls.
     */
    static CARAPI_(Boolean) Eq(
        /* [in] */ IInterface* o1,
        /* [in] */ IInterface* o2);

private:
    static const Int64 sSerialVersionUID = 5457747651344034263L;

    AutoPtr<ICopyOnWriteArrayList> mAl;
};

} // namespace Concurrent
} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CCOPYONWRITEARRAYSET_H__
