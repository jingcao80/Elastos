
#ifndef __ELASTOS_DROID_WIDGET_ABSOLUTELAYOUT_H__
#define __ELASTOS_DROID_WIDGET_ABSOLUTELAYOUT_H__

#include "elastos/droid/view/ViewGroup.h"

using Elastos::Droid::View::ViewGroup;
using Elastos::Droid::View::IViewGroupLayoutParams;

namespace Elastos {
namespace Droid {
namespace Widget {


/**
 * A layout that lets you specify exact locations (x/y coordinates) of its
 * children. Absolute layouts are less flexible and harder to maintain than
 * other types of layouts without absolute positioning.
 *
 * <p><strong>XML attributes</strong></p> <p> See {@link
 * android.R.styleable#ViewGroup ViewGroup Attributes}, {@link
 * android.R.styleable#View View Attributes}</p>
 *
 * @deprecated Use {@link android.widget.FrameLayout}, {@link android.widget.RelativeLayout}
 *             or a custom layout instead.
 */
class AbsoluteLayout
    : public ViewGroup
    , public IAbsoluteLayout
{
public:
/**
 * Per-child layout information associated with AbsoluteLayout.
 * See
 * {@link android.R.styleable#AbsoluteLayout_Layout Absolute Layout Attributes}
 * for a list of all child view attributes that this class supports.
 */
    class LayoutParams
        : public ViewGroup::LayoutParams
        , public IAbsoluteLayoutLayoutParams
    {
    public:
        CAR_INTERFACE_DECL()

        LayoutParams();

        /**
         * Creates a new set of layout parameters with the specified width,
         * height and location.
         *
         * @param width the width, either {@link #MATCH_PARENT},
                  {@link #WRAP_CONTENT} or a fixed size in pixels
         * @param height the height, either {@link #MATCH_PARENT},
                  {@link #WRAP_CONTENT} or a fixed size in pixels
         * @param x the X location of the child
         * @param y the Y location of the child
         */
        CARAPI constructor(
            /* [in] */ Int32 width,
            /* [in] */ Int32 height,
            /* [in] */ Int32 x,
            /* [in] */ Int32 y);

        /**
         * Creates a new set of layout parameters. The values are extracted from
         * the supplied attributes set and context. The XML attributes mapped
         * to this set of layout parameters are:
         *
         * <ul>
         *   <li><code>layout_x</code>: the X location of the child</li>
         *   <li><code>layout_y</code>: the Y location of the child</li>
         *   <li>All the XML attributes from
         *   {@link android.view.ViewGroup.LayoutParams}</li>
         * </ul>
         *
         * @param c the application environment
         * @param attrs the set of attributes fom which to extract the layout
         *              parameters values
         */
        CARAPI constructor(
            /* [in] */ IContext* c,
            /* [in] */ IAttributeSet* attrs);

        /**
         * {@inheritDoc}
         */
        CARAPI constructor(
            /* [in] */ IViewGroupLayoutParams* source);

        virtual ~LayoutParams() {}

        CARAPI SetX(
            /* [in] */ Int32 x);

        CARAPI SetY(
            /* [in] */ Int32 y);

        CARAPI GetX(
            /* [out] */ Int32* x);

        CARAPI GetY(
            /* [out] */ Int32* y);

    private:
        CARAPI InitFromAttributes(
            /* [in] */ IContext* c,
            /* [in] */ IAttributeSet* attrs);

    public:
        /**
         * The horizontal, or X, location of the child within the view group.
         */
        Int32 mX;
        /**
         * The vertical, or Y, location of the child within the view group.
         */
        Int32 mY;
    };

public:
    CAR_INTERFACE_DECL()

    //@Override
    CARAPI GenerateLayoutParams(
        /* [in] */ IAttributeSet* attrs,
        /* [out] */ IViewGroupLayoutParams** params);

    AbsoluteLayout();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle,
        /* [in] */ Int32 defStyleRes);

    //@Override
    CARAPI_(void) OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    //@Override
    CARAPI_(Boolean) ShouldDelayChildPressedState();

    /**
     * Returns a set of layout parameters with a width of
     * {@link android.view.ViewGroup.LayoutParams#WRAP_CONTENT},
     * a height of {@link android.view.ViewGroup.LayoutParams#WRAP_CONTENT}
     * and with the coordinates (0, 0).
     */
    //@Override
    CARAPI GenerateDefaultLayoutParams(
        /* [out] */ IViewGroupLayoutParams** lp);

    //@Override
    CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 l,
        /* [in] */ Int32 t,
        /* [in] */ Int32 r,
        /* [in] */ Int32 b);

    // Override to allow type-checking of LayoutParams.
    //@Override
    CARAPI_(Boolean) CheckLayoutParams(
        /* [in] */ IViewGroupLayoutParams* p);

    //@Override
    CARAPI_(AutoPtr<IViewGroupLayoutParams>) GenerateLayoutParams(
        /* [in] */ IViewGroupLayoutParams* p);
};


}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif // __ELASTOS_DROID_WIDGET_ABSOLUTELAYOUT_H__

