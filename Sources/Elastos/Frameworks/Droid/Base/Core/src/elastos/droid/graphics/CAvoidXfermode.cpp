
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/CAvoidXfermode.h"
#include <SkAvoidXfermode.h>
#include <SkPixelXorXfermode.h>

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_OBJECT_IMPL(CAvoidXfermode);

CAR_INTERFACE_IMPL(CAvoidXfermode, Xfermode, IAvoidXfermode);

ECode CAvoidXfermode::constructor(
    /* [in] */ Int32 opColor,
    /* [in] */ Int32 tolerance,
    /* [in] */ Int32 mode)
{
    if (tolerance < 0 || tolerance > 255) {
        // throw new IllegalArgumentException("tolerance must be 0..255");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mNativeInstance = NativeCreate(opColor, tolerance, mode);
    return NOERROR;
}

Int64 CAvoidXfermode::NativeCreate(
    /* [in] */ Int32 opColor,
    /* [in] */ Int32 tolerance,
    /* [in] */ Int32 nativeMode)
{
    SkAvoidXfermode::Mode mode = static_cast<SkAvoidXfermode::Mode>(nativeMode);
    return reinterpret_cast<Int64>(SkAvoidXfermode::Create(opColor, tolerance, mode));
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
