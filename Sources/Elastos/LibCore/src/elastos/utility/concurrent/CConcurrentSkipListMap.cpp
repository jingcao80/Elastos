
#include "Elastos.CoreLibrary.IO.h"
#include "CConcurrentSkipListMap.h"
#include <Math.h>
#include "CRandom.h"
#include "CArrayList.h"
//#include "CCollections.h"

using Elastos::Core::Math;
using Elastos::Core::EIID_IComparable;
using Elastos::Core::EIID_ICloneable;
using Elastos::Utility::CRandom;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;
using Elastos::IO::EIID_ISerializable;

namespace Elastos {
namespace Utility {
namespace Concurrent {

//====================================================================
// CConcurrentSkipListMap::
//====================================================================

//static long serialVersionUID = -8627078645895051609L;

AutoPtr<IRandom> CConcurrentSkipListMap::sSeedGenerator;

AutoPtr<IInterface> CConcurrentSkipListMap::sBASE_HEADER;// = new CObject();

CAR_INTERFACE_IMPL_5(CConcurrentSkipListMap, AbstractMap, INavigableMap, ISortedMap, IConcurrentMap, ICloneable, ISerializable)

CAR_OBJECT_IMPL(CConcurrentSkipListMap);

ECode CConcurrentSkipListMap::Initialize()
{
    CRandom::New((IRandom**)&sSeedGenerator);
    mKeySet = NULL;
    mEntrySet = NULL;
    mValues = NULL;
    mDescendingMap = NULL;
    Int32 num = 0;
    sSeedGenerator->NextInt32(&num);
    mRandomSeed = num | 0x0100; // ensure nonzero
    mHead = new HeadIndex(new Node(NULL, sBASE_HEADER, NULL),
                        NULL, NULL, 1);
    return NOERROR;
}

Boolean CConcurrentSkipListMap::CasHead(
    /* [in] */ HeadIndex* cmp,
    /* [in] */ HeadIndex* val)
{
    assert(0 && "TODO");
//    return UNSAFE.compareAndSwapObject(this, headOffset, cmp, val);
    return FALSE;
}

//====================================================================
// CConcurrentSkipListMap::Node::
//====================================================================
CConcurrentSkipListMap::Node::Node(
    /* [in] */ IInterface* key,
    /* [in] */ IInterface* value,
    /* [in] */ Node* next)
{
    mKey = key;
    mValue = value;
    mNext = next;
}

CConcurrentSkipListMap::Node::Node(
    /* [in] */ Node* next)
{
    mKey = NULL;
    mValue = TO_IINTERFACE(this);
    mNext = next;
}

Boolean CConcurrentSkipListMap::Node::CasValue(
    /* [in] */ IInterface* cmp,
    /* [in] */ IInterface* val)
{
    assert(0 && "TODO");
//    return UNSAFE.compareAndSwapObject(this, valueOffset, cmp, val);
    return FALSE;
}

Boolean CConcurrentSkipListMap::Node::CasNext(
    /* [in] */ Node* cmp,
    /* [in] */ Node* val)
{
    assert(0 && "TODO");
//    return UNSAFE.compareAndSwapObject(this, nextOffset, cmp, val);
    return FALSE;
}

Boolean CConcurrentSkipListMap::Node::IsMarker()
{
    Boolean b = Object::Equals(mValue.Get(), TO_IINTERFACE(this));
    return b;
}

Boolean CConcurrentSkipListMap::Node::IsBaseHeader()
{
    Boolean b = Object::Equals(mValue.Get(), sBASE_HEADER);
    return b;
}

Boolean CConcurrentSkipListMap::Node::AppendMarker(
    /* [in] */ Node* f)
{
    return CasNext(f, new Node(f));
}

void CConcurrentSkipListMap::Node::HelpDelete(
    /* [in] */ Node* b,
    /* [in] */ Node* f)
{
    /*
     * Rechecking links and then doing only one of the
     * help-out stages per call tends to minimize CAS
     * interference among helping threads.
     */
    if (Object::Equals(f->Probe(EIID_IInterface), mNext->Probe(EIID_IInterface))
        && Object::Equals(TO_IINTERFACE(this), b->mNext->Probe(EIID_IInterface))) {
        if (f == NULL || !Object::Equals(f->mValue.Get(), f->Probe(EIID_IInterface))) // not already marked
            AppendMarker(f);
        else
            b->CasNext(this, f->mNext);
    }
}

AutoPtr<IInterface> CConcurrentSkipListMap::Node::GetValidValue()
{
    AutoPtr<IInterface> v = mValue;
    if (Object::Equals(v, TO_IINTERFACE(this))
        || Object::Equals(v, sBASE_HEADER) )
        return NULL;
    return v;
}

AutoPtr<AbstractMap::SimpleImmutableEntry> CConcurrentSkipListMap::Node::CreateSnapshot()
{
    AutoPtr<IInterface> v = GetValidValue();
    if (v == NULL)
        return NULL;
    AutoPtr<SimpleImmutableEntry> p = new SimpleImmutableEntry(mKey, v);
    return p;
}

// UNSAFE mechanics

// private static final sun.misc.Unsafe UNSAFE;
// private static final long valueOffset;
// private static final long nextOffset;

// static {
//     try {
//         UNSAFE = sun.misc.Unsafe.getUnsafe();
//         Class<?> k = Node.class;
//         valueOffset = UNSAFE.objectFieldOffset
//             (k.getDeclaredField("value"));
//         nextOffset = UNSAFE.objectFieldOffset
//             (k.getDeclaredField("next"));
//     } catch (Exception e) {
//         throw new Error(e);
//     }
// }

//====================================================================
// CConcurrentSkipListMap::Index::
//====================================================================
CConcurrentSkipListMap::Index::Index(
    /* [in] */ Node* node,
    /* [in] */ Index* down,
    /* [in] */ Index* right)
{
    mNode = node;
    mDown = down;
    mRight = right;
}

Boolean CConcurrentSkipListMap::Index::CasRight(
    /* [in] */ Index* cmp,
    /* [in] */ Index* val)
{
    assert(0 && "TODO");
//    return UNSAFE.compareAndSwapObject(this, rightOffset, cmp, val);
    return FALSE;
}

Boolean CConcurrentSkipListMap::Index::IndexesDeletedNode()
{
    return mNode->mValue == NULL;
}

Boolean CConcurrentSkipListMap::Index::Link(
    /* [in] */ Index* succ,
    /* [in] */ Index* newSucc)
{
    AutoPtr<Node> n = mNode;
    newSucc->mRight = succ;
    return n->mValue != NULL && CasRight(succ, newSucc);
}

Boolean CConcurrentSkipListMap::Index::Unlink(
    /* [in] */ Index* succ)
{
    return !IndexesDeletedNode() && CasRight(succ, succ->mRight);
}

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

//====================================================================
// CConcurrentSkipListMap::HeadIndex::
//====================================================================

CConcurrentSkipListMap::HeadIndex::HeadIndex(
    /* [in] */ Node* node,
    /* [in] */ Index* down,
    /* [in] */ Index* right,
    /* [in] */ Int32 level) : Index(node, down, right)
{
    mLevel = level;
}

//====================================================================
// CConcurrentSkipListMap::ComparableUsingComparator::
//====================================================================
CAR_INTERFACE_IMPL(CConcurrentSkipListMap::ComparableUsingComparator, Object, IComparable)

CConcurrentSkipListMap::ComparableUsingComparator::ComparableUsingComparator(
    /* [in] */ IInterface* key,
    /* [in] */ IComparator* cmp)
{
    mActualKey = key;
    mCmp = cmp;
}

ECode CConcurrentSkipListMap::ComparableUsingComparator::CompareTo(
    /* [in] */ IInterface* another,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    return mCmp->Compare(mActualKey, another, result);
}

//====================================================================
// CConcurrentSkipListMap::
//====================================================================

AutoPtr<IComparable> CConcurrentSkipListMap::Comparable(
    /* [in] */ PInterface key)
{
    if (key == NULL)
        return NULL;
//        throw new NullPointerException();
    AutoPtr<IComparable> p;
    if (mComparator != NULL) {
        p = new ComparableUsingComparator(key, mComparator);
        return p;
    }
    else {
        p = (IComparable*)key->Probe(EIID_IComparable);
        return p;
    }
}

Int32 CConcurrentSkipListMap::Compare(
    /* [in] */ PInterface k1,
    /* [in] */ PInterface k2)
{
    AutoPtr<IComparator> cmp = mComparator;
    if (cmp != NULL) {
        Int32 res;
        cmp->Compare(k1, k2, &res);
        return res;
    }
    else {
        AutoPtr<IComparable> p = (IComparable*)k1->Probe(EIID_IComparable);
        if (p != NULL) {
            Int32 res;
            p->CompareTo(k2, &res);
            return res;
        }
    }
    return -1;
}

Boolean CConcurrentSkipListMap::InHalfOpenRange(
    /* [in] */ PInterface key,
    /* [in] */ PInterface least,
    /* [in] */ PInterface fence)
{
    if (key == NULL)
        return FALSE;
//        throw new NullPointerException();
    return ((least == NULL || Compare(key, least) >= 0) &&
            (fence == NULL || Compare(key, fence) <  0));
}

Boolean CConcurrentSkipListMap::InOpenRange(
    /* [in] */ PInterface key,
    /* [in] */ PInterface least,
    /* [in] */ PInterface fence)
{
    if (key == NULL)
        return FALSE;
//        throw new NullPointerException();
    return ((least == NULL || Compare(key, least) >= 0) &&
            (fence == NULL || Compare(key, fence) <= 0));
}

/* ---------------- Traversal -------------- */

AutoPtr<CConcurrentSkipListMap::Node> CConcurrentSkipListMap::FindPredecessor(
    /* [in] */ IComparable* key)
{
    if (key == NULL)
        return NULL;
//        throw new NullPointerException(); // don't postpone errors
    for (;;) {
        AutoPtr<HeadIndex> q = mHead;
        AutoPtr<Index> r = q->mRight;
        for (;;) {
            if (r != NULL) {
                AutoPtr<Node> n = r->mNode;
                AutoPtr<IInterface> k = n->mKey;
                if (n->mValue == NULL) {
                    if (!q->Unlink(r))
                        break;           // restart
                    r = q->mRight;         // reread r
                    continue;
                }
                Int32 res;
                if ((key->CompareTo(k, &res), res) > 0) {
                    q = (HeadIndex*)(r.Get());
                    r = r->mRight;
                    continue;
                }
            }
            AutoPtr<Index> d = q->mDown;
            if (d != NULL) {
                q = (HeadIndex*)(d.Get());
                r = d->mRight;
            }
            else
                return q->mNode;
        }
    }
    return NULL;
}

AutoPtr<CConcurrentSkipListMap::Node> CConcurrentSkipListMap::FindNode(
    /* [in] */ IComparable* key)
{
    for (;;) {
        AutoPtr<Node> b = FindPredecessor(key);
        AutoPtr<Node> n = b->mNext;
        for (;;) {
            if (n == NULL)
                return NULL;
            AutoPtr<Node> f = n->mNext;
            if (!Object::Equals(n->Probe(EIID_IInterface), b->mNext->Probe(EIID_IInterface)))                // inconsistent read
                break;
            AutoPtr<IInterface> v = n->mValue;
            if (v == NULL) {                // n is deleted
                n->HelpDelete(b, f);
                break;
            }
            if (Object::Equals(v, n->Probe(EIID_IInterface))
                || b->mValue == NULL)  // b is deleted
                break;
            Int32 c = 0;
            key->CompareTo(n->mKey, &c);
            if (c == 0)
                return n;
            if (c < 0)
                return NULL;
            b = n;
            n = f;
        }
    }
}

AutoPtr<IInterface> CConcurrentSkipListMap::DoGet(
    /* [in] */ PInterface okey)
{
    AutoPtr<IComparable> key = Comparable(okey);
    /*
     * Loop needed here and elsewhere in case value field goes
     * null just as it is about to be returned, in which case we
     * lost a race with a deletion, so must retry.
     */
    for (;;) {
        AutoPtr<Node> n = FindNode(key);
        if (n == NULL)
            return NULL;
        AutoPtr<IInterface> v = n->mValue;
        if (v != NULL)
            return v;
    }
}

/* ---------------- Insertion -------------- */

AutoPtr<IInterface> CConcurrentSkipListMap::DoPut(
    /* [in] */ PInterface kkey,
    /* [in] */ PInterface value,
    /* [in] */ Boolean onlyIfAbsent)
{
    AutoPtr<IComparable> key = Comparable(kkey);
    for (;;) {
        AutoPtr<Node> b = FindPredecessor(key);
        AutoPtr<Node> n = b->mNext;
        for (;;) {
            if (n != NULL) {
                AutoPtr<Node> f = n->mNext;
                if (!Object::Equals(n->Probe(EIID_IInterface), b->mNext->Probe(EIID_IInterface)))               // inconsistent read
                    break;
                AutoPtr<IInterface> v = n->mValue;
                if (v == NULL) {               // n is deleted
                    n->HelpDelete(b, f);
                    break;
                }
                if (Object::Equals(v, n->Probe(EIID_IInterface))
                    || b->mValue == NULL) // b is deleted
                    break;
                Int32 c = 0;
                key->CompareTo(n->mKey, &c);
                if (c > 0) {
                    b = n;
                    n = f;
                    continue;
                }
                if (c == 0) {
                    if (onlyIfAbsent || n->CasValue(v, value))
                        return v;
                    else
                        break; // restart if lost race to replace value
                }
                // else c < 0; fall through
            }

            AutoPtr<Node> z = new Node(kkey, value, n);
            if (!b->CasNext(n, z))
                break;         // restart if lost race to append to b
            Int32 level = RandomLevel();
            if (level > 0)
                InsertIndex(z, level);
            return NULL;
        }
    }
}

Int32 CConcurrentSkipListMap::RandomLevel()
{
    Int32 x = mRandomSeed;
    x ^= x << 13;
    x ^= x >> 17;
    mRandomSeed = x ^= x << 5;
    if ((x & 0x80000001) != 0) // test highest and lowest bits
        return 0;
    Int32 level = 1;
    while (((x >>= 1) & 1) != 0) ++level;
    return level;
}

void CConcurrentSkipListMap::InsertIndex(
    /* [in] */ Node* z,
    /* [in] */ Int32 level)
{
    AutoPtr<HeadIndex> h = mHead;
    Int32 max = h->mLevel;

    if (level <= max) {
        AutoPtr<Index> idx = NULL;
        for (Int32 i = 1; i <= level; ++i)
            idx = new Index(z, idx, NULL);
        AddIndex(idx, h, level);
    }
    else { // Add a new level
        /*
         * To reduce interference by other threads checking for
         * empty levels in tryReduceLevel, new levels are added
         * with initialized right pointers. Which in turn requires
         * keeping levels in an array to access them while
         * creating new head index nodes from the opposite
         * direction.
         */
        level = max + 1;
        AutoPtr<ArrayOf<Index*> > idxs = ArrayOf<Index*>::Alloc(level+1);
        AutoPtr<Index> idx = NULL;
        for (Int32 i = 1; i <= level; ++i) {
            idx = new Index(z, idx, NULL);
            idxs->Set(i, idx);
        }

        AutoPtr<HeadIndex> oldh;
        Int32 k;
        for (;;) {
            oldh = mHead;
            Int32 oldLevel = oldh->mLevel;
            if (level <= oldLevel) { // lost race to add level
                k = level;
                break;
            }
            AutoPtr<HeadIndex> newh = oldh;
            AutoPtr<Node> oldbase = oldh->mNode;
            for (Int32 j = oldLevel+1; j <= level; ++j)
                newh = new HeadIndex(oldbase, newh, (*idxs)[j], j);
            if (CasHead(oldh, newh)) {
                k = oldLevel;
                break;
            }
        }
        AddIndex((*idxs)[k], oldh, k);
    }
}

void CConcurrentSkipListMap::AddIndex(
    /* [in] */ Index* idx,
    /* [in] */ HeadIndex* h,
    /* [in] */ Int32 indexLevel)
{
    // Track next level to insert in case of retries
    Int32 insertionLevel = indexLevel;
    AutoPtr<IComparable> key = Comparable(idx->mNode->mKey);
    if (key == NULL) return; //throw new NullPointerException();

    // Similar to findPredecessor, but adding index nodes along
    // path to key.
    for (;;) {
        Int32 j = h->mLevel;
        AutoPtr<Index> q = h;
        AutoPtr<Index> r = q->mRight;
        AutoPtr<Index> t = idx;
        for (;;) {
            if (r != NULL) {
                AutoPtr<Node> n = r->mNode;
                // compare before deletion check avoids needing recheck
                Int32 c = 0;
                key->CompareTo(n->mKey, &c);
                if (n->mValue == NULL) {
                    if (!q->Unlink(r))
                        break;
                    r = q->mRight;
                    continue;
                }
                if (c > 0) {
                    q = r;
                    r = r->mRight;
                    continue;
                }
            }

            if (j == insertionLevel) {
                // Don't insert index if node already deleted
                if (t->IndexesDeletedNode()) {
                    FindNode(key); // cleans up
                    return;
                }
                if (!q->Link(r, t))
                    break; // restart
                if (--insertionLevel == 0) {
                    // need final deletion check before return
                    if (t->IndexesDeletedNode())
                        FindNode(key);
                    return;
                }
            }

            if (--j >= insertionLevel && j < indexLevel)
                t = t->mDown;
            q = q->mDown;
            r = q->mRight;
        }
    }
}

/* ---------------- Deletion -------------- */

AutoPtr<IInterface> CConcurrentSkipListMap::DoRemove(
    /* [in] */ PInterface okey,
    /* [in] */ PInterface value)
{
    AutoPtr<IComparable> key = Comparable(okey);
    for (;;) {
        AutoPtr<Node> b = FindPredecessor(key);
        AutoPtr<Node> n = b->mNext;
        for (;;) {
            if (n == NULL)
                return NULL;
            AutoPtr<Node> f = n->mNext;
            if (!Object::Equals(n->Probe(EIID_IInterface), b->mNext->Probe(EIID_IInterface)))                    // inconsistent read
                break;
            AutoPtr<IInterface> v = n->mValue;
            if (v == NULL) {                    // n is deleted
                n->HelpDelete(b, f);
                break;
            }
            if (Object::Equals(v, n->Probe(EIID_IInterface)) || b->mValue == NULL)      // b is deleted
                break;
            Int32 c = 0;
            key->CompareTo(n->mKey, &c);
            if (c < 0)
                return NULL;
            if (c > 0) {
                b = n;
                n = f;
                continue;
            }
            if (value != NULL && !Object::Equals(value, v))
                return NULL;
            if (!n->CasValue(v, NULL))
                break;
            if (!n->AppendMarker(f) || !b->CasNext(n, f))
                FindNode(key);                  // Retry via findNode
            else {
                FindPredecessor(key);           // Clean index
                if (mHead->mRight == NULL)
                    TryReduceLevel();
            }
            return v;
        }
    }
}

void CConcurrentSkipListMap::TryReduceLevel()
{
    AutoPtr<HeadIndex> h = mHead;
    AutoPtr<HeadIndex> d;
    AutoPtr<HeadIndex> e;
    if (h->mLevel > 3 &&
        (d = (HeadIndex*)(h->mDown.Get())) != NULL &&
        (e = (HeadIndex*)(d->mDown.Get())) != NULL &&
        e->mRight == NULL &&
        d->mRight == NULL &&
        h->mRight == NULL &&
        CasHead(h, d) && // try to set
        h->mRight != NULL) // recheck
        CasHead(d, h);   // try to backout
}

/* ---------------- Finding and removing first element -------------- */

AutoPtr<CConcurrentSkipListMap::Node> CConcurrentSkipListMap::FindFirst()
{
    for (;;) {
        AutoPtr<Node> b = mHead->mNode;
        AutoPtr<Node> n = b->mNext;
        if (n == NULL)
            return NULL;
        if (n->mValue != NULL)
            return n;
        n->HelpDelete(b, n->mNext);
    }
}

AutoPtr<IMapEntry> CConcurrentSkipListMap::DoRemoveFirstEntry()
{
    for (;;) {
        AutoPtr<Node> b = mHead->mNode;
        AutoPtr<Node> n = b->mNext;
        if (n == NULL)
            return NULL;
        AutoPtr<Node> f = n->mNext;
        if (!Object::Equals(n->Probe(EIID_IInterface), b->mNext->Probe(EIID_IInterface)))
            continue;
        AutoPtr<IInterface> v = n->mValue;
        if (v == NULL) {
            n->HelpDelete(b, f);
            continue;
        }
        if (!n->CasValue(v, NULL))
            continue;
        if (!n->AppendMarker(f) || !b->CasNext(n, f))
            FindFirst(); // retry
        ClearIndexToFirst();
        AutoPtr<SimpleImmutableEntry> p = new SimpleImmutableEntry(n->mKey, v);
        AutoPtr<IMapEntry> res = (IMapEntry*)p->Probe(EIID_IMapEntry);
        return res;
    }
}

void CConcurrentSkipListMap::ClearIndexToFirst()
{
    for (;;) {
        AutoPtr<HeadIndex> q = mHead;
        for (;;) {
            AutoPtr<Index> r = q->mRight;
            if (r != NULL && r->IndexesDeletedNode() && !q->Unlink(r))
                break;
            if ((q = (HeadIndex*)(q->mDown.Get())) == NULL) {
                if (mHead->mRight == NULL)
                    TryReduceLevel();
                return;
            }
        }
    }
}

/* ---------------- Finding and removing last element -------------- */

AutoPtr<CConcurrentSkipListMap::Node> CConcurrentSkipListMap::FindLast()
{
    /*
     * findPredecessor can't be used to traverse index level
     * because this doesn't use comparisons.  So traversals of
     * both levels are folded together.
     */
    AutoPtr<HeadIndex> q = mHead;
    for (;;) {
        AutoPtr<Index> d, r;
        if ((r = q->mRight) != NULL) {
            if (r->IndexesDeletedNode()) {
                q->Unlink(r);
                q = mHead; // restart
            }
            else
                q = (HeadIndex*)(r.Get());
        }
        else if ((d = q->mDown) != NULL) {
            q = (HeadIndex*)(d.Get());
        }
        else {
            AutoPtr<Node> b = q->mNode;
            AutoPtr<Node> n = b->mNext;
            for (;;) {
                if (n == NULL)
                    return b->IsBaseHeader() ? NULL : b;
                AutoPtr<Node> f = n->mNext;            // inconsistent read
                if (!Object::Equals(n->Probe(EIID_IInterface), b->mNext->Probe(EIID_IInterface)))
                    break;
                AutoPtr<IInterface> v = n->mValue;
                if (v == NULL) {                 // n is deleted
                    n->HelpDelete(b, f);
                    break;
                }
                if (Object::Equals(v, n->Probe(EIID_IInterface))
                    || b->mValue == NULL)   // b is deleted
                    break;
                b = n;
                n = f;
            }
            q = mHead; // restart
        }
    }
}

AutoPtr<CConcurrentSkipListMap::Node> CConcurrentSkipListMap::FindPredecessorOfLast()
{
    for (;;) {
        AutoPtr<HeadIndex> q = mHead;
        for (;;) {
            AutoPtr<Index> d, r;
            if ((r = q->mRight) != NULL) {
                if (r->IndexesDeletedNode()) {
                    q->Unlink(r);
                    break;    // must restart
                }
                // proceed as far across as possible without overshooting
                if (r->mNode->mNext != NULL) {
                    q = (HeadIndex*)(r.Get());
                    continue;
                }
            }
            if ((d = q->mDown) != NULL)
                q = (HeadIndex*)(d.Get());
            else
                return q->mNode;
        }
    }
}

AutoPtr<IMapEntry> CConcurrentSkipListMap::DoRemoveLastEntry()
{
    for (;;) {
        AutoPtr<Node> b = FindPredecessorOfLast();
        AutoPtr<Node> n = b->mNext;
        if (n == NULL) {
            if (b->IsBaseHeader())               // empty
                return NULL;
            else
                continue; // all b's successors are deleted; retry
        }
        for (;;) {
            AutoPtr<Node> f = n->mNext;
            if (!Object::Equals(n->Probe(EIID_IInterface), b->mNext->Probe(EIID_IInterface)))                    // inconsistent read
                break;
            AutoPtr<IInterface> v = n->mValue;
            if (v == NULL) {                    // n is deleted
                n->HelpDelete(b, f);
                break;
            }
            if (Object::Equals(v, n->Probe(EIID_IInterface)) || b->mValue == NULL)      // b is deleted
                break;
            if (f != NULL) {
                b = n;
                n = f;
                continue;
            }
            if (!n->CasValue(v, NULL))
                break;
            AutoPtr<IInterface> key = n->mKey;
            AutoPtr<IComparable> ck = Comparable(key);
            if (!n->AppendMarker(f) || !b->CasNext(n, f))
                FindNode(ck);                  // Retry via findNode
            else {
                FindPredecessor(ck);           // Clean index
                if (mHead->mRight == NULL)
                    TryReduceLevel();
            }
            AutoPtr<SimpleImmutableEntry> p = new SimpleImmutableEntry(key, v);
            AutoPtr<IMapEntry> res = (IMapEntry*)p->Probe(EIID_IMapEntry);
            return res;
        }
    }
}

/* ---------------- Relational operations -------------- */

// Control values OR'ed as arguments to findNear

Int32 CConcurrentSkipListMap::sEQ = 1;
Int32 CConcurrentSkipListMap::sLT = 2;
Int32 CConcurrentSkipListMap::sGT = 0; // Actually checked as !sLT

AutoPtr<CConcurrentSkipListMap::Node> CConcurrentSkipListMap::FindNear(
    /* [in] */ PInterface kkey,
    /* [in] */ Int32 rel)
{
    AutoPtr<IComparable> key = Comparable(kkey);
    for (;;) {
        AutoPtr<Node> b = FindPredecessor(key);
        AutoPtr<Node> n = b->mNext;
        for (;;) {
            if (n == NULL)
                return ((rel & sLT) == 0 || b->IsBaseHeader()) ? NULL : b;
            AutoPtr<Node> f = n->mNext;
            if (!Object::Equals(n->Probe(EIID_IInterface), b->mNext->Probe(EIID_IInterface)))                  // inconsistent read
                break;
            AutoPtr<IInterface> v = n->mValue;
            if (v == NULL) {                  // n is deleted
                n->HelpDelete(b, f);
                break;
            }
            if (Object::Equals(v, n->Probe(EIID_IInterface)) || b->mValue == NULL)    // b is deleted
                break;
            Int32 c = 0;
            key->CompareTo(n->mKey, &c);
            if ((c == 0 && (rel & sEQ) != 0) ||
                (c <  0 && (rel & sLT) == 0))
                return n;
            if ( c <= 0 && (rel & sLT) != 0)
                return b->IsBaseHeader() ? NULL : b;
            b = n;
            n = f;
        }
    }
}

AutoPtr<AbstractMap::SimpleImmutableEntry> CConcurrentSkipListMap::GetNear(
    /* [in] */ PInterface key,
    /* [in] */ Int32 rel)
{
    for (;;) {
        AutoPtr<Node> n = FindNear(key, rel);
        if (n == NULL)
            return NULL;
        AutoPtr<SimpleImmutableEntry> e = n->CreateSnapshot();
        if (e != NULL)
            return e;
    }
}

/* ---------------- Constructors -------------- */

ECode CConcurrentSkipListMap::constructor()
{
    mComparator = NULL;
    return Initialize();
}

ECode CConcurrentSkipListMap::constructor(
    /* [in] */ IComparator* comparator)
{
    mComparator = comparator;
    return Initialize();
}

ECode CConcurrentSkipListMap::constructor(
    /* [in] */ IMap* m)
{
    mComparator = NULL;
    Initialize();
    return AbstractMap::PutAll(m);
}

ECode CConcurrentSkipListMap::constructor(
    /* [in] */ ISortedMap* m)
{
    m->GetComparator((IComparator**)&mComparator);
    Initialize();
    BuildFromSorted(m);
    return NOERROR;
}

ECode CConcurrentSkipListMap::Clone(
    /* [out] */ IInterface** res)
{
    VALIDATE_NOT_NULL(res);
    AutoPtr<INavigableMap> clone;
    CConcurrentSkipListMap::New((INavigableMap**)&clone);
    CConcurrentSkipListMap* cRes = (CConcurrentSkipListMap*)clone.Get();
    cRes->Initialize();
    cRes->BuildFromSorted(this);
    *res = clone->Probe(EIID_IInterface);
    REFCOUNT_ADD(*res);
    return NOERROR;
}

void CConcurrentSkipListMap::BuildFromSorted(
    /* [in] */ ISortedMap* map)
{
    if (map == NULL)
        return;
    //    throw new NullPointerException();

    AutoPtr<HeadIndex> h = mHead;
    AutoPtr<Node> basepred = h->mNode;

    // Track the current rightmost node at each level. Uses an
    // ArrayList to avoid committing to initial or maximum level.
    AutoPtr<IArrayList> preds;
    CArrayList::New((IArrayList**)&preds);

    // initialize
    for (Int32 i = 0; i <= h->mLevel; ++i)
        (IList::Probe(preds))->Add(NULL);
    AutoPtr<Index> q = h;
    AutoPtr<IInterface> pre;
    for (Int32 i = h->mLevel; i > 0; --i) {
        (IList::Probe(preds))->Set(i, q->Probe(EIID_IInterface), (IInterface**)&pre);
        q = q->mDown;
    }

    AutoPtr<ISet> s;
    (IMap::Probe(map))->GetEntrySet((ISet**)&s);
    AutoPtr<IIterator> it;
    (IIterable::Probe(s))->GetIterator((IIterator**)&it);
    Boolean bNext;
    while ((it->HasNext(&bNext), bNext)) {
        AutoPtr<IInterface> me;
        it->GetNext((IInterface**)&me);
        IMapEntry* e = IMapEntry::Probe(me);
        Int32 j = RandomLevel();
        if (j > h->mLevel) j = h->mLevel + 1;
        AutoPtr<IInterface> k;
        e->GetKey((IInterface**)&k);
        AutoPtr<IInterface> v;
        e->GetValue((IInterface**)&v);
        if (k == NULL || v == NULL)
            return;
//            throw new NullPointerException();
        AutoPtr<Node> z = new Node(k, v, NULL);
        basepred->mNext = z;
        basepred = z;
        if (j > 0) {
            AutoPtr<Index> idx = NULL;
            for (Int32 i = 1; i <= j; ++i) {
                idx = new Index(z, idx, NULL);
                if (i > h->mLevel)
                    h = new HeadIndex(h->mNode, h, idx, i);
                Int32 size = 0;
                (ICollection::Probe(preds))->GetSize(&size);
                if (i < size) {
                    AutoPtr<IInterface> ar;
                    (IList::Probe(preds))->Get(i, (IInterface**)&ar);
                    ((Index*)IObject::Probe(ar))->mRight = idx;
                    AutoPtr<IInterface> pre;
                    (IList::Probe(preds))->Set(i, idx->Probe(EIID_IInterface), (IInterface**)&pre);
                }
                else {
                    (IList::Probe(preds))->Add(idx->Probe(EIID_IInterface));
                }
            }
        }
    }
    mHead = h;
}

/* ---------------- Serialization -------------- */

void CConcurrentSkipListMap::WriteObject(
    /* [in] */ IObjectOutputStream* s)
{
    assert(0 && "TODO");
    // Write out the Comparator and any hidden stuff
    s->DefaultWriteObject();

    // Write out keys and values (alternating)
    for (AutoPtr<Node> n = FindFirst(); n != NULL; n = n->mNext) {
        AutoPtr<IInterface> v = n->GetValidValue();
        if (v != NULL) {
//            s->WriteObject(n->mKey);
//            s->WriteObject(v);
        }
    }
//    s->WriteObject(NULL);
}

void CConcurrentSkipListMap::ReadObject(
    /* [in] */ IObjectInputStream* s)
{
    // Read in the Comparator and any hidden stuff
    s->DefaultReadObject();
    // Reset transients
    Initialize();

    /*
     * This is nearly identical to buildFromSorted, but is
     * distinct because readObject calls can't be nicely adapted
     * as the kind of iterator needed by buildFromSorted. (They
     * can be, but doing so requires type cheats and/or creation
     * of adaptor classes.) It is simpler to just adapt the code.
     */

    AutoPtr<HeadIndex> h = mHead;
    AutoPtr<Node> basepred = h->mNode;
    AutoPtr<IArrayList> preds;
    CArrayList::New((IArrayList**)&preds);
    for (Int32 i = 0; i <= h->mLevel; ++i) {
        (IList::Probe(preds))->Add(NULL);
    }
    AutoPtr<Index> q = h;
    for (Int32 i = h->mLevel; i > 0; --i) {
        AutoPtr<IInterface> pre;
        (IList::Probe(preds))->Set(i, q->Probe(EIID_IInterface), (IInterface**)&pre);
        q = q->mDown;
    }

    for (;;) {
        assert(0 && "TODO");
        AutoPtr<IInterface> k;// = s->ReadObject();
        if (k == NULL)
            break;
        AutoPtr<IInterface> v;// = s->ReadObject();
        if (v == NULL)
            return;
        //    throw new NullPointerException();
        AutoPtr<IInterface> key = k;
        AutoPtr<IInterface> val = v;
        Int32 j = RandomLevel();
        if (j > h->mLevel) j = h->mLevel + 1;
        AutoPtr<Node> z = new Node(key, val, NULL);
        basepred->mNext = z;
        basepred = z;
        if (j > 0) {
            AutoPtr<Index> idx = NULL;
            for (Int32 i = 1; i <= j; ++i) {
                idx = new Index(z, idx, NULL);
                if (i > h->mLevel)
                    h = new HeadIndex(h->mNode, h, idx, i);
                Int32 size = 0;
                (ICollection::Probe(preds))->GetSize(&size);
                if (i < size) {
                    AutoPtr<IInterface> ar;
                    (IList::Probe(preds))->Get(i, (IInterface**)&ar);
                    ((Index*)IObject::Probe(ar))->mRight = idx;
                    AutoPtr<IInterface> pre;
                    (IList::Probe(preds))->Set(i, idx->Probe(EIID_IInterface), (IInterface**)&pre);
                }
                else {
                    (IList::Probe(preds))->Add(idx->Probe(EIID_IInterface));
                }
            }
        }
    }
    mHead = h;
}

/* ------ Map API methods ------ */

ECode CConcurrentSkipListMap::ContainsKey(
    /* [in] */ IInterface* key,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = DoGet(key) != NULL;
    return NOERROR;
}

ECode CConcurrentSkipListMap::Get(
    /* [in] */ IInterface* key,
    /* [out] */ IInterface** value)
{
    VALIDATE_NOT_NULL(value);
    AutoPtr<IInterface> temp = DoGet(key);
    *value = temp;
    REFCOUNT_ADD(*value);
    return NOERROR;
}

ECode CConcurrentSkipListMap::Put(
    /* [in] */ IInterface* key,
    /* [in] */ IInterface* value,
    /* [out] */ IInterface** oldValue)
{
    VALIDATE_NOT_NULL(oldValue);
    if (value == NULL)
        return E_NULL_POINTER_EXCEPTION;
    AutoPtr<IInterface> temp = DoPut(key, value, FALSE);
    *oldValue = temp;
    REFCOUNT_ADD(*oldValue);
    return NOERROR;
}

ECode CConcurrentSkipListMap::Remove(
    /* [in] */ IInterface* key,
    /* [out] */ IInterface** value)
{
    VALIDATE_NOT_NULL(value);
    AutoPtr<IInterface> temp = DoRemove(key, NULL);
    *value = temp;
    REFCOUNT_ADD(*value);
    return NOERROR;
}

ECode CConcurrentSkipListMap::ContainsValue(
    /* [in] */ IInterface* value,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (value == NULL)
        return E_NULL_POINTER_EXCEPTION;
    for (AutoPtr<Node> n = FindFirst(); n != NULL; n = n->mNext) {
        AutoPtr<IInterface> v = n->GetValidValue();
        if (v != NULL && Object::Equals(value, v)) {
            *result = TRUE;
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode CConcurrentSkipListMap::GetSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size);
    Int64 count = 0;
    for (AutoPtr<Node> n = FindFirst(); n != NULL; n = n->mNext) {
        if (n->GetValidValue() != NULL)
            ++count;
    }
    *size = (count >= Elastos::Core::Math::INT32_MAX_VALUE) ? Elastos::Core::Math::INT32_MAX_VALUE : (Int32) count;
    return NOERROR;
}

ECode CConcurrentSkipListMap::IsEmpty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FindFirst() == NULL;
    return NOERROR;
}

ECode CConcurrentSkipListMap::Clear()
{
    return Initialize();
}

/* ---------------- View methods -------------- */

ECode CConcurrentSkipListMap::GetKeySet(
    /* [out] */ ISet** res)
{
    VALIDATE_NOT_NULL(res);
    if (mKeySet == NULL) {
        mKeySet = new _KeySet(this);
    }

    *res = mKeySet;
    REFCOUNT_ADD(*res);
    return NOERROR;
}

ECode CConcurrentSkipListMap::GetNavigableKeySet(
    /* [out] */ INavigableSet** res)
{
    VALIDATE_NOT_NULL(res);
    if (mKeySet == NULL) {
        mKeySet = new _KeySet(this);
    }

    *res = mKeySet;
    REFCOUNT_ADD(*res);
    return NOERROR;
}

ECode CConcurrentSkipListMap::GetValues(
    /* [out] */ ICollection** value)
{
    VALIDATE_NOT_NULL(value);
    if (mValues == NULL) {
        mValues = new _Values(this);
    }
    *value = mValues;
    REFCOUNT_ADD(*value);
    return NOERROR;
}

ECode CConcurrentSkipListMap::GetEntrySet(
    /* [out] */ ISet** entries)
{
    VALIDATE_NOT_NULL(entries);
    if (mEntrySet == NULL) {
        mEntrySet = new _EntrySet(this);
    }
    *entries = mEntrySet;
    REFCOUNT_ADD(*entries);
    return NOERROR;
}

ECode CConcurrentSkipListMap::GetDescendingMap(
    /* [out] */ INavigableMap** res)
{
    VALIDATE_NOT_NULL(res);
    if (mDescendingMap == NULL) {
        mDescendingMap = new _SubMap(this, NULL, FALSE, NULL, FALSE, TRUE);
    }
    *res = mDescendingMap;
    REFCOUNT_ADD(*res);
    return NOERROR;
}

ECode CConcurrentSkipListMap::GetDescendingKeySet(
    /* [out] */ INavigableSet** res)
{
    VALIDATE_NOT_NULL(res);
    AutoPtr<INavigableMap> nm;
    GetDescendingMap((INavigableMap**)&nm);
    return nm->GetNavigableKeySet(res);
}

/* ---------------- AbstractMap Overrides -------------- */

ECode CConcurrentSkipListMap::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (Object::Equals(object, TO_IINTERFACE(this))) {
        *result = TRUE;
        return NOERROR;
    }

    AutoPtr<IMap> m = (IMap*)object->Probe(EIID_IMap);
    if (m == NULL) {
        *result = FALSE;
        return E_NULL_POINTER_EXCEPTION;
    }

    AutoPtr<ISet> entries;
    this->GetEntrySet((ISet**)&entries);
    AutoPtr<ArrayOf<IInterface*> > arr;
    (ICollection::Probe(entries))->ToArray((ArrayOf<IInterface*>**)&arr);
    for (Int32 i = 0;i < arr->GetLength();i++) {
        AutoPtr<IInterface> o = (*arr)[i];
        AutoPtr<IMapEntry> e = (IMapEntry*)o->Probe(EIID_IMapEntry);
        AutoPtr<IInterface> k;
        e->GetKey((IInterface**)&k);
        AutoPtr<IInterface> v;
        m->Get(k, (IInterface**)&v);
        AutoPtr<IInterface> v2;
        e->GetValue((IInterface**)&v2);
        if (!Object::Equals(v2, v)) {
            *result = FALSE;
            return NOERROR;
        }
    }
    AutoPtr<ISet> entries2;
    m->GetEntrySet((ISet**)&entries2);
    AutoPtr<ArrayOf<IInterface*> > arr2;
    (ICollection::Probe(entries2))->ToArray((ArrayOf<IInterface*>**)&arr2);
    for (Int32 i = 0;i < arr2->GetLength();i++) {
        AutoPtr<IInterface> o = (*arr2)[i];
        AutoPtr<IMapEntry> e = (IMapEntry*)o->Probe(EIID_IMapEntry);
        AutoPtr<IInterface> k;
        e->GetKey((IInterface**)&k);
        AutoPtr<IInterface> v;
        e->GetValue((IInterface**)&v);
        AutoPtr<IInterface> v2;
        Get(k, (IInterface**)&v2);
        if (k == NULL || v == NULL || !Object::Equals(v2, v)) {
            *result = FALSE;
            return NOERROR;
        }
    }
    *result = TRUE;
    return NOERROR;
}

/* ------ ConcurrentMap API methods ------ */

ECode CConcurrentSkipListMap::PutIfAbsent(
    /* [in] */ PInterface key,
    /* [in] */ PInterface value,
    /* [out] */ PInterface* out)
{
    VALIDATE_NOT_NULL(out);
    if (value == NULL)
        return E_NULL_POINTER_EXCEPTION;
    AutoPtr<IInterface> temp = DoPut(key, value, TRUE);
    *out = temp;
    REFCOUNT_ADD(*out);
    return NOERROR;
}

ECode CConcurrentSkipListMap::Remove(
    /* [in] */ PInterface key,
    /* [in] */ PInterface value,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    if (key == NULL)
        return E_NULL_POINTER_EXCEPTION;
    if (value == NULL) {
        *res = FALSE;
        return NOERROR;
    }
    *res = DoRemove(key, value) != NULL;
    return NOERROR;
}

ECode CConcurrentSkipListMap::Replace(
    /* [in] */ PInterface key,
    /* [in] */ PInterface oldValue,
    /* [in] */ PInterface newValue,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    if (oldValue == NULL || newValue == NULL)
        return E_NULL_POINTER_EXCEPTION;
    AutoPtr<IComparable> k = Comparable(key);
    for (;;) {
        AutoPtr<Node> n = FindNode(k);
        if (n == NULL) {
            *res = FALSE;
            return NOERROR;
        }
        AutoPtr<IInterface> v = n->mValue;
        if (v != NULL) {
            if (! Object::Equals(oldValue, v)) {
                *res = FALSE;
                return NOERROR;
            }
            if (n->CasValue(v, newValue)) {
                *res = TRUE;
                return NOERROR;
            }
        }
    }
}

ECode CConcurrentSkipListMap::Replace(
    /* [in] */ PInterface key,
    /* [in] */ PInterface value,
    /* [out] */ PInterface* res)
{
    VALIDATE_NOT_NULL(res);
    if (value == NULL)
        return E_NULL_POINTER_EXCEPTION;
    AutoPtr<IComparable> k = Comparable(key);
    for (;;) {
        AutoPtr<Node> n = FindNode(k);
        if (n == NULL)
            return E_NULL_POINTER_EXCEPTION;
        AutoPtr<IInterface> v = n->mValue;
        if (v != NULL && n->CasValue(v, value)) {
            *res = v;
            REFCOUNT_ADD(*res);
            return NOERROR;
        }
    }
}

/* ------ SortedMap API methods ------ */

ECode CConcurrentSkipListMap::GetComparator(
    /* [out] */ IComparator** comp)
{
    VALIDATE_NOT_NULL(comp);
    *comp = mComparator;
    REFCOUNT_ADD(*comp);
    return NOERROR;
}

ECode CConcurrentSkipListMap::GetFirstKey(
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);
    AutoPtr<Node> n = FindFirst();
    if (n == NULL)
        return E_NO_SUCH_ELEMENT_EXCEPTION;
    *outface = n->mKey;
    REFCOUNT_ADD(*outface);
    return NOERROR;
}

ECode CConcurrentSkipListMap::GetLastKey(
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);
    AutoPtr<Node> n = FindLast();
    if (n == NULL)
        return E_NO_SUCH_ELEMENT_EXCEPTION;
    *outface = n->mKey;
    REFCOUNT_ADD(*outface);
    return NOERROR;
}

