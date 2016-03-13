
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/CPixelXorXfermode.h"
#include <skia/effects/SkPixelXorXfermode.h>

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_OBJECT_IMPL(CPixelXorXfermode);
CAR_INTERFACE_IMPL(CPixelXorXfermode, Xfermode, IPixelXorXfermode);
ECode CPixelXorXfermode::constructor(
    /* [in] */ Int32 opColor)
{
    mNativeInstance = NativeCreate(opColor);
    return NOERROR;
}

Int64 CPixelXorXfermode::NativeCreate(
    /* [in] */ Int32 opColor)
{
    return reinterpret_cast<Int64>(SkPixelXorXfermode::Create(opColor));
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
