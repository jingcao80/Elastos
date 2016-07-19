#include "elastos/droid/systemui/keyguard/CKeyguardViewMediator.h"
#include "elastos/droid/systemui/statusbar/phone/StatusBarKeyguardViewManager.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Telephony.h"
#include "elastos/droid/app/ActivityManagerNative.h"
#include <elastos/droid/os/Looper.h>
#include <elastos/droid/os/SystemClock.h>
#include <elastos/droid/R.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/Math.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/Thread.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::App::CActivityManagerHelper;
using Elastos::Droid::App::CPendingIntentHelper;
using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IActivityManagerHelper;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::App::IPendingIntentHelper;
using Elastos::Droid::App::Admin::IDevicePolicyManager;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Internal::Telephony::IccCardConstantsState;
using Elastos::Droid::Internal::Telephony::IccCardConstantsState_ABSENT;
using Elastos::Droid::Internal::Telephony::IccCardConstantsState_NOT_READY;
using Elastos::Droid::Internal::Telephony::IccCardConstantsState_PERM_DISABLED;
using Elastos::Droid::Internal::Telephony::IccCardConstantsState_PIN_REQUIRED;
using Elastos::Droid::Internal::Telephony::IccCardConstantsState_PUK_REQUIRED;
using Elastos::Droid::Internal::Telephony::IccCardConstantsState_READY;
using Elastos::Droid::Internal::Widget::CLockPatternUtils;
using Elastos::Droid::SystemUI::Keyguard::EIID_IKeyguardUpdateMonitorCallback;
using Elastos::Droid::SystemUI::Keyguard::EIID_IViewMediatorCallback;
using Elastos::Droid::Media::CSoundPool;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Os::CUserManagerHelper;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Os::IUserManagerHelper;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Droid::Provider::CSettingsSecure;
using Elastos::Droid::Provider::CSettingsSystem;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::SystemUI::StatusBar::Phone::StatusBarKeyguardViewManager;
using Elastos::Droid::SystemUI::Keyguard::EIID_IKeyguardViewMediator;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::View::CWindowManagerGlobalHelper;
using Elastos::Droid::View::IWindowManagerGlobalHelper;
using Elastos::Droid::View::IWindowManagerPolicy;
using Elastos::Droid::View::Animation::CAnimationUtils;
using Elastos::Droid::View::Animation::IAnimationUtils;
using Elastos::Droid::R;
using Elastos::Core::AutoLock;
using Elastos::Core::Math;
using Elastos::Core::StringUtils;
using Elastos::Core::Thread;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

//=================================================
// CKeyguardViewMediator::MyKeyguardUpdateMonitorCallback
//=================================================
CKeyguardViewMediator::MyKeyguardUpdateMonitorCallback::MyKeyguardUpdateMonitorCallback(
    /* [in] */ CKeyguardViewMediator* host)
    : mHost(host)
{}

ECode CKeyguardViewMediator::MyKeyguardUpdateMonitorCallback::OnUserSwitching(
    /* [in] */ Int32 userId)
{
    // Note that the mLockPatternUtils user has already been updated from setCurrentUser.
    // We need to force a reset of the views, since lockNow (called by
    // ActivityManagerService) will not reconstruct the keyguard if it is already showing.
    {
        AutoLock syncLock(mHost);
        mHost->mSwitchingUser = TRUE;
        mHost->mKeyguardDonePending = FALSE;
        mHost->ResetStateLocked();
        mHost->AdjustStatusBarLocked();
        // When we switch users we want to bring the new user to the biometric unlock even
        // if the current user has gone to the backup.
#if 0 //TODO
        AutoPtr<IKeyguardUpdateMonitorHelper> kumh;
        CKeyguardUpdateMonitorHelper::AcquireSingleton((IKeyguardUpdateMonitorHelper**)&kumh);
        AutoPtr<IKeyguardUpdateMonitor> kum;
        kumh->GetInstance(mHost->mContext, (IKeyguardUpdateMonitor**)&kum);
        kum->SetAlternateUnlockEnabled(TRUE);
#endif
    }
    return NOERROR;
}

ECode CKeyguardViewMediator::MyKeyguardUpdateMonitorCallback::OnUserSwitchComplete(
    /* [in] */ Int32 userId)
{
    mHost->mSwitchingUser = FALSE;
    if (userId != IUserHandle::USER_OWNER) {
        AutoPtr<IUserManagerHelper> umh;
        CUserManagerHelper::AcquireSingleton((IUserManagerHelper**)&umh);
        AutoPtr<IUserManager> um;
        umh->Get(mHost->mContext, (IUserManager**)&um);
        AutoPtr<IUserInfo> info;
        um->GetUserInfo(userId, (IUserInfo**)&info);
        Boolean isGuest;
        info->IsGuest(&isGuest);
        if (info != NULL && isGuest) {
            // If we just switched to a guest, try to dismiss keyguard.
            mHost->Dismiss();
        }
    }
    return NOERROR;
}

ECode CKeyguardViewMediator::MyKeyguardUpdateMonitorCallback::OnUserRemoved(
    /* [in] */ Int32 userId)
{
    mHost->mLockPatternUtils->RemoveUser(userId);
#if 0 //TODO
    AutoPtr<IMultiUserAvatarCacheHelper> muach;
    CMultiUserAvatarCacheHelper::AcquireSingleton((IMultiUserAvatarCacheHelper**)&muach);
    AutoPtr<IMultiUserAvatarCache> muac;
    muach->GetInstance((IMultiUserAvatarCache**)&muac);
    muac->Clear(userId);
#endif
    return NOERROR;
}

ECode CKeyguardViewMediator::MyKeyguardUpdateMonitorCallback::OnUserInfoChanged(
    /* [in] */ Int32 userId)
{
#if 0 //TODO
    AutoPtr<IMultiUserAvatarCacheHelper> muach;
    CMultiUserAvatarCacheHelper::AcquireSingleton((IMultiUserAvatarCacheHelper**)&muach);
    AutoPtr<IMultiUserAvatarCache> muac;
    muach->GetInstance((IMultiUserAvatarCache**)&muac);
    muac->Clear(userId);
#endif
    return NOERROR;
}

ECode CKeyguardViewMediator::MyKeyguardUpdateMonitorCallback::OnPhoneStateChanged(
    /* [in] */ Int32 phoneState)
{
    {
        AutoLock syncLock(mHost);
        if (ITelephonyManager::CALL_STATE_IDLE == phoneState  // call ending
                && !mHost->mScreenOn                           // screen off
                && mHost->mExternallyEnabled) {                // not disabled by any app

            // note: this is a way to gracefully reenable the keyguard when the call
            // ends and the screen is off without always reenabling the keyguard
            // each time the screen turns off while in call (and having an occasional ugly
            // flicker while turning back on the screen and disabling the keyguard again).
            if (DEBUG) {
                Logger::D(TAG, "screen is off and call ended, let's make sure the %s",
                    "keyguard is showing");
            }
            mHost->DoKeyguardLocked(NULL);
        }
    }
    return NOERROR;
}

ECode CKeyguardViewMediator::MyKeyguardUpdateMonitorCallback::OnClockVisibilityChanged()
{
    mHost->AdjustStatusBarLocked();
    return NOERROR;
}

ECode CKeyguardViewMediator::MyKeyguardUpdateMonitorCallback::OnDeviceProvisioned()
{
    mHost->SendUserPresentBroadcast();
    return NOERROR;
}

ECode CKeyguardViewMediator::MyKeyguardUpdateMonitorCallback::OnSimStateChanged(
    /* [in] */ IccCardConstantsState simState)
{
    if (DEBUG) Logger::D(TAG, "onSimStateChanged: %d", simState);

    switch (simState) {
        case IccCardConstantsState_NOT_READY:
        case IccCardConstantsState_ABSENT:
            // only force lock screen in case of missing sim if user hasn't
            // gone through setup wizard
            {
                AutoLock syncLock(this);
                Boolean isDeviceProvisioned = FALSE;
                //TODO
                // mHost->mUpdateMonitor->IsDeviceProvisioned(&isDeviceProvisioned);
                if (!isDeviceProvisioned) {
                    Boolean isShowing;
                    mHost->IsShowing(&isShowing);
                    if (!isShowing) {
                        if (DEBUG) Logger::D(TAG, "ICC_ABSENT isn't showing,%s",
                            " we need to show the keyguard since the device isn't provisioned yet.");
                        mHost->DoKeyguardLocked(NULL);
                    }
                    else {
                        mHost->ResetStateLocked();
                    }
                }
            }
            break;
        case IccCardConstantsState_PIN_REQUIRED:
        case IccCardConstantsState_PUK_REQUIRED:
            {
                AutoLock syncLock(this);
                Boolean isShowing;
                mHost->IsShowing(&isShowing);
                if (!isShowing) {
                    if (DEBUG) Logger::D(TAG, "INTENT_VALUE_ICC_LOCKED and keygaurd isn't %s",
                        "showing; need to show keyguard so user can enter sim pin");
                    mHost->DoKeyguardLocked(NULL);
                }
                else {
                    mHost->ResetStateLocked();
                }
            }
            break;
        case IccCardConstantsState_PERM_DISABLED:
            {
                AutoLock syncLock(this);
                Boolean isShowing;
                mHost->IsShowing(&isShowing);
                if (!isShowing) {
                    if (DEBUG) Logger::D(TAG, "PERM_DISABLED and %s", "keygaurd isn't showing.");
                    mHost->DoKeyguardLocked(NULL);
                }
                else {
                    if (DEBUG) Logger::D(TAG, "PERM_DISABLED, resetStateLocked to%s",
                        "show permanently disabled message in lockscreen.");
                    mHost->ResetStateLocked();
                }
            }
            break;
        case IccCardConstantsState_READY:
            {
                AutoLock syncLock(this);
                Boolean isShowing;
                mHost->IsShowing(&isShowing);
                if (!isShowing) {
                    mHost->ResetStateLocked();
                }
            }
            break;
    }
    return NOERROR;
}

ECode CKeyguardViewMediator::MyKeyguardUpdateMonitorCallback::OnFingerprintRecognized(
    /* [in] */ Int32 userId)
{
    Boolean isBouncerShowing;
    mHost->mStatusBarKeyguardViewManager->IsBouncerShowing(&isBouncerShowing);
    if (isBouncerShowing) {
        mHost->mViewMediatorCallback->KeyguardDone(TRUE);
    }
    return NOERROR;
}

//=================================================
// CKeyguardViewMediator::MyViewMediatorCallback
//=================================================

CAR_INTERFACE_IMPL(CKeyguardViewMediator::MyViewMediatorCallback, Object, IViewMediatorCallback)

CKeyguardViewMediator::MyViewMediatorCallback::MyViewMediatorCallback(
    /* [in] */ CKeyguardViewMediator* host)
    : mHost(host)
{}

ECode CKeyguardViewMediator::MyViewMediatorCallback::UserActivity()
{
    mHost->UserActivity();
    return NOERROR;
}

ECode CKeyguardViewMediator::MyViewMediatorCallback::KeyguardDone(
    /* [in] */ Boolean authenticated)
{
    mHost->KeyguardDone(authenticated, TRUE);
    return NOERROR;
}

ECode CKeyguardViewMediator::MyViewMediatorCallback::KeyguardDoneDrawing()
{
    Boolean result;
    return mHost->mHandler->SendEmptyMessage(KEYGUARD_DONE_DRAWING, &result);
}

ECode CKeyguardViewMediator::MyViewMediatorCallback::SetNeedsInput(
    /* [in] */ Boolean needsInput)
{
    return mHost->mStatusBarKeyguardViewManager->SetNeedsInput(needsInput);
}

ECode CKeyguardViewMediator::MyViewMediatorCallback::OnUserActivityTimeoutChanged()
{
    return mHost->mStatusBarKeyguardViewManager->UpdateUserActivityTimeout();
}

ECode CKeyguardViewMediator::MyViewMediatorCallback::KeyguardDonePending()
{
    mHost->mKeyguardDonePending = TRUE;
    mHost->mHideAnimationRun = TRUE;
    return mHost->mStatusBarKeyguardViewManager->StartPreHideAnimation(NULL /* finishRunnable */);
}

ECode CKeyguardViewMediator::MyViewMediatorCallback::KeyguardGone()
{
    //TODO
    // return mHost->mKeyguardDisplayManager->Hide();
    // assert(0 && "TODO");
    return NOERROR;
}

ECode CKeyguardViewMediator::MyViewMediatorCallback::ReadyForKeyguardDone()
{
    if (mHost->mKeyguardDonePending) {
        // Somebody has called keyguardDonePending before, which means that we are
        // authenticated
        mHost->KeyguardDone(TRUE /* authenticated */, TRUE /* wakeUp */);
    }
    return NOERROR;
}

ECode CKeyguardViewMediator::MyViewMediatorCallback::PlayTrustedSound()
{
    mHost->PlayTrustedSound();
    return NOERROR;
}

//=================================================
// CKeyguardViewMediator::MyBroadcastReceiver
//=================================================

CKeyguardViewMediator::MyBroadcastReceiver::MyBroadcastReceiver(
    /* [in] */ CKeyguardViewMediator* host)
    : mHost(host)
{}

ECode CKeyguardViewMediator::MyBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (DELAYED_KEYGUARD_ACTION.Equals(action)) {
        Int32 value;
        intent->GetInt32Extra(String("seq"), 0, &value);
        const Int32 sequence = value;
        if (DEBUG) {
            Logger::D(TAG, "received DELAYED_KEYGUARD_ACTION with seq = %d, mDelayedShowingSequence = %d",
            sequence, mHost->mDelayedShowingSequence);
        }

        {
            AutoLock syncLock(mHost);
            if (mHost->mDelayedShowingSequence == sequence) {
                // Don't play lockscreen SFX if the screen went off due to timeout.
                mHost->mSuppressNextLockSound = TRUE;
                mHost->DoKeyguardLocked(NULL);
            }
        }
    }
    return NOERROR;
}

//=================================================
// CKeyguardViewMediator::StartKeyguardExitAnimParams
//=================================================

CKeyguardViewMediator::StartKeyguardExitAnimParams::StartKeyguardExitAnimParams(
    /* [in] */ Int64 startTime,
    /* [in] */ Int64 fadeoutDuration)
    : mStartTime(startTime)
    , mFadeoutDuration(fadeoutDuration)
{}

//=================================================
// CKeyguardViewMediator::MyHandler
//=================================================

CKeyguardViewMediator::MyHandler::MyHandler(
    /* [in] */ CKeyguardViewMediator* host)
    : Handler(Looper::GetMyLooper(), NULL, TRUE)
    , mHost(host)
{}

ECode CKeyguardViewMediator::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case SHOW: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            AutoPtr<IBundle> b = IBundle::Probe(obj);
            mHost->HandleShow(b);
            break;
        }
        case HIDE:
            mHost->HandleHide();
            break;
        case RESET:
            mHost->HandleReset();
            break;
        case VERIFY_UNLOCK:
            mHost->HandleVerifyUnlock();
            break;
        case NOTIFY_SCREEN_OFF:
            mHost->HandleNotifyScreenOff();
            break;
        case NOTIFY_SCREEN_ON: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            AutoPtr<IIKeyguardShowCallback> cb = IIKeyguardShowCallback::Probe(obj);
            mHost->HandleNotifyScreenOn(cb);
            break;
        }
        case KEYGUARD_DONE: {
            Int32 arg1, arg2;
            msg->GetArg1(&arg1);
            msg->GetArg2(&arg2);
            mHost->HandleKeyguardDone(arg1 != 0, arg2 != 0);
            break;
        }
        case KEYGUARD_DONE_DRAWING:
            mHost->HandleKeyguardDoneDrawing();
            break;
        case KEYGUARD_DONE_AUTHENTICATING:
            mHost->KeyguardDone(TRUE, TRUE);
            break;
        case SET_OCCLUDED: {
            Int32 arg1;
            msg->GetArg1(&arg1);
            mHost->HandleSetOccluded(arg1 != 0);
            break;
        }
        case KEYGUARD_TIMEOUT:
            {    AutoLock syncLock(mHost);
                AutoPtr<IInterface> obj;
                msg->GetObj((IInterface**)&obj);
                AutoPtr<IBundle> b = IBundle::Probe(obj);
                mHost->DoKeyguardLocked(b);
            }
            break;
        case DISMISS:
            mHost->HandleDismiss();
            break;
        case START_KEYGUARD_EXIT_ANIM: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            AutoPtr<StartKeyguardExitAnimParams> params = (StartKeyguardExitAnimParams*)(IObject::Probe(obj));
            mHost->HandleStartKeyguardExitAnimation(params->mStartTime, params->mFadeoutDuration);
            break;
        }
        case ON_ACTIVITY_DRAWN:
            mHost->HandleOnActivityDrawn();
            break;
    }
    return NOERROR;
}

//=================================================
// CKeyguardViewMediator::MyRunnable
//=================================================

CKeyguardViewMediator::MyRunnable::MyRunnable(
    /* [in] */ CKeyguardViewMediator* host)
    : mHost(host)
{}

ECode CKeyguardViewMediator::MyRunnable::Run()
{
    // Don't actually hide the Keyguard at the moment, wait for window
    // manager until it tells us it's safe to do so with
    // startKeyguardExitAnimation.
    Boolean b1, b2;
    mHost->mStatusBarKeyguardViewManager->ShouldDisableWindowAnimationsForUnlock(&b1);
    mHost->mStatusBarKeyguardViewManager->IsGoingToNotificationShade(&b2);
    ECode ec = mHost->mWM->KeyguardGoingAway(b1, b2, FALSE);
    if (FAILED(ec)) {
        Logger::E(TAG, "Error while calling WindowManager%08x", ec);
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

//=================================================
// CKeyguardViewMediator
//=================================================

CAR_INTERFACE_IMPL(CKeyguardViewMediator, SystemUI, IKeyguardViewMediator)

CAR_OBJECT_IMPL(CKeyguardViewMediator)

const Int32 CKeyguardViewMediator::KEYGUARD_DISPLAY_TIMEOUT_DELAY_DEFAULT;
const Boolean CKeyguardViewMediator::DEBUG = TRUE;
const Boolean CKeyguardViewMediator::DBG_WAKE = TRUE;

const String CKeyguardViewMediator::TAG("CKeyguardViewMediator");
const String CKeyguardViewMediator::DELAYED_KEYGUARD_ACTION("com.android.internal.policy.impl.PhoneWindowManager.DELAYED_KEYGUARD");

const Int32 CKeyguardViewMediator::SHOW;
const Int32 CKeyguardViewMediator::HIDE;
const Int32 CKeyguardViewMediator::RESET;
const Int32 CKeyguardViewMediator::VERIFY_UNLOCK;
const Int32 CKeyguardViewMediator::NOTIFY_SCREEN_OFF;
const Int32 CKeyguardViewMediator::NOTIFY_SCREEN_ON;
const Int32 CKeyguardViewMediator::KEYGUARD_DONE;
const Int32 CKeyguardViewMediator::KEYGUARD_DONE_DRAWING;
const Int32 CKeyguardViewMediator::KEYGUARD_DONE_AUTHENTICATING;
const Int32 CKeyguardViewMediator::SET_OCCLUDED;
const Int32 CKeyguardViewMediator::KEYGUARD_TIMEOUT;
const Int32 CKeyguardViewMediator::DISMISS;
const Int32 CKeyguardViewMediator::START_KEYGUARD_EXIT_ANIM;
const Int32 CKeyguardViewMediator::ON_ACTIVITY_DRAWN;

const Int32 CKeyguardViewMediator::AWAKE_INTERVAL_DEFAULT_MS;
const Int32 CKeyguardViewMediator::KEYGUARD_LOCK_AFTER_DELAY_DEFAULT;
const Int32 CKeyguardViewMediator::KEYGUARD_DONE_DRAWING_TIMEOUT_MS;

const String CKeyguardViewMediator::KEYGUARD_ANALYTICS_SETTING("keyguard_analytics");

static AutoPtr<IIntent> InitUPI()
{
    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_USER_PRESENT, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_RECEIVER_REPLACE_PENDING
        | IIntent::FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
    return intent;
}

AutoPtr<IIntent> CKeyguardViewMediator::USER_PRESENT_INTENT = InitUPI();

CKeyguardViewMediator::CKeyguardViewMediator()
    : mMasterStreamType(0)
    , mSwitchingUser(FALSE)
    , mSystemReady(FALSE)
    , mBootCompleted(FALSE)
    , mBootSendUserPresent(FALSE)
    , mSuppressNextLockSound(TRUE)
    , mExternallyEnabled(FALSE) // TRUE // TODO restore. zhaohui
    , mNeedToReshowWhenReenabled(FALSE)
    , mShowing(FALSE)
    , mOccluded(FALSE)
    , mDelayedShowingSequence(0)
    , mScreenOn(FALSE)
    , mPhoneState(ITelephonyManager::EXTRA_STATE_IDLE)
    , mHiding(FALSE)
    , mWaitingUntilKeyguardVisible(FALSE)
    , mKeyguardDonePending(FALSE)
    , mHideAnimationRun(FALSE)
    , mLockSoundId(0)
    , mUnlockSoundId(0)
    , mTrustedSoundId(0)
    , mLockSoundStreamId(0)
    , mLockSoundVolume(0.0f)
{
    Logger::E(TAG, "TODO restore mExternallyEnabled to TRUE when keyguard ready.");
}

ECode CKeyguardViewMediator::constructor()
{
    mUpdateCallback = new MyKeyguardUpdateMonitorCallback(this);
    mViewMediatorCallback = new MyViewMediatorCallback(this);
    mBroadcastReceiver = new MyBroadcastReceiver(this);
    mHandler = new MyHandler(this);
    mKeyguardGoingAwayRunnable = new MyRunnable(this);
    return NOERROR;
}

ECode CKeyguardViewMediator::UserActivity()
{
    mPM->UserActivity(SystemClock::GetUptimeMillis(), FALSE);
    return NOERROR;
}

void CKeyguardViewMediator::Setup()
{
    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&obj);
    mPM = IPowerManager::Probe(obj);

    AutoPtr<IWindowManagerGlobalHelper> wmgh;
    CWindowManagerGlobalHelper::AcquireSingleton((IWindowManagerGlobalHelper**)&wmgh);
    wmgh->GetWindowManagerService((IIWindowManager**)&mWM);

    obj = NULL;
    mContext->GetSystemService(IContext::USER_SERVICE, (IInterface**)&obj);
    mUserManager = IUserManager::Probe(obj);
    mPM->NewWakeLock(IPowerManager::PARTIAL_WAKE_LOCK, String("show keyguard"),
        (IPowerManagerWakeLock**)&mShowKeyguardWakeLock);
    mShowKeyguardWakeLock->SetReferenceCounted(FALSE);

    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New(DELAYED_KEYGUARD_ACTION, (IIntentFilter**)&filter);
    AutoPtr<IIntent> intent;
    mContext->RegisterReceiver(mBroadcastReceiver, filter, (IIntent**)&intent);

    //TODO
    // CKeyguardDisplayManager::New(mContext, (IKeyguardDisplayManager**)&mKeyguardDisplayManager);

    obj = NULL;
    mContext->GetSystemService(IContext::ALARM_SERVICE, (IInterface**)&obj);
    mAlarmManager = IAlarmManager::Probe(obj);

    //TODO
    // AutoPtr<IKeyguardUpdateMonitorHelper> kumh;
    // CKeyguardUpdateMonitorHelper::AcquireSingleton((IKeyguardUpdateMonitorHelper**)&kumh);
    // kumh->GetInstance(mContext, (IKeyguardUpdateMonitor**)&mUpdateMonitor);

    CLockPatternUtils::New(mContext, (ILockPatternUtils**)&mLockPatternUtils);

    AutoPtr<IActivityManagerHelper> amh;
    CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&amh);
    Int32 currentUser;
    amh->GetCurrentUser(&currentUser);
    mLockPatternUtils->SetCurrentUser(currentUser);

    // Assume keyguard is showing (unless it's disabled) until we know for sure...
    //TODO
    // Boolean isDeviceProvisioned, isSecure, isLockScreenDisabled;
    // mUpdateMonitor->IsDeviceProvisioned(&isDeviceProvisioned);
    // mLockPatternUtils->IsSecure(&isSecure);
    // mLockPatternUtils->IsLockScreenDisabled(&isLockScreenDisabled);
    // mShowing = isDeviceProvisioned || isSecure && (!isLockScreenDisabled);

    mStatusBarKeyguardViewManager = new StatusBarKeyguardViewManager(mContext,
            mViewMediatorCallback, mLockPatternUtils);
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    const AutoPtr<IContentResolver> cr = resolver;

    mPM->IsScreenOn(&mScreenOn);

    CSoundPool::New(1, IAudioManager::STREAM_SYSTEM, 0, (ISoundPool**)&mLockSounds);
    AutoPtr<ISettingsGlobal> sg;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&sg);
    String soundPath;
    sg->GetString(cr, ISettingsGlobal::LOCK_SOUND, &soundPath);
    if (!soundPath.IsNull()) {
        mLockSounds->Load(soundPath, 1, &mLockSoundId);
    }
    if (soundPath.IsNull() || mLockSoundId == 0) {
        Logger::W(TAG, "failed to load lock sound from %s", soundPath.string());
    }
    sg->GetString(cr, ISettingsGlobal::UNLOCK_SOUND, &soundPath);
    if (!soundPath.IsNull()) {
        mLockSounds->Load(soundPath, 1, &mUnlockSoundId);
    }
    if (soundPath.IsNull() || mUnlockSoundId == 0) {
        Logger::W(TAG, "failed to load unlock sound from %s", soundPath.string());
    }
    sg->GetString(cr, ISettingsGlobal::TRUSTED_SOUND, &soundPath);
    if (!soundPath.IsNull()) {
        mLockSounds->Load(soundPath, 1, &mTrustedSoundId);
    }
    if (soundPath.IsNull() || mTrustedSoundId == 0) {
        Logger::W(TAG, "failed to load trusted sound from %s", soundPath.string());
    }

    AutoPtr<IResources> resources;
    mContext->GetResources((IResources**)&resources);
    Int32 lockSoundDefaultAttenuation;
    resources->GetInteger(R::integer::config_lockSoundVolumeDb, &lockSoundDefaultAttenuation);
    mLockSoundVolume = (Float)Elastos::Core::Math::Pow(10, (Float)lockSoundDefaultAttenuation/20);

    AutoPtr<IAnimationUtils> au;
    CAnimationUtils::AcquireSingleton((IAnimationUtils**)&au);
    au->LoadAnimation(mContext, R::anim::lock_screen_behind_enter, (IAnimation**)&mHideAnimation);
}

ECode CKeyguardViewMediator::Start()
{
    Setup();
    SystemUI::PutComponent(String("EIID_IKeyguardViewMediator"), TO_IINTERFACE(this));
    return NOERROR;
}

ECode CKeyguardViewMediator::OnSystemReady()
{
    AutoPtr<IInterface> obj;
    Logger::D(TAG, "TODO [OnSystemReady]: Not implement SEARCH_SERVICE.");
    // mContext->GetSystemService(IContext::SEARCH_SERVICE, (IInterface**)&obj);
    mSearchManager = ISearchManager::Probe(obj);
    {
        AutoLock syncLock(this);
        if (DEBUG) Logger::D(TAG, "onSystemReady");
        mSystemReady = TRUE;
        //TODO
        // mUpdateMonitor->RegisterCallback(mUpdateCallback);

        // Suppress biometric unlock right after boot until things have settled if it is the
        // selected security method, otherwise unsuppress it.  It must be unsuppressed if it is
        // not the selected security method for the following reason:  if the user starts
        // without a screen lock selected, the biometric unlock would be suppressed the first
        // time they try to use it.
        //
        // Note that the biometric unlock will still not show if it is not the selected method.
        // Calling setAlternateUnlockEnabled(true) simply says don't suppress it if it is the
        // selected method.
        Boolean ubw, bwi;
        mLockPatternUtils->UsingBiometricWeak(&ubw);
        mLockPatternUtils->IsBiometricWeakInstalled(&bwi);
        if (ubw && bwi) {
            if (DEBUG) Logger::D(TAG, "suppressing biometric unlock during boot");
            //TODO
            // mUpdateMonitor->SetAlternateUnlockEnabled(FALSE);
        } else {
            //TODO
            // mUpdateMonitor->SetAlternateUnlockEnabled(TRUE);
        }

        DoKeyguardLocked(NULL);
    }
    // Most services aren't available until the system reaches the ready state, so we
    // send it here when the device first boots.
    MaybeSendUserPresentBroadcast();
    return NOERROR;
}

ECode CKeyguardViewMediator::OnScreenTurnedOff(
    /* [in] */ Int32 why)
{
    {
        AutoLock syncLock(this);
        mScreenOn = FALSE;
        if (DEBUG) Logger::D(TAG, "onScreenTurnedOff(%d)", why);

        mKeyguardDonePending = FALSE;
        mHideAnimationRun = FALSE;

        // Lock immediately based on setting if secure (user has a pin/pattern/password).
        // This also "locks" the device when not secure to provide easy access to the
        // camera while preventing unwanted input.
        Boolean b1,b2;
        mLockPatternUtils->GetPowerButtonInstantlyLocks(&b1);
        mLockPatternUtils->IsSecure(&b2);
        const Boolean lockImmediately = b1 || !b2;

        NotifyScreenOffLocked();

        if (mExitSecureCallback != NULL) {
            if (DEBUG) Logger::D(TAG, "pending exit secure callback cancelled");

            ECode ec = mExitSecureCallback->OnKeyguardExitResult(FALSE);
            if (FAILED(ec)) {
                Logger::W(TAG, "Failed to call onKeyguardExitResult(false)%08x", ec);
                return E_REMOTE_EXCEPTION;
            }
            mExitSecureCallback = NULL;
            if (!mExternallyEnabled) {
                HideLocked();
            }
        }
        else if (mShowing) {
            ResetStateLocked();
        }
        else if (why == IWindowManagerPolicy::OFF_BECAUSE_OF_TIMEOUT
               || (why == IWindowManagerPolicy::OFF_BECAUSE_OF_USER && !lockImmediately)) {
            DoKeyguardLaterLocked();
        }
        else {
            DoKeyguardLocked(NULL);
        }
    }

    //TODO
    // AutoPtr<IKeyguardUpdateMonitorHelper> kumh;
    // CKeyguardUpdateMonitorHelper::AcquireSingleton((IKeyguardUpdateMonitorHelper**)&kumh);
    // AutoPtr<IKeyguardUpdateMonitor> kum;
    // kumh->GetInstance(mContext, (IKeyguardUpdateMonitor**)&kum);
    // kum->DispatchScreenTurndOff(why);
    return NOERROR;
}

void CKeyguardViewMediator::DoKeyguardLaterLocked()
{
    // if the screen turned off because of timeout or the user hit the power button
    // and we don't need to lock immediately, set an alarm
    // to enable it a little bit later (i.e, give the user a chance
    // to turn the screen back on within a certain window without
    // having to unlock the screen)
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    const AutoPtr<IContentResolver> cr = resolver;

    // From DisplaySettings
    AutoPtr<ISettingsSystem> ss;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&ss);
    Int32 v;
    ss->GetInt32(cr, ISettingsSystem::SCREEN_OFF_TIMEOUT, KEYGUARD_DISPLAY_TIMEOUT_DELAY_DEFAULT, &v);
    Int64 displayTimeout = v;

    // From SecuritySettings
    AutoPtr<ISettingsSecure> ss2;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&ss2);
    Int32 v2;
    ss2->GetInt32(cr, ISettingsSecure::LOCK_SCREEN_LOCK_AFTER_TIMEOUT,
        KEYGUARD_LOCK_AFTER_DELAY_DEFAULT, &v2);
    const Int64 lockAfterTimeout = v2;

    // From DevicePolicyAdmin
    Int32 cu;
    mLockPatternUtils->GetCurrentUser(&cu);
    AutoPtr<IDevicePolicyManager> dpm;
    mLockPatternUtils->GetDevicePolicyManager((IDevicePolicyManager**)&dpm);
    Int64 pt;
    dpm->GetMaximumTimeToLock(NULL, cu, &pt);
    const Int64 policyTimeout = pt;

    Int64 timeout;
    if (policyTimeout > 0) {
        // policy in effect. Make sure we don't go beyond policy limit.
        displayTimeout = Elastos::Core::Math::Max(displayTimeout, 0L); // ignore negative values
        timeout = Elastos::Core::Math::Min(policyTimeout - displayTimeout, lockAfterTimeout);
    }
    else {
        timeout = lockAfterTimeout;
    }

    if (timeout <= 0) {
        // Lock now
        mSuppressNextLockSound = TRUE;
        DoKeyguardLocked(NULL);
    }
    else {
        // Lock in the future
        Int32 when = SystemClock::GetElapsedRealtime() + timeout;
        AutoPtr<IIntent> intent;
        CIntent::New(DELAYED_KEYGUARD_ACTION, (IIntent**)&intent);
        intent->PutExtra(String("seq"), mDelayedShowingSequence);
        AutoPtr<IPendingIntentHelper> pih;
        CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&pih);
        AutoPtr<IPendingIntent> sender;
        pih->GetBroadcast(mContext, 0, intent, IPendingIntent::FLAG_CANCEL_CURRENT,
            (IPendingIntent**)&sender);
        mAlarmManager->Set(IAlarmManager::ELAPSED_REALTIME_WAKEUP, when, sender);
        if (DEBUG) Logger::D(TAG, "setting alarm to turn off keyguard, seq = %d", mDelayedShowingSequence);
    }
}

void CKeyguardViewMediator::CancelDoKeyguardLaterLocked()
{
    mDelayedShowingSequence++;
}

ECode CKeyguardViewMediator::OnScreenTurnedOn(
    /* [in] */ IIKeyguardShowCallback* callback)
{
    {
        AutoLock syncLock(this);
        mScreenOn = TRUE;
        CancelDoKeyguardLaterLocked();
        if (DEBUG) Logger::D(TAG, "onScreenTurnedOn, seq = %d", mDelayedShowingSequence);
        if (callback != NULL) {
            NotifyScreenOnLocked(callback);
        }
    }

    Logger::D(TAG, "TODO [OnScreenTurnedOn] : Not implement the app Keyguard");
    //TODO
    // AutoPtr<IKeyguardUpdateMonitorHelper> kumh;
    // CKeyguardUpdateMonitorHelper::AcquireSingleton((IKeyguardUpdateMonitorHelper**)&kumh);
    // AutoPtr<IKeyguardUpdateMonitor> kum;
    // kumh->GetInstance(mContext, (IKeyguardUpdateMonitor**)&kum);
    // kum->DispatchScreenTurnedOn();
    MaybeSendUserPresentBroadcast();
    return NOERROR;
}

void CKeyguardViewMediator::MaybeSendUserPresentBroadcast()
{
    Boolean result;
    mLockPatternUtils->IsLockScreenDisabled(&result);
    if (mSystemReady && result) {
        // Lock screen is disabled because the user has set the preference to "None".
        // In this case, send out ACTION_USER_PRESENT here instead of in
        // handleKeyguardDone()
        SendUserPresentBroadcast();
    }
}

ECode CKeyguardViewMediator::OnDreamingStarted()
{
    {
        AutoLock syncLock(this);
        Boolean isSecure;
        mLockPatternUtils->IsSecure(&isSecure);
        if (mScreenOn && isSecure) {
            DoKeyguardLaterLocked();
        }
    }
    return NOERROR;
}

ECode CKeyguardViewMediator::OnDreamingStopped()
{
    {
        AutoLock syncLock(this);
        if (mScreenOn) {
            CancelDoKeyguardLaterLocked();
        }
    }
    return NOERROR;
}

ECode CKeyguardViewMediator::SetKeyguardEnabled(
    /* [in] */ Boolean enabled)
{
    {
        AutoLock syncLock(this);
        if (DEBUG) Logger::D(TAG, "setKeyguardEnabled(%s)", enabled ? "TRUE" : "FALSE");

        mExternallyEnabled = enabled;

        if (!enabled && mShowing) {
            if (mExitSecureCallback != NULL) {
                if (DEBUG) Logger::D(TAG, "in process of verifyUnlock request, ignoring");
                // we're in the process of handling a request to verify the user
                // can get past the keyguard. ignore extraneous requests to disable / reenable
                return E_NULL_POINTER_EXCEPTION;
            }

            // hiding keyguard that is showing, remember to reshow later
            if (DEBUG) Logger::D(TAG, "remembering to reshow, hiding keyguard, %s",
                "disabling status bar expansion");
            mNeedToReshowWhenReenabled = TRUE;
            HideLocked();
        }
        else if (enabled && mNeedToReshowWhenReenabled) {
            // reenabled after previously hidden, reshow
            if (DEBUG) Logger::D(TAG, "previously hidden, reshowing, reenabling %s",
                "status bar expansion");
            mNeedToReshowWhenReenabled = FALSE;

            if (mExitSecureCallback != NULL) {
                if (DEBUG) Logger::D(TAG, "onKeyguardExitResult(false), resetting");

                ECode ec = mExitSecureCallback->OnKeyguardExitResult(FALSE);
                if (FAILED(ec)) {
                    Logger::W(TAG, "Failed to call onKeyguardExitResult(false)%08x", ec);
                    return E_REMOTE_EXCEPTION;
                }
                mExitSecureCallback = NULL;
                ResetStateLocked();
            }
            else {
                ShowLocked(NULL);

                // block until we know the keygaurd is done drawing (and post a message
                // to unblock us after a timeout so we don't risk blocking too long
                // and causing an ANR).
                mWaitingUntilKeyguardVisible = TRUE;
                Boolean b3;
                mHandler->SendEmptyMessageDelayed(KEYGUARD_DONE_DRAWING, KEYGUARD_DONE_DRAWING_TIMEOUT_MS, &b3);
                if (DEBUG) Logger::D(TAG, "waiting until mWaitingUntilKeyguardVisible is false");
                while (mWaitingUntilKeyguardVisible) {
                    // try {
                    Wait();
                    // } catch (InterruptedException e) {
                    AutoPtr<IThread> thread = Thread::GetCurrentThread();
                    thread->Interrupt();
                    // }
                }
                if (DEBUG) Logger::D(TAG, "done waiting for mWaitingUntilKeyguardVisible");
            }
        }
    }
    return NOERROR;
}

ECode CKeyguardViewMediator::VerifyUnlock(
    /* [in] */ IIKeyguardExitCallback* callback)
{
    {
        AutoLock syncLock(this);
        Boolean isDeviceProvisioned = TRUE;
        // mUpdateMonitor->IsDeviceProvisioned(&isDeviceProvisioned);
        if (!isDeviceProvisioned) {
            // don't allow this api when the device isn't provisioned
            if (DEBUG) Logger::D(TAG, "ignoring because device isn't provisioned");
            ECode ec = callback->OnKeyguardExitResult(FALSE);
            if (FAILED(ec)) {
                Logger::W(TAG, "Failed to call onKeyguardExitResult(false)%08x", ec);
                return E_REMOTE_EXCEPTION;
            }
        }
        else if (mExternallyEnabled) {
            // this only applies when the user has externally disabled the
            // keyguard.  this is unexpected and means the user is not
            // using the api properly.
            if (DEBUG) Logger::W(TAG, "verifyUnlock called when not externally disabled");
            ECode ec = callback->OnKeyguardExitResult(FALSE);
            if (FAILED(ec)) {
                Logger::W(TAG, "Failed to call onKeyguardExitResult(false)%08x", ec);
                return E_REMOTE_EXCEPTION;
            }
        }
        else if (mExitSecureCallback != NULL) {
            if (DEBUG) {
                Logger::D(TAG, "VerifyUnlock already in progress with someone else. %s",
                    TO_CSTR(mExitSecureCallback));
            }
            // already in progress with someone else
            ECode ec = callback->OnKeyguardExitResult(FALSE);
            if (FAILED(ec)) {
                Logger::W(TAG, "Failed to call onKeyguardExitResult(false)%08x", ec);
                return E_REMOTE_EXCEPTION;
            }
        }
        else {
            mExitSecureCallback = callback;
            VerifyUnlockLocked();
        }
    }
    return NOERROR;
}

ECode CKeyguardViewMediator::IsShowing(
    /* [out] */ Boolean* isShowing)
{
    VALIDATE_NOT_NULL(isShowing)
    *isShowing = mShowing;
    return NOERROR;
}

ECode CKeyguardViewMediator::IsOccluded(
    /* [out] */ Boolean* isOccluded)
{
    VALIDATE_NOT_NULL(isOccluded)
    *isOccluded = mOccluded;
    return NOERROR;
}

ECode CKeyguardViewMediator::IsShowingAndNotOccluded(
    /* [out] */ Boolean* isShowingAndNotOccluded)
{
    VALIDATE_NOT_NULL(isShowingAndNotOccluded)
    *isShowingAndNotOccluded = mShowing && !mOccluded;
    return NOERROR;
}

ECode CKeyguardViewMediator::SetOccluded(
    /* [in] */ Boolean isOccluded)
{
    if (DEBUG) Logger::D(TAG, "setOccluded %s", isOccluded ? "TRUE" : "FALSE");
    mHandler->RemoveMessages(SET_OCCLUDED);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(SET_OCCLUDED, (isOccluded ? 1 : 0), 0, (IMessage**)&msg);
    Boolean result;
    mHandler->SendMessage(msg, &result);
    return NOERROR;
}

void CKeyguardViewMediator::HandleSetOccluded(
    /* [in] */ Boolean isOccluded)
{
    {
        AutoLock syncLock(this);
        if (mOccluded != isOccluded) {
            mOccluded = isOccluded;
            mStatusBarKeyguardViewManager->SetOccluded(isOccluded);
            UpdateActivityLockScreenState();
            AdjustStatusBarLocked();
        }
    }
}

ECode CKeyguardViewMediator::DoKeyguardTimeout(
    /* [in] */ IBundle* options)
{
    mHandler->RemoveMessages(KEYGUARD_TIMEOUT);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(KEYGUARD_TIMEOUT, options, (IMessage**)&msg);
    Boolean result;
    mHandler->SendMessage(msg, &result);
    return NOERROR;
}

ECode CKeyguardViewMediator::IsInputRestricted(
    /* [out] */ Boolean* isInputRestricted)
{
    VALIDATE_NOT_NULL(isInputRestricted)
    Boolean isDeviceProvisioned = TRUE;
    // mUpdateMonitor->IsDeviceProvisioned(&isDeviceProvisioned);
    *isInputRestricted = mShowing || mNeedToReshowWhenReenabled || !isDeviceProvisioned;
    return NOERROR;
}

void CKeyguardViewMediator::DoKeyguardLocked(
    /* [in] */ IBundle* options)
{
    // if another app is disabling us, don't show
    if (!mExternallyEnabled) {
        if (DEBUG) Logger::D(TAG, "doKeyguard: not showing because externally disabled");

        // note: we *should* set mNeedToReshowWhenReenabled=true here, but that makes
        // for an occasional ugly flicker in this situation:
        // 1) receive a call with the screen on (no keyguard) or make a call
        // 2) screen times out
        // 3) user hits key to turn screen back on
        // instead, we reenable the keyguard when we know the screen is off and the call
        // ends (see the broadcast receiver below)
        // TODO: clean this up when we have better support at the window manager level
        // for apps that wish to be on top of the keyguard
        return;
    }

    // if the keyguard is already showing, don't bother
    Boolean isShowing;
    mStatusBarKeyguardViewManager->IsShowing(&isShowing);
    if (isShowing) {
        if (DEBUG) Logger::D(TAG, "doKeyguard: not showing because it is already showing");
        return;
    }

    Logger::D(TAG, "TODO: Need the app Keyguard.");
    // if the setup wizard hasn't run yet, don't show
    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    //TODO
    // Boolean b1 = FALSE;
    Boolean b2 = FALSE;
    // sp->GetBoolean(String("keyguard.no_require_sim"), FALSE, &b1);
    // mUpdateMonitor->IsDeviceProvisioned(&b2);
    // const Boolean requireSim = !b1;
    const Boolean provisioned = b2;

    //TODO
    // IccCardConstantsState state;
    // mUpdateMonitor->GetSimState(&state);
    // const Boolean lockedOrMissing = state.isPinLocked()
    //         || ((state == IccCardConstantsState_ABSENT
    //         || state == IccCardConstantsState_PERM_DISABLED)
    //         && requireSim);
    Boolean lockedOrMissing = FALSE;//TODO
    if (!lockedOrMissing && !provisioned) {
        if (DEBUG) Logger::D(TAG, "doKeyguard: not showing because device isn't provisioned%s",
            " and the sim is not locked or missing");
        return;
    }

    Boolean b4, b5;
    mLockPatternUtils->IsLockScreenDisabled(&b4);
    if (b4 && !lockedOrMissing) {
        if (DEBUG) Logger::D(TAG, "doKeyguard: not showing because lockscreen is off");
        return;
    }

    mLockPatternUtils->CheckVoldPassword(&b5);
    if (b5) {
        if (DEBUG) Logger::D(TAG, "Not showing lock screen since just decrypted");
        // Without this, settings is not enabled until the lock screen first appears
        mShowing = FALSE;
        HideLocked();
        return;
    }

    if (DEBUG) Logger::D(TAG, "doKeyguard: showing the lock screen");
    ShowLocked(options);
}

ECode CKeyguardViewMediator::HandleDismiss()
{
    if (mShowing && !mOccluded) {
        mStatusBarKeyguardViewManager->Dismiss();
    }
    return NOERROR;
}

ECode CKeyguardViewMediator::Dismiss()
{
    Boolean result;
    mHandler->SendEmptyMessage(DISMISS, &result);
    return NOERROR;
}

void CKeyguardViewMediator::ResetStateLocked()
{
    if (DEBUG) Logger::E(TAG, "resetStateLocked");
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(RESET, (IMessage**)&msg);
    Boolean result;
    mHandler->SendMessage(msg, &result);
}

void CKeyguardViewMediator::VerifyUnlockLocked()
{
    if (DEBUG) Logger::D(TAG, "verifyUnlockLocked");
    Boolean result;
    mHandler->SendEmptyMessage(VERIFY_UNLOCK, &result);
}

void CKeyguardViewMediator::NotifyScreenOffLocked()
{
    if (DEBUG) Logger::D(TAG, "notifyScreenOffLocked");
    Boolean result;
    mHandler->SendEmptyMessage(NOTIFY_SCREEN_OFF, &result);
}

void CKeyguardViewMediator::NotifyScreenOnLocked(
    /* [in] */ IIKeyguardShowCallback* result)
{
    if (DEBUG) Logger::D(TAG, "notifyScreenOnLocked");
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(NOTIFY_SCREEN_ON, (IMessage**)&msg);
    Boolean b;
    mHandler->SendMessage(msg, &b);
}

void CKeyguardViewMediator::ShowLocked(
    /* [in] */ IBundle* options)
{
    if (DEBUG) Logger::D(TAG, "showLocked");
    // ensure we stay awake until we are finished displaying the keyguard
    mShowKeyguardWakeLock->AcquireLock();
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(SHOW, options, (IMessage**)&msg);
    Boolean result;
    mHandler->SendMessage(msg, &result);
}

void CKeyguardViewMediator::HideLocked()
{
    if (DEBUG) Logger::D(TAG, "hideLocked");
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(HIDE, (IMessage**)&msg);
    Boolean result;
    mHandler->SendMessage(msg, &result);
}

ECode CKeyguardViewMediator::IsSecure(
    /* [out] */ Boolean* isSecure)
{
    VALIDATE_NOT_NULL(isSecure)
    Boolean b;
    mLockPatternUtils->IsSecure(&b);

    //TODO
    // AutoPtr<IKeyguardUpdateMonitorHelper> kumh;
    // CKeyguardUpdateMonitorHelper::AcquireSingleton((IKeyguardUpdateMonitorHelper**)&kumh);
    // AutoPtr<IKeyguardUpdateMonitor> kum;
    // kumh->GetInstance(mContext, (IKeyguardUpdateMonitor**)&kum);

    // Boolean b2;
    // kum->IsSimPinSecure(&b2);

    // *isSecure = b || b2;

    return NOERROR;
}

ECode CKeyguardViewMediator::SetCurrentUser(
    /* [in] */ Int32 newUserId)
{
    return mLockPatternUtils->SetCurrentUser(newUserId);
}

ECode CKeyguardViewMediator::KeyguardDone(
    /* [in] */ Boolean authenticated,
    /* [in] */ Boolean wakeup)
{
    if (DEBUG) Logger::D(TAG, "keyguardDone(%s)", authenticated ? "TRUE" : "FALSE");
    // EventLog.writeEvent(70000, 2);//TODO
    {
        AutoLock syncLock(this);
        mKeyguardDonePending = FALSE;
    }
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(KEYGUARD_DONE, authenticated ? 1 : 0, wakeup ? 1 : 0, (IMessage**)&msg);
    Boolean result;
    mHandler->SendMessage(msg, &result);
    return NOERROR;
}

void CKeyguardViewMediator::HandleKeyguardDone(
    /* [in] */ Boolean authenticated,
    /* [in] */ Boolean wakeup)
{
    if (DEBUG) Logger::D(TAG, "handleKeyguardDone");

    if (authenticated) {
        //TODO
        // mUpdateMonitor->ClearFailedUnlockAttempts();
    }
    //TODO
    // mUpdateMonitor->ClearFingerprintRecognized();

    if (mExitSecureCallback != NULL) {
        ECode ec = mExitSecureCallback->OnKeyguardExitResult(authenticated);
        if (FAILED(ec)) {
            Logger::W(TAG, "Failed to call onKeyguardExitResult(%s)%08x", authenticated ? "TRUE" : "FALSE", ec);
            // return E_REMOTE_EXCEPTION;
        }

        mExitSecureCallback = NULL;

        if (authenticated) {
            // after succesfully exiting securely, no need to reshow
            // the keyguard when they've released the lock
            mExternallyEnabled = TRUE;
            mNeedToReshowWhenReenabled = FALSE;
        }
    }

    HandleHide();
}

void CKeyguardViewMediator::SendUserPresentBroadcast()
{
    {
        AutoLock syncLock(this);
        if (mBootCompleted) {
            Int32 user;
            mLockPatternUtils->GetCurrentUser(&user);
            AutoPtr<IUserHandle> uh;
            CUserHandle::New(user, (IUserHandle**)&uh);
            const AutoPtr<IUserHandle> currentUser = uh;
            mContext->SendBroadcastAsUser(USER_PRESENT_INTENT, currentUser);
        }
        else {
            mBootSendUserPresent = TRUE;
        }
    }
}

void CKeyguardViewMediator::HandleKeyguardDoneDrawing()
{
    {
        AutoLock syncLock(this);
        if (DEBUG) Logger::D(TAG, "handleKeyguardDoneDrawing");
        if (mWaitingUntilKeyguardVisible) {
            if (DEBUG) Logger::D(TAG, "handleKeyguardDoneDrawing: notifying mWaitingUntilKeyguardVisible");
            mWaitingUntilKeyguardVisible = FALSE;
            NotifyAll();

            // there will usually be two of these sent, one as a timeout, and one
            // as a result of the callback, so remove any remaining messages from
            // the queue
            mHandler->RemoveMessages(KEYGUARD_DONE_DRAWING);
        }
    }
}

void CKeyguardViewMediator::PlaySounds(
    /* [in] */ Boolean locked)
{
    // User feedback for keyguard.

    if (mSuppressNextLockSound) {
        mSuppressNextLockSound = FALSE;
        return;
    }

    PlaySound(locked ? mLockSoundId : mUnlockSoundId);
}

void CKeyguardViewMediator::PlaySound(
    /* [in] */ Int32 soundId)
{
    if (soundId == 0) return;
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    const AutoPtr<IContentResolver> cr = resolver;

    AutoPtr<ISettingsSystem> ss;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&ss);
    Int32 v;
    ss->GetInt32(cr, ISettingsSystem::LOCKSCREEN_SOUNDS_ENABLED, 1, &v);
    if (v == 1) {
        mLockSounds->Stop(mLockSoundStreamId);
        // Init mAudioManager
        if (mAudioManager == NULL) {
            AutoPtr<IInterface> obj;
            mContext->GetSystemService(IContext::AUDIO_SERVICE, (IInterface**)&obj);
            mAudioManager = IAudioManager::Probe(obj);

            if (mAudioManager == NULL) return;
            mAudioManager->GetMasterStreamType(&mMasterStreamType);
        }
        // If the stream is muted, don't play the sound
        Boolean b;
        mAudioManager->IsStreamMute(mMasterStreamType, &b);
        if (b) return;

        mLockSounds->Play(soundId, mLockSoundVolume, mLockSoundVolume,
            1/*priortiy*/, 0/*loop*/, 1.0f/*rate*/, &mLockSoundStreamId);
    }
}

void CKeyguardViewMediator::PlayTrustedSound()
{
    if (mSuppressNextLockSound) {
        return;
    }
    PlaySound(mTrustedSoundId);
}

void CKeyguardViewMediator::UpdateActivityLockScreenState()
{
    AutoPtr<IIActivityManager> am = ActivityManagerNative::GetDefault();
    ECode ec = am->SetLockScreenShown(mShowing && !mOccluded);
    if (FAILED(ec)) return;//return E_REMOTE_EXCEPTION;
}

void CKeyguardViewMediator::HandleShow(
    /* [in] */ IBundle* options)
{
    {
        AutoLock syncLock(this);
        if (!mSystemReady) {
            if (DEBUG) Logger::D(TAG, "ignoring handleShow because system is not ready.");
            return;
        }
        else {
            if (DEBUG) Logger::D(TAG, "handleShow");
        }

        mStatusBarKeyguardViewManager->Show(options);
        mHiding = FALSE;
        mShowing = TRUE;
        mKeyguardDonePending = FALSE;
        mHideAnimationRun = FALSE;
        UpdateActivityLockScreenState();
        AdjustStatusBarLocked();
        UserActivity();

        // Do this at the end to not slow down display of the keyguard.
        PlaySounds(TRUE);

        mShowKeyguardWakeLock->ReleaseLock();
    }
    // mKeyguardDisplayManager->Show();
}

void CKeyguardViewMediator::HandleHide()
{
    {
        AutoLock syncLock(this);
        if (DEBUG) Logger::D(TAG, "handleHide");

        mHiding = TRUE;
        if (mShowing && !mOccluded) {
            if (!mHideAnimationRun) {
                mStatusBarKeyguardViewManager->StartPreHideAnimation(mKeyguardGoingAwayRunnable);
            }
            else {
                mKeyguardGoingAwayRunnable->Run();
            }
        }
        else {

            // Don't try to rely on WindowManager - if Keyguard wasn't showing, window
            // manager won't start the exit animation.
            Int64 so, d;
            mHideAnimation->GetStartOffset(&so);
            mHideAnimation->GetDuration(&d);
            HandleStartKeyguardExitAnimation(SystemClock::GetUptimeMillis() + so, d);
        }
    }
}

void CKeyguardViewMediator::HandleOnActivityDrawn()
{
    if (mKeyguardDonePending) {
        mStatusBarKeyguardViewManager->OnActivityDrawn();
    }
}

void CKeyguardViewMediator::HandleStartKeyguardExitAnimation(
    /* [in] */ Int64 startTime,
    /* [in] */ Int64 fadeoutDuration)
{
    {
        AutoLock syncLock(this);

        if (!mHiding) {
            return;
        }
        mHiding = FALSE;

        // only play "unlock" noises if not on a call (since the incall UI
        // disables the keyguard)
        if (ITelephonyManager::EXTRA_STATE_IDLE.Equals(mPhoneState)) {
            PlaySounds(FALSE);
        }

        mStatusBarKeyguardViewManager->Hide(startTime, fadeoutDuration);
        mShowing = FALSE;
        mKeyguardDonePending = FALSE;
        mHideAnimationRun = FALSE;
        UpdateActivityLockScreenState();
        AdjustStatusBarLocked();
        SendUserPresentBroadcast();
    }
}

void CKeyguardViewMediator::AdjustStatusBarLocked()
{
    if (mStatusBarManager == NULL) {
        AutoPtr<IInterface> obj;
        mContext->GetSystemService(IContext::STATUS_BAR_SERVICE, (IInterface**)&obj);
        mStatusBarManager = IStatusBarManager::Probe(obj);
    }
    if (mStatusBarManager == NULL) {
        Logger::W(TAG, "Could not get status bar manager");
    }
    else {
        // Disable aspects of the system/status/navigation bars that must not be re-enabled by
        // windows that appear on top, ever
        Int32 flags = IStatusBarManager::DISABLE_NONE;
        if (mShowing) {
            // Permanently disable components not available when keyguard is enabled
            // (like recents). Temporary enable/disable (e.g. the "back" button) are
            // done in KeyguardHostView.
            flags |= IStatusBarManager::DISABLE_RECENT;
            flags |= IStatusBarManager::DISABLE_SEARCH;
        }
        Boolean n;
        IsShowingAndNotOccluded(&n);
        if (n) {
            flags |= IStatusBarManager::DISABLE_HOME;
        }

        if (DEBUG) {
            Boolean isSecure;
            IsSecure(&isSecure);
            Logger::D(TAG, "adjustStatusBarLocked: mShowing=%s mOccluded=%s isSecure=%s --> flags=0x%s",
                mShowing ? "TRUE" : "FALSE",
                mOccluded ? "TRUE" : "FALSE",
                isSecure ? "TRUE" : "FALSE",
                StringUtils::ToHexString(flags).string());
        }

        if (IActivity::Probe(mContext) == NULL) {
            mStatusBarManager->Disable(flags);
        }
    }
}

void CKeyguardViewMediator::HandleReset()
{
    {
        AutoLock syncLock(this);
        if (DEBUG) Logger::D(TAG, "handleReset");
        mStatusBarKeyguardViewManager->Reset();
    }
}

void CKeyguardViewMediator::HandleVerifyUnlock()
{
    {
        AutoLock syncLock(this);
        if (DEBUG) Logger::D(TAG, "handleVerifyUnlock");
        mStatusBarKeyguardViewManager->VerifyUnlock();
        mShowing = TRUE;
        UpdateActivityLockScreenState();
    }
}

void CKeyguardViewMediator::HandleNotifyScreenOff()
{
    {
        AutoLock syncLock(this);
        if (DEBUG) Logger::D(TAG, "handleNotifyScreenOff");
        mStatusBarKeyguardViewManager->OnScreenTurnedOff();
    }
}

void CKeyguardViewMediator::HandleNotifyScreenOn(
    /* [in] */ IIKeyguardShowCallback* callback)
{
    {
        AutoLock syncLock(this);
        if (DEBUG) Logger::D(TAG, "handleNotifyScreenOn");
        mStatusBarKeyguardViewManager->OnScreenTurnedOn(callback);
    }
}

ECode CKeyguardViewMediator::IsDismissable(
    /* [out] */ Boolean* isDismissable)
{
    VALIDATE_NOT_NULL(isDismissable)
    Boolean b;
    IsSecure(&b);
    *isDismissable = mKeyguardDonePending || !b;
    return NOERROR;
}

Boolean CKeyguardViewMediator::IsAssistantAvailable()
{
    if (mSearchManager == NULL) {
        return FALSE;
    }

    AutoPtr<IIntent> intent;
    mSearchManager->GetAssistIntent(mContext, FALSE, IUserHandle::USER_CURRENT,
        (IIntent**)&intent);
    return intent != NULL;
}

ECode CKeyguardViewMediator::OnBootCompleted()
{
    //TODO
    // mUpdateMonitor->DispatchBootCompleted();
    {
        AutoLock syncLock(this);
        mBootCompleted = TRUE;
        if (mBootSendUserPresent) {
            SendUserPresentBroadcast();
        }
    }
    return NOERROR;
}

ECode CKeyguardViewMediator::RegisterStatusBar(
    /* [in] */ IPhoneStatusBar* phoneStatusBar,
    /* [in] */ IViewGroup* container,
    /* [in] */ IStatusBarWindowManager* statusBarWindowManager,
    /* [in] */ IScrimController* scrimController,
    /* [out] */ IStatusBarKeyguardViewManager** result)
{
    VALIDATE_NOT_NULL(result);
    mStatusBarKeyguardViewManager->RegisterStatusBar(phoneStatusBar, container,
            statusBarWindowManager, scrimController);
    *result = mStatusBarKeyguardViewManager;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CKeyguardViewMediator::StartKeyguardExitAnimation(
    /* [in] */ Int64 startTime,
    /* [in] */ Int64 fadeoutDuration)
{
    //TODO
    #if 0
    Message msg = mHandler.obtainMessage(START_KEYGUARD_EXIT_ANIM,
            new StartKeyguardExitAnimParams(startTime, fadeoutDuration));
    mHandler.sendMessage(msg);
    #endif
    return NOERROR;
}

ECode CKeyguardViewMediator::OnActivityDrawn()
{
    Boolean b;
    mHandler->SendEmptyMessage(ON_ACTIVITY_DRAWN, &b);
    return NOERROR;
}

ECode CKeyguardViewMediator::GetViewMediatorCallback(
    /* [out] */ IViewMediatorCallback** cb)
{
    VALIDATE_NOT_NULL(cb)
    *cb = mViewMediatorCallback;
    REFCOUNT_ADD(*cb);
    return NOERROR;
}

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos