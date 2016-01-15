
#ifndef __ELASTOS_DROID_GRAPHICS_CDISCRETEPATHEFFECT_H__
#define __ELASTOS_DROID_GRAPHICS_CDISCRETEPATHEFFECT_H__

#include "_Elastos_Droid_Graphics_CDiscretePathEffect.h"
#include "elastos/droid/graphics/PathEffect.h"

namespace Elastos {
namespace Droid {
namespace Graphics {

CarClass(CDiscretePathEffect)
    , public PathEffect
    , public IDiscretePathEffect
{
public:
    CAR_OBJECT_DECL();

    CAR_INTERFACE_DECL();

    /**
     * Chop the path into lines of segmentLength, randomly deviating from the
     * original path by deviation.
     */
    CARAPI constructor(
        /* [in] */ Float segmentLength,
        /* [in] */ Float deviation);

private:
    static CARAPI_(Int64) NativeCreate(
        /* [in] */ Float length,
        /* [in] */ Float deviation);
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GRAPHICS_CDISCRETEPATHEFFECT_H__
