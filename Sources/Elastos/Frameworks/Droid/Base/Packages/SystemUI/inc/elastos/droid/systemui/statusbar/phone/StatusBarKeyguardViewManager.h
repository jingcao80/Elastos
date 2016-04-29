
#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_STATUSBARKEYGUARDVIEWMANAGER_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_STATUSBARKEYGUARDVIEWMANAGER_H__

#include "_Elastos.Droid.SystemUI.h"
#include "elastos/droid/systemui/statusbar/phone/KeyguardBouncer.h"
#include "Elastos.Droid.Os.h"
#include <elastos/droid/os/Runnable.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Internal::Policy::IIKeyguardShowCallback;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

/**
 * Manages creating, showing, hiding and resetting the keyguard within the status bar. Calls back
 * via {@link ViewMediatorCallback} to poke the wake lock and report that the keyguard is done,
 * which is in turn, reported to this class by the current
 * {@link com.android.keyguard.KeyguardViewBase}.
 */
class StatusBarKeyguardViewManager
    : public Object
    , public IStatusBarKeyguardViewManager
{
private:
    class Runnable1: public Runnable
    {
    public:
        Runnable1(
            /* [in] */ StatusBarKeyguardViewManager* host,
            /* [in] */ IIKeyguardShowCallback* callback);

        // @Override
        CARAPI Run();

    private:
        StatusBarKeyguardViewManager* mHost;
        AutoPtr<IIKeyguardShowCallback> mCallback;
    };

    class Runnable2: public Runnable
    {
    public:
        Runnable2(
            /* [in] */ StatusBarKeyguardViewManager* host);

        // @Override
        CARAPI Run();

    private:
        StatusBarKeyguardViewManager* mHost;
    };

    class Runnable3: public Runnable
    {
    public:
        Runnable3(
            /* [in] */ StatusBarKeyguardViewManager* host);

        // @Override
        CARAPI Run();

    private:
        StatusBarKeyguardViewManager* mHost;
    };

    class Runnable4: public Runnable
    {
    public:
        Runnable4(
            /* [in] */ StatusBarKeyguardViewManager* host);

        // @Override
        CARAPI Run();

    private:
        StatusBarKeyguardViewManager* mHost;
    };

    class Runnable5: public Runnable
    {
    public:
        Runnable5(
            /* [in] */ StatusBarKeyguardViewManager* host);

        // @Override
        CARAPI Run();

    private:
        StatusBarKeyguardViewManager* mHost;
    };

    class Runnable6: public Runnable
    {
    public:
        Runnable6(
            /* [in] */ StatusBarKeyguardViewManager* host);

        // @Override
        CARAPI Run();

    private:
        StatusBarKeyguardViewManager* mHost;
    };

    class Runnable7: public Runnable
    {
    public:
        Runnable7(
            /* [in] */ StatusBarKeyguardViewManager* host);

        // @Override
        CARAPI Run();

    private:
        StatusBarKeyguardViewManager* mHost;
    };

public:
    CAR_INTERFACE_DECL();

    StatusBarKeyguardViewManager(
        /* [in] */ IContext* context,
        /* [in] */ IViewMediatorCallback* callback,
        /* [in] */ ILockPatternUtils* lockPatternUtils);

    CARAPI RegisterStatusBar(
        /* [in] */ IPhoneStatusBar* phoneStatusBar,
        /* [in] */ IViewGroup* container,
        /* [in] */ IStatusBarWindowManager* statusBarWindowManager,
        /* [in] */ IScrimController* scrimController);

    /**
     * Show the keyguard.  Will handle creating and attaching to the view manager
     * lazily.
     */
    CARAPI Show(
        /* [in] */ IBundle* options);

    CARAPI DismissWithAction(
        /* [in] */ IOnDismissAction* r,
        /* [in] */ Boolean afterKeyguardGone);

    /**
     * Reset the state of the view.
     */
    CARAPI Reset();

    CARAPI OnScreenTurnedOff();

    CARAPI OnScreenTurnedOn(
        /* [in] */ IIKeyguardShowCallback* callback);

    CARAPI VerifyUnlock();

    CARAPI SetNeedsInput(
        /* [in] */ Boolean needsInput);

    CARAPI UpdateUserActivityTimeout();

    CARAPI SetOccluded(
        /* [in] */ Boolean occluded);

    CARAPI IsOccluded(
        /* [out] */ Boolean* result);

    /**
     * Starts the animation before we dismiss Keyguard, i.e. an disappearing animation on the
     * security view of the bouncer.
     *
     * @param finishRunnable the runnable to be run after the animation finished, or {@code NULL} if
     *                       no action should be run
     */
    CARAPI StartPreHideAnimation(
        /* [in] */ IRunnable* finishRunnable);

    /**
     * Hides the keyguard view
     */
    CARAPI Hide(
        /* [in] */ Int64 startTime,
        /* [in] */ Int64 fadeoutDuration);

    /**
     * Dismisses the keyguard by going to the next screen or making it gone.
     */
    CARAPI Dismiss();

    CARAPI IsSecure(
        /* [out] */ Boolean* result);

    /**
     * @return Whether the keyguard is showing
     */
    CARAPI IsShowing(
        /* [out] */ Boolean* result);

    /**
     * Notifies this manager that the back button has been pressed.
     *
     * @return whether the back press has been handled
     */
    CARAPI OnBackPressed(
        /* [out] */ Boolean* result);

    CARAPI IsBouncerShowing(
        /* [out] */ Boolean* result);

    CARAPI OnMenuPressed(
        /* [out] */ Boolean* result);

    CARAPI InterceptMediaKey(
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    CARAPI OnActivityDrawn();

    CARAPI ShouldDisableWindowAnimationsForUnlock(
        /* [out] */ Boolean* result);

    CARAPI IsGoingToNotificationShade(
        /* [out] */ Boolean* result);

private:
    /**
     * Shows the notification keyguard or the bouncer depending on
     * {@link KeyguardBouncer#needsFullscreenBouncer()}.
     */
    CARAPI_(void) ShowBouncerOrKeyguard();

    CARAPI_(void) ShowBouncer();

    CARAPI_(void) CallbackAfterDraw(
        /* [in] */ IIKeyguardShowCallback* callback);

    CARAPI_(void) ExecuteAfterKeyguardGoneAction();

    CARAPI_(Int64) GetNavBarShowDelay();

    CARAPI_(void) UpdateStates();

private:
    // When hiding the Keyguard with timing supplied from WindowManager, better be early than late.
    static const Int64 HIDE_TIMING_CORRECTION_MS;

    // Delay for showing the navigation bar when the bouncer appears. This should be kept in sync
    // with the appear animations of the PIN/pattern/password views.
    static const Int64 NAV_BAR_SHOW_DELAY_BOUNCER;

    static String TAG;

    AutoPtr<IContext> mContext;

    AutoPtr<ILockPatternUtils> mLockPatternUtils;
    AutoPtr<IViewMediatorCallback> mViewMediatorCallback;
    AutoPtr<IPhoneStatusBar> mPhoneStatusBar;
    AutoPtr<IScrimController> mScrimController;

    AutoPtr<IViewGroup> mContainer;
    AutoPtr<IStatusBarWindowManager> mStatusBarWindowManager;

    Boolean mScreenOn;
    AutoPtr<KeyguardBouncer> mBouncer;
    Boolean mShowing;
    Boolean mOccluded;

    Boolean mFirstUpdate;
    Boolean mLastShowing;
    Boolean mLastOccluded;
    Boolean mLastBouncerShowing;
    Boolean mLastBouncerDismissible;
    AutoPtr<IOnDismissAction> mAfterKeyguardGoneAction;
    AutoPtr<IRunnable> mMakeNavigationBarVisibleRunnable;
};

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_STATUSBARKEYGUARDVIEWMANAGER_H__
