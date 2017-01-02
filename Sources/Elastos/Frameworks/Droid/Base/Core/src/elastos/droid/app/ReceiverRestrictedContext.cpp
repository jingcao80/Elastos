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

#include "elastos/droid/app/ReceiverRestrictedContext.h"

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL(ReceiverRestrictedContext, ContextWrapper, IReceiverRestrictedContext)

ReceiverRestrictedContext::ReceiverRestrictedContext()
{}

ReceiverRestrictedContext::~ReceiverRestrictedContext()
{}

ECode ReceiverRestrictedContext::constructor(
    /* [in] */ IContext* context)
{
    return ContextWrapper::constructor(context);
}

ECode ReceiverRestrictedContext::RegisterReceiver(
    /* [in] */ IBroadcastReceiver* receiver,
    /* [in] */ IIntentFilter* filter,
    /* [out] */ IIntent** intent)
{
   return RegisterReceiver(receiver, filter, String(NULL), NULL, intent);
}

ECode ReceiverRestrictedContext::RegisterReceiver(
    /* [in] */ IBroadcastReceiver* receiver,
    /* [in] */ IIntentFilter* filter,
    /* [in] */ const String& broadcastPermission,
    /* [in] */ IHandler* scheduler,
    /* [out] */ IIntent** intent)
{
    if (receiver == NULL) {
        // Allow retrieving current sticky broadcast; this is safe since we
        // aren't actually registering a receiver.
        return ContextWrapper::RegisterReceiver(NULL, filter, broadcastPermission
                , scheduler, intent);
    } else {
//         throw new ReceiverCallNotAllowedException(
//                 "BroadcastReceiver components are not allowed to register to receive intents");
        return E_RECEIVER_CALL_NOT_ALLOWED_EXCEPTION;
    }
}

ECode ReceiverRestrictedContext::RegisterReceiverAsUser(
    /* [in] */ IBroadcastReceiver* receiver,
    /* [in] */ IUserHandle* user,
    /* [in] */ IIntentFilter* filter,
    /* [in] */ const String& broadcastPermission,
    /* [in] */ IHandler* scheduler,
    /* [out] */ IIntent** intent)
{
    if (receiver == NULL) {
        // Allow retrieving current sticky broadcast; this is safe since we
        // aren't actually registering a receiver.
        return ContextWrapper::RegisterReceiverAsUser(NULL, user, filter
                , broadcastPermission, scheduler, intent);
    } else {
//         throw new ReceiverCallNotAllowedException(
//                 "BroadcastReceiver components are not allowed to register to receive intents");
        return E_RECEIVER_CALL_NOT_ALLOWED_EXCEPTION;
    }
}

ECode ReceiverRestrictedContext::BindService(
    /* [in] */ IIntent* service,
    /* [in] */ IServiceConnection* conn,
    /* [in] */ Int32 flags,
    /* [out] */ Boolean* succeeded)
{
//    throw new ReceiverCallNotAllowedException(
//            "BroadcastReceiver components are not allowed to bind to services");
    return E_RECEIVER_CALL_NOT_ALLOWED_EXCEPTION;
}

} // namespace App
} // namespace Droid
} // namespace Elastos