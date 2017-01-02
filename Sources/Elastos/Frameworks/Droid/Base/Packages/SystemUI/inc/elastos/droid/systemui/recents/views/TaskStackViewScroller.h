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

#ifndef  __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_TASKSTACKVIEWSCROLLER_H__
#define  __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_TASKSTACKVIEWSCROLLER_H__

#include <elastos/droid/animation/AnimatorListenerAdapter.h>
#include "elastos/droid/systemui/recents/views/TaskStackViewLayoutAlgorithm.h"
#include "elastos/droid/systemui/recents/RecentsConfiguration.h"
#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.Widget.h"

using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::AnimatorListenerAdapter;
using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::Animation::IValueAnimator;
using Elastos::Droid::Animation::IAnimatorUpdateListener;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Widget::IOverScroller;
using Elastos::Droid::SystemUI::Recents::IRecentsConfiguration;
using Elastos::Droid::SystemUI::Recents::RecentsConfiguration;
using Elastos::Core::IRunnable;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Views {

/* The scrolling logic for a TaskStackView */
class TaskStackViewScroller
    : public Object
    , public ITaskStackViewScroller
{
private:
    class MyAnimatorListenerAdapter
        : public AnimatorListenerAdapter
    {
    public:
        TO_STRING_IMPL("TaskStackViewScroller::MyAnimatorListenerAdapter")

        MyAnimatorListenerAdapter(
            /* [in] */ IRunnable* postRunnable,
            /* [in] */ IObjectAnimator* scrollAnimator);

        // @Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        AutoPtr<IRunnable> mPostRunnable;
        AutoPtr<IObjectAnimator> mScrollAnimator;
    };

    class AnimatorUpdateListener
        : public Object
        , public IAnimatorUpdateListener
    {
    public:
        CAR_INTERFACE_DECL()

        TO_STRING_IMPL("TaskStackViewScroller::AnimatorUpdateListener")

        AnimatorUpdateListener(
            /* [in] */ TaskStackViewScroller* host);

        // @Override
        CARAPI OnAnimationUpdate(
            /* [in] */ IValueAnimator* animation);

    private:
        TaskStackViewScroller* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    TaskStackViewScroller();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IRecentsConfiguration* config,
        /* [in] */ ITaskStackViewLayoutAlgorithm* layoutAlgorithm);

    /** Sets the callbacks */
    CARAPI_(void) SetCallbacks(
        /* [in] */ ITaskStackViewScrollerCallbacks* cb);

    /** Gets the current stack scroll */
    CARAPI GetStackScroll(
        /* [out] */ Float* scroll);

    /** Gets the current stack scroll */
    CARAPI_(Float) GetStackScroll();

    /** Sets the current stack scroll */
    CARAPI SetStackScroll(
        /* [in] */ Float s);

    /** Sets the current stack scroll without calling the callback. */
    CARAPI_(void) SetStackScrollRaw(
        /* [in] */ Float s);

    /** Sets the current stack scroll to the initial state when you first enter recents */
    CARAPI_(void) SetStackScrollToInitialState();

    /** Bounds the current scroll if necessary */
    CARAPI_(Boolean) BoundScroll();

    /** Bounds the current scroll if necessary, but does not synchronize the stack view with the model. */
    CARAPI_(Boolean) BoundScrollRaw();

    /** Returns the bounded stack scroll */
    CARAPI_(Float) GetBoundedStackScroll(
        /* [in] */ Float scroll);

    /** Returns the amount that the aboslute value of how much the scroll is out of bounds. */
    CARAPI_(Float) GetScrollAmountOutOfBounds(
        /* [in] */ Float scroll);

    /** Returns whether the specified scroll is out of bounds */
    CARAPI_(Boolean) IsScrollOutOfBounds();

    /** Animates the stack scroll into bounds */
    CARAPI_(AutoPtr<IObjectAnimator>) AnimateBoundScroll();

    /** Animates the stack scroll */
    CARAPI_(void) AnimateScroll(
        /* [in] */ Float curScroll,
        /* [in] */ Float newScroll,
        /* [in] */ IRunnable* postRunnable);

    /** Aborts any current stack scrolls */
    CARAPI_(void) StopBoundScrollAnimation();

    /**** OverScroller ****/

    CARAPI_(Int32) ProgressToScrollRange(
        /* [in] */ Float p);

    CARAPI_(Float) ScrollRangeToProgress(
        /* [in] */ Int32 s);

    /** Called from the view draw, computes the next scroll. */
    CARAPI_(Boolean) ComputeScroll();

    /** Returns whether the overscroller is scrolling. */
    CARAPI_(Boolean) IsScrolling();

    /** Stops the scroller and any current fling. */
    CARAPI_(void) StopScroller();

public:
    AutoPtr<RecentsConfiguration> mConfig;
    AutoPtr<TaskStackViewLayoutAlgorithm> mLayoutAlgorithm;
    AutoPtr<ITaskStackViewScrollerCallbacks> mCb;

    Float mStackScrollP;

    AutoPtr<IOverScroller> mScroller;
    AutoPtr<IObjectAnimator> mScrollAnimator;
};

} // namespace Views
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_TASKSTACKVIEWSCROLLER_H__
