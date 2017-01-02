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

#ifndef __ELASTOS_DROID_GRAPHICS_DRAWABLE_GRADIENTDRAWABLE_H__
#define __ELASTOS_DROID_GRAPHICS_DRAWABLE_GRADIENTDRAWABLE_H__

#include "elastos/droid/graphics/drawable/Drawable.h"
#include "elastos/droid/graphics/CRectF.h"

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

/**
 * A Drawable with a color gradient for buttons, backgrounds, etc.
 *
 * <p>It can be defined in an XML file with the <code>&lt;shape></code> element. For more
 * information, see the guide to <a
 * href="{@docRoot}guide/topics/resources/drawable-resource.html">Drawable Resources</a>.</p>
 *
 * @attr ref android.R.styleable#GradientDrawable_visible
 * @attr ref android.R.styleable#GradientDrawable_shape
 * @attr ref android.R.styleable#GradientDrawable_innerRadiusRatio
 * @attr ref android.R.styleable#GradientDrawable_innerRadius
 * @attr ref android.R.styleable#GradientDrawable_thicknessRatio
 * @attr ref android.R.styleable#GradientDrawable_thickness
 * @attr ref android.R.styleable#GradientDrawable_useLevel
 * @attr ref android.R.styleable#GradientDrawableSize_width
 * @attr ref android.R.styleable#GradientDrawableSize_height
 * @attr ref android.R.styleable#GradientDrawableGradient_startColor
 * @attr ref android.R.styleable#GradientDrawableGradient_centerColor
 * @attr ref android.R.styleable#GradientDrawableGradient_endColor
 * @attr ref android.R.styleable#GradientDrawableGradient_useLevel
 * @attr ref android.R.styleable#GradientDrawableGradient_angle
 * @attr ref android.R.styleable#GradientDrawableGradient_type
 * @attr ref android.R.styleable#GradientDrawableGradient_centerX
 * @attr ref android.R.styleable#GradientDrawableGradient_centerY
 * @attr ref android.R.styleable#GradientDrawableGradient_gradientRadius
 * @attr ref android.R.styleable#GradientDrawableSolid_color
 * @attr ref android.R.styleable#GradientDrawableStroke_width
 * @attr ref android.R.styleable#GradientDrawableStroke_color
 * @attr ref android.R.styleable#GradientDrawableStroke_dashWidth
 * @attr ref android.R.styleable#GradientDrawableStroke_dashGap
 * @attr ref android.R.styleable#GradientDrawablePadding_left
 * @attr ref android.R.styleable#GradientDrawablePadding_top
 * @attr ref android.R.styleable#GradientDrawablePadding_right
 * @attr ref android.R.styleable#GradientDrawablePadding_bottom
 */
