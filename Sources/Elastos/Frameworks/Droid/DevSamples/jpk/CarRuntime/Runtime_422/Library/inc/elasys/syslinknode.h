//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================
#include <elatypes.h>
#include <assert.h>

#ifndef __ELASTOS_LINKNODE_H__
#define __ELASTOS_LINKNODE_H__

#define INVALID_ADDRVALUE           ((virtaddr_t)(0xcccccccc))

#ifndef ForEachDLinkNode

#define ForEachDLinkNode(t, p, h) \
        for (p = (t)((h)->m_pNext); p != (t)h; p = (t)(p->m_pNext))

#define FOR_EACH_DLINKNODE(t, p, h) \
        ForEachDLinkNode(t, p, h)

#define ForEachDLinkNodeReversely(t, p, h) \
        for (p = (t)((h)->m_pPrev); p != (t)h; p = (t)(p->m_pPrev))

#define FOR_EACH_DLINKNODE_REVERSELY(t, p, h) \
        ForEachDLinkNodeReversely(t, p, h)

#endif // ForEachDLinkNode

#ifndef ForEachSLinkNode

#define ForEachSLinkNode(t, p, h) \
        for (p = (t)((h)->m_pNext); p != (t)h; p = (t)(p->m_pNext))

#define FOR_EACH_SLINKNODE(t, p, h) \
        ForEachSLinkNode(t, p, h)

#endif // ForEachSLinkNode

//
//  Double Link Node -- For double link circular list with head-node.
//
class DoubleLinkNode
{
public:

#if defined(_DEBUG) && !defined(__NO_LINKNODE_CONSTRUCTOR)
    DoubleLinkNode()
    {
        m_pPrev = m_pNext = (DoubleLinkNode *)INVALID_ADDRVALUE;
    }
#endif

    //
    // Non-head-node operations
    //
    // Return the previous node.
    DoubleLinkNode *Prev() const    { return m_pPrev; }

    // Return the next node.
    DoubleLinkNode *Next() const    { return m_pNext; }

    // Insert a new node before me. Return the new previous node.
    DoubleLinkNode *InsertPrev(DoubleLinkNode *pPrevNode);
    DoubleLinkNode *InsertPrev(
        DoubleLinkNode *pPrevNode, uint_t *puNumberOfNodes);

    // Insert a new node after me.  Return the new next node.
    DoubleLinkNode *InsertNext(DoubleLinkNode *pNextNode);
    DoubleLinkNode *InsertNext(
        DoubleLinkNode *pNextNode, uint_t *puNumberOfNodes);

    // Remove myself from list. Return myself.
    DoubleLinkNode *Detach();
    DoubleLinkNode *Detach(uint_t *puNumberOfNodes);

    //
    // Head-node operations
    //
    void Initialize()               { m_pPrev = m_pNext = this; }

    bool_t IsEmpty() const          { return this == m_pNext; }

    // Return the first node of list.
    DoubleLinkNode *First() const   { return Next(); }

    // Return the last node of list.
    DoubleLinkNode *Last()  const   { return Prev(); }

    // Insert a new node as the first of list. Return the new first node.
    DoubleLinkNode *InsertFirst(DoubleLinkNode *pFirstNode)
    {
        return InsertNext(pFirstNode);
    }

    DoubleLinkNode *InsertFirst(
        DoubleLinkNode *pFirstNode, uint_t *puNumberOfNodes)
    {
        return InsertNext(pFirstNode, puNumberOfNodes);
    }

    // Insert a new node as the last of list.  Return the new last node.
    DoubleLinkNode *InsertLast(DoubleLinkNode *pLastNode)
    {
        return InsertPrev(pLastNode);
    }

    DoubleLinkNode *InsertLast(
        DoubleLinkNode *pLastNode, uint_t *puNumberOfNodes)
    {
        return InsertPrev(pLastNode, puNumberOfNodes);
    }

    void AssertValid() const;
    void AssertValid(uint_t uNumberOfNodes) const;

public:
    DoubleLinkNode      *m_pPrev;
    DoubleLinkNode      *m_pNext;
};

typedef DoubleLinkNode DLinkNode;

CAR_INLINE DoubleLinkNode *DoubleLinkNode::InsertPrev(DoubleLinkNode *pPrevNode)
{
    assert(pPrevNode);

    pPrevNode->m_pPrev  = m_pPrev;
    pPrevNode->m_pNext  = this;
    m_pPrev->m_pNext    = pPrevNode;
    m_pPrev             = pPrevNode;

    return pPrevNode;
}

CAR_INLINE DoubleLinkNode *DoubleLinkNode::InsertPrev(
    DoubleLinkNode *pPrevNode, uint_t *puNumberOfNodes)
{
    assert(pPrevNode);

    pPrevNode->m_pPrev  = m_pPrev;
    pPrevNode->m_pNext  = this;
    m_pPrev->m_pNext    = pPrevNode;
    m_pPrev             = pPrevNode;
    (*puNumberOfNodes)++;

    return pPrevNode;
}

CAR_INLINE DoubleLinkNode *DoubleLinkNode::InsertNext(DoubleLinkNode *pNextNode)
{
    assert(pNextNode);

    pNextNode->m_pPrev  = this;
    pNextNode->m_pNext  = m_pNext;
    m_pNext->m_pPrev    = pNextNode;
    m_pNext             = pNextNode;

    return pNextNode;
}

CAR_INLINE DoubleLinkNode *DoubleLinkNode::InsertNext(
    DoubleLinkNode *pNextNode, uint_t *puNumberOfNodes)
{
    assert(pNextNode);

    pNextNode->m_pPrev  = this;
    pNextNode->m_pNext  = m_pNext;
    m_pNext->m_pPrev    = pNextNode;
    m_pNext             = pNextNode;
    (*puNumberOfNodes)++;

    return pNextNode;
}

CAR_INLINE DoubleLinkNode *DoubleLinkNode::Detach()
{
    m_pPrev->m_pNext = m_pNext;
    m_pNext->m_pPrev = m_pPrev;
#ifdef _DEBUG
    m_pPrev = m_pNext = (DoubleLinkNode *)INVALID_ADDRVALUE;
#endif

    return this;
}

CAR_INLINE DoubleLinkNode *DoubleLinkNode::Detach(uint_t *puNumberOfNodes)
{
    m_pPrev->m_pNext = m_pNext;
    m_pNext->m_pPrev = m_pPrev;
#ifdef _DEBUG
    m_pPrev = m_pNext = (DoubleLinkNode *)INVALID_ADDRVALUE;
#endif
    (*puNumberOfNodes)--;

    return this;
}

CAR_INLINE void DoubleLinkNode::AssertValid() const
{
#if defined(_DEBUG)
    assert(m_pPrev->m_pNext == this);
    assert(m_pNext->m_pPrev == this);

    DoubleLinkNode *pNode;
    ForEachDLinkNode(DoubleLinkNode *, pNode, this) {
        assert(pNode->m_pPrev->m_pNext == pNode);
        assert(pNode->m_pNext->m_pPrev == pNode);
    }
#endif
}

CAR_INLINE void DoubleLinkNode::AssertValid(uint_t uNumberOfNodes) const
{
#if defined(_DEBUG)
    assert(m_pPrev->m_pNext == this);
    assert(m_pNext->m_pPrev == this);

    uint_t uActualNumberOfNodes = 0;
    DoubleLinkNode *pNode;
    ForEachDLinkNode(DoubleLinkNode *, pNode, this) {
        assert(pNode->m_pPrev->m_pNext == pNode);
        assert(pNode->m_pNext->m_pPrev == pNode);
        uActualNumberOfNodes++;
    }

    assert(uNumberOfNodes == uActualNumberOfNodes);
#endif
}

