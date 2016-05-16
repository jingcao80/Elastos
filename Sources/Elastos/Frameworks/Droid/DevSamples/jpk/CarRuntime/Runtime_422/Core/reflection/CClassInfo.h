//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#ifndef __CCLASSINFO_H__
#define __CCLASSINFO_H__

#include "CEntryList.h"

struct CBMethodDesc
{
    MethodDescriptor *pDesc;
    UInt32 uIndex;
    UInt32 uEventNum;
};

class CClassInfo : public IClassInfo
{
public:
    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI_(UInt32) AddRef();

    CARAPI_(UInt32) Release();

    CARAPI GetInterfaceID(
        /* [in] */ IInterface *pObject,
        /* [out] */ InterfaceID *pIID);

    CARAPI GetName(
        /* [out] */ StringBuf * pName);

    CARAPI GetId(
        /* [out] */ ClassID * pclsid);

    CARAPI GetModuleInfo(
        /* [out] */ IModuleInfo ** ppModuleInfo);

    CARAPI IsSingleton(
        /* [out] */ Boolean * pIsSingleton);

    CARAPI GetThreadingModel(
        /* [out] */ ThreadingModel *pThreadingModel);

    CARAPI IsPrivate(
        /* [out] */ Boolean * pIsPrivate);

    CARAPI IsReturnValue(
        /* [out] */ Boolean * pIsReturnValue);

    CARAPI IsBaseClass(
        /* [out] */ Boolean * pIsBaseClass);

    CARAPI HasBaseClass(
        /* [out] */ Boolean * pHasBaseClass);

    CARAPI GetBaseClassInfo(
        /* [out] */ IClassInfo ** ppBaseClassInfo);

    CARAPI IsGeneric(
        /* [out] */ Boolean * pIsGeneric);

    CARAPI HasGeneric(
        /* [out] */ Boolean * pHasGeneric);

    CARAPI GetGenericInfo(
        /* [out] */ IClassInfo ** ppGenericInfo);

    CARAPI IsRegime(
        /* [out] */ Boolean * pIsRegime);

    CARAPI GetAspectCount(
        /* [out] */ Int32 * pCount);

    CARAPI GetAllAspectInfos(
        /* [out] */ BufferOf<IClassInfo *> * pAspectInfos);

    CARAPI GetAspectInfo(
        /* [in] */ CString name,
        /* [out] */ IClassInfo ** ppAspectInfo);

    CARAPI IsAspect(
        /* [out] */ Boolean * pIsAspect);

    CARAPI GetAggregateeCount(
        /* [out] */ Int32 * pCount);

    CARAPI GetAllAggregateeInfos(
        /* [out] */ BufferOf<IClassInfo *> * pAggregateeInfos);

    CARAPI GetAggregateeInfo(
        /* [in] */ CString name,
        /* [out] */ IClassInfo ** ppAggregateeInfo);

    CARAPI GetConstructorCount(
        /* [out] */ Int32 * pCount);

    CARAPI GetAllConstructorInfos(
        /* [out] */ BufferOf<IConstructorInfo *> * pConstructorInfos);

    CARAPI GetConstructorInfoByParamNames(
        /* [in] */ CString name,
        /* [out] */ IConstructorInfo **ppConstructorInfo);

    CARAPI GetConstructorInfoByParamCount(
        /* [in] */ Int32 count,
        /* [out] */ IConstructorInfo **ppConstructorInfo);

    CARAPI GetInterfaceCount(
        /* [out] */ Int32 * pCount);

    CARAPI GetAllInterfaceInfos(
        /* [out] */ BufferOf<IInterfaceInfo *> * pInterfaceInfos);

    CARAPI GetInterfaceInfo(
        /* [in] */ CString name,
        /* [out] */ IInterfaceInfo ** ppInterfaceInfo);

    CARAPI GetCallbackInterfaceCount(
        /* [out] */ Int32 * pCount);

    CARAPI GetAllCallbackInterfaceInfos(
        /* [out] */ BufferOf<IInterfaceInfo *> * pCallbackInterfaceInfos);

    CARAPI GetCallbackInterfaceInfo(
        /* [in] */ CString name,
        /* [out] */ IInterfaceInfo ** ppCallbackInterfaceInfo);

    CARAPI GetMethodCount(
        /* [out] */ Int32 * pCount);

    CARAPI GetAllMethodInfos(
        /* [out] */ BufferOf<IMethodInfo *> * pMethodInfos);

    CARAPI GetMethodInfo(
        /* [in] */ CString name,
        /* [out] */ IMethodInfo ** ppMethodInfo);

    CARAPI GetCallbackMethodCount(
        /* [out] */ Int32 * pCount);

    CARAPI GetAllCallbackMethodInfos(
        /* [out] */ BufferOf<ICallbackMethodInfo *> * pCallbackMethodInfos);

    CARAPI GetCallbackMethodInfo(
        /* [in] */ CString name,
        /* [out] */ ICallbackMethodInfo ** ppCallbackMethodInfo);

    CARAPI RemoveAllCallbackHandlers(
        /* [in] */ PInterface server);

    CARAPI Init();

    CARAPI CreateObject(
        /* [out] */ PInterface * pObject);

    CARAPI CreateObjectInRegime(
        /* [in] */ PRegime rgm,
        /* [out] */ PInterface * pObject);

    CARAPI CreateIFList();

    CARAPI AcquireSpecialMethodList(EntryType type, CEntryList **ppEntryList);

    CARAPI AcquireAggregateeList();
    CARAPI AcquireAspectList();
    CARAPI AcquireMethodList();
    CARAPI AcquireInterfaceList();
    CARAPI AcquireCBInterfaceList();
    CARAPI AcquireConstructorList();
    CARAPI AcquireCBMethodList();

    CARAPI CreateObjInRgm(
        /* [in] */ PRegime rgm,
        /* [out] */ PInterface * pObject);

    CClassInfo(
        /* [in] */ CClsModule * pCClsModule,
        /* [in] */ ClassDirEntry *pClsDirEntry);

    virtual ~CClassInfo();

    ClassDirEntry   *m_pClassDirEntry;
    ClassID          m_clsId;

    IFIndexEntry    *m_pCBIFList;
    IFIndexEntry    *m_pIFList;
    CBMethodDesc    *m_pCBMethodDesc;

    UInt32           m_uIFCount;
    UInt32           m_uCBIFCount;

private:
    CClsModule      *m_pCClsModule;
    CLSModule       *m_pClsMod;
    ClassDescriptor *m_pDesc;
    Int32            m_nBase;

    char   m_szUrn[_MAX_PATH];



    UInt32 m_uMethodCount;
    UInt32 m_uCBMethodCount;
    UInt32 m_uCtorCount;

    CEntryList *m_pAspectList;
    CEntryList *m_pAggregateeList;
    CEntryList *m_pMethodList;
    CEntryList *m_pCBInterfaceList;
    CEntryList *m_pInterfaceList;
    CEntryList *m_pCtorList;
    CEntryList *m_pCBMethodList;

    CClassInfo *m_pCtorClassInfo;
    IClassInfo *m_pGenericInfo;

    Int32      m_cRef;
};

#endif // __CCLASSINFO_H__
