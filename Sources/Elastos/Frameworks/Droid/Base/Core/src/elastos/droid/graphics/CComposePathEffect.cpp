
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/CComposePathEffect.h"
#include <SkPathEffect.h>

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_OBJECT_IMPL(CComposePathEffect);
ECode CComposePathEffect::constructor(
    /* [in] */ IPathEffect* outerpe,
    /* [in] */ IPathEffect* innerpe)
{
    mNativeInstance = NativeCreate(
            ((PathEffect*)(IPathEffect*)outerpe->Probe(EIID_PathEffect))->mNativeInstance,
            ((PathEffect*)(IPathEffect*)innerpe->Probe(EIID_PathEffect))->mNativeInstance);
    return NOERROR;
}

PInterface CComposePathEffect::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_PathEffect) {
        return reinterpret_cast<PInterface>((PathEffect*)this);
    }
    else if (riid == EIID_IComposePathEffect) {
        return (IComposePathEffect*)this;
    }

    return PathEffect::Probe(riid);
}

UInt32 CComposePathEffect::AddRef()
{
    return PathEffect::AddRef();
}

UInt32 CComposePathEffect::Release()
{
    return PathEffect::Release();
}

ECode CComposePathEffect::GetInterfaceID(
    /* [in] */ IInterface* object,
    /* [out] */ InterfaceID* iid)
{
    return PathEffect::GetInterfaceID(object, iid);
}

Int64 CComposePathEffect::NativeCreate(
    /* [in] */ Int64 outerHandle,
    /* [in] */ Int64 innerHandle)
{
    SkPathEffect* outer = reinterpret_cast<SkPathEffect*>(outerHandle);
    SkPathEffect* inner = reinterpret_cast<SkPathEffect*>(innerHandle);
    SkPathEffect* effect = SkComposePathEffect::Create(outer, inner);
    return reinterpret_cast<Int64>(effect);
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
