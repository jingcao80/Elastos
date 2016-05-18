
#include "elastos/droid/systemui/doze/CDozeService.h"
#include "elastos/droid/systemui/doze/DozeLog.h"
#include "elastos/droid/systemui/Utils.h"
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Os.h"
#include <elastos/droid/os/SystemClock.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::IApplication;
using Elastos::Droid::App::CPendingIntentHelper;
using Elastos::Droid::App::IPendingIntentHelper;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Hardware::EIID_ISensorEventListener;
using Elastos::Droid::Hardware::EIID_ITriggerEventListener;
using Elastos::Droid::Media::CAudioAttributesBuilder;
using Elastos::Droid::Media::IAudioAttributes;
using Elastos::Droid::Media::IAudioAttributesBuilder;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::IVibrator;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::View::IDisplay;
using Elastos::Core::CSystem;
using Elastos::Core::EIID_IRunnable;
using Elastos::Core::ISystem;
using Elastos::Core::StringBuilder;
using Elastos::Utility::CDate;
using Elastos::Utility::IDate;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Doze {

const String CDozeService::TAG("DozeService");
Boolean CDozeService::DEBUG = Logger::IsLoggable(TAG, Logger::___DEBUG);
const String CDozeService::ACTION_BASE("Elastos.Droid.SystemUI.Doze");
const String CDozeService::PULSE_ACTION = ACTION_BASE + ".pulse";
const String CDozeService::NOTIFICATION_PULSE_ACTION = ACTION_BASE + ".notification_pulse";
const String CDozeService::EXTRA_INSTANCE("instance");

CDozeService::DozeBroadcastReceiver::DozeBroadcastReceiver(
    /* [in] */ CDozeService* host)
    : mHost(host)
{}

ECode CDozeService::DozeBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (PULSE_ACTION.Equals(action)) {
        if (DEBUG) Logger::D(mHost->mTag, "Received pulse intent");
        mHost->RequestPulse();
    }
    if (NOTIFICATION_PULSE_ACTION.Equals(action)) {
        Int64 instance = 0;
        intent->GetInt64Extra(EXTRA_INSTANCE, -1, &instance);
        if (DEBUG) Logger::D(mHost->mTag, "Received notification pulse intent instance=" + instance);
        DozeLog::TraceNotificationPulse(instance);
        mHost->RequestPulse();
        mHost->RescheduleNotificationPulse(mHost->mNotificationLightOn);
    }
    if (IUiModeManager::ACTION_ENTER_CAR_MODE.Equals(action)) {
        mHost->mCarMode = TRUE;
        if (mHost->mCarMode && mHost->mDreaming) {
            mHost->FinishForCarMode();
        }
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(CDozeService::DozeHostCallback, Object, IDozeHostCallback)
CDozeService::DozeHostCallback::DozeHostCallback(
    /* [in] */ CDozeService* host)
    : mHost(host)
{}

ECode CDozeService::DozeHostCallback::OnNewNotifications()
{
    if (DEBUG) Logger::D(mHost->mTag, "onNewNotifications");
    // noop for now
    return NOERROR;
}

ECode CDozeService::DozeHostCallback::OnBuzzBeepBlinked()
{
    if (DEBUG) Logger::D(mHost->mTag, "onBuzzBeepBlinked");
    mHost->UpdateNotificationPulse();
    return NOERROR;
}

ECode CDozeService::DozeHostCallback::OnNotificationLight(
    /* [in] */ Boolean on)
{
    if (DEBUG) Logger::D(mHost->mTag, "onNotificationLight on=" + on);
    if (mHost->mNotificationLightOn == on) return NOERROR;
    mHost->mNotificationLightOn = on;
    if (mHost->mNotificationLightOn) {
        mHost->UpdateNotificationPulse();
    }
    return NOERROR;
}

ECode CDozeService::DozeHostCallback::OnPowerSaveChanged(
    /* [in] */ Boolean active)
{
    mHost->mPowerSaveActive = active;
    if (mHost->mPowerSaveActive && mHost->mDreaming) {
        mHost->FinishToSavePower();
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(CDozeService::TriggerSensor, Object, ITriggerEventListener)
CDozeService::TriggerSensor::TriggerSensor(
    /* [in] */ Int32 type,
    /* [in] */ Boolean configured,
    /* [in] */ Boolean debugVibrate,
    /* [in] */ CDozeService* host)
    : mHost(host)
    , mConfigured(FALSE)
    , mDebugVibrate(FALSE)
    , mRequested(FALSE)
    , mRegistered(FALSE)
    , mDisabled(FALSE)
{
    mHost->mSensors->GetDefaultSensor(type, (ISensor**)&mSensor);
    mConfigured = configured;
    mDebugVibrate = debugVibrate;
}

ECode CDozeService::TriggerSensor::SetListening(
    /* [in] */ Boolean listen)
{
    if (mRequested == listen) return NOERROR;
    mRequested = listen;
    UpdateListener();
    return NOERROR;
}

ECode CDozeService::TriggerSensor::SetDisabled(
    /* [in] */ Boolean disabled)
{
    if (mDisabled == disabled) return NOERROR;
    mDisabled = disabled;
    UpdateListener();
    return NOERROR;
}

void CDozeService::TriggerSensor::UpdateListener()
{
    if (!mConfigured || mSensor == NULL) return;
    if (mRequested && !mDisabled) {
        mHost->mSensors->RequestTriggerSensor(this, mSensor, &mRegistered);
    }
    else if (mRegistered) {
        Boolean result = FALSE;
        mHost->mSensors->CancelTriggerSensor(this, mSensor, &result);
        mRegistered = FALSE;
    }
}

ECode CDozeService::TriggerSensor::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    StringBuilder sb;
    sb.Append("{mRegistered=");
    sb.Append(mRegistered);
    sb.Append(", mRequested=");
    sb.Append(mRequested);
    sb.Append(", mDisabled=");
    sb.Append(mDisabled);
    sb.Append(", mConfigured=");
    sb.Append(mConfigured);
    sb.Append(", mDebugVibrate=");
    sb.Append(mDebugVibrate);
    sb.Append(", mSensor=");
    sb.Append(mSensor);
    sb.Append("}");
    return sb.ToString(str);
}

ECode CDozeService::TriggerSensor::OnTrigger(
    /* [in] */ ITriggerEvent* event)
{
    mHost->mWakeLock->AcquireLock();
    // try {
    if (DEBUG) Logger::D(mHost->mTag, "onTrigger: %s", TriggerEventToString(event).string());
    if (mDebugVibrate) {
        AutoPtr<IInterface> obj;
        mHost->mContext->GetSystemService(IContext::VIBRATOR_SERVICE, (IInterface**)&obj);
        AutoPtr<IVibrator> v = IVibrator::Probe(obj);
        if (v != NULL) {
            AutoPtr<IAudioAttributesBuilder> builder;
            CAudioAttributesBuilder::New((IAudioAttributesBuilder**)&builder);
            builder->SetContentType(IAudioAttributes::CONTENT_TYPE_SONIFICATION);
            builder->SetUsage(IAudioAttributes::USAGE_ASSISTANCE_SONIFICATION);
            AutoPtr<IAudioAttributes> aa;
            builder->Build((IAudioAttributes**)&aa);
            v->Vibrate(1000, aa);
        }
    }

    mHost->RequestPulse();
    SetListening(TRUE);  // reregister, this sensor only fires once

    // reset the notification pulse schedule, but only if we think we were not triggered
    // by a notification-related vibration
    Int64 timeSinceNotification = 0;
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    system->GetCurrentTimeMillis(&timeSinceNotification);
    timeSinceNotification -= mHost->mNotificationPulseTime;
    Int32 tmp = mHost->mDozeParameters->GetPickupVibrationThreshold();
    Boolean withinVibrationThreshold = timeSinceNotification < tmp;
    if (withinVibrationThreshold) {
       if (DEBUG) Logger::D(mHost->mTag, "Not resetting schedule, recent notification");
    }
    else {
        mHost->ResetNotificationResets();
    }
    if ((mSensor->GetType(&tmp), tmp) == ISensor::TYPE_PICK_UP_GESTURE) {
        DozeLog::TracePickupPulse(withinVibrationThreshold);
    }
    // } finally {
    mHost->mWakeLock->ReleaseLock();
    // }
    return NOERROR;
}

const Int32 CDozeService::ProximityCheck::RESULT_UNKNOWN = 0;
const Int32 CDozeService::ProximityCheck::RESULT_NEAR = 1;
const Int32 CDozeService::ProximityCheck::RESULT_FAR = 2;
const Int32 CDozeService::ProximityCheck::TIMEOUT_DELAY_MS = 500;
CAR_INTERFACE_IMPL_2(CDozeService::ProximityCheck, Object, ISensorEventListener, IRunnable);
CDozeService::ProximityCheck::ProximityCheck(
    /* [in] */ CDozeService* host)
    : mHost(host)
    , mRegistered(FALSE)
    , mFinished(FALSE)
    , mMaxRange(0)
{
    mTag = mHost->mTag + ".ProximityCheck";
}

ECode CDozeService::ProximityCheck::Check()
{
    if (mFinished || mRegistered) return NOERROR;
    AutoPtr<ISensor> sensor;
    mHost->mSensors->GetDefaultSensor(ISensor::TYPE_PROXIMITY, (ISensor**)&sensor);
    if (sensor == NULL) {
        if (DEBUG) Logger::D(mTag, "No sensor found");
        FinishWithResult(RESULT_UNKNOWN);
        return NOERROR;
    }
    // the pickup sensor interferes with the prox event, disable it until we have a result
    mHost->mPickupSensor->SetDisabled(TRUE);

    sensor->GetMaximumRange(&mMaxRange);
    Boolean tmp = FALSE;
    mHost->mSensors->RegisterListener(this, sensor, ISensorManager::SENSOR_DELAY_NORMAL,
        0, mHost->mHandler, &tmp);
    mHost->mHandler->PostDelayed(this, TIMEOUT_DELAY_MS, &tmp);
    mRegistered = TRUE;
    return NOERROR;
}

ECode CDozeService::ProximityCheck::OnSensorChanged(
    /* [in] */ ISensorEvent* event)
{
    AutoPtr<ArrayOf<Float> > values;
    event->GetValues((ArrayOf<Float>**)&values);
    if (values->GetLength() == 0) {
        if (DEBUG) Logger::D(mTag, "Event has no values!");
        FinishWithResult(RESULT_UNKNOWN);
    }
    else {
        if (DEBUG) Logger::D(mTag, "Event: value=%f max=%d", (*values)[0], mMaxRange);
        Boolean isNear = (*values)[0] < mMaxRange;
        FinishWithResult(isNear ? RESULT_NEAR : RESULT_FAR);
    }
    return NOERROR;
}

ECode CDozeService::ProximityCheck::Run()
{
    if (DEBUG) Logger::D(mTag, "No event received before timeout");
    FinishWithResult(RESULT_UNKNOWN);
    return NOERROR;
}

void CDozeService::ProximityCheck::FinishWithResult(
    /* [in] */ Int32 result)
{
    if (mFinished) return;
    if (mRegistered) {
        mHost->mHandler->RemoveCallbacks(this);
        mHost->mSensors->UnregisterListener(this);
        // we're done - reenable the pickup sensor
        mHost->mPickupSensor->SetDisabled(FALSE);
        mRegistered = FALSE;
    }
    OnProximityResult(result);
    mFinished = TRUE;
}

ECode CDozeService::ProximityCheck::OnAccuracyChanged(
    /* [in] */ ISensor* sensor,
    /* [in] */ Int32 accuracy)
{
    // noop
    return NOERROR;
}

CAR_INTERFACE_IMPL(CDozeService::ProximityCheckImpl::PulseCallback, Object, IDozeHostPulseCallback)
CDozeService::ProximityCheckImpl::PulseCallback::PulseCallback(
    /* [in] */ ProximityCheckImpl* host)
    : mHost(host)
{}

ECode CDozeService::ProximityCheckImpl::PulseCallback::OnPulseStarted()
{
    if (mHost->mHost->mPulsing && mHost->mHost->mDreaming) {
        mHost->mHost->TurnDisplayOn();
    }
    return NOERROR;
}

ECode CDozeService::ProximityCheckImpl::PulseCallback::OnPulseFinished()
{
    if (mHost->mHost->mPulsing && mHost->mHost->mDreaming) {
        mHost->mHost->mPulsing = FALSE;
        mHost->mHost->TurnDisplayOff();
    }
    mHost->mHost->mWakeLock->ReleaseLock(); // needs to be unconditional to balance acquire
    return NOERROR;
}

CDozeService::ProximityCheckImpl::ProximityCheckImpl(
    /* [in] */ CDozeService* host,
    /* [in] */ Int64 start)
    : ProximityCheck(host)
    , mStart(start)
{}

ECode CDozeService::ProximityCheckImpl::OnProximityResult(
    /* [in] */ Int32 result)
{
    // avoid pulsing in pockets
    Boolean isNear = result == RESULT_NEAR;
    DozeLog::TraceProximityResult(isNear, SystemClock::GetUptimeMillis() - mStart);
    if (isNear) {
        mHost->mPulsing = FALSE;
        mHost->mWakeLock->ReleaseLock();
        return NOERROR;
    }

    // not in-pocket, continue pulsing
    AutoPtr<PulseCallback> callback = new PulseCallback(this);
    mHost->mHost->PulseWhileDozing(callback);
    return NOERROR;
}

CDozeService::DSRunnable::DSRunnable(
    /* [in] */ CDozeService* host)
    : mHost(host)
{}

ECode CDozeService::DSRunnable::Run()
{
    if (mHost->mDreaming) {
        mHost->StartDozing();

        // From this point until onDreamingStopped we will need to hold a
        // wakelock whenever we are doing work.  Note that we never call
        // stopDozing because can we just keep dozing until the bitter end.
    }
    return NOERROR;
}


CAR_OBJECT_IMPL(CDozeService)
CAR_INTERFACE_IMPL(CDozeService, DreamService, IDozeService)
CDozeService::CDozeService()
    : mDreaming(FALSE)
    , mPulsing(FALSE)
    , mBroadcastReceiverRegistered(FALSE)
    , mDisplayStateSupported(FALSE)
    , mNotificationLightOn(FALSE)
    , mPowerSaveActive(FALSE)
    , mCarMode(FALSE)
    , mNotificationPulseTime(0)
    , mScheduleResetsRemaining(0)
{
    Int32 hc = 0;
    GetHashCode(&hc);
    String value;
    value.AppendFormat(".%08x", hc);
    mTag = TAG + value;
    mContext = this;
    mDozeParameters = new DozeParameters(mContext);
    CHandler::New((IHandler**)&mHandler);
    mBroadcastReceiver = new DozeBroadcastReceiver(this);
    mHostCallback = new DozeHostCallback(this);
}

ECode CDozeService::constructor()
{
    if (DEBUG) Logger::D(mTag, "new DozeService()");
    SetDebug(DEBUG);
    return NOERROR;
}

void CDozeService::DumpOnHandler(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    assert(0 && "TODO");
    // DreamService::DumpOnHandler(fd, pw, args);
    pw->Print(String("  mDreaming: "));
    pw->Println(mDreaming);
    pw->Print(String("  mPulsing: "));
    pw->Println(mPulsing);
    pw->Print(String("  mWakeLock: held="));
    Boolean tmp = FALSE;
    mWakeLock->IsHeld(&tmp);
    pw->Println(tmp);
    pw->Print(String("  mHost: "));
    pw->Println(mHost);
    pw->Print(String("  mBroadcastReceiverRegistered: "));
    pw->Println(mBroadcastReceiverRegistered);
    pw->Print(String("  mSigMotionSensor: "));
    pw->Println(mSigMotionSensor->Probe(EIID_IInterface));
    pw->Print(String("  mPickupSensor:"));
    pw->Println(mPickupSensor->Probe(EIID_IInterface));
    pw->Print(String("  mDisplayStateSupported: "));
    pw->Println(mDisplayStateSupported);
    pw->Print(String("  mNotificationLightOn: "));
    pw->Println(mNotificationLightOn);
    pw->Print(String("  mPowerSaveActive: "));
    pw->Println(mPowerSaveActive);
    pw->Print(String("  mCarMode: "));
    pw->Println(mCarMode);
    pw->Print(String("  mNotificationPulseTime: "));
    pw->Println(mNotificationPulseTime);
    pw->Print(String("  mScheduleResetsRemaining: "));
    pw->Println(mScheduleResetsRemaining);
    mDozeParameters->Dump(pw);
}

ECode CDozeService::OnCreate()
{
    if (DEBUG) Logger::D(mTag, "onCreate");
    DreamService::OnCreate();

    AutoPtr<IApplication> a;
    GetApplication((IApplication**)&a);
    if (ISystemUIApplication::Probe(a)) {
        AutoPtr<ISystemUIApplication> app = ISystemUIApplication::Probe(a);
        AutoPtr<IInterface> obj;
        app->GetComponent(String("EIID_IDozeHost")/*.class*/, (IInterface**)&obj);
        mHost = IDozeHost::Probe(obj);
    }
    if (mHost == NULL) Logger::W(TAG, "No doze service host found.");

    SetWindowless(TRUE);

    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::SENSOR_SERVICE, (IInterface**)&obj);
    mSensors = ISensorManager::Probe(obj);
    Boolean tmp1 = mDozeParameters->GetPulseOnSigMotion();
    Boolean tmp2 = mDozeParameters->GetVibrateOnSigMotion();
    mSigMotionSensor = new TriggerSensor(ISensor::TYPE_SIGNIFICANT_MOTION, tmp1, tmp2, this);

    tmp1 = mDozeParameters->GetPulseOnPickup();
    tmp2 = mDozeParameters->GetVibrateOnPickup();
    mPickupSensor = new TriggerSensor(ISensor::TYPE_PICK_UP_GESTURE, tmp1, tmp2, this);

    obj = NULL;
    mContext->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&obj);
    mPowerManager = IPowerManager::Probe(obj);
    mPowerManager->NewWakeLock(IPowerManager::PARTIAL_WAKE_LOCK, mTag,
        (IPowerManagerWakeLock**)&mWakeLock);
    mWakeLock->SetReferenceCounted(TRUE);

    obj = NULL;
    mContext->GetSystemService(IContext::ALARM_SERVICE, (IInterface**)&obj);
    mAlarmManager = IAlarmManager::Probe(obj);
    mDisplayStateSupported = mDozeParameters->GetDisplayStateSupported();

    obj = NULL;
    mContext->GetSystemService(IContext::UI_MODE_SERVICE, (IInterface**)&obj);
    mUiModeManager = IUiModeManager::Probe(obj);
    TurnDisplayOff();
    return NOERROR;
}

ECode CDozeService::OnAttachedToWindow()
{
    if (DEBUG) Logger::D(mTag, "onAttachedToWindow");
    DreamService::OnAttachedToWindow();
    return NOERROR;
}

ECode CDozeService::OnDreamingStarted()
{
    DreamService::OnDreamingStarted();

    if (mHost == NULL) {
        Finish();
        return NOERROR;
    }

    mHost->IsPowerSaveActive(&mPowerSaveActive);
    Int32 tmp = 0;
    mCarMode = (mUiModeManager->GetCurrentModeType(&tmp), tmp) == IConfiguration::UI_MODE_TYPE_CAR;
    Boolean can = FALSE;
    CanDoze(&can);
    if (DEBUG) Logger::D(mTag, "onDreamingStarted canDoze=%d mPowerSaveActive=%d mCarMode=%d",
        can ,mPowerSaveActive, mCarMode);
    if (mPowerSaveActive) {
        FinishToSavePower();
        return NOERROR;
    }
    if (mCarMode) {
        FinishForCarMode();
        return NOERROR;
    }

    mDreaming = TRUE;
    ListenForPulseSignals(TRUE);
    RescheduleNotificationPulse(FALSE /*predicate*/);  // cancel any pending pulse alarms

    // Ask the host to get things ready to start dozing.
    // Once ready, we call startDozing() at which point the CPU may suspend
    // and we will need to acquire a wakelock to do work.
    AutoPtr<DSRunnable> run = new DSRunnable(this);
    mHost->StartDozing(run);
    return NOERROR;
}

ECode CDozeService::OnDreamingStopped()
{
    Boolean dozing = FALSE;
    IsDozing(&dozing);
    if (DEBUG) Logger::D(mTag, "onDreamingStopped isDozing=%d", dozing);
    DreamService::OnDreamingStopped();

    if (mHost == NULL) {
        return NOERROR;
    }

    mDreaming = FALSE;
    ListenForPulseSignals(FALSE);

    // Tell the host that it's over.
    mHost->StopDozing();
    return NOERROR;
}

void CDozeService::RequestPulse()
{
    if (mHost != NULL && mDreaming && !mPulsing) {
        // Let the host know we want to pulse.  Wait for it to be ready, then
        // turn the screen on.  When finished, turn the screen off again.
        // Here we need a wakelock to stay awake until the pulse is finished.
        mWakeLock->AcquireLock();
        mPulsing = TRUE;
        Int64 start = SystemClock::GetUptimeMillis();
        AutoPtr<ProximityCheck> check = new ProximityCheckImpl(this, start);
        check->Check();
    }
}

void CDozeService::TurnDisplayOff()
{
    if (DEBUG) Logger::D(mTag, "Display off");
    SetDozeScreenState(IDisplay::STATE_OFF);
}

void CDozeService::TurnDisplayOn()
{
    if (DEBUG) Logger::D(mTag, "Display on");
    SetDozeScreenState(mDisplayStateSupported ? IDisplay::STATE_DOZE : IDisplay::STATE_ON);
}

void CDozeService::FinishToSavePower()
{
    Logger::W(mTag, "Exiting ambient mode due to low power battery saver");
    Finish();
}

void CDozeService::FinishForCarMode()
{
    Logger::W(mTag, "Exiting ambient mode, not allowed in car mode");
    Finish();
}

void CDozeService::ListenForPulseSignals(
    /* [in] */ Boolean listen)
{
    if (DEBUG) Logger::D(mTag, "ListenForPulseSignals: " + listen);
    mSigMotionSensor->SetListening(listen);
    mPickupSensor->SetListening(listen);
    ListenForBroadcasts(listen);
    ListenForNotifications(listen);
}

void CDozeService::ListenForBroadcasts(
    /* [in] */ Boolean listen)
{
    if (listen) {
        AutoPtr<IIntentFilter> filter;
        CIntentFilter::New(PULSE_ACTION, (IIntentFilter**)&filter);
        filter->AddAction(NOTIFICATION_PULSE_ACTION);
        filter->AddAction(IUiModeManager::ACTION_ENTER_CAR_MODE);
        AutoPtr<IIntent> tmp;
        mContext->RegisterReceiver(mBroadcastReceiver, filter, (IIntent**)&tmp);
        mBroadcastReceiverRegistered = TRUE;
    }
    else {
        if (mBroadcastReceiverRegistered) {
            mContext->UnregisterReceiver(mBroadcastReceiver);
        }
        mBroadcastReceiverRegistered = FALSE;
    }
}

void CDozeService::ListenForNotifications(
    /* [in] */ Boolean listen)
{
    if (listen) {
        ResetNotificationResets();
        mHost->AddCallback(mHostCallback);
    }
    else {
        mHost->RemoveCallback(mHostCallback);
    }
}

void CDozeService::ResetNotificationResets()
{
    if (DEBUG) Logger::D(mTag, "ResetNotificationResets");
    mScheduleResetsRemaining = mDozeParameters->GetPulseScheduleResets();
}

void CDozeService::UpdateNotificationPulse()
{
    if (DEBUG) Logger::D(mTag, "UpdateNotificationPulse");
    if (!mDozeParameters->GetPulseOnNotifications()) return;
    if (mScheduleResetsRemaining <= 0) {
        if (DEBUG) Logger::D(mTag, "No more schedule resets remaining");
        return;
    }
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    Int64 now = 0;
    system->GetCurrentTimeMillis(&now);
    if ((now - mNotificationPulseTime) < mDozeParameters->GetPulseDuration()) {
        if (DEBUG) Logger::D(mTag, "Recently updated, not resetting schedule");
        return;
    }
    mScheduleResetsRemaining--;
    if (DEBUG) Logger::D(mTag, "mScheduleResetsRemaining = " + mScheduleResetsRemaining);
    mNotificationPulseTime = now;
    RescheduleNotificationPulse(TRUE /*predicate*/);
}

AutoPtr<IPendingIntent> CDozeService::NotificationPulseIntent(
    /* [in] */ Int64 instance)
{
    AutoPtr<IPendingIntentHelper> helper;
    CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&helper);
    AutoPtr<IIntent> intent;
    CIntent::New(NOTIFICATION_PULSE_ACTION, (IIntent**)&intent);
    String name;
    GetPackageName(&name);
    intent->SetPackage(name);
    intent->PutExtra(EXTRA_INSTANCE, instance);
    intent->SetFlags(IIntent::FLAG_RECEIVER_FOREGROUND);
    AutoPtr<IPendingIntent> pIntent;
    helper->GetBroadcast(mContext, 0, intent,
            IPendingIntent::FLAG_UPDATE_CURRENT, (IPendingIntent**)&pIntent);
    return pIntent;
}

