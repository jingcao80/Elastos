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

#ifndef __ELASTOS_UTILITY_CENUMMAP_H__
#define __ELASTOS_UTILITY_CENUMMAP_H__

#include "_Elastos_Utility_CEnumMap.h"
#include "AbstractMap.h"
#include "AbstractSet.h"
#include "AbstractCollection.h"
#include "MapEntry.h"

using Elastos::IO::IObjectInputStream;
using Elastos::IO::IObjectOutputStream;
using Elastos::IO::IObjectStreamField;
using Elastos::Core::IEnum;

namespace Elastos {
namespace Utility {

CarClass(CEnumMap)
    , public AbstractMap
    , public IEnumMap
    , public ISerializable
    , public ICloneable
{
private:
    class EnumMapEntry
        : public MapEntry
    {
    public:
        EnumMapEntry(
            /* [in] */ IInterface* theKey,
            /* [in] */ IInterface* theValue,
            /* [in] */ CEnumMap* em);

        // @SuppressWarnings("unchecked")
        // @Override
        CARAPI Equals(
            /* [in] */ IInterface* object,
            /* [out] */ Boolean* value);

        // @Override
        CARAPI GetHashCode(
            /* [out] */ Int32* value);

        // @SuppressWarnings("unchecked")
        // @Override
        CARAPI GetKey(
            /* [out] */ IInterface** outface);

        // @SuppressWarnings("unchecked")
        // @Override
        CARAPI GetValue(
            /* [out] */ IInterface** outface);

        // @SuppressWarnings("unchecked")
        // @Override
        CARAPI SetValue(
            /* [in] */ IInterface* value,
            /* [out] */ IInterface** outface);

        // @Override
        CARAPI ToString(
            /* [out] */ String* str);

    private:
        CARAPI CheckEntryStatus();

    private:
        AutoPtr<CEnumMap> mEnumMap;

        Int32 mOrdinal;
    };

    class EnumMapIterator
        : public Object
        , public IIterator
    {
    public:
        EnumMapIterator(
            /* [in] */ MapEntry::Type* value,
            /* [in] */ CEnumMap* em);

        CAR_INTERFACE_DECL()

        CARAPI HasNext(
            /* [out] */ Boolean* value);

        // @SuppressWarnings("unchecked")
        CARAPI GetNext(
            /* [out] */ IInterface** outface);

        CARAPI Remove();

        // @Override
        // @SuppressWarnings("unchecked")
        CARAPI ToString(
            /* [out] */ String* str);

    private:
        CARAPI CheckStatus();

    public:
        Int32 mPosition;

        Int32 mPrePosition;

        AutoPtr<CEnumMap> mEnumMap;

        AutoPtr<MapEntry::Type> mType;
    };

    class EnumMapKeySet
        : public AbstractSet
    {
    public:
        EnumMapKeySet(
            /*  */ CEnumMap* em);

        // @Override
        CARAPI Clear();

        // @Override
        CARAPI Contains(
            /* [in] */ IInterface* object,
            /* [out] */ Boolean* value);

        // @Override
        // @SuppressWarnings("unchecked")
        CARAPI GetIterator(
            /* [out] */ IIterator** outiter);

        // @Override
        // @SuppressWarnings("unchecked")
        CARAPI Remove(
            /* [in] */ IInterface* object,
            /* [out] */ Boolean* value);

        // @Override
        CARAPI GetSize(
            /* [out] */ Int32* value);

        CARAPI Add(
            /* [in] */ IInterface* object,
            /* [out] */ Boolean* modified);

        CARAPI AddAll(
            /* [in] */ ICollection* collection,
            /* [out] */ Boolean* modified);

        CARAPI ContainsAll(
            /* [in] */ ICollection* collection,
            /* [out] */ Boolean* result);

        CARAPI Equals(
            /* [in] */ IInterface* object,
            /* [out] */ Boolean* result);

        CARAPI GetHashCode(
            /* [out] */ Int32* hashCode);

        CARAPI IsEmpty(
            /* [out] */ Boolean* result);

        CARAPI RemoveAll(
            /* [in] */ ICollection* collection,
            /* [out] */ Boolean* modified);

        CARAPI RetainAll(
            /* [in] */ ICollection* collection,
            /* [out] */ Boolean* modified);

        CARAPI ToArray(
            /* [out, callee] */ ArrayOf<IInterface*>** array);

        CARAPI ToArray(
            /* [in] */ ArrayOf<IInterface*>* inArray,
            /* [out, callee] */ ArrayOf<IInterface*>** outArray);

    protected:
        CARAPI_(String) GetClassName() { return String("CEnumMap::EnumMapKeySet"); }

    private:
        CEnumMap* mEnumMap;
    };

    class EnumMapKeySetType
        : public MapEntry::Type
    {
    public:
        CARAPI Get(
            /* [in] */ IMapEntry* entry,
            /* [out] */ IInterface** outface);
    };

    class EnumMapValueCollection
        : public AbstractCollection
    {
    public:
        EnumMapValueCollection(
            /* [in] */ CEnumMap* em);

        // @Override
        CARAPI Clear();

        // @Override
        CARAPI Contains(
            /* [in] */ IInterface* object,
            /* [out] */ Boolean* value);

        // @Override
        // @SuppressWarnings("unchecked")
        CARAPI GetIterator(
            /* [out] */ IIterator** outiter);

        // @Override
        // @SuppressWarnings("unchecked")
        CARAPI Remove(
            /* [in] */ IInterface* object,
            /* [out] */ Boolean* value);

        // @Override
        CARAPI GetSize(
            /* [out] */ Int32* value);

        CARAPI Add(
            /* [in] */ IInterface* object,
            /* [out] */ Boolean* modified);

        CARAPI AddAll(
            /* [in] */ ICollection* collection,
            /* [out] */ Boolean* modified);

        CARAPI ContainsAll(
            /* [in] */ ICollection* collection,
            /* [out] */ Boolean* result);

        CARAPI Equals(
            /* [in] */ IInterface* object,
            /* [out] */ Boolean* result);

        CARAPI GetHashCode(
            /* [out] */ Int32* hashCode);

        CARAPI IsEmpty(
            /* [out] */ Boolean* result);

        CARAPI RemoveAll(
            /* [in] */ ICollection* collection,
            /* [out] */ Boolean* modified);

        CARAPI RetainAll(
            /* [in] */ ICollection* collection,
            /* [out] */ Boolean* modified);

        CARAPI ToArray(
            /* [out, callee] */ ArrayOf<IInterface*>** array);

        CARAPI ToArray(
            /* [in] */ ArrayOf<IInterface*>* inArray,
            /* [out, callee] */ ArrayOf<IInterface*>** outArray);

    protected:
        CARAPI_(String) GetClassName() { return String("CEnumMap::EnumMapValueCollection"); }
    private:
        CEnumMap* mEnumMap;
    };

    class EnumMapValueCollectionType
        : public MapEntry::Type
    {
    public:
        CARAPI Get(
            /* [in] */ IMapEntry* entry,
            /* [out] */ IInterface** outface);
    };

    class EnumMapEntryIterator
        : public EnumMapIterator
    {
    public:
        EnumMapEntryIterator(
            /* [in] */ MapEntry::Type* value,
            /* [in] */ CEnumMap* em);

        // @SuppressWarnings("unchecked")
        // @Override
        CARAPI GetNext(
            /* [out] */ IInterface** outface);
    };

    class EnumMapEntrySet
        : public AbstractSet
    {
    public:
        EnumMapEntrySet(
            /* [in] */ CEnumMap* em);

        // @Override
        CARAPI Clear();

        // @Override
        CARAPI Contains(
            /* [in] */ IInterface* object,
            /* [out] */ Boolean* value);

        // @Override
        // @SuppressWarnings("unchecked")
        CARAPI GetIterator(
            /* [out] */ IIterator** outiter);

        // @Override
        // @SuppressWarnings("unchecked")
        CARAPI Remove(
            /* [in] */ IInterface* object,
            /* [out] */ Boolean* value);

        // @Override
        CARAPI GetSize(
            /* [out] */ Int32* value);

        CARAPI Add(
            /* [in] */ IInterface* object,
            /* [out] */ Boolean* modified);

        CARAPI AddAll(
            /* [in] */ ICollection* collection,
            /* [out] */ Boolean* modified);

        CARAPI ContainsAll(
            /* [in] */ ICollection* collection,
            /* [out] */ Boolean* result);

        CARAPI Equals(
            /* [in] */ IInterface* object,
            /* [out] */ Boolean* result);

        CARAPI GetHashCode(
            /* [out] */ Int32* hashCode);

        CARAPI IsEmpty(
            /* [out] */ Boolean* result);

        CARAPI RemoveAll(
            /* [in] */ ICollection* collection,
            /* [out] */ Boolean* modified);

        CARAPI RetainAll(
            /* [in] */ ICollection* collection,
            /* [out] */ Boolean* modified);

        CARAPI ToArray(
            /* [out, callee] */ ArrayOf<IInterface*>** array);

        CARAPI ToArray(
            /* [in] */ ArrayOf<IInterface*>* inArray,
            /* [out, callee] */ ArrayOf<IInterface*>** outArray);

    protected:
        CARAPI_(String) GetClassName() { return String("CEnumMap::EnumMapEntrySet"); }
    private:
        CEnumMap* mEnumMap;
    };

    class EnumMapEntrySetType
        : public MapEntry::Type
    {
    public:
        CARAPI Get(
            /* [in] */ IMapEntry* entry,
            /* [out] */ IInterface** outface);
    };

public:
    CEnumMap();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    /**
     * Constructs an empty {@code EnumMap} using the given key type.
     *
     * @param keyType
     *            the class object giving the type of the keys used by this {@code EnumMap}.
     * @throws NullPointerException
     *             if {@code keyType} is {@code null}.
     */
    CARAPI constructor(
        /* [in] */ InterfaceID keyType);

    /**
     * Constructs an {@code EnumMap} using the same key type as the given {@code EnumMap} and
     * initially containing the same mappings.
     *
     * @param map
     *            the {@code EnumMap} from which this {@code EnumMap} is initialized.
     * @throws NullPointerException
     *             if {@code map} is {@code null}.
     */
    CARAPI constructor(
        /* [in] */ IEnumMap* enummap);

    /**
     * Constructs an {@code EnumMap} initialized from the given map. If the given map
     * is an {@code EnumMap} instance, this constructor behaves in the exactly the same
     * way as {@link EnumMap#EnumMap(EnumMap)}}. Otherwise, the given map
     * should contain at least one mapping.
     *
     * @param map
     *            the map from which this {@code EnumMap} is initialized.
     * @throws IllegalArgumentException
     *             if {@code map} is not an {@code EnumMap} instance and does not contain
     *             any mappings.
     * @throws NullPointerException
     *             if {@code map} is {@code null}.
     */
    // @SuppressWarnings("unchecked")
    CARAPI constructor(
        /* [in] */ IMap* map);

    /**
     * Returns a string containing a concise, human-readable description of this
     * object. In this case, the enum constant's name is returned.
     *
     * @return a printable representation of this object.
     */
    // @Override
    CARAPI ToString(
        /* [out] */ String* str);

    /**
     * Compares this object with the specified object and indicates if they are
     * equal. In order to be equal, {@code object} must be identical to this
     * enum constant.
     *
     * @param other
     *            the object to compare this enum constant with.
     * @return {@code true} if the specified object is equal to this
     *         {@code Enum}; {@code false} otherwise.
     */
    // @Override
    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* value);

