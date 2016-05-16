//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#include "CObjInfoList.h"

#include "CModuleInfo.h"
#include "CClassInfo.h"
#include "CInterfaceInfo.h"
#include "CStructInfo.h"
#include "CEnumInfo.h"
#include "CTypeAliasInfo.h"
#include "CConstructorInfo.h"
#include "CIntrinsicInfo.h"
#include "CCppVectorInfo.h"
#include "CCarArrayInfo.h"
#include "CConstantInfo.h"
#include "CConstructorInfo.h"
#include "CLocalTypeInfo.h"
#include "CLocalPtrInfo.h"
#include "CStringBufInfo.h"
#include "CCallbackMethodInfo.h"
#include <pthread.h>
#include <dlfcn.h>

typedef
struct ModuleRsc {
    unsigned int    uSize;
    const char      uClsinfo[0];
} ModuleRsc;

int dlGetClassInfo(void *handle, void **pAddress, int *pSize)
{
    ModuleRsc** pRsc = NULL;

    assert(NULL != pAddress);
    assert(NULL != pSize);

    pRsc = (ModuleRsc **)dlsym(handle, "g_pDllResource");
    if (NULL == pRsc) {
        goto E_FAIL_EXIT;
    }

    *pAddress = (void*)&((*pRsc)->uClsinfo[0]);
    *pSize = (*pRsc)->uSize;
    return 0;

E_FAIL_EXIT:
    return -1;
}

UInt32 CObjInfoList::AddRef()
{
    Int32 nRef = atomic_inc(&m_cRef);
    return (UInt32)nRef;
}

UInt32 CObjInfoList::Release()
{
    Int32 nRef = atomic_dec(&m_cRef);

    if (0 == nRef) {
        delete this;
    }
    assert(nRef >= 0);
    return nRef;
}

CObjInfoList::CObjInfoList()
{
    pthread_mutexattr_t recursiveAttr;

    pthread_mutexattr_init(&recursiveAttr);
    pthread_mutexattr_settype(&recursiveAttr, PTHREAD_MUTEX_RECURSIVE);
    int ret = pthread_mutex_init(&m_lockTypeAlias, &recursiveAttr);
    if (ret) m_bLockTypeAlias = FALSE;
    else m_bLockTypeAlias = TRUE;

    ret = pthread_mutex_init(&m_lockEnum, &recursiveAttr);
    if (ret) m_bLockEnum = FALSE;
    else m_bLockEnum = TRUE;

    ret = pthread_mutex_init(&m_lockClass, &recursiveAttr);
    if (ret) m_bLockClass = FALSE;
    else m_bLockClass = TRUE;

    ret = pthread_mutex_init(&m_lockStruct, &recursiveAttr);
    if (ret) m_bLockStruct = FALSE;
    else m_bLockStruct = TRUE;

    ret = pthread_mutex_init(&m_lockMethod, &recursiveAttr);
    if (ret) m_bLockMethod = FALSE;
    else m_bLockMethod = TRUE;

    ret = pthread_mutex_init(&m_lockInterface, &recursiveAttr);
    if (ret) m_bLockInterface = FALSE;
    else m_bLockInterface = TRUE;

    ret = pthread_mutex_init(&m_lockModule, &recursiveAttr);
    if (ret) m_bLockModule = FALSE;
    else m_bLockModule = TRUE;

    ret = pthread_mutex_init(&m_lockDataType, &recursiveAttr);
    if (ret) m_bLockDataType = FALSE;
    else m_bLockDataType = TRUE;

    ret = pthread_mutex_init(&m_lockLocal, &recursiveAttr);
    if (ret) m_bLockLocal = FALSE;
    else m_bLockLocal = TRUE;

    ret = pthread_mutex_init(&m_lockClsModule, &recursiveAttr);
    if (ret) m_bLockClsModule = FALSE;
    else m_bLockClsModule = TRUE;

    pthread_mutexattr_destroy(&recursiveAttr);

    memset(m_pDataTypeInfos, 0, sizeof(IInterface *) * MAX_ITEM_COUNT);
    memset(m_pLocalTypeInfos, 0, sizeof(IInterface *) * MAX_ITEM_COUNT);
    memset(m_pCarArrayInfos, 0, sizeof(IInterface *) * MAX_ITEM_COUNT);

    m_pEnumInfoHead = NULL;
    m_pCCppVectorHead = NULL;
    m_pStructInfoHead = NULL;
    m_pCarArrayInfoHead = NULL;
    m_cRef = 0;
}

CObjInfoList::~CObjInfoList()
{
    for (int i = 0; i < MAX_ITEM_COUNT; i++) {
        if (m_pDataTypeInfos[i]) {
            m_pDataTypeInfos[i]->Release();
        }
        if (m_pLocalTypeInfos[i]) {
            m_pLocalTypeInfos[i]->Release();
        }
        if (m_pCarArrayInfos[i]) {
            m_pCarArrayInfos[i]->Release();
        }
    }

    m_hTypeAliasInfos.Clear();
    m_hEnumInfos.Clear();
    m_hClassInfos.Clear();
    m_hStructInfos.Clear();
    m_hMethodInfos.Clear();
    m_hIFInfos.Clear();
    m_hModInfos.Clear();
    m_hLocalPtrInfos.Clear();
    m_hClsModule.Clear();

    pthread_mutex_destroy(&m_lockTypeAlias);
    pthread_mutex_destroy(&m_lockEnum);
    pthread_mutex_destroy(&m_lockClass);
    pthread_mutex_destroy(&m_lockStruct);
    pthread_mutex_destroy(&m_lockMethod);
    pthread_mutex_destroy(&m_lockInterface);
    pthread_mutex_destroy(&m_lockModule);
    pthread_mutex_destroy(&m_lockDataType);
    pthread_mutex_destroy(&m_lockLocal);
    pthread_mutex_destroy(&m_lockClsModule);
}

ECode CObjInfoList::LockHashTable(
    /* [in] */ EntryType type)
{
    switch (type) {
        case EntryType_Module:
            if (m_bLockModule)
                pthread_mutex_lock(&m_lockModule);
            break;
        case EntryType_Aspect:
        case EntryType_Aggregatee:
        case EntryType_Class:
            if (m_bLockClass)
                pthread_mutex_lock(&m_lockClass);
            break;
        case EntryType_ClassInterface:
        case EntryType_Interface:
            if (m_bLockInterface)
                pthread_mutex_lock(&m_lockInterface);
            break;
        case EntryType_Struct:
            if (m_bLockStruct)
                pthread_mutex_lock(&m_lockStruct);
            break;
        case EntryType_Constant:
        case EntryType_Enum:
            if (m_bLockEnum)
                pthread_mutex_lock(&m_lockEnum);
            break;
        case EntryType_TypeAliase:
            if (m_bLockTypeAlias)
                pthread_mutex_lock(&m_lockTypeAlias);
            break;
        case EntryType_Constructor:
        case EntryType_CBMethod:
        case EntryType_Method:
            if (m_bLockMethod)
                pthread_mutex_lock(&m_lockMethod);
            break;
        case EntryType_DataType:
            if (m_bLockDataType)
                pthread_mutex_lock(&m_lockDataType);
            break;
        case EntryType_Local:
            if (m_bLockLocal)
                pthread_mutex_lock(&m_lockLocal);
            break;
        case EntryType_ClsModule:
            if (m_bLockClsModule)
                pthread_mutex_lock(&m_lockClsModule);
            break;

        default:
            break;
    }

    return NOERROR;
}

ECode CObjInfoList::UnlockHashTable(
    /* [in] */ EntryType type)
{
    switch (type) {
        case EntryType_Module:
            if (m_bLockModule)
                pthread_mutex_unlock(&m_lockModule);
            break;
        case EntryType_Aspect:
        case EntryType_Aggregatee:
        case EntryType_Class:
            if (m_bLockClass)
                pthread_mutex_unlock(&m_lockClass);
            break;
        case EntryType_ClassInterface:
        case EntryType_Interface:
            if (m_bLockInterface)
                pthread_mutex_unlock(&m_lockInterface);
            break;
        case EntryType_Struct:
            if (m_bLockStruct)
                pthread_mutex_unlock(&m_lockStruct);
            break;
        case EntryType_Constant:
        case EntryType_Enum:
            if (m_bLockEnum)
                pthread_mutex_unlock(&m_lockEnum);
            break;
        case EntryType_TypeAliase:
            if (m_bLockTypeAlias)
                pthread_mutex_unlock(&m_lockTypeAlias);
            break;
        case EntryType_Constructor:
        case EntryType_CBMethod:
        case EntryType_Method:
            if (m_bLockMethod)
                pthread_mutex_unlock(&m_lockMethod);
            break;
        case EntryType_DataType:
            if (m_bLockDataType)
                pthread_mutex_unlock(&m_lockDataType);
            break;
        case EntryType_Local:
            if (m_bLockLocal)
                pthread_mutex_unlock(&m_lockLocal);
            break;
        case EntryType_ClsModule:
            if (m_bLockClsModule)
                pthread_mutex_unlock(&m_lockClsModule);
            break;

        default:
            return E_INVALID_ARGUMENT;
    }

    return NOERROR;
}

ECode CObjInfoList::AcquireModuleInfo(
    /* [in] */ CString name,
    /* [out] */ IModuleInfo **ppModuleInfo)
{
    if (name.IsNull() || !ppModuleInfo) {
        return E_INVALID_ARGUMENT;
    }

    void *pIModule;
    ECode ec = NOERROR;

    pIModule = dlopen(name, RTLD_NOW);
    if(NULL == pIModule){
        return E_FILE_NOT_FOUND;
    }

    LockHashTable(EntryType_Module);
    IModuleInfo **ppModInfo = m_hModInfos.Get((char *)(const char*)name);
    if (ppModInfo) {
        dlclose(pIModule);
        (*ppModInfo)->AddRef();
        *ppModuleInfo = *ppModInfo;
        UnlockHashTable(EntryType_Module);
        return NOERROR;
    }

    CModuleRefInfo* pModInfo = NULL;
    CClsModule*  pCClsModule = NULL;

    LockHashTable(EntryType_ClsModule);

    CClsModule** ppCClsModule = NULL;
    ppCClsModule = m_hClsModule.Get((char *)(const char*)name);
    if (ppCClsModule) {
        pCClsModule = *ppCClsModule;
    }

    if (!pCClsModule) {
        ppCClsModule = NULL;
        //get cls module metadata
        MemorySize nSize;
        void *lpLockRes;
        CLSModule *pClsMod;
        Boolean bAllocedClsMod = FALSE;

        if (-1 == dlGetClassInfo(pIModule, &lpLockRes, &nSize)) {
            ec = E_DOES_NOT_EXIST;
            goto Exit;
        }

        if (((CLSModule *)lpLockRes)->dwAttribs & CARAttrib_compress) {
            if (RelocFlattedCLS((CLSModule *)lpLockRes, nSize, &pClsMod) < 0) {
                ec = E_OUT_OF_MEMORY;
                goto Exit;
            }
            bAllocedClsMod = TRUE;
        }
        else {
            pClsMod = (CLSModule *)lpLockRes;
        }

        pCClsModule = new CClsModule(pClsMod, bAllocedClsMod, name, pIModule);
        if (pCClsModule == NULL) {
            if (bAllocedClsMod) DisposeFlattedCLS(pClsMod);
            ec = E_OUT_OF_MEMORY;
            goto Exit;
        }

        if (!m_hClsModule.Put((char *)(const char*)name, (CClsModule**)&pCClsModule)) {
            delete pCClsModule;
            ec = E_OUT_OF_MEMORY;
            goto Exit;
        }
    }

    pModInfo = new CModuleRefInfo(pCClsModule, name);
    if (pModInfo == NULL) {
        if (!ppCClsModule && pCClsModule) delete pCClsModule;
        ec = E_OUT_OF_MEMORY;
        goto Exit;
    }

    if (!g_objInfoList.m_hModInfos.Put((char *)(const char*)name,
            (IModuleInfo**)&pModInfo)) {
        ec = E_OUT_OF_MEMORY;
        goto Exit;
    }

    pModInfo->AddRef();
    *ppModuleInfo = (IModuleInfo *)pModInfo;

    ec = NOERROR;

Exit:
    UnlockHashTable(EntryType_ClsModule);
    UnlockHashTable(EntryType_Module);

    if (FAILED(ec)) {
        if (pModInfo) delete pModInfo;
    }

    return ec;
}

ECode CObjInfoList:: RemoveModuleInfo(
    /* [in] */ CString path)
{
    if (path.IsNull()) {
        return E_INVALID_ARGUMENT;
    }
    m_hModInfos.Remove((PVoid)(const char *)path);
    return NOERROR;
}

