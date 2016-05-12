
#ifndef __CARCALLBACKPROXY_H__
#define __CARCALLBACKPROXY_H__

#include <v8.h>
#include <pthread.h>
#include "eldef.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Node {

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
            /* [in] */ v8::Persistent<v8::Value>* params,
            /* [in] */ v8::Isolate* isolate)
            : mObject(object)
            , mMethodInfo(methodInfo)
            , mParamCount(paramCount)
            , mV8Params(params)
            , mIsolate(isolate)
        {}

        ~Callback();

        void Call();

    private:
        AutoPtr<CarCallbackObject> mObject;
        AutoPtr<IMethodInfo> mMethodInfo;
        Int32 mParamCount;
        v8::Persistent<v8::Value>* mV8Params;
        v8::Isolate* mIsolate;
    };

public:
    CarCallbackInterfaceProxy()
        : mMethodInfos(NULL)
    {}

    ~CarCallbackInterfaceProxy();

    static CARAPI_(PInterface) S_Probe(
        /* [in] */ CarCallbackInterfaceProxy* pThis,
        /* [in] */ REIID riid);

    static CARAPI_(UInt32) S_AddRef(
            /* [in] */ CarCallbackInterfaceProxy* pThis);

    static CARAPI_(UInt32) S_Release(
            /* [in] */ CarCallbackInterfaceProxy* pThis);

    static CARAPI S_GetInterfaceID(
        /* [in] */ CarCallbackInterfaceProxy* pThis,
        /* [in] */ IInterface* pObject,
        /* [out] */ InterfaceID* pIID);

    CARAPI ReadParam(
        /* [in] */ IMethodInfo* methodInfo,
        /* [in] */ UInt32* puArgs,
        /* [in] */ Int32 paramCount,
        /* [in] */ v8::Persistent<v8::Value>* v8Args);

    static CARAPI ProxyEntry(
        /* [in] */ UInt32 *puArgs);

public:
    PVoid mVptr;       // must be first member
    PVoid mProxyEntry; // must be m_pvVptr's next member

    AutoPtr<IInterfaceInfo> mInterfaceInfo;
    InterfaceID mIID;
    AutoPtr< ArrayOf<IMethodInfo*> > mMethodInfos;

    CarCallbackObject* mOwner;
};

class CarCallbackObject
    : public Elastos::Core::Object
{
public:
    ~CarCallbackObject();

    CARAPI ToString(
        /* [out] */ Elastos::String* info);

    static CARAPI_(CarCallbackObject*) S_CreateObject(
        /* [in] */ IInterfaceInfo* itfInfo,
        /* [in] */ v8::Local<v8::Object> v8Object);

public:
    CarCallbackInterfaceProxy* mInterface;

    //node.js main thread
    pthread_t mMainThread;
    v8::Isolate* mIsolate;
    v8::Persistent<v8::Object> mV8Object;
};

} // namespace Node
} // namespace Elastos

#endif //__CARCALLBACKPROXY_H__
