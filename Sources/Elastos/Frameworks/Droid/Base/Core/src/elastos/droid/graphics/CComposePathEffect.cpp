
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/CComposePathEffect.h"
#include <SkPathEffect.h>

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_OBJECT_IMPL(CComposePathEffect);
CAR_INTERFACE_IMPL(CComposePathEffect, PathEffect, IComposePathEffect);
ECode CComposePathEffect::constructor(
    /* [in] */ IPathEffect* outerpe,
    /* [in] */ IPathEffect* innerpe)
{
    mNativeInstance = NativeCreate(
            ((PathEffect*)outerpe)->mNativeInstance,
            ((PathEffect*)innerpe)->mNativeInstance);
    return NOERROR;
}

Int64 CComposePathEffect::NativeCreate(
    /* [in] */ Int64 outerHandle,
    /* [in] */ Int64 innerHandle)
{
    SkPathEffect* outer = reinterpret_cast<SkPathEffect*>(outerHandle);
    SkPathEffect* inner = reinterpret_cast<SkPathEffect*>(innerHandle);
    SkPathEffect* effect = SkComposePathEffect::Create(outer, inner);
    return reinterpret_cast<Int64>(effect);
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
