
#include "elastos/droid/packages/systemui/statusbar/ExpandableOutlineView.h"
#include <elastos/core/Math.h>

using Elastos::Droid::Graphics::CRect;

namespace Elastos {
namespace Droid {
namespace Packages {
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

CAR_INTERFACE_IMPL(ExpandableOutlineView, ExpandableView, IExpandableOutlineView);
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
} // namespace Packages
} // namespace Droid
} // namespace Elastos
