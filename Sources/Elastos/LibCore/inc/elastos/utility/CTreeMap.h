#ifndef __ELASTOS_UTILITY_CTREEMAP_H__
#define __ELASTOS_UTILITY_CTREEMAP_H__

#include "_Elastos_Utility_CTreeMap.h"
#include "AbstractMap.h"
#include "AbstractSet.h"
#include "MapEntry.h"

using Elastos::IO::IObjectInputStream;
using Elastos::IO::IObjectOutputStream;
using Elastos::IO::IObjectStreamField;
using Elastos::IO::ISerializable;
using Elastos::Core::ICloneable;
using Elastos::Utility::AbstractMap;
using Elastos::Core::IComparator;

namespace Elastos {
namespace Utility {

CarClass(CTreeMap)
    , public AbstractMap
    , public ISortedMap
    , public INavigableMap
    , public ICloneable
    , public ISerializable
    , public ITreeMap
{
public:
    class Node
        : public MapEntry
    {
    public:
        Node(
            /* [in] */ Node* parent,
            /* [in] */ IInterface* key);

        CARAPI_(AutoPtr<Node>) Copy(
            /* [in] */ Node* parent);

        CARAPI Equals(
            /* [in] */ IInterface* entry,
            /* [out] */ Boolean* result);

        CARAPI GetKey(
            /* [out] */ PInterface* key);

        CARAPI GetValue(
            /* [out] */ PInterface* value);

        CARAPI GetHashCode(
            /* [out] */ Int32* hashCode);

        CARAPI SetValue(
            /* [in] */ IInterface* valueReplacer,
            /* [out] */ IInterface** valueReplacee);

        CARAPI ToString(
            /* [out] */ String* str);

        /**
         * Returns the next node in an inorder traversal, or null if this is the
         * last node in the tree.
         */
        CARAPI_(AutoPtr<Node>) GetNext();

        /**
         * Returns the previous node in an inorder traversal, or null if this is
         * the first node in the tree.
         */
        CARAPI_(AutoPtr<Node>) GetPrev();

        /**
         * Returns the first node in this subtree.
         */
        CARAPI_(AutoPtr<Node>) GetFirst();

        /**
         * Returns the last node in this subtree.
         */
        CARAPI_(AutoPtr<Node>) GetLast();

    public:
        AutoPtr<Node> mParent;
        AutoPtr<Node> mLeft;
        AutoPtr<Node> mRight;
        AutoPtr<IInterface> mKey;
        AutoPtr<IInterface> mValue;
        Int32 mHeight;
    };

    /**
     * Walk the nodes of the tree left-to-right or right-to-left. Note that in
     * descending iterations, {@code next} will return the previous node.
     */
    class MapIterator
        : public Object
        , public IIterator
    {
    public:
        MapIterator(
            /* [in] */ Node* next,
            /* [in] */ CTreeMap* host);

        CAR_INTERFACE_DECL()

        CARAPI HasNext(
            /* [out] */ Boolean* result);

        CARAPI Remove();

    protected:
        CARAPI StepForward(
            /* [out] */ Node** outnode);

        CARAPI StepBackward(
            /* [out] */ Node** outnode);

    protected:
        AutoPtr<Node> mNext;
        AutoPtr<Node> mLast;
        Int32 mExpectedModCount;
        CTreeMap* mHost;
    };

    class _KeySetBackwardMapIterator
        : public MapIterator
    {
    public:
        _KeySetBackwardMapIterator(
            /* [in] */ Node* next,
            /* [in] */ CTreeMap* host)
            : MapIterator(next, host){}

        ECode GetNext(
            /* [out] */ IInterface** object)
        {
            VALIDATE_NOT_NULL(object)

            AutoPtr<Node> res;
            FAIL_RETURN(MapIterator::StepBackward((Node**)&res));
            *object = res->mKey;
            REFCOUNT_ADD(*object)
            return NOERROR;
        }
    };

    class _KeySetForwardMapIterator
        : public MapIterator
    {
    public:
        _KeySetForwardMapIterator(
            /* [in] */ Node* next,
            /* [in] */ CTreeMap* host)
            : MapIterator(next, host){}

        ECode GetNext(
            /* [out] */ IInterface** object)
        {
            VALIDATE_NOT_NULL(object)

            AutoPtr<Node> res;
            FAIL_RETURN(MapIterator::StepForward((Node**)&res));
            *object = res->mKey;
            REFCOUNT_ADD(*object)
            return NOERROR;
        }
    };

    class _EntrySetForwardMapIterator
        : public MapIterator
    {
    public:
        _EntrySetForwardMapIterator(
            /* [in] */ Node* next,
            /* [in] */ CTreeMap* host)
            : MapIterator(next, host){}

        ECode GetNext(
            /* [out] */ IInterface** object)
        {
            VALIDATE_NOT_NULL(object)

            AutoPtr<Node> res;
            FAIL_RETURN(MapIterator::StepForward((Node**)&res));
            *object = IMapEntry::Probe(res);
            REFCOUNT_ADD(*object)
            return NOERROR;
        }
    };

    /*
     * View implementations.
     */

    class _EntrySet
        : public AbstractSet
    {
    public:
        _EntrySet(
            /* [in] */ CTreeMap* host);

        CARAPI GetSize(
            /* [out] */ Int32* value);

        CARAPI GetIterator(
            /* [out] */ IIterator** outiter);

        CARAPI Contains(
            /* [in] */ IInterface* o,
            /* [out] */ Boolean* value);

        CARAPI Remove(
            /* [in] */ IInterface* o,
            /* [out] */ Boolean* value);

        CARAPI Clear();

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

        CARAPI Equals(
            /* [in] */ IInterface* object,
            /* [out] */ Boolean* result);

        CARAPI GetHashCode(
            /* [out] */ Int32* hashCode);

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

    private:
        CTreeMap* mHost;
    };

    class _KeySet
        : public AbstractSet
        , public INavigableSet
    {
    public:
        _KeySet(
            /* [in] */ CTreeMap* host);

        CAR_INTERFACE_DECL()

        CARAPI GetComparator(
            /* [out] */ IComparator** outcom);

        CARAPI GetFirst(
            /* [out] */ IInterface** outface);

        CARAPI GetHeadSet(
            /* [in] */ IInterface* end,
            /* [out] */ ISortedSet** outsort);

        CARAPI GetLast(
            /* [out] */ IInterface** outface);

        CARAPI GetSubSet(
            /* [in] */ IInterface* start,
            /* [in] */ IInterface* end,
            /* [out] */ ISortedSet** outsort);

        CARAPI GetTailSet(
            /* [in] */ IInterface* start,
            /* [out] */ ISortedSet** outsort);

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

        CARAPI PollFirst(
            /* [out] */ IInterface** outface);

        CARAPI PollLast(
            /* [out] */ IInterface** outface);

        CARAPI GetDescendingSet(
            /* [out] */ INavigableSet** outnav);

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

        CARAPI GetSize(
            /* [out] */ Int32* value);

        CARAPI GetIterator(
            /* [out] */ IIterator** outiter);

        CARAPI Contains(
            /* [in] */ IInterface* o,
            /* [out] */ Boolean* value);

        CARAPI Remove(
            /* [in] */ IInterface* key,
            /* [out] */ Boolean* value);

        CARAPI Clear();

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

        CARAPI Equals(
            /* [in] */ IInterface* object,
            /* [out] */ Boolean* result);

        CARAPI GetHashCode(
            /* [out] */ Int32* hashCode);

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

        CARAPI Add(
            /* [in] */ IInterface* object);

        CARAPI AddAll(
            /* [in] */ ICollection* collection);

        CARAPI Remove(
            /* [in] */ IInterface* object);

        CARAPI RemoveAll(
            /* [in] */ ICollection* collection);

        CARAPI RetainAll(
            /* [in] */ ICollection* collection);

    private:
        CTreeMap* mHost;
    };

    /*
     * Bounded views implementations.
     */

    enum Bound {
        INCLUSIVE,
        // {
        //     @Override public String leftCap(Object from) {
        //         return "[" + from;
        //     }
        //     @Override public String rightCap(Object to) {
        //         return to + "]";
        //     }
        // },
        EXCLUSIVE,
        // {
        //     @Override public String leftCap(Object from) {
        //         return "(" + from;
        //     }
        //     @Override public String rightCap(Object to) {
        //         return to + ")";
        //     }
        // },
        NO_BOUND,
        // {
        //     @Override public String leftCap(Object from) {
        //         return ".";
        //     }
        //     @Override public String rightCap(Object to) {
        //         return ".";
        //     }
        // };

        // public abstract String leftCap(Object from);
        // public abstract String rightCap(Object to);
    };

   /*
     * AVL methods
     */

    enum Relation {
        LOWER,
        FLOOR,
        EQUAL,
        CREATE,
        CEILING,
        HIGHER,
        ILLEGAL,

        /**
         * Returns a possibly-flipped relation for use in descending views.
         *
         * @param ascending false to flip; true to return this.
         */
        // Relation forOrder(boolean ascending) {
        //     if (ascending) {
        //         return this;
        //     }

        //     switch (this) {
        //         case LOWER:
        //             return HIGHER;
        //         case FLOOR:
        //             return CEILING;
        //         case EQUAL:
        //             return EQUAL;
        //         case CEILING:
        //             return FLOOR;
        //         case HIGHER:
        //             return LOWER;
        //         default:
        //             throw new IllegalStateException();
        //     }
        // }
    };

    /**
     * A map with optional limits on its range.
     */
    class BoundedMap
        : public AbstractMap
        , public ISortedMap
        , public INavigableMap
        , public ISerializable
    {
    public:
        /*
         * Bounded view implementations.
         */

        class BoundedIterator
            : public MapIterator
        {
        protected:
            BoundedIterator(
                /* [in] */ Node* next,
                /* [in] */ BoundedMap* host);

            CARAPI StepForward(
                /* [out] */ Node** outnode);

            CARAPI StepBackward(
                /* [out] */ Node** outnode);

        protected:
            AutoPtr<BoundedMap> mBoundParent;
        };

        class BoundedEntrySet
            : public AbstractSet
        {
        private:
            class BoundedEntrySetIterator
                : public BoundedIterator
            {
            public:
                BoundedEntrySetIterator(
                    /* [in] */ Node* next,
                    /* [in] */ BoundedMap* host);

                CARAPI GetNext(
                    /* [out] */ IInterface** object);
            };

        public:
            BoundedEntrySet(
                /* [in] */ BoundedMap* host);

            CARAPI GetSize(
                /* [out] */ Int32* value);

            CARAPI IsEmpty(
                /* [out] */ Boolean* value);

            CARAPI GetIterator(
                /* [out] */ IIterator** outiter);

            CARAPI Contains(
                /* [in] */ IInterface* o,
                /* [out] */ Boolean* value);

            CARAPI Remove(
                /* [in] */ IInterface* o,
                /* [out] */ Boolean* value);

            CARAPI Clear();

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
            BoundedMap* mHost;
        };

        class BoundedKeySet
            : public AbstractSet
            , public INavigableSet
        {
        private:
            class BoundedKeySetIterator
                : public BoundedIterator
            {
            public:
                BoundedKeySetIterator(
                    /* [in] */ Node* next,
                    /* [in] */ BoundedMap* host);

                CARAPI GetNext(
                    /* [out] */ IInterface** object);
            };

            class BoundedKeySetDescendingIterator
                : public BoundedIterator
            {
            public:
                BoundedKeySetDescendingIterator(
                    /* [in] */ Node* next,
                    /* [in] */ BoundedMap* host);

                CARAPI GetNext(
                    /* [out] */ IInterface** object);
            };

        public:
            BoundedKeySet(
                /* [in] */ BoundedMap* host);

            CAR_INTERFACE_DECL()

            CARAPI GetComparator(
                /* [out] */ IComparator** outcom);

            CARAPI GetFirst(
                /* [out] */ IInterface** outface);

            CARAPI GetHeadSet(
                /* [in] */ IInterface* end,
                /* [out] */ ISortedSet** outsort);

            CARAPI GetLast(
                /* [out] */ IInterface** outface);

            CARAPI GetSubSet(
                /* [in] */ IInterface* start,
                /* [in] */ IInterface* end,
                /* [out] */ ISortedSet** outsort);

            CARAPI GetTailSet(
                /* [in] */ IInterface* start,
                /* [out] */ ISortedSet** outsort);

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

            CARAPI PollFirst(
                /* [out] */ IInterface** outface);

            CARAPI PollLast(
                /* [out] */ IInterface** outface);

            CARAPI GetDescendingSet(
                /* [out] */ INavigableSet** outnav);

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

            CARAPI GetSize(
                /* [out] */ Int32* value);

            CARAPI IsEmpty(
                /* [out] */ Boolean* value);

            CARAPI GetIterator(
                /* [out] */ IIterator** outiter);

            CARAPI Contains(
                /* [in] */ IInterface* o,
                /* [out] */ Boolean* value);

            CARAPI Remove(
                /* [in] */ IInterface* o,
                /* [out] */ Boolean* value);

            CARAPI Clear();

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

            CARAPI Add(
                /* [in] */ IInterface* object);

            CARAPI AddAll(
                /* [in] */ ICollection* collection);

            CARAPI Remove(
                /* [in] */ IInterface* object);

            CARAPI RemoveAll(
                /* [in] */ ICollection* collection);

            CARAPI RetainAll(
                /* [in] */ ICollection* collection);

        private:
            BoundedMap* mHost;
        };

    public:
        CAR_INTERFACE_DECL()

        CARAPI constructor(
            /* [in] */ Boolean ascending,
            /* [in] */ IInterface* from,
            /* [in] */ Bound fromBound,
            /* [in] */ IInterface* to,
            /* [in] */ Bound toBound,
            /* [in] */ CTreeMap* host);

        CARAPI Clear();

        CARAPI ContainsKey(
            /* [in] */ IInterface* key,
            /* [out] */ Boolean* result);

        CARAPI ContainsValue(
            /* [in] */ IInterface* value,
            /* [out] */ Boolean* result);

        CARAPI GetEntrySet(
            /* [out] */ ISet** entries);

        CARAPI Equals(
            /* [in] */ IInterface* object,
            /* [out] */ Boolean* result);

        CARAPI Get(
            /* [in] */ PInterface key,
            /* [out] */ PInterface* value);

        CARAPI GetHashCode(
            /* [out] */ Int32* hashCode);

        CARAPI IsEmpty(
            /* [out] */ Boolean* result);

        CARAPI GetKeySet(
            /* [out] */ ISet** keySet);

        CARAPI Put(
            /* [in] */ PInterface key,
            /* [in] */ PInterface value,
            /* [out] */ PInterface* oldValue);

        CARAPI Put(
            /* [in] */ PInterface key,
            /* [in] */ PInterface value);

        CARAPI PutAll(
            /* [in] */ IMap* map);

        CARAPI Remove(
            /* [in] */ PInterface key,
            /* [out] */ PInterface* value);

        CARAPI Remove(
            /* [in] */ PInterface key);

        CARAPI GetSize(
            /* [out] */ Int32* size);

        CARAPI GetValues(
            /* [out] */ ICollection** value);

        CARAPI GetComparator(
            /* [out] */ IComparator** comp);

        CARAPI GetFirstKey(
            /* [out] */ IInterface** outface);

        CARAPI GetHeadMap(
            /* [in] */ IInterface* endKey,
            /* [out] */ ISortedMap** sortmap);

        CARAPI GetLastKey(
            /* [out] */ IInterface** outface);

        CARAPI GetSubMap(
            /* [in] */ IInterface* startKey,
            /* [in] */ IInterface* endKey,
            /* [out] */ ISortedMap** sortmap);

        CARAPI GetTailMap(
            /* [in] */ IInterface* startKey,
            /* [out] */ ISortedMap** sortmap);

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

        CARAPI GetCeilingEntry(
            /* [in] */ IInterface* key,
            /* [out] */ IMapEntry** outent);

        CARAPI GetCeilingKey(
            /* [in] */ IInterface* key,
            /* [out] */ IInterface** outface);

        CARAPI GetHigherEntry(
            /* [in] */ IInterface* key,
            /* [out] */ IMapEntry** outent);

        CARAPI GetHigherKey(
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

        CARAPI GetDescendingMap(
            /* [out] */ INavigableMap** outnav);

        CARAPI GetNavigableKeySet(
            /* [out] */ INavigableSet** outnav);

        CARAPI GetDescendingKeySet(
            /* [out] */ INavigableSet** outnav);

        CARAPI GetSubMap(
            /* [in] */ IInterface* fromKey,
            /* [in] */ Boolean fromInclusive,
            /* [in] */ IInterface* toKey,
            /* [in] */ Boolean toInclusive,
            /* [out] */ INavigableMap** outnav);

        CARAPI GetHeadMap(
            /* [in] */ IInterface* toKey,
            /* [in] */ Boolean inclusive,
            /* [out] */ INavigableMap** outnav);

        CARAPI GetTailMap(
            /* [in] */ IInterface* fromKey,
            /* [in] */ Boolean inclusive,
            /* [out] */ INavigableMap** outnav);

        CARAPI_(AutoPtr<IInterface>) WriteReplace();

    private:
        /**
         * Returns true if the key is in bounds.
         */
        // @SuppressWarnings("unchecked") // this method throws ClassCastExceptions!
        CARAPI_(Boolean) IsInBounds(
            /* [in] */ IInterface* key);

        /**
         * Returns true if the key is in bounds. Use this overload with
         * NO_BOUND to skip bounds checking on either end.
         */
        CARAPI_(Boolean) IsInBounds(
            /* [in] */ IInterface* key,
            /* [in] */ Bound fromBound,
            /* [in] */ Bound toBound);

        /**
         * Returns the entry if it is in bounds, or null if it is out of bounds.
         */
        CARAPI_(AutoPtr<Node>) GetBound(
            /* [in] */ Node* node,
            /* [in] */ Bound fromBound,
            /* [in] */ Bound toBound);

        /**
         * @param first true for the first element, false for the last.
         */
        CARAPI_(AutoPtr<Node>) Endpoint(
            /* [in] */ Boolean first);

        /**
         * Performs a find on the underlying tree after constraining it to the
         * bounds of this view. Examples:
         *
         *   bound is (A..C)
         *   findBounded(B, FLOOR) stays source.find(B, FLOOR)
         *
         *   bound is (A..C)
         *   findBounded(C, FLOOR) becomes source.find(C, LOWER)
         *
         *   bound is (A..C)
         *   findBounded(D, LOWER) becomes source.find(C, LOWER)
         *
         *   bound is (A..C]
         *   findBounded(D, FLOOR) becomes source.find(C, FLOOR)
         *
         *   bound is (A..C]
         *   findBounded(D, LOWER) becomes source.find(C, FLOOR)
         */
        CARAPI_(AutoPtr<IMapEntry>) FindBounded(
            /* [in] */ IInterface* key,
            /* [in] */ Relation relation);

        CARAPI SubMap(
            /* [in] */ IInterface* from,
            /* [in] */ Bound fromBound,
            /* [in] */ IInterface* to,
            /* [in] */ Bound toBound,
            /* [out] */ INavigableMap** outnav);

        CARAPI OutOfBounds(
            /* [in] */ IInterface* value,
            /* [in] */ Bound fromBound,
            /* [in] */ Bound toBound);

    private:
        Boolean mAscending;
        AutoPtr<IInterface> mFrom;
        Bound mFromBound;
        AutoPtr<IInterface> mTo;
        Bound mToBound;
        AutoPtr<BoundedEntrySet> mEntrySet;
        AutoPtr<BoundedKeySet> mKeySet;
        AutoPtr<CTreeMap> mHost;
    };

    class NavigableSubMap
        : public AbstractMap
        , public ISerializable
    {
    public:
        NavigableSubMap(
            /* [in] */ CTreeMap* delegate,
            /* [in] */ IInterface* from,
            /* [in] */ Bound fromBound,
            /* [in] */ IInterface* to,
            /* [in] */ Bound toBound,
            /* [in] */ Boolean isDescending);

        CAR_INTERFACE_DECL()

        CARAPI GetEntrySet(
            /* [out] */ ISet** entries);

        CARAPI Clear();

        CARAPI ContainsKey(
            /* [in] */ IInterface* key,
            /* [out] */ Boolean* result);

        CARAPI ContainsValue(
            /* [in] */ IInterface* value,
            /* [out] */ Boolean* result);

        CARAPI Equals(
            /* [in] */ IInterface* object,
            /* [out] */ Boolean* result);

        CARAPI Get(
            /* [in] */ PInterface key,
            /* [out] */ PInterface* value);

        CARAPI GetHashCode(
            /* [out] */ Int32* hashCode);

        CARAPI IsEmpty(
            /* [out] */ Boolean* result);

        CARAPI GetKeySet(
            /* [out] */ ISet** keySet);

        CARAPI Put(
            /* [in] */ PInterface key,
            /* [in] */ PInterface value,
            /* [out] */ PInterface* oldValue);

        CARAPI  PutAll(
            /* [in] */ IMap* map);

        CARAPI Remove(
            /* [in] */ PInterface key,
            /* [out] */ PInterface* value);

        CARAPI GetSize(
            /* [out] */ Int32* size);

        CARAPI GetValues(
            /* [out] */ ICollection** value);

    protected:
        // @SuppressWarnings("unchecked") // we have to trust that the bounds are Ks
        CARAPI ReadResolve(
            /* [out] */ IInterface** outface);

    public:
        AutoPtr<CTreeMap> mM;
        AutoPtr<IInterface> mLo;
        AutoPtr<IInterface> mHi;
        Boolean mFromStart;
        Boolean mToEnd;
        Boolean mLoInclusive;
        Boolean mHiInclusive;

    private:
        Boolean mIsDescending;
    };

    class DescendingSubMap
        : public NavigableSubMap
    {
    public:
        DescendingSubMap(
            /* [in] */ CTreeMap* delegate,
            /* [in] */ IInterface* from,
            /* [in] */ Bound fromBound,
            /* [in] */ IInterface* to,
            /* [in] */ Bound toBound);

    public:
        AutoPtr<IComparator> mReverseComparator;
    };

    class AscendingSubMap
        : public NavigableSubMap
    {
    public:
        AscendingSubMap(
            /* [in] */ CTreeMap* delegate,
            /* [in] */ IInterface* from,
            /* [in] */ Bound fromBound,
            /* [in] */ IInterface* to,
            /* [in] */ Bound toBound);
    };

    class SubMap
        : public AbstractMap
        , public ISerializable
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI GetEntrySet(
            /* [out] */ ISet** entries);

        CARAPI Clear();

        CARAPI ContainsKey(
            /* [in] */ IInterface* key,
            /* [out] */ Boolean* result);

        CARAPI ContainsValue(
            /* [in] */ IInterface* value,
            /* [out] */ Boolean* result);

        CARAPI Equals(
            /* [in] */ IInterface* object,
            /* [out] */ Boolean* result);

        CARAPI Get(
            /* [in] */ PInterface key,
            /* [out] */ PInterface* value);

        CARAPI GetHashCode(
            /* [out] */ Int32* hashCode);

        CARAPI IsEmpty(
            /* [out] */ Boolean* result);

        CARAPI GetKeySet(
            /* [out] */ ISet** keySet);

        CARAPI Put(
            /* [in] */ PInterface key,
            /* [in] */ PInterface value,
            /* [out] */ PInterface* oldValue);

        CARAPI  PutAll(
            /* [in] */ IMap* map);

        CARAPI Remove(
            /* [in] */ PInterface key,
            /* [out] */ PInterface* value);

        CARAPI GetSize(
            /* [out] */ Int32* size);

        CARAPI GetValues(
            /* [out] */ ICollection** value);

        // @SuppressWarnings("unchecked") // we have to trust that the bounds are Ks
    protected:
        CARAPI ReadResolve(
            /* [out] */ IInterface** outface);

    public:
        AutoPtr<IInterface> mFromKey;
        AutoPtr<IInterface> mToKey;
        Boolean mFromStart;
        Boolean mToEnd;

    private:
        AutoPtr<CTreeMap> mHost;
    };

private:
    class OrderComparator
        : public Object
        , public IComparator
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI Compare(
            /* [in] */ IInterface* lhs,
            /* [in] */ IInterface* rhs,
            /* [out] */ Int32* result);
    };

public:
    CTreeMap();

    CAR_INTERFACE_DECL()

    /**
     * Create a natural order, empty tree map whose keys must be mutually
     * comparable and non-null.
     */
    // @SuppressWarnings("unchecked") // unsafe! this assumes K is comparable
    CARAPI constructor();

    /**
     * Create a natural order tree map populated with the key/value pairs of
     * {@code copyFrom}. This map's keys must be mutually comparable and
     * non-null.
     *
     * <p>Even if {@code copyFrom} is a {@code SortedMap}, the constructed map
     * <strong>will not</strong> use {@code copyFrom}'s ordering. This
     * constructor always creates a naturally-ordered map. Because the {@code
     * TreeMap} constructor overloads are ambiguous, prefer to construct a map
     * and populate it in two steps: <pre>   {@code
     *   TreeMap<String, Integer> customOrderedMap
     *       = new TreeMap<String, Integer>(copyFrom.comparator());
     *   customOrderedMap.putAll(copyFrom);
     * }</pre>
     */
    CARAPI constructor(
        /* [in] */ IMap* copyFrommap);

    /**
     * Create a tree map ordered by {@code comparator}. This map's keys may only
     * be null if {@code comparator} permits.
     *
     * @param comparator the comparator to order elements with, or {@code null} to use the natural
     * ordering.
     */
    // @SuppressWarnings("unchecked") // unsafe! if comparator is null, this assumes K is comparable
    CARAPI constructor(
        /* [in] */ IComparator* comparator);

