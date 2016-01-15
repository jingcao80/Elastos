
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/CSumPathEffect.h"
#include <SkPathEffect.h>

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_OBJECT_IMPL(CSumPathEffect);
ECode CSumPathEffect::constructor(
    /* [in] */ IPathEffect* first,
    /* [in] */ IPathEffect* second)
{
    mNativeInstance = NativeCreate((
            (PathEffect*)(IPathEffect*)first->Probe(EIID_PathEffect))->mNativeInstance,
            ((PathEffect*)(IPathEffect*)second->Probe(EIID_PathEffect))->mNativeInstance);
    return NOERROR;
}

PInterface CSumPathEffect::Probe(
    /* [in]  */ REIID riid)
{
    if (riid == EIID_PathEffect) {
        return reinterpret_cast<PInterface>((PathEffect*)this);
    }
    else if (riid == EIID_ISumPathEffect) {
        return (ISumPathEffect*)this;
    }
    return PathEffect::Probe(riid);
}

UInt32 CSumPathEffect::AddRef()
{
    return PathEffect::AddRef();
}

UInt32 CSumPathEffect::Release()
{
    return PathEffect::Release();
}

ECode CSumPathEffect::GetInterfaceID(
    /* [in] */ IInterface* object,
    /* [out] */ InterfaceID* iid)
{
    return PathEffect::GetInterfaceID(object, iid);
}

Int64 CSumPathEffect::NativeCreate(
    /* [in] */ Int64 firstHandle,
    /* [in] */ Int64 secondHandle)
{
    SkPathEffect* first = reinterpret_cast<SkPathEffect*>(firstHandle);
    SkPathEffect* second = reinterpret_cast<SkPathEffect*>(secondHandle);
    SkPathEffect* effect = SkSumPathEffect::Create(first, second);
    return reinterpret_cast<Int64>(effect);
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
