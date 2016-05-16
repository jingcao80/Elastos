
#include <stdio.h>
#include <stdlib.h>
#include <elaatomics.h>
#include <sys/mman.h>
#include "prxstub.h"
#include "rot.h"
#include "DataBuffer.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

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
namespace RPC {
Address s_proxyEntryAddress = NULL;
} // namespace RPC
} // namespace Elastos
#define PROXY_ENTRY_BASE    Elastos::RPC::s_proxyEntryAddress
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

EXTERN_C void ProxyEntryFunc_RPC(void);

#ifdef _GNUC
#    if defined(_arm)
#       define DECL_SYS_PROXY_ENTRY()
            __asm__(                               \
            ".text;"                            \
            ".align 4;"                         \
            ".globl ProxyEntryFunc_RPC;"           \
            "ProxyEntryFunc_RPC:"                  \
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
                ".globl _ProxyEntryFunc_RPC;"      \
                "_ProxyEntryFunc_RPC:"             \
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
            __declspec( naked ) void ProxyEntryFunc_RPC()    \
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

EXTERN_C void __ProxyEntry_RPC(void);

EXTERN_C ECode GlobalProxyEntry_RPC(UInt32 *puArgs)
{
    return Elastos::RPC::CInterfaceProxy::ProxyEntry_RPC(puArgs);
}

#ifdef _GNUC
#define DECL_PROXY_ENTRY()              \
    __asm__(                            \
        ".text;"                        \
        ".align 4;"                     \
        ".globl __ProxyEntry_RPC;"         \
        "__ProxyEntry_RPC:"                \
        "push   {r1};"         \
        "add    r0, sp, #4;"            \
        "bl     GlobalProxyEntry_RPC;"     \
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

EXTERN_C ECode GlobalProxyEntry_RPC(UInt32 *puArgs)
{
    return Elastos::RPC::CInterfaceProxy::ProxyEntry_RPC(puArgs);
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
        "la     $9, GlobalProxyEntry_RPC;"
        "jalr   $9;"
        "addiu  $29, $29, 0x1c;"
        "lw     $31, -4($29);"
        "jr     $31;");
}
#endif

static void siguserHandler(int s)
{
    pthread_exit(NULL);
}

namespace Elastos {
namespace RPC {

void InitProxyEntry()
{
    s_proxyEntryAddress =
            (Address)mmap((void*)0,
                          PAGE_ALIGN(PROXY_ENTRY_NUM * PROXY_ENTRY_SIZE),
                          PROT_READ | PROT_WRITE | PROT_EXEC,
                          MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (s_proxyEntryAddress == 0) {
        printf("out of memory.\n");
        return;
    }

#if defined(_x86)
    char *p = (char *)s_proxyEntryAddress;
    for (UInt32 n = 0; n < PROXY_ENTRY_NUM; n++) {
        memcpy(p, (void*)&ProxyEntryFunc_RPC, PROXY_ENTRY_SIZE);
        *(Int16*)&(p[SYS_PROXY_RET_OFFSET]) = ( (n + 1) << 2);
        p+= PROXY_ENTRY_SIZE;
    }
#elif defined(_arm)
    char * p = (char *)s_proxyEntryAddress;
    for (Int32 n = 0; n < PROXY_ENTRY_NUM; n++) {
        memcpy(p, (void *)&ProxyEntryFunc_RPC, PROXY_ENTRY_SIZE);
        p[8] = n;
        p += PROXY_ENTRY_SIZE;
    }
#else
#error "unsupported architecture yet."
#endif

    g_marshalVtbl[0] = (UInt32)&Elastos::RPC::CInterfaceProxy::S_Probe;
    g_marshalVtbl[1] = (UInt32)&Elastos::RPC::CInterfaceProxy::S_AddRef;
    g_marshalVtbl[2] = (UInt32)&Elastos::RPC::CInterfaceProxy::S_Release;
    g_marshalVtbl[3] = (UInt32)&Elastos::RPC::CInterfaceProxy::S_GetInterfaceID;
    for (int m = 4; m < MSH_MAX_METHODS; m++) {
        g_marshalVtbl[m] = PROXY_ENTRY_BASE + ((m - 4) << PROXY_ENTRY_SHIFT);
    }

    signal(SIGUSR2, siguserHandler);
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
    /* [in] */ const String& netAddress,
    /* [in] */ REMuid clsId,
    /* [out] */ CIClassInfo ** ppClassInfo)
{
    ECode ec;
    Int32 ciSize;
    CIModuleInfo *pModInfo;
    Int32 socketfd = -1;
    struct sockaddr_in name;
    String ipAddr;
    Int32 port;
    String COMMAND("GET_CLASS_INFO");
    AutoPtr<CRemoteParcel> data, reply;
    void* payload;
    Int32 index, totalSize, payloadSize, size;
    Byte buf[4096];
    AutoPtr<DataBuffer> dataBuf;

    if (ppClassInfo == NULL) return E_INVALID_ARGUMENT;

    socketfd = socket(PF_INET, SOCK_STREAM, 0);
    if (socketfd < 0) {
        MARSHAL_DBGOUT(MSHDBG_ERROR,
                printf("GetRemoteClassInfo: create socket failed.\n"));
        ec = E_FAIL;
        goto Exit;
    }

    index = netAddress.IndexOf(":");
    ipAddr = netAddress.Substring(0, index);
    port = atoi(netAddress.Substring(index + 1).string());

    name.sin_family = AF_INET;
    name.sin_addr.s_addr = inet_addr(ipAddr);
    name.sin_port = htons(port);

    if (connect(socketfd, (struct sockaddr*)&name, sizeof(name)) < 0) {
        MARSHAL_DBGOUT(MSHDBG_ERROR,
                printf("GetRemoteClassInfo: connect socket failed.\n"));
        ec = E_FAIL;
        goto Exit;
    }
    data = new CRemoteParcel(FALSE);
    data->WriteString(COMMAND);
    data->GetElementPayload((Handle32*)&payload);
    data->GetElementSize(&payloadSize);
    dataBuf = new DataBuffer();
    totalSize = htonl(sizeof(totalSize) + payloadSize);
    dataBuf->Append((void*)&totalSize, sizeof(totalSize));
    dataBuf->Append(payload, payloadSize);
    assert(ntohl(totalSize) == dataBuf->DataSize());
    if (send(socketfd, dataBuf->Data(), dataBuf->DataSize(), 0) == -1) {
        MARSHAL_DBGOUT(MSHDBG_ERROR,
                printf("GetRemoteClassInfo: send socket failed.\n"));
        ec = E_FAIL;
        goto Exit;
    }

    dataBuf->Reset();
    size = recv(socketfd, (void*)buf, sizeof(buf), 0);
    if (size <= 0) {
        MARSHAL_DBGOUT(MSHDBG_ERROR,
                printf("GetRemoteClassInfo: remote socket broken.\n"));
        ec = E_FAIL;
        goto Exit;
    }
    totalSize = ntohl(*(Int32*)buf);
    dataBuf->Append(buf + sizeof(Int32), size - sizeof(Int32));
    while((totalSize = totalSize - size) > 0) {
        size = recv(socketfd, (void*)buf, sizeof(buf), 0);
        if (size <= 0) {
            MARSHAL_DBGOUT(MSHDBG_ERROR,
                    printf("GetRemoteClassInfo: remote socket broken.\n"));
            ec = E_FAIL;
            goto Exit;
        }
        dataBuf->Append(buf, size);
    }

    reply = new CRemoteParcel(FALSE);
    reply->Write(dataBuf->Data(), dataBuf->DataSize());
    reply->SetDataPosition(0);

    reply->ReadInt32(&ec);
    if (FAILED(ec)) {
        goto Exit;
    }
    reply->ReadInt32(&ciSize);
    pModInfo = (CIModuleInfo*)reply->ReadInplace(ciSize);
    ec = RegisterModuleInfo(pModInfo);
    if (FAILED(ec)) {
        MARSHAL_DBGOUT(MSHDBG_ERROR,
            _DumpGUID(clsId));
        MARSHAL_DBGOUT(MSHDBG_ERROR,
                printf("Register module info failed.\n"));
        goto Exit;
    }

    if (FAILED(LookupModuleInfo(clsId, &pModInfo))) {
        MARSHAL_DBGOUT(MSHDBG_ERROR,
            _DumpGUID(clsId));
        MARSHAL_DBGOUT(MSHDBG_ERROR,
                printf("Find module info failed.\n"));
        goto Exit;
    }

    ec = E_DOES_NOT_EXIST;
    for (Int32 m = 0; m < pModInfo->classNum; m++) {
        if (pModInfo->classes[m].clsid == clsId) {
            *ppClassInfo = &(pModInfo->classes[m]);

            MARSHAL_DBGOUT(MSHDBG_NORMAL, printf("iid: "));
            MARSHAL_DBGOUT(MSHDBG_NORMAL,
                    DUMP_GUID((*ppClassInfo)->clsid));
            MARSHAL_DBGOUT(MSHDBG_NORMAL,
                    printf("UUNM: %s.\n", (*ppClassInfo)->pszUunm));
            MARSHAL_DBGOUT(MSHDBG_NORMAL,
                    printf("InterfaceNumber: %d.\n", (*ppClassInfo)->interfaceNum));

            ec = NOERROR;
            goto Exit;
        }
    }

Exit:
    if (socketfd != -1) close(socketfd);
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
//                printf("Proxy unmsh: invalid magic(%x)\n", pHeader->m_uMagic));
//        return E_MARSHAL_DATA_TRANSPORT_ERROR;
//    }

#if defined(_DEBUG)
    // TODO:
//    if (pHeader->m_hInterfaceIndex != (Int16)m_uIndex) {
//        MARSHAL_DBGOUT(MSHDBG_ERROR,
//                printf("Proxy unmsh: invalid iidx(%x)\n",
//                pHeader->m_hInterfaceIndex));
//        return E_MARSHAL_DATA_TRANSPORT_ERROR;
//    }
//    if (pHeader->m_hMethodIndex != (Int16)(uMethodIndex + 4)) {
//        MARSHAL_DBGOUT(MSHDBG_ERROR, printf(
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

ECode CInterfaceProxy::ProxyEntry_RPC(UInt32 *puArgs)
{
    CInterfaceProxy *pThis;
    UInt32 uMethodIndex, uInSize, uOutSize;
    Int32 size, payloadSize, totalSize;
    Boolean isMethodOneway;
    MarshalHeader *pInHeader = NULL;
    AutoPtr<CRemoteParcel> pInParcel, pOutParcel;
    Int32 socketfd;
    struct sockaddr_in name;
    timeval recvTimeout;
    String netAddress, ipAddr;
    Int32 port;
    Int32 index;
    const Int32 tag = htonl(MSH_NOT_NULL);
    const char pad = '\0';
    const char* INVOKE = "INVOKE";
    const char* ONEWAY = "ONEWAY";
    UInt8* pData = NULL;
    char buf[4096];
    AutoPtr<DataBuffer> dataBuf, replyBuf;
    ECode ec, orgec;
#ifndef _mips
    UInt32 cArgs;
#endif


    puArgs++; // skip ret address

    pThis = (CInterfaceProxy *)*puArgs;
    puArgs++; // skip this

    MARSHAL_DBGOUT(MSHDBG_NORMAL,
            printf("*puArgs = %x, puArgs = %x, ", *puArgs, puArgs));
    MARSHAL_DBGOUT(MSHDBG_NORMAL, printf("iid: "));
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

    MARSHAL_DBGOUT(MSHDBG_NORMAL, printf(
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
                printf("Proxy BufferSize() failed, ec = %x\n", ec));
        goto ProxyExit;
    }

    MARSHAL_DBGOUT(MSHDBG_NORMAL, printf(
            "Buffer size: inSize(%d), outSize(%d)\n", uInSize, uOutSize));
    assert(uInSize >= sizeof(MarshalHeader));

    pInParcel = new CRemoteParcel();
    ec = pThis->MarshalIn(uMethodIndex, puArgs, pInParcel);

    if (SUCCEEDED(ec)) {
        if (pThis->m_pOwner->m_cSocketfd == -1) {
            socketfd = socket(PF_INET, SOCK_STREAM, 0);
            if (socketfd < 0) {
                MARSHAL_DBGOUT(MSHDBG_ERROR,
                        printf("ProxyEntry_RPC: create socket failed.\n"));
                ec = E_FAIL;
                goto ProxyExit;
            }

            netAddress = pThis->m_pOwner->m_sNetAddress;
            index = netAddress.IndexOf(":");
            ipAddr = netAddress.Substring(0, index);
            port = atoi(netAddress.Substring(index + 1).string());

            name.sin_family = AF_INET;
            name.sin_addr.s_addr = inet_addr(ipAddr);
            name.sin_port = htons(port);

            if (connect(socketfd, (struct sockaddr*)&name, sizeof(name)) < 0) {
                MARSHAL_DBGOUT(MSHDBG_ERROR,
                        printf("ProxyEntry_RPC: connect socket failed.\n"));
                ec = E_FAIL;
                goto ProxyExit;
            }

            recvTimeout = {60, 0};
            if (setsockopt(socketfd, SOL_SOCKET, SO_RCVTIMEO, &recvTimeout, sizeof(recvTimeout)) == -1) {
                MARSHAL_DBGOUT(MSHDBG_ERROR,
                        printf("ProxyEntry_RPC: set so_rcvtimeo failed.\n"));
                ec = E_FAIL;
                goto ProxyExit;
            }

            pThis->m_pOwner->m_cSocketfd = socketfd;

            if (pThis->m_pOwner->m_pPingTask == NULL) {
                AutoPtr<CObjectProxy::PingRunnable> r = new CObjectProxy::PingRunnable(pThis->m_pOwner);
                ThreadPoolExecutor::Self()->RunTask(r);
                pThis->m_pOwner->m_pPingTask = r;
            }
        }

        isMethodOneway = pThis->IsMethodOneway(uMethodIndex);
        dataBuf = new DataBuffer();
        pInParcel->GetElementSize(&payloadSize);
        pInParcel->GetElementPayload((Handle32*)&pData);
        pInHeader = pInParcel->GetMarshalHeader();
        pInHeader->m_uInSize = payloadSize;
        pInHeader->m_uOutSize = uOutSize;
        totalSize = htonl(sizeof(totalSize) + sizeof(tag) + strlen(INVOKE) + 1 + sizeof(pad)
                + (isMethodOneway ? sizeof(tag) + strlen(ONEWAY) + 1 + sizeof(pad) : 0)
                + payloadSize);
        dataBuf->Append((void*)&totalSize, sizeof(totalSize));
        dataBuf->Append((void*)&tag, sizeof(tag));
        dataBuf->Append(INVOKE, strlen(INVOKE) + 1);
        dataBuf->Append((void*)&pad, sizeof(pad));
        if (isMethodOneway) {
            dataBuf->Append((void*)&tag, sizeof(tag));
            dataBuf->Append(ONEWAY, strlen(ONEWAY) + 1);
            dataBuf->Append((void*)&pad, sizeof(pad));
        }
        dataBuf->Append(pData, payloadSize);
        MARSHAL_DBGOUT(MSHDBG_NORMAL, printf(
                "Before RemoteInvoke: ParcelSize(%d)\n", payloadSize));
        assert(ntohl(totalSize) == dataBuf->DataSize());

        {
            android::AutoMutex _l(pThis->m_pOwner->m_Lock);

            if (send(pThis->m_pOwner->m_cSocketfd, dataBuf->Data(), dataBuf->DataSize(), 0) == -1) {
                MARSHAL_DBGOUT(MSHDBG_ERROR,
                        printf("RemoteInvoke: send socket failed.\n"));
                ec = E_FAIL;
                goto ProxyExit;
            }

            replyBuf = new DataBuffer();
            size = recv(pThis->m_pOwner->m_cSocketfd, (void*)buf, sizeof(buf), 0);
            if (size <= 0) {
                MARSHAL_DBGOUT(MSHDBG_ERROR,
                        printf("RemoteInvoke: remote socket broken.\n"));
                pThis->m_pOwner->StubDied();
                ec = E_FAIL;
                goto ProxyExit;
            }
            totalSize = ntohl(*(Int32*)buf);
            replyBuf->Append(buf + sizeof(Int32), size - sizeof(Int32));
            while((totalSize = totalSize - size) > 0) {
                size = recv(pThis->m_pOwner->m_cSocketfd, (void*)buf, sizeof(buf), 0);
                if (size <= 0) {
                    MARSHAL_DBGOUT(MSHDBG_ERROR,
                            printf("RemoteInvoke: remote socket broken.\n"));
                    pThis->m_pOwner->StubDied();
                    ec = E_FAIL;
                    goto ProxyExit;
                }
                replyBuf->Append(buf, size);
            }
        }

        if (!pThis->IsMethodOneway(uMethodIndex)) {
            pOutParcel = new CRemoteParcel(FALSE);
            pOutParcel->SetData((UInt8*)replyBuf->Data(), replyBuf->DataSize());
            pOutParcel->ReadInt32(&ec);
            if (SUCCEEDED(ec)) {
                if (pThis->MethodHasOutArgs(uMethodIndex)) {
                    if (uOutSize) {
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
                        printf("proxy RemoteInvoke() exit. ec = %x\n", ec));
            }
        }
    }
    else {
        MARSHAL_DBGOUT(MSHDBG_ERROR,
                printf("proxy MarshalIn() failed. ec = %x\n", ec));
    }

ProxyExit:
    MARSHAL_DBGOUT(MSHDBG_NORMAL, printf("Exit proxy: ec(%x)\n", ec));

#ifndef _mips
    SYS_PROXY_EXIT(ec, &puArgs - 1, cArgs);
#else
    SYS_PROXY_EXIT(ec, &puArgs - 1, 0);
#endif
}

DeathRecipientList::DeathRecipientList()
{
    MARSHAL_DBGOUT(MSHDBG_NORMAL, printf("New DRL @ %p", this));
}

DeathRecipientList::~DeathRecipientList()
{
    MARSHAL_DBGOUT(MSHDBG_NORMAL, printf("Destroy DRL @ %p", this));
    android::AutoMutex _l(m_Lock);

    // Should never happen -- the JavaDeathRecipient objects that have added themselves
    // to the list are holding references on the list object.  Only when they are torn
    // down can the list header be destroyed.
    if (m_List.size() > 0) {
        android::List< AutoPtr<CarDeathRecipient> >::iterator iter;
        for (iter = m_List.begin(); iter != m_List.end(); iter++) {
            (*iter)->WarnIfStillLive();
        }
    }
}

PInterface DeathRecipientList::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (IInterface*)this;
    }
    else if (riid == EIID_IWeakReferenceSource) {
        return (IWeakReferenceSource*)this;
    }
    return NULL;
}

UInt32 DeathRecipientList::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 DeathRecipientList::Release()
{
    return ElRefBase::Release();
}

ECode DeathRecipientList::GetInterfaceID(
    /* [in] */ IInterface* object,
    /* [out] */ InterfaceID* iid)
{
    return E_NOT_IMPLEMENTED;
}

ECode DeathRecipientList::GetWeakReference(
    /* [out] */ IWeakReference** weakReference)
{
    *weakReference = new WeakReferenceImpl((IInterface*)this, CreateWeak(this));
    (*weakReference)->AddRef();
    return NOERROR;
}

void DeathRecipientList::Add(
    /* [in] */ const AutoPtr<CarDeathRecipient>& recipient)
{
    android::AutoMutex _l(m_Lock);

    MARSHAL_DBGOUT(MSHDBG_NORMAL, printf("DRL @ %p : add CDR %p", this, recipient.Get()));
    m_List.push_back(recipient);
}

void DeathRecipientList::Remove(
    /* [in] */ const AutoPtr<CarDeathRecipient>& recipient)
{
    android::AutoMutex _l(m_Lock);

    android::List< AutoPtr<CarDeathRecipient> >::iterator iter;
    for (iter = m_List.begin(); iter != m_List.end(); iter++) {
        if (*iter == recipient) {
            MARSHAL_DBGOUT(MSHDBG_NORMAL, printf("DRL @ %p : remove CDR %p", this, recipient.Get()));
            m_List.erase(iter);
            return;
        }
    }
}

AutoPtr<CarDeathRecipient> DeathRecipientList::Find(
    /* [in] */ IProxyDeathRecipient* recipient)
{
    android::AutoMutex _l(m_Lock);

    android::List< AutoPtr<CarDeathRecipient> >::iterator iter;
    for (iter = m_List.begin(); iter != m_List.end(); iter++) {
        if ((*iter)->Matches(recipient)) {
            return *iter;
        }
    }
    return NULL;
}

void DeathRecipientList::NotifyAll(
    /* [in] */ CObjectProxy* proxy)
{
    android::AutoMutex _l(m_Lock);

    android::List< AutoPtr<CarDeathRecipient> >::iterator iter = m_List.begin();
    while (iter != m_List.end()) {
        (*iter)->StubDied(proxy);
        iter = m_List.erase(iter);
    }
}

CarDeathRecipient::CarDeathRecipient(
    /* [in] */ IProxyDeathRecipient* recipient,
    /* [in] */ const AutoPtr<DeathRecipientList>& list)
    : m_pObject(recipient)
{
    if (m_pObject != NULL) {
        m_pObject->AddRef();
    }

    AutoPtr<IWeakReference> wr;
    IWeakReferenceSource::Probe(list)->GetWeakReference((IWeakReference**)&wr);
    // These objects manage their own lifetimes so are responsible for final bookkeeping.
    // The list holds a strong reference to this object.
    m_List = wr;
}

CarDeathRecipient::~CarDeathRecipient()
{
    if (m_pObject != NULL) {
        m_pObject->Release();
        m_pObject = NULL;
    }
}

void CarDeathRecipient::StubDied(
    /* [in] */ const CObjectProxy* who)
{
    MARSHAL_DBGOUT(MSHDBG_NORMAL, printf("Receiving StubDied() on CarDeathRecipient %p\n", this));
    if (m_pObject != NULL) {
        ECode ec = m_pObject->ProxyDied();
        if (FAILED(ec)) {
            MARSHAL_DBGOUT(MSHDBG_ERROR,
                    printf("*** Uncaught exception returned from death notification!"));
        }

        // Demote from strong ref to weak after StubDied() has been delivered,
        // to allow the DeathRecipient and BinderProxy to be GC'd if no longer needed.
        m_pObject->Release();
        m_pObject = NULL;
    }
}

void CarDeathRecipient::ClearReference()
{
    AutoPtr<IInterface> obj;
    m_List->Resolve(EIID_IInterface, (IInterface**)&obj);
    DeathRecipientList* list = (DeathRecipientList*)obj.Get();
    if (list != NULL) {
        MARSHAL_DBGOUT(MSHDBG_NORMAL, printf("Removing CDR %p from DRL %p", this, list));
        list->Remove(this);
    }
    else {
        MARSHAL_DBGOUT(MSHDBG_NORMAL, printf("clearReference() on CDR %p but DRL wp purged", this));
    }
}

Boolean CarDeathRecipient::Matches(
    /* [in] */ IProxyDeathRecipient* recipient)
{
    Boolean result;

    if (m_pObject == NULL || recipient == NULL) result = FALSE;
    else result = m_pObject == recipient;

    return result;
}

void CarDeathRecipient::WarnIfStillLive()
{
    if (m_pObject != NULL) {
        MARSHAL_DBGOUT(MSHDBG_NORMAL,
                printf("BinderProxy is being destroyed but the application did not call "
                        "unlinkToDeath to unlink all of its death recipients beforehand.  "));
    }
}

ECode CObjectProxy::PingRunnable::Run()
{
    const Int32 tag = htonl(MSH_NOT_NULL);
    const char pad = '\0';
    const char* PING = "PING";
    Int32 totalSize = htonl(sizeof(totalSize) + sizeof(tag) + strlen(PING) + 1);
    AutoPtr<DataBuffer> dataBuf = new DataBuffer();
    dataBuf->Append((void*)&totalSize, sizeof(totalSize));
    dataBuf->Append((void*)&tag, sizeof(tag));
    dataBuf->Append(PING, strlen(PING) + 1);

    while (TRUE) {
        sleep(30);

        android::AutoMutex _l(mHost->m_Lock);

        if (send(mHost->m_cSocketfd, dataBuf->Data(), dataBuf->DataSize(), 0) == -1) {
            MARSHAL_DBGOUT(MSHDBG_ERROR,
                    printf("PingRunnable: send socket failed.\n"));
        }

        Byte buf[32];
        Int32 size = recv(mHost->m_cSocketfd, (void*)buf, sizeof(buf), 0);
        if (size <= 0) {
            MARSHAL_DBGOUT(MSHDBG_ERROR,
                    printf("PingRunnable: recv socket broken.\n"));
            break;
        }
    }

    return mHost->StubDied();
}

CObjectProxy::CObjectProxy() :
    m_cSocketfd(-1),
    m_pInterfaces(NULL),
    m_pICallbackConnector(NULL),
    m_bStubDied(FALSE),
    m_cRef(1)
{}

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
    ThreadPoolExecutor::Self()->StopTask(m_pPingTask);
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
        MARSHAL_DBGOUT(MSHDBG_WARNING, printf(
                "Proxy: QI failed, iid: "));
        MARSHAL_DBGOUT(MSHDBG_WARNING, DUMP_GUID(riid));

        return NULL;
    }

    return (IInterface *)&(m_pInterfaces[n].m_pvVptr);
}

UInt32 CObjectProxy::AddRef(void)
{
    Int32 lRefs = atomic_inc(&m_cRef);

    MARSHAL_DBGOUT(MSHDBG_CREF, printf(
            "Proxy AddRef: %d\n", lRefs));
    return (UInt32)lRefs;
}

UInt32 CObjectProxy::Release(void)
{
    ECode ec;

    Int32 lRefs = atomic_dec(&m_cRef);
    if (lRefs == 0) {
        MARSHAL_DBGOUT(MSHDBG_NORMAL, printf(
                "Proxy destructed.\n"));

        ec = UnregisterImportObject(m_sNetAddress);
        if (ec == S_FALSE) {
            return 1;// other thread hold the proxy
        }

        if (m_pICallbackConnector) {
            m_pICallbackConnector->DisconnectCallbackSink();
            delete m_pICallbackConnector;
        }

        //bugbug: maybe we don't need call release of stub,
        //        we should give this task to the binder.
        const Int32 tag = htonl(MSH_NOT_NULL);
        const char* RELEASE = "RELEASE";
        Int32 totalSize = htonl(sizeof(totalSize) + sizeof(tag) + strlen(RELEASE) + 1);
        AutoPtr<DataBuffer> dataBuf = new DataBuffer();
        dataBuf->Append((void*)&totalSize, sizeof(Int32));
        dataBuf->Append((void*)&tag, sizeof(tag));
        dataBuf->Append(RELEASE, strlen(RELEASE) + 1);
        assert(ntohl(totalSize) == dataBuf->DataSize());
        if (send(m_cSocketfd, dataBuf->Data(), dataBuf->DataSize(), 0) == -1) {
            MARSHAL_DBGOUT(MSHDBG_ERROR,
                    printf("GetRemoteClassInfo: send socket failed.\n"));
        }

        delete this;

        return 0;
    }

    MARSHAL_DBGOUT(MSHDBG_CREF, printf(
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
    MARSHAL_DBGOUT(MSHDBG_WARNING, printf(
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
    assert(0);
    return NOERROR;
}

ECode CObjectProxy::LinkToDeath(
    /* [in] */ IProxyDeathRecipient* recipient,
    /* [in] */ Int32 flags)
{
    if (recipient == NULL) {
        return E_INVALID_POINTER;
    }

    if (m_bStubDied) {
        MARSHAL_DBGOUT(MSHDBG_ERROR,
                printf("Remote stub is died when calling linkToDeath() with recip=%p)\n", recipient));
        return E_FAIL;
    }

    MARSHAL_DBGOUT(MSHDBG_NORMAL,
                printf("linkToDeath: recipient=%p\n", recipient));

    if (m_pOrgue == NULL) m_pOrgue = new DeathRecipientList();
    AutoPtr<CarDeathRecipient> cdr = new CarDeathRecipient(recipient, m_pOrgue);
    MARSHAL_DBGOUT(MSHDBG_NORMAL, printf("Adding CDR %p to DRL %p", cdr.Get(), m_pOrgue.Get()));
    m_pOrgue->Add(cdr);

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

    if (m_bStubDied) {
        MARSHAL_DBGOUT(MSHDBG_ERROR,
                printf("Remote stub is died when calling linkToDeath() with recip=%p)\n", recipient));
        return E_FAIL;
    }

    MARSHAL_DBGOUT(MSHDBG_NORMAL,
                printf("unlinkToDeath: recipient=%p\n", recipient));

    // If we find the matching recipient, proceed to unlink using that
    AutoPtr<CarDeathRecipient> origCDR = m_pOrgue->Find(recipient);
    MARSHAL_DBGOUT(MSHDBG_NORMAL,
            printf("   unlink found list %p and CDR %p", m_pOrgue.Get(), origCDR.Get()));
    if (origCDR != NULL) {
        origCDR->ClearReference();
    }

    return NOERROR;
}

ECode CObjectProxy::StubDied()
{
    m_pOrgue->NotifyAll(this);
    return NOERROR;
}

ECode CObjectProxy::S_CreateObject(
    /* [in] */ REMuid rclsid,
    /* [in] */ const String& address,
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
                printf("Create proxy object: out of memory.\n"));
        return E_OUT_OF_MEMORY;
    }

    pProxy->m_sNetAddress = address;

    ec = LookupClassInfo(rclsid, &(pProxy->m_pInfo));
    if (FAILED(ec)) {
        ec = GetRemoteClassInfo(pProxy->m_sNetAddress,
                                rclsid,
                                &pProxy->m_pInfo);
        if (FAILED(ec)) goto ErrorExit;
    }

    pProxy->m_cInterfaces = ((CIClassInfo*)(pProxy->m_pInfo))->interfaceNum;
    pInterfaces = new CInterfaceProxy[pProxy->m_cInterfaces];
    if (!pInterfaces) {
        MARSHAL_DBGOUT(MSHDBG_ERROR,
                printf("Create proxy interfaces: out of memory.\n"));
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
        pInterfaces[n].m_pvProxyEntry = (PVoid)&CInterfaceProxy::ProxyEntry_RPC;
#elif defined(_arm)
        pInterfaces[n].m_pvProxyEntry = (PVoid)&__ProxyEntry_RPC;
#elif defined(_mips)
        pInterfaces[n].m_pvProxyEntry = (PVoid)&ProxyContinue;
#else
#error unknown architecture
#endif
    }

    ec = RegisterImportObject(pProxy->m_sNetAddress, (IProxy*)pProxy);
    if (FAILED(ec)) {
        MARSHAL_DBGOUT(MSHDBG_ERROR, printf(
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

} // namespace RPC
} // namespace Elastos
