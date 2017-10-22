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

#include "elastos/droid/widget/TableRow.h"
#include "elastos/droid/widget/CTableRowLayoutParams.h"
#include "elastos/droid/view/CGravity.h"
#include "elastos/droid/R.h"
#include <elastos/core/Math.h>

using Elastos::Utility::Etl::HashMap;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::CGravity;
using Elastos::Droid::View::EIID_IViewGroupOnHierarchyChangeListener;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Droid::R;
using Elastos::Core::CString;

namespace Elastos {
namespace Droid {
namespace Widget {

const Int32 TableRow::TableRowLayoutParams::LOCATION;
const Int32 TableRow::TableRowLayoutParams::LOCATION_NEXT;

//==============================================================================
//              TableRow::TableRowLayoutParams
//==============================================================================

CAR_INTERFACE_IMPL(TableRow::TableRowLayoutParams, LinearLayout::LinearLayoutLayoutParams, ITableRowLayoutParams)

TableRow::TableRowLayoutParams::TableRowLayoutParams()
    : mColumn(0)
    , mSpan(0)
{
    mOffset = ArrayOf<Int32>::Alloc(2);
}

ECode TableRow::TableRowLayoutParams::constructor(
    /* [in] */ IContext* c,
    /* [in] */ IAttributeSet* attrs)
{
    FAIL_RETURN(LinearLayout::LinearLayoutLayoutParams::constructor(c, attrs));

    AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::TableRow_Cell);
    AutoPtr<ITypedArray> a;
    c->ObtainStyledAttributes(attrs, attrIds, (ITypedArray**)&a);

    a->GetInt32(R::styleable::TableRow_Cell_layout_column, -1, &mColumn);
    a->GetInt32(R::styleable::TableRow_Cell_layout_span, 1, &mSpan);
    if (mSpan <= 1) {
        mSpan = 1;
    }

    a->Recycle();
    return NOERROR;
}

ECode TableRow::TableRowLayoutParams::constructor(
    /* [in] */ Int32 w,
    /* [in] */ Int32 h)
{
    FAIL_RETURN(LinearLayout::LinearLayoutLayoutParams::constructor(w, h));
    mColumn = -1;
    mSpan = 1;
    return NOERROR;
}

ECode TableRow::TableRowLayoutParams::constructor(
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ Float initWeight)
{
    FAIL_RETURN(LinearLayout::LinearLayoutLayoutParams::constructor(w, h, initWeight));
    mColumn = -1;
    mSpan = 1;
    return NOERROR;
}

ECode TableRow::TableRowLayoutParams::constructor()
{
    FAIL_RETURN(LinearLayout::LinearLayoutLayoutParams::constructor(
            IViewGroupLayoutParams::MATCH_PARENT,
            IViewGroupLayoutParams::WRAP_CONTENT));
    mColumn = -1;
    mSpan = 1;
    return NOERROR;
}

ECode TableRow::TableRowLayoutParams::constructor(
    /* [in] */ Int32 column)
{
    FAIL_RETURN(LinearLayout::LinearLayoutLayoutParams::constructor(
            IViewGroupLayoutParams::MATCH_PARENT,
            IViewGroupLayoutParams::WRAP_CONTENT));
    mColumn = column;
    mSpan = 1;
    return NOERROR;
}

ECode TableRow::TableRowLayoutParams::constructor(
    /* [in] */ IViewGroupLayoutParams* p)
{
    FAIL_RETURN(LinearLayout::LinearLayoutLayoutParams::constructor(p));
    mColumn = -1;
    mSpan = 1;
    return NOERROR;
}

ECode TableRow::TableRowLayoutParams::constructor(
    /* [in] */ IViewGroupMarginLayoutParams* source)
{
    FAIL_RETURN(LinearLayout::LinearLayoutLayoutParams::constructor(source));
    mColumn = -1;
    mSpan = 1;
    return NOERROR;
}

ECode TableRow::TableRowLayoutParams::SetColumn(
    /* [in] */ Int32 column)
{
    mColumn = column;
    return NOERROR;
}

ECode TableRow::TableRowLayoutParams::GetColumn(
    /* [out] */ Int32* column)
{
    VALIDATE_NOT_NULL(column)
    *column = mColumn;
    return NOERROR;
}

ECode TableRow::TableRowLayoutParams::SetSpan(
    /* [in] */ Int32 span)
{
    mSpan = span;
    return NOERROR;
}

ECode TableRow::TableRowLayoutParams::GetSpan(
    /* [out] */ Int32* span)
{
    VALIDATE_NOT_NULL(span)
    *span = mSpan;
    return NOERROR;
}

ECode TableRow::TableRowLayoutParams::SetBaseAttributes(
    /* [in] */ ITypedArray* a,
    /* [in] */ Int32 widthAttr,
    /* [in] */ Int32 heightAttr)
{
    // We don't want to force users to specify a layout_width
    Boolean res;
    a->HasValue(widthAttr, &res);

    if (res) {
        FAIL_RETURN(a->GetLayoutDimension(widthAttr, String("layout_width"), &mWidth));
    }
    else {
        mWidth = IViewGroupLayoutParams::MATCH_PARENT;
    }

    // We don't want to force users to specify a layout_height
    a->HasValue(heightAttr, &res);
    if (res) {
        FAIL_RETURN(a->GetLayoutDimension(heightAttr, String("layout_height"), &mHeight));
    }
    else {
        mHeight = IViewGroupLayoutParams::WRAP_CONTENT;
    }
    return NOERROR;
}

//==============================================================================
//              TableRow::ChildrenTracker
//==============================================================================

CAR_INTERFACE_IMPL(TableRow::ChildrenTracker, Object, IViewGroupOnHierarchyChangeListener)

TableRow::ChildrenTracker::ChildrenTracker(
    /* [in] */ TableRow* owner)
    : mOwner(owner)
{
}

ECode TableRow::ChildrenTracker::SetOnHierarchyChangeListener(
    /* [in] */ IViewGroupOnHierarchyChangeListener* listener)
{
    mListener = listener;
    return NOERROR;
}

ECode TableRow::ChildrenTracker::OnChildViewAdded(
    /* [in] */ IView* parent,
    /* [in] */ IView* child)
{
    // dirties the index to column map
    mOwner->mColumnToChildIndex = NULL;

    if (mListener != NULL) {
        mListener->OnChildViewAdded(parent, child);
    }

    return NOERROR;
}

ECode TableRow::ChildrenTracker::OnChildViewRemoved(
    /* [in] */ IView* parent,
    /* [in] */ IView* child)
{
    // dirties the index to column map
    mOwner->mColumnToChildIndex = NULL;

    if (mListener != NULL) {
        mListener->OnChildViewRemoved(parent, child);
    }

    return NOERROR;
}

//==============================================================================
//              TableRow
//==============================================================================
CAR_INTERFACE_IMPL(TableRow, LinearLayout, ITableRow)

TableRow::TableRow()
    : mNumColumns(0)
{
}

/**
 * <p>Creates a new TableRow for the given context.</p>
 *
 * @param context the application environment
 */
ECode TableRow::constructor(
    /* [in] */ IContext* context)
{
    LinearLayout::constructor(context);
    InitTableRow();
    return NOERROR;
}

/**
 * <p>Creates a new TableRow for the given context and with the
 * specified set attributes.</p>
 *
 * @param context the application environment
 * @param attrs a collection of attributes
 */
ECode TableRow::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    LinearLayout::constructor(context, attrs);
    InitTableRow();
    return NOERROR;
}

void TableRow::InitTableRow()
{
    AutoPtr<IViewGroupOnHierarchyChangeListener> oldListener = mOnHierarchyChangeListener;
    mChildrenTracker = new ChildrenTracker(this);
    if (oldListener != NULL) {
        mChildrenTracker->SetOnHierarchyChangeListener(oldListener);
    }
    LinearLayout::SetOnHierarchyChangeListener(mChildrenTracker);
}

/**
 * {@inheritDoc}
 */
ECode TableRow::SetOnHierarchyChangeListener(
    /* [in] */ IViewGroupOnHierarchyChangeListener* listener)
{
    mChildrenTracker->SetOnHierarchyChangeListener(listener);

    return NOERROR;
}

/**
 * <p>Collapses or restores a given column.</p>
 *
 * @param columnIndex the index of the column
 * @param collapsed true if the column must be collapsed, false otherwise
 * {@hide}
 */
ECode TableRow::SetColumnCollapsed(
    /* [in] */ Int32 columnIndex,
    /* [in] */ Boolean collapsed)
{
    AutoPtr<IView> child;
    GetVirtualChildAt(columnIndex, (IView**)&child);
    if (child != NULL) {
        child->SetVisibility(collapsed ? IView::GONE : IView::VISIBLE);
    }

    return NOERROR;
}

/**
 * {@inheritDoc}
 */
ECode TableRow::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    // enforce horizontal layout
    MeasureHorizontal(widthMeasureSpec, heightMeasureSpec);
    return NOERROR;
}

/**
 * {@inheritDoc}
 */
ECode TableRow::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 r,
    /* [in] */ Int32 b)
{
    // enforce horizontal layout
    LayoutHorizontal(l, t, r, b);
    return NOERROR;
}

