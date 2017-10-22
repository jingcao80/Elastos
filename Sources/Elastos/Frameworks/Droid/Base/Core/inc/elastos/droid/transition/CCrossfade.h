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

#ifndef __ELASTOS_DROID_TRANSITION_CCROSSFADE_H__
#define __ELASTOS_DROID_TRANSITION_CCROSSFADE_H__

#include "_Elastos_Droid_Transition_CCrossfade.h"
#include "elastos/droid/transition/Transition.h"

#include <elastos/core/Object.h>

using Elastos::Droid::Animation::IAnimatorUpdateListener;
using Elastos::Droid::Animation::IValueAnimator;
using Elastos::Droid::Animation::ITypeEvaluator;
using Elastos::Droid::Graphics::Drawable::IBitmapDrawable;

namespace Elastos {
namespace Droid {
namespace Transition {

/**
 * This transition captures bitmap representations of target views before and
 * after the scene change and fades between them.
 *
 * <p>Note: This transition is not compatible with {@link TextureView}
 * or {@link SurfaceView}.</p>
 *
 * @hide
 */
CarClass(CCrossfade)
    , public Transition
    , public ICrossfade
{
    // TODO: Add a hook that lets a Transition call user code to query whether it should run on
    // a given target view. This would save bitmap comparisons in this transition, for example.
private:
    class AnimatorUpdateListenerOverride
        : public Object
        , public IAnimatorUpdateListener
    {
    public:
        CAR_INTERFACE_DECL()

        AnimatorUpdateListenerOverride(
            /* [in] */ IBitmapDrawable* startDrawable,
            /* [in] */ IView* view);

        CARAPI OnAnimationUpdate(
            /* [in] */ IValueAnimator* animation);

    public:
        IBitmapDrawable* mStartDrawable;
        IView* mView;
    };

    class AnimatorListenerAdapter_1
        : public AnimatorListenerAdapter
    {
    public:
        AnimatorListenerAdapter_1(
            /* [in] */ Boolean useParentOverlay,
            /* [in] */ IView* view,
            /* [in] */ IBitmapDrawable* startDrawable,
            /* [in] */ IBitmapDrawable* endDrawable,
            /* [in] */ Int32 fadeBehavior);

        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    public:
        Boolean mUseParentOverlay;
        IView* mView;
        IBitmapDrawable* mStartDrawable;
        IBitmapDrawable* mEndDrawable;
        Int32 mFadeBehavior;
    };
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI SetFadeBehavior(
        /* [in] */ Int32 fadeBehavior,
        /* [out] */ ICrossfade** result);

    CARAPI GetFadeBehavior(
        /* [out] */ Int32* result);

    CARAPI SetResizeBehavior(
        /* [in] */ Int32 resizeBehavior,
        /* [out] */ ICrossfade** result);

    CARAPI GetResizeBehavior(
        /* [out] */ Int32* result);

    CARAPI CreateAnimator(
        /* [in] */ IViewGroup* sceneRoot,
        /* [in] */ ITransitionValues* startValues,
        /* [in] */ ITransitionValues* endValues,
        /* [out] */ IAnimator** result);

    CARAPI CaptureStartValues(
        /* [in] */ ITransitionValues* transitionValues);

    CARAPI CaptureEndValues(
        /* [in] */ ITransitionValues* transitionValues);

    CARAPI Clone(
        /* [out] */ IInterface** obj);

    CARAPI CloneImpl(
        /* [in] */ ICrossfade* obj);

private:
    using Transition::CaptureValues;

    CARAPI CaptureValues(
        /* [in] */ ITransitionValues* transitionValues);

private:
    static const String TAG;

    static const String PROPNAME_BITMAP;
    static const String PROPNAME_DRAWABLE;
    static const String PROPNAME_BOUNDS;

    static AutoPtr<ITypeEvaluator> sRectEvaluator;

    Int32 mFadeBehavior;
    Int32 mResizeBehavior;
};

} // namespace Transition
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TRANSITION_CCROSSFADE_H__
