//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#include "CArgumentList.h"
#include "CMethodInfo.h"
#include "CConstructorInfo.h"

CArgumentList::CArgumentList()
    : mParamBuf(NULL)
    , mParamElem(NULL)
    , mParamCount(0)
    , mParamBufSize(0)
    , mIsMethodInfo(FALSE)
{}

CArgumentList::~CArgumentList()
{
    // TODO
    if (!mInterfaceParams.IsEmpty() && mParamCount > 0) {
        for (UInt32 i = 0; i < mParamCount; ++i) {
            if (mParamElem[i].mType == CarDataType_Interface) {
                IInterface** prev = mInterfaceParams.Get(&i);
                if (prev && *prev) {
                    (*prev)->Release();
                }
            }
        }
    }

    if (mParamBuf) free(mParamBuf);
}

UInt32 CArgumentList::AddRef()
{
    return ElLightRefBase::AddRef();
}

UInt32 CArgumentList::Release()
{
    return ElLightRefBase::Release();
}

PInterface CArgumentList::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)this;
    }
    else if (riid == EIID_IArgumentList) {
        return (IArgumentList *)this;
    }
    else {
        return NULL;
    }
}

ECode CArgumentList::GetInterfaceID(
    /* [in] */ IInterface* object,
    /* [out] */ InterfaceID* iid)
{
    return E_NOT_IMPLEMENTED;
}

ECode CArgumentList::Init(
    /* [in] */ IFunctionInfo* functionInfo,
    /* [in] */ ParmElement* paramElem,
    /* [in] */ UInt32 paramCount,
    /* [in] */ UInt32 paramBufSize,
    /* [in] */ Boolean isMethodInfo)
{
    mParamElem = paramElem;
    mParamCount = paramCount;

    mParamBuf = (PByte)malloc(paramBufSize);
    if (mParamBuf == NULL) {
        return E_OUT_OF_MEMORY;
    }

    mParamBufSize = paramBufSize;
    memset(mParamBuf, 0, paramBufSize);
    mFunctionInfo = functionInfo;

    mIsMethodInfo = isMethodInfo;

    return NOERROR;
}

ECode CArgumentList::GetFunctionInfo(
    /* [out] */ IFunctionInfo** functionInfo)
{
    if (!functionInfo) {
        return E_INVALID_ARGUMENT;
    }
    *functionInfo = mFunctionInfo;
    (*functionInfo)->AddRef();
    return NOERROR;
}

ECode CArgumentList::SetParamValue(
    /* [in] */ Int32 index,
    /* [in] */ PVoid param,
    /* [in] */ CarDataType type,
    /* [in] */ ParamIOAttribute attrib,
    /* [in] */ Int32 pointer)
{
    if (type == CarDataType_CarArray
        && mParamElem[index].mType == CarDataType_ArrayOf) {
        type = mParamElem[index].mType;
    }

    if (index < 0 || index >= (Int32)mParamCount
        || (mParamElem[index].mAttrib != attrib)
        || (type != mParamElem[index].mType)
        || (type != CarDataType_LocalPtr
        && mParamElem[index].mPointer != pointer)) {
        return E_INVALID_ARGUMENT;
    }

    if (!mParamBuf || mParamElem[index].mPos
        + mParamElem[index].mSize > mParamBufSize) {
        return E_INVALID_OPERATION;
    }

    if (mParamElem[index].mSize == 1) {
        *(Byte *)(mParamBuf + mParamElem[index].mPos) = *(Byte *)param;
    }
    else if (mParamElem[index].mSize == 2) {
        *(UInt16 *)(mParamBuf + mParamElem[index].mPos) = *(UInt16 *)param;
    }
    else if (mParamElem[index].mSize == 4) {
        if (type == CarDataType_String) {
            *(String **)(mParamBuf + mParamElem[index].mPos) = (String *)param;
        }
        else {
            *(UInt32 *)(mParamBuf + mParamElem[index].mPos) = *(UInt32 *)param;
        }
    }
    else if (mParamElem[index].mSize == 8) {
        *(UInt64 *)(mParamBuf + mParamElem[index].mPos) = *(UInt64 *)param;
    }
    else {
        return E_INVALID_OPERATION;
    }

    return NOERROR;
}

ECode CArgumentList::GetInputArgumentOfInt16(
    /* [in] */ Int32 index,
    /* [out] */ Int16* value)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CArgumentList::SetInputArgumentOfInt16(
    /* [in] */ Int32 index,
    /* [in] */ Int16 value)
{
    UInt32 nParam = value;
    return SetParamValue(index, &nParam, CarDataType_Int16, ParamIOAttribute_In);
}

ECode CArgumentList::GetInputArgumentOfInt32(
    /* [in] */ Int32 index,
    /* [in] */ Int32* value)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CArgumentList::SetInputArgumentOfInt32(
    /* [in] */ Int32 index,
    /* [in] */ Int32 value)
{
    return SetParamValue(index, &value, CarDataType_Int32, ParamIOAttribute_In);
}

ECode CArgumentList::GetInputArgumentOfInt64(
    /* [in] */ Int32 index,
    /* [out] */ Int64* value)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CArgumentList::SetInputArgumentOfInt64(
    /* [in] */ Int32 index,
    /* [in] */ Int64 value)
{
    return SetParamValue(index, &value, CarDataType_Int64, ParamIOAttribute_In);
}

ECode CArgumentList::GetInputArgumentOfByte(
    /* [in] */ Int32 index,
    /* [out] */ Byte* value)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CArgumentList::SetInputArgumentOfByte(
    /* [in] */ Int32 index,
    /* [in] */ Byte value)
{
    UInt32 nParam = value;
    return SetParamValue(index, &nParam, CarDataType_Byte, ParamIOAttribute_In);
}

ECode CArgumentList::GetInputArgumentOfFloat(
    /* [in] */ Int32 index,
    /* [out] */ Float* value)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CArgumentList::SetInputArgumentOfFloat(
    /* [in] */ Int32 index,
    /* [in] */ Float value)
{
    return SetParamValue(index, &value, CarDataType_Float, ParamIOAttribute_In);
}

ECode CArgumentList::GetInputArgumentOfDouble(
    /* [in] */ Int32 index,
    /* [out] */ Double* value)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CArgumentList::SetInputArgumentOfDouble(
    /* [in] */ Int32 index,
    /* [in] */ Double value)
{
    return SetParamValue(index, &value, CarDataType_Double, ParamIOAttribute_In);
}

ECode CArgumentList::GetInputArgumentOfChar(
    /* [in] */ Int32 index,
    /* [out] */ Char32* value)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CArgumentList::SetInputArgumentOfChar(
    /* [in] */ Int32 index,
    /* [in] */ Char32 value)
{
    UInt32 nParam = value;
    return SetParamValue(index, &nParam, CarDataType_Char32, ParamIOAttribute_In);
}

ECode CArgumentList::GetInputArgumentOfString(
    /* [in] */ Int32 index,
    /* [out] */ String* value)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CArgumentList::SetInputArgumentOfString(
    /* [in] */ Int32 index,
    /* [in] */ const String& value)
{
    return SetParamValue(index, reinterpret_cast<PVoid>(const_cast<String*>(&value)),
            CarDataType_String, ParamIOAttribute_In);
}

ECode CArgumentList::GetInputArgumentOfBoolean(
    /* [in] */ Int32 index,
    /* [out] */ Boolean* value)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CArgumentList::SetInputArgumentOfBoolean(
    /* [in] */ Int32 index,
    /* [in] */ Boolean value)
{
    UInt32 nParam = value;
    return SetParamValue(index, &nParam, CarDataType_Boolean, ParamIOAttribute_In);
}

ECode CArgumentList::GetInputArgumentOfEMuid(
    /* [in] */ Int32 index,
    /* [out] */ EMuid* value)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CArgumentList::SetInputArgumentOfEMuid(
    /* [in] */ Int32 index,
    /* [in] */ EMuid* value)
{
    return SetParamValue(index, &value, CarDataType_EMuid, ParamIOAttribute_In);
}

ECode CArgumentList::GetInputArgumentOfEGuid(
    /* [in] */ Int32 index,
    /* [out] */ EGuid* value)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CArgumentList::SetInputArgumentOfEGuid(
    /* [in] */ Int32 index,
    /* [in] */ EGuid* value)
{
    return SetParamValue(index, &value, CarDataType_EGuid, ParamIOAttribute_In);
}

ECode CArgumentList::GetInputArgumentOfECode(
    /* [in] */ Int32 index,
    /* [out] */ ECode* value)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CArgumentList::SetInputArgumentOfECode(
    /* [in] */ Int32 index,
    /* [in] */ ECode value)
{
    return SetParamValue(index, &value, CarDataType_ECode, ParamIOAttribute_In);
}

ECode CArgumentList::SetInputArgumentOfLocalPtr(
    /* [in] */ Int32 index,
    /* [in] */ LocalPtr value)
{
    return SetParamValue(index, &value, CarDataType_LocalPtr,
            ParamIOAttribute_In);
}

ECode CArgumentList::SetInputArgumentOfLocalType(
    /* [in] */ Int32 index,
    /* [in] */ PVoid value)
{
    return SetParamValue(index, value, CarDataType_LocalType,
            ParamIOAttribute_In);
}

ECode CArgumentList::GetInputArgumentOfEnum(
    /* [in] */ Int32 index,
    /* [out] */ Int32* value)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CArgumentList::SetInputArgumentOfEnum(
    /* [in] */ Int32 index,
    /* [in] */ Int32 value)
{
    return SetParamValue(index, &value, CarDataType_Enum, ParamIOAttribute_In);
}

ECode CArgumentList::GetInputArgumentOfCarArray(
    /* [in] */ Int32 index,
    /* [out] */ PCarQuintet* value)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CArgumentList::SetInputArgumentOfCarArray(
    /* [in] */ Int32 index,
    /* [in] */ PCarQuintet value)
{
    return SetParamValue(index, &value, CarDataType_CarArray,
            ParamIOAttribute_In);
}

ECode CArgumentList::SetInputArgumentOfStructPtr(
    /* [in] */ Int32 index,
    /* [in] */ PVoid value)
{
    return SetParamValue(index, &value, CarDataType_Struct, ParamIOAttribute_In);
}

ECode CArgumentList::GetInputArgumentOfObjectPtr(
    /* [in] */ Int32 index,
    /* [out] */ PInterface* value)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CArgumentList::SetInputArgumentOfObjectPtr(
    /* [in] */ Int32 index,
    /* [in] */ PInterface value)
{
    if (CarDataType_Interface != mParamElem[index].mType) {
        return E_INVALID_ARGUMENT;
    }

    if (value) {
        CMethodInfo* methodInfo = NULL;
        ECode ec = NOERROR;
        if (mIsMethodInfo) {
            methodInfo = (CMethodInfo *)mFunctionInfo.Get();
        }
        else {
            methodInfo = ((CConstructorInfo *)mFunctionInfo.Get())->mMethodInfo;
        }

        Int32 base = methodInfo->mClsModule->mBase;
        TypeDescriptor* typeDesc = &(getParamDescAddr(base,
                methodInfo->mMethodDescriptor->mParams, index)->mType);
        if (typeDesc->mType == Type_alias) {
            ec = methodInfo->mClsModule->AliasToOriginal(typeDesc, &typeDesc);
            if (FAILED(ec)) return ec;
        }

        InterfaceDirEntry* ifDir = getInterfaceDirAddr(base,
                methodInfo->mClsMod->mInterfaceDirs, typeDesc->mIndex);
        EIID iid = adjustInterfaceDescAddr(base, ifDir->mDesc)->mIID;
        value = value->Probe(iid);
        if (!value) return E_NO_INTERFACE;

        // TODO hold param's ref-count.
        IInterface** prev = mInterfaceParams.Get(&index);
        if (prev && *prev) {
            (*prev)->Release();
        }
        value->AddRef();
        mInterfaceParams.Put(&index, (IInterface**)&value);
    }

    return SetParamValue(index, &value, CarDataType_Interface,
            ParamIOAttribute_In, 1);
}

