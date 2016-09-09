
#include "elastos/droid/settings/dashboard/CDashboardContainerView.h"
#include "elastos/droid/settings/dashboard/CDashboardTileView.h"
#include "Elastos.Droid.View.h"
#include "../R.h"
#include <elastos/core/Math.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::View::IView;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Dashboard {

CAR_OBJECT_IMPL(CDashboardContainerView)

CDashboardContainerView::CDashboardContainerView()
    : mCellGapX(0.f)
    , mCellGapY(0.f)
    , mNumRows(0)
    , mNumColumns(0)
{}

CDashboardContainerView::~CDashboardContainerView()
{}

ECode CDashboardContainerView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    ViewGroup::constructor(context, attrs);

    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    res->GetDimension(R::dimen::dashboard_cell_gap_x, &mCellGapX);
    res->GetDimension(R::dimen::dashboard_cell_gap_y, &mCellGapY);
    res->GetInteger(R::integer::dashboard_num_columns, &mNumColumns);

    return NOERROR;
}

ECode CDashboardContainerView::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    Int32 width = View::MeasureSpec::GetSize(widthMeasureSpec);
    Int32 pl, pr;
    GetPaddingLeft(&pl);
    GetPaddingRight(&pr);
    Int32 availableWidth = (Int32) (width - pl - pr - (mNumColumns - 1) * mCellGapX);
    using Elastos::Core::Math;
    Float cellWidth = (Float) Math::Ceil(((Float) availableWidth) / mNumColumns);
    Int32 N;
    GetChildCount(&N);

    Int32 cellHeight = 0;
    Int32 cursor = 0;

    AutoPtr<IView> view;
    for (Int32 i = 0; i < N; ++i) {
        view = NULL;
        GetChildAt(i, (IView**)&view);
        AutoPtr<CDashboardTileView> v = (CDashboardTileView*)view.Get();
        Int32 visibility;
        v->GetVisibility(&visibility);
        if (visibility == IView::GONE) {
            continue;
        }

        AutoPtr<IViewGroupLayoutParams> lp;
        v->GetLayoutParams((IViewGroupLayoutParams**)&lp);
        Int32 colSpan = v->GetColumnSpan();
        lp->SetWidth((Int32) ((colSpan * cellWidth) + (colSpan - 1) * mCellGapX));

        // Measure the child
        Int32 width, height;
        lp->GetWidth(&width);
        lp->GetHeight(&height);
        Int32 newWidthSpec = GetChildMeasureSpec(widthMeasureSpec, 0, width);
        Int32 newHeightSpec = GetChildMeasureSpec(heightMeasureSpec, 0, height);
        v->Measure(newWidthSpec, newHeightSpec);

        // Save the cell height
        if (cellHeight <= 0) {
            v->GetMeasuredHeight(&cellHeight);
        }

        lp->SetHeight(cellHeight);

        cursor += colSpan;
    }

    mNumRows = (Int32) Math::Ceil((Float) cursor / mNumColumns);
    Int32 top, bottom;
    GetPaddingTop(&top);
    GetPaddingBottom(&bottom);
    Int32 newHeight = (Int32) ((mNumRows * cellHeight) + ((mNumRows - 1) * mCellGapY)) + top + bottom;

    SetMeasuredDimension(width, newHeight);
    return NOERROR;
}

ECode CDashboardContainerView::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 r,
    /* [in] */ Int32 b)
{
    Int32 N;
    GetChildCount(&N);
    Boolean isLayoutRtl;
    IsLayoutRtl(&isLayoutRtl);
    Int32 width;
    GetWidth(&width);

    Int32 x;
    GetPaddingStart(&x);
    Int32 y;
    GetPaddingTop(&y);
    Int32 cursor = 0;

    AutoPtr<IView> view;
    for (Int32 i = 0; i < N; ++i) {
        view = NULL;
        GetChildAt(i, (IView**)&view);
        AutoPtr<CDashboardTileView> child = (CDashboardTileView*)view.Get();

        AutoPtr<IViewGroupLayoutParams> lp;
        child->GetLayoutParams((IViewGroupLayoutParams**)&lp);

        Int32 visibility;
        child->GetVisibility(&visibility);
        if (visibility == IView::GONE) {
            continue;
        }

        Int32 col = cursor % mNumColumns;
        Int32 colSpan = child->GetColumnSpan();

        Int32 childWidth;
        lp->GetWidth(&childWidth);
        Int32 childHeight;
        lp->GetHeight(&childHeight);

        Int32 row = cursor / mNumColumns;

        if (row == mNumRows - 1) {
            child->SetDividerVisibility(FALSE);
        }

        // Push the item to the next row if it can't fit on this one
        if ((col + colSpan) > mNumColumns) {
            GetPaddingStart(&x);
            y += childHeight + mCellGapY;
            row++;
        }

        Int32 childLeft = (isLayoutRtl) ? width - x - childWidth : x;
        Int32 childRight = childLeft + childWidth;

        Int32 childTop = y;
        Int32 childBottom = childTop + childHeight;

        // Layout the container
        child->Layout(childLeft, childTop, childRight, childBottom);

        // Offset the position by the cell gap or reset the position and cursor when we
        // reach the end of the row
        cursor += child->GetColumnSpan();
        if (cursor < (((row + 1) * mNumColumns))) {
            x += childWidth + mCellGapX;
        }
        else {
            GetPaddingStart(&x);
            y += childHeight + mCellGapY;
        }
    }
    return NOERROR;
}

} // namespace Dashboard
} // namespace Settings
} // namespace Droid
} // namespace Elastos