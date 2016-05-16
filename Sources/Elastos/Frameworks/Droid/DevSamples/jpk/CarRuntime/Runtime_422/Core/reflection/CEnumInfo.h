//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#ifndef __CENUMINFO_H__
#define __CENUMINFO_H__

#include "CEntryList.h"

class CEnumRefInfo : public IEnumInfo
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

    CARAPI GetModuleInfo(
        /* [out] */ IModuleInfo ** ppModuleInfo);

    CARAPI GetItemCount(
        /* [out] */ Int32 * pCount);

    CARAPI GetAllItemInfos(
        /* [out] */ BufferOf<IEnumItemInfo *> * pItemInfos);

    CARAPI GetItemInfo(
        /* [in] */ CString name,
        /* [out] */ IEnumItemInfo ** ppEnumItemInfo);

    CARAPI InitItemInfos();

    CARAPI InitStatic(
        /* [in] */ CClsModule * pCClsModule,
        /* [in] */ EnumDirEntry *pEnumDirEntry);

    CARAPI InitDynamic(
        /* [in] */ CString name,
        /* [in] */ const BufferOf<CString>& itemNames,
        /* [in] */ const BufferOf<Int32>& itemValues);

    CEnumRefInfo();

    virtual ~CEnumRefInfo();

    BufferOf<CString>     *m_pItemNames;
    BufferOf<Int32>      *m_pItemValues;

private:
    CClsModule      *m_pCClsModule;
    EnumDirEntry    *m_pEnumDirEntry;
    BufferOf<IEnumItemInfo *>     *m_pItemInfos;

    char            *m_pName;
    char            *m_pBuf;

    Int32            m_cRef;
};

#endif // __CENUMINFO_H__
