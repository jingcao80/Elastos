//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_GRAPHICS_DRAWABLE_BITMAPDRAWABLE_H__
#define __ELASTOS_DROID_GRAPHICS_DRAWABLE_BITMAPDRAWABLE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/drawable/Drawable.h"


using Elastos::Droid::Utility::IDisplayMetrics;

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

class BitmapDrawable
    : public Drawable
    , public IBitmapDrawable
{
public:
    class BitmapState
        : public Drawable::ConstantState
    {
    public:
        BitmapState(
            /* [in] */ IBitmap* bitmap);

        BitmapState(
            /* [in] */ BitmapState* bitmap);

        // @Override
        CARAPI CanApplyTheme(
            /* [out] */ Boolean* can);

        // @Override
        CARAPI GetBitmap(
            /* [out] */ IBitmap** bitmap);

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

    public:
        AutoPtr<IPaint> mPaint;

        // Values loaded during inflation.
        AutoPtr<ArrayOf<Int32> > mThemeAttrs;
        AutoPtr<IBitmap> mBitmap;
        AutoPtr<IColorStateList> mTint;
        PorterDuffMode mTintMode;
        Int32 mGravity;
        Float mBaseAlpha;
        ShaderTileMode mTileModeX;
        ShaderTileMode mTileModeY;
        Int32 mTargetDensity;
        Boolean mAutoMirrored;

        Int32 mChangingConfigurations;
        Boolean mRebuildShader;
    };

public:
    CAR_INTERFACE_DECL()

    BitmapDrawable();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IResources* res);

    CARAPI constructor(
        /* [in] */ IBitmap* bitmap);

    CARAPI constructor(
        /* [in] */ IResources* res,
        /* [in] */ IBitmap* bitmap);

    CARAPI constructor(
        /* [in] */ const String& filepath);

    CARAPI constructor(
        /* [in] */ IResources* res,
        /* [in] */ const String& filepath);

    CARAPI constructor(
        /* [in] */ IInputStream* is);

    CARAPI constructor(
        /* [in] */ IResources* res,
        /* [in] */ IInputStream* is);

    CARAPI constructor(
        /* [in] */ IDrawableConstantState* state,
        /* [in] */ IResources* res,
        /* [in] */ IResourcesTheme* theme);

    CARAPI GetPaint(
        /* [out] */ IPaint** paint);

    /**
     * Returns the bitmap used by this drawable to render. May be null.
     */
    CARAPI GetBitmap(
        /* [out] */ IBitmap** bitmap);

    /**
     * Set the density scale at which this drawable will be rendered. This
     * method assumes the drawable will be rendered at the same density as the
     * specified canvas.
     *
     * @param canvas The Canvas from which the density scale must be obtained.
     *
     * @see android.graphics.Bitmap#setDensity(Int32)
     * @see android.graphics.Bitmap#getDensity()
     */
    virtual CARAPI SetTargetDensity(
        /* [in] */ ICanvas* canvas);

    /**
     * Set the density scale at which this drawable will be rendered.
     *
     * @param metrics The DisplayMetrics indicating the density scale for this drawable.
     *
     * @see android.graphics.Bitmap#setDensity(Int32)
     * @see android.graphics.Bitmap#getDensity()
     */
    virtual CARAPI SetTargetDensity(
        /* [in] */ IDisplayMetrics* metrics);

    /**
     * Set the density at which this drawable will be rendered.
     *
     * @param density The density scale for this drawable.
     *
     * @see android.graphics.Bitmap#setDensity(Int32)
     * @see android.graphics.Bitmap#getDensity()
     */
    virtual CARAPI SetTargetDensity(
        /* [in] */ Int32 density);

    /** Get the gravity used to position/stretch the bitmap within its bounds.
     * See android.view.Gravity
     * @return the gravity applied to the bitmap
     */
    virtual CARAPI GetGravity(
        /* [out] */ Int32* gravity);

    /** Set the gravity used to position/stretch the bitmap within its bounds.
     *  See android.view.Gravity
     * @param gravity the gravity
     */
    virtual CARAPI SetGravity(
        /* [in] */ Int32 gravity);

    /**
     * Enables or disables the mipmap hint for this drawable's bitmap.
     * See {@link Bitmap#setHasMipMap(boolean)} for more information.
     *
     * If the bitmap is null calling this method has no effect.
     *
     * @param mipMap True if the bitmap should use mipmaps, false otherwise.
     *
     * @see #hasMipMap()
     */
    virtual CARAPI SetMipMap(
        /* [in] */ Boolean mipMap);

    /**
     * Indicates whether the mipmap hint is enabled on this drawable's bitmap.
     *
     * @return True if the mipmap hint is set, false otherwise. If the bitmap
     *         is null, this method always returns false.
     *
     * @see #setMipMap(boolean)
     * @attr ref android.R.styleable#BitmapDrawable_mipMap
     */
    virtual CARAPI HasMipMap(
        /* [out] */ Boolean* has);

    /**
     * Enables or disables anti-aliasing for this drawable. Anti-aliasing affects
     * the edges of the bitmap only so it applies only when the drawable is rotated.
     *
     * @param aa True if the bitmap should be anti-aliased, false otherwise.
     */
    virtual CARAPI SetAntiAlias(
        /* [in] */ Boolean aa);

    /**
     * Indicates whether anti-aliasing is enabled for this drawable.
     *
     * @return True if anti-aliasing is enabled, false otherwise.
     *
     * @see #setAntiAlias(boolean)
     */
    virtual CARAPI HasAntiAlias(
        /* [out] */ Boolean* has);

    //@Override
    CARAPI SetFilterBitmap(
        /* [in] */ Boolean filter);

    //@Override
    CARAPI SetDither(
        /* [in] */ Boolean dither);

    /**
     * Indicates the repeat behavior of this drawable on the X axis.
     *
     * @return {@link Shader.TileMode#CLAMP} if the bitmap does not repeat,
     *         {@link Shader.TileMode#REPEAT} or {@link Shader.TileMode#MIRROR} otherwise.
     */
    virtual CARAPI GetTileModeX(
        /* [out] */ ShaderTileMode* tileModeX);

    /**
     * Indicates the repeat behavior of this drawable on the Y axis.
     *
     * @return {@link Shader.TileMode#CLAMP} if the bitmap does not repeat,
     *         {@link Shader.TileMode#REPEAT} or {@link Shader.TileMode#MIRROR} otherwise.
     */
    virtual CARAPI GetTileModeY(
        /* [out] */ ShaderTileMode* tileModeY);

    /**
     * Sets the repeat behavior of this drawable on the X axis. By default, the drawable
     * does not repeat its bitmap. Using {@link Shader.TileMode#REPEAT} or
     * {@link Shader.TileMode#MIRROR} the bitmap can be repeated (or tiled) if the bitmap
     * is smaller than this drawable.
     *
     * @param mode The repeat mode for this drawable.
     *
     * @see #setTileModeY(android.graphics.Shader.TileMode)
     * @see #setTileModeXY(android.graphics.Shader.TileMode, android.graphics.Shader.TileMode)
     */
    virtual CARAPI SetTileModeX(
        /* [in] */ ShaderTileMode mode);

    /**
     * Sets the repeat behavior of this drawable on the Y axis. By default, the drawable
     * does not repeat its bitmap. Using {@link Shader.TileMode#REPEAT} or
     * {@link Shader.TileMode#MIRROR} the bitmap can be repeated (or tiled) if the bitmap
     * is smaller than this drawable.
     *
     * @param mode The repeat mode for this drawable.
     *
     * @see #setTileModeX(android.graphics.Shader.TileMode)
     * @see #setTileModeXY(android.graphics.Shader.TileMode, android.graphics.Shader.TileMode)
     */
    CARAPI SetTileModeY(
        /* [in] */ ShaderTileMode mode);

    /**
     * Sets the repeat behavior of this drawable on both axis. By default, the drawable
     * does not repeat its bitmap. Using {@link Shader.TileMode#REPEAT} or
     * {@link Shader.TileMode#MIRROR} the bitmap can be repeated (or tiled) if the bitmap
     * is smaller than this drawable.
     *
     * @param xmode The X repeat mode for this drawable.
     * @param ymode The Y repeat mode for this drawable.
     *
     * @see #setTileModeX(android.graphics.Shader.TileMode)
     * @see #setTileModeY(android.graphics.Shader.TileMode)
     */
    virtual CARAPI SetTileModeXY(
        /* [in] */ ShaderTileMode xmode,
        /* [in] */ ShaderTileMode ymode);

    // @Override
    virtual CARAPI SetAutoMirrored(
        /* [in] */ Boolean mirrored);

    // @Override
    CARAPI IsAutoMirrored(
        /* [out] */ Boolean* mirrored);

    //@Override
    CARAPI GetChangingConfigurations(
        /* [out] */ Int32* configuration);

    //@Override
    CARAPI Draw(
        /* [in] */ ICanvas* canvas);

    //@Override
    CARAPI SetAlpha(
        /* [in] */ Int32 alpha);

    // @Override
    CARAPI GetAlpha(
        /* [out] */ Int32* alpha);

    /**
     * @hide
     */
    // @Override
    virtual CARAPI GetOpticalInsets(
        /* [out] */ IInsets** sets);

    // @Override
    CARAPI GetOutline(
        /* [in] */ /*@NonNull*/ IOutline* outline);

    //@Override
    CARAPI SetColorFilter(
        /* [in] */ IColorFilter* cf);

    // @Override
    CARAPI GetColorFilter(
        /* [out] */ IColorFilter** filter);

    // @Override
    CARAPI SetTintList(
        /* [in] */ IColorStateList* tint);

    // @Override
    CARAPI SetTintMode(
        /* [in] */ PorterDuffMode tintMode);

    /**
     * @hide only needed by a hack within ProgressBar
     */
    CARAPI GetTint(
        /* [out] */ IColorStateList** list);

    /**
     * @hide only needed by a hack within ProgressBar
     */
    CARAPI GetTintMode(
        /* [out] */ PorterDuffMode* mode);

    /**
     * @hide Candidate for future API inclusion
     */
    // @Override
    CARAPI SetXfermode(
        /* [in] */ IXfermode* xfermode);

    //@Override
    CARAPI Mutate();

    CARAPI IsStateful(
        /* [out] */ Boolean* is);

    //@Override
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

    //@Override
    CARAPI GetIntrinsicWidth(
        /* [out] */ Int32* width);

    //@Override
    CARAPI GetIntrinsicHeight(
        /* [out] */ Int32* height);

    //@Override
    CARAPI GetOpacity(
        /* [out] */ Int32* opacity);

    //@Override
    CARAPI GetConstantState(
        /* [out] */ IDrawableConstantState** state);

