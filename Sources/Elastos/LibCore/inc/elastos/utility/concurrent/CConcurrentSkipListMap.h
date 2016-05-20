
#ifndef __ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CCONCURRENTSKIPLISTMAP_H__
#define __ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CCONCURRENTSKIPLISTMAP_H__

#include "_Elastos_Utility_Concurrent_CConcurrentSkipListMap.h"
#include "AbstractMap.h"
#include "Object.h"

using Elastos::Core::IComparable;
using Elastos::Core::IComparator;
using Elastos::Core::ICloneable;
using Elastos::IO::IObjectOutputStream;
using Elastos::IO::IObjectInputStream;
using Elastos::IO::ISerializable;
using Elastos::Utility::IRandom;
using Elastos::Utility::Concurrent::IConcurrentNavigableMap;

namespace Elastos {
namespace Utility {
namespace Concurrent {

CarClass(CConcurrentSkipListMap)
    , public AbstractMap
    , public INavigableMap
    , public ISortedMap
    , public IConcurrentMap
    , public ICloneable
    , public ISerializable
{
public:
    class Node
        : public Object
    {
    public:
        Node(
            /* [in] */ IInterface* key,
            /* [in] */ IInterface* value,
            /* [in] */ Node* next);

        Node(
            /* [in] */ Node* next);

        CARAPI_(Boolean) CasValue(
            /* [in] */ IInterface* cmp,
            /* [in] */ IInterface* val);

        CARAPI_(Boolean) CasNext(
            /* [in] */ Node* cmp,
            /* [in] */ Node* val);

        CARAPI_(Boolean) IsMarker();

        CARAPI_(Boolean) IsBaseHeader();

        CARAPI_(Boolean) AppendMarker(
            /* [in] */ Node* f);

        CARAPI_(void) HelpDelete(
            /* [in] */ Node* b,
            /* [in] */ Node* f);

        CARAPI_(AutoPtr<IInterface>) GetValidValue();

        CARAPI_(AutoPtr<SimpleImmutableEntry>) CreateSnapshot();

    public:
        AutoPtr<IInterface> mKey;
        AutoPtr<IInterface> mValue;
        AutoPtr<Node> mNext;

        // UNSAFE mechanics

    //     private static final sun.misc.Unsafe UNSAFE;
    //     private static final long valueOffset;
    //     private static final long nextOffset;

    //     static {
    //         try {
    //             UNSAFE = sun.misc.Unsafe.getUnsafe();
    //             Class<?> k = Node.class;
    //             valueOffset = UNSAFE.objectFieldOffset
    //                 (k.getDeclaredField("value"));
    //             nextOffset = UNSAFE.objectFieldOffset
    //                 (k.getDeclaredField("next"));
    //         } catch (Exception e) {
    //             throw new Error(e);
    //         }
    //     }
    };

    class Index
        : public Object
    {
    public:
        Index(
            /* [in] */ Node* node,
            /* [in] */ Index* down,
            /* [in] */ Index* right);

        CARAPI_(Boolean) CasRight(
            /* [in] */ Index* cmp,
            /* [in] */ Index* val);

        CARAPI_(Boolean) IndexesDeletedNode();

        CARAPI_(Boolean) Link(
            /* [in] */ Index* succ,
            /* [in] */ Index* newSucc);

        CARAPI_(Boolean) Unlink(
            /* [in] */ Index* succ);

        // Unsafe mechanics
        // private static final sun.misc.Unsafe UNSAFE;
        // private static final long rightOffset;
        // static {
        //     try {
        //         UNSAFE = sun.misc.Unsafe.getUnsafe();
        //         Class<?> k = Index.class;
        //         rightOffset = UNSAFE.objectFieldOffset
        //             (k.getDeclaredField("right"));
        //     } catch (Exception e) {
        //         throw new Error(e);
        //     }
        // }
    public:
        AutoPtr<Node> mNode;
        AutoPtr<Index> mDown;
        AutoPtr<Index> mRight;
    };

    /* ---------------- Head nodes -------------- */

    /**
     * Nodes heading each level keep track of their level.
     */
    class HeadIndex
        : public Index
    {
    public:
        HeadIndex(
            /* [in] */ Node* node,
            /* [in] */ Index* down,
            /* [in] */ Index* right,
            /* [in] */ Int32 level);

    public:
        Int32 mLevel;
    };

    /* ---------------- Comparison utilities -------------- */

    class ComparableUsingComparator
        : public IComparable
        , public Object
    {
    public:
        CAR_INTERFACE_DECL()

        ComparableUsingComparator(
            /* [in] */ IInterface* key,
            /* [in] */ IComparator* cmp);

        CARAPI CompareTo(
            /* [in] */ IInterface* another,
            /* [out] */ Int32* result);

    public:
        AutoPtr<IInterface> mActualKey;
        AutoPtr<IComparator> mCmp;
    };

        /* ---------------- Iterators -------------- */

    class Iter
        : public IIterator
        , public Object
    {
    public:
        CAR_INTERFACE_DECL()

        Iter(
            /* [in] */ CConcurrentSkipListMap* owner);

        CARAPI HasNext(
            /* [out] */ Boolean* result);

        CARAPI Advance();

        CARAPI Remove();

    public:
        /** the last node returned by next() */
        AutoPtr<Node> mLastReturned;
        /** the next node to return from next(); */
        AutoPtr<Node> mNext;
        /** Cache of next value field to maintain weak consistency */
        AutoPtr<IInterface> mNextValue;

        CConcurrentSkipListMap* mOwner;
    };

    class _ValueIterator
        : public Iter
    {
    public:
        _ValueIterator(
            /* [in] */ CConcurrentSkipListMap* owner);

        CARAPI GetNext(
            /* [out] */ IInterface** object);
    };

    class _KeyIterator
        : public Iter
    {
    public:
        _KeyIterator(
            /* [in] */ CConcurrentSkipListMap* owner);

        CARAPI GetNext(
            /* [out] */ IInterface** object);
    };

    class _EntryIterator
        : public Iter
    {
    public:
        _EntryIterator(
            /* [in] */ CConcurrentSkipListMap* owner);

        CARAPI GetNext(
            /* [out] */ IInterface** object);
    };

    class _KeySet
        : public AbstractSet
        , public INavigableSet
        , public ISortedSet
    {
    public:
        CAR_INTERFACE_DECL()

        _KeySet(
            /* [in] */ INavigableMap* map);

        CARAPI GetSize(
            /* [out] */ Int32* size);

        CARAPI IsEmpty(
            /* [out] */ Boolean* result);

        CARAPI Contains(
            /* [in] */ IInterface* object,
            /* [out] */ Boolean* result);

        CARAPI Remove(
            /* [in] */ IInterface* object,
            /* [out] */ Boolean* modified);

        CARAPI Clear();

        CARAPI GetLower(
            /* [in] */ IInterface* e,
            /* [out] */ IInterface** outface);

        CARAPI GetFloor(
            /* [in] */ IInterface* e,
            /* [out] */ IInterface** outface);

        CARAPI GetCeiling(
            /* [in] */ IInterface* e,
            /* [out] */ IInterface** outface);

        CARAPI GetHigher(
            /* [in] */ IInterface* e,
            /* [out] */ IInterface** outface);

        CARAPI GetComparator(
            /* [out] */ IComparator** outcom);

        CARAPI GetFirst(
            /* [out] */ IInterface** outface);

        CARAPI GetLast(
            /* [out] */ IInterface** outface);

        CARAPI PollFirst(
            /* [out] */ IInterface** outface);

        CARAPI PollLast(
            /* [out] */ IInterface** outface);

        CARAPI GetIterator(
            /* [out] */ IIterator** it);

        CARAPI Equals(
            /* [in] */ IInterface* object,
            /* [out] */ Boolean* result);

        CARAPI ToArray(
            /* [out, callee] */ ArrayOf<IInterface*>** array);

        CARAPI ToArray(
            /* [in] */ ArrayOf<IInterface*>* inArray,
            /* [out, callee] */ ArrayOf<IInterface*>** outArray);

        CARAPI GetDescendingIterator(
            /* [out] */ IIterator** outiter);

        CARAPI GetSubSet(
            /* [in] */ IInterface* fromElement,
            /* [in] */ Boolean fromInclusive,
            /* [in] */ IInterface* toElement,
            /* [in] */ Boolean toInclusive,
            /* [out] */ INavigableSet** outnav);

        CARAPI GetHeadSet(
            /* [in] */ IInterface* toElement,
            /* [in] */ Boolean inclusive,
            /* [out] */ INavigableSet** outnav);

        CARAPI GetTailSet(
            /* [in] */ IInterface* fromElement,
            /* [in] */ Boolean inclusive,
            /* [out] */ INavigableSet** outnav);

        CARAPI GetSubSet(
            /* [in] */ IInterface* start,
            /* [in] */ IInterface* end,
            /* [out] */ ISortedSet** outsort);

        CARAPI GetHeadSet(
            /* [in] */ IInterface* end,
            /* [out] */ ISortedSet** outsort);

        CARAPI GetTailSet(
            /* [in] */ IInterface* start,
            /* [out] */ ISortedSet** outsort);

        CARAPI GetDescendingSet(
            /* [out] */ INavigableSet** outnav);

        CARAPI Add(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* result);

        CARAPI Add(
            /* [in] */ IInterface* obj);

        CARAPI AddAll(
            /* [in] */ ICollection* collection,
            /* [out] */ Boolean* result);

        CARAPI AddAll(
            /* [in] */ ICollection* collection);

        CARAPI ContainsAll(
            /* [in] */ ICollection* collection,
            /* [out] */ Boolean* result);

        CARAPI GetHashCode(
            /* [out] */ Int32* result);

        CARAPI Remove(
            /* [in] */ IInterface* obj);

        CARAPI RemoveAll(
            /* [in] */ ICollection* collection,
            /* [out] */ Boolean* result);

        CARAPI RemoveAll(
            /* [in] */ ICollection* collection);

        CARAPI RetainAll(
            /* [in] */ ICollection* collection,
            /* [out] */ Boolean* result);

        CARAPI RetainAll(
            /* [in] */ ICollection* collection);

    private:
        AutoPtr<INavigableMap> mM;
    };

    class _Values
        : public AbstractCollection
    {
    public:
        _Values(
            /* [in] */ INavigableMap* map);

        CARAPI GetIterator(
            /* [out] */ IIterator** it);

        CARAPI GetSize(
            /* [out] */ Int32* size);

        CARAPI IsEmpty(
            /* [out] */ Boolean* result);

        CARAPI Contains(
            /* [in] */ IInterface* object,
            /* [out] */ Boolean* result);

        CARAPI Clear();

        CARAPI ToArray(
            /* [out, callee] */ ArrayOf<IInterface*>** array);

        CARAPI ToArray(
            /* [in] */ ArrayOf<IInterface*>* inArray,
            /* [out, callee] */ ArrayOf<IInterface*>** outArray);

    private:
        AutoPtr<INavigableMap> mM;
    };

    class _EntrySet
        : public AbstractSet
    {
    public:
        _EntrySet(
            /* [in] */ INavigableMap* map);

        CARAPI Remove(
            /* [in] */ IInterface* object,
            /* [out] */ Boolean* modified);

        CARAPI Equals(
            /* [in] */ IInterface* object,
            /* [out] */ Boolean* result);

        CARAPI GetIterator(
            /* [out] */ IIterator** it);

        CARAPI GetSize(
            /* [out] */ Int32* size);

        CARAPI IsEmpty(
            /* [out] */ Boolean* result);

        CARAPI Contains(
            /* [in] */ IInterface* object,
            /* [out] */ Boolean* result);

        CARAPI Clear();

        CARAPI ToArray(
            /* [out, callee] */ ArrayOf<IInterface*>** array);

        CARAPI ToArray(
            /* [in] */ ArrayOf<IInterface*>* inArray,
            /* [out, callee] */ ArrayOf<IInterface*>** outArray);

        CARAPI Add(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean*);

        CARAPI Add(
            /* [in] */ IInterface* obj);

        CARAPI AddAll(
            /* [in] */ ICollection* collection,
            /* [out] */ Boolean* result);

        CARAPI AddAll(
            /* [in] */ ICollection* collection);

        CARAPI ContainsAll(
            /* [in] */ ICollection* collection,
            /* [out] */ Boolean* result);

        CARAPI GetHashCode(
            /* [out] */ Int32* result);

        CARAPI Remove(
            /* [in] */ IInterface* obj);

        CARAPI RemoveAll(
            /* [in] */ ICollection* collection,
            /* [out] */ Boolean* result);

        CARAPI RemoveAll(
            /* [in] */ ICollection* collection);

        CARAPI RetainAll(
            /* [in] */ ICollection* collection,
            /* [out] */ Boolean* result);

        CARAPI RetainAll(
            /* [in] */ ICollection* collection);
    private:
        AutoPtr<INavigableMap> mM;
    };

    class _SubMap
        : public AbstractMap
        , public INavigableMap
        , public IConcurrentMap
        , public ISortedMap
    {
    public:
        class SubMapIter
            : public IIterator
            , public Object
        {
        public:
            CAR_INTERFACE_DECL()

            SubMapIter(
                /* [in] */ _SubMap* owner);

            CARAPI HasNext(
                /* [out] */ Boolean* result);

            CARAPI_(void) Advance();

            CARAPI Remove();

        private:
            CARAPI_(void) Ascend();

            CARAPI_(void) Descend();

        public:
            AutoPtr<Node> mLastReturned;
            AutoPtr<Node> mNext;
            AutoPtr<IInterface> mNextValue;
            _SubMap* mOwner;
        };

        class SubMapValueIterator
            : public SubMapIter
        {
        public:
            SubMapValueIterator(
                /* [in] */ _SubMap* owner);

            CARAPI GetNext(
                /* [out] */ IInterface** object);
        };

        class SubMapKeyIterator
            : public SubMapIter
        {
        public:
            SubMapKeyIterator(
                /* [in] */ _SubMap* owner);

            CARAPI GetNext(
                /* [out] */ IInterface** object);
        };

        class SubMapEntryIterator
            : public SubMapIter
        {
        public:
            SubMapEntryIterator(
                /* [in] */ _SubMap* owner);

            CARAPI GetNext(
                /* [out] */ IInterface** object);
        };

    public:
        CAR_INTERFACE_DECL()

        _SubMap(
            /* [in] */ CConcurrentSkipListMap* map,
            /* [in] */ IInterface* fromKey,
            /* [in] */ Boolean fromInclusive,
            /* [in] */ IInterface* toKey,
            /* [in] */ Boolean toInclusive,
            /* [in] */ Boolean isDescending);

        /* ----------------  Map API methods -------------- */

        CARAPI ContainsKey(
            /* [in] */ IInterface* key,
            /* [out] */ Boolean* result);

        CARAPI Get(
            /* [in] */ IInterface* key,
            /* [out] */ IInterface** value);

        CARAPI Put(
            /* [in] */ IInterface* key,
            /* [in] */ IInterface* value,
            /* [out] */ IInterface** oldValue);

        CARAPI Remove(
            /* [in] */ IInterface* key,
            /* [out] */ IInterface** value);

        CARAPI GetSize(
            /* [out] */ Int32* size);

        CARAPI IsEmpty(
            /* [out] */ Boolean* result);

        CARAPI ContainsValue(
            /* [in] */ IInterface* value,
            /* [out] */ Boolean* result);

        CARAPI Clear();

        /* ----------------  ConcurrentMap API methods -------------- */

        CARAPI PutIfAbsent(
            /* [in] */ PInterface key,
            /* [in] */ PInterface value,
            /* [out] */ PInterface* out);

        CARAPI Remove(
            /* [in] */ PInterface key,
            /* [in] */ PInterface value,
            /* [out] */ Boolean* res);

        CARAPI Replace(
            /* [in] */ PInterface key,
            /* [in] */ PInterface oldValue,
            /* [in] */ PInterface newValue,
            /* [out] */ Boolean* res);

        CARAPI Replace(
            /* [in] */ PInterface key,
            /* [in] */ PInterface value,
            /* [out] */ PInterface* res);

        /* ----------------  SortedMap API methods -------------- */

        CARAPI GetComparator(
            /* [out] */ IComparator** comp);

        CARAPI GetSubMap(
            /* [in] */ PInterface fromKey,
            /* [in] */ Boolean fromInclusive,
            /* [in] */ PInterface toKey,
            /* [in] */ Boolean toInclusive,
            /* [out] */ INavigableMap** res);

        CARAPI GetHeadMap(
            /* [in] */ PInterface toKey,
            /* [in] */ Boolean inclusive,
            /* [out] */ INavigableMap** res);

        CARAPI GetTailMap(
            /* [in] */ PInterface fromKey,
            /* [in] */ Boolean inclusive,
            /* [out] */ INavigableMap** res);

        CARAPI GetSubMap(
            /* [in] */ PInterface fromKey,
            /* [in] */ PInterface toKey,
            /* [out] */ ISortedMap** res);

        CARAPI GetHeadMap(
            /* [in] */ PInterface toKey,
            /* [out] */ ISortedMap** res);

        CARAPI GetTailMap(
            /* [in] */ PInterface fromKey,
            /* [out] */ ISortedMap** res);

        CARAPI GetDescendingMap(
            /* [out] */ INavigableMap** res);

        /* ----------------  Relational methods -------------- */

        CARAPI GetCeilingEntry(
            /* [in] */ IInterface* key,
            /* [out] */ IMapEntry** outent);

        CARAPI GetCeilingKey(
            /* [in] */ IInterface* key,
            /* [out] */ IInterface** outface);

        CARAPI GetLowerEntry(
            /* [in] */ IInterface* key,
            /* [out] */ IMapEntry** outent);

        CARAPI GetLowerKey(
            /* [in] */ IInterface* key,
            /* [out] */ IInterface** outface);

        CARAPI GetFloorEntry(
            /* [in] */ IInterface* key,
            /* [out] */ IMapEntry** outent);

        CARAPI GetFloorKey(
            /* [in] */ IInterface* key,
            /* [out] */ IInterface** outface);

        CARAPI GetHigherEntry(
            /* [in] */ IInterface* key,
            /* [out] */ IMapEntry** outent);

        CARAPI GetHigherKey(
            /* [in] */ IInterface* key,
            /* [out] */ IInterface** outface);

        CARAPI GetFirstKey(
            /* [out] */ IInterface** outface);

        CARAPI GetLastKey(
            /* [out] */ IInterface** outface);

        CARAPI GetFirstEntry(
            /* [out] */ IMapEntry** outent);

        CARAPI GetLastEntry(
            /* [out] */ IMapEntry** outent);

        CARAPI PollFirstEntry(
            /* [out] */ IMapEntry** outent);

        CARAPI PollLastEntry(
            /* [out] */ IMapEntry** outent);

        /* ---------------- Submap Views -------------- */

        CARAPI GetKeySet(
            /* [out] */ ISet** res);

        CARAPI GetNavigableKeySet(
            /* [out] */ INavigableSet** res);

        CARAPI GetValues(
            /* [out] */ ICollection** value);

        CARAPI GetEntrySet(
            /* [out] */ ISet** entries);

        CARAPI GetDescendingKeySet(
            /* [out] */ INavigableSet** res);

        AutoPtr<IIterator> KeyIterator();

        AutoPtr<IIterator> ValueIterator();

        AutoPtr<IIterator> EntryIterator();

        CARAPI Equals(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* result);

        CARAPI GetHashCode(
            /* [out] */ Int32* hash);

        CARAPI Put(
            /* [in] */ IInterface* key,
            /* [in] */ IInterface* obj);

        CARAPI PutAll(
            /* [in] */ IMap* map);

        CARAPI Remove(
            /* [in] */ IInterface* obj);


    private:
        /* ----------------  Utilities -------------- */

        CARAPI_(Boolean) TooLow(
            /* [in] */ IInterface* key);

        CARAPI_(Boolean) TooHigh(
            /* [in] */ PInterface key);

        CARAPI_(Boolean) InBounds(
            /* [in] */ PInterface key);

        CARAPI CheckKeyBounds(
            /* [in] */ PInterface key);

        CARAPI_(Boolean) IsBeforeEnd(
            /* [in] */ Node* n);

        CARAPI_(AutoPtr<Node>) LoNode();

        CARAPI_(AutoPtr<Node>) HiNode();

        CARAPI_(AutoPtr<IInterface>) LowestKey();

        CARAPI_(AutoPtr<IInterface>) HighestKey();

        CARAPI_(AutoPtr<IMapEntry>) LowestEntry();

        CARAPI_(AutoPtr<IMapEntry>) HighestEntry();

        CARAPI_(AutoPtr<IMapEntry>) RemoveLowest();

        CARAPI_(AutoPtr<IMapEntry>) RemoveHighest();

        CARAPI_(AutoPtr<IMapEntry>) GetNearEntry(
            /* [in] */ PInterface key,
            /* [in] */ Int32 rel);

        CARAPI_(AutoPtr<IInterface>) GetNearKey(
            /* [in] */ PInterface key,
            /* [in] */ Int32 rel);

        CARAPI_(AutoPtr<_SubMap>) NewSubMap(
            /* [in] */ PInterface fromKey,
            /* [in] */ Boolean fromInclusive,
            /* [in] */ PInterface toKey,
            /* [in] */ Boolean toInclusive);

    private:
//        static Int64 serialVersionUID = -7647078645895051609L;

        /** Underlying map */
        AutoPtr<CConcurrentSkipListMap> mM;
        /** lower bound key, or null if from start */
        AutoPtr<IInterface> mLo;
        /** upper bound key, or null if to end */
        AutoPtr<IInterface> mHi;
        /** inclusion flag for lo */
        Boolean mLoInclusive;
        /** inclusion flag for hi */
        Boolean mHiInclusive;
        /** direction */
        Boolean mIsDescending;

        // Lazily initialized view holders
        AutoPtr<_KeySet> mKeySetView;
        AutoPtr<ISet> mEntrySetView;
        AutoPtr<ICollection> mValuesView;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI Initialize();

    CARAPI_(Int32) Compare(
        /* [in] */ PInterface k1,
        /* [in] */ PInterface k2);

    CARAPI_(Boolean) InHalfOpenRange(
        /* [in] */ PInterface key,
        /* [in] */ PInterface least,
        /* [in] */ PInterface fence);

    CARAPI_(Boolean) InOpenRange(
        /* [in] */ PInterface key,
        /* [in] */ PInterface least,
        /* [in] */ PInterface fence);

    /* ---------------- Deletion -------------- */

    CARAPI_(AutoPtr<IInterface>) DoRemove(
        /* [in] */ PInterface okey,
        /* [in] */ PInterface value);

    /* ---------------- Finding and removing first element -------------- */

    CARAPI_(AutoPtr<Node>) FindFirst();

    CARAPI_(AutoPtr<IMapEntry>) DoRemoveFirstEntry();

    /* ---------------- Finding and removing last element -------------- */

    CARAPI_(AutoPtr<Node>) FindLast();

    CARAPI_(AutoPtr<IMapEntry>) DoRemoveLastEntry();

    /* ---------------- Relational operations -------------- */

    // Control values OR'ed as arguments to findNear

    CARAPI_(AutoPtr<Node>) FindNear(
        /* [in] */ PInterface kkey,
        /* [in] */ Int32 rel);

    CARAPI_(AutoPtr<AbstractMap::SimpleImmutableEntry>) GetNear(
        /* [in] */ PInterface key,
        /* [in] */ Int32 rel);

    /* ---------------- Constructors -------------- */

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IComparator* comparator);

    CARAPI constructor(
        /* [in] */ IMap* m);

    CARAPI constructor(
        /* [in] */ ISortedMap* m);

    CARAPI Clone(
        /* [out] */ IInterface** res);

    /* ------ Map API methods ------ */

    CARAPI ContainsKey(
        /* [in] */ IInterface* key,
        /* [out] */ Boolean* result);

    CARAPI Get(
        /* [in] */ IInterface* key,
        /* [out] */ IInterface** value);

    CARAPI Put(
        /* [in] */ IInterface* key,
        /* [in] */ IInterface* value,
        /* [out] */ IInterface** oldValue);

    CARAPI Remove(
        /* [in] */ IInterface* key,
        /* [out] */ IInterface** value);

    CARAPI GetSize(
        /* [out] */ Int32* size);

    CARAPI IsEmpty(
        /* [out] */ Boolean* result);

    CARAPI ContainsValue(
        /* [in] */ IInterface* value,
        /* [out] */ Boolean* result);

    CARAPI Clear();

    /* ---------------- View methods -------------- */

    CARAPI GetKeySet(
        /* [out] */ ISet** res);

    CARAPI GetNavigableKeySet(
        /* [out] */ INavigableSet** res);

    CARAPI GetValues(
        /* [out] */ ICollection** value);

    CARAPI GetEntrySet(
        /* [out] */ ISet** entries);

    CARAPI GetDescendingMap(
        /* [out] */ INavigableMap** res);

    CARAPI GetDescendingKeySet(
        /* [out] */ INavigableSet** res);

    /* ---------------- AbstractMap Overrides -------------- */

    CARAPI Equals(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* result);

    /* ------ ConcurrentMap API methods ------ */

    CARAPI PutIfAbsent(
        /* [in] */ PInterface key,
        /* [in] */ PInterface value,
        /* [out] */ PInterface* out);

    CARAPI Remove(
        /* [in] */ PInterface key,
        /* [in] */ PInterface value,
        /* [out] */ Boolean* res);

    CARAPI Replace(
        /* [in] */ PInterface key,
        /* [in] */ PInterface oldValue,
        /* [in] */ PInterface newValue,
        /* [out] */ Boolean* res);

    CARAPI Replace(
        /* [in] */ PInterface key,
        /* [in] */ PInterface value,
        /* [out] */ PInterface* res);

    /* ------ SortedMap API methods ------ */

    CARAPI GetComparator(
        /* [out] */ IComparator** comp);

    CARAPI GetFirstKey(
        /* [out] */ IInterface** outface);

    CARAPI GetLastKey(
        /* [out] */ IInterface** outface);

    CARAPI GetSubMap(
        /* [in] */ PInterface fromKey,
        /* [in] */ Boolean fromInclusive,
        /* [in] */ PInterface toKey,
        /* [in] */ Boolean toInclusive,
        /* [out] */ INavigableMap** res);

    CARAPI GetHeadMap(
        /* [in] */ PInterface toKey,
        /* [in] */ Boolean inclusive,
        /* [out] */ INavigableMap** res);

    CARAPI GetTailMap(
        /* [in] */ PInterface fromKey,
        /* [in] */ Boolean inclusive,
        /* [out] */ INavigableMap** res);

    CARAPI GetSubMap(
        /* [in] */ PInterface fromKey,
        /* [in] */ PInterface toKey,
        /* [out] */ ISortedMap** res);

    CARAPI GetHeadMap(
        /* [in] */ PInterface toKey,
        /* [out] */ ISortedMap** res);

    CARAPI GetTailMap(
        /* [in] */ PInterface fromKey,
        /* [out] */ ISortedMap** res);

    /* ---------------- Relational operations -------------- */

    CARAPI GetLowerEntry(
        /* [in] */ IInterface* key,
        /* [out] */ IMapEntry** outent);

    CARAPI GetLowerKey(
        /* [in] */ IInterface* key,
        /* [out] */ IInterface** outface);

    CARAPI GetFloorEntry(
        /* [in] */ IInterface* key,
        /* [out] */ IMapEntry** outent);

    CARAPI GetFloorKey(
        /* [in] */ IInterface* key,
        /* [out] */ IInterface** outface);

    CARAPI GetHigherEntry(
        /* [in] */ IInterface* key,
        /* [out] */ IMapEntry** outent);

    CARAPI GetHigherKey(
        /* [in] */ IInterface* key,
        /* [out] */ IInterface** outface);

    CARAPI GetCeilingEntry(
        /* [in] */ IInterface* key,
        /* [out] */ IMapEntry** outent);

    CARAPI GetCeilingKey(
        /* [in] */ IInterface* key,
        /* [out] */ IInterface** outface);

    CARAPI GetFirstEntry(
        /* [out] */ IMapEntry** outent);

    CARAPI GetLastEntry(
        /* [out] */ IMapEntry** outent);

    CARAPI PollFirstEntry(
        /* [out] */ IMapEntry** outent);

    CARAPI PollLastEntry(
        /* [out] */ IMapEntry** outent);

    // Factory methods for iterators needed by ConcurrentSkipListSet etc

    CARAPI_(AutoPtr<IIterator>) KeyIterator();

    CARAPI_(AutoPtr<IIterator>) ValueIterator();

    CARAPI_(AutoPtr<IIterator>) EntryIterator();

    /* ---------------- View Classes -------------- */

    static CARAPI_(AutoPtr<IList>) ToList(
        /* [in] */ ICollection* c);

    CARAPI PutAll(
        /* [in] */ IMap* map);

    CARAPI GetHashCode(
        /* [out] */ Int32* hash);

    CARAPI Put(
        /* [in] */ IInterface* key,
        /* [in] */ IInterface* value);

    CARAPI Remove(
        /* [in] */ IInterface* key);

private:
    CARAPI_(Boolean) CasHead(
        /* [in] */ HeadIndex* cmp,
        /* [in] */ HeadIndex* val);

    CARAPI_(AutoPtr<IComparable>) Comparable(
        /* [in] */ PInterface key);

    /* ---------------- Traversal -------------- */

    CARAPI_(AutoPtr<Node>) FindPredecessor(
        /* [in] */ IComparable* key);

    CARAPI_(AutoPtr<Node>) FindNode(
        /* [in] */ IComparable* key);

    CARAPI_(AutoPtr<IInterface>) DoGet(
        /* [in] */ PInterface okey);

    /* ---------------- Insertion -------------- */

    CARAPI_(AutoPtr<IInterface>) DoPut(
        /* [in] */ PInterface kkey,
        /* [in] */ PInterface value,
        /* [in] */ Boolean onlyIfAbsent);

    CARAPI_(Int32) RandomLevel();

    CARAPI_(void) InsertIndex(
        /* [in] */ Node* z,
        /* [in] */ Int32 level);

    CARAPI_(void) AddIndex(
        /* [in] */ Index* idx,
        /* [in] */ HeadIndex* h,
        /* [in] */ Int32 indexLevel);

    CARAPI_(void) TryReduceLevel();

    CARAPI_(void) ClearIndexToFirst();

    CARAPI_(AutoPtr<Node>) FindPredecessorOfLast();

    CARAPI_(void) BuildFromSorted(
        /* [in] */ ISortedMap* map);
    /* ---------------- Serialization -------------- */

    CARAPI_(void) WriteObject(
        /* [in] */ IObjectOutputStream* s);

    CARAPI_(void) ReadObject(
        /* [in] */ IObjectInputStream* s);

private:
//    static long serialVersionUID = -8627078645895051609L;

    static AutoPtr<IRandom> sSeedGenerator;

    static AutoPtr<IInterface> sBASE_HEADER;

    AutoPtr<HeadIndex> mHead;

    AutoPtr<IComparator> mComparator;

    Int32 mRandomSeed;

    AutoPtr<_KeySet> mKeySet;
    AutoPtr<_EntrySet> mEntrySet;
    AutoPtr<_Values> mValues;
    AutoPtr<INavigableMap> mDescendingMap;

    static Int32 sEQ;
    static Int32 sLT;
    static Int32 sGT; // Actually checked as !LT

    // Unsafe mechanics
    // private static final sun.misc.Unsafe UNSAFE;
    // private static final long headOffset;
    // static {
    //     try {
    //         UNSAFE = sun.misc.Unsafe.getUnsafe();
    //         Class<?> k = ConcurrentSkipListMap.class;
    //         headOffset = UNSAFE.objectFieldOffset
    //             (k.getDeclaredField("head"));
    //     } catch (Exception e) {
    //         throw new Error(e);
    //     }
    // }
};

} // namespace Concurrent
} // namespace Utility
} // namespace Elastos


DEFINE_CONVERSION_FOR(Elastos::Utility::Concurrent::CConcurrentSkipListMap::Index, IInterface)


#endif //__ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CCONCURRENTSKIPLISTMAP_H__
