
#ifndef __ELASTOS_DROID_UTILITY_SPARSEARRAY_H__
#define __ELASTOS_DROID_UTILITY_SPARSEARRAY_H__

#include "Elastos.Droid.Utility.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Core::ICloneable;

namespace Elastos {
namespace Droid {
namespace Utility {

class ECO_PUBLIC SparseArray
    : public Object
    , public ISparseArray
    , public ICloneable
{
public:
    CAR_INTERFACE_DECL();

    SparseArray();

    /**
     * Creates a new SparseArray containing no mappings.
     */
    CARAPI constructor();

    /**
     * Creates a new SparseArray containing no mappings that will not
     * require any additional memory allocation to store the specified
     * number of mappings.  If you supply an initial capacity of 0, the
     * sparse array will be initialized with a light-weight representation
     * not requiring any additional array allocations.
     */
    CARAPI constructor(
        /* [in] */ Int32 initialCapacity);

    CARAPI Clone(
        /* [out] */ IInterface** clone);

    /**
     * Gets the Object mapped from the specified key, or <code>null</code>
     * if no such mapping has been made.
     */
    CARAPI Get(
        /* [in] */ Int32 key,
        /* [out] */ IInterface** outface);

    /**
     * Gets the Object mapped from the specified key, or the specified Object
     * if no such mapping has been made.
     */
    CARAPI Get(
        /* [in] */ Int32 key,
        /* [in] */ IInterface* valueIfKeyNotFound,
        /* [out] */ IInterface** outface);

    /**
     * Removes the mapping from the specified key, if there was any.
     */
    CARAPI Delete(
        /* [in] */ Int32 key);

    /**
     * Alias for {@link #delete(Int32)}.
     */
    CARAPI Remove(
        /* [in] */ Int32 key);

    /**
     * Removes the mapping at the specified index.
     * @hide
     */
    CARAPI RemoveAt(
        /* [in] */ Int32 index);

    /**
     * Remove a range of mappings as a batch.
     *
     * @param index Index to begin at
     * @param size Number of mappings to remove
     */
    CARAPI RemoveAtRange(
        /* [in] */ Int32 index,
        /* [in] */ Int32 size);

    /**
     * Adds a mapping from the specified key to the specified value,
     * replacing the previous mapping from the specified key if there
     * was one.
     */
    CARAPI Put(
        /* [in] */ Int32 key,
        /* [in] */ IInterface* value);

    /**
     * Returns the number of key-value mappings that this SparseArray
     * currently stores.
     */
    CARAPI GetSize(
        /* [out] */ Int32* size);

    /**
     * Given an index in the range <code>0...size()-1</code>, returns
     * the key from the <code>index</code>th key-value mapping that this
     * SparseArray stores.
     *
     * <p>The keys corresponding to indices in ascending order are guaranteed to
     * be in ascending order, e.g., <code>keyAt(0)</code> will return the
     * smallest key and <code>keyAt(size()-1)</code> will return the largest
     * key.</p>
     */
    CARAPI KeyAt(
        /* [in] */ Int32 index,
        /* [out] */ Int32* value);

    /**
     * Given an index in the range <code>0...size()-1</code>, returns
     * the value from the <code>index</code>th key-value mapping that this
     * SparseArray stores.
     *
     * <p>The values corresponding to indices in ascending order are guaranteed
     * to be associated with keys in ascending order, e.g.,
     * <code>valueAt(0)</code> will return the value associated with the
     * smallest key and <code>valueAt(size()-1)</code> will return the value
     * associated with the largest key.</p>
     */
    CARAPI ValueAt(
        /* [in] */ Int32 index,
        /* [out] */ IInterface** outface);

    /**
     * Given an index in the range <code>0...size()-1</code>, sets a new
     * value for the <code>index</code>th key-value mapping that this
     * SparseArray stores.
     */
    CARAPI SetValueAt(
        /* [in] */ Int32 index,
        /* [in] */ IInterface* value);

    /**
     * Returns the index for which {@link #keyAt} would return the
     * specified key, or a negative number if the specified
     * key is not mapped.
     */
    CARAPI IndexOfKey(
        /* [in] */ Int32 key,
        /* [out] */ Int32* value);

    /**
     * Returns an index for which {@link #valueAt} would return the
     * specified key, or a negative number if no keys map to the
     * specified value.
     * <p>Beware that this is a linear search, unlike lookups by key,
     * and that multiple keys can map to the same value and this will
     * find only one of them.
     * <p>Note also that unlike most collections' {@code indexOf} methods,
     * this method compares values using {@code ==} rather than {@code equals}.
     */
    CARAPI IndexOfValue(
        /* [in] */ IInterface* value,
        /* [out] */ Int32* outval);

    /**
     * Removes all key-value mappings from this SparseArray.
     */
    CARAPI Clear();

    /**
     * Puts a key/value pair into the array, optimizing for the case where
     * the key is greater than all existing keys in the array.
     */
    CARAPI Append(
        /* [in] */ Int32 key,
        /* [in] */ IInterface* value);

    /**
     * {@inheritDoc}
     *
     * <p>This implementation composes a string by iterating over its mappings. If
     * this map contains itself as a value, the string "(this Map)"
     * will appear in its place.
     */
    //@Override
    CARAPI ToString(
        /* [out] */ String* str);

private:
    ECO_LOCAL CARAPI_(void) Gc();

private:
    static const AutoPtr<Object> DELETED;
    Boolean mGarbage;
    AutoPtr<ArrayOf<Int32> > mKeys;
    AutoPtr<ArrayOf<IInterface*> > mValues;
    Int32 mSize;
};

}
}
}

#endif // __ELASTOS_DROID_UTILITY_SPARSEARRAY_H__