    /**
     * Create a tree map with the ordering and key/value pairs of {@code
     * copyFrom}. This map's keys may only be null if the {@code copyFrom}'s
     * ordering permits.
     *
     * <p>The constructed map <strong>will always use</strong> {@code
     * copyFrom}'s ordering. Because the {@code TreeMap} constructor overloads
     * are ambiguous, prefer to construct a map and populate it in two steps:
     * <pre>   {@code
     *   TreeMap<String, Integer> customOrderedMap
     *       = new TreeMap<String, Integer>(copyFrom.comparator());
     *   customOrderedMap.putAll(copyFrom);
     * }</pre>
     */
    // @SuppressWarnings("unchecked") // if copyFrom's keys are comparable this map's keys must be also
    CARAPI constructor(
        /* [in] */ ISortedMap* copyFrom);

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

    /**
     * Returns the comparator used to compare keys in this sorted map.
     *
     * @return the comparator or {@code null} if the natural order is used.
     */
    CARAPI GetComparator(
        /* [out] */ IComparator** comp);

    /**
     * Returns the first key in this sorted map.
     *
     * @return the first key in this sorted map.
     * @throws NoSuchElementException
     *                if this sorted map is empty.
     */
    CARAPI GetFirstKey(
        /* [out] */ IInterface** outface);

    /**
     * Returns a sorted map over a range of this sorted map with all keys that
     * are less than the specified {@code endKey}. Changes to the returned
     * sorted map are reflected in this sorted map and vice versa.
     * <p>
     * Note: The returned map will not allow an insertion of a key outside the
     * specified range.
     *
     * @param endKey
     *            the high boundary of the range specified.
     * @return a sorted map where the keys are less than {@code endKey}.
     * @throws ClassCastException
     *             if the class of the end key is inappropriate for this sorted
     *             map.
     * @throws NullPointerException
     *             if the end key is {@code null} and this sorted map does not
     *             support {@code null} keys.
     * @throws IllegalArgumentException
     *             if this map is itself a sorted map over a range of another
     *             map and the specified key is outside of its range.
     */
    CARAPI GetHeadMap(
        /* [in] */ IInterface* endKey,
        /* [out] */ ISortedMap** sortmap);

    /**
     * Returns the last key in this sorted map.
     *
     * @return the last key in this sorted map.
     * @throws NoSuchElementException
     *                if this sorted map is empty.
     */
    CARAPI GetLastKey(
        /* [out] */ IInterface** outface);

    /**
     * Returns a sorted map over a range of this sorted map with all keys
     * greater than or equal to the specified {@code startKey} and less than the
     * specified {@code endKey}. Changes to the returned sorted map are
     * reflected in this sorted map and vice versa.
     * <p>
     * Note: The returned map will not allow an insertion of a key outside the
     * specified range.
     *
     * @param startKey
     *            the low boundary of the range (inclusive).
     * @param endKey
     *            the high boundary of the range (exclusive),
     * @return a sorted map with the key from the specified range.
     * @throws ClassCastException
     *             if the class of the start or end key is inappropriate for
     *             this sorted map.
     * @throws NullPointerException
     *             if the start or end key is {@code null} and this sorted map
     *             does not support {@code null} keys.
     * @throws IllegalArgumentException
     *             if the start key is greater than the end key, or if this map
     *             is itself a sorted map over a range of another sorted map and
     *             the specified range is outside of its range.
     */
    CARAPI GetSubMap(
        /* [in] */ IInterface* startKey,
        /* [in] */ IInterface* endKey,
        /* [out] */ ISortedMap** sortmap);

    /**
     * Returns a sorted map over a range of this sorted map with all keys that
     * are greater than or equal to the specified {@code startKey}. Changes to
     * the returned sorted map are reflected in this sorted map and vice versa.
     * <p>
     * Note: The returned map will not allow an insertion of a key outside the
     * specified range.
     *
     * @param startKey
     *            the low boundary of the range specified.
     * @return a sorted map where the keys are greater or equal to
     *         {@code startKey}.
     * @throws ClassCastException
     *             if the class of the start key is inappropriate for this
     *             sorted map.
     * @throws NullPointerException
     *             if the start key is {@code null} and this sorted map does not
     *             support {@code null} keys.
     * @throws IllegalArgumentException
     *             if this map itself a sorted map over a range of another map
     *             and the specified key is outside of its range.
     */
    CARAPI GetTailMap(
        /* [in] */ IInterface* startKey,
        /* [out] */ ISortedMap** sortmap);

    /**
     * Returns a key-value mapping associated with the greatest key
     * strictly less than the given key, or {@code null} if there is
     * no such key.
     *
     * @param key the key
     * @return an entry with the greatest key less than {@code key},
     *         or {@code null} if there is no such key
     * @throws ClassCastException if the specified key cannot be compared
     *         with the keys currently in the map
     * @throws NullPointerException if the specified key is null
     *         and this map does not permit null keys
     */
    CARAPI GetLowerEntry(
        /* [in] */ IInterface* key,
        /* [out] */ IMapEntry** outent);

    /**
     * Returns the greatest key strictly less than the given key, or
     * {@code null} if there is no such key.
     *
     * @param key the key
     * @return the greatest key less than {@code key},
     *         or {@code null} if there is no such key
     * @throws ClassCastException if the specified key cannot be compared
     *         with the keys currently in the map
     * @throws NullPointerException if the specified key is null
     *         and this map does not permit null keys
     */
    CARAPI GetLowerKey(
        /* [in] */ IInterface* key,
        /* [out] */ IInterface** outface);

    /**
     * Returns a key-value mapping associated with the greatest key
     * less than or equal to the given key, or {@code null} if there
     * is no such key.
     *
     * @param key the key
     * @return an entry with the greatest key less than or equal to
     *         {@code key}, or {@code null} if there is no such key
     * @throws ClassCastException if the specified key cannot be compared
     *         with the keys currently in the map
     * @throws NullPointerException if the specified key is null
     *         and this map does not permit null keys
     */
    CARAPI GetFloorEntry(
        /* [in] */ IInterface* key,
        /* [out] */ IMapEntry** outent);

    /**
     * Returns the greatest key less than or equal to the given key,
     * or {@code null} if there is no such key.
     *
     * @param key the key
     * @return the greatest key less than or equal to {@code key},
     *         or {@code null} if there is no such key
     * @throws ClassCastException if the specified key cannot be compared
     *         with the keys currently in the map
     * @throws NullPointerException if the specified key is null
     *         and this map does not permit null keys
     */
    CARAPI GetFloorKey(
        /* [in] */ IInterface* key,
        /* [out] */ IInterface** outface);

    /**
     * Returns a key-value mapping associated with the least key
     * greater than or equal to the given key, or {@code null} if
     * there is no such key.
     *
     * @param key the key
     * @return an entry with the least key greater than or equal to
     *         {@code key}, or {@code null} if there is no such key
     * @throws ClassCastException if the specified key cannot be compared
     *         with the keys currently in the map
     * @throws NullPointerException if the specified key is null
     *         and this map does not permit null keys
     */
    CARAPI GetCeilingEntry(
        /* [in] */ IInterface* key,
        /* [out] */ IMapEntry** outent);

    /**
     * Returns the least key greater than or equal to the given key,
     * or {@code null} if there is no such key.
     *
     * @param key the key
     * @return the least key greater than or equal to {@code key},
     *         or {@code null} if there is no such key
     * @throws ClassCastException if the specified key cannot be compared
     *         with the keys currently in the map
     * @throws NullPointerException if the specified key is null
     *         and this map does not permit null keys
     */
    CARAPI GetCeilingKey(
        /* [in] */ IInterface* key,
        /* [out] */ IInterface** outface);

    /**
     * Returns a key-value mapping associated with the least key
     * strictly greater than the given key, or {@code null} if there
     * is no such key.
     *
     * @param key the key
     * @return an entry with the least key greater than {@code key},
     *         or {@code null} if there is no such key
     * @throws ClassCastException if the specified key cannot be compared
     *         with the keys currently in the map
     * @throws NullPointerException if the specified key is null
     *         and this map does not permit null keys
     */
    CARAPI GetHigherEntry(
        /* [in] */ IInterface* key,
        /* [out] */ IMapEntry** outent);

    /**
     * Returns the least key strictly greater than the given key, or
     * {@code null} if there is no such key.
     *
     * @param key the key
     * @return the least key greater than {@code key},
     *         or {@code null} if there is no such key
     * @throws ClassCastException if the specified key cannot be compared
     *         with the keys currently in the map
     * @throws NullPointerException if the specified key is null
     *         and this map does not permit null keys
     */
    CARAPI GetHigherKey(
        /* [in] */ IInterface* key,
        /* [out] */ IInterface** outface);

    /**
     * Returns a key-value mapping associated with the least
     * key in this map, or {@code null} if the map is empty.
     *
     * @return an entry with the least key,
     *         or {@code null} if this map is empty
     */
    CARAPI GetFirstEntry(
        /* [out] */ IMapEntry** outent);

