
#include "CNotificationManagerService.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/app/ActivityManagerNative.h"
#include "elastos/droid/app/AppGlobals.h"
#include "CStatusBarManagerService.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/R.h"
#include "util/Xml.h"

using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Logging::Slogger;
using Elastos::Core::StringBuilder;
using Elastos::Core::CString;
using Elastos::Core::IBoolean;
using Elastos::Core::CBoolean;
using Elastos::Core::StringUtils;
using Elastos::IO::IFile;
using Elastos::IO::CFile;
using Elastos::IO::IIoUtils;
using Elastos::IO::CIoUtils;
using Elastos::IO::IFileOutputStream;
using Elastos::IO::IFileInputStream;
using Org::Xmlpull::V1::IXmlPullParser;
using Elastos::Droid::Utility::Xml;
using Elastos::Droid::Utility::IAtomicFile;
using Elastos::Droid::Utility::CAtomicFile;
using Elastos::Droid::Utility::IFastXmlSerializer;
using Elastos::Droid::Utility::CFastXmlSerializer;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Os::IBinderHelper;
using Elastos::Droid::Os::CBinderHelper;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::Provider::CSettingsSystem;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Droid::App::AppGlobals;
using Elastos::Droid::App::IActivityManagerHelper;
using Elastos::Droid::App::CActivityManagerHelper;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Media::IIRingtonePlayer;
using Elastos::Droid::View::Accessibility::IAccessibilityEvent;
using Elastos::Droid::View::Accessibility::IAccessibilityManager;
using Elastos::Droid::View::Accessibility::CAccessibilityManager;
using Elastos::Droid::View::Accessibility::IAccessibilityManagerHelper;
using Elastos::Droid::View::Accessibility::CAccessibilityManagerHelper;
using Elastos::Droid::View::Accessibility::IAccessibilityEventHelper;
using Elastos::Droid::View::Accessibility::CAccessibilityEventHelper;
using Elastos::Droid::Widget::IRemoteViews;
using Elastos::Droid::Widget::IToast;
using Elastos::Droid::StatusBar::IStatusBarNotification;
using Elastos::Droid::StatusBar::CStatusBarNotification;

namespace Elastos {
namespace Droid {
namespace Server {

//===============================================================================
//                  CNotificationManagerService::NotificationRecord
//===============================================================================
CNotificationManagerService::NotificationRecord::NotificationRecord(
    /* [in] */ const String& pkg,
    /* [in] */ const String& tag,
    /* [in] */ Int32 id,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 initialPid,
    /* [in] */ Int32 userId,
    /* [in] */ Int32 score,
    /* [in] */ INotification* notification)
    : mPkg(pkg)
    , mTag(tag)
    , mId(id)
    , mUid(uid)
    , mInitialPid(initialPid)
    , mUserId(userId)
    , mScore(score)
    , mNotification(notification)
{}

//===============================================================================
//                  CNotificationManagerService::ToastRecord
//===============================================================================
CAR_INTERFACE_IMPL(CNotificationManagerService::ToastRecord, IInterface)

CNotificationManagerService::ToastRecord::ToastRecord(
    /* [in] */ Int32 pid,
    /* [in] */ const String& pkg,
    /* [in] */ IITransientNotification* callback,
    /* [in] */ Int32 duration)
    : mPid(pid)
    , mPkg(pkg)
    , mCallback(callback)
    , mDuration(duration)
{}

void CNotificationManagerService::ToastRecord::Update(
    /* [in] */ Int32 duration)
{
    mDuration = duration;
}

//===============================================================================
//                  CNotificationManagerService::MyNotificationCallbacks
//===============================================================================
CAR_INTERFACE_IMPL(CNotificationManagerService::MyNotificationCallbacks, IStatusBarManagerServiceNotificationCallbacks)

CNotificationManagerService::MyNotificationCallbacks::MyNotificationCallbacks(
    /* [in] */ CNotificationManagerService* host)
    : mHost(host)
{}

ECode CNotificationManagerService::MyNotificationCallbacks::OnSetDisabled(
    /* [in] */ Int32 status)
{
    AutoLock lock(mHost->mNotificationListLock);

    mHost->mDisabledNotifications = status;

    if ((mHost->mDisabledNotifications & IStatusBarManager::DISABLE_NOTIFICATION_ALERTS) != 0) {
        // cancel whatever's going on
        AutoPtr<IBinderHelper> binderHelper;
        CBinderHelper::AcquireSingleton((IBinderHelper**)&binderHelper);
        Int64 identity;

        if (mHost->mAudioService != NULL) {
            binderHelper->ClearCallingIdentity(&identity);
            AutoPtr<IIRingtonePlayer> player;
            mHost->mAudioService->GetRingtonePlayer((IIRingtonePlayer**)&player);

            if (player != NULL) {
                player->StopAsync();
            }
            binderHelper->RestoreCallingIdentity(identity);
        }

        if (mHost->mVibrator != NULL) {
            binderHelper->ClearCallingIdentity(&identity);
            mHost->mVibrator->Cancel();
            binderHelper->RestoreCallingIdentity(identity);
        }
    }
    return NOERROR;
}

ECode CNotificationManagerService::MyNotificationCallbacks::OnClearAll()
{
    // XXX to be totally correct, the caller should tell us which user
    // this is for.
    AutoPtr<IActivityManagerHelper> activityMgrHelper;
    CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&activityMgrHelper);

    Int32 currentUser;
    activityMgrHelper->GetCurrentUser(&currentUser);

    mHost->CancelAll(currentUser);
    return NOERROR;
}

ECode CNotificationManagerService::MyNotificationCallbacks::OnNotificationClick(
    /* [in] */ const String& pkg,
    /* [in] */ const String& tag,
    /* [in] */ Int32 id)
{
    // XXX to be totally correct, the caller should tell us which user
    // this is for.
    AutoPtr<IActivityManagerHelper> activityMgrHelper;
    CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&activityMgrHelper);

    Int32 currentUser;
    activityMgrHelper->GetCurrentUser(&currentUser);

    mHost->CancelNotification(pkg, tag, id, INotification::FLAG_AUTO_CANCEL,
            INotification::FLAG_FOREGROUND_SERVICE, FALSE,
            currentUser);
    return NOERROR;
}

ECode CNotificationManagerService::MyNotificationCallbacks::OnNotificationClear(
    /* [in] */ const String& pkg,
    /* [in] */ const String& tag,
    /* [in] */ Int32 id)
{
    // XXX to be totally correct, the caller should tell us which user
    // this is for.
    AutoPtr<IActivityManagerHelper> activityMgrHelper;
    CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&activityMgrHelper);

    Int32 currentUser;
    activityMgrHelper->GetCurrentUser(&currentUser);

    mHost->CancelNotification(pkg, tag, id, 0,
        INotification::FLAG_ONGOING_EVENT | INotification::FLAG_FOREGROUND_SERVICE,
        TRUE, currentUser);
    return NOERROR;
}

ECode CNotificationManagerService::MyNotificationCallbacks::OnPanelRevealed()
{
    AutoLock lock(mHost->mNotificationListLock);

    // sound
    mHost->mSoundNotification = NULL;

    AutoPtr<IBinderHelper> binderHelper;
    CBinderHelper::AcquireSingleton((IBinderHelper**)&binderHelper);

    Int64 identity;

    if (mHost->mAudioService != NULL) {
        binderHelper->ClearCallingIdentity(&identity);
        AutoPtr<IIRingtonePlayer> player;
        mHost->mAudioService->GetRingtonePlayer((IIRingtonePlayer**)&player);

        if (player != NULL) {
            player->StopAsync();
        }
        binderHelper->RestoreCallingIdentity(identity);
    }

    // vibrate
    mHost->mVibrateNotification = NULL;
    if (mHost->mVibrator != NULL) {
        binderHelper->ClearCallingIdentity(&identity);
        mHost->mVibrator->Cancel();
        binderHelper->RestoreCallingIdentity(identity);
    }

    // light
    mHost->mLights.Clear();
    mHost->mLedNotification = NULL;
    mHost->UpdateLightsLocked();
    return NOERROR;
}

ECode CNotificationManagerService::MyNotificationCallbacks::OnNotificationError(
    /* [in] */ const String& pkg,
    /* [in] */ const String& tag,
    /* [in] */ Int32 id,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 initialPid,
    /* [in] */ const String& message)
{
    StringBuilder buf;
    buf += "onNotification error pkg=";
    buf += pkg;
    buf += " tag=";
    buf += tag;
    buf += " id=";
    buf += id;
    buf += "; will crashApplication(uid=";
    buf += uid;
    buf += ", pid=";
    buf += initialPid;
    buf += ")";
    Logger::D(TAG, buf.ToString().string());

    // XXX to be totally correct, the caller should tell us which user
    // this is for.
    AutoPtr<IUserHandleHelper> handleHelper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&handleHelper);
    Int32 userId;
    handleHelper->GetUserId(uid, &userId);
    mHost->CancelNotification(pkg, tag, id, 0, 0, FALSE, userId);

    AutoPtr<IBinderHelper> binderHelper;
    CBinderHelper::AcquireSingleton((IBinderHelper**)&binderHelper);

    Int64 ident;
    binderHelper->ClearCallingIdentity(&ident);

    AutoPtr<IIActivityManager> activityMgr;
    mHost->mContext->GetSystemService(IContext::ACTIVITY_SERVICE, (IInterface**)&activityMgr);
    assert(activityMgr != NULL);

    StringBuilder msg;
    msg += "Bad notification posted from package ";
    msg += pkg;
    msg += ": ";
    msg += message;

    activityMgr->CrashApplication(uid, initialPid, pkg, msg.ToString());
    binderHelper->RestoreCallingIdentity(ident);
    return NOERROR;
}

