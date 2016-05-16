//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#include "CEntryList.h"

#include "CConstantInfo.h"
#include "CConstructorInfo.h"
#include "CEnumItemInfo.h"
#include "CParamInfo.h"
#include "CFieldInfo.h"
#include "CClassInfo.h"

UInt32 CEntryList::AddRef()
{
    Int32 nRef = atomic_inc(&m_cRef);
    return (UInt32)nRef;
}

UInt32 CEntryList::Release()
{
    Int32 nRef = atomic_dec(&m_cRef);

    if (0 == nRef) {
        delete this;
    }
    assert(nRef >= 0);
    return nRef;
}

CEntryList::CEntryList(
    /* [in] */ EntryType type,
    /* [in] */ void *pDesc,
    /* [in] */ UInt32 uTotalCount,
    /* [in] */ CClsModule * pCClsModule,
    /* [in] */ IFIndexEntry *pIFList,
    /* [in] */ UInt32 uListCount,
    /* [in] */ CClassInfo *pClsInfo)
{
    m_pCClsModule = pCClsModule;
    m_pCClsModule->AddRef();
    m_pClsMod = m_pCClsModule->m_pClsMod;

    m_type = type;
    m_pDesc = pDesc;
    m_uTotalCount = uTotalCount;

    m_pObjElement = NULL;
    m_pIFList = pIFList;
    m_uListCount = uListCount;
    m_pClsInfo = pClsInfo;

    m_nBase = m_pCClsModule->m_nBase;
    m_cRef = 0;
}

CEntryList::~CEntryList()
{
    if (m_pCClsModule) m_pCClsModule->Release();

    if (m_pObjElement) {
        for (UInt32 i = 0; i < m_uTotalCount; i++) {
            if (m_pObjElement[i].pObj) {
                m_pObjElement[i].pObj->Release();
            }
        }
        delete[] m_pObjElement;
    }
}

ECode CEntryList::InitElemList()
{
    if (m_uTotalCount == 0) {
        return E_DOES_NOT_EXIST;
    }

    if (m_pObjElement) {
        return NOERROR;
    }

    m_pObjElement = new ObjElement[m_uTotalCount];
    if (m_pObjElement == NULL) {
        return E_OUT_OF_MEMORY;
    }
    memset(m_pObjElement, 0, sizeof(ObjElement) * m_uTotalCount);

    UInt32 uIndex = 0, i = 0, j = 0, n = 0;
    if (m_type == EntryType_TypeAliase) {
        AliasDirEntry* pAliasDir = NULL;
        m_pObjElement[i].uIndex = 0;
        for (i = 0; i < m_pClsMod->cAliases; i++) {
            if (!IsSysAlaisType(m_pCClsModule, i)) {
                pAliasDir = getAliasDirAddr(m_nBase, m_pClsMod->ppAliasDir, i);
                m_pObjElement[j].uIndex = i;
                m_pObjElement[j].pszName = adjustNameAddr(m_nBase, pAliasDir->pszName);
                if (!m_pHTIndexs.Put(m_pObjElement[j].pszName, j)) {
                    return E_OUT_OF_MEMORY;
                }
                j++;
            }
        }
        return NOERROR;
    }
    else if (m_type == EntryType_ClassInterface) {
        for (i = 0; i < m_uListCount; i++) {
            m_pObjElement[i].uIndex = m_pIFList[i].uIndex;
            m_pObjElement[i].pObj = NULL;
            m_pObjElement[i].pDesc = NULL;
            m_pObjElement[i].pszName = m_pIFList[i].pszName;
            if (!m_pHTIndexs.Put(m_pObjElement[i].pszName, i)) {
                return E_OUT_OF_MEMORY;
            }
        }
        return NOERROR;
    }
    else if (m_type == EntryType_Method || m_type == EntryType_Constructor
            || m_type == EntryType_CBMethod) {

        n = 0;
        for (i = 0; i < m_uListCount; i++) {
            for (j = 0; j < m_pIFList[i].pDesc->cMethods; j++, n++) {
                if (n == m_uTotalCount) return E_INVALID_ARGUMENT;

                m_pObjElement[n].uIndex = MK_METHOD_INDEX(m_pIFList[i].uIndex,
                        m_pIFList[i].uBeginNo + j);
                m_pObjElement[n].pObj = NULL;
                m_pObjElement[n].pDesc = getMethodDescAddr(m_nBase,
                        m_pIFList[i].pDesc->ppMethods, j);
                m_pObjElement[n].pszName = adjustNameAddr(m_nBase,
                        ((MethodDescriptor *)m_pObjElement[n].pDesc)->pszName);
                if (!m_pHTIndexs.Put(m_pObjElement[n].pszName, n)) {
                    return E_OUT_OF_MEMORY;
                }
            }
        }
        return NOERROR;
    }

	ClassDirEntry*      pClassDir = NULL;
	InterfaceDirEntry*  pIFDir = NULL;
	StructDirEntry*     pStructDir = NULL;
	EnumDirEntry*       pEmumDir = NULL;
	ConstDirEntry*      pConstDir = NULL;
    //No EntryType_TypeAliase
    for (i = 0; i < m_uTotalCount; i++) {
        m_pObjElement[i].uIndex = i;
        m_pObjElement[i].pObj = NULL;
        m_pObjElement[i].pDesc = NULL;

        switch (m_type) {
            case EntryType_Aspect:
                uIndex = adjustIndexsAddr(m_nBase,
                        ((ClassDescriptor *)m_pDesc)->pAspectIndexs)[i];
                pClassDir = getClassDirAddr(m_nBase, m_pClsMod->ppClassDir, uIndex);
                m_pObjElement[i].pszName = adjustNameAddr(m_nBase,
                        pClassDir->pszName);
                break;
            case EntryType_Aggregatee:
                uIndex =  adjustIndexsAddr(m_nBase,
                        ((ClassDescriptor *)m_pDesc)->pAggrIndexs)[i];
                pClassDir = getClassDirAddr(m_nBase, m_pClsMod->ppClassDir, uIndex);
                m_pObjElement[i].pszName = adjustNameAddr(m_nBase,
                        pClassDir->pszName);
                break;
            case EntryType_Class:
                pClassDir = getClassDirAddr(m_nBase, m_pClsMod->ppClassDir, i);
                m_pObjElement[i].pszName  = adjustNameAddr(m_nBase,
                        pClassDir->pszName);
                break;
            case EntryType_ClassInterface:
                uIndex =  m_pObjElement[i].uIndex;
                pIFDir = getInterfaceDirAddr(m_nBase,
                        m_pClsMod->ppInterfaceDir, uIndex);
                m_pObjElement[i].pszName = adjustNameAddr(m_nBase,
                        pIFDir->pszName);
                break;
            case EntryType_Interface:
                pIFDir = getInterfaceDirAddr(m_nBase, m_pClsMod->ppInterfaceDir, i);
                m_pObjElement[i].pszName = adjustNameAddr(m_nBase,
                        pIFDir->pszName);
                break;
            case EntryType_Struct:
                pStructDir = getStructDirAddr(m_nBase, m_pClsMod->ppStructDir, i);
                m_pObjElement[i].pszName = adjustNameAddr(m_nBase,
                        pStructDir->pszName);
                break;
            case EntryType_Enum:
                pEmumDir = getEnumDirAddr(m_nBase, m_pClsMod->ppEnumDir, i);
                m_pObjElement[i].pszName = adjustNameAddr(m_nBase,
                        pEmumDir->pszName);
                break;
            case EntryType_Constant:
                pConstDir = getConstDirAddr(m_nBase, m_pClsMod->ppConstDir, i);
                m_pObjElement[i].pszName = adjustNameAddr(m_nBase,
                        pConstDir->pszName);
                break;
            default:
                return E_INVALID_OPERATION;
        }

        if (!m_pHTIndexs.Put(m_pObjElement[i].pszName, i)) {
            return E_OUT_OF_MEMORY;
        }
    }
    return NOERROR;
}