    // @Override
    CARAPI GetHashCode(
        /* [out] */ Int32* value);

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
        /* [in] */ IInterface* key,
        /* [in] */ IInterface* value);

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
        /* [in] */ IInterface* key);

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
    CARAPI_(String) GetClassName() { return String("CEnumMap"); }

private:
    // @SuppressWarnings("unchecked")
    CARAPI ReadObject(
        /* [in] */ IObjectInputStream* stream);

    CARAPI WriteObject(
        /* [in] */ IObjectOutputStream* stream);

    CARAPI_(Boolean) IsValidKeyType(
        /* [in] */ IInterface* key);

    // @SuppressWarnings("unchecked")
    CARAPI Initialization(
        /* [in] */ IEnumMap* enumMap);

    CARAPI Initialization(
        /* [in] */ InterfaceID type);

    // @SuppressWarnings("unchecked")
    CARAPI PutAllImpl(
        /* [in] */ IMap* map);

    // @SuppressWarnings("unchecked")
    CARAPI PutImpl(
        /* [in] */ IInterface* key,
        /* [in] */ IInterface* value,
        /* [out] */ IInterface** oldValue);

private:
    // BEGIN android-changed
    // added implements Map<K, V> for apicheck
    // END android-changed

    const static Int64 sSerialVersionUID = 458661240069192865L;

    InterfaceID mKeyType;

    AutoPtr< ArrayOf<IEnum*> > mKeys;

    AutoPtr< ArrayOf<IInterface*> > mValues;

    AutoPtr< ArrayOf<Boolean> > mHasMapping;

    Int32 mMappingsCount;

    Int32 mEnumSize;

    AutoPtr<EnumMapEntrySet> mEntrySet;
};

} // namespace Utility
} // namespace Elastos

#endif // __ELASTOS_UTILITY__CENUMMAP_H__
