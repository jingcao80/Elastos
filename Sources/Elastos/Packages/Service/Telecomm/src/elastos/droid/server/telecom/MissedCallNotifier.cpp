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

#include "elastos/droid/server/telecom/MissedCallNotifier.h"
#include "elastos/droid/server/telecom/Constants.h"
#include "elastos/droid/server/telecom/Log.h"
#include "elastos/droid/server/telecom/CTelecomBroadcastReceiver.h"
#include "R.h"
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Database.h>
#include <Elastos.Droid.Provider.h>
#include <Elastos.Droid.Telephony.h>
#include <Elastos.Droid.Graphics.h>
#include <elastos/droid/R.h>
#include <elastos/droid/net/Uri.h>
#include <elastos/droid/os/Build.h>
#include <elastos/droid/provider/Settings.h>
#include <elastos/droid/text/TextUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>

using Elastos::Droid::App::CNotificationBuilder;
using Elastos::Droid::App::CPendingIntentHelper;
using Elastos::Droid::App::CTaskStackBuilderHelper;
using Elastos::Droid::App::INotificationBuilder;
using Elastos::Droid::App::IPendingIntentHelper;
using Elastos::Droid::App::ITaskStackBuilder;
using Elastos::Droid::App::ITaskStackBuilderHelper;
using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IAsyncQueryHandler;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::Drawable::IBitmapDrawable;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Net::Uri;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Provider::CCalls;
using Elastos::Droid::Provider::IBaseColumns;
using Elastos::Droid::Provider::ICalls;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::Provider::Settings;
using Elastos::Droid::Telecom::CDisconnectCause;
using Elastos::Droid::Telecom::ICallState;
using Elastos::Droid::Telecom::IDisconnectCause;
using Elastos::Droid::Telecom::IPhoneAccount;
using Elastos::Droid::Telephony::CPhoneNumberUtils;
using Elastos::Droid::Telephony::IPhoneNumberUtils;
using Elastos::Droid::Text::CBidiFormatterHelper;
using Elastos::Droid::Text::CTextDirectionHeuristics;
using Elastos::Droid::Text::IBidiFormatter;
using Elastos::Droid::Text::IBidiFormatterHelper;
using Elastos::Droid::Text::ITextDirectionHeuristic;
using Elastos::Droid::Text::ITextDirectionHeuristics;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::CInteger32;
using Elastos::Core::ICharSequence;
using Elastos::Core::IInteger32;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::IO::ICloseable;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

//=============================================================================
// MissedCallNotifier::SubAsyncQueryHandler
//=============================================================================
MissedCallNotifier::SubAsyncQueryHandler::SubAsyncQueryHandler(
    /* [in] */ MissedCallNotifier* host)
    : mHost(host)
{}

ECode MissedCallNotifier::SubAsyncQueryHandler::constructor(
    /* [in] */ IContentResolver* contentResolver)
{
    return AsyncQueryHandler::constructor(contentResolver);
}

ECode MissedCallNotifier::SubAsyncQueryHandler::OnQueryComplete(
    /* [in] */ Int32 token,
    /* [in] */ IInterface* cookie,
    /* [in] */ ICursor* cursor)
{
    Log::D("MissedCallNotifier", "onQueryComplete()...");
    if (cursor != NULL) {
        // try {
        ECode ec;
        do {
            Boolean isMoveToNextOk;
            while (cursor->MoveToNext(&isMoveToNextOk), isMoveToNextOk) {
                // Get data about the missed call from the cursor
                String handleString;
                Int32 index;
                ec = cursor->GetColumnIndexOrThrow(ICalls::NUMBER, &index);
                if (FAILED(ec)) break;
                ec = cursor->GetString(
                        index, &handleString);
                if (FAILED(ec)) break;
                String resStr;
                ec = cursor->GetString(CALL_LOG_COLUMN_NUMBER, &resStr);
                if (FAILED(ec)) break;
                AutoPtr<IUri> handle;
                ec = Uri::Parse(resStr, (IUri**)&handle);
                if (FAILED(ec)) break;
                Int64 date;
                ec = cursor->GetInt64(CALL_LOG_COLUMN_DATE, &date);
                if (FAILED(ec)) break;
                ec = cursor->GetColumnIndexOrThrow(
                        ICalls::NUMBER_PRESENTATION, &index);
                if (FAILED(ec)) break;
                Int32 presentation;
                ec = cursor->GetInt32(index, &presentation);
                if (FAILED(ec)) break;
                if (presentation != ICalls::PRESENTATION_ALLOWED
                        || TextUtils::IsEmpty(handleString)) {
                    handle = NULL;
                } else {
                    AutoPtr<IPhoneNumberUtils> phoneNumberUtilsHelper;
                    CPhoneNumberUtils::AcquireSingleton((IPhoneNumberUtils**)&phoneNumberUtilsHelper);
                    Boolean isUriNumber;
                    phoneNumberUtilsHelper->IsUriNumber(handleString, &isUriNumber);
                    handle = NULL;
                    ec = Uri::FromParts(isUriNumber ?
                            IPhoneAccount::SCHEME_SIP : IPhoneAccount::SCHEME_TEL,
                                    handleString, String(NULL), (IUri**)&handle);
                    if (FAILED(ec)) break;
                }
                // Convert the data to a call object
                AutoPtr<ICall> call = new Call();
                ((Call*) call.Get())->constructor(mHost->mContext, NULL, NULL, NULL, NULL, NULL, TRUE,
                        FALSE);
                AutoPtr<IDisconnectCause> disconnectCause;
                CDisconnectCause::New(IDisconnectCause::MISSED, (IDisconnectCause**)&disconnectCause);
                ec = ((Call*) call.Get())->SetDisconnectCause(disconnectCause);
                if (FAILED(ec)) break;
                ec = ((Call*) call.Get())->SetState(ICallState::DISCONNECTED);
                if (FAILED(ec)) break;
                ec = ((Call*) call.Get())->SetCreationTimeMillis(date);
                if (FAILED(ec)) break;
                // Listen for the update to the caller information before posting the
                // notification so that we have the contact info and photo.
                ec = ((Call*) call.Get())->AddListener(new SubCallListenerBase(mHost));
                if (FAILED(ec)) break;
                // Set the handle here because that is what triggers the contact info
                // query.
                ec = ((Call*) call.Get())->SetHandle(handle, presentation);
                if (FAILED(ec)) break;
            }
        } while(FALSE);
        // } finally {
        ICloseable::Probe(cursor)->Close();
        // }
    }
    return NOERROR;
}

//=============================================================================
// MissedCallNotifier::SubCallListenerBase
//=============================================================================
MissedCallNotifier::SubCallListenerBase::SubCallListenerBase(
    /* [in] */ MissedCallNotifier* host)
    : mHost(host)
{}

ECode MissedCallNotifier::SubCallListenerBase::OnCallerInfoChanged(
    /* [in] */ ICall* call)
{
    ((Call*) call)->RemoveListener(this);  // No longer need to listen to call
                                // changes after the contact info
                                // is retrieved.
    mHost->ShowMissedCallNotification(call);
    return NOERROR;
}

//=============================================================================
// MissedCallNotifier
//=============================================================================
AutoPtr<ArrayOf<String> > MissedCallNotifier::CALL_LOG_PROJECTION = InitCALL_LOG_PROJECTION();
const Int32 MissedCallNotifier::DEFAULT_COLOR = 0xFFFFFF; //White
const Int32 MissedCallNotifier::DEFAULT_TIME = 1000; // 1 second
const Int32 MissedCallNotifier::CALL_LOG_COLUMN_ID = 0;
const Int32 MissedCallNotifier::CALL_LOG_COLUMN_NUMBER = 1;
const Int32 MissedCallNotifier::CALL_LOG_COLUMN_NUMBER_PRESENTATION = 2;
const Int32 MissedCallNotifier::CALL_LOG_COLUMN_DATE = 3;
const Int32 MissedCallNotifier::CALL_LOG_COLUMN_DURATION = 4;
const Int32 MissedCallNotifier::CALL_LOG_COLUMN_TYPE = 5;
const Int32 MissedCallNotifier::MISSED_CALL_NOTIFICATION_ID = 1;

MissedCallNotifier::MissedCallNotifier()
    : mMissedCallCount(0)
{}

ECode MissedCallNotifier::constructor(
    /* [in] */ IContext* context)
{
    mContext = context;
    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::NOTIFICATION_SERVICE, (IInterface**)&obj);
    mNotificationManager = INotificationManager::Probe(obj);
    UpdateOnStartup();
    return NOERROR;
}

ECode MissedCallNotifier::OnCallStateChanged(
    /* [in] */ ICall* call,
    /* [in] */ Int32 oldState,
    /* [in] */ Int32 newState)
{
    AutoPtr<IDisconnectCause> disconnectCause;
    ((Call*) call)->GetDisconnectCause((IDisconnectCause**)&disconnectCause);
    Int32 code;
    disconnectCause->GetCode(&code);
    if (oldState == ICallState::RINGING && newState == ICallState::DISCONNECTED &&
            code == IDisconnectCause::MISSED) {
        ShowMissedCallNotification(call);
    }
    return NOERROR;
}

ECode MissedCallNotifier::ClearMissedCalls()
{
    // Clear the list of new missed calls from the call log.
    AutoPtr<IContentValues> values;
    CContentValues::New((IContentValues**)&values);
    values->Put(ICalls::NEW, 0);
    values->Put(ICalls::IS_READ, 1);
    StringBuilder where;
    where.Append(ICalls::NEW);
    where.Append(" = 1 AND ");
    where.Append(ICalls::TYPE);
    where.Append(" = ?");
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(1);
    (*array)[0] = StringUtils::ToString(ICalls::MISSED_TYPE);
    AutoPtr<IContentResolver> contentResolver;
    mContext->GetContentResolver((IContentResolver**)&contentResolver);
    AutoPtr<ICalls> helper;
    CCalls::AcquireSingleton((ICalls**)&helper);
    AutoPtr<IUri> contentUri;
    helper->GetCONTENT_URI((IUri**)&contentUri);
    Int32 updateCount;
    contentResolver->Update(contentUri, values, where.ToString(),
            array, &updateCount);
    CancelMissedCallNotification();
    return NOERROR;
}

