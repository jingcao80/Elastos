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

#include "elastos/droid/accessibilityservice/AccessibilityService.h"
#include "elastos/droid/accessibilityservice/CAccessibilityServiceClientWrapper.h"
#include "elastos/droid/view/accessibility/CAccessibilityInteractionClient.h"
#include "elastos/droid/internal/os/CHandlerCaller.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Internal::Os::CHandlerCaller;
using Elastos::Droid::Internal::Os::EIID_IHandlerCallerCallback;
using Elastos::Droid::Content::EIID_IContext;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Droid::View::Accessibility::CAccessibilityInteractionClient;
using Elastos::Droid::View::Accessibility::IAccessibilityInteractionClient;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace AccessibilityService {

//==============================================================================
// IAccessibilityServiceClientWrapper
//==============================================================================
const Int32 IAccessibilityServiceClientWrapper::DO_SET_SET_CONNECTION = 1;
const Int32 IAccessibilityServiceClientWrapper::DO_ON_INTERRUPT = 2;
const Int32 IAccessibilityServiceClientWrapper::DO_ON_ACCESSIBILITY_EVENT = 3;
const Int32 IAccessibilityServiceClientWrapper::DO_ON_GESTURE = 4;
const Int32 IAccessibilityServiceClientWrapper::DO_CLEAR_ACCESSIBILITY_CACHE = 5;
const Int32 IAccessibilityServiceClientWrapper::DO_ON_KEY_EVENT = 6;

CAR_INTERFACE_IMPL_3(IAccessibilityServiceClientWrapper, Object, IIAccessibilityServiceClient,
        IBinder, IHandlerCallerCallback)

IAccessibilityServiceClientWrapper::IAccessibilityServiceClientWrapper()
    : mConnectionId(0)
{}

IAccessibilityServiceClientWrapper::~IAccessibilityServiceClientWrapper()
{}

ECode IAccessibilityServiceClientWrapper::constructor()
{
    return NOERROR;
}

ECode IAccessibilityServiceClientWrapper::constructor(
    /* [in] */ IContext* context,
    /* [in] */ ILooper* looper,
    /* [in] */ IAccessibilityServiceCallbacks* callback)
{
    mCallback = callback;
    CHandlerCaller::New(context, looper, this, TRUE, FALSE,
        (IHandlerCaller**)&mCaller);
    return NOERROR;
}

ECode IAccessibilityServiceClientWrapper::SetConnection(
    /* [in] */ IIAccessibilityServiceConnection* connection,
    /* [in] */ Int32 connectionId)
{
    AutoPtr<IMessage> message;
    mCaller->ObtainMessageIO(DO_SET_SET_CONNECTION, connectionId, connection, (IMessage**)&message);
    return mCaller->SendMessage(message);
}

ECode IAccessibilityServiceClientWrapper::OnInterrupt()
{
    AutoPtr<IMessage> message;
    mCaller->ObtainMessage(DO_ON_INTERRUPT, (IMessage**)&message);
    return mCaller->SendMessage(message);
}

ECode IAccessibilityServiceClientWrapper::OnAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    AutoPtr<IMessage> message;
    mCaller->ObtainMessageO(DO_ON_ACCESSIBILITY_EVENT, event, (IMessage**)&message);
    return mCaller->SendMessage(message);
}

ECode IAccessibilityServiceClientWrapper::OnGesture(
    /* [in] */ Int32 gestureId)
{
    AutoPtr<IMessage> message;
    mCaller->ObtainMessageI(DO_ON_GESTURE, gestureId, (IMessage**)&message);
    return mCaller->SendMessage(message);
}

ECode IAccessibilityServiceClientWrapper::ClearAccessibilityCache()
{
    AutoPtr<IMessage> message;
    mCaller->ObtainMessage(DO_CLEAR_ACCESSIBILITY_CACHE, (IMessage**)&message);
    return mCaller->SendMessage(message);
}

ECode IAccessibilityServiceClientWrapper::OnKeyEvent(
    /* [in] */ IKeyEvent* event,
    /* [in] */ Int32 sequence)
{
    AutoPtr<IMessage> message;
    mCaller->ObtainMessageIO(DO_ON_KEY_EVENT, sequence, event, (IMessage**)&message);
    return mCaller->SendMessage(message);
}

