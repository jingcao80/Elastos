
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/CBlurMaskFilter.h"
#include <skia/effects/SkBlurMask.h>
#include <skia/effects/SkBlurMaskFilter.h>

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_OBJECT_IMPL(CBlurMaskFilter);
CAR_INTERFACE_IMPL(CBlurMaskFilter, MaskFilter, IBlurMaskFilter);
ECode CBlurMaskFilter::constructor(
    /* [in] */ Float radius,
    /* [in] */ Int32 style)
{
    mNativeInstance = NativeConstructor(radius, style);
    return NOERROR;
}

Int64 CBlurMaskFilter::NativeConstructor(
    /* [in] */ Float radius,
    /* [in] */ Int32 blurStyle)
{
    SkScalar sigma = SkBlurMask::ConvertRadiusToSigma(radius);
    SkMaskFilter* filter = SkBlurMaskFilter::Create((SkBlurStyle)blurStyle, sigma);
    assert(filter);
    return reinterpret_cast<Int64>(filter);
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
