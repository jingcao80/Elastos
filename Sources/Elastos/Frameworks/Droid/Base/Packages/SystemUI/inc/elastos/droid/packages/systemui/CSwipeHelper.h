#ifndef __ELASTOS_DROID_SYSTEMUI_CSWIPEHELPER_H__
#define __ELASTOS_DROID_SYSTEMUI_CSWIPEHELPER_H__

#include "_Elastos_Droid_SystemUI_CSwipeHelper.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/Runnable.h"
#include "elastos/droid/view/VelocityTracker.h"
#include "elastos/droid/animation/AnimatorListenerAdapter.h"

using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Graphics::IRectF;
using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::Animation::IValueAnimator;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::AnimatorListenerAdapter;
using Elastos::Droid::Animation::IAnimatorUpdateListener;
using Elastos::Droid::Animation::EIID_IAnimatorUpdateListener;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IViewOnLongClickListener;
using Elastos::Droid::View::VelocityTracker;
using Elastos::Droid::View::Animation::ILinearInterpolator;

namespace Elastos {
namespace Droid {
namespace SystemUI {

CarClass(CSwipeHelper)
{
private:
    class WatchLongPressRunnable : public Runnable
    {
    public:
        WatchLongPressRunnable(
            /* [in] */ CSwipeHelper* host);

        virtual CARAPI Run();

    private:
        CSwipeHelper* mHost;
    };

    class AnimatorListenerAdapterOne
        : public AnimatorListenerAdapter
    {
    public:
        AnimatorListenerAdapterOne(
            /* [in] */ CSwipeHelper* host,
            /* [in] */ IView* animView,
            /* [in] */ IView* view);

        virtual CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        CSwipeHelper* mHost;
        AutoPtr<IView> mView;
        AutoPtr<IView> mAnimView;
    };

    class AnimatorUpdateListenerOne
        : public IAnimatorUpdateListener
        , public ElRefBase
    {
    public:
        CAR_INTERFACE_DECL();

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
    CSwipeHelper();

    CARAPI constructor(
        /* [in] */ Int32 swipeDirection,
        /* [in] */ ISwipeHelperCallback* callback,
        /* [in] */ Float densityScale,
        /* [in] */ Float pagingTouchSlop);

    CARAPI SetLongPressListener(
        /* [in] */ IViewOnLongClickListener* listener);

    CARAPI SetDensityScale(
        /* [in] */ Float densityScale);

    CARAPI SetPagingTouchSlop(
        /* [in] */ Float pagingTouchSlop);

    CARAPI RemoveLongPressCallback();

    CARAPI SetMinAlpha(
        /* [in] */ Float minAlpha);

    CARAPI DismissChild(
        /* [in] */ IView* view,
        /* [in] */ Float velocity);

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
    Float GetPos(
        /* [in] */ IMotionEvent* ev);

    Float GetTranslation(
        /* [in] */ IView* v);

    Float GetVelocity(
        /* [in] */ VelocityTracker* vt);

    AutoPtr<IObjectAnimator> CreateTranslationAnimation(
        /* [in] */ IView* v,
        /* [in] */ Float newPos);

    Float GetPerpendicularVelocity(
        /* [in] */ VelocityTracker* vt);

    void SetTranslation(
        /* [in] */ IView* v,
        /* [in] */ Float translate);

    Float GetSize(
        /* [in] */ IView* v);

    Float GetAlphaForOffset(
        /* [in] */ IView* view);

private:
    static const String TAG;// = "com.android.systemui.SwipeHelper";
    static const Boolean DEBUG;// = false;
    static const Boolean DEBUG_INVALIDATE;// = false;
    static const Boolean SLOW_ANIMATIONS;// = false; // DEBUG;
    static const Boolean CONSTRAIN_SWIPE;// = true;
    static const Boolean FADE_OUT_DURING_SWIPE;// = true;
    static const Boolean DISMISS_IF_SWIPED_FAR_ENOUGH;// = true;

    static AutoPtr<ILinearInterpolator> sLinearInterpolator;// = new LinearInterpolator();

    Float SWIPE_ESCAPE_VELOCITY;// = 100f; // dp/sec
    Int32 DEFAULT_ESCAPE_ANIMATION_DURATION;// = 200; // ms
    Int32 MAX_ESCAPE_ANIMATION_DURATION;// = 400; // ms
    Int32 MAX_DISMISS_VELOCITY;// = 2000; // dp/sec
    static const Int32 SNAP_ANIM_LEN;//= SLOW_ANIMATIONS ? 1000 : 150; // ms

    Float mMinAlpha;// = 0f;

    Float mPagingTouchSlop;
    // mCallback has this's reference
    ISwipeHelperCallback* mCallback;
    AutoPtr<IHandler> mHandler;
    Int32 mSwipeDirection;
    AutoPtr<VelocityTracker> mVelocityTracker;

    Float mInitialTouchPos;
    Boolean mDragging;
    AutoPtr<IView> mCurrView;
    AutoPtr<IView> mCurrAnimView;
    Boolean mCanCurrViewBeDimissed;
    Float mDensityScale;

    Boolean mLongPressSent;
    AutoPtr<IViewOnLongClickListener> mLongPressListener;
    AutoPtr<IRunnable> mWatchLongPress;
    Int64 mLongPressTimeout;
};

}// namespace SystemUI
}// namespace Droid
}// namespace Elastos


#endif //__ELASTOS_DROID_SYSTEMUI_CSWIPEHELPER_H__
