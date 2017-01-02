//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_UTILITY_SPARSEINT32ARRAY_H__
#define __ELASTOS_DROID_UTILITY_SPARSEINT32ARRAY_H__

#include "Elastos.Droid.Utility.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Core::ICloneable;

namespace Elastos {
namespace Droid {
namespace Utility {

/**
 * SparseIntArrays map integers to integers.  Unlike a normal array of integers,
 * there can be gaps in the indices.  It is intended to be more memory efficient
 * than using a HashMap to map Integers to Integers, both because it avoids
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
class SparseInt32Array
    : public Object
    , public ISparseInt32Array
    , public ICloneable
{
public:
    CAR_INTERFACE_DECL()

    SparseInt32Array();

    /**
     * Creates a new SparseIntArray containing no mappings.
     */
    CARAPI constructor();

    /**
     * Creates a new SparseIntArray containing no mappings that will not
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
     * Gets the Int32 mapped from the specified key, or <code>0</code>
     * if no such mapping has been made.
     */
    CARAPI Get(
        /* [in] */ Int32 key,
        /* [out] */ Int32* value);

    /**
     * Gets the Int32 mapped from the specified key, or the specified value
     * if no such mapping has been made.
     */
    CARAPI Get(
        /* [in] */ Int32 key,
        /* [in] */ Int32 valueIfKeyNotFound,
        /* [out] */ Int32* value);

    /**
     * Removes the mapping from the specified key, if there was any.
     */
    CARAPI Delete(
        /* [in] */ Int32 key);

    /**
     * Removes the mapping at the given index.
     */
    CARAPI RemoveAt(
        /* [in] */ Int32 index);

    /**
     * Adds a mapping from the specified key to the specified value,
     * replacing the previous mapping from the specified key if there
     * was one.
     */
    CARAPI Put(
        /* [in] */ Int32 key,
        /* [in] */ Int32 value);

    /**
     * Returns the number of key-value mappings that this SparseIntArray
     * currently stores.
     */
    CARAPI GetSize(
        /* [out] */ Int32* size);

    /**
     * Given an index in the range <code>0...size()-1</code>, returns
     * the key from the <code>index</code>th key-value mapping that this
     * SparseIntArray stores.
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
     * SparseIntArray stores.
     *
     * <p>The values corresponding to indices in ascending order are guaranteed
     * to be associated with keys in ascending order, e.g.,
     * <code>valueAt(0)</code> will return the value associated with the
     * smallest key and <code>valueAt(size()-1)</code> will return the value
     * associated with the largest key.</p>
     */
    CARAPI ValueAt(
        /* [in] */ Int32 index,
        /* [out] */ Int32* value);

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
        /* [in] */ Int32 value,
        /* [out] */ Int32* index);

    /**
     * Removes all key-value mappings from this SparseIntArray.
     */
    CARAPI Clear();

    /**
     * Puts a key/value pair into the array, optimizing for the case where
     * the key is greater than all existing keys in the array.
     */
    CARAPI Append(
        /* [in] */ Int32 key,
        /* [in] */ Int32 value);

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
    AutoPtr<ArrayOf<Int32> > mValues;
    Int32 mSize;
};

} // namespace Utility
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_UTILITY_SPARSEINT32ARRAY_H__

