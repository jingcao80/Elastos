
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/CPathDashPathEffect.h"
#include "elastos/droid/graphics/CPath.h"
#include <skia/effects/Sk1DPathEffect.h>
#include <skia/core/SkPath.h>

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_OBJECT_IMPL(CPathDashPathEffect);
ECode CPathDashPathEffect::constructor(
    /* [in] */ IPath* shape,
    /* [in] */ Float advance,
    /* [in] */ Float phase,
    /* [in] */ PathDashPathEffectStyle style)
{
    mNativeInstance = NativeCreate(
                        ((CPath*)shape)->Ni(),
                        advance,
                        phase,
                        style);
    return NOERROR;
}

PInterface CPathDashPathEffect::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_PathEffect) {
        return reinterpret_cast<PInterface>((PathEffect*)this);
    }
    else if (riid == EIID_IPathDashPathEffect) {
        return (IPathDashPathEffect*)this;
    }
    return PathEffect::Probe(riid);
}

UInt32 CPathDashPathEffect::AddRef()
{
    return PathEffect::AddRef();
}

UInt32 CPathDashPathEffect::Release()
{
    return PathEffect::Release();
}

ECode CPathDashPathEffect::GetInterfaceID(
    /* [in] */ IInterface* object,
    /* [out] */ InterfaceID* iid)
{
    return PathEffect::GetInterfaceID(object, iid);
}

Int64 CPathDashPathEffect::NativeCreate(
    /* [in] */ Int64 nativePath,
    /* [in] */ Float advance,
    /* [in] */ Float phase,
    /* [in] */ Int32 nativeStyle)
{
    const SkPath* shape = reinterpret_cast<SkPath*>(nativePath);
    SkASSERT(shape != NULL);
    SkPathEffect* effect = SkPath1DPathEffect::Create(*shape, advance, phase,
            (SkPath1DPathEffect::Style)nativeStyle);
    return reinterpret_cast<Int64>(effect);
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
