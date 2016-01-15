
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/CLayerRasterizer.h"
#include "elastos/droid/graphics/Paint.h"

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_OBJECT_IMPL(CLayerRasterizer);
ECode CLayerRasterizer::constructor()
{
    mNativeInstance = NativeConstructor();
    return NOERROR;
}

PInterface CLayerRasterizer::Probe(
    /* [in]  */ REIID riid)
{
    if (riid == EIID_Rasterizer) {
        return reinterpret_cast<PInterface>((Rasterizer*)this);
    }
    else if (riid == EIID_ILayerRasterizer) {
        return (ILayerRasterizer*)this;
    }
    return Rasterizer::Probe(riid);
}

UInt32 CLayerRasterizer::AddRef()
{
    return Rasterizer::AddRef();
}

UInt32 CLayerRasterizer::Release()
{
    return Rasterizer::Release();
}

ECode CLayerRasterizer::GetInterfaceID(
    /* [in] */ IInterface* object,
    /* [out] */ InterfaceID* iid)
{
    return Rasterizer::GetInterfaceID(object, iid);
}

ECode CLayerRasterizer::AddLayer(
    /* [in] */ IPaint* paint,
    /* [in] */ Float dx,
    /* [in] */ Float dy)
{
    NativeAddLayer(mNativeInstance, ((Paint*)(IPaint*)paint->Probe(EIID_Paint))->mNativePaint, dx, dy);
    return NOERROR;
}

ECode CLayerRasterizer::AddLayer(
    /* [in] */ IPaint* paint)
{
    NativeAddLayer(mNativeInstance, ((Paint*)(IPaint*)paint->Probe(EIID_Paint))->mNativePaint, 0, 0);
    return NOERROR;
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
