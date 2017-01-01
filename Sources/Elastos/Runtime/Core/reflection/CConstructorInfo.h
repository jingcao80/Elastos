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

#ifndef __CCONSTRUCTORINFO_H__
#define __CCONSTRUCTORINFO_H__

#include "CMethodInfo.h"

class CConstructorInfo
    : public ElLightRefBase
    , public IConstructorInfo
{
public:
    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI_(UInt32) AddRef();

    CARAPI_(UInt32) Release();

    CARAPI GetInterfaceID(
        /* [in] */ IInterface* object,
        /* [out] */ InterfaceID* iid);

    CARAPI Init(
        /* [in] */ CClsModule* clsModule,
        /* [in] */ MethodDescriptor* methodDescriptor,
        /* [in] */ UInt32 index,
        /* [in] */ ClassID* clsId);

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

    CARAPI CreateObject(
        /* [in] */ IArgumentList* argumentList,
        /* [out] */ PInterface* object);

    CARAPI CreateObjectInRegime(
        /* [in] */ PRegime rgm,
        /* [in] */ IArgumentList* argumentList,
        /* [out] */ PInterface* object);

    CARAPI AcquireParamList();

    CARAPI CreateObjInRgm(
        /* [in] */ PRegime rgm,
        /* [in] */ IArgumentList* argumentList,
        /* [out] */ PInterface* object);

public:
    AutoPtr<CMethodInfo>    mMethodInfo;

    char                    mUrn2[_MAX_PATH];
    ClassID                 mInstClsId;

private:
    Int32                   mOutParamIndex;

    char                    mUrn[_MAX_PATH];
    ClassID                 mClsId;
};

#endif // __CCONSTRUCTORINFO_H__
