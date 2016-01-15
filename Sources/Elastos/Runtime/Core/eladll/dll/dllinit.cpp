//==========================================================================
// Copyright (c) 2000-2009,  Elastos, Inc.  All Rights Reserved.
//==========================================================================
#include <elastos.h>

_ELASTOS_NAMESPACE_USING

extern "C" UInt32 StartServiceCentral();
extern "C" UInt32 StopServiceCentral();

extern void InitTLS();
extern void UninitTLS();

extern ECode InitMIL();
extern void UninitMIL();

namespace Elastos {
namespace IPC {
extern ECode InitROT();
extern void UninitROT();

extern void InitProxyEntry();
extern void UninitProxyEntry();
} // namespace IPC
} // namespace Elastos

namespace Elastos {
namespace RPC {
extern ECode InitROT_RPC();
extern void UninitROT_RPC();

extern void InitProxyEntry();
extern void UninitProxyEntry();
} // namespace RPC
} // namespace Elastos

extern pthread_mutex_t g_LocModListLock;

#define EXIT_IFFAILED(func)  do { \
        ec = func(); \
        if (FAILED(ec)) { \
            goto E_FAIL_EXIT; \
        } \
    }while(0)

Boolean AttachElastosDll()
{
#ifdef _win32
    ECode ec = NOERROR;
#endif
    pthread_mutexattr_t recursiveAttr;

    InitTLS();
    InitMIL();

    Elastos::IPC::InitROT();
    Elastos::IPC::InitProxyEntry();

    Elastos::RPC::InitROT_RPC();
    Elastos::RPC::InitProxyEntry();

    pthread_mutexattr_init(&recursiveAttr);
    pthread_mutexattr_settype(&recursiveAttr, PTHREAD_MUTEX_RECURSIVE);
    if (pthread_mutex_init(&g_LocModListLock, &recursiveAttr)) {
        goto E_FAIL_EXIT;
    }
    pthread_mutexattr_destroy(&recursiveAttr);

    return TRUE;

E_FAIL_EXIT:
    return FALSE;
}

void DetachElastosDll()
{
    pthread_mutex_destroy(&g_LocModListLock);

    Elastos::RPC::UninitProxyEntry();
    Elastos::RPC::UninitROT_RPC();

    Elastos::IPC::UninitProxyEntry();
    Elastos::IPC::UninitROT();

    UninitMIL();
    UninitTLS();
}

