
#include "elastos/droid/widget/TableLayout.h"
#include "elastos/droid/widget/CTableLayoutLayoutParams.h"
#include "elastos/droid/widget/TableRow.h"
#include "elastos/droid/utility/CSparseBooleanArray.h"
#include "elastos/droid/R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/Math.h>
#include <elastos/core/StringUtils.h>

using Elastos::Droid::Utility::CSparseBooleanArray;
using Elastos::Droid::View::EIID_IViewGroupOnHierarchyChangeListener;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Droid::R;
using Elastos::Core::StringUtils;
using Elastos::Core::ICharSequence;
using Elastos::Utility::Regex::IPattern;
using Elastos::Utility::Regex::IPatternHelper;
using Elastos::Utility::Regex::CPatternHelper;
using Elastos::Utility::Regex::IMatcher;

namespace Elastos {
namespace Droid {
namespace Widget {

CAR_INTERFACE_IMPL(TableLayout::TableLayoutLayoutParams, LinearLayout::LinearLayoutLayoutParams, ITableLayoutLayoutParams)

TableLayout::TableLayoutLayoutParams::TableLayoutLayoutParams()
{}

TableLayout::TableLayoutLayoutParams::~TableLayoutLayoutParams()
{}

ECode TableLayout::TableLayoutLayoutParams::constructor(
    /* [in] */ IContext* c,
    /* [in] */ IAttributeSet* attrs)
{
    return LinearLayout::LinearLayoutLayoutParams::constructor(c, attrs);
}

ECode TableLayout::TableLayoutLayoutParams::constructor(
    /* [in] */ Int32 w,
    /* [in] */ Int32 h)
{
    return LinearLayout::LinearLayoutLayoutParams::constructor(IViewGroupLayoutParams::MATCH_PARENT, h);
}

ECode TableLayout::TableLayoutLayoutParams::constructor(
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ Float initWeight)
{
    return LinearLayout::LinearLayoutLayoutParams::constructor(IViewGroupLayoutParams::MATCH_PARENT, h, initWeight);
}

ECode TableLayout::TableLayoutLayoutParams::constructor()
{
    return LinearLayout::LinearLayoutLayoutParams::constructor(IViewGroupLayoutParams::MATCH_PARENT, WRAP_CONTENT);
}

ECode TableLayout::TableLayoutLayoutParams::constructor(
    /* [in] */ IViewGroupLayoutParams* p)
{
    return LinearLayout::LinearLayoutLayoutParams::constructor(p);
}

ECode TableLayout::TableLayoutLayoutParams::constructor(
    /* [in] */ IViewGroupMarginLayoutParams* source)
{
    return LinearLayout::LinearLayoutLayoutParams::constructor(source);
}

ECode TableLayout::TableLayoutLayoutParams::SetBaseAttributes(
    /* [in] */ ITypedArray* a,
    /* [in] */ Int32 widthAttr,
    /* [in] */ Int32 heightAttr)
{
    mWidth = IViewGroupLayoutParams::MATCH_PARENT;

    Boolean res;
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
//     TableLayout::PassThroughHierarchyChangeListener
//==============================================================================
CAR_INTERFACE_IMPL(TableLayout::PassThroughHierarchyChangeListener, Object, IViewGroupOnHierarchyChangeListener)

TableLayout::PassThroughHierarchyChangeListener::PassThroughHierarchyChangeListener(
    /* [in] */ TableLayout* owner)
    : mOwner(owner)
{
}

/**
 * {@inheritDoc}
 */
ECode TableLayout::PassThroughHierarchyChangeListener::OnChildViewAdded(
    /* [in] */ IView* parent,
    /* [in] */ IView* child)
{
    mOwner->TrackCollapsedColumns(child);

    if (mOnHierarchyChangeListener != NULL) {
        mOnHierarchyChangeListener->OnChildViewAdded(parent, child);
    }

    return NOERROR;
}

/**
 * {@inheritDoc}
 */
ECode TableLayout::PassThroughHierarchyChangeListener::OnChildViewRemoved(
    /* [in] */ IView* parent,
    /* [in] */ IView* child)
{
    if (mOnHierarchyChangeListener != NULL) {
        mOnHierarchyChangeListener->OnChildViewRemoved(parent, child);
    }

    return NOERROR;
}


//==============================================================================
//     TableLayout
//==============================================================================

CAR_INTERFACE_IMPL(TableLayout, LinearLayout, ITableLayout)

TableLayout::TableLayout()
    : mShrinkAllColumns(FALSE)
    , mStretchAllColumns(FALSE)
    , mInitialized(FALSE)
{}

/**
 * <p>Creates a new TableLayout for the given context.</p>
 *
 * @param context the application environment
 */
ECode TableLayout::constructor(
    /* [in] */ IContext* context)
{
    FAIL_RETURN(LinearLayout::constructor(context))
    InitTableLayout();
    return NOERROR;
}

/**
 * <p>Creates a new TableLayout for the given context and with the
 * specified set attributes.</p>
 *
 * @param context the application environment
 * @param attrs a collection of attributes
 */
ECode TableLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    FAIL_RETURN(LinearLayout::constructor(context, attrs))
    FAIL_RETURN(InitFromAttributes(context, attrs))
    InitTableLayout();
    return NOERROR;
}

TableLayout::~TableLayout()
{}

ECode TableLayout::InitFromAttributes(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::TableLayout);
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, attrIds, (ITypedArray**)&a);

