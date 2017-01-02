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

#ifndef __ELASTOS_DROID_TRANSITION_CEXPLODE_H__
#define __ELASTOS_DROID_TRANSITION_CEXPLODE_H__

#include "_Elastos_Droid_Transition_CExplode.h"
#include "elastos/droid/transition/Visibility.h"

namespace Elastos {
namespace Droid {
namespace Transition {

CarClass(CExplode)
    , public Visibility
{
public:
    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI CaptureStartValues(
        /* [in] */ ITransitionValues* transitionValues);

    CARAPI CaptureEndValues(
        /* [in] */ ITransitionValues* transitionValues);

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
        /* [in] */ ITransition* obj);

private:
    CARAPI_(void) CaptureValues(
        /* [in] */ ITransitionValues* transitionValues);

    CARAPI_(void) CalculateOut(
        /* [in] */ IView* sceneRoot,
        /* [in] */ IRect* bounds,
        /* [in] */ ArrayOf<Int32>* outVector);

    static CARAPI_(Float) CalculateMaxDistance(
        /* [in] */ IView* sceneRoot,
        /* [in] */ Int32 focalX,
        /* [in] */ Int32 focalY);

    static CARAPI_(Float) CalculateDistance(
        /* [in] */ Float x,
        /* [in] */ Float y);

private:
    static AutoPtr<ITimeInterpolator> sDecelerate;
    static AutoPtr<ITimeInterpolator> sAccelerate;
    static const String TAG;
    static const String PROPNAME_SCREEN_BOUNDS;

    AutoPtr<ArrayOf<Int32> > mTempLoc;
};

} // namespace Transition
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TRANSITION_CEXPLODE_H__
