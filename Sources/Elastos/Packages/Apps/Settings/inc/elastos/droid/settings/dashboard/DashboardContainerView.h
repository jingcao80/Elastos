

package com.android.settings.dashboard;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Settings::IR;

public class DashboardContainerView extends ViewGroup {

    private Float mCellGapX;
    private Float mCellGapY;

    private Int32 mNumRows;
    private Int32 mNumColumns;

    public DashboardContainerView(Context context, AttributeSet attrs) {
        Super(context, attrs);

        final Resources res = context->GetResources();
        mCellGapX = res->GetDimension(R.dimen.dashboard_cell_gap_x);
        mCellGapY = res->GetDimension(R.dimen.dashboard_cell_gap_y);
        mNumColumns = res->GetInteger(R.integer.dashboard_num_columns);
    }

    //@Override
    protected void OnMeasure(Int32 widthMeasureSpec, Int32 heightMeasureSpec) {
        final Int32 width = MeasureSpec->GetSize(widthMeasureSpec);
        final Int32 availableWidth = (Int32) (width - GetPaddingLeft() - GetPaddingRight() -
                (mNumColumns - 1) * mCellGapX);
        Float cellWidth = (Float) Math->Ceil(((Float) availableWidth) / mNumColumns);
        final Int32 N = GetChildCount();

        Int32 cellHeight = 0;
        Int32 cursor = 0;

        for (Int32 i = 0; i < N; ++i) {
            DashboardTileView v = (DashboardTileView) GetChildAt(i);
            if (v->GetVisibility() == View.GONE) {
                continue;
            }

            ViewGroup.LayoutParams lp = v->GetLayoutParams();
            Int32 colSpan = v->GetColumnSpan();
            lp.width = (Int32) ((colSpan * cellWidth) + (colSpan - 1) * mCellGapX);

            // Measure the child
            Int32 newWidthSpec = GetChildMeasureSpec(widthMeasureSpec, 0, lp.width);
            Int32 newHeightSpec = GetChildMeasureSpec(heightMeasureSpec, 0, lp.height);
            v->Measure(newWidthSpec, newHeightSpec);

            // Save the cell height
            if (cellHeight <= 0) {
                cellHeight = v->GetMeasuredHeight();
            }

            lp.height = cellHeight;

            cursor += colSpan;
        }

        mNumRows = (Int32) Math->Ceil((Float) cursor / mNumColumns);
        final Int32 newHeight = (Int32) ((mNumRows * cellHeight) + ((mNumRows - 1) * mCellGapY)) +
                GetPaddingTop() + GetPaddingBottom();

        SetMeasuredDimension(width, newHeight);
    }

    //@Override
    protected void OnLayout(Boolean changed, Int32 l, Int32 t, Int32 r, Int32 b) {
        final Int32 N = GetChildCount();
        final Boolean isLayoutRtl = IsLayoutRtl();
        final Int32 width = GetWidth();

        Int32 x = GetPaddingStart();
        Int32 y = GetPaddingTop();
        Int32 cursor = 0;

        for (Int32 i = 0; i < N; ++i) {
            final DashboardTileView child = (DashboardTileView) GetChildAt(i);
            final ViewGroup.LayoutParams lp = child->GetLayoutParams();
            if (child->GetVisibility() == GONE) {
                continue;
            }

            final Int32 col = cursor % mNumColumns;
            final Int32 colSpan = child->GetColumnSpan();

            final Int32 childWidth = lp.width;
            final Int32 childHeight = lp.height;

            Int32 row = cursor / mNumColumns;

            if (row == mNumRows - 1) {
                child->SetDividerVisibility(FALSE);
            }

            // Push the item to the next row if it can't fit on this one
            if ((col + colSpan) > mNumColumns) {
                x = GetPaddingStart();
                y += childHeight + mCellGapY;
                row++;
            }

            final Int32 childLeft = (isLayoutRtl) ? width - x - childWidth : x;
            final Int32 childRight = childLeft + childWidth;

            final Int32 childTop = y;
            final Int32 childBottom = childTop + childHeight;

            // Layout the container
            child->Layout(childLeft, childTop, childRight, childBottom);

            // Offset the position by the cell gap or reset the position and cursor when we
            // reach the end of the row
            cursor += child->GetColumnSpan();
            if (cursor < (((row + 1) * mNumColumns))) {
                x += childWidth + mCellGapX;
            } else {
                x = GetPaddingStart();
                y += childHeight + mCellGapY;
            }
        }
    }
}
