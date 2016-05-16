//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#ifndef __CVARIABLEOFSTRINGBUF_H__
#define __CVARIABLEOFSTRINGBUF_H__

#include <elastos.h>

_ELASTOS_NAMESPACE_USING

class CVariableOfStringBuf  :
    public IVariableOfStringBuf,
    public IStringBufSetter,
    public IStringBufGetter
{
public:
    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI_(UInt32) AddRef();

    CARAPI_(UInt32) Release();

    CARAPI GetInterfaceID(
        /* [in] */ IInterface *pObject,
        /* [out] */ InterfaceID *pIID);

    CARAPI GetTypeInfo(
        /* [out] */ IDataTypeInfo ** ppTypeInfo);

    CARAPI GetPayload(
        /* [out] */ PVoid * pPayload);

    CARAPI Rebox(
        /* [in] */ PVoid localVariablePtr);

    CARAPI GetSetter(
        /* [out] */ IStringBufSetter ** ppSetter);

    CARAPI GetGetter(
        /* [out] */ IStringBufGetter ** ppGetter);

    CARAPI SetElement(
        /* [in] */ Int32 index,
        /* [in] */ Char8 character);

    CARAPI SetData(
        /* [in] */ CString str);

    CARAPI GetCapacity(
        /* [out] */ Int32 * pCapacity);

    CARAPI GetLength(
        /* [out] */ Int32 * pLength);

    CARAPI GetElement(
        /* [in] */ Int32 index,
        /* [out] */ Char8 * pAch);

    CARAPI GetData(
        /* [out] */ StringBuf * sBuf);

    CVariableOfStringBuf(
        /* [in] */ PStringBuf psb,
        /* [in] */ Boolean bAlloc);

    virtual ~CVariableOfStringBuf();

private:
    PStringBuf     m_psb;
    Boolean         m_bAlloc;

    Int32          m_cRef;
};

#endif // __CVARIABLEOFSTRINGBUF_H__
