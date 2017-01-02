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

#ifndef __ELASTOS_DROID_GRAPHICS_DRAWABLE_SHAPEDRAWABLE_H__
#define __ELASTOS_DROID_GRAPHICS_DRAWABLE_SHAPEDRAWABLE_H__

#include "elastos/droid/graphics/drawable/Drawable.h"

using Elastos::Droid::Graphics::Drawable::Shapes::IShape;

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

class ShapeDrawable
    : public Drawable
    , public IShapeDrawable
{
protected:
    /**
     * Defines the intrinsic properties of this ShapeDrawable's Shape.
     */
    class ShapeState
        : public Drawable::ConstantState
    {
    public:
        ShapeState(
            /* [in] */ ShapeState* orig);

        // @Override
        CARAPI CanApplyTheme(
            /* [out] */ Boolean* can);

        //@Override
        CARAPI NewDrawable(
            /* [out] */ IDrawable** drawable);

        //@Override
        CARAPI NewDrawable(
            /* [in] */ IResources* res,
            /* [out] */ IDrawable** drawable);

        // @Override
        CARAPI NewDrawable(
            /* [in] */ IResources* res,
            /* [in] */ IResourcesTheme* theme,
            /* [out] */ IDrawable** drawable);

        //@Override
        CARAPI GetChangingConfigurations(
            /* [out] */ Int32* conf);

    public:
        AutoPtr<ArrayOf<Int32> > mThemeAttrs;
        Int32 mChangingConfigurations;
        AutoPtr<IPaint> mPaint;
        AutoPtr<IShape> mShape;
        AutoPtr<IColorStateList> mTint;
        PorterDuffMode mTintMode;
        AutoPtr<IRect> mPadding;
        Int32 mIntrinsicWidth;
        Int32 mIntrinsicHeight;
        Int32 mAlpha;
        AutoPtr<IShaderFactory> mShaderFactory;
    };



public:
    CAR_INTERFACE_DECL()

    /**
     * ShapeDrawable constructor.
     */
    ShapeDrawable();

    /**
     * ShapeDrawable constructor.
     */
    CARAPI constructor();

    /**
     * Creates a ShapeDrawable with a specified Shape.
     *
     * @param s the Shape that this ShapeDrawable should be
     */
    CARAPI constructor(
        /* [in] */ IShape* s);

    CARAPI constructor(
        /* [in] */ IDrawableConstantState* state,
        /* [in] */ IResources* res,
        /* [in] */ IResourcesTheme* theme);

    /**
     * Returns the Shape of this ShapeDrawable.
     */
    virtual CARAPI GetShape(
        /* [out] */ IShape** shape);

    /**
     * Sets the Shape of this ShapeDrawable.
     */
    virtual CARAPI SetShape(
        /* [in] */ IShape* s);

    /**
     * Sets a ShaderFactory to which requests for a
     * {@link android.graphics.Shader} object will be made.
     *
     * @param fact an instance of your ShaderFactory implementation
     */
    virtual CARAPI SetShaderFactory(
        /* [in] */ IShaderFactory* fact);

    /**
     * Returns the ShaderFactory used by this ShapeDrawable for requesting a
     * {@link android.graphics.Shader}.
     */
    virtual CARAPI GetShaderFactory(
        /* [out] */ IShaderFactory** fact);

    /**
     * Returns the Paint used to draw the shape.
     */
    virtual CARAPI GetPaint(
        /* [out] */ IPaint** paint);

    /**
     * Sets padding for the shape.
     *
     * @param left    padding for the left side (in pixels)
     * @param top     padding for the top (in pixels)
     * @param right   padding for the right side (in pixels)
     * @param bottom  padding for the bottom (in pixels)
     */
    virtual CARAPI SetPadding(
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    /**
     * Sets padding for this shape, defined by a Rect object.
     * Define the padding in the Rect object as: left, top, right, bottom.
     */
    virtual CARAPI SetPadding(
        /* [in] */ IRect* padding);

    /**
     * Sets the intrinsic (default) width for this shape.
     *
     * @param width the intrinsic width (in pixels)
     */
    virtual CARAPI SetIntrinsicWidth(
        /* [in] */ Int32 width);

    /**
     * Sets the intrinsic (default) height for this shape.
     *
     * @param height the intrinsic height (in pixels)
     */
    virtual CARAPI SetIntrinsicHeight(
        /* [in] */ Int32 height);

    //@Override
    CARAPI GetIntrinsicWidth(
        /* [out] */ Int32* width);

    //@Override
    CARAPI GetIntrinsicHeight(
        /* [out] */ Int32* height);

    //@Override
    CARAPI GetPadding(
        /* [in] */ IRect* padding,
        /* [out] */ Boolean* isPadding);

    //@Override
    CARAPI Draw(
        /* [in] */ ICanvas* canvas);

    //@Override
    CARAPI GetChangingConfigurations(
        /* [out] */ Int32* configuration);

    /**
     * Set the alpha level for this drawable [0..255]. Note that this drawable
     * also has a color in its paint, which has an alpha as well. These two
     * values are automatically combined during drawing. Thus if the color's
     * alpha is 75% (i.e. 192) and the drawable's alpha is 50% (i.e. 128), then
     * the combined alpha that will be used during drawing will be 37.5%
     * (i.e. 96).
     */
    //@Override
    CARAPI SetAlpha(
        /* [in] */ Int32 alpha);

    // @Override
    CARAPI GetAlpha(
        /* [out] */ Int32* alpha);

    // @Override
    CARAPI SetTintList(
        /* [in] */ IColorStateList* tint);

    // @Override
    CARAPI SetTintMode(
        /* [in] */ PorterDuffMode tintMode);

    //@Override
    CARAPI SetColorFilter(
        /* [in] */ IColorFilter* cf);

    //@Override
    CARAPI GetOpacity(
        /* [out] */ Int32* opacity);

    //@Override
    CARAPI SetDither(
        /* [in] */ Boolean dither);

    //@Override
    CARAPI Inflate(
        /* [in] */ IResources* r,
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ IResourcesTheme* theme);

    // @Override
    CARAPI ApplyTheme(
        /* [in] */ IResourcesTheme* t);

    //@Override
    CARAPI GetConstantState(
        /* [out] */ IDrawableConstantState** state);

    //@Override
    CARAPI Mutate();

    // @Override
    CARAPI IsStateful(
        /* [out] */ Boolean* isStateful);

    // @Override
    CARAPI GetOutline(
        /* [in] */ IOutline* outline);

protected:
    /**
     * Called from the drawable's draw() method after the canvas has been set
     * to draw the shape at (0,0). Subclasses can override for special effects
     * such as multiple layers, stroking, etc.
     */
    CARAPI_(void) OnDraw(
        /* [in] */ IShape* shape,
        /* [in] */ ICanvas* canvas,
        /* [in] */ IPaint* paint);

    //@Override
    CARAPI_(void) OnBoundsChange(
        /* [in] */ IRect* bounds);

    /**
     * Subclasses override this to parse custom subelements.
     * If you handle it, return true, else return <em>super.inflateTag(...)</em>.
     */
    virtual CARAPI_(Boolean) InflateTag(
        /* [in] */ const String& name,
        /* [in] */ IResources* r,
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ IAttributeSet* attrs);

    // @Override
    CARAPI_(Boolean) OnStateChange(
        /* [in] */ ArrayOf<Int32>* stateSet);

private:
    static CARAPI_(Int32) ModulateAlpha(
        /* [in] */ Int32 paintAlpha,
        /* [in] */ Int32 alpha);

    CARAPI_(void) UpdateShape();

    CARAPI UpdateStateFromTypedArray(
        /* [in] */ ITypedArray* a);

    /**
     * Initializes local dynamic properties from state. This should be called
     * after significant state changes, e.g. from the One True Constructor and
     * after inflating or applying a theme.
     */
    CARAPI InitializeWithState(
        /* [in] */ ShapeState* state,
        /* [in] */ IResources* res);

private:
    AutoPtr<ShapeState> mShapeState;
    AutoPtr<IPorterDuffColorFilter> mTintFilter;
    Boolean mMutated;
};

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GRAPHICS_DRAWABLE_SHAPEDRAWABLE_H__
