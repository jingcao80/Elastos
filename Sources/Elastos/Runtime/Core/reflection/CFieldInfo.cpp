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

#include "CFieldInfo.h"
#include "CObjInfoList.h"

CFieldInfo::CFieldInfo(
    /* [in] */ IStructInfo* structInfo,
    /* [in] */ const String& name,
    /* [in] */ IDataTypeInfo* typeInfo)
    : mStructInfo(structInfo)
    , mName(name)
    , mTypeInfo(typeInfo)
{}

CFieldInfo::~CFieldInfo()
{}

UInt32 CFieldInfo::AddRef()
{
    return mStructInfo->AddRef();
}

UInt32 CFieldInfo::Release()
{
    return mStructInfo->Release();
}

PInterface CFieldInfo::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)this;
    }
    else if (riid == EIID_IFieldInfo) {
        return (IFieldInfo *)this;
    }
    else {
        return NULL;
    }
}

ECode CFieldInfo::GetInterfaceID(
    /* [in] */ IInterface* object,
    /* [out] */ InterfaceID* iid)
{
    return E_NOT_IMPLEMENTED;
}

ECode CFieldInfo::GetName(
    /* [out] */ String* name)
{
    if (name == NULL) {
        return E_INVALID_ARGUMENT;
    }

    *name = mName;
    return NOERROR;
}

ECode CFieldInfo::GetTypeInfo(
    /* [out] */ IDataTypeInfo** typeInfo)
{
    if (!typeInfo) {
        return E_INVALID_ARGUMENT;
    }

    *typeInfo = mTypeInfo;
    (*typeInfo)->AddRef();
    return NOERROR;
}
