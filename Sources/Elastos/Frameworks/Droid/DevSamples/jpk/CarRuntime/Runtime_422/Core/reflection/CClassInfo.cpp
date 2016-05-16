//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#include "CClassInfo.h"
#include "CCallbackMethodInfo.h"
#include "CConstructorInfo.h"

UInt32 CClassInfo::AddRef()
{
    Int32 nRef = atomic_inc(&m_cRef);
    return (UInt32)nRef;
}

UInt32 CClassInfo::Release()
{
    g_objInfoList.LockHashTable(EntryType_Class);
    Int32 nRef = atomic_dec(&m_cRef);

    if (0 == nRef) {
        g_objInfoList.RemoveClassInfo(m_pClassDirEntry);
        delete this;
    }
    g_objInfoList.UnlockHashTable(EntryType_Class);
    assert(nRef >= 0);
    return nRef;
}

PInterface CClassInfo::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)this;
    }
    else if (riid == EIID_IClassInfo) {
        return (IClassInfo *)this;
    }
    else {
        return NULL;
    }
}

ECode CClassInfo::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    return E_NOT_IMPLEMENTED;
}

CClassInfo::CClassInfo(
    /* [in] */ CClsModule * pCClsModule,
    /* [in] */ ClassDirEntry *pClsDirEntry)
{
    m_pCClsModule = pCClsModule;
    m_pCClsModule->AddRef();
    m_pClsMod = m_pCClsModule->m_pClsMod;
    m_pClassDirEntry = pClsDirEntry;
    m_pIFList = NULL;
    m_pCBIFList = NULL;

    m_pMethodList = NULL;
    m_pAspectList = NULL;
    m_pAggregateeList = NULL;
    m_pCBInterfaceList = NULL;
    m_pInterfaceList = NULL;
    m_pCtorList = NULL;
    m_pCBMethodList = NULL;
    m_pCBMethodDesc = NULL;
    m_pCtorClassInfo = NULL;
    m_pGenericInfo = NULL;

    m_uMethodCount = 0;
    m_uCBMethodCount = 0;
    m_uCBIFCount = 0;
    m_uIFCount = 0;
    m_cRef = 0;

    m_nBase = m_pCClsModule->m_nBase;
    m_pDesc = adjustClassDescAddr(m_nBase, m_pClassDirEntry->pDesc);

    m_clsId.pUunm = m_szUrn;
    m_clsId.clsid =  m_pDesc->clsid;
    strcpy(m_clsId.pUunm, adjustNameAddr(m_nBase, m_pClsMod->pszUunm));
}

CClassInfo::~CClassInfo()
{
    if (m_pCClsModule) m_pCClsModule->Release();
    if (m_pMethodList) m_pMethodList->Release();
    if (m_pCBMethodList) m_pCBMethodList->Release();
    if (m_pCBMethodDesc) {
        delete [] m_pCBMethodDesc;
    }
    if (m_pIFList) delete[] m_pIFList;
    if (m_pCBIFList) delete[] m_pCBIFList;

    if (m_pAspectList) m_pAspectList->Release();
    if (m_pAggregateeList) m_pAggregateeList->Release();
    if (m_pCBInterfaceList) m_pCBInterfaceList->Release();
    if (m_pInterfaceList) m_pInterfaceList->Release();
    if (m_pCtorList) m_pCtorList->Release();

    if (m_pCtorClassInfo) m_pCtorClassInfo->Release();
    if (m_pGenericInfo) m_pGenericInfo->Release();
}

ECode CClassInfo::Init()
{
    ECode ec = CreateIFList();
    if (FAILED(ec)) return ec;

    return NOERROR;
}

ECode CClassInfo::GetName(
    /* [out] */ StringBuf * pName)
{
    if (pName == NULL || !pName->GetCapacity()) {
        return E_INVALID_ARGUMENT;
    }

    pName->Copy(adjustNameAddr(m_nBase, m_pClassDirEntry->pszName));
    return NOERROR;
}

ECode CClassInfo::GetId(
    /* [out] */ ClassID * pclsid)
{
    if (!pclsid) {
        return E_INVALID_ARGUMENT;
    }

    pclsid->clsid =  m_pDesc->clsid;

    strcpy(pclsid->pUunm,  adjustNameAddr(m_nBase, m_pClsMod->pszUunm));

    return NOERROR;
}