ECode CObjInfoList:: RemoveClsModule(
    /* [in] */ CString path)
{
    if (path.IsNull()) {
        return E_INVALID_ARGUMENT;
    }
    m_hClsModule.Remove((PVoid)(const char *)path);
    return NOERROR;
}

ECode CObjInfoList::AcquireClassInfo(
    /* [in] */ CClsModule * pCClsModule,
    /* [in] */ ClassDirEntry *pClsDirEntry,
    /* [in, out] */ IInterface ** ppObject)
{
    if (!pCClsModule || !pClsDirEntry || !ppObject) {
        return E_INVALID_ARGUMENT;
    }

    LockHashTable(EntryType_Class);
    if (*ppObject) {
        UnlockHashTable(EntryType_Class);
        return NOERROR;
    }

    IInterface **ppObj = m_hClassInfos.Get(&pClsDirEntry);
    if (!ppObj) {
        CClassInfo *pClassInfo = new CClassInfo(pCClsModule, pClsDirEntry);
        if (pClassInfo == NULL) {
            UnlockHashTable(EntryType_Class);
            return E_OUT_OF_MEMORY;
        }

        ECode ec = pClassInfo->Init();
        if (FAILED(ec)) {
            delete pClassInfo;
            UnlockHashTable(EntryType_Class);
            return ec;
        }

        if (!m_hClassInfos.Put(&pClsDirEntry, (IInterface**)&pClassInfo)) {
            delete pClassInfo;
            UnlockHashTable(EntryType_Class);
            return E_OUT_OF_MEMORY;
        }

        *ppObject = pClassInfo;
    }
    else {
        *ppObject = *ppObj;
    }

    (*ppObject)->AddRef();
    UnlockHashTable(EntryType_Class);

    return NOERROR;
}

ECode CObjInfoList::RemoveClassInfo(
    /* [in] */ ClassDirEntry *pClsDirEntry)
{
    if (!pClsDirEntry) {
        return E_INVALID_ARGUMENT;
    }

    m_hClassInfos.Remove(&pClsDirEntry);
    return NOERROR;
}

ECode CObjInfoList::AcquireStaticStructInfo(
    /* [in] */ CClsModule * pCClsModule,
    /* [in] */ StructDirEntry *pStructDirEntry,
    /* [in, out] */ IInterface ** ppObject)
{
    if (!pCClsModule || !pStructDirEntry || !ppObject) {
        return E_INVALID_ARGUMENT;
    }

    LockHashTable(EntryType_Struct);
    if (*ppObject) {
        UnlockHashTable(EntryType_Struct);
        return NOERROR;
    }

    IInterface **ppObj = m_hStructInfos.Get(&pStructDirEntry);
    if (!ppObj) {
        CStructRefInfo *pStructInfo = new CStructRefInfo();
        if (pStructInfo == NULL) {
            UnlockHashTable(EntryType_Struct);
            return E_OUT_OF_MEMORY;
        }

        ECode ec = pStructInfo->InitStatic(pCClsModule, pStructDirEntry);
        if (FAILED(ec)) {
            delete pStructInfo;
            UnlockHashTable(EntryType_Struct);
            return ec;
        }

        if (!m_hStructInfos.Put(&pStructDirEntry, (IInterface**)&pStructInfo)) {
            delete pStructInfo;
            UnlockHashTable(EntryType_Struct);
            return E_OUT_OF_MEMORY;
        }

        *ppObject = pStructInfo;
    }
    else {
        *ppObject = *ppObj;
    }
    (*ppObject)->AddRef();

    UnlockHashTable(EntryType_Struct);

    return NOERROR;
}

ECode CObjInfoList::AcquireDynamicStructInfo(
    /* [in] */ CString name,
    /* [in] */ const BufferOf<CString>& fieldNames,
    /* [in] */ const BufferOf<IDataTypeInfo *>& fieldTypeInfos,
    /* [out] */ IStructInfo **ppStructInfo)
{
    if (name.IsNullOrEmpty() || fieldNames.IsNullOrEmpty()
        || fieldTypeInfos.IsNullOrEmpty() || !ppStructInfo
        || fieldNames.GetUsed() != fieldTypeInfos.GetUsed()) {
        return E_INVALID_ARGUMENT;
    }

    InfoLinkNode *pNode = m_pStructInfoHead;
    ECode ec = NOERROR;
    StringBuf_<_MAX_PATH> structName;
    CStructRefInfo *pStructInfo = NULL;
    Int32 count = 0, i = 0;
    BufferOf<CString>    *pFieldNames = NULL;
    BufferOf<IDataTypeInfo *>     *pFieldTypeInfos = NULL;
    LockHashTable(EntryType_Struct);
    for (; pNode; pNode = pNode->pNext) {
        pStructInfo = (CStructRefInfo *)pNode->pInfo;
        pStructInfo->GetName(&structName);

        if (name.Compare((char*)structName) == 0) {
            pStructInfo->GetFieldCount(&count);
            if (count != fieldNames.GetUsed()) {
                UnlockHashTable(EntryType_Struct);
                return E_DATAINFO_EXIST;
            }

            pFieldNames = pStructInfo->m_pFieldNames;
            pFieldTypeInfos = pStructInfo->m_pFieldTypeInfos;
            for (i = 0; i < count; i++) {
                if (fieldNames[i].Compare((*pFieldNames)[i])) {
                    UnlockHashTable(EntryType_Struct);
                    return E_DATAINFO_EXIST;
                }
                if ((*pFieldTypeInfos)[i] != fieldTypeInfos[i]) {
                    UnlockHashTable(EntryType_Struct);
                    return E_DATAINFO_EXIST;
                }
            }

            pStructInfo->AddRef();
            *ppStructInfo  = pStructInfo;
            UnlockHashTable(EntryType_Struct);
            return ec;
        }
    }

    pStructInfo = new CStructRefInfo();
    if (pStructInfo == NULL) {
        UnlockHashTable(EntryType_Struct);
        return E_OUT_OF_MEMORY;
    }

    ec = pStructInfo->InitDynamic(name, fieldNames, fieldTypeInfos);
    if (FAILED(ec)) {
        delete pStructInfo;
        UnlockHashTable(EntryType_Struct);
        return ec;
    }

    ec = AddInfoNode(pStructInfo, &m_pStructInfoHead);
    if (FAILED(ec)) {
        delete pStructInfo;
        UnlockHashTable(EntryType_Struct);
        return ec;
    }

    pStructInfo->AddRef();
    *ppStructInfo = pStructInfo;

    UnlockHashTable(EntryType_Struct);

    return NOERROR;
}

