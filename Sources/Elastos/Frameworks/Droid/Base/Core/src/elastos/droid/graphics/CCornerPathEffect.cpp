
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/CCornerPathEffect.h"
#include <skia/effects/SkCornerPathEffect.h>

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_OBJECT_IMPL(CCornerPathEffect);
ECode CCornerPathEffect::constructor(
    /* [in] */ Float radius)
{
    mNativeInstance = NativeCreate(radius);
    return NOERROR;
}

PInterface CCornerPathEffect::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_PathEffect) {
        return reinterpret_cast<PInterface>((PathEffect*)this);
    }
    else if (riid == EIID_ICornerPathEffect) {
        return (ICornerPathEffect*)this;
    }
    return PathEffect::Probe(riid);
}

UInt32 CCornerPathEffect::AddRef()
{
    return PathEffect::AddRef();
}

UInt32 CCornerPathEffect::Release()
{
    return PathEffect::Release();
}

ECode CCornerPathEffect::GetInterfaceID(
    /* [in] */ IInterface* object,
    /* [out] */ InterfaceID* iid)
{
    return PathEffect::GetInterfaceID(object, iid);
}

Int64 CCornerPathEffect::NativeCreate(
    /* [in] */ Float radius)
{
    SkPathEffect* effect = SkCornerPathEffect::Create(radius);
    return reinterpret_cast<Int64>(effect);
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

