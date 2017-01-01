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

#include <elapi.h>
#include <stdlib.h>
#include <ctype.h>
#include <pthread.h>

#ifdef _win32
#include <e_time.h>
#endif

using namespace Elastos;

EXTERN_C const InterfaceID EIID_IProxy;

class CCallbackRendezvous
    : public ElLightRefBase
    , public ICallbackRendezvous
{
public:
    CCallbackRendezvous(
        /* [in] */ PInterface callbackContext,
        /* [in] */ ICallbackSink* callbackSink,
        /* [in] */ Int32 eventId,
        /* [out] */ Boolean* eventFlag,
        /* [in] */ Boolean newCallback)
        : mCallbackContext(callbackContext)
        , mCallbackSink(callbackSink)
        , mEventId(eventId)
        , mEventFlag(eventFlag)
        , mNewCallback(newCallback)
    {}

    virtual ~CCallbackRendezvous();

    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI_(UInt32) AddRef()
    {
        return ElLightRefBase::AddRef();
    }

    CARAPI_(UInt32) Release()
    {
        return ElLightRefBase::Release();
    }

    CARAPI GetInterfaceID(
        /* [in] */ IInterface* object,
        /* [in] */ InterfaceID* iid)
    {
        return E_NOT_IMPLEMENTED;
    }

    CARAPI Wait(
        /* in */ Int32 msTimeout,
        /* out */ WaitResult* result);

public:
    AutoPtr<IInterface> mCallbackContext;
    AutoPtr<ICallbackSink> mCallbackSink;
    Int32 mEventId;
    Boolean* mEventFlag;
    Boolean mNewCallback;
};

PInterface CCallbackRendezvous::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)this;
    }
    return NULL;
}

CCallbackRendezvous::~CCallbackRendezvous()
{
    if (mCallbackSink) {
        if (mNewCallback) {
            PInterface orgCtx;
            orgCtx = (PInterface)pthread_getspecific(TL_CALLBACK_SLOT);
            if (mCallbackContext) mCallbackContext->AddRef();
            pthread_setspecific(TL_CALLBACK_SLOT, mCallbackContext);
            mCallbackSink->RemoveCallback(mEventId, EventHandler::Make(NULL, NULL));
            if (mCallbackContext) mCallbackContext->Release();
            pthread_setspecific(TL_CALLBACK_SLOT, orgCtx);
        }
    }
}

ECode CCallbackRendezvous::Wait(
    /* [in] */ Int32 msTimeOut,
    /* [in] */ WaitResult* result)
{
    return _Impl_CallbackSink_WaitForCallbackEvent(
            mCallbackContext, msTimeOut, result, mEventFlag, 0);
}

ELAPI _Impl_CCallbackRendezvous_New(
    /* [in] */ PInterface callbackContext,
    /* [in] */ ICallbackSink* callbackSink,
    /* [in] */ CallbackEventId eventId,
    /* [in] */ Boolean* eventFlag,
    /* [in] */ Boolean newCallback,
    /* [in] */ ICallbackRendezvous** callbackRendezvous)
{
    if (NULL == callbackRendezvous) return E_INVALID_ARGUMENT;
    *callbackRendezvous = new CCallbackRendezvous(
            callbackContext, callbackSink, eventId, eventFlag, newCallback);
    if (*callbackRendezvous == NULL) return E_OUT_OF_MEMORY;
    (*callbackRendezvous)->AddRef();

    return NOERROR;
}

// ElAPI functions
//

pthread_key_t g_TlSystemSlots[10];

EXTERN_C pthread_key_t *getTlSystemSlotBase()
{
    return g_TlSystemSlots;
}

EXTERN void InitTLS()
{
    for (Int32 i = 0; i < 10; i++) {
        pthread_key_create(&g_TlSystemSlots[i], NULL);
    }
}

EXTERN void UninitTLS()
{
    for (Int32 i = 0; i < 10; i++) {
        pthread_key_delete(g_TlSystemSlots[i]);
    }
}

ELAPI _CObject_EnterRegime(
    /* [in] */ PInterface object,
    /* [in] */ PRegime regime)
{
    return regime->ObjectEnter(object);
}

ELAPI _CObject_LeaveRegime(
    /* [in] */ PInterface object,
    /* [in] */ PRegime regime)
{
    return regime->ObjectLeave(object);
}

