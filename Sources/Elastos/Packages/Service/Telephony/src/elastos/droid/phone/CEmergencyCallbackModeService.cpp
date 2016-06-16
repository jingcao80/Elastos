
#include "elastos/droid/phone/CPhoneApp.h"

namespace Elastos {
namespace Droid {
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
            StopSelf();
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

ECode CEmergencyCallbackModeService::LocalBinder::GetService(
    /* [out] */ IEmergencyCallbackModeService** service)
{
    VALIDATE_NOT_NULL(service)

    *service = EmergencyCallbackModeService.this;
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
            mHost->ResetEcmTimer(IAsyncResult::Probe(obj));
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
    EmergencyCallbackModeService.this::ShowNotification(millisUntilFinished);
    return NOERROR;
}

ECode CEmergencyCallbackModeService::MyCountDownTimer::OnFinish()
{
    //Do nothing
    return NOERROR;
}

const Int32 CEmergencyCallbackModeService::DEFAULT_ECM_EXIT_TIMER_VALUE = 300000;
const String CEmergencyCallbackModeService::LOG_TAG("EmergencyCallbackModeService");

const Int32 CEmergencyCallbackModeService::ECM_TIMER_RESET = 1;

CAR_INTERFACE_IMPL(CEmergencyCallbackModeService, Service, IEmergencyCallbackModeService)

CAR_OBJECT_IMPL(CEmergencyCallbackModeService)

CEmergencyCallbackModeService::CEmergencyCallbackModeService()
    : mTimeLeft(0)
    , mInEmergencyCall(FALSE)
{
    mHandler = new MyHandler(this);

    CEmergencyCallbackModeServiceBroadcastReceiver::New(this, (BroadcastReceiver**)&mEcmReceiver);

    mBinder = new LocalBinder();
}

ECode CEmergencyCallbackModeService::constructor()
{
    return Service::constructor();
}

ECode CEmergencyCallbackModeService::OnCreate()
{
    // Check if it is CDMA phone
    AutoPtr<IPhone> phone;
    PhoneFactory::GetDefaultPhone((IPhone**)&phone);
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
        Logger::E(LOG_TAG, sb.ToString());
        StopSelf();
    }

    // Register receiver for intents
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(ITelephonyIntents::ACTION_EMERGENCY_CALLBACK_MODE_CHANGED);
    filter->AddAction(ITelephonyIntents::ACTION_SHOW_NOTICE_ECM_BLOCK_OTHERS);
    RegisterReceiver(mEcmReceiver, filter);

    AutoPtr<IInterface> obj;
    GetSystemService(NOTIFICATION_SERVICE, (IInterface**)&obj);
    mNotificationManager = INotificationManager::Probe(obj);

    // Register ECM timer reset notfication
    PhoneFactory::GetDefaultPhone((IPhone**)&mPhone);
    mPhone->RegisterForEcmTimerReset(mHandler, ECM_TIMER_RESET, NULL);

    StartTimerNotification();
}

ECode CEmergencyCallbackModeService::OnDestroy()
{
    // Unregister receiver
    UnregisterReceiver(mEcmReceiver);
    // Unregister ECM timer reset notification
    mPhone->UnregisterForEcmTimerReset(mHandler);

    // Cancel the notification and timer
    mNotificationManager->Cancel(R.string.phone_in_ecm_notification_title);
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
        mTimer = new MyCountDownTimer(ecmTimeout, 1000);
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
        Logger::I(LOG_TAG, "Asked to show notification but not in ECM mode");
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
    builder->SetSmallIcon(R.drawable.ic_emergency_callback_mode);

    AutoPtr<ICharSequence> text1;
    GetText(R.string.phone_entered_ecm_text, (ICharSequence**)&text1);
    builder->SetTicker(text1);

    AutoPtr<ICharSequence> text2;
    GetText(R.string.phone_in_ecm_notification_title, (ICharSequence**)&text2);
    builder->SetContentTitle(text2);

    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    AutoPtr<IColor> color;
    resources->GetColor(R.color.dialer_theme_color, (IColor**)&color);
    builder->SetColor(color);

    // PendingIntent to launch Emergency Callback Mode Exit activity if the user selects
    // this notification
    AutoPtr<IPendingIntentHelper> helper2;
    CPendingIntentHelper::New((IPendingIntentHelper**)&helper2);
    AutoPtr<IIntent> intent;
    CIntent::New(IEmergencyCallbackModeExitDialog::ACTION_SHOW_ECM_EXIT_DIALOG, (IIntent**)&intent);
    AutoPtr<IPendingIntent> contentIntent;
    helper2->GetActivity(this, 0, intent, 0, (IPendingIntent**)&contentIntent);
    builder->SetContentIntent(contentIntent);

    // Format notification string
    String text;
    if(mInEmergencyCall) {
        AutoPtr<ICharSequence> text3;
        GetText(R.string.phone_in_ecm_call_notification_text, (ICharSequence**)&text3);
        text3->ToString(&text);
    }
    else {
        Int32 minutes = (Int32)(millisUntilFinished / 60000);
        String time = String.format("%d:%02d", minutes, (millisUntilFinished % 60000) / 1000);

        AutoPtr<IResources> resources;
        GetResources((IResources**)&resources);
        AutoPtr<ICharSequence> text4;
        resources->getQuantityText(R.plurals.phone_in_ecm_notification_time, minutes, (ICharSequence**)&text4);
        String str;
        text4->ToString(&str);
        text = String.format(str, time);
    }
    builder->SetContentText(text);

    // Show notification
    AutoPtr<INotification> notification;
    builder->Build((INotification**)&notification);
    mNotificationManager->Notify(R.string.phone_in_ecm_notification_title, notification);
}

void CEmergencyCallbackModeService::ResetEcmTimer(
    /* [in] */ IAsyncResult* r)
{
    Boolean isTimerCanceled = ((Boolean)r.result).booleanValue();

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
} // namespace Droid
} // namespace Elastos