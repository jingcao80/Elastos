
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/CLightingColorFilter.h"
#include <skia/core/SkColorFilter.h>

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_OBJECT_IMPL(CLightingColorFilter);

CLightingColorFilter::CLightingColorFilter()
    : mMul(0)
    , mAdd(0)
{
}

ECode CLightingColorFilter::constructor(
    /* [in] */ Int32 mul,
    /* [in] */ Int32 add)
{
    mMul = mul;
    mAdd = add;
    Update();
    return NOERROR;
}

PInterface CLightingColorFilter::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_ColorFilter) {
        return reinterpret_cast<PInterface>((ColorFilter*)this);
    }
    else if (riid == EIID_ILightingColorFilter) {
        return (ILightingColorFilter*)this;
    }
    return ColorFilter::Probe(riid);
}

UInt32 CLightingColorFilter::AddRef()
{
    return ColorFilter::AddRef();
}

UInt32 CLightingColorFilter::Release()
{
    return ColorFilter::Release();
}

ECode CLightingColorFilter::GetInterfaceID(
    /* [in] */ IInterface* object,
    /* [out] */ InterfaceID* iid)
{
    return ColorFilter::GetInterfaceID(object, iid);
}

ECode CLightingColorFilter::GetColorMultiply(
    /* [out] */ Int32* colorMultiply)
{
    VALIDATE_NOT_NULL(colorMultiply);
    *colorMultiply = mMul;
    return NOERROR;
}

ECode CLightingColorFilter::SetColorMultiply(
    /* [in] */ Int32 mul)
{
    mMul = mul;
    Update();
    return NOERROR;
}

ECode CLightingColorFilter::GetColorAdd(
    /* [out] */ Int32* color)
{
    VALIDATE_NOT_NULL(color);
    *color = mAdd;
    return NOERROR;
}

ECode CLightingColorFilter::SetColorAdd(
    /* [in] */ Int32 add)
{
    mAdd = add;
    Update();
    return NOERROR;
}

void CLightingColorFilter::Update()
{
    DestroyFilter(mNativeInstance);
    mNativeInstance = NativeCreateLightingFilter(mMul, mAdd);
}

Int64 CLightingColorFilter::NativeCreateLightingFilter(
    /* [in] */ Int32 mul,
    /* [in] */ Int32 add)
{
    return reinterpret_cast<Int64>(SkColorFilter::CreateLightingFilter(mul, add));
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
