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

#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/animation/TranslateAnimation.h"
#include "elastos/droid/view/animation/CTranslateAnimation.h"
#include "elastos/droid/R.h"

using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::R;

namespace Elastos {
namespace Droid {
namespace View {
namespace Animation {

CAR_INTERFACE_IMPL(TranslateAnimation, Animation, ITranslateAnimation);

TranslateAnimation::TranslateAnimation()
    : mFromXType(IAnimation::ABSOLUTE)
    , mToXType(IAnimation::ABSOLUTE)
    , mFromYType(IAnimation::ABSOLUTE)
    , mToYType(IAnimation::ABSOLUTE)
    , mFromXValue(0.0f)
    , mToXValue(0.0f)
    , mFromYValue(0.0f)
    , mToYValue(0.0f)
{}

ECode TranslateAnimation::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    FAIL_RETURN(Animation::constructor(context, attrs));

    AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::TranslateAnimation);
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, attrIds, (ITypedArray**)&a);

    AutoPtr<ITypedValue> value;
    a->PeekValue(R::styleable::TranslateAnimation_fromXDelta, (ITypedValue**)&value);
    AutoPtr<Animation::Description> d = Animation::Description::ParseValue(value);
    mFromXType = d->mType;
    mFromXValue = d->mValue;

    value = NULL;
    a->PeekValue(R::styleable::TranslateAnimation_toXDelta, (ITypedValue**)&value);
    d = Description::ParseValue(value);
    mToXType = d->mType;
    mToXValue = d->mValue;

    value = NULL;
    a->PeekValue(R::styleable::TranslateAnimation_fromYDelta, (ITypedValue**)&value);
    d = Description::ParseValue(value);
    mFromYType = d->mType;
    mFromYValue = d->mValue;

    value = NULL;
    a->PeekValue(R::styleable::TranslateAnimation_toYDelta, (ITypedValue**)&value);
    d = Description::ParseValue(value);
    mToYType = d->mType;
    mToYValue = d->mValue;

    a->Recycle();

    return NOERROR;
}

ECode TranslateAnimation::constructor(
    /* [in] */ Float fromXDelta,
    /* [in] */ Float toXDelta,
    /* [in] */ Float fromYDelta,
    /* [in] */ Float toYDelta)
{
    FAIL_RETURN(Animation::constructor());
    mFromXValue = fromXDelta;
    mToXValue = toXDelta;
    mFromYValue = fromYDelta;
    mToYValue = toYDelta;

    mFromXType = IAnimation::ABSOLUTE;
    mToXType = IAnimation::ABSOLUTE;
    mFromYType = IAnimation::ABSOLUTE;
    mToYType = IAnimation::ABSOLUTE;

    return NOERROR;
}

ECode TranslateAnimation::constructor(
   /* [in] */ Float fromXType,
   /* [in] */ Float fromXValue,
   /* [in] */ Float toXType,
   /* [in] */ Float toXValue,
   /* [in] */ Float fromYType,
   /* [in] */ Float fromYValue,
   /* [in] */ Float toYType,
   /* [in] */ Float toYValue)
{
    FAIL_RETURN(Animation::constructor());
    mFromXValue = fromXValue;
    mToXValue = toXValue;
    mFromYValue = fromYValue;
    mToYValue = toYValue;

    mFromXType = fromXType;
    mToXType = toXType;
    mFromYType = fromYType;
    mToYType = toYType;

    return NOERROR;
}

void TranslateAnimation::ApplyTransformation(
    /* [in] */ Float interpolatedTime,
    /* [in] */ ITransformation* t)
{
    Float dx = mFromXDelta;
    Float dy = mFromYDelta;
    if (mFromXDelta != mToXDelta) {
        dx = mFromXDelta + ((mToXDelta - mFromXDelta) * interpolatedTime);
    }
    if (mFromYDelta != mToYDelta) {
        dy = mFromYDelta + ((mToYDelta - mFromYDelta) * interpolatedTime);
    }

    AutoPtr<IMatrix> matrix;
    t->GetMatrix((IMatrix**)&matrix);
    matrix->SetTranslate(dx, dy);
}

ECode TranslateAnimation::Initialize(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 parentWidth,
    /* [in] */ Int32 parentHeight)
{
    Animation::Initialize(width, height, parentWidth, parentHeight);
    mFromXDelta = ResolveSize(mFromXType, mFromXValue, width, parentWidth);
    mToXDelta = ResolveSize(mToXType, mToXValue, width, parentWidth);
    mFromYDelta = ResolveSize(mFromYType, mFromYValue, height, parentHeight);
    mToYDelta = ResolveSize(mToYType, mToYValue, height, parentHeight);

    return NOERROR;
}

ECode TranslateAnimation::Clone(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);

    AutoPtr<ITranslateAnimation> obj;
    CTranslateAnimation::New(0.0f, 0.0f, 0.0f, 0.0f, (ITranslateAnimation**)&obj);
    TranslateAnimation::CloneImpl(obj);
    *object = obj;
    REFCOUNT_ADD(*object);
    return NOERROR;
}

ECode TranslateAnimation::CloneImpl(
    /* [in] */ ITranslateAnimation* object)
{
    Animation::CloneImpl(IAnimation::Probe(object));
    TranslateAnimation* obj = (TranslateAnimation*)object;

    obj->mFromXType = mFromXType;
    obj->mToXType = mToXType;
    obj->mFromYType = mFromYType;
    obj->mToYType = mToYType;
    obj->mFromXValue = mFromXValue;
    obj->mToXValue = mToXValue;
    obj->mFromYValue = mFromYValue;
    obj->mToYValue = mToYValue;
    obj->mFromXDelta = mFromXDelta;
    obj->mToXDelta = mToXDelta;
    obj->mFromYDelta = mFromYDelta;
    obj->mToYDelta = mToYDelta;

    return NOERROR;
}

}   //namespace Animation
}   //namespace View
}   //namespace Droid
}   //namespace Elastos
