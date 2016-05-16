//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#ifndef __CFIELDINFO_H__
#define __CFIELDINFO_H__

#include "refutil.h"

class CFieldInfo : public IFieldInfo
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

    CFieldInfo(
        /* [in] */ IStructInfo * pStructInfo,
        /* [in] */ CString name,
        /* [in] */ IDataTypeInfo *pTypeInfo);

    virtual ~CFieldInfo();

private:
    IStructInfo    *m_pStructInfo;
    String         m_sName;
    IDataTypeInfo  *m_pTypeInfo;

    Int32          m_cRef;
};

#endif // __CFIELDINFO_H__
