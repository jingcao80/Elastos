
#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_KEYGUARDAFFORDANCEHELPER_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_KEYGUARDAFFORDANCEHELPER_H__

#include "_Elastos.Droid.SystemUI.h"
#include "elastos/droid/systemui/statusbar/FlingAnimationUtils.h"
#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include <elastos/droid/animation/AnimatorListenerAdapter.h>
#include <elastos/droid/os/Runnable.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Animation::AnimatorListenerAdapter;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::IAnimatorUpdateListener;
using Elastos::Droid::Animation::IValueAnimator;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::View::Animation::IInterpolator;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IVelocityTracker;
using Elastos::Droid::Content::IContext;
using Elastos::Core::IRunnable;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

/**
 * A touch handler of the keyguard which is responsible for launching phone and camera affordances.
 */
class KeyguardAffordanceHelper: public Object
{
private:
    class FlingEndListener: public AnimatorListenerAdapter
    {
    public:
        FlingEndListener(
            /* [in] */ KeyguardAffordanceHelper* host);

        // @Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        KeyguardAffordanceHelper* mHost;
    };

    class AnimationEndRunnable: public Runnable
    {
    public:
        AnimationEndRunnable(
            /* [in] */ KeyguardAffordanceHelper* host);

        // @Override
        CARAPI Run();

    private:
        KeyguardAffordanceHelper* mHost;
    };

    class AnimatorListenerAdapter1: public AnimatorListenerAdapter
    {
    public:
        AnimatorListenerAdapter1(
            /* [in] */ KeyguardAffordanceHelper* host,
            /* [in] */ IRunnable* onFinishedListener,
            /* [in] */ IKeyguardAffordanceView* targetView,
            /* [in] */ Boolean right);

        // @Override
        CARAPI OnAnimationCancel(
            /* [in] */ IAnimator* animation);

        // @Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        KeyguardAffordanceHelper* mHost;
        AutoPtr<IRunnable> mOnFinishedListener;
        AutoPtr<IKeyguardAffordanceView> mTargetView;
        Boolean mRight;
        Boolean mCancelled;
    };

    class AnimatorListenerAdapter2: public AnimatorListenerAdapter
    {
    public:
        AnimatorListenerAdapter2(
            /* [in] */ KeyguardAffordanceHelper* host,
            /* [in] */ IKeyguardAffordanceView* targetView,
            /* [in] */ IRunnable* onFinishedListener);

        // @Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

        // @Override
        CARAPI OnAnimationStart(
            /* [in] */ IAnimator* animation);

    private:
        KeyguardAffordanceHelper* mHost;
        AutoPtr<IKeyguardAffordanceView> mTargetView;
        AutoPtr<IRunnable> mOnFinishedListener;
    };

    class AnimatorUpdateListener1
        : public Object
        , public IAnimatorUpdateListener
    {
    public:
        CAR_INTERFACE_DECL()

        AnimatorUpdateListener1(
            /* [in] */ KeyguardAffordanceHelper* host,
            /* [in] */ IKeyguardAffordanceView* targetView,
            /* [in] */ Boolean right);

        // @Override
        CARAPI OnAnimationUpdate(
            /* [in] */ IValueAnimator* animation);

    private:
        KeyguardAffordanceHelper* mHost;
        AutoPtr<IKeyguardAffordanceView> mTargetView;
        Boolean mRight;
    };

    class AnimatorUpdateListener2
        : public Object
        , public IAnimatorUpdateListener
    {
    public:
        CAR_INTERFACE_DECL()

        AnimatorUpdateListener2(
            /* [in] */ KeyguardAffordanceHelper* host);

        // @Override
        CARAPI OnAnimationUpdate(
            /* [in] */ IValueAnimator* animation);

    private:
        KeyguardAffordanceHelper* mHost;
    };

public:
    KeyguardAffordanceHelper(
        /* [in] */ IKeyguardAffordanceHelperCallback* callback,
        /* [in] */ IContext* context);

    CARAPI_(Boolean) OnTouchEvent(
        /* [in] */ IMotionEvent* event);

    CARAPI_(Boolean) OnInterceptTouchEvent(
        /* [in] */ IMotionEvent* ev);

    CARAPI_(void) StartHintAnimation(
        /* [in] */ Boolean right,
        /* [in] */ IRunnable* onFinishedListener);

    CARAPI_(void) AnimateHideLeftRightIcon();

    CARAPI_(void) OnConfigurationChanged();

    CARAPI_(void) Reset(
        /* [in] */ Boolean animate);

private:
    CARAPI_(void) InitDimens();

    CARAPI_(void) InitIcons();

    CARAPI_(void) EndMotion(
        /* [in] */ IMotionEvent* event,
        /* [in] */ Boolean forceSnapBack);

    CARAPI_(void) SetSwipingInProgress(
        /* [in] */ Boolean inProgress);

    CARAPI_(Boolean) RightSwipePossible();

    CARAPI_(Boolean) LeftSwipePossible();

    CARAPI_(void) StartHintAnimationPhase1(
        /* [in] */ Boolean right,
        /* [in] */ IRunnable* onFinishedListener);

    /**
     * Phase 2: Move back.
     */
    CARAPI_(void) StartUnlockHintAnimationPhase2(
        /* [in] */ Boolean right,
        /* [in] */ IRunnable* onFinishedListener);

    CARAPI_(AutoPtr<IValueAnimator>) GetAnimatorToRadius(
        /* [in] */ Boolean right,
        /* [in] */ Int32 radius);

    CARAPI_(void) CancelAnimation();

    CARAPI_(void) FlingWithCurrentVelocity(
        /* [in] */ Boolean forceSnapBack);

    CARAPI_(Boolean) IsBelowFalsingThreshold();

    CARAPI_(Int32) GetMinTranslationAmount();

    CARAPI_(void) Fling(
        /* [in] */ Float vel,
        /* [in] */ Boolean snapBack);

    CARAPI_(void) StartFinishingCircleAnimation(
        /* [in] */ Float velocity,
        /* [in] */ IRunnable* mAnimationEndRunnable);

    CARAPI_(void) SetTranslation(
        /* [in] */ Float translation,
        /* [in] */ Boolean isReset,
        /* [in] */ Boolean animateReset);

    CARAPI_(void) UpdateIconsFromRadius(
        /* [in] */ IKeyguardAffordanceView* targetView,
        /* [in] */ Float newRadius);

    CARAPI_(Float) GetTranslationFromRadius(
        /* [in] */ Float circleSize);

    CARAPI_(Float) GetRadiusFromTranslation(
        /* [in] */ Float translation);

    CARAPI_(void) UpdateIcon(
        /* [in] */ IKeyguardAffordanceView* view,
        /* [in] */ Float circleRadius,
        /* [in] */ Float alpha,
        /* [in] */ Boolean animate,
        /* [in] */ Boolean slowRadiusAnimation);

    CARAPI_(void) UpdateIconAlpha(
        /* [in] */ IKeyguardAffordanceView* view,
        /* [in] */ Float alpha,
        /* [in] */ Boolean animate);

    CARAPI_(Float) GetScale(
        /* [in] */ Float alpha);

    CARAPI_(void) TrackMovement(
        /* [in] */ IMotionEvent* event);

    CARAPI_(void) InitVelocityTracker();

    CARAPI_(Float) GetCurrentVelocity();

public:
    static const Float SWIPE_RESTING_ALPHA_AMOUNT;
    static const Int64 HINT_PHASE1_DURATION;

private:
    static const Int64 HINT_PHASE2_DURATION;
    static const Float BACKGROUND_RADIUS_SCALE_FACTOR;
    static const Int32 HINT_CIRCLE_OPEN_DURATION;

    AutoPtr<IContext> mContext;

    AutoPtr<FlingAnimationUtils> mFlingAnimationUtils;
    AutoPtr<IKeyguardAffordanceHelperCallback> mCallback;
    AutoPtr<IVelocityTracker> mVelocityTracker;
    Boolean mSwipingInProgress;
    Float mInitialTouchX;
    Float mInitialTouchY;
    Float mTranslation;
    Float mTranslationOnDown;
    Int32 mTouchSlop;
    Int32 mMinTranslationAmount;
    Int32 mMinFlingVelocity;
    Int32 mHintGrowAmount;
    AutoPtr<IKeyguardAffordanceView> mLeftIcon;
    AutoPtr<IKeyguardAffordanceView> mCenterIcon;
    AutoPtr<IKeyguardAffordanceView> mRightIcon;
    AutoPtr<IInterpolator> mAppearInterpolator;
    AutoPtr<IInterpolator> mDisappearInterpolator;
    AutoPtr<IAnimator> mSwipeAnimator;
    Int32 mMinBackgroundRadius;
    Boolean mMotionPerformedByUser;
    Boolean mMotionCancelled;
    AutoPtr<AnimatorListenerAdapter> mFlingEndListener;

    AutoPtr<IRunnable> mAnimationEndRunnable;
};

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_KEYGUARDAFFORDANCEHELPER_H__
