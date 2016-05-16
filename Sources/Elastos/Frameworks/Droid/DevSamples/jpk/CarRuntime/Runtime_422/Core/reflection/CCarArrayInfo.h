//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#ifndef __CCARARRAYINFO_H__
#define __CCARARRAYINFO_H__

#include "CClsModule.h"

class CCarArrayRefInfo : public ICarArrayInfo
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

    CARAPI CreateVariable(
        /* [in] */ Int32 capacity,
        /* [out] */ IVariableOfCarArray ** ppVariable);

    CARAPI CreateVariableBox(
        /* [in] */ PCarQuintet variableDescriptor,
        /* [out] */ IVariableOfCarArray ** ppVariable);

    CARAPI GetMaxAlignSize(
        /* [out] */ MemorySize * pAlignSize);

    CCarArrayRefInfo(
        /* [in] */ CarDataType quintetType,
        /* [in] */ IDataTypeInfo *pElementTypeInfo,
        /* [in] */ CarDataType dataType = CarDataType_Struct);

    virtual ~CCarArrayRefInfo();

private:
    CarQuintetFlag  m_carQuintetFlag;

    IDataTypeInfo  *m_pElementTypeInfo;
    CarDataType     m_elementDataType;
    CarDataType     m_quintetType;

    Int32 m_cRef;
};

#endif // __CCARARRAYINFO_H__
