
#ifndef __ELASTOS_DROID_GRAPHICS_DRAWABLE_COLORDRAWABLE_H__
#define __ELASTOS_DROID_GRAPHICS_DRAWABLE_COLORDRAWABLE_H__

#include "elastos/droid/graphics/drawable/Drawable.h"

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

class ColorDrawable
    : public Drawable
    , public IColorDrawable
{
protected:
    class ColorState
        : public Drawable::ConstantState
    {
    public:
        ColorState();

        ColorState(
            /* [in] */ ColorState* state);

        // @Override
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

    public:
        AutoPtr<ArrayOf<Int32> > mThemeAttrs;
        Int32 mBaseColor; // base color, independent of setAlpha()
        Int32 mUseColor;  // basecolor modulated by setAlpha()
        Int32 mChangingConfigurations;
        AutoPtr<IColorStateList> mTint;
        PorterDuffMode mTintMode;
    };

public:
    CAR_INTERFACE_DECL();

    /**
     * Creates a new black ColorDrawable.
     */
    ColorDrawable();

    /**
     * Creates a new ColorDrawable with the specified color.
     *
     * @param color The color to draw.
     */
    ColorDrawable(
        /* [in] */ Int32 color);

    ColorDrawable(
        /* [in] */ IDrawableConstantState* state,
        /* [in] */ IResources* res,
        /* [in] */ IResourcesTheme* theme);

    //@Override
    CARAPI GetChangingConfigurations(
        /* [out] */ Int32* configuration);

    /**
     * A mutable BitmapDrawable still shares its Bitmap with any other Drawable
     * that comes from the same resource.
     *
     * @return This drawable.
     */
    //@Override
    CARAPI Mutate(
        /* [out] */ IDrawable** drawable);

    //@Override
    CARAPI Draw(
        /* [in] */ ICanvas* canvas);

    /**
     * Gets the drawable's color value.
     *
     * @return int The color to draw.
     */
    virtual CARAPI GetColor(
        /* [out] */ Int32* color);

    /**
     * Sets the drawable's color value. This action will clobber the results of prior calls to
     * {@link #setAlpha(int)} on this object, which side-affected the underlying color.
     *
     * @param color The color to draw.
     */
    virtual CARAPI SetColor(
        /* [in] */ Int32 color);

    /**
     * Returns the alpha value of this drawable's color.
     *
     * @return A value between 0 and 255.
     */
    virtual CARAPI GetAlpha(
        /* [out] */ Int32* alpha);

    /**
     * Sets the color's alpha value.
     *
     * @param alpha The alpha value to set, between 0 and 255.
     */
    //@Override
    CARAPI SetAlpha(
        /* [in] */ Int32 alpha);

    /**
     * Sets the color filter applied to this color.
     * <p>
     * Only supported on version {@link android.os.Build.VERSION_CODES#LOLLIPOP} and
     * above. Calling this method has no effect on earlier versions.
     *
     * @see android.graphics.drawable.Drawable#setColorFilter(ColorFilter)
     */
    //@Override
    CARAPI SetColorFilter(
        /* [in] */ IColorFilter* colorFilter);

    // @Override
    CARAPI SetTintList(
        /* [in] */ IColorStateList* tint);

    // @Override
    CARAPI SetTintMode(
        /* [in] */ PorterDuffMode tintMode);

    // @Override
    CARAPI IsStateful(
        /* [out] */ Boolean* state);

    //@Override
    CARAPI GetOpacity(
        /* [out] */ Int32* opacity);

    // @Override
    CARAPI GetOutline(
        /* [in] */ /*@NonNull*/ IOutline* outline);

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

protected:
    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 color);

    CARAPI constructor(
        /* [in] */ IDrawableConstantState* state,
        /* [in] */ IResources* res,
        /* [in] */ IResourcesTheme* theme);

    // @Override
    CARAPI_(Boolean) OnStateChange(
        /* [in] */ const ArrayOf<Int32>* stateSet);

private:
    /**
     * Updates the constant state from the values in the typed array.
     */
    CARAPI_(void) UpdateStateFromTypedArray(
        /* [in] */ ITypedArray* a);

private:
    AutoPtr<ColorState> mColorState;
    AutoPtr<IPorterDuffColorFilter> mTintFilter;
    AutoPtr<IPaint> mPaint;
    Boolean mMutated;
};

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GRAPHICS_DRAWABLE_COLORDRAWABLE_H__

