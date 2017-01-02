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

#include <cmdef.h>
#include <CObjectContainer.h>
#include <elatypes.h>
#include <elapi.h>

namespace Elastos {
namespace Core {

CObjectContainer::CObjectContainer()
    : mCount(0)
    , mState(0)
{}

CObjectContainer::~CObjectContainer()
{
    this->Dispose();
}

ECode CObjectContainer::Add(
    /* [in] */ IInterface *pObject)
{
    VALIDATE_NOT_NULL(pObject);

    Mutex::AutoLock lock(_m_syncLock);

    ObjectNode *pNode = new ObjectNode(pObject);
    if (NULL == pNode) {
        return E_OUT_OF_MEMORY;
    }

    SimpleContainer::Add(pNode);
    mCount++;
    mState++;

    return NOERROR;
}

ECode CObjectContainer::Remove(
    /* [in] */ IInterface *pObject)
{
    VALIDATE_NOT_NULL(pObject);

    Mutex::AutoLock lock(_m_syncLock);

    ObjectNode *pNode;
    ForEachDLinkNode(ObjectNode *, pNode, &mHead) {
        if (pNode->mObject.Get() == pObject) {
            SimpleContainer::Remove(pNode);
            mCount--;
            mState++;
            delete pNode;

            return NOERROR;
        }
    }

    return S_FALSE;
}

ECode CObjectContainer::GetObjectCount(
    /* [out] */ Int32 *pCount)
{
    VALIDATE_NOT_NULL(pCount);
    *pCount = mCount;
    return NOERROR;
}

ECode CObjectContainer::GetObjectEnumerator(
    /* [out] */ IObjectEnumerator **ppEnumerator)
{
    VALIDATE_NOT_NULL(ppEnumerator);
    *ppEnumerator = NULL;

    CObjectEnumerator *pEnum;

    pEnum = new CObjectEnumerator(
                    (ObjectNode *)&mHead,
                    &_m_syncLock,
                    &mState);
    if (NULL == pEnum) return E_OUT_OF_MEMORY;

    *ppEnumerator = (IObjectEnumerator *)pEnum;
    REFCOUNT_ADD(*ppEnumerator);

    return NOERROR;
}

ECode CObjectContainer::Contains(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* contains)
{
    VALIDATE_NOT_NULL(contains);
    *contains = FALSE;
    VALIDATE_NOT_NULL(object);

    Mutex::AutoLock lock(_m_syncLock);

    AutoPtr<IObjectEnumerator> pEnumerator;
    ECode ec = GetObjectEnumerator((IObjectEnumerator**)&pEnumerator);
    if (FAILED(ec))
        return ec;

    Boolean hasNext = FALSE;
    while (pEnumerator->MoveNext(&hasNext), hasNext) {
        AutoPtr<IInterface> curObj;
        pEnumerator->Current((IInterface**)&curObj);
        if (object == curObj) {
            *contains = TRUE;
            break;
        }
    }
    return NOERROR;
}

ECode CObjectContainer::ContainsAll(
    /* [in] */ IObjectContainer* objectContainer,
    /* [out] */ Boolean* contains)
{
    VALIDATE_NOT_NULL(contains);

    Mutex::AutoLock lock(_m_syncLock);

    AutoPtr<IObjectEnumerator> emu;
    objectContainer->GetObjectEnumerator((IObjectEnumerator**)&emu);
    Boolean hasNext;
    emu->MoveNext(&hasNext);
    while (hasNext) {
        AutoPtr<IInterface> object;
        emu->Current((IInterface**)&object);
        Boolean has;
        Contains(object, &has);
        if (has == FALSE){
            *contains = FALSE;
            return NOERROR;
        }
    }
    *contains = TRUE;
    return NOERROR;
}

ECode CObjectContainer::Dispose()
{
    Mutex::AutoLock lock(_m_syncLock);

    mCount = 0;
    mState++;

    ObjectNode *pNode = (ObjectNode *)mHead.Next();
    while (&mHead != (DLinkNode *)pNode) {
        pNode->Detach();
        delete pNode;
        pNode = (ObjectNode *)mHead.Next();
    }
    mCurrent = NULL;

    return NOERROR;
}

} // namespace Core
} // namespace Elastos
