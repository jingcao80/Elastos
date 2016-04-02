
#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_PANELVIEW_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_PANELVIEW_H__

#include "_SystemUI.h"
#include "elastos/droid/systemui/statusbar/FlingAnimationUtils.h"
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.Droid.Animation.h"
#include <elastos/droid/animation/AnimatorListenerAdapter.h>
#include <elastos/droid/widget/FrameLayout.h>

using Elastos::Droid::Animation::AnimatorListenerAdapter;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::IAnimatorUpdateListener;
using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::Animation::IValueAnimator;
using Elastos::Droid::SystemUI::StatusBar::FlingAnimationUtils;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IOnGlobalLayoutListener;
using Elastos::Droid::Widget::FrameLayout;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

class PanelView
    : public FrameLayout
    , public IPanelView
{
private:
    class PeekRunnable: public Runnable
    {
    public:
        PeekRunnable(
            /* [in] */ PanelView* host);

        // @Override
        CARAPI Run();

    private:
        PanelView* mHost;
    };

    class FlingCollapseRunnable: public Runnable
    {
    public:
        FlingCollapseRunnable(
            /* [in] */ PanelView* host);

        // @Override
        CARAPI Run();

    private:
        PanelView* mHost;
    };

    class PostCollapseRunnable: public Runnable
    {
    public:
        PostCollapseRunnable(
            /* [in] */ PanelView* host);

        // @Override
        CARAPI Run();

    private:
        PanelView* mHost;
    };

    class AnimatorListenerAdapter1: public AnimatorListenerAdapter
    {
    private:
        class ListenerRunnable: public Runnable
        {
        public:
            ListenerRunnable(
                /* [in] */ PanelView* host);

            // @Override
            CARAPI Run();

        private:
            PanelView* mHost;
        };

    public:
        AnimatorListenerAdapter1(
            /* [in] */ PanelView* host);

        // @Override
        CARAPI OnAnimationCancel(
            /* [in] */ IAnimator* animation);

        // @Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        PanelView* mHost;
        Boolean mCancelled;
    };

    class AnimatorListenerAdapter2: public AnimatorListenerAdapter
    {
    public:
        AnimatorListenerAdapter2(
            /* [in] */ PanelView* host,
            /* [in] */ Boolean clearAllExpandHack);

        // @Override
        CARAPI OnAnimationCancel(
            /* [in] */ IAnimator* animation);

        // @Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        PanelView* mHost;
        Boolean mCancelled;
        Boolean mClearAllExpandHack;
    };

    class OnGlobalLayoutListener
        : public Object
        , public IOnGlobalLayoutListener
    {
    public:
        CAR_INTERFACE_DECL();

        OnGlobalLayoutListener(
            /* [in] */ PanelView* host);

        // @Override
        CARAPI OnGlobalLayout();

    private:
        PanelView* mHost;
    };

    class Runnable1: public Runnable
    {
    public:
        Runnable1(
            /* [in] */ PanelView* host);

        // @Override
        CARAPI Run();

    private:
        PanelView* mHost;
    };

    class AnimatorListenerAdapter3: public AnimatorListenerAdapter
    {
    public:
        AnimatorListenerAdapter3(
            /* [in] */ PanelView* host,
            /* [in] */ IRunnable* onAnimationFinished);

        // @Override
        CARAPI OnAnimationCancel(
            /* [in] */ IAnimator* animation);

        // @Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        PanelView* mHost;
        Boolean mCancelled;
        AutoPtr<IRunnable> mOnAnimationFinished;
    };

    class Runnable2: public Runnable
    {
    public:
        Runnable2(
            /* [in] */ PanelView* host);

        // @Override
        CARAPI Run();

    private:
        PanelView* mHost;
    };

    class AnimatorListenerAdapter4: public AnimatorListenerAdapter
    {
    public:
        AnimatorListenerAdapter4(
            /* [in] */ PanelView* host,
            /* [in] */ IRunnable* onAnimationFinished);

        // @Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        PanelView* mHost;
        AutoPtr<IRunnable> mOnAnimationFinished;
    };

    class AnimatorUpdateListener
        : public Object
        , public IAnimatorUpdateListener
    {
    public:
        CAR_INTERFACE_DECL();

        AnimatorUpdateListener(
            /* [in] */ PanelView* host);

        // @Override
        CARAPI OnAnimationUpdate(
            /* [in] */ IValueAnimator* animation);

    private:
        PanelView* mHost;
    };

public:
    CAR_INTERFACE_DECL();

    PanelView();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI SetTouchDisabled(
        /* [in] */ Boolean disabled);

    // @Override
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI OnInterceptTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    CARAPI GetName(
        /* [out] */ String* name);

    CARAPI SetExpandedHeight(
        /* [in] */ Float height);

    CARAPI SetExpandedHeightInternal(
        /* [in] */ Float h);

    CARAPI SetExpandedFraction(
        /* [in] */ Float frac);

    CARAPI GetExpandedHeight(
        /* [out] */ Float* result);

    CARAPI GetExpandedFraction(
        /* [out] */ Float* result);

    CARAPI IsFullyExpanded(
        /* [out] */ Boolean* result);

    CARAPI IsFullyCollapsed(
        /* [out] */ Boolean* result);

    CARAPI IsCollapsing(
        /* [out] */ Boolean* result);

    CARAPI IsTracking(
        /* [out] */ Boolean* result);

    CARAPI SetBar(
        /* [in] */ IPanelBar* panelBar);

    CARAPI Collapse(
        /* [in] */ Boolean delayed);

    CARAPI Expand();

    CARAPI CancelPeek();

    CARAPI InstantExpand();

    CARAPI InstantCollapse();

    CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

protected:
    virtual CARAPI_(void) OnExpandingFinished();

    virtual CARAPI_(void) OnExpandingStarted();

    virtual CARAPI_(void) LoadDimens();

    virtual CARAPI_(Boolean) HasConflictingGestures() = 0;

    virtual CARAPI_(void) OnTrackingStopped(
        /* [in] */ Boolean expand);

    virtual CARAPI_(void) OnTrackingStarted();

    virtual CARAPI_(Boolean) IsScrolledToBottom();

    virtual CARAPI_(Float) GetContentHeight();

    // @Override
    CARAPI OnFinishInflate();

    // @Override
    CARAPI_(void) OnConfigurationChanged(
        /* [in] */ IConfiguration* newConfig);

    /**
     * @param vel the current vertical velocity of the motion
     * @param vectorVel the length of the vectorial velocity
     * @return whether a fling should expands the panel; contracts otherwise
     */
    virtual CARAPI_(Boolean) FlingExpands(
        /* [in] */ Float vel,
        /* [in] */ Float vectorVel);

    virtual CARAPI_(void) Fling(
        /* [in] */ Float vel,
        /* [in] */ Boolean expand);

    // @Override
    CARAPI OnAttachedToWindow();

    // @Override
    CARAPI OnLayout (
        /* [in] */ Boolean changed,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    virtual CARAPI_(void) RequestPanelHeightUpdate();

    /**
     * @return TRUE if the panel tracking should be temporarily blocked; this is used when a
     *         conflicting gesture (opening QS) is happening
     */
    virtual CARAPI_(Boolean) IsTrackingBlocked() = 0;

    virtual CARAPI_(void) SetOverExpansion(
        /* [in] */ Float overExpansion,
        /* [in] */ Boolean isPixels) = 0;

    virtual CARAPI_(void) OnHeightUpdated(
        /* [in] */ Float expandedHeight) = 0;

    virtual CARAPI_(Float) GetOverExpansionAmount() = 0;

    virtual CARAPI_(Float) GetOverExpansionPixels() = 0;

    /**
     * This returns the maximum height of the panel. Children should override this if their
     * desired height is not the full height.
     *
     * @return the default implementation simply returns the maximum height.
     */
    virtual CARAPI_(Int32) GetMaxPanelHeight() = 0;

    virtual CARAPI_(void) StartUnlockHintAnimation();

    virtual CARAPI_(void) OnEdgeClicked(
        /* [in] */ Boolean right) = 0;

    virtual CARAPI_(Boolean) IsDozing() = 0;

    virtual CARAPI_(Float) GetPeekHeight() = 0;

    virtual CARAPI_(Float) GetCannedFlingDurationFactor() = 0;

    /**
     * @return whether "Clear all" button will be visible when the panel is fully expanded
     */
    virtual CARAPI_(Boolean) FullyExpandedClearAllVisible() = 0;

    virtual CARAPI_(Boolean) IsClearAllVisible() = 0;

    /**
     * @return the height of the clear all button, in pixels
     */
    virtual CARAPI_(Int32) GetClearAllHeight() = 0;

private:
    CARAPI_(void) NotifyExpandingStarted();

    CARAPI_(void) NotifyExpandingFinished();

    CARAPI_(void) SchedulePeek();

    CARAPI_(void) RunPeekAnimation();

    CARAPI_(void) TrackMovement(
        /* [in] */ IMotionEvent* event);

    CARAPI_(Int32) GetFalsingThreshold();

    CARAPI_(void) AbortAnimations();

    CARAPI_(void) InitVelocityTracker();

    CARAPI_(Boolean) IsBelowFalsingThreshold();

    /**
     * Phase 1: Move everything upwards.
     */
    CARAPI_(void) StartUnlockHintAnimationPhase1(
        /* [in] */ IRunnable* onAnimationFinished);

    /**
     * Phase 2: Bounce down.
     */
    CARAPI_(void) StartUnlockHintAnimationPhase2(
        /* [in] */ IRunnable* onAnimationFinished);

    CARAPI_(AutoPtr<IValueAnimator>) CreateHeightAnimator(
        /* [in] */ Float targetHeight);

    CARAPI_(void) NotifyBarPanelExpansionChanged();

    /**
     * Gets called when the user performs a click anywhere in the empty area of the panel.
     *
     * @return whether the panel will be expanded after the action performed by this method
     */
    CARAPI_(Boolean) OnEmptySpaceClick(
        /* [in] */ Float x);

    CARAPI_(Boolean) OnMiddleClicked();

    // CARAPI_(void) Logf(
    //     /* [in] */ const char *fmt,
    //     /* [in] */ Object... args)
    // {
    //     Log.v(TAG, (mViewName != NULL ? (mViewName.string() + ": ") : "") + String.format(fmt, args));
    // }

public:
    static const Boolean DEBUG;
    static const String TAG;

protected:
    AutoPtr<IPhoneStatusBar> mStatusBar;
    Float mExpandedHeight;
    Boolean mTracking;
    Int32 mTouchSlop;
    Boolean mHintAnimationRunning;
    AutoPtr<IKeyguardBottomAreaView> mKeyguardBottomArea;

private:
    Float mPeekHeight;
    Float mHintDistance;
    Int32 mEdgeTapAreaWidth;
    Float mInitialOffsetOnTouch;
    Float mExpandedFraction;
    Boolean mPanelClosedOnDown;
    Boolean mHasLayoutedSinceDown;
    Float mUpdateFlingVelocity;
    Boolean mUpdateFlingOnLayout;
    Boolean mPeekTouching;
    Boolean mJustPeeked;
    Boolean mClosing;
    Boolean mTouchSlopExceeded;
    Int32 mTrackingPointer;
    Boolean mOverExpandedBeforeFling;
    Float mOriginalIndicationY;
    Boolean mTouchAboveFalsingThreshold;
    Int32 mUnlockFalsingThreshold;

    AutoPtr<IValueAnimator> mHeightAnimator;
    AutoPtr<IObjectAnimator> mPeekAnimator;
    AutoPtr<IVelocityTrackerInterface> mVelocityTracker;
    AutoPtr<FlingAnimationUtils> mFlingAnimationUtils;

    /**
     * Whether an instant expand request is currently pending and we are just waiting for layout.
     */
    Boolean mInstantExpanding;

    AutoPtr<IPanelBar> mBar;

    String mViewName;
    Float mInitialTouchY;
    Float mInitialTouchX;
    Boolean mTouchDisabled;

    AutoPtr<IInterpolator> mLinearOutSlowInInterpolator;
    AutoPtr<IInterpolator> mFastOutSlowInInterpolator;
    AutoPtr<IInterpolator> mBounceInterpolator;

    Boolean mPeekPending;
    Boolean mCollapseAfterPeek;
    Boolean mExpanding;
    Boolean mGestureWaitForTouchSlop;
    AutoPtr<IRunnable> mPeekRunnable;

    AutoPtr<IRunnable> mFlingCollapseRunnable;
    AutoPtr<IRunnable> mPostCollapseRunnable;
};

}// namespace Phone
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_PANELVIEW_H__
