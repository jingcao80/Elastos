
#include <stdio.h>
#include <stdlib.h>
#include <elaatomics.h>
#include <sys/mman.h>
#include "prxstub.h"
#include "rot.h"

EXTERN_C const InterfaceID EIID_IProxy;

ECode RegisterModuleInfo(
    /* [in] */ CIModuleInfo *pSrcModuleInfo);

ECode LookupClassInfo(
    /* [in] */ REMuid rclsid,
    /* [out] */ CIClassInfo **ppClassInfo);

ECode LookupModuleInfo(
    /* [in] */ REMuid rclsid,
    /* [out] */ CIModuleInfo **ppModuleInfo);

void *GetUnalignedPtr(void *pPtr);

#define SYS_PROXY_RET_OFFSET    9

#ifndef PAGE_SIZE
#define PAGE_SIZE (1u << 12)
#endif
#ifndef PAGE_MASK
#define PAGE_MASK (~(PAGE_SIZE - 1))
#endif
#ifndef PAGE_ALIGN
#define PAGE_ALIGN(va) (((va)+PAGE_SIZE-1)&PAGE_MASK)
#endif

namespace Elastos {
namespace IPC {
Address s_proxyEntryAddress = NULL;
}
}
#define PROXY_ENTRY_BASE    Elastos::IPC::s_proxyEntryAddress
#define PROXY_ENTRY_SIZE    0x10
#define PROXY_ENTRY_MASK    0x0f
#define PROXY_ENTRY_SHIFT   4
#define PROXY_ENTRY_NUM     0xc0

#ifdef _x86

inline UInt32 CalcMethodIndex(UInt32 uCallerAddr)
{
    return (uCallerAddr - PROXY_ENTRY_BASE) >> PROXY_ENTRY_SHIFT;
}

#define SYS_PROXY_EXIT(ec, pret, argc)  \
    do {                                \
        UInt32 n;                         \
        n = *(UInt32 *)(pret);            \
        n &= PROXY_ENTRY_MASK;          \
        n += PROXY_ENTRY_BASE + ((argc) << PROXY_ENTRY_SHIFT);  \
        *(UInt32 *)(pret) = n;            \
        return (ec);                    \
    } while (0)

#elif defined(_arm)

#define SYS_PROXY_EXIT(ec, pret, argc)  \
    do {                                \
        return (ec);                    \
    } while (0)

#elif defined(_mips)

inline UInt32 CalcMethodIndex(UInt32 uCallerAddr)
{
    return (uCallerAddr - PROXY_ENTRY_BASE) >> PROXY_ENTRY_SHIFT;
}

#ifdef _GNUC

#define SYS_PROXY_EXIT(ec, pret, argc)  \
    do {                                \
            return  (ec);               \
    } while (0);

#elif defined(_EVC)

#define SYS_PROXY_EXIT(ec, pret, argc)  \
    do {                                \
            return  (ec);               \
    } while (0)

#else
#error unknown architecture
#endif
#endif

static UInt32 g_marshalVtbl[MSH_MAX_METHODS];

EXTERN_C void ProxyEntryFunc(void);

#ifdef _GNUC
#    if defined(_arm)
#       define DECL_SYS_PROXY_ENTRY()
            __asm__(                               \
            ".text;"                            \
            ".align 4;"                         \
            ".globl ProxyEntryFunc;"           \
            "ProxyEntryFunc:"                  \
            "push {r0 - r3};"            \
            "push {lr};"
            "mov  r1, #0xff;"                 \
            "ldr  pc, [r0, #4];"              \
        )
        DECL_SYS_PROXY_ENTRY();
#    elif defined(_x86)
#       define DECL_SYS_PROXY_ENTRY()             \
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
            __declspec( naked ) void ProxyEntryFunc()                   \
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

EXTERN_C ECode GlobalProxyEntry(UInt32 *puArgs)
{
    return Elastos::IPC::CInterfaceProxy::ProxyEntry(puArgs);
}

#ifdef _GNUC
#define DECL_PROXY_ENTRY()              \
    __asm__(                            \
        ".text;"                        \
        ".align 4;"                     \
        ".globl __ProxyEntry;"         \
        "__ProxyEntry:"                \
        "push   {r1};"         \
        "add    r0, sp, #4;"            \
        "bl     GlobalProxyEntry;"     \
        "add    sp, sp, #4;"           \
        "pop    {lr};"                \
        "pop    {r1};"                \
        "pop    {r1 - r3};"            \
        "push   {lr};"                \
        "pop    {pc};"                \
    )

DECL_PROXY_ENTRY();
#endif

#endif

#ifdef _mips

EXTERN_C ECode GlobalProxyEntry(UInt32 *puArgs)
{
    return Elastos::IPC::CInterfaceProxy::ProxyEntry(puArgs);
}

