
#ifndef  __ELASTOS_DROID_SYSTEMUI_CSWIPEHELPER_H__
#define  __ELASTOS_DROID_SYSTEMUI_CSWIPEHELPER_H__

#include "_Elastos_Droid_SystemUI_CSwipeHelper.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include <elastos/droid/animation/AnimatorListenerAdapter.h>
#include <elastos/core/Runnable.h>

using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::Animation::IValueAnimator;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::AnimatorListenerAdapter;
using Elastos::Droid::Animation::IAnimatorUpdateListener;
using Elastos::Droid::Animation::EIID_IAnimatorUpdateListener;
using Elastos::Droid::Content::IContext;
using Elastos::Core::Runnable;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Graphics::IRectF;
using Elastos::Droid::View::Animation::IInterpolator;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IViewOnLongClickListener;
using Elastos::Droid::View::IVelocityTracker;
using Elastos::Droid::View::Animation::ILinearInterpolator;

namespace Elastos {
namespace Droid {
namespace SystemUI {

CarClass(CSwipeHelper)
    , public Object
    , public ISwipeHelper
    , public IGefingerpoken
{
private:
    class WatchLongPressRunnable : public Runnable
    {
    public:
        WatchLongPressRunnable(
            /* [in] */ CSwipeHelper* host,
            /* [in] */ IMotionEvent* ev);

        virtual CARAPI Run();

    private:
        CSwipeHelper* mHost;
        AutoPtr<IMotionEvent> mEv;
    };

    class AnimatorListenerAdapterOne
        : public AnimatorListenerAdapter
    {
    public:
        AnimatorListenerAdapterOne(
            /* [in] */ CSwipeHelper* host,
            /* [in] */ IView* animView,
            /* [in] */ IView* view,
            /* [in] */ IRunnable* run);

        virtual CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        CSwipeHelper* mHost;
        AutoPtr<IView> mView;
        AutoPtr<IView> mAnimView;
        AutoPtr<IRunnable> mRun;
    };

    class AnimatorListenerAdapter2
        : public AnimatorListenerAdapter
    {
    public:
        AnimatorListenerAdapter2(
            /* [in] */ CSwipeHelper* host,
            /* [in] */ IView* animView,
            /* [in] */ Boolean canAnimViewBeDismissed);

        virtual CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        CSwipeHelper* mHost;
        AutoPtr<IView> mAnimView;
        Boolean mCanAnimViewBeDismissed;
    };

    class AnimatorUpdateListenerOne
        : public Object
        , public IAnimatorUpdateListener
    {
    public:
        CAR_INTERFACE_DECL()

        AnimatorUpdateListenerOne(
            /* [in] */ CSwipeHelper* host,
            /* [in] */ IView* animView,
            /* [in] */ Boolean canAnimViewBeDismissed);

        CARAPI OnAnimationUpdate(
            /* [in] */ IValueAnimator* animation);

    private:
        CSwipeHelper* mHost;
        AutoPtr<IView> mAnimView;
        Boolean mCanAnimViewBeDismissed;
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CSwipeHelper();

    CARAPI constructor(
        /* [in] */ Int32 swipeDirection,
        /* [in] */ ISwipeHelperCallback* callback,
        /* [in] */ IContext* context);

    CARAPI SetLongPressListener(
        /* [in] */ ISwipeHelperLongPressListener* listener);

    CARAPI SetDensityScale(
        /* [in] */ Float densityScale);

    CARAPI SetPagingTouchSlop(
        /* [in] */ Float pagingTouchSlop);

    CARAPI RemoveLongPressCallback();

    CARAPI SetMinSwipeProgress(
        /* [in] */ Float pagingTouchSlop);

    CARAPI SetMaxSwipeProgress(
        /* [in] */ Float maxSwipeProgress);

    CARAPI DismissChild(
        /* [in] */ IView* view,
        /* [in] */ Float velocity);

    /**
     * @param view The view to be dismissed
     * @param velocity The desired pixels/second speed at which the view should move
     * @param endAction The action to perform at the end
     * @param delay The delay after which we should start
     * @param useAccelerateInterpolator Should an accelerating Interpolator be used
     * @param fixedDuration If not 0, this exact duration will be taken
     */
    CARAPI DismissChild(
        /* [in] */ IView* view,
        /* [in] */ Float velocity,
        /* [in] */ IRunnable* endAction,
        /* [in] */ Int64 delay,
        /* [in] */ Boolean useAccelerateInterpolator,
        /* [in] */ Int64 fixedDuration);

    CARAPI SnapChild(
        /* [in] */ IView* view,
        /* [in] */ Float velocity);

    CARAPI OnInterceptTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* result);

    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* result);

    // invalidate the view's own bounds all the way up the view hierarchy
    static CARAPI InvalidateGlobalRegion(
        /* [in] */ IView* view);

    // invalidate a rectangle relative to the view's coordinate system all the way up the view
    // hierarchy
    static CARAPI InvalidateGlobalRegion(
        /* [in] */ IView* view,
        /* [in] */ IRectF* childBounds);

private:
    CARAPI_(Float) GetPos(
        /* [in] */ IMotionEvent* ev);

    CARAPI_(Float) GetTranslation(
        /* [in] */ IView* v);

    CARAPI_(Float) GetVelocity(
        /* [in] */ IVelocityTracker* vt);

    CARAPI_(AutoPtr)<IObjectAnimator> CreateTranslationAnimation(
        /* [in] */ IView* v,
        /* [in] */ Float newPos);

    CARAPI_(Float) GetPerpendicularVelocity(
        /* [in] */ IVelocityTracker* vt);

    CARAPI_(void) SetTranslation(
        /* [in] */ IView* v,
        /* [in] */ Float translate);

    CARAPI_(Float) GetSize(
        /* [in] */ IView* v);

    CARAPI_(Float) GetSwipeProgressForOffset(
        /* [in] */ IView* view);

    CARAPI_(void) UpdateSwipeProgressFromOffset(
        /* [in] */ IView* animView,
        /* [in] */ Boolean dismissable);

    CARAPI_(Int32) GetFalsingThreshold();

private:
    static const String TAG;// = "com.android.systemui.SwipeHelper";
    static const Boolean DEBUG;// = false;
    static const Boolean DEBUG_INVALIDATE;// = false;
    static const Boolean SLOW_ANIMATIONS;// = false; // DEBUG;
    static const Boolean CONSTRAIN_SWIPE;// = true;
    static const Boolean FADE_OUT_DURING_SWIPE;// = true;
    static const Boolean DISMISS_IF_SWIPED_FAR_ENOUGH;// = true;

    static AutoPtr<ILinearInterpolator> sLinearInterpolator;// = new LinearInterpolator();
    AutoPtr<IInterpolator> mFastOutLinearInInterpolator;

    Float SWIPE_ESCAPE_VELOCITY;// = 100f; // dp/sec
    Int32 DEFAULT_ESCAPE_ANIMATION_DURATION;// = 200; // ms
    Int32 MAX_ESCAPE_ANIMATION_DURATION;// = 400; // ms
    Int32 MAX_DISMISS_VELOCITY;// = 2000; // dp/sec
    static const Int32 SNAP_ANIM_LEN;//= SLOW_ANIMATIONS ? 1000 : 150; // ms

    Float mMinSwipeProgress;
    Float mMaxSwipeProgress;

    Float mPagingTouchSlop;
    // mCallback has this's reference
    ISwipeHelperCallback* mCallback;
    AutoPtr<IHandler> mHandler;
    Int32 mSwipeDirection;
    AutoPtr<IVelocityTracker> mVelocityTracker;

    Float mInitialTouchPos;
    Boolean mDragging;
    AutoPtr<IView> mCurrView;
    AutoPtr<IView> mCurrAnimView;
    Boolean mCanCurrViewBeDimissed;
    Float mDensityScale;

    Boolean mLongPressSent;
    AutoPtr<ISwipeHelperLongPressListener> mLongPressListener;
    AutoPtr<IRunnable> mWatchLongPress;
    Int64 mLongPressTimeout;

    AutoPtr<ArrayOf<Int32> > mTmpPos;
    Int32 mFalsingThreshold;
    Boolean mTouchAboveFalsingThreshold;
};

} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_CSWIPEHELPER_H__
