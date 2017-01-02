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

#include "elastos/droid/launcher2/CHolographicLinearLayout.h"
#include "Elastos.Droid.Service.h"
#include "R.h"

using Elastos::Droid::Graphics::Drawable::IStateListDrawable;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_INTERFACE_IMPL(CHolographicLinearLayout, LinearLayout, IHolographicLinearLayout);

CAR_OBJECT_IMPL(CHolographicLinearLayout)

CHolographicLinearLayout::CHolographicLinearLayout()
    : mImageViewId(0)
{
}

ECode CHolographicLinearLayout::constructor()
{
    return NOERROR;
}

ECode CHolographicLinearLayout::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode CHolographicLinearLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode CHolographicLinearLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    LinearLayout::constructor(context, attrs, defStyle);

    AutoPtr<ArrayOf<Int32> > attrIds =
        TO_ATTRS_ARRAYOF(Elastos::Droid::Launcher2::R::styleable::HolographicLinearLayout);
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, attrIds, defStyle, 0, (ITypedArray**)&a);
    a->GetResourceId(
            Elastos::Droid::Launcher2::R::styleable::HolographicLinearLayout_sourceImageViewId,
            -1, &mImageViewId);
    a->Recycle();

    SetWillNotDraw(FALSE);
    mHolographicHelper = new HolographicViewHelper(context);
    return NOERROR;
}

ECode CHolographicLinearLayout::DrawableStateChanged()
{
    LinearLayout::DrawableStateChanged();

    if (mImageView != NULL) {
        AutoPtr<IDrawable> d;
        mImageView->GetDrawable((IDrawable**)&d);
        if (IStateListDrawable::Probe(d) != NULL) {
            AutoPtr<IStateListDrawable> sld = IStateListDrawable::Probe(d);
            AutoPtr<ArrayOf<Int32> > stateSet;
            GetDrawableState((ArrayOf<Int32>**)&stateSet);
            Boolean isStateful;
            return IDrawable::Probe(sld)->SetState(stateSet, &isStateful);
        }
    }
    return NOERROR;
}

ECode CHolographicLinearLayout::InvalidatePressedFocusedStates()
{
    mHolographicHelper->InvalidatePressedFocusedStates(mImageView);
    return Invalidate();
}

void CHolographicLinearLayout::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    LinearLayout::OnDraw(canvas);

    // One time call to generate the pressed/focused state -- must be called after
    // measure/layout
    if (mImageView == NULL) {
        AutoPtr<IView> view;
        FindViewById(mImageViewId, (IView**)&view);
        mImageView = IImageView::Probe(view);
    }
    mHolographicHelper->GeneratePressedFocusedStates(mImageView);
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos