
#ifndef __ELASTOS_DROID_GRAPHICS_CPIXELXORXFERMODE_H__
#define __ELASTOS_DROID_GRAPHICS_CPIXELXORXFERMODE_H__

#include "_Elastos_Droid_Graphics_CPixelXorXfermode.h"
#include "elastos/droid/graphics/Xfermode.h"

namespace Elastos {
namespace Droid {
namespace Graphics {

/**
 * PixelXorXfermode implements a simple pixel xor (op ^ src ^ dst).
 * This transformation does not follow premultiplied conventions, therefore
 * this mode *always* returns an opaque color (alpha == 255). Thus it is
 * not really usefull for operating on blended colors.
 */
CarClass(CPixelXorXfermode)
    , public Xfermode
    , public IPixelXorXfermode
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ Int32 opColor);

private:
    static CARAPI_(Int64) NativeCreate(
        /* [in] */ Int32 opColor);
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GRAPHICS_CPIXELXORXFERMODE_H__
