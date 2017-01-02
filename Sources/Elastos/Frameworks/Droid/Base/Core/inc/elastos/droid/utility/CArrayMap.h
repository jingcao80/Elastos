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

#ifndef __ELASTOS_DROID_UTILITY_CARRAYMAP_H__
#define __ELASTOS_DROID_UTILITY_CARRAYMAP_H__

#include "_Elastos_Droid_Utility_CArrayMap.h"
#include <elastos/core/Object.h>
#include "elastos/droid/utility/MapCollections.h"

using Elastos::Utility::IMap;

namespace Elastos {
namespace Droid {
namespace Utility {

/**
 * ArrayMap is a generic key->value mapping data structure that is
 * designed to be more memory efficient than a traditional {@link java.util.HashMap}.
 * It keeps its mappings in an array data structure -- an integer array of hash
 * codes for each item, and an Object array of the key/value pairs.  This allows it to
 * avoid having to create an extra object for every entry put in to the map, and it
 * also tries to control the growth of the size of these arrays more aggressively
 * (since growing them only requires copying the entries in the array, not rebuilding
 * a hash map).
 *
 * <p>Note that this implementation is not intended to be appropriate for data structures
 * that may contain large numbers of items.  It is generally slower than a traditional
 * HashMap, since lookups require a binary search and adds and removes require inserting
 * and deleting entries in the array.  For containers holding up to hundreds of items,
 * the performance difference is not significant, less than 50%.</p>
 *
 * <p>Because this container is intended to better balance memory use, unlike most other
 * standard Java containers it will shrink its array as items are removed from it.  Currently
 * you have no control over this shrinking -- if you set a capacity and then remove an
 * item, it may reduce the capacity to better match the current size.  In the future an
 * explicit call to set the capacity should turn off this aggressive shrinking behavior.</p>
 */
CarClass(CArrayMap)
    , public Object
    , public IArrayMap
    , public IMap
{
private:
    class InnerMapCollections
        : public MapCollections
    {
    public:
        InnerMapCollections(
            /* [in] */ CArrayMap* host);

        virtual Int32 ColGetSize();

        virtual AutoPtr<IInterface> ColGetEntry(
            /* [in] */ Int32 index,
            /* [in] */ Int32 offset);

        virtual Int32 ColIndexOfKey(
            /* [in] */ IInterface* key);

        virtual Int32 ColIndexOfValue(
            /* [in] */ IInterface* key);

        virtual AutoPtr<IMap> ColGetMap();

        virtual void ColPut(
            /* [in] */ IInterface* key,
            /* [in] */ IInterface* value);

        virtual AutoPtr<IInterface> ColSetValue(
            /* [in] */ Int32 index,
            /* [in] */ IInterface* value);

        virtual void ColRemoveAt(
            /* [in] */ Int32 index);

        virtual void ColClear();

    private:
        CArrayMap* mHost;
    };

    class ObjectsEntry : public Object
    {
    public:
        ObjectsEntry(
            /* [in] */ ArrayOf<IInterface*>* objects)
            : mObjects(objects)
        {}

    public:
        AutoPtr<ArrayOf<IInterface*> > mObjects;
    };

    class HashesEntry : public Object
    {
    public:
        HashesEntry(
            /* [in] */ ArrayOf<Int32>* hashes)
            : mHashes(hashes)
        {}

    public:
        AutoPtr<ArrayOf<Int32> > mHashes;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CArrayMap();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 capacity);

    CARAPI constructor(
        /* [in] */ Boolean immutable);

    CARAPI constructor(
        /* [in] */ IArrayMap* map);

    CARAPI_(Int32) GetIndexOf(
        /* [in] */ IInterface* key,
        /* [in] */ Int32 hash);

    CARAPI_(Int32) GetIndexOfNull();

    CARAPI Clear();

    CARAPI Erase();

    /**
     * Ensure the array map can hold at least <var>minimumCapacity</var>
     * items.
     */
    CARAPI EnsureCapacity(
        /* [in] */ Int32 minimumCapacity);

    /**
     * Check whether a key exists in the array.
     *
     * @param key The key to search for.
     * @return Returns true if the key exists, else false.
     */
    CARAPI ContainsKey(
        /* [in] */ IInterface* key,
        /* [out] */ Boolean* result);

    /**
     * Returns the index of a key in the set.
     *
     * @param key The key to search for.
     * @return Returns the index of the key if it exists, else a negative integer.
     */
    CARAPI GetIndexOfKey(
        /* [in] */ IInterface* key,
        /* [out] */ Int32* result);

    CARAPI GetIndexOfValue(
        /* [in] */ IInterface* value,
        /* [out] */ Int32* result);

    /**
     * Check whether a value exists in the array.  This requires a linear search
     * through the entire array.
     *
     * @param value The value to search for.
     * @return Returns true if the value exists, else false.
     */
    CARAPI ContainsValue(
        /* [in] */ IInterface* value,
        /* [out] */ Boolean* result);

    /**
     * Retrieve a value from the array.
     * @param key The key of the value to retrieve.
     * @return Returns the value associated with the given key,
     * or null if there is no such key.
     */
    CARAPI Get(
        /* [in] */ IInterface* key,
        /* [out] */ IInterface** result);

    /**
     * Return the key at the given index in the array.
     * @param index The desired index, must be between 0 and {@link #size()}-1.
     * @return Returns the key stored at the given index.
     */
    CARAPI GetKeyAt(
        /* [in] */ Int32 index,
        /* [out] */ IInterface** key);

    /**
     * Return the value at the given index in the array.
     * @param index The desired index, must be between 0 and {@link #size()}-1.
     * @return Returns the value stored at the given index.
     */
    CARAPI GetValueAt(
        /* [in] */ Int32 index,
        /* [out] */ IInterface** value);

    /**
     * Set the value at a given index in the array.
     * @param index The desired index, must be between 0 and {@link #size()}-1.
     * @param value The new value to store at this index.
     * @return Returns the previous value at the given index.
     */
    CARAPI SetValueAt(
        /* [in] */ Int32 index,
        /* [in] */ IInterface* value,
        /* [out] */ IInterface** oldValue);

    CARAPI SetValueAt(
        /* [in] */ Int32 index,
        /* [in] */ IInterface* value);

    /**
     * Return true if the array map contains no items.
     */
    CARAPI IsEmpty(
        /* [out] */ Boolean* result);

    CARAPI Put(
        /* [in] */ IInterface* key,
        /* [in] */ IInterface* value,
        /* [out] */ IInterface** oldValue);

    CARAPI Put(
        /* [in] */ IInterface* key,
        /* [in] */ IInterface* value);

    CARAPI Append(
        /* [in] */ IInterface* key,
        /* [in] */ IInterface* result);

    CARAPI Validate();

    CARAPI PutAll(
        /* [in] */ IArrayMap* map);

    CARAPI Remove(
        /* [in] */ IInterface* key);

    CARAPI Remove(
        /* [in] */ IInterface* key,
        /* [out] */ IInterface** result);

    CARAPI RemoveAt(
        /* [in] */ Int32 index);

    CARAPI RemoveAt(
        /* [in] */ Int32 index,
        /* [out] */ IInterface** value);

    CARAPI GetSize(
        /* [out] */ Int32* size);

    CARAPI Equals(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* result);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI ContainsAll(
        /* [in] */ ICollection* collection,
        /* [out] */ Boolean* result);

    CARAPI PutAll(
        /* [in] */ IMap* map);

    CARAPI RemoveAll(
        /* [in] */ ICollection* collection);

    CARAPI RemoveAll(
        /* [in] */ ICollection* collection,
        /* [out] */ Boolean* result);

    CARAPI RetainAll(
        /* [in] */ ICollection* collection);

    CARAPI RetainAll(
        /* [in] */ ICollection* collection,
        /* [out] */ Boolean* result);

    CARAPI GetEntrySet(
        /* [out] */ ISet** set);

    CARAPI GetKeySet(
        /* [out] */ ISet** set);

    CARAPI GetValues(
        /* [out] */ ICollection** set);

private:
    CARAPI AllocArrays(
        /* [in] */ Int32 size);

    static CARAPI FreeArrays(
        /* [in] */ ArrayOf<Int32>* hashes,
        /* [in] */ ArrayOf<IInterface*>* array,
        /* [in] */ Int32 size);

    // ------------------------------------------------------------------------
    // Interop with traditional Java containers.  Not as efficient as using
    // specialized collection APIs.
    // ------------------------------------------------------------------------
    AutoPtr<MapCollections> GetCollection();

public:
    /**
     * @hide Special immutable empty ArrayMap.
     */
    static const AutoPtr<IArrayMap> EMPTY;// = new ArrayMap(true);

private:
    static const Boolean DEBUG;// = false;
    static const String TAG;// = "ArrayMap";

    /**
     * The minimum amount by which the capacity of a ArrayMap will increase.
     * This is tuned to be relatively space-efficient.
     */
    static const Int32 BASE_SIZE;// = 4;

    /**
     * Maximum number of entries to have in array caches.
     */
    static const Int32 CACHE_SIZE;// = 10;

    /**
     * Caches of small array objects to avoid spamming garbage.  The cache
     * Object[] variable is a pointer to a linked list of array objects.
     * The first entry in the array is a pointer to the next array in the
     * list; the second entry is a pointer to the Int32[] hash code array for it.
     */
    static Object sLock;
    static AutoPtr<ArrayOf<IInterface*> > sBaseCache;
    static AutoPtr<ArrayOf<Int32> > sBaseHashes;
    static Int32 sBaseCacheSize;
    static AutoPtr<ArrayOf<IInterface*> > sTwiceBaseCache;
    static AutoPtr<ArrayOf<Int32> > sTwiceBaseHashes;
    static Int32 sTwiceBaseCacheSize;

    /**
     * Special hash array value that indicates the container is immutable.
     */
    static const AutoPtr<ArrayOf<Int32> > EMPTY_IMMUTABLE_INTS;// = new Int32[0];

    AutoPtr<ArrayOf<Int32> > mHashes;
    AutoPtr<ArrayOf<IInterface*> > mArray;
    Int32 mSize;
    AutoPtr<MapCollections> mCollections;
};

} // Utility
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_UTILITY_CARRAYMAP_H__
