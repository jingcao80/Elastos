
#include <elaatomics.h>
#include "prxstub.h"
#include "rot.h"
#include "looper.h"
#include "DataBuffer.h"
#include <cutils/log.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>

#ifdef _x86

#ifdef _GNUC

#define GET_REG(reg, v)                 \
    ASM("mov    %%"#reg", %0;"          \
        :"=m"(v)                        \
    )

#define SET_REG(reg, v)                 \
    ASM("mov    %0, %%"#reg";"          \
        ::"m"(v)                        \
    )

#define STUB_INVOKE_METHOD(ec, argstart, addr) \
    ASM("call   *%2;"                   \
        "mov    %%eax, %0;"             \
        "mov    %1, %%esp;"             \
        :"=m"(ec)                       \
        :"m"(argstart), "m"(addr)       \
    )

#else

#define GET_REG(reg, v)                 \
    __asm mov v, reg;

#define SET_REG(reg, v)                 \
    __asm mov reg, v;

#define STUB_INVOKE_METHOD(ec, argstart, addr)    \
    do {                                          \
        __asm call addr                      \
        __asm mov ec, eax                         \
        __asm mov esp, argstart                   \
    } while(0)

#endif

#elif defined(_arm)

#define GET_REG(reg, v)                 \
    ASM("str    "#reg", %0;"            \
        :"=m"(v))

#define SET_REG(reg, v)                 \
    ASM("ldr    "#reg", %0;"            \
        : :"m"(v))

#define STUB_INVOKE_METHOD1(ec, puArgs, addr) \
    ASM("mov    lr, %1;"                \
        "ldr    r0, [lr];"              \
        "ldr    r1, [lr, #4];"          \
        "ldr    r2, [lr, #8];"          \
        "ldr    r3, [lr, #12];"         \
        "adr    lr, __StubInvokeNext1;" \
        "ldr    pc, %2;"                \
        "__StubInvokeNext1:"            \
        "str    r0, %0;"                \
        :"=m"(ec) :"r"(puArgs), "m"(addr))

#define STUB_INVOKE_METHOD2(ec, puArgs, addr) \
    ASM("mov    lr, %1;"                \
        "ldr    r0, [lr];"              \
        "ldr    r1, [lr, #4];"          \
        "ldr    r2, [lr, #8];"          \
        "ldr    r3, [lr, #12];"         \
        "add    sp, lr, #16;"           \
        "adr    lr, __StubInvokeNext2;" \
        "ldr    pc, %2;"                \
        "__StubInvokeNext2:"            \
        "str    r0, %0;"                \
        :"=m"(ec) :"r"(puArgs), "m"(addr))

#elif defined(_mips)

#ifdef _GNUC

#define GET_REG(reg, v)                 \
    ASM("sw\t"#reg", %0;":"=m"(v));

#define SET_REG(reg, v)                 \
    ASM("lw "#reg", %0;"::"m"(v));

#elif defined(_EVC)

#define GET_REG(reg, v)                 \
    ASM("sw "#reg", 0(%0);", &v)

#define SET_REG(reg, v)                 \
    ASM("move "#reg", %0;", v)

#define STUB_INVOKE_METHOD(ec, puArgs, addr)    \
    ASM(                                        \
        "addiu  sp, sp, -0x20;"                 \
        "sw     s0, 0x10(sp);"                  \
        "sw     s1, 0x14(sp);"                  \
        "sw     s2, 0x18(sp);"                  \
        "or     s0, a0, zero;"                  \
        "or     s1, sp, zero;"                  \
        "or     s2, a2, zero;"                  \
        "or     sp, a1, zero;"                  \
        "lw     a0, 0x0(sp);"                   \
        "lw     a1, 0x4(sp);"                   \
        "lw     a2, 0x8(sp);"                   \
        "lw     a3, 0xc(sp);"                   \
        "jal    s2;"                            \
        "sw     v0, 0(s0);"                     \
        "or     sp, s1, zero;"                  \
        "lw     s0, 0x10(sp);"                  \
        "lw     s1, 0x14(sp);"                  \
        "lw     s2, 0x18(sp);"                  \
        "addiu  sp, sp, 0x20;"                  \
        , &ec, puArgs, addr)
#else
#error unknown architecture
#endif
#endif

#define DUMP_ITFID(intf) \
    do { \
        InterfaceID iid; \
        intf->GetInterfaceID(intf, &iid); \
        printf("======== DUMP_ITFID ========\n"); \
        printf("{%p, %p, %p, {%p, %p, %p, %p, %p, %p, %p, %p} }\n", \
                iid.Data1, iid.Data2, iid.Data3, \
                iid.Data4[0], iid.Data4[1], \
                iid.Data4[2], iid.Data4[3], \
                iid.Data4[4], iid.Data4[5], \
                iid.Data4[6], iid.Data4[7]); \
        printf("============================\n"); \
        ALOGD("======== DUMP_ITFID ========\n"); \
        ALOGD("{%p, %p, %p, {%p, %p, %p, %p, %p, %p, %p, %p} }\n", \
                iid.Data1, iid.Data2, iid.Data3, \
                iid.Data4[0], iid.Data4[1], \
                iid.Data4[2], iid.Data4[3], \
                iid.Data4[4], iid.Data4[5], \
                iid.Data4[6], iid.Data4[7]); \
        ALOGD("============================\n"); \
    } while(0);


ECode LookupModuleInfo(
    /* [in] */ REMuid rclsid,
    /* [out] */ CIModuleInfo **ppModuleInfo);

ECode LookupClassInfo(
    /* [in] */ REMuid rclsid,
    /* [out] */ CIClassInfo **ppClassInfo);

ECode AcquireClassInfo(
    /* [in] */ const ClassID & classId,
    /* [out] */ CIClassInfo **ppClassInfo);

void FlatModuleInfo(
    /* [in] */ CIModuleInfo *pSrcModInfo,
    /* [out] */ CIModuleInfo *pDestModInfo);

void *GetUnalignedPtr(void *pPtr);

namespace Elastos {
namespace RPC {

ECode CInterfaceStub::UnmarshalIn(
    /* [in] */ const CIMethodInfo *pMethodInfo,
    /* [in] */ CRemoteParcel *pParcel,
    /* [in, out] */ MarshalHeader *pOutHeader,
    /* [in, out] */ UInt32 *puArgs)
{
    return Stub_ProcessUnmsh_In(
            pMethodInfo,
            (IParcel*)pParcel,
            (UInt32 *)pOutHeader,
            puArgs);
}

ECode CInterfaceStub::MarshalOut(
    /* [in] */ UInt32 *puArgs,
    /* [in] */ MarshalHeader *pInHeader,
    /* [out] */ MarshalHeader *pOutHeader,
    /* [in] */ Boolean bOnlyReleaseIn,
    /* [in, out] */ CRemoteParcel* pParcel)
{
    MarshalHeader *pHeader;
    ECode ec;

    ec = Stub_ProcessMsh_Out(
            &(m_pInfo->methods[pInHeader->m_hMethodIndex - 4]),
            puArgs,
            (UInt32 *)pOutHeader,
            bOnlyReleaseIn,
            (IParcel*)pParcel);

    if (pParcel && SUCCEEDED(ec)) {
        // TODO:
//        pHeader = pParcel->GetMarshalHeader();
//        assert(pHeader != NULL);
//        pHeader->m_uMagic = pInHeader->m_uMagic;
//        pHeader->m_hInterfaceIndex = pInHeader->m_hInterfaceIndex ;
//        pHeader->m_hMethodIndex = pInHeader->m_hMethodIndex;
    }

    return ec;
}

// Boolean CObjectStub::s_bEPoolStarted = FALSE;
// pthread_mutex_t CObjectStub::s_bEPoolStartedMutex = PTHREAD_MUTEX_INITIALIZER;

CObjectStub::CObjectStub()
    : m_pInterfaces(NULL)
    , m_bRequestToQuit(FALSE)
    , m_cSocketfd(-1)
    , m_cConnfd(-1)
{
}

CObjectStub::~CObjectStub()
{
    if (m_pInterfaces) {
        delete [] m_pInterfaces;
    }
    if (m_cConnfd != -1) {
        Looper::GetLooper()->RemoveFd(m_cConnfd);
        close(m_cConnfd);
    }
    if (m_cSocketfd != -1) {
        Looper::GetLooper()->RemoveFd(m_cSocketfd);
        close(m_cSocketfd);
    }
}

PInterface CObjectStub::Probe(
    /* [in] */ REIID riid)
{
    Int32 n;

    if (riid == EIID_IInterface) {
        return (IInterface*)(IStub*)this;
    }

    for (n = 0; n < m_cInterfaces; n++) {
        if (riid == m_pInterfaces[n].m_pInfo->iid) {
            break;
        }
    }
    if (n == m_cInterfaces) {
        MARSHAL_DBGOUT(MSHDBG_WARNING, printf(
                "Stub: Probe failed, iid: "));
        MARSHAL_DBGOUT(MSHDBG_WARNING, DUMP_GUID(riid));

        return NULL;
    }

    return m_pInterfaces[n].m_pObject;
}

UInt32 CObjectStub::AddRef(void)
{
    Int32 lRefs = ElLightRefBase::AddRef();

    MARSHAL_DBGOUT(MSHDBG_CREF, printf(
            "Stub AddRef: %d\n", lRefs));
    return (UInt32)lRefs;
}

UInt32 CObjectStub::Release(void)
{
    UInt32 lRefs = ElLightRefBase::Release();
    if (lRefs == 0) {
        MARSHAL_DBGOUT(MSHDBG_NORMAL, printf(
                "Stub destructed.\n"));
        return 0;
    }

    MARSHAL_DBGOUT(MSHDBG_CREF, printf(
            "Stub Release: %d\n", lRefs));
    return lRefs;
}

ECode CObjectStub::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    return E_NOT_IMPLEMENTED;
}

// void CObjectStub::onLastStrongRef(
//     /* [in] */ const void* id)
// {
//     UnregisterExportObject(/*(android::IBinder*)this*/);
// }

ECode CObjectStub::Invoke(
    /* [in] */ CRemoteParcel* data,
    /* [out] */ CRemoteParcel* reply)
{
    MarshalHeader *pInHeader = NULL, *pOutHeader = NULL;
    UInt32 *puArgs, uMethodAddr, uMethodIndex;
    ECode ec, orgec = NOERROR;
    CInterfaceStub *pCurInterface;
    const CIMethodInfo *pMethodInfo;
    CRemoteParcel *pParcel = NULL;
    CRemoteParcel* pReplyParcel = NULL;
    UInt32 uInSize;
#ifdef _x86
    UInt32 uEAX, uEDX, uECX, uESP;
#elif defined(_arm)
    struct ArgumentBuffer
    {
        UInt32    r0;
        UInt32    r1;
        UInt32    r2;
        UInt32    r3;
    } argbuf;
#if defined(_GNUC)
    UInt32 r0, r1, r2, r3, ip, lr, sp;
#else
#error Unknown C++ compiler
#endif // defined(_GNUC)
#elif defined(_mips)
    UInt32 uV0, uV1;
    UInt32 uA0, uA1, uA2, uA3;
    UInt32 uT0, uT1, uT2, uT3, uT4, uT5, uT6, uT7, uT8, uT9;
    UInt32 uSp;
#else
#error unknown architecture
#endif
    Boolean bOnlyReleaseIn = FALSE;

    MARSHAL_DBGOUT(MSHDBG_NORMAL,
            printf("Stub: in Invoke.\n"));

    pInHeader = (MarshalHeader*)data->ReadInplace(sizeof(*pInHeader));
    if (pInHeader == NULL) {
        MARSHAL_DBGOUT(MSHDBG_ERROR,
                printf("Stub error: read MarshalHeader failed.\n"));
        goto ErrorExit;
    }
    if (pInHeader->m_uMagic != MARSHAL_MAGIC) {
        MARSHAL_DBGOUT(MSHDBG_ERROR,
                printf("Stub: invalid magic - %x\n", pInHeader->m_uMagic));
        goto ErrorExit;
    }

#if defined(_DEBUG)
    if (pInHeader->m_uInSize < sizeof(MarshalHeader)) {
        goto ErrorExit;
    }
    if (pInHeader->m_hInterfaceIndex >= m_cInterfaces) {
        MARSHAL_DBGOUT(MSHDBG_ERROR,
                printf("Stub: interface index error - %d:%d\n",
                pInHeader->m_hInterfaceIndex, m_cInterfaces));
        goto ErrorExit;
    }
    MARSHAL_DBGOUT(MSHDBG_NORMAL,
            printf("Stub: interface idx(%d), method idx(%d)\n",
            pInHeader->m_hInterfaceIndex, pInHeader->m_hMethodIndex));
#endif

    if (pInHeader->m_hMethodIndex >= 4) {
        pCurInterface = &(m_pInterfaces[pInHeader->m_hInterfaceIndex]);

        // Uncomment the follow line to enable IPC invoke log.
        // PrintIpcLog(pInHeader);
        uMethodIndex = pInHeader->m_hMethodIndex - 4;
        if (uMethodIndex >= pCurInterface->m_pInfo->methodNumMinus4) {
            MARSHAL_DBGOUT(MSHDBG_ERROR,
                    printf("Stub: method index out of range - %d:%d\n",
                    uMethodIndex,
                    pCurInterface->m_pInfo->methodNumMinus4));
            goto ErrorExit;
        }

        uMethodAddr =
                pCurInterface->m_pInterface->m_vPtr->m_vTable[uMethodIndex + 4];

        if (0 != pInHeader->m_uOutSize) {
            pOutHeader = (MarshalHeader *)alloca(pInHeader->m_uOutSize);
            if (!pOutHeader) {
                MARSHAL_DBGOUT(MSHDBG_ERROR,
                        printf("Stub error: alloca() failed.\n"));
                ec = E_OUT_OF_MEMORY;
                goto ErrorExit;
            }
        }

        pMethodInfo = &((pCurInterface->m_pInfo)->methods[uMethodIndex]);
        uInSize = GET_LENGTH(pMethodInfo->reserved1) * 4 + 4;
        MARSHAL_DBGOUT(MSHDBG_NORMAL,
                printf("Stub: method args stack size (%d)\n", uInSize));
        puArgs = (UInt32 *)alloca(uInSize);
        if (!puArgs) {
            MARSHAL_DBGOUT(MSHDBG_ERROR,
                    printf("Stub error: alloca() failed.\n"));
            ec = E_OUT_OF_MEMORY;
            goto ErrorExit;
        }
#ifdef _x86
        GET_REG(ESP, puArgs);
#endif

        pParcel = data;
        if (!pParcel) {
            MARSHAL_DBGOUT(MSHDBG_ERROR,
                    printf("Stub error: new CRemoteParcel failed.\n"));
            ec = E_OUT_OF_MEMORY;
            goto ErrorExit;
        }
        ec = pCurInterface->UnmarshalIn(pMethodInfo,
                                        pParcel,
                                        pOutHeader,
                                        puArgs + 1);
        if (FAILED(ec)) {
            MARSHAL_DBGOUT(MSHDBG_ERROR,
                    printf("Stub error: UnmarshalIn() failed.\n"));
            goto ErrorExit;
        }

        *puArgs = (UInt32)pCurInterface->m_pObject; // fill this

        MARSHAL_DBGOUT(MSHDBG_NORMAL,
                printf("Stub: invoke method - args(%x), addr(%x) \n",
                puArgs, (UInt32)uMethodAddr));

#ifdef _x86
        GET_REG(EAX, uEAX);
        GET_REG(EDX, uEDX);
        GET_REG(ECX, uECX);
        GET_REG(ESP, uESP);
        assert(uESP == (UInt32)puArgs);
        // NOTE: we should promise STUB_INVOKE_METHOD just occures
        // in use space , if it used in kernel mode , the return
        // args in stack have the possibilty for being modified
        //
        STUB_INVOKE_METHOD(ec, puArgs, uMethodAddr);
        SET_REG(ECX, uECX);
        SET_REG(EDX, uEDX);
        SET_REG(EAX, uEAX);
#elif defined(_arm)

#if defined(_GNUC)
        if (sizeof(UInt32) * 4 >= uInSize) {
            GET_REG(r0, r0);
            GET_REG(r1, r1);
            GET_REG(r2, r2);
            GET_REG(r3, r3);
            GET_REG(ip, ip);
            GET_REG(lr, lr);
            STUB_INVOKE_METHOD1(ec, puArgs, uMethodAddr);
            SET_REG(lr, lr);
            SET_REG(ip, ip);
            SET_REG(r3, r3);
            SET_REG(r2, r2);
            SET_REG(r1, r1);
            SET_REG(r0, r0);
        }
        else {
            argbuf = *(ArgumentBuffer *)puArgs;
            GET_REG(r0, r0);
            GET_REG(r1, r1);
            GET_REG(r2, r2);
            GET_REG(r3, r3);
            GET_REG(ip, ip);
            GET_REG(lr, lr);
            GET_REG(sp, sp);
            STUB_INVOKE_METHOD2(ec, puArgs, uMethodAddr);
            SET_REG(sp, sp);
            SET_REG(lr, lr);
            SET_REG(ip, ip);
            SET_REG(r3, r3);
            SET_REG(r2, r2);
            SET_REG(r1, r1);
            SET_REG(r0, r0);
            *(ArgumentBuffer *)puArgs = argbuf;
        }
#else

#error Unknown C++ compiler

#endif // defined(_GNUC)

#elif defined(_mips)
        GET_REG($2, uV0);
        GET_REG($3, uV1);
        GET_REG($4, uA0);
        GET_REG($5, uA1);
        GET_REG($6, uA2);
        GET_REG($7, uA3);
        GET_REG($8, uT0);
        GET_REG($9, uT1);
        GET_REG($10, uT2);
        GET_REG($11, uT3);
        GET_REG($12, uT4);
        GET_REG($13, uT5);
        GET_REG($14, uT6);
        GET_REG($15, uT7);
        GET_REG($24, uT8);
        GET_REG($25, uT9);
        GET_REG($29, uSp);
        STUB_INVOKE_METHOD(ec, puArgs, uMethodAddr);
        SET_REG($29, uSp);
        SET_REG($2, uV0);
        SET_REG($3, uV1);
        SET_REG($4, uA0);
        SET_REG($5, uA1);
        SET_REG($6, uA2);
        SET_REG($7, uA3);
        SET_REG($8, uT0);
        SET_REG($9, uT1);
        SET_REG($10, uT2);
        SET_REG($11, uT3);
        SET_REG($12, uT4);
        SET_REG($13, uT5);
        SET_REG($14, uT6);
        SET_REG($15, uT7);
        SET_REG($24, uT8);
        SET_REG($25, uT9);
#else
#error unknown architecture
#endif

        if (FAILED(ec) && GET_IN_INTERFACE_MARK(pMethodInfo->reserved1)) {
            bOnlyReleaseIn = TRUE;
        }

        reply->WriteInt32((int32_t)ec);
        // puArgs + 1 , skip this pointer
        if ((pOutHeader && SUCCEEDED(ec))
            || GET_IN_INTERFACE_MARK(pMethodInfo->reserved1)) {

            if (pOutHeader && SUCCEEDED(ec)) {
                pReplyParcel = reply;
            }

            orgec = pCurInterface->MarshalOut(puArgs + 1,
                                              pInHeader,
                                              pOutHeader,
                                              bOnlyReleaseIn,
                                              pReplyParcel);

            if (FAILED(orgec)) {
                MARSHAL_DBGOUT(MSHDBG_ERROR, printf(
                        "Stub: marshal out args fail, hr(%x)\n", ec));
                ec = orgec;
                goto ErrorExit;
            }
        }
    }
    else {
        if (pInHeader->m_hMethodIndex == 0) {
            MARSHAL_DBGOUT(MSHDBG_ERROR,
                    printf("Stub error: Remote Probe().\n"));
        }
        else if (pInHeader->m_hMethodIndex == 1) {
            MARSHAL_DBGOUT(MSHDBG_ERROR,
                    printf("Stub error: Remote AddRef().\n"));
        }
        else if (pInHeader->m_hMethodIndex == 2) {
            MARSHAL_DBGOUT(MSHDBG_ERROR,
                    printf("Stub error: Remote Release().\n"));
        }
        else if (pInHeader->m_hMethodIndex == 3) {
            MARSHAL_DBGOUT(MSHDBG_ERROR,
                    printf("Stub error: Remote GetInterfaceID().\n"));
        }
        ec = E_INVALID_OPERATION;    // Do not allow remote Probe().
        goto ErrorExit;
    }

    MARSHAL_DBGOUT(MSHDBG_NORMAL, printf(
            "Stub: invoke server method ok, _SysReply\n"));

ErrorExit:
    return ec;
}

static void __DumpGUID(REIID riid)
{
    PFL_EX("%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X\n",
        riid.Data1, riid.Data2, riid.Data3,
        riid.Data4[0], riid.Data4[1], riid.Data4[2],
        riid.Data4[3], riid.Data4[4], riid.Data4[5],
        riid.Data4[6], riid.Data4[7]);
    ALOGD("%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X\n",
        riid.Data1, riid.Data2, riid.Data3,
        riid.Data4[0], riid.Data4[1], riid.Data4[2],
        riid.Data4[3], riid.Data4[4], riid.Data4[5],
        riid.Data4[6], riid.Data4[7]);
}

void CObjectStub::PrintIpcLog(
    /* [in] */ MarshalHeader* header)
{
    CInterfaceStub* pCurInterface = &(m_pInterfaces[header->m_hInterfaceIndex]);
    InterfaceID temp = pCurInterface->m_pInfo->iid;
    __DumpGUID(temp);
    PFL_EX("Interface Index = %d", header->m_hInterfaceIndex)
    PFL_EX("Method Index = %d", header->m_hMethodIndex)
    ALOGD("tid: %d, Interface Index = %d", gettid(), header->m_hInterfaceIndex);
    ALOGD("Method Index = %d",header->m_hMethodIndex);
}

ECode CObjectStub::GetClassID(
    /* [out] */ EMuid *pClsid)
{
    assert(pClsid != NULL);

    *pClsid = ((CIClassInfo*)m_pInfo)->clsid;
    return NOERROR;
}

ECode CObjectStub::GetClassInfo(
    /* [out] */ CIClassInfo **ppClassInfo)
{
    assert(ppClassInfo != NULL);

    *ppClassInfo = m_pInfo;
    return NOERROR;
}

ECode CObjectStub::GetInterfaceIndex(
    /* [in] */ IInterface *pObj,
    /* [out] */ UInt32 *puIndex)
{
    for (UInt32 n = 0; n < (UInt32)m_cInterfaces; n++) {
        if (m_pInterfaces[n].m_pObject == pObj) {
            *puIndex = n;
            return NOERROR;
        }
    }
    MARSHAL_DBGOUT(MSHDBG_WARNING, printf(
        "Stub: InterfaceIndex failed - pObj(%x)\n", (UInt32)pObj));

    return E_NO_INTERFACE;
}

ECode CObjectStub::OnTransact(
    /* [in] */ const String& cmd,
    /* [in] */ CRemoteParcel* data,
    /* [out] */ CRemoteParcel* reply,
    /* [in] */ Int32 flags)
{
    if (cmd.Equals("INVOKE")) {
        Invoke(data, reply);
        return NOERROR;
    }
    else if (cmd.Equals("GET_CLASS_INFO")) {
        EMuid clsid;
        CIModuleInfo *pSrcModInfo;
        CIModuleInfo *pDestModInfo;
        GetClassID(&clsid);
        if (FAILED(LookupModuleInfo(clsid, &pSrcModInfo))) {
            MARSHAL_DBGOUT(MSHDBG_ERROR,
                    printf("Lookup module info fail.\n"));
            reply->WriteInt32((int32_t)E_FAIL);
            return NOERROR;
        }

        pDestModInfo = (CIModuleInfo *)calloc(pSrcModInfo->totalSize, 1);
        FlatModuleInfo(pSrcModInfo, pDestModInfo);

        reply->WriteInt32((int32_t)NOERROR);
        reply->WriteInt32(pDestModInfo->totalSize);
        reply->Write((void*)pDestModInfo, pDestModInfo->totalSize);

        free(pDestModInfo);
        return NOERROR;
    }
    else if (cmd.Equals("PING")) {
        reply->WriteInt32((int32_t)NOERROR);
        return NOERROR;
    }
    else if (cmd.Equals("RELEASE")) {
        Release();
        reply->WriteInt32((int32_t)NOERROR);
        return E_FAIL;
    }

    MARSHAL_DBGOUT(MSHDBG_ERROR, printf(
            "CObjectStub OnTransact: cmd is wrong.\n"));
    reply->WriteInt32((int32_t)E_FAIL);
    return NOERROR;
}

ECode CObjectStub::TransactRunnable::Run()
{
    CObjectStub* pStub = (CObjectStub*)mObject;
    String cmd;
    mData->ReadString(&cmd);
    AutoPtr<CRemoteParcel> reply = new CRemoteParcel(FALSE);
    ECode ec = pStub->OnTransact(cmd, mData, reply);
    if (SUCCEEDED(ec)) {
        void* payload;
        reply->GetElementPayload((Handle32*)&payload);
        Int32 payloadSize;
        reply->GetElementSize(&payloadSize);
        AutoPtr<DataBuffer> dataBuf = new DataBuffer();
        Int32 totalSize = htonl(sizeof(totalSize) + payloadSize);
        dataBuf->Append((void*)&totalSize, sizeof(totalSize));
        dataBuf->Append(payload, payloadSize);
        assert(ntohl(totalSize) == dataBuf->DataSize());
        send(mConnfd, dataBuf->Data(), dataBuf->DataSize(), 0);
    }
    return NOERROR;
}

Int32 CObjectStub::AcceptCallback(
    /* [in] */ Int32 connfd,
    /* [in] */ Int32 events,
    /* [in] */ void* data)
{
    if (events & (LOOPER_EVENT_ERROR | LOOPER_EVENT_HANGUP)) {
        ALOGE("An error occurred. events=0x%x", events);
        return 0; // remove the callback
    }

    if (!(events & LOOPER_EVENT_INPUT)) {
        ALOGW("Received spurious callback for unhandled poll event. events=0x%x", events);
        return 1;
    }

    Byte buffer[1024];
    memset(buffer, 0, 1024);
    AutoPtr<CRemoteParcel> dataParcel = new CRemoteParcel(FALSE);
    Int32 size = recv(connfd, (void*)buffer, sizeof(buffer), 0);
    if (size <= 0) {
        ALOGD("RemoteInvoke: remote socket broken.");
        return 0;
    }
    Int32 totalSize = ntohl(*(Int32*)buffer);
    dataParcel->Write(buffer + sizeof(Int32), size - sizeof(Int32));
    while((totalSize = totalSize - size) > 0) {
        size = recv(connfd, (void*)buffer, sizeof(buffer), 0);
        if (size <= 0) {
            ALOGD("RemoteInvoke: remote socket broken.");
            return 0;
        }
        dataParcel->Write(buffer, size);
    }
    dataParcel->SetDataPosition(0);

    AutoPtr<ThreadPoolExecutor::Runnable> r = new TransactRunnable(
            connfd, events, data, dataParcel);
    ThreadPoolExecutor::Self()->RunTask(r);
    return 1;
}

Int32 CObjectStub::ListenCallback(
    /* [in] */ Int32 socketfd,
    /* [in] */ Int32 events,
    /* [in] */ void* data)
{
    if (events & (LOOPER_EVENT_ERROR | LOOPER_EVENT_HANGUP)) {
        ALOGE("An error occurred. events=0x%x", events);
        return 0; // remove the callback
    }

    if (!(events & LOOPER_EVENT_INPUT)) {
        ALOGW("Received spurious callback for unhandled poll event. events=0x%x", events);
        return 1;
    }

    struct sockaddr_in name;
    socklen_t len = sizeof(struct sockaddr_in);
    bzero(&name, len);

    Int32 connfd = accept(socketfd, (struct sockaddr*)&name, &len);
    if (connfd < 0) {
        MARSHAL_DBGOUT(MSHDBG_ERROR, printf(
                "accept socket failed\n"));
        return 0;
    }

    Looper::GetLooper()->AddFd(
            connfd,
            LOOPER_POLL_CALLBACK,
            LOOPER_EVENT_INPUT,
            CObjectStub::AcceptCallback,
            data);

    ((CObjectStub*)data)->m_cConnfd = connfd;
    return 1;
}

ECode CObjectStub::S_CreateObject(
    /* [in] */ IInterface *pObject,
    /* [out] */ IStub **ppIStub)
{
    CObjectStub *pStub;
    CInterfaceStub *pInterfaces;
    IInterface *pObj;
    IObject *pObj1;
    ClassID clsid;
    InterfaceID iid;
    ECode ec = NOERROR;
    CIClassInfo ** ppClassInfo = NULL;
    Int32 n;
    Int32 socketfd = -1;
    struct sockaddr_in name, realAddr;
    struct sockaddr_in* addr;
    struct ifreq ifr_ip;

    socklen_t socklen;
    String netAddress;

    if (!pObject || !ppIStub) {
        return E_INVALID_ARGUMENT;
    }

    *ppIStub = NULL;

    pStub = new CObjectStub();
    if (!pStub) {
        return E_OUT_OF_MEMORY;
    }

    pObj1 = (IObject*)pObject->Probe(EIID_IObject);
    if (!pObj1) {
        // DUMP_ITFID(pObject);
	    MARSHAL_DBGOUT(MSHDBG_ERROR, printf(
                "Create stub: interface do not support EIID_IObject QI.\n"));
        goto ErrorExit;
    }

    ec = pObj1->GetClassID(&clsid);
    if (FAILED(ec)) {
        MARSHAL_DBGOUT(MSHDBG_ERROR, printf(
                "Create stub: fail to get object's ClassID.\n"));
        goto ErrorExit;
    }

    MARSHAL_DBGOUT(MSHDBG_NORMAL, printf(
            "QI class info ok. EMuid is:\n"));
    MARSHAL_DBGOUT(MSHDBG_NORMAL, DUMP_CLSID(clsid));

    ec = pObject->GetInterfaceID(pObject, &iid);
    if (FAILED(ec)) {
        MARSHAL_DBGOUT(MSHDBG_ERROR, printf(
                "Create stub: interface does not have InterfaceID.\n"));
        goto ErrorExit;
    }
    MARSHAL_DBGOUT(MSHDBG_NORMAL, printf(
            "QI EIID info ok. EIID is:\n"));
    MARSHAL_DBGOUT(MSHDBG_NORMAL, DUMP_GUID(iid));

    ec = LookupClassInfo(clsid.clsid, &(pStub->m_pInfo));
    if (FAILED(ec)) {
        ec = AcquireClassInfo(clsid, &(pStub->m_pInfo));
        if (FAILED(ec)) {
            MARSHAL_DBGOUT(MSHDBG_ERROR,
                    printf("Create Stub: get class info fail, the ec = 0x%08x\n", ec));
            goto ErrorExit;
        }
    }

    ppClassInfo = &(pStub->m_pInfo);

    MARSHAL_DBGOUT(MSHDBG_NORMAL, printf(
            "Create stub: Get class info ok.\n"));

    pInterfaces = new CInterfaceStub[(*ppClassInfo)->interfaceNum];
    if (!pInterfaces) {
        MARSHAL_DBGOUT(MSHDBG_ERROR, printf("Create stub: out of memory.\n"));
        ec = E_OUT_OF_MEMORY;
        goto ErrorExit;
    }
    pStub->m_cInterfaces = (*ppClassInfo)->interfaceNum;
    pStub->m_pInterfaces = pInterfaces;
    pObject->AddRef();
    for (n = 0; n < (*ppClassInfo)->interfaceNum; n++) {
        CIInterfaceInfo *pInterfaceInfo =
                (CIInterfaceInfo *)GetUnalignedPtr(
                        (*ppClassInfo)->interfaces + n);
        pObj = pObject->Probe(pInterfaceInfo->iid);
        if (!pObj) {
            MARSHAL_DBGOUT(MSHDBG_ERROR,
                    printf("Create stub: no such interface.\n"));
            ec = E_NO_INTERFACE;
            pObject->Release();
            goto ErrorExit;
        }
        pInterfaces[n].m_pInfo = pInterfaceInfo;
        pInterfaces[n].m_pObject = pObj;
    }

    Looper::StartLooper();

    socketfd = socket(PF_INET, SOCK_STREAM, 0);
    if (socketfd < 0) {
        pObject->Release();
        MARSHAL_DBGOUT(MSHDBG_ERROR, printf(
                "Create stub: create socket failed\n"));
        ec = E_FAIL;
        goto ErrorExit;
    }

    name.sin_family = AF_INET;
    name.sin_addr.s_addr = htonl(INADDR_ANY);
    name.sin_port = htons(0);
    if (bind(socketfd, (struct sockaddr*)&name, sizeof(name)) < 0) {
        pObject->Release();
        MARSHAL_DBGOUT(MSHDBG_ERROR, printf(
                "Create stub: bind socket failed\n"));
        ec = E_FAIL;
        goto ErrorExit;
    }

    if (listen(socketfd, 10) == -1) {
        pObject->Release();
        MARSHAL_DBGOUT(MSHDBG_ERROR, printf(
                "Create stub: listen socket failed\n"));
        ec = E_FAIL;
        goto ErrorExit;
    }

    // get net address of this stub
    memset(&ifr_ip, 0, sizeof(ifr_ip));
    strncpy(ifr_ip.ifr_name, "eth0", sizeof(ifr_ip.ifr_name) - 1);
    if (ioctl(socketfd, SIOCGIFADDR, &ifr_ip) < 0) {
        pObject->Release();
        MARSHAL_DBGOUT(MSHDBG_ERROR, printf(
                "Create stub: get ip address failed\n"));
        ec = E_FAIL;
        goto ErrorExit;
    }
    addr = (struct sockaddr_in*)&ifr_ip.ifr_addr;
    netAddress = inet_ntoa(addr->sin_addr);

    socklen = sizeof(realAddr);
    if (getsockname(socketfd, (struct sockaddr*)&realAddr, &socklen) == -1) {
        pObject->Release();
        MARSHAL_DBGOUT(MSHDBG_ERROR, printf(
                "Create stub: getsocketname failed\n"));
        ec = E_FAIL;
        goto ErrorExit;
    }
    netAddress.AppendFormat(":%d", ntohs(realAddr.sin_port));
    pStub->m_sNetAddress = netAddress;
    ec = RegisterExportObject(netAddress, pObject, pStub);
    if (FAILED(ec)) {
        pObject->Release();
        MARSHAL_DBGOUT(MSHDBG_ERROR, printf(
                "Create stub: register export object failed, ec(%x)\n", ec));
        goto ErrorExit;
    }

    // 4) add to looper
    Looper::GetLooper()->AddFd(
            socketfd,
            LOOPER_POLL_CALLBACK,
            LOOPER_EVENT_INPUT,
            CObjectStub::ListenCallback,
            (void*)pStub);

    pStub->m_cSocketfd = socketfd;
    *ppIStub = (IStub *)pStub;

    return NOERROR;

ErrorExit:
    delete pStub;
    if (socketfd != -1) close(socketfd);
    return ec;
}

} // namespace RPC
} // namespace Elastos
