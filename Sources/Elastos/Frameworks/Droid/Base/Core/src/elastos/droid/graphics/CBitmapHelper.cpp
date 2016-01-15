
#include "elastos/droid/graphics/CBitmapHelper.h"
#include "elastos/droid/graphics/CBitmap.h"

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_SINGLETON_IMPL(CBitmapHelper);
CAR_INTERFACE_IMPL(CBitmapHelper, Singleton, IBitmapHelper);
ECode CBitmapHelper::SetDefaultDensity(
    /* [in] */ Int32 density)
{
    CBitmap::SetDefaultDensity(density);
    return NOERROR;
}

ECode CBitmapHelper::GetDefaultDensity(
    /* [out] */ Int32* density)
{
    VALIDATE_NOT_NULL(density);
    *density = CBitmap::GetDefaultDensity();
    return NOERROR;
}

ECode CBitmapHelper::CreateScaledBitmap(
    /* [in] */ IBitmap* src,
    /* [in] */ Int32 dstWidth,
    /* [in] */ Int32 dstHeight,
    /* [in] */ Boolean filter,
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap);
    return CBitmap::CreateScaledBitmap(src, dstWidth, dstHeight, filter, bitmap);
}

ECode CBitmapHelper::CreateBitmap(
    /* [in] */ IBitmap* source,
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap);
    return CBitmap::CreateBitmap(source, bitmap);
}

ECode CBitmapHelper::CreateBitmap(
    /* [in] */ IBitmap* source,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap);
    return CBitmap::CreateBitmap(source, x, y, width, height, bitmap);
}

ECode CBitmapHelper::CreateBitmap(
    /* [in] */ IBitmap* source,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ IMatrix* m,
    /* [in] */ Boolean filter,
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap);
    return CBitmap::CreateBitmap(source, x, y, width, height, m, filter, bitmap);
}

ECode CBitmapHelper::CreateBitmap(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ BitmapConfig config,
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap);
    return CBitmap::CreateBitmap(width, height, config, bitmap);
}

ECode CBitmapHelper::CreateBitmap(
    /* [in] */ ArrayOf<Int32>* colors,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 stride,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ BitmapConfig config,
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap);
    return CBitmap::CreateBitmap(colors, offset, stride, width, height, config, bitmap);
}

ECode CBitmapHelper::CreateBitmap(
    /* [in] */ ArrayOf<Int32>* colors,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ BitmapConfig config,
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap);
    return CBitmap::CreateBitmap(colors, width, height, config, bitmap);
}

ECode CBitmapHelper::CreateBitmap(
    /* [in] */ IDisplayMetrics* display,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ BitmapConfig config,
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap);
    return CBitmap::CreateBitmap(display, width, height, config, bitmap);
}

ECode CBitmapHelper::CreateBitmap(
    /* [in] */ IDisplayMetrics* display,
    /* [in] */ ArrayOf<Int32>* colors,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 stride,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ BitmapConfig config,
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap);
    return CBitmap::CreateBitmap(display, colors, offset, stride, width, height, config, bitmap);
}

ECode CBitmapHelper::CreateBitmap(
    /* [in] */ IDisplayMetrics* display,
    /* [in] */ ArrayOf<Int32>* colors,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ BitmapConfig config,
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap);
    return CBitmap::CreateBitmap(display, colors, width, height, config, bitmap);
}

ECode CBitmapHelper::ScaleFromDensity(
    /* [in] */ Int32 size,
    /* [in] */ Int32 sdensity,
    /* [in] */ Int32 tdensity,
    /* [out] */ Int32* retSize)
{
    VALIDATE_NOT_NULL(retSize);
    *retSize = CBitmap::ScaleFromDensity(size, sdensity, tdensity);
    return NOERROR;
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