/**
 * {@inheritDoc}
 */
ECode TableRow::GetVirtualChildAt(
    /* [in] */ Int32 i,
    /* [out] */ IView** v)
{
    VALIDATE_NOT_NULL(v)
    if (mColumnToChildIndex == NULL) {
        MapIndexAndColumns();
    }

    Int32 deflectedIndex = -1;
    HashMap<Int32, Int32>::Iterator it = mColumnToChildIndex->Find(i);
    if (it != mColumnToChildIndex->End()) {
        deflectedIndex = it->mSecond;
    }
    if (deflectedIndex != -1) {
        return GetChildAt(deflectedIndex, v);
    }

    *v = NULL;
    return NOERROR;
}

/**
 * {@inheritDoc}
 */
ECode TableRow::GetVirtualChildCount(
    /* [out] */ Int32* count)
{
    if (mColumnToChildIndex == NULL) {
        MapIndexAndColumns();
    }
    *count = mNumColumns;
    return NOERROR;
}

void TableRow::MapIndexAndColumns()
{
    if (mColumnToChildIndex == NULL) {
        Int32 virtualCount = 0;
        Int32 count;
        GetChildCount(&count);

        mColumnToChildIndex = new HashMap<Int32, Int32>(5);

        AutoPtr<IView> child;
        for (Int32 i = 0; i < count; i++) {
            child = NULL;
            GetChildAt(i, (IView**)&child);
            AutoPtr<IViewGroupLayoutParams> temp;
            child->GetLayoutParams((IViewGroupLayoutParams**)&temp);
            AutoPtr<ITableRowLayoutParams> layoutParams = ITableRowLayoutParams::Probe(temp);
            Int32 column, span;
            layoutParams->GetColumn(&column);
            layoutParams->GetSpan(&span);

            if (column >= virtualCount) {
                virtualCount = column;
            }

            for (Int32 j = 0; j < span; j++) {
                (*mColumnToChildIndex)[virtualCount++] = i;
            }
        }

        mNumColumns = virtualCount;
    }
}

/**
 * {@inheritDoc}
 */
Int32 TableRow::MeasureNullChild(
    /* [in] */ Int32 childIndex)
{
    return (*mConstrainedColumnWidths)[childIndex];
}

/**
 * {@inheritDoc}
 */
void TableRow::MeasureChildBeforeLayout(
    /* [in] */ IView* child,
    /* [in] */ Int32 childIndex,
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 totalWidth,
    /* [in] */ Int32 heightMeasureSpec,
    /* [in] */ Int32 totalHeight)
{
    if (mConstrainedColumnWidths != NULL) {
        AutoPtr<IViewGroupLayoutParams> temp;
        child->GetLayoutParams((IViewGroupLayoutParams**)&temp);
        AutoPtr<ITableRowLayoutParams> _lp = ITableRowLayoutParams::Probe(temp);
        Int32 measureMode = MeasureSpec::EXACTLY;
        Int32 columnWidth = 0;

        Int32 span;
        _lp->GetSpan(&span);
        ArrayOf<Int32>* constrainedColumnWidths = mConstrainedColumnWidths;
        for (Int32 i = 0; i < span; i++) {
            columnWidth += (*constrainedColumnWidths)[childIndex + i];
        }

        Int32 gravity;
        ILinearLayoutLayoutParams::Probe(_lp)->GetGravity(&gravity);
        Boolean isHorizontalGravity;
        AutoPtr<IGravity> g;
        CGravity::AcquireSingleton((IGravity**)&g);
        g->IsHorizontal(gravity, &isHorizontalGravity);

        if (isHorizontalGravity) {
            measureMode = MeasureSpec::AT_MOST;
        }

        // no need to care about padding here,
        // ViewGroup.getChildMeasureSpec() would get rid of it anyway
        // because of the EXACTLY measure spec we use
        Int32 leftMargin, rightMargin, topMargin, bottomMargin, heightLayout;
        IViewGroupMarginLayoutParams::Probe(_lp)->GetLeftMargin(&leftMargin);
        IViewGroupMarginLayoutParams::Probe(_lp)->GetRightMargin(&rightMargin);
        Int32 childWidthMeasureSpec = MeasureSpec::MakeMeasureSpec(
            Elastos::Core::Math::Max(0, columnWidth - leftMargin - rightMargin), measureMode);
        IViewGroupMarginLayoutParams::Probe(_lp)->GetTopMargin(&topMargin);
        IViewGroupMarginLayoutParams::Probe(_lp)->GetBottomMargin(&bottomMargin);
        IViewGroupLayoutParams::Probe(_lp)->GetHeight(&heightLayout);
        Int32 childHeightMeasureSpec = GetChildMeasureSpec(heightMeasureSpec,
                mPaddingTop + mPaddingBottom + topMargin +
                bottomMargin + totalHeight, heightLayout);

        child->Measure(childWidthMeasureSpec, childHeightMeasureSpec);
        TableRowLayoutParams* lp = (TableRowLayoutParams*)_lp.Get();
        if (isHorizontalGravity) {
            Int32 childWidth;
            child->GetMeasuredWidth(&childWidth);
            (*lp->mOffset)[TableRowLayoutParams::LOCATION_NEXT] = columnWidth - childWidth;

            Int32 layoutDirection;
            GetLayoutDirection(&layoutDirection);
            Int32 absoluteGravity;
            g->GetAbsoluteGravity(gravity, layoutDirection, &absoluteGravity);
            switch (absoluteGravity & IGravity::HORIZONTAL_GRAVITY_MASK) {
                case IGravity::LEFT:
                    // don't offset on X axis
                    break;
                case IGravity::RIGHT:
                    (*lp->mOffset)[TableRowLayoutParams::LOCATION] = (*lp->mOffset)[TableRowLayoutParams::LOCATION_NEXT];
                    break;
                case IGravity::CENTER_HORIZONTAL:
                    (*lp->mOffset)[TableRowLayoutParams::LOCATION] = (*lp->mOffset)[TableRowLayoutParams::LOCATION_NEXT] / 2;
                    break;
            }
        }
        else {
            (*lp->mOffset)[TableRowLayoutParams::LOCATION] = (*lp->mOffset)[TableRowLayoutParams::LOCATION_NEXT] = 0;
        }
    }
    else {
        // fail silently when column widths are not available
        LinearLayout::MeasureChildBeforeLayout(child, childIndex, widthMeasureSpec,
                totalWidth, heightMeasureSpec, totalHeight);
    }
}