ECode MissedCallNotifier::ShowMissedCallNotification(
    /* [in] */ ICall* call)
{
    mMissedCallCount++;
    Int32 titleResId;
    String expandedText;  // The text in the notification's line 1 and 2.
    // Display the first line of the notification:
    // 1 missed call: <caller name || handle>
    // More than 1 missed call: <number of calls> + "missed calls"
    if (mMissedCallCount == 1) {
        titleResId = R::string::notification_missedCallTitle;
        GetNameForCall(call, &expandedText);
    } else {
        titleResId = R::string::notification_missedCallsTitle;
        AutoPtr<ArrayOf<IInterface*> > array = ArrayOf<IInterface*>::Alloc(1);
        AutoPtr<IInteger32> i32;
        CInteger32::New(mMissedCallCount, (IInteger32**)&i32);
        array->Set(0, i32);
        mContext->GetString(R::string::notification_missedCallsMsg, array, &expandedText);
    }
    // Create the notification.
    AutoPtr<INotificationBuilder> builder;
    CNotificationBuilder::New(mContext, (INotificationBuilder**)&builder);
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    builder->SetSmallIcon(Elastos::Droid::R::drawable::stat_notify_missed_call);
    Int32 color;
    res->GetColor(R::color::theme_color, &color);
    builder->SetColor(color);
    Int64 creationTimeMillis;
    ((Call*) call)->GetCreationTimeMillis(&creationTimeMillis);
    builder->SetWhen(creationTimeMillis);
    AutoPtr<ICharSequence> text;
    mContext->GetText(titleResId, (ICharSequence**)&text);
    builder->SetContentTitle(text);
    builder->SetContentText(StringUtils::ParseCharSequence(expandedText));
    AutoPtr<IPendingIntent> callLogPendingIntent;
    CreateCallLogPendingIntent((IPendingIntent**)&callLogPendingIntent);
    builder->SetContentIntent(callLogPendingIntent);
    builder->SetAutoCancel(TRUE);
    AutoPtr<IPendingIntent> clearMissedCallsPendingIntent;
    CreateClearMissedCallsPendingIntent((IPendingIntent**)&clearMissedCallsPendingIntent);
    builder->SetDeleteIntent(clearMissedCallsPendingIntent);;
    AutoPtr<IUri> handleUri;
    ((Call*) call)->GetHandle((IUri**)&handleUri);
    String schemeSpecificPart;
    handleUri->GetSchemeSpecificPart(&schemeSpecificPart);
    String handle = handleUri == NULL ? String(NULL) : schemeSpecificPart;
    // Add additional actions when there is only 1 missed call, like call-back and SMS.
    if (mMissedCallCount == 1) {
        Log::D("MissedCallNotifier", "Add actions with number %s.", Log::PiiHandle(StringUtils::ParseCharSequence(handle)).string());
        String resStr;
        mContext->GetString(R::string::handle_restricted, &resStr);
        if (!TextUtils::IsEmpty(handle) && !TextUtils::Equals(handle, resStr)) {
            String resStr;
            mContext->GetString(R::string::notification_missedCall_call_back, &resStr);
            AutoPtr<IPendingIntent> callBackPendingIntent;
            CreateCallBackPendingIntent(handleUri, (IPendingIntent**)&callBackPendingIntent);
            builder->AddAction(R::drawable::stat_sys_phone_call, StringUtils::ParseCharSequence(resStr),
                    callBackPendingIntent);
            String resStrMsg;
            mContext->GetString(R::string::notification_missedCall_message, &resStrMsg);
            AutoPtr<IPendingIntent> sendSmsFromNotificationPendingIntent;
            CreateSendSmsFromNotificationPendingIntent(handleUri, (IPendingIntent**)&sendSmsFromNotificationPendingIntent);
            builder->AddAction(R::drawable::ic_text_holo_dark, StringUtils::ParseCharSequence(resStrMsg),
                    sendSmsFromNotificationPendingIntent);
        }
        AutoPtr<IBitmap> photoIcon;
        ((Call*) call)->GetPhotoIcon((IBitmap**)&photoIcon);
        if (photoIcon != NULL) {
            builder->SetLargeIcon(photoIcon);
        } else {
            AutoPtr<IDrawable> photo;
            ((Call*) call)->GetPhoto((IDrawable**)&photo);
            if (photo != NULL && IBitmapDrawable::Probe(photo) != NULL) {
                AutoPtr<IBitmap> bitmap;
                IBitmapDrawable::Probe(photo)->GetBitmap((IBitmap**)&bitmap);
                builder->SetLargeIcon(bitmap);
            }
        }
    } else {
        Log::D("MissedCallNotifier", "Suppress actions. handle: %s, missedCalls: %d.",
                Log::PiiHandle(StringUtils::ParseCharSequence(handle)).string(), mMissedCallCount);
    }
    AutoPtr<INotification> notification;
    builder->Build((INotification**)&notification);
    ConfigureLedOnNotification(mContext, notification);
    Log::I("MissedCallNotifier", "Adding missed call notification for %s.", TO_CSTR(call));
    AutoPtr<IUserHandleHelper> userHandleHelper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&userHandleHelper);
    AutoPtr<IUserHandle> current;
    userHandleHelper->GetCURRENT((IUserHandle**)&current);
    mNotificationManager->NotifyAsUser(
            String(NULL) /* tag */ , MISSED_CALL_NOTIFICATION_ID, notification, current);
    return NOERROR;
}

ECode MissedCallNotifier::CancelMissedCallNotification()
{
    // Reset the number of missed calls to 0.
    mMissedCallCount = 0;
    mNotificationManager->Cancel(MISSED_CALL_NOTIFICATION_ID);
    return NOERROR;
}

ECode MissedCallNotifier::GetNameForCall(
    /* [in] */ ICall* call,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IUri> callHandle;
    ((Call*) call)->GetHandle((IUri**)&callHandle);
    String schemeSpecificPart;
    callHandle->GetSchemeSpecificPart(&schemeSpecificPart);
    String handle = callHandle == NULL ? String(NULL) : schemeSpecificPart;
    String name;
    ((Call*) call)->GetName(&name);
    if (!TextUtils::IsEmpty(name) && TextUtils::IsGraphic(StringUtils::ParseCharSequence(name))) {
        *result = name;
        return NOERROR;
    } else if (!TextUtils::IsEmpty(handle)) {
        // A handle should always be displayed LTR using {@link BidiFormatter} regardless of the
        // content of the rest of the notification.
        // TODO: Does this apply to SIP addresses?
        AutoPtr<IBidiFormatterHelper> helper;
        CBidiFormatterHelper::AcquireSingleton((IBidiFormatterHelper**)&helper);
        AutoPtr<IBidiFormatter> bidiFormatter;
        helper->GetInstance((IBidiFormatter**)&bidiFormatter);
        AutoPtr<ITextDirectionHeuristics> textDirectionHeuristicsHelper;
        CTextDirectionHeuristics::AcquireSingleton((ITextDirectionHeuristics**)&textDirectionHeuristicsHelper);
        AutoPtr<ITextDirectionHeuristic> ltr;
        textDirectionHeuristicsHelper->GetLTR((ITextDirectionHeuristic**)&ltr);
        return bidiFormatter->UnicodeWrap(handle, ltr, result);
    } else {
        // Use "unknown" if the call is unidentifiable.
        String resStr;
        mContext->GetString(R::string::unknown, &resStr);
        *result = resStr;
        return NOERROR;
    }
    return NOERROR;
}

ECode MissedCallNotifier::CreateCallLogPendingIntent(
    /* [out] */ IPendingIntent** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_VIEW, NULL, (IIntent**)&intent);
    intent->SetType(ICalls::CONTENT_TYPE);
    AutoPtr<ITaskStackBuilderHelper> helper;
    CTaskStackBuilderHelper::AcquireSingleton((ITaskStackBuilderHelper**)&helper);
    AutoPtr<ITaskStackBuilder> taskStackBuilder;
    helper->Create(mContext, (ITaskStackBuilder**)&taskStackBuilder);
    taskStackBuilder->AddNextIntent(intent);
    return taskStackBuilder->GetPendingIntent(0, 0, result);
}

ECode MissedCallNotifier::CreateClearMissedCallsPendingIntent(
    /* [out] */ IPendingIntent** result)
{
    return CreateTelecomPendingIntent(
            TelecomBroadcastReceiver::ACTION_CLEAR_MISSED_CALLS, NULL, result);
}

ECode MissedCallNotifier::CreateCallBackPendingIntent(
    /* [in] */ IUri* handle,
    /* [out] */ IPendingIntent** result)
{
    return CreateTelecomPendingIntent(
            TelecomBroadcastReceiver::ACTION_CALL_BACK_FROM_NOTIFICATION, handle, result);
}

ECode MissedCallNotifier::CreateSendSmsFromNotificationPendingIntent(
    /* [in] */ IUri* handle,
    /* [out] */ IPendingIntent** result)
{
    VALIDATE_NOT_NULL(result)

    String schemeSpecificPart;
    handle->GetSchemeSpecificPart(&schemeSpecificPart);
    AutoPtr<IUri> uri;
    Uri::FromParts(Constants::SCHEME_SMSTO, schemeSpecificPart, String(NULL), (IUri**)&uri);
    return CreateTelecomPendingIntent(
            TelecomBroadcastReceiver::ACTION_SEND_SMS_FROM_NOTIFICATION,
            uri, result);
}

ECode MissedCallNotifier::CreateTelecomPendingIntent(
    /* [in] */ const String& action,
    /* [in] */ IUri* data,
    /* [out] */ IPendingIntent** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ITelecomBroadcastReceiver> obj;
    CTelecomBroadcastReceiver::New((ITelecomBroadcastReceiver**)&obj);
    AutoPtr<IClassInfo> classInfo;
    CObject::ReflectClassInfo(obj, (IClassInfo**)&classInfo);
    AutoPtr<IIntent> intent;
    CIntent::New(action, data, mContext, classInfo, (IIntent**)&intent);
    AutoPtr<IPendingIntentHelper> pendingIntentHelper;
    CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&pendingIntentHelper);
    return pendingIntentHelper->GetBroadcast(mContext, 0, intent, 0, result);
}

ECode MissedCallNotifier::ConfigureLedOnNotification(
    /* [in] */ IContext* context,
    /* [in] */ INotification* notification)
{
    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    Int32 i32;
    Settings::System::GetInt32(resolver,
            ISettingsSystem::NOTIFICATION_LIGHT_PULSE, 0, &i32);
    Boolean lightEnabled = i32 == 1;
    if (!lightEnabled) {
        return NOERROR;
    }
    Int32 flags;
    notification->GetFlags(&flags);
    flags |= INotification::FLAG_SHOW_LIGHTS;
    notification->SetFlags(flags);
    // Get Missed call values if they are to be used
    Settings::System::GetInt32(resolver,
            ISettingsSystem::NOTIFICATION_LIGHT_PULSE_CUSTOM_ENABLE, 0, &i32);
    Boolean customEnabled = i32 == 1;
    if (!customEnabled) {
        Int32 defaults;
        notification->GetDefaults(&defaults);
        defaults |= INotification::DEFAULT_LIGHTS;
        notification->SetDefaults(defaults);
        return NOERROR;
    }
    Int32 ledARGB;
    Settings::System::GetInt32(resolver,
        ISettingsSystem::NOTIFICATION_LIGHT_PULSE_CALL_COLOR, DEFAULT_COLOR, &ledARGB);
    notification->SetLedARGB(ledARGB);
    Int32 ledOnMS;
    Settings::System::GetInt32(resolver,
        ISettingsSystem::NOTIFICATION_LIGHT_PULSE_CALL_LED_ON, DEFAULT_TIME, &ledOnMS);
    notification->SetLedOnMS(ledOnMS);
    Int32 ledOffMS;
    Settings::System::GetInt32(resolver,
        ISettingsSystem::NOTIFICATION_LIGHT_PULSE_CALL_LED_OFF, DEFAULT_TIME, &ledOffMS);
    notification->SetLedOffMS(ledOffMS);
    return NOERROR;
}

ECode MissedCallNotifier::UpdateOnStartup()
{
    Log::D("MissedCallNotifier", "updateOnStartup()...");
    // instantiate query handler
    AutoPtr<IContentResolver> contentResolver;
    mContext->GetContentResolver((IContentResolver**)&contentResolver);
    AutoPtr<IAsyncQueryHandler> queryHandler = new SubAsyncQueryHandler(this);
    ((SubAsyncQueryHandler*) queryHandler.Get())->constructor(contentResolver);
    // setup query spec, look for all Missed calls that are new.
    StringBuilder where("type=");
    where.Append(ICalls::MISSED_TYPE);
    where.Append(" AND new=1");
    // start the query
    AutoPtr<ICalls> helper;
    CCalls::AcquireSingleton((ICalls**)&helper);
    AutoPtr<IUri> contentUri;
    helper->GetCONTENT_URI((IUri**)&contentUri);
    queryHandler->StartQuery(0, NULL, contentUri, CALL_LOG_PROJECTION,
            where.ToString(), NULL, ICalls::DEFAULT_SORT_ORDER);
    return NOERROR;
}

AutoPtr<ArrayOf<String> > MissedCallNotifier::InitCALL_LOG_PROJECTION()
{
    AutoPtr<ArrayOf<String> > rev = ArrayOf<String>::Alloc(6);
    (*rev)[0] = IBaseColumns::ID;
    (*rev)[1] = ICalls::NUMBER;
    (*rev)[2] = ICalls::NUMBER_PRESENTATION;
    (*rev)[3] = ICalls::DATE;
    (*rev)[4] = ICalls::DURATION;
    (*rev)[5] = ICalls::TYPE;
    return rev;
}

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos
