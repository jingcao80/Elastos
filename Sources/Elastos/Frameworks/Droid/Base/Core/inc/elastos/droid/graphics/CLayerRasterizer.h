
#ifndef __ELASTOS_DROID_GRAPHICS_CLAYERRASTERIZER_H__
#define __ELASTOS_DROID_GRAPHICS_CLAYERRASTERIZER_H__

#include "_Elastos_Droid_Graphics_CLayerRasterizer.h"
#include "elastos/droid/graphics/Rasterizer.h"

namespace Elastos {
namespace Droid {
namespace Graphics {

CarClass(CLayerRasterizer)
    , public Rasterizer
    , public ILayerRasterizer
{
public:
    CAR_OBJECT_DECL();

    CAR_INTERFACE_DECL();

    CARAPI constructor();

    /**
     * Add a new layer (above any previous layers) to the rasterizer.
     * The layer will extract those fields that affect the mask from
     * the specified paint, but will not retain a reference to the paint
     * object itself, so it may be reused without danger of side-effects.
     */
    CARAPI AddLayer(
        /* [in] */ IPaint* paint,
        /* [in] */ Float dx,
        /* [in] */ Float dy);

    CARAPI AddLayer(
        /* [in] */ IPaint* paint);

private:
    static CARAPI_(Int64) NativeConstructor();

    static CARAPI_(void) NativeAddLayer(
        /* [in] */ Int64 nativeLayer,
        /* [in] */ Int64 nativePaint,
        /* [in] */ Float dx,
        /* [in] */ Float dy);
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GRAPHICS_CLAYERRASTERIZER_H__
