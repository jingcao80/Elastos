
#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDSECURITYCONTAINER_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDSECURITYCONTAINER_H__

#include "_Elastos_Droid_SystemUI_Keyguard_CKeyguardSecurityContainer.h"
#include <Elastos.Droid.Internal.h>
#include <elastos/droid/widget/FrameLayout.h>
#include "elastos/droid/systemui/keyguard/KeyguardUpdateMonitorCallback.h"

using Elastos::Droid::Widget::FrameLayout;
using Elastos::Droid::Internal::Widget::ILockPatternUtils;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

CarClass(CKeyguardSecurityContainer)
    , public FrameLayout
    , public IKeyguardSecurityContainer
    , public IKeyguardSecurityView
{
private:
    class MyKeyguardUpdateMonitorCallback
        : public KeyguardUpdateMonitorCallback
    {
    public:
        TO_STRING_IMPL("CKeyguardSecurityContainer::KeyguardUpdateMonitorCallback")

        MyKeyguardUpdateMonitorCallback(
            /* [in] */ CKeyguardSecurityContainer* host)
            : mHost(host)
        {}

        CARAPI OnUserSwitchComplete(
            /* [in] */ Int32 userId);
    private:
        CKeyguardSecurityContainer* mHost;
    };

    class MyKeyguardSecurityCallback
        : public Object
        , public IKeyguardSecurityCallback
    {
    public:
        TO_STRING_IMPL("CKeyguardSecurityContainer::MyKeyguardSecurityCallback")

        CAR_INTERFACE_DECL()

        MyKeyguardSecurityCallback(
            /* [in] */ CKeyguardSecurityContainer* host)
            : mHost(host)
        {}

        CARAPI UserActivity();

        CARAPI Dismiss(
            /* [in] */ Boolean authenticated);

        CARAPI IsVerifyUnlockOnly(
            /* [out] */ Boolean* result);

        CARAPI ReportUnlockAttempt(
            /* [in] */ Boolean success);

        //@Override
        CARAPI ShowBackupSecurity();

    private:
        CKeyguardSecurityContainer* mHost;
    };

    class NullKeyguardSecurityCallback
        : public Object
        , public IKeyguardSecurityCallback
    {
    public:
        TO_STRING_IMPL("CKeyguardSecurityContainer::NullKeyguardSecurityCallback")

        CAR_INTERFACE_DECL()

        NullKeyguardSecurityCallback(
            /* [in] */ CKeyguardSecurityContainer* host)
            : mHost(host)
        {}

        CARAPI UserActivity();

        CARAPI Dismiss(
            /* [in] */ Boolean authenticated);

        CARAPI IsVerifyUnlockOnly(
            /* [out] */ Boolean* result);

        CARAPI ReportUnlockAttempt(
            /* [in] */ Boolean success);

        //@Override
        CARAPI ShowBackupSecurity();

    private:
        CKeyguardSecurityContainer* mHost;
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CKeyguardSecurityContainer();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr);

    CARAPI SetSecurityCallback(
        /* [in] */ IKeyguardSecurityContainerSecurityCallback* callback);

    CARAPI OnAttachedToWindow();

    CARAPI OnDetachedFromWindow();

    //@Override
    CARAPI OnResume(
        /* [in] */ Int32 reason);

    //@Override
    CARAPI OnPause();

    CARAPI StartAppearAnimation();

    CARAPI StartDisappearAnimation(
        /* [in] */ IRunnable* onFinishRunnable,
        /* [out] */ Boolean* result);

    CARAPI UpdateSecurityViews(
        /* [in] */ Boolean isBouncing);

    CARAPI AnnounceCurrentSecurityMethod();

    CARAPI SetLockPatternUtils(
        /* [in] */ ILockPatternUtils* utils);

    /**
     * Shows the primary security screen for the user. This will be either the multi-selector
     * or the user's security method.
     * @param turningOff true if the device is being turned off
     */
    CARAPI ShowPrimarySecurityScreen(
        /* [in] */ Boolean turningOff);

    /**
     * Shows the next security screen if there is one.
     * @param authenticated true if the user entered the correct authentication
     * @param authenticated
     * @return true if keyguard is done
     */
    CARAPI ShowNextSecurityScreenOrFinish(
        /* [in] */ Boolean authenticated,
        /* [out] */ Boolean* result);

    CARAPI ShowBouncer(
        /* [in] */ Int32 duration);

    CARAPI HideBouncer(
        /* [in] */ Int32 duration);

    CARAPI GetSecurityMode(
        /* [out] */ SecurityMode* mode);

    CARAPI VerifyUnlock();

    CARAPI GetCurrentSecuritySelection(
        /* [out] */ SecurityMode* mode);

    CARAPI Dismiss(
        /* [in] */ Boolean authenticated);

    CARAPI NeedsInput(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI SetKeyguardCallback(
        /* [in] */ IKeyguardSecurityCallback* callback);

    //@Override
    CARAPI Reset();

    //@Override
    CARAPI GetCallback(
        /* [in] */ IKeyguardSecurityCallback** callback);

    //@Override
    CARAPI ShowUsabilityHint();

protected:
    CARAPI OnFinishInflate();

private:
    CARAPI GetSecurityView(
        /* [in] */ SecurityMode securityMode,
        /* [out] */ IKeyguardSecurityView** view);

    CARAPI_(void) UpdateSecurityView(
        /* [in] */ IView* view,
        /* [in] */ Boolean isBouncing);

    CARAPI_(void) ShowDialog(
        /* [in] */ const String& title,
        /* [in] */ const String& message);

    CARAPI_(void) ShowTimeoutDialog();

    CARAPI_(void) ShowAlmostAtWipeDialog(
        /* [in] */ Int32 attempts,
        /* [in] */ Int32 remaining);

    CARAPI_(void) ShowWipeDialog(
        /* [in] */ Int32 attempts);

    CARAPI_(void) ShowAlmostAtAccountLoginDialog();

    CARAPI_(void) ReportFailedUnlockAttempt();

    /**
     * Shows the backup security screen for the current security mode.  This could be used for
     * password recovery screens but is currently only used for pattern unlock to show the
     * account unlock screen and biometric unlock to show the user's normal unlock.
     */
    CARAPI ShowBackupSecurityScreen();

    /**
     * Switches to the given security view unless it's already being shown, in which case
     * this is a no-op.
     *
     * @param securityMode
     */
    CARAPI ShowSecurityScreen(
        /* [in] */ SecurityMode securityMode);

    CARAPI_(AutoPtr<IKeyguardSecurityViewFlipper>) GetFlipper();

    CARAPI_(Int32) GetSecurityViewIdForMode(
        /* [in] */ SecurityMode securityMode);

    CARAPI_(Int32) GetLayoutIdFor(
        /* [in] */ SecurityMode securityMode);

private:
    static const Boolean DEBUG;

    AutoPtr<IKeyguardSecurityModel> mSecurityModel;
    Boolean mEnableFallback; // TODO: This should get the value from KeyguardPatternView
    AutoPtr<ILockPatternUtils> mLockPatternUtils;

    AutoPtr<IKeyguardSecurityViewFlipper> mSecurityViewFlipper;
    Boolean mIsVerifyUnlockOnly;
    SecurityMode mCurrentSecuritySelection;
    Boolean mIsBouncing;
    AutoPtr<IKeyguardSecurityContainerSecurityCallback> mSecurityCallback;

    AutoPtr<IKeyguardUpdateMonitor> mUpdateMonitor;
    AutoPtr<IKeyguardUpdateMonitorCallback> mUpdateMonitorCallbacks;

    AutoPtr<IKeyguardSecurityCallback> mCallback;

    // The following is used to ignore callbacks from SecurityViews that are no longer current
    // (e.g. face unlock). This avoids unwanted asynchronous events from messing with the
    // state for the current security method.
    AutoPtr<IKeyguardSecurityCallback> mNullCallback;
};

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDSECURITYCONTAINER_H__
