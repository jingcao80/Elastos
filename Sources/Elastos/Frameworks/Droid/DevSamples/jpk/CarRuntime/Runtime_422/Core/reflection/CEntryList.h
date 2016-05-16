//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#ifndef __CENTRYLIST_H__
#define __CENTRYLIST_H__

#include "CClsModule.h"
#include "CObjInfoList.h"

struct ObjElement
{
    UInt32   uIndex;
    void    *pDesc;
    char    *pszName;
    IInterface *pObj;
};

typedef enum IFAttrib
{
    IFAttrib_normal           = 0x01,
    IFAttrib_callback         = 0x02,
}IFAttrib;

struct IFIndexEntry
{
    UInt32               uIndex;
    UInt32               uBeginNo;
    char                *pszName;
    InterfaceDescriptor *pDesc;
    UInt32               attribs;
};

typedef BufferOf<IInterface *>*  PTypeInfos;

class CClassInfo;

class CEntryList
{
public:
    CARAPI_(UInt32) AddRef();
    CARAPI_(UInt32) Release();

    CARAPI AcquireObjByName(
        /* [in] */ CString name,
        /* [out] */ IInterface ** ppObject);

    CARAPI AcquireObjByIndex(
        /* [in] */ UInt32 uIndex,
        /* [out] */ IInterface ** ppObject);

    CARAPI InitElemList();

    CARAPI GetAllObjInfos(
        /* [out] */ BufferOf<IInterface *>* pObjInfos);

    CEntryList():m_cRef(0){}

    CEntryList(
        /* [in] */ EntryType type,
        /* [in] */ void *pDesc,
        /* [in] */ UInt32 uTotalCount,
        /* [in] */ CClsModule *pCClsModule,
        /* [in] */ IFIndexEntry *pIFList = NULL,
        /* [in] */ UInt32 uListCount = 0,
        /* [in] */ CClassInfo *pClsInfo = NULL);

    ~CEntryList();

    CClsModule       *m_pCClsModule;
    UInt32            m_uTotalCount;

private:
    CLSModule        *m_pClsMod;
    void             *m_pDesc;
    EntryType         m_type;
    CClassInfo       *m_pClsInfo;
    ObjElement       *m_pObjElement;
    Int32             m_nBase;

    IFIndexEntry     *m_pIFList;
    UInt32            m_uListCount;

    HashTable<UInt32, Type_String> m_pHTIndexs;

    Int32             m_cRef;
};

#endif // __CENTRYLIST_H__
