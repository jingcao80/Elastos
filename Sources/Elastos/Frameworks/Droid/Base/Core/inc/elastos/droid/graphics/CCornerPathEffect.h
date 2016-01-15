
#ifndef __ELASTOS_DROID_GRAPHICS_CCORNERPATHEFFECT_H__
#define __ELASTOS_DROID_GRAPHICS_CCORNERPATHEFFECT_H__

#include "_Elastos_Droid_Graphics_CCornerPathEffect.h"
#include "elastos/droid/graphics/PathEffect.h"

namespace Elastos {
namespace Droid {
namespace Graphics {

CarClass(CCornerPathEffect)
    , public PathEffect
    , public ICornerPathEffect
{
public:
    CAR_OBJECT_DECL();

    CAR_INTERFACE_DECL();

    /**
     * Transforms geometries that are drawn (either STROKE or FILL styles) by
     * replacing any sharp angles between line segments into rounded angles of
     * the specified radius.
     * @param radius Amount to round sharp angles between line segments.
     */
    CARAPI constructor(
        /* [in] */ Float radius);

private:
    static CARAPI_(Int64) NativeCreate(
        /* [in] */ Float radius);
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GRAPHICS_CCORNERPATHEFFECT_H__