/**
 * {@inheritDoc}
 */
Int32 TableRow::GetChildrenSkipCount(
    /* [in] */ IView* child,
    /* [in] */ Int32 index)
{
    AutoPtr<IViewGroupLayoutParams> layoutParams;
    child->GetLayoutParams((IViewGroupLayoutParams**)&layoutParams);
    Int32 span;
    ITableRowLayoutParams::Probe(layoutParams)->GetSpan(&span);

    // when the span is 1 (default), we need to skip 0 child
    return span - 1;
}

/**
 * {@inheritDoc}
 */
Int32 TableRow::GetLocationOffset(
    /* [in] */ IView* child)
{
    AutoPtr<IViewGroupLayoutParams> layoutParams;
    child->GetLayoutParams((IViewGroupLayoutParams**)&layoutParams);
    TableRowLayoutParams* lp = (TableRowLayoutParams*)ITableRowLayoutParams::Probe(layoutParams);
    return (*lp->mOffset)[TableRowLayoutParams::LOCATION];
}

/**
 * {@inheritDoc}
 */
Int32 TableRow::GetNextLocationOffset(
    /* [in] */ IView* child)
{
    AutoPtr<IViewGroupLayoutParams> layoutParams;
    child->GetLayoutParams((IViewGroupLayoutParams**)&layoutParams);
    TableRowLayoutParams* lp = (TableRowLayoutParams*)ITableRowLayoutParams::Probe(layoutParams);
    return (*lp->mOffset)[TableRowLayoutParams::LOCATION_NEXT];
}

/**
 * <p>Measures the preferred width of each child, including its margins.</p>
 *
 * @param widthMeasureSpec the width constraint imposed by our parent
 *
 * @return an array of integers corresponding to the width of each cell, or
 *         column, in this row
 * {@hide}
 */
AutoPtr<ArrayOf<Int32> > TableRow::GetColumnsWidths(
    /* [in] */ Int32 widthMeasureSpec)
{
    Int32 numColumns;
    GetVirtualChildCount(&numColumns);
    if (mColumnWidths == NULL || numColumns != mColumnWidths->GetLength()) {
        mColumnWidths = ArrayOf<Int32>::Alloc(numColumns);
    }

    AutoPtr<ArrayOf<Int32> > columnWidths = mColumnWidths;

    AutoPtr<IView> child;
    for (Int32 i = 0; i < numColumns; i++) {
        child = NULL;
        GetVirtualChildAt(i, (IView**)&child);
        Int32 visible;
        if (child != NULL && (child->GetVisibility(&visible), visible != IView::GONE)) {
            AutoPtr<IViewGroupLayoutParams> layoutParams;
            child->GetLayoutParams((IViewGroupLayoutParams**)&layoutParams);
            TableRowLayoutParams* lp = (TableRowLayoutParams*)ITableRowLayoutParams::Probe(layoutParams);

            if (lp->mSpan == 1) {
                Int32 spec;
                switch (lp->mWidth) {
                    case IViewGroupLayoutParams::WRAP_CONTENT:
                        spec = GetChildMeasureSpec(widthMeasureSpec, 0, IViewGroupLayoutParams::WRAP_CONTENT);
                        break;
                    case IViewGroupLayoutParams::MATCH_PARENT:
                        spec = MeasureSpec::MakeMeasureSpec(0, MeasureSpec::UNSPECIFIED);
                        break;
                    default:
                        spec = MeasureSpec::MakeMeasureSpec(lp->mWidth, MeasureSpec::EXACTLY);
                }
                child->Measure(spec, spec);

                Int32 width;
                child->GetMeasuredWidth(&width);

                (*columnWidths)[i] = width + lp->mLeftMargin + lp->mRightMargin;
            }
            else {
                (*columnWidths)[i] = 0;
            }
        } else {
            (*columnWidths)[i] = 0;
        }
    }

    return columnWidths;
}

/**
 * <p>Sets the width of all of the columns in this row. At layout time,
 * this row sets a fixed width, as defined by <code>columnWidths</code>,
 * on each child (or cell, or column.)</p>
 *
 * @param columnWidths the fixed width of each column that this row must
 *                     honor
 * @throws IllegalArgumentException when columnWidths' length is smaller
 *         than the number of children in this row
 * {@hide}
 */
ECode TableRow::SetColumnsWidthConstraints(
    /* [in] */ ArrayOf<Int32>* columnWidths)
{
    Int32 count;
    if (columnWidths == NULL ||
        columnWidths->GetLength() < (GetVirtualChildCount(&count), count)) {
//        throw new IllegalArgumentException(
//                "columnWidths should be >= getVirtualChildCount()");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mConstrainedColumnWidths = columnWidths;

    return NOERROR;
}

/**
 * {@inheritDoc}
 */
ECode TableRow::GenerateLayoutParams(
    /* [in] */ IAttributeSet* attrs,
    /* [out] */ IViewGroupLayoutParams** params)
{
    VALIDATE_NOT_NULL(params);
    AutoPtr<ITableRowLayoutParams> lp;
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    CTableRowLayoutParams::New(ctx, attrs, (ITableRowLayoutParams**)&lp);
    *params = IViewGroupLayoutParams::Probe(lp);
    REFCOUNT_ADD(*params);
    return NOERROR;
}

/**
 * Returns a set of layout parameters with a width of
 * {@link android.view.ViewGroup.LayoutParams#MATCH_PARENT},
 * a height of {@link android.view.ViewGroup.LayoutParams#WRAP_CONTENT} and no spanning.
 */
ECode TableRow::GenerateDefaultLayoutParams(
    /* [out] */ IViewGroupLayoutParams** params)
{
    VALIDATE_NOT_NULL(params)
    AutoPtr<ITableRowLayoutParams> lp;
    CTableRowLayoutParams::New((ITableRowLayoutParams**)&lp);
    *params = IViewGroupLayoutParams::Probe(lp);
    REFCOUNT_ADD(*params);
    return NOERROR;
}

/**
 * {@inheritDoc}
 */
Boolean TableRow::CheckLayoutParams(
    /* [in] */ IViewGroupLayoutParams* p)
{
    return ITableRowLayoutParams::Probe(p) != NULL;
}

/**
 * {@inheritDoc}
 */
AutoPtr<IViewGroupLayoutParams> TableRow::GenerateLayoutParams(
    /* [in] */ IViewGroupLayoutParams* p)
{
    AutoPtr<ITableRowLayoutParams> lp;
    CTableRowLayoutParams::New((ITableRowLayoutParams**)&lp);
    return IViewGroupLayoutParams::Probe(lp);
}

ECode TableRow::OnInitializeAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    LinearLayout::OnInitializeAccessibilityEvent(event);
    AutoPtr<ICharSequence> seq;
    CString::New(String("CTableRow"), (ICharSequence**)&seq);
    return IAccessibilityRecord::Probe(event)->SetClassName(seq);
}


ECode TableRow::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IAccessibilityNodeInfo* info)
{
    LinearLayout::OnInitializeAccessibilityNodeInfo(info);
    AutoPtr<ICharSequence> seq;
    CString::New(String("CTableRow"), (ICharSequence**)&seq);
    return info->SetClassName(seq);
}

} // namespace Widget
} // namespace Droid
} // namespace Elastos
