#ifndef __ELASTOS_DROID_SYSTEMUI_SEARCHPANELVIEW_H__
#define __ELASTOS_DROID_SYSTEMUI_SEARCHPANELVIEW_H__

#include "elastos/droid/widget/FrameLayout.h"
#include "elastos/droid/view/VelocityTracker.h"

using Elastos::Droid::Animation::ILayoutTransition;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IIWindowManager;
using Elastos::Droid::View::IWindowSession;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Droid::View::IChoreographer;
using Elastos::Droid::View::VelocityTracker;
using Elastos::Droid::Widget::FrameLayout;
using Elastos::Droid::SystemUI::StatusBar::IBaseStatusBar;


namespace Elastos {
namespace Droid {
namespace SystemUI {

class UniverseBackground : public FrameLayout
{
private:
    class AnimationCallback: public Runnable {
    public:
        AnimationCallback(
            /* [in] */ UniverseBackground* host);

        virtual CARAPI Run();

    private:
        UniverseBackground* mHost;
    };

public:
    UniverseBackground();

    UniverseBackground(
        /* [in] */ IContext* context);

    AutoPtr<IWindowManagerLayoutParams> GetWindowManagerLayoutParams();

    CARAPI AnimateCollapse();

    CARAPI AnimateCollapse(
        /* [in] */ Float velocityMultiplier);

    CARAPI ConsumeEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    CARAPI ResetLastAnimTime();

    CARAPI DoAnimation(
        /* [in] */ Int64 frameTimeNanos);

    CARAPI StopTracking();

    CARAPI IncrementAnim(
        /* [in] */ Int64 frameTimeNanos);

    CARAPI PrepareTracking(
        /* [in] */ Int32 y,
        /* [in] */ Boolean opening);

    CARAPI PerformFling(
        /* [in] */ Int32 y,
        /* [in] */ Float vel,
        /* [in] */ Boolean always);

protected:
    CARAPI_(void) OnConfigurationChanged(
        /* [in] */ IConfiguration* newConfig);

    CARAPI Init(
        /* [in] */ IContext* context);

    CARAPI InitImpl(
        /* [in] */ IContext* context);
private:
    CARAPI_(void) LoadDimens();

    CARAPI_(void) ComputeAveragePos(
        /* [in] */ IMotionEvent* event);

    CARAPI_(void) SendUniverseTransform();

    CARAPI_(Int32) GetExpandedViewMaxHeight();

    CARAPI_(void) UpdateUniverseScale();

    CARAPI_(void) TrackMovement(
        /* [in] */ IMotionEvent* event);

private:
    static const String TAG;
    static const Boolean SPEW = FALSE;
    static const Boolean CHATTY = FALSE;

    AutoPtr<IWindowSession> mSession;
    AutoPtr<IView> mContent;
    AutoPtr<IView> mBottomAnchor;

    AutoPtr<IRunnable> mAnimationCallback;

    // fling gesture tuning parameters, scaled to display density
    Float mSelfExpandVelocityPx; // classic value: 2000px/s
    Float mSelfCollapseVelocityPx; // classic value: 2000px/s (will be negated to collapse "up")
    Float mFlingExpandMinVelocityPx; // classic value: 200px/s
    Float mFlingCollapseMinVelocityPx; // classic value: 200px/s
    Float mCollapseMinDisplayFraction; // classic value: 0.08 (25px/min(320px,480px) on G1)
    Float mExpandMinDisplayFraction; // classic value: 0.5 (drag open halfway to expand)
    Float mFlingGestureMaxXVelocityPx; // classic value: 150px/s

    Float mExpandAccelPx; // classic value: 2000px/s/s
    Float mCollapseAccelPx; // classic value: 2000px/s/s (will be negated to collapse "up")

    static const Int32 STATE_CLOSED = 0;
    static const Int32 STATE_OPENING = 1;
    static const Int32 STATE_OPEN = 2;
    Int32 mState;// = STATE_CLOSED;

    Float mDragStartX;
    Float mDragStartY;
    Float mAverageX;
    Float mAverageY;

    // position
    AutoPtr<ArrayOf<Int32> > mPositionTmp; // new Int32[2];
    Boolean mExpanded;
    Boolean mExpandedVisible;

    Boolean mTracking;
    AutoPtr<VelocityTracker> mVelocityTracker;

    AutoPtr<IChoreographer> mChoreographer;
    Boolean mAnimating;
    Boolean mClosing; // only valid when mAnimating; indicates the initial acceleration
    Float mAnimY;
    Float mAnimVel;
    Float mAnimAccel;
    Int64 mAnimLastTimeNanos;
    Boolean mAnimatingReveal; // = false;

    Int32 mYDelta; // = 0;
    AutoPtr<ITransformation> mUniverseTransform; // = new Transformation();
    AutoPtr<ArrayOf<Float> > mTmpFloats; // new Float[9];
};


}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_SEARCHPANELVIEW_H__