ECode CConcurrentSkipListMap::GetSubMap(
    /* [in] */ PInterface fromKey,
    /* [in] */ Boolean fromInclusive,
    /* [in] */ PInterface toKey,
    /* [in] */ Boolean toInclusive,
    /* [out] */ INavigableMap** res)
{
    VALIDATE_NOT_NULL(res);
    if (fromKey == NULL || toKey == NULL)
        return E_NULL_POINTER_EXCEPTION;
    AutoPtr<_SubMap> p = new _SubMap(this, fromKey, fromInclusive, toKey, toInclusive, FALSE);
    *res = (INavigableMap*)p->Probe(EIID_INavigableMap);
    REFCOUNT_ADD(*res);
    return NOERROR;
}

ECode CConcurrentSkipListMap::GetHeadMap(
    /* [in] */ PInterface toKey,
    /* [in] */ Boolean inclusive,
    /* [out] */ INavigableMap** res)
{
    VALIDATE_NOT_NULL(res);
    if (toKey == NULL)
        return E_NULL_POINTER_EXCEPTION;
    AutoPtr<_SubMap> p = new _SubMap(this, NULL, FALSE, toKey, inclusive, FALSE);
    *res = (INavigableMap*)p->Probe(EIID_INavigableMap);
    REFCOUNT_ADD(*res);
    return NOERROR;
}

