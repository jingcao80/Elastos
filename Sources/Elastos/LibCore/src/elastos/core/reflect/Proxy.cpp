
#include "elastos/core/reflect/Proxy.h"
#include "elastos/utility/logging/Logger.h"
#include <sys/mman.h>

using Elastos::Utility::Logging::Logger;

ELAPI LookupInterfaceInfo(
    /* [in] */ REMuid ritfid,
    /* [out] */ CIInterfaceInfo** interfaceInfo);

namespace Elastos {
namespace Core {
namespace Reflect {

#ifndef PAGE_ALIGN
#define PAGE_ALIGN(va) (((va)+PAGE_SIZE-1)&PAGE_MASK)
#endif

#define MSH_MAX_METHODS         (224 + 4)

static UInt32 g_marshalVtbl[MSH_MAX_METHODS];

Address s_proxyEntryAddress = (Address)NULL;

#define PROXY_ENTRY_BASE    s_proxyEntryAddress
#define PROXY_ENTRY_SIZE    0x10
#define PROXY_ENTRY_MASK    0x0f
#define PROXY_ENTRY_SHIFT   4
#define PROXY_ENTRY_NUM     0xe0

EXTERN_C void ProxyEntryFunc(void);

#ifdef _GNUC
#    if defined(_arm)
#       define DECL_SYS_PROXY_ENTRY()              \
            __asm__(                               \
            ".text;"                               \
            ".align 4;"                            \
            ".globl ProxyEntryFunc;"               \
            "ProxyEntryFunc:"                      \
            "push {r0 - r3};"                      \
            "push {lr};"                           \
            "mov  r1, #0xff;"                      \
            "ldr  pc, [r0, #4];"                   \
        )
        DECL_SYS_PROXY_ENTRY();
#    elif defined(_x86)
#       define DECL_SYS_PROXY_ENTRY()              \
            __asm__(                               \
                ".text;"                           \
                ".align 4;"                        \
                ".globl _ProxyEntryFunc;"          \
                "_ProxyEntryFunc:"                 \
                ".intel_syntax;"                   \
                "push   esp;"                      \
                "mov    eax, dword ptr [esp + 8];" \
                "call   dword ptr [eax + 4];"      \
                "ret    0x4;"                      \
                ".att_syntax;"                     \
            )
        DECL_SYS_PROXY_ENTRY();
#    else
#        error "Unknown archibute"
#   endif
#else
#   if defined(_x86)
#       define DECL_SYS_PROXY_ENTRY() \
            __declspec( naked ) void ProxyEntryFunc() \
            {                                       \
                __asm push esp                      \
                __asm mov eax, dword ptr [esp + 8]  \
                __asm call dword ptr [eax +4]       \
                __asm ret  0x4                      \
            }
        DECL_SYS_PROXY_ENTRY();
#   endif
#endif

#ifdef _arm

EXTERN_C void __ProxyEntry(void);

EXTERN_C ECode GlobalProxyEntry(UInt32* args)
{
    return CInterfaceProxy::ProxyEntry(args);
}

#ifdef _GNUC
#define DECL_PROXY_ENTRY()              \
    __asm__(                            \
        ".text;"                        \
        ".align 4;"                     \
        ".globl __ProxyEntry;"          \
        "__ProxyEntry:"                 \
        "push   {r1};"                  \
        "add    r0, sp, #4;"            \
        "bl     GlobalProxyEntry;"      \
        "add    sp, sp, #4;"            \
        "pop    {lr};"                  \
        "pop    {r1};"                  \
        "pop    {r1 - r3};"             \
        "push   {lr};"                  \
        "pop    {pc};"                  \
    )

DECL_PROXY_ENTRY();
#endif

#endif

__attribute__((constructor))
void InitProxyEntry()
{
    s_proxyEntryAddress = (Address)mmap((void*)0,
            PAGE_ALIGN(PROXY_ENTRY_NUM * PROXY_ENTRY_SIZE),
            PROT_READ | PROT_WRITE | PROT_EXEC,
            MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (s_proxyEntryAddress == 0) {
        Logger::E("Proxy", "out of memory.\n");
        return;
    }

#if defined(_x86)
    char* p = (char *)s_proxyEntryAddress;
    for (Int32 n = 0; n < PROXY_ENTRY_NUM; n++) {
        memcpy(p, (void*)&ProxyEntryFunc, PROXY_ENTRY_SIZE);
        *(Int16*)&(p[SYS_PROXY_RET_OFFSET]) = ((n + 1) << 2);
        p+= PROXY_ENTRY_SIZE;
    }
#elif defined(_arm)
    char* p = (char *)s_proxyEntryAddress;
    for (Int32 n = 0; n < PROXY_ENTRY_NUM; n++) {
        memcpy(p, (void *)&ProxyEntryFunc, PROXY_ENTRY_SIZE);
        p[8] = n;
        p += PROXY_ENTRY_SIZE;
    }
#else
#error "unsupported architecture yet."
#endif

    g_marshalVtbl[0] = (UInt32)&CInterfaceProxy::S_Probe;
    g_marshalVtbl[1] = (UInt32)&CInterfaceProxy::S_AddRef;
    g_marshalVtbl[2] = (UInt32)&CInterfaceProxy::S_Release;
    g_marshalVtbl[3] = (UInt32)&CInterfaceProxy::S_GetInterfaceID;
    for (Int32 m = 4; m < MSH_MAX_METHODS; m++) {
        g_marshalVtbl[m] = PROXY_ENTRY_BASE + ((m - 4) << PROXY_ENTRY_SHIFT);
    }
}

__attribute__((destructor))
void UninitProxyEntry()
{
     if (0 != s_proxyEntryAddress) {
        munmap((void*)s_proxyEntryAddress,
               PAGE_ALIGN(PROXY_ENTRY_NUM * PROXY_ENTRY_SIZE));
        s_proxyEntryAddress = (Address)NULL;
    }
}

PInterface CInterfaceProxy::S_Probe(
    /* [in] */ CInterfaceProxy* thisPtr,
    /* [in] */ REIID riid)
{
    return thisPtr->mOwner->Probe(riid);
}

UInt32 CInterfaceProxy::S_AddRef(
    /* [in] */ CInterfaceProxy* thisPtr)
{
    return thisPtr->mOwner->AddRef();
}

UInt32 CInterfaceProxy::S_Release(
    /* [in] */ CInterfaceProxy* thisPtr)
{
    return thisPtr->mOwner->Release();
}

ECode CInterfaceProxy::S_GetInterfaceID(
    /* [in] */ CInterfaceProxy* thisPtr,
    /* [in] */ IInterface* object,
    /* [out] */ InterfaceID* iid)
{
    //todo: not correct.
    return thisPtr->mOwner->GetInterfaceID(object, iid);
}


ECode CInterfaceProxy::ProxyEntry(
    /* [in] */ UInt32* args)
{
    assert(0);
    return NOERROR;
}


CObjectProxy::CObjectProxy()
    : mInfo(NULL)
    , mInterfaceNum(0)
    , mInterfaces(NULL)
    , mCallbackConnector(NULL)
{}

CObjectProxy::~CObjectProxy()
{
    if (mInterfaces) {
        // mInterfaces[n].mVTPtr = &g_marshalVtbl, delete it ????
        //
        // for (Int32 n = 0; n < mInterfaceNum; n++) {
        //     if (mInterfaces[n].mVTPtr) {
        //         delete [] (UInt32 *)(mInterfaces[n].mVTPtr);
        //     }
        // }
        delete [] mInterfaces;
    }
}

// static const EMuid ECLSID_XOR_CallbackSink = \
// /* e724df56-e16a-4599-8edd-a97ab245d583 */
// {0xe724df56,0xe16a,0x4599,{0x8e,0xdd,0xa9,0x7a,0xb2,0x45,0xd5,0x83}};

PInterface CObjectProxy::Probe(REIID riid)
{
    int n;

    if (riid == EIID_IInterface) {
        return (IInterface*)(IProxy*)this;
    }
    else if (riid == EIID_IProxy) {
        return (IProxy*)this;
    }
    else if (riid == EIID_IObject) {
        return (IObject*)this;
    }
    else if (riid == EIID_IWeakReferenceSource) {
        return (IWeakReferenceSource*)this;
    }

    // if (riid == EIID_CALLBACK_CONNECTOR) {
    //     if (NULL == mCallbackConnector) {
    //         ClassID ezclsid;
    //         IInterface* temp = (IInterface *)&(mInterfaces[0].mVTPtr);
    //         this->GetClassID(&ezclsid.mClsid);

    //         ezclsid.mClsid.mData1    ^= ECLSID_XOR_CallbackSink.mData1;
    //         ezclsid.mClsid.mData2    ^= ECLSID_XOR_CallbackSink.mData2;
    //         ezclsid.mClsid.mData3    ^= ECLSID_XOR_CallbackSink.mData3;
    //         ezclsid.mClsid.mData4[0] ^= ECLSID_XOR_CallbackSink.mData4[0];
    //         ezclsid.mClsid.mData4[1] ^= ECLSID_XOR_CallbackSink.mData4[1];
    //         ezclsid.mClsid.mData4[2] ^= ECLSID_XOR_CallbackSink.mData4[2];
    //         ezclsid.mClsid.mData4[3] ^= ECLSID_XOR_CallbackSink.mData4[3];
    //         ezclsid.mClsid.mData4[4] ^= ECLSID_XOR_CallbackSink.mData4[4];
    //         ezclsid.mClsid.mData4[5] ^= ECLSID_XOR_CallbackSink.mData4[5];
    //         ezclsid.mClsid.mData4[6] ^= ECLSID_XOR_CallbackSink.mData4[6];
    //         ezclsid.mClsid.mData4[7] ^= ECLSID_XOR_CallbackSink.mData4[7];
    //         ezclsid.mUunm = (char*)alloca(sizeof(char) \
    //                 * (strlen(((CIClassInfo*)mInfo)->mUunm) + 1));
    //         strcpy(ezclsid.mUunm, ((CIClassInfo*)mInfo)->mUunm);
    //         _CObject_AcquireClassFactory(ezclsid, RGM_SAME_DOMAIN, EIID_CALLBACK_CONNECTOR, &temp);
    //         mCallbackConnector = (ICallbackConnector*)temp;
    //     }
    //     return (IInterface*)mCallbackConnector;
    // }

    for (n = 0; n < mInterfaceNum; n++) {
        if (riid == mInterfaces[n].mInfo->mIID) {
            break;
        }
    }
    if (n == mInterfaceNum) {
        // MARSHAL_DBGOUT(MSHDBG_WARNING, ALOGW(
        //         "Proxy: QI failed, iid: "));
        // MARSHAL_DBGOUT(MSHDBG_WARNING, DUMP_GUID(riid));

        return NULL;
    }

    return (IInterface *)&(mInterfaces[n].mVTPtr);
}

UInt32 CObjectProxy::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 CObjectProxy::Release()
{
    return ElRefBase::Release();
}

ECode CObjectProxy::GetInterfaceID(
    /* [in] */ IInterface* object,
    /* [out] */ InterfaceID* iid)
{
    if (NULL == iid) return E_INVALID_ARGUMENT;

    if (object == (IInterface*)(IProxy*)this) {
        *iid = EIID_IProxy;
    }
    else if (object == (IInterface*)(IObject*)this) {
        *iid = EIID_IObject;
    }
    else if (object == (IInterface*)(IWeakReferenceSource*)this) {
        *iid = EIID_IWeakReferenceSource;
    }
    else {
        return E_INVALID_ARGUMENT;
    }
    return NOERROR;
}

ECode CObjectProxy::GetInterface(
    /* [in] */ UInt32 index,
    /* [out] */ IInterface** object)
{
    assert(object != NULL);

    if (index < (UInt32)mInterfaceNum) {
        *object = (IInterface *)&(mInterfaces[index].mVTPtr);
        AddRef();
        return NOERROR;
    }
    // MARSHAL_DBGOUT(MSHDBG_WARNING, ALOGW(
    //         "Proxy: IndexQI failed - idx(%d), mInterfaceCount(%d)\n",
    //         index, mInterfaceNum));

    return E_NO_INTERFACE;
}

ECode CObjectProxy::GetInterfaceIndex(
     /* [in] */ IInterface* object,
     /* [out] */ UInt32* index)
{
    assert(index != NULL);

    Int32 i = 0;
    while (i < mInterfaceNum) {
        if ((void*)object == &(mInterfaces[i].mVTPtr)) {
            *index = i;
            return NOERROR;
        }
        i++;
    }
    return E_NO_INTERFACE;
}

ECode CObjectProxy::GetClassID(
    /* [out] */ EMuid* clsid)
{
    assert(clsid != NULL);

    *clsid = ((CIClassInfo*)mInfo)->mCLSID;
    return NOERROR;
}

ECode CObjectProxy::GetClassInfo(
    /* [out] */ CIClassInfo** classInfo)
{
    assert(classInfo != NULL);

    *classInfo = mInfo;
    return NOERROR;
}

ECode CObjectProxy::IsStubAlive(
    /* [out] */ Boolean* result)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CObjectProxy::LinkToDeath(
    /* [in] */ IProxyDeathRecipient* recipient,
    /* [in] */ Int32 flags)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CObjectProxy::UnlinkToDeath(
    /* [in] */ IProxyDeathRecipient* recipient,
    /* [in] */ Int32 flags,
    /* [out] */ Boolean* result)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CObjectProxy::Aggregate(
    /* [in] */ AggregateType type,
    /* [in] */ IInterface* object)
{
    return E_NOT_IMPLEMENTED;
}

ECode CObjectProxy::GetDomain(
    /* [out] */ IInterface** object)
{
    return E_NOT_IMPLEMENTED;
}

ECode CObjectProxy::GetClassID(
    /* [out] */ ClassID* clsid)
{
    return E_NOT_IMPLEMENTED;
}

ECode CObjectProxy::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    if (hashCode == NULL) return E_INVALID_ARGUMENT;
    *hashCode = (Int32)this;
    return NOERROR;
}

ECode CObjectProxy::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* result)
{
    if (result == NULL) return E_INVALID_ARGUMENT;
    *result = (IObject*)this == IObject::Probe(other);
    return NOERROR;
}

ECode CObjectProxy::ToString(
    /* [out] */ String* info)
{
    if (info == NULL) return E_INVALID_ARGUMENT;
    String str("CObjectProxy{");
    str.AppendFormat("0x%08x}", this);
    *info = str;
    return NOERROR;
}

ECode CObjectProxy::GetWeakReference(
    /* [out] */ IWeakReference** weakReference)
{
    if (weakReference == NULL) return E_INVALID_ARGUMENT;
    *weakReference = new WeakReferenceImpl((IProxy*)this, CreateWeak(this));
    (*weakReference)->AddRef();
    return NOERROR;
}

ECode CObjectProxy::S_CreateObject(
    /* [in] */ IClassLoader* loader,
    /* [in] */ ArrayOf<IInterfaceInfo*>* interfaceInfos,
    /* [in] */ IInvocationHandler* invocationHandler,
    /* [out] */ IInterface** proxy)
{
    CObjectProxy* proxyObj;
    CInterfaceProxy* interfaces;
    Int32 n;
    ECode ec;

    if (interfaceInfos == NULL || interfaceInfos->GetLength() == 0 || proxy == NULL) {
        return E_INVALID_ARGUMENT;
    }

    proxyObj = new CObjectProxy();
    assert(proxyObj != NULL);

    proxyObj->mInterfaceNum = interfaceInfos->GetLength();
    interfaces = new CInterfaceProxy[proxyObj->mInterfaceNum];
    assert(interfaces != NULL);
    proxyObj->mInterfaces = interfaces;
    memset(interfaces, 0, sizeof(CInterfaceProxy) * proxyObj->mInterfaceNum);
    for (n = 0; n < proxyObj->mInterfaceNum; n++) {
        interfaces[n].mIndex = n;
        interfaces[n].mOwner = proxyObj;
        InterfaceID iid;
        (*interfaceInfos)[n]->GetId(&iid);
        LookupInterfaceInfo(iid, &(interfaces[n].mInfo));
        interfaces[n].mVTPtr = g_marshalVtbl;

#ifdef _x86
        interfaces[n].mProxyEntry = (PVoid)&CInterfaceProxy::ProxyEntry;
#elif defined(_arm)
        interfaces[n].mProxyEntry = (PVoid)&__ProxyEntry;
#elif defined(_mips)
        interfaces[n].mProxyEntry = (PVoid)&ProxyContinue;
#else
#error unknown architecture
#endif
    }

    (*proxy) = (IProxy*)proxyObj;
    (*proxy)->AddRef();
    return NOERROR;
}

} // namespace Reflect
} // namespace Core
} // namespace Elastos