ECode CClassInfo::GetModuleInfo(
    /* [out] */ IModuleInfo ** ppModuleInfo)
{
    return m_pCClsModule->GetModuleInfo(ppModuleInfo);
}

ECode CClassInfo::IsSingleton(
    /* [out] */ Boolean * pIsSingleton)
{
    if (!pIsSingleton) {
        return E_INVALID_ARGUMENT;
    }

    if (m_pDesc->dwAttribs & ClassAttrib_singleton) {
        *pIsSingleton = TRUE;
    }
    else {
        *pIsSingleton = FALSE;
    }

    return NOERROR;
}

ECode CClassInfo::GetThreadingModel(
    /* [out] */ ThreadingModel *pThreadingModel)
{
    if (!pThreadingModel) {
        return E_INVALID_ARGUMENT;
    }

    if (m_pDesc->dwAttribs & ClassAttrib_freethreaded) {
        *pThreadingModel = ThreadingModel_ThreadSafe;
    }
    else if (m_pDesc->dwAttribs & ClassAttrib_naked) {
        *pThreadingModel = ThreadingModel_Naked;
    }
    else {
        return E_DOES_NOT_EXIST;
    }

    return NOERROR;
}

ECode CClassInfo::IsPrivate(
    /* [out] */ Boolean * pIsPrivate)
{
    if (!pIsPrivate) {
        return E_INVALID_ARGUMENT;
    }

    if (m_pDesc->dwAttribs & ClassAttrib_private) {
        *pIsPrivate = TRUE;
    }
    else {
        *pIsPrivate = FALSE;
    }

    return NOERROR;
}

ECode CClassInfo::IsReturnValue(
    /* [out] */ Boolean * pIsReturnValue)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CClassInfo::IsBaseClass(
    /* [out] */ Boolean * pIsBaseClass)
{
    if (!pIsBaseClass) {
        return E_INVALID_ARGUMENT;
    }

    if (m_pDesc->dwAttribs & ClassAttrib_hasvirtual) {
        *pIsBaseClass = TRUE;
    }
    else {
        *pIsBaseClass = FALSE;
    }

    return NOERROR;
}

ECode CClassInfo::HasBaseClass(
    /* [out] */ Boolean * pHasBaseClass)
{
    if (!pHasBaseClass) {
        return E_INVALID_ARGUMENT;
    }

    if (m_pDesc->dwAttribs & ClassAttrib_hasparent) {
        *pHasBaseClass = TRUE;
    }
    else {
        *pHasBaseClass = FALSE;
    }

    return NOERROR;
}

ECode CClassInfo::GetBaseClassInfo(
    /* [out] */ IClassInfo ** ppBaseClassInfo)
{
    if (!ppBaseClassInfo) {
        return E_INVALID_ARGUMENT;
    }

    if (!(m_pDesc->dwAttribs & ClassAttrib_hasparent)) {
        return E_DOES_NOT_EXIST;
    }

    *ppBaseClassInfo = NULL;
    return g_objInfoList.AcquireClassInfo(m_pCClsModule,
    	getClassDirAddr(m_nBase, m_pClsMod->ppClassDir, m_pDesc->sParentIndex),
        (IInterface **)ppBaseClassInfo);
}

ECode CClassInfo::IsGeneric(
    /* [out] */ Boolean * pIsGeneric)
{
    if (!pIsGeneric) {
        return E_INVALID_ARGUMENT;
    }

    if (m_pDesc->dwAttribs & ClassAttrib_t_generic) {
        *pIsGeneric = TRUE;
    }
    else {
        *pIsGeneric = FALSE;
    }

    return NOERROR;
}

ECode CClassInfo::HasGeneric(
    /* [out] */ Boolean * pbHasGeneric)
{
    if (!pbHasGeneric) {
        return E_INVALID_ARGUMENT;
    }
    *pbHasGeneric = FALSE;

    ECode ec = NOERROR;
    AutoPtr<IClassInfo> pParentClassInfo;
    AutoPtr<IClassInfo> pThisClassInfo = (IClassInfo*)this->Probe(EIID_IClassInfo);
    Boolean bHasBaseClass = FALSE;
    while (TRUE) {
        ec = pThisClassInfo->HasBaseClass(&bHasBaseClass);
        if (FAILED(ec)) break;

        if (bHasBaseClass) {
            pParentClassInfo = NULL;
            ec = pThisClassInfo->GetBaseClassInfo((IClassInfo**)&pParentClassInfo);
            if (FAILED(ec)) break;

            ec = pParentClassInfo->IsGeneric(pbHasGeneric);
            if (FAILED(ec)) break;

            if (*pbHasGeneric) {
                m_pGenericInfo = pParentClassInfo;
                m_pGenericInfo->AddRef();
                break;
            }
            else {
                pThisClassInfo = pParentClassInfo;
            }
        }
        else {
            break;
        }
    }

    return ec;
}

ECode CClassInfo::GetGenericInfo(
    /* [out] */ IClassInfo ** ppGenericInfo)
{
    if (!ppGenericInfo) {
        return E_INVALID_ARGUMENT;
    }

    *ppGenericInfo = m_pGenericInfo;
    if (*ppGenericInfo) (*ppGenericInfo)->AddRef();

    return NOERROR;
}

ECode CClassInfo::IsRegime(
    /* [out] */ Boolean * pIsRegime)
{
    if (!pIsRegime) {
        return E_INVALID_ARGUMENT;
    }

    if (m_pDesc->dwAttribs & ClassAttrib_t_regime) {
        *pIsRegime = TRUE;
    }
    else {
        *pIsRegime = FALSE;
    }

    return NOERROR;
}

ECode CClassInfo::GetAspectCount(
    /* [out] */ Int32 * pCount)
{
    if (!pCount) {
        return E_INVALID_ARGUMENT;
    }

    *pCount = m_pDesc->cAspects;
    return NOERROR;
}

ECode CClassInfo::AcquireAspectList()
{
    ECode ec = NOERROR;
    g_objInfoList.LockHashTable(EntryType_Aspect);
    if (!m_pAspectList) {
        m_pAspectList = new CEntryList(EntryType_Aspect,
            m_pDesc, m_pDesc->cAspects, m_pCClsModule);
        if (m_pAspectList)
            m_pAspectList->AddRef();
        else
            ec = E_OUT_OF_MEMORY;
    }
    g_objInfoList.UnlockHashTable(EntryType_Aspect);

    return ec;
}

ECode CClassInfo::GetAllAspectInfos(
    /* [out] */ BufferOf<IClassInfo *> * pAspectInfos)
{
    ECode ec = AcquireAspectList();
    if (FAILED(ec)) return ec;

    return m_pAspectList->GetAllObjInfos((PTypeInfos)pAspectInfos);
}

ECode CClassInfo::GetAspectInfo(
    /* [in] */ CString name,
    /* [out] */ IClassInfo ** ppAspectInfo)
{
    if (name.IsNull() || !ppAspectInfo) {
        return E_INVALID_ARGUMENT;
    }

    if (!m_pDesc->cAspects) {
        return E_DOES_NOT_EXIST;
    }

    ECode ec = AcquireAspectList();
    if (FAILED(ec)) return ec;

    return m_pAspectList->AcquireObjByName(name, (IInterface **)ppAspectInfo);
}

ECode CClassInfo::IsAspect(
    /* [out] */ Boolean * pIsAspect)
{
    if (!pIsAspect) {
        return E_INVALID_ARGUMENT;
    }

    if (m_pDesc->dwAttribs & ClassAttrib_t_aspect) {
        *pIsAspect = TRUE;
    }
    else {
        *pIsAspect = FALSE;
    }

    return NOERROR;
}

ECode CClassInfo::GetAggregateeCount(
    /* [out] */ Int32 * pCount)
{
    if (!pCount) {
        return E_INVALID_ARGUMENT;
    }

    *pCount = m_pDesc->cAggregates;
    return NOERROR;
}

ECode CClassInfo::AcquireAggregateeList()
{
    ECode ec = NOERROR;
    g_objInfoList.LockHashTable(EntryType_Aggregatee);
    if (!m_pAggregateeList) {
        m_pAggregateeList = new CEntryList(EntryType_Aggregatee,
                m_pDesc, m_pDesc->cAggregates, m_pCClsModule);
        if (m_pAggregateeList)
            m_pAggregateeList->AddRef();
        else
            ec = E_OUT_OF_MEMORY;
    }
    g_objInfoList.UnlockHashTable(EntryType_Aggregatee);

    return ec;
}

ECode CClassInfo::GetAllAggregateeInfos(
    /* [out] */ BufferOf<IClassInfo *> * pAggregateeInfos)
{
    ECode ec = AcquireAggregateeList();
    if (FAILED(ec)) return ec;

    return m_pAggregateeList->GetAllObjInfos((PTypeInfos)pAggregateeInfos);
}

ECode CClassInfo::GetAggregateeInfo(
    /* [in] */ CString name,
    /* [out] */ IClassInfo ** ppAggregateeInfo)
{
    if (name.IsNull() || !ppAggregateeInfo) {
        return E_INVALID_ARGUMENT;
    }

    if (!m_pDesc->cAggregates) {
        return E_DOES_NOT_EXIST;
    }

    ECode ec = AcquireAggregateeList();
    if (FAILED(ec)) return ec;

    return m_pAggregateeList->AcquireObjByName(name,
        (IInterface **)ppAggregateeInfo);
}

ECode CClassInfo::AcquireConstructorList()
{
    if (!(m_pDesc->dwAttribs & ClassAttrib_hasctor)) {
        return NOERROR;
    }

    ECode ec = NOERROR;
    g_objInfoList.LockHashTable(EntryType_Class);
    if (!m_pCtorClassInfo) {
        AutoPtr<IModuleInfo> pModuleInfo;

        ec = m_pCClsModule->GetModuleInfo((IModuleInfo**)&pModuleInfo);
        if (FAILED(ec)) {
            g_objInfoList.UnlockHashTable(EntryType_Class);
            return ec;
        }

        StringBuf_<_MAX_PATH> clsName;
        clsName << adjustNameAddr(m_nBase, m_pClassDirEntry->pszName)
                << "ClassObject";

        ec = pModuleInfo->GetClassInfo(clsName, (IClassInfo**)&m_pCtorClassInfo);
    }
    g_objInfoList.UnlockHashTable(EntryType_Class);

    if (FAILED(ec)) return ec;

    return m_pCtorClassInfo->AcquireSpecialMethodList(
                EntryType_Constructor, &m_pCtorList);
}

ECode CClassInfo::GetConstructorCount(
    /* [out] */ Int32 * pCount)
{
    if (!pCount) {
        return E_INVALID_ARGUMENT;
    }

    if (!(m_pDesc->dwAttribs & ClassAttrib_hasctor)) {
        *pCount = 0;
    }
    else {
        ECode ec = AcquireConstructorList();
        if (FAILED(ec)) return ec;

        *pCount = m_pCtorList->m_uTotalCount;
    }

    return NOERROR;
}

ECode CClassInfo::GetAllConstructorInfos(
    /* [out] */ BufferOf<IConstructorInfo *> * pConstructorInfos)
{
    if (!pConstructorInfos) {
        return E_INVALID_ARGUMENT;
    }

    if (!(m_pDesc->dwAttribs & ClassAttrib_hasctor)) {
        pConstructorInfos->SetUsed(0);
        return NOERROR;
    }

    ECode ec = AcquireConstructorList();
    if (FAILED(ec)) return ec;

    ec = m_pCtorList->GetAllObjInfos((PTypeInfos)pConstructorInfos);
    if (FAILED(ec)) return ec;

    Int32 used = pConstructorInfos->GetUsed();
    for(Int32 i = 0; i < used;i++) {
        CConstructorInfo *pCObj = (CConstructorInfo*)(*pConstructorInfos)[i];
        pCObj->m_instClsId.clsid = this->m_clsId.clsid;
        strcpy(pCObj->m_instClsId.pUunm, this->m_clsId.pUunm);
    }

    return NOERROR;
}

ECode CClassInfo::GetConstructorInfoByParamNames(
    /* [in] */ CString name,
    /* [out] */ IConstructorInfo **ppConstructorInfo)
{
    if (name.IsNull() || !ppConstructorInfo) {
        return E_INVALID_ARGUMENT;
    }

    if (!(m_pDesc->dwAttribs & ClassAttrib_hasctor)) {
        return E_DOES_NOT_EXIST;
    }

    ECode ec = AcquireConstructorList();
    if (FAILED(ec)) return ec;

    StringBuf_<_MAX_PATH> objName;
    objName.Copy("CreateObjectWith");
    objName.Append(name);

    ec = m_pCtorList->AcquireObjByName(objName,
            (IInterface **)ppConstructorInfo);
    if (FAILED(ec)) return ec;

    CConstructorInfo* pCObj = (CConstructorInfo*)(*ppConstructorInfo);
    pCObj->m_instClsId.clsid = this->m_clsId.clsid;
    strcpy(pCObj->m_instClsId.pUunm, this->m_clsId.pUunm);

    return NOERROR;
}

ECode CClassInfo::GetConstructorInfoByParamCount(
    /* [in] */ Int32 count,
    /* [out] */ IConstructorInfo **ppConstructorInfo)
{
    if (!ppConstructorInfo) {
        return E_INVALID_ARGUMENT;
    }
    *ppConstructorInfo = NULL;
    if (count < 0) {
        return E_INVALID_ARGUMENT;
    }

    if (!(m_pDesc->dwAttribs & ClassAttrib_hasctor)) {
        return E_DOES_NOT_EXIST;
    }

    ECode ec = AcquireConstructorList();
    if (FAILED(ec)) return ec;

    AutoPtr<IConstructorInfo> pConstructorInfo;

    Int32 iCount = 0;
    for (UInt32 i = 0; i < m_pCtorList->m_uTotalCount; i++) {
        pConstructorInfo = NULL;
        ec = m_pCtorList->AcquireObjByIndex(i, (IInterface **)&pConstructorInfo);
        if (FAILED(ec)) return ec;
        ec = pConstructorInfo->GetParamCount(&iCount);
        if (FAILED(ec)) {
            return ec;
        }
        if (iCount == count) {
            if (NULL == *ppConstructorInfo) {
                *ppConstructorInfo = pConstructorInfo;
                (*ppConstructorInfo)->AddRef();
            }
            else {
                (*ppConstructorInfo)->Release();
                *ppConstructorInfo = NULL;
                return E_OUT_OF_NUMBER;
            }
        }
    }

    if (*ppConstructorInfo == NULL) {
        return E_DOES_NOT_EXIST;
    }

    CConstructorInfo* pCObj = (CConstructorInfo*)(*ppConstructorInfo);
    pCObj->m_instClsId.clsid = this->m_clsId.clsid;
    strcpy(pCObj->m_instClsId.pUunm, this->m_clsId.pUunm);

    return NOERROR;
}

ECode CClassInfo::GetInterfaceCount(
    /* [out] */ Int32 * pCount)
{
    if (!pCount) {
        return E_INVALID_ARGUMENT;
    }

    *pCount = m_uIFCount;
    return NOERROR;
}

ECode CClassInfo::AcquireInterfaceList()
{
    ECode ec = NOERROR;
    g_objInfoList.LockHashTable(EntryType_ClassInterface);
    if (!m_pInterfaceList) {
        m_pInterfaceList = new CEntryList(EntryType_ClassInterface,
            m_pDesc, m_uIFCount, m_pCClsModule, m_pIFList, m_uIFCount);
        if (m_pInterfaceList)
            m_pInterfaceList->AddRef();
        else
            ec = E_OUT_OF_MEMORY;
    }
    g_objInfoList.UnlockHashTable(EntryType_ClassInterface);

    return ec;
}

ECode CClassInfo::GetAllInterfaceInfos(
    /* [out] */ BufferOf<IInterfaceInfo *> * pInterfaceInfos)
{
    ECode ec = AcquireInterfaceList();
    if (FAILED(ec)) return ec;

    return m_pInterfaceList->GetAllObjInfos((PTypeInfos)pInterfaceInfos);
}

ECode CClassInfo::GetInterfaceInfo(
    /* [in] */ CString name,
    /* [out] */ IInterfaceInfo ** ppInterfaceInfo)
{
    if (name.IsNull() || !ppInterfaceInfo) {
        return E_INVALID_ARGUMENT;
    }

    ECode ec = AcquireInterfaceList();
    if (FAILED(ec)) return ec;

    return m_pInterfaceList->AcquireObjByName(name,
        (IInterface **)ppInterfaceInfo);
}

ECode CClassInfo::GetCallbackInterfaceCount(
    /* [out] */ Int32 * pCount)
{
    if (!pCount) {
        return E_INVALID_ARGUMENT;
    }

    *pCount = m_uCBIFCount;
    return NOERROR;
}

ECode CClassInfo::AcquireCBInterfaceList()
{
    if (!m_pCBIFList) {
        return NOERROR;
    }

    ECode ec = NOERROR;
    g_objInfoList.LockHashTable(EntryType_ClassInterface);
    if (!m_pCBInterfaceList) {
        m_pCBInterfaceList = new CEntryList(EntryType_ClassInterface,
            m_pDesc, m_uCBIFCount, m_pCClsModule, m_pCBIFList, m_uCBIFCount);
        if (m_pCBInterfaceList)
            m_pCBInterfaceList->AddRef();
        else
            ec = E_OUT_OF_MEMORY;
    }
    g_objInfoList.UnlockHashTable(EntryType_ClassInterface);

    return ec;
}

ECode CClassInfo::GetAllCallbackInterfaceInfos(
    /* [out] */ BufferOf<IInterfaceInfo *> * pCallbackInterfaceInfos)
{
    ECode ec = AcquireCBInterfaceList();
    if (FAILED(ec)) return ec;

    return m_pCBInterfaceList->GetAllObjInfos((PTypeInfos)pCallbackInterfaceInfos);
}

ECode CClassInfo::GetCallbackInterfaceInfo(
    /* [in] */ CString name,
    /* [out] */ IInterfaceInfo ** ppCallbackInterfaceInfo)
{
    if (name.IsNull() || !ppCallbackInterfaceInfo) {
        return E_INVALID_ARGUMENT;
    }

    if (!m_pCBIFList) {
        return E_DOES_NOT_EXIST;
    }

    ECode ec = AcquireCBInterfaceList();
    if (FAILED(ec)) return ec;

    return m_pCBInterfaceList->AcquireObjByName(name,
        (IInterface **)ppCallbackInterfaceInfo);
}

ECode CClassInfo::GetMethodCount(
    /* [out] */ Int32 * pCount)
{
    if (!pCount) {
        return E_INVALID_ARGUMENT;
    }

    *pCount = m_uMethodCount;

    return NOERROR;
}

ECode CClassInfo::AcquireMethodList()
{
    return AcquireSpecialMethodList(EntryType_Method, &m_pMethodList);
}

ECode CClassInfo::AcquireSpecialMethodList(EntryType type, CEntryList **ppEntryList)
{
    ECode ec = NOERROR;

    g_objInfoList.LockHashTable(type);
    if (!*ppEntryList) {
        UInt32 uMethodCount = m_uMethodCount;
        if (type == EntryType_Constructor) {
            //delete functions of IInterface
            uMethodCount -= m_pIFList[0].pDesc->cMethods;

            //delete functions of IClassObject
            uMethodCount -= m_pIFList[1].pDesc->cMethods;
        }

        IFIndexEntry     *pIFList = NULL;
        UInt32            uListCount = 0;;

        if (type == EntryType_Constructor) {
            //the index of customer class object interface is 2
            pIFList = &m_pIFList[2];
            uListCount = m_uIFCount - 2;
        }
        else {
            pIFList = m_pIFList;
            uListCount = m_uIFCount;
        }

        *ppEntryList = new CEntryList(type,
            m_pDesc, uMethodCount, m_pCClsModule, pIFList, uListCount, this);
        if (*ppEntryList)
            (*ppEntryList)->AddRef();
        else
            ec = E_OUT_OF_MEMORY;
    }
    g_objInfoList.UnlockHashTable(type);

    return ec;
}

ECode CClassInfo::GetAllMethodInfos(
    /* [out] */ BufferOf<IMethodInfo *> * pMethodInfos)
{
    ECode ec = AcquireMethodList();
    if (FAILED(ec)) return ec;

    return m_pMethodList->GetAllObjInfos((PTypeInfos)pMethodInfos);
}

ECode CClassInfo::GetMethodInfo(
    /* [in] */ CString name,
    /* [out] */ IMethodInfo ** ppMethodInfo)
{
    if (name.IsNull() || !ppMethodInfo) {
        return E_INVALID_ARGUMENT;
    }

    if (!m_uMethodCount) {
        return E_DOES_NOT_EXIST;
    }

    ECode ec = AcquireMethodList();
    if (FAILED(ec)) return ec;

    return m_pMethodList->AcquireObjByName(name, (IInterface **)ppMethodInfo);
}

ECode CClassInfo::GetCallbackMethodCount(
    /* [out] */ Int32 * pCount)
{
    if (!pCount) {
        return E_INVALID_ARGUMENT;
    }

    *pCount = m_uCBMethodCount;

    return NOERROR;
}

ECode CClassInfo::AcquireCBMethodList()
{
    ECode ec = NOERROR;
    g_objInfoList.LockHashTable(EntryType_CBMethod);
    if (!m_pCBMethodList) {
        m_pCBMethodList = new CEntryList(EntryType_CBMethod,
            m_pClassDirEntry->pDesc, m_uCBMethodCount, m_pCClsModule,
            m_pCBIFList, m_uCBIFCount, this);
        if (m_pCBMethodList)
            m_pCBMethodList->AddRef();
        else
            ec = E_OUT_OF_MEMORY;
    }
    g_objInfoList.UnlockHashTable(EntryType_CBMethod);

    return ec;
}

ECode CClassInfo::GetAllCallbackMethodInfos(
    /* [out] */ BufferOf<ICallbackMethodInfo*> * pCallbackMethodInfos)
{
    ECode ec =AcquireCBMethodList();
    if (FAILED(ec)) return ec;

    return m_pCBMethodList->GetAllObjInfos((PTypeInfos)pCallbackMethodInfos);
}

ECode CClassInfo::GetCallbackMethodInfo(
    /* [in] */ CString name,
    /* [out] */ ICallbackMethodInfo ** ppCallbackMethodInfo)
{
    if (name.IsNull() || !ppCallbackMethodInfo) {
        return E_INVALID_ARGUMENT;
    }

    if (!m_uCBMethodCount) {
        return E_DOES_NOT_EXIST;
    }

    ECode ec =AcquireCBMethodList();
    if (FAILED(ec)) return ec;

    return m_pCBMethodList->AcquireObjByName(name, (IInterface **)ppCallbackMethodInfo);
}

ECode CClassInfo::RemoveAllCallbackHandlers(
    /* [in] */ PInterface server)
{
    return _CObject_RemoveAllCallbacks(server);
}

ECode CClassInfo::CreateObjInRgm(
    /* [in] */ PRegime rgm,
    /* [out] */ PInterface * pObject)
{
    int iIndex = getCIFAddr(m_nBase, m_pDesc->ppInterfaces, 0)->sIndex;
    InterfaceDirEntry* pIFDir = getInterfaceDirAddr(m_nBase,
            m_pClsMod->ppInterfaceDir, iIndex);
    EIID iid = adjustInterfaceDescAddr(m_nBase, pIFDir->pDesc)->iid;

    return _CObject_CreateInstance(m_clsId, rgm, iid, pObject);
}

ECode CClassInfo::CreateObject(
    /* [out] */ PInterface * pObject)
{
    if (!pObject) {
        return E_INVALID_ARGUMENT;
    }

    return CreateObjInRgm(RGM_SAME_DOMAIN, pObject);
}

ECode CClassInfo::CreateObjectInRegime(
    /* [in] */ PRegime rgm,
    /* [out] */ PInterface * pObject)
{
    if (IS_INVALID_REGIME(rgm) || !pObject) {
        return E_INVALID_ARGUMENT;
    }

    return CreateObjInRgm(rgm, pObject);
}

