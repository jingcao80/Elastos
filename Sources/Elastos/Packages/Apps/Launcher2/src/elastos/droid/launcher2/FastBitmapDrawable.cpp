
#include "elastos/droid/launcher2/FastBitmapDrawable.h"
#include "Elastos.Droid.Service.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_INTERFACE_IMPL(FastBitmapDrawable, Drawable, IFastBitmapDrawable);

FastBitmapDrawable::FastBitmapDrawable()
    : mAlpha(0)
    , mWidth(0)
    , mHeight(0)
{
}

ECode FastBitmapDrawable::constructor(
    /* [in] */ IBitmap* b)
{
Slogger::E("FastBitmapDrawable", "===================FastBitmapDrawable::constructor");
    CPaint::New(IPaint::FILTER_BITMAP_FLAG, (IPaint**)&mPaint);

    mAlpha = 255;
    mBitmap = b;
    if (b != NULL) {
        mBitmap->GetWidth(&mWidth);
        mBitmap->GetHeight(&mHeight);
    }
    else {
        mWidth = mHeight = 0;
    }
Slogger::E("FastBitmapDrawable", "===================FastBitmapDrawable::constructor mWidth=%d, mHeight=%d",mWidth,mHeight);
    return NOERROR;
}

ECode FastBitmapDrawable::Draw(
    /* [in] */ ICanvas* canvas)
{
    AutoPtr<IRect> r;
    GetBounds((IRect**)&r);
    // Draw the bitmap into the bounding rect
Slogger::E("FastBitmapDrawable", "===================FastBitmapDrawable::Draw");
    return canvas->DrawBitmap(mBitmap, NULL, r, mPaint);
}

ECode FastBitmapDrawable::SetColorFilter(
    /* [in] */ IColorFilter* cf)
{
    return mPaint->SetColorFilter(cf);
}

ECode FastBitmapDrawable::GetOpacity(
    /* [out] */ Int32* opacity)
{
    VALIDATE_NOT_NULL(opacity);

    *opacity = IPixelFormat::TRANSLUCENT;
    return NOERROR;
}

ECode FastBitmapDrawable::SetAlpha(
    /* [in] */ Int32 alpha)
{
    mAlpha = alpha;
    return mPaint->SetAlpha(alpha);
}

ECode FastBitmapDrawable::SetFilterBitmap(
    /* [in] */ Boolean filterBitmap)
{
    return mPaint->SetFilterBitmap(filterBitmap);
}

ECode FastBitmapDrawable::GetAlpha(
    /* [out] */ Int32* alpha)
{
    VALIDATE_NOT_NULL(alpha);

    *alpha = mAlpha;
    return NOERROR;
}

ECode FastBitmapDrawable::GetIntrinsicWidth(
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width);

    *width = mWidth;
    return NOERROR;
}

ECode FastBitmapDrawable::GetIntrinsicHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height);

    *height = mHeight;
    return NOERROR;
}

ECode FastBitmapDrawable::GetMinimumWidth(
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width);

    *width = mWidth;
    return NOERROR;
}

ECode FastBitmapDrawable::GetMinimumHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height);

    *height = mHeight;
    return NOERROR;
}

ECode FastBitmapDrawable::SetBitmap(
    /* [in] */ IBitmap* b)
{
    mBitmap = b;
    if (b != NULL) {
        mBitmap->GetWidth(&mWidth);
        mBitmap->GetHeight(&mHeight);
    }
    else {
        mWidth = mHeight = 0;
    }
    return NOERROR;
}

ECode FastBitmapDrawable::GetBitmap(
    /* [out] */ IBitmap** map)
{
    VALIDATE_NOT_NULL(map);

    *map = mBitmap;
    REFCOUNT_ADD(*map);
    return NOERROR;
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos