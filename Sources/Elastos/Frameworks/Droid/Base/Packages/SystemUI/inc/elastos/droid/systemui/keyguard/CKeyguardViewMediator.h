
#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_KEYGUARDVIEWMEDIATOR_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_KEYGUARDVIEWMEDIATOR_H__

#include "_Elastos_Droid_SystemUI_Keyguard_CKeyguardViewMediator.h"
#include "_Elastos.Droid.Core.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/os/Runnable.h"
#include "elastos/droid/systemui/SystemUI.h"

using Elastos::Droid::App::IAlarmManager;
using Elastos::Droid::App::ISearchManager;
using Elastos::Droid::App::IStatusBarManager;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Internal::Policy::IIKeyguardExitCallback;
using Elastos::Droid::Internal::Policy::IIKeyguardShowCallback;
using Elastos::Droid::Internal::Telephony::IccCardConstantsState;
using Elastos::Droid::Internal::Widget::ILockPatternUtils;
using Elastos::Droid::Keyguard::IKeyguardDisplayManager;
using Elastos::Droid::Keyguard::IKeyguardUpdateMonitor;
using Elastos::Droid::Keyguard::IKeyguardUpdateMonitorCallback;
using Elastos::Droid::Keyguard::IViewMediatorCallback;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Media::ISoundPool;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::IPowerManagerWakeLock;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::SystemUI::SystemUI;
using Elastos::Droid::SystemUI::StatusBar::Phone::IPhoneStatusBar;
using Elastos::Droid::SystemUI::StatusBar::Phone::IScrimController;
using Elastos::Droid::SystemUI::StatusBar::Phone::IStatusBarKeyguardViewManager;
using Elastos::Droid::SystemUI::StatusBar::Phone::IStatusBarWindowManager;
using Elastos::Droid::View::IIWindowManager;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::Animation::IAnimation;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

/**
 * Mediates requests related to the keyguard.  This includes queries about the
 * state of the keyguard, power management events that effect whether the keyguard
 * should be shown or reset, callbacks to the phone window manager to notify
 * it of when the keyguard is showing, and events from the keyguard view itself
 * stating that the keyguard was succesfully unlocked.
 *
 * Note that the keyguard view is shown when the screen is off (as appropriate)
 * so that once the screen comes on, it will be ready immediately.
 *
 * Example queries about the keyguard:
 * - is {movement, key} one that should wake the keygaurd?
 * - is the keyguard showing?
 * - are input events restricted due to the state of the keyguard?
 *
 * Callbacks to the phone window manager:
 * - the keyguard is showing
 *
 * Example external events that translate to keyguard view changes:
 * - screen turned off -> reset the keyguard, and show it so it will be ready
 *   next time the screen turns on
 * - keyboard is slid open -> if the keyguard is not secure, hide it
 *
 * Events from the keyguard view:
 * - user succesfully unlocked keyguard -> hide keyguard view, and no longer
 *   restrict input events.
 *
 * Note: in addition to normal power managment events that effect the state of
 * whether the keyguard should be showing, external apps and services may request
 * that the keyguard be disabled via {@link #setKeyguardEnabled(boolean)}.  When
 * false, this will override all other conditions for turning on the keyguard.
 *
 * Threading and synchronization:
 * This class is created by the initialization routine of the {@link android.view.WindowManagerPolicy},
 * and runs on its thread.  The keyguard UI is created from that thread in the
 * constructor of this class.  The apis may be called from other threads, including the
 * {@link com.android.server.input.InputManagerService}'s and {@link android.view.WindowManager}'s.
 * Therefore, methods on this class are synchronized, and any action that is pointed
 * directly to the keyguard UI is posted to a {@link android.os.Handler} to ensure it is taken on the UI
 * thread of the keyguard.
 */
CarClass(CKeyguardViewMediator)
    , public SystemUI
    , public IKeyguardViewMediator
{
private:
    class MyKeyguardUpdateMonitorCallback
        : public Object
        , public IKeyguardUpdateMonitorCallback
    {
    public:
        CAR_INTERFACE_DECL()

        MyKeyguardUpdateMonitorCallback(
            /* [in] */ CKeyguardViewMediator* host);

        // @Override
        CARAPI OnUserSwitching(
            /* [in] */ Int32 userId);

        // @Override
        CARAPI OnUserSwitchComplete(
            /* [in] */ Int32 userId);

        // @Override
        CARAPI OnUserRemoved(
            /* [in] */ Int32 userId);

        // @Override
        CARAPI OnUserInfoChanged(
            /* [in] */ Int32 userId);

        // @Override
        CARAPI OnPhoneStateChanged(
            /* [in] */ Int32 phoneState);

        // @Override
        CARAPI OnClockVisibilityChanged();

        // @Override
        CARAPI OnDeviceProvisioned();

        // @Override
        CARAPI OnSimStateChanged(
            /* [in] */ IccCardConstantsState simState);

        CARAPI OnFingerprintRecognized(
            /* [in] */ Int32 userId);

    private:
        CKeyguardViewMediator* mHost;
    };

    class MyViewMediatorCallback
        : public Object
        , public IViewMediatorCallback
    {
    public:
        CAR_INTERFACE_DECL()

        MyViewMediatorCallback(
            /* [in] */ CKeyguardViewMediator* host);

        CARAPI UserActivity();

        CARAPI KeyguardDone(
            /* [in] */ Boolean authenticated);

        CARAPI KeyguardDoneDrawing();

        // @Override
        CARAPI SetNeedsInput(
            /* [in] */ Boolean needsInput);

        // @Override
        CARAPI OnUserActivityTimeoutChanged();

        // @Override
        CARAPI KeyguardDonePending();

        // @Override
        CARAPI KeyguardGone();

        // @Override
        CARAPI ReadyForKeyguardDone();

        // @Override
        CARAPI PlayTrustedSound();

    private:
        CKeyguardViewMediator* mHost;
    };

    class StartKeyguardExitAnimParams
        : public Object
    {
    private:
        StartKeyguardExitAnimParams(
            /* [in] */ Int64 startTime,
            /* [in] */ Int64 fadeoutDuration);

    public:
        Int64 mStartTime;
        Int64 mFadeoutDuration;
    };

    class MyBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        MyBroadcastReceiver(
            /* [in] */ CKeyguardViewMediator* host);

        // @Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* str)
        {
            *str = String("CKeyguardViewMediator.MyBroadcastReceiver");
            return NOERROR;
        }

    private:
        CKeyguardViewMediator* mHost;
    };

    class MyHandler
        : public Handler
    {
    public:
        MyHandler(
            /* [in] */ CKeyguardViewMediator* host);

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CKeyguardViewMediator* mHost;
    };

    class MyRunnable
        : public Runnable
    {
    public:
        MyRunnable(
            /* [in] */ CKeyguardViewMediator* host);

        // @Override
        CARAPI Run();

    private:
        CKeyguardViewMediator* mHost;
    };

public:
    CKeyguardViewMediator();

    CAR_INTERFACE_DECL()

    CARAPI UserActivity();

private:
    CARAPI_(void) Setup();

public:
    // @Override
    CARAPI Start();

    /**
     * Let us know that the system is ready after startup.
     */
    CARAPI OnSystemReady();

    /**
     * Called to let us know the screen was turned off.
     * @param why either {@link android.view.WindowManagerPolicy#OFF_BECAUSE_OF_USER} or
     *   {@link android.view.WindowManagerPolicy#OFF_BECAUSE_OF_TIMEOUT}.
     */
    CARAPI OnScreenTurnedOff(
        /* [in] */ Int32 why);

    /**
     * Let's us know the screen was turned on.
     */
    CARAPI OnScreenTurnedOn(
        /* [in] */ IIKeyguardShowCallback* callback);

    /**
     * A dream started.  We should lock after the usual screen-off lock timeout but only
     * if there is a secure lock pattern.
     */
    CARAPI OnDreamingStarted();

    /**
     * A dream stopped.
     */
    CARAPI OnDreamingStopped();

    /**
     * Same semantics as {@link android.view.WindowManagerPolicy#enableKeyguard}; provide
     * a way for external stuff to override normal keyguard behavior.  For instance
     * the phone app disables the keyguard when it receives incoming calls.
     */
    CARAPI SetKeyguardEnabled(
        /* [in] */ Boolean enabled);

    /**
     * @see android.app.KeyguardManager#exitKeyguardSecurely
     */
    CARAPI VerifyUnlock(
        /* [in] */ IIKeyguardExitCallback* callback);

    /**
     * Is the keyguard currently showing?
     */
    CARAPI IsShowing(
        /* [out] */ Boolean* isShowing);

    CARAPI IsOccluded(
        /* [out] */ Boolean* isOccluded);

    /**
     * Is the keyguard currently showing and not being force hidden?
     */
    CARAPI IsShowingAndNotOccluded(
        /* [out] */ Boolean* isShowingAndNotOccluded);

    /**
     * Notify us when the keyguard is occluded by another window
     */
    CARAPI SetOccluded(
        /* [in] */ Boolean isOccluded);

    /**
     * Used by PhoneWindowManager to enable the keyguard due to a user activity timeout.
     * This must be safe to call from any thread and with any window manager locks held.
     */
    CARAPI DoKeyguardTimeout(
        /* [in] */ IBundle* options);

    /**
     * Given the state of the keyguard, is the input restricted?
     * Input is restricted when the keyguard is showing, or when the keyguard
     * was suppressed by an app that disabled the keyguard or we haven't been provisioned yet.
     */
    CARAPI IsInputRestricted(
        /* [out] */ Boolean* isInputRestricted);

    /**
     * Dismiss the keyguard through the security layers.
     */
    CARAPI HandleDismiss();

    CARAPI Dismiss();

    CARAPI IsSecure(
        /* [out] */ Boolean* isSecure);

    /**
     * Update the newUserId. Call while holding WindowManagerService lock.
     * NOTE: Should only be called by CKeyguardViewMediator in response to the user id changing.
     *
     * @param newUserId The id of the incoming user.
     */
    CARAPI SetCurrentUser(
        /* [in] */ Int32 newUserId);

    CARAPI KeyguardDone(
        /* [in] */ Boolean authenticated,
        /* [in] */ Boolean wakeup);

    CARAPI IsDismissable(
        /* [out] */ Boolean* isDismissable);

    CARAPI OnBootCompleted();

    CARAPI RegisterStatusBar(
        /* [in] */ IPhoneStatusBar* phoneStatusBar,
        /* [in] */ IViewGroup* container,
        /* [in] */ IStatusBarWindowManager* statusBarWindowManager,
        /* [in] */ IScrimController* scrimController,
        /* [out] */ IStatusBarKeyguardViewManager** result);

    CARAPI StartKeyguardExitAnimation(
        /* [in] */ Int64 startTime,
        /* [in] */ Int64 fadeoutDuration);

    CARAPI OnActivityDrawn();

    CARAPI GetViewMediatorCallback(
        /* [out] */ IViewMediatorCallback** cb);

private:
    CARAPI_(void) DoKeyguardLaterLocked();

    CARAPI_(void) CancelDoKeyguardLaterLocked();

    CARAPI_(void) MaybeSendUserPresentBroadcast();

    /**
     * Handles SET_OCCLUDED message sent by setOccluded()
     */
    CARAPI_(void) HandleSetOccluded(
        /* [in] */ Boolean isOccluded);

    /**
     * Enable the keyguard if the settings are appropriate.
     */
    CARAPI_(void) DoKeyguardLocked(
        /* [in] */ IBundle* options);

    /**
     * Send message to keyguard telling it to reset its state.
     * @see #handleReset
     */
    CARAPI_(void) ResetStateLocked();

    /**
     * Send message to keyguard telling it to verify unlock
     * @see #handleVerifyUnlock()
     */
    CARAPI_(void) VerifyUnlockLocked();

    /**
     * Send a message to keyguard telling it the screen just turned on.
     * @see #onScreenTurnedOff(int)
     * @see #handleNotifyScreenOff
     */
    CARAPI_(void) NotifyScreenOffLocked();

    /**
     * Send a message to keyguard telling it the screen just turned on.
     * @see #onScreenTurnedOn
     * @see #handleNotifyScreenOn
     */
    CARAPI_(void) NotifyScreenOnLocked(
        /* [in] */ IIKeyguardShowCallback* result);

    /**
     * Send message to keyguard telling it to show itself
     * @see #handleShow
     */
    CARAPI_(void) ShowLocked(
        /* [in] */ IBundle* options);

    /**
     * Send message to keyguard telling it to hide itself
     * @see #handleHide()
     */
    CARAPI_(void) HideLocked();

    /**
     * @see #keyguardDone
     * @see #KEYGUARD_DONE
     */
    CARAPI_(void) HandleKeyguardDone(
        /* [in] */ Boolean authenticated,
        /* [in] */ Boolean wakeup);

    CARAPI_(void) SendUserPresentBroadcast();

    /**
     * @see #keyguardDone
     * @see #KEYGUARD_DONE_DRAWING
     */
    CARAPI_(void) HandleKeyguardDoneDrawing();

    CARAPI_(void) PlaySounds(
        /* [in] */ Boolean locked);

    CARAPI_(void) PlaySound(
        /* [in] */ Int32 soundId);

    CARAPI_(void) PlayTrustedSound();

    CARAPI_(void) UpdateActivityLockScreenState();

    /**
     * Handle message sent by {@link #showLocked}.
     * @see #SHOW
     */
    CARAPI_(void) HandleShow(
        /* [in] */ IBundle* options);

    /**
     * Handle message sent by {@link #hideLocked()}
     * @see #HIDE
     */
    CARAPI_(void) HandleHide();

    CARAPI_(void) HandleOnActivityDrawn();

    CARAPI_(void) HandleStartKeyguardExitAnimation(
        /* [in] */ Int64 startTime,
        /* [in] */ Int64 fadeoutDuration);

    CARAPI_(void) AdjustStatusBarLocked();

    /**
     * Handle message sent by {@link #resetStateLocked}
     * @see #RESET
     */
    CARAPI_(void) HandleReset();

    /**
     * Handle message sent by {@link #verifyUnlock}
     * @see #VERIFY_UNLOCK
     */
    CARAPI_(void) HandleVerifyUnlock();

    /**
     * Handle message sent by {@link #notifyScreenOffLocked()}
     * @see #NOTIFY_SCREEN_OFF
     */
    CARAPI_(void) HandleNotifyScreenOff();

    /**
     * Handle message sent by {@link #notifyScreenOnLocked}
     * @see #NOTIFY_SCREEN_ON
     */
    CARAPI_(void) HandleNotifyScreenOn(
        /* [in] */ IIKeyguardShowCallback* callback);

    CARAPI_(Boolean) IsAssistantAvailable();

public:
    /**
     * The default amount of time we stay awake (used for all key input)
     */
    const static Int32 AWAKE_INTERVAL_DEFAULT_MS = 10000;

private:
    const static Int32 KEYGUARD_DISPLAY_TIMEOUT_DELAY_DEFAULT = 30000;
    const static Boolean DEBUG;
    const static Boolean DBG_WAKE;

    const static String TAG;

    const static String DELAYED_KEYGUARD_ACTION;
    // used for handler messages
    const static Int32 SHOW = 2;
    const static Int32 HIDE = 3;
    const static Int32 RESET = 4;
    const static Int32 VERIFY_UNLOCK = 5;
    const static Int32 NOTIFY_SCREEN_OFF = 6;
    const static Int32 NOTIFY_SCREEN_ON = 7;
    const static Int32 KEYGUARD_DONE = 9;
    const static Int32 KEYGUARD_DONE_DRAWING = 10;
    const static Int32 KEYGUARD_DONE_AUTHENTICATING = 11;
    const static Int32 SET_OCCLUDED = 12;
    const static Int32 KEYGUARD_TIMEOUT = 13;
    const static Int32 DISMISS = 17;
    const static Int32 START_KEYGUARD_EXIT_ANIM = 18;
    const static Int32 ON_ACTIVITY_DRAWN = 19;

    /**
     * How long to wait after the screen turns off due to timeout before
     * turning on the keyguard (i.e, the user has this much time to turn
     * the screen back on without having to face the keyguard).
     */
    const static Int32 KEYGUARD_LOCK_AFTER_DELAY_DEFAULT = 5000;

    /**
     * How long we'll wait for the {@link ViewMediatorCallback#keyguardDoneDrawing()}
     * callback before unblocking a call to {@link #setKeyguardEnabled(boolean)}
     * that is reenabling the keyguard.
     */
    const static Int32 KEYGUARD_DONE_DRAWING_TIMEOUT_MS = 2000;

    /**
     * Secure setting whether analytics are collected on the keyguard.
     */
    const static String KEYGUARD_ANALYTICS_SETTING;

    /** The stream type that the lock sounds are tied to. */
    Int32 mMasterStreamType;

    AutoPtr<IAlarmManager> mAlarmManager;
    AutoPtr<IAudioManager> mAudioManager;
    AutoPtr<IStatusBarManager> mStatusBarManager;
    Boolean mSwitchingUser;

    Boolean mSystemReady;
    Boolean mBootCompleted;
    Boolean mBootSendUserPresent;

    // Whether the next call to playSounds() should be skipped.  Defaults to
    // true because the first lock (on boot) should be silent.
    Boolean mSuppressNextLockSound;


    /** High level access to the power manager for WakeLocks */
    AutoPtr<IPowerManager> mPM;

    /** High level access to the window manager for dismissing keyguard animation */
    AutoPtr<IIWindowManager> mWM;

    /** UserManager for querying number of users */
    AutoPtr<IUserManager> mUserManager;

    /** SearchManager for determining whether or not search assistant is available */
    AutoPtr<ISearchManager> mSearchManager;

    /**
     * Used to keep the device awake while to ensure the keyguard finishes opening before
     * we sleep.
     */
    AutoPtr<IPowerManagerWakeLock> mShowKeyguardWakeLock;

    AutoPtr<IStatusBarKeyguardViewManager> mStatusBarKeyguardViewManager;

    // these are protected by synchronized (this)

    /**
     * External apps (like the phone app) can tell us to disable the keygaurd.
     */
    Boolean mExternallyEnabled;

    /**
     * Remember if an external call to {@link #setKeyguardEnabled} with value
     * false caused us to hide the keyguard, so that we need to reshow it once
     * the keygaurd is reenabled with another call with value true.
     */
    Boolean mNeedToReshowWhenReenabled;

    // cached value of whether we are showing (need to know this to quickly
    // answer whether the input should be restricted)
    Boolean mShowing;

    // true if the keyguard is hidden by another window
    Boolean mOccluded;

    /**
     * Helps remember whether the screen has turned on since the last time
     * it turned off due to timeout. see {@link #onScreenTurnedOff(int)}
     */
    Int32 mDelayedShowingSequence;

    /**
     * If the user has disabled the keyguard, then requests to exit, this is
     * how we'll ultimately let them know whether it was successful.  We use this
     * var being non-null as an indicator that there is an in progress request.
     */
    AutoPtr<IIKeyguardExitCallback> mExitSecureCallback;

    // the properties of the keyguard

    AutoPtr<IKeyguardUpdateMonitor> mUpdateMonitor;

    Boolean mScreenOn;

    // last known state of the cellular connection
    String mPhoneState;

    /**
     * Whether a hide is pending an we are just waiting for #startKeyguardExitAnimation to be
     * called.
     * */
    Boolean mHiding;

    /**
     * we send this intent when the keyguard is dismissed.
     */
    static AutoPtr<IIntent> USER_PRESENT_INTENT;

    /**
     * {@link #setKeyguardEnabled} waits on this condition when it reenables
     * the keyguard.
     */
    Boolean mWaitingUntilKeyguardVisible;
    AutoPtr<ILockPatternUtils> mLockPatternUtils;
    Boolean mKeyguardDonePending;
    Boolean mHideAnimationRun ;

    AutoPtr<ISoundPool> mLockSounds;
    Int32 mLockSoundId;
    Int32 mUnlockSoundId;
    Int32 mTrustedSoundId;
    Int32 mLockSoundStreamId;

    /**
     * The animation used for hiding keyguard. This is used to fetch the animation timings if
     * WindowManager is not providing us with them.
     */
    AutoPtr<IAnimation> mHideAnimation;

    /**
     * The volume applied to the lock/unlock sounds.
     */
    Float mLockSoundVolume;

    /**
     * For managing external displays
     */
    AutoPtr<IKeyguardDisplayManager> mKeyguardDisplayManager;

    AutoPtr<MyKeyguardUpdateMonitorCallback> mUpdateCallback;
    AutoPtr<MyViewMediatorCallback> mViewMediatorCallback;
    AutoPtr<MyBroadcastReceiver> mBroadcastReceiver;
    AutoPtr<MyRunnable> mKeyguardGoingAwayRunnable;

    /**
     * This handler will be associated with the policy thread, which will also
     * be the UI thread of the keyguard.  Since the apis of the policy, and therefore
     * this class, can be called by other threads, any action that directly
     * interacts with the keyguard ui should be posted to this handler, rather
     * than called directly.
     */
     AutoPtr<MyHandler> mHandler;
};

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_KEYGUARD_KEYGUARDVIEWMEDIATOR_H__