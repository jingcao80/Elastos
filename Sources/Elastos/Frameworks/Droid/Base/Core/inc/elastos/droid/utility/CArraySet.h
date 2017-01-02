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

#ifndef __ELASTOS_DROID_UTILITY_CARRAYSET_H__
#define __ELASTOS_DROID_UTILITY_CARRAYSET_H__

#include "_Elastos_Droid_Utility_CArraySet.h"
#include <elastos/core/Object.h>
#include "elastos/droid/utility/MapCollections.h"

using Elastos::Utility::ISet;
using Elastos::Utility::IIterable;
using Elastos::Utility::ICollection;

namespace Elastos {
namespace Droid {
namespace Utility {

CarClass(CArraySet)
    , public Object
    , public IArraySet
    , public ISet
    , public ICollection
    , public IIterable
{
private:
    class InnerMapCollections
        : public MapCollections
    {
    public:
        InnerMapCollections(
            /* [in] */ CArraySet* host);

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
        CArraySet* mHost;
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

    CArraySet();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 capacity);

    CARAPI constructor(
        /* [in] */ IArraySet* map);

    CARAPI_(Int32) GetIndexOf(
        /* [in] */ IInterface* key,
        /* [in] */ Int32 hash);

    CARAPI_(Int32) GetIndexOfNull();

    CARAPI Clear();

    /**
     * Ensure the array map can hold at least <var>minimumCapacity</var>
     * items.
     */
    CARAPI EnsureCapacity(
        /* [in] */ Int32 minimumCapacity);

    /**
     * Check whether a value exists in the set.
     *
     * @param key The value to search for.
     * @return Returns true if the value exists, else false.
     */
    CARAPI Contains(
        /* [in] */ IInterface* key,
        /* [out] */ Boolean* result);

    /**
     * Returns the index of a key in the set.
     *
     * @param key The key to search for.
     * @return Returns the index of the key if it exists, else a negative integer.
     */
    CARAPI GetIndexOf(
        /* [in] */ IInterface* key,
        /* [out] */ Int32* result);

    /**
     * Return the value at the given index in the array.
     * @param index The desired index, must be between 0 and {@link #size()}-1.
     * @return Returns the value stored at the given index.
     */
    CARAPI GetValueAt(
        /* [in] */ Int32 index,
        /* [out] */ IInterface** value);

    /**
     * Return true if the array map contains no items.
     */
    CARAPI IsEmpty(
        /* [out] */ Boolean* result);

    CARAPI Add(
        /* [in] */ IInterface* key,
        /* [out] */ Boolean* result);

    CARAPI Add(
        /* [in] */ IInterface* key);

    CARAPI AddAll(
        /* [in] */ IArraySet* set);

    CARAPI Remove(
        /* [in] */ IInterface* key);

    CARAPI Remove(
        /* [in] */ IInterface* key,
        /* [out] */ Boolean* result);

    CARAPI RemoveAt(
        /* [in] */ Int32 index,
        /* [out] */ IInterface** value);

    CARAPI GetSize(
        /* [out] */ Int32* size);

    CARAPI ToArray(
        /* [out, callee] */ ArrayOf<IInterface*>** array);

    CARAPI ToArray(
        /* [in] */ ArrayOf<IInterface*>* inArray,
        /* [out, callee] */ ArrayOf<IInterface*>** outArray);

    CARAPI Equals(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* result);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI GetIterator(
        /* [out] */ IIterator** it);

    CARAPI ContainsAll(
        /* [in] */ ICollection* collection,
        /* [out] */ Boolean* result);

    CARAPI AddAll(
        /* [in] */ ICollection* collection);

    CARAPI AddAll(
        /* [in] */ ICollection* collection,
        /* [out] */ Boolean* result);

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

protected:
    CARAPI_(String) GetClassName() { return String("CArraySet"); }

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
    CARAPI_(AutoPtr<MapCollections>) GetCollection();

private:
    static const Boolean DEBUG;// = false;
    static const String TAG;// = "ArraySet";

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
    static Int32 sBaseCacheSize;
    static AutoPtr<ArrayOf<IInterface*> > sTwiceBaseCache;
    static Int32 sTwiceBaseCacheSize;

    AutoPtr<ArrayOf<Int32> > mHashes;
    AutoPtr<ArrayOf<IInterface*> > mArray;
    Int32 mSize;
    AutoPtr<MapCollections> mCollections;
};

} // Utility
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_UTILITY_CARRAYSET_H__
