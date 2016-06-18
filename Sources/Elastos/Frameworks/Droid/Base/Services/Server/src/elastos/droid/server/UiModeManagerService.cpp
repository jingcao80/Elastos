
#include "elastos/droid/server/UiModeManagerService.h"
#include "elastos/droid/server/CUiModeManagerBinderService.h"
#include <elastos/droid/R.h>
#include <elastos/droid/os/Binder.h>
#include <elastos/droid/os/UserHandle.h>
#include <elastos/droid/provider/Settings.h>
#include <elastos/droid/app/ActivityManagerNative.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Slogger.h>
#include <Elastos.Droid.Internal.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Provider.h>
#include <Elastos.Droid.Service.h>

using Elastos::Droid::R;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::App::IUiModeManager;
using Elastos::Droid::App::INotification;
using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IActivityManager;
using Elastos::Droid::App::IIActivityManager;
using Elastos::Droid::App::IPendingIntentHelper;
using Elastos::Droid::App::CPendingIntentHelper;
using Elastos::Droid::App::CNotification;
using Elastos::Droid::App::EIID_IIUiModeManager;
using Elastos::Droid::Internal::App::ECLSID_CDisableCarModeActivity;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::Res::CConfiguration;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::Settings;
using Elastos::Droid::Service::Dreams::ISandman;
using Elastos::Droid::Service::Dreams::CSandman;
using Elastos::Droid::Server::Twilight::EIID_ITwilightListener;
using Elastos::Droid::Server::Twilight::EIID_ITwilightManager;
using Elastos::Core::AutoLock;
using Elastos::Core::StringBuilder;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {

const String UiModeManagerService::TAG("UiModeManagerService");
const Boolean UiModeManagerService::LOG = FALSE;

// Enable launching of applications when entering the dock.
const Boolean UiModeManagerService::ENABLE_LAUNCH_CAR_DOCK_APP = TRUE;
const Boolean UiModeManagerService::ENABLE_LAUNCH_DESK_DOCK_APP = TRUE;


//==================================================================
// UiModeManagerService::BinderService
//==================================================================
CAR_INTERFACE_IMPL_2(UiModeManagerService::BinderService, Object, IIUiModeManager, IBinder)

ECode UiModeManagerService::BinderService::constructor(
    /* [in] */ ISystemService* umm)
{
    mHost = (UiModeManagerService*)umm;
    return NOERROR;
}

ECode UiModeManagerService::BinderService::DisableCarMode(
    /* [in] */ Int32 flags)
{
    Int64 ident = Binder::ClearCallingIdentity();

     {
        AutoLock Lock(mHost->mLock);
        mHost->SetCarModeLocked(FALSE, 0);
        if (mHost->mSystemReady) {
            mHost->UpdateLocked(0, flags);
        }
    }

    Binder::RestoreCallingIdentity(ident);

    return NOERROR;
}

ECode UiModeManagerService::BinderService::EnableCarMode(
    /* [in] */ Int32 flags)
{
    Int64 ident = Binder::ClearCallingIdentity();
    {
        AutoLock Lock(mHost->mLock);
        mHost->SetCarModeLocked(TRUE, flags);
        if (mHost->mSystemReady) {
            mHost->UpdateLocked(flags, 0);
        }
    }

    Binder::RestoreCallingIdentity(ident);

    return NOERROR;
}

ECode UiModeManagerService::BinderService::GetCurrentModeType(
    /* [out] */ Int32 *type)
{
    VALIDATE_NOT_NULL(type);
    Int64 ident = Binder::ClearCallingIdentity();

    {
        AutoLock Lock(mHost->mLock);
        *type = mHost->mCurUiMode & IConfiguration::UI_MODE_TYPE_MASK;
    }

    Binder::RestoreCallingIdentity(ident);

    return NOERROR;
}

ECode UiModeManagerService::BinderService::SetNightMode(
    /* [in] */ Int32 mode)
{
    switch (mode) {
        case IUiModeManager::MODE_NIGHT_NO:
        case IUiModeManager::MODE_NIGHT_YES:
        case IUiModeManager::MODE_NIGHT_AUTO:
            break;
        default:
        {
            Slogger::E(TAG, "Unknown mode: %d", mode);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }

    Int64 ident = Binder::ClearCallingIdentity();
    {
        AutoLock Lock(mHost->mLock);
        Boolean isDoingNightModeLocked;
        mHost->IsDoingNightModeLocked(&isDoingNightModeLocked);
        if (isDoingNightModeLocked && mHost->mNightMode != mode) {
            AutoPtr<IContext> context;
            mHost->GetContext((IContext**)&context);
            AutoPtr<IContentResolver> resolver;
            context->GetContentResolver((IContentResolver**)&resolver);
            Boolean result;
            Settings::Secure::PutInt32(resolver,
                ISettingsSecure::UI_NIGHT_MODE, mode, &result);
            mHost->mNightMode = mode;
            mHost->UpdateLocked(0, 0);
        }
    }

    Binder::RestoreCallingIdentity(ident);

    return NOERROR;
}

ECode UiModeManagerService::BinderService::GetNightMode(
    /* [out] */ Int32 *mode)
{
    VALIDATE_NOT_NULL(mode);
    AutoLock Lock(mHost->mLock);
    *mode = mHost->mNightMode;
    return NOERROR;
}

ECode UiModeManagerService::BinderService::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = "UiModeManagerService::BinderService";
    return NOERROR;
}

// @Override
// ECode UiModeManagerService::BinderService::Dump(
//     /* [in] */ IFileDescriptor* fd,
//     /* [in] */ IPrintWriter* pw,
//     /* [out] */ ArrayOf<String>* args)
// {
//     if (getContext().checkCallingOrSelfPermission(android.Manifest.permission.DUMP)
//             != PackageManager.PERMISSION_GRANTED) {

//         pw.println("Permission Denial: can't dump uimode service from from pid="
//                 + Binder.getCallingPid()
//                 + ", uid=" + Binder.getCallingUid());
//         return;
//     }

//     dumpImpl(pw);
// }

//==================================================================
// UiModeManagerService::ResultReceiver
//==================================================================
UiModeManagerService::ResultReceiver::ResultReceiver(
    /* [in] */ UiModeManagerService* host)
{
    mHost = host;
}

ECode UiModeManagerService::ResultReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    Int32 resultCode;
    GetResultCode(&resultCode);
    if (resultCode != IActivity::RESULT_OK) {
        if (LOG) {
            StringBuilder sb("Handling broadcast result for action ");
            String action;
            intent->GetAction(&action);
            sb += action;
            sb += ": canceled: ";
            sb += resultCode;
            Slogger::V(TAG, sb.ToString());
        }
        return E_NULL_POINTER_EXCEPTION;
    }

    Int32 enableFlags;
    intent->GetInt32Extra(String("enableFlags"), 0, &enableFlags);
    Int32 disableFlags;
    intent->GetInt32Extra(String("disableFlags"), 0, &disableFlags);
    AutoLock Lock(mHost->mLock);
    String action;
    intent->GetAction(&action);
    mHost->UpdateAfterBroadcastLocked(action, enableFlags, disableFlags);
    return NOERROR;
}

//==================================================================
// UiModeManagerService::DockModeReceiver
//==================================================================
UiModeManagerService::DockModeReceiver::DockModeReceiver(
    /* [in] */ UiModeManagerService* host)
{
    mHost = host;
}

ECode UiModeManagerService::DockModeReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    Int32 state;
    intent->GetInt32Extra(IIntent::EXTRA_DOCK_STATE,
            IIntent::EXTRA_DOCK_STATE_UNDOCKED, &state);
    mHost->UpdateDockState(state);
    return NOERROR;
}

//==================================================================
// UiModeManagerService::BatteryReceiver
//==================================================================
UiModeManagerService::BatteryReceiver::BatteryReceiver(
    /* [in] */ UiModeManagerService* host)
{
    mHost = host;
}

ECode UiModeManagerService::BatteryReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    Int32 state;
    intent->GetInt32Extra(String("plugged"), 0, &state);//BatteryManager.EXTRA_PLUGGED
    mHost->mCharging = (state != 0);
    AutoLock Lock(mHost->mLock);
    if (mHost->mSystemReady) {
        mHost->UpdateLocked(0, 0);
    }
    return NOERROR;
}

//==================================================================
// UiModeManagerService::MyTwilightListener
//==================================================================
CAR_INTERFACE_IMPL(UiModeManagerService::MyTwilightListener, Object, ITwilightListener);

UiModeManagerService::MyTwilightListener::MyTwilightListener(
    /* [in] */ UiModeManagerService* host)
{
    mHost = host;
}

ECode UiModeManagerService::MyTwilightListener::OnTwilightStateChanged()
{
    mHost->UpdateTwilight();
    return NOERROR;
}

//==================================================================
// UiModeManagerService
//==================================================================
UiModeManagerService::UiModeManagerService()
    : mDockState(IIntent::EXTRA_DOCK_STATE_UNDOCKED)
    , mNightMode(IUiModeManager::MODE_NIGHT_NO)
    , mCarModeEnabled(FALSE)
    , mCharging(FALSE)
    , mDefaultUiModeType(0)
    , mCarModeKeepsScreenOn(FALSE)
    , mDeskModeKeepsScreenOn(FALSE)
    , mTelevision(FALSE)
    , mWatch(FALSE)
    , mComputedNightMode(FALSE)
    , mCarModeEnableFlags(FALSE)
    , mCurUiMode(0)
    , mSetUiMode(0)
    , mHoldingConfiguration(FALSE)
    , mSystemReady(FALSE)
{
}

AutoPtr<IIntent> UiModeManagerService::BuildHomeIntent(
    /* [in] */ const String& category)
{
    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_MAIN, (IIntent**)&intent);
    intent->AddCategory(category);
    intent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK
            | IIntent::FLAG_ACTIVITY_RESET_TASK_IF_NEEDED);
    return intent;
}

ECode UiModeManagerService::constructor(
    /* [in] */ IContext* context)
{
    CHandler::New((IHandler**)&mHandler);
    CConfiguration::New((IConfiguration**)&mConfiguration);

    mResultReceiver = new ResultReceiver(this);
    mDockModeReceiver = new DockModeReceiver(this);
    mBatteryReceiver = new BatteryReceiver(this);
    mTwilightListener = new MyTwilightListener(this);

    return SystemService::constructor(context);
}

ECode UiModeManagerService::OnStart()
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);

    AutoPtr<IInterface> service = GetLocalService(EIID_ITwilightManager);
    mTwilightManager = ITwilightManager::Probe(service);

    service = NULL;
    context->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&service);
    AutoPtr<IPowerManager> powerManager = IPowerManager::Probe(service);
    powerManager->NewWakeLock(IPowerManager::FULL_WAKE_LOCK, TAG, (IPowerManagerWakeLock**)&mWakeLock);

    AutoPtr<IIntentFilter> intentFilter;
    CIntentFilter::New(IIntent::ACTION_DOCK_EVENT, (IIntentFilter**)&intentFilter);
    AutoPtr<IIntent> intent;
    context->RegisterReceiver((IBroadcastReceiver*)mDockModeReceiver,
        intentFilter, (IIntent**)&intent);

    intent = NULL;
    AutoPtr<IIntentFilter> intentFilter2;
    CIntentFilter::New(IIntent::ACTION_BATTERY_CHANGED, (IIntentFilter**)&intentFilter2);
    context->RegisterReceiver((IBroadcastReceiver*)mBatteryReceiver,
        intentFilter2, (IIntent**)&intent);

    mConfiguration->SetToDefaults();

    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    resources->GetInteger(R::integer::config_defaultUiModeType, &mDefaultUiModeType);

    Int32 result;
    resources->GetInteger(R::integer::config_carDockKeepsScreenOn, &result);
    mCarModeKeepsScreenOn = (result == 1);

    resources->GetInteger(R::integer::config_deskDockKeepsScreenOn, &result);
    mDeskModeKeepsScreenOn = (result == 1);

    AutoPtr<IPackageManager> pm;
    context->GetPackageManager((IPackageManager**)&pm);
    pm->HasSystemFeature(IPackageManager::FEATURE_TELEVISION, &mTelevision);
    if (!mTelevision) {
        pm->HasSystemFeature(IPackageManager::FEATURE_LEANBACK, &mTelevision);
    }
    pm->HasSystemFeature(IPackageManager::FEATURE_WATCH, &mWatch);


    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    Settings::Secure::GetInt32(resolver, ISettingsSecure::UI_NIGHT_MODE,
        IUiModeManager::MODE_NIGHT_AUTO, &mNightMode);

    if (mTwilightManager != NULL) {
        mTwilightManager->RegisterListener(mTwilightListener, mHandler);
    }

    CUiModeManagerBinderService::New(this, (IIUiModeManager**)&mService);
    PublishBinderService(IContext::UI_MODE_SERVICE, IBinder::Probe(mService));
    return NOERROR;
}

ECode UiModeManagerService::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TAG;
    return NOERROR;
}

// void dumpImpl(PrintWriter pw) {
//     {    AutoLock syncLock(mLock);
//         pw.println("Current UI Mode Service state:");
//         pw.print("  mDockState="); pw.print(mDockState);
//                 pw.print(" mLastBroadcastState="); pw.println(mLastBroadcastState);
//         pw.print("  mNightMode="); pw.print(mNightMode);
//                 pw.print(" mCarModeEnabled="); pw.print(mCarModeEnabled);
//                 pw.print(" mComputedNightMode="); pw.print(mComputedNightMode);
//                 pw.print(" mCarModeEnableFlags="); pw.println(mCarModeEnableFlags);
//         pw.print("  mCurUiMode=0x"); pw.print(Integer.toHexString(mCurUiMode));
//                 pw.print(" mSetUiMode=0x"); pw.println(Integer.toHexString(mSetUiMode));
//         pw.print("  mHoldingConfiguration="); pw.print(mHoldingConfiguration);
//                 pw.print(" mSystemReady="); pw.println(mSystemReady);
//         pw.print("  mTwilightService.getCurrentState()=");
//                 pw.println(mTwilightManager.getCurrentState());
//     }
// }

ECode UiModeManagerService::OnBootPhase(
    /* [in] */ Int32 phase)
{
    if (phase == ISystemService::PHASE_SYSTEM_SERVICES_READY) {
        AutoLock Lock(mLock);
        mSystemReady = TRUE;
        mCarModeEnabled = (mDockState == IIntent::EXTRA_DOCK_STATE_CAR);
        UpdateComputedNightModeLocked();
        UpdateLocked(0, 0);
    }
    return NOERROR;
}

ECode UiModeManagerService::IsDoingNightModeLocked(
    /* [out] */ Boolean* isDoingNightModeLocked)
{
    VALIDATE_NOT_NULL(isDoingNightModeLocked);
    *isDoingNightModeLocked = mCarModeEnabled || mDockState != IIntent::EXTRA_DOCK_STATE_UNDOCKED;
    return NOERROR;
}

ECode UiModeManagerService::SetCarModeLocked(
    /* [in] */ Boolean enabled,
    /* [in] */ Int32 flags)
{
    if (mCarModeEnabled != enabled) {
        mCarModeEnabled = enabled;
    }
    mCarModeEnableFlags = flags;
    return NOERROR;
}

ECode UiModeManagerService::UpdateDockState(
    /* [in] */ Int32 newState)
{
    AutoLock Lock(mLock);
    if (newState != mDockState) {
        mDockState = newState;
        SetCarModeLocked(mDockState == IIntent::EXTRA_DOCK_STATE_CAR, 0);
        if (mSystemReady) {
            UpdateLocked(IUiModeManager::ENABLE_CAR_MODE_GO_CAR_HOME, 0);
        }
    }
    return NOERROR;
}

Boolean UiModeManagerService::IsDeskDockState(
    /* [in] */ Int32 state)
{
    switch (state) {
        case IIntent::EXTRA_DOCK_STATE_DESK:
        case IIntent::EXTRA_DOCK_STATE_LE_DESK:
        case IIntent::EXTRA_DOCK_STATE_HE_DESK:
            return TRUE;
        default:
            return FALSE;
    }
}

ECode UiModeManagerService::UpdateConfigurationLocked()
{

    Int32 uiMode = mDefaultUiModeType;
    if (mTelevision) {
        uiMode = IConfiguration::UI_MODE_TYPE_TELEVISION;
    }
    else if (mWatch) {
        uiMode = IConfiguration::UI_MODE_TYPE_WATCH;
    }
    else if (mCarModeEnabled) {
        if (mNightMode == IUiModeManager::MODE_NIGHT_AUTO) {
            UpdateComputedNightModeLocked();
            uiMode |= mComputedNightMode ? IConfiguration::UI_MODE_NIGHT_YES
                    : IConfiguration::UI_MODE_NIGHT_NO;
        } else {
            uiMode |= mNightMode << 4;
        }
    }
    else {
        // Disabling the car mode clears the night mode.
        uiMode = (uiMode & ~IConfiguration::UI_MODE_NIGHT_MASK) | IConfiguration::UI_MODE_NIGHT_NO;
    }

    if (LOG) {
        StringBuilder sb("updateConfigurationLocked: mDockState=");
        sb += mDockState;
        sb += "; mCarMode=";
        sb += mCarModeEnabled;
        sb += "; mNightMode=";
        sb += mNightMode;
        sb += "; uiMode=";
        sb += uiMode;
        Slogger::D(TAG, sb.ToString());
    }

    mCurUiMode = uiMode;
    if (!mHoldingConfiguration) {
        mConfiguration->SetUiMode(uiMode);
    }
    return NOERROR;
}

ECode UiModeManagerService::SendConfigurationLocked()
{
    Int32 uiMode;
    mConfiguration->GetUiMode(&uiMode);
    if (mSetUiMode != uiMode) {
        mSetUiMode = uiMode;

        //try {
        AutoPtr<IIActivityManager> am = ActivityManagerNative::GetDefault();
        am->UpdateConfiguration(mConfiguration);
        //} catch (RemoteException e) {
        //    Slog.w(TAG, "Failure communicating with activity manager", e);
        //}
    }
    return NOERROR;
}

ECode UiModeManagerService::UpdateLocked(
    /* [in] */ Int32 enableFlags,
    /* [in] */ Int32 disableFlags)
{
    String action(NULL);
    String oldAction(NULL);
    if (mLastBroadcastState == IIntent::EXTRA_DOCK_STATE_CAR) {
        AdjustStatusBarCarModeLocked();
        oldAction = IUiModeManager::ACTION_EXIT_CAR_MODE;
    }
    else if (IsDeskDockState(mLastBroadcastState)) {
        oldAction = IUiModeManager::ACTION_EXIT_DESK_MODE;
    }

    String nullStr;

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    if (mCarModeEnabled) {
        if (mLastBroadcastState != IIntent::EXTRA_DOCK_STATE_CAR) {
            AdjustStatusBarCarModeLocked();

            if (!oldAction.IsNull()) {
                AutoPtr<IIntent> intent;
                CIntent::New(oldAction, (IIntent**)&intent);
                context->SendBroadcastAsUser(intent, UserHandle::ALL);
            }
            mLastBroadcastState = IIntent::EXTRA_DOCK_STATE_CAR;
            action = IUiModeManager::ACTION_ENTER_CAR_MODE;
        }
    }
    else if (IsDeskDockState(mDockState)) {
        if (!IsDeskDockState(mLastBroadcastState)) {
            if (!oldAction.IsNull()) {
                AutoPtr<IIntent> intent;
                CIntent::New(oldAction, (IIntent**)&intent);
                context->SendBroadcastAsUser(intent, UserHandle::ALL);
            }
            mLastBroadcastState = mDockState;
            action = IUiModeManager::ACTION_ENTER_DESK_MODE;
        }
    }
    else {
        mLastBroadcastState = IIntent::EXTRA_DOCK_STATE_UNDOCKED;
        action = oldAction;
    }

    if (!action.IsNull()) {
        if (LOG) {
            Slogger::V(TAG,
                "updateLocked: preparing broadcast: action=%s enable=0x%08x disable=0x%08x",
                action.string(), enableFlags, disableFlags);
        }

        // Send the ordered broadcast; the result receiver will receive after all
        // broadcasts have been sent. If any broadcast receiver changes the result
        // code from the initial value of RESULT_OK, then the result receiver will
        // not launch the corresponding dock application. This gives apps a chance
        // to override the behavior and stay in their app even when the device is
        // placed into a dock.
        AutoPtr<IIntent> intent;
        CIntent::New(action, (IIntent**)&intent);
        intent->PutExtra(String("enableFlags"), enableFlags);
        intent->PutExtra(String("disableFlags"), disableFlags);
        context->SendOrderedBroadcastAsUser(intent, UserHandle::CURRENT, nullStr,
            mResultReceiver, NULL, IActivity::RESULT_OK, nullStr, NULL);

        // Attempting to make this transition a little more clean, we are going
        // to hold off on doing a configuration change until we have finished
        // the broadcast and started the home activity.
        mHoldingConfiguration = TRUE;
        UpdateConfigurationLocked();
    } else {
        String category(NULL);
        if (mCarModeEnabled) {
            if (ENABLE_LAUNCH_CAR_DOCK_APP
                    && (enableFlags & IUiModeManager::ENABLE_CAR_MODE_GO_CAR_HOME) != 0) {
                category = IIntent::CATEGORY_CAR_DOCK;
            }
        } else if (IsDeskDockState(mDockState)) {
            if (ENABLE_LAUNCH_DESK_DOCK_APP
                    && (enableFlags & IUiModeManager::ENABLE_CAR_MODE_GO_CAR_HOME) != 0) {
                category = IIntent::CATEGORY_DESK_DOCK;
            }
        } else {
            if ((disableFlags & IUiModeManager::DISABLE_CAR_MODE_GO_HOME) != 0) {
                category = IIntent::CATEGORY_HOME;
            }
        }

        if (LOG) {
            StringBuilder sb("updateLocked: null action, mDockState=");
            sb += mDockState;
            sb += ", category=";
            sb += category;
            Slogger::V(String(TAG), sb.ToString());
        }

        SendConfigurationAndStartDreamOrDockAppLocked(category);
    }

    // keep screen on when charging and in car mode
    Boolean keepScreenOn = mCharging &&
        ((mCarModeEnabled && mCarModeKeepsScreenOn &&
          (mCarModeEnableFlags & IUiModeManager::ENABLE_CAR_MODE_ALLOW_SLEEP) == 0) ||
         (mCurUiMode == IConfiguration::UI_MODE_TYPE_DESK && mDeskModeKeepsScreenOn));
    Boolean isHeld;
    mWakeLock->IsHeld(&isHeld);
    if (keepScreenOn != isHeld) {
        if (keepScreenOn) {
            mWakeLock->AcquireLock();
        } else {
            mWakeLock->ReleaseLock();
        }
    }
    return NOERROR;
}

ECode UiModeManagerService::UpdateAfterBroadcastLocked(
    /* [in] */ const String& action,
    /* [in] */ Int32 enableFlags,
    /* [in] */ Int32 disableFlags)
{
    // Launch a dock activity
    String category;
    if (IUiModeManager::ACTION_ENTER_CAR_MODE.Equals(action)) {
        // Only launch car home when car mode is enabled and the caller
        // has asked us to switch to it.
        if (ENABLE_LAUNCH_CAR_DOCK_APP
                && (enableFlags & IUiModeManager::ENABLE_CAR_MODE_GO_CAR_HOME) != 0) {
            category = IIntent::CATEGORY_CAR_DOCK;
        }
    } else if (IUiModeManager::ACTION_ENTER_DESK_MODE.Equals(action)) {
        // Only launch car home when desk mode is enabled and the caller
        // has asked us to switch to it.  Currently re-using the car
        // mode flag since we don't have a formal API for "desk mode".
        if (ENABLE_LAUNCH_DESK_DOCK_APP
                && (enableFlags & IUiModeManager::ENABLE_CAR_MODE_GO_CAR_HOME) != 0) {
            category = IIntent::CATEGORY_DESK_DOCK;
        }
    } else {
        // Launch the standard home app if requested.
        if ((disableFlags & IUiModeManager::DISABLE_CAR_MODE_GO_HOME) != 0) {
            category = IIntent::CATEGORY_HOME;
        }
    }

    if (LOG) {
        Slogger::V(TAG, "Handling broadcast result for action %s: enable=0x%08x, disable=0x%08x, category=%s",
                action.string(), enableFlags, disableFlags, category.string());
    }
    SendConfigurationAndStartDreamOrDockAppLocked(category);
    return NOERROR;
}

ECode UiModeManagerService::SendConfigurationAndStartDreamOrDockAppLocked(
    /* [in] */ const String& category)
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<ISandman> sandman;
    CSandman::AcquireSingleton((ISandman**)&sandman);

    // Update the configuration but don't send it yet.
    mHoldingConfiguration = FALSE;
    UpdateConfigurationLocked();

    // Start the dock app, if there is one.
    Boolean dockAppStarted = FALSE;

    if (!category.IsNull()) {
        // Now we are going to be careful about switching the
        // configuration and starting the activity -- we need to
        // do this in a specific order under control of the
        // activity manager, to do it cleanly.  So compute the
        // new config, but don't set it yet, and let the
        // activity manager take care of both the start and config
        // change.
        AutoPtr<IIntent> homeIntent = BuildHomeIntent(category);
        Boolean should = FALSE;
        sandman->ShouldStartDockApp(context, homeIntent, &should);
        if (should) {
            //try {
            AutoPtr<IIActivityManager> am = ActivityManagerNative::GetDefault();
            Int32 result;
            String nullStr;
            am->StartActivityWithConfig(
                   NULL, nullStr, homeIntent, nullStr, NULL, nullStr, 0, 0,
                    mConfiguration, NULL, IUserHandle::USER_CURRENT, &result);
            if (result >= IActivityManager::START_SUCCESS) {
                dockAppStarted = TRUE;
            } else if (result != IActivityManager::START_INTENT_NOT_RESOLVED) {
                StringBuilder sb("Could not start dock app: ");
                sb += homeIntent;
                sb += ", startActivityWithConfig result ";
                sb += result;
                Slogger::E(TAG, sb.ToString());
            }
            //} catch (RemoteException ex) {
            //    Slog.e(TAG, "Could not start dock app: " + homeIntent, ex);
            //}
        }
    }

    // Send the new configuration.
    SendConfigurationLocked();

    // If we did not start a dock app, then start dreaming if supported.
    if (!category.IsNull() && !dockAppStarted) {
        sandman->StartDreamWhenDockedIfAppropriate(context);
    }
    return NOERROR;
}

ECode UiModeManagerService::AdjustStatusBarCarModeLocked()
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    if (mStatusBarManager == NULL) {
        AutoPtr<IInterface> obj;
        context->GetSystemService(IContext::STATUS_BAR_SERVICE, (IInterface**)&obj);
        mStatusBarManager = IStatusBarManager::Probe(obj);
    }

    // Fear not: StatusBarManagerService manages a list of requests to disable
    // features of the status bar; these are ORed together to form the
    // active disabled list. So if (for example) the device is locked and
    // the status bar should be totally disabled, the calls below will
    // have no effect until the device is unlocked.
    if (mStatusBarManager != NULL) {
        mStatusBarManager->Disable(mCarModeEnabled
            ? IStatusBarManager::DISABLE_NOTIFICATION_TICKER
            : IStatusBarManager::DISABLE_NONE);
    }

    if (mNotificationManager == NULL) {
        AutoPtr<IInterface> obj;
        context->GetSystemService(IContext::NOTIFICATION_SERVICE, (IInterface**)&obj);
        mNotificationManager = INotificationManager::Probe(obj);
    }

    String nullStr;
    if (mNotificationManager != NULL) {
        if (mCarModeEnabled) {
            AutoPtr<IIntent> carModeOffIntent;
            CIntent::New(context, ECLSID_CDisableCarModeActivity,
               (IIntent**)&carModeOffIntent);

            AutoPtr<INotification> n;
            CNotification::New((INotification**)&n);
            n->SetIcon(R::drawable::stat_notify_car_mode);
            n->SetDefaults(INotification::DEFAULT_LIGHTS);
            n->SetFlags(INotification::FLAG_ONGOING_EVENT);
            n->SetWhen(0);

            Int32 color;
            AutoPtr<IResources> res;
            context->GetResources((IResources**)&res);
            res->GetColor(R::color::system_notification_accent_color, &color);
            n->SetColor(color);

            String str1, str2;
            context->GetString(R::string::car_mode_disable_notification_title, &str1);
            context->GetString(R::string::car_mode_disable_notification_message, &str2);
            AutoPtr<IPendingIntentHelper> helper;
            CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&helper);
            AutoPtr<IPendingIntent> pi;
            helper->GetActivityAsUser(context, 0, carModeOffIntent, 0,
                NULL, UserHandle::CURRENT, (IPendingIntent**)&pi);
            AutoPtr<ICharSequence> strArg1, strArg2;
            CString::New(str1, (ICharSequence**)&strArg1);
            CString::New(str2, (ICharSequence**)&strArg2);

            n->SetLatestEventInfo(context, strArg1, strArg2, pi);

            mNotificationManager->NotifyAsUser(nullStr, R::string::car_mode_disable_notification_title, n, UserHandle::ALL);
        }
        else {
            mNotificationManager->CancelAsUser(nullStr, R::string::car_mode_disable_notification_title, UserHandle::ALL);
        }
    }
    return NOERROR;
}

ECode UiModeManagerService::UpdateTwilight()
{
    AutoLock Lock(mLock);
    Boolean isDoingNightModeLocked;
    IsDoingNightModeLocked(&isDoingNightModeLocked);
    if (isDoingNightModeLocked && mNightMode == IUiModeManager::MODE_NIGHT_AUTO) {
        UpdateComputedNightModeLocked();
        UpdateLocked(0, 0);
    }
    return NOERROR;
}

ECode UiModeManagerService::UpdateComputedNightModeLocked()
{
    if (mTwilightManager) {
        AutoPtr<ITwilightState> state;
        mTwilightManager->GetCurrentState((ITwilightState**)&state);
        if (state != NULL) {
            state->IsNight(&mComputedNightMode);
        }
    }
    return NOERROR;
}

}
}
}
