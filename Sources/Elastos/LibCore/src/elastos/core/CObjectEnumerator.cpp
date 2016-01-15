#include "CObjectEnumerator.h"

namespace Elastos{
namespace Core{


UInt32 CObjectEnumerator::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 CObjectEnumerator::Release()
{
    return ElRefBase::Release();
}

PInterface CObjectEnumerator::Probe(
    REIID riid)
{
    if (EIID_IInterface == riid) {
        return (IInterface *)(IObjectEnumerator *)this;
    }
    else if (riid == EIID_IObject) {
        return (IObject *)this;
    }
    else if (EIID_IObjectEnumerator == riid) {
        return (IObjectEnumerator *)this;
    }
    else {
        return NULL;
    }
}

ECode CObjectEnumerator::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    if (NULL == pIID) return E_INVALID_ARGUMENT;

    if (pObject == (IInterface *)(IObject *)this) {
        *pIID = EIID_IObject;
    }
    else if (pObject == (IInterface *)(IObjectEnumerator *)this) {
        *pIID = EIID_IObjectEnumerator;
    }
    else {
        return E_INVALID_ARGUMENT;
    }
    return NOERROR;
}

ECode CObjectEnumerator::Aggregate(
    /* [in] */ AggrType aggrType,
    /* [in] */ PInterface pObject)
{
    return E_NOT_IMPLEMENTED;
}

ECode CObjectEnumerator::GetDomain(
    /* [out] */ PInterface *ppObj)
{
    return E_NOT_IMPLEMENTED;
}

ECode CObjectEnumerator::GetClassID(
    /* [out] */ ClassID *pCLSID)
{
    return E_NOT_IMPLEMENTED;
}

ECode CObjectEnumerator::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    if (NULL == obj) return E_INVALID_ARGUMENT;
    *result = FALSE;
    if (NULL == result) return E_INVALID_ARGUMENT;

    IObjectEnumerator * emu = IObjectEnumerator::Probe(obj);
    *result = (emu == THIS_PROBE(IObjectEnumerator));
    return NOERROR;
}

ECode CObjectEnumerator::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    if (NULL == hashCode) return E_INVALID_ARGUMENT;
    *hashCode = (Int32)THIS_PROBE(IObjectEnumerator);
    return NOERROR;
}

ECode CObjectEnumerator::ToString(
    /* [out] */ String* info)
{
    if (NULL == info) return E_INVALID_ARGUMENT;
    String str("[IObjectEnumerator:");
    str.AppendFormat("%p]", THIS_PROBE(IObjectEnumerator));
    *info = str;
    return NOERROR;
}

ECode CObjectEnumerator::Current(
    /* [out] */ PInterface *ppObj)
{
    if (NULL == ppObj) return E_INVALID_ARGUMENT;
    *ppObj = NULL;

    if (NULL == mCurrent || mHeader == mCurrent) {
        return E_INVALID_OPERATION;
    }

    Mutex::AutoLock lock(mLock);

    if (mOrgState != *mState) {
        mCurrent = NULL;
        return E_INVALID_OPERATION;
    }

    *ppObj = mCurrent->mObject;
    REFCOUNT_ADD(*ppObj);
    return NOERROR;
}

ECode CObjectEnumerator::MoveNext(
    /* [in] */ Boolean *pSucceeded)
{
    if (NULL == mCurrent) {
        *pSucceeded = FALSE;
        return E_INVALID_OPERATION;
    }

    Mutex::AutoLock lock(mLock);

    if (mOrgState != *mState) {
        mCurrent = NULL;
        *pSucceeded = FALSE;
        return E_INVALID_OPERATION;
    }

    if (mCurrent == mHeader->Prev()) {
        mCurrent = NULL;
        *pSucceeded = FALSE;
        return NOERROR;
    }

    mCurrent = (ObjectNode *)mCurrent->Next();
    *pSucceeded = TRUE;
    return NOERROR;
}

ECode CObjectEnumerator::Reset()
{
    mCurrent = mHeader;
    mOrgState = *mState;
    return NOERROR;
}

} // namespace Core
} // namespace Elastos
