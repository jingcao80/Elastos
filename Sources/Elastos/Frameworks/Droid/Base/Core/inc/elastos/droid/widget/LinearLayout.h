
#ifndef __ELASTOS_DROID_WIDGET_LINEARLAYOUT_H__
#define __ELASTOS_DROID_WIDGET_LINEARLAYOUT_H__

#include "Elastos.Droid.Widget.h"
#include <elastos/droid/view/ViewGroup.h>

using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::IViewGroupMarginLayoutParams;
using Elastos::Droid::View::ViewGroup;

namespace Elastos {
namespace Droid {
namespace Widget {

class ECO_PUBLIC LinearLayout
    : public ViewGroup
    , public ILinearLayout
{
public:
    class LinearLayoutLayoutParams
        : public ViewGroup::MarginLayoutParams
        , public ILinearLayoutLayoutParams
    {
    public:
        CAR_INTERFACE_DECL()

        LinearLayoutLayoutParams();

        CARAPI constructor(
            /* [in] */ IContext* c,
            /* [in] */ IAttributeSet* attrs);

        CARAPI constructor(
            /* [in] */ Int32 width,
            /* [in] */ Int32 height);

        CARAPI constructor(
            /* [in] */ Int32 width,
            /* [in] */ Int32 height,
            /* [in] */ Float weight);

        CARAPI constructor(
            /* [in] */ IViewGroupLayoutParams* source);

        CARAPI constructor(
            /* [in] */ IViewGroupMarginLayoutParams* source);

        CARAPI constructor(
            /* [in] */ ILinearLayoutLayoutParams* source);

        CARAPI SetWeight(
            /* [in] */ Float weight);

        CARAPI GetWeight(
            /* [out] */ Float* weight);

        CARAPI SetGravity(
            /* [in] */ Int32 gravity);

        CARAPI GetGravity(
            /* [out] */ Int32* gravity);

    public:
        /**
         * Indicates how much of the extra space in the LinearLayout will be
         * allocated to the view associated with these LayoutParams. Specify
         * 0 if the view should not be stretched. Otherwise the extra pixels
         * will be pro-rated among all views whose weight is greater than 0.
         */
        Float mWeight;

        /**
         * Gravity for the view associated with these LayoutParams.
         *
         * @see android.view.Gravity
         */
        Int32 mGravity;

    };

public:
    CAR_INTERFACE_DECL()

    LinearLayout();

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
        /* [in] */ IContext* ctx,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

    /**
     * Set how dividers should be shown between items in this layout
     *
     * @param showDividers One or more of {@link #SHOW_DIVIDER_BEGINNING},
     *                     {@link #SHOW_DIVIDER_MIDDLE}, or {@link #SHOW_DIVIDER_END},
     *                     or {@link #SHOW_DIVIDER_NONE} to show no dividers.
     */
    virtual CARAPI SetShowDividers(
        /* [in] */ Int32 showDividers);

    //@Override
    virtual CARAPI ShouldDelayChildPressedState(
        /* [out] */ Boolean* result);

    /**
     * @return A flag set indicating how dividers should be shown around items.
     * @see #setShowDividers(int)
     */
    virtual CARAPI GetShowDividers(
        /* [out] */ Int32* showDividers);

    /**
     * @return the divider Drawable that will divide each item.
     *
     * @see #setDividerDrawable(Drawable)
     *
     * @attr ref android.R.styleable#LinearLayout_divider
     */
    virtual CARAPI GetDividerDrawable(
        /* [out] */ IDrawable** drawable);

    /**
     * Set a drawable to be used as a divider between items.
     *
     * @param divider Drawable that will divide each item.
     *
     * @see #setShowDividers(int)
     *
     * @attr ref android.R.styleable#LinearLayout_divider
     */
    virtual CARAPI SetDividerDrawable(
        /* [in] */ IDrawable* divider);

    /**
     * Set padding displayed on both ends of dividers.
     *
     * @param padding Padding value in pixels that will be applied to each end
     *
     * @see #setShowDividers(int)
     * @see #setDividerDrawable(Drawable)
     * @see #getDividerPadding()
     */
    virtual CARAPI SetDividerPadding(
        /* [in] */ Int32 padding);

    /**
     * Get the padding size used to inset dividers in pixels
     *
     * @see #setShowDividers(int)
     * @see #setDividerDrawable(Drawable)
     * @see #setDividerPadding(int)
     */
    virtual CARAPI GetDividerPadding(
        /* [out] */ Int32* result);

    /**
     * Get the width of the current divider drawable.
     *
     * @hide Used internally by framework.
     */
    virtual CARAPI GetDividerWidth(
        /* [out] */ Int32* result);

    /**
     * <p>Indicates whether widgets contained within this layout are aligned
     * on their baseline or not.</p>
     *
     * @return true when widgets are baseline-aligned, false otherwise
     */
    virtual CARAPI IsBaselineAligned(
        /* [out] */ Boolean* result);

    /**
     * <p>Defines whether widgets contained in this layout are
     * baseline-aligned or not.</p>
     *
     * @param baselineAligned true to align widgets on their baseline,
     *         false otherwise
     *
     * @attr ref android.R.styleable#LinearLayout_baselineAligned
     */
    virtual CARAPI SetBaselineAligned(
        /* [in] */ Boolean baselineAligned);

    /**
     * When true, all children with a weight will be considered having
     * the minimum size of the largest child. If false, all children are
     * measured normally.
     *
     * @return True to measure children with a weight using the minimum
     *         size of the largest child, false otherwise.
     *
     * @attr ref android.R.styleable#LinearLayout_measureWithLargestChild
     */
    virtual CARAPI IsMeasureWithLargestChildEnabled(
        /* [out] */ Boolean* enabled);

    /**
     * When set to true, all children with a weight will be considered having
     * the minimum size of the largest child. If false, all children are
     * measured normally.
     *
     * Disabled by default.
     *
     * @param enabled True to measure children with a weight using the
     *        minimum size of the largest child, false otherwise.
     *
     * @attr ref android.R.styleable#LinearLayout_measureWithLargestChild
     */
    virtual CARAPI SetMeasureWithLargestChildEnabled(
        /* [in] */ Boolean enabled);

    virtual CARAPI GetBaseline(
        /* [out] */ Int32* baseline);

    /**
     * @return The index of the child that will be used if this layout is
     *   part of a larger layout that is baseline aligned, or -1 if none has
     *   been set.
     */
    virtual CARAPI GetBaselineAlignedChildIndex(
        /* [out] */ Int32* index);

    /**
     * @param i The index of the child that will be used if this layout is
     *          part of a larger layout that is baseline aligned.
     *
     * @attr ref android.R.styleable#LinearLayout_baselineAlignedChildIndex
     */
    virtual CARAPI SetBaselineAlignedChildIndex(
        /* [in] */ Int32 i);

    /**
     * Returns the desired weights sum.
     *
     * @return A number greater than 0.0f if the weight sum is defined, or
     *         a number lower than or equals to 0.0f if not weight sum is
     *         to be used.
     */
    virtual CARAPI GetWeightSum(
        /* [out] */ Float* weightSum);

    /**
     * Defines the desired weights sum. If unspecified the weights sum is computed
     * at layout time by adding the layout_weight of each child.
     *
     * This can be used for instance to give a single child 50% of the total
     * available space by giving it a layout_weight of 0.5 and setting the
     * weightSum to 1.0.
     *
     * @param weightSum a number greater than 0.0f, or a number lower than or equals
     *        to 0.0f if the weight sum should be computed from the children's
     *        layout_weight
     */
    virtual CARAPI SetWeightSum(
        /* [in] */ Float weightSum);

    /**
     * Should the layout be a column or a row.
     * @param orientation Pass HORIZONTAL or VERTICAL. Default
     * value is HORIZONTAL.
     *
     * @attr ref android.R.styleable#LinearLayout_orientation
     */
    virtual CARAPI SetOrientation(
        /* [in] */ Int32 orientation);

    /**
     * Returns the current orientation.
     *
     * @return either {@link #HORIZONTAL} or {@link #VERTICAL}
     */
    virtual CARAPI GetOrientation(
        /* [out] */ Int32* orientation);

    /**
     * Describes how the child views are positioned. Defaults to GRAVITY_TOP. If
     * this layout has a VERTICAL orientation, this controls where all the child
     * views are placed if there is extra vertical space. If this layout has a
     * HORIZONTAL orientation, this controls the alignment of the children.
     *
     * @param gravity See {@link android.view.Gravity}
     *
     * @attr ref android.R.styleable#LinearLayout_gravity
     */
    virtual CARAPI SetGravity(
        /* [in] */ Int32 gravity);

    virtual CARAPI SetHorizontalGravity(
        /* [in] */ Int32 horizontalGravity);

    virtual CARAPI SetVerticalGravity(
        /* [in] */ Int32 verticalGravity);

    virtual CARAPI GenerateLayoutParams(
        /* [in] */ IAttributeSet* attrs,
        /* [out] */ IViewGroupLayoutParams** params);

    virtual CARAPI OnInitializeAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    virtual CARAPI OnInitializeAccessibilityNodeInfo(
        /* [in] */ IAccessibilityNodeInfo* info);

    /**
     * <p>Returns the view at the specified index. This method can be overriden
     * to take into account virtual children. Refer to
     * {@link android.widget.TableLayout} and {@link android.widget.TableRow}
     * for an example.</p>
     *
     * @param index the child's index
     * @return the child at the specified index
     */
    virtual CARAPI GetVirtualChildAt(
        /* [in] */ Int32 index,
        /* [out] */ IView** view);

    /**
     * <p>Returns the virtual number of children. This number might be different
     * than the actual number of children if the layout can hold virtual
     * children. Refer to
     * {@link android.widget.TableLayout} and {@link android.widget.TableRow}
     * for an example.</p>
     *
     * @return the virtual number of children
     */
    virtual CARAPI GetVirtualChildCount(
        /* [out] */ Int32* count);

    /**
     * <p>Returns the number of children to skip after measuring/laying out
     * the specified child.</p>
     *
     * @param child the child after which we want to skip children
     * @param index the index of the child after which we want to skip children
     * @return the number of children to skip, 0 by default
     */
    virtual CARAPI_(Int32) GetChildrenSkipCount(
        /* [in] */ IView* child,
        /* [in] */ Int32 index);

    CARAPI MeasureVertical(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    CARAPI MeasureHorizontal(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    /**
     * <p>Returns the size (width or height) that should be occupied by a null
     * child.</p>
     *
     * @param childIndex the index of the null child
     * @return the width or height of the child depending on the orientation
     */
    virtual CARAPI_(Int32) MeasureNullChild(
        /* [in] */ Int32 childIndex);

    /**
     * <p>Measure the child according to the parent's measure specs. This
     * method should be overriden by subclasses to force the sizing of
     * children. This method is called by {@link #measureVertical(int, int)} and
     * {@link #measureHorizontal(int, int)}.</p>
     *
     * @param child the child to measure
     * @param childIndex the index of the child in this view
     * @param widthMeasureSpec horizontal space requirements as imposed by the parent
     * @param totalWidth extra space that has been used up by the parent horizontally
     * @param heightMeasureSpec vertical space requirements as imposed by the parent
     * @param totalHeight extra space that has been used up by the parent vertically
     */
    virtual CARAPI_(void) MeasureChildBeforeLayout(
        /* [in] */ IView* child,
        /* [in] */ Int32 childIndex,
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 totalWidth,
        /* [in] */ Int32 heightMeasureSpec,
        /* [in] */ Int32 totalHeight);

    /**
     * <p>Return the location offset of the specified child. This can be used
     * by subclasses to change the location of a given widget.</p>
     *
     * @param child the child for which to obtain the location offset
     * @return the location offset in pixels
     */
    virtual CARAPI_(Int32) GetLocationOffset(
        /* [in] */ IView* child);

    /**
     * <p>Return the size offset of the next sibling of the specified child.
     * This can be used by subclasses to change the location of the widget
     * following <code>child</code>.</p>
     *
     * @param child the child whose next sibling will be moved
     * @return the location offset of the next child in pixels
     */
    virtual CARAPI_(Int32) GetNextLocationOffset(
        /* [in] */ IView* child);

    /**
     * Position the children during a layout pass if the orientation of this
     * LinearLayout is set to {@link #VERTICAL}.
     *
     * @see #getOrientation()
     * @see #setOrientation(int)
     * @see #onLayout(boolean, int, int, int, int)
     */
    CARAPI_(void) LayoutVertical(
        /* [in] */ Int32 l,
        /* [in] */ Int32 t,
        /* [in] */ Int32 r,
        /* [in] */ Int32 b);

    /**
     * Position the children during a layout pass if the orientation of this
     * LinearLayout is set to {@link #HORIZONTAL}.
     *
     * @see #getOrientation()
     * @see #setOrientation(int)
     * @see #onLayout(boolean, int, int, int, int)
     */
    CARAPI_(void) LayoutHorizontal(
        /* [in] */ Int32 l,
        /* [in] */ Int32 t,
        /* [in] */ Int32 r,
        /* [in] */ Int32 b);

    using ViewGroup::GetChildCount;

protected:
    virtual CARAPI_(void) OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    CARAPI_(Boolean) HasDividerBeforeChildAt(
        /* [in] */ Int32 childIndex);

    virtual CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 l,
        /* [in] */ Int32 t,
        /* [in] */ Int32 r,
        /* [in] */ Int32 b);

    /**
     * Returns a set of layout parameters with a width of
     * {@link android.view.ViewGroup.LayoutParams#MATCH_PARENT}
     * and a height of {@link android.view.ViewGroup.LayoutParams#WRAP_CONTENT}
     * when the layout's orientation is {@link #VERTICAL}. When the orientation is
     * {@link #HORIZONTAL}, the width is set to {@link LayoutParams#WRAP_CONTENT}
     * and the height to {@link LayoutParams#WRAP_CONTENT}.
     */
    virtual CARAPI GenerateDefaultLayoutParams(
        /* [out] */ IViewGroupLayoutParams** lp);

    virtual CARAPI_(AutoPtr<IViewGroupLayoutParams>) GenerateLayoutParams(
        /* [in] */ IViewGroupLayoutParams* p);

    virtual CARAPI_(Boolean) CheckLayoutParams(
        /* [in] */ IViewGroupLayoutParams* p);

    CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas);

private:
    CARAPI_(void) DrawDividersVertical(
        /* [in] */ ICanvas* canvas);

    CARAPI_(void) DrawDividersHorizontal(
        /* [in] */ ICanvas* canvas);

    CARAPI_(void) DrawHorizontalDivider(
        /* [in] */ ICanvas* canvas,
        /* [in] */ Int32 top);

    CARAPI_(void) DrawVerticalDivider(
        /* [in] */ ICanvas* canvas,
        /* [in] */ Int32 left);

    CARAPI_(void) ForceUniformWidth(
        /* [in] */ Int32 count,
        /* [in] */ Int32 heightMeasureSpec);

    CARAPI_(void) ForceUniformHeight(
        /* [in] */ Int32 count,
        /* [in] */ Int32 widthMeasureSpec);

    CARAPI_(void) SetChildFrame(
        /* [in] */ IView* child,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    CARAPI InitFromAttributes(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

    CARAPI_(AutoPtr<IView>) GetVirtualChildAt(
        /* [in] */ Int32 index);

    CARAPI_(Int32) GetChildCount();

private:
    /**
     * Whether the children of this layout are baseline aligned.  Only applicable
     * if {@link #mOrientation} is horizontal.
     */
    Boolean mBaselineAligned;

    /**
     * If this layout is part of another layout that is baseline aligned,
     * use the child at this index as the baseline.
     *
     * Note: this is orthogonal to {@link #mBaselineAligned}, which is concerned
     * with whether the children of this layout are baseline aligned.
     */
    Int32 mBaselineAlignedChildIndex;

    /**
     * The additional offset to the child's baseline.
     * We'll calculate the baseline of this layout as we measure vertically; for
     * horizontal linear layouts, the offset of 0 is appropriate.
     */
    Int32 mBaselineChildTop;

    Int32 mOrientation;

    Int32 mGravity;

    Int32 mTotalLength;

    Float mWeightSum;

    Boolean mUseLargestChild;

    AutoPtr<ArrayOf<Int32> > mMaxAscent;

    AutoPtr<ArrayOf<Int32> > mMaxDescent;

    ECO_LOCAL static const Int32 VERTICAL_GRAVITY_COUNT;
    ECO_LOCAL static const Int32 INDEX_CENTER_VERTICAL;
    ECO_LOCAL static const Int32 INDEX_TOP;
    ECO_LOCAL static const Int32 INDEX_BOTTOM;
    ECO_LOCAL static const Int32 INDEX_FILL;

    AutoPtr<IDrawable> mDivider;
    Int32 mDividerWidth;
    Int32 mDividerHeight;
    Int32 mShowDividers;
    Int32 mDividerPadding;
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_LINEARLAYOUT_H__

