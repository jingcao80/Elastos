#include "elastos/droid/graphics/CPixelFormatHelper.h"
#include "elastos/droid/graphics/PixelFormat.h"

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_INTERFACE_IMPL(CPixelFormatHelper, Singleton, IPixelFormatHelper)
CAR_SINGLETON_IMPL(CPixelFormatHelper);

ECode CPixelFormatHelper::FormatHasAlpha(
    /* [in] */ Int32 format,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = PixelFormat::FormatHasAlpha(format);
    return NOERROR;
}

ECode CPixelFormatHelper::GetPixelFormatInfo(
    /* [in] */ Int32 format,
    /* [in] */ IPixelFormat* info)
{
    return PixelFormat::GetPixelFormatInfo(format, info);
}

ECode CPixelFormatHelper::IsPublicFormat(
    /* [in] */ Int32 format,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = PixelFormat::IsPublicFormat(format);
    return NOERROR;
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
