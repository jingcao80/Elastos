
#include "Elastos.CoreLibrary.IO.h"
#include "CConcurrentLinkedQueue.h"
#include "CArrayList.h"
#include <Math.h>

using Elastos::IO::EIID_ISerializable;
using Elastos::Core::Math;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;

namespace Elastos {
namespace Utility {
namespace Concurrent {

//====================================================================
// CConcurrentLinkedQueue::Node::
//====================================================================
CConcurrentLinkedQueue::Node::Node(
    /* [in] */ IInterface* item)
{
//    UNSAFE.putObject(this, itemOffset, item);
}

Boolean CConcurrentLinkedQueue::Node::CasItem(
    /* [in] */ IInterface* cmp,
    /* [in] */ IInterface* val)
{
//    return UNSAFE.compareAndSwapObject(this, itemOffset, cmp, val);
    return FALSE;
}

void CConcurrentLinkedQueue::Node::LazySetNext(
    /* [in] */ Node* val)
{
//    UNSAFE.putOrderedObject(this, nextOffset, val);
}

Boolean CConcurrentLinkedQueue::Node::CasNext(
    /* [in] */ Node* cmp,
    /* [in] */ Node* val)
{
//    return UNSAFE.compareAndSwapObject(this, nextOffset, cmp, val);
    return FALSE;
}

//    sun.misc.Unsafe UNSAFE;
    Int64 CConcurrentLinkedQueue::Node::sItemOffset;
    Int64 CConcurrentLinkedQueue::Node::sNextOffset;

    // static {
    //     try {
    //         UNSAFE = sun.misc.Unsafe.getUnsafe();
    //         Class<?> k = Node.class;
    //         itemOffset = UNSAFE.objectFieldOffset
    //             (k.getDeclaredField("item"));
    //         nextOffset = UNSAFE.objectFieldOffset
    //             (k.getDeclaredField("next"));
    //     } catch (Exception e) {
    //         throw new Error(e);
    //     }
    // }
//====================================================================
// CConcurrentLinkedQueue::
//====================================================================
CAR_INTERFACE_IMPL_2(CConcurrentLinkedQueue, AbstractQueue, IConcurrentLinkedQueue, ISerializable)

CAR_OBJECT_IMPL(CConcurrentLinkedQueue);

ECode CConcurrentLinkedQueue::constructor()
{
    mHead = mTail = new Node(NULL);
    return NOERROR;
}

ECode CConcurrentLinkedQueue::constructor(
    /* [in] */ ICollection* c)
{
    AutoPtr<Node> h, t;
    AutoPtr<IIterator> it;
    (IIterable::Probe(c))->GetIterator((IIterator**)&it);
    Boolean isflag = FALSE;
    while ((it->HasNext(&isflag), isflag)) {
        AutoPtr<IInterface> e;
        it->GetNext((IInterface**)&e);
        CheckNotNull(e);
        AutoPtr<Node> newNode = new Node(e);
        if (h == NULL)
            h = t = newNode;
        else {
            t->LazySetNext(newNode);
            t = newNode;
        }
    }
    if (h == NULL)
        h = t = new Node(NULL);
    mHead = h;
    mTail = t;
    return NOERROR;
}

ECode CConcurrentLinkedQueue::Add(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    return Offer(object, modified);
}

void CConcurrentLinkedQueue::UpdateHead(
    /* [in] */ Node* h,
    /* [in] */ Node* p)
{
    if (h != p && CasHead(h, p))
        h->LazySetNext(h);
}

AutoPtr<CConcurrentLinkedQueue::Node> CConcurrentLinkedQueue::Succ(
    /* [in] */ Node* p)
{
    AutoPtr<Node> next = p->mNext;
    return (Object::Equals(p->Probe(EIID_IInterface), next->Probe(EIID_IInterface))) ? mHead : next;
}

ECode CConcurrentLinkedQueue::Offer(
    /* [in] */ IInterface* e,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    CheckNotNull(e);
    AutoPtr<Node> newNode = new Node(e);

    for (AutoPtr<Node> t = mTail, p = t;;) {
        AutoPtr<Node> q = p->mNext;
        if (q == NULL) {
            // p is last node
            if (p->CasNext(NULL, newNode)) {
                // Successful CAS is the linearization point
                // for e to become an element of this queue,
                // and for newNode to become "live".
                if (p != t) // hop two nodes at a time
                    CasTail(t, newNode);  // Failure is OK.
                *result = TRUE;
                return NOERROR;
            }
            // Lost CAS race to another thread; re-read next
        }
        else if (p == q)
            // We have fallen off list.  If tail is unchanged, it
            // will also be off-list, in which case we need to
            // jump to head, from which all live nodes are always
            // reachable.  Else the new tail is a better bet.
            p = (t != (t = mTail)) ? t : mHead;
        else
            // Check for tail updates after two hops.
            p = (p != t && t != (t = mTail)) ? t : q;
    }
}

ECode CConcurrentLinkedQueue::Poll(
    /* [out] */ IInterface** e)
{
    VALIDATE_NOT_NULL(*e);
    RESTARTFROMHEAD:
    for (;;) {
        for (AutoPtr<Node> h = mHead, p = h, q;;) {
            AutoPtr<IInterface> item = p->mItem;

            if (item != NULL && p->CasItem(item, NULL)) {
                // Successful CAS is the linearization point
                // for item to be removed from this queue.
                if (p != h) // hop two nodes at a time
                    UpdateHead(h, ((q = p->mNext) != NULL) ? q : p);
                *e = item;
                REFCOUNT_ADD(*e);
                return NOERROR;
            }
            else if ((q = p->mNext) == NULL) {
                UpdateHead(h, p);
                *e = NULL;
                return NOERROR;
            }
            else if (p == q)
                goto RESTARTFROMHEAD;
            else
                p = q;
        }
    }
    return NOERROR;
}

ECode CConcurrentLinkedQueue::Peek(
    /* [out] */ IInterface** e)
{
    VALIDATE_NOT_NULL(*e);
    RESTARTFROMHEAD:
    for (;;) {
        for (AutoPtr<Node> h = mHead, p = h, q;;) {
            AutoPtr<IInterface> item = p->mItem;
            if (item != NULL || (q = p->mNext) == NULL) {
                UpdateHead(h, p);
                *e = item;
                REFCOUNT_ADD(*e);
                return NOERROR;
            }
            else if (p == q)
                goto RESTARTFROMHEAD;
            else
                p = q;
        }
    }
    return NOERROR;
}

AutoPtr<CConcurrentLinkedQueue::Node> CConcurrentLinkedQueue::First()
{
    RESTARTFROMHEAD:
    for (;;) {
        for (AutoPtr<Node> h = mHead, p = h, q;;) {
            Boolean hasItem = (p->mItem != NULL);
            if (hasItem || (q = p->mNext) == NULL) {
                UpdateHead(h, p);
                return hasItem ? p : NULL;
            }
            else if (p == q)
                goto RESTARTFROMHEAD;
            else
                p = q;
        }
    }
    return NULL;
}

ECode CConcurrentLinkedQueue::IsEmpty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = First() == NULL;
    return NOERROR;
}

ECode CConcurrentLinkedQueue::GetSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size);
    Int32 count = 0;
    for (AutoPtr<Node> p = First(); p != NULL; p = Succ(p))
        if (p->mItem != NULL)
            // Collection.size() spec says to max out
            if (++count == Elastos::Core::Math::INT32_MAX_VALUE)
                break;
    *size = count;
    return NOERROR;
}

ECode CConcurrentLinkedQueue::Contains(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (object == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    for (AutoPtr<Node> p = First(); p != NULL; p = Succ(p)) {
        AutoPtr<IInterface> item = p->mItem;
        if (item != NULL && Object::Equals(object, item)) {
            *result = TRUE;
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode CConcurrentLinkedQueue::Remove(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    VALIDATE_NOT_NULL(modified);
    if (object == NULL) {
        *modified = FALSE;
        return NOERROR;
    }
    AutoPtr<Node> pred;
    for (AutoPtr<Node> p = First(); p != NULL; p = Succ(p)) {
        AutoPtr<IInterface> item = p->mItem;
        if (item != NULL &&
            Object::Equals(object, item) &&
            p->CasItem(item, NULL)) {
            AutoPtr<Node> next = Succ(p);
            if (pred != NULL && next != NULL)
                pred->CasNext(p, next);
            *modified = TRUE;
            return NOERROR;
        }
        pred = p;
    }
    *modified = FALSE;
    return NOERROR;
}

ECode CConcurrentLinkedQueue::AddAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    VALIDATE_NOT_NULL(modified);
    if (collection->Probe(EIID_IInterface) == THIS_PROBE(IInterface))
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // As historically specified in AbstractQueue#addAll

    // Copy c into a private chain of Nodes
    AutoPtr<Node> beginningOfTheEnd, last;
    AutoPtr<IIterator> it;
    (IIterable::Probe(collection))->GetIterator((IIterator**)&it);
    Boolean isflag = FALSE;
    while ((it->HasNext(&isflag), isflag)) {
        AutoPtr<IInterface> e;
        it->GetNext((IInterface**)&e);
        CheckNotNull(e);
        AutoPtr<Node> newNode = new Node(e);
        if (beginningOfTheEnd == NULL)
            beginningOfTheEnd = last = newNode;
        else {
            last->LazySetNext(newNode);
            last = newNode;
        }
    }
    if (beginningOfTheEnd == NULL) {
        *modified = FALSE;
        return NOERROR;
    }

    // Atomically append the chain at the tail of this collection
    for (AutoPtr<Node> t = mTail, p = t;;) {
        AutoPtr<Node> q = p->mNext;
        if (q == NULL) {
            // p is last node
            if (p->CasNext(NULL, beginningOfTheEnd)) {
                // Successful CAS is the linearization point
                // for all elements to be added to this queue.
                if (!CasTail(t, last)) {
                    // Try a little harder to update tail,
                    // since we may be adding many elements.
                    t = mTail;
                    if (last->mNext == NULL)
                        CasTail(t, last);
                }
                *modified = TRUE;
                return NOERROR;
            }
            // Lost CAS race to another thread; re-read next
        }
        else if (p == q)
            // We have fallen off list.  If tail is unchanged, it
            // will also be off-list, in which case we need to
            // jump to head, from which all live nodes are always
            // reachable.  Else the new tail is a better bet.
            p = (t != (t = mTail)) ? t : mHead;
        else
            // Check for tail updates after two hops.
            p = (p != t && t != (t = mTail)) ? t : q;
    }
    return NOERROR;
}

ECode CConcurrentLinkedQueue::ToArray(
    /* [out, callee] */ ArrayOf<IInterface*>** array)
{
    VALIDATE_NOT_NULL(*array);
    // Use ArrayList to deal with resizing.
    AutoPtr<IArrayList> al;
    CArrayList::New((IArrayList**)&al);
    for (AutoPtr<Node> p = First(); p != NULL; p = Succ(p)) {
        AutoPtr<IInterface> item = p->mItem;
        if (item != NULL) {
            Boolean b = FALSE;
            (ICollection::Probe(al))->Add(item, &b);
        }
    }
    return (ICollection::Probe(al))->ToArray(array);
}

ECode CConcurrentLinkedQueue::ToArray(
    /* [in] */ ArrayOf<IInterface*>* inArray,
    /* [out, callee] */ ArrayOf<IInterface*>** outArray)
{
    VALIDATE_NOT_NULL(*outArray);
    // try to use sent-in array
    Int32 k = 0;
    AutoPtr<Node> p;
    for (p = First(); p != NULL && k < inArray->GetLength(); p = Succ(p)) {
        AutoPtr<IInterface> item = p->mItem;
        if (item != NULL) {
            inArray->Set(k, item);
            k++;
        }
    }
    if (p == NULL) {
        if (k < inArray->GetLength())
            inArray->Set(k, NULL);
        *outArray = inArray;
        REFCOUNT_ADD(*outArray);
        return NOERROR;
    }

    // If won't fit, use ArrayList version
    AutoPtr<IArrayList> al;
    CArrayList::New((IArrayList**)&al);
    for (AutoPtr<Node> q = First(); q != NULL; q = Succ(q)) {
        AutoPtr<IInterface> item = q->mItem;
        if (item != NULL) {
            Boolean b = FALSE;
            (ICollection::Probe(al))->Add(item, &b);
        }
    }
    return (ICollection::Probe(al))->ToArray(inArray, outArray);
}

ECode CConcurrentLinkedQueue::GetIterator(
    /* [out] */ IIterator** iterator)
{
    VALIDATE_NOT_NULL(*iterator);
    AutoPtr<Itr> p = new Itr(this);
    *iterator = (IIterator*)p->Probe(EIID_IIterator);
    REFCOUNT_ADD(*iterator);
    return NOERROR;
}

void CConcurrentLinkedQueue::WriteObject(
    /* [in] */ IObjectOutputStream* s)
{
    // Write out any hidden stuff
    s->DefaultWriteObject();

    // Write out all elements in the proper order.
    for (AutoPtr<Node> p = First(); p != NULL; p = Succ(p)) {
        AutoPtr<IInterface> item = p->mItem;
        if (item != NULL)
            /*s->WriteObject(item)*/;
    }

    // Use trailing null as sentinel
//    s->WriteObject(NULL);
}

void CConcurrentLinkedQueue::ReadObject(
    /* [in] */ IObjectInputStream* s)
{
    s->DefaultReadObject();

    // Read in elements until trailing null sentinel found
    AutoPtr<Node> h, t;
    AutoPtr<IInterface> item;
    while ((item/* = s->ReadObject()*/) != NULL) {
        AutoPtr<Node> newNode = new Node(item);
        if (h == NULL)
            h = t = newNode;
        else {
            t->LazySetNext(newNode);
            t = newNode;
        }
    }
    if (h == NULL)
        h = t = new Node(NULL);
    mHead = h;
    mTail = t;
}

ECode CConcurrentLinkedQueue::CheckNotNull(
    /* [in] */ IInterface* v)
{
    if (v == NULL)
        return E_NULL_POINTER_EXCEPTION;
    return NOERROR;
}

Boolean CConcurrentLinkedQueue::CasTail(
    /* [in] */ Node* cmp,
    /* [in] */ Node* val)
{
//    return UNSAFE.compareAndSwapObject(this, tailOffset, cmp, val);
    return FALSE;
}

Boolean CConcurrentLinkedQueue::CasHead(
    /* [in] */ Node* cmp,
    /* [in] */ Node* val)
{
//   return UNSAFE.compareAndSwapObject(this, headOffset, cmp, val);
    return FALSE;
}

ECode CConcurrentLinkedQueue::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    return E_NO_SUCH_METHOD_EXCEPTION;
}

ECode CConcurrentLinkedQueue::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    return E_NO_SUCH_METHOD_EXCEPTION;
}

//private static final sun.misc.Unsafe UNSAFE;
Int64 CConcurrentLinkedQueue::sHeadOffset;
Int64 CConcurrentLinkedQueue::sTailOffset;
// static {
//     try {
//         UNSAFE = sun.misc.Unsafe.getUnsafe();
//         Class<?> k = ConcurrentLinkedQueue.class;
//         headOffset = UNSAFE.objectFieldOffset
//             (k.getDeclaredField("head"));
//         tailOffset = UNSAFE.objectFieldOffset
//             (k.getDeclaredField("tail"));
//     } catch (Exception e) {
//         throw new Error(e);
//     }
// }

//====================================================================
// CConcurrentLinkedQueue::Itr::
//====================================================================
CAR_INTERFACE_IMPL(CConcurrentLinkedQueue::Itr, Object, IIterator)

CConcurrentLinkedQueue::Itr::Itr(
    /* [in] */ CConcurrentLinkedQueue* owner)
{
    Advance();
    mOwner = owner;
}

AutoPtr<IInterface> CConcurrentLinkedQueue::Itr::Advance()
{
    mLastRet = mNextNode;
    AutoPtr<IInterface> x = mNextItem;

    AutoPtr<Node> pred, p;
    if (mNextNode == NULL) {
        p = mOwner->First();
        pred = NULL;
    }
    else {
        pred = mNextNode;
        p = mOwner->Succ(mNextNode);
    }

    for (;;) {
        if (p == NULL) {
            mNextNode = NULL;
            mNextItem = NULL;
            return x;
        }
        AutoPtr<IInterface> item = p->mItem;
        if (item != NULL) {
            mNextNode = p;
            mNextItem = item;
            return x;
        }
        else {
            // skip over nulls
            AutoPtr<Node> next = mOwner->Succ(p);
            if (pred != NULL && next != NULL)
                pred->CasNext(p, next);
            p = next;
        }
    }
}

ECode CConcurrentLinkedQueue::Itr::HasNext(
    /* [out] */ Boolean* result)
{
    *result = mNextNode != NULL;
    return NOERROR;
}

ECode CConcurrentLinkedQueue::Itr::GetNext(
    /* [out] */ IInterface** object)
{
    if (mNextNode == NULL)
        return E_NO_SUCH_ELEMENT_EXCEPTION;
    AutoPtr<IInterface> p = Advance();
    *object = p.Get();
    REFCOUNT_ADD(*object);
    return NOERROR;
}

ECode CConcurrentLinkedQueue::Itr::Remove()
{
    AutoPtr<Node> l = mLastRet;
    if (l == NULL)
        return E_ILLEGAL_STATE_EXCEPTION;
    // rely on a future traversal to relink.
    l->mItem = NULL;
    mLastRet = NULL;
    return NOERROR;
}

} // namespace Concurrent
} // namespace Utility
} // namespace Elastos
