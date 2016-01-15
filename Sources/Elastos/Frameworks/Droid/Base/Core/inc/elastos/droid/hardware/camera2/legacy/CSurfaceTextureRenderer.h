
#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_CSURFACETEXTURERENDERER_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_CSURFACETEXTURERENDERER_H__

#include "_Elastos_Droid_Hardware_Camera2_Legacy_CSurfaceTextureRenderer.h"
#include "elastos/droid/hardware/camera2/legacy/SurfaceTextureRenderer.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Legacy {

CarClass(CSurfaceTextureRenderer)
    , public SurfaceTextureRenderer
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Legacy
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_CSURFACETEXTURERENDERER_H__
