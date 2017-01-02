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

#include "elastos/droid/systemui/statusbar/ExpandableOutlineView.h"
#include <elastos/core/Math.h>

using Elastos::Droid::Graphics::CRect;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

ExpandableOutlineView::ExpandableViewOutlineProvider::ExpandableViewOutlineProvider(
    /* [in] */ ExpandableOutlineView* host)
    : mHost(host)
{}

ECode ExpandableOutlineView::ExpandableViewOutlineProvider::GetOutline(
    /* [in] */ IView* view,
    /* [in] */ IOutline* outline)
{
    if (!mHost->mCustomOutline) {
        Int32 width = 0;
        mHost->GetWidth(&width);
        outline->SetRect(0,
                mHost->mClipTopAmount,
                width,
                Elastos::Core::Math::Max(mHost->mActualHeight, mHost->mClipTopAmount));
    }
    else {
        outline->SetRect(mHost->mOutlineRect);
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(ExpandableOutlineView, ExpandableView, IExpandableOutlineView)
ExpandableOutlineView::ExpandableOutlineView()
    : mCustomOutline(FALSE)
{
    CRect::New((IRect**)&mOutlineRect);
}

ECode ExpandableOutlineView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    ExpandableView::constructor(context, attrs);
    AutoPtr<ExpandableViewOutlineProvider> provider = new ExpandableViewOutlineProvider(this);
    SetOutlineProvider(provider);
    return NOERROR;
}

ECode ExpandableOutlineView::SetActualHeight(
    /* [in] */ Int32 actualHeight,
    /* [in] */ Boolean notifyListeners)
{
    ExpandableView::SetActualHeight(actualHeight, notifyListeners);
    InvalidateOutline();
    return NOERROR;
}

ECode ExpandableOutlineView::SetClipTopAmount(
    /* [in] */ Int32 clipTopAmount)
{
    ExpandableView::SetClipTopAmount(clipTopAmount);
    InvalidateOutline();
    return NOERROR;
}

void ExpandableOutlineView::SetOutlineRect(
    /* [in] */ IRectF* rect)
{
    if (rect != NULL) {
        Float left = 0, top = 0, right = 0, bottom = 0;
        rect->Get(&left, &top, &right, &bottom);
        SetOutlineRect(left, top, right, bottom);
    }
    else {
        mCustomOutline = FALSE;
        InvalidateOutline();
    }
}

void ExpandableOutlineView::SetOutlineRect(
    /* [in] */ Float left,
    /* [in] */ Float top,
    /* [in] */ Float right,
    /* [in] */ Float bottom)
{
    mCustomOutline = TRUE;

    mOutlineRect->Set((Int32) left, (Int32) top, (Int32) right, (Int32) bottom);

    // Outlines need to be at least 1 dp
    Int32 value = 0;
    mOutlineRect->GetBottom(&value);
    mOutlineRect->SetBottom((Int32) Elastos::Core::Math::Max((Int32)top, value));
    mOutlineRect->GetRight(&value);
    mOutlineRect->SetRight((Int32) Elastos::Core::Math::Max((Int32)left, value));

    InvalidateOutline();
}

} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
