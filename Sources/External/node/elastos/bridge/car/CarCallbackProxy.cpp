#include "ElNode.h"
#include "config.h"

#if ENABLE(CAR_BRIDGE)

#include "NPV8Object.h"
#include "V8NPUtils.h"
#include "CarSharedClient.h"

#include "CarMethodV8.h"

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
        ALOGD("InitCarCallbackProxyEntry: out of memory");
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
        for (Int32 i = 0; i < mMethodInfos->GetLength(); i++) {
            //TODO
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
    /* [out] */ CarValue** ppCarArgs)
{
    //ALOGD("CarCallbackInterfaceProxy::ReadParam====begin====");

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

        //ALOGD("CarCallbackInterfaceProxy::ReadParam====begin====%d/%d====type:%d", i, paramCount, paramDataType);

        carArgs[i].mTypeInfo = paramTypeInfo;
        carArgs[i].mType = paramDataType;
        carArgs[i].mIOAttribute = paramIOAttr;
        carArgs[i].mObjectWrapper = new CobjectWrapper(NULL,paramTypeInfo);

        switch (paramIOAttr) {
        case ParamIOAttribute_In: {
            switch (paramDataType) {
                case CarDataType_Int16:
                {
                    carArgs[i].value.mInt16Value = *puArgs;
                    puArgs++;
                    break;
                }
                case CarDataType_Int32:
                {
                    carArgs[i].value.mInt32Value = *puArgs;
                    puArgs++;
                    break;
                }
                case CarDataType_Int64:
                {
                    puArgs = (UInt32*)ROUND8((Int32)puArgs);
                    carArgs[i].value.mInt64Value = *puArgs;
                    puArgs += 2;
                    break;
                }
                case CarDataType_Byte:
                {
                    carArgs[i].value.mByteValue = *puArgs;
                    puArgs++;
                    break;
                }
                case CarDataType_Float:
                {
                    carArgs[i].value.mFloatValue = *(Float*)puArgs;
                    puArgs++;
                    break;
                }
                case CarDataType_Double:
                {
                    puArgs = (UInt32*)ROUND8((Int32)puArgs);
                    carArgs[i].value.mDoubleValue = *puArgs;
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
                    carArgs[i].value.mCharValue = *puArgs;
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
                    carArgs[i].value.mBooleanValue = *puArgs;
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
                    carArgs[i].value.mECodeValue = *puArgs;
                    puArgs++;
                    break;
                }
                //case CarDataType_LocalPtr:
                //case CarDataType_LocalType:
                case CarDataType_Enum:
                {
                    carArgs[i].value.mInt32Value = *puArgs;
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
                    //ALOGD("CarCallbackInterfaceProxy::ReadParam====CarDataType_Interface====0====");
                    //carArgs[i].pInterface = (IInterface*)(*puArgs);
                    carArgs[i].value.mObjectValue = (IInterface*)(*puArgs);
                    //ALOGD("CarCallbackInterfaceProxy::ReadParam====CarDataType_Interface====1====");
                    //carArgs[i].mObjectWrapper = new CobjectWrapper(NULL, paramTypeInfo);
                    //ALOGD("CarCallbackInterfaceProxy::ReadParam====CarDataType_Interface====2====");
                    //carArgs[i].mObjectWrapper->setInstance((IInterface*)(*puArgs));

                    //ALOGD("CarCallbackInterfaceProxy::ReadParam====CarDataType_Interface====2====");

                    puArgs++;
                    break;
                }
                default:
                {
                    ALOGD("CarCallbackInterfaceProxy::ReadParam: Invalid [in] type(%08x), param index: %d", paramDataType, i);
                    return E_INVALID_ARGUMENT;
                }
            }
            break;
        }   //case ParamIOAttribute_In
        case ParamIOAttribute_CalleeAllocOut: {
            ALOGD("CarCallbackInterfaceProxy::ReadParam======ParamIOAttribute_CalleeAllocOut======paramDataType:%d", paramDataType);

            carArgs[i].value.mLocalPtr = (LocalPtr)*puArgs;

            puArgs++;
            break;
        }   //case ParamIOAttribute_CalleeAllocOut
        case ParamIOAttribute_CallerAllocOut: {
            ALOGD("CarCallbackInterfaceProxy::ReadParam======ParamIOAttribute_CallerAllocOut======paramDataType:%d", paramDataType);

            carArgs[i].value.mLocalPtr = (LocalPtr)*puArgs;

            puArgs++;
            break;
        }   //case ParamIOAttribute_CallerAllocOut
        default: {  // [out]
            ALOGD("CarCallbackInterfaceProxy::ReadParam: Don't support [in/out] type in callback");

            puArgs++;
            break;
        }   //default
        }   //switch (paramIOAttr)

        (*paramInfos)[i]->Release();
        (*paramInfos)[i] = NULL;
    }

    ArrayOf<IParamInfo*>::Free(paramInfos);

    // ALOGD("CarCallbackInterfaceProxy::ReadParam====end====");

    return NOERROR;
}

// Callback in the main(webview/node.js) thread.
static void FireCallback(void* payload)
{
    CarCallbackInterfaceProxy::Callback* callback = (CarCallbackInterfaceProxy::Callback*)payload;
    callback->Call();
    delete callback;
}

ECode CarCallbackInterfaceProxy::GetMethodInfoByIndex(
    /* [in] */ UInt32 index,
    /* [out] */ IMethodInfo** methodInfo)
{
    if (mMethodInfos == NULL) {
        Int32 count;
        mInterfaceInfo->GetMethodCount(&count);

        if (count == 0) {
            Int32 paramCount = 1;
            CarValue* pCarArgs = new CarValue[paramCount];

            pCarArgs[0].mTypeInfo = mInterfaceInfo;
            pCarArgs[0].mType = CarDataType_Interface;
            pCarArgs[0].mIOAttribute = ParamIOAttribute_CallerAllocOut;
            pCarArgs[0].mObjectWrapper = new CobjectWrapper(NULL,mInterfaceInfo);

            pCarArgs[0].value.mLocalPtr = &mInterfaceInfo;

            Callback* callback = new Callback(mOwner, Elastos::String("GetCallbackInterfaceInfo"), pCarArgs, paramCount);

            if (mOwner->mMainThread == pthread_self()) {
                FireCallback((void*)callback);
            }
            else {
                CarSharedClient::EnqueueFunctionPtr(FireCallback, (void*)callback);
            }

            Elastos::String interfaceName;
            mInterfaceInfo->GetName(&interfaceName);
            ALOGD("CarCallbackInterfaceProxy::GetMethodInfoByIndex====JS interfaceName:%s====", interfaceName.string());

            mInterfaceInfo->GetMethodCount(&count);
            if (count == 0) {
                ALOGD("CarCallbackInterfaceProxy::GetMethodInfoByIndex====error:no valid interface!");
                ASSERT(0);
            }
        }

        mMethodInfos = ArrayOf<IMethodInfo*>::Alloc(count);
        mInterfaceInfo->GetAllMethodInfos(mMethodInfos);
    }

    *methodInfo = (*mMethodInfos)[index];
    (*methodInfo)->AddRef();

    return NOERROR;
}

//Run in Elastos UI thread
ECode CarCallbackInterfaceProxy::ProxyEntry(
    /* [in] */ UInt32 *puArgs)
{
    puArgs++; // skip ret address

    CarCallbackInterfaceProxy* pThis = (CarCallbackInterfaceProxy *)*puArgs;
    puArgs++; // skip this

    UInt32 uMethodIndex = puArgs[-3];

    IMethodInfo* methodInfo;
    pThis->GetMethodInfoByIndex(uMethodIndex, &methodInfo);

    Elastos::String methodName;
    methodInfo->GetName(&methodName);
    ALOGD("====CarCallbackInterfaceProxy::ProxyEntry====methodName:%s", methodName.string());

    CarValue* pCarArgs;
    ECode ec = pThis->ReadParam(methodInfo, puArgs, &pCarArgs);

    if (FAILED(ec)) {
        ALOGD("CarCallbackInterfaceProxy::ProxyEntry reads parameters failed");
        return ec;
    }

    Callback* callback = new Callback(pThis->mOwner, methodInfo, pCarArgs);

    // //if (hasOutParams || pThis->mOwner->mMainThread == pthread_self()) {
    if (pThis->mOwner->mMainThread == pthread_self()) {
        FireCallback((void*)callback);
    }
    else {
        CarSharedClient::EnqueueFunctionPtr(FireCallback, (void*)callback);
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
    // delete[] mOutParamPtrs;

    //TODO: delete mCarArgs
}

void CarCallbackInterfaceProxy::Callback::Call()
{
    ALOGD("CarCallbackInterfaceProxy::Callback::Call====begin====methodName:%s====paramCount:%d", mMethodName.string(), mParamCount);

    v8::Isolate* isolate = mObject->mIsolate;
    v8::Isolate::Scope isolateScope(isolate);
    v8::HandleScope scope(isolate);
    v8::Handle<v8::Context> context = isolate->GetCurrentContext();
    v8::Context::Scope contextScope(context);

    convertCarValuesToNPVariants(mCarArgs, mParamCount, &mNPParams);

    NPObject* obj = mObject->mObject;
    if (obj->_class == WebCore::npScriptObjectClass) {
        WebCore::V8NPObject* v8NPThisObject = (WebCore::V8NPObject*)(obj);
        v8::Local<v8::Object> thisObject = v8::Local<v8::Object>::New(isolate,v8NPThisObject->v8Object);;

        NPVariant npvValue;
        _NPN_GetProperty(0, obj, _NPN_GetStringIdentifier((const char*)mMethodName), &npvValue);

        if (npvValue.type == NPVariantType_Object) {
            WebCore::V8NPObject* v8NPFuncObject = (WebCore::V8NPObject*)(NPVARIANT_TO_OBJECT(npvValue));

            //TODO:
            if (((NPObject*)v8NPFuncObject)->_class == WebCore::npScriptObjectClass) {
                ALOGD("====CarCallbackInterfaceProxy::Callback::Call======NPVariantType_Object===V8 Object===");
            }else {
                ALOGD("====CarCallbackInterfaceProxy::Callback::Call======NPVariantType_Object===car Object===");
            }

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
ALOGD("CarCallbackInterfaceProxy::Callback::Call====6====");

            v8::Handle<v8::Value>* argv = ConvertParams();
ALOGD("CarCallbackInterfaceProxy::Callback::Call====7====");

            v8::TryCatch try_catch;
            //jsFunc->Call(context->Global(), mParamCount, argv);
            jsFunc->Call(thisObject, mParamCount, argv);
            if (try_catch.HasCaught()) {
                ALOGD("====CarCallbackInterfaceProxy::Callback::Call======NPVariantType_Object===run js error===");
                //FatalException(try_catch);
                ReportException(isolate, &try_catch);
            }
ALOGD("CarCallbackInterfaceProxy::Callback::Call====8====");

            //get out params
            convertV8ValuesToCarValues(argv, mParamCount, &mCarArgs);
ALOGD("CarCallbackInterfaceProxy::Callback::Call====9====");

            delete[] argv;
        }
        else {
            ALOGD("====CarCallbackInterfaceProxy::Callback::Call======not NPVariantType_Object======type:%d", npvValue.type);
        }
    }
    else {
        ALOGD("CarCallbackInterfaceProxy::Callback::Call object is not js object");
    }
ALOGD("CarCallbackInterfaceProxy::Callback::Call====10====");

    //ALOGD("CarCallbackInterfaceProxy::Callback::Call====end====");
}

v8::Handle<v8::Value>* CarCallbackInterfaceProxy::Callback::ConvertParams()
{
    if (mParamCount == 0) return NULL;

    v8::Isolate* isolate = mObject->mIsolate;

    v8::Handle<v8::Value>* params = new v8::Handle<v8::Value>[mParamCount];
    for (Int32 i = 0; i < mParamCount; i++) {
        params[i] = WebCore::convertNPVariantToV8Object(&mNPParams[i], NULL, isolate);
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
    CarSharedClient::EnqueueFunctionPtr(ReleaseNPObject, reinterpret_cast<void*>(mObject));
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
    else {
        ALOGD("====CarCallbackObject::Probe==begin==error: no interface found!====return mVptr");
        return (IInterface*)&mInterface->mVptr;
    }

    ALOGD("====CarCallbackObject::Probe==begin==error: no interface found!");
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
        ALOGD("CarCallbackObject::S_CreateObject failed");
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