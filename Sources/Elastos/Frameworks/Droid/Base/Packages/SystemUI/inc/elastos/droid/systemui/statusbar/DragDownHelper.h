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

#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_DRAGDOWNHELPER_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_DRAGDOWNHELPER_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.View.h"
#include <elastos/droid/animation/AnimatorListenerAdapter.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Animation::AnimatorListenerAdapter;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::IAnimatorUpdateListener;
using Elastos::Droid::Animation::IValueAnimator;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::View::Animation::IInterpolator;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IView;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

/**
 * A utility class to enable the downward swipe on the lockscreen to go to the full shade and expand
 * the notification where the drag started.
 */
class DragDownHelper
    : public Object
    , public IDragDownHelper
    , public IGefingerpoken
{
private:
    class AnimatorListenerAdapter1
        : public AnimatorListenerAdapter
    {
    public:
        AnimatorListenerAdapter1(
            /* [in] */ DragDownHelper* host,
            /* [in] */ IExpandableView* child);

        // @Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        DragDownHelper* mHost;
        AutoPtr<IExpandableView> mChild;
    };

    class AnimatorUpdateListener
        : public Object
        , public IAnimatorUpdateListener
    {
    public:
        CAR_INTERFACE_DECL()

        AnimatorUpdateListener(
            /* [in] */ DragDownHelper* host);

        // @Override
        CARAPI OnAnimationUpdate(
            /* [in] */ IValueAnimator* animation);

    private:
        DragDownHelper* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    DragDownHelper(
        /* [in] */ IContext* context,
        /* [in] */ IView* host,
        /* [in] */ IExpandHelperCallback* callback,
        /* [in] */ IDragDownCallback* dragDownCallback);

    // @Override
    CARAPI OnInterceptTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

private:
    CARAPI_(void) CaptureStartingChild(
        /* [in] */ Float x,
        /* [in] */ Float y);

    CARAPI_(void) HandleExpansion(
        /* [in] */ Float heightDelta,
        /* [in] */ IExpandableView* child);

    CARAPI_(void) CancelExpansion(
        /* [in] */ IExpandableView* child);

    CARAPI_(void) CancelExpansion();

    CARAPI_(void) StopDragging();

    CARAPI_(AutoPtr<IExpandableView>) FindView(
        /* [in] */ Float x,
        /* [in] */ Float y);

private:
    static const Float RUBBERBAND_FACTOR_EXPANDABLE;
    static const Float RUBBERBAND_FACTOR_STATIC;
    static const Int32 SPRING_BACK_ANIMATION_LENGTH_MS;

    Int32 mMinDragDistance;
    AutoPtr<IExpandHelperCallback> mCallback;
    Float mInitialTouchX;
    Float mInitialTouchY;
    Boolean mDraggingDown;
    Float mTouchSlop;
    AutoPtr<IDragDownCallback> mDragDownCallback;
    AutoPtr<IView> mHost;
    AutoPtr<ArrayOf<Int32> > mTemp2;
    Boolean mDraggedFarEnough;
    AutoPtr<IExpandableView> mStartingChild;
    AutoPtr<IInterpolator> mInterpolator;
    Float mLastHeight;
};

} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_DRAGDOWNHELPER_H__