protected:
    //@Override
    CARAPI_(void) OnBoundsChange(
        /* [in] */ IRect* bounds);

    CARAPI_(Boolean) OnStateChange(
        /* [in] */ ArrayOf<Int32>* stateSet);

private:
    CARAPI_(void) ComputeBitmapSize();

    CARAPI_(void) SetBitmap(
        /* [in] */ IBitmap* bitmap);

    CARAPI_(Boolean) NeedMirroring();

    CARAPI_(void) UpdateMirrorMatrix(
        /* [in] */ Float dx);

    CARAPI_(void) UpdateDstRectAndInsetsIfDirty();

    /**
     * Ensures all required attributes are set.
     *
     * @throws XmlPullParserException if any required attributes are missing
     */
    CARAPI VerifyState(
        /* [in] */ ITypedArray* a) /*throws XmlPullParserException*/;

    /**
     * Updates the constant state from the values in the typed array.
     */
    CARAPI UpdateStateFromTypedArray(
        /* [in] */ ITypedArray* a) /*throws XmlPullParserException*/;

    static CARAPI_(ShaderTileMode) ParseTileMode(
        /* [in] */ Int32 tileMode);

    /**
     * The one constructor to rule them all. This is called by all public
     * constructors to set the state and initialize local properties.
     */
    // private BitmapDrawable(BitmapState state, Resources res, Theme theme) {
    //     if (theme != null && state.canApplyTheme()) {
    //         // If we need to apply a theme, implicitly mutate.
    //         mBitmapState = new BitmapState(state);
    //         applyTheme(theme);
    //     } else {
    //         mBitmapState = state;
    //     }

    //     initializeWithState(state, res);
    // }

    /**
     * Initializes local dynamic properties from state. This should be called
     * after significant state changes, e.g. from the One True Constructor and
     * after inflating or applying a theme.
     */
    CARAPI InitializeWithState(
        /* [in] */ BitmapState* state,
        /* [in] */ IResources* res);

private:
    static const Int32 DEFAULT_PAINT_FLAGS;
    // Constants for {@link android.R.styleable#BitmapDrawable_tileMode}.
    static const Int32 TILE_MODE_UNDEFINED;
    static const Int32 TILE_MODE_DISABLED;
    static const Int32 TILE_MODE_CLAMP;
    static const Int32 TILE_MODE_REPEAT;
    static const Int32 TILE_MODE_MIRROR;

    AutoPtr<IRect> mDstRect;   // #updateDstRectAndInsetsIfDirty() sets this
    AutoPtr<BitmapState> mBitmapState;
    AutoPtr<IPorterDuffColorFilter> mTintFilter;

    Int32 mTargetDensity;

    Boolean mDstRectAndInsetsDirty;
    Boolean mMutated;

    // These are scaled to match the target density.
    Int32 mBitmapWidth;
    Int32 mBitmapHeight;

    /** Optical insets due to gravity. */
    AutoPtr<IInsets> mOpticalInsets;

    // Mirroring matrix for using with Shaders
    AutoPtr<IMatrix> mMirrorMatrix;
};

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GRAPHICS_DRAWABLE_BITMAPDRAWABLE_H__
