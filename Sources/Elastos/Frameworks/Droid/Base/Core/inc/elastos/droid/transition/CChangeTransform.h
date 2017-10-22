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

#ifndef __ELASTOS_DROID_TRANSITION_CCHANGETRANSFORM_H__
#define __ELASTOS_DROID_TRANSITION_CCHANGETRANSFORM_H__

#include "Elastos.Droid.Graphics.h"
#include "_Elastos_Droid_Transition_CChangeTransform.h"
#include "elastos/droid/animation/AnimatorListenerAdapter.h"
#include "elastos/droid/transition/Transition.h"

#include <elastos/core/Object.h>

using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::Graphics::IMatrix;
using Elastos::Droid::Utility::IProperty;
using Elastos::Droid::View::IGhostView;

namespace Elastos {
namespace Droid {
namespace Transition {

CarClass(CChangeTransform)
    , public Transition
    , public IChangeTransform
{
private:
    class Transforms
        : public Object
    {
    public:
        Transforms(
            /* [in] */ IView* view);

        CARAPI_(void) Restore(
            /* [in] */ IView* view);

        using Object::Equals;

        CARAPI_(Boolean) Equals(
            /* [in] */ IInterface* that);

    public:
        Float mTranslationX;
        Float mTranslationY;
        Float mTranslationZ;
        Float mScaleX;
        Float mScaleY;
        Float mRotationX;
        Float mRotationY;
        Float mRotationZ;
    };

    class GhostListener
        : public TransitionListenerAdapter
    {
    public:
        GhostListener(
            /* [in] */ IView* view,
            /* [in] */ IGhostView* ghostView,
            /* [in] */ IMatrix* endMatrix);

        CARAPI OnTransitionEnd(
            /* [in] */ ITransition* transition);

        CARAPI OnTransitionPause(
            /* [in] */ ITransition* transition);

        CARAPI OnTransitionResume(
            /* [in] */ ITransition* transition);

    private:
        AutoPtr<IView> mView;
        AutoPtr<IGhostView> mGhostView;
        AutoPtr<IMatrix> mEndMatrix;
    };

    class AnimatorListenerAdapter_1
        : public AnimatorListenerAdapter
    {
    public:
        AnimatorListenerAdapter_1(
            /* [in] */ Boolean handleParentChange,
            /* [in] */ Boolean useOverlay,
            /* [in] */ IMatrix* finalEndMatrix,
            /* [in] */ IView* v,
            /* [in] */ Transforms* trans,
            /* [in] */ CChangeTransform* owner);

        CARAPI OnAnimationCancel(
            /* [in] */ IAnimator* animation);

        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

        CARAPI OnAnimationPause(
            /* [in] */ IAnimator* animation);

        CARAPI OnAnimationResume(
            /* [in] */ IAnimator* animation);

    private:
        CARAPI_(void) SetCurrentMatrix(
            /* [in] */ IMatrix* currentMatrix);

    public:
        Boolean mIsCanceled;
        AutoPtr<IMatrix> mTempMatrix;
        Boolean mHandleParentChange;
        Boolean mUseOverlay;
        IMatrix* mFinalEndMatrix;
        IView* mView;
        Transforms* mTrans;
        CChangeTransform* mOwner;
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

    CAR_INTERFACE_DECL()

    CChangeTransform();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI GetReparentWithOverlay(
        /* [out] */ Boolean* result);

    CARAPI SetReparentWithOverlay(
        /* [in] */ Boolean reparentWithOverlay);

    CARAPI GetReparent(
        /* [out] */ Boolean* result);

    CARAPI SetReparent(
        /* [in] */ Boolean reparent);

    CARAPI GetTransitionProperties(
        /* [out] */ ArrayOf<String>** result);

    CARAPI CaptureStartValues(
        /* [in] */ ITransitionValues* transitionValues);

    CARAPI CaptureEndValues(
        /* [in] */ ITransitionValues* transitionValues);

    CARAPI CreateAnimator(
        /* [in] */ IViewGroup* sceneRoot,
        /* [in] */ ITransitionValues* startValues,
        /* [in] */ ITransitionValues* endValues,
        /* [out] */ IAnimator** result);

    CARAPI Clone(
        /* [out] */ IInterface** obj);

    CARAPI CloneImpl(
        /* [in] */ IChangeTransform* obj);

private:
    using Transition::CaptureValues;

    CARAPI_(void) CaptureValues(
        /* [in] */ ITransitionValues* transitionValues);

    CARAPI_(AutoPtr<IObjectAnimator>) CreateTransformAnimator(
        /* [in] */ ITransitionValues* startValues,
        /* [in] */ ITransitionValues* endValues,
        /* [in] */ Boolean handleParentChange);

    CARAPI_(Boolean) ParentsMatch(
        /* [in] */ IViewGroup* startParent,
        /* [in] */ IViewGroup* endParent);

    CARAPI_(void) CreateGhostView(
        /* [in] */ IViewGroup* sceneRoot,
        /* [in] */ ITransitionValues* startValues,
        /* [in] */ ITransitionValues* endValues);

    CARAPI_(void) SetMatricesForParent(
        /* [in] */ ITransitionValues* startValues,
        /* [in] */ ITransitionValues* endValues);

    static CARAPI_(void) SetIdentityTransforms(
        /* [in] */ IView* view);

    static CARAPI_(void) SetTransforms(
        /* [in] */ IView* view,
        /* [in] */ Float translationX,
        /* [in] */ Float translationY,
        /* [in] */ Float translationZ,
        /* [in] */ Float scaleX,
        /* [in] */ Float scaleY,
        /* [in] */ Float rotationX,
        /* [in] */ Float rotationY,
        /* [in] */ Float rotationZ);

private:
    static const String TAG;

    static const String PROPNAME_MATRIX;
    static const String PROPNAME_TRANSFORMS;
    static const String PROPNAME_PARENT;
    static const String PROPNAME_PARENT_MATRIX;
    static const String PROPNAME_INTERMEDIATE_PARENT_MATRIX;
    static const String PROPNAME_INTERMEDIATE_MATRIX;

    static AutoPtr<ArrayOf<String> > sTransitionProperties;

    static AutoPtr<IProperty> ANIMATION_MATRIX_PROPERTY;

    Boolean mUseOverlay;
    Boolean mReparent;
    AutoPtr<IMatrix> mTempMatrix;
};

} // namespace Transition
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TRANSITION_CCHANGETRANSFORM_H__
