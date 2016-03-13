
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/CCornerPathEffect.h"
#include <skia/effects/SkCornerPathEffect.h>

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_OBJECT_IMPL(CCornerPathEffect);
CAR_INTERFACE_IMPL(CCornerPathEffect, PathEffect, ICornerPathEffect);
ECode CCornerPathEffect::constructor(
    /* [in] */ Float radius)
{
    mNativeInstance = NativeCreate(radius);
    return NOERROR;
}

Int64 CCornerPathEffect::NativeCreate(
    /* [in] */ Float radius)
{
    SkPathEffect* effect = SkCornerPathEffect::Create(radius);
    return reinterpret_cast<Int64>(effect);
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

