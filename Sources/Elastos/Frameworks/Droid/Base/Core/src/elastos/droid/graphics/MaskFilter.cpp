
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/MaskFilter.h"
#include <SkMaskFilter.h>

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_INTERFACE_IMPL(MaskFilter, Object, IMaskFilter);
MaskFilter::~MaskFilter()
{
    NativeDestructor(mNativeInstance);
}

void MaskFilter::NativeDestructor(
    /* [in] */ Int64 nativeInstance)
{
    SkMaskFilter* obj = reinterpret_cast<SkMaskFilter*>(nativeInstance);
    SkSafeUnref(obj);
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
