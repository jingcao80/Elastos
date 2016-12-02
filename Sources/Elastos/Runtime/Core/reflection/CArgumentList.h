//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#ifndef __CARGLIST_H__
#define __CARGLIST_H__

#include "refutil.h"

class CArgumentList
    : public ElLightRefBase
    , public IArgumentList
{
public:
    CArgumentList();

    virtual ~CArgumentList();

    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI_(UInt32) AddRef();

    CARAPI_(UInt32) Release();

    CARAPI GetInterfaceID(
        /* [in] */ IInterface* object,
        /* [out] */ InterfaceID* iid);

    CARAPI Init(
        /* [in] */ IFunctionInfo* functionInfo,
        /* [in] */ ParmElement* paramElem,
        /* [in] */ UInt32 paramCount,
        /* [in] */ UInt32 paramBufSize,
        /* [in] */ Boolean methodInfo);

    CARAPI GetFunctionInfo(
        /* [out] */ IFunctionInfo** functionInfo);

    CARAPI GetInputArgumentOfInt16(
        /* [in] */ Int32 index,
        /* [out] */ Int16* value);

    CARAPI SetInputArgumentOfInt16(
        /* [in] */ Int32 index,
        /* [in] */ Int16 value);

    CARAPI GetInputArgumentOfInt32(
        /* [in] */ Int32 index,
        /* [out] */ Int32* value);

    CARAPI SetInputArgumentOfInt32(
        /* [in] */ Int32 index,
        /* [in] */ Int32 value);

    CARAPI GetInputArgumentOfInt64(
        /* [in] */ Int32 index,
        /* [out] */ Int64* value);

    CARAPI SetInputArgumentOfInt64(
        /* [in] */ Int32 index,
        /* [in] */ Int64 value);

    CARAPI GetInputArgumentOfByte(
        /* [in] */ Int32 index,
        /* [out] */ Byte* value);

    CARAPI SetInputArgumentOfByte(
        /* [in] */ Int32 index,
        /* [in] */ Byte value);

    CARAPI GetInputArgumentOfFloat(
        /* [in] */ Int32 index,
        /* [out] */ Float* value);

    CARAPI SetInputArgumentOfFloat(
        /* [in] */ Int32 index,
        /* [in] */ Float value);

    CARAPI GetInputArgumentOfDouble(
        /* [in] */ Int32 index,
        /* [out] */ Double* value);

    CARAPI SetInputArgumentOfDouble(
        /* [in] */ Int32 index,
        /* [in] */ Double value);

    CARAPI GetInputArgumentOfChar(
        /* [in] */ Int32 index,
        /* [out] */ Char32* value);

    CARAPI SetInputArgumentOfChar(
        /* [in] */ Int32 index,
        /* [in] */ Char32 value);

    CARAPI GetInputArgumentOfString(
        /* [in] */ Int32 index,
        /* [out] */ String* value);

    CARAPI SetInputArgumentOfString(
        /* [in] */ Int32 index,
        /* [in] */ const String& value);

    CARAPI GetInputArgumentOfBoolean(
        /* [in] */ Int32 index,
        /* [out] */ Boolean* value);

    CARAPI SetInputArgumentOfBoolean(
        /* [in] */ Int32 index,
        /* [in] */ Boolean value);

    CARAPI GetInputArgumentOfEMuid(
        /* [in] */ Int32 index,
        /* [out] */ EMuid* value);

    CARAPI SetInputArgumentOfEMuid(
        /* [in] */ Int32 index,
        /* [in] */ EMuid* value);

    CARAPI GetInputArgumentOfEGuid(
        /* [in] */ Int32 index,
        /* [out] */ EGuid* value);

    CARAPI SetInputArgumentOfEGuid(
        /* [in] */ Int32 index,
        /* [in] */ EGuid* value);

    CARAPI GetInputArgumentOfECode(
        /* [in] */ Int32 index,
        /* [out] */ ECode* value);

    CARAPI SetInputArgumentOfECode(
        /* [in] */ Int32 index,
        /* [in] */ ECode value);

    CARAPI SetInputArgumentOfLocalPtr(
        /* [in] */ Int32 index,
        /* [in] */ LocalPtr value);

    CARAPI SetInputArgumentOfLocalType(
        /* [in] */ Int32 index,
        /* [in] */ PVoid value);

    CARAPI GetInputArgumentOfEnum(
        /* [in] */ Int32 index,
        /* [out] */ Int32* value);

    CARAPI SetInputArgumentOfEnum(
        /* [in] */ Int32 index,
        /* [in] */ Int32 value);

    CARAPI GetInputArgumentOfCarArray(
        /* [in] */ Int32 index,
        /* [out] */ PCarQuintet* value);

    CARAPI SetInputArgumentOfCarArray(
        /* [in] */ Int32 index,
        /* [in] */ PCarQuintet value);

    CARAPI SetInputArgumentOfStructPtr(
        /* [in] */ Int32 index,
        /* [in] */ PVoid value);

    CARAPI GetInputArgumentOfObjectPtr(
        /* [in] */ Int32 index,
        /* [out] */ PInterface* value);

    CARAPI SetInputArgumentOfObjectPtr(
        /* [in] */ Int32 index,
        /* [in] */ PInterface value);

    CARAPI GetOutputArgumentOfInt16Ptr(
        /* [in] */ Int32 index,
        /* [out] */ PVoid* value);

    CARAPI SetOutputArgumentOfInt16Ptr(
        /* [in] */ Int32 index,
        /* [out] */ Int16* value);

    CARAPI GetOutputArgumentOfInt32Ptr(
        /* [in] */ Int32 index,
        /* [out] */ PVoid* value);

    CARAPI SetOutputArgumentOfInt32Ptr(
        /* [in] */ Int32 index,
        /* [out] */ Int32* value);

    CARAPI GetOutputArgumentOfInt64Ptr(
        /* [in] */ Int32 index,
        /* [out] */ PVoid* value);

    CARAPI SetOutputArgumentOfInt64Ptr(
        /* [in] */ Int32 index,
        /* [out] */ Int64* value);

    CARAPI GetOutputArgumentOfBytePtr(
        /* [in] */ Int32 index,
        /* [out] */ PVoid* value);

    CARAPI SetOutputArgumentOfBytePtr(
        /* [in] */ Int32 index,
        /* [out] */ Byte* value);

    CARAPI GetOutputArgumentOfFloatPtr(
        /* [in] */ Int32 index,
        /* [out] */ PVoid* value);

    CARAPI SetOutputArgumentOfFloatPtr(
        /* [in] */ Int32 index,
        /* [out] */ Float* value);

    CARAPI GetOutputArgumentOfDoublePtr(
        /* [in] */ Int32 index,
        /* [out] */ PVoid* value);

    CARAPI SetOutputArgumentOfDoublePtr(
        /* [in] */ Int32 index,
        /* [out] */ Double* value);

    CARAPI GetOutputArgumentOfCharPtr(
        /* [in] */ Int32 index,
        /* [out] */ PVoid* value);

    CARAPI SetOutputArgumentOfCharPtr(
        /* [in] */ Int32 index,
        /* [out] */ Char32* value);

    CARAPI GetOutputArgumentOfStringPtr(
        /* [in] */ Int32 index,
        /* [out] */ PVoid* value);

    CARAPI SetOutputArgumentOfStringPtr(
        /* [in] */ Int32 index,
        /* [out] */ String* value);

    CARAPI GetOutputArgumentOfBooleanPtr(
        /* [in] */ Int32 index,
        /* [out] */ PVoid* value);

    CARAPI SetOutputArgumentOfBooleanPtr(
        /* [in] */ Int32 index,
        /* [out] */ Boolean* value);

    CARAPI GetOutputArgumentOfEMuidPtr(
        /* [in] */ Int32 index,
        /* [out] */ PVoid* value);

    CARAPI SetOutputArgumentOfEMuidPtr(
        /* [in] */ Int32 index,
        /* [out] */ EMuid* value);

    CARAPI GetOutputArgumentOfEGuidPtr(
        /* [in] */ Int32 index,
        /* [out] */ PVoid* value);

    CARAPI SetOutputArgumentOfEGuidPtr(
        /* [in] */ Int32 index,
        /* [out] */ EGuid* value);

    CARAPI GetOutputArgumentOfECodePtr(
        /* [in] */ Int32 index,
        /* [out] */ PVoid* value);

    CARAPI SetOutputArgumentOfECodePtr(
        /* [in] */ Int32 index,
        /* [out] */ ECode* value);

    CARAPI SetOutputArgumentOfLocalPtrPtr(
        /* [in] */ Int32 index,
        /* [out] */ LocalPtr* value);

    CARAPI SetOutputArgumentOfLocalTypePtr(
        /* [in] */ Int32 index,
        /* [out] */ PVoid value);

    CARAPI GetOutputArgumentOfEnumPtr(
        /* [in] */ Int32 index,
        /* [out] */ PVoid* value);

    CARAPI SetOutputArgumentOfEnumPtr(
        /* [in] */ Int32 index,
        /* [out] */ Int32* value);

    CARAPI GetOutputArgumentOfCarArrayPtr(
        /* [in] */ Int32 index,
        /* [out] */ PCarQuintet* value);

    CARAPI SetOutputArgumentOfCarArrayPtr(
        /* [in] */ Int32 index,
        /* [out] */ PCarQuintet value);

    CARAPI GetOutputArgumentOfCarArrayPtrPtr(
        /* [in] */ Int32 index,
        /* [out] */ Handle32* value);

    CARAPI SetOutputArgumentOfCarArrayPtrPtr(
        /* [in] */ Int32 index,
        /* [out] */ PCarQuintet* value);

    CARAPI SetOutputArgumentOfStructPtr(
        /* [in] */ Int32 index,
        /* [out] */ PVoid value);

    CARAPI SetOutputArgumentOfStructPtrPtr(
        /* [in] */ Int32 index,
        /* [out] */ PVoid* value);

    CARAPI GetOutputArgumentOfObjectPtrPtr(
        /* [in] */ Int32 index,
        /* [out] */ Handle32* value);

    CARAPI SetOutputArgumentOfObjectPtrPtr(
        /* [in] */ Int32 index,
        /* [out] */ PInterface* value);

    CARAPI SetParamValue(
        /* [in] */ Int32 index,
        /* [in] */ PVoid param,
        /* [in] */ CarDataType type,
        /* [in] */ ParamIOAttribute attrib,
        /* [in] */ Int32 pointer = 0);

public:
    Byte*           mParamBuf;

private:
    ParmElement*    mParamElem;
    UInt32          mParamCount;
    UInt32          mParamBufSize;
    AutoPtr<IFunctionInfo>  mFunctionInfo;
    Boolean         mIsMethodInfo;

    HashTable<IInterface *> mInterfaceParams;
};

#endif // __CARGLIST_H__
