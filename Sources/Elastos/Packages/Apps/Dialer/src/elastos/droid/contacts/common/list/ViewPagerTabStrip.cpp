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

#include "elastos/droid/contacts/common/list/ViewPagerTabStrip.h"
#include "R.h"

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::CPaint;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace List {

ViewPagerTabStrip::ViewPagerTabStrip()
    : mSelectedUnderlineThickness(0)
    , mIndexForSelection(0)
    , mSelectionOffset(0.0)
{}

ECode ViewPagerTabStrip::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode ViewPagerTabStrip::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    FAIL_RETURN(LinearLayout::constructor(context, attrs))

    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);

    res->GetDimensionPixelSize(Elastos::Droid::Dialer::R::dimen::tab_selected_underline_height, &mSelectedUnderlineThickness);
    Int32 underlineColor;
    res->GetColor(Elastos::Droid::Dialer::R::color::tab_selected_underline_color, &underlineColor);
    Int32 backgroundColor;
    res->GetColor(Elastos::Droid::Dialer::R::color::actionbar_background_color, &backgroundColor);

    CPaint::New((IPaint**)&mSelectedUnderlinePaint);
    mSelectedUnderlinePaint->SetColor(underlineColor);

    SetBackgroundColor(backgroundColor);
    SetWillNotDraw(FALSE);
    return NOERROR;
}

void ViewPagerTabStrip::OnPageScrolled(
    /* [in] */ Int32 position,
    /* [in] */ Float positionOffset,
    /* [in] */ Int32 positionOffsetPixels)
{
    mIndexForSelection = position;
    mSelectionOffset = positionOffset;
    Invalidate();
}

void ViewPagerTabStrip::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    Int32 childCount;
    GetChildCount(&childCount);

    // Thick colored underline below the current selection
    if (childCount > 0) {
        AutoPtr<IView> selectedTitle;
        GetChildAt(mIndexForSelection, (IView**)&selectedTitle);
        Int32 selectedLeft;
        selectedTitle->GetLeft(&selectedLeft);
        Int32 selectedRight;
        selectedTitle->GetRight(&selectedRight);
        Boolean isRtl = IsRtl();
        GetChildCount(&childCount);
        Boolean hasNextTab = isRtl ? mIndexForSelection > 0 : (mIndexForSelection < (childCount - 1));
        if ((mSelectionOffset > 0.0f) && hasNextTab) {
            // Draw the selection partway between the tabs
            AutoPtr<IView> nextTitle;
            GetChildAt(mIndexForSelection + (isRtl ? -1 : 1), (IView**)&nextTitle);
            Int32 nextLeft;
            nextTitle->GetLeft(&nextLeft);
            Int32 nextRight;
            nextTitle->GetRight(&nextRight);

            selectedLeft = (Int32)(mSelectionOffset * nextLeft +
                    (1.0f - mSelectionOffset) * selectedLeft);
            selectedRight = (Int32)(mSelectionOffset * nextRight +
                    (1.0f - mSelectionOffset) * selectedRight);
        }

        Int32 height;
        GetHeight(&height);
        canvas->DrawRect(selectedLeft, height - mSelectedUnderlineThickness,
                selectedRight, height, mSelectedUnderlinePaint);
    }
}

Boolean ViewPagerTabStrip::IsRtl()
{
    Int32 direction;
    GetLayoutDirection(&direction);
    return direction == IView::LAYOUT_DIRECTION_RTL;
}

} // List
} // Common
} // Contacts
} // Droid
} // Elastos
