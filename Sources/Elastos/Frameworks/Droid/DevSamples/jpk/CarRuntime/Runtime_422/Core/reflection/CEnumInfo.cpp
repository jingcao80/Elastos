//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#include "CEnumInfo.h"
#include "CEnumItemInfo.h"

UInt32 CEnumRefInfo::AddRef()
{
    Int32 nRef = atomic_inc(&m_cRef);
    return (UInt32)nRef;
}

UInt32 CEnumRefInfo::Release()
{
    g_objInfoList.LockHashTable(EntryType_Enum);
    Int32 nRef = atomic_dec(&m_cRef);

    if (0 == nRef) {
        if (!m_pCClsModule) {
            g_objInfoList.DetachEnumInfo(this);
        }
        else {
            g_objInfoList.RemoveEnumInfo(m_pEnumDirEntry);
        }
        delete this;
    }
    g_objInfoList.UnlockHashTable(EntryType_Enum);
    assert(nRef >= 0);
    return nRef;
}

PInterface CEnumRefInfo::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)this;
    }
    else if (riid == EIID_IEnumInfo) {
        return (IEnumInfo *)this;
    }
    else if (riid == EIID_IDataTypeInfo) {
        return (IDataTypeInfo *)this;
    }
    else {
        return NULL;
    }
}

ECode CEnumRefInfo::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    return E_NOT_IMPLEMENTED;
}

CEnumRefInfo::CEnumRefInfo()
{
    m_pCClsModule = NULL;
    m_pName = NULL;
    m_pItemNames = NULL;
    m_pItemValues = NULL;
    m_pItemInfos = NULL;
    m_pBuf = NULL;
    m_cRef = 0;
}

CEnumRefInfo::~CEnumRefInfo()
{
    if (m_pCClsModule) m_pCClsModule->Release();
    if (m_pBuf) delete m_pBuf;
    if (m_pItemNames) BufferOf<CString>::Free(m_pItemNames);
    if (m_pItemValues) BufferOf<Int32>::Free(m_pItemValues);

    if (m_pItemInfos) {
        for (Int32 i = 0; i < m_pItemInfos->GetUsed(); i++) {
            if ((*m_pItemInfos)[i]) delete (*m_pItemInfos)[i];
        }
        BufferOf<IEnumItemInfo *>::Free(m_pItemInfos);
    }
}

ECode CEnumRefInfo::InitStatic(
    /* [in] */ CClsModule * pCClsModule,
    /* [in] */ EnumDirEntry *pEnumDirEntry)
{
    m_pCClsModule = pCClsModule;
    m_pCClsModule->AddRef();

	Int32 nBase = m_pCClsModule->m_nBase;
    m_pEnumDirEntry = pEnumDirEntry;
    m_pName = adjustNameAddr(nBase, m_pEnumDirEntry->pszName);
    EnumDescriptor* pDesc = adjustEnumDescAddr(nBase, pEnumDirEntry->pDesc);

    m_pItemValues = BufferOf<Int32>::Alloc(pDesc->cElems);
    if (!m_pItemValues) {
        return E_OUT_OF_MEMORY;
    }

    m_pItemNames = BufferOf<CString>::Alloc(pDesc->cElems);
    if (!m_pItemNames) {
        BufferOf<Int32>::Free(m_pItemValues);
        m_pItemValues = NULL;
        return E_OUT_OF_MEMORY;
    }

    memset(m_pItemValues->GetPayload(), 0, sizeof(Int32) * pDesc->cElems);
    memset(m_pItemNames->GetPayload(), 0, sizeof(CString) * pDesc->cElems);
    m_pItemValues->SetUsed(pDesc->cElems);
    m_pItemNames->SetUsed(pDesc->cElems);

    EnumElement* pElem = NULL;
    for (int i = 0; i < pDesc->cElems; i++) {
        pElem = getEnumElementAddr(nBase, pDesc->ppElems, i);
        (*m_pItemValues)[i] = pElem->nValue;
        (*m_pItemNames)[i] = adjustNameAddr(nBase, pElem->pszName);
    }

    ECode ec = InitItemInfos();
    if (FAILED(ec)) {
        BufferOf<CString>::Free(m_pItemNames);
        m_pItemNames = NULL;
        BufferOf<Int32>::Free(m_pItemValues);
        m_pItemValues = NULL;
    }

    return NOERROR;
}

ECode CEnumRefInfo::InitDynamic(
    /* [in] */ CString name,
    /* [in] */ const BufferOf<CString>& itemNames,
    /* [in] */ const BufferOf<Int32>& itemValues)
{
    ECode ec = NOERROR;

    //Get the totle size;
    int i, size = StringAlignSize(name);

    for (i = 0; i < itemNames.GetUsed(); i++) {
        size += StringAlignSize(itemNames[i]);
    }

    m_pBuf = (char *)malloc(size);
    if (!m_pBuf) {
        ec = E_OUT_OF_MEMORY;
        goto EExit;
    }

    //
    m_pItemNames = BufferOf<CString>::Alloc(itemNames.GetUsed());
    if (!m_pItemNames) {
        ec = E_OUT_OF_MEMORY;
        goto EExit;
    }

    size = 0;
    m_pName = m_pBuf;
    strcpy(m_pName, name);
    size = StringAlignSize(name);

    m_pItemNames->SetUsed(itemNames.GetUsed());
    for (i = 0; i < itemNames.GetUsed(); i++) {
        (*m_pItemNames)[i] = m_pBuf + size;
        strcpy((char *)(const char *)(*m_pItemNames)[i], itemNames[i]);
        size += StringAlignSize(itemNames[i]);
    }

    //
    m_pItemValues = itemValues.Clone();
    if (!m_pItemValues) {
        ec = E_OUT_OF_MEMORY;
        goto EExit;
    }

    ec = InitItemInfos();
    if (FAILED(ec)) goto EExit;

    return NOERROR;

EExit:
    if (m_pBuf) {
        delete m_pBuf;
        m_pBuf = NULL;
    }

    if (m_pItemNames) {
        BufferOf<CString>::Free(m_pItemNames);
        m_pItemNames = NULL;
    }

    if (m_pItemValues) {
        BufferOf<Int32>::Free(m_pItemValues);
        m_pItemValues = NULL;
    }

    return ec;
}

ECode CEnumRefInfo::GetSize(
    /* [out] */ MemorySize * pSize)
{
    if (!pSize) {
        return E_INVALID_ARGUMENT;
    }

    *pSize = sizeof(Int32);
    return NOERROR;
}

ECode CEnumRefInfo::GetDataType(
    /* [out] */ CarDataType * pDataType)
{
    if (!pDataType) {
        return E_INVALID_ARGUMENT;
    }

    *pDataType = CarDataType_Enum;
    return NOERROR;
}

ECode CEnumRefInfo::GetName(
    /* [out] */ StringBuf * pName)
{
    if (pName == NULL || !pName->GetCapacity()) {
        return E_INVALID_ARGUMENT;
    }

    pName->Copy(m_pName);
    return NOERROR;
}

ECode CEnumRefInfo::GetModuleInfo(
    /* [out] */ IModuleInfo ** ppModuleInfo)
{
    if (m_pCClsModule) {
        return m_pCClsModule->GetModuleInfo(ppModuleInfo);
    }
    else {
        return E_INVALID_OPERATION;
    }
}

ECode CEnumRefInfo::GetItemCount(
    /* [out] */ Int32 * pCount)
{
    if (!pCount) {
        return E_INVALID_ARGUMENT;
    }

    *pCount = m_pItemValues->GetUsed();
    return NOERROR;
}

ECode CEnumRefInfo::InitItemInfos()
{
    Int32 i = 0, count = m_pItemValues->GetUsed();
    m_pItemInfos = BufferOf<IEnumItemInfo *>::Alloc(count);
    if (m_pItemInfos == NULL) {
        return E_OUT_OF_MEMORY;
    }
    memset(m_pItemInfos->GetPayload(), 0, count * sizeof(PInterface));
    m_pItemInfos->SetUsed(count);

    for (i = 0; i < count; i++) {
        (*m_pItemInfos)[i] = new CEnumItemInfo(this,
                (*m_pItemNames)[i], (*m_pItemValues)[i]);
        if (!(*m_pItemInfos)[i]) goto EExit;
    }

    return NOERROR;

EExit:
    for (i = 0; i < count; i++) {
        if ((*m_pItemInfos)[i]) delete (*m_pItemInfos)[i];
    }
    BufferOf<IEnumItemInfo *>::Free(m_pItemInfos);
    m_pItemInfos = NULL;

    return NOERROR;
}

ECode CEnumRefInfo::GetAllItemInfos(
    /* [out] */ BufferOf<IEnumItemInfo *> * pItemInfos)
{
    if (!pItemInfos || !pItemInfos->GetCapacity()) {
        return E_INVALID_ARGUMENT;
    }

    pItemInfos->Copy(m_pItemInfos);
    for (Int32 i = 0; i < pItemInfos->GetUsed(); i++) {
        (*pItemInfos)[i]->AddRef();
    }

    return NOERROR;
}

ECode CEnumRefInfo::GetItemInfo(
    /* [in] */ CString name,
    /* [out] */ IEnumItemInfo ** ppEnumItemInfo)
{
    if (name.IsNull() || !ppEnumItemInfo) {
        return E_INVALID_ARGUMENT;
    }

    for (Int32 i = 0; i < m_pItemInfos->GetUsed(); i++) {
        if (name.Compare((*m_pItemNames)[i]) == 0) {
            (*m_pItemInfos)[i]->AddRef();
            *ppEnumItemInfo = (IEnumItemInfo *)(*m_pItemInfos)[i];
            return NOERROR;
        }
    }

    return E_DOES_NOT_EXIST;
}
