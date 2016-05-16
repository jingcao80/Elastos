//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#ifndef __CMODULEINFO_H__
#define __CMODULEINFO_H__

#include "CEntryList.h"

class CModuleRefInfo : public IModuleInfo
{
public:
    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI_(UInt32) AddRef();

    CARAPI_(UInt32) Release();

    CARAPI GetInterfaceID(
        /* [in] */ IInterface *pObject,
        /* [out] */ InterfaceID *pIID);

    CARAPI GetPath(
        /* [out] */ StringBuf * pPath);

    CARAPI GetVersion(
        /* [out] */ Int32 * pMajor,
        /* [out] */ Int32 * pMinor,
        /* [out] */ Int32 * pBuild,
        /* [out] */ Int32 * pRevision);

    CARAPI GetClassCount(
        /* [out] */ Int32 * pCount);

    CARAPI GetAllClassInfos(
        /* [out] */ BufferOf<IClassInfo *> * pClassInfos);

    CARAPI GetClassInfo(
        /* [in] */ CString name,
        /* [out] */ IClassInfo ** ppClassInfo);

    CARAPI GetInterfaceCount(
        /* [out] */ Int32 * pCount);

    CARAPI GetAllInterfaceInfos(
        /* [out] */ BufferOf<IInterfaceInfo *> * pInterfaceInfos);

    CARAPI GetInterfaceInfo(
        /* [in] */ CString name,
        /* [out] */ IInterfaceInfo ** ppInterfaceInfo);

    CARAPI GetStructCount(
        /* [out] */ Int32 * pCount);

    CARAPI GetAllStructInfos(
        /* [out] */ BufferOf<IStructInfo *> * pStructInfos);

    CARAPI GetStructInfo(
        /* [in] */ CString name,
        /* [out] */ IStructInfo ** ppStructInfo);

    CARAPI GetEnumCount(
        /* [out] */ Int32 * pCount);

    CARAPI GetAllEnumInfos(
        /* [out] */ BufferOf<IEnumInfo *> * pEnumInfos);

    CARAPI GetEnumInfo(
        /* [in] */ CString name,
        /* [out] */ IEnumInfo ** ppEnumInfo);

    CARAPI GetTypeAliasCount(
        /* [out] */ Int32 * pCount);

    CARAPI GetAllTypeAliasInfos(
        /* [out] */ BufferOf<ITypeAliasInfo *> * pTypeAliasInfos);

    CARAPI GetTypeAliasInfo(
        /* [in] */ CString name,
        /* [out] */ ITypeAliasInfo ** ppTypeAliasInfo);

    CARAPI GetConstantCount(
        /* [out] */ Int32 * pCount);

    CARAPI GetAllConstantInfos(
        /* [out] */ BufferOf<IConstantInfo *> * pConstantInfos);

    CARAPI GetConstantInfo(
        /* [in] */ CString name,
        /* [out] */ IConstantInfo ** ppConstantInfo);

    CARAPI GetImportModuleInfoCount(
        /* [out] */ Int32 * pCount);

    CARAPI GetAllImportModuleInfos(
        /* [out] */ BufferOf<IModuleInfo *> * pModuleInfos);

    CARAPI AcquireClassList();

    CARAPI AcquireInterfaceList();

    CARAPI AcquireStructList();

    CARAPI AcquireEnumList();

    CARAPI AcquireAliasList();

    CARAPI AcquireConstantList();

    CModuleRefInfo(
        /* [in] */ CClsModule * pCClsModule,
        /* [in] */ CString path);

    virtual ~CModuleRefInfo();

    CClsModule  *m_pCClsModule;
    CLSModule   *m_pClsMod;

private:
    CEntryList  *m_pClassList;
    CEntryList  *m_pInterfaceList;
    CEntryList  *m_pStructList;
    CEntryList  *m_pEnumList;
    CEntryList  *m_pAliasList;
    CEntryList  *m_pConstantList;

    StringBuf_<_MAX_PATH>  m_sbPath;
    Int32       m_iAliasCount;

    CRITICAL_SECTION  m_lockList;

    Int32 m_cRef;
};

#endif // __CMODULEINFO_H__