//
//  Single Link Node -- For single link circular list with head-node.
//
class SingleLinkNode
{
public:

#if defined(_DEBUG) && !defined(__NO_LINKNODE_CONSTRUCTOR)
    SingleLinkNode()
    {
        m_pNext = (SingleLinkNode *)INVALID_ADDRVALUE;
    }
#endif

    //
    // Non-head-node operations
    //
    // Return the next node.
    SingleLinkNode *Next() const    { return m_pNext; }

    // Insert a new node after me.  Return the new next node.
    SingleLinkNode *InsertNext(SingleLinkNode *pNextNode);
    SingleLinkNode *InsertNext(
        SingleLinkNode *pNextNode, uint_t *puNumberOfNodes);

    // Remove myself from list. Return myself.
    SingleLinkNode *Detach(SingleLinkNode *pPrevNode);
    SingleLinkNode *Detach(SingleLinkNode *pPrevNode, uint_t *puNumberOfNodes);

    //
    // Head-node operations
    //
    void Initialize()               { m_pNext = this; }

    bool_t IsEmpty() const          { return this == m_pNext; }

    // Return the first node of list.
    SingleLinkNode *First() const   { return Next(); }

    // Insert a new node as the first of list. Return the new first node.
    SingleLinkNode *InsertFirst(SingleLinkNode *pFirstNode)
    {
        return InsertNext(pFirstNode);
    }

    SingleLinkNode *InsertFirst(
        SingleLinkNode *pFirstNode, uint_t *puNumberOfNodes)
    {
        return InsertNext(pFirstNode, puNumberOfNodes);
    }

    void AssertValid() const;
    void AssertValid(uint_t uNumberOfNodes) const;

public:
    SingleLinkNode      *m_pNext;
};

typedef SingleLinkNode SLinkNode;

CAR_INLINE SingleLinkNode *SingleLinkNode::InsertNext(SingleLinkNode *pNextNode)
{
    assert(pNextNode);

    pNextNode->m_pNext  = m_pNext;
    m_pNext             = pNextNode;

    return pNextNode;
}

CAR_INLINE SingleLinkNode *SingleLinkNode::InsertNext(
    SingleLinkNode *pNextNode, uint_t *puNumberOfNodes)
{
    assert(pNextNode);

    pNextNode->m_pNext  = m_pNext;
    m_pNext             = pNextNode;
    (*puNumberOfNodes)++;

    return pNextNode;
}

CAR_INLINE SingleLinkNode *SingleLinkNode::Detach(SingleLinkNode *pPrevNode)
{
    pPrevNode->m_pNext = m_pNext;
#ifdef _DEBUG
    m_pNext = (SingleLinkNode *)INVALID_ADDRVALUE;
#endif

    return this;
}

CAR_INLINE SingleLinkNode *SingleLinkNode::Detach(
    SingleLinkNode *pPrevNode, uint_t *puNumberOfNodes)
{
    pPrevNode->m_pNext = m_pNext;
#ifdef _DEBUG
    m_pNext = (SingleLinkNode *)INVALID_ADDRVALUE;
#endif
    (*puNumberOfNodes)--;

    return this;
}

CAR_INLINE void SingleLinkNode::AssertValid() const
{
#if defined(_DEBUG)
    assert(m_pNext);

    SingleLinkNode *pNode;
    ForEachSLinkNode(SingleLinkNode *, pNode, this) {
        assert(pNode->m_pNext);
    }
#endif
}

CAR_INLINE void SingleLinkNode::AssertValid(uint_t uNumberOfNodes) const
{
#if defined(_DEBUG)
    assert(m_pNext);

    uint_t uActualNumberOfNodes = 0;
    SingleLinkNode *pNode;
    ForEachSLinkNode(SingleLinkNode *, pNode, this) {
        assert(pNode->m_pNext);
        uActualNumberOfNodes++;
    }

    assert(uNumberOfNodes == uActualNumberOfNodes);
#endif
}

#endif //__ELASTOS_LINKNODE_H__
