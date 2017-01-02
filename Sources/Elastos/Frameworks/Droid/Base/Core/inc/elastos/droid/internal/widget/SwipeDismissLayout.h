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

#ifndef __ELASTOS_DROID_INTERNAL_WIDGET_SWIPEDISMISSLAYOUT_H__
#define __ELASTOS_DROID_INTERNAL_WIDGET_SWIPEDISMISSLAYOUT_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/widget/FrameLayout.h"

using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::Widget::IOnDismissedListener;
using Elastos::Droid::Internal::Widget::IOnSwipeProgressChangedListener;
using Elastos::Droid::Internal::Widget::ISwipeDismissLayout;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IVelocityTracker;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::FrameLayout;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

class SwipeDismissLayout
    : public FrameLayout
    , public ISwipeDismissLayout
{
public:
    CAR_INTERFACE_DECL()

    SwipeDismissLayout();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    virtual CARAPI SetOnDismissedListener(
        /* [in] */ IOnDismissedListener* listener);

    virtual CARAPI SetOnSwipeProgressChangedListener(
        /* [in] */ IOnSwipeProgressChangedListener* listener);

    // @Override
    CARAPI OnInterceptTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* result);

protected:
    virtual CARAPI_(void) Cancel();

    /**
      * Tests scrollability within child views of v in the direction of dx.
      *
      * @param v View to test for horizontal scrollability
      * @param checkV Whether the view v passed should itself be checked for scrollability (true),
      *               or just its children (false).
      * @param dx Delta scrolled in pixels. Only the sign of this is used.
      * @param x X coordinate of the active touch point
      * @param y Y coordinate of the active touch point
      * @return true if child views of v can be scrolled by delta of dx.
      */
    virtual CARAPI_(Boolean) CanScroll(
        /* [in] */ IView* v,
        /* [in] */ Boolean checkV,
        /* [in] */ Float dx,
        /* [in] */ Float x,
        /* [in] */ Float y);

private:
    CARAPI_(void) Init(
        /* [in] */ IContext* context);

    CARAPI_(void) SetProgress(
        /* [in] */ Float deltaX);

    CARAPI_(void) Dismiss();

    /**
      * Resets internal members when canceling.
      */
    CARAPI_(void) ResetMembers();

    CARAPI_(void) UpdateSwiping(
        /* [in] */ IMotionEvent* ev);

    CARAPI_(void) UpdateDismiss(
        /* [in] */ IMotionEvent* ev);

private:
    static const String TAG;
    static const Float DISMISS_MIN_DRAG_WIDTH_RATIO;
    // Cached ViewConfiguration and system-wide constant values
    Int32 mSlop;
    Int32 mMinFlingVelocity;
    Int32 mMaxFlingVelocity;
    Int64 mAnimationTime;
    AutoPtr<ITimeInterpolator> mCancelInterpolator;
    AutoPtr<ITimeInterpolator> mDismissInterpolator;
    // Transient properties
    Int32 mActiveTouchId;
    Float mDownX;
    Float mDownY;
    Boolean mSwiping;
    Boolean mDismissed;
    Boolean mDiscardIntercept;
    AutoPtr<IVelocityTracker> mVelocityTracker;
    Float mTranslationX;
    AutoPtr<IOnDismissedListener> mDismissedListener;
    AutoPtr<IOnSwipeProgressChangedListener> mProgressListener;
    Float mLastX;
};

} // namespace Widget
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_WIDGET_SWIPEDISMISSLAYOUT_H__

