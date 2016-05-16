//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#include "CInterfaceInfo.h"

UInt32 CInterfaceInfo::AddRef()
{
    Int32 nRef = atomic_inc(&m_cRef);
    return (UInt32)nRef;
}

UInt32 CInterfaceInfo::Release()
{
    g_objInfoList.LockHashTable(EntryType_Interface);
    Int32 nRef = atomic_dec(&m_cRef);

    if (0 == nRef) {
        g_objInfoList.RemoveInterfaceInfo(m_pDesc->iid);
        delete this;
    }
    g_objInfoList.UnlockHashTable(EntryType_Interface);
    assert(nRef >= 0);
    return nRef;
}

PInterface CInterfaceInfo::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)this;
    }
    else if (riid == EIID_IInterfaceInfo) {
        return (IInterfaceInfo *)this;
    }
    else if (riid == EIID_IDataTypeInfo) {
        return (IDataTypeInfo *)this;
    }
    else {
        return NULL;
    }
}

ECode CInterfaceInfo::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    return E_NOT_IMPLEMENTED;
}

CInterfaceInfo::CInterfaceInfo(
    /* [in] */ CClsModule * pCClsModule,
    /* [in] */ UInt32 uIndex)
{
    m_pCClsModule = pCClsModule;
    m_pCClsModule->AddRef();
    m_pClsMod = m_pCClsModule->m_pClsMod;
    m_nBase =  m_pCClsModule->m_nBase;
    m_pInterfaceDirEntry =
            getInterfaceDirAddr(m_nBase, m_pClsMod->ppInterfaceDir, uIndex);
    m_pDesc = adjustInterfaceDescAddr(m_nBase, m_pInterfaceDirEntry->pDesc);
    m_uIndex = uIndex;
    m_pIFList = NULL;
    m_pMethodList = NULL;
    m_cRef = 0;
}

CInterfaceInfo::~CInterfaceInfo()
{
    if (m_pCClsModule) m_pCClsModule->Release();
    if (m_pMethodList) m_pMethodList->Release();
    if (m_pIFList) delete[] m_pIFList;
}

ECode CInterfaceInfo::Init()
{
    ECode ec = CreateIFList();
    if (FAILED(ec)) return ec;

    return NOERROR;
}

ECode CInterfaceInfo::GetSize(
    /* [out] */ MemorySize * pSize)
{
    if (!pSize) {
        return E_INVALID_ARGUMENT;
    }

    *pSize = sizeof(IInterface *);
    return NOERROR;
}

ECode CInterfaceInfo::GetDataType(
    /* [out] */ CarDataType * pDataType)
{
    if (!pDataType) {
        return E_INVALID_ARGUMENT;
    }

    *pDataType = CarDataType_Interface;
    return NOERROR;
}

ECode CInterfaceInfo::GetName(
    /* [out] */ StringBuf * pName)
{
    if (pName == NULL || !pName->GetCapacity()) {
        return E_INVALID_ARGUMENT;
    }

    pName->Copy(adjustNameAddr(m_nBase, m_pInterfaceDirEntry->pszName));
    return NOERROR;
}

ECode CInterfaceInfo::GetId(
    /* [out] */ InterfaceID * pId)
{
    if (!pId) {
        return E_INVALID_ARGUMENT;
    }

    *pId = m_pDesc->iid;
    return NOERROR;
}

ECode CInterfaceInfo::GetModuleInfo(
    /* [out] */ IModuleInfo ** ppModuleInfo)
{
    return m_pCClsModule->GetModuleInfo(ppModuleInfo);
}

ECode CInterfaceInfo::IsLocal(
    /* [out] */ Boolean * pLocal)
{
    if (!pLocal) {
        return E_INVALID_ARGUMENT;
    }


    if (m_pDesc->dwAttribs & InterfaceAttrib_local) {
        *pLocal = TRUE;
    }
    else {
        *pLocal = FALSE;
    }

    return NOERROR;
}

ECode CInterfaceInfo::HasBase(
    /* [out] */ Boolean * pHasBase)
{
    if (!pHasBase) {
        return E_INVALID_ARGUMENT;
    }

    if (m_uIndex != m_pDesc->sParentIndex) {
        *pHasBase = TRUE;
    }
    else {
        *pHasBase = FALSE;
    }

    return NOERROR;
}

ECode CInterfaceInfo::GetBaseInfo(
    /* [out] */ IInterfaceInfo ** ppBaseInfo)
{
    if (!ppBaseInfo) {
        return E_INVALID_ARGUMENT;
    }

    *ppBaseInfo = NULL;
    UInt32 uIndex = m_pDesc->sParentIndex;
    return g_objInfoList.AcquireInterfaceInfo(m_pCClsModule, uIndex,
                        (IInterface**)ppBaseInfo);
}

ECode CInterfaceInfo::GetMethodCount(
    /* [out] */ Int32 * pCount)
{
    if (!pCount) {
        return E_INVALID_ARGUMENT;
    }

    if (!m_pIFList) {
        ECode ec = CreateIFList();
        if (FAILED(ec)) return ec;
    }

    *pCount = 0;
    for (UInt32 i = 0; i < m_uIFCount; i++) {
        *pCount += m_pIFList[i].pDesc->cMethods;
    }

    return NOERROR;
}

ECode CInterfaceInfo::AcquireMethodList()
{
    ECode ec = NOERROR;
    g_objInfoList.LockHashTable(EntryType_Method);
    if (!m_pMethodList) {
        m_pMethodList = new CEntryList(EntryType_Method,
            m_pDesc, m_uMethodCount, m_pCClsModule, m_pIFList, m_uIFCount);
        if (m_pMethodList)
            m_pMethodList->AddRef();
        else
            ec = E_OUT_OF_MEMORY;
    }
    g_objInfoList.UnlockHashTable(EntryType_Method);

    return ec;

    return NOERROR;
}

ECode CInterfaceInfo::GetAllMethodInfos(
    /* [out] */ BufferOf<IMethodInfo *> * pMethodInfos)
{
    ECode ec = AcquireMethodList();
    if (FAILED(ec)) return ec;

    return m_pMethodList->GetAllObjInfos((PTypeInfos)pMethodInfos);
}

ECode CInterfaceInfo::GetMethodInfo(
    /* [in] */ CString name,
    /* [out] */ IMethodInfo ** ppMethodInfo)
{
    if (name.IsNull() || !ppMethodInfo) {
        return E_INVALID_ARGUMENT;
    }

    assert(m_uMethodCount);

    ECode ec = AcquireMethodList();
    if (FAILED(ec)) return ec;

    return m_pMethodList->AcquireObjByName(name, (IInterface **)ppMethodInfo);
}

ECode CInterfaceInfo::CreateIFList()
{
    if (m_pIFList) {
        return NOERROR;
    }

    UInt32 *pIndexList = (UInt32 *)alloca(
        m_pClsMod->cInterfaces * sizeof(UInt32));
    if (pIndexList == NULL) {
        return E_OUT_OF_MEMORY;
    }

    int i, j = 0;
    m_uIFCount = 0;
    UInt32 uIndex = m_uIndex;
    InterfaceDirEntry* pIFDir = NULL;
    while (uIndex != 0) {
        pIndexList[m_uIFCount++] = uIndex;
        pIFDir = getInterfaceDirAddr(m_nBase, m_pClsMod->ppInterfaceDir, uIndex);
        uIndex = adjustInterfaceDescAddr(m_nBase, pIFDir->pDesc)->sParentIndex;
    }

    pIndexList[m_uIFCount] = 0;
    m_uIFCount++;

    UInt32 uBeginNo = METHOD_START_NO;
    m_pIFList = new IFIndexEntry[m_uIFCount];
    if (m_pIFList == NULL) {
        return E_OUT_OF_MEMORY;
    }

    for (i = m_uIFCount - 1, j = 0; i >= 0; i--, j++) {
        uIndex = pIndexList[i];
        m_pIFList[j].uIndex = uIndex;
        m_pIFList[j].uBeginNo = uBeginNo;
        pIFDir = getInterfaceDirAddr(m_nBase, m_pClsMod->ppInterfaceDir, uIndex);
        m_pIFList[j].pszName = adjustNameAddr(m_nBase, pIFDir->pszName);
        m_pIFList[j].pDesc = adjustInterfaceDescAddr(m_nBase, pIFDir->pDesc);
        uBeginNo += m_pIFList[j].pDesc->cMethods;
    }

    m_uMethodCount = 0;
    for (i = 0; i < (int)m_uIFCount; i++) {
        m_uMethodCount += m_pIFList[i].pDesc->cMethods;
    }

    return NOERROR;
}
