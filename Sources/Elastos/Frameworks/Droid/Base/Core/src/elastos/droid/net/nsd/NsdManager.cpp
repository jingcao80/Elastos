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

#include "elastos/droid/net/nsd/NsdManager.h"
#include "elastos/droid/net/nsd/CNsdServiceInfo.h"
#include "elastos/droid/internal/utility/CAsyncChannel.h"
#include "elastos/droid/os/CHandlerThread.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/os/Looper.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/utility/CSparseArray.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/Thread.h>
#include <elastos/utility/etl/List.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/logging/Slogger.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::Utility::CAsyncChannel;
using Elastos::Droid::Internal::Utility::IAsyncChannel;
using Elastos::Droid::Internal::Utility::IProtocol;
using Elastos::Droid::Os::CHandlerThread;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IMessenger;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Utility::CSparseArray;
using Elastos::Droid::Utility::ILog;

using Elastos::Core::CObject;
using Elastos::Core::IThread;
using Elastos::Core::Thread;
using Elastos::Utility::Concurrent::CCountDownLatch;
using Elastos::Utility::Concurrent::ICountDownLatch;
using Elastos::Utility::Etl::List;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Net {
namespace Nsd {

CAR_INTERFACE_IMPL(NsdManager, Object, INsdManager)

const String NsdManager::TAG("NsdManager");
const Int32 NsdManager::BASE = IProtocol::BASE_NSD_MANAGER;
const Int32 NsdManager::INVALID_LISTENER_KEY = 0;
const Int32 NsdManager::BUSY_LISTENER_KEY = -1;

NsdManager::NsdManager()
    : mListenerKey(1)
{
    CSparseArray::New((ISparseArray**)&mListenerMap);
    CSparseArray::New((ISparseArray**)&mServiceMap);
    CAsyncChannel::New((IAsyncChannel**)&mAsyncChannel);
    CCountDownLatch::New(1, (ICountDownLatch**)&mConnected);
}

ECode NsdManager::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IINsdManager* service)
{
    mService = service;
    mContext = context;
    return Init();
}

ECode NsdManager::PutListener(
    /* [in] */ IInterface* listener,
    /* [in] */ INsdServiceInfo* s,
    /* [out] */ Int32* result)
{
    if (listener == NULL) {
        Slogger::E(TAG, "listener cannot be null");
        return INVALID_LISTENER_KEY;
    }

    Int32 key = 0;
    {    AutoLock syncLock(mMapLock);
        Int32 valueIndex;
        mListenerMap->IndexOfValue(listener, &valueIndex);
        if (valueIndex != -1) {
            return BUSY_LISTENER_KEY;
        }
        do {
            key = mListenerKey++;
        } while (key == INVALID_LISTENER_KEY);

        mListenerMap->Put(key, listener);

        mServiceMap->Put(key, s);
    }
    *result = key;
    return NOERROR;
}


ECode NsdManager::GetListener(
    /* [in] */ Int32 key,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    if (key == INVALID_LISTENER_KEY) {
        return NOERROR;
    }

    {    AutoLock syncLock(mMapLock);
        mListenerMap->Get(key, result);
    }

    return NOERROR;
}


ECode NsdManager::GetNsdService(
    /* [in] */ Int32 key,
    /* [out] */ INsdServiceInfo** result)
{
    VALIDATE_NOT_NULL(result)

    {    AutoLock syncLock(mMapLock);
        AutoPtr<IInterface> obj;
        mServiceMap->Get(key, (IInterface**)&obj);
        *result = INsdServiceInfo::Probe(obj);
        REFCOUNT_ADD(*result)
    }
    return NOERROR;
}


ECode NsdManager::RemoveListener(
    /* [in] */ Int32 key)
{
    if (key == INVALID_LISTENER_KEY) return NOERROR;

    {    AutoLock syncLock(mMapLock);
        mListenerMap->Remove(key);
        mServiceMap->Remove(key);
    }
    return NOERROR;
}


ECode NsdManager::GetListenerKey(
    /* [in] */ IInterface* listener,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    {    AutoLock syncLock(mMapLock);
        Int32 valueIndex;
        mListenerMap->IndexOfValue(listener, &valueIndex);
        if (valueIndex != -1) {
            mListenerMap->KeyAt(valueIndex, result);
            return NOERROR;
        }
    }

    *result = INVALID_LISTENER_KEY;
    return NOERROR;
}


ECode NsdManager::GetNsdServiceInfoType(
    /* [in] */ INsdServiceInfo* s,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    if (s == NULL) {
        *result = "?";
        return NOERROR;
    }
    return s->GetServiceType(result);
}


ECode NsdManager::Init()
{
    AutoPtr<IMessenger> messenger;
    GetMessenger((IMessenger**)&messenger);
    if (messenger == NULL)
    {
        Slogger::E(TAG, "Failed to initialize");
        return E_RUNTIME_EXCEPTION;
    }

    AutoPtr<IHandlerThread> t;
    CHandlerThread::New(String("NsdManager"), (IHandlerThread**)&t);
    IThread::Probe(t)->Start();

    AutoPtr<ILooper> looper;
    t->GetLooper((ILooper**)&looper);
    mHandler = new ServiceHandler(looper, this);
    mAsyncChannel->Connect(mContext, mHandler, messenger);
    // try {
    ECode ec = mConnected->Await();
    if (FAILED(ec))
        Slogger::E(TAG, "interrupted wait at init");
    // } catch (InterruptedException e) {
    //    Log.e(TAG, "interrupted wait at init");
    // }
    return NOERROR;
}


ECode NsdManager::RegisterService(
    /* [in] */ INsdServiceInfo* serviceInfo,
    /* [in] */ Int32 protocolType,
    /* [in] */ INsdManagerRegistrationListener* listener)
{
    String name, type;
    serviceInfo->GetServiceName(&name);
    serviceInfo->GetServiceType(&type);
    if (name.IsNullOrEmpty() || type.IsNullOrEmpty()) {
        Slogger::E(TAG, "Service name or type cannot be empty");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 port;
    serviceInfo->GetPort(&port);
    if (port <= 0) {
        Slogger::E(TAG, "Invalid port number");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (listener == NULL) {
        Slogger::E(TAG, "listener cannot be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (protocolType != PROTOCOL_DNS_SD) {
        Slogger::E(TAG, "Unsupported protocol");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 key;
    PutListener(listener, serviceInfo, &key);
    if (key == BUSY_LISTENER_KEY) {
        Logger::E(TAG, "listener already in use");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mAsyncChannel->SendMessage(INsdManager::REGISTER_SERVICE, 0, key, serviceInfo);
    return NOERROR;
}


ECode NsdManager::UnregisterService(
    /* [in] */ INsdManagerRegistrationListener* listener)
{
    if (listener == NULL) {
        Slogger::E(TAG, "listener cannot be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 id;
    GetListenerKey(listener->Probe(EIID_IInterface), &id);
    if (id == INVALID_LISTENER_KEY) {
        Slogger::E(TAG, "listener not registered");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mAsyncChannel->SendMessage(INsdManager::UNREGISTER_SERVICE, 0, id);
    return NOERROR;
}


ECode NsdManager::DiscoverServices(
    /* [in] */ const String& serviceType,
    /* [in] */ Int32 protocolType,
    /* [in] */ INsdManagerDiscoveryListener* listener)
{
    if (listener == NULL) {
        Slogger::E(TAG, "listener cannot be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (serviceType.IsNullOrEmpty()) {
        Slogger::E(TAG, "Service type cannot be empty");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (protocolType != PROTOCOL_DNS_SD) {
        Slogger::E(TAG, "Unsupported protocol");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<INsdServiceInfo> s;
    CNsdServiceInfo::New((INsdServiceInfo**)&s);
    s->SetServiceType(serviceType);
    Int32 key;
    PutListener(listener, s, &key);
    if (key == BUSY_LISTENER_KEY) {
        Logger::E(TAG, "listener already in use");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mAsyncChannel->SendMessage(INsdManager::DISCOVER_SERVICES, 0, key, s);
    return NOERROR;
}


ECode NsdManager::StopServiceDiscovery(
    /* [in] */ INsdManagerDiscoveryListener* listener)
{
    if (listener == NULL) {
        Slogger::E(TAG, "listener cannot be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 id;
    GetListenerKey(listener, &id);
    if (id == INVALID_LISTENER_KEY) {
        Slogger::E(TAG, "service discovery not active on listener");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mAsyncChannel->SendMessage(INsdManager::STOP_DISCOVERY, 0, id);
    return NOERROR;
}


ECode NsdManager::ResolveService(
    /* [in] */ INsdServiceInfo* serviceInfo,
    /* [in] */ INsdManagerResolveListener* listener)
{
    if (listener == NULL) {
        Slogger::E(TAG, "listener cannot be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    String name, type;
    serviceInfo->GetServiceName(&name);
    serviceInfo->GetServiceType(&type);

    if (name.IsNullOrEmpty() || type.IsNullOrEmpty()) {
        Slogger::E(TAG, "Service name or type cannot be empty");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 key;
    PutListener(listener, serviceInfo, &key);
    if (key == BUSY_LISTENER_KEY) {
        Logger::E(TAG, "listener already in use");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mAsyncChannel->SendMessage(INsdManager::RESOLVE_SERVICE, 0, key, serviceInfo);
    return NOERROR;
}


ECode NsdManager::SetEnabled(
    /* [in] */ Boolean enabled)
{
    // try {
    return mService->SetEnabled(enabled);
    // } catch (RemoteException e) { }
}


ECode NsdManager::GetMessenger(
    /* [out] */ IMessenger** result)
{
    VALIDATE_NOT_NULL(result)

    // try {
    AutoPtr<IMessenger> messenger;
    ECode ec = mService->GetMessenger((IMessenger**)&messenger);
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        *result = NULL;
        return NOERROR;
    }
    *result = messenger;
    REFCOUNT_ADD(*result);
    return NOERROR;
    // } catch (RemoteException e) {
    //     return NULL;
    // }
}


//===========================================================================
// NsdManager::ServiceHandler
//===========================================================================
NsdManager::ServiceHandler::ServiceHandler(
    /* [in] */ ILooper* looper,
    /* [in] */ NsdManager* host)
    : Handler(looper)
    , mHost(host)
{}

ECode NsdManager::ServiceHandler::HandleMessage(
    /* [in] */ IMessage* message)
{
    VALIDATE_NOT_NULL(message);

    Int32 what, arg1, arg2;
    message->GetWhat(&what);
    message->GetArg1(&arg1);
    message->GetArg2(&arg2);

    // Slogger::D("CNsdManager::ServiceHandler", " >> HandleMessage %s", CodeToString(what).string());

    switch (what) {
        case IAsyncChannel::CMD_CHANNEL_HALF_CONNECTED:
            mHost->mAsyncChannel->SendMessage(IAsyncChannel::CMD_CHANNEL_FULL_CONNECTION);
            return NOERROR;
        case IAsyncChannel::CMD_CHANNEL_FULLY_CONNECTED:
            mHost->mConnected->CountDown();
            return NOERROR;
        case IAsyncChannel::CMD_CHANNEL_DISCONNECTED:
            Slogger::E("CNsdManager::ServiceHandler", "Channel lost");
            return NOERROR;
        default:
            break;
        }
        AutoPtr<IInterface> listener;
        mHost->GetListener(arg2, (IInterface**)&listener);
        if (NULL == listener) {
            Slogger::D("CNsdManager::ServiceHandler", "Stale key %d", arg2);
            return NOERROR;
        }
        AutoPtr<INsdServiceInfo> ns;
        mHost->GetNsdService(arg2, (INsdServiceInfo**)&ns);
        switch (what) {
        case INsdManager::DISCOVER_SERVICES_STARTED: {
            AutoPtr<IInterface> obj;
            message->GetObj((IInterface**)&obj);
            AutoPtr<INsdServiceInfo> info = INsdServiceInfo::Probe(obj);
            String s;
            mHost->GetNsdServiceInfoType(info, &s);
            AutoPtr<INsdManagerDiscoveryListener> dl = INsdManagerDiscoveryListener::Probe(listener);
            dl->OnDiscoveryStarted(s);
            break;
        }
        case INsdManager::DISCOVER_SERVICES_FAILED: {
            mHost->RemoveListener(arg2);
            AutoPtr<INsdManagerDiscoveryListener> dl = INsdManagerDiscoveryListener::Probe(listener);
            String type;
            mHost->GetNsdServiceInfoType(ns, &type);
            dl->OnStartDiscoveryFailed(type, arg1);
            break;
        }
        case INsdManager::SERVICE_FOUND: {
            AutoPtr<IInterface> obj;
            message->GetObj((IInterface**)&obj);
            INsdServiceInfo* info = INsdServiceInfo::Probe(obj);
            INsdManagerDiscoveryListener* dl = INsdManagerDiscoveryListener::Probe(listener);
            dl->OnServiceFound(info);
            break;
        }
        case INsdManager::SERVICE_LOST: {
            AutoPtr<IInterface> obj;
            message->GetObj((IInterface**)&obj);
            INsdServiceInfo* info = INsdServiceInfo::Probe(obj);
            INsdManagerDiscoveryListener* dl = INsdManagerDiscoveryListener::Probe(listener);
            dl->OnServiceLost(info);
            break;
        }
        case INsdManager::STOP_DISCOVERY_FAILED: {
            mHost->RemoveListener(arg2);
            String type;
            mHost->GetNsdServiceInfoType(ns, &type);
            INsdManagerDiscoveryListener::Probe(listener)->OnStopDiscoveryFailed(type, arg1);
            break;
        }
        case INsdManager::STOP_DISCOVERY_SUCCEEDED: {
            mHost->RemoveListener(arg2);
            String type;
            mHost->GetNsdServiceInfoType(ns, &type);
            INsdManagerDiscoveryListener::Probe(listener)->OnDiscoveryStopped(type);
            break;
        }
        case INsdManager::REGISTER_SERVICE_FAILED: {
            mHost->RemoveListener(arg2);
            INsdManagerRegistrationListener::Probe(listener)->OnRegistrationFailed(ns, arg1);
            break;
        }
        case INsdManager::REGISTER_SERVICE_SUCCEEDED: {
            AutoPtr<IInterface> obj;
            message->GetObj((IInterface**)&obj);
            INsdServiceInfo* info = INsdServiceInfo::Probe(obj);
            INsdManagerRegistrationListener* dl = INsdManagerRegistrationListener::Probe(listener);
            assert(dl != NULL);
            dl->OnServiceRegistered(info);
            break;
        }
        case INsdManager::UNREGISTER_SERVICE_FAILED: {
            mHost->RemoveListener(arg2);
            INsdManagerRegistrationListener::Probe(listener)->OnUnregistrationFailed(ns, arg1);
            break;
        }
        case INsdManager::UNREGISTER_SERVICE_SUCCEEDED: {
            mHost->RemoveListener(arg2);
            INsdManagerRegistrationListener::Probe(listener)->OnServiceUnregistered(ns);
            break;
        }
        case INsdManager::RESOLVE_SERVICE_FAILED: {
            mHost->RemoveListener(arg2);
            INsdManagerResolveListener::Probe(listener)->OnResolveFailed(ns, arg1);
            break;
        }
        case INsdManager::RESOLVE_SERVICE_SUCCEEDED: {
            mHost->RemoveListener(arg2);
            AutoPtr<IInterface> obj;
            message->GetObj((IInterface**)&obj);
            INsdServiceInfo* info = INsdServiceInfo::Probe(obj);
            INsdManagerResolveListener* dl = INsdManagerResolveListener::Probe(listener);
            dl->OnServiceResolved(info);
        }

        default:
            Slogger::D("CNsdManager::ServiceHandler", "Ignored %d", what);
            break;
    }

    return NOERROR;
}

} // namespace Nsd
} // namespace Net
} // namespace Droid
} // namespace Elastos
