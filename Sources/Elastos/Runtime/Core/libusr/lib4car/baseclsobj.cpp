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

#include <car.h>

_ELASTOS_NAMESPACE_USING

extern "C" Int32 g_dllLockCount;

PInterface _CBaseClassObject::Probe(
    /* [in] */ REIID riid)
{
    if (EIID_IInterface == riid) {
        return (IInterface *)(IClassObject *)this;
    }
    else if (EIID_IObject == riid) {
        return (IObject *)this;
    }
    else if (EIID_IClassObject == riid) {
        return (IClassObject *)this;
    }
    else {
        return NULL;
    }
}

UInt32 _CBaseClassObject::AddRef()
{
    atomic_inc(&g_dllLockCount);
    return 2;
}

UInt32 _CBaseClassObject::Release()
{
    atomic_dec(&g_dllLockCount);
    return 1;
}

ECode _CBaseClassObject::GetInterfaceID(
    /* [in] */ IInterface* object,
    /* [out] */ InterfaceID* iid)
{
    if (NULL == iid) return E_INVALID_ARGUMENT;

    if (object == (IInterface *)(IObject *)this) {
        *iid = EIID_IObject;
    }
    else if (object == (IInterface *)(IClassObject *)this) {
        *iid = EIID_IClassObject;
    }
    else {
        return E_INVALID_ARGUMENT;
    }
    return NOERROR;
}

ECode _CBaseClassObject::Aggregate(
    /* [in] */ AggregateType type,
    /* [in] */ PInterface pObj)
{
    return E_NOT_IMPLEMENTED;
}

ECode _CBaseClassObject::GetDomain(
    /* [out] */ PInterface* object)
{
    return E_NOT_IMPLEMENTED;
}

ECode _CBaseClassObject::GetClassID(
    /* [out] */ ClassID* clsid)
{
    if (NULL == clsid) return E_INVALID_ARGUMENT;

    *clsid = ECLSID_CClassObject;
    return NOERROR;
}

ECode _CBaseClassObject::CreateObject(
    /* [in] */ IInterface* outer,
    /* [in] */ REIID riid,
    /* [out] */ IInterface** retObject)
{
    IInterface* object;
    if (NULL == mCreateObjectFunc) return E_CLASS_NOT_AVAILABLE;
    ECode ec = (*mCreateObjectFunc)(&object);
    if (FAILED(ec)) return ec;
    *retObject = object->Probe(riid);
    if (!(*retObject)) {
        object->Release();
        return E_NO_INTERFACE;
    }
    (*retObject)->AddRef();
    object->Release();
    return ec;
}

ECode _CBaseClassObject::StayResident(
    /* [in] */ Boolean isStayResident)
{
    if (isStayResident) {
        atomic_inc(&g_dllLockCount);
    }
    else {
        atomic_dec(&g_dllLockCount);
    }
    return NOERROR;
}
