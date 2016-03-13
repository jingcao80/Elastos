
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/CLayerRasterizer.h"
#include "elastos/droid/graphics/Paint.h"

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_OBJECT_IMPL(CLayerRasterizer);
CAR_INTERFACE_IMPL(CLayerRasterizer, Rasterizer, ILayerRasterizer);
ECode CLayerRasterizer::constructor()
{
    mNativeInstance = NativeConstructor();
    return NOERROR;
}

ECode CLayerRasterizer::AddLayer(
    /* [in] */ IPaint* paint,
    /* [in] */ Float dx,
    /* [in] */ Float dy)
{
    NativeAddLayer(mNativeInstance, ((Paint*)paint)->mNativePaint, dx, dy);
    return NOERROR;
}

ECode CLayerRasterizer::AddLayer(
    /* [in] */ IPaint* paint)
{
    NativeAddLayer(mNativeInstance, ((Paint*)paint)->mNativePaint, 0, 0);
    return NOERROR;
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
