
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/ColorFilter.h"
#include <SkColorFilter.h>

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_INTERFACE_IMPL(ColorFilter, Object, IColorFilter);

ColorFilter::ColorFilter()
    : mNativeInstance(0)
{}

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
