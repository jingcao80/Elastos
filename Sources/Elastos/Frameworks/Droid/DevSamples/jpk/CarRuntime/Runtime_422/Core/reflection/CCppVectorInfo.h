//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#ifndef __CCPPVECTORINFO_H__
#define __CCPPVECTORINFO_H__

#include "CClsModule.h"

class CCppVectorRefInfo : public ICppVectorInfo
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

    CARAPI GetElementTypeInfo(
        /* [out] */ IDataTypeInfo ** ppElementTypeInfo);

    CARAPI GetLength(
        /* [out] */ Int32 * pLength);

    CARAPI GetMaxAlignSize(
        /* [out] */ MemorySize * pAlignSize);

    CCppVectorRefInfo(
        /* [in] */ IDataTypeInfo *pElementTypeInfo,
        /* [in] */ Int32 length);

    virtual ~CCppVectorRefInfo();

private:

    IDataTypeInfo  *m_pElementTypeInfo;
    Int32          m_iLength;
    Int32          m_iSize;

    Int32          m_cRef;
};

#endif // __CCPPVECTORINFO_H__
