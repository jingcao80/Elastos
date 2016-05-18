
#ifndef __ELASTOS_DROID_GRAPHICS_DRAWABLE_SCALEDRAWABLE_H__
#define __ELASTOS_DROID_GRAPHICS_DRAWABLE_SCALEDRAWABLE_H__

#include "elastos/droid/graphics/drawable/Drawable.h"
#include "elastos/droid/graphics/CRect.h"

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

class ScaleDrawable
    : public Drawable
    , public IScaleDrawable
    , public IDrawableCallback
{
public:
    class ScaleState
        : public Drawable::ConstantState
    {
    public:
        ScaleState(
            /* [in] */ ScaleState* orig,
            /* [in] */ ScaleDrawable* owner,
            /* [in] */ IResources* res);

        CARAPI NewDrawable(
            /* [out] */ IDrawable** drawable);

        CARAPI NewDrawable(
            /* [in] */ IResources* res,
            /* [out] */ IDrawable** drawable);

        CARAPI GetChangingConfigurations(
            /* [out] */ Int32* config);

        CARAPI_(Boolean) CanConstantState();

    public:
        AutoPtr<IDrawable> mDrawable;
        Int32 mChangingConfigurations;
        Float mScaleWidth;
        Float mScaleHeight;
        Int32 mGravity;
        Boolean mUseIntrinsicSizeAsMin;

    private:
        Boolean mCheckedConstantState;
        Boolean mCanConstantState;
    };

public:
    CAR_INTERFACE_DECL()

    ScaleDrawable();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IDrawable* drawable,
        /* [in] */ Int32 gravity,
        /* [in] */ Float scaleWidth,
        /* [in] */ Float scaleHeight);

    CARAPI constructor(
        /* [in] */ IDrawableConstantState* state,
        /* [in] */ IResources* res);

    /**
     * Returns the drawable scaled by this ScaleDrawable.
     */
    virtual CARAPI GetDrawable(
        /* [out] */ IDrawable** drawable);

    //@Override
    CARAPI Inflate(
        /* [in] */ IResources* r,
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ IResourcesTheme* theme);

    CARAPI InvalidateDrawable(
        /* [in] */ IDrawable* who);

    CARAPI ScheduleDrawable(
        /* [in] */ IDrawable* who,
        /* [in] */ IRunnable* what,
        /* [in] */ Int64 when);

    CARAPI UnscheduleDrawable(
        /* [in] */ IDrawable* who,
        /* [in] */ IRunnable* what);

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

    //@Override
    CARAPI SetVisible(
        /* [in] */ Boolean visible,
        /* [in] */ Boolean restart,
        /* [out] */ Boolean* isDifferent);

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

protected:
    //@Override
    CARAPI_(Boolean) OnStateChange(
        /* [in] */ ArrayOf<Int32>* state);

    //@Override
    CARAPI_(Boolean) OnLevelChange(
        /* [in] */ Int32 level);

    //@Override
    CARAPI_(void) OnBoundsChange(
        /* [in] */ IRect* bounds);

private:
    static CARAPI_(Float) GetPercent(
        /* [in] */ ITypedArray* a,
        /* [in] */ Int32 name);

private:
    AutoPtr<ScaleState> mScaleState;
    Boolean mMutated;
    AutoPtr<IRect> mTmpRect;
};

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GRAPHICS_DRAWABLE_SCALEDRAWABLE_H__
