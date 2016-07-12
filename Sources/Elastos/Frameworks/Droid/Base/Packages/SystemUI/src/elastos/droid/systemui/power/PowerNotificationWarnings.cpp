#include "elastos/droid/systemui/power/PowerNotificationWarnings.h"
#include "elastos/droid/systemui/statusbar/phone/CSystemUIDialog.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "Elastos.CoreLibrary.Core.h"
#include "elastos/droid/os/AsyncTask.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/systemui/power/CPowerUI.h"
#include "elastos/droid/R.h"
#include "R.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::App::CNotificationBuilder;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::INotification;
using Elastos::Droid::App::CPendingIntentHelper;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::App::IPendingIntentHelper;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnDismissListener;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Media::CAudioAttributesBuilder;
using Elastos::Droid::Media::IAudioAttributesBuilder;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Os::AsyncTask;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::IIPowerManager;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::SystemUI::Power::EIID_IPowerUIWarningsUI;
using Elastos::Droid::SystemUI::StatusBar::Phone::CSystemUIDialog;
using Elastos::Droid::R;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::IRemoteViews;
using Elastos::Core::CInteger32;
using Elastos::Core::CSystem;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::IInteger32;
using Elastos::Core::ISystem;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Power {

//-----------------------------------------------------------------------------------
// PowerNotificationWarnings::Receiver
//-----------------------------------------------------------------------------------

PowerNotificationWarnings::Receiver::Receiver(
    /* [in] */ PowerNotificationWarnings* host)
    : mHost(host)
{}

void PowerNotificationWarnings::Receiver::Init()
{
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(PowerNotificationWarnings::ACTION_SHOW_BATTERY_SETTINGS);
    filter->AddAction(PowerNotificationWarnings::ACTION_START_SAVER);
    filter->AddAction(PowerNotificationWarnings::ACTION_STOP_SAVER);
    AutoPtr<IIntent> stickyIntent;
    mHost->mContext->RegisterReceiver((IBroadcastReceiver*)this, filter, String(NULL), mHost->mHandler, (IIntent**)&stickyIntent);
}

ECode PowerNotificationWarnings::Receiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (action.Equals(PowerNotificationWarnings::ACTION_SHOW_BATTERY_SETTINGS)) {
        mHost->DismissLowBatteryNotification();
        AutoPtr<IUserHandleHelper> uhh;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&uhh);
        AutoPtr<IUserHandle> current;
        uhh->GetCURRENT((IUserHandle**)&current);
        mHost->mContext->StartActivityAsUser(mHost->mOpenBatterySettings, current);
    }
    else if (action.Equals(PowerNotificationWarnings::ACTION_START_SAVER)) {
        mHost->DismissLowBatteryNotification();
        mHost->ShowStartSaverConfirmation();
    }
    else if (action.Equals(PowerNotificationWarnings::ACTION_STOP_SAVER)) {
        mHost->DismissSaverNotification();
        mHost->DismissLowBatteryNotification();
        mHost->SetSaverMode(FALSE);
    }
    return NOERROR;
}

//-----------------------------------------------------------------------------------
// PowerNotificationWarnings::MyOnClickRunnable
//-----------------------------------------------------------------------------------

PowerNotificationWarnings::MyOnClickRunnable::MyOnClickRunnable(
    /* [in] */ PowerNotificationWarnings* host)
    : mHost(host)
{}

ECode PowerNotificationWarnings::MyOnClickRunnable::Run()
{
    mHost->SetSaverMode(TRUE);
    return NOERROR;
}

//-----------------------------------------------------------------------------------
// PowerNotificationWarnings::MyOnClickListener
//-----------------------------------------------------------------------------------

CAR_INTERFACE_IMPL(PowerNotificationWarnings::MyOnClickListener, Object, IDialogInterfaceOnClickListener)

PowerNotificationWarnings::MyOnClickListener::MyOnClickListener(
    /* [in] */ PowerNotificationWarnings* host)
    : mHost(host)
{}

ECode PowerNotificationWarnings::MyOnClickListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    AutoPtr<MyOnClickRunnable> task = new MyOnClickRunnable(mHost);
    AsyncTask::Execute(task);
    return NOERROR;
}

