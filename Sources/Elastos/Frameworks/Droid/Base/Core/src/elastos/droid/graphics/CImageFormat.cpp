
#include "elastos/droid/graphics/CImageFormat.h"

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_SINGLETON_IMPL(CImageFormat);
CAR_INTERFACE_IMPL(CImageFormat, Singleton, IImageFormat);
ECode CImageFormat::GetBitsPerPixel(
    /* [in] */ Int32 format,
    /* [out] */ Int32* bits)
{
    VALIDATE_NOT_NULL(bits);
    switch (format) {
        case IImageFormat::RGB_565:
        case IImageFormat::NV16:
        case IImageFormat::YUY2:
        case IImageFormat::Y16:
        case IImageFormat::RAW_SENSOR: {
            *bits = 16;
            return NOERROR;
        }
        case IImageFormat::Y8: {
            *bits = 8;
            return NOERROR;
        }
        case IImageFormat::YV12:
        case IImageFormat::NV21:
        case IImageFormat::YUV_420_888: {
            *bits = 12;
            return NOERROR;
        }
        case IImageFormat::RAW10: {
            *bits = 10;
            return NOERROR;
        }
    }

    *bits = -1;
    return NOERROR;
}

ECode CImageFormat::IsPublicFormat(
    /* [in] */ Int32 format,
    /* [out] */ Boolean* is)
{
    VALIDATE_NOT_NULL(is);
    switch (format) {
        case RGB_565:
        case NV16:
        case YUY2:
        case YV12:
        case JPEG:
        case NV21:
        case YUV_420_888:
        case RAW_SENSOR:
        case RAW10: {
            *is = TRUE;
            return NOERROR;
        }
    }

    *is = FALSE;
    return NOERROR;
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos