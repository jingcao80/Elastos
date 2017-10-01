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

#ifndef __ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CCONCURRENTHASHMAP_H__
#define __ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CCONCURRENTHASHMAP_H__

#include "_Elastos_Utility_Concurrent_CConcurrentHashMap.h"
#include "AbstractMap.h"
#include "ReentrantLock.h"
#include "Object.h"

using Elastos::IO::ISerializable;
using Elastos::IO::IObjectOutputStream;
using Elastos::IO::IObjectInputStream;
using Elastos::IO::ISerializable;
using Elastos::Utility::IIterator;
using Elastos::Utility::IEnumeration;
using Elastos::Utility::Concurrent::Locks::ReentrantLock;
using Elastos::Utility::Concurrent::Locks::IReentrantLock;
using Elastos::Utility::Concurrent::Locks::ICondition;
using Elastos::Utility::Concurrent::Atomic::IAtomicInteger32;

namespace Elastos {
namespace Utility {
namespace Concurrent {

CarClass(CConcurrentHashMap)
    , public AbstractMap
    , public IConcurrentHashMap
    , public IConcurrentMap
    , public ISerializable
{
public:
    /* ---------------- Nodes -------------- */

    /**
     * Key-value entry.  This class is never exported out as a
     * user-mutable Map.Entry (i.e., one supporting setValue; see
     * MapEntry below), but can be used for read-only traversals used
     * in bulk tasks.  Subclasses of Node with a negative hash field
     * are special, and contain null keys and values (but are never
     * exported).  Otherwise, keys and vals are never null.
     */
    class Node
        : public Object
        , public IMapEntry
    {
    public:
        CAR_INTERFACE_DECL()

        Node(
            /* [in] */ Int32 hash,
            /* [in] */ IInterface* key,
            /* [in] */ IInterface* val,
            /* [in] */ Node* next);

        CARAPI GetKey(
            /* [out] */ IInterface** key);

        CARAPI GetValue(
            /* [out] */ IInterface** value);

        CARAPI GetHashCode(
            /* [out] */ Int32* hashCode);

        CARAPI ToString(
            /* [out] */ String* str);

        CARAPI SetValue(
            /* [in] */ IInterface* valueReplacer,
            /* [out] */ IInterface** valueReplacee);

        CARAPI Equals(
            /* [in] */ IInterface* entry,
            /* [out] */ Boolean* result);

        /**
         * Virtualized support for map.get(); overridden in subclasses.
         */
        CARAPI_(AutoPtr<Node>) Find(
            /* [in] */ Int32 h,
            /* [in] */ IInterface* k);

    public:
        Int32 mHash;
        AutoPtr<IInterface> mKey;
        AutoPtr<IInterface> mVal;
        AutoPtr<Node> mNext;
    };

    /**
     * Stripped-down version of helper class used in previous version,
     * declared for the sake of serialization compatibility
     */
    class Segment
        : public ReentrantLock
    {
    public:
        Segment(
            /* [in] */ Float lf) {
            mLoadFactor = lf;
        }

    public:
        static Int64 mSerialVersionUID;// = 2249069246763182397L;
        Float mLoadFactor;
    };

    /* ---------------- Special Nodes -------------- */

    /**
     * A node inserted at head of bins during transfer operations.
     */
    class ForwardingNode
        : public Node
        , public IForwardingNode
    {
    public:
        CAR_INTERFACE_DECL()

        ForwardingNode(
            /* [in] */ ArrayOf<Node*>* tab);

        CARAPI_(AutoPtr<Node>) Find(
            /* [in] */ Int32 h,
            /* [in] */ IInterface* k);

    public:
        AutoPtr<ArrayOf<Node*> > mNextTable;
    };

    /**
     * A place-holder node used in computeIfAbsent and compute
     */
    class ReservationNode
        : public Node
        , public IReservationNode
    {
    public:
        CAR_INTERFACE_DECL()

        ReservationNode() : Node(RESERVED, NULL, NULL, NULL) {}

        CARAPI_(AutoPtr<Node>) Find(
            /* [in] */ Int32 h,
            /* [in] */ IInterface* k) {
            return NULL;
        }
    };

    /* ---------------- TreeNodes -------------- */

    /**
     * Nodes for use in TreeBins
     */
    class TreeNode
        : public Node
        , public ITreeNode
    {
    public:
        CAR_INTERFACE_DECL()

        TreeNode(
            /* [in] */ Int32 hash,
            /* [in] */ IInterface* key,
            /* [in] */ IInterface* val,
            /* [in] */ Node* next,
            /* [in] */ TreeNode* parent);

        CARAPI_(AutoPtr<Node>) Find(
            /* [in] */ Int32 h,
            /* [in] */ IInterface* k);

        /**
         * Returns the TreeNode (or null if not found) for the given key
         * starting at given root.
         */
        CARAPI_(AutoPtr<TreeNode>) FindTreeNode(
            /* [in] */ Int32 h,
            /* [in] */ IInterface* k,
            /* [in] */ InterfaceID kc);

    public:
        AutoPtr<TreeNode> mParent;  // red-black tree links
        AutoPtr<TreeNode> mLeft;
        AutoPtr<TreeNode> mRight;
        AutoPtr<TreeNode> mPrev;    // needed to unlink next upon deletion
        Boolean mRed;
    };

    /* ---------------- TreeBins -------------- */

    /**
     * TreeNodes used at the heads of bins. TreeBins do not hold user
     * keys or values, but instead point to list of TreeNodes and
     * their root. They also maintain a parasitic read-write lock
     * forcing writers (who hold bin lock) to wait for readers (who do
     * not) to complete before tree restructuring operations.
     */
    class TreeBin
        : public Node
        , public ITreeBin
    {
    public:
        CAR_INTERFACE_DECL()

        /**
         * Creates bin with initial set of nodes headed by b.
         */
        TreeBin(
            /* [in] */ TreeNode* b);

        /**
         * Returns matching node or null if none. Tries to search
         * using tree comparisons from root, but continues linear
         * search when lock not available.
         */
        CARAPI_(AutoPtr<Node>) Find(
            /* [in] */ Int32 h,
            /* [in] */ IInterface* k);

        /**
         * Finds or adds a node.
         * @return null if added
         */
        CARAPI_(AutoPtr<TreeNode>) PutTreeVal(
            /* [in] */ Int32 h,
            /* [in] */ IInterface* k,
            /* [in] */ IInterface* v);

        /**
         * Removes the given node, that must be present before this
         * call.  This is messier than typical red-black deletion code
         * because we cannot swap the contents of an interior node
         * with a leaf successor that is pinned by "next" pointers
         * that are accessible independently of lock. So instead we
         * swap the tree linkages.
         *
         * @return true if now too small, so should be untreeified
         */
        CARAPI_(Boolean) RemoveTreeNode(
            /* [in] */ TreeNode* p);

        /* ------------------------------------------------------------ */
        // Red-black tree methods, all adapted from CLR

        static CARAPI_(AutoPtr<TreeNode>) RotateLeft(
            /* [in] */ TreeNode* root,
            /* [in] */ TreeNode* p);

        static CARAPI_(AutoPtr<TreeNode>) RotateRight(
            /* [in] */ TreeNode* root,
            /* [in] */ TreeNode* p);

        static CARAPI_(AutoPtr<TreeNode>) BalanceInsertion(
            /* [in] */ TreeNode* root,
            /* [in] */ TreeNode* x);

        static CARAPI_(AutoPtr<TreeNode>) BalanceDeletion(
            /* [in] */ TreeNode* root,
            /* [in] */ TreeNode* x);

        /**
         * Recursive invariant check
         */
        static CARAPI_(Boolean) CheckInvariants(
            /* [in] */ TreeNode* t);

    private:
        /**
         * Acquires write lock for tree restructuring.
         */
        CARAPI_(void) LockRoot();

        /**
         * Releases write lock for tree restructuring.
         */
        CARAPI_(void) UnlockRoot();

        /**
         * Possibly blocks awaiting root lock.
         */
        CARAPI_(void) ContendedLock();

    public:
        AutoPtr<TreeNode> mRoot;
        AutoPtr<TreeNode> mFirst;
        AutoPtr<IThread> mWaiter;
        Int32 mLockState;
        // values for lockState
        static Int32 WRITER; // set while holding write lock
        static Int32 WAITER; // set when waiting for write lock
        static Int32 READER; // increment value for setting read lock

        // private static final sun.misc.Unsafe U;
        // private static final long LOCKSTATE;
        // static {
        //     try {
        //         U = sun.misc.Unsafe.getUnsafe();
        //         Class<?> k = TreeBin.class;
        //         LOCKSTATE = U.objectFieldOffset
        //             (k.getDeclaredField("lockState"));
        //     } catch (Exception e) {
        //         throw new Error(e);
        //     }
        // }
    };

    /* ----------------Table Traversal -------------- */

    /**
     * Encapsulates traversal for methods such as containsValue; also
     * serves as a base class for other iterators.
     *
     * Method advance visits once each still-valid node that was
     * reachable upon iterator construction. It might miss some that
     * were added to a bin after the bin was visited, which is OK wrt
     * consistency guarantees. Maintaining this property in the face
     * of possible ongoing resizes requires a fair amount of
     * bookkeeping state that is difficult to optimize away amidst
     * volatile accesses.  Even so, traversal maintains reasonable
     * throughput.
     *
     * Normally, iteration proceeds bin-by-bin traversing lists.
     * However, if the table has been resized, then all future steps
     * must traverse both the bin at the current index as well as at
     * (index + baseSize); and so on for further resizings. To
     * paranoically cope with potential sharing by users of iterators
     * across threads, iteration terminates if a bounds checks fails
     * for a table read.
     */
    class Traverser
        : public Object
    {
    public:
        Traverser(
            /* [in] */ ArrayOf<Node*>* tab,
            /* [in] */ Int32 size,
            /* [in] */ Int32 index,
            /* [in] */ Int32 limit);

        /**
         * Advances if possible, returning next valid node, or null if none.
         */
        CARAPI_(AutoPtr<Node>) Advance();

    public:
        AutoPtr<ArrayOf<Node*> > mTab;        // current table; updated if resized
        AutoPtr<Node> mNext;         // the next entry to use
        Int32 mIndex;              // index of bin to use next
        Int32 mBaseIndex;          // current index of initial table
        Int32 mBaseLimit;          // index bound for initial table
        Int32 mBaseSize;     // initial table size
    };

    /**
     * Base of key, value, and entry Iterators. Adds fields to
     * Traverser to support iterator.remove.
     */
    class BaseIterator
        : public Traverser
    {
    public:
        BaseIterator(
            /* [in] */ ArrayOf<Node*>* tab,
            /* [in] */ Int32 size,
            /* [in] */ Int32 index,
            /* [in] */ Int32 limit,
            /* [in] */ CConcurrentHashMap* map);

        CARAPI HasNext(
            /* [out] */ Boolean* value);

        CARAPI HasMoreElements(
            /* [out] */ Boolean* value);

        CARAPI Remove();

    public:
        AutoPtr<CConcurrentHashMap> mMap;
        AutoPtr<Node> mLastReturned;
    };

    class KeyIterator
        : public BaseIterator
        , public IIterator
        , public IEnumeration
    {
    public:
        CAR_INTERFACE_DECL()

        KeyIterator(
            /* [in] */ ArrayOf<Node*>* tab,
            /* [in] */ Int32 index,
            /* [in] */ Int32 size,
            /* [in] */ Int32 limit,
            /* [in] */ CConcurrentHashMap* map);

        CARAPI GetNext(
            /* [out] */ IInterface** object);

        CARAPI GetNextElement(
            /* [out] */ IInterface ** inter);

        CARAPI HasNext(
            /* [out] */ Boolean* value);

        CARAPI HasMoreElements(
            /* [out] */ Boolean* value);

        CARAPI Remove();
    };

    class ValueIterator
        : public BaseIterator
        , public IIterator
        , public IEnumeration
    {
    public:
        CAR_INTERFACE_DECL()

        ValueIterator(
            /* [in] */ ArrayOf<Node*>* tab,
            /* [in] */ Int32 index,
            /* [in] */ Int32 size,
            /* [in] */ Int32 limit,
            /* [in] */ CConcurrentHashMap* map);

        CARAPI GetNext(
            /* [out] */ IInterface** object);

        CARAPI GetNextElement(
            /* [out] */ IInterface ** inter);

        CARAPI HasNext(
            /* [out] */ Boolean* value);

        CARAPI HasMoreElements(
            /* [out] */ Boolean* value);

        CARAPI Remove();
    };

    class EntryIterator
        : public BaseIterator
        , public IIterator
    {
    public:
        CAR_INTERFACE_DECL()

        EntryIterator(
            /* [in] */ ArrayOf<Node*>* tab,
            /* [in] */ Int32 index,
            /* [in] */ Int32 size,
            /* [in] */ Int32 limit,
            /* [in] */ CConcurrentHashMap* map);

        CARAPI GetNext(
            /* [out] */ IInterface** object);

        CARAPI HasNext(
            /* [out] */ Boolean* value);

        CARAPI Remove();
    };

    /**
     * Exported Entry for EntryIterator
     */
    class MapEntry
        : public Object
        , public IMapEntry
    {
    public:
        CAR_INTERFACE_DECL()

        MapEntry(
            /* [in] */ IInterface* key,
            /* [in] */ IInterface* val,
            /* [in] */ CConcurrentHashMap* map);

        CARAPI GetKey(
            /* [out] */ IInterface** key);

        CARAPI GetValue(
            /* [out] */ IInterface** value);

        CARAPI GetHashCode(
            /* [out] */ Int32* hashCode);

        CARAPI ToString(
            /* [out] */ String* str);

        CARAPI Equals(
            /* [in] */ IInterface* entry,
            /* [out] */ Boolean* result);

        CARAPI SetValue(
            /* [in] */ IInterface* valueReplacer,
            /* [out] */ IInterface** valueReplacee);

    public:
        AutoPtr<IInterface> mKey; // non-null
        AutoPtr<IInterface> mVal;       // non-null
        AutoPtr<CConcurrentHashMap> mMap;
    };

    /* ----------------Views -------------- */

    /**
     * Base class for views.
     *
     */
    class CollectionView
        : public Object
        , public ICollection
        , public IIterable
        , public ISerializable
    {
    public:
        CAR_INTERFACE_DECL()

        CollectionView(
            /* [in] */ CConcurrentHashMap* map);

        /**
         * Returns the map backing this view.
         *
         * @return the map backing this view
         */
        CARAPI_(AutoPtr<CConcurrentHashMap>) GetMap();

        /**
         * Removes all of the elements from this view, by removing all
         * the mappings from the map backing this view.
         */
        CARAPI Clear();

        CARAPI GetSize(
            /* [out] */ Int32* size);

        CARAPI IsEmpty(
            /* [out] */ Boolean* result);

        // implementations below rely on concrete classes supplying these
        // abstract methods
        /**
         * Returns a "weakly consistent" iterator that will never
         * throw {@link ConcurrentModificationException}, and
         * guarantees to traverse elements as they existed upon
         * construction of the iterator, and may (but is not
         * guaranteed to) reflect any modifications subsequent to
         * construction.
         */
        virtual CARAPI GetIterator(
            /* [out] */ IIterator** it) = 0;

        virtual CARAPI Contains(
            /* [in] */ IInterface* object,
            /* [out] */ Boolean* result) = 0;

        virtual CARAPI Remove(
            /* [in] */ IInterface* object,
            /* [out] */ Boolean* modified) = 0;

        CARAPI ToArray(
            /* [out, callee] */ ArrayOf<IInterface*>** array);

        CARAPI ToArray(
            /* [in] */ ArrayOf<IInterface*>* inArray,
            /* [out, callee] */ ArrayOf<IInterface*>** outArray);

        using Object::ToString;

        /**
         * Returns a string representation of this collection.
         * The string representation consists of the string representations
         * of the collection's elements in the order they are returned by
         * its iterator, enclosed in square brackets ({@code "[]"}).
         * Adjacent elements are separated by the characters {@code ", "}
         * (comma and space).  Elements are converted to strings as by
         * {@link String#valueOf(Object)}.
         *
         * @return a string representation of this collection
         */
        CARAPI_(String) ToString();

        CARAPI ContainsAll(
            /* [in] */ ICollection* collection,
            /* [out] */ Boolean* result);

        CARAPI RemoveAll(
            /* [in] */ ICollection* collection,
            /* [out] */ Boolean* modified);

        CARAPI RetainAll(
            /* [in] */ ICollection* collection,
            /* [out] */ Boolean* modified);

    protected:
        AutoPtr<CConcurrentHashMap> mMap;

    private:
        static Int64 mSerialVersionUID;// = 7249069246763182397L;
        static String mOomeMsg;
    };

    /**
     * A view of a ConcurrentHashMap as a {@link Set} of keys, in
     * which additions may optionally be enabled by mapping to a
     * common value.  This class cannot be directly instantiated.
     * See {@link #keySet() keySet()},
     * {@link #keySet(Object) keySet(V)},
     * {@link #newKeySet() newKeySet()},
     * {@link #newKeySet(int) newKeySet(int)}.
     *
     * @since 1.8
     *
     * @hide
     */
    class KeySetView
        : public CollectionView
        , public ISet
    {
    public:
        CAR_INTERFACE_DECL()

        KeySetView(
            /* [in] */ CConcurrentHashMap* map,
            /* [in] */ IInterface* value);

        /**
         * Returns the default mapped value for additions,
         * or {@code null} if additions are not supported.
         *
         * @return the default mapped value for additions, or {@code null}
         * if not supported
         */
        CARAPI_(AutoPtr<IInterface>) GetMappedValue();

        CARAPI Contains(
            /* [in] */ IInterface* object,
            /* [out] */ Boolean* result);

        CARAPI Remove(
            /* [in] */ IInterface* object,
            /* [out] */ Boolean* modified);

        CARAPI GetIterator(
            /* [out] */ IIterator** it);

        CARAPI Add(
            /* [in] */ IInterface* object,
            /* [out] */ Boolean* modified);

        CARAPI AddAll(
            /* [in] */ ICollection* collection,
            /* [out] */ Boolean* modified);

        CARAPI GetHashCode(
            /* [out] */ Int32* hashCode);

        CARAPI Equals(
            /* [in] */ IInterface* object,
            /* [out] */ Boolean* result);

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

        CARAPI Clear();

        CARAPI ContainsAll(
            /* [in] */ ICollection* collection,
            /* [out] */ Boolean* result);

        CARAPI IsEmpty(
        /* [out] */ Boolean* result);

        CARAPI RemoveAll(
            /* [in] */ ICollection* collection,
            /* [out] */ Boolean* result);

        CARAPI RetainAll(
            /* [in] */ ICollection* collection,
            /* [out] */ Boolean* result);

        CARAPI GetSize(
            /* [out] */ Int32* size);

        CARAPI ToArray(
            /* [out, callee] */ ArrayOf<IInterface*>** result);

        CARAPI ToArray(
            /* [in] */ ArrayOf<IInterface*>* array,
            /* [out, callee] */ ArrayOf<IInterface*>** result);

    private:
        static Int64 mSerialVersionUID;// = 7249069246763182397L;
        AutoPtr<IInterface> mValue;
    };

    /**
     * A view of a ConcurrentHashMap as a {@link Collection} of
     * values, in which additions are disabled. This class cannot be
     * directly instantiated. See {@link #values()}.
     */
    class ValuesView
        : public CollectionView
    {
    public:
        ValuesView(
            /* [in] */ CConcurrentHashMap* map);

        CARAPI Contains(
            /* [in] */ IInterface* object,
            /* [out] */ Boolean* result);

        CARAPI Remove(
            /* [in] */ IInterface* object,
            /* [out] */ Boolean* modified);

        CARAPI GetIterator(
            /* [out] */ IIterator** it);

        CARAPI Add(
            /* [in] */ IInterface* object,
            /* [out] */ Boolean* modified);

        CARAPI AddAll(
            /* [in] */ ICollection* collection,
            /* [out] */ Boolean* modified);

        CARAPI Add(
            /* [in] */ IInterface* object);

        CARAPI AddAll(
            /* [in] */ ICollection* collection);

        CARAPI Remove(
            /* [in] */ IInterface* object);

        using CollectionView::RemoveAll;

        CARAPI RemoveAll(
            /* [in] */ ICollection* collection);

        using CollectionView::RetainAll;

        CARAPI RetainAll(
            /* [in] */ ICollection* collection);

        CARAPI Equals(
            /* [in] */ IInterface* object,
            /* [out] */ Boolean* result);

        CARAPI GetHashCode(
            /* [out] */ Int32* hashCode);

    private:
        static Int64 mSerialVersionUID;// = 2249069246763182397L;
    };

    /**
     * A view of a ConcurrentHashMap as a {@link Set} of (key, value)
     * entries.  This class cannot be directly instantiated. See
     * {@link #entrySet()}.
     */
    class EntrySetView
        : public CollectionView
        , public ISet
    {
    public:
        CAR_INTERFACE_DECL()

        EntrySetView(
            /* [in] */ CConcurrentHashMap* map);

        CARAPI Contains(
            /* [in] */ IInterface* object,
            /* [out] */ Boolean* result);

        CARAPI Remove(
            /* [in] */ IInterface* object,
            /* [out] */ Boolean* modified);

        CARAPI GetIterator(
            /* [out] */ IIterator** it);

        CARAPI Add(
            /* [in] */ IInterface* object,
            /* [out] */ Boolean* modified);

        CARAPI AddAll(
            /* [in] */ ICollection* collection,
            /* [out] */ Boolean* modified);

        CARAPI GetHashCode(
            /* [out] */ Int32* hashCode);

        CARAPI Equals(
            /* [in] */ IInterface* object,
            /* [out] */ Boolean* result);

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

        CARAPI Clear();

        CARAPI ContainsAll(
            /* [in] */ ICollection* collection,
            /* [out] */ Boolean* result);

        CARAPI IsEmpty(
        /* [out] */ Boolean* result);

        CARAPI RemoveAll(
            /* [in] */ ICollection* collection,
            /* [out] */ Boolean* result);

        CARAPI RetainAll(
            /* [in] */ ICollection* collection,
            /* [out] */ Boolean* result);

        CARAPI GetSize(
            /* [out] */ Int32* size);

        CARAPI ToArray(
            /* [out, callee] */ ArrayOf<IInterface*>** result);

        CARAPI ToArray(
            /* [in] */ ArrayOf<IInterface*>* array,
            /* [out, callee] */ ArrayOf<IInterface*>** result);
    private:
        static Int64 mSerialVersionUID;// = 2249069246763182397L;
    };


    /* ---------------- Counters -------------- */

    // Adapted from LongAdder and Striped64.
    // See their internal docs for explanation.

    // A padded cell for distributing counts
    class CounterCell
        : public Object
    {
    public:
        CounterCell(
            /* [in] */ Int64 x) {
            mValue = x;
        }

    public:
        Int64 mP0, mP1, mP2, mP3, mP4, mP5, mP6;
        Int64 mValue;
        Int64 mQ0, mQ1, mQ2, mQ3, mQ4, mQ5, mQ6;
    };

    /**
     * Holder for the thread-local hash code determining which
     * CounterCell to use. The code is initialized via the
     * counterHashCodeGenerator, but may be moved upon collisions.
     */
    class CounterHashCode
        : public Object
    {
    public:
        Int32 mCode;
    };

    class StaticInitializer
    {
    public:
        StaticInitializer();
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /**
     * Creates a new, empty map with the specified initial
     * capacity, load factor and concurrency level.
     *
     * @param initialCapacity the initial capacity. The implementation
     * performs internal sizing to accommodate this many elements.
     * @param loadFactor  the load factor threshold, used to control resizing.
     * Resizing may be performed when the average number of elements per
     * bin exceeds this threshold.
     * @param concurrencyLevel the estimated number of concurrently
     * updating threads. The implementation performs internal sizing
     * to try to accommodate this many threads.
     * @throws IllegalArgumentException if the initial capacity is
     * negative or the load factor or concurrencyLevel are
     * nonpositive.
     */
    // @SuppressWarnings("unchecked")
    CARAPI constructor(
        /* [in] */ Int32 initialCapacity,
        /* [in] */ Float loadFactor,
        /* [in] */ Int32 concurrencyLevel);

    /**
     * Creates a new, empty map with the specified initial capacity
     * and load factor and with the default concurrencyLevel (16).
     *
     * @param initialCapacity The implementation performs internal
     * sizing to accommodate this many elements.
     * @param loadFactor  the load factor threshold, used to control resizing.
     * Resizing may be performed when the average number of elements per
     * bin exceeds this threshold.
     * @throws IllegalArgumentException if the initial capacity of
     * elements is negative or the load factor is nonpositive
     *
     * @since 1.6
     */
    CARAPI constructor(
        /* [in] */ Int32 initialCapacity,
        /* [in] */ Float loadFactor);

    /**
     * Creates a new, empty map with the specified initial capacity,
     * and with default load factor (0.75) and concurrencyLevel (16).
     *
     * @param initialCapacity the initial capacity. The implementation
     * performs internal sizing to accommodate this many elements.
     * @throws IllegalArgumentException if the initial capacity of
     * elements is negative.
     */
    CARAPI constructor(
        /* [in] */ Int32 initialCapacity);

    /**
     * Creates a new, empty map with a default initial capacity (16),
     * load factor (0.75) and concurrencyLevel (16).
     */
    CARAPI constructor();

    /**
     * Creates a new map with the same mappings as the given map.
     * The map is created with a capacity of 1.5 times the number
     * of mappings in the given map or 16 (whichever is greater),
     * and a default load factor (0.75) and concurrencyLevel (16).
     *
     * @param m the map
     */
    CARAPI constructor(
        /* [in] */ IMap* m);

    // Original (since JDK1.2) Map methods

    /**
     * {@inheritDoc}
     */
    CARAPI GetSize(
        /* [out] */ Int32* outsize);

    /**
     * {@inheritDoc}
     */
    CARAPI IsEmpty(
        /* [out] */ Boolean* result);

    /**
     * Returns the value to which the specified key is mapped,
     * or {@code null} if this map contains no mapping for the key.
     *
     * <p>More formally, if this map contains a mapping from a key
     * {@code k} to a value {@code v} such that {@code key.equals(k)},
     * then this method returns {@code v}; otherwise it returns
     * {@code null}.  (There can be at most one such mapping.)
     *
     * @throws NullPointerException if the specified key is null
     */
    CARAPI Get(
        /* [in] */ IInterface* key,
        /* [out] */ IInterface** value);

    /**
     * Tests if the specified object is a key in this table.
     *
     * @param  key possible key
     * @return {@code true} if and only if the specified object
     *         is a key in this table, as determined by the
     *         {@code equals} method; {@code false} otherwise
     * @throws NullPointerException if the specified key is null
     */
    CARAPI ContainsKey(
        /* [in] */ IInterface* key,
        /* [out] */ Boolean* result);

    /**
     * Returns {@code true} if this map maps one or more keys to the
     * specified value. Note: This method may require a full traversal
     * of the map, and is much slower than method {@code containsKey}.
     *
     * @param value value whose presence in this map is to be tested
     * @return {@code true} if this map maps one or more keys to the
     *         specified value
     * @throws NullPointerException if the specified value is null
     */
    CARAPI ContainsValue(
        /* [in] */ IInterface* value,
        /* [out] */ Boolean* result);

    /**
     * Maps the specified key to the specified value in this table.
     * Neither the key nor the value can be null.
     *
     * <p>The value can be retrieved by calling the {@code get} method
     * with a key that is equal to the original key.
     *
     * @param key key with which the specified value is to be associated
     * @param value value to be associated with the specified key
     * @return the previous value associated with {@code key}, or
     *         {@code null} if there was no mapping for {@code key}
     * @throws NullPointerException if the specified key or value is null
     */
    CARAPI Put(
        /* [in] */ IInterface* key,
        /* [in] */ IInterface* value,
        /* [out] */ IInterface** oldValue);

    CARAPI Put(
        /* [in] */ IInterface* key,
        /* [in] */ IInterface* value);

    /** Implementation for put and putIfAbsent */
    CARAPI PutVal(
        /* [in] */ IInterface* key,
        /* [in] */ IInterface* value,
        /* [in] */ Boolean onlyIfAbsent,
        /* [out] */ IInterface** oldValue);

    /**
     * Copies all of the mappings from the specified map to this one.
     * These mappings replace any mappings that this map had for any of the
     * keys currently in the specified map.
     *
     * @param m mappings to be stored in this map
     */
    CARAPI PutAll(
        /* [in] */ IMap* map);

    /**
     * Removes the key (and its corresponding value) from this map.
     * This method does nothing if the key is not in the map.
     *
     * @param  key the key that needs to be removed
     * @return the previous value associated with {@code key}, or
     *         {@code null} if there was no mapping for {@code key}
     * @throws NullPointerException if the specified key is null
     */
    CARAPI Remove(
        /* [in] */ IInterface* key,
        /* [out] */ IInterface** value);

    CARAPI Remove(
        /* [in] */ IInterface* key);

    using AbstractMap::Remove;

    /**
     * Implementation for the four public remove/replace methods:
     * Replaces node value with v, conditional upon match of cv if
     * non-null.  If resulting value is null, delete.
     */
    CARAPI_(AutoPtr<IInterface>) ReplaceNode(
        /* [in] */ IInterface* key,
        /* [in] */ IInterface* value,
        /* [in] */ IInterface* cv);

    /**
     * Removes all of the mappings from this map.
     */
    CARAPI Clear();

    /**
     * Returns a {@link Set} view of the keys contained in this map.
     * The set is backed by the map, so changes to the map are
     * reflected in the set, and vice-versa. The set supports element
     * removal, which removes the corresponding mapping from this map,
     * via the {@code Iterator.remove}, {@code Set.remove},
     * {@code removeAll}, {@code retainAll}, and {@code clear}
     * operations.  It does not support the {@code add} or
     * {@code addAll} operations.
     *
     * <p>The view's {@code iterator} is a "weakly consistent" iterator
     * that will never throw {@link ConcurrentModificationException},
     * and guarantees to traverse elements as they existed upon
     * construction of the iterator, and may (but is not guaranteed to)
     * reflect any modifications subsequent to construction.
     *
     * @return the set view
     *
     */
    CARAPI GetKeySet(
        /* [out] */ ISet** keySet);

    /**
     * Returns a {@link Collection} view of the values contained in this map.
     * The collection is backed by the map, so changes to the map are
     * reflected in the collection, and vice-versa.  The collection
     * supports element removal, which removes the corresponding
     * mapping from this map, via the {@code Iterator.remove},
     * {@code Collection.remove}, {@code removeAll},
     * {@code retainAll}, and {@code clear} operations.  It does not
     * support the {@code add} or {@code addAll} operations.
     *
     * <p>The view's {@code iterator} is a "weakly consistent" iterator
     * that will never throw {@link ConcurrentModificationException},
     * and guarantees to traverse elements as they existed upon
     * construction of the iterator, and may (but is not guaranteed to)
     * reflect any modifications subsequent to construction.
     *
     * @return the collection view
     */
    CARAPI GetValues(
        /* [out] */ ICollection** value);

    /**
     * Returns a {@link Set} view of the mappings contained in this map.
     * The set is backed by the map, so changes to the map are
     * reflected in the set, and vice-versa.  The set supports element
     * removal, which removes the corresponding mapping from the map,
     * via the {@code Iterator.remove}, {@code Set.remove},
     * {@code removeAll}, {@code retainAll}, and {@code clear}
     * operations.
     *
     * <p>The view's {@code iterator} is a "weakly consistent" iterator
     * that will never throw {@link ConcurrentModificationException},
     * and guarantees to traverse elements as they existed upon
     * construction of the iterator, and may (but is not guaranteed to)
     * reflect any modifications subsequent to construction.
     *
     * @return the set view
     */
    CARAPI GetEntrySet(
        /* [out] */ ISet** entries);

    /**
     * Returns the hash code value for this {@link Map}, i.e.,
     * the sum of, for each key-value pair in the map,
     * {@code key.hashCode() ^ value.hashCode()}.
     *
     * @return the hash code value for this map
     */
    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    using AbstractMap::ToString;

    /**
     * Returns a string representation of this map.  The string
     * representation consists of a list of key-value mappings (in no
     * particular order) enclosed in braces ("{@code {}}").  Adjacent
     * mappings are separated by the characters {@code ", "} (comma
     * and space).  Each key-value mapping is rendered as the key
     * followed by an equals sign ("{@code =}") followed by the
     * associated value.
     *
     * @return a string representation of this map
     */
    CARAPI_(String) ToString();

    /**
     * Compares the specified object with this map for equality.
     * Returns {@code true} if the given object is a map with the same
     * mappings as this map.  This operation may return misleading
     * results if either map is concurrently modified during execution
     * of this method.
     *
     * @param o object to be compared for equality with this map
     * @return {@code true} if the specified object is equal to this map
     */
    CARAPI Equals(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* result);

    // ConcurrentMap methods

    /**
     * {@inheritDoc}
     *
     * @return the previous value associated with the specified key,
     *         or {@code null} if there was no mapping for the key
     * @throws NullPointerException if the specified key or value is null
     */
    CARAPI PutIfAbsent(
        /* [in] */ IInterface* key,
        /* [in] */ IInterface* value,
        /* [out] */ IInterface** outface);

    /**
     * {@inheritDoc}
     *
     * @throws NullPointerException if the specified key is null
     */
    CARAPI Remove(
        /* [in] */ IInterface* key,
        /* [in] */ IInterface* value,
        /* [out] */ Boolean* result);

    /**
     * {@inheritDoc}
     *
     * @throws NullPointerException if any of the arguments are null
     */
    CARAPI Replace(
        /* [in] */ IInterface* key,
        /* [in] */ IInterface* oldValue,
        /* [in] */ IInterface* newValue,
        /* [out] */ Boolean* result);

    /**
     * {@inheritDoc}
     *
     * @return the previous value associated with the specified key,
     *         or {@code null} if there was no mapping for the key
     * @throws NullPointerException if the specified key or value is null
     */
    CARAPI Replace(
        /* [in] */ IInterface* key,
        /* [in] */ IInterface* value,
        /* [out] */ IInterface** result);

    // Hashtable legacy methods

    /**
     * Legacy method testing if some key maps into the specified value
     * in this table.  This method is identical in functionality to
     * {@link #containsValue(Object)}, and exists solely to ensure
     * full compatibility with class {@link java.util.Hashtable}.
     *
     * @param  value a value to search for
     * @return {@code true} if and only if some key maps to the
     *         {@code value} argument in this table as
     *         determined by the {@code equals} method;
     *         {@code false} otherwise
     * @throws NullPointerException if the specified value is null
     */
    CARAPI Contains(
        /* [in] */ IInterface* value,
        /* [out] */ Boolean* result);

    /**
     * Returns an enumeration of the keys in this table.
     *
     * @return an enumeration of the keys in this table
     * @see #keySet()
     */
    CARAPI Keys(
        /* [out] */ IEnumeration** outenum);

    /**
     * Returns an enumeration of the values in this table.
     *
     * @return an enumeration of the values in this table
     * @see #values()
     */
    CARAPI Elements(
        /* [out] */ IEnumeration** outenum);

    // ConcurrentHashMap-only methods

    /**
     * Returns the number of mappings. This method should be used
     * instead of {@link #size} because a ConcurrentHashMap may
     * contain more mappings than can be represented as an int. The
     * value returned is an estimate; the actual count may differ if
     * there are concurrent insertions or removals.
     *
     * @return the number of mappings
     * @since 1.8
     *
     * @hide
     */
    CARAPI_(Int64) MappingCount();

    /**
     * Creates a new {@link Set} backed by a ConcurrentHashMap
     * from the given type to {@code Boolean.TRUE}.
     *
     * @return the new set
     * @since 1.8
     *
     * @hide
     */
    static CARAPI_(AutoPtr<KeySetView>) NewKeySet();

    /**
     * Creates a new {@link Set} backed by a ConcurrentHashMap
     * from the given type to {@code Boolean.TRUE}.
     *
     * @param initialCapacity The implementation performs internal
     * sizing to accommodate this many elements.
     * @throws IllegalArgumentException if the initial capacity of
     * elements is negative
     * @return the new set
     * @since 1.8
     *
     * @hide
     */
    static CARAPI_(AutoPtr<KeySetView>) NewKeySet(
        /* [in] */ Int32 initialCapacity);

    /**
     * Returns a {@link Set} view of the keys in this map, using the
     * given common mapped value for any additions (i.e., {@link
     * Collection#add} and {@link Collection#addAll(Collection)}).
     * This is of course only appropriate if it is acceptable to use
     * the same value for all additions from this view.
     *
     * @param mappedValue the mapped value to use for any additions
     * @return the set view
     * @throws NullPointerException if the mappedValue is null
     *
     * @hide
     */
    CARAPI KeySet(
        /* [in] */ IInterface* mappedValue,
        /* [out] */ ISet** set);

    CARAPI_(Int64) SumCount();

    /**
     * Spreads (XORs) higher bits of hash to lower and also forces top
     * bit to 0. Because the table uses power-of-two masking, sets of
     * hashes that vary only in bits above the current mask will
     * always collide. (Among known examples are sets of Float keys
     * holding consecutive whole numbers in small tables.)  So we
     * apply a transform that spreads the impact of higher bits
     * downward. There is a tradeoff between speed, utility, and
     * quality of bit-spreading. Because many common sets of hashes
     * are already reasonably distributed (so don't benefit from
     * spreading), and because we use trees to handle large sets of
     * collisions in bins, we just XOR some shifted bits in the
     * cheapest possible way to reduce systematic lossage, as well as
     * to incorporate impact of the highest bits that would otherwise
     * never be used in index calculations because of table bounds.
     */
    static CARAPI_(Int32) Spread(
        /* [in] */ Int32 h);

    /**
     * Returns x's Class if it is of the form "class C implements
     * Comparable<C>", else null.
     */
    static CARAPI_(InterfaceID) ComparableClassFor(
        /* [in] */ IInterface* x);

    /**
     * Returns k.compareTo(x) if x matches kc (k's screened comparable
     * class), else 0.
     */
    static CARAPI_(Int32) CompareComparables(
        /* [in] */ const InterfaceID& kc,
        /* [in] */ IInterface* k,
        /* [in] */ IInterface* x);

    /* ---------------- Table element access -------------- */

    /*
     * Volatile access methods are used for table elements as well as
     * elements of in-progress next table while resizing.  All uses of
     * the tab arguments must be null checked by callers.  All callers
     * also paranoically precheck that tab's length is not zero (or an
     * equivalent check), thus ensuring that any index argument taking
     * the form of a hash value anded with (length - 1) is a valid
     * index.  Note that, to be correct wrt arbitrary concurrency
     * errors by users, these checks must operate on local variables,
     * which accounts for some odd-looking inline assignments below.
     * Note that calls to setTabAt always occur within locked regions,
     * and so do not need full volatile semantics, but still require
     * ordering to maintain concurrent readability.
     */
    static CARAPI_(AutoPtr<Node>) TabAt(
        /* [in] */ ArrayOf<Node*>* tab,
        /* [in] */ Int32 i);

    static CARAPI_(Boolean) CasTabAt(
        /* [in] */ ArrayOf<Node*>* tab,
        /* [in] */ Int32 i,
        /* [in] */ Node* c,
        /* [in] */ Node* v);

    static CARAPI_(void) SetTabAt(
        /* [in] */ ArrayOf<Node*>* tab,
        /* [in] */ Int32 i,
        /* [in] */ Node* v);

private:
    /* ---------------- Serialization Support -------------- */

    /**
     * Saves the state of the <tt>ConcurrentHashMap</tt> instance to a
     * stream (i.e., serializes it).
     * @param s the stream
     * @serialData
     * the key (Object) and value (Object)
     * for each key-value mapping, followed by a null pair.
     * The key-value mappings are emitted in no particular order.
     */
    CARAPI WriteObject(
        /* [in] */ IObjectOutputStream* s);

    /**
     * Reconstitutes the <tt>ConcurrentHashMap</tt> instance from a
     * stream (i.e., deserializes it).
     * @param s the stream
     */
    // @SuppressWarnings("unchecked")
    CARAPI ReadObject(
        /* [in] */ IObjectInputStream* s);

    /**
     * Returns a power of two table size for the given desired capacity.
     * See Hackers Delight, sec 3.2
     */
    static CARAPI_(Int32) TableSizeFor(
        /* [in] */ Int32 c);

    /* ---------------- Table Initialization and Resizing -------------- */

    /**
     * Initializes table, using the size recorded in sizeCtl.
     */
    CARAPI_(AutoPtr<ArrayOf<Node*> >) InitTable();

    /**
     * Adds to count, and if table is too small and not already
     * resizing, initiates transfer. If already resizing, helps
     * perform transfer if work is available.  Rechecks occupancy
     * after a transfer to see if another resize is already needed
     * because resizings are lagging additions.
     *
     * @param x the count to add
     * @param check if <0, don't check resize, if <= 1 only check if uncontended
     */
    CARAPI_(void) AddCount(
        /* [in] */ Int64 x,
        /* [in] */ Int32 check);

    /**
     * Helps transfer if a resize is in progress.
     */
    CARAPI_(AutoPtr<ArrayOf<Node*> >) HelpTransfer(
        /* [in] */ ArrayOf<Node*>* tab,
        /* [in] */ Node* f);

    /**
     * Tries to presize table to accommodate the given number of elements.
     *
     * @param size number of elements (doesn't need to be perfectly accurate)
     */
    CARAPI_(void) TryPresize(
        /* [in] */ Int32 size);

    /**
     * Moves and/or copies the nodes in each bin to new table. See
     * above for explanation.
     */
    CARAPI_(void) Transfer(
        /* [in] */ ArrayOf<Node*>* tab,
        /* [in] */ ArrayOf<Node*>* nextTab);

    /* ---------------- Conversion from/to TreeBins -------------- */

    /**
     * Replaces all linked nodes in bin at given index unless table is
     * too small, in which case resizes instead.
     */
    CARAPI_(void) TreeifyBin(
        /* [in] */ ArrayOf<Node*>* tab,
        /* [in] */ Int32 index);

    /**
     * Returns a list on non-TreeNodes replacing those in given list.
     */
    static CARAPI_(AutoPtr<Node>) Untreeify(
        /* [in] */ Node* b);

    // See LongAdder version for explanation
    CARAPI_(void) FullAddCount(
        /* [in] */ Int64 x,
        /* [in] */ CounterHashCode* hc,
        /* [in] */ Boolean wasUncontended);

public:
    /*
     * The basic strategy is to subdivide the table among Segments,
     * each of which itself is a concurrently readable hash table.  To
     * reduce footprint, all but one segments are constructed only
     * when first needed (see ensureSegment). To maintain visibility
     * in the presence of lazy construction, accesses to segments as
     * well as elements of segment's table must use volatile access,
     * which is done via Unsafe within methods segmentAt etc
     * below. These provide the functionality of AtomicReferenceArrays
     * but reduce the levels of indirection. Additionally,
     * volatile-writes of table elements and entry "next" fields
     * within locked operations use the cheaper "lazySet" forms of
     * writes (via putOrderedObject) because these writes are always
     * followed by lock releases that maintain sequential consistency
     * of table updates.
     *
     * Historical note: The previous version of this class relied
     * heavily on "final" fields, which avoided some volatile reads at
     * the expense of a large initial footprint.  Some remnants of
     * that design (including forced construction of segment 0) exist
     * to ensure serialization compatibility.
     */

    /* ---------------- Constants -------------- */

    /**
     * The largest possible table capacity.  This value must be
     * exactly 1<<30 to stay within Java array allocation and indexing
     * bounds for power of two table sizes, and is further required
     * because the top two bits of 32bit hash fields are used for
     * control purposes.
     */
    static const Int32 MAXIMUM_CAPACITY;

    /**
     * The default initial table capacity.  Must be a power of 2
     * (i.e., at least 1) and at most MAXIMUM_CAPACITY.
     */
    static const Int32 DEFAULT_CAPACITY;

    /**
     * The largest possible (non-power of two) array size.
     * Needed by toArray and related methods.
     */
    static const Int32 MAX_ARRAY_SIZE;

    /**
     * The default concurrency level for this table. Unused but
     * defined for compatibility with previous versions of this class.
     */
    static const Int32 DEFAULT_CONCURRENCY_LEVEL;

    /**
     * The load factor for this table. Overrides of this value in
     * constructors affect only the initial table capacity.  The
     * actual floating point value isn't normally used -- it is
     * simpler to use expressions such as {@code n - (n >>> 2)} for
     * the associated resizing threshold.
     */
    static const Float LOAD_FACTOR;

    /**
     * The bin count threshold for using a tree rather than list for a
     * bin.  Bins are converted to trees when adding an element to a
     * bin with at least this many nodes. The value must be greater
     * than 2, and should be at least 8 to mesh with assumptions in
     * tree removal about conversion back to plain bins upon
     * shrinkage.
     */
    static const Int32 TREEIFY_THRESHOLD;

    /**
     * The bin count threshold for untreeifying a (split) bin during a
     * resize operation. Should be less than TREEIFY_THRESHOLD, and at
     * most 6 to mesh with shrinkage detection under removal.
     */
    static const Int32 UNTREEIFY_THRESHOLD;

    /**
     * The smallest table capacity for which bins may be treeified.
     * (Otherwise the table is resized if too many nodes in a bin.)
     * The value should be at least 4 * TREEIFY_THRESHOLD to avoid
     * conflicts between resizing and treeification thresholds.
     */
    static const Int32 MIN_TREEIFY_CAPACITY;

    /**
     * Minimum number of rebinnings per transfer step. Ranges are
     * subdivided to allow multiple resizer threads.  This value
     * serves as a lower bound to avoid resizers encountering
     * excessive memory contention.  The value should be at least
     * DEFAULT_CAPACITY.
     */
    static const Int32 MIN_TRANSFER_STRIDE;

    /*
     * Encodings for Node hash fields. See above for explanation.
     */
    static const Int32 MOVED;
    static const Int32 TREEBIN;
    static const Int32 RESERVED;
    static const Int32 HASH_BITS;

    /** Number of CPUS, to place bounds on some sizings */
    static const Int32 NCPU;

    /** For serialization compatibility. */
    // private static final ObjectStreamField[] serialPersistentFields = {
    //     new ObjectStreamField("segments", Segment[].class),
    //     new ObjectStreamField("segmentMask", Integer.TYPE),
    //     new ObjectStreamField("segmentShift", Integer.TYPE)
    // };

    /* ---------------- Fields -------------- */

    /**
     * The array of bins. Lazily initialized upon first insertion.
     * Size is always a power of two. Accessed directly by iterators.
     */
    /* transient */ AutoPtr<ArrayOf<Node*> > mTable;

    /**
     * Generates initial value for per-thread CounterHashCodes.
     */
    static const AutoPtr<IAtomicInteger32> sCounterHashCodeGenerator;

    /**
     * Increment for counterHashCodeGenerator. See class ThreadLocal
     * for explanation.
     */
    static Int32 SEED_INCREMENT;

    /**
     * Per-thread counter hash codes. Shared across all instances.
     */
    static pthread_key_t sThreadCounterHashCode;

private:
    /**
     * The next table to use; non-null only while resizing.
     */
    /* transient */ AutoPtr<ArrayOf<Node*> > mNextTable;

    /**
     * Base counter value, used mainly when there is no contention,
     * but also as a fallback during table initialization
     * races. Updated via CAS.
     */
    /* transient */ Int64 mBaseCount;

    /**
     * Table initialization and resizing control.  When negative, the
     * table is being initialized or resized: -1 for initialization,
     * else -(1 + the number of active resizing threads).  Otherwise,
     * when table is null, holds the initial table size to use upon
     * creation, or 0 for default. After initialization, holds the
     * next element count value upon which to resize the table.
     */
    /* transient */ Int32 mSizeCtl;

    /**
     * The next table index (plus one) to split while resizing.
     */
    /* transient */ Int32 mTransferIndex;

    /**
     * The least available table index to split while resizing.
     */
    /* transient */ Int32 mTransferOrigin;

    /**
     * Spinlock (locked via CAS) used when resizing and/or creating CounterCells.
     */
    /* transient */ Int32 mCellsBusy;

    /**
     * Table of counter cells. When non-null, size is a power of 2.
     */
    /* transient */ AutoPtr<ArrayOf<CounterCell*> > mCounterCells;

    // views
    /* transient */ AutoPtr<KeySetView> mKeySet;
    /* transient */ AutoPtr<ValuesView> mValues;
    /* transient */ AutoPtr<EntrySetView> mEntrySet;

    static const StaticInitializer sInitializer;
};

} // namespace Concurrent
} // namespace Utility
} // namespace Elastos

DEFINE_CONVERSION_FOR(Elastos::Utility::Concurrent::CConcurrentHashMap::CounterCell, IInterface)

DEFINE_CONVERSION_FOR(Elastos::Utility::Concurrent::CConcurrentHashMap::Node, IInterface)

DEFINE_CONVERSION_FOR(Elastos::Utility::Concurrent::CConcurrentHashMap::Segment, IInterface)

#endif //__ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CCONCURRENTHASHMAP_H__
