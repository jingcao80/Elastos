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

#include <Elastos.CoreLibrary.Utility.Concurrent.h>
#include "elastos/droid/os/CHandlerThread.h"
#include "elastos/droid/internal/utility/CAsyncChannel.h"
#include "elastos/droid/wifi/CRttManager.h"
#include "elastos/droid/wifi/CRttManagerCapabilities.h"
#include "elastos/droid/wifi/CRttManagerParcelableRttParams.h"
#include "elastos/droid/utility/CSparseArray.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Internal::Utility::CAsyncChannel;
using Elastos::Droid::Os::CHandlerThread;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IMessenger;
using Elastos::Droid::Utility::CSparseArray;
using Elastos::Core::AutoLock;
using Elastos::Core::IThread;
using Elastos::Utility::Concurrent::CCountDownLatch;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Wifi {

//===============================================================
//                  CRttManager::ServiceHandler
//===============================================================

CRttManager::ServiceHandler::ServiceHandler(
    /* [in] */ ILooper* looper)
    : Handler(looper)
{
}

//@Override
ECode CRttManager::ServiceHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case IAsyncChannel::CMD_CHANNEL_HALF_CONNECTED: {
                Int32 arg1;
                msg->GetArg1(&arg1);
                if (arg1 == IAsyncChannel::STATUS_SUCCESSFUL) {
                    sAsyncChannel->SendMessage(IAsyncChannel::CMD_CHANNEL_FULL_CONNECTION);
                }
                else {
                    Logger::E(TAG, "Failed to set up channel connection");
                    // This will cause all further async API calls on the WifiManager
                    // to fail and throw an exception
                    sAsyncChannel = NULL;
                }
                return sConnected->CountDown();
            }
        case IAsyncChannel::CMD_CHANNEL_FULLY_CONNECTED:
            return NOERROR;
        case IAsyncChannel::CMD_CHANNEL_DISCONNECTED:
            Logger::E(TAG, "Channel connection lost");
            // This will cause all further async API calls on the WifiManager
            // to fail and throw an exception
            sAsyncChannel = NULL;
            AutoPtr<ILooper> looper;
            GetLooper((ILooper**)&looper);
            return looper->Quit();
    }

    Int32 arg2;
    msg->GetArg2(&arg2);
    AutoPtr<IObject> listener = GetListener(arg2);
    if (listener == NULL) {
        if (DBG) Logger::D(TAG, "invalid listener key = %d", arg2);
        return NOERROR;
    }
    else {
        if (DBG) Logger::D(TAG, "listener key = %s", arg2);
    }

    switch (what) {
        /* ActionListeners grouped together */
        case CMD_OP_SUCCEEDED :
            ReportSuccess(listener, msg);
            RemoveListener(arg2);
            break;
        case CMD_OP_FAILED :
            ReportFailure(listener, msg);
            RemoveListener(arg2);
            break;
        case CMD_OP_ABORTED :
            IRttManagerRttListener::Probe(listener)->OnAborted();
            RemoveListener(arg2);
            break;
        default:
            if (DBG) Logger::D(TAG, "Ignoring message %d", what);
            return NOERROR;
    }

    return  NOERROR;
}

ECode CRttManager::ServiceHandler::ReportSuccess(
    /* [in] */ IObject* listener,
    /* [in] */ IMessage* msg)
{
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    IRttManagerParcelableRttResults* parcelableResults = IRttManagerParcelableRttResults::Probe(obj);
    AutoPtr< ArrayOf<IRttManagerRttResult*> > results;
    parcelableResults->GetResults((ArrayOf<IRttManagerRttResult*>**)&results);
    return IRttManagerRttListener::Probe(listener)->OnSuccess(results);
}

ECode CRttManager::ServiceHandler::ReportFailure(
    /* [in] */ IObject* listener,
    /* [in] */ IMessage* msg)
{
    AutoPtr<IInterface> bundle;
    msg->GetObj((IInterface**)&bundle);
    Int32 arg1;
    msg->GetArg1(&arg1);
    String str;
    IBundle::Probe(bundle)->GetString(DESCRIPTION_KEY, &str);
    return IRttManagerRttListener::Probe(listener)->OnFailure(arg1, str);
}

//===============================================================
//                         CRttManager
//===============================================================

static AutoPtr<ISparseArray> sListenerMapInit()
{
    AutoPtr<ISparseArray> array;
    CSparseArray::New((ISparseArray**)&array);
    return array;
}

const Boolean CRttManager::DBG = TRUE;
const String CRttManager::TAG("RttManager");

const Int32 CRttManager::INVALID_KEY;
Int32 CRttManager::sListenerKey = 1;

AutoPtr<ISparseArray> CRttManager::sListenerMap = sListenerMapInit();
Object CRttManager::sListenerMapLock;

AutoPtr<IAsyncChannel> CRttManager::sAsyncChannel;
AutoPtr<ICountDownLatch> CRttManager::sConnected;

