
#ifndef __ELASTOS_DROID_SETTINGS_DRAWABLE_CIRCLEFRAMEDDRAWABLE_H__
#define __ELASTOS_DROID_SETTINGS_DRAWABLE_CIRCLEFRAMEDDRAWABLE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/drawable/Drawable.h"

using Elastos::Droid::Content::IContext;
// using Elastos::Droid::Graphics::Drawable::Drawable;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IColorFilter;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::IPath;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::IRectF;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Drawable {

/**
 * Converts the user avatar icon to a circularly clipped one.
 * TODO: Move this to an internal framework class and share with the one in Keyguard.
 */
class CircleFramedDrawable
    : public Elastos::Droid::Graphics::Drawable::Drawable
{
public:
    CircleFramedDrawable(
        /* [in] */ IBitmap* icon,
        /* [in] */ Int32 size,
        /* [in] */ Int32 frameColor,
        /* [in] */ Float strokeWidth,
        /* [in] */ Int32 frameShadowColor,
        /* [in] */ Float shadowRadius,
        /* [in] */ Int32 highlightColor);

    ~CircleFramedDrawable();

    static CARAPI_(AutoPtr<CircleFramedDrawable>) GetInstance(
        /* [in] */ IContext* context,
        /* [in] */ IBitmap* icon);

    //@Override
    CARAPI Draw(
        /* [in] */ ICanvas* canvas);
    CARAPI SetScale(
        /* [in] */ Float scale);

    CARAPI_(Float) GetScale();

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

    //@Override
    CARAPI GetIntrinsicWidth(
        /* [out] */ Int32* width);

    //@Override
    CARAPI GetIntrinsicHeight(
        /* [out] */ Int32* height);

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

} // namespace Drawable
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_DRAWABLE_CIRCLEFRAMEDDRAWABLE_H__
