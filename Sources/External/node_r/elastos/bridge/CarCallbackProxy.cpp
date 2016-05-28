
#include "CarCallbackProxy.h"
#include "V8Utility.h"
#include "V8CarObject.h"
#include <sys/mman.h>
#include <utils/Log.h>
#include <uv.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

#define ROUND8(n)       (((n)+7)&~7)   // round up to multiple of 8 bytes

#define MSH_MAX_METHODS     (192 + 4)

#ifndef PAGE_SIZE
#define PAGE_SIZE (1u << 12)
#endif
#ifndef PAGE_MASK
#define PAGE_MASK (~(PAGE_SIZE - 1))
#endif
#ifndef PAGE_ALIGN
#define PAGE_ALIGN(va) (((va)+PAGE_SIZE-1)&PAGE_MASK)
#endif

UInt32 s_proxyEntryAddress = 0;

#define PROXY_ENTRY_BASE    s_proxyEntryAddress
#define PROXY_ENTRY_SIZE    0x10
#define PROXY_ENTRY_MASK    0x0f
#define PROXY_ENTRY_SHIFT   4
#define PROXY_ENTRY_NUM     0xc0

extern "C" void ProxyEntryFunc(void);

#define DECL_SYS_PROXY_ENTRY()   \
    __asm__(                     \
    ".text;"                     \
    ".align 4;"                  \
    ".globl ProxyEntryFunc;"     \
    "ProxyEntryFunc:"            \
    ".code 32;"                  \
    "push {r0 - r3};"            \
    "push {lr};"                 \
    "mov  r1, #0xff;"            \
    "ldr  pc, [r0, #4];"         \
)

DECL_SYS_PROXY_ENTRY();

namespace Elastos {
namespace Node {

// {8CC78507-90A3-4771-9122-F279D7D20DCA}
static const InterfaceID EIID_CarCallbackObject =
        { 0x8cc78507, 0x90a3, 0x4771, { 0x91, 0x22, 0xf2, 0x79, 0xd7, 0xd2, 0xd, 0xca } };

static UInt32 g_marshalVtbl[MSH_MAX_METHODS];

EXTERN_C void __ProxyEntry(void);

EXTERN_C ECode GlobalProxyEntry(UInt32 *puArgs)
{
    return CarCallbackInterfaceProxy::ProxyEntry(puArgs);
}

#define DECL_PROXY_ENTRY()           \
    __asm__(                         \
        ".text;"                     \
        ".align 4;"                  \
        ".globl __ProxyEntry;"       \
        "__ProxyEntry:"              \
        ".code 32;"                  \
        "push   {r1};"               \
        "add    r0, sp, #4;"         \
        "bl     GlobalProxyEntry;"   \
        "add    sp, sp, #4;"         \
        "pop    {lr};"               \
        "pop    {r1};"               \
        "pop    {r1 - r3};"          \
        "push   {lr};"               \
        "pop    {pc};"               \
    )

DECL_PROXY_ENTRY();

static int InitCarCallbackProxyEntry()
{
    if (s_proxyEntryAddress != 0) return 0;

    s_proxyEntryAddress =
            (UInt32)mmap((void*)0,
                          PAGE_ALIGN(PROXY_ENTRY_NUM * PROXY_ENTRY_SIZE),
                          PROT_READ | PROT_WRITE | PROT_EXEC,
                          MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (s_proxyEntryAddress == 0) {
        ALOGE("InitCarCallbackProxyEntry: out of memory");
        return -1;
    }
    char * p = (char *)s_proxyEntryAddress;
    for (int n = 0; n < PROXY_ENTRY_NUM; n++) {
        memcpy(p, (void *)&ProxyEntryFunc, PROXY_ENTRY_SIZE);
        p[8] = n;
        p += PROXY_ENTRY_SIZE;
    }

    g_marshalVtbl[0] = (UInt32)&CarCallbackInterfaceProxy::S_Probe;
    g_marshalVtbl[1] = (UInt32)&CarCallbackInterfaceProxy::S_AddRef;
    g_marshalVtbl[2] = (UInt32)&CarCallbackInterfaceProxy::S_Release;
    g_marshalVtbl[3] = (UInt32)&CarCallbackInterfaceProxy::S_GetInterfaceID;
    for (int m = 4; m < MSH_MAX_METHODS; m++) {
        g_marshalVtbl[m] = (UInt32)(PROXY_ENTRY_BASE + ((m - 4) << PROXY_ENTRY_SHIFT));
    }
    return 0;
}

//============================================================
//  CarCallbackInterfaceProxy
//============================================================
CarCallbackInterfaceProxy::CarCallbackInterfaceProxy()
{}

CarCallbackInterfaceProxy::~CarCallbackInterfaceProxy()
{}

PInterface CarCallbackInterfaceProxy::S_Probe(
    /* [in] */ CarCallbackInterfaceProxy *pThis,
    /* [in] */ REIID riid)
{
    return pThis->mOwner->Probe(riid);
}

UInt32 CarCallbackInterfaceProxy::S_AddRef(
    /* [in] */ CarCallbackInterfaceProxy *pThis)
{
    return pThis->mOwner->AddRef();
}

UInt32 CarCallbackInterfaceProxy::S_Release(
    /* [in] */ CarCallbackInterfaceProxy *pThis)
{
    return pThis->mOwner->Release();
}

ECode CarCallbackInterfaceProxy::S_GetInterfaceID(
    /* [in] */ CarCallbackInterfaceProxy *pThis,
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    return pThis->mOwner->GetInterfaceID(pObject, pIID);
}

ECode CarCallbackInterfaceProxy::ReadParam(
    /* [in] */ IMethodInfo* methodInfo,
    /* [in] */ UInt32* puArgs,
    /* [in] */ Int32 paramCount,
    /* [in] */ v8::Persistent<v8::Value>* v8Args)
{
    v8::Isolate* isolate = mOwner->mIsolate;

    AutoPtr< ArrayOf<IParamInfo*> > paramInfos = ArrayOf<IParamInfo*>::Alloc(paramCount);
    methodInfo->GetAllParamInfos(paramInfos);

    for (Int32 i = 0; i < paramCount; i++) {
        IParamInfo* paramInfo = (*paramInfos)[i];
        ParamIOAttribute paramIOAttr;
        paramInfo->GetIOAttribute(&paramIOAttr);
        AutoPtr<IDataTypeInfo> paramTypeInfo;
        paramInfo->GetTypeInfo((IDataTypeInfo**)&paramTypeInfo);
        CarDataType paramDataType;
        paramTypeInfo->GetDataType(&paramDataType);

        switch (paramIOAttr) {
            case ParamIOAttribute_In: {
                switch (paramDataType) {
                    case CarDataType_Boolean:
                    {
                        v8Args[i].Reset(isolate, v8::Boolean::New(isolate, (bool)*puArgs));
                        puArgs++;
                        break;
                    }
                    case CarDataType_Byte:
                    case CarDataType_Char32:
                    case CarDataType_Int16:
                    case CarDataType_Int32:
                    case CarDataType_ECode:
                    case CarDataType_Enum:
                    {
                        v8Args[i].Reset(isolate, v8::Number::New(isolate, (double)*puArgs));
                        puArgs++;
                        break;
                    }
                    case CarDataType_Float:
                    {
                        v8Args[i].Reset(isolate, v8::Number::New(isolate, (double)*(Float*)puArgs));
                        puArgs++;
                        break;
                    }
                    case CarDataType_Int64:
                    case CarDataType_Double:
                    {
                        puArgs = (UInt32*)ROUND8((Int32)puArgs);
                        v8Args[i].Reset(isolate, v8::Number::New(isolate, (double)*(UInt64*)puArgs));
                        puArgs += 2;
                        break;
                    }
                    case CarDataType_String:
                    {
                        String& str = **(String**)puArgs;
                        v8Args[i].Reset(isolate, v8::String::NewFromUtf8(isolate, str.string()));
                        puArgs++;
                        break;
                    }
                    case CarDataType_EMuid:
                    {
                        EMuid& iid = *(EMuid*)puArgs;

                        v8::Local<v8::Array> v8Array = v8::Local<v8::Array>::New(isolate, v8::Array::New(isolate, 4));
                        v8Array->Set(0, v8::Number::New(isolate, iid.mData1));
                        v8Array->Set(1, v8::Number::New(isolate, iid.mData2));
                        v8Array->Set(2, v8::Number::New(isolate, iid.mData3));
                        v8::Local<v8::Array> v8Array1 = v8::Local<v8::Array>::New(isolate, v8::Array::New(isolate, 4));
                        for(Int32 j = 0; j < 8; j++) {
                            v8Array1->Set(j, v8::Number::New(isolate, iid.mData4[j]));
                        }
                        v8Array->Set(3, v8Array1);

                        v8Args[i].Reset(isolate, v8Array);

                        puArgs += sizeof(EMuid) / 4;
                        break;
                    }
                    case CarDataType_EGuid:
                    {
                        EGuid& cid = *(EGuid*)puArgs;
                        EMuid& iid = cid.mClsid;

                        v8::Local<v8::Array> v8Array = v8::Local<v8::Array>::New(isolate, v8::Array::New(isolate, 4));
                        v8Array->Set(0, v8::Number::New(isolate, iid.mData1));
                        v8Array->Set(1, v8::Number::New(isolate, iid.mData2));
                        v8Array->Set(2, v8::Number::New(isolate, iid.mData3));
                        v8::Local<v8::Array> v8Array1 = v8::Local<v8::Array>::New(isolate, v8::Array::New(isolate, 4));
                        for(Int32 j = 0; j < 8; j++) {
                            v8Array1->Set(j, v8::Number::New(isolate, iid.mData4[j]));
                        }
                        v8Array->Set(3, v8Array1);

                        v8::Local<v8::Object> v8Object = v8::Local<v8::Object>::New(isolate, v8::Object::New(isolate));
                        v8Object->Set(v8::String::NewFromUtf8(isolate, "mClsid"), v8Array);
                        v8Object->Set(v8::String::NewFromUtf8(isolate, "mUunm"), v8::String::NewFromUtf8(isolate, cid.mUunm));
                        v8Object->Set(v8::String::NewFromUtf8(isolate, "mCarcode"), v8::Number::New(isolate, cid.mCarcode));

                        v8Args[i].Reset(isolate, v8Object);

                        puArgs += sizeof(EGuid) / 4;
                        break;
                    }
                    case CarDataType_Interface:
                    {
                        IInterface* carObject = (IInterface*)*puArgs;

                        if (carObject->Probe(EIID_CarCallbackObject) == NULL) {
                            CarValue value;
                            value.mType = CarDataType_Interface;
                            value.mObjectValue = carObject;
                            v8::Local<v8::Object> v8Object = V8CarObject::New(isolate, value);

                            v8Args[i].Reset(isolate, v8Object);
                        }
                        else {
                            Logger::E("CarCallbackInterfaceProxy", "CarCallbackObject unsupported!");
                        }

                        puArgs++;
                        break;
                    }
                    case CarDataType_ArrayOf:
                    {
                        CarQuintet* carArray = (CarQuintet*)*puArgs;

                        AutoPtr<IDataTypeInfo> elementTypeInfo;
                        ICarArrayInfo::Probe(paramTypeInfo)->GetElementTypeInfo((IDataTypeInfo**)&elementTypeInfo);
                        CarDataType elementDataType;
                        elementTypeInfo->GetDataType(&elementDataType);

                        switch(elementDataType) {
                            case CarDataType_Byte:
                            {
                                Int32 count = carArray->mSize / sizeof(Byte);
                                Byte* buffer = (Byte*)carArray->mBuf;

                                v8::Local<v8::Array> v8Array = v8::Local<v8::Array>::New(isolate, v8::Array::New(isolate, count));
                                for (Int32 j = 0; j < count; j++) {
                                    v8Array->Set(j, v8::Number::New(isolate, buffer[j]));
                                }

                                v8Args[i].Reset(isolate, v8Array);
                                break;
                            }
                            case CarDataType_Int16:
                            {
                                Int32 count = carArray->mSize / sizeof(Int16);
                                Int16* buffer = (Int16*)carArray->mBuf;

                                v8::Local<v8::Array> v8Array = v8::Local<v8::Array>::New(isolate, v8::Array::New(isolate, count));
                                for (Int32 j = 0; j < count; j++) {
                                    v8Array->Set(j, v8::Number::New(isolate, buffer[j]));
                                }

                                v8Args[i].Reset(isolate, v8Array);
                                break;
                            }
                            case CarDataType_Char32:
                            case CarDataType_Int32:
                            case CarDataType_ECode:
                            case CarDataType_Enum:
                            {
                                Int32 count = carArray->mSize / sizeof(Int32);
                                Int32* buffer = (Int32*)carArray->mBuf;

                                v8::Local<v8::Array> v8Array = v8::Local<v8::Array>::New(isolate, v8::Array::New(isolate, count));
                                for (Int32 j = 0; j < count; j++) {
                                    v8Array->Set(j, v8::Number::New(isolate, buffer[j]));
                                }

                                v8Args[i].Reset(isolate, v8Array);
                                break;
                            }
                            case CarDataType_Float:
                            {
                                Int32 count = carArray->mSize / sizeof(Float);
                                Float* buffer = (Float*)carArray->mBuf;

                                v8::Local<v8::Array> v8Array = v8::Local<v8::Array>::New(isolate, v8::Array::New(isolate, count));
                                for (Int32 j = 0; j < count; j++) {
                                    v8Array->Set(j, v8::Number::New(isolate, buffer[j]));
                                }

                                v8Args[i].Reset(isolate, v8Array);
                                break;
                            }
                            case CarDataType_Int64:
                            case CarDataType_Double:
                            {
                                Int32 count = carArray->mSize / sizeof(Double);
                                Double* buffer = (Double*)carArray->mBuf;

                                v8::Local<v8::Array> v8Array = v8::Local<v8::Array>::New(isolate, v8::Array::New(isolate, count));
                                for (Int32 j = 0; j < count; j++) {
                                    v8Array->Set(j, v8::Number::New(isolate, buffer[j]));
                                }

                                v8Args[i].Reset(isolate, v8Array);
                                break;
                            }
                            case CarDataType_Boolean:
                            {
                                Int32 count = carArray->mSize / sizeof(Boolean);
                                Boolean* buffer = (Boolean*)carArray->mBuf;

                                v8::Local<v8::Array> v8Array = v8::Local<v8::Array>::New(isolate, v8::Array::New(isolate, count));
                                for (Int32 j = 0; j < count; j++) {
                                    v8Array->Set(j, v8::Boolean::New(isolate, buffer[j]));
                                }

                                v8Args[i].Reset(isolate, v8Array);
                                break;
                            }
                            case CarDataType_String:
                            {
                                Int32 count = carArray->mSize / sizeof(String);
                                String* buffer = (String*)carArray->mBuf;

                                v8::Local<v8::Array> v8Array = v8::Local<v8::Array>::New(isolate, v8::Array::New(isolate, count));
                                for (Int32 j = 0; j < count; j++) {
                                    v8Array->Set(j, v8::String::NewFromUtf8(isolate, buffer[j].string()));
                                }

                                v8Args[i].Reset(isolate, v8Array);
                                break;
                            }
                            case CarDataType_EMuid:
                            {
                                Int32 count = carArray->mSize / sizeof(EMuid);
                                EMuid* buffer = (EMuid*)carArray->mBuf;

                                v8::Local<v8::Array> v8Array = v8::Local<v8::Array>::New(isolate, v8::Array::New(isolate, count));
                                for (Int32 j = 0; j < count; j++) {
                                    EMuid& iid = buffer[j];

                                    v8::Local<v8::Array> v8EMuid = v8::Local<v8::Array>::New(isolate, v8::Array::New(isolate, 4));
                                    v8EMuid->Set(0, v8::Number::New(isolate, iid.mData1));
                                    v8EMuid->Set(1, v8::Number::New(isolate, iid.mData2));
                                    v8EMuid->Set(2, v8::Number::New(isolate, iid.mData3));
                                    v8::Local<v8::Array> v8EMuidData4 = v8::Local<v8::Array>::New(isolate, v8::Array::New(isolate, 4));
                                    for(Int32 k = 0; k < 8; k++) {
                                        v8EMuidData4->Set(k, v8::Number::New(isolate, iid.mData4[k]));
                                    }
                                    v8EMuid->Set(3, v8EMuidData4);

                                    v8Array->Set(j, v8EMuid);
                                }

                                v8Args[i].Reset(isolate, v8Array);
                                break;
                            }
                            case CarDataType_EGuid:
                            {
                                Int32 count = carArray->mSize / sizeof(EMuid);
                                EMuid* buffer = (EMuid*)carArray->mBuf;

                                v8::Local<v8::Array> v8Array = v8::Local<v8::Array>::New(isolate, v8::Array::New(isolate, count));
                                for (Int32 j = 0; j < count; j++) {
                                    EGuid& cid = *(EGuid*)puArgs;
                                    EMuid& iid = cid.mClsid;

                                    v8::Local<v8::Array> v8EMuid = v8::Local<v8::Array>::New(isolate, v8::Array::New(isolate, 4));
                                    v8EMuid->Set(0, v8::Number::New(isolate, iid.mData1));
                                    v8EMuid->Set(1, v8::Number::New(isolate, iid.mData2));
                                    v8EMuid->Set(2, v8::Number::New(isolate, iid.mData3));
                                    v8::Local<v8::Array> v8EMuidData4 = v8::Local<v8::Array>::New(isolate, v8::Array::New(isolate, 4));
                                    for(Int32 k = 0; k < 8; k++) {
                                        v8EMuidData4->Set(k, v8::Number::New(isolate, iid.mData4[k]));
                                    }
                                    v8EMuid->Set(3, v8EMuidData4);

                                    v8::Local<v8::Object> v8EGuid = v8::Local<v8::Object>::New(isolate, v8::Object::New(isolate));
                                    v8EGuid->Set(v8::String::NewFromUtf8(isolate, "mClsid"), v8EMuid);
                                    v8EGuid->Set(v8::String::NewFromUtf8(isolate, "mUunm"), v8::String::NewFromUtf8(isolate, cid.mUunm));
                                    v8EGuid->Set(v8::String::NewFromUtf8(isolate, "mCarcode"), v8::Number::New(isolate, cid.mCarcode));

                                    v8Array->Set(j, v8EGuid);
                                }

                                v8Args[i].Reset(isolate, v8Array);
                                break;
                            }
                            case CarDataType_Interface:
                            {
                                Int32 count = carArray->mSize / sizeof(Int32);
                                IInterface** buffer = (IInterface**)carArray->mBuf;

                                v8::Local<v8::Array> v8Array = v8::Local<v8::Array>::New(isolate, v8::Array::New(isolate, count));
                                for (Int32 j = 0; j < count; j++) {
                                    IInterface* carObject = buffer[j];

                                    if (carObject->Probe(EIID_CarCallbackObject) == NULL) {
                                        CarValue value;
                                        value.mType = CarDataType_Interface;
                                        value.mObjectValue = carObject;
                                        v8::Local<v8::Object> v8Object = V8CarObject::New(isolate, value);

                                        v8Array->Set(j, v8Object);
                                    }
                                    else {
                                        Logger::E("CarCallbackInterfaceProxy", "CarCallbackObject unsupported!");
                                    }
                                }

                                v8Args[i].Reset(isolate, v8Array);
                                break;
                            }
                            default:
                                break;
                        }

                        puArgs++;
                        break;
                    }
                }

                break;
            }
            case ParamIOAttribute_CalleeAllocOut: {
                break;
            }
            case ParamIOAttribute_CallerAllocOut: {
                break;
            }
            default: {
                Logger::E("CarCallbackInterfaceProxy", "ParamIOAttribute of No.%d parameter is wrong!", i);
                break;
            }
        }
    }

    return NOERROR;
}

// Callback in the JS thread.
static void FireCallback(uv_work_t* req)
{
    CarCallbackInterfaceProxy::Callback* callback = (CarCallbackInterfaceProxy::Callback*)req->data;
    callback->Call();
    delete callback;
    delete req;
}

ECode CarCallbackInterfaceProxy::ProxyEntry(
    /* [in] */ UInt32 *puArgs)
{
    puArgs++; // skip ret address

    CarCallbackInterfaceProxy* pThis = (CarCallbackInterfaceProxy *)*puArgs;
    puArgs++; // skip this

    UInt32 uMethodIndex = puArgs[-3];

    if (pThis->mMethodInfos == NULL) {
        Int32 count;
        pThis->mInterfaceInfo->GetMethodCount(&count);
        pThis->mMethodInfos = ArrayOf<IMethodInfo*>::Alloc(count);
        pThis->mInterfaceInfo->GetAllMethodInfos(pThis->mMethodInfos);
    }

    IMethodInfo* methodInfo = (*pThis->mMethodInfos)[uMethodIndex];

    Elastos::String nameBuf;
    methodInfo->GetName(&nameBuf);

    Int32 paramCount;
    methodInfo->GetParamCount(&paramCount);

    v8::Persistent<v8::Value>* v8Args = new v8::Persistent<v8::Value>[paramCount];

    ECode ec = pThis->ReadParam(methodInfo, puArgs, paramCount, v8Args);
    if (FAILED(ec)) {
        Logger::E("CarCallbackInterfaceProxy", "ProxyEntry reads parameters failed!");
        delete[] v8Args;
        return ec;
    }

    Callback* callback = new Callback(pThis->mOwner, methodInfo, paramCount, v8Args, pThis->mOwner->mIsolate);

    if (pThis->mOwner->mMainThread == pthread_self()) {
        uv_work_t* req = new uv_work_t();
        req->data = (void*)callback;
        req->loop = NULL;
        req->work_cb = FireCallback;
        req->after_work_cb = NULL;

        FireCallback(req);
    }
    else {
        uv_work_t* req = new uv_work_t();
        req->data = (void*)callback;
        req->loop = uv_default_loop();
        req->work_cb = FireCallback;
        req->after_work_cb = NULL;

        uv_queue_work(uv_default_loop(),
                req, FireCallback, NULL);
    }

    return NOERROR;
}


//============================================================
//  CarCallbackInterfaceProxy::Callback
//============================================================
CarCallbackInterfaceProxy::Callback::~Callback()
{
    for (Int32 i = 0; i < mParamCount; ++i) {
        mV8Params[i].Reset();
    }
    delete[] mV8Params;
}

void CarCallbackInterfaceProxy::Callback::Call()
{
    Int32 paramCount;
    mMethodInfo->GetParamCount(&paramCount);

    if (paramCount != mParamCount) {
        Logger::E("CarCallbackInterfaceProxy::Callback", "ParamCount is wrong!");
        return;
    }

    v8::Isolate* isolate = v8::Isolate::GetCurrent();

    if (isolate != mIsolate) {
        Logger::E("CarCallbackInterfaceProxy::Callback", "Isolate is wrong!");
        return;
    }

    v8::HandleScope scope(isolate);

    v8::Local<v8::Object> object = v8::Local<v8::Object>::New(isolate, mObject->mV8Object);

    Elastos::String methodName;
    mMethodInfo->GetName(&methodName);
    v8::Local<v8::Value> method = V8Utility::GetProperty(object, methodName);

    if (method.IsEmpty() || !method->IsFunction()) {
        Logger::E("CarCallbackInterfaceProxy::Callback", "Method %s doesn't exist!", methodName.string());
        return;
    }

    v8::Local<v8::Function> jsFunction = v8::Local<v8::Function>::Cast(method);

    v8::Local<v8::Value>* argv = NULL;
    if (mParamCount > 0) {
        argv = new v8::Local<v8::Value>[mParamCount];
        for (int i = 0; i < mParamCount; i++) {
            argv[i] = v8::Local<v8::Value>::New(isolate, mV8Params[i]);
        }
    }

    jsFunction->Call(isolate->GetCurrentContext()->Global(), mParamCount, argv);

    if (argv != NULL) delete[] argv;
}


//============================================================
//  CarCallbackObject
//============================================================

CarCallbackObject::~CarCallbackObject()
{
    mV8Object.Reset();
}

PInterface CarCallbackObject::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_CarCallbackObject) {
        return (IObject*)this;
    }
    return Object::Probe(riid);
}

ECode CarCallbackObject::ToString(
    /* [out] */ Elastos::String* info)
{
    VALIDATE_NOT_NULL(info);
    *info = "Class CarCallbackObject";
    return NOERROR;
}

CarCallbackObject* CarCallbackObject::S_CreateObject(
    /* [in] */ IInterfaceInfo* itfInfo,
    /* [in] */ v8::Local<v8::Object> object)
{
    if (InitCarCallbackProxyEntry() != 0) {
        Logger::E("CarCallbackObject", "S_CreateObject failed!");
        return NULL;
    }

    CarCallbackInterfaceProxy* itfProxy = new CarCallbackInterfaceProxy();
    itfProxy->mInterfaceInfo = itfInfo;
    itfProxy->mVptr = g_marshalVtbl;
    itfProxy->mProxyEntry = (PVoid)&__ProxyEntry;
    itfInfo->GetId(&itfProxy->mIID);

    CarCallbackObject* cbObject = new CarCallbackObject();
    cbObject->mInterface = itfProxy;
    cbObject->mMainThread = pthread_self();
    cbObject->mIsolate = v8::Isolate::GetCurrent();
    cbObject->mV8Object.Reset(cbObject->mIsolate, object);

    itfProxy->mOwner = cbObject;

    return cbObject;
}

} // namespace Node
} // namespace Elastos