//===============================================================================
//                  CNotificationManagerService::MyBroadcastReceiver
//===============================================================================
CNotificationManagerService::MyBroadcastReceiver::MyBroadcastReceiver(
    /* [in] */ CNotificationManagerService* host)
    : mHost(host)
{
}

ECode CNotificationManagerService::MyBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    Logger::D("NotificationManagerService::MyBroadcastReceiver", "OnReceive %s", action.string());

    Boolean queryRestart = FALSE;
    Boolean packageChanged = FALSE;

    if (action.Equals(IIntent::ACTION_PACKAGE_REMOVED)
            || action.Equals(IIntent::ACTION_PACKAGE_RESTARTED)
            || (packageChanged = action.Equals(IIntent::ACTION_PACKAGE_CHANGED))
            || (queryRestart = action.Equals(IIntent::ACTION_QUERY_PACKAGE_RESTART))
            || action.Equals(IIntent::ACTION_EXTERNAL_APPLICATIONS_UNAVAILABLE))
    {
        AutoPtr< ArrayOf<String> > pkgList;

        if (action.Equals(IIntent::ACTION_EXTERNAL_APPLICATIONS_UNAVAILABLE)) {
            intent->GetStringArrayExtra(IIntent::EXTRA_CHANGED_PACKAGE_LIST, (ArrayOf<String>**)&pkgList);
        }
        else if (queryRestart) {
            intent->GetStringArrayExtra(IIntent::EXTRA_PACKAGES, (ArrayOf<String>**)&pkgList);
        }
        else {
            AutoPtr<IUri> uri;
            intent->GetData((IUri**)&uri);
            if (uri == NULL) {
                return NOERROR;
            }

            String pkgName;
            uri->GetSchemeSpecificPart(&pkgName);
            if (pkgName.IsNull()) {
                return NOERROR;
            }

            if (packageChanged) {
                // We cancel notifications for packages which have just been disabled
                AutoPtr<IPackageManager> pkgMgr;
                mHost->mContext->GetPackageManager((IPackageManager**)&pkgMgr);
                Int32 enabled;
                pkgMgr->GetApplicationEnabledSetting(pkgName, &enabled);

                if (enabled == IPackageManager::COMPONENT_ENABLED_STATE_ENABLED
                    || enabled == IPackageManager::COMPONENT_ENABLED_STATE_DEFAULT) {
                    return NOERROR;
                }
            }

            pkgList = ArrayOf<String>::Alloc(1);
            (*pkgList)[0] = pkgName;
        }

        if (pkgList != NULL && (pkgList->GetLength() > 0)) {
            for (Int32 i = 0; i < pkgList->GetLength(); ++i) {
                String pkgName = (*pkgList)[i];
                mHost->CancelAllNotificationsInt(pkgName, 0, 0, !queryRestart, IUserHandle::USER_ALL);
            }
        }
    }
    else if (action.Equals(IIntent::ACTION_SCREEN_ON)) {
        // Keep track of screen on/off state, but do not turn off the notification light
        // until user passes through the lock screen or views the notification.
        mHost->mScreenOn = FALSE;
    }
    else if (action.Equals(IIntent::ACTION_SCREEN_OFF)) {
        mHost->mScreenOn = FALSE;
    }
    // TODO
    // else if (action.Equals(ITelephonyManager::ACTION_PHONE_STATE_CHANGED)) {
    //     String value;
    //     intent->GetStringExtra(ITelephonyManager::EXTRA_STATE, &value);

    //     mHost->mInCall = value.Equals(ITelephonyManager::EXTRA_STATE_OFFHOOK);
    //     mHost->UpdateNotificationPulse();
    // }
    else if (action.Equals(IIntent::ACTION_USER_STOPPED)) {
        Int32 userHandle;
        intent->GetInt32Extra(IIntent::EXTRA_USER_HANDLE, -1, &userHandle);

        if (userHandle >= 0) {
            mHost->CancelAllNotificationsInt(String(""), 0, 0, TRUE, userHandle);
        }
    }
    else if (action.Equals(IIntent::ACTION_USER_PRESENT)) {
        // turn off LED when user passes through lock screen
        mHost->mNotificationLight->TurnOff();
    }

    return NOERROR;
}

//===============================================================================
//                  CNotificationManagerService::SettingsObserver
//===============================================================================
CNotificationManagerService::SettingsObserver::SettingsObserver(
    /* [in] */ CNotificationManagerService* host,
    /* [in] */ IHandler* handler)
    : ContentObserver(handler)
    , mHost(host)
{}

void CNotificationManagerService::SettingsObserver::Observe()
{
    AutoPtr<IContentResolver> resolver;
    mHost->mContext->GetContentResolver((IContentResolver**)&resolver);

    AutoPtr<ISettingsSystem> systemSettings;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&systemSettings);

    AutoPtr<IUri> uri;
    systemSettings->GetUriFor(ISettingsSystem::NOTIFICATION_LIGHT_PULSE, (IUri**)&uri);
    resolver->RegisterContentObserver(uri, FALSE, this);

    Update();
}

ECode CNotificationManagerService::SettingsObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    Update();
    return NOERROR;
}

void CNotificationManagerService::SettingsObserver::Update()
{
    AutoPtr<IContentResolver> resolver;
    mHost->mContext->GetContentResolver((IContentResolver**)&resolver);

    AutoPtr<ISettingsSystem> systemSettings;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&systemSettings);

    Int32 value;
    systemSettings->GetInt32(resolver, ISettingsSystem::NOTIFICATION_LIGHT_PULSE, 0, &value);

    Boolean pulseEnabled = (value != 0);
    if (mHost->mNotificationPulseEnabled != pulseEnabled) {
        mHost->mNotificationPulseEnabled = pulseEnabled;
        mHost->UpdateNotificationPulse();
    }
}

//===============================================================================
//                  CNotificationManagerService
//===============================================================================

AutoPtr<ArrayOf<Int64> > InitDEFAULT_VIBRATE_PATTERN()
{
    AutoPtr<ArrayOf<Int64> > result = ArrayOf<Int64>::Alloc(4);
    result->Set(0, 0);
    result->Set(1, 250);
    result->Set(2, 250);
    result->Set(3, 250);
    return result;
}

const String CNotificationManagerService::TAG("NotificationService");
const Boolean CNotificationManagerService::DBG = TRUE;

const Int32 CNotificationManagerService::MAX_PACKAGE_NOTIFICATIONS = 50;

const Int32 CNotificationManagerService::MESSAGE_TIMEOUT = 2;

const Int32 CNotificationManagerService::LONG_DELAY = 3500; // 3.5 seconds
const Int32 CNotificationManagerService::SHORT_DELAY = 2000; // 2 seconds

const AutoPtr<ArrayOf<Int64> > CNotificationManagerService::DEFAULT_VIBRATE_PATTERN = InitDEFAULT_VIBRATE_PATTERN();//{0, 250, 250, 250};
const Int32 CNotificationManagerService::VIBRATE_PATTERN_MAXLEN = 8 * 2 + 1; // up to eight bumps

const Int32 CNotificationManagerService::DEFAULT_STREAM_TYPE = IAudioManager::STREAM_NOTIFICATION;
const Boolean CNotificationManagerService::SCORE_ONGOING_HIGHER = FALSE;

const Int32 CNotificationManagerService::JUNK_SCORE = -1000;
const Int32 CNotificationManagerService::NOTIFICATION_PRIORITY_MULTIPLIER = 10;
const Int32 CNotificationManagerService::SCORE_DISPLAY_THRESHOLD = INotification::PRIORITY_MIN * 10 /*CNotificationManagerService::NOTIFICATION_PRIORITY_MULTIPLIER*/;
const Int32 CNotificationManagerService::SCORE_INTERRUPTION_THRESHOLD = INotification::PRIORITY_LOW * 10 /*CNotificationManagerService::NOTIFICATION_PRIORITY_MULTIPLIER*/;

const Boolean CNotificationManagerService::ENABLE_BLOCKED_NOTIFICATIONS = TRUE;
const Boolean CNotificationManagerService::ENABLE_BLOCKED_TOASTS = TRUE;

const Int32 CNotificationManagerService::DB_VERSION = 1;

const String CNotificationManagerService::TAG_BODY("notification-policy");
const String CNotificationManagerService::ATTR_VERSION("version");

const String CNotificationManagerService::TAG_BLOCKED_PKGS("blocked-packages");
const String CNotificationManagerService::TAG_PACKAGE("package");
const String CNotificationManagerService::ATTR_NAME("name");

CNotificationManagerService::WorkerHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what)
    {
        case CNotificationManagerService::MESSAGE_TIMEOUT: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            CNotificationManagerService::ToastRecord* record = (CNotificationManagerService::ToastRecord*)obj.Get();
            mHost->HandleTimeout(record);
            break;
        }
    }
    return NOERROR;
}

CNotificationManagerService::CNotificationManagerService()
    : mDefaultNotificationColor(0)
    , mDefaultNotificationLedOn(0)
    , mDefaultNotificationLedOff(0)
    , mSystemReady(FALSE)
    , mDisabledNotifications(0)
    , mScreenOn(TRUE)
    , mInCall(FALSE)
    , mNotificationPulseEnabled(FALSE)
{
}

ECode CNotificationManagerService::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Handle32 statusBar,
    /* [in] */ Handle32 _lights)
{
    mContext = context;
    mStatusBar = (CStatusBarManagerService*)statusBar;

    mContext->GetSystemService(IContext::VIBRATOR_SERVICE, (IInterface**)&mVibrator);
    mContext->GetSystemService(IContext::ACTIVITY_SERVICE, (IInterface**)&mAm);
    assert(mAm != NULL);

    mIntentReceiver = new MyBroadcastReceiver(this);
    mHandler = new WorkerHandler(this);

    LoadBlockDb();

    LightsService* lights = (LightsService*)_lights;
    mNotificationCallbacks = new MyNotificationCallbacks(this);
    mStatusBar->SetNotificationCallbacks(mNotificationCallbacks);

    mNotificationLight = lights->GetLight(LightsService::_LIGHT_ID_NOTIFICATIONS);
    mAttentionLight = lights->GetLight(LightsService::_LIGHT_ID_ATTENTION);

    AutoPtr<IResources> resources;
    mContext->GetResources((IResources**)&resources);

    resources->GetColor(R::color::config_defaultNotificationColor, &mDefaultNotificationColor);
    resources->GetInteger(R::integer::config_defaultNotificationLedOn, &mDefaultNotificationLedOn);
    resources->GetInteger(R::integer::config_defaultNotificationLedOff, &mDefaultNotificationLedOff);

    mDefaultVibrationPattern = GetLongArray(resources,
        R::array::config_defaultNotificationVibePattern, VIBRATE_PATTERN_MAXLEN, DEFAULT_VIBRATE_PATTERN);
    mFallbackVibrationPattern = GetLongArray(resources,
        R::array::config_notificationFallbackVibePattern, VIBRATE_PATTERN_MAXLEN, DEFAULT_VIBRATE_PATTERN);

    // Don't start allowing notifications until the setup wizard has run once.
    // After that, including subsequent boots, init with notifications turned on.
    // This works on the first boot because the setup wizard will toggle this
    // flag at least once and we'll go back to 0 after that.
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);

    AutoPtr<ISettingsGlobal> globalSettings;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&globalSettings);

    Int32 value;
    globalSettings->GetInt32(resolver, ISettingsGlobal::DEVICE_PROVISIONED, 0, &value);

    if (value == 0) {
        mDisabledNotifications = IStatusBarManager::DISABLE_NOTIFICATION_ALERTS;
    }

    // register for various Intents
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(IIntent::ACTION_SCREEN_ON);
    filter->AddAction(IIntent::ACTION_SCREEN_OFF);
    // filter->AddAction(ITelephonyManager::ACTION_PHONE_STATE_CHANGED);
    filter->AddAction(IIntent::ACTION_USER_PRESENT);
    filter->AddAction(IIntent::ACTION_USER_STOPPED);

    AutoPtr<IIntent> stickyIntent;
    mContext->RegisterReceiver(mIntentReceiver, filter, (IIntent**)&stickyIntent);

    AutoPtr<IIntentFilter> pkgFilter;
    CIntentFilter::New((IIntentFilter**)&pkgFilter);
    pkgFilter->AddAction(IIntent::ACTION_PACKAGE_REMOVED);
    pkgFilter->AddAction(IIntent::ACTION_PACKAGE_CHANGED);
    pkgFilter->AddAction(IIntent::ACTION_PACKAGE_RESTARTED);
    pkgFilter->AddAction(IIntent::ACTION_QUERY_PACKAGE_RESTART);
    pkgFilter->AddDataScheme(String("package"));

    stickyIntent = NULL;
    mContext->RegisterReceiver(mIntentReceiver, pkgFilter, (IIntent**)&stickyIntent);

    AutoPtr<IIntentFilter> sdFilter;
    CIntentFilter::New(IIntent::ACTION_EXTERNAL_APPLICATIONS_UNAVAILABLE, (IIntentFilter**)&sdFilter);

    stickyIntent = NULL;
    mContext->RegisterReceiver(mIntentReceiver, sdFilter, (IIntent**)&stickyIntent);

    AutoPtr<SettingsObserver> observer = new SettingsObserver(this, mHandler);
    observer->Observe();
    return NOERROR;
}

AutoPtr< ArrayOf<Int64> > CNotificationManagerService::GetLongArray(
    /* [in] */ IResources* res,
    /* [in] */ Int32 resid,
    /* [in] */ Int32 maxlen,
    /* [in] */ ArrayOf<Int64>* def)
{
    AutoPtr< ArrayOf<Int32> > ar;
    res->GetInt32Array(resid, (ArrayOf<Int32>**)&ar);

    AutoPtr< ArrayOf<Int64> > result;
    if (ar == NULL) {
        if (def != NULL) result = def->Clone();
        return result;
    }

    Int32 len = ar->GetLength() > maxlen ? maxlen : ar->GetLength();
    result = ArrayOf<Int64>::Alloc(len);
    for (Int32 i = 0; i < len; i++) {
        (*result)[i] = (*ar)[i];
    }

    return result;
}

void CNotificationManagerService::LoadBlockDb()
{
    AutoLock lock(mBlockedPackagesLock);

    if (mPolicyFile != NULL) {
        return;
    }

    AutoPtr<IFile> dir;
    if (FAILED(CFile::New(String("/data/system"), (IFile**)&dir)))
        return;
    AutoPtr<IFile> file;
    if (FAILED(CFile::New(dir, String("notification_policy.xml"), (IFile**)&file)))
        return;

    Boolean fileExits = FALSE;
    file->Exists(&fileExits);
    if (!fileExits) return;

    if (FAILED(CAtomicFile::New(file, (IAtomicFile**)&mPolicyFile)))
        return;

    mBlockedPackages.Clear();

    AutoPtr<IFileInputStream> infile;
    mPolicyFile->OpenRead((IFileInputStream**)&infile);

    String nullStr;
    AutoPtr<IXmlPullParser> parser = Xml::NewPullParser();
    parser->SetInput(infile, nullStr);

    Int32 type;
    String tag;
    Int32 version = DB_VERSION;

    while ((parser->Next(&type), type) != IXmlPullParser::END_DOCUMENT) {
        if (type != IXmlPullParser::START_TAG) {
            continue;
        }

        parser->GetName(&tag);

        if (TAG_BODY.Equals(tag)) {
            String value;
            parser->GetAttributeValue(nullStr, ATTR_VERSION, &value);
            version = StringUtils::ParseInt32(value);
        }
        else if (TAG_BLOCKED_PKGS.Equals(tag)) {
            parser->Next(&type);

            while (type != IXmlPullParser::END_DOCUMENT) {
                parser->GetName(&tag);

                if (TAG_PACKAGE.Equals(tag)) {
                    String value;
                    parser->GetAttributeValue(nullStr, ATTR_NAME, &value);
                    mBlockedPackages.Insert(value);
                }
                else if (TAG_BLOCKED_PKGS.Equals(tag) && type == IXmlPullParser::END_TAG) {
                    break;
                }
            }
        }
    }

    // //Log.wtf(TAG, "Unable to read blocked notifications database", e);

    AutoPtr<IIoUtils> ioUtils;
    CIoUtils::AcquireSingleton((IIoUtils**)&ioUtils);
    ioUtils->CloseQuietly(infile);
}

