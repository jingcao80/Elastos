#include "elastos/droid/app/FastBitmapDrawable.h"
#include "elastos/droid/graphics/CPaint.h"
#include "elastos/droid/graphics/CPorterDuffXfermode.h"

using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::IXfermode;
using Elastos::Droid::Graphics::IPorterDuffXfermode;
using Elastos::Droid::Graphics::CPorterDuffXfermode;
using Elastos::Droid::Graphics::IPixelFormat;

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL(FastBitmapDrawable, Elastos::Droid::Graphics::Drawable::Drawable, IFastBitmapDrawable)

FastBitmapDrawable::FastBitmapDrawable()
    : mWidth(0)
    , mHeight(0)
    , mDrawLeft(0)
    , mDrawTop(0)
{
}

ECode FastBitmapDrawable::constructor(
    /* [in] */ IBitmap* bitmap)
{
    mBitmap = bitmap;
    bitmap->GetWidth(&mWidth);
    bitmap->GetHeight(&mHeight);

    SetBounds(0, 0, mWidth, mHeight);

    ASSERT_SUCCEEDED(CPaint::New((IPaint**)&mPaint));
    AutoPtr<IPorterDuffXfermode> mode;
    ASSERT_SUCCEEDED(CPorterDuffXfermode::New(Elastos::Droid::Graphics::PorterDuffMode_SRC,
            (IPorterDuffXfermode**)&mode));
    mPaint->SetXfermode(IXfermode::Probe(mode));
    return NOERROR;
}

ECode FastBitmapDrawable::Draw(
    /* [in] */ ICanvas* canvas)
{
    return canvas->DrawBitmap(mBitmap, (Float)mDrawLeft, (Float)mDrawTop, mPaint);
}

ECode FastBitmapDrawable::GetOpacity(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = IPixelFormat::OPAQUE;
    return NOERROR;
}

ECode FastBitmapDrawable::SetBounds(
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    mDrawLeft = left + (right-left - mWidth) / 2;
    mDrawTop = top + (bottom-top - mHeight) / 2;
    return NOERROR;
}

ECode FastBitmapDrawable::GetIntrinsicWidth(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mWidth;
    return NOERROR;
}

ECode FastBitmapDrawable::GetIntrinsicHeight(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mHeight;
    return NOERROR;
}

ECode FastBitmapDrawable::GetMinimumWidth(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mWidth;
    return NOERROR;
}

ECode FastBitmapDrawable::GetMinimumHeight(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mHeight;
    return NOERROR;
}

} // namespace App
} // namespace Droid
} // namespace Elastos
