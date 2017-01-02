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

#ifndef __ELASTOS_DROID_TRANSITION_CFADE_H__
#define __ELASTOS_DROID_TRANSITION_CFADE_H__

#include "_Elastos_Droid_Transition_CFade.h"
#include "elastos/droid/transition/Visibility.h"

namespace Elastos {
namespace Droid {
namespace Transition {

CarClass(CFade)
    , public Visibility
    , public IFade
{
private:
    class FadeAnimatorListener
        : public AnimatorListenerAdapter
    {
    public:
        FadeAnimatorListener(
            /* [in] */ IView* view);

        CARAPI OnAnimationStart(
            /* [in] */ IAnimator* animator);

        CARAPI OnAnimationCancel(
            /* [in] */ IAnimator* animator);

        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animator);

        CARAPI OnAnimationPause(
            /* [in] */ IAnimator* animator);

        CARAPI OnAnimationResume(
            /* [in] */ IAnimator* animator);

    private:
        AutoPtr<IView> mView;
        Boolean mCanceled;
        Float mPausedAlpha;
        Boolean mLayerTypeChanged;
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    /**
     * Constructs a Fade transition that will fade targets in and out.
     */
    CARAPI constructor();

    /**
     * Constructs a Fade transition that will fade targets in
     * and/or out, according to the value of fadingMode.
     *
     * @param fadingMode The behavior of this transition, a combination of
     * {@link #IN} and {@link #OUT}.
     */
    CARAPI constructor(
        /* [in] */ Int32 fadingMode);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI OnAppear(
        /* [in] */ IViewGroup* sceneRoot,
        /* [in] */ IView* view,
        /* [in] */ ITransitionValues* startValues,
        /* [in] */ ITransitionValues* endValues,
        /* [out] */ IAnimator** result);

    CARAPI OnDisappear(
        /* [in] */ IViewGroup* sceneRoot,
        /* [in] */ IView* view,
        /* [in] */ ITransitionValues* startValues,
        /* [in] */ ITransitionValues* endValues,
        /* [out] */ IAnimator** result);

    CARAPI Clone(
        /* [out] */ IInterface** obj);

    CARAPI CloneImpl(
        /* [in] */ IFade* obj);

private:
    /**
     * Utility method to handle creating and running the Animator.
     */
    CARAPI_(AutoPtr<IAnimator>) CreateAnimation(
        /* [in] */ IView* view,
        /* [in] */ Float startAlpha,
        /* [in] */ Float endAlpha);

public:
    /**
     * Fading mode used in {@link #Fade(int)} to make the transition
     * operate on targets that are appearing. Maybe be combined with
     * {@link #OUT} to fade both in and out. Equivalent to
     * {@link Visibility#MODE_IN}.
     */
    static Int32 IN;

    /**
     * Fading mode used in {@link #Fade(int)} to make the transition
     * operate on targets that are disappearing. Maybe be combined with
     * {@link #IN} to fade both in and out. Equivalent to
     * {@link Visibility#MODE_OUT}.
     */
    static Int32 OUT;

private:
    static Boolean DBG;

    static const String TAG;
};

} // namespace Transition
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TRANSITION_CFADE_H__
