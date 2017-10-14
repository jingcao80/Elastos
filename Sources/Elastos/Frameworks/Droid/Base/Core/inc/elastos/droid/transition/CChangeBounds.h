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

#ifndef __ELASTOS_DROID_TRANSITION_CCHANGEBOUNDS_H__
#define __ELASTOS_DROID_TRANSITION_CCHANGEBOUNDS_H__

#include "_Elastos_Droid_Transition_CChangeBounds.h"
#include "elastos/droid/animation/AnimatorListenerAdapter.h"
#include "elastos/droid/utility/Int32Property.h"
#include "elastos/droid/transition/Transition.h"

using Elastos::Droid::Animation::ITypeEvaluator;
using Elastos::Droid::Animation::IAnimatorListener;
using Elastos::Droid::Utility::Int32Property;
using Elastos::Droid::Utility::IProperty;
using Elastos::Droid::Graphics::Drawable::IBitmapDrawable;

using Elastos::Core::IInteger32;

namespace Elastos {
namespace Droid {
namespace Transition {

CarClass(CChangeBounds)
    , public Transition
    , public IChangeBounds
{
private:
    class OffsetProperty
        : public Int32Property
    {
    public:
        OffsetProperty(
            /* [in] */ const String& name);

        CARAPI SetValue(
            /* [in] */ IInterface* view,
            /* [in] */ Int32 value);

        CARAPI Get(
            /* [in] */ IInterface* object,
            /* [out] */ IInterface** rst);

        virtual CARAPI_(void) OffsetBy(
            /* [in] */ IView* view,
            /* [in] */ Int32 by) = 0;

    public:
        Int32 mPreviousValue;
    };

    class HorizontalOffsetProperty
        : public OffsetProperty
    {
    public:
        HorizontalOffsetProperty();

        CARAPI_(void) OffsetBy(
            /* [in] */ IView* view,
            /* [in] */ Int32 by);
    };

    class VerticalOffsetProperty
        : public OffsetProperty
    {
    public:
        VerticalOffsetProperty();

        CARAPI_(void) OffsetBy(
            /* [in] */ IView* view,
            /* [in] */ Int32 by);
    };

    class PointFProperty
        : public Object
        , public IProperty
    {
    public:
        CAR_INTERFACE_DECL()

        PointFProperty(
            /* [in] */ const String& str);

        CARAPI IsReadOnly(
            /* [out] */ Boolean* readOnly);

        CARAPI Set(
            /* [in] */ IInterface* obj,
            /* [in] */ IInterface* value);

        CARAPI Get(
            /* [in] */ IInterface* obj,
            /* [out] */ IInterface** rst);

        CARAPI GetName(
            /* [out] */ String* name);

        CARAPI GetType(
            /* [out] */ ClassID* id);

        CARAPI IsWriteOnly(
            /* [out] */ Boolean* writeOnly);

        CARAPI ForbiddenOperate(
            /* [out] */ Boolean* forbidden);

    private:
        AutoPtr<IRect> mBounds;
        String mName;
    };

    class TransitionListenerAdapter_1
        : public TransitionListenerAdapter
    {
    public:
        TransitionListenerAdapter_1(
            /* [in] */ IViewGroup* parent);

        CARAPI OnTransitionCancel(
            /* [in] */ ITransition* transition);

        CARAPI OnTransitionEnd(
            /* [in] */ ITransition* transition);

        CARAPI OnTransitionPause(
            /* [in] */ ITransition* transition);

        CARAPI OnTransitionResume(
            /* [in] */ ITransition* transition);

    public:
        Boolean mCanceled;
        AutoPtr<IViewGroup> mParent;
    };

    class AnimatorListenerAdapter_1
        : public AnimatorListenerAdapter
    {
    public:
        AnimatorListenerAdapter_1(
            /* [in] */ IViewGroup* sceneRoot,
            /* [in] */ IBitmapDrawable* drawable,
            /* [in] */ IView* v,
            /* [in] */ Float alpha);

        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    public:
        AutoPtr<IViewGroup> mSR;
        AutoPtr<IBitmapDrawable> mDrawable;
        AutoPtr<IView> mV;
        Float mAlpha;
    };

    class AnimatorListenerAdapter_2
        : public AnimatorListenerAdapter
    {
    public:
        AnimatorListenerAdapter_2(
            /* [in] */ IView* v);

        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    public:
        AutoPtr<IView> mV;
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CChangeBounds();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI GetTransitionProperties(
        /* [out, callee] */ ArrayOf<String>** result);

    CARAPI SetResizeClip(
        /* [in] */ Boolean resizeClip);

    CARAPI SetReparent(
        /* [in] */ Boolean reparent);

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
        /* [in] */ IChangeBounds* obj);

private:
    using Transition::CaptureValues;

    CARAPI_(void) CaptureValues(
        /* [in] */ ITransitionValues* values);

    CARAPI_(Boolean) ParentMatches(
        /* [in] */ IView* startParent,
        /* [in] */ IView* endParent);

public:
    AutoPtr<ArrayOf<Int32> > mTempLocation;
    Boolean mResizeClip;
    Boolean mReparent;

private:
    static const String PROPNAME_BOUNDS;
    static const String PROPNAME_PARENT;
    static const String PROPNAME_WINDOW_X;
    static const String PROPNAME_WINDOW_Y;
    static AutoPtr<ArrayOf<String> > sTransitionProperties;

    static AutoPtr<IProperty> DRAWABLE_ORIGIN_PROPERTY;

//    static String LOG_TAG;

    static AutoPtr<ITypeEvaluator> sRectEvaluator;
};

} // namespace Transition
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TRANSITION_CCHANGEBOUNDS_H__
