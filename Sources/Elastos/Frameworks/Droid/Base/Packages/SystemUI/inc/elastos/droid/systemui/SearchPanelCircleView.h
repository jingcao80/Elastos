
#ifndef  __ELASTOS_DROID_SYSTEMUI_SEARCHPANELCIRCLEVIEW_H__
#define  __ELASTOS_DROID_SYSTEMUI_SEARCHPANELCIRCLEVIEW_H__

#include "_SystemUI.h"
#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "Elastos.CoreLibrary.Core.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/animation/AnimatorListenerAdapter.h"
#include "elastos/droid/view/ViewOutlineProvider.h"
#include "elastos/droid/widget/FrameLayout.h"

using Elastos::Droid::Animation::AnimatorListenerAdapter;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::IAnimatorUpdateListener;
using Elastos::Droid::Animation::IValueAnimator;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IOutline;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::ViewOutlineProvider;
using Elastos::Droid::View::Animation::IInterpolator;
using Elastos::Droid::Widget::FrameLayout;
using Elastos::Droid::Widget::IImageView;
using Elastos::Core::IRunnable;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace SystemUI {

class SearchPanelCircleView
    : public FrameLayout
    , public ISearchPanelCircleView
{
public:
    class Ripple
        : public Object
    {
    private:
        class MyAnimatorUpdateListener
            : public Object
            , public IAnimatorUpdateListener
        {
        public:
            CAR_INTERFACE_DECL()

            MyAnimatorUpdateListener(
                /* [in] */ Ripple* host);

            //@Override
            CARAPI OnAnimationUpdate(
                /* [in] */ IValueAnimator* animation);

        private:
            Ripple* mHost;
        };

        class MyAnimatorListenerAdapter
            : public AnimatorListenerAdapter
        {
        public:
            MyAnimatorListenerAdapter(
                /* [in] */ Ripple* host);

            //@Override
            CARAPI OnAnimationEnd(
                /* [in] */ IAnimator* animation);

            CARAPI OnAnimationStart(
                /* [in] */ IAnimator* animation);

        private:
            Ripple* mHost;
        };

    public:
        Ripple(
            /* [in] */ Float x,
            /* [in] */ Float y,
            /* [in] */ Float endRadius,
            /* [in] */ SearchPanelCircleView* host);

        CARAPI_(void) Start();

        CARAPI_(void) Draw(
            /* [in] */ ICanvas* canvas);

    public:
        Float mX;
        Float mY;
        Float mRadius;
        Float mEndRadius;
        Float mAlpha;

    private:
        SearchPanelCircleView* mHost;
    };

private:
    class CircleUpdateListener
        : public Object
        , public IAnimatorUpdateListener
    {
    public:
        CAR_INTERFACE_DECL()

        CircleUpdateListener(
            /* [in] */ SearchPanelCircleView* host);

        //@Override
        CARAPI OnAnimationUpdate(
            /* [in] */ IValueAnimator* animation);

    private:
        SearchPanelCircleView* mHost;
    };

    class ClearAnimatorListener
        : public AnimatorListenerAdapter
    {
    public:
        ClearAnimatorListener(
            /* [in] */ SearchPanelCircleView* host);

        //@Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        SearchPanelCircleView* mHost;
    };

    class OffsetUpdateListener
        : public Object
        , public IAnimatorUpdateListener
    {
    public:
        CAR_INTERFACE_DECL()

        OffsetUpdateListener(
            /* [in] */ SearchPanelCircleView* host);

        //@Override
        CARAPI OnAnimationUpdate(
            /* [in] */ IValueAnimator* animation);

    private:
        SearchPanelCircleView* mHost;
    };

    class MyViewOutlineProvider
        : public ViewOutlineProvider
    {
    public:
        MyViewOutlineProvider(
            /* [in] */ SearchPanelCircleView* host);

        // @Override
        CARAPI GetOutline(
            /* [in] */ IView* view,
            /* [in] */ IOutline* outline);

    private:
        SearchPanelCircleView* mHost;
    };

    class MyAnimatorListenerAdapter2
        : public AnimatorListenerAdapter
    {
    public:
        MyAnimatorListenerAdapter2(
            /* [in] */ SearchPanelCircleView* host,
            /* [in] */ IRunnable* endRunnable);

        //@Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        SearchPanelCircleView* mHost;
        const AutoPtr<IRunnable> mEndRunnable;
    };

    class MyAnimatorListenerAdapter3
        : public AnimatorListenerAdapter
    {
    public:
        MyAnimatorListenerAdapter3(
            /* [in] */ SearchPanelCircleView* host,
            /* [in] */ IRunnable* endRunnable);

        //@Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        SearchPanelCircleView* mHost;
        const AutoPtr<IRunnable> mEndRunnable;
    };

    class MyAnimatorListenerAdapter4
        : public AnimatorListenerAdapter
    {
    public:
        MyAnimatorListenerAdapter4(
            /* [in] */ SearchPanelCircleView* host,
            /* [in] */ IRunnable* runnable);

        //@Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        SearchPanelCircleView* mHost;
        const AutoPtr<IRunnable> mRunnable;
    };

    class MyRunnable
        : public Runnable
    {
    public:
        MyRunnable(
            /* [in] */ SearchPanelCircleView* host);

        // @Override
        CARAPI Run();

    private:
        SearchPanelCircleView* mHost;
    };

    class MyAnimatorUpdateListener2
        : public Object
        , public IAnimatorUpdateListener
    {
    public:
        CAR_INTERFACE_DECL()

        MyAnimatorUpdateListener2(
            /* [in] */ SearchPanelCircleView* host);

        //@Override
        CARAPI OnAnimationUpdate(
            /* [in] */ IValueAnimator* animation);

    private:
        SearchPanelCircleView* mHost;
    };

    class MyAnimatorListenerAdapter5
        : public AnimatorListenerAdapter
    {
    public:
        MyAnimatorListenerAdapter5(
            /* [in] */ SearchPanelCircleView* host,
            /* [in] */ IRunnable* endRunnable);

        //@Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        SearchPanelCircleView* mHost;
        const AutoPtr<IRunnable> mEndRunnable;
    };

public:
    CAR_INTERFACE_DECL()

    SearchPanelCircleView(
        /* [in] */ IContext* context);

    SearchPanelCircleView(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    SearchPanelCircleView(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr);

    SearchPanelCircleView(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

    CARAPI SetCircleSize(
        /* [in] */ Float circleSize);

    CARAPI SetCircleSize(
        /* [in] */ Float circleSize,
        /* [in] */ Boolean animated,
        /* [in] */ IRunnable* endRunnable,
        /* [in] */ Int32 startDelay,
        /* [in] */ IInterpolator* interpolator);

    /**
     * Sets the offset to the edge of the screen. By default this not not animated.
     *
     * @param offset The offset to apply.
     */
    CARAPI SetOffset(
        /* [in] */ Float offset);

    CARAPI SetHorizontal(
        /* [in] */ Boolean horizontal);

    CARAPI SetDragDistance(
        /* [in] */ Float distance);

    CARAPI StartAbortAnimation(
        /* [in] */ IRunnable* endRunnable);

    CARAPI StartEnterAnimation();


    CARAPI StartExitAnimation(
        /* [in] */ IRunnable* endRunnable);

    CARAPI SetDraggedFarEnough(
        /* [in] */ Boolean farEnough);

    CARAPI Reset();

    /**
     * Check if an animation is currently running
     *
     * @param enterAnimation Is the animating queried the enter animation.
     */
    CARAPI IsAnimationRunning(
        /* [in] */ Boolean enterAnimation,
        /* [out] */ Boolean* result);

    CARAPI PerformOnAnimationFinished(
        /* [in] */ IRunnable* runnable);

    CARAPI SetAnimatingOut(
        /* [in] */ Boolean animatingOut);

    /**
     * @return Whether the circle is currently launching to the search activity or aborting the
     * interaction
     */
    CARAPI IsAnimatingOut(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI HasOverlappingRendering(
        /* [out] */ Boolean* result);

protected:
    // @Override
    CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas);

    // @Override
    CARAPI OnFinishInflate();

    // @Override
    CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 l,
        /* [in] */ Int32 t,
        /* [in] */ Int32 r,
        /* [in] */ Int32 b);

private:
    CARAPI_(void) DrawRipples(
        /* [in] */ ICanvas* canvas);

    CARAPI_(void) DrawBackground(
        /* [in] */ ICanvas* canvas);

    CARAPI_(void) ApplyCircleSize(
        /* [in] */ Float circleSize);

    CARAPI_(void) UpdateElevation();

    /**
     * Sets the offset to the edge of the screen.
     *
     * @param offset The offset to apply.
     * @param animate Whether an animation should be performed.
     * @param startDelay The desired start delay if animated.
     * @param interpolator The desired interpolator if animated. If null,
     *                     a default interpolator will be taken designed for appearing or
     *                     disappearing.
     * @param endRunnable The end runnable which should be executed when the animation is finished.
     */
    CARAPI_(void) SetOffset(
        /* [in] */ Float offset,
        /* [in] */ Boolean animate,
        /* [in] */ Int32 startDelay,
        /* [in] */ IInterpolator* interpolator,
        /* [in] */ IRunnable* endRunnable);

    CARAPI_(void) UpdateLayout();

    CARAPI_(void) UpdateClipping();

    CARAPI_(void) UpdateLogo();

    CARAPI_(void) UpdateCircleRect();

    CARAPI_(void) UpdateCircleRect(
        /* [in] */ IRect* rect,
        /* [in] */ Float offset,
        /* [in] */ Boolean useStaticSize);

    CARAPI_(Float) Rubberband(
        /* [in] */ Float diff);

    CARAPI_(void) PerformExitFadeOutAnimation(
        /* [in] */ Int32 startDelay,
        /* [in] */ Int32 duration,
       /* [in] */ IRunnable* endRunnable);

    CARAPI_(void) AddRipple();

private:
    Int32 mCircleMinSize;
    Int32 mBaseMargin;
    Int32 mStaticOffset;
    AutoPtr<IPaint> mBackgroundPaint;
    AutoPtr<IPaint> mRipplePaint;
    AutoPtr<IRect> mCircleRect;
    AutoPtr<IRect> mStaticRect;
    AutoPtr<IInterpolator> mFastOutSlowInInterpolator;
    AutoPtr<IInterpolator> mAppearInterpolator;
    AutoPtr<IInterpolator> mDisappearInterpolator;

    Boolean mClipToOutline;
    Int32 mMaxElevation;
    Boolean mAnimatingOut;
    Float mOutlineAlpha;
    Float mOffset;
    Float mCircleSize;
    Boolean mHorizontal;
    Boolean mCircleHidden;
    AutoPtr<IImageView> mLogo;
    Boolean mDraggedFarEnough;
    Boolean mOffsetAnimatingIn;
    Float mCircleAnimationEndValue;
    AutoPtr<IArrayList> mRipples;

    AutoPtr<IValueAnimator> mOffsetAnimator;
    AutoPtr<IValueAnimator> mCircleAnimator;
    AutoPtr<IValueAnimator> mFadeOutAnimator;
    AutoPtr<IAnimatorUpdateListener> mCircleUpdateListener;
    AutoPtr<AnimatorListenerAdapter> mClearAnimatorListener;
    AutoPtr<IAnimatorUpdateListener> mOffsetUpdateListener;
};

} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_SEARCHPANELCIRCLEVIEW_H__
