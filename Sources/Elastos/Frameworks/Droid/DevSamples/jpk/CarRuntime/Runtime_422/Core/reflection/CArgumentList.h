//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#ifndef __CARGLIST_H__
#define __CARGLIST_H__

#include "refutil.h"

class CArgumentList : public IArgumentList
{
public:
    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI_(UInt32) AddRef();

    CARAPI_(UInt32) Release();

    CARAPI GetInterfaceID(
        /* [in] */ IInterface *pObject,
        /* [out] */ InterfaceID *pIID);

    CARAPI GetFunctionInfo(
        /* [out] */ IFunctionInfo ** ppFunctionInfo);

    CARAPI SetInputArgumentOfInt16(
        /* [in] */ Int32 index,
        /* [in] */ Int16 value);

    CARAPI SetInputArgumentOfInt32(
        /* [in] */ Int32 index,
        /* [in] */ Int32 value);

    CARAPI SetInputArgumentOfInt64(
        /* [in] */ Int32 index,
        /* [in] */ Int64 value);

    CARAPI SetInputArgumentOfByte(
        /* [in] */ Int32 index,
        /* [in] */ Byte value);

    CARAPI SetInputArgumentOfFloat(
        /* [in] */ Int32 index,
        /* [in] */ Float value);

    CARAPI SetInputArgumentOfDouble(
        /* [in] */ Int32 index,
        /* [in] */ Double value);

    CARAPI SetInputArgumentOfChar8(
        /* [in] */ Int32 index,
        /* [in] */ Char8 value);

    CARAPI SetInputArgumentOfChar16(
        /* [in] */ Int32 index,
        /* [in] */ Char16 value);

    CARAPI SetInputArgumentOfChar32(
        /* [in] */ Int32 index,
        /* [in] */ Char32 value);

    CARAPI SetInputArgumentOfCString(
        /* [in] */ Int32 index,
        /* [in] */ CString value);

    CARAPI SetInputArgumentOfString(
        /* [in] */ Int32 index,
        /* [in] */ const String& value);

    CARAPI SetInputArgumentOfBoolean(
        /* [in] */ Int32 index,
        /* [in] */ Boolean value);

    CARAPI SetInputArgumentOfEMuid(
        /* [in] */ Int32 index,
        /* [in] */ EMuid * pValue);

    CARAPI SetInputArgumentOfEGuid(
        /* [in] */ Int32 index,
        /* [in] */ EGuid * pValue);

    CARAPI SetInputArgumentOfECode(
        /* [in] */ Int32 index,
        /* [in] */ ECode value);

    CARAPI SetInputArgumentOfLocalPtr(
        /* [in] */ Int32 index,
        /* [in] */ LocalPtr value);

    CARAPI SetInputArgumentOfLocalType(
        /* [in] */ Int32 index,
        /* [in] */ PVoid value);

    CARAPI SetInputArgumentOfEnum(
        /* [in] */ Int32 index,
        /* [in] */ Int32 value);

    CARAPI SetInputArgumentOfCarArray(
        /* [in] */ Int32 index,
        /* [in] */ PCarQuintet value);

    CARAPI SetInputArgumentOfStructPtr(
        /* [in] */ Int32 index,
        /* [in] */ PVoid value);

    CARAPI SetInputArgumentOfObjectPtr(
        /* [in] */ Int32 index,
        /* [in] */ PInterface value);

    CARAPI SetOutputArgumentOfInt16Ptr(
        /* [in] */ Int32 index,
        /* [out] */ Int16 * pValue);

    CARAPI SetOutputArgumentOfInt32Ptr(
        /* [in] */ Int32 index,
        /* [out] */ Int32 * pValue);

    CARAPI SetOutputArgumentOfInt64Ptr(
        /* [in] */ Int32 index,
        /* [out] */ Int64 * pValue);

    CARAPI SetOutputArgumentOfBytePtr(
        /* [in] */ Int32 index,
        /* [out] */ Byte * pValue);

    CARAPI SetOutputArgumentOfFloatPtr(
        /* [in] */ Int32 index,
        /* [out] */ Float * pValue);

    CARAPI SetOutputArgumentOfDoublePtr(
        /* [in] */ Int32 index,
        /* [out] */ Double * pValue);

    CARAPI SetOutputArgumentOfChar8Ptr(
        /* [in] */ Int32 index,
        /* [out] */ Char8 * pValue);

    CARAPI SetOutputArgumentOfChar16Ptr(
        /* [in] */ Int32 index,
        /* [out] */ Char16 * pValue);

    CARAPI SetOutputArgumentOfChar32Ptr(
        /* [in] */ Int32 index,
        /* [out] */ Char32 * pValue);

    CARAPI SetOutputArgumentOfStringPtr(
        /* [in] */ Int32 index,
        /* [out] */ String * pValue);

    CARAPI SetOutputArgumentOfBooleanPtr(
        /* [in] */ Int32 index,
        /* [out] */ Boolean * pValue);

    CARAPI SetOutputArgumentOfEMuidPtr(
        /* [in] */ Int32 index,
        /* [out] */ EMuid * pValue);

    CARAPI SetOutputArgumentOfEGuidPtr(
        /* [in] */ Int32 index,
        /* [out] */ EGuid * pValue);

    CARAPI SetOutputArgumentOfECodePtr(
        /* [in] */ Int32 index,
        /* [out] */ ECode * pValue);

    CARAPI SetOutputArgumentOfLocalPtrPtr(
        /* [in] */ Int32 index,
        /* [out] */ LocalPtr * pValue);

    CARAPI SetOutputArgumentOfLocalTypePtr(
        /* [in] */ Int32 index,
        /* [out] */ PVoid value);

    CARAPI SetOutputArgumentOfEnumPtr(
        /* [in] */ Int32 index,
        /* [out] */ Int32 * pValue);

    CARAPI SetOutputArgumentOfStringBufPtr(
        /* [in] */ Int32 index,
        /* [out] */ StringBuf * pvalue);

    CARAPI SetOutputArgumentOfStringBufPtrPtr(
        /* [in] */ Int32 index,
        /* [out] */ StringBuf ** pValue);

    CARAPI SetOutputArgumentOfCarArrayPtr(
        /* [in] */ Int32 index,
        /* [out] */ PCarQuintet value);

    CARAPI SetOutputArgumentOfCarArrayPtrPtr(
        /* [in] */ Int32 index,
        /* [out] */ PCarQuintet * pValue);

    CARAPI SetOutputArgumentOfStructPtr(
        /* [in] */ Int32 index,
        /* [out] */ PVoid value);

    CARAPI SetOutputArgumentOfStructPtrPtr(
        /* [in] */ Int32 index,
        /* [out] */ PVoid * pValue);

    CARAPI SetOutputArgumentOfObjectPtrPtr(
        /* [in] */ Int32 index,
        /* [out] */ PInterface * pValue);

    CARAPI SetParamValue(
        /* [in] */ Int32 index,
        /* [in] */ PVoid pParam,
        /* [in] */ CarDataType type,
        /* [in] */ ParamIOAttribute attrib,
        /* [in] */ Int32 iPointer = 0);

    CARAPI Init(
        /* [in] */ IFunctionInfo *pFunctionInfo,
        /* [in] */ ParmElement *pParamElem,
        /* [in] */ UInt32 uParamCount,
        /* [in] */ UInt32 uParamBufSize,
        /* [in] */ Boolean bMethodInfo);

    CArgumentList();
    virtual ~CArgumentList();

    Byte  *m_pParamBuf;

private:
    ParmElement    *m_pParamElem;
    UInt32          m_uParamCount;
    UInt32          m_uParamBufSize;
    IFunctionInfo  *m_pFunctionInfo;
    Boolean         m_bMethodInfo;

    Int32           m_cRef;
};

#endif // __CARGLIST_H__
