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
namespace IPC {
Address s_proxyEntryAddress = (Address)NULL;
}
}
#define PROXY_ENTRY_BASE    Elastos::IPC::s_proxyEntryAddress
#define PROXY_ENTRY_SIZE    0x10
#define PROXY_ENTRY_MASK    0x0f
#define PROXY_ENTRY_SHIFT   4
#define PROXY_ENTRY_NUM     0xe0

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
    return Elastos::IPC::CInterfaceProxy::ProxyEntry(args);
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

#ifdef _mips

EXTERN_C ECode GlobalProxyEntry(UInt32* args)
{
    return Elastos::IPC::CInterfaceProxy::ProxyEntry(args);
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
    s_proxyEntryAddress = (Address)mmap((void*)0,
            PAGE_ALIGN(PROXY_ENTRY_NUM * PROXY_ENTRY_SIZE),
            PROT_READ | PROT_WRITE | PROT_EXEC,
            MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (s_proxyEntryAddress == 0) {
        ALOGE("out of memory.\n");
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

void UninitProxyEntry()
{
     if (0 != s_proxyEntryAddress) {
        munmap((void*)s_proxyEntryAddress,
               PAGE_ALIGN(PROXY_ENTRY_NUM * PROXY_ENTRY_SIZE));
        s_proxyEntryAddress = (Address)NULL;
    }
}

ECode GetRemoteClassInfo(
    /* [in] */ android::IBinder* remoteObj,
    /* [in] */ REMuid clsId,
    /* [out] */ CIClassInfo** classInfo)
{
    ECode ec;
    android::Parcel data, reply;
    int32_t ciSize;
    CIModuleInfo* modInfo;

    if (classInfo == NULL) return E_INVALID_ARGUMENT;

    if (remoteObj->transact(IStub::GET_CLASS_INFO, data, &reply) != android::NO_ERROR) {
        MARSHAL_DBGOUT(MSHDBG_ERROR,
            _DumpGUID(clsId));
        MARSHAL_DBGOUT(MSHDBG_ERROR,
                ALOGE("Get class info failed.\n"));
        ec = E_REMOTE_FAIL;
        goto Exit;
    }

    ec = reply.readInt32();
    if (FAILED(ec)) {
        goto Exit;
    }

    ciSize = reply.readInt32();
    modInfo = (CIModuleInfo*)reply.readInplace(ciSize);
    ec = RegisterModuleInfo(modInfo);
    if (FAILED(ec)) {
        MARSHAL_DBGOUT(MSHDBG_ERROR,
            _DumpGUID(clsId));
        MARSHAL_DBGOUT(MSHDBG_ERROR,
                ALOGE("Register module info failed.\n"));
        goto Exit;
    }

    if (FAILED(LookupModuleInfo(clsId, &modInfo))) {
        MARSHAL_DBGOUT(MSHDBG_ERROR,
            _DumpGUID(clsId));
        MARSHAL_DBGOUT(MSHDBG_ERROR,
                ALOGE("Find module info failed.\n"));
        goto Exit;
    }

    ec = E_DOES_NOT_EXIST;
    for (Int32 m = 0; m < modInfo->mClassNum; m++) {
        if (modInfo->mClasses[m].mCLSID == clsId) {
            *classInfo = &(modInfo->mClasses[m]);

            MARSHAL_DBGOUT(MSHDBG_NORMAL, ALOGD("iid: "));
            MARSHAL_DBGOUT(MSHDBG_NORMAL,
                    DUMP_GUID((*classInfo)->mCLSID));
            MARSHAL_DBGOUT(MSHDBG_NORMAL,
                    ALOGD("UUNM: %s.\n", (*classInfo)->mUunm));
            MARSHAL_DBGOUT(MSHDBG_NORMAL,
                    ALOGD("InterfaceNumber: %d.\n", (*classInfo)->mInterfaceNum));

            ec = NOERROR;
            goto Exit;
        }
    }

Exit:
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
//    MarshalHeader *header = parcel->GetMarshalHeader();
//
//    if (header->mMagic != MARSHAL_MAGIC) {
//        MARSHAL_DBGOUT(MSHDBG_ERROR,
//                ALOGE("Proxy unmsh: invalid magic(%x)\n", header->mMagic));
//        return E_MARSHAL_DATA_TRANSPORT_ERROR;
//    }

#if defined(_DEBUG)
    // TODO:
//    if (header->mInterfaceIndex != (Int16)mIndex) {
//        MARSHAL_DBGOUT(MSHDBG_ERROR,
//                ALOGE("Proxy unmsh: invalid iidx(%x)\n",
//                header->mInterfaceIndex));
//        return E_MARSHAL_DATA_TRANSPORT_ERROR;
//    }
//    if (header->mMethodIndex != (Int16)(methodIndex + 4)) {
//        MARSHAL_DBGOUT(MSHDBG_ERROR, ALOGE(
//                "Proxy unmsh: invalid method(%x)\n", header->mMethodIndex));
//    }
#endif

    // TODO:
    return Proxy_ProcessUnmsh_Out(
            &(mInfo->mMethods[methodIndex]),
            (IParcel*)parcel,
            0/*header->mOutSize - sizeof(MarshalHeader)*/,
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
    Int32 cParams = methodInfo->mParamNum;
    const CIBaseType* params = methodInfo->mParams;

    for (Int32 n = 0; n < cParams; n++) {
        if (BT_IS_OUT(params[n])) return TRUE;
    }

    return FALSE;
}

Boolean CInterfaceProxy::IsMethodOneway(
    /* [in] */ UInt32 methodIndex)
{
    return BT_IS_ONEWAY((mInfo->mMethods[methodIndex]).mAttribs);
}

#if defined(_DEBUG) || defined(_MARSHAL_DEBUG)
int _DumpGUID(REIID riid)
{
    ALOGD("%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X\n",
        riid.mData1, riid.mData2, riid.mData3,
        riid.mData4[0], riid.mData4[1], riid.mData4[2],
        riid.mData4[3], riid.mData4[4], riid.mData4[5],
        riid.mData4[6], riid.mData4[7]);
    return printf("%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X\n",
        riid.mData1, riid.mData2, riid.mData3,
        riid.mData4[0], riid.mData4[1], riid.mData4[2],
        riid.mData4[3], riid.mData4[4], riid.mData4[5],
        riid.mData4[6], riid.mData4[7]);
}

int _DumpCLSID(RClassID rclsid)
{
    _DumpGUID((REIID)rclsid);
    ALOGD("%s\n", rclsid.mUunm);
    return printf("%s\n", rclsid.mUunm);
}

void _DumpInterfaceProxy(CInterfaceProxy* ip)
{
    ALOGD("  >> interface index:%d, id:", ip->mIndex);
    if (ip->mInfo) {
        _DumpGUID(ip->mInfo->mIID);
    }
    else {
        ALOGE("null");
    }
}

void _DumpObjectProxy(CObjectProxy* op)
{
    if (op->mInfo) {
        ALOGD(" >>> module: %s, CLSID:", op->mInfo->mUunm);
        _DumpGUID(op->mInfo->mCLSID);
    }

    ALOGD(" >>> %d interfaces:", op->mInterfaceNum);
    CInterfaceProxy* p = op->mInterfaces;
    for (Int32 i = 0; i < op->mInterfaceNum; ++i) {
        if (p) {
            _DumpInterfaceProxy(p);
        }
        ++p;
    }
}
#endif // _DEBUG || _MARSHAL_DEBUG

ECode CInterfaceProxy::ProxyEntry(
    /* [in] */ UInt32* args)
{
    CInterfaceProxy* thisPtr;
    UInt32 methodIndex, inSize, outSize;
    Int32 size = 0;
    MarshalHeader* inHeader = NULL;
    CRemoteParcel* inParcel = NULL;
    CRemoteParcel* outParcel = NULL;
    ECode ec, orgec;
#ifndef _mips
    UInt32 argNum __attribute__((__unused__));
#endif

    args++; // skip ret address

    thisPtr = (CInterfaceProxy *)*args;
    args++; // skip this

    MARSHAL_DBGOUT(MSHDBG_NORMAL,
            ALOGD("*args = %x, args = %x, ", *args, (UInt32)args));
    MARSHAL_DBGOUT(MSHDBG_NORMAL, ALOGD("iid: "));
    MARSHAL_DBGOUT(MSHDBG_NORMAL, DUMP_GUID(thisPtr->mInfo->mIID));

#ifdef _x86
    methodIndex = CalcMethodIndex(*(UInt32 *)((UInt32)&args - 4));
#elif defined(_arm)
    methodIndex = args[-3];
#elif defined(_mips)
    methodIndex = CalcMethodIndex(*(args - 4) - 4);
#else
#error unknown architecture
#endif

#ifndef _mips
    // Get the stack length, not contain "this"
    argNum = thisPtr->CountMethodArgs(methodIndex);

    MARSHAL_DBGOUT(MSHDBG_NORMAL, ALOGD(
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
                ALOGE("Proxy BufferSize() failed, ec = %x\n", ec));
        goto ProxyExit;
    }

    MARSHAL_DBGOUT(MSHDBG_NORMAL, ALOGD(
            "Buffer size: inSize(%d), outSize(%d)\n", inSize, outSize));
    assert(inSize >= sizeof(MarshalHeader));

#if defined(_DEBUG) || defined(_MARSHAL_DEBUG)
    // _DumpObjectProxy(thisPtr->mOwner);
    // ALOGD(" >>> tid:%d, Current Interface:", gettid());
    // _DumpInterfaceProxy(thisPtr);
    // ALOGD(" >>> Method index:%d, argNum:%d\n", methodIndex, argNum);
    // ALOGD(" >>> Buffer size: inSize(%d), outSize(%d)\n", inSize, outSize);
#endif

    inParcel = new CRemoteParcel();
    ec = thisPtr->MarshalIn(methodIndex, args, inParcel);

    if (SUCCEEDED(ec)) {
        android::Parcel* data;
        android::Parcel reply;

        inParcel->GetDataSize(&size);
        inParcel->GetDataPayload((Handle32*)&data);
        inHeader = inParcel->GetMarshalHeader();
        inHeader->mInSize = size;
        inHeader->mOutSize = outSize;
        MARSHAL_DBGOUT(MSHDBG_NORMAL, ALOGD(
                "Before RemoteInvoke: ParcelSize(%d)\n", size));
        uint32_t flags = thisPtr->IsMethodOneway(methodIndex) ? android::IBinder::FLAG_ONEWAY : 0;
        android::status_t st;
        if ((st = thisPtr->mOwner->mBinder->transact(IStub::INVOKE, *data, &reply, flags))
                != android::NO_ERROR) {
            MARSHAL_DBGOUT(MSHDBG_ERROR, _DumpObjectProxy(thisPtr->mOwner));
            MARSHAL_DBGOUT(MSHDBG_ERROR, ALOGE(" >>> Current Interface:"));
            MARSHAL_DBGOUT(MSHDBG_ERROR, _DumpInterfaceProxy(thisPtr));
            MARSHAL_DBGOUT(MSHDBG_ERROR, ALOGE("Method index:%d, argNum: %d\n", methodIndex, argNum));
            MARSHAL_DBGOUT(MSHDBG_ERROR, ALOGE("Buffer size: inSize(%d), outSize(%d)\n", inSize, outSize));
            MARSHAL_DBGOUT(MSHDBG_ERROR, ALOGE("Remote invoke failed. <status: 0x%x>\n", st));

            ec = E_REMOTE_FAIL;
            goto ProxyExit;
        }

        // TODO:
//        outParcel = new CRemoteParcel(&reply);
//        outParcel->ReadInt32(&ec);
        if (flags != android::IBinder::FLAG_ONEWAY) {
            ec = (ECode)reply.readInt32();
            if (SUCCEEDED(ec)) {
                if (thisPtr->MethodHasOutArgs(methodIndex)) {
                    if (outSize) {
                        outParcel = new CRemoteParcel(&reply);
                        orgec = thisPtr->UnmarshalOut(methodIndex,
                                outParcel, args);
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
        _DumpObjectProxy(thisPtr->mOwner);
        ALOGD(" >>> Current Interface:");
        _DumpInterfaceProxy(thisPtr);
        ALOGD(" >>> Method index:%d, argNum:%d\n", methodIndex, argNum);
        ALOGD(" >>> Buffer size: inSize(%d), outSize(%d)\n", inSize, outSize);

        assert(0);
#endif
    }

ProxyExit:
    MARSHAL_DBGOUT(MSHDBG_NORMAL, ALOGD("Exit proxy: ec(%x)\n", ec));

    if (inParcel != NULL) delete inParcel;
    if (outParcel != NULL) delete outParcel;

#ifndef _mips
    SYS_PROXY_EXIT(ec, &args - 1, argNum);
#else
    SYS_PROXY_EXIT(ec, &args - 1, 0);
#endif
}

DeathRecipientList::DeathRecipientList()
{
    MARSHAL_DBGOUT(MSHDBG_NORMAL, ALOGD("New DRL @ %p", this));
}

DeathRecipientList::~DeathRecipientList()
{
    MARSHAL_DBGOUT(MSHDBG_NORMAL, ALOGD("Destroy DRL @ %p", this));
    android::AutoMutex _l(mLock);

    // Should never happen -- the JavaDeathRecipient objects that have added themselves
    // to the list are holding references on the list object.  Only when they are torn
    // down can the list header be destroyed.
    if (mDeathRecipients.size() > 0) {
        android::List< android::sp<CarDeathRecipient> >::iterator iter;
        for (iter = mDeathRecipients.begin(); iter != mDeathRecipients.end(); iter++) {
            (*iter)->warnIfStillLive();
        }
    }
}

void DeathRecipientList::Add(
    /* [in] */ const android::sp<CarDeathRecipient>& recipient)
{
    android::AutoMutex _l(mLock);

    MARSHAL_DBGOUT(MSHDBG_NORMAL, ALOGD("DRL @ %p : add CDR %p", this, recipient.get()));
    mDeathRecipients.push_back(recipient);
}

void DeathRecipientList::Remove(
    /* [in] */ const android::sp<CarDeathRecipient>& recipient)
{
    android::AutoMutex _l(mLock);

    android::List< android::sp<CarDeathRecipient> >::iterator iter;
    for (iter = mDeathRecipients.begin(); iter != mDeathRecipients.end(); iter++) {
        if (*iter == recipient) {
            MARSHAL_DBGOUT(MSHDBG_NORMAL, ALOGD("DRL @ %p : remove CDR %p", this, recipient.get()));
            mDeathRecipients.erase(iter);
            return;
        }
    }
}

android::sp<CarDeathRecipient> DeathRecipientList::Find(
    /* [in] */ IProxyDeathRecipient* recipient)
{
    android::AutoMutex _l(mLock);

    android::List< android::sp<CarDeathRecipient> >::iterator iter;
    for (iter = mDeathRecipients.begin(); iter != mDeathRecipients.end(); iter++) {
        if ((*iter)->matches(recipient)) {
            return *iter;
        }
    }
    return NULL;
}

CarDeathRecipient::CarDeathRecipient(
    /* [in] */ IProxyDeathRecipient* recipient,
    /* [in] */ const android::sp<DeathRecipientList>& list)
    : mObject(recipient)
    , mDeathRecipients(list)
{
    if (mObject != NULL) {
        mObject->AddRef();
    }
    // These objects manage their own lifetimes so are responsible for final bookkeeping.
    // The list holds a strong reference to this object.
    MARSHAL_DBGOUT(MSHDBG_NORMAL, ALOGD("Adding CDR %p to DRL %p", this, list.get()));
    list->Add(this);
}

CarDeathRecipient::~CarDeathRecipient()
{
    if (mObject != NULL) {
        mObject->Release();
        mObject = NULL;
    }
}

void CarDeathRecipient::binderDied(
    /* [in] */ const android::wp<android::IBinder>& who)
{
    MARSHAL_DBGOUT(MSHDBG_NORMAL, ALOGD("Receiving binderDied() on CarDeathRecipient %p\n", this));
    if (mObject != NULL) {
        ECode ec = mObject->ProxyDied();
        if (FAILED(ec)) {
            MARSHAL_DBGOUT(MSHDBG_ERROR,
                ALOGE("*** Uncaught exception returned from death notification ProxyDied()! this: %p, mObject: %p",
                    this, mObject));
        }

        // Demote from strong ref to weak after binderDied() has been delivered,
        // to allow the DeathRecipient and BinderProxy to be GC'd if no longer needed.
        mObject->Release();
        mObject = NULL;
    }
}

void CarDeathRecipient::clearReference()
{
    android::sp<DeathRecipientList> list = mDeathRecipients.promote();
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

    if (mObject == NULL || recipient == NULL) result = FALSE;
    else result = mObject == recipient;

    return result;
}

void CarDeathRecipient::warnIfStillLive()
{
    if (mObject != NULL) {
        MARSHAL_DBGOUT(MSHDBG_NORMAL,
                ALOGD("BinderProxy is being destroyed but the application did not call "
                        "unlinkToDeath to unlink all of its death recipients beforehand.  "));
    }
}

CObjectProxy::CObjectProxy()
    : mOrgue(NULL)
    , mInterfaces(NULL)
    , mCallbackConnector(NULL)
{
}

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

    mBinder = NULL;
    mOrgue = NULL;
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
    else if (riid == EIID_IObject) {
        return (IObject*)this;
    }
    else if (riid == EIID_IWeakReferenceSource) {
        return (IWeakReferenceSource*)this;
    }

    if (riid == EIID_CALLBACK_CONNECTOR) {
        if (NULL == mCallbackConnector) {
            ClassID ezclsid;
            IInterface* temp = (IInterface *)&(mInterfaces[0].mVTPtr);
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
            _CObject_AcquireClassFactory(ezclsid, RGM_SAME_DOMAIN, EIID_CALLBACK_CONNECTOR, &temp);
            mCallbackConnector = (ICallbackConnector*)temp;
        }
        return (IInterface*)mCallbackConnector;
    }

    for (n = 0; n < mInterfaceNum; n++) {
        if (riid == mInterfaces[n].mInfo->mIID) {
            break;
        }
    }
    if (n == mInterfaceNum) {
        MARSHAL_DBGOUT(MSHDBG_WARNING, ALOGW(
                "Proxy: QI failed, iid: "));
        MARSHAL_DBGOUT(MSHDBG_WARNING, DUMP_GUID(riid));

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

void CObjectProxy::OnLastStrongRef(
    /* [in] */ const void* id)
{
#if defined(_DEBUG)
    String dbgInfo;
    ToString(&dbgInfo);
    MARSHAL_DBGOUT(MSHDBG_NORMAL, ALOGD(
            "Proxy OnLastStrongRef. binder: %s\n", dbgInfo.string()));
#endif

    ECode ec = UnregisterImportObject(mBinder.get());
    if (ec == S_FALSE) {
        MARSHAL_DBGOUT(MSHDBG_ERROR, ALOGD(
            "CObjectProxy UnregisterImportObject failed.\n"));
    }

    if (mCallbackConnector) {
        mCallbackConnector->DisconnectCallbackSink();
        mCallbackConnector->Release();
        mCallbackConnector = NULL;
    }

    // when BpBinder destructed, BBinder is not released immediately.
    // BBinder may be never released, if IPCThreadState::joinThreadPool is not called.
    // The following codes to ensure that BBinder can be released
    //
    android::Parcel data, reply;
    if (mBinder->transact(IStub::RELEASE, data, &reply) != android::NO_ERROR) {
#if defined(_DEBUG)
        MARSHAL_DBGOUT(MSHDBG_ERROR,
            ALOGE("CObjectProxy: Call stub %s release failed.\n", dbgInfo.string()));
#endif
    }
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
        AddRef();
        return NOERROR;
    }
    MARSHAL_DBGOUT(MSHDBG_WARNING, ALOGW(
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
    android::IBinder* target = mBinder.get();
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
        return E_INVALID_ARGUMENT;
    }

    android::IBinder* target = mBinder.get();
    if (target == NULL) {
        MARSHAL_DBGOUT(MSHDBG_ERROR,
                ALOGE("Binder has been finalized when calling linkToDeath() with recip=%p)\n", recipient));
        return E_FAIL;
    }

    MARSHAL_DBGOUT(MSHDBG_NORMAL,
                ALOGD("linkToDeath: binder=%p recipient=%p\n", target, recipient));

    if (!target->localBinder()) {
        if (mOrgue == NULL) mOrgue = new DeathRecipientList();
        android::sp<CarDeathRecipient> cdr = new CarDeathRecipient(recipient, mOrgue);
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
        return E_INVALID_ARGUMENT;
    }

    android::IBinder* target = mBinder.get();
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
        DeathRecipientList* list = mOrgue.get();
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

ECode CObjectProxy::GetClassInfo(
    /* [out] */ IInterface** clsInfo)
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
    /* [in] */ REMuid rclsid,
    /* [in] */ const android::sp<android::IBinder>& binder,
    /* [out] */ IProxy** proxy)
{
    CObjectProxy* proxyObj;
    CInterfaceProxy* interfaces;
    Int32 n;
    ECode ec;

    if (proxy == NULL) return E_INVALID_ARGUMENT;

    proxyObj = new CObjectProxy();
    if (!proxyObj) {
        MARSHAL_DBGOUT(MSHDBG_ERROR,
                ALOGE("Create proxy object: out of memory.\n"));
        return E_OUT_OF_MEMORY;
    }

    proxyObj->mBinder = binder;

    ec = LookupClassInfo(rclsid, &(proxyObj->mInfo));
    if (FAILED(ec)) {
        ec = GetRemoteClassInfo(proxyObj->mBinder.get(),
                                 rclsid,
                                 &proxyObj->mInfo);
        if (FAILED(ec)) goto ErrorExit;
    }

    proxyObj->mInterfaceNum = ((CIClassInfo*)(proxyObj->mInfo))->mInterfaceNum;
    interfaces = new CInterfaceProxy[proxyObj->mInterfaceNum];
    if (!interfaces) {
        MARSHAL_DBGOUT(MSHDBG_ERROR,
                ALOGE("Create proxy interfaces: out of memory.\n"));
        ec = E_OUT_OF_MEMORY;
        goto ErrorExit;
    }
    proxyObj->mInterfaces = interfaces;
    memset(interfaces, 0, sizeof(CInterfaceProxy) * proxyObj->mInterfaceNum);
    for (n = 0; n < proxyObj->mInterfaceNum; n++) {
        interfaces[n].mIndex = n;
        interfaces[n].mOwner = proxyObj;
        CIInterfaceInfo *pInterfaceInfo =
            (CIInterfaceInfo *)GetUnalignedPtr(
                    proxyObj->mInfo->mInterfaces + n);
        interfaces[n].mInfo = pInterfaceInfo;
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

    ec = RegisterImportObject(proxyObj->mBinder.get(), (IProxy*)proxyObj);
    if (FAILED(ec)) {
        MARSHAL_DBGOUT(MSHDBG_ERROR, ALOGE(
                "Create proxy: register import object failed, ec(%x)\n",
                ec));
        goto ErrorExit;
    }

    *proxy = (IProxy*)proxyObj;
    (*proxy)->AddRef();
    return NOERROR;

ErrorExit:
    delete proxyObj;
    return ec;
}

} // namespace IPC
} // namespace Elastos
