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

#include "CIntrinsicInfo.h"
#include "CObjInfoList.h"
#include "refutil.h"

CIntrinsicInfo::CIntrinsicInfo(
    /* [in] */ CarDataType dataType,
    /* [in] */ UInt32 size)
{
    mDataType = dataType;
    mSize = size;
}

UInt32 CIntrinsicInfo::AddRef()
{
    return ElLightRefBase::AddRef();
}

UInt32 CIntrinsicInfo::Release()
{
    return ElLightRefBase::Release();
}

PInterface CIntrinsicInfo::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)this;
    }
    else if (riid == EIID_IDataTypeInfo) {
        return (IDataTypeInfo *)this;
    }
    else {
        return NULL;
    }
}

ECode CIntrinsicInfo::GetInterfaceID(
    /* [in] */ IInterface* object,
    /* [out] */ InterfaceID* iid)
{
    return E_NOT_IMPLEMENTED;
}

ECode CIntrinsicInfo::GetName(
    /* [out] */ String* name)
{
    if (name == NULL) {
        return E_INVALID_ARGUMENT;
    }

    *name = g_cDataTypeList[mDataType].mName;
    return NOERROR;
}

ECode CIntrinsicInfo::GetSize(
    /* [out] */ MemorySize* size)
{
    if (!mSize) {
        return E_INVALID_OPERATION;
    }

    if (!size) {
        return E_INVALID_ARGUMENT;
    }

    *size = mSize;
    return NOERROR;
}

ECode CIntrinsicInfo::GetDataType(
    /* [out] */ CarDataType* dataType)
{
    if (!dataType) {
        return E_INVALID_ARGUMENT;
    }

    *dataType = mDataType;
    return NOERROR;
}
