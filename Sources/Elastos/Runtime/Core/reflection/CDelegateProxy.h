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

#ifndef __CDELEGATEPROXY_H__
#define __CDELEGATEPROXY_H__

#include "refutil.h"

class CDelegateProxy
    : public ElLightRefBase
    , public IDelegateProxy
{
public:
    CDelegateProxy(
        /* [in] */ ICallbackMethodInfo* callbackMethodInfo,
        /* [in] */ ICallbackInvocation* callbackInvocation,
        /* [in] */ PVoid targetObject,
        /* [in] */ PVoid targetMethod);

    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI_(UInt32) AddRef();

    CARAPI_(UInt32) Release();

    CARAPI GetInterfaceID(
        /* [in] */ IInterface* object,
        /* [out] */ InterfaceID* iid);

    CARAPI GetCallbackMethodInfo(
        /* [out] */ ICallbackMethodInfo** callbackMethodInfo);

    CARAPI GetTargetObject(
        /* [out] */ PVoid* targetObject);

    CARAPI GetTargetMethod(
        /* [out] */ PVoid* targetMethod);

    CARAPI GetCallbackInvocation(
        /* [out] */ ICallbackInvocation** callbackInvocation);

#ifdef _arm
    CARAPI OnEvent(
        /* [in] */ PInterface server, ...);
#elif _aarch64
    CARAPI OnEvent(
        /* [in] */ PInterface server, ...);
#else
    CARAPI OnEvent(
        /* [in] */ PInterface server);
#endif

    CARAPI EventHander(
        /* [in] */ PVoid paramBuf,
        /* [out] */ UInt32* paramBufSize);

    CARAPI GetDelegate(
        /* [out] */ EventHandler* handler);

private:
    AutoPtr<ICallbackMethodInfo>    mCallbackMethodInfo;
    AutoPtr<ICallbackInvocation>    mCallbackInvocation;
    PVoid                           mTargetObject;
    PVoid                           mTargetMethod;
};

#endif // __CDELEGATEPROXY_H__
