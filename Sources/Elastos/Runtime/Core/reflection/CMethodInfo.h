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

#ifndef __CMETHODINFO_H__
#define __CMETHODINFO_H__

#include "CClsModule.h"
#include "CEntryList.h"

class CMethodInfo
    : public ElLightRefBase
    , public IMethodInfo
{
public:
    CMethodInfo(
        /* [in] */ CClsModule* clsModule,
        /* [in] */ MethodDescriptor* methodDescriptor,
        /* [in] */ UInt32 index);

    virtual ~CMethodInfo();

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

    CARAPI GetSignature(
        /* [out] */ String* signature);

    CARAPI GetAnnotationCount(
        /* [out] */ Int32* count);

    CARAPI GetAllAnnotationInfos(
        /* [out] */ ArrayOf<IAnnotationInfo *>* annotationInfos);

    CARAPI GetAnnotation(
        /* [in] */ const String& fullName,
        /* [out] */ IAnnotationInfo** annotationInfo);

    CARAPI GetDeclaringInterface(
        /* [out] */ IInterfaceInfo** interfaceInfo);

    CARAPI GetParamCount(
        /* [out] */ Int32* count);

    CARAPI GetAllParamInfos(
        /* [out] */ ArrayOf<IParamInfo *>* paramInfos);

    CARAPI GetParamInfoByIndex(
        /* [in] */ Int32 index,
        /* [out] */ IParamInfo** paramInfo);

    CARAPI GetParamInfoByName(
        /* [in] */ const String& name,
        /* [out] */ IParamInfo** paramInfo);

    CARAPI CreateArgumentList(
        /* [out] */ IArgumentList** argumentList);

    CARAPI Invoke(
        /* [in] */ PInterface target,
        /* [in] */ IArgumentList* argumentList);

    CARAPI SetParamElem(
        /* [in] */ ParamDescriptor* paramDescriptor,
        /* [in] */ ParmElement* parmElement);

    CARAPI InitParmElement();

    CARAPI InitParamInfos();

    CARAPI CreateFunctionArgumentList(
        /* [in] */ IFunctionInfo* functionInfo,
        /* [in] */ Boolean isMethodInfo,
        /* [out] */ IArgumentList** argumentList);

    CARAPI CreateCBArgumentList(
        /* [in] */ ICallbackMethodInfo* callbackMethodInfo,
        /* [out] */ ICallbackArgumentList** cbArgumentList);

    CARAPI AcquireAnnotationInfos();

public:
    MethodDescriptor*   mMethodDescriptor;
    UInt32              mIndex;
    CLSModule*          mClsMod;
    AutoPtr<CClsModule> mClsModule;
    IInterfaceInfo*     mDeclaringInterface;

private:
    AutoPtr< ArrayOf<IAnnotationInfo*> > mAnnotationInfos;
    ArrayOf<IParamInfo *>*  mParameterInfos;
    ParmElement*            mParamElem;
    UInt32                  mParamBufSize;
    Int32                   mBase;
};

#endif // __CMETHODINFO_H__
