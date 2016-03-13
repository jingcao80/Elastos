
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/CDashPathEffect.h"
#include <SkDashPathEffect.h>
#include <SkTemplates.h>

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_OBJECT_IMPL(CDashPathEffect);
CAR_INTERFACE_IMPL(CDashPathEffect, PathEffect, IDashPathEffect);
ECode CDashPathEffect::constructor(
    /* [in] */ const ArrayOf<Float>& intervals,
    /* [in] */ Float phase)
{
    if (intervals.GetLength() < 2) {
//        throw new ArrayIndexOutOfBoundsException();
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    mNativeInstance = NativeCreate(intervals, phase);
    return NOERROR;
}

Int64 CDashPathEffect::NativeCreate(
    /* [in] */ const ArrayOf<Float>& intervalArray,
    /* [in] */ Float phase)
{
    int         count = intervalArray.GetLength() & ~1;  // even number
#ifdef SK_SCALAR_IS_FLOAT
    SkScalar*   intervals = intervalArray.GetPayload();
#else
    #error Need to convert float array to SkScalar array before calling the following function.
#endif
    SkPathEffect* effect = SkDashPathEffect::Create(intervals, count, phase);
    return reinterpret_cast<Int64>(effect);
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