#ifdef _GNUC
void __ProxyContinue()
{
DECL_ASMENTRY(ProxyContinue)
#elif defined(_EVC)
DECL_NAKED void ProxyContinue()
{
#else
#error unknown compiler
#endif
    ASM(
        "sw     $4, 0x0($29);"
        "sw     $5, 0x4($29);"
        "sw     $6, 0x8($29);"
        "sw     $7, 0xc($29);"
        "addiu  $29, $29, -0x1c;"
        "sw     $12, 0x10($29);"
        "add    $4, $29, 0x18;"
        "sw     $4, 0x14($29);"
        "la     $9, GlobalProxyEntry;"
        "jalr   $9;"
        "addiu  $29, $29, 0x1c;"
        "lw     $31, -4($29);"
        "jr     $31;");
}
#endif

namespace Elastos {
namespace IPC {

void InitProxyEntry()
{
    s_proxyEntryAddress =
            (Address)mmap((void*)0,
                          PAGE_ALIGN(PROXY_ENTRY_NUM * PROXY_ENTRY_SIZE),
                          PROT_READ | PROT_WRITE | PROT_EXEC,
                          MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (s_proxyEntryAddress == 0) {
        ALOGE("out of memory.\n");
        return;
    }

#if defined(_x86)
    char *p = (char *)s_proxyEntryAddress;
    for (UInt32 n = 0; n < PROXY_ENTRY_NUM; n++) {
        memcpy(p, (void*)&ProxyEntryFunc, PROXY_ENTRY_SIZE);
        *(Int16*)&(p[SYS_PROXY_RET_OFFSET]) = ( (n + 1) << 2);
        p+= PROXY_ENTRY_SIZE;
    }
#elif defined(_arm)
    char * p = (char *)s_proxyEntryAddress;
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
    for (int m = 4; m < MSH_MAX_METHODS; m++) {
        g_marshalVtbl[m] = PROXY_ENTRY_BASE + ((m - 4) << PROXY_ENTRY_SHIFT);
    }
}

void UninitProxyEntry()
{
     if (0 != s_proxyEntryAddress) {
        munmap((void*)s_proxyEntryAddress,
               PAGE_ALIGN(PROXY_ENTRY_NUM * PROXY_ENTRY_SIZE));
        s_proxyEntryAddress = (Address)NULL;
    }
}

ECode GetRemoteClassInfo(
    /* [in] */ android::IBinder* pRemoteObj,
    /* [in] */ REMuid clsId,
    /* [out] */ CIClassInfo ** ppClassInfo)
{
    ECode ec;
    android::Parcel data, reply;
    int32_t ciSize;
    CIModuleInfo *pModInfo;

    if (ppClassInfo == NULL) return E_INVALID_ARGUMENT;

    if (pRemoteObj->transact(IStub::GET_CLASS_INFO, data, &reply) != android::NO_ERROR) {
        MARSHAL_DBGOUT(MSHDBG_ERROR,
            _DumpGUID(clsId));
        MARSHAL_DBGOUT(MSHDBG_ERROR,
                ALOGE("Get class info failed.\n"));
        ec = E_REMOTE_EXCEPTION;
        goto Exit;
    }

    ec = reply.readInt32();
    if (FAILED(ec)) {
        goto Exit;
    }

    ciSize = reply.readInt32();
    pModInfo = (CIModuleInfo*)reply.readInplace(ciSize);
    ec = RegisterModuleInfo(pModInfo);
    if (FAILED(ec)) {
        MARSHAL_DBGOUT(MSHDBG_ERROR,
            _DumpGUID(clsId));
        MARSHAL_DBGOUT(MSHDBG_ERROR,
                ALOGE("Register module info failed.\n"));
        goto Exit;
    }

    if (FAILED(LookupModuleInfo(clsId, &pModInfo))) {
        MARSHAL_DBGOUT(MSHDBG_ERROR,
            _DumpGUID(clsId));
        MARSHAL_DBGOUT(MSHDBG_ERROR,
                ALOGE("Find module info failed.\n"));
        goto Exit;
    }

    ec = E_DOES_NOT_EXIST;
    for (Int32 m = 0; m < pModInfo->classNum; m++) {
        if (pModInfo->classes[m].clsid == clsId) {
            *ppClassInfo = &(pModInfo->classes[m]);

            MARSHAL_DBGOUT(MSHDBG_NORMAL, ALOGD("iid: "));
            MARSHAL_DBGOUT(MSHDBG_NORMAL,
                    DUMP_GUID((*ppClassInfo)->clsid));
            MARSHAL_DBGOUT(MSHDBG_NORMAL,
                    ALOGD("UUNM: %s.\n", (*ppClassInfo)->pszUunm));
            MARSHAL_DBGOUT(MSHDBG_NORMAL,
                    ALOGD("InterfaceNumber: %d.\n", (*ppClassInfo)->interfaceNum));

            ec = NOERROR;
            goto Exit;
        }
    }

Exit:
    return ec;
}

PInterface CInterfaceProxy::S_Probe(
    /* [in] */ CInterfaceProxy *pThis,
    /* [in] */ REIID riid)
{
    return pThis->m_pOwner->Probe(riid);
}

UInt32 CInterfaceProxy::S_AddRef(
    /* [in] */ CInterfaceProxy *pThis)
{
    return pThis->m_pOwner->AddRef();
}

UInt32 CInterfaceProxy::S_Release(
    /* [in] */ CInterfaceProxy *pThis)
{
    return pThis->m_pOwner->Release();
}

ECode CInterfaceProxy::S_GetInterfaceID(
    /* [in] */ CInterfaceProxy *pThis,
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    //todo: not correct.
    return pThis->m_pOwner->GetInterfaceID(pObject, pIID);
}

ECode CInterfaceProxy::BufferSize(
    /* [in] */ UInt32 uMethodIndex,
    /* [in] */ UInt32 *puArgs,
    /* [out] */ UInt32 *puInSize,
    /* [out] */ UInt32 *puOutSize)
{
    ECode ec;
    const CIMethodInfo *pMethodInfo;
    pMethodInfo = &(m_pInfo->methods[uMethodIndex]);

    ec = Proxy_ProcessMsh_BufferSize(
            pMethodInfo,
            puArgs,
            puInSize,
            puOutSize);
    if (0 != *puOutSize){
        *puOutSize += sizeof(MarshalHeader);
    }

    if (SUCCEEDED(ec)) {
        *puInSize += sizeof(MarshalHeader);
    }
    return ec;
}

ECode CInterfaceProxy::MarshalIn(
    /* [in] */ UInt32 uMethodIndex,
    /* [in] */ UInt32 *puArgs,
    /* [in, out] */ CRemoteParcel *pParcel)
{
    ECode ec;
    const CIMethodInfo *pMethodInfo;
    MarshalHeader *pHeader;

    pMethodInfo = &(m_pInfo->methods[uMethodIndex]);

    ec = Proxy_ProcessMsh_In(
            pMethodInfo,
            puArgs,
            (IParcel*)pParcel);

    if (SUCCEEDED(ec)) {
        pHeader = pParcel->GetMarshalHeader();
        assert(pHeader != NULL);
        pHeader->m_uMagic = MARSHAL_MAGIC;
        pHeader->m_hInterfaceIndex = m_uIndex;
        pHeader->m_hMethodIndex = uMethodIndex + 4;
    }

    return ec;
}

ECode CInterfaceProxy::UnmarshalOut(
    /* [in] */ UInt32 uMethodIndex,
    /* [out] */ CRemoteParcel *pParcel,
    /* [in] */ UInt32 *puArgs)
{
    // TODO:
//    MarshalHeader *pHeader = pParcel->GetMarshalHeader();
//
//    if (pHeader->m_uMagic != MARSHAL_MAGIC) {
//        MARSHAL_DBGOUT(MSHDBG_ERROR,
//                ALOGE("Proxy unmsh: invalid magic(%x)\n", pHeader->m_uMagic));
//        return E_MARSHAL_DATA_TRANSPORT_ERROR;
//    }

#if defined(_DEBUG)
    // TODO:
//    if (pHeader->m_hInterfaceIndex != (Int16)m_uIndex) {
//        MARSHAL_DBGOUT(MSHDBG_ERROR,
//                ALOGE("Proxy unmsh: invalid iidx(%x)\n",
//                pHeader->m_hInterfaceIndex));
//        return E_MARSHAL_DATA_TRANSPORT_ERROR;
//    }
//    if (pHeader->m_hMethodIndex != (Int16)(uMethodIndex + 4)) {
//        MARSHAL_DBGOUT(MSHDBG_ERROR, ALOGE(
//                "Proxy unmsh: invalid method(%x)\n", pHeader->m_hMethodIndex));
//    }
#endif

    // TODO:
    return Proxy_ProcessUnmsh_Out(
            &(m_pInfo->methods[uMethodIndex]),
            (IParcel*)pParcel,
            0/*pHeader->m_uOutSize - sizeof(MarshalHeader)*/,
            puArgs);
}

UInt32 CInterfaceProxy::CountMethodArgs(
    /* [in] */ UInt32 uMethodIndex)
{
    return GET_LENGTH((m_pInfo->methods[uMethodIndex]).reserved1);
}

Boolean CInterfaceProxy::MethodHasOutArgs(
    /* [in] */ UInt32 uMethodIndex)
{
    int n, cParams;
    const CIMethodInfo *pMethodInfo;
    const CIBaseType *pParams;

    pMethodInfo = &(m_pInfo->methods[uMethodIndex]);
    cParams = pMethodInfo->paramNum;
    pParams = pMethodInfo->params;

    for (n = 0; n < cParams; n++) {
        if (BT_IS_OUT(pParams[n])) return TRUE;
    }

    return FALSE;
}

Boolean CInterfaceProxy::IsMethodOneway(
    /* [in] */ UInt32 uMethodIndex)
{
    return BT_IS_ONEWAY((m_pInfo->methods[uMethodIndex]).dwAttribs);
}

#if defined(_DEBUG) || defined(_MARSHAL_DEBUG)
int _DumpGUID(REIID riid)
{
    ALOGD("%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X\n",
        riid.Data1, riid.Data2, riid.Data3,
        riid.Data4[0], riid.Data4[1], riid.Data4[2],
        riid.Data4[3], riid.Data4[4], riid.Data4[5],
        riid.Data4[6], riid.Data4[7]);
    return printf("%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X\n",
        riid.Data1, riid.Data2, riid.Data3,
        riid.Data4[0], riid.Data4[1], riid.Data4[2],
        riid.Data4[3], riid.Data4[4], riid.Data4[5],
        riid.Data4[6], riid.Data4[7]);
}

int _DumpCLSID(RClassID rclsid)
{
    _DumpGUID((REIID)rclsid);
    ALOGD("%s\n", rclsid.pUunm);
    return printf("%s\n", rclsid.pUunm);
}
#endif // _DEBUG || _MARSHAL_DEBUG

ECode CInterfaceProxy::ProxyEntry(UInt32 *puArgs)
{
    CInterfaceProxy *pThis;
    UInt32 uMethodIndex, uInSize, uOutSize;
    Int32 size = 0;
    MarshalHeader *pInHeader = NULL;
    CRemoteParcel *pInParcel = NULL, *pOutParcel = NULL;
    ECode ec, orgec;
#ifndef _mips
    UInt32 cArgs;
#endif


    puArgs++; // skip ret address

    pThis = (CInterfaceProxy *)*puArgs;
    puArgs++; // skip this

    MARSHAL_DBGOUT(MSHDBG_NORMAL,
            ALOGD("*puArgs = %x, puArgs = %x, ", *puArgs, puArgs));
    MARSHAL_DBGOUT(MSHDBG_NORMAL, ALOGD("iid: "));
    MARSHAL_DBGOUT(MSHDBG_NORMAL, DUMP_GUID(pThis->m_pInfo->iid));

#ifdef _x86
    uMethodIndex = CalcMethodIndex(*(UInt32 *)((UInt32)&puArgs - 4));
#elif defined(_arm)
    uMethodIndex = puArgs[-3];
#elif defined(_mips)
    uMethodIndex = CalcMethodIndex(*(puArgs - 4) - 4);
#else
#error unknown architecture
#endif

#ifndef _mips
    // Get the stack length, not contain "this"
    cArgs = pThis->CountMethodArgs(uMethodIndex);

    MARSHAL_DBGOUT(MSHDBG_NORMAL, ALOGD(
            "Method index(%d), args size(%d)\n", uMethodIndex + 4, cArgs * 4));
#endif

    // Calculate the package size
    //
    // NOTE:
    //  1. Alloc pOutHeader on the stack with MAX-out-size
    //  2. Assign pInHeader->m_uOutSize with MAX-out-size
    //  3. Pass the MIN-out-size to SysInvoke's last parameter
    //  4. Call Thread::ReallocBuffer in SysReply if necessary to pass back the
    //      marshaled-out data with error info
    //
    ec = pThis->BufferSize(uMethodIndex, puArgs, &uInSize, &uOutSize);
    if (FAILED(ec)) {
        MARSHAL_DBGOUT(MSHDBG_ERROR,
                ALOGE("Proxy BufferSize() failed, ec = %x\n", ec));
        goto ProxyExit;
    }

    MARSHAL_DBGOUT(MSHDBG_NORMAL, ALOGD(
            "Buffer size: inSize(%d), outSize(%d)\n", uInSize, uOutSize));
    assert(uInSize >= sizeof(MarshalHeader));

    pInParcel = new CRemoteParcel();
    ec = pThis->MarshalIn(uMethodIndex, puArgs, pInParcel);

    if (SUCCEEDED(ec)) {
        android::Parcel *pData, reply;

        pInParcel->GetElementSize(&size);
        pInParcel->GetElementPayload((Handle32*)&pData);
        pInHeader = pInParcel->GetMarshalHeader();
        pInHeader->m_uInSize = size;
        pInHeader->m_uOutSize = uOutSize;
        MARSHAL_DBGOUT(MSHDBG_NORMAL, ALOGD(
                "Before RemoteInvoke: ParcelSize(%d)\n", size));
        uint32_t flags = pThis->IsMethodOneway(uMethodIndex) ? android::IBinder::FLAG_ONEWAY : 0;
        android::status_t st;
        if ((st = pThis->m_pOwner->m_pBinder->transact(IStub::INVOKE, *pData, &reply, flags))
                != android::NO_ERROR) {
            if (pThis->m_pOwner && pThis->m_pOwner->m_pInfo) {
                MARSHAL_DBGOUT(MSHDBG_ERROR, ALOGE("package : %s, interfaceNum: %d",
                    pThis->m_pOwner->m_pInfo->pszUunm, pThis->m_pOwner->m_pInfo->interfaceNum));
            }
            MARSHAL_DBGOUT(MSHDBG_ERROR, ALOGE("iid: "));
            MARSHAL_DBGOUT(MSHDBG_ERROR, DUMP_GUID(pThis->m_pInfo->iid));
            MARSHAL_DBGOUT(MSHDBG_ERROR, ALOGE("Method index(%d), args size(%d)\n", uMethodIndex + 4, cArgs * 4));
            MARSHAL_DBGOUT(MSHDBG_ERROR, ALOGE("Buffer size: inSize(%d), outSize(%d)\n", uInSize, uOutSize));
            MARSHAL_DBGOUT(MSHDBG_ERROR, ALOGE("Remote invoke failed. <status: 0x%x>\n", st));

            ec = E_REMOTE_EXCEPTION;
            goto ProxyExit;
        }

        // TODO:
//        pOutParcel = new CRemoteParcel(&reply);
//        pOutParcel->ReadInt32(&ec);
        if (flags != android::IBinder::FLAG_ONEWAY) {
            ec = (ECode)reply.readInt32();
            if (SUCCEEDED(ec)) {
                if (pThis->MethodHasOutArgs(uMethodIndex)) {
                    if (uOutSize) {
                        pOutParcel = new CRemoteParcel(&reply);
                        orgec = pThis->UnmarshalOut(uMethodIndex,
                                                    pOutParcel,
                                                    puArgs);
                        if (FAILED(orgec)) {
                            ec = orgec;
                        }
                    }
                }
            }
            else {
                MARSHAL_DBGOUT(MSHDBG_WARNING,
                        ALOGW("proxy RemoteInvoke() exit. ec = %x\n", ec));
            }
        }
    }
    else {
        MARSHAL_DBGOUT(MSHDBG_ERROR,
                ALOGE("proxy MarshalIn() failed. ec = %x\n", ec));

#if defined(_DEBUG) || defined(_MARSHAL_DEBUG)
        if (pThis->m_pOwner && pThis->m_pOwner->m_pInfo) {
            ALOGE(" >>> package : %s, interfaceNum: %d",
                pThis->m_pOwner->m_pInfo->pszUunm, pThis->m_pOwner->m_pInfo->interfaceNum);
        }

        if (pThis->m_pInfo) {
            ALOGE(" >>> interface id");
            _DumpGUID(pThis->m_pInfo->iid);
        }

        ALOGD(" >>> Method index(%d), args size(%d)\n", uMethodIndex + 4, cArgs * 4);
        ALOGD(" >>> Buffer size: inSize(%d), outSize(%d)\n", uInSize, uOutSize);

        assert(0);
#endif
    }

ProxyExit:
    MARSHAL_DBGOUT(MSHDBG_NORMAL, ALOGD("Exit proxy: ec(%x)\n", ec));

    if (pInParcel != NULL) delete pInParcel;
    if (pOutParcel != NULL) delete pOutParcel;

#ifndef _mips
    SYS_PROXY_EXIT(ec, &puArgs - 1, cArgs);
#else
    SYS_PROXY_EXIT(ec, &puArgs - 1, 0);
#endif
}

DeathRecipientList::DeathRecipientList()
{
    MARSHAL_DBGOUT(MSHDBG_NORMAL, ALOGD("New DRL @ %p", this));
}

DeathRecipientList::~DeathRecipientList()
{
    MARSHAL_DBGOUT(MSHDBG_NORMAL, ALOGD("Destroy DRL @ %p", this));
    android::AutoMutex _l(m_Lock);

    // Should never happen -- the JavaDeathRecipient objects that have added themselves
    // to the list are holding references on the list object.  Only when they are torn
    // down can the list header be destroyed.
    if (m_List.size() > 0) {
        android::List< android::sp<CarDeathRecipient> >::iterator iter;
        for (iter = m_List.begin(); iter != m_List.end(); iter++) {
            (*iter)->warnIfStillLive();
        }
    }
}

void DeathRecipientList::Add(
    /* [in] */ const android::sp<CarDeathRecipient>& recipient)
{
    android::AutoMutex _l(m_Lock);

    MARSHAL_DBGOUT(MSHDBG_NORMAL, ALOGD("DRL @ %p : add CDR %p", this, recipient.get()));
    m_List.push_back(recipient);
}

void DeathRecipientList::Remove(
    /* [in] */ const android::sp<CarDeathRecipient>& recipient)
{
    android::AutoMutex _l(m_Lock);

    android::List< android::sp<CarDeathRecipient> >::iterator iter;
    for (iter = m_List.begin(); iter != m_List.end(); iter++) {
        if (*iter == recipient) {
            MARSHAL_DBGOUT(MSHDBG_NORMAL, ALOGD("DRL @ %p : remove CDR %p", this, recipient.get()));
            m_List.erase(iter);
            return;
        }
    }
}

android::sp<CarDeathRecipient> DeathRecipientList::Find(
    /* [in] */ IProxyDeathRecipient* recipient)
{
    android::AutoMutex _l(m_Lock);

    android::List< android::sp<CarDeathRecipient> >::iterator iter;
    for (iter = m_List.begin(); iter != m_List.end(); iter++) {
        if ((*iter)->matches(recipient)) {
            return *iter;
        }
    }
    return NULL;
}

CarDeathRecipient::CarDeathRecipient(
    /* [in] */ IProxyDeathRecipient* recipient,
    /* [in] */ const android::sp<DeathRecipientList>& list) :
    m_pObject(recipient),
    m_List(list)
{
    if (m_pObject != NULL) {
        m_pObject->AddRef();
    }
    // These objects manage their own lifetimes so are responsible for final bookkeeping.
    // The list holds a strong reference to this object.
    MARSHAL_DBGOUT(MSHDBG_NORMAL, ALOGD("Adding CDR %p to DRL %p", this, list.get()));
    list->Add(this);
}

CarDeathRecipient::~CarDeathRecipient()
{
    if (m_pObject != NULL) {
        m_pObject->Release();
        m_pObject = NULL;
    }
}

void CarDeathRecipient::binderDied(
    /* [in] */ const android::wp<android::IBinder>& who)
{
    MARSHAL_DBGOUT(MSHDBG_NORMAL, ALOGD("Receiving binderDied() on CarDeathRecipient %p\n", this));
    if (m_pObject != NULL) {
        ECode ec = m_pObject->ProxyDied();
        if (FAILED(ec)) {
            MARSHAL_DBGOUT(MSHDBG_ERROR,
                ALOGE("*** Uncaught exception returned from death notification ProxyDied()! this: %p, m_pObject: %p",
                    this, m_pObject));
        }

        // Demote from strong ref to weak after binderDied() has been delivered,
        // to allow the DeathRecipient and BinderProxy to be GC'd if no longer needed.
        m_pObject->Release();
        m_pObject = NULL;
    }
}

void CarDeathRecipient::clearReference()
{
    android::sp<DeathRecipientList> list = m_List.promote();
    if (list != NULL) {
        MARSHAL_DBGOUT(MSHDBG_NORMAL, ALOGD("Removing CDR %p from DRL %p", this, list.get()));
        list->Remove(this);
    }
    else {
        MARSHAL_DBGOUT(MSHDBG_NORMAL, ALOGD("clearReference() on CDR %p but DRL wp purged", this));
    }
}

bool CarDeathRecipient::matches(
    /* [in] */ IProxyDeathRecipient* recipient)
{
    bool result;

    if (m_pObject == NULL || recipient == NULL) result = FALSE;
    else result = m_pObject == recipient;

    return result;
}

void CarDeathRecipient::warnIfStillLive()
{
    if (m_pObject != NULL) {
        MARSHAL_DBGOUT(MSHDBG_NORMAL,
                ALOGD("BinderProxy is being destroyed but the application did not call "
                        "unlinkToDeath to unlink all of its death recipients beforehand.  "));
    }
}

CObjectProxy::CObjectProxy() :
    m_pOrgue(NULL),
    m_pInterfaces(NULL),
    m_pICallbackConnector(NULL),
    m_cRef(1)
{
}

CObjectProxy::~CObjectProxy()
{
    if (m_pInterfaces) {
        // m_pInterfaces[n].m_pvVptr = &g_marshalVtbl, delete it ????
        //
        // for (Int32 n = 0; n < m_cInterfaces; n++) {
        //     if (m_pInterfaces[n].m_pvVptr) {
        //         delete [] (UInt32 *)(m_pInterfaces[n].m_pvVptr);
        //     }
        // }
        delete [] m_pInterfaces;
    }

    m_pBinder = NULL;
    m_pOrgue = NULL;
}

static const EMuid ECLSID_XOR_CallbackSink = \
/* e724df56-e16a-4599-8edd-a97ab245d583 */
{0xe724df56,0xe16a,0x4599,{0x8e,0xdd,0xa9,0x7a,0xb2,0x45,0xd5,0x83}};

PInterface CObjectProxy::Probe(REIID riid)
{
    int n;

    if (riid == EIID_IInterface) {
        return (IInterface*)(IProxy*)this;
    }
    else if (riid == EIID_IProxy) {
        return (IProxy*)this;
    }

    if (riid == EIID_CALLBACK_CONNECTOR) {
        if (NULL == m_pICallbackConnector) {
            ClassID ezclsid;
            IInterface *pTemp = (IInterface *)&(m_pInterfaces[0].m_pvVptr);
            this->GetClassID(&ezclsid.clsid);

            ezclsid.clsid.Data1    ^= ECLSID_XOR_CallbackSink.Data1;
            ezclsid.clsid.Data2    ^= ECLSID_XOR_CallbackSink.Data2;
            ezclsid.clsid.Data3    ^= ECLSID_XOR_CallbackSink.Data3;
            ezclsid.clsid.Data4[0] ^= ECLSID_XOR_CallbackSink.Data4[0];
            ezclsid.clsid.Data4[1] ^= ECLSID_XOR_CallbackSink.Data4[1];
            ezclsid.clsid.Data4[2] ^= ECLSID_XOR_CallbackSink.Data4[2];
            ezclsid.clsid.Data4[3] ^= ECLSID_XOR_CallbackSink.Data4[3];
            ezclsid.clsid.Data4[4] ^= ECLSID_XOR_CallbackSink.Data4[4];
            ezclsid.clsid.Data4[5] ^= ECLSID_XOR_CallbackSink.Data4[5];
            ezclsid.clsid.Data4[6] ^= ECLSID_XOR_CallbackSink.Data4[6];
            ezclsid.clsid.Data4[7] ^= ECLSID_XOR_CallbackSink.Data4[7];
            ezclsid.pUunm = (char*)alloca(sizeof(char) \
                    * (strlen(((CIClassInfo*)m_pInfo)->pszUunm) + 1));
            strcpy(ezclsid.pUunm, ((CIClassInfo*)m_pInfo)->pszUunm);
            _CObject_AcquireClassFactory(ezclsid, RGM_SAME_DOMAIN, EIID_CALLBACK_CONNECTOR, &pTemp);
            m_pICallbackConnector = (ICallbackConnector*)pTemp;
        }
        return (IInterface*)m_pICallbackConnector;
    }

    for (n = 0; n < m_cInterfaces; n++) {
        if (riid == m_pInterfaces[n].m_pInfo->iid) {
            break;
        }
    }
    if (n == m_cInterfaces) {
        MARSHAL_DBGOUT(MSHDBG_WARNING, ALOGW(
                "Proxy: QI failed, iid: "));
        MARSHAL_DBGOUT(MSHDBG_WARNING, DUMP_GUID(riid));

        return NULL;
    }

    return (IInterface *)&(m_pInterfaces[n].m_pvVptr);
}

UInt32 CObjectProxy::AddRef(void)
{
    Int32 lRefs = atomic_inc(&m_cRef);

    MARSHAL_DBGOUT(MSHDBG_CREF, ALOGD(
            "Proxy AddRef: %d\n", lRefs));
    return (UInt32)lRefs;
}

UInt32 CObjectProxy::Release(void)
{
    ECode ec;

    Int32 lRefs = atomic_dec(&m_cRef);
    if (lRefs == 0) {
        MARSHAL_DBGOUT(MSHDBG_NORMAL, ALOGD(
                "Proxy destructed.\n"));

        ec = UnregisterImportObject(m_pBinder.get());
        if (ec == S_FALSE) {
            return 1;// other thread hold the proxy
        }

        if (m_pICallbackConnector) {
            m_pICallbackConnector->DisconnectCallbackSink();
            delete m_pICallbackConnector;
        }

        // when BpBinder destructed, BBinder is not released immediately.
        // BBinder may be never released, if IPCThreadState::joinThreadPool is not called.
        // The following codes to ensure that BBinder can be released
        //
        android::Parcel data, reply;
        if (m_pBinder->transact(IStub::RELEASE, data, &reply) != android::NO_ERROR) {
            MARSHAL_DBGOUT(MSHDBG_ERROR, ALOGE("Call stub release failed.\n"));
        }

        delete this;

        return 0;
    }

    MARSHAL_DBGOUT(MSHDBG_CREF, ALOGD(
            "ProxyRelease: %d\n", lRefs));

    return (UInt32)lRefs;
}

ECode CObjectProxy::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    if (NULL == pIID) return E_INVALID_ARGUMENT;

    if (pObject == (IInterface *)(IProxy *)this) {
        *pIID = EIID_IProxy;
    }
    else {
        return E_INVALID_ARGUMENT;
    }
    return NOERROR;
}

ECode CObjectProxy::GetInterface(
    /* [in] */ UInt32 uIndex,
    /* [out] */ IInterface **ppObj)
{
    assert(ppObj != NULL);

    if (uIndex < (UInt32)m_cInterfaces) {
        *ppObj = (IInterface *)&(m_pInterfaces[uIndex].m_pvVptr);
        this->AddRef();
        return NOERROR;
    }
    MARSHAL_DBGOUT(MSHDBG_WARNING, ALOGW(
            "Proxy: IndexQI failed - idx(%d), cInterfaces(%d)\n",
            uIndex, m_cInterfaces));

    return E_NO_INTERFACE;
}

ECode CObjectProxy::GetInterfaceIndex(
     /* [in] */ IInterface *pObj,
     /* [out] */ UInt32* pIndex)
{
    assert(pIndex != NULL);

    Int32 Index = 0;
    while (Index < m_cInterfaces) {
        if ((void*)pObj == &(m_pInterfaces[Index].m_pvVptr)) {
            *pIndex = Index;
            return NOERROR;
        }
        Index++;
    }
    return E_NO_INTERFACE;
}

ECode CObjectProxy::GetClassID(
    /* [out] */ EMuid *pClsid)
{
    assert(pClsid != NULL);

    *pClsid = ((CIClassInfo*)m_pInfo)->clsid;
    return NOERROR;
}

ECode CObjectProxy::GetClassInfo(
    /* [out] */ CIClassInfo **ppClassInfo)
{
    assert(ppClassInfo != NULL);

    *ppClassInfo = m_pInfo;
    return NOERROR;
}

ECode CObjectProxy::IsStubAlive(
    /* [out] */ Boolean* result)
{
    android::IBinder* target = m_pBinder.get();
    if (target == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    bool alive = target->isBinderAlive();
    *result = alive ? TRUE : FALSE;
    return NOERROR;
}

ECode CObjectProxy::LinkToDeath(
    /* [in] */ IProxyDeathRecipient* recipient,
    /* [in] */ Int32 flags)
{
    if (recipient == NULL) {
        return E_INVALID_POINTER;
    }

    android::IBinder* target = m_pBinder.get();
    if (target == NULL) {
        MARSHAL_DBGOUT(MSHDBG_ERROR,
                ALOGE("Binder has been finalized when calling linkToDeath() with recip=%p)\n", recipient));
        return E_FAIL;
    }

    MARSHAL_DBGOUT(MSHDBG_NORMAL,
                ALOGD("linkToDeath: binder=%p recipient=%p\n", target, recipient));

    if (!target->localBinder()) {
        if (m_pOrgue == NULL) m_pOrgue = new DeathRecipientList();
        android::sp<CarDeathRecipient> cdr = new CarDeathRecipient(recipient, m_pOrgue);
        android::status_t err = target->linkToDeath(cdr, NULL, flags);
        if (err != android::NO_ERROR) {
            // Failure adding the death recipient, so clear its reference
            // now.
            cdr->clearReference();
            return E_FAIL;
        }
    }

    return NOERROR;
}

ECode CObjectProxy::UnlinkToDeath(
    /* [in] */ IProxyDeathRecipient* recipient,
    /* [in] */ Int32 flags,
    /* [out] */ Boolean* result)
{
    assert(result != NULL);

    *result = FALSE;
    if (recipient == NULL) {
        return E_INVALID_POINTER;
    }

    android::IBinder* target = m_pBinder.get();
    if (target == NULL) {
        MARSHAL_DBGOUT(MSHDBG_ERROR,
                ALOGE("Binder has been finalized when calling linkToDeath() with recip=%p)\n", recipient));
        return E_FAIL;
    }

    MARSHAL_DBGOUT(MSHDBG_NORMAL,
                ALOGD("unlinkToDeath: binder=%p recipient=%p\n", target, recipient));

    if (!target->localBinder()) {
        android::status_t err = android::NAME_NOT_FOUND;

        // If we find the matching recipient, proceed to unlink using that
        DeathRecipientList* list = m_pOrgue.get();
        android::sp<CarDeathRecipient> origCDR = list->Find(recipient);
        MARSHAL_DBGOUT(MSHDBG_NORMAL,
                ALOGD("   unlink found list %p and CDR %p", list, origCDR.get()));
        if (origCDR != NULL) {
            android::wp<android::IBinder::DeathRecipient> dr;
            err = target->unlinkToDeath(origCDR, NULL, flags, &dr);
            if (err == android::NO_ERROR && dr != NULL) {
                android::sp<android::IBinder::DeathRecipient> sdr = dr.promote();
                CarDeathRecipient* cdr = static_cast<CarDeathRecipient*>(sdr.get());
                if (cdr != NULL) {
                    cdr->clearReference();
                }
            }
        }

        if (err == android::NO_ERROR || err == android::DEAD_OBJECT) {
            *result = TRUE;
        }
        else {
            return E_FAIL;
        }
    }

    return NOERROR;
}

ECode CObjectProxy::S_CreateObject(
    /* [in] */ REMuid rclsid,
    /* [in] */ const android::sp<android::IBinder> & binder,
    /* [out] */ IProxy **ppIProxy)
{
    CObjectProxy *pProxy;
    CInterfaceProxy *pInterfaces;
    Int32 n;
    ECode ec;

    if (ppIProxy == NULL) return E_INVALID_ARGUMENT;

    pProxy = new CObjectProxy();
    if (!pProxy) {
        MARSHAL_DBGOUT(MSHDBG_ERROR,
                ALOGE("Create proxy object: out of memory.\n"));
        return E_OUT_OF_MEMORY;
    }

    pProxy->m_pBinder = binder;

    ec = LookupClassInfo(rclsid, &(pProxy->m_pInfo));
    if (FAILED(ec)) {
        ec = GetRemoteClassInfo(pProxy->m_pBinder.get(),
                                 rclsid,
                                 &pProxy->m_pInfo);
        if (FAILED(ec)) goto ErrorExit;
    }

    pProxy->m_cInterfaces = ((CIClassInfo*)(pProxy->m_pInfo))->interfaceNum;
    pInterfaces = new CInterfaceProxy[pProxy->m_cInterfaces];
    if (!pInterfaces) {
        MARSHAL_DBGOUT(MSHDBG_ERROR,
                ALOGE("Create proxy interfaces: out of memory.\n"));
        ec = E_OUT_OF_MEMORY;
        goto ErrorExit;
    }
    pProxy->m_pInterfaces = pInterfaces;
    memset(pInterfaces, 0, sizeof(CInterfaceProxy) * pProxy->m_cInterfaces);
    for (n = 0; n < pProxy->m_cInterfaces; n++) {
        pInterfaces[n].m_uIndex = n;
        pInterfaces[n].m_pOwner = pProxy;
        CIInterfaceInfo *pInterfaceInfo =
            (CIInterfaceInfo *)GetUnalignedPtr(
                    pProxy->m_pInfo->interfaces + n);
        pInterfaces[n].m_pInfo = pInterfaceInfo;
        pInterfaces[n].m_pvVptr = g_marshalVtbl;

#ifdef _x86
        pInterfaces[n].m_pvProxyEntry = (PVoid)&CInterfaceProxy::ProxyEntry;
#elif defined(_arm)
        pInterfaces[n].m_pvProxyEntry = (PVoid)&__ProxyEntry;
#elif defined(_mips)
        pInterfaces[n].m_pvProxyEntry = (PVoid)&ProxyContinue;
#else
#error unknown architecture
#endif
    }

    ec = RegisterImportObject(pProxy->m_pBinder.get(), (IProxy*)pProxy);
    if (FAILED(ec)) {
        MARSHAL_DBGOUT(MSHDBG_ERROR, ALOGE(
                "Create proxy: register import object failed, ec(%x)\n",
                ec));
        goto ErrorExit;
    }

    *ppIProxy = (IProxy*)pProxy;

    return NOERROR;

ErrorExit:
    delete pProxy;
    return ec;
}

} // namespace IPC
} // namespace Elastos
