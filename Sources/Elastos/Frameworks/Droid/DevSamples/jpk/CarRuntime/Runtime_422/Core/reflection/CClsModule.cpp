//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#include "CClsModule.h"
#include "CEntryList.h"
#include <dlfcn.h>

UInt32 CClsModule::AddRef()
{
    Int32 nRef = atomic_inc(&m_cRef);
    return (UInt32)nRef;
}

UInt32 CClsModule::Release()
{
    g_objInfoList.LockHashTable(EntryType_ClsModule);
    Int32 nRef = atomic_dec(&m_cRef);

    if (0 == nRef) {
        g_objInfoList.RemoveClsModule(m_sbPath);
        delete this;
    }
    g_objInfoList.UnlockHashTable(EntryType_ClsModule);
    assert(nRef >= 0);
    return nRef;
}

CClsModule::~CClsModule()
{
    if (m_pTypeAliasList) {
        for (int i = 0; i < m_pClsMod->cAliases; i++) {
            if (m_pTypeAliasList[i].pOrgTypeDesc) {
                delete m_pTypeAliasList[i].pOrgTypeDesc;
            }
        }
        delete[] m_pTypeAliasList;
    }

    if (m_bAllocedClsMode) {
        if (m_pClsMod) DisposeFlattedCLS(m_pClsMod);
    }
    else {
        if (m_pIModule) dlclose(m_pIModule);
    }
}

CClsModule::CClsModule(CLSModule *pClsMod, Boolean bAllocedClsMod,
                CString path, Void *pIModule)
{
    m_pClsMod = pClsMod;
    m_bAllocedClsMode = bAllocedClsMod;
    m_sbPath.Copy(path);
    m_pTypeAliasList = NULL;
    m_pIModule = NULL;
    m_cRef = 0;
    if (!m_bAllocedClsMode && pIModule) {
        m_pIModule = pIModule;
   }

    if (bAllocedClsMod)
        m_nBase = 0;
    else
        m_nBase = Int32(pClsMod);
}

ECode CClsModule::GetModuleInfo(
    /* [out] */ IModuleInfo ** ppModuleInfo)
{
    if (!ppModuleInfo) {
        return E_INVALID_ARGUMENT;
    }

    return _CReflector_AcquireModuleInfo(adjustNameAddr(this->m_nBase, m_pClsMod->pszUunm), ppModuleInfo);
}

ECode CClsModule::InitOrgType()
{
    if (!m_pClsMod->cAliases) {
        return NOERROR;
    }

    int i = 0;
    m_pTypeAliasList = new TypeAliasDesc[m_pClsMod->cAliases];
    if (!m_pTypeAliasList) {
        return E_OUT_OF_MEMORY;
    }

    memset(m_pTypeAliasList, 0, sizeof(TypeAliasDesc) * m_pClsMod->cAliases);

    for (i = 0; i < m_pClsMod->cAliases; i++) {
        m_pTypeAliasList[i].pOrgTypeDesc = new TypeDescriptor;
        if (!m_pTypeAliasList[i].pOrgTypeDesc) goto EExit;
        m_pTypeAliasList[i].pTypeDesc = &(getAliasDirAddr(this->m_nBase,
                m_pClsMod->ppAliasDir, i)->type);
        _GetOriginalType(this, m_pTypeAliasList[i].pTypeDesc,
                m_pTypeAliasList[i].pOrgTypeDesc);
    }

    return NOERROR;

EExit:
    for (i = 0; i < m_pClsMod->cAliases; i++) {
        if (m_pTypeAliasList[i].pOrgTypeDesc) {
            delete m_pTypeAliasList[i].pOrgTypeDesc;
        }
    }

    delete[] m_pTypeAliasList;
    m_pTypeAliasList = NULL;

    return E_OUT_OF_MEMORY;
}

ECode CClsModule::AliasToOriginal(
    /* [in] */ TypeDescriptor *pTypeDype,
    /* [out] */ TypeDescriptor **ppOrgTypeDesc)
{
    if (!m_pClsMod->cAliases) {
        return E_INVALID_OPERATION;
    }

    if (pTypeDype->type != Type_alias) {
        return E_INVALID_ARGUMENT;
    }

    if (!m_pTypeAliasList) {
        ECode ec = InitOrgType();
        if (FAILED(ec)) {
            return ec;
        }
    }

//    while (pTypeDype->type == Type_alias) {
//        pTypeDype = &m_pClsMod->ppAliasDir[pTypeDype->sIndex]->type;
//    }

    if (pTypeDype->sIndex >= m_pClsMod->cAliases) {
        return E_INVALID_ARGUMENT;
    }

    *ppOrgTypeDesc = m_pTypeAliasList[pTypeDype->sIndex].pOrgTypeDesc;
    return NOERROR;
}
