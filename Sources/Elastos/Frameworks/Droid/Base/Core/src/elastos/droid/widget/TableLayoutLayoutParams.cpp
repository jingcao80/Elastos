
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/widget/TableLayoutLayoutParams.h"

namespace Elastos {
namespace Droid {
namespace Widget {

/**
 * {@inheritDoc}
 */
TableLayoutLayoutParams::TableLayoutLayoutParams(
    /* [in] */ IContext* c,
    /* [in] */ IAttributeSet* attrs)
    : LinearLayout::LayoutParams(c, attrs)
{
}

/**
 * {@inheritDoc}
 */
TableLayoutLayoutParams::TableLayoutLayoutParams(
    /* [in] */ Int32 w,
    /* [in] */ Int32 h)
    : LinearLayout::LayoutParams(w, h)
{
}

/**
 * {@inheritDoc}
 */
TableLayoutLayoutParams::TableLayoutLayoutParams(
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ Float initWeight)
    : LinearLayout::LayoutParams(
            IViewGroupLayoutParams::MATCH_PARENT, h, initWeight)
{
}

/**
 * <p>Sets the child width to
 * {@link android.view.ViewGroup.LayoutParams} and the child height to
 * {@link android.view.ViewGroup.LayoutParams#WRAP_CONTENT}.</p>
 */
TableLayoutLayoutParams::TableLayoutLayoutParams()
    : LinearLayout::LayoutParams(
            IViewGroupLayoutParams::MATCH_PARENT,
            IViewGroupLayoutParams::WRAP_CONTENT)
{
}

/**
 * {@inheritDoc}
 */
TableLayoutLayoutParams::TableLayoutLayoutParams(
    /* [in] */ ViewGroup::LayoutParams* p)
    : LinearLayout::LayoutParams(p)
{
}

/**
 * {@inheritDoc}
 */
TableLayoutLayoutParams::TableLayoutLayoutParams(
    /* [in] */ ViewGroup::MarginLayoutParams* source)
    : LinearLayout::LayoutParams(source)
{
}

/**
 * <p>Fixes the row's width to
 * {@link android.view.ViewGroup.LayoutParams#MATCH_PARENT}; the row's
 * height is fixed to
 * {@link android.view.ViewGroup.LayoutParams#WRAP_CONTENT} if no layout
 * height is specified.</p>
 *
 * @param a the styled attributes set
 * @param widthAttr the width attribute to fetch
 * @param heightAttr the height attribute to fetch
 */
ECode TableLayoutLayoutParams::SetBaseAttributes(
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

} // namespace Widget
} // namespace Droid
} // namespace Elastos
