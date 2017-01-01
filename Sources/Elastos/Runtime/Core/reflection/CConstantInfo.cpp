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

#include "CConstantInfo.h"

CConstantInfo::CConstantInfo(
    /* [in] */ CClsModule* clsModule,
    /* [in] */ ConstDirEntry* constDirEntry)
    : mClsModule(clsModule)
    , mConstDirEntry(constDirEntry)
{}

UInt32 CConstantInfo::AddRef()
{
    return ElLightRefBase::AddRef();
}

UInt32 CConstantInfo::Release()
{
    return ElLightRefBase::Release();
}

PInterface CConstantInfo::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)this;
    }
    else if (riid == EIID_IConstantInfo) {
        return (IConstantInfo *)this;
    }
    else {
        return NULL;
    }
}

ECode CConstantInfo::GetInterfaceID(
    /* [in] */ IInterface* object,
    /* [out] */ InterfaceID* iid)
{
    return E_NOT_IMPLEMENTED;
}

ECode CConstantInfo::GetName(
    /* [out] */ String* name)
{
    if (name == NULL) {
        return E_INVALID_ARGUMENT;
    }

    *name = adjustNameAddr(mClsModule->mBase, mConstDirEntry->mName);
    return NOERROR;
}

ECode CConstantInfo::GetValue(
    /* [out] */ Int32* value)
{
    if (!value) {
        return E_INVALID_ARGUMENT;
    }

    if (mConstDirEntry->mType == TYPE_INTEGER32) {
        *value = mConstDirEntry->mV.mInt32Value.mValue;
        return NOERROR;
    }
    else return E_INVALID_OPERATION;
}

ECode CConstantInfo::GetModuleInfo(
    /* [out] */ IModuleInfo** moduleInfo)
{
    return mClsModule->GetModuleInfo(moduleInfo);
}
