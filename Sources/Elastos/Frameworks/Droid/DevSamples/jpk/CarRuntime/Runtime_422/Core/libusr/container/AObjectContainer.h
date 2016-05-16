//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#ifndef __AOBJECTCONTAINER_H__
#define __AOBJECTCONTAINER_H__

#include "_AObjectContainer.h"
#include "CEnumerator.h"
#include <elapi.h>

class AObjectContainer : public _AObjectContainer
{
public:
    CARAPI Add(
        /* [in] */ IInterface *pObject);

    CARAPI Remove(
        /* [in] */ IInterface *pObject);

    CARAPI GetObjectEnumerator(
        /* [out] */ IObjectEnumerator **ppObjects);

    CARAPI GetObjectCount(
        /* [out] */ Int32 *pCount);

    CARAPI Dispose();

    CARAPI constructor();

    ~AObjectContainer();

private:
    int                 m_nCount;
    int                 m_nState;
    ObjectContainer     m_container;
    CriticalSection     m_lock;
};

CAR_INLINE AObjectContainer::~AObjectContainer()
{
    this->Dispose();
    _CCriticalSection_Terminate(&m_lock);
}

#endif // __AOBJECTCONTAINER_H__
