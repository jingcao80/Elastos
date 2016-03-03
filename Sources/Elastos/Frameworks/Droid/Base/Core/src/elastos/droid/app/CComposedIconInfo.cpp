
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/app/CComposedIconInfo.h"

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL(CComposedIconInfo, Object, IComposedIconInfo)

CAR_OBJECT_IMPL(CComposedIconInfo)

CComposedIconInfo::CComposedIconInfo()
    : mIconUpon(0)
    , mIconMask(0)
    , mIconScale(0)
    , mIconDensity(0)
    , mIconSize(0)
{}

ECode CComposedIconInfo::constructor()
{
    return NOERROR;
}

ECode CComposedIconInfo::GetIconUpon(
    /* [out] */ Int32* iconUpon)
{
    VALIDATE_NOT_NULL(iconUpon);
    *iconUpon = mIconUpon;
    return NOERROR;
}

ECode CComposedIconInfo::SetIconUpon(
    /* [in] */ Int32 iconUpon)
{
    mIconUpon = iconUpon;
    return NOERROR;
}

ECode CComposedIconInfo::GetIconMask(
    /* [out] */ Int32* iconMask)
{
    VALIDATE_NOT_NULL(iconMask);
    *iconMask = mIconMask;
    return NOERROR;
}

ECode CComposedIconInfo::SetIconMask(
    /* [in] */ Int32 iconMask)
{
    mIconMask = iconMask;
    return NOERROR;
}

ECode CComposedIconInfo::GetIconBacks(
    /* [out, callee] */ ArrayOf<Int32>** iconBacks)
{
    VALIDATE_NOT_NULL(iconBacks);
    *iconBacks = mIconBacks;
    REFCOUNT_ADD(*iconBacks);
    return NOERROR;
}

ECode CComposedIconInfo::SetIconBacks(
    /* [in] */ ArrayOf<Int32>* iconBacks)
{
    mIconBacks = iconBacks;
    return NOERROR;
}

ECode CComposedIconInfo::GetIconScale(
    /* [out] */ Float* iconScale)
{
    VALIDATE_NOT_NULL(iconScale);
    *iconScale = mIconScale;
    return NOERROR;
}

ECode CComposedIconInfo::SetIconScale(
    /* [in] */ Float iconScale)
{
    mIconScale = iconScale;
    return NOERROR;
}

ECode CComposedIconInfo::GetIconDensity(
    /* [out] */ Int32* iconDensity)
{
    VALIDATE_NOT_NULL(iconDensity);
    *iconDensity = mIconDensity;
    return NOERROR;
}

ECode CComposedIconInfo::SetIconDensity(
    /* [in] */ Int32 iconDensity)
{
    mIconDensity = iconDensity;
    return NOERROR;
}

ECode CComposedIconInfo::GetIconSize(
    /* [out] */ Int32* iconSize)
{
    VALIDATE_NOT_NULL(iconSize);
    *iconSize = mIconSize;
    return NOERROR;
}

ECode CComposedIconInfo::SetIconSize(
    /* [in] */ Int32 iconSize)
{
    mIconSize = iconSize;
    return NOERROR;
}

ECode CComposedIconInfo::GetColorFilter(
    /* [out, callee] */ ArrayOf<Float>** colorFilter)
{
    VALIDATE_NOT_NULL(colorFilter);
    *colorFilter = mColorFilter;
    REFCOUNT_ADD(*colorFilter);
    return NOERROR;
}

ECode CComposedIconInfo::SetColorFilter(
    /* [in] */ ArrayOf<Float>* colorFilter)
{
    mColorFilter = colorFilter;
    return NOERROR;
}

ECode CComposedIconInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadFloat(&mIconScale);
    source->ReadInt32(&mIconDensity);
    source->ReadInt32(&mIconSize);
    Int32 backCount;
    source->ReadInt32(&backCount);
    if (backCount > 0) {
        mIconBacks = ArrayOf<Int32>::Alloc(backCount);
        for (Int32 i = 0; i < backCount; i++) {
            Int32 val;
            source->ReadInt32(&val);
            (*mIconBacks)[i] = val;
        }
    }
    source->ReadInt32(&mIconMask);
    source->ReadInt32(&mIconUpon);
    Int32 colorFilterSize;
    source->ReadInt32(&colorFilterSize);
    if (colorFilterSize > 0) {
        mColorFilter = ArrayOf<Float>::Alloc(colorFilterSize);
        for (Int32 i = 0; i < colorFilterSize; i++) {
            Float val;
            source->ReadFloat(&val);
            (*mColorFilter)[i] = val;
        }
    }
    return NOERROR;
}

ECode CComposedIconInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteFloat(mIconScale);
    dest->WriteInt32(mIconDensity);
    dest->WriteInt32(mIconSize);
    dest->WriteInt32(mIconBacks != NULL ? mIconBacks->GetLength() : 0);
    if (mIconBacks != NULL) {
        for (Int32 i = 0; i < mIconBacks->GetLength(); i++) {
            Int32 resId = (*mIconBacks)[i];
            dest->WriteInt32(resId);
        }
    }
    dest->WriteInt32(mIconMask);
    dest->WriteInt32(mIconUpon);
    if (mColorFilter != NULL) {
        dest->WriteInt32(mColorFilter->GetLength());
        for (Int32 i = 0; i < mColorFilter->GetLength(); i++) {
            Float val = (*mColorFilter)[i];
            dest->WriteFloat(val);
        }
    }
    else {
        dest->WriteInt32(0);
    }
    return NOERROR;
}

} // namespace App
} // namespace Droid
} // namespace Elastos
