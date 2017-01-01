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

#ifndef __CCLASSINFO_H__
#define __CCLASSINFO_H__

#include "CEntryList.h"

struct CBMethodDesc
{
    MethodDescriptor* mDesc;
    UInt32 mIndex;
    UInt32 mEventNum;
};

class CClassInfo
    : public ElLightRefBase
    , public IClassInfo
{
public:
    CClassInfo(
        /* [in] */ CClsModule* clsModule,
        /* [in] */ ClassDirEntry* clsDirEntry);

    virtual ~CClassInfo();

    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI_(UInt32) AddRef();

    CARAPI_(UInt32) Release();

    CARAPI GetInterfaceID(
        /* [in] */ IInterface* object,
        /* [out] */ InterfaceID* iid);

    CARAPI Init();

    CARAPI GetName(
        /* [out] */ String* name);

    CARAPI GetNamespace(
        /* [out] */ String* ns);

    CARAPI GetId(
        /* [out] */ ClassID* clsid);

    CARAPI GetModuleInfo(
        /* [out] */ IModuleInfo** moduleInfo);

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

    CARAPI IsSingleton(
        /* [out] */ Boolean* isSingleton);

    CARAPI GetThreadingModel(
        /* [out] */ ThreadingModel* threadingModel);

    CARAPI IsPrivate(
        /* [out] */ Boolean* isPrivate);

    CARAPI IsReturnValue(
        /* [out] */ Boolean* isReturnValue);

    CARAPI IsBaseClass(
        /* [out] */ Boolean* isBaseClass);

    CARAPI HasBaseClass(
        /* [out] */ Boolean* hasBaseClass);

    CARAPI GetBaseClassInfo(
        /* [out] */ IClassInfo** baseClassInfo);

    CARAPI IsGeneric(
        /* [out] */ Boolean* isGeneric);

    CARAPI HasGeneric(
        /* [out] */ Boolean* hasGeneric);

    CARAPI GetGenericInfo(
        /* [out] */ IClassInfo** genericInfo);

    CARAPI IsRegime(
        /* [out] */ Boolean* isRegime);

    CARAPI GetAspectCount(
        /* [out] */ Int32* count);

    CARAPI GetAllAspectInfos(
        /* [out] */ ArrayOf<IClassInfo *>* aspectInfos);

    CARAPI GetAspectInfo(
        /* [in] */ const String& name,
        /* [out] */ IClassInfo** aspectInfo);

    CARAPI IsAspect(
        /* [out] */ Boolean* isAspect);

    CARAPI GetAggregateeCount(
        /* [out] */ Int32* count);

    CARAPI GetAllAggregateeInfos(
        /* [out] */ ArrayOf<IClassInfo *>* aggregateeInfos);

    CARAPI GetAggregateeInfo(
        /* [in] */ const String& name,
        /* [out] */ IClassInfo** aggregateeInfo);

    CARAPI GetConstructorCount(
        /* [out] */ Int32* count);

    CARAPI GetAllConstructorInfos(
        /* [out] */ ArrayOf<IConstructorInfo *>* constructorInfos);

    CARAPI GetConstructorInfoByParamNames(
        /* [in] */ const String& name,
        /* [out] */ IConstructorInfo** constructorInfo);

    CARAPI GetConstructorInfoByParamCount(
        /* [in] */ Int32 count,
        /* [out] */ IConstructorInfo** constructorInfo);

    CARAPI GetInterfaceCount(
        /* [out] */ Int32* count);

    CARAPI GetAllInterfaceInfos(
        /* [out] */ ArrayOf<IInterfaceInfo *>* interfaceInfos);

    CARAPI GetInterfaceInfo(
        /* [in] */ const String& fullName,
        /* [out] */ IInterfaceInfo** interfaceInfo);

    CARAPI HasInterfaceInfo(
        /* [in] */ IInterfaceInfo* interfaceInfo,
        /* [out] */ Boolean* result);

    CARAPI GetCallbackInterfaceCount(
        /* [out] */ Int32* count);

    CARAPI GetAllCallbackInterfaceInfos(
        /* [out] */ ArrayOf<IInterfaceInfo *>* callbackInterfaceInfos);

    CARAPI GetCallbackInterfaceInfo(
        /* [in] */ const String& name,
        /* [out] */ IInterfaceInfo** callbackInterfaceInfo);

    CARAPI GetMethodCount(
        /* [out] */ Int32* count);

    CARAPI GetAllMethodInfos(
        /* [out] */ ArrayOf<IMethodInfo *>* methodInfos);

    CARAPI GetMethodInfo(
        /* [in] */ const String& name,
        /* [in] */ const String& signature,
        /* [out] */ IMethodInfo** methodInfo);

    CARAPI GetCallbackMethodCount(
        /* [out] */ Int32* count);

    CARAPI GetAllCallbackMethodInfos(
        /* [out] */ ArrayOf<ICallbackMethodInfo *>* callbackMethodInfos);

    CARAPI GetCallbackMethodInfo(
        /* [in] */ const String& name,
        /* [out] */ ICallbackMethodInfo** callbackMethodInfo);

    CARAPI RemoveAllCallbackHandlers(
        /* [in] */ PInterface server);

    CARAPI CreateObject(
        /* [out] */ PInterface* object);

    CARAPI CreateObjectInRegime(
        /* [in] */ PRegime rgm,
        /* [out] */ PInterface* object);

    CARAPI CreateIFList();

    CARAPI AcquireSpecialMethodList(
        /* [in] */ EntryType type,
        /* [out] */ CEntryList** entryList);

    CARAPI AcquireAggregateeList();

    CARAPI AcquireAspectList();

    CARAPI AcquireMethodList();

    CARAPI AcquireInterfaceList();

    CARAPI AcquireCBInterfaceList();

    CARAPI AcquireConstructorList();

    CARAPI AcquireCBMethodList();

    CARAPI AcquireAnnotationInfos();

    CARAPI CreateObjInRgm(
        /* [in] */ PRegime rgm,
        /* [out] */ PInterface* object);

public:
    ClassDirEntry*  mClassDirEntry;
    ClassID         mClsId;

    IFIndexEntry*   mCBIFList;
    IFIndexEntry*   mIFList;
    CBMethodDesc*   mCBMethodDesc;

    UInt32          mIFCount;
    UInt32          mCBIFCount;

private:
    AutoPtr<CClsModule> mClsModule;
    CLSModule*          mClsMod;
    ClassDescriptor*    mDesc;
    Int32               mBase;

    char   mUrn[_MAX_PATH];

    UInt32 mMethodCount;
    UInt32 mCBMethodCount;
    UInt32 mCtorCount;

    AutoPtr<CEntryList> mAspectList;
    AutoPtr<CEntryList> mAggregateeList;
    AutoPtr<CEntryList> mMethodList;
    AutoPtr<CEntryList> mCBInterfaceList;
    AutoPtr<CEntryList> mInterfaceList;
    AutoPtr<CEntryList> mCtorList;
    AutoPtr<CEntryList> mCBMethodList;

    AutoPtr<CClassInfo> mCtorClassInfo;
    AutoPtr<IClassInfo> mGenericInfo;
    AutoPtr<IInterface> mClassLoader;

    AutoPtr< ArrayOf<IAnnotationInfo*> > mAnnotationInfos;
};

#endif // __CCLASSINFO_H__
