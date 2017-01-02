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

#include "elastos/droid/server/telecom/TelecomBroadcastReceiver.h"
#include "elastos/droid/server/telecom/BlacklistCallNotifier.h"
#include "elastos/droid/server/telecom/Call.h"
#include "elastos/droid/server/telecom/CallsManager.h"
#include "elastos/droid/server/telecom/Log.h"
#include "elastos/droid/server/telecom/MissedCallNotifier.h"
#include <Elastos.Droid.Net.h>

using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Internal::Telephony::Utility::IBlacklistUtils;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserHandleHelper;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

CAR_INTERFACE_IMPL(TelecomBroadcastReceiver, BroadcastReceiver, ITelecomBroadcastReceiver)

const String TelecomBroadcastReceiver::ACTION_SEND_SMS_FROM_NOTIFICATION("com.android.server.telecom.ACTION_SEND_SMS_FROM_NOTIFICATION");
const String TelecomBroadcastReceiver::ACTION_CALL_BACK_FROM_NOTIFICATION("com.android.server.telecom.ACTION_CALL_BACK_FROM_NOTIFICATION");
const String TelecomBroadcastReceiver::ACTION_CLEAR_MISSED_CALLS("com.android.server.telecom.ACTION_CLEAR_MISSED_CALLS");
const String TelecomBroadcastReceiver::REMOVE_BLACKLIST("com.android.phone.REMOVE_BLACKLIST");
const String TelecomBroadcastReceiver::EXTRA_NUMBER("number");
const String TelecomBroadcastReceiver::EXTRA_TYPE("type");
const String TelecomBroadcastReceiver::EXTRA_FROM_NOTIFICATION("fromNotification");

ECode TelecomBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    Log::V("TelecomBroadcastReceiver", "Action received: %s.", action.string());
    AutoPtr<MissedCallNotifier> missedCallNotifier;
    CallsManager::GetInstance()->GetMissedCallNotifier((MissedCallNotifier**)&missedCallNotifier);
    // Send an SMS from the missed call notification.
    if (ACTION_SEND_SMS_FROM_NOTIFICATION.Equals(action)) {
        // Close the notification shade and the notification itself.
        CloseSystemDialogs(context);
        missedCallNotifier->ClearMissedCalls();
        AutoPtr<IIntent> callIntent;
        AutoPtr<IUri> data;
        intent->GetData((IUri**)&data);
        CIntent::New(IIntent::ACTION_SENDTO, data, (IIntent**)&callIntent);
        callIntent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
        context->StartActivity(callIntent);
    // Call back recent caller from the missed call notification.
    } else if (ACTION_CALL_BACK_FROM_NOTIFICATION.Equals(action)) {
        // Close the notification shade and the notification itself.
        CloseSystemDialogs(context);
        missedCallNotifier->ClearMissedCalls();
        AutoPtr<IIntent> callIntent;
        AutoPtr<IUri> data;
        intent->GetData((IUri**)&data);
        CIntent::New(IIntent::ACTION_CALL_PRIVILEGED, data, (IIntent**)&callIntent);
        callIntent->SetFlags(
                IIntent::FLAG_ACTIVITY_NEW_TASK | IIntent::FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS);
        context->StartActivity(callIntent);
    // Clear the missed call notification and call log entries.
    } else if (ACTION_CLEAR_MISSED_CALLS.Equals(action)) {
        missedCallNotifier->ClearMissedCalls();
    }  else if (ACTION_CLEAR_BLACKLISTED_CALLS.Equals(action)) {
        AutoPtr<BlacklistCallNotifier> bcn;
        CallsManager::GetInstance()->GetBlacklistCallNotifier((BlacklistCallNotifier**)&bcn);
        assert(0 && "TODO IBlacklistUtils");
        // bcn->CancelBlacklistedNotification(IBlacklistUtils::BLOCK_CALLS);
    } else if (ACTION_CLEAR_BLACKLISTED_MESSAGES.Equals(action)) {
        AutoPtr<BlacklistCallNotifier> bcn;
        CallsManager::GetInstance()->GetBlacklistCallNotifier((BlacklistCallNotifier**)&bcn);
        assert(0 && "TODO IBlacklistUtils");
        // bcn->CancelBlacklistedNotification(IBlacklistUtils::BLOCK_MESSAGES);
    } else if (action.Equals(REMOVE_BLACKLIST)) {
        Boolean b;
        intent->GetBooleanExtra(EXTRA_FROM_NOTIFICATION, FALSE, &b);
        if (b) {
            // Dismiss the notification that brought us here
            Int32 blacklistType;
            intent->GetInt32Extra(EXTRA_TYPE, 0, &blacklistType);
            AutoPtr<BlacklistCallNotifier> bcn;
            CallsManager::GetInstance()->GetBlacklistCallNotifier((BlacklistCallNotifier**)&bcn);
            bcn->CancelBlacklistedNotification(blacklistType);
            String s;
            intent->GetStringExtra(EXTRA_NUMBER, &s);
            assert(0 && "TODO BlacklistUtils");
            // BlacklistUtils::AddOrUpdate(context, s, 0, blacklistType);
        }
    } else if (ACTION_REJECTED_SMS.Equals(action)) {
        Boolean b;
        intent->GetBooleanExtra(String("blacklisted"), FALSE, &b);
        if (!b) {
            return NOERROR;
        }
        String sender;
        intent->GetStringExtra(String("sender"), &sender);
        Int64 timestamp;
        intent->GetInt64Extra(String("timestamp"), 0, &timestamp);
        Int32 matchType;
        intent->GetInt32Extra(String("blacklistMatchType"), -1, &matchType);
        AutoPtr<BlacklistCallNotifier> bcn;
        CallsManager::GetInstance()->GetBlacklistCallNotifier((BlacklistCallNotifier**)&bcn);
        bcn->NotifyBlacklistedMessage(sender, timestamp, matchType);
    }
    return NOERROR;
}

ECode TelecomBroadcastReceiver::CloseSystemDialogs(
    /* [in] */ IContext* context)
{
    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_CLOSE_SYSTEM_DIALOGS, (IIntent**)&intent);
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    AutoPtr<IUserHandle> all;
    helper->GetALL((IUserHandle**)&all);
    context->SendBroadcastAsUser(intent, all);
    return NOERROR;
}

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos
