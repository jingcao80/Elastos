
#ifndef __ELASTOS_DROID_WIDGET_TABLELAYOUTLAYOUTPARAMS_H__
#define __ELASTOS_DROID_WIDGET_TABLELAYOUTLAYOUTPARAMS_H__

namespace Elastos {
namespace Droid {
namespace Widget {

/**
 * <p>This set of layout parameters enforces the width of each child to be
 * {@link #MATCH_PARENT} and the height of each child to be
 * {@link #WRAP_CONTENT}, but only if the height is not specified.</p>
 */
class TableLayoutLayoutParams : public LinearLayout::LayoutParams
{
public:
    /**
     * {@inheritDoc}
     */
    TableLayoutLayoutParams(
        /* [in] */ IContext* c,
        /* [in] */ IAttributeSet* attrs);

    /**
     * {@inheritDoc}
     */
    TableLayoutLayoutParams(
        /* [in] */ Int32 w,
        /* [in] */ Int32 h);

    /**
     * {@inheritDoc}
     */
    TableLayoutLayoutParams(
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ Float initWeight);

    /**
     * <p>Sets the child width to
     * {@link android.view.ViewGroup.LayoutParams} and the child height to
     * {@link android.view.ViewGroup.LayoutParams#WRAP_CONTENT}.</p>
     */
    TableLayoutLayoutParams();

    /**
     * {@inheritDoc}
     */
    TableLayoutLayoutParams(
        /* [in] */ ViewGroup::LayoutParams* p);

    /**
     * {@inheritDoc}
     */
    TableLayoutLayoutParams(
        /* [in] */ ViewGroup::MarginLayoutParams* source);

protected:
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
    //@Override
    virtual CARAPI SetBaseAttributes(
        /* [in] */ ITypedArray* a,
        /* [in] */ Int32 widthAttr,
        /* [in] */ Int32 heightAttr);
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif
