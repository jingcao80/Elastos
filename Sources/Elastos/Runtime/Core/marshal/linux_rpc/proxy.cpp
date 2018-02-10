//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <prxstub.h>
#include <rot.h>
#include <DataBuffer.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

EXTERN_C const InterfaceID EIID_IProxy;

ECode RegisterModuleInfo(
    /* [in] */ CIModuleInfo* srcModuleInfo);

ECode LookupClassInfo(
    /* [in] */ REMuid rclsid,
    /* [out] */ CIClassInfo** classInfo);

ECode LookupModuleInfo(
    /* [in] */ REMuid rclsid,
    /* [out] */ CIModuleInfo** moduleInfo);

void *GetUnalignedPtr(void* ptr);

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
uintptr_t s_proxyEntryAddress = NULL;
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

#define SYS_PROXY_EXIT(ec, pret, argc)    \
    do {                                  \
        UInt32 n;                         \
        n = *(UInt32 *)(pret);            \
        n &= PROXY_ENTRY_MASK;            \
        n += PROXY_ENTRY_BASE + ((argc) << PROXY_ENTRY_SHIFT);  \
        *(UInt32 *)(pret) = n;            \
        return (ec);                      \
    } while (0)

#elif defined(_arm)

#define SYS_PROXY_EXIT(ec, pret, argc)  \
    do {                                \
        return (ec);                    \
    } while (0)

#elif defined(_aarch64)

#define SYS_PROXY_EXIT(ec, pret, argc)  \
    do {                                \
        return (ec);                    \
    } while (0)

#elif defined(_mips)

inline UInt32 CalcMethodIndex(UInt32 callerAddr)
{
    return (callerAddr - PROXY_ENTRY_BASE) >> PROXY_ENTRY_SHIFT;
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
#       define DECL_SYS_PROXY_ENTRY()              \
            __asm__(                               \
            ".text;"                               \
            ".align 4;"                            \
            ".globl ProxyEntryFunc_RPC;"           \
            "ProxyEntryFunc_RPC:"                  \
            "push {r0 - r3};"                      \
            "push {lr};"                           \
            "mov  r1, #0xff;"                      \
            "ldr  pc, [r0, #4];"                   \
        )
        DECL_SYS_PROXY_ENTRY();
#    elif defined(_aarch64)
#       define DECL_SYS_PROXY_ENTRY()              \
            __asm__(                               \
            ".text;"                               \
            ".align 8;"                            \
            ".globl ProxyEntryFunc_RPC;"           \
            "ProxyEntryFunc_RPC:"                  \
            "stp lr, x1, [sp, #-16];"              \
            "mov w1, #0xff;"                       \
            "ldr lr, [x0, #4];"                    \
            "br lr;"                               \
        )
        DECL_SYS_PROXY_ENTRY();
#    elif defined(_x86)
#       define DECL_SYS_PROXY_ENTRY()              \
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
            __declspec( naked ) void ProxyEntryFunc_RPC() \
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

EXTERN_C ECode GlobalProxyEntry_RPC(UInt32* args)
{
    return Elastos::RPC::CInterfaceProxy::ProxyEntry_RPC(args);
}

#ifdef _GNUC
#define DECL_PROXY_ENTRY()                 \
    __asm__(                               \
        ".text;"                           \
        ".align 4;"                        \
        ".globl __ProxyEntry_RPC;"         \
        "__ProxyEntry_RPC:"                \
        "push   {r1};"                     \
        "add    r0, sp, #4;"               \
        "bl     GlobalProxyEntry_RPC;"     \
        "add    sp, sp, #4;"               \
        "pop    {lr};"                     \
        "pop    {r1};"                     \
        "pop    {r1 - r3};"                \
        "push   {lr};"                     \
        "pop    {pc};"                     \
    )

DECL_PROXY_ENTRY();
#endif

#endif

#ifdef _aarch64

EXTERN_C void __ProxyEntry_RPC(void);

EXTERN_C ECode GlobalProxyEntry_RPC(UInt32* args)
{
    return Elastos::RPC::CInterfaceProxy::ProxyEntry_RPC(args);
}

#ifdef _GNUC
#define DECL_PROXY_ENTRY()                 \
    __asm__(                               \
        ".text;"                           \
        ".align 8;"                        \
        ".globl __ProxyEntry_RPC;"         \
        "__ProxyEntry_RPC:"                \
        "ldr lr, [sp, #-16];"              \
        "str x0, [sp, #-16];"              \
        "str lr, [sp, #-24];"              \
        "str w1, [sp, #-28];"              \
        "sub x0, sp, #24;"                 \
        "bl GlobalProxyEntry_RPC;"         \
        "ldr lr, [sp, #-16];"              \
        "ldr x1, [sp, #-8];"               \
        "ret;"                             \
    )

DECL_PROXY_ENTRY();
#endif

#endif

#ifdef _mips

EXTERN_C ECode GlobalProxyEntry_RPC(UInt32* args)
{
    return Elastos::RPC::CInterfaceProxy::ProxyEntry_RPC(args);
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
    s_proxyEntryAddress = reinterpret_cast<uintptr_t>(mmap((void*)0,
            PAGE_ALIGN(PROXY_ENTRY_NUM * PROXY_ENTRY_SIZE),
            PROT_READ | PROT_WRITE | PROT_EXEC,
            MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));
    if (s_proxyEntryAddress == 0) {
        printf("out of memory.\n");
        return;
    }

#if defined(_x86)
    char* p = (char *)s_proxyEntryAddress;
    for (Int32 n = 0; n < PROXY_ENTRY_NUM; n++) {
        memcpy(p, (void*)&ProxyEntryFunc_RPC, PROXY_ENTRY_SIZE);
        *(Int16*)&(p[SYS_PROXY_RET_OFFSET]) = ((n + 1) << 2);
        p+= PROXY_ENTRY_SIZE;
    }
#elif defined(_arm)
    char* p = (char *)s_proxyEntryAddress;
    for (Int32 n = 0; n < PROXY_ENTRY_NUM; n++) {
        memcpy(p, (void *)&ProxyEntryFunc_RPC, PROXY_ENTRY_SIZE);
        p[8] = n;
        p += PROXY_ENTRY_SIZE;
    }
#elif defined(_aarch64)
    char* p = reinterpret_cast<char *>(s_proxyEntryAddress);
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
    for (Int32 m = 4; m < MSH_MAX_METHODS; m++) {
        g_marshalVtbl[m] = PROXY_ENTRY_BASE + ((m - 4) << PROXY_ENTRY_SHIFT);
    }

    signal(SIGUSR2, siguserHandler);
}

void UninitProxyEntry()
{
     if (0 != s_proxyEntryAddress) {
        munmap(reinterpret_cast<void*>(s_proxyEntryAddress),
               PAGE_ALIGN(PROXY_ENTRY_NUM * PROXY_ENTRY_SIZE));
        s_proxyEntryAddress = NULL;
    }
}

ECode GetRemoteClassInfo(
    /* [in] */ const String& netAddress,
    /* [in] */ REMuid clsId,
    /* [out] */ CIClassInfo** classInfo)
{
    ECode ec;
    Int32 ciSize;
    CIModuleInfo* modInfo;
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

    if (classInfo == NULL) return E_INVALID_ARGUMENT;

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
    data->GetDataPayload((HANDLE*)&payload);
    data->GetDataSize(&payloadSize);
    dataBuf = new DataBuffer();
    totalSize = htonl(sizeof(totalSize) + payloadSize);
    dataBuf->Append((void*)&totalSize, sizeof(totalSize));
    dataBuf->Append(payload, payloadSize);
    assert((Int32)ntohl(totalSize) == dataBuf->DataSize());
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
    modInfo = (CIModuleInfo*)reply->ReadInplace(ciSize);
    ec = RegisterModuleInfo(modInfo);
    if (FAILED(ec)) {
        MARSHAL_DBGOUT(MSHDBG_ERROR,
            _DumpGUID(clsId));
        MARSHAL_DBGOUT(MSHDBG_ERROR,
                printf("Register module info failed.\n"));
        goto Exit;
    }

    if (FAILED(LookupModuleInfo(clsId, &modInfo))) {
        MARSHAL_DBGOUT(MSHDBG_ERROR,
            _DumpGUID(clsId));
        MARSHAL_DBGOUT(MSHDBG_ERROR,
                printf("Find module info failed.\n"));
        goto Exit;
    }

    ec = E_DOES_NOT_EXIST;
    for (Int32 m = 0; m < modInfo->mClassNum; m++) {
        if (modInfo->mClasses[m].mCLSID == clsId) {
            *classInfo = &(modInfo->mClasses[m]);

            MARSHAL_DBGOUT(MSHDBG_NORMAL, printf("iid: "));
            MARSHAL_DBGOUT(MSHDBG_NORMAL,
                    DUMP_GUID((*classInfo)->mCLSID));
            MARSHAL_DBGOUT(MSHDBG_NORMAL,
                    printf("UUNM: %s.\n", (*classInfo)->mUunm));
            MARSHAL_DBGOUT(MSHDBG_NORMAL,
                    printf("InterfaceNumber: %d.\n", (*classInfo)->mInterfaceNum));

            ec = NOERROR;
            goto Exit;
        }
    }

Exit:
    if (socketfd != -1) close(socketfd);
    return ec;
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

ECode CInterfaceProxy::BufferSize(
    /* [in] */ UInt32 methodIndex,
    /* [in] */ UInt32* args,
    /* [out] */ UInt32* inSize,
    /* [out] */ UInt32* outSize)
{
    const CIMethodInfo* methodInfo = &(mInfo->mMethods[methodIndex]);

    ECode ec = Proxy_ProcessMsh_BufferSize(
            methodInfo, args, inSize, outSize);
    if (0 != *outSize){
        *outSize += sizeof(MarshalHeader);
    }

    if (SUCCEEDED(ec)) {
        *inSize += sizeof(MarshalHeader);
    }
    return ec;
}

ECode CInterfaceProxy::MarshalIn(
    /* [in] */ UInt32 methodIndex,
    /* [in] */ UInt32* args,
    /* [in, out] */ CRemoteParcel* parcel)
{
    const CIMethodInfo* methodInfo = &(mInfo->mMethods[methodIndex]);

    ECode ec = Proxy_ProcessMsh_In(
            methodInfo, args, (IParcel*)parcel);

    if (SUCCEEDED(ec)) {
        MarshalHeader* header = parcel->GetMarshalHeader();
        assert(header != NULL);
        header->mMagic = MARSHAL_MAGIC;
        header->mInterfaceIndex = mIndex;
        header->mMethodIndex = methodIndex + 4;
    }

    return ec;
}

ECode CInterfaceProxy::UnmarshalOut(
    /* [in] */ UInt32 methodIndex,
    /* [out] */ CRemoteParcel* parcel,
    /* [in] */ UInt32* args)
{
    // TODO:
//    MarshalHeader *pHeader = parcel->GetMarshalHeader();
//
//    if (pHeader->mMagic != MARSHAL_MAGIC) {
//        MARSHAL_DBGOUT(MSHDBG_ERROR,
//                printf("Proxy unmsh: invalid magic(%x)\n", pHeader->mMagic));
//        return E_MARSHAL_DATA_TRANSPORT_ERROR;
//    }

#if defined(_DEBUG)
    // TODO:
//    if (pHeader->mInterfaceIndex != (Int16)mIndex) {
//        MARSHAL_DBGOUT(MSHDBG_ERROR,
//                printf("Proxy unmsh: invalid iidx(%x)\n",
//                pHeader->mInterfaceIndex));
//        return E_MARSHAL_DATA_TRANSPORT_ERROR;
//    }
//    if (pHeader->mMethodIndex != (Int16)(methodIndex + 4)) {
//        MARSHAL_DBGOUT(MSHDBG_ERROR, printf(
//                "Proxy unmsh: invalid method(%x)\n", pHeader->mMethodIndex));
//    }
#endif

    // TODO:
    return Proxy_ProcessUnmsh_Out(
            &(mInfo->mMethods[methodIndex]),
            (IParcel*)parcel,
            0/*pHeader->mOutSize - sizeof(MarshalHeader)*/,
            args);
}

UInt32 CInterfaceProxy::CountMethodArgs(
    /* [in] */ UInt32 methodIndex)
{
    return GET_LENGTH((mInfo->mMethods[methodIndex]).mReserved1);
}

Boolean CInterfaceProxy::MethodHasOutArgs(
    /* [in] */ UInt32 methodIndex)
{
    const CIMethodInfo* methodInfo = &(mInfo->mMethods[methodIndex]);
    Int32 paramNum = methodInfo->mParamNum;
    const CIBaseType* params = methodInfo->mParams;

    for (Int32 n = 0; n < paramNum; n++) {
        if (BT_IS_OUT(params[n])) return TRUE;
    }

    return FALSE;
}

Boolean CInterfaceProxy::IsMethodOneway(
    /* [in] */ UInt32 methodIndex)
{
    return BT_IS_ONEWAY((mInfo->mMethods[methodIndex]).mAttribs);
}

ECode CInterfaceProxy::ProxyEntry_RPC(
    /* [in] */ UInt32* args)
{
    CInterfaceProxy* thisPtr;
    UInt32 methodIndex, inSize, outSize;
    Int32 size, payloadSize, totalSize;
    Boolean isMethodOneway;
    MarshalHeader* inHeader = NULL;
    AutoPtr<CRemoteParcel> inParcel, outParcel;
    Int32 socketfd;
    struct sockaddr_in name;
    timeval recvTimeout = {60, 0};
    String netAddress, ipAddr;
    Int32 port;
    Int32 index;
    const Int32 tag = htonl(MSH_NOT_NULL);
    const char pad = '\0';
    const char* INVOKE = "INVOKE";
    const char* ONEWAY = "ONEWAY";
    UInt8* data = NULL;
    char buf[4096];
    AutoPtr<DataBuffer> dataBuf, replyBuf;
    ECode ec, orgec;
#ifndef _mips
    UInt32 argNum __attribute__((__unused__));
#endif


    args++; // skip ret address

    thisPtr = (CInterfaceProxy *)*args;
    args++; // skip this

    MARSHAL_DBGOUT(MSHDBG_NORMAL,
            printf("*args = %x, args = %x, ", *args, (UInt32)args));
    MARSHAL_DBGOUT(MSHDBG_NORMAL, printf("iid: "));
    MARSHAL_DBGOUT(MSHDBG_NORMAL, DUMP_GUID(thisPtr->mInfo->mIID));

#ifdef _x86
    methodIndex = CalcMethodIndex(*(UInt32 *)((UInt32)&args - 4));
#elif defined(_arm)
    methodIndex = args[-3];
#elif defined(_aarch64)
    methodIndex = args[-3];
#elif defined(_mips)
    methodIndex = CalcMethodIndex(*(args - 4) - 4);
#else
#error unknown architecture
#endif

#ifndef _mips
    // Get the stack length, not contain "this"
    argNum = thisPtr->CountMethodArgs(methodIndex);

    MARSHAL_DBGOUT(MSHDBG_NORMAL, printf(
            "Method index(%d), args size(%d)\n", methodIndex + 4, argNum * 4));
#endif

    // Calculate the package size
    //
    // NOTE:
    //  1. Alloc outHeader on the stack with MAX-out-size
    //  2. Assign inHeader->mOutSize with MAX-out-size
    //  3. Pass the MIN-out-size to SysInvoke's last parameter
    //  4. Call Thread::ReallocBuffer in SysReply if necessary to pass back the
    //      marshaled-out data with error info
    //
    ec = thisPtr->BufferSize(methodIndex, args, &inSize, &outSize);
    if (FAILED(ec)) {
        MARSHAL_DBGOUT(MSHDBG_ERROR,
                printf("Proxy BufferSize() failed, ec = %x\n", ec));
        goto ProxyExit;
    }

    MARSHAL_DBGOUT(MSHDBG_NORMAL, printf(
            "Buffer size: inSize(%d), outSize(%d)\n", inSize, outSize));
    assert(inSize >= sizeof(MarshalHeader));

    inParcel = new CRemoteParcel();
    ec = thisPtr->MarshalIn(methodIndex, args, inParcel);

    if (SUCCEEDED(ec)) {
        if (thisPtr->mOwner->mSocketfd == -1) {
            socketfd = socket(PF_INET, SOCK_STREAM, 0);
            if (socketfd < 0) {
                MARSHAL_DBGOUT(MSHDBG_ERROR,
                        printf("ProxyEntry_RPC: create socket failed.\n"));
                ec = E_FAIL;
                goto ProxyExit;
            }

            netAddress = thisPtr->mOwner->mNetAddress;
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

            if (setsockopt(socketfd, SOL_SOCKET, SO_RCVTIMEO, &recvTimeout, sizeof(recvTimeout)) == -1) {
                MARSHAL_DBGOUT(MSHDBG_ERROR,
                        printf("ProxyEntry_RPC: set so_rcvtimeo failed.\n"));
                ec = E_FAIL;
                goto ProxyExit;
            }

            thisPtr->mOwner->mSocketfd = socketfd;

            if (thisPtr->mOwner->mPingTask == NULL) {
                AutoPtr<CObjectProxy::PingRunnable> r = new CObjectProxy::PingRunnable(thisPtr->mOwner);
                ThreadPoolExecutor::Self()->RunTask(r);
                thisPtr->mOwner->mPingTask = r;
            }
        }

        isMethodOneway = thisPtr->IsMethodOneway(methodIndex);
        dataBuf = new DataBuffer();
        inParcel->GetDataSize(&payloadSize);
        inParcel->GetDataPayload((HANDLE*)&data);
        inHeader = inParcel->GetMarshalHeader();
        inHeader->mInSize = payloadSize;
        inHeader->mOutSize = outSize;
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
        dataBuf->Append(data, payloadSize);
        MARSHAL_DBGOUT(MSHDBG_NORMAL, printf(
                "Before RemoteInvoke: ParcelSize(%d)\n", payloadSize));
        assert((Int32)ntohl(totalSize) == dataBuf->DataSize());

        {
            android::AutoMutex _l(thisPtr->mOwner->mLock);

            if (send(thisPtr->mOwner->mSocketfd, dataBuf->Data(), dataBuf->DataSize(), 0) == -1) {
                MARSHAL_DBGOUT(MSHDBG_ERROR,
                        printf("RemoteInvoke: send socket failed.\n"));
                ec = E_FAIL;
                goto ProxyExit;
            }

            replyBuf = new DataBuffer();
            size = recv(thisPtr->mOwner->mSocketfd, (void*)buf, sizeof(buf), 0);
            if (size <= 0) {
                MARSHAL_DBGOUT(MSHDBG_ERROR,
                        printf("RemoteInvoke: remote socket broken.\n"));
                thisPtr->mOwner->StubDied();
                ec = E_FAIL;
                goto ProxyExit;
            }
            totalSize = ntohl(*(Int32*)buf);
            replyBuf->Append(buf + sizeof(Int32), size - sizeof(Int32));
            while((totalSize = totalSize - size) > 0) {
                size = recv(thisPtr->mOwner->mSocketfd, (void*)buf, sizeof(buf), 0);
                if (size <= 0) {
                    MARSHAL_DBGOUT(MSHDBG_ERROR,
                            printf("RemoteInvoke: remote socket broken.\n"));
                    thisPtr->mOwner->StubDied();
                    ec = E_FAIL;
                    goto ProxyExit;
                }
                replyBuf->Append(buf, size);
            }
        }

        if (!thisPtr->IsMethodOneway(methodIndex)) {
            outParcel = new CRemoteParcel(FALSE);
            outParcel->SetData((UInt8*)replyBuf->Data(), replyBuf->DataSize());
            outParcel->ReadInt32(&ec);
            if (SUCCEEDED(ec)) {
                if (thisPtr->MethodHasOutArgs(methodIndex)) {
                    if (outSize) {
                        orgec = thisPtr->UnmarshalOut(methodIndex,
                                                    outParcel,
                                                    args);
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
    SYS_PROXY_EXIT(ec, &args - 1, argNum);
#else
    SYS_PROXY_EXIT(ec, &args - 1, 0);
#endif
}

DeathRecipientList::DeathRecipientList()
{
    MARSHAL_DBGOUT(MSHDBG_NORMAL, printf("New DRL @ %p", this));
}

DeathRecipientList::~DeathRecipientList()
{
    MARSHAL_DBGOUT(MSHDBG_NORMAL, printf("Destroy DRL @ %p", this));
    android::AutoMutex _l(mLock);

    // Should never happen -- the JavaDeathRecipient objects that have added themselves
    // to the list are holding references on the list object.  Only when they are torn
    // down can the list header be destroyed.
    if (mDeathRecipients.size() > 0) {
        android::List< AutoPtr<CarDeathRecipient> >::iterator iter;
        for (iter = mDeathRecipients.begin(); iter != mDeathRecipients.end(); iter++) {
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
    android::AutoMutex _l(mLock);

    MARSHAL_DBGOUT(MSHDBG_NORMAL, printf("DRL @ %p : add CDR %p", this, recipient.Get()));
    mDeathRecipients.push_back(recipient);
}

void DeathRecipientList::Remove(
    /* [in] */ const AutoPtr<CarDeathRecipient>& recipient)
{
    android::AutoMutex _l(mLock);

    android::List< AutoPtr<CarDeathRecipient> >::iterator iter;
    for (iter = mDeathRecipients.begin(); iter != mDeathRecipients.end(); iter++) {
        if (*iter == recipient) {
            MARSHAL_DBGOUT(MSHDBG_NORMAL, printf("DRL @ %p : remove CDR %p", this, recipient.Get()));
            mDeathRecipients.erase(iter);
            return;
        }
    }
}

AutoPtr<CarDeathRecipient> DeathRecipientList::Find(
    /* [in] */ IProxyDeathRecipient* recipient)
{
    android::AutoMutex _l(mLock);

    android::List< AutoPtr<CarDeathRecipient> >::iterator iter;
    for (iter = mDeathRecipients.begin(); iter != mDeathRecipients.end(); iter++) {
        if ((*iter)->Matches(recipient)) {
            return *iter;
        }
    }
    return NULL;
}

void DeathRecipientList::NotifyAll(
    /* [in] */ CObjectProxy* proxy)
{
    android::AutoMutex _l(mLock);

    android::List< AutoPtr<CarDeathRecipient> >::iterator iter = mDeathRecipients.begin();
    while (iter != mDeathRecipients.end()) {
        (*iter)->StubDied(proxy);
        iter = mDeathRecipients.erase(iter);
    }
}

CarDeathRecipient::CarDeathRecipient(
    /* [in] */ IProxyDeathRecipient* recipient,
    /* [in] */ const AutoPtr<DeathRecipientList>& list)
    : mObject(recipient)
{
    if (mObject != NULL) {
        mObject->AddRef();
    }

    AutoPtr<IWeakReference> wr;
    IWeakReferenceSource::Probe(list)->GetWeakReference((IWeakReference**)&wr);
    // These objects manage their own lifetimes so are responsible for final bookkeeping.
    // The list holds a strong reference to this object.
    mDeathRecipients = wr;
}

CarDeathRecipient::~CarDeathRecipient()
{
    if (mObject != NULL) {
        mObject->Release();
        mObject = NULL;
    }
}

void CarDeathRecipient::StubDied(
    /* [in] */ const CObjectProxy* who)
{
    MARSHAL_DBGOUT(MSHDBG_NORMAL, printf("Receiving StubDied() on CarDeathRecipient %p\n", this));
    if (mObject != NULL) {
        ECode ec = mObject->ProxyDied();
        if (FAILED(ec)) {
            MARSHAL_DBGOUT(MSHDBG_ERROR,
                    printf("*** Uncaught exception returned from death notification!"));
        }

        // Demote from strong ref to weak after StubDied() has been delivered,
        // to allow the DeathRecipient and BinderProxy to be GC'd if no longer needed.
        mObject->Release();
        mObject = NULL;
    }
}

void CarDeathRecipient::ClearReference()
{
    AutoPtr<IInterface> obj;
    mDeathRecipients->Resolve(EIID_IInterface, (IInterface**)&obj);
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

    if (mObject == NULL || recipient == NULL) result = FALSE;
    else result = mObject == recipient;

    return result;
}

void CarDeathRecipient::WarnIfStillLive()
{
    if (mObject != NULL) {
        MARSHAL_DBGOUT(MSHDBG_NORMAL,
                printf("BinderProxy is being destroyed but the application did not call "
                        "unlinkToDeath to unlink all of its death recipients beforehand.  "));
    }
}

ECode CObjectProxy::PingRunnable::Run()
{
    const Int32 tag = htonl(MSH_NOT_NULL);
    const char* PING = "PING";
    Int32 totalSize = htonl(sizeof(totalSize) + sizeof(tag) + strlen(PING) + 1);
    AutoPtr<DataBuffer> dataBuf = new DataBuffer();
    dataBuf->Append((void*)&totalSize, sizeof(totalSize));
    dataBuf->Append((void*)&tag, sizeof(tag));
    dataBuf->Append(PING, strlen(PING) + 1);

    while (TRUE) {
        sleep(30);

        android::AutoMutex _l(mHost->mLock);

        if (send(mHost->mSocketfd, dataBuf->Data(), dataBuf->DataSize(), 0) == -1) {
            MARSHAL_DBGOUT(MSHDBG_ERROR,
                    printf("PingRunnable: send socket failed.\n"));
        }

        Byte buf[32];
        Int32 size = recv(mHost->mSocketfd, (void*)buf, sizeof(buf), 0);
        if (size <= 0) {
            MARSHAL_DBGOUT(MSHDBG_ERROR,
                    printf("PingRunnable: recv socket broken.\n"));
            break;
        }
    }

    return mHost->StubDied();
}

CObjectProxy::CObjectProxy()
    : mSocketfd(-1)
    , mInterfaces(NULL)
    , mCallbackConnector(NULL)
    , mStubDied(FALSE)
    , mRef(1)
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
    ThreadPoolExecutor::Self()->StopTask(mPingTask);
}

static const EMuid ECLSID_XOR_CallbackSink = \
/* e724df56-e16a-4599-8edd-a97ab245d583 */
{0xe724df56,0xe16a,0x4599,{0x8e,0xdd,0xa9,0x7a,0xb2,0x45,0xd5,0x83}};

PInterface CObjectProxy::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (IInterface*)(IProxy*)this;
    }
    else if (riid == EIID_IProxy) {
        return (IProxy*)this;
    }

    if (riid == EIID_CALLBACK_CONNECTOR) {
        if (NULL == mCallbackConnector) {
            ClassID ezclsid;
            IInterface *pTemp = (IInterface *)&(mInterfaces[0].mVTPtr);
            this->GetClassID(&ezclsid.mClsid);

            ezclsid.mClsid.mData1    ^= ECLSID_XOR_CallbackSink.mData1;
            ezclsid.mClsid.mData2    ^= ECLSID_XOR_CallbackSink.mData2;
            ezclsid.mClsid.mData3    ^= ECLSID_XOR_CallbackSink.mData3;
            ezclsid.mClsid.mData4[0] ^= ECLSID_XOR_CallbackSink.mData4[0];
            ezclsid.mClsid.mData4[1] ^= ECLSID_XOR_CallbackSink.mData4[1];
            ezclsid.mClsid.mData4[2] ^= ECLSID_XOR_CallbackSink.mData4[2];
            ezclsid.mClsid.mData4[3] ^= ECLSID_XOR_CallbackSink.mData4[3];
            ezclsid.mClsid.mData4[4] ^= ECLSID_XOR_CallbackSink.mData4[4];
            ezclsid.mClsid.mData4[5] ^= ECLSID_XOR_CallbackSink.mData4[5];
            ezclsid.mClsid.mData4[6] ^= ECLSID_XOR_CallbackSink.mData4[6];
            ezclsid.mClsid.mData4[7] ^= ECLSID_XOR_CallbackSink.mData4[7];
            ezclsid.mUunm = (char*)alloca(sizeof(char) \
                    * (strlen(((CIClassInfo*)mInfo)->mUunm) + 1));
            strcpy(ezclsid.mUunm, ((CIClassInfo*)mInfo)->mUunm);
            _CObject_AcquireClassFactory(ezclsid, RGM_SAME_DOMAIN, EIID_CALLBACK_CONNECTOR, &pTemp);
            mCallbackConnector = (ICallbackConnector*)pTemp;
        }
        return (IInterface*)mCallbackConnector;
    }

    Int32 n;
    for (n = 0; n < mInterfaceNum; n++) {
        if (riid == mInterfaces[n].mInfo->mIID) {
            break;
        }
    }
    if (n == mInterfaceNum) {
        MARSHAL_DBGOUT(MSHDBG_WARNING, printf(
                "Proxy: QI failed, iid: "));
        MARSHAL_DBGOUT(MSHDBG_WARNING, DUMP_GUID(riid));

        return NULL;
    }

    return (IInterface *)&(mInterfaces[n].mVTPtr);
}

UInt32 CObjectProxy::AddRef()
{
    Int32 lRefs = atomic_inc(&mRef);

    MARSHAL_DBGOUT(MSHDBG_CREF, printf(
            "Proxy AddRef: %d\n", lRefs));
    return (UInt32)lRefs;
}

UInt32 CObjectProxy::Release()
{
    Int32 lRefs = atomic_dec(&mRef);
    if (lRefs == 0) {
        MARSHAL_DBGOUT(MSHDBG_NORMAL, printf(
                "Proxy destructed.\n"));

        ECode ec = UnregisterImportObject(mNetAddress);
        if (ec == S_FALSE) {
            return 1;// other thread hold the proxy
        }

        if (mCallbackConnector) {
            mCallbackConnector->DisconnectCallbackSink();
            mCallbackConnector->Release();
            mCallbackConnector = NULL;
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
        assert((Int32)ntohl(totalSize) == dataBuf->DataSize());
        if (send(mSocketfd, dataBuf->Data(), dataBuf->DataSize(), 0) == -1) {
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
    /* [in] */ IInterface* object,
    /* [out] */ InterfaceID* iid)
{
    if (NULL == iid) return E_INVALID_ARGUMENT;

    if (object == (IInterface *)(IProxy *)this) {
        *iid = EIID_IProxy;
    }
    else {
        return E_INVALID_ARGUMENT;
    }
    return NOERROR;
}

ECode CObjectProxy::GetInterfaceCount(
    /* [out] */ Int32* count)
{
    if (count == NULL) return E_INVALID_ARGUMENT;

    *count = mInterfaceNum;
    return NOERROR;
}

ECode CObjectProxy::GetAllInterfaceInfos(
    /* [out] */ ArrayOf<IInterfaceInfo*>* interfaceInfos)
{
    if (interfaceInfos == NULL) return E_INVALID_ARGUMENT;

    return NOERROR;
}

ECode CObjectProxy::GetInterface(
    /* [in] */ UInt32 index,
    /* [out] */ IInterface** object)
{
    assert(object != NULL);

    if (index < (UInt32)mInterfaceNum) {
        *object = (IInterface *)&(mInterfaces[index].mVTPtr);
        this->AddRef();
        return NOERROR;
    }
    MARSHAL_DBGOUT(MSHDBG_WARNING, printf(
            "Proxy: IndexQI failed - idx(%d), mInterfaceCount(%d)\n",
            index, mInterfaceNum));

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
    assert(0);
    return NOERROR;
}

ECode CObjectProxy::LinkToDeath(
    /* [in] */ IProxyDeathRecipient* recipient,
    /* [in] */ Int32 flags)
{
    if (recipient == NULL) {
        return E_INVALID_ARGUMENT;
    }

    if (mStubDied) {
        MARSHAL_DBGOUT(MSHDBG_ERROR,
                printf("Remote stub is died when calling linkToDeath() with recip=%p)\n", recipient));
        return E_FAIL;
    }

    MARSHAL_DBGOUT(MSHDBG_NORMAL,
                printf("linkToDeath: recipient=%p\n", recipient));

    if (mOrgue == NULL) mOrgue = new DeathRecipientList();
    AutoPtr<CarDeathRecipient> cdr = new CarDeathRecipient(recipient, mOrgue);
    MARSHAL_DBGOUT(MSHDBG_NORMAL, printf("Adding CDR %p to DRL %p", cdr.Get(), mOrgue.Get()));
    mOrgue->Add(cdr);

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
        return E_INVALID_ARGUMENT;
    }

    if (mStubDied) {
        MARSHAL_DBGOUT(MSHDBG_ERROR,
                printf("Remote stub is died when calling linkToDeath() with recip=%p)\n", recipient));
        return E_FAIL;
    }

    MARSHAL_DBGOUT(MSHDBG_NORMAL,
                printf("unlinkToDeath: recipient=%p\n", recipient));

    // If we find the matching recipient, proceed to unlink using that
    AutoPtr<CarDeathRecipient> origCDR = mOrgue->Find(recipient);
    MARSHAL_DBGOUT(MSHDBG_NORMAL,
            printf("   unlink found list %p and CDR %p", mOrgue.Get(), origCDR.Get()));
    if (origCDR != NULL) {
        origCDR->ClearReference();
    }

    return NOERROR;
}

ECode CObjectProxy::StubDied()
{
    mOrgue->NotifyAll(this);
    return NOERROR;
}

ECode CObjectProxy::S_CreateObject(
    /* [in] */ REMuid rclsid,
    /* [in] */ const String& address,
    /* [out] */ IProxy** proxy)
{
    CObjectProxy* proxyObj;
    CInterfaceProxy* interfaces;
    Int32 n;

    if (proxy == NULL) return E_INVALID_ARGUMENT;

    proxyObj = new CObjectProxy();
    if (!proxyObj) {
        MARSHAL_DBGOUT(MSHDBG_ERROR,
                printf("Create proxy object: out of memory.\n"));
        return E_OUT_OF_MEMORY;
    }

    proxyObj->mNetAddress = address;

    ECode ec = LookupClassInfo(rclsid, &(proxyObj->mInfo));
    if (FAILED(ec)) {
        ec = GetRemoteClassInfo(proxyObj->mNetAddress,
                                rclsid,
                                &proxyObj->mInfo);
        if (FAILED(ec)) goto ErrorExit;
    }

    proxyObj->mInterfaceNum = ((CIClassInfo*)(proxyObj->mInfo))->mInterfaceNum;
    interfaces = new CInterfaceProxy[proxyObj->mInterfaceNum];
    if (!interfaces) {
        MARSHAL_DBGOUT(MSHDBG_ERROR,
                printf("Create proxy interfaces: out of memory.\n"));
        ec = E_OUT_OF_MEMORY;
        goto ErrorExit;
    }
    proxyObj->mInterfaces = interfaces;
    memset(interfaces, 0, sizeof(CInterfaceProxy) * proxyObj->mInterfaceNum);
    for (n = 0; n < proxyObj->mInterfaceNum; n++) {
        interfaces[n].mIndex = n;
        interfaces[n].mOwner = proxyObj;
        CIInterfaceInfo* interfaceInfo =
            (CIInterfaceInfo *)GetUnalignedPtr(
                    proxyObj->mInfo->mInterfaces + n);
        interfaces[n].mInfo = interfaceInfo;
        interfaces[n].mVTPtr = g_marshalVtbl;

#ifdef _x86
        interfaces[n].mProxyEntry = (PVoid)&CInterfaceProxy::ProxyEntry_RPC;
#elif defined(_arm)
        interfaces[n].mProxyEntry = (PVoid)&__ProxyEntry_RPC;
#elif defined(_aarch64)
        interfaces[n].mProxyEntry = (PVoid)&__ProxyEntry_RPC;
#elif defined(_mips)
        interfaces[n].mProxyEntry = (PVoid)&ProxyContinue;
#else
#error unknown architecture
#endif
    }

    ec = RegisterImportObject(proxyObj->mNetAddress, (IProxy*)proxyObj);
    if (FAILED(ec)) {
        MARSHAL_DBGOUT(MSHDBG_ERROR, printf(
                "Create proxy: register import object failed, ec(%x)\n",
                ec));
        goto ErrorExit;
    }

    *proxy = (IProxy*)proxyObj;

    return NOERROR;

ErrorExit:
    delete proxyObj;
    return ec;
}

} // namespace RPC
} // namespace Elastos
