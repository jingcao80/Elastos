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

#ifndef __ELASTOS_DROID_SERVER_TELECOM_RESPONDVIASMSMANAGER_H__
#define __ELASTOS_DROID_SERVER_TELECOM_RESPONDVIASMSMANAGER_H__

#include "_Elastos.Droid.Server.Telecom.h"
#include "elastos/droid/server/telecom/CallsManagerListenerBase.h"
#include <elastos/core/Object.h>
#include <elastos/core/Thread.h>
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/droid/os/Handler.h>

using Elastos::Core::Thread;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Telecom::IResponse;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

/**
 * Helper class to manage the "Respond via Message" feature for incoming calls.
 */
class RespondViaSmsManager
    : public CallsManagerListenerBase
{
private:
    class SubHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("RespondViaSmsManager::Handler")

        SubHandler(
            /* [in] */ RespondViaSmsManager* host);

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        RespondViaSmsManager* mHost;
    };

    class SubThread
        : public Thread
    {
    public:
        TO_STRING_IMPL("RespondViaSmsManager::SubThread")

        SubThread(
            /* [in] */ RespondViaSmsManager* host,
            /* [in] */ IResponse* response,
            /* [in] */ IContext* context);

        // @Override
        CARAPI Run();

    private:
        RespondViaSmsManager* mHost;
        AutoPtr<IResponse> mResponse;
        AutoPtr<IContext> mContext;
    };

public:
    static CARAPI GetInstance(
        /* [out] */ RespondViaSmsManager** result);

    /**
     * Read the (customizable) canned responses from SharedPreferences,
     * or from defaults if the user has never actually brought up
     * the Settings UI.
     *
     * The interface of this method is asynchronous since it does disk I/O.
     *
     * @param response An object to receive an async reply, which will be called from
     *                 the main thread.
     * @param context The context.
     */
    CARAPI LoadCannedTextMessages(
        /* [in] */ IResponse* response,
        /* [in] */ IContext* context);

    // @Override
    CARAPI OnIncomingCallRejected(
        /* [in] */ ICall* call,
        /* [in] */ Boolean rejectWithMessage,
        /* [in] */ const String& textMessage);

private:
    RespondViaSmsManager();

    CARAPI ShowMessageSentToast(
        /* [in] */ const String& phoneNumber,
        /* [in] */ IContext* context);

    /**
     * Reject the call with the specified message. If message is null this call is ignored.
     */
    CARAPI RejectCallWithMessage(
        /* [in] */ IContext* context,
        /* [in] */ const String& phoneNumber,
        /* [in] */ const String& textMessage,
        /* [in] */ Int64 subId);

private:
    static const Int32 MSG_CANNED_TEXT_MESSAGES_READY;

    static const Int32 MSG_SHOW_SENT_TOAST;

    static AutoPtr<RespondViaSmsManager> sInstance;

    AutoPtr<IHandler> mHandler;
};

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TELECOM_RESPONDVIASMSMANAGER_H__
