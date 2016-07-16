
#include "elastos/droid/systemui/keyguard/AppearAnimationUtils.h"
#include "Elastos.Droid.View.h"


namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

CAR_INTERFACE_IMPL(KeyguardActivityLauncher::MyOnDismissAction, Object, IKeyguardHostViewOnDismissAction)

ECode KeyguardActivityLauncher::MyOnDismissAction::OnDismiss(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    mHost->DismissKeyguardOnNextActivity();
    mHost->StartActivityForCurrentUser(intent, animation, worker, onStarted);
    *result = TRUE;
    return NOERROR;
}

ECode KeyguardActivityLauncher::MyRunnable::Run()
{
    //try {
    ECode ec = NOERROR;
    AutoPtr<IIActivityManager> manager = ActivityManagerNative::GetDefault();

    AutoPtr<IContext> context;
    FAIL_GOTO(ec = GetContext((IContext**)&context), ERROR)
    AutoPtr<IContentResolver> cr;
    FAIL_GOTO(ec = context->GetContentResolver((IContentResolver**)&cr), ERROR)
    String type;
    FAIL_GOTO(ec = intent->ResolveTypeIfNeeded(cr, &type), ERROR)
    Int32 id;
    FAIL_GOTO(ec = mUser->GetIdentifier(&id), ERROR)

    AutoPtr<IActivityManagerWaitResult> result;
    FAIL_GOTO(ec =manager->StartActivityAndWait(
            NULL /*caller*/,
            NULL /*caller pkg*/,
            intent,
            type,
            NULL /*resultTo*/,
            NULL /*resultWho*/,
            0 /*requestCode*/,
            IIntent::FLAG_ACTIVITY_NEW_TASK,
            NULL /*profilerInfo*/,
            options,
            id,
            (IActivityManagerWaitResult**)&result), ERROR)
        // if (DEBUG) Log.d(TAG, String.format("waitResult[%s,%s,%s,%s] at %s",
        //         result.result, result.thisTime, result.totalTime, result.who,
        //         SystemClock.uptimeMillis()));
    //} catch (RemoteException e) {
ERROR:
    if (ec == (ECode)RemoteException) {
        Logger::W(TAG, "Error starting activity %d", ec);
        return NOERROR;
    }
    // try {
    onStarted->Run();
    //} catch (Throwable t) {
        Log.w(TAG, "Error running onStarted callback", t);
    //}
    return NOERROR;
}

const String KeyguardActivityLauncher::TAG("KeyguardActivityLauncher");// = KeyguardActivityLauncher.class.getSimpleName();

const Boolean KeyguardActivityLauncher::DEBUG = IKeyguardConstants::DEBUG;

const String KeyguardActivityLauncher::META_DATA_KEYGUARD_LAYOUT("com.android.keyguard.layout");

static AutoPtr<IIntent> initSECURE_CAMERA_INTENT()
{
    AutoPtr<IIntent> intent;
    CIntent::New(IMediaStore::INTENT_ACTION_STILL_IMAGE_CAMERA_SECURE, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS);
    return intent;
}

AutoPtr<IIntent> KeyguardActivityLauncher::SECURE_CAMERA_INTENT = initSECURE_CAMERA_INTENT();

static AutoPtr<IIntent> initINSECURE_CAMERA_INTENT()
{
    AutoPtr<IIntent> intent;
    CIntent::New(IMediaStore::INTENT_ACTION_STILL_IMAGE_CAMERA, (IIntent**)&intent);
    return intent;
}

AutoPtr<IIntent> KeyguardActivityLauncher::INSECURE_CAMERA_INTENT = initINSECURE_CAMERA_INTENT();

AutoPtr<CameraWidgetInfo> KeyguardActivityLauncher::GetCameraWidgetInfo()
{
    AutoPtr<CameraWidgetInfo> info = new KeyguardActivityLauncher::CameraWidgetInfo();
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
        return NULL;
    }
    AutoPtr<IResolveInfo> resolved;
    packageManager->ResolveActivityAsUser(intent,
            IPackageManager::MATCH_DEFAULT_ONLY | IPackageManager::GET_META_DATA,
            user);
    if (DEBUG) Logger::D(TAG, "getCameraWidgetInfo(): resolved: %s", TO_CSTR(resolved));
    if (WouldLaunchResolverActivity(resolved, appList)) {
        if (DEBUG) Logger::D(TAG, "getCameraWidgetInfo(): Would launch resolver");
        return info;
    }
    if (resolved == NULL) {
        return NULL;
    }

    AutoPtr<IActivityInfo> activityInfo;
    resolved->GetActivityInfo((IActivityInfo**)&activityInfo);
    if (activityInfo == NULL) {
        return NULL;
    }

    AutoPtr<IBundle> metaData;
    IPackageItemInfo::Probe(activityInfo)->GetMetaData((IBundle**)&metaData);
    Boolean res;
    if (metaData == NULL || (metaData->IsEmpty(&res), res)) {
        if (DEBUG) Logger::D(TAG, "getCameraWidgetInfo(): no metadata found");
        return info;
    }
    Int32 layoutId;
    metaData->GetInt32(META_DATA_KEYGUARD_LAYOUT, &layoutId);
    if (layoutId == 0) {
        if (DEBUG) Logger::D(TAG, "getCameraWidgetInfo(): no layout specified");
        return info;
    }

    activityInfo->GetPackageName(&(info->mContextPackage));
    info->mLayoutId = layoutId;
    return info;
}

ECode KeyguardActivityLauncher::LaunchCamera(
    /* [in] */ IHandler* worker,
    /* [in] */ IRunnable* onSecureCameraStarted)
{
    AutoPtr<ILockPatternUtils> lockPatternUtils = GetLockPatternUtils();

    // Workaround to avoid camera release/acquisition race when resuming face unlock
    // after showing lockscreen camera (bug 11063890).
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<KeyguardUpdateMonitor> updateMonitor = KeyguardUpdateMonitor::GetInstance(context);
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
    AutoPtr<ILockPatternUtils> lockPatternUtils = GetLockPatternUtils();
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<KeyguardUpdateMonitor> updateMonitor = KeyguardUpdateMonitor::GetInstance(context);
    Int32 currentUser;
    lockPatternUtils->GetCurrentUser(&currentUser);
    Boolean res;
    return lockPatternUtils->IsSecure() && (updateMonitor->GetUserHasTrust(currentUser, &res), !res);
}

ECode KeyguardActivityLauncher::LaunchWidgetPicker(
    /* [in] */ Int32 appWidgetId)
{
    AutoPtr<IIntent> pickIntent;
    CIntent::New(IAppWidgetManager::ACTION_KEYGUARD_APPWIDGET_PICK, (IIntent**)&pickIntent);

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
    AutoPtr<ILockPatternUtils> lockPatternUtils = GetLockPatternUtils();
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
        if (DEBUG) Logger::D(TAG, "Starting activity for intent %s at %d",
                TO_CSTR(intent), SystemClock::UptimeMillis()));
        StartActivityForCurrentUser(intent, animation, worker, onStarted);
        // } catch (ActivityNotFoundException e) {
        //     Log.w(TAG, "Activity not found for intent + " + intent.getAction());
        // }
    } else {
        // Create a runnable to start the activity and ask the user to enter their
        // credentials.
        AutoPtr<IKeyguardHostViewOnDismissAction> ac = new MyOnDismissAction(this);
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
    AutoPtr<IRunnabl> r = new MyRunnable(this, user);
    worker->Post(r);
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
    utils->getCurrentUser(&user);
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
        String name;
        IPackageItemInfo::Probe(activityInfo)->GetName(&name);
        String pName;
        IPackageItemInfo::Probe(activityInfo)->GetPackageName(&pName);

        AutoPtr<IActivityInfo> activityInfo2;
        resolved->GetActivityInfo((IActivityInfo**)&activityInfo2);
        String name2;
        IPackageItemInfo::Probe(activityInfo2)->GetName(&name2);
        String pName2;
        IPackageItemInfo::Probe(activityInfo2)->GetPackageName(&pName2);

        if (name.Equals(name2) && pName.equals(pName2)) {
            return FALSE;
        }
    }
    return TRUE;
}

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