//-----------------------------------------------------------------------------------
// PowerNotificationWarnings::MyOnDismissListener
//-----------------------------------------------------------------------------------

CAR_INTERFACE_IMPL(PowerNotificationWarnings::MyOnDismissListener, Object, IDialogInterfaceOnDismissListener)

PowerNotificationWarnings::MyOnDismissListener::MyOnDismissListener(
    /* [in] */ PowerNotificationWarnings* host)
    : mHost(host)
{}

ECode PowerNotificationWarnings::MyOnDismissListener::OnDismiss(
    /* [in] */ IDialogInterface* dialog)
{
    mHost->mSaverConfirmation = NULL;
    return NOERROR;
}

//-----------------------------------------------------------------------------------
// PowerNotificationWarnings
//-----------------------------------------------------------------------------------

const String PowerNotificationWarnings::TAG("PowerNotificationWarnings");
const Boolean PowerNotificationWarnings::DEBUG = FALSE;

const String PowerNotificationWarnings::TAG_NOTIFICATION("low_battery");
const Int32 PowerNotificationWarnings::ID_NOTIFICATION;

const Int32 PowerNotificationWarnings::SHOWING_NOTHING;
const Int32 PowerNotificationWarnings::SHOWING_WARNING;
const Int32 PowerNotificationWarnings::SHOWING_SAVER;
const Int32 PowerNotificationWarnings::SHOWING_INVALID_CHARGER;

static AutoPtr<ArrayOf<String> > InitSHOWING_STRINGS()
{
    AutoPtr<ArrayOf<String> > strs = ArrayOf<String>::Alloc(4);
    (*strs)[0] = String("SHOWING_NOTHING");
    (*strs)[1] = String("SHOWING_WARNING");
    (*strs)[2] = String("SHOWING_SAVER");
    (*strs)[3] = String("SHOWING_INVALID_CHARGER");
    return strs;
}

const AutoPtr<ArrayOf<String> > PowerNotificationWarnings::SHOWING_STRINGS = InitSHOWING_STRINGS();
const String PowerNotificationWarnings::ACTION_SHOW_BATTERY_SETTINGS("PNW.batterySettings");
const String PowerNotificationWarnings::ACTION_START_SAVER("PNW.startSaver");
const String PowerNotificationWarnings::ACTION_STOP_SAVER("PNW.stopSaver");

static AutoPtr<IAudioAttributes> InitAUDIO_ATTRIBUTES()
{
    AutoPtr<IAudioAttributesBuilder> aab;
    CAudioAttributesBuilder::New((IAudioAttributesBuilder**)&aab);
    aab->SetContentType(IAudioAttributes::CONTENT_TYPE_SONIFICATION);
    aab->SetUsage(IAudioAttributes::USAGE_ASSISTANCE_SONIFICATION);
    AutoPtr<IAudioAttributes> aa;
    aab->Build((IAudioAttributes**)&aa);
    return aa;
}

const AutoPtr<IAudioAttributes> PowerNotificationWarnings::AUDIO_ATTRIBUTES = InitAUDIO_ATTRIBUTES();

CAR_INTERFACE_IMPL(PowerNotificationWarnings, Object, IPowerUIWarningsUI)

PowerNotificationWarnings::PowerNotificationWarnings()
    : mBatteryLevel(0)
    , mBucket(0)
    , mScreenOffTime(0L)
    , mShowing(0)
    , mBucketDroppedNegativeTimeMs(0L)
    , mSaver(FALSE)
    , mWarning(FALSE)
    , mPlaySound(FALSE)
    , mInvalidCharger(FALSE)
{}

ECode PowerNotificationWarnings::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IPhoneStatusBar* phoneStatusBar)
{
    mContext = context;
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::NOTIFICATION_SERVICE, (IInterface**)&obj);
    mNoMan = INotificationManager::Probe(obj);
    AutoPtr<IInterface> obj2;
    context->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&obj2);
    mPowerMan = IPowerManager::Probe(obj2);
    mStartSaverMode = new MyOnClickListener(this);

    CHandler::New((IHandler**)&mHandler);
    mReceiver = new Receiver(this);
    mReceiver->Init();
    mOpenBatterySettings = Settings(IIntent::ACTION_POWER_USAGE_SUMMARY);
    mOpenSaverSettings = Settings(ISettings::ACTION_BATTERY_SAVER_SETTINGS);
    return NOERROR;
}

ECode PowerNotificationWarnings::Update(
    /* [in] */ Int32 batteryLevel,
    /* [in] */ Int32 bucket,
    /* [in] */ Int64 screenOffTime)
{
    mBatteryLevel = batteryLevel;
    if (bucket >= 0) {
        mBucketDroppedNegativeTimeMs = 0;
    }
    else if (bucket < mBucket) {
        AutoPtr<ISystem> sys;
        CSystem::AcquireSingleton((ISystem**)&sys);
        sys->GetCurrentTimeMillis(&mBucketDroppedNegativeTimeMs);
    }
    mBucket = bucket;
    mScreenOffTime = screenOffTime;
    return NOERROR;
}

ECode PowerNotificationWarnings::ShowSaverMode(
    /* [in] */ Boolean mode)
{
    mSaver = mode;
    if (mSaver && mSaverConfirmation != NULL) {
        IDialogInterface::Probe(mSaverConfirmation)->Dismiss();
    }
    UpdateNotification();
    return NOERROR;
}

void PowerNotificationWarnings::UpdateNotification()
{
    if (DEBUG) {
        Slogger::D(TAG, "updateNotification mWarning=%s mPlaySound=%s mSaver=%s mInvalidCharger=%s",
            mWarning ? "TRUE" : "FALSE",
            mPlaySound ? "TRUE" : "FALSE",
            mSaver ? "TRUE" : "FALSE",
            mInvalidCharger ? "TRUE" : "FALSE");
    }
    if (mInvalidCharger) {
        ShowInvalidChargerNotification();
        mShowing = SHOWING_INVALID_CHARGER;
    } else if (mWarning) {
        ShowWarningNotification();
        mShowing = SHOWING_WARNING;
    } else if (mSaver) {
        ShowSaverNotification();
        mShowing = SHOWING_SAVER;
    } else {
        mNoMan->Cancel(TAG_NOTIFICATION, ID_NOTIFICATION);
        mShowing = SHOWING_NOTHING;
    }
}

void PowerNotificationWarnings::ShowInvalidChargerNotification()
{
    AutoPtr<INotificationBuilder> nb;
    CNotificationBuilder::New(mContext, (INotificationBuilder**)&nb);
    nb->SetSmallIcon(R::drawable::ic_power_low);
    nb->SetWhen(0);
    nb->SetShowWhen(FALSE);
    nb->SetOngoing(TRUE);
    String str1;
    mContext->GetString(R::string::invalid_charger_title, &str1);
    AutoPtr<ICharSequence> cs1;
    CString::New(str1, (ICharSequence**)&cs1);
    nb->SetContentTitle(cs1);
    String str2;
    mContext->GetString(R::string::invalid_charger_text, &str2);
    AutoPtr<ICharSequence> cs2;
    CString::New(str2, (ICharSequence**)&cs2);
    nb->SetContentText(cs2);
    nb->SetPriority(INotification::PRIORITY_MAX);
    nb->SetCategory(INotification::CATEGORY_SYSTEM);
    nb->SetVisibility(INotification::VISIBILITY_PUBLIC);
    AutoPtr<IResources> resources;
    mContext->GetResources((IResources**)&resources);
    Int32 color;
    resources->GetColor(Elastos::Droid::R::color::system_notification_accent_color, &color);
    nb->SetColor(color);
    AutoPtr<INotification> n;
    nb->Build((INotification**)&n);

    AutoPtr<IRemoteViews> rv;
    n->GetHeadsUpContentView((IRemoteViews**)&rv);
    if (rv != NULL) {
        rv->SetViewVisibility(Elastos::Droid::R::id::right_icon, IView::GONE);
    }
    AutoPtr<IUserHandleHelper> uhh;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&uhh);
    AutoPtr<IUserHandle> uh;
    uhh->GetCURRENT((IUserHandle**)&uh);
    mNoMan->NotifyAsUser(TAG_NOTIFICATION, ID_NOTIFICATION, n, uh);
}

void PowerNotificationWarnings::ShowWarningNotification()
{
    const Int32 textRes = mSaver ? R::string::battery_low_percent_format_saver_started
        : R::string::battery_low_percent_format;
    AutoPtr<INotificationBuilder> nb;
    CNotificationBuilder::New(mContext, (INotificationBuilder**)&nb);
    nb->SetSmallIcon(R::drawable::ic_power_low);
    // Bump the notification when the bucket dropped.
    nb->SetWhen(mBucketDroppedNegativeTimeMs);
    String ct;
    mContext->GetString(R::string::battery_low_title, &ct);
    AutoPtr<ICharSequence> ctcs;
    CString::New(ct, (ICharSequence**)&ctcs);
    nb->SetContentTitle(ctcs);
    AutoPtr<IInteger32> i;
    CInteger32::New(mBatteryLevel, (IInteger32**)&i);
    AutoPtr<ArrayOf<IInterface*> > array = ArrayOf<IInterface*>::Alloc(1);
    array->Set(0, i);
    String ct2;
    mContext->GetString(textRes, array, &ct2);
    AutoPtr<ICharSequence> ctcs2;
    CString::New(ct2, (ICharSequence**)&ctcs2);
    nb->SetContentText(ctcs2);
    nb->SetOnlyAlertOnce(TRUE);
    nb->SetPriority(INotification::PRIORITY_MAX);
    nb->SetCategory(INotification::CATEGORY_SYSTEM);
    nb->SetVisibility(INotification::VISIBILITY_PUBLIC);

    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    Int32 color;
    res->GetColor(Elastos::Droid::R::color::battery_saver_mode_color, &color);
    nb->SetColor(color);

    if (HasBatterySettings()) {
        nb->SetContentIntent(PendingBroadcast(ACTION_SHOW_BATTERY_SETTINGS));
    }
    if (!mSaver) {
        String str;
        mContext->GetString(R::string::battery_saver_start_action, &str);
        AutoPtr<ICharSequence> cs;
        CString::New(str, (ICharSequence**)&cs);
        nb->AddAction(0, cs, PendingBroadcast(ACTION_START_SAVER));
    }
    else {
        AddStopSaverAction(nb);
    }
    if (mPlaySound) {
        AttachLowBatterySound(nb);
        mPlaySound = FALSE;
    }
    AutoPtr<INotification> n;
    nb->Build((INotification**)&n);
    AutoPtr<IRemoteViews> headsUpContentView;
    n->GetHeadsUpContentView((IRemoteViews**)&headsUpContentView);
    if (headsUpContentView != NULL) {
        headsUpContentView->SetViewVisibility(Elastos::Droid::R::id::right_icon, IView::GONE);
    }
    AutoPtr<IUserHandleHelper> uhh;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&uhh);
    AutoPtr<IUserHandle> uh;
    uhh->GetCURRENT((IUserHandle**)&uh);
    mNoMan->NotifyAsUser(TAG_NOTIFICATION, ID_NOTIFICATION, n, uh);
}

void PowerNotificationWarnings::ShowSaverNotification()
{
    AutoPtr<INotificationBuilder> nb;
    CNotificationBuilder::New(mContext, (INotificationBuilder**)&nb);
    nb->SetSmallIcon(R::drawable::ic_power_saver);

    String ct;
    mContext->GetString(R::string::battery_saver_notification_title, &ct);
    AutoPtr<ICharSequence> ctcs;
    CString::New(ct, (ICharSequence**)&ctcs);
    nb->SetContentTitle(ctcs);

    String ct2;
    mContext->GetString(R::string::battery_saver_notification_text, &ct2);
    AutoPtr<ICharSequence> ctcs2;
    CString::New(ct2, (ICharSequence**)&ctcs2);
    nb->SetContentText(ctcs2);
    nb->SetOngoing(TRUE);
    nb->SetShowWhen(FALSE);
    nb->SetCategory(INotification::CATEGORY_SYSTEM);
    nb->SetVisibility(INotification::VISIBILITY_PUBLIC);

    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    Int32 color;
    res->GetColor(Elastos::Droid::R::color::battery_saver_mode_color, &color);
    nb->SetColor(color);

    AddStopSaverAction(nb);
    if (HasSaverSettings()) {
        nb->SetContentIntent(PendingActivity(mOpenSaverSettings));
    }
    AutoPtr<INotification> n;
    nb->Build((INotification**)&n);
    AutoPtr<IUserHandleHelper> uhh;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&uhh);
    AutoPtr<IUserHandle> uh;
    uhh->GetCURRENT((IUserHandle**)&uh);
    mNoMan->NotifyAsUser(TAG_NOTIFICATION, ID_NOTIFICATION, n, uh);
}

void PowerNotificationWarnings::AddStopSaverAction(
    /* [in] */ INotificationBuilder* nb)
{
    String ct;
    mContext->GetString(R::string::battery_saver_notification_action_text, &ct);
    AutoPtr<ICharSequence> cs;
    CString::New(ct, (ICharSequence**)&cs);
    nb->AddAction(0, cs, PendingBroadcast(ACTION_STOP_SAVER));
}

void PowerNotificationWarnings::DismissSaverNotification()
{
    if (mSaver) Slogger::I(TAG, "dismissing saver notification");
    mSaver = FALSE;
    UpdateNotification();
}

AutoPtr<IPendingIntent> PowerNotificationWarnings::PendingActivity(
    /* [in] */ IIntent* intent)
{
    AutoPtr<IUserHandleHelper> uhh;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&uhh);
    AutoPtr<IUserHandle> uh;
    uhh->GetCURRENT((IUserHandle**)&uh);
    AutoPtr<IPendingIntentHelper> pih;
    CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&pih);
    AutoPtr<IPendingIntent> pi;
    pih->GetActivityAsUser(mContext, 0, intent, 0, NULL, uh,
        (IPendingIntent**)&pi);
    return pi;
}

AutoPtr<IPendingIntent> PowerNotificationWarnings::PendingBroadcast(
    /* [in] */ const String& action)
{
    AutoPtr<IUserHandleHelper> uhh;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&uhh);
    AutoPtr<IUserHandle> uh;
    uhh->GetCURRENT((IUserHandle**)&uh);
    AutoPtr<IPendingIntentHelper> pih;
    CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&pih);
    AutoPtr<IIntent> intent;
    CIntent::New(action, (IIntent**)&intent);
    AutoPtr<IPendingIntent> pi;
    pih->GetBroadcastAsUser(mContext,0, intent, 0, uh,
        (IPendingIntent**)&pi);
    return pi;
}

AutoPtr<IIntent> PowerNotificationWarnings::Settings(
    /* [in] */ const String& action)
{
    AutoPtr<IIntent> intent;
    CIntent::New(action, (IIntent**)&intent);
    intent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK
        | IIntent::FLAG_ACTIVITY_MULTIPLE_TASK
        | IIntent::FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS
        | IIntent::FLAG_ACTIVITY_NO_HISTORY
        | IIntent::FLAG_ACTIVITY_CLEAR_TOP);
    return intent;
}

ECode PowerNotificationWarnings::IsInvalidChargerWarningShowing(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mInvalidCharger;
    return NOERROR;
}

ECode PowerNotificationWarnings::UpdateLowBatteryWarning()
{
    UpdateNotification();
    return NOERROR;
}

ECode PowerNotificationWarnings::DismissLowBatteryWarning()
{
    if (DEBUG) Slogger::D(TAG, "dismissing low battery warning: level=%d", mBatteryLevel);
    DismissLowBatteryNotification();
    return NOERROR;
}

void PowerNotificationWarnings::DismissLowBatteryNotification()
{
    if (mWarning) Slogger::I(TAG, "dismissing low battery notification");
    mWarning = FALSE;
    UpdateNotification();
}

Boolean PowerNotificationWarnings::HasBatterySettings()
{
    AutoPtr<IPackageManager> pm;
    mContext->GetPackageManager((IPackageManager**)&pm);
    AutoPtr<IComponentName> cn;
    mOpenBatterySettings->ResolveActivity(pm, (IComponentName**)&cn);
    return cn != NULL;
}

Boolean PowerNotificationWarnings::HasSaverSettings()
{
    AutoPtr<IPackageManager> pm;
    mContext->GetPackageManager((IPackageManager**)&pm);
    AutoPtr<IComponentName> cn;
    mOpenBatterySettings->ResolveActivity(pm, (IComponentName**)&cn);
    return cn != NULL;
}

ECode PowerNotificationWarnings::ShowLowBatteryWarning(
    /* [in] */ Boolean playSound)
{
    Slogger::I(TAG,
        "show low battery warning: level=%d [%d] playSound=%s",
        mBatteryLevel, mBucket, playSound ? "TRUE" : "FALSE");
    mPlaySound = playSound;
    mWarning = TRUE;
    UpdateNotification();
    return NOERROR;
}

void PowerNotificationWarnings::AttachLowBatterySound(
    /* [in] */ INotificationBuilder* b)
{
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);

    AutoPtr<ISettingsGlobal> sg;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&sg);
    Int32 silenceAfter;
    sg->GetInt32(cr, ISettingsGlobal::LOW_BATTERY_SOUND_TIMEOUT, 0, &silenceAfter);

    const Int64 offTime = SystemClock::GetElapsedRealtime() - mScreenOffTime;
    if (silenceAfter > 0 && mScreenOffTime > 0 && offTime > silenceAfter) {
        Slogger::I(TAG, "screen off too long (%ldms, limit %dms): not waking up the user with low battery sound", offTime, silenceAfter);
        return;
    }

    if (DEBUG) {
        Slogger::D(TAG, "playing low battery sound. pick-a-doop!"); // WOMP-WOMP is deprecated
    }

    Int32 i;
    sg->GetInt32(cr, ISettingsGlobal::POWER_SOUNDS_ENABLED, 1, &i);
    if (i == 1) {
        String soundPath;
        sg->GetString(cr, ISettingsGlobal::LOW_BATTERY_SOUND, &soundPath);
        if (!soundPath.IsNull()) {
            AutoPtr<IUriHelper> helper;
            CUriHelper::AcquireSingleton((IUriHelper**)&helper);
            AutoPtr<IUri> soundUri;
            helper->Parse(String("file://") + soundPath, (IUri**)&soundUri);
            if (soundUri != NULL) {
                b->SetSound(soundUri, AUDIO_ATTRIBUTES);
                if (DEBUG) Slogger::D(TAG, "playing sound %s", TO_CSTR(soundUri));
            }
        }
    }
}

ECode PowerNotificationWarnings::DismissInvalidChargerWarning()
{
    DismissInvalidChargerNotification();
    return NOERROR;
}

void PowerNotificationWarnings::DismissInvalidChargerNotification()
{
    if (mInvalidCharger) Slogger::I(TAG, "dismissing invalid charger notification");
    mInvalidCharger = FALSE;
    UpdateNotification();
}

ECode PowerNotificationWarnings::ShowInvalidChargerWarning()
{
    mInvalidCharger = TRUE;
    UpdateNotification();
    return NOERROR;
}

void PowerNotificationWarnings::ShowStartSaverConfirmation()
{
    if (mSaverConfirmation != NULL) return;
    AutoPtr<ISystemUIDialog> d;
    CSystemUIDialog::New(mContext, (ISystemUIDialog**)&d);
    AutoPtr<ICharSequence> title;
    CString::New(StringUtils::ToString(R::string::battery_saver_confirmation_title), (ICharSequence**)&title);
    IDialog::Probe(d)->SetTitle(title);

    AutoPtr<ICharSequence> message;
    CString::New(StringUtils::ToString(Elastos::Droid::R::string::battery_saver_description), (ICharSequence**)&message);
    IAlertDialog::Probe(d)->SetMessage(message);

    IAlertDialogBuilder::Probe(d)->SetNegativeButton(Elastos::Droid::R::string::cancel, NULL);
    IAlertDialogBuilder::Probe(d)->SetPositiveButton(R::string::battery_saver_confirmation_ok, mStartSaverMode);
    d->SetShowForAllUsers(TRUE);
    AutoPtr<MyOnDismissListener> odl = new MyOnDismissListener(this);
    IDialog::Probe(d)->SetOnDismissListener(odl);
    IDialog::Probe(d)->Show();
    mSaverConfirmation = d;
}

void PowerNotificationWarnings::SetSaverMode(
    /* [in] */ Boolean mode)
{
    Boolean b;
    IIPowerManager::Probe(mPowerMan)->SetPowerSaveMode(mode, &b);
}

ECode PowerNotificationWarnings::Dump(
    /* [in] */ IPrintWriter* pw)
{
    return NOERROR;
}

} // namespace Power
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos