
#ifndef __ELASTOS_DROID_GRAPHICS_DRAWABLE_NINEPATCHDRAWABLE_H__
#define __ELASTOS_DROID_GRAPHICS_DRAWABLE_NINEPATCHDRAWABLE_H__

#include "elastos/droid/graphics/drawable/Drawable.h"
#include "elastos/droid/graphics/Insets.h"

using Elastos::Droid::Utility::IDisplayMetrics;

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

class NinePatchDrawable
    : public Drawable
    , public INinePatchDrawable
{
public:
    class NinePatchState
        : public Drawable::ConstantState
    {
    public:
        NinePatchState();

        NinePatchState(
            /* [in] *//*@NonNull*/ INinePatch* ninePatch,
            /* [in] *//*@Nullable*/ IRect* padding);

        NinePatchState(
            /* [in] *//*@NonNull*/ INinePatch* ninePatch,
            /* [in] *//*@Nullable*/ IRect* padding,
            /* [in] *//*@Nullable*/ IRect* opticalInsets);

        NinePatchState(
            /* [in] *//*@NonNull*/ INinePatch* ninePatch,
            /* [in] *//*@Nullable*/ IRect* padding,
            /* [in] *//*@Nullable*/ IRect* opticalInsets,
            /* [in] */ Boolean dither,
            /* [in] */ Boolean autoMirror);

        // Copy constructor
        NinePatchState(
            /* [in] *//*@NonNull*/ NinePatchState* state);

        // @Override
        CARAPI CanApplyTheme(
            /* [out] */ Boolean* can);

        // @Override
        CARAPI GetBitmap(
            /* [out] */ IBitmap** bitmap);

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

        // @Override
        CARAPI GetChangingConfigurations(
            /* [out] */ Int32* config);

    private:
        CARAPI constructor(
            /* [in] *//*@NonNull*/ INinePatch* ninePatch,
            /* [in] *//*@Nullable*/ IRect* padding,
            /* [in] *//*@Nullable*/ IRect* opticalInsets,
            /* [in] */ Boolean dither,
            /* [in] */ Boolean autoMirror);

    public:
        // Values loaded during inflation.
        AutoPtr<ArrayOf<Int32> > mThemeAttrs;
        AutoPtr<INinePatch> mNinePatch;
        AutoPtr<IColorStateList> mTint;
        PorterDuffMode mTintMode;
        AutoPtr<IRect> mPadding;
        AutoPtr<Insets> mOpticalInsets;
        Float mBaseAlpha;
        Boolean mDither;
        Int32 mTargetDensity;
        Boolean mAutoMirrored;

        Int32 mChangingConfigurations;
    };

public:
    CAR_INTERFACE_DECL();

    NinePatchDrawable();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IBitmap* bitmap,
        /* [in] */ ArrayOf<Byte>* chunk,
        /* [in] */ IRect* padding,
        /* [in] */ const String& srcName);

    CARAPI constructor(
        /* [in] */ IResources* res,
        /* [in] */ IBitmap* bitmap,
        /* [in] */ ArrayOf<Byte>* chunk,
        /* [in] */ IRect* padding,
        /* [in] */ const String& srcName);

    /**
     * Create drawable from raw nine-patch data, setting initial target density
     * based on the display metrics of the resources.
     *
     * @hide
     */
    CARAPI constructor(
        /* [in] */ IResources* res,
        /* [in] */ IBitmap* bitmap,
        /* [in] */ ArrayOf<Byte>* chunk,
        /* [in] */ IRect* padding,
        /* [in] */ IRect* layoutInsets,
        /* [in] */ const String& srcName);

    CARAPI constructor(
        /* [in] */ INinePatch* patch);

    CARAPI constructor(
        /* [in] */ IResources* res,
        /* [in] */ INinePatch* patch);

    /**
     * The one constructor to rule them all. This is called by all public
     * constructors to set the state and initialize local properties.
     */
    CARAPI constructor(
        /* [in] */ IDrawableConstantState* state,
        /* [in] */ IResources* res,
        /* [in] */ IResourcesTheme* theme);

    virtual CARAPI SetTargetDensity(
        /* [in] */ ICanvas* canvas);

    virtual CARAPI SetTargetDensity(
        /* [in] */ IDisplayMetrics* metrics);

    virtual CARAPI SetTargetDensity(
        /* [in] */ Int32 density);

    //@Override
    CARAPI Draw(
        /* [in] */ ICanvas* canvas);

    //@Override
    CARAPI GetChangingConfigurations(
        /* [out] */ Int32* configuration);

    //@Override
    CARAPI GetPadding(
        /* [in] */ IRect* padding,
        /* [out] */ Boolean* isPadding);

    // @Override
    CARAPI GetOutline(
        /* [in] */ /*@NonNull*/ IOutline* outline);

    /**
     * @hide
     */
    //@Override
    CARAPI GetOpticalInsets(
        /* [out] */ IInsets** insets);

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

    //@Override
    CARAPI SetDither(
        /* [in] */ Boolean dither);

    // @Override
    CARAPI SetAutoMirrored(
        /* [in] */ Boolean mirrored);

    // @Override
    CARAPI IsAutoMirrored(
        /* [out] */ Boolean* mirrored);

    //@Override
    CARAPI SetFilterBitmap(
        /* [in] */ Boolean filter);

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

    virtual CARAPI GetPaint(
        /* [out] */ IPaint** paint);

    //@Override
    CARAPI GetIntrinsicWidth(
        /* [out] */ Int32* width);

    //@Override
    CARAPI GetIntrinsicHeight(
        /* [out] */ Int32* height);

    //@Override
    CARAPI GetMinimumWidth(
        /* [out] */ Int32* width);

    //@Override
    CARAPI GetMinimumHeight(
        /* [out] */ Int32* height);

    //@Override
    CARAPI GetOpacity(
        /* [out] */ Int32* opacity);

    //@Override
    CARAPI GetTransparentRegion(
        /* [out] */ IRegion** bounds);

    //@Override
    CARAPI GetConstantState(
        /* [out] */ IDrawableConstantState** state);

    //@Override
    CARAPI Mutate();

    // @Override
    CARAPI IsStateful(
        /* [out] */ Boolean* isStateful);

