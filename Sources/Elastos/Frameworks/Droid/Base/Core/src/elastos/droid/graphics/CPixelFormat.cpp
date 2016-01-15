
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/CPixelFormat.h"

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_OBJECT_IMPL(CPixelFormat);
ECode CPixelFormat::FormatHasAlpha(
    /* [in] */ Int32 format,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = PixelFormat::FormatHasAlpha(format);
    return NOERROR;
}

ECode CPixelFormat::GetPixelFormatInfo(
    /* [in] */ Int32 format,
    /* [in] */ IPixelFormat* info)
{
    return PixelFormat::GetPixelFormatInfo(format, info);
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
