
#include "elastos/droid/view/CMagnificationSpec.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace View {

const Int32 CMagnificationSpec::MAX_POOL_SIZE = 20;
AutoPtr<Pools::SynchronizedPool<IMagnificationSpec> > CMagnificationSpec::sPool =
        new Pools::SynchronizedPool<IMagnificationSpec>(MAX_POOL_SIZE);

CAR_OBJECT_IMPL(CMagnificationSpec);
CAR_INTERFACE_IMPL_2(CMagnificationSpec, Object, IMagnificationSpec, IParcelable);
CMagnificationSpec::CMagnificationSpec()
    : mScale(1.0f)
    , mOffsetX(0)
    , mOffsetY(0)
{
    /* do nothing - reducing visibility */
}

ECode CMagnificationSpec::constructor()
{
    return NOERROR;
}

ECode CMagnificationSpec::Initialize(
    /* [in] */ Float scale,
    /* [in] */ Float offsetX,
    /* [in] */ Float offsetY)
{
    if (scale < 1) {
        // throw new IllegalArgumentException("Scale must be greater than or equal to one!");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mScale = scale;
    mOffsetX = offsetX;
    mOffsetY = offsetY;
    return NOERROR;
}

ECode CMagnificationSpec::IsNop(
    /* [out] */ Boolean* nop)
{
    VALIDATE_NOT_NULL(nop);
    *nop = mScale == 1.0f && mOffsetX == 0 && mOffsetY == 0;
    return NOERROR;
}

AutoPtr<IMagnificationSpec> CMagnificationSpec::Obtain(
    /* [in] */ IMagnificationSpec* _other)
{
    AutoPtr<IMagnificationSpec> info = Obtain();
    CMagnificationSpec* other = (CMagnificationSpec*)_other;
    ((CMagnificationSpec*)info.Get())->mScale = other->mScale;
    ((CMagnificationSpec*)info.Get())->mOffsetX = other->mOffsetX;
    ((CMagnificationSpec*)info.Get())->mOffsetY = other->mOffsetY;
    return info;
}

AutoPtr<IMagnificationSpec> CMagnificationSpec::Obtain()
{
    AutoPtr<IMagnificationSpec> spec = sPool->AcquireItem();
    if (spec != NULL) {
        return spec;
    }

    CMagnificationSpec::New((IMagnificationSpec**)&spec);
    return spec;
}

ECode CMagnificationSpec::Recycle()
{
    Clear();
    sPool->ReleaseItem(this);
    return NOERROR;
}

ECode CMagnificationSpec::Clear()
{
   mScale = 1.0f;
   mOffsetX = 0.0f;
   mOffsetY = 0.0f;
   return NOERROR;
}

ECode CMagnificationSpec::WriteToParcel(
    /* [in] */ IParcel* parcel)
{
    parcel->WriteFloat(mScale);
    parcel->WriteFloat(mOffsetX);
    parcel->WriteFloat(mOffsetY);
    return Recycle();
}

ECode CMagnificationSpec::ReadFromParcel(
        /* [in] */ IParcel* source)
{
    source->ReadFloat(&mScale);
    source->ReadFloat(&mOffsetX);
    source->ReadFloat(&mOffsetY);
    return NOERROR;
}

ECode CMagnificationSpec::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    StringBuilder builder;
    builder.Append("<scale:");
    builder.Append(mScale);
    builder.Append(",offsetX:");
    builder.Append(mOffsetX);
    builder.Append(",offsetY:");
    builder.Append(mOffsetY);
    builder.Append(">");
    return builder.ToString(str);
}

ECode CMagnificationSpec::SetScale(
    /* [in] */ Float value)
{
    mScale = value;
    return NOERROR;
}

ECode CMagnificationSpec::GetScale(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mScale;
    return NOERROR;
}

ECode CMagnificationSpec::SetOffsetX(
    /* [in] */ Float value)
{
    mOffsetX = value;
    return NOERROR;
}

ECode CMagnificationSpec::GetOffsetX(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mOffsetX;
    return NOERROR;
}

ECode CMagnificationSpec::SetOffsetY(
    /* [in] */ Float value)
{
    mOffsetY = value;
    return NOERROR;
}

ECode CMagnificationSpec::GetOffsetY(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mOffsetY;
    return NOERROR;
}

} // namespace View
} // namepsace Droid
} // namespace Elastos
