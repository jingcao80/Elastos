
#ifndef __ELASTOS_DROID_GRAPHICS_DRAWABLE_INSETDRAWABLE_H__
#define __ELASTOS_DROID_GRAPHICS_DRAWABLE_INSETDRAWABLE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/drawable/Drawable.h"
#include "elastos/droid/graphics/CRect.h"

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

class InsetDrawable
    : public Drawable
    , public IInsetDrawable
    , public IDrawableCallback
{
public:
    class InsetState
        : public Drawable::ConstantState
    {
    public:
        InsetState(
            /* [in] */ InsetState* orig,
            /* [in] */ InsetDrawable* owner,
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
        AutoPtr<ArrayOf<Int32> > mThemeAttrs;
        AutoPtr<IDrawable> mDrawable;
        Int32 mChangingConfigurations;

        Int32 mInsetLeft;
        Int32 mInsetTop;
        Int32 mInsetRight;
        Int32 mInsetBottom;

        Boolean mCheckedConstantState;
        Boolean mCanConstantState;
    };

public:
    CAR_INTERFACE_DECL();

    InsetDrawable();

    InsetDrawable(
        /* [in] */ IDrawable* drawable,
        /* [in] */ Int32 inset);

    InsetDrawable(
        /* [in] */ IDrawable* drawable,
        /* [in] */ Int32 insetLeft,
        /* [in] */ Int32 insetTop,
        /* [in] */ Int32 insetRight,
        /* [in] */ Int32 insetBottom);

    // @Override
    CARAPI Inflate(
        /* [in] */ IResources* r,
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ IResourcesTheme* theme) /*throws XmlPullParserException, IOException*/;

    // @Override
    CARAPI ApplyTheme(
        /* [in] */ IResourcesTheme* t);

    // @Override
    CARAPI CanApplyTheme(
        /* [out] */ Boolean* can);

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

    /** @hide */
    // @Override
    CARAPI GetOpticalInsets(
        /* [out] */ IInsets** sets);

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

    //@Override
    CARAPI SetVisible(
        /* [in] */ Boolean visible,
        /* [in] */ Boolean restart,
        /* [out] */ Boolean* isDifferent);

    CARAPI SetAlpha(
        /* [in] */ Int32 alpha);

    // @Override
    CARAPI GetAlpha(
        /* [out] */ Int32* alpha);

    CARAPI SetColorFilter(
        /* [in] */ IColorFilter* cf);

    // @Override
    CARAPI SetTintList(
        /* [in] */ IColorStateList* tint);

    // @Override
    CARAPI SetTintMode(
        /* [in] */ PorterDuffMode tintMode);

    /** {@hide} */
    // @Override
    CARAPI SetLayoutDirection(
        /* [in] */ Int32 layoutDirection);

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

    // @Override
    CARAPI GetOutline(
        /* [in] */ /*@NonNull*/ IOutline* outline);

    //@Override
    CARAPI GetConstantState(
        /* [out] */ IDrawableConstantState** state);

    //@Override
    CARAPI Mutate(
        /* [out] */ IDrawable** drawable);

    virtual CARAPI GetDrawable(
        /* [out] */ IDrawable** drawable);

protected:
    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IDrawable* drawable,
        /* [in] */ Int32 inset);

    CARAPI constructor(
        /* [in] */ IDrawable* drawable,
        /* [in] */ Int32 insetLeft,
        /* [in] */ Int32 insetTop,
        /* [in] */ Int32 insetRight,
        /* [in] */ Int32 insetBottom);

    CARAPI constructor(
        /* [in] */ InsetState* state,
        /* [in] */ IResources* res);

    //@Override
    CARAPI_(Boolean) OnStateChange(
        /* [in] */ const ArrayOf<Int32>* state);

    // @Override
    CARAPI_(Boolean) OnLevelChange(
        /* [in] */ Int32 level);

    //@Override
    CARAPI_(void) OnBoundsChange(
        /* [in] */ IRect* bounds);

private:
    InsetDrawable(
        /* [in] */ InsetState* state,
        /* [in] */ IResources* res);

    CARAPI VerifyRequiredAttributes(
        /* [in] */ ITypedArray* a) /*throws XmlPullParserException*/;

    CARAPI UpdateStateFromTypedArray(
        /* [in] */ ITypedArray* a) /*throws XmlPullParserException*/;

private:
    AutoPtr<InsetState> mInsetState;
    AutoPtr<CRect> mTmpRect;
    Boolean mMutated;
};

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GRAPHICS_DRAWABLE_INSETDRAWABLE_H__
