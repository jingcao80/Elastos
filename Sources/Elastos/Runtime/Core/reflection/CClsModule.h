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

#ifndef __CCLSMODULE_H__
#define __CCLSMODULE_H__

#include "refutil.h"

class CClsModule : public ElLightRefBase
{
public:
    CClsModule(
        /* [in] */ CLSModule* clsMod,
        /* [in] */ Boolean allocedClsMod,
        /* [in] */ const String& path,
        /* [in] */ Void* module);

    ~CClsModule();

    CARAPI_(UInt32) AddRef();

    CARAPI_(UInt32) Release();

    CARAPI GetModuleInfo(
        /* [out] */ IModuleInfo** moduleInfo);

    CARAPI InitOrgType();

    CARAPI AliasToOriginal(
        /* [in] */ TypeDescriptor* typeDype,
        /* [out] */ TypeDescriptor** orgTypeDesc);

public:
    CLSModule*      mClsMod;
    Boolean         mAllocedClsMode;
    uintptr_t       mBase;

private:
    TypeAliasDesc*  mTypeAliasList;
    String          mPath;
    Void*           mModule;
};

#endif // __CCLSMODULE_H__
