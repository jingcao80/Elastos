
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/CPathDashPathEffect.h"
#include "elastos/droid/graphics/CPath.h"
#include <skia/effects/Sk1DPathEffect.h>
#include <skia/core/SkPath.h>

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_OBJECT_IMPL(CPathDashPathEffect);
CAR_INTERFACE_IMPL(CPathDashPathEffect, PathEffect, IPathDashPathEffect);
ECode CPathDashPathEffect::constructor(
    /* [in] */ IPath* shape,
    /* [in] */ Float advance,
    /* [in] */ Float phase,
    /* [in] */ PathDashPathEffectStyle style)
{
    mNativeInstance = NativeCreate(
                        ((CPath*)shape)->mNativePath,
                        advance,
                        phase,
                        style);
    return NOERROR;
}

Int64 CPathDashPathEffect::NativeCreate(
    /* [in] */ Int64 nativePath,
    /* [in] */ Float advance,
    /* [in] */ Float phase,
    /* [in] */ Int32 nativeStyle)
{
    const SkPath* shape = reinterpret_cast<SkPath*>(nativePath);
    SkASSERT(shape != NULL);
    SkPathEffect* effect = SkPath1DPathEffect::Create(*shape, advance, phase,
            (SkPath1DPathEffect::Style)nativeStyle);
    return reinterpret_cast<Int64>(effect);
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
