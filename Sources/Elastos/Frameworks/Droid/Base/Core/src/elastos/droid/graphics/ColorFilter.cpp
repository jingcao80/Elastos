
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/ColorFilter.h"
#include <SkColorFilter.h>

namespace Elastos {
namespace Droid {
namespace Graphics {

// {2C6350C4-CC6F-4B0C-AB2D-B43F37E35194}
extern const InterfaceID EIID_ColorFilter =
    { 0x2c6350c4, 0xcc6f, 0x4b0c, { 0xab, 0x2d, 0xb4, 0x3f, 0x37, 0xe3, 0x51, 0x94 } };

CAR_INTERFACE_IMPL(ColorFilter, Object, IColorFilter);
ColorFilter::~ColorFilter()
{
    DestroyFilter(mNativeInstance);
}

void ColorFilter::DestroyFilter(
    /* [in] */ Int64 native_instance)
{
    SkColorFilter* filter = reinterpret_cast<SkColorFilter *>(native_instance);
    if (filter) SkSafeUnref(filter);
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
