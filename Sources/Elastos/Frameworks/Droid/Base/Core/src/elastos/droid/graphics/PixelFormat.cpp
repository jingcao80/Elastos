
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/PixelFormat.h"
#include "elastos/droid/graphics/CPixelFormat.h"
#include <ui/PixelFormat.h>

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_INTERFACE_IMPL(PixelFormat, Object, IPixelFormat);
Boolean PixelFormat::FormatHasAlpha(
    /* [in] */ Int32 format)
{
    switch (format) {
        case IPixelFormat::A_8:
        case IPixelFormat::LA_88:
        case IPixelFormat::RGBA_4444:
        case IPixelFormat::RGBA_5551:
        case IPixelFormat::RGBA_8888:
        case IPixelFormat::TRANSLUCENT:
        case IPixelFormat::TRANSPARENT:
            return TRUE;
    }
    return FALSE;
}

ECode PixelFormat::GetPixelFormatInfo(
    /* [in] */ Int32 format,
    /* [in] */ IPixelFormat* infoObj)
{
    PixelFormat* info = (PixelFormat*)infoObj;
    switch (format) {
        case RGBA_8888:
        case RGBX_8888:
            info->mBitsPerPixel = 32;
            info->mBytesPerPixel = 4;
            break;
        case RGB_888:
            info->mBitsPerPixel = 24;
            info->mBytesPerPixel = 3;
            break;
        case RGB_565:
        case RGBA_5551:
        case RGBA_4444:
        case LA_88:
            info->mBitsPerPixel = 16;
            info->mBytesPerPixel = 2;
            break;
        case A_8:
        case L_8:
        case RGB_332:
            info->mBitsPerPixel = 8;
            info->mBytesPerPixel = 1;
            break;
        case YCbCr_422_SP:
        case YCbCr_422_I:
            info->mBitsPerPixel = 16;
            info->mBytesPerPixel = 1;
            break;
        case YCbCr_420_SP:
            info->mBitsPerPixel = 12;
            info->mBytesPerPixel = 1;
            break;
        default:
            // throw new IllegalArgumentException("unknown pixel format " + format);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

Boolean PixelFormat::IsPublicFormat(
    /* [in] */ Int32 format)
{
    switch (format) {
        case RGBA_8888:
        case RGBX_8888:
        case RGB_888:
        case RGB_565:
            return TRUE;
    }

    return FALSE;
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
