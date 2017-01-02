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

#ifndef __ELASTOS_DROID_ANIMATION_CARGBEVALUATOR_H__
#define __ELASTOS_DROID_ANIMATION_CARGBEVALUATOR_H__

#include "_Elastos_Droid_Animation_CArgbEvaluator.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Animation {

/**
 * This evaluator can be used to perform type interpolation between integer
 * values that represent ARGB colors.
 */
CarClass(CArgbEvaluator)
    , public Object
    , public IArgbEvaluator
    , public ITypeEvaluator
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CArgbEvaluator();

    ~CArgbEvaluator();

    CARAPI constructor();

    /**
     * Returns an instance of <code>ArgbEvaluator</code> that may be used in
     * {@link ValueAnimator#setEvaluator(TypeEvaluator)}. The same instance may
     * be used in multiple <code>Animator</code>s because it holds no state.
     * @return An instance of <code>ArgbEvalutor</code>.
     *
     * @hide
     */
    static CARAPI_(AutoPtr<IArgbEvaluator>) GetInstance();

    /**
     * This function returns the calculated in-between value for a color
     * given integers that represent the start and end values in the four
     * bytes of the 32-bit int. Each channel is separately linearly interpolated
     * and the resulting calculated values are recombined into the return value.
     *
     * @param fraction The fraction from the starting to the ending values
     * @param startValue A 32-bit int value representing colors in the
     * separate bytes of the parameter
     * @param endValue A 32-bit int value representing colors in the
     * separate bytes of the parameter
     * @return A value that is calculated to be the linearly interpolated
     * result, derived by separating the start and end values into separate
     * color channels and interpolating each one separately, recombining the
     * resulting values in the same way.
     */
    CARAPI Evaluate(
        /* [in] */ Float fraction,
        /* [in] */ IInterface* startValue,
        /* [in] */ IInterface* endValue,
        /* [out] */ IInterface** result);

private:
    static AutoPtr<IArgbEvaluator> sInstance;
};

}   //namespace Animation
}   //namespace Droid
}   //namespace Elastos

#endif  //__ELASTOS_DROID_ANIMATION_CARGBEVALUATOR_H__