class GradientDrawable
    : public Drawable
    , public IGradientDrawable
{
public:
    class GradientState
        : public Drawable::ConstantState
    {
        friend class GradientDrawable;

    public:
        GradientState(
            /* [in] */ GradientDrawableOrientation orientation,
            /* [in] */ ArrayOf<Int32>* colors);

        GradientState(
            /* [in] */ GradientState* state);

        ~GradientState();

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

        CARAPI_(void) SetShape(
            /* [in] */ Int32 shape);

        CARAPI_(void) SetGradientType(
            /* [in] */ Int32 gradient);

        CARAPI_(void) SetGradientCenter(
            /* [in] */ Float x,
            /* [in] */ Float y);

        CARAPI_(void) SetColors(
            /* [in] */ ArrayOf<Int32>* colors);

        CARAPI_(void) SetColorStateList(
            /* [in] */ IColorStateList* colorStateList);

        CARAPI_(void) SetStroke(
            /* [in] */ Int32 width,
            /* [in] */ IColorStateList* colorStateList,
            /* [in] */ Float dashWidth,
            /* [in] */ Float dashGap);

        CARAPI_(void) SetCornerRadius(
            /* [in] */ Float radius);

        CARAPI_(void) SetCornerRadii(
            /* [in] */ ArrayOf<Float>* radii);

        CARAPI_(void) SetSize(
            /* [in] */ Int32 width,
            /* [in] */ Int32 height);

        CARAPI_(void) SetGradientRadius(
            /* [in] */ Float gradientRadius,
            /* [in] */ Int32 type);

        // @Override
        virtual CARAPI CanApplyTheme(
            /* [out] */ Boolean* can);

    private:
        CARAPI_(void) ComputeOpacity();

        static CARAPI_(Boolean) IsOpaque(
            /* [in] */ Int32 color);

    public:
        Int32 mChangingConfigurations;
        Int32 mShape;
        Int32 mGradient;
        Int32 mAngle;
        GradientDrawableOrientation mOrientation;
        AutoPtr<IColorStateList> mColorStateList;
        AutoPtr<IColorStateList> mStrokeColorStateList;
        AutoPtr< ArrayOf<Int32> > mColors;
        AutoPtr< ArrayOf<Int32> > mTempColors; // no need to copy
        AutoPtr< ArrayOf<Float> > mTempPositions; // no need to copy
        AutoPtr< ArrayOf<Float> > mPositions;
        Int32 mStrokeWidth;   // if >= 0 use stroking.
        Float mStrokeDashWidth;
        Float mStrokeDashGap;
        Float mRadius;    // use this if mRadiusArray is null
        AutoPtr< ArrayOf<Float> > mRadiusArray;
        AutoPtr<IRect> mPadding;
        Int32 mWidth;
        Int32 mHeight;
        Float mInnerRadiusRatio;
        Float mThicknessRatio;
        Int32 mInnerRadius;
        Boolean mDither;
        Int32 mThickness;
        AutoPtr<ArrayOf<Int32> > mThemeAttrs;
        AutoPtr<ArrayOf<Int32> > mAttrSize;
        AutoPtr<ArrayOf<Int32> > mAttrGradient;
        AutoPtr<ArrayOf<Int32> > mAttrSolid;
        AutoPtr<ArrayOf<Int32> > mAttrStroke;
        AutoPtr<ArrayOf<Int32> > mAttrCorners;
        AutoPtr<ArrayOf<Int32> > mAttrPadding;

    private:
        Float mCenterX;
        Float mCenterY;
        Float mGradientRadius;
        Int32 mGradientRadiusType;
        Boolean mUseLevel;
        Boolean mUseLevelForShape;
        Boolean mOpaqueOverBounds;
        Boolean mOpaqueOverShape;
    };

public:
    CAR_INTERFACE_DECL()

    GradientDrawable();

    CARAPI constructor();

    /**
     * Create a new gradient drawable given an orientation and an array
     * of colors for the gradient.
     */
    CARAPI constructor(
        /* [in] */ GradientDrawableOrientation orientation,
        /* [in] */ ArrayOf<Int32>* colors);

    CARAPI constructor(
        /* [in] */ IDrawableConstantState* state,
        /* [in] */ IResourcesTheme* theme);

    //@Override
    CARAPI GetPadding(
        /* [in] */ IRect* padding,
        /* [out] */ Boolean* isPadding);

    /**
     * Specify radii for each of the 4 corners. For each corner, the array
     * contains 2 values, [X_radius, Y_radius]. The corners are ordered
     * top-left, top-right, bottom-right, bottom-left
     */
    virtual CARAPI SetCornerRadii(
        /* [in] */ ArrayOf<Float>* radii);

    /**
     * Specify radius for the corners of the gradient. If this is > 0, then the
     * drawable is drawn in a round-rectangle, rather than a rectangle.
     */
    virtual CARAPI SetCornerRadius(
        /* [in] */ Float radius);

    /**
     * Set the stroke width and color for the drawable. If width is zero,
     * then no stroke is drawn.
     */
    virtual CARAPI SetStroke(
        /* [in] */ Int32 width,
        /* [in] */ Int32 color);

    virtual CARAPI SetStroke(
        /* [in] */ Int32 width,
        /* [in] */ Int32 color,
        /* [in] */ Float dashWidth,
        /* [in] */ Float dashGap);

    virtual CARAPI SetStroke(
        /* [in] */ Int32 width,
        /* [in] */ IColorStateList* colorStateList,
        /* [in] */ Float dashWidth,
        /* [in] */ Float dashGap);

    virtual CARAPI SetStroke(
        /* [in] */ Int32 width,
        /* [in] */ IColorStateList* colorStateList);

    virtual CARAPI SetSize(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    virtual CARAPI SetShape(
        /* [in] */ Int32 shape);

    virtual CARAPI SetGradientType(
        /* [in] */ Int32 gradient);

    virtual CARAPI SetGradientCenter(
        /* [in] */ Float x,
        /* [in] */ Float y);

    virtual CARAPI SetGradientRadius(
        /* [in] */ Float gradientRadius);

    /**
     * Returns the radius of the gradient in pixels. The radius is valid only
     * when the gradient type is set to {@link #RADIAL_GRADIENT}.
     *
     * @return Radius in pixels.
     */
    virtual CARAPI GetGradientRadius(
        /* [out] */ Float* gradientRadius);

    virtual CARAPI SetUseLevel(
        /* [in] */ Boolean useLevel);

    /**
     * Returns the orientation of the gradient defined in this drawable.
     */
    virtual CARAPI GetOrientation(
        /*[out] */ GradientDrawableOrientation* orientation);

    /**
     * <p>Changes the orientation of the gradient defined in this drawable.</p>
     * <p><strong>Note</strong>: changing orientation will affect all instances
     * of a drawable loaded from a resource. It is recommended to invoke
     * {@link #mutate()} before changing the orientation.</p>
     *
     * @param orientation The desired orientation (angle) of the gradient
     *
     * @see #mutate()
     */
    virtual CARAPI SetOrientation(
        /* [in] */ GradientDrawableOrientation orientation);

    /**
     * <p>Sets the colors used to draw the gradient. Each color is specified as an
     * ARGB integer and the array must contain at least 2 colors.</p>
     * <p><strong>Note</strong>: changing orientation will affect all instances
     * of a drawable loaded from a resource. It is recommended to invoke
     * {@link #mutate()} before changing the orientation.</p>
     *
     * @param colors 2 or more ARGB colors
     *
     * @see #mutate()
     * @see #setColor(int)
     */
    virtual CARAPI SetColors(
        /* [in] */ ArrayOf<Int32>* colors);

    //@Override
    CARAPI Draw(
        /* [in] */ ICanvas* canvas);

    virtual CARAPI SetColor(
        /* [in] */ Int32 argb);

    virtual CARAPI SetColor(
        /* [in] */ IColorStateList* colorStateList);

    //@Override
    CARAPI GetChangingConfigurations(
        /* [out] */ Int32* configuration);

    //@Override
    CARAPI SetAlpha(
        /* [in] */ Int32 alpha);

    //@Override
    CARAPI SetDither(
        /* [in] */ Boolean dither);

    //@Override
    CARAPI SetColorFilter(
        /* [in] */ IColorFilter* cf);

    //@Override
    CARAPI GetOpacity(
        /* [out] */ Int32* opacity);

    //@Override
    virtual CARAPI Inflate(
        /* [in] */ IResources* r,
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ IResourcesTheme* theme)/* throws XmlPullParserException, IOException*/;

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

    virtual CARAPI IsStateful(
        /* [out] */ Boolean* isStateful);

    virtual CARAPI GetAlpha(
        /* [out] */ Int32* alpha);

    virtual CARAPI GetColorFilter(
        /* [out] */ IColorFilter** filter);

    // @Override
    CARAPI CanApplyTheme(
        /* [out] */ Boolean* can);

    // @Override
    CARAPI GetOutline(
        /* [out] */ IOutline* outline);

protected:
    //@Override
    CARAPI_(void) OnBoundsChange(
        /* [in] */ IRect* r);

    //@Override
    CARAPI_(Boolean) OnLevelChange(
        /* [in] */ Int32 level);

    CARAPI_(Boolean) OnStateChange(
        /* [in] */ ArrayOf<Int32>* stateSet);

private:
    CARAPI_(Int32) ModulateAlpha(
        /* [in] */ Int32 alpha);

    CARAPI_(AutoPtr<IPath>) BuildRing(
        /* [in] */ GradientState* st);

    /**
     * This checks mRectIsDirty, and if it is true, recomputes both our drawing
     * rectangle (mRect) and the gradient itself, since it depends on our
     * rectangle too.
     * @return true if the resulting rectangle is not empty, false otherwise
     */
    CARAPI_(Boolean) EnsureValidRect();

    static CARAPI_(Float) GetFloatOrFraction(
        /* [in] */ ITypedArray* a,
        /* [in] */ Int32 index,
        /* [in] */ Float defaultValue);

    CARAPI_(void) InitializeWithState(
        /* [in] */ GradientState* state);

    CARAPI SetStrokeInternal(
        /* [in] */ Int32 width,
        /* [in] */ Int32 color,
        /* [in] */ Float dashWidth,
        /* [in] */ Float dashGap);

    CARAPI_(void) BuildPathIfDirty();

    /**
     * Updates the constant state from the values in the typed array.
     */
    CARAPI UpdateStateFromTypedArray(
        /* [in] */ ITypedArray* a);

    CARAPI ApplyThemeChildElements(
        /* [in] */ IResourcesTheme* t);

    CARAPI InflateChildElements(
        /* [in] */ IResources* r,
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ IResourcesTheme* theme) /*throws XmlPullParserException, IOException*/;

    CARAPI UpdateGradientDrawablePadding(
        /* [in] */ ITypedArray* a);

    CARAPI UpdateDrawableCorners(
        /* [in] */ ITypedArray* a);

    CARAPI UpdateGradientDrawableStroke(
        /* [in] */ ITypedArray* a);

    CARAPI UpdateGradientDrawableSolid(
        /* [in] */ ITypedArray* a);

    CARAPI UpdateGradientDrawableGradient(
        /* [in] */ IResources* r,
        /* [in] */ ITypedArray* a) /*throws XmlPullParserException*/;

    CARAPI UpdateGradientDrawableSize(
        /* [in] */ ITypedArray* a);

    CARAPI_(Boolean) IsOpaqueForState();

    static CARAPI_(Boolean) IsOpaque(
        /* [in] */ Int32 color);

private:
    /** Radius is in pixels. */
    static const Int32 RADIUS_TYPE_PIXELS;

    /** Radius is a fraction of the base size. */
    static const Int32 RADIUS_TYPE_FRACTION;

    /** Radius is a fraction of the bounds size. */
    static const Int32 RADIUS_TYPE_FRACTION_PARENT;

    static const Float DEFAULT_INNER_RADIUS_RATIO;
    static const Float DEFAULT_THICKNESS_RATIO;

    AutoPtr<GradientState> mGradientState;

    AutoPtr<IPaint> mFillPaint;
    AutoPtr<IRect> mPadding;
    AutoPtr<IPaint> mStrokePaint;   // optional, set by the caller
    AutoPtr<IColorFilter> mColorFilter;   // optional, set by the caller
    Int32 mAlpha;  // modified by the caller

    AutoPtr<IPath> mPath;
    AutoPtr<IRectF> mRect;

    AutoPtr<IPaint> mLayerPaint;    // internal, used if we use saveLayer()
    Boolean mGradientIsDirty;   // internal state
    Boolean mMutated;
    AutoPtr<IPath> mRingPath;
    Boolean mPathIsDirty;

    /** Current gradient radius, valid when {@link #mGradientIsDirty} is false. */
    Float mGradientRadius;
};

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GRAPHICS_DRAWABLE_GRADIENTDRAWABLE_H__
