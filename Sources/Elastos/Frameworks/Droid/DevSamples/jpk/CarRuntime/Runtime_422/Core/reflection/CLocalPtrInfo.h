//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#ifndef __CLOCALPTRINFO_H__
#define __CLOCALPTRINFO_H__

#include "CClsModule.h"

class CLocalPtrInfo : public ILocalPtrInfo
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

    CARAPI GetSize(
        /* [out] */ MemorySize * pSize);

    CARAPI GetDataType(
        /* [out] */ CarDataType * pDataType);

    CARAPI GetTargetTypeInfo(
        /* [out] */ IDataTypeInfo ** ppDateTypeInfo);

    CARAPI GetPtrLevel(
        /* [out] */ Int32 *pLevel);

    CLocalPtrInfo(
        /* [in] */ CClsModule * pCClsModule,
        /* [in] */ TypeDescriptor *pTypeDescriptor,
        /* [in] */ Int32 iPointer);

    virtual ~CLocalPtrInfo();

private:
    CClsModule      *m_pCClsModule;
    TypeDescriptor  *m_pTypeDescriptor;
    Int32           m_iPointer;

    Int32           m_cRef;
};

#endif // __CLOCALPTRINFO_H__
