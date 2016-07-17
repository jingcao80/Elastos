
#include "ElNode.h"
#include "config.h"

#if ENABLE(CAR_BRIDGE)

#include "NPV8Object.h"
#include "V8NPUtils.h"
#include "V8Proxy.h"
#include "node_JavaSharedClient.h"

#include "CarNPObjectV8.h"
#include "CarInstanceV8.h"

#include "CarCallbackProxy.h"
#include <sys/mman.h>

#include <utils/Log.h>

// {8CC78507-90A3-4771-9122-F279D7D20DCA}
static const InterfaceID EIID_CarCallbackObject =
        { 0x8cc78507, 0x90a3, 0x4771, { 0x91, 0x22, 0xf2, 0x79, 0xd7, 0xd2, 0xd, 0xca } };

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

namespace JSC {
namespace Bindings {

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
        LOG_ERROR("InitCarCallbackProxyEntry: out of memory");
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

CarCallbackInterfaceProxy::~CarCallbackInterfaceProxy()
{
    if (mMethodInfos != NULL) {
        //for (Int32 i = 0; i < mMethodInfos->GetUsed(); i++) {
        for (Int32 i = 0; i < mMethodInfos->GetLength(); i++) {
            //(*mMethodInfos)[i]->Release();
            //(*mMethodInfos)[i] = NULL;
        }
        ArrayOf<IMethodInfo*>::Free(mMethodInfos);
    }
}

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
    /* [in] */ UInt32 *puArgs,
    /* [in] */ Int32 paramCount,
    /* [in] */ NPVariant* npArgs,
    /* [in] */ Int32* outParamPtrs,
    /* [in] */ Int32* outParamTypes,
    /* [in] */ Int32* outParamElementTypes,
    /* [out] */ CarValue** ppCarArgs,
    /* [in] */ bool* hasOutParams)
{
    ArrayOf<IParamInfo*>* paramInfos = ArrayOf<IParamInfo*>::Alloc(paramCount);
    methodInfo->GetAllParamInfos(paramInfos);

    *ppCarArgs = new CarValue[paramCount];
    CarValue* carArgs = *ppCarArgs;

    for (Int32 i = 0; i < paramCount; i++) {
        IParamInfo* paramInfo = (*paramInfos)[i];
        ParamIOAttribute paramIOAttr;
        paramInfo->GetIOAttribute(&paramIOAttr);
        AutoPtr<IDataTypeInfo> paramTypeInfo;
        paramInfo->GetTypeInfo((IDataTypeInfo**)&paramTypeInfo);
        CarDataType paramDataType;
        paramTypeInfo->GetDataType(&paramDataType);

        AutoPtr<IDataTypeInfo> elementDataTypeInfo;
        CarDataType elementDataType;

        AutoPtr<IDataTypeInfo> localPtrElementDataTypeInfo;
        CarDataType localPtrElementDataType;

        if(paramDataType == CarDataType_ArrayOf) {
            (*(ICarArrayInfo **)&paramTypeInfo)->GetElementTypeInfo((IDataTypeInfo**)&elementDataTypeInfo);
            elementDataTypeInfo->GetDataType(&elementDataType);
        } else if (paramDataType == CarDataType_CppVector) {
            (*(ICppVectorInfo **)&paramTypeInfo)->GetElementTypeInfo((IDataTypeInfo**)&elementDataTypeInfo);
            elementDataTypeInfo->GetDataType(&elementDataType);
        } else if (paramDataType == CarDataType_LocalPtr) {
            (*(ILocalPtrInfo **)&paramTypeInfo)->GetTargetTypeInfo((IDataTypeInfo**)&elementDataTypeInfo);
            elementDataTypeInfo->GetDataType(&elementDataType);
            if (elementDataType == CarDataType_ArrayOf) {
                (*(ICarArrayInfo **)&elementDataTypeInfo)->GetElementTypeInfo((IDataTypeInfo**)&localPtrElementDataTypeInfo);
                localPtrElementDataTypeInfo->GetDataType(&localPtrElementDataType);

            }
        }

        outParamPtrs[i] = 0;
        outParamTypes[i] = paramDataType;
        outParamElementTypes[i] = elementDataType;

        switch (paramIOAttr) {
        case ParamIOAttribute_In: {
            switch (paramDataType) {
                case CarDataType_Byte:
                case CarDataType_Char32:
                case CarDataType_Int16:
                case CarDataType_Int32:
                case CarDataType_ECode:
                case CarDataType_Enum:
                {
                    npArgs[i].type = NPVariantType_Int32;
                    npArgs[i].value.intValue = *puArgs;
                    puArgs++;
                    break;
                }
                case CarDataType_Int64:
                case CarDataType_Double:
                {
                    puArgs = (UInt32*)ROUND8((Int32)puArgs);
                    npArgs[i].type = NPVariantType_Double;
                    npArgs[i].value.doubleValue = *(UInt64*)puArgs;
                    puArgs += 2;
                    break;
                }
                case CarDataType_Float:
                {
                    npArgs[i].type = NPVariantType_Double;
                    npArgs[i].value.doubleValue = *(Float*)puArgs;
                    puArgs++;
                    break;
                }
                case CarDataType_Boolean:
                {
                    npArgs[i].type = NPVariantType_Bool;
                    npArgs[i].value.boolValue = *puArgs;
                    puArgs++;
                    break;
                }
                case CarDataType_String:
                {
                    Elastos::String& str = **(Elastos::String**)puArgs;
                    const char* utf8String = strdup(str.string());
                    // The copied string is freed in NPN_ReleaseVariantValue (see npruntime.cpp)
                    STRINGZ_TO_NPVARIANT(utf8String, npArgs[i]);
                    puArgs++;
                    break;
                }
                case CarDataType_EMuid:
                {
                    EMuid& iid = *(EMuid*)puArgs;

                /*----TODO------------------------------------------------------------
                    v8::Persistent<v8::Array> v8Array(v8::Array::New(4));
                    v8Array->Set(0, v8::Number::New(iid.mData1));
                    v8Array->Set(1, v8::Number::New(iid.mData2));
                    v8Array->Set(2, v8::Number::New(iid.mData3));

                    v8::Handle<v8::Array> v8Array1(v8::Array::New(4));
                    for(Int32 j = 0; j < 8; j++) {
                        v8Array1->Set(j, v8::Number::New(iid.mData4[j]));
                    }
                    v8Array->Set(3, v8Array1);

                    WebCore::V8NPObject* v8NPObject = (WebCore::V8NPObject*)_NPN_CreateObject(NULL, WebCore::npScriptObjectClass);
                    v8NPObject->v8Object = v8Array;
                    npArgs[i].type = NPVariantType_Object;
                    npArgs[i].value.objectValue = _NPN_RetainObject((NPObject*)v8NPObject);
                ----------------------------------------------------------------*/

                    puArgs += sizeof(EMuid) / 4;
                    break;
                }
                case CarDataType_EGuid:
                {
                    EGuid& cid = *(EGuid*)puArgs;
                    EMuid& iid = cid.mClsid;

                    /*----TODO------------------------------------------------------------
                    v8::Persistent<v8::Array> v8Array(v8::Array::New(4));
                    v8Array->Set(0, v8::Number::New(iid.mData1));
                    v8Array->Set(1, v8::Number::New(iid.mData2));
                    v8Array->Set(2, v8::Number::New(iid.mData3));
                    v8::Handle<v8::Array> v8Array1(v8::Array::New(4));
                    for(Int32 j = 0; j < 8; j++){
                        v8Array1->Set(j, v8::Number::New(iid.mData4[j]));
                    }
                    v8Array->Set(3, v8Array1);

                    v8::Persistent<v8::Object> v8Object(v8::Object::New());
                    v8Object->Set(v8::String::New("mClsid"), v8Array);
                    v8Object->Set(v8::String::New("mUunm"), v8::String::New(cid.mUunm));
                    v8Object->Set(v8::String::New("mCarcode"), v8::Number::New(cid.mCarcode));

                    WebCore::V8NPObject* v8NPObject = (WebCore::V8NPObject*)_NPN_CreateObject(NULL, WebCore::npScriptObjectClass);
                    v8NPObject->v8Object = v8Object;
                    npArgs[i].type = NPVariantType_Object;
                    npArgs[i].value.objectValue = _NPN_RetainObject((NPObject*)v8NPObject);
                    ----------------------------------------------------------------*/

                    puArgs += sizeof(EGuid) / 4;
                    break;
                }
                case CarDataType_Interface:
                {
                    IInterface* obj = (IInterface*)*puArgs;

                    bool bNotCarCallbackObject = false;
                    if (obj) {
                        IInterface* carCallbackObject;
                        carCallbackObject = obj->Probe(EIID_CarCallbackObject);
                        bNotCarCallbackObject = (carCallbackObject == NULL);
                    }

                    if (bNotCarCallbackObject) {
                        npArgs[i].type = NPVariantType_Object;
                        npArgs[i].value.objectValue = CarInstanceToNPObject(new CarInstanceV8(new CobjectWrapper(obj, paramTypeInfo), true));
                    }
                    else {
                        Int32 iii = 1/0;
                        assert(0);
                    }

                    puArgs++;
                    break;
                }
                case CarDataType_ArrayOf:
                {
                    CarQuintet* carArray = (CarQuintet*)*puArgs;

                    /*----TODO------------------------------------------------------------
                    switch(elementDataType) {
                        case CarDataType_Byte:
                        {
                            Int32 size = carArray->m_size / sizeof(Byte);
                            Byte* buf = (Byte*)carArray->m_pBuf;

                            v8::Persistent<v8::Array> v8Array(v8::Array::New(size));

                            for (Int32 j = 0; j < size; j++) {
                                v8Array->Set(j, v8::Int32::New(buf[j]));
                            }

                            WebCore::V8NPObject* v8NPObject = (WebCore::V8NPObject*)_NPN_CreateObject(NULL, WebCore::npScriptObjectClass);
                            v8NPObject->v8Object = v8Array;
                            // v8NPObject->rootObject = rootObject;

                            npArgs[i].type = NPVariantType_Object;
                            npArgs[i].value.objectValue = _NPN_RetainObject((NPObject*)v8NPObject);
                            break;
                        }
                        case CarDataType_Char32:
                        {
                            Int32 size = carArray->m_size / sizeof(Char32);
                            Char32* buf = (Char32*)carArray->m_pBuf;

                            v8::Persistent<v8::Array> v8Array(v8::Array::New(size));

                            for (Int32 j = 0; j < size; j++) {
                                v8Array->Set(j, v8::Int32::New(buf[j]));
                            }

                            WebCore::V8NPObject* v8NPObject = (WebCore::V8NPObject*)_NPN_CreateObject(NULL, WebCore::npScriptObjectClass);
                            v8NPObject->v8Object = v8Array;
                            // v8NPObject->rootObject = rootObject;

                            npArgs[i].type = NPVariantType_Object;
                            npArgs[i].value.objectValue = _NPN_RetainObject((NPObject*)v8NPObject);
                            break;
                        }
                        case CarDataType_Int16:
                        {
                            Int32 size = carArray->m_size / sizeof(Int16);
                            Int16* buf = (Int16*)carArray->m_pBuf;

                            v8::Persistent<v8::Array> v8Array(v8::Array::New(size));

                            for (Int32 j = 0; j < size; j++) {
                                v8Array->Set(j, v8::Int32::New(buf[j]));
                            }

                            WebCore::V8NPObject* v8NPObject = (WebCore::V8NPObject*)_NPN_CreateObject(NULL, WebCore::npScriptObjectClass);
                            v8NPObject->v8Object = v8Array;
                            // v8NPObject->rootObject = rootObject;

                            npArgs[i].type = NPVariantType_Object;
                            npArgs[i].value.objectValue = _NPN_RetainObject((NPObject*)v8NPObject);
                            break;
                        }
                        case CarDataType_Int32:
                        case CarDataType_ECode:
                        case CarDataType_Enum:
                        {
                            Int32 size = carArray->m_size / sizeof(Int32);
                            Int32* buf = (Int32*)carArray->m_pBuf;

                            v8::Persistent<v8::Array> v8Array(v8::Array::New(size));

                            for (Int32 j = 0; j < size; j++) {
                                v8Array->Set(j, v8::Int32::New(buf[j]));
                            }

                            WebCore::V8NPObject* v8NPObject = (WebCore::V8NPObject*)_NPN_CreateObject(NULL, WebCore::npScriptObjectClass);
                            v8NPObject->v8Object = v8Array;
                            // v8NPObject->rootObject = rootObject;

                            npArgs[i].type = NPVariantType_Object;
                            npArgs[i].value.objectValue = _NPN_RetainObject((NPObject*)v8NPObject);
                            break;
                        }
                        case CarDataType_Int64:
                        case CarDataType_Double:
                        {
                            Int32 size = carArray->m_size / sizeof(Double);
                            Double* buf = (Double*)carArray->m_pBuf;

                            v8::Persistent<v8::Array> v8Array(v8::Array::New(size));

                            for (Int32 j = 0; j < size; j++) {
                                v8Array->Set(j, v8::Number::New(buf[j]));
                            }

                            WebCore::V8NPObject* v8NPObject = (WebCore::V8NPObject*)_NPN_CreateObject(NULL, WebCore::npScriptObjectClass);
                            v8NPObject->v8Object = v8Array;
                            // v8NPObject->rootObject = rootObject;

                            npArgs[i].type = NPVariantType_Object;
                            npArgs[i].value.objectValue = _NPN_RetainObject((NPObject*)v8NPObject);
                            break;
                        }
                        case CarDataType_Float:
                        {
                            Int32 size = carArray->m_size / sizeof(Float);
                            Float* buf = (Float*)carArray->m_pBuf;

                            v8::Persistent<v8::Array> v8Array(v8::Array::New(size));

                            for (Int32 j = 0; j < size; j++) {
                                v8Array->Set(j, v8::Number::New(buf[j]));
                            }

                            WebCore::V8NPObject* v8NPObject = (WebCore::V8NPObject*)_NPN_CreateObject(NULL, WebCore::npScriptObjectClass);
                            v8NPObject->v8Object = v8Array;
                            // v8NPObject->rootObject = rootObject;

                            npArgs[i].type = NPVariantType_Object;
                            npArgs[i].value.objectValue = _NPN_RetainObject((NPObject*)v8NPObject);
                            break;
                        }
                        case CarDataType_Boolean:
                        {
                            Int32 size = carArray->m_size / sizeof(Boolean);
                            Boolean* buf = (Boolean*)carArray->m_pBuf;

                            v8::Persistent<v8::Array> v8Array(v8::Array::New(size));

                            for (Int32 j = 0; j < size; j++) {
                                v8Array->Set(j, v8::Boolean::New(buf[j]));
                            }

                            WebCore::V8NPObject* v8NPObject = (WebCore::V8NPObject*)_NPN_CreateObject(NULL, WebCore::npScriptObjectClass);
                            v8NPObject->v8Object = v8Array;
                            // v8NPObject->rootObject = rootObject;

                            npArgs[i].type = NPVariantType_Object;
                            npArgs[i].value.objectValue = _NPN_RetainObject((NPObject*)v8NPObject);
                            break;
                        }
                        case CarDataType_String:
                        {
                            Int32 size = carArray->m_size / sizeof(Elastos::String);
                            Elastos::String* buf = (Elastos::String*)carArray->m_pBuf;

                            v8::Persistent<v8::Array> v8Array(v8::Array::New(size));

                            for (Int32 j = 0; j < size; j++) {
                                const char* utf8String = strdup(buf[j].string());
                                v8Array->Set(j, v8::String::New(utf8String));
                            }

                            WebCore::V8NPObject* v8NPObject = (WebCore::V8NPObject*)_NPN_CreateObject(NULL, WebCore::npScriptObjectClass);
                            v8NPObject->v8Object = v8Array;
                            // v8NPObject->rootObject = rootObject;

                            npArgs[i].type = NPVariantType_Object;
                            npArgs[i].value.objectValue = _NPN_RetainObject((NPObject*)v8NPObject);
                            break;
                        }
                        case CarDataType_EMuid:
                        {
                            break;
                        }
                        case CarDataType_EGuid:
                        {
                            break;
                        }
                        case CarDataType_Interface:
                        {
                            break;
                        }
                        default:
                            break;
                    }
                    ----------------------------------------------------------------*/
                    puArgs++;
                    break;
                }
                case CarDataType_LocalPtr:
                {
                    switch(elementDataType) {
                        // case CarDataType_Int16:
                        // case CarDataType_Int32:
                        // case CarDataType_Int64:
                        // case CarDataType_Byte:
                        // case CarDataType_Float:
                        // case CarDataType_Double:
                        // case CarDataType_Char8:
                        // case CarDataType_Char16:
                        // case CarDataType_Char32:
                        // case CarDataType_CString:
                        // case CarDataType_String:
                        // case CarDataType_Boolean:
                        // case CarDataType_EMuid:
                        // case CarDataType_EGuid:
                        // case CarDataType_ECode:
                        // case CarDataType_LocalPtr:
                        // case CarDataType_LocalType:
                        // case CarDataType_Enum:
                        // case CarDataType_StringBuf:
                        case CarDataType_ArrayOf:
                        {
                            CarQuintet* carArray = (CarQuintet*)*puArgs;
                            switch(localPtrElementDataType) {
                                // case CarDataType_Int16:
                                case CarDataType_Int32:
                                {
                                    v8::Isolate* isolate = mOwner->mIsolate;
                                    isolate->Enter();

                                    ArrayOf<Int32>* pArray = reinterpret_cast< ArrayOf<Int32>* >(carArray);

                                    Int32 length;
                                    if (pArray) {
                                        length = pArray->GetLength();
                                    }
                                    else {
                                        length = 0;
                                    }

                                    v8::Local<v8::Array> pV8Array(v8::Array::New(isolate,length));
                                    for (Int32 i = 0; i < length; i++) {
                                        pV8Array->Set(i, v8::Int32::New(isolate,(*pArray)[i]));
                                    }

                                    WebCore::V8NPObject* pV8NPObject = (WebCore::V8NPObject*)_NPN_CreateObject(NULL, WebCore::npScriptObjectClass);
                                    pV8NPObject->v8Object.Reset(isolate,pV8Array);
                                    pV8NPObject->rootObject = getRootObject();

                                    npArgs[i].type = NPVariantType_Object;
                                    npArgs[i].value.objectValue = _NPN_RetainObject((NPObject*)pV8NPObject);

                                    isolate->Exit();
                                    break;
                                }
                                // case CarDataType_Int64:
                                // case CarDataType_Byte:
                                // case CarDataType_Float:
                                // case CarDataType_Double:
                                // case CarDataType_Char8:
                                // case CarDataType_Char16:
                                // case CarDataType_Char32:
                                // case CarDataType_CString:
                                case CarDataType_String:
                                {
                                    v8::Isolate* isolate = mOwner->mIsolate;
                                    isolate->Enter();

                                    ArrayOf<Elastos::String>* pArray = reinterpret_cast< ArrayOf<Elastos::String>* >(carArray);

                                    Int32 length;
                                    if (pArray) {
                                        length = pArray->GetLength();
                                    }
                                    else {
                                        length = 0;
                                    }

                                    v8::Local<v8::Array> pV8Array(v8::Array::New(isolate,length));

                                    for (Int32 i = 0; i < length; i++) {
                                        const char* utf8String = strdup(((*pArray)[i]).string());
                                        pV8Array->Set(i, v8::String::NewFromUtf8(isolate,utf8String));
                                    }

                                    WebCore::V8NPObject* pV8NPObject = (WebCore::V8NPObject*)_NPN_CreateObject(NULL, WebCore::npScriptObjectClass);
                                    pV8NPObject->v8Object.Reset(isolate,pV8Array);
                                    pV8NPObject->rootObject = getRootObject();

                                    npArgs[i].type = NPVariantType_Object;
                                    npArgs[i].value.objectValue = _NPN_RetainObject((NPObject*)pV8NPObject);

                                    isolate->Exit();
                                    break;
                                }
                                // case CarDataType_Boolean:
                                // case CarDataType_EMuid:
                                // case CarDataType_EGuid:
                                // case CarDataType_ECode:
                                // case CarDataType_LocalPtr:
                                // case CarDataType_LocalType:
                                // case CarDataType_Enum:
                                // case CarDataType_StringBuf:
                                // case CarDataType_ArrayOf:
                                // case CarDataType_BufferOf:
                                // case CarDataType_MemoryOf:
                                // case CarDataType_CppVector:
                                // case CarDataType_Struct:
                                // case CarDataType_Interface:
                                default:
                                {
                                    ALOGD("CarCallbackInterfaceProxy::ReadParam======LocalPtr======ArrayOf======other");
                                    //break;
                                    return E_INVALID_ARGUMENT;
                                }
                            }   //switch

                            break;
                        }
                        // case CarDataType_BufferOf:
                        // case CarDataType_MemoryOf:
                        // case CarDataType_CppVector:
                        // case CarDataType_Struct:
                        case CarDataType_Interface:
                        {
                            //transfer pp pointer to js, js should set the pp value with a js object by calling native method as response.
                            npArgs[i].type = NPVariantType_Int32;
                            npArgs[i].value.intValue = *puArgs;
                            break;
                        }
                        default:
                        {
                            ALOGD("CarCallbackInterfaceProxy::ReadParam======LocalPtr======LocalPtr======other:%d", elementDataType);
                            //break;
                            return E_INVALID_ARGUMENT;
                        }
                    }   //switch

                    puArgs++;
                    break;
                }
                default:
                {
                    ALOGD("CarCallbackInterfaceProxy::ReadParam======in======other======");
                    LOG_ERROR("CarCallbackInterfaceProxy::ReadParam: Invalid [in] type(%08x), param index: %d",
                            paramDataType, i);
                    //break;
                    return E_INVALID_ARGUMENT;
                }
            }

            break;
        }
        case ParamIOAttribute_CalleeAllocOut: {
            *hasOutParams = true;

            outParamPtrs[i] = *puArgs;

            //use v8 from node.js thread in EPK UI main thread

            v8::Isolate* isolate = mOwner->mIsolate;
            isolate->Enter();

            v8::Handle<v8::Context> context = v8::Isolate::GetCurrent()->GetCurrentContext();

            v8::Isolate::Scope isolateScope(isolate);
            v8::Context::Scope contextScope(context);

            v8::Local<v8::Object> v8Object(v8::Object::New(isolate));

            v8::Local<v8::String> v8Name(v8::String::NewFromUtf8(isolate,"data"));
            v8Object->Set(v8Name, v8::String::NewFromUtf8(isolate,"Callback output parameters"));

            WebCore::V8NPObject* v8NPObject = (WebCore::V8NPObject*)_NPN_CreateObject(NULL, WebCore::npScriptObjectClass);

            v8NPObject->v8Object.Reset(isolate,v8Object);
            v8NPObject->rootObject = getRootObject();

            npArgs[i].type = NPVariantType_Object;
            npArgs[i].value.objectValue = _NPN_RetainObject((NPObject*)v8NPObject);

            isolate->Exit();

            puArgs++;
            break;
        }
        case ParamIOAttribute_CallerAllocOut: {
            *hasOutParams = true;

            switch (paramDataType) {
                case CarDataType_Boolean:
                case CarDataType_Interface:
                {
                    outParamPtrs[i] = *puArgs;

                    //use v8 from node.js thread in EPK UI main thread

                    v8::Isolate* isolate = mOwner->mIsolate;
                    isolate->Enter();

                    v8::Handle<v8::Context> context = v8::Isolate::GetCurrent()->GetCurrentContext();

                    v8::Isolate::Scope isolateScope(isolate);
                    v8::Context::Scope contextScope(context);

                    v8::Local<v8::Object> v8Object(v8::Object::New(isolate));

                    v8::Local<v8::String> v8Name(v8::String::NewFromUtf8(isolate,"data"));
                    v8Object->Set(v8Name, v8::String::NewFromUtf8(isolate,"Callback output parameters"));

                    WebCore::V8NPObject* v8NPObject = (WebCore::V8NPObject*)_NPN_CreateObject(NULL, WebCore::npScriptObjectClass);

                    v8NPObject->v8Object.Reset(isolate,v8Object);
                    v8NPObject->rootObject = getRootObject();

                    npArgs[i].type = NPVariantType_Object;
                    npArgs[i].value.objectValue = _NPN_RetainObject((NPObject*)v8NPObject);

                    isolate->Exit();

                    puArgs++;
                    break;
                }
                case CarDataType_LocalPtr:
                {
                    ALOGD("CarCallbackInterfaceProxy::ProxyEntry======ReadParam======CarDataType_LocalPtr====");
                    puArgs++;
                    break;
                }
                default:
                {
                    ALOGD("CarCallbackInterfaceProxy::ReadParam======CallerAllocOut======other======");
                    puArgs++;
                    break;
                }
            }

            break;
        }
        default: {
            puArgs++;
            break;
        }

        //else {  // [out]
        //    LOG_ERROR("CarCallbackInterfaceProxy::ReadParam: Don't support [out] type in callback");

        //    ALOGD("CarCallbackInterfaceProxy::ReadParam======out param======");

            //puArgs++;
        } //switch (paramIOAttr)

        (*paramInfos)[i]->Release();
        (*paramInfos)[i] = NULL;

    }   //for

    ArrayOf<IParamInfo*>::Free(paramInfos);

    return NOERROR;
}

ECode CarCallbackInterfaceProxy::ReadParam_bak(
    /* [in] */ IMethodInfo* methodInfo,
    /* [in] */ UInt32 *puArgs,
    /* [out] */ CarValue** ppCarArgs)
{
    Int32 paramCount;
    methodInfo->GetParamCount(&paramCount);

    ArrayOf<IParamInfo*>* paramInfos = ArrayOf<IParamInfo*>::Alloc(paramCount);
    methodInfo->GetAllParamInfos(paramInfos);

    *ppCarArgs = new CarValue[paramCount];
    CarValue* carArgs = *ppCarArgs;

    for (Int32 i = 0; i < paramCount; i++) {
        IParamInfo* paramInfo = (*paramInfos)[i];
        ParamIOAttribute paramIOAttr;
        paramInfo->GetIOAttribute(&paramIOAttr);
        AutoPtr<IDataTypeInfo> paramTypeInfo;
        paramInfo->GetTypeInfo((IDataTypeInfo**)&paramTypeInfo);
        CarDataType paramDataType;
        paramTypeInfo->GetDataType(&paramDataType);

        AutoPtr<IDataTypeInfo> elementDataTypeInfo;
        CarDataType elementDataType;
        if(paramDataType == CarDataType_ArrayOf) {
            (*(ICarArrayInfo **)&paramTypeInfo)->GetElementTypeInfo((IDataTypeInfo**)&elementDataTypeInfo);
            elementDataTypeInfo->GetDataType(&elementDataType);
        } else if (paramDataType == CarDataType_CppVector) {
            (*(ICppVectorInfo **)&paramTypeInfo)->GetElementTypeInfo((IDataTypeInfo**)&elementDataTypeInfo);
            elementDataTypeInfo->GetDataType(&elementDataType);
        } else if (paramDataType == CarDataType_LocalPtr) {
            (*(ILocalPtrInfo **)&paramTypeInfo)->GetTargetTypeInfo((IDataTypeInfo**)&elementDataTypeInfo);
            elementDataTypeInfo->GetDataType(&elementDataType);
        }

        carArgs[i].mType = paramDataType;
        carArgs[i].mElementType = elementDataType;
        carArgs[i].mIOAttribute = paramIOAttr;

        if (paramIOAttr == ParamIOAttribute_In) { // [in]
            switch (paramDataType) {
                case CarDataType_Int16:
                {
                    carArgs[i].mInt16Value = *puArgs;
                    puArgs++;
                    break;
                }
                case CarDataType_Int32:
                {
                    carArgs[i].mInt32Value = *puArgs;
                    puArgs++;
                    break;
                }
                case CarDataType_Int64:
                {
                    puArgs = (UInt32*)ROUND8((Int32)puArgs);
                    carArgs[i].mInt64Value = *puArgs;
                    puArgs += 2;
                    break;
                }
                case CarDataType_Byte:
                {
                    carArgs[i].mByteValue = *puArgs;
                    puArgs++;
                    break;
                }
                case CarDataType_Float:
                {
                    carArgs[i].mFloatValue = *(Float*)puArgs;
                    puArgs++;
                    break;
                }
                case CarDataType_Double:
                {
                    puArgs = (UInt32*)ROUND8((Int32)puArgs);
                    carArgs[i].mDoubleValue = *puArgs;
                    puArgs += 2;
                    break;
                }
                //--------to be delete begin--------
                /*
                case CarDataType_Char8:
                {
                    carArgs[i].mChar8Value = *puArgs;
                    puArgs++;
                    break;
                }
                case CarDataType_Char16:
                {
                    carArgs[i].mChar16Value = *puArgs;
                    puArgs++;
                    break;
                }
                */
                //--------to be delete end--------
                case CarDataType_Char32:
                {
                    carArgs[i].mCharValue = *puArgs;
                    puArgs++;
                    break;
                }
                //case CarDataType_CString:
                case CarDataType_String:
                {
                    Elastos::String& str = **(Elastos::String**)puArgs;
                    const char* utf8String = strdup(str.string());
                    carArgs[i].mStringValue = utf8String;
                    puArgs++;
                    break;
                }
                case CarDataType_Boolean:
                {
                    carArgs[i].mBooleanValue = *puArgs;
                    puArgs++;
                    break;
                }
                case CarDataType_EMuid:
                {
                    // pParcel->WriteEMuid(*(EMuid *)puArgs);
                    puArgs += sizeof(EMuid) / 4;
                    break;
                }
                case CarDataType_EGuid:
                {
                    // pParcel->WriteEGuid(*(EGuid *)puArgs);
                    puArgs += sizeof(EGuid) / 4;
                    break;
                }
                case CarDataType_ECode:
                {
                    carArgs[i].mECodeValue = *puArgs;
                    puArgs++;
                    break;
                }
                //case CarDataType_LocalPtr:
                //case CarDataType_LocalType:
                case CarDataType_Enum:
                {
                    carArgs[i].mInt32Value = *puArgs;
                    puArgs++;
                    break;
                }
                //case CarDataType_StringBuf:
                case CarDataType_ArrayOf:
                {
                    // pParcel->WriteArrayOf((Handle32)*puArgs);
                    puArgs++;
                    break;
                }
                //case CarDataType_BufferOf:
                //case CarDataType_MemoryBuf:
                //case CarDataType_CppVector:
                //case CarDataType_Struct:
                case CarDataType_Interface:
                {
                    // ec = pParcel->WriteInterfacePtr((IInterface *)*puArgs);
                    // if (FAILED(ec)) {
                    //     MARSHAL_DBGOUT(MSHDBG_ERROR, ALOGE(
                    //             "MshProc: marshal interface, ec = %x, param index: %d\n", ec, n));
                    //     return ec;
                    // }
                    //carArgs[i].pInterface = (IInterface*)(*puArgs);
                    carArgs[i].mObjectValue = (IInterface*)(*puArgs);
                    puArgs++;
                    break;
                }
                default:
                {
                    LOG_ERROR("CarCallbackInterfaceProxy::ReadParam: Invalid [in] type(%08x), param index: %d", paramDataType, i);
                    return E_INVALID_ARGUMENT;
                }
            }
        }
        else {  // [out]
            LOG_ERROR("CarCallbackInterfaceProxy::ReadParam: Don't support [out] type in callback");
            puArgs++;
        }

        (*paramInfos)[i]->Release();
        (*paramInfos)[i] = NULL;
    }

    ArrayOf<IParamInfo*>::Free(paramInfos);

    return NOERROR;
}

// Callback in the main(webview/node.js) thread.
static void FireCallback(void* payload)
{
    CarCallbackInterfaceProxy::Callback* callback = (CarCallbackInterfaceProxy::Callback*)payload;
    callback->Call();
    delete callback;
}

ECode CarCallbackInterfaceProxy::ProxyEntry(
    /* [in] */ UInt32 *puArgs)
{
    //Run in Elastos UI thread

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

    NPVariant* npArgs = new NPVariant[paramCount];
    Int32* outParamPtrs = new Int32[paramCount];
    Int32* outParamTypes = new Int32[paramCount];
    Int32* outParamElementTypes = new Int32[paramCount];
    bool hasOutParams = false;

    CarValue* pCarArgs;
    ECode ec = pThis->ReadParam(methodInfo, puArgs, paramCount, npArgs, outParamPtrs, outParamTypes, outParamElementTypes, &pCarArgs, &hasOutParams);

    if (FAILED(ec)) {
        LOG_ERROR("CarCallbackInterfaceProxy::ProxyEntry reads parameters failed");
        return ec;
    }

    //Callback* callback;
    Callback* callback = new Callback(pThis->mOwner, methodInfo, paramCount, npArgs, outParamPtrs, outParamTypes, outParamElementTypes, pThis->mOwner->mIsolate);

    //if (hasOutParams || pThis->mOwner->mMainThread == pthread_self()) {
    if (pThis->mOwner->mMainThread == pthread_self()) {
        FireCallback((void*)callback);
    }
    else {
        JavaSharedClient::EnqueueFunctionPtr(FireCallback, (void*)callback);
    }

    return NOERROR;
}


CarCallbackInterfaceProxy::Callback::~Callback()
{
    // _NPN_ReleaseVariantValue
    for (Int32 i = 0; i < mParamCount; ++i) {
        _NPN_ReleaseVariantValue(&mNPParams[i]);
    }
    delete[] mNPParams;
    delete[] mOutParamPtrs;
}

const char* ToCString(const v8::String::Utf8Value& value) {
    return *value ? *value : "<string conversion failed>";
}

void ReportException(v8::Isolate* isolate, v8::TryCatch* try_catch) {
  v8::HandleScope handle_scope(isolate);
  v8::String::Utf8Value exception(try_catch->Exception());
  const char* exception_string = ToCString(exception);
  v8::Handle<v8::Message> message = try_catch->Message();
  if (message.IsEmpty()) {
    // V8 didn't provide any extra information about this error; just
    // print the exception.
    ALOGD("js error====%s\n", exception_string);
  } else {
    // Print (filename):(line number): (message).
    v8::String::Utf8Value filename(message->GetScriptOrigin().ResourceName());
    const char* filename_string = ToCString(filename);
    int linenum = message->GetLineNumber();
    ALOGD("js error====%s:%i: %s\n", filename_string, linenum, exception_string);
    // Print line of source code.
    v8::String::Utf8Value sourceline(message->GetSourceLine());
    const char* sourceline_string = ToCString(sourceline);
    ALOGD("%s\n", sourceline_string);
    // Print wavy underline (GetUnderline is deprecated).
    int start = message->GetStartColumn();
    for (int i = 0; i < start; i++) {
      ALOGD(" ");
    }
    int end = message->GetEndColumn();
    for (int i = start; i < end; i++) {
      ALOGD("^");
    }
    ALOGD("\n");
    v8::String::Utf8Value stack_trace(try_catch->StackTrace());
    if (stack_trace.length() > 0) {
      const char* stack_trace_string = ToCString(stack_trace);
      ALOGD("%s\n", stack_trace_string);
    }
  }
}

void CarCallbackInterfaceProxy::Callback::Call()
{
    ALOGD("====CarCallbackInterfaceProxy::Callback::Call======begin======");

    Int32 paramCount;
    mMethodInfo->GetParamCount(&paramCount);

    if (paramCount != mParamCount) {
        //LOG_ERROR("CarCallbackInterfaceProxy::Callback::Call paramCount is wrong");
        ALOGD("CarCallbackInterfaceProxy::Callback::Call paramCount is wrong");
        return;
    }

    ALOGD("====CarCallbackInterfaceProxy::Callback::Call============thread id:%x",pthread_self());

    v8::Isolate* isolate = mObject->mIsolate;
    v8::Handle<v8::Context> context = isolate->GetCurrentContext();
    v8::HandleScope scope(isolate);

    NPObject* obj = mObject->mObject;
    if (obj->_class == WebCore::npScriptObjectClass) {
        Elastos::String methodName;
        mMethodInfo->GetName(&methodName);

        NPVariant npvValue;
        _NPN_GetProperty(0, obj, _NPN_GetStringIdentifier((const char*)methodName), &npvValue);

        if (npvValue.type == NPVariantType_Object) {
            WebCore::V8NPObject* v8NPFuncObject = (WebCore::V8NPObject*)(NPVARIANT_TO_OBJECT(npvValue));

            v8::Local<v8::Object> jsObject;
            v8::Local<v8::Function> jsFunc;
            if (v8NPFuncObject->v8Object.IsWeak()) {
                jsObject = v8::Local<v8::Object>::New(isolate,v8NPFuncObject->v8Object);
                jsFunc = v8::Local<v8::Function>::Cast(jsObject);
            }
            else {
                jsObject = *reinterpret_cast<v8::Local<v8::Object>*>(const_cast<v8::Persistent<v8::Object>*>(&v8NPFuncObject->v8Object));
                jsFunc = v8::Local<v8::Function>::Cast(jsObject);
            }

            v8::Handle<v8::Value>* argv = ConvertParams();

            v8::TryCatch try_catch;
            jsFunc->Call(context->Global(), mParamCount, argv);
            if (try_catch.HasCaught()) {
                //FatalException(try_catch);
                ReportException(isolate, &try_catch);
            }

            //get out params
            for (Int32 i = 0; i < mParamCount; i++) {
                if (!mOutParamPtrs[i]) continue;

                v8::Handle<v8::Value> outV8Handle = argv[i];

                v8::Local<v8::Object> outV8Object(v8::Handle<v8::Object>::Cast(outV8Handle));

                WebCore::V8NPObject* v8NPObject = (WebCore::V8NPObject*)_NPN_CreateObject(NULL, WebCore::npScriptObjectClass);
                v8NPObject->v8Object.Reset(isolate,outV8Object);
                v8NPObject->rootObject = getRootObject();

                NPVariant npvOutValue;
                _NPN_GetProperty(0, (NPObject*)v8NPObject, _NPN_GetStringIdentifier("data"), &npvOutValue);

                switch (mOutParamTypes[i]) {
                    case CarDataType_Int16:
                    {
                        ALOGD("CarCallbackInterfaceProxy::Callback::Call======get output param value===to do====Int16====CarDataType:%d", mOutParamTypes[i]);
                        break;
                    }
                    case CarDataType_Int32:
                    {
                        ALOGD("CarCallbackInterfaceProxy::Callback::Call======get output param value===to do====Int32====CarDataType:%d", mOutParamTypes[i]);
                        break;
                    }
                    case CarDataType_Int64:
                    {
                        ALOGD("CarCallbackInterfaceProxy::Callback::Call======get output param value===to do====Int64====CarDataType:%d", mOutParamTypes[i]);
                        break;
                    }
                    case CarDataType_Byte:
                    {
                        ALOGD("CarCallbackInterfaceProxy::Callback::Call======get output param value===to do====Byte====CarDataType:%d", mOutParamTypes[i]);
                        break;
                    }
                    case CarDataType_Float:
                    {
                        ALOGD("CarCallbackInterfaceProxy::Callback::Call======get output param value===to do====Float====CarDataType:%d", mOutParamTypes[i]);
                        break;
                    }
                    case CarDataType_Double:
                    {
                        ALOGD("CarCallbackInterfaceProxy::Callback::Call======get output param value===to do====Double====CarDataType:%d", mOutParamTypes[i]);
                        break;
                    }
                    // case CarDataType_Char8:
                    // {
                    //     ALOGD("CarCallbackInterfaceProxy::Callback::Call======get output param value===to do====Char8====CarDataType:%d", mOutParamTypes[i]);
                    //     break;
                    // }
                    // case CarDataType_Char16:
                    // {
                    //     ALOGD("CarCallbackInterfaceProxy::Callback::Call======get output param value===to do====Char16====CarDataType:%d", mOutParamTypes[i]);
                    //     break;
                    // }
                    case CarDataType_Char32:
                    {
                        ALOGD("CarCallbackInterfaceProxy::Callback::Call======get output param value===to do====Char32====CarDataType:%d", mOutParamTypes[i]);
                        break;
                    }
                    // case CarDataType_CString:
                    // {
                    //     ALOGD("CarCallbackInterfaceProxy::Callback::Call======get output param value===to do====CString====CarDataType:%d", mOutParamTypes[i]);
                    //     break;
                    // }
                    case CarDataType_String:
                    {
                        ALOGD("CarCallbackInterfaceProxy::Callback::Call======get output param value===to do====String====CarDataType:%d", mOutParamTypes[i]);
                        break;
                    }
                    case CarDataType_Boolean:
                    {
                        *(Boolean*)(mOutParamPtrs[i]) = (Boolean)NPVARIANT_TO_BOOLEAN(npvOutValue);
                        break;
                    }
                    case CarDataType_EMuid:
                    {
                        ALOGD("CarCallbackInterfaceProxy::Callback::Call======get output param value===to do====EMuid====CarDataType:%d", mOutParamTypes[i]);
                        break;
                    }
                    case CarDataType_EGuid:
                    {
                        ALOGD("CarCallbackInterfaceProxy::Callback::Call======get output param value===to do====EGuid====CarDataType:%d", mOutParamTypes[i]);
                        break;
                    }
                    case CarDataType_ECode:
                    {
                        ALOGD("CarCallbackInterfaceProxy::Callback::Call======get output param value===to do====ECode====CarDataType:%d", mOutParamTypes[i]);
                        break;
                    }
                    case CarDataType_LocalPtr:
                    {
                        ALOGD("CarCallbackInterfaceProxy::Callback::Call======get output param value===to do====LocalPtr====CarDataType:%d", mOutParamTypes[i]);
                        break;
                    }
                    case CarDataType_LocalType:
                    {
                        ALOGD("CarCallbackInterfaceProxy::Callback::Call======get output param value===to do====LocalType====CarDataType:%d", mOutParamTypes[i]);
                        break;
                    }
                    case CarDataType_Enum:
                    {
                        ALOGD("CarCallbackInterfaceProxy::Callback::Call======get output param value===to do====Enum====CarDataType:%d", mOutParamTypes[i]);
                        break;
                    }
                    // case CarDataType_StringBuf:
                    // {
                    //     ALOGD("CarCallbackInterfaceProxy::Callback::Call======get output param value===to do====StringBuf====CarDataType:%d", mOutParamTypes[i]);
                    //     break;
                    // }
                    case CarDataType_ArrayOf:
                    {
                        NPObject* object = NPVARIANT_IS_OBJECT(npvOutValue) ? NPVARIANT_TO_OBJECT(npvOutValue) : 0;

                        if (!object) {
                            ALOGD("CarCallbackInterfaceProxy::Callback::Call CarDataType_ArrayOf 1 error: no return object parameter");
                            return;
                        }

                        NPVariant npvLength;
                        bool success = _NPN_GetProperty(0, object, _NPN_GetStringIdentifier("length"), &npvLength);
                        if (!success) {
                            // No length property so we don't know how many elements to put into the array.
                            // Treat this as an error.
                            // JSC sends null for an array that is not an array of strings or basic types,
                            // do this also in the unknown length case.
                            break;
                        }

                        // Convert to null if the length property is not a number.
                        if (!NPVARIANT_IS_INT32(npvLength) && !NPVARIANT_IS_DOUBLE(npvLength)) {
                            break;
                        }

                        // Convert to null if the length property is out of bounds.
                        Int32 length = NPVARIANT_IS_INT32(npvLength) ? NPVARIANT_TO_INT32(npvLength) : (Int32)NPVARIANT_TO_DOUBLE(npvLength);
                        if (length < 0 || length > INT32_MAX) {
                            break;
                        }

                        CarQuintet* carArray = NULL;

                        switch(mOutParamElementTypes[i]) {
                            // case CarDataType_Int16:
                            case CarDataType_Int32:
                            {
                                ALOGD("CarCallbackInterfaceProxy::Callback::Call======output====ArrayOf======Int32====0");

                                carArray = ArrayOf<Int32>::Alloc(length);

                                // Now iterate over each element and add to the array.
                                for (Int32 i = 0; i < length; i++) {
                                    NPVariant npvValue;
                                    _NPN_GetProperty(0, object, _NPN_GetIntIdentifier(i), &npvValue);
                                    Int32 iVal = (Int32)NPVARIANT_TO_DOUBLE(npvValue);
                                    reinterpret_cast< ArrayOf<Int32>* >(carArray)->Set(i, iVal);
                                }

                                *(ArrayOf<Int32>**)(mOutParamPtrs[i]) = (ArrayOf<Int32>*)carArray;
                                _CarQuintet_AddRef(carArray);

                                break;
                            }
                            // case CarDataType_Int64:
                            // case CarDataType_Byte:
                            // case CarDataType_Float:
                            // case CarDataType_Double:
                            // case CarDataType_Char8:
                            // case CarDataType_Char16:
                            // case CarDataType_Char32:
                            // case CarDataType_CString:
                            case CarDataType_String:
                            {
                                ALOGD("CarCallbackInterfaceProxy::Callback::Call======output====ArrayOf======String====0");

                                carArray = ArrayOf<Elastos::String>::Alloc(length);

                                for (Int32 i = 0; i < length; i++) {
                                    NPVariant npvValue;
                                    _NPN_GetProperty(0, object, _NPN_GetIntIdentifier(i), &npvValue);
                                    NPString src = NPVARIANT_TO_STRING(npvValue);
                                    reinterpret_cast< ArrayOf<Elastos::String>* >(carArray)->Set(i, Elastos::String(src.UTF8Characters));
                                }

                                *(ArrayOf<Elastos::String>**)(mOutParamPtrs[i]) = (ArrayOf<Elastos::String>*)carArray;
                                _CarQuintet_AddRef(carArray);

                                break;
                            }
                            // case CarDataType_Boolean:
                            // case CarDataType_EMuid:
                            // case CarDataType_EGuid:
                            // case CarDataType_ECode:
                            // case CarDataType_LocalPtr:
                            // case CarDataType_LocalType:
                            // case CarDataType_Enum:
                            // case CarDataType_StringBuf:
                            // case CarDataType_ArrayOf:
                            // case CarDataType_BufferOf:
                            // case CarDataType_MemoryOf:
                            // case CarDataType_CppVector:
                            // case CarDataType_Struct:
                            // case CarDataType_Interface:
                            default:
                            {
                                ALOGD("CarCallbackInterfaceProxy::ReadParam======LocalPtr======ArrayOf======other");
                                break;
                            }
                        }  //switch

                        break;
                    }
                    // case CarDataType_BufferOf:
                    // {
                    //     ALOGD("CarCallbackInterfaceProxy::Callback::Call======get output param value===to do====BufferOf====CarDataType:%d", mOutParamTypes[i]);
                    //     break;
                    // }
                    // case CarDataType_MemoryBuf:
                    // {
                    //     ALOGD("CarCallbackInterfaceProxy::Callback::Call======get output param value===to do====MemoryBuf====CarDataType:%d", mOutParamTypes[i]);
                    //     break;
                    // }
                    case CarDataType_CppVector:
                    {
                        ALOGD("CarCallbackInterfaceProxy::Callback::Call======get output param value===to do====CppVector====CarDataType:%d", mOutParamTypes[i]);
                        break;
                    }
                    case CarDataType_Struct:
                    {
                        ALOGD("CarCallbackInterfaceProxy::Callback::Call======get output param value===to do====Struct====CarDataType:%d", mOutParamTypes[i]);
                        break;
                    }
                    case CarDataType_Interface:
                    {
                        if (mOutParamPtrs[i]) {
                            ALOGD("CarCallbackInterfaceProxy::Callback::Call======get output param value====Interface====CarDataType:%d", mOutParamTypes[i]);
                            NPObject* obj = NPVARIANT_TO_OBJECT(npvOutValue);
                            CarNPObject* carObj = reinterpret_cast<CarNPObject*>(obj);
                            CobjectWrapper* objectWrapper = carObj->mInstance->getInstance();

                            *(IInterface**)(mOutParamPtrs[i]) = objectWrapper->getInstance();
                        }
                        else {
                            ALOGD("CarCallbackInterfaceProxy::Callback::Call======get output param value====Interface null====CarDataType:%d", mOutParamTypes[i]);
                        }

                        break;
                    }
                    default: {
                        ALOGD("CarCallbackInterfaceProxy::Callback::Call======get output param value===to do===other====CarDataType:%d", mOutParamTypes[i]);
                        break;
                    }
                }   //switch (mOutParamTypes[i])

                ALOGD("CarCallbackInterfaceProxy::Callback::Call======get output param value===2===");
            }

            delete[] argv;
        }
    }
    else {
        ALOGD("CarCallbackInterfaceProxy::Callback::Call object is not js object");
    }
}

v8::Handle<v8::Value>* CarCallbackInterfaceProxy::Callback::ConvertParams()
{
    if (mParamCount == 0) return NULL;

    v8::Handle<v8::Value>* params = new v8::Handle<v8::Value>[mParamCount];
    for (Int32 i = 0; i < mParamCount; i++) {
        params[i] = WebCore::convertNPVariantToV8Object(&mNPParams[i], NULL);
    }

    return params;
}

// Callback in the main thread.
static void ReleaseNPObject(void* object)
{
    _NPN_ReleaseObject(reinterpret_cast<NPObject*>(object));
}

CarCallbackObject::~CarCallbackObject()
{
    delete mInterface;
    JavaSharedClient::EnqueueFunctionPtr(ReleaseNPObject, reinterpret_cast<void*>(mObject));
    mObject = NULL;
}

PInterface CarCallbackObject::Probe(
    /* [in] */ REIID riid)
{
    if (riid == mInterface->mIID) {
        return (IInterface*)&mInterface->mVptr;
    }
    else if (riid == EIID_IInterface) {
        return (IInterface*)(IObject*)this;
    }
    else if (riid == EIID_IObject) {
        return (IObject*)this;
    }
    else if (riid == EIID_IWeakReferenceSource) {
        return (IWeakReferenceSource*)this;
    }
    else if (riid == EIID_CarCallbackObject) {
        return (IObject*)this;
    }

    return NULL;
}

UInt32 CarCallbackObject::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 CarCallbackObject::Release()
{
    return ElRefBase::Release();
}

ECode CarCallbackObject::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    if (pIID == NULL) return E_INVALID_ARGUMENT;

