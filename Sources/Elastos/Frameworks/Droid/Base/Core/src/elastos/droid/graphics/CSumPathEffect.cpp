
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/CSumPathEffect.h"
#include <SkPathEffect.h>

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_OBJECT_IMPL(CSumPathEffect);
CAR_INTERFACE_IMPL(CSumPathEffect, PathEffect, ISumPathEffect);
ECode CSumPathEffect::constructor(
    /* [in] */ IPathEffect* first,
    /* [in] */ IPathEffect* second)
{
    mNativeInstance = NativeCreate((
            (PathEffect*)first)->mNativeInstance,
            ((PathEffect*)second)->mNativeInstance);
    return NOERROR;
}

Int64 CSumPathEffect::NativeCreate(
    /* [in] */ Int64 firstHandle,
    /* [in] */ Int64 secondHandle)
{
    SkPathEffect* first = reinterpret_cast<SkPathEffect*>(firstHandle);
    SkPathEffect* second = reinterpret_cast<SkPathEffect*>(secondHandle);
    SkPathEffect* effect = SkSumPathEffect::Create(first, second);
    return reinterpret_cast<Int64>(effect);
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
