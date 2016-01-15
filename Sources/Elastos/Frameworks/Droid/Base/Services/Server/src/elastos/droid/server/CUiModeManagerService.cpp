
#include "CUiModeManagerService.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Slogger.h>
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/app/ActivityManagerNative.h"

using Elastos::Core::StringBuilder;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::App::IUiModeManager;
using Elastos::Droid::App::INotification;
using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IActivityManager;
using Elastos::Droid::App::IIActivityManager;
using Elastos::Droid::App::IPendingIntentHelper;
using Elastos::Droid::App::CPendingIntentHelper;
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
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Provider::CSettingsSecure;
using Elastos::Droid::Provider::ISettingsSecure;
//using Elastos::Droid::Service::ISandmanHelper;
//using Elastos::Droid::Service::CSandmanHelper;

namespace Elastos {
namespace Droid {
namespace Server {

const String CUiModeManagerService::TAG("UiModeManager");//UiModeManager.class.getSimpleName();
const Boolean CUiModeManagerService::LOG = FALSE;

// Enable launching of applications when entering the dock.
const Boolean CUiModeManagerService::ENABLE_LAUNCH_CAR_DOCK_APP = TRUE;
const Boolean CUiModeManagerService::ENABLE_LAUNCH_DESK_DOCK_APP = TRUE;

CUiModeManagerService::CUiModeManagerService()
{
    CHandler::New((IHandler**)&mHandler);
    mDockState = IIntent::EXTRA_DOCK_STATE_UNDOCKED;
    mLastBroadcastState = IIntent::EXTRA_DOCK_STATE_UNDOCKED;

    mNightMode = IUiModeManager::MODE_NIGHT_NO;
    mCarModeEnabled = FALSE;
    mCharging = FALSE;

    mDefaultUiModeType = 0;

    mCarModeKeepsScreenOn = FALSE;
    mDeskModeKeepsScreenOn = FALSE;
    mTelevision = FALSE;

    mComputedNightMode = FALSE;

    mCurUiMode = 0;
    mSetUiMode = 0;

    mHoldingConfiguration = FALSE;
    CConfiguration::New((IConfiguration **)&mConfiguration);

    mSystemReady = FALSE;

    mResultReceiver = new ResultReceiver(this);

    mDockModeReceiver = new DockModeReceiver(this);

    mBatteryReceiver = new BatteryReceiver(this);

    mTwilightListener = new _TwilightListener(this);
}

AutoPtr<IIntent> CUiModeManagerService::BuildHomeIntent(
    /* [in] */ const String& category)
{
    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_MAIN, (IIntent**)&intent);
    intent->AddCategory(category);
    intent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK
            | IIntent::FLAG_ACTIVITY_RESET_TASK_IF_NEEDED);
    return intent;
}

CUiModeManagerService::ResultReceiver::ResultReceiver(
    /* [in] */ CUiModeManagerService* host)
{
    mHost = host;
}

ECode CUiModeManagerService::ResultReceiver::OnReceive(
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

CUiModeManagerService::DockModeReceiver::DockModeReceiver(
    /* [in] */ CUiModeManagerService* host)
{
    mHost = host;
}

ECode CUiModeManagerService::DockModeReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    Int32 state;
    intent->GetInt32Extra(IIntent::EXTRA_DOCK_STATE,
            IIntent::EXTRA_DOCK_STATE_UNDOCKED, &state);
    mHost->UpdateDockState(state);
    return NOERROR;
}

CUiModeManagerService::BatteryReceiver::BatteryReceiver(
    /* [in] */ CUiModeManagerService* host)
{
    mHost = host;
}

ECode CUiModeManagerService::BatteryReceiver::OnReceive(
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

CAR_INTERFACE_IMPL(CUiModeManagerService::_TwilightListener, IInterface);

CUiModeManagerService::_TwilightListener::_TwilightListener(
    /* [in] */ CUiModeManagerService* host)
{
    mHost = host;
}

ECode CUiModeManagerService::_TwilightListener::OnTwilightStateChanged()
{
    mHost->UpdateTwilight();
    return NOERROR;
}

ECode CUiModeManagerService::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Handle32 twilight)
{
    mContext = context;
    mTwilightService = (TwilightService*)twilight;

    AutoPtr<IServiceManager> sm;
    CServiceManager::AcquireSingleton((IServiceManager**)&sm);
    sm->AddService(IContext::UI_MODE_SERVICE, (IIUiModeManager*)this);

    AutoPtr<IIntentFilter> intentFilter;
    CIntentFilter::New(IIntent::ACTION_DOCK_EVENT, (IIntentFilter**)&intentFilter);
    AutoPtr<IIntent> intent;
    mContext->RegisterReceiver((IBroadcastReceiver*)mDockModeReceiver,
            intentFilter, (IIntent**)&intent);

    AutoPtr<IIntentFilter> intentFilter2;
    CIntentFilter::New(IIntent::ACTION_BATTERY_CHANGED, (IIntentFilter**)&intentFilter2);
    AutoPtr<IIntent> intent2;
    mContext->RegisterReceiver((IBroadcastReceiver*)mBatteryReceiver,
            intentFilter2, (IIntent**)&intent2);

    context->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&mPowerManager);
    mPowerManager->NewWakeLock(IPowerManager::FULL_WAKE_LOCK, TAG, (IPowerManagerWakeLock**)&mWakeLock);

    mConfiguration->SetToDefaults();

    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    resources->GetInteger(0x010e0012, &mDefaultUiModeType);//com.android.internal.R.integer.config_defaultUiModeType

    Int32 result;
    resources->GetInteger(0x010e0014, &result);//com.android.internal.R.integer.config_carDockKeepsScreenOn
    mCarModeKeepsScreenOn = (result == 1);

    resources->GetInteger(0x010e0013, &result);//com.android.internal.R.integer.config_deskDockKeepsScreenOn
    mDeskModeKeepsScreenOn = (result == 1);

    AutoPtr<IPackageManager> pm;
    context->GetPackageManager((IPackageManager**)&pm);
    Boolean hasSystemFeature;
    pm->HasSystemFeature(IPackageManager::FEATURE_TELEVISION, &hasSystemFeature);

    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<ISettingsSecure> settingsSecure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
    settingsSecure->GetInt32(resolver, ISettingsSecure::UI_NIGHT_MODE,
            IUiModeManager::MODE_NIGHT_AUTO, &mNightMode);

    mTwilightService->RegisterListener((TwilightService::ITwilightListener*)mTwilightListener, mHandler);
    return NOERROR;
}

ECode CUiModeManagerService::DisableCarMode(
    /* [in] */ Int32 flags)
{
    Int64 ident = Binder::ClearCallingIdentity();

    do {
        AutoLock Lock(mLock);
        SetCarModeLocked(FALSE);
        if (mSystemReady) {
            UpdateLocked(0, flags);
        }
    } while(FALSE);

    Binder::RestoreCallingIdentity(ident);

    return NOERROR;
}

ECode CUiModeManagerService::EnableCarMode(
    /* [in] */ Int32 flags)
{
    Int64 ident = Binder::ClearCallingIdentity();
    do {
        AutoLock Lock(mLock);
        SetCarModeLocked(TRUE);
        if (mSystemReady) {
            UpdateLocked(flags, 0);
        }
    } while(0);

    Binder::RestoreCallingIdentity(ident);

    return NOERROR;
}

ECode CUiModeManagerService::GetCurrentModeType(
    /* [out] */ Int32 *type)
{
    VALIDATE_NOT_NULL(type);
    Int64 ident = Binder::ClearCallingIdentity();

    do {
        AutoLock Lock(mLock);
        *type = mCurUiMode & IConfiguration::UI_MODE_TYPE_MASK;
    }while(0);

    Binder::RestoreCallingIdentity(ident);

    return NOERROR;
}

ECode CUiModeManagerService::SetNightMode(
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
    do {
        AutoLock Lock(mLock);
        Boolean isDoingNightModeLocked;
        IsDoingNightModeLocked(&isDoingNightModeLocked);
        if (isDoingNightModeLocked && mNightMode != mode) {
            AutoPtr<IContentResolver> resolver;
            mContext->GetContentResolver((IContentResolver **)&resolver);
            Boolean result;
            AutoPtr<ISettingsSecure> settingsSecure;
            CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
            settingsSecure->PutInt32(resolver,
                    ISettingsSecure::UI_NIGHT_MODE, mode, &result);
            mNightMode = mode;
            UpdateLocked(0, 0);
        }
    } while(0);

    Binder::RestoreCallingIdentity(ident);

    return NOERROR;
}

ECode CUiModeManagerService::GetNightMode(
    /* [out] */ Int32 *mode)
{
    VALIDATE_NOT_NULL(mode);
    AutoLock Lock(mLock);
    *mode = mNightMode;
    return NOERROR;
}

ECode CUiModeManagerService::ToString(
    /* [out] */ String* result)
{
    return E_NOT_IMPLEMENTED;
}

ECode CUiModeManagerService::SystemReady()
{
    AutoLock Lock(mLock);
    mSystemReady = TRUE;
    mCarModeEnabled = (mDockState == IIntent::EXTRA_DOCK_STATE_CAR);
    UpdateComputedNightModeLocked();
    UpdateLocked(0, 0);
    return NOERROR;
}

ECode CUiModeManagerService::IsDoingNightModeLocked(
    /* [out] */ Boolean* isDoingNightModeLocked)
{
    VALIDATE_NOT_NULL(isDoingNightModeLocked);
    *isDoingNightModeLocked = mCarModeEnabled || mDockState != IIntent::EXTRA_DOCK_STATE_UNDOCKED;
    return NOERROR;
}

ECode CUiModeManagerService::SetCarModeLocked(
    /* [in] */ Boolean enabled)
{
    if (mCarModeEnabled != enabled) {
        mCarModeEnabled = enabled;
    }
    return NOERROR;
}

ECode CUiModeManagerService::UpdateDockState(
    /* [in] */ Int32 newState)
{
    AutoLock Lock(mLock);
    if (newState != mDockState) {
        mDockState = newState;
        SetCarModeLocked(mDockState == IIntent::EXTRA_DOCK_STATE_CAR);
        if (mSystemReady) {
            UpdateLocked(IUiModeManager::ENABLE_CAR_MODE_GO_CAR_HOME, 0);
        }
    }
    return NOERROR;
}

Boolean CUiModeManagerService::IsDeskDockState(
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

ECode CUiModeManagerService::UpdateConfigurationLocked()
{
    Int32 uiMode = mTelevision ? IConfiguration::UI_MODE_TYPE_TELEVISION : mDefaultUiModeType;
    if (mCarModeEnabled) {
        uiMode = IConfiguration::UI_MODE_TYPE_CAR;
    } else if (IsDeskDockState(mDockState)) {
        uiMode = IConfiguration::UI_MODE_TYPE_DESK;
    }
    if (mCarModeEnabled) {
        if (mNightMode == IUiModeManager::MODE_NIGHT_AUTO) {
            UpdateComputedNightModeLocked();
            uiMode |= mComputedNightMode ? IConfiguration::UI_MODE_NIGHT_YES
                    : IConfiguration::UI_MODE_NIGHT_NO;
        } else {
            uiMode |= mNightMode << 4;
        }
    } else {
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

ECode CUiModeManagerService::SendConfigurationLocked()
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

ECode CUiModeManagerService::UpdateLocked(
    /* [in] */ Int32 enableFlags,
    /* [in] */ Int32 disableFlags)
{
    String action(NULL);
    String oldAction(NULL);
    if (mLastBroadcastState == IIntent::EXTRA_DOCK_STATE_CAR) {
        AdjustStatusBarCarModeLocked();
        oldAction = IUiModeManager::ACTION_EXIT_CAR_MODE;
    } else if (IsDeskDockState(mLastBroadcastState)) {
        oldAction = IUiModeManager::ACTION_EXIT_DESK_MODE;
    }

    AutoPtr<IUserHandle> ALL;
    CUserHandle::New(IUserHandle::USER_ALL, (IUserHandle**)&ALL);
    AutoPtr<IUserHandle> CURRENT;
    CUserHandle::New(IUserHandle::USER_CURRENT, (IUserHandle**)&CURRENT);

    if (mCarModeEnabled) {
        if (mLastBroadcastState != IIntent::EXTRA_DOCK_STATE_CAR) {
            AdjustStatusBarCarModeLocked();

            if (!oldAction.IsNull()) {
                AutoPtr<IIntent> intent;
                CIntent::New(oldAction, (IIntent **)&intent);
                mContext->SendBroadcastAsUser(intent, ALL);
            }
            mLastBroadcastState = IIntent::EXTRA_DOCK_STATE_CAR;
            action = IUiModeManager::ACTION_ENTER_CAR_MODE;
        }
    } else if (IsDeskDockState(mDockState)) {
        if (!IsDeskDockState(mLastBroadcastState)) {
            if (!oldAction.IsNull()) {
                AutoPtr<IIntent> intent;
                CIntent::New(oldAction, (IIntent **)&intent);
                mContext->SendBroadcastAsUser(intent, ALL);
            }
            mLastBroadcastState = mDockState;
            action = IUiModeManager::ACTION_ENTER_DESK_MODE;
        }
    } else {
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
        mContext->SendOrderedBroadcastAsUser(intent, CURRENT, String(NULL),
                mResultReceiver, NULL, IActivity::RESULT_OK, String(NULL), NULL);

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
            ((mCarModeEnabled && mCarModeKeepsScreenOn) ||
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

ECode CUiModeManagerService::UpdateAfterBroadcastLocked(
    /* [in] */ const String& action,
    /* [in] */ Int32 enableFlags,
    /* [in] */ Int32 disableFlags)
{
    // Launch a dock activity
    String category(NULL);
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

ECode CUiModeManagerService::SendConfigurationAndStartDreamOrDockAppLocked(
    /* [in] */ const String& category)
{
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
//TODO        AutoPtr<ISandmanHelper> sandman;
//TODO        CSandmanHelper::AcquireSingleton((ISandmanHelper**)&sandman);
//TODO        sandman->ShouldStartDockApp(mContext, homeIntent, &should);
        if (should) {
            //try {
            AutoPtr<IIActivityManager> am = ActivityManagerNative::GetDefault();
            Int32 result;
            am->StartActivityWithConfig(
                   NULL, homeIntent, String(NULL), NULL, String(NULL), 0, 0,
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
//TODO        AutoPtr<ISandmanHelper> sandman;
//TODO        CSandmanHelper::AcquireSingleton((ISandmanHelper**)&sandman);
//TODO        sandman->StartDreamWhenDockedIfAppropriate(mContext);
    }
    return NOERROR;
}

ECode CUiModeManagerService::AdjustStatusBarCarModeLocked()
{
    if (mStatusBarManager == NULL) {
        mContext->GetSystemService(IContext::STATUS_BAR_SERVICE,
            (IInterface **)&mStatusBarManager);
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
        mContext->GetSystemService(IContext::NOTIFICATION_SERVICE,
            (IInterface **)&mNotificationManager);

    }

    AutoPtr<IUserHandle> ALL;
    CUserHandle::New(IUserHandle::USER_ALL, (IUserHandle**)&ALL);
    if (mNotificationManager != NULL) {
        if (mCarModeEnabled) {
            AutoPtr<IIntent> carModeOffIntent;
            CIntent::New(mContext, ECLSID_CDisableCarModeActivity,
               (IIntent **)&carModeOffIntent);

            AutoPtr<INotification> n;
            CNotification::New((INotification**)&n);
            n->SetIcon(0x01080517);//R.drawable.stat_notify_car_mode

            n->SetDefaults(INotification::DEFAULT_LIGHTS);

            n->SetFlags(INotification::FLAG_ONGOING_EVENT);

            n->SetWhen(0);

            String str1;
            mContext->GetString(0x01040493, &str1);
            String str2;
            mContext->GetString(0x01040494, &str2);
            AutoPtr<IUserHandle> CURRENT;
            CUserHandle::New(IUserHandle::USER_CURRENT, (IUserHandle**)&CURRENT);
            AutoPtr<IPendingIntentHelper> helper;
            CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&helper);
            AutoPtr<IPendingIntent> pi;
            helper->GetActivityAsUser(mContext, 0, carModeOffIntent, 0,
                            NULL, CURRENT, (IPendingIntent**)&pi);
            AutoPtr<ICharSequence> strArg1, strArg2;
            CString::New(str1, (ICharSequence**)&strArg1);
            CString::New(str2, (ICharSequence**)&strArg2);

            n->SetLatestEventInfo(mContext, strArg1, strArg2, pi);

            mNotificationManager->NotifyAsUser(String(NULL), 0x01040493, n, ALL);
        }
        else
        {
            mNotificationManager->CancelAsUser(String(NULL), 0x01040493, ALL);
        }
    }
    return NOERROR;
}

ECode CUiModeManagerService::UpdateTwilight()
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

ECode CUiModeManagerService::UpdateComputedNightModeLocked()
{
    AutoPtr<TwilightService::TwilightState> state;
    mTwilightService->GetCurrentState((TwilightService::TwilightState **)&state);
    if (state != NULL) {
        Boolean isNight = state->IsNight();
        mComputedNightMode = isNight;
    }
    return NOERROR;
}

// ECode UiModeManagerService::Dump(
//     /* [in] */ IFileDescriptor *fd,
//     /* [in] */ IPrintWriter *pw,
//     /* [in] */ ArrayOf<String> *args)
// {
//     String permission = Elastos::Droid::Manifest::permission::DUMP;
//     Int32 checked;
//     mContext->CheckCallingOrSelfPermission(permission, &checked);
//     if (checked != PackageManager::PERMISSION_GRANTED) {
//         StringBuffer sb("Permission Denial: can't dump uimode service from from pid=");
//         sb += Binder::GetCallingPid();
//         sb += ", uid=";
//         sb += Binder::GetCallingUid();
//         pw->PrintStringln(sb.Substring(0, sb.GetLength()));
//         return NULL;
//     }

//     AutoLock(mLock);
//     pw->PrintStringln(String("Current UI Mode Service state:"));
//     pw->PrintString(String("  mDockState="));
//     pw->PrintInt32(mDockState);
//     pw->PrintString(String(" mLastBroadcastState="));
//     pw->PrintInt32(mLastBroadcastState);
//     pw->PrintString(String("  mNightMode="));
//     pw->PrintInt32(mNightMode);
//     pw->PrintString(String(" mCarModeEnabled="));
//     pw->PrintBoolean(mCarModeEnabled);
//     pw->PrintString(String(" mComputedNightMode="));
//     pw->PrintBooleanln(mComputedNightMode);
//     pw->PrintString(String("  mCurUiMode=0x"));
//     pw->PrintInt32(mCurUiMode);
//     pw->PrintString(String(" mSetUiMode=0x"));
//     pw->PrintInt32(mSetUiMode));
//     pw->PrintString(String("  mHoldingConfiguration="));
//     pw->PrintBoolean(mHoldingConfiguration);
//     pw->PrintString(String(" mSystemReady="));
//     pw->PrintBooleanln(mSystemReady);
//     pw->PrintString(String("  mTwilightService.getCurrentState()="));
//     AutoPtr<ITwilightService> ts;
//     mTwilightService->GetCurrentState((ITwilightService **)&ts);
//     pw->PrintObjectln(ts);
//     return NOERROR;
// }

}
}
}
