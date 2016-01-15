
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/CDiscretePathEffect.h"
#include <skia/effects/SkDiscretePathEffect.h>

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_OBJECT_IMPL(CDiscretePathEffect);
ECode CDiscretePathEffect::constructor(
    /* [in] */ Float segmentLength,
    /* [in] */ Float deviation)
{
    mNativeInstance = NativeCreate(
                        segmentLength,
                        deviation);
    return NOERROR;
}

PInterface CDiscretePathEffect::Probe(
    /* [in]  */ REIID riid)
{
    if (riid == EIID_PathEffect) {
        return reinterpret_cast<PInterface>((PathEffect*)this);
    }
    else if (riid == EIID_IDiscretePathEffect) {
        return (IDiscretePathEffect*)this;
    }
    return PathEffect::Probe(riid);
}

UInt32 CDiscretePathEffect::AddRef()
{
    return PathEffect::AddRef();
}

UInt32 CDiscretePathEffect::Release()
{
    return PathEffect::Release();
}

ECode CDiscretePathEffect::GetInterfaceID(
    /* [in] */ IInterface* object,
    /* [out] */ InterfaceID* iid)
{
    return PathEffect::GetInterfaceID(object, iid);
}

Int64 CDiscretePathEffect::NativeCreate(
    /* [in] */ Float length,
    /* [in] */ Float deviation)
{
    SkPathEffect* effect = SkDiscretePathEffect::Create(length, deviation);
    return reinterpret_cast<Int64>(effect);
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