void CNotificationManagerService::WriteBlockDb()
{
    AutoLock lock(mBlockedPackagesLock);

    ECode ec = NOERROR;
    HashSet<String>::Iterator it;
    AutoPtr<IBoolean> standalone;

    AutoPtr<IFileOutputStream> outfile;
    mPolicyFile->StartWrite((IFileOutputStream**)&outfile);

    String nullStr;
    String emptyStr("");
    AutoPtr<IXmlSerializer> out;
    ec = CFastXmlSerializer::New((IFastXmlSerializer**)&out);
    FAIL_GOTO(ec, _EXIT_)

    ec = out->SetOutput(outfile, String("utf-8"));
    FAIL_GOTO(ec, _EXIT_)

    CBoolean::New(TRUE, (IBoolean**)&standalone);
    ec = out->StartDocument(nullStr, standalone);
    FAIL_GOTO(ec, _EXIT_)

    ec = out->WriteStartTag(nullStr, TAG_BODY);
    FAIL_GOTO(ec, _EXIT_)

    {
        ec = out->WriteAttribute(emptyStr, ATTR_VERSION, StringUtils::Int32ToString(DB_VERSION));
        FAIL_GOTO(ec, _EXIT_)

        ec = out->WriteStartTag(emptyStr, TAG_BLOCKED_PKGS);
        FAIL_GOTO(ec, _EXIT_)

        {
            // write all known network policies
            it = mBlockedPackages.Begin();

            while (it != mBlockedPackages.End()) {
                String pkg = *it;

                ec = out->WriteStartTag(nullStr, TAG_PACKAGE);
                FAIL_GOTO(ec, _EXIT_)

                ec = out->WriteAttribute(nullStr, ATTR_NAME, pkg);
                FAIL_GOTO(ec, _EXIT_)

                ec = out->WriteEndTag(nullStr, TAG_PACKAGE);
                FAIL_GOTO(ec, _EXIT_)

                ++it;
            }
        }

        ec = out->WriteEndTag(nullStr, TAG_BLOCKED_PKGS);
        FAIL_GOTO(ec, _EXIT_)
    }

    ec = out->WriteEndTag(nullStr, TAG_BODY);
    FAIL_GOTO(ec, _EXIT_)

    ec = out->EndDocument();
    FAIL_GOTO(ec, _EXIT_)

    ec = mPolicyFile->FinishWrite(outfile);
    FAIL_GOTO(ec, _EXIT_)

    if (outfile) {
        outfile->Close();
    }
    return;

_EXIT_:
    if (outfile != NULL) {
        mPolicyFile->FailWrite(outfile);
        outfile->Close();
    }
}


// Toasts
// ============================================================================
ECode CNotificationManagerService::EnqueueToast(
    /* [in] */ const String& pkg,
    /* [in] */ IITransientNotification* callback,
    /* [in] */ Int32 duration)
{
    if (DBG) {
        Slogger::I(TAG, "enqueueToast pkg=[%s], callback=%p, duration=%d", pkg.string(), callback, duration);
    }

    if (pkg.IsNull() || callback == NULL) {
        Slogger::E(TAG, "Not doing toast. pkg=[%s], callback=%p", pkg.string(), callback);
        return NOERROR;
    }

    Boolean isSystemToast = pkg.Equals("android");

    if (ENABLE_BLOCKED_TOASTS && !isSystemToast && !AreNotificationsEnabledForPackageInt(pkg)) {
        StringBuilder buf;
        buf += "Suppressing toast from package ";
        buf += pkg;
        buf += " by user request.";
        Logger::E(TAG, buf.ToString());
        return NOERROR;
    }

    {
        AutoLock lock(mToastQueueLock);

        AutoPtr<IBinderHelper> binderHelper;
        CBinderHelper::AcquireSingleton((IBinderHelper**)&binderHelper);

        Int32 callingPid;
        binderHelper->GetCallingPid(&callingPid);

        Int64 callingId;
        binderHelper->ClearCallingIdentity(&callingId);

        AutoPtr<CNotificationManagerService::ToastRecord> record;
        Int32 index = IndexOfToastLocked(pkg, callback);

        // If it's already in the queue, we update it in place, we don't
        // move it to the end of the queue.
        if (index >= 0) {
            record = mToastQueue[index];
            record->Update(duration);
        }
        else {
            // Limit the number of toasts that any given package except the android
            // package can enqueue.  Prevents DOS attacks and deals with leaks.
            if (!isSystemToast) {
                Int32 count = 0;

                List< AutoPtr<ToastRecord> >::Iterator it = mToastQueue.Begin();
                for (; it != mToastQueue.End(); it++) {
                    AutoPtr<CNotificationManagerService::ToastRecord> r = *it;

                    if (r->mPkg.Equals(pkg)) {
                        count++;

                        if (count >= MAX_PACKAGE_NOTIFICATIONS) {
                            StringBuilder buf;
                            buf += "Package has already posted ";
                            buf += count;
                            buf += " toasts. Not showing more. Package=";
                            buf += pkg;
                            Logger::E(TAG, buf.ToString());

                            return NOERROR;
                        }
                    }
                }
            }

            record = new CNotificationManagerService::ToastRecord(callingPid, pkg, callback, duration);
            mToastQueue.PushBack(record);
            index = mToastQueue.GetSize() - 1;
            KeepProcessAliveLocked(callingPid);
        }

        // If it's at index 0, it's the current toast.  It doesn't matter if it's
        // new or just been updated.  Call back and tell it to show itself.
        // If the callback fails, this will remove it from the list, so don't
        // assume that it's valid after this.
        if (index == 0) {
            ShowNextToastLocked();
        }

        binderHelper->RestoreCallingIdentity(callingId);
    }

    return NOERROR;
}

ECode CNotificationManagerService::CancelToast(
    /* [in] */ const String& pkg,
    /* [in] */ IITransientNotification* callback)
{
    Slogger::I(TAG, "cancelToast pkg=%s, callback=%p", pkg.string(), callback);

    if (pkg.IsNull() || callback == NULL) {
        Slogger::E(TAG, "Not cancelling notification. pkg=%s, callback=%p", pkg.string(), callback);
        return NOERROR;
    }

    {
        AutoLock lock(mToastQueueLock);

        AutoPtr<IBinderHelper> binderHelper;
        CBinderHelper::AcquireSingleton((IBinderHelper**)&binderHelper);

        Int64 callingId;
        binderHelper->ClearCallingIdentity(&callingId);

        Int32 index = IndexOfToastLocked(pkg, callback);

        if (index >= 0) {
            CancelToastLocked(index);
        } else {
            Slogger::W(TAG, "Toast already cancelled. pkg=%s, callback=%p", pkg.string(), callback);
        }

        binderHelper->RestoreCallingIdentity(callingId);
    }

    return NOERROR;
}


ECode CNotificationManagerService::SetNotificationsEnabledForPackage(
    /* [in] */ const String& pkg,
    /* [in] */ Boolean enabled)
{
    CheckCallerIsSystem();

    if (DBG) {
        StringBuilder buf;
        buf += (enabled ? "en" : "dis");
        buf += "abling notifications for ";
        buf += pkg;
        Logger::V(TAG, buf.ToString());
    }

    if (enabled) {
        HashSet<String>::Iterator it = mBlockedPackages.Find(pkg);
        if (it != mBlockedPackages.End()) {
            mBlockedPackages.Erase(it);
        }
    }
    else {
        mBlockedPackages.Insert(pkg);

        // Now, cancel any outstanding notifications that are part of a just-disabled app
        if (ENABLE_BLOCKED_NOTIFICATIONS) {
            AutoLock lock(mNotificationListLock);

            NotificationRecordIterator it = mNotificationList.Begin();
            for (; it != mNotificationList.End(); it++) {
                AutoPtr<NotificationRecord> record = *it;

                if (record->mPkg.Equals(pkg)) {
                    CancelNotificationLocked(record, FALSE);
                }
            }
        }
        // Don't bother canceling toasts, they'll go away soon enough.
    }

    WriteBlockDb();
    return NOERROR;
}

String CNotificationManagerService::IdDebugString(
    /* [in] */ IContext* baseContext,
    /* [in] */ const String& packageName,
    /* [in] */ Int32 id)
{
    AutoPtr<IContext> c;

    if (!packageName.IsNull()) {
        ECode ec = baseContext->CreatePackageContext(packageName, 0, (IContext**)&c);

        if (FAILED(ec)) {
            c = baseContext;
        }
    } else {
        c = baseContext;
    }

    String pkg;
    String type;
    String name;

    AutoPtr<IResources> r;
    c->GetResources((IResources**)&r);

    ECode ec = r->GetResourceName(id, &name);

    if (FAILED(ec)) {
        return String("<name unknown>");
    }

    return name;
}

ECode CNotificationManagerService::AreNotificationsEnabledForPackage(
    /* [in] */ const String& pkg,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    CheckCallerIsSystem();
    *result = AreNotificationsEnabledForPackageInt(pkg);
    return NOERROR;
}

ECode CNotificationManagerService::SystemReady()
{
    AutoPtr<IInterface> tmpObj = ServiceManager::GetService(IContext::AUDIO_SERVICE);
    mAudioService = IIAudioService::Probe(tmpObj.Get());

    // no beeping until we're basically done booting
    mSystemReady = TRUE;
    return NOERROR;
}

Boolean CNotificationManagerService::AreNotificationsEnabledForPackageInt(
    /* [in] */ const String& pkg)
{
    HashSet<String>::Iterator it = mBlockedPackages.Find(pkg);

    Boolean enabled;
    enabled = (it != mBlockedPackages.End()) ? FALSE : TRUE;

    if (DBG) {
        StringBuilder buf;
        buf += "notifications are ";
        buf += (enabled ? "en" : "dis");
        buf += "abled for ";
        buf += pkg;
        Logger::V(TAG, buf.ToString());
    }

    return enabled;
}

