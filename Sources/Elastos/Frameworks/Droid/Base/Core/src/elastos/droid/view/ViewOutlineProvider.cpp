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

#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/view/ViewOutlineProvider.h"

using Elastos::Droid::Graphics::Drawable::IDrawable;

namespace Elastos {
namespace Droid {
namespace View {

Boolean ViewOutlineProvider::sInit = InitStatic();
AutoPtr<IViewOutlineProvider> ViewOutlineProvider::BACKGROUND;
AutoPtr<IViewOutlineProvider> ViewOutlineProvider::BOUNDS;
AutoPtr<IViewOutlineProvider> ViewOutlineProvider::PADDED_BOUNDS;

CAR_INTERFACE_IMPL(ViewOutlineProvider, Object, IViewOutlineProvider);
Boolean ViewOutlineProvider::InitStatic()
{
    BACKGROUND = new BackgroundViewOutlineProvider();
    BOUNDS = new BoundsViewOutlineProvider();
    PADDED_BOUNDS = new PaddedViewOutlineProvider();
    return TRUE;
}

ECode BackgroundViewOutlineProvider::GetOutline(
    /* [in] */ IView* view,
    /* [in] */ IOutline* outline)
{
    AutoPtr<IDrawable> background;
    view->GetBackground((IDrawable**)&background);
    if (background != NULL) {
        background->GetOutline(outline);
    } else {
        Int32 width = 0, height = 0;
        view->GetWidth(&width);
        view->GetHeight(&height);
        outline->SetRect(0, 0, width, height);
        outline->SetAlpha(0.0f);
    }
    return NOERROR;
}

ECode BoundsViewOutlineProvider::GetOutline(
    /* [in] */ IView* view,
    /* [in] */ IOutline* outline)
{
    Int32 width = 0, height = 0;
    view->GetWidth(&width);
    view->GetHeight(&height);
    return outline->SetRect(0, 0, width, height);
}

ECode PaddedViewOutlineProvider::GetOutline(
    /* [in] */ IView* view,
    /* [in] */ IOutline* outline)
{
    Int32 width = 0, height = 0, left = 0, top = 0, right = 0, bottom = 0;
    view->GetWidth(&width);
    view->GetHeight(&height);
    view->GetPaddingLeft(&left);
    view->GetPaddingTop(&top);
    view->GetPaddingRight(&right);
    view->GetPaddingBottom(&bottom);
    return outline->SetRect(left, top, width - right, height - bottom);
}

} // namespace View
} // namepsace Droid
} // namespace Elastos
