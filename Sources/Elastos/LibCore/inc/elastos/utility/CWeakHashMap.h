#ifndef __ELASTOS_UTILITY_CWEAKHASHMAP_H__
#define __ELASTOS_UTILITY_CWEAKHASHMAP_H__

#include "_Elastos_Utility_CWeakHashMap.h"
#include "AbstractMap.h"
#include "MapEntry.h"

namespace Elastos {
namespace Utility {

CarClass(CWeakHashMap)
    , public AbstractMap
    , public IWeakHashMap
{
private:
    class Entry
        : public Object
        , public IMapEntry
    {
    public:
        Entry(
            /* [in] */ IInterface* key,
            /* [in] */ IInterface* object);

        CAR_INTERFACE_DECL()

        CARAPI GetKey(
            /* [out] */ IInterface** key);

        CARAPI GetValue(
            /* [out] */ IInterface** value);

        CARAPI SetValue(
            /* [in] */ IInterface* object,
            /* [out] */ IInterface** oldObject);

        CARAPI Equals(
            /* [in] */ IInterface* other,
            /* [out] */ Boolean* result);

        CARAPI GetHashCode(
            /* [out] */ Int32* hashCode);

        CARAPI ToString(
            /* [out] */ String* str);

    public:
        AutoPtr<IWeakReference> mKey;
        Int32 mHash;
        Boolean mIsNull;
        AutoPtr<IInterface> mValue;
        AutoPtr<Entry> mNext;
    };

public:
    class HashIterator
        : public Object
        , public IIterator
    {
    public:
        HashIterator(
            /* [in] */ MapEntry::Type* type,
            /* [in] */ CWeakHashMap* owner)
            : mPosition(0)
            , mExpectedModCount(owner->mModCount)
            , mType(type)
            , mOwner(owner)
        {}

        CAR_INTERFACE_DECL()

        CARAPI HasNext(
            /* [out] */ Boolean* result);

        CARAPI GetNext(
            /* [out] */ IInterface** object);

        CARAPI Remove();

    private:
        Int32 mPosition;
        Int32 mExpectedModCount;
        AutoPtr<Entry> mCurrentEntry;
        AutoPtr<Entry> mNextEntry;
        AutoPtr<IInterface> mNextKey;
        AutoPtr<MapEntry::Type> mType;
        AutoPtr<CWeakHashMap> mOwner;
    };

private:
    class _EntrySet
        : public AbstractSet
    {
    private:
        class Type
            : public MapEntry::Type
        {
        public:
            CARAPI Get(
                /* [in] */ IMapEntry* entry,
                /* [out] */ IInterface** outface);
        };

    public:
        _EntrySet(
            /* [in] */ CWeakHashMap* owner)
            : mOwner(owner)
        {}

        CARAPI GetSize(
            /* [out] */ Int32* size);

        CARAPI Clear();

        CARAPI Remove(
            /* [in] */ IInterface* o,
            /* [out] */ Boolean* result);

        CARAPI Contains(
            /* [in] */ IInterface* o,
            /* [out] */ Boolean* result);

        CARAPI GetIterator(
            /* [out] */ IIterator** result);

        CARAPI IsEmpty(
            /* [out] */ Boolean* result);

        CARAPI Add(
            /* [in] */ IInterface* object,
            /* [out] */ Boolean* modified);

        CARAPI AddAll(
            /* [in] */ ICollection* collection,
            /* [out] */ Boolean* modified);

        CARAPI ContainsAll(
            /* [in] */ ICollection* collection,
            /* [out] */ Boolean* result);

        CARAPI RemoveAll(
            /* [in] */ ICollection* collection,
            /* [out] */ Boolean* result);

        CARAPI RetainAll(
            /* [in] */ ICollection* collection,
            /* [out] */ Boolean* result);

        CARAPI ToArray(
            /* [out, callee] */ ArrayOf<IInterface*>** array);

        CARAPI ToArray(
            /* [in] */ ArrayOf<IInterface*>* contents,
            /* [out, callee] */ ArrayOf<IInterface*>** outArray);

        CARAPI Equals(
            /* [in] */ IInterface* object,
            /* [out] */ Boolean* result);

        CARAPI GetHashCode(
            /* [out] */ Int32* hashCode);

    private:
        AutoPtr<CWeakHashMap> mOwner;
    };

    class _KeySet
        : public AbstractSet
    {
    private:
        class Type
            : public MapEntry::Type
        {
        public:
            CARAPI Get(
                /* [in] */ IMapEntry* entry,
                /* [out] */ IInterface** outface);
        };

    public:
        _KeySet(
            /* [in] */ CWeakHashMap* owner)
            : mOwner(owner)
        {}

        CARAPI Contains(
            /* [in] */ IInterface* o,
            /* [out] */ Boolean* result);

        CARAPI GetSize(
            /* [out] */ Int32* size);

        CARAPI Clear();

        CARAPI Remove(
            /* [in] */ IInterface* o,
            /* [out] */ Boolean* result);

        CARAPI GetIterator(
            /* [out] */ IIterator** result);

        CARAPI ToArray(
            /* [out, callee] */ ArrayOf<IInterface*>** array);

        CARAPI ToArray(
            /* [in] */ ArrayOf<IInterface*>* contents,
            /* [out, callee] */ ArrayOf<IInterface*>** outArray);

        CARAPI IsEmpty(
            /* [out] */ Boolean* result);

        CARAPI Add(
            /* [in] */ IInterface* object,
            /* [out] */ Boolean* modified);

        CARAPI AddAll(
            /* [in] */ ICollection* collection,
            /* [out] */ Boolean* modified);

        CARAPI ContainsAll(
            /* [in] */ ICollection* collection,
            /* [out] */ Boolean* result);

        CARAPI RemoveAll(
            /* [in] */ ICollection* collection,
            /* [out] */ Boolean* result);

        CARAPI RetainAll(
            /* [in] */ ICollection* collection,
            /* [out] */ Boolean* result);

        CARAPI Equals(
            /* [in] */ IInterface* object,
            /* [out] */ Boolean* result);

        CARAPI GetHashCode(
            /* [out] */ Int32* hashCode);

    private:
        AutoPtr<CWeakHashMap> mOwner;
    };

    class _Values
        : public AbstractCollection
    {
    private:
        class Type
            : public MapEntry::Type
        {
        public:
            CARAPI Get(
                /* [in] */ IMapEntry* entry,
                /* [out] */ IInterface** outface);
        };

    public:
        _Values(
            /* [in] */ CWeakHashMap* owner)
            : mOwner(owner)
        {}

        CARAPI GetSize(
            /* [out] */ Int32* size);

        CARAPI Clear();

        CARAPI Contains(
            /* [in] */ IInterface* o,
            /* [out] */ Boolean* result);

        CARAPI GetIterator(
            /* [out] */ IIterator** result);

        CARAPI IsEmpty(
            /* [out] */ Boolean* result);

        CARAPI Remove(
            /* [in] */ IInterface* o,
            /* [out] */ Boolean* result);

        CARAPI Add(
            /* [in] */ IInterface* object,
            /* [out] */ Boolean* modified);

        CARAPI AddAll(
            /* [in] */ ICollection* collection,
            /* [out] */ Boolean* modified);

        CARAPI ContainsAll(
            /* [in] */ ICollection* collection,
            /* [out] */ Boolean* result);

        CARAPI RemoveAll(
            /* [in] */ ICollection* collection,
            /* [out] */ Boolean* result);

        CARAPI RetainAll(
            /* [in] */ ICollection* collection,
            /* [out] */ Boolean* result);

        CARAPI ToArray(
            /* [out, callee] */ ArrayOf<IInterface*>** array);

        CARAPI ToArray(
            /* [in] */ ArrayOf<IInterface*>* contents,
            /* [out, callee] */ ArrayOf<IInterface*>** outArray);

        CARAPI Equals(
            /* [in] */ IInterface* object,
            /* [out] */ Boolean* result);

        CARAPI GetHashCode(
            /* [out] */ Int32* hashCode);

    private:
        AutoPtr<CWeakHashMap> mOwner;
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CWeakHashMap()
        : mElementCount(0)
        , mLoadFactor(0)
        , mThreshold(0)
        , mModCount(0)
    {}

    /**
     * Constructs a new empty {@code WeakHashMap} instance.
     */
    CARAPI constructor();

    /**
     * Constructs a new empty {@code WeakHashMap} instance.
     */
    CARAPI constructor(
        /* [in] */ Int32 capacity);

    /**
     * Constructs a new {@code WeakHashMap} instance with the specified capacity
     * and load factor.
     *
     * @param capacity
     *            the initial capacity of this map.
     * @param loadFactor
     *            the initial load factor.
     * @throws IllegalArgumentException
     *             if the capacity is less than zero or the load factor is less
     *             or equal to zero.
     */
    CARAPI constructor(
        /* [in] */ Int32 capacity,
        /* [in] */ Float loadFactor);

    /**
     * Constructs a new {@code WeakHashMap} instance containing the mappings
     * from the specified map.
     *
     * @param map
     *            the mappings to add.
     */
    CARAPI constructor(
        /* [in] */ IMap* map);

    /**
     * Removes all mappings from this map, leaving it empty.
     *
     * @see #isEmpty()
     * @see #size()
     */
    CARAPI Clear();

    /**
     * Returns whether this map contains the specified key.
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
     * Returns a set containing all of the mappings in this map. Each mapping is
     * an instance of {@link Map.Entry}. As the set is backed by this map,
     * changes in one will be reflected in the other. It does not support adding
     * operations.
     *
     * @return a set of the mappings.
     */
    CARAPI GetEntrySet(
        /* [out] */ ISet** entrys);

    /**
     * Returns a set of the keys contained in this map. The set is backed by
     * this map so changes to one are reflected by the other. The set does not
     * support adding.
     *
     * @return a set of the keys.
     */
    CARAPI GetKeySet(
        /* [out] */ ISet** keys);

    /**
     * Returns a collection of the values contained in this map. The collection
     * is backed by this map so changes to one are reflected by the other. The
     * collection supports remove, removeAll, retainAll and clear operations,
     * and it does not support add or addAll operations.
     * <p>
     * This method returns a collection which is the subclass of
     * AbstractCollection. The iterator method of this subclass returns a
     * "wrapper object" over the iterator of map's entrySet(). The size method
     * wraps the map's size method and the contains method wraps the map's
     * containsValue method.
     * <p>
     * The collection is created when this method is called at first time and
     * returned in response to all subsequent calls. This method may return
     * different Collection when multiple calls to this method, since it has no
     * synchronization performed.
     *
     * @return a collection of the values contained in this map.
     */
    CARAPI GetValues(
        /* [out] */ ICollection** values);

    /**
     * Returns the value of the mapping with the specified key.
     *
     * @param key
     *            the key.
     * @return the value of the mapping with the specified key, or {@code null}
     *         if no mapping for the specified key is found.
     */
    CARAPI Get(
        /* [in] */ IInterface* key,
        /* [out] */ IInterface** value);

    CARAPI_(AutoPtr<Entry>) GetEntry(
        /* [in] */ IInterface* key);

    /**
     * Returns whether this map contains the specified value.
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
     * Returns the number of elements in this map.
     *
     * @return the number of elements in this map.
     */
    CARAPI IsEmpty(
        /* [out] */ Boolean* result);

    CARAPI_(void) Poll();

    CARAPI_(void) RemoveEntry(
        /* [in] */ Entry* toRemove);

    /**
     * Maps the specified key to the specified value.
     *
     * @param key
     *            the key.
     * @param value
     *            the value.
     * @return the value of any previous mapping with the specified key or
     *         {@code null} if there was no mapping.
     */
    CARAPI Put(
        /* [in] */ IInterface* key,
        /* [in] */ IInterface* value,
        /* [out] */ IInterface** oldValue);

    CARAPI Put(
        /* [in] */ IInterface* key,
        /* [in] */ IInterface* value);

    /**
     * Copies all the mappings in the given map to this map. These mappings will
     * replace all mappings that this map had for any of the keys currently in
     * the given map.
     *
     * @param map
     *            the map to copy mappings from.
     * @throws NullPointerException
     *             if {@code map} is {@code null}.
     */
    CARAPI PutAll(
        /* [in] */ IMap* map);

    /**
     * Removes the mapping with the specified key from this map.
     *
     * @param key
     *            the key of the mapping to remove.
     * @return the value of the removed mapping or {@code null} if no mapping
     *         for the specified key was found.
     */
    CARAPI Remove(
        /* [in] */ IInterface* key,
        /* [out] */ IInterface** value);

    CARAPI Remove(
        /* [in] */ IInterface* key);

    /**
     * Returns the number of elements in this map.
     *
     * @return the number of elements in this map.
     */
    CARAPI GetSize(
        /* [out] */ Int32* size);

    CARAPI Equals(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

private:
    static CARAPI_(AutoPtr< ArrayOf<Entry*> >) NewEntryArray(
        /* [in] */ Int32 size);

    CARAPI_(void) ComputeMaxSize();

    CARAPI_(void) Rehash();

    CARAPI_(void) PutAllImpl(
        /* [in] */ IMap* map);

private:
    static const Int32 DEFAULT_SIZE = 16;
    Int32 mElementCount;
    AutoPtr< ArrayOf<Entry*> > mElementData;
    Int32 mLoadFactor;
    Int32 mThreshold;
    Int32 mModCount;
};

} // namespace Utility
} // namespace Elastos


DEFINE_CONVERSION_FOR(Elastos::Utility::CWeakHashMap::Entry, IInterface)


#endif // __ELASTOS_UTILITY__CWEAKHASHMAP_H__
