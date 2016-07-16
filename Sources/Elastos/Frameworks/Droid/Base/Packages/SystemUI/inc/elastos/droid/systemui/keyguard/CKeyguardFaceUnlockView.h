
#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDFACEUNLOCKVIEW_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDFACEUNLOCKVIEW_H__

#include "_Elastos_Droid_SystemUI_Keyguard_CKeyguardFaceUnlockView.h"
#include "elastos/droid/widget/LinearLayout.h"

using Elastos::Droid::Widget::LinearLayout;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

CarClass(CKeyguardFaceUnlockView)
    , public LinearLayout
    , public IKeyguardSecurityView
{
private:
    class MyKeyguardUpdateMonitorCallback
        : public KeyguardUpdateMonitorCallback
    {
    public:
        TO_STRING_IMPL("CKeyguardFaceUnlockView::MyKeyguardUpdateMonitorCallback")

        MyKeyguardUpdateMonitorCallback(
            /* [in] */ CKeyguardFaceUnlockView* host)
            : mHost(host)
        {}

        // We need to stop the biometric unlock when a phone call comes in
        //@Override
        CARAPI OnPhoneStateChanged(
            /* [in] */ Int32 phoneState);

        //@Override
        CARAPI OnUserSwitching(
            /* [in] */ Int32 userId);

        //@Override
        CARAPI OnUserSwitchComplete(
            /* [in] */ Int32 userId);

        //@Override
        CARAPI OnKeyguardVisibilityChanged(
            /* [in] */ Boolean showing);

        //@Override
        CARAPI OnKeyguardBouncerChanged(
            /* [in] */ Boolean bouncer);

        //@Override
        CARAPI OnScreenTurnedOn();

        //@Override
        CARAPI OnScreenTurnedOff(
            /* [in] */ Int32 why);

        //@Override
        CARAPI OnEmergencyCallAction();

    private:
        CKeyguardFaceUnlockView* mHost;
    };

    class MyOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        TO_STRING_IMPL("CKeyguardFaceUnlockView::MyOnClickListener")

        MyOnClickListener(
            /* [in] */ CKeyguardFaceUnlockView* host)
            : mHost(host)
        {}

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CKeyguardFaceUnlockView* mHost;
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CKeyguardFaceUnlockView();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    //@Override
    CARAPI SetKeyguardCallback(
        /* [in] */ IKeyguardSecurityCallback* callback);

    //@Override
    CARAPI SetLockPatternUtils(
        /* [in] */ ILockPatternUtils* utils);

    //@Override
    CARAPI Reset();

    //@Override
    CARAPI OnDetachedFromWindow();

    //@Override
    CARAPI OnPause();

    //@Override
    CARAPI OnResume(
        /* [in] */ Int32 reason);

    //@Override
    CARAPI NeedsInput(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI GetCallback(
        /* [out] */ IKeyguardSecurityCallback** callback);

    //@Override
    CARAPI ShowUsabilityHint();

    //@Override
    CARAPI ShowBouncer(
        /* [in] */ Int32 duration);

    //@Override
    CARAPI HideBouncer(
        /* [in] */ Int32 duration);

    //@Override
    CARAPI StartAppearAnimation();

    //@Override
    CARAPI StartDisappearAnimation(
        /* [in] */ IRunnable* finishRunnable,
        /* [out] */ Boolean* result);

protected:
    //@Override
    CARAPI OnFinishInflate();

    //@Override
    CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 l,
        /* [in] */ Int32 t,
        /* [in] */ Int32 r,
        /* [in] */ Int32 b);

private:
    CARAPI_(void) InitializeBiometricUnlockView();

    /**
     * Starts the biometric unlock if it should be started based on a number of factors.  If it
     * should not be started, it either goes to the back up, or remains showing to prepare for
     * it being started later.
     */
    CARAPI_(void) MaybeStartBiometricUnlock();

    // Returns true if the device is currently in a state where the user is seeing the bouncer.
    // This requires isKeyguardBouncer() to be true, but that doesn't imply that the screen is on or
    // the keyguard visibility is set to true, so we must check those conditions as well.
    CARAPI_(Boolean) IsBouncerVisibleToUser();

    // Starts the biometric unlock if the bouncer was not previously visible to the user, but is now
    // visibile to the user.  Stops the biometric unlock if the bouncer was previously visible to
    // the user, but is no longer visible to the user.
    CARAPI_(void) HandleBouncerUserVisibilityChanged();

private:
    static const String TAG;
    static const Boolean DEBUG;
    AutoPtr<IKeyguardSecurityCallback> mKeyguardSecurityCallback;
    AutoPtr<ILockPatternUtils> mLockPatternUtils;
    AutoPtr<IBiometricSensorUnlock> mBiometricUnlock;
    AutoPtr<IView> mFaceUnlockAreaView;
    AutoPtr<IImageButton> mCancelButton;
    AutoPtr<ISecurityMessageDisplay> mSecurityMessageDisplay;
    AutoPtr<IView> mEcaView;
    AutoPtr<IDrawable> mBouncerFrame;

    Boolean mIsBouncerVisibleToUser;
    Object mIsBouncerVisibleToUserLock;

    Int32 mLastRotation;
    Boolean mWatchingRotation;
    AutoPtr<IIWindowManager> mWindowManager;

    AutoPtr<IIRotationWatcher> mRotationWatcher;

    AutoPtr<IKeyguardUpdateMonitorCallback> mUpdateCallback;
};

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDFACEUNLOCKVIEW_H__
