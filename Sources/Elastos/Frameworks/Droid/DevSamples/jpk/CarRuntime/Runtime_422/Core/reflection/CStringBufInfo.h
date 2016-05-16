//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#ifndef __CSTRINGBUFINFO_H__
#define __CSTRINGBUFINFO_H__

#include <elastos.h>

class CStringBufInfo : public IStringBufInfo
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

    CARAPI CreateVariable(
        /* [in] */ Int32 capacity,
        /* [out] */ IVariableOfStringBuf ** ppVariableBox);

    CARAPI CreateVariableBox(
        /* [in] */ PCarQuintet variableDescriptor,
        /* [out] */ IVariableOfStringBuf ** ppVariableBox);

    CStringBufInfo():m_cRef(0){}

private:
    Int32 m_cRef;
};

#endif // __CSTRINGBUFINFO_H__
