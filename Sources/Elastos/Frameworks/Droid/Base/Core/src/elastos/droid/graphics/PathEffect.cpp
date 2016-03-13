
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/PathEffect.h"
#include <SkPathEffect.h>

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_INTERFACE_IMPL(PathEffect, Object, IPathEffect);
PathEffect::~PathEffect()
{
    NativeDestructor(mNativeInstance);
}

void PathEffect::NativeDestructor(
    /* [in] */ Int64 nativeInstance)
{
    SkPathEffect* obj = reinterpret_cast<SkPathEffect*>(nativeInstance);
    SkSafeUnref(obj);
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
