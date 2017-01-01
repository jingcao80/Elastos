//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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

    CARAPI IsOutputArgumentNullPtr(
        /* [in] */ Int32 index,
        /* [out] */ Boolean* isNull);

    CARAPI AssignOutputArgumentOfInt16Ptr(
        /* [in] */ Int32 index,
        /* [in] */ Int16 value);

    CARAPI SetOutputArgumentOfInt16Ptr(
        /* [in] */ Int32 index,
        /* [out] */ Int16* value);

    CARAPI AssignOutputArgumentOfInt32Ptr(
        /* [in] */ Int32 index,
        /* [in] */ Int32 value);

    CARAPI SetOutputArgumentOfInt32Ptr(
        /* [in] */ Int32 index,
        /* [out] */ Int32* value);

    CARAPI AssignOutputArgumentOfInt64Ptr(
        /* [in] */ Int32 index,
        /* [in] */ Int64 value);

    CARAPI SetOutputArgumentOfInt64Ptr(
        /* [in] */ Int32 index,
        /* [out] */ Int64* value);

    CARAPI AssignOutputArgumentOfBytePtr(
        /* [in] */ Int32 index,
        /* [in] */ Byte value);

    CARAPI SetOutputArgumentOfBytePtr(
        /* [in] */ Int32 index,
        /* [out] */ Byte* value);

    CARAPI AssignOutputArgumentOfFloatPtr(
        /* [in] */ Int32 index,
        /* [in] */ Float value);

    CARAPI SetOutputArgumentOfFloatPtr(
        /* [in] */ Int32 index,
        /* [out] */ Float* value);

    CARAPI AssignOutputArgumentOfDoublePtr(
        /* [in] */ Int32 index,
        /* [in] */ Double value);

    CARAPI SetOutputArgumentOfDoublePtr(
        /* [in] */ Int32 index,
        /* [out] */ Double* value);

    CARAPI AssignOutputArgumentOfCharPtr(
        /* [in] */ Int32 index,
        /* [in] */ Char32 value);

    CARAPI SetOutputArgumentOfCharPtr(
        /* [in] */ Int32 index,
        /* [out] */ Char32* value);

    CARAPI AssignOutputArgumentOfStringPtr(
        /* [in] */ Int32 index,
        /* [in] */ const String& value);

    CARAPI SetOutputArgumentOfStringPtr(
        /* [in] */ Int32 index,
        /* [out] */ String* value);

    CARAPI AssignOutputArgumentOfBooleanPtr(
        /* [in] */ Int32 index,
        /* [in] */ Boolean value);

    CARAPI SetOutputArgumentOfBooleanPtr(
        /* [in] */ Int32 index,
        /* [out] */ Boolean* value);

    CARAPI AssignOutputArgumentOfEMuidPtr(
        /* [in] */ Int32 index,
        /* [in] */ EMuid* value);

    CARAPI SetOutputArgumentOfEMuidPtr(
        /* [in] */ Int32 index,
        /* [out] */ EMuid* value);

    CARAPI AssignOutputArgumentOfEGuidPtr(
        /* [in] */ Int32 index,
        /* [in] */ EGuid* value);

    CARAPI SetOutputArgumentOfEGuidPtr(
        /* [in] */ Int32 index,
        /* [out] */ EGuid* value);

    CARAPI AssignOutputArgumentOfECodePtr(
        /* [in] */ Int32 index,
        /* [in] */ ECode value);

    CARAPI SetOutputArgumentOfECodePtr(
        /* [in] */ Int32 index,
        /* [out] */ ECode* value);

    CARAPI SetOutputArgumentOfLocalPtrPtr(
        /* [in] */ Int32 index,
        /* [out] */ LocalPtr* value);

    CARAPI SetOutputArgumentOfLocalTypePtr(
        /* [in] */ Int32 index,
        /* [out] */ PVoid value);

    CARAPI AssignOutputArgumentOfEnumPtr(
        /* [in] */ Int32 index,
        /* [in] */ Int32 value);

    CARAPI SetOutputArgumentOfEnumPtr(
        /* [in] */ Int32 index,
        /* [out] */ Int32* value);

    CARAPI AssignOutputArgumentOfCarArrayPtr(
        /* [in] */ Int32 index,
        /* [in] */ PCarQuintet value);

    CARAPI SetOutputArgumentOfCarArrayPtr(
        /* [in] */ Int32 index,
        /* [out] */ PCarQuintet value);

    CARAPI AssignOutputArgumentOfCarArrayPtrPtr(
        /* [in] */ Int32 index,
        /* [in] */ PCarQuintet value);

    CARAPI SetOutputArgumentOfCarArrayPtrPtr(
        /* [in] */ Int32 index,
        /* [out] */ PCarQuintet* value);

    CARAPI SetOutputArgumentOfStructPtr(
        /* [in] */ Int32 index,
        /* [out] */ PVoid value);

    CARAPI SetOutputArgumentOfStructPtrPtr(
        /* [in] */ Int32 index,
        /* [out] */ PVoid* value);

    CARAPI AssignOutputArgumentOfObjectPtrPtr(
        /* [in] */ Int32 index,
        /* [in] */ PInterface value);

    CARAPI SetOutputArgumentOfObjectPtrPtr(
        /* [in] */ Int32 index,
        /* [out] */ PInterface* value);

    CARAPI SetParamValue(
        /* [in] */ Int32 index,
        /* [in] */ PVoid param,
        /* [in] */ CarDataType type,
        /* [in] */ ParamIOAttribute attrib,
        /* [in] */ Int32 pointer = 0);

    CARAPI GetParamValue(
        /* [in] */ Int32 index,
        /* [in] */ CarDataType type,
        /* [in] */ ParamIOAttribute attrib,
        /* [in] */ Int32 pointer,
        /* [in, out] */ PVoid param);

    CARAPI AssignOutputParamValue(
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