    if (pObject == (IInterface*)&mInterface->mVptr) {
        *pIID = mInterface->mIID;
    }
    else if (pObject == (IInterface*)(IObject*)this) {
        *pIID = EIID_IObject;
    }
    else if (pObject == (IInterface*)(IWeakReferenceSource*)this) {
        *pIID = EIID_IWeakReferenceSource;
    }

    return NOERROR;
}

ECode CarCallbackObject::Aggregate(
    /* [in] */ AggregateType type,
    /* [in] */ IInterface* object)
{
    return E_NOT_IMPLEMENTED;
}

ECode CarCallbackObject::GetDomain(
    /* [out] */ IInterface** object)
{
    return E_NOT_IMPLEMENTED;
}

ECode CarCallbackObject::GetClassID(
    /* [out] */ ClassID* clsid)
{
    return E_NOT_IMPLEMENTED;
}

ECode CarCallbackObject::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    if (hashCode == NULL) return E_INVALID_ARGUMENT;

    *hashCode = (Int32)this;
    return NOERROR;
}

ECode CarCallbackObject::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* result)
{
    if (result == NULL) return E_INVALID_ARGUMENT;

    *result = (Probe(EIID_IInterface) == other->Probe(EIID_IInterface));
    return NOERROR;
}

ECode CarCallbackObject::ToString(
    /* [out] */ Elastos::String* info)
{
    if (info == NULL) return E_INVALID_ARGUMENT;

    *info = "Class CarCallbackObject";
    return NOERROR;
}

ECode CarCallbackObject::GetWeakReference(
    /* [out] */ IWeakReference** weakReference)
{
    if (weakReference == NULL) return E_INVALID_ARGUMENT;

    *weakReference = new WeakReferenceImpl(Probe(EIID_IInterface), CreateWeak(this));
    (*weakReference)->AddRef();
    return NOERROR;
}

CarCallbackObject* CarCallbackObject::S_CreateObject(
    /* [in] */ IInterfaceInfo* itfInfo,
    /* [in] */ NPObject* object)
{
    if (InitCarCallbackProxyEntry() != 0) {
        LOG_ERROR("CarCallbackObject::S_CreateObject failed");
        return NULL;
    }

    CarCallbackInterfaceProxy* itfProxy = new CarCallbackInterfaceProxy();
    itfProxy->mInterfaceInfo = itfInfo;
    itfProxy->mVptr = g_marshalVtbl;
    itfProxy->mProxyEntry = (PVoid)&__ProxyEntry;
    itfInfo->GetId(&itfProxy->mIID);

    CarCallbackObject* cbObject = new CarCallbackObject();
    cbObject->mInterface = itfProxy;
    cbObject->mMainThread = pthread_self();     //webview/node.js main thread
    cbObject->mObject = _NPN_RetainObject(object);
    cbObject->mIsolate = v8::Isolate::GetCurrent();

    cbObject->mContext = v8::Isolate::GetCurrent()->GetCurrentContext();

    itfProxy->mOwner = cbObject;

    return cbObject;
}

} // namespace Bindings
} // namespace JSC

#endif // ENABLE(CAR_BRIDGE)
