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

#ifndef __PRXSTUB_H__
#define __PRXSTUB_H__

#include <binder/Binder.h>
#include <utils/List.h>
#include <utils/Mutex.h>
#include <CRemoteParcel.h>
#include <semaphore.h>
#include <pthread.h>
#include "elrefbase.h"

namespace Elastos {
namespace IPC {

extern ECode Proxy_ProcessMsh_BufferSize(
    /* [in] */ const CIMethodInfo* methodInfo,
    /* [in] */ UInt32* args,
    /* [out] */ UInt32* inSize,
    /* [out] */ UInt32* outSize);

extern ECode Proxy_ProcessMsh_In(
    /* [in] */ const CIMethodInfo* methodInfo,
    /* [in] */ UInt32* args,
    /* [in, out] */ IParcel* parcel);

extern ECode Proxy_ProcessUnmsh_Out(
    /* [in] */ const CIMethodInfo* methodInfo,
    /* [in] */ IParcel* parcel,
    /* [in] */ UInt32 dataSize,
    /* [in, out] */ UInt32* args);

extern ECode Stub_ProcessUnmsh_In(
    /* [in] */ const CIMethodInfo* methodInfo,
    /* [in] */ IParcel* parcel,
    /* [in, out] */ UInt32* outBuffer,
    /* [in, out] */ UInt32* args);

extern ECode Stub_ProcessMsh_Out(
    /* [in] */ const CIMethodInfo* methodInfo,
    /* [in] */ UInt32* args,
    /* [in] */ UInt32* outBuffer,
    /* [in] */ Boolean onlyReleaseIn,
    /* [in, out] */ IParcel* parcel);

#define GET_LENGTH(a) ((a) & 0x3f)
#define GET_IN_INTERFACE_MARK(a) ((a) & 0x80)

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

    CARAPI BufferSize(
        /* [in] */ UInt32 methodIndex,
        /* [in] */ UInt32* args,
        /* [out] */ UInt32* inSize,
        /* [out] */ UInt32* outSize);

    CARAPI MarshalIn(
        /* [in] */ UInt32 methodIndex,
        /* [in] */ UInt32* args,
        /* [in, out] */ CRemoteParcel* parcel);

    CARAPI UnmarshalOut(
        /* [in] */ UInt32 methodIndex,
        /* [out] */ CRemoteParcel* parcel,
        /* [in] */ UInt32* args);

    inline CARAPI_(UInt32) CountMethodArgs(
        /* [in] */ UInt32 methodIndex);

    CARAPI_(Boolean) MethodHasOutArgs(
        /* [in] */ UInt32 methodIndex);

    inline CARAPI_(Boolean) IsMethodOneway(
        /* [in] */ UInt32 methodIndex);

    static CARAPI ProxyEntry(
        /* [in] */ UInt32* args);

public:
    PVoid                   mVTPtr;       // must be first member
    PVoid                   mProxyEntry; // must be mVTPtr's next member

    UInt32                  mIndex;       // interface index in object
    CObjectProxy*           mOwner;
    const CIInterfaceInfo*  mInfo;
};

class CarDeathRecipient;

class DeathRecipientList : public android::RefBase
{
public:
    DeathRecipientList();

    ~DeathRecipientList();

    CARAPI_(void) Add(
        /* [in] */ const android::sp<CarDeathRecipient>& recipient);

    CARAPI_(void) Remove(
        /* [in] */ const android::sp<CarDeathRecipient>& recipient);

    CARAPI_(android::sp<CarDeathRecipient>) Find(
        /* [in] */ IProxyDeathRecipient* recipient);

public:
    android::List< android::sp<CarDeathRecipient> > mDeathRecipients;
    android::Mutex mLock;
};

class CarDeathRecipient : public android::IBinder::DeathRecipient
{
public:
    CarDeathRecipient(
        /* [in] */ IProxyDeathRecipient* recipient,
        /* [in] */ const android::sp<DeathRecipientList>& list);

    ~CarDeathRecipient();

    void binderDied(
        /* [in] */ const android::wp<android::IBinder>& who);

    void clearReference();

    bool matches(
        /* [in] */ IProxyDeathRecipient* recipient);

    void warnIfStillLive();

private:
    IProxyDeathRecipient* mObject;
    android::wp<DeathRecipientList> mDeathRecipients;
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

    CARAPI_(void) OnLastStrongRef(
        /* [in] */ const void* id);

    CARAPI GetInterfaceID(
        /* [in] */ IInterface* object,
        /* [out] */ InterfaceID* iid);

    CARAPI GetInterfaceCount(
        /* [out] */ Int32* count);

    CARAPI GetAllInterfaceInfos(
        /* [out] */ ArrayOf<IInterfaceInfo*>* interfaceInfos);

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
        /* [in] */ REMuid rclsid,
        /* [in] */ const android::sp<android::IBinder>& binder,
        /* [out] */ IProxy** proxy);

public:
    android::sp<android::IBinder>   mBinder;
    android::sp<DeathRecipientList> mOrgue;
    CIClassInfo*                    mInfo;
    Int32                           mInterfaceNum;
    CInterfaceProxy*                mInterfaces; // sizeis(mInterfaceNum)
    ICallbackConnector*             mCallbackConnector;
};

typedef struct InterfaceStruct {
    struct {
        UInt32 mVTable[1];
    }* mVPtr;
} InterfaceStruct;

class CInterfaceStub
{
public:
    CARAPI UnmarshalIn(
        /* [in] */ const CIMethodInfo* methodInfo,
        /* [in] */ CRemoteParcel* parcel,
        /* [in, out] */ MarshalHeader* outHeader,
        /* [in, out] */ UInt32* args);

    CARAPI MarshalOut(
        /* [in] */ UInt32* args,
        /* [in] */ MarshalHeader* inHeader,
        /* [out] */ MarshalHeader* outHeader,
        /* [in] */ Boolean onlyReleaseIn,
        /* [in, out] */ CRemoteParcel* parcel);

public:
    union
    {
        IInterface*         mObject;
        InterfaceStruct*    mInterface;
    };

    const CIInterfaceInfo*  mInfo;
};

interface IStub : public IInterface
{
    enum {
        INVOKE = android::IBinder::FIRST_CALL_TRANSACTION,
        GET_CLASS_INFO,
        RELEASE
    };

    virtual CARAPI Invoke(
        /* [in] */ const android::Parcel& data,
        /* [out] */ android::Parcel* reply) = 0;

    virtual CARAPI GetClassID(
        /* [out] */ EMuid* clsid) = 0;

    virtual CARAPI GetInterfaceIndex(
        /* [in] */ IInterface* object,
        /* [out] */ UInt32* index) = 0;

    virtual CARAPI GetClassInfo(
        /* [out] */ CIClassInfo** classInfo) = 0;
};

class CObjectStub : public IStub, public android::BBinder
{
public:
    CObjectStub();

    virtual ~CObjectStub();

    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI_(UInt32) AddRef();

    CARAPI_(UInt32) Release();

    CARAPI GetInterfaceID(
        /* [in] */ IInterface* object,
        /* [out] */ InterfaceID* iid);

    CARAPI Invoke(
        /* [in] */ const android::Parcel& data,
        /* [out] */ android::Parcel* reply);

    CARAPI GetClassID(
        /* [out] */ EMuid* clsid);

    CARAPI GetInterfaceIndex(
        /* [in] */ IInterface* object,
        /* [out] */ UInt32* index);

    CARAPI GetClassInfo(
        /* [out] */ CIClassInfo** classInfo);

    static CARAPI S_CreateObject(
        /* [in] */ IInterface* object,
        /* [out] */ IStub** stub);

private:
    static CARAPI_(void) StartThreadPool();

    android::status_t onTransact(
        /* [in] */ uint32_t code,
        /* [in] */ const android::Parcel& data,
        /* [out] */ android::Parcel* reply,
        /* [in] */ uint32_t flags = 0);

    CARAPI_(void) PrintIpcLog(
        /* [in] */ MarshalHeader* header);

    void onLastStrongRef(
        /* [in] */ const void* id);

public:
    CIClassInfo*        mInfo;
    Int32               mInterfaceNum;
    CInterfaceStub*     mInterfaces; // size_is(mInterfaceNum)
    Boolean             mRequestToQuit;

private:
    static Boolean          sThreadPoolStarted;
    static pthread_mutex_t  sThreadPoolStartedMutex;
};

} // namespace IPC
} // namespace Elastos

#endif //__PRXSTUB_H__
