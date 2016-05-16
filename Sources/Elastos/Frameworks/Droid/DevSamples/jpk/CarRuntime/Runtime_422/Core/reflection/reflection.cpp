//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#include "CEntryList.h"
#include "CModuleInfo.h"

CObjInfoList g_objInfoList;

ELAPI _CReflector_AcquireModuleInfo(
    /* [in] */ CString name,
    /* [out] */ IModuleInfo **ppModuleInfo)
{
    return g_objInfoList.AcquireModuleInfo(name, ppModuleInfo);
}

ELAPI _CReflector_AcquireIntrinsicTypeInfo(
    /* [in] */ CarDataType intrinsicType,
    /* [out] */ IDataTypeInfo **ppIntrinsicTypeInfo)
{
    return g_objInfoList.AcquireIntrinsicInfo(intrinsicType, ppIntrinsicTypeInfo);
}

ELAPI _CReflector_AcquireEnumInfo(
    /* [in] */ CString name,
    /* [in] */ const BufferOf<CString>* pItemNames,
    /* [in] */ const BufferOf<Int32>* pItemValues,
    /* [out] */ IEnumInfo **ppEnumInfo)
{
    return g_objInfoList.AcquireDynamicEnumInfo(name, *pItemNames,
                                                *pItemValues, ppEnumInfo);
}

ELAPI _CReflector_AcquireCppVectorInfo(
    /* [in] */ IDataTypeInfo *pElementTypeInfo,
    /* [in] */ Int32 length,
    /* [out] */ ICppVectorInfo **ppCppVectorInfo)
{
    return g_objInfoList.AcquireCppVectorInfo(pElementTypeInfo,
                                              length, ppCppVectorInfo);
}

ELAPI _CReflector_AcquireStructInfo(
    /* [in] */ CString name,
    /* [in] */ const BufferOf<CString> * pFieldNames,
    /* [in] */ const BufferOf<IDataTypeInfo *> * pFieldTypeInfos,
    /* [out] */ IStructInfo **ppStructInfo)
{
    return g_objInfoList.AcquireDynamicStructInfo(name, *pFieldNames,
                                                *pFieldTypeInfos, ppStructInfo);
}

ELAPI _CReflector_AcquireCarArrayInfo(
    /* [in] */ CarDataType quintetType,
    /* [in] */ IDataTypeInfo *pElementTypeInfo,
    /* [out] */ ICarArrayInfo **ppCarArrayInfo)
{
    return g_objInfoList.AcquireCarArrayInfo(quintetType, pElementTypeInfo,
                                                ppCarArrayInfo);
}

ELAPI _CObject_ReflectModuleInfo(
    /* [in] */ PInterface pObj,
    /* [out] */ IModuleInfo **ppModuleInfo)
{
    if (!pObj || !ppModuleInfo) {
        return E_INVALID_ARGUMENT;
    }

    ClassID clsid;
    IObject *pObject;
    ECode ec;

    pObject = (IObject*)pObj->Probe(EIID_IObject);
    if (pObject == NULL) return E_NO_INTERFACE;

    ec = pObject->GetClassID(&clsid);
    if (FAILED(ec)) {
        return E_NO_INTERFACE;
    }

    return _CReflector_AcquireModuleInfo(clsid.pUunm, ppModuleInfo);
}

ELAPI _CObject_ReflectClassInfo(
    /* [in] */ PInterface pObj,
    /* [out] */ IClassInfo **ppClassInfo)
{
    if (!pObj || !ppClassInfo) {
        return E_INVALID_ARGUMENT;
    }

    ClassID clsid;
    IObject *pObject;
    ECode ec;

    pObject = (IObject*)pObj->Probe(EIID_IObject);
    if (pObject == NULL) return E_NO_INTERFACE;

    ec = pObject->GetClassID(&clsid);
    if (FAILED(ec)) {
        return E_NO_INTERFACE;
    }

    AutoPtr<CModuleRefInfo> pModuleInfo;
    ec = g_objInfoList.AcquireModuleInfo(clsid.pUunm,
                                         (IModuleInfo **)&pModuleInfo);
    if (FAILED(ec)) {
        return ec;
    }

    ClassDirEntry* pClassDir = NULL;
    ClassDescriptor *pClsDesc = NULL;
    Int32 nBase = pModuleInfo->m_pCClsModule->m_nBase;

    *ppClassInfo = NULL;
    for (int i = 0; i < pModuleInfo->m_pClsMod->cClasses; i++) {
        pClassDir = getClassDirAddr(nBase, pModuleInfo->m_pClsMod->ppClassDir, i);
        pClsDesc = adjustClassDescAddr(nBase, pClassDir->pDesc);
        if (pClsDesc->clsid == clsid.clsid) {
            ec = g_objInfoList.AcquireClassInfo(pModuleInfo->m_pCClsModule,
                                      pClassDir, (IInterface **)ppClassInfo);
            return ec;
        }
    }

    return E_DOES_NOT_EXIST;
}

ELAPI _CObject_ReflectInterfaceInfo(
    /* [in] */ PInterface pObj,
    /* [out] */ IInterfaceInfo **ppInterfaceInfo)
{
    if (!pObj || !ppInterfaceInfo) {
        return E_INVALID_ARGUMENT;
    }

    ClassID clsid;
    EIID iid;
    IObject *pObject;
    ECode ec;

    ec = pObj->GetInterfaceID(pObj, &iid);
    if (FAILED(ec)) return E_INVALID_ARGUMENT;

    pObject = (IObject*)pObj->Probe(EIID_IObject);
    if (pObject == NULL) return E_NO_INTERFACE;

    ec = pObject->GetClassID(&clsid);
    if (FAILED(ec)) return E_INVALID_ARGUMENT;

    AutoPtr<CModuleRefInfo> pModuleInfo;
    ec = _CReflector_AcquireModuleInfo(clsid.pUunm, (IModuleInfo **)&pModuleInfo);
    if (FAILED(ec)) {
        return ec;
    }

    ClassDirEntry* pClassDir = NULL;
    ClassDescriptor *pClsDesc = NULL;
    ClassInterface* pCIFDir = NULL;
    InterfaceDirEntry* pIFDir = NULL;
    InterfaceDescriptor *pIFDesc = NULL;
    unsigned short   sIndex = 0;
    Int32 nBase = pModuleInfo->m_pCClsModule->m_nBase;

    *ppInterfaceInfo = NULL;
    for (int i = 0; i < pModuleInfo->m_pClsMod->cClasses; i++) {
        pClassDir = getClassDirAddr(nBase, pModuleInfo->m_pClsMod->ppClassDir, i);
        pClsDesc = adjustClassDescAddr(nBase, pClassDir->pDesc);
        //find the class
        if (pClsDesc->clsid == clsid.clsid) {
            for (int j = 0; j < pClsDesc->cInterfaces; j++) {
                pCIFDir = getCIFAddr(nBase, pClsDesc->ppInterfaces, j);
                sIndex = pCIFDir->sIndex;
                pIFDir = getInterfaceDirAddr(nBase,
                                pModuleInfo->m_pClsMod->ppInterfaceDir, sIndex);
                pIFDesc = adjustInterfaceDescAddr(nBase, pIFDir->pDesc);
                //find the interface
                if (pIFDesc->iid == iid) {
                    ec = g_objInfoList.AcquireInterfaceInfo(
                                pModuleInfo->m_pCClsModule,
                                sIndex,
                                (IInterface **)ppInterfaceInfo);
                    return ec;
                }
            }
        }
    }

    return E_DOES_NOT_EXIST;
}
