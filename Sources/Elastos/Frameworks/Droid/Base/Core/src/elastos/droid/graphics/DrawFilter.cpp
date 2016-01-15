
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/DrawFilter.h"
#include <skia/core/SkDrawFilter.h>

namespace Elastos {
namespace Droid {
namespace Graphics {

// {33362B1D-958F-4EB9-896B-AF55769500FB}
extern const InterfaceID EIID_DrawFilter =
    { 0x33362b1d, 0x958f, 0x4eb9, { 0x89, 0x6b, 0xaf, 0x55, 0x76, 0x95, 0x0, 0xfb } };

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
