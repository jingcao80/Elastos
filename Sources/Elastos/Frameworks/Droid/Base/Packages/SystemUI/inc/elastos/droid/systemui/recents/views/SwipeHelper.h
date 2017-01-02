//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef  __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_SWIPEHELPER_H__
#define  __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_SWIPEHELPER_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.Graphics.h"
#include <elastos/droid/animation/AnimatorListenerAdapter.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::AnimatorListenerAdapter;
using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::Animation::IValueAnimator;
using Elastos::Droid::Animation::IAnimatorUpdateListener;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IVelocityTracker;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::Animation::ILinearInterpolator;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Views {

/**
 * This class facilitates swipe to dismiss. It defines an interface to be implemented by the
 * by the class hosting the views that need to swiped, and, using this interface, handles touch
 * events and translates / fades / animates the view as it is dismissed.
 */
class SwipeHelper : public Object
{
private:
    class MyAnimatorListenerAdapter
        : public AnimatorListenerAdapter
    {
    public:
        MyAnimatorListenerAdapter(
            /* [in] */ ISwipeHelperCallback* callback,
            /* [in] */ IView* view,
            /* [in] */ Boolean canAnimViewBeDismissed);

        // @Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        AutoPtr<ISwipeHelperCallback> mCallback;
        AutoPtr<IView> mView;
        Boolean mCanAnimViewBeDismissed;
    };

    class AnimatorUpdateListener
        : public Object
        , public IAnimatorUpdateListener
    {
    public:
        AnimatorUpdateListener(
            /* [in] */ SwipeHelper* host,
            /* [in] */ IView* view,
            /* [in] */ Boolean canAnimViewBeDismissed);

        CAR_INTERFACE_DECL()

        // @Override
        CARAPI OnAnimationUpdate(
            /* [in] */ IValueAnimator* animation);

    private:
        SwipeHelper* mHost;
        AutoPtr<IView> mView;
        Boolean mCanAnimViewBeDismissed;
    };

    class MyAnimatorListenerAdapter2
        : public AnimatorListenerAdapter
    {
    public:
        MyAnimatorListenerAdapter2(
            /* [in] */ ISwipeHelperCallback* callback,
            /* [in] */ IView* view,
            /* [in] */ Boolean canAnimViewBeDismissed);

        // @Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        AutoPtr<ISwipeHelperCallback> mCallback;
        AutoPtr<IView> mView;
        Boolean mCanAnimViewBeDismissed;
    };

    class AnimatorUpdateListener2
        : public Object
        , public IAnimatorUpdateListener
    {
    public:
        AnimatorUpdateListener2(
            /* [in] */ SwipeHelper* host,
            /* [in] */ IView* view,
            /* [in] */ Boolean canAnimViewBeDismissed);

        CAR_INTERFACE_DECL()

        // @Override
        CARAPI OnAnimationUpdate(
            /* [in] */ IValueAnimator* animation);

    private:
        SwipeHelper* mHost;
        AutoPtr<IView> mView;
        Boolean mCanAnimViewBeDismissed;
    };

public:
    SwipeHelper(
        /* [in] */ Int32 swipeDirection,
        /* [in] */ ISwipeHelperCallback* callback,
        /* [in] */ Float densityScale,
        /* [in] */ Float pagingTouchSlop);

    CARAPI_(void) SetDensityScale(
        /* [in] */ Float densityScale);

    CARAPI_(void) SetPagingTouchSlop(
        /* [in] */ Float pagingTouchSlop);

    CARAPI_(void) CancelOngoingDrag();

    CARAPI_(void) ResetTranslation(
        /* [in] */ IView* v);

    CARAPI_(void) SetMinAlpha(
        /* [in] */ Float minAlpha);

    CARAPI_(Float) GetAlphaForOffset(
        /* [in] */ IView* view);

    /**
     * Determines whether the given view has RTL layout.
     */
    // @TargetApi(Build.VERSION_CODES.JELLY_BEAN_MR1)
    static CARAPI_(Boolean) IsLayoutRtl(
        /* [in] */ IView* view);

    CARAPI_(Boolean) OnInterceptTouchEvent(
        /* [in] */ IMotionEvent* ev);

    CARAPI_(Boolean) OnTouchEvent(
        /* [in] */ IMotionEvent* ev);

private:
    CARAPI_(Float) GetPos(
        /* [in] */ IMotionEvent* ev);

    CARAPI_(Float) GetTranslation(
        /* [in] */ IView* v);

    CARAPI_(Float) GetVelocity(
        /* [in] */ IVelocityTracker* vt);

    CARAPI_(AutoPtr<IObjectAnimator>) CreateTranslationAnimation(
        /* [in] */ IView* v,
        /* [in] */ Float newPos);

    CARAPI_(Float) GetPerpendicularVelocity(
        /* [in] */ IVelocityTracker* vt);

    CARAPI_(void) SetTranslation(
        /* [in] */ IView* v,
        /* [in] */ Float translate);

    CARAPI_(Float) GetSize(
        /* [in] */ IView* v);

    /**
     * @param view The view to be dismissed
     * @param velocity The desired pixels/second speed at which the view should move
     */
    CARAPI_(void) DismissChild(
        /* [in] */ IView* view,
        /* [in] */ Float velocity);


    CARAPI_(void) SnapChild(
        /* [in] */ IView* view,
        /* [in] */ Float velocity);

    CARAPI_(void) SetSwipeAmount(
        /* [in] */ Float amount);

    CARAPI_(Boolean) IsValidSwipeDirection(
        /* [in] */ Float amount);

    CARAPI_(void) EndSwipe(
        /* [in] */ IVelocityTracker* velocityTracker);

public:
    static const String TAG;

    static const Int32 X = 0;
    static const Int32 Y = 1;

    static Float ALPHA_FADE_START; // fraction of thumbnail width
                                                 // where fade starts
    static const Float ALPHA_FADE_END = 0.65f; // fraction of thumbnail width
                                              // beyond which alpha->0
    AutoPtr<ISwipeHelperCallback> mCallback;

    Boolean mAllowSwipeTowardsStart;
    Boolean mAllowSwipeTowardsEnd;

private:
    static const Boolean SLOW_ANIMATIONS; // DEBUG;
    static const Boolean CONSTRAIN_SWIPE;
    static const Boolean FADE_OUT_DURING_SWIPE;
    static const Boolean DISMISS_IF_SWIPED_FAR_ENOUGH;

    static AutoPtr<ILinearInterpolator> sLinearInterpolator;

    static const Int32 SNAP_ANIM_LEN; // ms

    Float SWIPE_ESCAPE_VELOCITY; // dp/sec
    Int32 DEFAULT_ESCAPE_ANIMATION_DURATION; // ms
    Int32 MAX_ESCAPE_ANIMATION_DURATION; // ms
    Int32 MAX_DISMISS_VELOCITY; // dp/sec
    Float mMinAlpha;

    Float mPagingTouchSlop;
    Int32 mSwipeDirection;
    AutoPtr<IVelocityTracker> mVelocityTracker;

    Float mInitialTouchPos;
    Boolean mDragging;

    AutoPtr<IView> mCurrView;
    Boolean mCanCurrViewBeDimissed;
    Float mDensityScale;
    Boolean mRtl;
};

} // namespace Views
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_SWIPEHELPER_H__
