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

#include "CEnumItemInfo.h"
#include "CEnumInfo.h"

CEnumItemInfo::CEnumItemInfo(
    /* [in] */ IEnumInfo* enumInfo,
    /* [in] */ const String& name,
    /* [in] */ Int32 value)
    : mEnumInfo(enumInfo)
    , mName(name)
    , mValue(value)
{}

CEnumItemInfo::~CEnumItemInfo()
{}

UInt32 CEnumItemInfo::AddRef()
{
    return mEnumInfo->AddRef();
}

UInt32 CEnumItemInfo::Release()
{
    return mEnumInfo->Release();
}

PInterface CEnumItemInfo::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)this;
    }
    else if (riid == EIID_IEnumItemInfo) {
        return (IEnumItemInfo *)this;
    }
    else {
        return NULL;
    }
}

ECode CEnumItemInfo::GetInterfaceID(
    /* [in] */ IInterface* object,
    /* [out] */ InterfaceID* iid)
{
    return E_NOT_IMPLEMENTED;
}

ECode CEnumItemInfo::GetName(
    /* [out] */ String* name)
{
    if (name == NULL) {
        return E_INVALID_ARGUMENT;
    }

    *name = mName;
    return NOERROR;
}

ECode CEnumItemInfo::GetEnumInfo(
    /* [out] */ IEnumInfo** enumInfo)
{
    if (!enumInfo) {
        return E_INVALID_ARGUMENT;
    }

    *enumInfo = mEnumInfo;
    (*enumInfo)->AddRef();
    return NOERROR;
}

ECode CEnumItemInfo::GetValue(
    /* [out] */ Int32* value)
{
    if (!value) {
        return E_INVALID_ARGUMENT;
    }

    *value = mValue;
    return NOERROR;
}
