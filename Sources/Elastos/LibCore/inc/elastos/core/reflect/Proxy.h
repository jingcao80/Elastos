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

#ifndef __ELASTOS_CORE_REFLECT_PROXY_H__
#define __ELASTOS_CORE_REFLECT_PROXY_H__

#include "Elastos.CoreLibrary.Core.h"
#include <clsinfo.h>

namespace Elastos {
namespace Core {
namespace Reflect {

class CObjectProxy;

class CInterfaceProxy
{
public:
    static CARAPI_(PInterface) S_Probe(
        /* [in] */ CInterfaceProxy* thisPtr,
        /* [in] */ REIID riid);

    static CARAPI_(UInt32) S_AddRef(
        /* [in] */ CInterfaceProxy* thisPtr);

    static CARAPI_(UInt32) S_Release(
        /* [in] */ CInterfaceProxy* thisPtr);

    static CARAPI S_GetInterfaceID(
        /* [in] */ CInterfaceProxy* thisPtr,
        /* [in] */ IInterface* object,
        /* [out] */ InterfaceID* iid);

//    CARAPI BufferSize(
//        /* [in] */ UInt32 methodIndex,
//        /* [in] */ UInt32* args,
//        /* [out] */ UInt32* inSize,
//        /* [out] */ UInt32* outSize);
//
    CARAPI PackingArguments(
        /* [in] */ IMethodInfo* methodInfo,
        /* [in] */ UInt32* args,
        /* [in, out] */ IArgumentList* argList);

    inline CARAPI_(UInt32) CountMethodArgs(
        /* [in] */ UInt32 methodIndex);

//    CARAPI_(Boolean) MethodHasOutArgs(
//        /* [in] */ UInt32 methodIndex);
//
//    inline CARAPI_(Boolean) IsMethodOneway(
//        /* [in] */ UInt32 methodIndex);
//
    static CARAPI ProxyEntry(
        /* [in] */ UInt32* args);

public:
    PVoid                   mVTPtr;       // must be first member
    PVoid                   mProxyEntry; // must be mVTPtr's next member

    UInt32                  mIndex;       // interface index in object
    CObjectProxy*           mOwner;
    CIInterfaceInfo*        mInfo;
    AutoPtr<IInterfaceInfo> mInfo2;

private:
    static const Boolean DEBUG;
};

class CObjectProxy
    : public ElRefBase
    , public IProxy
    , public IObject
    , public IWeakReferenceSource
{
public:
    CObjectProxy();

    virtual ~CObjectProxy();

    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI_(UInt32) AddRef();

    CARAPI_(UInt32) Release();

//    CARAPI_(void) OnLastStrongRef(
//        /* [in] */ const void* id);
//
    CARAPI GetInterfaceCount(
        /* [out] */ Int32* count);

    CARAPI GetAllInterfaceInfos(
        /* [out] */ ArrayOf<IInterfaceInfo*>* interfaceInfos);

    CARAPI GetInterfaceID(
        /* [in] */ IInterface* object,
        /* [out] */ InterfaceID* iid);

    CARAPI GetInterface(
        /* [in] */ UInt32 uIndex,
        /* [out] */ IInterface** object);

    CARAPI GetInterfaceIndex(
        /* [in] */ IInterface* object,
        /* [out] */ UInt32* index);

    CARAPI GetClassID(
        /* [out] */ EMuid* clsid);

    CARAPI GetClassInfo(
        /* [out] */ CIClassInfo** classInfo);

    CARAPI IsStubAlive(
        /* [out] */ Boolean* result);

    CARAPI LinkToDeath(
        /* [in] */ IProxyDeathRecipient* recipient,
        /* [in] */ Int32 flags);

    CARAPI UnlinkToDeath(
        /* [in] */ IProxyDeathRecipient* recipient,
        /* [in] */ Int32 flags,
        /* [out] */ Boolean* result);

    CARAPI Aggregate(
        /* [in] */ AggregateType type,
        /* [in] */ IInterface* object);

    CARAPI GetDomain(
        /* [out] */ IInterface** object);

    CARAPI GetClassID(
        /* [out] */ ClassID* clsid);

    CARAPI GetClassInfo(
        /* [out] */ IInterface** clsInfo);

    CARAPI SetClassInfo(
        /* [in] */ IInterface *clsInfo);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* result);

    CARAPI ToString(
        /* [out] */ String* info);

    CARAPI GetWeakReference(
        /* [out] */ IWeakReference** weakReference);

    static CARAPI S_CreateObject(
        /* [in] */ IClassLoader* loader,
        /* [in] */ ArrayOf<IInterfaceInfo*>* interfaces,
        /* [in] */ IInvocationHandler* invocationHandler,
        /* [out] */ IInterface** prxObject);

public:
    // android::sp<android::IBinder>   mBinder;
    // android::sp<DeathRecipientList> mOrgue;
    CIClassInfo*                    mInfo;
    Int32                           mInterfaceNum;
    CInterfaceProxy*                mInterfaces; // sizeis(mInterfaceNum)
    ICallbackConnector*             mCallbackConnector;
    AutoPtr<IInvocationHandler>     mHandler;
};

} // namespace Reflect
} // namespace Core
} // namespace Elastos

#endif // __ELASTOS_CORE_REFLECT_PROXY_H__