void CNotificationManagerService::ShowNextToastLocked()
{
    List< AutoPtr<ToastRecord> >::Iterator ator = mToastQueue.Begin();
    AutoPtr<ToastRecord> record;
    if (ator != mToastQueue.End()) {
        record = *ator;
    }

    while (record != NULL) {
        if (DBG) {
            Slogger::D(TAG, "Show pkg=[%s], callback=%p",
                record->mPkg.string(), record->mCallback.Get());
        }

        ECode ec = record->mCallback->Show();
        if (SUCCEEDED(ec)) {
            ScheduleTimeoutLocked(record, FALSE);
            return;
        }
        else {
            Slogger::W(TAG, "Object died trying to show notification %p in package %s",
                record->mCallback.Get(), record->mPkg.string());
            // remove it from the list and let the process die
            mToastQueue.Remove(record);
            KeepProcessAliveLocked(record->mPid);

            if (mToastQueue.IsEmpty() == FALSE) {
                record = mToastQueue[0];
            } else {
                record = NULL;
            }
        }
    }
}

void CNotificationManagerService::CancelToastLocked(
    /* [in] */ Int32 index)
{
    assert(index >= 0 && index < mToastQueue.GetSize());

    AutoPtr<ToastRecord> record = mToastQueue[index];
    ECode ec = record->mCallback->Hide();
    if (FAILED(ec)) {
        /*
        Slog.w(TAG, "Object died trying to hide notification " + record.callback
                + " in package " + record.pkg);
        //*/
        // don't worry about this, we're about to remove it from
        // the list anyway
    }

    mToastQueue.Remove(index);
    KeepProcessAliveLocked(record->mPid);

    if (mToastQueue.IsEmpty() == FALSE) {
        // Show the next one. If the callback fails, this will remove
        // it from the list, so don't assume that the list hasn't changed
        // after this point.
        ShowNextToastLocked();
    }
}

void CNotificationManagerService::ScheduleTimeoutLocked(
    /* [in] */ CNotificationManagerService::ToastRecord* record,
    /* [in] */ Boolean immediate)
{
    assert(record);

    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(MESSAGE_TIMEOUT, (IInterface*)record, (IMessage**)&msg);
    Int64 delay = immediate ? 0 :
        (record->mDuration == IToast::LENGTH_LONG ? LONG_DELAY : SHORT_DELAY);
    mHandler->RemoveCallbacksAndMessages((IInterface*)record);
    Boolean result;
    mHandler->SendMessageDelayed(msg, delay, &result);
}

void CNotificationManagerService::HandleTimeout(
    /* [in] */ ToastRecord* record)
{
    AutoLock lock(mToastQueueLock);

    if (DBG) Slogger::D(TAG, "Timeout pkg=%s, callback=%p",
        record->mPkg.string(), record->mCallback.Get());

    Int32 index = IndexOfToastLocked(record->mPkg, record->mCallback);
    if (index >= 0) {
        CancelToastLocked(index);
    }
}

Int32 CNotificationManagerService::IndexOfToastLocked(
    /* [in] */ const String& pkg,
    /* [in] */ IITransientNotification* callback)
{
    AutoPtr<IBinder> cbak = IBinder::Probe(callback);

    List< AutoPtr<ToastRecord> >::Iterator it = mToastQueue.Begin();
    for (Int32 index = 0; it != mToastQueue.End(); index++, ++it) {
        AutoPtr<CNotificationManagerService::ToastRecord> record = *it;
        AutoPtr<IBinder> tmpCbak = IBinder::Probe(record->mCallback.Get());

        if (record->mPkg.Equals(pkg) && tmpCbak == cbak) {
            return index;
        }
    }

    return -1;
}

void CNotificationManagerService::KeepProcessAliveLocked(
    /* [in] */ Int32 pid)
{
    Int32 toastCount = 0; // toasts from this pid
    List< AutoPtr<ToastRecord> >::Iterator it = mToastQueue.Begin();

    while (it != mToastQueue.End()) {
        AutoPtr<ToastRecord> record = *it;

        if (record->mPid == pid) {
            toastCount++;
        }

        ++it;
    }

    mAm->SetProcessForeground(mForegroundToken, pid, toastCount > 0);
}

// Notifications
// ============================================================================
ECode CNotificationManagerService::EnqueueNotificationWithTag(
    /* [in] */ const String& pkg,
    /* [in] */ const String& tag,
    /* [in] */ Int32 id,
    /* [in] */ INotification* notification,
    /* [out, callee] */ ArrayOf<Int32>** idOut,
    /* [in] */ Int32 userId)
{
    VALIDATE_NOT_NULL(idOut);
    *idOut = NULL;

    AutoPtr<IBinderHelper> binderHelper;
    CBinderHelper::AcquireSingleton((IBinderHelper**)&binderHelper);

    Int32 uid;
    binderHelper->GetCallingUid(&uid);

    Int32 pid;
    binderHelper->GetCallingPid(&pid);

    return EnqueueNotificationInternal(pkg, uid, pid, tag, id, notification, idOut, userId);
}

Int32 CNotificationManagerService::Clamp(
    /* [in] */ Int32 x,
    /* [in] */ Int32 low,
    /* [in] */ Int32 high)
{
    return (x < low) ? low : ((x > high) ? high : x);
}

ECode CNotificationManagerService::EnqueueNotificationInternal(
    /* [in] */ const String& pkg,
    /* [in] */ Int32 callingUid,
    /* [in] */ Int32 callingPid,
    /* [in] */ const String& tag,
    /* [in] */ Int32 id,
    /* [in] */ INotification* notification,
    /* [out, callee] */ ArrayOf<Int32>** idOut,
    /* [in] */ Int32 userId)
{
    assert(idOut != NULL);

    if (DBG) {
        AutoPtr<ICharSequence> text;
        notification->GetTickerText((ICharSequence**)&text);
        String str;
        if(text != NULL){
            text->ToString(&str);
        }

        Logger::V(TAG, "enqueueNotificationInternal: pkg=[%s] id=%d userId=%d notification=[%s]",
            pkg.string(), id, userId, str.string());
    }

    CheckCallerIsSystemOrSameApp(pkg);
    Boolean isSystemNotification = pkg.Equals("android");

    AutoPtr<IActivityManagerHelper> activityMgrHelper;
    CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&activityMgrHelper);

    Int32 tmpUserId;
    activityMgrHelper->HandleIncomingUser(callingPid, callingUid, userId, TRUE, FALSE,
        String("enqueueNotification"), pkg, &tmpUserId);

    AutoPtr<IUserHandle> user;
    CUserHandle::New(userId, (IUserHandle**)&user);

    // Limit the number of notifications that any given package except the android
    // package can enqueue.  Prevents DOS attacks and deals with leaks.
    if (!isSystemNotification) {
        AutoLock lock(mNotificationListLock);

        Int32 count = 0;

        NotificationRecordIterator it;
        for (it = mNotificationList.Begin(); it != mNotificationList.End(); it++) {
            AutoPtr<NotificationRecord> record = *it;

            if (record->mPkg.Equals(pkg) && record->mUserId == userId) {
                count++;

                if (count >= MAX_PACKAGE_NOTIFICATIONS) {
                    Logger::E(TAG, "Package has already posted %d notifications.  Not showing more.  package=%s",
                        count, pkg.string());
                    return NOERROR;
                }
            }
        }
    }

    // This conditional is a dirty hack to limit the logging done on
    //     behalf of the download manager without affecting other apps.
    // if (!pkg.Equals("com.android.providers.downloads") || Log.isLoggable("DownloadManager", Log.VERBOSE)) {
    //     EventLog.writeEvent(EventLogTags.NOTIFICATION_ENQUEUE, pkg, id, tag, userId,
    //             notification.toString());
    // }

    if (pkg.IsNull() || notification == NULL) {
        Logger::E(TAG, "null not allowed: pkg=[%s], id=%d, notification=%p", pkg.string(), id, notification);
        //throw new IllegalArgumentException("null not allowed: pkg=" + pkg + " id=" + id + " notification=" + notification);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 icon;
    notification->GetIcon(&icon);
    if (icon != 0) {
        AutoPtr<IRemoteViews> view;
        notification->GetContentView((IRemoteViews**)&view);

        if (view == NULL) {
            Logger::E(TAG, "contentView required: pkg=[%s], id=%d, notification=[%p]", pkg.string(), id, notification);
            //throw new IllegalArgumentException("contentView required: pkg=" + pkg + " id=" + id + " notification=" + notification);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }

    // === Scoring ===

    // 0. Sanitize inputs
    Int32 priority;
    notification->GetPriority(&priority);
    notification->SetPriority(Clamp(priority, INotification::PRIORITY_MIN, INotification::PRIORITY_MAX));

    // Migrate notification flags to scores
    Int32 flags;
    notification->GetFlags(&flags);

    if (0 != (flags & INotification::FLAG_HIGH_PRIORITY)) {
        Int32 tmpPriority;
        notification->GetPriority(&tmpPriority);

        if (tmpPriority < INotification::PRIORITY_MAX) {
            notification->SetPriority(INotification::PRIORITY_MAX);
        }
    }
    else if (SCORE_ONGOING_HIGHER && 0 != (flags & INotification::FLAG_ONGOING_EVENT)) {
        Int32 tmpPriority;
        notification->GetPriority(&tmpPriority);

        if (tmpPriority < INotification::PRIORITY_HIGH) {
            notification->SetPriority(INotification::PRIORITY_HIGH);
        }
    }

    // 1. initial score: buckets of 10, around the app
    notification->GetPriority(&priority);
    Int32 score = priority * NOTIFICATION_PRIORITY_MULTIPLIER; //[-20..20]

    // 2. Consult external heuristics (TBD)

    // 3. Apply local rules

    // blocked apps
    if (ENABLE_BLOCKED_NOTIFICATIONS && !isSystemNotification && !AreNotificationsEnabledForPackageInt(pkg)) {
        score = JUNK_SCORE;

        Logger::E(TAG, "Suppressing notification from package %s by user request.", pkg.string());
    }

    if (DBG) {
        AutoPtr<ICharSequence> text;
        notification->GetTickerText((ICharSequence**)&text);

        String str;
        if(text != NULL){
            text->ToString(&str);
        }
        Logger::V(TAG, "Assigned score=%d to %s", score, str.string());
    }

    if (score < SCORE_DISPLAY_THRESHOLD) {
        // Notification will be blocked because the score is too low.
        return NOERROR;
    }

    // Should this notification make noise, vibe, or use the LED?
    const Boolean canInterrupt = (score >= SCORE_INTERRUPTION_THRESHOLD);
    {
        AutoLock lock(mNotificationListLock);

        AutoPtr<NotificationRecord> record = new NotificationRecord(
            pkg, tag, id, callingUid, callingPid, userId, score, notification);
        AutoPtr<NotificationRecord> old;

        Int32 index = IndexOfNotificationLocked(pkg, tag, id, userId);

        if (index < 0) {
            mNotificationList.PushBack(record);
        }
        else {
            old = mNotificationList[index];
            mNotificationList.Remove(old);
            mNotificationList.Insert(index, record);

            // Make sure we don't lose the foreground service state.
            if (old != NULL) {
                notification->GetFlags(&flags);

                Int32 oldFlags;
                old->mNotification->GetFlags(&oldFlags);

                notification->SetFlags(flags |= oldFlags & INotification::FLAG_FOREGROUND_SERVICE);
            }
        }

        // Ensure if this is a foreground service that the proper additional
        // flags are set.
        notification->GetFlags(&flags);
        if ((flags & INotification::FLAG_FOREGROUND_SERVICE) != 0) {
            notification->SetFlags(flags |= INotification::FLAG_ONGOING_EVENT | INotification::FLAG_NO_CLEAR);
        }

        AutoPtr<IActivityManagerHelper> activityMgrHelper;
        CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&activityMgrHelper);

        Int32 currentUser;
        activityMgrHelper->GetCurrentUser(&currentUser);

        AutoPtr<IBinderHelper> binderHelper;
        CBinderHelper::AcquireSingleton((IBinderHelper**)&binderHelper);

        Int64 token;
        binderHelper->ClearCallingIdentity(&token);

        notification->GetIcon(&icon);
        if (icon != 0) {
            AutoPtr<IStatusBarNotification> statusBarNotification;
            CStatusBarNotification::New(pkg, id, tag, record->mUid, record->mInitialPid,
                score, notification, user, (IStatusBarNotification**)&statusBarNotification);

            if (DBG) {
                String sbnStr;
                statusBarNotification->ToString(&sbnStr);
                Slogger::I(TAG, "statusBarNotification: [%s]", sbnStr.string());
            }

            Int64 identity;
            binderHelper->ClearCallingIdentity(&identity);

            if (old != NULL && old->mStatusBarKey != NULL) {
                record->mStatusBarKey = old->mStatusBarKey;
                mStatusBar->UpdateNotification(record->mStatusBarKey, statusBarNotification);
            }
            else {
                record->mStatusBarKey = NULL;
                mStatusBar->AddNotification(statusBarNotification, (IBinder**)&(record->mStatusBarKey));

                AutoPtr<INotification> n;
                statusBarNotification->GetNotification((INotification**)&n);

                Int32 flags;
                n->GetFlags(&flags);
                if ((flags & INotification::FLAG_SHOW_LIGHTS) != 0 && canInterrupt) {
                    mAttentionLight->Pulse();
                }
            }

            binderHelper->RestoreCallingIdentity(identity);

            // Send accessibility events only for the current user.
            if (currentUser == userId) {
                AutoPtr<ICharSequence> seq;
                CString::New(pkg, (ICharSequence**)&seq);
                SendAccessibilityEvent(notification, seq);
            }
        }
        else {
            Slogger::E(TAG, "Ignoring notification with icon==0: ");
            if (old != NULL && old->mStatusBarKey != NULL) {
                Int64 identity;
                binderHelper->ClearCallingIdentity(&identity);
                mStatusBar->RemoveNotification(old->mStatusBarKey);
                binderHelper->RestoreCallingIdentity(identity);
            }
        }

        // If we're not supposed to beep, vibrate, etc. then don't.
        notification->GetFlags(&flags);

        if (((mDisabledNotifications & IStatusBarManager::DISABLE_NOTIFICATION_ALERTS) == 0)
            && (!(old != NULL
                && (flags & INotification::FLAG_ONLY_ALERT_ONCE) != 0 ))
            && (record->mUserId == IUserHandle::USER_ALL ||
                (record->mUserId == userId && record->mUserId == currentUser))
            && canInterrupt
            && mSystemReady)
        {
            AutoPtr<IInterface> tmpObj;
            mContext->GetSystemService(IContext::AUDIO_SERVICE, (IInterface**)&tmpObj);
            AutoPtr<IAudioManager> audioManager = IAudioManager::Probe(tmpObj.Get());
            assert(audioManager != NULL);

            // sound
            Int32 defaults;
            notification->GetDefaults(&defaults);
            Boolean useDefaultSound = (defaults & INotification::DEFAULT_SOUND) != 0;

            AutoPtr<IUri> soundUri;
            Boolean hasValidSound = FALSE;

            if (useDefaultSound) {
                AutoPtr<ISettingsSystem> ss;
                CSettingsSystem::AcquireSingleton((ISettingsSystem**)&ss);
                ss->GetDefaultNotificationUri((IUri**)&soundUri);

                // check to see if the default notification sound is silent
                AutoPtr<IContentResolver> resolver;
                mContext->GetContentResolver((IContentResolver**)&resolver);
                String str;
                ss->GetString(resolver, ISettingsSystem::NOTIFICATION_SOUND, &str);
                hasValidSound = !str.IsNull();
            }
            else {
                notification->GetSound((IUri**)&soundUri);
                hasValidSound = (soundUri != NULL);
            }

            if (hasValidSound) {
                Boolean looping = (flags & INotification::FLAG_INSISTENT) != 0;
                Int32 audioStreamType;

                Int32 tmpType;
                notification->GetAudioStreamType(&tmpType);

                if (tmpType >= 0) {
                    audioStreamType = tmpType;
                }
                else {
                    audioStreamType = DEFAULT_STREAM_TYPE;
                }

                mSoundNotification = record;

                // do not play notifications if stream volume is 0
                // (typically because ringer mode is silent) or if speech recognition is active.
                if (audioManager) {
                    Int32 vol;
                    audioManager->GetStreamVolume(audioStreamType, &vol);
                    Boolean isActive;
                    audioManager->IsSpeechRecognitionActive(&isActive);

                    if ((vol != 0) && !isActive) {
                        Int64 identity;
                        binderHelper->ClearCallingIdentity(&identity);

                        AutoPtr<IIRingtonePlayer> player;
                        mAudioService->GetRingtonePlayer((IIRingtonePlayer**)&player);

                        if (player != NULL) {
                            player->PlayAsync(soundUri, user, looping, audioStreamType);
                        }

                        binderHelper->RestoreCallingIdentity(identity);
                    }
                }
            }

            // vibrate
            // Does the notification want to specify its own vibration?
            AutoPtr< ArrayOf<Int64> > vibrate;
            notification->GetVibrate((ArrayOf<Int64>**)&vibrate);
            Boolean hasCustomVibrate = vibrate != NULL;

            if (audioManager) {
                // new in 4.2: if there was supposed to be a sound and we're in vibrate mode,
                // and no other vibration is specified, we fall back to vibration
                Int32 mode;
                audioManager->GetRingerMode(&mode);

                Boolean convertSoundToVibration =
                    !hasCustomVibrate && hasValidSound
                    && (mode == IAudioManager::RINGER_MODE_VIBRATE);

                // The DEFAULT_VIBRATE flag trumps any custom vibration AND the fallback.
                notification->GetDefaults(&defaults);
                Boolean useDefaultVibrate =
                        (defaults & INotification::DEFAULT_VIBRATE) != 0;

                if ((useDefaultVibrate || convertSoundToVibration || hasCustomVibrate)
                        && !(mode == IAudioManager::RINGER_MODE_SILENT))
                {
                    mVibrateNotification = record;

                    if (useDefaultVibrate || convertSoundToVibration) {
                        // Escalate privileges so we can use the vibrator even if the notifying app
                        // does not have the VIBRATE permission.
                        Int64 identity;
                        binderHelper->ClearCallingIdentity(&identity);

                        mVibrator->Vibrate(useDefaultVibrate
                            ? *mDefaultVibrationPattern : *mFallbackVibrationPattern,
                            ((flags & INotification::FLAG_INSISTENT) != 0) ? 0: -1);

                        binderHelper->RestoreCallingIdentity(identity);
                    }
                    else if (vibrate->GetLength() > 1) {
                        // If you want your own vibration pattern, you need the VIBRATE permission
                        mVibrator->Vibrate(*vibrate, ((flags & INotification::FLAG_INSISTENT) != 0) ? 0: -1);
                    }
                }
            }
        }

        // this option doesn't shut off the lights

        // light
        // the most recent thing gets the light
        mLights.Remove(old);
        if (mLedNotification == old) {
            mLedNotification = NULL;
        }

        notification->GetFlags(&flags);

        if ((flags & INotification::FLAG_SHOW_LIGHTS) != 0 && canInterrupt) {
            mLights.PushBack(record);
            UpdateLightsLocked();
        }
        else {
            if (old != NULL) {
                Int32 oldFlags;
                old->mNotification->GetFlags(&oldFlags);
                Slogger::I(TAG, "notification.lights=%d", ((oldFlags & INotification::FLAG_SHOW_LIGHTS) != 0));

                if (((oldFlags & INotification::FLAG_SHOW_LIGHTS) != 0)) {
                    UpdateLightsLocked();
                }
            }

        }
    }

    *idOut = ArrayOf<Int32>::Alloc(1);
    REFCOUNT_ADD(*idOut);
    (**idOut)[0] = id;
    return NOERROR;
}

void CNotificationManagerService::SendAccessibilityEvent(
    /* [in] */ INotification* notification,
    /* [in] */ ICharSequence* packageName)
{
    AutoPtr<IAccessibilityManager> manager;
    AutoPtr<IAccessibilityManagerHelper> amHelper;
    CAccessibilityManagerHelper::AcquireSingleton((IAccessibilityManagerHelper**)&amHelper);
    amHelper->GetInstance(mContext, (IAccessibilityManager**)&manager);

    Boolean isEnabled;
    manager->IsEnabled(&isEnabled);
    if (isEnabled == FALSE) {
        return;
    }

    AutoPtr<IAccessibilityEventHelper> eventHelper;
    CAccessibilityEventHelper::AcquireSingleton((IAccessibilityEventHelper**)&eventHelper);

    AutoPtr<IAccessibilityEvent> event;
    eventHelper->Obtain(IAccessibilityEvent::TYPE_NOTIFICATION_STATE_CHANGED, (IAccessibilityEvent**)&event);

    AutoPtr<ICharSequence> clsSeq;
    CString::New(String("CNotification"), (ICharSequence**)&clsSeq);
    event->SetPackageName(packageName);
    event->SetClassName(clsSeq);
    event->SetParcelableData(IParcelable::Probe(notification));

    AutoPtr<ICharSequence> tickerText;
    notification->GetTickerText((ICharSequence**)&tickerText);

    if (!TextUtils::IsEmpty(tickerText)) {
        AutoPtr<IObjectContainer> container;
        event->GetText((IObjectContainer**)&container);
        container->Add(tickerText);
    }

    manager->SendAccessibilityEvent(event);
}

void CNotificationManagerService::CancelNotificationLocked(
    /* [in] */ CNotificationManagerService::NotificationRecord* record,
    /* [in] */ Boolean sendDelete)
{
    Slogger::D(TAG, "CNotificationManagerService::CancelNotificationLocked");
    // tell the app
    if (sendDelete) {
        AutoPtr<IPendingIntent> intent;
        record->mNotification->GetDeleteIntent((IPendingIntent**)&intent);

        if (intent != NULL) {
            ECode ec = intent->Send();

            if (FAILED(ec)) {
                // do nothing - there's no relevant way to recover, and
                //     no reason to let this propagate
                StringBuilder buf;
                buf += "canceled PendingIntent for ";
                buf += record->mPkg;
                Logger::W(TAG, buf.ToString());
            }
        }
    }

    // status bar
    Int32 icon;
    record->mNotification->GetIcon(&icon);

    if (icon != 0) {
        AutoPtr<IBinderHelper> binderHelper;
        CBinderHelper::AcquireSingleton((IBinderHelper**)&binderHelper);

        Int64 identity;
        binderHelper->ClearCallingIdentity(&identity);
        mStatusBar->RemoveNotification(record->mStatusBarKey);
        binderHelper->RestoreCallingIdentity(identity);
        record->mStatusBarKey = NULL;
    }

    // sound
    if (mSoundNotification.Get() == record) {
        mSoundNotification = NULL;

        AutoPtr<IBinderHelper> binderHelper;
        CBinderHelper::AcquireSingleton((IBinderHelper**)&binderHelper);

        Int64 identity;
        binderHelper->ClearCallingIdentity(&identity);
        AutoPtr<IIRingtonePlayer> player;
        mAudioService->GetRingtonePlayer((IIRingtonePlayer**)&player);
        if (player != NULL) {
            player->StopAsync();
        }
        binderHelper->RestoreCallingIdentity(identity);
    }

    // vibrate
    if (mVibrateNotification.Get() == record) {
        mVibrateNotification = NULL;

        AutoPtr<IBinderHelper> binderHelper;
        CBinderHelper::AcquireSingleton((IBinderHelper**)&binderHelper);
        Int64 identity;
        binderHelper->ClearCallingIdentity(&identity);
        mVibrator->Cancel();
        binderHelper->RestoreCallingIdentity(identity);
    }

    // light
    mLights.Remove(record);
    if (mLedNotification.Get() == record) {
        mLedNotification = NULL;
    }
}

void CNotificationManagerService::CancelNotification(
    /* [in] */ const String& pkg,
    /* [in] */ const String& tag,
    /* [in] */ Int32 id,
    /* [in] */ Int32 mustHaveFlags,
    /* [in] */ Int32 mustNotHaveFlags,
    /* [in] */ Boolean sendDelete,
    /* [in] */ Int32 userId)
{
    Slogger::D(TAG, "CNotificationManagerService::CancelNotification: id:%d, pkg:%s, tag:%s, userId:%d, mustHaveFlags:%08x, mustNotHaveFlags:%08x",
        id, pkg.string(), tag.string(), userId, mustHaveFlags, mustNotHaveFlags);
    //EventLog.writeEvent(EventLogTags.NOTIFICATION_CANCEL, pkg, id, tag, userId, mustHaveFlags, mustNotHaveFlags);

    AutoLock lock(mNotificationListLock);

    Int32 index = IndexOfNotificationLocked(pkg, tag, id, userId);
    if (index < 0) {
        return;
    }

    AutoPtr<NotificationRecord> record = mNotificationList[index];

    Int32 flags;
    record->mNotification->GetFlags(&flags);
    if ((flags & mustHaveFlags) != mustHaveFlags) {
        return;
    }
    if ((flags & mustNotHaveFlags) != 0) {
        return;
    }

    mNotificationList.Remove(record);

    CancelNotificationLocked(record, sendDelete);
    UpdateLightsLocked();
}

Boolean CNotificationManagerService::NotificationMatchesUserId(
    /* [in] */ CNotificationManagerService::NotificationRecord* record,
    /* [in] */ Int32 userId)
{
    return
      // looking for USER_ALL notifications? match everything
      userId == IUserHandle::USER_ALL
      // a notification sent to USER_ALL matches any query
      || record->mUserId == IUserHandle::USER_ALL
      // an exact user match
      || record->mUserId == userId;
}

Boolean CNotificationManagerService::CancelAllNotificationsInt(
    /* [in] */ const String& pkg,
    /* [in] */ Int32 mustHaveFlags,
    /* [in] */ Int32 mustNotHaveFlags,
    /* [in] */ Boolean doit,
    /* [in] */ Int32 userId)
{
    //EventLog.writeEvent(EventLogTags.NOTIFICATION_CANCEL_ALL, pkg, userId, mustHaveFlags, mustNotHaveFlags);

    AutoLock lock(mNotificationListLock);

    Boolean canceledSomething = FALSE;
    NotificationRecordList::ReverseIterator it = mNotificationList.RBegin();;

    for (; it != mNotificationList.REnd();) {
        AutoPtr<CNotificationManagerService::NotificationRecord> record = *it;

        if (!NotificationMatchesUserId(record, userId)) {
            ++it;
            continue;
        }

        // Don't remove notifications to all, if there's no package name specified
        if (record->mUserId == IUserHandle::USER_ALL && pkg.IsNull()) {
            ++it;
            continue;
        }

        Int32 flags;
        record->mNotification->GetFlags(&flags);

        if ((flags & mustHaveFlags) != mustHaveFlags) {
            ++it;
            continue;
        }

        if ((flags & mustNotHaveFlags) != 0) {
            ++it;
            continue;
        }

        if (!pkg.IsNull() && !record->mPkg.Equals(pkg)) {
            ++it;
            continue;
        }

        canceledSomething = TRUE;

        if (!doit) {
            return TRUE;
        }

        CancelNotificationLocked(record, FALSE);
        it = NotificationRecordList::ReverseIterator(mNotificationList.Erase(--(it.GetBase())));
    }

    if (canceledSomething) {
        UpdateLightsLocked();
    }

    return canceledSomething;
}

ECode CNotificationManagerService::CancelNotificationWithTag(
    /* [in] */ const String& pkg,
    /* [in] */ const String& tag,
    /* [in] */ Int32 id,
    /* [in] */ Int32 userId)
{
    CheckCallerIsSystemOrSameApp(pkg);

    AutoPtr<IBinderHelper> binderHelper;
    CBinderHelper::AcquireSingleton((IBinderHelper**)&binderHelper);

    Int32 uid;
    binderHelper->GetCallingUid(&uid);

    Int32 pid;
    binderHelper->GetCallingPid(&pid);

    AutoPtr<IActivityManagerHelper> activityMgrHelper;
    CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&activityMgrHelper);

    Int32 tmpUserId;
    activityMgrHelper->HandleIncomingUser(pid, uid, userId, TRUE, FALSE,
        String("cancelNotificationWithTag"), pkg, &tmpUserId);

    // Don't allow client applications to cancel foreground service notis.
    CancelNotification(pkg, tag, id, 0, uid == IProcess::SYSTEM_UID ?
        0 : INotification::FLAG_FOREGROUND_SERVICE, FALSE, userId);
    return NOERROR;
}

ECode CNotificationManagerService::CancelAllNotifications(
    /* [in] */ const String& pkg,
    /* [in] */ Int32 userId)
{
    CheckCallerIsSystemOrSameApp(pkg);

    AutoPtr<IBinderHelper> binderHelper;
    CBinderHelper::AcquireSingleton((IBinderHelper**)&binderHelper);

    Int32 uid;
    binderHelper->GetCallingUid(&uid);

    Int32 pid;
    binderHelper->GetCallingPid(&pid);

    AutoPtr<IActivityManagerHelper> activityMgrHelper;
    CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&activityMgrHelper);

    Int32 tmpUserId;
    activityMgrHelper->HandleIncomingUser(pid, uid, userId, TRUE, FALSE,
        String("cancelAllNotifications"), pkg, &tmpUserId);

    // Calling from user space, don't allow the canceling of actively
    // running foreground services.
    CancelAllNotificationsInt(pkg, 0, INotification::FLAG_FOREGROUND_SERVICE, TRUE, tmpUserId);
    return NOERROR;
}

ECode CNotificationManagerService::CheckCallerIsSystem()
{
    AutoPtr<IBinderHelper> binderHelper;
    CBinderHelper::AcquireSingleton((IBinderHelper**)&binderHelper);

    Int32 uid;
    binderHelper->GetCallingUid(&uid);

    AutoPtr<IUserHandleHelper> handleHelper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&handleHelper);

    Int32 appId;
    handleHelper->GetAppId(uid, &appId);

    if (appId == IProcess::SYSTEM_UID || uid == 0) {
        return NOERROR;
    }

    //throw new SecurityException("Disallowed call for uid " + uid);
    StringBuilder buf;
    buf += "Disallowed call for uid ";
    buf += uid;
    Logger::E(TAG, buf.ToString());

    return E_SECURITY_EXCEPTION;
}

ECode CNotificationManagerService::CheckCallerIsSystemOrSameApp(
    /* [in] */ const String& pkg)
{
    AutoPtr<IBinderHelper> binderHelper;
    CBinderHelper::AcquireSingleton((IBinderHelper**)&binderHelper);

    Int32 uid;
    binderHelper->GetCallingUid(&uid);

    AutoPtr<IUserHandleHelper> handleHelper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&handleHelper);

    Int32 appId;
    handleHelper->GetAppId(uid, &appId);

    if (appId == IProcess::SYSTEM_UID || uid == 0) {
        return NOERROR;
    }

    AutoPtr<IIPackageManager> pm = AppGlobals::GetPackageManager();

    Int32 callingUserId;
    handleHelper->GetCallingUserId(&callingUserId);

    AutoPtr<IApplicationInfo> appInfo;
    pm->GetApplicationInfo(pkg, 0, callingUserId, (IApplicationInfo**)&appInfo);

    Int32 appUid;
    appInfo->GetUid(&appUid);

    Boolean result;
    ECode ec = handleHelper->IsSameApp(appUid, uid, &result);

    if (FAILED(ec)) {
        //throw new SecurityException("Unknown package " + pkg + "\n" + re);
        return E_SECURITY_EXCEPTION;
    }

    if (result == FALSE) {
        //throw new SecurityException("Calling uid " + uid + " gave package" + pkg + " which is owned by uid " + ai.uid);
        StringBuilder buf;
        buf += "Calling uid ";
        buf += uid;
        buf += " gave package";
        buf += pkg;
        buf += " which is owned by uid ";
        buf += appUid;
        Logger::E(TAG, buf.ToString());

        return E_SECURITY_EXCEPTION;
    }

    return NOERROR;
}

void CNotificationManagerService::CancelAll(
    /* [in] */ Int32 userId)
{
    AutoLock lock(mNotificationListLock);
    NotificationRecordList::ReverseIterator it = mNotificationList.RBegin();

    for (; it != mNotificationList.REnd();) {
        AutoPtr<CNotificationManagerService::NotificationRecord> record = *it;

        if (!NotificationMatchesUserId(record, userId)) {
            ++it;
            continue;
        }

        Int32 flags;
        record->mNotification->GetFlags(&flags);

        if ((flags & (INotification::FLAG_ONGOING_EVENT | INotification::FLAG_NO_CLEAR)) == 0) {
            CancelNotificationLocked(record, TRUE);
            it = NotificationRecordList::ReverseIterator(mNotificationList.Erase(--(it.GetBase())));
        }
        else {
            ++it;
        }
    }

    UpdateLightsLocked();
}

void CNotificationManagerService::UpdateLightsLocked()
{
    if (mLights.IsEmpty())
        return;

    // handle notification lights
    if (mLedNotification == NULL) {
        // get next notification, if any
        if (mLights.Begin() != mLights.End()) {
            mLedNotification = *(mLights.RBegin());
        }
    }

    // Don't flash while we are in a call or screen is on
    if (mLedNotification == NULL || mScreenOn || mInCall) {
        mNotificationLight->TurnOff();
    }
    else {
        Int32 ledARGB, ledOnMS, ledOffMS, defaults;

        mLedNotification->mNotification->GetLedARGB(&ledARGB);
        mLedNotification->mNotification->GetLedOnMS(&ledOnMS);
        mLedNotification->mNotification->GetLedOffMS(&ledOffMS);
        mLedNotification->mNotification->GetDefaults(&defaults);

        if ((defaults & INotification::DEFAULT_LIGHTS) != 0) {
            ledARGB = mDefaultNotificationColor;
            ledOnMS = mDefaultNotificationLedOn;
            ledOffMS = mDefaultNotificationLedOff;
        }

        if (mNotificationPulseEnabled) {
            // pulse repeatedly
            mNotificationLight->SetFlashing(ledARGB, LightsService::_LIGHT_FLASH_TIMED, ledOnMS, ledOffMS);
        }
    }
}

Int32 CNotificationManagerService::IndexOfNotificationLocked(
    /* [in] */ const String& pkg,
    /* [in] */ const String& tag,
    /* [in] */ Int32 id,
    /* [in] */ Int32 userId)
{
    NotificationRecordIterator it = mNotificationList.Begin();

    Int32 index = 0;
    for (; it != mNotificationList.End(); index++, it++) {
        AutoPtr<NotificationRecord> record = *it;

        if (!NotificationMatchesUserId(record, userId) || record->mId != id) {
            continue;
        }

        if (tag.IsNull()) {
            if (!record->mTag.IsNull()) {
                continue;
            }
        } else {
            if (!tag.Equals(record->mTag)) {
                continue;
            }
        }

        if (pkg.Equals(record->mPkg)) {
            return index;
        }
    }

    return -1;
}

void CNotificationManagerService::UpdateNotificationPulse()
{
    AutoLock lock(mNotificationListLock);
    UpdateLightsLocked();
}

ECode CNotificationManagerService::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = String("NotificationManagerService");
    return NOERROR;
}

} // Server
} // Droid
} // Elastos
