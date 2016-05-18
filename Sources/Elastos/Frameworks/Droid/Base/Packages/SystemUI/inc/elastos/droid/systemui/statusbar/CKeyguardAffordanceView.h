
#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CKEYGUARDAFFORDANCEVIEW_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CKEYGUARDAFFORDANCEVIEW_H__

#include "_Elastos_Droid_SystemUI_StatusBar_CKeyguardAffordanceView.h"
#include "elastos/droid/systemui/statusbar/FlingAnimationUtils.h"
#include "Elastos.Droid.Animation.h"
#include <elastos/droid/animation/AnimatorListenerAdapter.h>
#include <elastos/droid/widget/ImageView.h>

using Elastos::Droid::Animation::AnimatorListenerAdapter;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::IAnimatorListener;
using Elastos::Droid::Animation::IAnimatorUpdateListener;
using Elastos::Droid::Animation::IArgbEvaluator;
using Elastos::Droid::Animation::IValueAnimator;
using Elastos::Droid::View::Animation::IInterpolator;
using Elastos::Droid::Widget::ImageView;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

/**
 * An ImageView which does not have overlapping renderings commands and therefore does not need a
 * layer when alpha is changed.
 */
CarClass(CKeyguardAffordanceView)
    , public ImageView
    , public IKeyguardAffordanceView
{
private:
    class ClipEndListener: public AnimatorListenerAdapter
    {
    public:
        ClipEndListener(
            /* [in] */ CKeyguardAffordanceView* host);

        // @Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        CKeyguardAffordanceView* mHost;
    };

    class CircleEndListener: public AnimatorListenerAdapter
    {
    public:
        CircleEndListener(
            /* [in] */ CKeyguardAffordanceView* host);

        // @Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        CKeyguardAffordanceView* mHost;
    };

    class ScaleEndListener: public AnimatorListenerAdapter
    {
    public:
        ScaleEndListener(
            /* [in] */ CKeyguardAffordanceView* host);

        // @Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        CKeyguardAffordanceView* mHost;
    };

    class AlphaEndListener: public AnimatorListenerAdapter
    {
    public:
        AlphaEndListener(
            /* [in] */ CKeyguardAffordanceView* host);

        // @Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        CKeyguardAffordanceView* mHost;
    };

    class ArrowEndListener: public AnimatorListenerAdapter
    {
    public:
        ArrowEndListener(
            /* [in] */ CKeyguardAffordanceView* host);

        // @Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        CKeyguardAffordanceView* mHost;
    };

    class AnimatorListenerAdapter1: public AnimatorListenerAdapter
    {
    public:
        AnimatorListenerAdapter1(
            /* [in] */ IRunnable* runnable);

        // @Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        AutoPtr<IRunnable> mAnimationEndRunnable;
    };

    class AnimatorListenerAdapter2: public AnimatorListenerAdapter
    {
    public:
        AnimatorListenerAdapter2(
            /* [in] */ CKeyguardAffordanceView* host);

        // @Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        CKeyguardAffordanceView* mHost;
    };

    class AnimatorListenerAdapter3: public AnimatorListenerAdapter
    {
    public:
        AnimatorListenerAdapter3(
            /* [in] */ IRunnable* runnable);

        // @Override
        CARAPI OnAnimationCancel(
            /* [in] */ IAnimator* animation);

        // @Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        AutoPtr<IRunnable> mRunnable;
        Boolean mCancelled;
    };

    class AnimatorUpdateListener
        : public Object
        , public IAnimatorUpdateListener
    {
    public:
        CAR_INTERFACE_DECL()

        AnimatorUpdateListener(
            /* [in] */ CKeyguardAffordanceView* host);

        // @Override
        CARAPI OnAnimationUpdate(
            /* [in] */ IValueAnimator* animation);

    private:
        CKeyguardAffordanceView* mHost;
    };

    class AnimatorUpdateListener2
        : public Object
        , public IAnimatorUpdateListener
    {
    public:
        CAR_INTERFACE_DECL()

        AnimatorUpdateListener2(
            /* [in] */ CKeyguardAffordanceView* host);

        // @Override
        CARAPI OnAnimationUpdate(
            /* [in] */ IValueAnimator* animation);

    private:
        CKeyguardAffordanceView* mHost;
    };

    class AnimatorUpdateListener3
        : public Object
        , public IAnimatorUpdateListener
    {
    public:
        CAR_INTERFACE_DECL()

        AnimatorUpdateListener3(
            /* [in] */ CKeyguardAffordanceView* host,
            /* [in] */ IDrawable* background);

        // @Override
        CARAPI OnAnimationUpdate(
            /* [in] */ IValueAnimator* animation);

    private:
        CKeyguardAffordanceView* mHost;
        AutoPtr<IDrawable> mBackground;
    };

    class AnimatorUpdateListener4
        : public Object
        , public IAnimatorUpdateListener
    {
    public:
        CAR_INTERFACE_DECL()

        AnimatorUpdateListener4(
            /* [in] */ CKeyguardAffordanceView* host);

        // @Override
        CARAPI OnAnimationUpdate(
            /* [in] */ IValueAnimator* animation);

    private:
        CKeyguardAffordanceView* mHost;
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CKeyguardAffordanceView();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

    CARAPI SetPreviewView(
        /* [in] */ IView* v);

    CARAPI FinishAnimation(
        /* [in] */ Float velocity,
        /* [in] */ IRunnable* mAnimationEndRunnable);

    CARAPI SetCircleRadius(
        /* [in] */ Float circleRadius);

    CARAPI SetCircleRadius(
        /* [in] */ Float circleRadius,
        /* [in] */ Boolean slowAnimation);

    CARAPI SetCircleRadiusWithoutAnimation(
        /* [in] */ Float circleRadius);

    CARAPI SetImageScale(
        /* [in] */ Float imageScale,
        /* [in] */ Boolean animate);

    /**
     * Sets the scale of the containing image
     *
     * @param imageScale The new Scale.
     * @param animate Should an animation be performed
     * @param duration If animate, whats the duration? When -1 we take the default duration
     * @param interpolator If animate, whats the interpolator? When NULL we take the default
     *                     interpolator.
     */
    CARAPI SetImageScale(
        /* [in] */ Float imageScale,
        /* [in] */ Boolean animate,
        /* [in] */ Int64 duration,
        /* [in] */ IInterpolator* interpolator);

    using ImageView::SetImageAlpha;
    CARAPI SetImageAlpha(
        /* [in] */ Float alpha,
        /* [in] */ Boolean animate);

    /**
     * Sets the alpha of the containing image
     *
     * @param alpha The new alpha.
     * @param animate Should an animation be performed
     * @param duration If animate, whats the duration? When -1 we take the default duration
     * @param interpolator If animate, whats the interpolator? When NULL we take the default
     *                     interpolator.
     */
    CARAPI SetImageAlpha(
        /* [in] */ Float alpha,
        /* [in] */ Boolean animate,
        /* [in] */ Int64 duration,
        /* [in] */ IInterpolator* interpolator,
        /* [in] */ IRunnable* runnable);

    CARAPI GetCircleRadius(
        /* [out] */ Float* result);

    CARAPI ShowArrow(
        /* [in] */ Boolean show);

    CARAPI SetIsLeft(
        /* [in] */ Boolean left);

    // @Override
    CARAPI PerformClick(
        /* [out] */ Boolean* result);

protected:
    // @Override
    CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    // @Override
    CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas);

private:
    CARAPI_(void) DrawArrow(
        /* [in] */ ICanvas* canvas);

    CARAPI_(void) UpdateIconColor();

    CARAPI_(void) DrawBackgroundCircle(
        /* [in] */ ICanvas* canvas);

    CARAPI_(void) UpdateCircleColor();

    CARAPI_(Float) GetMaxCircleSize();

    CARAPI_(void) SetCircleRadius(
        /* [in] */ Float circleRadius,
        /* [in] */ Boolean slowAnimation,
        /* [in] */ Boolean noAnimation);

    CARAPI_(AutoPtr<IValueAnimator>) GetAnimatorToRadius(
        /* [in] */ Float circleRadius);

    CARAPI_(void) CancelAnimator(
        /* [in] */ IAnimator* animator);

    CARAPI_(AutoPtr<IAnimatorListener>) GetEndListener(
        /* [in] */ IRunnable* runnable);

public:
    static const Float MAX_ICON_SCALE_AMOUNT;
    static const Float MIN_ICON_SCALE_AMOUNT;

private:
    static const Int64 CIRCLE_APPEAR_DURATION;
    static const Int64 CIRCLE_DISAPPEAR_MAX_DURATION;
    static const Int64 NORMAL_ANIMATION_DURATION;

    Int32 mMinBackgroundRadius;
    AutoPtr<IPaint> mCirclePaint;
    AutoPtr<IInterpolator> mAppearInterpolator;
    AutoPtr<IInterpolator> mDisappearInterpolator;
    Int32 mInverseColor;
    Int32 mNormalColor;
    AutoPtr<IArgbEvaluator> mColorInterpolator;
    AutoPtr<FlingAnimationUtils> mFlingAnimationUtils;
    AutoPtr<IDrawable> mArrowDrawable;
    Int32 mHintChevronPadding;
    Float mCircleRadius;
    Int32 mCenterX;
    Int32 mCenterY;
    AutoPtr<IValueAnimator> mCircleAnimator;
    AutoPtr<IValueAnimator> mAlphaAnimator;
    AutoPtr<IValueAnimator> mScaleAnimator;
    AutoPtr<IValueAnimator> mArrowAnimator;
    Float mCircleStartValue;
    Boolean mCircleWillBeHidden;
    AutoPtr<ArrayOf<Int32> > mTempPoint;
    Float mImageScale;
    Int32 mCircleColor;
    Boolean mIsLeft;
    Float mArrowAlpha;
    AutoPtr<IView> mPreviewView;
    Float mCircleStartRadius;
    Float mMaxCircleSize;
    AutoPtr<IAnimator> mPreviewClipper;
    AutoPtr<AnimatorListenerAdapter> mClipEndListener;
    AutoPtr<AnimatorListenerAdapter> mCircleEndListener;
    AutoPtr<AnimatorListenerAdapter> mScaleEndListener;
    AutoPtr<AnimatorListenerAdapter> mAlphaEndListener;
    AutoPtr<AnimatorListenerAdapter> mArrowEndListener;
};

} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CKEYGUARDAFFORDANCEVIEW_H__
