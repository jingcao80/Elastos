
#ifndef __ELASTOS_DROID_GRAPHICS_DRAWABLE_LAYERDRAWABLE_H__
#define __ELASTOS_DROID_GRAPHICS_DRAWABLE_LAYERDRAWABLE_H__

#include "elastos/droid/graphics/drawable/Drawable.h"

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

class LayerDrawable;

class ChildDrawable : public Object
{
public:
    ChildDrawable();

    ChildDrawable(
        /* [in] */ ChildDrawable* orig,
        /* [in] */ LayerDrawable* owner,
        /* [in] */ IResources* res);

    AutoPtr<IDrawable> mDrawable;
    AutoPtr<ArrayOf<Int32> > mThemeAttrs;
    Int32 mInsetL;
    Int32 mInsetT;
    Int32 mInsetR;
    Int32 mInsetB;
    Int32 mId;
};

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos

DEFINE_CONVERSION_FOR(Elastos::Droid::Graphics::Drawable::ChildDrawable, IInterface);

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

class LayerDrawable
    : public Drawable
    , public ILayerDrawable
    , public IDrawableCallback
{
public:
    class LayerState
        : public Drawable::ConstantState
    {
    public:
        LayerState(
            /* [in] */ LayerState* orig,
            /* [in] */ LayerDrawable* owner,
            /* [in] */ IResources* res);

        virtual ~LayerState();

        CARAPI CanApplyTheme(
            /* [out] */ Boolean* can);

        CARAPI NewDrawable(
            /* [out] */ IDrawable** drawable);

        CARAPI NewDrawable(
            /* [in] */ IResources* res,
            /* [out] */ IDrawable** drawable);

        // @Override
        CARAPI NewDrawable(
            /* [in] */ IResources* res,
            /* [in] */ IResourcesTheme* theme,
            /* [out] */ IDrawable** drawable);

        CARAPI GetChangingConfigurations(
            /* [out] */ Int32* config);

        CARAPI_(Int32) GetOpacity();

        CARAPI_(Boolean) IsStateful();

        CARAPI_(Boolean) CanConstantState();

        CARAPI_(void) InvalidateCache();

    public:
        Int32 mNum;
        AutoPtr< ArrayOf<ChildDrawable*> > mChildren;
        AutoPtr<ArrayOf<Int32> > mThemeAttrs;

        Int32 mChangingConfigurations;
        Int32 mChildrenChangingConfigurations;

    private:
        Boolean mHaveOpacity;
        Int32 mOpacity;

        Boolean mHaveIsStateful;
        Boolean mIsStateful;
        Boolean mAutoMirrored;

        Int32 mPaddingMode;
        friend class LayerDrawable;
    };

public:
    CAR_INTERFACE_DECL()

    LayerDrawable();

    virtual ~LayerDrawable();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ ArrayOf<IDrawable*>* layers);

    /**
     * Create a new layer drawable with the specified list of layers and the specified
     * constant state.
     *
     * @param layers The list of layers to add to this drawable.
     * @param state The constant drawable state.
     */
    CARAPI constructor(
        /* [in] */ ArrayOf<IDrawable*>* layers,
        /* [in] */ IDrawableConstantState* state);

    CARAPI constructor(
        /* [in] */ IDrawableConstantState* state,
        /* [in] */ IResources* res,
        /* [in] */ IResourcesTheme* theme);

    CARAPI Inflate(
        /* [in] */ IResources* r,
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ IResourcesTheme* theme);

    // @Override
    CARAPI ApplyTheme(
        /* [in] */ IResourcesTheme* t);

    // @Override
    CARAPI CanApplyTheme(
        /* [out] */ Boolean* can);

    /**
     * @hide
     */
    // @Override
    CARAPI IsProjected(
        /* [out] */ Boolean* projected);

    /**
     * Looks for a layer with the given ID and returns its {@link Drawable}.
     * <p>
     * If multiple layers are found for the given ID, returns the
     * {@link Drawable} for the matching layer at the highest index.
     *
     * @param id The layer ID to search for.
     * @return The {@link Drawable} for the highest-indexed layer that has the
     *         given ID, or null if not found.
     */
    virtual CARAPI FindDrawableByLayerId(
        /* [in] */ Int32 id,
        /* [out] */ IDrawable** drawable);

    /**
     * Sets the ID of a layer->
     *
     * @param index The index of the layer which will received the ID.
     * @param id The ID to assign to the layer->
     */
    virtual CARAPI SetId(
        /* [in] */ Int32 index,
        /* [in] */ Int32 id);

    /**
     * Returns the number of layers contained within this.
     * @return The number of layers.
     */
    virtual CARAPI GetNumberOfLayers(
        /* [out] */ Int32* number);

    /**
     * Returns the drawable at the specified layer index.
     *
     * @param index The layer index of the drawable to retrieve.
     *
     * @return The {@link android.graphics.drawable.Drawable} at the specified layer index.
     */
    virtual CARAPI GetDrawable(
        /* [in] */ Int32 index,
        /* [out] */ IDrawable** drawable);

    /**
     * Returns the id of the specified layer.
     *
     * @param index The index of the layer.
     *
     * @return The id of the layer or {@link android.view.View#NO_ID} if the layer has no id.
     */
    virtual CARAPI GetId(
        /* [in] */ Int32 index,
        /* [out] */ Int32* id);

    /**
     * Sets (or replaces) the {@link Drawable} for the layer with the given id.
     *
     * @param id The layer ID to search for->
     * @param drawable The replacement {@link Drawable}.
     * @return Whether the {@link Drawable} was replaced (could return FALSE if
     *         the id was not found).
     */
    virtual CARAPI SetDrawableByLayerId(
        /* [in] */ Int32 id,
        /* [in] */ IDrawable* drawable,
        /* [out] */ Boolean* res);

    /**
     * Specifies the insets in pixels for the drawable at the specified index.
     *
     * @param index the index of the drawable to adjust
     * @param l number of pixels to add to the left bound
     * @param t number of pixels to add to the top bound
     * @param r number of pixels to subtract from the right bound
     * @param b number of pixels to subtract from the bottom bound
     */
    virtual CARAPI SetLayerInset(
        /* [in] */ Int32 index,
        /* [in] */ Int32 l,
        /* [in] */ Int32 t,
        /* [in] */ Int32 r,
        /* [in] */ Int32 b);

    /**
     * Specifies how layer padding should affect the bounds of subsequent
     * layers. The default value is {@link #PADDING_MODE_NEST}.
     *
     * @param mode padding mode, one of:
     *            <ul>
     *            <li>{@link #PADDING_MODE_NEST} to nest each layer inside the
     *            padding of the previous layer
     *            <li>{@link #PADDING_MODE_STACK} to stack each layer directly
     *            atop the previous layer
     *            </ul>
     *
     * @see #getPaddingMode()
     * @attr ref android.R.styleable#LayerDrawable_paddingMode
     */
    virtual CARAPI SetPaddingMode(
        /* [in] */ Int32 mode);

    /**
     * @return the current padding mode
     *
     * @see #setPaddingMode(int)
     * @attr ref android.R.styleable#LayerDrawable_paddingMode
     */
    virtual CARAPI GetPaddingMode(
        /* [out] */ Int32* mode);

    //@Override
    CARAPI InvalidateDrawable(
        /* [in] */ IDrawable* who);

    //@Override
    CARAPI ScheduleDrawable(
        /* [in] */ IDrawable* who,
        /* [in] */ IRunnable* what,
        /* [in] */ Int64 when);

    //@Override
    CARAPI UnscheduleDrawable(
        /* [in] */ IDrawable* who,
        /* [in] */ IRunnable* what);

    //@Override
    CARAPI Draw(
        /* [in] */ ICanvas* canvas);

    //@Override
    CARAPI GetChangingConfigurations(
        /* [out] */ Int32* configs);

    //@Override
    CARAPI GetPadding(
        /* [in] */ IRect* padding,
        /* [out] */ Boolean* isPadding);

    //@Override
    CARAPI SetVisible(
        /* [in] */ Boolean visible,
        /* [in] */ Boolean restart,
        /* [out] */ Boolean* isDifferent);

    //@Override
    CARAPI SetDither(
        /* [in] */ Boolean dither);

    //@Override
    CARAPI SetAlpha(
        /* [in] */ Int32 alpha);

    // @Override
    CARAPI GetAlpha(
        /* [out] */ Int32* alpha);

    //@Override
    CARAPI SetColorFilter(
        /* [in] */ IColorFilter* cf);

    // @Override
    CARAPI SetTintList(
        /* [in] */ IColorStateList* tint);

    // @Override
    CARAPI SetTintMode(
        /* [in] */ PorterDuffMode tintMode);

    /**
     * Sets the opacity of this drawable directly, instead of collecting the states from
     * the layers
     *
     * @param opacity The opacity to use, or {@link PixelFormat#UNKNOWN PixelFormat.UNKNOWN}
     * for the default behavior
     *
     * @see PixelFormat#UNKNOWN
     * @see PixelFormat#TRANSLUCENT
     * @see PixelFormat#TRANSPARENT
     * @see PixelFormat#OPAQUE
     */
    virtual CARAPI SetOpacity(
        /* [in] */ Int32 opacity);

    //@Override
    CARAPI GetOpacity(
        /* [out] */ Int32* opacity);

    // @Override
    CARAPI SetAutoMirrored(
        /* [in] */ Boolean mirrored);

    // @Override
    CARAPI IsAutoMirrored(
        /* [out] */ Boolean* mirrored);

    //@Override
    CARAPI IsStateful(
        /* [out] */ Boolean* isStateful);

    //@Override
    CARAPI GetIntrinsicWidth(
        /* [out] */ Int32* width);

    //@Override
    CARAPI GetIntrinsicHeight(
        /* [out] */ Int32* height);

    //@Override
    CARAPI GetConstantState(
        /* [out] */ IDrawableConstantState** state);

    //@Override
    CARAPI Mutate();

    //@Override
    CARAPI SetLayoutDirection(
        /* [in] */ Int32 layoutDirection);

    /**
     * Populates <code>outline</code> with the first available (non-empty) layer outline.
     *
     * @param outline Outline in which to place the first available layer outline
     */
    // @Override
    virtual CARAPI GetOutline(
        /* [in] */ /*@NonNull*/ IOutline* outline);

    // @Override
    virtual CARAPI SetHotspot(
        /* [in] */ Float x,
        /* [in] */ Float y);

    // @Override
    virtual CARAPI SetHotspotBounds(
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    /** @hide */
    // @Override
    virtual CARAPI GetHotspotBounds(
        /* [in] */ IRect* outRect);

protected:
    virtual CARAPI_(AutoPtr<LayerState>) CreateConstantState(
        /* [in] */ LayerState* state,
        /* [in] */ IResources* res);

    //@Override
    CARAPI_(Boolean) OnStateChange(
        /* [in] */ ArrayOf<Int32>* state);

    //@Override
    CARAPI_(Boolean) OnLevelChange(
        /* [in] */ Int32 level);

    //@Override
    CARAPI_(void) OnBoundsChange(
        /* [in] */ IRect* bounds);

protected:
    /**
     * Add a new layer to this drawable. The new layer is identified by an id.
     *
     * @param layer The drawable to add as a layer.
     * @param themeAttrs Theme attributes extracted from the layer.
     * @param id The id of the new layer.
     * @param left The left padding of the new layer.
     * @param top The top padding of the new layer.
     * @param right The right padding of the new layer.
     * @param bottom The bottom padding of the new layer.
     */
    CARAPI_(AutoPtr<ChildDrawable>) AddLayer(
        /* [in] */ IDrawable* layer,
        /* [in] */ ArrayOf<Int32>* themeAttrs,
        /* [in] */ Int32 id,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    /**
     * Refreshes the cached padding values for the specified child.
     *
     * @return true if the child's padding has changed
     */
    CARAPI_(Boolean) RefreshChildPadding(
        /* [in] */ Int32 i,
        /* [in] */ ChildDrawable* r);

    /**
     * Ensures the child padding caches are large enough.
     */
    CARAPI_(void) EnsurePadding();

    /**
     * Initializes the constant state from the values in the typed array.
     */
    CARAPI_(void) UpdateStateFromTypedArray(
        /* [in] */ ITypedArray* a);

    /**
     * Inflates child layers using the specified parser.
     */
    CARAPI InflateLayers(
        /* [in] */ IResources* r,
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ IResourcesTheme* theme) /*throws XmlPullParserException, IOException*/;

    CARAPI_(void) UpdateLayerFromTypedArray(
        /* [in] */ ChildDrawable* layer,
        /* [in] */ ITypedArray* a);

    CARAPI_(void) AddLayer(
        /* [in] */ ChildDrawable* layer);

    CARAPI_(void) ComputeNestedPadding(
        /* [in] */ IRect* padding);

    CARAPI_(void) ComputeStackedPadding(
        /* [in] */ IRect* padding);

public:
    AutoPtr<LayerState> mLayerState;

private:
    Int32 mOpacityOverride;
    AutoPtr< ArrayOf<Int32> > mPaddingL;
    AutoPtr< ArrayOf<Int32> > mPaddingT;
    AutoPtr< ArrayOf<Int32> > mPaddingR;
    AutoPtr< ArrayOf<Int32> > mPaddingB;

    AutoPtr<IRect> mTmpRect;
    AutoPtr<IRect> mHotspotBounds;
    Boolean mMutated;
};

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GRAPHICS_DRAWABLE_LAYERDRAWABLE_H__
