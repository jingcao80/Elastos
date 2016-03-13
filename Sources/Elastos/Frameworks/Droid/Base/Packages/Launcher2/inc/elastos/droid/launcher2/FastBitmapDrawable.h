#ifndef  __ELASTOS_DROID_LAUNCHER2_FASTBITMAPDRAWABLE_H__
#define  __ELASTOS_DROID_LAUNCHER2_FASTBITMAPDRAWABLE_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

class FastBitmapDrawable
    : public Drawable
{
public:
    FastBitmapDrawable(
        /* [in] */ IBitmap* b);

    //@Override
    CARAPI Draw(
        /* [in] */ ICanvas* canvas);

    //@Override
    CARAPI SetColorFilter(
        /* [in] */ IColorFilter* cf);

    //@Override
    CARAPI GetOpacity(
        /* [out] */ Int32* opacity);

    //@Override
    CARAPI SetAlpha(
        /* [in] */ Int32 alpha);

    CARAPI SetFilterBitmap(
        /* [in] */ Boolean filterBitmap);

    CARAPI GetAlpha(
        /* [out] */ Int32* alpha);

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

    CARAPI SetBitmap(
        /* [in] */ IBitmap* b);

    CARAPI GetBitmap(
        /* [out] */ IBitmap** map);

private:
    AutoPtr<IBitmap> mBitmap;
    Int32 mAlpha;
    Int32 mWidth;
    Int32 mHeight;
    AutoPtr<IPaint> mPaint;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_FASTBITMAPDRAWABLE_H__