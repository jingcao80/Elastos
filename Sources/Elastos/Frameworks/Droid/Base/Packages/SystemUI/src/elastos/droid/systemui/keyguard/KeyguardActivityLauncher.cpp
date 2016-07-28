
#include "elastos/droid/systemui/keyguard/KeyguardActivityLauncher.h"
#include "elastos/droid/systemui/keyguard/AppearAnimationUtils.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.AppWidget.h"
#include "Elastos.Droid.Provider.h"
#include <elastos/droid/app/ActivityManagerNative.h>
#include <elastos/droid/os/SystemClock.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::IIActivityManager;
using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::App::IActivityOptions;
using Elastos::Droid::App::IActivityOptionsHelper;
using Elastos::Droid::App::CActivityOptionsHelper;
using Elastos::Droid::App::IActivityManagerWaitResult;
using Elastos::Droid::AppWidget::IAppWidgetManager;
using Elastos::Droid::AppWidget::IAppWidgetProviderInfo;
using Elastos::Droid::Provider::IMediaStore;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::View::IIWindowManager;
using Elastos::Droid::View::IWindowManagerGlobalHelper;
using Elastos::Droid::View::CWindowManagerGlobalHelper;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

static const String TAG("KeyguardActivityLauncher");

//===================================================================
// KeyguardActivityLauncher::MyOnDismissAction
//===================================================================
CAR_INTERFACE_IMPL(KeyguardActivityLauncher::MyOnDismissAction, Object, IKeyguardHostViewOnDismissAction)

KeyguardActivityLauncher::MyOnDismissAction::MyOnDismissAction(
    /* [in] */ KeyguardActivityLauncher* host,
    /* [in] */ IIntent* intent,
    /* [in] */ IBundle* animation,
    /* [in] */ IHandler* worker,
    /* [in] */ IRunnable* onStarted)
    : mHost(host)
    , mIntent(intent)
    , mAnimation(animation)
    , mWorker(worker)
    , mOnStarted(onStarted)
{}

ECode KeyguardActivityLauncher::MyOnDismissAction::OnDismiss(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    mHost->DismissKeyguardOnNextActivity();
    mHost->StartActivityForCurrentUser(mIntent, mAnimation, mWorker, mOnStarted);
    *result = TRUE;
    return NOERROR;
}

//===================================================================
// KeyguardActivityLauncher::MyRunnable
//===================================================================
KeyguardActivityLauncher::MyRunnable::MyRunnable(
    /* [in] */ KeyguardActivityLauncher* host,
    /* [in] */ IIntent* intent,
    /* [in] */ IBundle* options,
    /* [in] */ IRunnable* onStarted,
    /* [in] */ IUserHandle* user)
    : mHost(host)
    , mIntent(intent)
    , mOptions(options)
    , mOnStarted(onStarted)
    , mUser(user)
{}

ECode KeyguardActivityLauncher::MyRunnable::Run()
{
    //try {
    ECode ec = NOERROR;
    AutoPtr<IIActivityManager> manager = ActivityManagerNative::GetDefault();

    String type, nullStr;
    Int32 id;

    AutoPtr<IActivityManagerWaitResult> result;
    AutoPtr<IContext> context;
    mHost->GetContext((IContext**)&context);
    AutoPtr<IContentResolver> cr;
    FAIL_GOTO(ec = context->GetContentResolver((IContentResolver**)&cr), ERROR)
    FAIL_GOTO(ec = mIntent->ResolveTypeIfNeeded(cr, &type), ERROR)
    FAIL_GOTO(ec = mUser->GetIdentifier(&id), ERROR)

    ec = manager->StartActivityAndWait(
            NULL /*caller*/,
            nullStr /*caller pkg*/,
            mIntent,
            type,
            NULL /*resultTo*/,
            nullStr /*resultWho*/,
            0 /*requestCode*/,
            IIntent::FLAG_ACTIVITY_NEW_TASK,
            NULL /*profilerInfo*/,
            mOptions,
            id,
            (IActivityManagerWaitResult**)&result);
    if (DEBUG)  {
        Logger::D(TAG, "waitResult[%s] at %lld", TO_CSTR(result), SystemClock::GetUptimeMillis());
    }
    //} catch (RemoteException e) {

ERROR:
    if (FAILED(ec)) {
        Logger::W(TAG, "Error starting activity %d", ec);
        return NOERROR;
    }
    // try {
    mOnStarted->Run();
    //} catch (Throwable t) {
        // Log.w(TAG, "Error running onStarted callback", t);
    //}
    return NOERROR;
}

//===================================================================
// KeyguardActivityLauncher::CameraWidgetInfo
//===================================================================
CAR_INTERFACE_IMPL(KeyguardActivityLauncher::CameraWidgetInfo, Object, IKeyguardActivityLauncherCameraWidgetInfo)

KeyguardActivityLauncher::CameraWidgetInfo::CameraWidgetInfo()
    : mLayoutId(0)
{
}

ECode KeyguardActivityLauncher::CameraWidgetInfo::SetContextPackage(
    /* [in] */ const String& str)
{
    mContextPackage = str;
    return NOERROR;
}

ECode KeyguardActivityLauncher::CameraWidgetInfo::GetContextPackage(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = mContextPackage;
    return NOERROR;
}

ECode KeyguardActivityLauncher::CameraWidgetInfo::SetLayoutId(
    /* [in] */ Int32 id)
{
    mLayoutId = id;
    return NOERROR;
}

ECode KeyguardActivityLauncher::CameraWidgetInfo::GetLayoutId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id)
    *id = mLayoutId;
    return NOERROR;
}

//===================================================================
// KeyguardActivityLauncher
//===================================================================
const Boolean KeyguardActivityLauncher::DEBUG = FALSE;

const String KeyguardActivityLauncher::META_DATA_KEYGUARD_LAYOUT("com.android.keyguard.layout");

static AutoPtr<IIntent> InitSECURE_CAMERA_INTENT()
{
    AutoPtr<IIntent> intent;
    CIntent::New(IMediaStore::INTENT_ACTION_STILL_IMAGE_CAMERA_SECURE, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS);
    return intent;
}

AutoPtr<IIntent> KeyguardActivityLauncher::SECURE_CAMERA_INTENT = InitSECURE_CAMERA_INTENT();

static AutoPtr<IIntent> InitINSECURE_CAMERA_INTENT()
{
    AutoPtr<IIntent> intent;
    CIntent::New(IMediaStore::INTENT_ACTION_STILL_IMAGE_CAMERA, (IIntent**)&intent);
    return intent;
}

AutoPtr<IIntent> KeyguardActivityLauncher::INSECURE_CAMERA_INTENT = InitINSECURE_CAMERA_INTENT();

CAR_INTERFACE_IMPL(KeyguardActivityLauncher, Object, IKeyguardActivityLauncher)

ECode KeyguardActivityLauncher::GetCameraWidgetInfo(
    /* [out] */ IKeyguardActivityLauncherCameraWidgetInfo** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<IKeyguardActivityLauncherCameraWidgetInfo> info = new CameraWidgetInfo();
    AutoPtr<IIntent> intent = GetCameraIntent();

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IPackageManager> packageManager;
    context->GetPackageManager((IPackageManager**)&packageManager);

    AutoPtr<ILockPatternUtils> utils;
    GetLockPatternUtils((ILockPatternUtils**)&utils);
    Int32 user;
    utils->GetCurrentUser(&user);

    AutoPtr<IList> appList;
    packageManager->QueryIntentActivitiesAsUser(
            intent, IPackageManager::MATCH_DEFAULT_ONLY, user, (IList**)&appList);
    Int32 size;
    appList->GetSize(&size);
    if (size == 0) {
        if (DEBUG) Logger::D(TAG, "getCameraWidgetInfo(): Nothing found");
        return NOERROR;
    }
    AutoPtr<IResolveInfo> resolved;
    packageManager->ResolveActivityAsUser(intent,
            IPackageManager::MATCH_DEFAULT_ONLY | IPackageManager::GET_META_DATA,
            user, (IResolveInfo**)&resolved);
    if (DEBUG) Logger::D(TAG, "getCameraWidgetInfo(): resolved: %s", TO_CSTR(resolved));
    if (WouldLaunchResolverActivity(resolved, appList)) {
        if (DEBUG) Logger::D(TAG, "getCameraWidgetInfo(): Would launch resolver");
        *result = info;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    if (resolved == NULL) {
        return NOERROR;
    }

    AutoPtr<IActivityInfo> activityInfo;
    resolved->GetActivityInfo((IActivityInfo**)&activityInfo);
    if (activityInfo == NULL) {
        return NOERROR;
    }

    AutoPtr<IBundle> metaData;
    IPackageItemInfo::Probe(activityInfo)->GetMetaData((IBundle**)&metaData);
    Boolean res;
    if (metaData == NULL || (metaData->IsEmpty(&res), res)) {
        if (DEBUG) Logger::D(TAG, "getCameraWidgetInfo(): no metadata found");
        *result = info;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    Int32 layoutId;
    metaData->GetInt32(META_DATA_KEYGUARD_LAYOUT, &layoutId);
    if (layoutId == 0) {
        if (DEBUG) Logger::D(TAG, "getCameraWidgetInfo(): no layout specified");
        *result = info;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }

    String name;
    IPackageItemInfo::Probe(activityInfo)->GetPackageName(&name);
    info->SetContextPackage(name);
    info->SetLayoutId(layoutId);
    *result = info;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode KeyguardActivityLauncher::LaunchCamera(
    /* [in] */ IHandler* worker,
    /* [in] */ IRunnable* onSecureCameraStarted)
{
    AutoPtr<ILockPatternUtils> lockPatternUtils;
    GetLockPatternUtils((ILockPatternUtils**)&lockPatternUtils);
    // Workaround to avoid camera release/acquisition race when resuming face unlock
    // after showing lockscreen camera (bug 11063890).
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IKeyguardUpdateMonitor> updateMonitor;
    Logger::E(TAG, " >> TODO KeyguardUpdateMonitor");
    // = KeyguardUpdateMonitor::GetInstance(context);
    updateMonitor->SetAlternateUnlockEnabled(FALSE);

    if (MustLaunchSecurely()) {
        // Launch the secure version of the camera
        if (WouldLaunchResolverActivity(SECURE_CAMERA_INTENT)) {
            // TODO: Show disambiguation dialog instead.
            // For now, we'll treat this like launching any other app from secure keyguard.
            // When they do, user sees the system's ResolverActivity which lets them choose
            // which secure camera to use.
            LaunchActivity(SECURE_CAMERA_INTENT, FALSE, FALSE, NULL, NULL);
        }
        else {
            LaunchActivity(SECURE_CAMERA_INTENT, TRUE, FALSE, worker, onSecureCameraStarted);
        }
    }
    else {
        // Launch the normal camera
        LaunchActivity(INSECURE_CAMERA_INTENT, FALSE, FALSE, NULL, NULL);
    }
    return NOERROR;
}

Boolean KeyguardActivityLauncher::MustLaunchSecurely()
{
    AutoPtr<ILockPatternUtils> lockPatternUtils;
    GetLockPatternUtils((ILockPatternUtils**)&lockPatternUtils);
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IKeyguardUpdateMonitor> updateMonitor;
    Logger::E(TAG, " >> TODO KeyguardUpdateMonitor");
    // = KeyguardUpdateMonitor::GetInstance(context);
    Int32 currentUser;
    lockPatternUtils->GetCurrentUser(&currentUser);
    Boolean bval1, bval2;
    lockPatternUtils->IsSecure(&bval1);
    return bval1 && (updateMonitor->GetUserHasTrust(currentUser, &bval2), !bval2);
}

ECode KeyguardActivityLauncher::LaunchWidgetPicker(
    /* [in] */ Int32 appWidgetId)
{
    AutoPtr<IIntent> pickIntent;
    assert(0 && "TODO");
    // CIntent::New(IAppWidgetManager::ACTION_KEYGUARD_APPWIDGET_PICK, (IIntent**)&pickIntent);

    pickIntent->PutExtra(IAppWidgetManager::EXTRA_APPWIDGET_ID, appWidgetId);
    pickIntent->PutExtra(IAppWidgetManager::EXTRA_CUSTOM_SORT, FALSE);
    pickIntent->PutExtra(IAppWidgetManager::EXTRA_CATEGORY_FILTER,
            IAppWidgetProviderInfo::WIDGET_CATEGORY_KEYGUARD);

    AutoPtr<IBundle> options;
    CBundle::New((IBundle**)&options);
    options->PutInt32(IAppWidgetManager::OPTION_APPWIDGET_HOST_CATEGORY,
            IAppWidgetProviderInfo::WIDGET_CATEGORY_KEYGUARD);
    pickIntent->PutExtra(IAppWidgetManager::EXTRA_APPWIDGET_OPTIONS, options);
    pickIntent->AddFlags(
            IIntent::FLAG_ACTIVITY_NEW_TASK
            | IIntent::FLAG_ACTIVITY_SINGLE_TOP
            | IIntent::FLAG_ACTIVITY_CLEAR_TOP
            | IIntent::FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS);

    return LaunchActivity(pickIntent, FALSE, FALSE, NULL, NULL);
}

ECode KeyguardActivityLauncher::LaunchActivity(
    /* [in] */ IIntent* intent,
    /* [in] */ Boolean showsWhileLocked,
    /* [in] */ Boolean useDefaultAnimations,
    /* [in] */ IHandler* worker,
    /* [in] */ IRunnable* onStarted)
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);

    AutoPtr<IBundle> animation;
    if (useDefaultAnimations) {
        animation = NULL;
    }
    else {
        AutoPtr<IActivityOptionsHelper> helper;
        CActivityOptionsHelper::AcquireSingleton((IActivityOptionsHelper**)&helper);
        AutoPtr<IActivityOptions> options;
        helper->MakeCustomAnimation(context, 0, 0, (IActivityOptions**)&options);
        options->ToBundle((IBundle**)&animation);
    }
    return LaunchActivityWithAnimation(intent, showsWhileLocked,
            animation, worker, onStarted);
}

ECode KeyguardActivityLauncher::LaunchActivityWithAnimation(
    /* [in] */ IIntent* intent,
    /* [in] */ Boolean showsWhileLocked,
    /* [in] */ IBundle* animation,
    /* [in] */ IHandler* worker,
    /* [in] */ IRunnable* onStarted)
{
    AutoPtr<ILockPatternUtils> lockPatternUtils;
    GetLockPatternUtils((ILockPatternUtils**)&lockPatternUtils);
    intent->AddFlags(
            IIntent::FLAG_ACTIVITY_NEW_TASK
            | IIntent::FLAG_ACTIVITY_SINGLE_TOP
            | IIntent::FLAG_ACTIVITY_CLEAR_TOP);
    Boolean mustLaunchSecurely = MustLaunchSecurely();
    if (!mustLaunchSecurely || showsWhileLocked) {
        if (!mustLaunchSecurely) {
            DismissKeyguardOnNextActivity();
        }
        //try {
        if (DEBUG) Logger::D(TAG, "Starting activity for intent %s at %lld",
                TO_CSTR(intent), SystemClock::GetUptimeMillis());
        StartActivityForCurrentUser(intent, animation, worker, onStarted);
        // } catch (ActivityNotFoundException e) {
        //     Log.w(TAG, "Activity not found for intent + " + intent.getAction());
        // }
    } else {
        // Create a runnable to start the activity and ask the user to enter their
        // credentials.
        AutoPtr<IKeyguardHostViewOnDismissAction> ac =
            new MyOnDismissAction(this, intent, animation, worker, onStarted);
        SetOnDismissAction(ac);
        RequestDismissKeyguard();
    }
    return NOERROR;
}

void KeyguardActivityLauncher::DismissKeyguardOnNextActivity()
{
    //try {
    AutoPtr<IWindowManagerGlobalHelper> helper;
    CWindowManagerGlobalHelper::AcquireSingleton((IWindowManagerGlobalHelper**)&helper);
    AutoPtr<IIWindowManager> windowManager;
    helper->GetWindowManagerService((IIWindowManager**)&windowManager);
    windowManager->DismissKeyguard();
    //} catch (RemoteException e) {
        //Log.w(TAG, "Error dismissing keyguard", e);
    //}
}

void KeyguardActivityLauncher::StartActivityForCurrentUser(
    /* [in] */ IIntent* intent,
    /* [in] */ IBundle* options,
    /* [in] */ IHandler* worker,
    /* [in] */ IRunnable* onStarted)
{
    AutoPtr<IUserHandle> user;
    CUserHandle::New(IUserHandle::USER_CURRENT, (IUserHandle**)&user);
    if (worker == NULL || onStarted == NULL) {
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        context->StartActivityAsUser(intent, options, user);
        return;
    }
    // if worker + onStarted are supplied, run blocking activity launch call in the background
    AutoPtr<IRunnable> r = new MyRunnable(this, intent, options, onStarted, user);
    Boolean bval;
    worker->Post(r, &bval);
}

AutoPtr<IIntent> KeyguardActivityLauncher::GetCameraIntent()
{
    return MustLaunchSecurely() ? SECURE_CAMERA_INTENT : INSECURE_CAMERA_INTENT;
}

Boolean KeyguardActivityLauncher::WouldLaunchResolverActivity(
    /* [in] */ IIntent* intent)
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IPackageManager> packageManager;
    context->GetPackageManager((IPackageManager**)&packageManager);

    AutoPtr<ILockPatternUtils> utils;
    GetLockPatternUtils((ILockPatternUtils**)&utils);
    Int32 user;
    utils->GetCurrentUser(&user);
    AutoPtr<IResolveInfo> resolved;
    packageManager->ResolveActivityAsUser(intent,
            IPackageManager::MATCH_DEFAULT_ONLY, user, (IResolveInfo**)&resolved);

    AutoPtr<IList> appList;
    packageManager->QueryIntentActivitiesAsUser(
            intent, IPackageManager::MATCH_DEFAULT_ONLY, user, (IList**)&appList);
    return WouldLaunchResolverActivity(resolved, appList);
}

Boolean KeyguardActivityLauncher::WouldLaunchResolverActivity(
    /* [in] */ IResolveInfo* resolved,
    /* [in] */ IList* appList)
{
    // If the list contains the above resolved activity, then it can't be
    // ResolverActivity itself.
    Int32 size;
    appList->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        appList->Get(i, (IInterface**)&obj);
        AutoPtr<IResolveInfo> tmp = IResolveInfo::Probe(obj);

        AutoPtr<IActivityInfo> activityInfo;
        tmp->GetActivityInfo((IActivityInfo**)&activityInfo);
        String name, pName;
        IPackageItemInfo::Probe(activityInfo)->GetName(&name);
        IPackageItemInfo::Probe(activityInfo)->GetPackageName(&pName);

        AutoPtr<IActivityInfo> activityInfo2;
        resolved->GetActivityInfo((IActivityInfo**)&activityInfo2);
        String name2, pName2;
        IPackageItemInfo::Probe(activityInfo2)->GetName(&name2);
        IPackageItemInfo::Probe(activityInfo2)->GetPackageName(&pName2);

        if (name.Equals(name2) && pName.Equals(pName2)) {
            return FALSE;
        }
    }
    return TRUE;
}

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
