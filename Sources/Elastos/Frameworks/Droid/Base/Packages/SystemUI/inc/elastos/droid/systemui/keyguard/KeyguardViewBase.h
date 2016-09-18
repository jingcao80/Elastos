
#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_KEYGUARDVIEWBASE_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_KEYGUARDVIEWBASE_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Telephony.h"
#include <elastos/droid/widget/FrameLayout.h>
#include "elastos/droid/systemui/keyguard/KeyguardActivityLauncher.h"

using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::Widget::FrameLayout;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

/**
 * Base class for keyguard view.  {@link #reset} is where you should
 * reset the state of your view.  Use the {@link KeyguardViewCallback} via
 * {@link #getCallback()} to send information back (such as poking the wake lock,
 * or finishing the keyguard).
 *
 * Handles intercepting of media keys that still work when the keyguard is
 * showing.
 */
class KeyguardViewBase
    : public FrameLayout
    , public IKeyguardViewBase
{
private:
    class InnerCallback
        : public Object
        , public IKeyguardSecurityContainerSecurityCallback
    {
    public:
        TO_STRING_IMPL("KeyguardViewBase::InnerCallback")

        CAR_INTERFACE_DECL()

        InnerCallback(
            /* [in] */ KeyguardViewBase* host);

        //@Override
        CARAPI Dismiss(
            /* [in] */ Boolean authenticated,
            /* [out] */ Boolean* result);

        /**
         * Authentication has happened and it's time to dismiss keyguard. This function
         * should clean up and inform KeyguardViewMediator.
         */
        //@Override
        CARAPI Finish();

        //@Override
        CARAPI OnSecurityModeChanged(
            /* [in] */ SecurityMode securityMode,
            /* [in] */ Boolean needsInput);

        CARAPI UserActivity();

    private:
        KeyguardViewBase* mHost;

    };

    class MyKeyguardActivityLauncher
        : public KeyguardActivityLauncher
    {
    public:
        TO_STRING_IMPL("KeyguardViewBase::MyKeyguardActivityLauncher")

        MyKeyguardActivityLauncher(
            /* [in] */ KeyguardViewBase* host)
            : mHost(host)
        {}

        //@Override
        CARAPI GetContext(
            /* [out] */ IContext** context);

        //@Override
        CARAPI SetOnDismissAction(
            /* [in] */ IKeyguardHostViewOnDismissAction* action);

        //@Override
        CARAPI GetLockPatternUtils(
            /* [out] */ ILockPatternUtils** utils);

        //@Override
        CARAPI RequestDismissKeyguard();

    private:
        KeyguardViewBase* mHost;
    };

public:
    TO_STRING_IMPL("KeyguardViewBase")

    CAR_INTERFACE_DECL()

    KeyguardViewBase();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    /**
     * Sets an action to run when keyguard finishes.
     *
     * @param action
     */
    CARAPI SetOnDismissAction(
        /* [in] */ IKeyguardHostViewOnDismissAction* action);

    /**
     * Called when the view needs to be shown.
     */
    CARAPI Show();

    /**
     *  Dismisses the keyguard by going to the next screen or making it gone.
     *
     *  @return True if the keyguard is done.
     */
    CARAPI Dismiss(
        /* [out] */ Boolean* result);

    CARAPI HandleBackKey(
        /* [out] */ Boolean* result);

    //@Override
    virtual CARAPI Dismiss(
        /* [in] */ Boolean authenticated,
        /* [out] */ Boolean* result);

    /**
     * Authentication has happened and it's time to dismiss keyguard. This function
     * should clean up and inform KeyguardViewMediator.
     */
    //@Override
    virtual CARAPI Finish();

    //@Override
    virtual CARAPI OnSecurityModeChanged(
        /* [in] */ SecurityMode securityMode,
        /* [in] */ Boolean needsInput);

    virtual CARAPI UserActivity();

    /**
     * Called when the Keyguard is not actively shown anymore on the screen.
     */
    CARAPI OnPause();

    /**
     * Called when the Keyguard is actively shown on the screen.
     */
    CARAPI OnResume();

    /**
     * Starts the animation when the Keyguard gets shown.
     */
    CARAPI StartAppearAnimation();

    CARAPI StartDisappearAnimation(
        /* [in] */ IRunnable* finishRunnable);

    /**
     * Verify that the user can get past the keyguard securely.  This is called,
     * for example, when the phone disables the keyguard but then wants to launch
     * something else that requires secure access.
     *
     * The result will be propogated back via {@link KeyguardViewCallback#keyguardDone(boolean)}
     */
    CARAPI VerifyUnlock();

    /**
     * Called before this view is being removed.
     */
    virtual CARAPI CleanUp() = 0;

    /**
     * Gets the desired user activity timeout in milliseconds, or -1 if the
     * default should be used.
     */
    virtual CARAPI GetUserActivityTimeout(
        /* [out] */ Int64* time) = 0;

    //@Override
    CARAPI DispatchKeyEvent(
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    /**
     * Allows the media keys to work when the keyguard is showing.
     * The media keys should be of no interest to the actual keyguard view(s),
     * so intercepting them here should not be of any harm.
     * @param event The key event
     * @return whether the event was consumed as a media key.
     */
    CARAPI InterceptMediaKey(
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI DispatchSystemUiVisibilityChanged(
        /* [in] */ Int32 visibility);

    CARAPI HandleMenuKey(
        /* [out] */ Boolean* result);

    CARAPI SetViewMediatorCallback(
        /* [in] */ IViewMediatorCallback* viewMediatorCallback);

    CARAPI ShowAssistant();

    CARAPI LaunchCamera();

    CARAPI SetLockPatternUtils(
        /* [in] */ ILockPatternUtils* utils);

    CARAPI GetSecurityMode(
        /* [out] */ SecurityMode* mose);

protected:
    //@Override
    CARAPI DispatchDraw(
        /* [in] */ ICanvas* canvas);

    //@Override
    CARAPI OnFinishInflate();

    CARAPI ShowBouncer(
        /* [in] */ Boolean show);

    CARAPI AnnounceCurrentSecurityMethod();

    CARAPI GetSecurityContainer(
        /* [out] */ IKeyguardSecurityContainer** container);

    CARAPI OnUserActivityTimeoutChanged();

    CARAPI GetActivityLauncher(
        /* [out] */ IKeyguardActivityLauncher** launcher);

    virtual CARAPI OnUserSwitching(
        /* [in] */ Boolean switching) = 0;

    virtual CARAPI OnCreateOptions(
        /* [in] */ IBundle* options) = 0;

    virtual CARAPI OnExternalMotionEvent(
        /* [in] */ IMotionEvent* event) = 0;

private:
    CARAPI_(void) HandleMediaKeyEvent(
        /* [in] */ IKeyEvent* keyEvent);

    CARAPI_(Boolean) ShouldEnableMenuKey();

public:
    static const Boolean DEBUG;

protected:
    AutoPtr<IViewMediatorCallback> mViewMediatorCallback;
    AutoPtr<ILockPatternUtils> mLockPatternUtils;

private:
    AutoPtr<IAudioManager> mAudioManager;
    AutoPtr<ITelephonyManager> mTelephonyManager;

    AutoPtr<IKeyguardHostViewOnDismissAction> mDismissAction;

    // Whether the volume keys should be handled by keyguard. If true, then
    // they will be handled here for specific media types such as music, otherwise
    // the audio service will bring up the volume dialog.
    static const Boolean KEYGUARD_MANAGES_VOLUME;

    AutoPtr<IKeyguardSecurityContainer> mSecurityContainer;

    /**
     * In general, we enable unlocking the insecure keyguard with the menu key. However, there are
     * some cases where we wish to disable it, notably when the menu button placement or technology
     * is prone to false positives.
     *
     * @return true if the menu key should be enabled
     */
    static const String ENABLE_MENU_KEY_FILE;

    AutoPtr<IKeyguardActivityLauncher> mActivityLauncher;
};

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_KEYGUARD_KEYGUARDVIEWBASE_H__