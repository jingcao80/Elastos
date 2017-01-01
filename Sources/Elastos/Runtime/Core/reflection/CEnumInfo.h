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

#ifndef __CENUMINFO_H__
#define __CENUMINFO_H__

#include "CEntryList.h"

class CEnumInfo
    : public ElLightRefBase
    , public IEnumInfo
{
public:
    CEnumInfo();

    virtual ~CEnumInfo();

    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI_(UInt32) AddRef();

    CARAPI_(UInt32) Release();

    CARAPI GetInterfaceID(
        /* [in] */ IInterface* object,
        /* [out] */ InterfaceID* iid);

    CARAPI InitStatic(
        /* [in] */ CClsModule* clsModule,
        /* [in] */ EnumDirEntry* enumDirEntry);

    CARAPI InitDynamic(
        /* [in] */ const String& fullName,
        /* [in] */ ArrayOf<String>* itemNames,
        /* [in] */ ArrayOf<Int32>* itemValues);

    CARAPI GetSize(
        /* [out] */ MemorySize* size);

    CARAPI GetDataType(
        /* [out] */ CarDataType* dataType);

    CARAPI GetName(
        /* [out] */ String* name);

    CARAPI GetNamespace(
        /* [out] */ String* ns);

    CARAPI GetModuleInfo(
        /* [out] */ IModuleInfo** moduleInfo);

    CARAPI GetItemCount(
        /* [out] */ Int32* count);

    CARAPI GetAllItemInfos(
        /* [out] */ ArrayOf<IEnumItemInfo *>* itemInfos);

    CARAPI GetItemInfo(
        /* [in] */ const String& name,
        /* [out] */ IEnumItemInfo** enumItemInfo);

    CARAPI InitItemInfos();

public:
    AutoPtr< ArrayOf<String> >  mItemNames;
    AutoPtr< ArrayOf<Int32> >   mItemValues;

private:
    AutoPtr<CClsModule>         mClsModule;
    EnumDirEntry*               mEnumDirEntry;
    ArrayOf<IEnumItemInfo *>*   mItemInfos;

    String                      mName;
    String                      mNamespace;
};

#endif // __CENUMINFO_H__
