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

#ifndef __ELASTOS_DROID_SERVER_TELECOM_MISSEDCALLNOTIFIER_H__
#define __ELASTOS_DROID_SERVER_TELECOM_MISSEDCALLNOTIFIER_H__

#include "_Elastos.Droid.Server.Telecom.h"
#include "elastos/droid/server/telecom/Call.h"
#include "elastos/droid/server/telecom/CallsManagerListenerBase.h"
#include <elastos/core/Object.h>
#include <elastos/droid/content/AsyncQueryHandler.h>
#include <elastos/droid/ext/frameworkext.h>

using Elastos::Droid::App::INotification;
using Elastos::Droid::App::INotificationManager;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::AsyncQueryHandler;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

/**
 * Creates a notification for calls that the user missed (neither answered nor rejected).
 * TODO: Make TelephonyManager.clearMissedCalls call into this class.
 * STOPSHIP: Resolve b/13769374 about moving this class to InCall.
 */
class MissedCallNotifier
    : public CallsManagerListenerBase
{
private:
    class SubAsyncQueryHandler
        : public AsyncQueryHandler
    {
    public:
        SubAsyncQueryHandler(
            /* [in] */ MissedCallNotifier* host);

        CARAPI constructor(
            /* [in] */ IContentResolver* contentResolver);

        // @Override
        CARAPI OnQueryComplete(
            /* [in] */ Int32 token,
            /* [in] */ IInterface* cookie,
            /* [in] */ ICursor* cursor);

    private:
        MissedCallNotifier* mHost;
    };

    class SubCallListenerBase
        : public Call::ListenerBase
    {
    public:
        SubCallListenerBase(
            /* [in] */ MissedCallNotifier* host);

        // @Override
        CARAPI OnCallerInfoChanged(
            /* [in] */ ICall* call);

    private:
        MissedCallNotifier* mHost;
    };

public:
    MissedCallNotifier();

    CARAPI constructor(
        /* [in] */ IContext* context);

    /** {@inheritDoc} */
    // @Override
    CARAPI OnCallStateChanged(
        /* [in] */ ICall* call,
        /* [in] */ Int32 oldState,
        /* [in] */ Int32 newState);

    /** Clears missed call notification and marks the call log's missed calls as read. */
    CARAPI ClearMissedCalls();

    /**
     * Create a system notification for the missed call.
     *
     * @param call The missed call.
     */
    CARAPI ShowMissedCallNotification(
        /* [in] */ ICall* call);

private:
    /** Cancels the "missed call" notification. */
    CARAPI CancelMissedCallNotification();

    /**
     * Returns the name to use in the missed call notification.
     */
    CARAPI GetNameForCall(
        /* [in] */ ICall* call,
        /* [out] */ String* result);

    /**
     * Creates a new pending intent that sends the user to the call log.
     *
     * @return The pending intent.
     */
    CARAPI CreateCallLogPendingIntent(
        /* [out] */ IPendingIntent** result);

    /**
     * Creates an intent to be invoked when the missed call notification is cleared.
     */
    CARAPI CreateClearMissedCallsPendingIntent(
        /* [out] */ IPendingIntent** result);

    /**
     * Creates an intent to be invoked when the user opts to "call back" from the missed call
     * notification.
     *
     * @param handle The handle to call back.
     */
    CARAPI CreateCallBackPendingIntent(
        /* [in] */ IUri* handle,
        /* [out] */ IPendingIntent** result);

    /**
     * Creates an intent to be invoked when the user opts to "send sms" from the missed call
     * notification.
     */
    CARAPI CreateSendSmsFromNotificationPendingIntent(
        /* [in] */ IUri* handle,
        /* [out] */ IPendingIntent** result);

    /**
     * Creates generic pending intent from the specified parameters to be received by
     * {@link TelecomBroadcastReceiver}.
     *
     * @param action The intent action.
     * @param data The intent data.
     */
    CARAPI CreateTelecomPendingIntent(
        /* [in] */ const String& action,
        /* [in] */ IUri* data,
        /* [out] */ IPendingIntent** result);

    /**
     * Configures a notification to emit the blinky notification light.
     *
     */
    static CARAPI ConfigureLedOnNotification(
        /* [in] */ IContext* context,
        /* [in] */ INotification* notification);

    /**
     * Adds the missed call notification on startup if there are unread missed calls.
     */
    CARAPI UpdateOnStartup();

    static CARAPI_(AutoPtr<ArrayOf<String> >) InitCALL_LOG_PROJECTION();

public:
    // notification light default constants
    static const Int32 DEFAULT_COLOR; //White
    static const Int32 DEFAULT_TIME; // 1 second

private:
    static AutoPtr<ArrayOf<String> > CALL_LOG_PROJECTION;

    static const Int32 CALL_LOG_COLUMN_ID;

    static const Int32 CALL_LOG_COLUMN_NUMBER;

    static const Int32 CALL_LOG_COLUMN_NUMBER_PRESENTATION;

    static const Int32 CALL_LOG_COLUMN_DATE;

    static const Int32 CALL_LOG_COLUMN_DURATION;

    static const Int32 CALL_LOG_COLUMN_TYPE;

    static const Int32 MISSED_CALL_NOTIFICATION_ID;

    // 1 second
    AutoPtr<IContext> mContext;

    AutoPtr<INotificationManager> mNotificationManager;

    // Used to track the number of missed calls.
    Int32 mMissedCallCount;
};

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TELECOM_MISSEDCALLNOTIFIER_H__
