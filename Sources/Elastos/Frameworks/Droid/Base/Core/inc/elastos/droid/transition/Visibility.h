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

#ifndef __ELASTOS_DROID_TRANSITION_VISIBILITY_H__
#define __ELASTOS_DROID_TRANSITION_VISIBILITY_H__

#include "elastos/droid/transition/Transition.h"

#include <elastos/core/Object.h>

using Elastos::Droid::Animation::IAnimatorListener;

namespace Elastos {
namespace Droid {
namespace Transition {

/**
 * This transition tracks changes to the visibility of target views in the
 * start and end scenes. Visibility is determined not just by the
 * {@link View#setVisibility(int)} state of views, but also whether
 * views exist in the current view hierarchy. The class is intended to be a
 * utility for subclasses such as {@link Fade}, which use this visibility
 * information to determine the specific animations to run when visibility
 * changes occur. Subclasses should implement one or both of the methods
 * {@link #onAppear(ViewGroup, TransitionValues, int, TransitionValues, int)},
 * {@link #onDisappear(ViewGroup, TransitionValues, int, TransitionValues, int)} or
 * {@link #onAppear(ViewGroup, View, TransitionValues, TransitionValues)},
 * {@link #onDisappear(ViewGroup, View, TransitionValues, TransitionValues)}.
 */
class Visibility
    : public Transition
    , public IVisibility
{
protected:
    class VisibilityInfo
        : public Object
    {
    public:
        Boolean mVisibilityChange;
        Boolean mFadeIn;
        Int32 mStartVisibility;
        Int32 mEndVisibility;
        AutoPtr<IViewGroup> mStartParent;
        AutoPtr<IViewGroup> mEndParent;
    };

private:
    class AnimatorListenerAdapter_1
        : public AnimatorListenerAdapter
    {
    public:
        AnimatorListenerAdapter_1(
            /* [in] */ IViewGroup* finalSR,
            /* [in] */ IView* finalOV);

        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    public:
        AutoPtr<IViewGroup> mFinalSceneRoot;
        AutoPtr<IView> mFinalOverlayView;
    };

    class AnimatorListenerAdapter_2
        : public AnimatorListenerAdapter
    {
    public:
        AnimatorListenerAdapter_2(
            /* [in] */ Boolean isFV,
            /* [in] */ IView* fVT,
            /* [in] */ Int32 fV);

        CARAPI OnAnimationPause(
            /* [in] */ IAnimator* animation);

        CARAPI OnAnimationResume(
            /* [in] */ IAnimator* animation);

        CARAPI OnAnimationCancel(
            /* [in] */ IAnimator* animation);

        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    public:
        Boolean mCanceled;
        Boolean mIsForcedVisibility;
        AutoPtr<IView> mFinalViewToKeep;
        Int32 mFinalVisibility;
    };

public:
    CAR_INTERFACE_DECL()

    Visibility();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    /**
     * Changes the transition to support appearing and/or disappearing Views, depending
     * on <code>mode</code>.
     *
     * @param mode The behavior supported by this transition, a combination of
     *             {@link #MODE_IN} and {@link #MODE_OUT}.
     * @attr ref android.R.styleable#VisibilityTransition_transitionVisibilityMode
     */
    CARAPI SetMode(
        /* [in] */ Int32 mode);

    /**
     * Returns whether appearing and/or disappearing Views are supported.
     *
     * Returns whether appearing and/or disappearing Views are supported. A combination of
     *         {@link #MODE_IN} and {@link #MODE_OUT}.
     * @attr ref android.R.styleable#VisibilityTransition_transitionVisibilityMode
     */
    CARAPI GetMode(
        /* [out] */ Int32* result);

    CARAPI GetTransitionProperties(
        /* [out] */ ArrayOf<String>** result);

    CARAPI CaptureStartValues(
        /* [in] */ ITransitionValues* transitionValues);

    CARAPI CaptureEndValues(
        /* [in] */ ITransitionValues* transitionValues);

    /** @hide */
    CARAPI ForceVisibility(
        /* [in] */ Int32 visibility,
        /* [in] */ Boolean isStartValue);

    /**
     * Returns whether the view is 'visible' according to the given values
     * object. This is determined by testing the same properties in the values
     * object that are used to determine whether the object is appearing or
     * disappearing in the {@link
     * Transition#createAnimator(ViewGroup, TransitionValues, TransitionValues)}
     * method. This method can be called by, for example, subclasses that want
     * to know whether the object is visible in the same way that Visibility
     * determines it for the actual animation.
     *
     * @param values The TransitionValues object that holds the information by
     * which visibility is determined.
     * @return True if the view reference by <code>values</code> is visible,
     * false otherwise.
     */
    CARAPI IsVisible(
        /* [in] */ ITransitionValues* values,
        /* [out] */ Boolean* result);

    CARAPI CreateAnimator(
        /* [in] */ IViewGroup* sceneRoot,
        /* [in] */ ITransitionValues* startValues,
        /* [in] */ ITransitionValues* endValues,
        /* [out] */ IAnimator** result);

    CARAPI OnAppear(
        /* [in] */ IViewGroup* sceneRoot,
        /* [in] */ ITransitionValues* startValues,
        /* [in] */ Int32 startVisibility,
        /* [in] */ ITransitionValues* endValues,
        /* [in] */ Int32 endVisibility,
        /* [out] */ IAnimator** result);

    CARAPI OnAppear(
        /* [in] */ IViewGroup* sceneRoot,
        /* [in] */ IView* view,
        /* [in] */ ITransitionValues* startValues,
        /* [in] */ ITransitionValues* endValues,
        /* [out] */ IAnimator** result);

    CARAPI OnDisappear(
        /* [in] */ IViewGroup* sceneRoot,
        /* [in] */ ITransitionValues* startValues,
        /* [in] */ Int32 startVisibility,
        /* [in] */ ITransitionValues* endValues,
        /* [in] */ Int32 endVisibility,
        /* [out] */ IAnimator** result);

    CARAPI AreValuesChanged(
        /* [in] */ ITransitionValues* oldValues,
        /* [in] */ ITransitionValues* newValues,
        /* [out] */ Boolean* result);

    CARAPI OnDisappear(
        /* [in] */ IViewGroup* sceneRoot,
        /* [in] */ IView* view,
        /* [in] */ ITransitionValues* startValues,
        /* [in] */ ITransitionValues* endValues,
        /* [out] */ IAnimator** result);

private:
    using Transition::CaptureValues;

    CARAPI_(void) CaptureValues(
        /* [in] */ ITransitionValues* transitionValues,
        /* [in] */ Int32 forcedVisibility);

    CARAPI_(AutoPtr<VisibilityInfo>) GetVisibilityChangeInfo(
        /* [in] */ ITransitionValues* startValues,
        /* [in] */ ITransitionValues* endValues);

public:
    static String PROPNAME_VISIBILITY;

    /**
     * Mode used in {@link #setMode(int)} to make the transition
     * operate on targets that are appearing. Maybe be combined with
     * {@link #MODE_OUT} to target Visibility changes both in and out.
     */
    static Int32 MODE_IN;

    /**
     * Mode used in {@link #setMode(int)} to make the transition
     * operate on targets that are disappearing. Maybe be combined with
     * {@link #MODE_IN} to target Visibility changes both in and out.
     */
    static Int32 MODE_OUT;

private:
    static String PROPNAME_PARENT;
    static String PROPNAME_SCREEN_LOCATION;
    static AutoPtr<ArrayOf<String> > sTransitionProperties;

    Int32 mMode;

    Int32 mForcedStartVisibility;
    Int32 mForcedEndVisibility;
};

} // namespace Transition
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TRANSITION_VISIBILITY_H__
