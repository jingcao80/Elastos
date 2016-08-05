
#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_KEYGUARDCIRCLEFRAMEDDRAWABLE_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_KEYGUARDCIRCLEFRAMEDDRAWABLE_H__

#include "_Elastos.Droid.SystemUI.h"
#include "elastos/droid/graphics/drawable/Drawable.h"
#include "Elastos.Droid.Graphics.h"

using Elastos::Droid::Graphics::IPath;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::IRectF;
using Elastos::Droid::Graphics::IColorFilter;
using Elastos::Droid::Graphics::Drawable::Drawable;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

class KeyguardCircleFramedDrawable
    : public Drawable
    , public IKeyguardCircleFramedDrawable
{
public:
    TO_STRING_IMPL("KeyguardCircleFramedDrawable")

    CAR_INTERFACE_DECL()

    KeyguardCircleFramedDrawable(
        /* [in] */ IBitmap* bitmap,
        /* [in] */ Int32 size,
        /* [in] */ Int32 frameColor,
        /* [in] */ Float strokeWidth,
        /* [in] */ Int32 frameShadowColor,
        /* [in] */ Float shadowRadius,
        /* [in] */ Int32 highlightColor);

    CARAPI Reset();

    //@Override
    CARAPI Draw(
        /* [in] */ ICanvas* canvas);

    CARAPI SetScale(
        /* [in] */ Float scale);

    CARAPI GetScale(
        /* [out] */ Float* scale);

    CARAPI SetPressed(
        /* [in] */ Boolean pressed);

    //@Override
    CARAPI GetOpacity(
        /* [out] */ Int32* opacity);

    //@Override
    CARAPI SetAlpha(
        /* [in] */ Int32 alpha);

    //@Override
    CARAPI SetColorFilter(
        /* [in] */ IColorFilter* cf);

    CARAPI VerifyParams(
        /* [in] */ Float iconSize,
        /* [in] */ Int32 frameColor,
        /* [in] */ Float stroke,
        /* [in] */ Int32 frameShadowColor,
        /* [in] */ Float shadowRadius,
        /* [in] */ Int32 highlightColor,
        /* [out] */ Boolean* result);

private:
    AutoPtr<IBitmap> mBitmap;
    Int32 mSize;
    AutoPtr<IPaint> mPaint;
    Float mShadowRadius;
    Float mStrokeWidth;
    Int32 mFrameColor;
    Int32 mHighlightColor;
    Int32 mFrameShadowColor;

    Float mScale;
    AutoPtr<IPath> mFramePath;
    AutoPtr<IRect> mSrcRect;
    AutoPtr<IRectF> mDstRect;
    AutoPtr<IRectF> mFrameRect;
    Boolean mPressed;
};

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_KEYGUARD_KEYGUARDCIRCLEFRAMEDDRAWABLE_H__