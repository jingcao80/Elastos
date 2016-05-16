//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#ifndef __CCONSTANTINFO_H__
#define __CCONSTANTINFO_H__

#include "CClsModule.h"

class CConstantInfo : public IConstantInfo
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

    CARAPI GetValue(
        /* [out] */ Int32 * pValue);

    CARAPI GetModuleInfo(
        /* [out] */ IModuleInfo ** ppModuleInfo);

    CConstantInfo(
        /* [in] */ CClsModule * pCClsModule,
        /* [in] */ ConstDirEntry  *pConstDirEntry);

    virtual ~CConstantInfo();

private:
    CClsModule     *m_pCClsModule;
    ConstDirEntry  *m_pConstDirEntry;

    Int32 m_cRef;
};

#endif // __CCONSTANTINFO_H__
