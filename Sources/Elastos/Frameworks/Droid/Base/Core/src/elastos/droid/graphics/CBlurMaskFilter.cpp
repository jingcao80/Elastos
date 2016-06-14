
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/CBlurMaskFilter.h"
#include <elastos/utility/logging/Slogger.h>
#include <skia/effects/SkBlurMask.h>
#include <skia/effects/SkBlurMaskFilter.h>

using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_OBJECT_IMPL(CBlurMaskFilter);

CAR_INTERFACE_IMPL(CBlurMaskFilter, MaskFilter, IBlurMaskFilter);

ECode CBlurMaskFilter::constructor(
    /* [in] */ Float radius,
    /* [in] */ Int32 style)
{
    return NativeConstructor(radius, style, &mNativeInstance);
}

ECode CBlurMaskFilter::NativeConstructor(
    /* [in] */ Float radius,
    /* [in] */ Int32 blurStyle,
    /* [out] */ Int64* outfilter)
{
    VALIDATE_NOT_NULL(outfilter)
    *outfilter = 0;

    SkScalar sigma = SkBlurMask::ConvertRadiusToSigma(radius);
    SkMaskFilter* filter = SkBlurMaskFilter::Create((SkBlurStyle)blurStyle, sigma);
    if (filter == NULL) {
        Slogger::E("CBlurMaskFilter", "IllegalArgumentException: radius:%.2f, blurStyle:%d, sigma: %d",
            radius, blurStyle, sigma);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    *outfilter = reinterpret_cast<Int64>(filter);
    return NOERROR;
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
