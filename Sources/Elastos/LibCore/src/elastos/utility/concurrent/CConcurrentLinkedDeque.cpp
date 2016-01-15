
#include "Elastos.CoreLibrary.IO.h"
#include "CConcurrentLinkedDeque.h"
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
// CConcurrentLinkedDeque::Node::
//====================================================================
CConcurrentLinkedDeque::Node::Node()
{  // default constructor for NEXT_TERMINATOR, PREV_TERMINATOR
}

CConcurrentLinkedDeque::Node::Node(
    /* [in] */ IInterface* item)
{
    assert(0 && "TODO");
//    UNSAFE.putObject(this, itemOffset, item);
}

Boolean CConcurrentLinkedDeque::Node::CasItem(
    /* [in] */ IInterface* cmp,
    /* [in] */ IInterface* val)
{
    assert(0 && "TODO");
//    return UNSAFE.compareAndSwapObject(this, itemOffset, cmp, val);
    return FALSE;
}

void CConcurrentLinkedDeque::Node::LazySetNext(
    /* [in] */ Node* val)
{
    assert(0 && "TODO");
//    UNSAFE.putOrderedObject(this, nextOffset, val);
}

Boolean CConcurrentLinkedDeque::Node::CasNext(
    /* [in] */ Node* cmp,
    /* [in] */ Node* val)
{
    assert(0 && "TODO");
//    return UNSAFE.compareAndSwapObject(this, nextOffset, cmp, val);
    return FALSE;
}

void CConcurrentLinkedDeque::Node::LazySetPrev(
    /* [in] */ Node* val)
{
    assert(0 && "TODO");
//    UNSAFE.putOrderedObject(this, prevOffset, val);
}

Boolean CConcurrentLinkedDeque::Node::CasPrev(
    /* [in] */ Node* cmp,
    /* [in] */ Node* val)
{
    assert(0 && "TODO");
//    return UNSAFE.compareAndSwapObject(this, prevOffset, cmp, val);
    return FALSE;
}

// Unsafe mechanics
Int64 CConcurrentLinkedDeque::Node::sPrevOffset = 0;
Int64 CConcurrentLinkedDeque::Node::sItemOffset = 0;
Int64 CConcurrentLinkedDeque::Node::sNextOffset = 0;

// static {
//     try {
//         UNSAFE = sun.misc.Unsafe.getUnsafe();
//         Class<?> k = Node.class;
//         prevOffset = UNSAFE.objectFieldOffset
//             (k.getDeclaredField("prev"));
//         itemOffset = UNSAFE.objectFieldOffset
//             (k.getDeclaredField("item"));
//         nextOffset = UNSAFE.objectFieldOffset
//             (k.getDeclaredField("next"));
//     } catch (Exception e) {
//         throw new Error(e);
//     }
// }

//====================================================================
// CConcurrentLinkedDeque::
//====================================================================
//static Int64 serialVersionUID = 876323262645176354L;

AutoPtr<CConcurrentLinkedDeque::Node> CConcurrentLinkedDeque::sPREV_TERMINATOR;
AutoPtr<CConcurrentLinkedDeque::Node> CConcurrentLinkedDeque::sNEXT_TERMINATOR;

CAR_INTERFACE_IMPL_3(CConcurrentLinkedDeque, AbstractCollection, IDeque, IQueue, ISerializable)

CAR_OBJECT_IMPL(CConcurrentLinkedDeque);

AutoPtr<CConcurrentLinkedDeque::Node> CConcurrentLinkedDeque::PrevTerminator()
{
    return sPREV_TERMINATOR;
}

AutoPtr<CConcurrentLinkedDeque::Node> CConcurrentLinkedDeque::NextTerminator()
{
    return sNEXT_TERMINATOR;
}

void CConcurrentLinkedDeque::LinkFirst(
    /* [in] */ IInterface* e)
{
    CheckNotNull(e);
    AutoPtr<Node> newNode = new Node(e);

RESTARTFROMHEAD:
    for (;;) {
        for (AutoPtr<Node> h = mHead, p = h, q;;) {
            if ((q = p->mPrev) != NULL &&
                (q = (p = q)->mPrev) != NULL)
                // Check for head updates every other hop.
                // If p == q, we are sure to follow head instead.
                p = (h != (h = mHead)) ? h : q;
            else if (Object::Equals(p->mNext->Probe(EIID_IInterface), p->Probe(EIID_IInterface))) { // PREV_TERMINATOR
                goto RESTARTFROMHEAD;
            }
            else {
                // p is first node
                newNode->LazySetNext(p); // CAS piggyback
                if (p->CasPrev(NULL, newNode)) {
                    // Successful CAS is the linearization point
                    // for e to become an element of this deque,
                    // and for newNode to become "live".
                    if (p != h) // hop two nodes at a time
                        CasHead(h, newNode);  // Failure is OK.
                    return;
                }
                // Lost CAS race to another thread; re-read prev
            }
        }
    }
}

