
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/Rasterizer.h"
#include "elastos/droid/graphics/CLayerRasterizer.h"
#include "elastos/droid/graphics/NativePaint.h"
#include "elastos/droid/graphics/GraphicsNative.h"
#include <skia/core/SkRasterizer.h>
#include <skia/effects/SkLayerRasterizer.h>

namespace Elastos {
namespace Droid {
namespace Graphics {


// Rasterizer.java holds a pointer (jlong) to this guy
class NativeRasterizer {
public:
    NativeRasterizer() {}
    virtual ~NativeRasterizer() {}

    // Can return NULL, or a ref to the skia rasterizer.
    virtual SkRasterizer* refRasterizer() { return NULL; }
};

class NativeLayerRasterizer : public NativeRasterizer {
public:
    SkLayerRasterizer::Builder fBuilder;

    virtual SkRasterizer* refRasterizer() {
        return fBuilder.snapshotRasterizer();
    }
};

SkRasterizer* GraphicsNative::RefNativeRasterizer(
    /* [in] */ Int64 rasterizerHandle)
{
    NativeRasterizer* nr = reinterpret_cast<NativeRasterizer*>(rasterizerHandle);
    return nr ? nr->refRasterizer() : NULL;
}

// {2D3A6B44-0939-4A44-A9C2-6C1E9E71D8FC}
extern const InterfaceID EIID_Rasterizer =
{ 0x2d3a6b44, 0x939, 0x4a44, { 0xa9, 0xc2, 0x6c, 0x1e, 0x9e, 0x71, 0xd8, 0xfc } };

CAR_INTERFACE_IMPL(Rasterizer, Object, IRasterizer);
Rasterizer::~Rasterizer()
{
    Finalizer(mNativeInstance);
}

void Rasterizer::Finalizer(
    /* [in] */ Int64 nativeInstance)
{
    delete reinterpret_cast<NativeRasterizer *>(nativeInstance);
}

Int64 CLayerRasterizer::NativeConstructor()
{
    return reinterpret_cast<Int64>(new NativeLayerRasterizer);
}

void CLayerRasterizer::NativeAddLayer(
    /* [in] */ Int64 layerHandle,
    /* [in] */ Int64 paintHandle,
    /* [in] */ Float dx,
    /* [in] */ Float dy)
{
    NativeLayerRasterizer* nr = reinterpret_cast<NativeLayerRasterizer *>(layerHandle);
    const NativePaint* paint = reinterpret_cast<NativePaint *>(paintHandle);
    SkASSERT(nr);
    SkASSERT(paint);
    nr->fBuilder.addLayer(*paint, dx, dy);
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
