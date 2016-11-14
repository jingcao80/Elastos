
#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDPATTERNVIEW_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDPATTERNVIEW_H__

#include "_Elastos_Droid_SystemUI_Keyguard_CKeyguardPatternView.h"
#include "elastos/droid/systemui/keyguard/AppearAnimationUtils.h"
#include "elastos/droid/animation/AnimatorListenerAdapter.h"
#include <elastos/droid/widget/LinearLayout.h>
#include <elastos/core/Runnable.h>
#include "elastos/droid/os/CountDownTimer.h"
#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.View.h"

using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::IValueAnimator;
using Elastos::Droid::Animation::AnimatorListenerAdapter;
using Elastos::Droid::Animation::IAnimatorUpdateListener;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Internal::Widget::ILockPatternView;
using Elastos::Droid::Internal::Widget::ILockPatternUtils;
using Elastos::Droid::Internal::Widget::IOnPatternListener;
using Elastos::Droid::Internal::Widget::ILockPatternViewCellState;
using Elastos::Core::Runnable;
using Elastos::Droid::Os::ICountDownTimer;
using Elastos::Droid::Os::CountDownTimer;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::Animation::IInterpolator;
using Elastos::Droid::Widget::LinearLayout;
using Elastos::Core::IRunnable;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

CarClass(CKeyguardPatternView)
    , public LinearLayout
    , public IKeyguardSecurityView
    , public IAppearAnimationCreator
{
private:
    class UnlockPatternListener
        : public Object
        , public IOnPatternListener
    {
    public:
        TO_STRING_IMPL("CKeyguardPatternView::UnlockPatternListener")

        CAR_INTERFACE_DECL()

        UnlockPatternListener(
            /* [in] */ CKeyguardPatternView* host)
            : mHost(host)
        {}

        CARAPI OnPatternStart();

        CARAPI OnPatternCleared();

        CARAPI OnPatternCellAdded(
            /* [in] */ IList* pattern);

        CARAPI OnPatternDetected(
            /* [in] */ IList* pattern);

    private:
        CKeyguardPatternView* mHost;
    };

    class MyRunnable
        : public Runnable
    {
    public:
        TO_STRING_IMPL("CKeyguardPatternView::MyRunnable")

        MyRunnable(
            /* [in] */ CKeyguardPatternView* host)
            : mHost(host)
        {}

        CARAPI Run();

    private:
        CKeyguardPatternView* mHost;
    };

    class MyCountDownTimer
        : public CountDownTimer
    {
    public:
        TO_STRING_IMPL("CKeyguardPatternView::MyCountDownTimer")

        MyCountDownTimer(
            /* [in] */ CKeyguardPatternView* host,
            /* [in] */ Int64 elapsedRealtimeDeadline,
            /* [in] */ Int64 countDownInterval);

        //@Override
        CARAPI OnTick(
            /* [in] */ Int64 millisUntilFinished);

        //@Override
        CARAPI OnFinish();

    private:
        CKeyguardPatternView* mHost;
    };

    class MyRunnable2
        : public Runnable
    {
    public:
        TO_STRING_IMPL("CKeyguardPatternView::MyRunnable2")

        MyRunnable2(
            /* [in] */ CKeyguardPatternView* host)
            : mHost(host)
        {}

        CARAPI Run();

    private:
        CKeyguardPatternView* mHost;
    };

    class MyAnimatorUpdateListener
        : public Object
        , public IAnimatorUpdateListener
    {
    public:
        TO_STRING_IMPL("CKeyguardPatternView::MyAnimatorUpdateListener")

        CAR_INTERFACE_DECL()

        MyAnimatorUpdateListener(
            /* [in] */ CKeyguardPatternView* host,
            /* [in] */ ILockPatternViewCellState* animatedCell)
            : mHost(host)
            , mAnimatedCell(animatedCell)
        {}

        CARAPI OnAnimationUpdate(
            /* [in] */ IValueAnimator* animation);

    private:
        CKeyguardPatternView* mHost;
        AutoPtr<ILockPatternViewCellState> mAnimatedCell;
    };

    class MyAnimatorListenerAdapter
        : public AnimatorListenerAdapter
    {
    public:
        TO_STRING_IMPL("CKeyguardPatternView::MyAnimatorListenerAdapter")

        MyAnimatorListenerAdapter(
            /* [in] */ IRunnable* finishListener)
            : mFinishListener(finishListener)
        {}

        //@Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        AutoPtr<IRunnable> mFinishListener;
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CKeyguardPatternView();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI SetKeyguardCallback(
        /* [in] */ IKeyguardSecurityCallback* callback);

    CARAPI SetLockPatternUtils(
        /* [in] */ ILockPatternUtils* utils);

    //@Override
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* result);

    CARAPI Reset();

    //@Override
    CARAPI ShowUsabilityHint();

    /** TODO: hook this up */
    CARAPI CleanUp();

    //@Override
    CARAPI NeedsInput(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnPause();

    //@Override
    CARAPI OnResume(
        /* [in] */ Int32 reason);

    //@Override
    CARAPI GetCallback(
        /* [out] */ IKeyguardSecurityCallback** callback);

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

    //@Override
    CARAPI CreateAnimation(
        /* [in] */ IInterface* animatedCell,
        /* [in] */ Int64 delay,
        /* [in] */ Int64 duration,
        /* [in] */ Float startTranslationY,
        /* [in] */ IInterpolator* interpolator,
        /* [in] */ IRunnable* finishListener);

    //@Override
    CARAPI HasOverlappingRendering(
        /* [out] */ Boolean* result);

protected:
    //@Override
    CARAPI OnFinishInflate();

private:
    CARAPI_(void) DisplayDefaultSecurityMessage();

    CARAPI_(void) HandleAttemptLockout(
        /* [in] */ Int64 elapsedRealtimeDeadline);

    CARAPI_(void) EnableClipping(
        /* [in] */ Boolean enable);

private:
    static const String TAG;
    static const Boolean DEBUG;

    // how long before we clear the wrong pattern
    static const Int32 PATTERN_CLEAR_TIMEOUT_MS;

    // how long we stay awake after each key beyond MIN_PATTERN_BEFORE_POKE_WAKELOCK
    static const Int32 UNLOCK_PATTERN_WAKE_INTERVAL_MS;

    // how many cells the user has to cross before we poke the wakelock
    static const Int32 MIN_PATTERN_BEFORE_POKE_WAKELOCK;

    AutoPtr<IKeyguardUpdateMonitor> mKeyguardUpdateMonitor;
    AutoPtr<AppearAnimationUtils> mAppearAnimationUtils;

    AutoPtr<ICountDownTimer> mCountdownTimer;
    AutoPtr<ILockPatternUtils> mLockPatternUtils;
    AutoPtr<ILockPatternView> mLockPatternView;
    AutoPtr<IKeyguardSecurityCallback> mCallback;

    /**
     * Keeps track of the last time we poked the wake lock during dispatching of the touch event.
     * Initialized to something guaranteed to make us poke the wakelock when the user starts
     * drawing the pattern.
     * @see #dispatchTouchEvent(android.view.MotionEvent)
     */
    Int64 mLastPokeTime;

    /**
     * Useful for clearing out the wrong pattern after a delay
     */
    AutoPtr<IRunnable> mCancelPatternRunnable;
    AutoPtr<IRect> mTempRect;
    AutoPtr<ISecurityMessageDisplay> mSecurityMessageDisplay;
    AutoPtr<IView> mEcaView;
    AutoPtr<IDrawable> mBouncerFrame;
    AutoPtr<IViewGroup> mKeyguardBouncerFrame;
    AutoPtr<IKeyguardMessageArea> mHelpMessage;
    Int32 mDisappearYTranslation;

    enum FooterMode {
        Normal,
        ForgotLockPattern,
        VerifyUnlocked
    };
};

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDPATTERNVIEW_H__
