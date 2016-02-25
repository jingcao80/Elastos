#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_PANELVIEW_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_PANELVIEW_H__

#include "elastos/droid/widget/FrameLayout.h"
#include "elastos/droid/os/Runnable.h"
#include "elastos/droid/view/VelocityTracker.h"

using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Animation::ITimeAnimator;
using Elastos::Droid::Animation::ITimeListener;
using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::View::VelocityTracker;
using Elastos::Droid::View::IViewOnTouchListener;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::Widget::FrameLayout;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {


class PanelView : public FrameLayout
{
private:
    class TimeListener
        : public ElRefBase
        , public ITimeListener
    {
    public:
        CAR_INTERFACE_DECL()

        TimeListener(
            /* [in] */ PanelView* host);

        virtual CARAPI OnTimeUpdate(
            /* [in] */ ITimeAnimator* animation,
            /* [in] */ Int64 totalTime,
            /* [in] */ Int64 deltaTime);

    private:
        PanelView* mHost;
    };

    class StopAnimatorRunnable
        : public Runnable
    {
    public:
        StopAnimatorRunnable(
            /* [in] */ PanelView* host);

        virtual CARAPI Run();
    private:
        PanelView* mHost;
    };

    class ViewOnTouchListener
        : public ElRefBase
        , public IViewOnTouchListener
    {
    public:
        CAR_INTERFACE_DECL()

        ViewOnTouchListener(
            /* [in] */ PanelView* host);

        virtual CARAPI OnTouch(
            /* [in] */ IView* v,
            /* [in] */ IMotionEvent* event,
            /* [out] */ Boolean* result);
    private:
        PanelView* mHost;
    };

public:
    PanelView();

    PanelView(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI SetRubberbandingEnabled(
        /* [in] */ Boolean enabled);

    CARAPI Fling(
        /* [in] */ Float vel,
        /* [in] */ Boolean always);

    CARAPI_(String) GetName();

    CARAPI SetExpandedHeight(
        /* [in] */ Float height);

    CARAPI SetExpandedHeightInternal(
        /* [in] */ Float height);

    CARAPI_(AutoPtr<IView>) GetHandle();

    CARAPI SetExpandedFraction(
        /* [in] */ Float frac);

    CARAPI_(Float) GetExpandedHeight();

    CARAPI_(Float) GetExpandedFraction();

    CARAPI_(Boolean) IsFullyExpanded();

    CARAPI_(Boolean) IsFullyCollapsed();

    CARAPI_(Boolean) IsCollapsing();

    CARAPI SetBar(
        /* [in] */ IPanelBar* panelBar);

    CARAPI Collapse();

    CARAPI Expand();

    // Pass all touches along to the handle, allowing the user to
    // drag the panel closed from its interior
    //@Override
    virtual Boolean OnTouchEvent(
        /* [in] */ IMotionEvent* event);

protected:
    //@Override
    virtual CARAPI OnFinishInflate();

    //@Override
    virtual CARAPI OnAttachedToWindow();

    //@Override
    virtual CARAPI_(void) OnViewAdded(
        /* [in] */ IView* child);

    // Rubberbands the panel to hold its contents.
    //@Override
    virtual CARAPI_(void) OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */  Int32 heightMeasureSpec);

    //@Override
    virtual CARAPI_(void) OnLayout (
        /* [in] */ Boolean changed,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

private:
    CARAPI_(void) RunPeekAnimation();

    CARAPI_(void) AnimationTick(
        /* [in] */ Int64 dtms);

    CARAPI_(void) LoadDimens();

    CARAPI_(void) TrackMovement(
        /* [in] */ IMotionEvent* event);

    Float GetFullHeight();

protected:
    AutoPtr<IPanelBar> mBar;
    Float mInitialTouchY;
    Float mFinalTouchY;

private:
    static const String TAG;

    Boolean mRubberbandingEnabled;// = true;

    Float mSelfExpandVelocityPx; // classic value: 2000px/s
    Float mSelfCollapseVelocityPx; // classic value: 2000px/s (will be negated to collapse "up")
    Float mFlingExpandMinVelocityPx; // classic value: 200px/s
    Float mFlingCollapseMinVelocityPx; // classic value: 200px/s
    Float mCollapseMinDisplayFraction; // classic value: 0.08 (25px/min(320px,480px) on G1)
    Float mExpandMinDisplayFraction; // classic value: 0.5 (drag open halfway to expand)
    Float mFlingGestureMaxXVelocityPx; // classic value: 150px/s

    Float mFlingGestureMinDistPx;

    Float mExpandAccelPx; // classic value: 2000px/s/s
    Float mCollapseAccelPx; // classic value: 2000px/s/s (will be negated to collapse "up")

    Float mFlingGestureMaxOutputVelocityPx; // how fast can it really go? (should be a little
                                                    // faster than mSelfCollapseVelocityPx)

    Float mCollapseBrakingDistancePx;// = 200; // XXX Resource
    Float mExpandBrakingDistancePx;// = 150; // XXX Resource
    Float mBrakingSpeedPx;// = 150; // XXX Resource

    AutoPtr<IView> mHandleView;
    Float mPeekHeight;
    Float mTouchOffset;
    Float mExpandedFraction;// = 0;
    Float mExpandedHeight;// = 0;
    Boolean mJustPeeked;
    Boolean mClosing;
    Boolean mRubberbanding;
    Boolean mTracking;

    AutoPtr<ITimeAnimator> mTimeAnimator;
    AutoPtr<IObjectAnimator> mPeekAnimator;
    AutoPtr<VelocityTracker> mVelocityTracker;

    Int32 mAbsPos[2];

    AutoPtr<ITimeListener> mAnimationCallback;

    AutoPtr<IRunnable> mStopAnimator;

    Float mVel;
    Float mAccel;
    Int32 mFullHeight;// = 0;
    String mViewName;
};


}// namespace Phone
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_PANELVIEW_H__