    String stretchedColumns;
    a->GetString(R::styleable::TableLayout_stretchColumns, &stretchedColumns);
    if (!stretchedColumns.IsNull()) {
        if (stretchedColumns.GetChar(0) == '*') {
            mStretchAllColumns = TRUE;
        }
        else {
            mStretchableColumns = ParseColumns(stretchedColumns);
        }
    }

    String shrinkedColumns;
    a->GetString(R::styleable::TableLayout_shrinkColumns, &shrinkedColumns);
    if (!shrinkedColumns.IsNull()) {
        if (shrinkedColumns.GetChar(0) == '*') {
            mShrinkAllColumns = TRUE;
        }
        else {
            mShrinkableColumns = ParseColumns(shrinkedColumns);
        }
    }

    String collapsedColumns;
    a->GetString(R::styleable::TableLayout_collapseColumns, &collapsedColumns);
    if (!collapsedColumns.IsNull()) {
        mCollapsedColumns = ParseColumns(collapsedColumns);
    }

    a->Recycle();

    return NOERROR;
}

/**
 * <p>Parses a sequence of columns ids defined in a CharSequence with the
 * following pattern (regex): \d+(\s*,\s*\d+)*</p>
 *
 * <p>Examples: "1" or "13, 7, 6" or "".</p>
 *
 * <p>The result of the parsing is stored in a sparse Boolean array. The
 * parsed column ids are used as the keys of the sparse array. The values
 * are always TRUE.</p>
 *
 * @param sequence a sequence of column ids, can be empty but not NULL
 * @return a sparse array of Boolean mapping column indexes to the columns
 *         collapse state
 */
AutoPtr<ISparseBooleanArray> TableLayout::ParseColumns(
    /* [in] */ const String& sequence)
{
    AutoPtr<ISparseBooleanArray> columns;
    CSparseBooleanArray::New((ISparseBooleanArray**)&columns);
    const String str("\\s*,\\s*");
    AutoPtr<IPatternHelper> helper;
    CPatternHelper::AcquireSingleton((IPatternHelper**)&helper);
    AutoPtr<IPattern> pattern;
    helper->Compile(str, (IPattern**)&pattern);
    AutoPtr<ArrayOf<String> > columnDefs;
    pattern->Split(sequence, (ArrayOf<String>**)&columnDefs);

    if (columnDefs != NULL) {
        for (Int32 i = 0; i < columnDefs->GetLength(); ++i) {
            Int32 columnIndex = StringUtils::ParseInt32((*columnDefs)[i]);
            // only valid, i.e. positive, columns indexes are handled
            if (columnIndex >= 0) {
                // putting TRUE in this sparse array indicates that the
                // column index was defined in the XML file
                columns->Put(columnIndex, TRUE);
            }
        }
    }

    return columns;
}

/**
 * <p>Performs initialization common to prorgrammatic use and XML use of
 * this widget.</p>
 */
void TableLayout::InitTableLayout()
{
    if (mCollapsedColumns == NULL) {
        CSparseBooleanArray::New((ISparseBooleanArray**)&mCollapsedColumns);
    }
    if (mStretchableColumns == NULL) {
        CSparseBooleanArray::New((ISparseBooleanArray**)&mStretchableColumns);
    }
    if (mShrinkableColumns == NULL) {
        CSparseBooleanArray::New((ISparseBooleanArray**)&mShrinkableColumns);
    }

    // TableLayouts are always in vertical orientation; keep this tracked
    // for shared LinearLayout code.
    SetOrientation(ILinearLayout::VERTICAL);

    mPassThroughListener = new PassThroughHierarchyChangeListener(this);
    // make sure to call the parent class method to avoid potential
    // infinite loops
    LinearLayout::SetOnHierarchyChangeListener(mPassThroughListener);

    mInitialized = TRUE;
}

/**
 * {@inheritDoc}
 */
ECode TableLayout::SetOnHierarchyChangeListener(
    /* [in] */ IViewGroupOnHierarchyChangeListener* listener)
{
    // the user listener is delegated to our pass-through listener
    mPassThroughListener->mOnHierarchyChangeListener = listener;

    return NOERROR;
}

void TableLayout::RequestRowsLayout()
{
    if (mInitialized) {
        Int32 count;
        GetChildCount(&count);
        for (Int32 i = 0; i < count; i++) {
            AutoPtr<IView> v;
            GetChildAt(i, (IView**)&v);
            v->RequestLayout();
        }
    }
}

/**
 * {@inheritDoc}
 */

ECode TableLayout::RequestLayout()
{
    if (mInitialized) {
        Int32 count;
        GetChildCount(&count);
        for (Int32 i = 0; i < count; i++) {
            AutoPtr<IView> v;
            GetChildAt(i, (IView**)&v);
            v->ForceLayout();
        }
    }

    return LinearLayout::RequestLayout();
}

/**
 * <p>Indicates whether all columns are shrinkable or not.</p>
 *
 * @return TRUE if all columns are shrinkable, FALSE otherwise
 */
ECode TableLayout::IsShrinkAllColumns(
    /* [out] */ Boolean* result)
{
    *result = mShrinkAllColumns;
    return NOERROR;
}

/**
 * <p>Convenience method to mark all columns as shrinkable.</p>
 *
 * @param shrinkAllColumns TRUE to mark all columns shrinkable
 *
 * @attr ref android.R.styleable#TableLayout_shrinkColumns
 */
ECode TableLayout::SetShrinkAllColumns(
    /* [in] */ Boolean shrinkAllColumns)
{
    mShrinkAllColumns = shrinkAllColumns;

    return NOERROR;
}

/**
 * <p>Indicates whether all columns are stretchable or not.</p>
 *
 * @return TRUE if all columns are stretchable, FALSE otherwise
 */
ECode TableLayout::IsStretchAllColumns(
    /* [out] */ Boolean* result)
{
    *result = mStretchAllColumns;
    return NOERROR;
}

/**
 * <p>Convenience method to mark all columns as stretchable.</p>
 *
 * @param stretchAllColumns TRUE to mark all columns stretchable
 *
 * @attr ref android.R.styleable#TableLayout_stretchColumns
 */
ECode TableLayout::SetStretchAllColumns(
    /* [in] */ Boolean stretchAllColumns)
{
    mStretchAllColumns = stretchAllColumns;

    return NOERROR;
}

/**
 * <p>Collapses or restores a given column. When collapsed, a column
 * does not appear on screen and the extra space is reclaimed by the
 * other columns. A column is collapsed/restored only when it belongs to
 * a {@link android.widget.TableRow}.</p>
 *
 * <p>Calling this method requests a layout operation.</p>
 *
 * @param columnIndex the index of the column
 * @param isCollapsed TRUE if the column must be collapsed, FALSE otherwise
 *
 * @attr ref android.R.styleable#TableLayout_collapseColumns
 */
ECode TableLayout::SetColumnCollapsed(
    /* [in] */ Int32 columnIndex,
    /* [in] */ Boolean isCollapsed)
{
    // update the collapse status of the column
    mCollapsedColumns->Put(columnIndex, isCollapsed);

    Int32 count;
    GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> view;
        GetChildAt(i, (IView**)&view);
        if (ITableRow::Probe(view) != NULL) {
            TableRow* tr = (TableRow*)ITableRow::Probe(view);
            tr->SetColumnCollapsed(columnIndex, isCollapsed);
        }
    }

    RequestRowsLayout();

    return NOERROR;
}

/**
 * <p>Returns the collapsed state of the specified column.</p>
 *
 * @param columnIndex the index of the column
 * @return TRUE if the column is collapsed, FALSE otherwise
 */
ECode TableLayout::IsColumnCollapsed(
    /* [in] */ Int32 columnIndex,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mCollapsedColumns->Get(columnIndex, result);
}

/**
 * <p>Makes the given column stretchable or not. When stretchable, a column
 * takes up as much as available space as possible in its row.</p>
 *
 * <p>Calling this method requests a layout operation.</p>
 *
 * @param columnIndex the index of the column
 * @param isStretchable TRUE if the column must be stretchable,
 *                      FALSE otherwise. Default is FALSE.
 *
 * @attr ref android.R.styleable#TableLayout_stretchColumns
 */
ECode TableLayout::SetColumnStretchable(
    /* [in] */ Int32 columnIndex,
    /* [in] */ Boolean isStretchable)
{
    mStretchableColumns->Put(columnIndex, isStretchable);
    RequestRowsLayout();

    return NOERROR;
}

/**
 * <p>Returns whether the specified column is stretchable or not.</p>
 *
 * @param columnIndex the index of the column
 * @return TRUE if the column is stretchable, FALSE otherwise
 */
