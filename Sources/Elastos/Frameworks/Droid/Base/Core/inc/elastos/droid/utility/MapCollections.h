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

#ifndef __ELASTOS_DROID_UTILITY_MAPCOLLECTIONS_H__
#define __ELASTOS_DROID_UTILITY_MAPCOLLECTIONS_H__

#include <elastos/core/Object.h>

using Elastos::Utility::ICollection;
using Elastos::Utility::IIterable;
using Elastos::Utility::IIterator;
using Elastos::Utility::ISet;
using Elastos::Utility::IMap;
using Elastos::Utility::IMapEntry;

namespace Elastos {
namespace Droid {
namespace Utility {

/**
 * Helper for writing standard Java collection interfaces to a data
 * structure like {@link ArrayMap}.
 * @hide
 */
class MapCollections
    : public Object
{
public:

    class ArrayIterator
        : public Object
        , public IIterator
    {
    public:
        CAR_INTERFACE_DECL()

        ArrayIterator(
            /* [in] */ Int32 offset,
            /* [in] */ MapCollections* mc);

        CARAPI HasNext(
            /* [out] */ Boolean* result);

        CARAPI GetNext(
            /* [out] */ IInterface** obj);

        CARAPI Remove();

    private:
        const Int32 mOffset;
        Int32 mSize;
        Int32 mIndex;
        Boolean mCanRemove;// = false;
        MapCollections* mHost;
    };

    class MapIterator
        : public Object
        , public IIterator
        , public IMapEntry
    {
    public:
        CAR_INTERFACE_DECL()

        MapIterator(
            /* [in] */ MapCollections* mc);

        CARAPI HasNext(
            /* [out] */ Boolean* result);

        CARAPI GetNext(
            /* [out] */ IInterface** obj);

        CARAPI Remove();

        CARAPI GetKey(
            /* [out] */ IInterface** key);

        CARAPI GetValue(
            /* [out] */ IInterface** value);

        CARAPI SetValue(
            /* [in] */ IInterface* object,
            /* [out] */ IInterface** oldObject);

        CARAPI Equals(
            /* [in] */ IInterface* o,
            /* [out] */ Boolean* result);

        CARAPI GetHashCode(
            /* [out] */ Int32* hash);

        CARAPI ToString(
            /* [out] */ String* str);

    private:
        Int32 mEnd;
        Int32 mIndex;
        Boolean mEntryValid;// = false;
        MapCollections* mHost;
    };

    class EntrySet
        : public Object
        , public ISet
        , public ICollection
        , public IIterable
    {
    public:
        CAR_INTERFACE_DECL()

        MapCollections* mHost;

        EntrySet(
            /* [in] */ MapCollections* mc)
            : mHost(mc)
        {}

        CARAPI Add(
            /* [in] */ IMapEntry* obj,
            /* [out] */ Boolean* result);

        CARAPI Add(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* result);

        CARAPI Add(
            /* [in] */ IInterface* obj);

        CARAPI AddAll(
            /* [in] */ ICollection* objs,
            /* [out] */ Boolean* result);

        CARAPI AddAll(
            /* [in] */ ICollection* objs);

        CARAPI Clear();

        CARAPI Contains(
            /* [in] */ IInterface* o,
            /* [out] */ Boolean* result);

        CARAPI ContainsAll(
            /* [in] */ ICollection* o,
            /* [out] */ Boolean* result);

        CARAPI IsEmpty(
            /* [out] */ Boolean* result);

        CARAPI GetIterator(
            /* [out] */ IIterator** it);

        CARAPI Remove(
            /* [in] */ IInterface* object,
            /* [out] */ Boolean* result);

        CARAPI RemoveAll(
            /* [in] */ ICollection* object,
            /* [out] */ Boolean* result);

        CARAPI RetainAll(
            /* [in] */ ICollection* object,
            /* [out] */ Boolean* result);

        CARAPI Remove(
            /* [in] */ IInterface* object);

        CARAPI RemoveAll(
            /* [in] */ ICollection* object);

        CARAPI RetainAll(
            /* [in] */ ICollection* object);

        CARAPI GetSize(
            /* [out] */ Int32* size);

        CARAPI ToArray(
            /* [out, callee] */ ArrayOf<IInterface*>** array);

        CARAPI ToArray(
            /* [in] */ ArrayOf<IInterface*>* inArray,
            /* [out, callee] */ ArrayOf<IInterface*>** outArray);

        CARAPI Equals(
            /* [in] */ IInterface* o,
            /* [out] */ Boolean* result);

        CARAPI GetHashCode(
            /* [out] */ Int32* hash);
    };

    class KeySet
        : public Object
        , public ISet
        , public ICollection
        , public IIterable
    {
    public:
        CAR_INTERFACE_DECL()

        MapCollections* mHost;

        KeySet(
            /* [in] */ MapCollections* mc)
            : mHost(mc)
        {}

        CARAPI Add(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* result);

        CARAPI Add(
            /* [in] */ IInterface* obj);

        CARAPI AddAll(
            /* [in] */ ICollection* objs);

        CARAPI AddAll(
            /* [in] */ ICollection* objs,
            /* [out] */ Boolean* result);

        CARAPI Clear();

        CARAPI Contains(
            /* [in] */ IInterface* o,
            /* [out] */ Boolean* result);

        CARAPI ContainsAll(
            /* [in] */ ICollection* o,
            /* [out] */ Boolean* result);

        CARAPI IsEmpty(
            /* [out] */ Boolean* result);

        CARAPI GetIterator(
            /* [out] */ IIterator** it);

        CARAPI Remove(
            /* [in] */ IInterface* object,
            /* [out] */ Boolean* result);

        CARAPI RemoveAll(
            /* [in] */ ICollection* object,
            /* [out] */ Boolean* result);

        CARAPI RetainAll(
            /* [in] */ ICollection* object,
            /* [out] */ Boolean* result);

        CARAPI Remove(
            /* [in] */ IInterface* objectt);

        CARAPI RemoveAll(
            /* [in] */ ICollection* object);

        CARAPI RetainAll(
            /* [in] */ ICollection* object);

        CARAPI GetSize(
            /* [out] */ Int32* size);

        CARAPI ToArray(
            /* [out, callee] */ ArrayOf<IInterface*>** array);

        CARAPI ToArray(
            /* [in] */ ArrayOf<IInterface*>* inArray,
            /* [out, callee] */ ArrayOf<IInterface*>** outArray);

        CARAPI Equals(
            /* [in] */ IInterface* o,
            /* [out] */ Boolean* result);

        CARAPI GetHashCode(
            /* [out] */ Int32* hash);
    };

    class ValuesCollection
        : public Object
        , public ICollection
        , public IIterable
    {
    public:
        CAR_INTERFACE_DECL()

        MapCollections* mHost;

        ValuesCollection(
            /* [in] */ MapCollections* mc)
            : mHost(mc)
        {}

        CARAPI Add(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* result);

        CARAPI Add(
            /* [in] */ IInterface* obj);

        CARAPI AddAll(
            /* [in] */ ICollection* objs,
            /* [out] */ Boolean* result);

        CARAPI AddAll(
            /* [in] */ ICollection* objs);

        CARAPI Clear();

        CARAPI Contains(
            /* [in] */ IInterface* o,
            /* [out] */ Boolean* result);

        CARAPI ContainsAll(
            /* [in] */ ICollection* o,
            /* [out] */ Boolean* result);

        CARAPI IsEmpty(
            /* [out] */ Boolean* result);

        CARAPI GetIterator(
            /* [out] */ IIterator** it);

        CARAPI Remove(
            /* [in] */ IInterface* object,
            /* [out] */ Boolean* result);

        CARAPI RemoveAll(
            /* [in] */ ICollection* object,
            /* [out] */ Boolean* result);

        CARAPI RetainAll(
            /* [in] */ ICollection* object,
            /* [out] */ Boolean* result);

        CARAPI Remove(
            /* [in] */ IInterface* object);

        CARAPI RemoveAll(
            /* [in] */ ICollection* object);

        CARAPI RetainAll(
            /* [in] */ ICollection* object);

        CARAPI GetSize(
            /* [out] */ Int32* size);

        CARAPI ToArray(
            /* [out, callee] */ ArrayOf<IInterface*>** array);

        CARAPI ToArray(
            /* [in] */ ArrayOf<IInterface*>* inArray,
            /* [out, callee] */ ArrayOf<IInterface*>** outArray);

        CARAPI Equals(
            /* [in] */ IInterface* o,
            /* [out] */ Boolean* result);

        CARAPI GetHashCode(
            /* [out] */ Int32* hash);
    };

public:
    static Boolean ContainsAllHelper(
        /* [in] */ IMap* map,
        /* [in] */ ICollection* collection);

    static Boolean RemoveAllHelper(
        /* [in] */ IMap* map,
        /* [in] */ ICollection* collection);

    static Boolean RetainAllHelper(
        /* [in] */ IMap* map,
        /* [in] */ ICollection* collection);

    AutoPtr<ArrayOf<IInterface*> > ToArrayHelper(
        /* [in] */ Int32 offset);

    AutoPtr<ArrayOf<IInterface*> > ToArrayHelper(
        /* [in] */ ArrayOf<IInterface*>* array,
        /* [in] */ Int32 offset);

    static Boolean EqualsSetHelper(
        /* [in] */ ISet* set,
        /* [in] */ IInterface* object);

    AutoPtr<ISet> GetEntrySet();

    AutoPtr<ISet> GetKeySet();

    AutoPtr<ICollection> GetValues();

protected:
    virtual Int32 ColGetSize() = 0;

    virtual AutoPtr<IInterface> ColGetEntry(
        /* [in] */ Int32 index,
        /* [in] */ Int32 offset) = 0;

    virtual Int32 ColIndexOfKey(
        /* [in] */ IInterface* key) = 0;

    virtual Int32 ColIndexOfValue(
        /* [in] */ IInterface* key) = 0;

    virtual AutoPtr<IMap> ColGetMap() = 0;

    virtual void ColPut(
        /* [in] */ IInterface* key,
        /* [in] */ IInterface* value) = 0;

    virtual AutoPtr<IInterface> ColSetValue(
        /* [in] */ Int32 index,
        /* [in] */ IInterface* value) = 0;

    virtual void ColRemoveAt(
        /* [in] */ Int32 index) = 0;

    virtual void ColClear() = 0;

public:
    AutoPtr<EntrySet> mEntrySet;
    AutoPtr<KeySet> mKeySet;
    AutoPtr<ValuesCollection> mValues;
};


} // Utility
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_UTILITY_MAPCOLLECTIONS_H__