ECode CEntryList::AcquireObjByName(
    /* [in] */ CString name,
    /* [out] */ IInterface ** ppObject)
{
    if (!ppObject || name.IsNull()) {
        return E_INVALID_ARGUMENT;
    }

    g_objInfoList.LockHashTable(m_type);
    ECode ec = InitElemList();
    g_objInfoList.UnlockHashTable(m_type);
    if (FAILED(ec)) {
        return ec;
    }

    UInt32 *pIndex = m_pHTIndexs.Get((PVoid)(const char *)name);
    if (pIndex == NULL) {
        return E_DOES_NOT_EXIST;
    }
    else {
        return AcquireObjByIndex(*pIndex, ppObject);
    }
}

ECode CEntryList::AcquireObjByIndex(
    /* [in] */ UInt32 uIndex,
    /* [out] */ IInterface ** ppObject)
{
    if (!ppObject) {
        return E_INVALID_ARGUMENT;
    }

    if (!m_uTotalCount || (uIndex & 0x0000FFFF) >= m_uTotalCount) {
        return E_DOES_NOT_EXIST;
    }

    g_objInfoList.LockHashTable(m_type);
    ECode ec = InitElemList();
    g_objInfoList.UnlockHashTable(m_type);
    if (FAILED(ec)) {
        return ec;
    }

    UInt32 i = 0;
    if ((m_type == EntryType_Method || m_type == EntryType_Constructor
            || m_type == EntryType_CBMethod) && (uIndex & 0xFFFF0000)) {
        //Method's Index
        //Change to the array's index
        for (i = 0; i < m_uTotalCount; i++) {
            if (m_pObjElement[i].uIndex == uIndex) {
                uIndex = i;
                break;
            }
        }
        if (uIndex != i) return E_INVALID_ARGUMENT;
    }

    UInt32 sIndex = 0;

    switch (m_type) {
        case EntryType_Aspect:
            sIndex = adjustIndexsAddr(m_nBase,
                ((ClassDescriptor *)m_pDesc)->pAspectIndexs)[uIndex];
            ec = g_objInfoList.AcquireClassInfo(m_pCClsModule,
                getClassDirAddr(m_nBase, m_pClsMod->ppClassDir, sIndex),
                &m_pObjElement[uIndex].pObj);
            break;

        case EntryType_Aggregatee:
            sIndex =  adjustIndexsAddr(m_nBase,
                ((ClassDescriptor *)m_pDesc)->pAggrIndexs)[uIndex];
            ec = g_objInfoList.AcquireClassInfo(m_pCClsModule,
                getClassDirAddr(m_nBase, m_pClsMod->ppClassDir, sIndex),
                &m_pObjElement[uIndex].pObj);
            break;

        case EntryType_Class:
            ec = g_objInfoList.AcquireClassInfo(m_pCClsModule,
                getClassDirAddr(m_nBase, m_pClsMod->ppClassDir, uIndex),
                &m_pObjElement[uIndex].pObj);
            break;

        case EntryType_ClassInterface:
            sIndex =  m_pObjElement[uIndex].uIndex;
            ec = g_objInfoList.AcquireInterfaceInfo(m_pCClsModule, sIndex,
                &m_pObjElement[uIndex].pObj);
            break;

        case EntryType_Interface:
            ec = g_objInfoList.AcquireInterfaceInfo(m_pCClsModule, uIndex,
                &m_pObjElement[uIndex].pObj);
            break;

        case EntryType_Struct:
            ec = g_objInfoList.AcquireStaticStructInfo(m_pCClsModule,
                getStructDirAddr(m_nBase, m_pClsMod->ppStructDir, uIndex),
                &m_pObjElement[uIndex].pObj);
            break;

        case EntryType_Enum:
            ec = g_objInfoList.AcquireStaticEnumInfo(m_pCClsModule,
                getEnumDirAddr(m_nBase, m_pClsMod->ppEnumDir, uIndex),
                &m_pObjElement[uIndex].pObj);
            break;

        case EntryType_TypeAliase:
            sIndex =  m_pObjElement[uIndex].uIndex;
            ec = g_objInfoList.AcquireTypeAliasInfo(m_pCClsModule,
                getAliasDirAddr(m_nBase, m_pClsMod->ppAliasDir, sIndex),
                &m_pObjElement[uIndex].pObj);
            break;

        case EntryType_Constant:
            ec = g_objInfoList.AcquireConstantInfo(m_pCClsModule,
                getConstDirAddr(m_nBase, m_pClsMod->ppConstDir, uIndex),
                &m_pObjElement[uIndex].pObj);
            break;

        case EntryType_Method:
            ec = g_objInfoList.AcquireMethodInfo(m_pCClsModule,
                (MethodDescriptor *)m_pObjElement[uIndex].pDesc,
                m_pObjElement[uIndex].uIndex,
                &m_pObjElement[uIndex].pObj);
            break;

        case EntryType_Constructor:
            ec = g_objInfoList.AcquireConstructorInfo(m_pCClsModule,
                (MethodDescriptor *)m_pObjElement[uIndex].pDesc,
                m_pObjElement[uIndex].uIndex,
                &m_pClsInfo->m_clsId,
                &m_pObjElement[uIndex].pObj);
            break;

        case EntryType_CBMethod:
            ec = g_objInfoList.AcquireCBMethodInfoInfo(m_pCClsModule,
                m_pClsInfo->m_pCBMethodDesc[uIndex].uEventNum,
                m_pClsInfo->m_pCBMethodDesc[uIndex].pDesc,
                m_pClsInfo->m_pCBMethodDesc[uIndex].uIndex,
                &m_pObjElement[uIndex].pObj);
            break;

        default:
            ec = E_INVALID_OPERATION;
    }

    if (FAILED(ec)) {
        return ec;
    }

    m_pObjElement[uIndex].pObj->AddRef();
    *ppObject = m_pObjElement[uIndex].pObj;
    return NOERROR;
}

ECode CEntryList::GetAllObjInfos(
    /* [out] */ BufferOf<IInterface *> * pObjInfos)
{
    if (!pObjInfos) {
        return E_INVALID_ARGUMENT;
    }

    Int32 nCapacity = pObjInfos->GetCapacity();
    if (!nCapacity) {
        return E_INVALID_ARGUMENT;
    }

    if (!m_uTotalCount) {
        pObjInfos->SetUsed(0);
        return NOERROR;
    }

    Int32 nCount = nCapacity < (int)m_uTotalCount ? nCapacity : m_uTotalCount;
    IInterface *pObject = NULL;
    ECode ec = NOERROR;
    for (Int32 i = 0; i < nCount; i++) {
        ec = AcquireObjByIndex(i, &pObject);
        if (FAILED(ec)) return ec;
        pObjInfos->SetUsed(i + 1);
        (*pObjInfos)[i] = pObject;
    }

    return NOERROR;
}
