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

#include "elastos/droid/systemui/statusbar/CDismissViewImageButton.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

CAR_OBJECT_IMPL(CDismissViewImageButton)
CAR_INTERFACE_IMPL(CDismissViewImageButton, ImageButton, IDismissViewImageButton)
ECode CDismissViewImageButton::constructor(
    /* [in] */ IContext* context)
{
    return ImageButton::constructor(context);
}

ECode CDismissViewImageButton::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return ImageButton::constructor(context, attrs);
}

ECode CDismissViewImageButton::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return ImageButton::constructor(context, attrs, defStyleAttr);
}

ECode CDismissViewImageButton::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    return ImageButton::constructor(context, attrs, defStyleAttr, defStyleRes);
}

ECode CDismissViewImageButton::GetDrawingRect(
    /* [in] */ IRect* outRect)
{
    ImageButton::GetDrawingRect(outRect);
    Float translationX = 0;
    IView::Probe(mParent)->GetTranslationX(&translationX);
    Float translationY = 0;
    IView::Probe(mParent)->GetTranslationY(&translationY);
    Int32 left = 0, right = 0, top = 0, bottom = 0;
    outRect->Get(&left, &top, &right, &bottom);
    outRect->SetLeft(left + translationX);
    outRect->SetRight(right + translationX);
    outRect->SetTop(top + translationY);
    outRect->SetBottom(bottom + translationY);
    return NOERROR;
}

} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
