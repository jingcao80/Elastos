
#ifndef __ELASTOS_DROID_GRAPHICS_DRAWABLE_RIPPLEDRAWABLE_H__
#define __ELASTOS_DROID_GRAPHICS_DRAWABLE_RIPPLEDRAWABLE_H__

#include "elastos/droid/graphics/drawable/LayerDrawable.h"
#include <Elastos.Droid.Utility.h>

using Elastos::Droid::Utility::IDisplayMetrics;

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

class RippleDrawable
    : public LayerDrawable
    , public IRippleDrawable
{
protected:
    class RippleState
        : public LayerState
        , public IRippleState
    {
    public:
        CAR_INTERFACE_DECL()

        RippleState(
            /* [in] */ LayerState* orig,
            /* [in] */ IRippleDrawable* owner,
            /* [in] */ IResources* res);

        // @Override
        CARAPI CanApplyTheme(
            /* [out] */ Boolean* can);

        // @Override
        CARAPI NewDrawable(
            /* [out] */ IDrawable** drawable);

        // @Override
        CARAPI NewDrawable(
            /* [in] */ IResources* res,
            /* [out] */ IDrawable** drawable);

        // @Override
        CARAPI NewDrawable(
            /* [in] */ IResources* res,
            /* [in] */ IResourcesTheme* theme,
            /* [out] */ IDrawable** drawable);

    public:
        AutoPtr<ArrayOf<Int32> > mTouchThemeAttrs;
        AutoPtr<IColorStateList> mColor;
        Int32 mMaxRadius;
    };

public:
    CAR_INTERFACE_DECL()

    RippleDrawable();

    /**
     * Constructor used for drawable inflation.
     */
    CARAPI constructor();

    /**
     * Creates a new ripple drawable with the specified ripple color and
     * optional content and mask drawables.
     *
     * @param color The ripple color
     * @param content The content drawable, may be {@code NULL}
     * @param mask The mask drawable, may be {@code NULL}
     */
    CARAPI constructor(
        /* [in] */ /*@NonNull*/ IColorStateList* color,
        /* [in] */ /*@Nullable*/ IDrawable* content,
        /* [in] */ /*@Nullable*/ IDrawable* mask);

    CARAPI constructor(
        /* [in] */ IDrawableConstantState* state,
        /* [in] */ IResources* res,
        /* [in] */ IResourcesTheme* theme);

    // @Override
    CARAPI JumpToCurrentState();

    // @Override
    CARAPI SetAlpha(
        /* [in] */ Int32 alpha);

    // @Override
    CARAPI SetColorFilter(
        /* [in] */ IColorFilter* cf);

    // @Override
    CARAPI GetOpacity(
        /* [out] */ Int32* opacity);

    // @Override
    CARAPI SetVisible(
        /* [in] */ Boolean visible,
        /* [in] */ Boolean restart,
        /* [out] */ Boolean* result);

    /**
     * @hide
     */
    // @Override
    CARAPI IsProjected(
        /* [out] */ Boolean* projected);

    // @Override
    CARAPI IsStateful(
        /* [out] */ Boolean* isStateful);

    CARAPI SetColor(
        /* [in] */ IColorStateList* color);

    // @Override
    CARAPI Inflate(
        /* [in] */ IResources* r,
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ IResourcesTheme* theme) /*throws XmlPullParserException, IOException*/;

    // @Override
    CARAPI SetDrawableByLayerId(
        /* [in] */ Int32 id,
        /* [in] */ IDrawable* drawable,
        /* [out] */ Boolean* result);

    /**
     * Specifies how layer padding should affect the bounds of subsequent
     * layers. The default and recommended value for RippleDrawable is
     * {@link #PADDING_MODE_STACK}.
     *
     * @param mode padding mode, one of:
     *            <ul>
     *            <li>{@link #PADDING_MODE_NEST} to nest each layer inside the
     *            padding of the previous layer
     *            <li>{@link #PADDING_MODE_STACK} to stack each layer directly
     *            atop the previous layer
     *            </ul>
     * @see #getPaddingMode()
     */
    // @Override
    CARAPI SetPaddingMode(
        /* [in] */ Int32 mode);

    // @Override
    CARAPI ApplyTheme(
        /* [in] */ IResourcesTheme* t);

    // @Override
    CARAPI CanApplyTheme(
        /* [out] */ Boolean* can);

    // @Override
    CARAPI SetHotspot(
        /* [in] */ Float x,
        /* [in] */ Float y);

    // @Override
    CARAPI SetHotspotBounds(
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    /** @hide */
    // @Override
    CARAPI GetHotspotBounds(
        /* [in] */ IRect* outRect);

    /**
     * Populates <code>outline</code> with the first available layer outline,
     * excluding the mask layer.
     *
     * @param outline Outline in which to place the first available layer outline
     */
    // @Override
    CARAPI GetOutline(
        /* [in] */ /*@NonNull*/ IOutline* outline);

    // @Override
    CARAPI Draw(
        /* [in] */ /*@NonNull*/ ICanvas* canvas);

    /**
     * Removes a ripple from the exiting ripple list.
     *
     * @param ripple the ripple to remove
     */
    CARAPI RemoveRipple(
        /* [in] */ IRipple* ripple);

    // @Override
    CARAPI GetDirtyBounds(
        /* [out] */ IRect** dirty);

    // @Override
    CARAPI GetConstantState(
        /* [out] */ IDrawableConstantState** state);

    // @Override
    CARAPI Mutate();

    /**
     * Sets the maximum ripple radius in pixels. The default value of
     * {@link #RADIUS_AUTO} defines the radius as the distance from the center
     * of the drawable bounds (or hotspot bounds, if specified) to a corner.
     *
     * @param maxRadius the maximum ripple radius in pixels or
     *            {@link #RADIUS_AUTO} to automatically determine the maximum
     *            radius based on the bounds
     * @see #getMaxRadius()
     * @see #setHotspotBounds(Int32, Int32, Int32, Int32)
     * @hide
     */
    CARAPI SetMaxRadius(
        /* [in] */ Int32 maxRadius);

    /**
     * @return the maximum ripple radius in pixels, or {@link #RADIUS_AUTO} if
     *         the radius is determined automatically
     * @see #setMaxRadius(Int32)
     * @hide
     */
    CARAPI GetMaxRadius(
        /* [out] */ Int32* radius);

protected:
    // @Override
    CARAPI_(AutoPtr<LayerState>) CreateConstantState(
        /* [in] */ LayerState* state,
        /* [in] */ IResources* res);

    // @Override
    CARAPI_(Boolean) OnStateChange(
        /* [in] */ ArrayOf<Int32>* stateSet);

    // @Override
    CARAPI_(void) OnBoundsChange(
        /* [in] */ IRect* bounds);

    CARAPI_(void) OnLastStrongRef(const void* id);

private:
    CARAPI_(Boolean) CancelExitingRipples();

    CARAPI_(void) SetRippleActive(
        /* [in] */ Boolean active);

    CARAPI_(void) SetBackgroundActive(
        /* [in] */ Boolean active);

    /**
     * Initializes the constant state from the values in the typed array.
     */
    CARAPI UpdateStateFromTypedArray(
        /* [in] */ ITypedArray* a) /*throws XmlPullParserException*/;

    CARAPI VerifyRequiredAttributes(
        /* [in] */ ITypedArray* a) /*throws XmlPullParserException*/;

    /**
     * Set the density at which this drawable will be rendered.
     *
     * @param metrics The display metrics for this drawable.
     */
    CARAPI_(void) SetTargetDensity(
        /* [in] */ IDisplayMetrics* metrics);

    /**
     * Creates an active hotspot at the specified location.
     */
    CARAPI_(void) TryBackgroundEnter();

    CARAPI_(void) TryBackgroundExit();

    /**
     * Attempts to start an enter animation for the active hotspot. Fails if
     * there are too many animating ripples.
     */
    CARAPI_(void) TryRippleEnter();

    /**
     * Attempts to start an exit animation for the active hotspot. Fails if
     * there is no active hotspot.
     */
    CARAPI_(void) TryRippleExit();

    /**
     * Cancels and removes the active ripple, all exiting ripples, and the
     * background. Nothing will be drawn after this method is called.
     */
    CARAPI_(void) ClearHotspots();

    /**
     * Notifies all the animating ripples that the hotspot bounds have changed.
     */
    CARAPI_(void) OnHotspotBoundsChanged();

    CARAPI_(Int32) GetRippleIndex(
        /* [in] */ IRipple* ripple);

    CARAPI_(Int32) DrawContentLayer(
        /* [in] */ ICanvas* canvas,
        /* [in] */ IRect* bounds,
        /* [in] */ IPorterDuffXfermode* mode);

    CARAPI_(Int32) DrawBackgroundLayer(
        /* [in] */ ICanvas* canvas,
        /* [in] */ IRect* bounds,
        /* [in] */ IPorterDuffXfermode* mode,
        /* [in] */ Boolean drawMask);

    CARAPI_(Int32) DrawRippleLayer(
        /* [in] */ ICanvas* canvas,
        /* [in] */ IRect* bounds,
        /* [in] */ IPorterDuffXfermode* mode);

    CARAPI_(Int32) DrawMaskingLayer(
        /* [in] */ ICanvas* canvas,
        /* [in] */ IRect* bounds,
        /* [in] */ IPorterDuffXfermode* mode);

    CARAPI_(AutoPtr<IPaint>) GetRipplePaint();

    CARAPI_(AutoPtr<IPaint>) GetMaskingPaint(
        /* [in] */ IPorterDuffXfermode* xfermode);

    CARAPI_(void) InitializeFromState();

    CARAPI Cancel();
public:
    /**
     * Constant for automatically determining the maximum ripple radius.
     *
     * @see #setMaxRadius(Int32)
     * @hide
     */
    static const Int32 RADIUS_AUTO;

private:
    static AutoPtr<IPorterDuffXfermode> DST_IN;
    static AutoPtr<IPorterDuffXfermode> SRC_ATOP;
    static AutoPtr<IPorterDuffXfermode> SRC_OVER;

    /** The maximum number of ripples supported. */
    static const Int32 MAX_RIPPLES;

    AutoPtr<IRect> mTempRect;

    /** Current ripple effect bounds, used to constrain ripple effects. */
    AutoPtr<IRect> mHotspotBounds;

    /** Current drawing bounds, used to compute dirty region. */
    AutoPtr<IRect> mDrawingBounds;

    /** Current dirty bounds, union of current and previous drawing bounds. */
    AutoPtr<IRect> mDirtyBounds;

    /** Mirrors mLayerState with some extra information. */
    AutoPtr<RippleState> mState;

    /** The masking layer, e.g. the layer with id R.id.mask. */
    AutoPtr<IDrawable> mMask;

    /** The current background. May be actively animating or pending entry. */
    AutoPtr<IRippleBackground> mBackground;

    /** Whether we expect to draw a background when visible. */
    Boolean mBackgroundActive;

    /** The current ripple. May be actively animating or pending entry. */
    AutoPtr<IRipple> mRipple;

    /** Whether we expect to draw a ripple when visible. */
    Boolean mRippleActive;

    // Hotspot coordinates that are awaiting activation.
    Float mPendingX;
    Float mPendingY;
    Boolean mHasPending;

    /**
     * Lazily-created array of actively animating ripples. Inactive ripples are
     * pruned during draw(). The locations of these will not change.
     */
    AutoPtr<ArrayOf<IRipple*> > mExitingRipples;
    Int32 mExitingRipplesCount;

    /** Paint used to control appearance of ripples. */
    AutoPtr<IPaint> mRipplePaint;

    /** Paint used to control reveal layer masking. */
    AutoPtr<IPaint> mMaskingPaint;

    /** Target density of the display into which ripples are drawn. */
    Float mDensity;

    /** Whether bounds are being overridden. */
    Boolean mOverrideBounds;

    /**
     * Whether the next draw MUST draw something to canvas. Used to work around
     * a bug in hardware invalidation following a render thread-accelerated
     * animation.
     */
    Boolean mNeedsDraw;
};

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos

#endif  // __ELASTOS_DROID_GRAPHICS_DRAWABLE_RIPPLEDRAWABLE_H__
