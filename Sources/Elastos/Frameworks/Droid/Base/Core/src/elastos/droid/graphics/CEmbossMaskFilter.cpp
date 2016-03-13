
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/CEmbossMaskFilter.h"
#include <skia/effects/SkBlurMaskFilter.h>
#include <skia/effects/SkBlurMask.h>

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_OBJECT_IMPL(CEmbossMaskFilter);
CAR_INTERFACE_IMPL(CEmbossMaskFilter, MaskFilter, IEmbossMaskFilter);
ECode CEmbossMaskFilter::constructor(
    /* [in] */ const ArrayOf<Float>& direction,
    /* [in] */ Float ambient,
    /* [in] */ Float specular,
    /* [in] */ Float blurRadius)
{
    if (direction.GetLength() < 3) {
        // throw new ArrayIndexOutOfBoundsException();
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    mNativeInstance = NativeConstructor(direction, ambient, specular, blurRadius);
    return NOERROR;
}

Int64 CEmbossMaskFilter::NativeConstructor(
    /* [in] */ const ArrayOf<Float>& dirArray,
    /* [in] */ Float ambient,
    /* [in] */ Float specular,
    /* [in] */ Float blurRadius)
{
    SkScalar direction[3];

    float* values = dirArray.GetPayload();
    for (int i = 0; i < 3; i++) {
        direction[i] = values[i];
    }

    SkScalar sigma = SkBlurMask::ConvertRadiusToSigma(blurRadius);
    SkMaskFilter* filter =  SkBlurMaskFilter::CreateEmboss(sigma,
            direction, ambient, specular);
    assert(filter != NULL);
    return reinterpret_cast<Int64>(filter);
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
