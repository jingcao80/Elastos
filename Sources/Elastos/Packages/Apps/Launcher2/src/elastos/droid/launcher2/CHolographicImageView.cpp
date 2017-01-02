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

#include "elastos/droid/launcher2/CHolographicImageView.h"
#include "Elastos.Droid.Service.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_INTERFACE_IMPL(CHolographicImageView, ImageView, IHolographicImageView);

CAR_OBJECT_IMPL(CHolographicImageView);

ECode CHolographicImageView::constructor()
{
    return NOERROR;
}

ECode CHolographicImageView::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode CHolographicImageView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode CHolographicImageView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    ImageView::constructor(context, attrs, defStyle);

    mHolographicHelper = new HolographicViewHelper(context);
    return NOERROR;
}

ECode CHolographicImageView::InvalidatePressedFocusedStates()
{
    mHolographicHelper->InvalidatePressedFocusedStates(this);
    return NOERROR;
}

void CHolographicImageView::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    ImageView::OnDraw(canvas);

    // One time call to generate the pressed/focused state -- must be called after
    // measure/layout
    mHolographicHelper->GeneratePressedFocusedStates(this);
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos