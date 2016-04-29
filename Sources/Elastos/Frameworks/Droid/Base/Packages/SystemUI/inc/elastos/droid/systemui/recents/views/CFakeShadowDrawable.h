
#ifndef  __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_FAKESHADOWDRAWABLE_H__
#define  __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_FAKESHADOWDRAWABLE_H__

#include "_Elastos_Droid_SystemUI_Recents_Views_CFakeShadowDrawable.h"
#include <elastos/droid/graphics/drawable/Drawable.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IColorFilter;
using Elastos::Droid::Graphics::ILinearGradient;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::IPath;
using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Droid::Graphics::IRadialGradient;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::IRectF;
using Elastos::Droid::Graphics::IShader;
using Elastos::Droid::Graphics::Drawable::Drawable;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Views {

/**
 * A rounded rectangle drawable which also includes a shadow around. This is mostly copied from
 * frameworks/support/v7/cardview/eclair-mr1/android/support/v7/widget/
 * RoundRectDrawableWithShadow.java revision c42ba8c000d1e6ce85e152dfc17089a0a69e739f with a few
 * modifications to suit our needs in SystemUI.
 */
CarClass(CFakeShadowDrawable), public Drawable
{
public:
    CAR_OBJECT_DECL();

    CFakeShadowDrawable();

    CARAPI constructor(
        /* [in] */ IResources* resources,
        /* [in] */ IInterface* config);

    // @Override
    CARAPI SetAlpha(
        /* [in] */ Int32 alpha);

    CARAPI SetShadowSize(
        /* [in] */ Float shadowSize,
        /* [in] */ Float maxShadowSize);

    // @Override
    CARAPI GetPadding(
        /* [in] */ IRect* padding,
        /* [out] */ Boolean* result);

    static CARAPI_(Float) CalculateVerticalPadding(
        /* [in] */ Float maxShadowSize,
        /* [in] */ Float cornerRadius,
        /* [in] */ Boolean addPaddingForCorners);

    static CARAPI_(Float) CalculateHorizontalPadding(
        /* [in] */ Float maxShadowSize,
        /* [in] */ Float cornerRadius,
        /* [in] */ Boolean addPaddingForCorners);

    // @Override
    CARAPI SetColorFilter(
        /* [in] */ IColorFilter* cf);

    // @Override
    CARAPI GetOpacity(
        /* [out] */ Int32* opacity);

    CARAPI Draw(
        /* [in] */ ICanvas* canvas);

    CARAPI_(Float) GetMinWidth();

    CARAPI_(Float) GetMinHeight();

protected:
    // @Override
    CARAPI_(void) OnBoundsChange(
        /* [in] */ IRect* bounds);

private:
    CARAPI_(void) DrawShadow(
        /* [in] */ ICanvas* canvas);

    CARAPI_(void) BuildShadowCorners();

    CARAPI_(void) BuildComponents(
        /* [in] */ IRect* bounds);

public:
    // used to calculate content padding
    static const Double COS_45;

    static const Float SHADOW_MULTIPLIER = 1.5f;

    Float mInsetShadow; // extra shadow to avoid gaps between card and shadow

    AutoPtr<IPaint> mCornerShadowPaint;

    AutoPtr<IPaint> mEdgeShadowPaint;

    AutoPtr<IRectF> mCardBounds;

    Float mCornerRadius;

    AutoPtr<IPath> mCornerShadowPath;

    // updated value with inset
    Float mMaxShadowSize;

    // actual value set by developer
    Float mRawMaxShadowSize;

    // multiplied value to account for shadow offset
    Float mShadowSize;

    // actual value set by developer
    Float mRawShadowSize;

private:
    Boolean mDirty;

    Int32 mShadowStartColor;

    Int32 mShadowEndColor;

    Boolean mAddPaddingForCorners;

    /**
     * If shadow size is set to a value above max shadow, we print a warning
     */
    Boolean mPrintedShadowClipWarning;
};

} // namespace Views
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_FAKESHADOWDRAWABLE_H__