ECode CConcurrentSkipListMap::GetTailMap(
    /* [in] */ PInterface fromKey,
    /* [in] */ Boolean inclusive,
    /* [out] */ INavigableMap** res)
{
    VALIDATE_NOT_NULL(res);
    if (fromKey == NULL)
        return E_NULL_POINTER_EXCEPTION;
    AutoPtr<_SubMap> p = new _SubMap(this, fromKey, inclusive, NULL, FALSE, FALSE);
    *res = (INavigableMap*)p->Probe(EIID_INavigableMap);
    REFCOUNT_ADD(*res);
    return NOERROR;
}

ECode CConcurrentSkipListMap::GetSubMap(
    /* [in] */ PInterface fromKey,
    /* [in] */ PInterface toKey,
    /* [out] */ ISortedMap** res)
{
    VALIDATE_NOT_NULL(res);
    AutoPtr<INavigableMap> p;
    ECode ec = GetSubMap(fromKey, TRUE, toKey, FALSE, (INavigableMap**)&p);
    if (p != NULL) {
        *res = (ISortedMap*)p->Probe(EIID_ISortedMap);
        REFCOUNT_ADD(*res);
    }
    return ec;
}

ECode CConcurrentSkipListMap::GetHeadMap(
    /* [in] */ PInterface toKey,
    /* [out] */ ISortedMap** res)
{
    VALIDATE_NOT_NULL(res);
    AutoPtr<INavigableMap> p;
    ECode ec = GetHeadMap(toKey, FALSE, (INavigableMap**)&p);
    if (p != NULL) {
        *res = (ISortedMap*)p->Probe(EIID_ISortedMap);
        REFCOUNT_ADD(*res);
    }
    return ec;
}

ECode CConcurrentSkipListMap::GetTailMap(
    /* [in] */ PInterface fromKey,
    /* [out] */ ISortedMap** res)
{
    VALIDATE_NOT_NULL(res);
    AutoPtr<INavigableMap> p;
    ECode ec = GetTailMap(fromKey, TRUE, (INavigableMap**)&p);
    if (p != NULL) {
        *res = (ISortedMap*)p->Probe(EIID_ISortedMap);
        REFCOUNT_ADD(*res);
    }
    return ec;
}

/* ---------------- Relational operations -------------- */

ECode CConcurrentSkipListMap::GetLowerEntry(
    /* [in] */ IInterface* key,
    /* [out] */ IMapEntry** outent)
{
    VALIDATE_NOT_NULL(outent);
    AutoPtr<IMapEntry> temp = GetNear(key, sLT);
    *outent = temp;
    REFCOUNT_ADD(*outent);
    return NOERROR;
}

ECode CConcurrentSkipListMap::GetLowerKey(
    /* [in] */ IInterface* key,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);
    *outface = NULL;

    AutoPtr<Node> n = FindNear(key, sLT);
    if (n == NULL) {
        *outface = n->mKey;
        REFCOUNT_ADD(*outface);
    }
    return NOERROR;
}

ECode CConcurrentSkipListMap::GetFloorEntry(
    /* [in] */ IInterface* key,
    /* [out] */ IMapEntry** outent)
{
    VALIDATE_NOT_NULL(outent);
    AutoPtr<IMapEntry> temp = GetNear(key, sLT|sEQ);
    *outent = temp;
    REFCOUNT_ADD(*outent);
    return NOERROR;
}

ECode CConcurrentSkipListMap::GetFloorKey(
    /* [in] */ IInterface* key,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);
    *outface = NULL;

    AutoPtr<Node> n = FindNear(key, sLT|sEQ);
    if ((n == NULL)) {
        *outface = n->mKey;
        REFCOUNT_ADD(*outface);
    }
    return NOERROR;
}

ECode CConcurrentSkipListMap::GetCeilingEntry(
    /* [in] */ IInterface* key,
    /* [out] */ IMapEntry** outent)
{
    VALIDATE_NOT_NULL(outent);
    AutoPtr<IMapEntry> temp = GetNear(key, sGT|sEQ);
    *outent = temp;
    REFCOUNT_ADD(*outent);
    return NOERROR;
}

ECode CConcurrentSkipListMap::GetCeilingKey(
    /* [in] */ IInterface* key,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);
    *outface = NULL;
    AutoPtr<Node> n = FindNear(key, sGT|sEQ);
    if (n == NULL) {
        *outface = n->mKey;
        REFCOUNT_ADD(*outface);
    }
    return NOERROR;
}

ECode CConcurrentSkipListMap::GetHigherEntry(
    /* [in] */ IInterface* key,
    /* [out] */ IMapEntry** outent)
{
    VALIDATE_NOT_NULL(outent);
    AutoPtr<IMapEntry> temp = GetNear(key, sGT);
    *outent = temp;
    REFCOUNT_ADD(*outent);
    return NOERROR;
}

ECode CConcurrentSkipListMap::GetHigherKey(
    /* [in] */ IInterface* key,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);
    *outface = NULL;
    AutoPtr<Node> n = FindNear(key, sGT);
    if (n == NULL) {
        *outface = n->mKey;
        REFCOUNT_ADD(*outface);
    }
    return NOERROR;
}

ECode CConcurrentSkipListMap::GetFirstEntry(
    /* [out] */ IMapEntry** outent)
{
    VALIDATE_NOT_NULL(outent);
    for (;;) {
        AutoPtr<Node> n = FindFirst();
        if (n == NULL) {
            *outent = NULL;
            return NOERROR;
        }
        AutoPtr<SimpleImmutableEntry> e = n->CreateSnapshot();
        if (e != NULL) {
            *outent = e;
            REFCOUNT_ADD(*outent);
            return NOERROR;
        }
    }
}

ECode CConcurrentSkipListMap::GetLastEntry(
    /* [out] */ IMapEntry** outent)
{
    VALIDATE_NOT_NULL(outent);
    for (;;) {
        AutoPtr<Node> n = FindLast();
        if (n == NULL) {
            *outent = NULL;
            return NOERROR;
        }
        AutoPtr<SimpleImmutableEntry> e = n->CreateSnapshot();
        if (e != NULL) {
            *outent = e;
            REFCOUNT_ADD(*outent);
            return NOERROR;
        }
    }
}

ECode CConcurrentSkipListMap::PollFirstEntry(
    /* [out] */ IMapEntry** outent)
{
    VALIDATE_NOT_NULL(outent);
    AutoPtr<IMapEntry> temp = DoRemoveFirstEntry();
    *outent = temp;
    REFCOUNT_ADD(*outent);
    return NOERROR;
}

ECode CConcurrentSkipListMap::PollLastEntry(
    /* [out] */ IMapEntry** outent)
{
    VALIDATE_NOT_NULL(outent);
    AutoPtr<IMapEntry> temp = DoRemoveLastEntry();
    *outent = temp;
    REFCOUNT_ADD(*outent);
    return NOERROR;
}

ECode CConcurrentSkipListMap::GetHashCode(
    /* [out] */ Int32* hash)
{
    return AbstractMap::GetHashCode(hash);
}

ECode CConcurrentSkipListMap::Put(
    /* [in] */ IInterface* key,
    /* [in] */ IInterface* value)
{
    return AbstractMap::Put(key, value);
}

ECode CConcurrentSkipListMap::Remove(
    /* [in] */ IInterface* key)
{
    return AbstractMap::Remove(key);
}

//====================================================================
// CConcurrentSkipListMap::Iter::
//====================================================================
CAR_INTERFACE_IMPL(CConcurrentSkipListMap::Iter, Object, IIterator);

CConcurrentSkipListMap::Iter::Iter(
    /* [in] */ CConcurrentSkipListMap* owner)
{
    mOwner = owner;
    for (;;) {
        mNext = mOwner->FindFirst();
        if (mNext == NULL)
            break;
        AutoPtr<IInterface> x = mNext->mValue;
        if (x != NULL && !Object::Equals(x, mNext->Probe(EIID_IInterface))) {
            mNextValue = x;
            break;
        }
    }
}

ECode CConcurrentSkipListMap::Iter::HasNext(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mNext != NULL;
    return NOERROR;
}

/** Advances next to higher entry. */
ECode CConcurrentSkipListMap::Iter::Advance()
{
    if (mNext == NULL)
        return E_NO_SUCH_ELEMENT_EXCEPTION;
    mLastReturned = mNext;
    for (;;) {
        mNext = mNext->mNext;
        if (mNext == NULL)
            break;
        AutoPtr<IInterface> x = mNext->mValue;
        if (x != NULL && !Object::Equals(x, mNext->Probe(EIID_IInterface))) {
            mNextValue = x;
            break;
        }
    }
    return NOERROR;
}

ECode CConcurrentSkipListMap::Iter::Remove()
{
    AutoPtr<Node> l = mLastReturned;
    if (l == NULL)
        return E_ILLEGAL_STATE_EXCEPTION;
    // It would not be worth all of the overhead to directly
    // unlink from here. Using remove is fast enough.
    AutoPtr<IInterface> pre;
    mOwner->Remove(l->mKey, (IInterface**)&pre);
    mLastReturned = NULL;
    return NOERROR;
}

//====================================================================
// CConcurrentSkipListMap::_ValueIterator::
//====================================================================
CConcurrentSkipListMap::_ValueIterator::_ValueIterator(
    /* [in] */ CConcurrentSkipListMap* owner) : Iter(owner)
{
}

ECode CConcurrentSkipListMap::_ValueIterator::GetNext(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);
    AutoPtr<IInterface> v = mNextValue;
    Advance();
    *object = v;
    REFCOUNT_ADD(*object);
    return NOERROR;
}

//====================================================================
// CConcurrentSkipListMap::_KeyIterator::
//====================================================================
CConcurrentSkipListMap::_KeyIterator::_KeyIterator(
    /* [in] */ CConcurrentSkipListMap* owner) : Iter(owner)
{
}

ECode CConcurrentSkipListMap::_KeyIterator::GetNext(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);
    AutoPtr<Node> n = mNext;
    Advance();
    *object = n->mKey;
    REFCOUNT_ADD(*object);
    return NOERROR;
}

//====================================================================
// CConcurrentSkipListMap::_EntryIterator::
//====================================================================
CConcurrentSkipListMap::_EntryIterator::_EntryIterator(
    /* [in] */ CConcurrentSkipListMap* owner) : Iter(owner)
{
}

ECode CConcurrentSkipListMap::_EntryIterator::GetNext(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);
    AutoPtr<Node> n = mNext;
    AutoPtr<IInterface> v = mNextValue;
    Advance();
    AutoPtr<SimpleImmutableEntry> p = new SimpleImmutableEntry(n->mKey, v);
    *object = p->Probe(EIID_IInterface);
    REFCOUNT_ADD(*object);
    return NOERROR;
}

//====================================================================
// CConcurrentSkipListMap::
//====================================================================

// Factory methods for iterators needed by ConcurrentSkipListSet etc

AutoPtr<IIterator> CConcurrentSkipListMap::KeyIterator()
{
    AutoPtr<_KeyIterator> cRes = new _KeyIterator(this);
    AutoPtr<IIterator> res = (IIterator*)cRes->Probe(EIID_IIterator);
    return res;
}

AutoPtr<IIterator> CConcurrentSkipListMap::ValueIterator()
{
    AutoPtr<_ValueIterator> cRes = new _ValueIterator(this);
    AutoPtr<IIterator> res = (IIterator*)cRes->Probe(EIID_IIterator);
    return res;
}

AutoPtr<IIterator> CConcurrentSkipListMap::EntryIterator()
{
    AutoPtr<_EntryIterator> cRes = new _EntryIterator(this);
    AutoPtr<IIterator> res = (IIterator*)cRes->Probe(EIID_IIterator);
    return res;
}

/* ---------------- View Classes -------------- */

AutoPtr<IList> CConcurrentSkipListMap::ToList(
    /* [in] */ ICollection* c)
{
    // Using size() here would be a pessimization.
    AutoPtr<IArrayList> list;
    CArrayList::New((IArrayList**)&list);
    AutoPtr<ArrayOf<IInterface*> > arr;
    c->ToArray((ArrayOf<IInterface*>**)&arr);
    for (Int32 i = 0;i < arr->GetLength();i++) {
        AutoPtr<IInterface> e = (*arr)[i];
        (IList::Probe(list))->Add(e);
    }
    return (IList*)list->Probe(EIID_IList);
}

ECode CConcurrentSkipListMap::PutAll(
    /* [in] */ IMap* map)
{
    return AbstractMap::PutAll(map);
}

//====================================================================
// CConcurrentSkipListMap::_KeySet::
//====================================================================
CAR_INTERFACE_IMPL_2(CConcurrentSkipListMap::_KeySet, AbstractSet, ISortedSet, INavigableSet);

CConcurrentSkipListMap::_KeySet::_KeySet(
    /* [in] */ INavigableMap* map)
{
    mM = map;
}

ECode CConcurrentSkipListMap::_KeySet::GetSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size);
    return (IMap::Probe(mM))->GetSize(size);
}

ECode CConcurrentSkipListMap::_KeySet::IsEmpty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return (IMap::Probe(mM))->IsEmpty(result);
}

ECode CConcurrentSkipListMap::_KeySet::Contains(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return (IMap::Probe(mM))->ContainsKey(object, result);
}

ECode CConcurrentSkipListMap::_KeySet::Remove(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    VALIDATE_NOT_NULL(modified);
    AutoPtr<IInterface> res;
    (IMap::Probe(mM))->Remove(object, (IInterface**)&res);
    *modified = res != NULL;
    return NOERROR;
}

ECode CConcurrentSkipListMap::_KeySet::Clear()
{
    return (IMap::Probe(mM))->Clear();
}

ECode CConcurrentSkipListMap::_KeySet::GetLower(
    /* [in] */ IInterface* e,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);
    return mM->GetLowerKey(e, outface);
}

ECode CConcurrentSkipListMap::_KeySet::GetFloor(
    /* [in] */ IInterface* e,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);
    return mM->GetFloorKey(e, outface);
}

ECode CConcurrentSkipListMap::_KeySet::GetCeiling(
    /* [in] */ IInterface* e,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);
    return mM->GetCeilingKey(e, outface);
}

ECode CConcurrentSkipListMap::_KeySet::GetHigher(
    /* [in] */ IInterface* e,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);
    return mM->GetHigherKey(e, outface);
}

ECode CConcurrentSkipListMap::_KeySet::GetComparator(
    /* [out] */ IComparator** outcom)
{
    VALIDATE_NOT_NULL(outcom);
    return (ISortedMap::Probe(mM))->GetComparator(outcom);
}

ECode CConcurrentSkipListMap::_KeySet::GetFirst(
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);
    return (ISortedMap::Probe(mM))->GetFirstKey(outface);
}

ECode CConcurrentSkipListMap::_KeySet::GetLast(
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);
    return (ISortedMap::Probe(mM))->GetLastKey(outface);
}

ECode CConcurrentSkipListMap::_KeySet::PollFirst(
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);
    AutoPtr<IMapEntry> e;
    mM->PollFirstEntry((IMapEntry**)&e);
    if (e == NULL) {
        *outface = NULL;
        return NOERROR;
    }
    else
        return e->GetKey(outface);
}

ECode CConcurrentSkipListMap::_KeySet::PollLast(
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);
    AutoPtr<IMapEntry> e;
    mM->PollLastEntry((IMapEntry**)&e);
    if (e == NULL) {
        *outface = NULL;
        return NOERROR;
    }
    else
        return e->GetKey(outface);
}

ECode CConcurrentSkipListMap::_KeySet::GetIterator(
    /* [out] */ IIterator** it)
{
    VALIDATE_NOT_NULL(it);
    *it = NULL;

    AutoPtr<INavigableMap> p = (INavigableMap*)mM->Probe(EIID_INavigableMap);
    if (p != NULL) {
        AutoPtr<CConcurrentSkipListMap> res = (CConcurrentSkipListMap*)p.Get();
        AutoPtr<IIterator> temp = res->KeyIterator();
        *it = temp;
        REFCOUNT_ADD(*it);
        return NOERROR;
    }
    else {
        AutoPtr<_SubMap> res = (_SubMap*)mM.Get();
        AutoPtr<IIterator> temp = res->KeyIterator();
        *it = temp;
        REFCOUNT_ADD(*it);
        return NOERROR;
    }
}

ECode CConcurrentSkipListMap::_KeySet::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (Object::Equals(object, TO_IINTERFACE(this))) {
        *result = TRUE;
        return NOERROR;
    }
    if (object->Probe(EIID_ISet) == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<ICollection> c = (ICollection*)object->Probe(EIID_ICollection);
    if (c == NULL) {
        *result = FALSE;
        return E_NULL_POINTER_EXCEPTION;
    }
    Boolean a = FALSE, b = FALSE;
    ContainsAll(c, &a);
    c->ContainsAll(this, &b);
    *result = a && b;
    return NOERROR;
}

ECode CConcurrentSkipListMap::_KeySet::ToArray(
    /* [out, callee] */ ArrayOf<IInterface*>** array)
{
    VALIDATE_NOT_NULL(array);
    return ICollection::Probe(ToList(this))->ToArray(array);
}

ECode CConcurrentSkipListMap::_KeySet::ToArray(
    /* [in] */ ArrayOf<IInterface*>* inArray,
    /* [out, callee] */ ArrayOf<IInterface*>** outArray)
{
    VALIDATE_NOT_NULL(outArray);
    return ICollection::Probe(ToList(this))->ToArray(inArray, outArray);
}

ECode CConcurrentSkipListMap::_KeySet::GetDescendingIterator(
    /* [out] */ IIterator** outiter)
{
    VALIDATE_NOT_NULL(outiter);
    AutoPtr<INavigableSet> s;
    GetDescendingSet((INavigableSet**)&s);
    return s->GetIterator(outiter);
}

ECode CConcurrentSkipListMap::_KeySet::GetSubSet(
    /* [in] */ IInterface* fromElement,
    /* [in] */ Boolean fromInclusive,
    /* [in] */ IInterface* toElement,
    /* [in] */ Boolean toInclusive,
    /* [out] */ INavigableSet** outnav)
{
    VALIDATE_NOT_NULL(outnav);
    AutoPtr<INavigableMap> m;
    mM->GetSubMap(fromElement, fromInclusive,
                toElement, toInclusive, (INavigableMap**)&m);
    *outnav = new _KeySet(m);
    REFCOUNT_ADD(*outnav);
    return NOERROR;
}

ECode CConcurrentSkipListMap::_KeySet::GetHeadSet(
    /* [in] */ IInterface* toElement,
    /* [in] */ Boolean inclusive,
    /* [out] */ INavigableSet** outnav)
{
    VALIDATE_NOT_NULL(outnav);
    AutoPtr<INavigableMap> m;
    mM->GetHeadMap(toElement, inclusive, (INavigableMap**)&m);
    *outnav = new _KeySet(m);
    REFCOUNT_ADD(*outnav);
    return NOERROR;
}

ECode CConcurrentSkipListMap::_KeySet::GetTailSet(
    /* [in] */ IInterface* fromElement,
    /* [in] */ Boolean inclusive,
    /* [out] */ INavigableSet** outnav)
{
    VALIDATE_NOT_NULL(outnav);
    AutoPtr<INavigableMap> m;
    mM->GetTailMap(fromElement, inclusive, (INavigableMap**)&m);
    *outnav = new _KeySet(m);
    REFCOUNT_ADD(*outnav);
    return NOERROR;
}

ECode CConcurrentSkipListMap::_KeySet::GetSubSet(
    /* [in] */ IInterface* start,
    /* [in] */ IInterface* end,
    /* [out] */ ISortedSet** outsort)
{
    VALIDATE_NOT_NULL(outsort);
    AutoPtr<INavigableSet> p;
    GetSubSet(start, TRUE, end, FALSE, (INavigableSet**)&p);
    AutoPtr<ISortedSet> res = (ISortedSet*)p->Probe(EIID_ISortedSet);
    *outsort = res;
    REFCOUNT_ADD(*outsort);
    return NOERROR;
}

ECode CConcurrentSkipListMap::_KeySet::GetHeadSet(
    /* [in] */ IInterface* end,
    /* [out] */ ISortedSet** outsort)
{
    VALIDATE_NOT_NULL(outsort);
    AutoPtr<INavigableSet> p;
    GetHeadSet(end, FALSE, (INavigableSet**)&p);
    AutoPtr<ISortedSet> res = (ISortedSet*)p->Probe(EIID_ISortedSet);
    *outsort = res;
    REFCOUNT_ADD(*outsort);
    return NOERROR;
}

ECode CConcurrentSkipListMap::_KeySet::GetTailSet(
    /* [in] */ IInterface* start,
    /* [out] */ ISortedSet** outsort)
{
    VALIDATE_NOT_NULL(outsort);
    AutoPtr<INavigableSet> p;
    GetTailSet(start, TRUE, (INavigableSet**)&p);
    AutoPtr<ISortedSet> res = (ISortedSet*)p->Probe(EIID_ISortedSet);
    *outsort = res;
    REFCOUNT_ADD(*outsort);
    return NOERROR;
}

ECode CConcurrentSkipListMap::_KeySet::GetDescendingSet(
    /* [out] */ INavigableSet** outnav)
{
    VALIDATE_NOT_NULL(outnav);
    AutoPtr<INavigableMap> m;
    mM->GetDescendingMap((INavigableMap**)&m);
    AutoPtr<INavigableMap> p = (INavigableMap*)m->Probe(EIID_INavigableMap);
    AutoPtr<_KeySet> res = new _KeySet(p);
    *outnav = (INavigableSet*)res->Probe(EIID_INavigableSet);
    REFCOUNT_ADD(*outnav);
    return NOERROR;
}

