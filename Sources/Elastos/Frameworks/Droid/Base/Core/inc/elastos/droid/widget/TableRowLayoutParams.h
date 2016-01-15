
#ifndef __ELASTOS_DROID_WIDGET_TABLEROWLAYOUTPARAMS_H__
#define __ELASTOS_DROID_WIDGET_TABLEROWLAYOUTPARAMS_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/widget/LinearLayout.h"

namespace Elastos {
namespace Droid {
namespace Widget {

/**
 * <p>Set of layout parameters used in table rows.</p>
 *
 * @see android.widget.TableLayout.LayoutParams
 *
 * @attr ref android.R.styleable#TableRow_Cell_layout_column
 * @attr ref android.R.styleable#TableRow_Cell_layout_span
 */
class TableRowLayoutParams
    : public LinearLayout::LayoutParams
    , public ITableRowLayoutParams
{
    friend class TableRow;

public:
    CAR_INTERFACE_DECL()

    TableRowLayoutParams();

    /**
     * {@inheritDoc}
     */
    CARAPI constructor(
        /* [in] */ IContext* c,
        /* [in] */ IAttributeSet* attrs);

    /**
     * <p>Sets the child width and the child height.</p>
     *
     * @param w the desired width
     * @param h the desired height
     */
    CARAPI constructor(
        /* [in] */ Int32 w,
        /* [in] */ Int32 h);

    /**
     * <p>Sets the child width, height and weight.</p>
     *
     * @param w the desired width
     * @param h the desired height
     * @param initWeight the desired weight
     */
    CARAPI constructor(
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ Float initWeight);

    /**
     * <p>Sets the child width to {@link android.view.ViewGroup.LayoutParams}
     * and the child height to
     * {@link android.view.ViewGroup.LayoutParams#WRAP_CONTENT}.</p>
     */
    CARAPI constructor();

    /**
     * <p>Puts the view in the specified column.</p>
     *
     * <p>Sets the child width to {@link android.view.ViewGroup.LayoutParams#MATCH_PARENT}
     * and the child height to
     * {@link android.view.ViewGroup.LayoutParams#WRAP_CONTENT}.</p>
     *
     * @param column the column index for the view
     */
    CARAPI constructor(
        /* [in] */ Int32 column);

    /**
     * {@inheritDoc}
     */
    CARAPI constructor(
        /* [in] */ IViewGroupLayoutParams* p);

    /**
     * {@inheritDoc}
     */
    CARAPI constructor(
        /* [in] */ IViewGroupMarginLayoutParams* source);

    virtual CARAPI SetColumn(
        /* [in] */ Int32 column);

    CARAPI GetColumn(
        /* [out] */ Int32* column);

    virtual CARAPI SetSpan(
        /* [in] */ Int32 span);

    CARAPI GetSpan(
        /* [out] */ Int32* span);

    //@Override
    virtual CARAPI SetBaseAttributes(
        /* [in] */ ITypedArray* a,
        /* [in] */ Int32 widthAttr,
        /* [in] */ Int32 heightAttr);

private:
    CARAPI InitFromAttributes(
        /* [in] */ IContext* c,
        /* [in] */ IAttributeSet* attrs);

public:
    /**
     * <p>The column index of the cell represented by the widget.</p>
     */
    Int32 mColumn;

    /**
     * <p>The number of columns the widgets spans over.</p>
     */
    Int32 mSpan;

private:
    static const Int32 LOCATION = 0;
    static const Int32 LOCATION_NEXT = 1;

    AutoPtr< ArrayOf<Int32> > mOffset;
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif
