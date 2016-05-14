
#ifndef __ELASTOS_DROID_WIDGET_FRAMELAYOUT_H__
#define __ELASTOS_DROID_WIDGET_FRAMELAYOUT_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Widget.h"
#include <elastos/droid/view/ViewGroup.h>

using Elastos::Droid::View::ViewGroup;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::IViewGroupMarginLayoutParams;

namespace Elastos {
namespace Droid {
namespace Widget {

class ECO_PUBLIC FrameLayout
    : public ViewGroup
    , public IFrameLayout
{
public:
    class FrameLayoutLayoutParams
        : public ViewGroup::MarginLayoutParams
        , public IFrameLayoutLayoutParams
    {
    public:
        CAR_INTERFACE_DECL();

        FrameLayoutLayoutParams();

        CARAPI constructor(
            /* [in] */ IContext* c,
            /* [in] */ IAttributeSet* attrs);

        /**
         * {@inheritDoc}
         */
        CARAPI constructor(
            /* [in] */ Int32 width,
            /* [in] */ Int32 height);

        /**
         * Creates a new set of layout parameters with the specified width, height
         * and weight.
         *
         * @param width the width, either {@link #MATCH_PARENT},
         *        {@link #WRAP_CONTENT} or a fixed size in pixels
         * @param height the height, either {@link #MATCH_PARENT},
         *        {@link #WRAP_CONTENT} or a fixed size in pixels
         * @param gravity the gravity
         *
         * @see android.view.Gravity
         */
        CARAPI constructor(
            /* [in] */ Int32 width,
            /* [in] */ Int32 height,
            /* [in] */ Int32 gravity);

        /**
         * {@inheritDoc}
         */
        CARAPI constructor(
            /* [in] */ IViewGroupLayoutParams* source);

        /**
         * {@inheritDoc}
         */
        CARAPI constructor(
            /* [in] */ IViewGroupMarginLayoutParams* source);

        /**
         * Copy constructor. Clones the width, height, margin values, and
         * gravity of the source.
         *
         * @param source The layout params to copy from.
         */
        CARAPI constructor(
            /* [in] */ IFrameLayoutLayoutParams* source);

        CARAPI SetGravity(
            /* [in] */ Int32 gravity);

        CARAPI GetGravity(
            /* [out] */ Int32* gravity);

    public:
        /**
         * Gravity for the view associated with these LayoutParams.
         *
         * @see android.view.Gravity
         */
        Int32 mGravity;
    };

public:
    CAR_INTERFACE_DECL();

    FrameLayout();

    virtual CARAPI GetForegroundGravity(
        /* [out] */ Int32* foregroundGravity);

    virtual CARAPI SetForegroundGravity(
        /* [in] */ Int32 foregroundGravity);

    virtual CARAPI SetForeground(
        /* [in] */ IDrawable* drawable);

    virtual CARAPI GetForeground(
        /* [out] */ IDrawable** foreground);

    virtual CARAPI GetMeasureAllChildren(
        /* [out] */ Boolean* measureAll);

    virtual CARAPI SetMeasureAllChildren(
        /* [in] */ Boolean measureAll);

    virtual CARAPI GetConsiderGoneChildrenWhenMeasuring(
        /* [out] */ Boolean* consider);

    virtual CARAPI JumpDrawablesToCurrentState();

    virtual CARAPI Draw(
        /* [in] */ ICanvas* canvas);

    virtual CARAPI GatherTransparentRegion(
        /* [in] */ IRegion* region,
        /* [out] */ Boolean* result);

    virtual CARAPI GenerateLayoutParams(
        /* [in] */ IAttributeSet* attrs,
        /* [out] */ IViewGroupLayoutParams** params);

    virtual CARAPI ShouldDelayChildPressedState(
        /* [out] */ Boolean* compatibility);

    virtual CARAPI OnInitializeAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    virtual CARAPI OnInitializeAccessibilityNodeInfo(
        /* [in] */ IAccessibilityNodeInfo* info);

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

    // @Override
    // @RemotableViewMethod
    CARAPI SetVisibility(
        /* [in] */ /*@Visibility*/ Int32 visibility);

    // @Override
    CARAPI DrawableHotspotChanged(
        /* [in] */ Float x,
        /* [in] */ Float y);

    /**
     * Applies a tint to the foreground drawable. Does not modify the current
     * tint mode, which is {@link PorterDuff.Mode#SRC_IN} by default.
     * <p>
     * Subsequent calls to {@link #setForeground(Drawable)} will automatically
     * mutate the drawable and apply the specified tint and tint mode using
     * {@link Drawable#setTintList(ColorStateList)}.
     *
     * @param tint the tint to apply, may be {@code null} to clear tint
     *
     * @attr ref android.R.styleable#FrameLayout_foregroundTint
     * @see #getForegroundTintList()
     * @see Drawable#setTintList(ColorStateList)
     */
    CARAPI SetForegroundTintList(
        /* [in] */ /*@Nullable*/ IColorStateList* tint);

    /**
     * @return the tint applied to the foreground drawable
     * @attr ref android.R.styleable#FrameLayout_foregroundTint
     * @see #setForegroundTintList(ColorStateList)
     */
    // @Nullable
    CARAPI GetForegroundTintList(
        /* [out] */ IColorStateList** csl);

    /**
     * Specifies the blending mode used to apply the tint specified by
     * {@link #setForegroundTintList(ColorStateList)}} to the foreground drawable.
     * The default mode is {@link PorterDuff.Mode#SRC_IN}.
     *
     * @param tintMode the blending mode used to apply the tint, may be
     *                 {@code null} to clear tint
     * @attr ref android.R.styleable#FrameLayout_foregroundTintMode
     * @see #getForegroundTintMode()
     * @see Drawable#setTintMode(PorterDuff.Mode)
     */
    CARAPI SetForegroundTintMode(
        /* [in] */ /*@Nullable*/ PorterDuffMode tintMode);

    /**
     * @return the blending mode used to apply the tint to the foreground
     *         drawable
     * @attr ref android.R.styleable#FrameLayout_foregroundTintMode
     * @see #setForegroundTintMode(PorterDuff.Mode)
     */
    // @Nullable
    CARAPI GetForegroundTintMode(
        /* [out] */ PorterDuffMode* mode);

protected:
    virtual CARAPI_(Boolean) VerifyDrawable(
        /* [in] */ IDrawable* who);

    virtual CARAPI DrawableStateChanged();

    virtual CARAPI GenerateDefaultLayoutParams(
        /* [out] */ IViewGroupLayoutParams** params);

    virtual CARAPI_(void) OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    virtual CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    virtual CARAPI_(void) LayoutChildren(
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom,
        /* [in] */ Boolean forceLeftGravity);

    virtual CARAPI_(void) OnSizeChanged(
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ Int32 oldw,
        /* [in] */ Int32 oldh);

    virtual CARAPI_(Boolean) CheckLayoutParams(
        /* [in] */ IViewGroupLayoutParams* p);

    virtual CARAPI_(AutoPtr<IViewGroupLayoutParams>) GenerateLayoutParams(
        /* [int] */ IViewGroupLayoutParams* p);

    virtual CARAPI_(Int32) GetPaddingLeftWithForeground();

    virtual CARAPI_(Int32) GetPaddingRightWithForeground();

private:
    CARAPI_(Int32) GetPaddingTopWithForeground();

    CARAPI_(Int32) GetPaddingBottomWithForeground();

    CARAPI ApplyForegroundTint();

protected:
    Boolean mForegroundInPadding;

private:
    static Int32 DEFAULT_CHILD_GRAVITY; // Gravity.TOP | Gravity.START;
    Boolean mMeasureAllChildren;
    AutoPtr<IDrawable> mForeground;
    AutoPtr<IColorStateList> mForegroundTintList;
    PorterDuffMode mForegroundTintMode;
    Boolean mHasForegroundTint;
    Boolean mHasForegroundTintMode;
    Int32 mForegroundPaddingLeft;
    Int32 mForegroundPaddingTop;
    Int32 mForegroundPaddingRight;
    Int32 mForegroundPaddingBottom;
    AutoPtr<IRect> mSelfBounds;
    AutoPtr<IRect> mOverlayBounds;
    Int32 mForegroundGravity;
    Boolean mForegroundBoundsChanged;

    List<AutoPtr<IView> > mMatchParentChildren;
};

}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_FRAMELAYOUT_H__