protected:
    // @Override
    CARAPI_(Boolean) OnStateChange(
        /* [in] */ const ArrayOf<Int32>& stateSet);

private:
    static CARAPI_(AutoPtr<Insets>) ScaleFromDensity(
        /* [in] */ Insets* insets,
        /* [in] */ Int32 sdensity,
        /* [in] */ Int32 tdensity);

    CARAPI_(void) ComputeBitmapSize();

    CARAPI_(void) SetNinePatch(
        /* [in] */ INinePatch* ninePatch);

    CARAPI_(Boolean) NeedsMirroring();

    /**
     * Updates the constant state from the values in the typed array.
     */
    CARAPI UpdateStateFromTypedArray(
        /* [in] */ ITypedArray* a) /*throws XmlPullParserException*/;

    /**
     * Initializes local dynamic properties from state.
     */
    CARAPI InitializeWithState(
        /* [in] */ NinePatchState* state,
        /* [in] */ IResources* res);

private:
    // dithering helps a lot, and is pretty cheap, so default is true
    static const Boolean DEFAULT_DITHER;
    AutoPtr<NinePatchState> mNinePatchState;
    AutoPtr<INinePatch> mNinePatch;
    AutoPtr<IPorterDuffColorFilter> mTintFilter;
    AutoPtr<IRect> mPadding;
    AutoPtr<Insets> mOpticalInsets;
    AutoPtr<IPaint> mPaint;
    Boolean mMutated;

    Int32 mTargetDensity;

    // These are scaled to match the target density.
    Int32 mBitmapWidth;
    Int32 mBitmapHeight;
};

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GRAPHICS_DRAWABLE_NINEPATCHDRAWABLE_H__