Object CRttManager::sThreadRefLock;
Int32 CRttManager::sThreadRefCount = 0;
AutoPtr<IHandlerThread> CRttManager::sHandlerThread;

CAR_INTERFACE_IMPL(CRttManager, Object, IRttManager)

CAR_OBJECT_IMPL(CRttManager)

ECode CRttManager::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IIRttManager* service)
{
    mContext = context;
    mService = service;
    Init();
    return NOERROR;
}

ECode CRttManager::GetCapabilities(
    /* [out] */ IRttManagerCapabilities** result)
{
    VALIDATE_NOT_NULL(result);
    return CRttManagerCapabilities::New(result);
}

ECode CRttManager::StartRanging(
    /* [in] */ ArrayOf<IRttManagerRttParams*>* params,
    /* [in] */ IRttManagerRttListener* listener)
{
    ValidateChannel();
    AutoPtr<IRttManagerParcelableRttParams> parcelableParams;
    CRttManagerParcelableRttParams::New(params, (IRttManagerParcelableRttParams**)&parcelableParams);
    return sAsyncChannel->SendMessage(CMD_OP_START_RANGING,
            0, PutListener(IObject::Probe(listener)), parcelableParams);
}

ECode CRttManager::StopRanging(
    /* [in] */ IRttManagerRttListener* listener)
{
    ValidateChannel();
    return sAsyncChannel->SendMessage(CMD_OP_STOP_RANGING, 0, RemoveListener(IObject::Probe(listener)));
}

void CRttManager::Init()
{
    AutoLock lock(sThreadRefLock);

    if (++sThreadRefCount == 1) {
        AutoPtr<IMessenger> messenger;
        // try {
            Logger::D(TAG, "Get the messenger from %p", mService.Get());
            mService->GetMessenger((IMessenger**)&messenger);
        // } catch (RemoteException e) {
        //     /* do nothing */
        // } catch (SecurityException e) {
        //     /* do nothing */
        // }

        if (messenger == NULL) {
            sAsyncChannel = NULL;
            return;
        }

        CHandlerThread::New(String("WifiScanner"), (IHandlerThread**)&sHandlerThread);
        CAsyncChannel::New((IAsyncChannel**)&sAsyncChannel);
        CCountDownLatch::New(1, (ICountDownLatch**)&sConnected);

        IThread::Probe(sHandlerThread)->Start();
        AutoPtr<ILooper> looper;
        sHandlerThread->GetLooper((ILooper**)&looper);
        AutoPtr<IHandler> handler = new ServiceHandler(looper);
        sAsyncChannel->Connect(mContext, handler, messenger);
        // try {
            sConnected->Await();
        // } catch (InterruptedException e) {
        //     Log.e(TAG, "interrupted wait at init");
        // }
    }
}

ECode CRttManager::ValidateChannel()
{
    if (sAsyncChannel == NULL) {
        // throw new IllegalStateException(
        //         "No permission to access and change wifi or a bad initialization");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

Int32 CRttManager::PutListener(
    /* [in] */ IObject* listener)
{
    if (listener == NULL) return INVALID_KEY;
    Int32 key;
    {
        AutoLock lock(sListenerMapLock);
        do {
            key = sListenerKey++;
        } while (key == INVALID_KEY);
        sListenerMap->Put(key, listener);
    }
    return key;
}

AutoPtr<IObject> CRttManager::GetListener(
    /* [in] */ Int32 key)
{
    if (key == INVALID_KEY) return NULL;

    {
        AutoLock lock(sListenerMapLock);
        AutoPtr<IInterface> listener;
        sListenerMap->Get(key, (IInterface**)&listener);
        return IObject::Probe(listener);
    }
}

Int32 CRttManager::GetListenerKey(
    /* [in] */ IObject* listener)
{
    if (listener == NULL) return INVALID_KEY;

    {
        AutoLock lock(sListenerMapLock);
        Int32 index;
        sListenerMap->IndexOfValue(listener, &index);
        if (index == -1) {
            return INVALID_KEY;
        }
        else {
            Int32 key;
            sListenerMap->KeyAt(index, &key);
            return key;
        }
    }
}

AutoPtr<IObject> CRttManager::RemoveListener(
    /* [in] */ Int32 key)
{
    if (key == INVALID_KEY) return NULL;

    {
        AutoLock lock(sListenerMapLock);
        AutoPtr<IInterface> listener;
        sListenerMap->Get(key, (IInterface**)&listener);
        sListenerMap->Remove(key);
        return IObject::Probe(listener);
    }
}

Int32 CRttManager::RemoveListener(
    /* [in] */ IObject* listener)
{
    Int32 key = GetListenerKey(listener);
    if (key == INVALID_KEY) return key; {
        AutoLock lock(sListenerMapLock);
        sListenerMap->Remove(key);
        return key;
    }
}

} // namespace Wifi
} // namespace Droid
} // namespace Elastos
