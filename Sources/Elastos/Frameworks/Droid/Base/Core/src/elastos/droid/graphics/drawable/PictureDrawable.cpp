
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/drawable/PictureDrawable.h"
#include "elastos/droid/graphics/CRect.h"

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

CAR_INTERFACE_IMPL(PictureDrawable, Drawable, IPictureDrawable);
PictureDrawable::PictureDrawable()
{}

ECode PictureDrawable::constructor(
    /* [in] */ IPicture* picture)
{
    mPicture = picture;
    return NOERROR;
}

ECode PictureDrawable::GetPicture(
    /* [out] */ IPicture** picture)
{
    VALIDATE_NOT_NULL(picture);
    *picture = mPicture;
    REFCOUNT_ADD(*picture);
    return NOERROR;
}

ECode PictureDrawable::SetPicture(
    /* [in] */ IPicture* picture)
{
    mPicture = picture;
    return NOERROR;
}

ECode PictureDrawable::Draw(
    /* [in] */ ICanvas* canvas)
{
    if (mPicture != NULL) {
        AutoPtr<IRect> bounds;
        GetBounds((IRect**)&bounds);
        Int32 count;
        canvas->Save(&count);
        Boolean isNotEmpty;
        canvas->ClipRect(bounds, &isNotEmpty);
        canvas->Translate(
                ((CRect*)bounds.Get())->mLeft, ((CRect*)bounds.Get())->mTop);
        canvas->DrawPicture(mPicture);
        canvas->Restore();
    }
    return NOERROR;
}

ECode PictureDrawable::GetIntrinsicWidth(
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width);
    *width = -1;
    if (mPicture != NULL) mPicture->GetWidth(width);
    return NOERROR;
}

ECode PictureDrawable::GetIntrinsicHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height);
    *height = -1;
    if (mPicture != NULL) mPicture->GetHeight(height);
    return NOERROR;
}

ECode PictureDrawable::GetOpacity(
    /* [out] */ Int32* opacity)
{
    VALIDATE_NOT_NULL(opacity);
    // not sure, so be safe
    *opacity = IPixelFormat::TRANSLUCENT;
    return NOERROR;
}

ECode PictureDrawable::SetFilterBitmap(
    /* [in] */ Boolean filter)
{
    return NOERROR;
}

ECode PictureDrawable::SetDither(
    /* [in] */ Boolean dither)
{
    return NOERROR;
}

ECode PictureDrawable::SetColorFilter(
    /* [in] */ IColorFilter* colorFilter)
{
    return NOERROR;
}

ECode PictureDrawable::SetAlpha(
    /* [in */ Int32 alpha)
{
    return NOERROR;
}

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos
