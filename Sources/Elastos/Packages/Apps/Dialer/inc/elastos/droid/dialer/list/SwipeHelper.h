#ifndef __ELASTOS_DROID_DIALER_LIST_SWIPEHELPER_H__
#define __ELASTOS_DROID_DIALER_LIST_SWIPEHELPER_H__

#include "_Elastos.Droid.Dialer.h"
#include <elastos/droid/animation/AnimatorListenerAdapter.h>
#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.View.h"

using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::IAnimatorUpdateListener;
using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::Animation::IValueAnimator;
using Elastos::Droid::Animation::AnimatorListenerAdapter;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::IRectF;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IVelocityTracker;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::Animation::ILinearInterpolator;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace List {

/**
 * Copy of packages/apps/UnifiedEmail - com.android.mail.ui.SwipeHelper with changes.
 */
class SwipeHelper
    : public Object
{
private:
    class DismissAnimatorListenerAdapter
        : public AnimatorListenerAdapter
    {
    public:
        DismissAnimatorListenerAdapter(
            /* [in] */ SwipeHelper* host);

        // @Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        SwipeHelper* mHost;
    };

    class DismissAnimatorUpdateListener
        : public Object
        , public IAnimatorUpdateListener
    {
    public:
        CAR_INTERFACE_DECL()

        DismissAnimatorUpdateListener(
            /* [in] */ SwipeHelper* host);

        // @Override
        CARAPI OnAnimationUpdate(
            /* [in] */ IValueAnimator* animation);
    private:
        SwipeHelper* mHost;
    };

    class SnapAnimatorUpdateListener
        : public Object
        , public IAnimatorUpdateListener
    {
    public:
        CAR_INTERFACE_DECL();

        SnapAnimatorUpdateListener(
            /* [in] */ SwipeHelper* host);

        // @Override
        CARAPI OnAnimationUpdate(
            /* [in] */ IValueAnimator* animation);
    private:
        SwipeHelper* mHost;
    };

    class SnapAnimatorListenerAdapter
        : public AnimatorListenerAdapter
    {
    public:
        SnapAnimatorListenerAdapter(
            /* [in] */ SwipeHelper* host);

        // @Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        SwipeHelper* mHost;
    };

public:
    SwipeHelper(
        /* [in] */ IContext* context,
        /* [in] */ Int32 swipeDirection,
        /* [in] */ ISwipeHelperCallback* callback,;
        /* [in] */ Float densityScale,
        /* [in] */ Float pagingTouchSlop);

    CARAPI_(void) SetDensityScale(
        /* [in] */ Float densityScale);

    CARAPI_(void) SetPagingTouchSlop(
        /* [in] */ Float pagingTouchSlop);

    CARAPI_(void) SetChildSwipedFarEnoughFactor(
        /* [in] */ Float factor);

    CARAPI_(void) SetChildSwipedFastEnoughFactor(
        /* [in] */ Float factor);

    CARAPI_(void) SetMinAlpha(
        /* [in] */ Float minAlpha);

    // invalidate the view's own bounds all the way up the view hierarchy
    static CARAPI_(void) InvalidateGlobalRegion(
        /* [in] */ IView* view);

    // invalidate a rectangle relative to the view's coordinate system all the way up the view
    // hierarchy
    static CARAPI_(void) InvalidateGlobalRegion(
        /* [in] */ IView* view,
        /* [in] */ IRectF* childBounds);

    CARAPI_(Boolean) OnInterceptTouchEvent(
        /* [in] */ IMotionEvent* ev);

    CARAPI_(void) SnapChild(
        /* [in] */ IView* view,
        /* [in] */ Float velocity);

    CARAPI_(Boolean) OnTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* result);

    static CARAPI_(void) SetSwipeable(
        /* [in] */ IView* view,
        /* [in] */ Boolean swipeable);

    static CARAPI_(Boolean) IsSwipeable(
        /* [in] */ IView* view);

private:
    CARAPI_(Float) GetVelocity(
        /* [in] */ IVelocityTracker* vt);

    CARAPI_(AutoPtr<IObjectAnimator>) CreateTranslationAnimation(
        /* [in] */ IView* v,
        /* [in] */ Float newPos);

    CARAPI_(AutoPtr<IObjectAnimator>) CreateDismissAnimation(
        /* [in] */ IView* v,
        /* [in] */ Float newPos,
        /* [in] */ Int32 duration);

    CARAPI_(Float) GetPerpendicularVelocity(
        /* [in] */ IVelocityTracker* vt);

    CARAPI_(void) SetTranslation(
        /* [in] */ IView* v,
        /* [in] */ Float translate);

    CARAPI_(Float) GetSize(
        /* [in] */ IView* v);

    CARAPI_(Float) GetAlphaForOffset(
        /* [in] */ IView* view);

    /**
     * @param view The view to be dismissed
     * @param velocity The desired pixels/second speed at which the view should
     *            move
     */
    CARAPI_(void) DismissChild(
        /* [in] */ IView* view,
        /* [in] */ Float velocity);

    CARAPI_(Int32) DetermineDuration(
        /* [in] */ IView* animView,
        /* [in] */ Float newPos,
        /* [in] */ Float velocity);

    CARAPI_(Float) DeterminePos(
        /* [in] */ IView* animView,
        /* [in] */ Float velocity);

public:
    static const Int32 IS_SWIPEABLE_TAG;// = R.id.is_swipeable_tag;
    static const AutoPtr<Object> IS_SWIPEABLE;

    static const Int32 X = 0;
    static const Int32 Y = 1;

    static const Float ALPHA_FADE_START = 0f; // fraction of thumbnail width
                                               // where fade starts

private:
    static const String TAG;

    static const Boolean DEBUG_INVALIDATE = FALSE;
    static const Boolean CONSTRAIN_SWIPE = TRUE;
    static const Boolean FADE_OUT_DURING_SWIPE = TRUE;
    static const Boolean DISMISS_IF_SWIPED_FAR_ENOUGH = TRUE;
    static const Boolean LOG_SWIPE_DISMISS_VELOCITY = FALSE; // STOPSHIP - DEBUG ONLY

    static AutoPtr<ILinearInterpolator> sLinearInterpolator; // = new LinearInterpolator();

    static Int32 SWIPE_ESCAPE_VELOCITY;
    static Int32 DEFAULT_ESCAPE_ANIMATION_DURATION;
    static Int32 MAX_ESCAPE_ANIMATION_DURATION;
    static Int32 MAX_DISMISS_VELOCITY;
    static Int32 SNAP_ANIM_LEN;
    static Int32 SWIPE_SCROLL_SLOP;
    static Float MIN_SWIPE;
    static Float MIN_VERT;
    static Float MIN_LOCK;

    static const Float ALPHA_FADE_END = 0.7f; // fraction of thumbnail width
                                              // beyond which alpha->0

    static const Float FACTOR = 1.2f;

    static const Int32 PROTECTION_PADDING = 50;

    Float mMinAlpha; // = 0.3f;

    Float mPagingTouchSlop;
    AutoPtr<ISwipeHelperCallback> mCallback;
    Int32 mSwipeDirection;
    AutoPtr<IVelocityTracker> mVelocityTracker;

    Float mInitialTouchPosX;
    Boolean mDragging;
    AutoPtr<IView> mCurrView;
    AutoPtr<IView> mCurrAnimView;
    Boolean mCanCurrViewBeDimissed;
    Float mDensityScale;
    Float mLastY;
    Float mInitialTouchPosY;

    Float mStartAlpha;
    Boolean mProtected;

    Float mChildSwipedFarEnoughFactor;
    Float mChildSwipedFastEnoughFactor;

    friend class DismissAnimatorListenerAdapter;
    friend class DismissAnimatorUpdateListener;
    friend class SnapAnimatorUpdateListener;
    friend class SnapAnimatorListenerAdapter;
};

} // List
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_LIST_SWIPEHELPER_H__