ECode TableLayout::IsColumnStretchable(
    /* [in] */ Int32 columnIndex,
    /* [out] */ Boolean* isColumnStretchable)
{
    VALIDATE_NOT_NULL(isColumnStretchable)
    Boolean res = FALSE;
    if (mStretchableColumns) {
        mStretchableColumns->Get(columnIndex, &res);
    }
    *isColumnStretchable = mStretchAllColumns || res;
    return NOERROR;
}

/**
 * <p>Makes the given column shrinkable or not. When a row is too wide, the
 * table can reclaim extra space from shrinkable columns.</p>
 *
 * <p>Calling this method requests a layout operation.</p>
 *
 * @param columnIndex the index of the column
 * @param isShrinkable TRUE if the column must be shrinkable,
 *                     FALSE otherwise. Default is FALSE.
 *
 * @attr ref android.R.styleable#TableLayout_shrinkColumns
 */
ECode TableLayout::SetColumnShrinkable(
    /* [in] */ Int32 columnIndex,
    /* [in] */ Boolean isShrinkable)
{
    mShrinkableColumns->Put(columnIndex, isShrinkable);
    RequestRowsLayout();

    return NOERROR;
}

/**
 * <p>Returns whether the specified column is shrinkable or not.</p>
 *
 * @param columnIndex the index of the column
 * @return TRUE if the column is shrinkable, FALSE otherwise. Default is FALSE.
 */
ECode TableLayout::IsColumnShrinkable(
    /* [in] */ Int32 columnIndex,
    /* [out] */ Boolean* isColumnShrinkable)
{
    VALIDATE_NOT_NULL(isColumnShrinkable)
    Boolean res = FALSE;
    if (mShrinkableColumns) {
        mShrinkableColumns->Get(columnIndex, &res);
    }
    *isColumnShrinkable = mShrinkAllColumns || res;
    return NOERROR;
}

/**
 * <p>Applies the columns collapse status to a new row added to this
 * table. This method is invoked by PassThroughHierarchyChangeListener
 * upon child insertion.</p>
 *
 * <p>This method only applies to {@link android.widget.TableRow}
 * instances.</p>
 *
 * @param child the newly added child
 */
void TableLayout::TrackCollapsedColumns(
    /* [in] */ IView* child)
{
    if (ITableRow::Probe(child) != NULL) {
        TableRow* row = (TableRow*)child;
        AutoPtr<ISparseBooleanArray> collapsedColumns = mCollapsedColumns;

        Int32 count;
        collapsedColumns->GetSize(&count);
        for (Int32 i = 0; i < count; i++) {
            Int32 columnIndex;
            collapsedColumns->KeyAt(i, &columnIndex);
            Boolean isCollapsed;
            collapsedColumns->ValueAt(i, &isCollapsed);
            // the collapse status is set only when the column should be
            // collapsed; otherwise, this might affect the default
            // visibility of the row's children
            if (isCollapsed) {
                row->SetColumnCollapsed(columnIndex, isCollapsed);
            }
        }
    }
}

/**
 * {@inheritDoc}
 */

ECode TableLayout::AddView(
    /* [in] */ IView* child)
{
    FAIL_RETURN(LinearLayout::AddView(child));
    RequestRowsLayout();

    return NOERROR;
}

/**
 * {@inheritDoc}
 */

ECode TableLayout::AddView(
    /* [in] */ IView* child,
    /* [in] */ Int32 index)
{
    FAIL_RETURN(LinearLayout::AddView(child, index));
    RequestRowsLayout();

    return NOERROR;
}

/**
 * {@inheritDoc}
 */

ECode TableLayout::AddView(
    /* [in] */ IView* child,
    /* [in] */ IViewGroupLayoutParams* params)
{
    FAIL_RETURN(LinearLayout::AddView(child, params));
    RequestRowsLayout();

    return NOERROR;
}

/**
 * {@inheritDoc}
 */

ECode TableLayout::AddView(
    /* [in] */ IView* child,
    /* [in] */ Int32 index,
    /* [in] */ IViewGroupLayoutParams* params)
{
    FAIL_RETURN(LinearLayout::AddView(child, index, params));
    RequestRowsLayout();

    return NOERROR;
}

/**
 * {@inheritDoc}
 */
ECode TableLayout::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    // enforce vertical layout
    MeasureVertical(widthMeasureSpec, heightMeasureSpec);
    return NOERROR;
}

/**
 * {@inheritDoc}
 */
ECode TableLayout::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 r,
    /* [in] */ Int32 b)
{
    // enforce vertical layout
    LayoutVertical(l, t, r, b);
    return NOERROR;
}

/**
 * {@inheritDoc}
 */
void TableLayout::MeasureChildBeforeLayout(
    /* [in] */ IView* child,
    /* [in] */ Int32 childIndex,
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 totalWidth,
    /* [in] */ Int32 heightMeasureSpec,
    /* [in] */ Int32 totalHeight)
{
    // when the measured child is a table row, we force the width of its
    // children with the widths computed in findLargestCells()
    if (ITableRow::Probe(child) != NULL) {
        ((TableRow*)child)->SetColumnsWidthConstraints(mMaxWidths);
    }

    LinearLayout::MeasureChildBeforeLayout(child, childIndex,
            widthMeasureSpec, totalWidth, heightMeasureSpec, totalHeight);
}

/**
 * {@inheritDoc}
 */
ECode TableLayout::MeasureVertical(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    FindLargestCells(widthMeasureSpec);
    ShrinkAndStretchColumns(widthMeasureSpec);

    return LinearLayout::MeasureVertical(widthMeasureSpec, heightMeasureSpec);
}

/**
 * <p>Finds the largest cell in each column. For each column, the width of
 * the largest cell is applied to all the other cells.</p>
 *
 * @param widthMeasureSpec the measure constraint imposed by our parent
 */
void TableLayout::FindLargestCells(
    /* [in] */ Int32 widthMeasureSpec)
{
    Boolean firstRow = TRUE;

    // find the maximum width for each column
    // the total number of columns is dynamically changed if we find
    // wider rows as we go through the children
    // the array is reused for each layout operation; the array can grow
    // but never shrinks. Unused extra cells in the array are just ignored
    // this behavior avoids to unnecessary grow the array after the first
    // layout operation
    Int32 count;
    GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);

        Int32 visible;
        child->GetVisibility(&visible);
        if (visible == IView::GONE) {
            continue;
        }

        if (ITableRow::Probe(child) != NULL) {
            TableRow* row = (TableRow*)ITableRow::Probe(child);
            // forces the row's height
            AutoPtr<IViewGroupLayoutParams> layoutParams;
            row->GetLayoutParams((IViewGroupLayoutParams**)&layoutParams);
            layoutParams->SetHeight(IViewGroupLayoutParams::WRAP_CONTENT);

            AutoPtr<ArrayOf<Int32> > widths = row->GetColumnsWidths(widthMeasureSpec);
            Int32 newLength = widths->GetLength();
            // this is the first row, we just need to copy the values
            if (firstRow) {
                if (mMaxWidths == NULL || mMaxWidths->GetLength() != newLength) {
                    mMaxWidths = ArrayOf<Int32>::Alloc(newLength);
                }
                mMaxWidths->Copy(0, widths, 0, newLength);
                firstRow = FALSE;
            }
            else {
                Int32 length = mMaxWidths->GetLength();
                Int32 difference = newLength - length;
                // the current row is wider than the previous rows, so
                // we just grow the array and copy the values
                if (difference > 0) {
                    AutoPtr<ArrayOf<Int32> > oldMaxWidths = mMaxWidths;
                    mMaxWidths = ArrayOf<Int32>::Alloc(newLength);
                    mMaxWidths->Copy(oldMaxWidths, length);
                    mMaxWidths->Copy(length, widths, length, difference);
                }

                // the row is narrower or of the same width as the previous
                // rows, so we find the maximum width for each column
                // if the row is narrower than the previous ones,
                // difference will be negative
                using Elastos::Core::Math;
                length = Math::Min(length, newLength);
                for (Int32 j = 0; j < length; j++) {
                    (*mMaxWidths)[j] = Math::Max((*mMaxWidths)[j], (*widths)[j]);
                }
            }
        }
    }
}

/**
 * <p>Shrinks the columns if their total width is greater than the
 * width allocated by widthMeasureSpec. When the total width is less
 * than the allocated width, this method attempts to stretch columns
 * to fill the remaining space.</p>
 *
 * @param widthMeasureSpec the width measure specification as indicated
 *                         by this widget's parent
 */
void TableLayout::ShrinkAndStretchColumns(
    /* [in] */ Int32 widthMeasureSpec)
{
    // when we have no row, mMaxWidths is not initialized and the loop
    // below could cause a NPE
    if (mMaxWidths == NULL) {
        return;
    }

    // should we honor AT_MOST, EXACTLY and UNSPECIFIED?
    Int32 totalWidth = 0;
    for (Int32 i = 0; i < mMaxWidths->GetLength(); ++i) {
        totalWidth += (*mMaxWidths)[i];
    }

    Int32 size = MeasureSpec::GetSize(widthMeasureSpec) - mPaddingLeft - mPaddingRight;

    Int32 count;
    if ((totalWidth > size) && (mShrinkAllColumns ||
            (mShrinkableColumns->GetSize(&count), count) > 0)) {
        // oops, the largest columns are wider than the row itself
        // fairly redistribute the row's widh among the columns
        MutateColumnsWidth(mShrinkableColumns, mShrinkAllColumns, size, totalWidth);
    }
    else if ((totalWidth < size) && (mStretchAllColumns ||
            (mStretchableColumns->GetSize(&count), count) > 0)) {
        // if we have some space left, we distribute it among the
        // expandable columns
        MutateColumnsWidth(mStretchableColumns, mStretchAllColumns, size, totalWidth);
    }
}

void TableLayout::MutateColumnsWidth(
    /* [in] */ ISparseBooleanArray* columns,
    /* [in] */ Boolean allColumns,
    /* [in] */ Int32 size,
    /* [in] */ Int32 totalWidth)
{
    Int32 skipped = 0;
    ArrayOf<Int32>* maxWidths = mMaxWidths;
    Int32 length = maxWidths->GetLength();
    Int32 count = length;
    if (!allColumns) {
        columns->GetSize(&count);
    }
    Int32 totalExtraSpace = size - totalWidth;
    Int32 extraSpace = totalExtraSpace / count;

    // Column's widths are changed: force child table rows to re-measure.
    // (done by super.measureVertical after shrinkAndStretchColumns.)
    Int32 nbChildren;
    GetChildCount(&nbChildren);
    for (Int32 i = 0; i < nbChildren; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        if (ITableRow::Probe(child) != NULL) {
            child->ForceLayout();
        }
    }

    if (!allColumns) {
        for (Int32 i = 0; i < count; i++) {
            Int32 column;
            columns->KeyAt(i, &column);
            Boolean value;
            columns->ValueAt(i, &value);
            if (value) {
                if (column < length) {
                    (*maxWidths)[column] += extraSpace;
                }
                else {
                    skipped++;
                }
            }
        }
    }
    else {
        for (Int32 i = 0; i < count; i++) {
            (*maxWidths)[i] += extraSpace;
        }

        // we don't skip any column so we can return right away
        return;
    }

    if (skipped > 0 && skipped < count) {
        // reclaim any extra space we left to columns that don't exist
        extraSpace = skipped * extraSpace / (count - skipped);
        for (Int32 i = 0; i < count; i++) {
            Int32 column;
            columns->KeyAt(i, &column);
            Boolean value;
            columns->ValueAt(i, &value);
            if (value && column < length) {
                if (extraSpace > (*maxWidths)[column]) {
                    (*maxWidths)[column] = 0;
                }
                else {
                    (*maxWidths)[column] += extraSpace;
                }
            }
        }
    }
}

/**
 * {@inheritDoc}
 */
ECode TableLayout::GenerateLayoutParams(
    /* [in] */ IAttributeSet* attrs,
    /* [out] */ IViewGroupLayoutParams** params)
{
    VALIDATE_NOT_NULL(params)

    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    AutoPtr<ITableLayoutLayoutParams> lp;
    CTableLayoutLayoutParams::New(ctx, attrs, (ITableLayoutLayoutParams**)&lp);
    *params = IViewGroupLayoutParams::Probe(lp);
    REFCOUNT_ADD(*params);
    return NOERROR;
}

/**
 * Returns a set of layout parameters with a width of
 * {@link android.view.ViewGroup.LayoutParams#MATCH_PARENT},
 * and a height of {@link android.view.ViewGroup.LayoutParams#WRAP_CONTENT}.
 */
ECode TableLayout::GenerateDefaultLayoutParams(
        /* [out] */ IViewGroupLayoutParams** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ITableLayoutLayoutParams> lp;
    CTableLayoutLayoutParams::New((ITableLayoutLayoutParams**)&lp);
    *result = IViewGroupLayoutParams::Probe(lp);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

/**
 * {@inheritDoc}
 */
Boolean TableLayout::CheckLayoutParams(
    /* [in] */ IViewGroupLayoutParams* p)
{
    return ITableLayoutLayoutParams::Probe(p) != NULL;
}

/**
 * {@inheritDoc}
 */
AutoPtr<IViewGroupLayoutParams> TableLayout::GenerateLayoutParams(
    /* [in] */ IViewGroupLayoutParams* p)
{
    AutoPtr<IViewGroupLayoutParams> lp;
    CTableLayoutLayoutParams::New(p, (IViewGroupLayoutParams**)&lp);
    return lp;
}

ECode TableLayout::OnInitializeAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    LinearLayout::OnInitializeAccessibilityEvent(event);
    return IAccessibilityRecord::Probe(event)->SetClassName(CoreUtils::Convert(String("CTableLayout")));
}

ECode TableLayout::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IAccessibilityNodeInfo* info)
{
    LinearLayout::OnInitializeAccessibilityNodeInfo(info);
    return info->SetClassName(CoreUtils::Convert(String("CTableLayout")));
}

} // namespace Widget
} // namespace Droid
} // namespace Elastos
