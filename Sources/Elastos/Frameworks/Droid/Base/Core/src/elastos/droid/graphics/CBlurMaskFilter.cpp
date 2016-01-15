
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/CBlurMaskFilter.h"
#include <skia/effects/SkBlurMask.h>
#include <skia/effects/SkBlurMaskFilter.h>

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_OBJECT_IMPL(CBlurMaskFilter);
ECode CBlurMaskFilter::constructor(
    /* [in] */ Float radius,
    /* [in] */ Int32 style)
{
    mNativeInstance = NativeConstructor(radius, style);
    return NOERROR;
}

PInterface CBlurMaskFilter::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_MaskFilter) {
        return reinterpret_cast<PInterface>((MaskFilter*)this);
    }
    if (riid == EIID_IBlurMaskFilter) {
        return (IBlurMaskFilter*)this;
    }
    return MaskFilter::Probe(riid);
}

UInt32 CBlurMaskFilter::AddRef()
{
    return MaskFilter::AddRef();
}

UInt32 CBlurMaskFilter::Release()
{
    return MaskFilter::Release();
}

ECode CBlurMaskFilter::GetInterfaceID(
    /* [in] */ IInterface* object,
    /* [out] */ InterfaceID* iid)
{
    return MaskFilter::GetInterfaceID(object, iid);
}

Int64 CBlurMaskFilter::NativeConstructor(
    /* [in] */ Float radius,
    /* [in] */ Int32 blurStyle)
{
    SkScalar sigma = SkBlurMask::ConvertRadiusToSigma(radius);
    SkMaskFilter* filter = SkBlurMaskFilter::Create((SkBlurStyle)blurStyle, sigma);
    assert(filter);
    return reinterpret_cast<Int64>(filter);
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