ECode CConcurrentSkipListMap::_KeySet::Add(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    return AbstractSet::Add(obj, result);
}

ECode CConcurrentSkipListMap::_KeySet::Add(
    /* [in] */ IInterface* obj)
{
    return AbstractSet::Add(obj);
}

ECode CConcurrentSkipListMap::_KeySet::AddAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    return AbstractSet::AddAll(collection, result);
}

ECode CConcurrentSkipListMap::_KeySet::AddAll(
    /* [in] */ ICollection* collection)
{
    return AbstractSet::AddAll(collection);
}

ECode CConcurrentSkipListMap::_KeySet::ContainsAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    return AbstractSet::ContainsAll(collection, result);
}

ECode CConcurrentSkipListMap::_KeySet::GetHashCode(
    /* [out] */ Int32* result)
{
    return AbstractSet::GetHashCode(result);
}

ECode CConcurrentSkipListMap::_KeySet::Remove(
    /* [in] */ IInterface* obj)
{
    return AbstractSet::Remove(obj);
}

ECode CConcurrentSkipListMap::_KeySet::RemoveAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    return AbstractSet::RemoveAll(collection, result);
}

ECode CConcurrentSkipListMap::_KeySet::RemoveAll(
    /* [in] */ ICollection* collection)
{
    return AbstractSet::RemoveAll(collection);
}

ECode CConcurrentSkipListMap::_KeySet::RetainAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    return AbstractSet::RetainAll(collection, result);
}

ECode CConcurrentSkipListMap::_KeySet::RetainAll(
    /* [in] */ ICollection* collection)
{
    return AbstractSet::RetainAll(collection);
}

//====================================================================
// CConcurrentSkipListMap::_Values::
//====================================================================
CConcurrentSkipListMap::_Values::_Values(
    /* [in] */ INavigableMap* map)
{
    mM = map;
}

ECode CConcurrentSkipListMap::_Values::GetIterator(
    /* [out] */ IIterator** it)
{
    VALIDATE_NOT_NULL(it);
    AutoPtr<INavigableMap> p = (INavigableMap*)mM->Probe(EIID_INavigableMap);
    if (p != NULL) {
        AutoPtr<CConcurrentSkipListMap> cp = (CConcurrentSkipListMap*)p.Get();
        AutoPtr<IIterator> res = cp->ValueIterator();
        *it = res;
        REFCOUNT_ADD(*it);
        return NOERROR;
    }
    else {
        AutoPtr<_SubMap> p2 = (_SubMap*)mM.Get();
        AutoPtr<IIterator> res = p2->ValueIterator();
        *it = res;
        REFCOUNT_ADD(*it);
        return NOERROR;
    }
}

ECode CConcurrentSkipListMap::_Values::IsEmpty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return IMap::Probe(mM)->IsEmpty(result);
}

ECode CConcurrentSkipListMap::_Values::GetSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size);
    return IMap::Probe(mM)->GetSize(size);
}

ECode CConcurrentSkipListMap::_Values::Contains(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return IMap::Probe(mM)->ContainsValue(object, result);
}

ECode CConcurrentSkipListMap::_Values::Clear()
{
    return IMap::Probe(mM)->Clear();
}

ECode CConcurrentSkipListMap::_Values::ToArray(
    /* [out, callee] */ ArrayOf<IInterface*>** array)
{
    VALIDATE_NOT_NULL(array);
    return ICollection::Probe(ToList(this))->ToArray(array);
}

ECode CConcurrentSkipListMap::_Values::ToArray(
    /* [in] */ ArrayOf<IInterface*>* inArray,
    /* [out, callee] */ ArrayOf<IInterface*>** outArray)
{
    VALIDATE_NOT_NULL(outArray);
    return ICollection::Probe(ToList(this))->ToArray(inArray, outArray);
}

//====================================================================
// CConcurrentSkipListMap::_EntrySet::
//====================================================================
CConcurrentSkipListMap::_EntrySet::_EntrySet(
    /* [in] */ INavigableMap* map)
{
    mM = map;
}

ECode CConcurrentSkipListMap::_EntrySet::GetIterator(
    /* [out] */ IIterator** it)
{
    VALIDATE_NOT_NULL(it);
    AutoPtr<INavigableMap> p = (INavigableMap*)mM->Probe(EIID_INavigableMap);
    if (p != NULL) {
        AutoPtr<CConcurrentSkipListMap> cp = (CConcurrentSkipListMap*)p.Get();
        AutoPtr<IIterator> res = cp->EntryIterator();
        *it = res;
        REFCOUNT_ADD(*it);
        return NOERROR;
    }
    else {
        AutoPtr<_SubMap> p2 = (_SubMap*)mM.Get();
        AutoPtr<IIterator> res = p2->EntryIterator();
        *it = res;
        REFCOUNT_ADD(*it);
        return NOERROR;
    }
}

ECode CConcurrentSkipListMap::_EntrySet::Contains(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IMapEntry> e = (IMapEntry*)object->Probe(EIID_IMapEntry);
    if (e == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<IInterface> k;
    e->GetKey((IInterface**)&k);
    AutoPtr<IInterface> v;
    IMap::Probe(mM)->Get(k, (IInterface**)&v);
    if (v != NULL) {
        AutoPtr<IInterface> v2;
        e->GetValue((IInterface**)&v2);
        *result = Object::Equals(v, v2);
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode CConcurrentSkipListMap::_EntrySet::Remove(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    VALIDATE_NOT_NULL(modified);
    if (object->Probe(EIID_IMapEntry) == NULL) {
        *modified = FALSE;
        return NOERROR;
    }
    AutoPtr<IMapEntry> e = (IMapEntry*)object->Probe(EIID_IMapEntry);
    AutoPtr<IInterface> k;
    e->GetKey((IInterface**)&k);
    AutoPtr<IInterface> v;
    e->GetValue((IInterface**)&v);
    AutoPtr<IConcurrentMap> res = (IConcurrentMap*)mM->Probe(EIID_IConcurrentMap);
    return res->Remove(k, v, modified);
}

ECode CConcurrentSkipListMap::_EntrySet::IsEmpty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return IMap::Probe(mM)->IsEmpty(result);
}

ECode CConcurrentSkipListMap::_EntrySet::GetSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size);
    return IMap::Probe(mM)->GetSize(size);
}

ECode CConcurrentSkipListMap::_EntrySet::Clear()
{
    return IMap::Probe(mM)->Clear();
}

ECode CConcurrentSkipListMap::_EntrySet::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (Object::Equals(object, TO_IINTERFACE(this))) {
        *result = TRUE;
        return NOERROR;
    }
    if (object->Probe(EIID_ISet) == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<ICollection> c = (ICollection*)object->Probe(EIID_ICollection);
    if (c == NULL) {
        *result = FALSE;
        return E_NULL_POINTER_EXCEPTION;
    }
    Boolean a = FALSE, b = FALSE;
    ContainsAll(c, &a);
    c->ContainsAll(this, &b);
    *result = a && b;
    return NOERROR;
}

ECode CConcurrentSkipListMap::_EntrySet::ToArray(
    /* [out, callee] */ ArrayOf<IInterface*>** array)
{
    VALIDATE_NOT_NULL(array);
    return ICollection::Probe(ToList(this))->ToArray(array);
}

ECode CConcurrentSkipListMap::_EntrySet::ToArray(
    /* [in] */ ArrayOf<IInterface*>* inArray,
    /* [out, callee] */ ArrayOf<IInterface*>** outArray)
{
    VALIDATE_NOT_NULL(outArray);
    return ICollection::Probe(ToList(this))->ToArray(inArray, outArray);
}


ECode CConcurrentSkipListMap::_EntrySet::Add(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    return AbstractSet::Add(obj, result);
}

ECode CConcurrentSkipListMap::_EntrySet::Add(
    /* [in] */ IInterface* obj)
{
    return AbstractSet::Add(obj);
}

ECode CConcurrentSkipListMap::_EntrySet::AddAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    return AbstractSet::AddAll(collection, result);
}

ECode CConcurrentSkipListMap::_EntrySet::AddAll(
    /* [in] */ ICollection* collection)
{
    return AbstractSet::AddAll(collection);
}

ECode CConcurrentSkipListMap::_EntrySet::ContainsAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    return AbstractSet::ContainsAll(collection, result);
}

ECode CConcurrentSkipListMap::_EntrySet::GetHashCode(
    /* [out] */ Int32* result)
{
    return AbstractSet::GetHashCode(result);
}

ECode CConcurrentSkipListMap::_EntrySet::Remove(
    /* [in] */ IInterface* obj)
{
    return AbstractSet::Remove(obj);
}

ECode CConcurrentSkipListMap::_EntrySet::RemoveAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    return AbstractSet::RemoveAll(collection, result);
}

ECode CConcurrentSkipListMap::_EntrySet::RemoveAll(
    /* [in] */ ICollection* collection)
{
    return AbstractSet::RemoveAll(collection);
}

ECode CConcurrentSkipListMap::_EntrySet::RetainAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    return AbstractSet::RetainAll(collection, result);
}

ECode CConcurrentSkipListMap::_EntrySet::RetainAll(
    /* [in] */ ICollection* collection)
{
    return AbstractSet::RetainAll(collection);
}

//====================================================================
// CConcurrentSkipListMap::_SubMap::
//====================================================================
CAR_INTERFACE_IMPL_3(CConcurrentSkipListMap::_SubMap, AbstractMap, INavigableMap, IConcurrentMap, ISortedMap)

CConcurrentSkipListMap::_SubMap::_SubMap(
    /* [in] */ CConcurrentSkipListMap* map,
    /* [in] */ IInterface* fromKey,
    /* [in] */ Boolean fromInclusive,
    /* [in] */ IInterface* toKey,
    /* [in] */ Boolean toInclusive,
    /* [in] */ Boolean isDescending)
{
    if (fromKey != NULL && toKey != NULL &&
        map->Compare(fromKey, toKey) > 0)
        return;

    mM = map;
    mLo = fromKey;
    mHi = toKey;
    mLoInclusive = fromInclusive;
    mHiInclusive = toInclusive;
    mIsDescending = isDescending;
}

/* ----------------  Utilities -------------- */

Boolean CConcurrentSkipListMap::_SubMap::TooLow(
    /* [in] */ IInterface* key)
{
    if (mLo != NULL) {
        Int32 c = mM->Compare(key, mLo);
        if (c < 0 || (c == 0 && !mLoInclusive))
            return TRUE;
    }
    return FALSE;
}

Boolean CConcurrentSkipListMap::_SubMap::TooHigh(
    /* [in] */ PInterface key)
{
    if (mHi != NULL) {
        Int32 c = mM->Compare(key, mHi);
        if (c > 0 || (c == 0 && !mHiInclusive))
            return TRUE;
    }
    return FALSE;
}

Boolean CConcurrentSkipListMap::_SubMap::InBounds(
    /* [in] */ PInterface key)
{
    return !TooLow(key) && !TooHigh(key);
}

ECode CConcurrentSkipListMap::_SubMap::CheckKeyBounds(
    /* [in] */ PInterface key)
{
    if (key == NULL)
        return E_NULL_POINTER_EXCEPTION;
    if (!InBounds(key))
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    return NOERROR;
}

Boolean CConcurrentSkipListMap::_SubMap::IsBeforeEnd(
    /* [in] */ Node* n)
{
    if (n == NULL)
        return FALSE;
    if (mHi == NULL)
        return TRUE;
    AutoPtr<IInterface> k = n->mKey;
    if (k == NULL) // pass by markers and headers
        return TRUE;
    Int32 c = mM->Compare(k, mHi);
    if (c > 0 || (c == 0 && !mHiInclusive))
        return FALSE;
    return TRUE;
}

AutoPtr<CConcurrentSkipListMap::Node> CConcurrentSkipListMap::_SubMap::LoNode()
{
    if (mLo == NULL)
        return mM->FindFirst();
    else if (mLoInclusive)
        return mM->FindNear(mLo, sGT|sEQ);
    else
        return mM->FindNear(mLo, sGT);
}

AutoPtr<CConcurrentSkipListMap::Node> CConcurrentSkipListMap::_SubMap::HiNode()
{
    if (mHi == NULL)
        return mM->FindLast();
    else if (mHiInclusive)
        return mM->FindNear(mHi, sLT|sEQ);
    else
        return mM->FindNear(mHi, sLT);
}

AutoPtr<IInterface> CConcurrentSkipListMap::_SubMap::LowestKey()
{
    AutoPtr<Node> n = LoNode();
    if (IsBeforeEnd(n))
        return n->mKey;
    else
        return NULL;
    //    throw new NoSuchElementException();
}

AutoPtr<IInterface> CConcurrentSkipListMap::_SubMap::HighestKey()
{
    AutoPtr<Node> n = HiNode();
    if (n != NULL) {
        AutoPtr<IInterface> last = n->mKey;
        if (InBounds(last))
            return last;
    }
    return NULL;
//    throw new NoSuchElementException();
}

AutoPtr<IMapEntry> CConcurrentSkipListMap::_SubMap::LowestEntry()
{
    for (;;) {
        AutoPtr<Node> n = LoNode();
        if (!IsBeforeEnd(n))
            return NULL;
        AutoPtr<IMapEntry> e = n->CreateSnapshot();
        if (e != NULL)
            return e;
    }
}

AutoPtr<IMapEntry> CConcurrentSkipListMap::_SubMap::HighestEntry()
{
    for (;;) {
        AutoPtr<Node> n = HiNode();
        if (n == NULL || !InBounds(n->mKey))
            return NULL;
        AutoPtr<IMapEntry> e = n->CreateSnapshot();
        if (e != NULL)
            return e;
    }
}

AutoPtr<IMapEntry> CConcurrentSkipListMap::_SubMap::RemoveLowest()
{
    for (;;) {
        AutoPtr<Node> n = LoNode();
        if (n == NULL)
            return NULL;
        AutoPtr<IInterface> k = n->mKey;
        if (!InBounds(k))
            return NULL;
        AutoPtr<IInterface> v = mM->DoRemove(k, NULL);
        if (v != NULL) {
            AutoPtr<SimpleImmutableEntry> p = new SimpleImmutableEntry(k, v);
            AutoPtr<IMapEntry> res = (IMapEntry*)p->Probe(EIID_IMapEntry);
            return res;
        }
    }
}

AutoPtr<IMapEntry> CConcurrentSkipListMap::_SubMap::RemoveHighest()
{
    for (;;) {
        AutoPtr<Node> n = HiNode();
        if (n == NULL)
            return NULL;
        AutoPtr<IInterface> k = n->mKey;
        if (!InBounds(k))
            return NULL;
        AutoPtr<IInterface> v = mM->DoRemove(k, NULL);
        if (v != NULL) {
            AutoPtr<SimpleImmutableEntry> p = new SimpleImmutableEntry(k, v);
            AutoPtr<IMapEntry> res = (IMapEntry*)p->Probe(EIID_IMapEntry);
            return res;
        }
    }
}

AutoPtr<IMapEntry> CConcurrentSkipListMap::_SubMap::GetNearEntry(
    /* [in] */ PInterface key,
    /* [in] */ Int32 rel)
{
    if (mIsDescending) { // adjust relation for direction
        if ((rel & sLT) == 0)
            rel |= sLT;
        else
            rel &= ~sLT;
    }
    if (TooLow(key))
        return ((rel & sLT) != 0) ? NULL : LowestEntry();
    if (TooHigh(key))
        return ((rel & sLT) != 0) ? HighestEntry() : NULL;
    for (;;) {
        AutoPtr<Node> n = mM->FindNear(key, rel);
        if (n == NULL || !InBounds(n->mKey))
            return NULL;
        AutoPtr<IInterface> k = n->mKey;
        AutoPtr<IInterface> v = n->GetValidValue();
        if (v != NULL) {
            AutoPtr<SimpleImmutableEntry> p = new SimpleImmutableEntry(k, v);
            AutoPtr<IMapEntry> res = (IMapEntry*)p->Probe(EIID_IMapEntry);
            return res;
        }
    }
}

// Almost the same as getNearEntry, except for keys
AutoPtr<IInterface> CConcurrentSkipListMap::_SubMap::GetNearKey(
    /* [in] */ PInterface key,
    /* [in] */ Int32 rel)
{
    if (mIsDescending) { // adjust relation for direction
        if ((rel & sLT) == 0)
            rel |= sLT;
        else
            rel &= ~sLT;
    }
    if (TooLow(key)) {
        if ((rel & sLT) == 0) {
            AutoPtr<Node> n = LoNode();
            if (IsBeforeEnd(n))
                return n->mKey;
        }
        return NULL;
    }
    if (TooHigh(key)) {
        if ((rel & sLT) != 0) {
            AutoPtr<Node> n = HiNode();
            if (n != NULL) {
                AutoPtr<IInterface> last = n->mKey;
                if (InBounds(last))
                    return last;
            }
        }
        return NULL;
    }
    for (;;) {
        AutoPtr<Node> n = mM->FindNear(key, rel);
        if (n == NULL || !InBounds(n->mKey))
            return NULL;
        AutoPtr<IInterface> k = n->mKey;
        AutoPtr<IInterface> v = n->GetValidValue();
        if (v != NULL)
            return k;
    }
}

/* ----------------  Map API methods -------------- */

ECode CConcurrentSkipListMap::_SubMap::ContainsKey(
    /* [in] */ IInterface* key,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (key == NULL) return E_NULL_POINTER_EXCEPTION;
    AutoPtr<IInterface> k = key;
    Boolean b = FALSE;
    *result = InBounds(k) && (mM->ContainsKey(k, &b), b);
    return NOERROR;
}

ECode CConcurrentSkipListMap::_SubMap::Get(
    /* [in] */ IInterface* key,
    /* [out] */ IInterface** value)
{
    VALIDATE_NOT_NULL(value);
    if (key == NULL) return E_NULL_POINTER_EXCEPTION;
    AutoPtr<IInterface> k = key;
    if (!InBounds(k)) {
        *value = NULL;
        return NOERROR;
    }
    else {
        return mM->Get(k, value);
    }
}

ECode CConcurrentSkipListMap::_SubMap::Put(
    /* [in] */ IInterface* key,
    /* [in] */ IInterface* value,
    /* [out] */ IInterface** oldValue)
{
    VALIDATE_NOT_NULL(oldValue);
    CheckKeyBounds(key);
    return mM->Put(key, value, oldValue);
}

ECode CConcurrentSkipListMap::_SubMap::Remove(
    /* [in] */ IInterface* key,
    /* [out] */ IInterface** value)
{
    VALIDATE_NOT_NULL(value);
    AutoPtr<IInterface> k = key;
    if (!InBounds(k)) {
        *value = NULL;
        return NOERROR;
    }
    else {
        return mM->Remove(k, value);
    }
}

ECode CConcurrentSkipListMap::_SubMap::GetSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size);
    Int64 count = 0;
    for (AutoPtr<Node> n = LoNode();
         IsBeforeEnd(n);
         n = n->mNext) {
        if (n->GetValidValue() != NULL)
            ++count;
    }
    *size = count >= Elastos::Core::Math::INT32_MAX_VALUE ? Elastos::Core::Math::INT32_MAX_VALUE : (Int32)count;
    return NOERROR;
}

ECode CConcurrentSkipListMap::_SubMap::IsEmpty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = !IsBeforeEnd(LoNode());
    return NOERROR;
}

ECode CConcurrentSkipListMap::_SubMap::ContainsValue(
    /* [in] */ IInterface* value,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (value == NULL) {
        *result = FALSE;
        return E_NULL_POINTER_EXCEPTION;
    }
    for (AutoPtr<Node> n = LoNode();
         IsBeforeEnd(n);
         n = n->mNext) {
        AutoPtr<IInterface> v = n->GetValidValue();
        if (v != NULL && Object::Equals(value, v)) {
            *result = TRUE;
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode CConcurrentSkipListMap::_SubMap::Clear()
{
    for (AutoPtr<Node> n = LoNode();
         IsBeforeEnd(n);
         n = n->mNext) {
        if (n->GetValidValue() != NULL) {
            AutoPtr<IInterface> pre;
            mM->Remove(n->mKey, (IInterface**)&pre);
        }
    }
    return NOERROR;
}

/* ----------------  ConcurrentMap API methods -------------- */

ECode CConcurrentSkipListMap::_SubMap::PutIfAbsent(
    /* [in] */ PInterface key,
    /* [in] */ PInterface value,
    /* [out] */ PInterface* out)
{
    VALIDATE_NOT_NULL(out);
    CheckKeyBounds(key);
    return mM->PutIfAbsent(key, value, out);
}

ECode CConcurrentSkipListMap::_SubMap::Remove(
    /* [in] */ PInterface key,
    /* [in] */ PInterface value,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    AutoPtr<IInterface> k = key;
    Boolean b = FALSE;
    *res = InBounds(k) && (mM->Remove(k, value, &b), b);
    return NOERROR;
}

ECode CConcurrentSkipListMap::_SubMap::Replace(
            /* [in] */ PInterface key,
            /* [in] */ PInterface oldValue,
            /* [in] */ PInterface newValue,
            /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    CheckKeyBounds(key);
    return mM->Replace(key, oldValue, newValue, res);
}

ECode CConcurrentSkipListMap::_SubMap::Replace(
    /* [in] */ PInterface key,
    /* [in] */ PInterface value,
    /* [out] */ PInterface* res)
{
    VALIDATE_NOT_NULL(res);
    CheckKeyBounds(key);
    return mM->Replace(key, value, res);
}

/* ----------------  SortedMap API methods -------------- */

ECode CConcurrentSkipListMap::_SubMap::GetComparator(
    /* [out] */ IComparator** comp)
{
    VALIDATE_NOT_NULL(comp);
    AutoPtr<IComparator> cmp;
    mM->GetComparator((IComparator**)&cmp);
    if (mIsDescending) {
//        return CCollections::_ReverseOrder(cmp, comp);
        return NOERROR;
    }
    else {
        *comp = cmp;
        REFCOUNT_ADD(*comp);
        return NOERROR;
    }
}

AutoPtr<CConcurrentSkipListMap::_SubMap> CConcurrentSkipListMap::_SubMap::NewSubMap(
    /* [in] */ PInterface fromKey,
    /* [in] */ Boolean fromInclusive,
    /* [in] */ PInterface toKey,
    /* [in] */ Boolean toInclusive)
{
    if (mIsDescending) { // flip senses
        AutoPtr<IInterface> tk = fromKey;
        fromKey = toKey;
        toKey = tk;
        Boolean ti = fromInclusive;
        fromInclusive = toInclusive;
        toInclusive = ti;
    }
    if (mLo != NULL) {
        if (fromKey == NULL) {
            fromKey = mLo;
            fromInclusive = mLoInclusive;
        }
        else {
            Int32 c = mM->Compare(fromKey, mLo);
            if (c < 0 || (c == 0 && !mLoInclusive && fromInclusive))
                return NULL;
        }
    }
    if (mHi != NULL) {
        if (toKey == NULL) {
            toKey = mHi;
            toInclusive = mHiInclusive;
        }
        else {
            Int32 c = mM->Compare(toKey, mHi);
            if (c > 0 || (c == 0 && !mHiInclusive && toInclusive))
                return NULL;
        }
    }
    AutoPtr<_SubMap> res = new _SubMap(mM, fromKey, fromInclusive,
                                    toKey, toInclusive, mIsDescending);
    return res;
}

ECode CConcurrentSkipListMap::_SubMap::GetSubMap(
    /* [in] */ PInterface fromKey,
    /* [in] */ Boolean fromInclusive,
    /* [in] */ PInterface toKey,
    /* [in] */ Boolean toInclusive,
    /* [out] */ INavigableMap** res)
{
    VALIDATE_NOT_NULL(res);
    if (fromKey == NULL || toKey == NULL)
        return E_NULL_POINTER_EXCEPTION;
    AutoPtr<_SubMap> p = NewSubMap(fromKey, fromInclusive, toKey, toInclusive);
    *res = (INavigableMap*)p->Probe(EIID_INavigableMap);
    REFCOUNT_ADD(*res);
    return NOERROR;
}

ECode CConcurrentSkipListMap::_SubMap::GetHeadMap(
    /* [in] */ PInterface toKey,
    /* [in] */ Boolean inclusive,
    /* [out] */ INavigableMap** res)
{
    VALIDATE_NOT_NULL(res);
    if (toKey == NULL)
        return E_NULL_POINTER_EXCEPTION;
    AutoPtr<_SubMap> p = NewSubMap(NULL, FALSE, toKey, inclusive);
    *res = (INavigableMap*)p->Probe(EIID_INavigableMap);
    REFCOUNT_ADD(*res);
    return NOERROR;
}

ECode CConcurrentSkipListMap::_SubMap::GetTailMap(
    /* [in] */ PInterface fromKey,
    /* [in] */ Boolean inclusive,
    /* [out] */ INavigableMap** res)
{
    VALIDATE_NOT_NULL(res);
    if (fromKey == NULL)
        return E_NULL_POINTER_EXCEPTION;
    AutoPtr<_SubMap> p = NewSubMap(fromKey, inclusive, NULL, FALSE);
    *res = (INavigableMap*)p->Probe(EIID_INavigableMap);
    REFCOUNT_ADD(*res);
    return NOERROR;
}

ECode CConcurrentSkipListMap::_SubMap::GetSubMap(
    /* [in] */ PInterface fromKey,
    /* [in] */ PInterface toKey,
    /* [out] */ ISortedMap** res)
{
    VALIDATE_NOT_NULL(res);
    AutoPtr<INavigableMap> p;
    ECode ec = GetSubMap(fromKey, TRUE, toKey, FALSE, (INavigableMap**)&p);
    if (p != NULL) {
        *res = (ISortedMap*)p->Probe(EIID_ISortedMap);
        REFCOUNT_ADD(*res);
    }
    return ec;
}

ECode CConcurrentSkipListMap::_SubMap::GetHeadMap(
    /* [in] */ PInterface toKey,
    /* [out] */ ISortedMap** res)
{
    VALIDATE_NOT_NULL(res);
    AutoPtr<INavigableMap> p;
    ECode ec = GetHeadMap(toKey, FALSE, (INavigableMap**)&p);
    if (p != NULL) {
        *res = (ISortedMap*)p->Probe(EIID_ISortedMap);
        REFCOUNT_ADD(*res);
    }
    return ec;
}

ECode CConcurrentSkipListMap::_SubMap::GetTailMap(
    /* [in] */ PInterface fromKey,
    /* [out] */ ISortedMap** res)
{
    VALIDATE_NOT_NULL(res);
    AutoPtr<INavigableMap> p;
    ECode ec = GetTailMap(fromKey, TRUE, (INavigableMap**)&p);
    if (p != NULL) {
        *res = (ISortedMap*)p->Probe(EIID_ISortedMap);
        REFCOUNT_ADD(*res);
    }
    return ec;
}

ECode CConcurrentSkipListMap::_SubMap::GetDescendingMap(
    /* [out] */ INavigableMap** res)
{
    VALIDATE_NOT_NULL(res);
    AutoPtr<_SubMap> p = new _SubMap(mM, mLo, mLoInclusive,
                        mHi, mHiInclusive, !mIsDescending);
    *res = (INavigableMap*)p->Probe(EIID_INavigableMap);
    REFCOUNT_ADD(*res);
    return NOERROR;
}

/* ----------------  Relational methods -------------- */

ECode CConcurrentSkipListMap::_SubMap::GetCeilingEntry(
    /* [in] */ IInterface* key,
    /* [out] */ IMapEntry** outent)
{
    VALIDATE_NOT_NULL(outent);
    AutoPtr<IMapEntry> p = GetNearEntry(key, sGT|sEQ);
    *outent = p.Get();
    REFCOUNT_ADD(*outent);
    return NOERROR;
}

ECode CConcurrentSkipListMap::_SubMap::GetCeilingKey(
    /* [in] */ IInterface* key,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);
    AutoPtr<IInterface> p = GetNearKey(key, sGT|sEQ);
    *outface = p.Get();
    REFCOUNT_ADD(*outface);
    return NOERROR;
}

ECode CConcurrentSkipListMap::_SubMap::GetLowerEntry(
    /* [in] */ IInterface* key,
    /* [out] */ IMapEntry** outent)
{
    VALIDATE_NOT_NULL(outent);
    AutoPtr<IMapEntry> p = GetNearEntry(key, sLT);
    *outent = p.Get();
    REFCOUNT_ADD(*outent);
    return NOERROR;
}

ECode CConcurrentSkipListMap::_SubMap::GetLowerKey(
    /* [in] */ IInterface* key,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);
    AutoPtr<IInterface> p = GetNearKey(key, sLT);
    *outface = p.Get();
    REFCOUNT_ADD(*outface);
    return NOERROR;
}

ECode CConcurrentSkipListMap::_SubMap::GetFloorEntry(
    /* [in] */ IInterface* key,
    /* [out] */ IMapEntry** outent)
{
    VALIDATE_NOT_NULL(outent);
    AutoPtr<IMapEntry> p = GetNearEntry(key, sLT|sEQ);
    *outent = p.Get();
    REFCOUNT_ADD(*outent);
    return NOERROR;
}

ECode CConcurrentSkipListMap::_SubMap::GetFloorKey(
    /* [in] */ IInterface* key,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);
    AutoPtr<IInterface> p = GetNearKey(key, sLT|sEQ);
    *outface = p.Get();
    REFCOUNT_ADD(*outface);
    return NOERROR;
}