ECode CClassInfo::CreateIFList()
{
    if (m_pIFList) {
        return NOERROR;
    }

    UInt32 *pIndexList =
        (UInt32 *)alloca(m_pClsMod->cInterfaces * sizeof(UInt32));
    if (pIndexList == NULL) {
        return E_OUT_OF_MEMORY;
    }

    IFIndexEntry *pAllIFList = (IFIndexEntry *)
                 alloca(m_pClsMod->cInterfaces * sizeof(IFIndexEntry));
    if (pAllIFList == NULL) {
        return E_OUT_OF_MEMORY;
    }

    int i, j, k, n = 0, iNo, uListCount = 0;
    UInt32 uIndex = 0, uEventNum = 1, uBeginNo = METHOD_START_NO;
    Boolean bCallBack = FALSE;
    m_uIFCount = 0;
    m_uCBIFCount = 0;
    ClassInterface* pCIFDir = NULL;
    InterfaceDirEntry* pIFDir = NULL;

    for (i = 0; i < m_pDesc->cInterfaces; i++) {
        pCIFDir = getCIFAddr(m_nBase, m_pDesc->ppInterfaces, i);
        if (pCIFDir->wAttribs & ClassInterfaceAttrib_callback) {
            bCallBack = TRUE;
        }
        else {
            bCallBack = FALSE;
        }

        uIndex = pCIFDir->sIndex;

        iNo = 0;
        while (uIndex != 0) {
            pIndexList[iNo++] = uIndex;
            pIFDir = getInterfaceDirAddr(m_nBase,
                    m_pClsMod->ppInterfaceDir, uIndex);
            uIndex = adjustInterfaceDescAddr(m_nBase, pIFDir->pDesc)->sParentIndex;
        }

        pIndexList[iNo] = 0;

        //Save the pIndexList to m_pIFList
        for (j = iNo; j >= 0; j--) {
            uIndex = pIndexList[j];
            if (uListCount != 0) {
                //If the same inteface in list, continue
                for (k = 0; k < uListCount; k++) {
                    if (pAllIFList[k].uIndex == uIndex) {
                        uBeginNo = pAllIFList[k].uBeginNo
                                    + pAllIFList[k].pDesc->cMethods;
                        if (!bCallBack) {
                            if (!(pAllIFList[k].attribs & IFAttrib_normal)) {
                                m_uIFCount++;
                                pAllIFList[k].attribs |= IFAttrib_normal;
                            }
                        }
                        else {
                            if (!(pAllIFList[k].attribs & IFAttrib_callback)) {
                                m_uCBIFCount++;
                                pAllIFList[k].attribs |= IFAttrib_callback;
                            }
                        }

                        break;
                    }
                }

                if (k < uListCount) {
                    continue;
                }
            }

            pAllIFList[uListCount].uIndex = uIndex;
            pAllIFList[uListCount].uBeginNo = uBeginNo;
            pIFDir = getInterfaceDirAddr(m_nBase, m_pClsMod->ppInterfaceDir, uIndex);
            pAllIFList[uListCount].pszName = adjustNameAddr(m_nBase, pIFDir->pszName);
            pAllIFList[uListCount].pDesc = adjustInterfaceDescAddr(m_nBase,
                    pIFDir->pDesc);

            if (!bCallBack) {
                m_uIFCount++;
                pAllIFList[uListCount].attribs = IFAttrib_normal;
            }
            else {
                m_uCBIFCount++;
                pAllIFList[uListCount].attribs = IFAttrib_callback;
            }
            uBeginNo +=  pAllIFList[uListCount].pDesc->cMethods;

            uListCount++;
        }
    }

    m_pIFList = new IFIndexEntry[m_uIFCount];
    if (!m_pIFList) goto EExit;

    if (m_uCBIFCount) {
        m_uCBIFCount--;
        m_pCBIFList = new IFIndexEntry[m_uCBIFCount];
        if (!m_pCBIFList) goto EExit;
    }

    m_uMethodCount = 0;
    m_uCBMethodCount = 0;
    j = 0;
    k = 0;
    for (i = 0; i < uListCount; i++) {
        if (pAllIFList[i].attribs & IFAttrib_normal) {
            memcpy(&m_pIFList[j], &pAllIFList[i], sizeof(IFIndexEntry));
            m_uMethodCount += pAllIFList[i].pDesc->cMethods;
            j++;
        }
        if (i && m_uCBIFCount && (pAllIFList[i].attribs & IFAttrib_callback)) {
            memcpy(&m_pCBIFList[k], &pAllIFList[i], sizeof(IFIndexEntry));
            m_uCBMethodCount += pAllIFList[i].pDesc->cMethods;
            k++;
        }
    }

    //Set Callback Method Info
    m_pCBMethodDesc = new CBMethodDesc[m_uCBMethodCount];
    if (!m_pCBMethodDesc) goto EExit;
    memset(m_pCBMethodDesc, 0, m_uCBMethodCount * sizeof(CBMethodDesc));

    for (i = 0; i < k; i++) {
        for (j = 0; j < m_pCBIFList[i].pDesc->cMethods; j++) {
            m_pCBMethodDesc[n].pDesc = getMethodDescAddr(m_nBase,
                        m_pCBIFList[i].pDesc->ppMethods, j);
            m_pCBMethodDesc[n].uIndex = MK_METHOD_INDEX(m_pCBIFList[i].uIndex,
                                        m_pCBIFList[i].uBeginNo + j);
            m_pCBMethodDesc[n].uEventNum = uEventNum;
            uEventNum++;
            n++;
        }
        uEventNum += 2;
    }

    return NOERROR;

EExit:
    m_uIFCount = 0;
    m_uCBIFCount = 0;
    m_uMethodCount = 0;
    m_uCBMethodCount = 0;

    if (m_pIFList) {
        delete [] m_pIFList;
        m_pIFList = NULL;
    }

    if (m_pCBIFList) {
        delete [] m_pCBIFList;
        m_pCBIFList = NULL;
    }

    if (m_pCBMethodDesc) {
        delete [] m_pCBMethodDesc;
        m_pCBMethodDesc = NULL;
    }

    return E_OUT_OF_MEMORY;
}