ECode IAccessibilityServiceClientWrapper::ExecuteMessage(
    /* [in] */ IMessage* message)
{
    Int32 what, arg1;
    message->GetWhat(&what);
    message->GetArg1(&arg1);

    switch (what) {
        case DO_ON_ACCESSIBILITY_EVENT : {
            AutoPtr<IInterface> obj;
            message->GetObj((IInterface**)&obj);

            IAccessibilityEvent* event = IAccessibilityEvent::Probe(obj);
            if (event != NULL) {
                AutoPtr<IAccessibilityInteractionClient> client = CAccessibilityInteractionClient::GetInstance();
                client->OnAccessibilityEvent(event);
                mCallback->OnAccessibilityEvent(event);
                // Make sure the event is recycled.
                // try {
                IAccessibilityRecord::Probe(event)->Recycle();
                // } catch (IllegalStateException ise) {
                //             /* ignore - best effort */
                // }
            }
            return NOERROR;
        }

        case DO_ON_INTERRUPT :
            mCallback->OnInterrupt();
            return NOERROR;

        case DO_SET_SET_CONNECTION : {
            AutoPtr<IAccessibilityInteractionClient> client = CAccessibilityInteractionClient::GetInstance();
            mConnectionId = arg1;
            AutoPtr<IInterface> obj;
            message->GetObj((IInterface**)&obj);
            IIAccessibilityServiceConnection* connection = IIAccessibilityServiceConnection::Probe(obj);

            if (connection != NULL) {
                client->AddConnection(mConnectionId, connection);
                mCallback->OnSetConnectionId(mConnectionId);
                mCallback->OnServiceConnected();
            }
            else {
                client->RemoveConnection(mConnectionId);
                client->ClearCache();
                mCallback->OnSetConnectionId(IAccessibilityInteractionClient::NO_ID);
            }
            return NOERROR;
        }

        case DO_ON_GESTURE : {
            Boolean result;
            mCallback->OnGesture(arg1, &result);
            break;
        }
        case DO_CLEAR_ACCESSIBILITY_CACHE : {
            AutoPtr<IAccessibilityInteractionClient> client = CAccessibilityInteractionClient::GetInstance();
            client->ClearCache();
            return NOERROR;
        }

        case DO_ON_KEY_EVENT : {
            AutoPtr<IInterface> obj;
            message->GetObj((IInterface**)&obj);
            AutoPtr<IKeyEvent> event = IKeyEvent::Probe(obj);
            // try {
            AutoPtr<IAccessibilityInteractionClient> client = CAccessibilityInteractionClient::GetInstance();
            AutoPtr<IIAccessibilityServiceConnection> connection;
            client->GetConnection(mConnectionId, (IIAccessibilityServiceConnection**)&connection);
            if (connection != NULL) {
                Boolean result;
                mCallback->OnKeyEvent(event, &result);
                Int32 sequence = arg1;
                // try {
                connection->SetOnKeyEventResult(result, sequence);
                // } catch (RemoteException re) {
                            /* ignore */
                // }
            }
            // } finally {
                    // Make sure the event is recycled.
                    // try {
            IAccessibilityRecord::Probe(event)->Recycle();
                    // } catch (IllegalStateException ise) {
                        /* ignore - best effort */
                    // // }
            // }
            return NOERROR;
        }

        default :
            Logger::W("IAccessibilityServiceClientWrapper", "Unknown message type %d", what);
            return NOERROR;
    }
    return NOERROR;
}

ECode IAccessibilityServiceClientWrapper::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);
    *info = String("IAccessibilityServiceClientWrapper");
    return NOERROR;
}

//==============================================================================
// AccessibilityService::MyAccessibilityServiceCallbacks
//==============================================================================

CAR_INTERFACE_IMPL(AccessibilityService::MyAccessibilityServiceCallbacks, Object, IAccessibilityServiceCallbacks)

AccessibilityService::MyAccessibilityServiceCallbacks::MyAccessibilityServiceCallbacks(
    /* [in] */ AccessibilityService* host)
    : mHost(host)
{
}

ECode AccessibilityService::MyAccessibilityServiceCallbacks::OnAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    return mHost->OnAccessibilityEvent(event);
}

ECode AccessibilityService::MyAccessibilityServiceCallbacks::OnInterrupt()
{
    return mHost->OnInterrupt();
}

ECode AccessibilityService::MyAccessibilityServiceCallbacks::OnServiceConnected()
{
    return mHost->OnServiceConnected();
}

ECode AccessibilityService::MyAccessibilityServiceCallbacks::OnSetConnectionId(
    /* [in] */ Int32 connectionId)
{
    mHost->mConnectionId = connectionId;
    return NOERROR;
}

ECode AccessibilityService::MyAccessibilityServiceCallbacks::OnGesture(
    /* [in] */ Int32 gestureId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return mHost->OnGesture(gestureId, result);
}

ECode AccessibilityService::MyAccessibilityServiceCallbacks::OnKeyEvent(
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return mHost->OnKeyEvent(event, result);
}

//==============================================================================
// AccessibilityService
//==============================================================================

const String AccessibilityService::TAG("AccessibilityService");

CAR_INTERFACE_IMPL(AccessibilityService, Service, IAccessibilityService)

AccessibilityService::AccessibilityService()
    : mConnectionId(0)
{}

AccessibilityService::~AccessibilityService()
{}

ECode AccessibilityService::OnAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    return NOERROR;
}

ECode AccessibilityService::OnInterrupt()
{
    return NOERROR;
}

ECode AccessibilityService::GetWindows(
    /* [out] */ IList** windows)
{
    VALIDATE_NOT_NULL(windows)

    AutoPtr<IAccessibilityInteractionClient> client = CAccessibilityInteractionClient::GetInstance();
    return client->GetWindows(mConnectionId, windows);
}

ECode AccessibilityService::GetRootInActiveWindow(
    /* [out] */ IAccessibilityNodeInfo** info)
{
    VALIDATE_NOT_NULL(info);

    AutoPtr<IAccessibilityInteractionClient> client = CAccessibilityInteractionClient::GetInstance();
    return client->GetRootInActiveWindow(mConnectionId, info);
}

ECode AccessibilityService::PerformGlobalAction(
    /* [in] */ Int32 action,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IAccessibilityInteractionClient> client = CAccessibilityInteractionClient::GetInstance();
    AutoPtr<IIAccessibilityServiceConnection> connection;
    client->GetConnection(mConnectionId, (IIAccessibilityServiceConnection**)&connection);

    if (connection != NULL) {
        // try {
        ECode ec = connection->PerformGlobalAction(action, result);
        if (SUCCEEDED(ec)) return NOERROR;
        Logger::W(TAG, "Error while calling performGlobalAction. 0x%08x", ec);
        // } catch (RemoteException re) {
        //     Log.w(TAG, "Error while calling performGlobalAction", re);
        // }
    }

    *result = FALSE;
    return NOERROR;
}

ECode AccessibilityService::FindFocus(
    /* [in] */ Int32 focus,
    /* [out] */ IAccessibilityNodeInfo** ret)
{
    VALIDATE_NOT_NULL(ret);

    AutoPtr<IAccessibilityInteractionClient> client = CAccessibilityInteractionClient::GetInstance();
    return client->FindFocus(mConnectionId,
            IAccessibilityNodeInfo::ANY_WINDOW_ID,
            IAccessibilityNodeInfo::ROOT_NODE_ID, focus, ret);
}

ECode AccessibilityService::GetServiceInfo(
    /* [out] */ IAccessibilityServiceInfo** info)
{
    VALIDATE_NOT_NULL(info);

    AutoPtr<IAccessibilityInteractionClient> client = CAccessibilityInteractionClient::GetInstance();
    AutoPtr<IIAccessibilityServiceConnection> connection;
    client->GetConnection(mConnectionId, (IIAccessibilityServiceConnection**)&connection);

    if (connection != NULL) {
        // try {
        ECode ec = connection->GetServiceInfo(info);
        if (SUCCEEDED(ec)) return NOERROR;
        Logger::W(TAG, "Error while getting AccessibilityServiceInfo. 0x%08x", ec);
        // } catch (RemoteException re) {
        //     Log.w(TAG, "Error while getting AccessibilityServiceInfo", re);
        // }
    }
    *info = NULL;
    return NOERROR;
}

ECode AccessibilityService::SetServiceInfo(
    /* [in] */ IAccessibilityServiceInfo* info)
{
    mInfo = info;
    SendServiceInfo();
    return NOERROR;
}

ECode AccessibilityService::OnBind(
    /* [in] */ IIntent* intent,
    /* [out] */ IBinder** binder)
{
    VALIDATE_NOT_NULL(binder);
    *binder = NULL;

    AutoPtr<ILooper> looper;
    Service::GetMainLooper((ILooper**)&looper);
    AutoPtr<MyAccessibilityServiceCallbacks> callbacks = new MyAccessibilityServiceCallbacks(this);
    AutoPtr<IIAccessibilityServiceClient> wrapper;
    CAccessibilityServiceClientWrapper::New(this, looper, callbacks,
        (IIAccessibilityServiceClient**)&wrapper);
    *binder = IBinder::Probe(wrapper);
    REFCOUNT_ADD(*binder);
    return NOERROR;
}

ECode AccessibilityService::OnServiceConnected()
{
    return NOERROR;
}

ECode AccessibilityService::OnGesture(
    /* [in] */ Int32 gestureId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

ECode AccessibilityService::OnKeyEvent(
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

void AccessibilityService::SendServiceInfo()
{
    AutoPtr<IAccessibilityInteractionClient> client = CAccessibilityInteractionClient::GetInstance();
    AutoPtr<IIAccessibilityServiceConnection> connection;
    client->GetConnection(mConnectionId, (IIAccessibilityServiceConnection**)&connection);

    if (mInfo != NULL && connection != NULL) {
        // try {
        ECode ec = connection->SetServiceInfo(mInfo);
        if (ec == (ECode)E_REMOTE_EXCEPTION) {
            Logger::W(TAG, "Error while setting AccessibilityServiceInfo. 0x%08x", ec);
        }
        mInfo = NULL;
        client->ClearCache();
        // } catch (RemoteException re) {
        //     Log.w(TAG, "Error while setting AccessibilityServiceInfo", re);
        // }
    }
}

}
}
}
