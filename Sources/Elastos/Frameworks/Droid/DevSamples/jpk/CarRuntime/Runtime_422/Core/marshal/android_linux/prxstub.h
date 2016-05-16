
#ifndef __PRXSTUB_H__
#define __PRXSTUB_H__

#include <binder/Binder.h>
#include <utils/List.h>
#include <utils/Mutex.h>
#include "CRemoteParcel.h"
#include <semaphore.h>
#include <pthread.h>

#include "eltypes.h"
#include "car.h"
#include "marshal_ipc.h"

namespace Elastos {
namespace IPC {

extern ECode Proxy_ProcessMsh_BufferSize(
    /* [in] */ const CIMethodInfo *pMethodInfo,
    /* [in] */ UInt32 *puArgs,
    /* [out] */ UInt32 *puInSize,
    /* [out] */ UInt32 *puOutSize);

extern ECode Proxy_ProcessMsh_In(
    /* [in] */ const CIMethodInfo *pMethodInfo,
    /* [in] */ UInt32 *puArgs,
    /* [in, out] */ IParcel *pParcel);

extern ECode Proxy_ProcessUnmsh_Out(
    /* [in] */ const CIMethodInfo *pMethodInfo,
    /* [in] */ IParcel *pParcel,
    /* [in] */ UInt32 uDataSize,
    /* [in, out] */ UInt32 *puArgs);

extern ECode Stub_ProcessUnmsh_In(
    /* [in] */ const CIMethodInfo *pMethodInfo,
    /* [in] */ IParcel *pParcel,
    /* [in, out] */ UInt32 *puOutBuffer,
    /* [in, out] */ UInt32 *puArgs);

extern ECode Stub_ProcessMsh_Out(
    /* [in] */ const CIMethodInfo *pMethodInfo,
    /* [in] */ UInt32 *puArgs,
    /* [in] */ UInt32 *puOutBuffer,
    /* [in] */ Boolean bOnlyReleaseIn,
    /* [in, out] */ IParcel* pParcel);

#define GET_LENGTH(a) ((a) & 0x3f)
#define GET_IN_INTERFACE_MARK(a) ((a) & 0x80)

class CObjectProxy;

class CInterfaceProxy
{
public:
    static CARAPI_(PInterface) S_Probe(
            /* [in] */ CInterfaceProxy *pThis,
            /* [in] */ REIID riid);

    static CARAPI_(UInt32) S_AddRef(
            /* [in] */ CInterfaceProxy *pThis);

    static CARAPI_(UInt32) S_Release(
            /* [in] */ CInterfaceProxy *pThis);

    static CARAPI S_GetInterfaceID(
            /* [in] */ CInterfaceProxy *pThis,
            /* [in] */ IInterface *pObject,
            /* [out] */ InterfaceID *pIID);

    CARAPI BufferSize(
            /* [in] */ UInt32 uMethodIndex,
            /* [in] */ UInt32 *puArgs,
            /* [out] */ UInt32 *puInSize,
            /* [out] */ UInt32 *puOutSize);

    CARAPI MarshalIn(
            /* [in] */ UInt32 uMethodIndex,
            /* [in] */ UInt32 *puArgs,
            /* [in, out] */ CRemoteParcel *pParcel);

    CARAPI UnmarshalOut(
            /* [in] */ UInt32 uMethodIndex,
            /* [out] */ CRemoteParcel *pParcel,
            /* [in] */ UInt32 *puArgs);

    inline CARAPI_(UInt32) CountMethodArgs(
            /* [in] */ UInt32 uMethodIndex);

    CARAPI_(Boolean) MethodHasOutArgs(
            /* [in] */ UInt32 uMethodIndex);

    inline CARAPI_(Boolean) IsMethodOneway(
            /* [in] */ UInt32 uMethodIndex);

    static CARAPI ProxyEntry(
            /* [in] */ UInt32 *puArgs);

public:
    PVoid                   m_pvVptr;       // must be first member
    PVoid                   m_pvProxyEntry; // must be m_pvVptr's next member

    UInt32                  m_uIndex;       // interface index in object
    CObjectProxy            *m_pOwner;
    const CIInterfaceInfo   *m_pInfo;
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
    android::List< android::sp<CarDeathRecipient> > m_List;
    android::Mutex m_Lock;
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
    IProxyDeathRecipient* m_pObject;
    android::wp<DeathRecipientList> m_List;
};

class CObjectProxy : public IProxy
{
public:
    CObjectProxy();
    virtual ~CObjectProxy();

    CARAPI_(PInterface) Probe(
            /* [in] */ REIID riid);

    CARAPI_(UInt32) AddRef(void);

    CARAPI_(UInt32) Release(void);

    CARAPI GetInterfaceID(
            /* [in] */ IInterface *pObject,
            /* [out] */ InterfaceID *pIID);

    CARAPI GetInterface(
            /* [in] */ UInt32 uIndex,
            /* [out] */ IInterface **ppObj);

    CARAPI GetInterfaceIndex(
             /* [in] */ IInterface *pObj,
             /* [out] */UInt32* pIndex);

    CARAPI GetClassID(
            /* [out] */ EMuid *pClsid);

    CARAPI GetClassInfo(
            /* [out] */ CIClassInfo **ppInfo);

    CARAPI IsStubAlive(
            /* [out] */ Boolean* result);

    CARAPI LinkToDeath(
            /* [in] */ IProxyDeathRecipient* recipient,
            /* [in] */ Int32 flags);

    CARAPI UnlinkToDeath(
            /* [in] */ IProxyDeathRecipient* recipient,
            /* [in] */ Int32 flags,
            /* [out] */ Boolean* result);

    static CARAPI S_CreateObject(
            /* [in] */ REMuid rclsid,
            /* [in] */ const android::sp<android::IBinder> & binder,
            /* [out] */ IProxy **ppIProxy);

public:
    android::sp<android::IBinder> m_pBinder;
    android::sp<DeathRecipientList> m_pOrgue;
    CIClassInfo         *m_pInfo;
    Int32               m_cInterfaces;
    CInterfaceProxy     *m_pInterfaces; // sizeis(m_cInterfaces)
    ICallbackConnector  *m_pICallbackConnector;

    Int32               m_cRef;
};

typedef struct InterfaceStruct {
    struct {
        UInt32 m_vTable[1];
    } *m_vPtr;
}   InterfaceStruct;

class CInterfaceStub
{
public:
    CARAPI UnmarshalIn(
            /* [in] */ const CIMethodInfo *pMethodInfo,
            /* [in] */ CRemoteParcel *pParcel,
            /* [in, out] */ MarshalHeader *pOutHeader,
            /* [in, out] */ UInt32 *puArgs);

    CARAPI MarshalOut(
            /* [in] */ UInt32 *puArgs,
            /* [in] */ MarshalHeader *pInHeader,
            /* [out] */ MarshalHeader *pOutHeader,
            /* [in] */ Boolean bOnlyReleaseIn,
            /* [in, out] */ CRemoteParcel *pParcel);

public:
    union
    {
        IInterface          *m_pObject;
        InterfaceStruct     *m_pInterface;
    };

    const CIInterfaceInfo   *m_pInfo;
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
            /* [out] */ EMuid *pClsid) = 0;

    virtual CARAPI GetInterfaceIndex(
            /* [in] */ IInterface *pObj,
            /* [out] */ UInt32 *puIndex) = 0;

    virtual CARAPI GetClassInfo(
            /* [out] */ CIClassInfo **ppClassInfo) = 0;
};

class CObjectStub : public IStub, public android::BBinder
{
public:
    CObjectStub();
    virtual ~CObjectStub();

    CARAPI_(PInterface) Probe(
            /* [in] */ REIID riid);

    CARAPI_(UInt32) AddRef(void);

    CARAPI_(UInt32) Release(void);

    CARAPI GetInterfaceID(
            /* [in] */ IInterface *pObject,
            /* [out] */ InterfaceID *pIID);

    CARAPI Invoke(
            /* [in] */ const android::Parcel& data,
            /* [out] */ android::Parcel* reply);

    CARAPI GetClassID(
            /* [out] */ EMuid *pClsid);

    CARAPI GetInterfaceIndex(
            /* [in] */ IInterface *pObj,
            /* [out] */ UInt32 *puIndex);

    CARAPI GetClassInfo(
            /* [out] */ CIClassInfo **ppClassInfo);

    static CARAPI S_CreateObject(
            /* [in] */ IInterface *pObject,
            /* [out] */ IStub **ppIStub);

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
    CIClassInfo         *m_pInfo;
    Int32               m_cInterfaces;
    CInterfaceStub      *m_pInterfaces; // size_is(m_cInterfaces)
    Boolean             m_bRequestToQuit;

private:
    static Boolean      s_bThreadPoolStarted;
    static pthread_mutex_t  s_bThreadPoolStartedMutex;
};

} // namespace IPC
} // namespace Elastos

#endif //__PRXSTUB_H__
