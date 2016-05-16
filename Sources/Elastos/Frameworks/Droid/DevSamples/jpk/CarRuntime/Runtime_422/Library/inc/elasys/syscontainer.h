//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#ifndef __ELASTOS_CONTAINER_H__
#define __ELASTOS_CONTAINER_H__

//
// In this file, there are two simple container implementations, they are
// ObjectContainer and CallbackContextContainer, both of them inherited from
// base class SimpleContainer. They provide six methods: Current,
// MoveNext, Reset, Add, Remove and Dispose.
// In fact, Add, Remove and Dispose are right methods of container, but
// the Current, MoveNext and Reset are more like methods of enumerator,
// we take them together for terse reason.
//
// Using them in multi-threading program is not safe, the synchronizing
// mechanism should be decided by outer codes.
// We don't record reference count of interface in ObjectContainer, the
// counting rules should be decided by outer codes too. A more precise
// definition and implementation of ObjectContainer is suplied with CAR
// aspect class AObjectContainer in elastos.dll.
//

#include <eltypes.h>
#include <syslinknode.h>

/** @addtogroup BaseTypesRef
  *   @{
  */
class SimpleContainer
{
public:
    SimpleContainer();

    _ELASTOS ECode Current(DLinkNode **);
    _ELASTOS ECode MoveNext();
    _ELASTOS ECode Reset();
    _ELASTOS ECode Add(DLinkNode *);
    _ELASTOS ECode Remove(DLinkNode *);

public:
    DLinkNode  mHead;
    DLinkNode  *mCurrent;
};
/** @} */

CAR_INLINE SimpleContainer::SimpleContainer()
{
    mHead.Initialize();
    mCurrent = &mHead;
}

CAR_INLINE _ELASTOS ECode SimpleContainer::Current(DLinkNode **ppNode)
{
    assert(NULL != ppNode);

    if (NULL == mCurrent || &mHead == mCurrent) {
        return E_INVALID_OPERATION;
    }
    *ppNode = mCurrent;
    return NOERROR;
}

CAR_INLINE _ELASTOS ECode SimpleContainer::MoveNext()
{
    if (NULL == mCurrent) {
        return E_INVALID_OPERATION;
    }
    if (mCurrent == mHead.Prev()) {
        mCurrent = NULL;
        return S_FALSE;
    }
    mCurrent = mCurrent->Next();
    return NOERROR;
}

CAR_INLINE _ELASTOS ECode SimpleContainer::Reset()
{
    mCurrent = &mHead;
    return NOERROR;
}

CAR_INLINE _ELASTOS ECode SimpleContainer::Add(DLinkNode *pNode)
{
    mHead.InsertPrev(pNode);
    return NOERROR;
}

CAR_INLINE _ELASTOS ECode SimpleContainer::Remove(DLinkNode *pNode)
{
    if (mCurrent == pNode) {
        mCurrent = mCurrent->Prev();
    }
    pNode->Detach();
    return NOERROR;
}

/** @addtogroup BaseTypesRef
  *   @{
  */
struct ObjectNode : public DLinkNode
{
    ObjectNode(PInterface pObj) { mObject = pObj; }

    PInterface mObject;
};

class ObjectContainer : public SimpleContainer
{
public:
    _ELASTOS ECode Current(PInterface *ppObj);
    _ELASTOS ECode Add(PInterface pObj);
    _ELASTOS ECode Remove(PInterface pObj);
    _ELASTOS ECode Dispose();

    virtual ~ObjectContainer();
};
/** @} */

CAR_INLINE ObjectContainer::~ObjectContainer()
{
    this->Dispose();
}

CAR_INLINE _ELASTOS ECode ObjectContainer::Dispose()
{
    ObjectNode *pNode;

    pNode = (ObjectNode *)mHead.Next();
    while (&mHead != (DLinkNode *)pNode) {
        pNode->Detach();
        delete pNode;
        pNode = (ObjectNode *)mHead.Next();
    }
    mCurrent = NULL;
    return NOERROR;
}

CAR_INLINE _ELASTOS ECode ObjectContainer::Current(PInterface *ppObj)
{
    _ELASTOS ECode ec;
    ObjectNode *pNode;

    ec = SimpleContainer::Current((DLinkNode **)&pNode);
    if (NOERROR == ec) *ppObj = pNode->mObject;
    return ec;
}

CAR_INLINE _ELASTOS ECode ObjectContainer::Add(PInterface pObj)
{
    ObjectNode *pNode;

    pNode = new ObjectNode(pObj);
    if (NULL == pNode) return E_OUT_OF_MEMORY;

    return SimpleContainer::Add(pNode);
}

CAR_INLINE _ELASTOS ECode ObjectContainer::Remove(PInterface pObj)
{
    ObjectNode *pNode;

    ForEachDLinkNode(ObjectNode *, pNode, &mHead) {
        if (pNode->mObject == pObj) {
            SimpleContainer::Remove(pNode);
            delete pNode;
            return NOERROR;
        }
    }
    return S_FALSE;
}

#endif // __ELASTOS_CONTAINER_H__