ECode CObjInfoList::RemoveStructInfo(
    /* [in] */ StructDirEntry *pStructDirEntry)
{
    if (!pStructDirEntry) {
        return E_INVALID_ARGUMENT;
    }

    m_hStructInfos.Remove(&pStructDirEntry);
    return NOERROR;
}

ECode CObjInfoList::AcquireStaticEnumInfo(
    /* [in] */ CClsModule * pCClsModule,
    /* [in] */ EnumDirEntry *pEnumDirEntry,
    /* [in, out] */ IInterface ** ppObject)
{
    if (!pCClsModule || !pEnumDirEntry || !ppObject) {
        return E_INVALID_ARGUMENT;
    }

    LockHashTable(EntryType_Enum);
    if (*ppObject) {
        UnlockHashTable(EntryType_Enum);
        return NOERROR;
    }

    IInterface **ppObj = m_hEnumInfos.Get(&pEnumDirEntry);
    if (!ppObj) {
        CEnumRefInfo *pEnumInfo = new CEnumRefInfo();
        if (pEnumInfo == NULL) {
            UnlockHashTable(EntryType_Enum);
            return E_OUT_OF_MEMORY;
        }

        ECode ec = pEnumInfo->InitStatic(pCClsModule, pEnumDirEntry);
        if (FAILED(ec)) {
            delete pEnumInfo;
            UnlockHashTable(EntryType_Enum);
            return ec;
        }

        if (!m_hEnumInfos.Put(&pEnumDirEntry, (IInterface**)&pEnumInfo)) {
            delete pEnumInfo;
            UnlockHashTable(EntryType_Enum);
            return E_OUT_OF_MEMORY;
        }

        *ppObject = pEnumInfo;
    }
    else {
        *ppObject = *ppObj;
    }
    (*ppObject)->AddRef();

    UnlockHashTable(EntryType_Enum);

    return NOERROR;
}

ECode CObjInfoList::RemoveEnumInfo(
    /* [in] */ EnumDirEntry *pEnumDirEntry)
{
    if (!pEnumDirEntry) {
        return E_INVALID_ARGUMENT;
    }

    m_hEnumInfos.Remove(&pEnumDirEntry);
    return NOERROR;
}

ECode CObjInfoList::AcquireDynamicEnumInfo(
    /* [in] */ CString name,
    /* [in] */ const BufferOf<CString>& itemNames,
    /* [in] */ const BufferOf<Int32>& itemValues,
    /* [out] */ IEnumInfo **ppEnumInfo)
{
    if (name.IsNull() || itemNames.IsNullOrEmpty()
        || itemValues.IsNullOrEmpty() || !ppEnumInfo
        || itemNames.GetUsed() != itemValues.GetUsed()) {
        return E_INVALID_ARGUMENT;
    }

    InfoLinkNode *pNode = m_pEnumInfoHead;
    ECode ec = NOERROR;
    StringBuf_<_MAX_PATH> enumName;
    CEnumRefInfo *pEnumInfo = NULL;
    Int32 count = 0, i = 0;
    BufferOf<CString>    *pItemNames;
    BufferOf<Int32>      *pItemValues;

    LockHashTable(EntryType_Enum);
    for (; pNode; pNode = pNode->pNext) {
        pEnumInfo = (CEnumRefInfo *)pNode->pInfo;
        pEnumInfo->GetName(&enumName);

        if (name.Compare((char*)enumName) == 0) {
            pEnumInfo->GetItemCount(&count);
            if (count != itemNames.GetUsed()) {
                if (!name.IsEmpty()) {
                    UnlockHashTable(EntryType_Enum);
                    return E_DATAINFO_EXIST;
                }
                else
                    continue;
            }

            pItemNames = pEnumInfo->m_pItemNames;
            pItemValues = pEnumInfo->m_pItemValues;
            for (i = 0; i < count; i++) {
                if (itemNames[i].Compare((*pItemNames)[i]) != 0) {
                    if (!name.IsEmpty()) {
                        UnlockHashTable(EntryType_Enum);
                        return E_DATAINFO_EXIST;
                    }
                    else
                        continue;
                }
                if (itemValues[i] != (*pItemValues)[i]) {
                    if (!name.IsEmpty()) {
                        UnlockHashTable(EntryType_Enum);
                        return E_DATAINFO_EXIST;
                    }
                    else
                        continue;
                }
            }

            pEnumInfo->AddRef();
            *ppEnumInfo  = pEnumInfo;
            return ec;
        }
    }

    pEnumInfo = new CEnumRefInfo();
    if (pEnumInfo == NULL) {
        UnlockHashTable(EntryType_Enum);
        return E_OUT_OF_MEMORY;
    }

    ec = pEnumInfo->InitDynamic(name, itemNames, itemValues);
    if (FAILED(ec)) {
        delete pEnumInfo;
        UnlockHashTable(EntryType_Enum);
        return ec;
    }

    ec = AddInfoNode(pEnumInfo, &m_pEnumInfoHead);
    if (FAILED(ec)) {
        delete pEnumInfo;
        UnlockHashTable(EntryType_Enum);
        return ec;
    }

    pEnumInfo->AddRef();
    *ppEnumInfo  = pEnumInfo;

    UnlockHashTable(EntryType_Enum);

    return NOERROR;
}

ECode CObjInfoList::AcquireTypeAliasInfo(
    /* [in] */ CClsModule * pCClsModule,
    /* [in] */ AliasDirEntry *pAliasDirEntry,
    /* [in, out] */ IInterface ** ppObject)
{
    if (!pCClsModule || !pAliasDirEntry || !ppObject) {
        return E_INVALID_ARGUMENT;
    }

    LockHashTable(EntryType_TypeAliase);
    if (*ppObject) {
        UnlockHashTable(EntryType_TypeAliase);
        return NOERROR;
    }

    IInterface **ppObj = m_hTypeAliasInfos.Get(&pAliasDirEntry);
    if (!ppObj) {
        CTypeAliasInfo *pAliasInfo = new CTypeAliasInfo(pCClsModule,
            pAliasDirEntry);
        if (pAliasInfo == NULL) {
            UnlockHashTable(EntryType_TypeAliase);
            return E_OUT_OF_MEMORY;
        }

        if (!m_hTypeAliasInfos.Put(&pAliasDirEntry, (IInterface**)&pAliasInfo)) {
            delete pAliasInfo;
            UnlockHashTable(EntryType_TypeAliase);
            return E_OUT_OF_MEMORY;
        }

        *ppObject = pAliasInfo;
    }
    else {
        *ppObject = *ppObj;
    }
    (*ppObject)->AddRef();

    UnlockHashTable(EntryType_TypeAliase);

    return NOERROR;
}

ECode CObjInfoList::RemoveTypeAliasInfo(
    /* [in] */ AliasDirEntry *pAliasDirEntry)
{
    if (!pAliasDirEntry) {
        return E_INVALID_ARGUMENT;
    }

    m_hTypeAliasInfos.Remove(&pAliasDirEntry);
    return NOERROR;
}

ECode CObjInfoList::AcquireInterfaceInfo(
    /* [in] */ CClsModule * pCClsModule,
    /* [in] */ UInt32 uIndex,
    /* [in, out] */ IInterface ** ppObject)
{
    if (!pCClsModule || !ppObject) {
        return E_INVALID_ARGUMENT;
    }

    LockHashTable(EntryType_Interface);
    if (*ppObject) {
        UnlockHashTable(EntryType_Interface);
        return NOERROR;
    }

    InterfaceDirEntry* pIFDir = getInterfaceDirAddr(pCClsModule->m_nBase,
            pCClsModule->m_pClsMod->ppInterfaceDir, uIndex);
    EIID iid = adjustInterfaceDescAddr(pCClsModule->m_nBase, pIFDir->pDesc)->iid;

    IInterface **ppObj = m_hIFInfos.Get(&iid);
    if (!ppObj) {
        CInterfaceInfo *pIFInfo = new CInterfaceInfo(pCClsModule, uIndex);
        if (pIFInfo == NULL) {
            UnlockHashTable(EntryType_Interface);
            return E_OUT_OF_MEMORY;
        }

        ECode ec = pIFInfo->Init();
        if (FAILED(ec)) {
            delete pIFInfo;
            UnlockHashTable(EntryType_Interface);
            return ec;
        }

        if (!m_hIFInfos.Put(&iid, (IInterface**)&pIFInfo)) {
            delete pIFInfo;
            UnlockHashTable(EntryType_Interface);
            return E_OUT_OF_MEMORY;
        }

        *ppObject = pIFInfo;
    }
    else {
        *ppObject = *ppObj;
    }
    (*ppObject)->AddRef();

    UnlockHashTable(EntryType_Interface);

    return NOERROR;
}

ECode CObjInfoList::RemoveInterfaceInfo(
    /* [in] */ EIID iid)
{
    m_hIFInfos.Remove(&iid);
    return NOERROR;
}

ECode CObjInfoList::AcquireMethodInfo(
    /* [in] */ CClsModule * pCClsModule,
    /* [in] */ MethodDescriptor *pMethodDescriptor,
    /* [in] */ UInt32 uIndex,
    /* [in, out] */ IInterface ** ppObject)
{
    if (!pCClsModule || !pMethodDescriptor || !ppObject) {
        return E_INVALID_ARGUMENT;
    }

    LockHashTable(EntryType_Method);
    if (*ppObject) {
        UnlockHashTable(EntryType_Method);
        return NOERROR;
    }

    UInt64 iKeyValue;
    memcpy(&iKeyValue, &pMethodDescriptor, 4);
    memcpy((PByte)&iKeyValue + 4, &uIndex, 4);
    IInterface **ppObj = m_hMethodInfos.Get(&iKeyValue);
    if (!ppObj) {
        CMethodInfo *pMethodInfo = new CMethodInfo(pCClsModule,
            pMethodDescriptor, uIndex);
        if (pMethodInfo == NULL) {
            UnlockHashTable(EntryType_Method);
            return E_OUT_OF_MEMORY;
        }

        ECode ec = pMethodInfo->Init();
        if (FAILED(ec)) {
            delete pMethodInfo;
            UnlockHashTable(EntryType_Method);
            return ec;
        }

        if (!m_hMethodInfos.Put(&iKeyValue, (IInterface**)&pMethodInfo)) {
            delete pMethodInfo;
            UnlockHashTable(EntryType_Method);
            return E_OUT_OF_MEMORY;
        }
        *ppObject = pMethodInfo;
    }
    else {
        *ppObject = *ppObj;
    }
    (*ppObject)->AddRef();

    UnlockHashTable(EntryType_Method);

    return NOERROR;
}

ECode CObjInfoList::RemoveMethodInfo(
    /* [in] */ MethodDescriptor *pMethodDescriptor,
    /* [in] */ UInt32 uIndex)
{
    if (!pMethodDescriptor) {
        return E_INVALID_ARGUMENT;
    }

    UInt64 iKeyValue;
    memcpy(&iKeyValue, &pMethodDescriptor, 4);
    memcpy((PByte)&iKeyValue + 4, &uIndex, 4);
    m_hMethodInfos.Remove(&iKeyValue);

    return NOERROR;
}

ECode CObjInfoList::AcquireDataTypeInfo(
    /* [in] */ CClsModule * pCClsModule,
    /* [in] */ TypeDescriptor *pTypeDesc,
    /* [out] */ IDataTypeInfo ** ppDataTypeInfo,
    /* [in] */ Boolean bCheckLocalPtr)
{
    if (!pCClsModule || !pTypeDesc || !ppDataTypeInfo) {
        return E_INVALID_ARGUMENT;
    }

    ECode ec = NOERROR;
    CLSModule *pClsMod = pCClsModule->m_pClsMod;
    Int32 pointer = pTypeDesc->nPointer;
    if (pTypeDesc->type == Type_alias) {
        ec = pCClsModule->AliasToOriginal(pTypeDesc, &pTypeDesc);
        if (FAILED(ec)) return ec;
        if (bCheckLocalPtr) pointer += pTypeDesc->nPointer;
    }

    CarDataType type = GetCarDataType(pTypeDesc->type);
    if (bCheckLocalPtr) {
        if (type == CarDataType_Interface) {
            if (pointer > 1) {
                type = CarDataType_LocalPtr;
                pointer -= 1;
            }
        }
        else if (pointer > 0) {
            type = CarDataType_LocalPtr;
        }
    }

    *ppDataTypeInfo = NULL;
    //LocalPtr
    if (type == CarDataType_LocalPtr) {
        return g_objInfoList.AcquireLocalPtrInfo(pCClsModule, pTypeDesc,
            pointer, (ILocalPtrInfo **)ppDataTypeInfo);
    }
    //LocalType
    else if (type == CarDataType_LocalType) {
        MemorySize size = GetDataTypeSize(pCClsModule, pTypeDesc);
        return AcquireLocalTypeInfo(pTypeDesc->type, size, ppDataTypeInfo);
    }
    //StructInfo
    else if (type == CarDataType_Struct) {
        StructDirEntry* pStructDir = getStructDirAddr(pCClsModule->m_nBase,
                pCClsModule->m_pClsMod->ppStructDir, pTypeDesc->sIndex);
        ec = AcquireStaticStructInfo(pCClsModule,
                pStructDir,
                (IInterface **)ppDataTypeInfo);
    }
    //InterfaceInfo
    else if (type == CarDataType_Interface) {
        ec = AcquireInterfaceInfo(pCClsModule,
                pTypeDesc->sIndex,
                (IInterface **)ppDataTypeInfo);
    }
    //EnumInfo
    else if (type == CarDataType_Enum) {
        EnumDirEntry* pEnumDir = getEnumDirAddr(pCClsModule->m_nBase,
                pClsMod->ppEnumDir, pTypeDesc->sIndex);
        ec = AcquireStaticEnumInfo(pCClsModule,
                pEnumDir,
                (IInterface **)ppDataTypeInfo);
    }
    //CppVectorInfo
    else if (type == CarDataType_CppVector) {
        AutoPtr<IDataTypeInfo> pElemInfo;
        ArrayDirEntry* pArrayDir = getArrayDirAddr(pCClsModule->m_nBase,
                pClsMod->ppArrayDir, pTypeDesc->sIndex);
        Int32 length = pArrayDir->nElements;
        TypeDescriptor *pType = &pArrayDir->type;
        ec = AcquireDataTypeInfo(pCClsModule, pType, (IDataTypeInfo**)&pElemInfo,
            bCheckLocalPtr);
        if (FAILED(ec)) return ec;
        ec = AcquireCppVectorInfo(pElemInfo, length,
            (ICppVectorInfo**)ppDataTypeInfo);
    }
    //CarArrayInfo
    else if (type == CarDataType_ArrayOf || type == CarDataType_BufferOf
            || type == CarDataType_MemoryBuf) {
        AutoPtr<IDataTypeInfo> pElemInfo;
        ec = AcquireCarArrayElementTypeInfo(pCClsModule, pTypeDesc,
            (IDataTypeInfo**)&pElemInfo);
        if (FAILED(ec)) return ec;
        ec = AcquireCarArrayInfo(type, pElemInfo, (ICarArrayInfo**)ppDataTypeInfo);
    }
    //IntrinsicInfo, StringBufInfo
    else {
        CarDataType type = GetCarDataType(pTypeDesc->type);

        if (type != CarDataType_LocalType) {
            UInt32 size = GetDataTypeSize(pCClsModule, pTypeDesc);
            ec = AcquireIntrinsicInfo(type, ppDataTypeInfo, size);
        }
    }

    return ec;
}

ECode CObjInfoList::AcquireIntrinsicInfo(
    /* [in] */ CarDataType dataType,
    /* [out] */ IDataTypeInfo ** ppDataTypeInfo,
    /* [in] */ UInt32 uSize)
{
    if (!ppDataTypeInfo) {
        return E_INVALID_ARGUMENT;
    }

    if (g_cDataTypeList[dataType].size) {
        uSize = g_cDataTypeList[dataType].size;
    }

    LockHashTable(EntryType_DataType);
    if (!m_pDataTypeInfos[dataType]) {
        if (dataType == CarDataType_StringBuf) {
            m_pDataTypeInfos[dataType] = new CStringBufInfo();
        }
//        else if (dataType ==
//            m_pDataTypeInfos[dataType] = new CCarArrayRefInfo(quintetType, pElementTypeInfo, dataType)
        else {
            m_pDataTypeInfos[dataType] = new CIntrinsicInfo(dataType, uSize);
        }

        if (!m_pDataTypeInfos[dataType]) {
            UnlockHashTable(EntryType_DataType);
            return E_OUT_OF_MEMORY;
        }
        m_pDataTypeInfos[dataType]->AddRef();
    }
    UnlockHashTable(EntryType_DataType);

    m_pDataTypeInfos[dataType]->AddRef();
    *ppDataTypeInfo = m_pDataTypeInfos[dataType];

    return NOERROR;
}

ECode CObjInfoList::AcquireLocalTypeInfo(
    /* [in] */ CARDataType type,
    /* [in] */ MemorySize size,
    /* [out] */ IDataTypeInfo ** ppDataTypeInfo)
{
    if (!ppDataTypeInfo) {
        return E_INVALID_ARGUMENT;
    }

    LockHashTable(EntryType_Local);
    if (!m_pLocalTypeInfos[type]) {
        m_pLocalTypeInfos[type] = new CLocalTypeInfo(size);
        if (!m_pLocalTypeInfos[type]) {
            UnlockHashTable(EntryType_Local);
            return E_OUT_OF_MEMORY;
        }
        m_pLocalTypeInfos[type]->AddRef();
    }
    UnlockHashTable(EntryType_Local);

    m_pLocalTypeInfos[type]->AddRef();
    *ppDataTypeInfo = m_pLocalTypeInfos[type];

    return NOERROR;
}

ECode CObjInfoList::AcquireLocalPtrInfo(
    /* [in] */ CClsModule * pCClsModule,
    /* [in] */ TypeDescriptor *pTypeDescriptor,
    /* [in] */ Int32 iPointer,
    /* [out] */ ILocalPtrInfo ** ppLocalPtrInfo)
{
    if (!pCClsModule || !pTypeDescriptor || !ppLocalPtrInfo) {
        return E_INVALID_ARGUMENT;
    }

    LockHashTable(EntryType_Local);

    UInt64 iKeyValue;
    memcpy(&iKeyValue, &pTypeDescriptor, 4);
    memcpy((PByte)&iKeyValue + 4, &iPointer, 4);
    IInterface **ppObj = m_hLocalPtrInfos.Get(&iKeyValue);
    if (!ppObj) {
        CLocalPtrInfo *pLocalPtrInfo = new CLocalPtrInfo(pCClsModule,
                                            pTypeDescriptor, iPointer);
        if (pLocalPtrInfo == NULL) {
            UnlockHashTable(EntryType_Local);
            return E_OUT_OF_MEMORY;
        }

        if (!m_hLocalPtrInfos.Put(&iKeyValue, (IInterface**)&pLocalPtrInfo)) {
            delete pLocalPtrInfo;
            UnlockHashTable(EntryType_Local);
            return E_OUT_OF_MEMORY;
        }
        *ppLocalPtrInfo = pLocalPtrInfo;
    }
    else {
        *ppLocalPtrInfo = (ILocalPtrInfo *)*ppObj;
    }
    (*ppLocalPtrInfo)->AddRef();

    UnlockHashTable(EntryType_Local);

    return NOERROR;
}

ECode CObjInfoList::RemoveLocalPtrInfo(
    /* [in] */ TypeDescriptor *pTypeDescriptor,
    /* [in] */ Int32 iPointer)
{
    if (!pTypeDescriptor) {
        return E_INVALID_ARGUMENT;
    }

    UInt64 iKeyValue;
    memcpy(&iKeyValue, &pTypeDescriptor, 4);
    memcpy((PByte)&iKeyValue + 4, &iPointer, 4);
    m_hLocalPtrInfos.Remove(&iKeyValue);

    return NOERROR;
}

ECode CObjInfoList::AcquireCppVectorInfo(
    /* [in] */ IDataTypeInfo *pElementTypeInfo,
    /* [in] */ Int32 length,
    /* [out] */ ICppVectorInfo **ppCppVectorInfo)
{
    if (!pElementTypeInfo || !ppCppVectorInfo) {
        return E_INVALID_ARGUMENT;
    }

    InfoLinkNode *pNode = m_pCCppVectorHead;
    Int32 len = 0;
    ICppVectorInfo *pCppVectorInfo = NULL;
    ECode ec = NOERROR;
    LockHashTable(EntryType_DataType);
    for (; pNode; pNode = pNode->pNext) {
        pCppVectorInfo = (ICppVectorInfo *)pNode->pInfo;
        pCppVectorInfo->GetLength(&len);
        if (len == length) {
            AutoPtr<IDataTypeInfo> pElementInfo;
            ec = pCppVectorInfo->GetElementTypeInfo((IDataTypeInfo **)&pElementInfo);
            if (FAILED(ec)) {
                UnlockHashTable(EntryType_DataType);
                return ec;
            }
            if (pElementInfo.Get() == pElementTypeInfo) {
                pCppVectorInfo->AddRef();
                *ppCppVectorInfo = pCppVectorInfo;
                UnlockHashTable(EntryType_DataType);
                return NOERROR;
            }
        }
    }

    pCppVectorInfo = new CCppVectorRefInfo(pElementTypeInfo, length);
    if (pCppVectorInfo == NULL) {
        UnlockHashTable(EntryType_DataType);
        return E_OUT_OF_MEMORY;
    }

    ec = AddInfoNode(pCppVectorInfo, &m_pCCppVectorHead);
    if (FAILED(ec)) {
        delete pCppVectorInfo;
        UnlockHashTable(EntryType_DataType);
        return ec;
    }

    pCppVectorInfo->AddRef();
    *ppCppVectorInfo = pCppVectorInfo;
    UnlockHashTable(EntryType_DataType);

    return NOERROR;
}

ECode CObjInfoList::AcquireCarArrayElementTypeInfo(
    /* [in] */ CClsModule * pCClsModule,
    /* [in] */ TypeDescriptor *pTypeDesc,
    /* [out] */ IDataTypeInfo ** ppElementTypeInfo)
{
    CarDataType type;
    switch (pTypeDesc->type) {
        case Type_ArrayOf:
        case Type_BufferOf:
        case Type_MemoryBuf:
            return AcquireDataTypeInfo(pCClsModule,
                adjustNestedTypeAddr(pCClsModule->m_nBase, pTypeDesc->pNestedType),
                ppElementTypeInfo);
        default:
            return E_INVALID_OPERATION;
    }

    return AcquireIntrinsicInfo(type, ppElementTypeInfo);
}

ECode CObjInfoList::AcquireCarArrayInfo(
    /* [in] */ CarDataType quintetType,
    /* [in] */ IDataTypeInfo *pElementTypeInfo,
    /* [out] */ ICarArrayInfo **ppCarArrayInfo)
{

    if ((quintetType != CarDataType_ArrayOf && quintetType != CarDataType_BufferOf
        && quintetType != CarDataType_MemoryBuf) || !ppCarArrayInfo) {
        return E_INVALID_ARGUMENT;
    }

    //BUGBUG::CarDataType_MemoryBuf should move to AcquireIntrinsicInfo,
    //        And should add CArrayOfInfo, CBufferOfInfo, CMemoryBufInfo
    if ((quintetType != CarDataType_MemoryBuf) && !pElementTypeInfo) {
        return E_INVALID_ARGUMENT;
    }

    ECode ec = NOERROR;
    if (quintetType == CarDataType_MemoryBuf) {
        ec = AcquireIntrinsicInfo(CarDataType_Byte, &pElementTypeInfo);
        if (FAILED(ec)) return ec;
        //  release for [out] parameter. pElementTypeInfo is alive after release
        pElementTypeInfo->Release();
    }

    InfoLinkNode *pNode = m_pCarArrayInfoHead;
    ICarArrayInfo *pCarArrayInfo = NULL;
    Void * pContext = NULL;

    CarDataType dataType;
    ec = pElementTypeInfo->GetDataType(&dataType);
    if (FAILED(ec)) return ec;

    if (dataType == CarDataType_StringBuf) {
        return E_INVALID_ARGUMENT;
    }

    if (dataType != CarDataType_Struct) {
        LockHashTable(EntryType_DataType);
        if (!m_pCarArrayInfos[dataType]) {
            m_pCarArrayInfos[dataType] =
                new CCarArrayRefInfo(quintetType, pElementTypeInfo, dataType);
            if (!m_pCarArrayInfos[dataType]) {
                UnlockHashTable(EntryType_DataType);
                return E_OUT_OF_MEMORY;
            }
            m_pCarArrayInfos[dataType]->AddRef();
        }
        UnlockHashTable(EntryType_DataType);

        m_pCarArrayInfos[dataType]->AddRef();
        *ppCarArrayInfo = m_pCarArrayInfos[dataType];
    }
    else {
        AutoPtr<IDataTypeInfo> pElementInfo;
        LockHashTable(EntryType_DataType);
        for (; pNode; pNode = pNode->pNext) {
            pCarArrayInfo = (ICarArrayInfo *)pNode->pInfo;
            pElementInfo = NULL;
            ec = pCarArrayInfo->GetElementTypeInfo((IDataTypeInfo **)&pElementInfo);
            if (FAILED(ec)) {
                UnlockHashTable(EntryType_DataType);
                return ec;
            }
            if (pElementInfo == pElementTypeInfo) {
                pCarArrayInfo->AddRef();
                *ppCarArrayInfo = pCarArrayInfo;
                UnlockHashTable(EntryType_DataType);
                return NOERROR;
            }
        }

        pCarArrayInfo = new CCarArrayRefInfo(quintetType, pElementTypeInfo);
        if (pCarArrayInfo == NULL) {
            UnlockHashTable(EntryType_DataType);
            return E_OUT_OF_MEMORY;
        }

        ec = AddInfoNode(pCarArrayInfo, &m_pCarArrayInfoHead);
        if (FAILED(ec)) {
            delete pCarArrayInfo;
            UnlockHashTable(EntryType_DataType);
            return ec;
        }

        pCarArrayInfo->AddRef();
        *ppCarArrayInfo = pCarArrayInfo;
        UnlockHashTable(EntryType_DataType);
    }

    return NOERROR;
}

ECode CObjInfoList::AcquireConstantInfo(
    /* [in] */ CClsModule * pCClsModule,
    /* [in] */ ConstDirEntry *pConstDirEntry,
    /* [in, out] */ IInterface ** ppObject)
{
    if (!pCClsModule || !pConstDirEntry || !ppObject) {
        return E_INVALID_ARGUMENT;
    }

    LockHashTable(EntryType_Constant);
    if (*ppObject) {
        UnlockHashTable(EntryType_Constant);
        return NOERROR;
    }

    CConstantInfo *pConstantInfo = new CConstantInfo(pCClsModule, pConstDirEntry);
    if (pConstantInfo == NULL) {
        UnlockHashTable(EntryType_Constant);
        return E_OUT_OF_MEMORY;
    }

    *ppObject = pConstantInfo;
    (*ppObject)->AddRef();
    UnlockHashTable(EntryType_Constant);

    return NOERROR;
}

ECode CObjInfoList::AcquireConstructorInfo(
    /* [in] */ CClsModule * pCClsModule,
    /* [in] */ MethodDescriptor *pMethodDescriptor,
    /* [in] */ UInt32 uIndex,
    /* [in] */ ClassID *pClsId,
    /* [in, out] */ IInterface ** ppObject)
{
    if (!pCClsModule || !pMethodDescriptor || !pClsId || !ppObject) {
        return E_INVALID_ARGUMENT;
    }

    LockHashTable(EntryType_Constructor);
    if (*ppObject) {
        UnlockHashTable(EntryType_Constructor);
        return NOERROR;
    }

    CConstructorInfo *pConstructInfo = new CConstructorInfo();
    if (pConstructInfo == NULL) {
        UnlockHashTable(EntryType_Constructor);
        return E_OUT_OF_MEMORY;
    }

    ECode ec = pConstructInfo->Init(pCClsModule,
                    pMethodDescriptor, uIndex, pClsId);
    if (FAILED(ec)) {
        delete pConstructInfo;
        UnlockHashTable(EntryType_Constructor);
        return ec;
    }

    *ppObject = pConstructInfo;
    (*ppObject)->AddRef();
    UnlockHashTable(EntryType_Constructor);

    return NOERROR;
}

ECode CObjInfoList::AcquireCBMethodInfoInfo(
    /* [in] */ CClsModule * pCClsModule,
    /* [in] */ UInt32 uEventNum,
    /* [in] */ MethodDescriptor *pMethodDescriptor,
    /* [in] */ UInt32 uIndex,
    /* [in, out] */ IInterface ** ppObject)
{
    if (!pCClsModule || !pMethodDescriptor || !ppObject) {
        return E_INVALID_ARGUMENT;
    }

    LockHashTable(EntryType_CBMethod);
    if (*ppObject) {
        UnlockHashTable(EntryType_CBMethod);
        return NOERROR;
    }

    CCallbackMethodInfo *pCBMethodInfo = new CCallbackMethodInfo();
    if (pCBMethodInfo == NULL) {
        UnlockHashTable(EntryType_CBMethod);
        return E_OUT_OF_MEMORY;
    }

    ECode ec = pCBMethodInfo->Init(pCClsModule,
                    uEventNum, pMethodDescriptor, uIndex);
    if (FAILED(ec)) {
        delete pCBMethodInfo;
        UnlockHashTable(EntryType_CBMethod);
        return ec;
    }

    *ppObject = pCBMethodInfo;
    (*ppObject)->AddRef();
    UnlockHashTable(EntryType_CBMethod);

    return NOERROR;
}

ECode CObjInfoList::AddInfoNode(
    IDataTypeInfo *pInfo,
    InfoLinkNode **ppLinkHead)
{
    InfoLinkNode *pNode = new InfoLinkNode();
    if (!pNode) {
        return E_OUT_OF_MEMORY;
    }

    pNode->pInfo = pInfo;
    pNode->pNext = NULL;
    if (!*ppLinkHead) {
        *ppLinkHead = pNode;
    }
    else {
        pNode->pNext = *ppLinkHead;
        *ppLinkHead = pNode;
    }

    return NOERROR;
}

ECode CObjInfoList::DeleteInfoNode(
    IDataTypeInfo *pInfo,
    InfoLinkNode **ppLinkHead)
{
    InfoLinkNode *pPreNode = *ppLinkHead;
    InfoLinkNode *pNode = pPreNode;

    while (pNode) {
        if (pInfo == pNode->pInfo) {
            if (pNode == pPreNode) {
                *ppLinkHead = pNode->pNext;
            }
            else {
                pPreNode->pNext = pNode->pNext;
            }
            delete pNode;
            break;
        }
        pPreNode = pNode;
        pNode = pNode->pNext;
    }

    return NOERROR;
}

ECode CObjInfoList::DetachEnumInfo(
    /* [in] */ IEnumInfo *pEnumInfo)
{
    return DeleteInfoNode(pEnumInfo, &m_pEnumInfoHead);
}

ECode CObjInfoList::DetachCppVectorInfo(
    /* [in] */ ICppVectorInfo *pCppVectorInfo)
{
    return DeleteInfoNode(pCppVectorInfo, &m_pCCppVectorHead);
}

ECode CObjInfoList::DetachStructInfo(
    /* [in] */ IStructInfo *pStructInfo)
{
    return DeleteInfoNode(pStructInfo, &m_pStructInfoHead);
}

ECode CObjInfoList::DetachCarArrayInfo(
    /* [in] */ ICarArrayInfo *pCarArrayInfo)
{
    return DeleteInfoNode(pCarArrayInfo, &m_pCarArrayInfoHead);
}