    /**
     * Returns a key-value mapping associated with the greatest
     * key in this map, or {@code null} if the map is empty.
     *
     * @return an entry with the greatest key,
     *         or {@code null} if this map is empty
     */
    CARAPI GetLastEntry(
        /* [out] */ IMapEntry** outent);

    /**
     * Removes and returns a key-value mapping associated with
     * the least key in this map, or {@code null} if the map is empty.
     *
     * @return the removed first entry of this map,
     *         or {@code null} if this map is empty
     */
    CARAPI PollFirstEntry(
        /* [out] */ IMapEntry** outent);

    /**
     * Removes and returns a key-value mapping associated with
     * the greatest key in this map, or {@code null} if the map is empty.
     *
     * @return the removed last entry of this map,
     *         or {@code null} if this map is empty
     */
    CARAPI PollLastEntry(
        /* [out] */ IMapEntry** outent);

    /**
     * Returns a reverse order view of the mappings contained in this map.
     * The descending map is backed by this map, so changes to the map are
     * reflected in the descending map, and vice-versa.  If either map is
     * modified while an iteration over a collection view of either map
     * is in progress (except through the iterator's own {@code remove}
     * operation), the results of the iteration are undefined.
     *
     * <p>The returned map has an ordering equivalent to
     * <tt>{@link Collections#reverseOrder(Comparator) Collections.reverseOrder}(comparator())</tt>.
     * The expression {@code m.descendingMap().descendingMap()} returns a
     * view of {@code m} essentially equivalent to {@code m}.
     *
     * @return a reverse order view of this map
     */
    CARAPI GetDescendingMap(
        /* [out] */ INavigableMap** outnav);

    /**
     * Returns a {@link NavigableSet} view of the keys contained in this map.
     * The set's iterator returns the keys in ascending order.
     * The set is backed by the map, so changes to the map are reflected in
     * the set, and vice-versa.  If the map is modified while an iteration
     * over the set is in progress (except through the iterator's own {@code
     * remove} operation), the results of the iteration are undefined.  The
     * set supports element removal, which removes the corresponding mapping
     * from the map, via the {@code Iterator.remove}, {@code Set.remove},
     * {@code removeAll}, {@code retainAll}, and {@code clear} operations.
     * It does not support the {@code add} or {@code addAll} operations.
     *
     * @return a navigable set view of the keys in this map
     */
    CARAPI GetNavigableKeySet(
        /* [out] */ INavigableSet** outnav);

    /**
     * Returns a reverse order {@link NavigableSet} view of the keys contained in this map.
     * The set's iterator returns the keys in descending order.
     * The set is backed by the map, so changes to the map are reflected in
     * the set, and vice-versa.  If the map is modified while an iteration
     * over the set is in progress (except through the iterator's own {@code
     * remove} operation), the results of the iteration are undefined.  The
     * set supports element removal, which removes the corresponding mapping
     * from the map, via the {@code Iterator.remove}, {@code Set.remove},
     * {@code removeAll}, {@code retainAll}, and {@code clear} operations.
     * It does not support the {@code add} or {@code addAll} operations.
     *
     * @return a reverse order navigable set view of the keys in this map
     */
    CARAPI GetDescendingKeySet(
        /* [out] */ INavigableSet** outnav);

    /**
     * Returns a view of the portion of this map whose keys range from
     * {@code fromKey} to {@code toKey}.  If {@code fromKey} and
     * {@code toKey} are equal, the returned map is empty unless
     * {@code fromExclusive} and {@code toExclusive} are both true.  The
     * returned map is backed by this map, so changes in the returned map are
     * reflected in this map, and vice-versa.  The returned map supports all
     * optional map operations that this map supports.
     *
     * <p>The returned map will throw an {@code IllegalArgumentException}
     * on an attempt to insert a key outside of its range, or to construct a
     * submap either of whose endpoints lie outside its range.
     *
     * @param fromKey low endpoint of the keys in the returned map
     * @param fromInclusive {@code true} if the low endpoint
     *        is to be included in the returned view
     * @param toKey high endpoint of the keys in the returned map
     * @param toInclusive {@code true} if the high endpoint
     *        is to be included in the returned view
     * @return a view of the portion of this map whose keys range from
     *         {@code fromKey} to {@code toKey}
     * @throws ClassCastException if {@code fromKey} and {@code toKey}
     *         cannot be compared to one another using this map's comparator
     *         (or, if the map has no comparator, using natural ordering).
     *         Implementations may, but are not required to, throw this
     *         exception if {@code fromKey} or {@code toKey}
     *         cannot be compared to keys currently in the map.
     * @throws NullPointerException if {@code fromKey} or {@code toKey}
     *         is null and this map does not permit null keys
     * @throws IllegalArgumentException if {@code fromKey} is greater than
     *         {@code toKey}; or if this map itself has a restricted
     *         range, and {@code fromKey} or {@code toKey} lies
     *         outside the bounds of the range
     */
    CARAPI GetSubMap(
        /* [in] */ IInterface* fromKey,
        /* [in] */ Boolean fromInclusive,
        /* [in] */ IInterface* toKey,
        /* [in] */ Boolean toInclusive,
        /* [out] */ INavigableMap** outnav);

    /**
     * Returns a view of the portion of this map whose keys are less than (or
     * equal to, if {@code inclusive} is true) {@code toKey}.  The returned
     * map is backed by this map, so changes in the returned map are reflected
     * in this map, and vice-versa.  The returned map supports all optional
     * map operations that this map supports.
     *
     * <p>The returned map will throw an {@code IllegalArgumentException}
     * on an attempt to insert a key outside its range.
     *
     * @param toKey high endpoint of the keys in the returned map
     * @param inclusive {@code true} if the high endpoint
     *        is to be included in the returned view
     * @return a view of the portion of this map whose keys are less than
     *         (or equal to, if {@code inclusive} is true) {@code toKey}
     * @throws ClassCastException if {@code toKey} is not compatible
     *         with this map's comparator (or, if the map has no comparator,
     *         if {@code toKey} does not implement {@link Comparable}).
     *         Implementations may, but are not required to, throw this
     *         exception if {@code toKey} cannot be compared to keys
     *         currently in the map.
     * @throws NullPointerException if {@code toKey} is null
     *         and this map does not permit null keys
     * @throws IllegalArgumentException if this map itself has a
     *         restricted range, and {@code toKey} lies outside the
     *         bounds of the range
     */
    CARAPI GetHeadMap(
        /* [in] */ IInterface* toKey,
        /* [in] */ Boolean inclusive,
        /* [out] */ INavigableMap** outnav);

    /**
     * Returns a view of the portion of this map whose keys are greater than (or
     * equal to, if {@code inclusive} is true) {@code fromKey}.  The returned
     * map is backed by this map, so changes in the returned map are reflected
     * in this map, and vice-versa.  The returned map supports all optional
     * map operations that this map supports.
     *
     * <p>The returned map will throw an {@code IllegalArgumentException}
     * on an attempt to insert a key outside its range.
     *
     * @param fromKey low endpoint of the keys in the returned map
     * @param inclusive {@code true} if the low endpoint
     *        is to be included in the returned view
     * @return a view of the portion of this map whose keys are greater than
     *         (or equal to, if {@code inclusive} is true) {@code fromKey}
     * @throws ClassCastException if {@code fromKey} is not compatible
     *         with this map's comparator (or, if the map has no comparator,
     *         if {@code fromKey} does not implement {@link Comparable}).
     *         Implementations may, but are not required to, throw this
     *         exception if {@code fromKey} cannot be compared to keys
     *         currently in the map.
     * @throws NullPointerException if {@code fromKey} is null
     *         and this map does not permit null keys
     * @throws IllegalArgumentException if this map itself has a
     *         restricted range, and {@code fromKey} lies outside the
     *         bounds of the range
     */
    CARAPI GetTailMap(
        /* [in] */ IInterface* fromKey,
        /* [in] */ Boolean inclusive,
        /* [out] */ INavigableMap** outnav);

    CARAPI Clone(
        /* [out] */ IInterface** object);

    /**
     * Returns a possibly-flipped relation for use in descending views.
     *
     * @param ascending false to flip; true to return this.
     */
    CARAPI_(Relation) ForOrder(
        /* [in] */ Boolean ascending,
        /* [in] */ Relation src);

    CARAPI PutInternal(
        /* [in] */ IInterface* key,
        /* [in] */ IInterface* value,
        /* [out] */ IInterface** object);

    /**
     * Returns the node at or adjacent to the given key, creating it if requested.
     *
     * @throws ClassCastException if {@code key} and the tree's keys aren't mutually comparable.
     */
    CARAPI Find(
        /* [in] */ IInterface* key,
        /* [in] */ Relation relation,
        /* [out] */ Node ** node);

    // @SuppressWarnings("unchecked") // this method throws ClassCastExceptions!
    CARAPI FindByObject(
        /* [in] */ IInterface* key,
        /* [out] */ Node** node);

    /**
     * Returns this map's entry that has the same key and value as {@code
     * entry}, or null if this map has no such entry.
     *
     * <p>This method uses the comparator for key equality rather than {@code
     * equals}. If this map's comparator isn't consistent with equals (such as
     * {@code String.CASE_INSENSITIVE_ORDER}), then {@code remove()} and {@code
     * contains()} will violate the collections API.
     */
    CARAPI FindByEntry(
        /* [in] */ IMapEntry* entry,
        /* [out] */ Node** node);

    /**
     * Removes {@code node} from this tree, rearranging the tree's structure as
     * necessary.
     */
    CARAPI RemoveInternal(
        /* [in] */ Node* node);

    CARAPI RemoveInternalByKey(
        /* [in] */ IInterface* key,
        /* [out] */ Node** removement);

    /**
     * Returns the number of elements in the iteration.
     */
    static CARAPI_(Int32) Count(
        /* [in] */ IIterator* iterator);

private:
    CARAPI ReplaceInParent(
        /* [in] */ Node* node,
        /* [in] */ Node* replacement);

    /**
     * Rebalances the tree by making any AVL rotations necessary between the
     * newly-unbalanced node and the tree's root.
     *
     * @param insert true if the node was unbalanced by an insert; false if it
     *     was by a removal.
     */
    CARAPI Rebalance(
        /* [in] */ Node* unbalanced,
        /* [in] */ Boolean insert);

    /**
     * Rotates the subtree so that its root's right child is the new root.
     */
    CARAPI RotateLeft(
        /* [in] */ Node* root);

    /**
     * Rotates the subtree so that its root's left child is the new root.
     */
    CARAPI RotateRight(
        /* [in] */ Node* root);

    /*
     * Navigable methods.
     */

    /**
     * Returns an immutable version of {@param entry}. Need this because we allow entry to be null,
     * in which case we return a null SimpleImmutableEntry.
     */
    CARAPI_(AutoPtr<SimpleImmutableEntry>) ImmutableCopy(
        /* [in] */ IMapEntry* entry);

    CARAPI_(AutoPtr<IMapEntry>) InternalPollFirstEntry();

    CARAPI_(AutoPtr<IMapEntry>) InternalPollLastEntry();

    CARAPI WriteObject(
        /* [in] */ IObjectOutputStream* stream);

    // @SuppressWarnings("unchecked") // we have to trust that keys are Ks and values are Vs
    CARAPI ReadObject(
        /* [in] */ IObjectInputStream* stream);

public:
    AutoPtr<IComparator> mComparator;
    AutoPtr<Node> mRoot;
    Int32 mSize;
    Int32 mModCount;

private:
     static const AutoPtr<IComparator> NATURAL_ORDER;

    /*
     * View factory methods.
     */

    AutoPtr<_EntrySet> mEntrySet;
    AutoPtr<_KeySet> mKeySet;
};

} // namespace Utility
} // namespace Elastos

#endif // __ELASTOS_UTILITY__CTREEMAP_H__
