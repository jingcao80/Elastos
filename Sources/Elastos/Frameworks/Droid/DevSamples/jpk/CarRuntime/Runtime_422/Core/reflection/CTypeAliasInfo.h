//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#ifndef __CTYPEALIASINFO_H__
#define __CTYPEALIASINFO_H__

#include "CClsModule.h"

class CTypeAliasInfo : public ITypeAliasInfo
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

    CARAPI GetTypeInfo(
        /* [out] */ IDataTypeInfo ** ppTypeInfo);

    CARAPI GetModuleInfo(
        /* [out] */ IModuleInfo ** ppModuleInfo);

    CARAPI IsDummy(
        /* [out] */ Boolean * pIsDummy);

    CARAPI GetPtrLevel(
        /* [out] */ Int32 *pLevel);

    CTypeAliasInfo(
        /* [in] */ CClsModule * pCClsModule,
        /* [in] */ AliasDirEntry *pAliasDirEntry);

    virtual ~CTypeAliasInfo();

private:
    CClsModule     *m_pCClsModule;
    AliasDirEntry  *m_pAliasDirEntry;

    Int32 m_cRef;
};

#endif // __CTYPEALIASINFO_H__
