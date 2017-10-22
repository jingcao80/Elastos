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

#ifndef __ELASTOS_DROID_TRANSITION_CCHANGEIMAGETRANSFORM_H__
#define __ELASTOS_DROID_TRANSITION_CCHANGEIMAGETRANSFORM_H__

#include "Elastos.Droid.Utility.h"
#include "_Elastos_Droid_Transition_CChangeImageTransform.h"
#include "elastos/droid/transition/Transition.h"

using Elastos::Droid::Animation::ITypeEvaluator;
using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::Graphics::IMatrix;
using Elastos::Droid::Utility::IProperty;
using Elastos::Droid::Widget::IImageView;

namespace Elastos {
namespace Droid {
namespace Transition {

CarClass(CChangeImageTransform)
    , public Transition
{
private:
    class TypeEvaluator
        : public Object
        , public ITypeEvaluator
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI Evaluate(
            /* [in] */ Float fraction,
            /* [in] */ IInterface* startValue,
            /* [in] */ IInterface* endValue,
            /* [out] */ IInterface** result);
    };

    class MatrixProperty
        : public Object
        , public IProperty
    {
    public:
        CAR_INTERFACE_DECL()

        MatrixProperty(
            /* [in] */ const String& str);

        CARAPI IsReadOnly(
            /* [out] */ Boolean* readOnly);

        CARAPI Set(
            /* [in] */ IInterface* object,
            /* [in] */ IInterface* value);

        CARAPI Get(
            /* [in] */ IInterface* object,
            /* [out] */ IInterface** result);

        CARAPI GetName(
            /* [out] */ String* name);

        CARAPI GetType(
            /* [out] */ ClassID* id);

        CARAPI IsWriteOnly(
            /* [out] */ Boolean* writeOnly);

        CARAPI ForbiddenOperate(
            /* [out] */ Boolean* forbidden);

    private:
        String mName;
    };

public:
    CAR_OBJECT_DECL()

    CChangeImageTransform();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI CaptureStartValues(
        /* [in] */ ITransitionValues* transitionValues);

    CARAPI CaptureEndValues(
        /* [in] */ ITransitionValues* transitionValues);

    CARAPI GetTransitionProperties(
        /* [out] */ ArrayOf<String>** result);

    CARAPI CreateAnimator(
        /* [in] */ IViewGroup* sceneRoot,
        /* [in] */ ITransitionValues* startValues,
        /* [in] */ ITransitionValues* endValues,
        /* [out] */ IAnimator** result);

    CARAPI Clone(
        /* [out] */ IInterface** obj);

    CARAPI CloneImpl(
        /* [in] */ ITransition* obj);

private:
    using Transition::CaptureValues;

    CARAPI_(void) CaptureValues(
        /* [in] */ ITransitionValues* transitionValues);

    CARAPI_(AutoPtr<IObjectAnimator>) CreateNullAnimator(
        /* [in] */ IImageView* imageView);

    CARAPI_(AutoPtr<IObjectAnimator>) CreateMatrixAnimator(
        /* [in] */ IImageView* imageView,
        /* [in] */ IMatrix* startMatrix,
        /* [in] */ IMatrix* endMatrix);

private:
    static String TAG;

    static String PROPNAME_MATRIX;
    static String PROPNAME_BOUNDS;

    static AutoPtr<ArrayOf<String> > sTransitionProperties;

    static AutoPtr<ITypeEvaluator> NULL_MATRIX_EVALUATOR;

    static AutoPtr<IProperty> ANIMATED_TRANSFORM_PROPERTY;
};

} // namespace Transition
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TRANSITION_CCHANGEIMAGETRANSFORM_H__
