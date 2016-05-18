
#ifndef  __ELASTOS_DROID_SYSTEMUI_CEXPANDHELPER_H__
#define  __ELASTOS_DROID_SYSTEMUI_CEXPANDHELPER_H__

#include "_Elastos_Droid_SystemUI_CExpandHelper.h"
#include "elastos/droid/systemui/statusbar/FlingAnimationUtils.h"
#include "Elastos.Droid.Content.h"
#include <elastos/droid/view/ScaleGestureDetector.h>
#include <elastos/droid/animation/AnimatorListenerAdapter.h>

using Elastos::Droid::Animation::AnimatorListenerAdapter;
using Elastos::Droid::Animation::IAnimatorSet;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Media::IAudioAttributes;
using Elastos::Droid::Os::IVibrator;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IVelocityTracker;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IScaleGestureDetector;
using Elastos::Droid::View::IScaleGestureDetectorOnScaleGestureListener;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::ScaleGestureDetector;
using Elastos::Droid::SystemUI::IExpandHelperCallback;
using Elastos::Droid::SystemUI::StatusBar::IExpandableView;
using Elastos::Droid::SystemUI::StatusBar::FlingAnimationUtils;
using Elastos::Droid::SystemUI::StatusBar::Policy::IScrollAdapter;

namespace Elastos {
namespace Droid {
namespace SystemUI {

class CViewScaler;

CarClass(CExpandHelper)
    , public Object
    , public IExpandHelper
    , public IGefingerpoken
{
private:
    class AnimatorListener : public AnimatorListenerAdapter
    {
    public:
        AnimatorListener(
            /* [in] */ CExpandHelper* host,
            /* [in] */ IView* scaledView);

        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        CExpandHelper* mHost;
        AutoPtr<IView> mView;
    };

    class ExpandScaleGestureListener
        : public ScaleGestureDetector::SimpleOnScaleGestureListener
    {
    public:
        ExpandScaleGestureListener(
            /* [in] */ CExpandHelper* host);

        virtual CARAPI OnScale(
            /* [in ]*/ IScaleGestureDetector* detector,
            /* [out] */ Boolean* res);

        virtual CARAPI OnScaleBegin(
            /* [in ]*/ IScaleGestureDetector* detector,
            /* [out] */ Boolean* res);

        virtual CARAPI OnScaleEnd(
            /* [in ]*/ IScaleGestureDetector* detector);
    private:
        CExpandHelper* mHost;
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CExpandHelper();

    /**
     * Handle expansion gestures to expand and contract children of the callback.
     *
     * @param context application context
     * @param callback the container that holds the items to be manipulated
     * @param small the smallest allowable size for the manuipulated items.
     * @param large the largest allowable size for the manuipulated items.
     * @param scoller if non-null also manipulate the scroll position to obey the gravity.
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IExpandHelperCallback* callback,
        /* [in] */ Int32 small,
        /* [in] */ Int32 large);

    CARAPI SetEventSource(
        /* [in] */ IView* eventSource);

    CARAPI SetGravity(
        /* [in] */ Int32 gravity);

    CARAPI SetScrollAdapter(
        /* [in] */ IScrollAdapter* adapter);

    CARAPI OnInterceptTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* result);

    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* result);

    /**
     * Use this to abort any pending expansions in progress.
     */
    CARAPI Cancel();

    CARAPI SetEnabled(
        /* [in] */ Boolean enable);

    /**
     * Change the expansion mode to only observe movements and don't perform any resizing.
     * This is needed when the expanding is finished and the scroller kicks in,
     * performing an overscroll motion. We only want to shrink it again when we are not
     * overscrolled.
     *
     * @param onlyMovements Should only movements be observed?
     */
    CARAPI OnlyObserveMovements(
        /* [in] */ Boolean onlyMovements);

private:
    CARAPI_(void) UpdateExpansion();

    CARAPI_(Float) Clamp(
        /* [in] */ Float target);

    CARAPI_(AutoPtr<IExpandableView>) FindView(
        /* [in] */ Float x,
        /* [in] */ Float y);

    CARAPI_(Boolean) IsInside(
        /* [in] */ IView* v,
        /* [in] */ Float x,
        /* [in] */ Float y);

    /**
     * @return True if the view is expandable, false otherwise.
     */
    CARAPI_(Boolean) StartExpanding(
        /* [in] */ IView* v,
        /* [in] */ Int32 expandType);

    CARAPI_(void) FinishExpanding(
        /* [in] */ Boolean force,
        /* [in] */ Float velocity);

    CARAPI_(void) ClearView();

    /**
     * Triggers haptic feedback.
     */
    CARAPI_(void) Vibrate(
         /* [in] */ Int64 duration);

    CARAPI_(void) TrackVelocity(
        /* [in] */ IMotionEvent* event);

    CARAPI_(void) MaybeRecycleVelocityTracker(
        /* [in] */ IMotionEvent* event);

    CARAPI_(Float) GetCurrentVelocity();

    CARAPI_(Boolean) IsEnabled();

    CARAPI_(Boolean) IsFullyExpanded(
        /* [in] */ IExpandableView* underFocus);

    static CARAPI_(Boolean) InitStatic();

protected:
    static const Boolean DEBUG;// = FALSE;
    static const Boolean DEBUG_SCALE;// = FALSE;

private:
    static const String TAG;// = "ExpandHelper";
    static const Float EXPAND_DURATION;

    // Set to FALSE to disable focus-based gestures (spread-finger vertical pull).
    static const Boolean USE_DRAG;// = TRUE;
    // Set to FALSE to disable scale-based gestures (both horizontal and vertical).
    static const Boolean USE_SPAN;// = TRUE;
    // Both gestures types may be active at the same time.
    // At least one gesture type should be active.
    // A variant of the screwdriver gesture will emerge from either gesture type.

    // amount of overstretch for maximum brightness expressed in U
    // 2f: maximum brightness is stretching a 1U to 3U, or a 4U to 6U
    static const Float STRETCH_INTERVAL;// = 2.0f;

    // level of glow for a touch, without overstretch
    // overstretch fills the range (GLOW_BASE, 1.0]
    static const Float GLOW_BASE;// = 0.5f;

    static AutoPtr<IAudioAttributes> VIBRATION_ATTRIBUTES;
    static Boolean sInit;

    // @SuppressWarnings("unused")
    AutoPtr<IContext> mContext;

    Boolean mExpanding;
    static const Int32 NONE;//    = 0;
    static const Int32 BLINDS;//  = 1<<0;
    static const Int32 PULL;//    = 1<<1;
    static const Int32 STRETCH;// = 1<<2;
    Int32 mExpansionStyle;// = NONE;
    Boolean mWatchingForPull;
    Boolean mHasPopped;
    IView* mEventSource;
    Float mOldHeight;
    Float mNaturalHeight;
    Float mInitialTouchFocusY;
    Float mInitialTouchY;
    Float mInitialTouchSpan;
    Float mLastFocusY;
    Float mLastSpanY;
    Int32 mTouchSlop;
    Float mLastMotionY;
    Int32 mPopDuration;
    Float mPullGestureMinXSpan;
    AutoPtr<IExpandHelperCallback> mCallback;
    AutoPtr<IScaleGestureDetector> mSGD;
    AutoPtr<ExpandScaleGestureListener> mScaleGestureListener;
    AutoPtr<IViewScaler> mScaler;
    AutoPtr<IObjectAnimator> mScaleAnimation;
    AutoPtr<IVibrator> mVibrator;
    Boolean mEnabled;
    AutoPtr<IExpandableView> mResizedView;
    Float mCurrentHeight;

    Int32 mSmallSize;
    Int32 mLargeSize;
    Float mMaximumStretch;
    Boolean mOnlyMovements;

    Int32 mGravity;
    IScrollAdapter* mScrollAdapter;
    AutoPtr<FlingAnimationUtils> mFlingAnimationUtils;
    AutoPtr<IVelocityTracker> mVelocityTracker;
    friend class CViewScaler;
};

} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_CEXPANDHELPER_H__