void CDozeService::RescheduleNotificationPulse(
    /* [in] */ Boolean predicate)
{
    if (DEBUG) Logger::D(mTag, "RescheduleNotificationPulse predicate=%d", predicate);
    AutoPtr<IPendingIntent> notificationPulseIntent = NotificationPulseIntent(0);
    mAlarmManager->Cancel(notificationPulseIntent);
    if (!predicate) {
        if (DEBUG) Logger::D(mTag, "  don't reschedule: predicate is FALSE");
        return;
    }
    AutoPtr<DozeParameters::PulseSchedule> schedule = mDozeParameters->GetPulseSchedule();
    if (schedule == NULL) {
        if (DEBUG) Logger::D(mTag, "  don't reschedule: schedule is NULL");
        return;
    }
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    Int64 now = 0;
    system->GetCurrentTimeMillis(&now);
    Int64 time = schedule->GetNextTime(now, mNotificationPulseTime);
    if (time <= 0) {
        if (DEBUG) Logger::D(mTag, "  don't reschedule: time is %lld", time);
        return;
    }
    const Int64 delta = time - now;
    if (delta <= 0) {
        if (DEBUG) Logger::D(mTag, "  don't reschedule: delta is %lld", delta);
        return;
    }
    const Int64 instance = time - mNotificationPulseTime;
    AutoPtr<IDate> date;
    CDate::New(time, (IDate**)&date);
    String str;
    date->ToString(&str);
    if (DEBUG) Logger::D(mTag, "Scheduling pulse %lld in %lld ms for %s", instance, delta, str.string());
    AutoPtr<IPendingIntent> intent = NotificationPulseIntent(instance);
    mAlarmManager->SetExact(IAlarmManager::RTC_WAKEUP, time, intent);
}

String CDozeService::TriggerEventToString(
    /* [in] */ ITriggerEvent* event)
{
    if (event == NULL) return String(NULL);
    StringBuilder sb("TriggerEvent[");
    Int64 timestamp = 0;
    event->GetTimestamp(&timestamp);
    sb.Append(timestamp);
    sb.AppendChar(',');
    AutoPtr<ISensor> sensor;
    event->GetSensor((ISensor**)&sensor);
    String name;
    sensor->GetName(&name);
    sb.Append(name);

    AutoPtr<ArrayOf<Float> > values;
    event->GetValues((ArrayOf<Float>**)&values);
    if (values != NULL) {
        for (Int32 i = 0; i < values->GetLength(); i++) {
            sb.AppendChar(',');
            sb.Append((*values)[i]);
        }
    }
    sb.AppendChar(']');
    return sb.ToString();
}

} // namespace Doze
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
