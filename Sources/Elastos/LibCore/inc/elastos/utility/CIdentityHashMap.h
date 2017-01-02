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

#ifndef __ELASTOS_UTILITY_CIDENTITYHASHMAP_H__
#define __ELASTOS_UTILITY_CIDENTITYHASHMAP_H__

#include "_Elastos_Utility_CIdentityHashMap.h"
#include "AbstractMap.h"
#include "MapEntry.h"
#include "AbstractSet.h"
#include "AbstractCollection.h"

using Elastos::IO::IObjectInputStream;
using Elastos::IO::IObjectOutputStream;
using Elastos::IO::IObjectStreamField;

namespace Elastos {
namespace Utility {

CarClass(CIdentityHashMap)
    , public AbstractMap
    , public IIdentityHashMap
    , public ISerializable
    , public ICloneable
{
public:
    class IdentityHashMapEntry
        : public MapEntry
    {
    public:
        IdentityHashMapEntry(
            /* [in] */ CIdentityHashMap* map,
            /* [in] */ IInterface* theKey,
            /* [in] */ IInterface* theValue);

        // @Override
        CARAPI Clone(
            /* [out] */ IInterface** outface);

        // @Override
        CARAPI Equals(
            /* [in] */ IInterface* object,
            /* [out] */ Boolean* value);

        // @Override
        CARAPI GetHashCode(
            /* [out] */ Int32* value);

        // @Override
        CARAPI ToString(
            /* [out] */ String* str);

        // @Override
        CARAPI SetValue(
            /* [in] */ IInterface* object,
            /* [out] */ IInterface** outface);

    private:
        AutoPtr<CIdentityHashMap> mMap;
    };

    class IdentityHashMapIterator
        : public Object
        , public IIterator
    {
    public:
        IdentityHashMapIterator(
            /* [in] */ MapEntry::Type* value,
            /* [in] */ CIdentityHashMap* hm);

        CAR_INTERFACE_DECL()

        CARAPI HasNext(
            /* [out] */ Boolean* value);

        CARAPI CheckConcurrentMod();

        CARAPI GetNext(
            /* [out] */ IInterface** outface);

        CARAPI Remove();

    public:
        AutoPtr<CIdentityHashMap> mAssociatedMap;

        Int32 mExpectedModCount;

        AutoPtr<MapEntry::Type> mType;

        Boolean mCanRemove;

    private:
        Int32 mPosition;// the current position

        // the position of the entry that was last returned from next()
        Int32 mLastPosition;
    };

    class IdentityHashMapEntrySet
        : public AbstractSet
    {
    public:
        IdentityHashMapEntrySet(
            /* [in] */ CIdentityHashMap* hm);

        AutoPtr<CIdentityHashMap> GetHashMap();

        CARAPI GetSize(
            /* [out] */ Int32* value);

        CARAPI Clear();

        CARAPI Remove(
            /* [in] */ IInterface* object,
            /* [out] */ Boolean* value);

        CARAPI Contains(
            /* [in] */ IInterface* object,
            /* [out] */ Boolean* value);

        CARAPI GetIterator(
            /* [out] */ IIterator** outiter);

    protected:
        CARAPI_(String) GetClassName() { return String("CIdentityHashMap::IdentityHashMapEntrySet"); }

    private:
        AutoPtr<CIdentityHashMap> mAssociatedMap;
    };

    class IdentityHashMapKeySet
        : public AbstractSet
    {
    public:
        IdentityHashMapKeySet(
            /* [in] */ CIdentityHashMap* hm);

        CARAPI GetSize(
            /* [out] */ Int32* value);

        CARAPI Clear();

        CARAPI Remove(
            /* [in] */ IInterface* object,
            /* [out] */ Boolean* value);

        CARAPI Contains(
            /* [in] */ IInterface* object,
            /* [out] */ Boolean* value);

        CARAPI GetIterator(
            /* [out] */ IIterator** outiter);

    protected:
        CARAPI_(String) GetClassName() { return String("CIdentityHashMap::IdentityHashMapKeySet"); }

    private:
        CIdentityHashMap* mAssociatedMap;
    };

    class IdentityHashMapValues
        : public AbstractCollection
    {
    public:
        IdentityHashMapValues(
            /* [in] */ CIdentityHashMap* hm);

        CARAPI GetSize(
            /* [out] */ Int32* value);

        CARAPI Clear();

        CARAPI Remove(
            /* [in] */ IInterface* object,
            /* [out] */ Boolean* value);

        CARAPI Contains(
            /* [in] */ IInterface* object,
            /* [out] */ Boolean* value);

        CARAPI GetIterator(
            /* [out] */ IIterator** outiter);

    protected:
        CARAPI_(String) GetClassName() { return String("CIdentityHashMap::IdentityHashMapValues"); }
    private:
        CIdentityHashMap* mAssociatedMap;
    };

    class IdentityHashMapMapEntryType
        : public MapEntry::Type
    {
    public:
        CARAPI Get(
            /* [in] */ IMapEntry* entry,
            /* [out] */ IInterface** outface);
    };

    class IdentityHashMapKeySetMapEntryType
        : public MapEntry::Type
    {
    public:
        CARAPI Get(
            /* [in] */ IMapEntry* entry,
            /* [out] */ IInterface** outface);
    };

    class IdentityHashMapValuesMapEntryType
        : public MapEntry::Type
    {
    public:
        CARAPI Get(
            /* [in] */ IMapEntry* entry,
            /* [out] */ IInterface** outface);
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CIdentityHashMap();

    /**
     * Creates an IdentityHashMap with default expected maximum size.
     */
    CARAPI constructor();

    /**
     * Creates an IdentityHashMap with the specified maximum size parameter.
     *
     * @param maxSize
     *            The estimated maximum number of entries that will be put in
     *            this map.
     */
    CARAPI constructor(
        /* [in] */ Int32 maxSize);

    /**
     * Creates an IdentityHashMap using the given map as initial values.
     *
     * @param map
     *            A map of (key,value) pairs to copy into the IdentityHashMap.
     */
    CARAPI constructor(
        /* [in] */ IMap* map);

    /**
     * Removes all elements from this {@code Map}, leaving it empty.
     *
     * @throws UnsupportedOperationException
     *                if removing elements from this {@code Map} is not supported.
     * @see #isEmpty()
     * @see #size()
     */
    CARAPI Clear();

    /**
     * Returns whether this {@code Map} contains the specified key.
     *
     * @param key
     *            the key to search for.
     * @return {@code true} if this map contains the specified key,
     *         {@code false} otherwise.
     */
    CARAPI ContainsKey(
        /* [in] */ IInterface* key,
        /* [out] */ Boolean* result);

    /**
     * Returns whether this {@code Map} contains the specified value.
     *
     * @param value
     *            the value to search for.
     * @return {@code true} if this map contains the specified value,
     *         {@code false} otherwise.
     */
    CARAPI ContainsValue(
        /* [in] */ IInterface* value,
        /* [out] */ Boolean* result);

    /**
     * Returns a {@code Set} containing all of the mappings in this {@code Map}. Each mapping is
     * an instance of {@link Map.Entry}. As the {@code Set} is backed by this {@code Map},
     * changes in one will be reflected in the other.
     *
     * @return a set of the mappings
     */
    CARAPI GetEntrySet(
        /* [out] */ ISet** entries);

    /**
     * Compares the argument to the receiver, and returns {@code true} if the
     * specified object is a {@code Map} and both {@code Map}s contain the same mappings.
     *
     * @param object
     *            the {@code Object} to compare with this {@code Object}.
     * @return boolean {@code true} if the {@code Object} is the same as this {@code Object}
     *         {@code false} if it is different from this {@code Object}.
     * @see #hashCode()
     * @see #entrySet()
     */
    CARAPI Equals(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* result);

    /**
     * Returns the value of the mapping with the specified key.
     *
     * @param key
     *            the key.
     * @return the value of the mapping with the specified key, or {@code null}
     *         if no mapping for the specified key is found.
     */
    CARAPI Get(
        /* [in] */ PInterface key,
        /* [out] */ PInterface* value);

    /**
     * Returns an integer hash code for the receiver. {@code Object}s which are equal
     * return the same value for this method.
     *
     * @return the receiver's hash.
     * @see #equals(Object)
     */
    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    /**
     * Returns whether this map is empty.
     *
     * @return {@code true} if this map has no elements, {@code false}
     *         otherwise.
     * @see #size()
     */
    CARAPI IsEmpty(
        /* [out] */ Boolean* result);

    /**
     * Returns a set of the keys contained in this {@code Map}. The {@code Set} is backed by
     * this {@code Map} so changes to one are reflected by the other. The {@code Set} does not
     * support adding.
     *
     * @return a set of the keys.
     */
    CARAPI GetKeySet(
        /* [out] */ ISet** keySet);

    /**
     * Maps the specified key to the specified value.
     *
     * @param key
     *            the key.
     * @param value
     *            the value.
     * @return the value of any previous mapping with the specified key or
     *         {@code null} if there was no mapping.
     * @throws UnsupportedOperationException
     *                if adding to this {@code Map} is not supported.
     * @throws ClassCastException
     *                if the class of the key or value is inappropriate for
     *                this {@code Map}.
     * @throws IllegalArgumentException
     *                if the key or value cannot be added to this {@code Map}.
     * @throws NullPointerException
     *                if the key or value is {@code null} and this {@code Map} does
     *                not support {@code null} keys or values.
     */
    CARAPI Put(
        /* [in] */ PInterface key,
        /* [in] */ PInterface value,
        /* [out] */ PInterface* oldValue);

    CARAPI Put(
        /* [in] */ PInterface key,
        /* [in] */ PInterface value);

    /**
     * Copies every mapping in the specified {@code Map} to this {@code Map}.
     *
     * @param map
     *            the {@code Map} to copy mappings from.
     * @throws UnsupportedOperationException
     *                if adding to this {@code Map} is not supported.
     * @throws ClassCastException
     *                if the class of a key or a value of the specified {@code Map} is
     *                inappropriate for this {@code Map}.
     * @throws IllegalArgumentException
     *                if a key or value cannot be added to this {@code Map}.
     * @throws NullPointerException
     *                if a key or value is {@code null} and this {@code Map} does not
     *                support {@code null} keys or values.
     */
    CARAPI PutAll(
        /* [in] */ IMap* map);

    /**
     * Removes a mapping with the specified key from this {@code Map}.
     *
     * @param key
     *            the key of the mapping to remove.
     * @return the value of the removed mapping or {@code null} if no mapping
     *         for the specified key was found.
     * @throws UnsupportedOperationException
     *                if removing from this {@code Map} is not supported.
     */
    CARAPI Remove(
        /* [in] */ PInterface key,
        /* [out] */ PInterface* value);

    CARAPI Remove(
        /* [in] */ PInterface key);

    /**
     * Returns the number of mappings in this {@code Map}.
     *
     * @return the number of mappings in this {@code Map}.
     */
    CARAPI GetSize(
        /* [out] */ Int32* size);

    /**
     * Returns a {@code Collection} of the values contained in this {@code Map}. The {@code Collection}
     * is backed by this {@code Map} so changes to one are reflected by the other. The
     * {@code Collection} supports {@link Collection#remove}, {@link Collection#removeAll},
     * {@link Collection#retainAll}, and {@link Collection#clear} operations,
     * and it does not support {@link Collection#add} or {@link Collection#addAll} operations.
     * <p>
     * This method returns a {@code Collection} which is the subclass of
     * {@link AbstractCollection}. The {@link AbstractCollection#iterator} method of this subclass returns a
     * "wrapper object" over the iterator of this {@code Map}'s {@link #entrySet()}. The {@link AbstractCollection#size} method
     * wraps this {@code Map}'s {@link #size} method and the {@link AbstractCollection#contains} method wraps this {@code Map}'s
     * {@link #containsValue} method.
     * <p>
     * The collection is created when this method is called at first time and
     * returned in response to all subsequent calls. This method may return
     * different Collection when multiple calls to this method, since it has no
     * synchronization performed.
     *
     * @return a collection of the values contained in this map.
     */
    CARAPI GetValues(
        /* [out] */ ICollection** value);

    CARAPI Clone(
        /* [out] */ IInterface** object);

protected:
    CARAPI_(String) GetClassName() { return String("CIdentityHashMap"); }

private:
    CARAPI_(Int32) GetThreshold(
        /* [in] */ Int32 maxSize);

    CARAPI_(Int32) ComputeElementArraySize();

    /**
     * Create a new element array
     *
     * @param s
     *            the number of elements
     * @return Reference to the element array
     */
    CARAPI_(AutoPtr< ArrayOf<IInterface*> >) NewElementArray(
        /* [in] */ Int32 s);

    // @SuppressWarnings("unchecked")
    CARAPI_(AutoPtr<IInterface>) MassageValue(
        /* [in] */ IInterface* value);

    CARAPI_(AutoPtr<IdentityHashMapEntry>) GetEntry(
        /* [in] */ IInterface* key);

    /**
     * Convenience method for getting the IdentityHashMapEntry without the
     * NULL_OBJECT elements
     */
    // @SuppressWarnings("unchecked")
    CARAPI_(AutoPtr<IdentityHashMapEntry>) GetEntry(
        /* [in] */ Int32 index);

    /**
     * Returns the index where the key is found at, or the index of the next
     * empty spot if the key is not found in this table.
     */
    CARAPI_(Int32) FindIndex(
        /* [in] */ IInterface* key,
        /* [in] */ ArrayOf<IInterface*>* array);

    CARAPI_(Int32) GetModuloHash(
        /* [in] */ IInterface* key,
        /* [in] */ Int32 length);

    CARAPI Rehash();

    CARAPI ComputeMaxSize();

    CARAPI WriteObject(
        /* [in] */ IObjectOutputStream* stream);

    // @SuppressWarnings("unchecked")
    CARAPI ReadObject(
        /* [in] */ IObjectInputStream* stream);

    CARAPI PutAllImpl(
        /* [in] */ IMap* map);

private:
    static const Int64 sSerialVersionUID = 8188218128353913216L;

    /*
     * The internal data structure to hold key value pairs This array holds keys
     * and values in an alternating fashion.
     */
    /* transient */ AutoPtr< ArrayOf<IInterface*> > mElementData;

    /* Actual number of key-value pairs. */
    Int32 mSize;

    /*
     * maximum number of elements that can be put in this map before having to
     * rehash.
     */
    /* transient */ Int32 mThreshold;

    /*
     * default threshold value that an IdentityHashMap created using the default
     * constructor would have.
     */
    static const Int32 DEFAULT_MAX_SIZE = 21;

    /* Default load factor of 0.75; */
    static const Int32 sLoadFactor = 7500;

    /*
     * modification count, to keep track of structural modifications between the
     * IdentityHashMap and the iterator
     */
    /* transient */ Int32 mModCount; // = 0;

    /*
     * Object used to represent null keys and values. This is used to
     * differentiate a literal 'null' key value pair from an empty spot in the
     * map.
     */
    static const AutoPtr<IInterface> NULL_OBJECT; // = new Object();  //$NON-LOCK-1$
};

} // namespace Utility
} // namespace Elastos

#endif // __ELASTOS_UTILITY__CIDENTITYHASHMAP_H__
