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

#ifndef __ELASTOS_DROID_TRANSITION_CTRANSITIONUTILS_H__
#define __ELASTOS_DROID_TRANSITION_CTRANSITIONUTILS_H__

#include "Elastos.Droid.Animation.h"
#include "_Elastos_Droid_Transition_CTransitionUtils.h"
#include <elastos/core/Object.h>
#include <elastos/core/Singleton.h>

using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::ITypeEvaluator;
using Elastos::Droid::Graphics::IMatrix;
using Elastos::Droid::Graphics::IRectF;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;

using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Transition {

/**
 * Static utility methods for Transitions.
 *
 * @hide
 */
CarClass(CTransitionUtils)
    , public Singleton
    , public ITransitionUtils
{
public:
    class MatrixEvaluator
        : public Object
        , public ITypeEvaluator
    {
    public:
        CAR_INTERFACE_DECL()

        MatrixEvaluator();

        CARAPI Evaluate(
            /* [in] */ Float fraction,
            /* [in] */ IInterface* startValue,
            /* [in] */ IInterface* endValue,
            /* [out] */ IInterface** result);

    public:
        AutoPtr<ArrayOf<Float> > mTempStartValues;

        AutoPtr<ArrayOf<Float> > mTempEndValues;

        AutoPtr<IMatrix> mTempMatrix;
    };

public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI MergeAnimators(
        /* [in] */ IAnimator* animator1,
        /* [in] */ IAnimator* animator2,
        /* [out] */ IAnimator** result);

    CARAPI MergeTransitions(
        /* [in] */ ArrayOf<ITransition*>* transitions,
        /* [out] */ ITransition** result);

    CARAPI CopyViewImage(
        /* [in] */ IViewGroup* sceneRoot,
        /* [in] */ IView* view,
        /* [in] */ IView* parent,
        /* [out] */ IView** result);

    CARAPI CreateViewBitmap(
        /* [in] */ IView* view,
        /* [in] */ IMatrix* matrix,
        /* [in] */ IRectF* bounds,
        /* [out] */ IBitmap** result);

private:
    static Int32 MAX_IMAGE_SIZE;
};

} // namespace Transition
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TRANSITION_CTRANSITIONUTILS_H__
