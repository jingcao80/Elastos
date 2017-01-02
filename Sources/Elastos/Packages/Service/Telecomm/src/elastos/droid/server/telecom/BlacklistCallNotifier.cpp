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

#include "elastos/droid/server/telecom/BlacklistCallNotifier.h"
#include "R.h"
#include "elastos/droid/server/telecom/Blacklist.h"
#include "elastos/droid/server/telecom/Log.h"
#include "elastos/droid/server/telecom/TelecomBroadcastReceiver.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Content.h>
#include <elastos/droid/text/TextUtils.h>
#include <elastos/droid/text/format/DateUtils.h>
#include <elastos/core/StringUtils.h>


using Elastos::Droid::App::CNotificationBuilder;
using Elastos::Droid::App::CNotificationInboxStyle;
using Elastos::Droid::App::CPendingIntentHelper;
using Elastos::Droid::App::INotification;
using Elastos::Droid::App::INotificationBuilder;
using Elastos::Droid::App::INotificationInboxStyle;
using Elastos::Droid::App::INotificationManager;
using Elastos::Droid::App::INotificationStyle;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::App::IPendingIntentHelper;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Text::CSpannableStringBuilder;
using Elastos::Droid::Text::ISpannable;
using Elastos::Droid::Text::ISpannableStringBuilder;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Text::Format::DateUtils;
using Elastos::Droid::Text::Format::IDateUtils;
using Elastos::Droid::Text::Style::CRelativeSizeSpan;
using Elastos::Core::CInteger32;
using Elastos::Core::IAppendable;
using Elastos::Core::ICharSequence;
using Elastos::Core::IInteger32;
using Elastos::Core::StringUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IIterator;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

//=============================================================================
// BlacklistCallNotifier::BlacklistedItemInfo
//=============================================================================
BlacklistCallNotifier::BlacklistedItemInfo::BlacklistedItemInfo(
    /* [in] */ const String& number,
    /* [in] */ Int64 date,
    /* [in] */ Int32 matchType)
{
    mNumber = number;
    mDate = date;
    mMatchType = matchType;
}

//=============================================================================
// BlacklistCallNotifier
//=============================================================================
const Boolean BlacklistCallNotifier::DEBUG = FALSE;
AutoPtr<IRelativeSizeSpan> BlacklistCallNotifier::TIME_SPAN = InitTIME_SPAN();
const Int32 BlacklistCallNotifier::BLACKLISTED_CALL_NOTIFICATION = 7;
const Int32 BlacklistCallNotifier::BLACKLISTED_MESSAGE_NOTIFICATION = 8;

ECode BlacklistCallNotifier::constructor(
    /* [in] */ IContext* context)
{
    CArrayList::New((IArrayList**)&mBlacklistedCalls);
    CArrayList::New((IArrayList**)&mBlacklistedMessages);
    mContext = context;
    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::NOTIFICATION_SERVICE, (IInterface**)&obj);
    mNotificationManager = INotificationManager::Probe(obj);
    return NOERROR;
}

ECode BlacklistCallNotifier::OnCallStateChanged(
    /* [in] */ ICall* call,
    /* [in] */ Int32 oldState,
    /* [in] */ Int32 newState)
{
    return NOERROR;
}

ECode BlacklistCallNotifier::NotifyBlacklistedCall(
    /* [in] */ const String& number,
    /* [in] */ Int64 date,
    /* [in] */ Int32 matchType)
{
    return NotifyBlacklistedItem(number, date, matchType, BLACKLISTED_CALL_NOTIFICATION);
}

ECode BlacklistCallNotifier::NotifyBlacklistedMessage(
    /* [in] */ const String& number,
    /* [in] */ Int64 date,
    /* [in] */ Int32 matchType)
{
    return NotifyBlacklistedItem(number, date, matchType, BLACKLISTED_MESSAGE_NOTIFICATION);
}

ECode BlacklistCallNotifier::NotifyBlacklistedItem(
    /* [in] */ const String& number,
    /* [in] */ Int64 date,
    /* [in] */ Int32 matchType,
    /* [in] */ Int32 notificationId)
{
    assert(0 && "TODO: BlacklistUtils");
    // if (!BlacklistUtils::IsBlacklistNotifyEnabled(mContext)) {
    //     return NOERROR;
    // }
    if (DEBUG) Log::D("BlacklistCallNotifier", "notifyBlacklistedItem(). number: %s, match type: %d, date: %lld, type: %d", number.string(), matchType, date, notificationId);

    AutoPtr<IArrayList> items = notificationId == BLACKLISTED_CALL_NOTIFICATION
            ? mBlacklistedCalls : mBlacklistedMessages;
    AutoPtr<IPendingIntent> clearIntent;
    if (notificationId == BLACKLISTED_CALL_NOTIFICATION) {
        CreateClearBlacklistedCallsIntent((IPendingIntent**)&clearIntent);
    }
    else {
        CreateClearBlacklistedMessagesIntent((IPendingIntent**)&clearIntent);
    }
    Int32 iconDrawableResId = notificationId == BLACKLISTED_CALL_NOTIFICATION
            ? R::drawable::ic_block_contact_holo_dark : R::drawable::ic_block_message_holo_dark;
    // Keep track of the call/message, keeping list sorted from newest to oldest
    items->Add(0, TO_IINTERFACE(new BlacklistedItemInfo(number, date, matchType)));
    // Get the intent to open Blacklist settings if user taps on content ready
    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_MAIN, (IIntent**)&intent);
    intent->SetClassName(String("Elastos.Droid.Settings"),
            String("Elastos.Droid.Settings.Blacklist.CBlacklistSettings"));
    AutoPtr<IPendingIntentHelper> pendingIntentHelper;
    CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&pendingIntentHelper);
    AutoPtr<IPendingIntent> blSettingsIntent;
    pendingIntentHelper->GetActivity(mContext, 0, intent, 0, (IPendingIntent**)&blSettingsIntent);
    // Start building the notification
    AutoPtr<INotificationBuilder> builder;
    CNotificationBuilder::New(mContext, (INotificationBuilder**)&builder);
    String str;
    mContext->GetString(R::string::blacklist_title, &str);
    builder->SetSmallIcon(iconDrawableResId);
    builder->SetContentIntent(blSettingsIntent);
    builder->SetAutoCancel(TRUE);
    builder->SetContentTitle(StringUtils::ParseCharSequence(str));
    builder->SetWhen(date);
    builder->SetDeleteIntent(clearIntent);
    // Add the 'Remove block' notification action only for MATCH_LIST items since
    // MATCH_REGEX and MATCH_PRIVATE items does not have an associated specific number
    // to unblock, and MATCH_UNKNOWN unblock for a single number does not make sense.
    Boolean addUnblockAction = TRUE;
    Int32 size;
    items->GetSize(&size);
    if (size == 1) {
        Int32 messageResId;
        assert(0 && "TODO: BlacklistUtils");
        // switch (matchType) {
        //     case BlacklistUtils.MATCH_PRIVATE:
        //         messageResId = notificationId == BLACKLISTED_CALL_NOTIFICATION
        //                 ? R::string::blacklist_call_notification_private_number
        //                 : R::string::blacklist_message_notification_private_number;
        //         break;
        //     case BlacklistUtils.MATCH_UNKNOWN:
        //         messageResId = notificationId == BLACKLISTED_CALL_NOTIFICATION
        //                 ? R::string::blacklist_call_notification_unknown_number
        //                 : R::string::blacklist_message_notification_unknown_number;
        //         break;
        //     default:
        //         messageResId = notificationId == BLACKLISTED_CALL_NOTIFICATION
        //                 ? R::string::blacklist_call_notification
        //                 : R::string::blacklist_message_notification;
        //         break;
        // }
        AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
        args->Set(0, StringUtils::ParseCharSequence(number));
        String s;
        mContext->GetString(messageResId, args, &s);
        builder->SetContentText(StringUtils::ParseCharSequence(s));
        assert(0 && "TODO: BlacklistUtils");
        // if (matchType != IBlacklistUtils::MATCH_LIST) {
        //     addUnblockAction = FALSE;
        // }
    } else {
        Int32 messageResId = notificationId == BLACKLISTED_CALL_NOTIFICATION
                ? R::string::blacklist_call_notification_multiple
                : R::string::blacklist_message_notification_multiple;
        Int32 size;
        items->GetSize(&size);
        AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
        AutoPtr<IInteger32> iSize;
        CInteger32::New(size, (IInteger32**)&iSize);
        args->Set(0, iSize);
        String message;
        mContext->GetString(messageResId, args, &message);
        builder->SetContentText(StringUtils::ParseCharSequence(message));
        builder->SetNumber(size);
        AutoPtr<INotificationInboxStyle> style;
        CNotificationInboxStyle::New(builder, (INotificationInboxStyle**)&style);
        AutoPtr<IIterator> it;
        items->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<BlacklistedItemInfo> info = (BlacklistedItemInfo*) IObject::Probe(obj);
            // Takes care of displaying "Private" instead of an empty string
            String str;
            mContext->GetString(R::string::blacklist_notification_list_private, &str);
            String numberString = TextUtils::IsEmpty(info->mNumber)
                    ? str : info->mNumber;
            AutoPtr<ICharSequence> cs;
            FormatSingleCallLine(numberString, info->mDate, (ICharSequence**)&cs);
            style->AddLine(cs);
            assert(0 && "TODO: BlacklistUtils");
            // if (!TextUtils::Equals(number, info->mNumber)) {
            //     addUnblockAction = FALSE;
            // } else if (info->mMatchType != IBlacklistUtils::MATCH_LIST) {
            //     addUnblockAction = FALSE;
            // }
        }
        style->SetBigContentTitle(StringUtils::ParseCharSequence(message));
        style->SetSummaryText(StringUtils::ParseCharSequence(String(" ")));
        builder->SetStyle(INotificationStyle::Probe(style));
    }
    if (addUnblockAction) {
        Int32 actionDrawableResId = notificationId == BLACKLISTED_CALL_NOTIFICATION
                ? R::drawable::ic_unblock_contact_holo_dark
                : R::drawable::ic_unblock_message_holo_dark;
        assert(0 && "TODO: BlacklistUtils");
        // Int32 unblockType = notificationId == BLACKLISTED_CALL_NOTIFICATION
        //         ? IBlacklistUtils::BLOCK_CALLS : BlacklistUtils.BLOCK_MESSAGES;
        // PendingIntent action = GetUnblockNumberFromNotificationPendingIntent(
        //         mContext, number, unblockType);
        // builder->AddAction(actionDrawableResId,
        //         mContext->GetString(R::string::unblock_number), action);
    }
    AutoPtr<INotification> notification;
    builder->GetNotification((INotification**)&notification);
    mNotificationManager->Notify(notificationId, notification);
    return NOERROR;
}

ECode BlacklistCallNotifier::CreateClearBlacklistedCallsIntent(
    /* [out] */ IPendingIntent** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IIntent> intent;
    CIntent::New(mContext, ECLSID_CTelecomBroadcastReceiver, (IIntent**)&intent);
    intent->SetAction(ITelecomBroadcastReceiver::ACTION_CLEAR_BLACKLISTED_CALLS);
    AutoPtr<IPendingIntentHelper> pendingIntentHelper;
    CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&pendingIntentHelper);
    return pendingIntentHelper->GetService(mContext, 0, intent, 0, result);
}

ECode BlacklistCallNotifier::CreateClearBlacklistedMessagesIntent(
    /* [out] */ IPendingIntent** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IIntent> intent;
    CIntent::New(mContext, ECLSID_CTelecomBroadcastReceiver, (IIntent**)&intent);
    intent->SetAction(ITelecomBroadcastReceiver::ACTION_CLEAR_BLACKLISTED_MESSAGES);
    AutoPtr<IPendingIntentHelper> pendingIntentHelper;
    CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&pendingIntentHelper);
    return pendingIntentHelper->GetService(mContext, 0, intent, 0, result);
}

ECode BlacklistCallNotifier::CancelBlacklistedNotification(
    /* [in] */ Int32 type)
{
    assert(0 && "TODO: BlacklistUtils");
    // if ((type & IBlacklistUtils::BLOCK_CALLS) != 0) {
    //     mBlacklistedCalls->Clear();
    //     mNotificationManager->Cancel(BLACKLISTED_CALL_NOTIFICATION);
    // }
    // if ((type & IBlacklistUtils::BLOCK_MESSAGES) != 0) {
    //     mBlacklistedMessages->Clear();
    //     mNotificationManager->Cancel(BLACKLISTED_MESSAGE_NOTIFICATION);
    // }
    return NOERROR;
}

ECode BlacklistCallNotifier::FormatSingleCallLine(
    /* [in] */ const String& caller,
    /* [in] */ Int64 date,
    /* [out] */ ICharSequence** result)
{
    VALIDATE_NOT_NULL(result)

    Int32 flags = IDateUtils::FORMAT_SHOW_TIME;
    if (!DateUtils::IsToday(date)) {
        flags |= IDateUtils::FORMAT_SHOW_WEEKDAY;
    }
    AutoPtr<ISpannableStringBuilder> lineBuilder;
    CSpannableStringBuilder::New((ISpannableStringBuilder**)&lineBuilder);
    IAppendable::Probe(lineBuilder)->Append(StringUtils::ParseCharSequence(caller));
    IAppendable::Probe(lineBuilder)->Append(StringUtils::ParseCharSequence(String("  ")));
    Int32 timeIndex;
    ICharSequence::Probe(lineBuilder)->GetLength(&timeIndex);
    IAppendable::Probe(lineBuilder)->Append(StringUtils::ParseCharSequence(DateUtils::FormatDateTime(mContext, date, flags)));
    Int32 length;
    ICharSequence::Probe(lineBuilder)->GetLength(&length);
    ISpannable::Probe(lineBuilder)->SetSpan(TIME_SPAN, timeIndex, length, 0);
    *result = ICharSequence::Probe(lineBuilder);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode BlacklistCallNotifier::GetUnblockNumberFromNotificationPendingIntent(
    /* [in] */ IContext* context,
    /* [in] */ const String& number,
    /* [in] */ Int32 type,
    /* [out] */ IPendingIntent** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IIntent> intent;
    CIntent::New(TelecomBroadcastReceiver::REMOVE_BLACKLIST, (IIntent**)&intent);
    intent->PutExtra(TelecomBroadcastReceiver::EXTRA_NUMBER, number);
    intent->PutExtra(TelecomBroadcastReceiver::EXTRA_FROM_NOTIFICATION, TRUE);
    intent->PutExtra(TelecomBroadcastReceiver::EXTRA_TYPE, type);
    AutoPtr<IPendingIntentHelper> pendingIntentHelper;
    CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&pendingIntentHelper);
    return pendingIntentHelper->GetBroadcast(context, 0, intent, IPendingIntent::FLAG_UPDATE_CURRENT, result);
}

AutoPtr<IRelativeSizeSpan> BlacklistCallNotifier::InitTIME_SPAN()
{
    AutoPtr<IRelativeSizeSpan> rev;
    CRelativeSizeSpan::New(0.7f, (IRelativeSizeSpan**)&rev);
    return rev;
}

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos
