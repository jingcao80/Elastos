//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#ifndef __CINTERFACEINFO_H__
#define __CINTERFACEINFO_H__

#include "CEntryList.h"

class CInterfaceInfo : public IInterfaceInfo
{
public:
    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI_(UInt32) AddRef();

    CARAPI_(UInt32) Release();

    CARAPI GetInterfaceID(
        /* [in] */ IInterface *pObject,
        /* [out] */ InterfaceID *pIID);

    CARAPI GetSize(
        /* [out] */ MemorySize * pSize);

    CARAPI GetDataType(
        /* [out] */ CarDataType * pDataType);

    CARAPI GetName(
        /* [out] */ StringBuf * pName);

    CARAPI GetId(
        /* [out] */ InterfaceID * pId);

    CARAPI GetModuleInfo(
        /* [out] */ IModuleInfo ** ppModuleInfo);

    CARAPI IsLocal(
        /* [out] */ Boolean * pLocal);

    CARAPI HasBase(
        /* [out] */ Boolean * pHasBase);

    CARAPI GetBaseInfo(
        /* [out] */ IInterfaceInfo ** ppBaseInfo);

    CARAPI GetMethodCount(
        /* [out] */ Int32 * pCount);

    CARAPI GetAllMethodInfos(
        /* [out] */ BufferOf<IMethodInfo *> * pMethodInfos);

    CARAPI GetMethodInfo(
        /* [in] */ CString name,
        /* [out] */ IMethodInfo ** ppMethodInfo);

    CARAPI CreateIFList();

    CARAPI AcquireMethodList();

    CARAPI Init();

    CInterfaceInfo(
        /* [in] */ CClsModule * pCClsModule,
        /* [in] */ UInt32 uIndex);

    virtual ~CInterfaceInfo();

private:
    CClsModule          *m_pCClsModule;
    CLSModule           *m_pClsMod;
    CEntryList          *m_pMethodList;
    InterfaceDirEntry   *m_pInterfaceDirEntry;
    InterfaceDescriptor *m_pDesc;
    IFIndexEntry        *m_pIFList;

    UInt32  m_uIndex;
    UInt32  m_uIFCount;
    UInt32  m_uMethodCount;
    Int32   m_nBase;

    Int32   m_cRef;
};

#endif // __CINTERFACEINFO_H__
