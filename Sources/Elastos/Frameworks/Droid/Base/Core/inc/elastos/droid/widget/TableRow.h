
#ifndef __ELASTOS_DROID_WIDGET_TABLEROW_H__
#define __ELASTOS_DROID_WIDGET_TABLEROW_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/widget/LinearLayout.h"
#include <elastos/utility/etl/HashMap.h>

namespace Elastos {
namespace Droid {
namespace Widget {

using Elastos::Droid::View::IViewGroupOnHierarchyChangeListener;
using Elastos::Droid::View::IViewGroup;

/**
 * <p>A layout that arranges its children horizontally. A TableRow should
 * always be used as a child of a {@link android.widget.TableLayout}. If a
 * TableRow's parent is not a TableLayout, the TableRow will behave as
 * an horizontal {@link android.widget.LinearLayout}.</p>
 *
 * <p>The children of a TableRow do not need to specify the
 * <code>layout_width</code> and <code>layout_height</code> attributes in the
 * XML file. TableRow always enforces those values to be respectively
 * {@link android.widget.TableLayout.LayoutParams#MATCH_PARENT} and
 * {@link android.widget.TableLayout.LayoutParams#WRAP_CONTENT}.</p>
 *
 * <p>
 * Also see {@link TableRow.LayoutParams android.widget.TableRow.LayoutParams}
 * for layout attributes </p>
 */
class TableRow
    : public LinearLayout
    , public ITableRow
{
private:
    // special transparent hierarchy change listener
    class ChildrenTracker
        : public Object
        , public IViewGroupOnHierarchyChangeListener
    {
        friend class TableRow;

    public:
        ChildrenTracker(
            /* [in] */ TableRow* owner);

        CAR_INTERFACE_DECL()

        CARAPI OnChildViewAdded(
            /* [in] */ IView* parent,
            /* [in] */ IView* child);

        CARAPI OnChildViewRemoved(
            /* [in] */ IView* parent,
            /* [in] */ IView* child);

    private:
        CARAPI SetOnHierarchyChangeListener(
            /* [in] */ IViewGroupOnHierarchyChangeListener* listener);

    private:
        AutoPtr<IViewGroupOnHierarchyChangeListener> mListener;
        TableRow* mOwner;
    };

public:
    CAR_INTERFACE_DECL()

    TableRow();

    /**
     * <p>Creates a new TableRow for the given context.</p>
     *
     * @param context the application environment
     */
    CARAPI constructor(
        /* [in] */ IContext* context);

    /**
     * <p>Creates a new TableRow for the given context and with the
     * specified set attributes.</p>
     *
     * @param context the application environment
     * @param attrs a collection of attributes
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    /**
     * {@inheritDoc}
     */
    //@Override
    virtual CARAPI SetOnHierarchyChangeListener(
        /* [in] */ IViewGroupOnHierarchyChangeListener* listener);

    /**
     * <p>Collapses or restores a given column.</p>
     *
     * @param columnIndex the index of the column
     * @param collapsed true if the column must be collapsed, false otherwise
     * {@hide}
     */
    CARAPI SetColumnCollapsed(
        /* [in] */ Int32 columnIndex,
        /* [in] */ Boolean collapsed);

    /**
     * {@inheritDoc}
     */
    //@Override
    virtual CARAPI GetVirtualChildAt(
        /* [in] */ Int32 i,
        /* [out] */ IView** v);

    /**
     * {@inheritDoc}
     */
    //@Override
    virtual CARAPI GetVirtualChildCount(
        /* [out] */ Int32* count);

    /**
     * {@inheritDoc}
     */
    //@Override
    virtual CARAPI_(Int32) MeasureNullChild(
        /* [in] */ Int32 childIndex);

    /**
     * {@inheritDoc}
     */
    //@Override
    virtual CARAPI_(void) MeasureChildBeforeLayout(
        /* [in] */ IView* child,
        /* [in] */ Int32 childIndex,
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 totalWidth,
        /* [in] */ Int32 heightMeasureSpec,
        /* [in] */ Int32 totalHeight);

    /**
     * {@inheritDoc}
     */
    //@Override
    virtual CARAPI_(Int32) GetChildrenSkipCount(
        /* [in] */ IView* child,
        /* [in] */ Int32 index);

    /**
     * {@inheritDoc}
     */
    //@Override
    virtual CARAPI_(Int32) GetLocationOffset(
        /* [in] */ IView* child);

    /**
     * {@inheritDoc}
     */
    //@Override
    virtual CARAPI_(Int32) GetNextLocationOffset(
        /* [in] */ IView* child);

    /**
     * <p>Measures the preferred width of each child, including its margins.</p>
     *
     * @param widthMeasureSpec the width constraint imposed by our parent
     *
     * @return an array of integers corresponding to the width of each cell, or
     *         column, in this row
     * {@hide}
     */
    CARAPI_(AutoPtr<ArrayOf<Int32> >) GetColumnsWidths(
        /* [in] */ Int32 widthMeasureSpec);

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
    CARAPI SetColumnsWidthConstraints(
        /* [in] */ ArrayOf<Int32>* columnWidths);

    /**
     * {@inheritDoc}
     */
    //@Override
    virtual CARAPI GenerateLayoutParams(
        /* [in] */ IAttributeSet* attrs,
        /* [out] */ IViewGroupLayoutParams** params);

    CARAPI OnInitializeAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    CARAPI OnInitializeAccessibilityNodeInfo(
        /* [in] */ IAccessibilityNodeInfo* info);


protected:
    /**
     * {@inheritDoc}
     */
    //@Override
    virtual CARAPI_(void) OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    /**
     * {@inheritDoc}
     */
    //@Override
    virtual CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 l,
        /* [in] */ Int32 t,
        /* [in] */ Int32 r,
        /* [in] */ Int32 b);

    /**
     * Returns a set of layout parameters with a width of
     * {@link android.view.ViewGroup.LayoutParams#MATCH_PARENT},
     * a height of {@link android.view.ViewGroup.LayoutParams#WRAP_CONTENT} and no spanning.
     */
    //@Override
    CARAPI GenerateDefaultLayoutParams(
        /* [out] */ IViewGroupLayoutParams** params);

    /**
     * {@inheritDoc}
     */
    //@Override
    virtual CARAPI_(Boolean) CheckLayoutParams(
        /* [in] */ IViewGroupLayoutParams* p);

    /**
     * {@inheritDoc}
     */
    //@Override
    virtual CARAPI_(AutoPtr<IViewGroupLayoutParams>) GenerateLayoutParams(
        /* [in] */ IViewGroupLayoutParams* p);

private:
    CARAPI_(void) InitTableRow();

    CARAPI_(void) MapIndexAndColumns();

private:
    Int32 mNumColumns;
    AutoPtr<ArrayOf<Int32> > mColumnWidths;
    AutoPtr<ArrayOf<Int32> > mConstrainedColumnWidths;
    AutoPtr< HashMap<Int32, Int32> > mColumnToChildIndex;

    AutoPtr<ChildrenTracker> mChildrenTracker;
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif
