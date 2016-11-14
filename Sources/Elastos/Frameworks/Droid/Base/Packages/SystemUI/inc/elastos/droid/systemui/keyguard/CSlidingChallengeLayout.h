
#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CSLIDINGCHALLENGELAYOUT_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CSLIDINGCHALLENGELAYOUT_H__

#include "_Elastos_Droid_SystemUI_Keyguard_CSlidingChallengeLayout.h"
#include "elastos/droid/animation/AnimatorListenerAdapter.h"
#include "elastos/droid/utility/FloatProperty.h"
#include <elastos/droid/view/ViewGroup.h>
#include <elastos/core/Runnable.h>
#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"

using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Animation::AnimatorListenerAdapter;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Core::Runnable;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::ViewGroup;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IVelocityTracker;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::IViewGroupMarginLayoutParams;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IViewConfigurationHelper;
using Elastos::Droid::View::CViewConfigurationHelper;
using Elastos::Droid::View::Animation::IInterpolator;
using Elastos::Droid::Utility::IProperty;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::Utility::FloatProperty;
using Elastos::Droid::Widget::IScroller;
using Elastos::Core::IRunnable;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

/**
 * This layout handles interaction with the sliding security challenge views
 * that overlay/resize other keyguard contents.
 */
CarClass(CSlidingChallengeLayout)
    , public ViewGroup
    , public IChallengeLayout
    , public ISlidingChallengeLayout
{
private:
    class MyProperty
        : public FloatProperty
    {
    public:
        TO_STRING_IMPL("CSlidingChallengeLayout::MyProperty")

        MyProperty(
            /* [in] */ const String& name)
            : FloatProperty(name)
        {}

        //@Override
        CARAPI SetValue(
            /* [in] */ IInterface* obj,
            /* [in] */ Float value);

        //@Override
        CARAPI Get(
            /* [in] */ IInterface* obj,
            /* [out] */ IInterface** rst);
    };

    class MyInterpolator
        : public Object
        , public IInterpolator
        , public ITimeInterpolator
    {
    public:
        TO_STRING_IMPL("CSlidingChallengeLayout::MyInterpolator")

        CAR_INTERFACE_DECL()

        CARAPI GetInterpolation(
            /* [in] */ Float input,
            /* [out] */ Float* output);

        CARAPI HasNativeInterpolator(
            /* [out] */ Boolean* res);
    };

    class MyInterpolator2
        : public Object
        , public IInterpolator
        , public ITimeInterpolator
    {
    public:
        TO_STRING_IMPL("CSlidingChallengeLayout::MyInterpolator2")

        CAR_INTERFACE_DECL()

        CARAPI GetInterpolation(
            /* [in] */ Float input,
            /* [out] */ Float* output);

        CARAPI HasNativeInterpolator(
            /* [out] */ Boolean* res);
    };

    class MyRunnable
        : public Runnable
    {
    public:
        TO_STRING_IMPL("CSlidingChallengeLayout::MyRunnable")

        MyRunnable(
            /* [in] */ CSlidingChallengeLayout* host)
            : mHost(host)
        {}

        CARAPI Run();

    private:
        CSlidingChallengeLayout* mHost;
    };

    class MyOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        TO_STRING_IMPL("CSlidingChallengeLayout::MyOnClickListener")

        CAR_INTERFACE_DECL()

        MyOnClickListener(
            /* [in] */ CSlidingChallengeLayout* host)
            : mHost(host)
        {}

        //@Override
        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CSlidingChallengeLayout* mHost;
    };

    class MyOnClickListener2
        : public Object
        , public IViewOnClickListener
    {
    public:
        TO_STRING_IMPL("CSlidingChallengeLayout::MyOnClickListener2")

        CAR_INTERFACE_DECL()

        MyOnClickListener2(
            /* [in] */ CSlidingChallengeLayout* host)
            : mHost(host)
        {}

        //@Override
        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CSlidingChallengeLayout* mHost;
    };

    class MyAnimatorListenerAdapter
        : public AnimatorListenerAdapter
    {
    public:
        TO_STRING_IMPL("CSlidingChallengeLayout::MyAnimatorListenerAdapter")

        MyAnimatorListenerAdapter(
            /* [in] */ CSlidingChallengeLayout* host)
            : mHost(host)
        {}

        //@Override
        CARAPI OnAnimationStart(
            /* [in] */ IAnimator* animation);

    private:
        CSlidingChallengeLayout* mHost;
    };

    class MyAnimatorListenerAdapter2
        : public AnimatorListenerAdapter
    {
    public:
        TO_STRING_IMPL("CSlidingChallengeLayout::MyAnimatorListenerAdapter2")

        MyAnimatorListenerAdapter2(
            /* [in] */ CSlidingChallengeLayout* host)
            : mHost(host)
        {}

        //@Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        CSlidingChallengeLayout* mHost;
    };

    class MyAnimatorListenerAdapter3
        : public AnimatorListenerAdapter
    {
    public:
        TO_STRING_IMPL("CSlidingChallengeLayout::MyAnimatorListenerAdapter3")

        MyAnimatorListenerAdapter3(
            /* [in] */ CSlidingChallengeLayout* host,
            /* [in] */ Boolean show)
            : mHost(host)
            , mShow(show)
        {}

        //@Override
        CARAPI OnAnimationStart(
            /* [in] */ IAnimator* animation);

        //@Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        CSlidingChallengeLayout* mHost;
        Boolean mShow;
    };

    class SlidingChallengeLayoutLayoutParams
        : public ViewGroup::MarginLayoutParams
        , public ISlidingChallengeLayoutLayoutParams
    {
    public:
        TO_STRING_IMPL("CSlidingChallengeLayout::LayoutParams")

        CAR_INTERFACE_DECL()

        SlidingChallengeLayoutLayoutParams();

        CARAPI constructor();

        CARAPI constructor(
            /* [in] */ Int32 width,
            /* [in] */ Int32 height);

        CARAPI constructor(
            /* [in] */ IViewGroupLayoutParams* source);

        CARAPI constructor(
            /* [in] */ IViewGroupMarginLayoutParams* source);

        CARAPI constructor(
            /* [in] */ ISlidingChallengeLayoutLayoutParams* source);

        CARAPI constructor(
            /* [in] */ IContext* c,
            /* [in] */ IAttributeSet* attrs);

    public:
        Int32 mChildType;
        Int32 mMaxHeight;
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CSlidingChallengeLayout();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI SetEnableChallengeDragging(
        /* [in] */ Boolean enabled);

    CARAPI SetInsets(
        /* [in] */ IRect* insets);

    CARAPI SetHandleAlpha(
        /* [in] */ Float alpha);

    CARAPI SetChallengeInteractive(
        /* [in] */ Boolean interactive);

    CARAPI AnimateHandle(
        /* [in] */ Boolean visible);

    CARAPI SetOnChallengeScrolledListener(
        /* [in] */ ISlidingChallengeLayoutOnChallengeScrolledListener* listener);

    CARAPI SetOnBouncerStateChangedListener(
        /* [in] */ IChallengeLayoutOnBouncerStateChangedListener* listener);

    //@Override
    CARAPI OnAttachedToWindow();

    //@Override
    CARAPI OnDetachedFromWindow();

    //@Override
    CARAPI RequestChildFocus(
        /* [in] */ IView* child,
        /* [in] */ IView* focused);

    // We want the duration of the page snap animation to be influenced by the distance that
    // the screen has to travel, however, we don't want this duration to be effected in a
    // purely linear fashion. Instead, we use this method to moderate the effect that the distance
    // of travel has on the overall snap duration.
    CARAPI DistanceInfluenceForSnapDuration(
        /* [in] */ Float f,
        /* [out] */ Float* value);

    CARAPI SetScrollState(
        /* [in] */ Int32 state);

    CARAPI CompleteChallengeScroll();

    CARAPI SetScrimView(
        /* [in] */ IView* scrim);

    /**
     * Animate the bottom edge of the challenge view to the given position.
     *
     * @param y desired final position for the bottom edge of the challenge view in px
     * @param velocity velocity in
     */
    CARAPI AnimateChallengeTo(
        /* [in] */ Int32 y,
        /* [in] */ Int32 velocity);

    /**
     * @return true if the challenge is at all visible.
     */
    CARAPI IsChallengeShowing(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI IsChallengeOverlapping(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI IsBouncing(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI GetBouncerAnimationDuration(
        /* [out] */ Int32* duration);

    //@Override
    CARAPI ShowBouncer();

    //@Override
    CARAPI HideBouncer();

    //@Override
    CARAPI RequestDisallowInterceptTouchEvent(
        /* [in] */ Boolean allowIntercept);

    //@Override
    CARAPI OnInterceptTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* result);

    // /@Override
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* result);

    /**
     * The lifecycle of touch events is subtle and it's very easy to do something
     * that will cause bugs that will be nasty to track when overriding this method.
     * Normally one should always override onInterceptTouchEvent instead.
     *
     * To put it another way, don't try this at home.
     */
    //@Override
    CARAPI DispatchTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI Draw(
        /* [in] */ ICanvas* c);

    CARAPI ComputeScroll();

    CARAPI FadeInChallenge();

    CARAPI FadeOutChallenge();

    CARAPI FadeChallenge(
        /* [in] */ Boolean show);

    CARAPI GetMaxChallengeTop(
        /* [out] */ Int32* top);

    /**
     * Show or hide the challenge view, animating it if necessary.
     * @param show true to show, false to hide
     */
    CARAPI ShowChallenge(
        /* [in] */ Boolean show);

    //@Override
    CARAPI GenerateLayoutParams(
        /* [in] */ IAttributeSet* attrs,
        /* [out] */ IViewGroupLayoutParams** p);

protected:
    // /@Override
    CARAPI OnMeasure(
        /* [in] */ Int32 widthSpec,
        /* [in] */ Int32 heightSpec);

    //@Override
    CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 l,
        /* [in] */ Int32 t,
        /* [in] */ Int32 r,
        /* [in] */ Int32 b);

    //@Override
    CARAPI_(Boolean) OnRequestFocusInDescendants(
        /* [in] */ Int32 direction,
        /* [in] */ IRect* previouslyFocusedRect);

    //@Override
    CARAPI_(AutoPtr<IViewGroupLayoutParams>) GenerateLayoutParams(
        /* [in] */ IViewGroupLayoutParams* p);

    //@Override
    CARAPI GenerateDefaultLayoutParams(
        /* [out] */ IViewGroupLayoutParams** p);

    //@Override
    CARAPI_(Boolean) CheckLayoutParams(
        /* [in] */ IViewGroupLayoutParams* p);

private:
    CARAPI_(void) SendInitialListenerUpdates();

    CARAPI_(void) SetChallengeShowing(
        /* [in] */ Boolean showChallenge);

    CARAPI_(Int32) GetChallengeMargin(
        /* [in] */ Boolean expanded);

    CARAPI_(Float) GetChallengeAlpha();

    CARAPI_(Boolean) ShouldEnableChallengeDragging();

    CARAPI_(Boolean) IsChallengeInteractionBlocked();

    CARAPI_(void) ResetTouch();

    CARAPI_(Boolean) IsEdgeSwipeBeginEvent(
        /* [in] */ IMotionEvent* ev);

    /**
     * We only want to add additional vertical space to the drag handle when the panel is fully
     * closed.
     */
    CARAPI_(Int32) GetDragHandleSizeAbove();

    CARAPI_(Int32) GetDragHandleSizeBelow();

    CARAPI_(Boolean) IsInChallengeView(
        /* [in] */ Float x,
        /* [in] */ Float y);

    CARAPI_(Boolean) IsInDragHandle(
        /* [in] */ Float x,
        /* [in] */ Float y);

    CARAPI_(Boolean) IsPointInView(
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ IView* view);

    CARAPI_(Boolean) CrossedDragHandle(
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Float initialY);

    CARAPI_(Int32) MakeChildMeasureSpec(
        /* [in] */ Int32 maxSize,
        /* [in] */ Int32 childDimen);

    CARAPI_(void) CancelTransitionsInProgress();

    CARAPI_(Int32) GetMaxChallengeBottom();

    CARAPI_(Int32) GetMinChallengeBottom();

    CARAPI_(void) OnFadeStart(
        /* [in] */ Boolean show);

    CARAPI_(void) EnableHardwareLayerForChallengeView();

    CARAPI_(void) OnFadeEnd(
        /* [in] */ Boolean show);

    /**
     * Move the bottom edge of mChallengeView to a new position and notify the listener
     * if it represents a change in position. Changes made through this method will
     * be stable across layout passes. If this method is called before first layout of
     * this SlidingChallengeLayout it will have no effect.
     *
     * @param bottom New bottom edge in px in this SlidingChallengeLayout's coordinate system.
     * @return true if the challenge view was moved
     */
    CARAPI_(Boolean) MoveChallengeTo(
        /* [in] */ Int32 bottom);

    /**
     * The bottom edge of this SlidingChallengeLayout's coordinate system; will coincide with
     * the bottom edge of mChallengeView when the challenge is fully opened.
     */
    CARAPI_(Int32) GetLayoutBottom();

    /**
     * The bottom edge of mChallengeView; essentially, where the sliding challenge 'is'.
     */
    CARAPI_(Int32) GetChallengeBottom();

    CARAPI_(void) ShowChallenge(
        /* [in] */ Int32 velocity);

    CARAPI_(void) ShowChallenge(
        /* [in] */ Boolean show,
        /* [in] */ Int32 velocity);

private:
    static const String TAG;
    static const Boolean DEBUG;

    // The drag handle is measured in dp above & below the top edge of the
    // challenge view; these parameters change based on whether the challenge
    // is open or closed.
    static const Int32 DRAG_HANDLE_CLOSED_ABOVE; // dp
    static const Int32 DRAG_HANDLE_CLOSED_BELOW; // dp
    static const Int32 DRAG_HANDLE_OPEN_ABOVE; // dp
    static const Int32 DRAG_HANDLE_OPEN_BELOW; // dp

    static const Int32 HANDLE_ANIMATE_DURATION; // ms

    // Drawn to show the drag handle in closed state; crossfades to the challenge view
    // when challenge is fully visible
    Boolean mEdgeCaptured;

    AutoPtr<IDisplayMetrics> mDisplayMetrics;

    // Initialized during measurement from child layoutparams
    AutoPtr<IView> mExpandChallengeView;
    AutoPtr<IKeyguardSecurityContainer> mChallengeView;
    AutoPtr<IView> mScrimView;
    AutoPtr<IView> mWidgetsView;

    // Range: 0 (fully hidden) to 1 (fully visible)
    Float mChallengeOffset;
    Boolean mChallengeShowing;
    Boolean mChallengeShowingTargetState;
    Boolean mWasChallengeShowing;
    Boolean mIsBouncing;

    AutoPtr<IScroller> mScroller;
    AutoPtr<IObjectAnimator> mFader;
    Int32 mScrollState;
    AutoPtr<ISlidingChallengeLayoutOnChallengeScrolledListener> mScrollListener;
    AutoPtr<IChallengeLayoutOnBouncerStateChangedListener> mBouncerListener;
    Boolean mEnableChallengeDragging;

    static const Int32 MAX_SETTLE_DURATION; // ms

    // ID of the pointer in charge of a current drag
    Int32 mActivePointerId;
    static const Int32 INVALID_POINTER;

    // True if the user is currently dragging the slider
    Boolean mDragging;
    // True if the user may not drag until a new gesture begins
    Boolean mBlockDrag;

    AutoPtr<IVelocityTracker> mVelocityTracker;
    Int32 mMinVelocity;
    Int32 mMaxVelocity;
    Float mGestureStartX, mGestureStartY; // where did you first touch the screen?
    Int32 mGestureStartChallengeBottom; // where was the challenge at that time?

    Int32 mDragHandleClosedBelow; // handle hitrect extension into the challenge view
    Int32 mDragHandleClosedAbove; // extend the handle's hitrect this far above the line
    Int32 mDragHandleOpenBelow; // handle hitrect extension into the challenge view
    Int32 mDragHandleOpenAbove; // extend the handle's hitrect this far above the line

    Int32 mDragHandleEdgeSlop;
    Int32 mChallengeBottomBound; // Number of pixels from the top of the challenge view
                                       // that should remain on-screen

    Int32 mTouchSlop;
    Int32 mTouchSlopSquare;

    Float mHandleAlpha;
    Float mFrameAlpha;
    Float mFrameAnimationTarget;
    AutoPtr<IObjectAnimator> mHandleAnimation;
    AutoPtr<IObjectAnimator> mFrameAnimation;

    Boolean mHasGlowpad;
    AutoPtr<IRect> mInsets;

    // We have an internal and external version, and we and them together.
    Boolean mChallengeInteractiveExternal;
    Boolean mChallengeInteractiveInternal;

    static AutoPtr<IProperty> HANDLE_ALPHA;

    // True if at least one layout pass has happened since the view was attached.
    Boolean mHasLayout;

    static AutoPtr<IInterpolator> sMotionInterpolator;

    static AutoPtr<IInterpolator> sHandleFadeInterpolator;

    AutoPtr<IRunnable> mEndScrollRunnable;

    AutoPtr<IViewOnClickListener> mScrimClickListener;

    AutoPtr<IViewOnClickListener> mExpandChallengeClickListener;
};

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CSLIDINGCHALLENGELAYOUT_H__