void CConcurrentLinkedDeque::LinkLast(
    /* [in] */ IInterface* e)
{
    CheckNotNull(e);
    AutoPtr<Node> newNode = new Node(e);

    RESTARTFROMTAIL:
    for (;;) {
        for (AutoPtr<Node> t = mTail, p = t, q;;) {
            if ((q = p->mNext) != NULL &&
                (q = (p = q)->mNext) != NULL)
                // Check for tail updates every other hop.
                // If p == q, we are sure to follow tail instead.
                p = (t != (t = mTail)) ? t : q;
            else if (Object::Equals(p->mPrev->Probe(EIID_IInterface), p->Probe(EIID_IInterface))) { // NEXT_TERMINATOR
                goto RESTARTFROMTAIL;
            }
            else {
                // p is last node
                newNode->LazySetPrev(p); // CAS piggyback
                if (p->CasNext(NULL, newNode)) {
                    // Successful CAS is the linearization point
                    // for e to become an element of this deque,
                    // and for newNode to become "live".
                    if (!Object::Equals(p->Probe(EIID_IInterface), t->Probe(EIID_IInterface))) // hop two nodes at a time
                        CasTail(t, newNode);  // Failure is OK.
                    return;
                }
                // Lost CAS race to another thread; re-read next
            }
        }
    }
}

Int32 CConcurrentLinkedDeque::sHOPS = 2;

void CConcurrentLinkedDeque::Unlink(
    /* [in] */ Node* x)
{
    // assert x != null;
    // assert x.item == null;
    // assert x != PREV_TERMINATOR;
    // assert x != NEXT_TERMINATOR;

    AutoPtr<Node> prev = x->mPrev;
    AutoPtr<Node> next = x->mNext;
    if (prev == NULL) {
        UnlinkFirst(x, next);
    }
    else if (next == NULL) {
        UnlinkLast(x, prev);
    }
    else {
        // Unlink interior node.
        //
        // This is the common case, since a series of polls at the
        // same end will be "interior" removes, except perhaps for
        // the first one, since end nodes cannot be unlinked.
        //
        // At any time, all active nodes are mutually reachable by
        // following a sequence of either next or prev pointers.
        //
        // Our strategy is to find the unique active predecessor
        // and successor of x.  Try to fix up their links so that
        // they point to each other, leaving x unreachable from
        // active nodes.  If successful, and if x has no live
        // predecessor/successor, we additionally try to gc-unlink,
        // leaving active nodes unreachable from x, by rechecking
        // that the status of predecessor and successor are
        // unchanged and ensuring that x is not reachable from
        // tail/head, before setting x's prev/next links to their
        // logical approximate replacements, self/TERMINATOR.
        AutoPtr<Node> activePred, activeSucc;
        Boolean isFirst, isLast;
        Int32 hops = 1;

        // Find active predecessor
        for (AutoPtr<Node> p = prev; ; ++hops) {
            if (p->mItem != NULL) {
                activePred = p;
                isFirst = FALSE;
                break;
            }
            AutoPtr<Node> q = p->mPrev;
            if (q == NULL) {
                if (Object::Equals(p->mNext->Probe(EIID_IInterface), p->Probe(EIID_IInterface)))
                    return;
                activePred = p;
                isFirst = TRUE;
                break;
            }
            else if (Object::Equals(p->Probe(EIID_IInterface), q->Probe(EIID_IInterface)))
                return;
            else
                p = q;
        }

        // Find active successor
        for (AutoPtr<Node> p = next; ; ++hops) {
            if (p->mItem != NULL) {
                activeSucc = p;
                isLast = FALSE;
                break;
            }
            AutoPtr<Node> q = p->mNext;
            if (q == NULL) {
                if (Object::Equals(p->mPrev->Probe(EIID_IInterface), p->Probe(EIID_IInterface)))
                    return;
                activeSucc = p;
                isLast = TRUE;
                break;
            }
            else if (Object::Equals(p->Probe(EIID_IInterface), q->Probe(EIID_IInterface)))
                return;
            else
                p = q;
        }

        // TODO: better HOP heuristics
        if (hops < sHOPS
            // always squeeze out interior deleted nodes
            && (isFirst | isLast))
            return;

        // Squeeze out deleted nodes between activePred and
        // activeSucc, including x.
        SkipDeletedSuccessors(activePred);
        SkipDeletedPredecessors(activeSucc);

        // Try to gc-unlink, if possible
        if ((isFirst | isLast) &&

            // Recheck expected state of predecessor and successor
            (Object::Equals(activePred->mNext->Probe(EIID_IInterface), activeSucc->Probe(EIID_IInterface))) &&
            (Object::Equals(activeSucc->mPrev->Probe(EIID_IInterface), activePred->Probe(EIID_IInterface))) &&
            (isFirst ? activePred->mPrev == NULL : activePred->mItem != NULL) &&
            (isLast  ? activeSucc->mNext == NULL : activeSucc->mItem != NULL)) {

            UpdateHead(); // Ensure x is not reachable from head
            UpdateTail(); // Ensure x is not reachable from tail

            // Finally, actually gc-unlink
            x->LazySetPrev(isFirst ? PrevTerminator().Get() : x);
            x->LazySetNext(isLast  ? NextTerminator().Get() : x);
        }
    }
    return;
}

void CConcurrentLinkedDeque::UnlinkFirst(
    /* [in] */ Node* first,
    /* [in] */ Node* next)
{
    // assert first != null;
    // assert next != null;
    // assert first.item == null;
    for (AutoPtr<Node> o = NULL, p = next, q;;) {
        if (p->mItem != NULL || (q = p->mNext) == NULL) {
            if (o != NULL && (Object::Equals(p->mPrev->Probe(EIID_IInterface), p->Probe(EIID_IInterface))) && first->CasNext(next, p)) {
                SkipDeletedPredecessors(p);
                if (first->mPrev == NULL &&
                    (p->mNext == NULL || p->mItem != NULL) &&
                    (Object::Equals(p->mPrev->Probe(EIID_IInterface), first->Probe(EIID_IInterface)))) {

                    UpdateHead(); // Ensure o is not reachable from head
                    UpdateTail(); // Ensure o is not reachable from tail

                    // Finally, actually gc-unlink
                    o->LazySetNext(o);
                    o->LazySetPrev(PrevTerminator());
                }
            }
            return;
        }
        else if (Object::Equals(p->Probe(EIID_IInterface), q->Probe(EIID_IInterface)))
            return;
        else {
            o = p;
            p = q;
        }
    }
}

void CConcurrentLinkedDeque::UnlinkLast(
    /* [in] */ Node* last,
    /* [in] */ Node* prev)
{
    // assert last != null;
    // assert prev != null;
    // assert last.item == null;
    for (AutoPtr<Node> o = NULL, p = prev, q;;) {
        if (p->mItem != NULL || (q = p->mPrev) == NULL) {
            if (o != NULL && p->mNext != p && last->CasPrev(prev, p)) {
                SkipDeletedSuccessors(p);
                if (last->mNext == NULL &&
                    (p->mPrev == NULL || p->mItem != NULL) &&
                    (Object::Equals(p->mNext->Probe(EIID_IInterface), last->Probe(EIID_IInterface)))) {

                    UpdateHead(); // Ensure o is not reachable from head
                    UpdateTail(); // Ensure o is not reachable from tail

                    // Finally, actually gc-unlink
                    o->LazySetPrev(o);
                    o->LazySetNext(NextTerminator());
                }
            }
            return;
        }
        else if (Object::Equals(p->Probe(EIID_IInterface), q->Probe(EIID_IInterface)))
            return;
        else {
            o = p;
            p = q;
        }
    }
}

void CConcurrentLinkedDeque::UpdateHead()
{
    // Either head already points to an active node, or we keep
    // trying to cas it to the first node until it does.
    AutoPtr<Node> h, p, q;
    RESTARTFROMHEAD:
    while ((h = mHead)->mItem == NULL && (p = h->mPrev) != NULL) {
        for (;;) {
            if ((q = p->mPrev) == NULL ||
                (q = (p = q)->mPrev) == NULL) {
                // It is possible that p is PREV_TERMINATOR,
                // but if so, the CAS is guaranteed to fail.
                if (CasHead(h, p)) {
                    return;
                }
                else {
                    goto RESTARTFROMHEAD;
                }
            }
            else if (!Object::Equals(h->Probe(EIID_IInterface), mHead->Probe(EIID_IInterface))) {
                goto RESTARTFROMHEAD;
            }
            else {
                p = q;
            }
        }
    }
}

void CConcurrentLinkedDeque::UpdateTail()
{
    // Either tail already points to an active node, or we keep
    // trying to cas it to the last node until it does.
    AutoPtr<Node> t, p, q;
    RESTARTFROMTAIL:
    while ((t = mTail)->mItem == NULL && (p = t->mNext) != NULL) {
        for (;;) {
            if ((q = p->mNext) == NULL ||
                (q = (p = q)->mNext) == NULL) {
                // It is possible that p is NEXT_TERMINATOR,
                // but if so, the CAS is guaranteed to fail.
                if (CasTail(t, p))
                    return;
                else
                    goto RESTARTFROMTAIL;
            }
            else if (!Object::Equals(t->Probe(EIID_IInterface), mTail->Probe(EIID_IInterface)))
                goto RESTARTFROMTAIL;
            else {
                p = q;
            }
        }
    }
}

void CConcurrentLinkedDeque::SkipDeletedPredecessors(
    /* [in] */ Node* x)
{
    WHILEACTIVE:
    do {
        AutoPtr<Node> prev = x->mPrev;
        // assert prev != null;
        // assert x != NEXT_TERMINATOR;
        // assert x != PREV_TERMINATOR;
        AutoPtr<Node> p = prev;
        FINDACTIVE:
        for (;;) {
            if (p->mItem != NULL)
                goto FINDACTIVE;
            AutoPtr<Node> q = p->mPrev;
            if (q == NULL) {
                if (Object::Equals(p->mNext->Probe(EIID_IInterface), p->Probe(EIID_IInterface))) {
                    goto WHILEACTIVE;
                }
                goto FINDACTIVE;
            }
            else if (Object::Equals(p->Probe(EIID_IInterface), q->Probe(EIID_IInterface))) {
                goto WHILEACTIVE;
            }
            else
                p = q;
        }

        // found active CAS target
        if ((Object::Equals(prev->Probe(EIID_IInterface), p->Probe(EIID_IInterface))) || x->CasPrev(prev, p))
            return;

    } while (x->mItem != NULL || x->mNext == NULL);
}

void CConcurrentLinkedDeque::SkipDeletedSuccessors(
    /* [in] */ Node* x)
{
    WHILEACTIVE:
    do {
        AutoPtr<Node> next = x->mNext;
        // assert next != null;
        // assert x != NEXT_TERMINATOR;
        // assert x != PREV_TERMINATOR;
        AutoPtr<Node> p = next;
        FINDACTIVE:
        for (;;) {
            if (p->mItem != NULL) {
                goto FINDACTIVE;
            }
            AutoPtr<Node> q = p->mNext;
            if (q == NULL) {
                if (Object::Equals(p->mPrev->Probe(EIID_IInterface), p->Probe(EIID_IInterface)))
                    goto WHILEACTIVE;
                goto FINDACTIVE;
            }
            else if (Object::Equals(p->Probe(EIID_IInterface), q->Probe(EIID_IInterface))) {
                goto WHILEACTIVE;
            }
            else {
                p = q;
            }
        }

        // found active CAS target
        if ((Object::Equals(next->Probe(EIID_IInterface), p->Probe(EIID_IInterface))) || x->CasNext(next, p))
            return;

    } while (x->mItem != NULL || x->mPrev == NULL);
}

AutoPtr<CConcurrentLinkedDeque::Node> CConcurrentLinkedDeque::Succ(
    /* [in] */ Node* p)
{
    // TODO: should we skip deleted nodes here?
    AutoPtr<Node> q = p->mNext;
    if (Object::Equals(q->Probe(EIID_IInterface), p->Probe(EIID_IInterface))) {
        return First();
    }
    return q;
}

AutoPtr<CConcurrentLinkedDeque::Node> CConcurrentLinkedDeque::Pred(
    /* [in] */ Node* p)
{
    AutoPtr<Node> q = p->mPrev;
    if (Object::Equals(q->Probe(EIID_IInterface), p->Probe(EIID_IInterface))) {
        return Last();
    }
    return q;
}

AutoPtr<CConcurrentLinkedDeque::Node> CConcurrentLinkedDeque::First()
{
    RESTARTFROMHEAD:
    for (;;) {
        for (AutoPtr<Node> h = mHead, p = h, q;;) {
            if ((q = p->mPrev) != NULL &&
                (q = (p = q)->mPrev) != NULL)
                // Check for head updates every other hop.
                // If p == q, we are sure to follow head instead.
                p = (h != (h = mHead)) ? h : q;
            else if (Object::Equals(p->Probe(EIID_IInterface), h->Probe(EIID_IInterface))
                     // It is possible that p is PREV_TERMINATOR,
                     // but if so, the CAS is guaranteed to fail.
                     || CasHead(h, p)) {
                return p;
            }
            else {
                goto RESTARTFROMHEAD;
            }
        }
    }
}

AutoPtr<CConcurrentLinkedDeque::Node> CConcurrentLinkedDeque::Last()
{
    RESTARTFROMTAIL:
    for (;;) {
        for (AutoPtr<Node> t = mTail, p = t, q;;) {
            if ((q = p->mNext) != NULL &&
                (q = (p = q)->mNext) != NULL) {
                // Check for tail updates every other hop.
                // If p == q, we are sure to follow tail instead.
                p = (t != (t = mTail)) ? t : q;
            }
            else if (Object::Equals(p->Probe(EIID_IInterface), t->Probe(EIID_IInterface))
                     // It is possible that p is NEXT_TERMINATOR,
                     // but if so, the CAS is guaranteed to fail.
                     || CasTail(t, p)) {
                return p;
            }
            else
                goto RESTARTFROMTAIL;
        }
    }
}

