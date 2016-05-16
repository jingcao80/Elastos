
#include "elastos/droid/launcher2/PagedViewCellLayoutChildren.h"
#include "elastos/droid/launcher2/PagedViewCellLayout.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Service.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

const String PagedViewCellLayoutChildren::TAG("PagedViewCellLayoutChildren");

CAR_INTERFACE_IMPL(PagedViewCellLayoutChildren, ViewGroup, IPagedViewCellLayoutChildren);

PagedViewCellLayoutChildren::PagedViewCellLayoutChildren(
    /* [in] */ IContext* context)
    : mCenterContent(FALSE)
    , mCellWidth(0)
    , mCellHeight(0)
    , mWidthGap(0)
    , mHeightGap(0)
{
    ViewGroup::constructor(context);
}

ECode PagedViewCellLayoutChildren::CancelLongPress()
{
    ViewGroup::CancelLongPress();

    // Cancel long press for all children
    Int32 count;
    GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        child->CancelLongPress();
    }
    return NOERROR;
}

ECode PagedViewCellLayoutChildren::SetGap(
    /* [in] */ Int32 widthGap,
    /* [in] */ Int32 heightGap)
{
    mWidthGap = widthGap;
    mHeightGap = heightGap;
    return RequestLayout();
}

ECode PagedViewCellLayoutChildren::SetCellDimensions(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    mCellWidth = width;
    mCellHeight = height;
    return RequestLayout();
}

ECode PagedViewCellLayoutChildren::RequestChildFocus(
    /* [in] */ IView* child,
    /* [in] */ IView* focused)
{
    ViewGroup::RequestChildFocus(child, focused);
    if (child != NULL) {
        AutoPtr<IRect> r;
        CRect::New((IRect**)&r);
        child->GetDrawingRect(r);
        Boolean res;
        RequestRectangleOnScreen(r, &res);
    }
    return NOERROR;
}

void PagedViewCellLayoutChildren::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    Int32 widthSpecMode = View::MeasureSpec::GetMode(widthMeasureSpec);
    Int32 widthSpecSize = View::MeasureSpec::GetSize(widthMeasureSpec);

    Int32 heightSpecMode = View::MeasureSpec::GetMode(heightMeasureSpec);
    Int32 heightSpecSize = View::MeasureSpec::GetSize(heightMeasureSpec);

    if (widthSpecMode == View::MeasureSpec::UNSPECIFIED || heightSpecMode == View::MeasureSpec::UNSPECIFIED) {
        //throw new RuntimeException("CellLayout cannot have UNSPECIFIED dimensions");
        Slogger::E(TAG, "CellLayout cannot have UNSPECIFIED dimensions");
        return;
    }

    Int32 count;
    GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        AutoPtr<IViewGroupLayoutParams> params;
        child->GetLayoutParams((IViewGroupLayoutParams**)&params);
        AutoPtr<PagedViewCellLayout::PagedViewCellLayoutLayoutParams> lp =
            (PagedViewCellLayout::PagedViewCellLayoutLayoutParams*)IPagedViewCellLayoutLayoutParams::Probe(params);
        Int32 left, top;
        GetPaddingLeft(&left);
        GetPaddingTop(&top);
        lp->Setup(mCellWidth, mCellHeight, mWidthGap, mHeightGap, left, top);

        Int32 childWidthMeasureSpec = View::MeasureSpec::MakeMeasureSpec(lp->mWidth,
                View::MeasureSpec::EXACTLY);
        Int32 childheightMeasureSpec = View::MeasureSpec::MakeMeasureSpec(lp->mHeight,
                View::MeasureSpec::EXACTLY);

        child->Measure(childWidthMeasureSpec, childheightMeasureSpec);
    }

    SetMeasuredDimension(widthSpecSize, heightSpecSize);
}

ECode PagedViewCellLayoutChildren::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 r,
    /* [in] */ Int32 b)
{
    Int32 count;
    GetChildCount(&count);

    Int32 offsetX = 0;
    if (mCenterContent && count > 0) {
        // determine the max width of all the rows and center accordingly
        Int32 maxRowX = 0;
        Int32 minRowX = Elastos::Core::Math::INT32_MAX_VALUE;
        for (Int32 i = 0; i < count; i++) {
            AutoPtr<IView> child;
            GetChildAt(i, (IView**)&child);
            Int32 visibility;
            child->GetVisibility(&visibility);
            if (visibility != GONE) {
                AutoPtr<IViewGroupLayoutParams> params;
                child->GetLayoutParams((IViewGroupLayoutParams**)&params);
                AutoPtr<PagedViewCellLayout::PagedViewCellLayoutLayoutParams> lp =
                        (PagedViewCellLayout::PagedViewCellLayoutLayoutParams*)IObject::Probe(params);
                minRowX = Elastos::Core::Math::Min(minRowX, lp->mX);
                maxRowX = Elastos::Core::Math::Max(maxRowX, lp->mX + lp->mWidth);
            }
        }
        Int32 maxRowWidth = maxRowX - minRowX;
        Int32 width;
        GetMeasuredWidth(&width);
        offsetX = (width - maxRowWidth) / 2;
    }

    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        Int32 visibility;
        child->GetVisibility(&visibility);
        if (visibility != GONE) {
            AutoPtr<IViewGroupLayoutParams> params;
            child->GetLayoutParams((IViewGroupLayoutParams**)&params);
            AutoPtr<PagedViewCellLayout::PagedViewCellLayoutLayoutParams> lp =
                    (PagedViewCellLayout::PagedViewCellLayoutLayoutParams*)IObject::Probe(params);

            Int32 childLeft = offsetX + lp->mX;
            Int32 childTop = lp->mY;
            child->Layout(childLeft, childTop, childLeft + lp->mWidth, childTop + lp->mHeight);
        }
    }
    return NOERROR;
}

ECode PagedViewCellLayoutChildren::EnableCenteredContent(
    /* [in] */ Boolean enabled)
{
    mCenterContent = enabled;
    return NOERROR;
}

void PagedViewCellLayoutChildren::SetChildrenDrawingCacheEnabled(
    /* [in] */ Boolean enabled)
{
    Int32 count;
    GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> view;
        GetChildAt(i, (IView**)&view);
        view->SetDrawingCacheEnabled(enabled);
        // Update the drawing caches
        Boolean res;
        view->IsHardwareAccelerated(&res);
        if (!res) {
            view->BuildDrawingCache(TRUE);
        }
    }
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos