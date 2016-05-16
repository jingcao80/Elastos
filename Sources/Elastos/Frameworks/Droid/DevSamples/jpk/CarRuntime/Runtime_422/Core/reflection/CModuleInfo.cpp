//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================
#include "CModuleInfo.h"
#include "_pubcrt.h"

UInt32 CModuleRefInfo::AddRef()
{
    Int32 nRef = atomic_inc(&m_cRef);
    return (UInt32)nRef;
}

UInt32 CModuleRefInfo::Release()
{
    g_objInfoList.LockHashTable(EntryType_Module);
    Int32 nRef = atomic_dec(&m_cRef);

    if (0 == nRef) {
        g_objInfoList.RemoveModuleInfo(m_sbPath);
        delete this;
    }
    g_objInfoList.UnlockHashTable(EntryType_Module);
    assert(nRef >= 0);
    return nRef;
}

PInterface CModuleRefInfo::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)this;
    }
    else if (riid == EIID_IModuleInfo) {
        return (IModuleInfo *)this;
    }
    else {
        return NULL;
    }
}

ECode CModuleRefInfo::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    return E_NOT_IMPLEMENTED;
}

CModuleRefInfo::CModuleRefInfo(
    /* [in] */ CClsModule * pCClsModule,
    /* [in] */ CString path)
{
    m_pCClsModule = pCClsModule;
    m_pCClsModule->AddRef();
    m_pClsMod = pCClsModule->m_pClsMod;
    m_sbPath.Copy(path);

    m_pClassList = NULL;
    m_pInterfaceList = NULL;
    m_pStructList = NULL;
    m_pEnumList = NULL;
    m_pAliasList = NULL;
    m_pConstantList = NULL;

    m_iAliasCount = 0;
    m_cRef = 0;
}

CModuleRefInfo::~CModuleRefInfo()
{
    if (m_pCClsModule) m_pCClsModule->Release();

    if (m_pClassList) m_pClassList->Release();
    if (m_pInterfaceList) m_pInterfaceList->Release();
    if (m_pStructList) m_pStructList->Release();
    if (m_pEnumList) m_pEnumList->Release();
    if (m_pAliasList) m_pAliasList->Release();
    if (m_pConstantList) m_pConstantList->Release();
}

ECode CModuleRefInfo::GetPath(
    /* [out] */ StringBuf * pPath)
{
    pPath->Copy((const char*)m_sbPath);
    return NOERROR;
}

ECode CModuleRefInfo::GetVersion(
    /* [out] */ Int32 * pMajor,
    /* [out] */ Int32 * pMinor,
    /* [out] */ Int32 * pBuild,
    /* [out] */ Int32 * pRevision)
{
    *pMajor = m_pClsMod->cMajorVersion;
    *pMinor = m_pClsMod->cMinorVersion;
    return NOERROR;
}

ECode CModuleRefInfo::GetClassCount(
    /* [out] */ Int32 * pCount)
{
    if (!pCount) {
        return E_INVALID_ARGUMENT;
    }

    *pCount = m_pClsMod->cClasses;
    return NOERROR;
}

ECode CModuleRefInfo::AcquireClassList()
{
    ECode ec = NOERROR;
    g_objInfoList.LockHashTable(EntryType_Class);
    if (!m_pClassList) {
        m_pClassList = new CEntryList(EntryType_Class,
            m_pClsMod, m_pClsMod->cClasses, m_pCClsModule);
        if (m_pClassList)
            m_pClassList->AddRef();
        else
            ec = E_OUT_OF_MEMORY;
    }
    g_objInfoList.UnlockHashTable(EntryType_Class);

    return ec;
}

ECode CModuleRefInfo::GetAllClassInfos(
    /* [out] */ BufferOf<IClassInfo *> * pClassInfos)
{
    ECode ec = AcquireClassList();
    if (FAILED(ec)) return ec;

    return m_pClassList->GetAllObjInfos((PTypeInfos)pClassInfos);
}

ECode CModuleRefInfo::GetClassInfo(
    /* [in] */ CString name,
    /* [out] */ IClassInfo ** ppClassInfo)
{
    if (name.IsNull() || NULL == ppClassInfo) {
        return E_INVALID_ARGUMENT;
    }

    ECode ec = AcquireClassList();
    if (FAILED(ec)) return ec;

    return m_pClassList->AcquireObjByName(name, (IInterface **)ppClassInfo);
}

ECode CModuleRefInfo::GetInterfaceCount(
    /* [out] */ Int32 * pCount)
{
    if (!pCount) {
        return E_INVALID_ARGUMENT;
    }

    *pCount = m_pClsMod->cInterfaces;
    return NOERROR;
}

ECode CModuleRefInfo::AcquireInterfaceList()
{
    ECode ec = NOERROR;
    g_objInfoList.LockHashTable(EntryType_Interface);
    if (!m_pInterfaceList) {
        m_pInterfaceList = new CEntryList(EntryType_Interface,
            m_pClsMod, m_pClsMod->cInterfaces, m_pCClsModule);
        if (m_pInterfaceList)
            m_pInterfaceList->AddRef();
        else
            ec = E_OUT_OF_MEMORY;
    }
    g_objInfoList.UnlockHashTable(EntryType_Interface);

    return ec;
}

ECode CModuleRefInfo::GetAllInterfaceInfos(
    /* [out] */ BufferOf<IInterfaceInfo *> * pInterfaceInfos)
{
    ECode ec = AcquireInterfaceList();
    if (FAILED(ec)) return ec;

    return m_pInterfaceList->GetAllObjInfos((PTypeInfos)pInterfaceInfos);
}

ECode CModuleRefInfo::GetInterfaceInfo(
    /* [in] */ CString name,
    /* [out] */ IInterfaceInfo ** ppInterfaceInfo)
{
    if (name.IsNull() || NULL == ppInterfaceInfo) {
        return E_INVALID_ARGUMENT;
    }

    ECode ec = AcquireInterfaceList();
    if (FAILED(ec)) return ec;

    return m_pInterfaceList->AcquireObjByName(
        name, (IInterface **)ppInterfaceInfo);
}

ECode CModuleRefInfo::GetStructCount(
    /* [out] */ Int32 * pCount)
{
    if (!pCount) {
        return E_INVALID_ARGUMENT;
    }

    *pCount = m_pClsMod->cStructs;
    return NOERROR;
}

ECode CModuleRefInfo::AcquireStructList()
{
    ECode ec = NOERROR;
    g_objInfoList.LockHashTable(EntryType_Struct);
    if (!m_pStructList) {
        m_pStructList = new CEntryList(EntryType_Struct,
            m_pClsMod, m_pClsMod->cStructs, m_pCClsModule);
        if (m_pStructList)
            m_pStructList->AddRef();
        else
            ec = E_OUT_OF_MEMORY;
    }
    g_objInfoList.UnlockHashTable(EntryType_Struct);

    return ec;
}

ECode CModuleRefInfo::GetAllStructInfos(
    /* [out] */ BufferOf<IStructInfo *> * pStructInfos)
{
    ECode ec = AcquireStructList();
    if (FAILED(ec)) return ec;

    return m_pStructList->GetAllObjInfos((PTypeInfos)pStructInfos);
}

ECode CModuleRefInfo::GetStructInfo(
    /* [in] */ CString name,
    /* [out] */ IStructInfo ** ppStructInfo)
{
    if (name.IsNull() || !ppStructInfo) {
        return E_INVALID_ARGUMENT;
    }

    if (!m_pClsMod->cStructs) {
        return E_DOES_NOT_EXIST;
    }

    ECode ec = AcquireStructList();
    if (FAILED(ec)) return ec;

    return m_pStructList->AcquireObjByName(name, (IInterface **)ppStructInfo);
}

ECode CModuleRefInfo::GetEnumCount(
    /* [out] */ Int32 * pCount)
{
    if (!pCount) {
        return E_INVALID_ARGUMENT;
    }

    *pCount = m_pClsMod->cEnums;
    return NOERROR;
}

ECode CModuleRefInfo::AcquireEnumList()
{
    ECode ec = NOERROR;
    g_objInfoList.LockHashTable(EntryType_Enum);
    if (!m_pEnumList) {
        m_pEnumList = new CEntryList(EntryType_Enum,
            m_pClsMod, m_pClsMod->cEnums, m_pCClsModule);
        if (m_pEnumList)
            m_pEnumList->AddRef();
        else
            ec = E_OUT_OF_MEMORY;
    }
    g_objInfoList.UnlockHashTable(EntryType_Enum);

    return ec;
}

ECode CModuleRefInfo::GetAllEnumInfos(
    /* [out] */ BufferOf<IEnumInfo *> * pEnumInfos)
{
    ECode ec = AcquireEnumList();
    if (FAILED(ec)) return ec;

    return m_pEnumList->GetAllObjInfos((PTypeInfos)pEnumInfos);
}

ECode CModuleRefInfo::GetEnumInfo(
    /* [in] */ CString name,
    /* [out] */ IEnumInfo ** ppEnumInfo)
{
    if (name.IsNull() || !ppEnumInfo) {
        return E_INVALID_ARGUMENT;
    }

    if (!m_pClsMod->cEnums) {
        return E_DOES_NOT_EXIST;
    }

    ECode ec = AcquireEnumList();
    if (FAILED(ec)) return ec;

    return m_pEnumList->AcquireObjByName(name, (IInterface **)ppEnumInfo);
}

ECode CModuleRefInfo::GetTypeAliasCount(
    /* [out] */ Int32 * pCount)
{
    if (!pCount) {
        return E_INVALID_ARGUMENT;
    }

    ECode ec = AcquireAliasList();
    if (FAILED(ec)) return ec;

    *pCount = m_iAliasCount;
    return NOERROR;
}

ECode CModuleRefInfo::AcquireAliasList()
{
    ECode ec = NOERROR;
    g_objInfoList.LockHashTable(EntryType_TypeAliase);
    if (!m_pAliasList) {

        m_iAliasCount = 0;
        for (Int32 i = 0; i < m_pClsMod->cAliases; i++) {
            if (!IsSysAlaisType(m_pCClsModule, i)) m_iAliasCount++;
        }

        m_pAliasList = new CEntryList(EntryType_TypeAliase,
            m_pClsMod, m_iAliasCount, m_pCClsModule);
        if (m_pAliasList)
            m_pAliasList->AddRef();
        else
            ec = E_OUT_OF_MEMORY;
    }
    g_objInfoList.UnlockHashTable(EntryType_TypeAliase);

    return ec;
    return NOERROR;
}

ECode CModuleRefInfo::GetAllTypeAliasInfos(
    /* [out] */ BufferOf<ITypeAliasInfo *> * pTypeAliasInfos)
{
    ECode ec = AcquireAliasList();
    if (FAILED(ec)) return ec;

    return m_pAliasList->GetAllObjInfos((PTypeInfos)pTypeAliasInfos);
}

ECode CModuleRefInfo::GetTypeAliasInfo(
    /* [in] */ CString name,
    /* [out] */ ITypeAliasInfo ** ppTypeAliasInfo)
{
    if (name.IsNull() || !ppTypeAliasInfo) {
        return E_INVALID_ARGUMENT;
    }

    if (!m_pClsMod->cAliases) {
        return E_DOES_NOT_EXIST;
    }

    ECode ec = AcquireAliasList();
    if (FAILED(ec)) return ec;

    return m_pAliasList->AcquireObjByName(name, (IInterface **)ppTypeAliasInfo);
}

ECode CModuleRefInfo::GetConstantCount(
    /* [out] */ Int32 * pCount)
{
    if (!pCount) {
        return E_INVALID_ARGUMENT;
    }

    *pCount = m_pClsMod->cConsts;
    return NOERROR;
}

ECode CModuleRefInfo::AcquireConstantList()
{
    ECode ec = NOERROR;
    g_objInfoList.LockHashTable(EntryType_Constant);
    if (!m_pConstantList) {
        m_pConstantList = new CEntryList(EntryType_Constant,
            m_pClsMod, m_pClsMod->cConsts, m_pCClsModule);
        if (m_pConstantList)
            m_pConstantList->AddRef();
        else
            ec = E_OUT_OF_MEMORY;
    }
    g_objInfoList.UnlockHashTable(EntryType_Constant);

    return ec;
}

ECode CModuleRefInfo::GetAllConstantInfos(
    /* [out] */ BufferOf<IConstantInfo *> * pConstantInfos)
{
    ECode ec = AcquireConstantList();
    if (FAILED(ec)) return ec;

    return m_pConstantList->GetAllObjInfos((PTypeInfos)pConstantInfos);
}

ECode CModuleRefInfo::GetConstantInfo(
    /* [in] */ CString name,
    /* [out] */ IConstantInfo ** ppConstantInfo)
{
    if (name.IsNull() || !ppConstantInfo) {
        return E_INVALID_ARGUMENT;
    }

    if (!m_pClsMod->cConsts) {
        return E_DOES_NOT_EXIST;
    }

    ECode ec = AcquireConstantList();
    if (FAILED(ec)) return ec;

    return m_pConstantList->AcquireObjByName(name, (IInterface **)ppConstantInfo);
}

ECode CModuleRefInfo::GetImportModuleInfoCount(
    /* [out] */ Int32 * pCount)
{
    if (!pCount) {
        return E_INVALID_ARGUMENT;
    }

    *pCount = m_pClsMod->cLibraries;
    return NOERROR;
}

ECode CModuleRefInfo::GetAllImportModuleInfos(
    /* [out] */ BufferOf<IModuleInfo *> * pModuleInfos)
{
    if (!pModuleInfos) {
        return E_INVALID_ARGUMENT;
    }

    Int32 nCapacity = pModuleInfos->GetCapacity();
    if (!nCapacity) {
        return E_INVALID_ARGUMENT;
    }

    Int32 nTotalCount = m_pClsMod->cLibraries;
    if (!nTotalCount) {
        pModuleInfos->SetUsed(0);
        return NOERROR;
    }

    Int32 nCount = nCapacity < nTotalCount ? nCapacity : nTotalCount;
    IInterface *pObject = NULL;
    ECode ec = NOERROR;
    for (Int32 i = 0; i < nCount; i++) {
        StringBuf_<256> libNames;
        libNames.Copy(getLibNameAddr(m_pCClsModule->m_nBase, m_pClsMod->ppLibNames, i));
        ec = g_objInfoList.AcquireModuleInfo(libNames, (IModuleInfo**)&pObject);
        if (FAILED(ec)) return ec;
        pModuleInfos->SetUsed(i + 1);
        (*pModuleInfos)[i] = (IModuleInfo*)pObject;
    }

    return NOERROR;

}