ECode CArgumentList::GetOutputArgumentOfInt16Ptr(
    /* [in] */ Int32 index,
    /* [out] */ PVoid* value)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CArgumentList::SetOutputArgumentOfInt16Ptr(
    /* [in] */ Int32 index,
    /* [out] */ Int16* value)
{
    return SetParamValue(index, &value, CarDataType_Int16,
            ParamIOAttribute_CallerAllocOut, 1);
}

ECode CArgumentList::GetOutputArgumentOfInt32Ptr(
    /* [in] */ Int32 index,
    /* [out] */ PVoid* value)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CArgumentList::SetOutputArgumentOfInt32Ptr(
    /* [in] */ Int32 index,
    /* [out] */ Int32* value)
{
    return SetParamValue(index, &value, CarDataType_Int32,
            ParamIOAttribute_CallerAllocOut, 1);
}

ECode CArgumentList::GetOutputArgumentOfInt64Ptr(
    /* [in] */ Int32 index,
    /* [out] */ PVoid* value)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CArgumentList::SetOutputArgumentOfInt64Ptr(
    /* [in] */ Int32 index,
    /* [out] */ Int64* value)
{
    return SetParamValue(index, &value, CarDataType_Int64,
            ParamIOAttribute_CallerAllocOut, 1);
}

ECode CArgumentList::GetOutputArgumentOfBytePtr(
    /* [in] */ Int32 index,
    /* [out] */ PVoid* value)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CArgumentList::SetOutputArgumentOfBytePtr(
    /* [in] */ Int32 index,
    /* [out] */ Byte* value)
{
    return SetParamValue(index, &value, CarDataType_Byte,
            ParamIOAttribute_CallerAllocOut, 1);
}

ECode CArgumentList::GetOutputArgumentOfFloatPtr(
    /* [in] */ Int32 index,
    /* [out] */ PVoid* value)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CArgumentList::SetOutputArgumentOfFloatPtr(
    /* [in] */ Int32 index,
    /* [out] */ Float* value)
{
    return SetParamValue(index, &value, CarDataType_Float,
            ParamIOAttribute_CallerAllocOut, 1);
}

ECode CArgumentList::GetOutputArgumentOfDoublePtr(
    /* [in] */ Int32 index,
    /* [out] */ PVoid* value)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CArgumentList::SetOutputArgumentOfDoublePtr(
    /* [in] */ Int32 index,
    /* [out] */ Double* value)
{
    return SetParamValue(index, &value, CarDataType_Double,
            ParamIOAttribute_CallerAllocOut, 1);
}

ECode CArgumentList::GetOutputArgumentOfCharPtr(
    /* [in] */ Int32 index,
    /* [out] */ PVoid* value)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CArgumentList::SetOutputArgumentOfCharPtr(
    /* [in] */ Int32 index,
    /* [out] */ Char32* value)
{
    return SetParamValue(index, &value, CarDataType_Char32,
            ParamIOAttribute_CallerAllocOut, 1);
}

ECode CArgumentList::GetOutputArgumentOfStringPtr(
    /* [in] */ Int32 index,
    /* [out] */ PVoid* value)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CArgumentList::SetOutputArgumentOfStringPtr(
    /* [in] */ Int32 index,
    /* [out] */ String* value)
{
    return SetParamValue(index, value, CarDataType_String,
            ParamIOAttribute_CallerAllocOut, 1);
}

ECode CArgumentList::GetOutputArgumentOfBooleanPtr(
    /* [in] */ Int32 index,
    /* [out] */ PVoid* value)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CArgumentList::SetOutputArgumentOfBooleanPtr(
    /* [in] */ Int32 index,
    /* [out] */ Boolean* value)
{
    return SetParamValue(index, &value, CarDataType_Boolean,
            ParamIOAttribute_CallerAllocOut, 1);
}

ECode CArgumentList::GetOutputArgumentOfEMuidPtr(
    /* [in] */ Int32 index,
    /* [out] */ PVoid* value)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CArgumentList::SetOutputArgumentOfEMuidPtr(
    /* [in] */ Int32 index,
    /* [out] */ EMuid* value)
{
    return SetParamValue(index, &value, CarDataType_EMuid,
            ParamIOAttribute_CallerAllocOut, 1);
}

ECode CArgumentList::GetOutputArgumentOfEGuidPtr(
    /* [in] */ Int32 index,
    /* [out] */ PVoid* value)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CArgumentList::SetOutputArgumentOfEGuidPtr(
    /* [in] */ Int32 index,
    /* [out] */ EGuid* value)
{
    return SetParamValue(index, &value, CarDataType_EGuid,
            ParamIOAttribute_CallerAllocOut, 1);
}

ECode CArgumentList::GetOutputArgumentOfECodePtr(
    /* [in] */ Int32 index,
    /* [out] */ PVoid* value)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CArgumentList::SetOutputArgumentOfECodePtr(
    /* [in] */ Int32 index,
    /* [out] */ ECode* value)
{
    return SetParamValue(index, &value, CarDataType_ECode,
            ParamIOAttribute_CallerAllocOut, 1);
}

ECode CArgumentList::SetOutputArgumentOfLocalPtrPtr(
    /* [in] */ Int32 index,
    /* [out] */ LocalPtr* value)
{
    return SetParamValue(index, &value, CarDataType_LocalPtr,
            ParamIOAttribute_CallerAllocOut, 1);
}

ECode CArgumentList::SetOutputArgumentOfLocalTypePtr(
    /* [in] */ Int32 index,
    /* [out] */ PVoid value)
{
    return SetParamValue(index, &value, CarDataType_LocalType,
            ParamIOAttribute_CallerAllocOut, 1);
}

ECode CArgumentList::GetOutputArgumentOfEnumPtr(
    /* [in] */ Int32 index,
    /* [out] */ PVoid* value)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CArgumentList::SetOutputArgumentOfEnumPtr(
    /* [in] */ Int32 index,
    /* [out] */ Int32* value)
{
    return SetParamValue(index, &value, CarDataType_Enum,
            ParamIOAttribute_CallerAllocOut, 1);
}

ECode CArgumentList::GetOutputArgumentOfCarArrayPtr(
    /* [in] */ Int32 index,
    /* [out] */ PCarQuintet* value)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CArgumentList::SetOutputArgumentOfCarArrayPtr(
    /* [in] */ Int32 index,
    /* [out] */ PCarQuintet value)
{
    return SetParamValue(index, &value, CarDataType_CarArray,
            ParamIOAttribute_CallerAllocOut, 0);
}

ECode CArgumentList::GetOutputArgumentOfCarArrayPtrPtr(
    /* [in] */ Int32 index,
    /* [out] */ Handle32* value)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CArgumentList::SetOutputArgumentOfCarArrayPtrPtr(
    /* [in] */ Int32 index,
    /* [out] */ PCarQuintet* value)
{
    return SetParamValue(index, &value, CarDataType_CarArray,
            ParamIOAttribute_CalleeAllocOut, 2);
}

ECode CArgumentList::SetOutputArgumentOfStructPtr(
    /* [in] */ Int32 index,
    /* [out] */ PVoid value)
{
    return SetParamValue(index, &value, CarDataType_Struct,
            ParamIOAttribute_CallerAllocOut, 1);
}

ECode CArgumentList::SetOutputArgumentOfStructPtrPtr(
    /* [in] */ Int32 index,
    /* [out] */ PVoid* value)
{
    return SetParamValue(index, &value, CarDataType_Struct,
            ParamIOAttribute_CalleeAllocOut, 2);
}

ECode CArgumentList::GetOutputArgumentOfObjectPtrPtr(
    /* [in] */ Int32 index,
    /* [out] */ Handle32* value)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CArgumentList::SetOutputArgumentOfObjectPtrPtr(
    /* [in] */ Int32 index,
    /* [out] */ PInterface* value)
{
    return SetParamValue(index, &value, CarDataType_Interface,
            ParamIOAttribute_CallerAllocOut, 2);
}
