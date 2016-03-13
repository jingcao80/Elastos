
#ifndef __ELASTOS_DROID_GRAPHICS_CCANVAS_H__
#define __ELASTOS_DROID_GRAPHICS_CCANVAS_H__

#include "_Elastos_Droid_Graphics_CCanvas.h"
#include "elastos/droid/graphics/Canvas.h"

namespace Elastos {
namespace Droid {
namespace Graphics {

CarClass(CCanvas), public Canvas
{
public:
    CAR_OBJECT_DECL();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IBitmap* bitmap);

    CARAPI constructor(
        /* [in] */ Int32 nativeCanvas);
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GRAPHICS_CCANVAS_H__
