//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#include "CConstantInfo.h"

UInt32 CConstantInfo::AddRef()
{
    Int32 nRef = atomic_inc(&m_cRef);
    return (UInt32)nRef;
}

UInt32 CConstantInfo::Release()
{
    Int32 nRef = atomic_dec(&m_cRef);

    if (0 == nRef) {
        delete this;
    }
    assert(nRef >= 0);
    return nRef;
}

PInterface CConstantInfo::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)this;
    }
    else if (riid == EIID_IConstantInfo) {
        return (IConstantInfo *)this;
    }
    else {
        return NULL;
    }
}

ECode CConstantInfo::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    return E_NOT_IMPLEMENTED;
}

CConstantInfo::CConstantInfo(
    /* [in] */ CClsModule * pCClsModule,
    /* [in] */ ConstDirEntry *pConstDirEntry)
{
    m_pCClsModule = pCClsModule;
    m_pCClsModule->AddRef();
    m_pConstDirEntry = pConstDirEntry;
    m_cRef = 0;
}

CConstantInfo::~CConstantInfo()
{
    if (m_pCClsModule) m_pCClsModule->Release();
}

ECode CConstantInfo::GetName(
    /* [out] */ StringBuf * pName)
{
    if (pName == NULL || !pName->GetCapacity()) {
        return E_INVALID_ARGUMENT;
    }

    pName->Copy(adjustNameAddr(m_pCClsModule->m_nBase, m_pConstDirEntry->pszName));
    return NOERROR;
}

ECode CConstantInfo::GetValue(
    /* [out] */ Int32 * pValue)
{
    if (!pValue) {
        return E_INVALID_ARGUMENT;
    }

    if (m_pConstDirEntry->type == TYPE_INTEGER32) {
        *pValue = m_pConstDirEntry->v.intValue.nValue;
        return NOERROR;
    }
    else return E_INVALID_OPERATION;
}

ECode CConstantInfo::GetModuleInfo(
    /* [out] */ IModuleInfo ** ppModuleInfo)
{
    return m_pCClsModule->GetModuleInfo(ppModuleInfo);
}
