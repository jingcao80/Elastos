
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/DrawFilter.h"
#include <skia/core/SkDrawFilter.h>

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_INTERFACE_IMPL(DrawFilter, Object, IDrawFilter);
DrawFilter::~DrawFilter()
{
    NativeDestructor(mNativeInstance);
}

void DrawFilter::NativeDestructor(
    /* [in] */ Int64 objHandle)
{
    SkDrawFilter* obj = reinterpret_cast<SkDrawFilter*>(objHandle);
    SkSafeUnref(obj);
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
