#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/widget/TableRowLayoutParams.h"
#include "elastos/droid/R.h"

using Elastos::Droid::R;

namespace Elastos {
namespace Droid {
namespace Widget {

const Int32 TableRowLayoutParams::LOCATION;
const Int32 TableRowLayoutParams::LOCATION_NEXT;

CAR_INTERFACE_IMPL(TableRowLayoutParams, LinearLayout::LayoutParams, ITableRowLayoutParams)

/**
 * {@inheritDoc}
 */
TableRowLayoutParams::TableRowLayoutParams()
    : mColumn(0)
    , mSpan(0)
{}

ECode TableRowLayoutParams::constructor(
    /* [in] */ IContext* c,
    /* [in] */ IAttributeSet* attrs)
{
    FAIL_RETURN(LinearLayout::LayoutParams::constructor(c, attrs));
    return InitFromAttributes(c, attrs);
}

ECode TableRowLayoutParams::constructor(
    /* [in] */ Int32 w,
    /* [in] */ Int32 h)
{
    FAIL_RETURN(LinearLayout::LayoutParams::constructor(w, h));
    mColumn = -1;
    mSpan = 1;
    return NOERROR;
}

ECode TableRowLayoutParams::constructor(
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ Float initWeight)
{
    FAIL_RETURN(LinearLayout::LayoutParams::constructor(w, h, initWeight));
    mColumn = -1;
    mSpan = 1;
    return NOERROR;
}

ECode TableRowLayoutParams::constructor()
{
    FAIL_RETURN(LinearLayout::LayoutParams::constructor(
            IViewGroupLayoutParams::MATCH_PARENT,
            IViewGroupLayoutParams::WRAP_CONTENT));
    mColumn = -1;
    mSpan = 1;
    return NOERROR;
}

ECode TableRowLayoutParams::constructor(
    /* [in] */ Int32 column)
{
    FAIL_RETURN(LinearLayout::LayoutParams::constructor(
            IViewGroupLayoutParams::MATCH_PARENT,
            IViewGroupLayoutParams::WRAP_CONTENT));
    mColumn = column;
    mSpan = 1;
    return NOERROR;
}

ECode TableRowLayoutParams::constructor(
    /* [in] */ IViewGroupLayoutParams* p)
{
    FAIL_RETURN(LinearLayout::LayoutParams::constructor(p));
    mColumn = -1;
    mSpan = 1;
    return NOERROR;
}

ECode TableRowLayoutParams::constructor(
    /* [in] */ IViewGroupMarginLayoutParams* source)
{
    FAIL_RETURN(LinearLayout::LayoutParams::constructor(source));
    mColumn = -1;
    mSpan = 1;
    return NOERROR;
}

ECode TableRowLayoutParams::InitFromAttributes(
    /* [in] */ IContext* c,
    /* [in] */ IAttributeSet* attrs)
{
    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
            const_cast<Int32 *>(R::styleable::TableRow_Cell),
            ArraySize(R::styleable::TableRow_Cell));
    AutoPtr<ITypedArray> a;
    c->ObtainStyledAttributes(
            attrs, attrIds, (ITypedArray**)&a);

    a->GetInt32(R::styleable::TableRow_Cell_layout_column,
            -1, &mColumn);
    a->GetInt32(R::styleable::TableRow_Cell_layout_span,
            1, &mSpan);
    if (mSpan <= 1) {
        mSpan = 1;
    }

    a->Recycle();

    return NOERROR;
}

ECode TableRowLayoutParams::SetColumn(
    /* [in] */ Int32 column)
{
    mColumn = column;
    return NOERROR;
}

ECode TableRowLayoutParams::GetColumn(
    /* [out] */ Int32* column)
{
    VALIDATE_NOT_NULL(column)
    *column = mColumn;
    return NOERROR;
}

ECode TableRowLayoutParams::SetSpan(
    /* [in] */ Int32 span)
{
    mSpan = span;
    return NOERROR;
}

ECode TableRowLayoutParams::GetSpan(
    /* [out] */ Int32* span)
{
    VALIDATE_NOT_NULL(span)
    *span = mSpan;
    return NOERROR;
}

ECode TableRowLayoutParams::SetBaseAttributes(
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

} // namespace Widget
} // namespace Droid
} // namespace Elastos
