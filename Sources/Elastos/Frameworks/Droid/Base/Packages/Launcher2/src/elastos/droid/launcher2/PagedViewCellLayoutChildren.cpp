
#include "elastos/droid/launcher2/PagedViewCellLayoutChildren.h"
#include "Elastos.Droid.Service.h"
#include "R.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

const String PagedViewCellLayoutChildren::TAG("PagedViewCellLayout");

CAR_INTERFACE_IMPL(PagedViewCellLayoutChildren, ViewGroup, IPagedViewCellLayoutChildren);

PagedViewCellLayoutChildren::PagedViewCellLayoutChildren(
    /* [in] */ IContext* context)
    : ViewGroup(context)
    , mCenterContent(FALSE)
    , mCellWidth(0)
    , mCellHeight(0)
    , mWidthGap(0)
    , mHeightGap(0)
{
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
        RequestRectangleOnScreen(r);
    }
    return NOERROR;
}

ECode PagedViewCellLayoutChildren::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    Int32 widthSpecMode;
    MeasureSpec::GetMode(widthMeasureSpec, &widthSpecMode);
    Int32 widthSpecSize;
    MeasureSpec::GetSize(widthMeasureSpec, &widthSpecSize);

    Int32 heightSpecMode;
    MeasureSpec::GetMode(heightMeasureSpec, &heightSpecMode);
    Int32 heightSpecSize;
    MeasureSpec::GetSize(heightMeasureSpec, &heightSpecSize);

    if (widthSpecMode == MeasureSpec::UNSPECIFIED || heightSpecMode == MeasureSpec::UNSPECIFIED) {
        //throw new RuntimeException("CellLayout cannot have UNSPECIFIED dimensions");
        Slogger::E(TAG, "CellLayout cannot have UNSPECIFIED dimensions");
        return E_RUNTIME_EXCEPTION;

    }

    Int32 count;
    GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        AutoPtr<IViewGroupLayoutParams> params;
        child->GetLayoutParams((IViewGroupLayoutParams**)&params);
        AutoPtr<PagedViewCellLayout::LayoutParams> lp =
            (PagedViewCellLayout::LayoutParams*)IObject::Probe(params);
        Int32 left;
        getPaddingLeft(&left);
        Int32 top;
        GetPaddingTop(&top);
        lp->Setup(mCellWidth, mCellHeight, mWidthGap, mHeightGap,
                left, top);

        Int32 childWidthMeasureSpec;
        MeasureSpec::MakeMeasureSpec(lp->mWidth, MeasureSpec::EXACTLY, &childWidthMeasureSpec);
        Int32 childheightMeasureSpec;
        MeasureSpec::MakeMeasureSpec(lp->mHeight, MeasureSpec::EXACTLY, &childheightMeasureSpec);

        child->Measure(childWidthMeasureSpec, childheightMeasureSpec);
    }

    return SetMeasuredDimension(widthSpecSize, heightSpecSize);
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
        Int32 minRowX = Integer::MAX_VALUE;
        for (Int32 i = 0; i < count; i++) {
            AutoPtr<IView> child;
            GetChildAt(i, (IView**)&child);
            Int32 visibility;
            child->GetVisibility(&visibility);
            if (visibility != GONE) {
                AutoPtr<IViewGroupLayoutParams> params;
                child->GetLayoutParams((IViewGroupLayoutParams**)&params);
                AutoPtr<PagedViewCellLayout::LayoutParams> lp =
                        (PagedViewCellLayout::LayoutParams*)IObject::Probe(params);
                minRowX = Math::Min(minRowX, lp->mX);
                maxRowX = Math::Max(maxRowX, lp->mX + lp->mWidth);
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
            AutoPtr<PagedViewCellLayout::LayoutParams> lp =
                    (PagedViewCellLayout::LayoutParams*)IObject::Probe(params);

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

ECode PagedViewCellLayoutChildren::SetChildrenDrawingCacheEnabled(
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
        view->isHardwareAccelerated(&res);
        if (!res) {
            view->BuildDrawingCache(TRUE);
        }
    }
    return NOERROR;
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos