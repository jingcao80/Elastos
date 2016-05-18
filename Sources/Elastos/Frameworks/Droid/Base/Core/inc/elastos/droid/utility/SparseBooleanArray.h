
#ifndef __ELASTOS_DROID_UTILITY_SPARSEBOOLEANARRAY_H__
#define __ELASTOS_DROID_UTILITY_SPARSEBOOLEANARRAY_H__

#include "Elastos.Droid.Utility.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Core::ICloneable;

namespace Elastos {
namespace Droid {
namespace Utility {

/**
 * SparseBooleanArrays map integers to booleans.
 * Unlike a normal array of booleans
 * there can be gaps in the indices.  It is intended to be more memory efficient
 * than using a HashMap to map Integers to Booleans, both because it avoids
 * auto-boxing keys and values and its data structure doesn't rely on an extra entry object
 * for each mapping.
 *
 * <p>Note that this container keeps its mappings in an array data structure,
 * using a binary search to find keys.  The implementation is not intended to be appropriate for
 * data structures
 * that may contain large numbers of items.  It is generally slower than a traditional
 * HashMap, since lookups require a binary search and adds and removes require inserting
 * and deleting entries in the array.  For containers holding up to hundreds of items,
 * the performance difference is not significant, less than 50%.</p>
 *
 * <p>It is possible to iterate over the items in this container using
 * {@link #keyAt(int)} and {@link #valueAt(int)}. Iterating over the keys using
 * <code>keyAt(int)</code> with ascending values of the index will return the
 * keys in ascending order, or the values corresponding to the keys in ascending
 * order in the case of <code>valueAt(int)</code>.</p>
 */
class SparseBooleanArray
    : public Object
    , public ISparseBooleanArray
    , public ICloneable
{
public:
    CAR_INTERFACE_DECL()

    SparseBooleanArray();

    /**
     * Creates a new SparseBooleanArray containing no mappings.
     */
    CARAPI constructor();

    /**
     * Creates a new SparseBooleanArray containing no mappings that will not
     * require any additional memory allocation to store the specified
     * number of mappings.  If you supply an initial capacity of 0, the
     * sparse array will be initialized with a light-weight representation
     * not requiring any additional array allocations.
     */
    CARAPI constructor(
        /* [in] */ Int32 initialCapacity);

    // @Override
    CARAPI Clone(
        /* [out] */ IInterface** clone);

    /**
     * Gets the boolean mapped from the specified key, or <code>false</code>
     * if no such mapping has been made.
     */
    CARAPI Get(
        /* [in] */ Int32 key,
        /* [out] */ Boolean* value);

    /**
     * Gets the boolean mapped from the specified key, or the specified value
     * if no such mapping has been made.
     */
    CARAPI Get(
        /* [in] */ Int32 key,
        /* [in] */ Boolean valueIfKeyNotFound,
        /* [out] */ Boolean* value);

    /**
     * Removes the mapping from the specified key, if there was any.
     */
    CARAPI Delete(
        /* [in] */ Int32 key);

    /** @hide */
    CARAPI RemoveAt(
        /* [in] */ Int32 index);

    /**
     * Adds a mapping from the specified key to the specified value,
     * replacing the previous mapping from the specified key if there
     * was one.
     */
    CARAPI Put(
        /* [in] */ Int32 key,
        /* [in] */ Boolean value);

    /**
     * Returns the number of key-value mappings that this SparseBooleanArray
     * currently stores.
     */
    CARAPI GetSize(
        /* [out] */ Int32* size);

    /**
     * Given an index in the range <code>0...size()-1</code>, returns
     * the key from the <code>index</code>th key-value mapping that this
     * SparseBooleanArray stores.
     *
     * <p>The keys corresponding to indices in ascending order are guaranteed to
     * be in ascending order, e.g., <code>keyAt(0)</code> will return the
     * smallest key and <code>keyAt(size()-1)</code> will return the largest
     * key.</p>
     */
    CARAPI KeyAt(
        /* [in] */ Int32 index,
        /* [out] */ Int32* key);

    /**
     * Given an index in the range <code>0...size()-1</code>, returns
     * the value from the <code>index</code>th key-value mapping that this
     * SparseBooleanArray stores.
     *
     * <p>The values corresponding to indices in ascending order are guaranteed
     * to be associated with keys in ascending order, e.g.,
     * <code>valueAt(0)</code> will return the value associated with the
     * smallest key and <code>valueAt(size()-1)</code> will return the value
     * associated with the largest key.</p>
     */
    CARAPI ValueAt(
        /* [in] */ Int32 index,
        /* [out] */ Boolean* value);

    /** @hide */
    CARAPI SetValueAt(
        /* [in] */ Int32 index,
        /* [in] */ Boolean value);

    /**
     * Returns the index for which {@link #keyAt} would return the
     * specified key, or a negative number if the specified
     * key is not mapped.
     */
    CARAPI IndexOfKey(
        /* [in] */ Int32 key,
        /* [out] */ Int32* index);

    /**
     * Returns an index for which {@link #valueAt} would return the
     * specified key, or a negative number if no keys map to the
     * specified value.
     * Beware that this is a linear search, unlike lookups by key,
     * and that multiple keys can map to the same value and this will
     * find only one of them.
     */
    CARAPI IndexOfValue(
        /* [in] */ Boolean value,
        /* [out] */ Int32* index);

    /**
     * Removes all key-value mappings from this SparseBooleanArray.
     */
    CARAPI Clear();

    /**
     * Puts a key/value pair into the array, optimizing for the case where
     * the key is greater than all existing keys in the array.
     */
    CARAPI Append(
        /* [in] */ Int32 key,
        /* [in] */ Boolean value);

    /**
     * {@inheritDoc}
     *
     * <p>This implementation composes a string by iterating over its mappings.
     */
    // @Override
    CARAPI ToString(
        /* [out] */ String* str);

private:
    AutoPtr<ArrayOf<Int32> > mKeys;
    AutoPtr<ArrayOf<Boolean> > mValues;
    Int32 mSize;
};

} // namespace Utility
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_UTILITY_SPARSEBOOLEANARRAY_H__

