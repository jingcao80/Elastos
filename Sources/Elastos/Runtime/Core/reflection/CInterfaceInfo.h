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

#ifndef __CINTERFACEINFO_H__
#define __CINTERFACEINFO_H__

#include "CEntryList.h"

class CInterfaceInfo
    : public ElLightRefBase
    , public IInterfaceInfo
{
public:
    CInterfaceInfo(
        /* [in] */ CClsModule* clsModule,
        /* [in] */ UInt32 index);

    virtual ~CInterfaceInfo();

    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI_(UInt32) AddRef();

    CARAPI_(UInt32) Release();

    CARAPI GetInterfaceID(
        /* [in] */ IInterface* object,
        /* [out] */ InterfaceID* iid);

    CARAPI Init();

    CARAPI GetSize(
        /* [out] */ MemorySize* size);

    CARAPI GetDataType(
        /* [out] */ CarDataType* dataType);

    CARAPI GetName(
        /* [out] */ String* name);

    CARAPI GetNamespace(
        /* [out] */ String* ns);

    CARAPI GetClassLoader(
        /* [out] */ IInterface** loader);

    CARAPI SetClassLoader(
        /* [in] */ IInterface* loader);

    CARAPI GetAnnotationCount(
        /* [out] */ Int32* count);

    CARAPI GetAllAnnotationInfos(
        /* [out] */ ArrayOf<IAnnotationInfo*>* annotationInfos);

    CARAPI GetAnnotation(
        /* [in] */ const String& fullName,
        /* [out] */ IAnnotationInfo** annotationInfo);

    CARAPI GetId(
        /* [out] */ InterfaceID* id);

    CARAPI GetModuleInfo(
        /* [out] */ IModuleInfo** moduleInfo);

    CARAPI IsLocal(
        /* [out] */ Boolean* local);

    CARAPI HasBase(
        /* [out] */ Boolean* hasBase);

    CARAPI GetBaseInfo(
        /* [out] */ IInterfaceInfo** baseInfo);

    CARAPI GetMethodCount(
        /* [out] */ Int32* count);

    CARAPI GetAllMethodInfos(
        /* [out] */ ArrayOf<IMethodInfo *>* methodInfos);

    CARAPI GetMethodInfo(
        /* [in] */ const String& name,
        /* [in] */ const String& signature,
        /* [out] */ IMethodInfo** methodInfo);

    CARAPI GetMethodInfo(
        /* [in] */ Int32 index,
        /* [out] */ IMethodInfo** methodInfo);

    CARAPI CreateIFList();

    CARAPI AcquireMethodList();

    CARAPI AcquireAnnotationInfos();

private:
    AutoPtr<CClsModule>     mClsModule;
    CLSModule*              mClsMod;
    AutoPtr<CEntryList>     mMethodList;
    InterfaceDirEntry*      mInterfaceDirEntry;
    InterfaceDescriptor*    mDesc;
    IFIndexEntry*           mIFList;
    AutoPtr<IInterface>     mClassLoader;
    AutoPtr< ArrayOf<IAnnotationInfo*> > mAnnotationInfos;

    UInt32  mIndex;
    UInt32  mIFCount;
    UInt32  mMethodCount;
    Int32   mBase;
};

#endif // __CINTERFACEINFO_H__