ELAPI_(Boolean) _Impl_CheckHelperInfoFlag(
    /* [in] */ UInt32 flag)
{
    UInt32 uFlag = 0;

    uFlag = (UInt32)pthread_getspecific(TL_HELPER_INFO_SLOT);

    if (uFlag & flag) return TRUE;
    return FALSE;
}

ELAPI_(void) _Impl_SetHelperInfoFlag(
    /* [in] */ UInt32 flag,
    /* [in] */ Boolean value)
{
    UInt32 uFlag = (UInt32)pthread_getspecific(TL_HELPER_INFO_SLOT);

    if (value) {
        uFlag |= flag;
    }
    else {
        uFlag &= ~flag;
    }

    ECode ec __attribute__((__unused__)) = pthread_setspecific(TL_HELPER_INFO_SLOT, (void*)uFlag);
    assert(SUCCEEDED(ec));
}

ELAPI _Impl_EnterProtectedZone()
{
    _Impl_SetHelperInfoFlag(HELPER_ENTERED_PROTECTED_ZONE, TRUE);
    return NOERROR;
}

ELAPI _Impl_LeaveProtectedZone()
{
    _Impl_SetHelperInfoFlag(HELPER_ENTERED_PROTECTED_ZONE, FALSE);
    return NOERROR;
}

ELAPI _Impl_InsideProtectedZone()
{
    if (!_Impl_CheckHelperInfoFlag(HELPER_ENTERED_PROTECTED_ZONE)) {
        return E_NOT_IN_PROTECTED_ZONE;
    }

    return NOERROR;
}

#include <marshal_ipc.h>
#include <marshal_rpc.h>

ELAPI _CObject_MarshalInterface(
    /* [in] */ IInterface* object,
    /* [in] */ MarshalType type,
    /* [out] */ void** package,
    /* [out] */ Int32* size)
{
    if (package == NULL) {
        if (size) *size = 0;
        return E_INVALID_ARGUMENT;
    }
    *package = NULL;

    if (size == NULL) {
        return E_INVALID_ARGUMENT;
    }
    *size = 0;

    if (object == NULL || size == NULL) {
        return E_INVALID_ARGUMENT;
    }

    if (type == MarshalType_IPC) {
        Elastos::IPC::InterfacePack* itfPack = (Elastos::IPC::InterfacePack*)calloc(sizeof(Elastos::IPC::InterfacePack), 1);
        if (itfPack == NULL) {
            return E_OUT_OF_MEMORY;
        }

        ECode ec = StdMarshalInterface(object, itfPack);
        if (FAILED(ec)) {
            free(itfPack);
            return ec;
        }

        *size = sizeof(Elastos::IPC::InterfacePack);
        *package = itfPack;
        return NOERROR;
    }
    else if (type == MarshalType_RPC) {
        Elastos::RPC::InterfacePack* itfPack = (Elastos::RPC::InterfacePack*)calloc(sizeof(Elastos::RPC::InterfacePack), 1);
        if (itfPack == NULL) {
            return E_OUT_OF_MEMORY;
        }

        ECode ec = StdMarshalInterface(object, itfPack);
        if (FAILED(ec)) {
            free(itfPack);
            return ec;
        }

        *size = sizeof(Elastos::RPC::InterfacePack);
        *package = itfPack;
        return NOERROR;
    }

    return E_INVALID_ARGUMENT;
}

ELAPI _CObject_UnmarshalInterface(
    /* [in] */ void* package,
    /* [in] */ MarshalType type,
    /* [in] */ UnmarshalFlag flag,
    /* [out] */ IInterface** object,
    /* [out] */ Int32* size)
{
    if (object == NULL) {
        if (size) *size = 0;
        return E_INVALID_ARGUMENT;
    }
    *object = NULL;

    if (size == NULL) {
        return E_INVALID_ARGUMENT;
    }
    *size = 0;

    if (package == NULL) {
        return E_INVALID_ARGUMENT;
    }

    if (type == MarshalType_IPC) {
        ECode ec = StdUnmarshalInterface(flag, (Elastos::IPC::InterfacePack*)package, object);
        if (FAILED(ec)) {
            return ec;
        }

        *size = sizeof(Elastos::IPC::InterfacePack);
        return NOERROR;
    }
    else if (type == MarshalType_RPC) {
        ECode ec = StdUnmarshalInterface(flag, (Elastos::RPC::InterfacePack*)package, object);
        if (FAILED(ec)) {
            return ec;
        }

        *size = sizeof(Elastos::RPC::InterfacePack);
        return NOERROR;
    }

    return E_INVALID_ARGUMENT;
}
