
#include "CarCallbackProxy.h"
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
}


//============================================================
//  CarCallbackObject
//============================================================
CarCallbackObject::~CarCallbackObject()
{
    mV8Object.Reset();
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
    // cbObject->mObject = _NPN_RetainObject(object);
    cbObject->mIsolate = v8::Isolate::GetCurrent();
    cbObject->mV8Object.Reset(cbObject->mIsolate, object);

    itfProxy->mOwner = cbObject;

    return cbObject;
}

} // namespace Node
} // namespace Elastos
