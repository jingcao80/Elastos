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

#ifndef __CCALLBACKMETHODINFO_H__
#define __CCALLBACKMETHODINFO_H__

#include "CMethodInfo.h"

class CCallbackMethodInfo
    : public ElLightRefBase
    , public ICallbackMethodInfo
{
public:
    CCallbackMethodInfo();

    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI_(UInt32) AddRef();

    CARAPI_(UInt32) Release();

    CARAPI GetInterfaceID(
        /* [in] */ IInterface* object,
        /* [out] */ InterfaceID* iid);

    CARAPI Init(
        /* [in] */ CClsModule* clsModule,
        /* [in] */ UInt32 eventNum,
        /* [in] */ MethodDescriptor* methodDescriptor,
        /* [in] */ UInt32 index);

    CARAPI GetName(
        /* [out] */ String* name);

    CARAPI GetSignature(
        /* [out] */ String* signature);

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

    CARAPI AddCallback(
        /* [in] */ PInterface server,
        /* [in] */ EventHandler handler);

    CARAPI RemoveCallback(
        /* [in] */ PInterface server,
        /* [in] */ EventHandler handler);

    CARAPI CreateDelegateProxy(
        /* [in] */ PVoid targetObject,
        /* [in] */ PVoid targetMethod,
        /* [in] */ ICallbackInvocation* callbackInvocation,
        /* [out] */ IDelegateProxy** delegateProxy);

    CARAPI CreateCBArgumentList(
        /* [out] */ ICallbackArgumentList** cbArgumentList);

private:
    AutoPtr<CMethodInfo>    mMethodInfo;
    UInt32                  mEventNum;
    MethodDescriptor*       mMethodDescriptor;
};

#endif // __CCALLBACKMETHODINFO_H__
