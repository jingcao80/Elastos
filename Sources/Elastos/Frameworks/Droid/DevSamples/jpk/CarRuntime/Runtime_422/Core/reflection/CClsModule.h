//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#ifndef __CCLSMODULE_H__
#define __CCLSMODULE_H__

#include "refutil.h"

class CClsModule
{
public:
    CARAPI_(UInt32) AddRef();
    CARAPI_(UInt32) Release();

    CClsModule():m_cRef(0){}

    CClsModule(CLSModule *pClsMod, Boolean bAllocedClsMod,
                    CString path, Void *pIModule);

    CARAPI GetModuleInfo(
        /* [out] */ IModuleInfo ** ppModuleInfo);

    CARAPI InitOrgType();

    CARAPI AliasToOriginal(
        /* [in] */ TypeDescriptor *pTypeDype,
        /* [out] */ TypeDescriptor **ppOrgTypeDesc);

    ~CClsModule();

    CLSModule*      m_pClsMod;
    Boolean         m_bAllocedClsMode;
    Int32           m_nBase;

private:
    TypeAliasDesc   *m_pTypeAliasList;
    StringBuf_<_MAX_PATH>  m_sbPath;
    Void         *m_pIModule;

    Int32           m_cRef;
};

#endif // __CCLSMODULE_H__
