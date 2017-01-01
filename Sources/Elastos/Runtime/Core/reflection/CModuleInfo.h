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

#ifndef __CMODULEINFO_H__
#define __CMODULEINFO_H__

#include "CEntryList.h"

class CModuleInfo
    : public ElLightRefBase
    , public IModuleInfo
{
public:
    CModuleInfo(
        /* [in] */ CClsModule* clsModule,
        /* [in] */ const String& path);

    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI_(UInt32) AddRef();

    CARAPI_(UInt32) Release();

    CARAPI GetInterfaceID(
        /* [in] */ IInterface* object,
        /* [out] */ InterfaceID* iid);

    CARAPI GetPath(
        /* [out] */ String* path);

    CARAPI GetVersion(
        /* [out] */ Int32* major,
        /* [out] */ Int32* minor,
        /* [out] */ Int32* build,
        /* [out] */ Int32* revision);

    CARAPI GetClassCount(
        /* [out] */ Int32* count);

    CARAPI GetAllClassInfos(
        /* [out] */ ArrayOf<IClassInfo *>* classInfos);

    CARAPI GetClassInfo(
        /* [in] */ const String& fullName,
        /* [out] */ IClassInfo** classInfo);

    CARAPI GetInterfaceCount(
        /* [out] */ Int32* count);

    CARAPI GetAllInterfaceInfos(
        /* [out] */ ArrayOf<IInterfaceInfo *>* interfaceInfos);

    CARAPI GetInterfaceInfo(
        /* [in] */ const String& fullName,
        /* [out] */ IInterfaceInfo** interfaceInfo);

    CARAPI GetStructCount(
        /* [out] */ Int32* count);

    CARAPI GetAllStructInfos(
        /* [out] */ ArrayOf<IStructInfo *>* structInfos);

    CARAPI GetStructInfo(
        /* [in] */ const String& name,
        /* [out] */ IStructInfo** structInfo);

    CARAPI GetEnumCount(
        /* [out] */ Int32* count);

    CARAPI GetAllEnumInfos(
        /* [out] */ ArrayOf<IEnumInfo *>* enumInfos);

    CARAPI GetEnumInfo(
        /* [in] */ const String& fullName,
        /* [out] */ IEnumInfo** enumInfo);

    CARAPI GetTypeAliasCount(
        /* [out] */ Int32* count);

    CARAPI GetAllTypeAliasInfos(
        /* [out] */ ArrayOf<ITypeAliasInfo *>* typeAliasInfos);

    CARAPI GetTypeAliasInfo(
        /* [in] */ const String& name,
        /* [out] */ ITypeAliasInfo** typeAliasInfo);

    CARAPI GetConstantCount(
        /* [out] */ Int32* count);

    CARAPI GetAllConstantInfos(
        /* [out] */ ArrayOf<IConstantInfo *>* constantInfos);

    CARAPI GetConstantInfo(
        /* [in] */ const String& name,
        /* [out] */ IConstantInfo** constantInfo);

    CARAPI GetImportModuleInfoCount(
        /* [out] */ Int32* count);

    CARAPI GetAllImportModuleInfos(
        /* [out] */ ArrayOf<IModuleInfo *>* moduleInfos);

    CARAPI AcquireClassList();

    CARAPI AcquireInterfaceList();

    CARAPI AcquireStructList();

    CARAPI AcquireEnumList();

    CARAPI AcquireAliasList();

    CARAPI AcquireConstantList();

public:
    AutoPtr<CClsModule> mClsModule;
    CLSModule*          mClsMod;

private:
    AutoPtr<CEntryList> mClassList;
    AutoPtr<CEntryList> mInterfaceList;
    AutoPtr<CEntryList> mStructList;
    AutoPtr<CEntryList> mEnumList;
    AutoPtr<CEntryList> mAliasList;
    AutoPtr<CEntryList> mConstantList;

    String              mPath;
    Int32               mAliasCount;
};

#endif // __CMODULEINFO_H__