ECode CConcurrentLinkedDeque::CheckNotNull(
    /* [in] */ IInterface* v)
{
    if (v == NULL)
        return E_NULL_POINTER_EXCEPTION;
    return NOERROR;
}

AutoPtr<IInterface> CConcurrentLinkedDeque::ScreenNullResult(
    /* [in] */ IInterface* v)
{
    if (v == NULL)
        return NULL;
        //throw new NoSuchElementException();
    return v;
}

AutoPtr<IArrayList> CConcurrentLinkedDeque::ToArrayList()
{
    AutoPtr<IArrayList> list;
    CArrayList::New((IArrayList**)&list);
    AutoPtr<Node> p = First();
    for (; p != NULL; p = Succ(p)) {
        AutoPtr<IInterface> item = p->mItem;
        if (item != NULL) {
            Boolean res;
            (ICollection::Probe(list))->Add(item, &res);
        }
    }
    return list;
}

ECode CConcurrentLinkedDeque::constructor()
{
    mHead = mTail = new Node(NULL);
    return NOERROR;
}

ECode CConcurrentLinkedDeque::constructor(
    /* [in] */ ICollection* c)
{
    // Copy c into a private chain of Nodes
    AutoPtr<Node> h, t;
    AutoPtr<IIterator> it;
    (IIterable::Probe(c))->GetIterator((IIterator**)&it);
    Boolean isflag = FALSE;
    while ((it->HasNext(&isflag), isflag)) {
        AutoPtr<IInterface> e;
        it->GetNext((IInterface**)&e);
        FAIL_RETURN(CheckNotNull(e));
        AutoPtr<Node> newNode = new Node(e);
        if (h == NULL)
            h = t = newNode;
        else {
            t->LazySetNext(newNode);
            newNode->LazySetPrev(t);
            t = newNode;
        }
    }
    InitHeadTail(h, t);
    return NOERROR;
}

void CConcurrentLinkedDeque::InitHeadTail(
    /* [in] */ Node* h,
    /* [in] */ Node* t)
{
    if (Object::Equals(h->Probe(EIID_IInterface), t->Probe(EIID_IInterface))) {
        if (h == NULL)
            h = t = new Node(NULL);
        else {
            // Avoid edge case of a single Node with non-null item.
            AutoPtr<Node> newNode = new Node(NULL);
            t->LazySetNext(newNode);
            newNode->LazySetPrev(t);
            t = newNode;
        }
    }
    mHead = h;
    mTail = t;
}

ECode CConcurrentLinkedDeque::AddFirst(
    /* [in] */ IInterface* e)
{
    LinkFirst(e);
    return NOERROR;
}

ECode CConcurrentLinkedDeque::AddLast(
    /* [in] */ IInterface* e)
{
    LinkLast(e);
    return NOERROR;
}

ECode CConcurrentLinkedDeque::OfferFirst(
    /* [in] */ IInterface* e,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    LinkFirst(e);
    *value = TRUE;
    return NOERROR;
}

ECode CConcurrentLinkedDeque::OfferLast(
    /* [in] */ IInterface* e,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    LinkLast(e);
    *value = TRUE;
    return NOERROR;
}

ECode CConcurrentLinkedDeque::PeekFirst(
    /* [out] */ IInterface** e)
{
    VALIDATE_NOT_NULL(e);
    AutoPtr<Node> p = First();
    for (; p != NULL; p = Succ(p)) {
        AutoPtr<IInterface> item = p->mItem;
        if (item != NULL) {
            *e = item;
            REFCOUNT_ADD(*e);
            return NOERROR;
        }
    }
    *e = NULL;
    return NOERROR;
}

ECode CConcurrentLinkedDeque::PeekLast(
    /* [out] */ IInterface** e)
{
    VALIDATE_NOT_NULL(e);
    AutoPtr<Node> p = Last();
    for (; p != NULL; p = Pred(p)) {
        AutoPtr<IInterface> item = p->mItem;
        if (item != NULL) {
            *e = item;
            REFCOUNT_ADD(*e);
            return NOERROR;
        }
    }
    *e = NULL;
    return NOERROR;
}

ECode CConcurrentLinkedDeque::GetFirst(
    /* [out] */ IInterface** e)
{
    VALIDATE_NOT_NULL(e);
    PeekFirst(e);
    ScreenNullResult(*e);
    return NOERROR;
}

ECode CConcurrentLinkedDeque::GetLast(
    /* [out] */ IInterface** e)
{
    VALIDATE_NOT_NULL(e);
    PeekLast(e);
    ScreenNullResult(*e);
    return NOERROR;
}

ECode CConcurrentLinkedDeque::PollFirst(
    /* [out] */ IInterface** e)
{
    VALIDATE_NOT_NULL(e);
    AutoPtr<Node> p = First();
    for (; p != NULL; p = Succ(p)) {
        AutoPtr<IInterface> item = p->mItem;
        if (item != NULL && p->CasItem(item, NULL)) {
            Unlink(p);
            *e = item;
            REFCOUNT_ADD(*e);
            return NOERROR;
        }
    }
    *e = NULL;
    return NOERROR;
}

ECode CConcurrentLinkedDeque::PollLast(
    /* [out] */ IInterface** e)
{
    VALIDATE_NOT_NULL(e);
    AutoPtr<Node> p = Last();
    for (; p != NULL; p = Pred(p)) {
        AutoPtr<IInterface> item = p->mItem;
        if (item != NULL && p->CasItem(item, NULL)) {
            Unlink(p);
            *e = item;
            REFCOUNT_ADD(*e);
            return NOERROR;
        }
    }
    *e = NULL;
    return NOERROR;
}

ECode CConcurrentLinkedDeque::RemoveFirst(
    /* [out] */ IInterface** e)
{
    VALIDATE_NOT_NULL(e)
    PollFirst(e);
    ScreenNullResult(*e);
    return NOERROR;
}

ECode CConcurrentLinkedDeque::RemoveLast(
    /* [out] */ IInterface** e)
{
    VALIDATE_NOT_NULL(e)
    PollLast(e);
    ScreenNullResult(*e);
    return NOERROR;
}

ECode CConcurrentLinkedDeque::Offer(
    /* [in] */ IInterface* e,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return OfferLast(e, result);
}

ECode CConcurrentLinkedDeque::Add(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    VALIDATE_NOT_NULL(modified)
    return OfferLast(object, modified);
}

ECode CConcurrentLinkedDeque::Poll(
    /* [out] */ IInterface** e)
{
    VALIDATE_NOT_NULL(e)
    return PollFirst(e);
}

ECode CConcurrentLinkedDeque::Remove(
    /* [out] */ IInterface** e)
{
    VALIDATE_NOT_NULL(e)
    return RemoveFirst(e);
}

ECode CConcurrentLinkedDeque::Peek(
    /* [out] */ IInterface** e)
{
    VALIDATE_NOT_NULL(e)
    return PeekFirst(e);
}

ECode CConcurrentLinkedDeque::GetElement(
    /* [out] */ IInterface** e)
{
    VALIDATE_NOT_NULL(e)
    return GetFirst(e);
}

ECode CConcurrentLinkedDeque::Push(
    /* [in] */ IInterface* e)
{
    return AddFirst(e);
}

ECode CConcurrentLinkedDeque::Pop(
    /* [out] */ IInterface** e)
{
    VALIDATE_NOT_NULL(e)
    return RemoveFirst(e);
}

ECode CConcurrentLinkedDeque::RemoveFirstOccurrence(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)
    FAIL_RETURN(CheckNotNull(o));
    AutoPtr<Node> p = First();
    for (; p != NULL; p = Succ(p)) {
        AutoPtr<IInterface> item = p->mItem;
        if (item != NULL && Object::Equals(o, item) && p->CasItem(item, NULL)) {
            Unlink(p);
            *value = TRUE;
            return NOERROR;
        }
    }
    *value = FALSE;
    return NOERROR;
}

ECode CConcurrentLinkedDeque::RemoveLastOccurrence(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)
    FAIL_RETURN(CheckNotNull(o));
    AutoPtr<Node> p = Last();
    for (; p != NULL; p = Pred(p)) {
        AutoPtr<IInterface> item = p->mItem;
        if (item != NULL && Object::Equals(o, item) && p->CasItem(item, NULL)) {
            Unlink(p);
            *value = TRUE;
            return NOERROR;
        }
    }
    *value = FALSE;
    return NOERROR;
}

ECode CConcurrentLinkedDeque::Contains(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (object == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<Node> p = First();
    for (; p != NULL; p = Succ(p)) {
        AutoPtr<IInterface> item = p->mItem;
        if (item != NULL && Object::Equals(object, item)) {
            *result = TRUE;
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode CConcurrentLinkedDeque::IsEmpty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IInterface> p;
    PeekFirst((IInterface**)&p);
    *result = p == NULL;
    return NOERROR;
}

ECode CConcurrentLinkedDeque::GetSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size)
    Int32 count = 0;
    AutoPtr<Node> p = First();
    for (; p != NULL; p = Succ(p))
        if (p->mItem != NULL)
            // Collection.size() spec says to max out
            if (++count == Elastos::Core::Math::INT32_MAX_VALUE)
                break;
    *size = count;
    return NOERROR;
}

ECode CConcurrentLinkedDeque::Remove(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    VALIDATE_NOT_NULL(modified)
    return RemoveFirstOccurrence(object, modified);
}

ECode CConcurrentLinkedDeque::AddAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    VALIDATE_NOT_NULL(modified)
    if (Object::Equals(collection->Probe(EIID_IInterface), THIS_PROBE(IInterface)))
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // As historically specified in AbstractQueue#addAll
        //throw new IllegalArgumentException();

    // Copy c into a private chain of Nodes
    AutoPtr<Node> beginningOfTheEnd = NULL, last = NULL;
    AutoPtr<IIterator> it;
    (IIterable::Probe(collection))->GetIterator((IIterator**)&it);
    Boolean isflag = FALSE;
    while ((it->HasNext(&isflag), isflag)) {
        AutoPtr<IInterface> e;
        it->GetNext((IInterface**)&e);
        FAIL_RETURN(CheckNotNull(e));
        AutoPtr<Node> newNode = new Node(e);
        if (beginningOfTheEnd == NULL)
            beginningOfTheEnd = last = newNode;
        else {
            last->LazySetNext(newNode);
            newNode->LazySetPrev(last);
            last = newNode;
        }
    }
    if (beginningOfTheEnd == NULL) {
        *modified = FALSE;
        return NOERROR;
    }

    // Atomically append the chain at the tail of this collection
    RESTARTFROMTAIL:
    for (;;) {
        for (AutoPtr<Node> t = mTail, p = t, q;;) {
            if ((q = p->mNext) != NULL &&
                (q = (p = q)->mNext) != NULL) {
                // Check for tail updates every other hop.
                // If p == q, we are sure to follow tail instead.
                p = (t != (t = mTail)) ? t : q;
            }
            else if (Object::Equals(p->mPrev->Probe(EIID_IInterface), p->Probe(EIID_IInterface))) { // NEXT_TERMINATOR
                goto RESTARTFROMTAIL;
            }
            else {
                // p is last node
                beginningOfTheEnd->LazySetPrev(p); // CAS piggyback
                if (p->CasNext(NULL, beginningOfTheEnd)) {
                    // Successful CAS is the linearization point
                    // for all elements to be added to this deque.
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
        }
    }
    return NOERROR;
}

ECode CConcurrentLinkedDeque::Clear()
{
    AutoPtr<IInterface> p;
    while ((PollFirst((IInterface**)&p), p != NULL))
        ;
    return NOERROR;
}

ECode CConcurrentLinkedDeque::ToArray(
    /* [out, callee] */ ArrayOf<IInterface*>** array)
{
    VALIDATE_NOT_NULL(array)
    AutoPtr<IArrayList> p = ToArrayList();
    return (ICollection::Probe(p))->ToArray(array);
}

ECode CConcurrentLinkedDeque::ToArray(
    /* [in] */ ArrayOf<IInterface*>* inArray,
    /* [out, callee] */ ArrayOf<IInterface*>** outArray)
{
    VALIDATE_NOT_NULL(outArray)
    AutoPtr<IArrayList> p = ToArrayList();
    return (ICollection::Probe(p))->ToArray(inArray, outArray);
}

ECode CConcurrentLinkedDeque::GetIterator(
    /* [out] */ IIterator** iterator)
{
    VALIDATE_NOT_NULL(iterator)
    AutoPtr<Itr> p = new Itr(this);
    *iterator = (IIterator*)p->Probe(EIID_IIterator);
    REFCOUNT_ADD(*iterator);
    return NOERROR;
}

ECode CConcurrentLinkedDeque::GetDescendingIterator(
    /* [out] */ IIterator** outiter)
{
    VALIDATE_NOT_NULL(outiter);
    AutoPtr<DescendingItr> p = new DescendingItr(this);
    *outiter = (IIterator*)p->Probe(EIID_IIterator);
    REFCOUNT_ADD(*outiter);
    return NOERROR;
}

ECode CConcurrentLinkedDeque::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    return E_NO_SUCH_METHOD_EXCEPTION;
}

ECode CConcurrentLinkedDeque::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    return E_NO_SUCH_METHOD_EXCEPTION;
}

ECode CConcurrentLinkedDeque::Add(
    /* [in] */ IInterface* object)
{
    return AbstractCollection::Add(object);
}

ECode CConcurrentLinkedDeque::AddAll(
    /* [in] */ ICollection* collection)
{
    return AbstractCollection::AddAll(collection);
}

ECode CConcurrentLinkedDeque::ContainsAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    return AbstractCollection::ContainsAll(collection, result);
}

ECode CConcurrentLinkedDeque::RemoveAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    return AbstractCollection::RemoveAll(collection, result);
}

ECode CConcurrentLinkedDeque::RemoveAll(
    /* [in] */ ICollection* collection)
{
    return AbstractCollection::RemoveAll(collection);
}

ECode CConcurrentLinkedDeque::RetainAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    return AbstractCollection::RetainAll(collection, result);
}

ECode CConcurrentLinkedDeque::RetainAll(
    /* [in] */ ICollection* collection)
{
    return AbstractCollection::RetainAll(collection);
}

ECode CConcurrentLinkedDeque::Remove(
    /* [in] */ IInterface* object)
{
    return AbstractCollection::Remove(object);
}

//====================================================================
// CConcurrentLinkedDeque::AbstractItr::
//====================================================================
CAR_INTERFACE_IMPL(CConcurrentLinkedDeque::AbstractItr, Object, IIterator)

CConcurrentLinkedDeque::AbstractItr::AbstractItr(
    /* [in] */ CConcurrentLinkedDeque* owner)
{
    Advance();
    mOwner = owner;
}

void CConcurrentLinkedDeque::AbstractItr::Advance()
{
    mLastRet = mNextNode;

    AutoPtr<Node> p = (mNextNode == NULL) ? StartNode() : NextNode(mNextNode);
    for (;; p = NextNode(p)) {
        if (p == NULL) {
            // p might be active end or TERMINATOR node; both are OK
            mNextNode = NULL;
            mNextItem = NULL;
            break;
        }
        AutoPtr<IInterface> item = p->mItem;
        if (item != NULL) {
            mNextNode = p;
            mNextItem = item;
            break;
        }
    }
}

ECode CConcurrentLinkedDeque::AbstractItr::HasNext(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mNextItem != NULL;
    return NOERROR;
}

ECode CConcurrentLinkedDeque::AbstractItr::GetNext(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);
    AutoPtr<IInterface> item = mNextItem;
    if (item == NULL)
        return E_NO_SUCH_ELEMENT_EXCEPTION;
    Advance();
    *object = item;
    REFCOUNT_ADD(*object);
    return NOERROR;
}

ECode CConcurrentLinkedDeque::AbstractItr::Remove()
{
    AutoPtr<Node> l = mLastRet;
    if (l == NULL)
        return E_ILLEGAL_STATE_EXCEPTION;
    l->mItem = NULL;
    mOwner->Unlink(l);
    mLastRet = NULL;
    return NOERROR;
}

void CConcurrentLinkedDeque::WriteObject(
    /* [in] */ IObjectOutputStream* s)
{
    // Write out any hidden stuff
    s->DefaultWriteObject();

    // Write out all elements in the proper order.
    AutoPtr<Node> p = First();
    for (; p != NULL; p = Succ(p)) {
        AutoPtr<IInterface> item = p->mItem;
        if (item != NULL)
        /*    s->WriteObject(item)*/;
    }

    // Use trailing null as sentinel
    //s->WriteObject(NULL);
}

void CConcurrentLinkedDeque::ReadObject(
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
            newNode->LazySetPrev(t);
            t = newNode;
        }
    }
    InitHeadTail(h, t);
}

Boolean CConcurrentLinkedDeque::CasHead(
    /* [in] */ Node* cmp,
    /* [in] */ Node* val)
{
    assert(0 && "TODO");
//    return UNSAFE.compareAndSwapObject(this, headOffset, cmp, val);
    return FALSE;
}

Boolean CConcurrentLinkedDeque::CasTail(
    /* [in] */ Node* cmp,
    /* [in] */ Node* val)
{
    assert(0 && "TODO");
//    return UNSAFE.compareAndSwapObject(this, tailOffset, cmp, val);
    return FALSE;
}

//static sun.misc.Unsafe UNSAFE;
Int64 CConcurrentLinkedDeque::sHeadOffset;
Int64 CConcurrentLinkedDeque::sTailOffset;
// static {
//     PREV_TERMINATOR = new Node<Object>();
//     PREV_TERMINATOR.next = PREV_TERMINATOR;
//     NEXT_TERMINATOR = new Node<Object>();
//     NEXT_TERMINATOR.prev = NEXT_TERMINATOR;
//     try {
//         UNSAFE = sun.misc.Unsafe.getUnsafe();
//         Class<?> k = ConcurrentLinkedDeque.class;
//         headOffset = UNSAFE.objectFieldOffset
//             (k.getDeclaredField("head"));
//         tailOffset = UNSAFE.objectFieldOffset
//             (k.getDeclaredField("tail"));
//     } catch (Exception e) {
//         throw new Error(e);
//     }
// }

} // namespace Concurrent
} // namespace Utility
} // namespace Elastos
