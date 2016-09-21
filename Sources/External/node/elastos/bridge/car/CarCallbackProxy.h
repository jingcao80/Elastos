
#ifndef __CARCALLBACKPROXY_H__
#define __CARCALLBACKPROXY_H__

#if ENABLE(CAR_BRIDGE)

#include <elastos.h>
#include <v8.h>
#include <npruntime_impl.h>

namespace JSC {
namespace Bindings {

class CarCallbackObject;

class CarCallbackInterfaceProxy
{
public:
    class Callback
    {
    public:
        Callback(
            /* [in] */ CarCallbackObject* object,
            /* [in] */ IMethodInfo* methodInfo,
            /* [in] */ Int32 paramCount,
            /* [in] */ NPVariant* params,
            /* [in] */ Int32* outParamPtrs,
            /* [in] */ Int32* outParamTypes,
            /* [in] */ Int32* outParamElementTypes,
            /* [in] */ v8::Isolate* isolate)
            : mObject(object)
            , mMethodInfo(methodInfo)
            , mParamCount(paramCount)
            , mNPParams(params)
            , mOutParamPtrs(outParamPtrs)
            , mOutParamTypes(outParamTypes)
            , mOutParamElementTypes(outParamElementTypes)
            , mIsolate(isolate)
        {}

        ~Callback();

        void Call();

    private:
        v8::Handle<v8::Value>* ConvertParams();

    private:
        AutoPtr<CarCallbackObject> mObject;
        IMethodInfo* mMethodInfo;
        Int32 mParamCount;
        NPVariant* mNPParams;

        Int32* mOutParamPtrs;

        Int32* mOutParamTypes;
        Int32* mOutParamElementTypes;

    public:
        v8::Isolate* mIsolate;
    };

public:
    CarCallbackInterfaceProxy()
        : mMethodInfos(NULL)
    {}

    ~CarCallbackInterfaceProxy();

    static CARAPI_(PInterface) S_Probe(
        /* [in] */ CarCallbackInterfaceProxy *pThis,
        /* [in] */ REIID riid);

    static CARAPI_(UInt32) S_AddRef(
            /* [in] */ CarCallbackInterfaceProxy *pThis);

    static CARAPI_(UInt32) S_Release(
            /* [in] */ CarCallbackInterfaceProxy *pThis);

    static CARAPI S_GetInterfaceID(
        /* [in] */ CarCallbackInterfaceProxy *pThis,
        /* [in] */ IInterface *pObject,
        /* [out] */ InterfaceID *pIID);

    CARAPI ReadParam(
        /* [in] */ IMethodInfo* methodInfo,
        /* [in] */ UInt32 *puArgs,
        /* [in] */ Int32 paramCount,
        /* [in] */ NPVariant* jsArgs,
        /* [in] */ Int32* outParamPtrs,
        /* [in] */ Int32* outParamTypes,
        /* [in] */ Int32* outParamElementTypes,
        /* [out] */ CarValue** ppCarArgs,
        /* [in] */ bool* hasOutParams);

    CARAPI ReadParam_bak(
        /* [in] */ IMethodInfo* methodInfo,
        /* [in] */ UInt32 *puArgs,
        /* [out] */ CarValue** ppCarArgs);

    static CARAPI ProxyEntry(
        /* [in] */ UInt32 *puArgs);

public:
    PVoid mVptr;       // must be first member
    PVoid mProxyEntry; // must be m_pvVptr's next member

    AutoPtr<IInterfaceInfo> mInterfaceInfo;
    InterfaceID mIID;
    ArrayOf<IMethodInfo*>* mMethodInfos;

    CarCallbackObject* mOwner;
};

class CarCallbackObject
    : public ElRefBase
    , public IObject
    , public IWeakReferenceSource
{
public:
    ~CarCallbackObject();

    CARAPI_(PInterface) Probe(
            /* [in] */ REIID riid);

    CARAPI_(UInt32) AddRef();

    CARAPI_(UInt32) Release();

    CARAPI GetInterfaceID(
            /* [in] */ IInterface *pObject,
            /* [out] */ InterfaceID *pIID);

    CARAPI Aggregate(
        /* [in] */ AggregateType type,
        /* [in] */ IInterface* object);

    CARAPI GetDomain(
        /* [out] */ IInterface** object);

    CARAPI GetClassID(
        /* [out] */ ClassID* clsid);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* result);

    CARAPI ToString(
        /* [out] */ Elastos::String* info);

    CARAPI GetWeakReference(
        /* [out] */ IWeakReference** weakReference);

    static CARAPI_(CarCallbackObject*) S_CreateObject(
        /* [in] */ IInterfaceInfo* itfInfo,
        /* [in] */ NPObject* object);

public:
    CarCallbackInterfaceProxy* mInterface;
    NPObject* mObject;

    pthread_t mMainThread;
    v8::Isolate* mIsolate;
    v8::Handle<v8::Context> mContext;
};

} // namespace Bindings
} // namespace JSC

#endif // ENABLE(CAR_BRIDGE)

#endif //__CARCALLBACKPROXY_H__