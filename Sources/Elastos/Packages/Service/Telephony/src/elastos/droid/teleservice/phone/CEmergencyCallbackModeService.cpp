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

#include "elastos/droid/teleservice/phone/CPhoneApp.h"
#include "elastos/droid/teleservice/phone/CEmergencyCallbackModeServiceBroadcastReceiver.h"
#include "elastos/droid/teleservice/phone/CEmergencyCallbackModeServiceLocalBinder.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/CoreUtils.h>
#include <Elastos.CoreLibrary.Core.h>
#include <elastos/utility/logging/Logger.h>
#include "R.h"

using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::App::INotification;
using Elastos::Droid::App::INotificationBuilder;
using Elastos::Droid::App::CNotificationBuilder;
using Elastos::Droid::App::IPendingIntentHelper;
using Elastos::Droid::App::CPendingIntentHelper;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Internal::Telephony::ITelephonyProperties;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Internal::Telephony::ITelephonyIntents;
using Elastos::Droid::Internal::Telephony::IPhoneFactory;
using Elastos::Droid::Internal::Telephony::CPhoneFactory;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Core::CoreUtils;
using Elastos::Core::IBoolean;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

ECode CEmergencyCallbackModeService::MyBroadcastReceiver::constructor(
    /* [in] */ IEmergencyCallbackModeService* host)
{
    mHost = (CEmergencyCallbackModeService*)host;
    return BroadcastReceiver::constructor();
}

ECode CEmergencyCallbackModeService::MyBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    // Stop the service when phone exits Emergency Callback Mode
    String action;
    intent->GetAction(&action);
    if (action.Equals(ITelephonyIntents::ACTION_EMERGENCY_CALLBACK_MODE_CHANGED)) {
        Boolean res;
        if ((intent->GetBooleanExtra(String("phoneinECMState"), FALSE, &res), res) == FALSE) {
            mHost->StopSelf();
        }
    }
    // Show dialog box
    else if (action.Equals(ITelephonyIntents::ACTION_SHOW_NOTICE_ECM_BLOCK_OTHERS)) {
        AutoPtr<IIntent> intent;
        CIntent::New(ITelephonyIntents::ACTION_SHOW_NOTICE_ECM_BLOCK_OTHERS, (IIntent**)&intent);
        intent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
        context->StartActivity(intent);
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(CEmergencyCallbackModeService::LocalBinder, Binder,
        IEmergencyCallbackModeServiceLocalBinder)

ECode CEmergencyCallbackModeService::LocalBinder::GetService(
    /* [out] */ IEmergencyCallbackModeService** service)
{
    VALIDATE_NOT_NULL(service)

    *service = (IEmergencyCallbackModeService*)this;
    REFCOUNT_ADD(*service);
    return NOERROR;
}


CEmergencyCallbackModeService::MyHandler::MyHandler(
    /* [in] */ CEmergencyCallbackModeService* host)
    : mHost(host)
{
    Handler::constructor();
}

ECode CEmergencyCallbackModeService::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case ECM_TIMER_RESET:
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            AutoPtr<AsyncResult> result = (AsyncResult*)IAsyncResult::Probe(obj);
            mHost->ResetEcmTimer(result);
            break;
    }
    return NOERROR;
}

CEmergencyCallbackModeService::MyCountDownTimer::MyCountDownTimer(
    /* [in] */ Int64 millisInFuture,
    /* [in] */ Int64 countDownInterval,
    /* [in] */ CEmergencyCallbackModeService* host)
    : mHost(host)
{
    CountDownTimer::constructor(millisInFuture, countDownInterval);
}

ECode CEmergencyCallbackModeService::MyCountDownTimer::OnTick(
    /* [in] */ Int64 millisUntilFinished)
{
    mHost->mTimeLeft = millisUntilFinished;
    mHost->ShowNotification(millisUntilFinished);
    return NOERROR;
}

ECode CEmergencyCallbackModeService::MyCountDownTimer::OnFinish()
{
    //Do nothing
    return NOERROR;
}

const Int32 CEmergencyCallbackModeService::DEFAULT_ECM_EXIT_TIMER_VALUE = 300000;
const String CEmergencyCallbackModeService::TAG("EmergencyCallbackModeService");

CAR_INTERFACE_IMPL(CEmergencyCallbackModeService, Service, IEmergencyCallbackModeService)

CAR_OBJECT_IMPL(CEmergencyCallbackModeService)

CEmergencyCallbackModeService::CEmergencyCallbackModeService()
    : mTimeLeft(0)
    , mInEmergencyCall(FALSE)
{
    mHandler = new MyHandler(this);

    CEmergencyCallbackModeServiceBroadcastReceiver::New(this, (IBroadcastReceiver**)&mEcmReceiver);

    CEmergencyCallbackModeServiceLocalBinder::New((IBinder**)&mBinder);
}

ECode CEmergencyCallbackModeService::constructor()
{
    return Service::constructor();
}

ECode CEmergencyCallbackModeService::OnCreate()
{
    // Check if it is CDMA phone
    AutoPtr<IPhone> phone;
    AutoPtr<IPhoneFactory> helper;
    CPhoneFactory::AcquireSingleton((IPhoneFactory**)&helper);
    helper->GetDefaultPhone((IPhone**)&phone);
    Int32 type;
    AutoPtr<IPhone> phone2;
    if (((phone->GetPhoneType(&type), type)  != IPhoneConstants::PHONE_TYPE_CDMA)
            && ((phone->GetImsPhone((IPhone**)&phone2), phone2) == NULL)) {
        StringBuilder sb;
        sb += "Error! Emergency Callback Mode not supported for ";
        String name;
        phone->GetPhoneName(&name);
        sb += name;
        sb += " phones";
        Logger::E(TAG, sb.ToString());
        StopSelf();
    }

    // Register receiver for intents
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(ITelephonyIntents::ACTION_EMERGENCY_CALLBACK_MODE_CHANGED);
    filter->AddAction(ITelephonyIntents::ACTION_SHOW_NOTICE_ECM_BLOCK_OTHERS);
    AutoPtr<IIntent> intent;
    RegisterReceiver(mEcmReceiver, filter, (IIntent**)&intent);

    AutoPtr<IInterface> obj;
    GetSystemService(NOTIFICATION_SERVICE, (IInterface**)&obj);
    mNotificationManager = INotificationManager::Probe(obj);

    // Register ECM timer reset notfication
    helper->GetDefaultPhone((IPhone**)&mPhone);
    mPhone->RegisterForEcmTimerReset(mHandler, ECM_TIMER_RESET, NULL);

    StartTimerNotification();
    return NOERROR;
}

ECode CEmergencyCallbackModeService::OnDestroy()
{
    // Unregister receiver
    UnregisterReceiver(mEcmReceiver);
    // Unregister ECM timer reset notification
    mPhone->UnregisterForEcmTimerReset(mHandler);

    // Cancel the notification and timer
    mNotificationManager->Cancel(Elastos::Droid::TeleService::R::string::phone_in_ecm_notification_title);
    return mTimer->Cancel();
}

void CEmergencyCallbackModeService::StartTimerNotification()
{
    // Get Emergency Callback Mode timeout value
    AutoPtr<ISystemProperties> helper;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&helper);
    Int64 ecmTimeout;
    helper->GetInt64(ITelephonyProperties::PROPERTY_ECM_EXIT_TIMER, DEFAULT_ECM_EXIT_TIMER_VALUE, &ecmTimeout);

    // Show the notification
    ShowNotification(ecmTimeout);

    // Start countdown timer for the notification updates
    if (mTimer != NULL) {
        mTimer->Cancel();
    }
    else {
        mTimer = new MyCountDownTimer(ecmTimeout, 1000, this);
    }
    mTimer->Start();
}

void CEmergencyCallbackModeService::ShowNotification(
    /* [in] */ Int64 millisUntilFinished)
{
    AutoPtr<ISystemProperties> helper;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&helper);
    String mode;
    helper->Get(ITelephonyProperties::PROPERTY_INECM_MODE, &mode);
    Boolean isInEcm = StringUtils::ParseBoolean(mode);

    if (!isInEcm) {
        Logger::I(TAG, "Asked to show notification but not in ECM mode");
        if (mTimer != NULL) {
            mTimer->Cancel();
        }
        return;
    }


    AutoPtr<IContext> context;
    GetApplicationContext((IContext**)&context);
    AutoPtr<INotificationBuilder> builder;
    CNotificationBuilder::New(context, (INotificationBuilder**)&builder);
    builder->SetOngoing(TRUE);
    builder->SetPriority(INotification::PRIORITY_HIGH);
    builder->SetSmallIcon(Elastos::Droid::TeleService::R::drawable::ic_emergency_callback_mode);

    AutoPtr<ICharSequence> text1;
    GetText(Elastos::Droid::TeleService::R::string::phone_entered_ecm_text, (ICharSequence**)&text1);
    builder->SetTicker(text1);

    AutoPtr<ICharSequence> text2;
    GetText(Elastos::Droid::TeleService::R::string::phone_in_ecm_notification_title,
            (ICharSequence**)&text2);
    builder->SetContentTitle(text2);

    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    Int32 color;
    resources->GetColor(Elastos::Droid::TeleService::R::color::dialer_theme_color,
            &color);
    builder->SetColor(color);

    // PendingIntent to launch Emergency Callback Mode Exit activity if the user selects
    // this notification
    AutoPtr<IPendingIntentHelper> helper2;
    CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&helper2);
    AutoPtr<IIntent> intent;
    CIntent::New(IEmergencyCallbackModeExitDialog::ACTION_SHOW_ECM_EXIT_DIALOG, (IIntent**)&intent);
    AutoPtr<IPendingIntent> contentIntent;
    helper2->GetActivity(this, 0, intent, 0, (IPendingIntent**)&contentIntent);
    builder->SetContentIntent(contentIntent);

    // Format notification string
    String text;
    if(mInEmergencyCall) {
        AutoPtr<ICharSequence> text3;
        GetText(Elastos::Droid::TeleService::R::string::phone_in_ecm_call_notification_text,
                (ICharSequence**)&text3);
        text3->ToString(&text);
    }
    else {
        Int32 minutes = (Int32)(millisUntilFinished / 60000);
        String time;
        time.AppendFormat("%d:%02d", minutes, (millisUntilFinished % 60000) / 1000);

        AutoPtr<IResources> resources;
        GetResources((IResources**)&resources);
        AutoPtr<ICharSequence> text4;
        resources->GetQuantityText(Elastos::Droid::TeleService::R::plurals::phone_in_ecm_notification_time,
                minutes, (ICharSequence**)&text4);
        String str;
        text4->ToString(&str);
        text.AppendFormat(str.string(), time.string());
    }
    AutoPtr<ICharSequence> cchar = CoreUtils::Convert(text);
    builder->SetContentText(cchar);

    // Show notification
    AutoPtr<INotification> notification;
    builder->Build((INotification**)&notification);
    mNotificationManager->Notify(Elastos::Droid::TeleService::R::string::phone_in_ecm_notification_title,
            notification);
}

void CEmergencyCallbackModeService::ResetEcmTimer(
    /* [in] */ AsyncResult* r)
{
    AutoPtr<IBoolean> value = IBoolean::Probe(r->mResult);
    Boolean isTimerCanceled;
    value->GetValue(&isTimerCanceled);

    if (isTimerCanceled) {
        mInEmergencyCall = TRUE;
        mTimer->Cancel();
        ShowNotification(0);
    }
    else {
        mInEmergencyCall = FALSE;
        StartTimerNotification();
    }
}

ECode CEmergencyCallbackModeService::OnBind(
    /* [in] */ IIntent* intent,
    /* [out] */ IBinder** binder)
{
    VALIDATE_NOT_NULL(binder)

    *binder = mBinder;
    REFCOUNT_ADD(*binder);
    return NOERROR;
}

ECode CEmergencyCallbackModeService::GetEmergencyCallbackModeTimeout(
    /* [out] */ Int64* timeout)
{
    VALIDATE_NOT_NULL(timeout)

    *timeout = mTimeLeft;
    return NOERROR;
}

ECode CEmergencyCallbackModeService::GetEmergencyCallbackModeCallState(
    /* [out] */ Boolean* state)
{
    VALIDATE_NOT_NULL(state)

    *state = mInEmergencyCall;
    return NOERROR;
}

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos