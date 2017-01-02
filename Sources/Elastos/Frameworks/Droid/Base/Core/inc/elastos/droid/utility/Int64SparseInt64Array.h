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

#ifndef __ELASTOS_DROID_UTILITY_INT64SPARSEINT64ARRAY_H__
#define __ELASTOS_DROID_UTILITY_INT64SPARSEINT64ARRAY_H__

#include "Elastos.Droid.Utility.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Core::ICloneable;

namespace Elastos {
namespace Droid {
namespace Utility {

/**
 * Map of {@code long} to {@code long}. Unlike a normal array of longs, there
 * can be gaps in the indices. It is intended to be more memory efficient than using a
 * {@code HashMap}, both because it avoids
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
 *
 * @hide
 */
class ECO_PUBLIC Int64SparseInt64Array
    : public Object
    , public ICloneable
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Creates a new SparseLongArray containing no mappings.
     */
    Int64SparseInt64Array();

    /**
     * Creates a new SparseLongArray containing no mappings that will not
     * require any additional memory allocation to store the specified
     * number of mappings.  If you supply an initial capacity of 0, the
     * sparse array will be initialized with a light-weight representation
     * not requiring any additional array allocations.
     */
    Int64SparseInt64Array(
        /* [in] */ Int32 initialCapacity);

    CARAPI Clone(
        /* [out] */ IInterface** result);

    /**
     * Gets the long mapped from the specified key, or <code>0</code>
     * if no such mapping has been made.
     */
    Int64 Get(
        /* [in] */ Int64 key);

    /**
     * Gets the Int64 mapped from the specified key, or the specified value
     * if no such mapping has been made.
     */
    Int64 Get(
        /* [in] */ Int64 key,
        /* [in] */ Int64 valueIfKeyNotFound);

    /**
     * Removes the mapping from the specified key, if there was any.
     */
    CARAPI Delete(
        /* [in] */ Int64 key);

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
        /* [in] */ Int64 key,
        /* [in] */ Int64 value);

    /**
     * Returns the number of key-value mappings that this SparseIntArray
     * currently stores.
     */
    Int32 Size();

    /**
     * Given an index in the range <code>0...size()-1</code>, returns
     * the key from the <code>index</code>th key-value mapping that this
     * SparseLongArray stores.
     *
     * <p>The keys corresponding to indices in ascending order are guaranteed to
     * be in ascending order, e.g., <code>keyAt(0)</code> will return the
     * smallest key and <code>keyAt(size()-1)</code> will return the largest
     * key.</p>
     */
    Int64 KeyAt(
        /* [in] */ Int32 index);

    /**
     * Given an index in the range <code>0...size()-1</code>, returns
     * the value from the <code>index</code>th key-value mapping that this
     * SparseLongArray stores.
     *
     * <p>The values corresponding to indices in ascending order are guaranteed
     * to be associated with keys in ascending order, e.g.,
     * <code>valueAt(0)</code> will return the value associated with the
     * smallest key and <code>valueAt(size()-1)</code> will return the value
     * associated with the largest key.</p>
     */
    Int64 ValueAt(
        /* [in] */ Int32 index);

    /**
     * Returns the index for which {@link #keyAt} would return the
     * specified key, or a negative number if the specified
     * key is not mapped.
     */
    Int32 IndexOfKey(
        /* [in] */ Int64 key);

    /**
     * Returns an index for which {@link #valueAt} would return the
     * specified key, or a negative number if no keys map to the
     * specified value.
     * Beware that this is a linear search, unlike lookups by key,
     * and that multiple keys can map to the same value and this will
     * find only one of them.
     */
    Int32 IndexOfValue(
        /* [in] */ Int64 value);

    /**
     * Removes all key-value mappings from this SparseIntArray.
     */
    CARAPI Clear();

    /**
     * Puts a key/value pair into the array, optimizing for the case where
     * the key is greater than all existing keys in the array.
     */
    CARAPI Append(
        /* [in] */ Int64 key,
        /* [in] */ Int64 value);

    /**
     * {@inheritDoc}
     *
     * <p>This implementation composes a string by iterating over its mappings.
     */
    CARAPI ToString(
        /* [out] */ String* str);

private:
    ECO_LOCAL void Init(
        /* [in] */ Int32 initialCapacity);

private:
    AutoPtr<ArrayOf<Int64> > mKeys;
    AutoPtr<ArrayOf<Int64> > mValues;
    Int32 mSize;
};


}
}
}

#endif // __ELASTOS_DROID_UTILITY_INT64SPARSEINT64ARRAY_H__
