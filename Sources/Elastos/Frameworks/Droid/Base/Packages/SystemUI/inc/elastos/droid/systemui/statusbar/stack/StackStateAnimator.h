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

#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_STACK_STACKSTATEANIMATOR_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_STACK_STACKSTATEANIMATOR_H__

#include "_Elastos.Droid.SystemUI.h"
#include "elastos/droid/systemui/statusbar/stack/AnimationFilter.h"
#include "elastos/droid/systemui/statusbar/stack/StackScrollState.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.View.h"
#include <elastos/droid/animation/AnimatorListenerAdapter.h>
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Runnable.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Animation::AnimatorListenerAdapter;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::IAnimatorUpdateListener;
using Elastos::Droid::Animation::IValueAnimator;
using Elastos::Core::Runnable;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::Animation::IInterpolator;
using Elastos::Core::Object;
using Elastos::Utility::IArrayList;
using Elastos::Utility::ISet;
using Elastos::Utility::IStack;

#define ViewState StackScrollState::ViewState

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Stack {

/**
 * An stack state animator which handles animations to new StackScrollStates
 */
class StackStateAnimator
    : public Object
    , public IStackStateAnimator
{
private:
    class AnimatorUpdateListener1
        : public Object
        , public IAnimatorUpdateListener
    {
    public:
        CAR_INTERFACE_DECL()

        AnimatorUpdateListener1(
            /* [in] */ IExpandableView* child);

        // @Override
        CARAPI OnAnimationUpdate(
            /* [in] */ IValueAnimator* animation);

    private:
        AutoPtr<IExpandableView> mChild;
    };

    class AnimatorListenerAdapter1: public AnimatorListenerAdapter
    {
    public:
        AnimatorListenerAdapter1(
            /* [in] */ IExpandableView* child);

        // @Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        AutoPtr<IExpandableView> mChild;
    };

    class AnimatorUpdateListener2
        : public Object
        , public IAnimatorUpdateListener
    {
    public:
        CAR_INTERFACE_DECL()

        AnimatorUpdateListener2(
            /* [in] */ IExpandableView* child);

        // @Override
        CARAPI OnAnimationUpdate(
            /* [in] */ IValueAnimator* animation);

    private:
        AutoPtr<IExpandableView> mChild;
    };

    class AnimatorListenerAdapter2: public AnimatorListenerAdapter
    {
    public:
        AnimatorListenerAdapter2(
            /* [in] */ IExpandableView* child);

        // @Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        AutoPtr<IExpandableView> mChild;
    };

    class AnimatorListenerAdapter3: public AnimatorListenerAdapter
    {
    public:
        AnimatorListenerAdapter3(
            /* [in] */ IExpandableView* child,
            /* [in] */ Float newEndValue);

        // @Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

        // @Override
        CARAPI OnAnimationCancel(
            /* [in] */ IAnimator* animation);

        // @Override
        CARAPI OnAnimationStart(
            /* [in] */ IAnimator* animation);

    public:
        Boolean mWasCancelled;

    private:
        AutoPtr<IExpandableView> mChild;
        Float mNewEndValue;
    };

    class RemoveTagAdapter: public AnimatorListenerAdapter
    {
    public:
        RemoveTagAdapter(
            /* [in] */ IExpandableView* child);

        // @Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);
    private:
        AutoPtr<IExpandableView> mChild;
    };

    class AnimatorListenerAdapter5: public AnimatorListenerAdapter
    {
    public:
        AnimatorListenerAdapter5(
            /* [in] */ IExpandableView* child);

        // @Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        AutoPtr<IExpandableView> mChild;
    };

    class AnimatorListenerAdapter6: public AnimatorListenerAdapter
    {
    public:
        AnimatorListenerAdapter6(
            /* [in] */ IExpandableView* child);

        // @Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        AutoPtr<IExpandableView> mChild;
    };

    class AnimatorListenerAdapter7: public AnimatorListenerAdapter
    {
    public:
        AnimatorListenerAdapter7(
            /* [in] */ IExpandableView* child);

        // @Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        AutoPtr<IExpandableView> mChild;
    };

    class AnimatorListenerAdapter8: public AnimatorListenerAdapter
    {
    public:
        AnimatorListenerAdapter8(
            /* [in] */ StackStateAnimator* host);

        // @Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

        // @Override
        CARAPI OnAnimationCancel(
            /* [in] */ IAnimator* animation);

        // @Override
        CARAPI OnAnimationStart(
            /* [in] */ IAnimator* animation);

    private:
        StackStateAnimator* mHost;
        Boolean mWasCancelled;
    };

    class Runnable1: public Runnable
    {
    public:
        Runnable1(
            /* [in] */ StackStateAnimator* host,
            /* [in] */ IExpandableView* changingView);

        // @Override
        CARAPI Run();

    private:
        StackStateAnimator* mHost;
        AutoPtr<IExpandableView> mChangingView;
    };

    class AnimatorUpdateListener3
        : public Object
        , public IAnimatorUpdateListener
    {
    public:
        CAR_INTERFACE_DECL()

        AnimatorUpdateListener3(
            /* [in] */ StackStateAnimator* host,
            /* [in] */ Boolean onTop,
            /* [in] */ Boolean isRubberbanded);

        // @Override
        CARAPI OnAnimationUpdate(
            /* [in] */ IValueAnimator* animation);

    private:
        StackStateAnimator* mHost;
        Boolean mOnTop;
        Boolean mIsRubberbanded;
    };

    class AnimatorListenerAdapter9: public AnimatorListenerAdapter
    {
    public:
        AnimatorListenerAdapter9(
            /* [in] */ StackStateAnimator* host,
            /* [in] */ Boolean onTop);

        // @Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        StackStateAnimator* mHost;
        Boolean mOnTop;
    };

public:
    CAR_INTERFACE_DECL()

    StackStateAnimator(
        /* [in] */ INotificationStackScrollLayout* hostLayout);

    CARAPI IsRunning(
        /* [out] */ Boolean* result);

    CARAPI StartAnimationForEvents(
        /* [in] */ IArrayList/*<NotificationStackScrollLayout.AnimationEvent>*/* mAnimationEvents,
        /* [in] */ IStackScrollState* finalState,
        /* [in] */ Int64 additionalDelay);

    CARAPI AnimateOverScrollToAmount(
        /* [in] */ Float targetAmount,
        /* [in] */ Boolean onTop,
        /* [in] */ Boolean isRubberbanded);

    CARAPI CancelOverScrollAnimators(
        /* [in] */ Boolean onTop);

    /**
     * Get the end value of the height animation running on a view or the actualHeight
     * if no animation is running.
     */
    static CARAPI_(Int32) GetFinalActualHeight(
        /* [in] */ IExpandableView* view);

private:
    CARAPI_(Int32) FindLastNotAddedIndex(
        /* [in] */ StackScrollState* finalState);

    /**
     * Start an animation to the given viewState
     */
    CARAPI_(void) StartAnimations(
        /* [in] */ IExpandableView* child,
        /* [in] */ ViewState* viewState,
        /* [in] */ StackScrollState* finalState,
        /* [in] */ Int32 i);

    CARAPI_(Int64) CalculateChildAnimationDelay(
        /* [in] */ ViewState* viewState,
        /* [in] */ StackScrollState* finalState);

    CARAPI_(Int64) CalculateDelayGoToFullShade(
        /* [in] */ ViewState* viewState);

    CARAPI_(void) StartHeightAnimation(
        /* [in] */ IExpandableView* child,
        /* [in] */ ViewState* viewState,
        /* [in] */ Int64 duration,
        /* [in] */ Int64 delay);

    CARAPI_(void) StartInsetAnimation(
        /* [in] */ IExpandableView* child,
        /* [in] */ ViewState* viewState,
        /* [in] */ Int64 duration,
        /* [in] */ Int64 delay);

    CARAPI_(void) StartAlphaAnimation(
        /* [in] */ IExpandableView* child,
        /* [in] */ ViewState* viewState,
        /* [in] */ Int64 duration,
        /* [in] */ Int64 delay);

    CARAPI_(void) StartZTranslationAnimation(
        /* [in] */ IExpandableView* child,
        /* [in] */ ViewState* viewState,
        /* [in] */ Int64 duration,
        /* [in] */ Int64 delay);

    CARAPI_(void) StartYTranslationAnimation(
        /* [in] */ IExpandableView* child,
        /* [in] */ ViewState* viewState,
        /* [in] */ Int64 duration,
        /* [in] */ Int64 delay);

    CARAPI_(void) StartScaleAnimation(
        /* [in] */ IExpandableView* child,
        /* [in] */ ViewState* viewState,
        /* [in] */ Int64 duration);

    CARAPI_(void) StartAnimator(
        /* [in] */ IValueAnimator* animator);

    /**
     * @return an adapter which ensures that onAnimationFinished is called once no animation is
     *         running anymore
     */
    CARAPI_(AutoPtr<AnimatorListenerAdapter>) GetGlobalAnimationFinishedListener();

    static CARAPI_(AutoPtr<IInterface>) GetChildTag(
        /* [in] */ IView* child,
        /* [in] */ Int32 tag);

    /**
     * Cancel the previous animator and get the duration of the new animation.
     *
     * @param duration the new duration
     * @param previousAnimator the animator which was running before
     * @return the new duration
     */
    CARAPI_(Int64) CancelAnimatorAndGetNewDuration(
        /* [in] */ Int64 duration,
        /* [in] */ IValueAnimator* previousAnimator);

    CARAPI_(void) OnAnimationFinished();

    /**
     * Process the animationEvents for a new animation
     *
     * @param animationEvents the animation events for the animation to perform
     * @param finalState the final state to animate to
     */
    CARAPI_(void) ProcessAnimationEvents(
        /* [in] */ IArrayList/*<NotificationStackScrollLayout.AnimationEvent>*/* animationEvents,
        /* [in] */ StackScrollState* finalState);

public:
    static const Int32 ANIMATION_DURATION_STANDARD;
    static const Int32 ANIMATION_DURATION_GO_TO_FULL_SHADE;
    static const Int32 ANIMATION_DURATION_APPEAR_DISAPPEAR;
    static const Int32 ANIMATION_DURATION_DIMMED_ACTIVATED;
    static const Int32 ANIMATION_DELAY_PER_ELEMENT_INTERRUPTING;
    static const Int32 ANIMATION_DELAY_PER_ELEMENT_MANUAL;
    static const Int32 ANIMATION_DELAY_PER_ELEMENT_GO_TO_FULL_SHADE;

private:
    static const Int32 DELAY_EFFECT_MAX_INDEX_DIFFERENCE;
    static const Int32 TAG_ANIMATOR_TRANSLATION_Y;
    static const Int32 TAG_ANIMATOR_TRANSLATION_Z;
    static const Int32 TAG_ANIMATOR_SCALE;
    static const Int32 TAG_ANIMATOR_ALPHA;
    static const Int32 TAG_ANIMATOR_HEIGHT;
    static const Int32 TAG_ANIMATOR_TOP_INSET;
    static const Int32 TAG_END_TRANSLATION_Y;
    static const Int32 TAG_END_TRANSLATION_Z;
    static const Int32 TAG_END_SCALE;
    static const Int32 TAG_END_ALPHA;
    static const Int32 TAG_END_HEIGHT;
    static const Int32 TAG_END_TOP_INSET;
    static const Int32 TAG_START_TRANSLATION_Y;
    static const Int32 TAG_START_TRANSLATION_Z;
    static const Int32 TAG_START_SCALE;
    static const Int32 TAG_START_ALPHA;
    static const Int32 TAG_START_HEIGHT;
    static const Int32 TAG_START_TOP_INSET;

    AutoPtr<IInterpolator> mFastOutSlowInInterpolator;
    Int32 mGoToFullShadeAppearingTranslation;
    AutoPtr<INotificationStackScrollLayout> mHostLayout;
    AutoPtr<IArrayList> mNewEvents;  /*<NotificationStackScrollLayout.AnimationEvent*/
    AutoPtr<IArrayList> mNewAddChildren;  /*<View*/
    AutoPtr<ISet> mAnimatorSet;  /*<Animator*/
    AutoPtr<IStack> mAnimationListenerPool;  /*<AnimatorListenerAdapter*/
    AutoPtr<AnimationFilter> mAnimationFilter;
    Int64 mCurrentLength;
    Int64 mCurrentAdditionalDelay;

    /** The current index for the last child which was not added in this event set. */
    Int32 mCurrentLastNotAddedIndex;

    AutoPtr<IValueAnimator> mTopOverScrollAnimator;
    AutoPtr<IValueAnimator> mBottomOverScrollAnimator;
};

} // namespace Stack
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_STACK_STACKSTATEANIMATOR_H__
