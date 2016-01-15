
#include <elastos/Thread.h>
#include "MyRunnable.h"
#include "stdio.h"

MyRunnable::MyRunnable()
    : mStop(FALSE)
{
}

UInt32 MyRunnable::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 MyRunnable::Release()
{
    return ElRefBase::Release();
}

PInterface MyRunnable::Probe(
    /* [in]  */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)this;
    }
    else if (riid == EIID_IRunnable) {
        return (IRunnable*)this;
    }

    return NULL;
}

ECode MyRunnable::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    if (pIID == NULL) {
        return E_INVALID_ARGUMENT;
    }

    if (pObject == (IInterface*)(IRunnable*)this) {
        *pIID = EIID_IRunnable;
    }
    else {
        return E_INVALID_ARGUMENT;
    }

    return NOERROR;
}

ECode MyRunnable::Run()
{
    while ( !mStop ) {
        printf("==========runrunrunrunrunrun=========\n");
        ECode ec = Thread::Sleep(1000);
        if (ec == (ECode)E_INTERRUPTED_EXCEPTION) {
            printf("======Thread interrupted...======\n");
        }

    }

    printf("======Thread exiting under request...======\n");
    return NOERROR;
}

Mutex* MyRunnable::GetSelfLock()
{
    return &mLock;
}
