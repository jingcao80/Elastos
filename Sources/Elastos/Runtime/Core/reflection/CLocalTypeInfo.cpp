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

#include "CLocalTypeInfo.h"

CLocalTypeInfo::CLocalTypeInfo(
    /* [in] */ MemorySize size)
    : mSize(size)
{}

UInt32 CLocalTypeInfo::AddRef()
{
    return ElLightRefBase::AddRef();
}

UInt32 CLocalTypeInfo::Release()
{
    return ElLightRefBase::Release();
}

PInterface CLocalTypeInfo::Probe(
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

ECode CLocalTypeInfo::GetInterfaceID(
    /* [in] */ IInterface* object,
    /* [out] */ InterfaceID* iid)
{
    return E_NOT_IMPLEMENTED;
}

ECode CLocalTypeInfo::GetName(
    /* [out] */ String* name)
{
    if (name == NULL) {
        return E_INVALID_ARGUMENT;
    }

    *name = g_cDataTypeList[CarDataType_LocalType].mName;
    return NOERROR;
}

ECode CLocalTypeInfo::GetSize(
    /* [out] */ MemorySize* size)
{
    if (!size) {
        return E_INVALID_ARGUMENT;
    }

    *size = mSize;

    return NOERROR;
}

ECode CLocalTypeInfo::GetDataType(
    /* [out] */ CarDataType* dataType)
{
    if (!dataType) {
        return E_INVALID_ARGUMENT;
    }

    *dataType = CarDataType_LocalType;

    return NOERROR;
}
