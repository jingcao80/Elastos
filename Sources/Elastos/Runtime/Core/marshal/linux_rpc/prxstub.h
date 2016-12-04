
#ifndef __PRXSTUB_H__
#define __PRXSTUB_H__

#include <utils/List.h>
#include <utils/Mutex.h>
#include <CRemoteParcel.h>
#include <threadpoolexecutor.h>

#define GET_LENGTH(a) ((a) & 0x3f)
#define GET_IN_INTERFACE_MARK(a) ((a) & 0x80)

namespace Elastos {
namespace RPC {

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

    static CARAPI ProxyEntry_RPC(
        /* [in] */ UInt32* args);

public:
    PVoid                   mVTPtr;       // must be first member
    PVoid                   mProxyEntry; // must be mVTPtr's next member

    UInt32                  mIndex;       // interface index in object
    CObjectProxy*           mOwner;
    const CIInterfaceInfo*  mInfo;
};

class CarDeathRecipient;

class DeathRecipientList
    : public ElRefBase
    , public IWeakReferenceSource
{
public:
    DeathRecipientList();

    ~DeathRecipientList();

    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI_(UInt32) AddRef();

    CARAPI_(UInt32) Release();

    CARAPI GetInterfaceID(
        /* [in] */ IInterface* object,
        /* [out] */ InterfaceID* iid);

    CARAPI GetWeakReference(
        /* [out] */ IWeakReference** weakReference);

    CARAPI_(void) Add(
        /* [in] */ const AutoPtr<CarDeathRecipient>& recipient);

    CARAPI_(void) Remove(
        /* [in] */ const AutoPtr<CarDeathRecipient>& recipient);

    CARAPI_(AutoPtr<CarDeathRecipient>) Find(
        /* [in] */ IProxyDeathRecipient* recipient);

    CARAPI_(void) NotifyAll(
        /* [in] */ CObjectProxy* proxy);

public:
    android::List< AutoPtr<CarDeathRecipient> > mDeathRecipients;
    android::Mutex mLock;
};

class CarDeathRecipient : public ElLightRefBase
{
public:
    CarDeathRecipient(
        /* [in] */ IProxyDeathRecipient* recipient,
        /* [in] */ const AutoPtr<DeathRecipientList>& list);

    ~CarDeathRecipient();

    CARAPI_(void) StubDied(
        /* [in] */ const CObjectProxy* who);

    CARAPI_(void) ClearReference();

    CARAPI_(Boolean) Matches(
        /* [in] */ IProxyDeathRecipient* recipient);

    CARAPI_(void) WarnIfStillLive();

private:
    IProxyDeathRecipient* mObject;
    AutoPtr<IWeakReference> mDeathRecipients;  //DeathRecipientList
};

class CObjectProxy : public IProxy
{
public:
    class PingRunnable : public ThreadPoolExecutor::Runnable
    {
    public:
        PingRunnable(
            /* [in] */ CObjectProxy* proxy)
            : mHost(proxy)
        {}

        CARAPI Run();

    private:
        CObjectProxy* mHost;
    };

public:
    CObjectProxy();

    virtual ~CObjectProxy();

    CARAPI_(PInterface) Probe(
            /* [in] */ REIID riid);

    CARAPI_(UInt32) AddRef();

    CARAPI_(UInt32) Release();

    CARAPI GetInterfaceID(
        /* [in] */ IInterface* object,
        /* [out] */ InterfaceID* iid);

    CARAPI GetInterfaceCount(
        /* [out] */ Int32* count);

    CARAPI GetAllInterfaceInfos(
        /* [out] */ ArrayOf<IInterfaceInfo*>* interfaceInfos);

    CARAPI GetInterface(
        /* [in] */ UInt32 index,
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

    CARAPI StubDied();

    static CARAPI S_CreateObject(
        /* [in] */ REMuid rclsid,
        /* [in] */ const String& address,
        /* [out] */ IProxy** proxy);

public:
    String              mNetAddress;
    Int32               mSocketfd;
    AutoPtr<DeathRecipientList> mOrgue;
    CIClassInfo*        mInfo;
    Int32               mInterfaceNum;
    CInterfaceProxy*    mInterfaces; // sizeis(mInterfaceNum)
    ICallbackConnector* mCallbackConnector;
    AutoPtr<PingRunnable> mPingTask;
    Boolean             mStubDied;
    android::Mutex      mLock;
    Int32               mRef;
};

typedef struct InterfaceStruct {
    struct {
        UInt32 mVTable[1];
    } *mVPtr;
}   InterfaceStruct;

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
    virtual CARAPI Invoke(
        /* [in] */ CRemoteParcel* data,
        /* [out] */ CRemoteParcel* reply) = 0;

    virtual CARAPI GetClassID(
        /* [out] */ EMuid* clsid) = 0;

    virtual CARAPI GetInterfaceIndex(
        /* [in] */ IInterface* object,
        /* [out] */ UInt32* index) = 0;

    virtual CARAPI GetClassInfo(
        /* [out] */ CIClassInfo** classInfo) = 0;
};

class CObjectStub
    : public ElLightRefBase
    , public IStub
{
private:
    class TransactRunnable : public ThreadPoolExecutor::Runnable
    {
    public:
        TransactRunnable(
            /* [in] */ Int32 connfd,
            /* [in] */ Int32 events,
            /* [in] */ void* obj,
            /* [in] */ CRemoteParcel* data)
            : mConnfd(connfd)
            , mEvents(events)
            , mObject(obj)
            , mData(data)
        {}

        CARAPI Run();

    private:
        Int32 mConnfd;
        Int32 mEvents;
        void* mObject;
        AutoPtr<CRemoteParcel> mData;
    };

public:
    CObjectStub();

    virtual ~CObjectStub();

    CARAPI_(PInterface) Probe(
            /* [in] */ REIID riid);

    CARAPI_(UInt32) AddRef(void);

    CARAPI_(UInt32) Release(void);

    CARAPI GetInterfaceID(
        /* [in] */ IInterface* object,
        /* [out] */ InterfaceID* iid);

    CARAPI Invoke(
        /* [in] */ CRemoteParcel* data,
        /* [out] */ CRemoteParcel* reply);

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
    CARAPI OnTransact(
        /* [in] */ const String& code,
        /* [in] */ CRemoteParcel* data,
        /* [out] */ CRemoteParcel* reply,
        /* [in] */ Int32 flags = 0);

    CARAPI_(void) PrintIpcLog(
        /* [in] */ MarshalHeader* header);

    // void onLastStrongRef(
    //     /* [in] */ const void* id);

    static Int32 ListenCallback(
        /* [in] */ Int32 socketfd,
        /* [in] */ Int32 events,
        /* [in] */ void* data);

    static Int32 AcceptCallback(
        /* [in] */ Int32 connfd,
        /* [in] */ Int32 events,
        /* [in] */ void* data);

public:
    CIClassInfo*        mInfo;
    Int32               mInterfaceNum;
    CInterfaceStub*     mInterfaces; // size_is(m_cInterfaces)
    Boolean             mRequestToQuit;
    Int32               mSocketfd;
    Int32               mConnfd;
    String              mNetAddress;
};

} // namespace RPC
} // namespace Elastos

#endif //__PRXSTUB_H__