ECode CConcurrentSkipListMap::_SubMap::GetHigherEntry(
    /* [in] */ IInterface* key,
    /* [out] */ IMapEntry** outent)
{
    VALIDATE_NOT_NULL(outent);
    AutoPtr<IMapEntry> p = GetNearEntry(key, sGT);
    *outent = p.Get();
    REFCOUNT_ADD(*outent);
    return NOERROR;
}

ECode CConcurrentSkipListMap::_SubMap::GetHigherKey(
    /* [in] */ IInterface* key,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);
    AutoPtr<IInterface> p = GetNearKey(key, sGT);
    *outface = p.Get();
    REFCOUNT_ADD(*outface);
    return NOERROR;
}

ECode CConcurrentSkipListMap::_SubMap::GetFirstKey(
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);
    AutoPtr<IInterface> p = mIsDescending ? HighestKey() : LowestKey();
    *outface = p.Get();
    REFCOUNT_ADD(*outface);
    return NOERROR;
}

ECode CConcurrentSkipListMap::_SubMap::GetLastKey(
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);
    AutoPtr<IInterface> p = mIsDescending ? LowestKey() : HighestKey();
    *outface = p.Get();
    REFCOUNT_ADD(*outface);
    return NOERROR;
}

ECode CConcurrentSkipListMap::_SubMap::GetFirstEntry(
    /* [out] */ IMapEntry** outent)
{
    VALIDATE_NOT_NULL(outent);
    AutoPtr<IMapEntry> p = mIsDescending ? HighestEntry() : LowestEntry();
    *outent = p.Get();
    REFCOUNT_ADD(*outent);
    return NOERROR;
}

ECode CConcurrentSkipListMap::_SubMap::GetLastEntry(
    /* [out] */ IMapEntry** outent)
{
    VALIDATE_NOT_NULL(outent);
    AutoPtr<IMapEntry> p = mIsDescending ? LowestEntry() : HighestEntry();
    *outent = p.Get();
    REFCOUNT_ADD(*outent);
    return NOERROR;
}

ECode CConcurrentSkipListMap::_SubMap::PollFirstEntry(
    /* [out] */ IMapEntry** outent)
{
    VALIDATE_NOT_NULL(outent);
    AutoPtr<IMapEntry> p = mIsDescending ? RemoveHighest() : RemoveLowest();
    *outent = p.Get();
    REFCOUNT_ADD(*outent);
    return NOERROR;
}

ECode CConcurrentSkipListMap::_SubMap::PollLastEntry(
    /* [out] */ IMapEntry** outent)
{
    VALIDATE_NOT_NULL(outent);
    AutoPtr<IMapEntry> p = mIsDescending ? RemoveLowest() : RemoveHighest();
    *outent = p.Get();
    REFCOUNT_ADD(*outent);
    return NOERROR;
}

/* ---------------- _SubMap Views -------------- */

ECode CConcurrentSkipListMap::_SubMap::GetKeySet(
    /* [out] */ ISet** res)
{
    VALIDATE_NOT_NULL(res);
    if (mKeySetView == NULL) {
        mKeySetView = new _KeySet(this);
    }
    *res = mKeySetView;
    REFCOUNT_ADD(*res);
    return NOERROR;
}

ECode CConcurrentSkipListMap::_SubMap::GetNavigableKeySet(
    /* [out] */ INavigableSet** res)
{
    VALIDATE_NOT_NULL(res);
    if (mKeySetView == NULL) {
        mKeySetView = new _KeySet(this);
    }
    *res = mKeySetView;
    REFCOUNT_ADD(*res);
    return NOERROR;
}

ECode CConcurrentSkipListMap::_SubMap::GetValues(
    /* [out] */ ICollection** value)
{
    VALIDATE_NOT_NULL(value);
    if (mValuesView == NULL) {
        mValuesView = new _Values(this);
    }
    *value = mValuesView;
    REFCOUNT_ADD(*value);
    return NOERROR;
}

ECode CConcurrentSkipListMap::_SubMap::GetEntrySet(
    /* [out] */ ISet** entries)
{
    VALIDATE_NOT_NULL(entries);
    if (mEntrySetView == NULL) {
        mEntrySetView = new _EntrySet(this);
    }
    *entries = mEntrySetView;
    REFCOUNT_ADD(*entries);
    return NOERROR;
}

ECode CConcurrentSkipListMap::_SubMap::GetDescendingKeySet(
    /* [out] */ INavigableSet** res)
{
    VALIDATE_NOT_NULL(res);
    AutoPtr<INavigableMap> m;
    GetDescendingMap((INavigableMap**)&m);
    return m->GetNavigableKeySet(res);
}

AutoPtr<IIterator> CConcurrentSkipListMap::_SubMap::KeyIterator()
{
    AutoPtr<SubMapKeyIterator> p = new SubMapKeyIterator(this);
    return (IIterator*)p->Probe(EIID_IIterator);
}

AutoPtr<IIterator> CConcurrentSkipListMap::_SubMap::ValueIterator()
{
    AutoPtr<SubMapValueIterator> p = new SubMapValueIterator(this);
    return (IIterator*)p->Probe(EIID_IIterator);
}

AutoPtr<IIterator> CConcurrentSkipListMap::_SubMap::EntryIterator()
{
    AutoPtr<SubMapEntryIterator> p = new SubMapEntryIterator(this);
    return (IIterator*)p->Probe(EIID_IIterator);
}

ECode CConcurrentSkipListMap::_SubMap::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    return AbstractMap::Equals(obj, result);
}

ECode CConcurrentSkipListMap::_SubMap::GetHashCode(
    /* [out] */ Int32* hash)
{
    return AbstractMap::GetHashCode(hash);
}

ECode CConcurrentSkipListMap::_SubMap::Put(
    /* [in] */ IInterface* key,
    /* [in] */ IInterface* value)
{
    return AbstractMap::Put(key, value);
}

ECode CConcurrentSkipListMap::_SubMap::PutAll(
    /* [in] */ IMap* map)
{
    return AbstractMap::PutAll(map);
}

ECode CConcurrentSkipListMap::_SubMap::Remove(
    /* [in] */ IInterface* obj)
{
    return AbstractMap::Remove(obj);
}

//====================================================================
// CConcurrentSkipListMap::_SubMap::SubMapIter::
//====================================================================
CAR_INTERFACE_IMPL(CConcurrentSkipListMap::_SubMap::SubMapIter, Object, IIterator);

CConcurrentSkipListMap::_SubMap::SubMapIter::SubMapIter(
    /* [in] */ _SubMap* owner)
{
    mOwner = owner;
    for (;;) {
        mNext = mOwner->mIsDescending ? mOwner->HiNode() : mOwner->LoNode();
        if (mNext == NULL)
            break;
        AutoPtr<IInterface> x = mNext->mValue;
        if (x != NULL && !Object::Equals(x, mNext->Probe(EIID_IInterface))) {
            if (!mOwner->InBounds(mNext->mKey))
                mNext = NULL;
            else
                mNextValue = x;
            break;
        }
    }
}

ECode CConcurrentSkipListMap::_SubMap::SubMapIter::HasNext(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mNext != NULL;
    return NOERROR;
}

void CConcurrentSkipListMap::_SubMap::SubMapIter::Advance()
{
    if (mNext == NULL)
        return;
//        throw new NoSuchElementException();
    mLastReturned = mNext;
    if (mOwner->mIsDescending)
        Descend();
    else
        Ascend();
}

void CConcurrentSkipListMap::_SubMap::SubMapIter::Ascend()
{
    for (;;) {
        mNext = mNext->mNext;
        if (mNext == NULL)
            break;
        AutoPtr<IInterface> x = mNext->mValue;
        if (x != NULL && !Object::Equals(x, mNext->Probe(EIID_IInterface))) {
            if (mOwner->TooHigh(mNext->mKey))
                mNext = NULL;
            else
                mNextValue = x;
            break;
        }
    }
}

void CConcurrentSkipListMap::_SubMap::SubMapIter::Descend()
{
    for (;;) {
        mNext = mOwner->mM->FindNear(mLastReturned->mKey, sLT);
        if (mNext == NULL)
            break;
        AutoPtr<IInterface> x = mNext->mValue;
        if (x != NULL && !Object::Equals(x, mNext->Probe(EIID_IInterface))) {
            if (mOwner->TooLow(mNext->mKey))
                mNext = NULL;
            else
                mNextValue = x;
            break;
        }
    }
}

ECode CConcurrentSkipListMap::_SubMap::SubMapIter::Remove()
{
    AutoPtr<Node> l = mLastReturned;
    if (l == NULL)
        return E_ILLEGAL_STATE_EXCEPTION;
    AutoPtr<IInterface> val;
    mOwner->mM->Remove(l->mKey, (IInterface**)&val);
    mLastReturned = NULL;
    return NOERROR;
}

//====================================================================
// CConcurrentSkipListMap::_SubMap::SubMapValueIterator::
//====================================================================
CConcurrentSkipListMap::_SubMap::SubMapValueIterator::SubMapValueIterator(
    /* [in] */ _SubMap* owner) : SubMapIter(owner)
{
}

ECode CConcurrentSkipListMap::_SubMap::SubMapValueIterator::GetNext(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);
    AutoPtr<IInterface> v = mNextValue;
    Advance();
    *object = v;
    REFCOUNT_ADD(*object);
    return NOERROR;
}

//====================================================================
// CConcurrentSkipListMap::_SubMap::SubMapKeyIterator::
//====================================================================
CConcurrentSkipListMap::_SubMap::SubMapKeyIterator::SubMapKeyIterator(
    /* [in] */ _SubMap* owner) : SubMapIter(owner)
{
}

ECode CConcurrentSkipListMap::_SubMap::SubMapKeyIterator::GetNext(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);
    AutoPtr<Node> n = mNext;
    Advance();
    *object = n->mKey;
    REFCOUNT_ADD(*object);
    return NOERROR;
}

//====================================================================
// CConcurrentSkipListMap::_SubMap::SubMapEntryIterator::
//====================================================================
CConcurrentSkipListMap::_SubMap::SubMapEntryIterator::SubMapEntryIterator(
    /* [in] */ _SubMap* owner) : SubMapIter(owner)
{
}

ECode CConcurrentSkipListMap::_SubMap::SubMapEntryIterator::GetNext(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);
    AutoPtr<Node> n = mNext;
    AutoPtr<IInterface> v = mNextValue;
    Advance();
    AutoPtr<SimpleImmutableEntry> p = new SimpleImmutableEntry(n->mKey, v);
    *object = p->Probe(EIID_IInterface);
    REFCOUNT_ADD(*object);
    return NOERROR;
}

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

} // namespace Concurrent
} // namespace Utility
} // namespace Elastos
