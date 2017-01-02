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
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/animation/AlphaAnimation.h"
#include "elastos/droid/view/animation/CAlphaAnimation.h"
#include "elastos/droid/R.h"

using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::R;

namespace Elastos {
namespace Droid {
namespace View {
namespace Animation {

CAR_INTERFACE_IMPL(AlphaAnimation, Animation, IAlphaAnimation);

AlphaAnimation::AlphaAnimation()
    : mFromAlpha(0.f)
    , mToAlpha(0.f)
{}

ECode AlphaAnimation::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    FAIL_RETURN(Animation::constructor(context, attrs));

    AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::AlphaAnimation);
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, attrIds, (ITypedArray**)&a);

    a->GetFloat(R::styleable::AlphaAnimation_fromAlpha, 1.0f, &mFromAlpha);
    a->GetFloat(R::styleable::AlphaAnimation_toAlpha, 1.0f, &mToAlpha);
    a->Recycle();

    return NOERROR;
}

ECode AlphaAnimation::constructor(
    /* [in] */ Float fromAlpha,
    /* [in] */ Float toAlpha)
{
    FAIL_RETURN(Animation::constructor());
    mFromAlpha = fromAlpha;
    mToAlpha = toAlpha;
    return NOERROR;
}

void AlphaAnimation::ApplyTransformation(
    /* [in] */ Float interpolatedTime,
    /* [in] */ ITransformation* t)
{
    t->SetAlpha(mFromAlpha + ((mToAlpha - mFromAlpha) * interpolatedTime));
}

ECode AlphaAnimation::WillChangeTransformationMatrix(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

ECode AlphaAnimation::WillChangeBounds(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

ECode AlphaAnimation::HasAlpha(
    /* [out] */ Boolean* has)
{
    VALIDATE_NOT_NULL(has);
    *has = TRUE;
    return NOERROR;
}

ECode AlphaAnimation::Clone(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);

    AutoPtr<IAlphaAnimation> obj;
    CAlphaAnimation::New(0.0f, 0.0f, (IAlphaAnimation**)&obj);
    AlphaAnimation::CloneImpl(obj);
    *object = obj;
    REFCOUNT_ADD(*object);
    return NOERROR;
}

ECode AlphaAnimation::CloneImpl(
    /* [in] */ IAlphaAnimation* object)
{
    Animation::CloneImpl(IAnimation::Probe(object));
    AlphaAnimation* obj = (AlphaAnimation*)object;
    obj->mFromAlpha = mFromAlpha;
    obj->mToAlpha = mToAlpha;

    return NOERROR;
}

}// namespace Animation
}// namespace View
}// namespace Droid
}// namespace Elastos
